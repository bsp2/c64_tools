/* ----
 * ---- file   : coff.h
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : COFF2 constants, types and structs. This is part of the "c64_load" utility.
 * ----
 * ---- changed: 06Sep2013, 08Sep2013, 04Oct2013
 * ----
 * ----
 */

#ifndef __C64_TOOLS_COFF_H__
#define __C64_TOOLS_COFF_H__

#include "../include/cplusplus_begin.h"


/* (note) the page/table numbers refer to spraao8.pdf */


typedef union {
   sChar chars[8];
   struct {
      sU32 lo;
      sU32 hi;
   } str_table_offset;
} coff_name_t;


#define COFF_FILE_HEADER_FLAG_RELFLG    (0x0001u)   // Relocation information was stripped from the file
#define COFF_FILE_HEADER_FLAG_EXEC      (0x0002u)   // The file is relocatable (it contains no unresolved external references)
#define COFF_FILE_HEADER_FLAG_LNNO      (0x0004u)   // TMS320/470: Line numbers were stripped from the file
#define COFF_FILE_HEADER_FLAG_LSYMS     (0x0008u)   // Local symbols were stripped from the file
#define COFF_FILE_HEADER_FLAG_UNKWN1    (0x0020u)   // ?
#define COFF_FILE_HEADER_FLAG_UNKWN2    (0x0040u)   // ?
#define COFF_FILE_HEADER_FLAG_LITTLE    (0x0100u)   // The target is a little endian device
#define COFF_FILE_HEADER_FLAG_BIG       (0x0200u)   // C6000/MSP430/TMS470: The target is a big endian device
#define COFF_FILE_HEADER_FLAG_SYMMERGE  (0x1000u)   // C2800/MSP430/TMS470: Duplicate symbols were removed (resvd on C6000)


#define COFF_FILE_HEADER_TARGETID_TMS470       (0x0097u)
#define COFF_FILE_HEADER_TARGETID_TMS320C5400  (0x0098u)
#define COFF_FILE_HEADER_TARGETID_TMS320C6000  (0x0099u) // <-- the C64+
#define COFF_FILE_HEADER_TARGETID_TMS320C5500  (0x009Cu)
#define COFF_FILE_HEADER_TARGETID_TMS320C2800  (0x009Du)
#define COFF_FILE_HEADER_TARGETID_MSP430       (0x00A0u)
#define COFF_FILE_HEADER_TARGETID_TMS320C5500P (0x00A1u)


typedef struct { // 22 bytes
   sU16 version;               // +00: 0xC2 00 if little endian, 0x00 C2 if byte swapped (not supported by c64_load)
   sU16 num_section_headers;   // +02: number of section headers (always 53 on C64+/BIOS 5.xx?)
   sU32 date_time;             // +04: time/date stamp, indicates when the file was created
   sU32 sym_tab_offset;        // +08: offset of symbol table
   sU32 num_sym_tab_entries;   // +12: number of entries in the symbol table
   sU16 num_opt_header_bytes;  // +16: size of optional header (0 or 28) (is 28/0x1C) in DSP image). see coff_opt_header_t.
   sU16 flags;                 // +18: see p.4/Table 2 (0x1163 in DSP image). see COFF_FILE_HEADER_FLAG_
   sU16 target_id;             // +20: contains COFF_FILE_HEADER_TARGETID_TMS320C6000 (0x99 0x00). see COFF_FILE_HEADER_TARGETID_xxx
                               //  see p.4/Table 3 (0x0099 in DSP image)
} coff_file_header_t;

#define COFF_FILE_HEADER_SIZE (22)

#define COFF_FILE_HEADER_VERSION (0x00C2u)  // (note) 0x014c for WIN32 PE COFF files


typedef struct { // 28 bytes
   sU16 magic;            // +00: magic number (0x0108)
   sU16 version;          // +02: version stamp
   sU32 text_size;        // +04: size of executable code
   sU32 init_size;        // +08: size of initialized data
   sU32 bss_size;         // +12: size of uninitialized data
   sU32 entry_point;      // +16: entry point (boot.o64P (.sysinit) (=> _c_int00)
   sU32 exec_start_addr;  // +20: start address of executable data
   sU32 init_start_addr;  // +24: start address of initialized data
} coff_opt_header_t;

#define COFF_OPT_HEADER_MAGIC  (0x0108u)
#define COFF_OPT_HEADER_SIZE   (28)


