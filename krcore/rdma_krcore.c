#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/version.h>

#include <rdma/ib_verbs.h>
#include <rdma/rdma_cm.h>
#include <rdma/ib_cache.h>

#include "rdma_krcore.h"

//  Define the module metadata.
#define MODULE_NAME "krcore"
#define  DEVICE_NAME "krcore"

MODULE_AUTHOR("cxz66666");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("A module to simulate KRCore kernel module");
MODULE_VERSION("0.1");

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 0, 0) && !defined(HAVE_UNLOCKED_IOCTL)
#define HAVE_UNLOCKED_IOCTL 1
#endif 


//  The device number, automatically set. The message buffer and current message
//  size. The number of device opens and the device class struct pointers.
static int    majorNumber;
static DEFINE_MUTEX(ioMutex);

static int SEND_COPY = 0;
static int RECV_COPY = 1;

//  Prototypes for our device functions.
static int     krcore_open(struct inode *, struct file *);
static int     krcore_release(struct inode *, struct file *);
static long krcore_ioctl(struct inode *inode, struct file *filep, unsigned int cmd, unsigned long arg);
static long krcore_unlocked_ioctl(struct file *filep, unsigned int cmd, unsigned long arg);

struct ib_device *global_device;

struct ib_client krcore_ib_client;

unsigned int current_mtu;
struct qp_handler {
    struct ib_pd pd;
};

struct krcore_info {
    struct list_head list;
    struct mutex lock;
    pid_t pid;
    pid_t tgid;

    // used for rdma
    struct ib_pd *pd;
    struct ib_mr *mr;
    struct ib_qp *qp;
    struct ib_cq *send_cq;
    struct ib_cq *recv_cq;
    struct ib_sge *send_sge_list;
    struct ib_sge *recv_sge_list;
    struct ib_send_wr *send_wr;
    struct ib_recv_wr *recv_wr;
    struct ib_wc *send_wc;
    struct ib_wc *recv_wc;

    size_t original_buf;
    struct scatterlist *sg;
    unsigned int sg_offset;
    size_t user_local_buf;
    size_t local_buf; // vmalloc address, can direct load/store
    size_t local_dma_buf; // dma address created by ib_dma_map_sg. 尽管连续的虚拟地址对应的是离散的物理地址，但是RDMA还是需要使用第0个sg的dma地址+offset作为rdma wr的地址
    // https://elixir.bootlin.com/linux/v5.15.102/source/drivers/infiniband/core/verbs.c#L2682
    size_t remote_dma_buf;
    unsigned int remote_rkey;
    int num_wrs;
    int num_sges_per_wr;
    int num_sges;
    int tx_depth;
    int rx_depth;
};
typedef struct krcore_info krcore_info_t;

//  Create the file operations instance for our driver.
static struct file_operations fops =
{
   .open = krcore_open,
#ifdef HAVE_UNLOCKED_IOCTL
    .unlocked_ioctl = krcore_unlocked_ioctl,
#else
    .ioctl = krcore_ioctl,
#endif  
   .release = krcore_release,
};

int krcore_add_device(struct ib_device *dev) {
    struct ib_port_attr port_attr;
    if (strcmp(dev->name, "mlx5_0") == 0) {
        global_device = dev;
        pr_info("%s: device added\n", dev->name);

        ib_query_port(dev, 1, &port_attr);
        current_mtu = 128 << port_attr.active_mtu;

        pr_info("%-20s : %d\n", "Max Outreads", dev->attrs.max_qp_rd_atom);
        pr_info("%-20s : %d\n", "Max Pkeys", dev->attrs.max_pkeys);
        pr_info("%-20s : %d\n", "Atomic Capacity", dev->attrs.atomic_cap);
        pr_info("%-20s : %d\n", "CUR MTU", 128 << port_attr.active_mtu);
        pr_info("%-20s : %d\n", "CUR SPEED", port_attr.active_speed);
    }
    return 0;
}

void krcore_remove_device(struct ib_device *dev, void *client_data) {
    if (strcmp(dev->name, "mlx5_0") == 0) {
        global_device = NULL;
        pr_info("%s: device removed\n", dev->name);
    }
}

