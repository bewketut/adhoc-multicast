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
	{ 0x686f3a13, "module_layout" },
	{ 0x8ccc1d8b, "xt_register_target" },
	{ 0x79aa04a2, "get_random_bytes" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0x46a3f5e5, "kmem_cache_alloc_trace" },
	{ 0xb3bbff72, "malloc_sizes" },
	{ 0x7d11c268, "jiffies" },
	{ 0x7c78aaa4, "xt_unregister_target" },
	{ 0x37a0cba, "kfree" },
	{ 0x50eedeb8, "printk" },
	{ 0x6223cafb, "_raw_spin_unlock_bh" },
	{ 0x87a45ee9, "_raw_spin_lock_bh" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "4C3170AA893EE56D63CAD59");
