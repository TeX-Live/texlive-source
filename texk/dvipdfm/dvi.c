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

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "system.h"
#include "mem.h"
#include "error.h"
#include "mfileio.h"
#include "numbers.h"
#include "dvi.h"
#include "dvicodes.h"
#include "pdflimits.h"
#include "pdfdev.h"
#include "pdfdoc.h"
#include "tfm.h"
#include "vf.h"


/* Interal Variables */

static FILE *dvi_file;
static char debug = 0;
static unsigned num_loaded_fonts = 0, max_loaded_fonts = 0, stackdepth;
static unsigned long *page_loc = NULL;
static unsigned long post_location, dvi_file_size;
static UNSIGNED_PAIR num_pages = 0;
static UNSIGNED_QUAD media_width, media_height;
static UNSIGNED_QUAD dvi_unit_num, dvi_unit_den, dvi_mag;


#define PHYSICAL 1
#define VIRTUAL 2
#define DVI 1
#define VF 2

struct loaded_font {
  int type;  /* Type is physical or virtual */
  int font_id;  /* id returned by dev (for PHYSICAL fonts)
		   or by vf module for (VIRTUAL fonts) */
  int tfm_id;
  spt_t size;
  int source;  /* Source is either DVI or VF */
} *loaded_fonts = NULL;

static void need_more_fonts (unsigned n) 
{
  if (num_loaded_fonts+n > max_loaded_fonts) {
    max_loaded_fonts += MAX_FONTS;
    loaded_fonts = RENEW (loaded_fonts, max_loaded_fonts, struct
			  loaded_font);
  }
}

struct font_def
{
  spt_t size;
  char *name;
  signed long tex_id;
  int font_id; /* index of _loaded_ font in loaded_fonts array */
  int used;
} *def_fonts = NULL;

static unsigned num_def_fonts = 0, max_def_fonts = 0;

static unsigned char verbose = 0;

static unsigned char compute_boxes = 0;

void dvi_set_verbose(void)
{
  if (verbose < 255) {
  verbose += 1;
  }
}

void dvi_compute_boxes (unsigned char boxes)
{
  compute_boxes = boxes;
}

void dvi_set_debug(void)
{
  debug = 1;
}

unsigned dvi_npages (void)
{
  return num_pages;
}

static void invalid_signature()
{
  ERROR ("dvi_init:  Something is wrong.  Are you sure this is a DVI file?\n");
}

#define range_check_loc(loc) {if ((loc) > dvi_file_size) invalid_signature();}


static void find_post (void)
{
  long current;
  int read_byte;

  /* First find end of file */  
  dvi_file_size = file_size (dvi_file);
  current = dvi_file_size;
 
  /* Scan backwards through PADDING */  
  do {
     current -= 1;
     seek_absolute (dvi_file, current);

  } while ((read_byte = fgetc(dvi_file)) == PADDING &&
	   current > 0);

  /* file_position now points to last non padding character or beginning of file */
  if (dvi_file_size - current < 4 ||
      current == 0 || read_byte != DVI_ID) {
    fprintf (stderr, "DVI ID = %d\n", read_byte);
    invalid_signature();
  } 

  /* Make sure post_post is really there */
  current = current - 5;
  seek_absolute (dvi_file, current);
  if ((read_byte = fgetc(dvi_file)) != POST_POST) {
     fprintf (stderr, "Found %d where post_post opcode should be\n", read_byte);
     invalid_signature();
  }
  current = get_signed_quad (dvi_file);
  seek_absolute (dvi_file, current);
  if ((read_byte = fgetc(dvi_file)) != POST) {
     fprintf (stderr, "Found %d where post_post opcode should be\n", read_byte);
     invalid_signature();
  }
  post_location = current;
}

