

/*   QuickMUD - The Lazy Man's ROM - $Id: act_info.c,v 1.3 2000/12/01 10:48:33 ring0 Exp $ */

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "lookup.h"
#include "utils.h"
#include "groups.h"
#include "db.h"
#include "fight.h"
#include "update.h"
#include "comm.h"
#include "save.h"
#include "do_sub.h"
#include "act_comm.h"
#include "act_obj.h"
#include "chars.h"
#include "rooms.h"
#include "objs.h"
#include "find.h"
#include "spell_info.h"
#include "globals.h"
#include "memory.h"
#include "items.h"
#include "players.h"
#include "extra_descrs.h"

#include "act_info.h"

#define SCAN_ALL_DIRS -2

bool do_filter_blind (CHAR_T *ch) {
    if (!IS_NPC (ch) && EXT_IS_SET (ch->ext_plr, PLR_HOLYLIGHT))
        return FALSE;
    FILTER (IS_AFFECTED (ch, AFF_BLIND),
        "You can't see a thing!\n\r", ch);
    return FALSE;
}

void do_scan_list (ROOM_INDEX_T *scan_room, CHAR_T *ch,
    sh_int depth, sh_int door)
{
    CHAR_T *rch;

    if (scan_room == NULL)
        return;
    for (rch = scan_room->people_first; rch != NULL; rch = rch->room_next) {
        if (rch == ch)
            continue;
        if (!IS_NPC (rch) && rch->invis_level > char_get_trust (ch))
            continue;
        if (!char_can_see_anywhere (ch, rch))
            continue;
        do_scan_char (rch, ch, depth, door);
    }
}

static char *const scan_distance[8] = {
    "right here.",
    "nearby %s.",
    "not far %s.",
    "a bit far %s.",
    "far %s.",
    "very far %s.",
    "very, very far %s.",
    "extremely far %s.",
};

void do_scan_char (CHAR_T *victim, CHAR_T *ch, sh_int depth,
    sh_int door)
{
    const DOOR_T *door_obj;
    char buf[MAX_INPUT_LENGTH];

    door_obj = door_get (door);
    sprintf (buf, scan_distance[depth], door_obj->to_phrase);

    printf_to_char (ch, "%s, %s\n\r", PERS_AW (victim, ch), buf);
}

void do_scan_real (CHAR_T *ch, char *argument, int max_depth) {
    char arg1[MAX_INPUT_LENGTH], buf[MAX_INPUT_LENGTH];
    int min_depth, i;
    ROOM_INDEX_T *scan_room;
    EXIT_T *ex;
    sh_int door, depth;

    argument = one_argument (argument, arg1);
    if (arg1[0] == '\0') {
        min_depth = 0;
        door = SCAN_ALL_DIRS;
        send_to_char ("Looking around you see:\n\r", ch);
        act ("$n looks all around.", ch, NULL, NULL, TO_NOTCHAR);
    }
    else if ((door = door_lookup (arg1)) >= 0) {
        min_depth = 1;
        act2 ("You peer intently $T.", "$n peers intently $T.",
            ch, NULL, door_table[door].name, 0, POS_RESTING);
        sprintf (buf, "Looking %s you see:\n\r", door_table[door].name);
        max_depth *= 2;
    }
    else {
        send_to_char ("That's not a valid direction.\n\r", ch);
        return;
    }

    if (min_depth <= 0) {
        do_scan_list (ch->in_room, ch, 0, 0);
        min_depth = 1;
    }
    for (i = 0; i < DIR_MAX; i++) {
        if (door != SCAN_ALL_DIRS && door != i)
            continue;
        scan_room = ch->in_room;
        for (depth = 1; depth <= max_depth; depth++) {
            if (depth < min_depth)
                continue;
            if ((ex = scan_room->exit[i]) == NULL)
                break;
            if (IS_SET (ex->exit_flags, EX_CLOSED))
                break;
            if ((scan_room = ex->to_room) == NULL)
                break;
            if (!char_can_see_room (ch, scan_room))
                break;
            do_scan_list (scan_room, ch, depth, i);
        }
    }
}

void do_look_room (CHAR_T *ch, int is_auto) {
    char sect_char = room_colour_char (ch->in_room);
    printf_to_char (ch, "{%c%s{x", sect_char, ch->in_room->name);

    if ((IS_IMMORTAL (ch) && (IS_NPC (ch) || EXT_IS_SET (ch->ext_plr, PLR_HOLYLIGHT)))
        || IS_BUILDER (ch, ch->in_room->area))
        printf_to_char (ch, "{r [{RRoom %d{r]{x", ch->in_room->vnum);
    send_to_char ("\n\r", ch);

    if (!is_auto || (!IS_NPC (ch) && !IS_SET (ch->comm, COMM_BRIEF)))
        printf_to_char(ch, "  {S%s{x", ch->in_room->description);

    if (!IS_NPC (ch) && EXT_IS_SET (ch->ext_plr, PLR_AUTOEXIT))
        do_function (ch, &do_exits, "auto");

    obj_list_show_to_char (ch->in_room->content_first, ch, FALSE, FALSE);
    char_list_show_to_char (ch->in_room->people_first, ch);
}

