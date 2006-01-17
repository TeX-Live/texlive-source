/*
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
 * 3/1995       CERN WWW library called to do document fetching.
 *
 *
 * Note: several coordinate systems are in use in this code.  Note
 * that the relation between a screen pixel and a dvi coordinate is
 * the current shrink factor times 2^16.
 * 
 * When multiple pages are allowed in one drawing area, the variables
 * xscroll_pages or yscroll_pages are set.
 *
 * Conversion between coordinates is done using the screen_to_page,
 * page_to_screen, screen_to_abs, abs_to_screen routines.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL PAUL VOJTA OR ANYONE ELSE BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#define HAVE_BOOLEAN
#include "xdvi-config.h"
#if defined(XHDVI) || defined(HTEX)
#include <kpathsea/c-pathch.h>
#include <kpathsea/c-ctype.h>
#include <kpathsea/c-fopen.h>
#include <kpathsea/c-stat.h>
#define GETCWD(str,size) xgetcwd()
#ifndef WIN32
#include <X11/Shell.h>	/* needed for def of XtNtitle */
#include <X11/StringDefs.h>
#endif

#ifdef WIN32
#include <shellapi.h>
#include "winhtex.h"
#else
#include "wwwconf.h"
#include "WWWLib.h"
#include "WWWInit.h"
#include "WWWCache.h"

#if 0	/* seems unnecessary?! */
#ifdef HAVE_NETDB_H
#include <netdb.h> /* for struct hostent */
#endif
#endif	/* seems unnecessary?! */
#endif /* !WIN32 */

/* Implementation of HyperTeX through \specials */
/* One option: allow HTML tags and recognize them */
/* applicationDoSpecial strips leading blanks, so first char should
   be '<' if it's a tag */

char *MyStrAllocCopy();
char *refscan();
extern void put_rule();
extern void make_absolute ();
extern int invokeviewer ();

static void extractbase ();
static void retitle ();

int this_is_a_href=0;

#define HTeX_A_NAME 1
#define HTeX_A_HREF 2

#define htex_shrinkfactor mane.shrinkfactor /* Only main win has refs */

typedef struct {
	int type; /* Type of anchor: URL, etc from WWW anchor list */
	char *name; /* Name string for anchor (Null by default) */
	char *href; /* A reference from this anchor */
	int llx, lly, urx, ury; /* Box on page where anchor located */
} HTeX_Anchor;

/* Structure to remember where we have been: */
typedef struct {
	char *refname; /* File name (or href?) */
	int pageno; /* Which page anchor was on */
	int x,y; /* Approximate location on page... */
	int which; /* Number of anchor on this page */
	int type; /* And the other properties of this anchor */
	char *name;
	char *href;
} Anchors;

int waiting_for_anchor = -1; /* If waiting for anchor to be properly parsed? */
int cur_anchor_on_page; /* Keep track of current page as it's read in */

#define HTeX_AnchorSTEP 20
HTeX_Anchor **HTeX_anchorlist = NULL;
int *nHTeX_anchors, *maxHTeX_anchors;

Anchors *HTeX_visited = NULL;
int nHTeX_visited = 0, maxHTeX_visited = 0;

#define HTeX_NSTACK 32
int HTeXAnest[HTeX_NSTACK]; /* Maximum number of nested anchors */
int HTeXAnestlevel; /* Current nesting level */

int HTeXreflevel; /* 0 if not currently inside an href anchor */

int *htex_parsedpages = NULL; /* List of all pages, = 1 if already parsed, zero if not */
int htex_total_pages;
/* size = total_pages, current page = current_page defined in xhdvi.h */

void htex_parseanchor ARGS((char *, HTeX_Anchor *));
char *MyStrAllocCopy ARGS((char **, char *));
char *refscan      ARGS(( char*, char **, char **));
void htex_loc_on_page ARGS((Anchors *));
void freeHTeXAnchors ARGS((HTeX_Anchor *, int));
void htex_img	ARGS((int, char *, int));
void htex_base	ARGS((int, char *, int));

#define BEGIN 0
#define END 1

/* Dave Oliver's hypertex format: */
/* Only understand my version of anchors so far */
/* ie.: his TYPE=text for hrefs, frag for names */
void
hy_handletag(cp, pageno)
    char *cp;
    int pageno;
{
	int beginend=BEGIN;

	while (isspace(*cp)) cp++;
	if (!strncasecmp(cp, "END", 3)) {
		beginend = END;
		cp += 3;
	}
/* Leave the parsing to htex_anchor! */
	htex_anchor(beginend, cp, pageno);
}

int
checkHyperTeX(cp, pageno)
char *cp;
int pageno;
{
	int htexfound = 0;

	if (strncasecmp(cp, "html:", 5) == 0) {
		cp += 5;
		while (isspace(*cp)) cp++;
		htexfound = 1;
	}
	if (*cp == '<') { /* Possibly Missing the header part */
		htexfound = 1;
		htex_handletag(cp, pageno);
	} else if (strncasecmp(cp, "hyp", 3) == 0) {
		/* Dave Oliver's HyperTeX */
		htexfound = 1;
		cp += 4;
		hy_handletag(cp, pageno);
	}

	return htexfound;
}

void
htex_handletag(cp, pageno)
char *cp;
int pageno;
{
	int beginend=BEGIN;

	if (*cp != '<') return;
	++cp;
	while (isspace(*cp)) cp++;
	if (*cp == '/') {
		beginend = END;
		cp++;
	}
	switch(*cp) {
	    case 'A':
	    case 'a': /* Anchors */
		htex_anchor(beginend, cp+1, pageno);
		break;
	    case 'b': /* Base name? */
		htex_base(beginend, cp, pageno);
		break;
	    case 'i': /* Included images? */
		htex_img(beginend, cp, pageno);
		break;
	    default: /* Tag not implemented yet */
		break;
	}
}

