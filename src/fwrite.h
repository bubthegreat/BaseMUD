

#ifndef __ROM_FWRITE_H
#define __ROM_FWRITE_H

#include "merc.h"

/* Writing functions. */
char *fwrite_flags_static (const FLAG_T *table, flag_t flags);
char *fwrite_flags_buf (const FLAG_T *table, flag_t flags, char *buf);
char *fwrite_ext_flags_static (const EXT_FLAG_DEF_T *table, EXT_FLAGS_T flags);
char *fwrite_ext_flags_buf (const EXT_FLAG_DEF_T *table, EXT_FLAGS_T flags,
    char *buf);

#endif
