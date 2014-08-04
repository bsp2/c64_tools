c64_tools
=========

TI C64+ DSP loader and IPC utilities.


# About

 This package provides a couple of tools and libraries to
  support the C64+ DSP found in TI's OMAP3 hardware.

 It includes the following components:

      "c64_load" -- A COFF2 DSP image loader.
                     Must be run with root privileges.

                    This utility loads DSP images created with
                     CodeComposerStudio / DSPBIOS 5.xx.

                    Licensed under the terms of the GPL.

        "c64.ko" -- A Linux kernel module that handles messaging
                     between the ARM (GPP) and the C64+ (DSP).
                
                    It also handles shared memory allocation
                     (via the Linux CMA (contiguous memory allocator))
                     and provides a couple of utility functions
                     to writeback/invalidate the ARM data caches.

                    When the module is loaded, it creates the
                     "/dev/c64" device.

                    Licensed under the terms of the GPL.

        "libc64" -- A utility library to be used with client applications.

                    It provides access to the "c64.ko" kernel module features and
                     lets applications use remote procedure calls to call 
                     functions on the DSP side.

                    Licensed under the terms of the LGPL.

    "libc64_dsp" -- The DSP-side counterpart to "libc64".

                    It allows registration of DSP components whose
                     commands (methods) can then be called in GPP
                     applications.

                    Licensed under the terms of the MIT license.

       "go64.sh" -- A utility script that loads and starts a DSP image.


 "c64_tools" supports multiple processes / multiple clients, although
  each process is limited to one client connection.



# Changes

## 18-Jan-2014
   - changed DSP power on/off sequences to match those found in TI's SysLink.

      This seems to fix stability issues on older OMAP3530 based Pandora devices
      (newer DM3730 based devices worked fine with the old sequences)
      
      Tests indicated that the stability issues were related to the way the DSP
       was powered off in previous releases.

      With the new power on/off code, the power consumption during c64_pwrbench
       (powercycling stresstest) dropped from ~2.7W to ~1.6W.

      DSP idle power consumption also dropped from ~80mW to ~10mW
       (when the 'c64' device is not in use, the DSP will still be completely
        powered off, though).

      The new power up sequence also keeps the IVA2 video sequencer (ARM926) in reset.

   - added: 'pwroff' kernel module parameter
      Setting this to 0 (i.e. 'insmod c64.ko pwroff=0') will prevent the kernel module
       from powering off the DSP when there are no client applications connected or
       when all DSP applications have called dsp_suspend().

      The DSP will still be powered off when the kernel module is unloaded via 'rmmod'.

   - changed: removed dsp_send_forced_message() call in c64_load/main.c
      (this was only needed by very early version of c64_tools)

   - added TC_L2SRAM_RAND_CHKSUM_* testcases (46..49) to 'c64_tc'

      full list of 'c64_tc' testcases:

                01: TC_TESTMSG
                02: TC_RPC_ADD
                03: TC_RPC_ADD_BENCHMARK
                04: TC_IOCTL_TEST
                05: TC_SHM_ACCESS
                06: TC_FASTCALL_RPC_ADD
                07: TC_L1SRAM_SPEED
                08: TC_PWR_OFF
                09: TC_RESUME
                10: TC_IDLE
                11: TC_SHM_CACHE_NONE
                12: TC_SHM_CACHE_RW
                13: TC_SHM_CACHE_R
                14: TC_SHM_CACHE_W
                15: TC_L1SRAM_ALLOCATOR
                16: TC_SHM_MSPACE
                17: TC_CTRLC
                18: TC_FASTCALL_CTRLC
                19: TC_LOGBUF
                20: TC_LOGBUF_MANY
                21: TC_LOGBUF_SPLITLINE
                22: TC_LOGBUF_NO_LF
                23: TC_QDMA_COPY1D
                24: TC_QDMA_COPY2D
                25: TC_SHM_CACHE_NONE_RAND_DSP
                26: TC_SHM_CACHE_RW_RAND_DSP
                27: TC_SHM_CACHE_R_RAND_DSP
                28: TC_SHM_CACHE_W_RAND_DSP
                29: TC_SHM_CACHE_NONE_RAND_GPP
                30: TC_SHM_CACHE_RW_RAND_GPP
                31: TC_SHM_CACHE_R_RAND_GPP
                32: TC_SHM_CACHE_W_RAND_GPP
                33: TC_SHM_CACHE_NONE_RAND_DSP_VIRT
                34: TC_SHM_CACHE_RW_RAND_DSP_VIRT
                35: TC_SHM_CACHE_R_RAND_DSP_VIRT
                36: TC_SHM_CACHE_W_RAND_DSP_VIRT
                37: TC_SHM_CACHE_NONE_RAND_GPP_VIRT
                38: TC_SHM_CACHE_RW_RAND_GPP_VIRT
                39: TC_SHM_CACHE_R_RAND_GPP_VIRT
                40: TC_SHM_CACHE_W_RAND_GPP_VIRT
                41: TC_SHM_CACHE_HUGETLB_V2P2V
                42: TC_SHM_CACHE_HUGETLB
                43: TC_L2SRAM_ALLOCATOR
                44: TC_QDMA_LINK1D
                45: TC_QDMA_LINK2
                46: TC_L2SRAM_RAND_CHKSUM_DSP
                47: TC_L2SRAM_RAND_CHKSUM_GPP
                48: TC_L2SRAM_RAND_CHKSUM_DSP_FC
                49: TC_L2SRAM_RAND_CHKSUM_GPP_FC



