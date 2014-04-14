/*
  font-open.c: find font files.  The routine font_open() itself bears
  no relation (except for the interface) to the original font_open().

  Copyright (c) 1999-2013  The texk project

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
#include "message-window.h"
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


	/* see just above the second init_t1_lookup for why this was chosen */
#define DVIPS_SETUP 1

#if FREETYPE || PS

/*
 *	The following code handles lookup of Type 1 fonts for use as FreeType
 *	fonts, and for use within MetaPost output.
 *	The system psfonts.map file is read into an AVL tree as needed (lazy
 *	evaluation), and searched for Type 1 fonts.
 */

struct p_list {					/* list of map file names */
	struct p_list	*next;
	const char	*value;
};

	/* Initialize this list to "psfonts.map".  */

static	struct p_list	psfonts_map		= {NULL, "psfonts.map"};

static	struct p_list	*p_head			= &psfonts_map;
static	struct p_list	**p_tail		= &psfonts_map.next;

static	FILE		*mapfile		= NULL;

static	char	*ffline	= NULL;		/* an array used to store */
					/* the file name being formed.  */
					/* It expands as needed. */
					/* Also used elsewhere.  */
static	size_t	ffline_len = 0;		/* current length of ffline[] */

/*
 *	Expand ffline[] to at least the given size.
 */

static void
expandline(int n)
{
	int	newlen	= n + 128;

	ffline = (ffline == NULL) ? xmalloc(newlen) : xrealloc(ffline, newlen);
	ffline_len = newlen;
}


/*
 *	Like fgets(), but read an arbitrarily long line into ffline[].
 */

static Boolean
fgets_long(FILE *f)
{
	int	len;

	if (fgets(ffline, ffline_len, f) == NULL)
	    return False;

	len = 0;
	for (;;) {
	    len += strlen(ffline + len);
	    if (len > 0 && ffline[len - 1] == '\n') {
		ffline[--len] = '\0';
		break;
	    }
	    if (len < ffline_len - 1)
		break;
	    expandline(len);
	    fgets(ffline + len, ffline_len - len, f);
	}

	return True;
}

# if DVIPS_SETUP

/*
 *	Get list of map files from dvips config file(s).
 */

static void
getdefaults(FILE *f)
{
	char		*p, *q;
	int		len;
	struct p_list	*p_node;

	while (fgets_long(f)) {
	    p = ffline;
	    while (*p == ' ' || *p == '\t') ++p;
	    if (*p == 'p') {
		do ++p;
		while (*p == ' ' || *p == '\t');

		if (*p == '+')
		    do ++p;
		    while (*p == ' ' || *p == '\t');
		else {	/* discard old list */
		    struct p_list *pl, *pl2;

		    *p_tail = NULL;
		    pl = p_head;
		    if (pl == &psfonts_map) pl = pl->next;
		    while (pl != NULL) {
			free((char *) pl->value);
			pl2 = pl->next;
			free(pl);
			pl = pl2;
		    }
		    p_tail = &p_head;
		}

		/* get white-delimited argument */
		len = strlen(p);
		q = memchr(p, ' ', len);
		if (q != NULL) len = q - p;
		q = memchr(p, '\t', len);
		if (q != NULL) len = q - p;
		p[len] = '\0';

		p_node = xmalloc(sizeof *p_node);
		p_node->value = xmemdup(p, len + 1);
		*p_tail = p_node;
		p_tail = &p_node->next;
	    }
	}

	fclose(f);
}

# endif /* DVIPS_SETUP */

/*
 *	Open the next (available) map file.
 */

static Boolean
open_next_mapfile(void)
{
	char	*filename;

	/*
	 * Look for the first (openable) map file.  Others will be read later
	 * as needed.
	 */

	while (p_head != NULL) {
	    filename = kpse_find_file(p_head->value, kpse_program_text_format,
	      True);
	    if (filename == NULL)
		filename = kpse_find_file(p_head->value, kpse_fontmap_format,
		  True);
	    if (filename == NULL) {
		XDVI_WARNING((stderr,
		  "could not find dvips map file %s; skipping\n",
		  p_head->value));
	    }
	    else {
		mapfile = XFOPEN(filename, OPEN_MODE);
		if (mapfile == NULL) {
		    XDVI_WARNING((stderr,
		      "could not open dvips map file %s: %s\n",
		      filename, strerror(errno)));
		    free(filename);
		}
		else {
		    TRACE_FT((stderr, "Map file: %s\n", filename));
		    free(filename);
		    return True;
		}
	    }
	    p_head = p_head->next;
	}

	return False;
}


/*
 *	Initialize these lookup routines.
 */

# if DVIPS_SETUP

