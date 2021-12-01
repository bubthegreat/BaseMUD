

#ifndef __ROM_ACT_GROUP_H
#define __ROM_ACT_GROUP_H

#include "merc.h"

/* Sub-routines and filters. */
void do_group_show (CHAR_T *ch);

/* Commands. */
DECLARE_DO_FUN (do_order_all);
DECLARE_DO_FUN (do_order);
DECLARE_DO_FUN (do_group);
DECLARE_DO_FUN (do_split);
DECLARE_DO_FUN (do_gtell);
DECLARE_DO_FUN (do_follow);

#endif
