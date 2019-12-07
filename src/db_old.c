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
 *   ROM 2.4 is copyright 1993-1998 Russ Taylor                            *
 *   ROM has been brought to you by the ROM consortium                     *
 *       Russ Taylor (rtaylor@hypercube.org)                               *
 *       Gabrielle Taylor (gtaylor@hypercube.org)                          *
 *       Brian Moore (zump@rom.org)                                        *
 *   By using this code, you have agreed to follow the terms of the        *
 *   ROM license, in the file Rom24/doc/rom.license                        *
 ***************************************************************************/

#include <string.h>
#include <stdlib.h>

#include "db.h"
#include "recycle.h"
#include "utils.h"
#include "interp.h"
#include "lookup.h"
#include "olc.h"
#include "affects.h"
#include "globals.h"
#include "memory.h"

#include "db_old.h"

/* Snarf a mob section.  old style */
void load_old_mob (FILE *fp) {
    MOB_INDEX_T *mob_index;
    /* for race updating */
    int race;
    char name[MAX_STRING_LENGTH];

    EXIT_IF_BUG (!area_last, /* OLC */
        "load_mobiles: no #AREA seen yet.", 0);

    while (1) {
        sh_int vnum;
        char letter;

        letter = fread_letter (fp);
        EXIT_IF_BUG (letter != '#',
            "load_mobiles: # not found.", 0);

        vnum = fread_number (fp);
        if (vnum == 0)
            break;

        in_boot_db = FALSE;
        EXIT_IF_BUG (get_mob_index (vnum) != NULL,
            "load_mobiles: vnum %d duplicated.", vnum);
        in_boot_db = TRUE;

        mob_index = mob_index_new ();
        mob_index->area = area_last;    /* OLC */
        mob_index->vnum = vnum;
        mob_index->anum = vnum - area_last->min_vnum;
        mob_index->new_format = FALSE;
        str_replace_dup (&mob_index->name,        fread_string (fp));
        str_replace_dup (&mob_index->short_descr, fread_string (fp));
        str_replace_dup (&mob_index->long_descr,  fread_string (fp));
        str_replace_dup (&mob_index->description, fread_string (fp));

        mob_index->long_descr[0]  = UPPER (mob_index->long_descr[0]);
        mob_index->description[0] = UPPER (mob_index->description[0]);

        mob_index->mob_plus         = fread_flag (fp);
        mob_index->affected_by_plus = fread_flag (fp);
        mob_index->shop             = NULL;
        mob_index->alignment        = fread_number (fp);
        letter                      = fread_letter (fp);
        mob_index->level            = fread_number (fp);

        /*
         * The unused stuff is for imps who want to use the old-style
         * stats-in-files method.
         */
        fread_number (fp);        /* Unused */
        fread_number (fp);        /* Unused */
        fread_number (fp);        /* Unused */
        /* 'd'      */ fread_letter (fp);
        /* Unused */
        fread_number (fp);        /* Unused */
        /* '+'      */ fread_letter (fp);
        /* Unused */
        fread_number (fp);        /* Unused */
        fread_number (fp);        /* Unused */
        /* 'd'      */ fread_letter (fp);
        /* Unused */
        fread_number (fp);        /* Unused */
        /* '+'      */ fread_letter (fp);
        /* Unused */
        fread_number (fp);        /* Unused */
        mob_index->wealth = fread_number (fp) / 20;
        /* xp can't be used! */ fread_number (fp);
        /* Unused */
        mob_index->start_pos = fread_number (fp);    /* Unused */
        mob_index->default_pos = fread_number (fp);    /* Unused */

        if (mob_index->start_pos < POS_SLEEPING)
            mob_index->start_pos = POS_STANDING;
        if (mob_index->default_pos < POS_SLEEPING)
            mob_index->default_pos = POS_STANDING;

        /* Back to meaningful values. */
        mob_index->sex = fread_number (fp);

        /* compute the race BS */
        one_argument (mob_index->name, name);
        if (name[0] == '\0' || (race = race_lookup_exact (name)) < 0) {
            if (name[0] != '\0')
                bugf ("Unknown race '%s'", name);

            /* fill in with blanks */
            mob_index->race = race_lookup_exact ("human");
            mob_index->off_flags_plus =
                OFF_DODGE | OFF_DISARM | OFF_TRIP | ASSIST_VNUM;
            mob_index->imm_flags_plus  = 0;
            mob_index->res_flags_plus  = 0;
            mob_index->vuln_flags_plus = 0;
            mob_index->form_plus =
                FORM_EDIBLE | FORM_SENTIENT | FORM_BIPED | FORM_MAMMAL;
            mob_index->parts_plus =
                PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
                PART_GUTS;
        }
        else {
            mob_index->race = race;
            mob_index->off_flags_plus =
                OFF_DODGE | OFF_DISARM | OFF_TRIP | ASSIST_RACE |
                race_table[race].off;
            mob_index->imm_flags_plus  = race_table[race].imm;
            mob_index->res_flags_plus  = race_table[race].res;
            mob_index->vuln_flags_plus = race_table[race].vuln;
            mob_index->form_plus       = race_table[race].form;
            mob_index->parts_plus      = race_table[race].parts;
        }

        EXIT_IF_BUG (letter != 'S',
            "load_mobiles: vnum %d non-S.", vnum);

        convert_mobile (mob_index); /* ROM OLC */
        db_finalize_mob (mob_index);

        /* Our mob is done loading - register it. */
        db_register_new_mob (mob_index);
    }
}

