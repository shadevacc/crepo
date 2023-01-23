#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/moduleparam.h>

#define FIRST_MINOR 0x0
#define NO_OF_DEV_CNT 0x1
#define DEV_FILE "sha"

static int __init dinit(void);
static void __exit dexit(void);

dev_t dev = 0;

static int __init dinit(void)
{
    pr_info("%s: dinit\n", KBUILD_MODNAME);

    dev = alloc_chrdev_region(&dev, FIRST_MINOR, NO_OF_DEV_CNT, DEV_FILE);
    if (dev < 0) {
        pr_err("Unable to allocate major no\n");
        return -ENOMEM;
    }
    pr_info("Major no allocated successfully\n");
    pr_info("Major & Minor for use: %d %d\n", MAJOR(dev), MINOR(dev));
    return 0;
}

static void __exit dexit(void)
{
    unregister_chrdev_region(dev, NO_OF_DEV_CNT);
    pr_info("Successfully deallocated chrdev region\n");
    pr_info("%s: dexit\n", KBUILD_MODNAME);
}

module_init(dinit);
module_exit(dexit);

MODULE_AUTHOR("sha");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("basic linux driver module");
MODULE_VERSION("1:0.4" );