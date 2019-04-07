/*
	Downgrade Launcher -> utils.c -> Responsible for providing common utilities
	by Davee
	
	28/12/2010
*/

#include <pspkernel.h>
#include <pspsysmem_kernel.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "utils.h"
#include "kernel_land.h"

void sceKernelIcacheInvalidateAll(void);

int isValidUserAddress(void *addr)
{
	return ((u32)((u32)addr - 0x08800000) < (24 << 20)) ? (1) : (0);
}

void KClearCaches(void)
{
	/* Clear the Icache */
	asm("\
	.word 0x40088000; .word 0x24091000; .word 0x7D081240;\
	.word 0x01094804; .word 0x4080E000; .word 0x4080E800;\
	.word 0x00004021; .word 0xBD010000; .word 0xBD030000;\
	.word 0x25080040; .word 0x1509FFFC; .word 0x00000000;\
	"::);
	
	/* Clear the dcache */
	asm("\
	.word 0x40088000; .word 0x24090800; .word 0x7D081180;\
	.word 0x01094804; .word 0x00004021; .word 0xBD140000;\
	.word 0xBD140000; .word 0x25080040; .word 0x1509FFFC;\
	.word 0x00000000; .word 0x0000000F; .word 0x00000000;\
	"::);
}

void ClearCaches(void)
{
	sceKernelDcacheWritebackInvalidateAll();
	sceKernelIcacheInvalidateAll();
}

u32 FindProc(char *modname, char *lib, u32 nid)
{
	/* declare our local vars */
	int i = 0, u;
	
	/* find the module */
	SceModule2 *mod = pspKernelFindModuleByName(modname);
	
	/* if no mod, error */
	if (mod == NULL)
	{
		return 0;
	}
	
	/* get the entry info */
	u32 entry_size = mod->ent_size;
	u32 entry_start = (u32)mod->ent_top;
	
	/* scan through the export list */
	while (i < entry_size)
	{
		/* point to the entry */
		SceLibraryEntryTable *entry = (SceLibraryEntryTable *)(entry_start + i);
		
		/* if there is a libname, check if it's the libname we want */
		if (entry->libname && (strcmp((char *)entry->libname, lib) == 0))
		{
			/* now lets scan through the stubs for our nid */
			u32 *table = entry->entrytable;
			int total = entry->stubcount + entry->vstubcount;
			
			/* if there is nids, lets continue */
			if (total > 0)
			{ 
				/* scan through the nidtable */
				for (u = 0; u < total; u++)
				{ 
					/* check if its the nid we're looking for */
					if (table[u] == nid)
					{
						/* our nid, let return the address */
						return table[u + total];
					}
				} 
			} 	
		}
		
		/* update entry counter */
		i += (entry->len << 2);
	}
	
	/* lib not found ): */
	return 0;
}

void ErrorExit(int millisecs, char *fmt, ...)
{
	va_list list;
	char msg[256];	

	/* collate args into string */
	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	/* print string */
	printf(msg);
	
	sceKernelDelayThread(millisecs*1000);
	sceKernelExitGame();
}
