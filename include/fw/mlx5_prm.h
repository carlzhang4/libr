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

#ifndef MLX5_PRM_H
#define MLX5_PRM_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef u8
#define u8 uint8_t
#endif

enum {
	MLX5_EVENT_TYPE_COMP                                       = 0x0,
	MLX5_EVENT_TYPE_PATH_MIG                                   = 0x1,
	MLX5_EVENT_TYPE_COMM_EST                                   = 0x2,
	MLX5_EVENT_TYPE_SQ_DRAINED                                 = 0x3,
	MLX5_EVENT_TYPE_SRQ_LAST_WQE                               = 0x13,
	MLX5_EVENT_TYPE_SRQ_RQ_LIMIT                               = 0x14,
	MLX5_EVENT_TYPE_DCT_DRAINED                                = 0x1c,
	MLX5_EVENT_TYPE_DCT_KEY_VIOLATION                          = 0x1d,
	MLX5_EVENT_TYPE_CQ_ERROR                                   = 0x4,
	MLX5_EVENT_TYPE_WQ_CATAS_ERROR                             = 0x5,
	MLX5_EVENT_TYPE_PATH_MIG_FAILED                            = 0x7,
	MLX5_EVENT_TYPE_PAGE_FAULT                                 = 0xc,
	MLX5_EVENT_TYPE_WQ_INVAL_REQ_ERROR                         = 0x10,
	MLX5_EVENT_TYPE_WQ_ACCESS_ERROR                            = 0x11,
	MLX5_EVENT_TYPE_SRQ_CATAS_ERROR                            = 0x12,
	MLX5_EVENT_TYPE_CODING_OBJECT_CHANGE_EVENT                 = 0x27,
	MLX5_EVENT_TYPE_INTERNAL_ERROR                             = 0x8,
	MLX5_EVENT_TYPE_PORT_CHANGE                                = 0x9,
	MLX5_EVENT_TYPE_GPIO_EVENT                                 = 0x15,
	MLX5_EVENT_TYPE_CODING_PORT_MODULE_EVENT                   = 0x16,
	MLX5_EVENT_TYPE_CODING_TEMP_WARNING_EVENT                  = 0x17,
	MLX5_EVENT_TYPE_REMOTE_CONFIG                              = 0x19,
	MLX5_EVENT_TYPE_CODING_DCBX_CHANGE_EVENT                   = 0x1e,
	MLX5_EVENT_TYPE_CODING_PPS_EVENT                           = 0x25,
	MLX5_EVENT_TYPE_CODING_GENERAL_NOTIFICATION_EVENT          = 0x22,
	MLX5_EVENT_TYPE_CODING_MONITOR_COUNTER_CHANGE_EVENT        = 0x24,
	MLX5_EVENT_TYPE_DB_BF_CONGESTION                           = 0x1a,
	MLX5_EVENT_TYPE_STALL_EVENT                                = 0x1b,
	MLX5_EVENT_TYPE_DROPPED_PACKET_LOGGED_EVENT                = 0x1f,
	MLX5_EVENT_TYPE_CODING_RESOURCE_DUMP_EVENT                 = 0x28,
	MLX5_EVENT_TYPE_CODING_DEVICE_TRACER_EVENT                 = 0x26,
	MLX5_EVENT_TYPE_CMD                                        = 0xa,
	MLX5_EVENT_TYPE_PAGE_REQUEST                               = 0xb,
	MLX5_EVENT_TYPE_NIC_VPORT_CHANGE                           = 0xd,
	MLX5_EVENT_TYPE_CODING_HOST_NET_FUNCTIONS_CHANGED          = 0xe,
	MLX5_EVENT_TYPE_CODING_VHCA_STATE_CHANGE                   = 0xf,
	MLX5_EVENT_TYPE_CODING_DRIVER_VERSION_CHANGE               = 0x29,
	MLX5_EVENT_TYPE_CODING_FPGA_ERROR                          = 0x20,
	MLX5_EVENT_TYPE_CODING_FPGA_QP_ERROR                       = 0x21,
	MLX5_EVENT_TYPE_CODING_XRQ_ERROR                           = 0x18
};

enum {
	MLX5_IPSEC_SYNDROME_IPSEC_OK    = 0x0,
	MLX5_IPSEC_SYNDROME_ICV_FAIL    = 0x1,
	MLX5_IPSEC_SYNDROME_BAD_TRAILER = 0x2
};

enum {
	MLX5_IPSEC_ASO_SYNDROME_ASO_OK        = 0x0,
	MLX5_IPSEC_ASO_SYNDROME_BAD_REPLAY    = 0x1,
	MLX5_IPSEC_ASO_SYNDROME_HARD_LIFETIME = 0x2
};

enum {
	MLX5_MACSEC_SYNDROME_MACSEC_OK   = 0x0,
	MLX5_MACSEC_SYNDROME_ICV_FAIL    = 0x1,
	MLX5_MACSEC_SYNDROME_BAD_TRAILER = 0x2
};

enum {
	MLX5_MACSEC_ASO_SYNDROME_ASO_OK        = 0x0,
	MLX5_MACSEC_ASO_SYNDROME_BAD_REPLAY    = 0x1,
	MLX5_MACSEC_ASO_SYNDROME_HARD_LIFETIME = 0x2
};

enum {
	MLX5_CMD_OP_QUERY_HCA_CAP                              = 0x100,
	MLX5_CMD_OP_QUERY_ADAPTER                              = 0x101,
	MLX5_CMD_OP_INIT_HCA                                   = 0x102,
	MLX5_CMD_OP_TEARDOWN_HCA                               = 0x103,
	MLX5_CMD_OP_ENABLE_HCA                                 = 0x104,
	MLX5_CMD_OP_DISABLE_HCA                                = 0x105,
	MLX5_CMD_OP_QUERY_PAGES                                = 0x107,
	MLX5_CMD_OP_MANAGE_PAGES                               = 0x108,
	MLX5_CMD_OP_SET_HCA_CAP                                = 0x109,
	MLX5_CMD_OP_QUERY_ISSI                                 = 0x10a,
	MLX5_CMD_OP_SET_ISSI                                   = 0x10b,
	MLX5_CMD_OP_QUERY_DRIVER_VERSION                       = 0x10c,
	MLX5_CMD_OP_SET_DRIVER_VERSION                         = 0x10d,
	MLX5_CMD_OP_QUERY_OTHER_HCA_CAP                        = 0x10e,
	MLX5_CMD_OP_MODIFY_OTHER_HCA_CAP                       = 0x10f,
	MLX5_CMD_OP_SET_TUNNELED_OPERATIONS                    = 0x110,
	MLX5_CMD_OP_QUERY_SF_PARTITIONS                        = 0x111,
	MLX5_CMD_OP_SET_SF_PARTITIONS                          = 0x112,
	MLX5_CMD_OP_ALLOC_SF                                   = 0x113,
	MLX5_CMD_OP_DEALLOC_SF                                 = 0x114,
	MLX5_CMD_OP_SUSPEND_VHCA                               = 0x115,
	MLX5_CMD_OP_RESUME_VHCA                                = 0x116,
	MLX5_CMD_OP_QUERY_VHCA_MIGRATION_STATE                 = 0x117,
	MLX5_CMD_OP_SAVE_VHCA_STATE                            = 0x118,
	MLX5_CMD_OP_LOAD_VHCA_STATE                            = 0x119,
	MLX5_CMD_OP_SYNC_DRIVER_VERSION                        = 0x11a,
	MLX5_CMD_OP_CREATE_MKEY                                = 0x200,
	MLX5_CMD_OP_QUERY_MKEY                                 = 0x201,
	MLX5_CMD_OP_DESTROY_MKEY                               = 0x202,
	MLX5_CMD_OP_QUERY_SPECIAL_CONTEXTS                     = 0x203,
	MLX5_CMD_OP_PAGE_FAULT_RESUME                          = 0x204,
	MLX5_CMD_OP_ALLOC_MEMIC                                = 0x205,
	MLX5_CMD_OP_DEALLOC_MEMIC                              = 0x206,
	MLX5_CMD_OP_MODIFY_MEMIC                               = 0x207,
	MLX5_CMD_OP_CREATE_EQ                                  = 0x301,
	MLX5_CMD_OP_DESTROY_EQ                                 = 0x302,
	MLX5_CMD_OP_QUERY_EQ                                   = 0x303,
	MLX5_CMD_OP_GEN_EQE                                    = 0x304,
	MLX5_CMD_OP_CREATE_CQ                                  = 0x400,
	MLX5_CMD_OP_DESTROY_CQ                                 = 0x401,
	MLX5_CMD_OP_QUERY_CQ                                   = 0x402,
	MLX5_CMD_OP_MODIFY_CQ                                  = 0x403,
	MLX5_CMD_OP_CREATE_QP                                  = 0x500,
	MLX5_CMD_OP_DESTROY_QP                                 = 0x501,
	MLX5_CMD_OP_RST2INIT_QP                                = 0x502,
	MLX5_CMD_OP_INIT2RTR_QP                                = 0x503,
	MLX5_CMD_OP_RTR2RTS_QP                                 = 0x504,
	MLX5_CMD_OP_RTS2RTS_QP                                 = 0x505,
	MLX5_CMD_OP_SQERR2RTS_QP                               = 0x506,
	MLX5_CMD_OP_2ERR_QP                                    = 0x507,
	MLX5_CMD_OP_2RST_QP                                    = 0x50a,
	MLX5_CMD_OP_QUERY_QP                                   = 0x50b,
	MLX5_CMD_OP_SQD2RTS_QP                                 = 0x50c,
	MLX5_CMD_OP_INIT2INIT_QP                               = 0x50e,
	MLX5_CMD_OP_SUSPEND_QP                                 = 0x50f,
	MLX5_CMD_OP_RESUME_QP                                  = 0x510,
	MLX5_CMD_OP_SET_RDB                                    = 0x511,
	MLX5_CMD_OP_QUERY_RDB                                  = 0x512,
	MLX5_CMD_OP_CREATE_PSV                                 = 0x600,
	MLX5_CMD_OP_DESTROY_PSV                                = 0x601,
	MLX5_CMD_OP_QUERY_PSV                                  = 0x602,
	MLX5_CMD_OP_CREATE_SRQ                                 = 0x700,
	MLX5_CMD_OP_DESTROY_SRQ                                = 0x701,
	MLX5_CMD_OP_QUERY_SRQ                                  = 0x702,
	MLX5_CMD_OP_ARM_RQ                                     = 0x703,
	MLX5_CMD_OP_RESIZE_SRQ                                 = 0x704,
	MLX5_CMD_OP_CREATE_XRC_SRQ                             = 0x705,
	MLX5_CMD_OP_DESTROY_XRC_SRQ                            = 0x706,
	MLX5_CMD_OP_QUERY_XRC_SRQ                              = 0x707,
	MLX5_CMD_OP_ARM_XRC_SRQ                                = 0x708,
	MLX5_CMD_OP_CREATE_DCT_LAG                             = 0x709,
	MLX5_CMD_OP_CREATE_DCT                                 = 0x710,
	MLX5_CMD_OP_DESTROY_DCT                                = 0x711,
	MLX5_CMD_OP_DRAIN_DCT                                  = 0x712,
	MLX5_CMD_OP_QUERY_DCT                                  = 0x713,
	MLX5_CMD_OP_ARM_DCT_FOR_KEY_VIOLATION                  = 0x714,
	MLX5_CMD_OP_SET_DC_CNAK_TRACE                          = 0x715,
	MLX5_CMD_OP_QUERY_DC_CNAK_TRACE                        = 0x716,
	MLX5_CMD_OP_CREATE_XRQ                                 = 0x717,
	MLX5_CMD_OP_DESTROY_XRQ                                = 0x718,
	MLX5_CMD_OP_QUERY_XRQ                                  = 0x719,
	MLX5_CMD_OP_CREATE_NVMF_BACKEND_CONTROLLER             = 0x720,
	MLX5_CMD_OP_DESTROY_NVMF_BACKEND_CONTROLLER            = 0x721,
	MLX5_CMD_OP_QUERY_NVMF_BACKEND_CONTROLLER              = 0x722,
	MLX5_CMD_OP_ATTACH_NVMF_NAMESPACE                      = 0x723,
	MLX5_CMD_OP_DETACH_NVMF_NAMESPACE                      = 0x724,
	MLX5_CMD_OP_QUERY_XRQ_DC_PARAMS_ENTRY                  = 0x725,
	MLX5_CMD_OP_SET_XRQ_DC_PARAMS_ENTRY                    = 0x726,
	MLX5_CMD_OP_QUERY_XRQ_ERROR_PARAMS                     = 0x727,
	MLX5_CMD_OP_QUERY_NVMF_NAMESPACE                       = 0x728,
	MLX5_CMD_OP_RELEASE_XRQ_ERROR                          = 0x729,
	MLX5_CMD_OP_MODIFY_XRQ                                 = 0x72a,
	MLX5_CMD_OPCODE_QUERY_HOST_NET_FUNCTIONS               = 0x740,
	MLX5_CMD_OP_QUERY_VPORT_STATE                          = 0x750,
	MLX5_CMD_OP_MODIFY_VPORT_STATE                         = 0x751,
	MLX5_CMD_OP_QUERY_ESW_VPORT_CONTEXT                    = 0x752,
	MLX5_CMD_OP_MODIFY_ESW_VPORT_CONTEXT                   = 0x753,
	MLX5_CMD_OP_QUERY_NIC_VPORT_CONTEXT                    = 0x754,
	MLX5_CMD_OP_MODIFY_NIC_VPORT_CONTEXT                   = 0x755,
	MLX5_CMD_OP_QUERY_ROCE_ADDRESS                         = 0x760,
	MLX5_CMD_OP_SET_ROCE_ADDRESS                           = 0x761,
	MLX5_CMD_OP_QUERY_HCA_VPORT_CONTEXT                    = 0x762,
	MLX5_CMD_OP_MODIFY_HCA_VPORT_CONTEXT                   = 0x763,
	MLX5_CMD_OP_QUERY_HCA_VPORT_GID                        = 0x764,
	MLX5_CMD_OP_QUERY_HCA_VPORT_PKEY                       = 0x765,
	MLX5_CMD_OP_QUERY_VNIC_ENV                             = 0x76f,
	MLX5_CMD_OP_QUERY_VPORT_COUNTER                        = 0x770,
	MLX5_CMD_OP_ALLOC_Q_COUNTER                            = 0x771,
	MLX5_CMD_OP_DEALLOC_Q_COUNTER                          = 0x772,
	MLX5_CMD_OP_QUERY_Q_COUNTER                            = 0x773,
	MLX5_CMD_OP_SET_MONITOR_COUNTER                        = 0x774,
	MLX5_CMD_OP_ARM_MONITOR_COUNTER                        = 0x775,
	MLX5_CMD_OP_SET_PP_RATE_LIMIT                          = 0x780,
	MLX5_CMD_OP_QUERY_PP_RATE_LIMIT                        = 0x781,
	MLX5_CMD_OP_CREATE_SCHEDULING_ELEMENT                  = 0x782,
	MLX5_CMD_OP_DESTROY_SCHEDULING_ELEMENT                 = 0x783,
	MLX5_CMD_OP_QUERY_SCHEDULING_ELEMENT                   = 0x784,
	MLX5_CMD_OP_MODIFY_SCHEDULING_ELEMENT                  = 0x785,
	MLX5_CMD_OP_CREATE_QOS_PARA_VPORT                      = 0x786,
	MLX5_CMD_OP_DESTROY_QOS_PARA_VPORT                     = 0x787,
	MLX5_CMD_OP_ALLOC_PD                                   = 0x800,
	MLX5_CMD_OP_DEALLOC_PD                                 = 0x801,
	MLX5_CMD_OP_ALLOC_UAR                                  = 0x802,
	MLX5_CMD_OP_DEALLOC_UAR                                = 0x803,
	MLX5_CMD_OP_CONFIG_INT_MODERATION                      = 0x804,
	MLX5_CMD_OP_ACCESS_REGISTER                            = 0x805,
	MLX5_CMD_OP_ATTACH_TO_MCG                              = 0x806,
	MLX5_CMD_OP_DETACH_FROM_MCG                            = 0x807,
	MLX5_CMD_OP_ATTACH_TO_SNIFFER                          = 0x808,
	MLX5_CMD_OP_DETACH_FROM_SNIFFER                        = 0x809,
	MLX5_CMD_OP_GET_DROPPED_PACKET_LOG                     = 0x80a,
	MLX5_CMD_OP_MAD_IFC                                    = 0x50d,
	MLX5_CMD_OP_QUERY_MAD_DEMUX                            = 0x80b,
	MLX5_CMD_OP_SET_MAD_DEMUX                              = 0x80c,
	MLX5_CMD_OP_NOP                                        = 0x80d,
	MLX5_CMD_OP_ALLOC_XRCD                                 = 0x80e,
	MLX5_CMD_OP_DEALLOC_XRCD                               = 0x80f,
	MLX5_CMD_OP_SET_BURST_SIZE                             = 0x812,
	MLX5_CMD_OP_QUERY_BURST_SIZE                           = 0x813,
	MLX5_CMD_OP_ACTIVATE_TRACER                            = 0x814,
	MLX5_CMD_OP_DEACTIVATE_TRACER                          = 0x815,
	MLX5_CMD_OP_ALLOC_TRANSPORT_DOMAIN                     = 0x816,
	MLX5_CMD_OP_DEALLOC_TRANSPORT_DOMAIN                   = 0x817,
	MLX5_CMD_OP_QUERY_DIAGNOSTIC_PARAMS                    = 0x819,
	MLX5_CMD_OP_SET_DIAGNOSTIC_PARAMS                      = 0x820,
	MLX5_CMD_OP_QUERY_DIAGNOSTIC_CNTRS                     = 0x821,
	MLX5_CMD_OP_QUERY_CONG_STATUS                          = 0x822,
	MLX5_CMD_OP_MODIFY_CONG_STATUS                         = 0x823,
	MLX5_CMD_OP_QUERY_CONG_PARAMS                          = 0x824,
	MLX5_CMD_OP_MODIFY_CONG_PARAMS                         = 0x825,
	MLX5_CMD_OP_QUERY_CONG_STATISTICS                      = 0x826,
	MLX5_CMD_OP_ADD_VXLAN_UDP_DPORT                        = 0x827,
	MLX5_CMD_OP_DELETE_VXLAN_UDP_DPORT                     = 0x828,
	MLX5_CMD_OP_SET_L2_TABLE_ENTRY                         = 0x829,
	MLX5_CMD_OP_QUERY_L2_TABLE_ENTRY                       = 0x82a,
	MLX5_CMD_OP_DELETE_L2_TABLE_ENTRY                      = 0x82b,
	MLX5_CMD_OP_SET_WOL_ROL                                = 0x830,
	MLX5_CMD_OP_QUERY_WOL_ROL                              = 0x831,
	MLX5_CMD_OP_ARM_RESOURCE_DUMP_EVENT                    = 0x832,
	MLX5_CMD_OP_CREATE_LAG                                 = 0x840,
	MLX5_CMD_OP_MODIFY_LAG                                 = 0x841,
	MLX5_CMD_OP_QUERY_LAG                                  = 0x842,
	MLX5_CMD_OP_DESTROY_LAG                                = 0x843,
	MLX5_CMD_OP_CREATE_VPORT_LAG                           = 0x844,
	MLX5_CMD_OP_DESTROY_VPORT_LAG                          = 0x845,
	MLX5_CMD_OP_CREATE_TIR                                 = 0x900,
	MLX5_CMD_OP_MODIFY_TIR                                 = 0x901,
	MLX5_CMD_OP_DESTROY_TIR                                = 0x902,
	MLX5_CMD_OP_QUERY_TIR                                  = 0x903,
	MLX5_CMD_OP_CREATE_SQ                                  = 0x904,
	MLX5_CMD_OP_MODIFY_SQ                                  = 0x905,
	MLX5_CMD_OP_DESTROY_SQ                                 = 0x906,
	MLX5_CMD_OP_QUERY_SQ                                   = 0x907,
	MLX5_CMD_OP_CREATE_RQ                                  = 0x908,
	MLX5_CMD_OP_MODIFY_RQ                                  = 0x909,
	MLX5_CMD_OP_DESTROY_RQ                                 = 0x90a,
	MLX5_CMD_OP_QUERY_RQ                                   = 0x90b,
	MLX5_CMD_OP_CREATE_RMP                                 = 0x90c,
	MLX5_CMD_OP_MODIFY_RMP                                 = 0x90d,
	MLX5_CMD_OP_DESTROY_RMP                                = 0x90e,
	MLX5_CMD_OP_QUERY_RMP                                  = 0x90f,
	MLX5_CMD_OP_SET_DELAY_DROP_PARAMS                      = 0x910,
	MLX5_CMD_OP_QUERY_DELAY_DROP_PARAMS                    = 0x911,
	MLX5_CMD_OP_CREATE_TIS                                 = 0x912,
	MLX5_CMD_OP_MODIFY_TIS                                 = 0x913,
	MLX5_CMD_OP_DESTROY_TIS                                = 0x914,
	MLX5_CMD_OP_QUERY_TIS                                  = 0x915,
	MLX5_CMD_OP_CREATE_RQT                                 = 0x916,
	MLX5_CMD_OP_MODIFY_RQT                                 = 0x917,
	MLX5_CMD_OP_DESTROY_RQT                                = 0x918,
	MLX5_CMD_OP_QUERY_RQT                                  = 0x919,
	MLX5_CMD_OP_SET_FLOW_TABLE_ROOT                        = 0x92f,
	MLX5_CMD_OP_CREATE_FLOW_TABLE                          = 0x930,
	MLX5_CMD_OP_DESTROY_FLOW_TABLE                         = 0x931,
	MLX5_CMD_OP_QUERY_FLOW_TABLE                           = 0x932,
	MLX5_CMD_OP_CREATE_FLOW_GROUP                          = 0x933,
	MLX5_CMD_OP_DESTROY_FLOW_GROUP                         = 0x934,
	MLX5_CMD_OP_QUERY_FLOW_GROUP                           = 0x935,
	MLX5_CMD_OP_SET_FLOW_TABLE_ENTRY                       = 0x936,
	MLX5_CMD_OP_QUERY_FLOW_TABLE_ENTRY                     = 0x937,
	MLX5_CMD_OP_DELETE_FLOW_TABLE_ENTRY                    = 0x938,
	MLX5_CMD_OP_ALLOC_FLOW_COUNTER                         = 0x939,
	MLX5_CMD_OP_DEALLOC_FLOW_COUNTER                       = 0x93a,
	MLX5_CMD_OP_QUERY_FLOW_COUNTER                         = 0x93b,
	MLX5_CMD_OP_MODIFY_FLOW_TABLE                          = 0x93c,
	MLX5_CMD_OP_ALLOC_PACKET_REFORMAT_CONTEXT              = 0x93d,
	MLX5_CMD_OP_DEALLOC_PACKET_REFORMAT_CONTEXT            = 0x93e,
	MLX5_CMD_OP_QUERY_PACKET_REFORMAT_CONTEXT              = 0x93f,
	MLX5_CMD_OP_ALLOC_MODIFY_HEADER_CONTEXT                = 0x940,
	MLX5_CMD_OP_DEALLOC_MODIFY_HEADER_CONTEXT              = 0x941,
	MLX5_CMD_OP_QUERY_MODIFY_HEADER_CONTEXT                = 0x942,
	MLX5_CMD_OP_MUST_NOT_USE                               = 0x943,
	MLX5_CMD_OP_FPGA_CREATE_QP                             = 0x960,
	MLX5_CMD_OP_FPGA_MODIFY_QP                             = 0x961,
	MLX5_CMD_OP_FPGA_QUERY_QP                              = 0x962,
	MLX5_CMD_OP_FPGA_DESTROY_QP                            = 0x963,
	MLX5_CMD_OP_FPGA_QUERY_QP_COUNTERS                     = 0x964,
	MLX5_CMD_OP_CREATE_AFU                                 = 0x970,
	MLX5_CMD_OP_QUERY_AFU                                  = 0x971,
	MLX5_CMD_OP_DESTROY_AFU                                = 0x972,
	MLX5_CMD_OP_CREATE_CAPI_PEC                            = 0x980,
	MLX5_CMD_OP_QUERY_CAPI_PEC                             = 0x981,
	MLX5_CMD_OP_DESTROY_CAPI_PEC                           = 0x982,
	MLX5_CMD_OP_CREATE_GENERAL_OBJECT                      = 0xa00,
	MLX5_CMD_OP_MODIFY_GENERAL_OBJECT                      = 0xa01,
	MLX5_CMD_OP_QUERY_GENERAL_OBJECT                       = 0xa02,
	MLX5_CMD_OP_DESTROY_GENERAL_OBJECT                     = 0xa03,
	MLX5_CMD_OP_CREATE_UCTX                                = 0xa04,
	MLX5_CMD_OP_QUERY_UCTX                                 = 0xa05,
	MLX5_CMD_OP_DESTROY_UCTX                               = 0xa06,
	MLX5_CMD_OP_CREATE_UMEM                                = 0xa08,
	MLX5_CMD_OP_QUERY_UMEM                                 = 0xa09,
	MLX5_CMD_OP_DESTROY_UMEM                               = 0xa0a,
	MLX5_CMD_OP_SYNC_STEERING                              = 0xb00,
	MLX5_CMD_OP_SEND_NVMF_CC                               = 0xb01,
	MLX5_CMD_OP_QUERY_NVMF_CC_RESPONSE                     = 0xb02,
	MLX5_CMD_OP_QUERY_EMULATED_FUNCTIONS_INFO              = 0xb03,
	MLX5_CMD_OP_SET_REGEXP_PARAMS                          = 0xb04,
	MLX5_CMD_OP_QUERY_REGEXP_PARAMS                        = 0xb05,
	MLX5_CMD_OP_USER_QUERY_XRQ_DC_PARAMS_ENTRY             = 0xb08,
	MLX5_CMD_OP_USER_SET_XRQ_DC_PARAMS_ENTRY               = 0xb09,
	MLX5_CMD_OP_USER_QUERY_XRQ_ERROR_PARAMS                = 0xb0a,
	MLX5_CMD_OP_USER_RELEASE_XRQ_ERROR                     = 0xb0b,
	MLX5_CMD_OP_ACCESS_REGISTER_USER                       = 0xb0c,
	MLX5_CMD_OP_QUERY_VHCA_STATE                           = 0xb0d,
	MLX5_CMD_OP_MODIFY_VHCA_STATE                          = 0xb0e,
	MLX5_CMD_OP_QUERY_EMULATION_DEVICE_EQ_MSIX_MAPPING     = 0xb0f,
	MLX5_CMD_OP_NISP_GEN_SPI                               = 0xb10,
	MLX5_CMD_OP_NISP_ROTATE_KEY                            = 0xb11,
	MLX5_CMD_OP_SYNC_CRYPTO                                = 0xb12,
	MLX5_CMD_OP_ALLOW_OTHER_VHCA_ACCESS                    = 0xb16,
	MLX5_CMD_OP_GENERATE_WQE                               = 0xb17,
	MLX5_CMD_OP_HOTPLUG_DEVICE                             = 0xb20,
	MLX5_CMD_OP_HOTUNPLUG_DEVICE                           = 0xb21,
	MLX5_CMD_OP_QUERY_VUID                                 = 0xb22,
	MLX5_CMD_OP_HOTPLUG_PASSTHROUGH_DEVICE                 = 0xb23,
	MLX5_CMD_OP_HOTUNPLUG_PASSTHROUGH_DEVICE               = 0xb24,
	MLX5_CMD_OP_CREATE_DPA_PARTITION                       = 0xb26,
	MLX5_CMD_OP_DESTROY_DPA_PARTITION                      = 0xb27,
	MLX5_CMD_OP_QUERY_DPA_PARTITION                        = 0xb28,
	MLX5_CMD_OP_MODIFY_DPA_PARTITION                       = 0xb29,
	MLX5_CMD_OP_QUERY_DPA_PARTITIONS                       = 0xb2a,
	MLX5_CMD_OP_DPA_PROCESS_COREDUMP                       = 0xb2b,
	MLX5_CMD_OP_MANAGE_ENH_STRWQ                           = 0xb2c,
	MLX5_CMD_OP_QUERY_RSV_RESOURCES                        = 0x8000,
	MLX5_CMD_OP_QUERY_MTT                                  = 0x8001,
	MLX5_CMD_OP_CREATE_MTT                                 = 0x8002,
	MLX5_CMD_OP_DESTROY_MTT                                = 0x8003,
	MLX5_CMD_OP_ACCESS_CR                                  = 0x8004,
	MLX5_CMD_OP_MODIFY_SCHED_QUEUE                         = 0x8005,
	MLX5_CMD_OP_QUERY_SCHED_QUEUE                          = 0x8006
};

enum {
	MLX5_GEN_OBJ_TYPE_NVME_DEVICE_EMULATION       = 0x6,
	MLX5_GEN_OBJ_TYPE_NVME_SQ                     = 0x7,
	MLX5_GEN_OBJ_TYPE_SW_ICM_BLOCK                = 0x8,
	MLX5_GEN_OBJ_TYPE_NVME_CQ                     = 0x9,
	MLX5_GEN_OBJ_TYPE_FLOW_METER                  = 0xa,
	MLX5_GEN_OBJ_TYPE_GENEVE_TLV_OPTION           = 0xb,
	MLX5_GEN_OBJ_TYPE_DEK                         = 0xc,
	MLX5_GEN_OBJ_TYPE_VIRTIO_NET_Q                = 0xd,
	MLX5_GEN_OBJ_TYPE_VHCA_TUNNEL_OBJECT          = 0xe,
	MLX5_GEN_OBJ_TYPE_NVME_NAMESPACE              = 0xf,
	MLX5_GEN_OBJ_TYPE_NVME_CTRL_COUNTERS          = 0x10,
	MLX5_GEN_OBJ_TYPE_NVME_NAMESPACE_COUNTERS     = 0x11,
	MLX5_GEN_OBJ_TYPE_NVME_SQ_COUNTERS            = 0x12,
	MLX5_GEN_OBJ_TYPE_IPSEC_OFFLOAD               = 0x13,
	MLX5_GEN_OBJ_TYPE_DEVICE_OBJECT               = 0x14,
	MLX5_GEN_OBJ_TYPE_VIRTIO_NET_DEVICE_EMULATION = 0x15,
	MLX5_GEN_OBJ_TYPE_VIRTIO_BLK_DEVICE_EMULATION = 0x16,
	MLX5_GEN_OBJ_TYPE_VIRTIO_BLK_Q                = 0x17,
	MLX5_GEN_OBJ_TYPE_MATCH_DEFINER               = 0x18,
	MLX5_GEN_OBJ_TYPE_NVMF_NAMESPACE              = 0x1a,
	MLX5_GEN_OBJ_TYPE_NVMF_NAMESPACE_COUNTER_SET  = 0x1b,
	MLX5_GEN_OBJ_TYPE_VIRTIO_Q_COUNTERS           = 0x1c,
	MLX5_GEN_OBJ_TYPE_IMPORT_KEK                  = 0x1d,
	MLX5_GEN_OBJ_TYPE_CREDENTIAL                  = 0x1e,
	MLX5_GEN_OBJ_TYPE_CRYPTO_LOGIN                = 0x1f,
	MLX5_GEN_OBJ_TYPE_FLOW_SAMPLER                = 0x20,
	MLX5_GEN_OBJ_TYPE_NVMEOTCP_TAG_BUFFER_TABLE   = 0x21,
	MLX5_GEN_OBJ_TYPE_PARSE_GRAPH_NODE            = 0x22,
	MLX5_GEN_OBJ_TYPE_HEADER_MODIFY_ARGUMENT      = 0x23,
	MLX5_GEN_OBJ_TYPE_FLOW_METER_ASO              = 0x24,
	MLX5_GEN_OBJ_TYPE_FLOW_HIT_ASO                = 0x25,
	MLX5_GEN_OBJ_TYPE_SCHEDULING_ELEMENT          = 0x26,
	MLX5_GEN_OBJ_TYPE_MACSEC_OFFLOAD              = 0x27,
	MLX5_GEN_OBJ_TYPE_DPA_MEM                     = 0x28,
	MLX5_GEN_OBJ_TYPE_DPA_PROCESS                 = 0x2a,
	MLX5_GEN_OBJ_TYPE_DPA_THREAD                  = 0x2b,
	MLX5_GEN_OBJ_TYPE_RESERVED_QPN                = 0x2c,
	MLX5_GEN_OBJ_TYPE_DPA_OUTBOX                  = 0x2d,
	MLX5_GEN_OBJ_TYPE_UPT_DEVICE_EMULATION        = 0x2e,
	MLX5_GEN_OBJ_TYPE_UPT_SQ                      = 0x2f,
	MLX5_GEN_OBJ_TYPE_UPT_RQ                      = 0x30,
	MLX5_GEN_OBJ_TYPE_CONN_TRACK_OFFLOAD          = 0x31,
	MLX5_GEN_OBJ_TYPE_EMULATED_DEV_DB_CQ_MAPPING  = 0x32,
	MLX5_GEN_OBJ_TYPE_DPA_EQ                      = 0x33,
	MLX5_GEN_OBJ_TYPE_NVME_SQ_BE                  = 0x38,
	MLX5_GEN_OBJ_TYPE_DPA_WINDOW                  = 0x39,
	MLX5_GEN_OBJ_TYPE_AV_QP_MAPPING               = 0x3a,
	MLX5_GEN_OBJ_TYPE_CHANNEL_SERVICE             = 0x3b,
	MLX5_GEN_OBJ_TYPE_CHANNEL_CONNECTION          = 0x3c,
	MLX5_GEN_OBJ_TYPE_VIRTIO_FS_DEVICE_EMULATION  = 0x3d,
	MLX5_GEN_OBJ_TYPE_VIRTIO_FS_Q                 = 0x3e,
	MLX5_GEN_OBJ_TYPE_STC                         = 0x40,
	MLX5_GEN_OBJ_TYPE_RTC                         = 0x41,
	MLX5_GEN_OBJ_TYPE_STE                         = 0x42,
	MLX5_GEN_OBJ_TYPE_HEADER_MODIFY_PATTERN       = 0x43,
	MLX5_GEN_OBJ_TYPE_UPT_RQ_COUNTERS             = 0x44,
	MLX5_GEN_OBJ_TYPE_UPT_SQ_COUNTERS             = 0x45,
	MLX5_GEN_OBJ_TYPE_PAGE_TRACK                  = 0x46,
	MLX5_GEN_OBJ_TYPE_INT_KEK                     = 0x47,
	MLX5_GEN_OBJ_TYPE_NVMF_BACKEND_CONTROLLER     = 0x48,
	MLX5_GEN_OBJ_TYPE_EMULATED_DEV_EQ             = 0x49
};

