/* ----
 * ---- file   : reg_read.c
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : This is part of the "c64_tools" package.
 * ----
 * ---- changed: 24Sep2013, 20Oct2013, 27Oct2013, 04Nov2013
 * ----
 * ----
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../include/inc_libc64.h"
#include "../include/dsp_priv.h"


/*--------------------------------------------------------------------------- usage() */
static void usage(void) {
   log_printf("Usage: reg_read <regaddr>\n");
}


/*--------------------------------------------------------------------------- loc_str_bin_32() */
static const char *loc_str_bin_32(sU32 _val) {
   static char buf[] = "1010 1010 1010 1010 1010 1010 1010 1010";
   sU32 mask = (1u << 31);
   char *d = buf;
   sUI i;

   for(i=0; i<32; i++)
   {
      if((i > 0) && (0 == (i & 3u)))
      {
         d++;
      }

      if(_val & mask)
      {
         *d++ = '1';
      }
      else
      {
         *d++ = '0';
      }

      mask >>= 1u;
   }

   return buf;
   
}


/*--------------------------------------------------------------------------- main() */
int main(int argc, char**argv) {
   int ret;

   if(2 != argc)
   {
      usage();

      ret = 10;
   }
   else
   {
      dsp_set_enable_skip_dsp_open(S_TRUE);

      if(0 == dsp_init_root())
      {
         sU32 addr;
         sU32 val;

         sscanf(argv[1], "%08x", &addr);

         if(0 == dsp_reg_read_32(addr, &val))
         {
            log_printf(" read reg[0x%08x] = 0x%08x (%s) (\'%c\' \'%c\' \'%c\' \'%c\')\n",
                       addr,
                       val,
                       loc_str_bin_32(val),
                       (val >> 24) & 255u,
                       (val >> 16) & 255u,
                       (val >>  8) & 255u,
                       (val      ) & 255u
                       );
         }

         dsp_exit_root();
      }

      ret = 0;
   }

   return ret;
}
