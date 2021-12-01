

#ifndef __ROM_RESETS_H
#define __ROM_RESETS_H

#include "merc.h"

/* Function prototypes. */
void reset_to_area (RESET_T *reset, AREA_T *area);
void reset_to_room (RESET_T *reset, ROOM_INDEX_T *room);
void reset_to_room_before (RESET_T *reset, ROOM_INDEX_T *room, int index);
void reset_run_all (ROOM_INDEX_T *room);
void reset_run (RESET_T *reset);
void reset_commit_all (void);
void reset_commit (RESET_T *reset);

#endif