enum {
	MLX5_MODIFY_FIELD_ID_OUT_SMAC_47_16       = 0x1,
	MLX5_MODIFY_FIELD_ID_OUT_SMAC_15_0        = 0x2,
	MLX5_MODIFY_FIELD_ID_OUT_ETHERTYPE        = 0x3,
	MLX5_MODIFY_FIELD_ID_OUT_DMAC_47_16       = 0x4,
	MLX5_MODIFY_FIELD_ID_OUT_DMAC_15_0        = 0x5,
	MLX5_MODIFY_FIELD_ID_OUT_IP_DSCP          = 0x6,
	MLX5_MODIFY_FIELD_ID_OUT_TCP_FLAGS        = 0x7,
	MLX5_MODIFY_FIELD_ID_OUT_TCP_SPORT        = 0x8,
	MLX5_MODIFY_FIELD_ID_OUT_TCP_DPORT        = 0x9,
	MLX5_MODIFY_FIELD_ID_OUT_IPV4_TTL         = 0xa,
	MLX5_MODIFY_FIELD_ID_OUT_UDP_SPORT        = 0xb,
	MLX5_MODIFY_FIELD_ID_OUT_UDP_DPORT        = 0xc,
	MLX5_MODIFY_FIELD_ID_OUT_SIPV6_127_96     = 0xd,
	MLX5_MODIFY_FIELD_ID_OUT_SIPV6_95_64      = 0xe,
	MLX5_MODIFY_FIELD_ID_OUT_SIPV6_63_32      = 0xf,
	MLX5_MODIFY_FIELD_ID_OUT_SIPV6_31_0       = 0x10,
	MLX5_MODIFY_FIELD_ID_OUT_DIPV6_127_96     = 0x11,
	MLX5_MODIFY_FIELD_ID_OUT_DIPV6_95_64      = 0x12,
	MLX5_MODIFY_FIELD_ID_OUT_DIPV6_63_32      = 0x13,
	MLX5_MODIFY_FIELD_ID_OUT_DIPV6_31_0       = 0x14,
	MLX5_MODIFY_FIELD_ID_OUT_SIPV4            = 0x15,
	MLX5_MODIFY_FIELD_ID_OUT_DIPV4            = 0x16,
	MLX5_MODIFY_FIELD_ID_OUT_FIRST_VID        = 0x17,
	MLX5_MODIFY_FIELD_ID_IN_SMAC_47_16        = 0x31,
	MLX5_MODIFY_FIELD_ID_IN_SMAC_15_0         = 0x32,
	MLX5_MODIFY_FIELD_ID_IN_ETHERTYPE         = 0x33,
	MLX5_MODIFY_FIELD_ID_IN_DMAC_47_16        = 0x34,
	MLX5_MODIFY_FIELD_ID_IN_DMAC_15_0         = 0x35,
	MLX5_MODIFY_FIELD_ID_IN_IP_DSCP           = 0x36,
	MLX5_MODIFY_FIELD_ID_IN_TCP_FLAGS         = 0x37,
	MLX5_MODIFY_FIELD_ID_IN_TCP_SPORT         = 0x38,
	MLX5_MODIFY_FIELD_ID_IN_TCP_DPORT         = 0x39,
	MLX5_MODIFY_FIELD_ID_IN_IPV4_TTL          = 0x3a,
	MLX5_MODIFY_FIELD_ID_IN_UDP_SPORT         = 0x3b,
	MLX5_MODIFY_FIELD_ID_IN_UDP_DPORT         = 0x3c,
	MLX5_MODIFY_FIELD_ID_IN_SIPV6_127_96      = 0x3d,
	MLX5_MODIFY_FIELD_ID_IN_SIPV6_95_64       = 0x3e,
	MLX5_MODIFY_FIELD_ID_IN_SIPV6_63_32       = 0x3f,
	MLX5_MODIFY_FIELD_ID_IN_SIPV6_31_0        = 0x40,
	MLX5_MODIFY_FIELD_ID_IN_DIPV6_127_96      = 0x41,
	MLX5_MODIFY_FIELD_ID_IN_DIPV6_95_64       = 0x42,
	MLX5_MODIFY_FIELD_ID_IN_DIPV6_63_32       = 0x43,
	MLX5_MODIFY_FIELD_ID_IN_DIPV6_31_0        = 0x44,
	MLX5_MODIFY_FIELD_ID_IN_SIPV4             = 0x45,
	MLX5_MODIFY_FIELD_ID_IN_DIPV4             = 0x46,
	MLX5_MODIFY_FIELD_ID_OUT_IPV6_HOPLIMIT    = 0x47,
	MLX5_MODIFY_FIELD_ID_IN_IPV6_HOPLIMIT     = 0x48,
	MLX5_MODIFY_FIELD_ID_METADATA_REG_A       = 0x49,
	MLX5_MODIFY_FIELD_ID_OUT_IP_PROTOCOL      = 0x4a,
	MLX5_MODIFY_FIELD_ID_METADATA_REG_B       = 0x50,
	MLX5_MODIFY_FIELD_ID_METADATA_REG_C_0     = 0x51,
	MLX5_MODIFY_FIELD_ID_METADATA_REG_C_1     = 0x52,
	MLX5_MODIFY_FIELD_ID_METADATA_REG_C_2     = 0x53,
	MLX5_MODIFY_FIELD_ID_METADATA_REG_C_3     = 0x54,
	MLX5_MODIFY_FIELD_ID_METADATA_REG_C_4     = 0x55,
	MLX5_MODIFY_FIELD_ID_METADATA_REG_C_5     = 0x56,
	MLX5_MODIFY_FIELD_ID_METADATA_REG_C_6     = 0x57,
	MLX5_MODIFY_FIELD_ID_METADATA_REG_C_7     = 0x58,
	MLX5_MODIFY_FIELD_ID_OUT_TCP_SEQ_NUM      = 0x59,
	MLX5_MODIFY_FIELD_ID_IN_TCP_SEQ_NUM       = 0x5a,
	MLX5_MODIFY_FIELD_ID_OUT_TCP_ACK_NUM      = 0x5b,
	MLX5_MODIFY_FIELD_ID_IN_TCP_ACK_NUM       = 0x5c,
	MLX5_MODIFY_FIELD_ID_IPSEC_SYNDROME       = 0x5d,
	MLX5_MODIFY_FIELD_ID_OUT_ESP_SPI          = 0x5e,
	MLX5_MODIFY_FIELD_ID_IN_ESP_SPI           = 0x5f,
	MLX5_MODIFY_FIELD_ID_LRH_SLID             = 0x60,
	MLX5_MODIFY_FIELD_ID_LRH_DLID             = 0x61,
	MLX5_MODIFY_FIELD_ID_GRH_FL               = 0x62,
	MLX5_MODIFY_FIELD_ID_GRH_TCLASS           = 0x63,
	MLX5_MODIFY_FIELD_ID_GRH_SGID_127_96      = 0x64,
	MLX5_MODIFY_FIELD_ID_GRH_SGID_95_64       = 0x65,
	MLX5_MODIFY_FIELD_ID_GRH_SGID_63_32       = 0x66,
	MLX5_MODIFY_FIELD_ID_GRH_SGID_31_0        = 0x67,
	MLX5_MODIFY_FIELD_ID_GRH_DGID_127_96      = 0x68,
	MLX5_MODIFY_FIELD_ID_GRH_DGID_95_64       = 0x69,
	MLX5_MODIFY_FIELD_ID_GRH_DGID_63_32       = 0x6a,
	MLX5_MODIFY_FIELD_ID_GRH_DGID_31_0        = 0x6b,
	MLX5_MODIFY_FIELD_ID_BTH_PKEY             = 0x6c,
	MLX5_MODIFY_FIELD_ID_BTH_DQPN             = 0x6d,
	MLX5_MODIFY_FIELD_ID_GTPU_TEID            = 0x6e,
	MLX5_MODIFY_FIELD_ID_OUT_EMD_TAG_DATA_0_1 = 0x6f,
	MLX5_MODIFY_FIELD_ID_OUT_EMD_TAG_DATA_2_5 = 0x70,
	MLX5_MODIFY_FIELD_ID_NISP_SYNDROME        = 0x71,
	MLX5_MODIFY_FIELD_ID_MACSEC_SYNDROME      = 0x72,
	MLX5_MODIFY_FIELD_ID_OUT_IP_ECN           = 0x73,
	MLX5_MODIFY_FIELD_ID_IN_IP_ECN            = 0x74,
	MLX5_MODIFY_FIELD_ID_TUNNEL_HDR_DW_1      = 0x75,
	MLX5_MODIFY_FIELD_ID_GTPU_FIRST_EXT_DW_0  = 0x76,
	MLX5_MODIFY_FIELD_ID_NISP_HEADER_0        = 0x77,
	MLX5_MODIFY_FIELD_ID_NISP_HEADER_1        = 0x78,
	MLX5_MODIFY_FIELD_ID_NISP_HEADER_2        = 0x79,
	MLX5_MODIFY_FIELD_ID_NISP_HEADER_3        = 0x7a,
	MLX5_MODIFY_FIELD_ID_NISP_HEADER_4        = 0x7b,
	MLX5_MODIFY_FIELD_ID_NISP_HEADER_5        = 0x7c,
	MLX5_MODIFY_FIELD_ID_NISP_HEADER_6        = 0x7d,
	MLX5_MODIFY_FIELD_ID_NISP_HEADER_7        = 0x7e,
	MLX5_MODIFY_FIELD_ID_NISP_HEADER_8        = 0x7f,
	MLX5_MODIFY_FIELD_ID_NISP_HEADER_9        = 0x80,
	MLX5_MODIFY_FIELD_ID_HASH_RESULT          = 0x81,
	MLX5_MODIFY_FIELD_ID_OUT_ESP_SEQ_NUM      = 0x82,
	MLX5_MODIFY_FIELD_ID_IN_ESP_SEQ_NUM       = 0x83,
	MLX5_MODIFY_FIELD_ID_IN_TUNNEL_HDR_DW_1   = 0x84,
	MLX5_MODIFY_FIELD_ID_OUT_MPLS_LABEL_0     = 0x85,
	MLX5_MODIFY_FIELD_ID_OUT_MPLS_LABEL_1     = 0x86,
	MLX5_MODIFY_FIELD_ID_OUT_MPLS_LABEL_2     = 0x87,
	MLX5_MODIFY_FIELD_ID_OUT_MPLS_LABEL_3     = 0x88,
	MLX5_MODIFY_FIELD_ID_OUT_MPLS_LABEL_4     = 0x89,
	MLX5_MODIFY_FIELD_ID_IN_MPLS_LABEL_0      = 0x8a,
	MLX5_MODIFY_FIELD_ID_IN_MPLS_LABEL_1      = 0x8b,
	MLX5_MODIFY_FIELD_ID_IN_MPLS_LABEL_2      = 0x8c,
	MLX5_MODIFY_FIELD_ID_IN_MPLS_LABEL_3      = 0x8d,
	MLX5_MODIFY_FIELD_ID_IN_MPLS_LABEL_4      = 0x8e,
	MLX5_MODIFY_FIELD_ID_IN2_SMAC_47_16       = 0x100,
	MLX5_MODIFY_FIELD_ID_IN2_SMAC_15_0        = 0x101,
	MLX5_MODIFY_FIELD_ID_IN2_ETHERTYPE        = 0x102,
	MLX5_MODIFY_FIELD_ID_IN2_DMAC_47_16       = 0x103,
	MLX5_MODIFY_FIELD_ID_IN2_DMAC_15_0        = 0x104,
	MLX5_MODIFY_FIELD_ID_IN2_IP_DSCP          = 0x105,
	MLX5_MODIFY_FIELD_ID_IN2_TCP_FLAGS        = 0x106,
	MLX5_MODIFY_FIELD_ID_IN2_TCP_SPORT        = 0x107,
	MLX5_MODIFY_FIELD_ID_IN2_TCP_DPORT        = 0x108,
	MLX5_MODIFY_FIELD_ID_IN2_IPV4_TTL         = 0x109,
	MLX5_MODIFY_FIELD_ID_IN2_UDP_SPORT        = 0x10a,
	MLX5_MODIFY_FIELD_ID_IN2_UDP_DPORT        = 0x10b,
	MLX5_MODIFY_FIELD_ID_IN2_SIPV6_127_96     = 0x10c,
	MLX5_MODIFY_FIELD_ID_IN2_SIPV6_95_64      = 0x10d,
	MLX5_MODIFY_FIELD_ID_IN2_SIPV6_63_32      = 0x10e,
	MLX5_MODIFY_FIELD_ID_IN2_SIPV6_31_0       = 0x10f,
	MLX5_MODIFY_FIELD_ID_IN2_DIPV6_127_96     = 0x110,
	MLX5_MODIFY_FIELD_ID_IN2_DIPV6_95_64      = 0x111,
	MLX5_MODIFY_FIELD_ID_IN2_DIPV6_63_32      = 0x112,
	MLX5_MODIFY_FIELD_ID_IN2_DIPV6_31_0       = 0x113,
	MLX5_MODIFY_FIELD_ID_IN2_SIPV4            = 0x114,
	MLX5_MODIFY_FIELD_ID_IN2_DIPV4            = 0x115,
	MLX5_MODIFY_FIELD_ID_IN2_IPV6_HOPLIMIT    = 0x116,
	MLX5_MODIFY_FIELD_ID_IN2_TCP_SEQ_NUM      = 0x117,
	MLX5_MODIFY_FIELD_ID_IN2_TCP_ACK_NUM      = 0x118,
	MLX5_MODIFY_FIELD_ID_IN2_ESP_SPI          = 0x119,
	MLX5_MODIFY_FIELD_ID_IN2_IP_ECN           = 0x11a,
	MLX5_MODIFY_FIELD_ID_IN2_ESP_SEQ_NUM      = 0x11b
};

enum {
	MLX5_ANCHOR_ID_PACKET_START            = 0x0,
	MLX5_ANCHOR_ID_MAC_START               = 0x1,
	MLX5_ANCHOR_ID_FIRST_VLAN              = 0x2,
	MLX5_ANCHOR_ID_SECOND_VLAN             = 0x3,
	MLX5_ANCHOR_ID_FIRST_CFG_ETHERTYPE     = 0x4,
	MLX5_ANCHOR_ID_SECOND_CFG_ETHERTYPE    = 0x5,
	MLX5_ANCHOR_ID_FIRST_MPLS              = 0x6,
	MLX5_ANCHOR_ID_IP_START                = 0x7,
	MLX5_ANCHOR_ID_ESP                     = 0x8,
	MLX5_ANCHOR_ID_TCP_UDP_START           = 0x9,
	MLX5_ANCHOR_ID_TUNNEL_HEADER_START     = 0xa,
	MLX5_ANCHOR_ID_FLEXPARSER0             = 0xb,
	MLX5_ANCHOR_ID_FLEXPARSER1             = 0xc,
	MLX5_ANCHOR_ID_FLEXPARSER2             = 0xd,
	MLX5_ANCHOR_ID_FLEXPARSER3             = 0xe,
	MLX5_ANCHOR_ID_FLEXPARSER4             = 0xf,
	MLX5_ANCHOR_ID_FLEXPARSER5             = 0x10,
	MLX5_ANCHOR_ID_FLEXPARSER6             = 0x11,
	MLX5_ANCHOR_ID_FLEXPARSER7             = 0x12,
	MLX5_ANCHOR_ID_IN_MAC_START            = 0x13,
	MLX5_ANCHOR_ID_IN_FIRST_VLAN           = 0x14,
	MLX5_ANCHOR_ID_IN_SECOND_VLAN          = 0x15,
	MLX5_ANCHOR_ID_IN_FIRST_CFG_ETHERTYPE  = 0x16,
	MLX5_ANCHOR_ID_IN_SECOND_CFG_ETHERTYPE = 0x17,
	MLX5_ANCHOR_ID_IN_FIRST_MPLS           = 0x18,
	MLX5_ANCHOR_ID_IN_IP_START             = 0x19,
	MLX5_ANCHOR_ID_IN_TCP_UDP_START        = 0x1a,
	MLX5_ANCHOR_ID_IN2_MAC_START           = 0x2d,
	MLX5_ANCHOR_ID_IN2_IP_START            = 0x33
};

enum {
	MLX5_ICMD_CMDS_OPCODE_ICMD_OPCODE_QUERY_FW_INFO      = 0x8007,
	MLX5_ICMD_CMDS_OPCODE_ICMD_QUERY_CAPABILITY          = 0x8400,
	MLX5_ICMD_CMDS_OPCODE_ICMD_MH_SYNC                   = 0x8402,
	MLX5_ICMD_CMDS_OPCODE_ICMD_MH_SYNC_STATUS            = 0x8403,
	MLX5_ICMD_CMDS_OPCODE_ICMD_GET_FRC_CHALLENGE         = 0x8404,
	MLX5_ICMD_CMDS_OPCODE_ICMD_SET_FRC                   = 0x8405,
	MLX5_ICMD_CMDS_OPCODE_ICMD_ACCESS_REGISTER           = 0x9001,
	MLX5_ICMD_CMDS_OPCODE_ICMD_QUERY_VIRTUAL_MAC         = 0x9003,
	MLX5_ICMD_CMDS_OPCODE_ICMD_SET_VIRTUAL_MAC           = 0x9004,
	MLX5_ICMD_CMDS_OPCODE_ICMD_QUERY_WOL_ROL             = 0x9005,
	MLX5_ICMD_CMDS_OPCODE_ICMD_SET_WOL_ROL               = 0x9006,
	MLX5_ICMD_CMDS_OPCODE_ICMD_OCBB_INIT                 = 0x9007,
	MLX5_ICMD_CMDS_OPCODE_ICMD_OCBB_QUERY_HEADER_STATUS  = 0x9008,
	MLX5_ICMD_CMDS_OPCODE_ICMD_OCBB_QUERY_ETOC_STATUS    = 0x9009,
	MLX5_ICMD_CMDS_OPCODE_ICMD_OCBB_SET_EVENT            = 0x900a,
	MLX5_ICMD_CMDS_OPCODE_ICMD_QUERY_VIRTUAL_PARAMETERS  = 0x900b,
	MLX5_ICMD_CMDS_OPCODE_ICMD_SET_VIRTUAL_PARAMETERS    = 0x900c,
	MLX5_ICMD_CMDS_OPCODE_ICMD_SET_DIAGNOSTIC_PARAMS     = 0x9020,
	MLX5_ICMD_CMDS_OPCODE_ICMD_QUERY_DIAGNOSTIC_PARAMS   = 0x9021,
	MLX5_ICMD_CMDS_OPCODE_ICMD_QUERY_DIAGNOSTIC_COUNTERS = 0x9022,
	MLX5_ICMD_CMDS_OPCODE_ICMD_OPCODE_INIT_OCSD          = 0xf004
};

struct mlx5_ifc_uint64_bits {
	u8         hi[0x20];

	u8         lo[0x20];
};

struct mlx5_ifc_flow_table_fields_supported_bits {
	u8         outer_dmac[0x1];
	u8         outer_smac[0x1];
	u8         outer_ether_type[0x1];
	u8         outer_ip_version[0x1];
	u8         outer_first_prio[0x1];
	u8         outer_first_cfi[0x1];
	u8         outer_first_vid[0x1];
	u8         outer_ipv4_ttl[0x1];
	u8         outer_second_prio[0x1];
	u8         outer_second_cfi[0x1];
	u8         outer_second_vid[0x1];
	u8         outer_ipv6_flow_label[0x1];
	u8         outer_sip[0x1];
	u8         outer_dip[0x1];
	u8         outer_frag[0x1];
	u8         outer_ip_protocol[0x1];
	u8         outer_ip_ecn[0x1];
	u8         outer_ip_dscp[0x1];
	u8         outer_udp_sport[0x1];
	u8         outer_udp_dport[0x1];
	u8         outer_tcp_sport[0x1];
	u8         outer_tcp_dport[0x1];
	u8         outer_tcp_flags[0x1];
	u8         outer_gre_protocol[0x1];
	u8         outer_gre_key[0x1];
	u8         outer_vxlan_vni[0x1];
	u8         outer_geneve_vni[0x1];
	u8         outer_geneve_oam[0x1];
	u8         outer_geneve_protocol_type[0x1];
	u8         outer_geneve_opt_len[0x1];
	u8         source_vhca_port[0x1];
	u8         source_eswitch_port[0x1];

	u8         inner_dmac[0x1];
	u8         inner_smac[0x1];
	u8         inner_ether_type[0x1];
	u8         inner_ip_version[0x1];
	u8         inner_first_prio[0x1];
	u8         inner_first_cfi[0x1];
	u8         inner_first_vid[0x1];
	u8         inner_ipv4_ttl[0x1];
	u8         inner_second_prio[0x1];
	u8         inner_second_cfi[0x1];
	u8         inner_second_vid[0x1];
	u8         inner_ipv6_flow_label[0x1];
	u8         inner_sip[0x1];
	u8         inner_dip[0x1];
	u8         inner_frag[0x1];
	u8         inner_ip_protocol[0x1];
	u8         inner_ip_ecn[0x1];
	u8         inner_ip_dscp[0x1];
	u8         inner_udp_sport[0x1];
	u8         inner_udp_dport[0x1];
	u8         inner_tcp_sport[0x1];
	u8         inner_tcp_dport[0x1];
	u8         inner_tcp_flags[0x1];
	u8         outer_tcp_seq_num[0x1];
	u8         inner_tcp_seq_num[0x1];
	u8         prog_sample_field[0x1];
	u8         outer_first_mpls_over_udp_cw[0x1];
	u8         outer_tcp_ack_num[0x1];
	u8         inner_tcp_ack_num[0x1];
	u8         outer_first_mpls_over_gre_cw[0x1];
	u8         metadata_reg_b[0x1];
	u8         metadata_reg_a[0x1];

	u8         geneve_tlv_option_0_data[0x1];
	u8         geneve_tlv_option_0_exist[0x1];
	u8         outer_vxlan_gpe_vni[0x1];
	u8         outer_vxlan_gpe_flags[0x1];
	u8         outer_vxlan_gpe_next_protocol[0x1];
	u8         outer_first_mpls_over_udp_ttl[0x1];
	u8         outer_first_mpls_over_udp_s_bos[0x1];
	u8         outer_first_mpls_over_udp_exp[0x1];
	u8         outer_first_mpls_over_udp_label[0x1];
	u8         outer_first_mpls_over_gre_ttl[0x1];
	u8         outer_first_mpls_over_gre_s_bos[0x1];
	u8         outer_first_mpls_over_gre_exp[0x1];
	u8         outer_first_mpls_over_gre_label[0x1];
	u8         inner_first_mpls_ttl[0x1];
	u8         inner_first_mpls_s_bos[0x1];
	u8         inner_first_mpls_exp[0x1];
	u8         inner_first_mpls_label[0x1];
	u8         outer_first_mpls_ttl[0x1];
	u8         outer_first_mpls_s_bos[0x1];
	u8         outer_first_mpls_exp[0x1];
	u8         outer_first_mpls_label[0x1];
	u8         outer_emd_tag[0x1];
	u8         inner_esp_spi[0x1];
	u8         outer_esp_spi[0x1];
	u8         inner_ipv6_hop_limit[0x1];
	u8         outer_ipv6_hop_limit[0x1];
	u8         bth_dst_qp[0x1];
	u8         inner_first_svlan[0x1];
	u8         inner_second_svlan[0x1];
	u8         outer_first_svlan[0x1];
	u8         outer_second_svlan[0x1];
	u8         source_sqn[0x1];

	u8         outer_gre_c_present[0x1];
	u8         outer_gre_k_present[0x1];
	u8         outer_gre_s_present[0x1];
	u8         ipsec_syndrome[0x1];
	u8         ipsec_next_header[0x1];
	u8         gtpu_first_ext_dw_0[0x1];
	u8         gtpu_dw_0[0x1];
	u8         gtpu_teid[0x1];
	u8         gtpu_msg_type[0x1];
	u8         gtpu_msg_flags[0x1];
	u8         outer_lrh_lid[0x1];
	u8         outer_grh_flow_label[0x1];
	u8         outer_grh_tclass[0x1];
	u8         outer_grh_gid[0x1];
	u8         outer_bth_pkey[0x1];
	u8         gtpu_dw_2[0x1];
	u8         outer_geneve_ver[0x1];
	u8         outer_geneve_c[0x1];
	u8         icmpv6_code[0x1];
	u8         icmp_code[0x1];
	u8         icmpv6_type[0x1];
	u8         icmp_type[0x1];
	u8         icmpv6_header_data[0x1];
	u8         icmp_header_data[0x1];
	u8         metadata_reg_c_7[0x1];
	u8         metadata_reg_c_6[0x1];
	u8         metadata_reg_c_5[0x1];
	u8         metadata_reg_c_4[0x1];
	u8         metadata_reg_c_3[0x1];
	u8         metadata_reg_c_2[0x1];
	u8         metadata_reg_c_1[0x1];
	u8         metadata_reg_c_0[0x1];
};

struct mlx5_ifc_flow_table_fields_supported_2_bits {
	u8         inner2[0x1];
	u8         inner_tunnel_header[0x1];
	u8         reserved_at_2[0x6];
	u8         inner_mpls[0x1];
	u8         outer_mpls[0x1];
	u8         lag_rx_port_affinity[0x1];
	u8         inner_esp_seq_num[0x1];
	u8         outer_esp_seq_num[0x1];
	u8         hash_result[0x1];
	u8         bth_opcode[0x1];
	u8         tunnel_header_2_3[0x1];
	u8         tunnel_header_0_1[0x1];
	u8         macsec_syndrome[0x1];
	u8         macsec_tag[0x1];
	u8         outer_lrh_sl[0x1];
	u8         inner_ipv4_ihl[0x1];
	u8         outer_ipv4_ihl[0x1];
	u8         nisp_syndrome[0x1];
	u8         inner_l3_ok[0x1];
	u8         inner_l4_ok[0x1];
	u8         outer_l3_ok[0x1];
	u8         outer_l4_ok[0x1];
	u8         nisp_header[0x1];
	u8         inner_ipv4_checksum_ok[0x1];
	u8         inner_l4_checksum_ok[0x1];
	u8         outer_ipv4_checksum_ok[0x1];
	u8         outer_l4_checksum_ok[0x1];

	u8         reserved_at_20[0x60];
};

struct mlx5_ifc_virtio_blk_config_bits {
	u8         capacity[0x40];

	u8         size_max[0x20];

	u8         seg_max[0x20];

	u8         reserved_at_80[0x20];

	u8         cylinders[0x10];
	u8         heads[0x8];
	u8         sectors[0x8];

	u8         reserved_at_c0[0x20];

	u8         blk_size[0x20];

	u8         reserved_at_100[0x20];

	u8         physical_blk_exp[0x8];
	u8         alignment_offset[0x8];
	u8         min_io_size[0x10];

	u8         opt_io_size[0x20];

	u8         reserved_at_160[0x20];

	u8         writeback[0x8];
	u8         reserved_at_188[0x8];
	u8         num_queues[0x10];

	u8         max_discard_sectors[0x20];

	u8         max_discard_seg[0x20];

	u8         discard_sector_alignment[0x20];

	u8         max_write_zeroes_sectors[0x20];

	u8         max_write_zeroes_segs[0x20];

	u8         write_zeroes_may_unmap[0x8];
	u8         reserved_at_248[0x18];

	u8         reserved_at_260[0x20];
};

struct mlx5_ifc_virtio_fs_config_bits {
	u8         tag[9][0x20];

	u8         num_request_queues[0x20];

	u8         reserved_at_140[0x140];
};

struct mlx5_ifc_virtio_initial_registers_bits {
	u8         device_features[0x40];

	u8         num_queue[0x10];
	u8         queue_size[0x10];

	u8         reserved_at_60[0x20];
};

struct mlx5_ifc_virtio_net_config_bits {
	u8         mac_47_16[0x20];

	u8         mac_15_0[0x10];
	u8         status[0x10];

	u8         max_virtqueue_pairs[0x10];
	u8         mtu[0x10];

	u8         reserved_at_60[0x20];
};

struct mlx5_ifc_mac_address_layout_bits {
	u8         reserved_at_0[0x10];
	u8         mac_addr_47_32[0x10];

	u8         mac_addr_31_0[0x20];
};

struct mlx5_ifc_cmd_pas_bits {
	u8         pa_h[0x20];

	u8         pa_l[0x14];
	u8         reserved_at_34[0xc];
};

struct mlx5_ifc_diagnostic_cntr_layout_bits {
	u8         sync[0x1];
	u8         reserved_at_1[0xf];
	u8         counter_id[0x10];
};

struct mlx5_ifc_flow_table_prop_layout_bits {
	u8         ft_support[0x1];
	u8         flow_tag[0x1];
	u8         flow_counter[0x1];
	u8         flow_modify_en[0x1];
	u8         modify_root[0x1];
	u8         identified_miss_table[0x1];
	u8         flow_table_modify[0x1];
	u8         reformat[0x1];
	u8         decap[0x1];
	u8         reset_root_to_default[0x1];
	u8         pop_vlan[0x1];
	u8         push_vlan[0x1];
	u8         fpga_vendor_acceleration[0x1];
	u8         pop_vlan_2[0x1];
	u8         push_vlan_2[0x1];
	u8         reformat_and_vlan_action[0x1];
	u8         modify_and_vlan_action[0x1];
	u8         sw_owner[0x1];
	u8         reformat_l3_tunnel_to_l2[0x1];
	u8         reformat_l2_to_l3_tunnel[0x1];
	u8         reformat_and_modify_action[0x1];
	u8         ignore_flow_level[0x1];
	u8         ip_type[0x1];
	u8         table_miss_action_domain[0x1];
	u8         termination_table[0x1];
	u8         reformat_and_fwd_to_table[0x1];
	u8         forward_vhca_rx[0x1];
	u8         forward_vhca_tx[0x1];
	u8         ipsec_encrypt[0x1];
	u8         ipsec_decrypt[0x1];
	u8         sw_owner_v2[0x1];
	u8         wqe_based_flow_update[0x1];

	u8         termination_table_raw_traffic[0x1];
	u8         vlan_and_fwd_to_table[0x1];
	u8         log_max_ft_size[0x6];
	u8         log_max_modify_header_context[0x8];
	u8         max_modify_header_actions[0x8];
	u8         max_ft_level[0x8];

	u8         reformat_add_esp_transport[0x1];
	u8         reformat_l2_to_l3_esp_tunnel[0x1];
	u8         reformat_add_esp_transport_over_udp[0x1];
	u8         reformat_del_esp_transport[0x1];
	u8         reformat_l3_esp_tunnel_to_l2[0x1];
	u8         reformat_del_esp_transport_over_udp[0x1];
	u8         execute_aso[0x1];
	u8         forward_flow_meter[0x1];
	u8         log_max_flow_sampler_num[0x8];
	u8         metadata_reg_b_width[0x8];
	u8         metadata_reg_a_width[0x8];

	u8         reformat_l2_to_l3_nisp_tunnel[0x1];
	u8         reformat_l3_nisp_tunnel_to_l2[0x1];
	u8         reformat_insert[0x1];
	u8         reformat_remove[0x1];
	u8         macsec_encrypt[0x1];
	u8         macsec_decrypt[0x1];
	u8         nisp_encrypt[0x1];
	u8         nisp_decrypt[0x1];
	u8         reformat_add_macsec[0x1];
	u8         reformat_remove_macsec[0x1];
	u8         reparse[0x1];
	u8         stc_insert_encap[0x1];
	u8         cross_vhca_object[0x1];
	u8         reformat_l2_to_l3_audp_tunnel[0x1];
	u8         reformat_l3_audp_tunnel_to_l2[0x1];
	u8         reserved_at_6f[0x9];
	u8         log_max_ft_num[0x8];

	u8         reserved_at_80[0x10];
	u8         log_max_flow_counter[0x8];
	u8         log_max_destination[0x8];

	u8         reserved_at_a0[0x18];
	u8         log_max_flow[0x8];

	u8         reserved_at_c0[0x40];

	struct mlx5_ifc_flow_table_fields_supported_bits ft_field_support;

	struct mlx5_ifc_flow_table_fields_supported_bits ft_field_bitmask_support;
};

struct mlx5_ifc_header_modify_cap_properties_bits {
	struct mlx5_ifc_flow_table_fields_supported_bits set_action_field_support;

	struct mlx5_ifc_flow_table_fields_supported_2_bits set_action_field_support_2;

	struct mlx5_ifc_flow_table_fields_supported_bits add_action_field_support;

	struct mlx5_ifc_flow_table_fields_supported_2_bits add_action_field_support_2;

	struct mlx5_ifc_flow_table_fields_supported_bits copy_action_field_support;

	struct mlx5_ifc_flow_table_fields_supported_2_bits copy_action_field_support_2;

	u8         reserved_at_300[0x100];
};

struct mlx5_ifc_nvme_initial_registers_bits {
	u8         nvme_registers[48][0x20];
};

struct mlx5_ifc_odp_per_transport_service_cap_bits {
	u8         send[0x1];
	u8         receive[0x1];
	u8         write[0x1];
	u8         read[0x1];
	u8         atomic[0x1];
	u8         rmp[0x1];
	u8         reserved_at_6[0x1a];
};

struct mlx5_ifc_virtio_blk_initial_registers_bits {
	struct mlx5_ifc_virtio_initial_registers_bits virtio_initial_registers;

	struct mlx5_ifc_virtio_blk_config_bits virtio_blk_config;

	u8         reserved_at_300[0x300];
};

struct mlx5_ifc_virtio_fs_initial_registers_bits {
	struct mlx5_ifc_virtio_initial_registers_bits virtio_initial_registers;

	struct mlx5_ifc_virtio_fs_config_bits virtio_fs_config;

	u8         reserved_at_300[0x300];
};

struct mlx5_ifc_virtio_net_initial_registers_bits {
	struct mlx5_ifc_virtio_initial_registers_bits virtio_initial_registers;

	struct mlx5_ifc_virtio_net_config_bits virtio_net_config;

	u8         reserved_at_100[0x500];
};