## 15-Dec-2013
   - changed: the CROSS_ROOT env.var. is optional now
      (if it is not set, CROSS_KERNEL _must_ be set)
   - added support for CROSS_KERNEL env.var. to force specific kernel source directory
   - changed some debug messages to use KERN_DEBUG instead of KERN_INFO
      (cosmetic change, only relevant for real (non-X11) consoles)
   - fixed: dsp_power_notify() was not called if 'pwrlog' kmod option was set to 0 (false)
   - changed: decreased udelay()s in kmod/dsp_c64.c
   - added proper DSP startup sync (wait for signal/flag from DSP instead of udelay())
   - added 'c64_pwrbench' testcase
      (benchmarks DSP suspend/resume latency (~1.37 millisec @800Mhz, ~2.84 millisec @200Mhz))


## 13-Dec-2013
   - added c64_memview graphics example
      (a realtime memory monitor. quite useless but fun. looks best after a fresh
       reboot (unfragmented memory), e.g. load LibreOffice)
   - added 'sramtest' DSP component
      (various L1/L2 access pattern tests)
   - added 'c64_sramtest' example
   - optimized 'dsprite' DSP graphics component
      Performance @800Mz is now (2000 32x32 ARGB32 sprites):
                           copy: 249 MPixel/sec
                      alphatest: 133 MPixel/sec
        premul_srcover_saturate: 132 MPixel/sec
                        srcover: 113 MPixel/sec
   - added 'age' DSP graphics component (WIP)
      (Amiga "copper" style software display controller that currently emulates
        up to 8 bitplanes, and up to 8 scrollregisters, ..work in progress..
        currently performancs @~90 MPixel/sec (8 bitplanes, ARGB32 target))
   - added c64_age graphics example (WIP)
   - added qdma_link1d() syscall
   - added qdma_link2d() syscall
   - fixed sS16 typedef (now signed)
   - refactored SRAM malloc code to minim.h/minim.c
   - renamed dsp_fshm_*() to dsp_l1sram_*() (old calls are marked deprecated)
   - added dsp_physgpp_to_physdsp() API function (L3 to local interconnect addr. translation)
   - added C64_IOCTL_L2SRAM_ALLOC to c64_dev.h
   - added C64_IOCTL_L2SRAM_FREE to c64_dev.h
   - added C64_IOCTL_L2SRAM_DEBUG to c64_dev.h
   - added dsp_l2sram_alloc() API function
   - added dsp_l2sram_free() API function
   - added dsp_l2sram_debug() (private) API function
   - added new c64_tc testcase TC_L2SRAM_ALLOCATOR
   - added CMD_LINK1D to 'test_qdma' DSP component
   - added CMD_LINK2D to 'test_qdma' DSP component
   - added new c64_tc testcase TC_QDMA_LINK1D
   - added new c64_tc testcase TC_QDMA_LINK2D
   - added DSP_MAILBOX_RESET compile time option to dsp_config.h
   - added C64_TOOLS_ROOT env.var. export to dsp/setenv.sh (for DSP out of tree builds)
   - added setenv.sh (for GPP out of tree builds)
   - moved GPP graphics examples from tests/ to gfx_tests/
   - moved DSP graphics components from dsp/components/ to gfx_tests/dsp/components/
   - moved dsp_component_id_find_by_name() to dsp_priv.h
   - fixed multiprocess sync. in dsp_component_load()
   - added CORE_FC_CMD_COM_OVERLAY_FIND fastcall to core DSP component
   - added some cache writeback calls to fix sporadic DSP side component (un-)registration issue
   - added dsp_resume() API function
   - deprecated dsp_power_off() (dsp_priv.h) function (falls back to dsp_suspend())
   - moved dsp_resume() to public header file (dsp.h)
   - fixed DSP resume which was essentially broken due to some forgotten test/debug code
      (registered components were lost during dsp_resume())
   - increased RPC timeouts
   - added retry loop to dsp_rpc_send() to wait until DSP exits fastcall mode
   - increased accuracy of 'c64_nops' utility
      (measures DSP clock speed by executing/benchmarking a lot of NOP instructions)
   - improved DSP power management: applications can now suspend/resume the DSP
      (DSP is powered off when all client apps. have requested the suspend state, and
       resumes when first client app. calls dsp_resume())
   - added 'c64_pwrtest' testcase
   - added 'pwrlog' c64.ko kernel module option (default is 1 / true)
      (if enabled, trace DSP suspend/resume actions in klog)
   - updated 'go64.sh' to support pwrlog kmod option (C64_PWRLOG env. var. overrides def. pwrlog setting)

   full list of 'c64_tc' testcases:

                01: TC_TESTMSG
                02: TC_RPC_ADD
                03: TC_RPC_ADD_BENCHMARK
                04: TC_IOCTL_TEST
                05: TC_SHM_ACCESS
                06: TC_FASTCALL_RPC_ADD
                07: TC_L1SRAM_SPEED
                08: TC_PWR_OFF
                09: TC_RESUME
                10: TC_IDLE
                11: TC_SHM_CACHE_NONE
                12: TC_SHM_CACHE_RW
                13: TC_SHM_CACHE_R
                14: TC_SHM_CACHE_W
                15: TC_L1SRAM_ALLOCATOR
                16: TC_SHM_MSPACE
                17: TC_CTRLC
                18: TC_FASTCALL_CTRLC
                19: TC_LOGBUF
                20: TC_LOGBUF_MANY
                21: TC_LOGBUF_SPLITLINE
                22: TC_LOGBUF_NO_LF
                23: TC_QDMA_COPY1D
                24: TC_QDMA_COPY2D
                25: TC_SHM_CACHE_NONE_RAND_DSP
                26: TC_SHM_CACHE_RW_RAND_DSP
                27: TC_SHM_CACHE_R_RAND_DSP
                28: TC_SHM_CACHE_W_RAND_DSP
                29: TC_SHM_CACHE_NONE_RAND_GPP
                30: TC_SHM_CACHE_RW_RAND_GPP
                31: TC_SHM_CACHE_R_RAND_GPP
                32: TC_SHM_CACHE_W_RAND_GPP
                33: TC_SHM_CACHE_NONE_RAND_DSP_VIRT
                34: TC_SHM_CACHE_RW_RAND_DSP_VIRT
                35: TC_SHM_CACHE_R_RAND_DSP_VIRT
                36: TC_SHM_CACHE_W_RAND_DSP_VIRT
                37: TC_SHM_CACHE_NONE_RAND_GPP_VIRT
                38: TC_SHM_CACHE_RW_RAND_GPP_VIRT
                39: TC_SHM_CACHE_R_RAND_GPP_VIRT
                40: TC_SHM_CACHE_W_RAND_GPP_VIRT
                41: TC_SHM_CACHE_HUGETLB_V2P2V
                42: TC_SHM_CACHE_HUGETLB
                43: TC_L2SRAM_ALLOCATOR
                44: TC_QDMA_LINK1D
                45: TC_QDMA_LINK2


