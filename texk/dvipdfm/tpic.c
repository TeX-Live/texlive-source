/*  $Header$

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

#include <string.h>
#include <stdlib.h>
#include "system.h"
#include "mem.h"
#include "mfileio.h"
#include "tpic.h"
#include "pdfparse.h"
#include "pdfdoc.h"
#include "pdfdev.h"
#include "dvi.h"

/* Following "constant" converts milli-inches to
   device (in this case PDF stream) coordinates */

#define MI2DEV (0.072/pdf_dev_scale())

double pen_size = 1.0;
int fill_shape = 0;
double fill_color = 0.0; double default_fill_color = 0.5;
static struct path
{
  double x, y;
} *path = NULL;
unsigned long path_length = 0, max_path_length = 0;

static void tpic_clear_state (void) 
{
  if ((path))
    RELEASE(path);
  path = NULL;
  path_length = 0;
  max_path_length = 0;
  fill_shape = 0;
  fill_color = 0.0;
  return;
}

static void set_pen_size (char **buffer, char *end)
{
  char *number;
  skip_white (buffer, end);
  if ((number = parse_number(buffer, end))) {
    pen_size = atof (number) * MI2DEV;
    RELEASE (number);
  } else {
    dump (*buffer, end);
    fprintf (stderr, "tpic special: pn: Invalid pen size\n");
  }
}

static void set_fill_color (char **buffer, char *end)
{
  char *number;
MEM_START
  fill_shape = 1;
  fill_color = default_fill_color; 
  skip_white (buffer, end);
  if ((number = parse_number(buffer, end))) {
    fill_color = 1.0 - atof (number);
    if (fill_color > 1.0)
      fill_color = 1.0;
    if (fill_color < 0.0)
      fill_color = 0.0;
    RELEASE (number);
  }
MEM_END
}

static void add_point (char **buffer, char *end) 
{
  char *x= NULL, *y= NULL;
MEM_START
  skip_white (buffer, end);
  if (*buffer < end)
    x = parse_number (buffer, end);
  skip_white (buffer, end);
  if (*buffer < end)
    y = parse_number (buffer, end);
  if ((x) && (y)) {
    if (path_length >= max_path_length) {
      max_path_length += 256;
      path = RENEW (path, max_path_length, struct path);
    }
    path[path_length].x = atof(x)*MI2DEV;
    path[path_length].y = atof(y)*MI2DEV;
    path_length += 1;
  } else {
    dump (*buffer, end);
    fprintf (stderr, "tpic special: pa: Missing coordinate\n");
  }
  if (x) RELEASE(x);
  if (y) RELEASE(y);
MEM_END
  return;
}

static void show_path (int hidden) 
{
  int len;
  /* The semantics of a fill_color of 0.0 or 0.5 will be to use current
     painting color known to dvipdfm */
  if (fill_shape && fill_color != 0.0) {
    sprintf (work_buffer, " %.2f g", fill_color);
    len = strlen (work_buffer);
    pdf_doc_add_to_page (work_buffer, len);
  }
  if (!hidden && fill_shape) {
    pdf_doc_add_to_page (" b", 2);
  }
  if (hidden && fill_shape) {
    pdf_doc_add_to_page (" f", 2);
  }
  if (!hidden && !fill_shape)
    pdf_doc_add_to_page (" S", 2);
  if (fill_shape)
    fill_shape = 0;
  fill_color = 0.0;
  fill_shape = 0.0;
}


