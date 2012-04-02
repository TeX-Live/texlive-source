/*========================================================================*\

Copyright (c) 1990-2004  Paul Vojta

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
#include "xdvi.h"

#include "dvi-init.h"
#include "dvi-draw.h"
#include "util.h"
#include "x_util.h"
#include "exit-handlers.h"
#include "mime.h"
#include "pagesel.h"
#include "special.h"
#include "hypertex.h"
#include "kpathsea/c-fopen.h"
#include "kpathsea/c-stat.h"
#include "kpathsea/magstep.h"
#include "kpathsea/tex-glyph.h"
#include "dvi.h"
#include "string-utils.h"
#include "browser.h"
#include "sfSelFile.h"
#include "xm_toolbar.h"
#include "pagehist.h"

#if DELAYED_MKTEXPK
#include "font-open.h" /* for reinit_missing_font_count() */    
#endif



#ifdef T1LIB
#include "t1lib.h"
#endif

#include "message-window.h"
#include "search-internal.h"
#include "statusline.h"
#include "events.h"
#include "font-open.h"

#define	PK_PRE		247
#define	PK_ID		89
#define	PK_MAGIC	((PK_PRE << 8) | PK_ID)
#define	GF_PRE		247
#define	GF_ID		131
#define	GF_MAGIC	((GF_PRE << 8) | GF_ID)
#define	VF_PRE		247
#define	VF_ID_BYTE	202
#define	VF_MAGIC	((VF_PRE << 8) | VF_ID_BYTE)

/* font stuff */
struct font *tn_table[TNTABLELEN];
struct font *font_head = NULL;
struct tn *tn_head = NULL;
wide_ubyte maxchar;
unsigned short	current_timestamp = 0;


unsigned long magnification;
double dimconv;
double tpic_conv;

/* Curently processed page number (starting at 0). */
int current_page = 0;
int total_pages = 0;

/* postamble offset is saved in this global variable */
long g_postamble_offset;


static struct stat fstatbuf;

static FILE *m_dvi_fp = NULL; /* original user's file */


static Boolean open_dvi_file(const char *filename, Boolean open_new_instance);

/*
 * DVI preamble and postamble information.
 */
static unsigned long numerator, denominator;
static unsigned int dvi_unshrunk_page_w, dvi_unshrunk_page_h;
static unsigned int m_paper_unshrunk_w, m_paper_unshrunk_h;


/*
 * Offset in DVI file of last page, set in read_postamble().
 */
static long m_last_page_offset;

static const char *dvi_err_list[] = {
    /* NO_ERROR                  */	"No Error",
    /* WRONG_DVI_VERSION         */	"Wrong version of DVI output for this program",
    /* DVI_CORRUPTED             */	"DVI file corrupted",
    /* NOT_A_DVI_FILE            */	"Not a DVI file",
    /* POSTAMBLE_NO_POST         */	"Postamble doesn't begin with POST",
    /* POSTAMBLE_NO_MATCH        */	"Postamble doesn't match preamble",
    /* POSTAMBLE_NON_FNTDEF      */	"Non-fntdef command found in postamble",
    /* NOT_ALL_PIXEL_FILES_FOUND */	"Not all pixel files were found",
    /* NO_BOP_AT_PAGEDESC        */	"Page description doesn't begin with BOP",
    /* FILE_HAS_ZERO_SIZE        */	"File has zero size",
    /* FILE_DOESNT_EXIST         */	"No such file",
    /* FILE_IS_DIRECTORY         */	"Is a directory",
    /* UNKNOWN_ERROR             */	"An unknown error occurred"
};

/*
 * access method for above list
 */
const char *
get_dvi_error(dviErrFlagT flag) {
    ASSERT(flag < XtNumber(dvi_err_list), "Flag out of range");
    return dvi_err_list[flag];
}


/*
 * Extract the unit used in paper size specification.  This information is used
 * to decide the initial grid separation.
 */
static int
atopixunit(const char *arg)
{
    int len = strlen(arg);

    /* check if the unit 'mm' or 'cm' occurs in the arg string */
    return (len > 2 && (arg[len - 2] == 'c' || arg[len - 2] == 'm') && arg[len - 1] == 'm' ?
	    1.0 / 2.54 : 1.0) * resource.pixels_per_inch + 0.5;
}

/*
 *	free_vf_chain frees the vf_chain structure.
 */

static void
free_vf_chain(struct tn *tnp)
{
    while (tnp != NULL) {
	struct tn *tnp1 = tnp->next;
	free((char *)tnp);
	tnp = tnp1;
    }
}

/*
 *	delete glyph information in a font.
 */

static void
delete_glyphs(struct font *fontp)
{
    struct glyph *g;

    for (g = fontp->glyph; g <= fontp->glyph + fontp->maxchar; ++g) {
	if (g->bitmap2.bits) {
	    free(g->bitmap2.bits);
	    g->bitmap2.bits = NULL;
	}
#ifdef	GREY
	if (g->pixmap2) {
	    XDestroyImage(g->image2);
	    g->pixmap2 = NULL;
	    if (g->pixmap2_gc2 != NULL) {
		free(g->pixmap2_gc2);
		g->pixmap2_gc2 = NULL;
	    }
	}
#if COLOR
	g->fg = NULL;
#endif
#endif
    }
}

/*
 *	Release all shrunken bitmaps for all fonts.
 */

void
reset_fonts(void)
{
    struct font *f;

    for (f = font_head; f != NULL; f = f->next) {
	if ((f->flags & FONT_LOADED) && !(f->flags & FONT_VIRTUAL)) {
	    delete_glyphs(f);
	}
    }
}

/*
 * free up fonts no longer in use.
 */
static void
free_unused_fonts(void)
{
    struct font *fontp;
    struct font **fontpp;

    fontpp = &font_head;
    while ((fontp = *fontpp) != NULL) {
	if (fontp->flags & FONT_IN_USE)
	    fontpp = &fontp->next;
	else {
	    if (globals.debug & DBG_PK)
		printf("xdvi: Discarding font \"%s\" at %d dpi\n",
		       fontp->fontname, (int)(fontp->fsize + 0.5));
	    *fontpp = fontp->next;	/* remove from list */
	    free(fontp->fontname);
	    if (fontp->flags & FONT_LOADED) {
		if (fontp->file != NULL) {
		    fclose(fontp->file);
		}
		free(fontp->filename);
		if (fontp->flags & FONT_VIRTUAL) {
		    struct macro *m;

		    for (m = fontp->macro;
			 m <= fontp->macro + fontp->maxchar; ++m)
			if (m->free_me) free((char *)m->pos);
		    free((char *)fontp->macro);
		    free((char *)fontp->vf_table);
		    free_vf_chain(fontp->vf_chain);
		}
		else {
		    delete_glyphs(fontp);
		    free((char *)fontp->glyph);
		}
		free((char *)fontp);
	    }
	}
    }
}

#if COLOR

/*
 *	Release all allocated pixels, and (in greyscale mode) invalidate
 *	all shrunken glyphs.
 */

void
reset_colors(void)
{
    if (color_list_len != 0) {
	XFreeColors(DISP, G_colormap, color_list, color_list_len, 0);
	color_list_len = 0;
    }
    while (bg_head != NULL) {
	struct bgrec *bgp;
	struct fgrec *fgp;

	for (fgp = bg_head->fg_head; fgp != NULL;) {
	    struct fgrec *fgp1 = fgp->next;
	    free(fgp);
	    fgp = fgp1;
	}
	bgp = bg_head->next;
	free(bg_head);
	bg_head = bgp;
    }
#if GREY
    if (resource.use_grey) {
	struct font *f;
	struct glyph *g;

	for (f = font_head; f != NULL; f = f->next)
	    if ((f->flags & FONT_LOADED) && !(f->flags & FONT_VIRTUAL))
		for (g = f->glyph; g <= f->glyph + f->maxchar; ++g)
		    g->fg = NULL;
    }
#endif /* GREY */
    bg_current = NULL;
    fg_active = NULL;
    color_warned = False;
}

