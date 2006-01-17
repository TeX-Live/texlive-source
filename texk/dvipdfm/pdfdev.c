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

#include "config.h"
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "system.h"
#include "mem.h"
#include "error.h"
#include "mfileio.h"
#include "numbers.h"
#include "dvi.h"
#include "tfm.h"
#include "pdfdev.h"
#include "pdfdoc.h"
#include "pdfobj.h"
#include "type1.h"
#include "ttf.h"
#include "pkfont.h"
#include "pdfspecial.h"
#include "pdfparse.h"
#include "tpic.h"
#include "htex.h"
#include "mpost.h"
#include "psspecial.h"
#include "colorsp.h"
#include "pdflimits.h"
#include "twiddle.h"
#include "encodings.h"

/* Internal functions */
static void dev_clear_color_stack (void);
static void dev_clear_xform_stack (void);

double hoffset = 72.0, voffset=72.0;

static double dvi2pts = 0.0;

 /* Acrobat doesn't seem to like coordinate systems
    that involve scalings around 0.01, so we use
    a scaline of 1.0.  In other words, device units = pts */ 
/* Following dimensions in virtual device coordinates,
which are points */

static double page_width=612.0, page_height=792.0;
int page_size_readonly = 0;

void dev_set_page_size (double width, double height)
{
  if (page_size_readonly) {
    fprintf (stderr, "\nSorry.  Too late to change page size\n");
  } else {
    page_width = width;
    page_height = height;
  }
}

double dev_page_width(void)
{
  page_size_readonly = 1;
  return page_width;
}

double dev_page_height(void)
{
  page_size_readonly = 1;
  return page_height;
}

static int debug = 0, verbose = 0;

void dev_set_verbose (void)
{
  verbose += 1;
}
void dev_set_debug (void)
{
  debug = 1;
}

#define GRAPHICS_MODE 1
#define TEXT_MODE 2
#define STRING_MODE 3

int motion_state = GRAPHICS_MODE; /* Start in graphics mode */

#define FORMAT_BUF_SIZE 4096
static char format_buffer[FORMAT_BUF_SIZE];

/* The coordinate system in the pdf file is setup so that 1 unit in the
   PDF content stream's coordinate system represents 65,800 DVI units.
   This choice was made so that a PDF coordinate represented only
   to the hundredths place represents an exact integer number of DVI units.
   Doing so allows relative motions in a PDF file to be known
   precisely in DVI units, and allows us to keep track of relative motions
   using integer arithmetic.  Relative motions in the PDF file are
   represented in decimal with no more than two digits after the decimal
   point.  In the PDF stream, a PDF user coordinate of 0.01 represents
   exactly 658 DVI units.

   The "pdfdev" module is the only module that knows the
   relationship between PDF units and true points.  It provides
   pdf_dev_scale() to inform other modules of the scale.
   Modules that render PDF marking operators (such as those
   that render tpic specials or PS specials) need this value.
   The overhead of this call is a slight performance hit for
   rendering images, but allows dvipdfm to set text blazingly fast
  
 
   Some constants related to this representation follow: */

#define PDF_U 65800L	/* Number of DVI units in a PDF unit */
#define CENTI_PDF_U 658	/* Number of DVI units in a centi PDF unit */

/* pdf_dev_scale() returns the factor by which a PDF unit
 * must be multiplied to produce an Adobe point (big point) */

double pdf_dev_scale (void)
{
  return 65800.0*dvi2pts;
}


 /* Device coordinates are relative to upper left of page.  One of the
    first things appearing in the page stream is a coordinate transformation
    matrix that forces this to be true.  This coordinate
    transformation is the only place where the paper size is required.
    Unfortunately, positive is up, which doesn't agree with TeX's convention.  */

static spt_t text_xorigin = 0, text_yorigin = 0,
  text_offset = 0;
double text_slant = 0.0, text_extend = 1.0;

unsigned  num_dev_fonts = 0;
unsigned  num_phys_fonts = 0;
int current_font = -1;

#define PHYSICAL 1
#define VIRTUAL 2

#define TYPE1 1
#define PK 2
#define TRUETYPE 3

#define DEFAULT_MAP_FILE "dvipdfm.map"

static struct dev_font {
  char short_name[7];	/* Needs to be big enough to hold name "Fxxx"
			   where xxx is number of largest font */
  unsigned char used_on_this_page, format;
  char *tex_name;
  spt_t sptsize;
  pdf_obj *font_resource;
  char *used_chars;
  double extend, slant;
  int remap;
} *dev_font = NULL;

static unsigned max_device_fonts = 0;

void dev_fonts_need (unsigned n)
{
  if (n > max_device_fonts) {
    max_device_fonts = MAX(max_device_fonts+MAX_FONTS, n);
    dev_font = RENEW (dev_font, max_device_fonts, struct dev_font);
  }
}

struct map_record {
   char *tex_name, *font_name, *enc_name;
   double slant, extend;
   int remap;
}  *font_map = NULL;

unsigned int num_font_map = 0, max_font_map = 0;

static void font_maps_need (int n)
{
  if (n > max_font_map) {
    max_font_map = MAX(max_font_map+MAX_FONTS, n);
    font_map = RENEW (font_map, max_font_map, struct map_record);
  }
  return;
}

