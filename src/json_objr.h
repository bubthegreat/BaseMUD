

#ifndef __ROM_JSON_OBJR_H
#define __ROM_JSON_OBJR_H

#include "merc.h"

/* creating world objects from JSON objects. */
ROOM_INDEX_T *json_objr_room (const JSON_T *json);
EXTRA_DESCR_T *json_objr_extra_descr (const JSON_T *json);
EXIT_T *json_objr_exit (const JSON_T *json, ROOM_INDEX_T *room);
RESET_T *json_objr_reset (const JSON_T *json, ROOM_INDEX_T *room);
void json_objr_reset_values (const JSON_T *json, RESET_VALUE_T *v,
    char command, ROOM_INDEX_T *room);
SHOP_T *json_objr_shop (const JSON_T *json, const char *backup_area);
MOB_INDEX_T *json_objr_mobile (const JSON_T *json);
OBJ_INDEX_T *json_objr_object (const JSON_T *json);
void json_objr_object_values (const JSON_T *json, OBJ_INDEX_T *obj);
AREA_T *json_objr_area (const JSON_T *json);
SOCIAL_T *json_objr_social (const JSON_T *json);
PORTAL_T *json_objr_portal (const JSON_T *json);
HELP_AREA_T *json_objr_help_area (const JSON_T *json);
HELP_T *json_objr_help (const JSON_T *json);
AFFECT_T *json_objr_affect (JSON_T *json);
ANUM_T *json_objr_anum (const JSON_T *json, int type, sh_int *vnum_ptr,
    const char *backup_area);

#endif
