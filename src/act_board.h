

#ifndef __ROM_ACT_BOARD_H
#define __ROM_ACT_BOARD_H

#include "merc.h"

/* Sub-routines and filters. */
void do_nread_next (CHAR_T *ch, char *argument, time_t *last_note);
void do_nread_number (CHAR_T *ch, char *argument, time_t *last_note,
    int number);

/* Commands. */
DECLARE_DO_FUN (do_nwrite);
DECLARE_DO_FUN (do_nread);
DECLARE_DO_FUN (do_nremove);
DECLARE_DO_FUN (do_nlist);
DECLARE_DO_FUN (do_ncatchup);
DECLARE_DO_FUN (do_note);
DECLARE_DO_FUN (do_board);

#endif