static void init_map_record (struct map_record *r) 
{
  r->tex_name = NULL;
  r->enc_name = NULL;
  r->font_name = NULL;
  r->slant = 0.0;
  r->extend = 1.0;
  r->remap = 0.0;
  return;
}

static void release_map_record (struct map_record *r)
{
  if (r && r->tex_name)
    RELEASE (r->tex_name);
  if (r && r->enc_name)
    RELEASE (r->enc_name);
  if (r && r->font_name)
    RELEASE (r->font_name);
}


static void fill_in_defaults (struct map_record *this_map_record)
{
  if (this_map_record -> enc_name != NULL && 
      (!strcmp (this_map_record->enc_name, "default") ||
       !strcmp (this_map_record->enc_name, "none"))) {
    RELEASE(this_map_record->enc_name);
    this_map_record -> enc_name = NULL;
  }
  if (this_map_record -> font_name != NULL && 
      (!strcmp (this_map_record->font_name, "default") ||
       !strcmp (this_map_record->font_name, "none"))) {
    RELEASE(this_map_record->font_name);
    this_map_record -> font_name = NULL;
  }
  /* We *must* fill in a font_name either explicitly or by default
     (the tex_name) */
  if (this_map_record -> font_name == NULL) {
    this_map_record -> font_name = NEW (strlen(this_map_record->tex_name)+1, char);
    strcpy (this_map_record->font_name, this_map_record->tex_name);
  }
  return;
}

void dev_read_mapfile (char *filename)
{
  FILE *mapfile;
  char *full_map_filename, *start = NULL, *end, *tex_name;
  if (verbose > 0)
    fprintf (stderr, "<%s", filename);
  full_map_filename = kpse_find_file (filename, kpse_fontmap_format,
				      0);
  if (full_map_filename == NULL || 
      (mapfile = MFOPEN (full_map_filename, FOPEN_R_MODE)) == NULL) {
    fprintf (stderr, "Warning:  Couldn't open font map file %s\n", filename);
    mapfile = NULL;
  }
  if (mapfile) {
    while ((start = mfgets (work_buffer, WORK_BUFFER_SIZE, mapfile)) !=
	   NULL) {
      end = work_buffer + strlen(work_buffer);
      skip_white (&start, end);
      if (start >= end)
	continue;
      if (*start == '%')
	continue;
      if ((tex_name = parse_ident (&start, end)) == NULL)
	continue;
      /* Parse record line in map file.  First two fields (after TeX font
	 name) are position specific.  Arguments start at the first token
	 beginning with a  '-' */
      font_maps_need (num_font_map+1);
      init_map_record(font_map+num_font_map);
      font_map[num_font_map].tex_name = tex_name;
      skip_white (&start, end);
      if (*start != '-') {
	font_map[num_font_map].enc_name = parse_ident (&start, end); /* May be null */  
	skip_white (&start, end);
      }
      if (*start != '-') {
	font_map[num_font_map].font_name = parse_ident (&start, end); /* May be null */
	skip_white (&start, end);
      }
      /* Parse any remaining arguments */ 
      while (start+1 < end && *start == '-') {
	char *number;
	switch (*(start+1)) {
	case 's': /* Slant option */
	  start += 2;
	  skip_white (&start, end);
	  if (start < end && 
	      (number = parse_number(&start, end))) {
	    font_map[num_font_map].slant = atof (number);
	    RELEASE (number);
	  } else {
	    fprintf (stderr, "\n\nMissing slant value in map file for %s\n\n",
		     tex_name);
	  }
	  break;
	case 'e': /* Extend option */
	  start += 2;
	  skip_white (&start, end);
	  if (start < end && 
	      (number = parse_number(&start, end))) {
	    font_map[num_font_map].extend = atof (number);
	    RELEASE (number);
	  } else {
	    fprintf (stderr, "\n\nMissing extend value in map file for %s\n\n",
		     tex_name);
	  }
	  break;
	case 'r': /* Remap option */
	  start += 2;
	  skip_white (&start, end);
	  font_map[num_font_map].remap = 1;
	  break;
	default: 
	  fprintf (stderr, "\n\nWarning: Unrecognized option in map file %s: -->%s<--\n\n",
		   tex_name, start);
	  start = end;
	}
	skip_white (&start, end);
      }
      fill_in_defaults (font_map+num_font_map);
      num_font_map += 1;
    }
    MFCLOSE (mapfile);
    if (verbose > 0)
      fprintf (stderr, ">");
  }
  return;
}

struct map_record *get_map_record (const char *tex_name)
{
  struct map_record *result = NULL;
  int tried_default = 0;
  unsigned int i;
  if (!font_map && !tried_default) {
    dev_read_mapfile (DEFAULT_MAP_FILE);
    tried_default = 1;
  }
  if (!font_map)
    return result;
  for (i=0; i<num_font_map; i++) {
    if (!strcmp (font_map[i].tex_name, tex_name)) {
      result = font_map+i;
      break;
    }
  }
  return result;
}

/*
 * reset_text_state() outputs a BT
 * and does any necessary coordinate transformations
 * to get ready to ship out text
 */

