

#ifndef __ROM_BOARD_H
#define __ROM_BOARD_H

#include "merc.h"

/* Definitions and global variables. */
#define L_SUP (MAX_LEVEL - 1) /* if not already defined */
#define BOARD_NOACCESS -1
#define BOARD_NOTFOUND -1

/* Board functions. */
int board_number (const BOARD_T *board);
void board_save_all ();
void board_load (BOARD_T *board);
void board_load_all ();
void board_save (BOARD_T *board);
int board_get_unread_notes_for_char (BOARD_T *board, CHAR_T *ch);
NOTE_T *board_find_note (BOARD_T *board, CHAR_T *ch, int num);

/* Note functions. */
void note_write (NOTE_T *note, FILE *fp);
void note_finish (NOTE_T *note, BOARD_T *board);
void note_unlink (NOTE_T *note, BOARD_T *board);
bool note_is_for_char (NOTE_T *note, CHAR_T *ch);
void note_show_to_char (NOTE_T *note, CHAR_T *ch, int num);
NOTE_T *note_create_personal (const char *sender, const char *to,
    const char *subject, const int expire_days, const char *text);
NOTE_T *note_create (const char *board_name, const char *sender, const char *to,
    const char *subject, const int expire_days, const char *text);
void note_to_board (NOTE_T *note, BOARD_T *board);

/* For nanny */
DECLARE_NANNY_FUN (handle_con_note_to);
DECLARE_NANNY_FUN (handle_con_note_subject);
DECLARE_NANNY_FUN (handle_con_note_expire);
DECLARE_NANNY_FUN (handle_con_note_text);
DECLARE_NANNY_FUN (handle_con_note_finish);

#endif
