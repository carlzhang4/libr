#include "rdma_chain.hpp"

#include <algorithm>
#include <array>
#include <exception>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "libr.hpp"

namespace rdma_chain {

struct LinkState {
    QpHandler *qp = nullptr;
    void *buffer = nullptr;
    size_t buffer_size = 0;
    size_t packet_size = 0;
    size_t slot_count = 0;
    size_t send_region_offset = 0;
    size_t recv_region_offset = 0;
    size_t send_reserved = 0;
    size_t send_completed = 0;
    std::array<ibv_wc, CTX_POLL_BATCH> send_wc{};
    std::array<ibv_wc, CTX_POLL_BATCH> recv_wc{};
};

struct ThreadRuntime {
    int host_id = 0;
    int thread_index = 0;
    int expected_cpu = -1;
    std::atomic<bool> *stop_flag = nullptr;
    PacketHandler *handler = nullptr;
    LinkState *prev = nullptr;
    LinkState *next = nullptr;
};

namespace {

constexpr int kNumHosts = 3;
constexpr size_t kHugePageSize = 2 * 1024 * 1024;

int ToNodeId(int host_id) {
    return host_id - 1;
}

bool HasPrevLink(int host_id) {
    return host_id > 1;
}

bool HasNextLink(int host_id) {
    return host_id < 3;
}

int PrevNodeId(int host_id) {
    return ToNodeId(host_id) - 1;
}

int NextNodeId(int host_id) {
    return ToNodeId(host_id) + 1;
}

struct ThreadBootstrap {
    int has_prev = 0;
    PingPongInfo prev{};
    int has_next = 0;
    PingPongInfo next{};
};

size_t MaxSendInflight(const LinkState &link) {
    return std::min(link.slot_count, static_cast<size_t>(link.qp->tx_depth));
}

bool DrainSendCompletions(LinkState *link) {
    if (link == nullptr) {
        return false;
    }
    int ne = poll_send_cq(*link->qp, link->send_wc.data());
    rt_assert(ne >= 0, "poll_send_cq failed");
    for (int i = 0; i < ne; ++i) {
        rt_assert(link->send_wc[i].status == IBV_WC_SUCCESS,
                  string_format("send completion failed, status=%d", link->send_wc[i].status));
        ++link->send_completed;
    }
    return ne > 0;
}

void DestroyLinkResources(LinkState *link) {
    if (link == nullptr) {
        return;
    }
    if (link->qp != nullptr) {
        free(link->qp->send_sge_list);
        free(link->qp->recv_sge_list);
        free(link->qp->send_wr);
        free(link->qp->recv_wr);
        free(link->qp->send_bar_wr);
        free(link->qp->recv_bar_wr);

        ibv_destroy_qp(link->qp->qp);
        ibv_dereg_mr(link->qp->mr);
        ibv_destroy_cq(link->qp->send_cq);
        ibv_destroy_cq(link->qp->recv_cq);
        ibv_dealloc_pd(link->qp->pd);
        free(link->qp);
        link->qp = nullptr;
    }
    if (link->buffer != nullptr) {
        munmap(link->buffer, link->buffer_size);
        link->buffer = nullptr;
    }
}

class ChainRunner {
public:
    ChainRunner(const ChainConfig &config, PacketHandler &handler)
        : config_(config), handler_(handler),
          stop_flag_(config.stop_flag != nullptr ? config.stop_flag : &owned_stop_) {
    }

    int Run() {
        try {
            ValidateConfig();
            InitNetParam();
            socket_init(net_param_);
            roce_init(net_param_, config_.threads);
            BuildLocalLinks();
            ExchangeBootstrap();
            ConnectLinks();
            StartWorkers();
            WaitWorkers();
            Cleanup();
        } catch (...) {
            stop_flag_->store(true);
            Cleanup();
            throw;
        }
        return 0;
    }

private:
    struct ThreadLinks {
        std::unique_ptr<LinkState> prev;
        std::unique_ptr<LinkState> next;
    };

    void ValidateConfig() const {
        rt_assert(config_.host_id >= 1 && config_.host_id <= 3, "host_id must be 1, 2 or 3");
        rt_assert(config_.threads > 0, "threads must be positive");
        rt_assert(config_.packet_size > 0, "packet_size must be positive");
        rt_assert(config_.queue_depth > 0, "queue_depth must be positive");
        if (config_.host_id != 1) {
            rt_assert(!config_.control_ip.empty(), "control_ip is required on host2 and host3");
        }
    }

