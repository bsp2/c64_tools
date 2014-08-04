/* ----
 * ---- file   : main.c
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : This is part of the "c64_load" utility.
 * ----
 * ---- changed: 06Sep2013, 07Sep2013, 08Sep2013, 13Sep2013, 18Sep2013, 02Oct2013, 03Oct2013
 * ----          04Oct2013
 * ----
 * ----
 */

//#define TESTCODE defined


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(TESTCODE) || defined(HAVE_OS_LINUX)
#include <unistd.h>
#endif /* TESTCODE */

#include "../include/types.h"
#include "../include/log.h"
#include "../include/osal_err.h"
#include "../include/osal.h"
#include "../include/dsp_err.h"
#include "../include/dsp_common.h"
#include "../include/dsp_config.h"
#include "../dsp/libc64_dsp/include/com.h"
#include "../include/msg.h"
#include "../include/dsp.h"
#include "../include/dsp_priv.h"
#include "../include/coff_err.h"
#include "../include/coff.h"



/*--------------------------------------------------------------------------- err_t */
typedef union {
   s_error_t    any;
   osal_error_t osal;
   coff_error_t coff;
   dsp_error_t  dsp;
} err_t;


/*--------------------------------------------------------------------------- module vars */
static const char *dsp_image_pathname;


/*--------------------------------------------------------------------------- usage() */
static void usage(void) {
   log_puts("c64_load -- a TMS320C6000 DSP image loader. (c) 2013 Bastian Spiegel <bs@tkscript.de>\n");
   log_puts("  Usage:");
   log_puts("    c64_load [-v | -vv | -vvv] <dsp_image_pathname>");
   log_puts("");
}


/*--------------------------------------------------------------------------- loc_parse_args() */
static sBool loc_parse_args(int argc, char**argv) {
   sBool ret;

   if(argc < 2)
   {
      usage();

      ret = S_FALSE;
   }
   else if(3 == argc)
   {
      if(!strcmp(argv[1], "-v"))
      {
         // Enable debug
         log_debug_level = 1;
         dsp_image_pathname = argv[2];

         ret = S_TRUE;
      }
      else if(!strcmp(argv[1], "-vv"))
      {
         // Enable debug + file/opt/section header output
         log_debug_level = 10;
         dsp_image_pathname = argv[2];

         ret = S_TRUE;
      }
      else if(!strcmp(argv[1], "-vvv"))
      {
         // Detailed tracing (regaccess etc.)
         log_debug_level = 20;
         dsp_image_pathname = argv[2];

         ret = S_TRUE;
      }
      else
      {
         usage();
         
         ret = S_FALSE;
      }
   }
   else
   {
      dsp_image_pathname = argv[1];

      ret = S_TRUE;
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_testcode() */
#ifdef TESTCODE
static void loc_testcode(void) {
   // (todo) remove

   log_puts("xxx just try to disable the mmu");

   dsp_init_root();
   dsp_reset();
   dsp_start(0);
   usleep(1000000 * 1);
   dsp_power_down();
   dsp_exit_root();

   exit(10);
}
#endif


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

   if(0 == log_debug_level)
   {
      log_printf(LOG_INFO "reading %u section headers", coff_file_header.num_section_headers);
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

         log_printf(LOG_DEBUG "reading section header %u/%u\n",
                    (secIdx + 1),
                    coff_file_header.num_section_headers
                    );
      }
      else
      {
         putc('.', stdout);
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
         log_printf(LOG_ERROR "failed to parse section header %u/%u\n",
                (secIdx + 1),
                coff_file_header.num_section_headers
                );
         break;
      }

   } /* iterate section headers */

   if(0 == log_debug_level)
   {
      log_putc('\n');
   }

   if(LOG_DEBUG_LEVEL(10))
   {
      log_printf(LOG_DEBUG "coff_max_raw_data_size=0x%08x (%u)\n",
                 coff_max_raw_data_size,
                 coff_max_raw_data_size
                 );
   }

   return ret;
}


