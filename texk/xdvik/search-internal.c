/*
 * Copyright (c) 2003-2004 the xdvik development team
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

/*
  How dvi search works:

  (1) Map the search string passed by the user from resource.text_encoding
  to UTF-8 (currently only maps ISO_8859-1), and create a regexp from
  the string if needed.

  If case-insensitive matching is activated and regexp search is not
  active, the search string is converted to lowercase. For regexp
  searches, the flag available with regcomp to ignore case is used.
  
  (2) Scan 2 adjacent pages (so that we can also find matches across page
  boundaries), collecting the text into a char *buffer.
  This involves the following steps:

  - Map characters from wide_ubyte to uint32_t encoding (ucs-4),
  using and Adobe names lookup table (for Postscript Type1
  fonts) or font-specific character info (for Metafont fonts).
	
  This step also maps accented glyphs to composite glyphs,
  expands ligatures to separate characters (e.g. `ffi' to
  `f'`f'`i'), and tries to detect word boundaries and line
  breaks.

  If case-insensitive matching is activated, the characters are
  also lowercased in this step.
	
  Optionally, hyphenation can be removed in this step.
	
  Routine: do_char() in dvi-draw.c (must be in dvi_draw.c since
  we need access to drawing-related stuff like `currinf' to get
  information about the current font, position in the DVI file
  etc.).

  - Save the uint32_t characters into a char *buffer, mapping to
  utf-8.
      
  (3) Search the buffer from (2) for the string from (1). If a match
  has been found, the page is scanned again to store the bounding
  box information of the matched text for highlighting it.
*/



#include "xdvi-config.h"
#include "xdvi.h"

#define SWITCH_TO_UTF8 1

#if HAVE_REGEX_H
#include <regex.h>
#endif

#if HAVE_ICONV_H
#include <iconv.h>
#endif

#include <locale.h>

#if USE_LANGINFO
#include <langinfo.h>
#endif

#include <ctype.h>

#include <X11/keysym.h>

#include "dvi-init.h" /* for total_pages */
#include "dvi-draw.h"
#include "search-internal.h"

#include "search-dialog.h"
#include "message-window.h"
#include "statusline.h"
#include "events.h"
#include "encodings.h"
#include "pagesel.h"
#include "events.h"
#include "util.h"
#include "x_util.h"
#include "string-utils.h"
#include "mag.h"
#include "pagehist.h"
#include "translations.h"

#define DEBUG_SEARCH 0
/* #define MATCH_HIGHLIGHT_INVERTED 1 */


/* margins for highlighting the match */
const int BBOX_LOWER_MARGIN = 2;
const int BBOX_UPPER_MARGIN = 1;

#if 0
#define GET_BBOX(bbox, shrink, x, y, w, h)				\
	x = (bbox.ulx / (double)shrink + 0.5) - BBOX_UPPER_MARGIN;	\
	y = (bbox.uly / (double)shrink + 0.5) - BBOX_UPPER_MARGIN;	\
	w = ((bbox.lrx - bbox.ulx) / (double)shrink) + 0.5		\
	    + BBOX_UPPER_MARGIN + BBOX_LOWER_MARGIN + 1;		\
	h = ((bbox.lry - bbox.uly) / (double)shrink) + 0.5		\
	    + BBOX_UPPER_MARGIN + BBOX_LOWER_MARGIN + 2;
#endif /* 0 */
/* use integer arithmetic */
#define GET_BBOX(bbox, shrink, x, y, w, h)				\
	x = (bbox.ulx + shrink / 2) / shrink - BBOX_UPPER_MARGIN;	\
	y = (bbox.uly + shrink / 2) / shrink - BBOX_UPPER_MARGIN;	\
	w = (bbox.lrx - bbox.ulx + shrink - 1) / shrink			\
	    + BBOX_UPPER_MARGIN + BBOX_LOWER_MARGIN + 1;		\
	h = (bbox.lry - bbox.uly + shrink - 1) / shrink			\
	    + BBOX_UPPER_MARGIN + BBOX_LOWER_MARGIN + 2;

/* access from drawing routines */
static struct word_info *m_info = NULL;
static int m_match_page = -1;

/* Force restarting search if user switches pages. E.g. if
   the current match is the last of 3 matches on the page,
   and user re-selects the page, the search should start again
   with match 1. Similarly, if current page is second of two
   pages scanned, and user clicks on first, search shouldn't continue
   on second page but restart on current one.
*/
static Boolean m_changed_page = False;

static Boolean m_highlight_region_changed = True;

void
search_signal_page_changed(void)
{
    m_changed_page = True;
}


static void
reset_info(struct word_info *w_info)
{
    if (w_info == NULL)
	return;

    free(w_info->txt_buf);
    w_info->txt_buf = NULL;
    w_info->txt_buf_size = 0;
    w_info->curr_buf_idx = 0;

    free(w_info->bboxes);
    w_info->bboxes = NULL;
    w_info->bboxes_size = 0;
    w_info->bboxes_idx = 0;

    m_highlight_region_changed = True;
}


static void
generate_highlight_expose(const struct word_info *info)
{
    int x1 = INT_MAX, y1 = INT_MAX, x2 = 0, y2 = 0;
    size_t i;
    for (i = 0; info->bboxes != NULL && i <= info->bboxes_idx && info->bboxes[i].ulx < INT_MAX; i++) {
	if (info->bboxes[i].ulx < x1)
	    x1 = info->bboxes[i].ulx;
	if (info->bboxes[i].uly < y1)
	    y1 = info->bboxes[i].uly;
	if (info->bboxes[i].lrx > x2)
	    x2 = info->bboxes[i].lrx;
	if (info->bboxes[i].lry > y2)
	    y2 = info->bboxes[i].lry;
    }
#if 0
    fprintf(stderr, "region for exposure: %d,%d to %d,%d\n",
	    x1, y1, x2, y2);
#endif /* 0 */
    x1 -= MAX(BBOX_UPPER_MARGIN, BBOX_LOWER_MARGIN) - 2;
    y1 -= MAX(BBOX_UPPER_MARGIN, BBOX_LOWER_MARGIN) - 2;
    x2 += MAX(BBOX_UPPER_MARGIN, BBOX_LOWER_MARGIN) + 2;
    y2 += MAX(BBOX_UPPER_MARGIN, BBOX_LOWER_MARGIN) + 2;
    clearexpose(&mane, x1, y1, x2 - x1, y2 - y1);
}

static void
draw_bboxes(const struct word_info *info)
{
    size_t i;
    static GC bboxGC = 0;
    static Boolean highlight_inverted_bak;

    if (bboxGC != 0 && highlight_inverted_bak != resource.match_highlight_inverted) {
	XFreeGC(DISP, bboxGC);
	bboxGC = 0;
    }
    highlight_inverted_bak = resource.match_highlight_inverted;
    
    if (resource.match_highlight_inverted) {
	if (MAGNIFIER_ACTIVE && !INSIDE_MANE_WIN)
	    return;
	
	TRACE_FIND((stderr, "-- EXPOSED region: x %d, y %d, w %d, h %d",
		    globals.win_expose.min_x, globals.win_expose.min_y, globals.win_expose.max_x - globals.win_expose.min_x, globals.win_expose.max_y - globals.win_expose.min_y));
	
	if (bboxGC == 0) {
	    XGCValues values;
	    unsigned long valuemask;
	    
	    values.function = GXinvert; /* was: GXxor; see bug #850788 */
	    if (values.function == GXinvert) {
		valuemask = GCFunction;
	    }
	    else {
		values.foreground = WhitePixelOfScreen(SCRN) ^ BlackPixelOfScreen(SCRN);
		/* 		fprintf(stderr, "foreground: 0x%lx, white pixel: 0x%lx, black pixel: 0x%lx\n", */
		/* 			values.foreground, WhitePixelOfScreen(SCRN), BlackPixelOfScreen(SCRN)); */
		valuemask = GCFunction | GCForeground;
	    }
	    bboxGC = XCreateGC(DISP, XtWindow(globals.widgets.top_level), valuemask, &values);
	}
    }
    else if (bboxGC == 0) {
	XGCValues values;
	values.function = GXcopy;
	values.line_width = 2;
	values.cap_style = CapRound;
	values.foreground = resource.hl_Pixel;
	values.background = resource.back_Pixel;
	bboxGC = XCreateGC(DISP, XtWindow(globals.widgets.top_level),
			   GCFunction | GCLineWidth | GCCapStyle | GCForeground | GCBackground, &values);
    }

    TRACE_FIND((stderr, "bboxes_idx: %lu", (unsigned long)info->bboxes_idx));
    for (i = 0; info->bboxes != NULL && i <= info->bboxes_idx && info->bboxes[i].ulx < INT_MAX; i++) {
	int x, y, w, h;

	if (info->bboxes[i].ulx > 10000 || info->bboxes[i].uly > 10000) {
	    TRACE_FIND((stderr, "skipping box: x %d, y %d",
			info->bboxes[i].ulx, info->bboxes[i].uly));
 	    continue;
	}
	
	GET_BBOX(info->bboxes[i], currwin.shrinkfactor, x, y, w, h);
	TRACE_FIND((stderr, "DRAWING box: x %d, y %d, w %d, h %d; shrink: %d",
		    x, y, w, h, currwin.shrinkfactor));
	if (resource.match_highlight_inverted) {
	    if (clip_region(&x, &y, &w, &h)) {
		TRACE_FIND((stderr, "CLIPPED box: x %d, y %d, w %d, h %d",
			    x, y, w, h));
		TEST_DELAY("Redrawing BBOX ............... ");
		XFillRectangle(DISP, mane.win, bboxGC, x, y, w, h);
		TEST_DELAY("Inverting BBOX ............... ");
	    }
	}
	/* 	XDrawRectangle(DISP, mane.win, globals.gc.high, x, y, w + 0, h + 0); */
	/* 	clearexpose(&mane, x, y, w + 3, h + 3); */
	/* 	XDrawRectangle(DISP, mane.win, globals.gc.high, x, y, w + 2, h + 2); */
	/*         XDrawRectangle(DISP, mane.win, globals.gc.high, x + 1, y + 1, w, h); */
	else {
	    static XPoint points[5] = { {0,0}, {0,0}, {0,0}, {0,0}, {0,0} };
	    
	    h++;
	    
	    points[0].x = x;
	    points[0].y = y;
	    
	    points[1].x = w;
	    
	    points[2].y = h;
	    
	    points[3].x = -w;
	    
	    points[4].y = -h;
	    
	    XDrawLines(DISP, mane.win, bboxGC, points, 5, CoordModePrevious);
	}

	if (!MAGNIFIER_ACTIVE) {
	    /* note: inverted y_max y_min, since we want to display the full box here */
	    scroll_page_if_needed(x + w, x, y + h, y);
	}
    }
}

