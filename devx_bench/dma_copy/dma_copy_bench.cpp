#include "dma_copy.h"
#include "bench.h"
using namespace std;
std::mutex IO_LOCK;


constexpr uint32_t QP_DEPTH = 64;

int NUM_THREADS;
string DEVICE_NAME;
int GID_INDEX;
bool IS_READ;
int PAYLOAD;
int BATCH_SIZE;
int NUMA_NODE;
int BIND_OFFSET;
string EXPORTER_IP;
int QP_PER_THREAD;

vhca_resource *resources = nullptr;

volatile double total_bw = 0;
volatile bool start_flag = false;

DEFINE_int32(threads, 1, "num_threads");
DEFINE_string(deviceName, "mlx5_0", "deviceName");
DEFINE_int32(gidIndex, 3, "gidIndex");
DEFINE_bool(isRead, false, "isRead");
DEFINE_int32(payload, 64, "payload");
DEFINE_int32(batchSize, 1, "batchSize");
DEFINE_int32(numaNode, 0, "numaNode");
DEFINE_int32(bindOffset, 0, "bindOffset");
DEFINE_int32(port, 6666, "bind_port");
DEFINE_int32(life_time, 15, "time(s) to live");
DEFINE_string(serverIp, "", "serverIp");
DEFINE_int32(qpPerThread, 1, "qpPerThread");

// sudo ./dma_copy_bench -numaNode 0 -deviceName mlx5_2 -gidIndex 1  -payload 1024 -batchSize 64 -threads 1 -serverIp 10.0.0.100 -life_time 15 -qpPerThread 1

ibv_qp *create_dma_qp(struct ibv_context *ibv_ctx,
    struct ibv_pd *pd, struct ibv_cq *rq_cq, struct ibv_cq *sq_cq) {
    struct ibv_qp_cap qp_cap = {
    .max_send_wr = QP_DEPTH,
    .max_recv_wr = QP_DEPTH,
    .max_send_sge = 1,
    .max_recv_sge = 1,
    .max_inline_data = 64
    };

    struct ibv_qp_init_attr_ex init_attr = {
        .qp_context = NULL,
        .send_cq = sq_cq,
        .recv_cq = rq_cq,
        .cap = qp_cap,
        .qp_type = IBV_QPT_RC,
        .sq_sig_all = 1,

        .comp_mask = IBV_QP_INIT_ATTR_PD | IBV_QP_INIT_ATTR_SEND_OPS_FLAGS,
        .pd = pd,
        .send_ops_flags = IBV_QP_EX_WITH_RDMA_WRITE | IBV_QP_EX_WITH_RDMA_WRITE_WITH_IMM | \
                          IBV_QP_EX_WITH_SEND | IBV_QP_EX_WITH_SEND_WITH_IMM | IBV_QP_EX_WITH_RDMA_READ,
    };

    struct mlx5dv_qp_init_attr attr_dv = {
        .comp_mask = MLX5DV_QP_INIT_ATTR_MASK_SEND_OPS_FLAGS,
        .send_ops_flags = MLX5DV_QP_EX_WITH_MEMCPY,
    };

    struct ibv_qp *qp = mlx5dv_create_qp(ibv_ctx, &init_attr, &attr_dv);
    if (NULL == qp) {
        LOG_E("failed to create qp\n");
        exit(__LINE__);
    }

    struct ibv_qp_attr qpa = {};
    struct ibv_qp_init_attr qpia = {};
    if (ibv_query_qp(qp, &qpa, IBV_QP_CAP, &qpia)) {
        LOG_E("failed to query qp cap\n");
        exit(__LINE__);
    }

    LOG_I("create qp with qpn = 0x%x, max_send_wr = 0x%x, max_recv_wr = 0x%x, "
        "max_send_sge = 0x%x, max_recv_sge = 0x%x, max_inline_data = 0x%x\n",
        qp->qp_num, qpa.cap.max_send_wr, qpa.cap.max_recv_wr, qpa.cap.max_send_sge,
        qpa.cap.max_recv_sge, qpa.cap.max_inline_data);

    return qp;
}

ibv_cq *create_dma_cq(ibv_context *ibv_ctx, int depth_scale) {
    struct ibv_cq_init_attr_ex cq_attr = {
    .cqe = QP_DEPTH * depth_scale,
    .cq_context = NULL,
    .channel = NULL,
    .comp_vector = 0
    };

    struct ibv_cq_ex *cq_ex = mlx5dv_create_cq(ibv_ctx, &cq_attr, NULL);
    if (NULL == cq_ex) {
        LOG_E("failed to create cq\n");
        exit(__LINE__);
    }

    return ibv_cq_ex_to_cq(cq_ex);
}

