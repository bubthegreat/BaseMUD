

#ifndef __ROM_ACT_INFO_H
#define __ROM_ACT_INFO_H

#include "merc.h"

/* Sub-routines and filters. */
bool do_filter_blind (CHAR_T *ch);
void do_scan_list (ROOM_INDEX_T *scan_room, CHAR_T *ch,
    sh_int depth, sh_int door);
void do_scan_char (CHAR_T *victim, CHAR_T *ch, sh_int depth,
    sh_int door);
void do_scan_real (CHAR_T *ch, char *argument, int max_depth);
void do_look_room (CHAR_T *ch, int is_auto);
void do_look_in (CHAR_T *ch, char *arg);
void do_look_direction (CHAR_T *ch, int door);
bool do_filter_description_remove_line (CHAR_T *ch);
bool do_filter_description_append (CHAR_T *ch, char *argument);
bool do_filter_description_alter (CHAR_T *ch, char *argument);

/* Commands. */
DECLARE_DO_FUN (do_motd);
DECLARE_DO_FUN (do_rules);
DECLARE_DO_FUN (do_story);
DECLARE_DO_FUN (do_wizlist);
DECLARE_DO_FUN (do_credits);
DECLARE_DO_FUN (do_look);
DECLARE_DO_FUN (do_read);
DECLARE_DO_FUN (do_examine);
DECLARE_DO_FUN (do_lore);
DECLARE_DO_FUN (do_exits);
DECLARE_DO_FUN (do_worth);
DECLARE_DO_FUN (do_score);
DECLARE_DO_FUN (do_affects);
DECLARE_DO_FUN (do_time);
DECLARE_DO_FUN (do_weather);
DECLARE_DO_FUN (do_help);
DECLARE_DO_FUN (do_whois);
DECLARE_DO_FUN (do_who);
DECLARE_DO_FUN (do_count);
DECLARE_DO_FUN (do_inventory);
DECLARE_DO_FUN (do_equipment);
DECLARE_DO_FUN (do_compare);
DECLARE_DO_FUN (do_where);
DECLARE_DO_FUN (do_title);
DECLARE_DO_FUN (do_description);
DECLARE_DO_FUN (do_report);
DECLARE_DO_FUN (do_commands);
DECLARE_DO_FUN (do_areas);
DECLARE_DO_FUN (do_scan_short);
DECLARE_DO_FUN (do_scan_far);

#endif