/* return true if we have a match on page pageno */
Boolean
search_have_match(int pageno)
{
    /*     fprintf(stderr, "pageno: %d --- m_info: %p\n", pageno, m_info); */
    /*     fprintf(stderr, "m_match_page: %d\n", m_match_page); */
    return m_info != NULL && m_match_page == pageno;
}

/* TODO: Speed this up, by doing something like bsearch? */
/*
  Returns 2 if we're at the 1st or last character of the bbox, 1 if
  inside the bbox, and 0 else (currently the return value 2 is unused).
*/
int
search_inside_bbox_match(int x, int y)
{
    size_t i;

    ASSERT(m_info != NULL, "inside_match musn't be called with m_info == NULL!");
    TRACE_FIND((stderr, "m_info->bboxes: %p; idx: %lu", (void *)m_info->bboxes, (unsigned long)m_info->bboxes_idx));
    for (i = 0; m_info->bboxes != NULL && i <= m_info->bboxes_idx && m_info->bboxes[i].ulx < INT_MAX; i++) {
	TRACE_FIND((stderr, "inside_bbox_match: %d, %d", x, y));
	TRACE_FIND((stderr, "x: %d, y: %d, x2: %d, y2: %d",
		    (int)(m_info->bboxes[i].ulx / (double)currwin.shrinkfactor + 0.5),
		    (int)(m_info->bboxes[i].uly / (double)currwin.shrinkfactor + 0.5),
		    (int)(m_info->bboxes[i].lrx / (double)currwin.shrinkfactor + 0.5),
		    (int)(m_info->bboxes[i].lry / (double)currwin.shrinkfactor + 0.5)));

	if (x >= (int)(m_info->bboxes[i].ulx / (double)currwin.shrinkfactor + 0.5)
	    && x <= (int)(m_info->bboxes[i].lrx / (double)currwin.shrinkfactor + 0.5)
	    && y >= (int)(m_info->bboxes[i].uly / (double)currwin.shrinkfactor + 0.5)
	    && y <= (int)(m_info->bboxes[i].lry / (double)currwin.shrinkfactor + 0.5)) {
	    TRACE_FIND((stderr, "%d == %d",
			x - 1, (int)(m_info->bboxes[i].ulx / (double)currwin.shrinkfactor + 0.5)));
	    TRACE_FIND((stderr, "%d == %d",
			y - 1, (int)(m_info->bboxes[i].uly / (double)currwin.shrinkfactor + 0.5)));
	    if (x - 1 == (int)(m_info->bboxes[i].ulx / (double)currwin.shrinkfactor + 0.5)
		&& y - 1 == (int)(m_info->bboxes[i].uly / (double)currwin.shrinkfactor + 0.5))
		return 2;
	    else
		return 1;
	}
    }
    return 0;
}

void
search_draw_inverted_regions(void)
{
    if (m_info != NULL) {
	draw_bboxes(m_info);
    }
}


/*
 * shift everything in info down by 1 pages, so that info for page 1 becomes
 * info for page 0.
 */
static void
shift_info_down(struct search_info *searchinfo, struct word_info *info, struct page_mapping *page_mapping)
{
    size_t initial_offset = 0;
    if (page_mapping[0].offset != -1) { /* remember value */
	initial_offset = page_mapping[0].offset;
    }
    
    if (globals.debug & DBG_FIND) {
	int i;
	fprintf(stderr, "current page_mapping:\n");
	for (i = 0; i < 2; i++) {
	    fprintf(stderr, "%d: %d\n", page_mapping[i].pageno, page_mapping[i].offset);
	}
    }
    
    TRACE_FIND((stderr, "initial offset: %lu", (unsigned long)initial_offset));

    page_mapping[0].offset = page_mapping[1].offset - initial_offset;
    page_mapping[0].pageno = page_mapping[1].pageno;
    ASSERT(page_mapping[0].offset >= 0, "new index mustn't be negative!\n");
    page_mapping[1].offset = page_mapping[1].pageno = -1;
    
    TRACE_FIND((stderr, "new val at 0: %d; curr_idx: %lu, len: %lu",
		page_mapping[0].offset,
		(unsigned long)(info->curr_buf_idx - initial_offset),
		(unsigned long)(strlen(info->txt_buf + initial_offset))));

    /* move the text buffer down, and update its current index accordingly */
    ASSERT(info->curr_buf_idx >= initial_offset, "info->curr_buf_idx mustn't become negative!");
    memmove(info->txt_buf, info->txt_buf + initial_offset, info->curr_buf_idx - initial_offset + 1);
    info->curr_buf_idx -= initial_offset;
    TRACE_FIND((stderr, "new index: %lu", (unsigned long)info->curr_buf_idx));

    searchinfo->from_pos -= initial_offset;
    searchinfo->to_pos -= initial_offset;
    if (searchinfo->from_pos < 0)
	searchinfo->from_pos = 0;
    if (searchinfo->to_pos < 0)
	searchinfo->to_pos = 0;
    TRACE_FIND((stderr, "new offsets: from=%d, to=%d", searchinfo->from_pos, searchinfo->to_pos));
}

/*
 * shift everything in info up by 1 page, so that info for page 0 becomes info
 * for page 1.
 */
static void
shift_info_up(struct word_info *info, struct page_mapping *page_mapping)
{
    if (globals.debug & DBG_FIND) {
	int i;
	fprintf(stderr, "current page_mapping:\n");
	for (i = 0; i < 2; i++) {
	    fprintf(stderr, "%d: %d\n", page_mapping[i].pageno, page_mapping[i].offset);
	}
    }
    
    ASSERT(page_mapping[0].offset > 0, "info->curr_buf_idx mustn't become negative!");
    info->curr_buf_idx = page_mapping[0].offset;
    TRACE_FIND((stderr, "new index: %lu", (unsigned long)info->curr_buf_idx));

    page_mapping[1].offset = page_mapping[0].offset;
    page_mapping[1].pageno = page_mapping[0].pageno;
    page_mapping[0].offset = page_mapping[0].pageno = -1;
    
}

static void
append_to_info(struct word_info *info, const char *str)
{
    size_t len = strlen(str);
    size_t new_size = info->txt_buf_size;
    while (len >= new_size) { /* space for trailing 0 */
	new_size++;
    }
    if (new_size > info->txt_buf_size) {
	info->txt_buf_size = new_size;
	info->txt_buf = xrealloc(info->txt_buf, sizeof *(info->txt_buf) * info->txt_buf_size);
    }
    memcpy(info->txt_buf + info->curr_buf_idx, str, len + 1); /* also copy trailing 0 */
    info->curr_buf_idx += len;
}

/* append info2 to info1, enlarging info1 as needed. */
static void
append_info(struct word_info *info1, const struct word_info *info2)
{
    if (info2->txt_buf_size > 0) {
	info1->txt_buf = xrealloc(info1->txt_buf, sizeof *(info1->txt_buf)
				  * (info1->txt_buf_size + info2->txt_buf_size));
	memcpy(info1->txt_buf + info1->curr_buf_idx, info2->txt_buf, info2->txt_buf_size);
	info1->txt_buf_size += info2->txt_buf_size;
	info1->curr_buf_idx += info2->curr_buf_idx;
    }
}

/* prepend info1 to info2, enlarging info2 as needed. */
static void
prepend_info(const struct word_info *info1, struct word_info *info2)
{
    if (info1->txt_buf_size > 0) {
	info2->txt_buf = xrealloc(info2->txt_buf, sizeof *(info2->txt_buf)
				  * (info2->txt_buf_size + info1->txt_buf_size + 1));
	memmove(info2->txt_buf + info1->curr_buf_idx, info2->txt_buf, info2->curr_buf_idx);
	info2->txt_buf[info1->curr_buf_idx + info2->curr_buf_idx] = '\0';
	memcpy(info2->txt_buf, info1->txt_buf, info1->curr_buf_idx);
	/* 	TRACE_FIND((stderr, "prepend_info: info2->txt_buf is: |%s|", info2->txt_buf)); */
	info2->txt_buf_size += info1->txt_buf_size;
	info2->curr_buf_idx += info1->curr_buf_idx;
    }
}

#if HAVE_REGEX_H
/* convert the encoding part (between the `.' and the `@') of the
   locale string passed as argument to UTF-8, and return the result
   in a newly allocated string which the caller is responsible for
   free()ing.
*/
static char *
locale_to_utf8(const char *locale)
{
    char *utf8_locale = xstrdup(locale);
    char *ptr;
    if ((ptr = strchr(utf8_locale, '.')) != NULL) {
	char *ptr2, *rest = "";
	if ((ptr2 = strchr(utf8_locale, '@')) != NULL)
	    rest = xstrdup(ptr2);

	utf8_locale = xrealloc(utf8_locale,
			       ptr - utf8_locale + strlen(".utf8") + strlen(rest) + 1);
	*ptr = '\0';
	utf8_locale = strcat(utf8_locale, ".utf8");
	if (ptr2 != NULL) {
	    utf8_locale = strcat(utf8_locale, rest);
	    free(rest);
	}
    }
    else {
	utf8_locale = xstrcat(utf8_locale, ".utf8");
    }
    return utf8_locale;
}
#endif /* HAVE_REGEX_H */

