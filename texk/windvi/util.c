/*========================================================================*\

Copyright (c) 1990-1999  Paul Vojta

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
PAUL VOJTA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

NOTE:
	xdvi is based on prior work, as noted in the modification history
	in xdvi.c.

\*========================================================================*/
#define HAVE_BOOLEAN
#include "xdvi-config.h"
#include <kpathsea/c-ctype.h>
#include <kpathsea/c-fopen.h>
#include <kpathsea/c-vararg.h>

#if defined(HTEX) && !defined(WIN32)
#include "HTEscape.h"
#endif

#ifndef WIN32
#include <pwd.h>
#endif
#include <sys/types.h>

#ifdef VMS
#include <rmsdef.h>
#endif /* VMS */

/*
 *	General utility routines.
 */

/*
 *	Print error message and quit.
 */

int lastwwwopen;

#ifdef HAVE_PROTOTYPES
NORETURN void
oops(_Xconst char *message, ...)
#else
/* VARARGS */
NORETURN void
oops(va_alist)
	va_dcl
#endif
{
  int msgLen;
#ifndef HAVE_PROTOTYPES
  _Xconst char *message;
#endif
  va_list	args;
  
#ifndef WIN32
  Fprintf(stderr, "%s: ", prog);
#ifdef HAVE_PROTOTYPES
  va_start(args, message);
#else
  va_start(args);
  message = va_arg(args, _Xconst char *);
#endif
  msgLen = vfprintf(stderr, message, args);
  va_end(args);
  Putc('\n', stderr);
#else /* WIN32 */
  if (msgLen > 0) {
    char *msgBuf = malloc(msgLen * sizeof(char) + 10);
    if (msgBuf) {
      va_start(args, message);
      vsprintf(msgBuf, message, args);
      va_end(args);
      if (MessageBox(NULL, msgBuf, 
		     "Fatal Error", 
		     MB_OK|MB_ICONERROR|MB_TOPMOST) == 0)
	Win32Error("util.c/oops()");
      free(msgBuf);
    }
  }
#endif
#if	PS
	psp.destroy();
#endif
	Exit(1);
}


#ifndef KPATHSEA

/*
 *	Either (re)allocate storage or fail with explanation.
 */

void *
xmalloc(size)
	unsigned	size;
{
	/* Avoid malloc(0), though it's not clear if it ever actually
	   happens any more.  */
	void *mem = malloc(size ? size : 1);

	if (mem == NULL)
	  oops("! Out of memory (allocating %u bytes).\n", size);
	return mem;
}

void *
xrealloc(where, size)
	char		*where;
	unsigned	size;
{
	void	*mem	= realloc(where, size);

	if (mem == NULL)
	  oops("! Out of memory (reallocating %u bytes).\n", size);
	return mem;
}

/*
 *	Allocate a new string.  The second argument is the length, or -1.
 */

char	*
xmemdup(str, len)
	_Xconst char	*str;
	int		len;
{
	char	*new;

	new = xmalloc(len);
	bcopy(str, new, len);
	return new;
}

#endif  /* not KPATHSEA */

/*
 *	Expand the matrix *ffline to at least the given size.
 */

void
expandline(n)
	int	n;
{
	int	newlen	= n + 128;

	ffline = (ffline == NULL) ? xmalloc(newlen) : xrealloc(ffline, newlen);
	ffline_len = newlen;
}


/*
 *	Allocate bitmap for given font and character
 */

void
alloc_bitmap(bitmap)
	struct bitmap *bitmap;
{
	unsigned int	size;

	/* width must be multiple of 16 bits for raster_op */
	bitmap->bytes_wide = ROUNDUP((int) bitmap->w, BMBITS) * BMBYTES;
	size = bitmap->bytes_wide * bitmap->h;
	bitmap->bits = xmalloc(size != 0 ? size : 1);
}


#ifndef KPATHSEA

/*
 *	Put a variable in the environment or abort on error.
 */

extern	char	**environ;

void
xputenv(var, value)
	_Xconst char	*var;
	_Xconst char	*value;
{

#if HAVE_PUTENV

	char	*buf;
	int	len1, len2;

	len1 = strlen(var);
	len2 = strlen(value) + 1;
	buf = xmalloc((unsigned int) len1 + len2 + 1);
	bcopy(var, buf, len1);
	buf[len1++] = '=';
	bcopy(value, buf + len1, len2);
	if (putenv(buf) != 0)
	    oops("! Failure in setting environment variable.");
	return;

#elif HAVE_SETENV

	if (setenv(var, value, True) != 0)
	    oops("! Failure in setting environment variable.");
	return;

#else /* not HAVE_{PUTENV,SETENV} */

	int		len1;
	int		len2;
	char		*buf;
	char		**linep;
	static	Boolean	did_malloc = False;

	len1 = strlen(var);
	len2 = strlen(value) + 1;
	buf = xmalloc((unsigned int) len1 + len2 + 1);
	bcopy(var, buf, len1);
	buf[len1++] = '=';
	bcopy(value, buf + len1, len2);
	for (linep = environ; *linep != NULL; ++linep)
	    if (memcmp(*linep, buf, len1) == 0) {
		*linep = buf;
		return;
	    }
	len1 = linep - environ;
	if (did_malloc)
	  environ = xrealloc(environ,
		(unsigned int) (len1 + 2) * sizeof(char *));
	else {
	    linep = xmalloc((unsigned int)(len1 + 2) * sizeof(char *));
	    bcopy((char *) environ, (char *) linep, len1 * sizeof(char *));
	    environ = linep;
	    did_malloc = True;
	}
	environ[len1++] = buf;
	environ[len1] = NULL;

#endif /* not HAVE_{PUTENV,SETENV} */

}

