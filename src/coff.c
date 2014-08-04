/* ----
 * ---- file   : coff.c
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : COFF2 loader. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 06Sep2013, 08Sep2013, 04Oct2013
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


/*--------------------------------------------------------------------------- global vars */
coff_file_header_t coff_file_header;
coff_opt_header_t  coff_opt_header;
coff_sec_header_t *coff_sec_headers;

sBool b_coff_bios_5xx; // 1=built with DSPBIOS 5.xx (required). set while parsing the section headers.

sU32 coff_cinit_addr;
sU32 coff_cinit_size;

sUI    coff_string_table_size;
sChar *coff_string_table;

sUI coff_max_raw_data_size;


/*--------------------------------------------------------------------------- module vars */
static FILE *coff_fh;
static sUI   coff_size;
static sSI   coff_off;


/*--------------------------------------------------------------------------- loc_convert_sec_char() */
static char loc_convert_sec_char(char _c) {
   if(_c >= ' ')
   {
      return _c;
   }
   else
   {
      return ' ';
   }
}


static void loc_print_file_header_flags(sU32 _flags) {
   if(0 != (_flags & COFF_FILE_HEADER_FLAG_RELFLG))
   {
      log_printf("RELFLG ");
   }

   if(0 != (_flags & COFF_FILE_HEADER_FLAG_EXEC))
   {
      log_printf("EXEC ");
   }

   if(0 != (_flags & COFF_FILE_HEADER_FLAG_LNNO))
   {
      log_printf("LNNO ");
   }

   if(0 != (_flags & COFF_FILE_HEADER_FLAG_LSYMS))
   {
      log_printf("LSYMS ");
   }

   if(0 != (_flags & COFF_FILE_HEADER_FLAG_UNKWN1))
   {
      log_printf("UNKWN1 ");
   }

   if(0 != (_flags & COFF_FILE_HEADER_FLAG_UNKWN2))
   {
      log_printf("UNKWN2 ");
   }

   if(0 != (_flags & COFF_FILE_HEADER_FLAG_LITTLE))
   {
      log_printf("LITTLE ");
   }

   if(0 != (_flags & COFF_FILE_HEADER_FLAG_BIG))
   {
      log_printf("BIG ");
   }

   if(0 != (_flags & COFF_FILE_HEADER_FLAG_SYMMERGE))
   {
      log_printf("SYMMERGE ");
   }
}

/*--------------------------------------------------------------------------- loc_print_sec_header_flags() */
static void loc_print_sec_header_flags(sU32 _flags) {
   if(0 != (_flags & COFF_SEC_HEADER_FLAG_REG))
   {
      log_printf("REG ");
   }
   
   if(0 != (_flags & COFF_SEC_HEADER_FLAG_DSECT))
   {
      log_printf("DSECT ");
   }

   if(0 != (_flags & COFF_SEC_HEADER_FLAG_NOLOAD))
   {
      log_printf("NOLOAD ");
   }

   if(0 != (_flags & COFF_SEC_HEADER_FLAG_GROUP))
   {
      log_printf("GROUP ");
   }

   if(0 != (_flags & COFF_SEC_HEADER_FLAG_PAD))
   {
      log_printf("PAD ");
   }

   if(0 != (_flags & COFF_SEC_HEADER_FLAG_COPY))
   {
      log_printf("COPY ");
   }

   if(0 != (_flags & COFF_SEC_HEADER_FLAG_TEXT))
   {
      log_printf("TEXT ");
   }

   if(0 != (_flags & COFF_SEC_HEADER_FLAG_DATA))
   {
      log_printf("DATA ");
   }

   if(0 != (_flags & COFF_SEC_HEADER_FLAG_BSS))
   {
      log_printf("BSS ");
   }

   if(0 != (_flags & COFF_SEC_HEADER_FLAG_BLOCK))
   {
      log_printf("BLOCK ");
   }

   if(0 != (_flags & COFF_SEC_HEADER_FLAG_PASS))
   {
      log_printf("PASS ");
   }

   if(0 != (_flags & COFF_SEC_HEADER_FLAG_CLINK))
   {
      log_printf("CLINK ");
   }

   if(0 != (_flags & COFF_SEC_HEADER_FLAG_VECTOR))
   {
      log_printf("VECTOR ");
   }

   if(0 != (_flags & COFF_SEC_HEADER_FLAG_PADDED))
   {
      log_printf("PADDED ");
   }
}


