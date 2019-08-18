/*========================================================================*\

Copyright (c) 1993-2000  Paul Vojta

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

\*========================================================================*/

#if !lint
static	char	copyright[] =
"@(#) Copyright (c) 1993-1998 Paul Vojta.\n";
#endif

/*
 *	Reference for PK file format:
 *
 *	Tomas Rokicki, Packed (PK) font file format, TUGBoat 6 (1985) 115-120.
 */

/*
 * Kpathsea version by Thomas Esser, John Interrante, Yves Arrouye, Karl Berry.
 */

#include "version.h"

#ifndef KPATHSEA

#include "config.h"

/* Some O/S dependent kludges.  */
#if _AIX
#define _ALL_SOURCE 1
#endif

#if __hpux
#define _HPUX_SOURCE 1
#endif

#if STDC_HEADERS
# include <stdlib.h>
# include <string.h>
#else
# if !HAVE_STRCHR
#  define strchr index
#  define strrchr rindex
# endif
char *strchr(), *strrchr();
# if !HAVE_MEMCPY
#  define memcpy(d, s, n)	bcopy((s), (d), (n))
#  define memmove(d, s, n)	bcopy((s), (d), (n))
#  define memcmp(s1, s2, n)	bcmp((s1), (s2), (n))
# endif
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <setjmp.h>
#include <signal.h>
#include <fcntl.h>

#include <errno.h>
#ifndef errno
extern	int	errno;
#endif

#if HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#ifndef atof
double	atof();
#endif
char	*getenv();

/* <sys/types.h> is already included.  */
#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#ifndef WIFSTOPPED
#define	WIFSTOPPED(stat_val)	(((stat_val) & 0377) == 0177)
#endif

#ifndef WIFSIGNALED
#define	WIFSIGNALED(stat_val)	(((stat_val) & 0377) != 0)
#endif

#ifndef WTERMSIG
#define	WTERMSIG(stat_val)	((stat_val) & 0177)
#endif

#ifndef WEXITSTATUS
#define WEXITSTATUS(stat_val)	((unsigned)(stat_val) >> 8)
#endif

#ifndef SIGCHLD
#define	SIGCHLD	SIGCLD
#endif

/* How to open a text file for reading:  */
#ifndef FOPEN_R_MODE
#define	FOPEN_R_MODE	"r"
#endif

/* How to open a binary file for reading:  */
#ifndef FOPEN_RBIN_MODE
#if DOS
#define	FOPEN_RBIN_MODE "r+b"
#elif VMS || VMCMS || OS2 || WIN32
#define	FOPEN_RBIN_MODE	"rb"
#else
#define	FOPEN_RBIN_MODE	"r"
#endif
#endif /* undef FOPEN_RBIN_MODE */

/* How to open a binary file for writing:  */
#ifndef FOPEN_WBIN_MODE
#if DOS
#define	FOPEN_WBIN_MODE "w+b"
#elif OS2 || WIN32
#define	FOPEN_WBIN_MODE "wb"
#elif VMCMS
#define	FOPEN_WBIN_MODE "wb, lrecl=1024, recfm=f"
#else
#define	FOPEN_WBIN_MODE	"w"
#endif
#endif /* undef FOPEN_WBIN_MODE */

#else /* KPATHSEA */

#include <kpathsea/config.h>
#include <kpathsea/c-errno.h>
#include <kpathsea/c-ctype.h>
#include <kpathsea/c-fopen.h>
#include <kpathsea/c-pathmx.h>
#include <kpathsea/proginit.h>
#include <kpathsea/tex-file.h>
#include <kpathsea/tex-make.h>
#include <kpathsea/variable.h>
#include <kpathsea/version.h>
#include <c-auto.h>
#include <signal.h>
#include <fcntl.h>
#include <setjmp.h>

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#if !HAVE_STRCHR
#define	strchr	index
#endif

#if !HAVE_STRRCHR
#define	strrchr	rindex
#endif

#if HAVE_POLL && !HAVE_POLL_H
#undef HAVE_POLL
#endif

/* Add two new flags to kpathsea_debug for debugging gsftopk */
#define	GSPK_DEBUG_BITMAP	(KPSE_LAST_DEBUG + 1)
#define	GSPK_DEBUG_PK		(KPSE_LAST_DEBUG + 2)

/* <sys/types.h> is already included:  <kpathsea/config.h>
     --> <kpathsea/c-std.h> --> <kpathsea/c-unistd.h>
     --> <kpathsea/systypes.h> --> <sys/types.h>.  */
#if HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif

#if WIN32

/* code from Akira Kakuto */

/* message values for callback */
#define GSDLL_STDIN  1
#define GSDLL_STDOUT 2
#define GSDLL_DEVICE 3
#define GSDLL_SYNC   4
#define GSDLL_PAGE   5
#define GSDLL_SIZE   6
#define GSDLL_POLL   7
/* return values from gsdll_init() */
#define GSDLL_INIT_IN_USE  100
#define GSDLL_INIT_QUIT    101

#include <windows.h>

HINSTANCE hgsdll = NULL;
PROC pgsdll_init = NULL;
PROC pgsdll_exit = NULL;
PROC pgsdll_revision = NULL;

#if 0 /* unused */
static	long revision_n = 0;
static	char product_str[128];
static	char copyright_str[128];
static	long revdate_n;
#endif /* 0, unused */

static void Win32Error(const char *s)
{
  fprintf(stderr, "%s\n", s);
  exit(EXIT_FAILURE);
}

#ifdef _WIN64
#define GSDLLNAME "gsdll64.dll"
#else
#define GSDLLNAME "gsdll32.dll"
#endif

static HINSTANCE gs_locate(void)
{
  hgsdll = GetModuleHandle(GSDLLNAME);
  if(hgsdll == NULL) {
    hgsdll = LoadLibrary(GSDLLNAME);
  }
  return hgsdll;
}

static void gs_dll_release(void)
{
  FreeLibrary(hgsdll);
}

static void gs_dll_initialize(void)
{
  pgsdll_init = GetProcAddress(hgsdll, "gsdll_init");
  pgsdll_exit = GetProcAddress(hgsdll, "gsdll_exit");
  pgsdll_revision = GetProcAddress(hgsdll, "gsdll_revision");
  if(pgsdll_init == NULL || pgsdll_exit == NULL ||
     pgsdll_revision == NULL) {
    fprintf(stderr, "Failed to get proc addresses in GSDLL32.\n");
    gs_dll_release();
    exit(100);
  }
}

/* end of code from Akira Kakuto */

HANDLE hGsThread = NULL;
HANDLE hGsDataIn = 0, hGsDataOut = 0; /* Events to synchronize threads */
/* Arguments to gs dll */
const
char *gs_argv[] = { "rungs.exe",		/* 0, dummy */
		    "-dNODISPLAY",		/* 1, */
		    "-dNOGC",			/* 2, */
		    "-dNOSAFER",		/* 3, */
		    NULL,			/* 4, substarg */
		    "-q",			/* 5, */
		    "--",			/* 6, */
		    NULL,			/* 7, searchpath */
		    NULL,			/* 8, PSname */
		    NULL,			/* 9, dlstring != NULL ? dlstring : "" */
		    NULL,			/* 10, specinfo */
		    NULL,			/* 11, dpistr */
		    NULL			/* 12, NULL terminator */
  };
int gs_argc = 12;

char *buffer_stdin; /* This is the buffer from where data are taken. */

#else /* not WIN32 */

#ifndef WEXITSTATUS
#define WEXITSTATUS(stat_val)	((unsigned)(stat_val) >> 8)
#endif

#ifndef WIFSTOPPED
#define	WIFSTOPPED(stat_val)	(((stat_val) & 0377) == 0177)
#endif

#ifndef WIFSIGNALED
#define	WIFSIGNALED(stat_val)	(((stat_val) & 0377) != 0)
#endif

#ifndef WTERMSIG
#define	WTERMSIG(stat_val)	((stat_val) & 0177)
#endif

#endif /* not WIN32 */

#endif /* KPATHSEA */

#if HAVE_POLL
# include <poll.h>
#else
# if HAVE_SYS_SELECT_H
#  include <sys/select.h>
# else
#  if HAVE_SELECT_H
#   include <select.h>
#  endif
# endif
#endif

#if HAVE_VFORK_H
#include <vfork.h>
#endif

#if _AMIGA
#include <proto/dos.h>
#include <dos/dostags.h>
#endif

#define	NUMBER(x)	(sizeof (x) / sizeof *(x))

/* if POSIX O_NONBLOCK is not available, use O_NDELAY */
#if !defined(O_NONBLOCK) && defined(O_NDELAY)
#define	O_NONBLOCK	O_NDELAY
#endif

#ifndef S_ISDIR
#if defined(S_IFMT) && defined(S_IFDIR)
#define	S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#endif
#endif

#ifndef GS_PATH
#define	GS_PATH	"gs"
#endif

#include <stdarg.h>

typedef	char	Boolean;
#define	True	1
#define	False	0

typedef	int		wide_bool;

#ifndef MAXPATHLEN
#define	MAXPATHLEN	256
#endif

#define	PK_PRE	(char) 247
#define	PK_ID	(char) 89
#define	PK_SPC	(char) 240
#define	PK_POST	(char) 245
#define	PK_NOP	(char) 246

#ifndef KPATHSEA
char	progname[]	= "gsftopk ";
#else
char	progname[]	= "gsftopk(k) ";
#endif

char	version[]	= VERSION;

/*
 *	Command line arguments
 */

#define		OPT_DBG		0x101

