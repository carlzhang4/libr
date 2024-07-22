#include <unistd.h>
#include <stdlib.h>
#include <infiniband/verbs.h>
#include <infiniband/mlx5dv.h>
#include "util.hpp"
#include "multi_os_prm.h"
#include "mr.h"
#include "devx.h"

#define MLX5_GENERAL_OBJ_TYPE_MKEY 0xff01
#define PAGE_SIZE (sysconf(_SC_PAGESIZE))   /* OS page size */
#define LOG2(X) ((unsigned) (8*sizeof (unsigned long long) - __builtin_clzll((X)) - 1))

int devx_query_ibv_pd_number(struct ibv_pd *pd, uint32_t *pdn) {
    struct mlx5dv_obj mlx5_obj;
    mlx5_obj.pd.in = pd;
    struct mlx5dv_pd out_pd;
    mlx5_obj.pd.out = &out_pd;
    int ret = mlx5dv_init_obj(&mlx5_obj, MLX5DV_OBJ_PD);
    if (ret) {
        return DEVX_FAILED;
    }
    *pdn = out_pd.pdn;
    return DEVX_OK;
}

static int devx_allow_other_vhca_access(struct ibv_context *context, uint32_t lkey, void *access_key, int len) {
    uint32_t in[MLX5_ST_SZ_DW(allow_other_vhca_access_in)] = { 0 };
    uint32_t out[MLX5_ST_SZ_DW(allow_other_vhca_access_out)] = { 0 };

    if (len != 0x20) {
        return -1;
    }

    MLX5_SET(allow_other_vhca_access_in, in, opcode, MLX5_CMD_OP_ALLOW_OTHER_VHCA_ACCESS);
    MLX5_SET(allow_other_vhca_access_in, in, object_type_to_be_accessed, MLX5_GENERAL_OBJ_TYPE_MKEY);
    MLX5_SET(allow_other_vhca_access_in, in, object_id_to_be_accessed, lkey >> 8);
    void *access_key_dest = MLX5_ADDR_OF(allow_other_vhca_access_in, in, access_key);
    memcpy(access_key_dest, access_key, len);

    /* Send the command to FW using DevX. */
    int ret = mlx5dv_devx_general_cmd(context, in, sizeof(in), out, sizeof(out));
    if (ret) {
        LOG_E("Failed to allow other vhca access syndrome = 0x%x", MLX5_GET(allow_other_vhca_access_out, out, syndrome));
        return -1;
    }
    LOG_I("allow_other_vhca_access lkey %u", lkey);

    return 0;
}

static struct mlx5dv_devx_obj *devx_create_mkey(struct ibv_context *context, uint32_t pdn, uint32_t umem_id, uint32_t *mkey, uint64_t addr, uint64_t len, uint32_t access_flags) {
    int log_of_umem_page_size = LOG2(PAGE_SIZE);
    uint32_t in[MLX5_ST_SZ_DW(create_mkey_in)] = { 0 };
    uint32_t out[MLX5_ST_SZ_DW(create_mkey_out)] = { 0 };

    MLX5_SET(create_mkey_in, in, opcode, MLX5_CMD_OP_CREATE_MKEY);
    MLX5_SET(create_mkey_in, in, translations_octword_actual_size, 1);
    MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.access_mode_1_0, (int)PRIV_DOCA_MLX5_MKC_ACCESS_MODE_MTT);

    if (access_flags & IBV_ACCESS_REMOTE_ATOMIC) {
        MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.a, 1);
    }
    if (access_flags & IBV_ACCESS_REMOTE_WRITE) {
        MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.rw, 1);
    }
    if (access_flags & IBV_ACCESS_REMOTE_READ) {
        MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.rr, 1);
    }
    if (access_flags & IBV_ACCESS_LOCAL_WRITE) {
        MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.lw, 1);
    }
    MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.lr, 1);

    MLX5_SET64(create_mkey_in, in, memory_key_mkey_entry.start_addr, addr);
    MLX5_SET64(create_mkey_in, in, memory_key_mkey_entry.len, len);
    MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.pd, pdn);
    MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.qpn, 0xffffff);
    MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.mkey_7_0, 0x66);
    MLX5_SET(create_mkey_in, in, pg_access, 1);
    MLX5_SET(create_mkey_in, in, mkey_umem_id, umem_id);
    MLX5_SET(create_mkey_in, in, mkey_umem_valid, 1);
    void *mkc = MLX5_ADDR_OF(create_mkey_in, in, memory_key_mkey_entry);
    MLX5_SET(mkc, mkc, translations_octword_size_crossing_target_mkey, 1);
    MLX5_SET(mkc, mkc, log_entity_size, log_of_umem_page_size);

    struct mlx5dv_devx_obj *mkey_obj = mlx5dv_devx_obj_create(context, in, sizeof(in), out, sizeof(out));
    if (mkey_obj == NULL) {
        return NULL;
    }

    *mkey = MLX5_GET(create_mkey_out, out, mkey_index) << 8 | 0x66;

    LOG_I("create mkey %u, pdn:%u, umem_id:%u, addr:%p, len:%lu, flag:%u", *mkey, pdn, umem_id, (void *)addr, len, access_flags);
    return mkey_obj;
}

