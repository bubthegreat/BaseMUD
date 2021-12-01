

#ifndef __ROM_SPELL_AFF_H
#define __ROM_SPELL_AFF_H

#include "merc.h"

/* Filters and sub-routines. */
void spell_perform_curse_char (int sn, int level, CHAR_T *ch,
    CHAR_T *victim, bool quiet);
void spell_perform_blindness (int sn, int level, CHAR_T *ch,
    CHAR_T *victim, bool quet);

/* Spells. */
DECLARE_SPELL_FUN (spell_armor);
DECLARE_SPELL_FUN (spell_bless_object);
DECLARE_SPELL_FUN (spell_bless_char);
DECLARE_SPELL_FUN (spell_bless);
DECLARE_SPELL_FUN (spell_blindness);
DECLARE_SPELL_FUN (spell_blindness_quiet);
DECLARE_SPELL_FUN (spell_calm);
DECLARE_SPELL_FUN (spell_change_sex);
DECLARE_SPELL_FUN (spell_charm_person);
DECLARE_SPELL_FUN (spell_curse_object);
DECLARE_SPELL_FUN (spell_curse_char);
DECLARE_SPELL_FUN (spell_curse_char_quiet);
DECLARE_SPELL_FUN (spell_curse);
DECLARE_SPELL_FUN (spell_detect_evil);
DECLARE_SPELL_FUN (spell_detect_good);
DECLARE_SPELL_FUN (spell_detect_hidden);
DECLARE_SPELL_FUN (spell_detect_invis);
DECLARE_SPELL_FUN (spell_detect_magic);
DECLARE_SPELL_FUN (spell_enchant_armor);
DECLARE_SPELL_FUN (spell_enchant_weapon);
DECLARE_SPELL_FUN (spell_fireproof);
DECLARE_SPELL_FUN (spell_faerie_fire);
DECLARE_SPELL_FUN (spell_faerie_fog);
DECLARE_SPELL_FUN (spell_fly);
DECLARE_SPELL_FUN (spell_frenzy);
DECLARE_SPELL_FUN (spell_giant_strength);
DECLARE_SPELL_FUN (spell_haste);
DECLARE_SPELL_FUN (spell_infravision);
DECLARE_SPELL_FUN (spell_invis_object);
DECLARE_SPELL_FUN (spell_invis_char);
DECLARE_SPELL_FUN (spell_invis);
DECLARE_SPELL_FUN (spell_mass_invis);
DECLARE_SPELL_FUN (spell_pass_door);
DECLARE_SPELL_FUN (spell_plague);
DECLARE_SPELL_FUN (spell_poison_object);
DECLARE_SPELL_FUN (spell_poison_char);
DECLARE_SPELL_FUN (spell_poison);
DECLARE_SPELL_FUN (spell_protection_evil);
DECLARE_SPELL_FUN (spell_protection_good);
DECLARE_SPELL_FUN (spell_sanctuary);
DECLARE_SPELL_FUN (spell_shield);
DECLARE_SPELL_FUN (spell_sleep);
DECLARE_SPELL_FUN (spell_slow);
DECLARE_SPELL_FUN (spell_stone_skin);
DECLARE_SPELL_FUN (spell_weaken);

#endif