struct mlx5_ifc_vlan_layout_bits {
	u8         reserved_at_0[0x14];
	u8         vlan[0xc];

	u8         reserved_at_20[0x20];
};

struct mlx5_ifc_ads_bits {
	u8         fl[0x1];
	u8         free_ar[0x1];
	u8         reserved_at_2[0xe];
	u8         pkey_index[0x10];

	u8         reserved_at_20[0x8];
	u8         grh[0x1];
	u8         mlid[0x7];
	u8         rlid[0x10];

	u8         ack_timeout[0x5];
	u8         reserved_at_45[0x3];
	u8         src_addr_index[0x8];
	u8         log_rtm[0x4];
	u8         stat_rate[0x4];
	u8         hop_limit[0x8];

	u8         reserved_at_60[0x4];
	u8         tclass[0x8];
	u8         flow_label[0x14];

	u8         rgid_or_rip[4][0x20];

	u8         reserved_at_100[0x4];
	u8         f_dscp[0x1];
	u8         f_ecn[0x1];
	u8         reserved_at_106[0x1];
	u8         f_eth_prio[0x1];
	u8         ecn[0x2];
	u8         dscp[0x6];
	u8         udp_sport[0x10];

	u8         reserved_at_120[0x1];
	u8         eth_prio[0x3];
	u8         sl[0x4];
	u8         vhca_port_num[0x8];
	u8         rmac_47_32[0x10];

	u8         rmac_31_0[0x20];
};

struct mlx5_ifc_adv_virtualization_capabilities_bits {
	u8         reserved_at_0[0x3];
	u8         pg_track_log_max_num[0x5];
	u8         pg_track_max_num_range[0x8];
	u8         pg_track_log_min_addr_space[0x8];
	u8         pg_track_log_max_addr_space[0x8];

	u8         reserved_at_20[0x3];
	u8         pg_track_log_min_msg_size[0x5];
	u8         reserved_at_28[0x3];
	u8         pg_track_log_max_msg_size[0x5];
	u8         reserved_at_30[0x3];
	u8         pg_track_log_min_page_size[0x5];
	u8         reserved_at_38[0x3];
	u8         pg_track_log_max_page_size[0x5];

	u8         reserved_at_40[0x7c0];
};

struct mlx5_ifc_as_notify_params_bits {
	u8         local_partition_id[0xc];
	u8         process_id[0x14];

	u8         reserved_at_20[0x10];
	u8         thread_id[0x10];
};

enum {
	MLX5_ATOMIC_CAPS_ATOMIC_REQ_8B_ENDIANNESS_MODE_BIG                           = 0x0,
	MLX5_ATOMIC_CAPS_ATOMIC_REQ_8B_ENDIANNESS_MODE_ACCORDING_TO_HOST_ENDIANNESS  = 0x1,
};

enum {
	MLX5_ATOMIC_CAPS_ATOMIC_OPERATIONS_ATOMIC_COMPARE_AND_SWAP_SUPPORTED         = 0x1,
	MLX5_ATOMIC_CAPS_ATOMIC_OPERATIONS_ATOMIC_FETCH_AND_ADD_SUPPORTED            = 0x2,
	MLX5_ATOMIC_CAPS_ATOMIC_OPERATIONS_ATOMIC_MASKED_COMPARE_AND_SWAP_SUPPORTED  = 0x4,
	MLX5_ATOMIC_CAPS_ATOMIC_OPERATIONS_ATOMIC_MASKED_FETCH_AND_ADD_SUPPORTED     = 0x8,
	MLX5_ATOMIC_CAPS_ATOMIC_OPERATIONS_REMOTE_MICRO_APPLICATIONS_SUPPORTED       = 0x10,
};

enum {
	MLX5_ATOMIC_CAPS_ATOMIC_SIZE_QP_BYTE_1     = 0x1,
	MLX5_ATOMIC_CAPS_ATOMIC_SIZE_QP_BYTES_2    = 0x2,
	MLX5_ATOMIC_CAPS_ATOMIC_SIZE_QP_BYTES_4    = 0x4,
	MLX5_ATOMIC_CAPS_ATOMIC_SIZE_QP_BYTES_8    = 0x8,
	MLX5_ATOMIC_CAPS_ATOMIC_SIZE_QP_BYTES_16   = 0x10,
	MLX5_ATOMIC_CAPS_ATOMIC_SIZE_QP_BYTES_32   = 0x20,
	MLX5_ATOMIC_CAPS_ATOMIC_SIZE_QP_BYTES_64   = 0x40,
	MLX5_ATOMIC_CAPS_ATOMIC_SIZE_QP_BYTES_128  = 0x80,
	MLX5_ATOMIC_CAPS_ATOMIC_SIZE_QP_BYTES_256  = 0x100,
};

enum {
	MLX5_ATOMIC_CAPS_ATOMIC_SIZE_DC_BYTE_1     = 0x1,
	MLX5_ATOMIC_CAPS_ATOMIC_SIZE_DC_BYTES_2    = 0x2,
	MLX5_ATOMIC_CAPS_ATOMIC_SIZE_DC_BYTES_4    = 0x4,
	MLX5_ATOMIC_CAPS_ATOMIC_SIZE_DC_BYTES_8    = 0x8,
	MLX5_ATOMIC_CAPS_ATOMIC_SIZE_DC_BYTES_16   = 0x10,
	MLX5_ATOMIC_CAPS_ATOMIC_SIZE_DC_BYTES_32   = 0x20,
	MLX5_ATOMIC_CAPS_ATOMIC_SIZE_DC_BYTES_64   = 0x40,
	MLX5_ATOMIC_CAPS_ATOMIC_SIZE_DC_BYTES_128  = 0x80,
	MLX5_ATOMIC_CAPS_ATOMIC_SIZE_DC_BYTES_256  = 0x100,
};

enum {
	MLX5_ATOMIC_CAPS_FETCH_ADD_PCI_ATOMIC_BYTES_4   = 0x1,
	MLX5_ATOMIC_CAPS_FETCH_ADD_PCI_ATOMIC_BYTES_8   = 0x2,
	MLX5_ATOMIC_CAPS_FETCH_ADD_PCI_ATOMIC_BYTES_16  = 0x4,
};

enum {
	MLX5_ATOMIC_CAPS_SWAP_PCI_ATOMIC_BYTES_4   = 0x1,
	MLX5_ATOMIC_CAPS_SWAP_PCI_ATOMIC_BYTES_8   = 0x2,
	MLX5_ATOMIC_CAPS_SWAP_PCI_ATOMIC_BYTES_16  = 0x4,
};

enum {
	MLX5_ATOMIC_CAPS_COMPARE_SWAP_PCI_ATOMIC_BYTES_4   = 0x1,
	MLX5_ATOMIC_CAPS_COMPARE_SWAP_PCI_ATOMIC_BYTES_8   = 0x2,
	MLX5_ATOMIC_CAPS_COMPARE_SWAP_PCI_ATOMIC_BYTES_16  = 0x4,
};

struct mlx5_ifc_atomic_caps_bits {
	u8         reserved_at_0[0x40];

	u8         atomic_req_8b_endianness_mode[0x2];
	u8         reserved_at_42[0x4];
	u8         supported_atomic_req_8b_endianness_mode_1[0x1];
	u8         reserved_at_47[0x19];

	u8         reserved_at_60[0x20];

	u8         reserved_at_80[0x10];
	u8         atomic_operations[0x10];

	u8         reserved_at_a0[0x10];
	u8         atomic_size_qp[0x10];

	u8         reserved_at_c0[0x10];
	u8         atomic_size_dc[0x10];

	u8         reserved_at_e0[0x20];

	u8         load_remote_micro_app_4byte[0x20];

	u8         load_remote_micro_app_8byte[0x20];

	u8         store_remote_micro_app_4byte[0x20];

	u8         store_remote_micro_app_8byte[0x20];

	u8         reserved_at_180[0x100];

	u8         fetch_add_pci_atomic[0x10];
	u8         swap_pci_atomic[0x10];

	u8         compare_swap_pci_atomic[0x10];
	u8         reserved_at_2b0[0x10];

	u8         reserved_at_2c0[0x540];
};

enum {
	MLX5_CMD_HCA_CAP_GID_TABLE_SIZE_GID_ENTRIES_8    = 0x0,
	MLX5_CMD_HCA_CAP_GID_TABLE_SIZE_GID_ENTRIES_16   = 0x1,
	MLX5_CMD_HCA_CAP_GID_TABLE_SIZE_GID_ENTRIES_32   = 0x2,
	MLX5_CMD_HCA_CAP_GID_TABLE_SIZE_GID_ENTRIES_64   = 0x3,
	MLX5_CMD_HCA_CAP_GID_TABLE_SIZE_GID_ENTRIES_128  = 0x4,
};

enum {
	MLX5_CMD_HCA_CAP_PKEY_TABLE_SIZE_ENTRIES_128  = 0x0,
	MLX5_CMD_HCA_CAP_PKEY_TABLE_SIZE_ENTRIES_256  = 0x1,
	MLX5_CMD_HCA_CAP_PKEY_TABLE_SIZE_ENTRIES_512  = 0x2,
	MLX5_CMD_HCA_CAP_PKEY_TABLE_SIZE_ENTRIES_1K   = 0x3,
	MLX5_CMD_HCA_CAP_PKEY_TABLE_SIZE_ENTRIES_2K   = 0x4,
	MLX5_CMD_HCA_CAP_PKEY_TABLE_SIZE_ENTRIES_4K   = 0x5,
};

enum {
	MLX5_CAP_PORT_TYPE_IB                = 0x0,
	MLX5_CAP_PORT_TYPE_ETH               = 0x1,
};

enum {
	MLX5_CMD_HCA_CAP_UMR_FENCE_STRONG  = 0x0,
	MLX5_CMD_HCA_CAP_UMR_FENCE_SMALL   = 0x1,
	MLX5_CMD_HCA_CAP_UMR_FENCE_NONE    = 0x2,
};

enum {
	MLX5_CMD_HCA_CAP_CMDIF_CHECKSUM_DISABLED       = 0x0,
	MLX5_CMD_HCA_CAP_CMDIF_CHECKSUM_INITIAL_STATE  = 0x1,
	MLX5_CMD_HCA_CAP_CMDIF_CHECKSUM_ENABLED        = 0x3,
};

enum {
	MLX5_CMD_HCA_CAP_LAG_DCT_NOT_SUPPORTED  = 0x0,
	MLX5_CMD_HCA_CAP_LAG_DCT_SW_ASSISTED    = 0x1,
	MLX5_CMD_HCA_CAP_LAG_DCT_HW_ONLY        = 0x2,
};

enum {
	MLX5_CMD_HCA_CAP_FLOW_COUNTER_BULK_ALLOC_BULK_128  = 0x1,
	MLX5_CMD_HCA_CAP_FLOW_COUNTER_BULK_ALLOC_BULK_256  = 0x2,
	MLX5_CMD_HCA_CAP_FLOW_COUNTER_BULK_ALLOC_BULK_512  = 0x4,
	MLX5_CMD_HCA_CAP_FLOW_COUNTER_BULK_ALLOC_BULK_1K   = 0x8,
	MLX5_CMD_HCA_CAP_FLOW_COUNTER_BULK_ALLOC_BULK_2K   = 0x10,
	MLX5_CMD_HCA_CAP_FLOW_COUNTER_BULK_ALLOC_BULK_4K   = 0x20,
	MLX5_CMD_HCA_CAP_FLOW_COUNTER_BULK_ALLOC_BULK_8K   = 0x40,
	MLX5_CMD_HCA_CAP_FLOW_COUNTER_BULK_ALLOC_BULK_16K  = 0x80,
};

enum {
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_NVME_DEVICE_EMULATION_OBJECT  = 0x40,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_NVME_SQ                       = 0x80,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_SW_ICM_BLOCK                  = 0x100,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_NVME_CQ                       = 0x200,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_FLOW_METER                    = 0x400,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_GENEVE_TLV_OPTION             = 0x800,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_DEK                           = 0x1000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_VIRTQ_NET_Q                   = 0x2000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_VHCA_TUNNEL_OBJECT            = 0x4000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_NVME_NAMESPACE                = 0x8000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_NVME_CTRL_COUNTERS            = 0x10000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_NVME_NAMESPACE_COUNTERS       = 0x20000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_NVME_SQ_COUNTERS              = 0x40000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_IPSEC_OFFLOAD                 = 0x80000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_DEVICE_OBJECT                 = 0x100000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_VIRTIO_NET_DEVICE_EMULATION   = 0x200000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_VIRTIO_BLK_DEVICE_EMULATION   = 0x400000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_VIRTIO_BLK_Q                  = 0x800000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_MATCH_DEFINER                 = 0x1000000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_NVMF_NAMESPACE                = 0x4000000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_NVMF_NAMESPACE_COUNTER_SET    = 0x8000000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_VIRTIO_Q_COUNTERS             = 0x10000000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_IMPORT_KEK                    = 0x20000000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_CREDENTIAL                    = 0x40000000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_CRYPTO_LOGIN                  = 0x80000000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_FLOW_SAMPLER                  = 0x1,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_NVMEOTCP_TAG_BUFFER_TABLE     = 0x2,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_PARSE_GRAPH_NODE              = 0x4,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_HEADER_MODIFY_ARGUMENT        = 0x8,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_FLOW_METER_ASO                = 0x10,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_FLOW_HIT_ASO                  = 0x20,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_SCHEDULING_ELEMENT            = 0x40,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_MACSEC_OFFLOAD                = 0x80,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_DPA_MEM                       = 0x100,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_DPA_PROCESS                   = 0x400,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_DPA_THREAD                    = 0x800,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_RESERVED_QPN                  = 0x1000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_DPA_OUTBOX                    = 0x2000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_UPT_DEVICE_EMULATION          = 0x4000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_UPT_SQ                        = 0x8000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_UPT_RQ                        = 0x10000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_CONN_TRACK_OFFLOAD            = 0x20000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_EMULATED_DEV_DB_CQ_MAPPING    = 0x40000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_DPA_EQ                        = 0x80000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_NVME_SQ_BE                    = 0x1000000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_DPA_WINDOW                    = 0x2000000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_AV_QP_MAPPING                 = 0x4000000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_CHANNEL_SERVICE               = 0x8000000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_CHANNEL_CONNECTION            = 0x10000000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_VIRTIO_FS_DEVICE_EMULATION    = 0x20000000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_VIRTIO_FS_Q                   = 0x40000000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_STC                           = 0x1,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_RTC                           = 0x2,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_STE                           = 0x4,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_HEADER_MODIFY_PATTERN         = 0x8,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_UPT_RQ_COUNTERS               = 0x10,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_UPT_SQ_COUNTERS               = 0x20,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_PAGE_TRACK                    = 0x40,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_INT_KEK                       = 0x80,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_NVMF_BACKEND_CONTROLLER       = 0x100,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_EMULATED_DEV_EQ               = 0x200,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_SF                            = 0x1,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_MKEY                          = 0x2,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_QP                            = 0x4,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_PSV                           = 0x8,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_RMP                           = 0x10,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_XRC_SRQ                       = 0x20,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_RQ                            = 0x40,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_SQ                            = 0x80,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_TIR                           = 0x100,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_TIS                           = 0x200,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_DCT                           = 0x400,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_XRQ                           = 0x800,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_RQT                           = 0x4000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_FLOW_COUNTER                  = 0x8000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_CQ                            = 0x10000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_UAR                           = 0x20000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_PD                            = 0x40000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_UMEM                          = 0x80000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_EQ                            = 0x100000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_FLOW_TABLE_ALIAS              = 0x200000,
	MLX5_CMD_HCA_CAP_GENERAL_OBJ_TYPES_63_0_TIR_ALIAS                     = 0x400000,
};

enum {
	MLX5_CMD_HCA_CAP_SQ_TS_FORMAT_FREE_RUNNING_TS                = 0x0,
	MLX5_CMD_HCA_CAP_SQ_TS_FORMAT_REAL_TIME_TS                   = 0x1,
	MLX5_CMD_HCA_CAP_SQ_TS_FORMAT_FREE_RUNNING_AND_REAL_TIME_TS  = 0x2,
};

enum {
	MLX5_CMD_HCA_CAP_RQ_TS_FORMAT_FREE_RUNNING_TS                = 0x0,
	MLX5_CMD_HCA_CAP_RQ_TS_FORMAT_REAL_TIME_TS                   = 0x1,
	MLX5_CMD_HCA_CAP_RQ_TS_FORMAT_FREE_RUNNING_AND_REAL_TIME_TS  = 0x2,
};

enum {
	MLX5_CMD_HCA_CAP_STEERING_FORMAT_VERSION_CONNECTX_5    = 0x0,
	MLX5_CMD_HCA_CAP_STEERING_FORMAT_VERSION_CONNECTX_6DX  = 0x1,
	MLX5_CMD_HCA_CAP_STEERING_FORMAT_VERSION_CONNECTX_7    = 0x2,
};

enum {
	MLX5_CMD_HCA_CAP_FLEX_PARSER_PROTOCOLS_FLEX_IPV4_OVER_VXLAN_ENABLED      = 0x1,
	MLX5_CMD_HCA_CAP_FLEX_PARSER_PROTOCOLS_FLEX_IPV4_OVER_IP_ENABLED         = 0x2,
	MLX5_CMD_HCA_CAP_FLEX_PARSER_PROTOCOLS_FLEX_IPV6_OVER_IP_ENABLED         = 0x4,
	MLX5_CMD_HCA_CAP_FLEX_PARSER_PROTOCOLS_FLEX_GENEVE_ENABLED               = 0x8,
	MLX5_CMD_HCA_CAP_FLEX_PARSER_PROTOCOLS_FLEX_VXLAN_GPE_ENABLED            = 0x80,
	MLX5_CMD_HCA_CAP_FLEX_PARSER_PROTOCOLS_FLEX_ICMP_ENABLED                 = 0x100,
	MLX5_CMD_HCA_CAP_FLEX_PARSER_PROTOCOLS_FLEX_ICMPV6_ENABLED               = 0x200,
	MLX5_CMD_HCA_CAP_FLEX_PARSER_PROTOCOLS_FLEX_GENEVE_TLV_OPTION_0_ENABLED  = 0x400,
	MLX5_CMD_HCA_CAP_FLEX_PARSER_PROTOCOLS_FLEX_GTP_U_ENABLED                = 0x800,
	MLX5_CMD_HCA_CAP_FLEX_PARSER_PROTOCOLS_FLEX_IPOIB_UD_QP_ENABLED          = 0x1000,
	MLX5_CMD_HCA_CAP_FLEX_PARSER_PROTOCOLS_FLEX_VXLAN_PAD_ENABLED            = 0x2000,
	MLX5_CMD_HCA_CAP_FLEX_PARSER_PROTOCOLS_FLEX_CW_MPLS_OVER_GRE_ENABLED     = 0x4000,
	MLX5_CMD_HCA_CAP_FLEX_PARSER_PROTOCOLS_FLEX_CW_MPLS_OVER_UDP_ENABLED     = 0x8000,
	MLX5_CMD_HCA_CAP_FLEX_PARSER_PROTOCOLS_FLEX_GTPU_DW_2_ENABLED            = 0x10000,
	MLX5_CMD_HCA_CAP_FLEX_PARSER_PROTOCOLS_FLEX_GTPU_FIRST_EXT_DW_0_ENABLED  = 0x20000,
	MLX5_CMD_HCA_CAP_FLEX_PARSER_PROTOCOLS_FLEX_GTPU_DW_0_ENABLED            = 0x40000,
	MLX5_CMD_HCA_CAP_FLEX_PARSER_PROTOCOLS_FLEX_GTPU_TEID_ENABLED            = 0x80000,
};

enum {
	MLX5_CMD_HCA_CAP_CQ_TIME_STAMP_REAL_TIME_MODE_REAL_TIME_ONLY                = 0x1,
	MLX5_CMD_HCA_CAP_CQ_TIME_STAMP_REAL_TIME_MODE_REAL_TIME_AND_INTERNAL_TIMER  = 0x2,
};

enum {
	MLX5_CMD_HCA_CAP_VHCA_TUNNEL_COMMANDS_INIT_HCA                            = 0x1,
	MLX5_CMD_HCA_CAP_VHCA_TUNNEL_COMMANDS_TEARDOWN_HCA                        = 0x2,
	MLX5_CMD_HCA_CAP_VHCA_TUNNEL_COMMANDS_ENABLE_HCA                          = 0x4,
	MLX5_CMD_HCA_CAP_VHCA_TUNNEL_COMMANDS_DISABLE_HCA                         = 0x8,
	MLX5_CMD_HCA_CAP_VHCA_TUNNEL_COMMANDS_QUERY_SPECIAL_CONTEXT               = 0x10,
	MLX5_CMD_HCA_CAP_VHCA_TUNNEL_COMMANDS_FLOW_TABLE_COMMANDS                 = 0x20,
	MLX5_CMD_HCA_CAP_VHCA_TUNNEL_COMMANDS_QP_COMMANDS                         = 0x40,
	MLX5_CMD_HCA_CAP_VHCA_TUNNEL_COMMANDS_SET_ROCE_ADDRESS                    = 0x200,
	MLX5_CMD_HCA_CAP_VHCA_TUNNEL_COMMANDS_QUERY_ROCE_ADDRESS                  = 0x400,
	MLX5_CMD_HCA_CAP_VHCA_TUNNEL_COMMANDS_GEN_OBJ_CMD_NVME_SQ                 = 0x800,
	MLX5_CMD_HCA_CAP_VHCA_TUNNEL_COMMANDS_GEN_OBJ_CMD_NVME_CQ                 = 0x1000,
	MLX5_CMD_HCA_CAP_VHCA_TUNNEL_COMMANDS_GEN_OBJ_CMD_NVME_DEVICE_EUMULATION  = 0x2000,
	MLX5_CMD_HCA_CAP_VHCA_TUNNEL_COMMANDS_NVME_NAMESPACE                      = 0x4000,
	MLX5_CMD_HCA_CAP_VHCA_TUNNEL_COMMANDS_PD                                  = 0x8000,
};

struct mlx5_ifc_cmd_hca_cap_bits {
	u8         access_other_hca_roce[0x1];
	u8         alloc_flow_counter_pd[0x1];
	u8         flow_counter_access_aso[0x1];
	u8         reserved_at_3[0x5];
	u8         flow_counter_acess_aso_opc_mod[0x8];
	u8         shared_object_to_user_object_allowed[0x1];
	u8         reserved_at_11[0x2];
	u8         log_sha_mmo_max_size[0x5];
	u8         reserved_at_18[0x7];
	u8         vhca_resource_manager[0x1];

	u8         hca_cap_2[0x1];
	u8         create_lag_when_not_master_up[0x1];
	u8         dtor[0x1];
	u8         event_on_vhca_state_teardown_request[0x1];
	u8         event_on_vhca_state_in_use[0x1];
	u8         event_on_vhca_state_active[0x1];
	u8         event_on_vhca_state_allocated[0x1];
	u8         event_on_vhca_state_invalid[0x1];
	u8         transpose_max_element_size[0x8];
	u8         vhca_id[0x10];

	u8         transpose_max_cols[0x8];
	u8         transpose_max_rows[0x8];
	u8         transpose_max_size[0x10];

	u8         resources_on_virtio_fs_emulation_manager[0x1];
	u8         sw_steering_icm_large_scale_steering[0x1];
	u8         qp_data_in_order[0x1];
	u8         log_regexp_scatter_gather_size[0x5];
	u8         ibl2_delay_drop[0x1];
	u8         ibl2_qp[0x1];
	u8         init2init_qp_drain_sigerr[0x1];
	u8         log_dma_mmo_max_size[0x5];
	u8         relaxed_ordering_write_pci_enabled[0x1];
	u8         ibl2_tx_vl15[0x1];
	u8         decompress_snappy_v2[0x1];
	u8         log_compress_max_size[0x5];
	u8         decompress_lz4_data_only_v2[0x1];
	u8         decompress_lz4_no_checksum_v2[0x1];
	u8         decompress_lz4_checksum_v2[0x1];
	u8         log_decompress_max_size[0x5];

	u8         log_max_srq_sz[0x8];
	u8         log_max_qp_sz[0x8];
	u8         event_cap[0x1];
	u8         rc_qp_init2rts[0x1];
	u8         rc_qp_create_on_init[0x1];
	u8         isolate_vl_tc_new[0x1];
	u8         comm_channel_dc[0x1];
	u8         comm_channel_cross_esw[0x1];
	u8         nvmeotcp[0x1];
	u8         pcie_hanged[0x1];
	u8         prio_tag_required[0x1];
	u8         wqe_index_ignore_cap[0x1];
	u8         dynamic_qp_allocation[0x1];
	u8         log_max_qp[0x5];

	u8         must_be_0[0x1];
	u8         reserved_at_a1[0x1];
	u8         regexp_alloc_onbehalf_umem[0x1];
	u8         ece[0x1];
	u8         regexp_num_of_engines[0x4];
	u8         allow_pause_tx[0x1];
	u8         reg_c_preserve[0x1];
	u8         reserved_at_aa[0x1];
	u8         log_max_srq[0x5];
	u8         nisp[0x1];
	u8         uplink_follow[0x1];
	u8         ts_cqe_to_dest_cqn[0x1];
	u8         reserved_at_b3[0x5];
	u8         selective_repeat[0x1];
	u8         go_back_n[0x1];
	u8         shampo[0x1];
	u8         scatter_fcs_w_decap_disable[0x1];
	u8         reserved_at_bc[0x4];

	u8         max_sgl_for_optimized_performance[0x8];
	u8         log_max_cq_sz[0x8];
	u8         reserved_at_d0[0x2];
	u8         access_register_user[0x1];
	u8         max_num_of_fixed_buffers_per_mkey[0x5];
	u8         upt_device_emulation_manager[0x1];
	u8         virtio_net_device_emulation_manager[0x1];
	u8         virtio_blk_device_emulation_manager[0x1];
	u8         log_max_cq[0x5];

	u8         log_max_eq_sz[0x8];
	u8         relaxed_ordering_write[0x1];
	u8         relaxed_ordering_read_pci_enabled[0x1];
	u8         log_max_mkey[0x6];
	u8         tunneled_atomic[0x1];
	u8         as_notify[0x1];
	u8         m_pci_port[0x1];
	u8         m_vhca_mk[0x1];
	u8         hotplug_manager[0x1];
	u8         nvme_device_emulation_manager[0x1];
	u8         terminate_scatter_list_mkey[0x1];
	u8         repeated_mkey[0x1];
	u8         dump_fill_mkey[0x1];
	u8         dpp[0x1];
	u8         resources_on_nvme_emulation_manager[0x1];
	u8         fast_teardown[0x1];
	u8         log_max_eq[0x4];

	u8         max_indirection[0x8];
	u8         fixed_buffer_size[0x1];
	u8         log_max_mrw_sz[0x7];
	u8         force_teardown[0x1];
	u8         hard_coded_1[0x1];
	u8         log_max_bsf_list_size[0x6];
	u8         umr_extended_translation_offset[0x1];
	u8         null_mkey[0x1];
	u8         log_max_klm_list_size[0x6];

	u8         non_wire_sq[0x1];
	u8         ats_ro_dependence[0x1];
	u8         qpc_extension[0x1];
	u8         log_max_static_sq_wq_size[0x5];
	u8         resources_on_virtio_net_emulation_manager[0x1];
	u8         resources_on_virtio_blk_emulation_manager[0x1];
	u8         log_max_ra_req_dc[0x6];
	u8         vhca_trust_level_reg[0x1];
	u8         eth_wqe_too_small_mode[0x1];
	u8         vnic_env_eth_wqe_too_small[0x1];
	u8         log_max_static_sq_wq[0x5];
	u8         ooo_sl_mask[0x1];
	u8         vnic_env_cq_overrun[0x1];
	u8         log_max_ra_res_dc[0x6];

	u8         cc_roce_ecn_rp_classify_mode[0x1];
	u8         cc_roce_ecn_rp_dynamic_rtt[0x1];
	u8         cc_roce_ecn_rp_dynamic_ai[0x1];
	u8         cc_roce_ecn_rp_dynamic_g[0x1];
	u8         cc_roce_ecn_rp_burst_decouple[0x1];
	u8         release_all_pages[0x1];
	u8         reserved_at_146[0x1];
	u8         sig_crc64_xp10[0x1];
	u8         sig_crc32c[0x1];
	u8         roce_accl[0x1];
	u8         log_max_ra_req_qp[0x6];
	u8         rtr2rts_qp_counters_set_id[0x1];
	u8         rts2rts_udp_sport[0x1];
	u8         rts2rts_lag_tx_port_affinity[0x1];
	u8         dma_mmo_sq[0x1];
	u8         compress_min_block_size[0x4];
	u8         compress_mmo_sq[0x1];
	u8         decompress_mmo_sq_v1[0x1];
	u8         log_max_ra_res_qp[0x6];

	u8         end_pad[0x1];
	u8         cc_query_allowed[0x1];
	u8         cc_modify_allowed[0x1];
	u8         start_pad[0x1];
	u8         cache_line_128byte[0x1];
	u8         gid_table_size_ro[0x1];
	u8         pkey_table_size_ro[0x1];
	u8         rts2rts_qp_rmp[0x1];
	u8         rnr_nak_q_counters[0x1];
	u8         rts2rts_qp_counters_set_id[0x1];
	u8         rts2rts_qp_dscp[0x1];
	u8         gen3_cc_negotiation[0x1];
	u8         vnic_env_int_rq_oob[0x1];
	u8         sbcam_reg[0x1];
	u8         cwcam_reg[0x1];
	u8         qcam_reg[0x1];
	u8         gid_table_size[0x10];

	u8         out_of_seq_cnt[0x1];
	u8         vport_counters[0x1];
	u8         retransmission_q_counters[0x1];
	u8         debug[0x1];
	u8         modify_rq_counters_set_id[0x1];
	u8         rq_delay_drop[0x1];
	u8         max_qp_cnt[0xa];
	u8         pkey_table_size[0x10];

	u8         vport_group_manager[0x1];
	u8         vhca_group_manager[0x1];
	u8         ib_virt[0x1];
	u8         eth_virt[0x1];
	u8         vnic_env_queue_counters[0x1];
	u8         ets[0x1];
	u8         nic_flow_table[0x1];
	u8         eswitch_manager[0x1];
	u8         device_memory[0x1];
	u8         mcam_reg[0x1];
	u8         pcam_reg[0x1];
	u8         local_ca_ack_delay[0x5];
	u8         port_module_event[0x1];
	u8         enhanced_retransmission_q_counters[0x1];
	u8         port_checks[0x1];
	u8         pulse_gen_control[0x1];
	u8         disable_link_up_by_init_hca[0x1];
	u8         beacon_led[0x1];
	u8         port_type[0x2];
	u8         num_ports[0x8];

	u8         reserved_at_1c0[0x1];
	u8         pps[0x1];
	u8         pps_modify[0x1];
	u8         log_max_msg[0x5];
	u8         multi_path_xrc_rdma[0x1];
	u8         multi_path_dc_rdma[0x1];
	u8         multi_path_rc_rdma[0x1];
	u8         traffic_fast_control[0x1];
	u8         max_tc[0x4];
	u8         temp_warn_event[0x1];
	u8         dcbx[0x1];
	u8         general_notification_event[0x1];
	u8         multi_prio_sq[0x1];
	u8         afu_owner[0x1];
	u8         fpga[0x1];
	u8         rol_s[0x1];
	u8         rol_g[0x1];
	u8         ib_port_sniffer[0x1];
	u8         wol_s[0x1];
	u8         wol_g[0x1];
	u8         wol_a[0x1];
	u8         wol_b[0x1];
	u8         wol_m[0x1];
	u8         wol_u[0x1];
	u8         wol_p[0x1];

	u8         stat_rate_support[0x10];
	u8         sig_block_4048[0x1];
	u8         pci_sync_for_fw_update_event[0x1];
	u8         init2rtr_drain_sigerr[0x1];
	u8         log_max_extended_rnr_retry[0x5];
	u8         init2_lag_tx_port_affinity[0x1];
	u8         flow_group_type_hash_split[0x1];
	u8         stat_rate_200[0x1];
	u8         wqe_based_flow_table_update_cap[0x1];
	u8         cqe_version[0x4];

	u8         compact_address_vector[0x1];
	u8         eth_striding_wq[0x1];
	u8         reserved_at_202[0x1];
	u8         ipoib_enhanced_offloads[0x1];
	u8         ipoib_basic_offloads[0x1];
	u8         ib_link_list_striding_wq[0x1];
	u8         repeated_block_disabled[0x1];
	u8         umr_modify_entity_size_disabled[0x1];
	u8         umr_modify_atomic_disabled[0x1];
	u8         umr_indirect_mkey_disabled[0x1];
	u8         umr_fence[0x2];
	u8         dc_req_sctr_data_cqe[0x1];
	u8         dc_connect_qp[0x1];
	u8         dc_cnak_trace[0x1];
	u8         drain_sigerr[0x1];
	u8         cmdif_checksum[0x2];
	u8         sigerr_cqe[0x1];
	u8         e_psv[0x1];
	u8         wq_signature[0x1];
	u8         sctr_data_cqe[0x1];
	u8         bsf_in_create_mkey[0x1];
	u8         sho[0x1];
	u8         tph[0x1];
	u8         rf[0x1];
	u8         dct[0x1];
	u8         qos[0x1];
	u8         eth_net_offloads[0x1];
	u8         roce[0x1];
	u8         atomic[0x1];
	u8         extended_retry_count[0x1];

	u8         cq_oi[0x1];
	u8         cq_resize[0x1];
	u8         cq_moderation[0x1];
	u8         cq_period_mode_modify[0x1];
	u8         cq_invalidate[0x1];
	u8         ib_striding_wq_cq_first_indication[0x1];
	u8         cq_eq_remap[0x1];
	u8         pg[0x1];
	u8         block_lb_mc[0x1];
	u8         exponential_backoff[0x1];
	u8         scqe_break_moderation[0x1];
	u8         cq_period_start_from_cqe[0x1];
	u8         cd[0x1];
	u8         atm[0x1];
	u8         apm[0x1];
	u8         vector_calc[0x1];
	u8         umr_ptr_rlkey[0x1];
	u8         imaicl[0x1];
	u8         qp_packet_based[0x1];
	u8         ib_cyclic_striding_wq[0x1];
	u8         ipoib_enhanced_pkey_change[0x1];
	u8         initiator_src_dct_in_cqe[0x1];
	u8         qkv[0x1];
	u8         pkv[0x1];
	u8         set_deth_sqpn[0x1];
	u8         rts2rts_primary_sl[0x1];
	u8         initiator_src_dct[0x1];
	u8         dc_v2[0x1];
	u8         xrc[0x1];
	u8         ud[0x1];
	u8         uc[0x1];
	u8         rc[0x1];

