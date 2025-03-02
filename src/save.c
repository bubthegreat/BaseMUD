

#include "recycle.h"
#include "lookup.h"
#include "colour.h"
#include "db.h"
#include "utils.h"
#include "recycle.h"
#include "board.h"
#include "chars.h"
#include "objs.h"
#include "globals.h"
#include "memory.h"
#include "items.h"
#include "mobiles.h"
#include "players.h"
#include "fread.h"
#include "fwrite.h"
#include "rooms.h"
#include "affects.h"
#include "extra_descrs.h"

#include "save.h"

/* Array of containers read for proper re-nesting of objects. */
#define MAX_NEST    100
static OBJ_T *obj_nest[MAX_NEST];

/* Save a character and inventory.
 * Would be cool to save NPC's too for quest purposes,
 *   some of the infrastructure is provided. */
void save_char_obj (CHAR_T *ch) {
    char strsave[MAX_INPUT_LENGTH];
    FILE *fp;

    if (IS_NPC (ch))
        return;

    /* Fix by Edwin. JR -- 10/15/00
     *
     * Don't save if the character is invalidated.
     * This might happen during the auto-logoff of players.
     * (or other places not yet found out) */
    BAIL_IF_BUG (!IS_VALID (ch),
        "save_char_obj: Trying to save an invalidated character.\n", 0);

    if (ch->desc != NULL && ch->desc->original != NULL)
        ch = ch->desc->original;

#if defined(unix)
    /* create god log */
    if (IS_IMMORTAL (ch) || ch->level >= LEVEL_IMMORTAL) {
        fclose (reserve_file);
        sprintf (strsave, "%s%s", GOD_DIR, str_capitalized (ch->name));
        if ((fp = fopen (strsave, "w")) == NULL) {
            bug ("save_char_obj: fopen", 0);
            perror (strsave);
        }

        fprintf (fp, "Lev %2d Trust %2d  %s%s\n",
            ch->level, char_get_trust (ch), ch->name, ch->pcdata->title);
        fclose (fp);
        reserve_file = fopen (NULL_FILE, "r");
    }
#endif

    fclose (reserve_file);
    sprintf (strsave, "%s%s", PLAYER_DIR, str_capitalized (ch->name));
    if ((fp = fopen (TEMP_FILE, "w")) == NULL) {
        bug ("save_char_obj: fopen", 0);
        perror (strsave);
    }
    else {
        fwrite_char (ch, fp);
        if (ch->content_first != NULL)
            fwrite_obj (ch, ch->content_first, fp, 0);
        /* save the pets */
        if (ch->pet != NULL && ch->pet->in_room == ch->in_room)
            fwrite_pet (ch->pet, fp);
        fprintf (fp, "#END\n");
    }
    fclose (fp);
    rename (TEMP_FILE, strsave);
    reserve_file = fopen (NULL_FILE, "r");
}

/* Write the char. */
void fwrite_char (CHAR_T *ch, FILE *fp) {
    const PC_RACE_T *pc_race;
    const SKILL_T *skill;
    const SKILL_GROUP_T *group;
    AFFECT_T *paf;
    int sn, gn, pos, i;

    pc_race = pc_race_get_by_race (ch->race);

    fprintf (fp, "#%s\n", IS_NPC (ch) ? "MOB" : "PLAYER");
    fprintf (fp, "Name %s~\n", ch->name);
    fprintf (fp, "Id   %ld\n", ch->id);
    fprintf (fp, "LogO %ld\n", current_time);
    fprintf (fp, "Vers %d\n", 5);
    if (ch->short_descr[0] != '\0')
        fprintf (fp, "ShD  %s~\n", ch->short_descr);
    if (ch->long_descr[0] != '\0')
        fprintf (fp, "LnD  %s~\n", ch->long_descr);
    if (ch->description[0] != '\0')
        fprintf (fp, "Desc %s~\n", ch->description);
    if (ch->prompt != NULL || !str_cmp (ch->prompt, "<%hhp %mm %vmv> ")
        || !str_cmp (ch->prompt, "{c<%hhp %mm %vmv>{x "))
        fprintf (fp, "Prom %s~\n", ch->prompt);
    fprintf (fp, "Race %s~\n", pc_race->name);
    if (ch->clan)
        fprintf (fp, "Clan %s~\n", clan_table[ch->clan].name);
    fprintf (fp, "Sex  %d\n", ch->sex);
    fprintf (fp, "ClassName %s~\n", class_get_name (ch->class));
    fprintf (fp, "Levl %d\n", ch->level);
    if (ch->trust != 0)
        fprintf (fp, "Tru  %d\n", ch->trust);
    fprintf (fp, "Sec  %d\n", ch->pcdata->security);    /* OLC */
    fprintf (fp, "Plyd %d\n", ch->played + (int) (current_time - ch->logon));
    fprintf (fp, "Scro %d\n", ch->lines);
    fprintf (fp, "Room %d\n", (ch->in_room == room_get_index (ROOM_VNUM_LIMBO)
                               && ch->was_in_room != NULL)
             ? ch->was_in_room->vnum
             : ch->in_room == NULL ? ROOM_VNUM_TEMPLE : ch->in_room->vnum);

    fprintf (fp, "HMV  %d %d %d %d %d %d\n",
             ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move,
             ch->max_move);

    if (ch->gold > 0)
        fprintf (fp, "Gold %ld\n", ch->gold);
    else
        fprintf (fp, "Gold 0\n");

    if (ch->silver > 0)
        fprintf (fp, "Silv %ld\n", ch->silver);
    else
        fprintf (fp, "Silv 0\n");

    fprintf (fp, "Exp  %d\n", ch->exp);
    if (EXT_IS_NONZERO (ch->ext_mob))
        fprintf (fp, "Mob  %s\n", fwrite_ext_flags_static (mob_flags, ch->ext_mob));
    if (EXT_IS_NONZERO (ch->ext_plr))
        fprintf (fp, "Plr  %s\n", fwrite_ext_flags_static (plr_flags, ch->ext_plr));
    if (ch->affected_by != 0)
        fprintf (fp, "AfBy %s\n", fwrite_flags_static (
            affect_flags, ch->affected_by));
    fprintf (fp, "Comm %s\n", fwrite_flags_static (comm_flags, ch->comm));
    if (ch->wiznet)
        fprintf (fp, "Wizn %s\n", fwrite_flags_static (wiz_flags, ch->wiznet));
    if (ch->invis_level)
        fprintf (fp, "Invi %d\n", ch->invis_level);
    if (ch->incog_level)
        fprintf (fp, "Inco %d\n", ch->incog_level);
    fprintf (fp, "Pos  %d\n",
             ch->position == POS_FIGHTING ? POS_STANDING : ch->position);
    if (ch->practice != 0)
        fprintf (fp, "Prac %d\n", ch->practice);
    if (ch->train != 0)
        fprintf (fp, "Trai %d\n", ch->train);
    if (ch->saving_throw != 0)
        fprintf (fp, "Save  %d\n", ch->saving_throw);
    fprintf (fp, "Alig  %d\n", ch->alignment);
    if (ch->hitroll != 0)
        fprintf (fp, "Hit   %d\n", ch->hitroll);
    if (ch->damroll != 0)
        fprintf (fp, "Dam   %d\n", ch->damroll);
    fprintf (fp, "ACs %d %d %d %d\n",
             ch->armor[0], ch->armor[1], ch->armor[2], ch->armor[3]);
    if (ch->wimpy != 0)
        fprintf (fp, "Wimp  %d\n", ch->wimpy);
    fprintf (fp, "Attr %d %d %d %d %d\n",
             ch->perm_stat[STAT_STR],
             ch->perm_stat[STAT_INT],
             ch->perm_stat[STAT_WIS],
             ch->perm_stat[STAT_DEX], ch->perm_stat[STAT_CON]);

    fprintf (fp, "AMod %d %d %d %d %d\n",
             ch->mod_stat[STAT_STR],
             ch->mod_stat[STAT_INT],
             ch->mod_stat[STAT_WIS],
             ch->mod_stat[STAT_DEX], ch->mod_stat[STAT_CON]);

    if (IS_NPC (ch))
        fprintf (fp, "Vnum %d\n", ch->mob_index->vnum);
    else {
        fprintf (fp, "Pass %s~\n", ch->pcdata->pwd);
        if (ch->pcdata->bamfin[0] != '\0')
            fprintf (fp, "Bin  %s~\n", ch->pcdata->bamfin);
        if (ch->pcdata->bamfout[0] != '\0')
            fprintf (fp, "Bout %s~\n", ch->pcdata->bamfout);
        fprintf (fp, "Titl %s~\n", ch->pcdata->title);
        fprintf (fp, "Pnts %d\n", ch->pcdata->creation_points);
        fprintf (fp, "TSex %d\n", ch->pcdata->true_sex);
        fprintf (fp, "LLev %d\n", ch->pcdata->last_level);
        fprintf (fp, "HMVP %d %d %d\n", ch->pcdata->perm_hit,
                 ch->pcdata->perm_mana, ch->pcdata->perm_move);
        fprintf (fp, "Cnd  %d %d %d %d\n",
                 ch->pcdata->cond_hours[0], ch->pcdata->cond_hours[1],
                 ch->pcdata->cond_hours[2], ch->pcdata->cond_hours[3]);
        for (i = 0; i < COLOUR_SETTING_MAX; i++)
            fprintf (fp, "Colour %d %ld\n", i, ch->pcdata->colour[i]);

        /* write alias */
        for (pos = 0; pos < MAX_ALIAS; pos++) {
            if (ch->pcdata->alias[pos] == NULL
                || ch->pcdata->alias_sub[pos] == NULL)
                break;

            fprintf (fp, "Alias %s %s~\n", ch->pcdata->alias[pos],
                     ch->pcdata->alias_sub[pos]);
        }

        /* Save note board status */
        /* Save number of boards in case that number changes */
        fprintf (fp, "Boards       %d ", BOARD_MAX);
        for (i = 0; i < BOARD_MAX; i++)
            fprintf (fp, "%s %ld ", board_table[i].name, ch->pcdata->last_note[i]);
        fprintf (fp, "\n");

        for (sn = 0; sn < SKILL_MAX; sn++) {
            if ((skill = skill_get (sn)) == NULL)
                break;
            if (skill->name != NULL && ch->pcdata->learned[sn] > 0)
                fprintf (fp, "Sk %d '%s'\n", ch->pcdata->learned[sn],
                    skill->name);
        }

        for (gn = 0; gn < SKILL_GROUP_MAX; gn++) {
            if ((group = skill_group_get (gn)) == NULL)
                break;
            if (group->name != NULL && ch->pcdata->group_known[gn])
                fprintf (fp, "Gr '%s'\n", group->name);
        }
    }

    for (paf = ch->affect_first; paf != NULL; paf = paf->on_next) {
        if (paf->type < 0 || paf->type >= SKILL_MAX)
            continue;
        fprintf (fp, "Affc '%s' %3d %3d %3d %3d %3d %10ld\n",
                 skill_table[paf->type].name,
                 paf->bit_type, paf->level, paf->duration, paf->modifier,
                 paf->apply, paf->bits);
    }

#ifdef IMC
    imc_savechar( ch, fp );
#endif
    fprintf (fp, "End\n\n");
}

