/*========================================================================*\

Copyright (c) 1992-2004  Paul Vojta

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

\*========================================================================*/

/*
 *	VF font reading routines.
 *	Public routine is read_index---because virtual characters are presumed
 *	to be short, we read the whole virtual font in at once, instead of
 *	faulting in characters as needed.
 */

#include "xdvi-config.h"
#include "dvi.h"
#include "xdvi.h"
#include "util.h"
#include "dvi-init.h"
#include "dvi-draw.h"

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

unsigned long
read_VF_index(struct font *fontp, wide_bool hushcs)
{
    FILE *VF_file = fontp->file;
    ubyte cmnd;
    ubyte *avail, *availend;	/* available space for macros */
    long checksum;
    struct macro *newmacro;
    unsigned long maxcc = 0;
    Boolean dummy_success = False;
    
    fontp->read_char = NULL;
    fontp->flags |= FONT_VIRTUAL;
    fontp->set_char_p = set_vf_char;
    if (globals.debug & DBG_PK)
	printf("Reading VF file %s\n", fontp->filename);
    /*
     *	Read preamble.
     */
    fseek(VF_file, (long)get_byte(VF_file), SEEK_CUR);	/* skip comment */
    checksum = get_bytes(VF_file, 4);
    if (checksum != fontp->checksum && checksum != 0 && fontp->checksum != 0
	&& !hushcs)
	XDVI_WARNING((stderr, "Checksum mismatch (dvi = %lu, vf = %lu) in font file %s",
		      fontp->checksum, checksum, fontp->filename));
    (void)get_bytes(VF_file, 4);	/* skip design size */
    /*
     *	Read the fonts.
     */
    fontp->vf_table = xmalloc(VFTABLELEN * sizeof(struct font *));
    memset((char *)fontp->vf_table, 0, VFTABLELEN * sizeof(struct font *));
    fontp->vf_chain = NULL;
    fontp->first_font = NULL;
    while ((cmnd = get_byte(VF_file)) >= FNTDEF1 && cmnd <= FNTDEF4) {
	struct font *newfontp = define_font(True,
#if DELAYED_MKTEXPK
					    True,
#endif
					    VF_file, cmnd, fontp,
					    fontp->vf_table, VFTABLELEN,
					    &fontp->vf_chain,
					    &dummy_success);
	if (fontp->first_font == NULL)
	    fontp->first_font = newfontp;
    }
    /*
     *	Prepare macro array.
     */
    if (resource.omega) {
	fontp->maxchar = 65535;
	fontp->macro = xmalloc(65536 * sizeof(struct macro));
	memset((char *)fontp->macro, 0, 65536 * sizeof(struct macro));
    }
    else {
	fontp->macro = xmalloc(256 * sizeof(struct macro));
	memset((char *)fontp->macro, 0, 256 * sizeof(struct macro));
    }
    /*
     *	Read macros.
     */
    avail = availend = NULL;
    for (; cmnd <= LONG_CHAR; cmnd = get_byte(VF_file)) {
	struct macro *m;
	int len;
	unsigned long cc;
	long width;

	if (cmnd == LONG_CHAR) {	/* long form packet */
	    len = get_bytes(VF_file, 4);
	    cc = get_bytes(VF_file, 4);
	    width = get_bytes(VF_file, 4);
	    if ((resource.omega && cc >= 65536)
		|| (!resource.omega && cc >= 256)) {
		XDVI_WARNING((stderr, "Virtual character %lu in font %s ignored.",
			      cc, fontp->fontname));
		fseek(VF_file, (long)len, SEEK_CUR);
		continue;
	    }
	}
	else {	/* short form packet */
	    len = cmnd;
	    cc = get_byte(VF_file);
	    width = get_bytes(VF_file, 3);
	}
	if (resource.omega) {
	    maxcc = (cc > maxcc) ? cc : maxcc;
	}
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
		else
		    m->pos = xmalloc((unsigned)len);
	    }
	    (void)fread((char *)m->pos, 1, len, VF_file);
	    m->end = m->pos + len;
	}
	if (globals.debug & DBG_PK)
	    printf("Read VF macro for character %lu; dy = %ld, length = %d\n",
		   cc, m->dvi_adv, len);
    }
    if (cmnd != POST)
	XDVI_FATAL((stderr, "Wrong command byte found in VF macro list:  %d", cmnd));

    fclose(VF_file);
    fontp->file = NULL;
    if (resource.omega) {
	size_t i;
	newmacro = xmalloc((maxcc + 1) * sizeof(struct macro));
	for (i = 0; i <= maxcc; i++) {
	    newmacro[i] = fontp->macro[i];
	}
	free(fontp->macro);
	fontp->macro = newmacro;
	fontp->maxchar = maxcc;
	return maxcc;
    }
    else
	return 0; /* dummy */
}