/*--------------------------------------------------------------------------- loc_read_sections */
static s_error_t loc_read_sections(void) {
   err_t              ret;
   sUI                secIdx = 0;
   coff_sec_header_t *secHdr = coff_sec_headers;
   sUI                totalTextSize = 0;
   sUI                totalDataSize = 0;
   sUI                totalBSSSize  = 0;

   /* Initialize DSP memory and copy sections */

   if(coff_max_raw_data_size > 0)
   {
      // (todo) load directly into DSP RAM */
      sU8 *rawBuf = osal_alloc(coff_max_raw_data_size);

      if(NULL != rawBuf)
      {
         log_puts(LOG_INFO "initializing DSP memory and copying sections..");

         /* Clear reset vector */
         ret.dsp = dsp_memset(dsp_config.reset_vector.phys_addr + DSP_RESET_VECTOR_CODESIZE,
                              0xCC,
                              (DSP_RESET_VECTOR_MINSIZE - DSP_RESET_VECTOR_CODESIZE)
                              );

         if(DSP_ERR_OK == ret.dsp)
         {
            while
               (
                  (secIdx < coff_file_header.num_section_headers) &&
                  (S_ERR_OK == ret.dsp)
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
                           log_printf(LOG_DEBUG "clearing BSS section @0x%08x, sz=0x%08x (%u)..",
                                      secHdr->phys_addr,
                                      secHdr->size,
                                      secHdr->size
                                      );
                        }

                        /* BSS, just clear the mem region */
                        if(secHdr->phys_addr >= dsp_config.reset_vector.phys_addr) /* avoid access to L1DSRAM */
                        {
                           ret.dsp = dsp_memset(secHdr->phys_addr, 0, secHdr->size);

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
                                 log_printf(LOG_DEBUG "copying TEXT/DATA section @0x%08x, sz=0x%08x (%u)..",
                                            secHdr->phys_addr,
                                            secHdr->size,
                                            secHdr->size
                                            );
                              }

                              /* Code or data, copy to DSP memory */
                              ret.dsp = dsp_copy_to(secHdr->phys_addr, rawBuf, secHdr->size);

                              if(LOG_DEBUG_LEVEL(10))
                              {
                                 log_puts("..OK!");
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
                              log_printf(LOG_ERROR "failed to read section #%d/%d raw data\n",
                                         (secIdx + 1),
                                         coff_file_header.num_section_headers
                                         );
                           }
                        }
                        else
                        {
                           log_printf(LOG_ERROR "failed to seek to section #%d/%d raw data\n",
                                      (secIdx + 1),
                                      coff_file_header.num_section_headers
                                      );
                        }
                     }
                     else
                     {
                        log_printf(LOG_WARN "section #%d/%d is flagged as TEXT or DATA but raw_data_off is 0. skipping..\n",
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

            /* Succeeded */
            log_printf(LOG_INFO " text=0x%08x  data=0x%08x  bss=0x%08x\n",
                       totalTextSize,
                       totalDataSize,
                       totalBSSSize
                       );

         } /* if clear reset vector */
         else
         {
            /* Failed: dsp_memset() */
            log_puts(LOG_FATAL "failed to zero RESET_VECTOR memory region.");
         }

         osal_free(rawBuf);

      } /* if allocate rawBuf */
      else
      {
         /* Failed: could not allocate raw code/data buffer */
         log_printf(LOG_ERROR "failed to allocate raw code/data buffer (%u bytes)\n",
                    coff_max_raw_data_size
                    );

         ret.coff = COFF_ERR_GEN_RAWBUFALLOC;
      }

   } /* if max raw size > 0 */
   else
   {
      /* Failed: COFF file contains no code or data */
      log_printf(LOG_ERROR "max_raw_data_size == 0 !! (no code/data in DSPImage ?!!\n");

      ret.coff = COFF_ERR_GEN_NOCODEORDATA;
   }

   return ret.any;
}


/*--------------------------------------------------------------------------- loc_check_dsp_activity() */
static dsp_error_t loc_check_dsp_activity(void) {
   dsp_error_t ret;

   /* Check if dsp is running */
#ifdef HAVE_OS_LINUX
   log_printf(LOG_DEBUG "waiting for DSP startup..\n");
   usleep(1000*250); /* may not be needed */
#endif /* HAVE_OS_LINUX */

#if 0
   if(LOG_DEBUG_LEVEL(21))
   {
      sU32 val;
      ret = dsp_reg_read_32(dsp_config.reset_vector.phys_addr + DSP_RESET_VECTOR_CODESIZE + 4,
                            &val
                            );
                                                
      log_printf("xxx read val=0x%08x after usleep\n", val);
   }
#endif

   {
      sU32 i;
      sU32 j = 64/4; /* (note) start at offset 64 -- the first 16 ints are reserved for DSP_MLB_RAM_xxx. */
      sBool bRunning = S_TRUE;

      for(i=64; i<(DSP_RESET_VECTOR_MINSIZE - DSP_RESET_VECTOR_CODESIZE); i+=4)
      {
         sU32 val;

         ret = dsp_reg_read_32(dsp_config.reset_vector.phys_addr + DSP_RESET_VECTOR_CODESIZE + i, &val);

         if(DSP_ERR_OK == ret)
         {
            if(val != j)
            {
               /* Failed: DSP did not write expected value */
               if(LOG_DEBUG_LEVEL(30))
               {
                  log_printf(LOG_ERROR "check_dsp: expected %d, have %d.\n", j, val);
               }

               bRunning = S_FALSE;
               ///break;
            }
         }

         /* Next value */
         j++;
      }

      if(bRunning)
      {
         ret = DSP_ERR_OK;
      }
      else
      {
         ret = DSP_ERR_MEM_CHECK;
      }
   }

   return ret;
}


