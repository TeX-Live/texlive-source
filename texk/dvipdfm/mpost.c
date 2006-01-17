/*  $Header: SCCS/s.mpost.c 1.1 00/08/05 01:52:14-00:00 mwicks@gaspra $
    
    This is dvipdfm, a DVI to PDF translator.
    Copyright (C) 1998, 1999 by Mark A. Wicks
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    
    The author may be contacted via the e-mail address
    
    mwicks@kettering.edu
*/

#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "system.h"
#include "mem.h"
#include "error.h"
#include "mfileio.h"
#include "dvi.h"
#include "pdfobj.h"
#include "pdfspecial.h"
#include "pdfparse.h"
#include "mpost.h"
#include "pdfparse.h"
#include "pdflimits.h"
#include "pdfdev.h"
#include "pdfdoc.h"

int check_for_mp (FILE *image_file) 
{
  rewind (image_file);
  /* For now, this is an exact test that must be passed, character for
     character */
  mfgets (work_buffer, WORK_BUFFER_SIZE, image_file);
  if (strncmp (work_buffer, "%!PS", 4))
    return 0;
  mfgets (work_buffer, WORK_BUFFER_SIZE, image_file);
  if (strncmp (work_buffer, "%%BoundingBox", strlen("%%BoundingBox")))
    return 0;
  mfgets (work_buffer, WORK_BUFFER_SIZE, image_file);
  if (strncmp (work_buffer, "%%Creator: MetaPost", strlen("%%Creator: MetaPost")))
    return 0;
  return 1;
}

static struct mp_fonts 
{
  char *tex_name;
  int font_id;
  double pt_size;
} *mp_fonts = NULL;

int n_mp_fonts = 0, max_mp_fonts = 0;

static void need_more_mp_fonts (unsigned n)
{
  if (n_mp_fonts + n > max_mp_fonts) {
    max_mp_fonts += MAX_FONTS;
    mp_fonts = RENEW (mp_fonts, max_mp_fonts, struct mp_fonts);
  }
}



int mp_locate_font (char *tex_name, double pt_size)
{
  int result=-1, i;
  for (i=0; i<n_mp_fonts; i++) {
    if (!strcmp (tex_name, mp_fonts[i].tex_name) &&
	mp_fonts[i].pt_size == pt_size)
      break;
  }
  need_more_mp_fonts (1);
  if (i == n_mp_fonts) {
    n_mp_fonts += 1;
    mp_fonts[i].tex_name = NEW (strlen(tex_name)+1, char);
    strcpy (mp_fonts[i].tex_name, tex_name);
    mp_fonts[i].pt_size = pt_size;
    /* The following line is a bit of a kludge.  MetaPost inclusion
       was an afterthought */
    mp_fonts[i].font_id = dev_locate_font (tex_name,
					   pt_size/dvi_unit_size());
    result = mp_fonts[i].font_id;
  } else 
    result = -1;
  return result;
}

static void release_fonts (void)
{
  int i;
  for (i=0; i<n_mp_fonts; i++) {
    RELEASE (mp_fonts[i].tex_name);
  }
  if (mp_fonts) {
    RELEASE (mp_fonts);
    mp_fonts = NULL;
  }
  n_mp_fonts = 0;
  max_mp_fonts = 0;
}

int mp_is_font_name (char *tex_name)
{
  int i;
  for (i=0; i<n_mp_fonts; i++) {
    if (!strcmp (tex_name, mp_fonts[i].tex_name))
      break;
  }
  if (i < n_mp_fonts)
    return 1;
  else
    return 0;
}

int mp_fontid (char *tex_name, double pt_size)
{
  int i;
  for (i=0; i<n_mp_fonts; i++) {
    if (!strcmp (tex_name, mp_fonts[i].tex_name) &&
	(mp_fonts[i].pt_size == pt_size))
      break;
  }
  if (i < n_mp_fonts) {
    return i;
  }
  else {
    return -1;
  }
}

static int mp_parse_headers (FILE *image_file, struct xform_info *p)
{
  int error = 0;
  char *start, *end, *token, *name;
  char *llx = NULL, *lly = NULL, *urx = NULL, *ury = NULL;
  unsigned long save_position;
#ifdef MEM_DEBUG
  MEM_START
#endif
 /* Scan for bounding box record */
  for (;;) {
    mfgets (work_buffer, WORK_BUFFER_SIZE, image_file);
    if (work_buffer[0] != '%' ||
	!strncmp (work_buffer, "%%BoundingBox:",
		 strlen("%%BoundingBox")))
      break;
  }
  if (work_buffer[0] == '%') { /* Found %%BoundingBox */
    start = work_buffer + strlen("%%BoundingBox:");
    end = start+strlen(start);
    skip_white (&start, end);
    /* Expect 4 numbers or fail */
    if ((llx = parse_number (&start, end)) &&
	(lly = parse_number (&start, end)) &&
	(urx = parse_number (&start, end)) &&
	(ury = parse_number (&start, end))) {
      /* Set the crop box to the true bounding box specified in the
	 file */
      p->c_llx = atof (llx);
      p->c_lly = atof (lly);
      p->c_urx = atof (urx);
      p->c_ury = atof (ury);
    } else{
      fprintf (stderr, "\nMissing expected number in bounding box specification:\n");
      dump (start, end);
      error = 1;
    }
  } else {	/* Didn't find Bounding box */
    fprintf (stderr, "\nFailed to find an expected BoundingBox record.\n");
    error = 1;
  }
  if (llx) RELEASE(llx);
  if (lly) RELEASE(lly);
  if (urx) RELEASE(urx);
  if (ury) RELEASE(ury);
  /* Got four numbers */
  /* Read all headers--act on *Font records */
  save_position = tell_position(image_file);
  while (!error && !feof(image_file) && mfgets (work_buffer, WORK_BUFFER_SIZE,
				      image_file)) {
    if (*work_buffer != '%') {
      seek_absolute (image_file, save_position);
      break;
    }
    save_position = tell_position (image_file);
    if (*(work_buffer+1) == '*' &&
	!strncmp (work_buffer+2, "Font:", strlen("Font:"))) {
      double ps_ptsize;
      start = work_buffer+strlen("%*Font:");
      end = start+strlen(start);
      skip_white (&start, end);
      if ((name = parse_ident (&start, end))) {
	skip_white (&start, end);
      } else {
	fprintf (stderr, "\nMissing expected font name:\n");
	dump (start, end);
	error = 1;
	break;
      }
      if ((token = parse_number (&start, end))) {
	ps_ptsize = atof (token);
	RELEASE (token);
      } else {
	fprintf (stderr, "\nMissing expected font point size specification:\n");
	dump (start, end);
	error = 1;
	break;
      }
      mp_locate_font (name, ps_ptsize);
      RELEASE (name);
    }
  }
#ifdef MEM_DEBUG
  MEM_END
#endif
    return !error;
}

