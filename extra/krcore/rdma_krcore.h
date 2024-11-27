#ifndef __KRCORE_H__
#define __KRCORE_H__


#define KRCORE_IOCTL 0x12

#define KRCORE_TX_DEPTH 256
#define KRCORE_RX_DEPTH 256
#define KRCORE_NUM_WRS 1   // = batch size
#define KRCORE_NUM_SGES_PER_WR 1 // = sge per wr
#define KRCORE_ALLOC_SIZE 16*1024*1024
#define KRCORE_RDMA_GID_INDEX 3
#define KRCORE_MIN_RNR_TIMER		(12)
#define KRCORE_DEF_QP_TIME   (14)
#define KRCORE_CQ_POLL_BATCH 16
#define KRCORE_MAX_QP_PER_CORE 8

struct PingPongInfo {
    int lid;
    int qpn;
    int psn;
    unsigned rkey;
    unsigned long long vaddr;
    unsigned char	raw_gid[16];
    unsigned char mac[6];
    int gid_index;
    int out_reads;
};
struct KRCORE_IOC_CREATE_QP_PARAMS {
    size_t qp_per_core;
    size_t user_buf[KRCORE_MAX_QP_PER_CORE]; // size=KRCORE_ALLOC_SIZE, create in user space, half of the buffer used for send and the other half for recv
    size_t batch_size;
    struct PingPongInfo info[KRCORE_MAX_QP_PER_CORE];
};
#define KRCORE_IOC_CREATE_QP _IOWR(KRCORE_IOCTL, 1, struct KRCORE_IOC_CREATE_QP_PARAMS)

// -----------

struct KRCORE_IOC_INIT_QP_PARAMS {
    size_t qp_per_core;
    struct PingPongInfo info[KRCORE_MAX_QP_PER_CORE];
};
#define KRCORE_IOC_INIT_QP _IOWR(KRCORE_IOCTL, 2, struct KRCORE_IOC_INIT_QP_PARAMS)

// -----------

struct KRCORE_IOC_POST_SEND_PARAMS {
    size_t qp_id;
    size_t offset;
    size_t length;
    size_t batch_size;
    size_t success_send_cnt;
};
#define KRCORE_IOC_POST_SEND _IOWR(KRCORE_IOCTL, 3, struct KRCORE_IOC_POST_SEND_PARAMS)

// -----------

struct KRCORE_IOC_POST_RECV_PARAMS {
    size_t qp_id;
    size_t offset;
    size_t length;
    size_t batch_size;
    size_t success_post_cnt;
};
#define KRCORE_IOC_POST_RECV _IOWR(KRCORE_IOCTL, 4, struct KRCORE_IOC_POST_RECV_PARAMS)

// -----------

struct KRCORE_IOC_POLL_SEND_CQ_PARAMS {
    size_t qp_id;
    int max_poll_num;
    int actual_poll_num;
};
#define KRCORE_IOC_POLL_SEND_CQ _IOWR(KRCORE_IOCTL, 5, struct KRCORE_IOC_POLL_SEND_CQ_PARAMS)

// -----------

struct KRCORE_IOC_POLL_RECV_CQ_PARAMS {
    size_t qp_id;
    int max_poll_num;
    int actual_poll_num;
};
#define KRCORE_IOC_POLL_RECV_CQ _IOWR(KRCORE_IOCTL, 6, struct KRCORE_IOC_POLL_RECV_CQ_PARAMS)

// -----------

struct KRCORE_IOC_FREE_QP_PARAMS {
    size_t qp_per_core;
    size_t success;
};
#define KRCORE_IOC_FREE_QP _IOWR(KRCORE_IOCTL, 7, struct KRCORE_IOC_FREE_QP_PARAMS)

#endif  