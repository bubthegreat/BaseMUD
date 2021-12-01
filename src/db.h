

#ifndef __ROM_DB_H
#define __ROM_DB_H

#include "merc.h"

/* macro for flag swapping */
#define GET_UNSET(flag1,flag2)    (~(flag1)&((flag1)|(flag2)))

/* Helper functions. */
void assign_area_vnum (int vnum, AREA_T *area);
bool check_pet_affected (int vnum, AFFECT_T *paf);
void db_dump_world (const char *filename);
int db_hash_from_vnum (int vnum);

/* World init functions. */
void boot_db (void);
void init_time_weather (void);
void init_areas (void);

/* Loading functions. */
void load_area (FILE *fp);
void load_area_olc (FILE *fp);
void load_area_finish (AREA_T *area);
void load_resets (FILE *fp);
void load_rooms (FILE *fp);
void load_shops (FILE *fp);
void load_specials (FILE *fp);
void load_socials (FILE *fp);
bool load_socials_string (FILE *fp, char **str);
void load_mobiles (FILE *fp);
void load_objects (FILE *fp);
void load_helps (FILE *fp, char *fname);
void load_mobprogs (FILE *fp);

/* Post-loading functions. */
void db_finalize_mob (MOB_INDEX_T *mob);
void db_finalize_obj (OBJ_INDEX_T *obj_index);
void db_register_new_room (ROOM_INDEX_T *room);
void db_register_new_mob (MOB_INDEX_T *mob);
void db_register_new_obj (OBJ_INDEX_T *obj);
void fix_two_way_exit_doors (ROOM_INDEX_T *room_from, int dir_from,
                             ROOM_INDEX_T *room_to,   int dir_to);
void fix_resets (void);
void fix_exits (void);
void fix_mobprogs (void);

/* Temporary objects used during loading. */
ANUM_T *anum_new (void);
void anum_free (ANUM_T *anum);

#endif