static struct mlx5dv_devx_obj *devx_create_alias_memory_region(struct ibv_context *context, uint32_t *mkey, uint32_t lkey, void *access_key, int len, uint16_t vhca_id, uint32_t pdn) {
    void *hdr, *alias_ctx, *op_param, *access_key_addr, *metadata;
    uint32_t in[MLX5_ST_SZ_DW(create_alias_obj_in)] = { 0 };
    uint32_t out[MLX5_ST_SZ_DW(create_alias_obj_out)] = { 0 };

    /* Create alias */
    hdr = MLX5_ADDR_OF(create_alias_obj_in, in, hdr);
    alias_ctx = MLX5_ADDR_OF(create_alias_obj_in, in, alias_ctx);
    op_param = MLX5_ADDR_OF(general_obj_in_cmd_hdr, in, op_param);

    MLX5_SET(general_obj_in_cmd_hdr, hdr, opcode, MLX5_CMD_OP_CREATE_GENERAL_OBJECT);
    MLX5_SET(general_obj_in_cmd_hdr, hdr, obj_type, MLX5_GENERAL_OBJ_TYPE_MKEY);
    MLX5_SET(general_obj_create_param, op_param, alias_object, 1);
    MLX5_SET(alias_context, alias_ctx, vhca_id_to_be_accessed, vhca_id);
    MLX5_SET(alias_context, alias_ctx, object_id_to_be_accessed, lkey >> 8);
    access_key_addr = MLX5_ADDR_OF(alias_context, alias_ctx, access_key);
    memcpy(access_key_addr, access_key, len);
    metadata = MLX5_ADDR_OF(alias_context, alias_ctx, metadata);
    int be_pdn = htobe32(pdn);
    memcpy(metadata, &be_pdn, sizeof(be_pdn));

    struct mlx5dv_devx_obj *mkey_obj = mlx5dv_devx_obj_create(context, in, sizeof(in), out, sizeof(out));
    if (mkey_obj == NULL) {
        return NULL;
    }
    int ret = MLX5_GET(create_alias_obj_out, out, alias_ctx.status);
    if (ret) {
        mlx5dv_devx_obj_destroy(mkey_obj);
        LOG_E("Created mr alias object in bad state, ret = %d", ret);
        return NULL;
    }

    *mkey = MLX5_GET(create_alias_obj_out, out, hdr.obj_id) << 8;
    LOG_I("create_alias_memory_region mkey %u, pdn:%u", *mkey, pdn);
    return mkey_obj;
}

static struct mlx5dv_devx_obj *devx_create_crossing_vhca_mkey(struct ibv_context *context, uint32_t *mkey, uint32_t pdn, uint32_t introspection_mkey, uint16_t vhca_id, uint32_t access_flags) {
    uint32_t in[MLX5_ST_SZ_DW(create_mkey_in)] = { 0 };
    uint32_t out[MLX5_ST_SZ_DW(create_mkey_out)] = { 0 };

