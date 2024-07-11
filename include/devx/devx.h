#ifndef __DEVX_H__
#define __DEVX_H__

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DEVX_OK (0)
#define DEVX_FAILED (-1)
#define DEVX_VUID_BYTES 128

/* interface for user to set his own log function */
typedef void (devx_log_fn)(const char *log_str);
void devx_set_log_fn(devx_log_fn *log_fn);
void devx_logger_set_level(int level);

struct devx_host_net_functions {
    uint8_t host_number;
    bool vf_info_valid;
    uint16_t total_vf_cnt;
    uint16_t vf_cnt;
    uint16_t pf_vhca_id;
    uint16_t pci_bus;
    uint16_t pci_device;
    uint16_t pci_function;
    uint16_t vf_stride;
    uint16_t first_vf_offset;
};

struct devx_hca_capabilities {
    uint16_t vhca_id;
    bool vhca_resource_manager;
    bool hca_cap_2;
    bool cross_gvmi_mkey_enabled;
    bool emulation_manager_virtio_net;
    bool emulation_manager_virtio_blk;
    bool emulation_manager_nvme;
    bool emulation_manager_virtio_fs;
    bool hotplug_manager;
    bool eswitch_manager;
    bool introspection_mkey_access_allowed;
    uint32_t introspection_mkey;
    uint64_t general_object_types_supported;
    bool ib_port_type;
    bool crossing_vhca_mkey_supported;
    uint8_t num_ports;
    uint8_t native_port_num;
    bool wod_supported;
    size_t uar_reg_size;
};


struct devx_vuid {
    uint8_t id[DEVX_VUID_BYTES];
};

enum {
    DEVX_EMU_OP_MOD_NVME_PHYSICAL_FUNCTIONS = 0x0,
    DEVX_EMU_OP_MOD_VIRTIO_NET_PHYSICAL_FUNCTIONS = 0x1,
    DEVX_EMU_OP_MOD_VIRTIO_BLK_PHYSICAL_FUNCTIONS = 0x2,
    DEVX_EMU_OP_MOD_VIRTUAL_FUNCTIONS = 0x3,
    DEVX_EMU_OP_MOD_VIRTIO_FS_PHYSICAL_FUNCTIONS = 0x5,
    DEVX_EMU_OP_MOD_GENERIC_PCI_PHYSICAL_FUNCTIONS = 0x6,
};

struct devx_emulated_devinfo {
    uint16_t vhca_id;
    uint16_t pci_bdf;
    uint16_t nb_total_vfs;
    uint16_t nb_vfs;
    uint16_t op_mod;
    bool hotplug;
    bool vf_exist;
    bool is_vf;
};

struct devx_klm {
    uint32_t byte_count;
    uint32_t mkey;
    uint64_t address;
};

struct devx_mr;
struct ibv_context;
struct ibv_pd;

/*
 * Query host net functions from DPU side.
 *
 * @param context   the context of ibv device (DPU side)
 * @param funcs     host net functions
 * @return          return 0 when succeed; return -1 when failed.
 */
int devx_query_host_net_functions(struct ibv_context *context, struct devx_host_net_functions *funcs);

/*
 * Query HCA capabilities
 *
 * @param context   the context of ibv device (Host or DPU side)
 * @param caps      HCA capabilities
 * @return          return 0 when succeed; return -1 when failed.
 */
int devx_query_hca_caps(struct ibv_context *context, struct devx_hca_capabilities *caps);

/*
 * Query HCA capabilities with function_id
 *
 * @param context          the context of ibv device (Host or DPU side)
 * @param caps             HCA capabilities
 * @param other_function   Query other function
 * @param function_id      Function Id
 * @return                 return 0 when succeed; return -1 when failed.
 */
int devx_query_hca_caps_ex(struct ibv_context *context, struct devx_hca_capabilities *caps, bool other_function, uint16_t function_id);

/*
 * Query HCA capabilities with function_id
 *
 * @param context          the context of ibv device (DPU side)
 * @param vhca_id          vhca id to query
 * @param vuid             vuid
 * @return                 return 0 when succeed; return -1 when failed.
 */
int devx_query_hca_vuid(struct ibv_context *context, uint16_t vhca_id, struct devx_vuid *vuid);

