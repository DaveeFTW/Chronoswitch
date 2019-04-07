/*
	Downgrade Launcher -> kernel_land.c -> Responsible for containing code access in kernel mode
	by Davee
	
	28/12/2010
*/

#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <psploadexec_kernel.h>

#include <stdio.h>
#include <string.h>

#include "kernel_land.h"
#include "rebootex.h"
#include "utils.h"
#include "downgrade_ctrl/patch_table.h"

/* function pointers */
int (* pspKernelGetModel)(void) = NULL;
int (* pspSysconGetBaryonVersion)(u32 *baryon) = NULL;
SceModule2 *(* pspKernelFindModuleByName)(const char *name) = NULL;
int (* pspKernelLoadExecVSHEf1)(const char *path, struct SceKernelLoadExecVSHParam *param) = NULL;
int (* pspKernelLoadExecVSHMs1)(const char *path, struct SceKernelLoadExecVSHParam *param) = NULL;
SceUID (* pspIoOpen)(char *file, int flags, SceMode mode) = NULL;
int (* pspIoWrite)(SceUID fd, void *data, u32 len) = NULL;
int (* pspIoClose)(SceUID fd) = NULL;

/* globals */
struct SceKernelLoadExecVSHParam g_exec_param;

u32 getBaryon(void)
{
	u32 baryon;
	
	/* get the baryon version */
	pspSysconGetBaryonVersion(&baryon);
	
	/* return it */
	return baryon;
}

int getModel(void)
{
	/* return the PSP model */
	return pspKernelGetModel();
}

int delete_resume_game(void)
{
	u8 _header[512+64];
	
	/* use a pointer for math ease */
	u8 *header = _header; 
	
	/* align to 64 */
	header = (u8 *)((u32)header & ~0x3F); header = (u8 *)((u32)header + 0x40);
	
	/* now clear it */
	memset(header, 0, 512);
	
	/* open hibernation fs */
	SceUID fd = pspIoOpen("eflash0a:__hibernation", 0x04000003, 0);
	
	/* check for error */
	if (fd < 0)
	{
		/* return error */
		return fd;
	}
	
	/* write the blank header */
	int written = pspIoWrite(fd, header, 512);
	
	/* check for error */
	if (written < 0)
	{
		/* close file */
		pspIoClose(fd);
		
		/* return the error code */
		return written;
	}
	
	/* return result */
	return pspIoClose(fd);
}

int patch_loadexec_phat(void)
{
	/* Find the LoadExec */
	SceModule2 *mod = pspKernelFindModuleByName("sceLoadExec");
	
	if(g_devkit_version == FIRMWARE_VERSION_638 || g_devkit_version == FIRMWARE_VERSION_639 || g_devkit_version == FIRMWARE_VERSION_660 || g_devkit_version == FIRMWARE_VERSION_661)
	{
		/* Patch the reboot process */
		MAKE_CALL(mod->text_addr + 0x2DAC, RebootEntryPatched); //6.38		
		/* get past the userlevel check */
		MAKE_RELATIVE_BRANCH(0x23D0, 0x2418, mod->text_addr); //6.38
	}
	else
	{
		/* Patch the reboot process */
		MAKE_CALL(mod->text_addr + 0x2D94, RebootEntryPatched); //6.31/6.35
		
		/* get past the userlevel check */
		MAKE_RELATIVE_BRANCH(0x23B8, 0x2400, mod->text_addr); //6.31/6.35
	}
	
	KClearCaches();
	
	/* just return 0 */
	return pspKernelLoadExecVSHMs1(OTHER_UPDATER_PATH, &g_exec_param);
}

int patch_loadexec_slim(void)
{
	/* Find the LoadExec */
	SceModule2 *mod = pspKernelFindModuleByName("sceLoadExec");
	
	if(g_devkit_version == FIRMWARE_VERSION_638 || g_devkit_version == FIRMWARE_VERSION_639 || g_devkit_version == FIRMWARE_VERSION_660 || g_devkit_version == FIRMWARE_VERSION_661)
	{
		/* Patch the reboot process */
		MAKE_CALL(mod->text_addr + 0x2DAC, RebootEntryPatched); //6.38		
		/* get past the userlevel check */
		MAKE_RELATIVE_BRANCH(0x23D0, 0x2418, mod->text_addr); //6.38
	}
	else
	{
		/* Patch the reboot process */
		MAKE_CALL(mod->text_addr + 0x2D94, RebootEntryPatched); //6.31/6.35
		
		/* get past the userlevel check */
		MAKE_RELATIVE_BRANCH(0x23B8, 0x2400, mod->text_addr); //6.31/6.35
	}
	
	KClearCaches();
	
	/* just return 0 */
	return pspKernelLoadExecVSHMs1(OTHER_UPDATER_PATH, &g_exec_param);
}

int patch_loadexec_3000(void)
{
	/* Find the LoadExec */
	SceModule2 *mod = pspKernelFindModuleByName("sceLoadExec");
	
	if(g_devkit_version == FIRMWARE_VERSION_638 || g_devkit_version == FIRMWARE_VERSION_639 || g_devkit_version == FIRMWARE_VERSION_660 || g_devkit_version == FIRMWARE_VERSION_661)
	{
		/* Patch the reboot process */
		MAKE_CALL(mod->text_addr + 0x2DAC, RebootEntryPatched); //6.38		
		/* get past the userlevel check */
		MAKE_RELATIVE_BRANCH(0x23D0, 0x2418, mod->text_addr); //6.38
	}
	else
	{
		/* Patch the reboot process */
		MAKE_CALL(mod->text_addr + 0x2D94, RebootEntryPatched); //6.31/6.35
		
		/* get past the userlevel check */
		MAKE_RELATIVE_BRANCH(0x23B8, 0x2400, mod->text_addr); //6.31/6.35
	}
	
	KClearCaches();
	
	/* just return 0 */
	return pspKernelLoadExecVSHMs1(OTHER_UPDATER_PATH, &g_exec_param);
}

