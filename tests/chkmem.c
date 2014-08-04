
/* dump IVA2 sequencer instruction memory */

#include "../include/types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>


int main(int argc, char**argv) {
   void *virt;

   int fdMem = open("/dev/mem", O_RDWR | O_SYNC);

   if(-1 != fdMem)
   {
      virt = mmap(NULL /*addr*/,
                  8192,
                  PROT_READ | PROT_WRITE,
                  MAP_SHARED,
                  fdMem,
                  (off_t) 0x5e098000u // seq IMEM
                  );
      
      if(MAP_FAILED != virt)
      {
         sU32 *s = virt;
         sUI i;
         
         for(i=0; i< 8192/sizeof(sU32); i++)
         {
            if(0 != s[i])
            {
               printf("s[%08x] = 0x%08x\n", (i << 2), s[i]);
            }
         }

         /* Dump sequencer instruction memory */
         {
            FILE *fh = fopen("seq.imem", "wb");

            fwrite(virt, 8192, 1, fh);

            fclose(fh);
         }
         
         munmap(virt, 8192);
      }
      else
      {
         printf("mmap() failed (errno=%d(\%s\"))\n", errno, strerror(errno));
      }

      close(fdMem);
   }
   else
   {
      printf("failed to open /dev/mem. errno=%d(\"%s\")\n", errno, strerror(errno));
   }
   
   return 0;
}
