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

#ifndef MLX5_PRM_MANUAL_H_
#define MLX5_PRM_MANUAL_H_

#include <stdint.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef u8
#define u8 uint8_t
#endif

#define PRIV_DOCA_MLX5_GENERAL_OBJ_TYPES_CAP_CHANNEL_SERVICE     \
    (1ULL << MLX5_GEN_OBJ_TYPE_CHANNEL_SERVICE)

#define PRIV_DOCA_MLX5_GENERAL_OBJ_TYPES_CAP_CHANNEL_CONNECTION  \
    (1ULL << MLX5_GEN_OBJ_TYPE_CHANNEL_CONNECTION)

/*
 * Need to define this since this enumeration value has 64 bits and
 * A-ME doesn't know how to handle this
 */
#define PRIV_DOCA_MLX5_HCA_CAPS_2_ALLOWED_OBJECT_FOR_OTHER_VHCA_ACCESS_DPA_THREAD \
    (1ULL << 0x2b)

enum priv_doca_mlx5_qpc_opt_mask_32 {
	PRIV_DOCA_MLX5_QPC_OPT_MASK_32_INIT2INIT_MMO = 1 << 3,
};

struct mlx5_ifc_qpc_extension_and_pas_list_bits {
	struct mlx5_ifc_qp_context_extension_bits qpc_data_extension;
	uint8_t pas[0][0x40];
};

struct mlx5_ifc_create_alias_obj_in_bits {
	struct mlx5_ifc_general_obj_in_cmd_hdr_bits hdr;
	struct mlx5_ifc_alias_context_bits alias_ctx;
};

struct mlx5_ifc_create_alias_obj_out_bits {
	struct mlx5_ifc_general_obj_out_cmd_hdr_bits hdr;
	struct mlx5_ifc_alias_context_bits alias_ctx;
};

struct mlx5_ifc_create_channel_service_obj_in_bits {
	struct mlx5_ifc_general_obj_in_cmd_hdr_bits hdr;
	struct mlx5_ifc_channel_service_obj_bits ch_svc_obj;
};

struct mlx5_ifc_create_channel_service_obj_out_bits {
	struct mlx5_ifc_general_obj_out_cmd_hdr_bits hdr;
};

struct mlx5_ifc_create_channel_connection_obj_in_bits {
	struct mlx5_ifc_general_obj_in_cmd_hdr_bits hdr;
	struct mlx5_ifc_channel_connection_obj_bits ch_conn_obj;
};

struct mlx5_ifc_create_channel_connection_obj_out_bits {
	struct mlx5_ifc_general_obj_out_cmd_hdr_bits hdr;
};

struct mlx5_ifc_create_dct_in_bits {
	u8 opcode[0x10];
	u8 uid[0x10];

	u8 reserved_at_20[0x10];
	u8 op_mod[0x10];

	u8 reserved_at_40[0x40];

	struct mlx5_ifc_dctc_bits dct_context;

	u8 reserved_at_280[0x180]; /** padding required for Winof2 devx (to get to 176B structure) */
};

enum {
	PRIV_DOCA_MLX5_SEND_WQE_BB = 64,
	PRIV_DOCA_MLX5_SEND_WQE_SHIFT = 6,
};

#pragma pack(push, 1)
struct priv_doca_mlx5_wqe_ctrl_seg {
	uint32_t opmod_idx_opcode;
	uint32_t qpn_ds;
	union {
		struct {
			uint8_t signature;
			uint8_t rsvd[2];
			uint8_t fm_ce_se;
		};
		struct {
			uint32_t flags;
		};
	};

	uint32_t imm;
};
#pragma pack(pop)

struct priv_doca_mlx5_wqe_umr_ctrl_seg {
	uint8_t flags;
	uint8_t rsvd0[3];
	__be16 klm_octowords;
	union {
		__be16 translation_offset;
		__be16 bsf_octowords;
	};
	__be64 mkey_mask;
	uint8_t rsvd1[32];
};

struct priv_doca_mlx5_wqe_umr_klm_seg {
	/* up to 2GB */
	__be32 byte_count;
	__be32 mkey;
	__be64 address;
};

union priv_doca_mlx5_wqe_umr_inline_seg {
	struct priv_doca_mlx5_wqe_umr_klm_seg klm;
};

