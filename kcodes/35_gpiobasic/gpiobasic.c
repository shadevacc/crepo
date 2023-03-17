#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/err.h>
#include <linux/err.h>
#include <linux/gpio.h>

#define EXIT_SUCCESS 0
#define FIRST_MINOR 0
#define EXIT_FAILURE -1
#define REQ_DEV_CNT 1
#define DEV "sha"
#define CLS_NAME_DEV "cls_sha"
#define DEV_NAME_DEV "dev_sha"
#define GPIO_21     (21)

dev_t dev_num = 0;
static struct class *cls_sha;
static struct cdev mcdev;

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
    .read = mread,
    .write = mwrite,
    .release = mrelease,
};

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
    uint8_t m_gpio_state;

    m_gpio_state = gpio_get_value(GPIO_21);
    len = 1;

    if (copy_to_user(buf, &m_gpio_state, len) > 1) {
        pr_err("Not all bytes have been copied to user\n");
    }

    pr_info("%s: reading file GPIO_21: %d\n", __func__, m_gpio_state);
    return EXIT_SUCCESS;
}

static ssize_t mwrite(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
    uint8_t m_kbuf[10] = {0};

    pr_info("%s: writing into file", __func__);
    if (copy_from_user(m_kbuf, buf, len) > 0) {
        pr_err("Not all bytes are copied from user\n");
    }

    if (m_kbuf[0] == 1) {
        // Set the GPIO pin value to 1
        gpio_set_value(GPIO_21, 1);
    } else if (m_kbuf[0] == 0) {
        // Set the GPIO pin value to 0
        gpio_set_value(GPIO_21, 0);
    } else {
        pr_err("U can only set value to either 0 or 1\n");
    }
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

    cdev_init(&mcdev, &mfops);
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

    if (gpio_is_valid(GPIO_21) == false) {
        pr_err("GPIO_21 is not valid\n");
        goto cleanup_gpioisvalid;
    }
    pr_info("GPIO_21 is valid\n");

    if (gpio_request(GPIO_21, "GPIO_21") < 0) {
        pr_err("GPIO_21 request failed\n");
        goto cleanup_gpiorequest;
    }
    pr_info("GPIO_21 request successful\n");

    gpio_direction_output(GPIO_21, 0);
    pr_info("GPIO_21 direction is set to output mode\n");

    gpio_export(GPIO_21, false);
    pr_info("GPIO_21 export is successful\n");

    pr_info("Module installed successfully\n");
    return EXIT_SUCCESS;

cleanup_gpiorequest:
    gpio_free(GPIO_21);
cleanup_gpioisvalid:
    device_destroy(cls_sha, dev_num);
cleanup_device_create_failure:
    class_destroy(cls_sha);
cleanup_class_create_failure:
    cdev_del(&mcdev);
cleanup_cdev_add:
    unregister_chrdev_region(dev_num, REQ_DEV_CNT);
    return EXIT_FAILURE;
}

static void __exit dexit(void)
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

module_init(dinit);
module_exit(dexit);

MODULE_AUTHOR("sha");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("basic linux driver module");
MODULE_VERSION("1:0.5" );