/*
 *	All of the above, plus discard all scanned information.
 */

void
full_reset_colors(void)
{
    if (page_colors.stack != NULL) {
	size_t i;
	struct rgb *last_freed = &fg_initial;

	/* fprintf(stderr, "i: %d; last freed: %p\n", page_colors.size, &fg_initial); */
	for (i = 0; i < page_colors.size; ++i) {
	    if (page_colors.stack[i].colorstack != last_freed) {
		last_freed = page_colors.stack[i].colorstack;
		/* BUG ALERT: don't free &fg_initial, else segfault with changing
		   foreground in xm_colorsel.c! */
		if (last_freed != NULL && last_freed != &fg_initial) {
		    /* fprintf(stderr, "freeing %d: %p\n", i, last_freed); */
		    free(last_freed);
		}
	    }
	}
	free(page_colors.stack);
	page_colors.stack = NULL;
    }
    reset_colors();
}

#endif /* COLOR */



/*
 *	realloc_font allocates the font structure to contain (newsize + 1)
 *	characters.
 */

void
realloc_font(struct font *fontp, wide_ubyte newsize)
{
    struct glyph *glyph;

    glyph = fontp->glyph = xrealloc(fontp->glyph,
				    (unsigned int)(newsize + 1) * sizeof(struct glyph));
    if (newsize > fontp->maxchar)
	memset((char *)(glyph + fontp->maxchar + 1), 0,
	       (int)(newsize - fontp->maxchar) * sizeof(struct glyph));
    maxchar = fontp->maxchar = newsize;
}


/*
 *	realloc_virtual_font does the same thing for virtual fonts.
 */

void
realloc_virtual_font(struct font *fontp, wide_ubyte newsize)
{
    struct macro *macro;

    macro = fontp->macro = xrealloc(fontp->macro,
				    (unsigned int)(newsize + 1) * sizeof(struct macro));
    if (newsize > fontp->maxchar)
	memset((char *)(macro + fontp->maxchar + 1), 0,
	       (int)(newsize - fontp->maxchar) * sizeof(struct macro));
    maxchar = fontp->maxchar = newsize;
}


/*
 * load_font locates the t1 font or raster file and reads the index of
 * characters, plus whatever other preprocessing is done (depending on
 * the format).
 *
 * Returns True if sucessful, False if not.
 */

Boolean
load_font(struct font *fontp, Boolean use_t1lib
#if DELAYED_MKTEXPK
	  , Boolean load_font_now
#endif
	  )
{
    double fsize = fontp->fsize;
    int dpi = fsize + 0.5;
    char *font_found;
    int size_found;
    int magic;
    Boolean hushcs = resource.hush_chk;

    fontp->file = NULL;

    /* BUG ALERT: This used to be:
     *
     * if (--globals.ev.ctr == 0) {
     *     read_events(EV_GE_IDLE);
     * }
     * force_statusline_update();
     * XSync(DISP, False);
     *
     * The idea was to update the `loading fonts ...' popup. However,
     * calling read_events() here may call dvi_file_changed() if the
     * user clicks on the window, which calls file_exists_p(), and
     * that changes m_dvi_fp while it's still being used to read the
     * postamble (where load_font() is called from), which will cause
     * xdvi to crash! (bug #968127).
     *
     * Sadly, without this update, the `loading fonts' popup doesn't
     * appear before the main window comes up ...
     */

    fontp->file = font_open(
#if DELAYED_MKTEXPK
			    load_font_now,
#endif
			    fontp,
			    (const char **) &font_found,
			    &size_found,
#ifdef T1LIB
			    &fontp->t1id,
#endif
			    use_t1lib);

#if DELAYED_MKTEXPK
    if (!load_font_now)
	return True;
#endif
    
#ifdef T1LIB
    if (fontp->t1id >= 0 && use_t1lib) {
	/* It's a type1 font */
	fontp->fsize = fsize;	/* It comes in all sizes */
	fontp->timestamp = ++current_timestamp;
	fontp->maxchar = maxchar = 255;
	fontp->set_char_p = set_t1_char;
	/* read_T1_char is a dummy */
	fontp->read_char = read_T1_char;
	fontp->glyph = xmalloc (256 * sizeof (struct glyph));
	memset((char *) fontp->glyph, 0, 256 * sizeof (struct glyph));
	fontp->flags |= FONT_LOADED;
	if (font_found != NULL) {
	    statusline_error(STATUS_MEDIUM,
			     "Error: Can't find font %s; using %s instead. Expect ugly output.",
			     fontp->fontname, font_found);
	    force_statusline_update();
	    free(fontp->fontname);
	    fontp->fontname = font_found; /* this has been allocated by font_open */
	}
	return True;
    }
#endif /* T1LIB */
    /* when not using T1lib, fontp->file == NULL means total failure */
    if (fontp->file == NULL) {
	if (globals.ev.flags & EV_GE_NEWDOC)
	    return False;
	fontp->flags |= FONT_LOADED;	/* as loaded as it'll get */
	XDVI_ERROR((stderr, "Can't find font %s.%dpk", fontp->fontname, dpi));
	return False;
    }
    fontp->flags |= FONT_LOADED;

    if (font_found != NULL && strcmp(fontp->fontname, font_found) != 0) {
	/* some other font used - FIXME: is there a more efficient way than strcmp() for checking this? */
	statusline_error(STATUS_MEDIUM,
			 "Can't find pixel font %s; using %s instead at %d dpi.",
			 fontp->fontname, font_found, dpi);
	force_statusline_update();
	free(fontp->fontname);
	fontp->fontname = font_found; /* this has been allocated by font_open */
	hushcs = True;
    }
    else if (!kpse_bitmap_tolerance((double)size_found, fsize)) { /* a different size used */
	statusline_error(STATUS_MEDIUM,
			 "Can't find pixel font %s at %d dpi; using %d dpi instead.",
			 fontp->fontname, dpi, size_found);
	force_statusline_update();
    }

    /* PK version of some font found */
    fontp->fsize = size_found;
    fontp->timestamp = ++current_timestamp;
    fontp->maxchar = maxchar = 255;
    fontp->set_char_p = set_char;
    magic = get_bytes(fontp->file, 2);

    switch(magic) {
    case PK_MAGIC:
	read_PK_index(fontp, (wide_bool)hushcs);
	break;
#ifdef USE_GF
    case GF_MAGIC:
	read_GF_index(fontp, (wide_bool)hushcs);
	break;
#endif
    case VF_MAGIC:
	if (resource.omega)
	    maxchar = read_VF_index(fontp, (wide_bool)hushcs);
	else
	    (void)read_VF_index(fontp, (wide_bool)hushcs);
	break;
    default:
	XDVI_FATAL((stderr, "Cannot recognize format for font file %s", fontp->filename));
	break;
    }
    
    if (fontp->flags & FONT_VIRTUAL) {
	if (!resource.omega) {
	    while (maxchar > 0 && fontp->macro[maxchar].pos == NULL) {
		--maxchar;
	    }
	    if (maxchar < 255) {
		realloc_virtual_font(fontp, (wide_ubyte)maxchar);
	    }
	}
    }
    else {
	while (maxchar > 0 && fontp->glyph[maxchar].addr == 0)
	    --maxchar;
	if (maxchar < 255) {
	    realloc_font(fontp, (wide_ubyte)maxchar);
	}
    }
    return True;
}


/*
 *	MAGSTEPVALUE - If the given magnification is close to a \magstep
 *	or a \magstephalf, then return twice the number of \magsteps.
 *	Otherwise return NOMAGSTP.
 */

