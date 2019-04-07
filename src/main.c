/*
	Downgrade Launcher R1
	by Davee
	
	Fin-rev 24/01/2011
*/

#include <pspkernel.h>
#include <pspsdk.h>
#include <psputility.h>
#include <pspctrl.h>

#include <pspsysmem_kernel.h>
#include <psploadexec_kernel.h>
#include <psploadcore.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>

#include "utils.h"
#include "kernel_land.h"
#include "kernel_exploit.h"
#include "rebootex.h"

PSP_MODULE_INFO("Chronoswitch", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(3 << 10);

#define DOWNGRADER_VER	("5.0")

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
	
u32 get_updater_version(u32 is_pspgo)
{
	int i;
	char *fw_data;
	u32 pbp_header[0x28/4];
	u8 sfo_buffer[4 << 10];
	SfoHeader *header = (SfoHeader *)sfo_buffer;
    SfoEntry *entries = (SfoEntry *)((char *)sfo_buffer + sizeof(SfoHeader));
	
	/* Lets open the updater */
	char *file = (is_pspgo) ? ("ef0:/PSP/GAME/UPDATE/EBOOT.pbp") : ("ms0:/PSP/GAME/UPDATE/EBOOT.pbp");
	
	/* open file */
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0777);
	
	/* check for failure */
	if (fd < 0)
	{
		/* error firmware */
		return 0xFFF;
	}
	
	/* read the PBP header */
	sceIoRead(fd, pbp_header, sizeof(pbp_header));
	
	/* seek to the SFO */
	sceIoLseek32(fd, pbp_header[8/4], PSP_SEEK_SET);
	
	/* calculate the size of the SFO */
	u32 sfo_size = pbp_header[12/4] - pbp_header[8/4];
	
	/* check if greater than buffer size */
	if (sfo_size > sizeof(sfo_buffer))
	{
		/* too much */
		sceIoClose(fd);
		return 0xFFF;
	}
	
	/* read the sfo */
	sceIoRead(fd, sfo_buffer, sizeof(sfo_buffer));
	
	/* close the file */
	sceIoClose(fd);
	
	/* now parse the SFO */
	for (i = 0; i < header->count; i++)
	{
		/* check this name */
		if (strcmp((char *)((char *)sfo_buffer + header->keyofs + entries[i].nameofs), "UPDATER_VER") == 0)
		{
			/* get the string */
			fw_data = (char *)((char *)sfo_buffer + header->valofs + entries[i].valofs);
			break;
		}
	}
	
	/* see if we went through all the data */
	if (i == header->count)
	{
		return 0xFFF;
	}
	
	/* return the firmware version */
	return (((fw_data[0] - '0') & 0xF) << 8) | (((fw_data[2] - '0') & 0xF) << 4) | (((fw_data[3] - '0') & 0xF) << 0);
}