struct priv_doca_mlx5_wqe_mkey_context_seg {
	uint8_t free;
	uint8_t reserved1;
	uint8_t access_flags;
	uint8_t sf;
	__be32 qpn_mkey;
	__be32 reserved2;
	__be32 flags_pd;
	__be64 start_addr;
	__be64 len;
	__be32 bsf_octword_size;
	__be32 reserved3[4];
	__be32 translations_octword_size;
	uint8_t reserved4[3];
	uint8_t log_page_size;
	__be32 reserved;
	union priv_doca_mlx5_wqe_umr_inline_seg inseg[0];
};

#pragma pack(push, 1)
struct priv_doca_mlx5_wqe_data_seg {
	__be32 byte_count;
	__be32 lkey;
	__be64 addr;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct priv_doca_mlx5_wqe_eth_seg {
		union {
		struct {
			__be32 swp_offs;
			uint8_t	cs_flags;
			uint8_t	swp_flags;
			__be16 mss;
			__be32 metadata;
			__be16 inline_hdr_sz;
			union {
				__be16 inline_data;
				__be16 vlan_tag;
			};
		};
		struct {
			__be32 offsets;
			__be32 flags;
			__be32 flow_metadata;
			__be32 inline_hdr;
		};
	};
};
#pragma pack(pop)

struct priv_doca_mlx5_wqe_remote_address_seg {
	__be64 remote_virtual_address;
	__be32 rkey;
	__be32 reserved;
};

struct priv_doca_mlx5_wqe_atomic_seg {
	__be64 swap_or_add_data;
	__be64 compare_data;
};

enum {
	PRIV_DOCA_MLX5_WQE_CTRL_CE_CQE_ON_CQE_ERROR = 0x0,
	PRIV_DOCA_MLX5_WQE_CTRL_CE_CQE_ON_FIRST_CQE_ERROR = 0x1,
	PRIV_DOCA_MLX5_WQE_CTRL_CE_CQE_ALWAYS = 0x2,
	PRIV_DOCA_MLX5_WQE_CTRL_CE_CQE_AND_EQE = 0x3,
};

enum {
	PRIV_DOCA_MLX5_WQE_CTRL_FM_NO_FENCE = 0x0,
	PRIV_DOCA_MLX5_WQE_CTRL_FM_INITIATOR_SMALL_FENCE = 0x1,
	PRIV_DOCA_MLX5_WQE_CTRL_FM_FENCE = 0x2,
	PRIV_DOCA_MLX5_WQE_CTRL_FM_STRONG_ORDERING = 0x3,
	PRIV_DOCA_MLX5_WQE_CTRL_FM_FENCE_AND_INITIATOR_SMALL_FENCE = 0x4,
};

/* The completion mode offset in the WQE control segment line 2. */
#define MLX5_COMP_MODE_OFFSET 2

enum {
	PRIV_DOCA_MLX5_WQE_CTRL_CQ_UPDATE = PRIV_DOCA_MLX5_WQE_CTRL_CE_CQE_ALWAYS << MLX5_COMP_MODE_OFFSET,
	PRIV_DOCA_MLX5_WQE_CTRL_SOLICITED = 1 << 1,
	PRIV_DOCA_MLX5_WQE_CTRL_FENCE = PRIV_DOCA_MLX5_WQE_CTRL_FM_FENCE_AND_INITIATOR_SMALL_FENCE << 5,
	PRIV_DOCA_MLX5_WQE_CTRL_INITIATOR_SMALL_FENCE = PRIV_DOCA_MLX5_WQE_CTRL_FM_INITIATOR_SMALL_FENCE << 5,
};

enum {
	PRIV_DOCA_MLX5_RCV_DBR = 0,
	PRIV_DOCA_MLX5_SND_DBR = 1,
};

enum priv_doca_mlx5_opcode_type {
	PRIV_DOCA_MLX5_OPCODE_NOP = 0x00,
	PRIV_DOCA_MLX5_OPCODE_SEND_INVAL = 0x01,
	PRIV_DOCA_MLX5_OPCODE_RDMA_WRITE = 0x08,
	PRIV_DOCA_MLX5_OPCODE_RDMA_WRITE_IMM = 0x09,
	PRIV_DOCA_MLX5_OPCODE_SEND = 0x0a,
	PRIV_DOCA_MLX5_OPCODE_SEND_IMM = 0x0b,
	PRIV_DOCA_MLX5_OPCODE_LSO = 0x0e,
	PRIV_DOCA_MLX5_OPCODE_RDMA_READ = 0x10,
	PRIV_DOCA_MLX5_OPCODE_ATOMIC_CS = 0x11,
	PRIV_DOCA_MLX5_OPCODE_ATOMIC_FA = 0x12,
	PRIV_DOCA_MLX5_OPCODE_ATOMIC_MASK_CS = 0x14,
	PRIV_DOCA_MLX5_OPCODE_ATOMIC_MASK_FA = 0x15,
	PRIV_DOCA_MLX5_OPCODE_BIND_MW = 0x18,
	PRIV_DOCA_MLX5_OPCODE_FMR = 0x19,
	PRIV_DOCA_MLX5_OPCODE_LOCAL_INVAL = 0x1b,
	PRIV_DOCA_MLX5_OPCODE_CONFIG_CMD = 0x1f,
	PRIV_DOCA_MLX5_OPCODE_DUMP = 0x23,
	PRIV_DOCA_MLX5_OPCODE_UMR = 0x25,
	PRIV_DOCA_MLX5_OPCODE_MMO = 0x2f,