typedef struct { // 48 bytes
   coff_name_t name;                // +00: 8 character section name or pointer into string table if name is longer than 8 chars
   sU32        phys_addr;           // +08: section's physical address
   sU32        virt_addr;           // +12: section's virtual address (equal to phys_addr on C64+)
   sU32        size;                // +16: section size in bytes (C6000/C55x/TMS470/TMS430) or words (C2800/C5400)
   sU32        raw_data_off;        // +20: file pointer to raw data
   sU32        reloc_off;           // +24: file pointer to relocation entries
   sU32        reservd1;            // +28: reserved
   sU32        num_reloc_entries;   // +32: number of relocation entries
   sU32        num_lineno_entries;  // +36: TMS470/TMS430 only: Number of line number entries
   sU32        flags;               // +40: (see p.6/Table 7). see COFF_SEC_HADER_FLAG_xxx
   sU16        reservd2;            // +44: reserved (either 0x0000 or 0x0008)
   sU16        mem_page_nr;         // +46: memory page number (not used on C64+, always 0)   
   
} coff_sec_header_t;

#define COFF_SEC_HEADER_SIZE  (48)


// (note) "loaded" means that the raw data for this section appears in the object file
//         only allocated sections are written to target memory

#define COFF_SEC_HEADER_FLAG_REG     (0x00000000u) // Regular section (allocated, relocated, loaded)
#define COFF_SEC_HEADER_FLAG_DSECT   (0x00000001u) // Dummy section (not allocated, relocated, not loaded)
#define COFF_SEC_HEADER_FLAG_NOLOAD  (0x00000002u) // Noload section (allocated, relocated, not loaded)
#define COFF_SEC_HEADER_FLAG_GROUP   (0x00000004u) // Grouped section (formed from several input sections
#define COFF_SEC_HEADER_FLAG_PAD     (0x00000008u) // Padding section (not allocated, not relocated, loaded)
#define COFF_SEC_HEADER_FLAG_COPY    (0x00000010u) // Copy section (not allocated, relocated, loaded)
#define COFF_SEC_HEADER_FLAG_TEXT    (0x00000020u) // Section contains executable code
#define COFF_SEC_HEADER_FLAG_DATA    (0x00000040u) // Section contains initialized data
#define COFF_SEC_HEADER_FLAG_BSS     (0x00000080u) // Section contains uninitialized data
#define COFF_SEC_HEADER_FLAG_BLOCK   (0x00001000u) // Alignment used as a blocking factor (?)
#define COFF_SEC_HEADER_FLAG_PASS    (0x00002000u) // Section should pass through unchanged
#define COFF_SEC_HEADER_FLAG_CLINK   (0x00004000u) // Section requires conditional linking
#define COFF_SEC_HEADER_FLAG_VECTOR  (0x00008000u) // Section contains vector table
#define COFF_SEC_HEADER_FLAG_PADDED  (0x00010000u) // Section has been padded


typedef struct { // 18 bytes
   coff_name_t name;                // +00: 8 character symbol entry name or pointer into string table if name is longer than 8 chars
   sU32        value;               // +08: Symbol value; storage class dependent
   sS16        sec_nr;              // +12: Section number of the symbol (-2=resvd, -1=abs.sym, 0=undef ext., 1=.text, 2=.data, 3=.bss, 4..32767 sec#)
   sU16        reservd1;            // +14: Reserved
   sU8         storage_class;       // +16: Storage class of the symbol
   sU8         num_aux_entries;     // +17: Number of auxiliary entries (always 0 or 1)
} coff_sym_entry_t;

#define COFF_SYM_ENTRY_SIZE  (18u)
 

