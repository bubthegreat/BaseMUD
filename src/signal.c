

#include <signal.h>
#include <stdlib.h>

#include "utils.h"
#include "globals.h"

#include "signal.h"

void handle_sigint (int sig) {
    /* Try to cancel normally first. */
    if (!merc_down) {
        log_string ("Interrupt signal detected - closing game loop");
        merc_down = TRUE;
        return;
    }
    else {
        log_string ("Interrupt signal detected - killing");
        exit (128 + sig); /* UNIX convention. */
    }
}

void init_signal_handlers (void) {
    signal (SIGINT, handle_sigint);
}
