

#ifndef __ROM_HELP_H
#define __ROM_HELP_H

#include "merc.h"

void help_area_to_area (HELP_AREA_T *had, AREA_T *area);
void help_to_help_area (HELP_T *help, HELP_AREA_T *had);
int help_area_count_pages (HELP_AREA_T *had);

#endif