static void reset_text_state(void)
{
  int len;
  text_xorigin = 0;
  text_yorigin = 0;
  text_offset = 0;
  /* 
   * We need to reset the line matrix to handle slanted fonts 
   */
  sprintf (format_buffer, " BT");
  len = strlen (format_buffer);
  if (current_font >= 0 && /* If not at top of page */
      (dev_font[current_font].slant != 0.0 ||
       dev_font[current_font].extend != 1.0)) {
    sprintf (format_buffer+len, " %.7g 0 %.3g 1 ",
	     dev_font[current_font].extend,
	     dev_font[current_font].slant);
    len += strlen (format_buffer+len);
    len += centi_u_to_a (format_buffer+len, IDIVRND (text_xorigin, CENTI_PDF_U));
    format_buffer[len++] = ' ';
    len += centi_u_to_a (format_buffer+len, IDIVRND (text_yorigin, CENTI_PDF_U));
    format_buffer[len++] = ' ';
    format_buffer[len++] = 'T';
    format_buffer[len++] = 'm';
  }
  pdf_doc_add_to_page (format_buffer, len);
}

static void text_mode (void)
{
  switch (motion_state) {
  case STRING_MODE:
    pdf_doc_add_to_page (")]TJ", 4);
  case TEXT_MODE:
    break;
  case GRAPHICS_MODE:
    reset_text_state();
    break;
  }
  motion_state = TEXT_MODE;
  text_offset = 0;
  return;
}

void graphics_mode (void)
{
  switch (motion_state) {
  case GRAPHICS_MODE:
    break;
  case STRING_MODE:
    pdf_doc_add_to_page (")]TJ", 4);
  case TEXT_MODE:
    pdf_doc_add_to_page (" ET", 3);
    break;
  }
  motion_state = GRAPHICS_MODE;
  return;
}

static void string_mode (spt_t xpos, spt_t ypos, double slant, double extend)
{
  spt_t delx, dely;
  int len = 0;
  switch (motion_state) {
  case STRING_MODE:
    break;
  case GRAPHICS_MODE:
    reset_text_state();
    /* Fall through now... */
    /* Following may be necessary after a rule (and also after
       specials) */
  case TEXT_MODE:
    delx = xpos - text_xorigin;
    {
      spt_t rounded_delx, desired_delx;
      spt_t rounded_dely, desired_dely;
      spt_t dvi_xerror, dvi_yerror;

      /* First round dely (it is needed for delx) */
      dely = ypos - text_yorigin;
      desired_dely = dely;
      rounded_dely = IDIVRND(desired_dely, CENTI_PDF_U) * CENTI_PDF_U;
      /* Next round delx, precompensating for line transformation matrix */
      desired_delx = (delx-desired_dely*slant)/extend;
      rounded_delx = IDIVRND(desired_delx, CENTI_PDF_U) * CENTI_PDF_U;
      /* Estimate errors in DVI units */
      dvi_yerror = (desired_dely - rounded_dely);
      dvi_xerror = (extend*(desired_delx - rounded_delx)+slant*dvi_yerror);
      format_buffer[len++] = ' ';
      len += centi_u_to_a (format_buffer+len, rounded_delx/CENTI_PDF_U);
      format_buffer[len++] = ' ';
      len += centi_u_to_a (format_buffer+len, rounded_dely/CENTI_PDF_U);
      pdf_doc_add_to_page (format_buffer, len);
      len = 0;
      pdf_doc_add_to_page (" TD[(", 5);
      text_xorigin = xpos-dvi_xerror;
      text_yorigin = ypos-dvi_yerror;
    }
    text_offset = 0;
    break;
  }
  motion_state = STRING_MODE;
  return;
}

/* The purpose of the following routine is to force a Tf only
   when it's actually necessary.  This became a problem when the
   VF code was added.  The VF spec says to instantiate the
   first font contained in the VF file before drawing a virtual
   character.  However, that font may not be used for
   many characters (e.g. small caps fonts).  For this reason, 
   dev_select_font() should not force a "physical" font selection.
   This routine prevents a PDF Tf font selection until there's
   really a character in that font.  */

static void dev_set_font (int font_id)
{
  int len = 0;
  text_mode();
  sprintf (format_buffer, "/%s ", dev_font[font_id].short_name);
  len = strlen (format_buffer);
  len += centi_u_to_a (format_buffer+len, IDIVRND(dev_font[font_id].sptsize, CENTI_PDF_U));
  format_buffer[len++] = ' ';
  format_buffer[len++] = 'T';
  format_buffer[len++] = 'f';
  if (dev_font[font_id].slant != text_slant ||
      dev_font[font_id].extend != text_extend) {
    sprintf (format_buffer+len, " %.7g 0 %.3g 1 ",
	     dev_font[font_id].extend,
	     dev_font[font_id].slant);
    len += strlen (format_buffer+len);
    len += centi_u_to_a (format_buffer+len, IDIVRND(text_xorigin, CENTI_PDF_U));
    format_buffer[len++] = ' ';
    len += centi_u_to_a (format_buffer+len, IDIVRND(text_yorigin, CENTI_PDF_U));
    format_buffer[len++] = ' ';
    format_buffer[len++] = 'T';
    format_buffer[len++] = 'm';
     /* There's no longer any uncertainty about where we are */
    text_slant = dev_font[font_id].slant;
    text_extend = dev_font[font_id].extend;
  }
  pdf_doc_add_to_page (format_buffer, len);
  /* Add to Font list in Resource dictionary for this page */
  if (!dev_font[font_id].used_on_this_page) { 
    pdf_doc_add_to_page_fonts (dev_font[font_id].short_name,
			       pdf_link_obj(dev_font[font_id].font_resource));
    dev_font[font_id].used_on_this_page = 1;
  }
  current_font = font_id;
  return;
}