/* Snarf an obj section.  old style */
void load_old_obj (FILE *fp) {
    OBJ_INDEX_T *obj_index;

    EXIT_IF_BUG (!area_last, /* OLC */
        "load_objects: no #AREA seen yet.", 0);

    while (1) {
        sh_int vnum;
        char letter;

        letter = fread_letter (fp);
        EXIT_IF_BUG (letter != '#',
            "load_objects: # not found.", 0);

        vnum = fread_number (fp);
        if (vnum == 0)
            break;

        in_boot_db = FALSE;
        EXIT_IF_BUG (get_obj_index (vnum) != NULL,
            "load_objects: vnum %d duplicated.", vnum);
        in_boot_db = TRUE;

        obj_index = obj_index_new ();
        obj_index->area = area_last; /* OLC */
        obj_index->vnum = vnum;
        obj_index->anum = vnum - area_last->min_vnum;
        obj_index->new_format = FALSE;
        obj_index->reset_num = 0;
        str_replace_dup (&obj_index->name,        fread_string (fp));
        str_replace_dup (&obj_index->short_descr, fread_string (fp));
        str_replace_dup (&obj_index->description, fread_string (fp));
        /* Action description */ fread_string (fp);

        obj_index->short_descr[0] = LOWER (obj_index->short_descr[0]);
        obj_index->description[0] = UPPER (obj_index->description[0]);

        obj_index->item_type = fread_number (fp);
        obj_index->extra_flags = fread_flag (fp);
        obj_index->wear_flags = fread_flag (fp);
        obj_index->v.value[0] = fread_number (fp);
        obj_index->v.value[1] = fread_number (fp);
        obj_index->v.value[2] = fread_number (fp);
        obj_index->v.value[3] = fread_number (fp);
        obj_index->v.value[4] = 0;
        obj_index->level = 0;
        obj_index->condition = 100;
        obj_index->weight = fread_number (fp);
        obj_index->cost = fread_number (fp);
        fread_number (fp); /* Cost per day? Unused? */

        if (obj_index->item_type == ITEM_WEAPON) {
            if (is_name ("two",        obj_index->name) ||
                is_name ("two-handed", obj_index->name) ||
                is_name ("claymore",   obj_index->name))
            {
                SET_BIT (obj_index->v.weapon.flags, WEAPON_TWO_HANDS);
            }
        }

        while (1) {
            char letter = fread_letter (fp);
            if (letter == 'A') {
                AFFECT_T *paf = affect_new ();
                sh_int duration, modifier;

                duration = fread_number (fp);
                modifier = fread_number (fp);
                affect_init (paf, AFF_TO_OBJECT, -1, 20, -1, duration, modifier, 0);
                LIST_BACK (paf, next, obj_index->affected, AFFECT_T);
            }
            else if (letter == 'E') {
                EXTRA_DESCR_T *ed = extra_descr_new ();
                str_replace_dup (&ed->keyword, fread_string (fp));
                str_replace_dup (&ed->description, fread_string (fp));
                LIST_BACK (ed, next, obj_index->extra_descr, EXTRA_DESCR_T);
            }
            else {
                ungetc (letter, fp);
                break;
            }
        }

        /* fix armors */
        if (obj_index->item_type == ITEM_ARMOR) {
            obj_index->v.armor.vs_pierce = obj_index->v.value[0];
            obj_index->v.armor.vs_bash   = obj_index->v.value[0];
            obj_index->v.armor.vs_slash  = obj_index->v.value[0];
        }

        /* Translate spell "slot numbers" to internal "skill numbers." */
        switch (obj_index->item_type) {
            case ITEM_PILL: {
                int i;
                for (i = 0; i < PILL_SKILL_MAX; i++)
                    obj_index->v.pill.skill[i] = skill_get_index_by_slot (
                        obj_index->v.value[i + 1]);
                break;
            }

            case ITEM_POTION: {
                int i;
                for (i = 0; i < POTION_SKILL_MAX; i++)
                    obj_index->v.potion.skill[i] = skill_get_index_by_slot (
                        obj_index->v.value[i + 1]);
                break;
            }

            case ITEM_SCROLL: {
                int i;
                for (i = 0; i < SCROLL_SKILL_MAX; i++)
                    obj_index->v.scroll.skill[i] = skill_get_index_by_slot (
                        obj_index->v.scroll.skill[i + 1]);
                break;
            }

            case ITEM_STAFF:
                obj_index->v.staff.skill = skill_get_index_by_slot (
                    obj_index->v.value[3]);
                break;

            case ITEM_WAND:
                obj_index->v.wand.skill = skill_get_index_by_slot (
                    obj_index->v.value[3]);
                break;
        }

        /* Post-processing for loaded objects. */
        db_finalize_obj (obj_index);

        /* Our object is done loading - register it. */
        db_register_new_obj (obj_index);
    }
}