static void get_page_info (void) 
{
  int i;
  seek_absolute (dvi_file, post_location+27);
  num_pages = get_unsigned_pair (dvi_file);
  if (verbose > 2) {
    fprintf (stderr, "Page count:\t %4d\n", num_pages);
  }
  if (num_pages == 0) {
    ERROR ("dvi_init:  Page count is 0!");
  }
  page_loc = NEW (num_pages, unsigned long);
  seek_absolute (dvi_file, post_location+1);
  page_loc[num_pages-1] = get_unsigned_quad(dvi_file);
  range_check_loc(page_loc[num_pages-1]+41);
  for (i=num_pages-2; i>=0; i--) {
    seek_absolute (dvi_file, page_loc[i+1]+41);
    page_loc[i] = get_unsigned_quad(dvi_file);
    range_check_loc(page_loc[num_pages-1]+41);
  }
}

/* Following are computed "constants" used for unit conversion */
static double dvi2pts = 0.0, total_mag = 1.0;

double dvi_tell_mag (void)
{
  return total_mag;
}

static void do_scales (double mag)
{
  total_mag = (double) dvi_mag / 1000.0 * mag;
  dvi2pts = (double) dvi_unit_num / (double) dvi_unit_den;
  dvi2pts *= (72.0)/(254000.0);
}


static void get_dvi_info (void)
{
  seek_absolute (dvi_file, post_location+5);
  dvi_unit_num = get_unsigned_quad(dvi_file);
  dvi_unit_den = get_unsigned_quad(dvi_file);
  dvi_mag = get_unsigned_quad(dvi_file);
  media_height = get_unsigned_quad(dvi_file);
  media_width = get_unsigned_quad(dvi_file);
  stackdepth = get_unsigned_pair(dvi_file);
  if (stackdepth > DVI_MAX_STACK_DEPTH) {
    fprintf (stderr, "DVI needs stack depth of %d,", stackdepth);
    fprintf (stderr, "but MAX_DVI_STACK_DEPTH is %d", DVI_MAX_STACK_DEPTH);
    ERROR ("Capacity exceeded.");
  }

  if (verbose > 2) {
    fprintf (stderr, "DVI File Info\n");
    fprintf (stderr, "Unit: %ld / %ld\n", dvi_unit_num, dvi_unit_den);
    fprintf (stderr, "Mag: %ld\n", dvi_mag);
    fprintf (stderr, "Media Height: %ld\n", media_height);
    fprintf (stderr, "Media Width: %ld\n", media_width);
    fprintf (stderr, "Stack Depth: %d\n", stackdepth);
  }
  
}

static void dump_font_info (void)
{
  unsigned i;
  fprintf (stderr, "\nDVI file font info\n");
  for (i=0; i<num_def_fonts; i++) {
    fprintf (stderr, "name: %10s, ", def_fonts[i].name);
    fprintf (stderr, "TeX/DVI ID: %5ld, ", def_fonts[i].tex_id);
    fprintf (stderr, "size: %5.2f pt, ", def_fonts[i].size*dvi2pts);
    fprintf (stderr, "\n");
  }
}

static void get_a_font_record (SIGNED_QUAD tex_id)
{
  UNSIGNED_BYTE dir_length, name_length;
  UNSIGNED_QUAD checksum, size, design_size;
  char *directory, *name;
  if (debug) {
    fprintf (stderr, "get_a_font_record: tex_id = %ld\n", tex_id);
  }
  if (num_def_fonts >= max_def_fonts) {
    max_def_fonts += MAX_FONTS;
    def_fonts = RENEW (def_fonts, max_def_fonts, struct font_def);
  }
  checksum = get_unsigned_quad (dvi_file);
  size = get_unsigned_quad (dvi_file);
  design_size = get_unsigned_quad (dvi_file);
  dir_length = get_unsigned_byte (dvi_file);
  name_length = get_unsigned_byte (dvi_file);
  directory = NEW (dir_length+1, char);
  if (fread (directory, 1, dir_length, dvi_file) !=
      dir_length) {
    invalid_signature();
  }
  name = NEW (name_length+1, char);
  if (fread (name, 1, name_length, dvi_file) !=
      name_length) {
    invalid_signature();
  }
  directory[dir_length] = 0;
  RELEASE (directory);
  name[name_length] = 0;
  def_fonts[num_def_fonts].name = name;
  def_fonts[num_def_fonts].size = size;
  def_fonts[num_def_fonts].tex_id = tex_id;
  def_fonts[num_def_fonts].used = 0;
  num_def_fonts +=1;
  return;
}

