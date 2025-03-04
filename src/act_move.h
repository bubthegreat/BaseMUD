

#ifndef __ROM_ACT_MOVE_H
#define __ROM_ACT_MOVE_H

#include "merc.h"

/* Sub-routines and filters. */
int do_door_filter_find (CHAR_T *ch, char *argument);
bool do_door_filter_is_door (CHAR_T *ch, EXIT_T *pexit, OBJ_T *obj,
    flag_t *out_flags, bool *out_container, int *out_key);
bool do_door_filter_can_open   (CHAR_T *ch, EXIT_T *pexit, OBJ_T *obj);
bool do_door_filter_can_close  (CHAR_T *ch, EXIT_T *pexit, OBJ_T *obj);
bool do_door_filter_can_lock   (CHAR_T *ch, EXIT_T *pexit, OBJ_T *obj);
bool do_door_filter_can_unlock (CHAR_T *ch, EXIT_T *pexit, OBJ_T *obj);
bool do_door_filter_can_pick   (CHAR_T *ch, EXIT_T *pexit, OBJ_T *obj);
void do_open_object   (CHAR_T *ch, OBJ_T *obj);
void do_close_object  (CHAR_T *ch, OBJ_T *obj);
void do_unlock_object (CHAR_T *ch, OBJ_T *obj);
void do_lock_object   (CHAR_T *ch, OBJ_T *obj);
void do_pick_object   (CHAR_T *ch, OBJ_T *obj);
void do_open_door     (CHAR_T *ch, int door);
void do_close_door    (CHAR_T *ch, int door);
void do_unlock_door   (CHAR_T *ch, int door);
void do_lock_door     (CHAR_T *ch, int door);
void do_pick_door     (CHAR_T *ch, int door);
void do_door (CHAR_T *ch, char *argument, char *arg_msg,
    void (*func_obj)  (CHAR_T *, OBJ_T *),
    void (*func_door) (CHAR_T *, int));
bool do_filter_change_position (CHAR_T *ch, int pos, char *same_msg);

/* Commands. */
DECLARE_DO_FUN (do_north);
DECLARE_DO_FUN (do_east);
DECLARE_DO_FUN (do_south);
DECLARE_DO_FUN (do_west);
DECLARE_DO_FUN (do_up);
DECLARE_DO_FUN (do_down);
DECLARE_DO_FUN (do_open);
DECLARE_DO_FUN (do_close);
DECLARE_DO_FUN (do_unlock);
DECLARE_DO_FUN (do_lock);
DECLARE_DO_FUN (do_pick);
DECLARE_DO_FUN (do_stand);
DECLARE_DO_FUN (do_rest);
DECLARE_DO_FUN (do_sit);
DECLARE_DO_FUN (do_sleep);
DECLARE_DO_FUN (do_wake);
DECLARE_DO_FUN (do_sneak);
DECLARE_DO_FUN (do_hide);
DECLARE_DO_FUN (do_visible);
DECLARE_DO_FUN (do_recall);
DECLARE_DO_FUN (do_enter);

#endif