#define	NOMAGSTP (-29999)
#define	NOBUILD	29999

static int
magstepvalue(float *mag)
{
    int m = 0;
    double fmag = *mag;
    double xmag = resource.pixels_per_inch;
    float margin = fmag * 0.002;

    if (fmag < resource.pixels_per_inch)
	for (;;) {
	    if (xmag - fmag < margin && -(xmag - fmag) < margin) {
		*mag = xmag;
		return m;
	    }
	    if (xmag < fmag)
		break;
	    xmag *= 0.9128709292;
	    --m;
	}
    else
	for (;;) {
	    if (xmag - fmag < margin && -(xmag - fmag) < margin) {
		*mag = xmag;
		return m;
	    }
	    if (xmag > fmag)
		break;
	    xmag *= 1.095445115;
	    ++m;
	}
    return NOMAGSTP;
}

/*
 *	reuse_font recursively sets the flags for font structures being reused.
 */

static void
reuse_font(struct font *fontp)
{
    struct font **fp;
    struct tn *tnp;

    if (fontp->flags & FONT_IN_USE)
	return;

    fontp->flags |= FONT_IN_USE;
    if (resource.list_fonts)
	printf("xdvi: (reusing) %s at %d dpi\n", fontp->fontname,
	       (int)(fontp->fsize + 0.5));
    if (fontp->flags & FONT_VIRTUAL) {
	for (fp = fontp->vf_table; fp < fontp->vf_table + VFTABLELEN; ++fp)
	    if (*fp != NULL)
		reuse_font(*fp);
	for (tnp = fontp->vf_chain; tnp != NULL; tnp = tnp->next)
	    reuse_font(tnp->fontp);
    }
}


/*
 *      define_font reads the rest of the fntdef command and then reads in
 *      the specified pixel file, adding it to the global linked-list holding
 *      all of the fonts used in the job.
 */
struct font *
define_font(
#if DELAYED_MKTEXPK
	    Boolean read_fonts,		/* reading font definitions */
	    Boolean initialize_fonts,	/* also initializing internal data structures for fonts */
#else
	    Boolean load_font_now,	/* only scanning, or also loading the font? */
#endif
	    FILE *file,
	    wide_ubyte cmnd,
	    struct font *vfparent,	/* vf parent of this font, or NULL */
	    struct font **tntable,	/* table for low TeXnumbers */
	    unsigned int tn_table_len,	/* length of table for TeXnumbers */
	    struct tn **tn_headpp,	/* addr of head of list of TeXnumbers */
	    Boolean *not_found_flag)	/* signal that font hasn't been found */
{
    unsigned int TeXnumber;
    struct font *fontp;
    float fsize;
    double scale_dimconv;
    long checksum;
    int scale;
    int design;
    int magstepval;
    int len;
    char *fontname;
    int size;

    TeXnumber = get_bytes(file, (int)cmnd - FNTDEF1 + 1);
    checksum = get_bytes(file, 4);
    scale = get_bytes(file, 4);
    design = get_bytes(file, 4);
    len = get_byte(file);
    len += get_byte(file);	/* sequence point in the middle */

#if DELAYED_MKTEXPK
    if (!read_fonts) {
	get_bytes(file, len);
	return NULL;
    }
#else
    if (!load_font_now)
	return NULL;
#endif
    
    fontname = xmalloc((unsigned)len + 1);
    (void)fread(fontname, sizeof(char), len, file);
    fontname[len] = '\0';
    
    if (globals.debug & DBG_PK)
	printf("xdvi: Define font \"%s\" scale=%d design=%d number=%d\n",
	       fontname, scale, design, TeXnumber);
    if (vfparent == NULL) {
	fsize = 0.001 * scale / design * magnification * resource.pixels_per_inch;
	scale_dimconv = dimconv;
    }
    else {
	/*
	 * The scaled size is given in units of vfparent->scale * 2 ** -20
	 *      SPELL units, so we convert it into SPELL units by multiplying by
	 *              vfparent->dimconv.
	 *      The design size is given in units of 2 ** -20 pt, so we convert
	 *      into SPELL units by multiplying by
	 *              (resource.pixels_per_inch * 2**16) / (72.27 * 2**20).
	 */
	fsize = (72.27 * (1 << 4)) * vfparent->dimconv * scale / design;
	scale_dimconv = vfparent->dimconv;
    }
    magstepval = magstepvalue(&fsize);
    size = fsize + 0.5;
    /*
     * reuse font if possible
     */
    for (fontp = font_head;; fontp = fontp->next) {
	if (fontp == NULL) {	/* if font doesn't exist yet */
	    if (resource.list_fonts)
		printf("xdvi: %s at %d dpi\n", fontname, (int)(fsize + 0.5));
	    fontp = xmalloc((unsigned)sizeof(struct font));
	    fontp->fontname = fontname;
	    fontp->fsize = fsize;
	    fontp->magstepval = magstepval;
	    fontp->file = NULL;	/* needed if it's a virtual font */
	    fontp->checksum = checksum;
	    fontp->flags = FONT_IN_USE;
	    fontp->dimconv = scale * scale_dimconv / (1 << 20);
	    fontp->set_char_p = load_n_set_char;
		/* pixsize = scaled size of the font in pixels,
		 *	  = scale * [vfparent->]dimconv / (1 << 16).
		 */
		fontp->pixsize =
		  (vfparent != NULL ? vfparent->dimconv : dimconv) * scale
		  / (1 << 16);
	    if (vfparent == NULL)
		if (!load_font(fontp, resource.t1lib
#if DELAYED_MKTEXPK
			       , initialize_fonts
#endif
			       )) {
		    if (globals.ev.flags & EV_GE_NEWDOC) {	/* if aborting */
			free(fontname);
			free(fontp);
			return NULL;
		    }
		    *not_found_flag = True;
		}
	    fontp->next = font_head;
	    font_head = fontp;
	    break;
	}
	if (strcmp(fontname, fontp->fontname) == 0
	    && size == (int)(fontp->fsize + 0.5)) {
	    /* if font already in use */
	    reuse_font(fontp);
	    free(fontname);
	    break;
	}
    }
    if (TeXnumber < tn_table_len)
	tntable[TeXnumber] = fontp;
    else {
	struct tn *tnp;
	tnp = xmalloc((unsigned)sizeof(struct tn));
	tnp->next = *tn_headpp;
	*tn_headpp = tnp;
	tnp->TeXnumber = TeXnumber;
	tnp->fontp = fontp;
    }
    return fontp;
}

	


/*
 *      process_preamble reads the information in the preamble and stores
 *      it into global variables for later use.
 */
Boolean
process_preamble(FILE *fp, dviErrFlagT *errflag)
{
    ubyte k;
    static char job_id[300];

    TRACE_FILES((stderr, "process_preamble: fp = %p, errflag = %d", (void *)fp, *errflag));
    
    if (get_byte(fp) != PRE) {
	*errflag = NOT_A_DVI_FILE;
	TRACE_FILES((stderr, "process_preamble: fp = %p, errflag = %d, returning False", (void *)fp, *errflag));
	return False;
    }
    if (get_byte(fp) != 2) {
	*errflag = WRONG_DVI_VERSION;
	TRACE_FILES((stderr, "process_preamble: fp = %p, errflag = %d, returning False", (void *)fp, *errflag));
	return False;
    }
    numerator = get_bytes(fp, 4);
    denominator = get_bytes(fp, 4);
    magnification = get_bytes(fp, 4);
    dimconv = (((double)numerator * magnification)
	       / ((double)denominator * 1000.));
    dimconv = dimconv * (((long)resource.pixels_per_inch) << 16) / 254000;
    tpic_conv = resource.pixels_per_inch * magnification / 1000000.0;
    k = get_byte(fp);
    (void)fread(job_id, sizeof(char), (int)k, fp);
    job_id[k] = '\0';

    TRACE_FILES((stderr, "process_preamble: fp = %p, errflag = %d, returning True", (void *)fp, *errflag));
    
    return True;
}

