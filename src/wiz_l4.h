

#ifndef __ROM_WIZ_L4_H
#define __ROM_WIZ_L4_H

#include "merc.h"

/* Sub-routines and filters. */
void do_restore_single (CHAR_T *ch, CHAR_T *vch);
void do_flag_ext (CHAR_T *ch, CHAR_T *victim, char type,
    const EXT_FLAG_DEF_T *flag_table, EXT_FLAGS_T *flags, char *argument);
void do_flag_simple (CHAR_T *ch, CHAR_T *victim, char type,
    const FLAG_T *flag_table, flag_t *flags, char *argument);

/* Commands. */
DECLARE_DO_FUN (do_guild);
DECLARE_DO_FUN (do_sockets);
DECLARE_DO_FUN (do_flag);
DECLARE_DO_FUN (do_freeze);
DECLARE_DO_FUN (do_load);
DECLARE_DO_FUN (do_mload);
DECLARE_DO_FUN (do_oload);
DECLARE_DO_FUN (do_pecho);
DECLARE_DO_FUN (do_purge);
DECLARE_DO_FUN (do_restore);
DECLARE_DO_FUN (do_echo);
DECLARE_DO_FUN (do_vnum);
DECLARE_DO_FUN (do_slookup);
DECLARE_DO_FUN (do_mfind);
DECLARE_DO_FUN (do_ofind);
DECLARE_DO_FUN (do_zecho);

#endif
