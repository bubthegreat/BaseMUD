

#ifndef __ROM_MOBILES_H
#define __ROM_MOBILES_H

#include "merc.h"

/* Function prototypes. */
MOB_INDEX_T *mobile_get_index (int vnum);
CHAR_T *mobile_create (MOB_INDEX_T *mob_index);
void mobile_clone (CHAR_T *parent, CHAR_T *clone);
int mobile_should_assist_player (CHAR_T *bystander, CHAR_T *player,
    CHAR_T *victim);
bool mobile_should_assist_attacker (CHAR_T *bystander, CHAR_T *attacker,
    CHAR_T *victim);
void mobile_hit (CHAR_T *ch, CHAR_T *victim, int dt);
int mobile_get_skill_learned (const CHAR_T *ch, int sn);
bool mobile_is_friendly (const CHAR_T *ch);
void mobile_to_mob_index (CHAR_T *mob, MOB_INDEX_T *mob_index);
void mob_index_to_area (MOB_INDEX_T *mob, AREA_T *area);
void mobile_die (CHAR_T *ch);
int mobile_get_obj_cost (const CHAR_T *ch, const OBJ_T *obj, bool buy);
SHOP_T *mobile_get_shop (const CHAR_T *ch);
void mobile_update_all (void);
void mobile_update (CHAR_T *ch);
bool mobile_wander (CHAR_T *ch);
void mob_index_to_hash (MOB_INDEX_T *mob);
void mob_index_from_hash (MOB_INDEX_T *mob);
int mobile_get_weapon_skill (const CHAR_T *ch, int sn);

#endif