/*
 *      find_postamble locates the beginning of the postamble
 *	and leaves the file ready to start reading at that location.
 */
#define	TMPSIZ	516	/* 4 trailer bytes + 512 junk bytes allowed */
Boolean
find_postamble(FILE *fp, dviErrFlagT *errflag)
{
    long pos;
    ubyte temp[TMPSIZ];
    ubyte *p;
    ubyte *p1;
    ubyte byte;

    TRACE_FILES((stderr, "find_postamble on fp: %p", (void *)fp));
    
    fseek(fp, 0L, SEEK_END);
    pos = ftell(fp) - TMPSIZ;
    if (pos < 0)
	pos = 0;
    fseek(fp, pos, SEEK_SET);
    p = temp + fread((char *)temp, sizeof(char), TMPSIZ, fp);
    for (;;) {
	p1 = p;
	while (p1 > temp && *(--p1) != TRAILER);
	p = p1;
	while (p > temp && *(--p) == TRAILER);
	if (p <= p1 - 4)
	    break;	/* found 4 TRAILER bytes */
	if (p <= temp) {
	    *errflag = DVI_CORRUPTED;
	    TRACE_FILES((stderr, "find_postamble: returning FALSE"));
	    return False;
	}
    }
    pos += p - temp;
    byte = *p;
    while (byte == TRAILER) {
	fseek(fp, --pos, SEEK_SET);
	byte = get_byte(fp);
    }
    if (byte != 2) {
	*errflag = WRONG_DVI_VERSION;
	TRACE_FILES((stderr, "find_postamble: returning FALSE"));
	return False;
    }
    fseek(fp, pos - 4, SEEK_SET);
    fseek(fp, get_lbytes(fp, 4), SEEK_SET);
    TRACE_FILES((stderr, "find_postamble: returning TRUE"));
    return True;
}



Boolean
set_paper_type(const char *arg)
{
    const char *arg1;
    char temp[21];
    const char **p;
    char *q;
    const char **paper_types = get_paper_types();
    size_t paper_types_size = get_paper_types_size();
    
    if (*arg == '+') {
	++arg;
	ignore_papersize_specials = True;
    }
    if (strlen(arg) > sizeof(temp) - 1)
	return False;
    q = temp;
    for (;;) {	/* convert to lower case */
	char c = *arg++;
	if (c >= 'A' && c <= 'Z')
	    c ^= ('a' ^ 'A');
	*q++ = c;
	if (c == '\0')
	    break;
    }
    arg = temp;
    /* perform substitutions */
    for (p = paper_types; p < paper_types + paper_types_size; p += 2) {
	if (strcmp(temp, *p) == 0) {
	    arg = p[1];
	    break;
	}
    }
    arg1 = strchr(arg, 'x');
    if (arg1 == NULL)
	return False;
    m_paper_unshrunk_w = atopix(arg, False);
    m_paper_unshrunk_h = atopix(arg1 + 1, False);

    globals.grid_paper_unit = atopixunit(arg);
    
    return (m_paper_unshrunk_w != 0 && m_paper_unshrunk_h != 0);
}

/*
 *      read_postamble reads the information in the postamble from fp,
 *	storing it into global variables.
 *      It also takes care of reading in all of the pixel files for the fonts
 *      used in the job.
 *
 *	FIXME: Would be better (speed up initialization when needing to generate fonts,
 *	and allow to open window on first page) if the font loading was done on-demand later!
 */
Boolean
read_postamble(FILE *fp, dviErrFlagT *errflag,
#if DELAYED_MKTEXPK
	       Boolean read_fonts, Boolean initialize_fonts
#else
	       Boolean load_fonts
#endif
	       )
{
    ubyte cmnd;
    Boolean font_not_found = False;
    struct font	*fontp;

#if DELAYED_MKTEXPK
    int tmp_total_pages;
    unsigned long tmp_numerator = numerator;
    unsigned long tmp_denominator = denominator;
    unsigned long tmp_magnification = magnification;
    unsigned int tmp_dvi_unshrunk_page_w, tmp_dvi_unshrunk_page_h;
    long tmp_last_page_offset;
    
    TRACE_FILES((stderr, "read_postamble: reading %p (%d, %d)", (void *)fp, read_fonts, initialize_fonts));

    if (read_fonts && initialize_fonts) {
	/* clear existing font table */
	memset((char *)tn_table, 0, (int)sizeof tn_table);
	free_vf_chain(tn_head);
	tn_head = NULL;
	for (fontp = font_head; fontp != NULL; fontp = fontp->next)
	    fontp->flags &= ~FONT_IN_USE;
    }
#else /* DELAYED_MKTEXPK */    
    TRACE_FILES((stderr, "read_postamble: reading %p (%d)", (void *)fp, load_fonts));

    /* clear existing font table */
    memset((char *)tn_table, 0, (int)sizeof tn_table);
    free_vf_chain(tn_head);
    tn_head = NULL;
    for (fontp = font_head; fontp != NULL; fontp = fontp->next)
	fontp->flags &= ~FONT_IN_USE;
#endif /* DELAYED_MKTEXPK */
    
    if (get_byte(fp) != POST) {
	*errflag = POSTAMBLE_NO_POST;
	TRACE_FILES((stderr, "read_postamble: returning FALSE"));
	return False;
    }
#if DELAYED_MKTEXPK
    tmp_last_page_offset = get_bytes(fp, 4);
    if (read_fonts && initialize_fonts)
	m_last_page_offset = tmp_last_page_offset;

    if (tmp_numerator != get_bytes(fp, 4)
	|| tmp_denominator != get_bytes(fp, 4)
	|| tmp_magnification != get_bytes(fp, 4)) {
	*errflag = POSTAMBLE_NO_MATCH;
	TRACE_FILES((stderr, "read_postamble: returning FALSE"));
	return False;
    }
    else if (read_fonts && initialize_fonts) {
	numerator = tmp_numerator;
	denominator = tmp_denominator;
	magnification = tmp_magnification;
    }
    
    /* read largest box height and width */
    tmp_dvi_unshrunk_page_h = (spell_conv(get_lbytes(fp, 4)) >> 16) + resource.yoffset_int;
    tmp_dvi_unshrunk_page_w = (spell_conv(get_lbytes(fp, 4)) >> 16) + resource.xoffset_int;
    (void)get_bytes(fp, 2);	/* max stack size */
    tmp_total_pages = get_bytes(fp, 2);

    if (read_fonts && initialize_fonts) {
	dvi_unshrunk_page_h = tmp_dvi_unshrunk_page_h;
	if (dvi_unshrunk_page_h < m_paper_unshrunk_h)
	    dvi_unshrunk_page_h = m_paper_unshrunk_h;
	dvi_unshrunk_page_w = tmp_dvi_unshrunk_page_w;
	if (dvi_unshrunk_page_w < m_paper_unshrunk_w)
	    dvi_unshrunk_page_w = m_paper_unshrunk_w;
	total_pages = tmp_total_pages;
    }
#else /* DELAYED_MKTEXPK */
    m_last_page_offset = get_bytes(fp, 4);
    if (numerator != get_bytes(fp, 4)
	|| denominator != get_bytes(fp, 4)
	|| magnification != get_bytes(fp, 4)) {
	*errflag = POSTAMBLE_NO_MATCH;
	TRACE_FILES((stderr, "read_postamble: returning FALSE"));
	return False;
    }

    /* read largest box height and width */
    dvi_unshrunk_page_h = (spell_conv(get_lbytes(fp, 4)) >> 16) + resource.yoffset_int;
    if (dvi_unshrunk_page_h < m_paper_unshrunk_h)
	dvi_unshrunk_page_h = m_paper_unshrunk_h;
    dvi_unshrunk_page_w = (spell_conv(get_lbytes(fp, 4)) >> 16) + resource.xoffset_int;
    if (dvi_unshrunk_page_w < m_paper_unshrunk_w)
	dvi_unshrunk_page_w = m_paper_unshrunk_w;
    (void)get_bytes(fp, 2);	/* max stack size */
    total_pages = get_bytes(fp, 2);
#endif /* DELAYED_MKTEXPK */

    /* read font definitions */
    while ((cmnd = get_byte(fp)) >= FNTDEF1 && cmnd <= FNTDEF4) {
	struct font *f = define_font(
#if DELAYED_MKTEXPK
				     read_fonts, initialize_fonts,
#else
				     load_fonts,
#endif
				     fp, cmnd, (struct font *)NULL, tn_table,
				     TNTABLELEN, &tn_head, &font_not_found);
	if (
#if DELAYED_MKTEXPK
	    read_fonts && initialize_fonts
#else
	    load_fonts
#endif
	    && f == NULL) {
	    TRACE_FILES((stderr, "read_postamble: returning FALSE"));
	    return False;
	}
#if !DELAYED_MKTEXPK
	else if (!load_fonts) { /* return early */
	    TRACE_FILES((stderr, "read_postamble: returning TRUE"));
	    return True;
	}
#endif
    }

    if (cmnd != POSTPOST) {
	*errflag = POSTAMBLE_NON_FNTDEF;
	TRACE_FILES((stderr, "read_postamble: returning FALSE"));
	return False;
    }
    if (
#if DELAYED_MKTEXPK
	read_fonts && initialize_fonts &&
#endif
	font_not_found) {
	*errflag = NOT_ALL_PIXEL_FILES_FOUND;
	TRACE_FILES((stderr, "read_postamble: returning FALSE"));
	return False;
    }
#if DELAYED_MKTEXPK
    if (read_fonts && initialize_fonts)
	free_unused_fonts();
#else
    free_unused_fonts();
#endif

    TRACE_FILES((stderr, "read_postamble: returning TRUE"));
    return True;
}