/*
 * Query pd number of ibv_pd
 *
 * @param pd        the ibv pd
 * @param pdn       pd number
 * @return          return 0 when succeed; return -1 when failed.
 */
int devx_query_ibv_pd_number(struct ibv_pd *pd, uint32_t *pdn);

/*
 * Register a memory region using umem
 *
 * @param pd        the ibv pd
 * @param addr      address of memory region to register
 * @param size      size of memory region to register
 * @param access    access flags, same with ib_reg_mr
 * @return          return pointer of mr when succeed; return NULL when failed.
 */
struct devx_mr *devx_reg_mr(struct ibv_pd *pd, void *addr, size_t size, uint32_t access);

/*
 * Register a memory region using ibv_reg_mr
 *
 * @param pd        the ibv pd
 * @param addr      address of memory region to register
 * @param size      size of memory region to register
 * @param access    access flags, same with ib_reg_mr
 * @return          return pointer of mr when succeed; return NULL when failed.
 */
struct devx_mr *devx_ibv_reg_mr(struct ibv_pd *pd, void *addr, size_t size, uint32_t access);

/*
 * Deregister a memory region
 *
 * @param mr        pointer of memory region
 * @return          return 0 when succeed; return -1 when failed.
 */
int devx_dereg_mr(struct devx_mr *mr);

/*
 * Query mkey of a memory region
 *
 * @param mr        pointer of memory region
 * @return          mkey
 */
uint32_t devx_mr_query_mkey(struct devx_mr *mr);

/*
 * Allow other VHCA access to a memory region
 *
 * @param mr          pointer of memory region
 * @param access_key  pointer of access key
 * @param len         length of access key, MUST be 32
 * @return            return 0 when succeed; return -1 when failed.
 */
int devx_mr_allow_other_vhca_access(struct devx_mr *mr, void *access_key, size_t len);

/*
 * Create a crossing memory region
 *
 * @param pd              the ibv pd
 * @param addr            address of memory region
 * @param size            size of memory region
 * @param vhca_id         vhca id to access
 * @param other_mkey      mkey to access
 * @param access_key      pointer of access key
 * @param access_key_len  length of access key, MUST be 32
 * @return                return pointer of mr when succeed; return NULL when failed.
 */
struct devx_mr *devx_create_crossing_mr(struct ibv_pd *pd, void *addr, size_t size, uint16_t vhca_id, uint32_t other_mkey, void *access_key, size_t access_key_len);

/*
 * Create a crossing vhca memory region
 *
 * @param pd                  the ibv pd
 * @param introspection_mkey  introspection mkey to access
 * @param vhca_id             vhca id to access
 * @return                    return pointer of mr when succeed; return NULL when failed.
 */
struct devx_mr *devx_create_crossing_vhca_mr(struct ibv_pd *pd, uint32_t introspection_mkey, uint16_t vhca_id, uint32_t access_flags);


/*
 * Query emulated functions info
 *
 * @param context          the context of ibv device (Host or DPU side)
 * @param op_mod           function op mod
 * @param pf_op_mod        function op mod of pf
 * @param pf_vhca_id       vhca id of pf
 * @param devinfos         functions info [OUT]
 * @param nb_max_functions functions info limit
 * @return                 return count of functions when succeed; return DEVX_FAILED when failed.
 */
int devx_query_emulated_functions_info(struct ibv_context *context,
        uint16_t op_mod,
        uint16_t pf_op_mod,
        uint16_t pf_vhca_id,
        struct devx_emulated_devinfo *devinfos,
        uint16_t nb_max_functions);

/*
 * Create a indirect memory region
 *
 * @param pd              the ibv pd
 * @param addr            address of memory region
 * @param size            size of memory region
 * @param access_flags    access flags, same with ib_reg_mr
 * @param klm_array       array of KLMs
 * @param klm_num         number of KLMs
 * @return                return pointer of mr when succeed; return NULL when failed.
 */
struct devx_mr *devx_create_indirect_mr(struct ibv_pd *pd, uint64_t addr, uint64_t len, uint32_t access_flags, struct devx_klm *klm_array, int klm_num);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __DEVX_H__ */

