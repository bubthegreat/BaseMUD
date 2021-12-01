

#ifndef __ROM_COLOUR_H
#define __ROM_COLOUR_H

#include "merc.h"

/* Colour Bits */
#define CB_RED          (BIT_01)
#define CB_GREEN        (BIT_02)
#define CB_BLUE         (BIT_03)
#define CB_BRIGHT       (BIT_04)
#define CB_BACK_RED     (BIT_05)
#define CB_BACK_GREEN   (BIT_06)
#define CB_BACK_BLUE    (BIT_07)
#define CB_BACK_BRIGHT  (BIT_08)
#define CB_BEEP         (BIT_09)
#define CB_DEFAULT      (BIT_10)
#define CB_BACK_DEFAULT (BIT_11)

/* Colour bitMasks */
#define CM_FORECOLOUR   ((BIT_01) | (BIT_02) | (BIT_03) | (BIT_04) | (BIT_10))
#define CM_BACKCOLOUR   ((BIT_05) | (BIT_06) | (BIT_07) | (BIT_08) | (BIT_11))
#define CM_BEEP         (BIT_09)

/* Colour Codes */
#define CC_CLEAR               (CB_DEFAULT | CB_BACK_DEFAULT)
#define CC_DEFAULT             (CB_DEFAULT)
#define CC_BLACK               (0x00                       )
#define CC_RED                 (CB_RED                     )
#define CC_GREEN               (         CB_GREEN          )
#define CC_YELLOW              (CB_RED | CB_GREEN          )
#define CC_BLUE                (                    CB_BLUE)
#define CC_MAGENTA             (CB_RED            | CB_BLUE)
#define CC_CYAN                (         CB_GREEN | CB_BLUE)
#define CC_WHITE               (CB_RED | CB_GREEN | CB_BLUE)
#define CC_BRIGHT_DEFAULT      (CB_BRIGHT | CC_DEFAULT)
#define CC_DARK_GREY           (CB_BRIGHT | CC_BLACK)
#define CC_BRIGHT_RED          (CB_BRIGHT | CC_RED)
#define CC_BRIGHT_GREEN        (CB_BRIGHT | CC_GREEN)
#define CC_BRIGHT_YELLOW       (CB_BRIGHT | CC_YELLOW)
#define CC_BRIGHT_BLUE         (CB_BRIGHT | CC_BLUE)
#define CC_BRIGHT_MAGENTA      (CB_BRIGHT | CC_MAGENTA)
#define CC_BRIGHT_CYAN         (CB_BRIGHT | CC_CYAN)
#define CC_BRIGHT_WHITE        (CB_BRIGHT | CC_WHITE)
#define CC_BACK_DEFAULT        (CB_BACK_DEFAULT)
#define CC_BACK_BLACK          (CC_BLACK   << 4)
#define CC_BACK_RED            (CC_RED     << 4)
#define CC_BACK_GREEN          (CC_GREEN   << 4)
#define CC_BACK_YELLOW         (CC_YELLOW  << 4)
#define CC_BACK_BLUE           (CC_BLUE    << 4)
#define CC_BACK_MAGENTA        (CC_MAGENTA << 4)
#define CC_BACK_CYAN           (CC_CYAN    << 4)
#define CC_BACK_WHITE          (CC_WHITE   << 4)
#define CC_BACK_BRIGHT_DEFAULT (CC_BACK_BRIGHT | CB_BACK_DEFAULT)
#define CC_BACK_DARK_GREY      (CC_DARK_GREY      << 4)
#define CC_BACK_BRIGHT_RED     (CC_BRIGHT_RED     << 4)
#define CC_BACK_BRIGHT_GREEN   (CC_BRIGHT_GREEN   << 4)
#define CC_BACK_BRIGHT_YELLOW  (CC_BRIGHT_YELLOW  << 4)
#define CC_BACK_BRIGHT_BLUE    (CC_BRIGHT_BLUE    << 4)
#define CC_BACK_BRIGHT_MAGENTA (CC_BRIGHT_MAGENTA << 4)
#define CC_BACK_BRIGHT_CYAN    (CC_BRIGHT_CYAN    << 4)
#define CC_BACK_BRIGHT_WHITE   (CC_BRIGHT_WHITE   << 4)

/* functions */
int colour_to_full_name (flag_t colour, char *buf_out, size_t size);
int colour_code_to_ansi (CHAR_T *ch, bool use_colour, char type, char *buf_out,
    size_t size);
int colour_puts (CHAR_T *ch, bool use_colour, const char *buf_in, char *buf_out,
    size_t size);
int colour_to_ansi (flag_t colour, char *buf_out, size_t size);
const COLOUR_SETTING_T *colour_setting_get_by_char (char ch);

#endif
