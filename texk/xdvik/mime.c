/* Copyright (c) 1994-1999  All rights reserved. */
/* Written from scratch July 30, 1994, by A. P. Smith. */

/*  Copyright (c) 2003-2004 the xdvik development team */

/*
  Read mime.types and mailcap entries to check what
  viewers to invoke on a new file.

  Patch by Allin Cottrell (cottrell@ricardo.ecn.wfu.edu) to
  invokeviewer applied 30/11/98.

  Patched further in january 1999 by Nicolai Langfeldt
  (janl@math.uio.no) to allow saner mime typing.

  Rewritten by Stefan Ulrich <stefanulrich@users.sourceforge.net>
  on 2003/03/25 for better RFC 1343 conformance.
  
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
#include "hypertex.h"
#include "xdvi-debug.h"
#include "util.h"
#include "string-utils.h"
#include "mime.h"
#include "browser.h"
#include "message-window.h"

#include <ctype.h>
#include "kpathsea/c-fopen.h"
#include "kpathsea/variable.h"
#include "kpathsea/c-pathmx.h"

/* default settings if $EXTENSIONMAPS and $MAILCAPS is not set */

static const char *const DEFAULT_EXTENSIONMAPS =
"$HOME/.mime.types:/etc/mime.types:/usr/etc/mime.types:/usr/local/etc/mime.types";

static const char *const DEFAULT_MAILCAPS =
"$HOME/.mailcap:/etc/mailcap:/usr/etc/mailcap:/usr/local/etc/mailcap";

struct mime_map {
    char *content_type;
    char *extensions;
};

static struct mime_map default_mimemap[] = {
    {"application/postscript", "ai eps epsf ps",},
    {"application/octet-stream", "bin",},
    {"application/oda", "oda",},
    {"application/pdf", "pdf",},
    {"application/rtf", "rtf",},
    {"application/x-mif", "mif",},
    {"application/x-csh", "csh",},
    {"application/x-dvi", "dvi Dvi DVI",},
    {"application/x-hdf", "hdf",},
    {"application/x-latex", "latex",},
    {"application/x-netcdf", "nc cdf",},
    {"application/x-sh", "sh",},
    {"application/x-tcl", "tcl",},
    {"application/x-tex", "tex",},
    {"application/x-texinfo", "texinfo texi",},
    {"application/x-troff", "t tr roff",},
    {"application/x-troff-man", "man",},
    {"application/x-troff-me", "me",},
    {"application/x-troff-ms", "ms",},
    {"application/x-wais-source", "src",},
    {"application/zip", "zip",},
    {"application/x-bcpio", "bcpio",},
    {"application/x-cpio", "cpio",},
    {"application/x-gtar", "gtar",},
    {"application/x-shar", "shar",},
    {"application/x-sv4cpio", "sv4cpio",},
    {"application/x-sv4crc", "sv4crc",},
    {"application/x-tar", "tar",},
    {"application/x-ustar", "ustar",},
    {"audio/basic", "au snd",},
    {"audio/x-aiff", "aif aiff aifc",},
    {"audio/x-wav", "wav",},
    {"image/gif", "gif",},
    {"image/ief", "ief",},
    {"image/jpeg", "jpeg jpg jpe",},
    {"image/tiff", "tiff tif",},
    {"image/x-cmu-raster", "ras",},
    {"image/x-portable-anymap", "pnm",},
    {"image/x-portable-bitmap", "pbm",},
    {"image/x-portable-graymap", "pgm",},
    {"image/x-portable-pixmap", "ppm",},
    {"image/x-rgb", "rgb",},
    {"image/x-xbitmap", "xbm",},
    {"image/x-xpixmap", "xpm",},
    {"image/x-xwindowdump", "xwd",},
    {"text/html", "html htm sht shtml",},
    {"text/plain", "txt",},
    {"text/richtext", "rtx",},
    {"text/tab-separated-values", "tsv",},
    {"text/x-setext", "etx",},
    {"video/mpeg", "mpeg mpg mpe",},
    {"video/quicktime", "qt mov",},
    {"video/x-msvideo", "avi",},
    {"video/x-sgi-movie", "movie",},
    {"application/gzip", "gz",},
    {"application/compress", "Z",},
    {"application/bzip", "bz",},
    {"application/bzip2", "bz2",},
};