/* write a pet */
void fwrite_pet (CHAR_T *pet, FILE *fp) {
    const RACE_T *race;
    const MOB_INDEX_T *mob_index;
    AFFECT_T *paf;

    mob_index = pet->mob_index;
    race = race_get (pet->race);

    fprintf (fp, "#PET\n");

    fprintf (fp, "Vnum %d\n", mob_index->vnum);

    fprintf (fp, "Name %s~\n", pet->name);
    fprintf (fp, "LogO %ld\n", current_time);
    if (pet->short_descr != mob_index->short_descr)
        fprintf (fp, "ShD  %s~\n", pet->short_descr);
    if (pet->long_descr != mob_index->long_descr)
        fprintf (fp, "LnD  %s~\n", pet->long_descr);
    if (pet->description != mob_index->description)
        fprintf (fp, "Desc %s~\n", pet->description);
    if (pet->race != mob_index->race)
        fprintf (fp, "Race %s~\n", race->name);
    if (pet->clan)
        fprintf (fp, "Clan %s~\n", clan_table[pet->clan].name);
    fprintf (fp, "Sex  %d\n", pet->sex);
    if (pet->level != mob_index->level)
        fprintf (fp, "Levl %d\n", pet->level);
    fprintf (fp, "HMV  %d %d %d %d %d %d\n",
             pet->hit, pet->max_hit, pet->mana, pet->max_mana, pet->move,
             pet->max_move);
    if (pet->gold > 0)
        fprintf (fp, "Gold %ld\n", pet->gold);
    if (pet->silver > 0)
        fprintf (fp, "Silv %ld\n", pet->silver);
    if (pet->exp > 0)
        fprintf (fp, "Exp  %d\n", pet->exp);
    if (!EXT_EQUALS (pet->ext_mob, mob_index->ext_mob_final))
        fprintf (fp, "Mob  %s\n", fwrite_ext_flags_static (
            mob_flags, pet->ext_mob));
    if (EXT_IS_NONZERO (pet->ext_plr))
        fprintf (fp, "Plr  %s\n", fwrite_ext_flags_static (
            plr_flags, pet->ext_plr));
    if (pet->affected_by != mob_index->affected_by_final)
        fprintf (fp, "AfBy %s\n", fwrite_flags_static (
            affect_flags, pet->affected_by));
    if (pet->comm != 0)
        fprintf (fp, "Comm %s\n", fwrite_flags_static (
            comm_flags, pet->comm));
    fprintf (fp, "Pos  %d\n", pet->position =
             POS_FIGHTING ? POS_STANDING : pet->position);
    if (pet->saving_throw != 0)
        fprintf (fp, "Save %d\n", pet->saving_throw);
    if (pet->alignment != mob_index->alignment)
        fprintf (fp, "Alig %d\n", pet->alignment);
    if (pet->hitroll != mob_index->hitroll)
        fprintf (fp, "Hit  %d\n", pet->hitroll);
    if (pet->damroll != mob_index->damage.bonus)
        fprintf (fp, "Dam  %d\n", pet->damroll);
    fprintf (fp, "ACs  %d %d %d %d\n",
             pet->armor[0], pet->armor[1], pet->armor[2], pet->armor[3]);
    fprintf (fp, "Attr %d %d %d %d %d\n",
             pet->perm_stat[STAT_STR], pet->perm_stat[STAT_INT],
             pet->perm_stat[STAT_WIS], pet->perm_stat[STAT_DEX],
             pet->perm_stat[STAT_CON]);
    fprintf (fp, "AMod %d %d %d %d %d\n",
             pet->mod_stat[STAT_STR], pet->mod_stat[STAT_INT],
             pet->mod_stat[STAT_WIS], pet->mod_stat[STAT_DEX],
             pet->mod_stat[STAT_CON]);

    for (paf = pet->affect_first; paf != NULL; paf = paf->on_next) {
        if (paf->type < 0 || paf->type >= SKILL_MAX)
            continue;
        fprintf (fp, "Affc '%s' %3d %3d %3d %3d %3d %10ld\n",
                 skill_table[paf->type].name,
                 paf->bit_type, paf->level, paf->duration, paf->modifier,
                 paf->apply, paf->bits);
    }
    fprintf (fp, "End\n");
}

