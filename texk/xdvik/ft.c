/*========================================================================*\

Copyright (c) 2013  Paul Vojta

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
 *	FreeType (PostScript Type 1 and TrueType) font reading routines.
 *	Public routines are load_ft_font and read_ft_char.
 *	The prototype for load_ft_font is in dvi-init.h.
 */

#include "xdvi-config.h"
#include "xdvi.h"
#include "dvi-init.h"
#include "dvi-draw.h"
#include "util.h"
#include "font-open.h"

#include <ctype.h>
#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_SIZES_H

static	FT_Library	library		= NULL;

static	struct avl_enc	*enc_head	= NULL;

/* From xc/lib/X11/PutImage.c */

#if !WORDS_BIGENDIAN
static const unsigned char reverse_byte[0x100] = {
	0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
	0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
	0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
	0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
	0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
	0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
	0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
	0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
	0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
	0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
	0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
	0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
	0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
	0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
	0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
	0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
	0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
	0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
	0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
	0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
	0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
	0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
	0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
	0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
	0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
	0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
	0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
	0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
	0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
	0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
	0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
	0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};
#endif /* !WORDS_BIGENDIAN */


/*
 *	FreeType I/O stream functions
 */

static unsigned long
xdvi_stream_read(FT_Stream stream,
		unsigned long offset,
		unsigned char *buffer,
		unsigned long count)
{
	struct font	*fontp;

	fontp = ((struct ftfont *) stream->descriptor.pointer)->first_size;
	open_font_file(fontp);
	fseek(fontp->file, offset, SEEK_SET);
	return (unsigned long) fread(buffer, 1, count, fontp->file);
}

static void
xdvi_stream_close(FT_Stream stream)
{
	/* do nothing */
}


/*
 *	Public routines
 */

static void
read_ft_char(struct font *fontp, wide_ubyte ch)
{
	struct glyph	*g;
	FT_Face		face;
	FT_GlyphSlot	slot;
	int		bmTypeT_wide;
	int		err;

	if (globals.debug & DBG_PK)
	    printf("Loading FreeType char %d", ch);

	/*
	 * Load it unscaled first, so that we can get a more accurate
	 * (non-hinted) value of the horizontal advance.
	 */

	g = &fontp->glyph[ch];
	face = fontp->ft->face;
	FT_Activate_Size(fontp->size);
	err = FT_Load_Glyph(face, g->addr, FT_LOAD_NO_SCALE);
	if (err != 0)
	    XDVI_FATAL((stderr, "FT_Load_Glyph: error = %d", err));

	g->dvi_adv = face->glyph->metrics.horiAdvance * fontp->ft->expn
	  * fontp->pixsize * (1 << 16) / face->units_per_EM + 0.5;

	/* Now do the real loading.  */

	err = FT_Load_Glyph(face, g->addr, FT_LOAD_RENDER | FT_LOAD_MONOCHROME);
	if (err != 0)
	    XDVI_FATAL((stderr, "FT_Load_Glyph: error = %d", err));

	slot = face->glyph;
	g->bitmap.w = slot->bitmap.width;
	g->bitmap.h = slot->bitmap.rows;

	if (globals.debug & DBG_PK)
	    printf(", size=%dx%d, dvi_adv=%ld\n", g->bitmap.w, g->bitmap.h,
		g->dvi_adv);

	alloc_bitmap(&g->bitmap);
	bmTypeT_wide = ROUNDUP((int) g->bitmap.w, BMBITS);

	if (slot->bitmap.width > 0) {
	    int			i;
	    unsigned char	*p;
	    bmUnitT		*q;

	    p = slot->bitmap.buffer;
	    q = (bmUnitT *) g->bitmap.bits;
	    for (i = g->bitmap.h; i > 0; --i) {
#if WORDS_BIGENDIAN
		memcpy(q, p, (slot->bitmap.width + 7) / 8);
#else
		unsigned char	*p1	= p;
		bmUnitT		*q1	= q;
		bmUnitT		data	= 0;
		int		shift	= 0;
		int		j;

		for (j = (slot->bitmap.width + 7) / 8; j > 0; --j) {
		    if (shift >= BMBITS) {
			*q1++ = data;
			data = 0;
			shift = 0;
		    }
		    data |= reverse_byte[*p1++] << shift;
		    shift += 8;
		}
		*q1 = data;
#endif
		p += slot->bitmap.pitch;
		q += bmTypeT_wide;
	    }
	}

	/* The offset of (-1,-1) is from comparing bitmaps of 'R' and 'a'
	   between cmr10.pfb and cmr10.300pk.  */
	g->x = -1 - slot->bitmap_left;
	g->y = slot->bitmap_top - 1;
}

