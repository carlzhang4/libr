#include "libr.hpp"


struct ibv_device *ctx_find_dev(char const *ib_devname) {
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
	LOG_I("%-20s : %s", "IB_DEV_NAME", ibv_get_device_name(ib_dev));
	return ib_dev;
}

struct ibv_context *ctx_open_device(struct ibv_device *ib_dev) {
	struct ibv_context *context;
	context = ibv_open_device(ib_dev);
	assert(context != NULL);
	return context;
}

const char *transport_type_str(enum ibv_transport_type t) {
	switch (t) {
	case IBV_TRANSPORT_UNKNOWN: return "IBV_TRANSPORT_UNKNOWN";
	case IBV_TRANSPORT_IB: return "IBV_TRANSPORT_IB";
	case IBV_TRANSPORT_IWARP: return "IBV_TRANSPORT_IWARP";
	case IBV_TRANSPORT_USNIC: return "IBV_TRANSPORT_USNIC";
	case IBV_TRANSPORT_USNIC_UDP: return "IBV_TRANSPORT_USNIC_UDP";
	case IBV_TRANSPORT_UNSPECIFIED: return "IBV_TRANSPORT_UNSPECIFIED";
	}
	return "Unknow TransportType";
}

const char *link_layer_str(int8_t link_layer) {
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

int get_cache_line_size() {
	int size = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
	if (size == 0) {
		char const *file_name = "/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size";
		FILE *fp;
		char line[10];
		fp = fopen(file_name, "r");
		if (fp == NULL) {
			return 64;//byte
		}
		if (fgets(line, 10, fp) != NULL) {
			size = atoi(line);
			fclose(fp);
		}
	}
	if (size <= 0)
		size = 64;

	return size;
}

void init_net_param(NetParam &net_param) {
	if (net_param.nodeId == 0) {
		net_param.sockfd = new int[net_param.numNodes];//zero is left unused
	} else {
		net_param.sockfd = new int;
	}

	net_param.ib_port = 1;//minimum 1
	net_param.page_size = sysconf(_SC_PAGESIZE);
	net_param.cacheline_size = get_cache_line_size();

	LOG_I("%-20s : %d", "NetParam.numNodes", net_param.numNodes);
	LOG_I("%-20s : %d", "NetParam.nodeId", net_param.nodeId);
	LOG_I("%-20s : %s", "NetParam.serverIp", net_param.serverIp.c_str());
}

void get_opt(NetParam &net_param, int argc, char *argv[]) {
	int opt;
	const char *optstring = "n:i:s:";
	int total_set = 0;
	while ((opt = getopt(argc, argv, optstring)) != -1) {
		switch (opt) {
		case 'n':
			net_param.numNodes = stoi(optarg);
			total_set++;
			break;
		case 'i':
			net_param.nodeId = stoi(optarg);
			total_set++;
			break;
		case 's':
			net_param.serverIp = string(optarg);
			total_set++;
			break;
		default:
			LOG_E("Unknow parameter");
		}
	}
	assert(total_set == 3);
	if (net_param.nodeId == 0) {
		net_param.sockfd = new int[net_param.numNodes];//zero is left unused
	} else {
		net_param.sockfd = new int;
	}

	net_param.ib_port = 1;//minimum 1
	net_param.gid_index = 3;//minimum 1, 2 is v1
	net_param.page_size = sysconf(_SC_PAGESIZE);
	net_param.cacheline_size = get_cache_line_size();

	LOG_I("nodeId:%d numNodes:%d", net_param.nodeId, net_param.numNodes);
}

void roce_init(NetParam &net_param, int num_contexts) {
	net_param.num_contexts = num_contexts;
	ALLOCATE(net_param.contexts, struct ibv_context *, num_contexts);
	struct ibv_device *ib_dev = ctx_find_dev(net_param.device_name.c_str());
	for (int i = 0;i < num_contexts;i++) {
		net_param.contexts[i] = ctx_open_device(ib_dev);
	}
	struct ibv_context *context = net_param.contexts[0];

	//check link
	LOG_I("%-20s : %s", "Transport type", transport_type_str(context->device->transport_type));
	struct ibv_port_attr port_attr;
	assert(ibv_query_port(context, net_param.ib_port, &port_attr) == 0);
	LOG_I("%-20s : %s", "Line Layer", link_layer_str(port_attr.link_layer));
	assert(port_attr.state == IBV_PORT_ACTIVE);
	struct ibv_device_attr attr;
	assert(ibv_query_device(context, &attr) == 0);
	LOG_I("%-20s : %d", "Max Outreads", attr.max_qp_rd_atom);
	LOG_I("%-20s : %d", "Max Pkeys", attr.max_pkeys);
	LOG_I("%-20s : %d", "Atomic Capacity", attr.atomic_cap);

	//set mtu
	assert((port_attr.active_mtu >= IBV_MTU_256 && port_attr.active_mtu <= IBV_MTU_4096));
	net_param.cur_mtu = port_attr.active_mtu;
	LOG_I("%-20s : %d", "CUR MTU", 128 << (net_param.cur_mtu));

	srand48(getpid() * time(NULL));
}

QpHandler *create_qp_rc(NetParam &net_param, void *buf, size_t size, struct PingPongInfo *info) {
	static int context_index = 0;
	assert(context_index < net_param.num_contexts);
	QpHandler *qp_handler;
	ALLOCATE(qp_handler, QpHandler, 1);
	int max_out_reads = 1;
	int tx_depth = 128;
	int rx_depth = 512;
	uint32_t max_inline_size = 0;

	int num_wrs = net_param.batch_size != 0 ? net_param.batch_size : 1;
	int num_sges = num_wrs * 1;
	struct ibv_sge *send_sge_list;
	struct ibv_sge *recv_sge_list;
	struct ibv_send_wr *send_wr;
	struct ibv_recv_wr *recv_wr;

	struct ibv_pd *pd;
	struct ibv_mr *mr;
	struct ibv_cq *send_cq;
	struct ibv_cq *recv_cq;
	struct ibv_comp_channel *channel = NULL;
	struct ibv_qp *qp;

	int flags = IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_REMOTE_READ;//send/recv/read/write 


	ALLOCATE(send_sge_list, struct ibv_sge, num_sges);
	ALLOCATE(recv_sge_list, struct ibv_sge, num_sges);
	ALLOCATE(send_wr, struct ibv_send_wr, num_wrs);
	ALLOCATE(recv_wr, struct ibv_recv_wr, num_wrs);

	//check valid mem
	assert(size > static_cast<size_t>(net_param.page_size));
	assert((reinterpret_cast<size_t>(buf)) % net_param.page_size == 0);

	//create pd/mr/scq/rcq
	assert(pd = ibv_alloc_pd(net_param.contexts[context_index]));
	assert(mr = ibv_reg_mr(pd, buf, size, flags));
	assert(send_cq = ibv_create_cq(net_param.contexts[context_index], tx_depth, NULL, channel, 0));
	assert(recv_cq = ibv_create_cq(net_param.contexts[context_index], rx_depth, NULL, channel, 0));

	//create qp
	struct ibv_qp_init_attr attr;
	memset(&attr, 0, sizeof(struct ibv_qp_init_attr));
	attr.send_cq = send_cq;
	attr.recv_cq = recv_cq;
	attr.cap.max_inline_data = max_inline_size;
	attr.cap.max_send_wr = tx_depth;
	attr.cap.max_send_sge = num_sges;
	attr.cap.max_recv_wr = rx_depth;
	attr.cap.max_recv_sge = num_sges;
	attr.qp_type = IBV_QPT_RC;
	qp = ibv_create_qp(pd, &attr);
	if (qp == NULL && errno == ENOMEM) {
		fprintf(stderr, "Requested QP size might be too big. Try reducing TX depth and/or inline size.\n");
		fprintf(stderr, "Current TX depth is %d and inline size is %d .\n", tx_depth, max_inline_size);
	}
	assert(max_inline_size <= attr.cap.max_inline_data);

	//modify qp to init
	struct ibv_qp_attr attr_qp;
	memset(&attr_qp, 0, sizeof(struct ibv_qp_attr));
	flags = IBV_QP_STATE | IBV_QP_PKEY_INDEX | IBV_QP_PORT;
	flags |= IBV_QP_ACCESS_FLAGS;
	attr_qp.qp_state = IBV_QPS_INIT;
	attr_qp.pkey_index = 0;
	attr_qp.port_num = net_param.ib_port;
	attr_qp.qp_access_flags = IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_LOCAL_WRITE;//for send
	assert(ibv_modify_qp(qp, &attr_qp, flags) == 0);

	//setup connection
	union ibv_gid temp_gid;
	struct ibv_port_attr attr_port;
	assert(ibv_query_port(net_param.contexts[context_index], net_param.ib_port, &attr_port) == 0);
	assert(ibv_query_gid(net_param.contexts[context_index], net_param.ib_port, net_param.gid_index, &temp_gid) == 0);
	info->lid = attr_port.lid;//local id, it seems only useful for ib instead of roce
	info->gid_index = net_param.gid_index;
	info->qpn = qp->qp_num;
	info->psn = lrand48() & 0xffffff;
	info->rkey = mr->rkey;
	info->out_reads = max_out_reads;
	info->vaddr = reinterpret_cast<uintptr_t>(buf);
	memcpy(info->gid.raw, temp_gid.raw, 16);

	qp_handler->buf = reinterpret_cast<size_t> (buf);
	qp_handler->send_cq = send_cq;
	qp_handler->recv_cq = recv_cq;
	qp_handler->max_inline_size = max_inline_size;
	qp_handler->qp = qp;
	qp_handler->pd = pd;
	qp_handler->mr = mr;
	qp_handler->send_sge_list = send_sge_list;
	qp_handler->recv_sge_list = recv_sge_list;
	qp_handler->send_wr = send_wr;
	qp_handler->recv_wr = recv_wr;
	qp_handler->num_sges = num_sges;
	qp_handler->num_wrs = num_wrs;
	qp_handler->tx_depth = tx_depth;
	qp_handler->rx_depth = rx_depth;

	context_index++;

	return qp_handler;
}

void init_wr_base_send_recv(QpHandler &qp_handler) {
	//send
	assert(qp_handler.num_wrs == qp_handler.num_sges);
	memset(qp_handler.send_wr, 0, sizeof(struct ibv_send_wr) * qp_handler.num_wrs);
	ibv_send_wr *send_wr = qp_handler.send_wr;
	ibv_sge *send_sge_list = qp_handler.send_sge_list;

	for (int i = 0;i < qp_handler.num_wrs;i++) {
		send_sge_list[i].addr = qp_handler.buf;
		send_sge_list[i].lkey = qp_handler.mr->lkey;
		send_wr[i].sg_list = send_sge_list + i;
		send_wr[i].num_sge = 1;
		send_wr[i].wr_id = 1000;//todo
		send_wr[i].next = NULL;
		send_wr[i].send_flags = IBV_SEND_SIGNALED;
		send_wr[i].opcode = IBV_WR_SEND;
		if (i > 0) {
			send_wr[i - 1].next = &send_wr[i];
		}
	}

	//recv
	memset(qp_handler.recv_wr, 0, sizeof(struct ibv_recv_wr) * qp_handler.num_wrs);
	ibv_recv_wr *recv_wr = qp_handler.recv_wr;
	ibv_sge *recv_sge_list = qp_handler.recv_sge_list;

	for (int i = 0;i < qp_handler.num_wrs;i++) {
		recv_sge_list[i].addr = qp_handler.buf;
		recv_sge_list[i].lkey = qp_handler.mr->lkey;
		recv_wr[i].sg_list = recv_sge_list + i;
		recv_wr[i].num_sge = 1;
		recv_wr[i].wr_id = 1001;//todo
		recv_wr[i].next = NULL;//todo
		if (i > 0) {
			recv_wr[i - 1].next = &recv_wr[i];
		}
	}

}

void init_wr_base_write(QpHandler &qp_handler) {
	//write
	assert(qp_handler.num_wrs == qp_handler.num_sges);
	memset(qp_handler.send_wr, 0, sizeof(struct ibv_send_wr) * qp_handler.num_wrs);
	ibv_send_wr *send_wr = qp_handler.send_wr;
	ibv_sge *send_sge_list = qp_handler.send_sge_list;

	for (int i = 0;i < qp_handler.num_wrs;i++) {
		send_sge_list[i].addr = qp_handler.buf;
		send_sge_list[i].lkey = qp_handler.mr->lkey;
		send_wr[i].wr.rdma.remote_addr = qp_handler.remote_buf;
		send_wr[i].wr.rdma.rkey = qp_handler.remote_rkey;

		send_wr[i].sg_list = send_sge_list + i;
		send_wr[i].num_sge = 1;
		send_wr[i].wr_id = 0;//todo
		send_wr[i].next = NULL;
		send_wr[i].send_flags = IBV_SEND_SIGNALED;
		send_wr[i].opcode = IBV_WR_RDMA_WRITE;
	}
}

void connect_qp_rc(NetParam &net_param, QpHandler &qp_handler, struct PingPongInfo *remote_info, struct PingPongInfo *local_info) {
	struct ibv_ah *ah;//todo
	(void)ah;
	struct ibv_qp_attr attr;
	memset(&attr, 0, sizeof attr);
	int flags = IBV_QP_STATE;
	attr.qp_state = IBV_QPS_RTR;
	attr.ah_attr.src_path_bits = 0;
	attr.ah_attr.port_num = net_param.ib_port;
	attr.ah_attr.dlid = remote_info->lid;
	attr.ah_attr.sl = 0;//service level default 0
	attr.ah_attr.is_global = 1;
	attr.ah_attr.grh.dgid = remote_info->gid;
	attr.ah_attr.grh.sgid_index = net_param.gid_index;
	attr.ah_attr.grh.hop_limit = 0xFF;
	attr.ah_attr.grh.traffic_class = 0;

	//UD does not need below code
	attr.path_mtu = net_param.cur_mtu;
	attr.dest_qp_num = remote_info->qpn;
	attr.rq_psn = remote_info->psn;
	flags |= (IBV_QP_AV | IBV_QP_PATH_MTU | IBV_QP_DEST_QPN | IBV_QP_RQ_PSN);

	//only for RC
	attr.max_dest_rd_atomic = remote_info->out_reads;
	attr.min_rnr_timer = MIN_RNR_TIMER;
	flags |= (IBV_QP_MIN_RNR_TIMER | IBV_QP_MAX_DEST_RD_ATOMIC);

	//modify qp to rtr
	assert(ibv_modify_qp(qp_handler.qp, &attr, flags) == 0);
	LOG_D("Connected success, local QPN:%#06x, remote QPN:%#08x", local_info->qpn, remote_info->qpn);

	{
		//modify qp to rts
		flags = IBV_QP_STATE;
		struct ibv_qp_attr *_attr = &attr;
		_attr->qp_state = IBV_QPS_RTS;
		flags |= IBV_QP_SQ_PSN;
		_attr->sq_psn = local_info->psn;

		//only for RC
		_attr->timeout = DEF_QP_TIME;
		_attr->retry_cnt = 7;
		_attr->rnr_retry = 7;
		_attr->max_rd_atomic = remote_info->out_reads;
		flags |= (IBV_QP_TIMEOUT | IBV_QP_RETRY_CNT | IBV_QP_RNR_RETRY | IBV_QP_MAX_QP_RD_ATOMIC);
		assert(ibv_modify_qp(qp_handler.qp, _attr, flags) == 0);
	}
	ah = ibv_create_ah(qp_handler.pd, &(attr.ah_attr));

	qp_handler.remote_buf = remote_info->vaddr;
	qp_handler.remote_rkey = remote_info->rkey;

	init_wr_base_send_recv(qp_handler);
}

void print_pingpong_info(struct PingPongInfo *info) {
	uint16_t dlid = info->lid;
	LOG_I(INFO_FMT, dlid, info->qpn, info->psn,
		info->rkey, info->vaddr,
		"GID",
		info->gid.raw[0], info->gid.raw[1],
		info->gid.raw[2], info->gid.raw[3],
		info->gid.raw[4], info->gid.raw[5],
		info->gid.raw[6], info->gid.raw[7],
		info->gid.raw[8], info->gid.raw[9],
		info->gid.raw[10], info->gid.raw[11],
		info->gid.raw[12], info->gid.raw[13],
		info->gid.raw[14], info->gid.raw[15]);
}

void post_send(QpHandler &qp_handler, size_t offset, int length) {
	qp_handler.send_sge_list[0].addr = qp_handler.buf + offset;
	qp_handler.send_sge_list[0].length = length;
	if (length <= qp_handler.max_inline_size) {
		qp_handler.send_wr[0].send_flags |= IBV_SEND_INLINE;
	}
	qp_handler.send_wr->wr_id = offset;
	qp_handler.send_wr->next = NULL;
	// fuck https://github.com/linux-rdma/rdma-core/blob/6cd09097ad2eebde9a7fa3d3bb09a2cea6e3c2d6/providers/rxe/rxe.c#L1665-L1666
	assert(ibv_post_send(qp_handler.qp, &qp_handler.send_wr[0], &qp_handler.send_bar_wr) == 0);
	// qp_handler.send_wr[0].send_flags = IBV_SEND_SIGNALED;
	// qp_handler.send_wr[0].wr_id = 0;
}

void post_send_batch(QpHandler &qp_handler, int batch_size, size_t offset, int length) {
	assert(batch_size <= qp_handler.num_wrs);
	for (int i = 0;i < batch_size;i++) {
		qp_handler.send_sge_list[i].addr = qp_handler.buf + offset;
		qp_handler.send_sge_list[i].length = length;
		if (length <= qp_handler.max_inline_size) {
			qp_handler.send_wr[i].send_flags |= IBV_SEND_INLINE;
		}
		qp_handler.send_wr[i].wr_id = offset;
		qp_handler.send_wr[i].next = NULL;
		if (i > 0) {
			qp_handler.send_wr[i - 1].next = &qp_handler.send_wr[i];
		}
	}
	int res = ibv_post_send(qp_handler.qp, qp_handler.send_wr, &qp_handler.send_bar_wr);
	if (res != 0) {
		LOG_E("post_send error %d", res);
	}
}

void post_recv(QpHandler &qp_handler, size_t offset, int length) {
	qp_handler.recv_sge_list[0].addr = qp_handler.buf + offset;
	qp_handler.recv_sge_list[0].length = length;
	qp_handler.recv_wr->wr_id = offset;
	qp_handler.recv_wr->next = NULL;
	// fuck https://github.com/linux-rdma/rdma-core/blob/6cd09097ad2eebde9a7fa3d3bb09a2cea6e3c2d6/providers/rxe/rxe.c#L1665-L1666
	assert(ibv_post_recv(qp_handler.qp, &qp_handler.recv_wr[0], &qp_handler.recv_bar_wr) == 0);
}

int poll_send_cq(QpHandler &qp_handler, struct ibv_wc *wc) {
	int ne = ibv_poll_cq(qp_handler.send_cq, CTX_POLL_BATCH, wc);//if error, ne < 0
	return ne;
}

int poll_recv_cq(QpHandler &qp_handler, struct ibv_wc *wc) {
	int ne = ibv_poll_cq(qp_handler.recv_cq, CTX_POLL_BATCH, wc);
	return ne;
}