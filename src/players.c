

#include <string.h>

#include "chars.h"
#include "lookup.h"
#include "objs.h"
#include "affects.h"
#include "memory.h"
#include "utils.h"
#include "globals.h"
#include "comm.h"
#include "save.h"
#include "magic.h"
#include "groups.h"
#include "fight.h"
#include "rooms.h"
#include "items.h"

#include "players.h"

bool player_has_clan (const CHAR_T *ch) {
    if (IS_NPC (ch))
        return FALSE;
    return (ch->clan > 0) ? TRUE : FALSE;
}

bool player_is_independent (const CHAR_T *ch) {
    const CLAN_T *clan;
    if (IS_NPC (ch))
        return TRUE;
    if ((clan = clan_get (ch->clan)) == NULL)
        return TRUE;
    return clan->independent ? TRUE : FALSE;
}

bool player_in_same_clan (const CHAR_T *ch, const CHAR_T *victim) {
    const CLAN_T *clan;
    if (IS_NPC (ch) || IS_NPC (victim))
        return FALSE;
    if ((clan = clan_get (ch->clan)) == NULL)
        return FALSE;
    if (clan->independent)
        return FALSE;
    return (ch->clan == victim->clan);
}

/* used to de-screw characters */
void player_reset (CHAR_T *ch) {
    int loc, mod, stat;
    OBJ_T *obj;
    AFFECT_T *af;
    int i;

    if (IS_NPC (ch))
        return;

    if (ch->pcdata->perm_hit   == 0 ||
        ch->pcdata->perm_mana  == 0 ||
        ch->pcdata->perm_move  == 0 ||
        ch->pcdata->last_level == 0)
    {
        /* do a FULL reset */
        for (loc = 0; loc < WEAR_LOC_MAX; loc++) {
            obj = char_get_eq_by_wear_loc (ch, loc);
            if (obj == NULL)
                continue;
            if (!obj->enchanted) {
                for (af = obj->obj_index->affect_first; af; af = af->on_next) {
                    mod = af->modifier;
                    switch (af->apply) {
                        case APPLY_SEX:
                            ch->sex -= mod;
                            if (ch->sex < 0 || ch->sex > 2)
                                ch->sex = IS_NPC (ch) ? 0 : ch->pcdata->true_sex;
                            break;
                        case APPLY_MANA: ch->max_mana -= mod; break;
                        case APPLY_HIT:  ch->max_hit  -= mod; break;
                        case APPLY_MOVE: ch->max_move -= mod; break;
                    }
                }
            }

            for (af = obj->affect_first; af; af = af->on_next) {
                mod = af->modifier;
                switch (af->apply) {
                    case APPLY_SEX:  ch->sex      -= mod; break;
                    case APPLY_MANA: ch->max_mana -= mod; break;
                    case APPLY_HIT:  ch->max_hit  -= mod; break;
                    case APPLY_MOVE: ch->max_move -= mod; break;
                }
            }
        }

        /* now reset the permanent stats */
        ch->pcdata->perm_hit = ch->max_hit;
        ch->pcdata->perm_mana = ch->max_mana;
        ch->pcdata->perm_move = ch->max_move;
        ch->pcdata->last_level = ch->played / 3600;

        if (ch->pcdata->true_sex < 0 || ch->pcdata->true_sex > 2) {
            if (ch->sex > 0 && ch->sex < 3)
                ch->pcdata->true_sex = ch->sex;
            else
                ch->pcdata->true_sex = 0;
        }
    }

    /* now restore the character to his/her true condition */
    for (stat = 0; stat < STAT_MAX; stat++)
        ch->mod_stat[stat] = 0;

    if (ch->pcdata->true_sex < 0 || ch->pcdata->true_sex > 2)
        ch->pcdata->true_sex = 0;
    ch->sex      = ch->pcdata->true_sex;
    ch->max_hit  = ch->pcdata->perm_hit;
    ch->max_mana = ch->pcdata->perm_mana;
    ch->max_move = ch->pcdata->perm_move;

    for (i = 0; i < 4; i++)
        ch->armor[i] = 100;

    ch->hitroll = 0;
    ch->damroll = 0;
    ch->saving_throw = 0;

    /* now start adding back the effects */
    for (loc = 0; loc < WEAR_LOC_MAX; loc++) {
        obj = char_get_eq_by_wear_loc (ch, loc);
        if (obj == NULL)
            continue;
        for (i = 0; i < 4; i++)
            ch->armor[i] -= obj_get_ac_type (obj, loc, i);
        if (obj->enchanted)
            continue;
        for (af = obj->obj_index->affect_first; af; af = af->on_next)
            affect_modify_char_apply (af, ch, TRUE);
        for (af = obj->affect_first; af; af = af->on_next)
            affect_modify_char_apply (af, ch, TRUE);
    }

    /* now add back spell effects */
    for (af = ch->affect_first; af; af = af->on_next)
        affect_modify_char_apply (af, ch, TRUE);

    /* make sure sex is RIGHT!!!! */
    if (ch->sex < 0 || ch->sex > 2)
        ch->sex = ch->pcdata->true_sex;
}