static Boolean
is_utf8_ideograph(const unsigned char *p)
{
    int len;
    uint32_t ucs4;
    
    if ((len = utf8_to_ucs4((const char *)p, &ucs4, strlen((const char*)p))) <= 0)
	return False;
    
    return is_ideograph(ucs4);
}


static void
dump_buffer(const struct word_info *info, size_t offset, FILE *fp, outputFormatT fmt)
{
    size_t i = offset, len, tot_len;

    if (info->txt_buf == NULL)
	return;
    
    tot_len = strlen(info->txt_buf);
    while (i < tot_len) {
	if (fmt == FMT_UTF8) { /* just dump as-is */
	    fputc(info->txt_buf[i++], fp);
	}
	else { /* convert to iso-latin1, rendering unknown characters as `?' */
	    uint32_t ucs4;
	    const char *ret;

	    /* first apply normalization heurisitcs also used by search */
	    len = utf8_to_ucs4(info->txt_buf + i, &ucs4, strlen(info->txt_buf + i));
	    if ((ret = search_normalize_chars(ucs4)) != NULL)
		fputs(ret, fp);
	    else if (ucs4 <= 0xff) /* in iso-latin1 range */
		fputc((unsigned char)ucs4, fp);
	    else
		fprintf(fp, "\\%.4lX", (unsigned long)ucs4);
	    i += len;
	}
    }
}

static void
scan_page(FILE *fp, int pageno, struct word_info *w_info)
{
    off_t pos_save;
    static ubyte my_scan_buffer[DVI_BUFFER_LEN];
    struct drawinf currinf_save;
    struct scan_info info;
    ubyte maxchar_save;

    reinit_text_scan(); /* to reset scanning heuristics (linebreaks etc.) */
    info.data = (void *)w_info;
    info.geom_special = NULL; /* no procedure here */
    
    /* Save file position */
    pos_save = save_file_status(globals.dvi_file.bak_fp, &currinf_save, &maxchar_save);
    
    lseek(fileno(fp), pageinfo_get_offset(pageno), SEEK_SET);
    memset((char *)&currinf.data, 0, sizeof currinf.data);
    currinf.tn_table_len = TNTABLELEN;
    currinf.tn_table = tn_table;
    currinf.tn_head = tn_head;

    /* point currinf to our own buffer: */
    G_dvi_buf_ptr = my_scan_buffer;
    
    currinf.pos = currinf.end = G_dvi_buf_ptr;
    currinf.virtual = NULL;
    
    geom_scan(text_do_char, fp, &info, pageno);

    /* Restore file status.  */
    restore_file_status(globals.dvi_file.bak_fp, currinf_save, maxchar_save, pos_save);
}


static Boolean
do_scan_page(struct word_info *w_info, struct search_settings *settings,
	     struct page_mapping *page_mapping, int buffer_offset, int pageno,
	     searchDirectionT direction)
{
    struct word_info page_info = { NULL, 0, 0, NULL, 0, 0, NULL, NULL, 0, False, True, False };
    page_info.settings = settings;
    ASSERT(buffer_offset >= 0, "buffer_offset must have been initialized");
    page_info.buffer_offset = buffer_offset;
    TRACE_FIND((stderr, "scanning page: %d; from_pos: %d", pageno, settings->searchinfo->from_pos));

    if (read_events(EV_NOWAIT) & EV_GE_FIND_CANCEL) {
	TRACE_FIND((stderr, "interrupted!!"));
	return False;
    }
    
    scan_page(globals.dvi_file.bak_fp, pageno, &page_info);

    /*     TRACE_FIND((stderr, "scanned buffer of length %d on page %d; contents: |%s|", */
    /* 		page_info.curr_buf_idx, pageno, page_info.txt_buf)); */
    
    /* terminate page */
    append_to_info(&page_info, "\n");
    if (direction == SEARCH_DOWN) {
	/* append to existing info */
	append_info(w_info, &page_info);
	page_mapping->offset = w_info->curr_buf_idx;
    }
    else {
	prepend_info(&page_info, w_info);
	/* 	TRACE_FIND((stderr, "buffer after prepending: |%s|", w_info->txt_buf)); */
	page_mapping->offset = page_info.curr_buf_idx;
    }
    page_mapping->pageno = pageno;
    
    return True;
}

#if HAVE_REGEX_H
static void
report_regexp_error(int errcode, regex_t *regex, const char *term, int flag)
{
    size_t n = regerror(errcode, regex, NULL, 0);
    char *err_buf = xmalloc(n);
    regerror(errcode, regex, err_buf, n);
    xdvi_bell();
    popup_message(XtNameToWidget(globals.widgets.top_level, "*find_popup"),
		  MSG_ERR,
		  NULL,
		  "Could not %s regular expression \"%s\": %s\n",
		  flag == 0 ? "compile" : "execute",
		  term, err_buf);
    free(err_buf);
}
#endif /* HAVE_REGEX_H */

static void
try_match(const struct word_info *info,
	  const struct search_settings *settings)
{
    char *res = NULL;
    size_t from_pos = 0;
    struct search_info *searchinfo = settings->searchinfo;
    
    ASSERT(info->curr_buf_idx == strlen(info->txt_buf), "");
    TRACE_FIND((stderr, "buffer index: %lu, len: %lu; from: %lu",
		(unsigned long)info->curr_buf_idx,
		(unsigned long)strlen(info->txt_buf),
		(unsigned long)searchinfo->from_pos));
    if (searchinfo->from_pos != -1)
	from_pos = searchinfo->from_pos;
    
    if (settings->direction == SEARCH_DOWN) {
	TRACE_FIND((stderr, "trying to match |%s| from %lu; total: %lu",
		    settings->utf8_term,
		    (unsigned long)from_pos,
		    (unsigned long)strlen(info->txt_buf)));

	res = strstr(info->txt_buf + from_pos, settings->utf8_term);
    }
    else {
	size_t curr_pos = 0;
	char *res_bak = NULL;

	TRACE_FIND((stderr, "UP; trying to match |%s| from %lu, %lu",
		    settings->utf8_term,
		    (unsigned long)curr_pos, (unsigned long)from_pos));
	TRACE_FIND((stderr, "buf: |%s|", info->txt_buf + curr_pos));
	while (curr_pos <= from_pos) {
	    res_bak = res;
	    res = strstr(info->txt_buf + curr_pos, settings->utf8_term);
	    if (res != NULL) {
		curr_pos = res - info->txt_buf + strlen(settings->utf8_term);
	    }
	    else {
		break;
	    }
	}
	res = res_bak;
    }
    if (res != NULL) {
	searchinfo->have_match = True;
	searchinfo->from_pos = res - info->txt_buf;
	searchinfo->to_pos = searchinfo->from_pos + strlen(settings->utf8_term);
    }
    else {
	searchinfo->have_match = False;
    }
}

#if HAVE_REGEX_H
static Boolean
try_regexp_match(regex_t *regex,
		 const struct word_info *info,
		 const struct search_settings *settings)
{
    int have_match;
    regmatch_t re_match;
    size_t from_pos = 0;
    Boolean retval = True;

    struct search_info *searchinfo = settings->searchinfo;
    
#if SWITCH_TO_UTF8
    /* switch to UTF-8 encoding, as for regcomp() */
    char *utf8_locale = locale_to_utf8(globals.orig_locale);
    TRACE_FIND((stderr, "current locale: |%s|, utf8 version: |%s|", globals.orig_locale, utf8_locale));
    setlocale(LC_ALL, utf8_locale);
#endif

    re_match.rm_so = re_match.rm_eo = -1;
    
    if (searchinfo->from_pos != -1)
	from_pos = searchinfo->from_pos;
    
    if (settings->direction == SEARCH_DOWN) {
	Boolean search_again = True;
	
	while (search_again && from_pos < info->curr_buf_idx) {
	    searchinfo->have_match = False;
	    search_again = False;

	    /* 	    TRACE_FIND((stderr, "search string: |%s|, from_pos: %d", info->txt_buf + from_pos, from_pos)); */
	    have_match = regexec(regex, info->txt_buf + from_pos, 1, &re_match, 0);

	    if (have_match == 0) { /* match */
		TRACE_FIND((stderr, "match from %d to %d", (int)re_match.rm_so, (int)re_match.rm_eo));
		if (re_match.rm_so == re_match.rm_eo) { /* skip zero-length matches */
		    from_pos += re_match.rm_so + 1;
		    search_again = True;
		}
		else {
		    searchinfo->from_pos = re_match.rm_so + from_pos;
		    searchinfo->to_pos = re_match.rm_eo + from_pos;
		    searchinfo->have_match = True;
		}
	    }
	    else if (have_match != REG_NOMATCH) { /* error */
		report_regexp_error(have_match, regex, settings->term, 1);
		retval = False;
		break;
	    }
	}
    }
    else { /* searching backwards */
	size_t curr_pos = 0, prev_pos = 0;
	regmatch_t re_match_bak;
	Boolean search_again = True;
	searchinfo->have_match = False;
	TRACE_FIND((stderr, "UP; trying to match |%s| from %lu, %lu",
		    settings->utf8_term,
		    (unsigned long)curr_pos, (unsigned long)from_pos));

	re_match_bak.rm_so = re_match_bak.rm_eo = -1;
	
	while (search_again && curr_pos <= from_pos) {
	    search_again = False;

	    /* remember previous match data */
	    re_match_bak.rm_so = prev_pos + re_match.rm_so;
	    re_match_bak.rm_eo = prev_pos + re_match.rm_eo;

	    TRACE_FIND((stderr, "backup values: %d, %d; curr_pos: %lu",
			(int)re_match_bak.rm_so, (int)re_match_bak.rm_eo, (unsigned long)curr_pos));

	    /* 	    TRACE_FIND((stderr, "text buf: |%s|", info->txt_buf + curr_pos)); */
	    have_match = regexec(regex, info->txt_buf + curr_pos, 1, &re_match, 0);
	    while (have_match == 0 && re_match.rm_so == re_match.rm_eo) { /* skip zero-length matches */
		have_match = regexec(regex, info->txt_buf + ++curr_pos, 1, &re_match, 0);
	    }

	    
	    if (have_match == 0) { /* match */
		TRACE_FIND((stderr, "match from %d to %d", (int)re_match.rm_so, (int)re_match.rm_eo));
		TRACE_FIND((stderr, "updating pos: %lu -> %lu; from_pos: %lu",
			    (unsigned long)curr_pos,
			    (unsigned long)(curr_pos + re_match.rm_eo),
			    (unsigned long)from_pos));
		prev_pos = curr_pos;
		curr_pos += re_match.rm_eo;
		search_again = True;
	    }
	    else if (have_match != REG_NOMATCH) { /* error */
		report_regexp_error(have_match, regex, settings->term, 1);
		retval = False;
		break;
	    }
	}
	if (retval) {
	    re_match.rm_so = re_match_bak.rm_so;
	    re_match.rm_eo = re_match_bak.rm_eo;
	    
	    if (re_match.rm_so != -1) {
		TRACE_FIND((stderr, "remembering from-to: %d - %d", (int)re_match.rm_so, (int)re_match.rm_eo));
		searchinfo->from_pos = re_match.rm_so;
		searchinfo->to_pos = re_match.rm_eo;
		searchinfo->have_match = True;
	    }
	}
    }

#if SWITCH_TO_UTF8
    /* switch back to original encoding */
    setlocale(LC_ALL, globals.orig_locale);
    setlocale(LC_NUMERIC, "C");
    free(utf8_locale);
#endif
    
    return retval;
}
#endif /* HAVE_REGEX_H */

