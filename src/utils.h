/*
	Downgrade Launcher -> utils.h -> Provide documentation for standard proceedures
	by Davee
	
	28/12/2010
*/
#ifndef __UTILS__H__
#define __UTILS__H__

#define MAKE_JUMP(a, f)					_sw(0x08000000 | (((u32)(f) >> 2)  & 0x03ffffff), a)
#define MAKE_CALL(a, f)					_sw(0x0C000000 | (((u32)(f) >> 2)  & 0x03ffffff), a)
#define REDIRECT_FUNCTION(a, f) 		{ u32 address = a; _sw(0x08000000 | (((u32)(f) >> 2)  & 0x03ffffff), address);  _sw(0, address+4); }
#define MAKE_RELATIVE_BRANCH(a, f, t)	_sw((0x10000000 | ((((f - a) >> 2) - 1) & 0xFFFF)), a + t)

#define FIRMWARE_VERSION_631	(0x06030110)
#define FIRMWARE_VERSION_635	(0x06030510)
#define FIRMWARE_VERSION_638	(0x06030810)
#define FIRMWARE_VERSION_639	(0x06030910)
#define FIRMWARE_VERSION_660	(0x06060010)

#define PSPGO_UPDATER_PATH	"ef0:/PSP/GAME/UPDATE/EBOOT.PBP"
#define OTHER_UPDATER_PATH	"ms0:/PSP/GAME/UPDATE/EBOOT.PBP"

#define printf pspDebugScreenPrintf

/**
	Clears both the instruction and data caches USER MODE ONLY
*/
void ClearCaches(void);

/**
	Clears both the instruction and data caches KERNEL MODE ONLY
*/
void KClearCaches(void);

/**
	checks whether or not a pointer is a valid pointer into userspace
	
	@param addr: the pointer to check
	@return 1 on valid else 0 on invalid
*/
int isValidUserAddress(void *addr);

/**
	Find an export within the system
	
	@param modname: the name of the module containing the export
	@param libname: the library the export belongs to
	@param nid: the nid of the export
	
	@return the address of export else 0 on error
*/
u32 FindProc(char *modname, char *lib, u32 nid);

/**
	Display an error and exit game
	
	@param millisecs: the amount of time to delay before exiting (in milliseconds)
	@param fmt: a formattable string with args to follow
*/
void ErrorExit(int millisecs, char *fmt, ...);

/* internal prototypes */
void clearIcache(void);

#endif /* __UTILS__H__ */
