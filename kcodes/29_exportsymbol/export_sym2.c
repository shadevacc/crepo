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
#define DEV "sha2"
#define CLS_NAME_DEV "cls_sha2"
#define DEV_NAME_DEV "dev_sha2"

dev_t dev_num = 0;
static struct class *cls_sha;
static struct cdev mcdev;
extern int mshared_var;

static int __init dinit2(void);
static void __exit dexit2(void);

static int mopen2(struct inode *inode, struct file *file);
static int mrelease2(struct inode *inode, struct file *file);
static ssize_t mread2(struct file *file, char __user *buf, size_t len, loff_t *off);
static ssize_t mwrite2(struct file *file, const char __user *buf, size_t len, loff_t *off);
extern void mshared_func(void);

static struct file_operations mfops2 =
{
    .owner = THIS_MODULE,
    .open = mopen2,
    .read = mread2,
    .write = mwrite2,
    .release = mrelease2,
};

static int mopen2(struct inode *inode, struct file *file)
{
    pr_info("%s: opened file\n", __func__);
    return EXIT_SUCCESS;
}

static int mrelease2(struct inode *inode, struct file *file)
{
    pr_info("%s: closed file\n", __func__);
    return EXIT_SUCCESS;
}

static ssize_t mread2(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    pr_info("%s: reading file", __func__);
    mshared_func();
    pr_info("%s: %s mshared_var:%d \n", KBUILD_MODNAME, __func__, mshared_var);
    pr_info("Reading done...\n");
    return EXIT_SUCCESS;
}

static ssize_t mwrite2(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
    pr_info("%s: writing into file", __func__);
    return len;
}

static int __init dinit2(void)
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

    cdev_init(&mcdev, &mfops2);
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

static void __exit dexit2(void)
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

module_init(dinit2);
module_exit(dexit2);

MODULE_AUTHOR("sha");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("basic linux driver module");
MODULE_VERSION("1:0.29.b" );