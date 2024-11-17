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
    struct ib_send_wr *send_bar_wr;
    struct ib_recv_wr *recv_bar_wr;
    size_t original_buf;
    struct scatterlist sg;
    unsigned int sg_offset;
    size_t local_buf;
    size_t remote_buf;
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

static int krcore_create_qp(krcore_info_t *info, void __user *_params) {
    int ret = 0;
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
    info->mr = ib_alloc_mr(info->pd, IB_MR_TYPE_MEM_REG, 1);
    if (!info->mr) {
        pr_err("%s: failed to allocate mr\n", MODULE_NAME);
        return -ENOMEM;
    }
    info->original_buf = (size_t)kmalloc(KRCORE_ALLOC_SIZE, GFP_KERNEL);
    if (!info->original_buf) {
        pr_err("%s: failed to allocate original_buf\n", MODULE_NAME);
        return -ENOMEM;
    }
    sg_init_one(&info->sg, (void *)info->original_buf, KRCORE_ALLOC_SIZE);
    info->sg_offset = 0;
    if (ib_map_mr_sg(info->mr, &info->sg, 1, &info->sg_offset, PAGE_SIZE) < 0) {
        pr_err("%s: failed to map mr sg\n", MODULE_NAME);
        return -ENOMEM;
    }
    info->local_buf = info->original_buf + info->sg_offset;


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
    if (ib_modify_qp(info->qp, &qp_attr, flags)) {
        pr_err("%s: failed to modify qp\n", MODULE_NAME);
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
    params.info.vaddr = info->local_buf;
    memcpy(params.info.gid.raw, temp_gid.raw, 16);

    info->send_sge_list = kmalloc(sizeof(struct ib_sge) * KRCORE_NUM_SGES_PER_WR * KRCORE_NUM_WRS, GFP_KERNEL);
    info->recv_sge_list = kmalloc(sizeof(struct ib_sge) * KRCORE_NUM_SGES_PER_WR * KRCORE_NUM_WRS, GFP_KERNEL);
    info->send_wr = kmalloc(sizeof(struct ib_send_wr) * KRCORE_NUM_WRS, GFP_KERNEL);
    info->recv_wr = kmalloc(sizeof(struct ib_recv_wr) * KRCORE_NUM_WRS, GFP_KERNEL);
    info->send_bar_wr = kmalloc(sizeof(struct ib_send_wr), GFP_KERNEL);
    info->recv_bar_wr = kmalloc(sizeof(struct ib_recv_wr), GFP_KERNEL);
    info->num_sges = KRCORE_NUM_SGES_PER_WR * KRCORE_NUM_WRS;
    info->num_sges_per_wr = KRCORE_NUM_SGES_PER_WR;
    info->num_wrs = KRCORE_NUM_WRS;
    info->tx_depth = KRCORE_TX_DEPTH;
    info->rx_depth = KRCORE_RX_DEPTH;

    if (copy_to_user(_params, &params, sizeof(params))) {
        pr_err("%s: failed to copy params to user\n", MODULE_NAME);
        return -EFAULT;
    }

    return ret;
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
    attr.ah_attr.sl = 0;//service level default 0
    attr.ah_attr.grh.dgid = params.info.gid;
    attr.ah_attr.grh.sgid_index = KRCORE_RDMA_GID_INDEX;
    attr.ah_attr.grh.hop_limit = 0xFF;
    attr.ah_attr.grh.traffic_class = 0;
    // TODO maybe need add rdma_ah_attr_type?
    attr.path_mtu = current_mtu;
    attr.dest_qp_num = params.info.qpn;
    attr.rq_psn = params.info.psn;
    flags |= (IB_QP_AV | IB_QP_PATH_MTU | IB_QP_DEST_QPN | IB_QP_RQ_PSN);

    attr.max_dest_rd_atomic = params.info.out_reads;
    attr.min_rnr_timer = KRCORE_MIN_RNR_TIMER;
    flags |= (IB_QP_MIN_RNR_TIMER | IB_QP_MAX_DEST_RD_ATOMIC);

    if (ib_modify_qp(info->qp, &attr, flags)) {
        pr_err("%s: failed to modify qp\n", MODULE_NAME);
        return -ENOMEM;
    }

    flags = IB_QP_STATE;
    memset(&attr, 0, sizeof(attr));
    attr.qp_state = IB_QPS_RTS;
    flags |= IB_QP_SQ_PSN;
    attr.sq_psn = info->qp->qp_num; // just a hack!

    attr.timeout = KRCORE_DEF_QP_TIME;
    attr.retry_cnt = 7;
    attr.rnr_retry = 7;
    attr.max_rd_atomic = params.info.out_reads;
    flags |= (IB_QP_TIMEOUT | IB_QP_RETRY_CNT | IB_QP_RNR_RETRY | IB_QP_MAX_QP_RD_ATOMIC);
    if (ib_modify_qp(info->qp, &attr, flags)) {
        pr_err("%s: failed to modify qp\n", MODULE_NAME);
        return -ENOMEM;
    }

    info->remote_buf = params.info.vaddr;
    info->remote_rkey = params.info.rkey;

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