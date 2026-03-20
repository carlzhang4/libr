#ifndef __RDMA_CHAIN_HPP__
#define __RDMA_CHAIN_HPP__

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <string>

namespace rdma_chain {

enum class PeerType {
    kPrev = 0,
    kNext = 1,
};

struct ChainConfig {
    int host_id = 1;
    std::string control_ip;
    int threads = 1;
    std::string device_name = "mlx5_0";
    int gid_index = 3;
    int port = 6666;
    int numa_node = 0;
    int core_offset = 0;
    int packet_size = 2048;
    int queue_depth = 128;
    bool use_devx_context = false;
    std::atomic<bool> *stop_flag = nullptr;
};

struct PacketView {
    const uint8_t *data = nullptr;
    uint32_t length = 0;
};

struct LinkState;
struct ThreadRuntime;

class ThreadContext;

class SendBuffer {
public:
    SendBuffer() = default;
    ~SendBuffer();

    SendBuffer(const SendBuffer &) = delete;
    SendBuffer &operator=(const SendBuffer &) = delete;

    SendBuffer(SendBuffer &&other) noexcept;
    SendBuffer &operator=(SendBuffer &&other) noexcept;

    uint8_t *data() const { return data_; }
    size_t capacity() const { return capacity_; }
    bool valid() const { return data_ != nullptr; }

    void Commit(uint32_t length);

private:
    friend class ThreadContext;

    SendBuffer(ThreadContext *ctx, PeerType peer, LinkState *link, size_t slot_index,
               uint8_t *data, size_t capacity);

    void Reset() noexcept;

    ThreadContext *ctx_ = nullptr;
    PeerType peer_ = PeerType::kPrev;
    LinkState *link_ = nullptr;
    size_t slot_index_ = 0;
    uint8_t *data_ = nullptr;
    size_t capacity_ = 0;
    bool committed_ = false;
};

class PacketHandler {
public:
    virtual ~PacketHandler() = default;

    virtual void OnThreadStart(ThreadContext &ctx) {
        (void)ctx;
    }

    virtual void OnPacket(ThreadContext &ctx, PeerType from, const PacketView &packet) = 0;

    virtual void OnIdle(ThreadContext &ctx) {
        (void)ctx;
    }
};

class ThreadContext {
public:
    ThreadContext() = default;
    explicit ThreadContext(ThreadRuntime *runtime);

    int host_id() const;
    int thread_index() const;
    bool has_peer(PeerType peer) const;

    SendBuffer AcquireSendBuffer(PeerType peer);
    void Stop();

private:
    friend class SendBuffer;

    LinkState *GetLink(PeerType peer) const;

    ThreadRuntime *runtime_ = nullptr;
};

int RunChain(const ChainConfig &config, PacketHandler &handler);

const char *PeerTypeString(PeerType peer);

}  // namespace rdma_chain

#endif