    MLX5_SET(create_mkey_in, in, opcode, MLX5_CMD_OP_CREATE_MKEY);
    MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.access_mode_1_0, PRIV_DOCA_MLX5_MKC_ACCESS_MODE_CROSSING_VHCA_MKEY & 0x3);
    MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.access_mode_4_2, (PRIV_DOCA_MLX5_MKC_ACCESS_MODE_CROSSING_VHCA_MKEY >> 2) & 0x7);

    if (access_flags & IBV_ACCESS_REMOTE_ATOMIC) {
        MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.a, 1);
    }
    if (access_flags & IBV_ACCESS_REMOTE_WRITE) {
        MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.rw, 1);
    }
    if (access_flags & IBV_ACCESS_REMOTE_READ) {
        MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.rr, 1);
    }
    if (access_flags & IBV_ACCESS_LOCAL_WRITE) {
        MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.lw, 1);
    }
    MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.lr, 1);

    MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.pd, pdn);
    MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.qpn, 0xffffff);
    MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.length64, 1);
    MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.mkey_7_0, 0x66);
    void *mkc = MLX5_ADDR_OF(create_mkey_in, in, memory_key_mkey_entry);
    MLX5_SET(mkc, mkc, crossing_target_vhca_id, vhca_id);
    MLX5_SET(mkc, mkc, translations_octword_size_crossing_target_mkey, introspection_mkey);

    struct mlx5dv_devx_obj *mkey_obj = mlx5dv_devx_obj_create(context, in, sizeof(in), out, sizeof(out));
    if (mkey_obj == NULL) {
        LOG_E("create crossing vhca mkey failed = %u", MLX5_GET(create_mkey_out, out, syndrome));
        return NULL;
    }

    *mkey = MLX5_GET(create_mkey_out, out, mkey_index) << 8 | 0x66;

    LOG_I("create crossing vhca mkey %u, pdn:%u, introspection_mkey:%u, vhca_id:%u, flag:%u", *mkey, pdn, introspection_mkey, vhca_id, access_flags);
    return mkey_obj;
}


static struct mlx5dv_devx_umem *devx_umem_reg(struct ibv_context *context, void *addr, size_t size, uint32_t access) {
    struct mlx5dv_devx_umem_in umem_in = { 0 };

    umem_in.addr = addr;
    umem_in.size = size;
    umem_in.access = access;
    if (access & IBV_ACCESS_HUGETLB) {
        umem_in.access ^= IBV_ACCESS_HUGETLB;
        umem_in.pgsz_bitmap = 2 * 1024 * 1024;
    } else {
        umem_in.pgsz_bitmap = PAGE_SIZE;
    }
    return  mlx5dv_devx_umem_reg_ex(context, &umem_in);
}

struct devx_mr *devx_reg_mr(struct ibv_pd *pd, void *addr, size_t size, uint32_t access) {
    uint32_t pdn;
    uint32_t mkey;
    struct devx_mr *mr;
    struct mlx5dv_devx_umem *umem = devx_umem_reg(pd->context, addr, size, access);
    struct mlx5dv_devx_obj *mkey_obj;
    if (umem == NULL && errno == EPROTONOSUPPORT) {
        umem = mlx5dv_devx_umem_reg(pd->context, addr, size, access);
    }
    if (!umem) {
        LOG_E("devx_umem_reg failed, Got errno: %s", strerror(errno));
        goto __failed;
    }
    if (devx_query_ibv_pd_number(pd, &pdn) != DEVX_OK) {
        LOG_E("devx_query_ibv_pd_number failed");
        goto __destory_umem;
    }
    mkey_obj = devx_create_mkey(pd->context, pdn, umem->umem_id, &mkey, (uint64_t)addr, size, access);
    if (!mkey_obj) {
        LOG_E("devx_create_mkey failed");
        goto __destory_umem;
    }
    mr = reinterpret_cast<devx_mr *>(calloc(1, sizeof(*mr)));
    mr->context = pd->context;
    mr->pd = pd;
    mr->addr = addr;
    mr->length = size;
    mr->handle = umem->umem_id;
    mr->lkey = mkey;
    mr->rkey = mkey;
    mr->umem = umem;
    mr->mkey = mkey_obj;
    mr->vhca_id = 0;
    LOG_I("register mr %p: pd:%p, addr:%p, len:%lu, umem_id:%u, mkey:%u", mr, pd, addr, size, umem->umem_id, mkey);
    return mr;
__destory_umem:
    mlx5dv_devx_umem_dereg(umem);
__failed:
    return NULL;
}

struct devx_mr *devx_ibv_reg_mr(struct ibv_pd *pd, void *addr, size_t size, uint32_t access) {
    struct ibv_mr *ib_mr = ibv_reg_mr(pd, addr, size, access);
    struct devx_mr *mr;
    if (!ib_mr) {
        LOG_E("ibv_reg_mr failed");
        goto __failed;
    }
    mr = reinterpret_cast<devx_mr *>(calloc(1, sizeof(*mr)));
    mr->context = pd->context;
    mr->pd = pd;
    mr->addr = addr;
    mr->length = size;
    mr->handle = 0;
    mr->lkey = ib_mr->lkey;
    mr->rkey = ib_mr->rkey;
    mr->umem = NULL;
    mr->mkey = NULL;
    mr->vhca_id = 0;
    mr->ib_mr = ib_mr;
    LOG_I("register ib mr %p: pd:%p, addr:%p, len:%lu, mkey:%u", mr, pd, addr, size, ib_mr->lkey);
    return mr;
__failed:
    return NULL;
}