Boolean		test		= False;
char		*fontname;
int		fontlen;
char		*mapline	= NULL;
char		*mapfile	= NULL;
char		gspath[]	= GS_PATH;	/* gs interpreter path */
Boolean		dosnames	= False;
Boolean		quiet		= False;

struct option {
	const char	*longname;
	short		shortname;
	Boolean		has_arg;
	void		*addr;
	int		value;
};

static	const struct option	options[]	= {
		{"test",	't',	False,	&test,	True},
		{"mapline",	0,	True,	&mapline, 0},
		{"mapfile",	0,	True,	&mapfile, 0},
		{"interpreter",	'i',	True,	&gspath, 0},
		{"dosnames",	0,	False,	&dosnames, True},
		{"quiet",	'q',	False,	&quiet,	True},
#if defined(KPATHSEA) && defined (KPSE_DEBUG)
		{"debug",	OPT_DBG,True,	NULL,	0},
#endif
		{"version",	'v',	False,	NULL,	0},
		{"help",	'h',	False,	NULL,	0}};

FILE		*pk_file	= NULL;
char		*xfilename;
int		col		= 0;		/* current column number */
const char	*specinfo	= "";
pid_t		gs_pid		= 0;

/*
 *	Config file options
 */

#ifndef KPATHSEA				/* 'H' option */
const char	*config_file_header_path	= HEADERPATH;
#else
const char	*config_file_header_path	= NULL;
#endif

struct p_list {					/* list of 'p' options */
	struct p_list	*next;
	const char	*value;
};

	/* Initialize this list to "psfonts.map".  */

struct p_list	psfonts_map		= {NULL, "psfonts.map"};

struct p_list	*p_head			= &psfonts_map;
struct p_list	**p_tail		= &psfonts_map.next;

/*
 *	Reading from the pipe from ghostscript
 */

Boolean		data_eof	= False;

#if !_AMIGA

#define		BUFSIZE		512

typedef	unsigned char	byte;

int		data_fd;
byte		buffer[BUFSIZE];
byte		*data_out	= buffer;
byte		*data_end	= buffer;

#else /* _AMIGA */

FILE		*data_file;

/* This string will be used to open pipes in and out */
char	tmpname[]	= "gsftopkXXXXXX";

#endif /* _AMIGA */

/*
 *	Information from the .tfm file.
 */

int		tfm_lengths[12];
#define	lh	tfm_lengths[1]
#define	bc	tfm_lengths[2]
#define	ec	tfm_lengths[3]
#define	nw	tfm_lengths[4]

long		checksum;
long		design;
byte		width_index[256];
long		tfm_widths[256];

/*
 *	Information on the bitmap currently being worked on.
 */

byte		*bitmap;
int		width;
int		skip;
int		height;
int		hoff;
int		voff;
int		bytes_wide;
unsigned int	bm_size;
byte		*bitmap_end;
int		pk_len;


#if !_AMIGA

/*
 *	Exit, and kill the child process, too.
 */

static void
exit_toto_too(void)
{
#if !WIN32
	if (gs_pid != 0)
	    kill(gs_pid, SIGKILL);
#else
	if (hGsThread) {
	  switch (WaitForSingleObject(hGsThread, 2000)) {
	  case WAIT_OBJECT_0:
	    CloseHandle(hGsThread);
	    hGsThread = NULL;
	    break;
	  case WAIT_TIMEOUT:
	    fprintf(stderr, "Timeout waiting for Gs thread.\n");
	    break;
	  case WAIT_FAILED:
	    fprintf(stderr, "WaitForSingleObject failed on Gs thread (Error code %d).\n",
		    (int)GetLastError());
	    break;
	  default:
	    break;
	  }

	  if (hGsThread) {
	    if (TerminateThread(hGsThread, 1) == 0) {
	      fprintf(stderr, "... couldn't terminate gs thread\n");
	    }
	    CloseHandle(hGsThread);
	    /* FIXME : is it right to call this ? */
	    gs_dll_release();
	  }	  
	}

	if (hGsDataIn)
	  CloseHandle(hGsDataIn);
	if (hGsDataOut)
	  CloseHandle(hGsDataOut);

#endif
	if (pk_file != NULL) {
	    fclose(pk_file);
	    if (unlink(xfilename) != 0) perror("unlink");
	}

	_exit(1);
}

#else /* _AMIGA */

#define	exit_toto_too()	exit(1)

#endif /* _AMIGA */

/*
 *	Print error message and quit.
 */

static void
oops(const char *message, ...)
{
	va_list	args;

	va_start(args, message);
	if (col != 0) putchar('\n');
	vfprintf(stderr, message, args);
	va_end(args);
	putc('\n', stderr);
	exit_toto_too();
}


/*
 *	Same as oops, but with arguments.
 */
static void
opt_oops(const char *message, ...)
{
	va_list	args;

	va_start(args, message);
	fputs("gsftopk: ", stderr);
	vfprintf(stderr, message, args);
	va_end(args);
	fputs("\nTry `gsftopk --help' for more information.\n", stderr);
	exit(1);
}


#ifndef KPATHSEA

/*
 *	Either allocate storage or fail.
 */

static void *
xmalloc(unsigned size)
{
	void *mem = (void *) malloc(size);

	if (mem == NULL)
	    oops("gsftopk: Cannot allocate %u bytes.\n", size);
	return mem;
}


/*
 *	Either reallocate storage or fail.
 */

static void *
xrealloc(char *oldp, unsigned size)
{
	void	*mem;

	mem = oldp == NULL ? (void *) malloc(size)
	    : (void *) realloc(oldp, size);
	if (mem == NULL)
	    oops("gsftopk: Cannot reallocate %u bytes.\n", size);
	return mem;
}

#endif /* not KPATHSEA */


/*
 *	Get a single white-space-delimited argument (or fail).
 */

static char *
get_one_arg(const char *src)
{
	char		*dest;
	const char	*p;
	unsigned int	len;

	len = strlen(src);
	p = memchr(src, ' ', len);
	if (p != NULL) len = p - src;
	p = memchr(src, '\t', len);
	if (p != NULL) len = p - src;

	dest = xmalloc(len + 1);
	memcpy(dest, src, len);
	dest[len] = '\0';

	return dest;
}

#if !_AMIGA

/*
 *	Signal handlers.
 */

#if WIN32

static BOOL WINAPI
handle_sigterm(DWORD dwCtrlType)
{

	/*
	 *	Fix me:  There is a problem if a system() command is running.
	 *	We should wait for the child process to be interrupted.
	 *	Only way I can think of to do that : rewrite system() based on
	 *	spawn() with parsing of the command line and set a global pid
	 *	Next cwait(pid) in the HandlerRoutine.
	 */

	switch (dwCtrlType) {
	    case CTRL_C_EVENT:
	    case CTRL_BREAK_EVENT:
		fprintf(stderr, "...exiting\n");
		if (hGsThread) {
		  if (TerminateThread(hGsThread, 1) == 0) {
		    fprintf(stderr, "... couldn't terminate gs thread\n");
		  }
		}
		exit_toto_too();
		return FALSE;
	    default:
		fprintf(stderr, "... not exiting\n");
		return TRUE;
        }
}

#else /* not WIN32 */

static	Boolean	got_sigchld	= False;

/* ARGSUSED */
static RETSIGTYPE
handle_sigchild(int signo)
{
	got_sigchld = True;
}

/* ARGSUSED */
static RETSIGTYPE
handle_sigterm(int signo)
{
	exit_toto_too();
}

#endif /* WIN32 */

#define	gs_is_done	(gs_pid == 0)

typedef	int		gsf_wait_t;

#if WIN32

/* This is the callback function for gs. It is mainly used to read and
  write  data on   gs   stdin/stdout. Data exchanges   happen  through
  buffers.  */
static int __cdecl
gsdll_callback(int message, char *str, unsigned long count)
{
  int n;
  static char **pin = &buffer_stdin; /* not yet allocated, so used a pointer on it. */

  switch (message) {

  case GSDLL_STDIN:
    /* Put count chars on gs stdin */
#if DEBUG
    fprintf(stderr, "gs wants %d chars\n", count);
#endif
    strncpy(str, *pin, count);
    *pin += count;
    return strlen(str);

  case GSDLL_STDOUT:
    /* Fill the buffer in, wait for gsftopk to ask for data. */
    WaitForSingleObject(hGsDataOut, INFINITE);
#if DEBUG
    fprintf(stderr, "gs gives %d chars\n", count);
#endif
    data_out = buffer;

    if (str == (char *)NULL || count == 0) {
	data_eof = True;
	data_end = data_out;
	/* Tell data_fillbuf() */
	SetEvent(hGsDataIn);
	return 0;
    }
    n = (count >= BUFSIZE ? BUFSIZE : count);
    memcpy(data_out, str, n);
    data_end = data_out + n;
    /* Tell data_fillbuf() that data are available */
    if (SetEvent(hGsDataIn) == FALSE)
      Win32Error("gsdll_callback/SetEvent");
    /* return the number of chars read */
    return n;

  case GSDLL_DEVICE:
#if DEBUG
    fprintf(stdout,"Callback: DEVICE %p %s\n", str,
	    count ? "open" : "close");
#endif
    break;

  case GSDLL_SYNC:
#if DEBUG
    fprintf(stdout,"Callback: SYNC %p\n", str);
#endif
    break;

  case GSDLL_PAGE:
    fprintf(stdout,"Callback: PAGE %p\n", str);
    break;

  case GSDLL_SIZE:
#if DEBUG
    fprintf(stdout,"Callback: SIZE %p width=%d height=%d\n", str,
	    (int)(count & 0xffff), (int)((count>>16) & 0xffff) );
#endif
    break;

  case GSDLL_POLL:
    return 0; /* no error ? */
  default:
    fprintf(stdout,"%s: gs callback: unknown message=%d\n",progname, message);
    break;
  }
  return 0;
}

