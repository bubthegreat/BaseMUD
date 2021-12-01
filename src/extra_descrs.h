

#ifndef __ROM_EXTRA_DESCRS_H
#define __ROM_EXTRA_DESCRS_H

#include "merc.h"

/* Link management */
void extra_descr_to_room_index_front (EXTRA_DESCR_T *ed, ROOM_INDEX_T *room);
void extra_descr_to_room_index_back (EXTRA_DESCR_T *ed, ROOM_INDEX_T *room);
void extra_descr_to_obj_front (EXTRA_DESCR_T *ed, OBJ_T *obj);
void extra_descr_to_obj_back (EXTRA_DESCR_T *ed, OBJ_T *obj);
void extra_descr_to_obj_index_back (EXTRA_DESCR_T *ed, OBJ_INDEX_T *obj_index);
void extra_descr_unlink (EXTRA_DESCR_T *ed);
char *extra_descr_get_description (EXTRA_DESCR_T *list, const char *name);

#endif