## 02-Nov-2013
   - added pnd_src/ folder
   - added -nops cmdline option to c64_dsprite
     (executes 800 million NOPs on DSP side, can be used to calibrate the DSP clock rate)
   - added DSP_CACHE_W (caches writes / uncached reads) cache type
   - added dsp_cache_inv_virt() API function
   - added dsp_cache_wb_virt() API function
   - added dsp_cache_wbinv_virt() API function
   - added USE_DSP_POWER_NOTIFY build option to kmod.h
      (if defined, call dsp_power_notify() before/after DSP is resumed/suspended)
   - added USE_FORCED_LOWPOWER_BYPASS build option to kmod.h
   - fixed: kmod/dev.c cache actions (inv/wb/wbinv) now use customized versions of
      v7_dma_map_area() instead of falling back to (the slow) cache_flush_all()
   - added support for huge pages (DSP_CACHE_HUGETLB cache type)
   - added (a lot of) new c64_tc testcases (run "./c64_tc" to see a list)


## 25-Oct-2013
   - fixed: DSP build dependencies
   - changed: DSP overlays can now be compiled w/o installing DSP/BIOS
   - changed: build versioned libc64.so.1
   - changed: renamed c64_msg_t to dsp_msg_t
   - changed: renamed dsp_shm_init()/exit() to dsp_shm_alloc()/free()
   - added: dsp_shm_alloc() memory attributes DSP_CACHE_NONE, DSP_CACHE_R (write-through), DSP_CACHE_RW
   - added: dsp_fshm_alloc() and free() (and associated ioctls). Used for L1DSRAM mem. allocation in userspace.
             The allocation granularity is 64 bytes (one cacheline).
   - added: malloc()/free() like memory allocator for e.g. shared memory. Uses Doug Lea's "dl_malloc".
             New API functions: dsp_mspace_create()/malloc()/memalign()/free()/destroy()
   - fixed: terminate fastcall-sequence in kmod/dev.c in case client forgot to
   - fixed: multiprocess overlay load race condition
   - added: dsp_virt_to_phys() and dsp_phys_to_virt() API calls
   - added: dsp_cache_wbinvall() API call
   - added: DSP ringbuffer for printf(). New DSP syscalls puts()/printf()/vprintf(). New API call dsp_logbuf_print()
   - added: new DSP syscalls qdma_init()/wait()/copy1d()/copy2d()
   - added: new DSP example components test_logbuf, test_qdma, dsprite
   - added: new GPP examples "c64_minimal" and "c64_dsprite"
   - added: new "tests/c64_tc.c" testcases:
                11: TC_SHM_CACHE_NONE
                12: TC_SHM_CACHE_RW
                13: TC_SHM_CACHE_R
                14: TC_FSHM_ALLOCATOR
                15: TC_SHM_MSPACE
                16: TC_CTRLC
                17: TC_FASTCALL_CTRLC
                18: TC_LOGBUF
                19: TC_LOGBUF_MANY
                20: TC_LOGBUF_SPLITLINE
                21: TC_LOGBUF_NO_LF
                22: TC_QDMA_COPY1D
                23: TC_QDMA_COPY2D