/* Erase exising highlighting. TODO: Move this into dvi-draw so that
   there's no big delay between erasing and drawing the next marker.
*/
static void
erase_match_highlighting(struct word_info *info, Boolean flag)
{
    size_t i;

    if (info == NULL || (MAGNIFIER_ACTIVE && !INSIDE_MANE_WIN))
	return;

    for (i = 0; info->bboxes != NULL && i <= info->bboxes_idx && info->bboxes[i].ulx < INT_MAX; i++) {
	int x, y, w, h;
	GET_BBOX(info->bboxes[i], currwin.shrinkfactor, x, y, w, h);
	if (resource.match_highlight_inverted) {
	    if (flag || m_highlight_region_changed) {
		clearexpose(&mane, x, y, w, h);
	    }
	    else if (clip_region(&x, &y, &w, &h)) {
		TRACE_FIND((stderr, "ERASING box: x %d, y %d, w %d, h %d",
			    x, y, w, h));
		TEST_DELAY("Showing BBOX ............... ");
		XClearArea(DISP, mane.win, x, y, w, h, False);
		TEST_DELAY("Clearing BBOX ............... ");
	    }
	}
	else if (flag) {
	    /* erase only the existing marks, not the entire bounding box, to avoid flashing of
	       the text inside the bounding box. 1 pixel added to w, h just to make sure in case of
	       rounding errors. */
	    h++;
	    clearexpose(&mane, x - 1, y - 1, 2, h + 2);
	    clearexpose(&mane, x - 1, y - 1, w + 2, 2);
	    clearexpose(&mane, x + w - 1, y - 1, 2, h + 2);
	    clearexpose(&mane, x - 1, y + h - 1, w + 2, 2);
	}
    }
    if (flag) {
	reset_info(info);
	info = NULL;
    }
    else
	m_highlight_region_changed = False;
}

static void
highlight_match(struct search_settings *settings,
		const struct word_info *w_info,
		const struct page_mapping *page_mapping)
{
    const int context = 10;
    int match_page, i, j = 0;
    static struct word_info curr_info = { NULL, 0, 0, NULL, 0, 0, NULL, NULL, 0, True, False, False };
    struct search_info *searchinfo = settings->searchinfo;
    Boolean match_wrapped = False;
    
    curr_info.settings = settings;
    curr_info.page_mapping = page_mapping;

    TRACE_FIND((stderr, "MATCH from pos %d to %d:",
		searchinfo->from_pos, searchinfo->to_pos));
    
    if (globals.debug & DBG_FIND) {
	fprintf(stderr, "current page_mapping:\n");
	for (i = 0; i < 2; i++) {
	    fprintf(stderr, "%d: %d\n", page_mapping[i].pageno, page_mapping[i].offset);
	}
    }

    TRACE_FIND((stderr, "to_pos: %d, %d", searchinfo->to_pos, page_mapping[0].offset));
    if (searchinfo->from_pos < page_mapping[0].offset) {
	match_page = page_mapping[0].pageno;
	if (searchinfo->to_pos > page_mapping[0].offset) {
	    match_wrapped = True;
	}
    }
    else {
	if (searchinfo->from_pos >= page_mapping[1].offset) {
	    XDVI_ERROR((stderr, "to_pos (%d) should be smaller than page_mapping[1].offset (%d)!",
			searchinfo->from_pos, page_mapping[1].offset));
	}
	ASSERT(searchinfo->from_pos < page_mapping[1].offset, "to_pos should be smaller than page_mapping[1].offset!");
	match_page = page_mapping[1].pageno;
	if (searchinfo->to_pos > page_mapping[1].offset) {
	    match_wrapped = True;
	}
    }
    if (!settings->isearchterm) {
	if (match_wrapped)
	    statusline_info(STATUS_MEDIUM, "Match from page %d to %d", match_page + 1,  match_page + 2);
	else
	    statusline_info(STATUS_MEDIUM, "Match on page %d", match_page + 1);
    
	if (settings->wrapcnt > 0)
	    statusline_append(STATUS_MEDIUM, " (wrapped)", " (wrapped)");
    }
    if (globals.debug & DBG_FIND) {
	fprintf(stderr, "*** match_page: %d, adding: %d\n", match_page, searchinfo->page_offset);
    
	for (j = searchinfo->from_pos - context; j < searchinfo->from_pos; j++) {
	    if (j >= 0)
		fputc(w_info->txt_buf[j], stderr);
	    else
		fputc(' ', stderr);
	}
	fputs("\n        >>", stderr);
	for (j = searchinfo->from_pos; j < searchinfo->to_pos; j++) {
	    fputc(w_info->txt_buf[j], stderr);
	}
	fputs("<<\n          ", stderr);
	for (j = searchinfo->from_pos; j < searchinfo->to_pos; j++) {
	    fputc(' ', stderr);
	}
	for (j = searchinfo->to_pos; j < (int)w_info->curr_buf_idx; j++) {
	    fputc(w_info->txt_buf[j], stderr);
	    if (w_info->txt_buf[j] == '\n')
		break;
	}
	fputc('\n', stderr);
    }
    
    if (match_page != current_page) {
	goto_page(match_page, resource.keep_flag ? NULL : home, False);
	page_history_insert(match_page);
	/* 	globals.ev.flags |= EV_NEWPAGE; */
    }
    /* don't raise the window - this can obscure the search popup */
    /*     XMapRaised(DISP, XtWindow(globals.widgets.top_level)); */

    /* this erases contents of m_info, so be careful - contents of curr_info
       will be indirectly erased by this ... */
    erase_match_highlighting(m_info, True);
    
    /* now rescan the page to get bounding box info for the match */
    scan_page(globals.dvi_file.bak_fp, match_page, &curr_info);
    m_info = &curr_info;
    
    m_match_page = match_page;
    
    do_autoscroll = True; /* enable scrolling to match */
    /* create an expose event so that the region gets highlighted */
    if (m_info != NULL) {
	generate_highlight_expose(m_info);
    }
}

#if HAVE_REGEX_H

/*
 * A mapping of Perl-style character classes to POSIX-style character classes.
 */
static struct perl2posix_mapping {
    const char *perl_class;
    const char *posix_class;
} perl2posix_map[] = {
    { "\\w", "[[:alnum:]]" },
    { "\\W", "[^[:alnum:]]" },
    { "\\d", "[[:digit:]]" },
    { "\\D", "[^[:digit:]]" },
    { "\\s", "[[:space:]]" },
    { "\\S", "[^[:space:]]" },
    { NULL, NULL } /* terminate */
};

/*
 * Convert `perl_regex', a regexp using Perl-style character classes,
 * to a regexp using POSIX-style character classes. Return the latter
 * in freshly allocated memory, which the caller is responsible to free().
 * Uses the perl2posix_map table.
 */
static char *
perl2posix(const char *perl_regex)
{
    char *posix_regex = xstrdup(perl_regex);
    size_t i;

    for (i = 0; perl2posix_map[i].perl_class != NULL; i++) {
	const char *ptr;
	size_t offset = 0;
	TRACE_FIND((stderr, "searching for |%s| at offset %lu", perl2posix_map[i].perl_class, (unsigned long)offset));
	while ((ptr = find_format_str(posix_regex + offset, perl2posix_map[i].perl_class)) != NULL) {
	    size_t len1 = ptr - posix_regex; /* length up to match */
	    size_t len2 = strlen(perl2posix_map[i].posix_class);
	    TRACE_FIND((stderr, "Regexp: mapping %s to %s",
			perl2posix_map[i].perl_class,
			perl2posix_map[i].posix_class));
	    /* -1 since -2 for perl format string, +1 for trailing '\0' */
	    posix_regex = xrealloc(posix_regex, strlen(posix_regex) + len2 - 1);
	    /* make space for new format string */
	    memmove(posix_regex + len1 + len2, posix_regex + len1 + 2, strlen(posix_regex + len1 + 2) + 1);
	    /* copy in new format string */
	    memcpy(posix_regex + len1, perl2posix_map[i].posix_class, len2);
	    TRACE_FIND((stderr, "Expanded regex: |%s|", posix_regex));
	    offset = len1 + len2;
	    TRACE_FIND((stderr, "searching again for |%s| at offset %lu",
			perl2posix_map[i].perl_class,
			(unsigned long)offset));
	}
    }
    return posix_regex;
}

#endif /* HAVE_REGEX_H */