/* Config Colour stuff */
void player_reset_colour (CHAR_T *ch) {
    int i;

    if (ch == NULL)
        return;
    ch = REAL_CH (ch);

    if (ch->pcdata == NULL)
        return;

    for (i = 0; i < COLOUR_SETTING_MAX; i++)
        ch->pcdata->colour[i] = colour_setting_table[i].default_colour;
}

void player_set_title (CHAR_T *ch, char *title) {
    char buf[MAX_STRING_LENGTH];

    BAIL_IF_BUG (IS_NPC (ch),
        "player_set_title: NPC.", 0);

    if (title[0] != '.' && title[0] != ',' && title[0] != '!' && title[0] != '?') {
        buf[0] = ' ';
        strcpy (buf + 1, title);
    }
    else
        strcpy (buf, title);

    str_replace_dup (&(ch->pcdata->title), buf);
}

/* Advancement stuff. */
void player_advance_level (CHAR_T *ch, bool hide) {
    char buf[MAX_STRING_LENGTH];
    int add_hp, add_mana, add_move, add_prac;

    if (IS_NPC (ch))
        return;

    ch->pcdata->last_level =
        (ch->played + (int) (current_time - ch->logon)) / 3600;

    sprintf (buf, "the %s",
        title_table[ch->class][ch->level][ch->sex == SEX_FEMALE ? 1 : 0]);
    player_set_title (ch, buf);

    add_hp = char_con_level_hp (ch) +
        number_range (class_table[ch->class].hp_min,
                      class_table[ch->class].hp_max);
    add_mana = number_range (2, (2 * char_get_curr_stat (ch, STAT_INT)
                                 + char_get_curr_stat (ch, STAT_WIS)) / 5);
    if (!class_table[ch->class].gains_mana)
        add_mana /= 2;
    add_move = number_range (1, (char_get_curr_stat (ch, STAT_CON)
                                 + char_get_curr_stat (ch, STAT_DEX)) / 6);
    add_prac = char_wis_level_practices (ch);

    add_hp   = add_hp   * 9 / 10;
    add_mana = add_mana * 9 / 10;
    add_move = add_move * 9 / 10;

    add_hp   = UMAX (2, add_hp);
    add_mana = UMAX (2, add_mana);
    add_move = UMAX (6, add_move);

    ch->max_hit  += add_hp;
    ch->max_mana += add_mana;
    ch->max_move += add_move;
    ch->practice += add_prac;
    ch->train += 1;

    ch->pcdata->perm_hit += add_hp;
    ch->pcdata->perm_mana += add_mana;
    ch->pcdata->perm_move += add_move;

    if (!hide) {
        printf_to_char (ch, "You gain %d hit point%s, %d mana, %d move, and "
            "%d practice%s.\n\r", add_hp, add_hp == 1 ? "" : "s", add_mana,
            add_move, add_prac, add_prac == 1 ? "" : "s");
    }
}

void player_gain_exp (CHAR_T *ch, int gain) {
    if (IS_NPC (ch) || ch->level >= LEVEL_HERO)
        return;

    ch->exp = UMAX (player_get_exp_per_level (ch), ch->exp + gain);
    while (ch->level < LEVEL_HERO && ch->exp >=
           player_get_exp_per_level (ch) * (ch->level + 1))
    {
        send_to_char ("{GYou raise a level!!  {x", ch);
        ch->level += 1;
        log_f ("%s gained level %d", ch->name, ch->level);
        wiznetf (ch, NULL, WIZ_LEVELS, 0, 0,
            "$N has attained level %d!", ch->level);
        player_advance_level (ch, FALSE);
        save_char_obj (ch);
    }
}

int player_get_exp_to_next_level (const CHAR_T *ch) {
    if (IS_NPC (ch) || ch->level >= LEVEL_HERO)
        return 1000;
    return (ch->level + 1) * player_get_exp_per_level (ch) - ch->exp;
}

