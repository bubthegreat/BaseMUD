

#ifndef __ROM_PLAYERS_H
#define __ROM_PLAYERS_H

#include "merc.h"

/* Function prototypes. */
bool player_has_clan (const CHAR_T *ch);
bool player_is_independent (const CHAR_T *ch);
bool player_in_same_clan (const CHAR_T *ch, const CHAR_T *victim);
void player_reset (CHAR_T *ch);
void player_reset_colour (CHAR_T *ch);
void player_set_title (CHAR_T *ch, char *title);
void player_advance_level (CHAR_T *ch, bool hide);
void player_gain_exp (CHAR_T *ch, int gain);
int player_get_exp_to_next_level (const CHAR_T *ch);
int player_get_exp_per_level (const CHAR_T *ch);
int player_get_exp_per_level_with_points (const CHAR_T *ch, int points);
void player_set_default_skills (CHAR_T *ch);
int player_get_skill_learned (const CHAR_T *ch, int sn);
void player_list_skills_and_groups (CHAR_T *ch, bool chosen);
void player_try_skill_improve (CHAR_T *ch, int sn, bool success,
    int multiplier);
void player_add_skill (CHAR_T *ch, int sn, bool deduct);
void player_remove_skill (CHAR_T *ch, int sn, bool refund);
void player_add_skill_group (CHAR_T *ch, int gn, bool deduct);
void player_remove_skill_group (CHAR_T *ch, int gn, bool refund);
void player_add_skill_or_group (CHAR_T *ch, const char *name, bool deduct);
void player_remove_skill_or_group (CHAR_T *ch, const char *name, bool refund);
bool player_is_undesirable (const CHAR_T *ch);
void player_die (CHAR_T *ch);
int player_get_weapon_skill (const CHAR_T *ch, int sn);
bool player_move_filter (CHAR_T *ch, ROOM_INDEX_T *from_room,
    ROOM_INDEX_T *to_room);
bool player_drop_weapon_if_too_heavy (CHAR_T *ch);
void player_change_conditions (CHAR_T *ch, int drunk, int full, int thirst,
    int hunger);
void player_change_condition (CHAR_T *ch, int cond, int value);
void player_update (CHAR_T *ch);

#endif