void init_dma_qp(ibv_qp *qp) {
    int mask = IBV_QP_STATE | IBV_QP_PORT | IBV_QP_PKEY_INDEX | IBV_QP_ACCESS_FLAGS;
    struct ibv_qp_attr attr = {
        .qp_state = IBV_QPS_INIT,
        .qp_access_flags = IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_REMOTE_READ,
        .pkey_index = 0,
        .port_num = 1,
    };

    if (ibv_modify_qp(qp, &attr, mask)) {
        LOG_E("failed to modify qp:0x%x to init\n", qp->qp_num);
        exit(__LINE__);
    }
}

void dma_qp_self_connected(ibv_qp *qp) {
    ibv_gid gid = {};

    if (ibv_query_gid(qp->context, 1, GID_INDEX, &gid)) {
        printf("failed to query port gid\n");
        exit(__LINE__);
    }

    int mask = IBV_QP_STATE | IBV_QP_AV | \
        IBV_QP_PATH_MTU | IBV_QP_DEST_QPN | IBV_QP_RQ_PSN | \
        IBV_QP_MAX_DEST_RD_ATOMIC | IBV_QP_MIN_RNR_TIMER;
    struct ibv_qp_attr qpa = {
        .qp_state = IBV_QPS_RTR,
        .path_mtu = IBV_MTU_1024,
        .rq_psn = 0,
        .dest_qp_num = qp->qp_num,
        .ah_attr = {
            .grh = {
                .dgid = gid,
                .sgid_index = static_cast<uint8_t>(GID_INDEX) ,
                .hop_limit = 64,
            },
            .is_global = 1,
            .port_num = 1,
        },
        .max_dest_rd_atomic = 1,
        .min_rnr_timer = 0x12,
    };

    if (ibv_modify_qp(qp, &qpa, mask)) {
        LOG_E("failed to modify qp:0x%x to rtr, errno 0x%x\n", qp->qp_num, errno);
        exit(__LINE__);
    }

    qpa.qp_state = IBV_QPS_RTS;
    qpa.timeout = 12;
    qpa.retry_cnt = 6;
    qpa.rnr_retry = 0;
    qpa.sq_psn = 0;
    qpa.max_rd_atomic = 1;
    mask = IBV_QP_STATE | IBV_QP_TIMEOUT | IBV_QP_RETRY_CNT | \
        IBV_QP_RNR_RETRY | IBV_QP_SQ_PSN | IBV_QP_MAX_QP_RD_ATOMIC;
    if (ibv_modify_qp(qp, &qpa, mask)) {
        printf("failed to modify qp:0x%x to rts, errno 0x%x\n", qp->qp_num, errno);
        exit(__LINE__);
    }
}

uint32_t get_mmo_dma_max_length(struct ibv_context *ibv_ctx) {
    struct mlx5dv_context attrs_out = {};

    attrs_out.comp_mask = MLX5DV_CONTEXT_MASK_WR_MEMCPY_LENGTH;
    if (mlx5dv_query_device(ibv_ctx, &attrs_out)) {
        printf("failed to query mmo dma max length\n");
        exit(__LINE__);
    }

    if (attrs_out.comp_mask & MLX5DV_CONTEXT_MASK_WR_MEMCPY_LENGTH) {
        return attrs_out.max_wr_memcpy_length;
    }

    return 0;
}


