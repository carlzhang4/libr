#ifndef __KRCORE_H__
#define __KRCORE_H__


#define KRCORE_IOCTL 0x12

struct PingPongInfo {
    int lid;
    int qpn;
    int psn;
    unsigned rkey;
    unsigned long long vaddr;
    unsigned char	raw_gid[16];
    int gid_index;
    int out_reads;
};
struct KRCORE_IOC_CREATE_QP_PARAMS {
    size_t user_buf; // size=KRCORE_ALLOC_SIZE, create in user space, half of the buffer used for send and the other half for recv
    struct PingPongInfo info;
};
#define KRCORE_IOC_CREATE_QP _IOWR(KRCORE_IOCTL, 1, struct KRCORE_IOC_CREATE_QP_PARAMS)

// -----------

struct KRCORE_IOC_INIT_QP_PARAMS {
    struct PingPongInfo info;
};
#define KRCORE_IOC_INIT_QP _IOWR(KRCORE_IOCTL, 2, struct KRCORE_IOC_INIT_QP_PARAMS)

// -----------

struct KRCORE_IOC_POST_SEND_PARAMS {
    size_t offset;
    int length;
    int success_send_cnt;
};
#define KRCORE_IOC_POST_SEND _IOWR(KRCORE_IOCTL, 3, struct KRCORE_IOC_POST_SEND_PARAMS)

// -----------

struct KRCORE_IOC_POST_RECV_PARAMS {
    size_t offset;
    int length;
    int success_post_cnt;
};
#define KRCORE_IOC_POST_RECV _IOWR(KRCORE_IOCTL, 4, struct KRCORE_IOC_POST_RECV_PARAMS)

// -----------

struct KRCORE_IOC_POLL_SEND_CQ_PARAMS {
    int max_poll_num;
    int actual_poll_num;
};
#define KRCORE_IOC_POLL_SEND_CQ _IOWR(KRCORE_IOCTL, 5, struct KRCORE_IOC_POLL_SEND_CQ_PARAMS)

// -----------

struct KRCORE_IOC_POLL_RECV_CQ_PARAMS {
    int max_poll_num;
    int actual_poll_num;
};
#define KRCORE_IOC_POLL_RECV_CQ _IOWR(KRCORE_IOCTL, 6, struct KRCORE_IOC_POLL_RECV_CQ_PARAMS)

// -----------

struct KRCORE_IOC_FREE_QP_PARAMS {
    int success;
};
#define KRCORE_IOC_FREE_QP _IOWR(KRCORE_IOCTL, 7, struct KRCORE_IOC_FREE_QP_PARAMS)

#define KRCORE_TX_DEPTH 256
#define KRCORE_RX_DEPTH 256
#define KRCORE_NUM_WRS 1   // = batch size
#define KRCORE_NUM_SGES_PER_WR 1 // = sge per wr
#define KRCORE_ALLOC_SIZE 16*1024*1024
#define KRCORE_RDMA_GID_INDEX 3
#define KRCORE_MIN_RNR_TIMER		(12)
#define KRCORE_DEF_QP_TIME   (14)
#define KRCORE_CQ_POLL_BATCH 16

#endif  