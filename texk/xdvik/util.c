/*========================================================================*\

Copyright (c) 1990-2015  Paul Vojta and others

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
PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

NOTE:
xdvi is based on prior work, as noted in the modification history
in xdvi.c.

\*========================================================================*/

#include "xdvi-config.h"

#include <sys/types.h> /* ZLB: must be before sys/socket.h for IRIX 5.3 */
#include <sys/socket.h>
#include <sys/file.h>	/* this defines FASYNC */
#include <sys/ioctl.h>	/* this defines SIOCSPGRP and FIOASYNC */
#include <sys/wait.h>	/* this defines WIFEXITED and WEXITSTATUS */

#include "xdvi.h"
#include "hypertex.h"
#include "dvi-init.h"
#include "special.h"
#include "string-utils.h"

#include "kpathsea/tex-file.h"

#include "events.h"
#include "util.h"
#include "x_util.h"
#include "message-window.h"
#include "search-internal.h"
#include "encodings.h"
#include "filehist.h"
#include "xm_prefs.h" /* for preferences_changed() */

#include <errno.h>

#ifndef HAVE_MEMICMP
#include <ctype.h>
#endif

#ifdef VMS
#include <rmsdef.h>
#endif /* VMS */

#ifdef	X_NOT_STDC_ENV
extern int errno;
extern void *malloc();
extern void *realloc();
#endif


#ifdef	DOPRNT	/* define this if vfprintf gives you trouble */
#define	vfprintf(stream, message, args)	_doprnt(message, args, stream)
#endif

#ifdef VMS
#include <rmsdef.h>
#endif /* VMS */

#ifdef	X_NOT_STDC_ENV
extern int errno;
extern void *malloc();
extern void *realloc();
#endif

#if HAVE_XKB_BELL_EXT
# include <X11/XKBlib.h>
# define XBell(dpy, percent) XkbBell(dpy, mane.win, percent, (Atom) None)
#endif

/* if POSIX O_NONBLOCK is not available, use O_NDELAY */
#if !defined O_NONBLOCK && defined O_NDELAY
# define O_NONBLOCK O_NDELAY
#endif

/* Linux prefers O_ASYNC over FASYNC; SGI IRIX does the opposite.  */
#if !defined(FASYNC) && defined(O_ASYNC)
# define FASYNC O_ASYNC
#endif

#if !defined(FLAKY_SIGPOLL) && !HAVE_STREAMS && !defined(FASYNC)
# if !defined(SIOCSPGRP) || !defined(FIOASYNC)
#  define FLAKY_SIGPOLL	1
# endif
#endif

#if !FLAKY_SIGPOLL && HAVE_STREAMS
# include <stropts.h>

# ifndef S_RDNORM
#  define S_RDNORM S_INPUT
# endif

# ifndef S_RDBAND
#  define S_RDBAND 0
# endif

# ifndef S_HANGUP
#  define S_HANGUP 0
# endif

# ifndef S_WRNORM
#  define S_WRNORM S_OUTPUT
# endif

#endif /* not FLAKY_SIGPOLL && HAVE_STREAMS */

#include <sys/param.h>
#include <sys/stat.h>
#include <pwd.h>
#include <errno.h>

#ifndef MAXSYMLINKS		/* Workaround for Linux libc 4.x/5.x */
#define MAXSYMLINKS 5
#endif

#if HAVE_ICONV_H
#include <iconv.h>
#endif

#include <locale.h>

#if USE_LANGINFO
#include <langinfo.h>
#endif

#define BUF_SIZE 1024

void
xdvi_assert(const char *version,
	    const char *filename,
	    int lineno,
	    Boolean condition,
	    const char *fmt, ...)
{
    if (!(condition)) {
	va_list argp;
	fprintf(stderr,
		"\n************************************************************\n"
		"XDvi %s: Failed assertion:\n%s:%d: ",
		version, filename, lineno);
	va_start(argp, fmt);
	(void)vfprintf(stderr, fmt, argp);
	va_end(argp);
#ifdef NDEBUG
	fprintf(stderr,
		"\nAborting now. Please report this as a bug to:\n"
		"http://sourceforge.net/tracker/?group_id=23164&atid=377580\n"
		"If a core dump has been produced, please invoke:\n"
		"gdb %s core\nThen type \"bt\", "
		"and include the resulting output in your bug report.\n"
 		"************************************************************\n",
		globals.program_name);
	do_abort();
#else
	fprintf(stderr,
		"\nPlease report this as a bug to:\n"
		"http://sourceforge.net/tracker/?group_id=23164&atid=377580\n"
 		"************************************************************\n");
#endif
    }
}


void
xdvi_bell(void)
{
    if (!resource.hush_bell) {
	XBell(DISP, 0);
    }
}



/* NOTE: keep this table in sync with the #defines in xdvi-debug.h! */
struct debug_string_options debug_options[] = {
    {  DBG_BITMAP,	"bitmap",	", " },
    {  DBG_DVI,		"dvi",		", " },
    {  DBG_PK,		"pk",		", " },
    {  DBG_BATCH,	"batch",	", " },
    {  DBG_EVENT,	"events",	", " },
    {  DBG_PS,		"ps",		",\n"},
    {  DBG_STAT,	"stat",		", " },
    {  DBG_HASH,	"hash",		", " },
    {  DBG_OPEN,	"open",		", " },
    {  DBG_PATHS,	"paths",	", " },
    {  DBG_EXPAND,	"expand",	", " },
    {  DBG_SEARCH,	"search",	", " },
    {  DBG_KPATHSEA,	"kpathsea",	",\n"},
    {  DBG_HTEX,	"htex",		", " },
    {  DBG_SRC_SPECIALS,"src",		", " },
    {  DBG_CLIENT,	"client",	", " },
    {  DBG_FT,		"ft",		", " },
    {  DBG_FT_VERBOSE,	"ft_verbose",	",\n"},
    {  DBG_GUI,		"gui",		", " },
    {  DBG_FIND,	"find",		", " },
    {  DBG_FILES,	"files",	", " },
    {  DBG_ALL,		"all",		"\n" },
    /* end marker */
    {  0,		NULL,		NULL }
};

/*
 *	General utility routines.
 */

/*
 * 2-step fopen using close_a_file() if first opening attempt fails with
 * `too many open files'.
 */
FILE *
try_fopen(const char *fname, const char *mode)
{
    FILE *fp = fopen(fname, mode);
    if (fp == NULL && (errno == EMFILE || errno == ENFILE)) {
	close_a_file();
	fp = fopen(fname, mode);
    }
    return fp;
}

/*
 * Like try_fopen(), for fdopen().
 */
FILE *
try_fdopen(int fd, const char *mode)
{
    FILE *fp = fdopen(fd, mode);
    if (fp == NULL && (errno == EMFILE || errno == ENFILE)) {
	close_a_file();
	fp = fdopen(fd, mode);
    }
    return fp;
}


/*
 * Like try_fopen(), for open().
 */
int
try_open(const char *fname, int flags)
{
    int fd = open(fname, flags);
    if (fd < 0 && (errno == EMFILE || errno == ENFILE)) {
	close_a_file();
	fd = open(fname, flags);
    }
    return fd;
}

/*
 * Like try_fopen(), for open() with 3 arguments.
 */
