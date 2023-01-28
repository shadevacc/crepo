#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/err.h>

#define EXIT_SUCCESS 0
#define FIRST_MINOR 0
#define EXIT_FAILURE -1
#define REQ_DEV_CNT 1
#define DEV "sha"
#define CLS_NAME_DEV "cls_sha"
#define DEV_NAME_DEV "dev_sha"
#define SHA_PROC_DIR "sha_proc_dir"
#define SHA_PROC_DIR_ENTRY "sha_proc_dir_entry"

dev_t dev_num = 0;
static char k_write[20]="try_pro_array\n";
static struct class *cls_sha;
static struct proc_dir_entry *proc_dir;

static int __init dinit(void);
static void __exit dexit(void);

static int open_proc(struct inode *inode, struct file *file);
static int release_proc(struct inode *inode, struct file *file);
static long int read_proc(struct file *file, char __user *buf, long unsigned len, long long int *off);
static long int write_proc(struct file *file, const char __user *buf, long unsigned len, long long int *off);

static struct proc_ops proc_fops =
{
    .proc_open = open_proc,
    .proc_release = release_proc,
    .proc_read = read_proc,
    .proc_write = write_proc,
};

static int open_proc(struct inode *inode, struct file *file)
{
    pr_info("%s: File opened\n", __func__);
    return EXIT_SUCCESS;
}

static int release_proc(struct inode *inode, struct file *file)
{
    pr_info("%s: File closed\n", __func__);
    return EXIT_SUCCESS;
}

static ssize_t read_proc(struct file *file, char __user *buf, long unsigned len, long long int *off)
{
    u_long ret;

    pr_info("%s\n", __func__);

    if (len) {
        len = 0;
    } else {
        len = 1;
        return EXIT_SUCCESS;
    }

    ret = copy_to_user(buf, k_write, 20);
    if (ret < 0) {
        pr_info("copy_to_user failed\n");
        return ret;
    }
    pr_info("copy_to_user succeeded\n");
    return 20;
}

static ssize_t write_proc(struct file *file, const char __user *buf, long unsigned len, long long int *off)
{
    u_long ret;

    ret = copy_from_user(k_write, buf, len);
    if (ret < 0) {
        pr_err("copy_from_user failed\n");
        return ret;
    }
    pr_info("%s: copy_from_user succeeded \n", __func__);
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

    if (IS_ERR(device_create(cls_sha, NULL, dev_num, NULL, DEV_NAME_DEV))) {
        pr_err("Failed to create device\n");
        goto cleanup_device_create_failure;
    }
    pr_info("device is created successfully\n");

    proc_dir = proc_mkdir(SHA_PROC_DIR, NULL);
    if (proc_dir == NULL) {
        pr_err("creating sha_proc_dir failed\n");
        goto cleanup_proc_mkdir;
    }
    pr_info("sha_proc_dir created successfully\n");
    proc_create(SHA_PROC_DIR_ENTRY, 0666, proc_dir, &proc_fops);
    pr_info("sha_proc_dir_entry created successfully\n");
    pr_info("Module installed successfully\n");
    return EXIT_SUCCESS;
cleanup_proc_mkdir:
    proc_remove(proc_dir);
cleanup_device_create_failure:
    class_destroy(cls_sha);
cleanup_class_create_failure:
    unregister_chrdev_region(dev_num, REQ_DEV_CNT);
    return EXIT_FAILURE;
}

static void __exit dexit(void)
{
    proc_remove(proc_dir);
    device_destroy(cls_sha, dev_num);
    class_destroy(cls_sha);
    unregister_chrdev_region(dev_num, REQ_DEV_CNT);
    pr_info("Successfully:\n\
            proc device removed\n\
            destroyed device\n\
            destroyed class\n\
            deallocated chrdev region\n");
    pr_info("%s: dexit\n", KBUILD_MODNAME);
}

module_init(dinit);
module_exit(dexit);

MODULE_AUTHOR("sha");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("basic linux driver module");
MODULE_VERSION("1:0.9" );