/* Basically just want to parse the line... */
/* Should use WWW library stuff ? */

void 
htex_anchor(beginend, cp, pageno)
int beginend, pageno;
char *cp;
{
	int *nap, *maxp;
	int oldllx, oldlly, oldurx, oldury;
	HTeX_Anchor *HTeXAp, **HTeXApp;

	HTeXApp = HTeX_anchorlist + pageno;
	nap = nHTeX_anchors + pageno;
	maxp = maxHTeX_anchors + pageno;
	if (*HTeXApp == NULL) {
		*maxp = HTeX_AnchorSTEP;
		*HTeXApp = xmalloc((*maxp)*sizeof(HTeX_Anchor));
	} else if (*nap == *maxp) {
		*maxp += HTeX_AnchorSTEP;
		*HTeXApp = (HTeX_Anchor *) realloc(*HTeXApp,
					(*maxp)*sizeof(HTeX_Anchor));
	}
	if (htex_parsedpages[pageno] != 1) {  /* Only do if page not done yet */
	  if (beginend == END) {
	    HTeXAnestlevel--;
	    if (HTeXAnestlevel < 0) {
		HTeXAnestlevel = 0; /* Extra </a>'s? */
	    } else {
		HTeXAp = *HTeXApp + HTeXAnest[HTeXAnestlevel];
		if (HTeXAp->llx > DVI_H) {
			HTeXAp->llx = DVI_H;
		}
		if (HTeXAp->urx < DVI_H) {
			HTeXAp->urx = DVI_H;
		}
		if (HTeXAp->lly > DVI_V) {
			HTeXAp->lly = DVI_V;
		}
		if (HTeXAp->ury < DVI_V) {
			HTeXAp->ury = DVI_V;
		}
		oldllx = HTeXAp->llx;
		oldlly = HTeXAp->lly;
		oldurx = HTeXAp->urx;
		oldury = HTeXAp->ury;
		if (debug & DBG_ANCHOR) {
		    Printf("Added anchor %d, level %d:\n",
			    HTeXAnest[HTeXAnestlevel], HTeXAnestlevel);
		    if (HTeXAp->type&HTeX_A_HREF) {
			Printf("href = %s\n", HTeXAp->href);
		    }
		    if (HTeXAp->type&HTeX_A_NAME) {
			Printf("name = %s\n", HTeXAp->name);
		    }
		    Printf("box %d %d %d %d\n",
			HTeXAp->llx, HTeXAp->lly, HTeXAp->urx, HTeXAp->ury);
		}
		if (waiting_for_anchor == HTeXAnest[HTeXAnestlevel]) {
			htex_to_anchor(current_page, waiting_for_anchor);
			waiting_for_anchor = -1; /* Reset it! */
		}
		/* End of debug section */
		if (HTeXAnestlevel > 0) {
			HTeXAp = *HTeXApp + HTeXAnest[HTeXAnestlevel-1];
			/* Check llx, lly, urx, ury info */
			if (oldllx < HTeXAp->llx) {
				HTeXAp->llx = oldllx;
			}
			if (oldlly < HTeXAp->lly) {
				HTeXAp->lly = oldlly;
			}
			if (oldurx > HTeXAp->urx) {
				HTeXAp->urx = oldurx;
			}
			if (oldury > HTeXAp->ury) {
				HTeXAp->ury = oldury;
			}
		}
	    }
	  } else {
		HTeXAp = *HTeXApp + *nap;
		/* Set type, and the name, href */
		htex_parseanchor(cp, HTeXAp);
		if (HTeXAp->type != 0) {
		    cur_anchor_on_page++; /* Increment the count of anchors here */
		    if (htex_parsedpages[pageno] == 2) {
    			/* Go to this anchor in list we already have: */
			HTeXAp = *HTeXApp + cur_anchor_on_page;
			HTeXAp->urx = HTeXAp->llx = DVI_H; /* Current horiz pos.*/
			HTeXAp->ury = HTeXAp->lly = DVI_V; /* Current vert. pos. */
			if (HTeXAnestlevel >= HTeX_NSTACK) {
				/* Error - too many nested anchors! */
			} else {
				HTeXAnest[HTeXAnestlevel++] = cur_anchor_on_page;
			}
		    } else if (htex_parsedpages[pageno] != 1) {  /* Only do if page not done yet */
			HTeXAp->urx = HTeXAp->llx = DVI_H; /* Current horiz pos.*/
			HTeXAp->ury = HTeXAp->lly = DVI_V; /* Current vert. pos. */
			if (HTeXAnestlevel >= HTeX_NSTACK) {
				/* Error - too many nested anchors! */
			} else {
				HTeXAnest[HTeXAnestlevel++] = *nap;
			}
			(*nap)++;
		    }
		}
	  }
	} else { /* if page has been properly parsed before */
	  if (beginend != END) {
		HTeXAp = *HTeXApp + *nap;
		/* Set type, and the name, href */
		htex_parseanchor(cp, HTeXAp);
	  }
	}
	if (beginend == END) {
		if (HTeXreflevel > 0) HTeXreflevel--;
	} else {
		if (HTeXAp->type&HTeX_A_HREF) HTeXreflevel++;
	}
}

void
htex_initpage() /* Starting a new page */
{
	if (htex_parsedpages == NULL) htex_reinit();
	HTeXAnestlevel = 0; /* Start with zero nesting level for a page */
	HTeXreflevel = 0;
	HTeXnext_extern = 0; /* Go to links in current window */
	cur_anchor_on_page = -1;
	paint_anchor(NULL);
}