static Boolean
prepare_pages(dviErrFlagT *errflag)
{
    int i;
    long offset;

    TRACE_FILES((stderr, "calling pageinfo_deallocate"));
    pageinfo_deallocate();

    TRACE_FILES((stderr, "pageinfo_allocate for %d pages", total_pages + 1));
    pageinfo_allocate(total_pages + 1);
    pageinfo_set_page_width(total_pages, m_paper_unshrunk_w);
    pageinfo_set_page_height(total_pages, m_paper_unshrunk_h);
    pageinfo_set_window_width(total_pages, dvi_unshrunk_page_w);
    pageinfo_set_window_height(total_pages, dvi_unshrunk_page_h);

    pageinfo_set_offset(total_pages - 1, m_last_page_offset);
    /*
     * Follow back pointers through pages in the DVI file,
     * storing the offsets in the pageinfo table.
     */
    for (offset = m_last_page_offset, i = total_pages; i > 0; i--) {
	fseek(globals.dvi_file.bak_fp, offset, SEEK_SET);
	if (get_byte(globals.dvi_file.bak_fp) != BOP) {
	    *errflag = NO_BOP_AT_PAGEDESC;
	    return False;
	}
	pageinfo_set_offset(i-1, offset);
	/* from c_0, read count0, the TeX page number */
	pageinfo_set_number(i-1, get_bytes(globals.dvi_file.bak_fp, 4));

	/* skip c_1 to c_9 */
	fseek(globals.dvi_file.bak_fp, 9*4L, SEEK_CUR);

	/* next 4 byte contain offset to previous bop */
	offset = get_bytes(globals.dvi_file.bak_fp, 4);
    }
    /* If not prescanning, initialize page sizes.  */
    if (!resource.prescan) {
	for (i = 0; i < total_pages; ++i) {
	    pageinfo_set_page_width(i, m_paper_unshrunk_w);
	    pageinfo_set_page_height(i, m_paper_unshrunk_h);
	    pageinfo_set_window_width(i, dvi_unshrunk_page_w);
	    pageinfo_set_window_height(i, dvi_unshrunk_page_h);
	}
    }
    return True;
}

void
init_page(void)
{
    if (globals.dvi_file.bak_fp == NULL)
	return;
    globals.page.unshrunk_w = pageinfo_get_page_width(current_page);
    globals.page.unshrunk_h = pageinfo_get_page_height(current_page);
    globals.page.w = ROUNDUP(globals.page.unshrunk_w, mane.shrinkfactor) + 2;
    globals.page.h = ROUNDUP(globals.page.unshrunk_h, mane.shrinkfactor) + 2;
    TRACE_FILES((stderr, "init_page: setting globals.page.w = %d, globals.page.h = %d", globals.page.w, globals.page.h));
}

#ifndef	S_ISDIR
#define	S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#endif

static char *m_tmp_dvi_name = NULL; /* name of backup file for useTempFp */

/* access function for backup file name */
char *get_tmp_dvi_name(void) {
    return m_tmp_dvi_name;
}

static void
remove_tmp_dvi_file(void *dummy)
{
    UNUSED(dummy);
    if (m_tmp_dvi_name != NULL) {
	unlink(m_tmp_dvi_name);
	free(m_tmp_dvi_name);
    }
    m_tmp_dvi_name = NULL;
}


static FILE *
make_backup_fp(FILE *source_fp, FILE *target_fp)
{
    static Boolean first_time = True;
    static int tmp_fd = 0;

#if !HAVE_FTRUNCATE
    /* in this case, we can't use ftruncate() on the existing temp file -
       just close the existing one, and set flag to open a new one */
    remove_tmp_dvi_file(NULL);
    if (target_fp != NULL)
	fclose(target_fp);
    /* make sure we use a new temp file, else we'd have a race condition
       after closing it */
    first_time = True;
#endif
    
    if (first_time) { /* doesn't exist yet, create it */
	if ((tmp_fd = xdvi_temp_fd(&m_tmp_dvi_name)) == -1) {
	    XDVI_ERROR((stderr, "error creating temporary file - disabling `useTempFp'."));
	    resource.use_temp_fp = False;
	    remove_tmp_dvi_file(NULL);
	    return NULL;
	}
	/* 	fprintf(stderr, "temporary file name: |%s|, %d\n", m_tmp_dvi_name, tmp_fd); */
	TRACE_EVENTS((stderr, "Created temp file: |%s|\n", m_tmp_dvi_name));
	if ((target_fp = try_fdopen(tmp_fd, "wb+")) == NULL) {
	    XDVI_ERROR((stderr, "error opening temporary file (%s) - disabling `useTempFp'.", strerror(errno)));
	    resource.use_temp_fp = False;
	    remove_tmp_dvi_file(NULL);
	    return NULL;
	}
	first_time = False;
    }
    else { /* if not first time, truncate the existing file,
	      and position both files at beginning */
	ASSERT(target_fp != NULL, "");
	ASSERT(source_fp != NULL, "");

#if HAVE_FTRUNCATE
	if (ftruncate(tmp_fd, 0) < 0) {
		
	    XDVI_ERROR((stderr, "Couldn't truncate file %s: %s - disabling `useTempFp'; target_fp: %p.",
			m_tmp_dvi_name, strerror(errno), target_fp));
	    resource.use_temp_fp = False;
	    remove_tmp_dvi_file(NULL);
	    fclose(target_fp);
	    return NULL;
	}
#endif
	fseek(target_fp, 0L, SEEK_SET);
	fseek(source_fp, 0L, SEEK_SET);
    }

    /* copy the file */
    if (!copy_fp(source_fp, target_fp)) {
	XDVI_ERROR((stderr,
		    "Error creating temporary file: %s\n"
		    "- disabling `useTempFp'.",
		    strerror(errno)));
	remove_tmp_dvi_file(NULL);
	resource.use_temp_fp = False;
	fclose(target_fp);
	target_fp = NULL;
    }
    
    /* rewind both files, else DVI parsing will fail! */
    if (target_fp != NULL) {
	fflush(target_fp);
    }
    fseek(source_fp, 0L, SEEK_SET);
    if (target_fp != NULL) {
	fseek(target_fp, 0L, SEEK_SET);
    }
    
    return target_fp;
}

