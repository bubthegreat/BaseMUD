

#ifndef __ROM_SAVE_H
#define __ROM_SAVE_H

#include "merc.h"

/* Function prototypes. */
void save_char_obj (CHAR_T *ch);
bool load_char_obj (DESCRIPTOR_T *d, char *name);
void load_old_colour (CHAR_T *ch, FILE *fp, char *name);

/* read functions. */
void fread_char (CHAR_T *ch, FILE *fp);
void fread_pet (CHAR_T *ch, FILE *fp);
void fread_obj (CHAR_T *ch, FILE *fp);

/* write functions. */
void fwrite_char (CHAR_T *ch, FILE *fp);
void fwrite_pet (CHAR_T *pet, FILE *fp);
void fwrite_obj (CHAR_T *ch, OBJ_T *obj, FILE *fp, int nest);

#endif