/*--------------------------------------------------------------------------- coff_strerror() */
const char * S_CALL coff_strerror(coff_error_t _err) {
   switch(_err)
   {
      default:
         return NULL;

      case COFF_ERR_OK:                    return "OK";

      case COFF_ERR_GEN_STRUCTPACK:        return "COFF_ERR_GEN_STRUCTPACK";
      case COFF_ERR_GEN_BIOS5XX:           return "COFF_ERR_GEN_BIOS5XX";
      case COFF_ERR_GEN_NOCODEORDATA:      return "COFF_ERR_GEN_NOCODEORDATA";
      case COFF_ERR_GEN_RAWBUFALLOC:       return "COFF_ERR_GEN_RAWBUFALLOC";

      case COFF_ERR_FILE:                  return "COFF_ERR_FILE";
      case COFF_ERR_FILE_ALREADYOPEN:      return "COFF_ERR_FILE_ALREADYOPEN";
      case COFF_ERR_FILE_NOTFOUND:         return "COFF_ERR_FILE_NOTFOUND";
      case COFF_ERR_FILE_NOTOPEN:          return "COFF_ERR_FILE_NOTOPEN";
      case COFF_ERR_FILE_READOUTOFBOUNDS:  return "COFF_ERR_FILE_READOUTOFBOUNDS";
      case COFF_ERR_FILE_READ:             return "COFF_ERR_FILE_READ";
      case COFF_ERR_FILE_SEEK:             return "COFF_ERR_FILE_SEEK";

      case COFF_ERR_FILE_HEADER:           return "COFF_ERR_FILE_HEADER";
      case COFF_ERR_FILE_HEADER_VERSION:   return "COFF_ERR_FILE_HEADER_VERSION";
      case COFF_ERR_FILE_HEADER_FLAGS:     return "COFF_ERR_FILE_HEADER_FLAGS";
      case COFF_ERR_FILE_HEADER_OPTSIZE:   return "COFF_ERR_FILE_HEADER_OPTSIZE";
      case COFF_ERR_FILE_HEADER_TARGETID:  return "COFF_ERR_FILE_HEADER_TARGETID";

      case COFF_ERR_OPT_HEADER:            return "COFF_ERR_OPT_HEADER";
      case COFF_ERR_OPT_HEADER_MAGIC:      return "COFF_ERR_OPT_HEADER_MAGIC";

      case COFF_ERR_SEC_HEADER:            return "COFF_ERR_SEC_HEADER";
      case COFF_ERR_SEC_HEADER_ALLOC:      return "COFF_ERR_SEC_HEADER_ALLOC";

      case COFF_ERR_STRTBL:                return "COFF_ERR_STRTBL";
      case COFF_ERR_STRTBL_SEEK:           return "COFF_ERR_STRTBL_SEEK";
      case COFF_ERR_STRTBL_READSZ:         return "COFF_ERR_STRTBL_READSZ";
      case COFF_ERR_STRTBL_ALLOC:          return "COFF_ERR_STRTBL_ALLOC";
      case COFF_ERR_STRTBL_READCHARS:      return "COFF_ERR_STRTBL_READCHARS";
   }
}


/*--------------------------------------------------------------------------- coff_init() */
coff_error_t S_CALL coff_init(void) {
   coff_error_t ret;

   coff_fh   = NULL;
   coff_size = 0;
   coff_off  = 0;

   coff_sec_headers       = NULL;
   b_coff_bios_5xx        = S_FALSE;
   coff_cinit_addr        = 0;
   coff_cinit_size        = 0;
   coff_string_table_size = 0;
   coff_string_table      = NULL;
   coff_max_raw_data_size = 0;

   // Check whether the compiler's structure packing matches
   ret = COFF_ERR_GEN_STRUCTPACK;

   if(COFF_FILE_HEADER_SIZE == sizeof(coff_file_header_t))
   {
      if(COFF_OPT_HEADER_SIZE == sizeof(coff_opt_header_t))
      {
         if(COFF_SEC_HEADER_SIZE == sizeof(coff_sec_header_t))
         {
            if(COFF_SYM_ENTRY_SIZE == sizeof(coff_sym_entry_t))
            {
               // Succeeded
               ret = COFF_ERR_OK;
            }
            else
            {
               log_printf(LOG_ERROR "coff_init: sizeof(coff_sym_entry_t) (%u) != %u\n",
                          sizeof(coff_sym_entry_t),
                          COFF_SYM_ENTRY_SIZE
                          );
            }
         }
         else
         {
            log_printf(LOG_ERROR "coff_init: sizeof(coff_sec_header_t) (%u) != %u\n",
                       sizeof(coff_sec_header_t),
                       COFF_SEC_HEADER_SIZE
                       );
         }
      }
      else
      {
         log_printf(LOG_ERROR "coff_init: sizeof(coff_opt_header_t) (%u) != %u\n",
                    sizeof(coff_opt_header_t),
                    COFF_OPT_HEADER_SIZE
                    );
      }
   }
   else
   {
      log_printf(LOG_ERROR "coff_init: sizeof(coff_file_header_t) (%u) != %u\n", 
                 sizeof(coff_file_header_t),
                 COFF_FILE_HEADER_SIZE
                 );
   }

   return ret;
}


