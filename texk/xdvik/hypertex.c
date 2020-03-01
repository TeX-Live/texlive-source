
/*
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * Original copyright:
 *
 * Hypertex modifications to DVI previewer for X.
 * This portion of xhdvi is completely in the public domain. The
 * author renounces any copyright claims. It may be freely used for
 * commercial or non-commercial purposes. The author makes no claims
 * or guarantees - use this at your own risk.
 *
 * Arthur Smith, U. of Washington, 1994
 *
 * 5/1994       code written from scratch, probably inspired by (but
 *                incompatible with) the CERN WWW library.
 *
 * 3/1995       CERN WWW library called to do document fetching.
 *
 * 5/2002	SU: rewritten from scratch to make it work with the new
 *		event handling in xdvi >= 22.41: Anchors are now saved
 *		per page instead of per document, making it more robust
 *		wrt. interrupting the drawing and skipping to another page.
 *		Only drawback is that links breaking over pages are more
 *		difficult to handle (but they didn't work properly before either).
 *		Clicking on a link now performs a search for the
 *		corresponding anchor similiar to `forward search'.
 *
 *		Removed libwww, since it has become too bloated
 *		and buggy, and had only rudimentary support for GUI
 *		interaction to begin with (e.g., interrupting a
 *		download). And since this is 2002, we might just as well
 *		use a web browser to fetch remote documents ;-)
 */

/* TODO:
   
- implement popup with list of visited links, as specified in
http://xdvi.sourceforge.net/gui.html#navigation-openLinks
(maybe this should be a simple menu list, not a popup window).
*/

#define COPY_TMP_FILE 0 /* see comments below */

#include "xdvi-config.h"
#include "xdvi.h"

    /* #define DEBUG */

    /*  #define DEBUG_MEMORY_HANDLING */
#include "alloc-debug.h"

#include <string.h>

#include <ctype.h>
#include "kpathsea/c-fopen.h"
#include "kpathsea/c-stat.h"
#include <X11/StringDefs.h> /* for XtNwidth, XtNheight */

#include "events.h"
#include "dvi-init.h"
#include "message-window.h"
#include "util.h"
#include "dl_list.h"
#include "x_util.h"
#include "mime.h"
#include "mag.h"
#include "dvi-draw.h"
#include "statusline.h"
#include "browser.h"
#include "hypertex.h"
#include "special.h"
#include "string-utils.h"
#include "xm_toolbar.h"
#include "my-snprintf.h"
#include "pagehist.h"

    /* globals */
    /* rgb specifications to translate resource.{visited_}link_color into */
    char *g_link_color_rgb = NULL;
char *g_visited_link_color_rgb = NULL;
char *g_anchor_pos = NULL;
size_t g_anchor_len = 0;

static const int DEFAULT_MARKER_X_OFFSET = 2; /* horizontal offset of anchor marker to edge of screen */
static const int HTEX_ALLOC_STEP = 32;

/* distance of underline from lower edge of character bbox */
static const int ANCHOR_XTRA_V_BBOX = 6;

/* info whether we're dealing with hypertex or hdvips type links */
typedef enum { HYPERTEX_LINK, HDVIPS_LINK } hrefLinkT;
static hrefLinkT m_href_type;

typedef enum {
    A_HREF = 0,
    A_NAME,
    A_HREF_FILE,	/* hdvips file ref */
    A_HREF_URL,		/* hdvips URL ref */
    A_HDVIPS_INTERNAL,	/* internal ref */
    A_HDVIPS_HREF,	/* hdvips href */
    A_HDVIPS_NAME,	/* hdvips name */
    A_OTHER,
    A_END,
    A_NONE,
    A_MISMATCH
} htexAnchorT;

/****************************** helper routines ******************************/

static Boolean
htex_is_href(htexAnchorT type)
{
    return type == A_HREF
	|| type == A_HREF_FILE
	|| type == A_HREF_URL
	|| type == A_HDVIPS_HREF
	|| type == A_HDVIPS_INTERNAL;
}

static Boolean
htex_is_name(htexAnchorT type)
{
    return type == A_NAME || type == A_HDVIPS_NAME;
}

static void
parse_html_special(const char *special, size_t len, const char **beg, const char **end)
{
    *beg = special;
    while (isspace((int)**beg) || **beg == '=') {
	(*beg)++;
	len--;
    }
    *end = *beg + len - 1;

    while (isspace((int)**end) || **end == '>')
	(*end)--;

    /* remove quote pairs */
    if (**beg == '"') {
	(*beg)++;
	if (**end == '"') {
	    (*end)--;
	}
    }
    /* now end points to last anchor char, move behind that */
    (*end)++;
}

/*
  Anchor prescanning stuff: We make a pass through all pages in the document
  before displaying them to collect information about mismatched anchors
  (anchors where the opening tag is on some previous page).
  The information is stored in the following struct:
*/
struct prescan_info {
    int anchor_depth;
    size_t anchor_num;
    size_t anchor_list_size;
    int *anchor_list;
    /* pointers to either NULL, or to mismatched anchor strings
       (the contents of '<a href="...">' on some previous page),
       for each page */
    size_t pagelist_len;
    char **pagelist;
};

static struct prescan_info m_prescan_info = { 0, 0, 0, NULL, 0, NULL };
static struct prescan_info m_save_prescan_info = { 0, 0, 0, NULL, 0, NULL };


/*
  Save prescan info from previous page, in case we only have a partial
  scan of the current page and need to start all over.
*/
void
htex_prescan_save(void)
{
    m_save_prescan_info.anchor_depth = m_prescan_info.anchor_depth;
    m_save_prescan_info.anchor_num = m_prescan_info.anchor_num;
    /* copy over anchor list */
    while (m_save_prescan_info.anchor_depth >= (int)m_save_prescan_info.anchor_list_size) {
	int i;
	m_save_prescan_info.anchor_list_size += HTEX_ALLOC_STEP;
	m_save_prescan_info.anchor_list = XREALLOC(m_save_prescan_info.anchor_list,
						   m_save_prescan_info.anchor_list_size
						   * sizeof *(m_save_prescan_info.anchor_list));
	for (i = 0; i < m_save_prescan_info.anchor_depth; i++) {
	    m_save_prescan_info.anchor_list[i] = m_prescan_info.anchor_list[i];
	}
    }
    /* the page list of anchor strings can only have had its current position modified;
       we'll simply delete that position in htex_prescan_restore().
    */
}

/*
  Restore prescan info from m_save_prescan_info.
*/
void
htex_prescan_restore(int pageno)
{
    m_prescan_info.anchor_depth = m_save_prescan_info.anchor_depth;
    m_prescan_info.anchor_num = m_save_prescan_info.anchor_num;
    /* copy back anchor list */
    if (m_save_prescan_info.anchor_depth > 0) {
	int i;
	/* old list might have been free()d in htex_prescan_reset_firstpass */
	while (m_save_prescan_info.anchor_depth >= (int)m_prescan_info.anchor_list_size) {
	    m_prescan_info.anchor_list_size += HTEX_ALLOC_STEP;
	    m_prescan_info.anchor_list = XREALLOC(m_prescan_info.anchor_list,
						  m_prescan_info.anchor_list_size
						  * sizeof *(m_prescan_info.anchor_list));
	}
	for (i = 0; i < m_save_prescan_info.anchor_depth; i++) {
	    m_prescan_info.anchor_list[i] = m_save_prescan_info.anchor_list[i];
	}
    }
    /* reset anchor string info for this page */
    if ((int)m_prescan_info.pagelist_len > pageno) {
	ASSERT(m_prescan_info.pagelist != NULL, "m_prescan_info.pagelist not properly allocated");
	FREE(m_prescan_info.pagelist[pageno]);
	m_prescan_info.pagelist[pageno] = NULL;
    }
}

/*
  Reset prescan info for first pass (anchor numbers only) to its initial state,
  freeing up allocated resources for anchor_list.
  
*/
void
htex_prescan_reset_firstpass(void)
{
    MYTRACE((stderr, "resetting anchor_depth to 0!!!!!!!!!!!!"));
    m_prescan_info.anchor_depth = 0;
    m_prescan_info.anchor_num = 0;
    m_prescan_info.anchor_list_size = 0;
    FREE(m_prescan_info.anchor_list);
    m_prescan_info.anchor_list = NULL;
}

/*
  Reset the prescan info, freeing up all allocated memory.
  Used when e.g. switching to a different file via Ctrl-F.
*/
static void
htex_prescan_reset(void)
{
    size_t i;
    m_prescan_info.anchor_depth = 0;
    m_prescan_info.anchor_num = 0;
    m_prescan_info.anchor_list_size = 0;
    
    free(m_prescan_info.anchor_list);
    m_prescan_info.anchor_list = NULL;
    
    for (i = 0; i < m_prescan_info.pagelist_len; i++) {
 	free(m_prescan_info.pagelist[i]);
    }
    free(m_prescan_info.pagelist);
    m_prescan_info.pagelist = NULL;
    m_prescan_info.pagelist_len = 0;
}

/*
  Initialize m_prescan_info for scanning a new page.
*/
void
htex_prescan_initpage(void)
{
    m_prescan_info.anchor_num = 0;
}


