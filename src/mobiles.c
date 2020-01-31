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

#include "globals.h"
#include "utils.h"
#include "recycle.h"
#include "memory.h"
#include "chars.h"
#include "affects.h"
#include "lookup.h"
#include "fight.h"
#include "interp.h"
#include "act_fight.h"
#include "magic.h"

#include "mobiles.h"

/* Translates mob virtual number to its mob index struct.
 * Hash table lookup. */
MOB_INDEX_T *mobile_get_index (int vnum) {
    MOB_INDEX_T *mob_index;

    for (mob_index = mob_index_hash[vnum % MAX_KEY_HASH];
         mob_index != NULL; mob_index = mob_index->next)
        if (mob_index->vnum == vnum)
            return mob_index;

    /* NOTE: This did cause the server not to boot, but that seems a bit
     *       extreme. So we just return NULL instead, and keep on booting.
     *       -- Synival */
    if (in_boot_db) {
        bug ("mobile_get_index: bad vnum %d.", vnum);
     // exit (1);
    }

    return NULL;
}

/* Create an instance of a mobile. */
CHAR_T *mobile_create (MOB_INDEX_T *mob_index) {
    CHAR_T *mob;
    int i;
    AFFECT_T af;

    mobile_count++;
    EXIT_IF_BUG (mob_index == NULL,
        "mobile_create: NULL mob_index.", 0);

    mob = char_new ();

    mob->index_data = mob_index;
    str_replace_dup (&mob->name,        mob_index->name);        /* OLC */
    str_replace_dup (&mob->short_descr, mob_index->short_descr); /* OLC */
    str_replace_dup (&mob->long_descr,  mob_index->long_descr);  /* OLC */
    str_replace_dup (&mob->description, mob_index->description); /* OLC */
    mob->id           = get_mob_id ();
    mob->spec_fun     = mob_index->spec_fun;
    mob->prompt       = NULL;
    mob->mprog_target = NULL;
    mob->class        = CLASS_NONE;

    if (mob_index->wealth == 0) {
        mob->silver = 0;
        mob->gold = 0;
    }
    else {
        long wealth;
        wealth = number_range (mob_index->wealth / 2, 3 * mob_index->wealth / 2);
        mob->gold = number_range (wealth / 200, wealth / 100);
        mob->silver = wealth - (mob->gold * 100);
    }

    /* load in new style */
    if (mob_index->new_format) {
        /* read from prototype */
        mob->group    = mob_index->group;
        mob->ext_mob  = mob_index->ext_mob_final;
        mob->plr      = 0;
        mob->comm     = COMM_NOCHANNELS | COMM_NOSHOUT | COMM_NOTELL;
        mob->affected_by = mob_index->affected_by_final;
        mob->alignment = mob_index->alignment;
        mob->level    = mob_index->level;
        mob->hitroll  = mob_index->hitroll;
        mob->damroll  = mob_index->damage.bonus;
        mob->max_hit  = dice (mob_index->hit.number, mob_index->hit.size)
            + mob_index->hit.bonus;
        mob->hit      = mob->max_hit;
        mob->max_mana = dice (mob_index->mana.number, mob_index->mana.size)
            + mob_index->mana.bonus;
        mob->mana     = mob->max_mana;
        mob->damage   = mob_index->damage;
        mob->damage.bonus = 0;
        mob->attack_type = mob_index->attack_type;

        if (mob->attack_type == 0) {
            switch (number_range (1, 3)) {
                case 1: mob->attack_type = ATTACK_SLASH;  break;
                case 2: mob->attack_type = ATTACK_POUND;  break;
                case 3: mob->attack_type = ATTACK_PIERCE; break;
            }
        }
        for (i = 0; i < 4; i++)
            mob->armor[i] = mob_index->ac[i];
        mob->off_flags   = mob_index->off_flags_final;
        mob->imm_flags   = mob_index->imm_flags_final;
        mob->res_flags   = mob_index->res_flags_final;
        mob->vuln_flags  = mob_index->vuln_flags_final;
        mob->start_pos   = mob_index->start_pos;
        mob->default_pos = mob_index->default_pos;

        mob->sex = mob_index->sex;
        if (mob->sex == 3) /* random sex */
            mob->sex = number_range (1, 2);
        mob->race     = mob_index->race;
        mob->form     = mob_index->form_final;
        mob->parts    = mob_index->parts_final;
        mob->size     = mob_index->size;
        mob->material = mob_index->material;

        /* computed on the spot */
        for (i = 0; i < STAT_MAX; i++)
            mob->perm_stat[i] = UMIN (25, 11 + mob->level / 4);

        if (EXT_IS_SET (mob->ext_mob, MOB_WARRIOR)) {
            mob->perm_stat[STAT_STR] += 3;
            mob->perm_stat[STAT_INT] -= 1;
            mob->perm_stat[STAT_CON] += 2;
        }
        if (EXT_IS_SET (mob->ext_mob, MOB_THIEF)) {
            mob->perm_stat[STAT_DEX] += 3;
            mob->perm_stat[STAT_INT] += 1;
            mob->perm_stat[STAT_WIS] -= 1;
        }
        if (EXT_IS_SET (mob->ext_mob, MOB_CLERIC)) {
            mob->perm_stat[STAT_WIS] += 3;
            mob->perm_stat[STAT_DEX] -= 1;
            mob->perm_stat[STAT_STR] += 1;
        }
        if (EXT_IS_SET (mob->ext_mob, MOB_MAGE)) {
            mob->perm_stat[STAT_INT] += 3;
            mob->perm_stat[STAT_STR] -= 1;
            mob->perm_stat[STAT_DEX] += 1;
        }
        if (IS_SET (mob->off_flags, OFF_FAST))
            mob->perm_stat[STAT_DEX] += 2;

        mob->perm_stat[STAT_STR] += mob->size - SIZE_MEDIUM;
        mob->perm_stat[STAT_CON] += (mob->size - SIZE_MEDIUM) / 2;

        /* let's get some spell action */
        if (IS_AFFECTED (mob, AFF_SANCTUARY)) {
            affect_init (&af, AFF_TO_AFFECTS, skill_lookup_exact ("sanctuary"), mob->level, -1, APPLY_NONE, 0, AFF_SANCTUARY);
            affect_to_char (mob, &af);
        }
        if (IS_AFFECTED (mob, AFF_HASTE)) {
            affect_init (&af, AFF_TO_AFFECTS, skill_lookup_exact ("haste"), mob->level, -1, APPLY_DEX, 1 + (mob->level >= 18) + (mob->level >= 25) + (mob->level >= 32), AFF_HASTE);
            affect_to_char (mob, &af);
        }
        if (IS_AFFECTED (mob, AFF_PROTECT_EVIL)) {
            affect_init (&af, AFF_TO_AFFECTS, skill_lookup_exact ("protection evil"), mob->level, -1, APPLY_SAVES, -1, AFF_PROTECT_EVIL);
            affect_to_char (mob, &af);
        }
        if (IS_AFFECTED (mob, AFF_PROTECT_GOOD)) {
            affect_init (&af, AFF_TO_AFFECTS, skill_lookup_exact ("protection good"), mob->level, -1, APPLY_SAVES, -1, AFF_PROTECT_GOOD);
            affect_to_char (mob, &af);
        }
    }
    /* read in old format and convert */
    else {
        mob->ext_mob     = mob_index->ext_mob_final;
        mob->plr         = 0;
        mob->affected_by = mob_index->affected_by_final;
        mob->alignment   = mob_index->alignment;
        mob->level       = mob_index->level;
        mob->hitroll     = mob_index->hitroll;
        mob->damroll     = 0;
        mob->max_hit     = (mob->level * 8 + number_range
            (mob->level * mob->level / 4, mob->level * mob->level)) * 9 / 10;
        mob->hit         = mob->max_hit;
        mob->max_mana    = 100 + dice (mob->level, 10);
        mob->mana        = mob->max_mana;
        switch (number_range (1, 3)) {
            case 1: mob->attack_type = ATTACK_SLASH;  break;
            case 2: mob->attack_type = ATTACK_POUND;  break;
            case 3: mob->attack_type = ATTACK_PIERCE; break;
        }
        for (i = 0; i < 3; i++)
            mob->armor[i] = int_interpolate (mob->level, 100, -100);
        mob->armor[3]    = int_interpolate (mob->level, 100, 0);
        mob->race        = mob_index->race;
        mob->off_flags   = mob_index->off_flags_final;
        mob->imm_flags   = mob_index->imm_flags_final;
        mob->res_flags   = mob_index->res_flags_final;
        mob->vuln_flags  = mob_index->vuln_flags_final;
        mob->start_pos   = mob_index->start_pos;
        mob->default_pos = mob_index->default_pos;
        mob->sex         = mob_index->sex;
        mob->form        = mob_index->form_final;
        mob->parts       = mob_index->parts_final;
        mob->size        = SIZE_MEDIUM;

        for (i = 0; i < STAT_MAX; i++)
            mob->perm_stat[i] = 11 + mob->level / 4;
    }
    mob->position = mob->start_pos;

    /* link the mob to the world list */
    LIST_FRONT (mob, next, char_list);
    mob_index->count++;
    return mob;
}