struct point {
  double x, y;
};

struct curve {
  double x[3], y[3];
};

static struct path_element
{
  char type;
  void  *element;
} *path = NULL;
static int n_path_pts = 0, max_path_pts = 0;
static int path_clip = 0, path_close = 0;

#define MORE_PATH_POINTS 64

static void need_points (int n)
{
  if (n_path_pts+n > max_path_pts) {
    max_path_pts += MORE_PATH_POINTS;
    path = RENEW (path, max_path_pts, struct path_element);
  }
  return;
}

static void clip_path (void)
{
  path_clip = 1;
}

static void close_path (void)
{
  path_close = 1;
}

static void add_point_to_path (double x, double y, char type)
{
  struct point *p;
  need_points (1);
  path[n_path_pts].type = type;
  p = NEW (1, struct point);
  p->x = x; p->y = y;
  path[n_path_pts].element = p;
  n_path_pts += 1;
  return;
}

static void add_curve_to_path (double x0, double y0,
			       double x1, double y1,
			       double x2, double y2)
{
  struct curve *c;
  need_points (1);
  c = NEW (1, struct curve);
  path[n_path_pts].type = 'c';
  (c->x)[0] = x0; (c->y)[0] = y0;
  (c->x)[1] = x1; (c->y)[1] = y1;
  (c->x)[2] = x2; (c->y)[2] = y2;
  path[n_path_pts].element = c;
  n_path_pts += 1;
  return;
}

static void void_path (void)
{
  int i;
  for (i=0; i<n_path_pts; i++) {
    RELEASE (path[i].element);
  }
  path_clip = 0;
  path_close = 0;
  n_path_pts = 0;
}

static void flush_path (void)
{
  int i, len;
  for (i=0; i<n_path_pts; i++) {
    switch (path[i].type) {
    case 'm': /* moveto */ 
      /* Moveto must start a new path */
      /*      void_path (); */
    case 'l': /* lineto */ 
      {
	struct point *p;
	p = path[i].element;
	sprintf (work_buffer, "\n%g %g %c",
		       ROUND(p->x, 0.001), ROUND(p->y, 0.001),
		       path[i].type);
        len = strlen (work_buffer);
	pdf_doc_add_to_page (work_buffer, len);
	RELEASE (p);
      }
      break;
    case 'c': /* curveto */
      {
	struct curve *p;
	p = path[i].element;
	sprintf (work_buffer, "\n%g %g %g %g %g %g c",
		       ROUND((p->x)[0], 0.001), ROUND((p->y)[0], 0.001),
		       ROUND((p->x)[1], 0.001), ROUND((p->y)[1], 0.001),
		       ROUND((p->x)[2], 0.001), ROUND((p->y)[2], 0.001));
        len = strlen (work_buffer);
	pdf_doc_add_to_page (work_buffer, len);
	RELEASE (p);
      }
      break;
    default: /* This shouldn't happen! */
      ERROR ("Internal error in mpost.c, flush_path ()");
    }
  }
  n_path_pts = 0;
  if (path_close) {
    pdf_doc_add_to_page (" h", 2);
    path_close = 0;
  }
  if (path_clip) {
    pdf_doc_add_to_page (" W", 2);
    path_clip = 0;
  }
  return;
}

static void transform_path (double a, double b, double c, double d,
			    double e, double f)
{
  double an, bn, cn, dn, en, fn, delta, xn, yn;
  unsigned i;
  delta = a*d-b*c;
  if (delta == 0.0) {
    ERROR ("Determinant exactly zero in transform_path()");
  }
  an=d/delta; bn=-b/delta; cn=-c/delta; dn=a/delta;
  en=(c*f-e*d)/delta, fn=(b*e-a*f)/delta;
  for (i=0; i<n_path_pts; i++) {
    switch (path[i].type) {
    case 'l':
    case 'm':
      {
	struct point *p;
	p = path[i].element;
	xn = an*(p->x)+cn*(p->y)+en;
	yn = bn*(p->x)+dn*(p->y)+fn;
	p->x = xn;
	p->y = yn;
      }
      break;
    case 'c':
      {
	int j;
	struct curve *c;
	c = path[i].element;
	for (j=0; j<3; j++) {
	  xn = an*(c->x)[j]+cn*(c->y)[j]+en;
	  yn = bn*(c->x)[j]+dn*(c->y)[j]+fn;
	  (c->x)[j] = xn; (c->y)[j] = yn;
	}
      }
    }
  }
}
static void dump_path (void)
{
  unsigned i;
  for (i=0; i<n_path_pts; i++) {
    switch (path[i].type) {
    case 'l':
    case 'm': 
      {
	struct point *p;
	p = path[i].element;
	fprintf (stderr, "\t%g %g %c\n", p->x, p->y, path[i].type);
      }
      break;
    case 'c': 
      {
	struct curve *c;
	int j;
	c = path[i].element;
	for (j=0; j<3; j++) {
	  fprintf (stderr, "\t%g %g\n", (c->x)[j], (c->y)[j]);
	}
	fprintf (stderr, "\t\t\tcurveto\n");
	break;
      }
    }
  }
}