Boolean
htex_prescan_special(const char *cp, int cp_len, struct htex_prescan_data *data)
{
    UNUSED(cp_len);
    ASSERT(data != NULL, "data argument to htex_prescan_special() mustn't be NULL");
    if (data->pageno + 1 < (int)m_prescan_info.pagelist_len) { /* already scanned this page */
	MYTRACE((stderr, "already scanned page %d", data->pageno+1));
	return False;
    }

    /* resize pagelist */
    while (data->pageno >= (int)m_prescan_info.pagelist_len) {
	size_t old_len = m_prescan_info.pagelist_len;
	size_t i;
	MYTRACE((stderr, "============ resizing pagelist to %d", data->pageno + 1));
	m_prescan_info.pagelist_len = data->pageno + 1;
	m_prescan_info.pagelist = XREALLOC(m_prescan_info.pagelist,
					   m_prescan_info.pagelist_len * sizeof *m_prescan_info.pagelist);
	/* initialize with NULL values */
	for (i = old_len; i < m_prescan_info.pagelist_len; i++) {
	    MYTRACE((stderr, "============ initializing pagelist %d", i));
	    m_prescan_info.pagelist[i] = NULL;
	}
    }
    
    if (data->scan_type == HTEX_ANCHOR_STRING) {
	if ((int)m_prescan_info.anchor_num == data->anchor_num) {
	    char *anchor;
	    const char *beg, *end;
	    const char *cp1 = cp;
	    const char *ptr = NULL;
	    const char *pptr = NULL;
	    
	    if (memicmp(cp1, "<a href", strlen("<a href")) == 0) {
		cp1 += strlen("<a href");
		parse_html_special(cp1, strlen(cp1), &beg, &end);
		anchor = xmalloc(end - beg + 1);
		memcpy(anchor, beg, end - beg);
		anchor[end - beg] = '\0';
		/* save the anchor string in m_prescan_info.pagelist[<current_page>] */
		m_prescan_info.pagelist[data->pageno] = anchor;
	    }
	    else if (memcmp(cp1, "/A", 2) == 0) {
		if ((ptr = strstr(cp1 + 2, "/GoToR")) != NULL /* external file */
		    && (ptr = strchr(ptr, '(')) != NULL
		    && (pptr = strchr(ptr + 1, '(')) != NULL) {
		    anchor = xmalloc(pptr - ptr);
		    memcpy(anchor, ptr + 1, pptr - ptr - 1);
		    anchor[pptr - ptr - 1] = '\0';
		    /* save the anchor string in m_prescan_info.pagelist[<current_page>] */
		    m_prescan_info.pagelist[data->pageno] = anchor;
		}
		else if ((ptr = strstr(cp1 + 2, "/URI")) != NULL /* external file */
			 && (ptr = strchr(ptr, '(')) != NULL
			 && (pptr = strchr(ptr + 1, '(')) != NULL) {
		    anchor = xmalloc(pptr - ptr);
		    memcpy(anchor, ptr + 1, pptr - ptr - 1);
		    anchor[pptr - ptr - 1] = '\0';
		    /* save the anchor string in m_prescan_info.pagelist[<current_page>] */
		    m_prescan_info.pagelist[data->pageno] = anchor;
		}
	    }
	    else if (memcmp(cp, "/L", 2) == 0) {
		if ((ptr = strstr(cp1 + 2, "/Dest")) != NULL /* internal link */
		    && (ptr = strchr(ptr, '(')) != NULL
		    && (pptr = strchr(ptr + 1, '(')) != NULL) {
		    anchor = xmalloc(pptr - ptr);
		    memcpy(anchor, ptr + 1, pptr - ptr - 1);
		    anchor[pptr - ptr - 1] = '\0';
		    /* save the anchor string in m_prescan_info.pagelist[<current_page>] */
		    m_prescan_info.pagelist[data->pageno] = anchor;
		}
	    }
	}
    }
    if (memicmp(cp, "<a ", 3) == 0) {
	while (m_prescan_info.anchor_depth >= (int)m_prescan_info.anchor_list_size) {
	    m_prescan_info.anchor_list_size += HTEX_ALLOC_STEP;
	    m_prescan_info.anchor_list = XREALLOC(m_prescan_info.anchor_list,
						  m_prescan_info.anchor_list_size
						  * sizeof *(m_prescan_info.anchor_list));
	}
	ASSERT(m_prescan_info.anchor_depth >= 0, "List should contain previous anchor info");
	m_prescan_info.anchor_list[m_prescan_info.anchor_depth] = m_prescan_info.anchor_num;
	m_prescan_info.anchor_depth++;
	m_prescan_info.anchor_num++;
    }
    else if (memicmp(cp, "</a", 3) == 0) {
	if (m_prescan_info.anchor_depth < 1) {
	    /* this can happen when stuff had been prescanned before */
	    return False;
	}
	m_prescan_info.anchor_depth--;
	m_prescan_info.anchor_num++;
    }
    else if (memcmp(cp, "H.S end", strlen("H.S end")) == 0) { /* start of anchor */
	while (m_prescan_info.anchor_depth >= (int)m_prescan_info.anchor_list_size) {
	    m_prescan_info.anchor_list_size += HTEX_ALLOC_STEP;
	    m_prescan_info.anchor_list = XREALLOC(m_prescan_info.anchor_list,
						  m_prescan_info.anchor_list_size
						  * sizeof *(m_prescan_info.anchor_list));
	}
	ASSERT(m_prescan_info.anchor_depth >= 0, "List should contain previous anchor info");
	m_prescan_info.anchor_list[m_prescan_info.anchor_depth] = m_prescan_info.anchor_num;
	m_prescan_info.anchor_depth++;
	m_prescan_info.anchor_num++;

    }
    else if (memcmp(cp, "H.R end", strlen("H.R end")) == 0 /* end of rect */
	     || memcmp(cp, "H.A end", strlen("H.A end")) == 0 /* end of anchor */
	     || memcmp(cp, "H.L end", strlen("H.L end")) == 0 /* end of link */
	     ) {
	if (m_prescan_info.anchor_depth < 1) {
	    /* this can happen when stuff had been prescanned before */
	    return False;
	}
	m_prescan_info.anchor_depth--;
	m_prescan_info.anchor_num++;
    }
    return False;
}

int
htex_prescan_get_depth(void)
{
    return m_prescan_info.anchor_depth;
}

/* copy over pagelist from old_page to new_page */
void
htex_prescan_carry_over(int old_page, int new_page)
{
    ASSERT(old_page >= 0, "old_page out of range");
    ASSERT(old_page < (int)m_prescan_info.pagelist_len, "old_page out of range");

    /* resize if needed */
    if (new_page >= (int)m_prescan_info.pagelist_len) {
	size_t old_len = m_prescan_info.pagelist_len;
	size_t i;
	m_prescan_info.pagelist_len = new_page + 1;
	m_prescan_info.pagelist = XREALLOC(m_prescan_info.pagelist,
					   m_prescan_info.pagelist_len * sizeof *m_prescan_info.pagelist);
	/* initialize with NULL values */
	for (i = old_len; i < m_prescan_info.pagelist_len; i++) {
	    m_prescan_info.pagelist[i] = NULL;
	}
    }
    free(m_prescan_info.pagelist[new_page]);
    /* don't share pointers here */
    if (m_prescan_info.pagelist[old_page] != NULL) {
	m_prescan_info.pagelist[new_page] = xstrdup(m_prescan_info.pagelist[old_page]);
    }
    else {
	m_prescan_info.pagelist[new_page] = NULL;
    }
}

size_t
htex_prescan_get_mismatched_anchor_num(size_t depth)
{
    ASSERT((int)depth <= m_prescan_info.anchor_depth, "depth too large");
    ASSERT(depth <= m_prescan_info.anchor_list_size, "depth too large for lookup list");
    
    return m_prescan_info.anchor_list[m_prescan_info.anchor_depth - 1];
}

/******************************* end of prescan stuff ******************************/

struct anchor_marker {
    int page;	/* page on which marker is located */
    char *filename; /* file in which the marker is located */
    int y_pos;  /* vertical position for marker */
    int x_pos;  /* horizontal position for marker */
} g_anchormarker = { -1, NULL, -1, -1 };

static XtIntervalId m_href_timeout_id = 0;


struct history_info {
    char *anchor;	/* anchor name */
    char *filename;	/* name of file in which this anchor is located */
    int page;		/* pagenumber on which anchor is located */
};

struct anchor_info {
    /* anchor contents as strings */
    char *a_name;
    char *a_href;
    htexObjectT object_type;
    /* bounding box info for this anchor */
    int lrx, lry;	/* lower-right corner */
    int ulx, uly;	/* upper-left corner */
    /*     int refpage;	/\* page in DVI file for stack of visited anchors *\/ */
    /*     char *filename;		/\* name of file in which this anchor is located, for visited anchors *\/ */
    int prev_wrapped;	/* index of prev elem, for wrapped hrefs, or -1 */
    int next_wrapped;	/* index of next elem, for wrapped hrefs, or -1 */
};

struct htex_page_info {
    struct anchor_info *anchors;	/* anchor info */
    int tot_cnt;		/* anchor info size */
    int curr_cnt;		/* current number of anchor on page */
    int page;			/* page in DVI file this anchor info refers to */
    int have_wrapped;		/* -1 if no wrapped anchor on beginning of page, or index of wrapped anchor */
};

/* file-scope globals ... */
/* record x and y positions of current anchor, to recognize linebreaks in anchors */
static int x_pos_bak = 0;
static int y_pos_bak = 0;
/* holds all anchors on current page */
static struct htex_page_info htex_page_info = { NULL, 0, 0, -1, -1 };

/*
  double linked list with history of clicked links, for htex_forward/htex_back
*/
static struct dl_list *htex_history = NULL;	/* current list insertion point */

struct htex_anchor_stack_elem {
    htexAnchorT type;	/* type of anchor (href, anchor, ...) */
    int anchor_num;		/* number of this anchor on page (same as curr_cnt in struct htex_page_info) */
};

/*
 * stack datatype and access functions; used for nested anchors
 */
struct htex_anchor_stack {
    size_t size;
    size_t depth;
    struct htex_anchor_stack_elem *types;
};

struct visited_anchor {
    int *list;
    size_t list_size;
};

struct visited_anchors {
    struct visited_anchor *anchors;
    size_t size;
};

static struct visited_anchors visited_links = { NULL, 0 };

static struct htex_anchor_stack stack = { 0, 0, NULL };



