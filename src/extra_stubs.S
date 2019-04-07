	.set noreorder

#include "pspstub.s"

	STUB_START "sceNetInet",0x00090000,0x00010005
	STUB_FUNC  0x1BDF5D13,sceNetInetInetAton
	STUB_END
	
	STUB_START "sceUtility_private",0x40090000,0x00020005
	STUB_FUNC  0xE65F37C8,sceUtilityPowerRegisterCallback
	STUB_FUNC  0x9ABAFCC5,sceUtilityPowerunRegisterCallback
	STUB_END

	STUB_START	"sceNetIfhandle_lib",0x40090000,0x00010005
	STUB_FUNC	0xE80F00A4, sceNetMPulldown
	STUB_END
	
	STUB_START "UtilsForUser",0x40000011,0x00020005
	STUB_FUNC  0x920F104A,sceKernelIcacheInvalidateAll
	STUB_FUNC  0xB435DEC5,sceKernelDcacheWritebackInvalidateAll
	STUB_END