#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xd68aeba2, "module_layout" },
	{ 0x4495cd86, "kthread_stop" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x4d73c34d, "cdev_del" },
	{ 0x1a0ed02a, "class_destroy" },
	{ 0xc55d4267, "device_destroy" },
	{ 0x83aee92a, "wake_up_process" },
	{ 0x97f1c356, "kthread_create_on_node" },
	{ 0x5042a054, "device_create" },
	{ 0x3cb885da, "__class_create" },
	{ 0xbf119f97, "cdev_add" },
	{ 0xecf6c37c, "cdev_init" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xf9a482f9, "msleep" },
	{ 0xb3f7646e, "kthread_should_stop" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xc5850110, "printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "126687410CFBB1AEAFC971E");