static Boolean
file_exists_p(const char *path, dviErrFlagT *errflag)
{
    TRACE_FILES((stderr, "file_exists_p for |%s|", path));
    *errflag = UNKNOWN_ERROR;
    if ((m_dvi_fp = XFOPEN(path, OPEN_MODE)) == NULL) {
	/*  	fprintf(stderr, "after internal_open_dvi1: xfopen\n"); */
	*errflag = FILE_DOESNT_EXIST;
	return False;
    }
    TRACE_FILES((stderr, "m_dvi_fp for |%s| = %p", path, (void *)m_dvi_fp));
    /*      fprintf(stderr, "after internal_open_dvi2: xfopen\n"); */
    
    /* shouldn't happen */
    if (fstat(fileno(m_dvi_fp), &fstatbuf) != 0 || S_ISDIR(fstatbuf.st_mode)) {	/* if it's a directory */
	*errflag = FILE_IS_DIRECTORY;
	fclose(m_dvi_fp);
	m_dvi_fp = NULL;
	return False;
    }
    /* If file has zero size, something has gone wrong with downloading
       it, and the user should already have been warned about that;
       just return in this case.
       TODO: can it still happen that we try to load such a file as .dvi
       file? (Will exit  with `draw_part: unknown op-code xyz' or some such).
       In this case, it would be better to look at the preamble before
       entering the drawing loop.
    */
    if (fstatbuf.st_size == 0) {
	*errflag = FILE_HAS_ZERO_SIZE;
	fclose(m_dvi_fp);
	m_dvi_fp = NULL;
	return False;
    }
    return True;
}

/*
 *	internal_init_dvi is the main subroutine for reading the startup
 *	information from the dvi file.
 */

static Boolean
internal_init_dvi(dviErrFlagT *errflag,
#if DELAYED_MKTEXPK
		  Boolean read_fonts, Boolean initialize_fonts
#else
		  Boolean load_fonts
#endif
		  )
{
    char *icon_name = NULL, *title_name = NULL;

    have_src_specials = False;

    TRACE_FILES((stderr, "internal_init_dvi, globals.dvi_file.bak_fp = %p", (void *)globals.dvi_file.bak_fp));
    
    if (!process_preamble(globals.dvi_file.bak_fp, errflag)
	|| !find_postamble(globals.dvi_file.bak_fp, errflag)
#if DELAYED_MKTEXPK
	|| !read_postamble(globals.dvi_file.bak_fp, errflag, read_fonts, initialize_fonts)
#else
	|| !read_postamble(globals.dvi_file.bak_fp, errflag, load_fonts)
	|| !prepare_pages(errflag)
#endif
	) {
	return False;
    }
#if DELAYED_MKTEXPK
    if (!read_fonts || !initialize_fonts) /* return early */
	return True;
    
    if (!prepare_pages(errflag))
	return False;
#endif
    
    if (current_page >= total_pages)
	current_page = total_pages - 1;

    globals.warn_spec_now = resource.warn_spec;
    globals.src.fwd_box_page = -1;
    search_reset_info();
    
    if (globals.pausing.num_save != NULL) {
	free(globals.pausing.num_save);
	globals.pausing.num_save = NULL;
    }
    if (resource.pause && total_pages > 0) {
	globals.pausing.num_save = xmalloc(total_pages * sizeof *globals.pausing.num_save);
	memset(globals.pausing.num_save, 0, total_pages * sizeof *globals.pausing.num_save);
    }
    
    init_prescan();
    /* this allocates icon_name and title_name */
    get_icon_and_title(globals.dvi_name, &icon_name, &title_name);
    set_icon_and_title(icon_name, title_name);
    free(icon_name);
    free(title_name);
    icon_name = title_name = NULL;
    
#if defined(MOTIF) && HAVE_XPM
    tb_check_navigation_sensitivity(current_page);
#endif
    refresh_pagelist(total_pages, current_page);

    return True;
}

/*
 *	internal_open_dvi does the real opening of the dvi file, and sets
 *	globals.dvi_file.time.  It returns True on success, and sets m_dvi_fp and globals.dvi_file.bak_fp.
 */

Boolean
internal_open_dvi(const char *path, dviErrFlagT *errflag,
#if DELAYED_MKTEXPK
		  Boolean read_fonts, Boolean initialize_fonts
#else
		  Boolean load_fonts
#endif
		  )
{
    /*     FILE *tmp_fp = NULL; */
    /*     static FILE *bak_fp = NULL; /\* re-use the temporary backup fp *\/ */
    /*      fprintf(stderr, "------------ opening: |%s|\n", path); */

#if DELAYED_MKTEXPK
    TRACE_FILES((stderr, "internal_open_dvi for |%s|; loading fonts: %d, %d", path, read_fonts, initialize_fonts));
#else
    TRACE_FILES((stderr, "internal_open_dvi for |%s|", path));
#endif
    close_old_filep();

    if (!file_exists_p(path, errflag)) { /* this should set fstatbuf.st_mtime */
	return False;
    }

    if (!resource.use_temp_fp || (globals.dvi_file.bak_fp = make_backup_fp(m_dvi_fp, globals.dvi_file.bak_fp)) == NULL) {
	globals.dvi_file.bak_fp = m_dvi_fp;
    }

    register_exit_handler(remove_tmp_dvi_file, NULL);
    
    globals.dvi_file.time = fstatbuf.st_mtime;

    if (!internal_init_dvi(errflag,
#if DELAYED_MKTEXPK
			   read_fonts, initialize_fonts
#else
			   load_fonts
#endif
			   )) {
	return False;
    }
    
#if COLOR
    full_reset_colors();
#endif /* COLOR */
    reset_papersize_special();
    
    TRACE_FILES((stderr, "internal_open_dvi: SUCCESS!"));

    return True;
}

/*
  check whether `filename' is a DVI file, by checking if it exists, and if so,
  opening it and trying to read a DVI preamble from it:
*/
static char *
is_dvi_file(const char *filename)
{
    FILE *fp;
    struct stat statbuf;
    char *full_pathname;
    dviErrFlagT unused_error;

    TRACE_FILES((stderr, "is_dvi_file %s", filename));
    TRACE_HTEX((stderr, "filename: |%s|", filename));

    /* used to append `.dvi' if not already present, but that was a
       bad idea - if we have both foo.2 and foo.2.dvi, user will want
       to open `foo.2' (with some appropriate application) if the link
       points to that filename. This means that we need to have
       different semantics for filenames on the command-line and
       filenames in hyperlinks; the latter *must* specify an
       extension, the former may omit the extension and default to DVI
       files.
    */
    /* if ((full_filename = filename_append_dvi(filename)) == NULL) { */
    /*     free(full_filename); */
    /*     return NULL; */
    /* } */
    /* TRACE_HTEX((stderr, "full_filename: |%s|\n", full_filename)); */

    if ((full_pathname = find_file(filename, &statbuf, kpse_program_text_format)) == NULL) {
	return NULL;
    }
    else {
	char *tmp = canonicalize_path(full_pathname);
	free(full_pathname);
	full_pathname = tmp;
    }
    
    TRACE_HTEX((stderr, "is_dvi_file: full_pathname: |%s|", full_pathname));

    if ((fp = XFOPEN(full_pathname, OPEN_MODE)) == NULL) {
	free(full_pathname);
	return NULL;
    }

    if (!process_preamble(fp, &unused_error)) {
	free(full_pathname);
	fclose(fp);
	return NULL;
    }

    fclose(fp);
    return full_pathname;
}

