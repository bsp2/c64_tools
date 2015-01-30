
export C64_TOOLS_ROOT=`pwd`

# utility alias for invoking make
alias m="make -f makefile.linux"


# CROSS_COMPILE must be set
export CROSS_COMPILE=arm-none-linux-gnueabi-

# CROSS_ROOT must be set if the cross-compiler can't find the target includes/libs
#  it is also used to find the kernel source directory, in case CROSS_KERNEL is not set
#export PNDROOT=${PNDSDK}/arm-2011.09
#export CROSS_ROOT=${PNDROOT}

# CROSS_KERNEL can be set to force a specific kernel source directory
#export CROSS_KERNEL=${CROSS_ROOT}/usr/src/pandora-kernel
export CROSS_KERNEL=${CROSS_ROOT}/usr/src/pandora-kernel-3.2.61