    void InitNetParam() {
        net_param_.numNodes = kNumHosts;
        net_param_.nodeId = ToNodeId(config_.host_id);
        net_param_.serverIp = config_.control_ip;
        net_param_.device_name = config_.device_name;
        net_param_.gid_index = config_.gid_index;
        net_param_.numa_node = config_.numa_node;
        net_param_.batch_size = 1;
        net_param_.sge_per_wr = 1;
        net_param_.sock_port = config_.port;
        net_param_.use_devx_context = config_.use_devx_context;
        init_net_param(net_param_);

        if (net_param_.nodeId == 0) {
            for (int i = 0; i < net_param_.numNodes; ++i) {
                net_param_.sockfd[i] = -1;
            }
        } else {
            net_param_.sockfd[0] = -1;
        }
    }

    size_t RingBytes() const {
        return static_cast<size_t>(config_.packet_size) * static_cast<size_t>(config_.queue_depth);
    }

    size_t BufferBytes() const {
        return std::max(round_up(RingBytes() * 2, kHugePageSize), kHugePageSize);
    }

    std::unique_ptr<LinkState> CreateLink(int thread_index, PingPongInfo *info) {
        auto link = std::make_unique<LinkState>();
        link->packet_size = static_cast<size_t>(config_.packet_size);
        link->slot_count = static_cast<size_t>(config_.queue_depth);
        link->send_region_offset = 0;
        link->recv_region_offset = RingBytes();
        link->buffer_size = BufferBytes();
        link->buffer = malloc_2m_numa(link->buffer_size, net_param_.numa_node);
        rt_assert(link->recv_region_offset + RingBytes() <= link->buffer_size, "ring buffer layout overflow");
        link->qp = create_qp_rc(net_param_, link->buffer, link->buffer_size, info, thread_index);
        return link;
    }

    void BuildLocalLinks() {
        bootstrap_.assign(kNumHosts * config_.threads, ThreadBootstrap{});
        links_.resize(static_cast<size_t>(config_.threads));

        for (int i = 0; i < config_.threads; ++i) {
            if (HasPrevLink(config_.host_id)) {
                bootstrap_[i].has_prev = 1;
                links_[static_cast<size_t>(i)].prev = CreateLink(i, &bootstrap_[i].prev);
            }
            if (HasNextLink(config_.host_id)) {
                bootstrap_[i].has_next = 1;
                links_[static_cast<size_t>(i)].next = CreateLink(i, &bootstrap_[i].next);
            }
        }
    }

    void ExchangeBootstrap() {
        exchange_data(net_param_, reinterpret_cast<char *>(bootstrap_.data()),
                      static_cast<int>(sizeof(ThreadBootstrap) * config_.threads));
    }

    void ConnectLinks() {
        ThreadBootstrap *my_bootstrap = bootstrap_.data() + net_param_.nodeId * config_.threads;
        for (int i = 0; i < config_.threads; ++i) {
            ThreadLinks &thread_links = links_[static_cast<size_t>(i)];
            if (thread_links.prev) {
                ThreadBootstrap *remote = bootstrap_.data() + PrevNodeId(config_.host_id) * config_.threads;
                rt_assert(remote[i].has_next != 0, "missing remote next-link bootstrap info");
                connect_qp_rc(net_param_, *thread_links.prev->qp, &remote[i].next, &my_bootstrap[i].prev);
            }
            if (thread_links.next) {
                ThreadBootstrap *remote = bootstrap_.data() + NextNodeId(config_.host_id) * config_.threads;
                rt_assert(remote[i].has_prev != 0, "missing remote prev-link bootstrap info");
                connect_qp_rc(net_param_, *thread_links.next->qp, &remote[i].prev, &my_bootstrap[i].next);
            }
        }
    }

    void PostInitialReceives(LinkState *link) {
        if (link == nullptr) {
            return;
        }
        const size_t recv_depth = std::min(link->slot_count, static_cast<size_t>(link->qp->rx_depth));
        for (size_t i = 0; i < recv_depth; ++i) {
            const size_t offset = link->recv_region_offset + i * link->packet_size;
            post_recv(*link->qp, offset, static_cast<int>(link->packet_size));
        }
    }

