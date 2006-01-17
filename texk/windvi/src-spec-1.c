/************************************************************************
src special support for xdvi(k) previewer
written by S. Ulrich (ulrich@cis.uni-muenchen.de) 8/1999

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

------------------------------------------------------------

NOTES ON IMPLEMENTATION

The method used here for evaluating the src specials is very simple:
During the scan of the .dvi page, applicationDoSpecial will call
src_eval_special which inserts the special just scanned into a
lookup array (in case src specials mode is ON; see below) containing
pointers to the horizontal lists of specials. The length of the
array is dynamically allocated to (page_h * shrink_factor).

While this data structure isn't terribly efficient because of the
`gaps' in the y-coordinates (\lineskip), it fulfils the following
two requirements which are important in our case:

- sequential searching has to be possible in case the user clicks
  somewhere between two specials, i.e. the structure has to be
  order-preserving w.r.t. the coordinates on the page;

- insertion should be fairly fast because redrawing the page
  will cause each special to be checked if it's already contained
  in the structure.
  
(Note that for these reasons a hash table would probably not
be better suited, because of the requirement of order-preserving
and the `gaps' in the data.)

This structure should work reasonably with any number of specials on
the page; while it might be wasteful if only few (< 50) specials are
used, it still works well with a large number of specials per page (it
can handle even the pathological case where one special is used for
each glyph on the page).

To call the editor for a certain special, the x/y coordinates of the
Xevent (mouse position) are handed to a function (src_find_special)
that checks if there is a special just at those coordinates, and if
not, it returns the next special on the current line or the first
special in the next line (doing a search in the horizontal list).

Here are some examples for this search (where `x' is the point of the
click, `_' the location of the specials, `^' marks the next special
that is found by the search, and dots are marking the baseline of
the text in the .dvi window):

   .x.............
   ...._....._....
       ^


   ......x........
   ...._....._....
       ^

   ...............
   ...._.x..._....
             ^


The basic idea is that there's a key that takes the user into `src
special' mode: In this mode, the location of the specials on the page
is indicated by little tick marks (indeed the list of specials is only
built and maintained with this mode being active), and a convenient
mouse/keyboard action will do a system() call with some editor command
for the next special found.  IMHO the advantages of such a special
mode are: The user gets some visual feedback where the next click will
take him (before he actually clicks somewhere), which is especially
important if the insertion technique for the specials (on the TeX
side) doesn't always produce predictable results; and the performance
in `normal mode' isn't affected by the source specials (list handling,
display stuff, and bugs ;).

************************************************************************/

#ifdef WIN32
#define XDVIK
#define HAVE_BOOLEAN
#define true True
#define false False
#endif

#ifdef XDVIK
#include "xdvi-config.h"
#include <kpathsea/c-fopen.h>
#include <kpathsea/c-stat.h>
#include <kpathsea/magstep.h>
#include <kpathsea/tex-file.h>
#include "dvi.h"
#define XMALLOC(x,y) xmalloc(x)
#define XREALLOC(x,y,z) xrealloc(x,y)

#else /* non-k version of xdvi */

#ifndef findrec
#include "filf_app.h"		/* application-related defs, etc. */
#include "filefind.h"
#endif /* findrec */
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>

#define XMALLOC(x,y) xmalloc(x,y)		/* has explanative string as 2nd argument */
#define XREALLOC(x,y,z) xrealloc(x,y,z)		/* has explanative string as 3rd argument */
#endif /* XDVIK */

#include <unistd.h> 
#include <limits.h> 
#include <ctype.h>

#ifdef SRC_SPECIALS

/*
 * Define a reasonable maximum length of the lookup array, because
 * the variable the value is usually taken from (page_h) might
 * overflow on a 32-bit architecture; here are examples for the size
 * of page_h at various font resolutions on my Intel Linux box:
 *
 *    resolution		page_h (at shrink factor 10)
 *
 *		600dpi			 704
 *	   1200dpi			1406
 *     2400dpi			2812
 *	   2540dpi			2972
 *     3386dpi			6601
 *     5333dpi			429494415
 *
 * It's obvious that the last number results from an overflow at
 * that resolution, while all the other values are inside some reasonable
 * range << 100 000 (the worst case would be ten times the above values
 * for shrink factor 1). (This overflow also shows in the display:
 * the vertical scrollbar is very small, and the glyphs below a certain
 * y coordinate are not drawn.)
 *
 * Using (page_w * 2) instead could be a possible workaround, but that's
 * no clean solution either (src specials exceeding the y maximum would
 * still cause `out of bounds' errors in the `src_insert_into_array' function).
 *
 * So we simply define the maximum size of the lookup array to be 100000.
 */

#define ARR_MAX_SIZE 100000 


/* from special.c */
#define	xRESOLUTION	(pixels_per_inch/shrink_factor)
#define	yRESOLUTION	(pixels_per_inch/shrink_factor)

#define src_is_smaller(x,y) ((x->y_coord < y->y_coord) || ((x->y_coord == y->y_coord) && (x->x_coord < y->x_coord)))
#define src_is_equal(x,y) ((x->y_coord == y->y_coord) && (x->x_coord == y->x_coord))


/* get the values of these from Xresources or command-line options */



#define src_specialFormat resource._src_specialFormat
#define src_tickSize      resource._src_tickSize



/* kludgy check if magnifier is off; is this always defined/correct?? */
#define MAGNIFIER_NOT_ACTIVE (mane.shrinkfactor == currwin.shrinkfactor)

#define PRELIM_VERSION


/* double-linked list structure for src specials
 * 
 * Note: currently only the `next' pointer of the list is used;
 * so a single-linked list would also do. Not sure if we might not
 * need the `previous' pointer in the future, though.
 */

struct SRC_special {
  char *filename;		/* filename contained in the special */
  unsigned int lineno;  /* line number contained in the special */
  int pageno;			/* number of displayed page on which special is drawn */
  float pt_size;		
  /*
   * pixel coordinates in xdvi display
   * (take the values of PXL_H, PXL_V; hence int)
  */
  int x_coord;
  int y_coord;
  /* pointers to next/previous elem in list */
  struct SRC_special *next_special;
  struct SRC_special *prev_special;
};

/*
 * To remember specials that caused warnings,
 * for displaying these warnings only once:
 */