/* Write an object and its contents. */
void fwrite_obj (CHAR_T *ch, OBJ_T *obj, FILE *fp, int nest) {
    EXTRA_DESCR_T *ed;
    AFFECT_T *paf;
    OBJ_INDEX_T *obj_index;

    /* Slick recursion to write lists backwards,
     * so loading them will load in forwards order. */
    /* TODO: it may be slick, but fwrite_obj() shouldn't write more than
     *       one object. put this somewhere else! */
    if (obj->content_next != NULL)
        fwrite_obj (ch, obj->content_next, fp, nest);

    /* Don't store non-persistant items. */
    if (!item_should_save_for_level (obj, ch->level))
        return;

    obj_index = obj->obj_index;

    fprintf (fp, "#O\n");
    fprintf (fp, "Vnum %d\n", obj_index->vnum);
    if (!obj_index->new_format)
        fprintf (fp, "Oldstyle\n");
    if (obj->enchanted)
        fprintf (fp, "Enchanted\n");
    fprintf (fp, "Nest %d\n", nest);

    /* These data are only used if they do not match the defaults */
    if (obj->name != obj_index->name)
        fprintf (fp, "Name %s~\n", obj->name);
    if (obj->short_descr != obj_index->short_descr)
        fprintf (fp, "ShD  %s~\n", obj->short_descr);
    if (obj->description != obj_index->description)
        fprintf (fp, "Desc %s~\n", obj->description);
    if (obj->extra_flags != obj_index->extra_flags)
        fprintf (fp, "ExtF %ld\n", obj->extra_flags);
    if (obj->wear_flags != obj_index->wear_flags)
        fprintf (fp, "WeaF %ld\n", obj->wear_flags);
    if (obj->item_type != obj_index->item_type)
        fprintf (fp, "Ityp %d\n", obj->item_type);
    if (obj->weight != obj_index->weight)
        fprintf (fp, "Wt   %d\n", obj->weight);
    if (obj->condition != obj_index->condition)
        fprintf (fp, "Cond %d\n", obj->condition);

    /* Variable data */
    fprintf (fp, "Wear %d\n", obj->wear_loc);
    if (obj->level != obj_index->level)
        fprintf (fp, "Lev  %d\n", obj->level);
    if (obj->timer != 0)
        fprintf (fp, "Time %d\n", obj->timer);
    fprintf (fp, "Cost %d\n", obj->cost);
    if (obj->v.value[0] != obj_index->v.value[0] ||
        obj->v.value[1] != obj_index->v.value[1] ||
        obj->v.value[2] != obj_index->v.value[2] ||
        obj->v.value[3] != obj_index->v.value[3] ||
        obj->v.value[4] != obj_index->v.value[4])
    {
        fprintf (fp, "Val  %ld %ld %ld %ld %ld\n",
                 obj->v.value[0], obj->v.value[1], obj->v.value[2],
                 obj->v.value[3], obj->v.value[4]);
    }

    item_write_save_data (obj, fp);

    for (paf = obj->affect_first; paf != NULL; paf = paf->on_next) {
        if (paf->type < 0 || paf->type >= SKILL_MAX)
            continue;
        fprintf (fp, "Affc '%s' %3d %3d %3d %3d %3d %10ld\n",
                 skill_table[paf->type].name,
                 paf->bit_type, paf->level, paf->duration, paf->modifier,
                 paf->apply, paf->bits);
    }

    for (ed = obj->extra_descr_first; ed != NULL; ed = ed->on_next)
        fprintf (fp, "ExDe %s~ %s~\n", ed->keyword, ed->description);
    fprintf (fp, "End\n\n");

    if (obj->content_first != NULL)
        fwrite_obj (ch, obj->content_first, fp, nest + 1);
}

