#!/bin/sh

# ----
# ---- file   : go64.sh
# ---- author : Bastian Spiegel <bs@tkscript.de>
# ---- legal  : (c) 2013 by Bastian Spiegel. 
# ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
# ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING_GPL for further information.
# ----
# ---- info   : This is part of the "c64_tools" package.
# ----
# ---- changed: 06Sep2013, 08Sep2013, 14Sep2013, 16Sep2013, 18Sep2013, 01Oct2013, 13Dec2013
# ----
# ----
#

#
#
# You need to be root in order to run this:
#
#  $ sudo ./go64.sh [c64_load args] [c64_load dspimage pathname]
#
# Enable kernel module debug output by setting the C64_DEBUG variable
#  before running this script:
#
#  $ export C64_DEBUG=1
#
# To disable debug output, unset the variable:
#
#  $ unset C64_DEBUG
#
# then re-run this script.
#
#
# In order to enable power management klog traces (DSP suspend/resume), set
#  the C64_PWRLOG variable before running this script:
#
#  $ export C64_PWRLOG=1
#
#

#export C64_DEBUG=10

#ARGS=-vv
#ARGS=-vv
#ARGS=-v
ARGS=

DSPIMAGE=./core.out

LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:./"


if [ $# -eq 2 ]; then
   # args + DSP image pathname
   ARGS="$1"
   DSPIMAGE="$2"
else
   if [ $# -eq 1 ]; then
      if [ "x`echo $1 | cut -c -1`" = "x-" ]; then
         # args only
         ARGS="$1"
      else
         # DSP image pathname only
         DSPIMAGE="$1"
      fi
   else 
      if [ $# -eq 0 ]; then
        # use default args
        echo " "
      else
        echo "Usage: sudo go64.sh [c64_load args] [c64_load dspimage pathname]"
        exit 10;
      fi
   fi
fi



# Unload kernel module if it's already loaded
if [ "x" != "x`/sbin/lsmod | grep c64`" ] ; then
  /sbin/rmmod c64
  rm -f /dev/c64
fi


# Load kernel module
KMODARGS=
if [ "x" != "x$C64_DEBUG" ]; then
  KMODARGS="debug=$C64_DEBUG"
fi

if [ "x" != "x$C64_PWRLOG" ]; then
  KMODARGS_PWRLOG="pwrlog=$C64_PWRLOG"
else
  KMODARGS_PWRLOG=
fi

/sbin/insmod ./c64.ko $KMODARGS $KMODARGS_PWRLOG


sleep 0.25


# Grant everyone access to the c64 driver 
chmod 0666 /dev/c64


# Load COFF2 DSP image and start the DSP
echo "go64.sh: ARGS=\"${ARGS}\" DSPIMAGE=\"${DSPIMAGE}\""
./c64_load ${ARGS} ${DSPIMAGE}
