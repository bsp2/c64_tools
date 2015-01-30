
export C64_TOOLS_DSP_ROOT=`pwd`
export C64_TOOLS_ROOT=`pwd`/..

export TI_ROOT=${HOME}/ti

# where the C64+ compiler is installed
export TI_CGTOOLS="${TI_ROOT}/TI_CGT_C6000_7.4.5"

# where the DSP/BIOS 5.xx includes/libs are installed
export TI_DSPBIOS="${TI_ROOT}/bios_5_42_01_09"
#export TI_DSPBIOS="${TI_ROOT}/bios_5_40_03_23"
#export TI_DSPBIOS="${TI_ROOT}/bios_5_41_10_36"
#export TI_DSPBIOS="${TI_ROOT}/bios_5_41_04_18"
#export TI_DSPBIOS="${TI_ROOT}/bios_5_33_06"

# where the XDCTOOLS (tconf, ..) are installed
export TI_XDCTOOLS="${TI_DSPBIOS}/xdctools"
#export TI_XDCTOOLS="${TI_ROOT}/xdctools_3_25_03_72"


# where fastRTS (FPU emu.) includes/libs are installed
export TI_FASTRTS="${TI_ROOT}/fastRTS_c62xc64x_1_42"

# where iqmath (fixed point math) includes/libs are installed
export TI_IQMATH="${TI_ROOT}/c64xplus-iqmath_2_01_04_00"


# add DSP cross compiler tools (cg6x, ..) to PATH
export PATH="${PATH}:${TI_CGTOOLS}/bin"


# utility alias for invoking make
alias m="make -f makefile.c64p"
