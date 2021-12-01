

#ifndef __ROM_FREAD_H
#define __ROM_FREAD_H

#include "merc.h"

/* Writing functions. */
char fread_letter (FILE *fp);
int fread_number (FILE *fp);
flag_t fread_flag (FILE *fp, const FLAG_T *table);
flag_t fread_flag_convert (char letter);
flag_t fread_flag_simple (FILE *fp);
flag_t fread_flag_brackets (FILE *fp, const FLAG_T *table);
EXT_FLAGS_T fread_ext_flag (FILE *fp, const EXT_FLAG_DEF_T *table);
EXT_FLAGS_T fread_ext_flag_brackets (FILE *fp, const EXT_FLAG_DEF_T *table);
void fread_bracket_value (FILE *fp, char *buf, size_t size);
char *fread_string_replace (FILE *fp, char **value);
char *fread_string_dup (FILE *fp);
char *fread_string_static (FILE *fp);
char *fread_string (FILE *fp, char *buf, size_t size);
char *fread_string_eol_replace (FILE *fp, char **value);
char *fread_string_eol_dup (FILE *fp);
char *fread_string_eol_static (FILE *fp);
char *fread_string_eol (FILE *fp, char *buf, size_t size);
void fread_to_eol (FILE *fp);
char *fread_word_replace (FILE *fp, char **value);
char *fread_word_dup (FILE *fp);
char *fread_word_static (FILE *fp);
char *fread_word (FILE *fp, char *buf, size_t size);
void fread_dice (FILE *fp, DICE_T *out);

#endif
