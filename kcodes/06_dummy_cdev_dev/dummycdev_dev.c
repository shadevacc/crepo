#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/err.h>

dev_t dev;
static struct class *cls_sha;
static struct cdev cdev_sha_mem;

#define EXIT_SUCCESS 0
#define EXIT_FAILURE -1
#define FIRST_MINOR 0
#define REQ_DEV_CNT 1
#define DEV "sha"
#define CLS_NAME_DEV "cls_sha"
#define DEV_NAME_DEV "dev_sha"
#define CDEV_DEV_NAME "cdev_sha_new"

static int __init dinit(void);
static void __exit dexit(void);
static int m_open(struct inode *inode, struct file *file);
static int m_release(struct inode *inode, struct file *file);
static ssize_t m_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t m_write(struct file *filp, const char *buf, size_t len, loff_t *off);

static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .open = m_open,
    .read = m_read,
    .write = m_write,
    .release = m_release,
};

static int m_open(struct inode *inode, struct file *file)
{
    pr_info("In mopen\n");
    return EXIT_SUCCESS;
}
static int m_release(struct inode *inode, struct file *file)
{
    pr_info("In mrelease\n");
    return EXIT_SUCCESS;
}
static ssize_t m_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    pr_info("In mread\n");
    return EXIT_SUCCESS;
}
static ssize_t m_write(struct file *filp, const char *buf, size_t len, loff_t *off)
{
    pr_info("In mwrite\n");
    return len;
}

static int __init dinit(void)
{
    int ret;

    pr_info("%s: dinit\n", KBUILD_MODNAME);
    ret = alloc_chrdev_region(&dev, FIRST_MINOR, REQ_DEV_CNT, DEV);
    if (ret < 0) {
        pr_err("Unable to allocate major no\n");
        return -ENOMEM;
    }
    pr_info("Major no allocated successfully\n");
    pr_info("Major & Minor for use: %d %d\n", MAJOR(dev), MINOR(dev));

    cdev_init(&cdev_sha_mem, &fops);
    if ((cdev_add(&cdev_sha_mem, dev, REQ_DEV_CNT)) < 0) {
        pr_err("Unable to register cdev\n");
        goto cleanup_cdev_add_failure;
    }
    cls_sha = class_create(THIS_MODULE, CLS_NAME_DEV);
    if (IS_ERR(cls_sha)) {
        pr_err("Not able to create class\n");
        goto cleanup_class_create_failure;
    }
    pr_info("cls_sha is created successfully\n");

    if (IS_ERR(device_create(cls_sha, NULL, dev, NULL, DEV_NAME_DEV))) {
        pr_err("Failed to create device\n");
        goto cleanup_device_create_failure;
    }
    pr_info("device is created successfully\n");

    pr_info("Module installed successfully\n");
    return EXIT_SUCCESS;

cleanup_device_create_failure:
    class_destroy(cls_sha);
cleanup_class_create_failure:
    cdev_del(&cdev_sha_mem);
cleanup_cdev_add_failure:
    unregister_chrdev_region(dev, REQ_DEV_CNT);
    return EXIT_FAILURE;
}

static void __exit dexit(void)
{
    device_destroy(cls_sha, dev);
    class_destroy(cls_sha);
    cdev_del(&cdev_sha_mem);
    unregister_chrdev_region(dev, REQ_DEV_CNT);
    pr_info("Successfully:\n\
            destroyed device\n\
            destroyed class\n\
            deleted cdev struct\n\
            deallocated chrdev region\n");
    pr_info("%s: dexit\n", KBUILD_MODNAME);
}

module_init(dinit);
module_exit(dexit);

MODULE_AUTHOR("sha");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("basic linux driver module");
MODULE_VERSION("1:0.6" );