struct SRC_warn_special {
  char *src_string;
  struct SRC_warn_special *next_warn_special;
};

#ifndef XDVIK

static	_Xconst	char	no_f_str_tex[]	= "/%f";

static	struct findrec search_tex	= {
	/* path1	*/	NULL,
#if	CFGFILE
	/* envptr	*/	NULL,
#endif
	/* path2	*/	DEFAULT_HEADER_PATH,
	/* type		*/	"TeX file",
	/* fF_etc	*/	"fF",
	/* x_var_char	*/	'f',
	/* n_var_opts	*/	2,
	/* no_f_str	*/	no_f_str_tex,
	/* no_f_str_end	*/	no_f_str_tex + sizeof(no_f_str_tex) - 1,
	/* abs_str	*/	"%f",
#ifdef	PK_AND_GF
	/* no_f_str_flags */	F_FILE_USED,
	/* abs_str_flags */	F_FILE_USED,
	/* pk_opt_char	*/	'f',
	/* pk_gf_addr	*/	NULL,
#endif
	/* pct_s_str	*/	"%q//",
	{
	  /* v.stephead		*/	NULL,
	  /* v.pct_s_head	*/	NULL,
	  /* v.pct_s_count	*/	0,
	  /* v.pct_s_atom	*/	NULL,
	  /* v.rootp		*/	NULL,
	}
};
#endif /* not XDVIK */

/*
 * maximum length of long int in octal representation; compare C FAQ.
 * CHAR_BIT is from limits.h (may we assume 8 if it's unavailable?)
 * I *think* the +2 is for rounding up the length to include the full triad;
 * the / 3 computes the length of the octal digit.
 * The + 1 is for the minus sign. Note that the value is the same for int as for
 * unsigned int, since there the minus sign might be taken by another digit
 * (the maximum value of unsigned being twice as much as the maximum value for signed).
 * How about the trailing `\n' ?
 */
#define LONG_SIZE ((sizeof(long) * CHAR_BIT + 2) / 3 + 1) 
#define UINT_SIZE ((sizeof(int) * CHAR_BIT + 2) / 3 + 1)

/* global variables */
struct SRC_special *head_special = NULL;
struct SRC_warn_special *head_warn_special = NULL;
struct SRC_special *last_active_special = NULL;
int save_shrinkfactor = 0;

static Boolean specials_not_initialized = True;
static Boolean src_arr_not_initialized = True;
static int corr_fact;
static	struct stat statbuf;

/*
 * A global array holding pointers to src special lists.
 * The array is implemented using pointers so that its
 * size can be set at run-time (inside src_initialize_array).
 * Throughout the program, this array will be treated as an 
 * ordinary array, i.e. we'll write
 *		src_arr[i]
 * instead of
 *		*(src_arr+i)
 */

struct SRC_special **src_arr;
static unsigned int src_arr_len;

/*
  ========================================================================
						Private Functions
  ========================================================================
*/

/*************************************************************************
  small helper function: print out the list of specials
 *************************************************************************/
static void
print_out_list(top)
	 struct SRC_special *top;
{
  struct SRC_special *curr;
  int j;
  for (curr = top, j=0; curr != NULL; curr = curr->next_special, j++) {
	Fprintf(stderr, "[%d] %s:%u (%d,%d)\n",
			j, curr->filename, curr->lineno, curr->x_coord, curr->y_coord);
  }
 
}

/* currently unused */
static void
print_out_warn_list()
{
  struct SRC_warn_special *curr;
  int j;
  for (curr = head_warn_special, j=0; curr != NULL; curr = curr->next_warn_special, j++) {
	Fprintf(stderr, "%d: %s\n",
			j, curr->src_string);
  }
 
}

/*************************************************************************
  print out the list of specials in reverse order
  currently unused
 *************************************************************************/
static void
print_out_reverse_list()
{
  struct SRC_special *curr;
  int j;
  for (curr = head_special, j=0; curr->next_special != NULL; curr = curr->next_special, j++) { ; }
  for (; curr != NULL; curr = curr->prev_special, j--) {
   	Fprintf(stderr, "[%d] %s:%u (%d,%d)\n",
			j, curr->filename, curr->lineno, curr->x_coord, curr->y_coord);
   	if (j > 500) {return;}
  }
  
}


/*************************************************************************
  draw glyphs representing the specials on a page 
 *************************************************************************/