/* duplicate a mobile exactly -- except inventory */
void mobile_clone (CHAR_T *parent, CHAR_T *clone) {
    int i;
    AFFECT_T *paf;

    if (parent == NULL || clone == NULL || !IS_NPC (parent))
        return;

    /* start fixing values */
    str_replace_dup (&clone->name, parent->name);
    clone->version     = parent->version;
    str_replace_dup (&clone->short_descr, parent->short_descr);
    str_replace_dup (&clone->long_descr,  parent->long_descr);
    str_replace_dup (&clone->description, parent->description);
    clone->group       = parent->group;
    clone->sex         = parent->sex;
    clone->class       = parent->class;
    clone->race        = parent->race;
    clone->level       = parent->level;
    clone->trust       = 0;
    clone->timer       = parent->timer;
    clone->wait        = parent->wait;
    clone->hit         = parent->hit;
    clone->max_hit     = parent->max_hit;
    clone->mana        = parent->mana;
    clone->max_mana    = parent->max_mana;
    clone->move        = parent->move;
    clone->max_move    = parent->max_move;
    clone->gold        = parent->gold;
    clone->silver      = parent->silver;
    clone->exp         = parent->exp;
    clone->ext_mob     = parent->ext_mob;
    clone->plr         = parent->plr;
    clone->comm        = parent->comm;
    clone->imm_flags   = parent->imm_flags;
    clone->res_flags   = parent->res_flags;
    clone->vuln_flags  = parent->vuln_flags;
    clone->invis_level = parent->invis_level;
    clone->affected_by = parent->affected_by;
    clone->position    = parent->position;
    clone->practice    = parent->practice;
    clone->train       = parent->train;
    clone->saving_throw = parent->saving_throw;
    clone->alignment   = parent->alignment;
    clone->hitroll     = parent->hitroll;
    clone->damroll     = parent->damroll;
    clone->wimpy       = parent->wimpy;
    clone->form        = parent->form;
    clone->parts       = parent->parts;
    clone->size        = parent->size;
    clone->material    = parent->material;
    clone->off_flags   = parent->off_flags;
    clone->attack_type = parent->attack_type;
    clone->start_pos   = parent->start_pos;
    clone->default_pos = parent->default_pos;
    clone->spec_fun    = parent->spec_fun;
    clone->damage      = parent->damage;

    for (i = 0; i < 4; i++)
        clone->armor[i] = parent->armor[i];

    for (i = 0; i < STAT_MAX; i++) {
        clone->perm_stat[i] = parent->perm_stat[i];
        clone->mod_stat[i]  = parent->mod_stat[i];
    }

    /* now add the affects */
    for (paf = parent->affected; paf != NULL; paf = paf->next)
        affect_to_char (clone, paf);
}

