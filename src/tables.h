

#ifndef __ROM_TABLES_H
#define __ROM_TABLES_H

#include "merc.h"

/* A table containing every table of every type. */
extern const TABLE_T          master_table[TABLE_MAX + 1];

/* All tables. */
extern const CLAN_T           clan_table[CLAN_MAX + 1];
extern const POSITION_T       position_table[POS_MAX + 1];
extern const SEX_T            sex_table[SEX_MAX + 1];
extern const SIZE_T           size_table[SIZE_MAX_R + 1];
extern const ITEM_T           item_table[ITEM_MAX + 1];
extern const DAM_T            dam_table[DAM_MAX + 1];
extern const ATTACK_T         attack_table[ATTACK_MAX + 1];
extern PC_RACE_T              pc_race_table[PC_RACE_MAX + 1];
extern const CLASS_T          class_table[CLASS_MAX + 1];
extern const STR_APP_T        str_app_table[ATTRIBUTE_HIGHEST + 2];
extern const INT_APP_T        int_app_table[ATTRIBUTE_HIGHEST + 2];
extern const WIS_APP_T        wis_app_table[ATTRIBUTE_HIGHEST + 2];
extern const DEX_APP_T        dex_app_table[ATTRIBUTE_HIGHEST + 2];
extern const CON_APP_T        con_app_table[ATTRIBUTE_HIGHEST + 2];
extern const LIQ_T            liq_table[LIQ_MAX + 1];
extern const SKILL_GROUP_T    skill_group_table[SKILL_GROUP_MAX + 1];
extern const SECTOR_T         sector_table[SECT_MAX + 1];
extern const DOOR_T           door_table[DIR_MAX + 1];
extern const SPEC_T           spec_table[SPEC_MAX + 1];
extern const WEAR_LOC_T       wear_loc_table[WEAR_LOC_MAX + 2];
extern const MATERIAL_T       material_table[MATERIAL_MAX + 1];
extern const COLOUR_SETTING_T colour_setting_table[COLOUR_SETTING_MAX + 1];
extern const DAY_T            day_table[DAY_MAX + 1];
extern const MONTH_T          month_table[MONTH_MAX + 1];
extern const SKY_T            sky_table[SKY_MAX + 1];
extern const SUN_T            sun_table[SUN_MAX + 1];
extern const HP_COND_T        hp_cond_table[HP_COND_MAX + 1];
extern const COLOUR_T         colour_table[COLOUR_MAX + 1];
extern const POSE_T           pose_table[CLASS_MAX + 1];
extern WEAPON_T               weapon_table[WEAPON_MAX + 1];
extern SKILL_T                skill_table[SKILL_MAX + 1];
extern BOARD_T                board_table[BOARD_MAX + 1];
extern SONG_T                 song_table[MAX_SONGS + 1];
extern RACE_T                 race_table[RACE_MAX + 1];

/* Internal tables. */
extern const AFFECT_BIT_T     affect_bit_table[AFF_TO_MAX + 1];
extern const EFFECT_T         effect_table[EFFECT_MAX + 1];
extern const FURNITURE_BITS_T furniture_table[POS_MAX + 1];
extern const MAP_LOOKUP_TABLE_T map_flags_table[MAP_LOOKUP_MAX + 1];
extern const MAP_LOOKUP_TABLE_T map_lookup_table[MAP_LOOKUP_MAX + 1];
extern const NANNY_HANDLER_T  nanny_table[NANNY_MAX + 1];
extern const OBJ_MAP_T        obj_map_table[ITEM_MAX + 1];
extern SKILL_MAP_T            skill_map_table[SKILL_MAP_MAX + 1];
extern RECYCLE_T              recycle_table[RECYCLE_MAX + 1];
extern const WIZNET_T         wiznet_table[WIZNET_MAX + 1];
extern const COND_T           cond_table[COND_MAX + 1];

/* Other tables. */
extern char *const title_table[CLASS_MAX][MAX_LEVEL + 1][2];

/* Table management functions. */
void table_dispose_all (void);
void table_dispose (const TABLE_T *table);

/* Init / disposal functions. */
DECLARE_DISPOSE_FUN (pc_race_dispose);
DECLARE_DISPOSE_FUN (race_dispose);
DECLARE_DISPOSE_FUN (song_dispose);

#endif