/* Load a char and inventory into a new ch structure. */
bool load_char_obj (DESCRIPTOR_T *d, char *name) {
    char strsave[MAX_INPUT_LENGTH];
    char buf[100];
    CHAR_T *ch;
    FILE *fp;
    bool found;
    int stat;

    ch = char_new ();
    ch->pcdata = pcdata_new ();

    d->character        = ch;
    ch->desc            = d;
    ch->name            = str_dup (name);
    ch->id              = get_pc_id ();
    ch->race            = race_lookup_exact ("human");
    ch->ext_plr         = EXT_BITS (PLR_NOSUMMON);
    ch->comm            = COMM_COMBINE | COMM_PROMPT;
    ch->prompt          = str_dup ("<%hhp %mm %vmv> ");
    ch->pcdata->confirm_delete = FALSE;
    ch->pcdata->board   = &board_table[DEFAULT_BOARD];
    ch->pcdata->pwd     = str_dup ("");
    ch->pcdata->bamfin  = str_dup ("");
    ch->pcdata->bamfout = str_dup ("");
    ch->pcdata->title   = str_dup ("");
    for (stat = 0; stat < STAT_MAX; stat++)
        ch->perm_stat[stat] = 13;
    ch->pcdata->cond_hours[COND_THIRST] = COND_HOURS_MAX;
    ch->pcdata->cond_hours[COND_FULL]   = COND_HOURS_MAX;
    ch->pcdata->cond_hours[COND_HUNGER] = COND_HOURS_MAX;
    ch->pcdata->security = 0; /* OLC */
    player_reset_colour (ch);

#ifdef IMC
    imc_initchar( ch );
#endif
    found = FALSE;
    fclose (reserve_file);

#if defined(unix)
    /* decompress if .gz file exists */
    sprintf (strsave, "%s%s%s", PLAYER_DIR, str_capitalized (name), ".gz");
    if ((fp = fopen (strsave, "r")) != NULL) {
        fclose (fp);
        sprintf (buf, "gzip -dfq %s", strsave);
        system (buf);
    }
#endif

    sprintf (strsave, "%s%s", PLAYER_DIR, str_capitalized (name));
    if ((fp = fopen (strsave, "r")) != NULL) {
        int nest;
        for (nest = 0; nest < MAX_NEST; nest++)
            obj_nest[nest] = NULL;

        found = TRUE;
        while (1) {
            char letter;
            char *word;

            letter = fread_letter (fp);
            if (letter == '*') {
                fread_to_eol (fp);
                continue;
            }
            if (letter != '#') {
                bug ("load_char_obj: # not found.", 0);
                break;
            }

            word = fread_word_static (fp);
                 if (!str_cmp (word, "PLAYER")) fread_char (ch, fp);
            else if (!str_cmp (word, "OBJECT")) fread_obj (ch, fp);
            else if (!str_cmp (word, "O"))      fread_obj (ch, fp);
            else if (!str_cmp (word, "PET"))    fread_pet (ch, fp);
            else if (!str_cmp (word, "END"))    break;
            else {
                bug ("load_char_obj: bad section.", 0);
                break;
            }
        }
        fclose (fp);
    }

    reserve_file = fopen (NULL_FILE, "r");

    /* initialize race */
    if (found) {
        const PC_RACE_T *pc_race;
        const RACE_T *race;

        int i;
        if (ch->race == 0)
            ch->race = race_lookup_exact ("human");

        pc_race = pc_race_get_by_race (ch->race);
        ch->size = pc_race->size;
        ch->attack_type = ATTACK_PUNCH;

        for (i = 0; i < PC_RACE_SKILL_MAX; i++) {
            if (pc_race->skills[i] == NULL)
                break;
            player_add_skill_or_group (ch, pc_race->skills[i], FALSE);
        }

        race = race_get (ch->race);
        ch->affected_by = ch->affected_by | race->aff;
        ch->imm_flags   = ch->imm_flags   | race->imm;
        ch->res_flags   = ch->res_flags   | race->res;
        ch->vuln_flags  = ch->vuln_flags  | race->vuln;
        ch->form        = race->form;
        ch->parts       = race->parts;
    }

    /* RT initialize skills */
    if (found && ch->version < 2) { /* need to add the new skills */
        player_add_skill_or_group (ch, "rom basics", FALSE);
        if (class_table[ch->class].base_group != NULL)
            player_add_skill_or_group (ch, class_table[ch->class].base_group, FALSE);
        if (class_table[ch->class].default_group != NULL)
            player_add_skill_or_group (ch, class_table[ch->class].default_group, TRUE);
        ch->pcdata->learned[SN(RECALL)] = 50;
    }

    /* fix levels */
    if (found && ch->version < 3 && (ch->level > 35 || ch->trust > 35)) {
        switch (ch->level) {
            case (40): ch->level = 60; break; /* imp -> imp */
            case (39): ch->level = 58; break; /* god -> supreme */
            case (38): ch->level = 56; break; /* deity -> god */
            case (37): ch->level = 53; break; /* angel -> demigod */
        }

        switch (ch->trust) {
            case (40): ch->trust = 60; break; /* imp -> imp */
            case (39): ch->trust = 58; break; /* god -> supreme */
            case (38): ch->trust = 56; break; /* deity -> god */
            case (37): ch->trust = 53; break; /* angel -> demigod */
            case (36): ch->trust = 51; break; /* hero -> hero */
        }
    }

    /* ream gold */
    if (found && ch->version < 4)
        ch->gold /= 100;
    return found;
}

void load_old_colour (CHAR_T *ch, FILE *fp, char *name) {
    const COLOUR_SETTING_T *setting;
    int number;
    flag_t flag;

    /* Always read the number */
    number = fread_number (fp);
    setting = colour_setting_get_by_name_exact (name);
    BAIL_IF_BUGF (setting == NULL,
        "load_old_colour: unknown color setting '%s'", name);

    /* Convert Lope's saved codes to a bitmask */
    /* NOTE: LOAD_COLOUR was *broken* and used > comparisons
     *       instead of >= comparisons. As a result, color code
     *       100 and 10 would potentially occur for black, and
     *       dark grey wouldn't load at all. */
    flag = 0;
    if (number >= 100) {
        number -= 100;
        flag |= CB_BEEP;
    }
    if (number >= 10) {
        number -= 10;
        flag |= CB_BRIGHT;
    }
    flag |= (number & 0x07);

    /* Number should now be >= 0x00 and <= 0x0f. If it's not, it's broken :(
     * Default to white. */
    if (number > 0x0f)
        flag = CC_WHITE;

    /* Default backcolor, always. */
    flag |= CC_BACK_DEFAULT;

    /* Store color! */
    ch->pcdata->colour[setting->index] = flag;
}