static int __init mod_init(void) {
    pr_info("%s: module loaded at 0x%p\n", MODULE_NAME, mod_init);

    //  Create a mutex to guard io operations.
    mutex_init(&ioMutex);

    //  Register the device, allocating a major number.
    majorNumber = register_chrdev(0 /* i.e. allocate a major number for me */, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        pr_alert("%s: failed to register a major number\n", MODULE_NAME);
        return majorNumber;
    }
    pr_info("%s: registered correctly with major number %d\n", MODULE_NAME, majorNumber);

    krcore_ib_client.name = "krcore";
    krcore_ib_client.add = krcore_add_device;
    krcore_ib_client.remove = krcore_remove_device;
    ib_register_client(&krcore_ib_client);

    return 0;
}

static void __exit mod_exit(void) {
    pr_info("%s: unloading...\n", MODULE_NAME);
    unregister_chrdev(majorNumber, DEVICE_NAME);

    ib_unregister_client(&krcore_ib_client);

    mutex_destroy(&ioMutex);
    pr_info("%s: device unregistered\n", MODULE_NAME);
}

/** @brief The device open function that is called each time the device is opened
 *  This will only increment the numberOpens counter in this case.
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int krcore_open(struct inode *inodep, struct file *filep) {
    krcore_info_t *info = kzalloc(sizeof(krcore_info_t), GFP_KERNEL);
    //  Try and lock the mutex.
    mutex_lock(&ioMutex);
    if (!info) {
        pr_err("%s: failed to allocate memory for krcore_info_t\n", MODULE_NAME);
        return -ENOMEM;
    }

    INIT_LIST_HEAD(&info->list);
    mutex_init(&info->lock);
    info->pid = current->pid;
    info->tgid = current->tgid;
    filep->private_data = info;
    pr_info("%s: tgid %d pid %d has been opened\n", MODULE_NAME, info->tgid, info->pid);
    mutex_unlock(&ioMutex);
    return 0;
}


/** @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int krcore_release(struct inode *inodep, struct file *filep) {
    krcore_info_t *info = filep->private_data;
    pid_t pid = info->pid, tgid = info->tgid;

    mutex_lock(&ioMutex);

    if (!info) {
        pr_err("%s: filep->private_data is NULL\n", MODULE_NAME);
        return -EIO;
    }
    if (current->tgid != tgid) {
        pr_err("%s: tgid %d pid %d is not allowed to close this file\n", MODULE_NAME, current->tgid, current->pid);
        return -EPERM;
    }

    mutex_lock(&info->lock);
    // free rdma verbs 
    mutex_unlock(&info->lock);

    kfree(info);
    filep->private_data = NULL;
    pr_info("%s: tgid %d pid %d successfully closed\n", MODULE_NAME, tgid, pid);

    mutex_unlock(&ioMutex);
    return 0;
}
static void krcore_send_reg_mr(krcore_info_t *info) {
    struct ib_reg_wr wr;
    wr.wr.next = NULL;

    wr.wr.opcode = IB_WR_REG_MR;
    wr.wr.num_sge = 0;
    wr.wr.send_flags = IB_SEND_SIGNALED;
    wr.mr = info->mr;
    wr.key = info->mr->lkey;
    wr.access = IB_ACCESS_LOCAL_WRITE | IB_ACCESS_REMOTE_READ | IB_ACCESS_REMOTE_WRITE;
    ib_post_send(info->qp, &wr.wr, NULL);

    while (ib_poll_cq(info->send_cq, 1, info->send_wc) == 0) {
    }
    if (info->send_wc->status != IB_WC_SUCCESS) {
        pr_err("%s: failed to register mr\n", MODULE_NAME);
    } else {
        pr_info("%s: mr registered\n", MODULE_NAME);
    }
}
static int krcore_create_qp(krcore_info_t *info, void __user *_params) {
    int ret = 0;
    int index = 0;
    unsigned long page_start;
    struct page *now_page;

    struct ib_cq_init_attr send_cq_attr = {
        .cqe = KRCORE_TX_DEPTH,
        .comp_vector = 0,
        .flags = 0
    };
    struct ib_cq_init_attr recv_cq_attr = {
        .cqe = KRCORE_RX_DEPTH,
        .comp_vector = 0,
        .flags = 0
    };
    struct ib_qp_init_attr qp_init_attr;
    struct ib_qp_attr qp_attr;
    int flags = IB_QP_STATE | IB_QP_PKEY_INDEX | IB_QP_PORT | IB_QP_ACCESS_FLAGS;
    union ib_gid temp_gid;
    struct ib_port_attr port_attr;

    struct KRCORE_IOC_CREATE_QP_PARAMS params;
    if (copy_from_user(&params, _params, sizeof(params))) {
        pr_err("%s: failed to copy params from user\n", MODULE_NAME);
        return -EFAULT;
    }

    info->pd = ib_alloc_pd(global_device, 0);
    if (!info->pd) {
        pr_err("%s: failed to allocate pd\n", MODULE_NAME);
        return -ENOMEM;
    }
    info->mr = ib_alloc_mr(info->pd, IB_MR_TYPE_MEM_REG, KRCORE_ALLOC_SIZE / PAGE_SIZE);
    if (!info->mr) {
        pr_err("%s: failed to allocate mr\n", MODULE_NAME);
        return -ENOMEM;
    }
    info->original_buf = (size_t)vmalloc(KRCORE_ALLOC_SIZE);
    if (!info->original_buf) {
        pr_err("%s: failed to allocate original_buf\n", MODULE_NAME);
        return -ENOMEM;
    }
    info->sg = kcalloc(KRCORE_ALLOC_SIZE / PAGE_SIZE, sizeof(struct scatterlist), GFP_KERNEL);
    if (!info->sg) {
        pr_err("%s: failed to allocate sg\n", MODULE_NAME);
        return -ENOMEM;
    }
    sg_init_table(info->sg, KRCORE_ALLOC_SIZE / PAGE_SIZE);
    for (;index < KRCORE_ALLOC_SIZE / PAGE_SIZE;index++) {
        page_start = (unsigned long)info->original_buf + (index * PAGE_SIZE);
        now_page = vmalloc_to_page((void *)page_start);
        if (!now_page) {
            pr_err("%s: failed to get page\n", MODULE_NAME);
            return -ENOMEM;
        }
        sg_set_page(info->sg + index, now_page, PAGE_SIZE, 0);
    }

    info->sg_offset = 0;
    if (ib_dma_map_sg(global_device, info->sg, KRCORE_ALLOC_SIZE / PAGE_SIZE, DMA_BIDIRECTIONAL) < 0) {
        pr_err("%s: failed to map sg\n", MODULE_NAME);
        return -ENOMEM;
    }
    // index = 0;
    // for (;index < 50;index++) {
    //     struct scatterlist *sg = info->sg + index;
    //     pr_info("%s: sg[%d] va: %lx page: %p, offset: %d, dma_address: %lx\n", MODULE_NAME, index, info->original_buf + (index * PAGE_SIZE), sg_page(sg), sg->offset, sg->dma_address);
    // }

    index = ib_map_mr_sg(info->mr, info->sg, KRCORE_ALLOC_SIZE / PAGE_SIZE, &info->sg_offset, PAGE_SIZE);
    if (index < 0 || index != KRCORE_ALLOC_SIZE / PAGE_SIZE) {
        pr_err("%s: failed to map mr sg\n", MODULE_NAME);
        return -ENOMEM;
    }

    // index = 0;
    // for (;index < 50;index++) {
    //     struct scatterlist *sg = info->sg + index;
    //     pr_info("%s: sg[%d] va: %lx page: %p, offset: %d, dma_address: %lx\n", MODULE_NAME, index, info->original_buf + (index * PAGE_SIZE), sg_page(sg), sg->offset, sg->dma_address);
    // }


    info->local_buf = info->original_buf;
    info->local_dma_buf = info->sg->dma_address;
    info->user_local_buf = params.user_buf;

    info->send_cq = ib_create_cq(global_device, NULL, NULL, NULL, &send_cq_attr);
    info->recv_cq = ib_create_cq(global_device, NULL, NULL, NULL, &recv_cq_attr);

    memset(&qp_init_attr, 0, sizeof(qp_init_attr));
    qp_init_attr.send_cq = info->send_cq;
    qp_init_attr.recv_cq = info->recv_cq;
    qp_init_attr.cap.max_inline_data = 0;
    qp_init_attr.cap.max_send_wr = KRCORE_TX_DEPTH;
    qp_init_attr.cap.max_send_sge = KRCORE_NUM_SGES_PER_WR;
    qp_init_attr.cap.max_recv_wr = KRCORE_RX_DEPTH;
    qp_init_attr.cap.max_recv_sge = KRCORE_NUM_SGES_PER_WR;
    qp_init_attr.qp_type = IB_QPT_RC;
    info->qp = ib_create_qp(info->pd, &qp_init_attr);
    if (!info->qp) {
        pr_err("%s: failed to create qp\n", MODULE_NAME);
        return -ENOMEM;
    }

    memset(&qp_attr, 0, sizeof(qp_attr));
    qp_attr.qp_state = IB_QPS_INIT;
    qp_attr.pkey_index = 0;
    qp_attr.port_num = 1;
    qp_attr.qp_access_flags = IB_ACCESS_REMOTE_WRITE | IB_ACCESS_LOCAL_WRITE;
    if ((ret = ib_modify_qp(info->qp, &qp_attr, flags))) {
        pr_err("%s: failed to modify qp to init errno: %d\n", MODULE_NAME, ret);
        return -ENOMEM;
    }

    ib_query_port(global_device, 1, &port_attr);
    rdma_query_gid(global_device, 1, KRCORE_RDMA_GID_INDEX, &temp_gid);
    params.info.lid = port_attr.lid;
    params.info.gid_index = KRCORE_RDMA_GID_INDEX;
    params.info.qpn = info->qp->qp_num;
    params.info.psn = params.info.qpn & 0xffffff;
    params.info.rkey = info->mr->rkey;
    params.info.out_reads = 1;
    params.info.vaddr = info->local_dma_buf;
    memcpy(params.info.raw_gid, temp_gid.raw, 16);

    if (params.batch_size == 0) {
        info->num_wrs = KRCORE_NUM_WRS;
    } else {
        info->num_wrs = params.batch_size;
    }
    info->send_sge_list = kmalloc(sizeof(struct ib_sge) * KRCORE_NUM_SGES_PER_WR * info->num_wrs, GFP_KERNEL);
    info->recv_sge_list = kmalloc(sizeof(struct ib_sge) * KRCORE_NUM_SGES_PER_WR * info->num_wrs, GFP_KERNEL);
    info->send_wr = kmalloc(sizeof(struct ib_send_wr) * info->num_wrs, GFP_KERNEL);
    info->recv_wr = kmalloc(sizeof(struct ib_recv_wr) * info->num_wrs, GFP_KERNEL);
    info->send_wc = kmalloc(sizeof(struct ib_wc) * KRCORE_CQ_POLL_BATCH, GFP_KERNEL);
    info->recv_wc = kmalloc(sizeof(struct ib_wc) * KRCORE_CQ_POLL_BATCH, GFP_KERNEL);
    info->num_sges = KRCORE_NUM_SGES_PER_WR * info->num_wrs;
    info->num_sges_per_wr = KRCORE_NUM_SGES_PER_WR;
    info->tx_depth = KRCORE_TX_DEPTH;
    info->rx_depth = KRCORE_RX_DEPTH;

    pr_info("%s: Create success, local QPN:%#06x\n", MODULE_NAME, info->qp->qp_num);

    if (copy_to_user(_params, &params, sizeof(params))) {
        pr_err("%s: failed to copy params to user\n", MODULE_NAME);
        return -EFAULT;
    }

    return ret;
}
static void krcore_init_wr_base_send_recv(krcore_info_t *info) {
    struct ib_send_wr *send_wr = info->send_wr;
    struct ib_recv_wr *recv_wr = info->recv_wr;
    struct ib_sge *send_sge_list = info->send_sge_list, *recv_sge_list = info->recv_sge_list;
    int index = 0;
    memset(send_wr, 0, sizeof(struct ib_send_wr) * info->num_wrs);
    memset(recv_wr, 0, sizeof(struct ib_recv_wr) * info->num_wrs);

    for (;index < info->num_wrs;index++) {
        send_sge_list[index].addr = info->local_dma_buf;
        send_sge_list[index].lkey = info->mr->lkey;

        send_wr[index].sg_list = send_sge_list + index * info->num_sges_per_wr;
        send_wr[index].num_sge = info->num_sges_per_wr;
        send_wr[index].wr_id = 1000;
        send_wr[index].next = NULL;
        send_wr[index].send_flags = IB_SEND_SIGNALED;
        send_wr[index].opcode = IB_WR_SEND;
        if (index > 0) {
            send_wr[index - 1].next = send_wr + index;
        }

        recv_sge_list[index].addr = info->local_dma_buf;
        recv_sge_list[index].lkey = info->mr->lkey;
        recv_wr[index].sg_list = recv_sge_list + index * info->num_sges_per_wr;
        recv_wr[index].num_sge = info->num_sges_per_wr;
        recv_wr[index].wr_id = 1001;
        recv_wr[index].next = NULL;
        if (index > 0) {
            recv_wr[index - 1].next = recv_wr + index;
        }
    }
}

static int krcore_init_qp(krcore_info_t *info, void __user *_params) {
    int ret = 0;
    struct ib_qp_attr attr;
    int flags = 0;

    struct KRCORE_IOC_INIT_QP_PARAMS params;
    if (copy_from_user(&params, _params, sizeof(params))) {
        pr_err("%s: failed to copy params from user\n", MODULE_NAME);
        return -EFAULT;
    }
    flags = IB_QP_STATE;
    memset(&attr, 0, sizeof(attr));
    attr.qp_state = IB_QPS_RTR;
    attr.ah_attr.port_num = 1;
    attr.ah_attr.ah_flags = IB_AH_GRH; // important
    attr.ah_attr.sl = 0;//service level default 0
    memcpy(attr.ah_attr.grh.dgid.raw, params.info.raw_gid, 16);
    attr.ah_attr.grh.sgid_index = KRCORE_RDMA_GID_INDEX;
    attr.ah_attr.grh.hop_limit = 0xFF;
    attr.ah_attr.grh.traffic_class = 0;
    // TODO maybe need add rdma_ah_attr_type?
    attr.ah_attr.type = RDMA_AH_ATTR_TYPE_ROCE;
    memcpy(attr.ah_attr.roce.dmac, params.info.mac, 6);

    attr.path_mtu = ilog2(current_mtu / 128);
    attr.dest_qp_num = params.info.qpn;
    attr.rq_psn = params.info.psn;
    flags |= (IB_QP_AV | IB_QP_PATH_MTU | IB_QP_DEST_QPN | IB_QP_RQ_PSN);

    attr.max_dest_rd_atomic = params.info.out_reads;
    attr.min_rnr_timer = KRCORE_MIN_RNR_TIMER;
    flags |= (IB_QP_MIN_RNR_TIMER | IB_QP_MAX_DEST_RD_ATOMIC);

    if ((ret = ib_modify_qp(info->qp, &attr, flags))) {
        pr_err("%s: failed to modify qp to RTR errno: %d\n", MODULE_NAME, ret);
        return -ENOMEM;
    }

    flags = IB_QP_STATE;
    memset(&attr, 0, sizeof(attr));
    attr.qp_state = IB_QPS_RTS;
    flags |= IB_QP_SQ_PSN;
    attr.sq_psn = info->qp->qp_num & 0xffffff; // just a hack!

    attr.timeout = KRCORE_DEF_QP_TIME;
    attr.retry_cnt = 7;
    attr.rnr_retry = 7;
    attr.max_rd_atomic = params.info.out_reads;
    flags |= (IB_QP_TIMEOUT | IB_QP_RETRY_CNT | IB_QP_RNR_RETRY | IB_QP_MAX_QP_RD_ATOMIC);
    if (ib_modify_qp(info->qp, &attr, flags)) {
        pr_err("%s: failed to modify qp to RTS errno: %d\n", MODULE_NAME, ret);
        return -ENOMEM;
    }

    info->remote_dma_buf = params.info.vaddr;
    info->remote_rkey = params.info.rkey;

    krcore_init_wr_base_send_recv(info);

    krcore_send_reg_mr(info);

    pr_info("%s: Connected success, local QPN:%#06x, remote QPN:%#08x\n", MODULE_NAME, info->qp->qp_num, params.info.qpn);

    if (copy_to_user(_params, &params, sizeof(params))) {
        pr_err("%s: failed to copy params to user\n", MODULE_NAME);
        return -EFAULT;
    }

    return ret;
}

static int krcore_post_send(krcore_info_t *info, void __user *_params) {
    int ret = 0;
    int index = 0;
    size_t now_offset = 0;
    struct KRCORE_IOC_POST_SEND_PARAMS params;
    if (copy_from_user(&params, _params, sizeof(params))) {
        pr_err("%s: failed to copy params from user\n", MODULE_NAME);
        return -EFAULT;
    }
    for (;index < params.batch_size;index++) {
        now_offset = (params.offset + index * params.length) % (KRCORE_ALLOC_SIZE / 2);
        // copy data!
        if (SEND_COPY) {
            if (copy_from_user((void *)(info->local_buf + now_offset), (void *)(info->user_local_buf + now_offset), params.length)) {
                pr_err("%s: failed to copy data from user\n", MODULE_NAME);
                return -EFAULT;
            }
        }


        info->send_sge_list[index].addr = info->local_dma_buf + now_offset;
        info->send_sge_list[index].length = params.length;
        info->send_wr[index].wr_id = now_offset;
        if (index < params.batch_size - 1) {
            info->send_wr[index].next = &info->send_wr[index + 1];
        } else {
            info->send_wr[index].next = NULL;
        }
    }

    if (ib_post_send(info->qp, info->send_wr, NULL)) {
        pr_err("%s: failed to post send\n", MODULE_NAME);
        return -ENOMEM;
    }

    params.success_send_cnt = params.batch_size;
    if (copy_to_user(_params, &params, sizeof(params))) {
        pr_err("%s: failed to copy params to user\n", MODULE_NAME);
        return -EFAULT;
    }
    return ret;
}

static int krcore_post_recv(krcore_info_t *info, void __user *_params) {
    int ret = 0;
    int index = 0;
    int now_offset = 0;
    struct KRCORE_IOC_POST_RECV_PARAMS params;
    if (copy_from_user(&params, _params, sizeof(params))) {
        pr_err("%s: failed to copy params from user\n", MODULE_NAME);
        return -EFAULT;
    }

    for (;index < params.batch_size;index++) {
        now_offset = (params.offset + index * params.length) % (KRCORE_ALLOC_SIZE / 2) + (KRCORE_ALLOC_SIZE / 2);
        info->recv_sge_list[index].addr = info->local_dma_buf + now_offset;
        info->recv_sge_list[index].length = params.length;
        info->recv_wr[index].wr_id = now_offset;
        if (index < params.batch_size - 1) {
            info->recv_wr[index].next = &info->recv_wr[index + 1];
        } else {
            info->recv_wr[index].next = NULL;
        }
    }

    if (ib_post_recv(info->qp, info->recv_wr, NULL)) {
        pr_err("%s: failed to post recv\n", MODULE_NAME);
        return -ENOMEM;
    }

    params.success_post_cnt = params.batch_size;
    if (copy_to_user(_params, &params, sizeof(params))) {
        pr_err("%s: failed to copy params to user\n", MODULE_NAME);
        return -EFAULT;
    }
    return ret;
}

static int krcore_poll_send_cq(krcore_info_t *info, void __user *_params) {
    int ret = 0;
    int index = 0;
    int ne;
    struct KRCORE_IOC_POLL_SEND_CQ_PARAMS params;
    if (copy_from_user(&params, _params, sizeof(params))) {
        pr_err("%s: failed to copy params from user\n", MODULE_NAME);
        return -EFAULT;
    }
    ne = ib_poll_cq(info->send_cq, min(KRCORE_CQ_POLL_BATCH, params.max_poll_num), info->send_wc);
    if (ne < 0) {
        pr_err("%s: failed to poll send cq\n", MODULE_NAME);
        return -ENOMEM;
    }
    for (;index < ne;index++) {
        if (info->send_wc[index].status != IB_WC_SUCCESS) {
            pr_err("%s: send wc status is not success %d\n", MODULE_NAME, info->send_wc[index].status);
            return -ENOMEM;
        }
    }
    params.actual_poll_num = ne;
    if (copy_to_user(_params, &params, sizeof(params))) {
        pr_err("%s: failed to copy params to user\n", MODULE_NAME);
        return -EFAULT;
    }
    return ret;
}

static int krcore_poll_recv_cq(krcore_info_t *info, void __user *_params) {
    int ret = 0;
    int index = 0;
    int ne;
    struct KRCORE_IOC_POLL_RECV_CQ_PARAMS params;
    if (copy_from_user(&params, _params, sizeof(params))) {
        pr_err("%s: failed to copy params from user\n", MODULE_NAME);
        return -EFAULT;
    }
    ne = ib_poll_cq(info->recv_cq, min(KRCORE_CQ_POLL_BATCH, params.max_poll_num), info->recv_wc);
    if (ne < 0) {
        pr_err("%s: failed to poll recv cq\n", MODULE_NAME);
        return -ENOMEM;
    }
    for (;index < ne;index++) {
        if (info->recv_wc[index].status != IB_WC_SUCCESS) {
            pr_err("%s: recv wc status is not success %d\n", MODULE_NAME, info->recv_wc[index].status);
            return -ENOMEM;
        }
        // copy data!!
        if (RECV_COPY) {
            if (copy_to_user((void *)(info->user_local_buf + info->recv_wc[index].wr_id), (void *)(info->local_buf + info->recv_wc[index].wr_id), info->recv_wc[index].byte_len)) {
                pr_err("%s: failed to copy data to user\n", MODULE_NAME);
                return -EFAULT;
            }
        }
    }
    params.actual_poll_num = ne;
    if (copy_to_user(_params, &params, sizeof(params))) {
        pr_err("%s: failed to copy params to user\n", MODULE_NAME);
        return -EFAULT;
    }
    return ret;
}

static int krcore_free_qp(krcore_info_t *info, void __user *_params) {
    int ret = 0;
    struct KRCORE_IOC_FREE_QP_PARAMS params;
    if (copy_from_user(&params, _params, sizeof(params))) {
        pr_err("%s: failed to copy params from user\n", MODULE_NAME);
        return -EFAULT;
    }
    kfree(info->send_sge_list);
    kfree(info->recv_sge_list);
    kfree(info->send_wr);
    kfree(info->recv_wr);
    kfree(info->send_wc);
    kfree(info->recv_wc);
    kfree(info->sg);

    ib_destroy_qp(info->qp);
    ib_dereg_mr(info->mr);
    ib_destroy_cq(info->send_cq);
    ib_destroy_cq(info->recv_cq);
    ib_dealloc_pd(info->pd);

    vfree((void *)info->original_buf);

    pr_info("%s: Free QP success, local QPN:%#06x\n", MODULE_NAME, info->qp->qp_num);

    params.success = 1;
    if (copy_to_user(_params, &params, sizeof(params))) {
        pr_err("%s: failed to copy params to user\n", MODULE_NAME);
        return -EFAULT;
    }
    return ret;
}

static long krcore_ioctl(struct inode *inode, struct file *filep, unsigned int cmd, unsigned long arg) {
    int ret = 0;
    krcore_info_t *info = filep->private_data;
    void __user *argp = (void __user *)arg;

    if (_IOC_TYPE(cmd) != KRCORE_IOCTL) {
        pr_err("%s: invalid ioctl type\n", MODULE_NAME);
        return -EINVAL;
    }

    if (!info) {
        pr_err("%s: filep->private_data is NULL\n", MODULE_NAME);
        return -EIO;
    }

    if (current->tgid != info->tgid) {
        pr_err("%s: tgid %d pid %d is not allowed to ioctl this file\n", MODULE_NAME, current->tgid, current->pid);
        return -EACCES;
    }

    switch (cmd) {
    case KRCORE_IOC_CREATE_QP:
        ret = krcore_create_qp(info, argp);
        break;
    case KRCORE_IOC_INIT_QP:
        ret = krcore_init_qp(info, argp);
        break;
    case KRCORE_IOC_POST_SEND:
        ret = krcore_post_send(info, argp);
        break;
    case KRCORE_IOC_POST_RECV:
        ret = krcore_post_recv(info, argp);
        break;
    case KRCORE_IOC_POLL_SEND_CQ:
        ret = krcore_poll_send_cq(info, argp);
        break;
    case KRCORE_IOC_POLL_RECV_CQ:
        ret = krcore_poll_recv_cq(info, argp);
        break;
    case KRCORE_IOC_FREE_QP:
        ret = krcore_free_qp(info, argp);
        break;
    default:
        pr_err("%s: invalid ioctl command %d\n", MODULE_NAME, cmd);
        return -ENOTTY;
    }
    return ret;
}

#ifdef HAVE_UNLOCKED_IOCTL
static long krcore_unlocked_ioctl(struct file *filep, unsigned int cmd, unsigned long arg) {
    return krcore_ioctl(0, filep, cmd, arg);
}
#endif  

module_init(mod_init);
module_exit(mod_exit);