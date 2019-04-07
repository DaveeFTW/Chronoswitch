/*
	Downgrade Control -> patch_table.h -> Provide API documentation and definitions for the table patching
	by Davee
	
	01/01/2011
*/
#ifndef __PATCH_TABLE_H__
#define __PATCH_TABLE_H__

typedef struct
{
	u32 devkit;
	u32 new_updater_check[5];
	u32 updater_decrypt_call;
	u32 updater_decrypt_func;
/*	u32 new_updater_keys;
	u32 new_updater_t3_arg;
	u32 new_updater_seed;
	u32 new_updater_mode;
	u32 new_updater_stack_arg;*/
	u32 prologue_module_func;
	u32 prologue_module_call;
	u32 memlmd_call[2];
	u32 memlmd_stub[2];
} PatchTable;

#define PATCH_TABLE_ADDR_START	(0x88FC0000)

extern PatchTable g_patch_table;
int CopyPatchTable(PatchTable *dst_table, void *_src_table, u32 devkit);

#endif /* __PATCH_TABLE_H__ */