static void get_dvi_fonts (void)
{
  UNSIGNED_BYTE code;
  SIGNED_QUAD tex_id;
  seek_absolute (dvi_file, post_location+29);
  while ((code = get_unsigned_byte(dvi_file)) != POST_POST) {
    switch (code)
      {
      case FNT_DEF1:
	tex_id = get_unsigned_byte (dvi_file);
	break;
      case FNT_DEF2:
	tex_id = get_unsigned_pair (dvi_file);
	break;
      case FNT_DEF3:
	tex_id = get_unsigned_triple (dvi_file);
	break;
      case FNT_DEF4:
	tex_id = get_signed_quad (dvi_file);
	break;
      default:
	fprintf (stderr, "Unexpected op code: %3d\n", code);
	invalid_signature();
      }
    get_a_font_record(tex_id);
  }
  if (verbose>2) {
    dump_font_info();
  }
}


void get_comment(void)
{
  UNSIGNED_BYTE length;
  static char dvi_comment[257];
  seek_absolute (dvi_file, 14);
  length = get_unsigned_byte(dvi_file);
  if (fread (dvi_comment, 1, length, dvi_file) != length) {
    invalid_signature();
  }
  dvi_comment[length] = 0;
  if (verbose) {
    fprintf (stderr, "DVI Comment: %s\n", dvi_comment);
  }
  dev_add_comment (dvi_comment);
}


/* The section below this line deals with the actual processing of the
   dvi file.

   The dvi file processor state is contained in the following
   variables: */

struct dvi_registers {
  SIGNED_QUAD h, v, w, x, y, z;
};

static struct dvi_registers dvi_state;
static struct dvi_registers dvi_stack[DVI_MAX_STACK_DEPTH];
static int current_font;
static unsigned dvi_stack_depth = 0;  
static int processing_page = 0;

static void clear_state (void)
{
  dvi_state.h = 0; dvi_state.v = 0; dvi_state.w = 0;
  dvi_state.x = 0; dvi_state.y = 0; dvi_state.z = 0;
  dvi_stack_depth = 0;
  current_font = -1;
}


double dvi_unit_size(void)
{
  return dvi2pts;
}

int dvi_locate_font (char *tex_name, spt_t ptsize)
{
  int thisfont;
  int font_id;
  if (debug) {
    fprintf (stderr, "dvi_locate_font: fontname: (%s) ptsize: %ld, dvi_id: %d\n",
	     tex_name, ptsize, num_loaded_fonts);
  }
  if (verbose)
    fprintf (stderr, "<%s@%.2fpt", tex_name, ptsize*dvi2pts);
  need_more_fonts (1);
  /* This routine needs to be recursive/reentrant.  Load current high water
     mark into an automatic variable  */
  thisfont = num_loaded_fonts++;
  loaded_fonts[thisfont].tfm_id = tfm_open (tex_name);
  loaded_fonts[thisfont].source = VF; /* This will be reset later if 
				     it was really generated by the
				     dvi file */
  font_id = vf_locate_font (tex_name, ptsize);
  loaded_fonts[thisfont].font_id = font_id;
  if (loaded_fonts[thisfont].font_id >= 0) {
    if (verbose)
      fprintf (stderr, "(VF)");
    loaded_fonts[thisfont].type = VIRTUAL;
  } else {
    loaded_fonts[thisfont].type = PHYSICAL;
    font_id = dev_locate_font (tex_name, ptsize);
    loaded_fonts[thisfont].font_id = font_id;
    if (loaded_fonts[thisfont].font_id < 0) {
      fprintf (stderr, "%s: Can't locate a Type 1, TTF, PK, or virtual font file\n", tex_name);
      ERROR ("Not sure how to proceed.  For now this is fatal\n\
Maybe in the future, I'll substitute some other font.");
    }
  }
  loaded_fonts[thisfont].size = ptsize;
  if (verbose)
    fprintf (stderr, ">");
  return (thisfont);
}

double dvi_dev_xpos(void) {
  return dvi_state.h*dvi2pts;
}

double dvi_dev_ypos (void)
{
  return -(dvi_state.v*dvi2pts);
}