int
try_open_mode(const char *fname, int flags, mode_t mode)
{
    int fd = open(fname, flags, mode);
    if (fd < 0 && (errno == EMFILE || errno == ENFILE)) {
	close_a_file();
	fd = open(fname, flags, mode);
    }
    return fd;
}


/*
 *	invoked on SIGSEGV: try to stop gs before aborting, to prevent gs
 *	running on with 100% CPU consumption - this can be annoying during
 *	testing. We'd also like to clean up the "xdvi windows" property in
 *	the root window, but it might be already too late to talk to the
 *	X server here (the code can also be triggered by X errors).
 */
void
do_abort(void)
{
#if PS
    ps_destroy();
#endif
    /*     if (globals.widgets.top_level) */
    /* 	exit_clean_windows(); */
    abort();
}

/*
  Expand leading ~ or ~user in path to the actual homedirectory of the user.
  Returns either NULL if unsuccessful, or result in freshly allocated string.
  Bugs: ~user doesn't work with NIS/yellow pages.
*/
char *
expand_homedir(const char *path)
{
    char *resolved = NULL;
    const char *orig_path = path;
    UNUSED(orig_path); /* if HAVE_GETPWNAM or HAVE_GETPWUID && HAVE_GETUID */

    if (path == NULL)
	return NULL;
    
    /* if path doesn't start with ~, just return a copy of path */
    if (*path != '~')
	return xstrdup(path);

    /* expand ~/ or ~user */
    path++;
    if (*path == '/') { /* expand `~/' to `$HOME/' */
	char *homedir = getenv("HOME");
	if (homedir != NULL) {
	    resolved = xstrdup(homedir);
	    resolved = xstrcat(resolved, path);
	}
	else {
#if defined(HAVE_GETPWUID) && defined(HAVE_GETUID)
	    /* homedir not set */
	    struct passwd *entry = getpwuid(getuid());
	    if (entry != NULL) {
		homedir = entry->pw_dir;
		if (homedir != NULL) {
		    resolved = xstrdup(homedir);
		    resolved = xstrcat(resolved, path);
		}
		else {
		    XDVI_ERROR((stderr, "getpwnam returned NULL for entry->pw_dir: %s", strerror(errno)));
		    return NULL;
		}
	    }
	    else {
		XDVI_ERROR((stderr, "getpwnam failed: %s", strerror(errno)));
		return NULL;
	    }
#else
	    popup_message(globals.widgets.top_level,
			  MSG_WARN,
			  NULL,
			  "$HOME not set, and getpwuid() or getuid() not supported - could not expand \"%s\".",
			  orig_path);
	    return NULL;
#endif
	}
	TRACE_GUI((stderr, "resolved: |%s|", resolved));
	return resolved;
    }
    else { /* ~user -> try getpwnam() to get homedir */
#ifdef HAVE_GETPWNAM
	struct passwd *entry;
	char *separator = strchr(path, '/');
	char *homedir;

	TRACE_GUI((stderr, "separator is: |%s|, len of username: %d",
		   separator, (int)(separator - path)));
	if (separator == NULL)
	    return NULL;

	resolved = xmalloc(separator - path + 1);
	memcpy(resolved, path, separator - path);
	resolved[separator - path] = '\0';

	TRACE_GUI((stderr, "username is: |%s|", resolved));
	entry = getpwnam(resolved);
	if (entry == NULL) {
	    XDVI_ERROR((stderr, "getpwnam failed: %s", strerror(errno)));
	    return NULL;
	}
	TRACE_GUI((stderr, "homedir of user is: |%s|", entry->pw_dir));
	homedir = entry->pw_dir;
	    
	free(resolved);
	resolved = xstrdup(homedir);
	resolved = xstrcat(resolved, path + (separator - path));
	TRACE_GUI((stderr, "resolved: |%s|", resolved));
	return resolved;
#else
	popup_message(globals.widgets.top_level,
		      MSG_WARN,
		      NULL,
		      "Expanding \"%s\" failed: This platform doesn't support getpwnam().",
		      orig_path);
	return NULL;
#endif
    }

}

/*
  realpath implementation if no implementation available.
  Try to canonicalize path, removing `~', `.' and `..' and expanding symlinks.
  
  Adopted from wu-ftpd's fb_realpath (in realpath.c), but without the
  seteuid(0) stuff (which we don't need, since we never change into
  directories or read files the user has no permissions for), and
  without the ugly goto(). Special care has been taken to avoid buffer
  overflows (e.g. this version is not affected by
  http://isec.pl/vulnerabilities/isec-0011-wu-ftpd.txt).
  
  `resolved' should be a buffer of size MAXPATHLEN.  */

char *
my_realpath(const char *path, char *resolved)
{
    struct stat sb;
    int n;
    /*     char *p, *q, *tmp; */
    char *base;
    char tmpbuf[MAXPATHLEN];
    int symlinks = 0;
#ifdef HAVE_FCHDIR
    int fd;
#else
    char cwd[MAXPATHLEN];
#endif

    /* Save cwd for going back later */
#ifdef HAVE_FCHDIR
    if ((fd = try_open(".", O_RDONLY)) < 0)
	return NULL;
#else /* HAVE_FCHDIR */
    if (
# ifdef HAVE_GETCWD
	getcwd(cwd, MAXPATHLEN)
# else
	getwd(cwd)
# endif
	== NULL)
	return NULL;
#endif /* HAVE_FCHDIR */

    if (strlen(path) + 1 > MAXPATHLEN) {
	errno = ENAMETOOLONG;
	return NULL;
    }
    
    strcpy(resolved, path);

    for (;;) { /* loop for resolving symlinks in base name */
	/* get base name and dir name components */
	char *p = strrchr(resolved, '/');
	if (p != NULL) {
	    base = p + 1;
	    if (p == resolved) {
		/* resolved is in root dir; this must be treated as a special case,
		   since we can't chop off at `/'; instead, just use the `/':
		*/
		p = "/";
	    }
	    else {
		/* not in root dir; chop off path name at slash */
		while (p > resolved && *p == '/') /* for multiple trailing slashes */
		    p--;
		*(p + 1) = '\0';
		p = resolved;
	    }

	    /* change into that dir */
	    if (chdir(p) != 0)
		break;
	}
	else /* no directory component */
	    base = resolved;

	/* resolve symlinks or directory names (not used in our case) in basename */
	if (*base != '\0') {
	    if (
#ifdef HAVE_LSTAT
		lstat(base, &sb)
#else
		stat(base, &sb)
#endif
		== 0) {
#ifdef HAVE_LSTAT
		if (S_ISLNK(sb.st_mode)) { /* if it's a symlink, iterate for what it links to */
		    if (++symlinks > MAXSYMLINKS) {
			errno = ELOOP;
			break;
		    }

		    if ((n = readlink(base, resolved, MAXPATHLEN)) < 0)
			break;

		    resolved[n] = '\0';
		    continue;
		}
#endif /* HAVE_LSTAT */
		if (S_ISDIR(sb.st_mode)) { /* if it's a directory, go there */
		    if (chdir(base) != 0)
			break;

		    base = "";
		}
	    }
	}

	/* Now get full pathname of current directory and concatenate it with saved copy of base name */
	strcpy(tmpbuf, base); /* cannot overrun, since strlen(base) <= strlen(path) < MAXPATHLEN */
	if (
#ifdef HAVE_GETCWD
	    getcwd(resolved, MAXPATHLEN)
#else
	    getwd(resolved)
#endif
	    == NULL)
	    break;

	/* need to append a slash if resolved is not the root dir */
	if (!(resolved[0] == '/' && resolved[1] == '\0')) {
	    if (strlen(resolved) + 2 > MAXPATHLEN) {
		errno = ENAMETOOLONG;
		break;
	    }
	    strcat(resolved, "/");
	}

	if (*tmpbuf) {
	    if (strlen(resolved) + strlen(tmpbuf) + 1 > MAXPATHLEN) {
		errno = ENAMETOOLONG;
		break;
	    }
	    strcat(resolved, tmpbuf);
	}

	/* go back to where we came from */
#ifdef HAVE_FCHDIR
	(void)fchdir(fd);
	close(fd);
#else
	(void)chdir(cwd);
#endif
	return resolved;
    }

    /* arrive here in case of error: go back to where we came from, and return NULL */
#ifdef HAVE_FCHDIR
    (void)fchdir(fd);
    close(fd);
#else
    (void)chdir(cwd);
#endif
    return NULL;
}