static void flush_path (double x_user, double y_user, int hidden,
			double dash_dot)
{
  int len;
MEM_START
  /* Make pen_size == 0 equivalent to hidden */
  if (pen_size == 0)
    hidden = 1;
  if (path_length > 1) {
    int i;
    sprintf (work_buffer, " q");
    len = strlen (work_buffer);
    pdf_doc_add_to_page (work_buffer, len);
    if (pen_size != 0.0) {
      sprintf (work_buffer, " %.2f w", pen_size);
      len = strlen (work_buffer);
    }
    pdf_doc_add_to_page (work_buffer, len);
    sprintf (work_buffer, " 1 J 1 j");
    len = strlen (work_buffer);
    pdf_doc_add_to_page (work_buffer, len);
    if (dash_dot != 0.0) {
      if (dash_dot > 0.0) {
	sprintf (work_buffer, " [%.1f %.1f] 0 d",
		 dash_dot*72.0, dash_dot*36.0);
        len = strlen (work_buffer);
      } else {
	sprintf (work_buffer, " [%.1f %.1f] 0 d", pen_size,
		 -dash_dot*72.0);
        len = strlen (work_buffer);
      }
      pdf_doc_add_to_page (work_buffer, len);
    }
    sprintf (work_buffer, " %.2f %.2f m",
	     x_user+path[0].x, y_user-path[0].y);
    len = strlen (work_buffer);
    pdf_doc_add_to_page (work_buffer, len);
    for (i=0; i<path_length; i++) {
      sprintf (work_buffer, " %.2f %.2f l", x_user+path[i].x, y_user-path[i].y);
      len = strlen (work_buffer);
      pdf_doc_add_to_page (work_buffer, len);
    } 
    show_path (hidden);
    pdf_doc_add_to_page (" Q", 2);
  } else {
    fprintf (stderr, "tpic special: fp: Not enough points!\n");
  }
  tpic_clear_state();
MEM_END
  return;
}
static void spline_path (double x_user, double y_user, double dash_dot)
{
  int len;
MEM_START
  /* Spline is meaningless for path length of less than 3 */
  if (path_length > 2) {
    int i;
    sprintf (work_buffer, " q 1.4 M %.2f w", pen_size);
    len = strlen (work_buffer);
    pdf_doc_add_to_page (work_buffer, len);
    if (dash_dot != 0.0) {
      if (dash_dot > 0.0) {
	sprintf (work_buffer, " [%.1f %.1f] 0 d",
		 dash_dot*72.0, dash_dot*36.0);
        len = strlen (work_buffer);
      } else if (dash_dot < 0.0) {
	sprintf (work_buffer, " [%.1f %.1f] 0 d 1 J", pen_size,
		 -dash_dot*72.0);
        len = strlen (work_buffer);
      }
      pdf_doc_add_to_page (work_buffer, len);
    }
    sprintf (work_buffer, " %.2f %.2f m",
	     x_user+path[0].x, y_user-path[0].y);
    len = strlen (work_buffer);
    pdf_doc_add_to_page (work_buffer, len);
    sprintf (work_buffer, " %.2f %.2f l",
	     x_user+0.5*(path[0].x+path[1].x), 
	     y_user-0.5*(path[0].y+path[1].y));
    len = strlen (work_buffer);
    pdf_doc_add_to_page (work_buffer, len);
    for (i=1; i<path_length-1; i++) {
      sprintf (work_buffer, " %.2f %.2f %.2f %.2f y",
	       x_user+path[i].x, y_user-path[i].y,
	       x_user+0.5*(path[i].x+path[i+1].x),
	       y_user-0.5*(path[i].y+path[i+1].y));
      len = strlen (work_buffer);
      pdf_doc_add_to_page (work_buffer, len);
    } 
    sprintf (work_buffer, " %.2f %.2f l",
	     x_user+path[path_length-1].x,
	     y_user-path[path_length-1].y);
    len = strlen (work_buffer);
    pdf_doc_add_to_page (work_buffer, len);
    show_path (0);
    pdf_doc_add_to_page (" Q", 2);
  } else {
    fprintf (stderr, "tpic special: sp: Not enough points!\n");
  }
  tpic_clear_state();
MEM_END
  return;
}

