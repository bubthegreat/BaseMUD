

#ifndef __ROM_JSON_IMPORT_H
#define __ROM_JSON_IMPORT_H

#include "merc.h"

struct json_eprop {
    char *name;
    bool required;
    struct json_eprop *prev, *next;
};

/* some useful(?) macros */
#define JGI(prop) JSON_GET_INT(json, prop)
#define JGS(prop) JSON_GET_STR(json, prop, buf)
#define JGB(prop) JSON_GET_BOOL(json, prop)

#define JGS_NL(prop) \
    (json_string_append_newline (JGS(prop), sizeof (buf)))

#define READ_PROP_STR(obj_prop, json_prop) \
    (JSON_GET_STR(json, (json_prop), (obj_prop)))
#define READ_PROP_STRP(obj_prop, json_prop) \
    (str_replace_dup (&(obj_prop), JGS (json_prop)))
#define READ_PROP_STRP_NL(obj_prop, json_prop) \
    (str_replace_dup (&(obj_prop), JGS_NL (json_prop)))
#define READ_PROP_INT(obj_prop, json_prop) \
    ((obj_prop) = JGI (json_prop))
#define READ_PROP_BOOL(obj_prop, json_prop) \
    ((obj_prop) = JGB (json_prop))
#define READ_PROP_FLAGS(obj_prop, json_prop, table) \
    ((obj_prop) = flags_from_string_exact (table, (JGS (json_prop), buf)))
#define READ_PROP_EXT_FLAGS(obj_prop, json_prop, table) \
    ((obj_prop) = ext_flags_from_string_exact (table, (JGS (json_prop), buf)))
#define READ_PROP_TYPE(obj_prop, json_prop, table) \
    ((obj_prop) = type_lookup_exact (table, (JGS (json_prop), buf)))

#define NO_NULL_STR(obj_prop) \
    do { \
        if ((obj_prop) == NULL) str_replace_dup (&(obj_prop), ""); \
    } while (0)

/* general import functions. */
int json_import_objects (JSON_T *json);
bool json_import_expect (const char *type, const JSON_T *json, ...);
char *json_string_append_newline (char *buf, size_t size);
void json_import_all (void);
AREA_T *json_import_link_areas_get_area (char **name);
void json_import_link_areas (void);

#endif
