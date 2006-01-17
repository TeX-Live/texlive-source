/*
   font_open.c: find font filenames.  This bears no relation (but the
   interface) to the original font_open.c.

Copyright (c) 1999-2004  The texk project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 */

#include "xdvi-config.h"
#include "xdvi.h"
#include "dvi-draw.h"
#include "util.h"
#include "events.h"
#include "my-snprintf.h"
#include "print-log.h"

#include "statusline.h"
#include "font-open.h"

#include "kpathsea/c-fopen.h"
#include "kpathsea/tex-glyph.h"

#include <stdlib.h>
#include <ctype.h>

#if HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif
#ifndef WIFEXITED
# define WIFEXITED(status)	(((status) & 255) == 0)
#endif
#ifndef WEXITSTATUS
# define WEXITSTATUS(status)	((unsigned)(status) >> 8)
#endif
#ifndef WIFSIGNALED
# ifndef WIFSTOPPED
#  define WIFSTOPPED(status)	(((status) & 0xff) == 0x7f)
# endif
# define WIFSIGNALED(status)	(!WIFSTOPPED(status) && !WIFEXITED(status))
#endif
#ifndef WTERMSIG
# define WTERMSIG(status)	((status) & 0x7f)
#endif

/* if POSIX O_NONBLOCK is not available, use O_NDELAY */
#if !defined(O_NONBLOCK) && defined(O_NDELAY)
# define O_NONBLOCK O_NDELAY
#endif

#ifdef EWOULDBLOCK
# ifdef EAGAIN
#  define AGAIN_CONDITION	(errno == EWOULDBLOCK || errno == EAGAIN)
# else
#  define AGAIN_CONDITION	(errno == EWOULDBLOCK)
# endif
#else /* EWOULDBLOCK */
# ifdef EAGAIN
#  define AGAIN_CONDITION	(errno == EAGAIN)
# endif
#endif /* EWOULDBLOCK */

#if HAVE_POLL
# include <poll.h>
# define XIO_IN POLLIN
# define XIO_OUT POLLOUT
#else
# define XIO_IN 1
# define XIO_OUT 2
#endif /* HAVE_POLL */

#if 0
// static int mktexpk_io[2];
// static struct xchild mktexpk_child = { NULL, 0, True, "font creation", NULL, NULL, mktexpk_ended };
// 
// static char *read_from_mktexpk(int ignored);
// static void write_to_mktexpk(int ignored);
// 
// static struct xio mktexpk_xio = { NULL, 0, XIO_IN,
// #if HAVE_POLL
// 				  NULL,
// #endif
// 				  read_from_mktexpk,
// 				  NULL};
// 

static void
mktexpk_ended(int status, struct xchild *this)
{
    char str[1024] = "";
    char *err_msg = NULL;

    fprintf(stderr, "------- MKTEXPK_ENDED!\n");
    if (this->io != NULL && WIFEXITED(status)) {
	err_msg = (this->io->read_proc)(this->io->fd);
	SNPRINTF(str, 1024, "\nProcess `%s' returned exit code %d.\n",
		this->name, WEXITSTATUS(status));
	str[1024 - 1] = '\0';
	printlog_append_str(str);
	if (err_msg != NULL) {
	    fprintf(stderr, "FROM MKTEXPK: |%s|\n", err_msg);
	    printlog_append_str(err_msg);
	}
    }
    printlog_enable_closebutton();
/*     free(this->name); */
/*     free(this->io); */
/*     free(this); */
    
//     read_from_mktexpk(0);
//     clear_io(this->io);
//     (void)close(mktexpk_xio.fd);
// 
//     if (WIFEXITED(status)) {
// 	if (WEXITSTATUS(status) == 0) {
// 	    printlog_append("Done.\n", strlen("Done.\n"));
// 	}
// 	else
// 	    sprintf(str, "\nPrint process returned exit code %d.\n",
// 		    WEXITSTATUS(status));
//     }
//     else if (WIFSIGNALED(status))
// 	sprintf(str, "\nPrint process terminated by signal %d.\n",
// 		WTERMSIG(status));
//     else
// 	sprintf(str, "\nPrint process returned unknown status 0x%x.\n",
// 		status);
// 

}

