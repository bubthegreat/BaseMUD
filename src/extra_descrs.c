

#include "utils.h"

#include "extra_descrs.h"

void extra_descr_to_room_index_front (EXTRA_DESCR_T *ed, ROOM_INDEX_T *room) {
    ed->parent_type = EXTRA_DESCR_ROOM_INDEX;
    ed->parent = room;
    LIST2_FRONT (ed, on_prev, on_next,
        room->extra_descr_first, room->extra_descr_last);
}

void extra_descr_to_room_index_back (EXTRA_DESCR_T *ed, ROOM_INDEX_T *room) {
    ed->parent_type = EXTRA_DESCR_ROOM_INDEX;
    ed->parent = room;
    LIST2_BACK (ed, on_prev, on_next,
        room->extra_descr_first, room->extra_descr_last);
}

void extra_descr_to_obj_front (EXTRA_DESCR_T *ed, OBJ_T *obj) {
    ed->parent_type = EXTRA_DESCR_OBJ;
    ed->parent = obj;
    LIST2_FRONT (ed, on_prev, on_next,
        obj->extra_descr_first, obj->extra_descr_last);
}

void extra_descr_to_obj_back (EXTRA_DESCR_T *ed, OBJ_T *obj) {
    ed->parent_type = EXTRA_DESCR_OBJ;
    ed->parent = obj;
    LIST2_BACK (ed, on_prev, on_next,
        obj->extra_descr_first, obj->extra_descr_last);
}

void extra_descr_to_obj_index_back (EXTRA_DESCR_T *ed, OBJ_INDEX_T *obj_index) {
    ed->parent_type = EXTRA_DESCR_OBJ_INDEX;
    ed->parent = obj_index;
    LIST2_BACK (ed, on_prev, on_next,
        obj_index->extra_descr_first, obj_index->extra_descr_last);
}

void extra_descr_unlink (EXTRA_DESCR_T *ed) {
    switch (ed->parent_type) {
        case EXTRA_DESCR_ROOM_INDEX: {
            ROOM_INDEX_T *room = ed->parent;
            if (room != NULL) {
                ed->parent = NULL;
                LIST2_REMOVE (ed, on_prev, on_next,
                    room->extra_descr_first, room->extra_descr_last);
            }
            break;
        }

        case EXTRA_DESCR_OBJ: {
            OBJ_T *obj = ed->parent;
            if (obj != NULL) {
                ed->parent = NULL;
                LIST2_REMOVE (ed, on_prev, on_next,
                    obj->extra_descr_first, obj->extra_descr_last);
            }
            break;
        }

        case EXTRA_DESCR_OBJ_INDEX: {
            OBJ_INDEX_T *obj_index = ed->parent;
            if (obj_index != NULL) {
                ed->parent = NULL;
                LIST2_REMOVE (ed, on_prev, on_next,
                    obj_index->extra_descr_first, obj_index->extra_descr_last);
            }
            break;
        }

        default:
            ed->parent = NULL;
    }

    ed->parent_type = EXTRA_DESCR_NONE;
}

/* Get an extra description from a list. */
char *extra_descr_get_description (EXTRA_DESCR_T *list, const char *name) {
    for (; list != NULL; list = list->on_next)
        if (str_in_namelist ((char *) name, list->keyword))
            return list->description;
    return NULL;
}
