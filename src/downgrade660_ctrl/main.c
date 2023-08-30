/*
	Downgrade Control -> Restore kernel patches and enforce updater launch
	by Davee
	
	31/12/2010
*/

#include <pspkernel.h>
#include <pspidstorage.h>
#include <pspsysmem_kernel.h>

#include <stdio.h>
#include <string.h>
#include <systemctrl.h>
#include <libinfinity.h>

#include "utils.h"
#include "patch_table.h"
#include "decrypt.h"

PSP_MODULE_INFO("DowngraderCTRL", 0x3007, 1, 0);

/* function pointers */
int (* PrologueModule)(void *modmgr_param, SceModule2 *mod) = NULL;
STMOD_HANDLER previous = NULL;

typedef struct __attribute__((packed))
{
        int magic; // 0
        int version; // 4
        unsigned int keyofs; // 8
        unsigned int valofs; // 12
        int count; // 16
} SfoHeader;

typedef struct __attribute__((packed))
{
        unsigned short nameofs; // 0
        char alignment; // 2
        char type; // 3
        int valsize; // 4
        int totalsize; // 8
        unsigned short valofs; // 12
        short unknown; // 16
} SfoEntry;

u32 g_spoof_ver = 0;
u8 g_sfo_buffer[4 << 10];

int ApplyFirmware(SceModule2 *mod)
{
	int i;
	char *fw_data = NULL;
	u8 device_fw_ver[4];
	u32 pbp_header[0x28/4];
	SfoHeader *header = (SfoHeader *)g_sfo_buffer;
	SfoEntry *entries = (SfoEntry *)((char *)g_sfo_buffer + sizeof(SfoHeader));
	
	/* Lets open the updater */
	char *file = (sceKernelGetModel() == 4) ? ("ef0:/PSP/GAME/UPDATE/EBOOT.pbp") : ("ms0:/PSP/GAME/UPDATE/EBOOT.pbp");
	
	/* set k1 */
	u32 k1 = pspSdkSetK1(0);
	
	/* lets open the file */
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0777);
	
	/* check for failure */
	if (fd < 0)
	{
		/* rage */
		pspSdkSetK1(k1);
		return fd;
	}
	
	/* read the PBP header */
	sceIoRead(fd, pbp_header, sizeof(pbp_header));
	
	/* seek to the SFO */
	sceIoLseek32(fd, pbp_header[8/4], PSP_SEEK_SET);
	
	/* calculate the size of the SFO */
	u32 sfo_size = pbp_header[12/4] - pbp_header[8/4];
	
	/* check if greater than buffer size */
	if (sfo_size > sizeof(g_sfo_buffer))
	{
		/* too much */
		pspSdkSetK1(k1);
		return -2;
	}
	
	/* read the sfo */
	sceIoRead(fd, g_sfo_buffer, sizeof(g_sfo_buffer));
	
	/* close the file */
	sceIoClose(fd);
	
	/* now parse the SFO */
	for (i = 0; i < header->count; i++)
	{
		/* check this name */
		if (strcmp((char *)((char *)g_sfo_buffer + header->keyofs + entries[i].nameofs), "UPDATER_VER") == 0)
		{
			/* get the string */
			fw_data = (char *)((char *)g_sfo_buffer + header->valofs + entries[i].valofs);
			break;
		}
	}
	
	/* see if we went through all the data */
	if (i == header->count)
	{
		/* error */
		pspSdkSetK1(k1);
		return -3;
	}
	
	/* ok, we have the firmware version in the eboot. */
	u32 min_ver = 0;
	u32 updater_ver = (((fw_data[0] - '0') & 0xF) << 8) | (((fw_data[2] - '0') & 0xF) << 4) | (((fw_data[3] - '0') & 0xF) << 0);

	/* ok, now get the idstorage value */
	int res = sceIdStorageLookup(0x51, 0, device_fw_ver, 4);
	
	/* check for error */
	if (res < 0)
	{		
		/* set minimum firmware to 1.00, if unknown */
		min_ver = 0x100;
	}
	else
	{
		/* convert to hex */		
		min_ver = (((device_fw_ver[0] - '0') & 0xF) << 8) | (((device_fw_ver[2] - '0') & 0xF) << 4) | (((device_fw_ver[3] - '0') & 0xF) << 0);
	}
	
	/* set the result to 0 */
	res = 0;
	
	/* check if the updater is 620 and the model is 09g */
	if (updater_ver == 0x620 && sceKernelGetModel() == 8)
	{		
		/* set result to 1 D: */
		res = 1;
	}

	//fix the issue with 6.61 -> 6.60
	if ((sceKernelDevkitVersion() == 0x06060110) && (updater_ver == 0x660)) {
		_sw(0x3C020606, 0x08801000); //lui	$v0, 0x606
		_sw(0x03E00008, 0x08801004); //jr	$ra
		_sw(0x34420010, 0x08801008); //ori	$v0, $v0, 0x10

		//redirect sceKernelDevkitVersion in updater module
		MAKE_JUMP(mod->text_addr + 0x123CD0, 0x08801000);
		_sw(0, mod->text_addr + 0x123CD4);

		ClearCaches();
	}

	/* do spoof! */
	g_spoof_ver = updater_ver;
	pspSdkSetK1(k1);
	return res;
}

