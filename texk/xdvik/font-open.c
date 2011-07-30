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
#include "dvi-init.h"
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
static int mktexpk_io[2];
static struct xchild mktexpk_child = { NULL, 0, True, "font creation", NULL, NULL, mktexpk_ended };

static char *read_from_mktexpk(int ignored);
static void write_to_mktexpk(int ignored);

static struct xio mktexpk_xio = { NULL, 0, XIO_IN,
#if HAVE_POLL
				  NULL,
#endif
				  read_from_mktexpk,
				  NULL, NULL};


static void
mktexpk_ended(int status, struct xchild *this)
{
    char str[1024] = "";
    char *err_msg = NULL;

    fprintf(stderr, "------- MKTEXPK_ENDED!\n");
    if (this->io != NULL && WIFEXITED(status)) {
	err_msg = (this->io->read_proc)(this->io->fd, NULL);
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
    
    read_from_mktexpk(0);
    clear_io(this->io);
    (void)close(mktexpk_xio.fd);

    if (WIFEXITED(status)) {
	if (WEXITSTATUS(status) == 0) {
	    printlog_append("Done.\n", strlen("Done.\n"));
	}
	else
	    sprintf(str, "\nPrint process returned exit code %d.\n",
		    WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
	sprintf(str, "\nPrint process terminated by signal %d.\n",
		WTERMSIG(status));
    else
	sprintf(str, "\nPrint process returned unknown status 0x%x.\n",
		status);


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
	    fprintf(stderr, "------- READ_FROM_MKTEXPK:|%s|\n", line);
	    printlog_append(line, bytes);
	}
    }
    buf = xmalloc(bytes + 1);
    memcpy(buf, line, bytes);
    buf[bytes] = '\0';
    return buf;
}

static void
write_to_mktexpk(int ignored)
{
    UNUSED(ignored);
    
    return;
}
#endif /* 0 */


#if DELAYED_MKTEXPK

/* hash table for names of missing fonts, and their indexes */
static hashTableT missing_font_hash;

/* counters for missing fonts */
static int missing_font_ctr = 0;
static int missing_font_curr = 0;

static const char *const dummy_font_value = ""; /* used as value in hash table of missing fonts ... */

/* static char **all_fonts = NULL; */
/* static size_t all_fonts_size = 0; */

void
reset_missing_font_count(void)
{
    missing_font_ctr = missing_font_curr = 0;
}

/* Register font `fname' at size `dpi' as a font for which we'll need
 * to create a PK file
 */
static void
add_missing_font(const char *fname, int dpi)
{
    char *buf = NULL;
    
    if (missing_font_hash.size == 0) {
	missing_font_hash = hash_create(197);
    }

    /* font name (hash key) needs to be dynamically allocated here */
    buf = xmalloc(strlen(fname) + strlen(" at ") + LENGTH_OF_INT + 1);
    sprintf(buf, "%s at %d", fname, dpi);
    if (hash_lookup(missing_font_hash, buf) == NULL) {
	missing_font_ctr++;
	hash_insert(&missing_font_hash, buf, dummy_font_value);
    }
}

/* Check if font `fname' at size `dpi' is in the hash of fonts for which
 * we need to create a PK file. If it is, return its index (>= 0) and delete
 * it from the hash table; else, return -1.
 */
static Boolean
get_and_remove_missing_font(const char *fname, int dpi)
{
    char buf[1024];

    if (missing_font_hash.size == 0)
	return False;

    SNPRINTF(buf, 1024, "%s at %d", fname, dpi);
    buf[1023] = '\0';
    if (hash_lookup(missing_font_hash, buf) == NULL) {
	return False;
    }
    
    hash_remove(&missing_font_hash, buf, dummy_font_value);
    return True;
}


static Boolean
message_font_creation(const char *fname, int dpi)
{
    if (get_and_remove_missing_font(fname, dpi)) {
	missing_font_curr++;
	statusline_info(STATUS_MEDIUM,
			 "Creating PK font: %s at %d dpi (%d of %d) ...",
			 fname,
			 dpi,
			 missing_font_curr,
			 missing_font_ctr);
	force_statusline_update();
	return True;
    }
    return False;
}
#endif /* DELAYED_MKTEXPK */

FILE *
font_open(
#if DELAYED_MKTEXPK
	  Boolean load_font_now,
#endif
	  struct font *fontp,
	  const char **font_ret,
	  int *dpi_ret,
#ifdef T1LIB
	  int *t1id,
#endif
	  Boolean use_t1lib)
{
    char *name;
    kpse_glyph_file_type file_ret;
#if DELAYED_MKTEXPK
    Boolean message_done = False;
    Boolean need_statusline_update = False;
#endif
    /* defaults in case of success; filename_ret will be
       non-NULL iff the fallback font is used.
    */
    *font_ret = NULL;
    /* filename_ret is NULL iff a T1 version of a font has been used */
    fontp->filename = NULL;
    *dpi_ret = fontp->fsize;

    if (resource.omega) { /* for omega, first try 16-bit ovf's, then 8-bit vf's. */
	name = kpse_find_ovf(fontp->fontname);
	if (name == NULL)
	    name = kpse_find_vf(fontp->fontname);
    }
    else {
	name = kpse_find_vf(fontp->fontname);
    }
    
#ifdef T1LIB
    if (resource.t1lib) {
	*t1id = -1;
    }
#endif /* T1LIB */

    if (name) { /* found a vf font */
	/* pretend it has the expected dpi value, else caller will complain */
 	*dpi_ret = fontp->fsize;
	fontp->filename = name;
	return XFOPEN(name, FOPEN_R_MODE);
    }

#ifdef T1LIB
    if (resource.t1lib && use_t1lib
#if DELAYED_MKTEXPK
	&& load_font_now
#endif
	) {
	/* First attempt: T1 font of correct size
	 * (for delayed_mtkexpk, only when scanning postamble for the first time)
	 */
	*t1id = find_T1_font(fontp->fontname);
	if (*t1id >= 0) {
	    TRACE_T1((stderr, "found T1 font %s", fontp->fontname));
	    return NULL;
	}
	TRACE_T1((stderr,
		  "T1 version of font %s not found, trying pixel version next, then fallback",
		  fontp->fontname));
    }
#endif /* T1LIB */
    
    
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
#if DELAYED_MKTEXPK
    if (load_font_now) {
	fprintf(stderr, "loading font now\n");
	if (message_font_creation(fontp->fontname, (int)(fontp->fsize + 0.5))) {
	    message_done = True;
	    name = kpse_find_glyph(fontp->fontname, (unsigned)(fontp->fsize + .5),
				   kpse_any_glyph_format, &file_ret);
	}
	else {
	    kpse_set_program_enabled(kpse_any_glyph_format, False, kpse_src_compile);
	    name = kpse_find_glyph(fontp->fontname, (unsigned)(fontp->fsize + .5),
				   kpse_any_glyph_format, &file_ret);
	    /* no success if either name is NULL or the filename returned in file_ret is
	       a different font */
#if 1
	    /* ??? Bug with tex/test2.tex if cmbr exists but cmr doesn't ??? */
	    fprintf(stderr, "creating %s\n", fontp->fontname);
	    if (!name || strcmp(file_ret.name, fontp->fontname) != 0) {
		statusline_info(STATUS_MEDIUM,
				 "Creating PK font: %s at %d dpi ...",
				 fontp->fontname,
				 (int)(fontp->fsize + 0.5));
		need_statusline_update = True;
		force_statusline_update();
		kpse_set_program_enabled(kpse_any_glyph_format, resource.makepk, kpse_src_compile);
		name = kpse_find_glyph(fontp->fontname, (unsigned)(fontp->fsize + .5),
				       kpse_any_glyph_format, &file_ret);
	    }
#endif
	}
    }
    else {
	name = kpse_find_glyph(fontp->fontname, (unsigned)(fontp->fsize + .5),
			       kpse_any_glyph_format, &file_ret);
    }
#else /* DELAYED_MKTEXPK */
    name = kpse_find_glyph(fontp->fontname, (unsigned)(fontp->fsize + .5),
			   kpse_any_glyph_format, &file_ret);
#endif /* DELAYED_MKTEXPK */
    
    if (name) { /* success */
#if DELAYED_MKTEXPK
	if (need_statusline_update) {
	    statusline_info(STATUS_SHORT, "Creating PK font: %s at %d dpi ... done",
			     fontp->fontname,
			     (int)(fontp->fsize + 0.5));
	    force_statusline_update();
	}
#endif
	*dpi_ret = file_ret.dpi;
	fontp->filename = name;
	*font_ret = file_ret.name;
	TRACE_T1((stderr, "Found pixel version: %s at %d dpi", file_ret.name, *dpi_ret));
#if DELAYED_MKTEXPK
	if (message_done) {
	    statusline_append(STATUS_VERYSHORT, "DUMMY", /* append text, don't overwrite */
			      "done.");
	    force_statusline_update();
	}
#endif
	return XFOPEN(name, FOPEN_R_MODE);
    }
#if DELAYED_MKTEXPK
    else if (!load_font_now) {
	add_missing_font(fontp->fontname, (int)(fontp->fsize + 0.5));
	return NULL;
    }
#endif
    else if (resource.alt_font != NULL) {
	/* The strange thing about kpse_find_glyph() is that it
	   won't create a PK version of alt_font if it doesn't
	   already exist. So we invoke it explicitly a second time
	   for that one.
	*/
	TRACE_T1((stderr, "Trying fallback"));
#ifdef T1LIB
	if (resource.t1lib
#if DELAYED_MKTEXPK
	    && load_font_now
#endif
	    ) {
	    /* Third attempt: T1 version of fallback font */
	    *t1id = find_T1_font(resource.alt_font);
	    if (*t1id >= 0) {
		TRACE_T1((stderr, "found fallback font for %s: %s", fontp->fontname, resource.alt_font));
		*font_ret = xstrdup(resource.alt_font);
		return NULL;
	    }
	    TRACE_T1((stderr,
		      "Type1 version of fallback font %s not found, trying pixel version",
		      resource.alt_font));
	}
#endif /* T1LIB */
	/* Forth attempt: PK version of fallback font */
	name = kpse_find_glyph(resource.alt_font, (unsigned)(fontp->fsize + .5),
			       kpse_any_glyph_format, &file_ret);
	if (name) { /* success */
	    TRACE_T1((stderr, "Success for PK version of fallback"));
	    *dpi_ret = file_ret.dpi;
	    fontp->filename = name;
	    *font_ret = xstrdup(resource.alt_font);
	    return XFOPEN(name, FOPEN_R_MODE);
	}
	else {
	    TRACE_T1((stderr, "Failure for PK version of fallback"));
	}
    }

    /* all other cases are failure */
    TRACE_T1((stderr, "Failure"));
    return NULL;
}
