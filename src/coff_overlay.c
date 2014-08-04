/* ----
 * ---- file   : coff_overlay.c
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : COFF2 loader. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 04Oct2013, 22Oct2013
 * ----
 * ----
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>
#include <time.h>

#include "../include/types.h"
#include "../include/log.h"
#include "../include/osal_err.h"
#include "../include/osal.h"

#include "../include/coff_err.h"
#include "../include/coff.h"


typedef union {
   s_error_t    any;
   osal_error_t osal;
   coff_error_t coff;
} err_t;


#define P2V(a) ( ((a) - _physAddr) + (sU32)_virtAddr )


/*--------------------------------------------------------------------------- global vars */


/*--------------------------------------------------------------------------- module vars */


/*--------------------------------------------------------------------------- loc_open_and_read_basic_headers() */
static coff_error_t loc_open_and_read_basic_headers(void) {
   coff_error_t ret;

   /* Parse file header and allocate section headers */
   ret = coff_file_header_parse();
      
   if(COFF_ERR_OK == ret)
   {
      ret = coff_opt_header_parse();
         
      if(COFF_ERR_OK == ret)
      {
         ret = coff_string_table_read();
            
         /* Succeeded (possibly) */
      }
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_read_section_headers */
static coff_error_t loc_read_section_headers(void) {
   coff_error_t       ret;
   sUI                secIdx;
   coff_sec_header_t *secHdr = coff_sec_headers;

   ret = 0;

   if(LOG_DEBUG_LEVEL(10))
   {
      log_printf(LOG_INFO "coff_load_overlay: reading %u section headers", coff_file_header.num_section_headers);
   }
   
   secIdx = 0;

   while
      (
         (secIdx < coff_file_header.num_section_headers) &&
         (COFF_ERR_OK == ret)
       )
   {
      if(log_debug_level > 0)
      {
         if(log_debug_level >= 10)
         {
            log_putc('\n');
         }
         
/*          log_printf(LOG_DEBUG "coff_load_overlay: reading section header %u/%u\n", */
/*                     (secIdx + 1), */
/*                     coff_file_header.num_section_headers */
/*                     ); */
      }
      else
      {
         if(LOG_DEBUG_LEVEL(1))
         {
            putc('.', stdout);
         }
      }

      ret = coff_sec_header_parse(secHdr);

      if(COFF_ERR_OK == ret)
      {
         /* Next section header */
         secIdx++;
         secHdr++;
      }
      else
      {
         /* Failed to parse section header */
         log_printf(LOG_ERROR "coff_load_overlay: failed to parse section header %u/%u\n",
                (secIdx + 1),
                coff_file_header.num_section_headers
                );
         break;
      }

   } /* iterate section headers */

   if(LOG_DEBUG_LEVEL(10))
   {
      log_putc('\n');
   }

   if(LOG_DEBUG_LEVEL(10))
   {
      log_printf(LOG_DEBUG "coff_load_overlay: coff_max_raw_data_size=0x%08x (%u)\n",
                 coff_max_raw_data_size,
                 coff_max_raw_data_size
                 );
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_read_sections */
static s_error_t loc_read_sections(void *_virtAddr, 
                                   sU32 _physAddr, 
                                   sU32 _size
                                   ) {
   err_t              ret;
   sUI                secIdx = 0;
   coff_sec_header_t *secHdr = coff_sec_headers;
   sUI                totalTextSize = 0;
   sUI                totalDataSize = 0;
   sUI                totalBSSSize  = 0;

   /* Initialize DSP memory and copy sections */

   if(LOG_DEBUG_LEVEL(20))
   {
      log_printf(LOG_DEBUG "loc_read_sections: virtAddr=0x%08x physAddr=0x%08x size=0x%08x\n",
                 (sU32)_virtAddr,
                 _physAddr,
                 _size
                 );
   }

   ret.any = 0;

   if(coff_max_raw_data_size > 0)
   {
      // (todo) load directly into DSP RAM */
      sU8 *rawBuf = osal_alloc(coff_max_raw_data_size);

      if(NULL != rawBuf)
      {
         if(LOG_DEBUG_LEVEL(10))
         {
            log_puts(LOG_INFO "coff_load_overlay: initializing DSP memory and copying sections..");
         }

         while
            (
               (secIdx < coff_file_header.num_section_headers) &&
               (S_ERR_OK == ret.any)
             )
         {
            if(0 /*NULL*/ != secHdr->phys_addr)
            {
               if(0 != (secHdr->flags & COFF_SEC_HEADER_FLAG_BSS))
               {
                  if(secHdr->size > 0)
                  {
                     if(LOG_DEBUG_LEVEL(10))
                     {
                        log_printf(LOG_DEBUG "coff_load_overlay: clearing BSS section @0x%08x, sz=0x%08x (%u)..",
                                   secHdr->phys_addr,
                                   secHdr->size,
                                   secHdr->size
                                   );
                     }
                     
                     /* BSS, just clear the mem region */
                     if((secHdr->phys_addr >= _physAddr) && /* restrict access to overlay area */
                        (secHdr->phys_addr <  (_physAddr + _size))
                        )
                     {
                        memset((void*)P2V(secHdr->phys_addr), 0, secHdr->size);
                        
                        if(LOG_DEBUG_LEVEL(10))
                        {
                           log_puts("..OK!");
                        }
                     }
                     else
                     {
                        if(LOG_DEBUG_LEVEL(10))
                        {
                           log_puts("..skipped!");
                        }
                     }
                     
                     totalBSSSize += secHdr->size;
                     
                  }
               } /* if BSS */
               else if(0 != (secHdr->flags & (COFF_SEC_HEADER_FLAG_TEXT | COFF_SEC_HEADER_FLAG_DATA)))
               {
                  if(secHdr->raw_data_off > 0)
                  {
                     /* Load raw section data */
                     ret.coff = coff_seek_abs(secHdr->raw_data_off);
                     
                     if(COFF_ERR_OK == ret.coff)
                     {
                        ret.coff = coff_read(rawBuf, secHdr->size);
                        
                        if(COFF_ERR_OK == ret.coff)
                        {
                           if(LOG_DEBUG_LEVEL(10))
                           {
                              log_printf(LOG_DEBUG "coff_load_overlay: copying TEXT/DATA section @0x%08x, sz=0x%08x (%u)..",
                                         secHdr->phys_addr,
                                         secHdr->size,
                                         secHdr->size
                                         );
                           }

                           if((secHdr->phys_addr >= _physAddr) && /* restrict access to overlay area */
                              (secHdr->phys_addr <  (_physAddr + _size))
                              )
                           {
                              /* Code or data, copy to DSP memory */
                              memcpy((void*)P2V(secHdr->phys_addr), rawBuf, secHdr->size);
                           
                              if(LOG_DEBUG_LEVEL(10))
                              {
                                 log_puts("..OK!");
                              }
                           }
                           else
                           {
                              if(LOG_DEBUG_LEVEL(10))
                              {
                                 log_puts("..skipped!");
                              }
                           }
                           
                           if(0 != (secHdr->flags & COFF_SEC_HEADER_FLAG_TEXT))
                           {
                              totalTextSize += secHdr->size;
                           }
                           else
                           {
                              totalDataSize += secHdr->size;
                           }
                        }
                        else
                        {
                           log_printf(LOG_ERROR "coff_load_overlay: failed to read section #%d/%d raw data\n",
                                      (secIdx + 1),
                                      coff_file_header.num_section_headers
                                      );
                        }
                     }
                     else
                     {
                        log_printf(LOG_ERROR "coff_load_overlay: failed to seek to section #%d/%d raw data\n",
                                   (secIdx + 1),
                                   coff_file_header.num_section_headers
                                   );
                     }
                  }
                  else
                  {
                     log_printf(LOG_WARN "coff_load_overlay: section #%d/%d is flagged as TEXT or DATA but raw_data_off is 0. skipping..\n",
                                (secIdx + 1),
                                coff_file_header.num_section_headers
                                );
                  }
               } /* else if text/data */
            }
            
            /* Next section */
            secIdx++;
            secHdr++;
            
         } /* iterate sections */
         
         log_printf(LOG_INFO "coff_load_overlay: text=0x%08x  data=0x%08x  bss=0x%08x\n",
                    totalTextSize,
                    totalDataSize,
                    totalBSSSize
                    );

         /* static var initialization (_c_int00 is NOT run for overlay images!) */
         if(0 != coff_cinit_addr)
         {
            union {
               const sU32 *u32;
               const sU8  *u8;
            } cinit;

            cinit.u32 = (const sU32*) P2V(coff_cinit_addr);

            sUI off = 0;

            while(off < coff_cinit_size)
            {
               sUI numBytes = cinit.u32[off >> 2];
               sUI dstPhys  = cinit.u32[(off + 4) >> 2];
               sU8 *dst     = (sU8*) (P2V(dstPhys));
               off += 8;

               if(LOG_DEBUG_LEVEL(20))
               {
                  log_printf(LOG_TRACE "coff_load_overlay: cinit %u bytes @0x%08x\n",
                             numBytes,
                             dstPhys
                             );
               }
               
               while(numBytes-- > 0)
               {
                  if(LOG_DEBUG_LEVEL(25))
                  {
                     log_printf(LOG_TRACE "coff_load_overlay: cinit[%08u] byte=%u (0x%02x) dst=0x%08x\n",
                                off,
                                cinit.u8[off],
                                cinit.u8[off],
                                dstPhys
                                );

                     dstPhys++;
                  }

                  *dst++ = cinit.u8[off++];
               }
                                      
               off = ( (off + 7u) >> 3 ) << 3;
            }

         }
         else
         {
            log_printf(LOG_WARN "coff_load_overlay: missing \'.cinit\' section!\n");
         }

         osal_free(rawBuf);

         /* Succeeded */

      } /* if allocate rawBuf */
      else
      {
         /* Failed: could not allocate raw code/data buffer */
         log_printf(LOG_ERROR "coff_load_overlay: failed to allocate raw code/data buffer (%u bytes)\n",
                    coff_max_raw_data_size
                    );

         ret.coff = COFF_ERR_GEN_RAWBUFALLOC;
      }

   } /* if max raw size > 0 */
   else
   {
      /* Failed: COFF file contains no code or data */
      log_printf(LOG_ERROR "coff_load_overlay: max_raw_data_size == 0 !! (no code/data in DSPImage ?!!\n");

      ret.coff = COFF_ERR_GEN_NOCODEORDATA;
   }

   return ret.any;
}


/*--------------------------------------------------------------------------- coff_load_overlay() */
int S_CALL coff_load_overlay(const char *_fname, 
                             void *_virtAddr, 
                             sU32 _physAddr, 
                             sU32 _size
                             ) {
   err_t err;

   err.coff = coff_init();

   if(COFF_ERR_OK == err.coff)
   {
      /* Open DSP image file */
      if(LOG_DEBUG_LEVEL(5))
      {
         log_printf(LOG_INFO "coff_load_overlay: loading DSP image from \"%s\"\n", _fname);
      }

      err.coff = coff_open(_fname);
         
      if(COFF_ERR_OK == err.coff)
      {
         /* Load file and opt headers and read string table */
         err.coff = loc_open_and_read_basic_headers();

         if(COFF_ERR_OK == err.coff)
         {
            err.coff = loc_read_section_headers();

            if(COFF_ERR_OK == err.coff)
            {
               /* Load sections and copy them to DSP memory */
               err.coff = loc_read_sections(_virtAddr, _physAddr, _size);
               
               if(COFF_ERR_OK == err.coff)
               {
                  if(LOG_DEBUG_LEVEL(5))
                  {
                     log_puts(LOG_INFO "coff_load_overlay: DSP image loaded successfully.");
                  }

                  /* Succeeded */

               } /* if loc_read_sections() */
               else
               {
                  log_puts(LOG_ERROR "coff_load_overlay: failed to read DSP image sections.");
               }
               
            } // if loc_read_section_headers()

         } // if loc_open_and_read_basic_headers()

         /* Close DSPImage file handle */
         coff_close();

      } // if coff_open()

      /* Free section headers / teardown */
      coff_exit();

   } // if coff_init()

   /* Final error check */
   if(S_ERR_OK == err.any)
   {
      /* Succeeded */
   }
   else
   {
      const char *msg;

      msg = coff_strerror(err.coff);
      
      if(NULL == msg)
      {
         msg = osal_strerror(err.osal);
      }

      log_printf(LOG_ERROR "coff_load_overlay: failed with error code 0x%08x (\"%s\")\n",
                 (sUI)err.any,
                 msg
                 );
   }

   return err.any;
}