void fread_char (CHAR_T *ch, FILE *fp) {
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool match;
    int count = 0;
    int lastlogoff = current_time;
    int percent;

    log_f ("Loading %s.", ch->name);

    while (1) {
        word = feof (fp) ? "End" : fread_word_static (fp);
        match = FALSE;

        switch (UPPER (word[0])) {
            case '*':
                match = TRUE;
                fread_to_eol (fp);
                break;

            case 'A':
                if (!str_cmp (word, "Act")) {
                    long old_pos = ftell (fp);
                    match = TRUE;

                    EXT_FLAGS_T flags = fread_ext_flag (fp, mob_flags);
                    if (EXT_IS_SET (flags, MOB_IS_NPC))
                        ch->ext_mob = flags;
                    else {
                        fseek (fp, old_pos, SEEK_SET);
                        ch->ext_plr = fread_ext_flag (fp, plr_flags);
                    }
                }

                KEY ("AffectedBy", ch->affected_by, fread_flag (fp, affect_flags));
                KEY ("AfBy",       ch->affected_by, fread_flag (fp, affect_flags));
                KEY ("Alignment",  ch->alignment,   fread_flag (fp, NULL));
                KEY ("Alig",       ch->alignment,   fread_number (fp));

                if (!str_cmp (word, "Alia")) {
                    if (count >= MAX_ALIAS) {
                        fread_to_eol (fp);
                        match = TRUE;
                        break;
                    }

                    fread_word_replace (fp, &ch->pcdata->alias[count]);
                    fread_string_replace (fp, &ch->pcdata->alias_sub[count]);
                    count++;
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "Alias")) {
                    if (count >= MAX_ALIAS) {
                        fread_to_eol (fp);
                        match = TRUE;
                        break;
                    }

                    fread_word_replace (fp, &ch->pcdata->alias[count]);
                    fread_string_replace (fp, &ch->pcdata->alias_sub[count]);
                    count++;
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "AC") || !str_cmp (word, "Armor")) {
                    fread_to_eol (fp);
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "ACs")) {
                    int i;
                    for (i = 0; i < 4; i++)
                        ch->armor[i] = fread_number (fp);
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "AffD")) {
                    AFFECT_T *paf;
                    int sn;

                    paf = affect_new ();
                    sn = skill_lookup (fread_word_static (fp));
                    if (sn < 0)
                        bug ("fread_char: unknown skill.", 0);
                    else
                        paf->type = sn;

                    paf->level    = fread_number (fp);
                    paf->duration = fread_number (fp);
                    paf->modifier = fread_number (fp);
                    paf->apply    = fread_number (fp);
                    paf->bits     = fread_number (fp);

                    affect_to_char_back (paf, ch);
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "Affc")) {
                    AFFECT_T *paf;
                    int sn;

                    paf = affect_new ();
                    sn = skill_lookup (fread_word_static (fp));
                    if (sn < 0)
                        bug ("fread_char: unknown skill.", 0);
                    else
                        paf->type = sn;

                    paf->bit_type = fread_number (fp);
                    paf->level    = fread_number (fp);
                    paf->duration = fread_number (fp);
                    paf->modifier = fread_number (fp);
                    paf->apply    = fread_number (fp);
                    paf->bits     = fread_number (fp);

                    affect_to_char_back (paf, ch);
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "AttrMod") || !str_cmp (word, "AMod")) {
                    int stat;
                    for (stat = 0; stat < STAT_MAX; stat++)
                        ch->mod_stat[stat] = fread_number (fp);
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "AttrPerm") || !str_cmp (word, "Attr")) {
                    int stat;

                    for (stat = 0; stat < STAT_MAX; stat++)
                        ch->perm_stat[stat] = fread_number (fp);
                    match = TRUE;
                    break;
                }
                break;

            case 'B':
                KEYS ("Bamfin",  ch->pcdata->bamfin,  fread_string_static (fp));
                KEYS ("Bamfout", ch->pcdata->bamfout, fread_string_static (fp));
                KEYS ("Bin",     ch->pcdata->bamfin,  fread_string_static (fp));
                KEYS ("Bout",    ch->pcdata->bamfout, fread_string_static (fp));

                /* Read in board status */
                if (!str_cmp(word, "Boards" )) {
                    int i,num = fread_number (fp); /* number of boards saved */
                    char *boardname;

                    for (; num ; num-- ) { /* for each of the board saved */
                        boardname = fread_word_static (fp);
                        i = board_lookup (boardname); /* find board number */

                        if (i == BOARD_NOTFOUND) { /* Does board still exist ? */
                            log_f ("fread_char: %s had unknown board name: %s. Skipped.",
                                ch->name, boardname);
                            fread_number (fp); /* read last_note and skip info */
                        }
                        else /* Save it */
                            ch->pcdata->last_note[i] = fread_number (fp);
                    } /* for */

                    match = TRUE;
                } /* Boards */
                break;

            case 'C':
                KEY ("ClassName", ch->class, class_lookup_exact (fread_string_static (fp)));
                KEY ("Class", ch->class, fread_number (fp));
                KEY ("Cla",   ch->class, fread_number (fp));
                KEY ("Clan",  ch->clan,  clan_lookup_exact (fread_string_static (fp)));
                KEY ("Comm",  ch->comm,  fread_flag (fp, comm_flags));

                if (!str_cmp (word, "Condition") || !str_cmp (word, "Cond")) {
                    ch->pcdata->cond_hours[0] = fread_number (fp);
                    ch->pcdata->cond_hours[1] = fread_number (fp);
                    ch->pcdata->cond_hours[2] = fread_number (fp);
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "Cnd")) {
                    ch->pcdata->cond_hours[0] = fread_number (fp);
                    ch->pcdata->cond_hours[1] = fread_number (fp);
                    ch->pcdata->cond_hours[2] = fread_number (fp);
                    ch->pcdata->cond_hours[3] = fread_number (fp);
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "Coloura")) {
                    load_old_colour (ch, fp, "text");
                    load_old_colour (ch, fp, "auction");
                    load_old_colour (ch, fp, "gossip");
                    load_old_colour (ch, fp, "music");
                    load_old_colour (ch, fp, "question");
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "Colourb")) {
                    load_old_colour (ch, fp, "answer");
                    load_old_colour (ch, fp, "quote");
                    load_old_colour (ch, fp, "quote_text");
                    load_old_colour (ch, fp, "immtalk_text");
                    load_old_colour (ch, fp, "immtalk_type");
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "Colourc")) {
                    load_old_colour (ch, fp, "info");
                    load_old_colour (ch, fp, "tell");
                    load_old_colour (ch, fp, "reply");
                    load_old_colour (ch, fp, "gtell_text");
                    load_old_colour (ch, fp, "gtell_type");
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "Colourd")) {
                    load_old_colour (ch, fp, "room_title");
                    load_old_colour (ch, fp, "room_text");
                    load_old_colour (ch, fp, "room_exits");
                    load_old_colour (ch, fp, "room_things");
                    load_old_colour (ch, fp, "prompt");
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "Coloure")) {
                    load_old_colour (ch, fp, "fight_death");
                    load_old_colour (ch, fp, "fight_yhit");
                    load_old_colour (ch, fp, "fight_ohit");
                    load_old_colour (ch, fp, "fight_thit");
                    load_old_colour (ch, fp, "fight_skill");
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "Colourf")) {
                    load_old_colour (ch, fp, "wiznet");
                    load_old_colour (ch, fp, "say");
                    load_old_colour (ch, fp, "say_text");
                    load_old_colour (ch, fp, "tell_text");
                    load_old_colour (ch, fp, "reply_text");
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "Colourg")) {
                    load_old_colour (ch, fp, "auction_text");
                    load_old_colour (ch, fp, "gossip_text");
                    load_old_colour (ch, fp, "music_text");
                    load_old_colour (ch, fp, "question_text");
                    load_old_colour (ch, fp, "answer_text");
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "Colour")) {
                    ch->pcdata->colour[fread_number(fp)] = fread_flag (fp, NULL);
                    match = TRUE;
                }
                break;

            case 'D':
                KEY ("Damroll",     ch->damroll, fread_number (fp));
                KEY ("Dam",         ch->damroll, fread_number (fp));
                KEYS("Description", ch->description, fread_string_dup (fp));
                KEYS("Desc",        ch->description, fread_string_dup (fp));
                break;

            case 'E':
                if (!str_cmp (word, "End")) {
                    /* adjust hp mana move up  -- here for speed's sake */
                    percent =
                        (current_time - lastlogoff) * 25 / (2 * 60 * 60);

                    percent = UMIN (percent, 100);

                    if (percent > 0 && !IS_AFFECTED (ch, AFF_POISON)
                        && !IS_AFFECTED (ch, AFF_PLAGUE))
                    {
                        ch->hit += (ch->max_hit - ch->hit) * percent / 100;
                        ch->mana += (ch->max_mana - ch->mana) * percent / 100;
                        ch->move += (ch->max_move - ch->move) * percent / 100;
                    }
                    return;
                }
                KEY ("Exp", ch->exp, fread_number (fp));
                break;

            case 'G':
                KEY ("Gold", ch->gold, fread_number (fp));
                if (!str_cmp (word, "Group") || !str_cmp (word, "Gr")) {
                    int gn;
                    char *temp;

                    temp = fread_word_static (fp);
                    gn = skill_group_lookup (temp);
                 /* gn = group_lookup (fread_word (fp)); */
                    if (gn < 0) {
                        fprintf (stderr, "%s", temp);
                        bug ("fread_char: unknown group. ", 0);
                    }
                    else
                        player_add_skill_group (ch, gn, FALSE);
                    match = TRUE;
                }
                break;

            case 'H':
                KEY ("Hitroll", ch->hitroll, fread_number (fp));
                KEY ("Hit", ch->hitroll, fread_number (fp));

                if (!str_cmp (word, "HpManaMove") || !str_cmp (word, "HMV")) {
                    ch->hit = fread_number (fp);
                    ch->max_hit = fread_number (fp);
                    ch->mana = fread_number (fp);
                    ch->max_mana = fread_number (fp);
                    ch->move = fread_number (fp);
                    ch->max_move = fread_number (fp);
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "HpManaMovePerm")
                    || !str_cmp (word, "HMVP"))
                {
                    ch->pcdata->perm_hit = fread_number (fp);
                    ch->pcdata->perm_mana = fread_number (fp);
                    ch->pcdata->perm_move = fread_number (fp);
                    match = TRUE;
                    break;
                }

                break;

            case 'I':
                KEY ("Id", ch->id, fread_number (fp));
                KEY ("InvisLevel", ch->invis_level, fread_number (fp));
                KEY ("Inco", ch->incog_level, fread_number (fp));
                KEY ("Invi", ch->invis_level, fread_number (fp));
