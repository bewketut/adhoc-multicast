#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x168fd31b, "struct_module" },
	{ 0x2d1bf18e, "xt_unregister_target" },
	{ 0x7aa5914a, "malloc_sizes" },
	{ 0xea3a5033, "ip_route_me_harder" },
	{ 0x945d4912, "xt_register_target" },
	{ 0x2455c4db, "kmem_cache_alloc" },
	{ 0x34f4d3b3, "xt_unregister_match" },
	{ 0x37a0cba, "kfree" },
	{ 0x98adfde2, "request_module" },
	{ 0xe34d45fd, "skb_make_writable" },
	{ 0x7a88e450, "xt_find_match" },
	{ 0x3ac441db, "xt_register_match" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=x_tables";


MODULE_INFO(srcversion, "473B0A8E9375FC7D4A2D72E");
