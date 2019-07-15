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
 **************************************************************************/

/***************************************************************************
 *   ROM 2.4 is copyright 1993-1998 Russ Taylor                            *
 *   ROM has been brought to you by the ROM consortium                     *
 *       Russ Taylor (rtaylor@hypercube.org)                               *
 *       Gabrielle Taylor (gtaylor@hypercube.org)                          *
 *       Brian Moore (zump@rom.org)                                        *
 *   By using this code, you have agreed to follow the terms of the        *
 *   ROM license, in the file Rom24/doc/rom.license                        *
 **************************************************************************/

#include <string.h>

#include "recycle.h"
#include "utils.h"
#include "comm.h"
#include "lookup.h"
#include "chars.h"

#include "affects.h"

/* TODO: functions to grab most affects from a table. */
/* TODO: maybe implement height + weight affects? */
/* TODO: maybe implement wood/silver/iron resistances? */
/* TODO: review function names and order */

/* for immunity, vulnerabiltiy, and resistant
   the 'globals' (magic and weapons) may be overriden
   three other cases -- wood, silver, and iron -- are checked in fight.c */
/* TODO: ^^^^ no they're not! */
int check_immune (CHAR_DATA * ch, int dam_type) {
    const DAM_TYPE *dam;
    int immune, def;
    flag_t bit;

    immune = -1;
    def = IS_NORMAL;

    if (dam_type == DAM_NONE)
        return immune;

    /* if a physical attack (bash, pierce, slash), check weapon resistance */
    /* TODO: magic number!! */
    if (dam_type <= 3) {
             if (IS_SET (ch->imm_flags,  RES_WEAPON)) def = IS_IMMUNE;
        else if (IS_SET (ch->res_flags,  RES_WEAPON)) def = IS_RESISTANT;
        else if (IS_SET (ch->vuln_flags, RES_WEAPON)) def = IS_VULNERABLE;
    }
    /* other, for magical attacks (anything else), check magic resistance */
    else {
             if (IS_SET (ch->imm_flags,  RES_MAGIC)) def = IS_IMMUNE;
        else if (IS_SET (ch->res_flags,  RES_MAGIC)) def = IS_RESISTANT;
        else if (IS_SET (ch->vuln_flags, RES_MAGIC)) def = IS_VULNERABLE;
    }

    /* check for resistance to the attack's specific resistance bit. */
    dam = dam_get (dam_type);
    if (dam == NULL || dam->res <= 0)
        return def;
    bit = dam->res;

    /* set immunity if that's the case... */
    if (IS_SET (ch->imm_flags, bit))
        immune = IS_IMMUNE;
    /* ...set resistance if not already immune... */
    else if (IS_SET (ch->res_flags, bit) && immune != IS_IMMUNE)
        immune = IS_RESISTANT;
    /* ...or set vulnerability by downgrading immunity. */
    else if (IS_SET (ch->vuln_flags, bit)) {
        if (immune == IS_IMMUNE)
            immune = IS_RESISTANT;
        else if (immune == IS_RESISTANT)
            immune = IS_NORMAL;
        else
            immune = IS_VULNERABLE;
    }

    /* return resistance-specific immunity or weapon/magic immunity. */
    return (immune == -1) ? def : immune;
}

/* enchanted stuff for eq */
void affect_enchant (OBJ_DATA * obj) {
    /* okay, move all the old flags into new vectors if we have to */
    if (!obj->enchanted) {
        AFFECT_DATA *paf, *af_new;
        obj->enchanted = TRUE;

        for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next) {
            af_new = affect_new ();
            affect_copy (af_new, paf);
            af_new->type = UMAX (0, af_new->type);
            LIST_FRONT (af_new, next, obj->affected);
        }
    }
}

void affect_modify_bits (CHAR_DATA * ch, AFFECT_DATA * paf, bool on) {
    if (on) {
        switch (paf->bit_type) {
            case TO_AFFECTS: SET_BIT (ch->affected_by, paf->bitvector); break;
            case TO_IMMUNE:  SET_BIT (ch->imm_flags,   paf->bitvector); break;
            case TO_RESIST:  SET_BIT (ch->res_flags,   paf->bitvector); break;
            case TO_VULN:    SET_BIT (ch->vuln_flags,  paf->bitvector); break;
        }
    }
    else {
        switch (paf->bit_type) {
            case TO_AFFECTS: REMOVE_BIT (ch->affected_by, paf->bitvector); break;
            case TO_IMMUNE:  REMOVE_BIT (ch->imm_flags,   paf->bitvector); break;
            case TO_RESIST:  REMOVE_BIT (ch->res_flags,   paf->bitvector); break;
            case TO_VULN:    REMOVE_BIT (ch->vuln_flags,  paf->bitvector); break;
        }
    }
}