int mobile_should_assist_player (CHAR_T *bystander, CHAR_T *player,
    CHAR_T *victim)
{
    if (!IS_NPC (bystander))
        return 0;
    if (IS_NPC (player))
        return 0;
    if (!IS_SET (bystander->off_flags, ASSIST_PLAYERS))
        return 0;

    /* bystander shouldn't assist any players fighting
     * something more than 5 levels above it. */
    if (bystander->level + 5 < victim->level)
        return 0;

    return 1;
}

bool mobile_should_assist_attacker (CHAR_T *bystander, CHAR_T *attacker,
    CHAR_T *victim)
{
    if (!IS_NPC (bystander))
        return FALSE;

    /* assist group, everyone, race, or like-aligned. */
    if (bystander->group && bystander->group == attacker->group)
        return TRUE;
    if (IS_SET (bystander->off_flags, ASSIST_ALL))
        return TRUE;
    if (IS_SET (bystander->off_flags, ASSIST_RACE) &&
            bystander->race == attacker->race)
        return TRUE;
    if (IS_SET (bystander->off_flags, ASSIST_ALIGN) &&
            IS_SAME_ALIGN (bystander, attacker))
        return TRUE;

    /* programmed to assist a specific vnum? */
    if (bystander->index_data == attacker->index_data &&
            IS_SET (bystander->off_flags, ASSIST_VNUM))
        return TRUE;

    return FALSE;
}

