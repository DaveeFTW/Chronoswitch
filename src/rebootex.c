/*
	Downgrade Launcher -> rebootex.c -> Provide patches to the Sony reboot.bin to insert our downgrade controller
	by Davee
	
	28/12/2010
*/

#include <pspkernel.h>
#include <pspsysmem_kernel.h>

#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "rebootex.h"
#include "patch_table.h"
#include "kernel_land.h"

#include "downgrade_ctrl.h"
#include "downgrade660_ctrl.h"

/* global variables */
u32 g_module_seek = 0;
u32 g_module_opened = 0;
void *g_module = NULL;
u32 g_devkit_version = 0;

/* function pointers */
int (* sceBootLfatOpen)(char *path) = NULL;
int (* sceBootLfatRead)(void *data, int size) = NULL;
int (* sceBootLfatClose)(void) = NULL;

u32 (* sceKernelCheckPspConfig)(void *btcnf_data, u32 size, int flag) = NULL;

int (* DecryptExecutable)(void *buffer, int size, int *outsize) = NULL;
int (* VerifySigncheck)(void *buffer, int size) = NULL;

/* The Sony Reboot */
int (* sceReboot)(void *reboot_param, void *exec_param, int api, int initial_rnd) = (void *)0x88600000;

int sceBootLfatOpenPatched(char *path)
{
	/* check the path for our virtual module */
	if (strcmp(path, "/kd/strange.charm") == 0)
	{
		/* set seek to 0 and set to opened and return success */
		g_module_seek = 0;
		g_module_opened = 1;
		
		return 0;
	}

	/* return original function */
	return sceBootLfatOpen(path);
}

int sceBootLfatReadPatched(void *data, int size)
{
	/* check if the module is virtually opened */
	if (g_module_opened)
	{
		/* get the remaining size */
		u32 remain = size_downgrade_ctrl - g_module_seek;
		
		/* now do some math to calculate how much we will copy (as read is done in 32KiB chunks) */
		remain = (remain < (32 << 10)) ? (remain) : (32 << 10);
		
		/* copy over data */
		memcpy(data, downgrade_ctrl + g_module_seek, remain);
		
		/* increment the seek */
		g_module_seek += remain;
		
		/* returned copied size */
		return remain;
	}

	/* return the read data */
	return sceBootLfatRead(data, size);
}

int sceBootLfatReadPatched660(void *data, int size)
{
	/* check if the module is virtually opened */
	if (g_module_opened)
	{
		/* get the remaining size */
		u32 remain = size_downgrade660_ctrl - g_module_seek;
		
		/* now do some math to calculate how much we will copy (as read is done in 32KiB chunks) */
		remain = (remain < (32 << 10)) ? (remain) : (32 << 10);
		
		/* copy over data */
		memcpy(data, downgrade660_ctrl + g_module_seek, remain);
		
		/* increment the seek */
		g_module_seek += remain;
		
		/* returned copied size */
		return remain;
	}

	/* return the read data */
	return sceBootLfatRead(data, size);
}

int sceBootLfatClosePatched(void)
{
	/* check if the module is virtually opened */
	if (g_module_opened)
	{
		/* it is, interally close and return success */
		g_module_opened = 0;
		return 0;
	}

	/* return the original */
	return sceBootLfatClose();
}

/* BTCNF INJECION CODE BY BUBBLETUNE (bubbletune.x-fusion.co.uk) */
int InsertModuleBtcnf(char *new_mod, char *before_mod, BtcnfHeader *header, int *size, u16 flags)
{
	/* cast and declare our local variables */
	int i, j;
	ModeEntry *modes = (ModeEntry *)((u32)header + header->modestart);
	ModuleEntry *modules = (ModuleEntry *)((u32)header + header->modulestart);
	char *names = (char *)((u32)header + header->modnamestart);
	int len = strlen(new_mod) + 1;

	/* loop through the modules */
	for (i = 0; i < header->nmodules; i++)
	{
		/* if we find the module we want to insert before */
		if (memcmp(names + modules[i].stroffset, before_mod, strlen(before_mod) + 1) == 0)
		{
			/* found it! lets move the the whole section so we can fit another entry*/
			memmove(modules + i + 1, modules + i, (*size)-header->modulestart-(i*sizeof(ModuleEntry)));
			
			/* update all the header variables */
			header->modnamestart += sizeof(ModuleEntry);
			header->modnameend += sizeof(ModuleEntry);
			*size += sizeof(ModuleEntry);
			header->nmodules++;
			
			/* add the new information */
			modules[i].stroffset = header->modnameend-header->modnamestart;
			modules[i].flags = flags;
			
			/* copy the string over :P */
			memcpy((char *)header + header->modnameend, new_mod, len);
			
			/* update the new size and the header modname end */
			*size += len;
			header->modnameend += len;
			
			/* change the modes */
			for (j = 0; j < header->nmodes; j++)
			{
				modes[j].searchstart = 0;
				modes[j].maxsearch++;
			}
			
			/* return success */
			return 0;
		}
	}
	
	/* return fail */
	return -1;
}

