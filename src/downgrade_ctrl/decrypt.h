/*
	Downgrade Control -> decrypt.h -> Provide API documentation and definitions
	by Davee
	
	31/12/2010
*/
#ifndef __DECRYPT_H__
#define __DECRYPT_H__

/* our functions */
void PatchMesgled(u32 text_addr);
int memlmd_7CF1CD3E_patched(void *exec, u32 exec_size, u32 *out_size);
int sceResmgr_9DC14891_patched(void *data, u32 datasize, u32 *outsize);
int sceMesgLed_driver_81F72B1F_patched(void *exec, u32 size, u32 *outsize);

extern int (* sceMesgLed_driver_81F72B1F)(void *exec, u32 size, u32 *outsize);

/* prototypes */
int memlmd_7CF1CD3E(void *exec, u32 exec_size, u32 *out_size);
int sceResmgr_driver_9DC14891(void *data, u32 datasize, u32 *outsize);
int sceUtilsBufferCopyWithRange(void *dst, u32 dst_size, void *src, u32 src_size, u32 cmd);
int sceUtilsBufferCopyByPollingWithRange(void *dst, u32 dst_size, void *src, u32 src_size, u32 cmd);
int sceKernelPowerLock(void);
int sceKernelPowerUnlock(void);

/* data structures */
typedef struct
{
	u32 tag;
	u8 key[16];
} mesgled_keys_struct;

typedef struct
{
	u32 signature; 		//0
	u16 mod_attribute; 	//4
	u16 comp_attribute;	//6
	u8 module_ver_lo;	//8
	u8 module_ver_hi;	//9
	char modname[28];	//0xA
	u8 mod_version;		//0x26
	u8 nsegments;		//0x27
	u32 elf_size;		//0x28
	u32 psp_size;		//0x2C
	u32 boot_entry;		//0x30
	u32 modinfo_offset; //0x34
	int bss_size;		//0x38
	u16 seg_align[4];	//0x3C
	u32 seg_address[4];	//0x44
	int seg_size[4];	//0x54
	u32 reserved[5];	//0x64
	u32 devkit_version;	//0x78
	u8 decrypt_mode;	//0x7C
	u8 padding;			//0x7D
	u16 overlap_size;	//0x7E
	u8 key_data[0x30];	//0x80
	u32 comp_size;		//0xB0
	int _80;			//0xB4
	u32 unk_B8;			//0xB8
	u32 unk_BC;			//0xBC
	u8 key_data2[0x10];	//0xC0
	u32 tag;			//0xD0
	u8 scheck[0x58];	//0xD4
	u8 sha1_hash[0x14];	//0x12C
	u8 key_data4[0x10]; //0x140
} PSP_Header2; //0x150

typedef struct
{
	u32 mode; //0
	u32 unk_4;
	u32 unk_8;
	u32 keyseed; //12
	u32 size; //16
} KIRK_CMD_HEADER; //20

#define KIRK_CMD_7		(7)
#define KIRK_CMD_SHA1	(0xB)

#endif /* __DECRYPT_H__ */
