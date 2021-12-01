

#include "utils.h"
#include "fread.h"

#include "quickmud.h"

void qmconfig_read (void) {
    FILE *fp;
    bool match, reading;
    char *word;
    extern int mud_ansiprompt, mud_ansicolor, mud_telnetga;

    log_f ("Loading configuration settings from %s.", QMCONFIG_FILE);
    fp = fopen (QMCONFIG_FILE, "r");
    if (!fp) {
        log_f ("%s not found. Using compiled-in defaults.", QMCONFIG_FILE);
        return;
    }

    reading = TRUE;
    while (reading) {
        word = feof (fp) ? "END" : fread_word_static (fp);
        match = FALSE;

        switch (UPPER(word[0])) {
            case '#':
                /* This is a comment line! */
                match = TRUE;
                fread_to_eol (fp);
                break;

            case '*':
                match = TRUE;
                fread_to_eol (fp);
                break;

            case 'A':
                KEY ("Ansicolor",  mud_ansicolor,  fread_number(fp));
                KEY ("Ansiprompt", mud_ansiprompt, fread_number(fp));
                break;

            case 'E':
                if (!str_cmp (word, "END")) {
                    reading = FALSE;
                    match = TRUE;
                }
                break;

            case 'T':
                KEY ("Telnetga", mud_telnetga, fread_number(fp));
                break;
        }
        if (!match) {
            log_f ("qmconfig_read: no match for %s!", word);
            fread_to_eol(fp);
        }
    }

 // log_f ("Settings have been read from %s", QMCONFIG_FILE);
    fclose (fp);
}
