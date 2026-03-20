#include <algorithm>
#include <atomic>
#include <csignal>
#include <cstring>

#include <gflags/gflags.h>

#include "rdma_chain.hpp"
#include "util.hpp"

namespace {

std::atomic<bool> stop_flag = false;

void SignalHandler(int) {
    stop_flag.store(true);
}

class ExampleChainHandler : public rdma_chain::PacketHandler {
public:
    ExampleChainHandler(int threads, size_t packet_size)
        : expected_replies_(threads), payload_size_(std::min<size_t>(packet_size, 64)) {
        rt_assert(packet_size >= 8, "packet_size must be at least 8 bytes for the example handler");
    }

    void OnThreadStart(rdma_chain::ThreadContext &ctx) override {
        if (ctx.host_id() == 1) {
            SendPacket(ctx, rdma_chain::PeerType::kNext, 0x01, nullptr, 0);
        }
    }

    void OnPacket(rdma_chain::ThreadContext &ctx, rdma_chain::PeerType from,
                  const rdma_chain::PacketView &packet) override {
        if (packet.length == 0) {
            return;
        }

        switch (packet.data[0]) {
        case 0x01:
            if (ctx.host_id() == 2 && from == rdma_chain::PeerType::kPrev) {
                SendPacket(ctx, rdma_chain::PeerType::kNext, 0x02, packet.data, packet.length);
            }
            break;
        case 0x02:
            if (ctx.host_id() == 3 && from == rdma_chain::PeerType::kPrev) {
                SendPacket(ctx, rdma_chain::PeerType::kPrev, 0x03, packet.data, packet.length);
            }
            break;
        case 0x03:
            if (ctx.host_id() == 2 && from == rdma_chain::PeerType::kNext) {
                SendPacket(ctx, rdma_chain::PeerType::kPrev, 0x04, packet.data, packet.length);
            }
            break;
        case 0x04:
            if (ctx.host_id() == 1 && from == rdma_chain::PeerType::kNext) {
                const int done = replies_.fetch_add(1) + 1;
                LOG_I("host1 thread[%d] completed chain, packet type=%u, total replies=%d/%d",
                      ctx.thread_index(), packet.data[0], done, expected_replies_);
                if (done == expected_replies_) {
                    LOG_I("example round finished on host1, press Ctrl-C on each host to exit or modify the handler for your own workflow");
                }
            }
            break;
        default:
            LOG_I("host%d thread[%d] ignored packet type=%u from %s",
                  ctx.host_id(), ctx.thread_index(), packet.data[0], rdma_chain::PeerTypeString(from));
            break;
        }
    }

private:
    void SendPacket(rdma_chain::ThreadContext &ctx, rdma_chain::PeerType peer, uint8_t type,
                    const uint8_t *src, size_t src_len) {
        rdma_chain::SendBuffer send = ctx.AcquireSendBuffer(peer);
        if (!send.valid()) {
            return;
        }

        size_t length = payload_size_;
        if (src != nullptr && src_len != 0) {
            length = std::min<size_t>(send.capacity(), src_len);
            std::memcpy(send.data(), src, length);
        } else {
            length = std::min<size_t>(send.capacity(), payload_size_);
            std::memset(send.data(), 0, length);
        }

        send.data()[0] = type;
        send.data()[1] = static_cast<uint8_t>(ctx.host_id());
        send.data()[2] = static_cast<uint8_t>(ctx.thread_index());
        send.Commit(static_cast<uint32_t>(length));
    }

    std::atomic<int> replies_{0};
    int expected_replies_ = 0;
    size_t payload_size_ = 0;
};

DEFINE_int32(host_id, 1, "host id: 1(host1), 2(host2), 3(host3)");
DEFINE_string(control_ip, "", "host1 control-plane ip, required on host2/host3");
DEFINE_int32(threads, 1, "thread count on each host");
DEFINE_string(device, "mlx5_0", "RDMA device name");
DEFINE_int32(gid_index, 3, "RoCE gid index");
DEFINE_int32(port, 6666, "control-plane TCP port");
DEFINE_int32(numa_node, 0, "NUMA node for memory and CPU binding");
DEFINE_int32(core_offset, 0, "CPU offset inside the NUMA node");
DEFINE_int32(packet_size, 2048, "max raw packet size in bytes");
DEFINE_int32(queue_depth, 128, "send/recv ring slots per QP");

}  // namespace

int main(int argc, char **argv) {
    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);
    gflags::SetUsageMessage("Three-host chain RDMA send/recv example. host1 is the control-plane server; users usually only edit ExampleChainHandler.");
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    rdma_chain::ChainConfig config;
    config.host_id = FLAGS_host_id;
    config.control_ip = FLAGS_control_ip;
    config.threads = FLAGS_threads;
    config.device_name = FLAGS_device;
    config.gid_index = FLAGS_gid_index;
    config.port = FLAGS_port;
    config.numa_node = FLAGS_numa_node;
    config.core_offset = FLAGS_core_offset;
    config.packet_size = FLAGS_packet_size;
    config.queue_depth = FLAGS_queue_depth;
    config.stop_flag = &stop_flag;

    ExampleChainHandler handler(FLAGS_threads, static_cast<size_t>(FLAGS_packet_size));
    return rdma_chain::RunChain(config, handler);
}