/*
 *	Do the ScaleFont, etc. directives.
 */

static Boolean
set_transform(struct ftfont *ftp,
		const char *str)
{
	double	x	= 1.0;
	double	y	= 0.0;

	for (;;) {
	    while (isspace((unsigned char)*str)) ++str;
	    if (*str == '\0')
		break;

	    if (isdigit((unsigned char)*str) || *str == '.' || *str == '-') {
		double	arg	= strtod(str, (char **) &str);

		while (isspace((unsigned char)*str)) ++str;

		if (memcmp(str, "ObliqueSlant", 12) == 0
		  && (isspace((unsigned char)str[12]) || str[12] == '\0')) {
		    arg = -tan(arg);
		    str += 12;
		    while (isspace((unsigned char)*str)) ++str;
		}

		if (memcmp(str, "SlantFont", 9) == 0
		  && (isspace((unsigned char)str[9]) || str[9] == '\0')) {
		    y += arg;
		    str += 9;
		}
		else if (memcmp(str, "ExtendFont", 10) == 0
		  && (isspace((unsigned char)str[10]) || str[10] == '\0')) {
		    x *= arg;
		    str += 10;
		}
		else return False;
	    }
	    else {	/* other characters; presume encoding name */
		while (!isspace((unsigned char)*str) && *str != '\0') ++str;
		while (isspace((unsigned char)*str)) ++str;
		if (memcmp(str, "ReEncodeFont", 12) == 0
		  && (isspace((unsigned char)str[12]) || str[12] == '\0'))
		    str += 12;
		else
		    return False;
	    }
	}

	if (x != 1.0 || y != 0.0) {
	    FT_Matrix mat;

	    mat.xx = (FT_Fixed) (x * 0x10000 + 0.5);
	    mat.xy = (FT_Fixed) (y * 0x10000 + 0.5);
	    mat.yx = 0;
	    mat.yy = 0x10000;

	    FT_Set_Transform(ftp->face, &mat, NULL);
	    ftp->expn = x;
	}

	return True;
}


/*
 *	When a document uses many freetype fonts, it can take a while to read
 *	them all (due mostly to disk access issues).  So, we delay loading
 *	freetype fonts until they're needed.
 *
 *	Therefore, this routine is not called until it is time to render a
 *	character from the font (see set_ft_char() in dvi-draw.c).
 *
 *	It loads and sets up the font.
 */