int player_get_exp_per_level (const CHAR_T *ch) {
    return player_get_exp_per_level_with_points (
        ch, ch->pcdata->creation_points);
}

int player_get_exp_per_level_with_points (const CHAR_T *ch, int points) {
    const PC_RACE_T *race;
    int expl, inc, mult;

    if (IS_NPC (ch))
        return 1000;

    expl = 1000;
    inc = 500;

    race = pc_race_get_by_race (ch->race);
    mult = race->class_mult[ch->class];

    if (points < 40)
        return 1000 * (mult ? mult / 100 : 1);

    /* processing */
    points -= 40;
    while (points > 9) {
        expl += inc;
        points -= 10;
        if (points > 9) {
            expl += inc;
            inc *= 2;
            points -= 10;
        }
    }

    expl += points * inc / 10;
    return expl * mult / 100;
}

void player_set_default_skills (CHAR_T *ch) {
    sh_int *learned = ch->pcdata->learned;
    if (learned[SN(RECALL)] < 50)
        learned[SN(RECALL)] = 50;
}

int player_get_skill_learned (const CHAR_T *ch, int sn) {
    const SKILL_T *skill = skill_get (sn);
    if (skill == NULL)
        return -1;
    return (ch->level < skill->classes[ch->class].level)
        ? 0 : ch->pcdata->learned[sn];
}

void player_list_skills_and_groups (CHAR_T *ch, bool chosen) {
    const SKILL_GROUP_T *group;
    const SKILL_T *skill;
    int num, col;

    chosen = (!!chosen);

    if (IS_NPC (ch))
        return;

#ifdef BASEMUD_DOTTED_LINES_IN_SKILLS
    #define LINE_CHAR '.'
#else
    #define LINE_CHAR ' '
#endif

    col = 0;
    printf_to_char (ch, "Groups:\n\r");
    for (num = 0; num < SKILL_GROUP_MAX; num++) {
        if ((group = skill_group_get (num)) == NULL || group->name == NULL)
            break;
        if (group->classes[ch->class].cost <= 0)
            continue;
        if (!!ch->pcdata->group_known[num] != chosen)
            continue;
        if (ch->gen_data && !!ch->gen_data->group_chosen[num] != chosen)
            continue;

        if (col % 3 == 0)
            send_to_char ("   ", ch);
        printf_to_char (ch, "%s%s%d  ", group->name,
            str_line (LINE_CHAR, 19 - strlen (group->name) +
                (3 - int_str_len (group->classes[ch->class].cost))),
            group->classes[ch->class].cost);
        if (++col % 3 == 0)
            send_to_char ("\n\r", ch);
    }
    if (col == 0)
        send_to_char ("   None.\n\r", ch);
    if (col % 3 != 0)
        send_to_char ("\n\r", ch);
    send_to_char ("\n\r", ch);

    col = 0;
    printf_to_char (ch, "Skills:\n\r");
    for (num = 0; num < SKILL_MAX; num++) {
        if ((skill = skill_get (num)) == NULL || skill->name == NULL)
            break;
        if (skill->classes[ch->class].effort <= 0)
            continue;
        if (skill->spell_fun != spell_null)
            continue;
        if (!!ch->pcdata->skill_known[num] != chosen)
            continue;
        if (ch->gen_data && !!ch->gen_data->skill_chosen[num] != chosen)
            continue;
        if ((ch->pcdata->learned[num] > 0) != chosen)
            continue;

        if (col % 3 == 0)
            send_to_char ("   ", ch);

        printf_to_char (ch, "%s%s%d  ", skill->name,
            str_line (LINE_CHAR, 19 - strlen (skill->name) +
                (3 - int_str_len (skill->classes[ch->class].effort))),
            skill->classes[ch->class].effort);
        if (++col % 3 == 0)
            send_to_char ("\n\r", ch);
    }
    if (col == 0)
        send_to_char ("   None.\n\r", ch);
    if (col % 3 != 0)
        send_to_char ("\n\r", ch);

    /* only show generation points during character generation. */
    if (ch->gen_data) {
        send_to_char ("\n\r", ch);
        printf_to_char (ch, "Creation points: %d\n\r",
            ch->pcdata->creation_points);
        printf_to_char (ch, "Experience per level: %d\n\r",
            player_get_exp_per_level (ch));
    }
}