static MOB_INDEX_T *convert_object_reset_mob = NULL;
static OBJ_INDEX_T *convert_object_reset_obj = NULL;

int convert_object_reset (RESET_T *reset) {
    RESET_VALUE_T *v = &(reset->v);

    #define MOB (convert_object_reset_mob)
    #define OBJ (convert_object_reset_obj)

    switch (reset->command) {
        case 'M':
            RETURN_IF_BUG (!(MOB = get_mob_index (v->mob.mob_vnum)),
                "convert_object_reset: 'M': bad vnum %d.", v->mob.mob_vnum, 0);
            break;

        case 'O':
            RETURN_IF_BUG (!(OBJ = get_obj_index (v->obj.obj_vnum)),
                "convert_object_reset: 'O': bad vnum %d.", v->obj.obj_vnum, 0);
            if (OBJ->new_format)
                return 1;

            RETURN_IF_BUG (!MOB,
                "convert_object_reset: 'O': No mob reset yet.", 0, 0);
            OBJ->level = OBJ->level < 1 ? MOB->level - 2
                : UMIN (OBJ->level, MOB->level - 2);
            break;

        case 'P': {
            OBJ_INDEX_T *OBJ, *OBJTo;
            RETURN_IF_BUG (!(OBJ = get_obj_index (v->put.obj_vnum)),
                "convert_object_reset: 'P': bad vnum %d.", v->put.obj_vnum, 0);
            if (OBJ->new_format)
                return 1;

            RETURN_IF_BUG (!(OBJTo = get_obj_index (v->put.into_vnum)),
                "convert_object_reset: 'P': bad vnum %d.", v->put.into_vnum, 0);
            OBJ->level = OBJ->level < 1 ? OBJTo->level
                : UMIN (OBJ->level, OBJTo->level);
            break;
        }

        case 'G':
        case 'E': {
            int obj_vnum = (reset->command == 'G')
                ? v->give.obj_vnum : v->equip.obj_vnum;

            RETURN_IF_BUG (!(OBJ = get_obj_index (obj_vnum)),
                "convert_object_reset: 'E' or 'G': bad vnum %d.", obj_vnum, 0);
            RETURN_IF_BUG (!MOB,
                "convert_object_reset: 'E' or 'G': null mob for vnum %d.", obj_vnum, 0);
            if (OBJ->new_format)
                return 1;

            if (MOB->shop) {
                switch (OBJ->item_type) {
                    default:
                        OBJ->level = UMAX (0, OBJ->level);
                        break;
                    case ITEM_PILL:
                    case ITEM_POTION:
                        OBJ->level = UMAX (5, OBJ->level);
                        break;
                    case ITEM_SCROLL:
                    case ITEM_ARMOR:
                    case ITEM_WEAPON:
                        OBJ->level = UMAX (10, OBJ->level);
                        break;
                    case ITEM_WAND:
                    case ITEM_TREASURE:
                        OBJ->level = UMAX (15, OBJ->level);
                        break;
                    case ITEM_STAFF:
                        OBJ->level = UMAX (20, OBJ->level);
                        break;
                }
            }
            else
                OBJ->level = OBJ->level < 1 ? MOB->level
                    : UMIN (OBJ->level, MOB->level);
            break;
        }

        default:
            bug ("convert_object_reset: '%c': unknown command.", reset->command);
            return 0;
    }

    #undef MOB
    #undef OBJ
    return 1;
}

