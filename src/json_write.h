

#ifndef __ROM_JSON_WRITE_H
#define __ROM_JSON_WRITE_H

#include "merc.h"

/* Writing functions. */
const char *json_escaped_string (const char *value, int newline_pos);
void json_print_real (JSON_T *json, FILE *fp, int new_line);
void json_print (JSON_T *json, FILE *fp);
void json_fwrite (JSON_T *json, const char *filename);
int json_mkdir (const char *dir);
int json_mkdir_to (const char *filename);

#endif