void do_look_in (CHAR_T *ch, char *argument) {
    OBJ_T *obj;

    BAIL_IF (argument[0] == '\0',
        "Look in what?\n\r", ch);
    BAIL_IF ((obj = find_obj_here (ch, argument)) == NULL,
        "You do not see that here.\n\r", ch);
    BAIL_IF (!item_look_in (obj, ch),
        "That is not a container.\n\r", ch);
}

void do_look_direction (CHAR_T *ch, int door) {
    EXIT_T *pexit;

    BAIL_IF ((pexit = ch->in_room->exit[door]) == NULL,
        "Nothing special there.\n\r", ch);

    if (pexit->description != NULL && pexit->description[0] != '\0')
        send_to_char (pexit->description, ch);
    else
        send_to_char ("Nothing special there.\n\r", ch);

    if (pexit->keyword != NULL && pexit->keyword[0] != '\0' &&
        pexit->keyword[0] != ' ')
    {
        if (IS_SET (pexit->exit_flags, EX_CLOSED))
            act ("The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR);
        else if (IS_SET (pexit->exit_flags, EX_ISDOOR))
            act ("The $d is open.", ch, NULL, pexit->keyword, TO_CHAR);
    }
}

bool do_filter_description_remove_line (CHAR_T *ch) {
    char buf[MAX_STRING_LENGTH];
    int len;
    bool found = FALSE;

    FILTER (ch->description == NULL || ch->description[0] == '\0',
        "No lines left to remove.\n\r", ch);

    strcpy (buf, ch->description);
    for (len = strlen (buf); len > 0; len--) {
        if (buf[len] == '\r') {
            if (!found) { /* back it up */
                if (len > 0)
                    len--;
                found = TRUE;
            }
            else { /* found the second one */
                buf[len + 1] = '\0';
                str_free (&(ch->description));
                ch->description = str_dup (buf);
                send_to_char ("Your description is:\n\r", ch);
                send_to_char (ch->description ? ch->description :
                              "(None).\n\r", ch);
                return FALSE;
            }
        }
    }

    buf[0] = '\0';
    str_free (&(ch->description));
    ch->description = str_dup (buf);
    send_to_char ("Description cleared.\n\r", ch);
    return FALSE;
}

bool do_filter_description_append (CHAR_T *ch, char *argument) {
    char buf[MAX_STRING_LENGTH];
    buf[0] = '\0';

    if (argument[0] == '+') {
        if (ch->description != NULL)
            strcat (buf, ch->description);
        argument++;
        while (isspace (*argument))
            argument++;
    }
    FILTER (strlen (buf) >= 1024,
        "Description too long.\n\r", ch);

    strcat (buf, argument);
    strcat (buf, "\n\r");

    str_replace_dup (&ch->description, buf);
    return FALSE;
}

bool do_filter_description_alter (CHAR_T *ch, char *argument) {
    str_smash_tilde (argument);
    if (argument[0] == '-')
        return do_filter_description_remove_line (ch);
    else
        return do_filter_description_append (ch, argument);
}

/* RT Commands to replace news, motd, imotd, etc from ROM */
DEFINE_DO_FUN (do_motd)
    { do_function (ch, &do_help, "motd"); }
DEFINE_DO_FUN (do_rules)
    { do_function (ch, &do_help, "rules"); }
DEFINE_DO_FUN (do_story)
    { do_function (ch, &do_help, "story"); }
DEFINE_DO_FUN (do_wizlist)
    { do_function (ch, &do_help, "wizlist"); }

/* Not-RT(?) commands that are similar */
DEFINE_DO_FUN (do_credits)
    { do_function (ch, &do_help, "diku"); }

DEFINE_DO_FUN (do_look) {
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    CHAR_T *victim;
    OBJ_T *obj_list, *obj;
    char *pdesc1, *pdesc2;
    int i, door;
    int number, count;

    if (ch->desc == NULL)
        return;

    BAIL_IF (ch->position < POS_SLEEPING,
        "You can't see anything but stars!\n\r", ch);
    BAIL_IF (ch->position == POS_SLEEPING,
        "You can't see anything, you're sleeping!\n\r", ch);
    if (do_filter_blind (ch))
        return;

    if (!IS_NPC (ch) && !EXT_IS_SET (ch->ext_plr, PLR_HOLYLIGHT) &&
        room_is_dark (ch->in_room))
    {
        send_to_char ("{DIt is pitch black ... {x\n\r", ch);
        char_list_show_to_char (ch->in_room->people_first, ch);
        return;
    }

    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);
    number   = number_argument (arg1, arg3);
    count    = 0;

    /* "Auto" look shows the room. */
    if (arg1[0] == '\0' || !str_cmp (arg1, "auto")) {
        do_look_room (ch, arg1[0] != '\0');
        return;
    }

    /* Looking in something? */
    if (!str_cmp (arg1, "i") || !str_cmp (arg1, "in") || !str_cmp (arg1, "on")) {
        do_look_in (ch, arg2);
        return;
    }

    /* Looking at someone? */
    if ((victim = find_char_same_room (ch, arg1)) != NULL) {
        char_look_at_char (victim, ch);
        return;
    }

    #define CHECK_LOOK(cond, str, with_crlf) \
        if ((cond)) { \
            if (++count == number) { \
                send_to_char ((str), ch); \
                if ((with_crlf)) \
                    send_to_char ("\n\r", ch); \
                return; \
            } \
            continue; \
        }

    /* Looking at any obj extra descriptions? */
    for (i = 0; i < 2; i++) {
        switch (i) {
            case 0:  obj_list = ch->content_first;          break;
            case 1:  obj_list = ch->in_room->content_first; break;
            default: obj_list = NULL;
        }
        for (obj = obj_list; obj != NULL; obj = obj->content_next) {
            if (!char_can_see_obj (ch, obj))
                continue;
            pdesc1 = extra_descr_get_description (
                obj->extra_descr_first, arg3);
            pdesc2 = extra_descr_get_description (
                obj->obj_index->extra_descr_first, arg3);

            CHECK_LOOK (pdesc1 != NULL, pdesc1, FALSE);
            CHECK_LOOK (pdesc2 != NULL, pdesc2, FALSE);
            CHECK_LOOK (str_in_namelist (arg3, obj->name),
                        obj->description, TRUE);
        }
    }

    do {
        pdesc1 = extra_descr_get_description (
            ch->in_room->extra_descr_first, arg3);
        CHECK_LOOK (pdesc1 != NULL, pdesc1, FALSE);
    } while (0);

    /* Did we exceed the count? */
    if (count > 0 && count != number) {
        if (count == 1)
            printf_to_char (ch, "You only see one %s here.\n\r", arg3);
        else
            printf_to_char (ch, "You only see %d of those here.\n\r", count);
        return;
    }

    /* Check for a specific direction. */
    if ((door = door_lookup (arg1)) >= 0) {
        do_look_direction (ch, door);
        return;
    }

    /* All options failed. */
    send_to_char ("You do not see that here.\n\r", ch);
}