/*****************************************************************************
 Name:       convert_objects
 Purpose:    Converts all old format objects to new format
 Called by:  boot_db (db.c).
 Note:       Loops over all resets to find the level of the mob
             loaded before the object to determine the level of
             the object.
             It might be better to update the levels in load_resets().
             This function is not pretty.. Sorry about that :)
             ^^^ Fixed! -- Synival
 Author:        Hugin
 ****************************************************************************/
void convert_objects (void) {
    int vnum;
    AREA_T *area;
    ROOM_INDEX_T *room;
    RESET_T *reset;
    OBJ_INDEX_T *obj;

    if (newobjs == TOP (RECYCLE_OBJ_INDEX_T))
        return; /* all objects in new format */

    convert_object_reset_mob = NULL;
    convert_object_reset_obj = NULL;

    for (area = area_first; area; area = area->next) {
        for (vnum = area->min_vnum; vnum <= area->max_vnum; vnum++) {
            if (!(room = get_room_index (vnum)))
                continue;
            for (reset = room->reset_first; reset; reset = reset->next)
                convert_object_reset (reset);
        }
    }

    /* do the conversion: */
    for (obj = obj_index_get_first(); obj; obj = obj_index_get_next (obj))
        if (!obj->new_format)
            convert_object (obj);
}

/*****************************************************************************
 Name:       convert_object
 Purpose:    Converts an old_format obj to new_format
 Called by:  convert_objects (db_old.c).
 Note:       Dug out of create_obj (db.c)
 Author:     Hugin
 ****************************************************************************/
void convert_object (OBJ_INDEX_T *obj_index) {
    int level;
    int number, type;            /* for dice-conversion */

    if (!obj_index || obj_index->new_format)
        return;

    level = obj_index->level;

    obj_index->level = UMAX (0, obj_index->level);    /* just to be sure */
    obj_index->cost = 10 * level;

    switch (obj_index->item_type) {
        default:
            bug ("obj_convert: vnum %d bad type.", obj_index->item_type);
            break;

        case ITEM_LIGHT:
        case ITEM_TREASURE:
        case ITEM_FURNITURE:
        case ITEM_TRASH:
        case ITEM_CONTAINER:
        case ITEM_DRINK_CON:
        case ITEM_KEY:
        case ITEM_FOOD:
        case ITEM_BOAT:
        case ITEM_CORPSE_NPC:
        case ITEM_CORPSE_PC:
        case ITEM_FOUNTAIN:
        case ITEM_MAP:
        case ITEM_CLOTHING:
        case ITEM_SCROLL:
            break;

        case ITEM_WAND:
            obj_index->v.wand.charges = obj_index->v.value[1];
            break;

        case ITEM_STAFF:
            obj_index->v.staff.charges = obj_index->v.value[1];
            break;

        case ITEM_WEAPON:
            /*
             * The conversion below is based on the values generated
             * in one_hit() (fight.c).  Since I don't want a lvl 50
             * weapon to do 15d3 damage, the min value will be below
             * the one in one_hit, and to make up for it, I've made
             * the max value higher.
             * (I don't want 15d2 because this will hardly ever roll
             * 15 or 30, it will only roll damage close to 23.
             * I can't do 4d8+11, because one_hit there is no dice-
             * bounus value to set...)
             *
             * The conversion below gives:

             level:   dice      min      max      mean
              1:     1d8      1( 2)    8( 7)     5( 5)
              2:     2d5      2( 3)   10( 8)     6( 6)
              3:     2d5      2( 3)   10( 8)     6( 6)
              5:     2d6      2( 3)   12(10)     7( 7)
             10:     4d5      4( 5)   20(14)    12(10)
             20:     5d5      5( 7)   25(21)    15(14)
             30:     5d7      5(10)   35(29)    20(20)
             50:     5d11     5(15)   55(44)    30(30)

             */

            number = UMIN (level / 4 + 1, 5);
            type = (level + 7) / number;

            obj_index->v.weapon.dice_num  = number;
            obj_index->v.weapon.dice_size = type;
            break;

        case ITEM_ARMOR:
            obj_index->v.armor.vs_pierce = level / 5 + 3;
            obj_index->v.armor.vs_bash   = obj_index->v.value[0];
            obj_index->v.armor.vs_slash  = obj_index->v.value[0];
            break;

        case ITEM_POTION:
        case ITEM_PILL:
            break;

        case ITEM_MONEY:
            obj_index->v.money.silver = obj_index->cost;
            break;
    }

    obj_index->new_format = TRUE;
    ++newobjs;
}

