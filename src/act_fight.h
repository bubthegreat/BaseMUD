

#ifndef __ROM_ACT_FIGHT_H
#define __ROM_ACT_FIGHT_H

#include "merc.h"

/* Sub-routines and filters. */
bool do_fight_filter_skill_target (CHAR_T *ch, const char *argument,
    int sn, flag_t npc_flag, const char *cant_msg, const char *self_msg,
    int *out_chance, CHAR_T **out_victim);

/* Commands. */
DECLARE_DO_FUN (do_berserk);
DECLARE_DO_FUN (do_bash);
DECLARE_DO_FUN (do_dirt);
DECLARE_DO_FUN (do_trip);
DECLARE_DO_FUN (do_kick);
DECLARE_DO_FUN (do_kill);
DECLARE_DO_FUN (do_murde);
DECLARE_DO_FUN (do_murder);
DECLARE_DO_FUN (do_backstab);
DECLARE_DO_FUN (do_flee);
DECLARE_DO_FUN (do_rescue);
DECLARE_DO_FUN (do_disarm);
DECLARE_DO_FUN (do_surrender);
DECLARE_DO_FUN (do_disengage);
DECLARE_DO_FUN (do_wimpy);
DECLARE_DO_FUN (do_consider);

#endif
