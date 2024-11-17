#ifndef __KRCORE_H__
#define __KRCORE_H__


#define KRCORE_IOCTL 0x12

struct KRCORE_IOC_CREATE_QP_PARAMS {
    int a;
};
#define KRCORE_IOC_CREATE_QP _IOWR(KRCORE_IOCTL, 1, struct KRCORE_IOC_CREATE_QP_PARAMS)

// -----------

struct KRCORE_IOC_INIT_QP_PARAMS {
    int a;
};
#define KRCORE_IOC_INIT_QP _IOWR(KRCORE_IOCTL, 2, struct KRCORE_IOC_INIT_QP_PARAMS)


#endif  