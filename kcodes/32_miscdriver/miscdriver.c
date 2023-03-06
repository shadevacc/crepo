#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>

#define EXIT_SUCCESS        0
#define EXIT_FAILURE        1

static int __init misc_init(void);
static void __exit misc_exit(void);
static int m_open(struct inode *inode, struct file *file);
static int m_release(struct inode *inode, struct file *file);
static ssize_t m_read(struct file *file, char __user *buf, size_t len,
                      loff_t *offset);
static ssize_t m_write(struct file *file, const char __user *buf,
                       size_t len, loff_t *offset);

static int m_open(struct inode *inode, struct file *file)
{
    pr_info("%s %s\n", KBUILD_MODNAME, __func__);
    return EXIT_SUCCESS;
}

static int m_release(struct inode *inode, struct file *file)
{
    pr_info("%s %s\n", KBUILD_MODNAME, __func__);
    return EXIT_SUCCESS;
}

static ssize_t m_read(struct file *file, char __user *buf, size_t len,
                      loff_t *offset)
{
    pr_info("%s %s\n", KBUILD_MODNAME, __func__);
    return EXIT_SUCCESS;
}

static ssize_t m_write(struct file *file, const char __user *buf,
                       size_t len, loff_t *offset)
{
    pr_info("%s %s\n", KBUILD_MODNAME, __func__);
    return len;
}
static struct file_operations m_fops = {
    .owner = THIS_MODULE,
    .open = m_open,
    .release = m_release,
    .read = m_read,
    .write = m_write,
    .llseek = no_llseek,
};

static struct miscdevice m_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "m_misc_dev",
    .fops = &m_fops,
};

static void __exit misc_exit(void) {
    misc_deregister(&m_miscdev);
    pr_info("Successfully deregistered misc device driver\n");
}
static int __init misc_init(void) {
    int m_err;

    m_err = misc_register(&m_miscdev);
    if (m_err) {
        pr_err("Registering misc device failed\n");
        return EXIT_FAILURE;
    }
    pr_info("Successfully registered misc device driver\n");
    return EXIT_SUCCESS;
}

module_init(misc_init);
module_exit(misc_exit);

MODULE_LICENSE("GPL v2.0");
MODULE_AUTHOR("sha");
MODULE_DESCRIPTION("Simple miscellaneous device driver");
MODULE_VERSION("1.0.32");