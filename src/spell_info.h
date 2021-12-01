

#ifndef __ROM_SPELL_INFO_H
#define __ROM_SPELL_INFO_H

#include "merc.h"

/* Helper functions. */
long int spell_identify_seed (CHAR_T *ch, OBJ_T *obj);
int spell_identify_know_check (CHAR_T *ch, OBJ_T *obj, int pos,
    int skill, int *know_count);
void spell_identify_perform (CHAR_T *ch, OBJ_T *obj, int power);
void spell_identify_perform_seeded (CHAR_T *ch, OBJ_T *obj, int power);
const char *spell_identify_know_message (int percent);

/* Spell declarations. */
DECLARE_SPELL_FUN (spell_detect_poison);
DECLARE_SPELL_FUN (spell_identify);
DECLARE_SPELL_FUN (spell_know_alignment);
DECLARE_SPELL_FUN (spell_locate_object);
DECLARE_SPELL_FUN (spell_farsight);

#endif