	PRIV_DOCA_MLX5_RECV_OPCODE_RDMA_WRITE_IMM = 0x00,
	PRIV_DOCA_MLX5_RECV_OPCODE_SEND = 0x01,
	PRIV_DOCA_MLX5_RECV_OPCODE_SEND_IMM = 0x02,
	PRIV_DOCA_MLX5_RECV_OPCODE_SEND_INVAL = 0x03,

	PRIV_DOCA_MLX5_CQE_OPCODE_ERROR = 0x1e,
	PRIV_DOCA_MLX5_CQE_OPCODE_RESIZE = 0x16,

	PRIV_DOCA_MLX5_OPCODE_INVALID = 0xff
};

enum {
	PRIV_DOCA_MLX5_OPC_MOD_MMO_SHA = 0x0,
	PRIV_DOCA_MLX5_OPC_MOD_MMO_DMA = 0x1,
	PRIV_DOCA_MLX5_OPC_MOD_MMO_COMPRESS = 0x2,
	PRIV_DOCA_MLX5_OPC_MOD_MMO_DECOMPRESS = 0x3,
	PRIV_DOCA_MLX5_OPC_MOD_MMO_REGEX = 0x4,
	PRIV_DOCA_MLX5_OPC_MOD_MMO_EC = 0x7,
};

enum {
	PRIV_DOCA_MLX5_WQE_UMR_CTRL_MKEY_MASK_LEN			= 1 << 0,
	PRIV_DOCA_MLX5_WQE_UMR_CTRL_MKEY_MASK_START_ADDR		= 1 << 6,
	PRIV_DOCA_MLX5_WQE_UMR_CTRL_MKEY_MASK_SIG_ERR			= 1 << 9,
	PRIV_DOCA_MLX5_WQE_UMR_CTRL_MKEY_MASK_BSF_ENABLE		= 1 << 12,
	PRIV_DOCA_MLX5_WQE_UMR_CTRL_MKEY_MASK_MKEY			= 1 << 13,
	PRIV_DOCA_MLX5_WQE_UMR_CTRL_MKEY_MASK_QPN			= 1 << 14,
	PRIV_DOCA_MLX5_WQE_UMR_CTRL_MKEY_MASK_ACCESS_LOCAL_WRITE	= 1 << 18,
	PRIV_DOCA_MLX5_WQE_UMR_CTRL_MKEY_MASK_ACCESS_REMOTE_READ	= 1 << 19,
	PRIV_DOCA_MLX5_WQE_UMR_CTRL_MKEY_MASK_ACCESS_REMOTE_WRITE	= 1 << 20,
	PRIV_DOCA_MLX5_WQE_UMR_CTRL_MKEY_MASK_ACCESS_ATOMIC		= 1 << 21,
	PRIV_DOCA_MLX5_WQE_UMR_CTRL_MKEY_MASK_FREE			= 1 << 29,
};

enum {
	PRIV_DOCA_MLX5_WQE_UMR_CTRL_FLAG_INLINE =		1 << 7,
	PRIV_DOCA_MLX5_WQE_UMR_CTRL_FLAG_CHECK_FREE =		1 << 5,
	PRIV_DOCA_MLX5_WQE_UMR_CTRL_FLAG_TRNSLATION_OFFSET =	1 << 4,
	PRIV_DOCA_MLX5_WQE_UMR_CTRL_FLAG_CHECK_QPN =		1 << 3,
};

// MSVC can't compile member structs if they have [0] at the end and they are not the last member
// See priv_doca_mlx5_wqe_mkey_context_seg above and it's usage in the code.
struct priv_doca_mlx5_wqe_mkey_context_seg_member {
	uint8_t free;
	uint8_t reserved1;
	uint8_t access_flags;
	uint8_t sf;
	__be32 qpn_mkey;
	__be32 reserved2;
	__be32 flags_pd;
	__be64 start_addr;
	__be64 len;
	__be32 bsf_octword_size;
	__be32 reserved3[4];
	__be32 translations_octword_size;
	uint8_t reserved4[3];
	uint8_t log_page_size;
	__be32 reserved;
};

struct priv_doca_mlx5_wqe_mmo_metadata_seg {
	__be32 mmo_control_31_0;
	__be32 local_key;
	__be64 local_address;
};

struct priv_doca_mlx5_mmo_wqe {
	struct priv_doca_mlx5_wqe_ctrl_seg ctrl;
	struct priv_doca_mlx5_wqe_mmo_metadata_seg mmo_meta;
	struct priv_doca_mlx5_wqe_data_seg src;
	struct priv_doca_mlx5_wqe_data_seg dest;
};

struct priv_doca_mlx5_wqe_transpose_seg {
	uint8_t reserved_0[0x3];
	uint8_t element_size;

