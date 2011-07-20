/*
  This code was stolen from dvips (getpsinfo() in resident.c) for xdvi
  use.  The file itself does not carry any licensing info, except for
  reference to dvips.h, which doesn't have a standart-type license
  either.  The code seems to be either in the public domain, or, in
  the worst case, GPL.
  
  - Nicolai Langfeldt, 21/2/1999

*/

#include "xdvi-config.h"
#include "xdvi.h"
#include "util.h"
#include "read-mapfile.h"
#include "dvi-draw.h" /* for add_T1_mapentry */
#include <stdio.h>

#ifdef T1LIB

#include "kpathsea/tex-file.h"

#define INLINE_SIZE 2048

/*
 * Read a dvips psfont map file.  Return True if it was read ok,
 * False otherwise.
 */
Boolean
read_map_file(char *name)
{
    FILE *fp;
    char *p;
    char *specinfo;
    char specbuf[500];
    char *fullname;
    int lineno = 0;
    static char inline_buf[INLINE_SIZE];

    /* Try first for xdvi specific file */
    fullname = kpse_find_file(name, kpse_program_text_format, 0);
    /* Then any fontmap file */
    if (fullname == NULL) {
	fullname = kpse_find_file(name, kpse_fontmap_format, 0);
    }

    if (fullname == NULL) {
	XDVI_WARNING((stderr, "Could not find map file \"%s\".", name));
	return False;
    }

    fp = XFOPEN(fullname, "r");
    if (fp == NULL) {
	XDVI_WARNING((stderr, "Could not open map file \"%s\": %s.\n", name, strerror(errno)));
	return False;
    }

    TRACE_T1((stderr, "Map file: %s\n", fullname));
    
    while (fgets(inline_buf, INLINE_SIZE, fp) != NULL) {
	char *TeXname = NULL;
	char *PSname = NULL;
	char *Fontfile = NULL;
	char *Vectfile = NULL;
	char *hdr_name = NULL;
	specinfo = NULL;
	specbuf[0] = 0;
	lineno++;

	/* One line at a time */
	p = inline_buf;

	if (*p < ' ' || *p == '*' || *p == '#' || *p == ';' || *p == '%')
	    continue;

	while (*p) {
	    while (*p && *p <= ' ')
		p++;
	    if (*p) {
		/* Note: In dvips, this flag is set outside the loop
		   so that font files may be scanned/included as encoding files
		   (I guess it doesn't matter for dvips, but it does for xdvi ...)
		*/
		boolean encoding_p = False;
		if (*p == '"') {	/* PostScript instructions? */
		    if (specinfo) {
			strcat(specbuf, specinfo);
			strcat(specbuf, " ");
		    }
		    specinfo = p + 1;

		}
		else if (*p == '<') {	/* Header to download? */
		    if (p[1] == '<') {	/* << means always full download, ignore */
			p++;
		    }
		    else if (p[1] == '[') {	/* <[ means an encoding */
			p++;
			encoding_p = True;
		    }
		    p++;
		    /* skip whitespace after < */
		    while (*p && *p <= ' ')
			p++;

		    /* save start of header name */
		    hdr_name = p;

		}
		else if (TeXname)	/* second regular word on line? */
		    PSname = p;

		else	/* first regular word? */
		    TeXname = p;

		if (*p == '"') {
		    p++;	/* find end of "..." word */
		    while (*p != '"' && *p)
			p++;
		}
		else
		    while (*p > ' ')	/* find end of anything else */
			p++;

		/* Terminate the substring we just identified */
		if (*p)
		    *p++ = 0;

		/* If we had a header we were downloading, figure out what to do;
		   couldn't do this above since we want to check the suffix.  */
		if (hdr_name) {
		    const char *suffix = find_suffix(hdr_name);
		    if (encoding_p
			|| (suffix != NULL && strcmp(suffix, "enc") == 0)) {
			Vectfile = hdr_name;
		    }
		    else {
			Fontfile = hdr_name;
		    }
		}
	    }
	}	/* while (*p) */

	/* Line finished.  Lookit what we got */
	if (specinfo) {
	    strcat(specbuf, specinfo);
	}

	if (TeXname) {
	    add_T1_mapentry(lineno, fullname, TeXname, Fontfile, Vectfile, specinfo);
	}

    }
    fclose(fp);
    return True;
}

#endif /* T1LIB */