/* A character or something was written: record position for current anchor */
void 
htex_recordbits(x, y, w, h) /* x,y are pixel positions on current page */
int x, y, w, h;
{
	HTeX_Anchor *HTeXAp;
	int dvix, dviy, dvix2, dviy2;
	int ch = 0;

	dvix = x *(htex_shrinkfactor << 16);
	dviy = y*(htex_shrinkfactor << 16);
	dvix2 = (x+w)*(htex_shrinkfactor << 16);
	dviy2 = (y+h)*(htex_shrinkfactor << 16);
	HTeXAp = HTeX_anchorlist[current_page] + HTeXAnest[HTeXAnestlevel-1];
	if (HTeXAp->llx > dvix) {
		HTeXAp->llx = dvix;
		ch++;
	}
	if (HTeXAp->lly > dviy) {
		HTeXAp->lly = dviy;
		ch++;
	}
	if (HTeXAp->urx < dvix2) {
		HTeXAp->urx = dvix2;
		ch++;
	}
	if (HTeXAp->ury > dviy2) {
		HTeXAp->ury = dviy2;
		ch++;
	}
	if (debug & DBG_ANCHOR) {
	    if (ch > 0) {
		Printf("New box for anchor %d, level %d: %d %d %d %d\n",
			HTeXAnest[HTeXAnestlevel-1], HTeXAnestlevel,
			HTeXAp->llx, HTeXAp->lly, HTeXAp->urx, HTeXAp->ury);
	    }
	}
}

void 
htex_donepage(i, pflag) /* This page has been completed */
int i, pflag;
{
	HTeX_Anchor *HTeXAp;

	/* Finish off boxes for nested anchors not done on this page */
	while (HTeXAnestlevel > 0) {
		HTeXAnestlevel--;
		HTeXAp = HTeX_anchorlist[i] + HTeXAnest[HTeXAnestlevel];
		if (HTeXAp->llx > DVI_H) {
			HTeXAp->llx = DVI_H;
		}
		if (HTeXAp->urx < DVI_H) {
			HTeXAp->urx = DVI_H;
		}
		if (HTeXAp->lly > DVI_V) {
			HTeXAp->lly = DVI_V;
		}
		if (HTeXAp->ury < DVI_V) {
			HTeXAp->ury = DVI_V;
		}
	}
	if (pflag == 1) { /* Really parsed this page */
		htex_drawboxes(); /* Draw boxes around the anchor positions */
		htex_parsedpages[i] = 1;
	} else {
		htex_parsedpages[i] = 2; /* Means htex_parsed, not done properly */
	}
}

/* If the dvi file has changed, assume all links have changed also,
   and reset everything! */
void 
htex_reinit()
{
	int i;

	if (htex_parsedpages == NULL) { /* First call to this routine */
		htex_parsedpages = xmalloc(total_pages*sizeof(int));
		HTeX_anchorlist = xmalloc(total_pages*sizeof(HTeX_Anchor *));
		nHTeX_anchors = xmalloc(total_pages* sizeof(int));
		maxHTeX_anchors = xmalloc(total_pages* sizeof(int));
		for (i=0; i < total_pages; i++) maxHTeX_anchors[i] = 0;
	} else if (htex_total_pages != total_pages) {
		htex_parsedpages = (int *) realloc(htex_parsedpages,
					total_pages*sizeof(int));
	/* Following operates if new has fewer pages than old: */
		for (i=total_pages; i < htex_total_pages; i++) {
		    	if (maxHTeX_anchors[i] > 0)
			    freeHTeXAnchors(HTeX_anchorlist[i], nHTeX_anchors[i]);
		}
		HTeX_anchorlist = (HTeX_Anchor **) realloc(HTeX_anchorlist,
				total_pages*sizeof(HTeX_Anchor *));
		nHTeX_anchors = (int *) realloc(nHTeX_anchors,
					total_pages* sizeof(int));
		maxHTeX_anchors = (int *) realloc(maxHTeX_anchors,
					total_pages* sizeof(int));
	/* Following operates if new has more pages than old: */
		for (i= htex_total_pages; i < total_pages; i++)
				maxHTeX_anchors[i] = 0;
	}
	htex_total_pages = total_pages;
	for (i=0; i < total_pages; i++) {
	    htex_parsedpages[i] = 0;
	    if (maxHTeX_anchors[i] > 0) { /* Get rid of the old anchor lists: */
		   freeHTeXAnchors(HTeX_anchorlist[i], nHTeX_anchors[i]);
		   free(HTeX_anchorlist[i]);
	    }
	    HTeX_anchorlist[i] = NULL;
	    nHTeX_anchors[i] = 0;
	    maxHTeX_anchors[i] = 0;
	}
}

/* Following parses the stuff after the '<' in the html tag */
/* Only understands name and href in anchor */
/*     html: <A HREF="..." NAME="..."> */
/*     html: <A NAME="..." HREF="...> */
void 
htex_parseanchor(cp, anchor)
char *cp;
HTeX_Anchor *anchor;
{
	char *ref, *str;

	anchor->type = 0;
	anchor->href = NULL;
	anchor->name = NULL;
	while (isspace(*cp)) cp++;
	while ((*cp) && (*cp != '>')) {
		cp = refscan(cp, &ref, &str);
		if (cp == NULL) break;
		if (strcasecmp(ref, "href") == 0) {
			anchor->type |= HTeX_A_HREF;
			MyStrAllocCopy(&(anchor->href), str);
		} else if (strcasecmp(ref, "name") == 0) {
			anchor->type |= HTeX_A_NAME;
			MyStrAllocCopy(&(anchor->name), str);
		}
	}
}

