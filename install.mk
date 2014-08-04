#
# GPP-side build system configuration
#

MAKE=make
RM=rm -f
CC=$(CROSS_COMPILE)gcc

MAKEFILE=makefile.linux

CFLAGS= -Wall

ifdef CROSS_ROOT
CFLAGS+= -I$(CROSS_ROOT)/usr/include
endif

CFLAGS += -Werror

EXTRAFLAGS=

CFGFLAGS= -DHAVE_OS_LINUX

LDFLAGS= -Wl,-R./ 

EXTRALIBS=

ifdef CROSS_ROOT
EXTRALIBS+= -L$(CROSS_ROOT)/usr/lib
endif