void dev_set_string (spt_t xpos, spt_t ypos, unsigned char *s, int
		     length, spt_t width, int font_id)
{
  int len = 0;
  long kern;
  
  if (font_id != current_font)
    dev_set_font(font_id); /* Force a Tf since we are actually trying
			       to write a character */
  /* Kern is in units of character units, i.e., 1000 = 1 em. */
  /* The following formula is of the form a*x/b where a, x, and b are
     long integers.  Since in integer arithmetic (a*x) could overflow
     and a*(x/b) would not be accurate, we use floating point
     arithmetic rather than trying to do this all with integer
     arithmetic. */
  kern =
    (1000.0/dev_font[font_id].extend*(text_xorigin+text_offset-xpos))/dev_font[font_id].sptsize;
  
  if (labs(ypos-text_yorigin) > CENTI_PDF_U || /* CENTI_PDF_U is smallest resolvable dimension */
      abs(kern) > 32000) { /* Some PDF Readers fail on large kerns */
    text_mode();
    kern = 0;
  }
  if (motion_state != STRING_MODE)
    string_mode(xpos, ypos, dev_font[font_id].slant, dev_font[font_id].extend);
  else if (kern != 0) {
    text_offset -=
      kern*dev_font[font_id].extend*(dev_font[font_id].sptsize/1000.0);
    /* Same issues as earlier.  Use floating point for simplicity */
    /* This routine needs to be fast, so we don't call sprintf() or
       strcpy() */
    format_buffer[len++] = ')';
    len += inttoa (format_buffer+len, kern);
    format_buffer[len++] = '(';
    pdf_doc_add_to_page (format_buffer, len);
    len = 0;
  }
  len += pdfobj_escape_str (format_buffer+len, FORMAT_BUF_SIZE-len, s,
			    length,
			    dev_font[font_id].remap);
  pdf_doc_add_to_page (format_buffer, len);

  /* Record characters used for partial font embedding */
  /* Fonts without pfbs don't get counted and have used_chars set to
     null */
  if (dev_font[font_id].used_chars != NULL) {
    int i;
    if (dev_font[font_id].remap)
      for (i=0; i<length; i++){
	(dev_font[font_id].used_chars)[twiddle(s[i])] = 1;
      }
    else 
      for (i=0; i<length; i++){
	(dev_font[font_id].used_chars)[s[i]] = 1;
      }
  }
  text_offset += width;
}

void dev_init (double scale, double x_offset, double y_offset)
{
  dvi2pts = scale;
  hoffset = x_offset;
  voffset = y_offset;
  if (debug) fprintf (stderr, "dev_init:\n");
  graphics_mode();
  dev_clear_color_stack();
  dev_clear_xform_stack();
}

void dev_close (void)
{
  /* Set page origin now that user has had plenty of time
     to set page size */
  pdf_doc_set_origin((double) hoffset, (double)
		     dev_page_height()-voffset);
}

void dev_add_comment (char *comment)
{
  pdf_doc_creator (comment);
}


/*  BOP, EOP, and FONT section.
   BOP and EOP manipulate some of the same data structures
   as the font stuff */ 

#define GRAY 1
#define RGB 2
#define CMYK 3
struct color {
  int colortype;
  double c1, c2, c3, c4;
} colorstack[MAX_COLORS], background = {GRAY, 1.0, 1.0, 1.0, 1.0},
    default_color = {GRAY, 0.0, 0.0, 0.0, 0.0};

#include "colors.h"

struct color color_by_name (char *s) 
{
  int i;
  struct color result;
  for (i=0; i<sizeof(colors_by_name)/sizeof(colors_by_name[0]); i++) {
    if (!strcmp (s, colors_by_name[i].name)) {
      break;
    }
  }
  if (i == sizeof(colors_by_name)/sizeof(colors_by_name[0])) {
    fprintf (stderr, "Color \"%s\" no known.  Using \"Black\" instead.\n", s);
    result = default_color;
  } else {
    result = colors_by_name[i].color;
  }
  return result;
}

static int num_colors = 0;

static void fill_page (void)
{
  if (background.colortype == GRAY && background.c1 == 1.0)
    return;
  switch (background.colortype) {
  case GRAY:
    sprintf (format_buffer, " q 0 w %.3f g %.3f G", background.c1, background.c1);
    break;
  case RGB:
    sprintf (format_buffer, " q 0 w %.3f %.3f %.3f rg %.3f %.3f %.3f RG",
	     background.c1, background.c2, background.c3,
	     background.c1, background.c2, background.c3);
    break;
  case CMYK:
    sprintf (format_buffer, " q 0 w %.3f %.3f %.3f %.3f k %.3f %.3f %.3f %.3f K ",
	     background.c1, background.c2, background.c3, background.c4,
	     background.c1, background.c2, background.c3, background.c4);
    break;
  }
  pdf_doc_this_bop (format_buffer, strlen(format_buffer));
  sprintf (format_buffer,
	   " 0 0 m %.2f 0 l %.2f %.2f l 0 %.2f l b Q ",
	   dev_page_width(), dev_page_width(), dev_page_height(),
	   dev_page_height());
  pdf_doc_this_bop (format_buffer, strlen(format_buffer));
  return;
}