	uint8_t reserved_20;
	uint8_t num_of_cols;
	uint8_t reserved_30;
	uint8_t num_of_rows;

	uint8_t reserved_40[0x8];
};

struct priv_doca_mlx5_eth_wqe {
	struct priv_doca_mlx5_wqe_ctrl_seg ctrl;
	struct priv_doca_mlx5_wqe_eth_seg eseg;
	struct priv_doca_mlx5_wqe_data_seg dseg0;
	struct priv_doca_mlx5_wqe_data_seg dseg1;
};

#pragma pack(push, 1)
struct priv_doca_mlx5_dump_wqe {
	struct priv_doca_mlx5_wqe_ctrl_seg cseg;
	struct priv_doca_mlx5_wqe_data_seg dseg;
};
#pragma pack(pop)

#define MLX5_MIN_SINGLE_STRIDE_LOG_NUM_BYTES 6
#define MLX5_MIN_SINGLE_WQE_LOG_NUM_STRIDES 9

/* Renaming structs wrt /usr/include/infiniband/mlx5dv.h for GPU support. */
#pragma pack(push, 1)
struct priv_doca_mlx5_wqe_mprq_next_seg {
	uint8_t rsvd0[2];
	uint16_t next_wqe_index;
	uint8_t signature;
	uint8_t rsvd1[11];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct priv_doca_mlx5_wqe_mprq {
	struct priv_doca_mlx5_wqe_mprq_next_seg next_seg;
	struct priv_doca_mlx5_wqe_data_seg dseg;
};
#pragma pack(pop)

#define MLX5_WSEG_SIZE 16u

#pragma pack(push, 1)
struct priv_doca_mlx5_wqe_wseg {
	uint32_t operation;
	uint32_t lkey;
	uint32_t va_high;
	uint32_t va_low;
	uint64_t value;
	uint64_t mask;
};
#pragma pack(pop)

enum {
	MLX5_ACCESS_REGISTER_IN_OP_MOD_WRITE  = 0x0,
	MLX5_ACCESS_REGISTER_IN_OP_MOD_READ   = 0x1,
};

enum {
	MLX5_REGISTER_ID_MTUTC  = 0x9055,
	MLX5_CRYPTO_OPERATIONAL_REGISTER_ID = 0xC002,
	MLX5_CRYPTO_COMMISSIONING_REGISTER_ID = 0xC003,
	MLX5_IMPORT_KEK_HANDLE_REGISTER_ID = 0xC004,
	MLX5_CREDENTIAL_HANDLE_REGISTER_ID = 0xC005,
	MLX5_QSHR_REGISTER_ID = 0x4030,
};

#define MLX5_MTUTC_TIMESTAMP_MODE_INTERNAL_TIMER 0
#define MLX5_MTUTC_TIMESTAMP_MODE_REAL_TIME 1

struct mlx5_ifc_register_mtutc_bits {
	u8 time_stamp_mode[0x2];
	u8 time_stamp_state[0x2];
	u8 reserved_at_4[0x18];
	u8 operation[0x4];
	u8 freq_adjustment[0x20];
	u8 reserved_at_40[0x40];
	u8 utc_sec[0x20];
	u8 utc_nsec[0x20];
	u8 time_adjustment[0x20];
};

struct mlx5_ifc_ets_global_config_register_bits {
	u8 reserved_at_0[0x2];
	u8 rate_limit_update[0x1];
	u8 reserved_at_3[0x29];
	u8 max_bw_units[0x4];
	u8 reserved_at_48[0x8];
	u8 max_bw_value[0x8];
};

#define MLX5_ACCESS_REGISTER_DATA_DWORD_MAX 8u

struct mlx5_ifc_access_register_out_bits {
	u8 status[0x8];
	u8 reserved_at_8[0x18];
	u8 syndrome[0x20];
	u8 reserved_at_40[0x40];
	u8 register_data[0][0x20];
};

struct mlx5_ifc_access_register_in_bits {
	u8 opcode[0x10];
	u8 reserved_at_10[0x10];
	u8 reserved_at_20[0x10];
	u8 op_mod[0x10];
	u8 reserved_at_40[0x10];
	u8 register_id[0x10];
	u8 argument[0x20];
	u8 register_data[0][0x20];
};

#define MLX5_MPRQ_LEN_MASK 0x000ffff
#define MLX5_MPRQ_LEN_SHIFT 0
#define MLX5_MPRQ_STRIDE_NUM_MASK 0x3fff0000
#define MLX5_MPRQ_STRIDE_NUM_SHIFT 16
#define MLX5_MPRQ_FILLER_MASK 0x80000000
#define MLX5_MPRQ_FILLER_SHIFT 31

#define MLX5_MPRQ_STRIDE_SHIFT_BYTE 2

enum {
	PRIV_DOCA_MLX5_CQE_OWNER_MASK		= 1,
	PRIV_DOCA_MLX5_CQE_REQ			= 0,
	PRIV_DOCA_MLX5_CQE_RESP_WR_IMM		= 1,
	PRIV_DOCA_MLX5_CQE_RESP_SEND		= 2,
	PRIV_DOCA_MLX5_CQE_RESP_SEND_IMM	= 3,
	PRIV_DOCA_MLX5_CQE_RESP_SEND_INV	= 4,
	PRIV_DOCA_MLX5_CQE_RESIZE_CQ		= 5,
	PRIV_DOCA_MLX5_CQE_NO_PACKET		= 6,
	PRIV_DOCA_MLX5_CQE_REQ_ERR		= 13,
	PRIV_DOCA_MLX5_CQE_RESP_ERR		= 14,
	PRIV_DOCA_MLX5_CQE_INVALID		= 15,
};

enum {
	PRIV_DOCA_MLX5_CQ_DB_REQ_NOT_SOL	= 1 << 24,
	PRIV_DOCA_MLX5_CQ_DB_REQ_NOT		= 0 << 24,
};

enum {
	PRIV_DOCA_MLX5_CQ_DOORBELL	= 0x20
};

enum {
	PRIV_DOCA_MLX5_CQ_SET_CI	= 0,
	PRIV_DOCA_MLX5_CQ_ARM_DB	= 1,
};

#define PRIV_DOCA_MLX5_CQE_OPCODE_SHIFT 4
#define PRIV_DOCA_MLX5_CQE_QPN_MASK 0xffffff
#define PRIV_DOCA_MLX5_CQE_USER_DATA_MASK 0xffffff

struct priv_doca_mlx5_64b_cqe {
	struct {
		uint8_t		reserved_at_0[2];
		__be16		wqe_id;

