/* ----
 * ---- file   : msg.h
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the LESSER GNU GENERAL PUBLIC LICENSE (LGPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING_LGPL for further information.
 * ----
 * ---- info   : DSP main API. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 11Sep2013, 18Sep2013, 19Sep2013, 21Sep2013
 * ----
 * ----
 */

#ifndef __C64_TOOLS_MSG_H__
#define __C64_TOOLS_MSG_H__

#include "cplusplus_begin.h"


#define C64_COMPONENT_ID_LINK  (0u)

////#define C64_MSG_REPLY_OK  (0x900df00du)  // positive DSP reply (succeeded)
////#define C64_MSG_REPLY_NO  (0xd3adb33fu)  // negative DSP reply (failed)


/* Copy of DSP-side "mlb.h" utility macro */
#define DSP_COMPONENT_FC_CMD_DECLARE(c)  ((c) << 16u)


/*
 * A message/request exchanged between the GPP and the DSP.
 *
 *  Messages are currently placed in the RESET_VECTOR section directly 
 *   after the boot code (0x80 bytes).
 *
 *  Usually there is space for (0x1000 - 0x0080) = 0x0F80 => 0x0F80/0x10 = 0xF8 = 248 messages
 *
 *  (note) the DSP replies to a message by sending a reference to the request msg structure back
 *          to the GPP after (optionally) replacing the content of the data[] array with the
 *          return values (if any)
 *
 *  (note) for each message/request, the DSP _must_ send a reply (i.e. req/ack count is symmetrical)
 *
 */
//#ifndef CCS
#pragma pack(push, 2)
//#endif 

typedef struct { // 12 bytes
   union {
      struct {
         sU16 dsp_component_id;  // lo, 0xF000..0xFFFFu are reserved for c64tools package / link driver.
         sU16 dsp_cmd_id;        // hi, meaning depends on DSP component.
      } fields;

      sU32 comp;
   } req;

   /* User defined arguments or return values */
   union {
      sU32 arg;
      sU32 ret;
   } data[2];

} dsp_msg_t;


//#ifndef CCS
#pragma pack(pop)
//#endif


#define DSP_MSG_INIT_CORE(msg, compid, cmdid, arg1, arg2)          \
   (msg)->req.fields.dsp_component_id = COMPONENT_ ## compid;      \
        (msg)->req.fields.dsp_cmd_id = compid ## _ ## cmdid;       \
             (msg)->data[0].arg = arg1;                            \
                  (msg)->data[1].arg = arg2


#define DSP_MSG_INIT(msg, compid, cmdid, arg1, arg2)               \
   (msg)->req.fields.dsp_component_id = compid;                    \
        (msg)->req.fields.dsp_cmd_id = cmdid;                      \
             (msg)->data[0].arg = arg1;                            \
                  (msg)->data[1].arg = arg2


#include "cplusplus_end.h"

#endif /* C64_TOOLS_MSG_H__ */
