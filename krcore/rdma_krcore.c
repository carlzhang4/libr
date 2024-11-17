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

#include "rdma_krcore.h"

//  Define the module metadata.
#define MODULE_NAME "krcore"
#define  DEVICE_NAME "krcore"
MODULE_AUTHOR("cxz66666");
MODULE_LICENSE("GPL v2");
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

struct qp_handler {
    struct ib_pd pd;
};

struct krcore_info {
    struct list_head list;
    struct mutex lock;
    pid_t pid;
    pid_t tgid;

    // used for rdma
    struct ib_qp qp;
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
    pr_info("%s: device added\n", dev->name);
    return 0;
}

void krcore_remove_device(struct ib_device *dev, void *client_data) {
    pr_info("%s: device removed\n", dev->name);
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

    global_device = ib_device_get_by_name("mlx5_0", RDMA_DRIVER_MLX5);
    if (!global_device) {
        pr_err("%s: failed to get ib_device\n", MODULE_NAME);
    } else {
        pr_info("%s: got ib_device %s\n", MODULE_NAME, global_device->name);
    }
    //  Success!
    return 0;
}

static void __exit mod_exit(void) {
    pr_info("%s: unloading...\n", MODULE_NAME);
    unregister_chrdev(majorNumber, DEVICE_NAME);

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
    struct KRCORE_IOC_CREATE_QP_PARAMS params;
    if (copy_from_user(&params, _params, sizeof(params))) {
        pr_err("%s: failed to copy params from user\n", MODULE_NAME);
        return -EFAULT;
    }



    if (copy_to_user(_params, &params, sizeof(params))) {
        pr_err("%s: failed to copy params to user\n", MODULE_NAME);
        return -EFAULT;
    }

    return ret;
}

static int krcore_init_qp(krcore_info_t *info, void __user *_params) {
    int ret = 0;
    struct KRCORE_IOC_INIT_QP_PARAMS params;
    if (copy_from_user(&params, _params, sizeof(params))) {
        pr_err("%s: failed to copy params from user\n", MODULE_NAME);
        return -EFAULT;
    }

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