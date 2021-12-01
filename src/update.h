

#ifndef __ROM_UPDATE_H
#define __ROM_UPDATE_H

#include "merc.h"

/* Function prototypes. */
int recovery_in_position (int gain, int position);
int hit_gain (CHAR_T *ch, bool apply_learning);
int mana_gain (CHAR_T *ch, bool apply_learning);
int move_gain (CHAR_T *ch, bool apply_learning);
void song_update (void);
void weather_update (void);
void health_update (void);
void health_update_ch (CHAR_T *ch);
void health_update_ch_stat (CHAR_T *ch, sh_int *cur, sh_int *max,
    sh_int *rem, int (*func) (CHAR_T *, bool));
void aggr_update (void);
void violence_update (void);
void pulse_update (void);
void update_handler (void);

#endif
