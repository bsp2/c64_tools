#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x87a425f9, "module_layout" },
	{ 0x5bf45f02, "cdev_alloc" },
	{ 0x19607e57, "cdev_del" },
	{ 0xf7747067, "cdev_init" },
	{ 0xfbc74f64, "__copy_from_user" },
	{ 0x9ec2cc8c, "clk_enable" },
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x4119b4ac, "arm_dma_ops" },
	{ 0x1074c970, "clk_disable" },
	{ 0xab6b1d34, "device_destroy" },
	{ 0x2e1ca751, "clk_put" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0x62b72b0d, "mutex_unlock" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0xf6288e02, "__init_waitqueue_head" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x899c0c0a, "dev_err" },
	{ 0xdc798d37, "__mutex_init" },
	{ 0x27e1a049, "printk" },
	{ 0x42224298, "sscanf" },
	{ 0xe16b893b, "mutex_lock" },
	{ 0x61fdb36a, "device_create" },
	{ 0xd6b8e852, "request_threaded_irq" },
	{ 0x2872b5c9, "platform_driver_register" },
	{ 0xe36ad500, "cdev_add" },
	{ 0x5643ae13, "_dev_info" },
	{ 0xe9ce8b95, "omap_ioremap" },
	{ 0x15331242, "omap_iounmap" },
	{ 0x34fe8833, "clk_get" },
	{ 0xb9e52429, "__wake_up" },
	{ 0x23f7ee8d, "cpu_cache" },
	{ 0xab1bbfa8, "remap_pfn_range" },
	{ 0xf7b4c997, "class_destroy" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x94de9769, "dsp_power_notify" },
	{ 0x4845e200, "platform_driver_unregister" },
	{ 0x7014dd29, "__class_create" },
	{ 0x29537c9e, "alloc_chrdev_region" },
	{ 0xf20dabd8, "free_irq" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "4F934424741D3C540AC7DD2");
