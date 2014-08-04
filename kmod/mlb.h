/* ----
 * ---- file   : mlb.h
 * ---- author : Bastian Spiegel <bs@tkscript.de>
 * ---- legal  : (c) 2013 by Bastian Spiegel. 
 * ----          Distributed under terms of the GNU GENERAL PUBLIC LICENSE (GPL). See 
 * ----          http://www.gnu.org/licenses/licenses.html#GPL or COPYING for further information.
 * ----
 * ---- info   : C64 kernel module. This is part of the "c64_tools" package.
 * ----
 * ---- changed: 06Sep2013, 08Sep2013, 10Sep2013, 11Sep2013, 14Sep2013, 15Sep2013, 18Sep2013
 * ----          03Oct2013, 15Oct2013, 19Oct2013, 15Dec2013, 14Jan2014
 * ----
 * ----
 */

#ifndef __C64_TOOLS_KMOD_MLB_H__
#define __C64_TOOLS_KMOD_MLB_H__


/*
 * Initialize mailbox (power on, map L1DSRAM, install interrupt handler)
 *
 */
sBool mlb_init(void);


/*
 * Check whether a new message can be sent to the DSP
 *
 */
sBool mlb_is_ready_to_send (void);


/* 
 * Allow messages to be sent to DSP
 *
 */
void mlb_set_ready_to_send(void);


/*
 * Send message to DSP
 *
 *  (note) if _client is NULL, _msg is not being dereferenced (used for sending test-messages)
 *
 */
int mlb_send_to_dsp (void *_client, const dsp_msg_t *_msg);


/*
 * Read DSP-side private debug value
 *
 */
sU32 mlb_debug_read (void);


/*
 * Read DSP-side user debug value
 *
 */
sU32 mlb_debug_usr_read (sU32 _idx);


/*
 * Set DSP boot flag (see dsp_common.h:DSP_BOOTFLAG_xxx)
 *
 */
void mlb_bootflag_set (sU32 _flag);


/*
 * Query DSP boot flag (used for startup sync)
 *
 */
sU32 mlb_bootflag_get (void);


/*
 * Query return value of last DSP procedure call
 *
 *  (note) this return value is written to the dev.c:current_client 'dsp_errno'
 *          however, this is just for internal debugging purposes.
 *          Regular applications should use on of the two regular RPC return values
 *           to pass error codes from the DSP to the GPP.
 *
 */
sU32 mlb_dsp_errno (void);


/*
 * Cancel last fastcall (if any)
 *
 */
void mlb_fastcall_end (void);


/*
 * Shutdown mailbox, uninstall interrupt handler and unmap L1DSRAM
 *
 */
void mlb_exit (void);


#endif /* __C64_TOOLS_KMOD_MLB_H__ */