#ifndef KPATHSEA

/*
 *	Either (re)allocate storage or fail with explanation.
 */

void *
xmalloc(unsigned size)
{
    void *mem = malloc(size);

    if (mem == NULL)
	XDVI_FATAL((stderr, "Out of memory (allocating %u bytes).", size));
    return mem;
}

void *
xrealloc(void *where, unsigned size)
{
    void *mem = realloc(where, size);

    if (mem == NULL)
	XDVI_FATAL((stderr, "Out of memory (reallocating %u bytes).", size));
    return mem;
}


/*
 *	Allocate a new string.
 */

char *
xstrdup(const char *str)
{
    size_t len;
    char *new;

    ASSERT(fprintf(stderr, "Test assertion!\n") && 1 == 0);
    ASSERT(str != NULL, "");
    len = strlen(str) + 1;
    new = xmalloc(len);
    memcpy(new, str, len);
    return new;
}

#endif /* not KPATHSEA */


/*
 *	Allocate a new string.  The second argument is the length.
 */

char *
xmemdup(const char *str, size_t len)
{
    char *new;

    new = xmalloc(len);
    memcpy(new, str, len);
    return new;
}

/* like xstrdup(), but with XtMalloc() */
char *
xt_strdup(const char *ptr)
{
    char *ret = XtMalloc(strlen(ptr) + 1);
    return strcpy(ret, ptr);
}


/*
 * Append str2 to str1, reallocating str1 as neccessary.
 * Note that this modifies str1, so if you want a clean
 * new copy, use xstrdup() first, then xstrcat().
 * str1 should be either NULL, or a valid char * that has
 * previously been malloc()ed.
 */

char *
xstrcat(char *str1, const char *str2)
{
    size_t len1 = (str1 == NULL) ? 0 : strlen(str1);
    size_t len2 = strlen(str2) + 1;

    str1 = xrealloc(str1, len1 + len2);
    memcpy(str1 + len1, str2, len2);
    return str1;
}


/*
 *	Allocate bitmap for given font and character
 */

void
alloc_bitmap(struct bitmap *bitmap)
{
    unsigned int size;

    /* fprintf(stderr, "allocating bitmap of size %u x %u\n", bitmap->w, bitmap->h); */
    /* width must be multiple of <arch-defined> bits for raster_op */
    bitmap->bytes_wide = ROUNDUP((int)bitmap->w, BMBITS) * BMBYTES;
    size = bitmap->bytes_wide * bitmap->h;
    bitmap->bits = xmalloc(size != 0 ? size : 1);
}


#ifndef HAVE_MEMICMP
/*
 * Case-insensitive version of memcmp().  This code assumes that the second
 * argument (i.e. what is being compared with) is lower case.
 */

int
memicmp(const char *s1, const char *s2, size_t n)
{
    while (n > 0) {
	int i = tolower((int)*s1) - *s2;
	if (i != 0)
	    return i;
	++s1;
	++s2;
	--n;
    }
    return 0;
}
#endif /* HAVE_MEMICMP */

/*
 * Try to close the pixel file for the least recently used font.
 * Invoked when we've run out of file descriptors.
 */
void
close_a_file(void)
{
    struct font *fontp;
    unsigned short oldest = USHRT_MAX;
    struct font *f = NULL;

    if (globals.debug & DBG_OPEN)
	puts("Calling close_a_file().");

    for (fontp = font_head; fontp != NULL; fontp = fontp->next) {
	if (fontp->file != NULL && fontp->timestamp <= oldest) {
	    f = fontp;
	    oldest = fontp->timestamp;
	}
    }
    /* fprintf(stderr, "oldest = %u\n", oldest); */
    if (f == NULL)
	XDVI_FATAL((stderr, "Can't find an open pixel file to close"));
    fclose(f->file);
    f->file = NULL;
}

/*
 * Open a file in the given mode. We use XFOPEN since xfopen is already
 * usurpated by kpathsea's xfopen.c, which just exits rather ungracefully
 * if it detects a NULL return value; most certainly NOT what we want here.
 */
FILE *
XFOPEN(const char *path, const char *mode)
{
    FILE *fp = NULL;
#ifdef TESTING_OPEN_FILES
    fprintf(stderr, "trying to open |%s|\n", path);
#endif
    if ((fp = try_fopen(path, mode)) == NULL && (errno == EMFILE || errno == ENFILE)) {
	XDVI_FATAL((stderr, "too many open files"));
    }
    return fp;
}

/*
 *	Create a pipe, closing a file if necessary.
 *	We use socketpair() instead of pipe() because several operating
 *	systems don't support SIGPOLL/SIGIO on pipes:
 *		SGI IRIX 6.5	F_SETOWN not implemented
 *		Linux 2.4.2	Not supported
 */

int
xpipe(int *fd)
{
    int	retval;
    
    for (;;) {
	retval = socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
	if (retval == 0) { /* success */
	    break;
	}
	if ((errno != EMFILE && errno != ENFILE)) {
	    /* failed, but not because of too many files */
	    break;
	}
	close_a_file();
    }
    return retval;
}



/*
 *
 *      Read size bytes from the FILE fp, constructing them into a
 *      signed/unsigned integer.
 *
 */

unsigned long
get_bytes(FILE *fp, int size)
{
    long x = 0;

    while (size--)
	x = (x << 8) | get_byte(fp);
    return x;
}

long
get_lbytes(FILE *fp, int size)
{
    long x;

#if	__STDC__
    x = (signed char)getc(fp);
#else
    x = (unsigned char)getc(fp);
    if (x & 0x80)
	x -= 0x100;
#endif
    while (--size)
	x = (x << 8) | get_byte(fp);
    return x;
}



/*
 *	Create a temporary file and return its fd.  Also put its filename
 *	in str.  Create str if it's NULL.
 */

#ifndef P_tmpdir
#define	P_tmpdir	"/tmp"
#endif

static const char tmp_suffix[] = "/xdvi-XXXXXX";

