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

#ifndef DVI_DRAW_H_
#define DVI_DRAW_H_

#include "xdvi.h"


extern void prescan(FILE *fp);


/* forward declarations */
struct font;
struct tn;

/* this information is saved when using virtual fonts */
struct drawinf {
    struct framedata data;
    struct font	*fontp;
    set_char_proc set_char_p;
    unsigned long tn_table_len;
    struct font	**tn_table;
    struct tn *tn_head;
    ubyte *pos, *end; /* pointers to a scan buffer defined in dvi-draw.c */
    struct font	*virtual;
#ifdef TEXXET
    int	dir;
#endif
};

struct src_parsed_special {
    int line;
    int col;
    char *filename;
    size_t filename_len;
};
extern void src_parse(const char *str, int str_len, struct src_parsed_special *parsed);

/*
 * pixel_conv is currently used only for converting absolute positions
 * to pixel values; although normally it should be
 *
 *	((int) ((x) / currwin.shrinkfactor + (1 << 15) >> 16)),
 *
 * the rounding is achieved instead by moving the constant 1 << 15 to
 * PAGE_OFFSET in dvi-draw.c.
 */

#define	pixel_conv(x)	    ((int) ((x) / currwin.shrinkfactor >> 16))
#define	pixel_round(x)	    ((int) ROUNDUP(x, currwin.shrinkfactor << 16))

/* entries below with the characters 'dvi' in them are actually stored in
   scaled pixel units */

#define DVI_H   currinf.data.dvi_h
#define PXL_H   pixel_conv(currinf.data.dvi_h)
#define DVI_V   currinf.data.dvi_v
#define PXL_V   currinf.data.pxl_v
#define WW      currinf.data.w
#define XX      currinf.data.x
#define YY      currinf.data.y
#define ZZ      currinf.data.z

extern void draw_page(void);
extern void source_reverse_search(int, int, wide_bool);
extern void source_special_show(wide_bool);
extern void source_forward_search(const char *);

extern void anchor_search(const char *str);


/*
  this is needed by any program that wants to use spcl_scan,
  since the buffer is supposed to be of that length.
*/
#ifndef	DVI_BUFFER_LEN
#define	DVI_BUFFER_LEN 2048
#endif

extern void open_font_file(struct font *fontp);
extern long text_do_char(FILE *fp, struct scan_info *info, wide_ubyte ch);
extern Boolean spcl_scan(Boolean(*spcl_proc) (char *str, int str_len, void *data), void *data, Boolean return_if_found, FILE *fp);

extern void geom_scan_part(long(*char_proc)(FILE *, struct scan_info *, wide_ubyte),
			   FILE *fp, struct scan_info *info, struct frame *min_frame, double current_dimconv);

extern void geom_scan(long(*char_proc)(FILE *, struct scan_info *, wide_ubyte),
		      FILE *fp, struct scan_info *info, int pageno);

extern off_t save_file_status(FILE *fp, struct drawinf *currinf_save, ubyte *maxchar_save);
extern void reinit_text_scan(void);
extern void restore_file_status(FILE *fp, struct drawinf currinf_save, ubyte maxchar_save, off_t pos_save);

void htex_do_special(const char *str, size_t len);

extern setcharRetvalT set_char(
#ifdef TEXXET
			       wide_ubyte cmd,
#endif
			       wide_ubyte ch);
extern setcharRetvalT load_n_set_char(
#ifdef TEXXET
				      wide_ubyte cmd,
#endif
				      wide_ubyte ch);
extern setcharRetvalT set_vf_char(
#ifdef TEXXET
				  wide_ubyte cmd,
#endif
				  wide_ubyte ch);

extern setcharRetvalT set_ft_char(
#if TEXXET
				  wide_ubyte cmd,
#endif
				  wide_ubyte ch);

#ifdef GREY
void init_plane_masks(void);
#endif

#if COLOR
struct rgb; /* forward declaration */

Pixel alloc_color(const struct rgb *, Pixel);
void do_color_change(void);
#elif GREY
void init_pix(void);
#endif

void dvi_fmt_error(const char *message, ...);

#endif /* DVI_DRAW_H_ */