	u8         uar_4k[0x1];
	u8         dci_no_rdma_wr_optimized_performance[0x1];
	u8         local_dma[0x1];
	u8         log_max_local_dma_size[0x5];
	u8         fl_rc_qp_when_roce_disabled[0x1];
	u8         regexp_params[0x1];
	u8         uar_sz[0x6];
	u8         port_selection_cap[0x1];
	u8         nic_cap_reg[0x1];
	u8         umem_uid_0[0x1];
	u8         log_max_dc_cnak_qps[0x5];
	u8         log_pg_sz[0x8];

	u8         bf[0x1];
	u8         driver_version[0x1];
	u8         pad_tx_eth_packet[0x1];
	u8         query_driver_version[0x1];
	u8         max_qp_retry_freq[0x1];
	u8         qp_by_name[0x1];
	u8         mkey_by_name[0x1];
	u8         virtio_fs_device_emulation_manager[0x1];
	u8         suspend_qp_uc[0x1];
	u8         suspend_qp_ud[0x1];
	u8         suspend_qp_rc[0x1];
	u8         log_bf_reg_size[0x5];
	u8         reserved_at_270[0x1];
	u8         ec_offload_engine_disabled[0x1];
	u8         reserved_at_272[0x1];
	u8         qp_error_syndrome[0x1];
	u8         qp_handling_when_roce_disabled[0x1];
	u8         atomic_rate_pa[0x1];
	u8         lag_dct[0x2];
	u8         lag_tx_port_affinity[0x1];
	u8         lag_native_fdb_selection[0x1];
	u8         reserved_at_27a[0x1];
	u8         lag_master[0x1];
	u8         num_lag_ports[0x4];

	u8         num_of_diagnostic_counters[0x10];
	u8         max_wqe_sz_sq[0x10];

	u8         reserved_at_2a0[0xc];
	u8         regexp_mmo_sq[0x1];
	u8         regexp_version[0x3];
	u8         max_wqe_sz_rq[0x10];

	u8         max_flow_counter_31_16[0x10];
	u8         max_wqe_sz_sq_dc[0x10];

	u8         reserved_at_2e0[0x7];
	u8         max_qp_mcg[0x19];

	u8         mlnx_tag_ethertype[0x10];
	u8         flow_counter_bulk_alloc[0x8];
	u8         log_max_mcg[0x8];

	u8         emulated_vf_exist[0x1];
	u8         uncorrectable_notification_dpu_reset_en[0x1];
	u8         always_fire_port_state_change_event[0x1];
	u8         log_max_transport_domain[0x5];
	u8         tir_esw_lb_filter_disable[0x1];
	u8         flow_table_freeze[0x1];
	u8         relaxed_ordering_read[0x1];
	u8         log_max_pd[0x5];
	u8         reserved_at_330[0x8];
	u8         vport_counter_local_loopback[0x1];
	u8         reserved_at_339[0x2];
	u8         log_max_xrcd[0x5];

	u8         nic_receive_steering_discard[0x1];
	u8         receive_discard_vport_down[0x1];
	u8         transmit_discard_vport_down[0x1];
	u8         eq_overrun_count[0x1];
	u8         nic_receive_steering_depth[0x1];
	u8         invalid_command_count[0x1];
	u8         quota_exceeded_count[0x1];
	u8         reserved_at_347[0x1];
	u8         log_max_flow_counter_bulk[0x8];
	u8         max_flow_counter_15_0[0x10];

	u8         modify_tis[0x1];
	u8         flow_counters_dump[0x1];
	u8         pad_tls_encrypt_sig[0x1];
	u8         log_max_rq[0x5];
	u8         reserved_at_368[0x3];
	u8         log_max_sq[0x5];
	u8         reserved_at_370[0x3];
	u8         log_max_tir[0x5];
	u8         reserved_at_378[0x3];
	u8         log_max_tis[0x5];

	u8         basic_cyclic_rcv_wqe[0x1];
	u8         eth_wqe_too_small_discard_no_cqe[0x1];
	u8         mem_rq_rmp[0x1];
	u8         log_max_rmp[0x5];
	u8         reserved_at_388[0x3];
	u8         log_max_rqt[0x5];
	u8         reserved_at_390[0x3];
	u8         log_max_rqt_size[0x5];
	u8         reserved_at_398[0x3];
	u8         log_max_tis_per_sq[0x5];

	u8         ext_stride_num_range[0x1];
	u8         roce_rw_supported[0x1];
	u8         log_max_current_uc_list_wr_supported[0x1];
	u8         log_max_stride_sz_rq[0x5];
	u8         reserved_at_3a8[0x3];
	u8         log_min_stride_sz_rq[0x5];
	u8         reserved_at_3b0[0x3];
	u8         log_max_stride_sz_sq[0x5];
	u8         reserved_at_3b8[0x3];
	u8         log_min_stride_sz_sq[0x5];

	u8         hairpin[0x1];
	u8         reserved_at_3c1[0x2];
	u8         log_max_hairpin_queues[0x5];
	u8         hairpin_ib_raw[0x1];
	u8         hairpin_eth2ipoib[0x1];
	u8         hairpin_ipoib2eth[0x1];
	u8         log_max_hairpin_wq_data_sz[0x5];
	u8         reserved_at_3d0[0x3];
	u8         log_max_hairpin_num_packets[0x5];
	u8         reserved_at_3d8[0x3];
	u8         log_max_wq_sz[0x5];

	u8         nic_vport_change_event[0x1];
	u8         disable_local_lb_uc[0x1];
	u8         disable_local_lb_mc[0x1];
	u8         log_min_hairpin_wq_data_sz[0x5];
	u8         system_image_guid_modifiable[0x1];
	u8         silent_mode[0x1];
	u8         vhca_state[0x1];
	u8         log_max_vlan_list[0x5];
	u8         reserved_at_3f0[0x1];
	u8         aes_xts_single_block_le_tweak[0x1];
	u8         aes_xts_multi_block_be_tweak[0x1];
	u8         log_max_current_mc_list[0x5];
	u8         reserved_at_3f8[0x3];
	u8         log_max_current_uc_list[0x5];

	u8         general_obj_types_63_0[0x40];

	u8         sq_ts_format[0x2];
	u8         rq_ts_format[0x2];
	u8         steering_format_version[0x4];
	u8         create_qp_start_hint[0x18];

	u8         reserved_at_460[0x1];
	u8         ats[0x1];
	u8         cross_vhca_rqt[0x1];
	u8         log_max_uctx[0x5];
	u8         reserved_at_468[0x1];
	u8         crypto[0x1];
	u8         ipsec_offload[0x1];
	u8         log_max_umem[0x5];
	u8         max_num_eqs[0x10];

	u8         sigerr_domain_and_sig_type[0x1];
	u8         tls_tx[0x1];
	u8         tls_rx[0x1];
	u8         log_max_l2_table[0x5];
	u8         reserved_at_488[0x8];
	u8         log_uar_page_sz[0x10];

	u8         e[0x1];
	u8         reserved_at_4a1[0x1f];

	u8         device_frequency_mhz[0x20];

	u8         device_frequency_khz[0x20];

	u8         capi[0x1];
	u8         create_pec[0x1];
	u8         nvmf_target_offload[0x1];
	u8         capi_invalidate[0x1];
	u8         reserved_at_504[0x17];
	u8         log_max_pasid[0x5];

	u8         num_of_uars_per_page[0x20];

	u8         flex_parser_protocols[0x20];

	u8         max_geneve_tlv_options[0x8];
	u8         geneve_tlv_sample[0x1];
	u8         geneve_tlv_option_offset[0x1];
	u8         reserved_at_56a[0x1];
	u8         max_geneve_tlv_option_data_len[0x5];
	u8         flex_parser_header_modify[0x1];
	u8         reserved_at_571[0x2];
	u8         log_max_guaranteed_connections[0x5];
	u8         driver_version_before_init_hca[0x1];
	u8         adv_virtualization[0x1];
	u8         reserved_at_57a[0x1];
	u8         log_max_dct_connections[0x5];

	u8         log_max_atomic_size_qp[0x8];
	u8         reserved_at_588[0x3];
	u8         log_max_dci_stream_channels[0x5];
	u8         reserved_at_590[0x3];
	u8         log_max_dci_errored_streams[0x5];
	u8         log_max_atomic_size_dc[0x8];

	u8         max_multi_user_group_size[0x10];
	u8         enhanced_cqe_compression[0x1];
	u8         reserved_at_5b1[0x1];
	u8         crossing_vhca_mkey[0x1];
	u8         log_max_dek[0x5];
	u8         reserved_at_5b8[0x1];
	u8         mini_cqe_resp_l3_l4_tag[0x1];
	u8         mini_cqe_resp_flow_tag[0x1];
	u8         reserved_at_5bb[0x1];
	u8         mini_cqe_resp_stride_index[0x1];
	u8         cqe_128_always[0x1];
	u8         cqe_compression_128b[0x1];
	u8         cqe_compression[0x1];

	u8         cqe_compression_timeout[0x10];
	u8         cqe_compression_max_num[0x10];

	u8         must_not_use[0x8];
	u8         flex_parser_id_gtpu_dw_0[0x4];
	u8         log_max_tm_offloaded_op_size[0x4];
	u8         tag_matching[0x1];
	u8         rndv_offload_rc[0x1];
	u8         rndv_offload_dc[0x1];
	u8         log_tag_matching_list_sz[0x5];
	u8         modify_xrq[0x1];
	u8         sync_steering_cache_inv[0x1];
	u8         must_not_use2[0x1];
	u8         log_max_xrq[0x5];

	u8         affiliate_nic_vport_criteria[0x8];
	u8         native_port_num[0x8];
	u8         num_vhca_ports[0x8];
	u8         flex_parser_id_gtpu_teid[0x4];
	u8         reserved_at_61c[0x1];
	u8         trusted_vnic_vhca[0x1];
	u8         sw_owner_id[0x1];
	u8         reserve_not_to_use[0x1];

	u8         max_num_of_monitor_counters[0x10];
	u8         num_ppcnt_monitor_counters[0x10];

	u8         max_num_sf[0x10];
	u8         num_q_monitor_counters[0x10];

	u8         reserved_at_660[0x20];

	u8         sf[0x1];
	u8         sf_set_partition[0x1];
	u8         reserved_at_682[0x1];
	u8         log_max_sf[0x5];
	u8         dpa[0x1];
	u8         reserved_at_689[0x2];
	u8         wait_on_data[0x1];
	u8         wait_on_time[0x1];
	u8         migration[0x1];
	u8         cq_time_stamp_real_time_mode[0x2];
	u8         log_min_sf_size[0x8];
	u8         max_num_sf_partitions[0x8];

	u8         uctx_permission[0x20];

	u8         flex_parser_id_mpls_over_x_cw[0x4];
	u8         flex_parser_id_geneve_tlv_option_0[0x4];
	u8         flex_parser_id_icmp_dw1[0x4];
	u8         flex_parser_id_icmp_dw0[0x4];
	u8         flex_parser_id_icmpv6_dw1[0x4];
	u8         flex_parser_id_icmpv6_dw0[0x4];
	u8         flex_parser_id_outer_first_mpls_over_gre[0x4];
	u8         flex_parser_id_outer_first_mpls_over_udp_label[0x4];

	u8         max_num_match_definer[0x10];
	u8         sf_base_id[0x10];

	u8         flex_parser_id_gtpu_dw_2[0x4];
	u8         flex_parser_id_gtpu_first_ext_dw_0[0x4];
	u8         num_total_dynamic_vf_msix[0x18];

	u8         reserved_at_720[0x3];
	u8         log_flow_hit_aso_granularity[0x5];
	u8         reserved_at_728[0x3];
	u8         log_flow_hit_aso_max_alloc[0x5];
	u8         reserved_at_730[0x4];
	u8         dynamic_msix_table_size[0xc];

	u8         sha_mmo_qp[0x1];
	u8         reserved_at_741[0x2];
	u8         log_max_num_flow_hit_aso[0x5];
	u8         dma_mmo_qp[0x1];
	u8         regexp_mmo_qp[0x1];
	u8         compress_mmo_qp[0x1];
	u8         decompress_deflate_v1[0x1];
	u8         min_dynamic_vf_msix_table_size[0x4];
	u8         decompress_deflate_v2[0x1];
	u8         host_net_functions_changed[0x1];
	u8         reserved_at_752[0x2];
	u8         max_dynamic_vf_msix_table_size[0xc];

	u8         reserved_at_760[0x3];
	u8         log_max_num_header_modify_argument[0x5];
	u8         log_header_modify_argument_granularity_offset[0x4];
	u8         log_header_modify_argument_granularity[0x4];
	u8         reserved_at_770[0x3];
	u8         log_header_modify_argument_max_alloc[0x5];
	u8         reserved_at_778[0x3];
	u8         max_flow_execute_aso[0x5];

	u8         vhca_tunnel_commands[0x40];

	u8         match_definer_format_supported[0x40];
};

enum {
	MLX5_CQE_ERROR_SYNDROME_SYNDROME_LOCAL_LENGTH_ERROR                = 0x1,
	MLX5_CQE_ERROR_SYNDROME_SYNDROME_LOCAL_QP_OPERATION_ERROR          = 0x2,
	MLX5_CQE_ERROR_SYNDROME_SYNDROME_LOCAL_PROTECTION_ERROR            = 0x4,
	MLX5_CQE_ERROR_SYNDROME_SYNDROME_WORK_REQUEST_FLUSHED_ERROR        = 0x5,
	MLX5_CQE_ERROR_SYNDROME_SYNDROME_MEMORY_WINDOW_BIND_ERROR          = 0x6,
	MLX5_CQE_ERROR_SYNDROME_SYNDROME_BAD_RESPONSE_ERROR                = 0x10,
	MLX5_CQE_ERROR_SYNDROME_SYNDROME_LOCAL_ACCESS_ERROR                = 0x11,
	MLX5_CQE_ERROR_SYNDROME_SYNDROME_REMOTE_INVALID_REQUEST_ERROR      = 0x12,
	MLX5_CQE_ERROR_SYNDROME_SYNDROME_REMOTE_ACCESS_ERROR               = 0x13,
	MLX5_CQE_ERROR_SYNDROME_SYNDROME_REMOTE_OPERATION_ERROR            = 0x14,
	MLX5_CQE_ERROR_SYNDROME_SYNDROME_TRANSPORT_RETRY_COUNTER_EXCEEDED  = 0x15,
	MLX5_CQE_ERROR_SYNDROME_SYNDROME_RNR_RETRY_COUNTER_EXCEEDED        = 0x16,
	MLX5_CQE_ERROR_SYNDROME_SYNDROME_ABORTED_ERROR                     = 0x22,
};

struct mlx5_ifc_cqe_error_syndrome_bits {
	u8         hw_error_syndrome[0x8];
	u8         hw_syndrome_type[0x4];
	u8         reserved_at_c[0x4];
	u8         vendor_error_syndrome[0x8];
	u8         syndrome[0x8];
};

enum {
	MLX5_CRYPTO_CAPS_WRAPPED_IMPORT_METHOD_TLS    = 0x0,
	MLX5_CRYPTO_CAPS_WRAPPED_IMPORT_METHOD_IPSEC  = 0x1,
	MLX5_CRYPTO_CAPS_WRAPPED_IMPORT_METHOD_AES    = 0x2,
};

enum {
	MLX5_CRYPTO_CAPS_FAILED_SELFTESTS_TEST_AES_GCM   = 0x1,
	MLX5_CRYPTO_CAPS_FAILED_SELFTESTS_TEST_AES_ECB   = 0x2,
	MLX5_CRYPTO_CAPS_FAILED_SELFTESTS_TEST_AES_XTS   = 0x4,
	MLX5_CRYPTO_CAPS_FAILED_SELFTESTS_TEST_HMAC_SHA  = 0x8,
	MLX5_CRYPTO_CAPS_FAILED_SELFTESTS_TEST_SHA       = 0x10,
};

enum {
	MLX5_CRYPTO_CAPS_SW_WRAPPED_DEK_TLS      = 0x2,
	MLX5_CRYPTO_CAPS_SW_WRAPPED_DEK_IPSEC    = 0x4,
	MLX5_CRYPTO_CAPS_SW_WRAPPED_DEK_AES_XTS  = 0x8,
};

struct mlx5_ifc_crypto_caps_bits {
	u8         wrapped_crypto_operational[0x1];
	u8         wrapped_crypto_going_to_commissioning[0x1];
	u8         reserved_at_2[0x1];
	u8         synchronize_dek[0x1];
	u8         int_kek_manual[0x1];
	u8         int_kek_auto[0x1];
	u8         reserved_at_6[0x6];
	u8         int_kek_manual_size[0x4];
	u8         reserved_at_10[0x3];
	u8         sw_wrapped_dek_key_purpose[0x1];
	u8         int_kek_auto_size[0x4];
	u8         wrapped_import_method[0x8];

	u8         reserved_at_20[0x3];
	u8         log_dek_max_alloc[0x5];
	u8         reserved_at_28[0x3];
	u8         log_max_num_deks[0x5];
	u8         reserved_at_30[0x3];
	u8         log_max_num_import_keks[0x5];
	u8         reserved_at_38[0x3];
	u8         log_max_num_creds[0x5];

	u8         failed_selftests[0x10];
	u8         num_nv_import_keks[0x8];
	u8         num_nv_credentials[0x8];

	u8         reserved_at_60[0x3];
	u8         log_dek_granularity[0x5];
	u8         reserved_at_68[0x3];
	u8         log_max_num_int_kek[0x5];
	u8         sw_wrapped_dek[0x10];

	u8         reserved_at_80[0x780];
};

struct mlx5_ifc_debug_cap_bits {
	u8         core_dump_general[0x1];
	u8         core_dump_qp[0x1];
	u8         reserved_at_2[0x2];
	u8         log_cr_dump_to_mem_size[0x5];
	u8         resource_dump[0x1];
	u8         reserved_at_a[0x1];
	u8         log_min_resource_dump_eq[0x5];
	u8         reserved_at_10[0x8];
	u8         log_max_samples[0x8];

	u8         single[0x1];
	u8         repetitive[0x1];
	u8         health_mon_rx_activity[0x1];
	u8         diag_counter_tracer_dump[0x1];
	u8         reserved_at_24[0x14];
	u8         log_min_sample_period[0x8];

	u8         reserved_at_40[0x1c0];

	struct mlx5_ifc_diagnostic_cntr_layout_bits diagnostic_counter[0];
};

struct mlx5_ifc_device_emulation_cap_bits {
	u8         nvme_offload_type_sqe[0x1];
	u8         reserved_at_1[0x1];
	u8         nvme_offload_type_command_capsule[0x1];
	u8         log_max_nvme_offload_namespaces[0x5];
	u8         nvme_cq_interrupt_disabled[0x1];
	u8         nvme_emulation_object_event[0x1];
	u8         reserved_at_a[0x6];
	u8         max_emulated_devices[0x10];

	u8         reserved_at_20[0xb];
	u8         log_max_nvme_offload_nsid[0x5];
	u8         registers_size[0x10];

	u8         log_max_queue_depth[0x10];
	u8         reserved_at_50[0x3];
	u8         log_max_emulated_sq[0x5];
	u8         reserved_at_58[0x3];
	u8         log_max_emulated_cq[0x5];

	u8         reserved_at_60[0x7a0];
};

enum {
	MLX5_DEVICE_MEM_CAP_MEMIC_ATOMIC_INCREMENT_SIZE_1BYTE   = 0x1,
	MLX5_DEVICE_MEM_CAP_MEMIC_ATOMIC_INCREMENT_SIZE_2BYTE   = 0x2,
	MLX5_DEVICE_MEM_CAP_MEMIC_ATOMIC_INCREMENT_SIZE_4BYTE   = 0x4,
	MLX5_DEVICE_MEM_CAP_MEMIC_ATOMIC_INCREMENT_SIZE_8BYTE   = 0x8,
	MLX5_DEVICE_MEM_CAP_MEMIC_ATOMIC_INCREMENT_SIZE_16BYTE  = 0x10,
};

enum {
	MLX5_DEVICE_MEM_CAP_MEMIC_ATOMIC_TEST_AND_SET_SIZE_1BYTE   = 0x1,
	MLX5_DEVICE_MEM_CAP_MEMIC_ATOMIC_TEST_AND_SET_SIZE_2BYTE   = 0x2,
	MLX5_DEVICE_MEM_CAP_MEMIC_ATOMIC_TEST_AND_SET_SIZE_4BYTE   = 0x4,
	MLX5_DEVICE_MEM_CAP_MEMIC_ATOMIC_TEST_AND_SET_SIZE_8BYTE   = 0x8,
	MLX5_DEVICE_MEM_CAP_MEMIC_ATOMIC_TEST_AND_SET_SIZE_16BYTE  = 0x10,
};

enum {
	MLX5_DEVICE_MEM_CAP_MEMIC_OPERATIONS_MEMIC_ATOMIC_INCREMENT     = 0x0,
	MLX5_DEVICE_MEM_CAP_MEMIC_OPERATIONS_MEMIC_ATOMIC_TEST_AND_SET  = 0x1,
};

struct mlx5_ifc_device_mem_cap_bits {
	u8         memic[0x1];
	u8         reserved_at_1[0x1f];

	u8         memic_atomic_increment[0x8];
	u8         reserved_at_28[0x3];
	u8         log_min_memic_alloc_size[0x5];
	u8         memic_atomic_test_and_set[0x8];
	u8         log_max_memic_addr_alignment[0x8];

	u8         memic_bar_start_addr[0x40];

	u8         memic_bar_size[0x20];

	u8         max_memic_size[0x20];

	u8         steering_sw_icm_start_address[0x40];

	u8         must_not_use[0x8];
	u8         log_header_modify_sw_icm_size[0x8];
	u8         reserved_at_110[0x2];
	u8         log_sw_icm_alloc_granularity[0x6];
	u8         log_steering_sw_icm_size[0x8];

	u8         log_indirect_encap_sw_icm_size[0x8];
	u8         reserved_at_128[0x10];
	u8         log_header_modify_pattern_sw_icm_size[0x8];

	u8         header_modify_sw_icm_start_address[0x40];

	u8         reserved_at_180[0x40];

	u8         header_modify_pattern_sw_icm_start_address[0x40];

	u8         memic_operations[0x20];

	u8         reserved_at_220[0x20];

	u8         indirect_encap_sw_icm_start_address[0x40];

	u8         indirect_encap_icm_base[0x40];

	u8         reserved_at_2c0[0x540];
};

struct mlx5_ifc_device_pci_params_bits {
	u8         device_id[0x10];
	u8         vendor_id[0x10];

	u8         revision_id[0x8];
	u8         class_code[0x18];

	u8         subsystem_id[0x10];
	u8         subsystem_vendor_id[0x10];

	u8         num_of_vfs[0x10];
	u8         num_msix[0x10];
};

struct mlx5_ifc_dpa_capabilities_bits {
	u8         thread_affinity_single_hart[0x1];
	u8         thread_affinity_hart_group[0x1];
	u8         reserved_at_2[0x1];
	u8         log_max_num_dpa_mem_blocks[0x5];
	u8         process_win_primary[0x1];
	u8         process_win_secondary[0x1];
	u8         reserved_at_a[0x1];
	u8         log_max_dpa_mem[0x5];
	u8         dpa_auth_supported[0x1];
	u8         dpa_auth_enabled[0x1];
	u8         reserved_at_12[0x1];
	u8         log_max_dpa_threads_per_process[0x5];
	u8         reserved_at_18[0x3];
	u8         log_max_dpa_process[0x5];

	u8         dpa_mem_block_size[0x20];

	u8         reserved_at_40[0x3];
	u8         log_max_dpa_threads[0x5];
	u8         reserved_at_48[0x3];
	u8         log_max_dpa_outbox[0x5];
	u8         max_num_dpa_thread_per_group[0x10];

	u8         reserved_at_60[0x3];
	u8         log_max_dpa_window[0x5];
	u8         reserved_at_68[0x3];
	u8         log_max_dpa_partitions[0x5];
	u8         reserved_at_70[0x3];
	u8         log_max_harts_per_partition[0x5];
	u8         reserved_at_78[0x3];
	u8         log_max_vhca_per_partition[0x5];

	u8         dpa_coredump_type[0x8];
	u8         reserved_at_88[0x18];

	u8         reserved_at_a0[0x760];
};

struct mlx5_ifc_dpp_cap_bits {
	u8         dpp_wire_protocol[0x40];

	u8         max_scatter_size[0x10];
	u8         reserved_at_50[0x3];
	u8         log_min_dpp_segment_size[0x5];
	u8         reserved_at_58[0x3];
	u8         log_max_dpp_segment_size[0x5];

	u8         reserved_at_60[0x8];
	u8         dpp_max_scatter_offset[0x8];
	u8         reserved_at_70[0x3];
	u8         log_max_dpp_buffer_size[0x5];
	u8         max_psn_size_supported[0x8];

	u8         reserved_at_80[0x780];
};

enum {
	MLX5_E_SWITCH_CAP_COUNTER_ESWITCH_AFFINITY_SOURCE_ESWITCH  = 0x0,
	MLX5_E_SWITCH_CAP_COUNTER_ESWITCH_AFFINITY_FLOW_ESWITCH    = 0x1,
};

struct mlx5_ifc_e_switch_cap_bits {
	u8         vport_svlan_strip[0x1];
	u8         vport_cvlan_strip[0x1];
	u8         vport_svlan_insert[0x1];
	u8         vport_cvlan_insert_if_not_exist[0x1];
	u8         vport_cvlan_insert_overwrite[0x1];
	u8         reserved_at_5[0x1];
	u8         vport_cvlan_insert_always[0x1];
	u8         esw_shared_ingress_acl[0x1];
	u8         esw_uplink_ingress_acl[0x1];
	u8         root_ft_on_other_esw[0x1];
	u8         sw_network_metadata[0x1];
	u8         reserved_at_b[0xd];
	u8         match_uplink_other_esw[0x1];
	u8         esw_functions_changed[0x1];
	u8         fdb_single_tir_destination[0x1];
	u8         ecpf_vport_exists[0x1];
	u8         counter_eswitch_affinity[0x1];
	u8         merged_eswitch[0x1];
	u8         nic_vport_node_guid_modify[0x1];
	u8         nic_vport_port_guid_modify[0x1];

	u8         vxlan_encap_decap[0x1];
	u8         nvgre_encap_decap[0x1];
	u8         encap_general_header[0x1];
	u8         log_max_fdb_encap_uplink[0x5];
	u8         reserved_at_28[0x3];
	u8         log_max_packet_reformat_context[0x5];
	u8         reserved_at_30[0x6];
	u8         max_encap_header_size[0xa];

	u8         reserved_at_40[0xb];
	u8         log_max_esw_sf[0x5];
	u8         esw_sf_base_id[0x10];

	u8         esw_manager_vport_number_valid[0x1];
	u8         reserved_at_61[0xf];
	u8         esw_manager_vport_number[0x10];

	u8         reserved_at_80[0x780];
};

enum {
	MLX5_EVENT_CAPABILITIES_USER_AFFILIATED_EVENTS_63_0_PATH_MIGRATED_SUCCEEDED          = 0x2,
	MLX5_EVENT_CAPABILITIES_USER_AFFILIATED_EVENTS_63_0_COMMUNICATION_ESTABLISHED        = 0x4,
	MLX5_EVENT_CAPABILITIES_USER_AFFILIATED_EVENTS_63_0_SEND_QUEUE_DRAINED               = 0x8,
	MLX5_EVENT_CAPABILITIES_USER_AFFILIATED_EVENTS_63_0_CQ_ERROR                         = 0x10,
	MLX5_EVENT_CAPABILITIES_USER_AFFILIATED_EVENTS_63_0_LOCAL_WQ_CATASTROPHIC_ERROR      = 0x20,
	MLX5_EVENT_CAPABILITIES_USER_AFFILIATED_EVENTS_63_0_PATH_MIGRATION_FAILED            = 0x80,
	MLX5_EVENT_CAPABILITIES_USER_AFFILIATED_EVENTS_63_0_INVALID_REQUEST_LOCAL_WQ_ERROR   = 0x10000,
	MLX5_EVENT_CAPABILITIES_USER_AFFILIATED_EVENTS_63_0_LOCAL_ACCESS_VIOLATION_WQ_ERROR  = 0x20000,
	MLX5_EVENT_CAPABILITIES_USER_AFFILIATED_EVENTS_63_0_LOCAL_SRQ_CATASTROPHIC_ERROR     = 0x40000,
	MLX5_EVENT_CAPABILITIES_USER_AFFILIATED_EVENTS_63_0_LAST_WQE_REACHED                 = 0x80000,
	MLX5_EVENT_CAPABILITIES_USER_AFFILIATED_EVENTS_63_0_SRQ_LIMIT                        = 0x100000,
	MLX5_EVENT_CAPABILITIES_USER_AFFILIATED_EVENTS_63_0_XRQ_ERROR                        = 0x1000000,
	MLX5_EVENT_CAPABILITIES_USER_AFFILIATED_EVENTS_63_0_DCT_ALL_CONNECTIONS_CLOSED       = 0x10000000,
	MLX5_EVENT_CAPABILITIES_USER_AFFILIATED_EVENTS_63_0_DCT_ACCESS_KEY_VIOLATION         = 0x20000000,
	MLX5_EVENT_CAPABILITIES_USER_AFFILIATED_EVENTS_63_0_OBJECT_CHANGE_EVENT              = 0x80,
};

enum {
	MLX5_EVENT_CAPABILITIES_USER_UNAFFILIATED_EVENTS_63_0_PORT_STATE_CHANGE           = 0x200,
	MLX5_EVENT_CAPABILITIES_USER_UNAFFILIATED_EVENTS_63_0_NIC_VPORT_CHANGE            = 0x2000,
	MLX5_EVENT_CAPABILITIES_USER_UNAFFILIATED_EVENTS_63_0_HOST_NET_FUNCTIONS_CHANGED  = 0x4000,
	MLX5_EVENT_CAPABILITIES_USER_UNAFFILIATED_EVENTS_63_0_PORT_MODULE_EVENT           = 0x400000,
	MLX5_EVENT_CAPABILITIES_USER_UNAFFILIATED_EVENTS_63_0_TEMP_WARNING_EVENT          = 0x800000,
	MLX5_EVENT_CAPABILITIES_USER_UNAFFILIATED_EVENTS_63_0_PPS_EVENT                   = 0x20,
	MLX5_EVENT_CAPABILITIES_USER_UNAFFILIATED_EVENTS_63_0_GENERAL_NOTIFICATION_EVENT  = 0x4,
};

struct mlx5_ifc_event_capabilities_bits {
	u8         user_affiliated_events_63_0[0x40];

	u8         user_affiliated_events_127_64[0x40];

	u8         user_affiliated_events_191_128[0x40];

	u8         user_affiliated_events_255_192[0x40];

	u8         user_unaffiliated_events_63_0[0x40];

	u8         user_unaffiliated_events_127_64[0x40];

	u8         user_unaffiliated_events_191_128[0x40];

	u8         user_unaffiliated_events_255_192[0x40];

	u8         reserved_at_200[0x600];
};

enum {
	MLX5_FLOW_TABLE_ESWITCH_CAP_FDB_TO_VPORT_REG_C_ID_REG_C_0  = 0x1,
	MLX5_FLOW_TABLE_ESWITCH_CAP_FDB_TO_VPORT_REG_C_ID_REG_C_1  = 0x2,
	MLX5_FLOW_TABLE_ESWITCH_CAP_FDB_TO_VPORT_REG_C_ID_REG_C_2  = 0x4,
	MLX5_FLOW_TABLE_ESWITCH_CAP_FDB_TO_VPORT_REG_C_ID_REG_C_3  = 0x8,
	MLX5_FLOW_TABLE_ESWITCH_CAP_FDB_TO_VPORT_REG_C_ID_REG_C_4  = 0x10,
	MLX5_FLOW_TABLE_ESWITCH_CAP_FDB_TO_VPORT_REG_C_ID_REG_C_5  = 0x20,
	MLX5_FLOW_TABLE_ESWITCH_CAP_FDB_TO_VPORT_REG_C_ID_REG_C_6  = 0x40,
	MLX5_FLOW_TABLE_ESWITCH_CAP_FDB_TO_VPORT_REG_C_ID_REG_C_7  = 0x80,
};

struct mlx5_ifc_flow_table_eswitch_cap_bits {
	u8         fdb_to_vport_reg_c_id[0x8];
	u8         reserved_at_8[0x8];
	u8         fdb_dynamic_tunnel[0x1];
	u8         vport_direction[0x1];
	u8         fdb_multi_path_any_table[0x1];
	u8         ingress_acl_forward_to_uplink[0x1];
	u8         ingress_acl_flow_counter_for_drop_actions[0x1];
	u8         esw_fdb_modify_header_fwd_to_table[0x1];
	u8         esw_fdb_ipv4_ttl_modify[0x1];
	u8         flow_source[0x1];
	u8         ingress_acl_forward_to_vport_ingress[0x1];
	u8         extended_fdb_flow_counter_en[0x1];
	u8         multi_fdb_encap[0x1];
	u8         egress_acl_forward_to_vport[0x1];
	u8         fdb_multi_path_to_table[0x1];
	u8         ingress_acl_forward_to_vport[0x1];
	u8         fdb_flow_counter_en[0x2];

	u8         reserved_at_20[0x1e0];

	struct mlx5_ifc_flow_table_prop_layout_bits flow_table_properties_nic_esw_fdb;

	struct mlx5_ifc_flow_table_prop_layout_bits flow_table_properties_esw_acl_ingress;

	struct mlx5_ifc_flow_table_prop_layout_bits flow_table_properties_esw_acl_egress;

	struct mlx5_ifc_header_modify_cap_properties_bits header_modify_esw_fdb;

	struct mlx5_ifc_header_modify_cap_properties_bits header_modify_esw_acl_ingress;

	struct mlx5_ifc_header_modify_cap_properties_bits header_modify_esw_acl_egress;