char *
get_text_selection(int *len, int ulx, int uly, int lrx, int lry)
{
    struct word_info txt_info = { NULL, 0, 0, NULL, 0, 0, NULL, NULL, 0, False, False, True };
    struct bbox text_bbox;
    
    text_bbox.ulx = ulx;
    text_bbox.uly = uly;
    text_bbox.lrx = lrx;
    text_bbox.lry = lry;

    txt_info.bboxes = &text_bbox;

    /* initialize buffer with empty string */
    txt_info.txt_buf_size = 1;
    txt_info.txt_buf = xmalloc(txt_info.txt_buf_size);
    txt_info.txt_buf[0] = '\0';

    /* this enlarges the buffer as needed */
    scan_page(globals.dvi_file.bak_fp, current_page, &txt_info);
    
    *len = txt_info.txt_buf_size;
    return txt_info.txt_buf;
    
#if 0
    /*     fprintf(stderr, "========== SELECTION (len %d):\n", txt_info.curr_buf_idx); */
    /*     dump_buffer(&txt_info, 0, stderr, FMT_ISO_8859_1); */
    /*     fprintf(stderr, "==========\n"); */

    buf = xmalloc(4 * txt_info.curr_buf_idx + 1); /* just in case we get many non-printables */
    while (i < txt_info.curr_buf_idx) {
	uint32_t ucs4;
	const char *ret;
	
	/* first apply normalization heurisitcs also used by search */
	size_t len = utf8_to_ucs4(txt_info.txt_buf + i, &ucs4, strlen(txt_info.txt_buf + i));
	if ((ret = search_normalize_chars(ucs4)) != NULL) {
	    size_t len_ret = strlen(ret);
	    memcpy(buf + offset, ret, len_ret);
	    offset += len_ret;
	}	
	else if (ucs4 <= 0xff) { /* in iso-latin1 range */
	    buf[offset++] = (unsigned char)ucs4;
	}
	else {
	    sprintf(buf + offset, "\\%.4lX", ucs4);
	    offset += 4;
	}
	i += len;
    }
    buf[offset] = '\0';
    free(txt_info.txt_buf);
    return buf;
#endif /* 0 */
}

Boolean
search_extract_text(struct save_or_print_info *info)
{
    int i;
    FILE *fp;
    
    if ((fp = XFOPEN(info->finfo->out_file, "wb")) == NULL) {
	popup_message(globals.widgets.top_level,
		      MSG_ERR,
		      NULL, "Could not open %s for writing: %s.",
		      info->finfo->out_file,
		      strerror(errno));
	return False;
    }

    for (i = 0; i < total_pages; i++) {
	if (info->pinfo->callback == NULL || info->pinfo->callback(info, i)) {
	    struct word_info txt_info = { NULL, 0, 0, NULL, 0, 0, NULL, NULL, 0, False, False, False };
	    scan_page(globals.dvi_file.bak_fp, i, &txt_info);

	    dump_buffer(&txt_info, 0, fp, info->fmt);

	    free(txt_info.txt_buf);
	    
	    fputs("\n\n", fp); /* two newlines for page breaks */
	}
    }
    
    fclose(fp);
    return True;
}

void
search_erase_highlighting(Boolean reset)
{
    erase_match_highlighting(m_info, reset);
}

static void
message_search_ended(XtPointer arg)
{
    struct search_settings *settings = (struct search_settings *)arg;

    settings->searchinfo->from_pos = settings->searchinfo->to_pos = -1;
    TRACE_FIND((stderr, "search ended; current_page: %d", current_page));
    settings->from_page = current_page;
    search_signal_page_changed();
    settings->message_window = 0;
    /*     statusline_append(STATUS_SHORT, " stopped."); */
    if (!settings->isearchterm)
	statusline_info(STATUS_SHORT, "Search stopped.");
}


void
search_restart(XtPointer arg)
{
    struct search_settings *settings = (struct search_settings *)arg;
    Widget popup, textfield;
    char *searchterm = NULL;
    char *textfield_name = NULL;
    
    TRACE_FIND((stderr, "restart search!"));

    /* Update the search term (user might have changed it before
       hitting `Return' to restart the search).  We need to start
       from the toplevel widget since this method may be called
       from a different window (e.g. confirmation popup).
    */
#if defined(MOTIF) && USE_COMBOBOX
    textfield_name = "Text";
#else
    textfield_name = Xdvi_SEARCHBOX_INPUT_NAME;
#endif /* defined(MOTIF) && USE_COMBOBOX */
    if (!settings->isearchterm
	&& get_widget_by_name(&popup, globals.widgets.top_level, Xdvi_SEARCH_POPUP_NAME, True)
	&& get_widget_by_name(&textfield, popup, textfield_name, True)) {
	XtVaGetValues(textfield,
#ifdef MOTIF
		      XmNvalue,
#else
		      XtNstring,
#endif
		      &searchterm, NULL);
	if (searchterm == NULL) {
	    XDVI_WARNING((stderr, "Got NULL searchterm in search_restart()!"));
	    return;
	}
	settings->term = searchterm;
    }

    if (settings->direction == SEARCH_DOWN) {
	settings->from_page = 0;
    }
    else {
	settings->from_page = total_pages - 1;
    }
    if (settings->direction == SEARCH_DOWN)
	settings->searchinfo->from_pos = settings->searchinfo->to_pos = -1;
    else
	settings->searchinfo->from_pos = settings->searchinfo->to_pos = INT_MAX;
    
    settings->message_window = 0;
    search_signal_page_changed();
    search_dvi((XtPointer)settings);
}

static void
normalize_newline(char *str)
{
    size_t i, j;
    for (i = 0, j = 0; str[i] != '\0'; i++, j++) {
	if (str[i] == '\\' && str[i + 1] == 'n') {
	    if (i > 0 && str[i - 1] == '\\') {
		str[j] = 'n';
		i++;
	    }
	    else {
		str[j] = '\n';
		i++;
	    }
	}
	else {
	    str[j] = str[i];
	}
    }
    str[j] = str[i]; /* copy terminating '\0' */
}


static Boolean
reinit_searchterm(struct search_settings *settings, const char *encoding)
{
    struct search_info *searchinfo = settings->searchinfo;

    free(settings->utf8_term);
    settings->utf8_term = NULL;

    if (memicmp(encoding, "iso-8859-1", strlen("iso-8859-1")) == 0
	|| memicmp(encoding, "iso8859-1", strlen("iso8859-1")) == 0) {
	int conv_len = (strlen(settings->term) + 1) * 2;
	int ret_len;
	char *ptr = xmalloc(conv_len);
	if ((ret_len = str_iso_8859_1_to_utf8(settings->term, ptr, conv_len)) < 0) {
	    xdvi_bell();
	    popup_message(XtNameToWidget(globals.widgets.top_level, "*find_popup"),
			  MSG_ERR,
			  NULL,
			  "Shouldn't happen: search term `%s' is too long (> %d)", settings->term, conv_len);
	    searchinfo->locked = False;
	    free(ptr);
	    settings->utf8_term = xstrdup("");
	    return False;
	}
	settings->utf8_term = ptr;
    }
    else if (memicmp(encoding, "utf-8", strlen("utf-8")) == 0
	     || memicmp(encoding, "utf8", strlen("utf8")) == 0) {
	settings->utf8_term = xstrdup(settings->term);
    }
    else {
	if ((settings->utf8_term = iconv_convert_string(encoding, "utf-8", settings->term)) == NULL) {
	    return False;
	}
    }

    TRACE_FIND((stderr, "UTF-8 search term: |%s|", settings->utf8_term));
    normalize_newline(settings->utf8_term);
    TRACE_FIND((stderr, "UTF-8 search term after normalizing newline: |%s|", settings->utf8_term));

#if 0
    /* lowercasing for regexps is dealt with by REG_ICASE */
    if (!settings->case_sensitive && !settings->use_regexp) {
        if (!utf8_lowercase(settings->utf8_term))
            return False;
        TRACE_FIND((stderr, "Lowercased UTF-8 search term: |%s|", settings->utf8_term));
    }
#else /* always lowercase, since REG_ICASE is broken with UTF-8(?) */
    if (!settings->case_sensitive && !utf8_lowercase(settings->utf8_term))
	return False;
    TRACE_FIND((stderr, "Lowercased UTF-8 search term: |%s|", settings->utf8_term));
#endif /* 1 */
    
    /* remove spaces/newlines before/after an ideographic char */
    {
	unsigned char *p = (unsigned char *)settings->utf8_term;
	unsigned char *q;
	int l, len = strlen((char *)p);
	uint32_t ucs4;
	Boolean had_ideograph;
	
	had_ideograph = False;
	while (len > 0) {
	    if (*p == ' ' || *p == '\n') {
		q = p + 1;
		while (*q == ' ' || *q == '\n')
		    q++;
		len -= q - p;
		if (had_ideograph || is_utf8_ideograph(q))
		    memmove(p, q, len + 1);	/* remove spaces/newlines */
		else
		    p = q;			/* preserve spaces/newlines */
	    }
	    else {
		if ((l = utf8_to_ucs4((char *)p, &ucs4, len)) <= 0)
		    break;
		len -= l;
		had_ideograph = is_ideograph(ucs4);
		p += l;
	    }
	}

	settings->utf8_term = xrealloc(settings->utf8_term, strlen(settings->utf8_term) + 1);
    }
    
    return True;
}

void
search_reset_info(void)
{
    TRACE_FIND((stderr, "resetting info!"));
    reset_info(m_info);
    m_info = NULL;
}