static void arc (char **buffer, char *end, double x_user, double
		 y_user, int hidden) 
{
  char *xcs= NULL, *ycs= NULL,
    *xrs=NULL, *yrs=NULL, *sas=NULL, *eas=NULL;
  char *save;
  double xc, yc, xr, yr, sa, ea;
MEM_START
  save = *buffer;
/* pen_size == 0 is equivalent to hidden */ 
  if (pen_size == 0)
    hidden = 1;
  if ((xcs=parse_number(buffer, end)) &&
      (ycs=parse_number(buffer, end)) &&
      (xrs=parse_number(buffer, end)) &&
      (yrs=parse_number(buffer, end)) &&
      (sas=parse_number(buffer, end)) &&
      (eas=parse_number(buffer, end))) {
    double c, s, cur_x, cur_y, inc_ang;
    double cp1_x, cp1_y, cp2_x, cp2_y;
    double new_x, new_y;
    int len, i, nsteps;
    xc=atof (xcs)*MI2DEV; yc=atof (ycs)*MI2DEV;
    xr=atof (xrs)*MI2DEV; yr=atof (yrs)*MI2DEV;
    sa=atof (sas); ea=atof (eas);
#define ROTATE(x,y,c,s) {new_x=(c)*(x)-(s)*(y);new_y=(s)*(x)+(c)*(y);x=new_x,y=new_y;}
    #define MAX_ANG_STEP 1.0
    nsteps = (int) ((ea-sa)/MAX_ANG_STEP) + 1;
    inc_ang = (ea-sa)/nsteps;
    c = cos(inc_ang); s = sin(inc_ang);
    cur_x=cos(sa); cur_y=sin(sa);
    cp1_x = cur_x - inc_ang/3.0*cur_y;
    cp1_y = cur_y + inc_ang/3.0*cur_x;
    cp2_x = cur_x + inc_ang/3.0*cur_y;
    cp2_y = cur_y - inc_ang/3.0*cur_x;
    sprintf (work_buffer, " q");
    len = strlen (work_buffer);
    pdf_doc_add_to_page (work_buffer, len);
    if (pen_size != 0.0) {
      sprintf (work_buffer, " %.2f w", pen_size);
      len = strlen (work_buffer);
     }
    pdf_doc_add_to_page (work_buffer, len);
    sprintf (work_buffer, " 1 J");
    len = strlen (work_buffer);
    pdf_doc_add_to_page (work_buffer, len);
    sprintf (work_buffer, " %.2f %.2f m", x_user+xr*cur_x+xc, y_user-yr*cur_y-yc);
    len = strlen (work_buffer);
    pdf_doc_add_to_page (work_buffer, len);
    ROTATE (cp2_x, cp2_y, c, s);
    ROTATE (cur_x, cur_y, c, s);
    for (i=0; i<nsteps; i++) {
      sprintf (work_buffer, " %.2f %.2f %.2f %.2f %.2f %.2f c",
	       x_user+xr*cp1_x+xc, y_user-yr*cp1_y-yc,
	       x_user+xr*cp2_x+xc, y_user-yr*cp2_y-yc,
	       x_user+xr*cur_x+xc, y_user-yr*cur_y-yc);
      len = strlen (work_buffer);
      pdf_doc_add_to_page (work_buffer, len);
      ROTATE (cur_x, cur_y, c, s);
      ROTATE (cp1_x, cp1_y, c, s);
      ROTATE (cp2_x, cp2_y, c, s);
    }
    show_path (hidden);
    pdf_doc_add_to_page (" Q", 2);
  } else {
    dump (save, end);
    fprintf (stderr, "tpic special: ar/ir: Error in parameters\n");
  }
  if (xcs) RELEASE(xcs);
  if (ycs) RELEASE(ycs);
  if (xrs) RELEASE(xrs);
  if (yrs) RELEASE(yrs);
  if (sas) RELEASE(sas);
  if (eas) RELEASE(eas);
MEM_END
  tpic_clear_state();
  return;
}

#define TPIC_PN 1
#define TPIC_PA 2
#define TPIC_FP 3
#define TPIC_IP 4
#define TPIC_DA 5
#define TPIC_DT 6
#define TPIC_SP 7
#define TPIC_AR 8
#define TPIC_IA 9
#define TPIC_SH 10
#define TPIC_WH 11
#define TPIC_BK 12
#define TPIC_TX 13