static void
draw_src_glyph(x_coord, y_coord, gc)
	 int x_coord, y_coord;
	 GC gc;
{
  /*
   * The height/width of the shapes we're going to draw.
   * Height/width have their `natural' meaning only for the rectangle.
   * For the equilateral triangle, height is the base of the triangle.
   * For the angles, height is the length of the lines, width is twice
   * the thickness of the lines.
   */
  
#ifdef WIN32
  POINT point1, point2, point3;
  POINT triangle_points[3];
  extern int xCurrentScroll, yCurrentScroll;

  x_coord -= xCurrentScroll;
  y_coord -= yCurrentScroll;

  switch (src_tickShape) {
  case 0:
    {
	  /* rectangle */
	  RECT r;
	  r.left = x_coord;
	  r.top = y_coord;
	  r.right = r.left + src_tick_width/mane.shrinkfactor;
	  r.bottom = r.top + src_tick_height/mane.shrinkfactor;
	  /* FIXME: this is wrong !
	   The use of foreGC / highGC / ruleGC should be sorted out ! */
	  FillRect(gc, &r, foreBrush);
	}
	break;
  case 1:
    /* equilateral triangle */
	/*
	 * Since XFillPolygon will be drawn with `CoordModePrevious', each point
	 * will be specified relative to its predecessor.
	 */
	/* top */
	point1.x = x_coord;
	point1.y = y_coord;
	/* lower left corner */
	point2.x = x_coord + (src_tick_height/shrink_factor)/2;
	point2.y = y_coord + src_tick_height/shrink_factor;
	/* lower right corner */
	point3.x = x_coord + src_tick_height/shrink_factor;
	point3.y = y_coord;
	
	triangle_points[0] = point1;
	triangle_points[1] = point2;
	triangle_points[2] = point3;
	Polygon(gc, triangle_points, 3);
	break;
  case 2:
    /*
     * angle:  _|
     */
    XFillRectangle(DISP,
				   mane.win,
				   gc,
				   x_coord - src_tick_height/mane.shrinkfactor + ((src_tick_width/2) / mane.shrinkfactor),
				   y_coord,
				   src_tick_height/mane.shrinkfactor,			/* length */
				   (src_tick_width/2)/mane.shrinkfactor		    /* height */
				   );
    /* vertical line */
    XFillRectangle(DISP,
				   mane.win,
				   gc,
				   x_coord,
				   /*
					* increase height for (height/10) to compensate for optical error
					* that makes vertical lines appear shorter
					*/
				   y_coord - (src_tick_height/mane.shrinkfactor - (src_tick_width/mane.shrinkfactor/2)),
				   
				   (src_tick_width/2)/mane.shrinkfactor,								/* height */
				   /* like before */
				   src_tick_height/mane.shrinkfactor - (src_tick_width/mane.shrinkfactor/2)
				   
				   );
	break;
  case 3:
    /*
     * angle, rotated 180 degs:  _
     *                          |
     */
    XFillRectangle(DISP,
				   mane.win,
				   gc,
				   x_coord,
				   y_coord,
				   src_tick_height/mane.shrinkfactor,
				   src_tick_width/mane.shrinkfactor/2
				   );
    /* vertical line */
    XFillRectangle(DISP,
				   mane.win,
				   gc,
				   x_coord,
				   y_coord + src_tick_width/mane.shrinkfactor/2,	
				   src_tick_width/mane.shrinkfactor/2,
				   src_tick_height/mane.shrinkfactor - src_tick_width/mane.shrinkfactor/2
				   );
	break;
  case 4:
	/* Ellipse */
	{
	  /* FIXME: this is wrong !
		 The use of foreGC / highGC / ruleGC should be sorted out ! */
	  Ellipse(gc, x_coord, y_coord,
			  x_coord + src_tick_width/mane.shrinkfactor,
			  y_coord + src_tick_height/mane.shrinkfactor);
	}
	break;
  }
#else
  XPoint point1, point2, point3;
  XPoint triangle_points[3];
  
  /* used to have this initialization which required that height and width are constant:
	 XPoint points[] = {
	    {x, y},
	    {((-height/shrink_factor)/2), (height/shrink_factor)},
	    {(height/shrink_factor), 0}
	 };
  */

  
  
  
  
  switch (src_tickShape) {
  case 0:
    /* rectangle */
    XFillRectangle(DISP, mane.win, gc, x_coord, y_coord, src_tick_width/mane.shrinkfactor, src_tick_height/mane.shrinkfactor);
    break;
  case 1:
    /* equilateral triangle */
	/*
	 * Since XFillPolygon will be drawn with `CoordModePrevious', each point
	 * will be specified relative to its predecessor.
	 */
	/* top */
	point1.x = x_coord;
	point1.y = y_coord;
	/* lower left corner */
	point2.x = (-src_tick_height/shrink_factor)/2;
	point2.y = src_tick_height/shrink_factor;
	/* lower right corner */
	point3.x = src_tick_height/shrink_factor;
	point3.y = 0;
	
	triangle_points[0] = point1;
	triangle_points[1] = point2;
	triangle_points[2] = point3;
	
    XFillPolygon(DISP,
				 mane.win,
				 gc,
				 triangle_points,
				 (sizeof(triangle_points)/sizeof(XPoint)),
				 Convex,
				 CoordModePrevious);
    break;
  case 2:
    /*
     * angle:  _|
     */
    /* horizontal line */
    XFillRectangle(DISP,
				   mane.win,
				   gc,
				   x_coord - src_tick_height/mane.shrinkfactor + ((src_tick_width/2) / mane.shrinkfactor),
				   y_coord,
				   src_tick_height/mane.shrinkfactor,			/* length */
				   (src_tick_width/2)/mane.shrinkfactor		    /* height */
				   );
    /* vertical line */
    XFillRectangle(DISP,
				   mane.win,
				   gc,
				   x_coord,
				   /*
					* increase height for (height/10) to compensate for optical error
					* that makes vertical lines appear shorter
					*/
				   y_coord - (src_tick_height/mane.shrinkfactor - (src_tick_width/mane.shrinkfactor/2)),
				   
				   (src_tick_width/2)/mane.shrinkfactor,								/* height */
				   /* like before */
				   src_tick_height/mane.shrinkfactor - (src_tick_width/mane.shrinkfactor/2)
				   
				   );
    break;
  case 3:
    /*
     * angle, rotated 180 degs:  _
     *                          |
     */
    /* horizontal line */
    XFillRectangle(DISP,
				   mane.win,
				   gc,
				   x_coord,
				   y_coord,
				   src_tick_height/mane.shrinkfactor,
				   src_tick_width/mane.shrinkfactor/2
				   );
    /* vertical line */
    XFillRectangle(DISP,
				   mane.win,
				   gc,
				   x_coord,
				   y_coord + src_tick_width/mane.shrinkfactor/2,	
				   src_tick_width/mane.shrinkfactor/2,
				   src_tick_height/mane.shrinkfactor - src_tick_width/mane.shrinkfactor/2
				   );
    break;
  }
#endif
}


/*************************************************************************
  check if string `my_string' occurs already in the SRC_warn_special list
 *************************************************************************/

static Boolean
occurs_in_warn_specials(my_string)
	 char *my_string;
{
  struct SRC_warn_special *curr;
  for (curr = head_warn_special; curr != NULL; curr = curr->next_warn_special) {
	if (strcmp(my_string, curr->src_string) == 0) { /* matched */
	  return True;
	}
  }
  return False;
}

/*************************************************************************
  insert SRC_special `new' into list pointed to by *list, return
  pointer to head of the list. Assumes that the space for `new' and
  its field `filename' have been malloc-ed by the caller.
 *************************************************************************/

static struct SRC_special *
insert_into_src_specials(top, new)
     struct SRC_special *top, *new;
{
  struct SRC_special *curr, *bak;
  
  
  if (top == NULL) { /* list empty */
    if (debug & DBG_SRC_SPECIALS) {
	  
    }
   	new->next_special = NULL;
   	new->prev_special = NULL;
    
   	return new;
  }
  /* arrive here only if list not empty. */
  curr = top;
  bak = NULL;
  
