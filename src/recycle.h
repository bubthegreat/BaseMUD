

#ifndef __ROM_RECYCLE_H
#define __ROM_RECYCLE_H

#include "merc.h"

/* Some handy macros. */
#define RECYCLE_BUNDLE(type, name, vtype)   \
    vtype * name ## _get_first (void)        \
        { return recycle_get_first_obj (type); } \
    vtype * name ## _get_next (const vtype * obj) \
        { return (obj->rec_data.next) ? obj->rec_data.next->obj : NULL; } \
    vtype * name ## _new (void)              \
        { return recycle_new (type); }       \
    void name ## _free (vtype *v)            \
        { recycle_free (type, v); }

#define DEC_RECYCLE_BUNDLE(name, vtype)           \
    vtype * name ## _new (void);                  \
    void name ## _free (vtype *);                 \
    vtype * name ## _get_first (void);            \
    vtype * name ## _get_next (const vtype * obj) \

/* Recycle globals and necessary functions. */
DEC_RECYCLE_BUNDLE (ban,         BAN_T);
DEC_RECYCLE_BUNDLE (area,        AREA_T);
DEC_RECYCLE_BUNDLE (extra_descr, EXTRA_DESCR_T);
DEC_RECYCLE_BUNDLE (exit,        EXIT_T);
DEC_RECYCLE_BUNDLE (room_index,  ROOM_INDEX_T);
DEC_RECYCLE_BUNDLE (obj_index,   OBJ_INDEX_T);
DEC_RECYCLE_BUNDLE (shop,        SHOP_T);
DEC_RECYCLE_BUNDLE (mob_index,   MOB_INDEX_T);
DEC_RECYCLE_BUNDLE (reset_data,  RESET_T);
DEC_RECYCLE_BUNDLE (help,        HELP_T);
DEC_RECYCLE_BUNDLE (mpcode,      MPROG_CODE_T);
DEC_RECYCLE_BUNDLE (descriptor,  DESCRIPTOR_T);
DEC_RECYCLE_BUNDLE (gen_data,    GEN_T);
DEC_RECYCLE_BUNDLE (affect,      AFFECT_T);
DEC_RECYCLE_BUNDLE (obj,         OBJ_T);
DEC_RECYCLE_BUNDLE (char,        CHAR_T);
DEC_RECYCLE_BUNDLE (pcdata,      PC_T);
DEC_RECYCLE_BUNDLE (mem_data,    MEM_T);
DEC_RECYCLE_BUNDLE (buf,         BUFFER_T);
DEC_RECYCLE_BUNDLE (mprog,       MPROG_LIST_T);
DEC_RECYCLE_BUNDLE (had,         HELP_AREA_T);
DEC_RECYCLE_BUNDLE (note,        NOTE_T);
DEC_RECYCLE_BUNDLE (social,      SOCIAL_T);
DEC_RECYCLE_BUNDLE (portal_exit, PORTAL_EXIT_T);
DEC_RECYCLE_BUNDLE (portal,      PORTAL_T);

/* Function prototypes (recycle operations). */
void *recycle_new (int type);
void recycle_free (int type, void *obj);
int recycle_free_all (void);
int recycle_free_all_type (int type);
void *recycle_get_first_obj (int type);

/* Initialization / disposal functions. */
DECLARE_INIT_FUN    (ban_init);
DECLARE_DISPOSE_FUN (ban_dispose);
DECLARE_INIT_FUN    (descriptor_init);
DECLARE_DISPOSE_FUN (descriptor_dispose);
DECLARE_INIT_FUN    (extra_descr_init);
DECLARE_DISPOSE_FUN (extra_descr_dispose);
DECLARE_DISPOSE_FUN (obj_dispose);
DECLARE_INIT_FUN    (char_init);
DECLARE_DISPOSE_FUN (char_dispose);
DECLARE_INIT_FUN    (pcdata_init);
DECLARE_DISPOSE_FUN (pcdata_dispose);
DECLARE_DISPOSE_FUN (buf_dispose);
DECLARE_INIT_FUN    (mprog_init);
DECLARE_DISPOSE_FUN (mprog_dispose);
DECLARE_DISPOSE_FUN (had_dispose);
DECLARE_DISPOSE_FUN (help_dispose);
DECLARE_INIT_FUN    (reset_data_init);
DECLARE_DISPOSE_FUN (reset_data_dispose);
DECLARE_INIT_FUN    (area_init);
DECLARE_DISPOSE_FUN (area_dispose);
DECLARE_INIT_FUN    (exit_init);
DECLARE_DISPOSE_FUN (exit_dispose);
DECLARE_INIT_FUN    (room_index_init);
DECLARE_DISPOSE_FUN (room_index_dispose);
DECLARE_INIT_FUN    (shop_init);
DECLARE_DISPOSE_FUN (shop_dispose);
DECLARE_INIT_FUN    (obj_index_init);
DECLARE_DISPOSE_FUN (obj_index_dispose);
DECLARE_INIT_FUN    (mob_index_init);
DECLARE_DISPOSE_FUN (mob_index_dispose);
DECLARE_INIT_FUN    (mpcode_init);
DECLARE_DISPOSE_FUN (mpcode_dispose);
DECLARE_INIT_FUN    (buf_init);
DECLARE_DISPOSE_FUN (note_dispose);
DECLARE_INIT_FUN    (social_init);
DECLARE_DISPOSE_FUN (social_dispose);
DECLARE_DISPOSE_FUN (portal_exit_dispose);
DECLARE_INIT_FUN    (portal_dispose);
DECLARE_DISPOSE_FUN (affect_dispose);

/* Functions related to specific recycleable objects. */
long get_pc_id (void);
long get_mob_id (void);

#endif
