

#include <string.h>

#include "materials.h"

char *material_format_part (const MATERIAL_T *mat) {
#ifdef BASEMUD_MATERIALS_COMMAND
    static char buf[MAX_STRING_LENGTH];
    if (mat == NULL || mat->type == MATERIAL_GENERIC)
        return "";
    else {
        sprintf (buf, "[{%c%s{x] ", mat->color, mat->name);
        return buf;
    }
#else
    return "";
#endif
}

char *material_strcat (char *buf, const MATERIAL_T *mat) {
    return strcat (buf, material_format_part (mat));
}