struct mailcap_map {
    char *content_type;
    const char *command;	/* command string */
    const char *testcmd;	/* value of the `test=' field, or NULL if no test field present */
    Boolean needsterminal;	/* whether the `needsterminal' flag has been specified for this entry */
    const char *format_string;	/* format specifier ("%s" or "%u" or "") */
};

static struct mailcap_map default_mailcap[] = {
    {"audio/*", "showaudio %s", NULL, False, "%s"},
    {"image/*", "xv %s", NULL, False, "%s"},
    {"video/mpeg", "mpeg_play %s", NULL, False, "%s"},
    {"application/pdf", "acroread %s", NULL, False, "%s"},
    {"text/html", "netscape-raise  -remote 'openURL(%s,new-window)'", NULL, False, "%s"},
    {"application/postscript", "ghostview %s", NULL, False, "%s"},
    {"application/x-dvi", "xdvi %s", NULL, False, "%s"},
};

static struct mime_map *m_mimemap = NULL;
static int m_mimemap_currlen = 0;
static int m_mimemap_size = 0;

static struct mailcap_map *m_mailcap = NULL;
static int m_mailcap_currlen, m_mailcap_size = 0;

static const size_t MIME_ALLOC_STEP = 64;

/* Utility function:
   Read a line from a config file, dealing with continued lines
   (ending with '\[ \t]*').
   The *linebuf argument is re-alloced as needed from its initial size alloc_len.
   The length of the read line is returned as ret_len.
   The lineno int is by the number of lines read.
   Trailing whitespace and newline is removed, and either the line,
   or NULL for EOF is returned.
*/
static char *
read_config_line(const char *filename, FILE *fp, char **linebuf,
		 size_t alloc_len, size_t *ret_len,
		 int *lineno)
{
    Boolean read_complete_line = False;
    *ret_len = 0;
    while (!read_complete_line) {
	if ((fgets((*linebuf) + *ret_len, alloc_len - *ret_len, fp)) == NULL) {
	    if (*ret_len == 0) /* at EOF */
		return NULL;
	    else /* nothing read, but we have a result from previous loop iteration */
		return *linebuf;
	}
	(*lineno)++;
	*ret_len = strlen(*linebuf);
	if (*ret_len > 0 && (*linebuf)[*ret_len - 1] != '\n') {
	    /* catch the special case of missing NL at EOF */
	    if (*ret_len < alloc_len - 1) {
		XDVI_WARNING((stderr, "%s, line %d: missing newline at end of file.", filename, *lineno));
		return *linebuf;
	    }
	    /* buffer too short, need to re-allocate */
	    alloc_len *= 2;
	    *linebuf = xrealloc(*linebuf, alloc_len);
	}
	else if (*ret_len > 2
		 && (*linebuf)[*ret_len - 1] == '\n'
		 && ((*linebuf)[*ret_len - 2] == '\\'
		     || isspace((int)(*linebuf)[*ret_len - 2]))) {
	    /* we may have a continued line */
	    /* chop off trailing whitespace */
	    while (*ret_len > 1 && isspace((int)(*linebuf)[*ret_len - 2]))
		(*ret_len)--;
	    if (*ret_len > 1 && (*linebuf)[*ret_len - 2] != '\\') { /* no continued line */
		read_complete_line = True;
	    }
	    else /* overwrite backslash and newline at next read */
		*ret_len -= 2;
	}
	else
	    read_complete_line = True;
    }
    /* chop off trailing NL */
    (*linebuf)[--(*ret_len)] = '\0';
    return *linebuf;
}

