

/***************************************************************************
 *                                                                         *
 *  Based on MERC 2.2 MOBprograms by N'Atas-ha.                            *
 *  Written and adapted to ROM 2.4 by                                      *
 *          Markku Nylander (markku.nylander@uta.fi)                       *
 *                                                                         *
 ***************************************************************************/

#ifndef __ROM_MOB_CMDS_H
#define __ROM_MOB_CMDS_H

#include "merc.h"

/* the command table itself */
extern const struct mob_cmd_type mob_cmd_table[];

/* Function prototypes. */
void mob_interpret (CHAR_T *ch, char *argument);
char *mprog_type_to_name (flag_t type);

/* Command functions.
 * Defined in mob_cmds.c */
DECLARE_DO_FUN (do_mob);
DECLARE_DO_FUN (do_mpstat);
DECLARE_DO_FUN (do_mpdump);
DECLARE_DO_FUN (do_mpgecho);
DECLARE_DO_FUN (do_mpzecho);
DECLARE_DO_FUN (do_mpasound);
DECLARE_DO_FUN (do_mpkill);
DECLARE_DO_FUN (do_mpassist);
DECLARE_DO_FUN (do_mpjunk);
DECLARE_DO_FUN (do_mpechoaround);
DECLARE_DO_FUN (do_mpechoat);
DECLARE_DO_FUN (do_mpecho);
DECLARE_DO_FUN (do_mpmload);
DECLARE_DO_FUN (do_mpoload);
DECLARE_DO_FUN (do_mppurge);
DECLARE_DO_FUN (do_mpgoto);
DECLARE_DO_FUN (do_mpat);
DECLARE_DO_FUN (do_mptransfer);
DECLARE_DO_FUN (do_mpgtransfer);
DECLARE_DO_FUN (do_mpforce);
DECLARE_DO_FUN (do_mpgforce);
DECLARE_DO_FUN (do_mpvforce);
DECLARE_DO_FUN (do_mpcast);
DECLARE_DO_FUN (do_mpdamage);
DECLARE_DO_FUN (do_mpremember);
DECLARE_DO_FUN (do_mpforget);
DECLARE_DO_FUN (do_mpdelay);
DECLARE_DO_FUN (do_mpcancel);
DECLARE_DO_FUN (do_mpcall);
DECLARE_DO_FUN (do_mpflee);
DECLARE_DO_FUN (do_mpotransfer);
DECLARE_DO_FUN (do_mpremove);

#endif
