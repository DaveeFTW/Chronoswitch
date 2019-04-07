/*
	Downgrade Control -> decrypt.c -> Responsible for decryption patches and hooks
	by Davee
	
	31/12/2010
*/

#include <pspkernel.h>
#include <pspsysmem_kernel.h>

#include <stdio.h>
#include <string.h>
#include <systemctrl.h>

#include "decrypt.h"
#include "patch_table.h"

int pspDecryptPRX(void *exec, u32 exec_size, u32 *out_size);

int (* sceMesgLed_driver_81F72B1F)(void *exec, u32 size, u32 *outsize) = NULL;
extern u32 g_spoof_ver;

int sceResmgr_9DC14891_patched(void *data, u32 datasize, u32 *outsize)
{
	/* call the original function to decrypt the index.dat */
	int res = sceResmgr_driver_9DC14891(data, datasize, outsize);
	
	/* if there is a firmware version, we will generate a version.txt to downgrade */
	if (g_spoof_ver)
	{
		sprintf( data,
				"release:%01X.%01X%01X:\n"
				"build:0000,0,3,1,0:builder@vsh-build6\n"
				"system:56422@release_%03X,0x%08X:\n"
				"vsh:p6576@release_%03X,v57929@release_%03X,20100625:\n"
				"target:1:WorldWide\n",
				/* first line, version (X.YZ) */ (g_spoof_ver >> 8) & 0xF, (g_spoof_ver >> 4) & 0xF, g_spoof_ver & 0xF,
				/* third line, release_XYZ + devkit */ g_spoof_ver, (((g_spoof_ver >> 8) & 0xF) << 24) | (((g_spoof_ver >> 4) & 0xF) << 16) | ((g_spoof_ver & 0xF) << 8) | 0x10,
				/* forth line, release_XYZ + release_XYZ */ g_spoof_ver, g_spoof_ver
				);
	}
	
	/* return the decryption result */
	return res;
}

void PatchMesgled(u32 text_addr)
{
	/* if firmware is >= 6.30 */
	if (sceKernelDevkitVersion() >= 0x06030010)
	{
		int model = sceKernelGetModel();
		if(model == 6 || model == 8)
		{
			model = 3;
		}
		
		/* Patch mesgled to allow older updaters to boot */
		_sw(0, text_addr + g_patch_table.new_updater_check[model]);
	}
}

int sceMesgLed_driver_81F72B1F_patched(void *exec, u32 exec_size, u32 *out_size)
{
	/* do sony decryption */
	int res = sceMesgLed_driver_81F72B1F(exec, exec_size, out_size);
	
	/* check for error */
	if (res != 0)
	{
		/* lets decrypt */
		res = pspDecryptPRX(exec, exec_size, out_size);
	}
	
	/* return result */
	return res;
}

int memlmd_7CF1CD3E_patched(void *exec, u32 exec_size, u32 *out_size)
{
	/* do sony decryption */
	int res = memlmd_7CF1CD3E(exec, exec_size, out_size);
	
	/* check for error */
	if (res != 0)
	{
		/* lets decrypt */
		res = pspDecryptPRX(exec, exec_size, out_size);
	}
	
	/* return result */
	return res;
}