/* the following list is from
   http://www.iana.org/assignments/uri-schemes
   re-sorted for likeliness. All protocols except for `file'
   are considered remote.
   (i.e. not accessible via the ordinary Unix file system)
*/
static const char *remote_URL_schemes[] = {
    "http:",            /* Hypertext Transfer Protocol                    [RFC2068] */
    "ftp:",             /* File Transfer Protocol                         [RFC1738] */
    "https:",           /* Hypertext Transfer Protocol Secure             [RFC2818] */
    "mailto:",          /* Electronic mail address                        [RFC2368] */
    "news:",            /* USENET news                                    [RFC1738] */
    "nntp:",            /* USENET news using NNTP access                  [RFC1738] */
    "telnet:",          /* Reference to interactive sessions              [RFC1738] */
    "nfs:",             /* network file system protocol                   [RFC2224] */
    "gopher:",          /* The Gopher Protocol                            [RFC1738] */
    "wais:",            /* Wide Area Information Servers                  [RFC1738] */
    /* the only exception: */
    /* "file:", */      /* Host-specific file names                       [RFC1738] */
    "prospero:",        /* Prospero Directory Service                     [RFC1738] */
    "z39.50s",          /* Z39.50 Session                                 [RFC2056] */
    "z39.50r",          /* Z39.50 Retrieval                               [RFC2056] */
    "cid:",             /* content identifier                             [RFC2392] */
    "mid:",             /* message identifier                             [RFC2392] */
    "vemmi:",           /* versatile multimedia interface                 [RFC2122] */
    "service:",         /* service location                               [RFC2609] */
    "imap:",            /* internet message access protocol               [RFC2192] */
    "acap:",            /* application configuration access protocol      [RFC2244] */
    "rtsp:",            /* real time streaming protocol                   [RFC2326] */
    "tip:",             /* Transaction Internet Protocol                  [RFC2371] */ 
    "pop:",             /* Post Office Protocol v3                        [RFC2384] */
    "data:",            /* data                                           [RFC2397] */
    "dav:",             /* dav                                            [RFC2518] */
    "opaquelocktoken:", /* opaquelocktoken                                [RFC2518] */
    "sip:",             /* session initiation protocol                    [RFC2543] */
    "tel:",             /* telephone                                      [RFC2806] */
    "fax:",             /* fax                                            [RFC2806] */
    "modem:",           /* modem                                          [RFC2806] */
    "ldap:",            /* Lightweight Directory Access Protocol          [RFC2255] */
    "soap.beep:",       /* soap.beep                                      [RFCSOAP] */
    "soap.beeps:",      /* soap.beeps                                     [RFCSOAP] */
    /* Reserved URI Scheme Names: */
    "afs:",             /* Andrew File System global file names			 */	
    "tn3270:",          /* Interactive 3270 emulation sessions			 */
    "mailserver:",      /* Access to data available from mail servers		 */
    NULL
};

/* prototypes */
static void htex_erase_anchormarker(XtPointer client_data, XtIntervalId *id);
static void htex_draw_anchormarker(int y);


static void
resize_info_if_needed(struct htex_page_info *info)
{
    /* resize info if needed */
    if (info->curr_cnt + 2 >= info->tot_cnt) {
	int i;
	while (info->curr_cnt + 2 >= info->tot_cnt) {
	    info->tot_cnt += HTEX_ALLOC_STEP;
	}
	info->anchors = XREALLOC(info->anchors, info->tot_cnt * sizeof *(info->anchors));
	for (i = info->curr_cnt; i < info->tot_cnt; i++) {
	    /*  	    fprintf(stderr, "initializing info at index %d\n", i); */
	    info->anchors[i].a_href = NULL;
	    info->anchors[i].a_name = NULL;
	    /* 	    info->anchors[i].filename = NULL; */
	    info->anchors[i].ulx = INT_MAX;
	    info->anchors[i].uly = INT_MAX;
	    info->anchors[i].lrx = 0;
	    info->anchors[i].lry = 0;
	    info->anchors[i].object_type = HTEX_TEXT;
	    info->anchors[i].prev_wrapped = -1;
	    info->anchors[i].next_wrapped = -1;
	}
    }
}

static void
init_visited_links(struct visited_anchors *links, int total_pages, Boolean new_dvi_file)
{
    size_t i, old_size = links->size;
    if (new_dvi_file) {
	/* free old list */
	for (i = 0; i < old_size; i++) {
	    FREE(links->anchors[i].list);
	    links->anchors[i].list = NULL;
	    links->anchors[i].list_size = 0;
	}
    }
    if (links->size <= (size_t)total_pages) {
	MYTRACE((stderr, "resetting visited links (%d != %d)", links->size, total_pages));
	links->size = total_pages + 1;
	links->anchors = XREALLOC(links->anchors, (links->size + 1) * sizeof *(links->anchors));
	for (i = old_size; i < links->size; i++) {
	    MYTRACE((stderr, "+++ initializing visited links for page %d", i));
	    links->anchors[i].list = NULL;
	    links->anchors[i].list_size = 0;
	}
    }
}

#if 0
static void
show_visited(struct visited_anchors *links, int pageno)
{
    size_t i;
    fprintf(stderr, "visited links on page %d:\n", pageno);
    for (i = 0; i < links->anchors[pageno].list_size; i++) {
	fprintf(stderr, "%d ", links->anchors[pageno].list[i]);
    }
    fprintf(stderr, "\n");
}
#endif

static void
save_in_list(struct visited_anchors *links, int pageno, int idx)
{
    links->anchors[pageno].list
	= XREALLOC(links->anchors[pageno].list,
		   (links->anchors[pageno].list_size + 1)
		   * sizeof *(links->anchors[pageno].list));
    links->anchors[pageno].list[links->anchors[pageno].list_size] = idx;
    links->anchors[pageno].list_size++;
}

static void
set_visited(struct visited_anchors *links, int pageno, int anchor_num)
{
    int i;
    /* is it already present? */
    for (i = 0; i < (int)links->anchors[pageno].list_size; i++) {
	if (links->anchors[pageno].list[i] == anchor_num)
	    return;
    }
    save_in_list(links, pageno, anchor_num);
    i = anchor_num;
    /* also set previous/next of this anchor to visited */
    while ((i = htex_page_info.anchors[i].prev_wrapped) != -1) {
	TRACE_HTEX((stderr, "set_visited: setting prev_wrapped %d to visited too", i));
	save_in_list(links, pageno, i);
    }
    i = anchor_num;
    while ((i = htex_page_info.anchors[i].next_wrapped) != -1) {
	TRACE_HTEX((stderr, "set_visited: setting next_wrapped %d to visited too", i));
	save_in_list(links, pageno, i);
    }
#if 0
    show_visited(links, pageno);
#endif
}

static Boolean
is_visited(struct visited_anchors *links, int pageno, int anchor_num)
{
    size_t i;
    
    ASSERT((size_t)pageno < links->size, "too few elements in links structure");

    for (i = 0; i < links->anchors[pageno].list_size; i++) {
	if (links->anchors[pageno].list[i] == anchor_num) {
	    return True;
	}
    }
    return False;
}

static void
push_stack(struct htex_anchor_stack *stack, htexAnchorT type, int anchor_num)
{
    size_t i = 0;
    if (stack->depth >= stack->size) {
    	stack->size += HTEX_ALLOC_STEP;
	stack->types = XREALLOC(stack->types, stack->size * sizeof *(stack->types));
	for (i = stack->depth; i < stack->size; i++) {
	    stack->types[i].type = A_NONE;
	    stack->types[i].anchor_num = -1;
	}
    }
    stack->types[stack->depth].type = type;
    stack->types[stack->depth].anchor_num = anchor_num;
    stack->depth++;
#if 0
    {
	fprintf(stderr, "PUSH - stack is now: \n");
	for (i = 0; i < stack->depth; i++)
	    fprintf(stderr, "%d:%d ", i, stack->types[i]);
	MYTRACE(stderr, "\n");
    }
#endif
}

static htexAnchorT
pop_stack(struct htex_anchor_stack *stack, int *anchor_num)
{
    htexAnchorT ret;
    
    if (stack->depth < 1) {
	return A_MISMATCH;
    }
    
#if 0
    {
	int i;
	MYTRACE((stderr, "POP - stack is now: "));
	for (i = 0; i < stack->depth; i++)
	    MYTRACE((stderr, "%d:%d ", i, stack->types[i]));
	MYTRACE((stderr, ""));
    }
#endif
    stack->depth--;
    ret = stack->types[stack->depth].type;
    *anchor_num = stack->types[stack->depth].anchor_num;
    stack->types[stack->depth].type = A_NONE;
    stack->types[stack->depth].anchor_num = -1;
    return ret;
}


/* return True if stack contains an A_HREF, False else */
static Boolean
get_href_depth(const struct htex_anchor_stack *stack)
{
    size_t i;
    for (i = 0; i <= stack->depth; i++) {
	ASSERT(stack->types != NULL, "types musn't be NULL!");
	if (htex_is_href(stack->types[i].type))
	    return True;
    }
    return False;
}

static htexAnchorT
peek_stack(struct htex_anchor_stack *stack, int *anchor_num)
{
    if (stack->depth < 1) {
	MYTRACE((stderr, "Xdvi warning: wrong nesting of anchors on page %d", current_page));
	*anchor_num = -1;
	return A_NONE;
    }
    
    *anchor_num = stack->types[stack->depth - 1].anchor_num;
    return stack->types[stack->depth - 1].type;
}

/* routines for coloring the anchors */
static void
push_colorspecial(void)
{
    int i;

    i = htex_page_info.curr_cnt - 1;
    /* apply color if needed */
    if (resource.link_style > 1) { /* colored links are wanted */
	Boolean visited = False;

	ASSERT(i >= 0, "i mustn't be negative");
	if (is_visited(&visited_links, current_page, i)) {/*  || wrapped_anchor_is_visited(i)) { */
	    visited = True;
	}

	MYTRACE((stderr, "anchor %d, %s is %s\n",
		 htex_page_info.curr_cnt - 1, htex_page_info.anchors[htex_page_info.curr_cnt - 1].a_href,
		 visited ? "******* visited ****** " : "not visited"));
	if ((visited && resource.visited_link_color != NULL)
	    || (! visited && resource.link_color != NULL)) {
	    color_special(visited ? g_visited_link_color_rgb : g_link_color_rgb);
	}
    }
}

static void
pop_colorspecial(void)
{
    color_special("pop");
}

/* return filename if it's a local file, NULL else */
const char *
is_local_file(const char *filename)
{
    int i;
    if (strchr(filename, ':') != NULL) {
	if (memicmp(filename, "file:", strlen("file:")) == 0) {
	    TRACE_HTEX((stderr, "%s uses file scheme", filename));
	    filename += strlen("file:");
	    /*
	      skip over `//localhost' part, and skip first `/' iff the
	      absolute path starts with `//' (as required by RFC2396,
	      but in the past most browsers/applications didn't support
	      this).
	    */
	    if (memicmp(filename, "//localhost", strlen("//localhost")) == 0) {
		filename += strlen("//localhost");
	    }
	    if (memicmp(filename, "//", 2) == 0) {
		filename += 1;
	    }
	    return filename;
	}
	
	/* check remote schemes */
	for (i = 0; remote_URL_schemes[i] != NULL; i++) {
	    if (memicmp(filename, remote_URL_schemes[i], strlen(remote_URL_schemes[i])) == 0) {
		TRACE_HTEX((stderr, "%s is a remote scheme", filename));
		return NULL;
	    }
	}
    }
    /* in all other cases, treat it as an ordinary filename */
    TRACE_HTEX((stderr, "%s is an ordinary filename", filename));
    return filename;
}


