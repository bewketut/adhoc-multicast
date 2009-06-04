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
	{ 0x168fd31b, "struct_module" },
	{ 0xfb309644, "xtnu_register_target" },
	{ 0x79aa04a2, "get_random_bytes" },
	{ 0x5a34a45c, "__kmalloc" },
	{ 0x2455c4db, "kmem_cache_alloc" },
	{ 0x7aa5914a, "malloc_sizes" },
	{ 0x7d11c268, "jiffies" },
	{ 0x25aae0a, "xtnu_unregister_target" },
	{ 0x37a0cba, "kfree" },
	{ 0x681ed1ed, "_spin_unlock_bh" },
	{ 0x239d4b98, "_spin_lock_bh" },
	{ 0xdd132261, "printk" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=compat_xtables";


MODULE_INFO(srcversion, "5F7E08DC95090ABCFAEE345");
