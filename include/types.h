/* ----
 * ---- file   : types.h
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the LESSER GNU GENERAL PUBLIC LICENSE (LGPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING_LGPL for further information.
 * ----
 * ---- info   : Common type definitions and constants. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 06Sep2013, 08Sep2013, 21Sep2013, 05Dec2013
 * ----
 * ----
 */

#ifndef __C64_TOOLS_TYPES_H__
#define __C64_TOOLS_TYPES_H__

#include "cplusplus_begin.h"


/* Determine C compiler */
#if defined(__GNUC__)
#undef HAVE_CC_GCC
#define HAVE_CC_GCC defined

#elif defined(_MSC_VER)
#undef HAVE_CC_MSVC
#define HAVE_CC_MSVC defined

#else
#undef HAVE_CC_UNKNOWN
#define HAVE_CC_UNKNOWN defined
#endif


/* Determine OS */
#if defined(__linux__)
#undef HAVE_OS_LINUX
#define HAVE_OS_LINUX defined

#elif defined(__CYGWIN32__)
#undef HAVE_OS_CYGWIN
#define HAVE_OS_CYGWIN defined

#elif defined(_WIN32) || defined(WIN32)
#undef HAVE_OS_WIN32
#define HAVE_OS_WIN32 defined

#else
#undef HAVE_OS_UNKNOWN
#define HAVE_OS_UNKNOWN defined
#endif


typedef char                sChar; /* (note) system dependent: either signed or unsigned */
typedef unsigned char       sU8;
typedef signed char         sS8;
typedef unsigned short      sU16;
typedef signed short        sS16;
typedef unsigned int        sU32;
typedef unsigned int        sUI;
typedef signed int          sS32;
typedef signed int          sSI;
typedef unsigned long long  sU64;
typedef signed long long    sS64;
typedef sSI                 sBool;

typedef sU32                sFix; /* 1:15:16 fixed point */
typedef sUI                 sSize;
typedef sSI                 s_error_t;

typedef float               sF32;
typedef double              sF64;


#define S_TRUE   (1)
#define S_FALSE  (0)

#define S_ERR_OK (0)

#ifndef NULL
#define NULL  ((void*)0)
#endif

#define S_PI     (3.1415926f)
#define S_2PI    (6.2831853f)
#define S_PI2    (1.5707963f)
#define S_E      (2.7182818f)
#define S_SQRT2  (1.4142135f)


#ifndef CCS
/* (note) the CodeSourcery GCC uses a default packing of 4 bytes, in order to correctly
 *         access members of structures that contain shorts, we need to use a tighter packing
 *         of 2 bytes
 */
#pragma pack(2)
#endif /* CCS */


/* How to declare global functions and variables */
#define S_EXTERN  extern

/* Calling convention (empty = use compiler default) */
#define S_CALL
//#define S_CALL __cdecl

/* How to declare inline functions */
#define S_INLINE static inline


#include "cplusplus_end.h"

#endif // __C64_TOOLS_TYPES_H__
