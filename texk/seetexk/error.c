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
 * Print an error message with an optional system error number, and
 * optionally quit.
 *
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#include "types.h"
#include "error.h"
#include "tempfile.h"

extern char *ProgName;		/* program name from argv[0] */

#include <errno.h>
#ifndef errno
extern int errno;
#endif

static FILE *trap_file;		/* error diversion file, if any */
static void (*trap_fn)(int, const char *);	/* trap function */
static char *trap_buf;		/* buffer for trapped error strings */
static int trap_size;		/* size of trap_buf */

#ifndef KPATHSEA
extern char *malloc(), *realloc();
#endif

#if !defined (HAVE_STRERROR) && !defined (strerror)
static char *
strerror (int errnum)
{
  extern char *sys_errlist[];
  extern int sys_nerr;
 
  return 0 < errnum && errnum <= sys_nerr
         ? sys_errlist[errnum] : "Unknown system error";
}
#endif /* not HAVE_STRERROR && not strerror */

/*
 * Read a trapped error into trap_buf.
 * Return a pointer to the (NUL-terminated) text.
 * If something goes wrong, return something else printable.
 */
static const char *
readback(void)
{
	int nbytes = ftell(trap_file) + 1;

	if (nbytes > trap_size) {
		if (trap_buf == NULL)
			trap_buf = malloc((unsigned)nbytes);
		else
			trap_buf = realloc(trap_buf, (unsigned)nbytes);
		if (trap_buf == NULL) {
			trap_size = 0;
			return ("Ouch!  Lost error text: out of memory?");
		}
	}
	rewind(trap_file);	/* now can read */
	nbytes = fread(trap_buf, 1, nbytes, trap_file);
	if (nbytes < 0)
		return ("Ouch!  Trouble reading error text!");
	trap_buf[nbytes] = 0;
	return (trap_buf);
}

/*
 * Print an error message to the error output (either stderr, or
 * if trapping errors, to the error trap file).  We precede this
 * with the program's name and an optional string (a0), then use
 * the format and variable argument list, then append an optional
 * Unix error string.  Finally, if errors are being trapped, we
 * pass the error text and the quit flag to the trap function.
 *
 * In the interest of `look and feel', if errors are being trapped,
 * the program name is omitted.
 */
static void
verror(int quit, const char *a0, const char *fmt, va_list l, int e)
{
	register FILE *fp = trap_file;

	/* print to the trap file, if any, else stderr */
	if ((fp = trap_file) != NULL)
		rewind(fp);		/* now can write */
	else {
		fp = stderr;
		(void) fflush(fp);
	}
	if (trap_file == NULL)
		(void) fprintf(fp, "%s: ", ProgName);
	if (a0)
		(void) fprintf(fp, "%s", a0);
	(void) vfprintf(fp, fmt, l);
	if (e)
		(void) fprintf(fp, ": %s", strerror(e));
	(void) putc('\n', fp);
	(void) fflush(fp);
	if (trap_file != NULL)
		(*trap_fn)(quit, readback());
	if (quit)
		exit(quit);
}

/*
 * Print an error message and optionally quit.
 */
void
error(int quit, int e, const char *fmt, ...)
{
	va_list l;
	va_start(l, fmt);
	if (e < 0)
		e = errno;
	verror(quit, NULL, fmt, l, e);
	va_end(l);
}
/*
 * Panic (print to stderr and abort).
 */
void
panic(const char *fmt, ...)
{
	va_list l;
	va_start(l, fmt);
	verror(0, "panic: ", fmt, l, 0);
	va_end(l);
	abort();
}

/*
 * Enable error trapping: register the function fn as the trapper.
 * If fn is NULL, stop trapping.
 */
void
SetErrorTrap(void (*fn)(int, const char *))
{
	int tempfd;
	char fname[BUFSIZ];

	/* shut down any existing error trap */
	if (trap_file) {
		(void) fclose(trap_file);
		trap_file = NULL;
	}
	if ((trap_fn = fn) == NULL)
		return;
	/* begin trapping */
	if ((tempfd = MakeRWTempFile(fname)) < 0)
		error(1, -1, "cannot create temporary file %s", fname);
	if (trap_size == 0) {
		trap_buf = malloc((unsigned)(trap_size = 1000));
		if (trap_buf == 0)
			error(1, -1,
			    "cannot get space for error buffer");
	}
	if ((trap_file = fdopen(tempfd, "r+")) == NULL)
		error(1, -1, "cannot get stdio file for error trap");
}