  /*
   * advance in list while next item is `smaller' than new item,
   * eventually reaching the end of the list. bak is a save pointer
   * to the previous item.
   */
  while (curr != NULL && src_is_smaller(curr, new)) {
   	bak = curr;
   	curr = curr->next_special;
  }
  
  /* don't append elems already in the list; top is unchanged: */
  if (curr != NULL && src_is_equal(curr, new)) {
	
   	free(new->filename);
   	free(new);
   	return top;
  }
  
  /*
   * now curr is NULL (end of list) or `bigger' item; bak is pointer to
   * the previous element (to NULL if we're still at the head of the list).
   * Use these to assign prev/next to new (doesn't matter if they are NULL):
   */
  new->next_special = curr;
  new->prev_special = bak;
  if(bak != NULL) {
   	/*
   	 * we're not not at the head of list. Make `new' the successor of previous;
   	 * top is unchanged:
   	 */
   	bak->next_special = new;
  }
  else {
   	/* we're at the head of the list; `new' becomes new top: */
   	top = new;
  }
  if(curr != NULL) {
   	/*
   	 * we're not at the end of the list. Make `new' predecessor of next;
   	 * top is unchanged:
   	 */
   	curr->prev_special = new;
  }
  if (debug & DBG_SRC_SPECIALS) {
	
	
  }
  return top;
}

#ifdef TEST

/*************************************************************************
  delete `elem' from specials list pointed to by `top'
 *************************************************************************/

static struct SRC_special *
delete_from_src_specials(top, elem)
	 struct SRC_special *top, *elem;
{
  struct SRC_special *curr, *tmp;
  int j;
  /* search elem to delete */
  for (curr = top; (curr != NULL) && (!src_is_equal(curr, elem)); curr = curr->next_special) {
	;	/* empty */
  }

  if (curr != NULL) {							/* found elem */
	if (top == elem) {							/* delete from top */
	  top = elem->next_special;
	  if (top) {
		top->prev_special = NULL;
	  }
	  else {
		if (debug & DBG_SRC_SPECIALS) {
		  Fprintf(stderr, "removing last elem in list\n");
		}
		
	  }
	}
	else {										/* delete from middle or end */
	  elem->prev_special->next_special = elem->next_special;
	  if (elem->next_special != NULL) {			/* delete from middle */
		elem->next_special->prev_special = elem->prev_special;
	  }
	}
	free(elem->filename);
	free(elem);
  }
  if (debug & DBG_SRC_SPECIALS) {
	Fprintf(stderr, "List after delete:\n");
	for (tmp = top, j=0; tmp != NULL; tmp = tmp->next_special, j++) {
	  Fprintf(stderr, "[%d] %s:%u (%d,%d)\n", j, tmp->filename, tmp->lineno, tmp->x_coord, tmp->y_coord);
	}
  }
  return top;
}
#endif

static struct SRC_special *
delete_from_pages(top, page)
	 struct SRC_special *top;
	 int page;
{
  struct SRC_special *curr, *tmp;

  
  /* loop throuhg list, deleting all elems with pageno == page */
  for (curr = top; curr != NULL; curr = tmp) {
	tmp = curr->next_special;
	if (curr->pageno == page) {
	  if (curr == top) {							/* delete from top */
		top = curr->next_special;
		if (top) {
		  top->prev_special = NULL;
		}
	  }
	  else {										/* delete from middle or end */
		curr->prev_special->next_special = curr->next_special;
		if (curr->next_special != NULL) {			/* delete from middle */
		  curr->next_special->prev_special = curr->prev_special;
		}
	  }
	  free(curr->filename);
	  free(curr);
	}
  }
  return top;
}


/*************************************************************************
 Print warning or error message (only once) if a special string doesn't
 conform to the current format selected. level = 0 is warning, level = 1
 is error.
*************************************************************************/
static void
src_special_format_error(level, input_string, err_msg)
	 int level;
	 char *input_string, *err_msg;
{
  int string_len;
  char *bad_src_string;
  struct SRC_warn_special *new_warn_special;
  static int have_mentioned_man_page = 0;
  
  if (!occurs_in_warn_specials(input_string)) {
	switch(level) {
	case 0:
	  Fprintf(stderr, "\nWarning: src special \"%s\"%s\n", input_string, err_msg);
	  break;
	case 1:
	  Fprintf(stderr, "\nError: src special \"%s\"%s\n", input_string, err_msg);
	  break;
	}
	if (have_mentioned_man_page == 0) {
	  Fprintf(stderr, "(See the xdvi man page on \"SRC special format\" for more information)\n");
	  have_mentioned_man_page++;
	}
	/* remember this special as warned about */
	string_len = strlen(input_string);
	bad_src_string = XMALLOC(string_len + 1, "remember bad special");
	Sprintf(bad_src_string, "%.*s", string_len, input_string);
	new_warn_special = (struct SRC_warn_special *) XMALLOC(sizeof(struct SRC_warn_special), "warn special");
	new_warn_special->src_string = bad_src_string;
	/* insert new_warn_special at begin of SRC_warn_special list: */
	new_warn_special->next_warn_special = head_warn_special;
	head_warn_special = new_warn_special;
  }
}

/**********************************************************************
  Parse the special string pointed to by `input_string' and return
  pointer to a SRC_special element containing the necessary
  information, or NULL if parsing `input_string' failed.
 **********************************************************************/   