static char *
parse_anchortext(const char *input, int len)
{
    char *anchor = NULL;
    const char *beg, *end;

    parse_html_special(input, len, &beg, &end);

    anchor = XMALLOC(anchor, end - beg + 1);
    memcpy(anchor, beg, end - beg);
    anchor[end - beg] = '\0';
    return anchor;
}

static void
add_anchor(struct htex_page_info *info, htexAnchorT type,
	   const char *str, size_t len,
	   int pageno, char *filename)
{
    UNUSED(pageno);
    UNUSED(filename);

    resize_info_if_needed(info);

    ASSERT(htex_is_name(type) || htex_is_href(type), "This doesn't look like a valid anchor");
    /* add an anchor or a href, depending on `type' */
    if (type == A_HREF) {
	if (info->anchors[info->curr_cnt].a_href == NULL) {
	    info->anchors[info->curr_cnt].a_href = parse_anchortext(str, len);
	}
	TRACE_HTEX((stderr, "adding HREF %d: |%s|", info->curr_cnt, info->anchors[info->curr_cnt].a_href));
    }
    else if (type == A_HREF_URL) {
	if (info->anchors[info->curr_cnt].a_href == NULL) {
	    info->anchors[info->curr_cnt].a_href = xmalloc(len + 1);
	    strncpy(info->anchors[info->curr_cnt].a_href, str, len);
	    info->anchors[info->curr_cnt].a_href[len] = '\0';
	}
	TRACE_HTEX((stderr, "adding HREF_URL %d: |%s|", info->curr_cnt, info->anchors[info->curr_cnt].a_href));
    }
    else if (type == A_HDVIPS_INTERNAL) {
	if (info->anchors[info->curr_cnt].a_href == NULL) {
	    /* dynamically add a `#' prefix */
	    if (str[0] != '#') {
		info->anchors[info->curr_cnt].a_href = xmalloc(len + 2);
		strcpy(info->anchors[info->curr_cnt].a_href, "#");
		memcpy(info->anchors[info->curr_cnt].a_href + 1, str, len);
		info->anchors[info->curr_cnt].a_href[len + 1] = '\0';
	    }
	    else {
		info->anchors[info->curr_cnt].a_href = xmalloc(len + 1);
		strncpy(info->anchors[info->curr_cnt].a_href, str, len);
		info->anchors[info->curr_cnt].a_href[len] = '\0';
	    }
	}
	TRACE_HTEX((stderr, "adding HREF_URL %d: |%s|", info->curr_cnt, info->anchors[info->curr_cnt].a_href));
    }
    else if (type == A_HREF_FILE) {
	if (info->anchors[info->curr_cnt].a_href == NULL) {
	    /* dynamically add a `file:' extension */
	    if (memcmp(str, "file:", strlen("file:")) == 0) {
		info->anchors[info->curr_cnt].a_href = xmalloc(len + 1);
		strncpy(info->anchors[info->curr_cnt].a_href, str, len);
		info->anchors[info->curr_cnt].a_href[len] = '\0';
	    }
	    else {
		info->anchors[info->curr_cnt].a_href = xmalloc(len + strlen("file:") + 1);
		strcpy(info->anchors[info->curr_cnt].a_href, "file:");
		memcpy(info->anchors[info->curr_cnt].a_href + strlen("file:"),
		    str, len);
		info->anchors[info->curr_cnt].a_href[len + strlen("file:")] = '\0';
	    }
	}
	TRACE_HTEX((stderr, "adding HREF_FILE %d: |%s|", info->curr_cnt, info->anchors[info->curr_cnt].a_href));
    }
    else if (type == A_NAME) {
	if (info->anchors[info->curr_cnt].a_name == NULL) {
	    info->anchors[info->curr_cnt].a_name = parse_anchortext(str, len);
	}
	TRACE_HTEX((stderr, "adding NAME %d: %s", info->curr_cnt, info->anchors[info->curr_cnt].a_name));
    }
    else if (type == A_HDVIPS_HREF) {
	if (info->anchors[info->curr_cnt].a_href == NULL) {
	    info->anchors[info->curr_cnt].a_href = xmalloc(len + 1);
	    strncpy(info->anchors[info->curr_cnt].a_href, str, len);
	    info->anchors[info->curr_cnt].a_href[len] = '\0';
	}
	TRACE_HTEX((stderr, "adding HDVIPS_HREF %d: |%s|", info->curr_cnt, info->anchors[info->curr_cnt].a_name));
    }
    else if (type == A_HDVIPS_NAME) {
	if (info->anchors[info->curr_cnt].a_name == NULL) {
	    info->anchors[info->curr_cnt].a_name = xmalloc(len + 1);
	    strncpy(info->anchors[info->curr_cnt].a_name, str, len);
	    info->anchors[info->curr_cnt].a_name[len] = '\0';
	}
	TRACE_HTEX((stderr, "adding HDVIPS_NAME %d: |%s|", info->curr_cnt, info->anchors[info->curr_cnt].a_name));
    }
}

static void
set_anchor_size(struct htex_page_info *info, int index,
		int ulx, int uly, int lrx, int lry)
{
    struct anchor_info *anchor;
    
    ASSERT(info->anchors != NULL, "info->anchors should have been allocated before");
    ASSERT(index < info->curr_cnt, "info too small");
    
    anchor = &(info->anchors[index]);

    anchor->ulx = ulx;
    anchor->uly = uly;
    anchor->lrx = lrx;
    anchor->lry = lry;
}

void
htex_set_objecttype(htexObjectT type)
{
    htex_page_info.anchors[htex_page_info.curr_cnt - 1].object_type = type;
}

void
htex_set_anchorsize(int ulx, int uly, int lrx, int lry)
{
    set_anchor_size(&htex_page_info, htex_page_info.curr_cnt - 1,
		    ulx, uly, lrx, lry);
}

static void
enlarge_anchor_size(struct htex_page_info *info, int index,
		    int ulx, int uly, int lrx, int lry)
{
    struct anchor_info *anchor;
    
    ASSERT(info->anchors != NULL, "info->anchors should have been allocated before");
    ASSERT(index < info->curr_cnt, "info too small");

    /*     fprintf(stderr, "enlarging anchor at index %d; %s\n", index, info->anchors[index].a_href); */
    anchor = &(info->anchors[index]);

    if (ulx < anchor->ulx) {
	anchor->ulx = ulx;
    }
    if (uly < anchor->uly) {
	anchor->uly = uly;
    }
    if (lrx > anchor->lrx) {
	anchor->lrx = lrx;
    }
    /* set lry only for first character, since this will be used
       to position underline */
    /*      if (lry > anchor->lry && anchor->lry == 0) { */
    if (lry > anchor->lry) {
	anchor->lry = lry;
    }
}

static void
reset_page_info(struct htex_page_info *info, int pageno, Boolean force_init)
{
    int i, dummy;

    if (force_init || pageno != info->page) {
#if 0
	fprintf(stderr, "%d or %d != %d: resetting anchorinfo for page %d (%d anchors)\n",
		force_init, pageno, info->page, current_page, info->curr_cnt);
#endif
	ASSERT(info->curr_cnt == 0 || info->anchors != NULL, "inconsistency in info structure");
	/* re-initialize all values */
	for (i = 0; i < info->curr_cnt; i++) {
	    TRACE_HTEX((stderr, "----- resetting info for anchor %d", i));
	    FREE(info->anchors[i].a_name);
	    FREE(info->anchors[i].a_href);
	    info->anchors[i].a_name = NULL;
	    info->anchors[i].a_href = NULL;
	    info->anchors[i].ulx = INT_MAX;
	    info->anchors[i].uly = INT_MAX;
	    info->anchors[i].lrx = 0;
	    info->anchors[i].lry = 0;
	    info->anchors[i].object_type = HTEX_TEXT;
	    info->anchors[i].prev_wrapped = -1;
	    info->anchors[i].next_wrapped = -1;
	}
    }
    if (pageno != info->page) { /* reset info */
	free(info->anchors);
	info->anchors = NULL;
	info->tot_cnt = 0;
    }
    TRACE_HTEX((stderr, "---------------- setting curr_cnt to 0, and emptying stack"));
    info->page = pageno;
    info->curr_cnt = 0;
    info->have_wrapped = -1;
    while (stack.depth > 0)
	pop_stack(&stack, &dummy);
}

/*
 * htex_initpage does what's neccessary at the beginning of a page:
 * - re-initialize geometry info if file or page size has changed (according to size_changed),
 * - re-initialize visited links info if file has changed (according to new_dvi_file),
 * - take care of mismatched anchors at the beginning of the page.
 */
void
htex_initpage(Boolean new_dvi_file, Boolean size_changed, int pageno)
{
    reset_page_info(&htex_page_info, pageno, size_changed | new_dvi_file);
    init_visited_links(&visited_links, total_pages, new_dvi_file);

#if 0
    show_visited(&visited_links, pageno);
#endif
    
    if (pageno > 0
	&& (int)m_prescan_info.pagelist_len > pageno /* pagelist_len will be 0 for file with no hyperlinks */
	&& m_prescan_info.pagelist[pageno - 1] != NULL) {
	add_anchor(&htex_page_info, A_HREF,
		   m_prescan_info.pagelist[pageno - 1],
		   strlen(m_prescan_info.pagelist[pageno - 1]),
		   pageno, NULL);
	htex_page_info.curr_cnt++;
	MYTRACE((stderr, "++++++++++ mismatched anchor text (at %d): |%s|",
		 htex_page_info.curr_cnt - 1, m_prescan_info.pagelist[pageno - 1]));
	/*  	x_pos_bak = y_pos_bak = INT_MAX; */
	x_pos_bak = y_pos_bak = INT_MAX;
	set_anchor_size(&htex_page_info, htex_page_info.curr_cnt - 1, 0, 0, 1, 1);
	htex_page_info.have_wrapped = htex_page_info.curr_cnt - 1;
	if (bg_current != NULL) { /* only if it has been initialized by redraw_page in events.c */
	    push_colorspecial();
	}
	/* 	else { */
	/* 	    fprintf(stderr, "----------------------- NOT pushing color, waiting ...\n"); */
	/* 	} */
	push_stack(&stack, A_HREF, htex_page_info.curr_cnt - 1);
    }
}