void dev_bg_rgb_color (double r, double g, double b)
{
  background.colortype = RGB;
  background.c1 = r;
  background.c2 = g;
  background.c3 = b;
  return;
}

void dev_bg_cmyk_color (double c, double m, double y, double k)
{
  background.colortype = CMYK;
  background.c1 = c;
  background.c2 = m;
  background.c3 = y;
  background.c4 = k;
  return;
}

void dev_bg_gray (double value)
{
  background.colortype = GRAY;
  background.c1 = value;
  return;
}

void dev_bg_named_color (char *s)
{
  struct color color = color_by_name (s);
  switch (color.colortype) {
  case GRAY:
    dev_bg_gray (color.c1);
    break;
  case RGB:
    dev_bg_rgb_color (color.c1, color.c2, color.c3);
    break;
  case CMYK:
    dev_bg_cmyk_color (color.c1, color.c2, color.c3, color.c4);
    break;
  }
  return;
}

static void dev_clear_color_stack (void)
{
  num_colors = 0;
  return;
}
static void dev_set_color (struct color color)
{
  switch (color.colortype) {
    int len;
  case RGB:
    sprintf (format_buffer, " %.2f %.2f %.2f",
	     color.c1,
	     color.c2,
	     color.c3);
    len = strlen (format_buffer);
    pdf_doc_add_to_page (format_buffer, len);
    pdf_doc_add_to_page (" rg", 3);
    pdf_doc_add_to_page (format_buffer, len);
    pdf_doc_add_to_page (" RG", 3);
    break;
  case CMYK:
    sprintf (format_buffer, " %.2f %.2f %.2f %.2f",
	     color.c1,
	     color.c2,
	     color.c3,
	     color.c4);
    len = strlen (format_buffer);
    pdf_doc_add_to_page (format_buffer, len);
    pdf_doc_add_to_page (" k", 2);
    pdf_doc_add_to_page (format_buffer, len);
    pdf_doc_add_to_page (" K ", 3);
    break;
  case GRAY:
    sprintf (format_buffer, " %.2f", color.c1);
    len = strlen (format_buffer);
    pdf_doc_add_to_page (format_buffer, len);
    pdf_doc_add_to_page (" g", 2);
    pdf_doc_add_to_page (format_buffer, len);
    pdf_doc_add_to_page (" G", 2);
    break;
  default:
    ERROR ("Internal error: Invalid default color item");
  }
}


void dev_do_color (void) 
{
  if (num_colors == 0) {
    dev_set_color (default_color);
  } else {
    dev_set_color (colorstack[num_colors-1]);
  }
  return;
}

void dev_set_def_rgb_color (double r, double g, double b)
{
  default_color.c1 = r;
  default_color.c2 = g;
  default_color.c3 = b;
  default_color.colortype = RGB;
  dev_do_color();
  return;
}

void dev_set_def_gray (double g) 
{
  default_color.c1 = g;
  default_color.colortype = GRAY;
  dev_do_color();
  return;
}

void dev_set_def_named_color (char *s)
{
  struct color color = color_by_name (s);
  switch (color.colortype) {
  case GRAY:
    dev_set_def_gray (color.c1);
    break;
  case RGB:
    dev_set_def_rgb_color (color.c1, color.c2, color.c3);
    break;
  case CMYK:
    dev_set_def_cmyk_color (color.c1, color.c2, color.c3, color.c4);
    break;
  }
  return;
}

void dev_set_def_cmyk_color (double c, double m, double y, double k)
{
  default_color.c1 = c;
  default_color.c2 = m;
  default_color.c3 = y;
  default_color.c4 = k;
  default_color.colortype = CMYK;
  dev_do_color();
  return;
}

void dev_begin_named_color (char *s)
{
  struct color color = color_by_name (s);
  switch (color.colortype) {
  case GRAY:
    dev_begin_gray (color.c1);
    break;
  case RGB:
    dev_begin_rgb_color (color.c1, color.c2, color.c3);
    break;
  case CMYK:
    dev_begin_cmyk_color (color.c1, color.c2, color.c3, color.c4);
    break;
  }
  return;
}

void dev_begin_rgb_color (double r, double g, double b)
{
  if (num_colors >= MAX_COLORS) {
    fprintf (stderr, "\ndev_set_color:  Exceeded depth of color stack\n");
    return;
  }
  colorstack[num_colors].c1 = r;
  colorstack[num_colors].c2 = g;
  colorstack[num_colors].c3 = b;
  colorstack[num_colors].colortype = RGB;
  num_colors+= 1;
  dev_do_color();
}

void dev_begin_cmyk_color (double c, double m, double y, double k)
{
  if (num_colors >= MAX_COLORS) {
    fprintf (stderr, "\ndev_set_color:  Exceeded depth of color stack\n");
    return;
  }
  colorstack[num_colors].c1 = c;
  colorstack[num_colors].c2 = m;
  colorstack[num_colors].c3 = y;
  colorstack[num_colors].c4 = k;
  colorstack[num_colors].colortype = CMYK;
  num_colors+= 1;
  dev_do_color();
}

void dev_begin_gray (double value)
{
  if (num_colors >= MAX_COLORS) {
    fprintf (stderr, "\ndev_begin_gray:  Exceeded depth of color stack\n");
    return;
  }
  colorstack[num_colors].c1 = value;
  colorstack[num_colors].colortype = GRAY;
  num_colors+= 1;
  dev_do_color();
}