## 05-Oct-2013
   - added: DSP auto suspend / resume (in suspend, DSP consumes no power).
             The DSP is automatically suspended when the last application using it quits.
             Vice versa, it resumes execution / is restarted when the next app. connects.
   - added: support for dynamically loaded overlay images (4*256k + 1*1024k).
             Overlays are unregistered when dsp_close() is called and their refcount becomes 0.
             In case an application crashes, an "emergency unload" mechanism in the kernel module
              takes care of this to prevent DSP code areas from getting lost.
             The DSP build system automatically creates 4 .out file variants for the 256k areas
              (linked to different base addresses).
             Which of these 4 variants is loaded when dsp_component_load() is called depends on
              what area is available / not already in use by other applications.
             "demo_calc", "demo_checksum", "demo_calc_fastcall" are examples for "area2" (256k) overlays.
             "demo_fractal" is an example for an "area3" (1024k) overlay.
   - added: DSP-side global syscalls table. Currently includes some c64_tools utility functions,
             cache management utilities, and mem/string handling functions.
   - changed: DSP components no longer require DSP/BIOS (=> considerable decrease in code size)
               For DSP component development, the following (free as in beer) TI packages are required:
                $HOME/ti/c64xplus-iqmath_2_01_04_00
                $HOME/ti/fastRTS_c62xc64x_1_42
                $HOME/ti/TI_CGT_C6000_7.4.5
   - changed: cleaned up DSP build system and moved common .mk code into include files
   - changed: cleaned up "core" tconf and split it into multiple .tcis (see dsp/tci/ folder).
               "config.tci" is the central memory configuration file.
               It is included by the new gen_link_areas.tks script, which generates the DSP linker
                command files (.cmd) and the "overlay_sections.c" source, which is also included
                by the GPP-side build.
               Unless you want to change the memory layout, you do not need to worry about this, though.
   - fixed: added mtx_clients mutex to kernel module to fix some (potential) multiprocess concurrency issues
   - changed: partially rewrote fastcall protocol to use two separate ARM/C64+ cachelines.
               A more complex handshake protocol is used now but fortunately that
               did not have a negative effect on the performance
               