#define PS_STACK_SIZE 1024
static pdf_obj *stack[PS_STACK_SIZE];
static unsigned top_stack;

double x_state, y_state;
static int state = 0;
static int num_saves = 0;

#define PUSH(o) { \
  if (top_stack<PS_STACK_SIZE) { \
    stack[top_stack++] = o; \
  } else { \
    fprintf (stderr, "PS stack overflow including MetaPost file or inline PS code"); \
    error=1; \
    break; \
  } \
  }

#define POP_STACK() (top_stack>0?stack[--top_stack]:NULL)

void dump_stack()
{
  int i;
  fprintf (stderr, "\ndump_stack\n");
  for (i=0; i<top_stack; i++) {
    pdf_write_obj (stderr, stack[i]);
    fprintf (stderr, "\n");
  }
}
#define ADD          	1
#define CLIP         	2
#define CLOSEPATH    	3
#define CONCAT       	4
#define CURVETO   	5
#define DIV		6
#define DTRANSFORM	7
#define EXCH		8
#define FILL		9
#define FSHOW		10
#define GSAVE		11
#define GRESTORE	12
#define IDTRANSFORM	13
#define LINETO		14
#define MOVETO		15
#define MUL		16
#define NEWPATH		17
#define POP		18
#define RLINETO		19
#define SCALE		20
#define SETCMYKCOLOR	21
#define SETDASH		22
#define SETGRAY		23
#define SETLINECAP	24
#define SETLINEJOIN	25
#define SETLINEWIDTH	26
#define SETMITERLIMIT	27
#define SETRGBCOLOR	28
#define SHOW		29
#define SHOWPAGE	30
#define STROKE		31
#define SUB		32
#define TRANSLATE	33
#define TRUNCATE	34
#define CURRENTPOINT    35
#define NEG    	        36
#define ROTATE          37
#define TEXFIG          38
#define ETEXFIG         39
#define FONTNAME	99

struct operators 
{
  char *t;
  int v;
} operators[] = {
  {"add", ADD},
  {"clip", CLIP},
  {"closepath", CLOSEPATH},
  {"concat", CONCAT},
  {"curveto", CURVETO},
  {"currentpoint", CURRENTPOINT},  /* This is here for rotate
				      support in graphics package-not MP support */
  {"div", DIV},
  {"dtransform", DTRANSFORM},
  {"exch", EXCH},
  {"fill", FILL},
  {"fshow", FSHOW},
  {"gsave", GSAVE},
  {"grestore", GRESTORE},
  {"idtransform", IDTRANSFORM},
  {"lineto", LINETO},
  {"moveto", MOVETO},
  {"mul", MUL},
  {"neg", NEG},
  {"newpath", NEWPATH},
  {"pop", POP},
  {"rlineto", RLINETO},
  {"rotate", ROTATE},
  {"scale", SCALE},
  {"setcmykcolor", SETCMYKCOLOR},
  {"setdash", SETDASH},
  {"setgray", SETGRAY},
  {"setlinecap", SETLINECAP},
  {"setlinejoin", SETLINEJOIN},
  {"setlinewidth", SETLINEWIDTH},
  {"setmiterlimit", SETMITERLIMIT},
  {"setrgbcolor", SETRGBCOLOR},
  {"show", SHOW},
  {"showpage", SHOWPAGE},
  {"startTexFig", TEXFIG},
  {"endTexFig", ETEXFIG},
  {"stroke", STROKE},  
  {"sub", SUB},  
  {"translate", TRANSLATE},
  {"truncate", TRUNCATE}
};

static int lookup_operator(char *token)
{
  int i, operator;
  operator = -1;
  for (i=0; i<sizeof(operators)/sizeof(operators[0]); i++) {
    if (!strcmp (token, operators[i].t)) {
      operator = operators[i].v;
      break;
    }
  }
  if (i == sizeof(operators)/sizeof(operators[0]) &&
      mp_is_font_name (token)) {
    operator = FONTNAME;
  }
  return operator;
}


/* Following needed to save texfig state */

static long next_fig = 1;
static pdf_obj *fig_xobj = NULL;
struct xform_info *fig_p;
static char fig_res_name[16];

