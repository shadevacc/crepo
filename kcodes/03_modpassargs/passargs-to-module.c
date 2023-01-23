#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

static int __init dinit(void);
static void __exit dexit(void);

int val, cbval=1;
char *name;
int arrval[4];

module_param(val, int, S_IRUSR | S_IWUSR);
module_param(name, charp, S_IRUSR | S_IWUSR);
module_param_array(arrval, int, NULL, S_IRUSR | S_IWUSR);

int notifyparam_change(const char *val, const struct kernel_param *kp)
{
    int res = param_set_int(val, kp);
    if (res == 0) {
        pr_info("cbval changed to %d \n", cbval);
        return 0;
    }
    pr_info("error occurred\n");
    return res;
}

const struct kernel_param_ops my_param_ops =
{
    .set = &notifyparam_change,
    .get = &param_get_int,
};

module_param_cb(cbval, &my_param_ops, &cbval, S_IRUSR | S_IWUSR);

static int __init dinit(void)
{
    int i;
    pr_info("%s: dinit\n", KBUILD_MODNAME);
    pr_info("val: %d\n", val);
    pr_info("name: %s\n", name);
    pr_info("cbval: %d\n", cbval);
    for (i=0; i<(sizeof(arrval)/sizeof(int)); i++)
        pr_info("arrval[%d]: %d\n", i, arrval[i]);
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
MODULE_VERSION("1:0.3" );