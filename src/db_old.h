

#ifndef __ROM_DB_OLD_H
#define __ROM_DB_OLD_H

#include "merc.h"

/* Function prototypes. */
void load_old_mob (FILE *fp);
void load_old_obj (FILE *fp);
int convert_object_reset (RESET_T *reset);
void convert_objects (void);
void convert_mobile (MOB_INDEX_T *mob_index);

#endif