Boolean
init_t1_lookup(void)
{
	char		*filename;
	struct stat	statbuf;
	FILE		*f;
	char		*dvipsrc;

	if (ffline == NULL) expandline(80);

	filename = find_file("config.ps", &statbuf, kpse_dvips_config_format);
	if (filename == NULL) {
	    TRACE_FT((stderr, "could not find file config.ps; skipping"));
	}
	else {
	    f = XFOPEN(filename, OPEN_MODE);
	    if (f == NULL) {
		TRACE_FT((stderr, "could not open file %s: %s",
		  filename, strerror(errno)));
	    }
	    else
		getdefaults(f);
	    free(filename);
	}

	dvipsrc = getenv("DVIPSRC");
	if (dvipsrc == NULL) {
	    dvipsrc = getenv("HOME");
	    if (dvipsrc != NULL) {
		size_t n;

		n = strlen(dvipsrc);
		if (n + 10 > ffline_len) expandline(n + 10);
		memcpy(ffline, dvipsrc, n);
		memcpy(ffline + n, "/.dvipsrc", 10);
		dvipsrc = ffline;
	    }
	}
	if (dvipsrc != NULL) {
	    f = XFOPEN(dvipsrc, OPEN_MODE);
	    if (f == NULL) {
		TRACE_FT((stderr,
		  "could not open dvipsrc file %s: %s; skipping",
		  dvipsrc, strerror(errno)));
	    }
	    else
		getdefaults(f);
	}

	filename = find_file("config.xdvi", &statbuf, kpse_dvips_config_format);
	if (filename == NULL) {
	    TRACE_FT((stderr, "could not find file config.xdvi; skipping"));
	}
	else {
	    f = XFOPEN(filename, OPEN_MODE);
	    if (f == NULL) {
		TRACE_FT((stderr, "could not open file %s: %s",
		  filename, strerror(errno)));
	    }
	    else
		getdefaults(f);
	    free(filename);
	}

	*p_tail = NULL;

	return open_next_mapfile();
}

# else /* !DVIPS_SETUP */

/*
 *	Under the earlier regime, T1 lookup was controlled by xdvi.cfg.
 *
 *	This has been disabled, for the following reasons.
 *
 *	It only supported "enc" and "dvipsmap" directives.
 *
 *	The "enc" directives selected implicit encoding files for fonts ending
 *	in certain character strings (8r, 8c, 8y), unless the map file
 *	explicitly referenced a .enc file.  However, among the dvips map files
 *	supplied with TeX Live 2012 (at least) there were no such fonts ending
 *	in 8r and 8y, and the encoding file for the 8c fonts was cork.enc,
 *	which is no longer included in TeX Live.
 *
 *	As for the dvipsmap directives, it seemed better to use the dvips
 *	configuration method.
 */

Boolean
init_t1_lookup(void)
{
	char *filename;
	FILE *fp;
	char *keyword;
	char *ptr;
	struct p_list *p_node;
	char *enc;
	char *name;
	static const char delim[] = "\t \n\r";

	if (ffline == NULL) expandline(80);

	filename = kpse_find_file("xdvi.cfg", kpse_program_text_format, 1);
	if (filename == NULL) {
	    statusline_error(STATUS_MEDIUM,
	      "Warning: Unable to find \"xdvi.cfg\"!");
	    return open_next_mapfile();
	}

	if ((fp = XFOPEN(filename, "r")) == NULL) {
	    XDVI_ERROR((stderr, "Cannot open config file `%s' for reading: %s",
	      filename, strerror(errno)));
	    free(filename);
	    return open_next_mapfile();
	}

	TRACE_FT((stderr, "Reading cfg file %s", filename));

	while (fgets_long(fp)) {
	    keyword = ffline;

	    /* Skip leading whitespace */
	    while (*keyword == ' ' || *keyword == '\t')
		keyword++;

	    /* % in first column is a correct comment */
	    if (*keyword == '%' || *keyword == '\0' || *keyword == '\n')
		continue;

	    keyword = strtok(keyword, delim);

	    if (strcmp(keyword, "dvipsmap") == 0) {
		if ((ptr = strtok(NULL, delim)) == NULL) {
		    XDVI_WARNING((stderr, "Syntax error in entry \"%s\"",
		      ffline));
		    continue;
		}
		TRACE_FT((stderr, "DVIPSMAP: '%s'", ptr));

		/* Add it to the list for later reading */
		p_node = xmalloc(sizeof *p_node);
		p_node->value = xmemdup(ptr, strlen(ptr) + 1);
		/* If we're still pointing to the default list, remove it */
		if (p_tail == &psfonts_map.next)
		    p_tail = &p_head;
		*p_tail = p_node;
		p_tail = &p_node->next;
	    }
	    else if (strcmp(keyword, "encmap") == 0) {
		popup_message(globals.widgets.top_level,
			  MSG_ERR,
			  "Your xdvi.cfg file is for a previous version "
			  "of xdvik. Please replace it with the xdvi.cfg file "
			  "in the current xdvik distribution.",
			  "The keyword \"encmap\" in xdvi.cfg is no longer "
			  "supported.  Please update the config file %s.",
			  filename);
	    }
	    else if (strcmp(keyword, "enc") == 0) {
		enc = strtok(NULL, delim);
		name = strtok(NULL, delim);
		if ((ptr = strtok(NULL, delim)) == NULL) {
		    XDVI_WARNING((stderr,
			    "Syntax error in entry \"%s\" (skipping line)",
			    ffline));
		    continue;
		}
#  if 0
		i = new_encoding(enc, ptr);
		TRACE_FT((stderr, "Encoding[%d]: '%s' = '%s' -> '%s'",
		  i, enc, name, ptr));
#  endif
	    } else {
		/* again, nag them with a popup so that they'll do something
		   about this ... */
		popup_message(globals.widgets.top_level,
			  MSG_ERR,
			  "Please check the syntax of your config file.  "
			  "Valid keywords are: \"enc\" and \"dvipsmap\".",
			  "Skipping unknown keyword \"%s\" in config file %s.",
			  keyword, filename);
	    }
	}
	*p_tail = NULL;	/* terminate linked list of map files */

	fclose(fp);
	free(filename);

	return open_next_mapfile();
}

# endif /* not DVIPS_SETUP */