		uint8_t		reserved_at_4[12];

		uint8_t		reserved_at_16;
		uint8_t		ml_path;
		uint8_t		rsvd20[2];

		uint8_t		rsvd22[2];
		__be16		slid;

		__be32		flags_rqpn;

		uint8_t		hds_ip_ext;
		uint8_t		l4_hdr_type_etc;
		__be16		vlan_info;
	};
	__be32		srqn_uidx;

	__be32		imm_inval_pkey;

	uint8_t		app;
	uint8_t		app_op;
	__be16		app_info;

	__be32		byte_cnt;

	__be64		timestamp;

	__be32		sop_drop_qpn;

	__be16		wqe_counter;
	uint8_t		signature;
	uint8_t		op_own;
};
static_assert(sizeof(struct priv_doca_mlx5_64b_cqe) == 64, "struct priv_doca_mlx5_64b_cqe is not 64 bytes.");

struct priv_doca_mlx5_error_cqe {
	uint8_t rsvd0[54];
	uint8_t vendor_err_synd;
	uint8_t syndrome;
	uint8_t rsvd56[4];
	__be16 wqe_counter;
	uint8_t rsvd62[2];
};
static_assert(sizeof(struct priv_doca_mlx5_error_cqe) == 64, "struct priv_doca_mlx5_error_cqe is not 64 bytes.");

enum {
	PRIV_DOCA_MLX5_CMD_STAT_OK = 0x0,
	PRIV_DOCA_MLX5_CMD_STAT_INT_ERR = 0x1,
	PRIV_DOCA_MLX5_CMD_STAT_BAD_OP_ERR = 0x2,
	PRIV_DOCA_MLX5_CMD_STAT_BAD_PARAM_ERR = 0x3,
	PRIV_DOCA_MLX5_CMD_STAT_BAD_SYS_STATE_ERR = 0x4,
	PRIV_DOCA_MLX5_CMD_STAT_BAD_RES_ERR = 0x5,
	PRIV_DOCA_MLX5_CMD_STAT_RES_BUSY = 0x6,
	PRIV_DOCA_MLX5_CMD_STAT_LIM_ERR = 0x8,
	PRIV_DOCA_MLX5_CMD_STAT_BAD_RES_STATE_ERR = 0x9,
	PRIV_DOCA_MLX5_CMD_STAT_IX_ERR = 0xa,
	PRIV_DOCA_MLX5_CMD_STAT_NO_RES_ERR = 0xf,
	PRIV_DOCA_MLX5_CMD_STAT_BAD_INP_LEN_ERR = 0x50,
	PRIV_DOCA_MLX5_CMD_STAT_BAD_OUTP_LEN_ERR = 0x51,
	PRIV_DOCA_MLX5_CMD_STAT_BAD_QP_STATE_ERR = 0x10,
	PRIV_DOCA_MLX5_CMD_STAT_BAD_PKT_ERR = 0x30,
	PRIV_DOCA_MLX5_CMD_STAT_BAD_SIZE_OUTS_CQES_ERR = 0x40,
};

enum {
	PRIV_DOCA_MLX5_MKC_ACCESS_MODE_PA    = 0x0,
	PRIV_DOCA_MLX5_MKC_ACCESS_MODE_MTT   = 0x1,
	PRIV_DOCA_MLX5_MKC_ACCESS_MODE_KLMS  = 0x2,
	PRIV_DOCA_MLX5_MKC_ACCESS_MODE_KSM   = 0x3,
	PRIV_DOCA_MLX5_MKC_ACCESS_MODE_SW_ICM = 0x4,
	PRIV_DOCA_MLX5_MKC_ACCESS_MODE_MEMIC = 0x5,
	PRIV_DOCA_MLX5_MKC_ACCESS_MODE_CROSSING_VHCA_MKEY = 0x6,
};

enum {
	PRIV_DOCA_MLX5_HCA_CAP_OP_MOD_GENERAL_DEVICE        = 0x0 << 1,
	PRIV_DOCA_MLX5_HCA_CAP_OP_MOD_ETHERNET_OFFLOAD      = 0x1 << 1,
	PRIV_DOCA_MLX5_HCA_CAP_OP_MOD_ATOMIC		    = 0x3 << 1,
	PRIV_DOCA_MLX5_HCA_CAP_OP_MOD_ROCE                  = 0x4 << 1,
	PRIV_DOCA_MLX5_HCA_CAP_OP_MOD_NIC_FLOW_TABLE        = 0x7 << 1,
	PRIV_DOCA_MLX5_HCA_CAP_OP_MOD_ESW_FLOW_TABLE        = 0x8 << 1,
	PRIV_DOCA_MLX5_HCA_CAP_OP_MOD_QOS                   = 0xc << 1,
	PRIV_DOCA_MLX5_HCA_CAP_OP_MOD_ESW                   = 0x9 << 1,
	PRIV_DOCA_MLX5_HCA_CAP_OP_MOD_DEVICE_MEMORY         = 0xf << 1,
	PRIV_DOCA_MLX5_HCA_CAP_OP_MOD_IPSEC                 = 0x15 << 1,
	PRIV_DOCA_MLX5_HCA_CAP_OP_MOD_CRYPTO                = 0x1a << 1,
	PRIV_DOCA_MLX5_HCA_CAP_OP_MOD_GENERAL_DEVICE_CAP_2  = 0x20 << 1,
};

enum priv_doca_mlx5_cap_mode {
	PRIV_DOCA_MLX5_HCA_CAP_OPMOD_GET_MAX = 0,
	PRIV_DOCA_MLX5_HCA_CAP_OPMOD_GET_CUR = 1,
};

/* Page size is provided in granularity of 4K. */
#define PRIV_DOCA_MLX5_ADAPTER_PAGE_SHIFT 12

enum {
	MLX5_RQC_MEM_RQ_TYPE_MEMORY_RQ_INLINE  = 0x0,
	MLX5_RQC_MEM_RQ_TYPE_MEMORY_RQ_RMP     = 0x1,
};

struct mlx5_ifc_query_rq_out_bits {
	u8 status[0x8];
	u8 reserved_at_8[0x18];
	u8 syndrome[0x20];
	u8 reserved_at_40[0xc0];
	struct mlx5_ifc_rqc_bits rq_context;
};

struct mlx5_ifc_query_rq_in_bits {
	u8 opcode[0x10];
	u8 reserved_at_10[0x10];
	u8 reserved_at_20[0x10];
	u8 op_mod[0x10];
	u8 reserved_at_40[0x8];
	u8 rqn[0x18];
	u8 reserved_at_60[0x20];
};

/* devX creation object */
struct mlx5_devx_obj {
	void *obj; /* The DV object. */
	int id; /* The object ID. */
};

struct mlx5_devx_wq_attr {
	uint32_t wq_type:4;
	uint32_t wq_signature:1;
	uint32_t end_padding_mode:2;
	uint32_t cd_slave:1;
	uint32_t hds_skip_first_sge:1;
	uint32_t log2_hds_buf_size:3;
	uint32_t page_offset:5;
	uint32_t lwm:16;
	uint32_t pd:24;
	uint32_t uar_page:24;
	uint64_t dbr_addr;
	uint32_t hw_counter;
	uint32_t sw_counter;
	uint32_t log_wq_stride:4;
	uint32_t log_wq_pg_sz:5;
	uint32_t log_wq_sz:5;
	uint32_t dbr_umem_valid:1;
	uint32_t wq_umem_valid:1;
	uint32_t log_hairpin_num_packets:5;
	uint32_t log_hairpin_data_sz:5;
	uint32_t single_wqe_log_num_of_strides:4;
	uint32_t two_byte_shift_en:1;
	uint32_t single_stride_log_num_of_bytes:3;
	uint32_t dbr_umem_id;
	uint32_t wq_umem_id;
	uint64_t wq_umem_offset;
};

/* Create RQ attributes structure, used by create RQ operation. */
struct mlx5_devx_create_rq_attr {
	uint32_t rlky:1;
	uint32_t delay_drop_en:1;
	uint32_t scatter_fcs:1;
	uint32_t vsd:1;
	uint32_t mem_rq_type:4;
	uint32_t state:4;
	uint32_t flush_in_error_en:1;
	uint32_t hairpin:1;
	uint32_t ts_format:2;
	uint32_t user_index:24;
	uint32_t cqn:24;
	uint32_t counter_set_id:8;
	uint32_t rmpn:24;
	struct mlx5_devx_wq_attr wq_attr;
};

/* Modify RQ attributes structure, used by modify RQ operation. */
struct mlx5_devx_modify_rq_attr {
	uint32_t rqn:24;
	uint32_t rq_state:4; /* Current RQ state. */
	uint32_t state:4; /* Required RQ state. */
	uint32_t scatter_fcs:1;
	uint32_t vsd:1;
	uint32_t counter_set_id:8;
	uint32_t hairpin_peer_sq:24;
	uint32_t hairpin_peer_vhca:16;
	uint64_t modify_bitmask;
	uint32_t lwm:16; /* Contained WQ lwm. */
};

/* SQ attributes structure, used by SQ create operation. */
struct mlx5_devx_create_sq_attr {
	uint32_t rlky:1;
	uint32_t cd_master:1;
	uint32_t fre:1;
	uint32_t flush_in_error_en:1;
	uint32_t allow_multi_pkt_send_wqe:1;
	uint32_t min_wqe_inline_mode:3;
	uint32_t state:4;
	uint32_t reg_umr:1;
	uint32_t allow_swp:1;
	uint32_t hairpin:1;
	uint32_t non_wire:1;
	uint32_t static_sq_wq:1;
	uint32_t ts_format:2;
	uint32_t user_index:24;
	uint32_t cqn:24;
	uint32_t packet_pacing_rate_limit_index:16;
	uint32_t tis_lst_sz:16;
	uint32_t tis_num:24;
	struct mlx5_devx_wq_attr wq_attr;
};

/* SQ attributes structure, used by SQ modify operation. */
struct mlx5_devx_modify_sq_attr {
	uint32_t sq_state:4;
	uint32_t state:4;
	uint32_t hairpin_peer_rq:24;
	uint32_t hairpin_peer_vhca:16;
};


/* CQ attributes structure, used by CQ operations. */
struct mlx5_devx_cq_attr {
	uint32_t q_umem_valid:1;
	uint32_t db_umem_valid:1;
	uint32_t use_first_only:1;
	uint32_t overrun_ignore:1;
	uint32_t cqe_compression_en:1;
	uint32_t mini_cqe_res_format_1_0:2;
	uint32_t mini_cqe_res_format_3_2:2;
	uint32_t log_cq_size:5;
	uint32_t log_page_size:5;
	uint32_t uar_page_id;
	uint32_t q_umem_id;
	uint64_t q_umem_offset;
	uint32_t db_umem_id;
	uint64_t db_umem_offset;
	uint32_t eqn;
	uint64_t db_addr;
};

struct mlx5_devx_qp_attr {
	uint32_t pd:24;
	uint32_t uar_index:24;
	uint32_t cqn:24;
	uint32_t log_page_size:5;
	uint32_t num_of_receive_wqes:17; /* Must be power of 2. */
	uint32_t log_rq_stride:3;
	uint32_t num_of_send_wqbbs:17; /* Must be power of 2. */
	uint32_t ts_format:2;
	uint32_t dbr_umem_valid:1;
	uint32_t dbr_umem_id;
	uint64_t dbr_address;
	uint32_t wq_umem_id;
	uint64_t wq_umem_offset;
	uint32_t user_index:24;
	uint32_t mmo:1;
};

/* TIS attributes structure. */
struct mlx5_devx_tis_attr {
	uint32_t strict_lag_tx_port_affinity:1;
	uint32_t tls_en:1;
	uint32_t lag_tx_port_affinity:4;
	uint32_t prio:4;
	uint32_t transport_domain:24;
};

struct mlx5_ifc_query_tis_out_bits {
	u8 status[0x8];
	u8 reserved_at_8[0x18];
	u8 syndrome[0x20];
	u8 reserved_at_40[0x40];
	struct mlx5_ifc_tisc_bits tis_context;
};

struct mlx5_ifc_query_tis_in_bits {
	u8 opcode[0x10];
	u8 reserved_at_10[0x10];
	u8 reserved_at_20[0x10];
	u8 op_mod[0x10];
	u8 reserved_at_40[0x8];
	u8 tisn[0x18];
	u8 reserved_at_60[0x20];
};

enum {
	MLX5_MODIFY_RQ_IN_MODIFY_BITMASK_WQ_LWM = 1ULL << 0,
	MLX5_MODIFY_RQ_IN_MODIFY_BITMASK_VSD = 1ULL << 1,
	MLX5_MODIFY_RQ_IN_MODIFY_BITMASK_SCATTER_FCS = 1ULL << 2,
	MLX5_MODIFY_RQ_IN_MODIFY_BITMASK_RQ_COUNTER_SET_ID = 1ULL << 3,
};

enum mlx5_rxq_modify_type {
	MLX5_RXQ_MOD_ERR2RST, /* modify state from error to reset. */
	MLX5_RXQ_MOD_RST2RDY, /* modify state from reset to ready. */
	MLX5_RXQ_MOD_RDY2ERR, /* modify state from ready to error. */
	MLX5_RXQ_MOD_RDY2RST, /* modify state from ready to reset. */
};

enum mlx5_txq_modify_type {
	MLX5_TXQ_MOD_RST2RDY, /* modify state from reset to ready. */
	MLX5_TXQ_MOD_RDY2RST, /* modify state from ready to reset. */
	MLX5_TXQ_MOD_ERR2RDY, /* modify state from error to ready. */
};

#define MLX5_WAIT_COND_INVERT 0x10u
#define MLX5_WAIT_COND_ALWAYS_TRUE 0u
#define MLX5_WAIT_COND_EQUAL 1u
#define MLX5_WAIT_COND_BIGGER 2u
#define MLX5_WAIT_COND_SMALLER 3u
#define MLX5_WAIT_COND_CYCLIC_BIGGER 4u
#define MLX5_WAIT_COND_CYCLIC_SMALLER 5u

#ifdef __cplusplus
}
#endif

#endif /* MLX5_PRM_MANUAL_H_ */