/*
  This is the thread function that will load the gs dll and
  send it the data.
*/
static DWORD WINAPI Win32GsSendData(LPVOID lpParam)
{
  int ret;

  if (gs_locate() == NULL) {
    fprintf(stderr, "Can't locate Ghostscript ! Exiting ...\n");
    return EXIT_FAILURE;
  }

  gs_dll_initialize();

  ret = (*pgsdll_init)(gsdll_callback,
		       NULL,
		       gs_argc,
		       gs_argv);

  if (ret == 0) {
    /* Should not happen : gs should quit
       right after being initialized. */
    (*pgsdll_exit)();
    /* FIXME: this is working, but we could expect something cleaner ! */
    /*  GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, 0); */
    return EXIT_FAILURE;
  }

  if (ret == GSDLL_INIT_QUIT) {
    gs_dll_release();
    return 0;
  }

  if (ret == GSDLL_INIT_IN_USE) {
    fprintf(stderr, "gsdll_init returned %d\n", ret);
    gs_dll_release();
    return EXIT_FAILURE;
  }

  (*pgsdll_exit)();

#if DEBUG  
  fprintf(stderr, "%s: gsdll_init returned %d\n", progname, ret);
#endif

  WaitForSingleObject(hGsDataOut, INFINITE);
  data_eof = True;
  data_end = data_out;
  /* Tell data_fillbuf() */
  SetEvent(hGsDataIn);

  gs_dll_release();

  return 0;
}

#else /* not WIN32 */

static void
wait_for_gs(void)
{
	gsf_wait_t	status;

#if _AMIGA

	/* Do nothing */

#else /* not _AMIGA */

	got_sigchld = False;

	for (;;) {
	    if (wait(&status) != -1) break;
	    /* if (errno == EINTR) continue; */
	    if (errno != EINTR) {
		perror("wait");
		exit_toto_too();
	    }
	}

	if (WIFSTOPPED(status))
	    return;

	gs_pid = 0;

	if (WIFSIGNALED(status))
	    oops("gs died due to signal %d\n", WTERMSIG(status));

	/* otherwise, it exited */
	if (WEXITSTATUS(status) != 0)
	    oops("gs terminated abnormally with status %d\n",
	      WEXITSTATUS(status));

#endif /* not _AMIGA */

}

#endif /* not WIN32 */

#endif /* not _AMIGA */

/*
 *	Routines to read from the data file.
 */

#if _AMIGA

#define	data_getc()	getc(data_file)
#define	data_ungetc(c)	ungetc(c, data_file)
#define	data_read(p, l)	fread(p, 1, l, data_file)

#else /* not _AMIGA */

#if WIN32

static void
data_fillbuf(void)
{
  if (data_eof)
    return;

  /* wait until data are available. First tell gs it can fill the buffer in. */
  SetEvent(hGsDataOut);
  /* wait for the data. */
  switch (WaitForSingleObject(hGsDataIn, INFINITE)) {
  case WAIT_OBJECT_0:
    /* normal case */
    break;
  case WAIT_TIMEOUT:
    /* should not happen */
    fprintf(stderr, "Gs did not return on time from callback.\n");
    break;
  case WAIT_FAILED:
    break;
  default:
    fprintf(stderr, "WaitForSingleObject failed for unknown reason.\n");
  }
}

#else /* not WIN32 */

#if HAVE_POLL
#define	ISSET(a, b)		((poll_fd.revents & POLLIN) != 0)
#else
#define	ISSET(a, b)		FD_ISSET(a, b)
#endif

static void
data_fillbuf(void)
{
	int			n;

#if HAVE_POLL
	static	struct pollfd	poll_fd	= {0, POLLIN, 0};
#else
	fd_set			read_fds;
	struct timeval		timeout;
#endif

	if (data_eof)
	    return;

	/* wait for readable data */
	if (!gs_is_done) {
	    for (;;) {
		if (!got_sigchld) {
#if HAVE_POLL
		    poll_fd.fd = data_fd;
		    poll_fd.revents = 0;
		    if (poll(&poll_fd, 1, 5000) == -1) {
			if (errno != EINTR) {
			    perror("poll");
			    sleep(4);
			}
			continue;
		    }
#else
		    FD_ZERO(&read_fds);
		    FD_SET(data_fd, &read_fds);
		    timeout.tv_sec = 5;
		    timeout.tv_usec = 0;
		    if (select(data_fd + 1, &read_fds, (fd_set *) NULL,
		      (fd_set *) NULL, &timeout) == -1) {
			if (errno != EINTR) {
			    perror("select");
			    sleep(4);
			}
			continue;
		    }
#endif
		}
		if (got_sigchld) {
		    wait_for_gs();
		    break;
		}
		if (ISSET(data_fd, &read_fds))
		    break;
	    }
	}

	/* read the data */
	for (;;) {
	    n = read(data_fd, (void *) (data_out = buffer), BUFSIZE);
	    if (n >= 0)
		break;
	    if (errno == EINTR)
		continue;
	    if (gs_is_done && errno == EAGAIN) {
		n = 0;
		break;
	    }
	    perror("read from gs");
	    sleep(4);
	}

	data_end = data_out + n;
	if (n == 0)
	    data_eof = True;
}

#endif /* not WIN32 */

static byte
data_fgetc(void)
{
	if (data_out >= data_end)
	    data_fillbuf();

	return data_eof ? EOF : *data_out++;
}

#define	data_getc()	(data_out < data_end ? *data_out++ : data_fgetc())

static void
data_ungetc(byte c)
{
	if (data_out <= buffer)
	    oops("Too many calls to data_ungetc()");

	*--data_out = c;
}

static int
data_read(byte *buf, int n)
{
	byte	*buf1	= buf;
	byte	*buf_end = buf + n;
	int	n1;

	if (buf1 >= buf_end)
	    return 0;

	while (!data_eof) {
	    n1 = buf_end - buf1;
	    if (n1 > data_end - data_out)
		n1 = data_end - data_out;
	    memcpy(buf1, data_out, n1);
	    buf1 += n1;
	    data_out += n1;
	    if (buf1 >= buf_end) break;
	    data_fillbuf();
	}

	return buf1 - buf;
}

static void
data_gets(byte *buf, int n)
{
	byte	*buf1	= buf;
	byte	*buf_end = buf + n - 1;
	int	n1;
	byte	*p1;

	if (n <= 0)
	    return;

	for (;;) {
	    if (data_eof)
		oops("Premature end of file");
	    n1 = buf_end - buf1;
	    if (n1 > data_end - data_out)
		n1 = data_end - data_out;
	    p1 = (byte *) memchr((char *) data_out, '\n', n1);
	    if (p1 != NULL)
		n1 = p1 + 1 - data_out;
	    memcpy((char *) buf1, (char *) data_out, n1);
	    buf1 += n1;
	    data_out += n1;
	    if (p1 != NULL || buf1 >= buf_end) break;
	    data_fillbuf();
	}

	*buf1 = '\0';

	return;
}

#endif /* not _AMIGA */

/*
 *	Here's the patch searching stuff.  First the typedefs and variables.
 */

#ifndef KPATHSEA
static	char	searchpath[MAXPATHLEN + 1];
#else
static	char	*searchpath;
#endif

#define	HUNKSIZE	(MAXPATHLEN + 2)

struct spacenode {	/* used for storage of directory names */
	struct spacenode	*next;
	char			*sp_end;	/* end of data for this chunk */
	char			sp[HUNKSIZE];
}
	firstnode;

#ifndef KPATHSEA

static	jmp_buf		found_env;
static	FILE		*searchfile;
static	const char	*searchname;
static	int		searchnamelen;

static const char *
find_dbl_slash(const char *sp_bgn, const char *sp_end)
{
	const char	*p;

	for (;;) {
	    p = memchr(sp_bgn, '/', sp_end - sp_bgn);
	    if (p == NULL) return sp_end;
	    if (p[1] == '/') return p;
	    sp_bgn = p + 1;
	}
}