int devx_dereg_mr(struct devx_mr *mr) {
    int ret1 = 0;
    int ret2 = 0;
    int ret3 = 0;
    if (!mr) {
        return DEVX_FAILED;
    }
    if (mr->mkey) {
        ret1 = mlx5dv_devx_obj_destroy(mr->mkey);
        mr->mkey = NULL;
    }
    if (mr->umem) {
        ret2 = mlx5dv_devx_umem_dereg(mr->umem);
        mr->umem = NULL;
    }
    if (mr->ib_mr) {
        ret3 = ibv_dereg_mr(mr->ib_mr);
        mr->ib_mr = NULL;
    }
    LOG_I("deregister mr %p", mr);
    free(mr);
    return ret1 | ret2 | ret3;
}

uint32_t devx_mr_query_mkey(struct devx_mr *mr) {
    return mr->lkey;
}

int devx_mr_allow_other_vhca_access(struct devx_mr *mr, void *access_key, size_t len) {
    return devx_allow_other_vhca_access(mr->context, mr->lkey, access_key, len);
}

struct devx_mr *devx_create_crossing_mr(struct ibv_pd *pd, void *addr, size_t size, uint16_t vhca_id, uint32_t other_mkey, void *access_key, size_t access_key_len) {
    uint32_t pdn;
    uint32_t mkey;
    struct devx_mr *mr;
    if (devx_query_ibv_pd_number(pd, &pdn) != DEVX_OK) {
        LOG_E("devx_query_ibv_pd_number failed");
        return NULL;
    }
    struct mlx5dv_devx_obj *mkey_obj = devx_create_alias_memory_region(pd->context, &mkey, other_mkey, access_key, access_key_len, vhca_id, pdn);
    if (!mkey_obj) {
        return NULL;
    }
    mr = reinterpret_cast<devx_mr *>(calloc(1, sizeof(*mr)));
    mr->context = pd->context;
    mr->pd = pd;
    mr->addr = addr;
    mr->length = size;
    mr->handle = other_mkey;
    mr->lkey = mkey;
    mr->rkey = mkey;
    mr->umem = NULL;
    mr->mkey = mkey_obj;
    mr->vhca_id = vhca_id;
    mr->ib_mr = NULL;
    LOG_I("create crossing mr %p: pd:%p, addr:%p, len:%lu, vhca_id:%u, other_mkey:%u, mkey:%u", mr, pd, addr, size, vhca_id, other_mkey, mkey);
    return mr;
}

struct devx_mr *devx_create_crossing_vhca_mr(struct ibv_pd *pd, uint32_t introspection_mkey, uint16_t vhca_id, uint32_t access_flags) {
    uint32_t pdn;
    uint32_t mkey;
    struct devx_mr *mr;
    if (devx_query_ibv_pd_number(pd, &pdn) != DEVX_OK) {
        LOG_E("devx_query_ibv_pd_number failed");
        return NULL;
    }
    struct mlx5dv_devx_obj *mkey_obj = devx_create_crossing_vhca_mkey(pd->context, &mkey, pdn, introspection_mkey, vhca_id, access_flags);
    if (!mkey_obj) {
        return NULL;
    }
    mr = reinterpret_cast<devx_mr *>(calloc(1, sizeof(*mr)));
    mr->context = pd->context;
    mr->pd = pd;
    mr->addr = 0;
    mr->length = 0xffffffffffffff;
    mr->handle = introspection_mkey;
    mr->lkey = mkey;
    mr->rkey = mkey;
    mr->umem = NULL;
    mr->mkey = mkey_obj;
    mr->vhca_id = vhca_id;
    mr->ib_mr = NULL;
    LOG_I("create crossing vhca mr %p: pd:%p, vhca_id:%u, introspection_mkey:%u, mkey:%u", mr, pd, vhca_id, introspection_mkey, mkey);
    return mr;
}

