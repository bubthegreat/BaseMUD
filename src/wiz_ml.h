

#ifndef __ROM_WIZ_ML_H
#define __ROM_WIZ_ML_H

#include "merc.h"

/* Sub-routines and filters. */
void do_dump_stats (CHAR_T *ch);
void do_dump_world_raw (CHAR_T *ch);
void do_dump_world_json (CHAR_T *ch);
void do_jsave_area (CHAR_T *ch, const char *arg);
void do_jsave_help (CHAR_T *ch, const char *arg);
void do_jsave_table (CHAR_T *ch, const char *arg);

/* Commands. */
DECLARE_DO_FUN (do_advance);
DECLARE_DO_FUN (do_copyover);
DECLARE_DO_FUN (do_trust);
DECLARE_DO_FUN (do_dump);
DECLARE_DO_FUN (do_violate);
DECLARE_DO_FUN (do_qmconfig);
DECLARE_DO_FUN (do_jsave);

#endif