/* RT added back for the hell of it */
DEFINE_DO_FUN (do_read)
    { do_function (ch, &do_look, argument); }

DEFINE_DO_FUN (do_examine) {
    char arg[MAX_INPUT_LENGTH];
    OBJ_T *obj;

    DO_REQUIRE_ARG (arg, "Examine what?\n\r");

    do_function (ch, &do_look, arg);
    if ((obj = find_obj_here (ch, arg)) == NULL)
        return;

    item_examine (obj, ch);
}

DEFINE_DO_FUN (do_lore) {
    OBJ_T *obj;
    int skill = char_get_skill (ch, SN(LORE));

    BAIL_IF (skill == 0,
        "You haven't studied any lore.\n\r", ch);
    BAIL_IF (argument[0] == '\0',
        "Check the lore on what?\n\r", ch);
    BAIL_IF ((obj = find_obj_here (ch, argument)) == NULL,
        "You can't find that here.\n\r", ch);
    spell_identify_perform (ch, obj, skill);
}

/* Thanks to Zrin for auto-exit part. */
DEFINE_DO_FUN (do_exits) {
    char buf[MAX_STRING_LENGTH];
    bool auto_exits;
    int mode;

    auto_exits = !str_cmp (argument, "auto");
    if (do_filter_blind (ch))
        return;

    if (auto_exits)
        sprintf (buf, "{o[Exits: ");
    else if (IS_IMMORTAL (ch))
        sprintf (buf, "Obvious exits from room %d:\n\r", ch->in_room->vnum);
    else
        sprintf (buf, "Obvious exits:\n\r");
    send_to_char (buf, ch);

    mode = auto_exits ? EXITS_AUTO : EXITS_LONG;
    char_format_exit_string (ch, ch->in_room, mode, buf, sizeof (buf));
    if (auto_exits)
        printf_to_char (ch, "%s]{x\n\r", buf);
    else
        printf_to_char (ch, "%s", buf);
}