/* procedure for all mobile attacks */
void mobile_hit (CHAR_T *ch, CHAR_T *victim, int dt) {
    int chance, number;
    CHAR_T *vch, *vch_next;

    one_hit (ch, victim, dt);
    if (ch->fighting != victim)
        return;

    /* Area attack -- BALLS nasty! */
    if (IS_SET (ch->off_flags, OFF_AREA_ATTACK)) {
        for (vch = ch->in_room->people; vch != NULL; vch = vch_next) {
            vch_next = vch->next;
            if ((vch != victim && vch->fighting == ch))
                one_hit (ch, vch, dt);
        }
    }

    if (IS_AFFECTED (ch, AFF_HASTE) ||
        (IS_SET (ch->off_flags, OFF_FAST) && !IS_AFFECTED (ch, AFF_SLOW)))
    {
        one_hit (ch, victim, dt);
    }
    if (ch->fighting != victim || dt == SN(BACKSTAB))
        return;

    chance = char_get_skill (ch, SN(SECOND_ATTACK)) / 2;
    if (IS_AFFECTED (ch, AFF_SLOW) && !IS_SET (ch->off_flags, OFF_FAST))
        chance /= 2;

    if (number_percent () < chance) {
        one_hit (ch, victim, dt);
        if (ch->fighting != victim)
            return;
    }

    chance = char_get_skill (ch, SN(THIRD_ATTACK)) / 4;
    if (IS_AFFECTED (ch, AFF_SLOW) && !IS_SET (ch->off_flags, OFF_FAST))
        chance = 0;

    if (number_percent () < chance) {
        one_hit (ch, victim, dt);
        if (ch->fighting != victim)
            return;
    }

    /* oh boy!  Fun stuff! */
    if (ch->wait > 0 || ch->position < POS_FIGHTING)
        return;

    /* now for the skills */
    number = number_range (0, 8);
    switch (number) {
        case 0:
            if (IS_SET (ch->off_flags, OFF_BASH))
                do_function (ch, &do_bash, "");
            break;

        case 1:
            if (IS_SET (ch->off_flags, OFF_BERSERK)
                && !IS_AFFECTED (ch, AFF_BERSERK))
                do_function (ch, &do_berserk, "");
            break;

        case 2:
            if (IS_SET (ch->off_flags, OFF_DISARM) ||
                (char_get_weapon_sn (ch) != SN(HAND_TO_HAND) &&
                 (EXT_IS_SET (ch->ext_mob, MOB_WARRIOR) ||
                  EXT_IS_SET (ch->ext_mob, MOB_THIEF))))
                do_function (ch, &do_disarm, "");
            break;

        case 3:
            if (IS_SET (ch->off_flags, OFF_KICK))
                do_function (ch, &do_kick, "");
            break;

        case 4:
            if (IS_SET (ch->off_flags, OFF_KICK_DIRT))
                do_function (ch, &do_dirt, "");
            break;

        case 5:
            if (IS_SET (ch->off_flags, OFF_TAIL))
                ; /* do_function(ch, &do_tail, "") */
            break;

        case 6:
            if (IS_SET (ch->off_flags, OFF_TRIP))
                do_function (ch, &do_trip, "");
            break;

        case 7:
            if (IS_SET (ch->off_flags, OFF_CRUSH))
                ; /* do_function(ch, &do_crush, "") */
            break;

        case 8:
            if (IS_SET (ch->off_flags, OFF_BACKSTAB))
                do_function (ch, &do_backstab, "");
            break;
    }
}

