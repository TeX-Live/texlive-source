/*
 * Copyright (c) 1990-2013  Paul Vojta and the xdvik development team
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef DVI_INIT_H_
#define DVI_INIT_H_

#include "xdvi.h"

#if FREETYPE
# include <ft2build.h>
# include FT_FREETYPE_H
#endif

typedef enum {
    NO_ERROR = 0,
    WRONG_DVI_VERSION,
    DVI_CORRUPTED,
    NOT_A_DVI_FILE,
    POSTAMBLE_NO_POST,
    POSTAMBLE_NO_MATCH,
    POSTAMBLE_NON_FNTDEF,
    NOT_ALL_PIXEL_FILES_FOUND,
    NO_BOP_AT_PAGEDESC,
    FILE_HAS_ZERO_SIZE,
    FILE_DOESNT_EXIST,
    FILE_IS_DIRECTORY,
    PS_CONVERSION_FAILED,
    PDF_CONVERSION_FAILED,
    UNKNOWN_ERROR
} dviErrFlagT;

extern const char *get_dvi_error(dviErrFlagT flag);

extern Boolean internal_open_dvi(const char *path, dviErrFlagT *errmsg,
#if DELAYED_MKTEXPK
				 Boolean read_fonts, Boolean initialize_fonts
#else
				 Boolean load_fonts
#endif
				 );
extern char *open_dvi_file_wrapper(const char *filename,
				   Boolean from_command_line,
				   Boolean open_new_instance,
				   Boolean *tried_dvi_ext,
				   Boolean from_file_history);
extern char *get_tmp_dvi_name(void);

/* font stuff */

/*
 * Bitmap structure for raster ops.
 */
struct bitmap {
    unsigned short  w, h;	/* width and height in pixels */
    short     bytes_wide;	/* scan line width in bytes */
    char     *bits;		/* pointer to the bits */
};

/*
 * Per-character information.
 * There is one of these for each character in a font (non-virtual fonts only).
 * All fields are filled in at font definition time,
 * except for the bitmap, which is "faulted in"
 * when the character is first referenced.
 */
struct glyph {
    long addr;		    /* address of bitmap in font file */
    long dvi_adv;	    /* DVI units to move reference point */
    short x, y;		    /* x and y offset in pixels */
    struct bitmap bitmap;   /* bitmap for character */
    short x2, y2;	    /* x and y offset in pixels for shrunken bitmap */
#if GREY
# if COLOR
    struct fgrec *fg;	/* fgrec for which these pixmaps are valid */
# endif
    /* `2' means `shrunken' here */
    XImage *image2;         /* shrunken pixmap for antialiased character */
    char *pixmap2;	    /* image data pointer for image2 */
    char *pixmap2_gc2;	    /* separate image data for drawing image to globals.gc.fore2 */
#endif /* GREY */
    struct bitmap bitmap2;  /* shrunken bitmap for character */
};

/*
 * Per-character information for virtual fonts
 */
struct macro {
	ubyte	*pos;		/* address of first byte of macro */
	ubyte	*end;		/* address of last+1 byte */
	long	dvi_adv;	/* DVI units to move reference point */
	Boolean	free_me;	/* if free(pos) should be called when */
				/* freeing space */
};

/*
 * The layout of a font information block.
 * There is one of these for every loaded font or magnification thereof.
 * Duplicates are eliminated:  this is necessary because of possible recursion
 * in virtual fonts.
 *
 * Also note the strange units.  The design size is in 1/2^20 point
 * units (also called micro-points), and the individual character widths
 * are in the TFM file in 1/2^20 ems units, i.e., relative to the design size.
 *
 * We then change the sizes to SPELL units (unshrunk pixel / 2^16).
 */

#define	NOMAGSTP (-29999)

#define	FONT_IN_USE	1	/* used for housekeeping */
#define	FONT_LOADED	2	/* if font file has been read */
#define	FONT_VIRTUAL	4	/* if font is virtual */

/* forward declarations */
struct font;
struct tn;

typedef	void (*read_char_proc) (struct font *, wide_ubyte);

struct font {
    struct font *next;		/* link to next font info block */
    char *fontname;		/* name of font */
    float fsize;		/* size information (dots per inch) */
    int magstepval;		/* magstep number * two, or NOMAGSTP */
    FILE *file;			/* open font file or NULL */
    const char *filename;	/* name of font file */
    long checksum;		/* checksum */
    unsigned short timestamp;	/* for LRU management of fonts */
    ubyte flags;		/* flags byte (see values below) */
    wide_ubyte maxchar;		/* largest character code */
    double dimconv;		/* size conversion factor */
    set_char_proc set_char_p;	/* proc used to set char */
    /* these fields are used by (loaded) non-virtual fonts */
    read_char_proc read_char;	/* function to read bitmap */
    struct glyph *glyph;
    /* these fields are used by (loaded) virtual fonts */
    struct font **vf_table;	/* list of fonts used by this vf */
    struct tn *vf_chain;	/* ditto, if TeXnumber >= VFTABLELEN */
    struct font *first_font;	/* first font defined */
    struct macro *macro;
#if FREETYPE
    /* these fields are used by (loaded) FreeType fonts */
    struct ftfont *ft;		/* master record for font (all sizes) */
    double pixsize;		/* scaled size of the font in pixels */
    FT_Size size;
    struct font *next_size;	/* next font from same face */
#endif
};

struct tn {
    struct tn *next;		/* link to next TeXnumber info block */
    unsigned long TeXnumber;	/* font number (in DVI file) */
    struct font *fontp;		/* pointer to the rest of the info */
};


extern void reset_fonts(void);
#if COLOR
extern void reset_colors(void);
extern void full_reset_colors(void);
#endif
extern void realloc_font(struct font *, wide_ubyte);
extern void realloc_virtual_font(struct font *, wide_ubyte);
extern Boolean load_font(struct font *
#if DELAYED_MKTEXPK
			 , Boolean load_font_now
#endif
			 );

extern struct font *define_font(
#if DELAYED_MKTEXPK
				Boolean read_fonts,
				Boolean initialize_fonts,
#else
				Boolean load_font_now,
#endif
				FILE *,
				wide_ubyte,
				struct font *,
				struct font **,
				unsigned int,
				struct tn **,
				Boolean *not_found_flag);
extern void init_page(void);
extern void form_dvi_property(void);
extern Boolean dvi_file_changed(void);
extern Boolean load_dvi_file(
#if !DELAYED_MKTEXPK
			     Boolean load_fonts,
#endif
			     dviErrFlagT *errflag);
extern void read_PK_index(struct font *, wide_bool);
extern void read_GF_index(struct font *, wide_bool);
extern unsigned long read_VF_index(struct font *, wide_bool);

#if FREETYPE
extern Boolean load_ft_font(struct font *fontp);
#endif

extern Boolean set_paper_type(const char *arg);

extern Boolean find_postamble(FILE *fp, dviErrFlagT *errflag);
extern Boolean read_postamble(FILE *fp, dviErrFlagT *errflag,
#if DELAYED_MKTEXPK
			      Boolean read_fonts, Boolean initialize_fonts
#else
			      Boolean load_fonts
#endif
			      );
extern void close_old_filep(void);
extern Boolean process_preamble(FILE *fp, dviErrFlagT *errflag);

extern FILE *file_exists(const char *path, dviErrFlagT *errflag);
extern char *find_dvi_file(const char *filename, Boolean *tried_dvi_ext, Boolean from_file_history);
#endif /* DVI_INIT_H_ */