#define INDEXFILE "flash0:/vsh/etc/index_04g.dat"
#define NEW_INDEXFILE "flash0:/vsh/etc/index_09g.dat"

SceUID sceIoOpenPatched(char *file, u32 mode, u32 mode2)
{
	/* check indexfile */
	if (strcmp(file, INDEXFILE) == 0)
	{
		/* if read mode, copy */
		if (mode == PSP_O_RDONLY)
		{
			strcpy(file, NEW_INDEXFILE);
		}
	}
	
	/* call original function */
	return sceIoOpen(file, mode, mode2);
}

int sceIoGetstatPatched(char *file, SceIoStat *stat)
{
	/* check indexfile */
	if (strcmp(file, INDEXFILE) == 0)
	{
		strcpy(file, NEW_INDEXFILE);
	}
	
	/* call original function */
	return sceIoGetstat(file, stat);
}

/* idstorage patching func */
int (* pspUtilsBufferCopyWithRange)(void *dst, u32 dst_size, void *src, u32 src_size, u32 cmd) = NULL;
int sceUtilsBufferCopyWithRangePatched(void *dst, u32 dst_size, void *src, u32 src_size, u32 cmd)
{
	u8 ids_cert[0xB8];
	
	/* if idstorage check */
	if (cmd == 0x12)
	{
		/* ok lets get the idstorage 0x100 key */
		int res = sceIdStorageLookup(0x100, 0x38, ids_cert, 0xB8);
		
		/* check for error */
		if (res < 0)
		{
			/* attempt with backup key */
			res = sceIdStorageLookup(0x120, 0x38, ids_cert, 0xB8);
			
			/* if error, exit */
			if (res < 0)
			{
				return res;
			}
		}
	
		/* yay its 0x100 key */
		/* change the model to 04g */
		ids_cert[7] = 6;
		
		/* copy the new certificate back */
		memcpy(src, ids_cert, sizeof(ids_cert));
		return 0;
	}
	
	/* return function */
	return pspUtilsBufferCopyWithRange(dst, dst_size, src, src_size, cmd);
}

int (* sceLflashFatfmtStartFatfmtOriginal)(int argc, char *argv[]) = NULL;

int sceLflashFatfmtStartFatfmtPatched(int argc, char *argv[])
{
    infSetRedirectionStatus(0);
    ClearCaches();
    
    return sceLflashFatfmtStartFatfmtOriginal(argc, argv);
}