/* checks for skill improvement */
void player_try_skill_improve (CHAR_T *ch, int sn, bool success,
    int multiplier)
{
    const SKILL_T *skill;
    int chance;

    if (IS_NPC (ch))
        return;

    /* skill is not known or already mastered */
    BAIL_IF_BUG ((skill = skill_get (sn)) == NULL,
        "player_try_skill_improve: Invalid skill %d", sn);
    if (ch->level < skill->classes[ch->class].level ||
        skill->classes[ch->class].effort == 0 ||
        ch->pcdata->learned[sn] == 0 ||
        ch->pcdata->learned[sn] == 100)
        return;

    /* check to see if the character has a chance to learn */
    chance = 10 * char_int_learn_rate (ch);
    chance /= (multiplier * skill->classes[ch->class].effort * 4);
    chance += ch->level;

    if (number_range (1, 1000) > chance)
        return;

    /* now that the character has a CHANCE to learn, see if they really have */
    if (success) {
        chance = URANGE (5, 100 - ch->pcdata->learned[sn], 95);
        if (number_percent () < chance) {
            printf_to_char (ch, "{5You have become better at %s!{x\n\r",
                skill->name);
            ch->pcdata->learned[sn]++;
            player_gain_exp (ch, 2 * skill->classes[ch->class].effort);
        }
    }
    else {
        chance = URANGE (5, ch->pcdata->learned[sn] / 2, 30);
        if (number_percent () < chance) {
            printf_to_char (ch,
                "{5You learn from your mistakes, and your %s skill improves.{x\n\r",
                skill->name);
            ch->pcdata->learned[sn] += number_range (1, 3);
            ch->pcdata->learned[sn] = UMIN (ch->pcdata->learned[sn], 100);
            player_gain_exp (ch, 2 * skill->classes[ch->class].effort);
        }
    }
}

void player_add_skill (CHAR_T *ch, int sn, bool deduct) {
    const SKILL_T *skill;

    if (IS_NPC (ch) || !ch->pcdata)
        return;
    if ((skill = skill_get (sn)) == NULL) {
        bugf ("player_add_skill: Unknown skill number %d", sn);
        return;
    }

    if (!ch->pcdata->skill_known[sn] && deduct)
        ch->pcdata->creation_points += skill->classes[ch->class].effort;
    ch->pcdata->skill_known[sn]++;
    if (ch->pcdata->learned[sn] == 0)
        ch->pcdata->learned[sn] = 1;
}

void player_remove_skill (CHAR_T *ch, int sn, bool refund) {
    const SKILL_T *skill;

    if (IS_NPC (ch) || !ch->pcdata)
        return;
    if ((skill = skill_get (sn)) == NULL) {
        bugf ("player_remove_skill: Unknown skill number %d", sn);
        return;
    }

    if (ch->pcdata->skill_known[sn] <= 0)
        return;
    if (refund)
        ch->pcdata->creation_points -= skill->classes[ch->class].effort;
    ch->pcdata->skill_known[sn]--;
    if (ch->pcdata->skill_known[sn] == 0)
        ch->pcdata->learned[sn] = 0;
}

/* recursively adds a group given its number -- uses skill_group_add */
void player_add_skill_group (CHAR_T *ch, int gn, bool deduct) {
    const SKILL_GROUP_T *group;
    int i;

    if (IS_NPC (ch) || !ch->pcdata)
        return;
    if ((group = skill_group_get (gn)) == NULL) {
        bugf ("player_add_skill_group: Unknown group number %d", gn);
        return;
    }

    if (!ch->pcdata->group_known[gn] && deduct)
        ch->pcdata->creation_points += group->classes[ch->class].cost;
    ch->pcdata->group_known[gn]++;

    for (i = 0; i < MAX_IN_GROUP; i++) {
        if (group->spells[i] == NULL)
            break;
        player_add_skill_or_group (ch, group->spells[i], FALSE);
    }
}

/* recusively removes a group given its number -- uses skill_group_remove */
void player_remove_skill_group (CHAR_T *ch, int gn, bool refund) {
    const SKILL_GROUP_T *group;
    int i;

    if (IS_NPC (ch) || !ch->pcdata)
        return;
    if ((group = skill_group_get (gn)) == NULL) {
        bugf ("player_remove_skill_group: Unknown group number %d", gn);
        return;
    }

    if (ch->pcdata->group_known[gn] <= 0)
        return;
    if (refund)
        ch->pcdata->creation_points -= group->classes[ch->class].cost;
    ch->pcdata->group_known[gn]--;

    for (i = 0; i < MAX_IN_GROUP; i++) {
        if (group->spells[i] == NULL)
            break;
        player_remove_skill_or_group (ch, group->spells[i], FALSE);
    }
}

