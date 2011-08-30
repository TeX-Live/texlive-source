/*
 *  Dump/undump Kanji encoding for (e)upTeX.
 */

#include "kanji.h"
#include <texmfmp.h>

void dump_kanji (FILE *fp)
{
    char buffer[12];
    const char *p = get_enc_string ();
    size_t len = strlen (p);

    if (len > 11) {
        fprintf (stderr, "Kanji encoding string \"%s\" exceeds 11 bytes.\n", p);
        uexit(1);
    }

    strcpy (buffer, p);
    for (len++; len < 12; len++)
        buffer[len] = 0;
    do_dump (buffer, 1, 12, fp);
}

void undump_kanji (FILE *fp)
{
    char buffer[12];
    char *p;

    do_undump (buffer, 1, 12, fp);
    buffer[11] = 0;  /* force string termination, just in case */

    p = strchr (buffer, '.');
    if (p)
        *p++ = 0;
    else
        p = buffer;

    /* Now BUFFER and P are the file and internal encoding strings.  */
    init_kanji (NULL, p);
}