#ifdef IMC
                if( ( match = imc_loadchar( ch, fp, word ) ) )
                    break;
#endif
                break;

            case 'L':
                KEY ("LastLevel", ch->pcdata->last_level, fread_number (fp));
                KEY ("LLev", ch->pcdata->last_level, fread_number (fp));
                KEY ("Level", ch->level, fread_number (fp));
                KEY ("Lev", ch->level, fread_number (fp));
                KEY ("Levl", ch->level, fread_number (fp));
                KEY ("LogO", lastlogoff, fread_number (fp));
                KEYS ("LongDescr", ch->long_descr, fread_string_static (fp));
                KEYS ("LnD", ch->long_descr, fread_string_static (fp));
                break;

            case 'M':
                KEY ("Mob", ch->ext_mob, fread_ext_flag (fp, mob_flags));
                break;

            case 'N':
                KEYS ("Name", ch->name, fread_string_dup (fp));
                break;

            case 'P':
                KEYS("Password", ch->pcdata->pwd, fread_string_dup (fp));
                KEYS("Pass",     ch->pcdata->pwd, fread_string_dup (fp));
                KEY ("Played",   ch->played, fread_number (fp));
                KEY ("Plyd",     ch->played, fread_number (fp));
                KEY ("Plr",      ch->ext_plr, fread_ext_flag (fp, plr_flags));
                KEY ("Points",   ch->pcdata->creation_points, fread_number (fp));
                KEY ("Pnts",     ch->pcdata->creation_points, fread_number (fp));
                KEY ("Position", ch->position, fread_number (fp));
                KEY ("Pos",      ch->position, fread_number (fp));
                KEY ("Practice", ch->practice, fread_number (fp));
                KEY ("Prac",     ch->practice, fread_number (fp));
                KEYS("Prompt",   ch->prompt, fread_string_dup (fp));
                KEYS("Prom",     ch->prompt, fread_string_dup (fp));
                break;

            case 'R':
                KEY ("Race", ch->race, race_lookup (fread_string_static (fp)));

                if (!str_cmp (word, "Room")) {
                    ch->in_room = room_get_index (fread_number (fp));
                    if (ch->in_room == NULL)
                        ch->in_room = room_get_index (ROOM_VNUM_LIMBO);
                    match = TRUE;
                    break;
                }

                break;

            case 'S':
                KEY ("SavingThrow", ch->saving_throw, fread_number (fp));
                KEY ("Save",        ch->saving_throw, fread_number (fp));
                KEY ("Scro",        ch->lines, fread_number (fp));
                KEY ("Sex",         ch->sex, fread_number (fp));
                KEYS("ShortDescr",  ch->short_descr, fread_string_dup (fp));
                KEYS("ShD",         ch->short_descr, fread_string_dup (fp));
                KEY ("Sec",         ch->pcdata->security, fread_number (fp));    /* OLC */
                KEY ("Silv",        ch->silver, fread_number (fp));

                if (!str_cmp (word, "Skill") || !str_cmp (word, "Sk")) {
                    int sn;
                    int value;
                    char *temp;

                    value = fread_number (fp);
                    temp = fread_word_static (fp);
                    sn = skill_lookup (temp);
                 /* sn = skill_lookup (fread_word (fp)); */
                    if (sn < 0) {
                        fprintf (stderr, "%s", temp);
                        bug ("fread_char: unknown skill. ", 0);
                    }
                    else
                        ch->pcdata->learned[sn] = value;
                    match = TRUE;
                }
                break;

            case 'T':
                KEY ("TrueSex", ch->pcdata->true_sex, fread_number (fp));
                KEY ("TSex", ch->pcdata->true_sex, fread_number (fp));
                KEY ("Trai", ch->train, fread_number (fp));
                KEY ("Trust", ch->trust, fread_number (fp));
                KEY ("Tru", ch->trust, fread_number (fp));

                if (!str_cmp (word, "Title") || !str_cmp (word, "Titl")) {
                    fread_string_replace (fp, &ch->pcdata->title);
                    if (ch->pcdata->title[0] != '.'
                        && ch->pcdata->title[0] != ','
                        && ch->pcdata->title[0] != '!'
                        && ch->pcdata->title[0] != '?')
                    {
                        sprintf (buf, " %s", ch->pcdata->title);
                        str_replace_dup (&(ch->pcdata->title), buf);
                    }
                    match = TRUE;
                    break;
                }

                break;

            case 'V':
                KEY ("Version", ch->version, fread_number (fp));
                KEY ("Vers", ch->version, fread_number (fp));
                if (!str_cmp (word, "Vnum")) {
                    mobile_to_mob_index (ch,
                        mobile_get_index (fread_number (fp)));
                    match = TRUE;
                    break;
                }
                break;

            case 'W':
                KEY ("Wimpy", ch->wimpy,  fread_number (fp));
                KEY ("Wimp",  ch->wimpy,  fread_number (fp));
                KEY ("Wizn",  ch->wiznet, fread_flag (fp, wiz_flags));
                break;
        }

        if (!match) {
            bug ("fread_char: no match.", 0);
            bug (word, 0);
            fread_to_eol (fp);
        }
    }
}