#if HAVE_REGEX_H
static Boolean
do_recompile_regexp(struct search_settings *settings,
		    regex_t *regex)
{
    int re_retval;
    /* use extended POSIX, and make `.' not match newlines (otherwise it's
       often too unintuitive because of the greediness of '+'/'*' */
    int re_flags = REG_EXTENDED | REG_NEWLINE;
    TRACE_FIND((stderr, "compiling regexp ..."));
    
    if (settings->posix_term != NULL) {
	TRACE_FIND((stderr, "freeing old regexp ..."));
	free(settings->posix_term);
	regfree(regex);
    }
    settings->posix_term = perl2posix(settings->utf8_term);

#if 0 /* this is broken with e.g. german umlauts; maybe it can't deal with UTF-8 encoding? */
    /* compile regexp from search term */
    if (!settings->case_sensitive)
        re_flags |= REG_ICASE;
#endif
    
    { /* change the encoding part of the locale to UTF-8 to match encoding
	 of the search string and the search buffer */
#if SWITCH_TO_UTF8
	char *utf8_locale = locale_to_utf8(globals.orig_locale);
	TRACE_FIND((stderr, "current locale: |%s|, utf8 version: |%s|", globals.orig_locale, utf8_locale));
        setlocale(LC_ALL, utf8_locale);
#endif
        re_retval = regcomp(regex, settings->posix_term, re_flags);
#if SWITCH_TO_UTF8
        setlocale(LC_ALL, globals.orig_locale);
        setlocale(LC_NUMERIC, "C");
	free(utf8_locale);
#endif
    }

    if (re_retval != 0) {
	report_regexp_error(re_retval, regex, settings->term, 0);
	settings->searchinfo->locked = False;
	return False;
    }

    return True;
}

#else  /* HAVE_REGEX_H */

static void
warn_no_regex(void)
{
    xdvi_bell();
    popup_message(XtNameToWidget(globals.widgets.top_level, "*find_popup"),
		  MSG_WARN,
		  NULL,
		  "POSIX regular expression support (regex.h) is not available on this platform; "
		  "using ordinary string matching instead.");
}
#endif /* HAVE_REGEX_H */

/*
  Scan at most two adjacent pages, writing results in to page_mapping.
  Return False if user cancelled the search, True else.
*/
static Boolean
scan_two_pages(struct search_settings *settings,
	       struct word_info *info,
	       struct page_mapping *page_mapping,
	       int curr_page)
{
    Boolean cancelled = False;
    struct search_info *searchinfo = settings->searchinfo;
    
    if (curr_page != page_mapping[0].pageno
	&& curr_page != page_mapping[1].pageno) { /* none of 2 pages scanned yet */
	reset_info(info);
	if (settings->direction == SEARCH_DOWN) {
	    if (!do_scan_page(info, settings, &(page_mapping[0]),
			      0, curr_page, settings->direction)
		|| (curr_page + 1 < total_pages &&
		    !do_scan_page(info, settings, &(page_mapping[1]),
				  page_mapping[0].offset, curr_page + 1, settings->direction)))
		cancelled = True;
	}
	else {
	    if (!do_scan_page(info, settings, &(page_mapping[1]),
			      0, curr_page, settings->direction)
		|| (curr_page > 0 &&
		    !do_scan_page(info, settings, &(page_mapping[0]),
				  page_mapping[1].offset, curr_page - 1, settings->direction)))
		cancelled = True;
	    else if (page_mapping[0].offset != -1) {
		page_mapping[1].offset += page_mapping[0].offset;
		if (searchinfo->from_pos != INT_MAX)
		    searchinfo->from_pos += page_mapping[0].offset;
	    }
	}
    }
    else if (curr_page != page_mapping[0].pageno) { /* current page scanned as page_mapping[1] */
	if (settings->direction == SEARCH_DOWN && curr_page + 1 < total_pages) {
	    shift_info_down(searchinfo, info, page_mapping);
	    if (!do_scan_page(info, settings, &(page_mapping[1]),
			      page_mapping[0].offset, curr_page + 1, settings->direction))
		cancelled = True;
	}
	else if (curr_page - 1 != page_mapping[0].pageno && curr_page > 0) {
	    if (!do_scan_page(info, settings, &(page_mapping[0]),
			      0, curr_page - 1, settings->direction))
		cancelled = True;
	    else {
		page_mapping[1].offset += page_mapping[0].offset;
		if (searchinfo->from_pos != INT_MAX)
		    searchinfo->from_pos += page_mapping[0].offset;
	    }
	}
    }
    else if (curr_page != page_mapping[1].pageno) { /* current page scanned as page_mapping[0] */
	if (settings->direction == SEARCH_UP && curr_page > 0) {
	    shift_info_up(info, page_mapping);
	    if (!do_scan_page(info, settings, &(page_mapping[0]),
			      0, curr_page - 1, settings->direction))
		cancelled = True;
	    else {
		page_mapping[1].offset += page_mapping[0].offset;
		if (searchinfo->from_pos != INT_MAX) {
		    searchinfo->from_pos += page_mapping[0].offset;
		    searchinfo->to_pos += page_mapping[0].offset;
		}
		TRACE_FIND((stderr, "new offsets: from=%d, to=%d", searchinfo->from_pos, searchinfo->to_pos));
	    }
	}
	else if (curr_page + 1 != page_mapping[1].pageno && curr_page + 1 < total_pages) {
	    if (!do_scan_page(info, settings, &(page_mapping[1]),
			      page_mapping[0].offset, curr_page + 1, settings->direction))
		cancelled = True;
	}
    }
    
    return !cancelled;
}

static void
search_over_pages(struct search_settings *settings,
		  struct page_mapping *page_mapping,
		  struct word_info *w_info,
		  int *from_pos_bak,
		  Boolean adjust_hyphen_offset
#if HAVE_REGEX_H
		  , regex_t *regex
#endif
		  )
{
    int curr_page = settings->from_page; /* page on which we're currently on */
    
    for (;;) {	/* scan pages, try to match */
	if (settings->isearchterm) {
	    const char *prefix = "";
	    if (settings->wrapcnt > 0)
		prefix = "Wrapped ";
	    statusline_info(STATUS_FOREVER, "%sI-search (ESC to exit): %s", prefix, settings->isearchterm);
	}
	else {
	    if (resource.expert_mode & XPRT_SHOW_STATUSLINE) /* too distracting for stdout */
		statusline_info(STATUS_MEDIUM, "Searching on page %d", curr_page);
	}
	TRACE_FIND((stderr, "curr_page: %d, pageno 0: %d, pageno1: %d",
		    curr_page, page_mapping[0].pageno, page_mapping[1].pageno));

	settings->hyphen_delta = 0;
	/* scan_two_pages will return False if user cancelled */
	if (!scan_two_pages(settings, w_info, page_mapping, curr_page)
	    || (read_events(EV_NOWAIT) & EV_GE_FIND_CANCEL)) {
	    if (!settings->isearchterm) {
		if (resource.expert_mode & XPRT_SHOW_STATUSLINE) /* too distracting for stdout */
		    statusline_append(STATUS_SHORT,
				      "- cancelled.",
				      "- cancelled.");
	    }
	    settings->searchinfo->locked = False;
	    return;
	
	}

	TRACE_FIND((stderr, "page mapping:\n%d: %d\n%d: %d",
		    page_mapping[0].pageno, page_mapping[0].offset,
		    page_mapping[1].pageno, page_mapping[1].offset));
	
	/*  	dump_buffer(w_info, 0, stderr, FMT_ISO_8859_1); */

	/* If ignore_hyphens has changed (in which case adjust_hyphen_offset
	   is true), the buffer will contain settings->hyphen_delta fewer
	   or more characters than in the previous pass due to the removal
	   or addition of hyphens; adjust from_pos and to_pos accordingly:
	*/
	if (adjust_hyphen_offset) {
	    TRACE_FIND((stderr, "adjusting offset by %d", settings->hyphen_delta));
	    if (settings->ignore_hyphens) { /* fewer characters */
		settings->searchinfo->from_pos -= settings->hyphen_delta;
		settings->searchinfo->to_pos -= settings->hyphen_delta;
	    }
	    else { /* more characters */
		settings->searchinfo->from_pos += settings->hyphen_delta;
		settings->searchinfo->to_pos += settings->hyphen_delta;
	    }
	    TRACE_FIND((stderr, "NEW from_pos: %d; to_pos: %d",
			settings->searchinfo->from_pos, settings->searchinfo->to_pos));
	}

    	
	/* match the searchstring */
#if HAVE_REGEX_H
	if (settings->use_regexp) {
	    if (!try_regexp_match(regex, w_info, settings)) /* regexp error */
		return;
	    
	}
	else {
#endif
	    try_match(w_info, settings);
#if HAVE_REGEX_H
	}
#endif

	/* again, check if user cancelled */
	if (read_events(EV_NOWAIT) & EV_GE_FIND_CANCEL) { /* user cancelled */
	    if (!settings->isearchterm)
		statusline_append(STATUS_SHORT,
				  "- cancelled.",
				  "- cancelled.");
	    settings->searchinfo->locked = False;
	    return;
	
	}
	
	if (settings->searchinfo->have_match) { /* match, highlight it */
	    highlight_match(settings, w_info, page_mapping);
	    settings->searchinfo->locked = False;
	    if (settings->direction == SEARCH_DOWN && !settings->isearchterm) {
		*from_pos_bak = settings->searchinfo->from_pos;
		settings->searchinfo->from_pos = settings->searchinfo->to_pos;
	    }
	    break;
	}
	else if ((settings->direction == SEARCH_DOWN && curr_page + 1 < total_pages)
		 || (settings->direction == SEARCH_UP && curr_page > 0)) {
	    if (settings->direction == SEARCH_DOWN)
		curr_page++;
	    else
		curr_page--;
	    TRACE_FIND((stderr, "continuing on page %d", curr_page));
	    erase_match_highlighting(m_info, True);
	    /* no match, and we have more pages; continue scanning */
	    continue;
	}
	else { /* reached end of file */
	    Widget find_popup;

	    if (settings->isearchterm) {
		const char *prefix = "Failed";
		fprintf(stderr, "reached end with wrapcnt: %d\n", settings->wrapcnt);
		if (settings->wrapcnt > 0)
		    prefix = "Failed wrapped";
		xdvi_bell();
		statusline_info(STATUS_FOREVER, "%s I-search (ESC to exit, RET to restart): %s", prefix, settings->term);
		settings->searchinfo->locked = False;
		erase_match_highlighting(m_info, True);
		settings->wrapcnt++;
		return;
	    }
	    
	    if (settings->wrap) {
		if (settings->wrapcnt > 0) {
		    positioned_popup_message(XtNameToWidget(globals.widgets.top_level, "*find_popup"),
					     MSG_INFO,
					     settings->x_pos, settings->y_pos,
					     NULL, "Pattern `%s' not found.", settings->term);
		    /* 		    positioned_choice_dialog(XtNameToWidget(globals.widgets.top_level, "*find_popup"), */
		    /* 					     MSG_INFO, */
		    /* 					     settings->x_pos, settings->y_pos, */
		    /* 					     NULL, */
		    /* #ifndef MOTIF */
		    /* 					     NULL, */
		    /* #endif */
		    /* 					     NULL, NULL, /\* pre callback *\/ */
		    /* 					     "OK", message_search_ended, (XtPointer)settings, */
		    /* 					     NULL, NULL, NULL, */
		    /* 					     "Pattern `%s' not found.", settings->term); */
		    settings->searchinfo->locked = False;
		    message_search_ended((XtPointer)settings);
		    return;
		}
		settings->wrapcnt++;
		erase_match_highlighting(m_info, True);
		settings->searchinfo->locked = False;
		search_restart((XtPointer)settings);
		return;
	    }
	    
	    if ((find_popup = XtNameToWidget(globals.widgets.top_level, "*find_popup")) == 0) {
		XDVI_WARNING((stderr, "Couldn't find \"find_popup\" widget!"));
		find_popup = globals.widgets.top_level;
	    }

	    if (!settings->isearchterm)
		statusline_append(STATUS_MEDIUM,
				  "... searched to end of file.",
				  "... searched to end of file.");
	    erase_match_highlighting(m_info, True);
	    settings->searchinfo->locked = False;
	    
	    settings->message_window =
		positioned_choice_dialog(find_popup,
					 MSG_QUESTION,
					 settings->x_pos, settings->y_pos,
					 NULL,
#ifndef MOTIF
					 "do-search-restart",
#endif
					 NULL, NULL, /* no pre_callbacks */
					 "Yes", search_restart, (XtPointer)settings,
					 "Cancel", message_search_ended, (XtPointer)settings,
					 "Searched %s of file without finding the pattern.\n"
					 "Start again from the %s of the file?",
					 settings->direction == SEARCH_DOWN
					 ? "to end" : "to beginning",
					 settings->direction == SEARCH_DOWN
					 ? "beginning" : "end");
	    TRACE_GUI((stderr, "message_window: %p\n", (void *)settings->message_window));
	    /* notreached */
	    break;
	}
    } /* for(;;) */
}

