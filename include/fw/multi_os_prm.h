/*
 * Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES, ALL RIGHTS RESERVED.
 *
 * This software product is a proprietary product of NVIDIA CORPORATION &
 * AFFILIATES (the "Company") and all right, title, and interest in and to the
 * software product, including all associated intellectual property rights, are
 * and shall remain exclusively with the Company.
 *
 * This software product is governed by the End User License Agreement
 * provided with the software product.
 *
 */

#pragma once

/**
 * This file includes the PRM for multi OS support
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __linux__

#include <linux/types.h>
#include <endian.h>

#else /* implicit windows */

#include <windows.h>
#include <winnt.h>
#include <intrin.h>

__inline int
mlx5dv_l2w_is_error(void const *p)
{
	LONG_PTR x = (LONG_PTR)p;
	return x >= -UM_MAX_ERRNO && x <= 0;
}

__inline int
mlx5dv_l2w_ptr_error(void const *p)
{
	if (p == NULL) {
		return -UM_ENOMEM;
	}

	return (int)(LONG_PTR)p;
}

#define IS_ERR(ptr) mlx5dv_l2w_is_error(ptr)
#define PTR_ERR(ptr) mlx5dv_l2w_ptr_error(ptr)

#define MLX5_BF_OFFSET 0x800

#endif /* __linux__ */

#include <mlx5_prm.h>
#include <mlx5_prm_manual.h>

#define __mlx5_nullp(typ) ((struct mlx5_ifc_##typ##_bits *)NULL)
#define __mlx5_st_sz_bits(typ) sizeof(struct mlx5_ifc_##typ##_bits)
#define __mlx5_bit_sz(typ, fld) sizeof(__mlx5_nullp(typ)->fld)
#define __mlx5_bit_off(typ, fld) offsetof(struct mlx5_ifc_##typ##_bits, fld)
#define __mlx5_dw_off(bit_off) ((bit_off) / 32)
#define __mlx5_64_off(bit_off) ((bit_off) / 64)
#define __mlx5_dw_bit_off(bit_sz, bit_off) (32 - (bit_sz) - ((bit_off)&0x1f))
#define __mlx5_mask(bit_sz) ((uint32_t)((1ull << (bit_sz)) - 1))
#define __mlx5_dw_mask(bit_sz, bit_off) (__mlx5_mask(bit_sz) << __mlx5_dw_bit_off(bit_sz, bit_off))

#define MLX5_FLD_SZ_BITS(typ, fld) (__mlx5_bit_sz(typ, fld))
#define MLX5_FLD_SZ_BYTES(typ, fld) (__mlx5_bit_sz(typ, fld) / 8)
#define MLX5_ST_SZ_BYTES(typ) (sizeof(struct mlx5_ifc_##typ##_bits) / 8)
#define MLX5_ST_SZ_DW(typ) (sizeof(struct mlx5_ifc_##typ##_bits) / 32)
#define MLX5_ST_SZ_QW(typ) (sizeof(struct mlx5_ifc_##typ##_bits) / 64)
#define MLX5_UN_SZ_BYTES(typ) (sizeof(union mlx5_ifc_##typ##_bits) / 8)
#define MLX5_UN_SZ_DW(typ) (sizeof(union mlx5_ifc_##typ##_bits) / 32)
#define MLX5_BYTE_OFF(typ, fld) (__mlx5_bit_off(typ, fld) / 8)
#define MLX5_ADDR_OF(typ, p, fld) ((unsigned char *)(p) + MLX5_BYTE_OFF(typ, fld))

static inline void
_mlx5_set(void *p, uint32_t value, size_t bit_off, size_t bit_sz)
{
	__be32 *fld = (__be32 *)(p) + __mlx5_dw_off(bit_off);
	uint32_t dw_mask = __mlx5_dw_mask(bit_sz, bit_off);
	uint32_t mask = __mlx5_mask(bit_sz);

	*fld = htobe32((be32toh(*fld) & (~dw_mask)) | ((value & mask) << __mlx5_dw_bit_off(bit_sz, bit_off)));
}

#define MLX5_SET(typ, p, fld, v) _mlx5_set(p, v, __mlx5_bit_off(typ, fld), __mlx5_bit_sz(typ, fld))

static inline uint32_t
_mlx5_get(const void *p, size_t bit_off, size_t bit_sz)
{
	return ((be32toh(*((const __be32 *)(p) + __mlx5_dw_off(bit_off))) >> __mlx5_dw_bit_off(bit_sz, bit_off)) &
		__mlx5_mask(bit_sz));
}

#define MLX5_GET(typ, p, fld) _mlx5_get(p, __mlx5_bit_off(typ, fld), __mlx5_bit_sz(typ, fld))

static inline void
_mlx5_set64(void *p, uint64_t v, size_t bit_off)
{
	*((__be64 *)(p) + __mlx5_64_off(bit_off)) = htobe64(v);
}

#define MLX5_SET64(typ, p, fld, v) _mlx5_set64(p, v, __mlx5_bit_off(typ, fld))

static inline uint64_t
_mlx5_get64(const void *p, size_t bit_off)
{
	return be64toh(*((const __be64 *)(p) + __mlx5_64_off(bit_off)));
}

#define MLX5_GET64(typ, p, fld) _mlx5_get64(p, __mlx5_bit_off(typ, fld))