/*--------------------------------------------------------------------------- coff_exit() */
void S_CALL coff_exit(void) {
   coff_close();

   // Free string table
   if(NULL != coff_string_table)
   {
      osal_free(coff_string_table);
      coff_string_table = NULL;
   }

   // Free section headers
   if(NULL != coff_sec_headers)
   {
      osal_free(coff_sec_headers);
      coff_sec_headers = NULL;
   }
}


/*--------------------------------------------------------------------------- coff_open() */
coff_error_t S_CALL coff_open(const char *_pathName) {
   coff_error_t ret;

   if(NULL == coff_fh)
   {
      coff_fh = fopen(_pathName, "rb");

      if(NULL != coff_fh)
      {
         // Determine file size
         fseek(coff_fh, 0, SEEK_END);
         coff_size = (sUI) ftell(coff_fh);
         fseek(coff_fh, 0, SEEK_SET);

         if(LOG_DEBUG_LEVEL(10))
         {
            log_printf(LOG_DEBUG "coff_open: file size=%u (0x%08x)\n", coff_size, coff_size);
         }

         // Succeeded
         ret = COFF_ERR_OK;
      }
      else
      {
         ret = COFF_ERR_FILE_NOTFOUND;
      }
   }
   else
   {
      ret = COFF_ERR_FILE_ALREADYOPEN;
   }
   
   return ret;
}


/*--------------------------------------------------------------------------- coff_read() */
coff_error_t S_CALL coff_read(void *_dest, sUI _numBytes) {
   coff_error_t ret;

   // printf("xxx coff_read: coff_off=%u numBytes=%u\n", coff_off, _numBytes);

   if(NULL != coff_fh)
   {
      if((coff_off + _numBytes) <= coff_size)
      {
         sUI numRead = (sUI) fread(_dest, 1, (size_t)_numBytes, coff_fh);

         if(numRead == _numBytes)
         {
            coff_off += numRead;

            // Succeeded
            ret = COFF_ERR_OK;
         }
         else
         {
            // Failed: short read
            log_printf(LOG_ERROR "coff_read: fread() %u/%u bytes\n", numRead, _numBytes);

            ret = COFF_ERR_FILE_READ;
         }
      }
      else
      {
         ret = COFF_ERR_FILE_READOUTOFBOUNDS;
      }
   }
   else
   {
      ret = COFF_ERR_FILE_NOTOPEN;
   }

   return ret;
}


/*--------------------------------------------------------------------------- coff_seek_abs() */
coff_error_t S_CALL coff_seek_abs(sUI _offset) {
   coff_error_t ret;

   if(0 == fseek(coff_fh, _offset, SEEK_SET))
   {
      // Succeeded
      coff_off = _offset;
      ret = COFF_ERR_OK;
   }
   else
   {
      // Failed: fseek()
      ret = COFF_ERR_FILE_SEEK;
   }

   return ret;
}


/*--------------------------------------------------------------------------- coff_close() */
void coff_close(void) {
   if(NULL != coff_fh)
   {
      fclose(coff_fh);
      coff_fh = NULL;
   }
}


