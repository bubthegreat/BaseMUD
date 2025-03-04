

#ifndef __ROM_JSON_OBJW_H
#define __ROM_JSON_OBJW_H

#include "merc.h"

/* node creation. */
JSON_T *json_objw_room (const char *name, const ROOM_INDEX_T *room);
JSON_T *json_objw_extra_descr (const char *name, const EXTRA_DESCR_T *ed);
JSON_T *json_objw_exit (const char *name, const ROOM_INDEX_T *from,
    const EXIT_T *ex);
JSON_T *json_objw_shop (const char *name, const SHOP_T *shop);
JSON_T *json_objw_mobile (const char *name, const MOB_INDEX_T *mob);
JSON_T *json_objw_object (const char *name, const OBJ_INDEX_T *obj);
JSON_T *json_objw_affect (const char *name, const AFFECT_T *aff);
JSON_T *json_objw_anum (const char *name, AREA_T *area_from, int vnum);
JSON_T *json_objw_reset (const char *name, const RESET_T *reset);
JSON_T *json_objw_shop (const char *name, const SHOP_T *shop);
JSON_T *json_objw_area (const char *name, const AREA_T *area);
JSON_T *json_objw_social (const char *name, const SOCIAL_T *soc);
JSON_T *json_objw_table (const char *name, const TABLE_T *table);
JSON_T *json_objw_portal_exit (const char *name, const PORTAL_EXIT_T *pex);
JSON_T *json_objw_portal (const char *name, const PORTAL_T *portal);
JSON_T *json_objw_help_area (const char *name, const HELP_AREA_T *had);
JSON_T *json_objw_help (const char *name, const HELP_T *help);

/* shorthand-functions for node creation as properties. */
JSON_T *json_prop_obj_room (JSON_T *parent, const char *name, const ROOM_INDEX_T *room);
JSON_T *json_prop_obj_extra_descr (JSON_T *parent, const char *name, const EXTRA_DESCR_T *ed);
JSON_T *json_prop_obj_exit (JSON_T *parent, const char *name, const ROOM_INDEX_T *from, const EXIT_T *ex);
JSON_T *json_prop_obj_shop (JSON_T *parent, const char *name, const SHOP_T *shop);
JSON_T *json_prop_obj_mobile (JSON_T *parent, const char *name, const MOB_INDEX_T *mob);
JSON_T *json_prop_obj_object (JSON_T *parent, const char *name, const OBJ_INDEX_T *obj);
JSON_T *json_prop_obj_affect (JSON_T *parent, const char *name, const AFFECT_T *aff);
JSON_T *json_prop_obj_anum (JSON_T *parent, const char *name, AREA_T *area_from, int vnum);
JSON_T *json_prop_obj_reset (JSON_T *parent, const char *name, const RESET_T *reset);
JSON_T *json_prop_obj_shop (JSON_T *parent, const char *name, const SHOP_T *shop);
JSON_T *json_prop_obj_area (JSON_T *parent, const char *name, const AREA_T *area);
JSON_T *json_prop_obj_social (JSON_T *parent, const char *name, const SOCIAL_T *soc);
JSON_T *json_prop_obj_table (JSON_T *parent, const char *name, const TABLE_T *table);
JSON_T *json_prop_obj_portal_exit (JSON_T *parent, const char *name, const PORTAL_EXIT_T *pex);
JSON_T *json_prop_obj_portal (JSON_T *parent, const char *name, const PORTAL_T *portal);
JSON_T *json_prop_obj_help_area (JSON_T *parent, const char *name, const HELP_AREA_T *had);
JSON_T *json_prop_obj_help (JSON_T *parent, const char *name, const HELP_T *help);

#endif