static void
read_mime_file(FILE *fp, const char *filename, int *mime_line)
{
    int i;
    const size_t line_len = 1024;
    size_t ret_len = 0;
    char *linebuf = xmalloc(line_len);
    char *cp, *cp2;

    int dummy = 0;
    TRACE_HTEX((stderr, "reading mime file \"%s\"", filename));
    while ((read_config_line(filename, fp, &linebuf, line_len, &ret_len, &dummy)) != NULL) {
	if (linebuf[ret_len] == '\n')
	    linebuf[ret_len] = '\0';

	cp = linebuf;
	while (isspace((int)*cp))
	    cp++;
	if (*cp == '#')
	    continue;

	/* split either on tab or on whitespace */
	if ((cp2 = strchr(cp, '\t')) == NULL && (cp2 = strchr(cp, ' ')) == NULL)
	    continue;
	*cp2 = '\0';	/* Terminate cp string */
	cp2++;
	while (isspace((int)*cp2))
	    cp2++;
	if (*cp2 == '\0')
	    continue;	/* No extensions list */
	if (*mime_line >= m_mimemap_size) {
	    m_mimemap_size += MIME_ALLOC_STEP;
	    m_mimemap = xrealloc(m_mimemap, m_mimemap_size * sizeof *m_mimemap);
	    for (i = *mime_line; i < m_mimemap_size; i++) {
		m_mimemap[i].content_type = m_mimemap[i].extensions = NULL;
	    }
	}
#if 0
	fprintf(stderr, "========== type %d: |%s| extension: |%s|\n", *mime_line, cp, cp2);
#endif
	free(m_mimemap[*mime_line].content_type);
	m_mimemap[*mime_line].content_type = xstrdup(cp);
	free(m_mimemap[*mime_line].extensions);
	m_mimemap[*mime_line].extensions = xstrdup(cp2);

	(*mime_line)++;
    }
    free(linebuf);
}

static char *
maybe_expand_homedir(const char *path)
{
    char *newpath = NULL;
    TRACE_HTEX((stderr, "maybe_expand_homedir: |%s|", path));
    if (memcmp(path, "$HOME", strlen("$HOME")) == 0) {
	newpath = xstrdup(getenv("HOME"));
	newpath = xstrcat(newpath, path + strlen("$HOME"));
    }
    else if (path[0] == '~') {
	newpath = expand_homedir(path);
	if (newpath == NULL) {
	    XDVI_WARNING((stderr, "Couldn't expand path `%s'", path));
	}
    }
    TRACE_HTEX((stderr, "maybe_expand_homedir: after expansion: |%s|", newpath ? newpath : "<NULL>"));
    return newpath;
}


