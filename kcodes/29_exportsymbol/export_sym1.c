#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>

#define EXIT_SUCCESS 0
#define FIRST_MINOR 0
#define EXIT_FAILURE -1
#define REQ_DEV_CNT 1
#define DEV "sha1"
#define CLS_NAME_DEV "cls_sha1"
#define DEV_NAME_DEV "dev_sha1"

dev_t dev_num = 0;
static struct class *cls_sha;
static struct cdev mcdev;
int mshared_var = 0;

static int __init dinit1(void);
static void __exit dexit1(void);

static int mopen1(struct inode *inode, struct file *file);
static int mrelease1(struct inode *inode, struct file *file);
static ssize_t mread1(struct file *file, char __user *buf, size_t len, loff_t *off);
static ssize_t mwrite1(struct file *file, const char __user *buf, size_t len, loff_t *off);

static struct file_operations mfops1 =
{
    .owner = THIS_MODULE,
    .open = mopen1,
    .read = mread1,
    .write = mwrite1,
    .release = mrelease1,
};

void mshared_func(void) {
    pr_info("%s: %s mshared_var:%d \n", KBUILD_MODNAME, __func__, mshared_var++);
}
EXPORT_SYMBOL(mshared_func);
EXPORT_SYMBOL(mshared_var);

static int mopen1(struct inode *inode, struct file *file)
{
    pr_info("%s: opened file\n", __func__);
    return EXIT_SUCCESS;
}

static int mrelease1(struct inode *inode, struct file *file)
{
    pr_info("%s: closed file\n", __func__);
    return EXIT_SUCCESS;
}

static ssize_t mread1(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    pr_info("%s: reading file", __func__);
    return EXIT_SUCCESS;
}

static ssize_t mwrite1(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
    pr_info("%s: writing into file", __func__);
    return len;
}


static int __init dinit1(void)
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

    cdev_init(&mcdev, &mfops1);
    if ((cdev_add(&mcdev, dev_num, REQ_DEV_CNT)) < 0) {
        pr_err("cdev_add failed\n");
        goto cleanup_cdev_add;
    }

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
    cdev_del(&mcdev);
cleanup_cdev_add:
    unregister_chrdev_region(dev_num, REQ_DEV_CNT);
    return EXIT_FAILURE;
}

static void __exit dexit1(void)
{
    device_destroy(cls_sha, dev_num);
    class_destroy(cls_sha);
    cdev_del(&mcdev);
    unregister_chrdev_region(dev_num, REQ_DEV_CNT);
    pr_info("Successfully:\n\
            destroyed device\n\
            destroyed class\n\
            deleted cdev\n\
            deallocated chrdev region\n");
    pr_info("%s: dexit\n", KBUILD_MODNAME);
}

module_init(dinit1);
module_exit(dexit1);

MODULE_AUTHOR("sha");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("basic linux driver module 1");
MODULE_VERSION("1:0.29.a" );