static char *
read_from_mktexpk(int fd)
{
    int bytes;
    char line[80];
    char *buf;

    fprintf(stderr, "------- READ_FROM_MKTEXPK!\n");
    for (;;) {
#ifndef MOTIF
	bytes = read(fd, line, sizeof line);
#else
	bytes = read(fd, line, sizeof line - 1);
#endif
	if (bytes < 0) {
	    if (AGAIN_CONDITION)
		break;
	    perror("xdvi: read_from_mktexpk");
	    break;
	}

	if (bytes == 0)
	    break;
	else {
#ifdef MOTIF
	    line[bytes] = '\0';
#endif
	    //	    fprintf(stderr, "------- READ_FROM_MKTEXPK:|%s|\n", line);
	    printlog_append(line, bytes);
	}
    }
    buf = xmalloc(bytes + 1);
    memcpy(buf, line, bytes);
    buf[bytes] = '\0';
    return buf;
}

// static void
// write_to_mktexpk(int ignored)
// {
//     UNUSED(ignored);
//     
//     return;
// }
#endif /* 0 */



/* We try for a VF first because that's what dvips does.  Also, it's
   easier to avoid running mktexpk if we have a VF this way.  */

FILE *
font_open(char *font, char **font_ret,
	  double dpi, int *dpi_ret,
	  char **filename_ret
#ifdef T1LIB
	  , int *t1id
#endif
	  )
{
    char *name;
    kpse_glyph_file_type file_ret;

    /* defaults in case of success; filename_ret will be
       non-NULL iff the fallback font is used.
    */
    *font_ret = NULL;
    /* filename_ret is NULL iff a T1 version of a font has been used */
    *filename_ret = NULL;
    *dpi_ret = dpi;

    if (resource.omega) { /* for omega, first try 16-bit ovf's, then 8-bit vf's. */
	name = kpse_find_ovf(font);
	if (name == NULL)
	    name = kpse_find_vf(font);
    }
    else {
	name = kpse_find_vf(font);
    }
    
#ifdef T1LIB
    if (resource.t1lib) {
	*t1id = -1;
    }
#endif /* T1LIB */

    if (name) { /* found a vf font */
	/* pretend it has the expected dpi value, else caller will complain */
 	*dpi_ret = dpi;
	*filename_ret = name;
	return XFOPEN(name, FOPEN_R_MODE);
    }

#ifdef T1LIB
    if (resource.t1lib) {
	/* First attempt: T1 font of correct size */
	*t1id = find_T1_font(font);
	if (*t1id >= 0) {
	    TRACE_T1((stderr, "found T1 font %s", font));
	    return NULL;
	}
	TRACE_T1((stderr,
		  "T1 version of font %s not found, trying pixel version next, then fallback",
		  font));
    }
#endif /* T1LIB */
    
#ifdef EXPERIMENTAL_DELAYED_MTKEXPK
    /*
      From this point on, kpathsea might create some fonts (and fail horribly ...)
      We want to catch its error messages and present them to the user in a window,
      so fork and collect all output of the child into a pipe, similar to what's done for
      dvips.

      Actually, the approach outlined here is bogus, since it still
      ignores the main issue of knowing, in the parent, when all the
      font creation commands have finished (so that we can start
      drawing the pages). What is needed here is a child procedure
      that works through a stack of font names, creating the fonts if
      needed, and the parent checking the return values from these
      font creation processes. Only after the child is finished,
      xdvi can go on drawing the page, by calling the drawing routine
      *from within* mktexpk_ended().
    */
    {
	struct xchild *mktexpk_child = xmalloc(sizeof *mktexpk_child);
	struct xio *mktexpk_xio = xmalloc(sizeof *mktexpk_xio);
	int mktexpk_io[2];
	int pid;
	
	fprintf(stderr, "BEFORE CREATING PIPE\n");
	
	if (xpipe(mktexpk_io) != 0) {
	    perror("[xdvi] pipe");
	    return NULL;
	}

	/* flush output buffers to avoid double buffering (i.e. data
	   waiting in the output buffer being written twice, by the parent
	   and the child) */
	fflush(stderr);
	fflush(stdout);

	switch (pid = fork()) {
	case -1:	/* forking error */
	    perror("fork");
	    close(mktexpk_io[0]);
	    close(mktexpk_io[1]);
	    return NULL;
	case 0:	/* child */
	    fprintf(stderr, "CHILD\n");
	    close(mktexpk_io[0]); /* no reading from io */
	    
	    /* make stdout and stderr of child go to mktexpk_io[1] */
	    if (mktexpk_io[1] == STDOUT_FILENO)
		XDVI_ABORT((stderr, "mktexpk_io[1] shouldn't be STDOUT_FILENO"));
	    if (dup2(mktexpk_io[1], STDOUT_FILENO) != STDOUT_FILENO)
		XDVI_ABORT((stderr, "dup2: mktexpk_io[1], STDOUT_FILENO: %s", strerror(errno)));

	    if (mktexpk_io[1] == STDERR_FILENO)
		XDVI_ABORT((stderr, "mktexpk_io[1] shouldn't be STDERR_FILENO"));
	    if (dup2(mktexpk_io[1], STDERR_FILENO) != STDERR_FILENO)
		XDVI_ABORT((stderr, "dup2: mktexpk_io[1], STDERR_FILENO: %s", strerror(errno)));
	    close(mktexpk_io[1]);

	    fprintf(stderr, "Running ...\n");
	    fprintf(stdout, "Running to stdout ...\n");

	    /* TODO: We could use a convenience routine from kpathsea (which we'd need to create first though)
	       like
	       kpse_find_glyph_nocreate()
	       that just returns NULL instead of invoking mktexpk. However this still doesn't give us
	       access to error messages from mktexpk etc.
	       Maybe it would be better to collect all output from mktexpk into a `log' window, and at
	       the same time filter the output for (shorter) messages in the statusline, a la:
	       `generating font %s (n of m) ...'
	       How is the `n of m' part done in e.g. kdvi? Investigate.
	    */
    
	    /* Second attempt: PK/GF/... font within allowable size range */
	    name = kpse_find_glyph(font, (unsigned)(dpi + .5),
				   kpse_any_glyph_format, &file_ret);
    
	    if (name) { /* success */
		fprintf(stderr, "\n1 DPI: %d\nFONTNAME: %s\n", file_ret.dpi, name);
		*dpi_ret = file_ret.dpi;
		*filename_ret = name;
		_exit(0);
		/* ?		return XFOPEN(name, FOPEN_R_MODE); */
	    }
	    else if (resource.alt_font != NULL) {
		/* The strange thing about kpse_find_glyph() is that it
		   won't create a PK version of alt_font if it doesn't
		   already exist. So we invoke it explicitly a second time
		   for that one.
		*/
		TRACE_T1((stderr, "Trying fallback"));
#ifdef T1LIB
		if (resource.t1lib) {
		    /* Third attempt: T1 version of fallback font */
		    *t1id = find_T1_font(resource.alt_font);
		    if (*t1id >= 0) {
			TRACE_T1((stderr, "found fallback font for %s: %s", font, resource.alt_font));
			*font_ret = xstrdup(resource.alt_font);
			_exit(0);
		    }
		    TRACE_T1((stderr,
			      "Type1 version of fallback font %s not found, trying pixel version",
			      resource.alt_font));
		}
#endif /* T1LIB */
		/* Forth attempt: PK version of fallback font */
		name = kpse_find_glyph(resource.alt_font, (unsigned)(dpi + .5),
				       kpse_any_glyph_format, &file_ret);
		if (name) { /* success */
		    fprintf(stderr, "\n2 DPI: %d\nFONTNAME: %s\nRET_FONT: %s\n", file_ret.dpi, name, resource.alt_font);
		    *dpi_ret = file_ret.dpi;
		    *filename_ret = name;
		    *font_ret = xstrdup(resource.alt_font);
		    _exit(0);
 		    /* ? return XFOPEN(name, FOPEN_R_MODE); */
		}
	    }
	    _exit(1);
	default: /* parent */
	    close(mktexpk_io[1]); /* no writing to io */

	    mktexpk_xio->next = NULL;
	    mktexpk_xio->fd = mktexpk_io[0];
	    mktexpk_xio->xio_events = XIO_IN;
#if HAVE_POLL
	    mktexpk_xio->pfd = NULL;
#endif
	    mktexpk_xio->read_proc = read_from_mktexpk;
	    mktexpk_xio->write_proc = NULL;
	    
	    mktexpk_child->next = NULL;
	    mktexpk_child->pid = pid;
	    mktexpk_child->name = xstrdup(font);
	    mktexpk_child->data = NULL;
	    mktexpk_child->proc = mktexpk_ended;
	    mktexpk_child->io = mktexpk_xio;
	    
	    set_chld(mktexpk_child);

	    /* TODO: get DPI and FONTNAME from the log window, save to *dpi_ret, *filename_ret and *font_ret
	       and return XFOPEN(name, FOPEN_R_MODE);
	    */
	    
	    return NULL;
	}
    }
    
#else /* EXPERIMENTAL_DELAYED_MTKEXPK */
    
    /*
      TODO:

      Probably a better approach would be as follows:

      1. Read the postamble to get all font definitions. Then, set:
      
	kpse_set_program_enabled(kpse_any_glyph_format, False, kpse_src_compile);

	and run load_font() on all of the fonts, with an array in which to save
	the names that don't exist (that returned NULL).

      2. Run load_font() again on the fonts that didn't exist in step
         (1) and display the output in a window. This somehow needs to
         be fork()ed so that the window itself remains responsive.
	 (Maybe it's easier to call mktexpk directly on the command-line?)

         _________________________________________________________
         |                                                       |
         |   Xdvi is creating fonts, please be patient ...       |
         |                                                       |
         |   Font xyz (n of m)                                   |
         |                                                       |
         |   Errors: 0          [ Show Details ... ]             |
         |                                                       |
         |   [ ... some progress meter or busy indicator ... ]   |
         |                                                       |
         |                                                       |
         |   [ Exit xdvi ]                            [ Help ]   |
         |                                                       |
         ---------------------------------------------------------
	 
      This window can be shown before the main window is opened.

    */

    /* Second try: PK/GF/... font within allowable size range */
    /*
      NOTE SU: The problem with this is that it will already use the PK version
      of the fallback font (e.g. cmr10.600pk) if the PK version exists, so the
      Type1 version of the fallback won't get used at all. But maybe this isn't
      that severe, given that the font is grossly wrong anyway.
     */
    name = kpse_find_glyph(font, (unsigned)(dpi + .5),
			   kpse_any_glyph_format, &file_ret);
    
    if (name) { /* success */
	*dpi_ret = file_ret.dpi;
	*filename_ret = name;
	*font_ret = file_ret.name;
	TRACE_T1((stderr, "Found pixel version: %s at %d dpi", file_ret.name, *dpi_ret));
	return XFOPEN(name, FOPEN_R_MODE);
    }
    else if (resource.alt_font != NULL) {
	/* The strange thing about kpse_find_glyph() is that it
	   won't create a PK version of alt_font if it doesn't
	   already exist. So we invoke it explicitly a second time
	   for that one.
	*/
	TRACE_T1((stderr, "Trying fallback"));
#ifdef T1LIB
	if (resource.t1lib) {
	    /* Third attempt: T1 version of fallback font */
	    *t1id = find_T1_font(resource.alt_font);
	    if (*t1id >= 0) {
		TRACE_T1((stderr, "found fallback font for %s: %s", font, resource.alt_font));
		*font_ret = xstrdup(resource.alt_font);
		return NULL;
	    }
	    TRACE_T1((stderr,
		      "Type1 version of fallback font %s not found, trying pixel version",
		      resource.alt_font));
	}
#endif /* T1LIB */
	/* Forth attempt: PK version of fallback font */
	name = kpse_find_glyph(resource.alt_font, (unsigned)(dpi + .5),
			       kpse_any_glyph_format, &file_ret);
	if (name) { /* success */
	    TRACE_T1((stderr, "Success for PK version of fallback"));
	    *dpi_ret = file_ret.dpi;
	    *filename_ret = name;
	    *font_ret = xstrdup(resource.alt_font);
	    return XFOPEN(name, FOPEN_R_MODE);
	}
	else {
	    TRACE_T1((stderr, "Failure for PK version of fallback"));
	}
    }
#endif /* EXPERIMENTAL_DELAYED_MTKEXPK */
    /* all other cases are failure */
    TRACE_T1((stderr, "Failure"));
    return NULL;
}
