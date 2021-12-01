

#include <string.h>

#include "save.h"

#include "fwrite.h"

/*
 * ROM OLC
 * Used in save_mobile and save_object below.  Writes
 * flags on the form fread_flag reads.
 *
 * *buf must hold at least 32+1 characters.
 *
 * -- Hugin
 */

char *fwrite_flags_static (const FLAG_T *table, flag_t flags) {
    static char buf[52];
    return fwrite_flags_buf (table, flags, buf);
}

char *fwrite_flags_buf (const FLAG_T *table, flag_t flags, char *buf) {
#ifndef BASEMUD_WRITE_FLAGS_OUT
    int count, pos = 0;
    for (count = 0; count < 32; count++) {
        if (IS_SET (flags, 1 << count)) {
            if (count < 26)
                buf[pos++] = 'A' + count;
            else
                buf[pos++] = 'a' + (count - 26);
        }
    }
    if (pos == 0)
        buf[pos++] = '0';
    buf[pos] = '\0';
#else
    sprintf (buf, "[%s]", flags_to_string_real (table, flags, ""));
#endif
    return buf;
}

char *fwrite_ext_flags_static (const EXT_FLAG_DEF_T *table, EXT_FLAGS_T flags) {
    static char buf[MAX_STRING_LENGTH];
    return fwrite_ext_flags_buf (table, flags, buf);
}

char *fwrite_ext_flags_buf (const EXT_FLAG_DEF_T *table, EXT_FLAGS_T flags,
    char *buf)
{
#ifndef BASEMUD_WRITE_EXT_FLAGS_OUT
    int count, pos = 0;
    for (count = 0; count < 32; count++) {
        if (EXT_IS_SET (flags, count)) {
            if (count < 26)
                buf[pos++] = 'A' + count;
            else
                buf[pos++] = 'a' + (count - 26);
        }
    }
    if (pos == 0)
        buf[pos++] = '0';
    buf[pos] = '\0';
#else
    sprintf (buf, "[%s]", ext_flags_to_string_real (table, flags, ""));
#endif
    return buf;
}