/*--------------------------------------------------------------------------- coff_file_header_parse() */
coff_error_t S_CALL coff_file_header_parse(void) {
   coff_error_t ret;

   ret = coff_read(&coff_file_header, COFF_FILE_HEADER_SIZE);

   if(COFF_ERR_OK == ret)
   {
      if(COFF_FILE_HEADER_VERSION == coff_file_header.version)
      {
         if(LOG_DEBUG_LEVEL(20))
         {
            log_puts  (LOG_DEBUG "coff_file_header_parse: read file header");
            log_printf(LOG_DEBUG "\t             version: 0x%04x\n",  coff_file_header.version);
            log_printf(LOG_DEBUG "\t num_section_headers: %d\n",      coff_file_header.num_section_headers);
            log_printf(LOG_DEBUG "\t           date_time: %d (",      coff_file_header.date_time);
            {
               static const char *monthNames[] = {
                  "Jan", "Feb", "Mar," "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
               };
               struct tm *ntime = localtime((time_t*)&coff_file_header.date_time);
               log_printf("%d/",    (1900 + ntime->tm_year));
               log_printf("%s/",     monthNames[ntime->tm_mon - 1]);
               log_printf("%02d ",   ntime->tm_mday);
               log_printf("%02d:",   ntime->tm_hour);
               log_printf("%02d:",   ntime->tm_min);
               log_printf("%02d)\n", ntime->tm_sec);
            }
            log_printf(LOG_DEBUG "\t      sym_tab_offset: %d\n",      coff_file_header.sym_tab_offset);
            log_printf(LOG_DEBUG "\t num_sym_tab_entries: %d\n",      coff_file_header.num_sym_tab_entries);
            log_printf(LOG_DEBUG "\tnum_opt_header_bytes: %d\n",      coff_file_header.num_opt_header_bytes);
            log_printf(LOG_DEBUG "\t               flags: 0x%04x ( ", coff_file_header.flags);
            loc_print_file_header_flags(coff_file_header.flags);
            log_puts(")");
         }

         if( (
                COFF_FILE_HEADER_FLAG_RELFLG |
                COFF_FILE_HEADER_FLAG_EXEC   |
                COFF_FILE_HEADER_FLAG_UNKWN1 |
                COFF_FILE_HEADER_FLAG_UNKWN2 |
                COFF_FILE_HEADER_FLAG_LITTLE |
                COFF_FILE_HEADER_FLAG_SYMMERGE
              )
             == coff_file_header.flags
             )
         {
            if(COFF_OPT_HEADER_SIZE == coff_file_header.num_opt_header_bytes)
            {
               if(COFF_FILE_HEADER_TARGETID_TMS320C6000 == coff_file_header.target_id)
               {
                  coff_sec_headers = osal_alloc(COFF_SEC_HEADER_SIZE * coff_file_header.num_section_headers);

                  if(NULL != coff_sec_headers)
                  {
                     // Succeeded
                     ret = COFF_ERR_OK;
                  }
                  else
                  {
                     // Failed: could not allocate section headers
                     ret = COFF_ERR_SEC_HEADER_ALLOC;
                  }
               }
               else
               {
                  // Failed: unsupported target_id
                  log_printf(LOG_ERROR "coff_file_header_parse: unsupported target id (0x%04x)\n", coff_file_header.target_id);

                  ret = COFF_ERR_FILE_HEADER_TARGETID;
               }
            }
            else
            {
               // Failed: unsupported opt header size
               log_printf(LOG_ERROR "coff_file_header_parse: unsupported opt header size (%u)\n", coff_file_header.num_opt_header_bytes);

               ret = COFF_ERR_FILE_HEADER_OPTSIZE;
            }
         }
         else
         {
            // Failed: unsupported flag combination
            log_printf(LOG_ERROR "coff_file_header_parse: unsupported flag set (0x%08x)\n", coff_file_header.flags);

            ret = COFF_ERR_FILE_HEADER_FLAGS;
         }
      }
      else
      {
         log_printf(LOG_ERROR "coff_file_header_parse: expected version 0x%04x, have 0x%04x\n",
                    COFF_FILE_HEADER_VERSION,
                    coff_file_header.version
                    );

         ret = COFF_ERR_FILE_HEADER_VERSION;
      }
   }

   return ret;
}