	struct mlx5_ifc_flow_table_fields_supported_2_bits ft_field_support_2_esw_fdb;

	struct mlx5_ifc_flow_table_fields_supported_2_bits ft_field_bitmask_support_2_esw_fdb;

	struct mlx5_ifc_flow_table_fields_supported_2_bits ft_field_support_2_esw_acl_ingress;

	struct mlx5_ifc_flow_table_fields_supported_2_bits ft_field_bitmask_support_2_esw_acl_ingress;

	struct mlx5_ifc_flow_table_fields_supported_2_bits ft_field_support_2_esw_acl_egress;

	struct mlx5_ifc_flow_table_fields_supported_2_bits ft_field_bitmask_support_2_esw_acl_egress;

	u8         reserved_at_1700[0x100];

	u8         sw_steering_fdb_action_drop_icm_address_rx[0x40];

	u8         sw_steering_fdb_action_drop_icm_address_tx[0x40];

	u8         sw_steering_uplink_icm_address_rx[0x40];

	u8         sw_steering_uplink_icm_address_tx[0x40];

	u8         reserved_at_1900[0x6700];
};

struct mlx5_ifc_flow_table_nic_cap_bits {
	u8         nic_rx_multi_path_tirs[0x1];
	u8         nic_rx_multi_path_tirs_fts[0x1];
	u8         allow_sniffer_and_nic_rx_shared_tir[0x1];
	u8         reserved_at_3[0x1];
	u8         nic_rx_flow_tag_multipath_en[0x1];
	u8         ttl_checksum_correction[0x1];
	u8         nic_rx_rdma_fwd_tir[0x1];
	u8         sw_owner_reformat_supported[0x1];
	u8         reserved_at_8[0x10];
	u8         nic_receive_max_steering_depth[0x8];

	u8         encap_general_header[0x1];
	u8         reserved_at_21[0xa];
	u8         log_max_packet_reformat_context[0x5];
	u8         reserved_at_30[0x6];
	u8         max_encap_header_size[0xa];

	u8         reserved_at_40[0x1c0];

	struct mlx5_ifc_flow_table_prop_layout_bits flow_table_properties_nic_receive;

	struct mlx5_ifc_flow_table_prop_layout_bits flow_table_properties_nic_receive_rdma;

	struct mlx5_ifc_flow_table_prop_layout_bits flow_table_properties_nic_receive_sniffer;

	struct mlx5_ifc_flow_table_prop_layout_bits flow_table_properties_nic_transmit;

	struct mlx5_ifc_flow_table_prop_layout_bits flow_table_properties_nic_transmit_rdma;

	struct mlx5_ifc_flow_table_prop_layout_bits flow_table_properties_nic_transmit_sniffer;

	u8         reserved_at_e00[0x200];

	struct mlx5_ifc_header_modify_cap_properties_bits header_modify_nic_receive;

	struct mlx5_ifc_flow_table_fields_supported_2_bits ft_field_support_2_nic_receive;

	struct mlx5_ifc_flow_table_fields_supported_2_bits ft_field_bitmask_support_2_nic_receive;

	struct mlx5_ifc_flow_table_fields_supported_2_bits ft_field_support_2_nic_receive_rdma;

	struct mlx5_ifc_flow_table_fields_supported_2_bits ft_field_bitmask_support_2_nic_receive_rdma;

	struct mlx5_ifc_flow_table_fields_supported_2_bits ft_field_support_2_nic_receive_sniffer;

	struct mlx5_ifc_flow_table_fields_supported_2_bits ft_field_bitmask_support_2_nic_receive_sniffer;

	struct mlx5_ifc_flow_table_fields_supported_2_bits ft_field_support_2_nic_transmit;

	struct mlx5_ifc_flow_table_fields_supported_2_bits ft_field_bitmask_support_2_nic_transmit;

	struct mlx5_ifc_flow_table_fields_supported_2_bits ft_field_support_2_nic_transmit_rdma;

	struct mlx5_ifc_flow_table_fields_supported_2_bits ft_field_bitmask_support_2_nic_transmit_rdma;

	struct mlx5_ifc_flow_table_fields_supported_2_bits ft_field_support_2_nic_transmit_sniffer;

	struct mlx5_ifc_flow_table_fields_supported_2_bits ft_field_bitmask_support_2_nic_transmit_sniffer;

	u8         reserved_at_1a00[0x200];

	struct mlx5_ifc_header_modify_cap_properties_bits header_modify_nic_transmit;

	u8         sw_steering_nic_rx_action_drop_icm_address[0x40];

	u8         sw_steering_nic_tx_action_drop_icm_address[0x40];

	u8         sw_steering_nic_tx_action_allow_icm_address[0x40];

	u8         reserved_at_20c0[0x5f40];
};

enum {
	MLX5_HCA_CAPS_2_CROSS_VHCA_OBJECT_TO_OBJECT_SUPPORTED_LOCAL_CQ_TO_REMOTE_DPA_UMEM                 = 0x1,
	MLX5_HCA_CAPS_2_CROSS_VHCA_OBJECT_TO_OBJECT_SUPPORTED_LOCAL_SQ_TO_REMOTE_DPA_UMEM                 = 0x2,
	MLX5_HCA_CAPS_2_CROSS_VHCA_OBJECT_TO_OBJECT_SUPPORTED_LOCAL_RQ_TO_REMOTE_DPA_UMEM                 = 0x4,
	MLX5_HCA_CAPS_2_CROSS_VHCA_OBJECT_TO_OBJECT_SUPPORTED_LOCAL_QP_TO_REMOTE_DPA_UMEM                 = 0x8,
	MLX5_HCA_CAPS_2_CROSS_VHCA_OBJECT_TO_OBJECT_SUPPORTED_LOCAL_CQ_TO_REMOTE_DPA_THREAD               = 0x10,
	MLX5_HCA_CAPS_2_CROSS_VHCA_OBJECT_TO_OBJECT_SUPPORTED_LOCAL_NVME_SQ_BE_REMOTE_NVME_SQ             = 0x20,
	MLX5_HCA_CAPS_2_CROSS_VHCA_OBJECT_TO_OBJECT_SUPPORTED_LOCAL_DPA_WINDOW_TO_REMOTE_PD               = 0x40,
	MLX5_HCA_CAPS_2_CROSS_VHCA_OBJECT_TO_OBJECT_SUPPORTED_LOCAL_DPA_OUTBOX_TO_REMOTE_UAR              = 0x80,
	MLX5_HCA_CAPS_2_CROSS_VHCA_OBJECT_TO_OBJECT_SUPPORTED_LOCAL_MKEY_TO_REMOTE_MKEY                   = 0x100,
	MLX5_HCA_CAPS_2_CROSS_VHCA_OBJECT_TO_OBJECT_SUPPORTED_LOCAL_CQ_TO_REMOTE_EMULATED_DEV_EQ          = 0x200,
	MLX5_HCA_CAPS_2_CROSS_VHCA_OBJECT_TO_OBJECT_SUPPORTED_LOCAL_STC_TO_REMOTE_TIR                     = 0x400,
	MLX5_HCA_CAPS_2_CROSS_VHCA_OBJECT_TO_OBJECT_SUPPORTED_LOCAL_STC_TO_REMOTE_FLOW_TABLE              = 0x800,
	MLX5_HCA_CAPS_2_CROSS_VHCA_OBJECT_TO_OBJECT_SUPPORTED_LOCAL_RTC_TO_REMOTE_FLOW_TABLE              = 0x1000,
	MLX5_HCA_CAPS_2_CROSS_VHCA_OBJECT_TO_OBJECT_SUPPORTED_LOCAL_FLOW_TABLE_TO_REMOTE_RTC              = 0x2000,
	MLX5_HCA_CAPS_2_CROSS_VHCA_OBJECT_TO_OBJECT_SUPPORTED_LOCAL_FLOW_TABLE_TO_REMOTE_FLOW_TABLE_MISS  = 0x4000,
};

enum {
	MLX5_HCA_CAPS_2_ALLOWED_OBJECT_FOR_OTHER_VHCA_ACCESS_PD               = 0x1,
	MLX5_HCA_CAPS_2_ALLOWED_OBJECT_FOR_OTHER_VHCA_ACCESS_UAR              = 0x2,
	MLX5_HCA_CAPS_2_ALLOWED_OBJECT_FOR_OTHER_VHCA_ACCESS_MKEY             = 0x4,
	MLX5_HCA_CAPS_2_ALLOWED_OBJECT_FOR_OTHER_VHCA_ACCESS_UMEM             = 0x8,
	MLX5_HCA_CAPS_2_ALLOWED_OBJECT_FOR_OTHER_VHCA_ACCESS_EQ               = 0x10,
	MLX5_HCA_CAPS_2_ALLOWED_OBJECT_FOR_OTHER_VHCA_ACCESS_NVME_SQ          = 0x80,
	MLX5_HCA_CAPS_2_ALLOWED_OBJECT_FOR_OTHER_VHCA_ACCESS_TIR              = 0x100,
	MLX5_HCA_CAPS_2_ALLOWED_OBJECT_FOR_OTHER_VHCA_ACCESS_FLOW_TABLE       = 0x200,
	MLX5_HCA_CAPS_2_ALLOWED_OBJECT_FOR_OTHER_VHCA_ACCESS_RTC              = 0x400,
	MLX5_HCA_CAPS_2_ALLOWED_OBJECT_FOR_OTHER_VHCA_ACCESS_EMULATED_DEV_EQ  = 0x800,
	MLX5_HCA_CAPS_2_ALLOWED_OBJECT_FOR_OTHER_VHCA_ACCESS_DPA_THREAD       = 0x800,
};

struct mlx5_ifc_cmd_hca_cap_2_bits {
	u8         reserved_at_0[0x80];

	u8         migratable[0x1];
	u8         force_multi_prio_sq[0x1];
	u8         cq_with_emulated_dev_eq[0x1];
	u8         max_num_prog_sample_field[0x5];
	u8         multi_path_force[0x1];
	u8         fw_cpu_monitoring[0x1];
	u8         enh_eth_striding_wq[0x1];
	u8         log_max_num_reserved_qpn[0x5];
	u8         reserved_at_90[0x1];
	u8         introspection_mkey_access_allowed[0x1];
	u8         query_vuid[0x1];
	u8         log_reserved_qpn_granularity[0x5];
	u8         reserved_at_98[0x3];
	u8         log_reserved_qpn_max_alloc[0x5];

	u8         max_reformat_insert_size[0x8];
	u8         max_reformat_insert_offset[0x8];
	u8         max_reformat_remove_size[0x8];
	u8         max_reformat_remove_offset[0x8];

	u8         multi_sl_qp[0x1];
	u8         non_tunnel_reformat[0x1];
	u8         bsf_v2[0x1];
	u8         log_min_stride_wqe_sz[0x5];
	u8         migration_multi_load[0x1];
	u8         migration_tracking_state[0x1];
	u8         reserved_at_ca[0x1];
	u8         log_conn_track_granularity[0x5];
	u8         reserved_at_d0[0x3];
	u8         log_conn_track_max_alloc[0x5];
	u8         reserved_at_d8[0x3];
	u8         log_max_conn_track_offload[0x5];

	u8         cross_vhca_object_to_object_supported[0x20];

	u8         allowed_object_for_other_vhca_access[0x40];

	u8         introspection_mkey[0x20];

	u8         ec_mmo_qp[0x1];
	u8         sync_driver_version[0x1];
	u8         driver_version_change_event[0x1];
	u8         hairpin_sq_wqe_bb_size[0x5];
	u8         hairpin_sq_wq_in_host_mem[0x1];
	u8         hairpin_data_buffer_locked[0x1];
	u8         reserved_at_16a[0x1];
	u8         log_ec_mmo_max_size[0x5];
	u8         reserved_at_170[0x3];
	u8         log_ec_mmo_max_src[0x5];
	u8         reserved_at_178[0x3];
	u8         log_ec_mmo_max_dst[0x5];

	u8         sync_driver_actions[0x20];

	u8         flow_table_type_2_type[0x8];
	u8         reserved_at_1a8[0x2];
	u8         format_select_dw_8_6_ext[0x1];
	u8         log_min_mkey_entity_size[0x5];
	u8         execute_aso_type[0x10];

	u8         general_obj_types_127_64[0x40];

	u8         repeated_mkey_v2[0x20];

	u8         reserved_gid_index_valid[0x1];
	u8         sw_vhca_id_valid[0x1];
	u8         sw_vhca_id[0xe];
	u8         reserved_gid_index[0x10];

	u8         reserved_at_240[0x3];
	u8         log_max_channel_service_connection[0x5];
	u8         reserved_at_248[0x3];
	u8         ts_cqe_metadata_size2wqe_counter[0x5];
	u8         reserved_at_250[0x3];
	u8         flow_counter_bulk_log_max_alloc[0x5];
	u8         reserved_at_258[0x3];
	u8         flow_counter_bulk_log_granularity[0x5];

	u8         format_select_dw_mpls_over_x_cw[0x8];
	u8         format_select_dw_geneve_tlv_option_0[0x8];
	u8         format_select_dw_outer_first_mpls_over_gre[0x8];
	u8         format_select_dw_outer_first_mpls_over_udp[0x8];

	u8         format_select_dw_gtpu_dw_0[0x8];
	u8         format_select_dw_gtpu_dw_1[0x8];
	u8         format_select_dw_gtpu_dw_2[0x8];
	u8         format_select_dw_gtpu_first_ext_dw_0[0x8];

	u8         generate_wqe_type[0x20];

	u8         max_enh_strwq_supported_profile[0x10];
	u8         reserved_at_2d0[0x3];
	u8         log_max_total_hairpin_data_buffer_locked_size[0x5];
	u8         reserved_at_2d8[0x3];
	u8         log_max_rq_hairpin_data_buffer_locked_size[0x5];

	u8         send_dbr_mode_no_dbr_int[0x1];
	u8         send_dbr_mode_no_dbr_ext[0x1];
	u8         reserved_at_2e2[0x1];
	u8         log_max_send_dbr_less_qp_sq[0x5];
	u8         reserved_at_2e8[0x3];
	u8         enh_strwq_max_log_page_size[0x5];
	u8         enh_strwq_max_headroom[0x8];
	u8         enh_strwq_max_tailroom[0x8];

	u8         migration_tag_version_0[4][0x20];

	u8         reserved_at_380[0x3];
	u8         log_max_hairpin_wqe_num[0x5];
	u8         reserved_at_388[0x8];
	u8         ec_vf_base_vport_number[0x10];

	u8         reserved_at_3a0[0x10];
	u8         max_rqt_vhca_id[0x10];

	u8         reserved_at_3c0[0x440];
};

enum {
	MLX5_HOTPLUG_CAPABILITIES_HOTPLUG_DEVICE_TYPES_SUPPORTED_NVME        = 0x1,
	MLX5_HOTPLUG_CAPABILITIES_HOTPLUG_DEVICE_TYPES_SUPPORTED_VIRTIO_NET  = 0x2,
	MLX5_HOTPLUG_CAPABILITIES_HOTPLUG_DEVICE_TYPES_SUPPORTED_VIRTIO_BLK  = 0x4,
	MLX5_HOTPLUG_CAPABILITIES_HOTPLUG_DEVICE_TYPES_SUPPORTED_VIRTIO_FS   = 0x10,
	MLX5_HOTPLUG_CAPABILITIES_HOTPLUG_DEVICE_TYPES_SUPPORTED_UPT         = 0x20,
};

struct mlx5_ifc_hotplug_capabilities_bits {
	u8         hotplug_device_types_supported[0x10];
	u8         log_max_bar_size[0x8];
	u8         max_hotplug_devices[0x8];

	u8         pci_hotplug_state_change[0x1];
	u8         virtio_transitional_device_hotplug[0x1];
	u8         reserved_at_22[0xe];
	u8         max_total_vfs[0x10];

	u8         max_num_hotplug_pt_special_region[0x10];
	u8         reserved_at_50[0x8];
	u8         max_num_hotplug_pt_function[0x8];

	u8         reserved_at_60[0x7a0];
};

union mlx5_ifc_hotplug_device_obj_emulation_initial_registers_auto_bits {
	struct mlx5_ifc_nvme_initial_registers_bits nvme_initial_registers;
	struct mlx5_ifc_virtio_net_initial_registers_bits virtio_net_initial_registers;
	struct mlx5_ifc_virtio_blk_initial_registers_bits virtio_blk_initial_registers;
	struct mlx5_ifc_virtio_fs_initial_registers_bits virtio_fs_initial_registers;
	u8         reserved_at_0[0x600];
};

enum {
	MLX5_IPSEC_ASO_MODE_NO_OPTIONAL_OP      = 0x0,
	MLX5_IPSEC_ASO_MODE_REPLAY_PROTECTION   = 0x1,
	MLX5_IPSEC_ASO_MODE_INCREMENT_IPSEC_SN  = 0x2,
};

enum {
	MLX5_IPSEC_ASO_WINDOW_SIZE_WIN_32BIT   = 0x0,
	MLX5_IPSEC_ASO_WINDOW_SIZE_WIN_64BIT   = 0x1,
	MLX5_IPSEC_ASO_WINDOW_SIZE_WIN_128BIT  = 0x2,
	MLX5_IPSEC_ASO_WINDOW_SIZE_WIN_256BIT  = 0x3,
};

struct mlx5_ifc_ipsec_aso_bits {
	u8         valid[0x1];
	u8         reserved_at_1[0x1];
	u8         mode[0x2];
	u8         window_size[0x2];
	u8         soft_lifetime_arm[0x1];
	u8         hard_lifetime_arm[0x1];
	u8         remove_flow_enable[0x1];
	u8         esn_event_arm[0x1];
	u8         reserved_at_a[0x16];

	u8         remove_flow_packet_count[0x20];

	u8         remove_flow_soft_lifetime[0x20];

	u8         reserved_at_60[0x80];

	u8         mode_parameter[0x20];

	u8         replay_protection_window[8][0x20];
};

struct mlx5_ifc_ipsec_cap_bits {
	u8         ipsec_full_offload[0x1];
	u8         ipsec_crypto_offload[0x1];
	u8         ipsec_esn[0x1];
	u8         ipsec_crypto_esp_aes_gcm_256_encrypt[0x1];
	u8         ipsec_crypto_esp_aes_gcm_128_encrypt[0x1];
	u8         ipsec_crypto_esp_aes_gcm_256_decrypt[0x1];
	u8         ipsec_crypto_esp_aes_gcm_128_decrypt[0x1];
	u8         reserved_at_7[0x4];
	u8         log_max_ipsec_offload[0x5];
	u8         reserved_at_10[0x10];

	u8         min_log_ipsec_full_replay_window[0x8];
	u8         max_log_ipsec_full_replay_window[0x8];
	u8         reserved_at_30[0x10];

	u8         reserved_at_40[0x7c0];
};

struct mlx5_ifc_macsec_cap_bits {
	u8         macsec_epn[0x1];
	u8         reserved_at_1[0x2];
	u8         macsec_crypto_esp_aes_gcm_256_encrypt[0x1];
	u8         macsec_crypto_esp_aes_gcm_128_encrypt[0x1];
	u8         macsec_crypto_esp_aes_gcm_256_decrypt[0x1];
	u8         macsec_crypto_esp_aes_gcm_128_decrypt[0x1];
	u8         reserved_at_7[0x4];
	u8         log_max_macsec_offload[0x5];
	u8         reserved_at_10[0x10];

	u8         min_log_macsec_full_replay_window[0x8];
	u8         max_log_macsec_full_replay_window[0x8];
	u8         reserved_at_30[0x10];

	u8         reserved_at_40[0x7c0];
};

union mlx5_ifc_nic_vport_context_current_uc_mac_address_auto_bits {
	struct mlx5_ifc_mac_address_layout_bits mac_address_layout;
	struct mlx5_ifc_vlan_layout_bits vlan_layout;
	u8         reserved_at_0[0x40];
};

struct mlx5_ifc_nisp_cap_bits {
	u8         reserved_at_0[0x1];
	u8         nisp_crypto_offload[0x1];
	u8         reserved_at_2[0x1];
	u8         nisp_crypto_esp_aes_gcm_256_encrypt[0x1];
	u8         nisp_crypto_esp_aes_gcm_128_encrypt[0x1];
	u8         nisp_crypto_esp_aes_gcm_256_decrypt[0x1];
	u8         nisp_crypto_esp_aes_gcm_128_decrypt[0x1];
	u8         reserved_at_7[0x4];
	u8         log_max_num_of_nisp_spi[0x5];
	u8         reserved_at_10[0x10];

	u8         reserved_at_20[0x7e0];
};

struct mlx5_ifc_nvmeotcp_cap_bits {
	u8         zerocopy[0x1];
	u8         crc_rx[0x1];
	u8         crc_tx[0x1];
	u8         reserved_at_3[0x15];
	u8         version[0x8];

	u8         reserved_at_20[0x13];
	u8         log_max_nvmeotcp_tag_buffer_table[0x5];
	u8         reserved_at_38[0x3];
	u8         log_max_nvmeotcp_tag_buffer_size[0x5];

	u8         reserved_at_40[0x7c0];
};

enum {
	MLX5_NVMF_CAP_NVMF_ERROR_FILTERS_REPORT_CC_CHECK_ORIGINATED_ERRORS               = 0x1,
	MLX5_NVMF_CAP_NVMF_ERROR_FILTERS_REPORT_DISK_ORIGINATED_ERRORS_WITH_MORE_BIT     = 0x2,
	MLX5_NVMF_CAP_NVMF_ERROR_FILTERS_REPORT_DISK_ORIGINATED_ERRORS_WITHOUT_MORE_BIT  = 0x4,
	MLX5_NVMF_CAP_NVMF_ERROR_FILTERS_REPORT_GOOD_DISK_COMPLETIONS                    = 0x8,
};

struct mlx5_ifc_nvmf_cap_bits {
	u8         release_xrq_error[0x1];
	u8         nvmf_stats_report_back[0x1];
	u8         reserved_at_2[0x1];
	u8         log_max_frontend_nsid[0x5];
	u8         current_q_depth[0x1];
	u8         last_req_latency[0x1];
	u8         reserved_at_a[0x1];
	u8         log_min_cmd_timeout[0x5];
	u8         report_error_origin[0x1];
	u8         reserved_at_11[0x1];
	u8         partial_write_cache[0x1];
	u8         log_max_cmd_timeout[0x5];
	u8         passthrough_sqe_rw_service[0x1];
	u8         reserved_at_19[0x2];
	u8         log_max_namespace_per_xrq[0x5];

	u8         reserved_at_20[0x6];
	u8         read_write_flush_offload_dc[0x1];
	u8         read_write_offload_dc[0x1];
	u8         read_offload_dc[0x1];
	u8         write_offload_dc[0x1];
	u8         reserved_at_2a[0x1];
	u8         log_max_staging_buffer_size[0x5];
	u8         reserved_at_30[0x3];
	u8         log_min_staging_buffer_size[0x5];
	u8         reserved_at_38[0x1];
	u8         cmd_unknown_namespace_cnt[0x1];
	u8         frontend_namespace_context[0x1];
	u8         read_write_flush_offload_rc[0x1];
	u8         read_write_offload_rc[0x1];
	u8         read_offload_rc[0x1];
	u8         write_offload_rc[0x1];
	u8         reserved_at_3f[0x1];

	u8         reserved_at_40[0x3];
	u8         log_max_io_size[0x5];
	u8         reserved_at_48[0x3];
	u8         log_max_backend_controller[0x5];
	u8         reserved_at_50[0x3];
	u8         log_max_backend_controller_per_xrq[0x5];
	u8         max_icdoff[0x8];

	u8         max_lba_metadata_size[0x10];
	u8         reserved_at_70[0x3];
	u8         log_max_queue_size[0x5];
	u8         reserved_at_78[0x3];
	u8         log_min_queue_size[0x5];

	u8         min_ioccsz[0x20];

	u8         max_ioccsz[0x20];

	u8         nvmf_error_filters[0x10];
	u8         reserved_at_d0[0xa];
	u8         lossless_error_report[0x1];
	u8         nvmf_error_reporting[0x1];
	u8         nvmf_error_message_size[0x4];

	u8         reserved_at_e0[0x14];
	u8         log_max_num_pacers_per_port[0x4];
	u8         reserved_at_f8[0x3];
	u8         log_max_metadata_size[0x5];

	u8         min_iorcsz[0x20];

	u8         max_iorcsz[0x20];

	u8         reserved_at_140[0x6c0];
};

struct mlx5_ifc_odp_cap_bits {
	u8         reserved_at_0[0x40];

	u8         sig[0x1];
	u8         reserved_at_41[0x1f];

	u8         reserved_at_60[0x20];

	struct mlx5_ifc_odp_per_transport_service_cap_bits rc_odp_caps;

	struct mlx5_ifc_odp_per_transport_service_cap_bits uc_odp_caps;

	struct mlx5_ifc_odp_per_transport_service_cap_bits ud_odp_caps;

	struct mlx5_ifc_odp_per_transport_service_cap_bits xrc_odp_caps;

	struct mlx5_ifc_odp_per_transport_service_cap_bits dc_odp_caps;

	u8         reserved_at_120[0x6e0];
};

enum {
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_IN_HEAD       = 0x2,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_IN_MAC        = 0x4,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_IN_IP         = 0x8,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_IN_GRE        = 0x10,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_IN_UDP        = 0x20,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_IN_MPLS       = 0x40,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_IN_TCP        = 0x80,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_IN_VXLAN_GPE  = 0x100,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_IN_GENEVE     = 0x200,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_IN_IPSEC_ESP  = 0x400,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_IN_IPV4       = 0x800,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_IN_IPV6       = 0x1000,
};

enum {
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_OUT_HEAD       = 0x2,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_OUT_MAC        = 0x4,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_OUT_IP         = 0x8,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_OUT_GRE        = 0x10,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_OUT_UDP        = 0x20,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_OUT_MPLS       = 0x40,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_OUT_TCP        = 0x80,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_OUT_VXLAN_GPE  = 0x100,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_OUT_GENEVE     = 0x200,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_OUT_IPSEC_ESP  = 0x400,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_OUT_IPV4       = 0x800,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_OUT_IPV6       = 0x1000,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_NODE_OUT_NISP       = 0x2000,
};

enum {
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_HEADER_LENGTH_MODE_FIXED_SIZE      = 0x1,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_HEADER_LENGTH_MODE_EXPLICIT_FIELD  = 0x2,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_HEADER_LENGTH_MODE_BITMASK_FIELD   = 0x4,
};

enum {
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_FLOW_MATCH_SAMPLE_OFFSET_MODE_FIXED_SIZE      = 0x1,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_FLOW_MATCH_SAMPLE_OFFSET_MODE_EXPLICIT_FIELD  = 0x2,
	MLX5_PARSE_GRAPH_NODE_CAP_PARSE_GRAPH_FLOW_MATCH_SAMPLE_OFFSET_MODE_BITMASK_FIELD   = 0x4,
};

struct mlx5_ifc_parse_graph_node_cap_bits {
	u8         parse_graph_node_in[0x20];

	u8         parse_graph_node_out[0x20];

	u8         parse_graph_header_length_mode[0x10];
	u8         parse_graph_flow_match_sample_offset_mode[0x10];

	u8         max_num_parse_graph_arc_in[0x8];
	u8         max_num_parse_graph_arc_out[0x8];
	u8         max_num_parse_graph_flow_match_sample[0x8];
	u8         reserved_at_78[0x3];
	u8         parse_graph_anchor[0x1];
	u8         ext_sample_id[0x1];
	u8         sample_tunnel_mode_inner2[0x1];
	u8         zero_size_flex_parser_supported[0x1];
	u8         parse_graph_flow_match_sample_id_in_out[0x1];

	u8         max_parse_graph_header_length_base_value[0x10];
	u8         reserved_at_90[0x3];
	u8         parse_graph_max_next_header_field_size[0x5];
	u8         max_parse_graph_flow_match_sample_field_base_offset_value[0x8];

	u8         parse_graph_max_next_header_field_offset[0x10];
	u8         reserved_at_b0[0x8];
	u8         parse_graph_header_length_field_mask_width[0x8];

	u8         reserved_at_c0[0x740];
};

enum {
	MLX5_PER_PROTOCOL_NETWORKING_OFFLOAD_CAPS_LRO_MAX_MSG_SZ_MODE_START_FROM_TCP_HEADER  = 0x0,
	MLX5_PER_PROTOCOL_NETWORKING_OFFLOAD_CAPS_LRO_MAX_MSG_SZ_MODE_START_FROM_L2_HEADER   = 0x1,
};

enum {
	MLX5_PER_PROTOCOL_NETWORKING_OFFLOAD_CAPS_MULTI_PKT_SEND_WQE_NOT_SUPPORTED  = 0x0,
	MLX5_PER_PROTOCOL_NETWORKING_OFFLOAD_CAPS_MULTI_PKT_SEND_WQE_PRIVILEGED     = 0x1,
};

enum {
	MLX5_PER_PROTOCOL_NETWORKING_OFFLOAD_CAPS_WQE_INLINE_MODE_L2             = 0x0,
	MLX5_PER_PROTOCOL_NETWORKING_OFFLOAD_CAPS_WQE_INLINE_MODE_VPORT_CONTEXT  = 0x1,
	MLX5_PER_PROTOCOL_NETWORKING_OFFLOAD_CAPS_WQE_INLINE_MODE_NOT_REQUIRED   = 0x2,
};

struct mlx5_ifc_per_protocol_networking_offload_caps_bits {
	u8         csum_cap[0x1];
	u8         vlan_strip[0x1];
	u8         lro_cap[0x1];
	u8         lro_psh_flag[0x1];
	u8         lro_time_stamp[0x1];
	u8         lro_max_msg_sz_mode[0x2];
	u8         wqe_vlan_insert[0x1];
	u8         self_lb_en_modifiable[0x1];
	u8         self_lb_mc[0x1];
	u8         self_lb_uc[0x1];
	u8         max_lso_cap[0x5];
	u8         multi_pkt_send_wqe[0x2];
	u8         wqe_inline_mode[0x2];
	u8         rss_ind_tbl_cap[0x4];
	u8         reg_umr_sq[0x1];
	u8         scatter_fcs[0x1];
	u8         enhanced_multi_pkt_send_wqe[0x1];
	u8         tunnel_lso_const_out_ip_id[0x1];
	u8         tunnel_lro_gre[0x1];
	u8         tunnel_lro_vxlan[0x1];
	u8         tunnel_stateless_gre[0x1];
	u8         tunnel_stateless_vxlan[0x1];

	u8         swp[0x1];
	u8         swp_csum[0x1];
	u8         swp_lso[0x1];
	u8         cqe_checksum_full[0x1];
	u8         tunnel_stateless_geneve_tx[0x1];
	u8         tunnel_stateless_mpls_over_udp[0x1];
	u8         tunnel_stateless_mpls_over_gre[0x1];
	u8         tunnel_stateless_vxlan_gpe[0x1];
	u8         tunnel_stateless_ipv4_over_vxlan[0x1];
	u8         tunnel_stateless_ip_over_ip[0x1];
	u8         insert_trailer[0x1];
	u8         tunnel_stateless_gtp[0x1];
	u8         tunnel_stateless_ip_over_ip_rx[0x1];
	u8         tunnel_stateless_ip_over_ip_tx[0x1];
	u8         reserved_at_2e[0x2];
	u8         max_vxlan_udp_ports[0x8];
	u8         reserved_at_38[0x6];
	u8         max_geneve_opt_len[0x1];
	u8         tunnel_stateless_geneve_rx[0x1];

	u8         reserved_at_40[0x10];
	u8         lro_min_mss_size[0x10];

	u8         reserved_at_60[0x120];

	u8         packet_merge_timer_supported_periods[4][0x20];

	u8         reserved_at_200[0x600];
};

struct mlx5_ifc_port_selection_capabilities_bits {
	u8         reserved_at_0[0x10];
	u8         port_select_flow_table[0x1];
	u8         port_select_eswitch[0x1];
	u8         port_select_flow_table_bypass[0x1];
	u8         reserved_at_13[0xd];

	u8         reserved_at_20[0x1e0];

	struct mlx5_ifc_flow_table_prop_layout_bits flow_table_properties_port_selection;

	struct mlx5_ifc_flow_table_fields_supported_2_bits ft_field_support_2_port_selection;

	struct mlx5_ifc_flow_table_fields_supported_2_bits ft_field_bitmask_support_2_port_selection;

	u8         reserved_at_500[0x300];

	struct mlx5_ifc_header_modify_cap_properties_bits header_modify_port_selection;

	u8         reserved_at_c00[0x7400];
};

enum {
	MLX5_QOS_CAPS_FLOW_METER_REG_ID_REG_C_0  = 0x1,
	MLX5_QOS_CAPS_FLOW_METER_REG_ID_REG_C_1  = 0x2,
	MLX5_QOS_CAPS_FLOW_METER_REG_ID_REG_C_2  = 0x4,
	MLX5_QOS_CAPS_FLOW_METER_REG_ID_REG_C_3  = 0x8,
	MLX5_QOS_CAPS_FLOW_METER_REG_ID_REG_C_4  = 0x10,
	MLX5_QOS_CAPS_FLOW_METER_REG_ID_REG_C_5  = 0x20,
	MLX5_QOS_CAPS_FLOW_METER_REG_ID_REG_C_6  = 0x40,
	MLX5_QOS_CAPS_FLOW_METER_REG_ID_REG_C_7  = 0x80,
};

struct mlx5_ifc_qos_caps_bits {
	u8         packet_pacing[0x1];
	u8         esw_scheduling[0x1];
	u8         esw_bw_share[0x1];
	u8         esw_rate_limit[0x1];
	u8         hll[0x1];
	u8         packet_pacing_burst_bound[0x1];
	u8         packet_pacing_typical_size[0x1];
	u8         flow_meter_old[0x1];
	u8         nic_sq_scheduling[0x1];
	u8         nic_bw_share[0x1];
	u8         nic_rate_limit[0x1];
	u8         packet_pacing_uid[0x1];
	u8         log_esw_max_sched_depth[0x4];
	u8         log_max_flow_meter[0x8];
	u8         flow_meter_reg_id[0x8];

	u8         wqe_rate_pp[0x1];
	u8         nic_qp_scheduling[0x1];
	u8         reserved_at_22[0x2];
	u8         log_nic_max_sched_depth[0x4];
	u8         flow_meter[0x1];
	u8         reserved_at_29[0x1];
	u8         qos_remap_pp[0x1];
	u8         log_max_qos_nic_queue_group[0x5];
	u8         reserved_at_30[0x8];
	u8         max_flow_meter_bs_exponent[0x8];

