#include "socket.hpp"

void socket_connect(struct perftest_parameters*	user_param){
	if (user_param->machine == SERVER) {
		printf("\n************************************\n");
		printf("* Waiting for client to connect... *\n");
		printf("************************************\n");
	}
	fflush(stdout);
	struct addrinfo *res, *t;
	struct addrinfo hints;
	char *service = (char*)malloc(sizeof(int));
	int sockfd = -1;
	memset(&hints, 0, sizeof hints);
	hints.ai_family   = AF_INET;

	if(user_param->machine == SERVER){
		int connfd;
		int n;
		hints.ai_flags    = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;
		assert(sprintf(service,"%d", user_param->port) >= 0);
		assert(getaddrinfo(NULL,service,&hints,&res) >= 0);

		for(t=res; t; t=t->ai_next){
			sockfd = socket(t->ai_family, t->ai_socktype, t->ai_protocol);
			if (sockfd >= 0) {
				n = 1;
				setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &n, sizeof n);
				if (!bind(sockfd, t->ai_addr, t->ai_addrlen))
					break;
				close(sockfd);
				sockfd = -1;
			}
		}
		freeaddrinfo(res);
		assert(sockfd >= 0);
		listen(sockfd, 1);
		connfd = accept(sockfd, NULL, 0);
		assert(connfd >= 0);
		user_param->sockfd = connfd;
	}else{
		hints.ai_socktype = SOCK_STREAM;
		assert(sprintf(service,"%d", user_param->port) >= 0);
		assert(getaddrinfo(user_param->servername,service,&hints,&res) >= 0);

		for (t = res; t; t = t->ai_next) {
			sockfd = socket(t->ai_family, t->ai_socktype, t->ai_protocol);
			if (sockfd >= 0) {
				if (!connect(sockfd, t->ai_addr, t->ai_addrlen))
					break;
				close(sockfd);
				sockfd = -1;
			}
		}
		freeaddrinfo(res);
		assert(sockfd >= 0);
		user_param->sockfd = sockfd;
	}
}

void ethernet_write_data(struct perftest_parameters*  user_param, char *msg, size_t size){
	assert(write(user_param->sockfd, msg, size) == size);
}

void ethernet_read_data(struct perftest_parameters*  user_param, char *recv_msg, size_t size){
	assert(read(user_param->sockfd, recv_msg, size) == size);
}

int ctx_xchg_data_ethernet(struct perftest_parameters*  user_param, void *my_data,void *rem_data,int size){
	if(user_param->machine == CLIENT){
		ethernet_write_data(user_param, (char *) my_data, size);
		ethernet_read_data(user_param, (char *) rem_data, size);
	}else{
		ethernet_read_data(user_param, (char *) rem_data, size);
		ethernet_write_data(user_param, (char *) my_data, size);
	}
	return 0;
}

enum ibv_mtu set_mtu(struct ibv_context *context,uint8_t ib_port,int user_mtu){
	struct ibv_port_attr port_attr;
	enum ibv_mtu curr_mtu;
	assert(ibv_query_port(context,ib_port,&port_attr) == 0);
	if (user_mtu == 0) {
		curr_mtu = port_attr.active_mtu;
	}else{
		printf("user_mtu != 0, not finished coed\n");
		exit(0);
	}
	assert((curr_mtu >= IBV_MTU_256 && curr_mtu <= IBV_MTU_4096));
	return curr_mtu;
}

