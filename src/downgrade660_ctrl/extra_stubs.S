	.set noreorder

#include "pspstub.s"
	
	STUB_START	"sceResmgr_driver",0x00090011,0x00010005
	STUB_FUNC	0x9DC14891,sceResmgr_driver_9DC14891
	STUB_END
	
	STUB_START	"memlmd",0x00090011,0x00010005
	STUB_FUNC	0x7CF1CD3E,memlmd_7CF1CD3E
	STUB_END
	
	STUB_START	"semaphore",0x00090011,0x00020005
	STUB_FUNC	0x4C537C72,sceUtilsBufferCopyWithRange
	STUB_FUNC	0x77E97079,sceUtilsBufferCopyByPollingWithRange
	STUB_END

	STUB_START	"sceIdStorage_driver",0x00090011,0x00010005
	STUB_FUNC	0x6FE062D1,sceIdStorageLookup
	STUB_END