static int do_operator(char *token,
		       double x_user, double y_user)
     /* Again, the only piece that needs x_user and y_user is
	that piece dealing with texfig */
{
  int operator, error = 0;
  pdf_obj *tmp1=NULL, *tmp2=NULL, *tmp3=NULL, *tmp4 = NULL;
  pdf_obj *tmp5=NULL, *tmp6=NULL;
  int len;
  /* PS to PDF conversion is not so simple.  We maintain
     state so we can change "gsave fill grestore stroke" to "B".
     We need to keep track of what we have seen.   This code is not
     too smart and could be easily fooled by real postscript. 
     It makes some assumptions since it is only looking at MetaPost

     States are as follows:
     0: Nothing special
     1: Started a path
     2: Saw gsave in path
     3: Saw a painting operator in state 2(fill, stroke, or newpath)
     4: Saw a grestore in state 3
  */
  operator = lookup_operator (token);
  switch (operator) {
  case ADD:
    tmp1 = POP_STACK();
    tmp2 = POP_STACK();
    if (tmp1 && tmp2)
      pdf_set_number (tmp1,
		      pdf_number_value(tmp1)+pdf_number_value(tmp2));
    if (tmp2)
      pdf_release_obj (tmp2);
    if (tmp1)
      PUSH(tmp1);
    break;
  case CLIP:
    clip_path();
    break;
  case CLOSEPATH:
    close_path();
    break;
  case CONCAT:
    tmp1 = POP_STACK();
    if (tmp1 && tmp1 -> type == PDF_ARRAY) {
      int i, len = 0;
      for (i=0; i<6; i++) {
	if (!(tmp2 = pdf_get_array(tmp1, i)))
	  break;
	if (i == 0) {
	   sprintf (work_buffer+len, "\n");
           len += strlen (work_buffer+len);
        } else  {
	   sprintf (work_buffer+len, " ");
           len += strlen (work_buffer+len);
        }
	sprintf (work_buffer+len, "%g",
		 ROUND(pdf_number_value(tmp2),0.0001));
        len += strlen (work_buffer+len);
      }
      sprintf (work_buffer+len, " cm");
      len += strlen (work_buffer+len);
      pdf_doc_add_to_page (work_buffer, len);
      /* Transform pending path, if any */
      transform_path (pdf_number_value(pdf_get_array(tmp1, 0)),
		      pdf_number_value(pdf_get_array(tmp1, 1)),
		      pdf_number_value(pdf_get_array(tmp1, 2)),
		      pdf_number_value(pdf_get_array(tmp1, 3)),
		      pdf_number_value(pdf_get_array(tmp1, 4)),
		      pdf_number_value(pdf_get_array(tmp1, 5)));
    } else {
      fprintf (stderr, "\nMissing array before \"concat\"\n");
    }
    if (tmp1)
      pdf_release_obj (tmp1);
    break;
  case CURRENTPOINT:
    state = 0;
    PUSH (pdf_new_number (x_user)); /* Remember that x_user and y_user */
    PUSH (pdf_new_number (y_user)); /* are off by 0.02 % */
    break;
  case CURVETO:
    if (state <= 1) /* In path now */
      state = 1;
    else {
      fprintf (stderr, "\nUnexpected path segment\n");
    }
    if ((tmp6 = POP_STACK()) && tmp6->type == PDF_NUMBER &&
	(tmp5 = POP_STACK()) && tmp5->type == PDF_NUMBER &&
	(tmp4 = POP_STACK()) && tmp4->type == PDF_NUMBER &&
	(tmp3 = POP_STACK()) && tmp3->type == PDF_NUMBER &&
	(tmp2 = POP_STACK()) && tmp2->type == PDF_NUMBER &&
	(tmp1 = POP_STACK()) && tmp1->type == PDF_NUMBER) {
      add_curve_to_path (pdf_number_value(tmp1),
			 pdf_number_value(tmp2),
			 pdf_number_value(tmp3),
			 pdf_number_value(tmp4),
			 pdf_number_value(tmp5),
			 pdf_number_value(tmp6));
      x_state = pdf_number_value (tmp5);
      y_state = pdf_number_value (tmp6);
    } else {
      fprintf (stderr, "\nMissing number(s) before \"curveto\"\n");
    }
    if (tmp1) pdf_release_obj (tmp1);
    if (tmp2) pdf_release_obj (tmp2);
    if (tmp3) pdf_release_obj (tmp3);
    if (tmp4) pdf_release_obj (tmp4);
    if (tmp5) pdf_release_obj (tmp5);
    if (tmp6) pdf_release_obj (tmp6);
    break;
  case DIV:
    tmp2 = POP_STACK();
    tmp1 = POP_STACK();
    if (tmp1 && tmp2)
      pdf_set_number (tmp1,
		      pdf_number_value(tmp1)/pdf_number_value(tmp2));
    if (tmp1)
      PUSH(tmp1);
    if (tmp2)
      pdf_release_obj (tmp2);
    break;
  case DTRANSFORM:
    if ((tmp2 = POP_STACK()) && tmp2 -> type == PDF_NUMBER &&
	(tmp1 = POP_STACK()) && tmp1 -> type == PDF_NUMBER) {
      pdf_set_number (tmp1, pdf_number_value(tmp1)*100.0);
      pdf_set_number (tmp2, pdf_number_value(tmp2)*100.0);
      PUSH(tmp1);
      PUSH(tmp2);
    } else {
      if (tmp1)
	pdf_release_obj (tmp1);
      if (tmp2)
	pdf_release_obj (tmp2);
      fprintf (stderr, "\nExpecting two numbers before \"dtransform\"");
    }
    break;
  case EXCH:
    if ((tmp1 = POP_STACK()) &&
	(tmp2 = POP_STACK())) {
      PUSH (tmp1);
      PUSH (tmp2);
    } else {
      if (tmp1)
	pdf_release_obj (tmp1);
    }
    break;
  case FILL:
    switch (state) {
    case 0:
      state = 0;
      break;
    case 1:
      flush_path ();
      pdf_doc_add_to_page (" f", 2);
      state = 0;
      break;
    case 2:
      state = 3;
      break;
    case 3:
      fprintf (stderr, "\nUnexpected fill\n");
      break;
    case 4:
      flush_path ();
      pdf_doc_add_to_page (" B", 2);
      state = 0;
      break;
    }
    break;
  case FSHOW: 
    {
      int fontid;
      if ((tmp3 = POP_STACK()) && (tmp3->type == PDF_NUMBER) &&
	  (tmp2 = POP_STACK()) && (tmp2->type == PDF_STRING) &&
	  (tmp1 = POP_STACK()) && (tmp1->type == PDF_STRING)) {
	if ((fontid = mp_fontid (pdf_string_value(tmp2),
				 pdf_number_value(tmp3))) < 0) {
	  fprintf (stderr, "\n\"fshow\": Missing font in MetaPost file? %s@%g\n", 
		   (char *) pdf_string_value(tmp2), pdf_number_value(tmp3));
	}
	dev_set_string (x_state/dvi_unit_size(), y_state/dvi_unit_size(),
			pdf_string_value(tmp1),
			pdf_string_length(tmp1), 0, mp_fonts[fontid].font_id);
	graphics_mode();
      }
      /* Treat fshow as a path terminator of sorts */
      state = 0;

      if (tmp1)
	pdf_release_obj (tmp1);
      if (tmp2)
	pdf_release_obj (tmp2);
      if (tmp3)
	pdf_release_obj (tmp3);
    }
    break;
  case GSAVE:
    switch (state) {
    case 0:
      pdf_doc_add_to_page ("\nq", 2);
      num_saves += 1;
      break;
    case 1:
      state = 2;
      break;
    case 4:
      state = 2;
      break;
    default:
      fprintf (stderr, "\nUnexpected gsave\n");
      break;
    }
    break;
  case GRESTORE:
    switch (state) {
    case 0:
      if (num_saves > 0) {
	num_saves -= 1;
	pdf_doc_add_to_page ("\nQ", 2);
	/* Unfortunately, the following two lines are necessary in case of a font or color
	   change inside of the save/restore pair.  Anything that was done
	   there must be redone, so in effect, we make no assumptions about
	   what fonts are active.  We act like we are starting a new page */
	dev_reselect_font();
	/* The following line was causing trouble - 1/30/01 */
	/*	dev_do_color(); */
      }
      else {
	fprintf (stderr, "PS special: \"grestore\" ignored.  More restores than saves on a page.\n");
      }
      break;
    case 2:
      state = 1;
      break;
    case 3:
      state = 4;
      break;
    default:
      fprintf (stderr, "\nUnexpected grestore\n");
      break;
    }
    break;
  case IDTRANSFORM:
    if ((tmp2 = POP_STACK()) && tmp2 -> type == PDF_NUMBER &&
	(tmp1 = POP_STACK()) && tmp1 -> type == PDF_NUMBER) {
      pdf_set_number (tmp1, pdf_number_value(tmp1)/100.0);
      pdf_set_number (tmp2, pdf_number_value(tmp2)/100.0);
      PUSH(tmp1);
      PUSH(tmp2);
    } else {
      if (tmp1)
	pdf_release_obj (tmp1);
      if (tmp2)
	pdf_release_obj (tmp2);
      fprintf (stderr, "\nExpecting two numbers before \"idtransform\"");
    }
    break;
  case LINETO: 
    {
      if (state <= 1) /* In path now */
	state = 1;
      else {
	fprintf (stderr, "\nUnexpected path segment\n");
      }
      if ((tmp2 = POP_STACK()) && tmp2-> type == PDF_NUMBER &&
	  (tmp1 = POP_STACK()) && tmp1-> type == PDF_NUMBER) {
	x_state = pdf_number_value (tmp1);
	y_state = pdf_number_value (tmp2);
	add_point_to_path (x_state, y_state, 'l');
      }
      if (tmp1)
	pdf_release_obj (tmp1);
      if (tmp2)
	pdf_release_obj (tmp2);
    }
    break;
  case MOVETO:
    if (state <= 1) /* In path now */
      state = 1;
    else {
      fprintf (stderr, "\nUnexpected path segment\n");
    }
    if ((tmp2 = POP_STACK()) && tmp2-> type == PDF_NUMBER &&
	(tmp1 = POP_STACK()) && tmp1-> type == PDF_NUMBER) {
      /* MetaPost likes to ship out a moveto before displayed text.
	 Save the results so we know where to place the text.  Save
	 the move but don't actually do it unless the next operator is
	 a graphics operator */
      x_state = pdf_number_value (tmp1);
      y_state = pdf_number_value (tmp2);
      add_point_to_path (x_state, y_state, 'm');
    }
    if (tmp1)
      pdf_release_obj (tmp1);
    if (tmp2)
      pdf_release_obj (tmp2);
    break;
  case MUL:
    tmp2 = POP_STACK();
    tmp1 = POP_STACK();
    if (tmp1 && tmp2)
      pdf_set_number (tmp1,
		      pdf_number_value(tmp1)*pdf_number_value(tmp2));
    if (tmp1)
      PUSH(tmp1);
    if (tmp2)
      pdf_release_obj (tmp2);
    break;
  case NEG:
    tmp1 = POP_STACK();
    if (tmp1 && tmp1 -> type == PDF_NUMBER) {
      pdf_set_number (tmp1, -pdf_number_value(tmp1));
      PUSH (tmp1);
    }
    break;
  case NEWPATH:
    flush_path ();
    pdf_doc_add_to_page ("\nn", 2);
    break;
  case POP:
    tmp1 = POP_STACK();
    if (tmp1)
      pdf_release_obj (tmp1);
    break;
  case RLINETO: 
    if (state <= 1) /* In path now */
      state = 1;
    else {
      fprintf (stderr, "\nUnexpected path segment\n");
    }
    if ((tmp2 = POP_STACK()) && tmp2->type == PDF_NUMBER &&
	(tmp1 = POP_STACK()) && tmp1->type == PDF_NUMBER) {
      x_state += pdf_number_value (tmp1);
      y_state += pdf_number_value (tmp2);
      add_point_to_path (x_state, y_state, 'l');
    }
    if (tmp1)
      pdf_release_obj (tmp1);
    if (tmp2)
      pdf_release_obj (tmp2);
    break;
  case SCALE: 
    if ((tmp2 = POP_STACK()) &&  tmp2->type == PDF_NUMBER &&
	(tmp1 = POP_STACK()) && tmp1->type == PDF_NUMBER) {
      sprintf (work_buffer, "\n%g 0 0 %g 0 0 cm",
	       ROUND(pdf_number_value (tmp1),0.01),
	       ROUND(pdf_number_value (tmp2),0.01));
      len = strlen (work_buffer);
      transform_path (pdf_number_value(tmp1), 0.0,
		      0.0, pdf_number_value(tmp2), 0.0, 0.0);
      pdf_doc_add_to_page (work_buffer, len);
    }
    if (tmp1)
      pdf_release_obj (tmp1);
    if (tmp2)
      pdf_release_obj (tmp2);
    break;
  case SETCMYKCOLOR:
    if ((tmp4 = POP_STACK()) && tmp4->type == PDF_NUMBER &&
	(tmp3 = POP_STACK()) && tmp3->type == PDF_NUMBER &&
	(tmp2 = POP_STACK()) && tmp2->type == PDF_NUMBER &&
	(tmp1 = POP_STACK()) && tmp1->type == PDF_NUMBER) {
      sprintf (work_buffer, "\n%g %g %g %g k",
	       ROUND(pdf_number_value (tmp1),0.001),
	       ROUND(pdf_number_value (tmp2),0.001),
	       ROUND(pdf_number_value (tmp3),0.001),
	       ROUND(pdf_number_value (tmp4),0.001));
      len = strlen (work_buffer);
      pdf_doc_add_to_page (work_buffer, len);
      sprintf (work_buffer, "\n%g %g %g %g K",
	        ROUND(pdf_number_value (tmp1),0.001),
	        ROUND(pdf_number_value (tmp2),0.001),
	        ROUND(pdf_number_value (tmp3),0.001),
	        ROUND(pdf_number_value (tmp4),0.001));
      len = strlen (work_buffer);
      pdf_doc_add_to_page (work_buffer, len);
    } else {
      fprintf (stderr, "\nExpecting four numbers before \"setcmykcolor\"\n");
    }
    if (tmp1)
      pdf_release_obj (tmp1);
    if (tmp2)
      pdf_release_obj (tmp2);
    if (tmp3)
      pdf_release_obj (tmp3);
    if (tmp4)
      pdf_release_obj (tmp4);
    break;
  case SETDASH:
    if ((tmp2 = POP_STACK()) && tmp2->type == PDF_NUMBER &&
	(tmp1 = POP_STACK()) && tmp1->type == PDF_ARRAY) {
      int i;
      pdf_doc_add_to_page ("\n[", 2);
      for (i=0;; i++) {
	if ((tmp3 = pdf_get_array (tmp1, i)) &&
	    tmp3 -> type == PDF_NUMBER) {
	  sprintf (work_buffer, " %g", ROUND(pdf_number_value(tmp3),0.01));
          len = strlen (work_buffer);
	  pdf_doc_add_to_page (work_buffer, len);
	} else 
	  break;
      }
      pdf_doc_add_to_page (" ]", 2);
      if (tmp2 -> type == PDF_NUMBER) {
	sprintf (work_buffer, " %g d", ROUND(pdf_number_value(tmp2),0.01));
        len = strlen (work_buffer);
	pdf_doc_add_to_page (work_buffer, len);
      }
    } else {
      fprintf (stderr, "\nExpecting array and number before \"setdash\"");
    }
    if (tmp1)
      pdf_release_obj (tmp1);
    if (tmp2)
      pdf_release_obj (tmp2);
    break;
  case SETGRAY:
    if ((tmp1 = POP_STACK()) && tmp1->type == PDF_NUMBER) {
      sprintf (work_buffer, "\n%g g",
	       ROUND(pdf_number_value (tmp1),0.001));
      len = strlen (work_buffer);
      pdf_doc_add_to_page (work_buffer, len);
      sprintf (work_buffer, " %g G",
	       ROUND(pdf_number_value (tmp1),0.001));
      len = strlen (work_buffer);
      pdf_doc_add_to_page (work_buffer, len);
    } else {
      fprintf (stderr, "\nExpecting a number before \"setgray\"\n");
    }
    if (tmp1)
      pdf_release_obj (tmp1);
    break;
  case SETLINECAP:
    if ((tmp1 = POP_STACK()) && tmp1->type == PDF_NUMBER) {
      sprintf (work_buffer, "\n%g J", pdf_number_value (tmp1));
      len = strlen (work_buffer);
      pdf_doc_add_to_page (work_buffer, len);
    } else {
      fprintf (stderr, "\nExpecting a number before \"setlinecap\"\n");
    }
    if (tmp1)
      pdf_release_obj (tmp1);
    break;
  case SETLINEJOIN:
    if ((tmp1 = POP_STACK()) && tmp1->type == PDF_NUMBER) {
      sprintf (work_buffer, "\n%g j", pdf_number_value (tmp1));
      len = strlen (work_buffer);
      pdf_doc_add_to_page (work_buffer, len);
    } else {
      fprintf (stderr, "\nExpecting a number before \"setlinejoin\"\n");
    }
    if (tmp1)
      pdf_release_obj (tmp1);
    break;
  case SETLINEWIDTH:
    if ((tmp1 = POP_STACK()) && tmp1->type == PDF_NUMBER) {
      sprintf (work_buffer, "\n%g w", ROUND(pdf_number_value (tmp1),0.01));
      len = strlen (work_buffer);
      pdf_doc_add_to_page (work_buffer, len);
    } else {
      fprintf (stderr, "\nExpecting a number before \"setlinewidth\"\n");
    }
    if (tmp1)
      pdf_release_obj (tmp1);
    break;
  case SETMITERLIMIT:
    if ((tmp1 = POP_STACK()) && tmp1->type == PDF_NUMBER) {
      sprintf (work_buffer, "\n%g M", ROUND(pdf_number_value
					    (tmp1),0.01));
      len = strlen (work_buffer);
      pdf_doc_add_to_page (work_buffer, len);
    } else {
      fprintf (stderr, "\nExpecting a number before \"setmiterlimit\"\n");
    }
    if (tmp1)
      pdf_release_obj (tmp1);
    break;
  case SETRGBCOLOR:
    if ((tmp3 = POP_STACK()) && tmp3->type == PDF_NUMBER &&
	(tmp2 = POP_STACK()) && tmp2->type == PDF_NUMBER &&
	(tmp1 = POP_STACK()) && tmp1->type == PDF_NUMBER) {
      sprintf (work_buffer, "\n%g %g %g rg",
	       ROUND(pdf_number_value (tmp1),0.001),
	       ROUND(pdf_number_value (tmp2),0.001),
	       ROUND(pdf_number_value (tmp3),0.001));
      len = strlen (work_buffer);
      pdf_doc_add_to_page (work_buffer, len);
      sprintf (work_buffer, "\n%g %g %g RG",
	       ROUND(pdf_number_value (tmp1),0.001),
	       ROUND(pdf_number_value (tmp2),0.001),
	       ROUND(pdf_number_value (tmp3),0.001));
      len = strlen (work_buffer);
      pdf_doc_add_to_page (work_buffer, len);
    } else {
      fprintf (stderr, "\nExpecting three numbers before \"setrgbcolor\"\n");
    }
    if (tmp1)
      pdf_release_obj (tmp1);
    if (tmp2)
      pdf_release_obj (tmp2);
    if (tmp3)
      pdf_release_obj (tmp3);
    break;
  case SHOWPAGE:
    /* Let's ignore this for now */
    void_path ();
    break;
  case STROKE:
    switch (state) {
    case 0:
      state = 0;
      break;
    case 1:
      flush_path ();
      pdf_doc_add_to_page (" S", 2);
      state = 0;
      break;
    case 2:
      state = 3;
      break;
    case 3:
      fprintf (stderr, "\nUnexpected fill\n");
      break;
    case 4:
      flush_path ();
      pdf_doc_add_to_page (" B", 2);
      state = 0;
      break;
    }
    break;
  case SUB:
    tmp2 = POP_STACK();
    tmp1 = POP_STACK();
    if (tmp1 && tmp2)
      pdf_set_number (tmp1,
		      pdf_number_value(tmp1)-pdf_number_value(tmp2));
    if (tmp1)
      PUSH(tmp1);
    if (tmp2)
      pdf_release_obj (tmp2);
    break;
  case ROTATE:
    if ((tmp1 = POP_STACK()) && (tmp1 -> type == PDF_NUMBER)) {
      double theta = pdf_number_value(tmp1)*M_PI/180.0;
      sprintf (work_buffer, "\n%.4f %.4f %.4f %.4f 0 0 cm",
	       cos(theta), -sin(theta),
	       sin(theta), cos(theta));
      len = strlen (work_buffer);
      pdf_doc_add_to_page (work_buffer, len);
    }
    if (tmp1)
      pdf_release_obj (tmp1);
    break;
  case TEXFIG:
    if ((tmp6 = POP_STACK()) && (tmp6 -> type == PDF_NUMBER) &&
	(tmp5 = POP_STACK()) && (tmp5 -> type == PDF_NUMBER) &&
	(tmp4 = POP_STACK()) && (tmp4 -> type == PDF_NUMBER) &&
	(tmp3 = POP_STACK()) && (tmp3 -> type == PDF_NUMBER) &&
	(tmp2 = POP_STACK()) && (tmp2 -> type == PDF_NUMBER) &&
	(tmp1 = POP_STACK()) && (tmp1 -> type == PDF_NUMBER)) {
      double dvi2pts = dvi_unit_size();
      fig_p = new_xform_info ();
      fig_p -> width = pdf_number_value(tmp1)*dvi2pts;
      fig_p -> height = pdf_number_value(tmp2)*dvi2pts;
      fig_p -> c_llx = pdf_number_value(tmp3)*dvi2pts;
      fig_p -> c_lly = pdf_number_value(tmp4)*dvi2pts;
      fig_p -> c_urx = pdf_number_value(tmp5)*dvi2pts;
      fig_p -> c_ury = pdf_number_value(tmp6)*dvi2pts;
      pdf_scale_image (fig_p);
      sprintf (fig_res_name, "Tf%ld", next_fig);
      fig_xobj = begin_form_xobj (fig_p->c_llx, -fig_p->c_ury, fig_p->c_llx, -fig_p->c_lly, 
				  fig_p->c_urx, -fig_p->c_ury, fig_res_name);
    }
    if (tmp1) pdf_release_obj(tmp1);
    if (tmp2) pdf_release_obj(tmp2);
    if (tmp3) pdf_release_obj(tmp3);
    if (tmp4) pdf_release_obj(tmp4);
    if (tmp5) pdf_release_obj(tmp5);
    if (tmp6) pdf_release_obj(tmp6);
    break;
  case ETEXFIG:
    if (fig_xobj) {
      next_fig += 1;
      end_form_xobj ();
      pdf_doc_add_to_page_xobjects (fig_res_name, pdf_ref_obj(fig_xobj));
      pdf_release_obj (fig_xobj);
      fig_xobj = NULL;
      pdf_doc_add_to_page ("\nq", 2);
      add_xform_matrix (x_user, y_user, fig_p->xscale, -fig_p->yscale, fig_p->rotate);
      if (fig_p->depth != 0.0)
	add_xform_matrix (0.0, -fig_p->depth, 1.0, -1.0, 0.0);
      release_xform_info (fig_p);
      sprintf (work_buffer, " /%s Do Q", fig_res_name);
      len = strlen (work_buffer);
      pdf_doc_add_to_page (work_buffer, len);
    } else {
      fprintf (stderr, "\nendTexFig without valid startTexFig ignored\n");
    }
    break;
  case TRANSLATE:
    if ((tmp2 = POP_STACK()) &&  tmp2->type == PDF_NUMBER &&
	(tmp1 = POP_STACK()) && tmp1->type == PDF_NUMBER) {
      sprintf (work_buffer, "\n1 0 0 1 %g %g cm", 
	       ROUND(pdf_number_value (tmp1),0.01),
	       ROUND(pdf_number_value (tmp2),0.01));
      len = strlen (work_buffer);
      pdf_doc_add_to_page (work_buffer, len);
      transform_path (1.0, 0.0, 0.0, 1.0,
		      pdf_number_value (tmp1),
		      pdf_number_value (tmp2));
    }
    if (tmp1)
      pdf_release_obj (tmp1);
    if (tmp2)
      pdf_release_obj (tmp2);
    break;
  case TRUNCATE:
    if ((tmp1 = POP_STACK()) && tmp1->type == PDF_NUMBER) {
      double val=pdf_number_value(tmp1);
      pdf_set_number (tmp1, val>0? floor(val): ceil(val));
      PUSH (tmp1);
    } else if (tmp1)
      pdf_release_obj (tmp1);
    break;
  case FONTNAME:
    PUSH (pdf_new_string (token, strlen(token)));
    break;
  default: 
    fprintf (stderr,
	     "\nIgnoring remaining special text following unknown PS operator: \"%s\"\n", token);
    error = 1;
    break;
  }
  return !error;
}