DEFINE_DO_FUN (do_worth) {
    if (IS_NPC (ch)) {
        printf_to_char (ch, "You have %ld gold and %ld silver.\n\r",
            ch->gold, ch->silver);
        return;
    }
    printf_to_char (ch,
        "You have %ld gold, %ld silver, and %d experience (%d exp to level).\n\r",
        ch->gold, ch->silver, ch->exp, player_get_exp_to_next_level(ch));
}

DEFINE_DO_FUN (do_score) {
    int i;

    printf_to_char (ch, "You are %s%s, level %d, %d years old (%d hours).\n\r",
        ch->name, IS_NPC (ch) ? "" : ch->pcdata->title, ch->level,
        char_get_age (ch), (ch->played + (int) (current_time - ch->logon)) / 3600);

    if (char_get_trust (ch) != ch->level)
        printf_to_char (ch, "You are trusted at level %d.\n\r",
            char_get_trust (ch));

    printf_to_char (ch, "Race: %s  Sex: %s  Class: %s\n\r",
        race_get_name (ch->race), sex_name (ch->sex), char_get_class_name (ch));

    printf_to_char (ch, "You have %d/%d hit, %d/%d mana, %d/%d movement.\n\r",
        ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move);

#ifdef BASEMUD_SHOW_RECOVERY_RATE
    printf_to_char (ch, "Your recovery rates are %+d hit, %+d mana, %+d movement.\n\r",
        hit_gain(ch, FALSE), mana_gain(ch, FALSE), move_gain(ch, FALSE));
#endif

    printf_to_char (ch, "You have %d practices and %d training sessions.\n\r",
        ch->practice, ch->train);

    printf_to_char (ch, "You are carrying %d/%d items with weight %ld/%d pounds.\n\r",
        ch->carry_number, char_get_max_carry_count (ch),
        char_get_carry_weight (ch) / 10, char_get_max_carry_weight (ch) / 10);

    printf_to_char (ch, "Str: %d(%d)  Int: %d(%d)  Wis: %d(%d)  Dex: %d(%d)  Con: %d(%d)\n\r",
        ch->perm_stat[STAT_STR], char_get_curr_stat (ch, STAT_STR),
        ch->perm_stat[STAT_INT], char_get_curr_stat (ch, STAT_INT),
        ch->perm_stat[STAT_WIS], char_get_curr_stat (ch, STAT_WIS),
        ch->perm_stat[STAT_DEX], char_get_curr_stat (ch, STAT_DEX),
        ch->perm_stat[STAT_CON], char_get_curr_stat (ch, STAT_CON));

    printf_to_char (ch, "You have scored %d exp, and have %ld gold and %ld silver coins.\n\r",
        ch->exp, ch->gold, ch->silver);

    /* RT shows exp to level */
    if (!IS_NPC (ch) && ch->level < LEVEL_HERO)
        printf_to_char (ch, "You need %d exp to level.\n\r",
            player_get_exp_to_next_level (ch));

    printf_to_char (ch, "Wimpy set to %d hit points.\n\r", ch->wimpy);

    if (IS_DRUNK (ch))
        send_to_char ("You are drunk.\n\r", ch);
    if (IS_THIRSTY (ch))
        send_to_char ("You are thirsty.\n\r", ch);
    if (IS_HUNGRY (ch))
        send_to_char ("You are hungry.\n\r", ch);

    printf_to_char (ch, "You are %s\r\n",
        char_get_position_str (ch, ch->position, ch->on, TRUE));

    /* print attack information */
    if (ch->level >= 15)
        printf_to_char (ch, "Hitroll: %d  Damroll: %d.\n\r",
            GET_HITROLL (ch), GET_DAMROLL (ch));

    /* print AC values */
    if (ch->level >= 25)
        printf_to_char (ch, "Armor: pierce: %d  bash: %d  slash: %d  magic: %d\n\r",
            GET_AC (ch, AC_PIERCE), GET_AC (ch, AC_BASH),
            GET_AC (ch, AC_SLASH), GET_AC (ch, AC_EXOTIC));

    for (i = 0; i < 4; i++)
        printf_to_char (ch, "You are %s %s.\n\r",
            ac_rating_phrase (GET_AC (ch, i)), ac_type_name(i));

    if (ch->level >= 10)
        printf_to_char (ch, "Alignment: %d.  ", ch->alignment);
    printf_to_char (ch, "You are %s.\n\r", align_name (ch->alignment));

    /* wizinvis and holy light */
    if (IS_IMMORTAL (ch)) {
        printf_to_char (ch, "Holy Light: %s",
            EXT_IS_SET (ch->ext_plr, PLR_HOLYLIGHT) ? "ON" : "OFF");
        if (ch->invis_level)
            printf_to_char (ch, "  Invisible: level %d", ch->invis_level);
        if (ch->incog_level)
            printf_to_char (ch, "  Incognito: level %d", ch->incog_level);
        send_to_char ("\n\r", ch);
    }

    if (IS_SET (ch->comm, COMM_SHOW_AFFECTS))
        do_function (ch, &do_affects, "");
}

