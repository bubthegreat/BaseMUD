

#ifndef __ROM_LOOKUP_H
#define __ROM_LOOKUP_H

#include "merc.h"

/* Looks up an element in any table by ensuring that 'table[n].ref' begins
 * with 'ref', then returns the index. */
#define SIMPLE_LOOKUP(table, ref, empty_val, max)                     \
    int i;                                                            \
    if ((ref) == NULL || (ref)[0] == '\0')                            \
        return empty_val;                                             \
    for (i = 0; (max <= 0 || i < max) && (table)[i].ref != NULL; i++) \
        if ((table)[i].ref != NULL && LOWER (ref[0]) == LOWER ((table)[i].ref[0]) \
            && !str_prefix (ref, (table)[i].ref))                     \
            return i;                                                 \
    return empty_val

/* Looks up an element in any table by ensuring that 'table[n].ref' begins
 * with 'ref', then returns the element. */
#define SIMPLE_GET_BY_NAME(table, ref, max)                           \
    int i;                                                            \
    if ((ref) == NULL || (ref)[0] == '\0')                            \
        return NULL;                                                  \
    for (i = 0; (max <= 0 || i < max) && (table)[i].ref != NULL; i++) \
        if ((table)[i].ref != NULL && LOWER (ref[0]) == LOWER ((table)[i].ref[0]) \
            && !str_prefix (ref, (table)[i].ref))                     \
            return ((table) + i);                                     \
    return NULL

/* Looks up an element in any table by ensuring that 'table[n].ref' begins
 * with 'ref', then returns property 'get'. */
#define SIMPLE_LOOKUP_PROP(table, get, ref, empty_val, max)           \
    int i;                                                            \
    if ((ref) == NULL || (ref)[0] == '\0')                            \
        return empty_val;                                             \
    for (i = 0; (max <= 0 || i < max) && (table)[i].ref != NULL; i++) \
        if ((table)[i].ref != NULL && LOWER (ref[0]) == LOWER ((table)[i].ref[0]) \
            && !str_prefix (ref, (table)[i].ref))                     \
            return (table)[i].get;                                    \
    return empty_val

/* Looks up an element in any table by ensuring that 'table[n].ref' begins
 * with 'ref', then returns the index. */
#define SIMPLE_LOOKUP_EXACT(table, ref, empty_val, max)               \
    int i;                                                            \
    if ((ref) == NULL || (ref)[0] == '\0')                            \
        return empty_val;                                             \
    for (i = 0; (max <= 0 || i < max) && (table)[i].ref != NULL; i++) \
        if ((table)[i].ref != NULL && !str_cmp (ref, (table)[i].ref)) \
            return i;                                                 \
    return empty_val

/* Looks up an element in any table by ensuring that 'table[n].ref' begins
 * with 'ref', then returns the element. */
#define SIMPLE_GET_BY_NAME_EXACT(table, ref, max)                     \
    int i;                                                            \
    if ((ref) == NULL || (ref)[0] == '\0')                            \
        return NULL;                                                  \
    for (i = 0; (max <= 0 || i < max) && (table)[i].ref != NULL; i++) \
        if ((table)[i].ref != NULL && !str_cmp (ref, (table)[i].ref)) \
            return ((table) + i);                                     \
    return NULL

/* Looks up an element in any table by ensuring that 'table[n].ref' begins
 * with 'ref', then returns property 'get'. */
#define SIMPLE_LOOKUP_PROP_EXACT(table, get, ref, empty_val, max)     \
    int i;                                                            \
    if ((ref) == NULL || (ref)[0] == '\0')                            \
        return empty_val;                                             \
    for (i = 0; (max <= 0 || i < max) && (table)[i].ref != NULL; i++) \
        if ((table)[i].ref != NULL && !str_cmp (ref, (table)[i].ref)) \
            return (table)[i].get;                                    \
    return empty_val

/* Looks up an element in any table by checking that 'table[n].check_prop' is
 * equal to 'check_val', then returns the element. */
#define SIMPLE_GET(table, ref, check_prop, check_val, max)            \
    int i;                                                            \
    for (i = 0; (max <= 0 || i < max) && (((table)[i]).check_prop) != (check_val); i++) \
        if (ref == ((table)[i].ref))                                  \
            return ((table) + i);                                     \
    return NULL

/* If 'element' exists, return 'element->name', otherwise return "unknown". */
#define SIMPLE_GET_NAME_FROM_ELEMENT(vtype, element, name) \
    const vtype *looked_up = element; \
    return (looked_up != NULL) ? looked_up->name : "unknown"

