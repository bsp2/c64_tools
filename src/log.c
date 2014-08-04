/* ----
 * ---- file   : log.h
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the LESSER GNU GENERAL PUBLIC LICENSE (LGPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING_LGPL for further information.
 * ----
 * ---- info   : This is part of the "c64_tools" package.
 * ----
 * ---- changed: 06Sep2013, 08Sep2013, 13Sep2013, 22Sep2013
 * ----
 * ----
 */

#include <stdio.h>
#include <stdarg.h>

#include "../include/types.h"
#include "../include/log.h"


#define USE_FFLUSH defined


/*--------------------------------------------------------------------------- vars */
sUI log_debug_level;


#ifndef LOG_OUTPUT


/*--------------------------------------------------------------------------- log_putc() / dummy */
void S_CALL log_putc(sChar _c) {
   (void)_c;
}


/*--------------------------------------------------------------------------- log_puts() / dummy */
void S_CALL log_puts(const sChar *_msg) {
   (void)_msg;
}


/*--------------------------------------------------------------------------- log_printf() / dummy */
void S_CALL log_printf(const sChar *_fmt, ...) {
   (void)_fmt;
}


#else


/*--------------------------------------------------------------------------- log_putc() */
void S_CALL log_putc(sChar _c) {
#if defined LOG_OUTPUT_STDOUT
   putc(_c, stdout);
#ifdef USE_FFLUSH
   fflush(stdout);
#endif /* USE_FFLUSH */
#elif defined(LOG_OUTPUT_STDERR)
   putc(_c, stderr);
#endif
}


/*--------------------------------------------------------------------------- log_puts() */
void S_CALL log_puts(const sChar *_msg) {
#if defined LOG_OUTPUT_STDOUT
   puts(_msg);
#ifdef USE_FFLUSH
   fflush(stdout);
#endif /* USE_FFLUSH */
#elif defined(LOG_OUTPUT_STDERR)
   puts(_msg, stderr);
#endif
}


/*--------------------------------------------------------------------------- log_printf() */
void S_CALL log_printf(const sChar *_fmt, ...) {
   va_list l;

   va_start(l, _fmt);

#if defined(LOG_OUTPUT_STDOUT)
   vprintf(_fmt, l);
#ifdef USE_FFLUSH
   fflush(stdout);
#endif /* USE_FFLUSH */
#elif defined(LOG_OUTPUT_STDERR)
   vfprintf(stderr, _fmt, l);
#endif

   va_end(l);
}


#endif /* LOG_OUTPUT */