void
search_dvi(XtPointer arg)
{
    struct search_settings *settings = (struct search_settings *)arg;
    struct search_info *searchinfo = settings->searchinfo;
    
#if HAVE_REGEX_H
    static regex_t regex;
#endif /* HAVE_REGEX_H */
    
    /* a mapping of page numbers to index positions in w_info->txt_buf,
       for the 2 pages that we scanned */
    static struct page_mapping page_mapping[2] = { { -1, -1 }, { -1, -1 } };
    
    static struct word_info w_info = { NULL, 0, 0, NULL, 0, 0, NULL, NULL, 0, False, False, False };

    static time_t dvi_time_bak = 0;
    static char *searchterm_bak = NULL;

    static const char *text_encoding = NULL;

    static Boolean case_sensitive_bak = False;
    static Boolean ignore_hyphens_bak = False;
    static Boolean ignore_linebreaks_bak = False;
    static searchDirectionT direction_bak = SEARCH_UNINITIALIZED;
    /* from_pos_bak is needed to mark the start of the match when switching from
       search down to search backwards, where we want to jump to the previous match,
       so we need the start of the current match again (and down search sets
       searchinfo->from_pos = searchinfo->to_pos) */
    static int from_pos_bak = -1;
    /*      int curr_page; */

    Boolean reinit = False;
    Boolean recompile_regexp = False;
    Boolean adjust_hyphen_offset = False;
    
    /* prevent multiple invocations while still busy searching */
    if (searchinfo->locked) {
	TRACE_FIND((stderr, "LOCKED"));
	return;
    }
    searchinfo->locked = True;

    if (dvi_time_bak == 0) { /* first invocation */
	case_sensitive_bak = settings->case_sensitive;
	ignore_hyphens_bak = settings->ignore_hyphens;
	ignore_linebreaks_bak = settings->ignore_linebreaks;
	dvi_time_bak = globals.dvi_file.time;
    }
    if (globals.dvi_file.time > dvi_time_bak) {
	dvi_time_bak = globals.dvi_file.time;
	reinit = True;
    }

    m_match_page = -1;
    
    if (raise_message_windows()) { /* any popups user still needs to deal with? */
	TRACE_GUI((stderr, "Still open message windows to deal with, returning ..."));
	searchinfo->locked = False;
	return;
    }

    ASSERT(settings->term != NULL, "settings->term mustn't be NULL!");
    if (strlen(settings->term) == 0) {
	if (settings->isearchterm) {
	    settings->searchinfo->locked = False;
	    return;
	}
	
	positioned_popup_message(XtNameToWidget(globals.widgets.top_level, "*find_popup"),
				 MSG_ERR,
				 settings->x_pos, settings->y_pos,
				 NULL, "Empty search term");
	searchinfo->locked = False;
	return;
    }
    
    TRACE_FIND((stderr, "dvi_search: Searching for |%s| from page %d", settings->term, settings->from_page));
    TRACE_FIND((stderr, "  settings: down = %d, re = %d, case = %d",
		settings->direction, settings->use_regexp, settings->case_sensitive));

    if (m_changed_page) {
	m_changed_page = False;
	reinit = True;
    }
    
    /* initialize text_encoding */
    if (text_encoding == NULL) {
	text_encoding = get_text_encoding();
    }
    
    /* first call to this routine, or search term changed:
       Re-initialize the utf-8 representation and the regexp */
    if (settings->utf8_term == NULL
	|| searchterm_bak == NULL
	|| strcmp(settings->term, searchterm_bak) != 0) {

	if (!reinit_searchterm(settings, text_encoding)) {
	    searchinfo->locked = False;
	    return;
	}

	free(searchterm_bak);
	searchterm_bak = xstrdup(settings->term);
	recompile_regexp = True;
    }

    if (strlen(settings->utf8_term) == 0) {
	positioned_popup_message(XtNameToWidget(globals.widgets.top_level, "*find_popup"),
				 MSG_WARN,
				 settings->x_pos, settings->y_pos,
				 NULL, "Search term is empty after UTF-8 conversion!");
	searchinfo->locked = False;
	return;
    }

    if (direction_bak != settings->direction && direction_bak != SEARCH_UNINITIALIZED
	&& searchinfo->from_pos != INT_MAX && searchinfo->from_pos != -1) {
	TRACE_FIND((stderr, "changed direction! from_pos: %d", searchinfo->from_pos));
	if (settings->direction == SEARCH_DOWN) {
	    searchinfo->from_pos = searchinfo->to_pos;
	    TRACE_FIND((stderr, "DOWN; new from_pos: %d", searchinfo->from_pos));
	}
	else if (settings->direction == SEARCH_UP) {
	    if (from_pos_bak != -1) {
		searchinfo->from_pos = from_pos_bak;
	    }
	    else {
		searchinfo->from_pos = 0;
	    }
	    TRACE_FIND((stderr, "UP; new from_pos: %d", searchinfo->from_pos));
	}
    }
    direction_bak = settings->direction;

    /* If one of the settings for case, hyphens ore linebreaks has
     * changed, we need to rescan the page to undo lowercasing,
     * hyphenation or linebreak removal in w_info, but preserve
     * searchinfo->from_pos since user will want to find next match.
     */
    if (case_sensitive_bak != settings->case_sensitive
	|| ignore_hyphens_bak != settings->ignore_hyphens
	|| ignore_linebreaks_bak != settings->ignore_linebreaks) {

	if (ignore_hyphens_bak != settings->ignore_hyphens)
	    adjust_hyphen_offset = True;
	case_sensitive_bak = settings->case_sensitive;
	ignore_hyphens_bak = settings->ignore_hyphens;
	ignore_linebreaks_bak = settings->ignore_linebreaks;
	
	reset_info(&w_info);

	/* adjust from_pos if it's on second page (we'll rescan it as first page) */
	if (searchinfo->from_pos >= page_mapping[0].offset)
	    searchinfo->from_pos -= page_mapping[0].offset;

	TRACE_FIND((stderr, "from_pos: %d", searchinfo->from_pos));
	page_mapping[0].offset = page_mapping[1].offset = page_mapping[0].pageno = page_mapping[1].pageno = -1;
	/* also need to recompile regexp, and undo lowercasing of search term */
	if (!reinit_searchterm(settings, text_encoding))
	    return;
	recompile_regexp = True;
    }
    
    if (reinit) { /* file changed, or on different page: re-initialize scan info */
	TRACE_FIND((stderr, "re-initializing scan info!"));
	page_mapping[0].offset = page_mapping[1].offset = page_mapping[0].pageno = page_mapping[1].pageno = -1;

	/* re-initialize info */
	reset_info(&w_info);
	if (settings->direction == SEARCH_DOWN)
	    searchinfo->from_pos = searchinfo->to_pos = 0;
	else
	    settings->searchinfo->from_pos = settings->searchinfo->to_pos = INT_MAX;
    }

    TRACE_FIND((stderr, "from_pos initialized with: %d", settings->searchinfo->from_pos));
    
    if (settings->use_regexp && recompile_regexp) {
#if HAVE_REGEX_H
	if (!do_recompile_regexp(settings, &regex))
	    return;
#else
	warn_no_regex();
#endif /* HAVE_REGEX_H */
    }

    /* do it */
    search_over_pages(settings, page_mapping, &w_info, &from_pos_bak, adjust_hyphen_offset
#if HAVE_REGEX_H
		      , &regex
#endif
		      );
}

static void cb_isearch(Widget w, XtPointer closure, XEvent *event, Boolean *cont);
	