DEFINE_DO_FUN (do_affects) {
    AFFECT_T *paf, *paf_last = NULL;

    BAIL_IF (ch->affect_first == NULL,
        "You are not affected by any spells.\n\r", ch);

    send_to_char ("You are affected by the following spells:\n\r", ch);
    for (paf = ch->affect_first; paf != NULL; paf = paf->on_next) {
        if (paf_last == NULL || paf->type != paf_last->type)
            printf_to_char (ch, "Spell: %-15s\n\r", skill_table[paf->type].name);
        else if (ch->level < 20)
            continue;

        if (ch->level >= 20) {
            if (paf->apply == APPLY_NONE)
                printf_to_char (ch, "   lasts ");
            else
                printf_to_char (ch, "   modifies %s by %d ",
                    affect_apply_name (paf->apply), paf->modifier);
            if (paf->duration == -1)
                send_to_char ("permanently\n\r", ch);
            else
                printf_to_char (ch, "for %d hours\n\r", paf->duration);
        }
        paf_last = paf;
    }
}

DEFINE_DO_FUN (do_time) {
    const DAY_T *day_obj;
    const MONTH_T *month_obj;
    char *suf;
    int day = time_info.day + 1;

    /* Determine suffix */
         if (day > 4 && day < 20) suf = "th";
    else if (day % 10 == 1)       suf = "st";
    else if (day % 10 == 2)       suf = "nd";
    else if (day % 10 == 3)       suf = "rd";
    else                          suf = "th";

    day_obj   = day_get_current();
    month_obj = month_get_current();

    printf_to_char (ch,
        "It is %d o'clock %s, Day of %s, %d%s the Month of %s.\n\r",
        (time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
        time_info.hour >= 12 ? "pm" : "am",
        day_obj->name, day, suf, month_obj->name);

    printf_to_char (ch, "ROM started up at %s.\n\rThe system time is %s.\n\r",
        str_boot_time, (char *) ctime_fixed (&current_time));
}

DEFINE_DO_FUN (do_weather) {
    const SKY_T *sky;
    char *change;

    BAIL_IF (!IS_OUTSIDE (ch),
        "You can't see the weather indoors.\n\r", ch);

    sky = sky_get_current ();
    change = weather_info.change >= 0
        ? "a warm southerly breeze blows"
        : "a cold northern gust blows";

    printf_to_char (ch, "The sky is %s and %s.\n\r", sky->description, change);
}

DEFINE_DO_FUN (do_help) {
    HELP_T *help;
    BUFFER_T *output;
    bool found = FALSE;
    char argall[MAX_INPUT_LENGTH], argone[MAX_INPUT_LENGTH];
    int level, trust;

    output = buf_new ();
    if (argument[0] == '\0')
        argument = "summary";

    /* This parts handles "help a b" so that it returns "help 'a b'" */
    argall[0] = '\0';
    while (argument[0] != '\0') {
        argument = one_argument (argument, argone);
        if (argall[0] != '\0')
            strcat (argall, " ");
        strcat (argall, argone);
    }

    trust = char_get_trust (ch);
    for (help = help_first; help != NULL; help = help->global_next) {
        level = (help->level < 0) ? -1 * help->level - 1 : help->level;
        if (level > trust)
            continue;

        if (str_in_namelist (argall, help->keyword)) {
            /* add seperator if found */
            if (found)
                buf_cat (output,
                    "\n\r============================================================\n\r\n\r");
            if (help->level >= 0 && str_cmp (argall, "imotd")) {
                buf_cat (output, help->keyword);
                buf_cat (output, "\n\r");
            }

            /* Strip leading '.' to allow initial blanks. */
            if (help->text[0] == '.')
                buf_cat (output, help->text + 1);
            else
                buf_cat (output, help->text);
            found = TRUE;

            /* small hack :) */
            if (ch->desc != NULL && ch->desc->connected != CON_PLAYING
                    && ch->desc->connected != CON_GEN_GROUPS)
                break;
        }
    }

    if (!found) {
        send_to_char ("No help on that word.\n\r", ch);
        /* Let's log unmet help requests so studious IMP's can improve their help files ;-)
         * But to avoid idiots, we will check the length of the help request, and trim to
         * a reasonable length (set it by redefining MAX_CMD_LEN in merc.h).  -- JR */
        if (strlen(argall) > MAX_CMD_LEN) {
            argall[MAX_CMD_LEN - 1] = '\0';
            log_f ("Excessive command length: %s requested %s.", ch->name, argall);
            send_to_char ("That was rude!\n\r", ch);
        }
        /* OHELPS_FILE is the "orphaned helps" files. Defined in merc.h -- JR */
        else
            append_file (ch, OHELPS_FILE, argall);
    }
    else
        page_to_char (buf_string (output), ch);

    buf_free (output);
}

/* whois command */
DEFINE_DO_FUN (do_whois) {
    char arg[MAX_INPUT_LENGTH];
    BUFFER_T *output;
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_T *d;
    bool found = FALSE;

    DO_REQUIRE_ARG (arg, "You must provide a name.\n\r");

    output = buf_new ();
    for (d = descriptor_first; d != NULL; d = d->global_next) {
        CHAR_T *wch = CH(d);
        if (d->connected != CON_PLAYING)
            continue;
        if (!char_can_see_anywhere (ch, d->character))
            continue;
        if (!char_can_see_anywhere (ch, wch))
            continue;
        if (str_prefix (arg, wch->name))
            continue;

        found = TRUE;
        char_get_who_string (ch, wch, buf, sizeof(buf));
        buf_cat (output, buf);
    }

    BAIL_IF (!found,
        "No one of that name is playing.\n\r", ch);

    page_to_char (buf_string (output), ch);
    buf_free (output);
}

/* New 'who' command originally by Alander of Rivers of Mud. */
DEFINE_DO_FUN (do_who) {
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    BUFFER_T *output;
    DESCRIPTOR_T *d;
    int i, level_lower, level_upper;
    int current_number, matches;
    bool restrict_class = FALSE;
    bool restrict_clan = FALSE;
    bool only_clan = FALSE;
    bool restrict_race = FALSE;
    bool only_immortal = FALSE;
    bool show_class[CLASS_MAX];
    bool show_race[RACE_MAX];
    bool show_clan[CLAN_MAX];

    /* Set default arguments. */
    level_lower = 0;
    level_upper = MAX_LEVEL;
    for (i = 0; i < CLASS_MAX; i++)
        show_class[i] = FALSE;
    for (i = 0; i < RACE_MAX; i++)
        show_race[i] = FALSE;
    for (i = 0; i < CLAN_MAX; i++)
        show_clan[i] = FALSE;

    /* Parse arguments. */
    current_number = 0;
    while (1) {
        char arg[MAX_STRING_LENGTH];
        argument = one_argument (argument, arg);
        if (arg[0] == '\0')
            break;

        /* Check for level arguments. */
        if (is_number (arg)) {
            switch (++current_number) {
                case 1: level_lower = atoi (arg); break;
                case 2: level_upper = atoi (arg); break;
                default:
                    send_to_char ("Only two level numbers allowed.\n\r", ch);
                    return;
            }
            continue;
        }

        /* Look for classes to turn on. */
        if (!str_prefix (arg, "immortals")) {
            only_immortal = TRUE;
            continue;
        }

        /* Check for explicit classes. */
        if ((i = class_lookup (arg)) >= 0) {
            restrict_class = TRUE;
            show_class[i] = TRUE;
            continue;
        }

        /* Check for explicit races. */
        i = pc_race_lookup (arg);
        if (i >= 1) {
            const PC_RACE_T *pc_race = pc_race_get (i);
            if ((i = race_lookup_exact (pc_race->name)) >= 1) {
                restrict_race = TRUE;
                show_race[i] = TRUE;
                continue;
            }
        }

        /* Check for anyone with a clan. */
        if (!str_prefix (arg, "clan")) {
            only_clan = TRUE;
            continue;
        }

        /* Check for specific clans. */
        if ((i = clan_lookup (arg)) >= 0) {
            restrict_clan = TRUE;
            show_clan[i] = TRUE;
            continue;
        }

        /* Unknown argument. */
        send_to_char ("That's not a valid race, class, or clan.\n\r", ch);
        return;
    }

    /* Now show matching chars.  */
    matches = 0;
    buf[0] = '\0';
    output = buf_new ();
    for (d = descriptor_first; d != NULL; d = d->global_next) {
        CHAR_T *wch = CH(d);

        /* Check for match against restrictions.
         * Don't use trust as that exposes trusted mortals.  */
        if (d->connected != CON_PLAYING)
            continue;
        if (!char_can_see_anywhere (ch, d->character))
            continue;
        if (!char_can_see_anywhere (ch, wch))
            continue;
        if (wch->level < level_lower || wch->level > level_upper)
            continue;
        if (only_immortal && wch->level < LEVEL_IMMORTAL)
            continue;
        if (restrict_class && !show_class[wch->class])
            continue;
        if (restrict_race && !show_race[wch->race])
            continue;
        if (only_clan && !player_has_clan (wch))
            continue;
        if (restrict_clan && !show_clan[wch->clan])
            continue;

        matches++;
        char_get_who_string (ch, wch, buf, sizeof(buf));
        buf_cat (output, buf);
    }

    sprintf (buf2, "\n\rPlayers found: %d\n\r", matches);
    buf_cat (output, buf2);
    page_to_char (buf_string (output), ch);
    buf_free (output);
}

/* for keeping track of the player count */
static int max_on = 0;
DEFINE_DO_FUN (do_count) {
    int count;
    DESCRIPTOR_T *d;

    count = 0;
    for (d = descriptor_first; d != NULL; d = d->global_next)
        if (d->connected == CON_PLAYING && char_can_see_anywhere (ch, d->character))
            count++;
    max_on = UMAX (count, max_on);

    if (max_on == count) {
        printf_to_char (ch,
            "There are %d characters on, the most so far today.\n\r",
            count);
    }
    else {
        printf_to_char (ch,
            "There are %d characters on, the most on today was %d.\n\r",
            count, max_on);
    }
}

DEFINE_DO_FUN (do_inventory) {
    send_to_char ("You are carrying:\n\r", ch);
    obj_list_show_to_char (ch->content_first, ch, TRUE, TRUE);
}

DEFINE_DO_FUN (do_equipment) {
    const WEAR_LOC_T *wear;
    OBJ_T *obj;
    int wear_loc;
    bool found;

    send_to_char ("You are using:\n\r", ch);
    found = FALSE;
    for (wear_loc = 0; wear_loc < WEAR_LOC_MAX; wear_loc++) {
        if ((obj = char_get_eq_by_wear_loc (ch, wear_loc)) == NULL)
            continue;
        if ((wear = wear_loc_get (wear_loc)) == NULL)
            continue;

        printf_to_char (ch, "%-21s ", wear->look_msg);
        if (char_can_see_obj (ch, obj))
            printf_to_char (ch, "%s\n\r", obj_format_to_char (obj, ch, TRUE));
        else
            send_to_char ("something.\n\r", ch);
        found = TRUE;
    }

    if (!found)
        send_to_char ("Nothing.\n\r", ch);
}

DEFINE_DO_FUN (do_compare) {
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_T *obj1;
    OBJ_T *obj2;
    int value1;
    int value2;
    char *msg;

    argument = one_argument (argument, arg1);
    BAIL_IF (arg1[0] == '\0',
        "Compare what to what?\n\r", ch);
    BAIL_IF ((obj1 = find_obj_own_inventory (ch, arg1)) == NULL,
        "You do not have that item.\n\r", ch);

    argument = one_argument (argument, arg2);
    if (arg2[0] == '\0') {
        for (obj2 = ch->content_first; obj2 != NULL; obj2 = obj2->content_next) {
            if (obj2->wear_loc == WEAR_LOC_NONE)
                continue;
            if (!char_can_see_obj (ch, obj2))
                continue;
            if (obj1->item_type != obj2->item_type)
                continue;
            if ((obj1->wear_flags & (obj2->wear_flags & ~ITEM_TAKE)) == 0)
                continue;
            break;
        }
        BAIL_IF (obj2 == NULL,
            "You aren't wearing anything comparable.\n\r", ch);
    }
    else {
        BAIL_IF ((obj2 = find_obj_own_inventory (ch, arg2)) == NULL,
            "You do not have that item.\n\r", ch);
    }

    msg = NULL;
    value1 = 0;
    value2 = 0;

    if (obj1 == obj2)
        msg = "You compare $p to itself.  It looks about the same.";
    else if (obj1->item_type != obj2->item_type)
        msg = "You can't compare $p and $P.";
    else if (!item_is_comparable (obj1))
        msg = "You can't compare $p and $P.";
    else {
        value1 = item_get_compare_value (obj1);
        value2 = item_get_compare_value (obj2);
    }

    if (msg == NULL) {
             if (value1 == value2) msg = "$p and $P look about the same.";
        else if (value1  > value2) msg = "$p looks better than $P.";
        else                       msg = "$p looks worse than $P.";
    }

    act (msg, ch, obj1, obj2, TO_CHAR);
}

DEFINE_DO_FUN (do_where) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_T *victim;
    DESCRIPTOR_T *d;
    bool found;

    one_argument (argument, arg);

    if (arg[0] == '\0') {
        send_to_char ("Players near you:\n\r", ch);
        found = FALSE;
        for (d = descriptor_first; d; d = d->global_next) {
            if (d->connected == CON_PLAYING
                && (victim = d->character) != NULL && !IS_NPC (victim)
                && victim->in_room != NULL
                && !IS_SET (victim->in_room->room_flags, ROOM_NOWHERE)
                && (room_is_owner (victim->in_room, ch)
                    || !room_is_private (victim->in_room))
                && victim->in_room->area == ch->in_room->area
                && char_can_see_anywhere (ch, victim))
            {
                found = TRUE;
                printf_to_char (ch, "%-28s %s\n\r",
                    victim->name, victim->in_room->name);
            }
        }
        if (!found)
            send_to_char ("None\n\r", ch);
    }
    else {
        found = FALSE;
        for (victim = char_first; victim != NULL; victim = victim->global_next) {
            if (victim->in_room != NULL
                && victim->in_room->area == ch->in_room->area
                && !IS_AFFECTED (victim, AFF_HIDE)
                && !IS_AFFECTED (victim, AFF_SNEAK)
                && char_can_see_anywhere (ch, victim)
                && str_in_namelist (arg, victim->name))
            {
                found = TRUE;
                printf_to_char (ch, "%-28s %s\n\r",
                    PERS_AW (victim, ch), victim->in_room->name);
                break;
            }
        }
        if (!found)
            act ("You didn't find any $T.", ch, NULL, arg, TO_CHAR);
    }
}