static void do_moveto (SIGNED_QUAD x, SIGNED_QUAD y)
{
  dvi_state.h = x;
  dvi_state.v = y;
}

void dvi_right (SIGNED_QUAD x)
{
  dvi_state.h += x;
}

void dvi_down (SIGNED_QUAD y)
{
  dvi_state.v += y;
}

static void do_string (unsigned char *s, int len)
{
  spt_t width = 0, height = 0, depth = 0;
  int i;
  struct loaded_font *p;
  if (debug) {
    int i;
    fprintf (stderr, "do_string: (font: %d)\n", current_font);
    for (i=0; i<len; i++) fputc (s[i], stderr);
    fputc ('\n', stderr);
  }
  
  if (current_font < 0) {
    ERROR ("do_string:  No font selected");
  }
  p = loaded_fonts+current_font;
  width = tfm_string_width (p->tfm_id, s, len);
  width = sqxfw (p->size, width);
  switch (p->type) {
  case PHYSICAL:
    dev_set_string (dvi_state.h, -dvi_state.v, s, len,
		    width, p->font_id);
    if (compute_boxes) {
      height = tfm_string_height (p->tfm_id, s, len);
      depth = tfm_string_depth (p->tfm_id, s, len);
      height = sqxfw (p->size, height);
      depth = sqxfw (p->size, depth);
      dev_expand_box (width, height, depth);
    }
    break;
  case VIRTUAL:
    dvi_push();
    for (i=0; i<len; i++) {
      dvi_set (s[i]);
    }
    dvi_pop();
  }
  dvi_state.h += width;
}

void dvi_set (SIGNED_QUAD ch)
{
  spt_t width, height = 0, depth = 0;
  struct loaded_font *p;
  unsigned char lch;
  if (current_font < 0) {
    ERROR ("dvi_set:  No font selected");
  }
  /* The division by dvi2pts seems strange since we actually know the
     "dvi" size of the fonts contained in the DVI file.  In other
     words, we converted from DVI units to pts and back again!
     The problem comes from fonts defined in VF files where we don't know the DVI
     size.  It's keeping me sane to keep *point sizes* of *all* fonts in
     the dev.c file and convert them back if necessary */ 
  p = loaded_fonts+current_font;
  width = tfm_get_fw_width (p->tfm_id, ch);
  width = sqxfw (p->size, width);
  switch (p->type) {
  case PHYSICAL:
    if (ch > 255) {
      ERROR ("Tried to set a multibyte character in a non-virtual font");
    }
    lch = (unsigned char) ch;
    dev_set_string (dvi_state.h, -dvi_state.v, &lch, 1, width, p->font_id);
    if (compute_boxes) {
      height = tfm_get_fw_height (p->tfm_id, ch);
      depth = tfm_get_fw_depth (p->tfm_id, ch);
      height = sqxfw (p->size, height);
      depth = sqxfw (p->size, depth);
      dev_expand_box (width, height, depth);
    }
    break;
  case VIRTUAL:
    {
      vf_set_char (ch, p->font_id);
      break;
    }
  }
  dvi_state.h += width;
}

void dvi_put (SIGNED_QUAD ch)
{
  spt_t width, height = 0, depth = 0;
  struct loaded_font *p;
  unsigned char lch;
  if (current_font < 0) {
    ERROR ("dvi_put:  No font selected");
  }
  p = loaded_fonts+current_font;
  switch (p->type) {
  case PHYSICAL:
    width = tfm_get_fw_width (p->tfm_id, ch);
    width = sqxfw (p->size, width);
    /* Treat a single character as a one byte string and use the
       string routine.  The possibly multi-byte character must
       be converted to a single-byte string */
    if (ch > 255) {
      ERROR ("Tried to set a multibyte character in a non-virtual font");
    }
    lch = (unsigned char) ch;
    dev_set_string (dvi_state.h, -dvi_state.v, &lch, 1, width, p->font_id);
    if (compute_boxes) {
      height = tfm_get_fw_height (p->tfm_id, ch);
      depth = tfm_get_fw_depth (p->tfm_id, ch);
      height = sqxfw (p->size, height);
      depth = sqxfw (p->size, depth);
      dev_expand_box (width, height, depth);
    }
    break;
  case VIRTUAL:    
    vf_set_char (ch, p->font_id);
    break;
  }
  return;
}


