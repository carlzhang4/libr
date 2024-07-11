#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <infiniband/verbs.h>
#include <infiniband/mlx5dv.h>
#include "util.hpp"
#include "multi_os_prm.h"
#include "devx.h"

#define LOCAL_MKEY_TO_REMOTE_MKEY_BIT 0x100

int devx_query_host_net_functions(struct ibv_context *context, struct devx_host_net_functions *funcs) {
    uint32_t in[MLX5_ST_SZ_DW(query_host_net_functions_in)] = { 0 };
    uint32_t out[MLX5_ST_SZ_DW(query_host_net_functions_out)] = { 0 };

    /* Set operation code and operation mode of the command. */
    MLX5_SET(query_host_net_functions_in, in, opcode, MLX5_CMD_OPCODE_QUERY_HOST_NET_FUNCTIONS);

    /* Send the command to FW using DevX. */
    int ret = mlx5dv_devx_general_cmd(context, in, sizeof(in), out, sizeof(out));
    if (ret) {
        LOG_E("Failed to query host net functions");
        return DEVX_FAILED;
    }

    /* Inspect the returned status of the command and print the syndrome in case of error. */
    uint8_t status = MLX5_GET(query_host_net_functions_out, out, status);
    if (status) {
        LOG_E("Query host net functions returned with status 0x%x, syndrome = 0x%x",
            status, MLX5_GET(query_host_net_functions_out, out, syndrome));
        return DEVX_FAILED;
    }

    /* Extract the host params field from the command output. */
    void *out_host_params = MLX5_ADDR_OF(query_host_net_functions_out, out, host_params_context);

    if (MLX5_GET(host_params, out_host_params, host_pf_disabled) == 1) {
        LOG_E("Host PF is disabled");
        return DEVX_FAILED;
    }

    if (MLX5_GET(host_params, out_host_params, host_pf_not_exist) == 1) {
        LOG_E("Host PF doesn't exist for this context");
        return DEVX_FAILED;
    }

    if (MLX5_GET(host_params, out_host_params, host_pf_vhca_id_valid) == 0) {
        LOG_E("Invalid PF vhca id");
        return DEVX_FAILED;
    }
    funcs->host_number = (uint8_t)MLX5_GET(host_params, out_host_params, host_number);
    funcs->vf_info_valid = (bool)MLX5_GET(host_params, out_host_params, host_pci_vf_info_valid);
    funcs->total_vf_cnt = (uint16_t)MLX5_GET(host_params, out_host_params, host_total_vfs);
    funcs->vf_cnt = (uint16_t)MLX5_GET(host_params, out_host_params, host_num_vfs);
    funcs->pf_vhca_id = (uint16_t)MLX5_GET(host_params, out_host_params, host_pf_vhca_id);
    funcs->pci_bus = (uint16_t)MLX5_GET(host_params, out_host_params, host_pci_bus);
    funcs->pci_device = (uint16_t)MLX5_GET(host_params, out_host_params, host_pci_device);
    funcs->pci_function = (uint16_t)MLX5_GET(host_params, out_host_params, host_pci_function);
    funcs->vf_stride = (uint16_t)MLX5_GET(host_params, out_host_params, host_pci_vf_stride);
    funcs->first_vf_offset = (uint16_t)MLX5_GET(host_params, out_host_params, host_pci_first_vf_offset);

    return DEVX_OK;
}