char *MyStrAllocCopy(dest, src)
char **dest, *src;
{
	if (*dest) free(*dest);
	if (! src)
		*dest = NULL;
	else {
		*dest = xmalloc(strlen(src) + 1);
		strcpy(*dest, src);
	}
	return *dest;
}

/* Parses cp containing 'ref="string"more', returning pointer to "more" */
char *refscan(name, ref, str)
char *name, **ref, **str;
{
	char *cp;

	*str = name;
	for (cp=name; *cp; cp++) {
		if (*cp == '=') {
			*cp = 0;
			*ref = name;
			*str = cp+1;
			break;
		}
	}
	cp = *str;
	if (cp != name) {
	    while (isspace(*cp)) cp++;
	    if (*cp == '"') { /* Yes, this really is a string being set */
		*str = cp+1;
		while ((cp = strchr(cp+1, '"')) != NULL) {
			if (cp[-1] != '\\') break; /* Check if quote escaped */
		}
		if (cp != NULL) {
			*cp = 0;
			cp++;
		}
	    } else {
		cp = NULL;
	    }
	} else {
		cp = NULL;
	}
	return cp;
}

/* What happens when mouse moves on screen: */
void 
htex_displayanchor(page, x, y)
int page, x, y; /* x,y coordinates of mouse position */
{
	char astr[256];
	char namestr[256];
	char hrefstr[256];
	HTeX_Anchor *HTeXAp;
	long dvix, dviy;
	int i;

/* Don't display until we've finished with the page: */
	if (htex_parsedpages == NULL) return;
/* Locate current page if we're scrolling them: */
	current_page = page;
	if (htex_parsedpages[current_page] != 1) return;
	dvix = x *(htex_shrinkfactor << 16);
	dviy = y *(htex_shrinkfactor << 16);
	/* Find anchor that fits current position: */
	HTeXAp = HTeX_anchorlist[current_page] + nHTeX_anchors[current_page] - 1;
	for (i=nHTeX_anchors[current_page] - 1; i >= 0; i--, HTeXAp--) {
		if (HTeXAp->llx > dvix) continue;
		if (HTeXAp->lly > dviy) continue;
		if (HTeXAp->urx < dvix) continue;
		if (HTeXAp->ury < dviy) continue;
		if (debug & DBG_ANCHOR) {
		    Printf("In anchor #%d\n", i);
		}
		if (HTeXAp->type & HTeX_A_NAME) {
			sprintf(namestr, "name = %s ", HTeXAp->name);
		} else {
			*namestr= 0;
		}
		if (HTeXAp->type & HTeX_A_HREF) {
			sprintf(hrefstr, "href = %s ", HTeXAp->href);
		} else {
			*hrefstr = 0;
		}
		sprintf(astr, "anchor #%d: %s%s", i, namestr, hrefstr);
		paint_anchor(astr);
		break;
	}
	if (i == -1) paint_anchor(NULL);
}

/* What happens when mouse is clicked: */
int
htex_handleref(page, x, y)
    int page, x, y; /* current mouse location when ref clicked */
{
	HTeX_Anchor *HTeXAp;
	long dvix, dviy;
	int i, afound;

/* Check that we've finished at least one page first! */
	if (htex_parsedpages == NULL) return 0;
/* Locate current page if we're scrolling them: */
	current_page = page;
	if (htex_parsedpages[current_page] != 1) return 0;
	dvix = x *(htex_shrinkfactor << 16);
	dviy = y *(htex_shrinkfactor << 16);
	/* Find anchor that fits current position: */
	HTeXAp = HTeX_anchorlist[current_page] + nHTeX_anchors[current_page] - 1;
	afound = -1;
#if 0
	fprintf(stderr, "click @ (%d, %d)\n", x, y);
#endif
	for (i=nHTeX_anchors[current_page]-1; i >= 0; i--, HTeXAp--) {
		if ((HTeXAp->type&HTeX_A_HREF) == 0) continue; /* Only ref on hrefs */
#if 0
		fprintf(stderr, "Href @ (%d, %d) -- (%d, %d)\n",
				(int)(HTeXAp->llx / (htex_shrinkfactor << 16)),
				(int)(HTeXAp->lly / (htex_shrinkfactor << 16)),
				(int)(HTeXAp->urx / (htex_shrinkfactor << 16)),
				(int)(HTeXAp->ury / (htex_shrinkfactor << 16)));
#endif
		if (HTeXAp->llx > dvix) continue;
		if (HTeXAp->lly > dviy) continue;
		if (HTeXAp->urx < dvix) continue;
		if (HTeXAp->ury < dviy) continue;
		afound = i; /* Get the last of them in case of nesting */
		break;
	}
	if (afound == -1) return 0; /* There was no href at this location */
/* Then just do it: */
	this_is_a_href=1;
	htex_dohref(HTeXAp->href);
	this_is_a_href=0;
	return 1;
}

