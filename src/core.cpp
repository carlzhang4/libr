#include "core.hpp"

struct ibv_device* ctx_find_dev(char const * ib_devname){
	int num_of_device;
	struct ibv_device **dev_list;
	struct ibv_device *ib_dev = NULL;

	dev_list = ibv_get_device_list(&num_of_device);

	assert(num_of_device > 0);

	if (!ib_devname) {
		ib_dev = dev_list[0];
		assert(ib_dev != NULL);
	} else {
		for (; (ib_dev = *dev_list); ++dev_list)
			if (!strcmp(ibv_get_device_name(ib_dev), ib_devname))
				break;
		assert(ib_dev != NULL);
	}
	printf("IB_DEV_NAME     : %s\n",ibv_get_device_name(ib_dev));
	return ib_dev;
}

struct ibv_context* ctx_open_device(struct ibv_device *ib_dev){
	struct ibv_context *context;
	context = ibv_open_device(ib_dev);
	assert(context != NULL);
	return context;
}

void check_link(struct ibv_context *context,struct perftest_parameters *user_param){
	printf("Transport Type  : %s\n",str_transport_type(context->device->transport_type));
	struct ibv_port_attr port_attr;
	assert(ibv_query_port(context, user_param->ib_port, &port_attr) == 0);
	printf("Link Layer      : %s\n",link_layer_str(port_attr.link_layer));
	assert(strcmp("Unknown", link_layer_str(port_attr.link_layer)) != 0);

	assert(port_attr.state == IBV_PORT_ACTIVE);

	if(port_attr.link_layer == IBV_LINK_LAYER_ETHERNET && user_param->gid_index == -1){
		user_param->gid_index = 0;
	}

	struct ibv_device_attr attr;
	assert(ibv_query_device(context,&attr) == 0);
	printf("Max Outreads    : %d, current 1\n",attr.max_qp_rd_atom);
	printf("Max Pkeys       : %d, current 0\n",attr.max_pkeys);
}

const char* str_transport_type(enum ibv_transport_type t){
	switch (t){
		case IBV_TRANSPORT_UNKNOWN : return "IBV_TRANSPORT_UNKNOWN";
		case IBV_TRANSPORT_IB : return "IBV_TRANSPORT_IB";
		case IBV_TRANSPORT_IWARP : return "IBV_TRANSPORT_IWARP";
		case IBV_TRANSPORT_USNIC : return "IBV_TRANSPORT_USNIC";
		case IBV_TRANSPORT_USNIC_UDP : return "IBV_TRANSPORT_USNIC_UDP";
		case IBV_TRANSPORT_UNSPECIFIED : return "IBV_TRANSPORT_UNSPECIFIED";
	}
}

const char *link_layer_str(int8_t link_layer){
	switch (link_layer) {
		case IBV_LINK_LAYER_UNSPECIFIED:
		case IBV_LINK_LAYER_INFINIBAND:
			return "IB";
		case IBV_LINK_LAYER_ETHERNET:
			return "Ethernet";
		default:
			return "Unknown";
	}
}

int get_cache_line_size(){
	int size = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
	if (size == 0) {
		char const* file_name = "/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size";
		FILE *fp;
		char line[10];
		fp = fopen(file_name, "r");
		if (fp == NULL) {
			return 64;//byte
		}
		if(fgets(line,10,fp) != NULL) {
			size = atoi(line);
			fclose(fp);
		}
	}
	if (size <= 0)
		size = 64;

	return size;
}

void ctx_wait_event(struct ibv_comp_channel *channel){
	struct ibv_cq       *ev_cq;
	void                *ev_ctx;
	assert(ibv_get_cq_event(channel,&ev_cq,&ev_ctx) == 0);
	ibv_ack_cq_events(ev_cq,1);
	assert(ibv_req_notify_cq(ev_cq, 0) == 0);
}