/*
 *	Information about Type 1 fonts is stored in an AVL tree.
 */

static	struct avl_t1	*t1_head	= NULL;


/*
 *	Parse line from psfonts.map file.
 */

static struct avl_t1 *
dvips_parse(const char *line)
{
	const char	*w1p, *encp, *pfp, *qp;
	size_t		w1l, encl, pfl, ql;
	const char	*w2p;
	size_t		w2l;
	const char	*p, *p0;
	const char	*err;
	struct avl_t1	*t1p;
	char		*q;

	w2l = w1l = encl = pfl = ql = 0;
	err = NULL;
	p = line;
	for (;;) {	/* loop over words */
	    while (*p == ' ' || *p == '\t') ++p;
	    if (*p == '\0')
		break;

	    if (*p == '"') {	/* quoted string */
		const char *p_end;

		p0 = p + 1;
		p_end = p0 + strlen(p0);
		p = memchr(p0, '"', p_end - p0);
		if (p == NULL) p = p_end;
		qp = (ql == 0 ? p0 : NULL);
		ql += p - p0 + 1;
		if (*p == '"') ++p;
		continue;
	    }

	    if (*p == '<') {	/* encoding or pfa/b file */
		int	wtype	= 0;

		++p;
		if (*p == '<') {
		    wtype = 1;	/* font file */
		    ++p;
		}
		else if (*p == '[') {
		    wtype = -1;	/* encoding file */
		    ++p;
		}

		/* find word */
		while (*p == ' ' || *p == '\t') ++p;
		p0 = p;
		while (*p != '\0' && *p != ' ' && *p != '\t') ++p;

		if (wtype == 0 && p > p0 + 4 && p[-4] == '.') {
		    if (memcmp(p - 3, "enc", 3) == 0
		      || memcmp(p - 3, "ENC", 3) == 0)
			wtype = -1;
		    else if (memcmp(p - 3, "pfa", 3) == 0
		      || memcmp(p - 3, "pfb", 3) == 0
		      || memcmp(p - 3, "PFA", 3) == 0
		      || memcmp(p - 3, "PFB", 3) == 0)
			wtype = 1;
		}

		if (wtype > 0) {
		    if (pfl != 0)
			err = "more than one font file given";
		    else {
			pfp = p0;
			pfl = p - p0 + 1;
		    }
		}
		else if (wtype < 0) {
		    if (encl != 0)
			err = "more than one encoding file given";
		    else {
			encp = p0;
			encl = p - p0 + 1;
		    }
		}
		else
		    err = "cannot identify file type";
	    }
	    else {	/* if ordinary word */
		p0 = p;
		while (*p != '\0' && *p != ' ' && *p != '\t') ++p;
		if (w1l == 0) {
		    w1p = p0;
		    w1l = p - p0;
		}
		else if (w2l == 0) {
		    w2p = p0;
		    w2l = p - p0;
		}
		else
		    err = "more than two non-download words given";
	    }
	}	/* end loop over words */

	if (w1l == 0) {
	    TRACE_FT((stderr,
	      "map file %s: line \"%s\" does not give a font name.",
	      p_head->value, line));
	    return NULL;
	}

	if (err != NULL) {
	    TRACE_FT((stderr, "map file %s, font %.*s: %s", p_head->value,
	      (int) w1l, w1p, err));
	    return NULL;
	}

	t1p = (struct avl_t1 *) avladd(w1p, w1l, (struct avl **) &t1_head,
	  sizeof(struct avl_t1));

	if (t1p->key != w1p) {	/* if existing record */
	    TRACE_FT((stderr,
	      "map file %s, font %.*s: duplicate record; using first one",
	      p_head->value, (int) w1l, w1p));
	    return NULL;
	}

	t1p->key = q = xmalloc(w1l + w2l + 1 + pfl + encl + ql);

	memcpy(q, w1p, w1l);
	q += w1l;
	t1p->psname = t1p->key;
	if (w2l != 0) {
	    t1p->psname = q;
	    memcpy(q, w2p, w2l);
	    q += w2l;
	}
	*q++ = '\0';

	t1p->fontfile = NULL;
	if (pfl != 0) {
	    t1p->fontfile = q;
	    memcpy(q, pfp, pfl - 1);
	    q += pfl;
	    q[-1] = '\0';
	}

	t1p->encname = t1p->addinfo = NULL;
# if FREETYPE
	t1p->bad = False;
	t1p->ft = NULL;
# endif

	if (encl != 0) {
	    t1p->encname = q;
	    memcpy(q, encp, encl - 1);
	    q += encl;
	    q[-1] = '\0';
	}

	if (ql != 0) {
	    t1p->addinfo = q;
	    if (qp != 0) {
		memcpy(q, qp, ql - 1);
		q += ql;
	    }
	    else {	/* multiple quoted strings; rescan to get them */
		const char	*p_end;

		p = line;
		p_end = p + strlen(p);
		for (;;) {
		    while (*p == ' ' || *p == '\t') ++p;
		    if (*p == '\0')
			break;

		    /* found a word */
		    if (*p == '"') {
			++p;
			p0 = p;
			p = memchr(p0, '"', p_end - p0);
			if (p == NULL) p = p_end;
			memcpy(q, p0, p - p0);
			q += p - p0;
			*q++ = ' ';
			if (*p == '\0') break;
			++p;
		    }
		    else	/* skip unquoted word */
			while (*p != '\0' && *p != ' ' && *p != '\t') ++p;
		}
	    }
	    q[-1] = '\0';
	}

	return t1p;
}