/* Returns a PostScript literal text string inside parentheses.  The
   scanner works according to "PostScript language reference, third
   edition" - Sec. 3.2.2. The specification is implemented completely:
   balanced parentheses and all escape sequences are considered.

   The argument STR is expected to be a pointer to the first character
   after the opening parenthesis. The function returns NULL if the
   text string is malformed. Otherwise the return value points to
   an allocated string which should be freed by the calling function.
*/
static char *
scan_ps_literal_text_string(const char *str)
{
    const char *s;
    char *cp, *c;
    int count = 1;
   
    /* Search end of string by considering balanced parentheses. */
    for (s = str; *s; s++)
	if (*s == '\\' && *(s+1))
	    s++;
	else if (*s == '(')
	    count++;
	else if (*s == ')')
	    if (! --count)
		break;

    if (!*s)
	return NULL;
   

    if ((cp = malloc((s - str + 1) * sizeof(*cp))) == NULL) {
	XDVI_ERROR((stderr, "Not enough memory"));
	return NULL;
    }

    /* copy string while translating escape sequences */
    for (c = cp; str < s; str++) {
	if (*str == '\\') {
	    switch (*++str) {
	    case 'n': *c++ = '\n';  break;
	    case 'r': *c++ = '\r';  break;
	    case 't': *c++ = '\t';  break;
	    case 'b': *c++ = '\b';  break;
	    case 'f': *c++ = '\f';  break;
	    case '\n': break;
	    case '\r': if (*(str + 1) == '\n') str++; break;
	    default:
		if (isdigit((unsigned char)*str)) {
		    char number[] = "xxx";
		    char *end;
		    strncpy(number, str, 3);
		    *c++ = strtoul(number, &end, 8);
		    str += end - number - 1;
		}
		else {
		    *c++ = *str; /* ignore \ if followed by another char,
				    including ( and ) and \ */
		}
	    }
	}
	else {
	    *c++ = *str;
	}
    }
    *c = '\0';
   
    return cp;
}

static void
hdvips_add_anchor(struct htex_page_info *info, htexAnchorT type, const char *str)
{
    char *ptr;
    if ((ptr = scan_ps_literal_text_string(str)) != NULL) {
	int curr_cnt_bak = info->curr_cnt;
	/* overwrite previously created dummy/wrapped anchors */
	ASSERT(info->curr_cnt > 0, "hdvips_add_anchor must be called after add_anchor()!");

	while (info->curr_cnt > 0
	       && info->anchors[info->curr_cnt - 1].a_href != NULL
	       && (strcmp(info->anchors[info->curr_cnt - 1].a_href, "__WRAPPED__") == 0
		   || strcmp(info->anchors[info->curr_cnt - 1].a_href, "__DUMMY__") == 0)) {
	    info->curr_cnt--;
	    free(info->anchors[info->curr_cnt].a_href);
	    free(info->anchors[info->curr_cnt].a_name);
	    info->anchors[info->curr_cnt].a_href = info->anchors[info->curr_cnt].a_name = NULL;
	    add_anchor(info, type, ptr, strlen(ptr), 0, NULL);
	}
	info->curr_cnt = curr_cnt_bak;
	free(ptr);
    }
    else {
	MYTRACE((stderr, "Xdvi warning: skipping malformed hdvips link `%s'", str));
    }
}

/*
 * htex_reset_page: invoked when drawing was interrupted,
 * assumes that htex_page_info is in an inconsistent state and resets it.
 */
void
htex_reset_page(int pageno)
{
    reset_page_info(&htex_page_info, pageno, True);
}

/* returns True iff inside a href tag */
Boolean
htex_scan_anchor(const char *cp, size_t len)
{
    char *ptr;
    
    if (memicmp(cp, "</a>", 4) == 0) { /* end tag */
	/* 	struct anchor_info *anchor; */
	int num;
	htexAnchorT type = pop_stack(&stack, &num);
	if (type == A_MISMATCH)
	    return False;

	/* ASSERT(htex_page_info.curr_cnt - 1 >= 0, "Index out of range"); */
	/* anchor = &(htex_page_info.anchors[htex_page_info.curr_cnt - 1]); */

	/* reset color if needed */
	if (resource.link_style > 1 && resource.link_color != NULL && htex_is_href(type))
	    pop_colorspecial();
    }
    else if (memicmp(cp, "<a ", 3) == 0) {
	m_href_type = HYPERTEX_LINK;
	cp += 3; /* skip over `<a ' part */
	len -= 3;
	TRACE_HTEX((stderr, "scan_anchor: |%s|", cp));
	if (memicmp(cp, "name", 4) == 0) {
	    add_anchor(&htex_page_info, A_NAME, cp + 4, len - 4, 0, NULL);
	    htex_page_info.curr_cnt++;
	    push_stack(&stack, A_NAME, htex_page_info.curr_cnt);
	}
	else if (memicmp(cp, "href", 4) == 0) {
	    add_anchor(&htex_page_info, A_HREF, cp + 4, len - 4, 0, NULL);
	    htex_page_info.curr_cnt++;
	    push_stack(&stack, A_HREF, htex_page_info.curr_cnt);

	    /* MYTRACE((stderr, "NON-WRAPPED ANCHOR at %d,%d!", PXL_H, PXL_V)); */
	    push_colorspecial();
	    x_pos_bak = PXL_H;
	    y_pos_bak = PXL_V;
	    /* 	    set_anchor_size(&htex_page_info, htex_page_info.curr_cnt - 1, PXL_H, PXL_V, PXL_H + 10, PXL_V + 10); */
	}
	else {
	    XDVI_WARNING((stderr, "Skipping unimplemented htex special `%s'", cp));
	    push_stack(&stack, A_OTHER, htex_page_info.curr_cnt);
	}
    }
    else if (memcmp(cp, "H.S end", strlen("H.S end")) == 0) {
	/* start of anchor, link or rect. We just assume that the link
	   target will have length 0, so when we see text between H.S
	   and H.R, assume that it's the link text.
	*/
	m_href_type = HDVIPS_LINK;
	/* add dummy */
	add_anchor(&htex_page_info, A_HDVIPS_HREF, "__DUMMY__", strlen("__DUMMY__"), 0, NULL);
	htex_page_info.curr_cnt++;
	push_stack(&stack, A_HREF, htex_page_info.curr_cnt - 1);
	push_colorspecial();
	x_pos_bak = PXL_H;
	y_pos_bak = PXL_V;
	return True;
    }
    else if (memcmp(cp, "H.R end", strlen("H.R end")) == 0 /* end of rect */
	     || memcmp(cp, "H.A end", strlen("H.A end")) == 0 /* end of anchor */
	     || memcmp(cp, "H.L end", strlen("H.L end")) == 0 /* end of link */
	     ) {
	int num;
	htexAnchorT type = pop_stack(&stack, &num);
	if (type == A_MISMATCH)
	    return False;
	if (resource.link_style > 1 && resource.link_color != NULL)
	    pop_colorspecial();
	return False;
    }
    /* add anchor texts for hdvips links */
    else if (memcmp(cp, "/A", 2) == 0) { /* possibly an hdvips external link */
	/* 	fprintf(stderr, "+++++ EXT: |%s|\n", cp); */
	if ((ptr = strstr(cp + 2, "/GoToR")) != NULL /* external file */
	    && (ptr = strchr(ptr, '(')) != NULL) {
	    hdvips_add_anchor(&htex_page_info, A_HREF_FILE, ptr + 1);
	}
	else if ((ptr = strstr(cp + 2, "/URI")) != NULL /* URL */
		 && (ptr = strchr(ptr, '(')) != NULL) {
	    hdvips_add_anchor(&htex_page_info, A_HREF_URL, ptr + 1);
	}
	return False;
    }
    else if (memcmp(cp, "/L", 2) == 0) { /* possibly an hdvips internal link */
	if ((ptr = strstr(cp + 2, "/Dest")) != NULL
	    && (ptr = strchr(ptr, '(')) != NULL) {
	    hdvips_add_anchor(&htex_page_info, A_HDVIPS_INTERNAL, ptr + 1);
	}
	return False;
    }
    else if (memcmp(cp, "/V", 2) == 0) { /* possibly an hdvips anchor */
	if ((ptr = strstr(cp + 2, "/Dest")) != NULL
	    && (ptr = strchr(ptr, '(')) != NULL) {
	    hdvips_add_anchor(&htex_page_info, A_HDVIPS_NAME, ptr + 1);
	}
	return False;
    }
    else { /* unrecognized tag */
	if (globals.warn_spec_now) {
	    XDVI_WARNING((stderr, "Ignoring unknown hyperref special `%s'", cp));
	}
	return False;
    }
    return get_href_depth(&stack);
}

void
htex_record_position(int ulx, int uly, int w, int h)
{
    int lrx, lry;
    int y_delta;

    if (!INSIDE_MANE_WIN) /* this would give wrong values */
	return;

    lrx = ulx + w;
    lry = uly + h;

    y_delta = lry - uly;

    /* heuristics for creating new bounding box at what might be linebreaks */
    if (lrx < x_pos_bak /* ordinary linebreak */
	|| lry + y_delta < y_pos_bak) { /* column break (from bottom of 1st to top 2nd column) */
	htexAnchorT type;
	int idx, prev, curr;
	/* get anchor index of topmost stack item, to find the matching open tag */
	if ((type = peek_stack(&stack, &idx)) == A_NONE) {
	    /* 	    fprintf(stderr, "!!!!!!!!!!!! couldn't find opening tag for this wrapped link!"); */
	    return;
	}
	ASSERT(idx >= 0, "Index out of range");
	/* 	fprintf(stderr, "wrapped link: index %d, type %d\n", idx, type); */
	/* get correct idx */
	if (m_href_type == HYPERTEX_LINK) {
	    while (htex_page_info.anchors[idx].a_href == NULL && idx > 0) {
		idx--;
	    }
	    if (htex_page_info.anchors[idx].a_href == NULL && idx == 0) {
		XDVI_ERROR((stderr, "Couldn't find wrapped anchor for idx %d, page %d!", idx, current_page));
		return;
	    }
	    add_anchor(&htex_page_info, A_HREF,
		       htex_page_info.anchors[idx].a_href,
		       strlen(htex_page_info.anchors[idx].a_href),
		       0, NULL);
	}
	else {
	    add_anchor(&htex_page_info, A_HREF, "__WRAPPED__", strlen("__WRAPPED__"), 0, NULL);
	}
	htex_page_info.curr_cnt++;
	/* add wrapping info */
	if (htex_page_info.have_wrapped >= 0) {
	    /* this is the only case where some other material might have come between
	       this and the previous anchor */
	    prev = htex_page_info.have_wrapped;
	    htex_page_info.have_wrapped = -1;
	}
	else {
	    prev = htex_page_info.curr_cnt - 2;
	}
	curr = htex_page_info.curr_cnt - 1;
	ASSERT(prev >= 0, "Index out of range");

	/* 	fprintf(stderr, "setting prev to %d, curr to %d\n", prev, curr); */
	htex_page_info.anchors[prev].next_wrapped = curr;
	htex_page_info.anchors[curr].prev_wrapped = prev;
	/* initialize it to cover current character */
	set_anchor_size(&htex_page_info, htex_page_info.curr_cnt - 1, ulx, uly, lrx, lry);
    }
    else {
	int prev_idx = htex_page_info.curr_cnt - 1;

	if (prev_idx >= 0) {
	    enlarge_anchor_size(&htex_page_info, prev_idx, ulx, uly, lrx, lry);
	}
	else {
	    MYTRACE((stderr, "!!!!!!!!!!!! Bug? prev_idx < 0"));
	}
    }
    x_pos_bak = lrx;
    y_pos_bak = uly;
}