	u8         packet_pacing_max_rate[0x20];

	u8         packet_pacing_min_rate[0x20];

	u8         reserved_at_80[0xb];
	u8         log_esw_max_rate_limit[0x5];
	u8         packet_pacing_rate_table_size[0x10];

	u8         esw_element_type[0x10];
	u8         esw_tsar_type[0x10];

	u8         max_qos_para_vport[0x10];
	u8         max_qos_para_vport_old[0x10];

	u8         max_tsar_bw_share[0x20];

	u8         nic_element_type[0x10];
	u8         nic_tsar_type[0x10];

	u8         reserved_at_120[0x3];
	u8         log_meter_aso_granularity[0x5];
	u8         reserved_at_128[0x3];
	u8         log_meter_aso_max_alloc[0x5];
	u8         reserved_at_130[0x3];
	u8         log_max_num_meter_aso[0x5];
	u8         reserved_at_138[0x8];

	u8         reserved_at_140[0x3];
	u8         log_max_qos_nic_scheduling_element[0x5];
	u8         reserved_at_148[0x3];
	u8         log_max_qos_esw_scheduling_element[0x5];
	u8         reserved_at_150[0x10];

	u8         reserved_at_160[0x6a0];
};

enum {
	MLX5_ROCE_CAP_QP_TS_FORMAT_FREE_RUNNING_TS                = 0x0,
	MLX5_ROCE_CAP_QP_TS_FORMAT_REAL_TIME_TS                   = 0x1,
	MLX5_ROCE_CAP_QP_TS_FORMAT_FREE_RUNNING_AND_REAL_TIME_TS  = 0x2,
};

enum {
	MLX5_ROCE_CAP_L3_TYPE_GRH   = 0x1,
	MLX5_ROCE_CAP_L3_TYPE_IPV4  = 0x2,
	MLX5_ROCE_CAP_L3_TYPE_IPV6  = 0x4,
};

enum {
	MLX5_ROCE_CAP_ROCE_VERSION_VERSION_1_0  = 0x1,
	MLX5_ROCE_CAP_ROCE_VERSION_VERSION_1_5  = 0x2,
	MLX5_ROCE_CAP_ROCE_VERSION_VERSION_2_0  = 0x4,
};

struct mlx5_ifc_roce_cap_bits {
	u8         roce_apm[0x1];
	u8         rts2rts_primary_eth_prio[0x1];
	u8         roce_rx_allow_untagged[0x1];
	u8         rts2rts_src_addr_index_for_vlan_valid_vlan_id[0x1];
	u8         sw_r_roce_src_udp_port[0x1];
	u8         fl_rc_qp_when_roce_disabled[0x1];
	u8         fl_rc_qp_when_roce_enabled[0x1];
	u8         roce_cc_general[0x1];
	u8         qp_ooo_transmit_default[0x1];
	u8         reserved_at_9[0x15];
	u8         qp_ts_format[0x2];

	u8         reserved_at_20[0x60];

	u8         reserved_at_80[0xc];
	u8         l3_type[0x4];
	u8         reserved_at_90[0x8];
	u8         roce_version[0x8];

	u8         reserved_at_a0[0x10];
	u8         r_roce_dest_udp_port[0x10];

	u8         r_roce_max_src_udp_port[0x10];
	u8         r_roce_min_src_udp_port[0x10];

	u8         reserved_at_e0[0x10];
	u8         roce_address_table_size[0x10];

	u8         reserved_at_100[0x700];
};

enum {
	MLX5_RX_HASH_FIELD_SELECT_SELECTED_FIELDS_SRC_IP     = 0x0,
	MLX5_RX_HASH_FIELD_SELECT_SELECTED_FIELDS_DST_IP     = 0x1,
	MLX5_RX_HASH_FIELD_SELECT_SELECTED_FIELDS_L4_SPORT   = 0x2,
	MLX5_RX_HASH_FIELD_SELECT_SELECTED_FIELDS_L4_DPORT   = 0x3,
	MLX5_RX_HASH_FIELD_SELECT_SELECTED_FIELDS_IPSEC_SPI  = 0x4,
};

struct mlx5_ifc_rx_hash_field_select_bits {
	u8         l3_prot_type[0x1];
	u8         l4_prot_type[0x1];
	u8         selected_fields[0x1e];
};

struct mlx5_ifc_shampo_cap_bits {
	u8         reserved_at_0[0x3];
	u8         shampo_log_max_reservation_size[0x5];
	u8         reserved_at_8[0x3];
	u8         shampo_log_min_reservation_size[0x5];
	u8         shampo_min_mss_size[0x10];

	u8         reserved_at_20[0x3];
	u8         shampo_log_max_headers_entry_size[0x5];
	u8         reserved_at_28[0x18];

	u8         reserved_at_40[0x7c0];
};

struct mlx5_ifc_tls_cap_bits {
	u8         tls_1_2_aes_gcm_128[0x1];
	u8         tls_1_3_aes_gcm_128[0x1];
	u8         tls_1_2_aes_gcm_256[0x1];
	u8         tls_1_3_aes_gcm_256[0x1];
	u8         reserved_at_4[0x1c];

	u8         reserved_at_20[0x7e0];
};

enum {
	MLX5_UPT_DEVICE_EMULATION_CAPS_SUPPORTED_FEATURES_VMK_NETVF_F_RXCSUM     = 0x1,
	MLX5_UPT_DEVICE_EMULATION_CAPS_SUPPORTED_FEATURES_VMK_NETVF_F_RSS        = 0x2,
	MLX5_UPT_DEVICE_EMULATION_CAPS_SUPPORTED_FEATURES_VMK_NETVF_F_RXVLAN     = 0x4,
	MLX5_UPT_DEVICE_EMULATION_CAPS_SUPPORTED_FEATURES_VMK_NETVF_F_LRO        = 0x8,
	MLX5_UPT_DEVICE_EMULATION_CAPS_SUPPORTED_FEATURES_VMK_NETVF_F_INNEROFLD  = 0x10,
};

enum {
	MLX5_UPT_DEVICE_EMULATION_CAPS_SUPPORTED_CAPABILITIES_GENEVE_CHECKSUM_OFFLOAD        = 0x4,
	MLX5_UPT_DEVICE_EMULATION_CAPS_SUPPORTED_CAPABILITIES_GENEVE_TSO                     = 0x8,
	MLX5_UPT_DEVICE_EMULATION_CAPS_SUPPORTED_CAPABILITIES_VXLAN_CHECKSUM_OFFLOAD         = 0x10,
	MLX5_UPT_DEVICE_EMULATION_CAPS_SUPPORTED_CAPABILITIES_VXLAN_TSO                      = 0x20,
	MLX5_UPT_DEVICE_EMULATION_CAPS_SUPPORTED_CAPABILITIES_GENEVE_OUTER_CHECKSUM_OFFLOAD  = 0x40,
	MLX5_UPT_DEVICE_EMULATION_CAPS_SUPPORTED_CAPABILITIES_VXLAN_OUTER_CHECKSUM_OFFLOAD   = 0x80,
};

struct mlx5_ifc_upt_device_emulation_caps_bits {
	u8         queue_period_upon_cqe[0x1];
	u8         queue_period_upon_event[0x1];
	u8         inner_rss_report[0x1];
	u8         reserved_at_3[0x1];
	u8         passthru_reg_bar_id[0x4];
	u8         reserved_at_8[0x10];
	u8         passthru_reg_size[0x8];

	u8         supported_features[0x10];
	u8         max_emulated_devices[0x10];

	u8         passthru_reg_bar_offset[0x40];

	u8         supported_capabilities[0x20];

	u8         reserved_at_a0[0x18];
	u8         max_num_interrupts[0x8];

	u8         max_num_upt_sqs[0x10];
	u8         max_num_upt_rqs[0x10];

	u8         max_sq_size[0x10];
	u8         max_rq_size[0x10];

	u8         reserved_at_100[0x700];
};

struct mlx5_ifc_vector_calc_caps_bits {
	u8         calc_matrix[0x1];
	u8         calc_matrix_type_8bits[0x1];
	u8         reserved_at_2[0x1e];

	u8         reserved_at_20[0x8];
	u8         max_vector_count[0x8];
	u8         reserved_at_30[0xd];
	u8         max_chunk_size[0x3];

	u8         reserved_at_40[0x10];
	u8         calc_operation_0[0x10];

	u8         reserved_at_60[0x10];
	u8         calc_operation_1[0x10];

	u8         reserved_at_80[0x10];
	u8         calc_operation_2[0x10];

	u8         reserved_at_a0[0x10];
	u8         calc_operation_3[0x10];

	u8         reserved_at_c0[0x740];
};

enum {
	MLX5_VIRTIO_EMULATION_CAP_EVENT_MODE_NO_MSIX_MODE  = 0x1,
	MLX5_VIRTIO_EMULATION_CAP_EVENT_MODE_QP_MODE       = 0x2,
	MLX5_VIRTIO_EMULATION_CAP_EVENT_MODE_MSIX_MODE     = 0x4,
};

enum {
	MLX5_VIRTIO_EMULATION_CAP_VIRTIO_QUEUE_TYPE_SPLIT   = 0x1,
	MLX5_VIRTIO_EMULATION_CAP_VIRTIO_QUEUE_TYPE_PACKED  = 0x2,
};

struct mlx5_ifc_virtio_emulation_cap_bits {
	u8         desc_tunnel_offload_type[0x1];
	u8         eth_frame_offload_type[0x1];
	u8         virtio_version_1_0[0x1];
	u8         device_feautres_bits_mask[0xd];
	u8         event_mode[0x8];
	u8         virtio_queue_type[0x8];

	u8         max_tunnel_desc[0x10];
	u8         dirty_byte_map[0x1];
	u8         queue_period_upon_cqe[0x1];
	u8         queue_period_upon_event[0x1];
	u8         log_doorbell_stride[0x5];
	u8         vnet_modify_ext[0x1];
	u8         virtio_net_q_addr_modify[0x1];
	u8         virtio_q_index_modify[0x1];
	u8         log_doorbell_bar_size[0x5];

	u8         doorbell_bar_offset[0x40];

	u8         must_not_be_used[0x8];
	u8         max_num_virtio_queues[0x18];

	u8         max_emulated_devices[0x10];
	u8         max_virtq_size[0x10];

	u8         reserved_at_c0[0x1d];
	u8         emulated_dev_db_cq_map[0x1];
	u8         emulated_dev_eq[0x1];
	u8         virtio_q_cfg_v2[0x1];

	u8         reserved_at_e0[0x20];

	u8         umem_1_buffer_param_a[0x20];

	u8         umem_1_buffer_param_b[0x20];

	u8         umem_2_buffer_param_a[0x20];

	u8         umem_2_buffer_param_b[0x20];

	u8         umem_3_buffer_param_a[0x20];

	u8         umem_3_buffer_param_b[0x20];

	u8         max_num_vf_dynamic_msix[0x10];
	u8         min_num_vf_dynamic_msix[0x10];

	u8         reserved_at_1e0[0x620];
};

enum {
	MLX5_WQ_TYPE_LINKED_LIST                 = 0x0,
	MLX5_WQ_TYPE_CYCLIC                      = 0x1,
	MLX5_WQ_TYPE_STRQ_LINKED_LIST            = 0x2,
	MLX5_WQ_TYPE_STRQ_CYCLIC                 = 0x3,
};

enum {
	MLX5_WQ_END_PAD_MODE_NONE               = 0x0,
	MLX5_WQ_END_PAD_MODE_ALIGN              = 0x1,
};

struct mlx5_ifc_wq_bits {
	u8         wq_type[0x4];
	u8         wq_signature[0x1];
	u8         end_padding_mode[0x2];
	u8         cd_slave[0x1];
	u8         reserved_at_8[0x18];

	u8         hds_skip_first_sge[0x1];
	u8         log2_hds_buf_size[0x3];
	u8         reserved_at_24[0x7];
	u8         page_offset[0x5];
	u8         lwm[0x10];

	u8         reserved_at_40[0x8];
	u8         pd[0x18];

	u8         reserved_at_60[0x8];
	u8         uar_page[0x18];

	u8         dbr_addr[0x40];

	u8         hw_counter[0x20];

	u8         sw_counter[0x20];

	u8         reserved_at_100[0xc];
	u8         log_wq_stride[0x4];
	u8         reserved_at_110[0x3];
	u8         log_wq_pg_sz[0x5];
	u8         reserved_at_118[0x3];
	u8         log_wq_sz[0x5];

	u8         dbr_umem_valid[0x1];
	u8         wq_umem_valid[0x1];
	u8         reserved_at_122[0x1];
	u8         log_hairpin_num_packets[0x5];
	u8         reserved_at_128[0x3];
	u8         log_hairpin_data_sz[0x5];
	u8         reserved_at_130[0x4];
	u8         single_wqe_log_num_of_strides[0x4];
	u8         two_byte_shift_en[0x1];
	u8         reserved_at_139[0x4];
	u8         single_stride_log_num_of_bytes[0x3];

	u8         dbr_umem_id[0x20];

	u8         wq_umem_id[0x20];

	u8         wq_umem_offset[0x40];

	u8         shampo_headers_buffer_mkey[0x20];

	u8         shampo[0x1];
	u8         reserved_at_1e1[0x4];
	u8         shampo_log_reservation_size[0x3];
	u8         reserved_at_1e8[0x5];
	u8         shampo_log_max_num_of_packets_per_reservation[0x3];
	u8         reserved_at_1f0[0x6];
	u8         shampo_log_headers_buffer_entry_size[0x2];
	u8         reserved_at_1f8[0x4];
	u8         shampo_log_num_entries_in_headers_buffer[0x4];

	u8         reserved_at_200[0xb];
	u8         log_hairpin_wqe_num[0x5];
	u8         enh_strwq_profile_id[0x10];

	u8         reserved_at_220[0x3e0];

	struct mlx5_ifc_cmd_pas_bits pas[0];
};

enum {
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_RTC_REPARSE_MODE_REPARSE_NEVER   = 0x1,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_RTC_REPARSE_MODE_REPARSE_ALWAYS  = 0x2,
};

enum {
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_RTC_INDEX_MODE_BY_HASH    = 0x1,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_RTC_INDEX_MODE_BY_OFFSET  = 0x2,
};

enum {
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_STE_FORMAT_MATCH_8DW        = 0x10,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_STE_FORMAT_MATCH_11DW       = 0x20,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_STE_FORMAT_MATCH_RANGE      = 0x80,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_STE_FORMAT_MATCH_4DW_RANGE  = 0x100,
};

enum {
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_STC_ACTION_TYPE_63_0_NOP                 = 0x1,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_STC_ACTION_TYPE_63_0_FIELD_COPY          = 0x20,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_STC_ACTION_TYPE_63_0_FIELD_SET           = 0x40,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_STC_ACTION_TYPE_63_0_FIELD_ADD           = 0x80,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_STC_ACTION_TYPE_63_0_REMOVE_WORDS        = 0x100,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_STC_ACTION_TYPE_63_0_HEADER_REMOVE       = 0x200,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_STC_ACTION_TYPE_63_0_HEADER_INSERT       = 0x800,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_STC_ACTION_TYPE_63_0_FLOW_TAG            = 0x1000,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_STC_ACTION_TYPE_63_0_HEADER_MODIFY_LIST  = 0x4000,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_STC_ACTION_TYPE_63_0_EXECUTE_ASO         = 0x40000,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_STC_ACTION_TYPE_63_0_COUNT               = 0x100000,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_STC_ACTION_TYPE_63_0_JUMP_TO_STE_TABLE   = 0x1,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_STC_ACTION_TYPE_63_0_JUMP_TO_TIR         = 0x2,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_STC_ACTION_TYPE_63_0_JUMP_TO_FLOW_TABLE  = 0x4,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_STC_ACTION_TYPE_63_0_JUMP_TO_DROP        = 0x8,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_STC_ACTION_TYPE_63_0_JUMP_TO_ALLOW       = 0x10,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_STC_ACTION_TYPE_63_0_JUMP_TO_VPORT       = 0x20,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_STC_ACTION_TYPE_63_0_JUMP_TO_UPLINK      = 0x40,
};

enum {
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_HEADER_INSERT_TYPE_ENCAPSULATE      = 0x1,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_HEADER_INSERT_TYPE_INSERT_L2        = 0x2,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_HEADER_INSERT_TYPE_INSERT_L2_TAG    = 0x4,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_HEADER_INSERT_TYPE_INSERT_IPV6_EXT  = 0x8,
};

enum {
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_HEADER_REMOVE_TYPE_L2_TUNNEL_DECAP      = 0x1,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_HEADER_REMOVE_TYPE_L3_TUNNEL_DECAP      = 0x2,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_HEADER_REMOVE_TYPE_L3_ENCAP             = 0x4,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_HEADER_REMOVE_TYPE_REMOVE_L2            = 0x8,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_HEADER_REMOVE_TYPE_POP_VLAN             = 0x10,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_HEADER_REMOVE_TYPE_IN_L2_TUNNEL_DECAP   = 0x20,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_HEADER_REMOVE_TYPE_IN_L3_TUNNEL_DECAP   = 0x40,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_HEADER_REMOVE_TYPE_REMOVE_AFTER_SAMPLE  = 0x80,
};

enum {
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_ACCESS_INDEX_MODE_BY_HASH  = 0x1,
	MLX5_WQE_BASED_FLOW_TABLE_CAPABILITIES_ACCESS_INDEX_MODE_LINEAR   = 0x2,
};

struct mlx5_ifc_wqe_based_flow_table_capabilities_bits {
	u8         reserved_at_0[0x3];
	u8         log_max_num_ste[0x5];
	u8         reserved_at_8[0x3];
	u8         log_max_num_stc[0x5];
	u8         reserved_at_10[0x3];
	u8         log_max_num_rtc[0x5];
	u8         reserved_at_18[0x3];
	u8         log_max_num_header_modify_pattern[0x5];

	u8         rtc_hash_split_table[0x1];
	u8         rtc_linear_lookup_table[0x1];
	u8         format_select_dw_unlimited[0x1];
	u8         stc_alloc_log_granularity[0x5];
	u8         reserved_at_28[0x3];
	u8         stc_alloc_log_max[0x5];
	u8         reserved_at_30[0x3];
	u8         ste_alloc_log_granularity[0x5];
	u8         reserved_at_38[0x3];
	u8         ste_alloc_log_max[0x5];

	u8         reserved_at_40[0x3];
	u8         jump_to_ste_log_max_hash_size[0x5];
	u8         reserved_at_48[0x3];
	u8         rtc_reparse_mode[0x5];
	u8         reserved_at_50[0x3];
	u8         rtc_index_mode[0x5];
	u8         reserved_at_58[0x3];
	u8         rtc_log_depth_max[0x5];

	u8         reserved_at_60[0x3];
	u8         rtc_index_mode_gen_wqe[0x5];
	u8         max_header_modify_pattern_length[0x8];
	u8         ste_format[0x10];

	u8         stc_action_type_63_0[0x40];

	u8         stc_action_type_127_64[0x40];

	u8         header_insert_type[0x10];
	u8         header_remove_type[0x10];

	u8         trivial_match_definer[0x20];

	u8         reserved_at_140[0x3];
	u8         rtc_max_num_match_ste[0x5];
	u8         reserved_at_148[0x3];
	u8         rtc_max_num_match_ste_gen_wqe[0x5];
	u8         reserved_at_150[0x3];
	u8         rtc_max_num_hash_definer[0x5];
	u8         reserved_at_158[0x3];
	u8         rtc_max_num_hash_definer_gen_wqe[0x5];

	u8         reserved_at_160[0x18];
	u8         access_index_mode[0x8];

	u8         stc_execute_aso_type[0x10];
	u8         ste_format_gen_wqe[0x10];

	u8         linear_match_definer_reg_c3[0x20];

	u8         reserved_at_1c0[0x640];
};

struct mlx5_ifc_array1024_auto_bits {
	u8         array1024_auto[32][0x20];
};

struct mlx5_ifc_channel_connection_client_bits {
	u8         connection_id[0x8];
	u8         qpn[0x18];
};

struct mlx5_ifc_cmd_e_bsf_pointer_bits {
	u8         reserved_at_0[0x19];
	u8         bsf_index_h[0x7];

	u8         bsf_index_l[0x1c];
	u8         reserved_at_3c[0x4];
};

struct mlx5_ifc_cmd_e_mtt_pointer_bits {
	u8         reserved_at_0[0x19];
	u8         mtt_index_h[0x7];

	u8         mtt_index_l[0x1d];
	u8         reserved_at_3d[0x3];
};

enum {
	MLX5_CQC_STATUS_OK             = 0x0,
	MLX5_CQC_STATUS_CQ_OVERFLOW    = 0x9,
	MLX5_CQC_STATUS_CQ_WRITE_FAIL  = 0xa,
	MLX5_CQC_STATUS_CQ_INVALID     = 0xf,
};

enum {
	MLX5_CQC_CQE_SZ_BYTES_64             = 0x0,
	MLX5_CQC_CQE_SZ_BYTES_128_IF_NEEDED  = 0x1,
	MLX5_CQC_CQE_SZ_BYTES_128_ALWAYS     = 0x2,
};

enum {
	MLX5_CQ_PERIOD_MODE_START_FROM_EQE  = 0x0,
	MLX5_CQ_PERIOD_MODE_START_FROM_CQE  = 0x1,
};

enum {
	MLX5_CQ_STATE_SOLICITED_ARMED                     = 0x6,
	MLX5_CQ_STATE_ARMED                               = 0x9,
	MLX5_CQ_STATE_FIRED                               = 0xa,
};

enum {
	MLX5_CQC_EXT_ELEMENT_TYPE_DPA_THREAD       = 0x0,
	MLX5_CQC_EXT_ELEMENT_TYPE_DPA_EQ           = 0x1,
	MLX5_CQC_EXT_ELEMENT_TYPE_EMULATED_DEV_EQ  = 0x2,
};

enum {
	MLX5_CQC_CQE_COMPRESSION_LAYOUT_BASIC_CQE_COMPRESSION     = 0x0,
	MLX5_CQC_CQE_COMPRESSION_LAYOUT_ENHANCED_CQE_COMPRESSION  = 0x1,
};

enum {
	MLX5_CQC_CQ_TIME_STAMP_FORMAT_INTERNAL_TIMER  = 0x0,
	MLX5_CQC_CQ_TIME_STAMP_FORMAT_REAL_TIME       = 0x1,
};

struct mlx5_ifc_cqc_bits {
	u8         status[0x4];
	u8         as_notify[0x1];
	u8         initiator_src_dct[0x1];
	u8         dbr_umem_valid[0x1];
	u8         ext_element[0x1];
	u8         cqe_sz[0x3];
	u8         cc[0x1];
	u8         reserved_at_c[0x1];
	u8         scqe_break_moderation_en[0x1];
	u8         oi[0x1];
	u8         cq_period_mode[0x2];
	u8         cqe_compression_en[0x1];
	u8         mini_cqe_res_format_1_0[0x2];
	u8         st[0x4];
	u8         always_armed_cq[0x1];
	u8         ext_element_type[0x3];
	u8         reserved_at_1c[0x2];
	u8         cqe_compression_layout[0x2];

	u8         dbr_umem_id[0x20];

	u8         reserved_at_40[0x14];
	u8         page_offset[0x6];
	u8         reserved_at_5a[0x2];
	u8         mini_cqe_res_format_3_2[0x2];
	u8         cq_time_stamp_format[0x2];

	u8         reserved_at_60[0x3];
	u8         log_cq_size[0x5];
	u8         uar_page[0x18];

	u8         reserved_at_80[0x4];
	u8         cq_period[0xc];
	u8         cq_max_count[0x10];

	u8         c_eqn_or_ext_element[0x20];

	u8         reserved_at_c0[0x3];
	u8         log_page_size[0x5];
	u8         reserved_at_c8[0x18];

	u8         reserved_at_e0[0x20];

	u8         reserved_at_100[0x8];
	u8         last_notified_index[0x18];

	u8         reserved_at_120[0x8];
	u8         last_solicit_index[0x18];

	u8         reserved_at_140[0x8];
	u8         consumer_counter[0x18];

	u8         reserved_at_160[0x8];
	u8         producer_counter[0x18];

	struct mlx5_ifc_as_notify_params_bits as_notify_params;

	u8         dbr_addr[0x40];
};

struct mlx5_ifc_emulated_function_info_bits {
	u8         pci_bdf[0x10];
	u8         vhca_id[0x10];

	u8         hotplug_function[0x1];
	u8         max_num_vfs_valid[0x1];
	u8         vf_exist[0x1];
	u8         reserved_at_23[0xd];
	u8         max_num_vfs[0x10];
};

struct mlx5_ifc_enum_entry_bits {
	u8         reserved_at_0[0x20];
};

struct mlx5_ifc_general_obj_in_cmd_hdr_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         vhca_tunnel_id[0x10];
	u8         obj_type[0x10];

	u8         obj_id[0x20];

	u8         op_param[0x20];
};

struct mlx5_ifc_general_obj_out_cmd_hdr_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         obj_id[0x20];

	u8         reserved_at_60[0x20];
};

enum {
	MLX5_HCA_VPORT_CONTEXT_FIELD_SELECT_PORT_GUID            = 0x1,
	MLX5_HCA_VPORT_CONTEXT_FIELD_SELECT_NODE_GUID            = 0x2,
	MLX5_HCA_VPORT_CONTEXT_FIELD_SELECT_VPORT_STATE_POLICY   = 0x4,
	MLX5_HCA_VPORT_CONTEXT_FIELD_SELECT_MIN_WQE_INLINE_MODE  = 0x8,
	MLX5_HCA_VPORT_CONTEXT_FIELD_SELECT_GRH_REQUIRED         = 0x10,
	MLX5_HCA_VPORT_CONTEXT_FIELD_SELECT_SYSTEM_IMAGE_GUID    = 0x20,
};

enum {
	MLX5_HCA_VPORT_CONTEXT_PORT_PHYSICAL_STATE_SLEEP                      = 0x1,
	MLX5_HCA_VPORT_CONTEXT_PORT_PHYSICAL_STATE_POLLING                    = 0x2,
	MLX5_HCA_VPORT_CONTEXT_PORT_PHYSICAL_STATE_DISABLED                   = 0x3,
	MLX5_HCA_VPORT_CONTEXT_PORT_PHYSICAL_STATE_PORTCONFIGURATIONTRAINING  = 0x4,
	MLX5_HCA_VPORT_CONTEXT_PORT_PHYSICAL_STATE_LINKUP                     = 0x5,
	MLX5_HCA_VPORT_CONTEXT_PORT_PHYSICAL_STATE_LINKERRORRECOVERY          = 0x6,
	MLX5_HCA_VPORT_CONTEXT_PORT_PHYSICAL_STATE_PHYTEST                    = 0x7,
};

enum {
	MLX5_HCA_VPORT_CONTEXT_VPORT_STATE_POLICY_DOWN    = 0x0,
	MLX5_HCA_VPORT_CONTEXT_VPORT_STATE_POLICY_UP      = 0x1,
	MLX5_HCA_VPORT_CONTEXT_VPORT_STATE_POLICY_FOLLOW  = 0x2,
};

enum {
	MLX5_HCA_VPORT_CONTEXT_PORT_STATE_DOWN    = 0x1,
	MLX5_HCA_VPORT_CONTEXT_PORT_STATE_INIT    = 0x2,
	MLX5_HCA_VPORT_CONTEXT_PORT_STATE_ARM     = 0x3,
	MLX5_HCA_VPORT_CONTEXT_PORT_STATE_ACTIVE  = 0x4,
};

enum {
	MLX5_HCA_VPORT_CONTEXT_VPORT_STATE_DOWN    = 0x1,
	MLX5_HCA_VPORT_CONTEXT_VPORT_STATE_INIT    = 0x2,
	MLX5_HCA_VPORT_CONTEXT_VPORT_STATE_ARM     = 0x3,
	MLX5_HCA_VPORT_CONTEXT_VPORT_STATE_ACTIVE  = 0x4,
};

struct mlx5_ifc_hca_vport_context_bits {
	u8         field_select[0x20];

	u8         field_select_2[0x20];

	u8         reserved_at_40[0xc0];

	u8         sm_virt_aware[0x1];
	u8         has_smi[0x1];
	u8         has_raw[0x1];
	u8         grh_required[0x1];
	u8         multi_sl_qp[0x1];
	u8         min_wqe_inline_mode[0x3];
	u8         reserved_at_108[0x8];
	u8         port_physical_state[0x4];
	u8         vport_state_policy[0x4];
	u8         port_state[0x4];
	u8         vport_state[0x4];

	u8         reserved_at_120[0x20];

	u8         system_image_guid[0x40];

	u8         port_guid[0x40];

	u8         node_guid[0x40];

	u8         cap_mask1[0x20];

	u8         cap_mask1_field_select[0x20];

	u8         cap_mask2[0x20];

	u8         cap_mask2_field_select[0x20];

	u8         reserved_at_280[0x10];
	u8         ooo_sl_mask[0x10];

	u8         reserved_at_2a0[0x60];

	u8         lid[0x10];
	u8         reserved_at_310[0x4];
	u8         init_type_reply[0x4];
	u8         lmc[0x3];
	u8         subnet_timeout[0x5];

	u8         sm_lid[0x10];
	u8         sm_sl[0x4];
	u8         reserved_at_334[0xc];

	u8         qkey_violation_counter[0x10];
	u8         pkey_violation_counter[0x10];

	u8         reserved_at_360[0xca0];
};

struct mlx5_ifc_host_params_bits {
	u8         host_number[0x8];
	u8         reserved_at_8[0x4];
	u8         host_pci_vf_info_valid[0x1];
	u8         host_pf_not_exist[0x1];
	u8         host_pf_vhca_id_valid[0x1];
	u8         host_pf_disabled[0x1];
	u8         host_num_vfs[0x10];

	u8         host_total_vfs[0x10];
	u8         host_pci_bus[0x10];

	u8         host_pf_vhca_id[0x10];
	u8         host_pci_device[0x10];

	u8         reserved_at_60[0x10];
	u8         host_pci_function[0x10];

	u8         host_pci_vf_stride[0x10];
	u8         host_pci_first_vf_offset[0x10];

	u8         reserved_at_a0[0x160];
};

enum {
	MLX5_HOTPLUG_DEVICE_OBJ_MODIFY_FIELD_SELECT_REGISTER_DATA  = 0x1,
};

enum {
	MLX5_HOTPLUG_DEVICE_TYPE_NVME                   = 0x0,
	MLX5_HOTPLUG_DEVICE_TYPE_VIRTIO_NET             = 0x1,
	MLX5_HOTPLUG_DEVICE_TYPE_VIRTIO_BLK             = 0x2,
	MLX5_HOTPLUG_DEVICE_TYPE_VIRTIO_FS              = 0x3,
	MLX5_HOTPLUG_DEVICE_OBJ_DEVICE_TYPE_UPT         = 0x5,
};

struct mlx5_ifc_hotplug_device_obj_bits {
	u8         modify_field_select[0x40];

	u8         reserved_at_40[0x17];
	u8         initial_registers_valid[0x1];
	u8         device_type[0x8];

	u8         total_vf[0x10];
	u8         vhca_id[0x10];

	struct mlx5_ifc_device_pci_params_bits device_pci_parameters;

	u8         reserved_at_100[0x100];

	union mlx5_ifc_hotplug_device_obj_emulation_initial_registers_auto_bits emulation_initial_registers;
};

enum {
	MLX5_IPSEC_OFFLOAD_MODIFY_FIELD_SELECT_ESN_OVERLAP  = 0x1,
	MLX5_IPSEC_OFFLOAD_MODIFY_FIELD_SELECT_ESN_MSB      = 0x2,
};

enum {
	MLX5_IPSEC_OFFLOAD_FULL_OFFLOAD_CRYPTO_OFFLOAD  = 0x0,
	MLX5_IPSEC_OFFLOAD_FULL_OFFLOAD_FULL_OFFLOAD    = 0x1,
};

enum {
	MLX5_IPSEC_OFFLOAD_ICV_LENGTH_LEN_16B  = 0x0,
	MLX5_IPSEC_OFFLOAD_ICV_LENGTH_LEN_12B  = 0x1,
	MLX5_IPSEC_OFFLOAD_ICV_LENGTH_LEN_8B   = 0x2,
};

enum {
	MLX5_IPSEC_OFFLOAD_ASO_RETURN_REG_REG_C_0_1  = 0x0,
	MLX5_IPSEC_OFFLOAD_ASO_RETURN_REG_REG_C_2_3  = 0x1,
	MLX5_IPSEC_OFFLOAD_ASO_RETURN_REG_REG_C_4_5  = 0x2,
	MLX5_IPSEC_OFFLOAD_ASO_RETURN_REG_REG_C_6_7  = 0x3,
};

struct mlx5_ifc_ipsec_offload_bits {
	u8         modify_field_select[0x40];

	u8         full_offload[0x1];
	u8         reserved_at_41[0x1];
	u8         esn_en[0x1];
	u8         esn_overlap[0x1];
	u8         reserved_at_44[0x2];
	u8         icv_length[0x2];
	u8         reserved_at_48[0x4];
	u8         aso_return_reg[0x4];
	u8         reserved_at_50[0x10];

	u8         esn_msb[0x20];

	u8         reserved_at_80[0x8];
	u8         dekn[0x18];

	u8         salt[0x20];

	u8         implicit_iv[0x40];

	u8         reserved_at_100[0x8];
	u8         ipsec_aso_access_pd[0x18];

	u8         reserved_at_120[0xe0];

	struct mlx5_ifc_ipsec_aso_bits ipsec_aso;
};

enum {
	MLX5_MKC_MA_TRANSLATION_MODE_NONE  = 0x0,
	MLX5_MKC_MA_TRANSLATION_MODE_ATS   = 0x1,
	MLX5_MKC_MA_TRANSLATION_MODE_CAPI  = 0x3,
};

enum {
	MLX5_MKC_CRYPTO_EN_CRYPTO_DISABLED  = 0x0,
	MLX5_MKC_CRYPTO_EN_CRYPTO_ENABLED   = 0x1,
};

