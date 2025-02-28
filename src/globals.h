

#ifndef __ROM_GLOBALS_H
#define __ROM_GLOBALS_H

#include "merc.h"

extern HELP_T       *help_first,   *help_last;
extern SHOP_T       *shop_first,   *shop_last;
extern AREA_T       *area_first,   *area_last;
extern BAN_T        *ban_first,    *ban_last;
extern HELP_AREA_T  *had_first,    *had_last;
extern ANUM_T       *anum_first,   *anum_last;
extern MPROG_CODE_T *mpcode_first, *mpcode_last;
extern OBJ_T        *object_first, *object_last;
extern CHAR_T       *char_first,   *char_last;

extern int newmob_count;
extern int newobj_count;
extern int mobile_count;

extern char *help_greeting;
extern char log_buf[2 * MAX_INPUT_LENGTH];
extern KILL_T kill_table[MAX_LEVEL];
extern TIME_INFO_T time_info;
extern WEATHER_T weather_info;

extern MOB_INDEX_T  *mob_index_hash[MAX_KEY_HASH],
                    *mob_index_hash_back[MAX_KEY_HASH];
extern OBJ_INDEX_T  *obj_index_hash[MAX_KEY_HASH],
                    *obj_index_hash_back[MAX_KEY_HASH];
extern ROOM_INDEX_T *room_index_hash[MAX_KEY_HASH],
                    *room_index_hash_back[MAX_KEY_HASH];

extern bool in_boot_db;
extern bool in_game_loop;
extern FILE *current_area_file;
extern char current_area_filename[MAX_INPUT_LENGTH];

/* Socket and TCP/IP stuff. */
extern const char echo_off_str[];
extern const char echo_on_str[];
extern const char go_ahead_str[];

extern DESCRIPTOR_T *descriptor_first, *descriptor_last; /* All open descriptors */
extern DESCRIPTOR_T *d_next;          /* Next descriptor in loop  */
extern FILE *reserve_file;            /* Reserved file handle     */
extern bool god;                      /* All new chars are gods!  */
extern bool merc_down;                /* Shutdown         */
extern bool wizlock;                  /* Game is wizlocked        */
extern bool newlock;                  /* Game is newlocked        */
extern char str_boot_time[MAX_INPUT_LENGTH];
extern time_t current_time;           /* time of this pulse */
extern bool trigger_mobs;             /* act() switch */

extern int port, control;
extern int mud_ansiprompt, mud_ansicolor, mud_telnetga;
extern char *mud_ipaddress;

extern int find_last_count;
extern int find_next_count;

extern bool log_all_commands;

extern int top_vnum_room;
extern int top_vnum_mob;
extern int top_vnum_obj;

extern flag_t music_line;
extern flag_t music_song;
extern flag_t music_queue[MAX_SONG_GLOBAL];

extern char str_empty[1];
extern size_t new_buf_size;

#endif
