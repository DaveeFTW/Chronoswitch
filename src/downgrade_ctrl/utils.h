/*
	Downgrade Control -> utils.h -> Provide API documentation and definitions for utilities
	by Davee
	
	30/12/2010
*/
#ifndef __UTILS__H__
#define __UTILS__H__

#define MAKE_JUMP(a, f)					_sw(0x08000000 | (((u32)(f) >> 2)  & 0x03ffffff), a)
#define MAKE_CALL(a, f)					_sw(0x0C000000 | (((u32)(f) >> 2)  & 0x03ffffff), a)
#define REDIRECT_FUNCTION(a, f) 		{ u32 address = a; _sw(0x08000000 | (((u32)(f) >> 2)  & 0x03ffffff), address);  _sw(0, address+4); }

#define KERNEL_HIJACK_FUNCTION(a, f, ptr)	{ \
											static u32 patch_buffer[3]; \
											_sw(_lw(a + 0x00), (u32)patch_buffer + 0x00); \
											_sw(_lw(a + 0x04), (u32)patch_buffer + 0x08);\
											MAKE_JUMP((u32)patch_buffer + 0x04, a + 0x08); \
											REDIRECT_FUNCTION(a, f); \
											ptr = (void *)patch_buffer; \
										}
/**
	Clears both the instruction and data caches
*/
void ClearCaches(void);

/**
	Allows to modify the kernel address called when a specific syscall is initiated
	
	@param addr: the address of the kernel function syscall you want to control
	@param newaddr: the new address the syscall will call
*/
void PatchSyscall(u32 addr, void *newaddr);

/**
	Find an export within the system
	
	@param modname: the name of the module containing the export
	@param libname: the library the export belongs to
	@param nid: the nid of the export
	
	@return the address of export else 0 on error
*/
u32 FindFunc(const char *modname, const char *lib, u32 nid);

#endif /* __UTILS__H__ */