static void
do_isearch_cancel(struct search_settings *settings)
{
    static const char default_key_translations[] =
	"\"0\":digit(0)\n"
	"\"1\":digit(1)\n"
	"\"2\":digit(2)\n"
	"\"3\":digit(3)\n"
	"\"4\":digit(4)\n"
	"\"5\":digit(5)\n"
	"\"6\":digit(6)\n"
	"\"7\":digit(7)\n"
	"\"8\":digit(8)\n"
	"\"9\":digit(9)\n"
	/* 		"\"-\":minus()\n" */
	"<Motion>:motion()\n";
    
    static const char default_mouse_translations[] =
	"<BtnUp>:release()";

    
    statusline_info(STATUS_SHORT, "I-search stopped.");

    free(settings->isearchterm);
    settings->isearchterm = NULL;
    settings->searchinfo->from_pos = settings->searchinfo->to_pos = 0;
    settings->from_page = current_page;
    erase_match_highlighting(m_info, True);

	

    /* restore translations */
    XtOverrideTranslations(globals.widgets.top_level, XtParseTranslationTable(base_key_translations));
    XtOverrideTranslations(globals.widgets.top_level, XtParseTranslationTable(default_key_translations));
    XtOverrideTranslations(globals.widgets.top_level, XtParseTranslationTable(base_mouse_translations));
    XtOverrideTranslations(globals.widgets.top_level, XtParseTranslationTable(default_mouse_translations));

    XtOverrideTranslations(globals.widgets.draw_widget, XtParseTranslationTable(base_key_translations));
    XtOverrideTranslations(globals.widgets.draw_widget, XtParseTranslationTable(default_key_translations));
    XtOverrideTranslations(globals.widgets.draw_widget, XtParseTranslationTable(base_mouse_translations));
    XtOverrideTranslations(globals.widgets.draw_widget, XtParseTranslationTable(default_mouse_translations));

    XtOverrideTranslations(globals.widgets.clip_widget, XtParseTranslationTable(base_key_translations));
    XtOverrideTranslations(globals.widgets.clip_widget, XtParseTranslationTable(default_key_translations));
    XtOverrideTranslations(globals.widgets.clip_widget, XtParseTranslationTable(base_mouse_translations));
    XtOverrideTranslations(globals.widgets.clip_widget, XtParseTranslationTable(default_mouse_translations));

    if (resource.main_translations != NULL) {
	XtOverrideTranslations(globals.widgets.draw_widget, XtParseTranslationTable(resource.main_translations));
	XtOverrideTranslations(globals.widgets.clip_widget, XtParseTranslationTable(resource.main_translations));
    }

    XtRemoveEventHandler(globals.widgets.top_level, KeyPressMask|KeyReleaseMask, False,
			 cb_isearch, (XtPointer)settings);
    XtRemoveEventHandler(globals.widgets.draw_widget, KeyPressMask|KeyReleaseMask, False,
			 cb_isearch, (XtPointer)settings);
    XtRemoveEventHandler(globals.widgets.clip_widget, KeyPressMask|KeyReleaseMask, False,
			 cb_isearch, (XtPointer)settings);
}

static void
cb_isearch(Widget w, XtPointer closure, XEvent *event, Boolean *cont)
{
    struct search_settings *settings = (struct search_settings *)closure;
    	
    UNUSED(w);
    UNUSED(cont);
	    
    if (settings->isearchterm == NULL)
	settings->isearchterm = xstrdup("");
    
    if (event->type == KeyPress) {
	char buf[48];
	KeySym keysym;
	int count;
	/* temporarily disable control mask; this gives better results with XLookupString() */
	const unsigned int save_state = event->xkey.state;
	/* 	event->xkey.state &= ~ControlMask; */
	count = XLookupString(&(event->xkey), buf, 48, &keysym, NULL);
	event->xkey.state = save_state;
	if (count > 0) {
	    /* Ctrl-g and Ctrl-f continue isearch and
	       switch to the search window, respectively */
	    if (event->xkey.state & ControlMask) {
		if (keysym == XK_g) {
		    settings->term = settings->isearchterm;
		    isearch_start(); /* restart search */
		    return;
		}
		else if (keysym == XK_f) {
		    static char *str = NULL;
		    if (str != NULL) {
			free(str);
		    }
		    str = xstrdup(settings->isearchterm);
		    do_isearch_cancel(settings);
		    TRACE_FIND((stderr, "restarting search: %s", str));
		    dvi_find_string(str, False);
		    return;
		}
	    }
	    if (keysym == XK_Escape) {
		do_isearch_cancel(settings);
		return;
	    }
	    if (keysym == XK_BackSpace || keysym == XK_Delete) {
		size_t len = strlen(settings->isearchterm);
		if (len > 0) {
		    settings->isearchterm = xrealloc(settings->isearchterm, len);
		    settings->isearchterm[len - 1] = '\0';
		    /* statusline_info(STATUS_FOREVER, "I-search (ESC to exit): %s", settings->posix_term); */
		}
		else {
		    xdvi_bell();
		}
	    }
            else if (keysym >= XK_Shift_L && keysym <= XK_Hyper_R) {
		fprintf(stderr, "keysym %ld: Modifier %s\n", keysym, XKeysymToString(keysym));
	    }
	    else if (keysym == XK_Return || keysym == XK_KP_Enter || keysym == XK_Linefeed) { /* TODO */
		settings->term = settings->isearchterm;
		isearch_start(); /* restart search */
		return;
		/* statusline_info(STATUS_FOREVER, "I-search (ESC to exit): %s (next match)", settings->isearchterm); */
	    }
	    else if ((keysym >= XK_KP_Space && keysym <= XK_KP_9)
		     || (keysym >= XK_space && keysym <= XK_asciitilde)
		     || (keysym >= XK_exclamdown && keysym <= XK_ydiaeresis)
		     || (keysym >= XK_F1 && keysym <= XK_F35)) {
		settings->isearchterm = xstrcat(settings->isearchterm, buf);
		/* statusline_info(STATUS_FOREVER, "I-search (ESC to exit): %s", settings->posix_term); */
            }
	    else { /* TODO: abort search? */
		xdvi_bell();
		TRACE_FIND((stderr, "keysym %ld: %s is not handled", keysym, XKeysymToString(keysym)));
	    }
	
	    if (settings->isearchterm[0] != '\0') {
		/* 		const char *prefix = ""; */
		/* 		fprintf(stderr, "cb_isearch called!\n"); */
		/* 		if (settings->wrapcnt > 0) */
		/* 		    prefix = "Wrapped "; */
		/* 		statusline_info(STATUS_FOREVER, "%sI-search (ESC to exit): %s", prefix, settings->isearchterm); */
		settings->from_page = current_page;
		settings->message_window = 0;
		/* 	settings->wrapcnt = 0; */
		settings->term = settings->isearchterm;
		search_dvi((XtPointer)settings);
	    }
	}
    }

}
	    
#ifdef MOTIF
#define XTranslations XmNtranslations
#else
#define XTranslations XtNtranslations
#endif
    
/* static void */
/* isearch_cancel(Widget w, XEvent *event, String *params, Cardinal *num_params) */
/* { */
/*     struct search_settings *settings = NULL; */
/*     void *ptr; */
    
/*     UNUSED(w); */
/*     UNUSED(event); */
    
/*     if (*num_params < 1) { */
/* 	XDVI_WARNING((stderr, "Wrong argument number (%d) in callback!", *num_params)); */
/* 	return; */
/*     } */
/*     sscanf(*params, "%p", &ptr); */
/*     settings = (struct search_settings *)ptr; */
/*     do_isearch_cancel(settings); */
/* } */


/* static XtActionsRec isearch_actions[] = { */
/*     {"CancelIsearch",		isearch_cancel }, */
/* }; */

void isearch_start(void)
{
    /*     char *ptr = NULL; */
    XtTranslations empty_trans;
    static struct search_settings settings = {
	NULL, NULL, NULL,
	False, False, False, False,
	False, 0,
	NULL, /* isearchterm */
	SEARCH_DOWN, NULL,
	0, 0,
	0, 0, 0,
	0, NULL
    };
    static struct search_info searchinfo = {
	False, False, False,
	0, 0, 0, 0
    };
    static int wrapcnt_bak = 0;
    
    settings.searchinfo = &searchinfo;

    if (settings.isearchterm) { /* already running a search, jump to next match */
	settings.searchinfo->from_pos = settings.searchinfo->to_pos;
	if (settings.wrapcnt > wrapcnt_bak) { /* search wrapped */
	    wrapcnt_bak = settings.wrapcnt;
	    search_restart((XtPointer)&settings);
	}
	else {
	    wrapcnt_bak = settings.wrapcnt;
	    search_dvi((XtPointer)&settings);
	}
	return;
    }
    
    settings.wrapcnt = 0;
    
    /*     ptr = get_string_va("<Key>osfCancel:CancelIsearch(%p)", (void *)&settings); */
    /*     empty_trans = XtParseTranslationTable(ptr); */
    empty_trans = XtParseTranslationTable("");
    /*     free(ptr); */
    /*     XtAddActions(isearch_actions, XtNumber(isearch_actions)); */

    statusline_info(STATUS_FOREVER, "I-search (ESC to exit): ");
    XtVaSetValues(globals.widgets.top_level, XTranslations, empty_trans, NULL);
    XtAddEventHandler(globals.widgets.top_level, KeyPressMask|KeyReleaseMask, False,
		      cb_isearch, (XtPointer)&settings);
    
    XtVaSetValues(globals.widgets.draw_widget, XTranslations, empty_trans, NULL);
    XtAddEventHandler(globals.widgets.draw_widget, KeyPressMask|KeyReleaseMask, False,
		      cb_isearch, (XtPointer)&settings);
    
    XtVaSetValues(globals.widgets.clip_widget, XTranslations, empty_trans, NULL);
    XtAddEventHandler(globals.widgets.clip_widget, KeyPressMask|KeyReleaseMask, False,
		      cb_isearch, (XtPointer)&settings);
}

