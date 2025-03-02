

#include <stdlib.h>

#include "db.h"
#include "comm.h"
#include "interp.h"
#include "fight.h"
#include "utils.h"
#include "chars.h"
#include "find.h"
#include "descs.h"
#include "globals.h"

#include "wiz_l3.h"

DEFINE_DO_FUN (do_disconnect) {
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_T *d;
    CHAR_T *victim;

    one_argument (argument, arg);
    BAIL_IF (arg[0] == '\0',
        "Disconnect whom?\n\r", ch);

    if (is_number (arg)) {
        int desc;

        desc = atoi (arg);
        for (d = descriptor_first; d != NULL; d = d->global_next) {
            if (d->descriptor == desc) {
                close_socket (d);
                printf_to_char (ch, "Connection #%d has been disconnected.\n\r", d->descriptor);
                return;
            }
        }
    }

    BAIL_IF ((victim = find_char_world (ch, arg)) == NULL,
        "They aren't here.\n\r", ch);
    BAIL_IF_ACT (victim->desc == NULL,
        "$N doesn't have a descriptor.", ch, NULL, victim);

    for (d = descriptor_first; d != NULL; d = d->global_next) {
        if (d == victim->desc) {
            close_socket (d);
            printf_to_char(ch, "%s has been disconnected.\n\r", victim->name);
            return;
        }
    }

    bugf("do_disconnect: desc not found.");
    printf_to_char(ch, "Descriptor not found!\n\r");
}

DEFINE_DO_FUN (do_pardon) {
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_T *victim;

    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);

    BAIL_IF (arg1[0] == '\0' || arg2[0] == '\0',
        "Syntax: pardon <character> <killer|thief>.\n\r", ch);
    BAIL_IF ((victim = find_char_world (ch, arg1)) == NULL,
        "They aren't here.\n\r", ch);
    BAIL_IF (IS_NPC (victim),
        "Not on NPC's.\n\r", ch);

    if (!str_cmp (arg2, "killer")) {
        if (EXT_IS_SET (victim->ext_plr, PLR_KILLER)) {
            EXT_UNSET (victim->ext_plr, PLR_KILLER);
            send_to_char ("Killer flag removed.\n\r", ch);
            send_to_char ("You are no longer a KILLER.\n\r", victim);
        }
        return;
    }
    if (!str_cmp (arg2, "thief")) {
        if (EXT_IS_SET (victim->ext_plr, PLR_THIEF)) {
            EXT_UNSET (victim->ext_plr, PLR_THIEF);
            send_to_char ("Thief flag removed.\n\r", ch);
            send_to_char ("You are no longer a THIEF.\n\r", victim);
        }
        return;
    }

    send_to_char ("Syntax: pardon <character> <killer|thief>.\n\r", ch);
}

DEFINE_DO_FUN (do_sla)
    { send_to_char ("If you want to SLAY, spell it out.\n\r", ch); }

DEFINE_DO_FUN (do_slay) {
    CHAR_T *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument (argument, arg);
    BAIL_IF (arg[0] == '\0',
        "Slay whom?\n\r", ch);
    BAIL_IF ((victim = find_char_same_room (ch, arg)) == NULL,
        "They aren't here.\n\r", ch);
    BAIL_IF (ch == victim,
        "Suicide is a mortal sin.\n\r", ch);
    BAIL_IF (!IS_NPC (victim) && victim->level >= char_get_trust (ch),
        "You failed.\n\r", ch);

    act3 ("{1You slay $M in cold blood!{x",
          "{1$n slays you in cold blood!{x",
          "{1$n slays $N in cold blood!{x", ch, NULL, victim, 0, POS_RESTING);
    char_die (victim);
}