struct mlx5_ifc_mkc_bits {
	u8         reserved_at_0[0x1];
	u8         free[0x1];
	u8         reserved_at_2[0x1];
	u8         access_mode_4_2[0x3];
	u8         reserved_at_6[0x7];
	u8         relaxed_ordering_write[0x1];
	u8         reserved_at_e[0x1];
	u8         small_fence_on_rdma_read_response[0x1];
	u8         umr_en[0x1];
	u8         a[0x1];
	u8         rw[0x1];
	u8         rr[0x1];
	u8         lw[0x1];
	u8         lr[0x1];
	u8         access_mode_1_0[0x2];
	u8         reserved_at_18[0x1];
	u8         tunneled_atomic[0x1];
	u8         ma_translation_mode[0x2];
	u8         reserved_at_1c[0x4];

	u8         qpn[0x18];
	u8         mkey_7_0[0x8];

	u8         reserved_at_40[0x8];
	u8         pasid[0x18];

	u8         length64[0x1];
	u8         bsf_en[0x1];
	u8         reserved_at_62[0x3];
	u8         expected_sigerr_count[0x1];
	u8         reserved_at_66[0x1];
	u8         en_rinval[0x1];
	u8         pd[0x18];

	u8         start_addr[0x40];

	u8         len[0x40];

	u8         bsf_octword_size[0x20];

	u8         reserved_at_120[0x60];

	u8         crossing_target_vhca_id[0x10];
	u8         reserved_at_190[0x10];

	u8         translations_octword_size_crossing_target_mkey[0x20];

	u8         reserved_at_1c0[0x19];
	u8         relaxed_ordering_read[0x1];
	u8         reserved_at_1da[0x1];
	u8         log_entity_size[0x5];

	u8         reserved_at_1e0[0x3];
	u8         crypto_en[0x2];
	u8         reserved_at_1e5[0x1b];
};

enum {
	MLX5_NIC_VPORT_CONTEXT_ETH_WQE_TOO_SMALL_MODE_CQE_IN_ERROR        = 0x0,
	MLX5_NIC_VPORT_CONTEXT_ETH_WQE_TOO_SMALL_MODE_CQE_OK_BYTE_CNT_0   = 0x1,
	MLX5_NIC_VPORT_CONTEXT_ETH_WQE_TOO_SMALL_MODE_DISCARD_WQE_NO_CQE  = 0x2,
};

enum {
	MLX5_NIC_VPORT_CONTEXT_ALLOWED_LIST_TYPE_CURRENT_UC_MAC_ADDRESS  = 0x0,
	MLX5_NIC_VPORT_CONTEXT_ALLOWED_LIST_TYPE_CURRENT_MC_MAC_ADDRESS  = 0x1,
	MLX5_NIC_VPORT_CONTEXT_ALLOWED_LIST_TYPE_VLAN_LIST               = 0x2,
};

struct mlx5_ifc_nic_vport_context_bits {
	u8         multi_prio_sq[0x1];
	u8         reserved_at_1[0x4];
	u8         min_wqe_inline_mode[0x3];
	u8         reserved_at_8[0x13];
	u8         eth_wqe_too_small_mode[0x2];
	u8         disable_mc_local_lb[0x1];
	u8         disable_uc_local_lb[0x1];
	u8         roce_en[0x1];

	u8         arm_change_event[0x1];
	u8         reserved_at_21[0x1a];
	u8         event_on_mtu[0x1];
	u8         event_on_promisc_change[0x1];
	u8         event_on_vlan_change[0x1];
	u8         event_on_mc_address_change[0x1];
	u8         event_on_uc_address_change[0x1];

	u8         vhca_id_type[0x1];
	u8         reserved_at_41[0xb];
	u8         affiliation_criteria[0x4];
	u8         affiliated_vhca_id[0x10];

	u8         reserved_at_60[0xc0];

	u8         reserved_at_120[0x10];
	u8         mtu[0x10];

	u8         system_image_guid[0x40];

	u8         port_guid[0x40];

	u8         node_guid[0x40];

	u8         max_qp_retry_limit[0x10];
	u8         max_qp_retry_period[0x10];

	u8         reserved_at_220[0x120];

	u8         qkey_violation_counter[0x10];
	u8         reserved_at_350[0x10];

	u8         reserved_at_360[0x420];

	u8         promisc_uc[0x1];
	u8         promisc_mc[0x1];
	u8         promisc_all[0x1];
	u8         reserved_at_783[0x2];
	u8         allowed_list_type[0x3];
	u8         reserved_at_788[0xc];
	u8         allowed_list_size[0xc];

	struct mlx5_ifc_mac_address_layout_bits permanent_address;

	u8         sw_network_metadata[0x20];

	union mlx5_ifc_nic_vport_context_current_uc_mac_address_auto_bits current_uc_mac_address[0];
};

struct mlx5_ifc_qp_context_extension_bits {
	u8         delay_drop_en[0x1];
	u8         vl15[0x1];
	u8         mmo[0x1];
	u8         error_syndrome_origin[0x1];
	u8         reserved_at_4[0xc];
	u8         dci_stream_channel_id[0x10];

	u8         qos_queue_group_id_requester[0x20];

	u8         qos_queue_group_id_responder[0x20];

	struct mlx5_ifc_cqe_error_syndrome_bits error_syndrome;

	u8         reserved_at_80[0x580];
};

struct mlx5_ifc_qp_pas_list_in_bits {
	struct mlx5_ifc_cmd_pas_bits pas[0];
};

enum {
	MLX5_QPC_STATE_RST        = 0x0,
	MLX5_QPC_STATE_INIT       = 0x1,
	MLX5_QPC_STATE_RTR        = 0x2,
	MLX5_QPC_STATE_RTS        = 0x3,
	MLX5_QPC_STATE_SQER       = 0x4,
	MLX5_QPC_STATE_SQDRAINED  = 0x5,
	MLX5_QPC_STATE_ERR        = 0x6,
};

enum {
	MLX5_QPC_ST_RC            = 0x0,
	MLX5_QPC_ST_UC            = 0x1,
	MLX5_QPC_ST_UD            = 0x2,
	MLX5_QPC_ST_XRC           = 0x3,
	MLX5_QPC_ST_IBL2          = 0x4,
	MLX5_QPC_ST_DCI           = 0x5,
	MLX5_QPC_ST_QP0           = 0x7,
	MLX5_QPC_ST_QP1           = 0x8,
	MLX5_QPC_ST_RAW_DATAGRAM  = 0x9,
	MLX5_QPC_ST_REG_UMR       = 0xc,
	MLX5_QPC_ST_DC_CNAK       = 0x10,
};

enum {
	MLX5_QPC_PM_STATE_ARMED     = 0x0,
	MLX5_QPC_PM_STATE_REARM     = 0x1,
	MLX5_QPC_PM_STATE_MIGRATED  = 0x3,
};

enum {
	MLX5_QPC_REQ_E2E_CREDIT_MODE_MSG_BASED     = 0x0,
	MLX5_QPC_REQ_E2E_CREDIT_MODE_PACKET_BASED  = 0x1,
};

enum {
	MLX5_QPC_OFFLOAD_TYPE_NONE  = 0x0,
	MLX5_QPC_OFFLOAD_TYPE_RNDV  = 0x1,
	MLX5_QPC_OFFLOAD_TYPE_NVME  = 0x2,
};

enum {
	MLX5_QPC_END_PADDING_MODE_SCATTER_AS_IS                = 0x0,
	MLX5_QPC_END_PADDING_MODE_PAD_TO_CACHE_LINE_ALIGNMENT  = 0x1,
};

enum {
	MLX5_QPC_MULTI_USER_QP_TYPE_SINGLE_USER_QP        = 0x0,
	MLX5_QPC_MULTI_USER_QP_TYPE_MULTI_USER_MASTER_QP  = 0x1,
	MLX5_QPC_MULTI_USER_QP_TYPE_MULTI_USER_SLAVE_QP   = 0x2,
};

enum {
	MLX5_QPC_MTU_256_BYTES        = 0x1,
	MLX5_QPC_MTU_512_BYTES        = 0x2,
	MLX5_QPC_MTU_1K_BYTES         = 0x3,
	MLX5_QPC_MTU_2K_BYTES         = 0x4,
	MLX5_QPC_MTU_4K_BYTES         = 0x5,
	MLX5_QPC_MTU_RAW_ETHERNET_QP  = 0x7,
};

enum {
	MLX5_QPC_RETRY_MODE_DEFAULT_MODE      = 0x0,
	MLX5_QPC_RETRY_MODE_GO_BACK_N         = 0x1,
	MLX5_QPC_RETRY_MODE_SELECTIVE_REPEAT  = 0x2,
};

enum {
	MLX5_QPC_TIMESTAMP_FORMAT_FREE_RUNNING = 0x0,
	MLX5_QPC_TIMESTAMP_FORMAT_DEFAULT   = 0x1,
	MLX5_QPC_TIMESTAMP_FORMAT_REAL_TIME = 0x2,
};

enum {
	MLX5_QPC_ULP_STATELESS_OFFLOAD_MODE_NONE            = 0x0,
	MLX5_QPC_ULP_STATELESS_OFFLOAD_MODE_BASIC_IPOIB     = 0x1,
	MLX5_QPC_ULP_STATELESS_OFFLOAD_MODE_ENHANCED_IPOIB  = 0x2,
};

enum {
	MLX5_QPC_SEND_DBR_MODE_DBR_VALID   = 0x0,
	MLX5_QPC_SEND_DBR_MODE_NO_DBR_EXT  = 0x1,
	MLX5_QPC_SEND_DBR_MODE_NO_DBR_INT  = 0x2,
};

enum {
	MLX5_QPC_ATOMIC_MODE_IB_SPEC     = 0x1,
	MLX5_QPC_ATOMIC_MODE_ONLY_8B     = 0x2,
	MLX5_QPC_ATOMIC_MODE_UP_TO_8B    = 0x3,
	MLX5_QPC_ATOMIC_MODE_UP_TO_16B   = 0x4,
	MLX5_QPC_ATOMIC_MODE_UP_TO_32B   = 0x5,
	MLX5_QPC_ATOMIC_MODE_UP_TO_64B   = 0x6,
	MLX5_QPC_ATOMIC_MODE_UP_TO_128B  = 0x7,
	MLX5_QPC_ATOMIC_MODE_UP_TO_256B  = 0x8,
};

enum {
	MLX5_QPC_RQ_TYPE_REGULAR              = 0x0,
	MLX5_QPC_RQ_TYPE_SRQ_RMP_XRC_SRQ_XRQ  = 0x1,
	MLX5_QPC_RQ_TYPE_ZERO_SIZE_RQ         = 0x3,
};

enum {
	MLX5_QPC_CS_REQ_DISABLE    = 0x0,
	MLX5_QPC_CS_REQ_UP_TO_32B  = 0x11,
	MLX5_QPC_CS_REQ_UP_TO_64B  = 0x22,
};

enum {
	MLX5_QPC_CS_RES_DISABLE    = 0x0,
	MLX5_QPC_CS_RES_UP_TO_32B  = 0x1,
	MLX5_QPC_CS_RES_UP_TO_64B  = 0x2,
};

struct mlx5_ifc_qpc_bits {
	u8         state[0x4];
	u8         lag_tx_port_affinity[0x4];
	u8         st[0x8];
	u8         sr_enabled[0x1];
	u8         ar_enabled[0x1];
	u8         isolate_vl_tc[0x1];
	u8         pm_state[0x2];
	u8         rdma_wr_disabled[0x1];
	u8         req_e2e_credit_mode[0x2];
	u8         offload_type[0x4];
	u8         end_padding_mode[0x2];
	u8         multi_user_qp_type[0x2];

	u8         wq_signature[0x1];
	u8         block_lb_mc[0x1];
	u8         atomic_like_write_en[0x1];
	u8         latency_sensitive[0x1];
	u8         dual_write[0x1];
	u8         drain_sigerr[0x1];
	u8         multi_path[0x1];
	u8         multi_path_force[0x1];
	u8         pd[0x18];

	u8         mtu[0x3];
	u8         log_msg_max[0x5];
	u8         reserved_at_48[0x1];
	u8         log_rq_size[0x4];
	u8         log_rq_stride[0x3];
	u8         no_sq[0x1];
	u8         log_sq_size[0x4];
	u8         reserved_at_55[0x1];
	u8         retry_mode[0x2];
	u8         ts_format[0x2];
	u8         data_in_order[0x1];
	u8         rlky[0x1];
	u8         ulp_stateless_offload_mode[0x4];

	u8         counter_set_id[0x8];
	u8         uar_page[0x18];

	u8         send_dbr_mode[0x2];
	u8         reserved_at_82[0x1];
	u8         full_handshake[0x1];
	u8         cnak_reverse_sl[0x4];
	u8         user_index[0x18];

	u8         reserved_at_a0[0x3];
	u8         log_page_size[0x5];
	u8         remote_qpn_or_multi_user_master_qp[0x18];

	struct mlx5_ifc_ads_bits primary_address_path;

	struct mlx5_ifc_ads_bits secondary_address_path;

	u8         log_ack_req_freq[0x4];
	u8         reserved_at_384[0x4];
	u8         log_sra_max[0x3];
	u8         extended_rnr_retry_valid[0x1];
	u8         reserved_at_38c[0x1];
	u8         retry_count[0x3];
	u8         rnr_retry[0x3];
	u8         extended_retry_count_valid[0x1];
	u8         fre[0x1];
	u8         cur_rnr_retry[0x3];
	u8         cur_retry_count[0x3];
	u8         extended_log_rnr_retry[0x5];

	u8         extended_cur_rnr_retry[0x10];
	u8         packet_pacing_rate_limit_index[0x10];

	u8         multi_user_group_size[0x8];
	u8         next_send_psn[0x18];

	u8         reserved_at_3e0[0x3];
	u8         log_num_dci_stream_channels[0x5];
	u8         cqn_snd[0x18];

	u8         reserved_at_400[0x3];
	u8         log_num_dci_errored_streams[0x5];
	u8         deth_sqpn_or_initiator_src_dct[0x18];

	u8         reserved_at_420[0x8];
	u8         extended_retry_count[0x8];
	u8         reserved_at_430[0x8];
	u8         extended_cur_retry_count[0x8];

	u8         reserved_at_440[0x8];
	u8         last_acked_psn[0x18];

	u8         reserved_at_460[0x8];
	u8         ssn[0x18];

	u8         reserved_at_480[0x8];
	u8         log_rra_max[0x3];
	u8         reserved_at_48b[0x1];
	u8         atomic_mode[0x4];
	u8         rre[0x1];
	u8         rwe[0x1];
	u8         rae[0x1];
	u8         reserved_at_493[0x1];
	u8         page_offset[0x6];
	u8         reserved_at_49a[0x3];
	u8         cd_slave_receive[0x1];
	u8         cd_slave_send[0x1];
	u8         cd_master[0x1];

	u8         reserved_at_4a0[0x3];
	u8         min_rnr_nak[0x5];
	u8         next_rcv_psn[0x18];

	u8         reserved_at_4c0[0x8];
	u8         xrcd[0x18];

	u8         reserved_at_4e0[0x8];
	u8         cqn_rcv[0x18];

	u8         dbr_addr[0x40];

	u8         q_key[0x20];

	u8         reserved_at_560[0x5];
	u8         rq_type[0x3];
	u8         srqn_rmpn_xrqn[0x18];

	u8         reserved_at_580[0x8];
	u8         rmsn[0x18];

	u8         hw_sq_wqebb_counter[0x10];
	u8         sw_sq_wqebb_counter[0x10];

	u8         hw_rq_counter[0x20];

	u8         sw_rq_counter[0x20];

	u8         reserved_at_600[0x8];
	u8         roce_adp_retrans_rtt[0x18];

	u8         reserved_at_620[0xf];
	u8         cgs[0x1];
	u8         cs_req[0x8];
	u8         cs_res[0x8];

	u8         dc_access_key[0x40];

	u8         rdma_active[0x1];
	u8         comm_est[0x1];
	u8         suspended[0x1];
	u8         dbr_umem_valid[0x1];
	u8         reserved_at_684[0x4];
	u8         send_msg_psn[0x18];

	u8         reserved_at_6a0[0x8];
	u8         rcv_msg_psn[0x18];

	u8         rdma_va[0x40];

	u8         rdma_key[0x20];

	u8         dbr_umem_id[0x20];
};

union mlx5_ifc_hca_cap_union_bits {
	struct mlx5_ifc_cmd_hca_cap_bits cmd_hca_cap;
	struct mlx5_ifc_per_protocol_networking_offload_caps_bits per_protocol_networking_offload_caps;
	struct mlx5_ifc_odp_cap_bits odp_cap;
	struct mlx5_ifc_atomic_caps_bits atomic_caps;
	struct mlx5_ifc_roce_cap_bits cmd_hca_cap_roce;
	struct mlx5_ifc_flow_table_nic_cap_bits flow_table_nic_cap;
	struct mlx5_ifc_flow_table_eswitch_cap_bits flow_table_eswitch_cap;
	struct mlx5_ifc_e_switch_cap_bits e_switch_cap;
	struct mlx5_ifc_vector_calc_caps_bits vector_calc_caps;
	struct mlx5_ifc_qos_caps_bits qos_caps;
	struct mlx5_ifc_debug_cap_bits debug_cap;
	struct mlx5_ifc_nvmf_cap_bits nvmf_cap;
	struct mlx5_ifc_device_mem_cap_bits device_mem_cap;
	struct mlx5_ifc_device_emulation_cap_bits device_emulation_cap;
	struct mlx5_ifc_tls_cap_bits tls_cap;
	struct mlx5_ifc_dpp_cap_bits dpp_cap;
	struct mlx5_ifc_event_capabilities_bits event_capabilities;
	struct mlx5_ifc_ipsec_cap_bits ipsec_cap;
	struct mlx5_ifc_virtio_emulation_cap_bits virtio_emulation_cap;
	struct mlx5_ifc_hotplug_capabilities_bits hotplug_capabilities;
	struct mlx5_ifc_nvmeotcp_cap_bits nvmeotcp_cap;
	struct mlx5_ifc_crypto_caps_bits crypto_caps;
	struct mlx5_ifc_wqe_based_flow_table_capabilities_bits wqe_based_flow_table_capabilities;
	struct mlx5_ifc_parse_graph_node_cap_bits parse_graph_node_cap;
	struct mlx5_ifc_shampo_cap_bits shampo_cap;
	struct mlx5_ifc_nisp_cap_bits nisp_cap;
	struct mlx5_ifc_macsec_cap_bits macsec_cap;
	struct mlx5_ifc_cmd_hca_cap_2_bits cmd_hca_cap_2;
	struct mlx5_ifc_upt_device_emulation_caps_bits upt_device_emulation_caps;
	struct mlx5_ifc_dpa_capabilities_bits dpa_capabilities;
	struct mlx5_ifc_port_selection_capabilities_bits port_selection_capabilities;
	struct mlx5_ifc_adv_virtualization_capabilities_bits adv_virtualization_capabilities;
	u8         reserved_at_0[0x8000];
};

struct mlx5_ifc_regexp_parameters_bits {
	u8         db_free[0x1];
	u8         reserved_at_1[0x1f];

	u8         db_mkey[0x20];

	u8         db_mkey_address[0x40];

	u8         rof_mkey[0x20];

	u8         rof_size[0x20];

	u8         rof_mkey_address[0x40];

	u8         reserved_at_100[0x80];
};

enum {
	MLX5_RMPC_STATE_RDY  = 0x1,
	MLX5_RMPC_STATE_ERR  = 0x3,
};

struct mlx5_ifc_rmpc_bits {
	u8         reserved_at_0[0x8];
	u8         state[0x4];
	u8         reserved_at_c[0x14];

	u8         basic_cyclic_rcv_wqe[0x1];
	u8         reserved_at_21[0x1f];

	u8         reserved_at_40[0x140];

	struct mlx5_ifc_wq_bits wq;
};

enum {
	MLX5_ROCE_ADDR_LAYOUT_ROCE_L3_TYPE_IPV4  = 0x0,
	MLX5_ROCE_ADDR_LAYOUT_ROCE_L3_TYPE_IPV6  = 0x1,
};

enum {
	MLX5_ROCE_ADDR_LAYOUT_ROCE_VERSION_VERSION_1_0  = 0x0,
	MLX5_ROCE_ADDR_LAYOUT_ROCE_VERSION_VERSION_1_5  = 0x1,
	MLX5_ROCE_ADDR_LAYOUT_ROCE_VERSION_VERSION_2_0  = 0x2,
};

struct mlx5_ifc_roce_addr_layout_bits {
	u8         source_l3_address[4][0x20];

	u8         reserved_at_80[0x2];
	u8         rx_allow_untagged[0x1];
	u8         vlan_valid[0x1];
	u8         vlan_id[0xc];
	u8         source_mac_47_32[0x10];

	u8         source_mac_31_0[0x20];

	u8         reserved_at_c0[0x14];
	u8         roce_l3_type[0x4];
	u8         roce_version[0x8];

	u8         reserved_at_e0[0x20];
};

enum {
	MLX5_RQC_RQ_TYPE_MEMORY_RQ_INLINE      = 0x0,
	MLX5_RQC_RQ_TYPE_MEMORY_RQ_RMP         = 0x1,
};

enum {
	MLX5_RQC_STATE_RST  = 0x0,
	MLX5_RQC_STATE_RDY  = 0x1,
	MLX5_RQC_STATE_ERR  = 0x3,
};

enum {
	MLX5_RQC_HAIRPIN_DATA_BUFFER_TYPE_UNLOCKED_INTERNAL_BUFFER  = 0x0,
	MLX5_RQC_HAIRPIN_DATA_BUFFER_TYPE_LOCKED_INTERNAL_BUFFER    = 0x1,
};

enum {
	MLX5_RQC_TS_FORMAT_FREE_RUNNING_TS  = 0x0,
	MLX5_RQC_TS_FORMAT_DEFAULT_TS       = 0x1,
	MLX5_RQC_TS_FORMAT_REAL_TIME_TS     = 0x2,
};

enum {
	MLX5_RQC_HAIRPIN_OFFLOAD_TYPE_HAIRPIN_ETH_RAW    = 0x0,
	MLX5_RQC_HAIRPIN_OFFLOAD_TYPE_HAIRPIN_IB_RAW     = 0x1,
	MLX5_RQC_HAIRPIN_OFFLOAD_TYPE_HAIRPIN_ETH2IPOIB  = 0x2,
	MLX5_RQC_HAIRPIN_OFFLOAD_TYPE_HAIRPIN_IPOIB2ETH  = 0x3,
};

enum {
	MLX5_RQC_DPP_WIRE_PROTOCOL_ST2110           = 0x0,
	MLX5_RQC_DPP_WIRE_PROTOCOL_ST2110_EXTENDED  = 0x1,
	MLX5_RQC_DPP_WIRE_PROTOCOL_GIGEVISION_EL0   = 0x2,
	MLX5_RQC_DPP_WIRE_PROTOCOL_GIGEVISION_EL1   = 0x3,
	MLX5_RQC_DPP_WIRE_PROTOCOL_ORAN_ECPRI       = 0x4,
};

enum {
	MLX5_RQC_SHAMPO_ALIGNMENT_GRANULARITY_BYTE_ALIGN    = 0x0,
	MLX5_RQC_SHAMPO_ALIGNMENT_GRANULARITY_STRIDE_ALIGN  = 0x1,
	MLX5_RQC_SHAMPO_ALIGNMENT_GRANULARITY_PAGE_ALIGN    = 0x2,
};

enum {
	MLX5_RQC_SHAMPO_MATCH_CRITERIA_TYPE_NO_MATCH  = 0x0,
	MLX5_RQC_SHAMPO_MATCH_CRITERIA_TYPE_EXTENDED  = 0x1,
};

struct mlx5_ifc_rqc_bits {
	u8         rlky[0x1];
	u8         delay_drop_en[0x1];
	u8         scatter_fcs[0x1];
	u8         vlan_strip_disable[0x1];
	u8         mem_rq_type[0x4];
	u8         state[0x4];
	u8         reserved_at_c[0x1];
	u8         flush_in_error_en[0x1];
	u8         hairpin[0x1];
	u8         reserved_at_f[0x6];
	u8         hairpin_data_buffer_type[0x3];
	u8         reserved_at_18[0x2];
	u8         ts_format[0x2];
	u8         hairpin_offload_type[0x4];

	u8         reserved_at_20[0x8];
	u8         user_index[0x18];

	u8         reserved_at_40[0x8];
	u8         cqn[0x18];

	u8         counter_set_id[0x8];
	u8         reserved_at_68[0x18];

	u8         reserved_at_80[0x8];
	u8         rmpn[0x18];

	u8         dpp_wire_protocol[0x8];
	u8         hairpin_peer_sq[0x18];

	u8         dpp_segment_size[0x10];
	u8         hairpin_peer_vhca[0x10];

	u8         dpp_mkey[0x20];

	u8         reserved_at_100[0x10];
	u8         dpp_scatter_offset[0x8];
	u8         reserved_at_118[0x3];
	u8         log_dpp_buffer_size[0x5];

	u8         reserved_at_120[0x6];
	u8         shampo_alignment_granularity[0x2];
	u8         reserved_at_128[0x6];
	u8         shampo_match_criteria_type[0x2];
	u8         shampo_reservation_timeout[0x10];

	u8         reserved_at_140[0x40];

	struct mlx5_ifc_wq_bits wq;
};

enum {
	MLX5_SQC_STATE_RST  = 0x0,
	MLX5_SQC_STATE_RDY  = 0x1,
	MLX5_SQC_STATE_ERR  = 0x3,
};

enum {
	MLX5_SQC_HAIRPIN_WQ_BUFFER_TYPE_UNLOCKED_INTERNAL_BUFFER  = 0x0,
	MLX5_SQC_HAIRPIN_WQ_BUFFER_TYPE_HOST_MEMORY               = 0x1,
};

enum {
	MLX5_SQC_TS_FORMAT_FREE_RUNNING_TS  = 0x0,
	MLX5_SQC_TS_FORMAT_DEFAULT_TS       = 0x1,
	MLX5_SQC_TS_FORMAT_REAL_TIME_TS     = 0x2,
};

enum {
	MLX5_SQC_HAIRPIN_OFFLOAD_TYPE_HAIRPIN_ETH_RAW    = 0x0,
	MLX5_SQC_HAIRPIN_OFFLOAD_TYPE_HAIRPIN_IB_RAW     = 0x1,
	MLX5_SQC_HAIRPIN_OFFLOAD_TYPE_HAIRPIN_ETH2IPOIB  = 0x2,
	MLX5_SQC_HAIRPIN_OFFLOAD_TYPE_HAIRPIN_IPOIB2ETH  = 0x3,
};

struct mlx5_ifc_sqc_bits {
	u8         rlky[0x1];
	u8         cd_master[0x1];
	u8         fre[0x1];
	u8         flush_in_error_en[0x1];
	u8         allow_multi_pkt_send_wqe[0x1];
	u8         min_wqe_inline_mode[0x3];
	u8         state[0x4];
	u8         reg_umr[0x1];
	u8         allow_swp[0x1];
	u8         hairpin[0x1];
	u8         non_wire[0x1];
	u8         static_sq_wq[0x1];
	u8         regexp_en[0x1];
	u8         qos_remap_en[0x1];
	u8         reserved_at_13[0x2];
	u8         hairpin_wq_buffer_type[0x3];
	u8         send_dbr_mode[0x2];
	u8         ts_format[0x2];
	u8         hairpin_offload_type[0x4];

	u8         reserved_at_20[0x8];
	u8         user_index[0x18];

	u8         reserved_at_40[0x8];
	u8         cqn[0x18];

	u8         reserved_at_60[0x8];
	u8         hairpin_peer_rq[0x18];

	u8         reserved_at_80[0x10];
	u8         hairpin_peer_vhca[0x10];

	u8         reserved_at_a0[0x8];
	u8         afu_id[0x18];

	u8         reserved_at_c0[0x8];
	u8         ts_cqe_to_dest_cqn[0x18];

	u8         qos_para_vport_number[0x10];
	u8         packet_pacing_rate_limit_index[0x10];

	u8         tis_lst_sz[0x10];
	u8         qos_queue_group_id[0x10];

	u8         reserved_at_120[0x8];
	u8         queue_handle[0x18];

	u8         reserved_at_140[0x20];

	u8         reserved_at_160[0x8];
	u8         tis_num_0[0x18];

	struct mlx5_ifc_wq_bits wq;
};

enum {
	MLX5_TIRC_DISP_TYPE_DIRECT    = 0x0,
	MLX5_TIRC_DISP_TYPE_INDIRECT  = 0x1,
};

enum {
	MLX5_TIRC_PACKET_MERGE_MASK_IPV4_LRO  = 0x1,
	MLX5_TIRC_PACKET_MERGE_MASK_IPV6_LRO  = 0x2,
};

enum {
	MLX5_TIRC_INLINE_Q_TYPE_RQ            = 0x0,
	MLX5_TIRC_INLINE_Q_TYPE_VIRTIO_NET_Q  = 0x1,
	MLX5_TIRC_INLINE_Q_TYPE_UPT_RQ        = 0x2,
};

enum {
	MLX5_TIRC_RX_HASH_FN_HASH_NONE           = 0x0,
	MLX5_TIRC_RX_HASH_FN_HASH_INVERTED_XOR8  = 0x1,
	MLX5_TIRC_RX_HASH_FN_HASH_TOEPLITZ       = 0x2,
};

enum {
	MLX5_TIRC_SELF_LB_EN_ENABLE_UNICAST    = 0x1,
	MLX5_TIRC_SELF_LB_EN_ENABLE_MULTICAST  = 0x2,
};

struct mlx5_ifc_tirc_bits {
	u8         reserved_at_0[0x20];

	u8         disp_type[0x4];
	u8         tls_en[0x1];
	u8         nvmeotcp_zerocopy_en[0x1];
	u8         nvmeotcp_crc_en[0x1];
	u8         reserved_at_27[0x19];

	u8         reserved_at_40[0x40];

	u8         reserved_at_80[0x4];
	u8         lro_timeout_period_usecs[0x10];
	u8         packet_merge_mask[0x4];
	u8         lro_max_msg_sz[0x8];

	u8         reserved_at_a0[0x8];
	u8         afu_id[0x18];

	u8         inline_rqn_vhca_id_valid[0x1];
	u8         reserved_at_c1[0xf];
	u8         inline_rqn_vhca_id[0x10];

	u8         reserved_at_e0[0x5];
	u8         inline_q_type[0x3];
	u8         inline_rqn[0x18];

	u8         rx_hash_symmetric[0x1];
	u8         reserved_at_101[0x1];
	u8         tunneled_offload_en[0x1];
	u8         reserved_at_103[0x5];
	u8         indirect_table[0x18];

	u8         rx_hash_fn[0x4];
	u8         reserved_at_124[0x2];
	u8         self_lb_en[0x2];
	u8         transport_domain[0x18];

	u8         rx_hash_toeplitz_key[10][0x20];

	struct mlx5_ifc_rx_hash_field_select_bits rx_hash_field_selector_outer;

	struct mlx5_ifc_rx_hash_field_select_bits rx_hash_field_selector_inner;

	u8         nvmeotcp_tag_buffer_table_id[0x20];

	u8         reserved_at_2e0[0x4a0];
};

struct mlx5_ifc_tisc_bits {
	u8         strict_lag_tx_port_affinity[0x1];
	u8         tls_en[0x1];
	u8         nvmeotcp_en[0x1];
	u8         reserved_at_3[0x1];
	u8         lag_tx_port_affinity[0x4];
	u8         reserved_at_8[0x4];
	u8         prio_or_sl[0x4];
	u8         reserved_at_10[0x10];

	u8         reserved_at_20[0x100];

	u8         reserved_at_120[0x8];
	u8         transport_domain[0x18];

	u8         reserved_at_140[0x8];
	u8         underlay_qpn[0x18];

	u8         reserved_at_160[0x8];
	u8         pd[0x18];

	u8         reserved_at_180[0x380];
};

struct mlx5_ifc_general_obj_create_param_bits {
	u8         alias_object[0x1];
	u8         reserved_at_1[0x2];
	u8         log_obj_range[0x5];
	u8         reserved_at_8[0x18];
};

struct mlx5_ifc_alias_context_bits {
	u8         vhca_id_to_be_accessed[0x10];
	u8         reserved_at_10[0xd];
	u8         status[0x3];

	u8         object_id_to_be_accessed[0x20];

	u8         reserved_at_40[0x40];

	u8         access_key[8][0x20];

	u8         metadata[4][0x20];
};

enum {
	MLX5_ALLOC_FLOW_COUNTER_IN_FLOW_COUNTER_BULK_BULK_128  = 0x1,
	MLX5_ALLOC_FLOW_COUNTER_IN_FLOW_COUNTER_BULK_BULK_256  = 0x2,
	MLX5_ALLOC_FLOW_COUNTER_IN_FLOW_COUNTER_BULK_BULK_512  = 0x4,
	MLX5_ALLOC_FLOW_COUNTER_IN_FLOW_COUNTER_BULK_BULK_1K   = 0x8,
	MLX5_ALLOC_FLOW_COUNTER_IN_FLOW_COUNTER_BULK_BULK_2K   = 0x10,
	MLX5_ALLOC_FLOW_COUNTER_IN_FLOW_COUNTER_BULK_BULK_4K   = 0x20,
	MLX5_ALLOC_FLOW_COUNTER_IN_FLOW_COUNTER_BULK_BULK_8K   = 0x40,
	MLX5_ALLOC_FLOW_COUNTER_IN_FLOW_COUNTER_BULK_BULK_16K  = 0x80,
};

struct mlx5_ifc_alloc_flow_counter_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         reserved_at_40[0x8];
	u8         pd[0x18];

	u8         reserved_at_60[0x13];
	u8         flow_counter_bulk_log_size[0x5];
	u8         flow_counter_bulk[0x8];
};

struct mlx5_ifc_alloc_flow_counter_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         flow_counter_id[0x20];

	u8         reserved_at_60[0x20];
};

struct mlx5_ifc_alloc_pd_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         reserved_at_40[0x40];
};

struct mlx5_ifc_alloc_pd_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x8];
	u8         pd[0x18];

	u8         reserved_at_60[0x20];
};

struct mlx5_ifc_alloc_q_counter_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         reserved_at_40[0x40];
};

struct mlx5_ifc_alloc_q_counter_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x18];
	u8         counter_set_id[0x8];

	u8         reserved_at_60[0x20];
};

struct mlx5_ifc_alloc_transport_domain_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         reserved_at_40[0x40];
};

