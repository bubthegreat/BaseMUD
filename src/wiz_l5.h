

#ifndef __ROM_WIZ_L5_H
#define __ROM_WIZ_L5_H

#include "merc.h"

/* Sub-routines and filters. */
bool do_obj_load_check (CHAR_T *ch, OBJ_T *obj);
void do_clone_recurse (CHAR_T *ch, OBJ_T *obj, OBJ_T *clone);

/* Commands. */
DECLARE_DO_FUN (do_nochannels);
DECLARE_DO_FUN (do_noemote);
DECLARE_DO_FUN (do_noshout);
DECLARE_DO_FUN (do_notell);
DECLARE_DO_FUN (do_transfer);
DECLARE_DO_FUN (do_peace);
DECLARE_DO_FUN (do_snoop);
DECLARE_DO_FUN (do_string);
DECLARE_DO_FUN (do_clone);
DECLARE_DO_FUN (do_newlock);

#endif
