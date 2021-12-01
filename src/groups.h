

#ifndef __ROM_GROUPS_H
#define __ROM_GROUPS_H

#include "merc.h"

/* Function prototypes. */
bool is_same_group (const CHAR_T *ach, const CHAR_T *bch);
void add_follower (CHAR_T *ch, CHAR_T *master);
void stop_follower (CHAR_T *ch);
void nuke_pets (CHAR_T *ch);
void die_follower (CHAR_T *ch);

#endif
