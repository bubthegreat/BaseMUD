

#include "magic.h"
#include "comm.h"
#include "utils.h"
#include "lookup.h"
#include "db.h"
#include "affects.h"
#include "objs.h"
#include "interp.h"
#include "chars.h"
#include "globals.h"
#include "items.h"

#include "spell_misc.h"

DEFINE_SPELL_FUN (spell_cancellation) {
    CHAR_T *victim = (CHAR_T *) vo;
    bool found = FALSE;

    level += 2;
    BAIL_IF (IS_NPC (ch) && !IS_NPC (victim),
        "You failed, try dispel magic.\n\r", ch);
    BAIL_IF (!IS_NPC (ch) && IS_NPC (victim) &&
             !(IS_AFFECTED (ch, AFF_CHARM) && ch->master == victim),
        "You failed, try dispel magic.\n\r", ch);

    /* unlike dispel magic, the victim gets NO save */

    /* begin running through the spells */
    found += check_dispel_quick (level, victim, "armor",           NULL);
    found += check_dispel_quick (level, victim, "bless",           NULL);
    found += check_dispel_quick (level, victim, "blindness",       "$n is no longer blinded.");
    found += check_dispel_quick (level, victim, "calm",            "$n no longer looks so peaceful...");
    found += check_dispel_quick (level, victim, "change sex",      "$n looks more like $mself again.");
    found += check_dispel_quick (level, victim, "charm person",    "$n regains $s free will.");
    found += check_dispel_quick (level, victim, "chill touch",     "$n looks warmer.");
    found += check_dispel_quick (level, victim, "curse",           NULL);
    found += check_dispel_quick (level, victim, "detect evil",     NULL);
    found += check_dispel_quick (level, victim, "detect good",     NULL);
    found += check_dispel_quick (level, victim, "detect hidden",   NULL);
    found += check_dispel_quick (level, victim, "detect invis",    NULL);
    found += check_dispel_quick (level, victim, "detect magic",    NULL);
    found += check_dispel_quick (level, victim, "faerie fire",     "$n's outline fades.");
    found += check_dispel_quick (level, victim, "fly",             "$n falls to the ground!");
    found += check_dispel_quick (level, victim, "frenzy",          "$n no longer looks so wild.");
    found += check_dispel_quick (level, victim, "giant strength",  "$n no longer looks so mighty.");
    found += check_dispel_quick (level, victim, "haste",           "$n is no longer moving so quickly.");
    found += check_dispel_quick (level, victim, "infravision",     NULL);
    found += check_dispel_quick (level, victim, "invis",           "$n fades into existance.");
    found += check_dispel_quick (level, victim, "mass invis",      "$n fades into existance.");
    found += check_dispel_quick (level, victim, "pass door",       NULL);
    found += check_dispel_quick (level, victim, "protection evil", NULL);
    found += check_dispel_quick (level, victim, "protection good", NULL);
    found += check_dispel_quick (level, victim, "sanctuary",       "The white aura around $n's body vanishes.");
    found += check_dispel_quick (level, victim, "shield",          "The shield protecting $n vanishes.");
    found += check_dispel_quick (level, victim, "sleep",           NULL);
    found += check_dispel_quick (level, victim, "slow",            "$n is no longer moving so slowly.");
    found += check_dispel_quick (level, victim, "stone skin",      "$n's skin regains its normal texture.");
    found += check_dispel_quick (level, victim, "weaken",          "$n looks stronger.");

    printf_to_char(ch, found ? "Success!\n\r" : "Spell failed.\n\r");
}

DEFINE_SPELL_FUN (spell_control_weather) {
    if (!str_cmp (target_name, "better")) {
        weather_info.change += dice (level / 3, 4);
        send_to_char ("You sense the clouds clearing up...\n\r", ch);
    }
    else if (!str_cmp (target_name, "worse")) {
        weather_info.change -= dice (level / 3, 4);
        send_to_char ("You sense a storm gathering...\n\r", ch);
    }
    else
        send_to_char ("Do you want it to get better or worse?\n\r", ch);
}