/* use for processing a skill or group for addition  */
void player_add_skill_or_group (CHAR_T *ch, const char *name, bool deduct) {
    int num;

    if (IS_NPC (ch) || !ch->pcdata)
        return;

    /* first, check for skills. */
    num = skill_lookup_exact (name);
    if (num != -1) {
        player_add_skill (ch, num, deduct);
        return;
    }

    num = skill_group_lookup_exact (name);
    if (num != -1) {
        player_add_skill_group (ch, num, deduct);
        return;
    }

    bugf ("player_add_skill_or_group: Unknown skill or group '%s'", name);
}

/* used for processing a skill or group for deletion */
void player_remove_skill_or_group (CHAR_T *ch, const char *name, bool refund) {
    int num;

    if (IS_NPC (ch) || !ch->pcdata)
        return;

    /* first, check for skills. */
    num = skill_lookup_exact (name);
    if (num != -1) {
        player_remove_skill (ch, num, refund);
        return;
    }

    /* now check groups */
    num = skill_group_lookup_exact (name);
    if (num != -1) {
        player_remove_skill_group (ch, num, refund);
        return;
    }

    bugf ("player_remove_skill_or_group: Unknown skill or group '%s'", name);
}

bool player_is_undesirable (const CHAR_T *ch) {
    return EXT_IS_SET (ch->ext_plr, PLR_KILLER) ||
           EXT_IS_SET (ch->ext_plr, PLR_THIEF);
}

void player_die (CHAR_T *ch) {
    const RACE_T *race;
    int i;

    nuke_pets (ch);

    stop_fighting (ch, TRUE);
    while (ch->content_first)
        obj_extract (ch->content_first);

    /* Death room is set in the clan table now */
    char_to_room (ch, room_get_index (clan_table[ch->clan].hall));

    while (ch->affect_first)
        affect_remove (ch->affect_first);

    race = race_get (ch->race);
    ch->affected_by = race->aff;
    for (i = 0; i < 4; i++)
        ch->armor[i] = 100;

    ch->position = POS_RESTING;
    ch->hit  = UMAX (1, ch->hit);
    ch->mana = UMAX (1, ch->mana);
    ch->move = UMAX (1, ch->move);
}
    
int player_get_weapon_skill (const CHAR_T *ch, int sn) {
    int skill;

    /* -1 is exotic */
    if (sn == -1)
        skill = 3 * ch->level;
    else
        skill = ch->pcdata->learned[sn];

    return URANGE (0, skill, 100);
}

bool player_move_filter (CHAR_T *ch, ROOM_INDEX_T *from_room,
    ROOM_INDEX_T *to_room)
{
    int i, j;
    int move, in_sect, to_sect, flying;

    for (i = 0; class_get (i) != NULL; i++) {
        for (j = 0; j < MAX_GUILD; j++) {
            FILTER (i != ch->class &&
                     to_room->vnum == class_table[i].guild[j],
                "You aren't allowed in there.\n\r", ch);
        }
    }

    in_sect = from_room->sector_type;
    to_sect = to_room->sector_type;
    flying = IS_AFFECTED (ch, AFF_FLYING) || IS_IMMORTAL (ch);

    FILTER ((in_sect == SECT_AIR || to_sect == SECT_AIR) && !flying,
        "You can't fly.\n\r", ch);

    if ((in_sect == SECT_WATER_NOSWIM || to_sect == SECT_WATER_NOSWIM) &&
         !flying)
    {
        /* Look for a boat. */
        FILTER (!char_has_boat (ch),
            "You need a boat to go there.\n\r", ch);
    }

    move = sector_table[UMIN (SECT_MAX - 1, from_room->sector_type)].move_loss
         + sector_table[UMIN (SECT_MAX - 1, to_room->sector_type)].move_loss;
    move /= 2; /* the average */

    /* conditional effects */
    if (IS_AFFECTED (ch, AFF_FLYING) || IS_AFFECTED (ch, AFF_HASTE))
        move /= 2;
    if (IS_AFFECTED (ch, AFF_SLOW))
        move *= 2;
    if (move < 1)
        move = 1;

    FILTER (ch->move < move,
        "You are too exhausted.\n\r", ch);

    WAIT_STATE (ch, 1);
    ch->move -= move;
    return FALSE;
}