int devx_query_hca_caps_ex(struct ibv_context *context, struct devx_hca_capabilities *caps, bool other_function, uint16_t function_id) {
    uint32_t in[MLX5_ST_SZ_DW(query_hca_cap_in)] = { 0 };
    uint32_t out[MLX5_ST_SZ_DW(query_hca_cap_out)] = { 0 };
    void *out_caps = MLX5_ADDR_OF(query_hca_cap_out, out, capability);
    size_t log_bf_reg_size;

    /* Set operation code and operation mode of the command. */
    MLX5_SET(query_hca_cap_in, in, opcode, MLX5_CMD_OP_QUERY_HCA_CAP);
    MLX5_SET(query_hca_cap_in, in, op_mod,
        PRIV_DOCA_MLX5_HCA_CAP_OP_MOD_GENERAL_DEVICE | PRIV_DOCA_MLX5_HCA_CAP_OPMOD_GET_CUR);
    MLX5_SET(query_hca_cap_in, in, other_function, other_function);
    MLX5_SET(query_hca_cap_in, in, function_id, function_id);

    /* Send the command to FW using DevX. */
    int ret = mlx5dv_devx_general_cmd(context, in, sizeof(in), out, sizeof(out));
    if (ret) {
        LOG_E("Failed to query HCA capabilities");
        return DEVX_FAILED;
    }

    /* Inspect the returned status of the command and print the syndrome in case of error. */
    if (MLX5_GET(query_hca_cap_out, out, status) != 0) {
        LOG_E("Query devx HCA capabilities failed, syndrome = 0x%x",
            MLX5_GET(query_hca_cap_out, out, syndrome));
        return DEVX_FAILED;
    }

    /* Translate the capabilities from PRM format to c structure. */
    caps->vhca_id = MLX5_GET(cmd_hca_cap, out_caps, vhca_id);
    caps->vhca_resource_manager = MLX5_GET(cmd_hca_cap, out_caps, vhca_resource_manager);
    caps->hca_cap_2 = MLX5_GET(cmd_hca_cap, out_caps, hca_cap_2);
    caps->emulation_manager_virtio_net = MLX5_GET(cmd_hca_cap, out_caps, virtio_net_device_emulation_manager);
    caps->emulation_manager_virtio_blk = MLX5_GET(cmd_hca_cap, out_caps, virtio_blk_device_emulation_manager);
    caps->emulation_manager_nvme = MLX5_GET(cmd_hca_cap, out_caps, nvme_device_emulation_manager);
    caps->emulation_manager_virtio_fs = MLX5_GET(cmd_hca_cap, out_caps, virtio_fs_device_emulation_manager);
    caps->hotplug_manager = MLX5_GET(cmd_hca_cap, out_caps, hotplug_manager);
    caps->eswitch_manager = MLX5_GET(cmd_hca_cap, out_caps, eswitch_manager);
    caps->general_object_types_supported = MLX5_GET64(cmd_hca_cap, out_caps, general_obj_types_63_0);
    caps->ib_port_type = !(MLX5_GET(cmd_hca_cap, out_caps, port_type));
    caps->crossing_vhca_mkey_supported = MLX5_GET(cmd_hca_cap, out_caps, crossing_vhca_mkey);
    caps->num_ports = MLX5_GET(cmd_hca_cap, out_caps, num_ports);
    caps->native_port_num = MLX5_GET(cmd_hca_cap, out_caps, native_port_num);
    caps->wod_supported = MLX5_GET(cmd_hca_cap, out_caps, wait_on_data);
    log_bf_reg_size = MLX5_GET(cmd_hca_cap, out_caps, log_bf_reg_size);
    caps->uar_reg_size = 1LLU << log_bf_reg_size;

    if (caps->hca_cap_2) {
        memset(in, 0, sizeof(in));
        memset(out, 0, sizeof(out));

        /* Set operation code and operation mode of the command. */
        MLX5_SET(query_hca_cap_in, in, opcode, MLX5_CMD_OP_QUERY_HCA_CAP);
        MLX5_SET(query_hca_cap_in, in, op_mod,
            PRIV_DOCA_MLX5_HCA_CAP_OP_MOD_GENERAL_DEVICE_CAP_2 | PRIV_DOCA_MLX5_HCA_CAP_OPMOD_GET_CUR);
        MLX5_SET(query_hca_cap_in, in, other_function, other_function);
        MLX5_SET(query_hca_cap_in, in, function_id, function_id);

        /* Send the command to FW using DevX. */
        int ret = mlx5dv_devx_general_cmd(context, in, sizeof(in), out, sizeof(out));
        if (ret) {
            LOG_E("Failed to query HCA capabilities");
            return DEVX_FAILED;
        }

        /* Inspect the returned status of the command and print the syndrome in case of error. */
        if (MLX5_GET(query_hca_cap_out, out, status) != 0) {
            LOG_E("Query devx HCA capabilities2 failed, syndrome = 0x%x",
                MLX5_GET(query_hca_cap_out, out, syndrome));
            return DEVX_FAILED;
        }

        caps->cross_gvmi_mkey_enabled =
            (MLX5_GET(cmd_hca_cap_2, out_caps, cross_vhca_object_to_object_supported) &
                LOCAL_MKEY_TO_REMOTE_MKEY_BIT) != 0;
        caps->introspection_mkey_access_allowed =
            MLX5_GET(cmd_hca_cap_2, out_caps, introspection_mkey_access_allowed);
        caps->introspection_mkey = MLX5_GET(cmd_hca_cap_2, out_caps, introspection_mkey);

    }

    return DEVX_OK;
}

int devx_query_hca_caps(struct ibv_context *context, struct devx_hca_capabilities *caps) {
    return devx_query_hca_caps_ex(context, caps, false, 0);
}

