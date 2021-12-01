

#ifndef __ROM_DESCS_H
#define __ROM_DESCS_H

#include "merc.h"

/* OS-dependent local functions. */
#if defined(macintosh) || defined(MSDOS)
    bool read_from_descriptor args ((DESCRIPTOR_T *d));
    bool write_to_descriptor args ((int desc, char *txt, int length));
#endif

#if defined(unix)
    int init_socket args ((int port));
    void init_descriptor args ((int control));
    bool read_from_descriptor args ((DESCRIPTOR_T *d));
    bool write_to_descriptor args ((int desc, char *txt, int length));
#endif

void close_socket (DESCRIPTOR_T *dclose);
void read_from_buffer (DESCRIPTOR_T *d);
bool process_output (DESCRIPTOR_T *d, bool prompt);
bool desc_flush_output (DESCRIPTOR_T *d);
void write_to_buffer (DESCRIPTOR_T *d, const char *txt, int length);
bool check_reconnect (DESCRIPTOR_T *d, char *name, bool conn);
bool check_playing (DESCRIPTOR_T *d, char *name);
void send_to_desc (const char *txt, DESCRIPTOR_T *d);
void clear_page (DESCRIPTOR_T *d);
void append_to_page (DESCRIPTOR_T *d, const char *txt);
int show_page (DESCRIPTOR_T *d);
void printf_to_desc (DESCRIPTOR_T *d, char *fmt, ...);
void desc_substitute_alias (DESCRIPTOR_T *d, char *argument);

#endif