static struct SRC_special *
src_parse_special_string(input_string, x, y)
	 char *input_string;
	 Position x, y;
{
  struct SRC_special *new_special;
  char *my_filename, *my_lineno, *save_special;
  
  int filename_string_len = 0, lineno_string_len = 0;
  
  unsigned long numtest;				/* for testing if line number fits into unsigned int */
  Boolean read_leading_garbage = False; /* true if we had to read over leading garbage */
  /*
   * preliminary stuff to account for various formats of the src specials.
   * 0 is: `<filename>:<lineno>'
   * 1 is: `<lineno> <filename>'
   * 2 is: `<lineno>[ ]*<filename>'
   *
   * where (0) has been used by myself for test purposes, (1) is used
   * by YAP, (2) (where there may be any (including 0) number of spaces
   * separating lineno and filename) is used by YAP and DVIWIN.
   * (note that if there are zero spaces, the filename mustn't start with
   * a digit; hence this format isn't really recommendable).
   * So (1) is a special case of (2); you might use it if you want to enforce the
   * use of white-space as separator.
   * 
   * I still have to find out which format the viewers/editors on some
   * (commercial) MAC implementations use.
   */
  
  new_special = (struct SRC_special *) XMALLOC(sizeof(struct SRC_special), "new special");
  save_special = input_string;
  
  switch(src_specialFormat) {
  case 0:
	/* this case is for testing only, so it doesn't contain many sanity checks that
	 * have been included in the tests below.
	 */ 
	if((my_lineno = rindex(input_string, ':')) == NULL) {
	  src_special_format_error(1, input_string, " doesn't contain a `:' (skipped)");
	  return NULL;
	}

	/* first char in my_lineno is now `:' - read over it: */
	my_lineno++;
	lineno_string_len = strlen(my_lineno);
	filename_string_len = strlen(input_string) - lineno_string_len - 1; /* - 1 for the `:' */
	
	my_filename = XMALLOC(filename_string_len + 1, "filename in special");
	(void) strncpy(my_filename, input_string, filename_string_len);
	my_filename[filename_string_len] = '\0';
	break;
	
  default: /* treat src_specialFormat == 1 and 2 simultaneously */
	/*
	 * Delete everything that isn't a digit from the start of
	 * input_string (including, eventually, minus signs etc.) Will
	 * check below for read_leading_garbage and issue a warning in
	 * this case (after fatal errors have been processed).
	 */
 	while(*input_string != '\0' && !isdigit(*input_string)) {
	  input_string++;
	  read_leading_garbage = True;
	}
	my_lineno = input_string;
	lineno_string_len = 0;

	if (src_specialFormat == 1) {
	  /* read line number until finding a space: */
	  while (*input_string != '\0' && *input_string != ' ') {
		input_string++;
		lineno_string_len++;
	  }
	}
	else {
	  while (*input_string != '\0' && isdigit(*input_string)) {
		input_string++;
		lineno_string_len++;
	  }
	}

	/* did we read a line number? */
	if(lineno_string_len == 0) {
	  src_special_format_error(1, save_special, " doesn't contain a digit (skipped)");
	  return NULL;
	}

	if (src_specialFormat == 1) {
	  /* read over single whitespace */
	  if (*input_string != '\0' && *input_string == ' ') {
		input_string++;
	  }
	  else {
		src_special_format_error(1, save_special, " doesn't contain a space (skipped)");
		return NULL;
	  }
	}
	else {
	  /* read over any amount of whitespace */
	  while (*input_string != '\0' && isspace(*input_string)) {
		input_string++;
	  }
	}
	
	filename_string_len = strlen(input_string);
	
	/* is the remaining chunk of input_string empty? */
	if(filename_string_len == 0) {
	  src_special_format_error(1, save_special, " contains only digits and no filename (skipped)");
	  return NULL;
	}
	
	/* if everything else was OK, warn if special had contained leading garbage: */
	if (read_leading_garbage) {
	  src_special_format_error(0, save_special, ": leading non-digits deleted");
	}

	/* finally, allocate and copy filename */
	my_filename = XMALLOC(filename_string_len + 1, "filename in special");
	(void) strncpy(my_filename, input_string, filename_string_len);
	my_filename[filename_string_len] = '\0';
	break;
  }
  new_special->filename = my_filename;

  /*
   * similar to atol(my_lineno), but converts to unsigned long int
   * and does additional error checking. First set errno to 0 so that
   * it won't contain the result of calls:
   */
  errno = 0;
  numtest = strtoul((my_lineno), (char **)NULL, 10);
  new_special->lineno = (unsigned int)numtest;

	/*
	 * If number has been too long for unsigned long int (ERANGE),
	 * it's definitely too long for unsigned int either.
	 * If not, check if it is too long for unsigned int.
	 * Note that the test for ERANGE isn't redundant, since on
	 * Intel Linux UINT_MAX == ULONG_MAX.
	 */
  if ((errno == ERANGE) || (numtest > UINT_MAX)) {
	Fprintf(stderr, "\nWarning: line number %s too long for data type \"unsigned int\";\nchopping to %u\n",
			my_lineno, new_special->lineno);
  }
  /*
   * If special is in a text line, currinf.fontp->pt_size
   * should contain the pt size of the font.
   */
  if (currinf.fontp != NULL) {
	new_special->pt_size = currinf.fontp->pt_size;
  }
  else {
	/*
	 * Assume some reasonable default value. Note that this is not
	 * critical, since currinf.fontp == NULL means that the current
	 * special doesn't occur in a text line at all
	 * (but before all text on the current page).
       */
	new_special->pt_size = 12.0;
  }
  new_special->x_coord = x;
  new_special->y_coord = y;
  new_special->pageno = current_page;
	
  if (debug & DBG_SRC_SPECIALS) {
	
  }
  return new_special;
}


/*************************************************************************
  Build up the editor command for the system call and perform the system
  call.
 *************************************************************************/
