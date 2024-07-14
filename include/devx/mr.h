#ifndef __DEVX_MR_H__
#define __DEVX_MR_H__

#include <infiniband/verbs.h>
#include <infiniband/mlx5dv.h>

struct devx_mr {
    // ibv_mr
    struct ibv_context *context;
    struct ibv_pd *pd;
    void *addr;
    size_t length;
    uint32_t handle;
    uint32_t lkey;
    uint32_t rkey;
    struct ibv_mr *ib_mr;
    struct mlx5dv_devx_umem *umem;
    struct mlx5dv_devx_obj *mkey;
    uint16_t vhca_id;
};

struct vhca_resource {
    ibv_pd *pd;
    devx_mr *mr;

    // exchanged
    uint16_t vhca_id;
    void *addr;
    uint64_t size;
    uint32_t mkey;
};

static inline uint32_t align(uint32_t val, uint32_t alignment) {
    return (val + alignment - 1) & ~(alignment - 1);
}

#endif /* __DEVX_MR_H__ */