int
xdvi_temp_fd(char **str)
{
    int fd;
    char *p;
    size_t len;
    static const char *template = NULL;
#if !HAVE_MKSTEMP
    static unsigned long seed;
    static char letters[] =
	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._";
    char *p1;
#endif

    if (*str != NULL) {
	p = *str;

	/* O_EXCL is there for security (if root runs xdvi) */
	if (!((fd = try_open_mode(p, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1
	      && errno == EEXIST))
	    return fd;
#if HAVE_MKSTEMP
	memcpy(p + strlen(p) - 6, "XXXXXX", 6);
#endif
    }
    else {
	if (template == NULL) {
	    const char *ourdir;

	    ourdir = getenv("TMPDIR");
	    if (ourdir == NULL || access(ourdir, W_OK) < 0) {
		ourdir = P_tmpdir;
		if (access(ourdir, W_OK) < 0)
		    ourdir = ".";
	    }
	    len = strlen(ourdir);
	    if (len > 0 && ourdir[len - 1] == '/')
		--len;
	    template = p = xmalloc(len + sizeof tmp_suffix);
	    memcpy(p, ourdir, len);
	    memcpy(p + len, tmp_suffix, sizeof tmp_suffix);
#if !HAVE_MKSTEMP
	    seed = 123456789 * time(NULL) + 987654321 * getpid();
#endif
	}
	*str = p = xstrdup(template);
    }

#if HAVE_MKSTEMP
    fd = mkstemp(p);
    if (fd == -1 && (errno == EMFILE || errno == ENFILE)) {
	close_a_file();
	memcpy(p + strlen(p) - 6, "XXXXXX", 6);
	fd = mkstemp(p);
    }
#else
    p1 = p + strlen(p) - 6;
    for (;;) {
	unsigned long s = ++seed;
	char *p2;

	for (p2 = p1 + 5; p2 >= p1; --p2) {
	    *p2 = letters[s & 63];
	    s >>= 6;
	}
	if (!((fd = try_open_mode(p, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1
	      && errno == EEXIST))
	    break;
    }
#endif
    return fd;
}


/* print a GUI error message for childs that exited with an errorcode */
void
handle_child_exit(int status, struct xchild *this)
{
    char *err_msg = NULL;
    
    /* if child exited with error and xio struct is available for child,
       print error text */
    if (this->io != NULL
	&& (WIFEXITED(status) != 0)
	&& (WEXITSTATUS(status) != 0)
	&& (err_msg = (this->io->read_proc)(this->io->fd, NULL)) != NULL) {

	if (this->name == NULL) {
	    popup_message(globals.widgets.top_level,
			  MSG_WARN,
			  NULL,
			  err_msg[0] == '\0' ? "An unknown error occurred" : err_msg);
	}
	else {
	    popup_message(globals.widgets.top_level,
			  MSG_WARN,
			  "Xdvi tries to collect all messages from STDERR. "
			  "When no useful error message is available "
			  "(e.g. because the program has written to STDOUT instead), "
			  "try to run the command again from the command line "
			  "to find out what the problem was.",
			  "Command \"%s\" exited with error code %d\n%s\n",
			  this->name, WEXITSTATUS(status), err_msg);
	}
	free(err_msg);
    }
    free(this->name);
    free(this->io);
    free(this);
}

static void
dummy_write_proc(int fd, void *data)
{
    UNUSED(fd);
    UNUSED(data);
    
    fprintf(stderr, "============== dummy_write_proc called for fd %d\n", fd);
}

/*
 * read what child printed to fd (should be set up to be stderr).
 * Allocates and returns error text; caller is responsible for free()ing it
 * afterwards.
 */
char *
read_child_error(int fd, void *data)
{
    int bytes = 0, buf_old_size = 0, buf_new_size = 0;
    char tmp_buf[BUF_SIZE];
    char *err_buf = xstrdup("");
    char *ret;
    UNUSED(data);
    
    /* collect stderr messages into err_buf */
    while ((bytes = read(fd, tmp_buf, BUF_SIZE - 1)) > 0) {
	buf_old_size = buf_new_size;
	buf_new_size += bytes;
	err_buf = xrealloc(err_buf, buf_new_size + 1);
	memcpy(err_buf + buf_old_size, tmp_buf, buf_new_size - buf_old_size);
	err_buf[buf_new_size] = '\0';
    }

    close(fd);
    ret = escape_format_arg(err_buf); /* this allocates ret */
    free(err_buf);
    return ret;
}

/*
 * Fork a child process. If exit_proc is NULL, the process' error messages
 * will be collected and a window popped up for GUI display. If exit_proc
 * is non-NULL, it should be a function that does something reasonable
 * with the error messages itself. It should also free the xchild struct.
 *
 * If dirname != NULL, the child will chdir into dirname before running the
 * command.
 *
 * If killsig == 0, the process will not be killed when xdvi terminates.
 * If killsig > 0, the process will be sent this signal when xdvi terminates.
 * If killsig < 0, the process will be given its own group, and the whole group
 * will be sent -killsig when xdvi terminates.  This is useful when invoking
 * /bin/sh.  Generally killsig (if nonzero) should be SIGKILL or -SIGKILL.
 */
Boolean
fork_process(const char *proc_name, Boolean redirect_stdout,
	     const char *dirname,
	     childProcT exit_proc, void *data,
	     int killsig,
	     char *const argv[])
{
    int i, pid;
    struct xchild *my_child = xmalloc(sizeof *my_child);
    struct xio *my_io = xmalloc(sizeof *my_io);
    int err_pipe[2];
    char *volatile buf = xstrdup("");

    for (i = 0; argv[i] != NULL; i++) {
	TRACE_GUI((stderr, "argv: |%s|", argv[i]));
	buf = xstrcat(buf, argv[i]);
	buf = xstrcat(buf, " ");
    }
    
    if (i > 0)
	buf[strlen(buf) - 1] = '\0'; /* chop off trailing space */

    TRACE_GUI((stderr, "forking: |%s|", buf));
    
    /* flush output buffers to avoid double buffering (i.e. data
       waiting in the output buffer being written twice, by the parent
       and the child) */
    fflush(stdout);
    fflush(stderr);

    if (pipe(err_pipe) < 0) {
	XDVI_FATAL((stderr, "pipe error"));
    }

    switch (pid = vfork()) {
    case -1:	/* forking error */
	perror("vfork");
	close(err_pipe[0]);
	close(err_pipe[1]);
	return False;
    case 0:	/* child */
	if (dirname != NULL)
	    (void)chdir(dirname);
	if (globals.debug & DBG_FILES) {
	    char path[MAXPATHLEN];
	    (void)getcwd(path, MAXPATHLEN);
	    fprintf(stderr, "Directory of running `%s': `%s'\n",
		    proc_name, path);
	}
	/* FIXME: There's a bug which prevents this from working
	   with xdvi as child: Whenever xdvi tries to print to stderr,
	   this will hang the child forever. Closing all other file
	   descriptors, as in the #if TRY_FIX regions, seems to fix
	   this, but it also loses all output ...
	*/
#if TRY_FIX
	close(0);
	close(1);
#endif /* TRY_FIX */
	close(err_pipe[0]);	/* no reading from stderr */

	/* (Maybe) set session ID, so that kill will also affect subprocesses */
	if (killsig < 0) {
	    if (setsid() == -1) {
		perror("setsid");
		_exit(EXIT_FAILURE);
		return False;	/* make compiler happy */
	    }
	}

	/* redirect writing to stderr */
	if (dup2(err_pipe[1], STDERR_FILENO) != STDERR_FILENO) {
	    perror("dup2 for stderr");
	    _exit(EXIT_FAILURE);
	    return False;	/* make compiler happy */
	}
	if (redirect_stdout) {
	    /* also redirect writing to stdout */
	    if (dup2(err_pipe[1], STDOUT_FILENO) != STDOUT_FILENO) {
		perror("dup2 for stdout");
		_exit(EXIT_FAILURE);
		return False;	/* make compiler happy */
	    }
	}

#if TRY_FIX
	/* close all remaining descriptors */
	i = 2;
	while (i < 256 /* TODO: better handling of OPEN_MAX; see Stevens p. 43 */) {
	    close(i++);
	}
#endif /* TRY_FIX */
	execvp(proc_name, argv);

	/* arrive here only if execvp failed */
	fprintf(stderr, "%s: Execution of %s failed.\n", globals.program_name, proc_name);
	fflush(stderr);
	close(err_pipe[1]);
	_exit(EXIT_FAILURE);
	return False;		/* make compiler happy */
    default:	/* parent */
	close(err_pipe[1]);	/* no writing to stderr */

	my_io->next = NULL;
	my_io->fd = err_pipe[0];
	my_io->xio_events = XIO_IN;
#if HAVE_POLL
	my_io->pfd = NULL;
#endif
	my_io->read_proc = read_child_error;
	my_io->write_proc = dummy_write_proc;
	my_io->data = data;
	
	my_child->next = NULL;
	my_child->pid = pid;
	my_child->name = buf;
	my_child->data = data;
	my_child->killsig = killsig;
	if (exit_proc == NULL) { /* use default exit procedure */
	    my_child->proc = handle_child_exit;
	}
	else {
	    my_child->proc = exit_proc;
	}
	my_child->io = my_io;
	
	set_chld(my_child);
	
	return True;
    }
}



/*
 *	Prepare the file descriptor to generate SIGPOLL/SIGIO events.
 *	If called with a True argument, set it up for non-blocking I/O.
 */

void
prep_fd(int fd, wide_bool noblock)
{
    /* Set file descriptor for non-blocking I/O */
    if (noblock)
	(void) fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
    
#if !FLAKY_SIGPOLL
# if HAVE_STREAMS
    if (isastream(fd) > 0) {
	if (ioctl(fd, I_SETSIG,
		  S_RDNORM | S_RDBAND | S_HANGUP | S_WRNORM) == -1)
	    perror("xdvi: ioctl I_SETSIG");
    }
    else
# endif
    {
# ifdef FASYNC
	if (fcntl(fd, F_SETOWN, getpid()) == -1)
	    perror("xdvi: fcntl F_SETOWN");
	if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | FASYNC) == -1)
	    perror("xdvi: fcntl F_SETFL");
# elif defined SIOCSPGRP && defined FIOASYNC
	/* For HP-UX B.10.10 and maybe others.  See "man 7 socket".  */
	int arg;
	    
	arg = getpid();
	if (ioctl(fd, SIOCSPGRP, &arg) == -1)
	    perror("xdvi: ioctl SIOCSPGRP");
	arg = 1;
	if (ioctl(fd, FIOASYNC, &arg) == -1)
	    perror("xdvi: ioctl FIOASYNC");
# endif
    }
#endif /* not FLAKY_SIGPOLL */
}


/* APS Pointer locator: */
/* Return screen positions */
Boolean
pointerlocate(int *xpos, int *ypos)
{
    Window root, child;
    int root_x, root_y;
    unsigned int keys_buttons;

    if (!XtIsRealized(globals.widgets.top_level))
	return False;
    
    return XQueryPointer(DISP, mane.win, &root, &child,
			 &root_x, &root_y, xpos, ypos, &keys_buttons);
}

unsigned long
parse_debugging_string(const char *arg)
{
    int retval = 0;
    const char *curr, *last;
    size_t i;
    
    curr = last = arg;

    while (*curr != '\0') {
	Boolean matched = False;

	while (isspace((int)*curr))
	    curr++;
	for (i = 0; debug_options[i].description != NULL; i++) {
	    size_t curr_opt_len = strlen(debug_options[i].description);
	    /* Should we match on length of passed argument, to allow for abbreviations? */
	    if (memicmp(curr,
			debug_options[i].description,
			curr_opt_len) == 0
		&& (curr[curr_opt_len] == '\0'
		    || curr[curr_opt_len] == ','
		    || isspace((int)curr[curr_opt_len]))) {
		matched = True;
		retval |= debug_options[i].bitmap;
		fprintf(stderr, "Debugging option: \"%s\" = \"%s\", debug: %d\n",
			curr, debug_options[i].description, retval);
	    }
	}
	if (!matched) {
	    char *tempstr = xstrdup(curr);
	    char *test;
	    if ((test = strchr(curr, ',')) != NULL) {
		*test = '\0';
	    }
	    XDVI_WARNING((stderr, "Ignoring unknown debugging option \"%s\". Valid options are:\n", tempstr));
	    for (i = 0; debug_options[i].description != NULL; i++) {
		fprintf(stderr, "`%s'%s",
			debug_options[i].description,
			debug_options[i].help_formatting);
	    }
	    fprintf(stderr, "\n");
	    free(tempstr);
	}
	curr = strchr(curr, ',');
	if (curr != NULL)
	    curr++;
    }

    return retval;
}

unsigned long
parse_debugging_option(const char *ptr)
{
    if (ptr == NULL)
	return 0L;
    else if (isdigit((int)*ptr)) {
	if (resource.debug_arg == NULL)
	    return DBG_ALL; /* per default debug everything */
	else
	    return strtol(resource.debug_arg, (char **)NULL, 10);
    } else if (*ptr == '-')
	return DBG_ALL;
    else return parse_debugging_string(ptr);
}

/* determine average width of a font */
int
get_avg_font_width(XFontStruct *font)
{
    int width;

    assert(font != NULL);
    width = font->max_bounds.width + font->min_bounds.width / 2;
    if (width == 0) {
	/* if min_bounds.width = -max_bounds.width, we probably
	   have a scalable TT font; try to determine its actual
	   width by measuring the letter `x':
	*/
	width = XTextWidth(font, "x", 1);
    }
    if (width == 0) { /* last resort */
	width = font->max_bounds.width / 2;
    }
    return width;
    
}

/*
  Splits LINE from BEGIN to END (not neccessarily null-terminated)
  into items separated by SEP, removes leading or trailing whitespace,
  and saves the items as newly allocated char*s into the return array.
  Returns the number of items that have been saved as RET_ITEMS.
  Empty entries are returned as such, i.e. both `abc:' and `abc: '
  return RET_ITEMS = 2 and "" as second entry.
*/
char **
split_line(const char *line, char sep, size_t begin, size_t end, size_t *ret_items)
{
    const char *c_ptr = line + begin;
    const char *e_ptr = line + end;
    const char *test_end;
    
    size_t result_cnt = 0;
    size_t alloc_len = 0;
    size_t len;
    const size_t ALLOC_STEP = 8;
    char **result_arr = NULL;
    
    /* create new result item, resizing result_arr as needed
     * (an empty string will coun1 as 1 item: "") */
    while (result_cnt + 1 >= alloc_len) {
	alloc_len += ALLOC_STEP;
	result_arr = xrealloc(result_arr, alloc_len * sizeof *result_arr);
    }

    while (c_ptr <= e_ptr) {
	/* skip leading whitespace */
	while (c_ptr < e_ptr && isspace((int)*c_ptr)) {
	    c_ptr++;
	}

	/* find end of current elem, which is either the separator or out of range */
	test_end = strchr(c_ptr, sep);
	/* skip escaped separators */
	while (test_end != NULL && test_end <= e_ptr) {
	    if (test_end > c_ptr && *(test_end - 1) == '\\') {
		test_end = strchr(test_end + 1, sep);
	    }
	    else
		break;
	}
	/* if nothing found, use e_ptr */
	if (test_end == NULL || test_end > e_ptr) {
	    test_end = e_ptr;
	}

	len = test_end - c_ptr;

	/* skip trailing whitespace */
	while (len > 0 && isspace((int)c_ptr[len - 1])) {
	    len--;
	}

	result_arr[result_cnt] = xmalloc(len + 1);
	/* copy into result item, skipping the escape '\\' characters */
	{
	    size_t i = 0, j = 0;
	    while (i < len) {
		if (c_ptr[i] == '\\' && c_ptr[i + 1] == sep) /* i + 1 is safe since (i < len) */
		    i++;
		result_arr[result_cnt][j++] = c_ptr[i++];
	    }
	    result_arr[result_cnt][j] = '\0';
	}
	result_cnt++;
	
	/* skip to next item */
	c_ptr = test_end + 1;
    }
    result_arr[result_cnt] = NULL; /* null-terminate return array, just to be safe */
    *ret_items = result_cnt;
    return result_arr;
}


/*------------------------------------------------------------
 *  find_file
 *
 *  Arguments:
 *   filename - absolute or relative file name
 *   statbuf  - buffer to stat filename
 *   pathinfo - kpse_file_format_type, only used if a kpathsearch for the
 *	        file is performed.
 *		See  kpathsea/tex-file.h for a list of possible values.
 *
 *  Returns:
 *	 expanded filename
 *
 *  Purpose:
 *	Find a file name corresponding to <filename>, possibly
 *	expanding it to a full path name; checks if the file
 *	exists by stat()ing it; returns NULL if nothing has
 *	been found, else the expanded filename in fresh memory.
 *

 *------------------------------------------------------------*/

char *
find_file(const char *filename, struct stat *statbuf, kpse_file_format_type pathinfo)
{
    char *tmp;
    char *pathname;

    TRACE_SRC((stderr, "checking filename \"%s\"", filename));

    /*
     * case 1:
     * try absolute filename
     */
    if (filename[0] == '/') {
	if (stat(filename, statbuf) == 0) {
	    TRACE_SRC((stderr, "Found absolute filename \"%s\"", filename));
	    return xstrdup(filename);
	}
	else {
	    TRACE_SRC((stderr, "Can't stat absolute filename \"%s\"\n", filename));
	    return NULL;
	}
    }

    /*
     * case 2:
     * prepend filename with dir component from the `main' xdvi file (globals.dvi_file.dirname).
     * This works for both
     * /absolute/path/ + filename
     * and
     * /absolute/path/ + relative/path/filename
     */
    ASSERT(globals.dvi_file.dirname != NULL, "globals.dvi_file.dirname should have been initialized");

    pathname = xstrdup(globals.dvi_file.dirname);
    pathname = xstrcat(pathname, filename);

    TRACE_SRC((stderr, "Trying globals.dvi_file.dirname: \"%s\"", pathname));
    if (stat(pathname, statbuf) == 0) {
	return pathname;
    }

    /*
     * case 3:
     * try current directory; if it's a match, expand to full (but uncanonicalized) path name.
     */
    if (stat(filename, statbuf) == 0) {
	TRACE_SRC((stderr, "Found file \"%s\" in current dir", filename));
	free(pathname);
	return expand_filename(filename, USE_CWD_PATH);
    }

    /*
     * case 4a:
     * try a kpathsea search for filename, from globals.dvi_file.dirname
     */
    {
#ifdef HAVE_FCHDIR
	int fd;
#else
	char cwd[MAXPATHLEN];
#endif

	/* Save cwd for going back later */
	if (
#ifdef HAVE_FCHDIR
	    (fd = try_open(".", O_RDONLY)) >= 0
#else /* HAVE_FCHDIR */
# ifdef HAVE_GETCWD
	    getcwd(cwd, MAXPATHLEN)
# else
	    getwd(cwd)
# endif
	    != NULL
#endif /* HAVE_FCHDIR */
	    ) {
	    if (chdir(globals.dvi_file.dirname) == 0) {
	    
		TRACE_SRC((stderr,
			   "trying kpathsearch for filename \"%s\" from %s",
			   filename, globals.dvi_file.dirname));
		tmp = kpse_find_file(filename, pathinfo, True);

		if (tmp != NULL && stat(tmp, statbuf) == 0) {
		    free(pathname);
		    /* go back to where we came from */
#ifdef HAVE_FCHDIR
		    (void)fchdir(fd);
		    close(fd);
#else
		    (void)chdir(cwd);
#endif
		    if (tmp[0] == '/') { /* is it an absolute path? */
			pathname = xstrdup(tmp);
		    }
		    else {
			pathname = xstrdup(globals.dvi_file.dirname);
			pathname = xstrcat(pathname, tmp);
		    }
		    TRACE_SRC((stderr, "Found file: `%s'", pathname));
		    free(tmp);
		    return pathname;
		}
	    }
	    else {
		/*
		 * case 4b:
		 * couldn't change to globals.dvi_file.dirname - try a kpathsea search from CWD
		 */
		TRACE_SRC((stderr,
			   "trying kpathsearch for filename \"%s\" from CWD",
			   filename));
		tmp = kpse_find_file(filename, pathinfo, True);

		if (tmp != NULL && stat(tmp, statbuf) == 0) {
		    TRACE_SRC((stderr, "Found file: `%s'", tmp));
		    free(pathname);
		    return tmp;
		}
	    }
	}
    }
    /*
     * case 5:
     * try a kpathsea search for pathname
     */
    TRACE_SRC((stderr,
	       "trying kpathsearch for pathname \"%s\"",
	       pathname));
    tmp = kpse_find_file(pathname, pathinfo, True);

    if (tmp != NULL && stat(tmp, statbuf) == 0) {
	TRACE_SRC((stderr, "Found file: `%s'", tmp));
	free(pathname);
	return tmp;
    }

    /* not found */
    free(pathname);
    free(tmp);
    errno = 0;
    return NULL;
}

/*
  Hashtable functions
  
  The purpose of these is to wrap kpathsea's rather strange hash
  functions that can be used to store 2 type of values: char *, and
  long, where the latter is interpreted as char *. (See kpathsea/dir.c
  for an example of where this is used).

  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  BUG ALERT: Note however that a long value may never be removed from
  the hash table with hash_delete(), since it would do a strcmp() on
  the long interpreted as a pointer!
  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  
  We can't use a different approach (like using a void *) since
  the debugging ouput of kpathsea relies on printing the value
  either as char * or long (depending on the value of the global flag
  `kpse_debug_hash_lookup_int').
*/
/*
  If key is in hashtable, return True and the integer value in val;
  else return False (and leave val untouched).
*/
Boolean
find_str_int_hash(hashTableT *hashtable, const char *key, size_t *val)
{
    const_string *ret;
#ifdef KPSE_DEBUG
    if (KPSE_DEBUG_P (KPSE_DEBUG_HASH))
	kpse_debug_hash_lookup_int = True;
#endif
    ret = hash_lookup(*hashtable, key);
#ifdef KPSE_DEBUG
    if (KPSE_DEBUG_P (KPSE_DEBUG_HASH))
	kpse_debug_hash_lookup_int = False;
#endif

    if (ret != NULL) {
	long l = (long)*ret;
	*val = (size_t)l;
	return True;
    }
    return False;
}

/*
  Insert key-value pair into hashtable. Note that the key is
  *not* copied (i.e. it is expected that is had been allocated
  somewhere else, and persists throughout the program).
*/
void
put_str_int_hash(hashTableT *hashtable, const char *key, size_t val)
{
    long ptr = (long)val;
    hash_insert(hashtable, key, (const string)ptr);
}


/*
 *	General AVL tree mechanism.  Search for a node, and return it if found.
 *	Otherwise insert a node.
 *	This uses the AVL algorithm from Knuth Vol. 3.
 */

struct avl *
avladd(const char *key, size_t key_len, struct avl **headp, size_t size)
{
	struct avl	*ap;
	struct avl	**app;
	struct avl	*sp;	/* place where rebalancing may be necessary */
	struct avl	**spp;	/* points to sp */
	int		i;

	/* Search */
	spp = app = headp;
	for (;;) {
	    ap = *app;
	    if (ap == NULL)	/* bottom of tree */
		break;
	    if (ap->bal != 0)
		spp = app;
	    i = key_len - ap->key_len;
	    if (i == 0)
		i = memcmp(key, ap->key, key_len);
	    if (i == 0)		/* found record already */
		return ap;
	    if (i < 0)		/* move left */
		app = &ap->left;
	    else
		app = &ap->right;
	}

	/* Insert */
	ap = xmalloc(size);
	ap->key = key;
	ap->key_len = key_len;
	ap->bal = 0;
	ap->left = ap->right = NULL;
	*app = ap;

	/* Adjust balance factors */
	sp = *spp;
	if (sp == ap)
	    return ap;
	i = key_len - sp->key_len;
	if (i == 0)
	    i = memcmp(key, sp->key, key_len);
	sp = (i < 0 ? sp->left : sp->right);
	while (sp != ap) {
	    i = key_len - sp->key_len;
	    if (i == 0)
		i = memcmp(key, sp->key, key_len);
	    if (i < 0) {
		sp->bal = -1;
		sp = sp->left;
	    }
	    else {
		sp->bal = 1;
		sp = sp->right;
	    }
	}

	/* Balancing act */
	sp = *spp;
	i = key_len - sp->key_len;
	if (i == 0)
	    i = memcmp(key, sp->key, key_len);
	if (i < 0) {
	    if (sp->bal >= 0)
		--sp->bal;
	    else {	/* need to rebalance */
		struct avl *left;

		left = sp->left;
		if (left->bal < 0) {	/* single rotation */
		    sp->left = left->right;
		    left->right = sp;
		    sp->bal = left->bal = 0;
		    *spp = left;
		}
		else {			/* double rotation */
		    struct avl	*newtop;

		    newtop = left->right;
		    sp->left = newtop->right;
		    newtop->right = sp;
		    left->right = newtop->left;
		    newtop->left = left;
		    sp->bal = left->bal = 0;
		    if (newtop->bal < 0) ++sp->bal;
		    else if (newtop->bal > 0) --left->bal;
		    newtop->bal = 0;
		    *spp = newtop;
		}
	    }
	}
	else {
	    if (sp->bal <= 0)
		++sp->bal;
	    else {	/* need to rebalance */
		struct avl *right;

		right = sp->right;
		if (right->bal > 0) {	/* single rotation */
		    sp->right = right->left;
		    right->left = sp;
		    sp->bal = right->bal = 0;
		    *spp = right;
		}
		else {			/* double rotation */
		    struct avl	*newtop;

		    newtop = right->left;
		    sp->right = newtop->left;
		    newtop->left = sp;
		    right->left = newtop->right;
		    newtop->right = right;
		    sp->bal = right->bal = 0;
		    if (newtop->bal > 0) --sp->bal;
		    else if (newtop->bal < 0) ++right->bal;
		    newtop->bal = 0;
		    *spp = newtop;
		}
	    }
	}

	return ap;
}


/* set globals.dvi_name, globals.dvi_file.dirname and globals.dvi_file.dirlen */
void
set_dvi_name_expand(const char *new_filename)
{
    ASSERT(new_filename != NULL, "");
    free(globals.dvi_name);
    globals.dvi_name = expand_filename_append_dvi(new_filename, USE_CWD_PATH, True);

    free(globals.dvi_file.dirname);
    globals.dvi_file.dirname = get_dir_component(globals.dvi_name);

    ASSERT(globals.dvi_file.dirname != NULL, "dvi_name should be a path with dir component");
    globals.dvi_file.dirlen = strlen(globals.dvi_file.dirname);
}   

/* set globals.dvi_name, globals.dvi_file.dirname and globals.dvi_file.dirlen
   In contrast to previous function, input filename is not copied.
*/
void
set_dvi_name(char *new_filename)
{
    ASSERT(new_filename != NULL, "");
    free(globals.dvi_name);
    globals.dvi_name = new_filename;

    free(globals.dvi_file.dirname);
    globals.dvi_file.dirname = get_dir_component(globals.dvi_name);

    ASSERT(globals.dvi_file.dirname != NULL, "dvi_name should be a path with dir component");
    globals.dvi_file.dirlen = strlen(globals.dvi_file.dirname);
}   

/*
 * Copy the file pointer `in' to the file pointer `out'.  Return True
 * if successful, False else (in which case caller should examine
 * errno to find the error).
 * The caller is responsible for closing the files.
 */
Boolean
copy_fp(FILE *in, FILE *out)
{
#define TMP_BUF_SIZE 4 * 1024
    char buf[TMP_BUF_SIZE];
    
    while (feof(in) == 0) {
	size_t bytes_in, bytes_out;
	
	bytes_in = fread(buf, 1, TMP_BUF_SIZE, in);
	if (bytes_in < TMP_BUF_SIZE && !feof(in))
	    return False;
	bytes_out = fwrite(buf, 1, bytes_in, out);
	/* 	fprintf(stderr, "read %d, wrote %d bytes\n", bytes_in, bytes_out); */
	if (bytes_out < bytes_in)
	    return False;
    }
    return True;

#undef TMP_BUF_SIZE
}


/*
 * Copy a file from `from_path' to `to'. Return True if successful, False else
 * (in which case caller should examine errno to find the error).
 */
Boolean
copy_file(const char *from_path, const char *to_path) {
    FILE *from_fp;
    FILE *to_fp;

    Boolean retval;

    if ((from_fp = try_fopen(from_path, "rb")) == NULL) {
	XDVI_ERROR((stderr, "opening %s for reading failed: %s", from_path, strerror(errno)));
	return False;
    }

    if ((to_fp = try_fopen(to_path, "wb")) == NULL) {
	XDVI_ERROR((stderr, "opening %s for writing failed: %s", to_path, strerror(errno)));
	return False;
    }

    retval = copy_fp(from_fp, to_fp);

    fclose(from_fp);
    fclose(to_fp);

    return retval;
}

const char *
get_text_encoding(void)
{
    const char *text_encoding = NULL;
    
    /* if resource.text_encoding isn't set, use nl_langinfo() if langinfo is available */
    if (resource.text_encoding == NULL) {
#if USE_LANGINFO
	if (globals.orig_locale == NULL) {
	    XDVI_ERROR((stderr, "Call to setlocale() returned NULL; assuming ISO-8859-1 charset."));
	    text_encoding = "ISO-8859-1";
	}
	else {
	    if (strcmp(globals.orig_locale, "C") == 0 || strcmp(globals.orig_locale, "POSIX") == 0) {
		/* nl_langinfo returns rather strange values for these ... */
		text_encoding = "ISO-8859-1";
		TRACE_FIND((stderr, "Assuming |%s| for locale |%s|",
			    text_encoding, globals.orig_locale));
	    }
	    else {
		text_encoding = nl_langinfo(CODESET);
		TRACE_FIND((stderr, "nl_langinfo returned: |%s| for locale |%s|",
			    text_encoding, globals.orig_locale));
	    }
	}
#else
	XDVI_WARNING((stderr,
		      "nl_langinfo() not available on this platform, "
		      "and XDvi.textEncoding resource not set; using default "
		      "encoding ISO-8859-1."));
	text_encoding = "ISO-8859-1";
#endif
    }
    else {
	text_encoding = resource.text_encoding;
    }
    return text_encoding;
}

char *
iconv_convert_string(const char *from_enc, const char *to_enc, const char *str)
{
    static Boolean have_warned = False;
#if HAVE_ICONV_H
    size_t input_len = strlen(str);
    size_t conv_len = input_len * 4 + 1; /* worst case ... */
    int conv_len_save = conv_len;
    char *conv_buf = xmalloc(conv_len);
    const char *in_ptr = str;
    const char *out_ptr = conv_buf;

    iconv_t conv_desc = iconv_open(to_enc, from_enc);

    if (conv_desc == (iconv_t)(-1)) {
	if (!have_warned) {
	    popup_message(XtNameToWidget(globals.widgets.top_level, "*find_popup"),
			  MSG_ERR,
			  NULL,
			  "iconv_open() error: Encoding \"%s\" is not supported by this version of iconv.\n"
			  "Please check the output of \"iconv -l\" and set the X resource\n"
			  "\"XDvi.textEncoding\" to an appropriate value.", from_enc);
	    have_warned = True;
	}
	free(conv_buf);
	return NULL;
    }

    TRACE_FIND((stderr, "iconv_convert_string: from `%s', to `%s'", from_enc, to_enc));
    if (iconv(conv_desc, (iconv_char_pptrT)&in_ptr, &input_len, (char **)&out_ptr, &conv_len) == (size_t)(-1)) {
	popup_message(XtNameToWidget(globals.widgets.top_level, "*find_popup"),
		      MSG_ERR,
		      NULL,
		      "iconv_convert_string(): Could not convert %s to %s: %s.",
		      from_enc, to_enc, strerror(errno));
	iconv_close(conv_desc);
	free(conv_buf);
	return NULL;
    }

    iconv_close(conv_desc);
    conv_len = conv_len_save - conv_len;
    conv_buf[conv_len] = '\0';

    TRACE_FIND((stderr, "after iconv conversion: |%s| %lu bytes\n",
		conv_buf, (unsigned long)conv_len));

    
    return conv_buf;
    
#else /* HAVE_ICONV_H */

    UNUSED(from_enc);
    UNUSED(to_enc);
    UNUSED(str);
    
    /* no iconv available */
    if (!have_warned) {
	popup_message(XtNameToWidget(globals.widgets.top_level, "*find_popup"),
		      MSG_ERR,
		      "You can either set the \"LANG\" environment variable or the X resource "
		      "\"XDvi.textEncoding\" to make xdvi use a different language/encoding setting.\n"
		      "Without iconv, only the encodings ISO-8859-1 and UTF-8 are supported. "
		      "For real iconv support, you will need to install the iconv library "
		      "and recompile xdvik.",
		      "Cannot convert from %s to UTF-8 without iconv support compiled in.");
	have_warned = True;
    }
    return NULL;
    
#endif /* HAVE_ICONV_H */
}


/* Replace (pseudo-)format arguments in NULL-terminated argv list as follows:
 * %f -> filename, %l -> linenumber, %c -> column number.
 * If %f or %l are not specified, they are appended as %f and +%l.
 * If colno == 0, no %c argument is provided.
 */
char **
src_format_arguments(char **argv, const char *filename, int lineno, int colno)
{
    size_t i;
    Boolean found_filename = False;
    Boolean found_lineno = False;
    
    for (i = 0; argv[i] != NULL; i++) {
	char *ptr, *curr = argv[i];
	while ((ptr = strchr(curr, '%')) != NULL) {
	    char *p1;
	    if ((p1 = strchr("flc", ptr[1])) != NULL) { /* we have a formatting char */
		char digit_arg[LENGTH_OF_INT];
		const char *new_elem = NULL;
		/* remember offsets and lengths */
		size_t l_init = ptr - argv[i];
		size_t l_rest = strlen(ptr + 2) + 1;
		size_t l_mid;
		
		if (*p1 == 'f') {
		    found_filename = True;
		    new_elem = filename;
		}
		else if (*p1 == 'l') {
		    found_lineno = True;
		    sprintf(digit_arg, "%d", lineno);
		    new_elem = digit_arg;
		}
		else if (*p1 == 'c') {
		    sprintf(digit_arg, "%d", colno);
		    new_elem = digit_arg;
		}
		
		l_mid = strlen(new_elem);
		
		argv[i] = xrealloc(argv[i], strlen(argv[i]) + l_mid + 1);
		curr = argv[i] + l_init; /* need to reinitialize it because of realloc */
		memmove(curr + l_mid, curr + 2, l_rest);
		memcpy(curr, new_elem, l_mid);
		curr += l_mid;
	    }
	    else if (ptr[1] == '%') { /* escaped %, skip both */
		curr = ptr + 2;
	    }
	    else {
		curr = ptr + 1;
	    }
	}
    }

    /* append line number and file name arguments if they were not specified */
    if (!found_lineno) {
	i++;
	argv = xrealloc(argv, (i + 1) * sizeof *argv);
	argv[i - 1] = xmalloc(LENGTH_OF_INT + 2);
	sprintf(argv[i - 1], "+%d", lineno);
	argv[i] = NULL;
    }
    
    if (!found_filename) {
	i++;
	argv = xrealloc(argv, (i + 1) * sizeof *argv);
	argv[i - 1] = xstrdup(filename);
	argv[i] = NULL;
    }

    return argv;
}

char *
xstrndup(const char *str, size_t len)
{
    char *new_str = xmalloc(len + 1);
    memcpy(new_str, str, len);
    new_str[len] = '\0';
    return new_str;
}

#if 0
/*
 * Search integer array <arr> of length <arr_len> for for <item>.
 * Return the index of the item, or the index of the next smaller item
 * if there's no exact match. (That we want the latter is the
 * reason why we can't use bsearch()).
 */
int
binary_search(int *arr, int arr_len, int item)
{
    int lower = -1;
    int upper = arr_len;
    int mid;

    ASSERT(arr_len >= 1, "binary_search expects arrays of length >= 1");
    
    do {
	mid = (lower + upper) / 2;
	if (item > arr[mid])
	    lower = mid;
	else if (item < arr[mid])
	    upper = mid;
	else /* exact match */
	    return mid;
    } while (upper - lower > 1);

    /* no exact match, return next lower item */
    if (mid > 0 && arr[mid] > item)
	return mid - 1;
    else
	return mid;
}
#endif /* 0 */