/* load a pet from the forgotten reaches */
void fread_pet (CHAR_T *ch, FILE *fp) {
    char *word;
    CHAR_T *pet;
    bool match;
    int lastlogoff = current_time;
    int percent;
    int vnum = 0;

    /* first entry had BETTER be the vnum or we barf */
    word = feof (fp) ? "END" : fread_word_static (fp);
    if (!str_cmp (word, "Vnum")) {
        vnum = fread_number (fp);
        if (mobile_get_index (vnum) == NULL) {
            bug ("fread_pet: bad vnum %d.", vnum);
            pet = mobile_create (mobile_get_index (MOB_VNUM_FIDO));
        }
        else
            pet = mobile_create (mobile_get_index (vnum));
    }
    else {
        bug ("fread_pet: no vnum in file.", 0);
        pet = mobile_create (mobile_get_index (MOB_VNUM_FIDO));
    }

    while (1) {
        word = feof (fp) ? "END" : fread_word_static (fp);
        match = FALSE;

        switch (UPPER (word[0])) {
            case '*':
                match = TRUE;
                fread_to_eol (fp);
                break;

            case 'A':
                if (!str_cmp (word, "Act")) {
                    long old_pos = ftell (fp);
                    match = TRUE;
                    EXT_FLAGS_T flags = fread_ext_flag (fp, mob_flags);
                    if (EXT_IS_SET (flags, MOB_IS_NPC))
                        pet->ext_mob = flags;
                    else {
                        fseek (fp, old_pos, SEEK_SET);
                        pet->ext_plr = fread_ext_flag (fp, plr_flags);
                    }
                }

                KEY ("AfBy", pet->affected_by, fread_flag (fp, affect_flags));
                KEY ("Alig", pet->alignment,   fread_number (fp));

                if (!str_cmp (word, "ACs")) {
                    int i;
                    for (i = 0; i < 4; i++)
                        pet->armor[i] = fread_number (fp);
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "AffD")) {
                    AFFECT_T *paf;
                    int sn;

                    paf = affect_new ();
                    sn = skill_lookup (fread_word_static (fp));
                    if (sn < 0)
                        bug ("fread_char: unknown skill.", 0);
                    else
                        paf->type = sn;

                    paf->level    = fread_number (fp);
                    paf->duration = fread_number (fp);
                    paf->modifier = fread_number (fp);
                    paf->apply    = fread_number (fp);
                    paf->bits     = fread_number (fp);

                    affect_to_char_back (paf, pet);
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "Affc")) {
                    AFFECT_T *paf;
                    int sn;

                    paf = affect_new ();
                    sn = skill_lookup (fread_word_static (fp));
                    if (sn < 0)
                        bug ("fread_char: unknown skill.", 0);
                    else
                        paf->type = sn;

                    paf->bit_type = fread_number (fp);
                    paf->level    = fread_number (fp);
                    paf->duration = fread_number (fp);
                    paf->modifier = fread_number (fp);
                    paf->apply    = fread_number (fp);
                    paf->bits     = fread_number (fp);

                    /* Added here after Chris Litchfield (The Mage's Lair)
                     * pointed out a bug with duplicating affects in saved
                     * pets. -- JR 2002/01/31 */
                    if (!check_pet_affected (vnum, paf))
                        affect_to_char_back (paf, pet);
                    else
                        affect_free (paf);
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "AMod")) {
                    int stat;
                    for (stat = 0; stat < STAT_MAX; stat++)
                        pet->mod_stat[stat] = fread_number (fp);
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "Attr")) {
                    int stat;
                    for (stat = 0; stat < STAT_MAX; stat++)
                        pet->perm_stat[stat] = fread_number (fp);
                    match = TRUE;
                    break;
                }
                break;

            case 'C':
                KEY ("Clan", pet->clan, clan_lookup (fread_string_static (fp)));
                KEY ("Comm", pet->comm, fread_flag (fp, comm_flags));
                break;

            case 'D':
                KEY ("Dam",  pet->damroll, fread_number (fp));
                KEYS("Desc", pet->description, fread_string_dup (fp));
                break;

            case 'E':
                if (!str_cmp (word, "End")) {
                    pet->leader = ch;
                    pet->master = ch;
                    ch->pet = pet;
                    /* adjust hp mana move up  -- here for speed's sake */
                    percent =
                        (current_time - lastlogoff) * 25 / (2 * 60 * 60);

                    if (percent > 0 && !IS_AFFECTED (ch, AFF_POISON)
                        && !IS_AFFECTED (ch, AFF_PLAGUE))
                    {
                        percent = UMIN (percent, 100);
                        pet->hit += (pet->max_hit - pet->hit) * percent / 100;
                        pet->mana +=
                            (pet->max_mana - pet->mana) * percent / 100;
                        pet->move +=
                            (pet->max_move - pet->move) * percent / 100;
                    }
                    return;
                }
                KEY ("Exp", pet->exp, fread_number (fp));
                break;

            case 'G':
                KEY ("Gold", pet->gold, fread_number (fp));
                break;

            case 'H':
                KEY ("Hit", pet->hitroll, fread_number (fp));
                if (!str_cmp (word, "HMV")) {
                    pet->hit      = fread_number (fp);
                    pet->max_hit  = fread_number (fp);
                    pet->mana     = fread_number (fp);
                    pet->max_mana = fread_number (fp);
                    pet->move     = fread_number (fp);
                    pet->max_move = fread_number (fp);
                    match = TRUE;
                    break;
                }
                break;

            case 'L':
                KEY ("Levl", pet->level, fread_number (fp));
                KEYS("LnD",  pet->long_descr, fread_string_dup (fp));
                KEY ("LogO", lastlogoff, fread_number (fp));
                break;

            case 'M':
                KEY ("Mob", pet->ext_mob, fread_ext_flag (fp, mob_flags));
                break;

            case 'N':
                KEYS("Name", pet->name, fread_string_dup (fp));
                break;

            case 'P':
                KEY ("Plr", pet->ext_plr,  fread_ext_flag (fp, plr_flags));
                KEY ("Pos", pet->position, fread_number (fp));
                break;

            case 'R':
                KEY ("Race", pet->race, race_lookup (fread_string_static (fp)));
                break;

            case 'S':
                KEY ("Save", pet->saving_throw, fread_number (fp));
                KEY ("Sex",  pet->sex, fread_number (fp));
                KEYS("ShD",  pet->short_descr, fread_string_dup (fp));
                KEY ("Silv", pet->silver, fread_number (fp));
                break;
        }

        if (!match) {
            bug ("fread_pet: no match.", 0);
            fread_to_eol (fp);
        }
    }
}