u32 sceKernelCheckPspConfigPatched(void *btcnf_data, u32 size, int flag)
{
	/* cast our variables and declare */
	BtcnfHeader *header = (BtcnfHeader *)btcnf_data;
	
	/* decrypt the btcnf so we have the decrypted version */
	int nsize = sceKernelCheckPspConfig(btcnf_data, size, flag);
	
	/* check the signature is valid */
	if (header->signature == 0x0F803001)
	{
		/* insert the module path into the config */
		InsertModuleBtcnf("/kd/strange.charm", "/kd/init.prx", btcnf_data, &nsize, (BOOTLOAD_VSH | BOOTLOAD_GAME | BOOTLOAD_POPS | BOOTLOAD_UPDATER | BOOTLOAD_UMDEMU | BOOTLOAD_MLNAPP));
	}
	
	/* return the new size */
	return nsize;
}

int DecryptExecutablePatched(void *header, int size, int *outsize)
{
	/* check for decryption tag */
	if (_lw((u32)header + 0x130) == 0x626F6F42)
	{
		*outsize = _lw((u32)header + 0xB0);
		memmove(header, header + 0x150, *outsize);
		return 0;
	}
	
	/* return the real decryption code */
	return DecryptExecutable(header, size, outsize);
}

int VerifySigncheckPatched(void *buffer, int size)
{
	int i;
	/* loop through the signcheck */
	for (i = 0; i < 0x58; i++)
	{
		/* if byte is 0 then call the signcheck removal */
		if (((u8 *)buffer)[0xD4 + i])
		{
			/* remove signcheck */
			return VerifySigncheck(buffer, size);
		}
	}
	
	/* return success :D */
	return 0;
}

int LoadCoreModuleStart631(int (* module_bootstart)(u32 argsize, void *argp), void *argp)
{
	/* get text_addr by substituting from module_bootstart address */
	u32 text_addr = ((u32)module_bootstart) - 0xBC4;
	
	/* assign our function pointers */
	DecryptExecutable = (void *)(text_addr + 0x8398);
	VerifySigncheck = (void *)(text_addr + 0x8378);
	
	/* patch the calls to the decryption */
	MAKE_CALL(text_addr + 0x6930, DecryptExecutablePatched);
	
	/* patch calls to the unsigncheck routines */
	MAKE_CALL(text_addr + 0x6954, VerifySigncheckPatched);
	
	/* call the loadcore bootstart */
	return module_bootstart(8, argp);
}

int LoadCoreModuleStart635(int (* module_bootstart)(u32 argsize, void *argp), void *argp)
{
	/* get text_addr by substituting from module_bootstart address */
	u32 text_addr = ((u32)module_bootstart) - 0xBBC;
	
	/* assign our function pointers */
	DecryptExecutable = (void *)(text_addr + 0x7B08);
	VerifySigncheck = (void *)(text_addr + 0x7AE8);
	
	/* patch the calls to the decryption */
	MAKE_CALL(text_addr + 0x5CA4, DecryptExecutablePatched);
	
	/* patch calls to the unsigncheck routines */
	MAKE_CALL(text_addr + 0x5CC8, VerifySigncheckPatched);
	
	/* call the loadcore bootstart */
	return module_bootstart(8, argp);
}

int LoadCoreModuleStart638(int (* module_bootstart)(u32 argsize, void *argp), void *argp)
{
	/* get text_addr by substituting from module_bootstart address */
	u32 text_addr = ((u32)module_bootstart) - 0xBBC;
	
	/* assign our function pointers */
	DecryptExecutable = (void *)(text_addr + 0x7B08);
	VerifySigncheck = (void *)(text_addr + 0x7AE8);
	
	/* patch the calls to the decryption */
	MAKE_CALL(text_addr + 0x5CA4, DecryptExecutablePatched);
	
	/* patch calls to the unsigncheck routines */
	MAKE_CALL(text_addr + 0x5CC8, VerifySigncheckPatched);
	
	/* call the loadcore bootstart */
	return module_bootstart(8, argp);
}