#endif /* not KPATHSEA */

#ifndef WIN32

/*
 *	Hopefully a self-explanatory name.  This code assumes the second
 *	argument is lower case.
 */

int
memicmp(s1, s2, n)
	_Xconst char	*s1;
	_Xconst char	*s2;
	size_t		n;
{
	while (n > 0) {
	    int i = TOLOWER(*s1) - *s2;
	    if (i != 0) return i;
	    ++s1;
	    ++s2;
	    --n;
	}
	return 0;
}

#endif /* !WIN32 */

/*
 *	Close the pixel file for the least recently used font.
 */

static	void
close_a_file()
{
	struct font *fontp;
	unsigned short oldest = ~0;
	struct font *f = NULL;

	if (debug & DBG_OPEN)
	    Puts("Calling close_a_file()");

	for (fontp = font_head; fontp != NULL; fontp = fontp->next)
	    if (fontp->file != NULL && fontp->timestamp <= oldest) {
		f = fontp;
		oldest = fontp->timestamp;
	    }
	if (f == NULL)
	    oops("Can't find an open pixel file to close");
	Fclose(f->file);
	f->file = NULL;
	++n_files_left;
}

/*
 *	This is necessary on some systems to work around a bug.
 */

#if SUNOS4
static	void
close_small_file()
{
	struct font *fontp;
	unsigned short oldest = ~0;
	struct font *f = NULL;

	if (debug & DBG_OPEN)
	    Puts("Calling close_small_file()");

	for (fontp = font_head; fontp != NULL; fontp = fontp->next)
	    if (fontp->file != NULL && fontp->timestamp <= oldest
	      && (unsigned char) fileno(fontp->file) < 128) {
		f = fontp;
		oldest = fontp->timestamp;
	    }
	if (f == NULL)
	    oops("Can't find an open pixel file to close");
	Fclose(f->file);
	f->file = NULL;
	++n_files_left;
}
#else
#define	close_small_file	close_a_file
#endif

#ifdef HTEX

/*
 *	Localize a local URL.  This is done in place, but the pointer
 *	returned probably points to somewhere _WITHIN_ the filename.
 *	So the caller needs to use the returned pointer and the returned
 *	pointer is unsuitable for free operations - janl 31/1/1999
 */

char *urlocalize P1C(char *,filename)

{
  if (strncmp(filename,"file:",5)==0) {

    if (debug & DBG_HYPER)
      fprintf(stderr,"Shortcircuting local file url: %s\n",filename);

    /* If it's a file: URL just remove the file: part, then
       we'll open it localy as a normal local file */
    filename+=5;
    /* If what we're left with starts in // then we need to remove
       a hostname part too */
    if (strncmp(filename,"//",2)==0) 
      filename=index(filename+2,'/');

#ifndef WIN32
    HTUnEscape(filename);
#endif
    if (debug & DBG_HYPER)
      fprintf(stderr,"Local filename is: %s\n",filename);

  }

  return filename;
}

#endif  /* HTEX */

/*
 *	Open a file in the given mode.  URL AWARE.
 */

FILE *
#ifndef	VMS
xfopen P2C(char *,filename, _Xconst char *,type)
#define	TYPE	type
#else
xfopen P3C(char *,filename, _Xconst char *,type, _Xconst char *,type2)
#define	TYPE	type, type2
#endif	/* VMS */
{
	FILE	*f;

        /* Try not to let the file table fill up completely.  */
	if (n_files_left <= 10)
	  close_a_file();
#ifdef HTEX

	filename=urlocalize(filename);

	if (URL_aware && 
	    (((URLbase != NULL) && htex_is_url(urlocalize(URLbase))) || 
	     (htex_is_url(filename)))) {
		int i;
		i = fetch_relative_url(URLbase, filename, temporary_dir);
		if (i < 0) return NULL;
		/* Don't bother waiting right now... */
		wait_for_urls(); 
		/* This needs to be set somehow... */
		f = fopen(filelist[i].file, TYPE); 
		if (debug & DBG_HYPER)
		  fprintf(stderr,"Opening %s for %s\n",
			  filelist[i].file,filelist[i].url);
		lastwwwopen=i;
	} else 
#endif /* HTEX */
	f = fopen(filename, TYPE);
	/* Interactive Unix 2.2.1 doesn't set errno to EMFILE
  	   or ENFILE even when it should, but if we do this
  	   unconditionally, then giving a nonexistent file on the
  	   command line gets the bizarre error `Can't find an open pixel
  	   file to close' instead of `No such file or directory'.  */
#ifndef	VMS
	if (f == NULL && (errno == EMFILE || errno == ENFILE))
#else	/* VMS */
	if (f == NULL && errno == EVMSERR && vaxc$errno == RMS$_ACC)
#endif	/* VMS */
	{
	    n_files_left = 0;
	    close_a_file();
	    f = fopen(filename, TYPE);
	}
#ifdef	F_SETFD
	if (f != NULL) (void) fcntl(fileno(f), F_SETFD, 1);
#endif
	return f;
}
#undef	TYPE

