

#ifndef __ROM_COMPAT_H
#define __ROM_COMPAT_H

#include "defs.h"

#if defined(macintosh)
    #include <types.h>
#else
    #include <sys/types.h>
    #include <sys/time.h>
#endif

/* Accommodate old non-Ansi compilers. */
#if defined(TRADITIONAL)
    #define const
    #define args(list) ()
#else
    #define args(list) list
#endif

#if defined(_AIX)
    typedef int sh_int;
    typedef int bool;
    #define unix
#else
    typedef short int sh_int;
    typedef unsigned char bool;
#endif

/* system calls */
int unlink();
int system();

/* OS-dependent declarations.
 * These are all very standard library functions,
 *   but some systems have incomplete or non-ansi header files. */
#if defined(_AIX)
    char *crypt args( ( const char *key, const char *salt ) );
#endif

#if defined(apollo)
    int atoi args( ( const char *string ) );
    void * calloc args( ( unsigned nelem, size_t size ) );
    char *crypt args( ( const char *key, const char *salt ) );
#endif

#if defined(hpux)
    char *crypt args( ( const char *key, const char *salt ) );
#endif

#if defined(linux)
    char *crypt args( ( const char *key, const char *salt ) );
#endif

#if defined(macintosh)
    #define NOCRYPT
    #if defined(unix)
        #undef    unix
    #endif
#endif

#if defined(MIPS_OS)
    char *crypt args( ( const char *key, const char *salt ) );
#endif

#if defined(MSDOS)
    #define NOCRYPT
    #if defined(unix)
        #undef unix
    #endif
#endif

#if defined(NeXT)
    char *crypt args( ( const char *key, const char *salt ) );
#endif

#if defined(sequent)
    char *crypt args( ( const char *key, const char *salt ) );
    int fclose args( ( FILE *stream ) );
    int fprintf args( ( FILE *stream, const char *format, ... ) );
    int fread args( ( void *ptr, int size, int n, FILE *stream ) );
    int fseek args( ( FILE *stream, long offset, int ptrname ) );
    void perror args( ( const char *s ) );
    int ungetc args( ( int c, FILE *stream ) );
#endif

#if defined(sun)
    char *crypt args( ( const char *key, const char *salt ) );
    int fclose args( ( FILE *stream ) );
    int fprintf args( ( FILE *stream, const char *format, ... ) );
#if defined(SYSV)
    siz_t fread args( ( void *ptr, size_t size, size_t n, FILE *stream) );
#elif !defined(__SVR4)
    int fread args( ( void *ptr, int size, int n, FILE *stream ) );
#endif
    int fseek args( ( FILE *stream, long offset, int ptrname ) );
    void perror args( ( const char *s ) );
    int ungetc args( ( int c, FILE *stream ) );
#endif

#if defined(ultrix)
    char *crypt args( ( const char *key, const char *salt ) );
#endif

/* The crypt(3) function is not available on some operating systems.
 * In particular, the U.S. Government prohibits its export from the
 *   United States to foreign countries.
 * Turn on NOCRYPT to keep passwords in plain text. */

#if defined(NOCRYPT)
    #define crypt(s1, s2) (s1)
#endif

#endif
