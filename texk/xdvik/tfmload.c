/* This code was stolen from dvips for xdvi use.  The file itself did
 * not carry any licensing info.  According to other filed the code
 * either public domain, or, in the worst case, GPL.
 *
 * Loads a tfm file.  It marks the characters as undefined.
 *
 * Modified for use by xdvi/t1 by Nicolai Langfeldt */

#include <stdio.h>

#include "xdvi-config.h"
#include "xdvi.h"
#include "util.h"
#include "tfmload.h"

#ifdef T1LIB

#include "kpathsea/tex-file.h"

int fallbacktfm = 0;

Boolean
tfmload(const char *name, long *design, long *widths, long *fontdimen2)
{
    int i;
    int li;
    int nw, hd;
    int bc, ec;
    int f_param_off, f_param_num;
    long wtab[256];
    unsigned short chardat[256];
    char *filename;
    FILE *curfnt;

    fallbacktfm = 0;

    filename = kpse_find_tfm(name);

    if (filename == NULL) {
	filename = kpse_find_tfm("cmr10.tfm");
	if (fallbacktfm == 0)
	    fallbacktfm = 1;
    }

    if (filename == NULL)
	return False;

    curfnt = XFOPEN(filename, "r");

    if (curfnt == NULL)
	return False;

    /* Next, we read the font data from the tfm file, and store it in
     * our own array.  */

    /* Lengths section, 16 bit unsigned quanities */
    li = get_bytes(curfnt, 2);	/* lf = length of file, in words(4 bytes) */
    hd = get_bytes(curfnt, 2);	/* lh = length of header data, in words */
    bc = get_bytes(curfnt, 2);	/* bc = lowest charcode */
    ec = get_bytes(curfnt, 2);	/* ec = largest charcode */
    nw = get_bytes(curfnt, 2);	/* nw = number of width words */
    f_param_off = get_bytes(curfnt, 2);	/* nh = number of height words */
    f_param_off += get_bytes(curfnt, 2);	/* nd = number of depth words */
    f_param_off += get_bytes(curfnt, 2);	/* ni = number of italic words */
    f_param_off += get_bytes(curfnt, 2);	/* nl = number of words in lig/kern table */
    f_param_off += get_bytes(curfnt, 2);	/* nk = number of words in the kern table */
    f_param_off += get_bytes(curfnt, 2);	/* ne = number of words in the extensible char tab */
    f_param_num = get_bytes(curfnt, 2);	/* np = number of font parameter words */

    /* Header, 32 bit signed quantities */
    (void)get_bytes(curfnt, 4);	/* header[0]: checksum */
    *design = get_bytes(curfnt, 4);	/* header[1]: design size */

    /* Skip the rest: header[2..(hd-1)] */
    for (i = 2; i < hd; i++)
	li = get_bytes(curfnt, 4);

    /* Initialize to undef value */
    for (i = 0; i < 256; i++) {
	chardat[i] = 256;
	widths[i] = 0;
    }

    /* The charinfo array: */
    for (i = bc; i <= ec; i++) {
	chardat[i] = get_byte(curfnt);	/* 8 bits of width index */
	(void)get_byte(curfnt);	/* skip */
	(void)get_bytes(curfnt, 2);	/* skip, jump */
    }

    /* The (compressed!) width table */
    for (i = 0; i < nw; i++)
	wtab[i] = get_bytes(curfnt, 4);

    /* skip to font parameter words */
    for (i = 0; i < f_param_off; i++)
	(void)get_bytes(curfnt, 4);

    /* read fontdimen2 (width of space) */
    for (i = 0; i < f_param_num; i++) {
	long l = get_bytes(curfnt, 4);
	if (i == 1) {
	    *fontdimen2 = l;
	    if (globals.debug & DBG_T1)
		fprintf(stderr, "fontdimen2 of %s: %ld\n", filename, *fontdimen2);
	}
	    
    }
    (void)fclose(curfnt);

    /* Now we have the table and the indexes.  Expand into a directly
       mapped width array */
    for (i = bc; i <= ec; i++)
	if (chardat[i] != 256)
	    widths[i] = wtab[chardat[i]];

    return True;
}

#endif /* T1LIB */
