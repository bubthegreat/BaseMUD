

#ifndef __ROM_AREAS_H
#define __ROM_AREAS_H

#include "merc.h"

/* Function prototypes */
void area_update_all (void);
void area_update (AREA_T *area);
bool area_should_update (const AREA_T *area);
void area_reset (AREA_T *area);
void area_reinsert_resets_in_room_order_all (void);
void area_reinsert_resets_in_room_order (AREA_T *area);

#endif
