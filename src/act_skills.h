

#ifndef __ROM_ACT_SKILLS_H
#define __ROM_ACT_SKILLS_H

#include "merc.h"

/* Sub-routines and filters. */
void do_skills_or_spells (CHAR_T *ch, char *argument, int spells);

/* Commands. */
DECLARE_DO_FUN (do_gain);
DECLARE_DO_FUN (do_skills);
DECLARE_DO_FUN (do_spells);
DECLARE_DO_FUN (do_abilities);
DECLARE_DO_FUN (do_groups);
DECLARE_DO_FUN (do_train);
DECLARE_DO_FUN (do_practice);
DECLARE_DO_FUN (do_cast);

#endif
