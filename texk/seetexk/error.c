/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/lib/RCS/error.c,v 2.8 89/08/22 21:49:12 chris Exp $";
#endif

/*
 * Print an error message with an optional system error number, and
 * optionally quit.
 *
 */
#include <stdio.h>

#include <c-auto.h>
#include "types.h"		/* for HAVE_VPRINTF */
#include "error.h"

#if defined(lint) && !defined(LINT_ANYWAY)

/* ARGSUSED */
void SetErrorTrap(fn) void (*fn)(); {;}

/* VARARGS3 ARGSUSED */
void error(quit, e, fmt) int quit, e; char *fmt; {;}

/* VARARGS1 ARGSUSED */
void panic(fmt) char *fmt; { exit(1); /* NOTREACHED */ }

#else /* lint */

extern char *ProgName;		/* program name from argv[0] */

#include <errno.h>
#ifndef errno
extern int errno;
#endif

static FILE *trap_file;		/* error diversion file, if any */
static void (*trap_fn)();	/* trap function */
static char *trap_buf;		/* buffer for trapped error strings */
static int trap_size;		/* size of trap_buf */

#ifndef KPATHSEA
extern char *malloc(), *realloc();
#endif

extern int MakeRWTempFile();


#if !defined (HAVE_STRERROR) && !defined (strerror)
static char *
strerror (errnum)
     int errnum;
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
static char *
readback()
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
verror(quit, a0, fmt, l, e)
	char *a0, *fmt;
	va_list l;
	int e;
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
void	error PVAR3C(int, quit,  int, e,  const char *, fmt,  l)
{
	if (e < 0)
		e = errno;
	verror(quit, (char *)NULL, fmt, l, e);
	va_end(l);
}}
/*
 * Panic (print to stderr and abort).
 */
void	panic PVAR1C(const char *, fmt,  l)
{
	verror(0, "panic: ", fmt, l, 0);
	va_end(l);
	abort();
}}

/*
 * Enable error trapping: register the function fn as the trapper.
 * If fn is NULL, stop trapping.
 */
void
SetErrorTrap(fn)
	void (*fn)(int, char *);
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

#endif /* lint */
