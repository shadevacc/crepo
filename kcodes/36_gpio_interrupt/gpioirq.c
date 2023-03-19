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
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <asm-generic/gpio.h>
/*
** EN_DEBOUNCE - Raspberry pi doesn't support DEBOUNCE so in case
** this macro is enabled, we will have a software hack to stop
** spurious interrupts from triggering.
*/
#define EN_DEBOUNCE
#ifdef EN_DEBOUNCE
#include <linux/jiffies.h>
unsigned long volatile jiffies = 0;
unsigned long old_jiffie;
#endif

#define EXIT_SUCCESS    0
#define FIRST_MINOR     0
#define EXIT_FAILURE    -1
#define REQ_DEV_CNT     1
#define DEV             "sha"
#define CLS_NAME_DEV    "cls_sha"
#define DEV_NAME_DEV    "dev_sha"
#define GPIO_21_OUT      21
#define GPIO_25_IN       25

unsigned int led_toggle = 0;
unsigned int GPIO_irqNum;


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

static irqreturn_t gpio_irq_handler(int irq, void *devid)
{
    static unsigned long flags = 0;
#ifdef EN_DEBOUNCE
    unsigned long diff = jiffies - old_jiffie;
    if (diff < 20)
        return IRQ_HANDLED;
    old_jiffie = jiffies;
#endif
    local_irq_save(flags);
    led_toggle = ( 0x01 ^ led_toggle);
    gpio_set_value(GPIO_21_OUT, led_toggle);
    pr_info("IRQ occured GPIO OUT %d\n", GPIO_21_OUT);
    local_irq_restore(flags);
    return IRQ_HANDLED;
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
    uint8_t m_gpio_state;

    m_gpio_state = gpio_get_value(GPIO_21_OUT);
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
        gpio_set_value(GPIO_21_OUT, 1);
    } else if (m_kbuf[0] == 0) {
        // Set the GPIO pin value to 0
        gpio_set_value(GPIO_21_OUT, 0);
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

    if (gpio_is_valid(GPIO_21_OUT) == false) {
        pr_err("GPIO OUT %d is not valid\n", GPIO_21_OUT);
        goto cleanup_validgpio21;
    }
    pr_info("GPIO OUT %d is valid\n", GPIO_21_OUT);

    if (gpio_request(GPIO_21_OUT, "GPIO_21") < 0) {
        pr_err("GPIO OUT %d request failed\n", GPIO_21_OUT);
        goto cleanup_reqgpio21;
    }
    pr_info("GPIO OUT %d requested successfully\n", GPIO_21_OUT);

    gpio_direction_output(GPIO_21_OUT, 0);

    if (gpio_is_valid(GPIO_25_IN) == false) {
        pr_err("GPIO IN %d is invalid\n", GPIO_25_IN);
        goto cleanup_validgpio25;
    }
    pr_info("GPIO IN %d is valid\n", GPIO_25_IN);

    if (gpio_request(GPIO_25_IN, "GPIO_25") < 0) {
        pr_err("GPIO IN %d request failed\n", GPIO_25_IN);
        goto cleanup_reqgpio25;
    }
    pr_info("GPIO IN %d requested successfully\n", GPIO_25_IN);

    gpio_direction_input(GPIO_25_IN);

#ifndef EN_DEBOUNCE
    if (gpio_set_debounce(GPIO_25_IN, 200) < 0) {
        pr_err("Error setting GPIO IN %d debounce\n", GPIO_25_IN);
        goto cleanup_gpiosetdebounce;
    }
    pr_info("Successfully set GPIO IN %d debounce\n", GPIO_25_IN);
#endif
    // gpio_export(GPIO_21, false);
    // pr_info("GPIO_21 export is successful\n");

    GPIO_irqNum = gpio_to_irq(GPIO_25_IN);
    pr_info("GPIO_25_IN got IRQ %d\n", GPIO_irqNum);

    if (request_irq(GPIO_irqNum,
                    (void *)gpio_irq_handler,
                    IRQF_TRIGGER_FALLING,
                    "gpio_dev",
                    NULL)) {
        pr_err("IRQ %d request for GPIO IN %d failed\n", GPIO_irqNum, GPIO_25_IN);
        goto cleanup_gpioreqirq;
    }
    pr_info("Module installed successfully\n");
    return EXIT_SUCCESS;

cleanup_gpioreqirq:
#ifndef EN_DEBOUNCE
cleanup_gpiosetdebounce:
#endif
cleanup_reqgpio25:
    gpio_free(GPIO_25_IN);
cleanup_validgpio25:
cleanup_reqgpio21:
    gpio_free(GPIO_21_OUT);
cleanup_validgpio21:
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
    free_irq(GPIO_irqNum, NULL);
    gpio_free(GPIO_25_IN); 
    gpio_free(GPIO_21_OUT); 
    device_destroy(cls_sha, dev_num);
    class_destroy(cls_sha);
    cdev_del(&mcdev);
    unregister_chrdev_region(dev_num, REQ_DEV_CNT);
    pr_info("Successfully:\n"
            "IRQ free is done\n"
            "GPIO_25_IN free is done\n"
            "GPIO_21_OUT free is done\n"
            "destroyed device\n"
            "destroyed class\n"
            "deleted cdev\n"
            "deallocated chrdev region\n");
    pr_info("%s: dexit\n", KBUILD_MODNAME);
}

module_init(dinit);
module_exit(dexit);

MODULE_AUTHOR("sha");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("GPIO device driver module catching vibrations to turn on/off led");
MODULE_VERSION("1:0.36" );