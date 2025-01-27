#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

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
	{ 0x37a0cba, "kfree" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x122c3a7e, "_printk" },
	{ 0xddf241a2, "init_task" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0x4c03a563, "random_kmalloc_seed" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x1d199deb, "kmalloc_trace" },
	{ 0x24980310, "kmalloc_caches" },
	{ 0x6ad2b3e, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "25A2E5EDAF24315292E7D01");