/*--------------------------------------------------------------------------- coff_opt_header_parse() */
coff_error_t S_CALL coff_opt_header_parse(void) {
   coff_error_t ret;

   ret = coff_read(&coff_opt_header, COFF_OPT_HEADER_SIZE);

   if(COFF_ERR_OK == ret)
   {
      if(COFF_OPT_HEADER_MAGIC == coff_opt_header.magic)
      {
         if(LOG_DEBUG_LEVEL(20))
         {
            log_puts  (LOG_DEBUG "coff_opt_header_parse: read optional header");
            log_printf(LOG_DEBUG "\t          magic: 0x%04x\n"     , coff_opt_header.magic);
            log_printf(LOG_DEBUG "\t        version: 0x%04x\n",      coff_opt_header.version);
            log_printf(LOG_DEBUG "\t      text_size: 0x%08x (%u)\n", coff_opt_header.text_size, coff_opt_header.text_size);
            log_printf(LOG_DEBUG "\t      init_size: 0x%08x (%u)\n", coff_opt_header.init_size, coff_opt_header.init_size);
            log_printf(LOG_DEBUG "\t       bss_size: 0x%08x (%u)\n", coff_opt_header.bss_size, coff_opt_header.bss_size);
            log_printf(LOG_DEBUG "\t    entry_point: 0x%08x\n",      coff_opt_header.entry_point);
            log_printf(LOG_DEBUG "\texec_start_addr: 0x%08x\n",      coff_opt_header.exec_start_addr);
            log_printf(LOG_DEBUG "\tinit_start_addr: 0x%08x\n",      coff_opt_header.init_start_addr);
         }

         // Succeeded
         ret = COFF_ERR_OK;
      }
      else
      {
         // Failed: wrong magic
         log_printf(LOG_ERROR "coff_opt_header_parse: expected magic 0x%04x, have 0x%04x\n",
                    COFF_OPT_HEADER_MAGIC,
                    coff_opt_header.magic
                    );
         ret = COFF_ERR_OPT_HEADER_MAGIC;
      }
   }

   return ret;
}


/*--------------------------------------------------------------------------- coff_string_table_read() */
coff_error_t S_CALL coff_string_table_read(void) {
   coff_error_t ret;

   sUI strTblOff =
      // COFF_FILE_HEADER_SIZE + COFF_OPT_HEADER_SIZE +
      // (coff_file_header.num_section_headers * COFF_SEC_HEADER_SIZE) +
      coff_file_header.sym_tab_offset + (coff_file_header.num_sym_tab_entries * COFF_SYM_ENTRY_SIZE)
      ;

   sUI oldFileOff = coff_off;

   if(LOG_DEBUG_LEVEL(20))
   {
      log_printf(LOG_DEBUG "coff_string_table_read: tbl offset=0x%08x (%u)\n", strTblOff, strTblOff);
   }

   ret = coff_seek_abs(strTblOff);

   if(COFF_ERR_OK == ret)
   {
      ret = coff_read(&coff_string_table_size, 4);

      if(COFF_ERR_OK == ret)
      {
         if(LOG_DEBUG_LEVEL(20))
         {
            log_printf(LOG_DEBUG "coff_string_table_read: tbl   size=0x%08x (%u)\n", coff_string_table_size, coff_string_table_size);
         }

         if( (coff_string_table_size >= 4) && ((strTblOff + coff_string_table_size) <= coff_size) )
         {
            // (note) size includes size field itself, subtract that
            coff_string_table_size -= 4;

            coff_string_table = osal_alloc(coff_string_table_size);

            if(NULL != coff_string_table)
            {
               ret = coff_read(coff_string_table, coff_string_table_size);

               if(COFF_ERR_OK == ret)
               {
                  // Succeeded (well, almost)
                  ret = coff_seek_abs(oldFileOff);
               }
               else
               {
                  // Failed: coff_read()
                  ret = COFF_ERR_STRTBL_READCHARS;
               }
            }
            else
            {
               // Failed: osal_alloc()
               log_printf(LOG_ERROR "coff_string_table_read: failed to allocate string table (%u bytes)\n", coff_string_table_size);

               ret = COFF_ERR_STRTBL_ALLOC;
            }
         }
         else
         {
            // Failed: invalid string table offset or size
            log_printf(LOG_ERROR "coff_string_table_read: invalid string table offset or size\n");

            ret = COFF_ERR_STRTBL;
         }
      }
      else
      {
         // Failed: coff_read()
         ret = COFF_ERR_STRTBL_READSZ;
      }
   }
   else
   {
      // Failed: coff_seek_abs()
      ret = COFF_ERR_STRTBL_SEEK;
   }
      
   return ret;
}


/*--------------------------------------------------------------------------- coff_string_table_get() */
const char * S_CALL coff_string_table_get(sUI _offset) {
   const char *ret;

   if(_offset < coff_string_table_size)
   {
      ret = coff_string_table + _offset;
   }
   else
   {
      // Failed: offset out of bounds
      ret = NULL;
   }

   return ret;
}


