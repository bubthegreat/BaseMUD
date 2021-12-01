

#ifndef __ROM_UTILS_H
#define __ROM_UTILS_H

#include "merc.h"

/* String utilities. */
char *str_capitalized (const char *str);
void str_smash_char (char *str, char from, char to);
void str_smash_tilde (char *str);
void str_smash_dollar (char *str);
bool str_cmp (const char *astr, const char *bstr);
bool str_prefix (const char *astr, const char *bstr);
bool str_infix (const char *astr, const char *bstr);
bool str_suffix (const char *astr, const char *bstr);
const char *str_if_null (const char *str, const char *ifnull);
char *str_without_extension (char *input);
bool str_in_namelist (const char *str, const char *namelist);
bool str_in_namelist_exact (const char *str, const char *namelist);
size_t str_inject_args (char *buf, size_t size, const char *format, ...);
char *str_line (char ch, int chars);

/* Number utilities. */
int int_interpolate (int level, int value_00, int value_32);
int int_str_len (int num);

/* Random number utilities. */
void number_mm_init (void);
long number_mm (void);
int number_fuzzy (int number);
int number_range (int from, int to);
int number_percent (void);
int number_door (void);
int number_bits (int width);
int dice (int number, int size);

/* File utilities. */
void append_file (CHAR_T *ch, char *file, char *str);

/* Logging utilities. */
void bug (const char *str, int param);
void bugf (const char *fmt, ...);
void log_string (const char *str);
void log_f (const char *fmt, ...);
void tail_chain (void);

/* Misc. utilities. */
char *ctime_fixed (const time_t *timep);

#endif