#if 0
static void
show_history(void)
{
    struct dl_list *ptr = htex_history;
    struct history_info *info = ptr->item;
    int i;
    
    fprintf(stderr, "**************** history: %s, %d, %s\n", info->filename, info->page, info->anchor);
    for (; ptr->prev != NULL; ptr = ptr->prev) { ; }
    for (i = 0; ptr != NULL; ptr = ptr->next, i++) {
	info = ptr->item;
	fprintf(stderr, "elem %d: %s, %d, %s\n", i, info->filename, info->page, info->anchor);
	MYTRACE((stderr, "elem: %p, prev: %p, next: %p", (void *)ptr, (void *)ptr->prev, (void *)ptr->next));
    }
}
#endif

/* check for names like foo/bar.dvi#link, return copy of
   link truncated at the `#' if found (NULL else), and
   save `link' into resource.anchor_pos for later use.
*/
static char *
check_relative_name(const char *link)
{
    char *ptr;
    TRACE_HTEX((stderr, "check_relative_name: |%s|", link));
    if ((ptr = strchr(link, '#')) != NULL
	&& ptr > link + 4
	&& (memicmp(ptr - 4, ".dvi", 4) == 0)) {
	char *new_link = xstrdup(link);
	new_link[ptr - link] = '\0'; /* truncate the copy */
	free(g_anchor_pos);
	g_anchor_pos = xstrdup(ptr + 1);
	g_anchor_len = strlen(g_anchor_pos);
	
	return new_link;
    }
    else { /* reset g_anchor_pos */
	free(g_anchor_pos);
	g_anchor_pos = NULL;
    }
    return NULL;
}

static void
htex_update_toolbar_navigation(void)
{
#if 0
    show_history();
#endif
#if defined(MOTIF) && HAVE_XPM
    tb_set_htex_back_sensitivity(htex_history->prev != NULL);
    tb_set_htex_forward_sensitivity(htex_history->next != NULL);
#endif
}

void
htex_back(void)
{
    struct history_info *info;

    if (htex_history == NULL) {
	xdvi_bell();
	statusline_info(STATUS_SHORT, "Hyperref history is empty");
	return;
    }
    info = htex_history->item;

    if (htex_history->prev == NULL) {
	xdvi_bell();
/* 	statusline_info(STATUS_SHORT, "At begin of history"); */
	htex_update_toolbar_navigation();
	return;
    }

    if (strcmp(info->filename, globals.dvi_name) != 0) { /* new filename different */
	Boolean tried_dvi_ext = False;
	char *new_dvi_name = NULL;
	
	if ((new_dvi_name = open_dvi_file_wrapper(info->filename, False, False,
						  &tried_dvi_ext, False))
	    == NULL) {
	    
	    statusline_error(STATUS_MEDIUM, "Re-opening file \"%s\" failed!\n", info->filename);
	    return;
	}
	else {
	    set_dvi_name(new_dvi_name);
	    globals.ev.flags |= EV_NEWDOC;
	    globals.ev.flags |= EV_PAGEHIST_INSERT;
	    /*  	    statusline_info(STATUS_MEDIUM, "Back to file: \"%s\"", globals.dvi_name); */
	}
    }
    else {
	page_history_insert(info->page);
    }
    goto_page(info->page, resource.keep_flag ? NULL : home, False);

    MYTRACE((stderr, "curr page now: %d", current_page));
    htex_history = htex_history->prev;

    info = htex_history->item;

#if 0
    MYTRACE((stderr, "------ before skipping: %d, %s", info->page, info->anchor));
    show_history();
    while (info->page == current_page && htex_history->prev != NULL) {
	/* skip identical locations */
	MYTRACE((stderr, "+++++++ skipping identical page %d, %s", current_page, info->anchor));
	htex_history = htex_history->prev;
	info = htex_history->item;
    }
    MYTRACE((stderr, "------ after skipping: %d, %s", info->page, info->anchor));
#endif

    htex_update_toolbar_navigation();
}


void
htex_forward(void)
{
    struct history_info *info;
    char *link;
    
    if (htex_history == NULL) {
	xdvi_bell();
	statusline_info(STATUS_SHORT, "Hyperref history is empty");
	return;
    }
    if (htex_history->next == NULL) {
	xdvi_bell();
/* 	statusline_info(STATUS_SHORT, "At end of history"); */
	return;
    }
    
    htex_history = htex_history->next;
    info = htex_history->item;
    link = info->anchor;
    /* go there */
    if (*link == '#') { /* it's a relative link */
	MYTRACE((stderr, "XXXXXXXX %s:%d: setting anchor to |%s|+1", __FILE__, __LINE__, info->anchor));
	free(g_anchor_pos);
	g_anchor_pos = xstrdup(info->anchor + 1);
	g_anchor_len = strlen(g_anchor_pos);
	globals.ev.flags |= EV_ANCHOR;
    }
    else { /* it's an absolute link */
	char *new_linkname = NULL;
	char *new_dvi_name = NULL;
	Boolean tried_dvi_ext = False;
	
	if ((new_linkname = check_relative_name(link)) != NULL)
	    link = new_linkname;

	if ((new_dvi_name = open_dvi_file_wrapper(link, False, False,
						  &tried_dvi_ext, False)) == NULL) {
	    statusline_error(STATUS_MEDIUM, "Re-opening file \"%s\" failed!\n", info->filename);
	    free(new_linkname);
	    return;
	}
	else {
	    set_dvi_name(new_dvi_name);
	    
	    globals.ev.flags |= EV_NEWDOC;
	    goto_page(0, resource.keep_flag ? NULL : home, False);
	    globals.ev.flags |= EV_PAGEHIST_INSERT;

	    if (g_anchor_pos != NULL)
		globals.ev.flags |= EV_ANCHOR;

	    /*  	    statusline_info(STATUS_MEDIUM, "Loaded file: \"%s\"", globals.dvi_name); */
	    free(new_linkname);
	}
    }

    htex_update_toolbar_navigation();
}

/* save this anchor in jump_history, current location in goback_history,
   and update Motif toolbar icons
*/
static void
history_save_anchor(const char *anchor, int current_page, const char *dvi_name)
{
    struct history_info *item;

    if (htex_history == NULL) {
	/* Insert dummy start elem. We need this to prevent dropping
	   off from the begin of the list, since otherwise a 1-elem
	   list couldn't encode the information whether we can still go
	   back. This way it will point to the dummy elem when there's
	   no way to go back.  */
	struct history_info *start_marker = XMALLOC(start_marker, sizeof *start_marker);
	start_marker->anchor = NULL;
	start_marker->filename = NULL;
	start_marker->page = -1;
	htex_history = dl_list_insert(htex_history, start_marker);
    }
    item = XMALLOC(item, sizeof *item);
    item->anchor = xstrdup(anchor);
    item->filename = xstrdup(dvi_name);
    item->page = current_page;
    htex_history = dl_list_truncate(dl_list_insert(htex_history, item));

#if 0
    MYTRACE((stderr, "==================== after inserting "));
    show_history();
#endif
    
    htex_update_toolbar_navigation();

}

/*
 * Return anchor string at coordinates x, y, and anchor count in 3rd argument,
 * or NULL if there's no anchor at these coordinates.
 */
static char *
get_anchor_at_index(int x, int y, int *count)
{
    int i;

    for (i = 0; i < htex_page_info.curr_cnt; i++) {
	struct anchor_info anchor = htex_page_info.anchors[i];
	/*  	fprintf(stderr, "getting anchor at index %d; %s\n", i, anchor.a_href); */
	if (anchor.a_href == NULL) {
	    continue;
	}
	if (anchor.lry + 2 > y && anchor.uly - 1 < y /* allow some vertical fuzz */
	    && anchor.lrx > x && anchor.ulx < x) {
	    *count = i;
	    return anchor.a_href;
	}
    }
    *count = -1;
    return NULL;
}


/*
  generate exposure events for anchor at index idx, and all anchors wrapping to/from it,
  so that they will be redrawn in `visited' color
*/
static void
redraw_anchors(int idx)
{
    struct anchor_info anchor = htex_page_info.anchors[idx];
    int other;
    int i = idx;
#define DO_EXPOSE(lx,ly,rx,ry) (clearexpose(&mane, lx - 20, ly - 3, rx - lx + 26, ry - ly + 6))    

    /* HACK ALERT: This is a workaround for a coloring problem with wrapped anchors
       (see regression/href/002/url-wrap-test2.dvi):
       We can't change the color on-the-fly for a wrapped anchor, since the special
       is pushed only at the opening tag. So for wrapped anchors, the visited colour
       wouldn't be set correctly. Redrawing the entire page ensures this (but it's ugly).
       Currently this hack overrides the algorithm below for all anchors that have a
       `prev_wrapped' pointer.
    */
    if (htex_page_info.anchors[i].prev_wrapped != -1) {
	globals.ev.flags |= EV_NEWPAGE;
	XSync(DISP, False);
	return;
    }

    DO_EXPOSE(anchor.ulx, anchor.uly, anchor.lrx, anchor.lry);

    /* previous parts of wrapped anchors: */
    for (i = idx;
	 i >= 0 && (other = htex_page_info.anchors[i].prev_wrapped) != -1;
	 i--) {
	DO_EXPOSE(htex_page_info.anchors[other].ulx, htex_page_info.anchors[other].uly,
		  htex_page_info.anchors[other].lrx, htex_page_info.anchors[other].lry);
    }

    /* later parts of wrapped anchors: */
    for (i = idx;
	 i < htex_page_info.curr_cnt && (other = htex_page_info.anchors[i].next_wrapped) != -1;
	 i++) {
	DO_EXPOSE(htex_page_info.anchors[other].ulx, htex_page_info.anchors[other].uly,
		  htex_page_info.anchors[other].lrx, htex_page_info.anchors[other].lry);
    }
#undef DO_EXPOSE
}