/*
 *	Information on the Ghostscript font aliasing mechanism is kept in
 *	another AVL tree.  Each identifier points to a list of strings,
 *	up to one from each Fontmap file.  Within a Fontmap file, later entries
 *	for a given string override earlier ones.
 */

struct avl_gs {		/* structure for gs font information */
	AVL_COMMON;
	short		fontmap_number;	/* sequence of most recent entry */
	Boolean		in_use;
	struct gs_list	*list;
};

struct gs_list {	/* list of Fontmap entries for this font name */
	struct gs_list	*next;
	const char	value[0];
};

static	struct avl_gs	*gs_head	= NULL;

typedef	void	(*gs_path_proc)(FILE *);

static	const char	*env_gs_lib;

static FILE *
gs_try_fopen(const char *str, unsigned int len, const char *name)
{
	unsigned int	namelen	= strlen(name) + 1;
	FILE		*f;

	if (len + namelen + 1 > ffline_len) expandline(len + namelen + 1);
	memcpy(ffline, str, len);
	ffline[len] = '/';
	memcpy(ffline + len + 1, name, namelen);

	f = XFOPEN(ffline, OPEN_MODE);

	TRACE_FT((stderr, "gs_try_fopen: %s: %s", ffline,
	      f != NULL ? "file opened" : strerror(errno)));

	return f;
}

static FILE *
gs_path_fopen(const char *name, gs_path_proc proc)
{
	const	char	*str1	= env_gs_lib;
	const	char	*str2	= DEFAULT_GS_LIB_PATH;
	const	char	*str1_end, *str2_end;
	const char	*p1, *p2;
	FILE		*f;
	unsigned int	namelen;

	if (str1 == NULL) {
	    str1 = str2;
	    str2 = NULL;
	}

	str1_end = str1 + strlen(str1);

	for (;;) {
	    p1 = memchr(str1, ':', str1_end - str1);
	    if (p1 == NULL) p1 = str1_end;
	    if (p1 == str1) {
		if (str2 != NULL) {
		    str2_end = str2 + strlen(str2);
		    for (;;) {
			p2 = memchr(str2, ':', str2_end - str2);
			if (p2 == NULL) p2 = str2_end;
			if (p2 > str2) {
			    f = gs_try_fopen(str2, p2 - str2, name);
			    if (f != NULL) {
				if (proc != NULL)
				    proc(f);
				else
				    return f;
			    }
			}
			if (*p2 == '\0')
			    break;
			str2 = p2 + 1;
		    }
		    str2 = NULL;
		}
	    }
	    else {
		f = gs_try_fopen(str1, p1 - str1, name);
		if (f != NULL) {
		    if (proc != NULL)
			proc(f);
		    else
			return f;
		}
	    }

	    if (*p1 == '\0')
		break;
	    str1 = p1 + 1;
	}

	/* leave the file name in ffline[] for error message */
	namelen = strlen(name) + 1;
	if (namelen > ffline_len) expandline(namelen);
	memcpy(ffline, name, namelen);

	return NULL;
}


static FILE *
lookup_gs_font(const char *font, const char **path_ret)
{
	struct avl_gs	*gsfp;
	int		font_len;
	int		i;

	font_len = strlen(font);
	gsfp = gs_head;
	for (;;) {
	    if (gsfp == NULL)		/* if not found */
		return NULL;

	    i = font_len - gsfp->key_len;
	    if (i == 0)
		i = memcmp(font, gsfp->key, font_len);
	    if (i == 0) {		/* if found */
		struct gs_list	*gl, *gl2, *gl3;
		FILE		*f;

		if (gsfp->in_use) {
		    TRACE_FT((stderr, "Alias loop for %s detected; ignoring.",
		      font));
		    return NULL;
		}

		/* If we haven't done it yet, reverse the linked list */
		if (gsfp->fontmap_number != 0) {
		    gsfp->fontmap_number = 0;
		    gl = gsfp->list;
		    gl2 = NULL;
		    while (gl != NULL) {
			gl3 = gl->next;
			gl->next = gl2;
			gl2 = gl;
			gl = gl3;
		    }
		    gsfp->list = gl2;
		}
		gsfp->in_use = True;
		f = NULL;
		for (gl = gsfp->list; gl != NULL; gl = gl->next) {
		    if (gl->value[0] == '\0') {	/* if alias */
			TRACE_FT((stderr, "Found alias %s --> %s",
			  font, gl->value + 1));
			f = lookup_gs_font(gl->value + 1, path_ret);
			if (f == NULL)
			    TRACE_FT((stderr, "Alias %s not found.",
			      gl->value + 1));
			if (f != NULL) break;
		    }
		    else {
			TRACE_FT((stderr, "Checking file %s", gl->value));
			if (gl->value[0] == '/' || (gl->value[0] == '.'
			  && (gl->value[1] == '/' || (gl->value[1] == '.'
			  && gl->value[2] == '/')))) {
			    f = XFOPEN(gl->value, OPEN_MODE);
			    if (f != NULL) {
				*path_ret = xstrdup(gl->value);
				break;
			    }
			}
			else {
			    f = gs_path_fopen(gl->value, NULL);
			    if (f != NULL) {
				*path_ret = xstrdup(ffline);
				break;
			    }
			}
		    }
		}
		gsfp->in_use = False;
		return f;
	    }
	    gsfp = (struct avl_gs *) (i < 0 ? gsfp->left : gsfp->right);
	}
}


