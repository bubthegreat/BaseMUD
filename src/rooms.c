/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik Strfeldt, Tom Madsen, and Katja Nyboe.    *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
 *  ROM 2.4 is copyright 1993-1998 Russ Taylor                             *
 *  ROM has been brought to you by the ROM consortium                      *
 *      Russ Taylor (rtaylor@hypercube.org)                                *
 *      Gabrielle Taylor (gtaylor@hypercube.org)                           *
 *      Brian Moore (zump@rom.org)                                         *
 *  By using this code, you have agreed to follow the terms of the         *
 *  ROM license, in the file Rom24/doc/rom.license                         *
 ***************************************************************************/

#include "db.h"
#include "lookup.h"
#include "utils.h"
#include "objs.h"
#include "interp.h"
#include "chars.h"
#include "globals.h"
#include "mobiles.h"
#include "items.h"
#include "resets.h"

#include "rooms.h"

/* Translates mob virtual number to its room index struct.
 * Hash table lookup. */
ROOM_INDEX_T *room_get_index (int vnum) {
    ROOM_INDEX_T *room_index;

    for (room_index = room_index_hash[vnum % MAX_KEY_HASH];
         room_index != NULL; room_index = room_index->hash_next)
        if (room_index->vnum == vnum)
            return room_index;

    /* NOTE: This did cause the server not to boot, but that seems a bit
     *       extreme. So we just return NULL instead, and keep on booting.
     *       -- Synival */
    if (in_boot_db) {
        bug ("room_get_index: bad vnum %d.", vnum);
     // exit (1);
    }

    return NULL;
}

/* random room generation procedure */
ROOM_INDEX_T *room_get_random_index (CHAR_T *ch) {
    ROOM_INDEX_T *room;

    while (1) {
        room = room_get_index (number_range (0, 65535));
        if (room == NULL)
            continue;
        if (!char_can_see_room (ch, room))
            continue;
        if (room_is_private (room))
            continue;
        if (IS_SET (room->room_flags, ROOM_PRIVATE))
            continue;
        if (IS_SET (room->room_flags, ROOM_SOLITARY))
            continue;
        if (IS_SET (room->room_flags, ROOM_SAFE))
            continue;
        if (IS_NPC (ch) || !IS_SET (room->room_flags, ROOM_LAW))
            break;
    }

    return room;
}

/* True if room is dark. */
bool room_is_dark (const ROOM_INDEX_T *room_index) {
    const SUN_T *sun;
    int sect;

    RETURN_IF_BUG (room_index == NULL,
        "room_is_dark(): room is NULL.", 0, FALSE);

    if (room_index->light > 0)
        return FALSE;
    if (IS_SET (room_index->room_flags, ROOM_DARK))
        return TRUE;

    sect = room_index->sector_type;
    sun = sun_get_current();
    if (sect == SECT_INSIDE || sect == SECT_CITY)
        return FALSE;
    if (sun->is_dark)
        return TRUE;

    return FALSE;
}

bool room_is_owner (const ROOM_INDEX_T *room, const CHAR_T *ch) {
    if (room->owner == NULL || room->owner[0] == '\0')
        return FALSE;
    return str_in_namelist (ch->name, room->owner);
}

/* True if room is private. */
bool room_is_private (const ROOM_INDEX_T *room_index) {
    CHAR_T *rch;
    int count;

    if (room_index->owner != NULL && room_index->owner[0] != '\0')
        return TRUE;

    count = 0;
    for (rch = room_index->people_first; rch != NULL; rch = rch->room_next)
        count++;
    if (IS_SET (room_index->room_flags, ROOM_PRIVATE) && count >= 2)
        return TRUE;
    if (IS_SET (room_index->room_flags, ROOM_SOLITARY) && count >= 1)
        return TRUE;
    if (IS_SET (room_index->room_flags, ROOM_IMP_ONLY))
        return TRUE;
    return FALSE;
}

char room_colour_char (const ROOM_INDEX_T *room) {
#ifdef BASEMUD_COLOR_ROOMS_BY_SECTOR
    int sect;
    const SECTOR_T *sect_data;

    if (room == NULL)
        return 's';

    sect = room->sector_type;
    sect_data = &(sector_table[UMIN (SECT_MAX, sect)]);

    if (sect_data == NULL)
        return 's';
    if (sect_data->colour_char == '\0')
        return 's';

    return sect_data->colour_char;
#else
    return 's';
#endif
}

