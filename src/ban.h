

#ifndef __ROM_BAN_H
#define __ROM_BAN_H

#include "merc.h"

/* Function prototypes. */
void ban_save_all (void);
void ban_load_all (void);
bool ban_check (char *site, int type);
void ban_site (CHAR_T *ch, char *argument, bool perm);

#endif
