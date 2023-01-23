#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

static int __init dinit(void);
static void __exit dexit(void);

static int __init dinit(void)
{
    pr_info("%s: dinit\n", KBUILD_MODNAME);
    return 0;
}

static void __exit dexit(void)
{
    pr_info("%s: dexit\n", KBUILD_MODNAME);
}

module_init(dinit);
module_exit(dexit);

MODULE_AUTHOR("sha");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("basic linux driver module");
MODULE_VERSION("1:0.2" );