void
htex_dohref(href)
   char *href;
{
	int i;

/* Update the list of where we used to be: */
	if (HTeX_visited == NULL) {
		maxHTeX_visited = HTeX_AnchorSTEP;
		HTeX_visited = xmalloc(maxHTeX_visited*sizeof(Anchors));
		for (i=0; i < maxHTeX_visited; i++) {
			HTeX_visited[i].refname = NULL;
			HTeX_visited[i].name = NULL;
			HTeX_visited[i].href = NULL;
		}
	} else if (nHTeX_visited >= maxHTeX_visited - 1) {
		maxHTeX_visited += HTeX_AnchorSTEP;
		HTeX_visited = (Anchors *) realloc(HTeX_visited,
			maxHTeX_visited*sizeof(Anchors));
		for (i=nHTeX_visited; i < maxHTeX_visited; i++) {
			HTeX_visited[i].refname = NULL;
			HTeX_visited[i].name = NULL;
			HTeX_visited[i].href = NULL;
		}
	}
	MyStrAllocCopy(&(HTeX_visited[nHTeX_visited].refname), dvi_name);
	HTeX_visited[nHTeX_visited].pageno = current_page;
#ifdef HTEX
	HTeX_visited[nHTeX_visited].x = mane.base_x;
	HTeX_visited[nHTeX_visited].y = mane.base_y;
#else
	HTeX_visited[nHTeX_visited].x = mane.base_ax;
	HTeX_visited[nHTeX_visited].y = mane.base_ay;
#endif
	HTeX_visited[nHTeX_visited].type = HTeX_A_HREF;
	HTeX_visited[nHTeX_visited].which = 0;
	MyStrAllocCopy(&(HTeX_visited[nHTeX_visited].href), href);
	nHTeX_visited++;
	if (htex_is_url(href)) htex_do_url(href);
	else htex_do_loc(href);
	/* Need to handle properly when ref doesn't exist! */
}

/* Draw boxes around the anchor positions */
void htex_drawboxes()
{
	HTeX_Anchor *HTeXAp;
	int i;
	int x, y, w, h;

	if (!underline_link) return;
	HTeXAp = HTeX_anchorlist[current_page];
	for (i=0; i < nHTeX_anchors[current_page]; i++, HTeXAp++) {
		if ((HTeXAp->type&HTeX_A_HREF) == 0) continue; /* Only box hrefs */
		x = pixel_conv(HTeXAp->llx)-1;
		y = pixel_conv(HTeXAp->lly)-1;
		w = pixel_conv(HTeXAp->urx) - x+2;
		h = pixel_conv(HTeXAp->ury) - y+2;
/* The last arg of put_rule is whether or not to
   use the "highlight" graphics context. */
#ifdef HTEX
		highlight = True;
		put_rule(x+1, y+h, w, 1);
		highlight = False;
#else
/*		put_rule(x, y, w, 1, True);
		put_rule(x+w, y, 1, h, True); */
		put_rule(x+1, y+h, w, 1, True);
/*		put_rule(x, y+1, 1, h, True); */
#endif
	}
}
#ifdef WIN32
RECT WinRect;
extern HWND hWndMain;
#else
static	Arg	arg_wh[] = {
	{XtNwidth,	(XtArgVal) &window_w},
	{XtNheight,	(XtArgVal) &window_h},
};
#endif
static void
invokedviviewer(filename, aname, height)
    char *filename;
    char *aname;
    int height;
{
	char buf[1024];

	if (filename == NULL) return;
#if 0
	/* Ignore the given height -- some calls use unreasonable values. */
	if (height < 50) height = 50;
	height += 50;
#endif

#ifdef WIN32
	GetWindowRect(hWndMain, &WinRect);
	window_w = WinRect.right - WinRect.left;
	window_h = WinRect.bottom - WinRect.top;
#else
	XtGetValues(draw_widget, arg_wh, XtNumber(arg_wh));
#endif
	if (aname == NULL) {
	    sprintf(buf, "%s %s -geometry %dx%d", program_invocation_name, 
                        filename,
			window_w, window_h); /* Same width and height */
	} else {
	    sprintf(buf, "%s %s#%s -geometry %dx%d", program_invocation_name,
	                filename, aname,
			window_w, window_h); /* Same width and height */
	}
#ifndef WIN32
	strcat(buf, " &");
#endif
	if (debug & DBG_HYPER) {
	  fprintf(stderr, "Executing: %s\n", buf);
	}
	system(buf);
}

/* It's a local reference - find the anchor and go to it */
void 
htex_do_loc(href)
char *href;
{
	int ipage, ia, reffound;
	HTeX_Anchor *HTeXAp, **HTeXApp;
	char astr[256];
	char *cp;
	
	if (href == NULL) return; /* shouldn't happen? */
	if (debug & DBG_HYPER) {
	    fprintf(stderr,"htex_do_url(%s)\n", href);
	}
	cp = href;
	while (*cp == '#') cp++;
	HTeXApp = HTeX_anchorlist;
	reffound = 0;
	/* Should hash based on "name" value? - to speed this up! */
	for (ipage = 0; ipage < total_pages; ipage++, HTeXApp++) {
	    	if (htex_parsedpages[ipage] == 0) continue;
		HTeXAp = *HTeXApp;
		for (ia=0; ia < nHTeX_anchors[ipage]; ia++, HTeXAp++) {
			if ((HTeXAp->type&HTeX_A_NAME) == 0) continue;
			if (!strcmp(HTeXAp->name, cp)) {
				reffound = 1;
				break;
			}
		}
		if (reffound) break;
	}
	if (reffound == 0) { /* Need to parse remaining pages */
		if (debug & DBG_ANCHOR) {
		    Printf("Searching for remaining anchors\n");
		}
		htex_parsepages();
		/* And try again: */
		HTeXApp = HTeX_anchorlist;
		for (ipage = 0; ipage < total_pages; ipage++, HTeXApp++) {
			if (htex_parsedpages[ipage] < 2) continue;
			HTeXAp = *HTeXApp;
			for (ia=0; ia < nHTeX_anchors[ipage]; ia++, HTeXAp++) {
				if ((HTeXAp->type&HTeX_A_NAME) == 0) continue;
				if (!strcmp(HTeXAp->name, cp)) {
					reffound = 1;
					break;
				}
			}
			if (reffound) break;
		}
	}
	if (reffound) {
	    if (HTeXnext_extern == 1) {
		invokedviviewer(dvi_name, cp, 
				pixel_conv(HTeXAp->ury) - pixel_conv(HTeXAp->lly));
	    } else {
		htex_to_anchor(ipage, ia); /* move to anchor */
	    }
	} else {
		if ((nHTeX_visited == 0) ||
		   (!strcmp(HTeX_visited[nHTeX_visited-1].refname, dvi_name))) {
		/* Really was from same file - just print error message */
			sprintf(astr, "Error: reference \"%s\" not found\n", cp);
			paint_anchor(astr);
		} else {
			/* Go to page 1 and print error message */
		sprintf(astr, "Error: reference \"%s\" in file %s not found\n",
				cp, dvi_name);
			htex_to_page(0); /* Go to first page! */
			paint_anchor(astr);
		}
	}
}

