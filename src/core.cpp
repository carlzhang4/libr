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
	return "Unknow TransportType";
}

const char *link_layer_str(int8_t link_layer){
	switch (link_layer) {
		case IBV_LINK_LAYER_UNSPECIFIED:
		case IBV_LINK_LAYER_INFINIBAND:
			return "IB";
		case IBV_LINK_LAYER_ETHERNET:
			return "Ethernet";
		default:
			LOG_E("Unkonwn link layer");
			return "Unknown";
	}
}

const char *verb_str(VerbType verb){
	switch (verb) {
		case WRITE: 	return "WRITE";
		case READ:  	return "READ";
		case SEND:  	return "SEND";
		case ATOMIC:  	return "ATOMIC";
		default:   		return "Unknown";
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

void roce_init(UserParam &user_param){
	struct ibv_device* ib_dev = ctx_find_dev("mlx5_0");
	struct ibv_context* context = ctx_open_device(ib_dev);
	user_param.context = context;

	//check link
	LOG_I("Transport type : %s",str_transport_type(context->device->transport_type));
	struct ibv_port_attr port_attr;
	assert(ibv_query_port(context, user_param.ib_port, &port_attr) == 0);
	LOG_I("Line Layer : %s",link_layer_str(port_attr.link_layer));
	assert(port_attr.state == IBV_PORT_ACTIVE);
	struct ibv_device_attr attr;
	assert(ibv_query_device(context,&attr) == 0);
	LOG_I("Max Outreads : %d",attr.max_qp_rd_atom);
	LOG_I("Max Pkeys : %d",attr.max_pkeys);
	LOG_I(Atomic Capacity : %d,attr.atomic_cap);

	//set mtu
	assert((port_attr.active_mtu >= IBV_MTU_256 && port_attr.active_mtu <= IBV_MTU_4096));
	user_param.cur_mtu = port_attr.active_mtu;
}

void create_qp_rc(UserParam& user_param, void* buf, size_t size, struct PingPongInfo *info, QpHandler &qp_handler){
	int max_out_reads = 1;
	int tx_depth = 128;
	int rx_depth = 512;
	int max_inline_size = 1024;
	int max_send_sge = 16;
	int max_recv_sge = 16;

	int num_wr = 1;
	int num_sge = num_wr * 1;
	struct ibv_sge* send_sge_list;
	struct ibv_sge* recv_sge_list;
	struct ibv_send_wr* send_wr;
	struct ibv_recv_wr* recv_wr;

	struct ibv_pd* pd;
	struct ibv_mr* mr;
	struct ibv_cq* send_cq;
	struct ibv_cq* recv_cq;
	struct ibv_comp_channel* channel = NULL;
	struct ibv_qp* qp;

	int flags = IBV_ACCESS_LOCAL_WRITE;//sufficient for send/recv 


	ALLOCATE(send_sge_list, struct ibv_sge, num_sge);
	ALLOCATE(recv_sge_list, struct ibv_sge, num_sge);
	ALLOCATE(send_wr, struct ibv_send_wr, num_wr);
	ALLOCATE(recv_wr, struct ibv_recv_wr, num_wr);

	//check valid mem
	assert(size > user_param.page_size);
	assert(((size_t)buf)%user_param.page_size == 0);
	
	//create pd/mr/scq/rcq
	assert(pd = ibv_alloc_pd(user_param.context));
	assert(mr = ibv_reg_mr(pd, buf, size, flags));
	assert(send_cq = ibv_create_cq(user_param.context, tx_depth, NULL, channel, 0));
	assert(recv_cq = ibv_create_cq(user_param.context, rx_depth, NULL,channel, 0));
	
	//create qp
	struct ibv_qp_init_attr attr;
	memset(&attr, 0, sizeof(struct ibv_qp_init_attr));
	attr.send_cq = send_cq;
	attr.recv_cq = recv_cq;
	attr.cap.max_inline_data = max_inline_size;
	attr.cap.max_send_wr = tx_depth;
	attr.cap.max_send_sge = max_send_sge;
	attr.cap.max_recv_wr = rx_depth;
	attr.cap.max_recv_sge = max_recv_sge;
	attr.qp_type = IBV_QPT_RC;
	qp = ibv_create_qp(pd, &attr);
	if(qp == NULL && errno == ENOMEM){
		fprintf(stderr, "Requested QP size might be too big. Try reducing TX depth and/or inline size.\n");
		fprintf(stderr, "Current TX depth is %d and inline size is %d .\n", tx_depth, max_inline_size);
	}
	assert(max_inline_size <= attr.cap.max_inline_data);

	//modify qp to init
	struct ibv_qp_attr attr_qp;
	memset(&attr_qp, 0, sizeof(struct ibv_qp_attr));
	flags = IBV_QP_STATE | IBV_QP_PKEY_INDEX | IBV_QP_PORT;
	flags |= IBV_QP_ACCESS_FLAGS;
	attr_qp.qp_state		= IBV_QPS_INIT;
	attr_qp.pkey_index		= 0;
	attr_qp.port_num 		= user_param.ib_port;
	attr_qp.qp_access_flags = IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_LOCAL_WRITE;//for send
	assert(ibv_modify_qp(qp, &attr_qp, flags) == 0);

	//setup connection
	union ibv_gid temp_gid;
	struct ibv_port_attr attr_port;
	srand48(getpid() * time(NULL));
	assert(ibv_query_port(user_param.context, user_param.ib_port, &attr_port) == 0);
	assert(ibv_query_gid(user_param.context, user_param.ib_port, user_param.gid_index, &temp_gid)==0);
	info->lid = attr_port.lid;//local id, it seems only useful for ib instead of roce
	info->gid_index = user_param.gid_index;
	info->qpn = qp->qp_num;
	info->psn = lrand48() & 0xffffff;
	info->rkey = mr->rkey;
	info->out_reads = max_out_reads;
	info->vaddr = (uintptr_t)buf;
	memcpy(info->gid.raw, temp_gid.raw, 16);

	qp_handler.qp = qp;
	qp_handler.pd = pd;
}

void connect_qp_rc(UserParam &user_param, QpHandler &qp_handler, struct PingPongInfo *info, struct PingPongInfo *my_info){
	struct ibv_ah* ah;//todo
	struct ibv_qp_attr attr;
	memset(&attr, 0, sizeof attr);
	int flags = IBV_QP_STATE;
	attr.qp_state = IBV_QPS_RTR;
	attr.ah_attr.src_path_bits = 0;
	attr.ah_attr.port_num = user_param.ib_port;
	attr.ah_attr.dlid = info->lid;
	attr.ah_attr.sl = 0;//service level default 0
	attr.ah_attr.is_global = 1;
	attr.ah_attr.grh.dgid = info->gid;
	attr.ah_attr.grh.sgid_index = user_param.gid_index;
	attr.ah_attr.grh.hop_limit = 0xFF;
	attr.ah_attr.grh.traffic_class = 0;

	//UD does not need below code
	attr.path_mtu = user_param.cur_mtu;
	attr.dest_qp_num = info->qpn;
	attr.rq_psn = info->psn;
	flags |= (IBV_QP_AV | IBV_QP_PATH_MTU | IBV_QP_DEST_QPN | IBV_QP_RQ_PSN);

	//only for RC
	attr.max_dest_rd_atomic = info->out_reads;
	attr.min_rnr_timer = MIN_RNR_TIMER;
	flags |= (IBV_QP_MIN_RNR_TIMER | IBV_QP_MAX_DEST_RD_ATOMIC);

	//modify qp to rtr
	assert(ibv_modify_qp(qp_handler.qp, &attr, flags) == 0);

	{
		//modify qp to rts
		flags = IBV_QP_STATE;
		struct ibv_qp_attr *_attr = &attr;
		_attr->qp_state = IBV_QPS_RTS;
		flags |= IBV_QP_SQ_PSN;
		_attr->sq_psn = my_info->psn;

		//only for RC
		_attr->timeout = DEF_QP_TIME;
		_attr->retry_cnt = 7;
		_attr->rnr_retry = 7;
		_attr->max_rd_atomic  = info->out_reads;
		flags |= (IBV_QP_TIMEOUT | IBV_QP_RETRY_CNT | IBV_QP_RNR_RETRY | IBV_QP_MAX_QP_RD_ATOMIC);
		assert(ibv_modify_qp(qp_handler.qp,_attr,flags) == 0);
	}
	ah = ibv_create_ah(qp_handler.pd,&(attr.ah_attr));


}