/*--------------------------------------------------------------------------- main */
int main(int argc, char**argv) {
   int ret;

#ifdef TESTCODE
   loc_testcode();
#endif

   if(loc_parse_args(argc, argv))
   {
      err_t err;

      err.coff = coff_init();

      if(COFF_ERR_OK == err.coff)
      {
         /* Open DSP image file */
         log_printf(LOG_INFO "loading DSP image from \"%s\"\n", dsp_image_pathname);

         ret = coff_open(dsp_image_pathname);
         
         if(COFF_ERR_OK == err.coff)
         {
            /* Load file and opt headers and read string table */
            err.coff = loc_open_and_read_basic_headers();

            if(COFF_ERR_OK == err.coff)
            {
               err.coff = loc_read_section_headers();

               if(COFF_ERR_OK == err.coff)
               {
                  /* Check whether DSP image was built with DSPBIOS 5.xx */
                  if(b_coff_bios_5xx)
                  {
                     dsp_autoresume_set(S_FALSE);

                     err.dsp = dsp_init_root();
                  
                     if(DSP_ERR_OK == err.dsp)
                     {
                        /* Hold DSP in reset so we can safely update its memory regions */
                        //err.dsp = dsp_reset();
                        err.dsp = dsp_power_on_and_reset();

                        if(DSP_ERR_OK == err.dsp)
                        {
                           /* Load sections and copy them to DSP memory */
                           err.coff = loc_read_sections();

                           if(COFF_ERR_OK == err.coff)
                           {
                              log_puts(LOG_INFO "DSP image loaded successfully. starting DSP..");

                              /* Release DSP from reset and jump to _c_int00 (boot.o64P in .sysinit section) */
                              err.dsp = dsp_start(coff_opt_header.entry_point);

                              if(DSP_ERR_OK == err.dsp)
                              {
                                 err.dsp = loc_check_dsp_activity();

                                 if(DSP_ERR_OK == err.dsp)
                                 {
                                    /* Succeeded (yay!) */
                                    log_printf(LOG_INFO "DSP is up and running!\n");

                                    /* Send a test message
                                     *  
                                     *  (note) seems to be superfluous
                                     *          (was only needed in very early releases of c64_tools)
                                     */
#if 0
                                    dsp_send_forced_message(0x37703770u);
#endif
                                 }
                                 else
                                 {
                                    log_printf(LOG_ERROR "DSP does not seem to be running.\n");
                                 }
                              }
                           } /* if loc_read_sections() */
                           else
                           {
                              log_puts(LOG_ERROR "failed to read DSP image sections.");
                           }
                        
                        } /* if dsp_poweron_and_reset() */
                        else
                        {
                           /* Failed: dsp_reset() */
                           log_puts(LOG_ERROR "failed to hold DSP in reset.");
                        }

                        /* Unmap DSP registers and RAM, close /dev/c64 */
                        dsp_exit_root();

                     } /* if dsp_init_root() */
                  
                  } // if b_coff_bios_5xx
                  else
                  {
                     log_printf(LOG_ERROR "DSPImage was not compiled/linked with DSPBIOS 5.xx.\n");
                     err.coff = COFF_ERR_GEN_BIOS5XX;
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
         ret = 0;
      }
      else
      {
         const char *msg;

         msg = dsp_strerror(err.dsp);

         if(NULL == msg)
         {
            msg = coff_strerror(err.coff);

            if(NULL == msg)
            {
               msg = osal_strerror(err.osal);
            }
         }

         log_printf(LOG_ERROR "c64_load failed with error code 0x%08x (\"%s\")\n",
                    (sUI)err.any,
                    msg
                    );
         
         ret = 10;
      }

   }
   else
   {
      /* Failed: loc_parse_args() */
      ret = 10;
   }

   return ret;
}
