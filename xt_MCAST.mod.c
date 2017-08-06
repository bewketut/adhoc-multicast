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
	{ 0x41086e, "module_layout" },
	{ 0xffe3552a, "xtnu_register_target" },
	{ 0x79aa04a2, "get_random_bytes" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0x884f145, "kmem_cache_alloc_trace" },
	{ 0xc288f8ce, "malloc_sizes" },
	{ 0x7d11c268, "jiffies" },
	{ 0x49db048d, "xtnu_unregister_target" },
	{ 0x37a0cba, "kfree" },
	{ 0xb72397d5, "printk" },
	{ 0x3aa1dbcf, "_spin_unlock_bh" },
	{ 0x93cbd1ec, "_spin_lock_bh" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=compat_xtables";


MODULE_INFO(srcversion, "5F7E08DC95090ABCFAEE345");

static const struct rheldata _rheldata __used
__attribute__((section(".rheldata"))) = {
	.rhel_major = 6,
	.rhel_minor = 6,
};
