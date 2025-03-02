

#include <string.h>

#include "db.h"
#include "comm.h"
#include "interp.h"
#include "save.h"
#include "fight.h"
#include "ban.h"
#include "utils.h"
#include "act_player.h"
#include "wiz_l4.h"
#include "chars.h"
#include "find.h"
#include "descs.h"
#include "globals.h"

#include "wiz_l1.h"

DEFINE_DO_FUN (do_deny) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_T *victim;

    one_argument (argument, arg);
    BAIL_IF (arg[0] == '\0',
        "Deny whom?\n\r", ch);
    BAIL_IF ((victim = find_char_world (ch, arg)) == NULL,
        "They aren't here.\n\r", ch);
    BAIL_IF (IS_NPC (victim),
        "Not on NPC's.\n\r", ch);
    BAIL_IF (char_get_trust (victim) >= char_get_trust (ch),
        "You failed.\n\r", ch);

    EXT_SET (victim->ext_plr, PLR_DENY);
    printf_to_char(victim, "You have been denied access!\n\r");
    wiznetf (ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0,
        "$N denies access to %s", victim->name);
    printf_to_char(ch, "%s has been denied and will be escorted out.\n\r", victim->name);

    save_char_obj (victim);
    stop_fighting (victim, TRUE);
    do_function (victim, &do_quit, "");
}

DEFINE_DO_FUN (do_permban)
    { ban_site (ch, argument, TRUE); }

DEFINE_DO_FUN (do_protect) {
    CHAR_T *victim;

    BAIL_IF (argument[0] == '\0',
        "Protect whom from snooping?\n\r", ch);
    BAIL_IF ((victim = find_char_world (ch, argument)) == NULL,
        "You can't find them.\n\r", ch);

    if (IS_SET (victim->comm, COMM_SNOOP_PROOF)) {
        act_new ("$N is no longer snoop-proof.", ch, NULL, victim, TO_CHAR,
                 POS_DEAD);
        send_to_char ("Your snoop-proofing was just removed.\n\r", victim);
        REMOVE_BIT (victim->comm, COMM_SNOOP_PROOF);
    }
    else {
        act_new ("$N is now snoop-proof.", ch, NULL, victim, TO_CHAR,
                 POS_DEAD);
        send_to_char ("You are now immune to snooping.\n\r", victim);
        SET_BIT (victim->comm, COMM_SNOOP_PROOF);
    }
}

DEFINE_DO_FUN (do_reboo) {
    send_to_char ("If you want to REBOOT, spell it out.\n\r", ch);
}

DEFINE_DO_FUN (do_reboot) {
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_T *d, *d_next;
    CHAR_T *vch;

    if (ch->invis_level < LEVEL_HERO) {
        sprintf (buf, "Reboot by %s.", ch->name);
        do_function (ch, &do_echo, buf);
    }

    merc_down = TRUE;
    for (d = descriptor_first; d != NULL; d = d_next) {
        d_next = d->global_next;
        vch = d->original ? d->original : d->character;
        if (vch != NULL)
            save_char_obj (vch);
        close_socket (d);
    }
}

DEFINE_DO_FUN (do_shutdow) {
    send_to_char ("If you want to SHUTDOWN, spell it out.\n\r", ch);
}

DEFINE_DO_FUN (do_shutdown) {
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_T *d, *d_next;
    CHAR_T *vch;

    if (ch->invis_level < LEVEL_HERO)
        sprintf (buf, "Shutdown by %s.", ch->name);
    append_file (ch, SHUTDOWN_FILE, buf);
    strcat (buf, "\n\r");
    if (ch->invis_level < LEVEL_HERO)
        do_function (ch, &do_echo, buf);
    merc_down = TRUE;

    for (d = descriptor_first; d != NULL; d = d_next) {
        d_next = d->global_next;
        vch = d->original ? d->original : d->character;
        if (vch != NULL)
            save_char_obj (vch);
        close_socket (d);
    }
}

DEFINE_DO_FUN (do_log) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_T *victim;

    one_argument (argument, arg);
    BAIL_IF (arg[0] == '\0',
        "Log whom?\n\r", ch);

    if (!str_cmp (arg, "all")) {
        if (log_all_commands) {
            log_all_commands = FALSE;
            send_to_char ("Log ALL off.\n\r", ch);
        }
        else {
            log_all_commands = TRUE;
            send_to_char ("Log ALL on.\n\r", ch);
        }
        return;
    }

    BAIL_IF ((victim = find_char_world (ch, arg)) == NULL,
        "They aren't here.\n\r", ch);
    BAIL_IF (IS_NPC (victim),
        "Not on NPC's.\n\r", ch);

    /* No level check, gods can log anyone. */
    if (EXT_IS_SET (victim->ext_plr, PLR_LOG)) {
        EXT_UNSET (victim->ext_plr, PLR_LOG);
        send_to_char ("LOG removed.\n\r", ch);
    }
    else {
        EXT_SET (victim->ext_plr, PLR_LOG);
        send_to_char ("LOG set.\n\r", ch);
    }
}