static int do_one_ps_line (char **start, char *end,
			   double x_user, double y_user)
 /* the only sections that need to know x_user and y _user are those
    dealing with texfig */
{
  char *token, *save = NULL;
  pdf_obj *obj;
  int error = 0;
  skip_white (start, end);
  while (*start < end && !error) {
    save = *start;
    if (isdigit (**start) || **start == '-' || **start == '.' ) {
      token = parse_number (start, end);
      PUSH (pdf_new_number(atof(token)));
      RELEASE (token);
    } else if (**start == '[' && /* This code assumes that arrays are contained on one line */
	       (obj = parse_pdf_array (start, end))) {
      PUSH (obj);
    } else if (**start == '(' &&
	       (obj = parse_pdf_string (start, end))) {
      PUSH (obj);
    } else if (**start == '/') {
      fprintf (stderr, "\nUnable to handle names in raw PS code.");
      dump (*start, end);
      error = 1;
    } else {
      token = parse_ident (start, end);
      if (!token || !do_operator (token, x_user, y_user)) {
	error = 1;
      }
      if (token)
	RELEASE (token);
    }
    skip_white (start, end);
  }
  if (*start < end) {
    fprintf (stderr, "\nRemainder of line unparsed.");
    dump (*start, end);
  }
  return !error;
}

