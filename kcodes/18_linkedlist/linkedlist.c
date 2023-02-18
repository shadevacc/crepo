#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/hw_irq.h>
#include <linux/irq.h>
#include <linux/workqueue.h>
#include <linux/list.h>

#define EXIT_SUCCESS        0
#define EXIT_FAILURE        -1
#define MINOR_ZERO          0
#define REQ_ONE_DEV         1
#define IRQ_NO_ELEVEN       11
#define SHA_DEV             "sha_dev"
#define SHA_DEV_2           "sha_dev_2"
#define SHA_CLASS           "sha_class"
#define KOBJ_DIR            "kobj_dir"

static dev_t dev_num = 0;
static volatile int m_val = 0;
static struct cdev m_cdev;
static struct class *m_class;
static struct kobject *m_kobject;
static struct workqueue_struct *own_wq;

static int m_open(struct inode *inode, struct file *file);
static int m_release(struct inode *inode, struct file *file);
static ssize_t m_read(struct file *file, char __user *buf, size_t len,
                      loff_t *off);
static ssize_t m_write(struct file *file, const char __user *buf, size_t len,
                       loff_t *off);
static ssize_t m_show(struct kobject *kobj, struct kobj_attribute *attr,
                      char __user *buf);
static ssize_t m_store(struct kobject *kobj, struct kobj_attribute *attr,
                       const char __user *buf, size_t count);
static void workqueue_fn(struct work_struct *work);

static DECLARE_WORK(work, workqueue_fn);

struct my_list {
    struct list_head mlist;
    int data;
};

LIST_HEAD(mlist_head);

static struct kobj_attribute m_kobj_attr = __ATTR(m_val, 0660, m_show, m_store);

static struct file_operations m_fops =
{
    .owner = THIS_MODULE,
    .open = m_open,
    .read = m_read,
    .write = m_write,
    .release = m_release,
};

static void workqueue_fn(struct work_struct *work) {

    static struct my_list *list_element = NULL;

    list_element = kmalloc(sizeof(struct my_list), GFP_KERNEL);
    list_element->data = m_val;
    INIT_LIST_HEAD(&list_element->mlist);
    list_add_tail(&list_element->mlist, &mlist_head);

    pr_info("%s +%d: %s()\n", __FILE__, __LINE__, __func__);
}

static irqreturn_t m_irq_handler(int irq, void *dev_id)
{
    pr_info("%s +%d: %s() Interrupt occurred successfully\n",
            __FILE__, __LINE__, __func__);
    queue_work(own_wq, &work);
    return IRQ_HANDLED;
}

static int m_open(struct inode *inode, struct file *file)
{
    pr_info("%s +%d: %s()\n", __FILE__, __LINE__, __func__);
    return EXIT_SUCCESS;
}

static int m_release(struct inode *inode, struct file *file)
{
    pr_info("%s +%d: %s()\n", __FILE__, __LINE__, __func__);
    return EXIT_SUCCESS;
}

static ssize_t m_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    struct my_list *temp_list;
    static int count = 0;

    pr_info("%s +%d: %s()\n", __FILE__, __LINE__, __func__);
    list_for_each_entry(temp_list, &mlist_head, mlist) {
        pr_info("Node: %d, data: %d\n", count++, temp_list->data);
    }
    pr_info("Total nodes: %d\n", count);

    return EXIT_SUCCESS;
}

static ssize_t m_write(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
    struct irq_desc *desc;

    pr_info("%s +%d: %s()\n", __FILE__, __LINE__, __func__);
    pr_info("Issuing interrupt\n");
    desc = irq_to_desc(11);
    if (!desc) {
        return -EINVAL;
    }
    __this_cpu_write(vector_irq[59], desc);
    
    if (copy_from_user(&m_val, buf, len)) {
        pr_err("%s: copy_from_user failed\n", __func__);
    }
    //0x20 + 0x10 + 11 = 0x3B = 59
    asm("int $0x3B");

    return len;
}

