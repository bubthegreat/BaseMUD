

#ifndef __ROM_BOOT_H
#define __ROM_BOOT_H

#include "merc.h"

/* Platform-specific function prototypes. */
#if defined(macintosh) || defined(MSDOS)
    void game_loop_mac_msdos (void);
#endif

#if defined(unix)
    void game_loop_unix (int control);
#endif

/* Function prototypes. */
void copyover_recover (void);

#endif