DEFINE_DO_FUN (do_title) {
    int i;

    if (IS_NPC (ch))
        return;

    /* Changed this around a bit to do some sanitization first   *
     * before checking length of the title. Need to come up with *
     * a centralized user input sanitization scheme. FIXME!      *
     * JR -- 10/15/00                                            */

    if (strlen (argument) > 45)
        argument[45] = '\0';

    i = strlen(argument);
    if (argument[i-1] == '{' && argument[i-2] != '{')
        argument[i-1] = '\0';

    BAIL_IF (argument[0] == '\0',
        "Change your title to what?\n\r", ch);

    str_smash_tilde (argument);
    player_set_title (ch, argument);
    printf_to_char(ch, "Your title is now:%s.\n\r", ch->pcdata->title);
}

DEFINE_DO_FUN (do_description) {
    if (argument[0] != '\0')
        if (do_filter_description_alter (ch, argument))
            return;

    send_to_char ("Your description is:\n\r", ch);
    send_to_char (ch->description ? ch->description : "(None).\n\r", ch);
}

DEFINE_DO_FUN (do_report) {
    char buf[MAX_INPUT_LENGTH];
    sprintf (buf, "I have %d/%d hp %d/%d mana %d/%d mv %d xp.",
        ch->hit, ch->max_hit, ch->mana, ch->max_mana,
        ch->move, ch->max_move, ch->exp);
    do_say (ch, buf);
}