void dev_end_color (void)
{
  if (num_colors <= 0) {
    fprintf (stderr, "\ndev_set_color:  End color with no corresponding begin color\n");
    return;
  }
  num_colors -= 1;
  dev_do_color();
}

static int num_transforms = 0;

static void dev_clear_xform_stack (void)
{
  num_transforms = 0;
  return;
}

void dev_begin_xform (double xscale, double yscale, double rotate,
		      double x_user, double y_user)
{
  double c, s;
  if (num_transforms >= MAX_TRANSFORMS) {
    fprintf (stderr, "\ndev_begin_xform:  Exceeded depth of transformation stack\n");
    return;
  }
  c = ROUND (cos(rotate),1e-5);
  s = ROUND (sin(rotate),1e-5);
  sprintf (work_buffer, " q %g %g %g %g %.2f %.2f cm",
	   xscale*c, xscale*s, -yscale*s, yscale*c,
	   (1.0-xscale*c)*x_user+yscale*s*y_user,
	   -xscale*s*x_user+(1.0-yscale*c)*y_user);
  pdf_doc_add_to_page (work_buffer, strlen(work_buffer));
  num_transforms += 1;
  return;
}

void dev_end_xform (void)
{
  if (num_transforms <= 0) {
    fprintf (stderr, "\ndev_end_xform:  End transform with no corresponding begin\n");
    return;
  }
  pdf_doc_add_to_page (" Q", 2);
  num_transforms -= 1;
  /* Unfortunately, the following two lines are necessary in case of a font or color
     change inside of the save/restore pair.  Anything that was done
     there must be redone, so in effect, we make no assumptions about
     what fonts. We act like we are starting a new page */
  dev_reselect_font();
  dev_do_color();
  return;
}

int dev_xform_depth (void)
{
  return num_transforms;
}

void dev_close_all_xforms (int depth)
{
  if (num_transforms > depth) {
    fprintf (stderr, "\nspecial: Closing pending transformations at end of page/XObject\n");
    while (num_transforms > depth) {
      num_transforms -= 1;
      pdf_doc_add_to_page (" Q", 2);
    }
    dev_reselect_font();
    dev_do_color();
  }
  return;
}


/* The following routine is here for forms.  Since
   a form is self-contained, it will need its own Tf command
   at the beginningg even if it is continuing to set type
   in the current font.  This routine simply forces reinstantiation
   of the current font. */
void dev_reselect_font(void)
{
  int i;
  current_font = -1;
  for (i=0; i<num_dev_fonts; i++) {
    dev_font[i].used_on_this_page = 0;
  }
  text_slant = 0.0;
  text_extend = 1.0;
}

static void bop_font_reset(void)
{
  dev_reselect_font();
}

void dev_bop (void)
{
#ifdef MEM_DEBUG
MEM_START
#endif
  if (debug) {
    fprintf (stderr, "dev_bop:\n");
  }
  pdf_doc_new_page ();
  graphics_mode();
  {
    text_slant = 0.0;
    text_extend = 1.0;
  }
  bop_font_reset();
  /* This shouldn't be necessary because line widths are now
     explicitly set for each rule */
  /*  pdf_doc_add_to_page ("0 w", 3); */
  dev_do_color(); /* Set text color since new page loses color state */
#ifdef MEM_DEBUG
MEM_END
#endif
}

void dev_eop (void)
{
#ifdef MEM_DEBUG
MEM_START
#endif
  if (debug) {
    fprintf (stderr, "dev_eop:\n");
  }
  graphics_mode();
  dev_close_all_xforms(0);
  fill_page();
  pdf_doc_finish_page ();
  /* Finish any pending PS specials */
  mp_eop_cleanup();
#ifdef MEM_DEBUG
MEM_END
#endif
}

