

#ifndef __ROM_ROOMS_H
#define __ROM_ROOMS_H

#include "merc.h"

/* Is/can functions. */
bool room_is_dark (const ROOM_INDEX_T *room_index);
bool room_is_private (const ROOM_INDEX_T *room_index);
bool room_is_owner (const ROOM_INDEX_T *room, const CHAR_T *ch);

/* Get functions. */
char room_colour_char (const ROOM_INDEX_T *room);
EXIT_T *room_get_opposite_exit (const ROOM_INDEX_T *from_room, int dir,
    ROOM_INDEX_T **out_room);
char *room_get_door_name (const char *keyword, char *out_buf, size_t size);
OBJ_T *room_get_obj_of_type (const ROOM_INDEX_T *room, const CHAR_T *ch,
    int type);
OBJ_T *room_get_obj_with_condition (const ROOM_INDEX_T *room, const CHAR_T *ch,
    bool (*cond) (const OBJ_T *obj));
ROOM_INDEX_T *room_get_index (int vnum);
ROOM_INDEX_T *room_get_random_index (CHAR_T *ch);
EXIT_T *room_get_orig_exit (const ROOM_INDEX_T *room, int dir);

/* Action functions. */
void room_add_money (ROOM_INDEX_T *room, int gold, int silver);
void room_reset (ROOM_INDEX_T *room);
void room_reset_exits (ROOM_INDEX_T *room);
void room_to_area (ROOM_INDEX_T *room, AREA_T *area);
void room_index_to_hash (ROOM_INDEX_T *room);
void room_index_from_hash (ROOM_INDEX_T *room);
void room_link_exits_by_vnum_all (void);
void room_link_exits_by_vnum (ROOM_INDEX_T *room);
void room_fix_two_way_exits_all (void);
void room_fix_two_way_exits (ROOM_INDEX_T *room);
void room_fix_two_way_exit_doors (ROOM_INDEX_T *room_from, int dir_from,
                                  ROOM_INDEX_T *room_to,   int dir_to);
int room_check_resets_all (void);
int room_check_resets (ROOM_INDEX_T *room);
EXIT_T *room_create_exit (ROOM_INDEX_T *room_index, int dir);

/* Exit linking functions. */
void exit_to_room_index_from (EXIT_T *exit, ROOM_INDEX_T *room, int dir);
void exit_to_room_index_to (EXIT_T *exit, ROOM_INDEX_T *room);

#endif
