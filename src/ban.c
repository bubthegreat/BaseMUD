

#include <string.h>

#include "recycle.h"
#include "utils.h"
#include "fread.h"
#include "interp.h"
#include "comm.h"
#include "chars.h"
#include "globals.h"
#include "memory.h"
#include "fwrite.h"

#include "ban.h"

void ban_save_all (void) {
    BAN_T *pban;
    FILE *fp;
    bool found = FALSE;

    fclose (reserve_file);
    if ((fp = fopen (BAN_FILE, "w")) == NULL)
        perror (BAN_FILE);

    for (pban = ban_first; pban != NULL; pban = pban->global_next) {
        if (IS_SET (pban->ban_flags, BAN_PERMANENT)) {
            found = TRUE;
            fprintf (fp, "%-20s %-2d %s\n", pban->name, pban->level,
                     fwrite_flags_static (ban_flags, pban->ban_flags));
        }
    }

    fclose (fp);
    reserve_file = fopen (NULL_FILE, "r");
    if (!found)
        unlink (BAN_FILE);
}

void ban_load_all (void) {
    FILE *fp;

    if ((fp = fopen (BAN_FILE, "r")) == NULL)
        return;

    while (1) {
        BAN_T *pban;
        if (feof (fp)) {
            fclose (fp);
            return;
        }

        pban = ban_new ();
        str_replace_dup (&pban->name, fread_word_static (fp));
        pban->level = fread_number (fp);
        pban->ban_flags = fread_flag (fp, ban_flags);
        fread_to_eol (fp);

        LIST2_BACK (pban, global_prev, global_next, ban_first, ban_last);
    }
}

bool ban_check (char *site, int type) {
    BAN_T *pban;
    char host[MAX_STRING_LENGTH];

    strcpy (host, str_capitalized (site));
    host[0] = LOWER (host[0]);

    for (pban = ban_first; pban != NULL; pban = pban->global_next) {
        if (!IS_SET (pban->ban_flags, type))
            continue;

        if (IS_SET (pban->ban_flags, BAN_PREFIX)
            && IS_SET (pban->ban_flags, BAN_SUFFIX)
            && strstr (pban->name, host) != NULL)
            return TRUE;

        if (IS_SET (pban->ban_flags, BAN_PREFIX)
            && !str_suffix (pban->name, host))
            return TRUE;

        if (IS_SET (pban->ban_flags, BAN_SUFFIX)
            && !str_prefix (pban->name, host))
            return TRUE;
    }

    return FALSE;
}

void ban_site (CHAR_T *ch, char *argument, bool perm) {
    char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    char *name;
    BUFFER_T *buffer;
    BAN_T *pban, *pban_next;
    bool prefix = FALSE, suffix = FALSE;
    int type;

    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);

    if (arg1[0] == '\0') {
        BAIL_IF (ban_first == NULL,
            "No sites banned at this time.\n\r", ch);
        buffer = buf_new ();

        buf_cat (buffer, "Banned sites  level  type     status\n\r");
        for (pban = ban_first; pban != NULL; pban = pban->global_next) {
            sprintf (buf2, "%s%s%s",
                     IS_SET (pban->ban_flags, BAN_PREFIX) ? "*" : "",
                     pban->name,
                     IS_SET (pban->ban_flags, BAN_SUFFIX) ? "*" : "");
            sprintf (buf, "%-12s    %-3d  %-7s  %s\n\r",
                     buf2, pban->level,
                     IS_SET (pban->ban_flags, BAN_NEWBIES) ? "newbies" :
                     IS_SET (pban->ban_flags, BAN_PERMIT) ? "permit" :
                     IS_SET (pban->ban_flags, BAN_ALL) ? "all" : "",
                     IS_SET (pban->ban_flags,
                             BAN_PERMANENT) ? "perm" : "temp");
            buf_cat (buffer, buf);
        }

        page_to_char (buf_string (buffer), ch);
        buf_free (buffer);
        return;
    }

    /* find out what type of ban */
    if (arg2[0] == '\0' || !str_prefix (arg2, "all"))
        type = BAN_ALL;
    else if (!str_prefix (arg2, "newbies"))
        type = BAN_NEWBIES;
    else if (!str_prefix (arg2, "permit"))
        type = BAN_PERMIT;
    else {
        send_to_char
            ("Acceptable ban types are all, newbies, and permit.\n\r", ch);
        return;
    }

    name = arg1;

    if (name[0] == '*') {
        prefix = TRUE;
        name++;
    }
    if (name[strlen (name) - 1] == '*') {
        suffix = TRUE;
        name[strlen (name) - 1] = '\0';
    }
    BAIL_IF (strlen (name) == 0,
        "You have to ban SOMETHING.\n\r", ch);

    for (pban = ban_first; pban != NULL; pban = pban_next) {
        pban_next = pban->global_next;
        if (str_cmp (name, pban->name))
            continue;
        BAIL_IF (pban->level > char_get_trust (ch),
            "That ban was set by a higher power.\n\r", ch);
        ban_free (pban);
    }

    pban = ban_new ();
    pban->name = str_dup (name);
    pban->level = char_get_trust (ch);

    /* set ban type */
    pban->ban_flags = type;

    if (prefix)
        SET_BIT (pban->ban_flags, BAN_PREFIX);
    if (suffix)
        SET_BIT (pban->ban_flags, BAN_SUFFIX);
    if (perm)
        SET_BIT (pban->ban_flags, BAN_PERMANENT);

    LIST2_FRONT (pban, global_prev, global_next, ban_first, ban_last);
    ban_save_all ();

    printf_to_char (ch, "%s has been banned.\n\r", pban->name);
}
