

#ifndef __ROM_ACT_CONF_H
#define __ROM_ACT_CONF_H

#include "merc.h"

/* Sub-routines and filters. */
void do_colour_one (CHAR_T *ch, const COLOUR_SETTING_T *setting,
    const COLOUR_T *colour, bool use_default, char *buf);
void do_colour_codes (CHAR_T *ch, char *argument);

/* Commands. */
DECLARE_DO_FUN (do_scroll);
DECLARE_DO_FUN (do_colour);
DECLARE_DO_FUN (do_autolist);
DECLARE_DO_FUN (do_autoassist);
DECLARE_DO_FUN (do_autoexit);
DECLARE_DO_FUN (do_autogold);
DECLARE_DO_FUN (do_autoloot);
DECLARE_DO_FUN (do_autosac);
DECLARE_DO_FUN (do_autosplit);
DECLARE_DO_FUN (do_noloot);
DECLARE_DO_FUN (do_nofollow);
DECLARE_DO_FUN (do_telnetga);
DECLARE_DO_FUN (do_brief);
DECLARE_DO_FUN (do_compact);
DECLARE_DO_FUN (do_show_affects);
DECLARE_DO_FUN (do_combine);
DECLARE_DO_FUN (do_materials);
DECLARE_DO_FUN (do_nosummon);
DECLARE_DO_FUN (do_autoall);
DECLARE_DO_FUN (do_prompt);
DECLARE_DO_FUN (do_alia);
DECLARE_DO_FUN (do_alias);
DECLARE_DO_FUN (do_unalias);

#endif