static void
issue_src_editor_cmd(filename, lineno)
	 char *filename;
	 long lineno;
{
  char *cmdstring;
  
  char *warnmsg;
  _Xconst char *kpse_pathname; /* used to hold the expanded path name of the file in the special */
  
  
  /*
   * Here's the very simple algorithm for finding the files contained
   * in the specials: First check if file is in current directory; if
   * so, open it.  Else, use kpse_find_tex (special form of
   * kpse_find_file) if kpathsearch library is available, or the
   * `filefind' function from non-k xdvi to expand the path name.
   * This should nearly always succeed, even if the path may not be
   * exactly what you wanted ... however you'll get decent results when
   * calling xdvi from the same directory as LaTeX, which is (I hope ;)
   * quite common.
   */
  if (stat(filename, &statbuf) == 0) {
	if (debug & DBG_SRC_SPECIALS) {
	  Fprintf(stderr, "Found file \"%s\" in current dir\n", filename);
	}
	kpse_pathname = filename;
  }
  else {
#ifdef XDVIK
	/* make a kpathsea search for filename */
	if (debug & DBG_SRC_SPECIALS) {
	  Fprintf(stderr, "file \"%s\" not found in cwd, trying kpathsea expansion\n", filename);
	}
	kpse_pathname = 
	  kpse_find_tex(filename);
	if (stat(kpse_pathname, &statbuf) != 0) {	  
	  Fprintf(stderr, "Error: file `%s' not found, couldn't jump to special `%s:%ld'\n",
			  filename, filename, lineno);
	  return;
	}
	else {
	  if (debug & DBG_SRC_SPECIALS) {
		Fprintf(stderr, "Found file: `%s'\n", kpse_pathname);
	  }
	}
#else	/* XDVIK */
	/*
	 * search for filename in $TEXINPUTS, using the filefind function from
	 * non-k xdvi's filefind.c
	 */
	if (debug & DBG_SRC_SPECIALS) {
	  Fprintf(stderr, "file \"%s\" not found in cwd, trying TEXINPUTS\n", filename);
	}
	search_tex.path1 = getenv("TEXINPUTS");
#if	CFGFILE
 	   search_tex.envptr = ffgetenv("TEXINPUTS");
 	   if (search_tex.envptr != NULL && search_tex.envptr->value == NULL) {
 		 search_tex.envptr = NULL;
 	   }
#endif	/* CFGFILE */
	(void) filefind(filename, &search_tex, &kpse_pathname);
	if (stat(kpse_pathname, &statbuf) != 0) {
	  Fprintf(stderr, "Error: file `%s' not found, couldn't jump to special `%s:%ld'\n",
			  filename, filename, lineno);
	  return;
	}
	else {
	  if (debug & DBG_SRC_SPECIALS) {
		Fprintf(stderr, "Found file: `%s'\n", kpse_pathname);
	  }
	}
#endif   /* XDVIK */
  }
  /*
   * Warn if source file is newer than .dvi file (the line number
   * information in the specials might be outdated in this case).
   */
  if (statbuf.st_mtime > dvi_time) {
	if (src_warn_verbosity >= SRC_WARNINGS_VERBOSE) {
	  warnmsg = XMALLOC((unsigned)strlen(kpse_pathname) +
						(unsigned)strlen("Warning: File %s is newer than dvi file!") + 2,
						"warning message");
	  Sprintf(warnmsg, "Warning: File %s is newer than dvi file!", kpse_pathname);
	  showmessage(warnmsg);
	  free(warnmsg);
	  XFlush(DISP);
	}
	else {
	  if (src_warn_verbosity >= SRC_WARNINGS_MEDIUM) {
		Fprintf(stdout, "\nWarning: file \"%s\" is newer than .dvi file;\n", kpse_pathname);
		Fprintf(stdout, "         line number in special might be wrong!\n\n");
	  }
	}
  }

  /* now allocate space for the command string, and issue the editor command */
	
  /* + 1 is for `\n' */
  cmdstring = XMALLOC((unsigned)strlen(src_editorCommand) + (unsigned)strlen(kpse_pathname) + LONG_SIZE + 1,
					  "editor command string");
  if (src_EditorCmd_filename_first) { /* set inside src_get_settings, called by main() */
	Sprintf(cmdstring, src_editorCommand, kpse_pathname, lineno);
  }
  else {
	Sprintf(cmdstring, src_editorCommand, lineno, kpse_pathname);
  }
  if (src_warn_verbosity >= SRC_WARNINGS_MEDIUM) {
	Fprintf (stderr, "Calling: %s\n", cmdstring);
  }
  (void) system(cmdstring);
  free(cmdstring);
}

/*
  ========================================================================
						Public Functions
  ========================================================================
*/



static Boolean
src_arr_too_long(len)
	 unsigned int len;
{
  if (len > ARR_MAX_SIZE) {
	Fprintf(stderr, "Error: Requested lookup array size %u exceeds sensible limit of %d\n", len, ARR_MAX_SIZE);
	Fprintf(stderr, "(Reason might be an overflow in page_h)\n", ARR_MAX_SIZE);
	Fprintf(stderr, "Disabling src specials at this resolution/magnification.\n\n", ARR_MAX_SIZE);
	src_arr_len = 0;
	return True;
  }
  return False;
}

static void
src_initialize_array(len)
	 unsigned int len;
{
  unsigned int j;

  /*
   * Note that src_arr is never free()d. Should we do
   * something like `atexit(cleanup_src_arr)?
   * However I think every real OS takes care of that
   * (and if it doesn't, it probably also doesn't have
   * `atexit'; and in any case all this would only work
   * in case of normal program termination anyway ...)
   */
  
  if (src_arr_too_long(len)) {
	return;
  }

  
  fprintf(stderr, "initializing array of len: %u; page_h: %u; shrink_factor: %d\n", src_arr_len, len, shrink_factor);
	
  src_arr = XMALLOC((len * sizeof(struct SRC_special *)), "src specials lookup array");

  for (j = 0; j < len; j++) {
	src_arr[j] = NULL;
  	
  }
}

static void
src_resize_array(len)
	 unsigned int len;
{
  unsigned int j;
  
  if (src_arr_too_long(len)) {
	return;
  }

  fprintf(stderr, "resizing array to len: %d; page_h: %d; shrink_factor: %d\n", src_arr_len, page_h, shrink_factor);
	
  src_arr = XREALLOC(src_arr, len * sizeof(struct SRC_special *), "src specials lookup array");

  for (j = 0; j < len; j++) {
	src_arr[j] = NULL;
  	
  }
}

/*************************************************************************
  Called from src_eval_special (once, when the first special is found);
  assigns values to the global variables:
  
  src_EditorCmd_filename_first
  src_tick_width
  src_tick_height
 
  by parsing the settings of src_editorCommand and src_tickSize,
  and initializes the lookup array src_arr.
 *************************************************************************/