static int ethernet_read_keys(struct pingpong_dest *rem_dest, struct perftest_parameters* user_param){
	if(rem_dest->gid_index == -1){
		printf("Did not handled ethernet_read_keys, please refer the original function, exit\n");
		exit(1);
	}else{
		char msg[KEY_MSG_SIZE_GID];
		char *pstr = msg, *term;
		char tmp[120];
		if(read(user_param->sockfd,msg,sizeof msg) != sizeof msg){
			fprintf(stderr, "ethernet_read_keys: Couldn't read remote address\n");
			return 1;
		}
		term = strpbrk(pstr, ":");
		memcpy(tmp, pstr, term - pstr);
		tmp[term - pstr] = 0;
		rem_dest->lid = (int)strtol(tmp, NULL, 16); /*LID*/

		pstr += term - pstr + 1;
		term = strpbrk(pstr, ":");
		memcpy(tmp, pstr, term - pstr);
		tmp[term - pstr] = 0;
		rem_dest->out_reads = (int)strtol(tmp, NULL, 16); /*OUT_READS*/

		pstr += term - pstr + 1;
		term = strpbrk(pstr, ":");
		memcpy(tmp, pstr, term - pstr);
		tmp[term - pstr] = 0;
		rem_dest->qpn = (int)strtol(tmp, NULL, 16); /*QPN*/

		pstr += term - pstr + 1;
		term = strpbrk(pstr, ":");
		memcpy(tmp, pstr, term - pstr);
		tmp[term - pstr] = 0;
		rem_dest->psn = (int)strtol(tmp, NULL, 16); /*PSN*/

		pstr += term - pstr + 1;
		term = strpbrk(pstr, ":");
		memcpy(tmp, pstr, term - pstr);
		tmp[term - pstr] = 0;
		rem_dest->rkey = (unsigned)strtoul(tmp, NULL, 16); /*RKEY*/

		pstr += term - pstr + 1;
		term = strpbrk(pstr, ":");
		memcpy(tmp, pstr, term - pstr);
		tmp[term - pstr] = 0;

		rem_dest->vaddr = strtoull(tmp, NULL, 16); /*VA*/

		for (int i = 0; i < 15; ++i) {
			pstr += term - pstr + 1;
			term = strpbrk(pstr, ":");
			memcpy(tmp, pstr, term - pstr);
			tmp[term - pstr] = 0;

			rem_dest->gid.raw[i] = (unsigned char)strtoll(tmp, NULL, 16);
		}

		pstr += term - pstr + 1;

		strcpy(tmp, pstr);
		rem_dest->gid.raw[15] = (unsigned char)strtoll(tmp, NULL, 16);
	}
	return 0;
}

#define KEY_PRINT_FMT_GID "%04x:%04x:%06x:%06x:%08x:%016llx:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:"

static int ethernet_write_keys(struct pingpong_dest *my_dest, struct perftest_parameters* user_param){
	if(my_dest->gid_index == -1){
		printf("Did not handled ethernet_write_keys, please refer the original function, exit\n");
		exit(1);
	}else{
		char msg[KEY_MSG_SIZE_GID];
		sprintf(msg,KEY_PRINT_FMT_GID, my_dest->lid,my_dest->out_reads,
				my_dest->qpn,my_dest->psn, my_dest->rkey, my_dest->vaddr,
				my_dest->gid.raw[0],my_dest->gid.raw[1],
				my_dest->gid.raw[2],my_dest->gid.raw[3],
				my_dest->gid.raw[4],my_dest->gid.raw[5],
				my_dest->gid.raw[6],my_dest->gid.raw[7],
				my_dest->gid.raw[8],my_dest->gid.raw[9],
				my_dest->gid.raw[10],my_dest->gid.raw[11],
				my_dest->gid.raw[12],my_dest->gid.raw[13],
				my_dest->gid.raw[14],my_dest->gid.raw[15]);
		
		if(write(user_param->sockfd, msg, sizeof msg) != sizeof msg){
			perror("client write");
			fprintf(stderr, "Couldn't send local address\n");
			return 1;
		}
	}
	return 0;
}

int ctx_hand_shake(struct perftest_parameters* user_param, struct pingpong_dest *my_dest, struct pingpong_dest *rem_dest){
	int (*read_func_ptr) (struct pingpong_dest*,struct perftest_parameters*);
	int (*write_func_ptr)(struct pingpong_dest*,struct perftest_parameters*);
	read_func_ptr  = &ethernet_read_keys;
	write_func_ptr = &ethernet_write_keys;

	rem_dest->gid_index = my_dest->gid_index;

	if(user_param->servername){
		assert((*write_func_ptr)(my_dest,user_param) == 0);
		assert((*read_func_ptr)(rem_dest,user_param) == 0);
	}else {
		assert((*read_func_ptr)(rem_dest,user_param) == 0);
		assert((*write_func_ptr)(my_dest,user_param) == 0);
	}
	return 0;
}

void ctx_print_pingpong_data(struct pingpong_dest *element, struct perftest_parameters* user_param, char const* side){
	uint16_t dlid = element->lid;
	printf(BASIC_ADDR_FMT, side, dlid, element->qpn, element->psn);
	switch(user_param->verb){
		case WRITE: printf(READ_FMT,element->out_reads);
		case READ: printf(RDMA_FMT,element->rkey,element->vaddr);
		default : ;
	}
	putchar('\n');

	if (user_param->gid_index > -1) {
		printf(PERF_GID_FMT,"GID",
				element->gid.raw[0], element->gid.raw[1],
				element->gid.raw[2], element->gid.raw[3],
				element->gid.raw[4], element->gid.raw[5],
				element->gid.raw[6], element->gid.raw[7],
				element->gid.raw[8], element->gid.raw[9],
				element->gid.raw[10],element->gid.raw[11],
				element->gid.raw[12],element->gid.raw[13],
				element->gid.raw[14],element->gid.raw[15]);
	}
}