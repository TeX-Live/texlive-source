/* eptexextra.c: Hand-coded routines for e-pTeX.

   This file is public domain.  */

#define	EXTERN /* Instantiate data from eptexd.h here.  */
#define DLLPROC dlleptexmain

/* This file defines TeX and epTeX.  */
#include <eptexd.h>

/* Hand-coded routines for TeX or Metafont in C.  */
#include <lib/texmfmp.c>

void getfiledump(int s, int offset, int length)
{
    FILE *f;
    int read, i;
    poolpointer data_ptr;
    poolpointer data_end;
    char *file_name;

    if (length == 0) {
        /* empty result string */
        return;
    }

    if (poolptr + 2 * length + 1 >= poolsize) {
        /* no place for result */
        poolptr = poolsize;
        /* error by str_toks that calls str_room(1) */
        return;
    }

    file_name = kpse_find_tex(makecfilename(s));
    if (file_name == NULL) {
        return;                 /* empty string */
    }

    /* read file data */
    f = fopen(file_name, FOPEN_RBIN_MODE);
    if (f == NULL) {
        xfree(file_name);
        return;
    }
    recorder_record_input(file_name);
    if (fseek(f, offset, SEEK_SET) != 0) {
        xfree(file_name);
        return;
    }
    /* there is enough space in the string pool, the read
       data are put in the upper half of the result, thus
       the conversion to hex can be done without overwriting
       unconverted bytes. */
    data_ptr = poolptr + length;
    read = fread(&strpool[data_ptr], sizeof(char), length, f);
    fclose(f);

    /* convert to hex */
    data_end = data_ptr + read;
    for (; data_ptr < data_end; data_ptr++) {
        i = snprintf((char *) &strpool[poolptr], 3,
                     "%.2X", (unsigned int) strpool[data_ptr]);
        check_nprintf(i, 3);
        poolptr += i;
    }
    xfree(file_name);
}
