

#ifndef __ROM_SPELL_MOVE_H
#define __ROM_SPELL_MOVE_H

#include "merc.h"

/* Sub-routines and filters. */
bool spell_filter_can_go_to (CHAR_T *ch, CHAR_T *victim, int level,
    flag_t res_type, flag_t dam_type);
bool spell_filter_use_warp_stone (CHAR_T *ch);
OBJ_T *spell_sub_create_portal (ROOM_INDEX_T *from_room, ROOM_INDEX_T *to_room,
    int duration);

/* Spells. */
DECLARE_SPELL_FUN (spell_gate);
DECLARE_SPELL_FUN (spell_summon);
DECLARE_SPELL_FUN (spell_teleport);
DECLARE_SPELL_FUN (spell_word_of_recall);
DECLARE_SPELL_FUN (spell_portal);
DECLARE_SPELL_FUN (spell_nexus);

#endif