bool player_drop_weapon_if_too_heavy (CHAR_T *ch) {
    OBJ_T *wield;
    static int depth;

    /* Check for weapon wielding.
     * Guard against recursion (for weapons with affects). */
    if (IS_NPC (ch))
        return FALSE;
    if ((wield = char_get_eq_by_wear_loc (ch, WEAR_LOC_WIELD)) == NULL)
        return FALSE;
    if (obj_get_weight (wield) <= char_str_max_wield_weight (ch) * 10)
        return FALSE;
    if (depth != 0)
        return FALSE;

    depth++;
    act2 ("You drop $p.", "$n drops $p.", ch, wield, NULL, 0, POS_RESTING);
    obj_give_to_room (wield, ch->in_room);
    depth--;
    return TRUE;
}

void player_change_conditions (CHAR_T *ch, int drunk, int full, int thirst,
    int hunger)
{
    if (IS_NPC (ch))
        return;
    if (drunk != 0)
        player_change_condition (ch, COND_DRUNK, drunk);
    if (thirst != 0)
        player_change_condition (ch, COND_THIRST, thirst);
    if (hunger != 0)
        player_change_condition (ch, COND_HUNGER, hunger);
    if (full != 0)
        player_change_condition (ch, COND_FULL, full);
}

void player_change_condition (CHAR_T *ch, int cond, int value) {
    const COND_T *cond_obj;
    int old_cond, new_cond;
    int was_bad, was_good;
    int is_bad,  is_good;
    bool send_good, send_bad, send_better, send_worse;
    const char *msg;

    if (value == 0)
        return;
    if (IS_NPC (ch) || ch->level >= LEVEL_IMMORTAL || ch->pcdata == NULL)
        return;
    if ((cond_obj = cond_get (cond)) == NULL)
        return;

    old_cond = ch->pcdata->cond_hours[cond];
    if (old_cond == -1)
        return;

    new_cond = URANGE (0, old_cond + value, COND_HOURS_MAX);
    if (old_cond == new_cond)
        return;

    /* Get our conditions before adjusting our condition. */
    was_good = (cond_obj->good_fun == NULL) ? -1 : cond_obj->good_fun (ch);
    was_bad  = (cond_obj->bad_fun  == NULL) ? -1 : cond_obj->bad_fun (ch);

    /* Adjust our condition. */
    ch->pcdata->cond_hours[cond] = new_cond;

    /* Get our conditions after adjusting our condition. */
    is_good = (cond_obj->good_fun == NULL) ? -1 : cond_obj->good_fun (ch);
    is_bad  = (cond_obj->bad_fun  == NULL) ? -1 : cond_obj->bad_fun (ch);

    /* What messages should we send over? */
    send_good   = ((was_good == 0) && (is_good >  0));
    send_bad    = ((was_bad  == 0) && (is_bad  >  0));
    send_better = ((was_bad  >  0) && (is_bad  == 0));
    send_worse  = ((was_good >  0) && (is_good == 0));

    msg = NULL;
    if (msg == NULL && send_good)
        msg = cond_obj->msg_good;
    if (msg == NULL && send_bad)
        msg = cond_obj->msg_bad;
    if (msg == NULL && send_better)
        msg = cond_obj->msg_better;
    if (msg == NULL && send_worse)
        msg = cond_obj->msg_worse;

    /* if there's a message, send it over. */
    if (msg)
        send_to_char (msg, ch);
}

void player_update (CHAR_T *ch) {
    OBJ_T *obj;
    bool is_big;

    if (ch->level >= LEVEL_IMMORTAL)
        return;

    if ((obj = char_get_eq_by_wear_loc (ch, WEAR_LOC_LIGHT)) != NULL &&
            item_is_lit (obj))
        item_light_fade (obj);

    if (IS_IMMORTAL (ch))
        ch->timer = 0;

    if (++ch->timer >= 12) {
        if (ch->was_in_room == NULL && ch->in_room != NULL) {
            ch->was_in_room = ch->in_room;
            if (ch->fighting != NULL)
                stop_fighting (ch, TRUE);
            send_to_char ("You disappear into the void.\n\r", ch);
            act ("$n disappears into the void.",
                 ch, NULL, NULL, TO_NOTCHAR);
            if (ch->level > 1)
                save_char_obj (ch);
            char_to_room (ch, room_get_index (ROOM_VNUM_LIMBO));
        }
    }

    is_big = (ch->size > SIZE_MEDIUM);
    player_change_conditions (ch,
        -1, is_big ? -4 : -2,
        -1, is_big ? -2 : -1);
}
