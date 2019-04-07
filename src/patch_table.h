/*
	Downgrade Launcher -> patch_table.h -> Provide API documentation and definitions for the table patching
	by Davee
	
	01/01/2011
*/
#ifndef __PATCH_TABLE_H__
#define __PATCH_TABLE_H__

#include "utils.h"

typedef struct
{
	u32 devkit;
	u32 new_updater_check[5];
	u32 updater_decrypt_call;
	u32 updater_decrypt_func;
	u32 prologue_module_func;
	u32 prologue_module_call;
	u32 memlmd_call[2];
	u32 memlmd_stub[2];
} PatchTable;

PatchTable g_patch_table[] =
{
	{ 
		FIRMWARE_VERSION_631, 
		{ 0x0D78, 0x0D78, 0x0D78, 0x0D78, 0x0D78, },
		
		/* for 05g mesgled */
		0x6B44, //mesgled updater decrypt call
		0x83E8, //mesgled updater decrypt func
		
		0x8138, //prologue func
		0x705C, //prologue call
		
		{ 0x6930, 0x6954 }, //memlmd calls
		{ 0x8398, 0x8378 }, //memlmd stubs
	},
	
	{
		FIRMWARE_VERSION_635, 
		{ 0x0D78, 0x0D78, 0x0D78, 0x0D78, 0x0D78, },
		
		/* for 05g mesgled */
		0x5EB8, //mesgled updater decrypt call
		0x7B58, //mesgled updater decrypt func
		
		0x8134, //prologue func
		0x7058, //prologue call
		
		{ 0x5CA4, 0x5CC8 }, //memlmd calls
		{ 0x7B08, 0x7AE8 }, //memlmd stubs
	},
	
	{
		FIRMWARE_VERSION_638, 
		{ 0x0D78, 0x0D78, 0x0D78, 0x0D78, 0x0D78, },
		
		/* for 05g mesgled */
		0x5EB8, //mesgled updater decrypt call
		0x7B58, //mesgled updater decrypt func
		
		0x8134, //prologue func
		0x7058, //prologue call
		
		{ 0x5CA4, 0x5CC8 }, //memlmd calls
		{ 0x7B08, 0x7AE8 }, //memlmd stubs
	},
	
	{
		FIRMWARE_VERSION_639, 
		{ 0x0D78, 0x0D78, 0x0D78, 0x0D78, 0x0D78, },
		
		/* for 05g mesgled */
		0x5EB8, //mesgled updater decrypt call
		0x7B58, //mesgled updater decrypt func
		
		0x8130, //prologue func
		0x7054, //prologue call
		
		{ 0x5CA4, 0x5CC8 }, //memlmd calls
		{ 0x7B08, 0x7AE8 }, //memlmd stubs
	},
	
	{
		FIRMWARE_VERSION_660, 
		{ 0x0B24, 0x0B24, 0x0B24, 0x0B24, 0x0B24, },
		
		/* for 05g mesgled */
		0x5B84, //mesgled updater decrypt call
		0x78AC, //mesgled updater decrypt func
		
		0x8124, //prologue func
		0x7048, //prologue call
		
		{ 0x5970, 0x5994 }, //memlmd calls
		{ 0x783C, 0x7824 }, //memlmd stubs
	},
};

#define PATCH_TABLE_ADDR_START	(0x88FC0000)

#endif /* __PATCH_TABLE_H__ */
