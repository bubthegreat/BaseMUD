

#ifndef __ROM_MEMORY_H
#define __ROM_MEMORY_H

#include "merc.h"

/* String functions. */
void string_space_init (void);
int string_space_dispose (void);
char *string_space_next (void);
void str_replace_dup (char **old, const char *str);
char *str_dup (const char *str);
void str_free (char **pstr);
char *str_register (const char *str);
char *str_get_registered (const char *str, unsigned int hash_index,
    unsigned int hash_value);
int str_get_strings_allocated (void);

/* Buffer functions. */
int buf_get_size (int val);
BUFFER_T *buf_new_size  (int size);
bool buf_cat (BUFFER_T *buffer, char *string);
void buf_clear (BUFFER_T *buffer);
char *buf_string (BUFFER_T *buffer);
void printf_to_buf (BUFFER_T *buffer, const char *fmt, ...);

/* General memory functions. */
void *mem_alloc (size_t size);
void mem_free (void *mem, size_t size);
void *mem_alloc_perm (size_t size);
int mem_pages_dispose (void);
char *mem_dump (char *eol);

#endif