void dma_copy_bench_routine(uint64_t thread_id, bench_runner *runner, bench_stat *stat, void *args) {
    vhca_resource *resource = reinterpret_cast<vhca_resource *>(args) + thread_id;

    void *local_buffer = malloc_2m_numa(PAYLOAD * BATCH_SIZE * QP_PER_THREAD, NUMA_NODE);
    rt_assert(local_buffer);

    ibv_mr *local_mr = ibv_reg_mr(resource->pd, local_buffer, PAYLOAD * BATCH_SIZE * QP_PER_THREAD, IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ
        | IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_HUGETLB | IBV_ACCESS_RELAXED_ORDERING);
    if (!local_mr) {
        LOG_E("can't create local_mr\n");
        exit(__LINE__);
    }

    ibv_cq *sq_cq = create_dma_cq(resource->pd->context, QP_PER_THREAD);
    ibv_cq *rq_cq = create_dma_cq(resource->pd->context, QP_PER_THREAD);

    std::vector<ibv_qp *> dma_qp_list;
    std::vector<ibv_qp_ex *> dma_qpx_list;
    std::vector<mlx5dv_qp_ex *> dma_mqpx_list;
    std::vector<uint64_t> dma_start_index_list;
    std::vector<uint64_t> dma_finish_index_list;

    for (int i = 0;i < QP_PER_THREAD; i++) {
        ibv_qp *dma_qp = create_dma_qp(resource->pd->context, resource->pd, rq_cq, sq_cq);
        init_dma_qp(dma_qp);
        dma_qp_self_connected(dma_qp);
        ibv_qp_ex *dma_qpx = ibv_qp_to_qp_ex(dma_qp);
        mlx5dv_qp_ex *dma_mqpx = mlx5dv_qp_ex_from_ibv_qp_ex(dma_qpx);

        // important for init before do memcpy
        dma_mqpx->wr_memcpy_direct_init(dma_mqpx);

        dma_qp_list.push_back(dma_qp);
        dma_qpx_list.push_back(dma_qpx);
        dma_mqpx_list.push_back(dma_mqpx);
        dma_start_index_list.push_back(0);
        dma_finish_index_list.push_back(0);
    }

    uint32_t local_mr_mkey = local_mr->lkey;
    uint32_t remote_mr_mkey = devx_mr_query_mkey(resource->mr);
    uint64_t per_qp_offset = PAYLOAD * BATCH_SIZE;
    ibv_wc *wc = new ibv_wc[16];

    while (!start_flag) {};

    Timer now_timer;
    now_timer.tic();


    for (uint64_t now_index = 0; now_index < static_cast<uint64_t>(BATCH_SIZE);now_index++) {
        for (uint64_t qp_index = 0;qp_index < static_cast<uint64_t>(QP_PER_THREAD); qp_index++) {
            size_t remote_offset = now_index * PAYLOAD + per_qp_offset * qp_index;
            size_t local_offset = now_index * PAYLOAD + per_qp_offset * qp_index;
            dma_qpx_list[qp_index]->wr_id = dma_start_index_list[qp_index] | (qp_index << 32);
            dma_qpx_list[qp_index]->wr_flags = IBV_SEND_SIGNALED;
            if (IS_READ) {
                dma_mqpx_list[qp_index]->wr_memcpy_direct(dma_mqpx_list[qp_index], local_mr_mkey, (uint64_t)local_buffer + local_offset, remote_mr_mkey, (uint64_t)resource->addr + remote_offset, PAYLOAD);
            } else {
                dma_mqpx_list[qp_index]->wr_memcpy_direct(dma_mqpx_list[qp_index], remote_mr_mkey, (uint64_t)resource->addr + remote_offset, local_mr_mkey, (uint64_t)local_buffer + local_offset, PAYLOAD);
            }
            dma_start_index_list[qp_index]++;
        }
    }

    while (runner->running()) {
        uint32_t num_wc = ibv_poll_cq(sq_cq, 16, wc);
        for (uint32_t i = 0;i < num_wc;i++) {
            uint64_t qp_index = wc[i].wr_id >> 32;
            if (wc[i].status != IBV_WC_SUCCESS || (wc[i].wr_id & 0xffffffff) != dma_finish_index_list[qp_index]) {
                LOG_E("wc status %d, want %ld, get %ld\n", wc[i].status, dma_finish_index_list[qp_index], (wc[i].wr_id & 0xffffffff));
                exit(__LINE__);
            }
            dma_finish_index_list[qp_index]++;

            size_t remote_offset = (dma_start_index_list[qp_index] % BATCH_SIZE) * PAYLOAD + per_qp_offset * qp_index;
            size_t local_offset = (dma_start_index_list[qp_index] % BATCH_SIZE) * PAYLOAD + per_qp_offset * qp_index;
            dma_qpx_list[qp_index]->wr_id = dma_start_index_list[qp_index] | (qp_index << 32);
            dma_qpx_list[qp_index]->wr_flags = IBV_SEND_SIGNALED;
            if (IS_READ) {
                dma_mqpx_list[qp_index]->wr_memcpy_direct(dma_mqpx_list[qp_index], local_mr_mkey, (uint64_t)local_buffer + local_offset, remote_mr_mkey, (uint64_t)resource->addr + remote_offset, PAYLOAD);
            } else {
                dma_mqpx_list[qp_index]->wr_memcpy_direct(dma_mqpx_list[qp_index], remote_mr_mkey, (uint64_t)resource->addr + remote_offset, local_mr_mkey, (uint64_t)local_buffer + local_offset, PAYLOAD);
            }
            dma_start_index_list[qp_index]++;
        }
    }

    for (uint64_t qp_index = 0;qp_index < static_cast<uint64_t>(QP_PER_THREAD); qp_index++) {
        if (dma_start_index_list[qp_index] == dma_finish_index_list[qp_index]) {
            continue;
        }
        while (dma_finish_index_list[qp_index] < dma_start_index_list[qp_index]) {
            uint32_t num_wc = ibv_poll_cq(sq_cq, 16, wc);
            for (uint32_t i = 0;i < num_wc;i++) {
                if (wc[i].status != IBV_WC_SUCCESS) {
                    LOG_E("wc status %d", wc[i].status);
                    exit(__LINE__);
                }
                dma_finish_index_list[wc[i].wr_id >> 32]++;
            }
        }
    }


    double dma_mops = accumulate(dma_start_index_list.begin(), dma_start_index_list.end(), 0) * 1.0 / now_timer.toc();

    std::lock_guard<std::mutex> guard(IO_LOCK);

    total_bw += dma_mops * PAYLOAD * 8 / 1000;
    LOG_I("Thread %ld: %lf Mops %lf Gbps\n", thread_id, dma_mops, dma_mops * 8 * PAYLOAD / 1000);

    delete[]wc;

    for (auto dma_qp : dma_qp_list) {
        ibv_destroy_qp(dma_qp);
    }
    ibv_destroy_cq(sq_cq);
    ibv_destroy_cq(rq_cq);
    ibv_dereg_mr(local_mr);

    // don't need free hugepage memory
}