/*
 * Invoked when clicking on a link.
 * newwindow == True means open new window,
 * newwindow == False means jump to link in current window.
 * With newwindow == True, don't update the toolbar navigation icons, since there's nothing to go back.
 */
Boolean
htex_handleref(int x, int y, Boolean newwindow)
{
    char *link;
    int link_num;

    /*      fprintf(stderr, "---------- htex_handleref!\n"); */
    if (!(globals.cursor.flags & CURSOR_LINK))
	/* When no link cursor is shown, clicking shouldn't jump to link */
	return False;
    
    if ((link = get_anchor_at_index(x, y, &link_num)) != NULL) {
	set_visited(&visited_links, current_page, link_num);

	if (*link == '#') { /* it's a relative link */
	    if (newwindow) {
		launch_xdvi(globals.dvi_name, link + 1);
		redraw_anchors(link_num);
		return True;
	    }
	    else {
		history_save_anchor(link, current_page, globals.dvi_name);
		free(g_anchor_pos);
		g_anchor_pos = xstrdup(link + 1);
		g_anchor_len = strlen(g_anchor_pos);
		globals.ev.flags |= EV_ANCHOR;
		redraw_anchors(link_num);
		return True;
	    }
	}
	else { /* it's an absolute link */
	    char *new_dvi_name = NULL;
	    int pageno_bak = current_page;
	    char *new_linkname = NULL;
	    char *orig_link = link;
	    Boolean tried_dvi_ext = False;
	    
	    MYTRACE((stderr, "OLD FILE: |%s|", globals.dvi_name));

	    if ((new_linkname = check_relative_name(link)) != NULL)
		link = new_linkname;
	    
	    if ((new_dvi_name = open_dvi_file_wrapper(link, False, newwindow,
						      &tried_dvi_ext, False)) != NULL) {
		/* only save link in history if opening succeeds */
		history_save_anchor(orig_link, pageno_bak, globals.dvi_name);
		set_dvi_name(new_dvi_name);
		if (!newwindow) {
		    globals.ev.flags |= EV_NEWDOC;
		    goto_page(0, resource.keep_flag ? NULL : home, False);
		    globals.ev.flags |= EV_PAGEHIST_INSERT;
		    
		    MYTRACE((stderr, "NEW FILE: |%s|", globals.dvi_name));
		    if (g_anchor_pos != NULL)
			globals.ev.flags |= EV_ANCHOR;
		    /*  		    statusline_info(STATUS_MEDIUM, "Loaded file: \"%s\"", dvi_name); */
		}
		else {
		    redraw_anchors(link_num);
		}
	    }
	    free(new_linkname);
	    redraw_anchors(link_num);
	    return True;
	}
    }
    return False;
}

/* change cursor and display anchor text in statusline */
void
htex_displayanchor(int x, int y)
{
    static int id_bak = -1;
    static int pageno_bak = -1;
    int id_curr;
    char *anchor_text;

    anchor_text = get_anchor_at_index(x, y, &id_curr);

    if (anchor_text != NULL) { /* found an anchor */
	if (!(globals.cursor.flags & CURSOR_LINK) && !(globals.cursor.flags & CURSOR_TEXT)) {
	    globals.cursor.flags |= CURSOR_LINK;
	    globals.ev.flags |= EV_CURSOR;
	}
	/* to prevent flicker, print only when it differs from previous one,
	   i.e. when it either has a different ID or is on a different page: */
	if ((resource.expert_mode & XPRT_SHOW_STATUSLINE) != 0
	    && (id_curr != id_bak || current_page != pageno_bak)) {
	    statusline_info(STATUS_FOREVER, "%s", anchor_text);
	    id_bak = id_curr;
	    pageno_bak = current_page;
	}
    }
    else { /* not over anchor text, reset cursor and statusline */
	if (globals.cursor.flags & CURSOR_LINK) {
	    globals.cursor.flags &= ~CURSOR_LINK;
	    globals.ev.flags |= EV_CURSOR;
	}

	if (id_bak != -1 && (resource.expert_mode & XPRT_SHOW_STATUSLINE) != 0) {
	    statusline_clear();
	    id_bak = -1;
	}
    }
}

/*
 * Invoked from all commands that change the shrink factor;
 * forces re-computation of the anchor markers.
 */
void
htex_resize_page(void)
{
    htex_initpage(False, True, current_page);
}

/*
 * Invoked from all commands that reread the DVI file;
 * resets all link infos.
 * Note: visited_links and anchor stack data
 * are preserved across multiple files, so we don't need to free() them.
 */
void
htex_reinit(void)
{
    htex_prescan_reset();
    htex_initpage(True, True, current_page);
}

/*
  Underline/highlight anchors if the appropriate resource is set.
  Since this is done after the entire page is parsed, we can't use
  the checks for fg_current and do_color_change(), but need to
  use a separate GC instead.
*/
void
htex_draw_anchormarkers(void)
{
    int i;
    int rule_height = (globals.page.h / 1000.0);
    XRectangle rect = { -1, -1, 0, 0 };
    
    if (rule_height == 0)
	rule_height = 1;

    if (current_page == g_anchormarker.page
	&& strcmp(globals.dvi_name, g_anchormarker.filename) == 0
	&&  g_anchormarker.y_pos > 0) {
	htex_draw_anchormarker(g_anchormarker.y_pos);
    }
    
    if (resource.link_style == 0 || resource.link_style == 2)
	return;
    
    for (i = 0; i < htex_page_info.curr_cnt; i++) {
	struct anchor_info anchor = htex_page_info.anchors[i];

	if (anchor.a_href != NULL) {
	    int offset;
	    Boolean visited = False;

#if 0
	    XDrawRectangle(DISP, mane.win, globals.gc.high,
			   anchor.ulx, anchor.uly,
			   anchor.lrx - anchor.ulx, anchor.lry - anchor.uly);
			   
#endif
	    
	    if (is_visited(&visited_links, current_page, i)) {/*  || wrapped_anchor_is_visited(i)) { */
		visited = True;
	    }
	    offset = ANCHOR_XTRA_V_BBOX / (double)currwin.shrinkfactor + 0.5;

	    TRACE_HTEX((stderr, "UNDERLINE: %d, %s is %s", i, anchor.a_href,
			visited ? "******* visited ****** " : "not visited"));
	    anchor.ulx -= currwin.base_x;
	    anchor.lrx -= currwin.base_x;
	    anchor.lry -= currwin.base_y;
	    anchor.uly -= currwin.base_y;
	    
	    /* BUG ALERT: Don't use put_rule here, since this will cause a segfault
	     * with the new color code when switching density (#616920)!
	     */
	    MYTRACE((stderr, "UNDERLINE: checking if %d is visited: %d!", i, visited));

	    /* 	    if (clip_region_to_rect(&rect)) { */
	    if (anchor.object_type == HTEX_IMG) {
		rect.x = anchor.ulx - 1;
		rect.y = anchor.uly - 1;
		rect.width = anchor.lrx - anchor.ulx + 2;
		rect.height = anchor.lry - anchor.uly + 2;
		if (clip_region_to_rect(&rect)) {
		    XDrawRectangle(DISP, mane.win,
				   visited ? globals.gc.visited_linkcolor : globals.gc.linkcolor,
				   rect.x, rect.y,
				   rect.width, rect.height);
		}
	    }
	    else {
		rect.x = anchor.ulx - 1;
		rect.y = anchor.lry + offset;
		rect.width = anchor.lrx - anchor.ulx + 2;
		rect.height = rule_height;
		if (clip_region_to_rect(&rect)) {
		    XFillRectangle(DISP, mane.win,
				   visited ? globals.gc.visited_linkcolor : globals.gc.linkcolor,
				   rect.x, rect.y,
				   rect.width, rect.height);
		}
	    }
	}
    }
}

void
launch_xdvi(const char *filename, const char *anchor_name)
{
#define ARG_LEN 32
    int i = 0;
    const char *argv[ARG_LEN];
    char *shrink_arg = NULL;

    ASSERT(filename != NULL, "filename argument to launch_xdvi() mustn't be NULL");

    argv[i++] = kpse_invocation_name;
    /* FIXME: there's something broken with this and invoking xdvi.bin.
       To reproduce the problem, invoke from the shell:
       xdvi.bin -geometry 829x1172 /usr/share/texmf/doc/programs/kpathsea.dvi
       this will run at 300dpi, i.e. ignore an .Xdefaults setting as opposed to:
       xdvi.bin /usr/share/texmf/doc/programs/kpathsea.dvi

       Also, how about the other command-line switches that might have
       been passed to the parent instance? How about things that have been changed
       at run-time, like shrink factor - should they be converted to command-line
       options?
    */

    argv[i++] = "-name";
    argv[i++] = "xdvi";

    /* start the new instance with the same debug flags as the current instance */
    if (globals.debug != 0) {
	argv[i++] = "-debug";
	argv[i++] = resource.debug_arg;
    }
    
    if (anchor_name != NULL) {
	argv[i++] = "-anchorposition";
	argv[i++] = anchor_name;
    }

    argv[i++] = "-s";
    shrink_arg = XMALLOC(shrink_arg, LENGTH_OF_INT + 1);
    sprintf(shrink_arg, "%d", currwin.shrinkfactor);
    argv[i++] = shrink_arg;

    argv[i++] = filename; /* FIXME */
    
    argv[i++] = NULL;
    ASSERT(i <= ARG_LEN, "Too few elements in argv[]");
    
    if (globals.debug & DBG_HTEX) {
	fprintf(stderr, "Invoking:\n");
	for (i = 0; argv[i]; i++) {
	    fprintf(stderr, "%s\n", argv[i]);
	}
    }

    /*
      FIXME: using fork_process here hangs the new xdvi process when it tries
      printing to stderr, so we use plain fork/exec (see comments in util.c)
    */
#if 0
    fork_process(argv[0], False, NULL, NULL, NULL, 0, argv);
#else
    {
	int pid;
	switch (pid = fork()) {
	case -1:
	    perror("fork");
	case 0:
	    execvp(argv[0], (char **)argv);
	    XDVI_ERROR((stderr, "%s: Execution of %s failed.", globals.program_name, argv[0]));
	    _exit(EXIT_FAILURE);
	default:
	    FREE(shrink_arg);
	}
    }
#endif
#undef ARG_LEN
}