## 29-Sep-2013
   - changed: removed CCS projects and renamed "c64_ccs_projects/" to "dsp/".
   - added: makefile based DSP build system. This requires the following TI packages:
```
             $HOME/ti/bios_5_42_01_09
             $HOME/ti/c64xplus-iqmath_2_01_04_00
             $HOME/ti/fastRTS_c62xc64x_1_42
             $HOME/ti/TI_CGT_C6000_7.4.5
             $HOME/ti/xdctools_3_25_03_72
```
 
            In order to build the DSP libraries and .out image, change to the "dsp/" directory,
             run ". setenv.sh", then "m all" to start the build.
     
   - added "reg_read" and "reg_write" utilities (for dev. purposes only)
   - changed: when "c64_fractal" is compiled with no DSP support, it no longer requires "/dev/c64" to be present.
   - fixed: DSP powerdown sequence when c64.ko module is unloaded
   - added: DSP powersave mode. The system power consumption should now be not much
             different when the DSP is running (~0.35 W/h with the lid closed and WIFI off)

## 23-Sep-2013
   - added: the c64.ko kernel module now supports select() (=> OS friendly message reception)
   - added dsp_poll_enable() API function (increases message throughput by ~500% but is
      not OS friendly (high GPP usage). default=use select).
   - the dsp_cache_inv() and dsp_cache_wbinv() now fall back to flush_cache_all() since all
      other functions (see [kmod/dev.c](../master/kmod/dev.c)) cause system instabilities. disabling interrupts
      during cache invalidates did not change that.
   - added "tests/omapfb.c" utility code for framebuffer access on OMAP3 / Open Pandora.
   - added "c64_fractal" GPP example and "demo_fractal" component on DSP-side.
      (a single precision floating point heavy effect. uses TI's IQmath library on the DSP side.
       the source also contains several other alternative (slower) implementations.)