int main(int argc, char *argv[]) {
    signal(SIGINT, ctrl_c_handler);
    signal(SIGTERM, ctrl_c_handler);

    gflags::ParseCommandLineFlags(&argc, &argv, true);

    NUM_THREADS = FLAGS_threads;
    DEVICE_NAME = FLAGS_deviceName;
    GID_INDEX = FLAGS_gidIndex;
    IS_READ = FLAGS_isRead;
    PAYLOAD = FLAGS_payload;
    BATCH_SIZE = FLAGS_batchSize;
    NUMA_NODE = FLAGS_numaNode;
    BIND_OFFSET = FLAGS_bindOffset;
    EXPORTER_IP = FLAGS_serverIp;
    QP_PER_THREAD = FLAGS_qpPerThread;

    NetParam net_param;
    net_param.numNodes = 2;
    net_param.nodeId = 1;
    net_param.serverIp = EXPORTER_IP;
    net_param.device_name = DEVICE_NAME;
    net_param.gid_index = GID_INDEX;
    net_param.numa_node = NUMA_NODE;
    net_param.batch_size = BATCH_SIZE;
    net_param.sock_port = FLAGS_port;
    // use devx context for devx vhca
    net_param.use_devx_context = true;

    resources = new vhca_resource[NUM_THREADS];
    init_net_param(net_param);
    socket_init(net_param);
    exchange_vhca_data(net_param, resources, NUM_THREADS);
    close(net_param.sockfd[0]);

    roce_init(net_param, NUM_THREADS);

    uint32_t mmo_dma_max_length = get_mmo_dma_max_length(net_param.contexts[0]);
    LOG_I("mmo_dma_max_length %u\n", mmo_dma_max_length);
    rt_assert(mmo_dma_max_length >= static_cast<uint32_t>(PAYLOAD));

    rt_assert(QP_DEPTH >= static_cast<uint32_t>(BATCH_SIZE));
    rt_assert(1ul * BATCH_SIZE * PAYLOAD * QP_PER_THREAD <= resources[0].size);
    uint8_t access_key[32] = { 0 };
    for (int i = 0; i < 32;i++) {
        access_key[i] = 1;
    }

    for (int i = 0;i < NUM_THREADS;i++) {
        resources[i].pd = ibv_alloc_pd(net_param.contexts[i]);
        if (resources[i].pd == nullptr) {
            LOG_E("ibv_alloc_pd failed\n");
            return -1;
        }
        resources[i].mr = devx_create_crossing_mr(resources[i].pd, resources[i].addr, resources[i].size, resources[i].vhca_id, resources[i].mkey, access_key, sizeof(access_key));
        if (resources[i].mr == nullptr) {
            LOG_E("devx_create_crossing_mr failed\n");
            return -1;
        }
    }

    auto runner = bench_runner(NUM_THREADS, BIND_OFFSET, NUMA_NODE);

    runner.run(dma_copy_bench_routine, resources);
    usleep(100000);

    start_flag = true;
    for (int i = 0; i < FLAGS_life_time && stop_flag == false; i++) {
        sleep(1);
    }
    stop_flag = true;
    runner.stop();

    LOG_I("total_bw %lf Gbps\n", total_bw);

    for (int i = 0;i < NUM_THREADS;i++) {
        if (devx_dereg_mr(resources[i].mr) != 0) {
            LOG_E("can't devx_dereg_mr\n");
            return -1;
        }
        ibv_dealloc_pd(resources[i].pd);
        ibv_close_device(net_param.contexts[i]);
    }

}