/*****************************************************************************
 Name:       convert_mobile
 Purpose:    Converts an old_format mob into new_format
 Called by:  load_old_mob (db.c).
 Note:       Dug out of char_create_mobile (db.c)
 Author:     Hugin
 ****************************************************************************/
void convert_mobile (MOB_INDEX_T *mob_index) {
    int i;
    int type, number, bonus;
    int level;

    if (!mob_index || mob_index->new_format)
        return;

    level = mob_index->level;
    mob_index->mob_plus |= MOB_WARRIOR;

    /*
     * Calculate hit dice.  Gives close to the hitpoints
     * of old format mobs created with char_create_mobile() (chars.c)
     * A high number of dice makes for less variance in mobiles
     * hitpoints.
     * (might be a good idea to reduce the max number of dice)
     *
     * The conversion below gives:

     level:     dice         min         max        diff       mean
      1:       1d2+6       7(  7)     8(   8)     1(   1)     8(   8)
      2:       1d3+15     16( 15)    18(  18)     2(   3)    17(  17)
      3:       1d6+24     25( 24)    30(  30)     5(   6)    27(  27)
      5:      1d17+42     43( 42)    59(  59)    16(  17)    51(  51)
     10:      3d22+96     99( 95)   162( 162)    63(  67)   131(    )
     15:     5d30+161    166(159)   311( 311)   145( 150)   239(    )
     30:    10d61+416    426(419)  1026(1026)   600( 607)   726(    )
     50:    10d169+920   930(923)  2610(2610)  1680(1688)  1770(    )

     The values in parenthesis give the values generated in char_create_mobile.
     Diff = max - min.  Mean is the arithmetic mean.
     (hmm.. must be some roundoff error in my calculations.. smurfette got
     1d6+23 hp at level 3 ? -- anyway.. the values above should be
     approximately right..)
    */

    type   = level * level * 27 / 40;
    number = UMIN (type / 40 + 1, 10);    /* how do they get 11 ??? */
    type   = UMAX (2, type / number);
    bonus  = UMAX (0, level * (8 + level) * .9 - number * type);

    mob_index->hit.number = number;
    mob_index->hit.size   = type;
    mob_index->hit.bonus  = bonus;

    mob_index->mana.number = level;
    mob_index->mana.size   = 10;
    mob_index->mana.bonus  = 100;

    /* Calculate dam dice.  Gives close to the damage
     * of old format mobs in damage()  (fight.c) */
    type   = level * 7 / 4;
    number = UMIN (type / 8 + 1, 5);
    type   = UMAX (2, type / number);
    bonus  = UMAX (0, level * 9 / 4 - number * type);

    mob_index->damage.number = number;
    mob_index->damage.size   = type;
    mob_index->damage.bonus  = bonus;

    switch (number_range (1, 3)) {
        case (1): mob_index->dam_type = 3;  break; /* slash  */
        case (2): mob_index->dam_type = 7;  break; /* pound  */
        case (3): mob_index->dam_type = 11; break; /* pierce */
    }

    for (i = 0; i < 3; i++)
        mob_index->ac[i] = interpolate (level, 100, -100);
    mob_index->ac[3] = interpolate (level, 100, 0);    /* exotic */

    mob_index->wealth /= 100;
    mob_index->size = SIZE_MEDIUM;

    mob_index->material = MATERIAL_GENERIC;
    mob_index->new_format = TRUE;

    ++newmobs;
}
