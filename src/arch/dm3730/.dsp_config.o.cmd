cmd_/bsp/pandora/c64_tools/kmod/../src/arch/dm3730/dsp_config.o := arm-none-linux-gnueabi-gcc -Wp,-MD,/bsp/pandora/c64_tools/kmod/../src/arch/dm3730/.dsp_config.o.d  -nostdinc -isystem /bsp/pandora-dev/arm-2011.09/bin/../lib/gcc/arm-none-linux-gnueabi/4.6.1/include -I/bsp/pandora-dev/arm-2011.09/usr/src/pandora-kernel/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include /bsp/pandora-dev/arm-2011.09/usr/src/pandora-kernel/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-omap2/include -Iarch/arm/plat-omap/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -O2 -marm -fno-dwarf2-cfi-asm -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO  -DMODULE  -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(dsp_config)"  -D"KBUILD_MODNAME=KBUILD_STR(c64)" -c -o /bsp/pandora/c64_tools/kmod/../src/arch/dm3730/.tmp_dsp_config.o /bsp/pandora/c64_tools/kmod/../src/arch/dm3730/dsp_config.c

source_/bsp/pandora/c64_tools/kmod/../src/arch/dm3730/dsp_config.o := /bsp/pandora/c64_tools/kmod/../src/arch/dm3730/dsp_config.c

deps_/bsp/pandora/c64_tools/kmod/../src/arch/dm3730/dsp_config.o := \
  /bsp/pandora/c64_tools/kmod/../src/arch/dm3730/../../../include/types.h \
  /bsp/pandora/c64_tools/kmod/../src/arch/dm3730/../../../include/cplusplus_begin.h \
  /bsp/pandora/c64_tools/kmod/../src/arch/dm3730/../../../include/cplusplus_end.h \
  /bsp/pandora/c64_tools/kmod/../src/arch/dm3730/../../../include/dsp_common.h \
  /bsp/pandora/c64_tools/kmod/../src/arch/dm3730/../../../include/dsp_config.h \
    $(wildcard include/config/h//.h) \

/bsp/pandora/c64_tools/kmod/../src/arch/dm3730/dsp_config.o: $(deps_/bsp/pandora/c64_tools/kmod/../src/arch/dm3730/dsp_config.o)

$(deps_/bsp/pandora/c64_tools/kmod/../src/arch/dm3730/dsp_config.o):