Boolean
load_ft_font(struct font *fontp)
{
	struct ftfont	*ftp;
	struct avl_t1	*t1p;
	struct font	*fontp2;
	FT_Face		face;
	FT_Size		size;
	int		err;
	const char	*path;
	FILE		*f;

	ftp = fontp->ft;
	t1p = ftp->t1;
	fontp2 = ftp->first_size;

	if (!resource.freetype) {
	    TRACE_FT((stderr,
	      "load_ft_font returning: freetype has been turned off"));
	    return False;
	}

	if (t1p->bad) {
	    TRACE_FT((stderr, "Font %s was flagged as bad; skipping (size %d)",
	      fontp->fontname, (int) (fontp->fsize + 0.5)));
	    return False;
	}

	face = ftp->face;
	if (face != NULL) {	/* if this face is already in use */
	    err = FT_New_Size(face, &size);
	    if (err != 0) {
		/* This error is probably better kept at stderr instead of
		   a popup (too annoying) or the statusline (might disappear
		   too fast) ... */
		XDVI_ERROR((stderr, "Could not load FreeType font %s at %d: "
		  "FreeType FT_New_Size error = %d.\n"
		  "Will try pixel version instead.\n"
		  "Please see the FreeType documentation for details "
		  "about this.",
		  fontp->fontname, (int) (fontp->fsize + 0.5), err));
		return False;
	    }
	}
	else {
	    FT_Open_Args args;

	    f = open_t1_font(t1p, &path);
	    if (f == NULL)
		return False;

	    if (library == NULL) {
		err = FT_Init_FreeType(&library);
		if (err != 0) {
		    XDVI_ERROR((stderr,
		      "Could not initialize FreeType library: "
		      "FreeType FT_Init_FreeType error = %d.\n"
		      "Turning off use of FreeType.\n"
		      "Please see the FreeType documentation for details "
		      "about this.",
		      err));
		    resource.freetype = False;
		    free((char *) path);
		    fclose(f);
		    return False;
		}
	    }

	    fontp2->filename = path;
	    fontp2->file = f;

	    fseek(f, 0L, SEEK_END);
	    ftp->stream.size = ftell(f);
	    fseek(f, 0L, SEEK_SET);	/* this might not be necessary */

	    ftp->stream.descriptor.pointer = ftp;
	    ftp->stream.read = xdvi_stream_read;
	    ftp->stream.close = xdvi_stream_close;

	    args.flags = FT_OPEN_STREAM;
	    args.stream = &ftp->stream;
	    err = FT_Open_Face(library, &args, 0, &face);
	    if (err != 0) {
		/* This error is probably better kept at stderr instead of
		   a popup (too annoying) or the statusline (might disappear
		   too fast) ... */
		XDVI_ERROR((stderr, "Could not load FreeType font %s: "
		  "FreeType FT_Open_Face error = %d.\n"
		  "Will try pixel version instead.\n"
		  "Please see the FreeType documentation for details "
		  "about this.",
		  fontp->fontname, err));
		free((char *) path);
		fclose(f);
		fontp2->file = NULL;
		fontp2->filename = NULL;
		return False;
	    }
	    ftp->face = face;

	    if (!FT_IS_SCALABLE(face)) {
		TRACE_FT((stderr, "Font %s is not scalable.", fontp->fontname));
		FT_Done_Face(face);
		free((char *) path);
		fclose(f);
		fontp2->file = NULL;
		fontp2->filename = NULL;
		return False;
	    }

	    ftp->expn = 1.0;
	    if (t1p->addinfo != NULL)
		if (!set_transform(ftp, t1p->addinfo)) {
		    FT_Done_Face(face);
		    free((char *) path);
		    fclose(f);
		    fontp2->file = NULL;
		    fontp2->filename = NULL;
		    return False;
		}

	    if (face->charmap == NULL) {
		if (face->num_charmaps > 0) {
		    if ((globals.debug & DBG_PK) && face->num_charmaps > 1)
			printf("Choosing the first of %d charmaps.\n",
			  face->num_charmaps);
		}
		FT_Set_Charmap(face, face->charmaps[0]);
	    }
	    else if (face->charmap->encoding == ft_encoding_unicode
	      && FT_Select_Charmap(face, ft_encoding_adobe_custom) != 0
	      && FT_Select_Charmap(face, ft_encoding_adobe_standard) != 0
	      && FT_Select_Charmap(face, ft_encoding_adobe_expert) != 0) {
		if (globals.debug & DBG_PK)
		    puts("Using unicode charmap for font.");
		fputs("Using unicode charmap\n", stderr);	/* ||| */
	    }

	    ftp->enc = NULL;
	    if (t1p->encname != NULL) {	/* if there's an encoding */
		struct avl_enc	*encp;

		encp = (struct avl_enc *) avladd(t1p->encname,
		  strlen(t1p->encname),
		  (struct avl **) &enc_head, sizeof *encp);
		if (encp->key == t1p->encname)	/* if new record */
		    read_encoding(encp);
		if (encp->valid)
		    ftp->enc = encp;
	    }

	    size = face->size;
	}

	/* Get character indices (store them in addr) */

	if (globals.debug & DBG_PK)
	    printf("Character indices for %s:\n", fontp->fontname);

	FT_Activate_Size(size);

	err = FT_Set_Char_Size(face,
	  (int) (fontp->pixsize * (72<<6) / resource.pixels_per_inch + 0.5), 0,
	  resource.pixels_per_inch, resource.pixels_per_inch);
	if (err != 0)
	    XDVI_FATAL((stderr, "FT_Set_Char_Size: error = %d\n", err));

	/* Look for already-computed character indices */
	for (fontp2 = ftp->first_size;; fontp2 = fontp2->next_size) {
	    int i;

	    if (fontp2 == NULL) {	/* if not found */
		struct avl_enc *encp = ftp->enc;

		for (i = 0; i < 256; ++i) {
		    int ci;

		    if (encp == NULL)
			ci = FT_Get_Char_Index(face, i);
		    else {
			const char *glyphname = encp->vec[i];

			if (glyphname == NULL)
			    ci = 0;
			else
			    ci = FT_Get_Name_Index(face,
			      (FT_String *) glyphname);
		    }

		    fontp->glyph[i].addr = ci;
		    if (globals.debug & DBG_PK)
			printf("%3d->%3d%s", i, ci, (i + 1) % 8 ? "  " : "\n");
		}
		break;
	    }
	    if (fontp2->size != NULL) {		/* found the information */
		for (i = 0; i < 256; ++i)
		    fontp->glyph[i].addr = fontp2->glyph[i].addr;
		break;
	    }
	}

	fontp->size = size;	/* it needed to be NULL in the above loop */
	fontp->read_char = read_ft_char;

	return True;
}