void dvi_rule (SIGNED_QUAD width, SIGNED_QUAD height)
{
  do_moveto (dvi_state.h, dvi_state.v);
  dev_rule (dvi_state.h, -dvi_state.v,
	    width, height);
}

static void do_set1(void)
{
  dvi_set (get_unsigned_byte(dvi_file));
}

static void do_set2(void)
{
  dvi_set (get_unsigned_pair(dvi_file));
}

static void do_setrule(void)
{
  SIGNED_QUAD width, height;
  height = get_signed_quad (dvi_file);
  width = get_signed_quad (dvi_file);
  if (width > 0 && height > 0) {
    dvi_rule (width, height);
  }
  dvi_right (width);
}

static void do_putrule(void)
{
  SIGNED_QUAD width, height;
  height = get_signed_quad (dvi_file);
  width = get_signed_quad (dvi_file);
  if (width > 0 && height > 0) {
    dvi_rule (width, height);
  }
}

static void do_put1(void)
{
  dvi_put (get_unsigned_byte(dvi_file));
}

static void do_put2(void)
{
  dvi_put (get_unsigned_pair(dvi_file));
}

void dvi_push (void) 
{
  if (debug) {
    fprintf (stderr, "Pushing onto stack of depth %d\n",
	     dvi_stack_depth);
  }
  if (dvi_stack_depth < DVI_MAX_STACK_DEPTH) {
    dvi_stack[dvi_stack_depth++] = dvi_state;
  }
  else
    ERROR ("DVI stack exceeded");
}

void dvi_pop (void)
{
  if (debug) {
    fprintf (stderr, "Popping off stack of depth %d\n",
	     dvi_stack_depth);
  }
  if (dvi_stack_depth > 0) {
    dvi_state = dvi_stack[--dvi_stack_depth];
  } else
    ERROR ("dvi_pop: Tried to pop an empty stack");
  do_moveto (dvi_state.h, dvi_state.v);
}


static void do_right1(void)
{
  dvi_right (get_signed_byte(dvi_file));
}

static void do_right2(void)
{
  dvi_right (get_signed_pair(dvi_file));
}

static void do_right3(void)
{
  dvi_right (get_signed_triple(dvi_file));
}

static void do_right4(void)
{
  dvi_right (get_signed_quad(dvi_file));
}

void dvi_w (SIGNED_QUAD ch)
{
  dvi_state.w = ch;
  dvi_right (ch);
}

void dvi_w0(void)
{
  dvi_right (dvi_state.w);
}

static void do_w1(void)
{
  dvi_w (get_signed_byte(dvi_file));
}

static void do_w2(void)
{
  dvi_w (get_signed_pair(dvi_file));
}

static void do_w3(void)
{
  dvi_w (get_signed_triple(dvi_file));
}

static void do_w4(void)
{
  dvi_w (get_signed_quad(dvi_file));
}

void dvi_x (SIGNED_QUAD ch)
{
  dvi_state.x = ch;
  dvi_right (ch);
}

void dvi_x0(void)
{
  dvi_right (dvi_state.x);
}

static void do_x1(void)
{
  dvi_x (get_signed_byte(dvi_file));
}

static void do_x2(void)
{
  dvi_x (get_signed_pair(dvi_file));
}

static void do_x3(void)
{
  dvi_x (get_signed_triple(dvi_file));
}

static void do_x4(void)
{
  dvi_x (get_signed_quad(dvi_file));
}

static void do_down1(void)
{
  dvi_down (get_signed_byte(dvi_file));
}

static void do_down2(void)
{
  dvi_down (get_signed_pair(dvi_file));
}

static void do_down3(void)
{
  dvi_down (get_signed_triple(dvi_file));
}

static void do_down4(void)
{
  dvi_down (get_signed_quad(dvi_file));
}

void dvi_y (SIGNED_QUAD ch)
{
  dvi_state.y = ch;
  dvi_down (ch);
}

void dvi_y0(void)
{
  dvi_down (dvi_state.y);
}

static void do_y1(void)
{
  dvi_y (get_signed_byte(dvi_file));
}