#define	GS_BUF_SIZE	4096

struct gsfile {
	FILE			*f;
	unsigned char		*buffer;
	const unsigned char	*bufpos;
	const unsigned char	*buf_end;
};

static Boolean
gs_fillbuf(struct gsfile *gsf)
{
	unsigned char	*p;
	unsigned int	len;

	if (gsf->buf_end < gsf->buffer + GS_BUF_SIZE)
	    return False;

	gsf->bufpos = p = gsf->buffer;
	for (;;) {
	    len = gsf->buf_end - p;
	    if (len <= 0) break;
	    len = fread(p, 1, len, gsf->f);
	    if (len <= 0) break;
	    p += len;
	}
	gsf->buf_end = p;
	return (p > gsf->buffer);
}

static	unsigned char	gs_ctype[256];

#define	GS_EOF	'\0'
#define	GS_ERR	'%'
#define	LPAREN	'('
#define	RPAREN	')'

static void
init_gs_ctype(void)
{
	const char	*p;

	for (p = " \t\f\n\r";; ++p) {
	    gs_ctype[(unsigned char) *p] = 1;	/* white space */
	    if (*p == '\0') break;
	}
	gs_ctype['/'] = 2;		/* literal token */
	gs_ctype['('] = 3;		/* string */
	for (p = ")<>[]{}%"; *p != '\0'; ++p)
	    gs_ctype[(unsigned char) *p] = 4;	/* delimiter */
}

static unsigned char
get_gs_character(struct gsfile *gsfp)
{
	unsigned char		c;

	for (;;) {
	    if (gsfp->bufpos >= gsfp->buf_end && !gs_fillbuf(gsfp))
		return '%';
	    c = *gsfp->bufpos++;

	    /* Check for comments */
	    if (c == '%') {
		for (;;) {
		    const unsigned char *p1, *p2;

		    p1 = memchr(gsfp->bufpos, '\n',
		      gsfp->buf_end - gsfp->bufpos);
		    if (p1 == NULL) p1 = gsfp->buf_end;
		    p2 = memchr(gsfp->bufpos, '\r', p1 - gsfp->bufpos);
		    if (p2 != NULL) p1 = p2;
		    p2 = memchr(gsfp->bufpos, '\f', p1 - gsfp->bufpos);
		    if (p2 != NULL) p1 = p2;
		    if (p1 < gsfp->buf_end) {
			gsfp->bufpos = p1 + 1;
			break;
		    }
		    if (!gs_fillbuf(gsfp))
			return '%';
		}
		continue;
	    }

	    if (gs_ctype[c] != 1)
		break;
	}

	return c;
}

static unsigned char
get_gs_token(struct gsfile *gsfp,
	unsigned int pos,
	unsigned int *pos_ret,
	const char *file_type)
{
	unsigned char		gs_t_type;
	unsigned char		c;
	const unsigned char	*p0;
	unsigned int		pos0;
	unsigned int		depth;

	gs_t_type = c = get_gs_character(gsfp);
	if (c == '%')
	    return GS_EOF;

	p0 = gsfp->bufpos;
	switch (gs_ctype[c]) {
	    case 0:	/* most characters */
	    case 2:	/* '/' */
		--p0;	/* retain initial character */
		pos0 = pos;
		for (;;) {
		    if (gsfp->bufpos >= gsfp->buf_end) {
			unsigned int	len	= gsfp->bufpos - p0;

			if (pos + len >= ffline_len) expandline(pos + len);
			memmove(ffline + pos, p0, len);
			pos += len;
			if (!gs_fillbuf(gsfp))
			    break;
			p0 = gsfp->buffer;
		    }
		    if (gs_ctype[*gsfp->bufpos] != 0) {
			unsigned int	len	= gsfp->bufpos - p0;

			if (pos + len >= ffline_len) expandline(pos + len);
			memmove(ffline + pos, p0, len);
			pos += len;
			break;
		    }
		    ++gsfp->bufpos;
		}
		/* Filter out DOS ^Z */
		if (pos == pos0 + 1 && ffline[pos0] == '\032')
		    return GS_EOF;
		break;

	    case 3:	/* left parenthesis */
		depth = 1;
		for (;;) {
		    const unsigned char *p1, *p2, *p3;

		    if (gsfp->bufpos >= gsfp->buf_end) {
			unsigned int	len	= gsfp->bufpos - p0;

			if (pos + len >= ffline_len) expandline(pos + len);
			memmove(ffline + pos, p0, len);
			pos += len;
			if (!gs_fillbuf(gsfp)) {
			    TRACE_FT((stderr,
			      "unterminated string in %s file; giving up.",
			      file_type));
			    return GS_ERR;
			}
			p0 = gsfp->buffer;
		    }
		    p1 = memchr(gsfp->bufpos, RPAREN,
		      gsfp->buf_end - gsfp->bufpos);
		    if (p1 == NULL) p1 = gsfp->buf_end;
		    for (;;) {
			p2 = memchr(gsfp->bufpos, LPAREN, p1 - gsfp->bufpos);
			if (p2 == NULL) p2 = p1;
			p3 = p2;
			for (;;) {
			    if (p3 <= gsfp->bufpos) {
				if (c == '\\') --p3;
				break;
			    }
			    if (p3[-1] != '\\') break;
			    --p3;
			}
			c = '\\' - 1 + ((p2 - p3) & 1);
			if (p2 >= p1)
			    break;
			if (c != '\\')
			    ++depth;
			gsfp->bufpos = p2 + 1;
			c = '\0';
		    }
		    if (p1 < gsfp->buf_end) {	/* if left parenthesis at p1 */
			if (c != '\\') {
			    if (--depth == 0) {
				unsigned int	len	= p1 - p0;

				if (pos + len >= ffline_len)
				    expandline(pos + len);
				memmove(ffline + pos, p0, len);
				pos += len;
				gsfp->bufpos = p1 + 1;
				break;
			    }
			}
			++p1;
		    }
		    gsfp->bufpos = p1;
		}
		/* We could do backslash escaping here, but it's probably
		   unnecessary.  */
		break;

	    default:
		TRACE_FT((stderr,
		  "invalid character `%c' encountered in %s file; giving up.",
		  c, file_type));
		return GS_ERR;
	}

	*pos_ret = pos;
	return gs_t_type;
}