void
src_get_settings(void)
{

  char *line_substring, *file_substring;
  char *my_size;
  char *test_src_editorCommand;

  if (src_editorCommand == NULL) { /* in case there's something wrong with the default settings */
	Fprintf(stderr, "\nShouldn't happen: Resource \"srcEditorCommand\" has no default value!\n");
	Fprintf(stderr, "Please report this as a bug to the maintainer, along with this error message.\n");
	src_editorCommand = "emacsclient --no-wait '+%u' '%s'";
	Fprintf(stderr, "(Assuming \"%s\" to recover.)\n", src_editorCommand);
  }
  
  if((file_substring = strstr(src_editorCommand, "%s")) == NULL) {
	Fprintf(stderr, "\nError: srcEditorCommand %s doesn't contain a \"%%s\"\n(to hold the file name)\n",
			src_editorCommand);
	src_editorCommand = "emacsclient --no-wait '+%u' '%s'";
	Fprintf(stderr, "Using built-in default: \"%s\"\n", src_editorCommand);
	src_EditorCmd_filename_first = False;
  }
  else {
	if((line_substring = strstr(src_editorCommand, "%u")) == NULL) {
	  Fprintf(stderr, "\nError: srcEditorCommand %s doesn't contain a \"%%u\"\n(to hold the line number)\n",
			  src_editorCommand);
	  src_editorCommand = "emacsclient --no-wait '+%u' '%s'";
	  Fprintf(stderr, "Using built-in default: \"%s\"\n", src_editorCommand);
	  src_EditorCmd_filename_first = False;
	}
	else {
	  /*
	   * warn if src_editorCommand doesn't start with a command, i.e.
	   * doesn't contain at least one alphabetic character in front of
	   * the format string.
	   */
	  test_src_editorCommand = src_editorCommand;
	  while (!isalpha(*test_src_editorCommand) && *test_src_editorCommand != '%') {
		test_src_editorCommand++;
	  }
	  if (*test_src_editorCommand == '%') {
		/* ring all bells */
		MessageBeep(MB_ICONEXCLAMATION);
		Fprintf(stderr, "\n\n****************************************************************\n");
		Fprintf(stderr, "WARNING: srcEditorCommand string %s doesn't seem to\n", src_editorCommand);
		Fprintf(stderr, "contain a command name -- this might be a security problem!\n");
		Fprintf(stderr, "Be sure to read the section on `src specials and security'\n");
		Fprintf(stderr, "in the xdvi man page before proceeding.\n");
		Fprintf(stderr, "****************************************************************\n\n");
	  }
	  if (strlen(file_substring) > strlen(line_substring)) {		/* %s before %u */
		if (debug & DBG_SRC_SPECIALS) {
		  Fprintf(stderr, "SRC info: filename comes first in editor command\n");
		}
		src_EditorCmd_filename_first = True;
	  }
	  else {														/* %u before %s */
		if (debug & DBG_SRC_SPECIALS) {
		  Fprintf(stderr, "SRC info: line number comes first in editor command\n");
		}
		src_EditorCmd_filename_first = False;
	  }
	}
  }

  /* get settings for src_tick_width, src_tick_height */
  if(src_tickSize == NULL) { /* in case there's something wrong with the default settings */
	Fprintf(stderr, "\nShouldn't happen: Resource \"srcTickSize\" has no default value!\n");
	Fprintf(stderr, "Please report this as a bug to the maintainer, along with this error message.\n");
	src_tickSize = "40x70";
	Fprintf(stderr, "(Assuming \"%s\" to recover.)\n", src_tickSize);
  }
  if ((my_size = rindex(src_tickSize, 'x')) == NULL) {
	src_tickSize = "40x70";
	src_tick_height = 70;
	src_tick_width = 40;
	Fprintf(stderr, "\nError: argument for \"-srcTickSize\" not in `geometry' format\n");
	Fprintf(stderr, "(e.g.: \"40x70\"); using default: %s\n", src_tickSize);
  }
  else {
	my_size++;
	fflush(stderr);
	src_tick_height = atoi(my_size);
	src_tick_width = atoi(src_tickSize);
  }
}  


/*************************************************************************
  perform a search to find the next src special starting at x,y.
  `jump' signals that we want to issue the editor command (if jump == 1)
  or just highlight the special found (jump == 0).
 *************************************************************************/
void
src_find_special(jump,x,y)
  int jump;				    /* 1 if we are to call the editor command, 0 else */
  Position x,y;				/* coordinates of Xevent that triggered the search */
{
  struct SRC_special *curr;
  Position i = y;		/* don't modify y, will be needed later */
  long last_line_y;

  if(src_arr_len == 0) {		/* return immediately if searching is disabled */
	MessageBeep(MB_ICONINFORMATION);
	fprintf(stdout, "No more specials on this page.\n");
	return;
  }
  
  /*
   * last_active_special (last item searched) will have highlight colour; redraw with
   * ordinary colour if last_active_special was on current page:
   */
  if (last_active_special != NULL && src_tickVisibility) {
    draw_src_glyph(last_active_special->x_coord, last_active_special->y_coord, highGC);
  }
	

  /* start searching for special after current point */
  
  if (debug & DBG_SRC_SPECIALS) {
	Fprintf(stderr, "Current point: x: %d, y: %d (%d,%d)\n", x, y, PXL_H, PXL_V);
  }
  
  /* check if lookup array at y position is empty, and if yes, move down: */
  if (src_arr[i] == NULL) {
	if (debug & DBG_SRC_SPECIALS) {
	  Fprintf(stderr, "array at %d is NULL\n", y);
	}
	while (src_arr[i] == NULL && i < src_arr_len) {
	  i++;
	}

	if (i == src_arr_len) { /* reached end of array without finding special */
	  MessageBeep(MB_ICONINFORMATION);
	  fprintf(stdout, "No more specials on this page.\n");
	  return;
	}
	else {
	  if (debug & DBG_SRC_SPECIALS) {
		fprintf(stderr, "found at %d\n", i);
	  }
	}
  }
  else { /* lookup array contains elem at y */
	if (debug & DBG_SRC_SPECIALS) {
	  Fprintf(stderr, "Found array position at %d: %u, %u; %s:%d\n", y,
			src_arr[i]->x_coord, src_arr[i]->y_coord, src_arr[i]->filename, src_arr[i]->lineno);
	}
  }
  if (debug & DBG_SRC_SPECIALS) {
	fprintf(stderr, "horizontal list:\n");
	for (curr = src_arr[i]; curr != NULL; curr = curr->next_special) {
	  fprintf(stderr, "%s:%u (%d,%d)\n", curr->filename, curr->lineno, curr->x_coord, curr->y_coord);
	}
  }
  if((curr = src_arr[i]) != NULL) {
	/* compute the y coord of the previous line: */
	last_line_y = i - (Position)(curr->pt_size / 72.27 * (double)yRESOLUTION);
	
	/* move to right in the horizontal list if y point is on current line: */
	if (y > last_line_y) {
	  while (curr != NULL && x > curr->x_coord) {
		if (debug & DBG_SRC_SPECIALS) {
		  Fprintf(stderr, "x: %d <= %d\n", curr->x_coord, x);
		}
		curr = curr->next_special;
	  }
	  if (curr == NULL) {
		
		i++;
		while (src_arr[i] == NULL && i < src_arr_len) {
		  i++;
		}
		curr = src_arr[i];
	  }
	}
	if (curr != NULL) { /* might be at end of page; do nothing in this case */
	  
	  last_active_special = curr;
	  
	  /* if src specials are visible, draw this special with foreground color: */
	  if (src_tickVisibility) {
		draw_src_glyph(curr->x_coord, curr->y_coord, foreGC);
	  }
	  Fprintf(stderr, "SRC special: %s:%d (x:%d,y:%d)\n", curr->filename, curr->lineno, curr->x_coord, curr->y_coord);
	  /*
	   * if `jump' argument is set to one, issue the editor command.
	   */
	  if (jump == 1) {
		issue_src_editor_cmd(curr->filename, curr->lineno);
	  }
	}
  }
}