static void do_y2(void)
{
  dvi_y (get_signed_pair(dvi_file));
}

static void do_y3(void)
{
  dvi_y (get_signed_triple(dvi_file));
}

static void do_y4(void)
{
  dvi_y (get_signed_quad(dvi_file));
}

void dvi_z (SIGNED_QUAD ch)
{
  dvi_state.z = ch;
  dvi_down (ch);
}

void dvi_z0(void)
{
  dvi_down (dvi_state.z);
}

static void do_z1(void)
{
  dvi_z (get_signed_byte(dvi_file));
}

static void do_z2(void)
{
  dvi_z (get_signed_pair(dvi_file));
}

static void do_z3(void)
{
  dvi_z (get_signed_triple(dvi_file));
}

static void do_z4(void)
{
  dvi_z (get_signed_quad(dvi_file));
}

static void do_fntdef(void)
{
  int area_len, name_len, i;
  get_signed_quad(dvi_file);
  get_signed_quad(dvi_file);
  get_signed_quad(dvi_file);
  area_len = get_unsigned_byte(dvi_file);
  name_len = get_unsigned_byte(dvi_file);
  for (i=0; i<area_len+name_len; i++) {
    get_unsigned_byte (dvi_file);
  }
}

static void do_fntdef1(void)
{
  get_unsigned_byte(dvi_file);
  do_fntdef();
}

static void do_fntdef2(void)
{
  get_unsigned_pair(dvi_file);
  do_fntdef();
}

static void do_fntdef3(void)
{
  get_unsigned_triple(dvi_file);
  do_fntdef();
}

static void do_fntdef4(void)
{
  get_signed_quad(dvi_file);
  do_fntdef();
}


void dvi_set_font (int font_id)
{
  current_font = font_id;
}

static void do_fnt (SIGNED_QUAD tex_id)
{
  int i;
  for (i=0; i<num_def_fonts; i++) {
    if (def_fonts[i].tex_id == tex_id)
      break;
  }
  if (i == num_def_fonts) {
    fprintf (stderr, "fontid: %ld\n", tex_id);
    ERROR ("dvi_do_fnt:  Tried to select a font that hasn't been defined");
  }
  if (!def_fonts[i].used) {
    int font_id;
    font_id = dvi_locate_font (def_fonts[i].name, def_fonts[i].size);
    loaded_fonts[font_id].source = DVI;
    def_fonts[i].used = 1;
    def_fonts[i].font_id = font_id;
  }
  current_font = def_fonts[i].font_id;
}

static void do_fnt1(void)
{
  SIGNED_QUAD font;
  font = get_unsigned_byte(dvi_file);
  do_fnt(font);
}

static void do_fnt2(void)
{
  SIGNED_QUAD font;
  font = get_unsigned_pair(dvi_file);
  do_fnt(font);
}

static void do_fnt3(void)
{
  SIGNED_QUAD font;
  font = get_unsigned_triple(dvi_file);
  do_fnt(font);
}

static void do_fnt4(void)
{
  SIGNED_QUAD font;
  font = get_signed_quad(dvi_file);
  do_fnt(font);
}

static void do_xxx(UNSIGNED_QUAD size) 
{
  UNSIGNED_QUAD i;
  Ubyte *buffer;
  buffer = NEW (size+1, Ubyte);
  for (i=0; i<size; i++) {
    buffer[i] = get_unsigned_byte(dvi_file);
  }
  if (debug)
    fprintf (stderr, "Special: %s\n", buffer);
  dev_do_special (buffer, size, dvi_state.h, dvi_state.v);
  RELEASE (buffer);
}

static void do_xxx1(void)
{
  SIGNED_QUAD size;
  if (debug)
    fprintf (stderr, "(xxx1)");
  size = get_unsigned_byte(dvi_file);
  do_xxx(size);
}

static void do_xxx2(void)
{
  SIGNED_QUAD size;
  size = get_unsigned_pair(dvi_file);
  do_xxx(size);
}

static void do_xxx3(void)
{
  SIGNED_QUAD size;
  size = get_unsigned_triple(dvi_file);
  do_xxx(size);
}