/* Contributed by Alander. */
DEFINE_DO_FUN (do_commands) {
    int cmd, col;

    col = 0;
    for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++) {
        if (cmd_table[cmd].level < LEVEL_HERO &&
            cmd_table[cmd].level <= char_get_trust (ch) && cmd_table[cmd].show)
        {
            printf_to_char (ch, "%-12s", cmd_table[cmd].name);
            if (++col % 6 == 0)
                send_to_char ("\n\r", ch);
        }
    }

    if (col % 6 != 0)
        send_to_char ("\n\r", ch);
}

DEFINE_DO_FUN (do_areas) {
    char buf[MAX_STRING_LENGTH];
    AREA_T *area1, *area2;
    int i, areas_half;

    BAIL_IF (argument[0] != '\0',
        "No argument is used with this command.\n\r", ch);

    areas_half = (TOP(RECYCLE_AREA_T) + 1) / 2;
    area1 = area_first;
    area2 = area_first;
    for (i = 0; i < areas_half; i++)
        area2 = area2->global_next;

    for (i = 0; i < areas_half; i++) {
        sprintf (buf, "%-39s%-39s\n\r",
                 area1->credits, (area2 != NULL) ? area2->credits : "");
        send_to_char_bw (buf, ch);
        area1 = area1->global_next;
        if (area2 != NULL)
            area2 = area2->global_next;
    }
}

DEFINE_DO_FUN (do_scan_short)
    { do_scan_real (ch, argument, 1); }
DEFINE_DO_FUN (do_scan_far)
    { do_scan_real (ch, argument, 3); }