struct {
  char *s;
  int tpic_command;
} tpic_specials[] = {
  {"pn", TPIC_PN},
  {"pa", TPIC_PA},
  {"fp", TPIC_FP},
  {"ip", TPIC_IP},
  {"da", TPIC_DA},
  {"dt", TPIC_DT},
  {"sp", TPIC_SP},
  {"ar", TPIC_AR},
  {"ia", TPIC_IA},
  {"sh", TPIC_SH},
  {"wh", TPIC_WH},
  {"bk", TPIC_BK},
  {"tx", TPIC_TX}
};

int tpic_parse_special(char *buffer, UNSIGNED_QUAD size, double
		       x_user, double y_user)
{
  int i, tpic_command, result = 0;
  char *end = buffer + size;
  char *token;
  skip_white (&buffer, end);
  if ((token = parse_ident (&buffer, end))) {
    for (i=0; i<(sizeof(tpic_specials)/sizeof(tpic_specials[0])); i++) {
      if (!strcmp (tpic_specials[i].s, token))
	break;
    }
    RELEASE (token);
  } else
    return 0;
  if (i < sizeof(tpic_specials)/sizeof(tpic_specials[0])) {
    tpic_command = tpic_specials[i].tpic_command;
    skip_white (&buffer, end);
    result = 1;
    switch (tpic_command) {
    case TPIC_PN:
      set_pen_size (&buffer, end);
      break;
    case TPIC_PA:
      add_point (&buffer, end);
      break;
    case TPIC_FP:
      flush_path(x_user, y_user, 0, 0.0);
      break;
    case TPIC_IP: 
      flush_path(x_user, y_user, 1, 0.0);
      break;
    case TPIC_DA:
      {
	char *s;
	if ((s=parse_number(&buffer, end))) {
	  flush_path(x_user, y_user, 0, atof (s));
	  RELEASE (s);
	}
      }
      break;
    case TPIC_DT:
      {
	char *s;
	if ((s=parse_number(&buffer, end))) {
	  flush_path(x_user, y_user, 0, -atof (s));
	  RELEASE (s);
	}
      }
      break;
    case TPIC_SP:
      {
	char *s;
	if ((s=parse_number(&buffer, end)))
	  spline_path (x_user, y_user, atof (s));
	else
	  spline_path(x_user, y_user, 0.0);
	break;
      }
    case TPIC_AR:
      arc (&buffer, end, x_user, y_user, 0);
      break;
    case TPIC_IA:
      arc (&buffer, end, x_user, y_user, 1);
      break;
    case TPIC_SH:
      set_fill_color (&buffer, end);
      break;
    case TPIC_WH: 
      fill_shape = 1;
      fill_color = 1.0;
      break;
    case TPIC_BK:
      fill_shape = 1;
      fill_color = 0.0;
      break;
    case TPIC_TX: 
      fill_shape = 1;
      {
	long num = 0, den = 0;
	while (buffer++ < end) {
	  switch (*(buffer++)) {
	  case '0':
	    num += 0;
	  case '1':
	  case '2':
	  case '4':
	  case '8':
	    num += 1;
	    break;
	  case '3':
	  case '5':
	  case '6':
	  case '9':
	  case 'a':
	  case 'A':
	  case 'c':
	  case 'C':
	    num += 2;
	    break;
	  case '7':
	  case 'b':
	  case 'B':
	  case 'd':
	  case 'D':
	    num += 3;
	    break;
	  case 'f':
	  case 'F':
	    num += 4;
	    break;
	  default:
	    break;
	  }
	  den += 16;
	}
	if (den != 0) {
	  default_fill_color = 1.0 - (float) (num)/(den);
	}
	else {
	  default_fill_color = 0.5;
	}
      }
      break;
    default:
      fprintf (stderr, "Fix me, I'm broke.  This should never happen");
      exit(1);
    }
  } else {
    result = 0;
  }
  return result;
}