int OnModuleStart(SceModule2 *mod)
{
	if (strcmp(mod->modname, "sceMesgLed") == 0)
	{
		PatchMesgled(mod->text_addr);
		ClearCaches();
	}
    
    else if (strcmp(mod->modname, "sceLflashFatfmtUpdater") == 0)
    {
        PatchSyscall(FindFunc("sceLflashFatfmtUpdater", "LflashFatfmt", 0xB7A424A4), sceLflashFatfmtStartFatfmtPatched);
        sceLflashFatfmtStartFatfmtOriginal = FindFunc("sceLflashFatfmtUpdater", "LflashFatfmt", 0xB7A424A4);
        ClearCaches();
    }
	
	else if (strcmp(mod->modname, "updater") == 0)
	{
		/* ok, lets see what we're doing here! */
		int res = ApplyFirmware(mod);
		
		/* check for success */
		if (res >= 0)
		{
			/* do these patches if we have 09g going to <6.3X */
			if (res == 1)
			{
				/* patch the IO */
				PatchSyscall(FindFunc("sceIOFileManager", "IoFileMgrForUser", 0x109F50BC), sceIoOpenPatched);
				PatchSyscall(FindFunc("sceIOFileManager", "IoFileMgrForUser", 0xACE946E8), sceIoGetstatPatched);
				
				/* find the function for idstorage verify */
				u32 func_address = FindFunc("sceMemlmd", "semaphore", 0x4C537C72);
				
				/* check if we have it... */
				if (func_address == 0)
				{
					/* ERROR */
					asm("break\n");
				}
				
				/* ok, lets patch it */
				KERNEL_HIJACK_FUNCTION(func_address, sceUtilsBufferCopyWithRangePatched, pspUtilsBufferCopyWithRange);
			}
			
			/* patch the version check on index.dat */
			PatchSyscall(FindFunc("sceMesgLed", "sceResmgr", 0x9DC14891), sceResmgr_9DC14891_patched);
			ClearCaches();	
		}
	}

	
	/* if there is a previous handler, call it */
	if (previous)
		return previous((SceModule2 *)mod);
	
	/* else just return 0 */
	return 0;
}

int PrologueModulePatched(void *modmgr_param, SceModule2 *mod)
{
	/* modmgr_param has changed from 1.50 so I have no included the structure defintion, for an updated version a re-reverse of 6.30 modulemgr is required */
	int res = PrologueModule(modmgr_param, mod);
	
	/* If this function errors, the module is shutdown so we better check for it */
	if (res >= 0)
	{
		/* Pass the module through the OnModuleStart chain */
		OnModuleStart(mod);
	}
	
	/* return success */
	return res;
}

static void PatchModuleManager(void)
{
	/* find the modulemgr module */
	SceModule2 *mod = (SceModule2 *)sceKernelFindModuleByName("sceModuleManager");
	u32 text_addr = mod->text_addr;
	
	/* link the original calls before hook */
	PrologueModule = (void *)(text_addr + g_patch_table.prologue_module_func);
	
	/* Patch call to PrologueModule from the StartModule function to allow a full coverage of loaded modules (even those without an entry point) */
	MAKE_CALL(text_addr + g_patch_table.prologue_module_call, PrologueModulePatched);
}

static void PatchLoadCore(void)
{
	/* Find the loadcore module */
	SceModule2 *mod = (SceModule2 *)sceKernelFindModuleByName("sceLoaderCore");
	u32 text_addr = mod->text_addr;
	
	/* Relink the memlmd calls (that reboot destroyed) */
	MAKE_CALL(text_addr + g_patch_table.memlmd_call[0], text_addr + g_patch_table.memlmd_stub[0]);
	MAKE_CALL(text_addr + g_patch_table.memlmd_call[1], text_addr + g_patch_table.memlmd_stub[1]);

	/* if >= 6.30 we need patches for the decryption */
	if (sceKernelDevkitVersion() >= 0x06030010)
	{
		/* we need to hook updater decryption */
		MAKE_CALL(text_addr + g_patch_table.memlmd_call[0], memlmd_7CF1CD3E_patched);
		MAKE_CALL(text_addr + g_patch_table.updater_decrypt_call, sceMesgLed_driver_81F72B1F_patched);
		
		sceMesgLed_driver_81F72B1F = (void *)(text_addr + g_patch_table.updater_decrypt_func);
	}
}

int module_start(SceSize argsize, void *argp)
{
	/* check if we're running as a plugin or in boot */
	if (sceKernelFindModuleByName("sceInit"))
	{
		/* plugin, assume we've got HEN (and an M33 nid resolver) */
		/* check for >= 6.30 */
		if (sceKernelDevkitVersion() >= 0x06030010)
		{
			/* this is firmware dependant :/ so not supported */
			return 0;
		}
		
		/* register with sctrl */
		previous = sctrlHENSetStartModuleHandler((STMOD_HANDLER)OnModuleStart);
	}
	else
	{
		/* boot */
		/* get patches and nids from loader */
		if (CopyPatchTable(&g_patch_table, (void *)PATCH_TABLE_ADDR_START, sceKernelDevkitVersion()) == 0)
		{
			/* no patches, no go */
			return 0;
		}
		
		/* perform main system patches */
		PatchLoadCore();
		PatchModuleManager();
	}

	/* Clear the caches and return success */
	ClearCaches();
	return 0;
}
