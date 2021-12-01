

#include "help.h"

void help_area_to_area (HELP_AREA_T *had, AREA_T *area) {
    LIST2_REASSIGN_BACK (had, area, area_prev, area_next,
        area, had_first, had_last);
}

void help_to_help_area (HELP_T *help, HELP_AREA_T *had) {
    LIST2_REASSIGN_BACK (help, had, had_prev, had_next,
        had, help_first, help_last);
}

int help_area_count_pages (HELP_AREA_T *had) {
    HELP_T *h;
    int count = 0;

    for (h = had->help_first; h != NULL; h = h->had_next)
        count++;
    return count;
}