int dev_locate_font (const char *tex_name, spt_t ptsize)
{
  int i;
  int this_font;

  if (ptsize == 0) {
    ERROR ("locate_dev_font() called with ptsize = 0");
  }
  /* Make sure we have room for a new one, even though we
     may not actually create one */
  dev_fonts_need (num_dev_fonts+1);
  this_font = num_dev_fonts;
  
  for (i=0; i<this_font; i++) {
    /* PK must match in name and size to resolve to the same device
       font */
    if (dev_font[i].tex_name &&
	strcmp (tex_name, dev_font[i].tex_name) == 0 &&
	dev_font[i].sptsize == ptsize &&
	dev_font[i].format == PK)
      break;
    /* Scaleable fonts must match in name; however, this routine
       must return a different id if the ptsize is different */
    if (dev_font[i].tex_name &&
	strcmp (tex_name, dev_font[i].tex_name) == 0 &&
	dev_font[i].format != PK)
      break;
  }
  if (i == this_font) { /* There is no physical font we can use */
    struct map_record *map_record;
    int font_id = -1, font_format = -1, tfm_id = -1, encoding_id = -1;
    int remap = 0;
    double extend= 1.0, slant = 0.0;
    const char *font_name;
    char short_name[7];
    /* Get appropriate info from map file (yes, PK fonts at two
       different point sizes would be looked up twice unecessarily) */
    if ((map_record = get_map_record (tex_name))) {
      remap = map_record -> remap;
      slant = map_record -> slant;
      extend = map_record -> extend;
      font_name = map_record -> font_name;
    } else {
      font_name = tex_name;
    }
    if (verbose>1){
      if (map_record) {
	fprintf (stderr, "\nfontmap: %s -> %s", tex_name,
		 map_record->font_name);
	if (map_record->enc_name)
	  fprintf (stderr, "(%s)", map_record->enc_name);
	if (map_record->slant)
	  fprintf (stderr, "[slant=%g]", map_record->slant);
	if (map_record->extend != 1.0)
	  fprintf (stderr, "[extend=%g]", map_record->extend);
	if (map_record->remap)
	  fprintf (stderr, "[remap]");
	fprintf (stderr, "\n");
      } else {
	fprintf (stderr, "\nfontmap: %s (no map)\n", tex_name);
      }
    }
    /* If this font has an encoding specified on the record, get its id */
    if (map_record && map_record -> enc_name != NULL) {
      encoding_id = get_encoding (map_record -> enc_name);
    } else { /* Otherwise set the encoding_id to -1 */
      encoding_id = -1;
    }
    tfm_id = tfm_open (tex_name);
    /* We assume, for now that we will find this as a physical font,
       as opposed to a vf, so we need a device name to tell the
       lower-level routines what we want this to be called.  We'll
       blast this name away later if we don't need it. */
    short_name[0] = 'F';
    inttoa (short_name+1, num_phys_fonts+1);
    if ((font_id = type1_font (font_name, tfm_id,
			       short_name, encoding_id, remap))>=0) {
      font_format = TYPE1;
#ifdef HAVE_TTF_FORMATS
    } else if ((font_id = ttf_font (font_name, tfm_id,
				    short_name, encoding_id, remap))>=0) {
      font_format = TRUETYPE;
#endif /* HAVE_TTF_FORMATS */
    } else if ((font_id = pk_font (font_name, ptsize*dvi2pts,
				   tfm_id,
				   short_name))>=0) {
      font_format = PK;
    }
    if (font_format >= 0) { /* This is a new physical font and we found a physical
			       font we can use */
      strcpy (dev_font[this_font].short_name, short_name);
      dev_font[this_font].tex_name = NEW (strlen (tex_name)+1, char);
      strcpy (dev_font[this_font].tex_name, tex_name);
      dev_font[this_font].sptsize = ptsize;
      dev_font[this_font].format = font_format;
      dev_font[this_font].slant = slant;
      dev_font[this_font].extend = extend;
      dev_font[this_font].remap = remap;
      dev_font[this_font].used_on_this_page = 0;
      switch (font_format) {
      case TYPE1:
	dev_font[this_font].font_resource =
	  type1_font_resource(font_id);
	dev_font[this_font].used_chars = type1_font_used(font_id);
	break;
#ifdef HAVE_TTF_FORMATS
      case TRUETYPE:
	dev_font[this_font].font_resource =
	  ttf_font_resource(font_id);
	dev_font[this_font].used_chars = ttf_font_used(font_id);
	break;
#endif /* HAVE_TTF_FORMATS */
      case PK:
	dev_font[this_font].font_resource = pk_font_resource (font_id);
	dev_font[this_font].used_chars = pk_font_used(font_id);
	break;
      default:
	ERROR ("Impossible font format in dev_locate_font()");
      }
      num_phys_fonts += 1;
    } else { /* No appropriate physical font exists */
      this_font = -1; /* A flag indicating no physical font */
    }
  } else { /* A previously existing physical font can be used;
	      however, this routine must return a distinct ID if the
	      ptsizes are different.  Copy the information from the
	      previous record to the new record */
    strcpy (dev_font[this_font].short_name, dev_font[i].short_name);
    dev_font[this_font].tex_name = NEW (strlen (tex_name)+1, char);
    strcpy (dev_font[this_font].tex_name, tex_name);
    dev_font[this_font].sptsize = ptsize;
    dev_font[this_font].format = dev_font[i].format;
    dev_font[this_font].used_chars = dev_font[i].used_chars;
    dev_font[this_font].slant = dev_font[i].slant;
    dev_font[this_font].extend = dev_font[i].extend;
    dev_font[this_font].remap = dev_font[i].remap;
    /* The value in useD_on_this_page will be incorrect if the font
       has already been used on a page in a different point size.
       It's too hard to do right.  The only negative consequence is
       that there will be an attempt to add the resource to the page
       resource dict.  However, the second attempt will do nothing */
    dev_font[this_font].used_on_this_page = 0;
    dev_font[this_font].font_resource = pdf_link_obj(dev_font[i].font_resource);
    /* These two fonts are treated as having the same physical
       "used_chars" */
    dev_font[this_font].used_chars = dev_font[i].used_chars;
  }
  if (this_font >= 0)
    num_dev_fonts += 1;
  return this_font;
}
  
void dev_close_all_fonts(void)
{
  int i;
  for (i=0; i<num_dev_fonts; i++) {
    pdf_release_obj (dev_font[i].font_resource);
    RELEASE (dev_font[i].tex_name);
  }
  if (dev_font)
    RELEASE (dev_font);

  /* Release all map entries */
  for (i=0; i<num_font_map; i++) {
    release_map_record (font_map+i);
  }

  if (font_map)
    RELEASE (font_map);
  /* Close the various font handlers */
  type1_close_all();
  pk_close_all();
#ifdef HAVE_TTF_FORMATS   
  ttf_close_all();
#endif   

  /* Now do encodings. */
  encoding_flush_all();
}

