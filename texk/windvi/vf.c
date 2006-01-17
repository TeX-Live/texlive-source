/*========================================================================*\

Copyright (c) 1992-1999  Paul Vojta

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

/*
 *	VF font reading routines.
 *	Public routine is read_index---because virtual characters are presumed
 *	to be short, we read the whole virtual font in at once, instead of
 *	faulting in characters as needed.
 */
#define HAVE_BOOLEAN
#include "xdvi-config.h"
#include "dvi.h"

#define	LONG_CHAR	242

/*
 *	These are parameters which determine whether macros are combined for
 *	storage allocation purposes.  Small macros ( <= VF_PARM_1 bytes) are
 *	combined into chunks of size VF_PARM_2.
 */

#ifndef	VF_PARM_1
#define	VF_PARM_1	20
#endif
#ifndef	VF_PARM_2
#define	VF_PARM_2	256
#endif

/*
 *	The main routine
 */

#ifdef Omega
unsigned long
#else
void
#endif
read_VF_index(fontp, hushcs)
	struct font	*fontp;
	wide_bool		hushcs;
{
	FILE	*VF_file = fontp->file;
	ubyte	cmnd;
	ubyte	*avail, *availend;	/* available space for macros */
	long	checksum;
#ifdef Omega
	struct macro *newmacro;
	unsigned long maxcc=0;
	int i;
#endif

	fontp->read_char = NULL;
	fontp->flags |= FONT_VIRTUAL;
	fontp->set_char_p = set_vf_char;
	if (debug & DBG_PK)
	    Printf("Reading VF pixel file %s\n", fontp->filename);
/*
 *	Read preamble.
 */
	Fseek(VF_file, (long) one(VF_file), 1);	/* skip comment */
	checksum = four(VF_file);
	if (checksum != fontp->checksum && checksum != 0 && fontp->checksum != 0
		&& !hushcs)
	    Fprintf(stderr,
		"Checksum mismatch (dvi = %lu, vf = %lu) in font file %s\n",
		fontp->checksum, checksum, fontp->filename);
	(void) four(VF_file);		/* skip design size */
/*
 *	Read the fonts.
 */
	fontp->vf_table = xmalloc(VFTABLELEN * sizeof(struct font *));
	bzero((char *) fontp->vf_table, VFTABLELEN * sizeof(struct font *));
	fontp->vf_chain = NULL;
	fontp->first_font = NULL;
	while ((cmnd = one(VF_file)) >= FNTDEF1 && cmnd <= FNTDEF4) {
	    struct font *newfontp = define_font(VF_file, cmnd, fontp,
		fontp->vf_table, VFTABLELEN, &fontp->vf_chain);
	    if (fontp->first_font == NULL) fontp->first_font = newfontp;
	}
/*
 *	Prepare macro array.
 */
#ifdef Omega
	fontp->maxchar = 65535;
	fontp->macro = xmalloc(65536 * sizeof(struct macro));
	bzero((char *) fontp->macro, 65536 * sizeof(struct macro));
#else
	fontp->macro = xmalloc(256 * sizeof(struct macro));
	bzero((char *) fontp->macro, 256 * sizeof(struct macro));
#endif
/*
 *	Read macros.
 */
	avail = availend = NULL;
	for (; cmnd <= LONG_CHAR; cmnd = one(VF_file)) {
	    struct macro *m;
	    int len;
	    unsigned long cc;
	    long width;

	    if (cmnd == LONG_CHAR) {	/* long form packet */
		len = four(VF_file);
		cc = four(VF_file);
		width = four(VF_file);
#ifdef Omega
		if (cc >= 65536) {
#else
		if (cc >= 256) {
#endif
		    Fprintf(stderr,
			"Virtual character %lu in font %s ignored.\n",
			cc, fontp->fontname);
		    Fseek(VF_file, (long) len, 1);
		    continue;
		}
	    }
	    else {	/* short form packet */
		len = cmnd;
		cc = one(VF_file);
		width = num(VF_file, 3);
	    }
#ifdef Omega
            maxcc = (cc>maxcc) ? cc : maxcc;
#endif
	    m = &fontp->macro[cc];
	    m->dvi_adv = width * fontp->dimconv;
	    if (len > 0) {
		if (len <= availend - avail) {
		    m->pos = avail;
		    avail += len;
		}
		else {
		    m->free_me = True;
		    if (len <= VF_PARM_1) {
			m->pos = avail = xmalloc(VF_PARM_2);
			availend = avail + VF_PARM_2;
			avail += len;
		    }
		    else m->pos = xmalloc((unsigned) len);
		}
		Fread((char *) m->pos, 1, len, VF_file);
		m->end = m->pos + len;
	    }
	    if (debug & DBG_PK)
		Printf("Read VF macro for character %lu; dy = %ld, length = %d\n",
			cc, m->dvi_adv, len);
	}
	if (cmnd != POST)
	    oops("Wrong command byte found in VF macro list:  %d", cmnd);

	Fclose (VF_file);
	fontp->file = NULL;
	n_files_left++;
#ifdef Omega
	newmacro = xmalloc((maxcc+1) * sizeof(struct macro));
	for (i=0; i<=maxcc; i++) {
	  newmacro[i] = fontp->macro[i];
	}
	free(fontp->macro);
	fontp->macro = newmacro;
	fontp->maxchar = maxcc;
        return maxcc;
#endif
}
