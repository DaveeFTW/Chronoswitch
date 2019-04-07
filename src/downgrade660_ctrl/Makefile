release: all
	psp-fixup-imports -m $(PSP_FW_VERSION)mapfile.txt $(TARGET).prx
	psp-packer $(TARGET).prx
	bin2c $(TARGET).prx ../$(TARGET).h $(TARGET)
	
TARGET = downgrade660_ctrl
OBJS = main.o utils.o patch_table.o decrypt.o pspdecrypt.o extra_stubs.o ../libasm/libinfinityKernel.o

INCDIR = ../include
CFLAGS = -Os -G0 -Wall -fno-pic -fshort-wchar
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

PSP_FW_VERSION = 660

USE_KERNEL_LIBS=1
USE_KERNEL_LIBC=1

LIBDIR = ../lib
LDFLAGS = -nostartfiles
LIBS = -lpspsystemctrl_kernel

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

