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
	{ 0x41086e, "module_layout" },
	{ 0x392948ae, "xt_unregister_target" },
	{ 0xe90dcae0, "__request_module" },
	{ 0xc288f8ce, "malloc_sizes" },
	{ 0x1effccad, "ip_route_me_harder" },
	{ 0x8ad2eadd, "xt_register_target" },
	{ 0x884f145, "kmem_cache_alloc_trace" },
	{ 0x37a0cba, "kfree" },
	{ 0xf5b53ea, "skb_make_writable" },
	{ 0xddbfebf7, "xt_find_match" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "473B0A8E9375FC7D4A2D72E");

static const struct rheldata _rheldata __used
__attribute__((section(".rheldata"))) = {
	.rhel_major = 6,
	.rhel_minor = 6,
};
