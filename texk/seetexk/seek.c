/* Copyright (c) 1987, 1989, 2012 University of Maryland Department of
   Computer Science.
   
   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation, the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions: The above copyright notice, this permission
   notice and the disclaimer statement shall be included in all copies
   or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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
#include "tempfile.h"

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
CopyFile(FILE *f)
{
	register int tf, n, ifd, w;
	register char *p, *buf;
	register int blksize;
#ifdef BSD_FILE_SYSTEM
	struct stat st;
#endif
	int e;
#ifdef MAXBSIZE
#define BSIZE MAXBSIZE
#else
#define BSIZE BUFSIZ
#endif
	char stackbuf[BSIZE];
#if defined(WIN32) || defined(MSDOS)
	int orig_fdmode;
#endif
	const char *open_mode;

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

#if defined(WIN32) || defined(MSDOS)
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
#if defined(WIN32) || defined(MSDOS)
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
FILE *SeekFile(FILE *f)
{
	int fd = fileno(f);

	return (lseek(fd, 0L, 1) >= 0 && !isatty(fd) ? f : CopyFile(f));
}
