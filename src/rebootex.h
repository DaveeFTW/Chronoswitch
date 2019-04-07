/*
	Downgrade Launcher -> rebootex.h -> Provide API documentation and definitions for the reboot patching processes
	by Davee
	
	28/12/2010
*/
#ifndef __REBOOTEX_H__
#define __REBOOTEX_H__

#define TAG_631			(0x4C9484F0)
#define TAG_635			(0x4C9484F0)
#define TAG_638			(0x4C948AF0)
#define TAG_639			(0x4C948AF0)
#define TAG_660			(0x4C9494F0)
#define MODULE_ID_TAG	(0x626F6F42)

/* variables */
extern u32 g_devkit_version;

/* Functions */
int RebootEntryPatched(void *reboot_param, void *exec_param, u32 api, u32 initial_rnd);

typedef struct BtcnfHeader
{
	u32 signature; // 0
	u32 devkit;		// 4
	u32 unknown[2];  // 8
	u32 modestart;  // 0x10
	int nmodes;  // 0x14
	u32 unknown2[2];  // 0x18
	u32 modulestart; // 0x20
	int nmodules;  // 0x24
	u32 unknown3[2]; // 0x28
	u32 modnamestart; // 0x30
	u32 modnameend; // 0x34
	u32 unknown4[2]; // 0x38
}  __attribute__((packed)) BtcnfHeader;

typedef struct ModeEntry
{
	u16 maxsearch;
	u16 searchstart; //
	int mode1;
	int mode2;
	int reserved[5];
} __attribute__((packed)) ModeEntry;

typedef struct ModuleEntry
{
	u32 stroffset; // 0
	int reserved; // 4
	u16 flags; // 8
	u8 loadmode; // 10
	u8 signcheck; // 11
	int reserved2; // 12
	u8  hash[0x10]; // 16
} __attribute__((packed)) ModuleEntry; // 32

#endif /* __REBOOTEX_H__ */
