
CC=cl6x
AR=ar6x
MAKE=make
RM=rm -f
DATE=/bin/date
TCONF=$(TI_XDCTOOLS)/tconf -b -Dconfig.importPath="$(TI_DSPBIOS)/packages;"


MAKEFILE=makefile.c64p

OBJDIR=obj

WFLAGS= --display_error_number --diag_warning=225 --set_error_limit=10

CFLAGS= $(WFLAGS) --abi=coffabi -mv64+ --define=CCS --gcc

CFLAGS+= --symdebug:none

CFLAGS += --include_path="$(TI_CGTOOLS)/include"
CFLAGS += --include_path="$(TI_DSPBIOS)/packages/ti/bios/include"
CFLAGS += --include_path="$(TI_DSPBIOS)/packages/ti/rtdx/include"

ifneq ($(SKIP_MATH_LIBS),1)
CFLAGS += --include_path="$(TI_FASTRTS)/c6400/C_fastRTS/include"
CFLAGS += --include_path="$(TI_IQMATH)/include"
endif

CFLAGS += --include_path="$(C64_TOOLS_DSP_ROOT)"

OPTFLAGS_NONE= -O0
OPTFLAGS_NONE_INTERLIST= -O0 --optimizer_interlist
OPTFLAGS_SPEED= -O3 --opt_for_speed=5 --auto_inline=512
OPTFLAGS_SPEED_INTERLIST= -O3 --optimizer_interlist --opt_for_speed=5 --auto_inline=512
OPTFLAGS_SPEED2= -O2 --opt_for_speed=3
OPTFLAGS_SPEED2_INTERLIST= -O2 --optimizer_interlist --opt_for_speed=3
OPTFLAGS_SIZE= -O0 -ms3
OPTFLAGS_SIZE_INTERLIST= -O0 -ms3 --optimizer_interlist

#LDFLAGS= $(WFLAGS) -z --warn_sections -mv64+ --reread_libs --rom_model
LDFLAGS= $(WFLAGS) -mv64+ -z --warn_sections 
LDFLAGS+= --rom_model

# library directories
LIBDIR=
LIBDIR+= -i"$(TI_CGTOOLS)/lib"
LIBDIR+= -i"$(TI_DSPBIOS)/packages/ti/rtdx/lib/c6000"
LIBDIR+= -i"$(TI_DSPBIOS)/packages/ti/bios/lib"


# project specific optimization flags
OPTFLAGS=

# project specific includes
INC=

# project specific libraries
LIB=

# other libs
LIB_PRE=

# sys libs
LIB_POST=

ifneq ($(SKIP_MATH_LIBS),1)
LIB_POST+= -l"$(TI_FASTRTS)/c6400/mthlib/lib/fastrts64x.lib"
LIB_POST+= -l"$(TI_IQMATH)/lib/IQmath_c64x+.lib"
LIB_POST+= -l"$(TI_IQMATH)/lib/IQmath_RAM_c64x+.lib"
endif

LIB_POST+= -l"libc.a"


# additional clean targets
CLEAN_RULES=