int mobile_get_skill_learned (const CHAR_T *ch, int sn) {
    const SKILL_T *skill = skill_get (sn);
    if (skill == NULL)
        return -1;

    /* TODO: this should probably be a table of some sort... */
    if (skill->spell_fun != spell_null)
        return 40 + 2 * ch->level;
    else if (sn == SN(SNEAK) || sn == SN(HIDE))
        return ch->level * 2 + 20;
    else if ((sn == SN(DODGE) && IS_SET (ch->off_flags, OFF_DODGE)) ||
             (sn == SN(PARRY) && IS_SET (ch->off_flags, OFF_PARRY)))
        return ch->level * 2;
    else if (sn == SN(SHIELD_BLOCK))
        return 10 + 2 * ch->level;
    else if (sn == SN(SECOND_ATTACK) && (EXT_IS_SET (ch->ext_mob, MOB_WARRIOR) ||
                                         EXT_IS_SET (ch->ext_mob, MOB_THIEF)))
        return 10 + 3 * ch->level;
    else if (sn == SN(THIRD_ATTACK) && EXT_IS_SET (ch->ext_mob, MOB_WARRIOR))
        return 4 * ch->level - 40;
    else if (sn == SN(HAND_TO_HAND))
        return 40 + 2 * ch->level;
    else if (sn == SN(TRIP) && IS_SET (ch->off_flags, OFF_TRIP))
        return 10 + 3 * ch->level;
    else if (sn == SN(BASH) && IS_SET (ch->off_flags, OFF_BASH))
        return 10 + 3 * ch->level;
    else if (sn == SN(DISARM) && (IS_SET (ch->off_flags, OFF_DISARM) ||
                                  EXT_IS_SET (ch->ext_mob, MOB_WARRIOR) ||
                                  EXT_IS_SET (ch->ext_mob, MOB_THIEF)))
        return 20 + 3 * ch->level;
    else if (sn == SN(BERSERK) && IS_SET (ch->off_flags, OFF_BERSERK))
        return 3 * ch->level;
    else if (sn == SN(KICK))
        return 10 + 3 * ch->level;
    else if (sn == SN(BACKSTAB) && EXT_IS_SET (ch->ext_mob, MOB_THIEF))
        return 20 + 2 * ch->level;
    else if (sn == SN(RESCUE))
        return 40 + ch->level;
    else if (sn == SN(RECALL))
        return 40 + ch->level;
    else if (sn == SN(SWORD) || sn == SN(DAGGER) || sn == SN(SPEAR) ||
             sn == SN(MACE)  || sn == SN(AXE)    || sn == SN(FLAIL) ||
             sn == SN(WHIP)  || sn == SN(POLEARM))
        return 40 + 5 * ch->level / 2;
    else
        return 0;
}

bool mobile_is_friendly (const CHAR_T *ch) {
    return IS_NPC (ch) && (
        EXT_IS_SET (ch->ext_mob, MOB_TRAIN)     ||
        EXT_IS_SET (ch->ext_mob, MOB_PRACTICE)  ||
        EXT_IS_SET (ch->ext_mob, MOB_IS_HEALER) ||
        EXT_IS_SET (ch->ext_mob, MOB_IS_CHANGER)
    );
}