static char line_buffer[1024];
int parse_contents (FILE *image_file)
{
  int error = 0;
  top_stack = 0;
  x_state = 0.0;
  y_state = 0.0;
  state = 0;
  while (!feof(image_file) && mfgets (line_buffer, sizeof(line_buffer),
				      image_file)) {
    char *start, *end;
    start = line_buffer;
    end = start+strlen(line_buffer);
    if (!do_one_ps_line (&start, end, 0.0, 0.0)) {
      error = 1;
      break;
    }
  }
  return !error;
}

int do_raw_ps_special (char **start, char* end, int cleanup,
		       double x_user, double y_user)
{
  int error = 0;
  state = 0;
  do_one_ps_line (start, end, x_user, y_user);
  if (cleanup)
    mp_cleanup(1);
  return !error;
}

void mp_cleanup (int sloppy_ok)
{
  release_fonts();
  if (state != 0) {
    if (!sloppy_ok)
      fprintf (stderr, "mp_cleanup(): State not zero\n");
    state = 0;
  }
  if (top_stack != 0) {
    if (!sloppy_ok)
      fprintf (stderr, "\nPS/MetaPost: PS stack not empty at end of figure!\n");
  }
  /* Cleanup paths */
  while (top_stack > 0) {
    pdf_obj *p;
    if ((p=POP_STACK()))
      pdf_release_obj (p);
  }
  if (n_path_pts > 0) {
    if (!sloppy_ok) {
      fprintf (stderr, "\nPS/MetaPost: Pending path at end of figure!\n");
      dump_path (); 
    }
    void_path ();
  }
  if (max_path_pts > 0) {
    RELEASE (path);
    path = NULL;
    max_path_pts = 0;
  }
}