void
htex_can_it()
{
#ifdef WIN32
  redraw_page();
#else
  canit = True;
  XFlush(DISP);
#endif
}

void
htex_to_page(pageno)
    int pageno;
{
	/* taken from keystroke subroutine: */
	current_page = pageno;
	htex_can_it();
}

void
htex_to_anchor(pageno, n)
    int pageno, n;
{
	int xp, yp;
	HTeX_Anchor *HTeXAp = HTeX_anchorlist[pageno] + n;

	if ((n < 0) || (n >= nHTeX_anchors[pageno])) return; /* Error message for this? */
	if (pageno != current_page) {
		if (htex_parsedpages[pageno] != 1) waiting_for_anchor = n;
	/* taken from keystroke subroutine: */
		current_page = pageno;
		htex_can_it();
	}
	xp = (HTeXAp->llx + HTeXAp->urx)/(2*htex_shrinkfactor << 16);
	yp = (HTeXAp->lly + HTeXAp->ury)/(2*htex_shrinkfactor << 16);
	if (debug & DBG_ANCHOR) {
		Printf("Moving to pixel %d %d\n", xp, yp);
	}
#ifndef HTEX
	if (htex_parsedpages[pageno] > 0) centerpage(pageno, xp, yp);
#endif
}

/* Following goes back to previous anchor point */
void 
htex_goback()
{
	int i;

	if (nHTeX_visited <= 0) return; /* There's nowhere to go! */
	if (debug & DBG_ANCHOR) {
	    Printf("Currently %d anchors in sequence:\n", nHTeX_visited);
	    for (i=0; i < nHTeX_visited; i++) {
		Printf("%d file %s, href=%s\n", i,
			HTeX_visited[i].refname, HTeX_visited[i].href);
	    }
	}
	nHTeX_visited--;
	if (strcmp(HTeX_visited[nHTeX_visited].refname, dvi_name) != 0) {
		/* Need to read in old file again! */
		MyStrAllocCopy(&dvi_name, HTeX_visited[nHTeX_visited].refname);
		i = URL_aware;
		URL_aware = FALSE;
		open_dvi_file();
		URL_aware = i;
		if (dvi_file != NULL) {
		    retitle(dvi_name);
		    extractbase(dvi_name);
		} else {
		    perror(dvi_name);
		    Exit(1);
		}
		htex_can_it();
		htex_reinit();
	}
	htex_loc_on_page(HTeX_visited + nHTeX_visited);
	/* taken from keystroke subroutine: */
	htex_can_it();
}

/* Is this a url we recognize? */
int
htex_is_url(href)
const char *href;
{
    /* Why reinvent the wheel?  Use libwww routines! */
    return (HTURL_isAbsolute(href)==YES ? 1 : 0);
}


#ifndef WIN32
static	Arg	temp_args4[] = {
	{XtNtitle,	(XtArgVal) 0},
	{XtNinput,	(XtArgVal) True},
};
#endif

static void
retitle(str) /* Change the title to "str" */
char *str;
{
#ifdef WIN32
  SetWindowText(top_level, str);
#else
	temp_args4[0].value = (XtArgVal) str;
	XtSetValues(top_level, temp_args4, XtNumber(temp_args4));
#endif
}

/* Can handle href's of form file:?.dvi#name */
/* Actually, supposedly we can handle anything now... */
void
htex_do_url(href)
char *href;
{
	if (href == NULL) return; /* shouldn't happen? */
	if (debug & DBG_HYPER) {
	    fprintf(stderr,"htex_do_url(%s)\n", href);
	}

	/* Have dvi_name parsed using libwww routines to make sure 
	   it doesn't contain invalid characters.  Actually, we may
	   assume that HTURL_isAbsolute(href) when this is called, 
	   so parsing it relative to URLbase shouldn't matter. */

#ifdef WIN32
	if (dvi_name)
	  free(dvi_name);
	dvi_name = HTParse(href, URLbase, PARSE_ALL);
#else
	/* HTParse already allocates memory. This should be fixed. */
	if (URLbase != NULL) { 
	    MyStrAllocCopy(&dvi_name, HTParse(href, URLbase, PARSE_ALL));
	} else {
	    MyStrAllocCopy(&dvi_name, HTParse(href, "", PARSE_ALL));
	}
#endif
	URL_aware = TRUE;
	detach_anchor();
	if (open_www_file() == 0) {
	    /* HTTP request was handled externally by invoking some viewer */
	    URL_aware = FALSE; 
	    htex_can_it();
	    htex_goback(); /* Go back to where we were! */
	    return;
	} else {
	    /* HTTP request was handled internally by calling open_dvi_file() */
	    URL_aware = FALSE;
	    /* retitle(dvi_name); */
	    /* extractbase(dvi_name); */
	    htex_reinit();
	    if (anchor_name != NULL) {
		/* This may be unnecessary, since it's already done in
		   check_for_anchor() which is called from draw_page(). */
	        htex_do_loc(anchor_name);
		free(anchor_name);
		anchor_name = NULL;
	    } else {
		/* This seems to be necessary if there's no anchor_name. */
		htex_to_page(0);
	    }
	    return;
	}
}

