

#include <string.h>
#include <stdlib.h>

#include "lookup.h"
#include "utils.h"

#include "skills.h"

void skill_clear_mapping (void) {
    int i;

    for (i = 0; i < SKILL_MAP_MAX; i++)
        skill_map_table[i].skill_index = -1;
    for (i = 0; i < SKILL_MAX; i++) {
        skill_table[i].map_index    = -1;
        skill_table[i].weapon_index = -1;
    }
    for (i = 0; i < WEAPON_MAX; i++)
        weapon_table[i].skill_index = -1;
}

void skill_init_mapping (void) {
    SKILL_MAP_T *map;
    SKILL_T *skill;
    WEAPON_T *weapon;
    int i, index;

    /* map skills by name to our internal mapping and vice-versa. */
    for (i = 0; i < SKILL_MAP_MAX; i++) {
        map = &(skill_map_table[i]);
        if (map->name == NULL)
            continue;
        if ((index = skill_lookup_exact (map->name)) < 0) {
            bugf ("skill_init_mapping: Skill '%s' not found", map->name);
            continue;
        }
        skill = &(skill_table[index]);

        map->skill_index = index;
        skill->map_index = i;
    }

    /* map weapons to skills and vice-versa. */
    for (i = 0; i < WEAPON_MAX; i++) {
        weapon = &(weapon_table[i]);
        if (weapon->name == NULL)
            continue;
        if ((index = skill_lookup_exact (weapon->skill)) < 0) {
            bugf ("skill_init_mapping: Weapon skill '%s' not found", weapon->skill);
            continue;
        }
        skill = &(skill_table[index]);

        weapon->skill_index = index;
        skill->weapon_index = i;
    }
}