/*--------------------------------------------------------------------------- coff_sec_header_parse() */
coff_error_t S_CALL coff_sec_header_parse(coff_sec_header_t *_secHdr) {
   coff_error_t ret;

   ret = coff_read(_secHdr, COFF_SEC_HEADER_SIZE);

   if(COFF_ERR_OK == ret)
   {
      if(LOG_DEBUG_LEVEL(20))
      {
         log_puts  (LOG_DEBUG "coff_sec_header_parse: read section header");
         log_printf(LOG_DEBUG"\t              name: ");
      }

      if(0 == _secHdr->name.str_table_offset.lo)
      {
         if(LOG_DEBUG_LEVEL(20))
         {
            const char *longName;

            // (note) offset must be at least 4 (the first four bytes of the string table store the table size)
            log_printf("str_table_offset=0x%08x (%u)\n",
                       _secHdr->name.str_table_offset.hi,
                       _secHdr->name.str_table_offset.hi
                       );

            longName = coff_string_table_get(_secHdr->name.str_table_offset.hi);

            log_printf("                           ==> \"%s\"\n",
                       longName
                       );
         }
      }
      else
      {
         if(LOG_DEBUG_LEVEL(20))
         {
            log_printf("chars=\"%c%c%c%c%c%c%c%c\"\n",
                       loc_convert_sec_char(_secHdr->name.chars[0]),
                       loc_convert_sec_char(_secHdr->name.chars[1]),
                       loc_convert_sec_char(_secHdr->name.chars[2]),
                       loc_convert_sec_char(_secHdr->name.chars[3]),
                       loc_convert_sec_char(_secHdr->name.chars[4]),
                       loc_convert_sec_char(_secHdr->name.chars[5]),
                       loc_convert_sec_char(_secHdr->name.chars[6]),
                       loc_convert_sec_char(_secHdr->name.chars[7])
                       );
         }

         if(!b_coff_bios_5xx)
         {
            if(
               ('.' == _secHdr->name.chars[0]) &&
               ('b' == _secHdr->name.chars[1]) &&
               ('i' == _secHdr->name.chars[2]) &&
               ('o' == _secHdr->name.chars[3]) &&
               ('s' == _secHdr->name.chars[4]) 
               )
            {
               b_coff_bios_5xx = S_TRUE;
            }
         }

         if(0 == coff_cinit_addr)
         {
            if(
               ('.' == _secHdr->name.chars[0]) &&
               ('c' == _secHdr->name.chars[1]) &&
               ('i' == _secHdr->name.chars[2]) &&
               ('n' == _secHdr->name.chars[3]) &&
               ('i' == _secHdr->name.chars[4]) && 
               ('t' == _secHdr->name.chars[5]) 
               )
            {
               coff_cinit_addr = _secHdr->phys_addr;
               coff_cinit_size = _secHdr->size;
            }
         }

         // Track largest text/data section
         if(_secHdr->raw_data_off > 0)
         {
            if(_secHdr->size > coff_max_raw_data_size)
            {
               coff_max_raw_data_size = _secHdr->size;
            }
         }
      }

      if(LOG_DEBUG_LEVEL(20))
      {
         log_printf(LOG_DEBUG "\t         phys_addr: 0x%08x\n",      _secHdr->phys_addr);
         log_printf(LOG_DEBUG "\t         virt_addr: 0x%08x\n",      _secHdr->virt_addr);
         log_printf(LOG_DEBUG "\t              size: 0x%08x (%u)\n", _secHdr->size, _secHdr->size);
         log_printf(LOG_DEBUG "\t      raw_data_off: 0x%08x (%u)\n", _secHdr->raw_data_off, _secHdr->raw_data_off);
         log_printf(LOG_DEBUG "\t         reloc_off: 0x%08x (%u)\n", _secHdr->reloc_off, _secHdr->reloc_off);
         log_printf(LOG_DEBUG "\t          reservd1: 0x%08x (%u)\n", _secHdr->reservd1, _secHdr->reservd1);
         log_printf(LOG_DEBUG "\t num_reloc_entries: %u\n",          _secHdr->num_reloc_entries);
         log_printf(LOG_DEBUG "\tnum_lineno_entries: %u\n",          _secHdr->num_lineno_entries);
         log_printf(LOG_DEBUG "\t             flags: 0x%08x ( ",     _secHdr->flags);
         loc_print_sec_header_flags(_secHdr->flags);
         log_puts(")");

         log_printf(LOG_DEBUG "\t          reservd2: 0x%04x\n",      _secHdr->reservd2);
         log_printf(LOG_DEBUG "\t       mem_page_nr: 0x%04x (%u)\n", _secHdr->mem_page_nr, _secHdr->mem_page_nr);
      }

      // Succeeded
      ret = COFF_ERR_OK;
   }

   return ret;
}