#define COFF_SYM_STORAGE_CLASS_NULL      (  0u)  // No storage class
#define COFF_SYM_STORAGE_CLASS_AUTO      (  1u)  // Reserved
#define COFF_SYM_STORAGE_CLASS_EXT       (  2u)  // External definition (value=relocatable address)
#define COFF_SYM_STORAGE_CLASS_STAT      (  3u)  // Static (value=relocatable address)
#define COFF_SYM_STORAGE_CLASS_REG       (  4u)  // Reserved
#define COFF_SYM_STORAGE_CLASS_EXTREF    (  5u)  // External reference
#define COFF_SYM_STORAGE_CLASS_LABEL     (  6u)  // Label (value=relocatable address)
#define COFF_SYM_STORAGE_CLASS_ULABEL    (  7u)  // Undefined label
#define COFF_SYM_STORAGE_CLASS_MOS       (  8u)  // Reserved
#define COFF_SYM_STORAGE_CLASS_ARG       (  9u)  // Reserved
#define COFF_SYM_STORAGE_CLASS_STRTAG    ( 10u)  // Reserved
#define COFF_SYM_STORAGE_CLASS_MOU       ( 11u)  // Reserved
#define COFF_SYM_STORAGE_CLASS_UNTAG     ( 12u)  // Reserved
#define COFF_SYM_STORAGE_CLASS_TPDEF     ( 13u)  // Reserved
#define COFF_SYM_STORAGE_CLASS_USTATIC   ( 14u)  // Undefined static
#define COFF_SYM_STORAGE_CLASS_ENTAG     ( 15u)  // Reserved
#define COFF_SYM_STORAGE_CLASS_MOE       ( 16u)  // Reserved
#define COFF_SYM_STORAGE_CLASS_REGPARAM  ( 17u)  // Reserved
#define COFF_SYM_STORAGE_CLASS_FIELD     ( 18u)  // Reserved
#define COFF_SYM_STORAGE_CLASS_UEXT      ( 19u)  // Tentative external definition
#define COFF_SYM_STORAGE_CLASS_STATLAB   ( 20u)  // Static load time label
#define COFF_SYM_STORAGE_CLASS_EXTLAB    ( 21u)  // External load time label
#define COFF_SYM_STORAGE_CLASS_VARARG    ( 27u)  // Last declared parameter of a function with a variable number of arguments
#define COFF_SYM_STORAGE_CLASS_BLOCK     (100u)  // Reserved
#define COFF_SYM_STORAGE_CLASS_FCN       (101u)  // Reserved
#define COFF_SYM_STORAGE_CLASS_EOS       (102u)  // Reserved
#define COFF_SYM_STORAGE_CLASS_FILE      (103u)  // Reserved
#define COFF_SYM_STORAGE_CLASS_LINE      (104u)  // Used only by utility programs


typedef struct { // 18 bytes (same as coff_sym_entry_t)
   sU32 sec_length;
   sU16 num_reloc_entries;
   sU16 num_linenr_entries;
   sU32 pad1;
   sU32 pad2;
   sU16 pad3;
} coff_aux_entry_t;


coff_file_header_t coff_file_header;
coff_opt_header_t  coff_opt_header;
coff_sec_header_t *coff_sec_headers;

sBool b_coff_bios_5xx; // true if DSPImage uses DSPBIOS 5.xx

sU32 coff_cinit_addr;
sU32 coff_cinit_size;

sUI coff_max_raw_data_size;


/*
 * (todo) document me
 *
 */
S_EXTERN const char * S_CALL coff_strerror (coff_error_t _err);


/*
 * (todo) document me
 *
 */
S_EXTERN void * S_CALL coff_alloc (sUI _numBytes);


/*
 * (todo) document me
 *
 */
S_EXTERN void S_CALL coff_free (void *_ptr);


/*
 * (todo) document me
 *
 */
S_EXTERN coff_error_t S_CALL coff_init (void);


/*
 * (todo) document me
 *
 */
S_EXTERN coff_error_t S_CALL coff_open (const char *_pathName);


/*
 * (todo) document me
 *
 */
S_EXTERN coff_error_t S_CALL coff_read (void *_dest, sUI _numBytes);


/*
 * (todo) document me
 *
 */
S_EXTERN coff_error_t S_CALL coff_seek_abs (sUI _offset);


/*
 * (todo) document me
 *
 */
S_EXTERN void S_CALL coff_close (void);


/*
 * (todo) document me
 *
 */
S_EXTERN coff_error_t S_CALL coff_file_header_parse (void);


/*
 * (todo) document me
 *
 */
S_EXTERN coff_error_t S_CALL coff_opt_header_parse (void);


/*
 * (todo) document me
 *
 */
S_EXTERN coff_error_t S_CALL coff_sec_header_parse (coff_sec_header_t *_secHdr);


/*
 * (todo) document me
 *
 */
S_EXTERN coff_error_t S_CALL coff_string_table_read (void);


/*
 * (todo) document me
 *
 */
S_EXTERN const char * S_CALL coff_string_table_get (sUI _offset);


/*
 * (todo) document me
 *
 */
S_EXTERN void S_CALL coff_exit (void);


/*
 * Utility function to load overlay images.
 *
 *  Called by dsp_component_load()
 *
 */
S_EXTERN int S_CALL coff_load_overlay (const char *_fname, void *_virtAddr, sU32 _physAddr, sU32 _size);


#include "../include/cplusplus_end.h"

#endif /* __C64_TOOLS_COFF_H__ */
