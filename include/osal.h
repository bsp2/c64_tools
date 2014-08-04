/* ----
 * ---- file   : osal.h
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the LESSER GNU GENERAL PUBLIC LICENSE (LGPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING_LGPL for further information.
 * ----
 * ---- info   : OS abstraction layer. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 06Sep2013, 08Sep2013, 13Dec2013
 * ----
 * ----
 */

#ifndef __C64_TOOLS_OSAL_H__
#define __C64_TOOLS_OSAL_H__

#include "cplusplus_begin.h"


/*
 * (todo) document me
 *
 */
S_EXTERN const char * S_CALL osal_strerror(osal_error_t _err);


/*
 * (todo) document me
 *
 */
S_EXTERN osal_error_t S_CALL osal_init(sBool _bRootRequired);


/*
 * (todo) document me
 *
 */
S_EXTERN void S_CALL osal_exit(void);


/*
 * (todo) document me
 *
 */
S_EXTERN void * S_CALL osal_alloc(sSize _numBytes);


/*
 * (todo) document me
 *
 */
S_EXTERN void S_CALL osal_free(void *_ptr);


/*
 * (todo) document me
 *
 */
S_EXTERN void * S_CALL osal_mmap(void *_physAddr, sSize _numBytes);


/*
 * (todo) document me
 *
 */
S_EXTERN sBool S_CALL osal_munmap(void *_virtAddr, sSize _numBytes);


/*
 * Sleep for the given number of microseconds.
 *
 */
S_EXTERN void osal_usleep(sUI _numMicrosecs);


/*
 * Sleep for the given number of milliseconds.
 *
 */
S_EXTERN void osal_msleep(sUI _numMillisecs);


/*
 * Interrupt thread execution and let OS schedule next thread
 *
 */
S_EXTERN void osal_yield(void);


/*
 *
 * Query milliseconds since osal_init()
 *
 */
S_EXTERN sUI osal_milliseconds_get (void);


#include "cplusplus_end.h"

#endif /* __C64_TOOLS_LOG_H__ */
