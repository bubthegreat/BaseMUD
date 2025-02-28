

#include "magic.h"
#include "find.h"
#include "comm.h"
#include "db.h"
#include "objs.h"
#include "utils.h"
#include "chars.h"
#include "globals.h"
#include "memory.h"
#include "items.h"

#include "spell_create.h"

DEFINE_SPELL_FUN (spell_continual_light) {
    OBJ_T *light;

    if (target_name[0] != '\0') { /* do a glow on some object */
        light = find_obj_own_inventory (ch, target_name);

        BAIL_IF (light == NULL,
            "You don't see that here.\n\r", ch);
        BAIL_IF_ACT (IS_OBJ_STAT (light, ITEM_GLOW),
            "$p is already glowing.", ch, light, NULL);

        SET_BIT (light->extra_flags, ITEM_GLOW);
        act ("$p glows with a white light.", ch, light, NULL, TO_ALL);
        return;
    }

    light = obj_create (obj_get_index (OBJ_VNUM_LIGHT_BALL), 0);
    obj_give_to_room (light, ch->in_room);
    act ("You twiddle your thumbs and $p appears.", ch, light, NULL, TO_CHAR);
    act ("$n twiddles $s thumbs and $p appears.", ch, light, NULL, TO_NOTCHAR);
}

DEFINE_SPELL_FUN (spell_create_food) {
    OBJ_T *mushroom;
    mushroom = obj_create (obj_get_index (OBJ_VNUM_MUSHROOM), 0);
    mushroom->v.food.hunger   = level / 2;
    mushroom->v.food.fullness = level;
    obj_give_to_room (mushroom, ch->in_room);
    act ("$p suddenly appears.", ch, mushroom, NULL, TO_ALL);
}

DEFINE_SPELL_FUN (spell_create_rose) {
    OBJ_T *rose;
    rose = obj_create (obj_get_index (OBJ_VNUM_ROSE), 0);
    obj_give_to_char (rose, ch);
    send_to_char ("You create a beautiful red rose.\n\r", ch);
    act ("$n has created a beautiful red rose.", ch, rose, NULL, TO_NOTCHAR);
}

DEFINE_SPELL_FUN (spell_create_spring) {
    OBJ_T *spring;
    spring = obj_create (obj_get_index (OBJ_VNUM_SPRING), 0);
    spring->timer = level;
    obj_give_to_room (spring, ch->in_room);
    act ("$p flows from the ground.", ch, spring, NULL, TO_ALL);
}

DEFINE_SPELL_FUN (spell_create_water) {
    OBJ_T *obj = (OBJ_T *) vo;
    int water;

    BAIL_IF (!item_can_fill (obj),
        "It is unable to hold water.\n\r", ch);
    BAIL_IF (obj->v.drink_con.liquid != LIQ_WATER && obj->v.drink_con.filled != 0,
        "It contains some other liquid.\n\r", ch);

    water = UMIN (level * (weather_info.sky >= SKY_RAINING ? 4 : 2),
                  obj->v.drink_con.capacity - obj->v.drink_con.filled);

    BAIL_IF_ACT (water <= 0,
        "The spell fails and $p refuses to fill.", ch, obj, NULL);

    obj->v.drink_con.liquid = LIQ_WATER;
    obj->v.drink_con.filled += water;
    if (!str_in_namelist ("water", obj->name)) {
        char buf[MAX_STRING_LENGTH];

        sprintf (buf, "%s water", obj->name);
        str_replace_dup (&(obj->name), buf);
    }

    act ("$p is filled.", ch, obj, NULL, TO_CHAR);
}

DEFINE_SPELL_FUN (spell_floating_disc) {
    OBJ_T *disc, *floating;

    floating = char_get_eq_by_wear_loc (ch, WEAR_LOC_FLOAT);
    BAIL_IF_ACT (floating != NULL && IS_OBJ_STAT (floating, ITEM_NOREMOVE),
        "You can't remove $p.", ch, floating, NULL);

    disc = obj_create (obj_get_index (OBJ_VNUM_DISC), 0);
    disc->v.container.capacity   = ch->level * 10; /* 10 pounds/level */
    disc->v.container.max_weight = ch->level *  5;  /* 5 pounds/level */
    disc->timer = ch->level * 2 - number_range (0, level / 2);

    send_to_char ("You create a floating disc.\n\r", ch);
    act ("$n has created a floating black disc.", ch, NULL, NULL, TO_NOTCHAR);

    obj_give_to_char (disc, ch);
    char_wear_obj (ch, disc, TRUE);
}