int patch_loadexec_4000(void)
{
	/* Find the LoadExec */
	SceModule2 *mod = pspKernelFindModuleByName("sceLoadExec");
	
	if(g_devkit_version == FIRMWARE_VERSION_638 || g_devkit_version == FIRMWARE_VERSION_639 || g_devkit_version == FIRMWARE_VERSION_660 || g_devkit_version == FIRMWARE_VERSION_661)
	{
		/* Patch the reboot process */
		MAKE_CALL(mod->text_addr + 0x2DAC, RebootEntryPatched); //6.38		
		/* get past the userlevel check */
		MAKE_RELATIVE_BRANCH(0x23D0, 0x2418, mod->text_addr); //6.38
	}
	else
	{
		/* Patch the reboot process */
		MAKE_CALL(mod->text_addr + 0x2D94, RebootEntryPatched); //6.31/6.35
		
		/* get past the userlevel check */
		MAKE_RELATIVE_BRANCH(0x23B8, 0x2400, mod->text_addr); //6.31/6.35
	}
	
	KClearCaches();
	
	/* just return 0 */
	return pspKernelLoadExecVSHMs1(OTHER_UPDATER_PATH, &g_exec_param);
}

int patch_loadexec_pspgo(void)
{
	/* Find the LoadExec */
	SceModule2 *mod = pspKernelFindModuleByName("sceLoadExec");
	
	if(g_devkit_version == FIRMWARE_VERSION_638 || g_devkit_version == FIRMWARE_VERSION_639 || g_devkit_version == FIRMWARE_VERSION_660 || g_devkit_version == FIRMWARE_VERSION_661)
	{
		/* Patch the reboot process */
		MAKE_CALL(mod->text_addr + 0x2FF8, RebootEntryPatched); //6.38
		
		/* get past the userlevel checks for VSH */
		MAKE_RELATIVE_BRANCH(0x2624, 0x266C, mod->text_addr); //6.31/6.35
	}
	else
	{
		/* Patch the reboot process */
		MAKE_CALL(mod->text_addr + 0x2FE0, RebootEntryPatched); //6.31/6.35
		
		/* get past the userlevel checks for VSH */
		MAKE_RELATIVE_BRANCH(0x260C, 0x2658, mod->text_addr); //6.31/6.35
	}
	
	/* clear the caches */
	KClearCaches();

	/* reboot into the updater */
	return pspKernelLoadExecVSHEf1(PSPGO_UPDATER_PATH, &g_exec_param);
}

int patch_loadexec_street(void)
{
	/* Find the LoadExec */
	SceModule2 *mod = pspKernelFindModuleByName("sceLoadExec");
	
	if(g_devkit_version == FIRMWARE_VERSION_660 || g_devkit_version == FIRMWARE_VERSION_661)
	{
		/* Patch the reboot process */
		MAKE_CALL(mod->text_addr + 0x2DAC, RebootEntryPatched); //6.38		
		/* get past the userlevel check */
		MAKE_RELATIVE_BRANCH(0x23D0, 0x2418, mod->text_addr); //6.38
	}
	
	KClearCaches();
	
	/* just return 0 */
	return pspKernelLoadExecVSHMs1(OTHER_UPDATER_PATH, &g_exec_param);
}

int launch_updater(void)
{
	KClearCaches();
	
	int res = -1;
	
	/* clear our param */
	memset(&g_exec_param, 0, sizeof(struct SceKernelLoadExecVSHParam));
	
	/* fill the field */
	g_exec_param.size = sizeof(struct SceKernelLoadExecVSHParam);
	g_exec_param.argp = (pspKernelGetModel() == 4) ? (PSPGO_UPDATER_PATH) : (OTHER_UPDATER_PATH);
	g_exec_param.args = strlen(g_exec_param.argp) + 1;
	g_exec_param.key = "updater";
	g_exec_param.vshmain_args_size = 0;
	g_exec_param.vshmain_args = NULL;
	g_exec_param.configfile = NULL;
	g_exec_param.unk4 = 0;
	g_exec_param.unk5 = 0x10000;
	
	/* get the model */
	u32 model = pspKernelGetModel();
	
	if(model == 6 || model == 8)
	{
		model = 3;
	}
	
	/* launch a loadexec patch depending on model */
	switch (model)
	{
		/* PSP PHAT */
		case 0:
		{
			/* launch the updater and patch reboot */
			res = patch_loadexec_phat();
			break;
		}
		
		/* PSP SLIM */
		case 1:
		{
			/* launch the updater and patch reboot */
			res = patch_loadexec_slim();
			break;
		}
		
		/* PSP 3000 */
		case 2:
		{
			/* launch the updater and patch reboot */
			res = patch_loadexec_3000();
			break;
		}
		
		/* PSP 4000 */
		case 3:
		{
			/* launch the updater and patch reboot */
			res = patch_loadexec_4000();
			break;
		}
		
		/* PSP Go */
		case 4:
		{
			/* launch the updater and patch reboot */
			res = patch_loadexec_pspgo();
			break;
		}

		/* PSP E-1000 (Street) */
		case 10:
		{
			res = patch_loadexec_street();
			break;
		}
	}
	
	/* return result */
	return res;
}