int main(int argc, char *argv[])
{
	int res;
	SceCtrlData pad_data;
	u32 cur_buttons, prev_buttons = 0;

#ifdef HBL_SUKKIRI
	pspUtilityHtmlViewerParam html_param;
#endif
	
	/* initialise the PSP screen */
	pspDebugScreenInit();
	pspDebugScreenSetTextColor(0x00D05435);
	
	/* display welcome message */
	printf(
		"Chronoswitch Downgrader" "\n"
		"Version %s. Built %s %s" "\n" "\n"
		
		"Contributions:" "\n"
		"\t"	"6.31/6.35 Support added by Davee" "\n"
		"\t"	"6.38/6.39/6.60 Support added by some1" "\n" "\n"
		
		"Web:" "\n"
		"\t"	"http://lolhax.org" "\n" "\n"
		, DOWNGRADER_VER, __DATE__, __TIME__);

#ifdef HBL_SUKKIRI	
	/* Clear html param to 0 */
	memset(&html_param, 0, sizeof(pspUtilityHtmlViewerParam));
	
	/* set enough params in html viewer to get through to module loading */
	html_param.base.size = sizeof(pspUtilityHtmlViewerParam);
	html_param.base.accessThread = 0xF;
	
	/* call sceUtilityHtmlViewerInitStart to load the htmlviewer_utility.prx which imports sceutility/scepower exploit */
	res = sceUtilityHtmlViewerInitStart(&html_param);
	
	/* check error */
	if (res < 0)
	{
		/* this could be an HBL resolving issue... */
		ErrorExit(5000, "Error 0x%08X starting htmlviewer\n", res);
	}
	
	/* wait a second for htmlviewer to get loaded */
	sceKernelDelayThread(1 * 1000 * 1000);
#endif
	
	/* check firmware*/
	printf("Checking firmware... ");
	
	/* do the kernel exploit */
	doKernelExploit();
	
	/* printf ok message */
	printf("OK\n");
	
	/* set the devkit */
	g_devkit_version = sceKernelDevkitVersion();
	
	/* get the PSP model */
	int model = execKernelFunction(getModel);
	int true_model = model;
	
	/* check for real model if it claims it is a 04g (can be 09g) */
	if (model == 3)
	{
		/* get the baryon */
		u32 baryon = execKernelFunction(getBaryon);
		
		/* now get the determinating model */
		u32 det_model = (baryon >> 16) & 0xFF;
		
		/* now check if it is within range */
		if (det_model >= 0x2E && det_model < 0x30)
		{
			/* it's a 09g (or a sneaky 07g...) */
			if ((baryon >> 24) == 1)
			{
				/* 07g!! */
				true_model = 6;
			}
			else
			{
				/* 09g */
				true_model = 8;
			}
		}
	}
	
	/* display model */
	printf("Your PSP reports model %02ig.\n", model+1);
	
	/* check if real != true */
	if (true_model != model)
	{
		/* display */
		printf("Your PSP is originally a %02ig model.\n", true_model + 1);
		ErrorExit(10000, "Due to the experimental nature of the whole 09g to 04g downgrade, functionality to change firmware is prohibited through this program.");
	}
	
	/* delay the thread */
	sceKernelDelayThread(5*1000*1000);
	
	/* check for 09g, we treat this as a 04g */
	if(model == 8)
	{
		model = 3;
	}
	
	/* check for unsupported model */
	if (model != 0 &&			/* PSP PHAT */
		model != 1 &&			/* PSP SLIM */
		model != 2 &&			/* PSP 3000 */
		model != 3 &&			/* PSP 4000 */
		model != 4				/* PSPgo */
	)
	{
		/* unsupported */
		ErrorExit(5000, "PSP %02ig not supported.\n", model+1);
	}
	
	/* check for pspgo */
	if (model == 4)
	{
		printf("\n" "Your PSPgo will require deletion of the [Resume Game] feature. Proceed? (X = Yes, R = No)\n");
		
		while (1)
		{
			sceCtrlPeekBufferPositive(&pad_data, 1);
			
			/* filter out previous buttons */
			cur_buttons = pad_data.Buttons & ~prev_buttons;
			prev_buttons = pad_data.Buttons;
			
			/* check for cross */
			if (cur_buttons & PSP_CTRL_CROSS)
			{
				break;
			}
			
			else if (cur_buttons & PSP_CTRL_RTRIGGER)
			{
				ErrorExit(5000, "Exiting in 5 seconds.\n");
			}
		}
		
		/* delete resume game */
		if (execKernelFunction(delete_resume_game) < 0)
		{
			/* ERROR */
			ErrorExit(5000, "Error deleting [Resume Game]. Exiting for safety reasons.\n");
		}
	}
	
	/* get the updater version */
	u32 upd_ver = get_updater_version(model == 4);
	
	/* do confirmation stuff */
	printf("Will attempt to Downgrade: %X.%X -> %X.%X.\n", (g_devkit_version >> 24) & 0xF, ((g_devkit_version >> 12) & 0xF0) | ((g_devkit_version >> 8) & 0xF), (upd_ver >> 8) & 0xF, upd_ver & 0xFF);
	printf("X to continue, R to exit.\n");
	
	/* get button */
	while (1)
	{
		sceCtrlPeekBufferPositive(&pad_data, 1);

		/* filter out previous buttons */
		cur_buttons = pad_data.Buttons & ~prev_buttons;
		prev_buttons = pad_data.Buttons;
		
		/* check for cross */
		if (cur_buttons & PSP_CTRL_CROSS)
		{
			break;
		}
		
		else if (cur_buttons & PSP_CTRL_RTRIGGER)
		{
			ErrorExit(5000, "Exiting in 5 seconds.\n");
		}
	}
	
	/* clear screen */
	pspDebugScreenClear();
	
	/* update should be OK, go for it */
	printf("By running this application and launching the SCE updater you accept all responsibility of any damage, temporary or permament, that may occur when using this application. This application has been tested with no loss of functionality or any damage to the system, however  it cannot be guaranteed to be completely safe." "\n" "BY RUNNING THIS APPLICATION YOU ACCEPT ALL THE RISK INVOLVED.\n\n" "Press X to start SCE updater. Press R to exit\n");
	
	while (1)
	{
		sceCtrlPeekBufferPositive(&pad_data, 1);

		/* filter out previous buttons */
		cur_buttons = pad_data.Buttons & ~prev_buttons;
		prev_buttons = pad_data.Buttons;
		
		/* check for cross */
		if (cur_buttons & PSP_CTRL_CROSS)
		{
			break;
		}
		
		else if (cur_buttons & PSP_CTRL_RTRIGGER)
		{
			ErrorExit(5000, "Exiting in 5 seconds.\n");
		}
	}

	printf("OK, good for launch\n");
	
	/* go go go go go */
	res = execKernelFunction(launch_updater);
	
	printf("loading SCE updater failed = 0x%08X\n", res);
	sceKernelDelayThread(5 *1000*1000);
	sceKernelExitGame();
	return 0;
}
