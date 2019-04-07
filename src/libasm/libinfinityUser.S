.set noreorder

#include "pspstub.s"

STUB_START "libinfinityUser",0x40090000,0x00040005
STUB_FUNC  0x0527FEC1,infGetVersion
STUB_FUNC  0xA69B7B7E,infGetCoreVersion
STUB_FUNC  0x3AE45BE1,infGetRedirectionStatus
STUB_FUNC  0xD81F887B,infSetRedirectionStatus
STUB_END