/* Find the anchor pointed to by ap on the given page */

void
htex_loc_on_page(ap)
Anchors *ap;
{
	if (htex_parsedpages[ap->pageno] == 0) {
		htex_parse_page(ap->pageno); /* Parse the needed page! */
	}
	htex_to_anchor(ap->pageno, ap->which); /* move to anchor i */
}

void
freeHTeXAnchors(HTeXAp, nHTeX)
HTeX_Anchor *HTeXAp;
int nHTeX;
{
	int i;

	for (i=0; i < nHTeX; i++) {
		if (HTeXAp[i].type&HTeX_A_NAME)
			free(HTeXAp[i].name);
		if (HTeXAp[i].type&HTeX_A_HREF)
			free(HTeXAp[i].href);
	}
}

/* Following shouldn't be necessary... */
/* Add the string cp to the current search string */
#define LINE 1024
char anchor_search_string[LINE];

void
add_search(cp, n)
char *cp;
int n;
{
	int anchor_search_len = 0;
	
	while (n>0) {
	    switch(*cp) {
		case '\v':
		case '\f':
		case '\r':
		case '\n': /* Finish string and search on it */
			anchor_search_string[anchor_search_len] = '\0';
			if (anchor_search_len > 0) {
				htex_dohref(anchor_search_string);
			/* Need to handle properly when ref doesn't exist! */
			}
			return;
			break;
		case '\b':
		case '\177':	/* Del */
			if (anchor_search_len > 0) anchor_search_len--; 
			break;
		case '':
			anchor_search_len = 0;
			break;
		default:
			if (*cp > 10) {
				anchor_search_string[anchor_search_len++] = *cp;
			}
			break;
		}
		cp++;
		n--;
	}
	anchor_search_string[anchor_search_len] = '\0';
	if (debug & DBG_ANCHOR) {
		Printf("Search string: %s\n", anchor_search_string);
	}
}

void
htex_base(beginend, cp, pageno)
int beginend, pageno;
char *cp;
{
	char *ref, *str;
	if (beginend == END) return;

	if (!strncasecmp(cp, "base", 4)) {
		cp += 4;
		cp = refscan(cp, &ref, &str);
		if (cp == NULL) return;
		while (isspace(*ref)) ref++;
		while (isspace(*str)) str++;
		if (strcasecmp(ref, "href") == 0) {
			cp = str + strlen(str) - 1; /* Fix end of anchor */
			while (isspace(*cp)) cp--;
			if (*cp == '>') cp --;
			while (isspace(*cp)) cp--;
			cp++;
			*cp = '\0'; 
			MyStrAllocCopy(&URLbase, str); /* Change base */
			if (debug & DBG_HYPER) {
				Printf("Changing base name to: %s\n", URLbase);
			}
		}
	}
}

void
htex_img(beginend, cp, pageno)
int beginend, pageno;
char *cp;
{
	char *ref, *str;
	char fullpathname[1024];

	if (beginend == END) return;
	if (pageno != current_page) return; /* Only do when on page */
	if (htex_parsedpages[pageno] == 1) return; /* And first time through */
	if (!strncasecmp(cp, "img", 3)) {
		cp += 3;
		cp = refscan(cp, &ref, &str);
		if (cp == NULL) return;
		while (isspace(*ref)) ref++;
		while (isspace(*str)) str++;
		if (strcasecmp(ref, "src") == 0) {
			cp = str + strlen(str) - 1; /* Fix end of anchor */
			while (isspace(*cp)) cp--;
			if (*cp == '>') cp --;
			while (isspace(*cp)) cp--;
			cp++;
			*cp = '\0'; 
			strcpy(fullpathname, str);
			make_absolute(fullpathname, URLbase, 1024);
			if (invokeviewer(fullpathname) != 1) {
	fprintf(stderr, "Don't know how to deal with <img src=%s>\n", fullpathname);
			}
		}
	}
}

#ifdef HTEX
#ifndef WIN32
#undef exit /* just in case */
void
htex_cleanup(arg)
int arg;
{
	/* Delete all the temp files we created */
	for (;nURLs>0; nURLs--) {
	  /* fprintf(stderr,"htex: Unlinking %s\n",filelist[nURLs-1].file); */
	  unlink(filelist[nURLs-1].file);
	}
	HTCache_flushAll();
	HTProfile_delete();
}
#endif /* !WIN32 */
/* Extract the URL base name from initial file name */
static void
extractbase(file)
char *file;
{
	char *cp;
	static char *cwd = NULL;
	int n;

	if (URLbase != NULL) {
		free(URLbase);
		URLbase = NULL;
	}

	if (strrchr(file, '/') != NULL) { /* If no /'s then leave dir NULL */
		n = strlen(file);
		if (htex_is_url(file)) { /* It already is a URL */
			URLbase = xmalloc((unsigned) (n+1));
		        Sprintf(URLbase, "%s", file);
		} else { /* Turn it into one: */
			cwd = GETCWD(cwd, 1024);
			URLbase = xmalloc((unsigned) (n+6 + strlen(cwd)));
		        Sprintf(URLbase, "file:%s/%s", cwd, file);
		}
		cp = strrchr(URLbase, '/');
		if (cp == NULL) {
			fprintf(stderr, "This should not happen!\n");  
			free(URLbase);
			URLbase = NULL;
			return;
		}
/*		cp[1] = '\0'; */ /* Leave it alone */
	}
}