/* Defines a bundle of lookup functions for elements that start at zero,
 * end before 'max', and whose values can be referenced by array[n]. */
#define SIMPLE_INDEX_BUNDLE(btype, vtype, max)                      \
    static int btype ## _max_value = -1;                            \
    static int btype ## _get_max (void) {                           \
        if (btype ## _max_value != -1)                              \
            return btype ## _max_value;                             \
        else {                                                      \
            int i;                                                  \
            for (i = 0; i < (max); i++)                             \
                if (btype ## _table[i].name == NULL)                \
                    break;                                          \
            btype ## _max_value = i;                                \
            return i;                                               \
        }                                                           \
    }                                                               \
                                                                    \
    int btype ## _lookup (const char *name)                         \
        { SIMPLE_LOOKUP (btype ## _table, name, -1, max); }         \
    int btype ## _lookup_exact (const char *name)                   \
        { SIMPLE_LOOKUP_EXACT (btype ## _table, name, -1, max); }   \
    const vtype * btype ## _get_by_name (const char *name)          \
        { SIMPLE_GET_BY_NAME (btype ## _table, name, max); }        \
    const vtype * btype ## _get_by_name_exact (const char *name)    \
        { SIMPLE_GET_BY_NAME_EXACT (btype ## _table, name, max); }  \
    const vtype * btype ## _get (int type)                          \
        { return (type < 0 || type >= btype ## _get_max ())         \
            ? NULL : (btype ## _table + type); }                    \
    const char *btype ## _get_name (int type)                       \
        { return (type < 0 || type >= btype ## _get_max ())         \
            ? NULL : (btype ## _get)(type)->name; }

/* Defines a bundle of lookup functions for elements that may start or end
 * at any value and must be referenced by an internal property for lookup. */
#define SIMPLE_HASH_BUNDLE(btype, vtype, ref)                       \
    int btype ## _lookup (const char *name)                         \
        { SIMPLE_LOOKUP_PROP (btype ## _table, ref, name, -1, 0); } \
    int btype ## _lookup_exact (const char *name)                   \
        { SIMPLE_LOOKUP_PROP_EXACT (btype ## _table, ref, name, -1, 0); } \
    const vtype * btype ## _get_by_name (const char *name)          \
        { SIMPLE_GET_BY_NAME (btype ## _table, name, 0); }          \
    const vtype * btype ## _get_by_name_exact (const char *name)    \
        { SIMPLE_GET_BY_NAME_EXACT (btype ## _table, name, 0); }    \
    const vtype * btype ## _get (int ref)                           \
        { SIMPLE_GET (btype ## _table, ref, name, NULL, 0); }       \
    const char *btype ## _get_name (int ref)                        \
        { SIMPLE_GET_NAME_FROM_ELEMENT (vtype, btype ## _get (ref), name); }

/* Defines a bundle of lookup functions for elements specifically for
 * recycleable types. */
#define SIMPLE_REC_BUNDLE(btype, vtype, rtype)                             \
    vtype * btype ## _get_by_name (const char *name) {                     \
        const OBJ_RECYCLE_T *orec = btype ## _get_rec_by_name (name);      \
        return (orec == NULL) ? NULL : (vtype *) orec->obj;                \
    }                                                                      \
    const OBJ_RECYCLE_T *btype ## _get_rec_by_name (const char *name) {    \
        const char *oname;                                                 \
        const RECYCLE_T *rec;                                              \
        OBJ_RECYCLE_T *orec;                                               \
        rec = recycle_get (rtype);                                         \
        if (rec->obj_name_off < 0)                                         \
            return NULL;                                                   \
        for (orec = rec->list_first; orec != NULL; orec = orec->next) {    \
            oname = DEREF_OFFSET (char *, orec->obj, rec->obj_name_off);   \
            if (oname[0] == name[0] && !str_prefix (name, oname))          \
                return orec;                                               \
        }                                                                  \
        return NULL;                                                       \
    }                                                                      \
                                                                           \
    vtype * btype ## _get_by_name_exact (const char *name) {               \
        const OBJ_RECYCLE_T *orec = btype ## _get_rec_by_name_exact (name);\
        return (orec == NULL) ? NULL : (vtype *) orec->obj;                \
    }                                                                      \
    const OBJ_RECYCLE_T *btype ## _get_rec_by_name_exact (const char *name) {\
        const RECYCLE_T *rec;                                              \
        OBJ_RECYCLE_T *orec;                                               \
        rec = recycle_get (rtype);                                         \
        if (rec->obj_name_off < 0)                                         \
            return NULL;                                                   \
        for (orec = rec->list_first; orec != NULL; orec = orec->next)      \
            if (!str_cmp (name, DEREF_OFFSET (                             \
                    char *, orec->obj, rec->obj_name_off)))                \
                return orec;                                               \
        return NULL;                                                       \
    }

#define DEC_SIMPLE_INDEX_BUNDLE(btype, vtype)           \
    int           btype ## _lookup       (const char *name); \
    int           btype ## _lookup_exact (const char *name); \
    const vtype * btype ## _get_by_name  (const char *name); \
    const vtype * btype ## _get_by_name_exact (const char *name); \
    const vtype * btype ## _get          (int type);         \
    const char *btype ## _get_name     (int type)

#define DEC_SIMPLE_HASH_BUNDLE(btype, vtype)                 \
    int           btype ## _lookup       (const char *name); \
    int           btype ## _lookup_exact (const char *name); \
    const vtype * btype ## _get_by_name  (const char *name); \
    const vtype * btype ## _get_by_name_exact (const char *name); \
    const vtype * btype ## _get          (int type);         \
    const char *btype ## _get_name     (int type)

#define DEC_SIMPLE_REC_BUNDLE(btype, vtype)                                  \
    vtype *              btype ## _get_by_name           (const char *name); \
    const OBJ_RECYCLE_T *btype ## _get_rec_by_name       (const char *name); \
    vtype *              btype ## _get_by_name_exact     (const char *name); \
    const OBJ_RECYCLE_T *btype ## _get_rec_by_name_exact (const char *name)

/* Lookup bundles. */
DEC_SIMPLE_INDEX_BUNDLE (master,   TABLE_T);

DEC_SIMPLE_INDEX_BUNDLE (attack,   ATTACK_T);
DEC_SIMPLE_INDEX_BUNDLE (board,    BOARD_T);
DEC_SIMPLE_INDEX_BUNDLE (clan,     CLAN_T);
DEC_SIMPLE_INDEX_BUNDLE (class,    CLASS_T);
DEC_SIMPLE_INDEX_BUNDLE (liq,      LIQ_T);
DEC_SIMPLE_INDEX_BUNDLE (pc_race,  PC_RACE_T);
DEC_SIMPLE_INDEX_BUNDLE (race,     RACE_T);
DEC_SIMPLE_INDEX_BUNDLE (skill,    SKILL_T);
DEC_SIMPLE_INDEX_BUNDLE (skill_group, SKILL_GROUP_T);
DEC_SIMPLE_INDEX_BUNDLE (song,     SONG_T);
DEC_SIMPLE_INDEX_BUNDLE (spec,     SPEC_T);

DEC_SIMPLE_HASH_BUNDLE (affect_bit, AFFECT_BIT_T);
DEC_SIMPLE_HASH_BUNDLE (colour,     COLOUR_T);
DEC_SIMPLE_HASH_BUNDLE (colour_setting, COLOUR_SETTING_T);
DEC_SIMPLE_HASH_BUNDLE (cond,       COND_T);
DEC_SIMPLE_HASH_BUNDLE (dam,        DAM_T);
DEC_SIMPLE_HASH_BUNDLE (day,        DAY_T);
DEC_SIMPLE_HASH_BUNDLE (door,       DOOR_T);
DEC_SIMPLE_HASH_BUNDLE (effect,     EFFECT_T);
DEC_SIMPLE_HASH_BUNDLE (furniture,  FURNITURE_BITS_T);
DEC_SIMPLE_HASH_BUNDLE (item,       ITEM_T);
DEC_SIMPLE_HASH_BUNDLE (map_flags,  MAP_LOOKUP_TABLE_T);
DEC_SIMPLE_HASH_BUNDLE (map_lookup, MAP_LOOKUP_TABLE_T);
DEC_SIMPLE_HASH_BUNDLE (material,   MATERIAL_T);
DEC_SIMPLE_HASH_BUNDLE (month,      MONTH_T);
DEC_SIMPLE_HASH_BUNDLE (nanny,      NANNY_HANDLER_T);
DEC_SIMPLE_HASH_BUNDLE (position,   POSITION_T);
DEC_SIMPLE_HASH_BUNDLE (recycle,    RECYCLE_T);
DEC_SIMPLE_HASH_BUNDLE (sector,     SECTOR_T);
DEC_SIMPLE_HASH_BUNDLE (sex,        SEX_T);
DEC_SIMPLE_HASH_BUNDLE (size,       SIZE_T);
DEC_SIMPLE_HASH_BUNDLE (sky,        SKY_T);
DEC_SIMPLE_HASH_BUNDLE (sun,        SUN_T);
DEC_SIMPLE_HASH_BUNDLE (weapon,     WEAPON_T);
DEC_SIMPLE_HASH_BUNDLE (wear_loc,   WEAR_LOC_T);
DEC_SIMPLE_HASH_BUNDLE (wiznet,     WIZNET_T);

DEC_SIMPLE_REC_BUNDLE (area,        AREA_T);
DEC_SIMPLE_REC_BUNDLE (ban,         BAN_T);
DEC_SIMPLE_REC_BUNDLE (had,         HELP_AREA_T);
DEC_SIMPLE_REC_BUNDLE (help,        HELP_T);
DEC_SIMPLE_REC_BUNDLE (obj_index,   OBJ_INDEX_T);
DEC_SIMPLE_REC_BUNDLE (portal_exit, PORTAL_EXIT_T);
DEC_SIMPLE_REC_BUNDLE (room_index,  ROOM_INDEX_T);
DEC_SIMPLE_REC_BUNDLE (social,      SOCIAL_T);

/* General functions. */
int lookup_func_backup (LOOKUP_FUN *func, const char *str,
    const char *errf, int backup);

/* Table functions. */
bool table_is_of_type (const void *table, int type);
bool table_is_flags (const void *table);
bool table_is_types (const void *table);
bool table_is_unique (const void *table);

/* Special lookup functions. */
const TABLE_T *master_table_get_exact (const char *name);
const TABLE_T *master_table_get_by_obj_name (const char *name);

SPEC_FUN* spec_lookup_function (const char *name);
const char *spec_function_name (SPEC_FUN *function);

const OBJ_MAP_T       *obj_map_get       (int item_type);
const OBJ_MAP_VALUE_T *obj_map_value_get (const OBJ_MAP_T *map, int index);

const char *map_lookup_get_string (int index, flag_t value);
flag_t      map_lookup_get_type (int index, const char *str);
int         map_flags_get_string  (int index, flag_t value, char *buf, size_t size);
flag_t      map_flags_get_value (int index, const char *str);
const TABLE_T *master_get_first (void);
const TABLE_T *master_get_next (const TABLE_T *table);
AREA_T *area_get_by_vnum (int vnum);
AREA_T *area_get_by_filename_exact (const char *filename);
AREA_T *area_get_by_inner_vnum (int vnum);
flag_t wear_loc_get_flag (int wear_loc);
HELP_AREA_T *help_area_get_by_help (HELP_T *help);
HELP_AREA_T *help_area_get_by_filename_exact (const char *filename);
const DAY_T *day_get_current ();
const MONTH_T *month_get_current ();
const SKY_T *sky_get_current ();
const SUN_T *sun_get_current ();
const SKY_T *sky_get_by_mmhg (int mmhg);
const SUN_T *sun_get_by_hour (int hour);
int skill_get_index_by_slot (int slot);
const char *wiz_class_by_level (int level);
const STR_APP_T *str_app_get (int attr);
const INT_APP_T *int_app_get (int attr);
const WIS_APP_T *wis_app_get (int attr);
const DEX_APP_T *dex_app_get (int attr);
const CON_APP_T *con_app_get (int attr);
PORTAL_EXIT_T *portal_exit_lookup_exact (const char *name);
SOCIAL_T *social_lookup_exact (const char *name);
const HP_COND_T *hp_cond_get_for_char (const CHAR_T *ch);
const PC_RACE_T *pc_race_get_by_race (type_t race);

const char *ac_rating_phrase (int ac);
const char *align_name (int align);
const char *position_name (int position);

const char *room_bit_name (flag_t flags);
const char *affect_bit_name (flag_t flags);
const char *extra_bit_name (flag_t flags);
const char *mob_bit_name (EXT_FLAGS_T flags);
const char *plr_bit_name (EXT_FLAGS_T flags);
const char *comm_bit_name (flag_t flags);
const char *res_bit_name (flag_t flags);
const char *wear_flag_name (flag_t flags);
const char *form_bit_name (flag_t flags);
const char *part_bit_name (flag_t flags);
const char *weapon_bit_name (flag_t flags);
const char *cont_bit_name (flag_t flags);
const char *off_bit_name (flag_t flags);

const char *affect_apply_name (type_t type);
const char *sex_name (type_t sex);
const char *ac_type_name (type_t type);

#endif
