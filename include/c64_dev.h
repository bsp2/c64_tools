/* ----
 * ---- file   : c64_dev.h
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING_LGPL for further information.
 * ----
 * ---- info   : Shared header file for c64 kernel module / userspace apps. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 16Sep2013, 18Sep2013, 02Oct2013, 03Oct2013, 05Oct2013, 15Oct2013, 18Oct2013
 * ----          22Oct2013, 02Nov2013, 08Dec2013
 * ----
 * ----
 */

#ifndef __C64_TOOLS_C64_DEV_H__
#define __C64_TOOLS_C64_DEV_H__


#define C64_DEV_MAJOR  (239u)
#define C64_DEV_MINOR  ( 64u)
#define C64_DEV_NAME   "c64"


// for C64_IOCTL_SHM_ALLOC:
typedef struct {
   dsp_mem_region_t mem;  /* caller must set 'size' field, kmod will return virt_addr/phys_addr on success */
   dsp_cache_t      type; /* 0=uncached, 1=writeback (cached), 2=write-thru. see dsp_common.h for enum. */
} c64_dev_ioctl_shm_alloc_t;


/* for C64_IOCTL_CACHE and C64_IOCTL_CACHE_VIRT: */
#define C64_CACHE_AC_WB        (1u)  /* writeback */
#define C64_CACHE_AC_INV       (2u)  /* invalidate */
#define C64_CACHE_AC_WBINV     (3u)  /* writeback and invalidate */
#define C64_CACHE_AC_WBINVALL  (4u)  /* writeback and invalidate all (flush_cache_all()) */

typedef struct {
   sU32 phys_addr;
   sU32 size;
   sU32 action;  /* one of C64_CACHE_AC_xxx */
} c64_dev_ioctl_cache_t;

typedef struct {
   sU32 virt_addr;
   sU32 size;
   sU32 action;  /* one of C64_CACHE_AC_xxx */
} c64_dev_ioctl_cache_virt_t;


/* for C64_IOCTL_DEBUG_USR: */
typedef union {
   sU32 idx;
   sU32 val;
} c64_dev_ioctl_debug_usr_t;


/* for C64_IOCTL_OVERLAY_ALLOC: */
#define C64_OVERLAY_AREA2  (2u)  /* 4 256KB areas */
#define C64_OVERLAY_AREA3  (3u)  /* 1 1MB area */

typedef union {
   sUI              overlay_area;

   struct {
      sUI              section_idx;
      dsp_mem_region_t mem;
   } ret;

} c64_dev_ioctl_overlay_alloc_t;


/* for : */




/* C64 IOCTL enumeration
 *
 *  (note) on a side note, why do the IOCTL numbers have to be unique system-wide?
 *          via udev we have dynamic device creation, there's dynamic device numbers, then this ? /rant
 */
#define C64_IOCTL_BASE C64_DEV_MAJOR

#define C64_IOCTL_SHM_ALLOC      _IOWR(C64_IOCTL_BASE,  0, c64_dev_ioctl_shm_alloc_t *)
#define C64_IOCTL_SHM_FREE       _IOR (C64_IOCTL_BASE,  1, dsp_cache_t)
#define C64_IOCTL_TEST           _IO  (C64_IOCTL_BASE,  2)
#define C64_IOCTL_CACHE          _IOR (C64_IOCTL_BASE,  3, c64_dev_ioctl_cache_t *)
#define C64_IOCTL_DEBUG_USR      _IOWR(C64_IOCTL_BASE,  4, c64_dev_ioctl_debug_usr_t *)
#define C64_IOCTL_PWR_ON_RST     _IO  (C64_IOCTL_BASE,  5)
#define C64_IOCTL_START          _IOR (C64_IOCTL_BASE,  6, sU32)
#define C64_IOCTL_PWR_OFF        _IO  (C64_IOCTL_BASE,  7)
#define C64_IOCTL_RESUME         _IO  (C64_IOCTL_BASE,  8)
#define C64_IOCTL_OVERLAY_ALLOC  _IOWR(C64_IOCTL_BASE,  9, c64_dev_ioctl_overlay_alloc_t *)
#define C64_IOCTL_OVERLAY_FREE   _IOWR(C64_IOCTL_BASE, 10, sU32)
#define C64_IOCTL_OVERLAY_REF    _IOR (C64_IOCTL_BASE, 11, sU32)
#define C64_IOCTL_L1SRAM_ALLOC   _IOWR(C64_IOCTL_BASE, 12, dsp_mem_region_t *)
#define C64_IOCTL_L1SRAM_FREE    _IOR (C64_IOCTL_BASE, 13, dsp_mem_region_t *)
#define C64_IOCTL_L1SRAM_DEBUG   _IOR (C64_IOCTL_BASE, 14, sUI)
#define C64_IOCTL_CACHE_VIRT     _IOR (C64_IOCTL_BASE, 15, c64_dev_ioctl_cache_virt_t *)
#define C64_IOCTL_HUGETLB_V2P    _IOWR(C64_IOCTL_BASE, 16, sU32 *)
#define C64_IOCTL_L2SRAM_ALLOC   _IOWR(C64_IOCTL_BASE, 17, dsp_mem_region_t *)
#define C64_IOCTL_L2SRAM_FREE    _IOR (C64_IOCTL_BASE, 18, dsp_mem_region_t *)
#define C64_IOCTL_L2SRAM_DEBUG   _IOR (C64_IOCTL_BASE, 19, sUI)


#endif /* __C64_TOOLS_C64_DEV_H__ */