/* modified for enhanced use */
DEFINE_SPELL_FUN (spell_dispel_magic) {
    CHAR_T *victim = (CHAR_T *) vo;
    bool found = FALSE;

    if (saves_spell (level, victim, DAM_OTHER)) {
        send_to_char ("You feel a brief tingling sensation.\n\r", victim);
        send_to_char ("You notice no change.\n\r", ch);
        return;
    }

    /* begin running through the spells */
    found += check_dispel_quick (level, victim, "armor",           NULL);
    found += check_dispel_quick (level, victim, "bless",           NULL);
    found += check_dispel_quick (level, victim, "blindness",       "$n is no longer blinded.");
    found += check_dispel_quick (level, victim, "calm",            "$n no longer looks so peaceful...");
    found += check_dispel_quick (level, victim, "change sex",      "$n looks more like $mself again.");
    found += check_dispel_quick (level, victim, "charm person",    "$n regains $s free will.");
    found += check_dispel_quick (level, victim, "chill touch",     "$n looks warmer.");
    found += check_dispel_quick (level, victim, "curse",           NULL);
    found += check_dispel_quick (level, victim, "detect evil",     NULL);
    found += check_dispel_quick (level, victim, "detect good",     NULL);
    found += check_dispel_quick (level, victim, "detect hidden",   NULL);
    found += check_dispel_quick (level, victim, "detect invis",    NULL);
    found += check_dispel_quick (level, victim, "detect magic",    NULL);
    found += check_dispel_quick (level, victim, "faerie fire",     "$n's outline fades.");
    found += check_dispel_quick (level, victim, "fly",             "$n falls to the ground!");
    found += check_dispel_quick (level, victim, "frenzy",          "$n no longer looks so wild.");
    found += check_dispel_quick (level, victim, "giant strength",  "$n no longer looks so mighty.");
    found += check_dispel_quick (level, victim, "haste",           "$n is no longer moving so quickly.");
    found += check_dispel_quick (level, victim, "infravision",     NULL);
    found += check_dispel_quick (level, victim, "invis",           "$n fades into existance.");
    found += check_dispel_quick (level, victim, "mass invis",      "$n fades into existance.");
    found += check_dispel_quick (level, victim, "pass door",       NULL);
    found += check_dispel_quick (level, victim, "protection evil", NULL);
    found += check_dispel_quick (level, victim, "protection good", NULL);
    found += check_dispel_quick (level, victim, "sanctuary",       "The white aura around $n's body vanishes.");
    found += check_dispel_quick (level, victim, "shield",          "The shield protecting $n vanishes.");
    found += check_dispel_quick (level, victim, "sleep",           NULL);
    found += check_dispel_quick (level, victim, "slow",            "$n is no longer moving so slowly.");
    found += check_dispel_quick (level, victim, "stone skin",      "$n's skin regains its normal texture.");
    found += check_dispel_quick (level, victim, "weaken",          "$n looks stronger.");

    if (IS_AFFECTED (victim, AFF_SANCTUARY)
        && !saves_dispel (level, victim->level, -1)
        && !affect_is_char_affected (victim, skill_lookup ("sanctuary")))
    {
        REMOVE_BIT (victim->affected_by, AFF_SANCTUARY);
        act ("The white aura around $n's body vanishes.",
             victim, NULL, NULL, TO_NOTCHAR);
        found = TRUE;
    }

    printf_to_char(ch, found ? "Success!\n\r" : "Spell failed.\n\r");
}

DEFINE_SPELL_FUN (spell_recharge) {
    OBJ_T *obj = (OBJ_T *) vo;
    int chance, percent;
    flag_t wlevel, *recharge_ptr, *charges_ptr;

    BAIL_IF (!item_get_recharge_values (obj, &wlevel, &recharge_ptr, &charges_ptr),
        "That item does not carry charges.\n\r", ch);
    BAIL_IF (wlevel >= 3 * level / 2,
        "Your skills are not great enough for that.\n\r", ch);
    BAIL_IF (*recharge_ptr == 0,
        "That item has already been recharged once.\n\r", ch);

    chance = 40 + 2 * level;
    chance -= wlevel; /* harder to do high-level spells */
    chance -= (*recharge_ptr - *charges_ptr) *
              (*recharge_ptr - *charges_ptr);
    chance = UMAX (level / 2, chance);

    percent = number_percent ();
    if (percent < chance / 2) {
        act ("$p glows softly.", ch, obj, NULL, TO_ALL);
        *charges_ptr  = UMAX (*recharge_ptr, *charges_ptr);
        *recharge_ptr = 0;
        return;
    }
    else if (percent <= chance) {
        int chargeback, chargemax;
        act ("$p glows softly for a moment.", ch, obj, NULL, TO_ALL);

        chargemax = *recharge_ptr - *charges_ptr;
        if (chargemax > 0)
            chargeback = UMAX (1, chargemax * percent / 100);
        else
            chargeback = 0;

        *charges_ptr += chargeback;
        *recharge_ptr = 0;
        return;
    }
    else if (percent <= UMIN (95, 3 * chance / 2)) {
        send_to_char ("Nothing seems to happen.\n\r", ch);
        if (*recharge_ptr > 0)
            (*recharge_ptr)--;
        return;
    }
    else { /* whoops! */
        act ("$p glows brightly and explodes!", ch, obj, NULL, TO_ALL);
        obj_extract (obj);
    }
}

DEFINE_SPELL_FUN (spell_ventriloquate) {
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char speaker[MAX_INPUT_LENGTH];
    CHAR_T *vch;

    target_name = one_argument (target_name, speaker);
    sprintf (buf1, "%s says '%s'.\n\r", speaker, target_name);
    sprintf (buf2, "Someone makes %s say '%s'.\n\r", speaker, target_name);
    buf1[0] = UPPER (buf1[0]);

    for (vch = ch->in_room->people_first; vch != NULL; vch = vch->room_next)
        if (!str_in_namelist_exact (speaker, vch->name) && IS_AWAKE (vch))
            send_to_char (saves_spell (level, vch, DAM_OTHER) ? buf2 : buf1, vch);
}
