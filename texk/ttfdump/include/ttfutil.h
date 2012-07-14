#ifndef __TTF_UTIL_H
#define __TTF_UTIL_H

#include "ttf.h"

/* $Id: ttfutil.h,v 1.2 1998/07/06 06:07:01 werner Exp $ */

#if !defined(EXIT_FAILURE)
#define EXIT_FAILURE 1
#endif

#if defined __GNUC__ && __GNUC__ >=3
__attribute__((__noreturn__))
#endif
void ttfError(const char *msg);

void FixedSplit(Fixed f, int b[]);
#define FT_MAKE_TAG( _x1, _x2, _x3, _x4 ) _x1 << 24 | _x2 << 16 | _x3 << 8 | _x4
char *TagToStr(ULONG tag);

#ifdef KPATHSEA

#include <stdlib.h>
#include <kpathsea/lib.h>
#include <kpathsea/tex-file.h>

static inline FILE *
fopen_truetype (const char *path)
{
    FILE *fp;
    char *p;

    if ((p = kpse_find_file (path, kpse_truetype_format, 0)) == NULL)
        return NULL;

    fp = fopen (p, "rb");
    free (p);

    return fp;
}

#else /* !KPATHSEA */

/* Functions copied or adapted from kpathsea.  */
void xfseek (FILE *fp, long offset, int wherefrom, const char *funcname);
long xftell (FILE *fp, const char *funcname);
void *xmalloc (size_t size);
void *xcalloc (size_t nelem, size_t elsize);
char *xstrdup(const char *s);

#define FATAL_PERROR(str) do { perror (str); exit (EXIT_FAILURE); } while (0)
#define XTALLOC(n, t) ((t *) xmalloc ((n) * sizeof (t)))
#define XTALLOC1(t) XTALLOC (1, t)

#define fopen_truetype(path) fopen (path, "rb")

#endif /* !KPATHSEA */

#define XCALLOC(n, t) ((t *) xcalloc (n, sizeof (t)))
#define XCALLOC1(t) XCALLOC (1, t)

#include "protos.h"

#endif /* __TTF_UTIL_H */


/* end of ttfutil.h */
