/*
	Downgrade Control -> patch_table.c -> Responsible for searching and handling patch tables
	by Davee
	
	01/01/2011
*/

#include <pspkernel.h>
#include <pspsysmem_kernel.h>

#include <stdio.h>
#include <string.h>
#include <systemctrl.h>

#include "patch_table.h"

PatchTable g_patch_table;

int CopyPatchTable(PatchTable *dst_table, void *_src_table, u32 devkit)
{
	int i;
	
	/* get the number of entries in the table */
	u32 nentries = _lw((u32)_src_table);
	
	/* cast our pointer */
	PatchTable *src_table = (PatchTable *)(_src_table + 4);
	
	/* loop through the entries */
	for (i = 0; i < nentries; i++)
	{
		/* if same devkit */
		if (src_table->devkit == devkit)
		{
			/* copy over and return 1 for a complete transfer */
			memcpy(dst_table, src_table, sizeof(PatchTable));
			return 1;
		}
		
		/* increment */
		src_table++;
	}
	
	/* no transfer */
	return 0;
}
