

#ifndef __ROM_ACT_COMM_H
#define __ROM_ACT_COMM_H

#include "merc.h"

/* Sub-routines and filters. */
bool do_comm_filter_quiet (CHAR_T *ch);
bool do_comm_filter_nochannels (CHAR_T *ch);
bool do_comm_filter_quiet_nochannels (CHAR_T *ch);
bool do_comm_filter_emote (CHAR_T *ch, char *argument);
void do_comm_channel_global (CHAR_T *ch, char *argument, flag_t channel,
    char *message_on, char *message_off, char *act_self, char *act_world,
    int max_pos);
bool do_comm_filter_can_tell_or_reply (CHAR_T *ch);
void do_comm_tell_to_buffer (CHAR_T *ch, CHAR_T *victim, char *msg);
bool do_comm_filter_tell_target_can_receive_tells(CHAR_T *ch,
    CHAR_T *victim);
bool do_comm_filter_tell_not_now (CHAR_T *ch, CHAR_T *victim, char *msg);
void do_comm_tell_send_message (CHAR_T *ch, CHAR_T *victim, char *msg);
void do_comm_try_tell (CHAR_T *ch, CHAR_T *victim, char *msg);

/* Commands. */
DECLARE_DO_FUN (do_socials);
DECLARE_DO_FUN (do_channels);
DECLARE_DO_FUN (do_deaf);
DECLARE_DO_FUN (do_quiet);
DECLARE_DO_FUN (do_afk);
DECLARE_DO_FUN (do_replay);
DECLARE_DO_FUN (do_auction);
DECLARE_DO_FUN (do_gossip);
DECLARE_DO_FUN (do_grats);
DECLARE_DO_FUN (do_quote);
DECLARE_DO_FUN (do_question);
DECLARE_DO_FUN (do_answer);
DECLARE_DO_FUN (do_music);
DECLARE_DO_FUN (do_clantalk);
DECLARE_DO_FUN (do_say);
DECLARE_DO_FUN (do_shout);
DECLARE_DO_FUN (do_tell);
DECLARE_DO_FUN (do_reply);
DECLARE_DO_FUN (do_yell);
DECLARE_DO_FUN (do_emote);
DECLARE_DO_FUN (do_pmote);
DECLARE_DO_FUN (do_pose);
DECLARE_DO_FUN (do_bug);
DECLARE_DO_FUN (do_typo);

#endif