static ssize_t m_show(struct kobject *kobj, struct kobj_attribute *attr,
                      char __user *buf)
{
    pr_info("%s +%d: %s()\n", __FILE__, __LINE__, __func__);
    return sprintf(buf, "%d", m_val);
}

static ssize_t m_store(struct kobject *kobj, struct kobj_attribute *attr,
                       const char __user *buf, size_t count)
{
    pr_info("%s +%d: %s()\n", __FILE__, __LINE__, __func__);
    sscanf(buf, "%d", &m_val);
    return count;
}


static int __init m_init(void);
static void __exit m_exit(void);

static int __init m_init(void)
{
    int ret;

    pr_info("%s +%d: %s() {\n", __FILE__, __LINE__, __func__);

    ret = alloc_chrdev_region(&dev_num, MINOR_ZERO, REQ_ONE_DEV, SHA_DEV);
    if (ret < 0) {
        pr_err("alloc_chardev_region failed\n");
        return -ENOMEM;
    }
    pr_info("alloc_chardev_region succeeded\n");

    cdev_init(&m_cdev, &m_fops);
    if ((cdev_add(&m_cdev, dev_num, REQ_ONE_DEV)) < 0) {
        pr_err("cdev_add failed\n");
        goto r_cdev;
    }
    pr_info("cdev_init and cdev_add succeeded\n");

    m_class = class_create(THIS_MODULE, SHA_CLASS);
    if (IS_ERR(m_class)) {
        pr_err("class_create failed\n");
        goto r_class;
    }
    pr_info("class_create succeeded\n");

    if (IS_ERR(device_create(m_class, NULL, dev_num, NULL, SHA_DEV_2))) {
        pr_err("device_create failed\n");
        goto r_device_create;
    }
    pr_info("device_create succeeded\n");

    m_kobject = kobject_create_and_add(KOBJ_DIR, kernel_kobj);
    if ((sysfs_create_file(m_kobject, &m_kobj_attr.attr)) < 0) {
        pr_err("sysfs_create failed\n");
        goto r_sysfs;
    }
    pr_info("kobject_create_and_add & sysfs_create succeeded\n");

    if (request_irq(IRQ_NO_ELEVEN, m_irq_handler, IRQF_SHARED, SHA_DEV_2, (void *)(m_irq_handler))) {
        pr_err("irq request failed\n");
        goto r_irq;
    }
    pr_info("irq request successfully registered\n");

    own_wq = create_workqueue("own_workqueue");
    pr_info("own workqueue created\n");

    return EXIT_SUCCESS;
r_irq:
    sysfs_remove_file(m_kobject, &m_kobj_attr.attr);
r_sysfs:
    kobject_put(m_kobject);
r_device_create:
    class_destroy(m_class);
r_class:
    cdev_del(&m_cdev);
r_cdev:
    unregister_chrdev_region(dev_num, REQ_ONE_DEV);
    return EXIT_SUCCESS;
}

static void __exit m_exit(void)
{
    struct my_list *cursor, *temp;

    pr_info("%s +%d: %s()\n", __FILE__, __LINE__, __func__);

    list_for_each_entry_safe(cursor, temp, &mlist_head, mlist) {
        list_del(&cursor->mlist);
        kfree(cursor);
    }
    destroy_workqueue(own_wq);
    free_irq(IRQ_NO_ELEVEN, (void *)(m_irq_handler));
    sysfs_remove_file(m_kobject, &m_kobj_attr.attr);
    kobject_put(m_kobject);
    device_destroy(m_class, dev_num);
    class_destroy(m_class);
    cdev_del(&m_cdev);
    unregister_chrdev_region(dev_num, REQ_ONE_DEV);

    pr_info("Successfully\n\
            freed irq_eleven\n\
            removed sysfs file\n\
            removed sysfs directory\n\
            destroyed device 2\n\
            destroyed class\n\
            deleted cdev\n\
            unregistered device number\n");
}

module_init(m_init);
module_exit(m_exit);

MODULE_AUTHOR("sha");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Interrupt device driver");
MODULE_VERSION("1:0.18");