void	
detach_anchor()
{
	char *cp;

	cp = strchr(dvi_name, '#'); /* Handle optional # in name */
	if (cp != NULL) {
		*cp = '\0'; /* Terminate filename string */
		cp++;
		while (*cp == '#') cp++;
		MyStrAllocCopy(&anchor_name, cp);
#ifdef WIN32
		/* FIXME : this is caused by some .dvi 
		   added to the filename */
		if (strlen(anchor_name) >= 4 &&
		    FILESTRCASEEQ(anchor_name+strlen(anchor_name)-4,
				  ".dvi")) {
		  anchor_name[strlen(anchor_name)-4] = '\0';
		}
#endif
	}
}

int
open_www_file()
{
  char *url = NULL;

#ifndef WIN32
  atexit(htex_cleanup);
#endif
  if (debug & DBG_HYPER) {
    fprintf(stderr, "open_www_file(%s)\n", dvi_name);
  }

  /* open_www_file() may be called from main() at program startup, in
     which case we have to turn dvi_name into an absolute URL. */

  if (URLbase && !HTURL_isAbsolute(dvi_name)) {
    MyStrAllocCopy(&url, dvi_name);
#ifdef WIN32
    if (dvi_name)
      free(dvi_name);
    dvi_name = HTParse(url, "", PARSE_ALL);
#else
    MyStrAllocCopy(&dvi_name, HTParse(url, "", PARSE_ALL));
#endif
    free(url);
  }

  /* Otherwise, we're called with an absoulte URL in dvi_name, either
     from main() or form htex_do_url(). In both cases detach_anchor()
     has been called before open_www_file(). */
#ifdef WIN32
  if (FILESTRNCASEEQ(dvi_name, "ftp:", 4)
      || FILESTRNCASEEQ(dvi_name, "mailto:", 7)
      || FILESTRNCASEEQ(dvi_name, "http:", 5)
      || (strlen(dvi_name) > 5
	  && FILESTRNCASEEQ(dvi_name, "file:", 5)
	  && !FILESTRCASEEQ(dvi_name + strlen(dvi_name) - 4, ".dvi"))) {
      /* FIXME : this is a bit rough ! */
      ShellExecute(NULL, "open", dvi_name, NULL, NULL, SW_SHOWNORMAL);
      return 0;
  }
#else

  if (strcmp(figure_mime_type(dvi_name),"application/x-dvi")!=0) {

    /* Try other standard extensions: */
    if (!invokeviewer(dvi_name)) {
      /* Set the WWW browser on it: */
      if (browser) {
	char *command = xmalloc(strlen(dvi_name)
				+ strlen(browser) + 10);
	sprintf(command, "%s '%s' &", browser, dvi_name);
	if (debug & DBG_HYPER) {
	  fprintf(stderr, "Executing: %s\n", command);
	}
	system(command);
	free(command);
	return 0;
      } else {
#if 0 
	/* Don't call paint_anchor before window was opened! */
	paint_anchor("Error: WWWBROWSER not set for URL access\n");
#endif
	fprintf(stderr, "You need to set the environment variable WWWBROWSER\n");
	fprintf(stderr, "or specify -browser on the command line to access\n");
	fprintf(stderr, "networked URL's which can't be handled otherwise.\n");
      }
    }
  }
#endif /* WIN32 */
  else {
    if (HTeXnext_extern == 1) {
      /* External viewer */
      if (anchor_name != NULL) {
	invokedviviewer(dvi_name, anchor_name, 0);
	free(anchor_name); 
	anchor_name = NULL;
      } else {
	invokedviviewer(dvi_name, NULL, 0);
      }
      return 0;
    } else { 
      /* Dvi file, ='internal viewer' */
      open_dvi_file();
      if (dvi_file != NULL) {
	retitle(dvi_name);
	extractbase(dvi_name);
	return 1;
      } else {
	perror(dvi_name);
	Exit(1);
      }
    }
  }
  return 0;
}

/* from xhdvi/events.c */

/* APS Pointer locator: */

int
pointerlocate(xpos, ypos) /* Return screen positions */
int *xpos, *ypos;
{
	Window root, child;
	int root_x, root_y;
	unsigned int keys_buttons;

#ifdef WIN32
	POINT curPos = {0, 0}, ptOrg = {0, 0};
	RECT rcClient, rcClip, rcPage;
	BOOL bRet;
	extern HWND hWndDraw;
	extern int xCurrentScroll, yCurrentScroll;
	extern int xMousePos, yMousePos;

	/* Retrieve the page dimensions */
	rcPage.left = 0;
	rcPage.top = 0; 
	rcPage.right = page_w - 1;
	rcPage.bottom = page_h - 1;
	
	if (!GetWindowOrgEx(maneDrawDC, &ptOrg)) {
	  Win32Error("pointerlocate/GetWindowOrgEx");
	}
	
	curPos.x = *xpos = xMousePos + xCurrentScroll + ptOrg.x;
	curPos.y = *ypos = yMousePos + yCurrentScroll + ptOrg.y;
	/* Retrieve the screen coordinates of the client area, 
	   and convert them into client coordinates.  */
	GetClientRect(hWndDraw, &rcClient);
	/* Intersect both rectangles. The mag. glass will be limited
	   to the actual visible part of the page. */
	IntersectRect(&rcClip, &rcClient, &rcPage);
	bRet = PtInRect(&rcClip, curPos);
	return bRet;
#else
	return XQueryPointer(DISP, mane.win, &root, &child,
			&root_x, &root_y, xpos, ypos, &keys_buttons);
#endif
}

#endif /* HTEX */
#endif /* XHDVI || HTEX */
