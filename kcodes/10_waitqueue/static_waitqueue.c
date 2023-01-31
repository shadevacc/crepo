#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/wait.h>

#define EXIT_SUCCESS 0
#define FIRST_MINOR 0
#define EXIT_FAILURE -1
#define REQ_DEV_CNT 1
#define DEV "sha"
#define CLS_NAME_DEV "cls_sha"
#define DEV_NAME_DEV "dev_sha"
#define M_WAIT_THREAD_NAME "mwait_func"

dev_t dev_num = 0;
static int mwait_event_flag = 0;
static int read_cnt = 0;
static struct class *cls_sha;
static struct cdev mcdev;
static struct task_struct *wait_thread;
DECLARE_WAIT_QUEUE_HEAD(mwait_queue);

static int __init dinit(void);
static void __exit dexit(void);

static int mopen(struct inode *inode, struct file *file);
static int mrelease(struct inode *inode, struct file *file);
static ssize_t mread(struct file *file, char __user *buf, size_t len, loff_t *off);
static ssize_t mwrite(struct file *file, const char __user *buf, size_t len, loff_t *off);

static struct file_operations mfops =
{
    .owner = THIS_MODULE,
    .open = mopen,
    .release = mrelease,
    .read = mread,
    .write = mwrite,
};

static int wait_threadf(void *unused) {
    while(1) {
        pr_info("Before wait event interruptible\n");
        wait_event_interruptible(mwait_queue, mwait_event_flag != 0);
        pr_info("After wait event interruptible\n");
        if (mwait_event_flag == 2) {
            pr_info("Event received from exit function\n");
            return EXIT_SUCCESS;
        }
        pr_info("Event received from read function %d\n", ++read_cnt);
        mwait_event_flag = 0;
    }
    return EXIT_SUCCESS;

}

static int mopen(struct inode *inode, struct file *file)
{
    pr_info("%s: opened file\n", __func__);
    return EXIT_SUCCESS;
}

static int mrelease(struct inode *inode, struct file *file)
{
    pr_info("%s: closed file\n", __func__);
    return EXIT_SUCCESS;
}

static ssize_t mread(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    pr_info("%s: read called", __func__);
    mwait_event_flag = 1;
    wake_up_interruptible(&mwait_queue);
    return EXIT_SUCCESS;
}

static ssize_t mwrite(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
    pr_info("%s: writing data to device file\n", __func__);
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

    cls_sha = class_create(THIS_MODULE, CLS_NAME_DEV);
    if (IS_ERR(cls_sha)) {
        pr_err("Not able to create class\n");
        goto cleanup_class_create_failure;
    }
    pr_info("cls_sha is created successfully\n");

    cdev_init(&mcdev, &mfops);
    if ((cdev_add(&mcdev, dev_num, REQ_DEV_CNT)) < 0) {
        pr_err("cdev_add failed\n");
        goto cleanup_cdev_add;
    }
    pr_info("cdev is created successfully\n");

    if (IS_ERR(device_create(cls_sha, NULL, dev_num, NULL, DEV_NAME_DEV))) {
        pr_err("Failed to create device\n");
        goto cleanup_device_create_failure;
    }
    pr_info("device is created successfully\n");

    wait_thread = kthread_create(wait_threadf, NULL, M_WAIT_THREAD_NAME);
    if (wait_thread) {
        pr_info("wait_thread created successfully\n");
        wake_up_process(wait_thread);
    } else {
        pr_err("Unable to create wait thread\n");
        goto cleanup_kthread_create;
    }

    pr_info("Module installed successfully\n");
    return EXIT_SUCCESS;

cleanup_kthread_create:
    device_destroy(cls_sha, dev_num);
cleanup_device_create_failure:
    class_destroy(cls_sha);
cleanup_cdev_add:
    cdev_del(&mcdev);
cleanup_class_create_failure:
    unregister_chrdev_region(dev_num, REQ_DEV_CNT);
    return EXIT_FAILURE;
}

static void __exit dexit(void)
{
    mwait_event_flag = 2;
    wake_up_interruptible(&mwait_queue);
    device_destroy(cls_sha, dev_num);
    cdev_del(&mcdev);
    class_destroy(cls_sha);
    unregister_chrdev_region(dev_num, REQ_DEV_CNT);
    pr_info("Successfully:\n\
            destroyed device\n\
            cdev deleted\n\
            destroyed class\n\
            deallocated chrdev region\n");
    pr_info("%s: dexit\n", KBUILD_MODNAME);
}

module_init(dinit);
module_exit(dexit);

MODULE_AUTHOR("sha");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("basic linux driver module");
MODULE_VERSION("1:0.10" );