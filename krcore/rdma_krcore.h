#ifndef __KRCORE_H__
#define __KRCORE_H__


#define KRCORE_IOCTL 0x12

struct PingPongInfo {
    int lid;
    int qpn;
    int psn;
    unsigned rkey;
    unsigned long long vaddr;
    union ib_gid gid;
    int gid_index;
    int out_reads;
};
struct KRCORE_IOC_CREATE_QP_PARAMS {
    struct PingPongInfo info;
};
#define KRCORE_IOC_CREATE_QP _IOWR(KRCORE_IOCTL, 1, struct KRCORE_IOC_CREATE_QP_PARAMS)

// -----------

struct KRCORE_IOC_INIT_QP_PARAMS {
    struct PingPongInfo info;
};
#define KRCORE_IOC_INIT_QP _IOWR(KRCORE_IOCTL, 2, struct KRCORE_IOC_INIT_QP_PARAMS)

#define KRCORE_TX_DEPTH 256
#define KRCORE_RX_DEPTH 256
#define KRCORE_NUM_WRS 1   // = batch size
#define KRCORE_NUM_SGES_PER_WR 1 // = sge per wr
#define KRCORE_ALLOC_SIZE 16*1024*1024
#define KRCORE_RDMA_GID_INDEX 3
#define KRCORE_MIN_RNR_TIMER		(12)
#define KRCORE_DEF_QP_TIME   (14)

#endif  