static void
read_mailcap(FILE *fp, const char *filename, int *mailcap_line)
{
    size_t alloc_len = 1024;
    size_t ret_len = 0;
    char *linebuf = xmalloc(alloc_len);
    int lineno = 0;

    TRACE_HTEX((stderr, "reading mailcap file \"%s\"", filename));

    while ((read_config_line(filename, fp, &linebuf, alloc_len, &ret_len, &lineno)) != NULL) {
	Boolean error = False;
	int i;
	size_t n, num_items;
	char **items;
	char *ptr = linebuf;

	while (isspace((int)*ptr)) {
	    ptr++;
	    ret_len--;
	}
	if (*ptr == '#' || *ptr == '\0') /* comments or empty lines */
	    continue;

	/* split line into fields */
	items = split_line(ptr, ';', 0, ret_len, &num_items);

	if (num_items == 0) {
	    XDVI_WARNING((stderr, "%s, line %d: skipping malformed line \"%s\" (no command specified)",
			  filename, lineno, linebuf));
	    error = True;
	}
	else if (num_items == 1 || (num_items >= 2 && strlen(items[1]) == 0)) {
	    XDVI_WARNING((stderr, "%s, line %d: skipping malformed line \"%s\" (empty command)",
			  filename, lineno, linebuf));
	    error = True;
	}
	
	if (error) {
	    for (n = 0; n < num_items; n++)
		free(items[n]);
	    free(items);
	    continue;
	}
	
	/* resize m_mailcap */
	while (*mailcap_line >= m_mailcap_size) {
	    m_mailcap_size += MIME_ALLOC_STEP;
	    m_mailcap = xrealloc(m_mailcap, m_mailcap_size * sizeof *m_mailcap);
	    for (i = *mailcap_line; i < m_mailcap_size; i++) {
		m_mailcap[i].content_type = NULL;
		m_mailcap[i].command = NULL;
		m_mailcap[i].needsterminal = False;
		m_mailcap[i].testcmd = NULL;
	    }
	}
	
	for (n = 0; n < num_items; n++) {
	    if (n == 0) { /* first field: content-type */
		m_mailcap[*mailcap_line].content_type = items[n];
		/*
		 * add \/\* to content-type if it only consists of one field
		 * (it's not clear to me from RFC 1343 how this should be handled)
		 */
		if (strchr(m_mailcap[*mailcap_line].content_type, '/') == NULL) {
		    m_mailcap[*mailcap_line].content_type = xstrcat(m_mailcap[*mailcap_line].content_type, "/*");
		}
	    }
	    else if (memcmp(items[n], "test=", strlen("test=")) == 0) {
		m_mailcap[*mailcap_line].testcmd = xstrdup(items[n] + strlen("test="));
		free(items[n]);
	    }
	    else if (memcmp(items[n], "needsterminal", strlen("needsterminal")) == 0) { /* set flag */
		m_mailcap[*mailcap_line].needsterminal = True;
		free(items[n]);
	    }
	    else if (memcmp(items[n], "copiousoutput", strlen("copiousoutput")) == 0
		     || memcmp(items[n], "compose=", strlen("compose=")) == 0
		     || memcmp(items[n], "composetyped=", strlen("composetyped=")) == 0
		     || memcmp(items[n], "print=", strlen("print=")) == 0
		     || memcmp(items[n], "edit=", strlen("edit=")) == 0
		     || memcmp(items[n], "x11-bitmap=", strlen("x11-bitmap=")) == 0
		     || memcmp(items[n], "description=", strlen("description=")) == 0) {
		free(items[n]);
		continue;
	    }
	    else { /* command field */
		m_mailcap[*mailcap_line].command = items[n];

		if (find_format_str(m_mailcap[*mailcap_line].command, "%s") != NULL)
		    m_mailcap[*mailcap_line].format_string = "%s";
		else if (find_format_str(m_mailcap[*mailcap_line].command, "%u") != NULL)
		    m_mailcap[*mailcap_line].format_string = "%u";
		else
		    m_mailcap[*mailcap_line].format_string = "";
	    }
	}
	free(items);
	
	(*mailcap_line)++;
    }
    free(linebuf);
}

/* parse mime or mailcap file, return true if successful. */
static Boolean
parse_mime_mailcap(const char *env_var,
		   const char *default_var_value,
		   int *lines,
		   void(*parse_func)(FILE *fp, const char *filename, int *mime_line))
{
    const char *path = NULL;
    char **path_elems;
    size_t elem_cnt, i;
    FILE *fp;
    Boolean success = False;
    
    if ((path = getenv(env_var)) == NULL) {
	path = default_var_value;
    }

    path_elems = split_line(path, ':', 0, strlen(path), &elem_cnt);
    for (i = 0; i < elem_cnt; i++) {
	char *newpath;
	/* expand paths */
	if ((newpath = maybe_expand_homedir(path_elems[i])) != NULL) {
	    free(path_elems[i]);
	    path_elems[i] = newpath;
	}

	if ((fp = XFOPEN(path_elems[i], FOPEN_R_MODE)) != NULL) {
	    parse_func(fp, path_elems[i], lines);
	    success = True;
	}
	free(path_elems[i]);
    }
    free(path_elems);
    return success;
}    

