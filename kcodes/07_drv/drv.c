#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/slab.h>


#define EXIT_SUCCESS    0
#define FIRST_MINOR     0
#define EXIT_FAILURE    -1
#define REQ_DEV_CNT     1
#define MEM_SIZE        1024
#define DEV             "sha"
#define CLS_NAME_DEV    "cls_sha"
#define DEV_NAME_DEV    "dev_sha"

dev_t dev_num = 0;
static struct class *cls_sha;
static struct cdev m_cdev;
static uint8_t *kernel_buf;

static int __init dinit(void);
static void __exit dexit(void);
static int m_open(struct inode *inode, struct file *file);
static int m_release(struct inode *inode, struct file *file);
static ssize_t m_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t m_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);

static struct file_operations my_fops =
{
    .owner = THIS_MODULE,
    .open = m_open,
    .release = m_release,
    .read = m_read,
    .write = m_write,
};

static int m_open(struct inode *inode, struct file *file)
{
    pr_info("Opened file\n");
    return EXIT_SUCCESS;
}

static int m_release(struct inode *inode, struct file *file)
{
    pr_info("Closed file\n");
    return EXIT_SUCCESS;
}

static ssize_t m_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    u_long ret;
    ret = copy_to_user(buf, kernel_buf, MEM_SIZE);
    if (ret < 0) {
        pr_err("Unable to copy buf to user space\n");
        return EXIT_FAILURE;
    }
    pr_info("Copied buf to user space\n");
    return MEM_SIZE;
}

static ssize_t m_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    u_long ret;
    ret = copy_from_user(kernel_buf, buf, len);
    if (ret < 0) {
        pr_info("Unable to copy buf to kernel space\n");
        return EXIT_FAILURE;
    }
    pr_info("Copied buf to kernel space\n");
    return len;
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
        pr_err("Error while adding cdev to kernel\n");
        goto cleanup_cdev_add;
    }
    pr_info("Successfully added cdev device to kernel tree\n");

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
    kernel_buf = kmalloc(MEM_SIZE, GFP_KERNEL);
    if (!kernel_buf) {
        pr_err("Unable to get mem for buffer\n");
        return -ENOMEM;
    }
    pr_info("Memory allocated for buf at address %p\n", kernel_buf);

    strcpy(kernel_buf, "Hello world");
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
    kfree(kernel_buf);
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
MODULE_VERSION("1:0.5" );