#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: ttfutil.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

/* FixedSplit: split Fixed in to two interger (16.16) */
void FixedSplit(Fixed f,int b[])
{
    b[0] = f & 0xff00;
    b[1] = f >> 16;
}

char *
TagToStr(ULONG tag)
{
    static char str[5] = "XXXX";

    str[0] = (tag >> 24) & 0xff;
    str[1] = (tag >> 16) & 0xff;
    str[2] = (tag >> 8) & 0xff;
    str[3] = tag & 0xff;
    return str;
}

void ttfError(const char * msg)
{
    fprintf(stderr,"%s",msg);
    exit(EXIT_FAILURE);
}

#ifndef KPATHSEA
/* Functions copied or adapted from kpathsea.  */
long
xftell (FILE *fp, const char *funcname)
{
    long where = ftell (fp);

    if (where < 0)
        FATAL_PERROR(funcname);

    return where;
}

void
xfseek (FILE *f, long offset, int wherefrom, const char *funcname)
{
    if (fseek (f, offset, wherefrom) < 0)
        FATAL_PERROR(funcname);
}

void *
xmalloc (size_t size)
{
    void *new_mem = (void *)malloc(size ? size : 1);

    if (new_mem == NULL) {
        fprintf(stderr, "fatal: memory exhausted (xmalloc of %lu bytes).\n",
                (unsigned long)size);
        exit(EXIT_FAILURE);
    }

    return new_mem;
}

void *
xcalloc (size_t nelem, size_t elsize)
{
    void *new_mem = (void*)calloc(nelem ? nelem : 1, elsize ? elsize : 1);

    if (new_mem == NULL) {
        fprintf(stderr,
                "xcalloc: request for %lu elements of size %lu failed.\n",
                (unsigned long)nelem, (unsigned long)elsize);
        exit(EXIT_FAILURE);
    }

    return new_mem;
}

char *
xstrdup (const char *s)
{
  char *new_string = xmalloc(strlen (s) + 1);
  return strcpy(new_string, s);
}
#endif /* !KPATHSEA */
