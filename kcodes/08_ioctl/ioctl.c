#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/ioctl.h>

#define EXIT_SUCCESS 0
#define FIRST_MINOR 0
#define EXIT_FAILURE -1
#define REQ_DEV_CNT 1
#define DEV "sha"
#define CLS_NAME_DEV "cls_sha"
#define DEV_NAME_DEV "dev_sha"

#define M_WRITE _IOW('a', 'a', int32_t*)
#define M_READ _IOR('b', 'b', int32_t*)

static int32_t value;
dev_t dev_num = 0;
static struct class *cls_sha;
static struct cdev m_cdev;

static int __init dinit(void);
static void __exit dexit(void);
static long m_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static struct file_operations my_fops =
{
    .unlocked_ioctl = m_ioctl,
};

static long m_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    u_long ret;
    switch (cmd) {
        case M_WRITE:
            ret = copy_from_user(&value, (int32_t *) arg, sizeof(value));
            if (ret < 0) {
                pr_err("Unable to copy_from_user\n");
                return ret;
            }
            pr_info("M_WRITE succeeded: %d\n", value);
            break;
        case M_READ:
            ret = copy_to_user((int32_t *) arg, &value, sizeof(value));
            if (ret < 0) {
                pr_err("Unable to copy_to_user\n");
                return ret;
            }
            pr_info("M_READ succeeded: %d\n", value);
            break;
        default:
            pr_info("Invalid command\n");
            break;
    }
    return ret;
}

static int __init dinit(void)
{
    int ret;

    pr_info("%s: dinit\n", KBUILD_MODNAME);

    ret = alloc_chrdev_region(&dev_num, FIRST_MINOR, REQ_DEV_CNT, DEV);
    if (ret < 0) {
        pr_err("Unable to allocate major no\n");
        return -ENOMEM;
    }
    pr_info("Major no allocated successfully\n");
    pr_info("Major & Minor for use: %d %d\n", MAJOR(dev_num), MINOR(dev_num));

    cdev_init(&m_cdev, &my_fops);
    if ((cdev_add(&m_cdev, dev_num, REQ_DEV_CNT)) < 0) {
        pr_err("Requested cdev_add failed\n");
        goto cleanup_cdev_add;
    }
    pr_info("cdev_add succeeded\n");

    cls_sha = class_create(THIS_MODULE, CLS_NAME_DEV);
    if (IS_ERR(cls_sha)) {
        pr_err("Not able to create class\n");
        goto cleanup_class_create_failure;
    }
    pr_info("cls_sha is created successfully\n");

    if (IS_ERR(device_create(cls_sha, NULL, dev_num, NULL, DEV_NAME_DEV))) {
        pr_err("Failed to create device\n");
        goto cleanup_device_create_failure;
    }
    pr_info("device is created successfully\n");

    pr_info("Module installed successfully\n");
    return EXIT_SUCCESS;

cleanup_device_create_failure:
    class_destroy(cls_sha);
cleanup_class_create_failure:
    cdev_del(&m_cdev);
cleanup_cdev_add:
    unregister_chrdev_region(dev_num, REQ_DEV_CNT);
    return EXIT_FAILURE;
}

static void __exit dexit(void)
{
    device_destroy(cls_sha, dev_num);
    class_destroy(cls_sha);
    cdev_del(&m_cdev);
    unregister_chrdev_region(dev_num, REQ_DEV_CNT);
    pr_info("Successfully:\n\
            destroyed device\n\
            destroyed class\n\
            deleted cdev\n\
            deallocated chrdev region\n");
    pr_info("%s: dexit\n", KBUILD_MODNAME);
}

module_init(dinit);
module_exit(dexit);

MODULE_AUTHOR("sha");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("basic linux driver module");
MODULE_VERSION("1:0.8" );