static void
main_search_proc(char *matpos, const char *sp_pos,
	const char *sp_slash, const char *sp_end,
	wide_bool skip_subdirs, struct spacenode *space, char *spacenext)
{
	char		*mp;
	struct stat	statbuf;
	DIR		*dir;
	struct dirent	*entry;
	int		lenleft;
	int		len;
	struct spacenode *space1;
	char		*spacenext1;

	mp = matpos + (sp_slash - sp_pos);
	/* check length */
	if (mp + searchnamelen >= searchpath + sizeof(searchpath) - 2) return;
	memcpy(matpos, sp_pos, sp_slash - sp_pos);
	if (sp_slash == sp_end) {	/* try for a file */
	    *mp = '/';
	    strcpy(mp + (mp == searchpath || mp[-1] != '/'), searchname);
	    searchfile = fopen(searchpath, "r");
	    if (searchfile != NULL) longjmp(found_env, True);
	}
	else {/* try for a subdirectory */
	    *mp = '\0';
	    if (stat(searchpath, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
		*mp++ = '/';
		main_search_proc(mp, sp_slash + 2,
		    find_dbl_slash(sp_slash + 2, sp_end), sp_end,
		    statbuf.st_nlink <= 2, space, spacenext);
	    }
	}
	if (skip_subdirs) return;
	*matpos = '\0';
	dir = opendir(searchpath);
	if (dir == NULL) return;
	lenleft = searchpath + sizeof(searchpath) - matpos;
	space1 = space;
	spacenext1 = spacenext;
	for (;;) {
	    entry = readdir(dir);
	    if (entry == NULL) break;
	    len = NAMLEN(entry) + 1;
	    if (len > lenleft) continue;	/* too long */
	    strcpy(matpos, entry->d_name);
	    if (*matpos == '.' && (matpos[1] == '\0' || (matpos[1] == '.'
		    && matpos[2] == '\0')))
		continue;		/* ignore . and .. */
	    if (stat(searchpath, &statbuf) != 0 || !S_ISDIR(statbuf.st_mode))
		continue;		/* if not a directory */
	    if (statbuf.st_nlink > 2) ++len;
	    if (spacenext1 + len > space1->sp + HUNKSIZE) {
		space1->sp_end = spacenext1;
		if (space1->next == NULL) {
		    space1->next = xmalloc(sizeof(struct spacenode));
		    space1->next->next = NULL;
		}
		space1 = space1->next;
		spacenext1 = space1->sp;
	    }
	    if (statbuf.st_nlink > 2) {
		*spacenext1++ = '/';
		--len;
	    }
	    memcpy(spacenext1, entry->d_name, len - 1);
	    spacenext1[len - 1] = '\0';
	    spacenext1 += len;
	}
	closedir(dir);
	for (;;) {
	    space1->sp_end = spacenext1;
	    if (spacenext == space->sp_end) {
		if (space == space1) break;
		space = space->next;
		spacenext = space->sp;
	    }
	    skip_subdirs = True;
	    if (*spacenext == '/') {
		++spacenext;
		skip_subdirs = False;
	    }
	    len = strlen(spacenext);
	    memcpy(matpos, spacenext, len);
	    matpos[len] = '/';
	    main_search_proc(matpos + len + 1, sp_pos, sp_slash, sp_end,
		skip_subdirs, space1, spacenext1);
	    spacenext += len + 1;
	}
}

static FILE *
search(const char *path, const char *path_var, const char *name)
{
	const char	*env_path	= NULL;
	FILE		*f;

	if (path_var != NULL) {
	    if (*name == '/') {
		strcpy(searchpath, name);
		return fopen(searchpath, "r");
	    }
	    env_path = getenv(path_var);
	}
	if (env_path == NULL) {
	    env_path = path;
	    path = NULL;
	}
	searchname = name;
	searchnamelen = strlen(name);
	for (;;) {
	    const char *p;

	    p = strchr(env_path, ':');
	    if (p == NULL) p = env_path + strlen(env_path);
	    if (p == env_path) {
		if (path != NULL) {
		    f = search(path, (char *) NULL, name);
		    if (f != NULL) return f;
		}
	    }
	    else {
		if (setjmp(found_env))
		    return searchfile;
		main_search_proc(searchpath,
		    env_path, find_dbl_slash(env_path, p), p,
		    True, &firstnode, firstnode.sp);
	    }
	    if (*p == '\0') return NULL;
	    env_path = p + 1;
	}
}

#endif /* not KPATHSEA */

/*
 *	Reading configuration and map files.
 */

char	*long_line		= NULL;	/* for reading config and map files */
int	long_line_len		= 82;	/* allocated length of the above */

static Boolean
fgets_long(FILE *f)
{
	int	len;

	if (fgets(long_line, long_line_len, f) == NULL)
	    return False;

	len = 0;
	for (;;) {
	    len += strlen(long_line + len);
	    if (len > 0 && long_line[len - 1] == '\n') {
		long_line[--len] = '\0';
		break;
	    }
	    if (len < long_line_len - 1)
		break;
	    long_line_len += 80;
	    long_line = xrealloc(long_line, long_line_len);
	    fgets(long_line + len, long_line_len - len, f);
	}

	return True;
}


static void
#ifndef KPATHSEA
getdefaults(FILE *f)
#else
getdefaults(const char *name)
#endif
{
#ifdef KPATHSEA
	FILE		*f;
#endif
	char		*p;
	char		c;
	struct p_list	*p_node;

#ifdef KPATHSEA
	p = kpse_find_file(name, kpse_dvips_config_format, false);
	if (p == NULL)
	    return;

	f = fopen(p, FOPEN_R_MODE);
	if (f == NULL)
	    return;
#endif

	while (fgets_long(f)) {
	    p = long_line;
	    while (*p == ' ' || *p == '\t') ++p;
	    c = *p;
	    if (c == '\0')
		continue;
	    do ++p;
	    while (*p == ' ' || *p == '\t');
	    switch (c) {
		case 'H':
		    config_file_header_path = get_one_arg(p);
		    break;

		case 'p':
		    if (*p == '+')
			do ++p;
			while (*p == ' ' || *p == '\t');
		    else
			p_tail = &p_head;	/* discard old list */

		    p_node = xmalloc(sizeof *p_node);
		    p_node->value = get_one_arg(p);
		    *p_tail = p_node;
		    p_tail = &p_node->next;
		    break;
	    }
	}

	fclose(f);
}


static Boolean
scan_map_file(FILE *f)
{
	while (fgets_long(f))
	    if (memcmp(long_line, fontname, fontlen) == 0
	      && (long_line[fontlen] == '\0' || isspace((unsigned char)long_line[fontlen]))) {
		fclose(f);
		return True;
	    }

	fclose(f);
	return False;
}


/*
 *	Add to dlstring
 */

char		*dlstring	= NULL;
unsigned int	dls_len		= 0;
unsigned int	dls_max		= 0;

static void
addtodls(const char *s)
{
	int	len	= strlen(s);

	if (dls_len + len >= dls_max) {
	    unsigned int newsize = dls_max + 80;

	    if (newsize <= dls_len + len) newsize = dls_len + len + 1;
	    dlstring = xrealloc(dlstring, dls_max = newsize);
	}
	strcpy(dlstring + dls_len, s);
	dls_len += len;
}



static long
getlong(FILE *f)
{
	int	value;

	value = (int) ((byte) getc(f)) << 24;
	value |= (int) ((byte) getc(f)) << 16;
	value |= (int) ((byte) getc(f)) << 8;
	value |= (int) ((byte) getc(f));
	return value;
}


char	line[82];

static void
expect(const char *waitingfor)
{
	for (;;) {
#if !_AMIGA
	    data_gets((byte *) line, sizeof(line));
#else
	    if (fgets(line, sizeof(line), data_file) == NULL)
		oops("Premature end of file");
#endif
	    if (memcmp(line, waitingfor, strlen(waitingfor)) == 0) return;
	    fputs("gs: ", stdout);
	    for (;;) {
		fputs(line, stdout);
		if (*line == '\0' || line[strlen(line) - 1] == '\n') break;
#if !_AMIGA
		data_gets((byte *) line, sizeof(line));
#else
		if (fgets(line, sizeof(line), data_file) == NULL)
		    oops("Premature end of file");
#endif
	    }
	}
}

static void
whitespace(void)
{
	char	c;

	for (;;) {
	    c = data_getc();
	    if (c == '#')
		do c = data_getc(); while (!data_eof && c != '\n');
	    else if (!isspace((unsigned char)c)) {
		data_ungetc(c);
		break;
	    }
	}
}

static int
getint(void)
{
	char	c;
	int	i	= 0;

	do c = data_getc(); while (isspace((unsigned char)c));
	if (c < '0' || c > '9') oops("digit expected");
	do {
	    i = i * 10 + (c - '0');
	    c = data_getc();
	} while (c >= '0' && c <= '9');
	if (!data_eof) data_ungetc(c);
	return i;
}

static	byte	masks[]	= {0, 1, 3, 7, 017, 037, 077, 0177, 0377};

byte	flag;
int	pk_dyn_f;
int	pk_dyn_g;
int	base;		/* cost of this character if pk_dyn_f = 0 */
int	deltas[13];	/* cost of increasing pk_dyn_f from i to i+1 */

/*
 *	Add up statistics for putting out the given shift count
 */

static void
tallyup(int n)
{
	int	m;

	if (n > 208) {
	    ++base;
	    n -= 192;
	    for (m = 0x100; m != 0 && m < n; m <<= 4) base += 2;
	    if (m != 0 && (m = (m - n) / 15) < 13) deltas[m] += 2;
	}
	else if (n > 13) ++deltas[(208 - n) / 15];
	else --deltas[n - 1];
}

/*
 *	Routines for storing the shift counts
 */

static	Boolean	odd	= False;
static	byte	part;

static void
pk_put_nyb(int n)
{
	if (odd) {
	    *bitmap_end++ = (part << 4) | n;
	    odd = False;
	}
	else {
	    part = n;
	    odd = True;
	}
}

static void
pk_put_long(int n)
{
	if (n >= 16) {
	    pk_put_nyb(0);
	    pk_put_long(n / 16);
	}
	pk_put_nyb(n % 16);
}

static void
pk_put_count(int n)
{
	if (n > pk_dyn_f) {
	    if (n > pk_dyn_g)
		pk_put_long(n - pk_dyn_g + 15);
	    else {
		pk_put_nyb(pk_dyn_f + (n - pk_dyn_f + 15) / 16);
		pk_put_nyb((n - pk_dyn_f - 1) % 16);
	    }
	}
	else pk_put_nyb(n);
}

static void
trim_bitmap(void)
{
	byte	*p;
	byte	mask;

	/* clear out garbage bits in bitmap */
	if (width % 8 != 0) {
	    mask = ~masks[8 - width % 8];
	    for (p = bitmap + bytes_wide - 1; p < bitmap_end; p += bytes_wide)
		*p &= mask;
	}

	/*
	 *	Find the bounding box of the bitmap.
	 */

	/* trim top */
	skip = 0;
	mask = 0;
	for (;;) {
	    if (bitmap >= bitmap_end) {	/* if bitmap is empty */
		width = height = hoff = voff = 0;
		return;
	    }
	    p = bitmap + bytes_wide;
	    while (p > bitmap) mask |= *--p;
	    if (mask) break;
	    ++skip;
	    bitmap += bytes_wide;
	}
	height -= skip;
	voff -= skip;
#if DEBUG
#if defined(KPATHSEA) && defined (KPSE_DEBUG)
     if (KPSE_DEBUG_P (GSPK_DEBUG_PK))
#endif
	if (skip < 2 || skip > 3)
	    printf("Character has %d empty rows at top\n", skip);
#endif /* DEBUG */

	/* trim bottom */
	skip = 0;
	mask = 0;
	for (;;) {
	    p = bitmap_end - bytes_wide;
	    while (p < bitmap_end) mask |= *p++;
	    if (mask) break;
	    ++skip;
	    bitmap_end -= bytes_wide;
	}
	height -= skip;
#if DEBUG
#if defined(KPATHSEA) && defined (KPSE_DEBUG)
     if (KPSE_DEBUG_P (GSPK_DEBUG_PK))
#endif
	if (skip < 2 || skip > 3)
	    printf("Character has %d empty rows at bottom\n", skip);
#endif /* DEBUG */

	/* trim right */
	skip = 0;
	--width;
	for (;;) {
	    mask = 0;
	    for (p = bitmap + width / 8; p < bitmap_end; p += bytes_wide)
		mask |= *p;
	    if (mask & (0x80 >> (width % 8))) break;
	    --width;
	    ++skip;
	}
	++width;
#if DEBUG
#if defined(KPATHSEA) && defined (KPSE_DEBUG)
     if (KPSE_DEBUG_P (GSPK_DEBUG_PK))
#endif
	if (skip < 2 || skip > 3)
	    printf("Character has %d empty columns at right\n", skip);
#endif /* DEBUG */

	/* trim left */
	skip = 0;
	for (;;) {
	    mask = 0;
	    for (p = bitmap + skip / 8; p < bitmap_end; p += bytes_wide)
		mask |= *p;
	    if (mask & (0x80 >> (skip % 8))) break;
	    ++skip;
	}
	width -= skip;
	hoff -= skip;
#if DEBUG
#if defined(KPATHSEA) && defined (KPSE_DEBUG)
     if (KPSE_DEBUG_P (GSPK_DEBUG_PK))
#endif
	if (skip < 2 || skip > 3)
	    printf("Character has %d empty columns at left\n", skip);
#endif /* DEBUG */
	bitmap += skip / 8;
	skip = skip % 8;
}

/*
 *	Pack the bitmap using the rll method.  (Return false if it's better
 *	to just pack the bits.)
 */

static Boolean
pk_rll_cvt(void)
{
	static	int	*counts		= NULL;	/* area for saving bit counts */
	static	int	maxcounts	= 0;	/* size of this area */
	unsigned int	ncounts;		/* max to allow this time */
	int	*nextcount;			/* next count value */
	int	*counts_end;			/* pointer to end */
	byte	*rowptr;
	byte	*p;
	byte	mask;
	byte	*rowdup;			/* last row checked for dup */
	byte	paint_switch;			/* 0 or 0xff */
	int	bits_left;			/* bits left in row */
	int	cost;
	int	i;

	/*
	 *	Allocate space for bit counts.
	 */

	ncounts = (width * height + 3) / 4;
	if (ncounts > maxcounts) {
	    if (counts != NULL) free(counts);
	    counts = xmalloc((ncounts + 2) * sizeof(int));
	    maxcounts = ncounts;
	}
	counts_end = counts + ncounts;

	/*
	 *	Form bit counts and collect statistics
	 */
	base = 0;
	memset(deltas, 0, sizeof(deltas));
	rowdup = NULL;	/* last row checked for duplicates */
	p = rowptr = bitmap;
	mask = 0x80 >> skip;
	flag = 0;
	paint_switch = 0;
	if (*p & mask) {
	    flag = 8;
	    paint_switch = 0xff;
	}
	bits_left = width;
	nextcount = counts;
	while (rowptr < bitmap_end) {	/* loop over shift counts */
	    int shift_count = bits_left;

	    for (;;) {
		if (bits_left == 0) {
		    if ((p = rowptr += bytes_wide) >= bitmap_end) break;
		    mask = 0x80 >> skip;
		    bits_left = width;
		    shift_count += width;
		}
		if (((*p ^ paint_switch) & mask) != 0) break;
		--bits_left;
		mask >>= 1;
		if (mask == 0) {
		    ++p;
		    while (*p == paint_switch && bits_left >= 8) {
			++p;
			bits_left -= 8;
		    }
		    mask = 0x80;
		}
	    }
	    if (nextcount >= counts_end) return False;
	    shift_count -= bits_left;
	    *nextcount++ = shift_count;
	    tallyup(shift_count);
	    /* check for duplicate rows */
	    if (rowptr != rowdup && bits_left != width) {
		byte	*p1	= rowptr;
		byte	*q	= rowptr + bytes_wide;
		int	repeat_count;

		while (q < bitmap_end && *p1 == *q) ++p1, ++q;
		repeat_count = (p1 - rowptr) / bytes_wide;
		if (repeat_count > 0) {
		    *nextcount++ = -repeat_count;
		    if (repeat_count == 1) --base;
		    else {
			++base;
			tallyup(repeat_count);
		    }
		    rowptr += repeat_count * bytes_wide;
		}
		rowdup = rowptr;
	    }
	    paint_switch = ~paint_switch;
	}

#if DEBUG
#if defined(KPATHSEA) && defined (KPSE_DEBUG)
	if (KPSE_DEBUG_P (GSPK_DEBUG_BITMAP))
#endif
	{
	    /*
	     *	Dump the bitmap
	     */

	    for (p = bitmap; p < bitmap_end; p += bytes_wide) {
		byte *p1	= p;
		int j;

		mask = 0x80 >> skip;
		for (j = 0; j < width; ++j) {
		    putchar(*p1 & mask ? '@' : '.');
		    if ((mask >>= 1) == 0) mask = 0x80, ++p1;
		}
		putchar('\n');
	    }
	    putchar('\n');
	}
#endif /* DEBUG */

	/*
	 *	Determine the best pk_dyn_f
	 */

	pk_dyn_f = 0;
	cost = base += 2 * (nextcount - counts);
	for (i = 1; i < 14; ++i) {
	    base += deltas[i - 1];
	    if (base < cost) {
		pk_dyn_f = i;
		cost = base;
	    }
	}
	/* last chance to bail out */
	if (cost * 4 > width * height) return False;

	/*
	 *	Pack the bit counts
	 */

	pk_dyn_g = 208 - 15 * pk_dyn_f;
	flag |= pk_dyn_f << 4;
	bitmap_end = bitmap;
	*nextcount = 0;
	nextcount = counts;
	while (*nextcount != 0) {
	    if (*nextcount > 0) pk_put_count(*nextcount);
	    else
		if (*nextcount == -1) pk_put_nyb(15);
		else {
		    pk_put_nyb(14);
		    pk_put_count(-*nextcount);
		}
	    ++nextcount;
	}
	if (odd) {
	    pk_put_nyb(0);
	    ++cost;
	}
	if (cost != 2 * (bitmap_end - bitmap))
	    printf("Cost miscalculation:  expected %d, got %ld\n", cost,
		2 * (long) (bitmap_end - bitmap));
	pk_len = bitmap_end - bitmap;
	return True;
}

static void
pk_bm_cvt(void)
{
	byte	*rowptr;
	byte	*p;
	int	blib1;		/* bits left in byte */
	int	bits_left;	/* bits left in row */
	byte	*q;
	int	blib2;
	byte	nextbyte;

	flag = 14 << 4;
	q = bitmap;
	blib2 = 8;
	nextbyte = 0;
	for (rowptr = bitmap; rowptr < bitmap_end; rowptr += bytes_wide) {
	    p = rowptr;
	    blib1 = 8 - skip;
	    bits_left = width;
	    if (blib2 != 8) {
		int	n;

		if (blib1 < blib2) {
		    nextbyte |= *p << (blib2 - blib1);
		    n = blib1;
		}
		else {
		    nextbyte |= *p >> (blib1 - blib2);
		    n = blib2;
		}
		blib2 -= n;
		if ((bits_left -= n) < 0) {
		    blib2 -= bits_left;
		    continue;
		}
		if ((blib1 -= n) == 0) {
		    blib1 = 8;
		    ++p;
		    if (blib2 > 0) {
			nextbyte |= *p >> (8 - blib2);
			blib1 -= blib2;
			bits_left -= blib2;
			if (bits_left < 0) {
			    blib2 = -bits_left;
			    continue;
			}
		    }
		}
		*q++ = nextbyte;
	    }
	    /* fill up whole (destination) bytes */
	    while (bits_left >= 8) {
		nextbyte = *p++ << (8 - blib1);
		*q++ = nextbyte | (*p >> blib1);
		bits_left -= 8;
	    }
	    /* now do the remainder */
	    nextbyte = *p << (8 - blib1);
	    if (bits_left > blib1) nextbyte |= p[1] >> blib1;
	    blib2 = 8 - bits_left;
	}
	if (blib2 != 8) *q++ = nextbyte;
	pk_len = q - bitmap;
}

static void
putshort(int w)
{
	putc(w >> 8, pk_file);
	putc(w, pk_file);
}

static void
putmed(long w)
{
	putc(w >> 16, pk_file);
	putc(w >> 8, pk_file);
	putc(w, pk_file);
}

static void
putlong(long w)
{
	putc(w >> 24, pk_file);
	putc(w >> 16, pk_file);
	putc(w >> 8, pk_file);
	putc(w, pk_file);
}

static void
putglyph(int cc)
{
	static	Boolean	have_first_line = False;
	static	int	llx, lly, urx, ury;
	static	float	char_width;
	static	byte	*area1	= NULL;
	static unsigned int size1 = 0;
	static	int	i;
	long	dm;
	long	tfm_wid;
	byte	*p;

	if (!quiet) {
	    int wid;
	    static const char *s = "";

	    wid = (cc >= 100) + (cc >= 10) + 4;
	    if (col + wid > 80) {
		s = "\n";
		col = 0;
	    }
	    printf("%s[%d", s, cc);
	    fflush(stdout);
	    col += wid;
	    s = " ";
	}
	if (!have_first_line) {
	    expect("#^");
	    if (sscanf(line, "#^ %d %d %d %d %d %f\n", &i,
		    &llx, &lly, &urx, &ury, &char_width) != 6)
		oops("Cannot scanf first line");
	}
	if (i < cc) oops("Character %d received, %d expected", i, cc);
	if (i > cc) {
	    fprintf(stderr, "Character %d is missing.\n", cc);
	    have_first_line = True;
	    return;
	}
	have_first_line = False;
	hoff = -llx + 2;
	voff = ury + 2 - 1;
	expect("P4\n");
	whitespace();
	width = getint();
	whitespace();
	height = getint();
	(void) data_getc();
	if (width != urx - llx + 4 || height != ury - lly + 4)
	    oops("Dimensions do not match:  %d %d %d %d %d %d",
		llx, lly, urx, ury, width, height);
	bytes_wide = (width + 7) / 8;
	bm_size = bytes_wide * height;
	if (size1 < bm_size) {
	    if (area1 != NULL) free(area1);
	    area1 = xmalloc(bm_size);
	    size1 = bm_size;
	}
	for (p = area1 + (height - 1) * bytes_wide; p >= area1; p -= bytes_wide)
	    if (data_read(p, bytes_wide) != bytes_wide)
		oops("Cannot read bitmap of size %u", bm_size);
	bitmap = area1;
	bitmap_end = bitmap + bm_size;
	trim_bitmap();
	if (height == 0 || !pk_rll_cvt()) pk_bm_cvt();
	tfm_wid = tfm_widths[width_index[cc]];
	dm = (long) (char_width + 0.5) - (char_width < -0.5);
	if (pk_len + 8 < 4 * 256 && tfm_wid < (1<<24) &&
		dm >= 0 && dm < 256 && width < 256 && height < 256 &&
		hoff >= -128 && hoff < 128 && voff >= -128 && voff < 128) {
	    putc(flag | ((pk_len + 8) >> 8), pk_file);
	    putc(pk_len + 8, pk_file);
	    putc(cc, pk_file);
	    putmed(tfm_wid);
	    putc(dm, pk_file);
	    putc(width, pk_file);
	    putc(height, pk_file);
	    putc(hoff, pk_file);
	    putc(voff, pk_file);
	} else
	if (pk_len + 13 < 3 * 65536L && tfm_wid < (1<<24) &&
		dm >= 0 && dm < 65536L && width < 65536L && height < 65536L &&
		hoff >= -65536L && hoff < 65536L &&
		voff >= -65536L && voff < 65536L) {
	    putc(flag | 4 | ((pk_len + 13) >> 16), pk_file);
	    putshort(pk_len + 13);
	    putc(cc, pk_file);
	    putmed(tfm_wid);
	    putshort(dm);
	    putshort(width);
	    putshort(height);
	    putshort(hoff);
	    putshort(voff);
	}
	else {
	    putc(flag | 7, pk_file);
	    putlong(pk_len + 28);
	    putlong(cc);
	    putlong(tfm_wid);
	    putlong((long) (char_width * 65536.0 + 0.5) - (char_width < -0.5));
	    putlong(0);
	    putlong(width);
	    putlong(height);
	    putlong(hoff);
	    putlong(voff);
	}
	fwrite(bitmap, 1, pk_len, pk_file);
	if (!quiet) {
	    putchar(']');
	    fflush(stdout);
	}
}

static void
putspecl(const char *str1, const char *str2)
{
	int	len1	= strlen(str1);
	int	len2	= 0;

	if (str2 != NULL) len2 = strlen(str2);
	if (len1 + len2 > 255) return;
	putc(PK_SPC, pk_file);
	putc(len1 + len2, pk_file);
	fwrite(str1, 1, len1, pk_file);
	if (len2 != 0) fwrite(str2, 1, len2, pk_file);
}

int
main(int argc, char **argv)
{
	FILE		*config_file;
	FILE		*render_ps;
	FILE		*tfm_file;
	char		**argp;
	float		dpi;
	const char	*dvipsrc;
	char		*p;
	char		*PSname		= NULL;
	char		*specinf	= NULL;
	char		*specp		= NULL;	/* NULL pacifies lint */
	char		charlist[10*2 + 90*3 + 156*4 + 1];
	char		designstr[20];
	char		dpistr[20];
#if HAVE_SIGACTION
	struct sigaction sigact;
#endif
#if _AMIGA
	char		fngs[50];
	char		fngsf[50];
	char		tfm_path[256];
	BPTR		in, out;
#else
	char		*substarg;
#endif
#if WIN32
	DWORD idGsThread;
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
#else /* not WIN32 */
#if !_AMIGA
	int		std_out[2];
#endif
	int		std_in[2];
#endif /* not WIN32 */
	int		cc;
	int		ppp;
	int		i;

	argp = argv;
	while (++argp < argv + argc && (*argp)[0] == '-') {
	    const struct option *opt_ptr;
	    const struct option *opt;
	    char *arg = *argp + 1;

	    if (*arg == '\0') --arg;	/* this will flag an error later */
	    if (*arg != '-') {		/* if short argument */
		opt = options;
		for (;;) {
		    if (*arg == opt->shortname)
			break;
		    if (++opt >= options + NUMBER(options))
			opt_oops("invalid option -- %c", *arg);
		}
		if (opt->has_arg) {
		    ++arg;
		    if (*arg == '\0') {
			if (++argp >= argv + argc)
			    opt_oops("option requires an argument -- %c",
			      arg[-1]);
			arg = *argp;
		    }
		}
		else {
		    if (arg[1] != '\0')
			opt_oops("invalid number of bytes in option `%s'",
			  arg - 1);
		}
	    }
	    else {			/* long argument */
		int	len;
		char	*arg1;

		++arg;
		if (*arg == '\0') {	/* if -- */
		    ++argp;
		    break;
		}
		len = strlen(arg);
		arg1 = memchr(arg, '=', len);
		if (arg1 != NULL) {
		    len = arg1 - arg;
		    ++arg1;
		}
		opt = NULL;
		for (opt_ptr = options; opt_ptr < options + NUMBER(options);
		  ++opt_ptr)
		    if (memcmp(arg, opt_ptr->longname, len) == 0) {
			if (opt != NULL)
			    opt_oops("option `%s' is ambiguous.", arg - 2);
			opt = opt_ptr;
		    }
		if (opt == NULL)
		    opt_oops("unrecognized option `%s'", arg - 2);
		if (opt->has_arg) {
		    if (arg1 == NULL) {
			if (++argp >= argv + argc)
			    opt_oops("option `--%s' requires an argument.",
			      opt->longname);
			arg1 = *argp;
		    }
		}
		else {
		    if (arg1 != NULL)
			opt_oops("option `--%s' doesn't allow an argument.",
			  opt->longname);
		}
		arg = arg1;
	    }		/* end long argument */

	    if (opt->addr != NULL) {
		if (opt->has_arg)
		    *((char **) opt->addr) = arg;
		else
		    *((Boolean *) opt->addr) = opt->value;
	    }

	    switch (opt->shortname) {
#if defined(KPATHSEA) && defined (KPSE_DEBUG)
	    case OPT_DBG:
		kpathsea_debug |= atoi(arg);
		break;
#endif
	    case 'h':
#ifndef KPATHSEA
		puts("\
Usage:  gsftopk [OPTION] FONT DPI\n\
Translate the PostScript Type 1 font FONT to PK bitmap format at DPI dpi.\n\
\n\
  -t, --test		check for presence of font in .map file.\n\
  --mapline=LINE	use LINE as the line from the .map file.\n\
  --mapfile=FILE	use FILE as a .map file; default psfonts.map.\n\
  -i GS, --interpreter=GS  use GS as Ghostscript interpreter.\n\
  --dosnames		short pk filename (cmr10.pk instead of cmr10.600pk).\n\
  -q, --quiet		don't print progress information to standard output.\n\
  -h, --help		print this message and exit.\n\
  -v, --version		print version number and exit.\n");
#else
		puts("\
Usage:  gsftopk [OPTION] FONT DPI\n\
Translate the PostScript Type 1 font FONT to PK bitmap format at DPI dpi.\n\
\n\
  -t, --test		check for presence of font in .map file.\n\
  --mapline=LINE	use LINE as the line from the .map file.\n\
  --mapfile=FILE	use FILE as a .map file; default psfonts.map.\n\
  -i GS, --interpreter=GS  use GS as Ghostscript interpreter.\n\
  --dosnames		short pk filename (cmr10.pk instead of cmr10.600pk).\n\
  -q, --quiet		don't print progress information to standard output.\n\
  --debug=NUM		set debugging flags.\n\
  -h, --help		print this message and exit.\n\
  -v, --version		print version number and exit.\n");
#endif
		return 0;
	    case 'v':
#ifndef KPATHSEA
		printf("gsftopk %s\n", version);
#else
		{
		    printf("gsftopk(k) %s\n", version);
		    puts(kpathsea_version_string);
		    puts("Copyright (C) 1993-1998 Paul Vojta.\n\
There is NO warranty.  You may redistribute this software\n\
under the terms of the GNU General Public License\n\
and the standard X consortium copyright notice.\n\
For more information about these matters, see the files\n\
named COPYING and gsftopk.c.\n\
Author of gsftopk: Paul Vojta.");
		}
#endif
		return 0;
	    }
	}

	if (mapfile != NULL && mapline != NULL)
	    opt_oops("cannot specify both `--mapline' and `--mapfile'");

	if (argp >= argv + argc)
	    opt_oops(test ? "must provide a font name"
	      : "must provide a font name and resolution");

	fontname = *argp++;
	fontlen = strlen(fontname);

	if (argp >= argv + argc) {
	    if (!test)
		opt_oops("must provide rendering resolution");
	    dpi = 0.0;
	}
	else {
	    dpi = atof(*argp++);
	    if (dpi <= 0.0)
		opt_oops("DPI argument `%s' must be a positive number", *argp);
	}

	if (argp < argv + argc)
	    opt_oops("no more than two arguments are allowed");

#ifdef KPATHSEA
#ifdef WIN32
	setmode(fileno(stdout), _O_BINARY);
#endif
	kpse_set_program_name(argv[0], "gsftopk");
	kpse_init_prog("GSFTOPK", (int) (dpi + 0.5), NULL, "cmr10");
	if (!test)
	    xputenv_int("KPATHSEA_DPI", (int) (dpi + 0.5));

#if defined(WIN32)
       /* TeXLive uses its own Ghostscript */  
#if !defined(_WIN64)
        texlive_gs_init();
#endif
#endif

#endif

#if _AMIGA
	/* [CL] 21-Jun-97
	   This is quite silly but it really helps things when determining the
	   font supplier and family.
	*/
	putenv("GSFTOPKTFM=");
#endif

	/*
	 * Read the dvips-style config file(s).
	 */

	long_line = xmalloc(long_line_len);	/* initialize fgets_long */

#ifndef KPATHSEA
	config_file = search(CONFIGPATH, "TEXCONFIG", "config.ps");
	if (config_file != NULL)
	    getdefaults(config_file);

	dvipsrc = getenv("DVIPSRC");
	if (dvipsrc == NULL) {
	    dvipsrc = getenv("HOME");
	    if (dvipsrc != NULL) {
		i = strlen(dvipsrc);
		p = xmalloc(i + 10);
		memcpy(p, dvipsrc, i);
		memcpy(p + i, "/.dvipsrc", 10);
	    }
	}
	if (dvipsrc != NULL) {
	    config_file = fopen(dvipsrc, "r");
	    if (config_file != NULL)
		getdefaults(config_file);
	}

	config_file = search(CONFIGPATH, "TEXCONFIG", "config.gsftopk");
	if (config_file != NULL)
	    getdefaults(config_file);
#else
	getdefaults("config.ps");

	dvipsrc = kpse_var_value("DVIPSRC");
	getdefaults(dvipsrc != NULL ? dvipsrc : "$HOME/.dvipsrc");

	getdefaults("config.gsftopk");

	/* Set HEADERPATH from config file.  */

	if (config_file_header_path != NULL)
	    kpse_format_info[kpse_tex_ps_header_format].client_path
	      = config_file_header_path;
#endif

	/*
	 * Get the map line.
	 */

	if (mapline != NULL) {
	    if (memcmp(mapline, fontname, fontlen) != 0
	      || (mapline[fontlen] != '\0' && !isspace((unsigned char)mapline[fontlen])))
		oops("font name does not match --mapline argument");
	}
	else {
	    Boolean font_found;

	    if (mapfile != NULL) {
#ifndef KPATHSEA
		config_file = search(CONFIGPATH, "TEXCONFIG",
				     mapfile);
#else
		config_file = kpse_open_file(mapfile,
					     kpse_fontmap_format);
#endif

		if (config_file == NULL) {
		    perror(mapfile);
		    exit(1);
		}
		font_found = scan_map_file(config_file);
	    }
	    else {
		struct p_list	*p_node;

		font_found = False;
		*p_tail = NULL;
		for (p_node = p_head; p_node != NULL; p_node = p_node->next) {
#ifndef KPATHSEA
		    config_file = search(CONFIGPATH, "TEXCONFIG",
		      p_node->value);
#else
		    config_file = kpse_open_file(p_node->value,
		      kpse_fontmap_format);
#endif
		    if (config_file != NULL)
			if (scan_map_file(config_file)) {
			    font_found = True;
			    break;
			}
		}
	    }

	    if (!font_found) {
		if (test)
		    exit(1);
		else
		    oops("Cannot find font %s in map file(s).", fontname);
	    }

	    mapline = long_line;
	}

	if (test)
	    exit(0);

	if (!quiet) {
	    printf("%sversion %s", progname, version);
	    fflush(stdout);
	    col = 1;	/* any nonzero value will do */
	}

	/*
	 * Parse the line from the map file.
	 */
	for (p = mapline + fontlen; *p != '\0'; ++p) {
	    if (isspace((unsigned char)*p)) continue;
	    if (*p == '<') {
		char	*q;
		char	endc;
		char	c;
		FILE	*f;

		++p;
		/* There may be two '<'s in front of the filename. */
		if (*p == '<') ++p;
		/* ... and maybe a '[' */
		if (*p == '[') ++p;
		q = p;
		while (*p != '\0' && !isspace((unsigned char)*p)) ++p;
		endc = *p;
		*p = '\0';
#ifdef KPATHSEA
		searchpath = kpse_find_file(q, kpse_tex_ps_header_format,true);
		f = searchpath ? fopen(searchpath, FOPEN_R_MODE) : NULL;
#else
		f = search(config_file_header_path, "DVIPSHEADERS", q);
#endif
		if (f == NULL) oops("Cannot find font file %s", q);
		/* search() also sets searchpath */
		addtodls(" (");
		addtodls(searchpath);
		c = getc(f);
		addtodls(c == '\0' ? ") ttload"
		  : c == '\200' ? ") brun"
		  : ") run");
		fclose(f);
		if (endc == '\0') break;
		continue;
	    }
	    else if (*p == '"') {
		char	*q;

		if (specinf != NULL)
		    *specp++ = ' ';
		else
		    specinf = specp = xmalloc(strlen(p));
		++p;
		q = strchr(p, '"');
		if (q == NULL) q = p + strlen(p);
		memcpy(specp, p, q - p);
		specp += q - p;
		p = q;
		if (*p == '\0') break;
		else continue;
	    }
	    else {
		PSname = p;
		while (*p != '\0' && !isspace((unsigned char)*p)) ++p;
		if (*p == '\0') break;
	    }
	    *p = '\0';
	}

#if OLD_DVIPS
	/* Parse lines like `Symbol-Slanted "/Symbol .167 SlantFont"'. */
	if (*(p = specinf) == '/') {
	    PSname = ++p;
	    while (*p && !isspace(*p)) ++p;
	    if (*p) *p++ = '\0';
	    specinf = p;
	}
#endif /* OLD_DVIPS */

	if (specinf != NULL) {
	    *specp = '\0';
	    specinfo = specinf;
	}

	if (PSname == NULL)
	    PSname = fontname;

	/*
	 *	Start up Ghostscript.
	 */

#ifdef KPATHSEA
	searchpath = kpse_find_file("render.ps", kpse_tex_ps_header_format,
				    true);
	render_ps = searchpath ? fopen (searchpath, FOPEN_R_MODE) : NULL;
#else
	render_ps = search(config_file_header_path, "DVIPSHEADERS",
	    "render.ps");
#endif
	if (render_ps == NULL)
	    oops("Cannot find PS driver file \"render.ps\".");
	fclose(render_ps);

#if !_AMIGA
	substarg = xmalloc(strlen(PSname) + 13);
	sprintf(substarg, "-sSUBSTFONT=%s", PSname);
#endif

	sprintf(dpistr, "%f", dpi);

#if _AMIGA

	mktemp(tmpname);

	sprintf(fngs, "fifo:%s/rweK", tmpname);
	sprintf(fngsf, "fifo:%s/rweKm", tmpname);

	std_in[1] = open(fngsf, O_WRONLY|O_TRUNC|O_CREAT, 511);

	if (std_in[1] == -1) {
	    perror("pipe");
	    return 1;
	}

	in = Open(fngs, MODE_OLDFILE);
	out = Open(fngs, MODE_NEWFILE);

	if (in && out) {
	    int error;
	    char *cmd;
	    char formatstr[] = "%s -dNODISPLAY -dNOGC -dNOSAFER -sSUBSTFONT=\"%s\" -q -- \"%s\" \"%s\" \"%s\" \"%s\" \"%s\"";
	    unsigned int len;

	    len = sizeof formatstr + strlen(gspath) + strlen(searchpath)
	      + 2 * strlen(PSname) + (dlstring != NULL ? strlen(dlstring) : 0)
	      + strlen(specinfo) + strlen(dpistr);

	    cmd = xmalloc(len);

	    sprintf(cmd, formatstr, PSname, gspath, searchpath, PSname,
	      dlstring != NULL ? dlstring : "", specinfo, dpistr);

	    error = SystemTags(cmd, SYS_Input, in,
				   SYS_Output, out,
				   SYS_Asynch, TRUE,
				   /* NP_StackSize, 50000, */
				   TAG_DONE, TAG_DONE);
	    free(cmd);

	    if (error == -1) {
		Close(in);
		Close(out);
		PrintFault(error, "System(gs)");
		exit(1);
	    }
	}
	else {
	    perror("pipes");
	    if (in) Close(in);
	    if (out) Close(out);
	    exit(1);
	}

#elif WIN32

	/* (later) */

#else /* neither _AMIGA nor WIN32 */

	if (pipe(std_in) != 0 || pipe(std_out) != 0) {
	    perror("pipe");
	    return 1;
	}

	/* Catch the signal for death of the child process. */

#if HAVE_SIGACTION
	sigact.sa_handler = handle_sigchild;
	(void) sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = SA_NOCLDSTOP;
	(void) sigaction(SIGCHLD, &sigact, (struct sigaction *) NULL);
#else
	(void) signal(SIGCHLD, handle_sigchild);
#endif

	/*
	 * Also catch various termination signals, so that we can kill the
	 * gs process before terminating.
	 */

	(void) signal(SIGHUP, handle_sigterm);
	(void) signal(SIGINT, handle_sigterm);
	(void) signal(SIGQUIT, handle_sigterm);
	(void) signal(SIGTERM, handle_sigterm);

	fflush(stderr);		/* to avoid double flushing */
	gs_pid = vfork();
	if (gs_pid == 0) {
	    close(std_in[1]);
	    dup2(std_in[0], 0);
	    close(std_in[0]);
	    close(std_out[0]);
	    dup2(std_out[1], 1);
	    close(std_out[1]);
	    execlp(gspath, "gs", "-dNODISPLAY", "-dNOGC", "-dNOSAFER", substarg, "-q", "--",
		/* render.ps */ searchpath,
		PSname,
		dlstring != NULL ? dlstring : "", specinfo, dpistr, NULL);
	    if (col != 0) {
		putc('\n', stderr);
		col = 0;
	    }
	    perror(gspath);
	    exit(1);
	}
	if (gs_pid == -1) {
	    perror("fork");
	    exit(1);
	}

#endif /* neither _AMIGA nor WIN32 */

	/*
	 *	Open and read the tfm file.  If this takes a while, at least
	 *	it can overlap with the startup of Ghostscript.
	 */

	xfilename = xmalloc(fontlen + 10);
	strcpy(xfilename, fontname);
#ifdef KPATHSEA
#if _AMIGA
	strcpy(tfm_path, kpse_find_file(xfilename, kpse_tfm_format, true));
	tfm_file = fopen(tfm_path, "r");
#else
	tfm_file = kpse_open_file(xfilename, kpse_tfm_format);
#endif /* _AMIGA */
#else /* not KPATHSEA */
	strcpy(xfilename + fontlen, ".tfm");
	tfm_file = search(TFMPATH, "TEXFONTS", xfilename);
#endif /* not KPATHSEA */
	if (tfm_file == NULL) oops("Cannot find tfm file.");

	for (i = 0; i < 12; ++i) {
	    int j;

	    j = (int) ((byte) getc(tfm_file)) << 8;
	    tfm_lengths[i] = j | (int) ((byte) getc(tfm_file));
	}
	checksum = getlong(tfm_file);
	design = getlong(tfm_file);
	fseek(tfm_file, 4 * (lh + 6), 0);
	p = charlist;
	for (cc = bc; cc <= ec; ++cc) {
	    width_index[cc] = (byte) getc(tfm_file);
	    if (width_index[cc] != 0) {
		sprintf(p, "%d ", cc);
		p += strlen(p);
	    }
	    (void) getc(tfm_file);
	    (void) getc(tfm_file);
	    (void) getc(tfm_file);
	}
	for (i = 0; i < nw; ++i) tfm_widths[i] = getlong(tfm_file);
	fclose(tfm_file);
	p[-1] = '\n';

	sprintf(designstr, "%f\n", (float) design / (1 << 20));

#if !WIN32

	/* write the design size and character list to the file */
	write(std_in[1], designstr, strlen(designstr));
	write(std_in[1], charlist, p - charlist);

	close(std_in[1]);

#else /* WIN32 */
	SetConsoleCtrlHandler(handle_sigterm, TRUE);

	hGsDataIn = CreateEvent(&sa, FALSE, FALSE, "gsDataIn");
	hGsDataOut = CreateEvent(&sa, FALSE, FALSE, "gsDataOut");

	gs_argv[4] = substarg;
	gs_argv[7] = searchpath;
	gs_argv[8] = PSname;
	gs_argv[9] = dlstring != NULL ? dlstring : "";
	gs_argv[10] = specinfo;
	gs_argv[11] = dpistr;

	buffer_stdin = concat(designstr, charlist);

	if ((hGsThread = CreateThread(&sa,             /* security attributes */
				      0,               /* default stack size */
				      Win32GsSendData, /* start address of thread */
				      0,               /* parameter */
				      0,               /* creation flags */
				      &idGsThread      /* thread id */
				      )) == NULL)
	  Win32Error("CreateThread");

#endif /* WIN32 */

/*
 *	Read the output from Ghostscript.
 */

#if _AMIGA

	if ((data_file = fopen(fngsf, "r")) == NULL) {
	    perror("GS_out");
	    exit(1);
	}

#elif WIN32

	/* Nothing */

#else /* neither _AMIGA nor WIN32 */

	data_fd = std_out[0];

	/* Set data_fd for non-blocking I/O */
	if (fcntl(data_fd, F_SETFL, fcntl(data_fd, F_GETFL, 0) | O_NONBLOCK)
	  == -1)
	    perror("fcntl");

#endif /* neither _AMIGA nor WIN32 */

/*
 *	Create pk file and write preamble.
 */

	if (dosnames)
	    strcpy(xfilename + fontlen, ".pk");
	else
	    sprintf(xfilename + fontlen, ".%dpk", (int) (dpi + 0.5));

	if ((pk_file = fopen(xfilename, FOPEN_WBIN_MODE)) == NULL) {
	    perror(xfilename);
	    exit_toto_too();
	}
	putc(PK_PRE, pk_file);
	putc(PK_ID, pk_file);
	expect("V");	/* get GS version */
	i = strlen(line) - 2;
	if (i < 0 || i > 10) i = 0;
	line[1] = '/';
	if (!quiet) {
	    if (i > 0) fwrite(line + 1, 1, i, stdout);
	    putchar('\n');
	    col = 0;
	}
	putc(sizeof(progname) + sizeof(version) + i - 2, pk_file);
	fwrite(progname, 1, sizeof(progname) - 1, pk_file);
	fwrite(version, 1, sizeof(version) - 1, pk_file);
	if (i >= 0) {
	    fwrite(line + 1, 1, i, pk_file);
	}
	putlong(design);
	putlong(checksum);
	ppp = dpi / 72.27 * 65536.0 + 0.5;
	putlong(ppp);	/* hppp */
	putlong(ppp);	/* vppp */

/*
 *	Write the actual characters.
 */

	for (cc = bc; cc <= ec; ++cc)
	    if (width_index[cc] != 0)
		putglyph(cc);

#if _AMIGA

	fclose(data_file);

#elif WIN32

	/* Release data buffer, enable thread to terminate */
	SetEvent(hGsDataOut);

#ifdef DEBUG
	fprintf(stderr, "Waiting for thread ... \n");
#endif
	if (hGsThread) {
	  switch (WaitForSingleObject(hGsThread, 2000)) {
	  case WAIT_OBJECT_0:
	    CloseHandle(hGsThread);
	    hGsThread = NULL;
	    break;
	  case WAIT_TIMEOUT:
	    fprintf(stderr, "Timeout waiting for Gs thread.\n");
	    break;
	  case WAIT_FAILED:
	    fprintf(stderr, "WaitForSingleObject failed on Gs thread (Error code %d).\n",
		    (int)GetLastError());
	    break;
	  default:
	    break;
	  }
	}

	if (hGsThread) {
	  if (TerminateThread(hGsThread, 1) == 0) {
	    fprintf(stderr, "... couldn't terminate gs thread\n");
	  }
	  CloseHandle(hGsThread);
	  /* FIXME : is it right to call this ? */
	  gs_dll_release();
	}
	if (hGsDataIn)
	  CloseHandle(hGsDataIn);
	if (hGsDataOut)
	  CloseHandle(hGsDataOut);

#else /* neither _AMIGA nor WIN32 */

	close(data_fd);
	if (!gs_is_done)
	    wait_for_gs();

#endif /* neither _AMIGA nor WIN32 */

/*
 *	Write out identifying specials:
 *		jobname=(font)
 *		mag=1
 *		mode=modeless
 *		pixels_per_inch=(dpi)
 */

	putspecl("jobname=", fontname);
	putspecl("mag=1", NULL);
	putspecl("mode=modeless", NULL);
	sprintf(dpistr, "%d", (int) dpi);
	putspecl("pixels_per_inch=", dpistr);

/*
 *	Postamble
 */

	putc(PK_POST, pk_file);
	while (ftell(pk_file) % 4 != 0) putc(PK_NOP, pk_file);
	fclose(pk_file);
	if (!quiet) putchar('\n');
	col = 0;

#if _AMIGA
	/* [CL] 21-Jun-97
	   The same silly thing to indicate the path of the tfm file
	*/
	{
	    char tmpstr[80];

	    sprintf(tmpstr, "GSFTOPKTFM=%s", tfm_path);
	    putenv(tmpstr);
	}
#endif

	return 0;
}