/*************************************************************************
  free the memory used by src_arr
 *************************************************************************/
void
src_cleanup(void) {
  fprintf(stderr, "freeing memory...\n");
  src_delete_all_specials();
  free(src_arr);
  src_arr_len = 0;
  src_arr_not_initialized = True;
}

/*************************************************************************
  delete all elements from specials list. Called from check_dvi_file
  (dvi_init.c).
 *************************************************************************/
void
src_delete_all_specials(void)
{
  int j;
  for (j = 0; j < src_arr_len; j++) {
	src_arr[j] = delete_from_pages(src_arr[j], current_page);
  }
  last_active_special = NULL;
}

/*************************************************************************
 insert special into array at index y. Returns 0 for success,
 1 if y is larger than array size.
*************************************************************************/
static int
src_insert_into_array(y,special)
	 unsigned y;
	 struct SRC_special *special;
{
  if (y >= src_arr_len) {
	Fprintf(stderr, "\nShouldn't happen: y coordinate %d exceeds src_arr bounds (%d)\n", y, src_arr_len);
	Fprintf(stderr, "- skipping this special.\n");
	Fprintf(stderr, "As a workaround, you may try a smaller magnification factor;\n");
	Fprintf(stderr, "however please report this bug to the maintainer(s) of xdvi(k).\n");
	return 1;
  }
  src_arr[y] = insert_into_src_specials(src_arr[y], special);
  return 0;
}



/*************************************************************************
  Evaluate a src special; called during the page scan from
  `applicationDoSpecial' (special.c).
  Main tasks are:
  - allocate space for the SRC_special struct and its elements
  - call `delete_from_pages' to delete specials from non-current
    pages or if magnification factor has changed
  - call `insert_into_src_specials' to insert special into list of specials
  - call `draw_src_glyph' to draw the `tick' indicating the specials on the
    displayed page
 *************************************************************************/
void
src_eval_special(input_string, x, y)
     char	*input_string;
	 Position x, y;
{
  struct SRC_special *new_special;
  unsigned int j;
  
  if (!src_evalMode) {
	return;
  }
  
  
  if (specials_not_initialized) {
	fprintf(stderr, "initializing specials...\n");
	src_get_settings();
	specials_not_initialized = False;
  }
  
  if (src_arr_not_initialized) {
	src_arr_len = page_h;
	/* initialize save_shrinkfactor to sensible value: */
	save_shrinkfactor = mane.shrinkfactor;
	src_initialize_array(src_arr_len);
	src_arr_not_initialized = False;
  }

  
  if (debug & DBG_SRC_SPECIALS) {
    if (currinf.fontp == NULL) {
      Fprintf(stderr, "SRC special: %s (%d, %d); (NULL font)\n", input_string, x, y);
    }
    else {
	  Fprintf(stderr, "SRC special: %s (%d, %d), font:%s (%f)\n",
			  input_string, x, y, currinf.fontp->fontname, currinf.fontp->pt_size
			  );
    }
	Fprintf(stderr, "shrink factors: %d,%d,%d\n", alt.shrinkfactor, mane.shrinkfactor, currwin.shrinkfactor);
  }
  
  /*
   * flush specials if shrink factor has changed.
   * The test (mane.shrinkfactor != save_shrinkfactor) gave misbehaviour with the magnifier
   * which also sets the shrinkfactor; hence the additional check
   * MAGNIFIER_NOT_ACTIVE (kludgy, see definition above).
   */
  if (mane.shrinkfactor != save_shrinkfactor && MAGNIFIER_NOT_ACTIVE) {
    if (debug & DBG_SRC_SPECIALS) {
      Fprintf(stderr, "shrink factor changed, flushing specials ...\n");
    }
	
	/* delete all specials from src_arr */
	for (j = 0; j < src_arr_len; j++) {
	  src_arr[j] = delete_from_pages(src_arr[j], current_page);
	}
	
	/* recompute length of src_arr */
	src_arr_len = page_h; 
	src_resize_array(src_arr_len);
	/*
	 * set last_active_special to NULL since it might point to the wrong value
	 * after size has changed:
	 */
	last_active_special = NULL;
  }
  /*
   * only insert new specials if displaying specials is ON (ignoring
   * size changes by the magnifier glass - this would insert way too
   * many specials), if src special is well-formed and if its y coordinate
   * doesn't exceed the length of the src_arr.
   */
  if (
	  MAGNIFIER_NOT_ACTIVE && (src_arr_len > 0)) {
    if ((new_special = src_parse_special_string(input_string, x, y)) != NULL) {
	  if ((src_insert_into_array(y, new_special) == 0)  && src_tickVisibility) {
		draw_src_glyph(new_special->x_coord, new_special->y_coord, highGC);
	  }
	}
  }	
  save_shrinkfactor = mane.shrinkfactor;
}

/*************************************************************************
  Currently unused.																		  
  In case an external function would have to draw the src ticks, it might
  want to call this function.
 *************************************************************************/
void draw_src_specials()
{
  struct SRC_special *curr;
  for(curr = head_special; curr != NULL; curr = curr->next_special) {
    draw_src_glyph(curr->x_coord, curr->y_coord, highGC);
  }
}

#endif