## 20-Sep-2013
   - changed: GPP applications do not require root privileges anymore (a bug in the last release)
   - GPP applications now have access to the second half of the L1DSRAM of the DSP (24 kbytes)
      The last 16 bytes of that area are used as IPC 'registers'. See [include/dsp_common.h](../master/include/dsp_common.h).
   - added dsp_fastcall_rpc() (can be called after the DSP has initiated a fastcall sequence)
   - added dsp_fastcall_end() (must be called to finish the fastcall sequence)
   - added dsp_rpc_send() and dsp_rpc_recv() (so that applications can do something else while
      the DSP is busy)
   - added new DSP "dsp_calc_fastcall" demo component that shows how to implement fastcall RPCs
   - added testcase #6 to "tests/c64_tc.c" (fastcall RPC example)
   - added testcase #7 to "tests/c64_tc.c" (SRAM access benchmark (=> ~3.89 million 32bit reads/writes per second)
   - the "c64.ko" kernel module now allocates the DSP image area (fixed address) during init.
      this means that it is no longer necessary to statically reserve memory for the DSP at boottime.
      (thanks to Notaz for providing a new Linux kernel and a small example module !)

      ** NOTE **  You need the updated kernel (see "pandora_sz1.55_kernel_update/uImage-3", copy it to "/boot" on the Pandora)
                   for the kernel module binary to work.

                  Please use the "scripts/cma_dsp_mem/autoboot.txt" boot script (works on all Pandora editions).

                  In case you do not want to update your kernel for some reason, you have to undefine
                   "USE_PLATFORM_DRIVER" in "kmod/kmod.c" and rebuild the module. You will also need
                   to use one of the "scripts/static_dsp_mem/*" boot scripts (depends on what edition you have).


## 18-Sep-2013
   - code cleanup
   - improved IPC/messaging speed by factor 1.7 (~23500 message roundtrips per second)
   - support for Linux CMA (contiguous memory allocator) (for dynamic GPP/DSP shared memory allocation)
   - GPP-side cache utility functions (writeback/invalidate/writeback+invalidate)
   - lots of new API functions (see [include/dsp.h](../master/include/dsp.h))
   - added Pandora "autoboot.txt" boot scripts to scripts/
      (use autoboot.txt__CLASSIC for Pandoras with 256 MBytes RAM, rename to autoboot.txt before copying
       to left SDCard)
   - now uses 4 MBytes DSP memory config (instead of 32 MBytes)
   - this "readme.txt"


## 15-Sep-2013
   - initial release. ~13000 message roundtrips per second (~10 times faster than DspBridge)



# Prerequisites

 - an OMAP3530 or DM3730 based board. I am working with an Open Pandora
    (you should get one -- it has a keyboard, a WVGA touchscreen, game controls,
      a quality audio output and an awesome battery life! and yes, lots of
      software!)

 - an ARM cross compiler (tested with CodeSourcery 2011.09).

 - the Linux kernel sources (if you want to rebuild the kernel module).

    The binary included with this release works with the Linux 3.2.45
    kernel shipped with the Open Pandora Super Zaxxon 1.55 firmware.

 - CodeComposerStudio v5.x (or better) for building DSP images

    (note: CCS is free (as in beer) for non-commercial/education use)


# Compiling

 (you can skip this part if you just want to try the precompiled binaries)

 First of all, set the following shell variables:

```
  $ export CROSS_COMPILE=arm-none-linux-gnueabi-
```

```
  $ export CROSS_ROOT=/bsp/pandora-dev/arm-2011.09
```

     (change this to point to the target filesystem crosscompile root, which
       must contain "usr/src/pandora-kernel")

     Alternatively you can set CROSS_KERNEL to point to the kernel source directory
      (overrides CROSS_ROOT when determining the source dir.)

 Then issue

```
  $ make -f makefile.linux all
```

 to rebuild the sources.



# Usage

 Copy the binary files (precompiled ones can be found in the "bin-18Sep2013/"
  directory) to the target device.

 If you have built the sources you can use the "scp" toplevel makefile
  target to copy the binaries to the device (edit the makefile and insert
  your user/hostname first).

 On the device, run "./go64.sh" to boot the DSP and load the kernel module:
```
   $ sudo su
   # export C64_DEBUG=<lvl>  ; (lvl=0..30. 0 if this line is omitted)
   # ./go64.sh
```

 The verbosity of the kernel module and libc64 can be controlled by setting
  the C64_DEBUG environment variable prior to running "go64".


 The "c64_tc" test application allows to run a couple of testcases:
```
   pandora:/home/bsp# ./c64_tc 
    Usage:
      c64_tc [testcase_nr]

        testcase_nr can be one of:
                     01: TC_TESTMSG
                     02: TC_RPC_ADD
                     03: TC_RPC_ADD_BENCHMARK
                     04: TC_IOCTL_TEST
                     05: TC_SHM_ACCESS
                     06: TC_FASTCALL_RPC_ADD
                     07: TC_SRAM_SPEED
```


 Start with running the IPC (interprocessor communication) benchmark/stresstest:

```
   $ ./c64_tc 3
```

 The stresstest can be started multiple times, in order to test the multi-client
  feature (add an '&' to the cmdline above, then issue the command multiple times).


# Design considerations

 The remote procedure call interface used by "c64_tools" is very simple:

   - The DSP side registers a number of "components" (with ids 1..15).

   - Each component has an optional init() function, a required
      exec() function, and another optional exit() function which is
      never called at the moment.

   - Each component can execute up to 65535 "commands".

   - A component command takes up to two 32bit input arguments and can
      return up to two 32bit output values.

     It can also set an error number (return value of the exec()
      function, 0=success)

   - The components used by a specific DSP image are enumerated in
      the file "components.h", located in the DSP project directory.

   - The GPP-side includes "components.h" so it can use the component
      IDs in remote procedure calls.


 Please take a look at the example DSP project ("c64p_simple_dm3730"),
  the code should be easy to understand.


 Something to consider:

  - please do not use multithreading on the DSP side. I.e. only one
     component exec() function can be executed at a time.

  - remember to invalidate and writeback memory regions before reading
     them on processor A after processor B has modified the memory
     (A/B=GPP or DSP)

     On the ARM-side, use the following libc64 functions:

```
       dsp_cache_wb()    -- write back data caches for the given range
       dsp_cache_inv()   -- invalidate data caches for the given range
       dsp_cache_wbinv() -- write back and invalidate data caches for the given range
```

     On the DSP-side, use the following DSPBIOS functions:

```
       BCACHE_wb()    -- write back data caches for the given range
       BCACHE_inv()   -- invalidate data caches for the given range
       BCACHE_wbInv() -- write back and invalidate data caches for the given range
```

  - the DSP side can set up to 4 debug values via "mlb_debug_usr()".

  - the GPP side can read those debug values via "dsp_debug_usr_get()".


 When developing DSP code, you can use the C64+ simulator included with TI's
  CodeComposerStudio.

 If your DSP code is plain "C", you may want to simulate the DSP environment
  by using multi-threading. Maybe I will include an emulation-version of
  libc64 in future releases (but that's much easier to do on a higher application level).


 About the fastcall RPCs:

  This feature does not use "slow" interrupts to talk to the DSP but rather uses the L1DSRAM.

  The general idea is:

    1) GPP sends regular RPC to DSP (via dsp_rpc_send())

    2) DSP component initiates fastcall sequence using mlb_fastcall_initiate()

    3) GPP calls dsp_fastcall_rpc() to send requests to the DSP

    4) GPP calls dsp_fastcall_end() when all requests have been sent
    
     

# Status

 Right now this software is, although quite new, stable, as tested so far.



# Known issues

 - the DSP MMU is currently disabled before booting the DSP.
    This will may be changed in future releases, so that the GPP OS is protected
    from misbehaving DSP software.

 - the kernel module should support message queues, so that when a client
    application calls write(), the call returns successfully after queuing
    the message. not a terribly important feature, though.

 - The maximum DSP image size is currently limited to 4MBytes (not rly an issue, probably)



# Authors

 This software was written by Bastian Spiegel. Feel free to contact me
  at "bs AT tkscript DOT de", if you have any questions or want to contribute.

 Thanks to Notaz for the platform_driver reference code (see [kmod/kmod.c](../master/kmod/kmod.c)).


# Acknowledgements

 Some small portions (a few lines of code) are loosely based on Texas Instruments'
  "CMEM" kernel module, which is distributed under the terms of the GPL.

  In particular: The GPP-side cache wb/inv/wbinv function calls. While these
  are just standard ARM Linux function calls, it was quite helpful to read the
  CMEM source instead of reading documentation finding out about these calls.
  So, thank you guys'n'gals !

 The source code that generates the C64+ branch instruction to jump to the
  actual DSP image entry point is also loosely based on code found in DspLink.

  It's not a verbatim copy but looking at the DspLink source saved me some time
   I otherwise would have needed for finding out the hex-values for the
   branch instruction.