void dev_rule (spt_t xpos, spt_t ypos, spt_t width, spt_t height)
{
  int len = 0;
  long w, p1, p2, p3, p4;
  graphics_mode();
   /* Is using a real stroke the right thing to do?  It seems to preserve
      the logical meaning of a "rule" as opposed to a filled rectangle.
      I am assume the reader can more intelligently render a rule than a filled rectangle */
  if (width> height) {  /* Horizontal stroke? */
    w = IDIVRND(height, CENTI_PDF_U);
    p1 = IDIVRND(xpos, CENTI_PDF_U);
    p2 = IDIVRND (2*ypos+height, 2*CENTI_PDF_U);
    p3 = IDIVRND(xpos+width,CENTI_PDF_U);
    p4 = IDIVRND (2*ypos+height, 2*CENTI_PDF_U);
  } else { /* Vertical stroke */
    w = IDIVRND(width,CENTI_PDF_U);
    p1 = IDIVRND (2*xpos+width, 2*CENTI_PDF_U);
    p2 = IDIVRND(ypos, CENTI_PDF_U);
    p3 = IDIVRND (2*xpos+width, 2*CENTI_PDF_U);
    p4 = IDIVRND(ypos+height,CENTI_PDF_U);
  }
  /* This needs to be quick */
  {
    format_buffer[len++] = ' ';
    len += centi_u_to_a (format_buffer+len, w);
    format_buffer[len++] = ' ';
    format_buffer[len++] = 'w';
    format_buffer[len++] = ' ';
    len += centi_u_to_a (format_buffer+len, p1);
    format_buffer[len++] = ' ';
    len += centi_u_to_a (format_buffer+len, p2);
    format_buffer[len++] = ' ';
    format_buffer[len++] = 'm';
    format_buffer[len++] = ' ';
    len += centi_u_to_a (format_buffer+len, p3);
    format_buffer[len++] = ' ';
    len += centi_u_to_a (format_buffer+len, p4);
    format_buffer[len++] = ' ';
    format_buffer[len++] = 'l';
    format_buffer[len++] = ' ';
    format_buffer[len++] = 'S';
  }
  pdf_doc_add_to_page (format_buffer, len);
}

/* The following routines tell the coordinates in true Adobe points
   with the coordinate system having its origin at the bottom
   left of the page. */

double dev_phys_x (void)
{
  return dvi_dev_xpos()*dvi_tell_mag() + hoffset;
}

double dev_phys_y (void)
{
  return dev_page_height() + dvi_tell_mag()*dvi_dev_ypos() -voffset;
}

static int src_special (char *buffer, UNSIGNED_QUAD size) {
  char *start = buffer;
  char *end = buffer + size;
  int result = 0;
  skip_white (&start, end);
  if ((start+3 < end) &&
      (!strncmp ("src:", start, 4)))
    result = 1;
  return result;
}

void dev_do_special (void *buffer, UNSIGNED_QUAD size, spt_t x_user, 
		     spt_t y_user)
{
  double dev_xuser, dev_yuser;
  dev_xuser = ((double) x_user) / ((double) PDF_U);
  dev_yuser = ((double) -y_user) / ((double) PDF_U);
  graphics_mode();
  if (!pdf_parse_special (buffer, size, dev_xuser, dev_yuser) &&
      !tpic_parse_special (buffer, size, dev_xuser, dev_yuser) &&
      !htex_parse_special (buffer, size) &&
      !color_special (buffer, size) &&
      !ps_parse_special (buffer, size, dev_xuser, dev_yuser) &&
      !src_special (buffer, size)) {
    fprintf (stderr, "\nUnrecognized special ignored");
    dump (buffer, ((char *)buffer)+size);
  }
}

static unsigned dvi_stack_depth = 0;
static int dvi_tagged_depth = -1;
static unsigned char link_annot = 1;

void dev_link_annot (unsigned char flag)
{
  link_annot = flag;
}

void dev_stack_depth (unsigned int depth)
{
  /* If decreasing below tagged_depth */
  if (link_annot && 
      dvi_stack_depth == dvi_tagged_depth &&
      depth == dvi_tagged_depth - 1) {
  /* See if this appears to be the end of a "logical unit"
     that's been broken.  If so, flush the logical unit */
    pdf_doc_flush_annot();
  }
  dvi_stack_depth = depth;
  return;
}

/* The following routines setup and tear down a callback at
   a certain stack depth.  This is used to handle broken (linewise)
   links */

void dev_tag_depth (void)
{
  dvi_tagged_depth = dvi_stack_depth;
  dvi_compute_boxes (1);
  return;
}

void dev_untag_depth (void)
{
  dvi_tagged_depth = -1;
  dvi_compute_boxes (0);
  return;
}

void dev_expand_box (spt_t width, spt_t height, spt_t depth)
{
  double phys_width, phys_height, phys_depth, scale;
  if (link_annot && dvi_stack_depth >= dvi_tagged_depth) {
    scale = dvi2pts*dvi_tell_mag();
    phys_width = scale*width;
    phys_height = scale*height;
    phys_depth = scale*depth;
    pdf_doc_expand_box (dev_phys_x(), dev_phys_y()-phys_depth,
			dev_phys_x()+phys_width,
			dev_phys_y()+phys_height);
  }
}