static struct mlx5dv_devx_obj *devx_create_indirect_mkey(struct ibv_context *context, uint32_t *mkey, uint32_t pdn, uint64_t addr, uint64_t len, uint32_t access_flags, struct devx_klm *klm_array, int klm_num) {
    uint32_t aligned_sz = align((uint32_t)klm_num, 4);
    size_t in_size_dw = MLX5_ST_SZ_DW(create_mkey_in) + (aligned_sz * MLX5_ST_SZ_DW(umr_mem_buffer));
    uint32_t in[in_size_dw];
    uint32_t out[MLX5_ST_SZ_DW(create_mkey_out)] = { 0 };
    memset(in, 0, in_size_dw * 4);
    MLX5_SET(create_mkey_in, in, opcode, MLX5_CMD_OP_CREATE_MKEY);
    void *mkc = MLX5_ADDR_OF(create_mkey_in, in, memory_key_mkey_entry);
    int i;
    uint8_t *klm = (uint8_t *)MLX5_ADDR_OF(create_mkey_in, in, klm_or_pas_or_mtt_bsf);
    for (i = 0; i < klm_num; i++) {
        MLX5_SET(umr_mem_buffer, klm, byte_count, klm_array[i].byte_count);
        MLX5_SET(umr_mem_buffer, klm, mkey, klm_array[i].mkey);
        MLX5_SET64(umr_mem_buffer, klm, address, klm_array[i].address);
        klm += MLX5_ST_SZ_BYTES(umr_mem_buffer);
    }
    for (; i < (int)aligned_sz; i++) {
        MLX5_SET(umr_mem_buffer, klm, mkey, 0x0);
        MLX5_SET64(umr_mem_buffer, klm, address, 0x0);
        klm += MLX5_ST_SZ_BYTES(umr_mem_buffer);
    }
    MLX5_SET(create_mkey_in, in, translations_octword_actual_size, aligned_sz);
    MLX5_SET(mkc, mkc, access_mode_1_0, PRIV_DOCA_MLX5_MKC_ACCESS_MODE_KLMS);

    if (access_flags & IBV_ACCESS_REMOTE_ATOMIC) {
        MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.a, 1);
    }
    if (access_flags & IBV_ACCESS_REMOTE_WRITE) {
        MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.rw, 1);
    }
    if (access_flags & IBV_ACCESS_REMOTE_READ) {
        MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.rr, 1);
    }
    if (access_flags & IBV_ACCESS_LOCAL_WRITE) {
        MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.lw, 1);
    }
    MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.lr, 1);

    MLX5_SET64(create_mkey_in, in, memory_key_mkey_entry.start_addr, addr);
    MLX5_SET64(create_mkey_in, in, memory_key_mkey_entry.len, len);
    MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.pd, pdn);
    MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.qpn, 0xffffff);
    MLX5_SET(create_mkey_in, in, memory_key_mkey_entry.mkey_7_0, 0x66);
    MLX5_SET(mkc, mkc, umr_en, 1);
    MLX5_SET(mkc, mkc, translations_octword_size_crossing_target_mkey, aligned_sz);

    struct mlx5dv_devx_obj *mkey_obj = mlx5dv_devx_obj_create(context, in, in_size_dw * 4, out, sizeof(out));
    if (mkey_obj == NULL) {
        return NULL;
    }

    *mkey = MLX5_GET(create_mkey_out, out, mkey_index) << 8 | 0x66;

    LOG_I("create indirect mkey %u, pdn:%u, addr:%p, len:%lu, flag:%u", *mkey, pdn, (void *)addr, len, access_flags);
    return mkey_obj;
}

struct devx_mr *devx_create_indirect_mr(struct ibv_pd *pd, uint64_t addr, uint64_t len, uint32_t access_flags, struct devx_klm *klm_array, int klm_num) {
    uint32_t pdn;
    uint32_t mkey;
    struct devx_mr *mr;
    if (devx_query_ibv_pd_number(pd, &pdn) != DEVX_OK) {
        LOG_E("devx_query_ibv_pd_number failed");
        return NULL;
    }
    struct mlx5dv_devx_obj *mkey_obj = devx_create_indirect_mkey(pd->context, &mkey, pdn, addr, len, access_flags, klm_array, klm_num);
    if (!mkey_obj) {
        return NULL;
    }
    mr = reinterpret_cast<devx_mr *>(calloc(1, sizeof(*mr)));
    mr->context = pd->context;
    mr->pd = pd;
    mr->addr = (void *)addr;
    mr->length = len;
    mr->handle = 0;
    mr->lkey = mkey;
    mr->rkey = mkey;
    mr->umem = NULL;
    mr->mkey = mkey_obj;
    mr->vhca_id = 0;
    mr->ib_mr = NULL;
    LOG_I("create indirect mr %p: pd:%p, addr:%ld, len:%lu, mkey:%u", mr, pd, addr, len, mkey);
    return mr;
}

