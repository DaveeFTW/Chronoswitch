/*
	Downgrade Launcher -> kernel_land.h -> Provide API documentation and definitions for the kernel operations
	by Davee
	
	28/12/2010
*/
#ifndef __KERNEL_LAND_H__
#define __KERNEL_LAND_H__

#include <psploadexec_kernel.h>

int getModel(void);
u32 getBaryon(void);
int launch_updater(void);
int delete_resume_game(void);

extern int (* pspKernelGetModel)(void);
extern int (* pspSysconGetBaryonVersion)(u32 *baryon);
extern SceModule *(* pspKernelFindModuleByName)(const char *name);
extern int (* pspKernelLoadExecVSHEf1)(const char *path, struct SceKernelLoadExecVSHParam *param);
extern int (* pspKernelLoadExecVSHMs1)(const char *path, struct SceKernelLoadExecVSHParam *param);
extern SceUID (* pspIoOpen)(char *file, int flags, SceMode mode);
extern int (* pspIoWrite)(SceUID fd, void *data, u32 len);
extern int (* pspIoClose)(SceUID fd);

#endif /* __KERNEL_LAND_H__ */