static void do_xxx4(void)
{
  SIGNED_QUAD size;
  size = get_unsigned_quad(dvi_file);
  do_xxx(size);
}

static void do_bop(void)
{
  int i;
  if (processing_page) 
    ERROR ("dvi_do_bop:  Got a bop inthe middle of a page");
  /* For now, ignore TeX's count registers */
  for (i=0; i<10; i++) {
    get_signed_quad (dvi_file);
  }
/*  Ignore previous page pointer since we have already saved this
    information */
  get_signed_quad (dvi_file);
  clear_state();
  processing_page = 1;
  dev_bop();
}

static void do_eop(void)
{
  processing_page = 0;
  if (dvi_stack_depth != 0) {
    ERROR ("do_eop:  stack_depth is not zero at end of page");
  }
  dev_eop();
}
/* Note to be absolutely certain that the string escape buffer doesn't
   hit its limit, FORMAT_BUF_SIZE should set to 4 times S_BUFFER_SIZE
   in pdfobj.c.  Is there any application that genenerate words with
   1k characters? */

#define S_BUFFER_SIZE 1024
static unsigned char s_buffer[S_BUFFER_SIZE];
static unsigned s_len = 0;

void dvi_do_page(unsigned n)  /* Most of the work of actually interpreting
			    the dvi file is here. */
{
  unsigned char opcode;
  /* Position to beginning of page */
  if (debug) fprintf (stderr, "Seeking to page %d @ %ld\n", n,
			  page_loc[n]);
  seek_absolute (dvi_file, page_loc[n]);
  dvi_stack_depth = 0;
  for (;;) {
    /* The most likely opcodes are individual setchars.  These are
       buffered for speed */
    s_len = 0;
    while ((opcode = fgetc (dvi_file)) <= SET_CHAR_127 &&
	   s_len < S_BUFFER_SIZE) {
      s_buffer[s_len++] = opcode;
    }
    if (s_len > 0) {
      do_string (s_buffer, s_len);
    }
    if (s_len == S_BUFFER_SIZE)
      continue;
    /* If we are here, we have an opcode that is something
       other than SET_CHAR */
    if (opcode >= FNT_NUM_0 && opcode <= FNT_NUM_63) {
      do_fnt (opcode - FNT_NUM_0);
      continue;
    }
    switch (opcode)
      {
      case SET1:
	do_set1();
	break;
      case SET2:
	do_set2();
	break;
      case SET3:
      case SET4:
	ERROR ("Multibyte (>16 bits) character in DVI file.  I can't handle this!");
	break;
      case SET_RULE:
	do_setrule();
	break;
      case PUT1:
	do_put1();
	break;
      case PUT2:
	do_put2();
	break;
      case PUT3:
      case PUT4:
	ERROR ("Multibyte character (>16 bits) in DVI file.  I can't handle this!");
	break;
      case PUT_RULE:
	do_putrule();
	break;
      case NOP:
	break;
      case BOP:
	do_bop();
	break;
      case EOP:
	do_eop();
	return;
      case PUSH:
	dvi_push();
	/* The following line needs to go here instead of in
	   dvi_push() since logical structure of document is
	   oblivous to virtual fonts. For example the last line on a
	   page could be at stack level 3 and the page footer should
	   be at stack level 3.  However, if the page footer contains
	   virtual fonts (or other nested constructions), it could
	   fool the link breaker into thinking it was a continuation
	   of the link */
	dev_stack_depth (dvi_stack_depth);
	break;
      case POP:
	dvi_pop();
	/* Above explanation holds for following line too */
	dev_stack_depth (dvi_stack_depth);
	break;
      case RIGHT1:
	do_right1();
	break;
      case RIGHT2:
	do_right2();
	break;
      case RIGHT3:
	do_right3();
	break;
      case RIGHT4:
	do_right4();
	break;
      case W0:
	dvi_w0();
	break;
      case W1:
	do_w1();
	break;
      case W2:
	do_w2();
	break;
      case W3:
	do_w3();
	break;
      case W4:
	do_w4();
	break;
      case X0:
	dvi_x0();
	break;
      case X1:
	do_x1();
	break;
      case X2:
	do_x2();
	break;
      case X3:
	do_x3();
	break;
      case X4:
	do_x4();
	break;
      case DOWN1:
	do_down1();
	break;
      case DOWN2:
	do_down2();
	break;
      case DOWN3:
	do_down3();
	break;
      case DOWN4:
	do_down4();
	break;
      case Y0:
	dvi_y0();
	break;
      case Y1:
	do_y1();
	break;
      case Y2:
	do_y2();
	break;
      case Y3:
	do_y3();
	break;
      case Y4:
	do_y4();
	break;
      case Z0:
	dvi_z0();
	break;
      case Z1:
	do_z1();
	break;
      case Z2:
	do_z2();
	break;
      case Z3:
	do_z3();
	break;
      case Z4:
	do_z4();
	break;
      case FNT1:
	do_fnt1();
	break;
      case FNT2:
	do_fnt2();
	break;
      case FNT3:
	do_fnt3();
	break;
      case FNT4:
	do_fnt4();
	break;
      case XXX1:
	do_xxx1();
	break;
      case XXX2:
	do_xxx2();
	break;
      case XXX3:
	do_xxx3();
	break;
      case XXX4:
	do_xxx4();
	break;
      case FNT_DEF1:
	do_fntdef1();
	break;
      case FNT_DEF2:
	do_fntdef2();
	break;
      case FNT_DEF3:
	do_fntdef3();
	break;
      case FNT_DEF4:
	do_fntdef4();
	break;
      case PRE:
      case POST:
      case POST_POST:
	ERROR("Unexpected preamble or postamble in dvi file");
	break;
      default:
	ERROR("Unexpected opcode or DVI file ended prematurely");
      }
  }
}