static void
report_open_error(const char *msg, const char *filename, dviErrFlagT errflag)
{
    const char *errmsg;
    switch(errflag) {
    case FILE_HAS_ZERO_SIZE:
	errmsg = "File has zero size";
	break;
    case FILE_DOESNT_EXIST:
	errmsg = "No such file";
	break;
    case FILE_IS_DIRECTORY:
	errmsg = "Is a directory";
	break;
    default:
	errmsg = "An unknown error occurred";
	break;
    }
    XDVI_ERROR((stderr, "%s: %s: %s", msg, filename, errmsg));
}

/*
  Implements the algorithm for opening a DVI file from the command line.
  This is similar to `open_dvi_file()' in non-k xdvi.
  
  If the file does not already have `.dvi' extension, append `.dvi'
  and set tried_dvi_ext to True. Try to open this file. If the file
  doesn't exist, try the original file name. If this file doesn't
  exist either, exit with an error message `No such file or directory'.
  If tried_dvi_ext == True, also report that file.dvi didn't exist
  either.

  (A later function will check if the file really is a DVI file, and
  will use tried_dvi_ext for the same purpose).
*/
char *
find_dvi_file(const char *filename, Boolean *tried_dvi_ext, Boolean from_file_history)
{
    char *new_filename;
    size_t len;
    dviErrFlagT errflag;

    ASSERT(filename != NULL, "Filename argument in find_dvi_file() musn't be NULL");
    len = strlen(filename);

    if (len < sizeof(".dvi") || strcmp(filename + len - sizeof(".dvi") + 1, ".dvi") != 0) {
	/* doesn't already have .dvi extension */
	TRACE_HTEX((stderr, "|%s| doesn't have .dvi extension, appending ...", filename));
	new_filename = xstrdup(filename);
	new_filename = xstrcat(new_filename, ".dvi");
	*tried_dvi_ext = True;
	
	if (file_exists_p(new_filename, &errflag)) { /* file exists */
	    char *expanded_filename = expand_filename(new_filename, USE_CWD_PATH);
	    free(new_filename);
	    return expanded_filename;
	}
	else { /* don't report an error; will try verbatim filename next */
	    free(new_filename);
	}
    }

    /* try verbatim filename (might be strange things like `foo.wdvi') */
    if (file_exists_p(filename, &errflag)) {
	char *expanded_filename = expand_filename(filename, USE_CWD_PATH);
	return expanded_filename;
    }
    else {
	if (*tried_dvi_ext) {
	    if (!from_file_history) {
		XDVI_FATAL((stderr, "%s: %s, and %s.dvi doesn't exist either.",
			    filename, get_dvi_error(errflag), filename));
	    }
	    else {
		popup_message(globals.widgets.top_level,
			      MSG_ERR,
			      NULL,
			      "Could not open \"%s\": %s.\n", filename, get_dvi_error(errflag));
	    }
	}
	else {
	    if (!from_file_history) {
		XDVI_FATAL((stderr, "%s: %s.", filename, get_dvi_error(errflag)));
	    }
	    /* else: file is from history; this is at startup, where we may
	       loop through the history until we find a usable file. Don't report
	       an error in this case. */
	}
    }
    return NULL;
}



/*
 * A wrapper for open_dvi_file that can also deal with remote files/URLs, and other
 * file types. Returns the (newly malloc'ed) new dvi file name if it succeeds, NULL else.
 */
char *
open_dvi_file_wrapper(const char *filename,
		      Boolean from_command_line,
		      Boolean open_new_instance,
		      Boolean *tried_dvi_ext,
		      Boolean from_file_history)
{
    char *real_filename = NULL;
    char *new_dvi_name = NULL;
    char canonical_path[MAXPATHLEN + 1];
    
    if (from_command_line) {
	TRACE_HTEX((stderr, "filename IS from commandline"));
	/*
	  if filename is from command-line, we want to treat the file as a DVI file
	  always (and NOT launch the browser or other programs; that'd just confuse
	  people, who meant to launch *xdvi*). `find_dvi_file' tries to locate the
	  file and does all error handling; on success, it returns a fully expanded
	  filename.
	*/
	real_filename = find_dvi_file(filename, tried_dvi_ext, from_file_history); /* this allocates real_filename */
	if (real_filename == NULL)
	    return False;
	
	TRACE_HTEX((stderr, "filename |%s| %p from commandline;\ndvi_name: |%s|,\nfilename: |%s|%p",
		    real_filename, real_filename, globals.dvi_name, filename, (void *)filename));
	new_dvi_name = xstrdup(REALPATH(real_filename, canonical_path));
	free(real_filename);
	TRACE_FILES((stderr, "new_dvi_name: |%s|", new_dvi_name));
	return new_dvi_name;
    }
    else {
	/*
	  if it's not from the command line (e.g. result of clicking Mouse-1 on a link);
	  in this case we might fall back to using the browser.
	  Check whether it's a local file:
	*/
	const char *filename_no_prefix;
	char *expanded_filename;
	TRACE_HTEX((stderr, "filename NOT from commandline"));
	if ((filename_no_prefix = is_local_file(filename)) != NULL) {
	    /* if it's a local file, check whether it's a DVI file: */
	    if ((expanded_filename = is_dvi_file(filename_no_prefix)) != NULL) {
		/* yes, open it */
		if (open_dvi_file(expanded_filename, open_new_instance)) {
		    TRACE_FILES((stderr, "success: %p |%s|", expanded_filename, expanded_filename));
		    if (!open_new_instance) {
			new_dvi_name = expand_filename_append_dvi(expanded_filename, USE_DVI_PATH, True);
			TRACE_FILES((stderr, "new_dvi_name: %p |%s|", (void*)new_dvi_name, new_dvi_name));
		    }
		}
		free(expanded_filename);
		return new_dvi_name;
	    }
	    else {
		/*
		  local file, but not a DVI file;
		  try other viewers for this MIME type:
		*/
		TRACE_HTEX((stderr, "%s is NOT a DVI file", filename_no_prefix));
		launch_program(filename);
		return NULL;
	    }
	}
	else {
	    /* not a local file, retrieve it with the browser: */
	    launch_browser(filename);
	    return NULL;
	}
    }
}


static Boolean
open_dvi_file(const char *filename, Boolean open_new_instance)
{
    Boolean retval = True;

    char *anchor_name = resource.anchor_pos;

    TRACE_HTEX((stderr, "open_dvi_file: |%s| + |%s|",
		filename, anchor_name == NULL ? "<no anchor>" : anchor_name));

    if (open_new_instance) {
	launch_xdvi(filename, anchor_name);
    }
    else {
	dviErrFlagT errflag = NO_ERROR;
	TRACE_HTEX((stderr, "internal_open_dvi: |%s|", filename));
	if (!internal_open_dvi(filename, &errflag, True
#if DELAYED_MKTEXPK
			       , TRUE
#endif
			       )) {
	    report_open_error("Cannot open DVI file", filename, errflag);
	    retval = False;
	}
    }
    return retval;
}

/**
 **	form_dvi_property forms the property used to exhibit the dvi file name
 **	used as a window property (used for source specials).
 **/

void
form_dvi_property(void)
{
#if 0
    size_t len;
    unsigned long ino;
    int i;
#endif

    if (m_dvi_fp == NULL)
	return;

    if (dvi_property != NULL)
	free(dvi_property);

    dvi_property_length = strlen(globals.dvi_name) + 1; /* also copy the terminating 0 */
    dvi_property = xmalloc(dvi_property_length);

    /* NOTE: we don't use dvi_inode like non-k xdvi, since dvi_name is
       always fully expanded with xdvik. */
    strcpy(dvi_property, globals.dvi_name);
}


