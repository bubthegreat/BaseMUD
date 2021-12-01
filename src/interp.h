

#ifndef __ROM_INTERP_H
#define __ROM_INTERP_H

#include "merc.h"

extern const CMD_T cmd_table[];

/* Function prototpyes. */
void interpret (CHAR_T *ch, char *argument);
const char *interpret_pos_message (int pos);
void do_function (CHAR_T *ch, DO_FUN *do_fun, char *argument);
bool check_social (CHAR_T *ch, char *command, char *argument);
bool is_number (const char *arg);
int number_argument (const char *arg_in, char *arg_out);
int mult_argument (const char *arg_in, char *arg_out);
char *one_argument (const char *argument, char *arg_first);

#endif