static void
parsemimes(void)
{
    static Boolean already_called = False;
    
    if (already_called)
	return;
    already_called = True;

    m_mimemap_currlen = 0;

    if (!parse_mime_mailcap("EXTENSIONMAPS", DEFAULT_EXTENSIONMAPS, &m_mimemap_currlen, read_mime_file)) {
	m_mimemap_currlen = XtNumber(default_mimemap);
	m_mimemap = default_mimemap;
    }
}


/*
  Partial parsing of mailcap file.

  Currently not implemented:
  - specifiers %t, %{xyz}, %n, %F
  - realplayer's %u is treated as %s (couldn't find any documentation on %u)
*/
static void
parsemailcap(void)
{
    static Boolean already_called = False;
    
    if (already_called)
	return;
    
    already_called = True;

    m_mailcap_currlen = 0;

    if (!parse_mime_mailcap("MAILCAPS", DEFAULT_MAILCAPS, &m_mailcap_currlen, read_mailcap)) {
	m_mailcap_currlen = XtNumber(default_mailcap);
	m_mailcap = default_mailcap;
    }
}


/*
  Try to match test against a content type definition pattern.
  The strings have one of the following formats (ignore spaces around `/'
  - these are just to avoid breaking the C comment syntax):
  * / *
  text / *
  * / plain
  text / plain
  where * matches everything.
*/
static Boolean
match_content_type(const char *pattern, const char *test)
{
    /* chop both strings into parts at the '/' */
    char *subtype_pattern, *subtype_test;
    if ((subtype_pattern = strchr(pattern, '/')) == NULL) { /* malformed string? */
	XDVI_WARNING((stderr, "Malformed content-type \"%s\" (should be \"type/subtype\")", pattern));
	return False;
    }
    subtype_pattern++;
    if ((subtype_test = strchr(test, '/')) == NULL) { /* malformed string? */
	XDVI_WARNING((stderr, "Malformed content-type \"%s\" (should be \"type/subtype\")", test));
	return False;
    }
    subtype_test++;
    /* check whether it either matches the strings or a wildcard */
    if ((*pattern == '*' && *subtype_pattern == '*') ||
	(*pattern == '*' && strcmp(subtype_pattern, subtype_test) == 0) ||
	(*subtype_pattern == '*' && memcmp(pattern, test, subtype_test - test) == 0) ||
	(strcmp(subtype_pattern, subtype_test) == 0 && memcmp(pattern, test, subtype_test - test) == 0)) {
	return True;
    }
    return False;
}

static Boolean
run_test_command(const char *cmd, const char *arg)
{
    int retval;
    const char *ptr;
    size_t len;
    char *syscmd = NULL;
    
    UNUSED(arg); /* FIXME */
    if (cmd == NULL)
	return True;

    syscmd = xmalloc(strlen(cmd) + strlen(arg) + 1);
    if ((ptr = find_format_str(cmd, "%s")) != NULL) {
	len = 2;
    }
    else if ((ptr = find_format_str(cmd, "%u")) != NULL) {
	len = 2;
    }
    else {
	len = 0;
	ptr = strchr(cmd, '\0');
    }
    /* build system command */
    memcpy(syscmd, cmd, ptr - cmd);
    if (len > 0) { /* append argument */
	strcpy(syscmd + (ptr - cmd), arg);
	strcpy(syscmd + (ptr - cmd) + strlen(arg), ptr + len);
    }
    else {
	syscmd[ptr - cmd] = '\0';
    }
    
    retval = system(syscmd);
    TRACE_HTEX((stderr, "Execution of test command `%s' returned value %d", syscmd, retval));
    free(syscmd);
    return retval == 0;
}

