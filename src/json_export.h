

#ifndef __ROM_JSON_EXPORT_H
#define __ROM_JSON_EXPORT_H

#include "merc.h"

/* Flags and modes for functions. */
#define JSON_EXPORT_MODE_SAVE           0
#define JSON_EXPORT_MODE_SAVE_AND_KEEP  1
#define JSON_EXPORT_MODE_ONLY_LOAD      2

#define JSON_EXPORT_OPTION_WRITE_INDIV  0x01
#define JSON_EXPORT_OPTION_UNLOAD       0x02

typedef void *   (*JSON_EXPORT_REC_NEXT_FUNC)  (const void *);
typedef JSON_T * (*JSON_EXPORT_REC_WRITE_FUNC) (const char *, const void *);

/* Function prototypes. */
void json_export_all (bool write_indiv, const char *everything);
void json_export_areas (int mode);
void json_export_socials (int mode);
void json_export_portals (int mode);
void json_export_tables (int mode);
void json_export_help_areas (int mode);
bool json_export_interpret_mode (int mode, flag_t *options_out);
void json_export_area (const AREA_T *area, int mode);
void json_export_recycleable (const char *objname, const char *filename,
    void *first,
    JSON_EXPORT_REC_NEXT_FUNC next_func,
    JSON_EXPORT_REC_WRITE_FUNC write_func,
    int mode);
bool json_can_export_table (const TABLE_T *table);
void json_export_table (const TABLE_T *table, int mode);
void json_export_help_area (const HELP_AREA_T *had, int mode);

#endif