void affect_modify_apply (CHAR_DATA * ch, AFFECT_DATA * paf, bool on) {
    int mod, i;

    mod = on ? (paf->modifier) : (0 - paf->modifier);
    switch (paf->apply) {
        case APPLY_NONE:          break;
        case APPLY_STR:           ch->mod_stat[STAT_STR] += mod; break;
        case APPLY_DEX:           ch->mod_stat[STAT_DEX] += mod; break;
        case APPLY_INT:           ch->mod_stat[STAT_INT] += mod; break;
        case APPLY_WIS:           ch->mod_stat[STAT_WIS] += mod; break;
        case APPLY_CON:           ch->mod_stat[STAT_CON] += mod; break;
        case APPLY_SEX:           ch->sex += mod; break;
        case APPLY_CLASS:         break;
        case APPLY_LEVEL:         break;
        case APPLY_AGE:           break;
        case APPLY_HEIGHT:        break; /* TODO: aww why not? */
        case APPLY_WEIGHT:        break; /* TODO: ditto */
        case APPLY_MANA:          ch->max_mana += mod; break;
        case APPLY_HIT:           ch->max_hit += mod; break;
        case APPLY_MOVE:          ch->max_move += mod; break;
        case APPLY_GOLD:          break;
        case APPLY_EXP:           break;
        case APPLY_HITROLL:       ch->hitroll += mod; break;
        case APPLY_DAMROLL:       ch->damroll += mod; break;
        case APPLY_SAVES:         ch->saving_throw += mod; break;
        case APPLY_SAVING_ROD:    ch->saving_throw += mod; break;
        case APPLY_SAVING_PETRI:  ch->saving_throw += mod; break;
        case APPLY_SAVING_BREATH: ch->saving_throw += mod; break;
        case APPLY_SAVING_SPELL:  ch->saving_throw += mod; break;
        case APPLY_SPELL_AFFECT:  break;

        case APPLY_AC:
            for (i = 0; i < 4; i++)
                ch->armor[i] += mod;
            break;

        default:
            bug ("affect_modify: unknown apply %d.", paf->apply);
            return;
    }
}

/* Apply or remove an affect to a character. */
void affect_modify (CHAR_DATA * ch, AFFECT_DATA * paf, bool on) {
    /* Modifiy various bits and other stats from 'apply'. */
    affect_modify_bits  (ch, paf, on);
    affect_modify_apply (ch, paf, on);

    /* Drop our weapon if it's suddenly too heavy. */
    /* TODO: this is a bit extreme to do here! */
    char_drop_weapon_if_too_heavy (ch);
}

/* find an effect in an affect list */
AFFECT_DATA *affect_find (AFFECT_DATA * paf, int sn) {
    AFFECT_DATA *paf_find;
    for (paf_find = paf; paf_find != NULL; paf_find = paf_find->next)
        if (paf_find->type == sn)
            return paf_find;
    return NULL;
}

/* fix object affects when removing one */
void affect_check (CHAR_DATA * ch, int bit_type, int vector) {
    AFFECT_DATA *paf;
    OBJ_DATA *obj;

    if (ch == NULL)
        return;
    if (bit_type == TO_OBJECT || bit_type == TO_WEAPON || vector == 0)
        return;

    for (paf = ch->affected; paf != NULL; paf = paf->next) {
        if (paf->bit_type == bit_type && paf->bitvector == vector) {
            affect_modify_bits (ch, paf, TRUE);
            return;
        }
    }

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
        if (obj->wear_loc == -1)
            continue;

        for (paf = obj->affected; paf != NULL; paf = paf->next)
            if (paf->bit_type == bit_type && paf->bitvector == vector)
                affect_modify_bits (ch, paf, TRUE);

        if (obj->enchanted)
            continue;
        for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
            if (paf->bit_type == bit_type && paf->bitvector == vector)
                affect_modify_bits (ch, paf, TRUE);
    }
}

void affect_copy (AFFECT_DATA *dest, AFFECT_DATA *src) {
    dest->bit_type  = src->bit_type;
    dest->type      = src->type;
    dest->level     = src->level;
    dest->duration  = src->duration;
    dest->apply     = src->apply;
    dest->modifier  = src->modifier;
    dest->bitvector = src->bitvector;
}

/* Give an affect to a char. */
void affect_to_char (CHAR_DATA *ch, AFFECT_DATA *paf) {
    AFFECT_DATA *paf_new;

    paf_new = affect_new ();
    affect_copy (paf_new, paf);
    LIST_FRONT (paf_new, next, ch->affected);

    affect_modify (ch, paf_new, TRUE);
    return;
}