static	short		gs_fontmap_number	= 0;

static void
process_gs_fontmap(FILE *f)
{
	struct gsfile	gsf;
	unsigned char	buffer[GS_BUF_SIZE];
	unsigned char	ttype;
	unsigned int	pos1, pos2, pos3;

	++gs_fontmap_number;

	gsf.f = f;
	gsf.buffer = buffer;
	gsf.bufpos = gsf.buf_end = buffer + GS_BUF_SIZE;

	/*
	 * Allow entries of the following types:
	 *
	 *	(string) .runlibfile
	 *	(string) .runlibfileifexists
	 *	/identifier (string) ;
	 *	/identifier /alias ;
	 */

	for (;;) {
	    ttype = get_gs_token(&gsf, 0, &pos1, "Fontmap");
	    if (ttype == GS_EOF || ttype == GS_ERR)
		break;
	    if (ttype == LPAREN) {
		Boolean	quiet = False;
		FILE	*f1;

		ttype = get_gs_token(&gsf, pos1, &pos2, "Fontmap");
		if (ttype == GS_ERR)
		    break;
		if (ttype == GS_EOF) {
		    TRACE_FT((stderr,
		      "unexpected end of Fontmap file; giving up."));
		    break;
		}
		if (ttype == '.' && pos2 - pos1 == 19
		  && memcmp(ffline + pos1, ".runlibfileifexists", 19) == 0)
		    quiet = True;
		else if (ttype != '.' || pos2 - pos1 != 11
		  || memcmp(ffline + pos1, ".runlibfile", 11) != 0) {
		    TRACE_FT((stderr, "invalid token following \"(%.*s)\" in Fontmap file; giving up.",
		      (int) pos1, ffline));
		    break;
		}

		ffline[pos1] = '\0';
		if (ffline[0] == '/' || (ffline[0] == '.' && (ffline[1] == '/'
		  || (ffline[1] == '.' && ffline[2] == '/'))))
		    f1 = XFOPEN(ffline, OPEN_MODE);
		else {
		    char *q;

		    q = xmemdup(ffline, pos1 + 1);
		    f1 = gs_path_fopen(q, NULL);
		    free(q);
		}

		if (f1 == NULL) {
		    if (!quiet)
			XDVI_WARNING((stderr, "Fontmap .runlibfile: %s: %s",
			  ffline, strerror(errno)));
		    else
			TRACE_FT((stderr,
			  "Fontmap .runlibfileifexists: %s: %s\n",
			  ffline, strerror(errno)));
		}
		else {
		    --gs_fontmap_number;
		    process_gs_fontmap(f1);
		}
	    }
	    else if (ttype == '/') {
		struct avl_gs	*gsfp;
		struct gs_list	*gslp;

		ttype = get_gs_token(&gsf, pos1, &pos2, "Fontmap");
		if (ttype == GS_ERR)
		    break;
		if (ttype == GS_EOF) {
		    TRACE_FT((stderr,
		      "unexpected end of Fontmap file; giving up."));
		    break;
		}
		if ((ttype != '/' && ttype != LPAREN)
		  || pos2 == pos1	/* empty string would mess things up */
		  || get_gs_token(&gsf, pos2, &pos3, "Fontmap") != ';'
		  || pos3 != pos2 + 1) {
		    TRACE_FT((stderr,
		      "invalid token following \"%.*s\" in Fontmap file; giving up.",
		      (int) pos1, ffline));
		    break;
		}
		if (ttype == '/')
		    ffline[pos1] = '\0';	/* mark aliases by initial \0 */
		ffline[pos2++] = '\0';		/* terminate string */

		/* Add to database */
		gsfp = (struct avl_gs *) avladd(ffline + 1, pos1 - 1,
		  (struct avl **) &gs_head, sizeof *gsfp);

		if (gsfp->key == ffline + 1) {	/* if new record */
		    gsfp->key = xmemdup(ffline + 1, pos1 - 1);
		    gsfp->in_use = False;
		    gsfp->list = NULL;
		}
		else {
		    if (strlen(gsfp->list->value + 1) + 2 == pos2 - pos1
		      && memcmp(gsfp->list->value, ffline + pos1, pos2 - pos1)
		      == 0)
			continue;	/* ignore duplicate entry */
		    if (gsfp->fontmap_number == gs_fontmap_number) {
			/* Later entries in a Fontmap file override earlier
			   ones */
			gslp = gsfp->list;
			gsfp->list = gslp->next;
			free(gslp);
		    }
		}
		gslp = xmalloc(sizeof *gslp + pos2 - pos1);
		gslp->next = gsfp->list;
		gsfp->list = gslp;
		memcpy((char *) gslp->value, ffline + pos1, pos2 - pos1);

		gsfp->fontmap_number = gs_fontmap_number;
	    }
	    else {
		TRACE_FT((stderr,
		  "invalid token \"%s\" in Fontmap file; giving up.",
		  ffline));
	    }
	}

	fclose(f);
}