/*
 *	Open a file, but temporary disable URL awareness.
 */

#ifdef HTEX
FILE* xfopen_local P2C(char *,filename, _Xconst char *,type)
{
	FILE *f;
	int url_aware_save;
	url_aware_save = URL_aware;
	URL_aware = FALSE;
	f = xfopen(filename, type);
	URL_aware = url_aware_save;
	return f;
}
#endif /* HTEX */

#if !defined(WIN32)

/*
 *	Create a pipe, closing a file if necessary.
 */

int
xpipe(fd)
	int	*fd;
{
	int	retval;

	for (;;) {
	    retval = pipe(fd);
	    if (retval == 0 || (errno != EMFILE && errno != ENFILE)) break;
	    n_files_left = 0;
	    close_a_file();
	}
	return retval;
}


/*
 *	Open a directory for reading, opening a file if necessary.
 */

DIR *
xdvi_xopendir(name)
	_Xconst char	*name;
{
	DIR	*retval;
	for (;;) {
	    retval = opendir(name);
	    if (retval == NULL || (errno != EMFILE && errno != ENFILE)) break;
	    n_files_left = 0;
	    close_a_file();
	}
	return retval;
}


/*
 *	Perform tilde expansion, updating the character pointer unless the
 *	user was not found.
 */

_Xconst	struct passwd *
ff_getpw(pp, p_end)
	_Xconst	char	**pp;
	_Xconst	char	*p_end;
{
	_Xconst	char		*p	= *pp;
	_Xconst	char		*p1;
	int			len;
	_Xconst	struct passwd	*pw;
	int			count;

	++p;	/* skip the tilde */
	p1 = p;
	while (p1 < p_end && *p1 != '/') ++p1;
	len = p1 - p;

	if (len != 0) {
	    if (len >= ffline_len)
		expandline(len);
	    bcopy(p, ffline, len);
	    ffline[len] = '\0';
	}

	for (count = 0;; ++count) {
	    if (len == 0)	/* if no user name */
		pw = getpwuid(getuid());
	    else
		pw = getpwnam(ffline);

	    if (pw != NULL) {
		*pp = p1;
		return pw;
	    }

	    /* On some systems, getpw{uid,nam} return without setting errno,
	     * even if the call failed because of too many open files.
	     * Therefore, we play it safe here.
	     */
	    if (count >= 2 && len != 0 && getpwuid(getuid()) != NULL)
		return NULL;

	    close_small_file();
	}
}

#endif	/* ! WIN32 */

/*
 *
 *      Read size bytes from the FILE fp, constructing them into a
 *      signed/unsigned integer.
 *
 */

unsigned long
num(fp, size)
	FILE *fp;
	int size;
{
	long x = 0;

	while (size--) x = (x << 8) | one(fp);
	return x;
}

long
snum(fp, size)
	FILE *fp;
	int size;
{
	long x;

#if	__STDC__
	x = (signed char) getc(fp);
#else
	x = (unsigned char) getc(fp);
	if (x & 0x80) x -= 0x100;
#endif
	while (--size) x = (x << 8) | one(fp);
	return x;
}

#if !HAVE_TEMPNAM	/* lacking in NeXT (and maybe others) */

char *
#if HAVE_PROTOTYPES
tempnam(dir, prefix)
	char	*dir;
	char	*prefix;
#else
tempnam(char *dir, char *prefix)
#endif
{
	char		*result;
	char		*ourdir	= getenv("TMPDIR");
	static int	seqno	= 0;

	if (ourdir == NULL || access(ourdir, W_OK) < 0)
	    ourdir = dir;
	if (ourdir == NULL || access(ourdir, W_OK) < 0)
	    ourdir = "/tmp";
	if (prefix == NULL)
	    prefix = "";
	result = xmalloc(strlen(ourdir) + 1 + strlen(prefix) + (2 + 5 + 1))
	Sprintf(result, "%s/%s%c%c%05d", ourdir, prefix,
	    (seqno%26) + 'A', (seqno/26)%26 + 'A', getpid());
	++seqno;
	return result;
}

#endif	/* not HAVE_TEMPNAM */

