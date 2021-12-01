

/* NOTE
 * ----
 * This file contains sub-routines for do_*() commands that can be shared
 * amongst themselves, as well as "filter" functions that perform a task
 * along with controlling code flow. There's a lot of miscellaneous stuff in
 * here that could probably be split into separate files if necessary,
 * but the overall goal here is to make commands shorter. If you notice
 * duplicate code between functions that can be extracted, do it here! If
 * you see code that belongs only to one function, but could be split into
 * separate functions (like ostat, mstat, rstat), split them into separate
 * do_() commands but leave them in their respective files.
 *    --- Synival */

#include "comm.h"
#include "chars.h"

#include "do_sub.h"

void do_autolist_flag (char *name, CHAR_T *ch, flag_t flags, flag_t flag) {
    char *msg = IS_SET (flags, flag) ? "{GON{x" : "{ROFF{x";
    do_autolist_flag_message (ch, name, msg);
}

void do_autolist_ext_flag (char *name, CHAR_T *ch, EXT_FLAGS_T flags,
    int bit)
{
    char *msg = EXT_IS_SET (flags, bit) ? "{GON{x" : "{ROFF{x";
    do_autolist_flag_message (ch, name, msg);
}

void do_autolist_flag_message (CHAR_T *ch, const char *name, const char *msg) {
    int padding = 15, i;
    for (i = 0; name[i] != '\0'; i++)
        if (name[i] == '{')
            padding += 2;
    printf_to_char (ch, "   %-*s%s\n\r", padding, name, msg);
}

void do_flag_toggle (CHAR_T *ch, int player_only, flag_t *flags,
    flag_t flag, char *off_msg, char *on_msg)
{
    BAIL_IF (player_only && IS_NPC (ch),
        "NPCs can't use player flags.\n\r", ch);

    if (IS_SET ((*flags), flag)) {
        send_to_char (off_msg, ch);
        REMOVE_BIT ((*flags), flag);
    }
    else {
        send_to_char (on_msg, ch);
        SET_BIT ((*flags), flag);
    }
}

void do_ext_flag_toggle (CHAR_T *ch, int player_only, EXT_FLAGS_T *flags,
    int bit, char *off_msg, char *on_msg)
{
    BAIL_IF (player_only && IS_NPC (ch),
        "NPCs can't use player flags.\n\r", ch);

    if (EXT_IS_SET (*flags, bit)) {
        send_to_char (off_msg, ch);
        EXT_UNSET (*flags, bit);
    }
    else {
        send_to_char (on_msg, ch);
        EXT_SET (*flags, bit);
    }
}

bool do_comm_toggle_channel_if_blank (CHAR_T *ch, char *argument,
    flag_t channel, char *message_on, char *message_off)
{
    if (argument[0] != '\0')
        return FALSE;
    do_flag_toggle (ch, FALSE, &(ch->comm), channel, message_on, message_off);
    return TRUE;
}
