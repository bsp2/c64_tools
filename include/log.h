/* ----
 * ---- file   : log.h
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the LESSER GNU GENERAL PUBLIC LICENSE (LGPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING_LGPL for further information.
 * ----
 * ---- info   : This is part of the "c64_tools" package.
 * ----
 * ---- changed: 06Sep2013, 08Sep2013, 18Sep2013
 * ----
 * ----
 */

#ifndef __C64_TOOLS_LOG_H__
#define __C64_TOOLS_LOG_H__

#include "cplusplus_begin.h"


// If defined, print log messages
#define LOG_OUTPUT defined

// If defined, print log messages to stdout
#define LOG_OUTPUT_STDOUT defined


#define LOG_DEBUG  "[dbg] "
#define LOG_TRACE  "[trc] "
#define LOG_INFO   "[...] "
#define LOG_WARN   "[~~~] "
#define LOG_ERROR  "[---] "
#define LOG_FATAL  "[!!!] "


S_EXTERN sUI log_debug_level;

#define LOG_DEBUG_LEVEL(a)  (log_debug_level >= (a))

//#define LOG_DEBUG_LEVEL(a)  (0)



S_EXTERN void S_CALL log_putc (sChar _c);

S_EXTERN void S_CALL log_puts (const sChar *_msg);

S_EXTERN void S_CALL log_printf (const sChar *_fmt, ...);


#include "cplusplus_end.h"

#endif /* __C64_TOOLS_LOG_H__ */
