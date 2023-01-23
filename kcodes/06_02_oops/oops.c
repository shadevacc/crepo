#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

static noinline void do_oops(void)
{
    *(int*)0x42 = 'a';
}

static int so2_oops_init(void)
{
    pr_info("oops_init\n");
    do_oops();

    return 0;
}

static void so2_oops_exit(void)
{
    pr_info("oops exit\n");
}

module_init(so2_oops_init);
module_exit(so2_oops_exit);

MODULE_AUTHOR("sha");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("oops linux driver module");
MODULE_VERSION("1:0.6.2" );