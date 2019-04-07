/*
	Downgrade Control -> utils.c -> Responsible for providing common utilities interfacing the kernel
	by Davee
	
	30/12/2010
*/

#include <pspkernel.h>
#include <pspsysmem_kernel.h>

#include <stdio.h>
#include <string.h>
#include <systemctrl.h>

#include "utils.h"

void ClearCaches(void)
{
	sceKernelIcacheClearAll();
	sceKernelDcacheWritebackAll();
}

u32 FindFunc(const char *modname, const char *lib, u32 nid)
{
	int i = 0, u;
	
	/* try and find the module by name */
	SceModule *mod = sceKernelFindModuleByName(modname);
	
	/* if fail */
	if (!mod)
	{
		/* fail */
		return 0;
	}
	
	/* copy over the structure data */
	u32 entry_size = mod->ent_size;
	u32 entry_start = (u32)mod->ent_top;
	
	/* loop until end of entry table */
	while (i < entry_size)
	{
		/* cast structure to memory */
		SceLibraryEntryTable *entry = (SceLibraryEntryTable *)(entry_start + i);
		
		/* if there is a libname, compare it to the lib else if there is no lib and there is no libname */
		if ((entry->libname && (strcmp(entry->libname, lib) == 0)) || (lib == NULL && entry->libname == NULL))
		{
			/* copy the table pointer and get the total number of entries */
			u32 *table = entry->entrytable;
			int total = entry->stubcount + entry->vstubcount;
			
			/* if there is some entries continue */
			if (total > 0)
			{ 
				/* loop through the entries */
				for (u = 0; u < total; u++)
				{
					/* if the nid matches */
					if (table[u] == nid)
					{
						/* return the pointer */
						return table[u + total];
					}
				} 
			} 	
		}
		
		/* increment the counter */
		i += (entry->len << 2);
	}
	
	/* could not find function */
	return 0;
}

void PatchSyscall(u32 addr, void *newaddr)
{
	u32 *vectors, i;
	
	/* get the vectors address from the co-processor */
	__asm__ volatile ("cfc0 %0, $12\n" "nop\n" : "=r" (vectors));
	
	/* loop through them */
	for (i = 0; i < 0x1000; i++)
	{
		/* if this is the address */
		if (vectors[i + 4] == addr)
		{
			/* then replace it :D */
			vectors[i + 4] = (u32)newaddr;
		}
	}
}
