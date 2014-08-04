#
# Makefile for "c64.ko" kernel module. This is part of the "c64_tools" package.
#
#  tested with Open Pandora kernel 3.2.45, Code Sourcery gcc 4.6.1. 
#
#     (major 239 is unassigned)
#
#  (note) please set CROSS_COMPILE and CROSS_ROOT (or CROSS_KERNEL) before running "make"
#
#          $ export CROSS_COMPILE=arm-none-linux-gnueabi-
#          $ export CROSS_ROOT=/bsp/pandora-dev/arm-2011.09
#             (must contain "usr/src/pandora-kernel/" sub-directory)
#
#            or
#          $ export CROSS_KERNEL=/bsp/pandora-dev/arm-2011.09/usr/src/pandora-kernel/
#
#

#
#  (note) the "ram" field must be constant for all binary compatible DSP images.
#
DSP_CONFIG=../src/arch/dm3730/dsp_config

ALL_HDR= reg.h mlb.h dsp_c64.h dev.h kmod.h ../include/dsp_common.h ../include/arch/dm3730/dm3730.h
ALL_SRC= reg.c mlb.c dsp_c64.c dev.c kmod.c minim.c $(DSP_CONFIG).c


c64-objs += reg.o
c64-objs += mlb.o
c64-objs += dsp_c64.o
c64-objs += dev.o
c64-objs += kmod.o
c64-objs += minim.o
c64-objs += $(DSP_CONFIG).o

obj-m += c64.o

ifdef CROSS_KERNEL
KDIR := $(CROSS_KERNEL)
else
ifdef CROSS_ROOT
KDIR := $(CROSS_ROOT)/usr/src/pandora-kernel
else
KDIR := /usr/src/linux
endif
endif

PWD := $(shell pwd)

bin: $(ALL_SRC) $(ALL_HDR)
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