EXIT_T *room_get_opposite_exit (const ROOM_INDEX_T *from_room, int dir,
    ROOM_INDEX_T **out_room)
{
    ROOM_INDEX_T *to_room;
    EXIT_T *pexit, *pexit_rev;;

    if (from_room == NULL)
        return NULL;
    if ((pexit = from_room->exit[dir]) == NULL)
        return NULL;
    if ((to_room = pexit->to_room) == NULL)
        return NULL;
    if ((pexit_rev = to_room->exit[REV_DIR(dir)]) == NULL)
        return NULL;
    if (pexit_rev->to_room != from_room)
        return NULL;
    if (out_room)
        *out_room = to_room;
    return pexit_rev;
}

char *room_get_door_name (const char *keyword, char *out_buf, size_t size) {
    if (keyword == NULL || keyword[0] == '\0')
        snprintf (out_buf, size, "door");
    else
        one_argument (keyword, out_buf);
    return out_buf;
}

void room_add_money (ROOM_INDEX_T *room, int gold, int silver) {
    OBJ_T *obj, *obj_next;
    for (obj = room->content_first; obj != NULL; obj = obj_next) {
        obj_next = obj->content_next;
        switch (obj->obj_index->vnum) {
            case OBJ_VNUM_SILVER_ONE:
                silver += 1;
                obj_extract (obj);
                break;

            case OBJ_VNUM_GOLD_ONE:
                gold += 1;
                obj_extract (obj);
                break;

            case OBJ_VNUM_SILVER_SOME:
                silver += obj->v.money.silver;
                obj_extract (obj);
                break;

            case OBJ_VNUM_GOLD_SOME:
                gold += obj->v.money.gold;
                obj_extract (obj);
                break;

            case OBJ_VNUM_COINS:
                silver += obj->v.money.silver;
                gold   += obj->v.money.gold;
                obj_extract (obj);
                break;
        }
    }
    obj_give_to_room (obj_create_money (gold, silver), room);
}

OBJ_T *room_get_obj_of_type (const ROOM_INDEX_T *room, const CHAR_T *ch,
    int type)
{
    OBJ_T *obj;
    for (obj = room->content_first; obj != NULL; obj = obj->content_next)
        if (obj->item_type == type && (!ch || char_can_see_obj (ch, obj)))
            return obj;
    return NULL;
}

OBJ_T *room_get_obj_with_condition (const ROOM_INDEX_T *room, const CHAR_T *ch,
    bool (*cond) (const OBJ_T *obj))
{
    OBJ_T *obj;
    for (obj = room->content_first; obj != NULL; obj = obj->content_next)
        if ((!ch || char_can_see_obj (ch, obj)) && cond (obj))
            return obj;
    return NULL;
}

/* OLC
 * Reset one room.  Called by reset_area and olc. */
void room_reset (ROOM_INDEX_T *room) {
    if (!room)
        return;
    room_reset_exits (room);
    reset_run_all (room);
}

void room_reset_exits (ROOM_INDEX_T *room) {
    EXIT_T *exit_obj, *exit_rev;
    int exit_n;

    for (exit_n = 0; exit_n < DIR_MAX; exit_n++) {
        if ((exit_obj = room->exit[exit_n]) == NULL)
            continue;
     /* if (IS_SET (exit_obj->exit_flags, EX_BASHED))
            continue; */
        exit_obj->exit_flags = exit_obj->rs_flags;

        if (exit_obj->to_room == NULL)
            continue;
        if ((exit_rev = exit_obj->to_room->exit[door_table[exit_n].reverse])
                == NULL)
            continue;
        exit_rev->exit_flags = exit_rev->rs_flags;
    }
}

void room_to_area (ROOM_INDEX_T *room, AREA_T *area) {
    LIST2_REASSIGN_BACK (
        room, area, area_prev, area_next,
        area, room_first, room_last);
}

void room_index_to_hash (ROOM_INDEX_T *room) {
    int hash = db_hash_from_vnum (room->vnum);
    LIST2_FRONT (room, hash_prev, hash_next,
        room_index_hash[hash], room_index_hash_back[hash]);
}

void room_index_from_hash (ROOM_INDEX_T *room) {
    int hash = db_hash_from_vnum (room->vnum);
    LIST2_REMOVE (room, hash_prev, hash_next,
        room_index_hash[hash], room_index_hash_back[hash]);
}
