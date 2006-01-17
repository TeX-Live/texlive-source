/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/lib/RCS/seek.c,v 3.1 89/08/22 21:59:04 chris Exp $";
#endif

/*
 * SeekFile copies an input stdio file, if necessary, so that
 * it produces a stdio file on which fseek() works properly.
 * It returns NULL if this cannot be done; in that case, the
 * input file is closed (or otherwise rendered worthless).
 *
 * CopyFile copies an input file unconditionally.  (On non-Unix
 * machines, it might `accidentally' not copy it.)
 *
 * On Unix machines, this means `if the input is a pipe or tty,
 * copy it to a temporary file'.  On other systems, all stdio files
 * might happen to be seekable.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef KPATHSEA
#include <kpathsea/c-fopen.h>
#endif

#include <stdio.h>
#include "types.h"		/* for BSD_FILE_SYSTEM */
#include "seek.h"

#ifdef HAVE_PROTOTYPES
extern int MakeRWTempFile(char *);
#else
extern int MakeRWTempFile();
#endif

#ifdef EASY_BUT_NOT_GOOD

FILE *CopyFile(f) FILE *f; { return (f); }
FILE *SeekFile(f) FILE *f; { return (f); }

#else

#include <errno.h>
#ifdef BSD_FILE_SYSTEM
#include <sys/param.h>		/* want MAXBSIZE */
#else
#include <sys/types.h>
#endif
#include <sys/stat.h>

#ifndef KPATHSEA
long	lseek();
char	*malloc();

extern int errno;
#endif

/*
 * Make and return a version of `f' on which fseek works (unconditionally).
 * This code is somewhat Unix-specific.
 */
FILE *
CopyFile(f)
	FILE *f;
{
	register int tf, n, ifd, w;
	register char *p, *buf;
	register int blksize;
	struct stat st;
	int e;
#ifdef MAXBSIZE
#define BSIZE MAXBSIZE
#else
#define BSIZE BUFSIZ
#endif
	char stackbuf[BSIZE];
#if defined(WIN32) || defined(MSDOS) || defined(__CYGWIN32__)
	int orig_fdmode;
#endif
	char *open_mode;

	/* get a read/write temp file which will vanish when closed */
	if ((tf = MakeRWTempFile(stackbuf)) < 0) {
		e = errno;
		(void) fclose(f);
		errno = e;
		return (NULL);
	}

	/* compute buffer size and choose buffer */
	ifd = fileno(f);
	buf = stackbuf;
	blksize = sizeof stackbuf;
#ifdef BSD_FILE_SYSTEM
	if (fstat(tf, &st) == 0 && st.st_blksize > blksize) {
		/*
		 * The output block size is the important one,
		 * but the input block size is not irrelevant.
		 *
		 * This should actually compute the lcm, but we
		 * will rely on block sizes being powers of two
		 * (so that the larger is the lcm).
		 */
		blksize = st.st_blksize;
		if (fstat(ifd, &st) == 0 && st.st_blksize > blksize)
			blksize = st.st_blksize;
		if ((buf = malloc((unsigned)blksize)) == NULL) {
			buf = stackbuf;
			blksize = sizeof stackbuf;
		}
	}
#endif

#if defined(WIN32) || defined(MSDOS) || defined(__CYGWIN32__)
	/* make sure we open the temp file in the same mode that
	   the original handle was open.  */
	orig_fdmode = setmode(ifd, 0);
	setmode(tf, orig_fdmode);
#endif
	/* copy from input file to temp file */
	(void) lseek(ifd, 0L, 0);	/* paranoia */
	while ((n = read(ifd, p = buf, blksize)) > 0) {
		do {
			if ((w = write(tf, p, n)) < 0) {
				(void) close(tf);
				(void) fclose(f);
				return (NULL);
			}
			p += w;
		} while ((n -= w) > 0);
	}
	e = errno;		/* in case n < 0 */
	if (buf != stackbuf)
		free(buf);
	if (n < 0) {
		(void) close(tf);
		(void) fclose(f);
		errno = e;
		return (NULL);
	}

	/* discard the input file, and rewind and open the temporary */
	(void) fclose(f);
	(void) lseek(tf, 0L, 0);
	errno = 0;
#if defined(WIN32) || defined(MSDOS) || defined(__CYGWIN32__)
	open_mode =  orig_fdmode == O_BINARY ? FOPEN_RBIN_MODE : "r";
#else
	open_mode = "r";
#endif
	if ((f = fdopen(tf, open_mode)) == NULL) {
		if (errno == 0)
			e = EMFILE;
		(void) close(tf);
		errno = e;
	}
	return (f);
}

/*
 * Copy an input file, but only if necessary.
 */
FILE *SeekFile(f)
	FILE *f;
{
	int fd = fileno(f);

	return (lseek(fd, 0L, 1) >= 0 && !isatty(fd) ? f : CopyFile(f));
}

#endif /* EASY_BUT_NOT_GOOD */