/* access for m_dvi_fp */
void
close_old_filep(void)
{
    if (m_dvi_fp != NULL) {
	fclose(m_dvi_fp);
	m_dvi_fp = NULL;
	if (!resource.use_temp_fp)
	    globals.dvi_file.bak_fp = NULL;
    }
}



/**
 **	Check for changes in dvi file.
 **	Return True if file has changed, False else.
 **/

Boolean
dvi_file_changed(void)
{
    TRACE_FILES((stderr, "dvi_file_changed: fp = %p?", (void *)m_dvi_fp));

    /*     fprintf(stderr, "m_dvi_fp3: %p (%s)\n", m_dvi_fp, globals.dvi_name); */
    if (m_dvi_fp == NULL) {
	TRACE_FILES((stderr, "m_dvi_fp == NULL"));
	if (stat(globals.dvi_name, &fstatbuf) == 0 && fstatbuf.st_mtime != globals.dvi_file.time) {
	    TRACE_FILES((stderr, "file changed"));
	    if (resource.use_temp_fp) {
		dviErrFlagT errflag = NO_ERROR;
#if !DELAYED_MKTEXPK
		if (resource.watch_file == 0.0)
		    statusline_info(STATUS_MEDIUM, "File changed ...");
		TRACE_FILES((stderr, "returning FALSE"));
#endif
		if (file_exists_p(globals.dvi_name, &errflag)
		    && process_preamble(m_dvi_fp, &errflag)
		    && find_postamble(m_dvi_fp, &errflag)
		    && read_postamble(m_dvi_fp, &errflag, False
#if DELAYED_MKTEXPK
				      , False
#endif
				      )) {
		    TRACE_FILES((stderr, "File OK, reloading ..."));
		    
		    globals.ev.flags |= EV_RELOAD;
		    return True;
		}
		else {
#if DELAYED_MKTEXPK
		    if (resource.watch_file == 0.0)
			statusline_info(STATUS_MEDIUM, "File corrupted (click on window to reload) ...");
		    TRACE_FILES((stderr, "returning FALSE"));
#endif
		    return False;
		}
	    }
	    /* Bug alert: Don't set EV_RELOAD again here, else xdvi
	       can go into a stretch of time where it again and again
	       tries to reload a file even if the user doesn't click
	       on the canvas.
	    */
	    /*
	      else {
	      globals.ev.flags |= EV_RELOAD;
	      }
	    */
	    TRACE_FILES((stderr, "returning TRUE"));
	    return True;
	}
	else {
	    TRACE_FILES((stderr, "file not changed"));
	}
    }
    /* BUG ALERT: Don't use fstat(fileno(m_dvi_fp) here; if file hase
       disappeared, this won't report an error! */
    else if (stat(globals.dvi_name, &fstatbuf) != 0 /* stat failed ... */
	     || fstatbuf.st_mtime != globals.dvi_file.time /* ... or different timestamp, reload: */) {
	dviErrFlagT errflag = NO_ERROR;

	TRACE_FILES((stderr, "Stat failed, or different timestamp ..."));
	globals.dvi_file.time = 0; /* force reload next time also if stat failed */
	
	if (resource.use_temp_fp) { /* in this case, reload only if file has been written completely */
	    if (resource.watch_file == 0.0) {
		statusline_info(STATUS_MEDIUM, "File corrupted (click on window to reload) ...");
		globals.cursor.flags |= CURSOR_CORRUPTED;
		globals.ev.flags |= EV_CURSOR;
	    }
	    else {
		statusline_info(STATUS_MEDIUM, "File corrupted (will try to reload) ...");
	    }
	    close_old_filep();
	    if (file_exists_p(globals.dvi_name, &errflag)
		&& process_preamble(m_dvi_fp, &errflag)
		&& find_postamble(m_dvi_fp, &errflag)
		&& read_postamble(m_dvi_fp, &errflag, False
#if DELAYED_MKTEXPK
				  , False
#endif
				  )) {
		TRACE_FILES((stderr, "File OK, reloading ..."));
		
		globals.ev.flags |= EV_RELOAD;
		return True;
	    }
	    else {
		TRACE_FILES((stderr, "NO successful load: %s", get_dvi_error(errflag)));
		/*  		fprintf(stderr, "=========== NO successful load: %s\n", get_dvi_error(errflag)); */
		return False;
	    }
	}
	else {
	    TRACE_FILES((stderr, "Not using temp fp, reloading..."));
	    globals.ev.flags |= EV_RELOAD;
	    return True;
	}
    }
    return False;
}

/**
 **	Reload the dvi file (unconditionally). Return True on success, False else.
 **/

Boolean
load_dvi_file(
#if !DELAYED_MKTEXPK
	      Boolean load_fonts,
#endif
	      dviErrFlagT *errflag)
{
    unsigned int old_page_w, old_page_h;
    static ino_t dvi_inode = 0;

    TRACE_FILES((stderr, "load_dvi_file: going to read %p", (void *)m_dvi_fp));
    
    if (resource.use_temp_fp && m_dvi_fp != NULL) {
	/* in this case, reload only if file has been written completely */
	*errflag = NO_ERROR;
	fseek(m_dvi_fp, 0L, SEEK_SET);
	if (!process_preamble(m_dvi_fp, errflag)
	    || !find_postamble(m_dvi_fp, errflag)
	    || !read_postamble(m_dvi_fp, errflag,
#if DELAYED_MKTEXPK
			       False, False
#else
			       True
#endif
			       )) {
	    TRACE_FILES((stderr, "reading of %p failed: %s!",
			 (void *)m_dvi_fp,
			 get_dvi_error(*errflag)));
	    return False;
	}
    }

    old_page_w = globals.page.w;
    old_page_h = globals.page.h;

    *errflag = NO_ERROR;

#if DELAYED_MKTEXPK
    /* use same trick with reading postamble twice to first find names of PK fonts
       that need to be created. */
    reset_missing_font_count();
    kpse_set_program_enabled(kpse_any_glyph_format, False, kpse_src_compile);
#endif
    
    if (!internal_open_dvi(globals.dvi_name, errflag,
#if DELAYED_MKTEXPK
			   True, False
#else
			   load_fonts
#endif
			   )) {
	XClearWindow(DISP, mane.win);
	xdvi_bell();
	statusline_info(STATUS_MEDIUM, "%s: %s%s", globals.dvi_name, get_dvi_error(*errflag),
			 resource.watch_file > 0.0 ? "; will try to reload ..." : " (click on window to reload)");
	close_old_filep();

	return False;
    }
#if DELAYED_MKTEXPK
    /* second time */
    kpse_set_program_enabled(kpse_any_glyph_format, True, kpse_src_compile);
    
    if (!internal_open_dvi(globals.dvi_name, errflag, True, True)) {
	XClearWindow(DISP, mane.win);
	xdvi_bell();
	statusline_info(STATUS_MEDIUM, "%s: %s%s", globals.dvi_name, get_dvi_error(*errflag),
			 resource.watch_file > 0.0 ? "; will try to reload ..." : " (click on window to reload)");
	close_old_filep();
	return False;
    }
#endif
    else { /* success */
	if (fstatbuf.st_ino != dvi_inode) {
	    dvi_inode = fstatbuf.st_ino;
	    form_dvi_property();
	    set_dvi_property();
	}
	if (globals.page.w != old_page_w || globals.page.h != old_page_h)
	    reconfig();

	htex_reinit();

	globals.cursor.flags &= ~CURSOR_CORRUPTED;
	return True;
    }
}