/* give an affect to an object */
void affect_to_obj (OBJ_DATA *obj, AFFECT_DATA *paf) {
    AFFECT_DATA *paf_new;

    paf_new = affect_new ();
    affect_copy (paf_new, paf);
    LIST_FRONT (paf_new, next, obj->affected);

    /* apply any affect vectors to the object's extra_flags */
    if (paf->bitvector) {
        switch (paf->bit_type) {
            case TO_OBJECT:
                SET_BIT (obj->extra_flags, paf->bitvector);
                break;
            case TO_WEAPON:
                if (obj->item_type == ITEM_WEAPON)
                    SET_BIT (obj->value[4], paf->bitvector);
                break;
        }
    }
}

/* Remove an affect from a char. */
void affect_remove (CHAR_DATA * ch, AFFECT_DATA * paf) {
    int bit_type;
    flag_t vector;

    if (ch->affected == NULL) {
        bug ("affect_remove: no affect.", 0);
        return;
    }

    affect_modify (ch, paf, FALSE);
    bit_type = paf->bit_type;
    vector = paf->bitvector;

    LIST_REMOVE (paf, next, ch->affected, AFFECT_DATA, return);
    affect_free (paf);
    affect_check (ch, bit_type, vector);
}

void affect_remove_obj (OBJ_DATA * obj, AFFECT_DATA * paf) {
    int bit_type, vector;
    if (obj->affected == NULL) {
        bug ("affect_remove_object: no affect.", 0);
        return;
    }

    if (obj->carried_by != NULL && obj->wear_loc != -1)
        affect_modify (obj->carried_by, paf, FALSE);

    bit_type = paf->bit_type;
    vector = paf->bitvector;

    /* remove flags from the object if needed */
    if (paf->bitvector) {
        switch (paf->bit_type) {
            case TO_OBJECT:
                REMOVE_BIT (obj->extra_flags, paf->bitvector);
                break;
            case TO_WEAPON:
                if (obj->item_type == ITEM_WEAPON)
                    REMOVE_BIT (obj->value[4], paf->bitvector);
                break;
        }
    }

    LIST_REMOVE (paf, next, obj->affected, AFFECT_DATA, return);
    affect_free (paf);
    if (obj->carried_by != NULL && obj->wear_loc != -1)
        affect_check (obj->carried_by, bit_type, vector);
}

/* Strip all affects of a given sn. */
void affect_strip (CHAR_DATA * ch, int sn) {
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;

    for (paf = ch->affected; paf != NULL; paf = paf_next) {
        paf_next = paf->next;
        if (paf->type == sn)
            affect_remove (ch, paf);
    }
}

/* Return true if a char is affected by a spell. */
bool is_affected (CHAR_DATA * ch, int sn) {
    AFFECT_DATA *paf;
    for (paf = ch->affected; paf != NULL; paf = paf->next)
        if (paf->type == sn)
            return TRUE;
    return FALSE;
}

/* Add or enhance an affect. */
void affect_join (CHAR_DATA * ch, AFFECT_DATA * paf) {
    AFFECT_DATA *paf_old;
#ifndef VANILLA
    int min, max;
#endif

    for (paf_old = ch->affected; paf_old != NULL; paf_old = paf_old->next) {
        if (paf_old->type == paf->type) {
            paf->level = (paf->level + paf_old->level) / 2;
            paf->duration += paf_old->duration;

#ifndef VANILLA
            min = UMIN (paf->modifier, paf_old->modifier);
            max = UMAX (paf->modifier, paf_old->modifier);
            paf->modifier += paf_old->modifier;

            /* we can't be weaker or stronger than either one of the affects
             * we're merging.  otherwise, something like chill touch could
             * cause -10 str penalty or something nuts. */
            paf->modifier = UMAX (paf->modifier, min);
            paf->modifier = UMIN (paf->modifier, max);
#else
            paf->modifier += paf_old->modifier;
#endif

            affect_remove (ch, paf_old);
            break;
        }
    }
    affect_to_char (ch, paf);
}

void affect_init (AFFECT_DATA *af, sh_int bit_type, sh_int type, sh_int level,
    sh_int duration, sh_int apply, sh_int modifier, flag_t bitvector)
{
    memset (af, 0, sizeof (AFFECT_DATA));
    af->bit_type  = bit_type;
    af->type      = type;
    af->level     = level;
    af->duration  = duration;
    af->apply     = apply;
    af->modifier  = modifier;
    af->bitvector = bitvector;

    /* presumably, we aren't initializing invalidated
     * objects... but this could break something if we did! */
    af->rec_data.valid = TRUE;
}
