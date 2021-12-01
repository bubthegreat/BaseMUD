

#ifndef __ROM_MUSIC_H
#define __ROM_MUSIC_H

#include "merc.h"

/* Function prototypes. */
bool music_queue_values (flag_t *line, flag_t *song_num, flag_t *queue,
    int queue_max, int song);
bool music_update_values (flag_t *line, flag_t *song_num, flag_t *queue,
    int queue_max, int *line_out);
void music_update_global (void);
void music_update_object (OBJ_T *obj);
void music_load_songs (void);
void music_list_jukebox_songs (const OBJ_T *obj, CHAR_T *ch,
    const char *filter);

#endif
