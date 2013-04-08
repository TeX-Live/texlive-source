/*
 * Copyright (c) 2002-2013 the xdvik development team
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
 */

#ifndef FONT_OPEN_H_
#define FONT_OPEN_H_


#if FREETYPE || PS

#include "util.h"	/* get AVL basics */

#if FREETYPE
# include <ft2build.h>
# include FT_FREETYPE_H
#endif


/*
 *	AVL tree structures.
 */

	/* Data structure for Type 1 fonts -- contents of psfonts.map */
struct avl_t1 {
	AVL_COMMON;
	const char	*psname;	/* PS name of font */
	const char	*fontfile;	/* (short) name of pfa/pfb file */
	const char	*encname;	/* (short) name of encoding file */
	const char	*addinfo;	/* additional PS instructions */
# if FREETYPE
	Boolean		bad;		/* if later found to be unloadable */
	struct ftfont	*ft;		/* pointer to FreeType record */
# endif
};


# if FREETYPE

struct ftfont {		/* info for FreeType font (Type 1 or TrueType) */
	FT_Face		face;		/* NULL means not loaded yet */
	struct font	*first_size;
	struct avl_t1	*t1;
	/* struct avl_tt *tt; */
	struct FT_StreamRec_ stream;
	struct avl_enc	*enc;		/* pointer to encoding record */
	double		expn;		/* expansion factor */
};

struct avl_enc {
	AVL_COMMON;
	Boolean		valid;
	const char	*vec[256];
};

# endif /* FREETYPE */

extern Boolean init_t1_lookup(void);
extern Boolean lookup_t1_font(struct font *fontp, const char *fontname);
extern FILE *open_t1_font (struct avl_t1 *t1p, const char **path_ret);
extern void read_encoding(struct avl_enc *encp);

#endif /* FREETYPE || PS */


extern FILE *font_open(
#if DELAYED_MKTEXPK
		       Boolean load_font_now,
#endif
		       struct font *fontp,
		       const char **font_ret,
		       int *dpi_ret);

#if DELAYED_MKTEXPK
void reset_missing_font_count(void);
#endif

#endif /* FONT_OPEN_H_ */