#if COPY_TMP_FILE
/* callback to remove temporary file after helper application has terminated.
   Currently unused. Note the possible danger of a temp race if the application
   does an fclose() on the file before ...
*/
static void
remove_temp_file(int status, struct xchild *this)
{
    char *pathname = (char *)this->data;
    if (WEXITSTATUS(status) == 0) {
	unlink(pathname);
    }
    else {
	popup_message(globals.widgets.top_level,
		      MSG_WARN, NULL,
		      "Warning: Calling `%s' on `%s' terminated with non-zero status (%d)"
		      "\n(Not removing `%s')",
		      this->name, pathname, status, pathname);
    }
    free(pathname);
}
#endif

void
launch_program(const char *filename)
{
    const char *format_string = NULL;
    char *content_type = NULL;
    char *viewer = NULL;
    char *argv[4];
    struct stat statbuf;
    const char *local_filename_sans_prefix;
    char *path, *syscmd, *tmp;
#if COPY_TMP_FILE
    int tmp_fd;
#endif
    const char *ptr;
    char *fullpath = NULL;
    char canonical_path[MAXPATHLEN + 1];
    Boolean needs_terminal;
    size_t offset;
    
    TRACE_HTEX((stderr, "launch_program called with |%s|", filename));

    /* is it a local file? */
    local_filename_sans_prefix = is_local_file(filename);
    if (local_filename_sans_prefix == NULL) { /* not local */
	launch_browser(filename);
	return;
    }

    /* expand the filename if it contains a relative path */
    path = find_file(local_filename_sans_prefix, &statbuf, kpse_program_text_format);
    if (path != NULL) {
	/* fully canonicalize path before passing it to helper applications */
	fullpath = REALPATH(path, canonical_path);
	if (fullpath == NULL) {
	    XDVI_WARNING((stderr, "Couldn't canonicalize %s to full path - returning unexpanded.",
			  path));
	    fullpath = path;
	}
	else
	    FREE(path);
    }
    else {
	XDVI_WARNING((stderr, "Couldn't find file %s; passing to application unchanged.",
		      local_filename_sans_prefix));
	/* if it doesn't exist, maybe the user has encoded some magic in the
	   filename; in that case, pass it to the application unchanged. */
	fullpath = (char *)local_filename_sans_prefix;
    }

    TRACE_HTEX((stderr, "fullpath: |%s|", fullpath));
    content_type = figure_mime_type(fullpath);
    ASSERT(content_type != NULL, "figure_mime_type() should have returned a valid type (eventually a fallback)");
    
    /* make it safe to pass the argument to system() or `sh -c',
       by escaping all `dangerous' characters: */
    tmp = shell_escape_string(fullpath);
    fullpath = tmp;
    
    if ((viewer = figure_viewer(content_type, &format_string, &needs_terminal, fullpath)) == NULL) {
	/* warn user if no application has been found. Help text needs to be constructed at
	   run-time, since we want the correct content-type in it. */
#if 0
	char *helptext = NULL;
#else
	char *helptext = get_string_va("Please assign an application to the "
			 "MIME type `%s' in your ~/.mailcap file. "
			 "E.g. if you want to view the file with firefox, "
			 "add the following line to your ~/.mailcap:\n"
			 "%s; firefox '%%s'\n\n",
			 content_type, content_type);
#endif
	popup_message(globals.widgets.top_level,
		      MSG_WARN,
		      helptext,
		      "Could not determine an application for the file %s, MIME type `%s'.",
		      fullpath, content_type);
	/* FIXME: We can't free helptext here, because it won't be used until
	 * the popup comes up and the user clicks on "Help".  This doesn't
	 * happen until after this function returns.  */
	/* free(helptext); */
	free(fullpath);
  	return;
    }
    
#if COPY_TMP_FILE
    /* copy the file to a temporary location before passing it to the
       viewer.  This is e.g. how Acroread does it. This should fix the
       problem with xdvizilla without -no-rm for DVI files (see comment
       in figure_viewer(), mime.c).
       SU 2003/10/02: currently I can't reproduce the xdvizilla problem
       any more - it seems that for xdvi files, argv[0] will be invoked
       anyway? Also, copying the files to tmp may break figure locations
       and relative links for DVI files. Suspended this for the time being.
    */
    tmp = NULL;
    if ((tmp_fd = xdvi_temp_fd(&tmp)) == -1) {
	XDVI_ERROR((stderr, "couldn't create temporary file; not calling helper application."));
	return;
    }
    TRACE_HTEX((stderr, "copying to temporary location: |%s->%s|", fullpath, tmp));
    if (!copy_file(fullpath, tmp)) {
	XDVI_ERROR((stderr, "couldn't copy %s to temporary file %s; not invoking helper application.",
		    fullpath, tmp));
	return;
    }
    free(fullpath);
    fullpath = tmp;
#endif
    
    if (strlen(format_string) > 0) {
	ptr = find_format_str(viewer, format_string);
    }
    else {
	/*
	  Contrary to RFC 1343, we don't pass stuff via pipes (too bothersome).
	  Instead, pass it as normal argument by appending it to the command.
	 */
	ptr = strchr(viewer, '\0');
    }

    offset = 0;
    if (needs_terminal) {
	offset += strlen("xterm -e ");
    }
    /* allocate extra byte for optional whitespace after viewer command */
    syscmd = xmalloc(offset + strlen(viewer) + strlen(fullpath) + 2);
    if (needs_terminal) {
	strcpy(syscmd, "xterm -e ");
    }
    /* viewer command */
    memcpy(syscmd + offset, viewer, ptr - viewer);
    /* bugfix for #2931447: add space separator after viewer command if no format string is present */
    if (strlen(format_string) == 0) {
	strcpy(syscmd + offset + (ptr - viewer), " ");
	offset += 1;
    }
    /* viewer argument */
    strcpy(syscmd + offset + (ptr - viewer), fullpath);
    /* rest of command */
    strcpy(syscmd + offset + (ptr - viewer) + strlen(fullpath), ptr + strlen(format_string));

    /*
      mailcap(5) says that the mailcap command shall be passed to system(),
      so we musn't use fork_process() directly here. Instead, we pass the command
      to `/bin/sh -c', but via fork_process so that
      
      - xdvi doesn't hang if the process doesn't return;
      - we can still catch the return value if `sh -c' exits with an error
      (e.g. if the viewer command is not found).

      Note however that this doesn't mimick most of POSIX's system() semantics.
    */
    TRACE_HTEX((stderr, "execv(\"/bin/sh -c %s\")", syscmd));
    argv[0] = "/bin/sh";
    argv[1] = "-c";
    argv[2] = syscmd;
    argv[3] = NULL;
#if COPY_TMP_FILE
    fork_process("/bin/sh", False, globals.dvi_file.dirname, remove_temp_file,
      fullpath, 0, argv);
#else
    fork_process("/bin/sh", False, globals.dvi_file.dirname, NULL,
      NULL, 0, argv);
#endif

    FREE(viewer);
#if COPY_TMP_FILE
#else
    FREE(fullpath);
#endif
    FREE(syscmd);
}

void
htex_set_anchormarker(int y)
{
    Position drawing_x;

    /* erase old marker if it's on the same page as the new marker, after
       cancelling the old timeout so that it won't affect the new marker */
    if (m_href_timeout_id)
	XtRemoveTimeOut(m_href_timeout_id);
    htex_erase_anchormarker(NULL, NULL);
    XFlush(DISP);

    XtVaGetValues(globals.widgets.draw_widget, XtNx, &drawing_x, NULL);
    g_anchormarker.page = current_page;
    free(g_anchormarker.filename);
    g_anchormarker.filename = xstrdup(globals.dvi_name);
    g_anchormarker.y_pos = y;
    g_anchormarker.x_pos = DEFAULT_MARKER_X_OFFSET + -drawing_x;
    m_href_timeout_id = XtAppAddTimeOut(globals.app, STATUS_SHORT * 1000, htex_erase_anchormarker, (XtPointer)NULL);
}

static void
htex_draw_anchormarker(int y)
{
    int x;
    XPoint points[3];

    Position drawing_x, clip_w;

    XtVaGetValues(globals.widgets.clip_widget, XtNwidth, &clip_w, NULL);
    XtVaGetValues(globals.widgets.draw_widget, XtNx, &drawing_x, NULL);

    /* compute offset to draw into visible region */
    x = DEFAULT_MARKER_X_OFFSET + -drawing_x;

    /* Eventually erase old marker, to avoid `smearing' on horizontal scrolls. */
    if (x != g_anchormarker.x_pos) {
	clearexpose(&mane, g_anchormarker.x_pos - 1, y - 3, 20, 10);
    }
    g_anchormarker.x_pos = x;

    points[0].x = x + 10;
    points[1].x = x + 19;
    points[2].x = x + 10;
    points[0].y = y - 3;
    points[1].y = y + 2;
    points[2].y = y + 7;

    XFillRectangle(DISP, mane.win, globals.gc.visited_linkcolor, x, y, 10, 4);
    XFillPolygon(DISP, mane.win, globals.gc.visited_linkcolor, points, 3, Convex, CoordModeOrigin);
    /* -1 indicates that no horizontal scrolling is wanted, since the
       anchormarker will always be horizontally positioned inside the
       visible area.
    */
    scroll_page_if_needed(-1, -1, y + 3, y - 3);
}


static void
htex_erase_anchormarker(XtPointer client_data, XtIntervalId *id)
{
    Position clip_w;
    
    UNUSED(client_data);
    UNUSED(id);

    if (globals.debug & DBG_EVENT) {
	fprintf(stderr, "htex_erase_anchormarker called!\n");
    }

    if (m_href_timeout_id == (XtIntervalId)0) { /* timeout was removed but callback happened anyway */
	return;
    }

    m_href_timeout_id = (XtIntervalId)0;
    /* clear the mark if we're in the same file and on the same page as the mark */
    if (g_anchormarker.filename != NULL
	&& strcmp(globals.dvi_name, g_anchormarker.filename) == 0
	&& g_anchormarker.page == current_page) {
	XtVaGetValues(globals.widgets.clip_widget, XtNwidth, &clip_w, NULL);
	clearexpose(&mane, 0, g_anchormarker.y_pos - 3, clip_w, 10);
    }
    g_anchormarker.y_pos = -1;
}

