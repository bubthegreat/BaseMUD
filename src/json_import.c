/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik Strfeldt, Tom Madsen, and Katja Nyboe.    *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
 *  ROM 2.4 is copyright 1993-1998 Russ Taylor                             *
 *  ROM has been brought to you by the ROM consortium                      *
 *      Russ Taylor (rtaylor@hypercube.org)                                *
 *      Gabrielle Taylor (gtaylor@hypercube.org)                           *
 *      Brian Moore (zump@rom.org)                                         *
 *  By using this code, you have agreed to follow the terms of the         *
 *  ROM license, in the file Rom24/doc/rom.license                         *
 ***************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "utils.h"
#include "recycle.h"
#include "db.h"
#include "lookup.h"
#include "portals.h"
#include "globals.h"
#include "memory.h"
#include "json_objr.h"
#include "help.h"

#include "json_import.h"

int json_import_objects (JSON_T *json) {
    const TABLE_T *table;
    JSON_T *sub;
    int count = 0;

    /* skip invalid import types. */
    if (json == NULL)
        return 0;
    if (json->type != JSON_OBJECT && json->type != JSON_ARRAY)
        return 0;

    /* if this is a named member, it might be eligable for importing. */
    if (json->type == JSON_OBJECT && json->parent &&
        json->parent->type == JSON_OBJECT && json->name != NULL)
    {
        /* skip certain objects. */
        if (strcmp (json->name, "flags") == 0)
            return 0;
        if (strcmp (json->name, "types") == 0)
            return 0;

        /* import! */
        /* TODO: this should be a table as well! */
        if (strcmp (json->name, "room") == 0)
            return json_objr_room (json) ? 1 : 0;
        if (strcmp (json->name, "mobile") == 0)
            return json_objr_mobile (json) ? 1 : 0;
        if (strcmp (json->name, "object") == 0)
            return json_objr_object (json) ? 1 : 0;
        if (strcmp (json->name, "area") == 0)
            return json_objr_area (json) ? 1 : 0;
        if (strcmp (json->name, "social") == 0)
            return json_objr_social (json) ? 1 : 0;
        if (strcmp (json->name, "portal") == 0)
            return json_objr_portal (json) ? 1 : 0;
        if (strcmp (json->name, "help") == 0) {
            HELP_AREA_T *had = json_objr_help_area (json);
            if (had == NULL)
                return 0;
            return 1 + help_area_count_pages (had);
        }
        if ((table = master_table_get_by_obj_name (json->name)) != NULL) {
            if (table->json_read_func)
                return (table->json_read_func (json, json->name)) ? 1 : 0;
            else
                return 0;
        }

        json_logf (json, "json_import_objects(): Unknown object type '%s'.",
            json->name);
        return 0;
    }

    /* recurse through arrays or objects. */
    for (sub = json->first_child; sub != NULL; sub = sub->next)
        if (sub->type == JSON_OBJECT || sub->type == JSON_ARRAY)
            count += json_import_objects (sub);

    /* return the total number of objects recursively imported. */
    return count;
}

bool json_import_expect (const char *type, const JSON_T *json, ...) {
    char *prop;
    va_list vargs;
    struct json_eprop *eprops = NULL, *eprops_back = NULL;
    struct json_eprop *eprop;
    bool success;
    JSON_T *jprop;

    /* get all of our supplied arguments. */
    va_start (vargs, json);
    eprops = NULL;
    eprops_back = NULL;

    success = TRUE;
    while (1) {
        if ((prop = va_arg (vargs, char *)) == NULL)
            break;

        eprop = calloc (1, sizeof (struct json_eprop));
        if (prop[0] == '*') {
            eprop->required = FALSE;
            eprop->name = strdup (prop + 1);
        }
        else {
            eprop->required = TRUE;
            eprop->name = strdup (prop);
        }

        LIST2_BACK (eprop, prev, next, eprops, eprops_back);
    }
    va_end (vargs);

    /* TODO: this algorithm could be much more optimized. */
    /* go through our expected properties and flag ones that are missing. */
    for (eprop = eprops; eprop != NULL; eprop = eprop->next) {
        for (jprop = json->first_child; jprop != NULL; jprop = jprop->next)
            if (strcmp (jprop->name, eprop->name) == 0)
                break;
        if (jprop == NULL && eprop->required) {
            json_logf (json, "json_import_expect(): Didn't find required "
                "property '%s' in object '%s'.\n", eprop->name, type);
            success = FALSE;
        }
    }

    for (jprop = json->first_child; jprop != NULL; jprop = jprop->next) {
        for (eprop = eprops; eprop != NULL; eprop = eprop->next)
            if (strcmp (jprop->name, eprop->name) == 0)
                break;
        if (eprop == NULL) {
            json_logf (json, "json_import_expect(): Found unexpected property "
                "'%s' in object '%s'.\n", jprop->name, type);
            success = FALSE;
        }
    }

    /* free allocated data. */
    while (eprops) {
        eprop = eprops;
        LIST2_REMOVE (eprop, prev, next, eprops, eprops_back);

        free (eprop->name);
        free (eprop);
    }

    return success;
}

char *json_string_append_newline (char *buf, size_t size) {
    int len;

    if (buf == NULL)
        return NULL;
    if ((len = strlen (buf)) + 1 >= size)
        return buf;

    buf[len + 0] = '\n';
    buf[len + 1] = '\r';
    buf[len + 2] = '\0';
    return buf;
}