error_t dvi_init (char *dvi_filename, char *pdf_filename, double mag, double x_offset, double
		  y_offset)
{
  if (!(dvi_file = MFOPEN (dvi_filename, FOPEN_RBIN_MODE))) {
    ERROR ("dvi_init:  Specified DVI file doesn't exist");
    return (FATAL_ERROR);
  }
  /* DVI files are most easily read backwards by searching
     for post_post and then post opcode */
  find_post ();
  get_dvi_info();
  do_scales(mag);
  dev_init(dvi2pts, x_offset, y_offset);
  get_page_info();
  pdf_doc_init (pdf_filename);
  get_comment();
  get_dvi_fonts();
  clear_state();
  return (NO_ERROR);
}

void dvi_close (void)
{
  int i;
  /* We add comment in dvi_close instead of dvi_init so user has
     a change to overwrite it.  The docinfo dictionary is
     treated as a write-once record */

  /* Do some house cleaning */
  MFCLOSE (dvi_file);
  for (i=0; i<num_def_fonts; i++) {
    RELEASE (def_fonts[i].name);
  }
  if (def_fonts)
    RELEASE (def_fonts);
  RELEASE (page_loc);
  num_loaded_fonts = 0;
  if (loaded_fonts)
    RELEASE (loaded_fonts);
  num_pages = 0;
  dvi_file = NULL;
  dev_close_all_fonts();
  vf_close_all_fonts();
  tfm_close_all();
  dev_close();
  pdf_doc_close();
}

/* The following are need to implement virtual fonts
   According to documentation, the vf "subroutine"
   must have state pushed and must have
   w,v,y, and z set to zero.  The current font
   is determined by the virtual font header, which
   may be undefined */

static int saved_dvi_font[MAX_VF_NESTING];
static int num_saved_fonts = 0;

void dvi_vf_init (int dev_font_id)
{
  dvi_push ();
  dvi_state.w = 0; dvi_state.x = 0;
  dvi_state.y = 0; dvi_state.z = 0;
  if (num_saved_fonts < MAX_VF_NESTING) {
    saved_dvi_font[num_saved_fonts++] = current_font;
  } else
    ERROR ("Virtual fonts nested too deeply!");
  current_font = dev_font_id;
}

/* After VF subroutine is finished, we simply pop the DVI stack */
void dvi_vf_finish (void)
{
  dvi_pop();
  if (num_saved_fonts > 0) 
    current_font = saved_dvi_font[--num_saved_fonts];
  else
    ERROR ("Tried to pop an empty font stack");
}