    bool PollReceives(ThreadContext &ctx, LinkState *link, PeerType peer) {
        if (link == nullptr) {
            return false;
        }
        int ne = poll_recv_cq(*link->qp, link->recv_wc.data());
        rt_assert(ne >= 0, "poll_recv_cq failed");
        for (int i = 0; i < ne; ++i) {
            const ibv_wc &wc = link->recv_wc[i];
            rt_assert(wc.status == IBV_WC_SUCCESS,
                      string_format("recv completion failed, status=%d", wc.status));
            rt_assert(wc.byte_len <= link->packet_size,
                      string_format("recv packet too large, byte_len=%u", wc.byte_len));

            const size_t offset = static_cast<size_t>(wc.wr_id);
            const auto *packet = reinterpret_cast<const uint8_t *>(link->buffer) + offset;
            PacketView view{packet, wc.byte_len};
            handler_.OnPacket(ctx, peer, view);

            post_recv(*link->qp, offset, static_cast<int>(link->packet_size));
        }
        return ne > 0;
    }

    void RecordWorkerError(std::exception_ptr error) {
        std::lock_guard<std::mutex> guard(error_lock_);
        if (worker_error_ == nullptr) {
            worker_error_ = std::move(error);
        }
        stop_flag_->store(true);
    }

    void WorkerLoop(ThreadRuntime *runtime) {
        try {
            ThreadContext ctx(runtime);
            wait_scheduling(runtime->expected_cpu, io_lock_);
            PostInitialReceives(runtime->prev);
            PostInitialReceives(runtime->next);

            handler_.OnThreadStart(ctx);
            while (!stop_flag_->load()) {
                bool progressed = false;
                progressed |= DrainSendCompletions(runtime->prev);
                progressed |= DrainSendCompletions(runtime->next);
                progressed |= PollReceives(ctx, runtime->prev, PeerType::kPrev);
                progressed |= PollReceives(ctx, runtime->next, PeerType::kNext);
                if (!progressed) {
                    handler_.OnIdle(ctx);
                    if (!stop_flag_->load()) {
                        std::this_thread::yield();
                    }
                }
            }

            while (DrainSendCompletions(runtime->prev) || DrainSendCompletions(runtime->next)) {
            }
        } catch (...) {
            RecordWorkerError(std::current_exception());
        }
    }

    void StartWorkers() {
        runtimes_.resize(static_cast<size_t>(config_.threads));
        workers_.resize(static_cast<size_t>(config_.threads));
        for (int i = 0; i < config_.threads; ++i) {
            ThreadRuntime &runtime = runtimes_[static_cast<size_t>(i)];
            runtime.host_id = config_.host_id;
            runtime.thread_index = i;
            runtime.expected_cpu = get_cpu_index_with_numa(i + config_.core_offset, config_.numa_node);
            runtime.stop_flag = stop_flag_;
            runtime.handler = &handler_;
            runtime.prev = links_[static_cast<size_t>(i)].prev.get();
            runtime.next = links_[static_cast<size_t>(i)].next.get();
            workers_[static_cast<size_t>(i)] = std::thread(&ChainRunner::WorkerLoop, this, &runtime);
            set_cpu_with_numa(workers_[static_cast<size_t>(i)], i + config_.core_offset, config_.numa_node);
        }
    }