char *
figure_viewer(const char *content_type, const char **format_string, Boolean *needs_terminal, const char *arg)
{
    int i;

    /* Hardwire xdvi for this MIME type so that xdvizilla isn't used,
       which would be too dangerous since xdvizilla tries to unlink
       the DVI file by default.  There might be other DVI viewers, but
       since the user invoking this from xdvi anyway ...
       
       FIXME: A better way would be to always copy the original file, like
       e.g. Acroread does it; see comment in launch_program(), hypertex.c.

       Furthermore, xdvizilla has been retired in the meantime.
    */
    /* FIXME: why do we actually need format_string - why not sprintf()
       directly into the target?
    */
    if (strcmp(content_type, "application/x-dvi") == 0) {
	*format_string = "%s";
	*needs_terminal = False;
	return xstrdup("xdvi %s");
    }
    /* try each command from our mailcap list */
    for (i = 0; i < m_mailcap_currlen; i++) {
	if (globals.debug & DBG_HTEX) {
	    fprintf(stderr, "type |%s| viewer |%s| needsterminal=%s testcmd |%s|\n",
		    m_mailcap[i].content_type, m_mailcap[i].command,
		    m_mailcap[i].needsterminal ? "yes" : "no",
		    m_mailcap[i].testcmd ? m_mailcap[i].testcmd : "None");
	}
	if (match_content_type(m_mailcap[i].content_type, content_type)
	    && run_test_command(m_mailcap[i].testcmd, arg)) {
	    *format_string = m_mailcap[i].format_string;
	    *needs_terminal = m_mailcap[i].needsterminal;
	    return xstrdup(m_mailcap[i].command);
	}
    }
    /* failure */
    return NULL;
}


/* return the mime type for filename, or default types
   for non-recognized extensions/no extensions */
char *
figure_mime_type(const char *filename)
{
    int i;
    char *extension, *cp;
    char *content_type = NULL;

    /* First check for the mailcap and mime files */
    parsemimes();
    parsemailcap();

    if (globals.debug & DBG_HTEX)
	fprintf(stderr, "figure_mime_type: Called to find type of %s\n",
		filename);
    
    /* See if this is a directory */
    if (filename[strlen(filename) - 1] == '/') {
	if (globals.debug & DBG_HTEX)
	    fprintf(stderr, "It's a directory, returning unknownExtensionMimeType: %s\n",
		    resource.unknown_mime_suffix);
	return resource.unknown_mime_suffix;
    }

    /* See if filename extension is on the mime list: */
    extension = strrchr(filename, '.');

    if (extension == NULL) {
	TRACE_HTEX((stderr,
		    "No extension, defaulting to noExtensionMimeType: %s\n",
		    resource.no_mime_suffix));
	return resource.no_mime_suffix;
    }
    extension++;
    /*
     * corrupt URLs might have empty extensions; we need to catch this,
     * since the while loop below would not terminate in that case:
     */
    if (strcmp(extension, "") == 0) {
	XDVI_WARNING((stderr, "Empty extension for file name or URL `%s'\n", filename));
	return resource.no_mime_suffix;
    }

    for (i = 0; i < m_mimemap_currlen; i++) {
	/*
	 * find extension in m_mimemap[i].extensions, a space-separated list
	 * of extension strings.
	 */
	cp = m_mimemap[i].extensions;
	while ((cp = strstr(cp, extension)) != NULL) {
	    if ((cp - m_mimemap[i].extensions > 0) && (cp[-1] != ' ')) {
		cp++;
		continue;
	    }
	    cp += strlen(extension);
	    if ((*cp != ' ') && (*cp != '\0'))
		continue;
	    content_type = m_mimemap[i].content_type;
	    break;
	}
	if (content_type != NULL)
	    break;
    }

    if (content_type == NULL) {
	content_type = xstrdup(resource.unknown_mime_suffix);
	TRACE_HTEX((stderr,
		    "Unknown extension, defaulting to unknownExtensionMimeType: %s",
		    content_type));
    }
    else {
	TRACE_HTEX((stderr, "Found mime type: %s", content_type));
    }

    return content_type;
}