/*
 *	Read Ghostscript Fontmap files.  These are used if the line in
 *	psfonts.map does not contain a filename for the font.
 *
 *	For example:
 *		n019003l NimbusSanL-Regu
 */

static void
read_gs_fontmaps(void)
{
	env_gs_lib = getenv("XDVI_GS_LIB");
	if (env_gs_lib == NULL)
	    env_gs_lib = getenv("GS_LIB");

	if (gs_ctype[0] == 0)
	    init_gs_ctype();

	(void) gs_path_fopen("Fontmap", process_gs_fontmap);
}


/*
 *	pre_lookup_t1_font - Find a Type 1 font (or return NULL).
 */

static struct avl_t1 *
pre_lookup_t1_font(const char *fontname)
{
	struct avl_t1	*t1p;
	size_t		len;
	int		i;

	/* first, search for the font */

	len = strlen(fontname);
	t1p = t1_head;
	while (t1p != NULL) {
	    i = len - t1p->key_len;
	    if (i == 0)
		i = memcmp(fontname, t1p->key, len);
	    if (i == 0)
		return t1p;	/* found it */
	    t1p = (struct avl_t1 *) (i < 0 ? t1p->left : t1p->right);
	}

	/* next, read in more records in hopes of finding the font */

	if (p_head != NULL)
	    for (;;) {
		if (!fgets_long(mapfile)) {	/* if end of file */
		    fclose(mapfile);
		    p_head = p_head->next;
		    if (!open_next_mapfile())
			return NULL;
		    continue;
		}

		if (*ffline < ' ' || *ffline == '*' || *ffline == '#'
		  || *ffline == ';' || *ffline == '%')
		    continue;

		t1p = dvips_parse(ffline);

		if (t1p != NULL && t1p->key_len == len
		  && memcmp(t1p->key, fontname, len) == 0)
		    return t1p;	/* found it */
	    }

	return NULL;
}

/*
 *	lookup_t1_font - Find a Type 1 font (or return NULL).
 */

Boolean
lookup_t1_font(struct font *fontp,
	const char *fontname)
{
	struct avl_t1	*t1p;
	struct ftfont	*ftp;

	t1p = pre_lookup_t1_font(fontname);

	if (t1p == NULL)
	    return False;

	if (t1p->bad) {
	    TRACE_FT((stderr,
	      "Font %s is marked as bad:  skipping scalable version",
	      fontname));
	    return False;
	}

	ftp = t1p->ft;
	if (ftp != NULL) {	/* if it's is already in use at another size */
	    struct font *first_size;

	    /* The first node in the linked list of sizes contains the file */
	    /* reference, so we link in the new node after the first node */
	    first_size = ftp->first_size;
	    fontp->next_size = first_size->next_size;
	    first_size->next_size = fontp;
	}
	else {	/* first use at this size */
	    t1p->ft = ftp = xmalloc(sizeof *ftp);
	    ftp->face = NULL;
	    ftp->t1 = t1p;
	    ftp->first_size = fontp;
	    fontp->next_size = NULL;
	}
	fontp->ft = ftp;
	fontp->size = NULL;

	return True;
}

FILE *
open_t1_font(struct avl_t1 *t1p,
	const char **path_ret)
{
	FILE		*f;

	if (t1p->fontfile == NULL) {	/* look up in GS Fontmap */
	    static Boolean gs_fontmap_initialized = False;

	    if (!gs_fontmap_initialized) {
		read_gs_fontmaps();
		gs_fontmap_initialized = True;
	    }

	    TRACE_FT((stderr,
	      "Looking for font %.*s using gs method (PS name %s) --",
	      t1p->key_len, t1p->key, t1p->psname));
	    f = lookup_gs_font(t1p->psname, path_ret);

	    if (f == NULL) {
		TRACE_FT((stderr, "cannot find Type 1 font %s",
		  t1p->psname));
		return NULL;
	    }

	    TRACE_FT((stderr, "Found file %s", *path_ret));
	}
	else {
	    char	*filename;

	    filename = kpse_find_file(t1p->fontfile, kpse_type1_format, 0);
	    if (filename == NULL) {
		TRACE_FT((stderr, "cannot find Type 1 font file %s "
		  "(will try PK version instead).",
		  t1p->fontfile));
		return NULL;
	    }

	    f = XFOPEN(filename, OPEN_MODE);
	    if (f == NULL) {
		TRACE_FT((stderr, "cannot open Type 1 font file %s: %s",
		  filename, strerror(errno)));
		free(filename);
		return NULL;
	    }
	    *path_ret = filename;
	}

	return f;
}


/*
 *	Read the encoding vector file.  This assumes the same format as afm2tfm.
 */

extern	struct findrec	search_header;	/* from special.c */