    void WaitWorkers() {
        for (auto &worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        if (worker_error_ != nullptr) {
            std::rethrow_exception(worker_error_);
        }
    }

    void CloseSockets() {
        if (net_param_.sockfd == nullptr) {
            return;
        }
        if (net_param_.nodeId == 0) {
            for (int i = 1; i < net_param_.numNodes; ++i) {
                if (net_param_.sockfd[i] >= 0) {
                    close(net_param_.sockfd[i]);
                    net_param_.sockfd[i] = -1;
                }
            }
            delete[] net_param_.sockfd;
        } else {
            if (net_param_.sockfd[0] >= 0) {
                close(net_param_.sockfd[0]);
                net_param_.sockfd[0] = -1;
            }
            delete net_param_.sockfd;
        }
        net_param_.sockfd = nullptr;
    }

    void CloseContexts() {
        if (net_param_.contexts == nullptr) {
            return;
        }
        for (int i = 0; i < net_param_.num_contexts; ++i) {
            if (net_param_.contexts[i] != nullptr) {
                ibv_close_device(net_param_.contexts[i]);
                net_param_.contexts[i] = nullptr;
            }
        }
        free(net_param_.contexts);
        net_param_.contexts = nullptr;
    }

    void Cleanup() {
        for (auto &worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        for (ThreadLinks &thread_links : links_) {
            DestroyLinkResources(thread_links.prev.get());
            DestroyLinkResources(thread_links.next.get());
            thread_links.prev.reset();
            thread_links.next.reset();
        }
        CloseSockets();
        CloseContexts();
    }

    ChainConfig config_;
    PacketHandler &handler_;
    std::atomic<bool> owned_stop_{false};
    std::atomic<bool> *stop_flag_ = nullptr;
    NetParam net_param_{};
    std::vector<ThreadBootstrap> bootstrap_;
    std::vector<ThreadLinks> links_;
    std::vector<ThreadRuntime> runtimes_;
    std::vector<std::thread> workers_;
    std::mutex io_lock_;
    std::mutex error_lock_;
    std::exception_ptr worker_error_;
};

}  // namespace

SendBuffer::SendBuffer(ThreadContext *ctx, PeerType peer, LinkState *link, size_t slot_index,
                       uint8_t *data, size_t capacity)
    : ctx_(ctx), peer_(peer), link_(link), slot_index_(slot_index), data_(data), capacity_(capacity) {
}

SendBuffer::~SendBuffer() {
    if (valid() && !committed_) {
        LOG_E("SendBuffer to %s destroyed without Commit", PeerTypeString(peer_));
    }
}

SendBuffer::SendBuffer(SendBuffer &&other) noexcept {
    *this = std::move(other);
}

SendBuffer &SendBuffer::operator=(SendBuffer &&other) noexcept {
    if (this != &other) {
        ctx_ = other.ctx_;
        peer_ = other.peer_;
        link_ = other.link_;
        slot_index_ = other.slot_index_;
        data_ = other.data_;
        capacity_ = other.capacity_;
        committed_ = other.committed_;
        other.Reset();
    }
    return *this;
}

void SendBuffer::Commit(uint32_t length) {
    rt_assert(valid(), "invalid SendBuffer");
    rt_assert(!committed_, "SendBuffer already committed");
    rt_assert(length <= capacity_, "send length exceeds buffer capacity");

    const size_t offset = link_->send_region_offset + slot_index_ * link_->packet_size;
    post_send(*link_->qp, offset, static_cast<int>(length));
    committed_ = true;
}

void SendBuffer::Reset() noexcept {
    ctx_ = nullptr;
    link_ = nullptr;
    data_ = nullptr;
    capacity_ = 0;
    committed_ = false;
    slot_index_ = 0;
}

ThreadContext::ThreadContext(ThreadRuntime *runtime) : runtime_(runtime) {
}

int ThreadContext::host_id() const {
    rt_assert(runtime_ != nullptr, "ThreadContext is not initialized");
    return runtime_->host_id;
}

int ThreadContext::thread_index() const {
    rt_assert(runtime_ != nullptr, "ThreadContext is not initialized");
    return runtime_->thread_index;
}

bool ThreadContext::has_peer(PeerType peer) const {
    return GetLink(peer) != nullptr;
}

LinkState *ThreadContext::GetLink(PeerType peer) const {
    rt_assert(runtime_ != nullptr, "ThreadContext is not initialized");
    return peer == PeerType::kPrev ? runtime_->prev : runtime_->next;
}

SendBuffer ThreadContext::AcquireSendBuffer(PeerType peer) {
    LinkState *link = GetLink(peer);
    rt_assert(link != nullptr, string_format("peer %s is not available", PeerTypeString(peer)));

    while (!runtime_->stop_flag->load()) {
        DrainSendCompletions(link);
        if (link->send_reserved - link->send_completed < MaxSendInflight(*link)) {
            const size_t slot_index = link->send_reserved % link->slot_count;
            ++link->send_reserved;
            auto *buffer = reinterpret_cast<uint8_t *>(link->buffer) +
                           link->send_region_offset + slot_index * link->packet_size;
            return SendBuffer(this, peer, link, slot_index, buffer, link->packet_size);
        }
        std::this_thread::yield();
    }

    return SendBuffer();
}

void ThreadContext::Stop() {
    rt_assert(runtime_ != nullptr, "ThreadContext is not initialized");
    runtime_->stop_flag->store(true);
}

int RunChain(const ChainConfig &config, PacketHandler &handler) {
    ChainRunner runner(config, handler);
    return runner.Run();
}

const char *PeerTypeString(PeerType peer) {
    switch (peer) {
    case PeerType::kPrev:
        return "prev";
    case PeerType::kNext:
        return "next";
    }
    return "unknown";
}

}  // namespace rdma_chain