int devx_query_hca_vuid(struct ibv_context *context, uint16_t vhca_id, struct devx_vuid *vuid) {
    uint32_t in[MLX5_ST_SZ_DW(query_vuid_in)] = { 0 };
    uint32_t out[MLX5_ST_SZ_BYTES(query_vuid_out) + (MLX5_ST_SZ_BYTES(array1024_auto))] = { 0 };
    MLX5_SET(query_vuid_in, in, opcode, MLX5_CMD_OP_QUERY_VUID);
    MLX5_SET(query_vuid_in, in, query_vfs_vuid, 1);
    MLX5_SET(query_vuid_in, in, vhca_id, vhca_id);
    /* Send the command to FW using DevX. */
    int ret = mlx5dv_devx_general_cmd(context, in, sizeof(in), out, sizeof(out));
    if (ret) {
        LOG_E("Failed to query vuid, vhca_id=%u", vhca_id);
        return DEVX_FAILED;
    }
    /* Inspect the returned status of the command and print the syndrome in case of error. */
    int status = MLX5_GET(query_vuid_out, out, status);
    if (status) {
        LOG_E("Query DevX VUID returned with status 0x%x", status);
        return DEVX_FAILED;
    }
    uint16_t nb_out_vuids = MLX5_GET(query_vuid_out, out, num_of_entries);
    if (nb_out_vuids != 1) {
        LOG_E("Query DevX VUID returned with nb_out_vuids %u", nb_out_vuids);
        return DEVX_FAILED;
    }
    memcpy(vuid, MLX5_ADDR_OF(query_vuid_out, out, vuid), DEVX_VUID_BYTES);
    return DEVX_OK;
}

int devx_query_emulated_functions_info(struct ibv_context *context,
    uint16_t op_mod,
    uint16_t pf_op_mod,
    uint16_t pf_vhca_id,
    struct devx_emulated_devinfo *devinfos,
    uint16_t nb_max_functions) {

    uint32_t in[MLX5_ST_SZ_DW(query_emulated_functions_info_in)] = { 0 };
    uint16_t nb_emulated_fns = 0;
    size_t out_len = MLX5_ST_SZ_BYTES(query_emulated_functions_info_out);
    out_len += MLX5_ST_SZ_BYTES(emulated_function_info) * nb_max_functions;
    uint8_t *out = reinterpret_cast<uint8_t *>(malloc(out_len));
    memset(out, 0, out_len);
    MLX5_SET(query_emulated_functions_info_in, in, opcode, MLX5_CMD_OP_QUERY_EMULATED_FUNCTIONS_INFO);
    MLX5_SET(query_emulated_functions_info_in, in, op_mod, op_mod);
    MLX5_SET(query_emulated_functions_info_in, in, pf_vhca_id, pf_vhca_id);

    /* Send the command to FW using DevX. */
    int ret = mlx5dv_devx_general_cmd(context, in, sizeof(in), out, out_len);
    if (ret) {
        LOG_E("Failed to query emulated functions info");
        return DEVX_FAILED;
    }
    nb_emulated_fns = MLX5_GET(query_emulated_functions_info_out, out, num_emulated_functions);
    if (nb_emulated_fns > nb_max_functions) {
        LOG_E("Query DevX emulated functions found more devices than requested, found %u, max %u", nb_emulated_fns, nb_max_functions);
        return DEVX_FAILED;
    }
    size_t emulated_info_size_bytes = MLX5_ST_SZ_BYTES(emulated_function_info);
    uint8_t *emulated_fn_info_out = (uint8_t *)MLX5_ADDR_OF(query_emulated_functions_info_out, out, emulated_function_info);
    uint16_t fn_idx;
    for (fn_idx = 0; fn_idx < nb_emulated_fns; fn_idx++) {
        devinfos[fn_idx].vhca_id = MLX5_GET(emulated_function_info, emulated_fn_info_out, vhca_id);
        devinfos[fn_idx].pci_bdf = MLX5_GET(emulated_function_info, emulated_fn_info_out, pci_bdf);
        devinfos[fn_idx].hotplug = MLX5_GET(emulated_function_info, emulated_fn_info_out, hotplug_function);
        devinfos[fn_idx].op_mod = pf_op_mod;
        devinfos[fn_idx].vf_exist = MLX5_GET(emulated_function_info, emulated_fn_info_out, vf_exist);
        devinfos[fn_idx].is_vf = op_mod == MLX5_QUERY_EMULATED_FUNCTIONS_INFO_IN_OP_MOD_VIRTUAL_FUNCTIONS;
        devinfos[fn_idx].nb_total_vfs = 0;
        if (MLX5_GET(emulated_function_info, emulated_fn_info_out, max_num_vfs_valid)) {
            devinfos[fn_idx].nb_total_vfs =
                MLX5_GET(emulated_function_info, emulated_fn_info_out, max_num_vfs);
        }
        emulated_fn_info_out += emulated_info_size_bytes;
    }

    return nb_emulated_fns;
}