void mp_eop_cleanup(void)
{
  mp_cleanup(1);
  num_saves = 0;
}


/* mp inclusion is a bit of a hack.  The routine
 * starts a form at the lower left corner of
 * the page and then calls begin_form_xobj telling
 * it to record the image drawn there and bundle it
 * up in an xojbect.  This allows us to use the coordinates
 * in the MP file directly.  This appears to be the
 * easiest way to be able to use the dev_set_string()
 * command (with its scaled and extended fonts) without
 * getting all confused about the coordinate system.
 * After the xobject is created, the whole thing can
 * be scaled any way the user wants */
 
pdf_obj *mp_include (FILE *image_file,  struct xform_info *p,
		     char *res_name, double x_user, double y_user)
{
   pdf_obj *xobj = NULL;
   rewind (image_file);
   if (mp_parse_headers (image_file, p)) {
      /* Looks like an MP file.  Setup xobj "capture" */
     pdf_scale_image (p);
     xobj = begin_form_xobj (p->u_llx,p->u_lly, p->c_llx, p->c_lly,
			     p->c_urx, p->c_ury, res_name);
     if (!xobj)
       return NULL;
     /* Flesh out the contents */
     if (!parse_contents (image_file)) {
       fprintf (stderr, "Errors occured while interpreting MetaPost file.\n\n");
     }
     /* Finish off the form */
     end_form_xobj();
   } else {
     fprintf (stderr, "\nErrors occured while scanning MetaPost file headers.\n");
   }
   mp_cleanup(0);
   return xobj;
}

/* struct xform_info *texfig_info (void)
{
  struct xform_info *p;
  p = new_xform_info ();
  p -> user_bbox = 1;
  p -> width = fig_width;
  p -> height = fig_height;
  p -> u_llx = fig_llx;
  p -> u_lly = fig_lly;
  p -> u_urx = fig_urx;
  p -> u_ury = fig_ury;
  return p;
} */
