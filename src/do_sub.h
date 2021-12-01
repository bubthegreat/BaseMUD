

#ifndef __ROM_DO_SUB_H
#define __ROM_DO_SUB_H

#include "merc.h"

void do_autolist_flag (char *name, CHAR_T *ch, flag_t flags, flag_t flag);
void do_autolist_ext_flag (char *name, CHAR_T *ch, EXT_FLAGS_T flags,
    int bit);
void do_autolist_flag_message (CHAR_T *ch, const char *name, const char *msg);

void do_flag_toggle (CHAR_T *ch, int player_only, flag_t *flags, flag_t flag,
    char *off_msg, char *on_msg);
void do_ext_flag_toggle (CHAR_T *ch, int player_only, EXT_FLAGS_T *flags,
    int bit, char *off_msg, char *on_msg);
bool do_comm_toggle_channel_if_blank (CHAR_T *ch, char *argument,
    flag_t channel, char *message_on, char *message_off);

#endif
