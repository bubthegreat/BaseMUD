

#include "utils.h"
#include "affects.h"
#include "comm.h"
#include "db.h"
#include "chars.h"
#include "globals.h"
#include "recycle.h"

#include "groups.h"

/* It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C */
bool is_same_group (const CHAR_T *ach, const CHAR_T *bch) {
    if (ach == NULL || bch == NULL)
        return FALSE;

    if (ach->leader != NULL)
        ach = ach->leader;
    if (bch->leader != NULL)
        bch = bch->leader;
    return ach == bch;
}

void add_follower (CHAR_T *ch, CHAR_T *master) {
    BAIL_IF_BUG (ch->master != NULL,
        "add_follower: non-null master.", 0);

    ch->master = master;
    ch->leader = NULL;

    if (char_can_see_in_room (master, ch))
        act ("$n now follows you.", ch, NULL, master, TO_VICT);
    act ("You now follow $N.", ch, NULL, master, TO_CHAR);
}

void stop_follower (CHAR_T *ch) {
    BAIL_IF_BUG (ch->master == NULL,
        "stop_follower: null master.", 0);

    if (IS_AFFECTED (ch, AFF_CHARM)) {
        REMOVE_BIT (ch->affected_by, AFF_CHARM);
        affect_strip_char (ch, SN(CHARM_PERSON));
    }

    if (ch->in_room != NULL) {
        if (char_can_see_in_room (ch->master, ch))
            act ("$n stops following you.", ch, NULL, ch->master, TO_VICT);
        act ("You stop following $N.", ch, NULL, ch->master, TO_CHAR);
    }
    if (ch->master->pet == ch)
        ch->master->pet = NULL;

    ch->master = NULL;
    ch->leader = NULL;
}

/* nukes charmed monsters and pets */
void nuke_pets (CHAR_T *ch) {
    CHAR_T *pet;

    if ((pet = ch->pet) != NULL) {
        stop_follower (pet);
        if (pet->in_room != NULL) {
            act ("$N slowly fades away.", ch, NULL, pet, TO_OTHERS);
            char_extract (pet);
        }
        else
            char_free (pet);
    }
    ch->pet = NULL;
}

void die_follower (CHAR_T *ch) {
    CHAR_T *fch;

    if (ch->master != NULL) {
        if (ch->master->pet == ch)
            ch->master->pet = NULL;
        stop_follower (ch);
    }

    ch->leader = NULL;
    for (fch = char_first; fch != NULL; fch = fch->global_next) {
        if (fch->master == ch)
            stop_follower (fch);
        if (fch->leader == ch)
            fch->leader = fch;
    }
}