int LoadCoreModuleStart660(int (* module_bootstart)(u32 argsize, void *argp), void *argp)
{
	/* get text_addr by substituting from module_bootstart address */
	u32 text_addr = ((u32)module_bootstart) - 0xAF8;
	
	/* assign our function pointers */
	DecryptExecutable = (void *)(text_addr + 0x783C);
	VerifySigncheck = (void *)(text_addr + 0x7824);
	
	/* patch the calls to the decryption */
	MAKE_CALL(text_addr + 0x5970, DecryptExecutablePatched);
	
	/* patch calls to the unsigncheck routines */
	MAKE_CALL(text_addr + 0x5994, VerifySigncheckPatched);
	
	/* call the loadcore bootstart */
	return module_bootstart(8, argp);
}


int RebootEntryPatched(void *reboot_param, void *exec_param, u32 api, u32 initial_rnd)
{
	u32 model = _lw((u32)reboot_param + 44);
	
	if(model == 6 || model == 8)
	{
		model = 3;
	}
	
	/* Copy the patch table */
	_sw(sizeof(g_patch_table)/sizeof(PatchTable), PATCH_TABLE_ADDR_START);
	memcpy((void *)(PATCH_TABLE_ADDR_START + 4), g_patch_table, sizeof(g_patch_table));
	
	/* check the devkit */
	if (g_devkit_version == FIRMWARE_VERSION_631)
	{
		/* lets fixup our executable */
		_sw(TAG_631, (u32)downgrade_ctrl + 0xD0);
		_sw(MODULE_ID_TAG, (u32)downgrade_ctrl + 0x130);
		
		/* model specific patches */
		switch (model)
		{
			case 0:
			{
				/* link our function pointers */
				sceBootLfatOpen = (void *)0x88608624;
				sceBootLfatRead = (void *)0x88608798;
				sceBootLfatClose = (void *)0x8860873C;
				
				/* lets patch the IO drivers */
				MAKE_CALL(0x88602764, sceBootLfatOpenPatched);
				MAKE_CALL(0x886027D4, sceBootLfatReadPatched);
				MAKE_CALL(0x88602800, sceBootLfatClosePatched);
				
				/* link our function pointers */
				sceKernelCheckPspConfig = (void *)0x8860588C;
				
				/* patch the pspbtcnf.bin decryption */
				MAKE_CALL(0x88607348, sceKernelCheckPspConfigPatched);
				
				/* force removeByDebugSection success */
				_sw(0x03E00008, 0x8860389C);
				_sw(0x24020001, 0x886038A0);
				
				/* prevent sceBootLfatfsMount failing */
				_sw(0x00000000, 0x8860275C);
				
				/* prevent this lseek failure */
				_sw(0x00000000, 0x886027B0);
				_sw(0x00000000, 0x886027C8);
				
				/* btcnf module hash check */
				_sw(0x00000000, 0x88607648);
				
				/* patch loadcore.prx module_start */
				_sw(0x02202021, 0x88605758);
				MAKE_JUMP(0x88605760, LoadCoreModuleStart631);
				
				break;
			}
			
			case 1:
			case 2:
			case 3:
			case 4:
			{
				/* link our function pointers */
				sceBootLfatOpen = (void *)0x886086F0;
				sceBootLfatRead = (void *)0x88608864;
				sceBootLfatClose = (void *)0x88608808;
				
				/* lets patch the IO drivers */
				MAKE_CALL(0x88602834, sceBootLfatOpenPatched);
				MAKE_CALL(0x886028A4, sceBootLfatReadPatched);
				MAKE_CALL(0x886028D0, sceBootLfatClosePatched);	
				
				/* link our function pointers */
				sceKernelCheckPspConfig = (void *)0x8860595C;
				
				/* patch the pspbtcnf.bin decryption */
				MAKE_CALL(0x88607438, sceKernelCheckPspConfigPatched);
				
				/* force removeByDebugSection success */
				_sw(0x03E00008, 0x8860396C);
				_sw(0x24020001, 0x88603970);
				
				/* prevent sceBootLfatfsMount failing */
				_sw(0x00000000, 0x8860282C);
				
				/* prevent this lseek failure */
				_sw(0x00000000, 0x88602880);
				_sw(0x00000000, 0x88602898);
				
				/* btcnf module hash check */
				_sw(0x00000000, 0x88607714);
				
				/* patch loadcore.prx module_start */
				_sw(0x02202021, 0x88605828);
				MAKE_JUMP(0x88605830, LoadCoreModuleStart631);
				
				break;
			}
		}
	}
	
	/* check the devkit */
	else if (g_devkit_version == FIRMWARE_VERSION_635)
	{
		/* lets fixup our executable */
		_sw(TAG_635, (u32)downgrade_ctrl + 0xD0);
		_sw(MODULE_ID_TAG, (u32)downgrade_ctrl + 0x130);
		
		/* model specific patches */
		switch (model)
		{
			case 0:
			{
				/* link our function pointers */
				sceBootLfatOpen = (void *)0x88608624;
				sceBootLfatRead = (void *)0x88608798;
				sceBootLfatClose = (void *)0x8860873C;
				
				/* lets patch the IO drivers */
				MAKE_CALL(0x88602764, sceBootLfatOpenPatched);
				MAKE_CALL(0x886027D4, sceBootLfatReadPatched);
				MAKE_CALL(0x88602800, sceBootLfatClosePatched);
				
				/* link our function pointers */
				sceKernelCheckPspConfig = (void *)0x8860588C;
				
				/* patch the pspbtcnf.bin decryption */
				MAKE_CALL(0x88607348, sceKernelCheckPspConfigPatched);
				
				/* force removeByDebugSection success */
				_sw(0x03E00008, 0x8860389C);
				_sw(0x24020001, 0x886038A0);
				
				/* prevent sceBootLfatfsMount failing */
				_sw(0x00000000, 0x8860275C);
				
				/* prevent this lseek failure */
				_sw(0x00000000, 0x886027B0);
				_sw(0x00000000, 0x886027C8);
				
				/* btcnf module hash check */
				_sw(0x00000000, 0x88607648);
				
				/* patch loadcore.prx module_start */
				_sw(0x02202021, 0x88605758);
				MAKE_JUMP(0x88605760, LoadCoreModuleStart635);
				
				break;
			}
			
			case 1:		
			case 2:
			case 3:
			case 4:
			{
				/* link our function pointers */
				sceBootLfatOpen = (void *)0x886086F0;
				sceBootLfatRead = (void *)0x88608864;
				sceBootLfatClose = (void *)0x88608808;
				
				/* lets patch the IO drivers */
				MAKE_CALL(0x88602834, sceBootLfatOpenPatched);
				MAKE_CALL(0x886028A4, sceBootLfatReadPatched);
				MAKE_CALL(0x886028D0, sceBootLfatClosePatched);	
				
				/* link our function pointers */
				sceKernelCheckPspConfig = (void *)0x8860595C;
				
				/* patch the pspbtcnf.bin decryption */
				MAKE_CALL(0x88607438, sceKernelCheckPspConfigPatched);
				
				/* force removeByDebugSection success */
				_sw(0x03E00008, 0x8860396C);
				_sw(0x24020001, 0x88603970);
				
				/* prevent sceBootLfatfsMount failing */
				_sw(0x00000000, 0x8860282C);
				
				/* prevent this lseek failure */
				_sw(0x00000000, 0x88602880);
				_sw(0x00000000, 0x88602898);
				
				/* btcnf module hash check */
				_sw(0x00000000, 0x88607714);
				
				/* patch loadcore.prx module_start */
				_sw(0x02202021, 0x88605828);
				MAKE_JUMP(0x88605830, LoadCoreModuleStart635);
				break;
			}
		}
	}
	
	/* check the devkit */
	else if (g_devkit_version == FIRMWARE_VERSION_638)
	{
		/* lets fixup our executable */
		_sw(TAG_638, (u32)downgrade_ctrl + 0xD0);
		_sw(MODULE_ID_TAG, (u32)downgrade_ctrl + 0x130);
		
		/* model specific patches */
		switch (model)
		{
			case 0:
			{
				/* link our function pointers */
				sceBootLfatOpen = (void *)0x88608250;
				sceBootLfatRead = (void *)0x886083C4;
				sceBootLfatClose = (void *)0x88608368;
				
				/* lets patch the IO drivers */
				MAKE_CALL(0x88602768, sceBootLfatOpenPatched);
				MAKE_CALL(0x886027D8, sceBootLfatReadPatched);
				MAKE_CALL(0x88602804, sceBootLfatClosePatched);
				
				/* link our function pointers */
				sceKernelCheckPspConfig = (void *)0x8860569C;
				
				/* patch the pspbtcnf.bin decryption */
				MAKE_CALL(0x8860711C, sceKernelCheckPspConfigPatched);
				
				/* force removeByDebugSection success */
				_sw(0x03E00008, 0x88603848);
				_sw(0x24020001, 0x8860384C);
				
				/* prevent sceBootLfatfsMount failing */
				_sw(0x00000000, 0x88602760);
				
				/* prevent this lseek failure */
				_sw(0x00000000, 0x886027B4);
				_sw(0x00000000, 0x886027CC);
				
				/* btcnf module hash check */
				_sw(0x00000000, 0x886073B4);
				
				/* patch loadcore.prx module_start */
				_sw(0x02202021, 0x88605588);
				MAKE_JUMP(0x88605590, LoadCoreModuleStart638);
				
				break;
			}
			
			case 1:		
			case 2:
			case 3:
			case 4:
			{
				/* link our function pointers */
				sceBootLfatOpen = (void *)0x88608320;
				sceBootLfatRead = (void *)0x88608494;
				sceBootLfatClose = (void *)0x88608438;
				
				/* lets patch the IO drivers */
				MAKE_CALL(0x88602838, sceBootLfatOpenPatched);
				MAKE_CALL(0x886028A8, sceBootLfatReadPatched);
				MAKE_CALL(0x886028D4, sceBootLfatClosePatched);	
				
				/* link our function pointers */
				sceKernelCheckPspConfig = (void *)0x8860576C;
				
				/* patch the pspbtcnf.bin decryption */
				MAKE_CALL(0x886071EC, sceKernelCheckPspConfigPatched);
				
				/* force removeByDebugSection success */
				_sw(0x03E00008, 0x88603918);
				_sw(0x24020001, 0x8860391C);
				
				/* prevent sceBootLfatfsMount failing */
				_sw(0x00000000, 0x88602830);
				
				/* prevent this lseek failure */
				_sw(0x00000000, 0x88602884);
				_sw(0x00000000, 0x8860289C);
				
				/* btcnf module hash check */
				_sw(0x00000000, 0x88607484);
				
				/* patch loadcore.prx module_start */
				_sw(0x02202021, 0x88605658);
				MAKE_JUMP(0x88605660, LoadCoreModuleStart638);
				break;
			}
		}
	}
	
	else if (g_devkit_version == FIRMWARE_VERSION_639)
	{
		/* lets fixup our executable */
		_sw(TAG_638, (u32)downgrade_ctrl + 0xD0);
		_sw(MODULE_ID_TAG, (u32)downgrade_ctrl + 0x130);
		
		/* model specific patches */
		switch (model)
		{
			case 0:
			{
				/* link our function pointers */
				sceBootLfatOpen = (void *)0x88608250;
				sceBootLfatRead = (void *)0x886083C4;
				sceBootLfatClose = (void *)0x88608368;
				
				/* lets patch the IO drivers */
				MAKE_CALL(0x88602768, sceBootLfatOpenPatched);
				MAKE_CALL(0x886027D8, sceBootLfatReadPatched);
				MAKE_CALL(0x88602804, sceBootLfatClosePatched);
				
				/* link our function pointers */
				sceKernelCheckPspConfig = (void *)0x8860569C;
				
				/* patch the pspbtcnf.bin decryption */
				MAKE_CALL(0x8860711C, sceKernelCheckPspConfigPatched);
				
				/* force removeByDebugSection success */
				_sw(0x03E00008, 0x88603848);
				_sw(0x24020001, 0x8860384C);
				
				/* prevent sceBootLfatfsMount failing */
				_sw(0x00000000, 0x88602760);
				
				/* prevent this lseek failure */
				_sw(0x00000000, 0x886027B4);
				_sw(0x00000000, 0x886027CC);
				
				/* btcnf module hash check */
				_sw(0x00000000, 0x886073B4);
				
				/* patch loadcore.prx module_start */
				_sw(0x02202021, 0x88605588);
				MAKE_JUMP(0x88605590, LoadCoreModuleStart638);
				
				break;
			}
			
			case 1:		
			case 2:
			case 3:
			case 4:
			{
				/* link our function pointers */
				sceBootLfatOpen = (void *)0x88608320;
				sceBootLfatRead = (void *)0x88608494;
				sceBootLfatClose = (void *)0x88608438;
				
				/* lets patch the IO drivers */
				MAKE_CALL(0x88602838, sceBootLfatOpenPatched);
				MAKE_CALL(0x886028A8, sceBootLfatReadPatched);
				MAKE_CALL(0x886028D4, sceBootLfatClosePatched);	
				
				/* link our function pointers */
				sceKernelCheckPspConfig = (void *)0x8860576C;
				
				/* patch the pspbtcnf.bin decryption */
				MAKE_CALL(0x886071EC, sceKernelCheckPspConfigPatched);
				
				/* force removeByDebugSection success */
				_sw(0x03E00008, 0x88603918);
				_sw(0x24020001, 0x8860391C);
				
				/* prevent sceBootLfatfsMount failing */
				_sw(0x00000000, 0x88602830);
				
				/* prevent this lseek failure */
				_sw(0x00000000, 0x88602884);
				_sw(0x00000000, 0x8860289C);
				
				/* btcnf module hash check */
				_sw(0x00000000, 0x88607484);
				
				/* patch loadcore.prx module_start */
				_sw(0x02202021, 0x88605658);
				MAKE_JUMP(0x88605660, LoadCoreModuleStart638);
				break;
			}
		}
	}
	
	else if (g_devkit_version == FIRMWARE_VERSION_660)
	{
		/* lets fixup our executable */
		_sw(TAG_660, (u32)downgrade660_ctrl + 0xD0);
		_sw(MODULE_ID_TAG, (u32)downgrade660_ctrl + 0x130);
		
		/* model specific patches */
		switch (model)
		{
			case 0:
			{

				/* link our function pointers */
				sceBootLfatOpen = (void *)0x8860822C;
				sceBootLfatRead = (void *)0x886083A0;
				sceBootLfatClose = (void *)0x88608344;
				
				/* lets patch the IO drivers */
				MAKE_CALL(0x886027C4, sceBootLfatOpenPatched);
				MAKE_CALL(0x88602834, sceBootLfatReadPatched660);
				MAKE_CALL(0x88602860, sceBootLfatClosePatched);
				
				/* link our function pointers */
				sceKernelCheckPspConfig = (void *)0x8860574C;
				
				/* patch the pspbtcnf.bin decryption */
				MAKE_CALL(0x886070F8, sceKernelCheckPspConfigPatched);
				
				/* force removeByDebugSection success */
				_sw(0x03E00008, 0x88603880);
				_sw(0x24020001, 0x88603884);
				
				/* prevent sceBootLfatfsMount failing */
				_sw(0x00000000, 0x886027BC);
				
				/* prevent this lseek failure */
				_sw(0x00000000, 0x88602810);
				_sw(0x00000000, 0x88602828);
				
				/* btcnf module hash check */
				_sw(0x00000000, 0x88607390);
				
				/* patch loadcore.prx module_start */
				_sw(0x02202021, 0x88605638);
				MAKE_JUMP(0x88605640, LoadCoreModuleStart660);
				
				break;
			}
			
			case 1:
			case 2:
			case 3:
			case 4:
			{
				/* link our function pointers */
				sceBootLfatOpen = (void *)0x886082EC;
				sceBootLfatRead = (void *)0x88608460;
				sceBootLfatClose = (void *)0x88608404;
				
				/* lets patch the IO drivers */
				MAKE_CALL(0x8860288C, sceBootLfatOpenPatched);
				MAKE_CALL(0x886028FC, sceBootLfatReadPatched660);
				MAKE_CALL(0x88602928, sceBootLfatClosePatched);	
				
				/* link our function pointers */
				sceKernelCheckPspConfig = (void *)0x8860580C;
				
				/* patch the pspbtcnf.bin decryption */
				MAKE_CALL(0x886071B8, sceKernelCheckPspConfigPatched);
				
				/* force removeByDebugSection success */
				_sw(0x03E00008, 0x88603948);
				_sw(0x24020001, 0x8860394C);
				
				/* prevent sceBootLfatfsMount failing */
				_sw(0x00000000, 0x88602884);
				
				/* prevent this lseek failure */
				_sw(0x00000000, 0x886028D8);
				_sw(0x00000000, 0x886028F0);
				
				/* btcnf module hash check */
				_sw(0x00000000, 0x88607450);
				
				/* patch loadcore.prx module_start */
				_sw(0x02202021, 0x886056F8);
				MAKE_JUMP(0x88605700, LoadCoreModuleStart660);
				break;
			}
		}
	}
	
	/* Clear the caches */
	KClearCaches();
	
	/* lets start the reboot process */
	return sceReboot(reboot_param, exec_param, api, initial_rnd);
}