static inline void
priv_doca_mlx5_set_ctrl_seg(struct priv_doca_mlx5_wqe_ctrl_seg *seg, uint16_t pi, uint8_t opcode, uint8_t opmod, uint32_t qp_num,
			    uint8_t fm_ce_se, uint8_t ds, uint8_t signature, uint32_t imm)
{
	seg->opmod_idx_opcode = htobe32(((uint32_t)opmod << 24) | ((uint32_t)pi << 8) | opcode);
	seg->qpn_ds = htobe32((qp_num << 8) | ds);
	seg->fm_ce_se = fm_ce_se;
	seg->signature = signature;
	/*
	 * The caller should prepare "imm" in advance based on WR opcode.
	 * For IBV_WR_SEND_WITH_IMM and IBV_WR_RDMA_WRITE_WITH_IMM,
	 * the "imm" should be assigned as is.
	 * For the IBV_WR_SEND_WITH_INV, it should be htobe32(imm).
	 */
	seg->imm = imm;
}

/*
 * Data Segments - contain pointers and a byte count for the scatter/gather list.
 * They can optionally contain data, which will save a memory read access for
 * gather Work Requests.
 */
static inline void
priv_doca_mlx5_set_data_seg(struct priv_doca_mlx5_wqe_data_seg *seg, uint32_t length, uint32_t lkey, uintptr_t address)
{
	seg->byte_count = htobe32(length);
	seg->lkey = htobe32(lkey);
	seg->addr = htobe64(address);
}

/*
 * Remote Address Segments - contain remote virtual address, rkey and lower 32 bits that are reserved.
 */
static inline void
priv_doca_mlx5_set_remote_address_seg(struct priv_doca_mlx5_wqe_remote_address_seg *seg,
				      uintptr_t remote_virtual_address, uint32_t rkey)
{
	seg->remote_virtual_address = htobe64(remote_virtual_address);
	seg->rkey = htobe32(rkey);
	seg->reserved = 0;
}

/*
 * Atomic Segments - contain data for swap \ add (depending on the atomic operation),
 *		     and data for comparaison (for compare & swap operations).
 * The caller should prepare the segment fields in big endian format in advance.
 */
static inline void
priv_doca_mlx5_set_atomic_seg(struct priv_doca_mlx5_wqe_atomic_seg *seg, uint64_t swap_or_add_data, uint64_t cmp_data)
{
	seg->swap_or_add_data = htobe64(swap_or_add_data);
	seg->compare_data = htobe64(cmp_data);
}

/*
 * Meta-data Segments - contain pointer, mkey and lower 32 bits of the MMO control.
 */
static inline void
priv_doca_mlx5_set_metadata_seg(struct priv_doca_mlx5_wqe_mmo_metadata_seg *seg, uint32_t mmo_control_31_0,
				uint32_t lkey, uintptr_t address)
{
	seg->mmo_control_31_0 = htobe32(mmo_control_31_0);
	seg->local_key = htobe32(lkey);
	seg->local_address = htobe64(address);
}

/**
 * @brief Get CQE opcode
 * @details This method retrieves the opcode out of the CQE
 * @param [in] cqe
 * @return op code
 */
static inline uint8_t
priv_doca_mlx5_get_cqe_opcode(const struct priv_doca_mlx5_64b_cqe *cqe)
{
	return cqe->op_own >> PRIV_DOCA_MLX5_CQE_OPCODE_SHIFT;
}

/**
 * @brief Get CQE immediate
 * @details This method retrieves the immediate out of the CQE
 * @param [in] cqe
 * @return immediate
 */
static inline uint32_t
priv_doca_mlx5_get_cqe_imm(const struct priv_doca_mlx5_64b_cqe *cqe)
{
	return be32toh(cqe->imm_inval_pkey);
}

/**
 * @brief Get CQE QP number
 * @details This method retrieves the QP number out of the CQE
 * @param [in] cqe
 * @return immediate
 */
static inline uint32_t
priv_doca_mlx5_get_cqe_qpn(const struct priv_doca_mlx5_64b_cqe *cqe)
{
	return be32toh(cqe->sop_drop_qpn) & PRIV_DOCA_MLX5_CQE_QPN_MASK;
}

/**
 * @brief Get CQE user index data
 * @details This method retrieves the user index of the QP related to that CQE
 * @param [in] cqe
 * @return user_index data
 */
static inline uint32_t
priv_doca_mlx5_get_user_data_qpn(const struct priv_doca_mlx5_64b_cqe *cqe)
{
	return be32toh(cqe->srqn_uidx) & PRIV_DOCA_MLX5_CQE_USER_DATA_MASK;
}

#define MLX5_ALIGN_DOWN_BY(length, alignment) \
    ((length) & ~((alignment) - 1))

#define MLX5_ALIGN_UP_BY(length, alignment) \
    MLX5_ALIGN_DOWN_BY((length) + (alignment) - 1, alignment)

// cyclic rq with single data segment
#define CB_DEVX_RQE_BB 16
#define CB_DEVX_RQE_DS 16
#define CB_DEVX_RQ_ALIGN 64
#define CB_DEVX_RQE_BB_LOG 4

/* NOTE: hardcoded for now - 64 is fine for BF ARM and x86. */
#define CACHELINE_SIZE 64
#define MLX5_DBR_SIZE CACHELINE_SIZE