void
read_encoding(struct avl_enc *encp)
{
	char		*filename;
	FILE		*f;
	struct gsfile	gsf;
	unsigned char	buffer[GS_BUF_SIZE];
	jmp_buf		err_env;
	unsigned char	ttype;
	unsigned int	pos1, pos2;
	unsigned int	identindex[256];
	const char	*str;
	unsigned int	i;

	TRACE_FT((stderr, "Reading encoding file %s", encp->key));

	encp->valid = False;

	/*
	 * With TDS 1.0/kpathsea 3.5.2(?), encoding files are in texmf/fonts/enc
	 * and accessed via kpse_enc_format; see e.g.:
	 * http://tug.org/mailman/htdig/tex-live/2004-January/004734.html
	 *
	 * The lookups under kpse_program_text_format and
	 * kpse_tex_ps_header_format are kept for backwards compatibility.
	 */

	filename = kpse_find_file(encp->key, kpse_enc_format, 0);
	if (filename == NULL) {
	    filename = kpse_find_file(filename, kpse_program_text_format, 0);
	    if (filename == NULL)
		filename = kpse_find_file(filename, kpse_tex_ps_header_format,
		  True);
	}
	if (filename == NULL) {
	    TRACE_FT((stderr,
	      "cannot find encoding file %s; ignoring encoding", encp->key));
	    return;
	}

	f = XFOPEN(filename, OPEN_MODE);
	if (f == NULL) {
	    TRACE_FT((stderr, "cannot open encoding file %s: %s",
	      filename, strerror(errno)));
	    free(filename);
	    return;
	}
	free(filename);

	if (gs_ctype[0] == 0)
	    init_gs_ctype();

	gsf.f = f;
	gsf.buffer = buffer;
	gsf.bufpos = gsf.buf_end = buffer + GS_BUF_SIZE;

	if (!setjmp(err_env)) {
	    if (get_gs_token(&gsf, 0, &pos1, "encoding") != '/'
	      || get_gs_character(&gsf) != '[')
		longjmp(err_env, 1);

	    pos1 = 0;
	    for (i = 0; i < 256; ++i) {
		if (get_gs_token(&gsf, pos1, &pos2, "encoding") != '/')
		    longjmp(err_env, 1);
		if (pos2 == pos1 + 8
		  && memcmp(ffline + pos1, "/.notdef", 8) == 0)
		    identindex[i] = 0;
		else {
		    ffline[pos1] = '\0';
		    identindex[i] = pos1 + 1;
		    pos1 = pos2;
		}
	    }

	    if (get_gs_character(&gsf) != ']')
		longjmp(err_env, 1);

	    ttype = get_gs_token(&gsf, pos1, &pos2, "encoding");
	    if (!(ttype == GS_EOF
	      || (ttype == 'd' && pos2 == pos1 + 3
	      && memcmp(ffline + pos1, "def", 3) == 0
	      && get_gs_token(&gsf, pos2, &pos2, "encoding") == GS_EOF)))
		longjmp(err_env, 1);

	    if (pos1 >= ffline_len) expandline(pos1 + 1);
	    ffline[pos1] = '\0';
	    str = xmemdup(ffline + 1, pos1);
	    for (i = 0; i < 256; ++i)
		encp->vec[i] =
		  (identindex[i] != 0 ? str + identindex[i] - 1 : NULL);

	    encp->valid = True;
	}
	else	/* if error */
	    TRACE_FT((stderr,
	      "invalid format in encoding file %s; giving up.", encp->key));

	fclose(f);
}

#endif /* FREETYPE || PS */


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
	  int *dpi_ret)
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
    
    if (name) { /* found a vf font */
	/* pretend it has the expected dpi value, else caller will complain */
 	*dpi_ret = fontp->fsize;
	fontp->filename = name;
	return XFOPEN(name, FOPEN_R_MODE);
    }

#if FREETYPE
    if (resource.freetype
# if DELAYED_MKTEXPK
	&& load_font_now
# endif
	) {
	/* First attempt: freetype font of correct size
	 * (for delayed_mtkexpk, only when scanning postamble for the first time)
	 */
	if (lookup_t1_font(fontp, fontp->fontname)) {
	    TRACE_FT((stderr, "found freetype font %s", fontp->fontname));
	    return NULL;
	}
	TRACE_FT((stderr,
		  "Freetype version of font %s not found, trying pixel version next, then fallback",
		  fontp->fontname));
    }
#endif /* FREETYPE */


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
	TRACE_FT((stderr, "Found pixel version: %s at %d dpi", file_ret.name, *dpi_ret));
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
	TRACE_FT((stderr, "Trying fallback"));
#if FREETYPE
	if (resource.freetype
#if DELAYED_MKTEXPK
	    && load_font_now
#endif
	    ) {
	    /* Third attempt: freetype version of fallback font */
	    if (lookup_t1_font(fontp, resource.alt_font)) {
		TRACE_FT((stderr, "found fallback font for %s: %s",
			fontp->fontname, resource.alt_font));
		*font_ret = xstrdup(resource.alt_font);
		return NULL;
	    }
	    TRACE_FT((stderr,
		      "Freetype version of fallback font %s not found, trying pixel version",
		      resource.alt_font));
	}
#endif /* FREETYPE */
	/* Fourth attempt: PK version of fallback font */
	name = kpse_find_glyph(resource.alt_font, (unsigned)(fontp->fsize + .5),
			       kpse_any_glyph_format, &file_ret);
	if (name) { /* success */
	    TRACE_FT((stderr, "Success for PK version of fallback"));
	    *dpi_ret = file_ret.dpi;
	    fontp->filename = name;
	    *font_ret = xstrdup(resource.alt_font);
	    return XFOPEN(name, FOPEN_R_MODE);
	}
	else {
	    TRACE_FT((stderr, "Failure for PK version of fallback"));
	}
    }

    /* all other cases are failure */
    TRACE_FT((stderr, "Failure"));
    return NULL;
}
