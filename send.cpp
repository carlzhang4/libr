#include <iostream>
#include "src/core.hpp"
#include "src/socket.hpp"
using namespace std;
int main(int argc, char *argv[]){
	struct ibv_device			*ib_dev = NULL;
	struct pingpong_context  	ctx;
	struct pingpong_dest	 	my_dest;
	struct pingpong_dest		rem_dest;
	struct perftest_parameters	user_param;

	memset(&ctx, 0,sizeof(struct pingpong_context));
	memset(&user_param, 0 , sizeof(struct perftest_parameters));
	if(argc == 2){
		user_param.machine = CLIENT;
		user_param.servername = argv[1];
		printf("servername : %s\n",user_param.servername);
	}else{
		user_param.machine = SERVER;
	}

	ctx.page_size				= sysconf(_SC_PAGESIZE);
	ctx.cache_line_size			= get_cache_line_size();

	user_param.ib_port 			= 1; //minimum 1
	user_param.gid_index		= -1;
	user_param.pkey_index		= 0;
	user_param.inline_size		= 0; //todo
	user_param.out_reads		= 1;
	user_param.port				= 18000;
	user_param.iters			= 10;
	user_param.connection_type	= RC;
	user_param.post_list		= 1;
	user_param.recv_post_list	= 1;
	user_param.size				= 4096;
	user_param.tx_depth			= 128;
	user_param.rx_depth			= 512;
	user_param.verb    			= SEND;
	user_param.use_event		= 0;

	assert(user_param.size % ctx.cache_line_size == 0);

	ib_dev = ctx_find_dev("mlx5_0");
	ctx.context = ctx_open_device(ib_dev);
	check_link(ctx.context,&user_param);
	socket_connect(&user_param);

	printf("Pagesize        : %d\n",ctx.page_size);
	printf("Cacheline Size  : %d\n",ctx.cache_line_size);

	user_param.curr_mtu = set_mtu(ctx.context,user_param.ib_port,0);
	printf("MTU             : %d\n",128 << (user_param.curr_mtu));

	memset(&my_dest, 0, sizeof(struct pingpong_dest)*1);
	memset(&rem_dest, 0, sizeof(struct pingpong_dest)*1);

	{/* alloc context */
		if(user_param.machine == CLIENT){
			ALLOCATE(ctx.sge_list, struct ibv_sge, user_param.post_list);
			ALLOCATE(ctx.wr, struct ibv_send_wr, user_param.post_list);
		}

		if(user_param.machine==SERVER){ //only send/recv needs
			ALLOCATE(ctx.recv_sge_list, struct ibv_sge,  user_param.recv_post_list);
			ALLOCATE(ctx.rwr, struct ibv_recv_wr,  user_param.recv_post_list);
		}
		ctx.buff_size = MAX(user_param.size, ctx.page_size) * 2;
		printf("Buff Size       : %ld\n",ctx.buff_size);
		if(user_param.connection_type==UD){
			ctx.buff_size += ctx.cache_line_size;
		}
	}
	if (user_param.machine == CLIENT) {
		printf("TX depth        : %d\n",user_param.tx_depth);
	}
	printf("Post List       : %d\n",user_param.post_list);
	printf("Recv Post List  : %d\n", user_param.recv_post_list);
	printf("GID index       : %d\n", user_param.gid_index);
	printf("Max inline data : %d[B]\n",user_param.inline_size);
	printf("Outstand reads  : %d\n",user_param.out_reads);
	printf(RESULT_LINE);

/********************************************* start ib_verbs code /**********************************************/

	{//ctx init
		if(user_param.use_event){
			assert(ctx.channel = ibv_create_comp_channel(ctx.context));
		}

		assert(ctx.pd = ibv_alloc_pd(ctx.context));

		assert(ctx.buf = memalign(ctx.page_size,ctx.buff_size));
		memset(ctx.buf, 0, ctx.buff_size);
		srand(time(NULL));
		for (int i = 0; i < ctx.buff_size/(sizeof(int)); i++) {
			((int*)ctx.buf)[i] = i;
		}

		int flags = IBV_ACCESS_LOCAL_WRITE;
		// if (user_param.verb == WRITE) {
		// 	flags |= IBV_ACCESS_REMOTE_WRITE;
		// } else if (user_param.verb == READ) {
		// 	flags |= IBV_ACCESS_REMOTE_READ;//if iwarp, add remote_write
		// } else if (user_param.verb == ATOMIC) {
		// 	flags |= IBV_ACCESS_REMOTE_ATOMIC;
		// }
		assert(ctx.mr = ibv_reg_mr(ctx.pd, ctx.buf, ctx.buff_size, flags));
		

		//cqs
		assert(ctx.send_cq = ibv_create_cq(ctx.context, user_param.tx_depth, NULL, ctx.channel, user_param.eq_num));
	
		//if send/recv, needs this recv_cq
		if(user_param.verb == SEND){
			assert(ctx.recv_cq = ibv_create_cq(ctx.context, user_param.rx_depth, NULL, ctx.channel, user_param.eq_num));
		}
	
		//qp
		struct ibv_qp_init_attr attr;
		memset(&attr, 0, sizeof(struct ibv_qp_init_attr));
		attr.send_cq = ctx.send_cq;
		attr.recv_cq = (user_param.verb == SEND) ? ctx.recv_cq : ctx.send_cq;
		attr.cap.max_inline_data = user_param.inline_size;
		attr.cap.max_send_wr 	= user_param.tx_depth;
		attr.cap.max_send_sge	= MAX_SEND_SGE; 
		attr.cap.max_recv_wr  = user_param.rx_depth;
		attr.cap.max_recv_sge = MAX_RECV_SGE;

		switch (user_param.connection_type) {
			case RC : attr.qp_type = IBV_QPT_RC; break;
			case UC : attr.qp_type = IBV_QPT_UC; break;
			case UD : attr.qp_type = IBV_QPT_UD; break;
			default:  fprintf(stderr, "Unknown connection type \n");
				exit(1);
		}
		ctx.qp = ibv_create_qp(ctx.pd, &attr);
		if(ctx.qp == NULL && errno == ENOMEM){
			fprintf(stderr, "Requested QP size might be too big. Try reducing TX depth and/or inline size.\n");
			fprintf(stderr, "Current TX depth is %d and inline size is %d .\n", user_param.tx_depth, user_param.inline_size);
		}
		assert(user_param.inline_size <= attr.cap.max_inline_data);
		
		//modify qp to init
		struct ibv_qp_attr attr_qp;
		flags = IBV_QP_STATE | IBV_QP_PKEY_INDEX | IBV_QP_PORT;
		memset(&attr_qp, 0, sizeof(struct ibv_qp_attr));
		attr_qp.qp_state		= IBV_QPS_INIT;
		attr_qp.pkey_index		= user_param.pkey_index;
		attr_qp.port_num 		= user_param.ib_port;
		switch (user_param.verb) {
			case ATOMIC: attr_qp.qp_access_flags = IBV_ACCESS_REMOTE_ATOMIC; break;
			case READ  : attr_qp.qp_access_flags = IBV_ACCESS_REMOTE_READ;  break;
			case WRITE : attr_qp.qp_access_flags = IBV_ACCESS_REMOTE_WRITE; break;
			case SEND  : attr_qp.qp_access_flags = IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_LOCAL_WRITE;
		}
		flags |= IBV_QP_ACCESS_FLAGS;
		assert(ibv_modify_qp(ctx.qp, &attr_qp, flags) == 0);
	}
	

	{//setup connection
		union ibv_gid temp_gid;
		struct ibv_port_attr attr;
		srand48(getpid() * time(NULL));
		assert(ibv_query_port(ctx.context, user_param.ib_port, &attr) == 0);
		user_param.gid_index = 2;//todo
		assert(ibv_query_gid(ctx.context, user_param.ib_port, user_param.gid_index, &temp_gid)==0);

		struct ibv_port_attr attr_;
		assert(ibv_query_port(ctx.context,user_param.ib_port,&attr_) == 0);
		my_dest.lid = attr_.lid;
		my_dest.gid_index = user_param.gid_index;

		my_dest.qpn   = ctx.qp->qp_num;
		my_dest.psn   = lrand48() & 0xffffff;
		my_dest.rkey  = ctx.mr->rkey;

		my_dest.out_reads = user_param.out_reads;

		my_dest.vaddr = (uintptr_t)ctx.buf + MAX(user_param.size,ctx.page_size);

		memcpy(my_dest.gid.raw, temp_gid.raw, 16);
	}
	

	ctx_hand_shake(&user_param,&my_dest,&rem_dest);
	
	{/* ctx_connect, prepare IB resources for rtr/rts */
		struct ibv_qp_attr attr;
		memset(&attr, 0, sizeof attr);
		int flags = IBV_QP_STATE;
		attr.qp_state = IBV_QPS_RTR;
		attr.ah_attr.src_path_bits = 0;
		attr.ah_attr.port_num = user_param.ib_port;

		attr.ah_attr.dlid = rem_dest.lid;
		attr.ah_attr.sl = 0; //service level default 0

		attr.ah_attr.is_global = 1;
		attr.ah_attr.grh.dgid = rem_dest.gid;
		attr.ah_attr.grh.sgid_index = user_param.gid_index;
		attr.ah_attr.grh.hop_limit = 0xFF;
		attr.ah_attr.grh.traffic_class = 0;//default 0

		if(user_param.connection_type != UD){
			attr.path_mtu = user_param.curr_mtu;
			attr.dest_qp_num = rem_dest.qpn;
			attr.rq_psn = rem_dest.psn;
			flags |= (IBV_QP_AV | IBV_QP_PATH_MTU | IBV_QP_DEST_QPN | IBV_QP_RQ_PSN);
			if(user_param.connection_type == RC){
				attr.max_dest_rd_atomic = my_dest.out_reads;
				attr.min_rnr_timer = MIN_RNR_TIMER;
				flags |= (IBV_QP_MIN_RNR_TIMER | IBV_QP_MAX_DEST_RD_ATOMIC);
			}
		}
		assert(ibv_modify_qp(ctx.qp, &attr, flags) == 0);
		if(user_param.machine == CLIENT){
			//ctx_modify_qp_to_rts
			int flags = IBV_QP_STATE;
			struct ibv_qp_attr *_attr = (struct ibv_qp_attr*)(&attr);
			_attr->qp_state = IBV_QPS_RTS;

			flags |= IBV_QP_SQ_PSN;
			_attr->sq_psn = my_dest.psn;

			if(user_param.connection_type == RC){
				_attr->timeout = DEF_QP_TIME;
				_attr->retry_cnt = 7;
				_attr->rnr_retry = 7;
				_attr->max_rd_atomic  = rem_dest.out_reads;
				flags |= (IBV_QP_TIMEOUT | IBV_QP_RETRY_CNT | IBV_QP_RNR_RETRY | IBV_QP_MAX_QP_RD_ATOMIC);
			}
			assert(ibv_modify_qp(ctx.qp,_attr,flags) == 0);
		}

		if(user_param.connection_type==UD && user_param.machine==CLIENT){
			assert(ctx.ah = ibv_create_ah(ctx.pd,&(attr.ah_attr)));
		}
	}

	ctx_hand_shake(&user_param,&my_dest,&rem_dest);
	ctx_print_pingpong_data(&my_dest,&user_param,"local");
	ctx_hand_shake(&user_param,&my_dest,&rem_dest);
	ctx_print_pingpong_data(&rem_dest,&user_param,"remote");
	printf(RESULT_LINE);

	if (user_param.use_event){
		assert(ibv_req_notify_cq(ctx.send_cq, 0) == 0);
		assert(ibv_req_notify_cq(ctx.recv_cq, 0) == 0);
	}
	
	if(user_param.machine == CLIENT){
		memset(&ctx.wr[0],0,sizeof(struct ibv_send_wr));
		ctx.sge_list[0].addr = (uintptr_t)ctx.buf;
		ctx.sge_list[0].length = user_param.size;//if raweth, need minus crc
		ctx.sge_list[0].lkey = ctx.mr->lkey;

		ctx.wr[0].sg_list = &ctx.sge_list[0];
		ctx.wr[0].num_sge = MAX_SEND_SGE;
		ctx.wr[0].wr_id = 0;
		ctx.wr[0].next = NULL;
		ctx.wr[0].send_flags = IBV_SEND_SIGNALED;
		// ctx.wr[0].opcode = opcode_verbs_array[user_param.verb];
		//ctx.wr[index].send_flags |= IBV_SEND_INLINE;
	}

	if(user_param.machine == SERVER){
		struct ibv_recv_wr	*bad_wr_recv;
		int	size_per_qp = user_param.rx_depth / user_param.recv_post_list;
		ctx.recv_sge_list[0].addr = (uintptr_t)ctx.buf + ctx.buff_size / 2;

		ctx.recv_sge_list[0].length = UD_EXTRA(user_param.connection_type,user_param.size);
		ctx.recv_sge_list[0].lkey = ctx.mr->lkey;

		ctx.rwr[0].sg_list = &ctx.recv_sge_list[0];
		ctx.rwr[0].num_sge = MAX_RECV_SGE;
		ctx.rwr[0].wr_id = 0;
		ctx.rwr[0].next=NULL;
	}
	assert(ctx_hand_shake(&user_param,&my_dest,&rem_dest) == 0);
	if(user_param.machine==CLIENT){
		struct ibv_send_wr 	*bad_wr = NULL;
		assert(ibv_post_send(ctx.qp, &ctx.wr[0], &bad_wr) == 0);

		if(user_param.use_event){
			ctx_wait_event(ctx.channel);
		}

		struct ibv_wc *wc = NULL;
		ALLOCATE(wc ,struct ibv_wc ,CTX_POLL_BATCH);
		int ne;
		do{
			ne = ibv_poll_cq(ctx.send_cq, CTX_POLL_BATCH, wc);
			printf("ne:%d\n",ne);
		}while(ne==0);
		printf("ne:%d\n",ne);
		for (int i=0; i<ne; i++) {
			int wc_id = (int)wc[i].wr_id;
			assert(wc[i].status == IBV_WC_SUCCESS);
		}
	}
	
	if(user_param.machine==SERVER){
		struct ibv_recv_wr  *bad_wr_recv = NULL;
		usleep(100000);
		assert(ibv_post_recv(ctx.qp, &ctx.rwr[0], &bad_wr_recv) == 0);
		
		if(user_param.use_event){
			ctx_wait_event(ctx.channel);
		}

		struct ibv_wc *wc = NULL;
		ALLOCATE(wc ,struct ibv_wc ,CTX_POLL_BATCH);
		int ne;
		do{
			ne = ibv_poll_cq(ctx.recv_cq,CTX_POLL_BATCH,wc);
			printf("ne:%d\n",ne);
		}while(ne==0);
		printf("ne:%d\n",ne);
		for (int i=0; i<ne; i++) {
			int wc_id = (int)wc[i].wr_id;
			assert(wc[i].status == IBV_WC_SUCCESS);
		}
	}
	for (int i = 0; i < ctx.buff_size/(sizeof(int)); i++) {
		cout<<((int *)ctx.buf)[i]<<" ";
	}
	return 0;
}