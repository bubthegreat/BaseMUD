

#ifndef __ROM_ACT_SHOP_H
#define __ROM_ACT_SHOP_H

#include "merc.h"

/* Sub-routines and filters. */
bool do_filter_get_keeper (CHAR_T *ch, CHAR_T **out_keeper);
void do_buy_pet (CHAR_T *ch, char *argument);
void do_buy_item (CHAR_T *ch, char *argument);
void do_list_pets (CHAR_T *ch, char *argument);
void do_list_items (CHAR_T *ch, char *argument);

/* Commands. */
DECLARE_DO_FUN (do_buy);
DECLARE_DO_FUN (do_list);
DECLARE_DO_FUN (do_sell);
DECLARE_DO_FUN (do_value);
DECLARE_DO_FUN (do_heal);

#endif
