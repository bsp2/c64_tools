
/* Include helper for overlay components */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "../../../include/types.h"

#include "../../../include/arch/dm3730/dm3730.h"      /* needed for L1DSRAM addr. pls ignore the rest of it. */
#include "../../../include/dsp_common.h"  /* for scratchbuffer addr/size */

#include "com.h"
#include "syscalls.h"

#ifdef SYSCALLS_C
#include "syscalls.c"
#endif /* SYSCALLS_C */
