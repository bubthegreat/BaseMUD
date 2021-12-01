

#ifndef __ROM_JSON_READ_H
#define __ROM_JSON_READ_H

#include "merc.h"

/* Data structures. */
struct json_read_t {
    const char *filename, *data, *pos;
    int line, col;
};

/* Read functions. */
JSON_T *json_read_directory_recursive (const char *path,
    int (*load_func) (JSON_T *), int *load_result);
void json_read_directory_real (JSON_T *obj, const char *path, bool recurse,
    int (*load_func) (JSON_T *), int *load_result);
JSON_T *json_read_file (const char *filename);
void json_read_logf (const JSON_READ_T *context, const char *format, ...);
JSON_T *json_read_object (JSON_READ_T *context, const char *name);
JSON_T *json_read_array (JSON_READ_T *context, const char *name);
JSON_T *json_read_any_type (JSON_READ_T *context, const char *name);
JSON_T *json_read_string (JSON_READ_T *context, const char *name);
char *json_read_string_content (JSON_READ_T *context, char *buf, size_t size);
JSON_T *json_read_number (JSON_READ_T *context, const char *name);
JSON_T *json_read_object (JSON_READ_T *context, const char *name);
JSON_T *json_read_array (JSON_READ_T *context, const char *name);
JSON_T *json_read_special (JSON_READ_T *context, const char *name);

#endif