void fread_obj (CHAR_T *ch, FILE *fp) {
    OBJ_T *obj;
    OBJ_INDEX_T *obj_index;
    char *word;
    int nest;
    bool match;
    bool nested;
    bool vnum;
    bool first;
    bool new_format; /* to prevent errors */
    bool make_new;   /* update object */

    vnum       = FALSE;
    obj        = NULL;
    obj_index  = NULL;
    first      = TRUE; /* used to counter fp offset */
    new_format = FALSE;
    make_new   = FALSE;

    word = feof (fp) ? "End" : fread_word_static (fp);
    if (!str_cmp (word, "Vnum")) {
        int vnum;
        first = FALSE; /* fp will be in right place */

        vnum = fread_number (fp);
        if ((obj_index = obj_get_index (vnum)) == NULL)
            bug ("fread_obj: bad vnum %d.", vnum);
        else {
            obj = obj_create (obj_index, -1);
            new_format = TRUE;
        }
    }

    if (obj == NULL) { /* either not found or old style */
        obj = obj_new ();
        obj_to_obj_index (obj, obj_index);
        str_replace_dup (&obj->name,        "");
        str_replace_dup (&obj->short_descr, "");
        str_replace_dup (&obj->description, "");
    }

    nested = FALSE;
    vnum = TRUE;
    nest = 0;

    while (1) {
        if (first)
            first = FALSE;
        else
            word = feof (fp) ? "End" : fread_word_static (fp);
        match = FALSE;

        switch (UPPER (word[0])) {
            case '*':
                match = TRUE;
                fread_to_eol (fp);
                break;

            case 'A':
                if (!str_cmp (word, "AffD")) {
                    AFFECT_T *paf;
                    int sn;

                    paf = affect_new ();
                    sn = skill_lookup (fread_word_static (fp));
                    if (sn < 0)
                        bug ("fread_obj: unknown skill.", 0);
                    else
                        paf->type = sn;

                    paf->level    = fread_number (fp);
                    paf->duration = fread_number (fp);
                    paf->modifier = fread_number (fp);
                    paf->apply    = fread_number (fp);
                    paf->bits     = fread_number (fp);

                    affect_to_obj_back (paf, obj);
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "Affc")) {
                    AFFECT_T *paf;
                    int sn;

                    paf = affect_new ();
                    sn = skill_lookup (fread_word_static (fp));
                    if (sn < 0)
                        bug ("fread_obj: unknown skill.", 0);
                    else
                        paf->type = sn;

                    paf->bit_type = fread_number (fp);
                    paf->level    = fread_number (fp);
                    paf->duration = fread_number (fp);
                    paf->modifier = fread_number (fp);
                    paf->apply    = fread_number (fp);
                    paf->bits     = fread_number (fp);

                    affect_to_obj_back (paf, obj);
                    match = TRUE;
                    break;
                }
                break;

            case 'C':
                KEY ("Cond", obj->condition, fread_number (fp));
                KEY ("Cost", obj->cost,      fread_number (fp));
                break;

            case 'D':
                KEYS("Description", obj->description, fread_string_dup (fp));
                KEYS("Desc",        obj->description, fread_string_dup (fp));
                break;

            case 'E':
                if (!str_cmp (word, "Enchanted")) {
                    obj->enchanted = TRUE;
                    match = TRUE;
                    break;
                }

                KEY ("ExtraFlags", obj->extra_flags, fread_flag (fp, extra_flags));
                KEY ("ExtF",       obj->extra_flags, fread_flag (fp, extra_flags));

                if (!str_cmp (word, "ExtraDescr") || !str_cmp (word, "ExDe")) {
                    EXTRA_DESCR_T *ed;

                    ed = extra_descr_new ();
                    fread_string_replace (fp, &ed->keyword);
                    fread_string_replace (fp, &ed->description);
                    extra_descr_to_obj_back (ed, obj);
                    match = TRUE;
                }

                if (!str_cmp (word, "End")) {
                    if (!nested || (vnum && obj->obj_index == NULL)) {
                        bug ("fread_obj: incomplete object.", 0);
                        obj_free (obj);
                        return;
                    }
                    else {
                        if (!vnum) {
                            obj_free (obj);
                            obj = obj_create (obj_get_index (OBJ_VNUM_DUMMY), 0);
                        }
                        if (!new_format) {
                            LIST2_FRONT (obj, global_prev, global_next,
                                object_first, object_last);
                        }

                        if (!obj->obj_index->new_format &&
                            item_is_armor (obj) &&
                            obj->v.armor.vs_bash == 0)
                        {
                            obj->v.armor.vs_bash  = obj->v.armor.vs_pierce;
                            obj->v.armor.vs_slash = obj->v.armor.vs_pierce;
                        }
                        if (make_new) {
                            int wear;

                            wear = obj->wear_loc;
                            obj_extract (obj);

                            obj = obj_create (obj->obj_index, 0);
                            obj->wear_loc = wear;
                        }
                        if (nest == 0 || obj_nest[nest] == NULL)
                            obj_give_to_char (obj, ch);
                        else
                            obj_give_to_obj (obj, obj_nest[nest - 1]);
                        return;
                    }
                }
                break;

            case 'I':
                KEY ("ItemType", obj->item_type, fread_number (fp));
                KEY ("Ityp", obj->item_type, fread_number (fp));
                break;

            case 'L':
                KEY ("Level", obj->level, fread_number (fp));
                KEY ("Lev", obj->level, fread_number (fp));
                break;

            case 'N':
                KEYS("Name", obj->name, fread_string_dup (fp));

                if (!str_cmp (word, "Nest")) {
                    nest = fread_number (fp);
                    if (nest < 0 || nest >= MAX_NEST)
                        bug ("fread_obj: bad nest %d.", nest);
                    else {
                        obj_nest[nest] = obj;
                        nested = TRUE;
                    }
                    match = TRUE;
                }
                break;

            case 'O':
                if (!str_cmp (word, "Oldstyle")) {
                    if (obj->obj_index != NULL && obj->obj_index->new_format)
                        make_new = TRUE;
                    match = TRUE;
                }
                break;

            case 'S':
                KEYS("ShortDescr", obj->short_descr, fread_string_dup (fp));
                KEYS("ShD",        obj->short_descr, fread_string_dup (fp));

                if (!str_cmp (word, "Spell")) {
                    int value, sn;
                    value = fread_number (fp);
                    sn = skill_lookup (fread_word_static (fp));

                    if (value < 0 || value >= OBJ_VALUE_MAX)
                        bug ("fread_obj: bad value %d.", value);
                    else if (sn < 0)
                        bug ("fread_obj: unknown skill.", 0);
                    else
                        obj->v.value[value] = sn;
                    match = TRUE;
                }
                break;

            case 'T':
                KEY ("Timer", obj->timer, fread_number (fp));
                KEY ("Time", obj->timer, fread_number (fp));
                break;

            case 'V':
                if (!str_cmp (word, "Values") || !str_cmp (word, "Vals")) {
                    obj->v.value[0] = fread_number (fp);
                    obj->v.value[1] = fread_number (fp);
                    obj->v.value[2] = fread_number (fp);
                    obj->v.value[3] = fread_number (fp);
                    if (item_is_weapon (obj) && obj->v.weapon.weapon_type == 0)
                        obj->v.weapon.weapon_type =
                            obj->obj_index->v.weapon.weapon_type;
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "Val")) {
                    obj->v.value[0] = fread_number (fp);
                    obj->v.value[1] = fread_number (fp);
                    obj->v.value[2] = fread_number (fp);
                    obj->v.value[3] = fread_number (fp);
                    obj->v.value[4] = fread_number (fp);
                    match = TRUE;
                    break;
                }

                if (!str_cmp (word, "Vnum")) {
                    int vnum;

                    vnum = fread_number (fp);
                    if ((obj_index = obj_get_index (vnum)) == NULL)
                        bug ("fread_obj: bad vnum %d.", vnum);
                    else {
                        obj_to_obj_index (obj, obj_index);
                        vnum = TRUE;
                    }
                    match = TRUE;
                    break;
                }
                break;

            case 'W':
                KEY ("WearFlags", obj->wear_flags, fread_flag (fp, wear_flags));
                KEY ("WeaF",      obj->wear_flags, fread_flag (fp, wear_flags));
                KEY ("WearLoc",   obj->wear_loc,   fread_number (fp));
                KEY ("Wear",      obj->wear_loc,   fread_number (fp));
                KEY ("Weight",    obj->weight,     fread_number (fp));
                KEY ("Wt",        obj->weight,     fread_number (fp));
                break;
        }

        if (!match) {
            bug ("fread_obj: no match.", 0);
            fread_to_eol (fp);
        }
    }
}