struct mlx5_ifc_alloc_transport_domain_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x8];
	u8         transport_domain[0x18];

	u8         reserved_at_60[0x20];
};

struct mlx5_ifc_allow_other_vhca_access_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         reserved_at_40[0x40];

	u8         reserved_at_80[0x10];
	u8         object_type_to_be_accessed[0x10];

	u8         object_id_to_be_accessed[0x20];

	u8         reserved_at_c0[0x40];

	u8         access_key[8][0x20];
};

struct mlx5_ifc_allow_other_vhca_access_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x40];
};

enum {
	MLX5_CHANNEL_CONNECTION_OBJECT_TRANSPORT_TYPE_UD  = 0x0,
	MLX5_CHANNEL_CONNECTION_OBJECT_TRANSPORT_TYPE_DC  = 0x1,
};

enum {
	MLX5_CHANNEL_CONNECTION_OBJECT_CONNECTION_TYPE_CONNECT_BY_SERVICE_ID  = 0x0,
	MLX5_CHANNEL_CONNECTION_OBJECT_CONNECTION_TYPE_CONNECT_BY_VHCA_ID     = 0x1,
};

struct mlx5_ifc_channel_connection_obj_bits {
	u8         modify_field_select[0x40];

	u8         reserved_at_40[0x20];

	u8         dst_vhca_id[0x10];
	u8         reserved_at_70[0x5];
	u8         transport_type[0x3];
	u8         reserved_at_78[0x5];
	u8         connection_type[0x3];

	u8         my_qpn_or_dci[0x20];

	u8         reserved_at_a0[0x20];

	u8         match_token[0x40];

	u8         provider_id[0x10];
	u8         reserved_at_110[0x10];

	u8         reserved_at_120[0xe0];

	u8         service_name[32][0x20];

	u8         reserved_at_600[0x200];
};

enum {
	MLX5_CHANNEL_SERVICE_OBJECT_TRANSPORT_TYPE_UD  = 0x0,
	MLX5_CHANNEL_SERVICE_OBJECT_TRANSPORT_TYPE_DC  = 0x1,
};

struct mlx5_ifc_channel_service_obj_bits {
	u8         modify_field_select[0x40];

	u8         reserved_at_40[0x20];

	u8         allowed_vhca_id[0x10];
	u8         reserved_at_70[0x5];
	u8         transport_type[0x3];
	u8         reserved_at_78[0x7];
	u8         enforce_token[0x1];

	u8         service_qpn_or_dct[0x20];

	u8         service_qkey[0x20];

	u8         token[0x40];

	u8         dct_key[0x40];

	u8         provider_id[0x10];
	u8         reserved_at_150[0xd];
	u8         channel_status[0x3];

	u8         max_num_connected_client[0x10];
	u8         num_connected_client[0x10];

	u8         reserved_at_180[0x80];

	u8         service_name[32][0x20];

	u8         reserved_at_600[0x200];

	struct mlx5_ifc_channel_connection_client_bits connected_client[0];
};

struct mlx5_ifc_cqe_error_layout_bits {
	u8         reserved_at_0[0x10];
	u8         eth_wqe_id[0x10];

	u8         reserved_at_20[0x80];

	u8         ib_stride_index[0x10];
	u8         reserved_at_b0[0x10];

	u8         reserved_at_c0[0x40];

	u8         reserved_at_100[0x8];
	u8         srqn_or_user_index[0x18];

	u8         reserved_at_120[0x40];

	u8         byte_cnt[0x20];

	u8         reserved_at_180[0x20];

	struct mlx5_ifc_cqe_error_syndrome_bits cqe_error_syndrome;

	u8         s_wqe_opcode[0x8];
	u8         qpn_or_dctn_or_flow_tag[0x18];

	u8         wqe_counter[0x10];
	u8         signature[0x8];
	u8         opcode[0x4];
	u8         reserved_at_1fc[0x3];
	u8         owner[0x1];
};

struct mlx5_ifc_compress_metadata_bits {
	u8         reserved_at_0[0x100];

	u8         crc_32[0x20];

	u8         adler_32[0x20];

	u8         reserved_at_140[0xc0];
};

struct mlx5_ifc_compress_mmo_ctrl_bits {
	u8         reserved_at_0[0x2];
	u8         le[0x1];
	u8         reserved_at_3[0x5];
	u8         dynamic_size[0x4];
	u8         block_size[0x4];
	u8         lz77_window_size[0x4];
	u8         reserved_at_14[0xc];

	u8         reserved_at_20[0x20];
};

struct mlx5_ifc_create_cq_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         reserved_at_40[0x8];
	u8         input_cqn[0x18];

	u8         reserved_at_60[0x20];

	struct mlx5_ifc_cqc_bits cq_context;

	struct mlx5_ifc_cmd_e_mtt_pointer_bits e_mtt_pointer_or_cq_umem_offset;

	u8         cq_umem_id[0x20];

	u8         cq_umem_valid[0x1];
	u8         reserved_at_2e1[0x1f];

	u8         reserved_at_300[0x580];

	struct mlx5_ifc_cmd_pas_bits pas[0];
};

struct mlx5_ifc_create_cq_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x8];
	u8         cqn[0x18];

	u8         reserved_at_60[0x20];
};

struct mlx5_ifc_create_dct_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x8];
	u8         dctn[0x18];

	u8         ece[0x20];
};

struct mlx5_ifc_create_general_obj_in_bits {
	struct mlx5_ifc_general_obj_in_cmd_hdr_bits general_obj_in_cmd_hdr;

	u8         obj_context[0x20];
};

struct mlx5_ifc_create_general_obj_out_bits {
	struct mlx5_ifc_general_obj_out_cmd_hdr_bits general_obj_out_cmd_hdr;
};

struct mlx5_ifc_create_ipsec_offload_in_bits {
	struct mlx5_ifc_general_obj_in_cmd_hdr_bits general_obj_in_cmd_hdr;

	struct mlx5_ifc_ipsec_offload_bits obj_context;
};

struct mlx5_ifc_create_mkey_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         reserved_at_40[0x8];
	u8         input_mkey_index[0x18];

	u8         pg_access[0x1];
	u8         mkey_umem_valid[0x1];
	u8         reserved_at_62[0x1e];

	struct mlx5_ifc_mkc_bits memory_key_mkey_entry;

	struct mlx5_ifc_cmd_e_mtt_pointer_bits e_mtt_pointer;

	struct mlx5_ifc_cmd_e_bsf_pointer_bits e_bsf_pointer;

	u8         translations_octword_actual_size[0x20];

	u8         mkey_umem_id[0x20];

	u8         mkey_umem_offset[0x40];

	u8         reserved_at_380[0x10];
	u8         bsf_octword_actual_size[0x10];

	u8         reserved_at_3a0[0x4e0];

	u8         klm_or_pas_or_mtt_bsf[0][0x20];
};

struct mlx5_ifc_create_mkey_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x8];
	u8         mkey_index[0x18];

	u8         reserved_at_60[0x20];
};

enum {
	MLX5_CREATE_QP_IN_OP_MOD_CREATE_ON_RST   = 0x0,
	MLX5_CREATE_QP_IN_OP_MOD_CREATE_ON_INIT  = 0x1,
};

enum {
	MLX5_CREATE_QP_IN_QPC_EXT_QP_PAS_LIST                 = 0x0,
	MLX5_CREATE_QP_IN_QPC_EXT_QPC_EXTENSION_AND_PAS_LIST  = 0x1,
};

struct mlx5_ifc_create_qp_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         vhca_tunnel_id[0x10];
	u8         op_mod[0x10];

	u8         qpc_ext[0x1];
	u8         reserved_at_41[0x7];
	u8         input_qpn[0x18];

	u8         reserved_at_60[0x20];

	u8         opt_param_mask[0x20];

	u8         ece[0x20];

	struct mlx5_ifc_qpc_bits qpc;

	struct mlx5_ifc_cmd_e_mtt_pointer_bits e_mtt_pointer_or_wq_umem_offset;

	u8         wq_umem_id[0x20];

	u8         wq_umem_valid[0x1];
	u8         reserved_at_861[0x1f];

	struct mlx5_ifc_qp_pas_list_in_bits qpc_pas_list;
};

struct mlx5_ifc_create_qp_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x8];
	u8         qpn[0x18];

	u8         ece[0x20];
};

struct mlx5_ifc_create_rmp_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         reserved_at_40[0xc0];

	struct mlx5_ifc_rmpc_bits ctx;
};

struct mlx5_ifc_create_rmp_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x8];
	u8         rmpn[0x18];

	u8         reserved_at_60[0x20];
};

struct mlx5_ifc_create_rq_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         reserved_at_40[0xc0];

	struct mlx5_ifc_rqc_bits ctx;
};

struct mlx5_ifc_create_rq_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x8];
	u8         rqn[0x18];

	u8         reserved_at_60[0x20];
};

struct mlx5_ifc_create_sq_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         reserved_at_40[0xc0];

	struct mlx5_ifc_sqc_bits ctx;
};

struct mlx5_ifc_create_sq_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x8];
	u8         sqn[0x18];

	u8         reserved_at_60[0x20];
};

struct mlx5_ifc_create_tir_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         reserved_at_40[0xc0];

	struct mlx5_ifc_tirc_bits tir_context;
};

struct mlx5_ifc_create_tir_out_bits {
	u8         status[0x8];
	u8         icm_address_63_40[0x18];

	u8         syndrome[0x20];

	u8         icm_address_39_32[0x8];
	u8         tirn[0x18];

	u8         icm_address_31_0[0x20];
};

struct mlx5_ifc_create_tis_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         reserved_at_40[0xc0];

	struct mlx5_ifc_tisc_bits ctx;
};

struct mlx5_ifc_create_tis_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x8];
	u8         tisn[0x18];

	u8         reserved_at_60[0x20];
};

enum {
	MLX5_DCTC_STATE_ACTIVE    = 0x0,
	MLX5_DCTC_STATE_DRAINING  = 0x1,
	MLX5_DCTC_STATE_DRAINED   = 0x2,
};

enum {
	MLX5_DCTC_OFFLOAD_TYPE_NONE  = 0x0,
	MLX5_DCTC_OFFLOAD_TYPE_RNDV  = 0x1,
	MLX5_DCTC_OFFLOAD_TYPE_NVME  = 0x2,
};

enum {
	MLX5_DCTC_RETRY_MODE_DEFAULT_MODE      = 0x0,
	MLX5_DCTC_RETRY_MODE_GO_BACK_N         = 0x1,
	MLX5_DCTC_RETRY_MODE_SELECTIVE_REPEAT  = 0x2,
};

enum {
	MLX5_DCTC_ATOMIC_MODE_IB_SPEC  = 0x1,
	MLX5_DCTC_ATOMIC_MODE_ONLY_8B  = 0x2,
	MLX5_DCTC_ATOMIC_MODE_UP_8B    = 0x3,
	MLX5_DCTC_ATOMIC_MODE_UP_16B   = 0x4,
	MLX5_DCTC_ATOMIC_MODE_UP_32B   = 0x5,
	MLX5_DCTC_ATOMIC_MODE_UP_64B   = 0x6,
	MLX5_DCTC_ATOMIC_MODE_UP_128B  = 0x7,
	MLX5_DCTC_ATOMIC_MODE_UP_256B  = 0x8,
};

enum {
	MLX5_DCTC_CS_RES_DISABLE    = 0x0,
	MLX5_DCTC_CS_RES_UP_TO_64B  = 0x2,
};

enum {
	MLX5_DCTC_MTU_BYTES_256  = 0x1,
	MLX5_DCTC_MTU_BYTES_512  = 0x2,
	MLX5_DCTC_MTU_BYTES_1K   = 0x3,
	MLX5_DCTC_MTU_BYTES_2K   = 0x4,
	MLX5_DCTC_MTU_BYTES_4K   = 0x5,
};

struct mlx5_ifc_dctc_bits {
	u8         sr_enabled[0x1];
	u8         ar_enabled[0x1];
	u8         reserved_at_2[0x2];
	u8         state[0x4];
	u8         reserved_at_8[0x10];
	u8         offload_type[0x4];
	u8         reserved_at_1c[0x1];
	u8         data_in_order[0x1];
	u8         retry_mode[0x2];

	u8         reserved_at_20[0x7];
	u8         multi_path_force[0x1];
	u8         user_index[0x18];

	u8         reserved_at_40[0x8];
	u8         cqn[0x18];

	u8         counter_set_id[0x8];
	u8         atomic_mode[0x4];
	u8         rre[0x1];
	u8         rwe[0x1];
	u8         rae[0x1];
	u8         atomic_like_write_en[0x1];
	u8         latency_sensitive[0x1];
	u8         rlky[0x1];
	u8         force_full_handshake[0x1];
	u8         multi_path[0x1];
	u8         reserved_at_74[0xc];

	u8         reserved_at_80[0x8];
	u8         cs_res[0x8];
	u8         reserved_at_90[0x3];
	u8         min_rnr_nak[0x5];
	u8         reserved_at_98[0x8];

	u8         reserved_at_a0[0x8];
	u8         srqn_or_xrqn[0x18];

	u8         reserved_at_c0[0x8];
	u8         pd[0x18];

	u8         tclass[0x8];
	u8         reserved_at_e8[0x4];
	u8         flow_label[0x14];

	u8         dc_access_key[0x40];

	u8         reserved_at_140[0x5];
	u8         mtu[0x3];
	u8         port[0x8];
	u8         pkey_index[0x10];

	u8         reserved_at_160[0x8];
	u8         my_addr_index[0x8];
	u8         reserved_at_170[0x8];
	u8         hop_limit[0x8];

	u8         dc_access_key_violation_count[0x20];

	u8         reserved_at_1a0[0x15];
	u8         eth_prio[0x3];
	u8         ecn[0x2];
	u8         dscp[0x6];

	u8         reserved_at_1c0[0x8];
	u8         min_guaranteed_connection[0x18];

	u8         reserved_at_1e0[0x8];
	u8         max_outstanding_connections[0x18];

	u8         ece[0x20];

	u8         reserved_at_220[0x160];
};

struct mlx5_ifc_dealloc_pd_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         reserved_at_40[0x8];
	u8         pd[0x18];

	u8         reserved_at_60[0x20];
};

struct mlx5_ifc_dealloc_pd_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x40];
};

struct mlx5_ifc_decompress_metadata_bits {
	u8         reserved_at_0[0x60];

	u8         crc_32[0x20];

	u8         adler_32[0x20];

	u8         crc_32c[0x20];

	u8         xxh_32[0x20];

	u8         reserved_at_e0[0x20];

	u8         crc_32_v1[0x20];

	u8         adler_32_v1[0x20];

	u8         reserved_at_140[0xc0];
};

struct mlx5_ifc_destroy_rq_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         reserved_at_40[0x8];
	u8         rqn[0x18];

	u8         reserved_at_60[0x20];
};

struct mlx5_ifc_destroy_rq_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x40];
};

struct mlx5_ifc_destroy_sq_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         reserved_at_40[0x8];
	u8         sqn[0x18];

	u8         reserved_at_60[0x20];
};

struct mlx5_ifc_destroy_sq_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x40];
};

enum {
	MLX5_ENCRYPTION_KEY_OBJ_MODIFY_FIELD_SELECT_ALL_FIELDS  = 0x1,
};

enum {
	MLX5_ENCRYPTION_KEY_OBJ_STATE_READY          = 0x0,
	MLX5_ENCRYPTION_KEY_OBJ_STATE_ERROR          = 0x1,
	MLX5_ENCRYPTION_KEY_OBJ_STATE_UNINITIALIZED  = 0x2,
};

enum {
	MLX5_ENCRYPTION_KEY_OBJ_KEY_SIZE_SIZE_128  = 0x0,
	MLX5_ENCRYPTION_KEY_OBJ_KEY_SIZE_SIZE_256  = 0x1,
};

enum {
	MLX5_ENCRYPTION_KEY_OBJ_KEY_PURPOSE_TLS          = 0x1,
	MLX5_ENCRYPTION_KEY_OBJ_KEY_PURPOSE_IPSEC        = 0x2,
	MLX5_ENCRYPTION_KEY_OBJ_KEY_PURPOSE_AES_XTS      = 0x3,
	MLX5_ENCRYPTION_KEY_OBJ_KEY_PURPOSE_MACSEC       = 0x4,
	MLX5_ENCRYPTION_KEY_OBJ_KEY_PURPOSE_GCM_MEM2MEM  = 0x5,
	MLX5_ENCRYPTION_KEY_OBJ_KEY_PURPOSE_NISP         = 0x6,
};

struct mlx5_ifc_encryption_key_obj_bits {
	u8         modify_field_select[0x40];

	u8         state[0x8];
	u8         sw_wrapped[0x1];
	u8         reserved_at_49[0xb];
	u8         key_size[0x4];
	u8         has_keytag[0x1];
	u8         reserved_at_59[0x3];
	u8         key_purpose[0x4];

	u8         reserved_at_60[0x8];
	u8         pd[0x18];

	u8         reserved_at_80[0x100];

	u8         opaque[0x40];

	u8         reserved_at_1c0[0x40];

	u8         key[32][0x20];

	u8         sw_wrapped_dek[32][0x20];

	u8         reserved_at_a00[0x600];
};

struct mlx5_ifc_hotplug_device_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x60];

	struct mlx5_ifc_hotplug_device_obj_bits hotplug_device_object;
};

struct mlx5_ifc_hotplug_device_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x40];

	struct mlx5_ifc_hotplug_device_obj_bits hotplug_device_object;
};

struct mlx5_ifc_hotunplug_device_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x60];

	struct mlx5_ifc_hotplug_device_obj_bits hotplug_device_object;
};

struct mlx5_ifc_hotunplug_device_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x40];
};

struct mlx5_ifc_init2init_qp_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         vhca_tunnel_id[0x10];
	u8         op_mod[0x10];

	u8         qpc_ext[0x1];
	u8         reserved_at_41[0x7];
	u8         qpn[0x18];

	u8         reserved_at_60[0x20];

	u8         opt_param_mask[0x20];

	u8         ece[0x20];

	struct mlx5_ifc_qpc_bits qpc;

	u8         reserved_at_800[0x40];

	u8         opt_param_mask_95_32[0x40];

	struct mlx5_ifc_qp_context_extension_bits qpc_data_extension;
};

struct mlx5_ifc_init2init_qp_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x20];

	u8         ece[0x20];
};

enum {
	MLX5_INIT2RTR_QP_IN_OP_MOD_INIT2RTR  = 0x0,
	MLX5_INIT2RTR_QP_IN_OP_MOD_INIT2RTS  = 0x1,
};

struct mlx5_ifc_init2rtr_qp_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         vhca_tunnel_id[0x10];
	u8         op_mod[0x10];

	u8         qpc_ext[0x1];
	u8         reserved_at_41[0x7];
	u8         qpn[0x18];

	u8         reserved_at_60[0x20];

	u8         opt_param_mask[0x20];

	u8         ece[0x20];

	struct mlx5_ifc_qpc_bits qpc;

	u8         reserved_at_800[0x40];

	u8         opt_param_mask_95_32[0x40];

	struct mlx5_ifc_qp_context_extension_bits qpc_data_extension;
};

struct mlx5_ifc_init2rtr_qp_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x20];

	u8         ece[0x20];
};

struct mlx5_ifc_modify_ipsec_offload_in_bits {
	struct mlx5_ifc_general_obj_in_cmd_hdr_bits general_obj_in_cmd_hdr;

	struct mlx5_ifc_ipsec_offload_bits obj_context;
};

struct mlx5_ifc_modify_ipsec_offload_out_bits {
	struct mlx5_ifc_general_obj_out_cmd_hdr_bits general_obj_out_cmd_hdr;
};

struct mlx5_ifc_modify_rq_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         rq_state[0x4];
	u8         reserved_at_44[0x4];
	u8         rqn[0x18];

	u8         reserved_at_60[0x20];

	u8         modify_bitmask[0x40];

	u8         reserved_at_c0[0x40];

	struct mlx5_ifc_rqc_bits ctx;
};

struct mlx5_ifc_modify_rq_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x40];
};

struct mlx5_ifc_modify_sq_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         sq_state[0x4];
	u8         reserved_at_44[0x4];
	u8         sqn[0x18];

	u8         reserved_at_60[0x20];

	u8         modify_bitmask[0x40];

	u8         reserved_at_c0[0x40];

	struct mlx5_ifc_sqc_bits ctx;
};

struct mlx5_ifc_modify_sq_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x40];
};

struct mlx5_ifc_qp_2err_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         vhca_tunnel_id[0x10];
	u8         op_mod[0x10];

	u8         reserved_at_40[0x8];
	u8         qpn[0x18];

	u8         reserved_at_60[0x20];
};

struct mlx5_ifc_qp_2err_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x40];
};

struct mlx5_ifc_qp_2rst_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         vhca_tunnel_id[0x10];
	u8         op_mod[0x10];

	u8         reserved_at_40[0x8];
	u8         qpn[0x18];

	u8         reserved_at_60[0x20];
};

struct mlx5_ifc_qp_2rst_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x40];
};

struct mlx5_ifc_qpc_extension_and_pas_list_in_bits {
	u8         qpc_data_extension[48][0x20];

	u8         pas[0][0x40];
};

enum {
	MLX5_QUERY_EMULATED_FUNCTIONS_INFO_IN_OP_MOD_NVME_PHYSICAL_FUNCTIONS        = 0x0,
	MLX5_QUERY_EMULATED_FUNCTIONS_INFO_IN_OP_MOD_VIRTIO_NET_PHYSICAL_FUNCTIONS  = 0x1,
	MLX5_QUERY_EMULATED_FUNCTIONS_INFO_IN_OP_MOD_VIRTIO_BLK_PHYSICAL_FUNCTIONS  = 0x2,
	MLX5_QUERY_EMULATED_FUNCTIONS_INFO_IN_OP_MOD_VIRTUAL_FUNCTIONS              = 0x3,
	MLX5_QUERY_EMULATED_FUNCTIONS_INFO_IN_OP_MOD_VIRTIO_FS_PHYSICAL_FUNCTIONS   = 0x5,
};

struct mlx5_ifc_query_emulated_functions_info_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         reserved_at_40[0x20];

	u8         reserved_at_60[0x10];
	u8         pf_vhca_id[0x10];
};

struct mlx5_ifc_query_emulated_functions_info_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x20];

	u8         reserved_at_60[0x10];
	u8         num_emulated_functions[0x10];

	struct mlx5_ifc_emulated_function_info_bits emulated_function_info[0];
};

enum {
	MLX5_QUERY_HCA_CAP_IN_OP_MOD_GENERAL_DEVICE_CAPABILITIES_2      = 0x20,
	MLX5_QUERY_HCA_CAP_IN_OP_MOD_UPT_DEVICE_EMULATION_CAPABILITIES  = 0x21,
	MLX5_QUERY_HCA_CAP_IN_OP_MOD_VIRTIO_FS_EMULATION_CAPABILITIES   = 0x23,
	MLX5_QUERY_HCA_CAP_IN_OP_MOD_DPA_CAPABILITIES                   = 0x24,
	MLX5_QUERY_HCA_CAP_IN_OP_MOD_PORT_SELECTION_CAPABILITIES        = 0x25,
	MLX5_QUERY_HCA_CAP_IN_OP_MOD_ADV_VIRTUALIZATION_CAPABILITIES    = 0x26,
};

struct mlx5_ifc_query_hca_cap_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         other_function[0x1];
	u8         ec_vf_function[0x1];
	u8         reserved_at_42[0xe];
	u8         function_id[0x10];

	u8         reserved_at_60[0x20];
};

struct mlx5_ifc_query_hca_cap_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x40];

	union mlx5_ifc_hca_cap_union_bits capability;
};

struct mlx5_ifc_query_hca_vport_context_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         other_vport[0x1];
	u8         reserved_at_41[0xb];
	u8         port_num[0x4];
	u8         vport_number[0x10];

	u8         reserved_at_60[0x20];
};

struct mlx5_ifc_query_hca_vport_context_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x40];

	struct mlx5_ifc_hca_vport_context_bits hca_vport_context;
};

struct mlx5_ifc_query_host_net_functions_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         reserved_at_40[0x40];
};

struct mlx5_ifc_query_host_net_functions_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x40];

	struct mlx5_ifc_host_params_bits host_params_context;

	u8         reserved_at_280[0x180];

	u8         sf_enable[0][0x40];
};

struct mlx5_ifc_query_ipsec_offload_out_bits {
	struct mlx5_ifc_general_obj_out_cmd_hdr_bits general_obj_out_cmd_hdr;

	struct mlx5_ifc_ipsec_offload_bits obj_context;
};

struct mlx5_ifc_query_issi_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         reserved_at_40[0x40];
};

struct mlx5_ifc_query_issi_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x10];
	u8         current_issi[0x10];

	u8         reserved_at_60[0xa0];

	u8         supported_issi[20][0x20];
};

struct mlx5_ifc_query_nic_vport_context_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         other_vport[0x1];
	u8         reserved_at_41[0xf];
	u8         vport_number[0x10];

	u8         reserved_at_60[0x5];
	u8         allowed_list_type[0x3];
	u8         reserved_at_68[0x18];
};

struct mlx5_ifc_query_nic_vport_context_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x40];

	struct mlx5_ifc_nic_vport_context_bits nic_vport_context;
};

struct mlx5_ifc_query_q_counter_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         reserved_at_40[0x80];

	u8         clear[0x1];
	u8         reserved_at_c1[0x1f];

	u8         reserved_at_e0[0x18];
	u8         counter_set_id[0x8];
};

struct mlx5_ifc_query_q_counter_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x40];

	u8         rx_write_requests[0x20];

	u8         reserved_at_a0[0x20];

	u8         rx_read_requests[0x20];

	u8         reserved_at_e0[0x20];

	u8         rx_atomic_requests[0x20];

	u8         reserved_at_120[0x20];

	u8         rx_dct_connect[0x20];

	u8         reserved_at_160[0x20];

	u8         out_of_buffer[0x20];

	u8         reserved_at_1a0[0x20];

	u8         out_of_sequence[0x20];

	u8         reserved_at_1e0[0x20];

	u8         duplicate_request[0x20];

	u8         reserved_at_220[0x20];

	u8         rnr_nak_retry_err[0x20];

	u8         reserved_at_260[0x20];

	u8         packet_seq_err[0x20];

	u8         reserved_at_2a0[0x20];

	u8         implied_nak_seq_err[0x20];

	u8         reserved_at_2e0[0x20];

	u8         local_ack_timeout_err[0x20];

	u8         reserved_at_320[0x20];

	u8         resp_rnr_nak[0x20];

	u8         reserved_at_360[0x20];

	u8         req_rnr_retries_exceeded[0x20];

	u8         reserved_at_3a0[0x20];

	u8         resp_local_length_error[0x20];

	u8         req_local_length_error[0x20];

	u8         resp_local_qp_error[0x20];

	u8         local_operation_error[0x20];

	u8         resp_local_protection[0x20];

	u8         req_local_protection[0x20];

	u8         resp_cqe_error[0x20];

	u8         req_cqe_error[0x20];

	u8         req_memory_window_binding[0x20];

	u8         req_bad_response[0x20];

	u8         req_remote_invalid_request[0x20];

	u8         resp_remote_invalid_request[0x20];

	u8         req_remote_access_errors[0x20];

	u8         resp_remote_access_errors[0x20];

	u8         req_remote_operation_errors[0x20];

	u8         req_transport_retries_exceeded[0x20];

	u8         cq_overflow[0x20];

	u8         resp_cqe_flush_error[0x20];

	u8         req_cqe_flush_error[0x20];

	u8         max_qp_retry_freq_exceeded[0x20];

	u8         roce_adp_retrans[0x20];

	u8         roce_adp_retrans_to[0x20];

	u8         roce_slow_restart[0x20];

	u8         roce_slow_restart_cnps[0x20];

	u8         roce_slow_restart_trans[0x20];

	u8         reserved_at_6e0[0x120];
};

enum {
	MLX5_QUERY_QP_IN_QPC_EXT_QP_PAS_LIST                 = 0x0,
	MLX5_QUERY_QP_IN_QPC_EXT_QPC_EXTENSION_AND_PAS_LIST  = 0x1,
};

struct mlx5_ifc_query_qp_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         vhca_tunnel_id[0x10];
	u8         op_mod[0x10];

	u8         qpc_ext[0x1];
	u8         reserved_at_41[0x7];
	u8         qpn[0x18];

	u8         reserved_at_60[0x20];
};

struct mlx5_ifc_query_qp_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x40];

	u8         opt_param_mask[0x20];

	u8         ece[0x20];

	struct mlx5_ifc_qpc_bits qpc;

	struct mlx5_ifc_cmd_e_mtt_pointer_bits e_mtt_pointer_or_wq_umem_offset;

	u8         wq_umem_id[0x20];

	u8         wq_umem_valid[0x1];
	u8         reserved_at_861[0x1f];

	struct mlx5_ifc_qp_pas_list_in_bits qpc_pas_list;
};

struct mlx5_ifc_query_roce_address_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         vhca_tunnel_id[0x10];
	u8         op_mod[0x10];

	u8         roce_address_index[0x10];
	u8         reserved_at_50[0xc];
	u8         vhca_port_num[0x4];

	u8         reserved_at_60[0x20];
};

struct mlx5_ifc_query_roce_address_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x40];

	struct mlx5_ifc_roce_addr_layout_bits roce_address;
};

struct mlx5_ifc_query_vuid_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x40];

	u8         query_vfs_vuid[0x1];
	u8         reserved_at_61[0xf];
	u8         vhca_id[0x10];
};

struct mlx5_ifc_query_vuid_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x1a0];

	u8         reserved_at_1e0[0x10];
	u8         num_of_entries[0x10];

	struct mlx5_ifc_array1024_auto_bits vuid[0];
};

struct mlx5_ifc_regexp_metadata_bits {
	u8         reserved_at_0[0x100];

	u8         reserved_at_100[0x10];
	u8         latency_count[0x10];

	u8         instruction_count[0x10];
	u8         primary_thread_count[0x10];

	u8         match_count[0x8];
	u8         detected_match_count[0x8];
	u8         status[0x10];

	u8         job_id[0x20];

	u8         reserved_at_180[0x80];
};

struct mlx5_ifc_regexp_mmo_ctrl_bits {
	u8         reserved_at_0[0x2];
	u8         le[0x1];
	u8         reserved_at_3[0x1];
	u8         subset_id_0[0xc];
	u8         reserved_at_10[0x4];
	u8         subset_id_1[0xc];

	u8         reserved_at_20[0x2];
	u8         mode[0x2];
	u8         subset_id_2[0xc];
	u8         reserved_at_30[0x4];
	u8         subset_id_3[0xc];
};

struct mlx5_ifc_rst2init_qp_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         vhca_tunnel_id[0x10];
	u8         op_mod[0x10];

	u8         qpc_ext[0x1];
	u8         reserved_at_41[0x7];
	u8         qpn[0x18];

	u8         reserved_at_60[0x20];

	u8         opt_param_mask[0x20];

	u8         ece[0x20];

	struct mlx5_ifc_qpc_bits qpc;

	u8         reserved_at_800[0x40];

	u8         opt_param_mask_95_32[0x40];

	struct mlx5_ifc_qp_context_extension_bits qpc_data_extension;
};

struct mlx5_ifc_rst2init_qp_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x20];

	u8         ece[0x20];
};

struct mlx5_ifc_rtr2rts_qp_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         vhca_tunnel_id[0x10];
	u8         op_mod[0x10];

	u8         qpc_ext[0x1];
	u8         reserved_at_41[0x7];
	u8         qpn[0x18];

	u8         reserved_at_60[0x20];

	u8         opt_param_mask[0x20];

	u8         ece[0x20];

	struct mlx5_ifc_qpc_bits qpc;

	u8         reserved_at_800[0x40];

	u8         opt_param_mask_95_32[0x40];

	struct mlx5_ifc_qp_context_extension_bits qpc_data_extension;
};

struct mlx5_ifc_rtr2rts_qp_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x20];

	u8         ece[0x20];
};

enum {
	MLX5_SET_REGEXP_PARAMS_IN_FIELD_SELECT_DB_MKEY  = 0x1,
	MLX5_SET_REGEXP_PARAMS_IN_FIELD_SELECT_ROF      = 0x4,
};

struct mlx5_ifc_set_regexp_params_in_bits {
	u8         opcode[0x10];
	u8         uid[0x10];

	u8         reserved_at_20[0x10];
	u8         op_mod[0x10];

	u8         reserved_at_40[0x18];
	u8         engine_id[0x8];

	u8         field_select[0x20];

	struct mlx5_ifc_regexp_parameters_bits regexp_params;
};

struct mlx5_ifc_set_regexp_params_out_bits {
	u8         status[0x8];
	u8         reserved_at_8[0x18];

	u8         syndrome[0x20];

	u8         reserved_at_40[0x40];
};

struct mlx5_ifc_sha_mmo_control_bits {
	u8         reserved_at_0[0x1];
	u8         re[0x1];
	u8         le[0x1];
	u8         reserved_at_3[0x11];
	u8         sha_func[0x4];
	u8         reserved_at_18[0x3];
	u8         pe[0x1];
	u8         reserved_at_1c[0x4];

	u8         reserved_at_20[0x20];
};

struct mlx5_ifc_ud_adrs_vector_bits {
	u8         dc_key[0x40];

	u8         ext[0x1];
	u8         reserved_at_41[0x3];
	u8         reverse_sl[0x4];
	u8         destination_qp_or_dct[0x18];

	u8         static_rate[0x4];
	u8         sl_or_eth_prio[0x4];
	u8         fl[0x1];
	u8         mlid[0x7];
	u8         rlid_or_udp_sport[0x10];

	u8         reserved_at_80[0x20];

	u8         rmac_47_16[0x20];

	u8         rmac_15_0[0x10];
	u8         tclass[0x8];
	u8         hop_limit[0x8];

	u8         reserved_at_e0[0x1];
	u8         grh[0x1];
	u8         reserved_at_e2[0x2];
	u8         src_addr_index[0x8];
	u8         flow_label[0x14];

	u8         rgid_or_rip[4][0x20];
};

struct mlx5_ifc_umr_mem_buffer_bits {
	u8         byte_count[0x20];

	u8         mkey[0x20];

	u8         address[0x40];
};

#ifdef __cplusplus
}
#endif

#endif /* MLX5_PRM_H */
