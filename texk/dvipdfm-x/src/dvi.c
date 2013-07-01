/*  
    
    This is dvipdfmx, an eXtended version of dvipdfm by Mark A. Wicks.

    Copyright (C) 2002-2012 by Jin-Hwan Cho and Shunsaku Hirata,
    the dvipdfmx project team.
    
    Copyright (C) 1998, 1999 by Mark A. Wicks <mwicks@kettering.edu>

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "system.h"
#include "mem.h"
#include "error.h"
#include "mfileio.h"
#include "numbers.h"

#include "pdfdev.h"
#include "pdfdoc.h"
#include "pdfparse.h"
#include "pdfencrypt.h"

#include "fontmap.h"

#include "dvicodes.h"
#include "tfm.h"
#include "vf.h"
#include "subfont.h"

#include "spc_util.h"
#include "specials.h"

#include "dvi.h"

#define DVI_STACK_DEPTH_MAX  256u
#define TEX_FONTS_ALLOC_SIZE 16u
#define VF_NESTING_MAX       16u

/* UTF-32 over U+FFFF -> UTF-16 surrogate pair */
#define UTF32toUTF16HS(x)  (0xd800 + (((x-0x10000) >> 10) & 0x3ff))
#define UTF32toUTF16LS(x)  (0xdc00 + (  x                 & 0x3ff))

/* Interal Variables */
static FILE          *dvi_file  = NULL;

static unsigned long *page_loc  = NULL;
static UNSIGNED_PAIR  num_pages = 0;

static unsigned long  dvi_file_size = 0;

static struct dvi_header
{
  UNSIGNED_QUAD unit_num;
  UNSIGNED_QUAD unit_den;
  UNSIGNED_QUAD mag;
  UNSIGNED_QUAD media_width, media_height;
  UNSIGNED_PAIR stackdepth;
  char  comment[257];  
} dvi_info = {
  25400000 , /* num */
  473628672, /* den */
  1000,      /* mag */
  0, 0,      /* media width and height */
  0,         /* stackdepth */
  {'\0'}     /* comment */
};

static double dev_origin_x = 72.0, dev_origin_y = 770.0;

#define PHYSICAL 1
#define VIRTUAL  2
#define SUBFONT  3
#define DVI      1
#define VF       2

static struct loaded_font
{
  int    type;     /* Type is physical or virtual */
  int    font_id;  /* id returned by dev (for PHYSICAL fonts)
		    * or by vf module for (VIRTUAL fonts)
		    */
  int   subfont_id; /* id returned by subfont_locate_font() */
  int   tfm_id;
  spt_t size;
  int   source;     /* Source is either DVI or VF */
} *loaded_fonts = NULL;
static int num_loaded_fonts = 0, max_loaded_fonts = 0;

static void
need_more_fonts (unsigned n) 
{
  if (num_loaded_fonts + n > max_loaded_fonts) {
    max_loaded_fonts += TEX_FONTS_ALLOC_SIZE;
    loaded_fonts = RENEW (loaded_fonts, max_loaded_fonts, struct loaded_font);
  }
}

static struct font_def
{
  long   tex_id;
  spt_t  point_size;
  spt_t  design_size;
  char  *font_name;
  int    font_id;   /* index of _loaded_ font in loaded_fonts array */
  int    used;
} *def_fonts = NULL;

static int num_def_fonts = 0, max_def_fonts = 0;
static int compute_boxes = 0, link_annot    = 1;
static int verbose       = 0;

void
dvi_set_verbose (void)
{
  verbose++;
  subfont_set_verbose();
  tfm_set_verbose();
  vf_set_verbose ();
  spc_set_verbose();
}

unsigned
dvi_npages (void)
{
  return num_pages;
}

static const char invalid_signature[] =
"Something is wrong. Are you sure this is a DVI file?";

#define range_check_loc(loc) \
 if ((loc) > dvi_file_size) {\
   ERROR(invalid_signature); \
 }

static long
find_post (void)
{
  long  current;
  int   ch;

  /* First find end of file */  
  dvi_file_size = file_size(dvi_file);
  current       = dvi_file_size;
 
  /* Scan backwards through PADDING */  
  do {
    current--;
    seek_absolute(dvi_file, current);
  } while ((ch = fgetc(dvi_file)) == PADDING &&
	   current > 0);

  /* file_position now points to last non padding character or
   * beginning of file */
  if (dvi_file_size - current < 4 ||
      current == 0 || !(ch == DVI_ID || ch == DVIV_ID)) {
    MESG("DVI ID = %d\n", ch);
    ERROR(invalid_signature);
  } 

  /* Make sure post_post is really there */
  current = current - 5;
  seek_absolute(dvi_file, current);
  if ((ch = fgetc(dvi_file)) != POST_POST) {
    MESG("Found %d where post_post opcode should be\n", ch);
    ERROR(invalid_signature);
  }
  current = get_signed_quad(dvi_file);
  seek_absolute(dvi_file, current);
  if ((ch = fgetc(dvi_file)) != POST) {
    MESG("Found %d where post_post opcode should be\n", ch);
    ERROR(invalid_signature);
  }

  return current;
}

static void
get_page_info (long post_location)
{
  int  i;

  seek_absolute(dvi_file, post_location + 27);
  num_pages = get_unsigned_pair(dvi_file);
  if (num_pages == 0) {
    ERROR("Page count is 0!");
  }
  if (verbose > 2) {
    MESG("Page count:\t %4d\n", num_pages);
  }

  page_loc = NEW(num_pages, unsigned long);

  seek_absolute(dvi_file, post_location + 1);
  page_loc[num_pages-1] = get_unsigned_quad(dvi_file);
  range_check_loc(page_loc[num_pages-1] + 41);
  for (i = num_pages - 2; i >= 0; i--) {
    seek_absolute(dvi_file, page_loc[i+1] + 41);
    page_loc[i] = get_unsigned_quad(dvi_file);
    range_check_loc(page_loc[num_pages-1] + 41);
  }
}

/* Following are computed "constants" used for unit conversion */
static double dvi2pts = 1.52018, total_mag = 1.0;

double
dvi_tell_mag (void)
{
  return total_mag;
}

static void
do_scales (double mag)
{
  total_mag = (double) dvi_info.mag / 1000.0 * mag;
  dvi2pts   = (double) dvi_info.unit_num / (double) dvi_info.unit_den;
  dvi2pts  *= (72.0 / 254000.0);
}

static void
get_dvi_info (long post_location)
{
  seek_absolute(dvi_file, post_location + 5);

  dvi_info.unit_num = get_unsigned_quad(dvi_file);
  dvi_info.unit_den = get_unsigned_quad(dvi_file);
  dvi_info.mag      = get_unsigned_quad(dvi_file);

  dvi_info.media_height = get_unsigned_quad(dvi_file);
  dvi_info.media_width  = get_unsigned_quad(dvi_file);

  dvi_info.stackdepth   = get_unsigned_pair(dvi_file);

  if (dvi_info.stackdepth > DVI_STACK_DEPTH_MAX) {
    WARN("DVI need stack depth of %d,", dvi_info.stackdepth);
    WARN("but DVI_STACK_DEPTH_MAX is %d.", DVI_STACK_DEPTH_MAX);
    ERROR("Capacity exceeded.");
  }

  if (verbose > 2) {
    MESG("DVI File Info\n");
    MESG("Unit: %ld / %ld\n",    dvi_info.unit_num, dvi_info.unit_den);
    MESG("Magnification: %ld\n", dvi_info.mag);
    MESG("Media Height: %ld\n",  dvi_info.media_height);
    MESG("Media Width: %ld\n",   dvi_info.media_width);
    MESG("Stack Depth: %d\n",    dvi_info.stackdepth);
  }
}

const char *
dvi_comment (void)
{
  return dvi_info.comment;
}

static void
read_font_record (SIGNED_QUAD tex_id)
{
  UNSIGNED_BYTE dir_length, name_length;
  UNSIGNED_QUAD point_size, design_size;
  char         *directory, *font_name;

  if (num_def_fonts >= max_def_fonts) {
    max_def_fonts += TEX_FONTS_ALLOC_SIZE;
    def_fonts = RENEW (def_fonts, max_def_fonts, struct font_def);
  }
                get_unsigned_quad(dvi_file);
  point_size  = get_unsigned_quad(dvi_file);
  design_size = get_unsigned_quad(dvi_file);
  dir_length  = get_unsigned_byte(dvi_file);
  name_length = get_unsigned_byte(dvi_file);

  directory   = NEW(dir_length + 1, char);
  if (fread(directory, 1, dir_length, dvi_file) != dir_length) {
    ERROR(invalid_signature);
  }
  directory[dir_length] = '\0';
  RELEASE(directory); /* unused */

  font_name   = NEW(name_length + 1, char);
  if (fread(font_name, 1, name_length, dvi_file) != name_length) {
    ERROR(invalid_signature);
  }
  font_name[name_length] = '\0';
  def_fonts[num_def_fonts].tex_id      = tex_id;
  def_fonts[num_def_fonts].font_name   = font_name;
  def_fonts[num_def_fonts].point_size  = point_size;
  def_fonts[num_def_fonts].design_size = design_size;
  def_fonts[num_def_fonts].used        = 0;
  num_def_fonts++;

  return;
}

static void
get_dvi_fonts (long post_location)
{
  UNSIGNED_BYTE  code;
  SIGNED_QUAD    tex_id = 0;

  seek_absolute(dvi_file, post_location + 29);
  while ((code = get_unsigned_byte(dvi_file)) != POST_POST) {
    switch (code) {
    case FNT_DEF1:
      tex_id = get_unsigned_byte(dvi_file);
      break;
    case FNT_DEF2:
      tex_id = get_unsigned_pair(dvi_file);
      break;
    case FNT_DEF3:
      tex_id = get_unsigned_triple(dvi_file);
      break;
    case FNT_DEF4:
      tex_id = get_signed_quad(dvi_file);
      break;
    default:
      MESG("Unexpected op code: %3d\n", code);
      ERROR(invalid_signature);
    }
    read_font_record(tex_id);
  }
  if (verbose > 2) {
    unsigned  i;

    MESG("\n");
    MESG("DVI file font info\n");
    for (i = 0; i < num_def_fonts; i++) {
      MESG("TeX Font: %10s loaded at ID=%5ld, ",
	   def_fonts[i].font_name, def_fonts[i].tex_id);
      MESG("size=%5.2fpt (scaled %4.1f%%)",
	   def_fonts[i].point_size * dvi2pts,
	   100.0 * ((double) def_fonts[i].point_size / def_fonts[i].design_size));
      MESG("\n");
    }
  }
}

static void get_comment (void)
{
  UNSIGNED_BYTE length;

  seek_absolute(dvi_file, 14);
  length = get_unsigned_byte(dvi_file);
  if (fread(dvi_info.comment,
	    1, length, dvi_file) != length) {
    ERROR(invalid_signature);
  }
  dvi_info.comment[length] = '\0';
  if (verbose) {
    MESG("DVI Comment: %s\n", dvi_info.comment);
  }
}

/*
 * The section below this line deals with the actual processing of the
 * dvi file.
 *
 * The dvi file processor state is contained in the following variables:
 */

struct dvi_registers
{
  SIGNED_QUAD h, v, w, x, y, z, d;
};

static struct   dvi_registers dvi_state;
static struct   dvi_registers dvi_stack[DVI_STACK_DEPTH_MAX];
static unsigned dvi_stack_depth = 0 ;  
static int      current_font    = -1;
static int      processing_page = 0 ;

static void
clear_state (void)
{
  dvi_state.h = 0; dvi_state.v = 0; dvi_state.w = 0;
  dvi_state.x = 0; dvi_state.y = 0; dvi_state.z = 0;
  dvi_state.d = 0; /* direction */
  dvi_stack_depth = 0;
  current_font    = -1;
}

/* Migrated from pdfdev.c:
 * The following codes are originally put into pdfdev.c.
 * But they are moved to here to make PDF output independent
 * from DVI input.
 * pdfdoc, pdfspecial and htex are also modified. pdfspecial
 * and htex does tag/untag depth. pdfdev and pdfdoc now does
 * not care about line-breaking at all.
 */
static unsigned marked_depth =  0;
static int      tagged_depth = -1;

static void
dvi_mark_depth (void)
{
  /* If decreasing below tagged_depth */
  if (link_annot && 
      marked_depth    == tagged_depth &&
      dvi_stack_depth == tagged_depth - 1) {
  /*
   * See if this appears to be the end of a "logical unit"
   * that's been broken.  If so, flush the logical unit.
   */
    pdf_doc_break_annot();
  }
  marked_depth = dvi_stack_depth;
}

/*
 * The following routines setup and tear down a callback at a
 * certain stack depth. This is used to handle broken (linewise)
 * links.
 */
void
dvi_tag_depth (void)
{
  tagged_depth = marked_depth;
  dvi_compute_boxes(1);
}

void
dvi_untag_depth (void)
{
  tagged_depth = -1;
  dvi_compute_boxes(0);
}

void
dvi_compute_boxes (int flag)
{
  compute_boxes = flag;
}

void
dvi_link_annot (int flag)
{
  link_annot = flag;
}

void
dvi_do_special (const void *buffer, UNSIGNED_QUAD size)
{
  double x_user, y_user, mag;
  const char *p;

  if (size > 0x7fffffffUL) {
    WARN("Special more than %ul bytes???", size);
    return;
  }

  graphics_mode();

  p = (const char *) buffer;

  x_user =  dvi_state.h * dvi2pts;
  y_user = -dvi_state.v * dvi2pts;
  mag    =  dvi_tell_mag();

  if (spc_exec_special(p, (long) size, x_user, y_user, mag) < 0) {
    if (verbose) {
      dump(p, p + size);
    }
  }

  return;
}

#if 0
double
dvi_unit_size (void)
{
  return dvi2pts;
}
#endif


int
dvi_locate_font (const char *tfm_name, spt_t ptsize)
{
  int           cur_id = -1;
  const char   *name = tfm_name;
  int           subfont_id = -1, font_id; /* VF or device font ID */
  fontmap_rec  *mrec;

  if (verbose)
    MESG("<%s@%.2fpt", tfm_name, ptsize * dvi2pts);

  need_more_fonts(1);

  /* This routine needs to be recursive/reentrant. Load current high water
   * mark into an automatic variable.
   */
  cur_id = num_loaded_fonts++;

  mrec = pdf_lookup_fontmap_record(tfm_name);
  /* Load subfont mapping table */
  if (mrec && mrec->charmap.sfd_name && mrec->charmap.subfont_id) {
    subfont_id = sfd_load_record(mrec->charmap.sfd_name, mrec->charmap.subfont_id);
  }

  /* TFM must exist here. */
  loaded_fonts[cur_id].tfm_id     = tfm_open(tfm_name, 1);
  loaded_fonts[cur_id].subfont_id = subfont_id;
  loaded_fonts[cur_id].size       = ptsize;
  /* This will be reset later if it was really generated by the dvi file. */
  loaded_fonts[cur_id].source     = VF;

  /* The order of searching fonts is as follows:
   *
   * 1. If mrec is null, that is, there is no map entry matching
   *    with tfm_name, then search a virtual font matching with
   *    tfm_name at first. If no virtual font is found, search a
   *    PK font matching with tfm_name.
   *
   * 2. If mrec is non-null, search a physical scalable font.
   *
   * 3. Notice that every subfont gets non-null mrec. In this case,
   *    enc_name corresponding to mrec will be used instead of mrec.
   *    That is enc_name is NULL, search a virtual font for Omega (.ovf)
   *    matching with the base name of the subfont. If no virtual font
   *    for Omega is found, it is a fatal error because there is no PK font
   *    for Omega.
   */
  if (!mrec) {
    font_id = vf_locate_font(tfm_name, ptsize);
    if (font_id >= 0) {
      loaded_fonts[cur_id].type    = VIRTUAL;
      loaded_fonts[cur_id].font_id = font_id;
      if (verbose)
        MESG("(VF)>");
      return  cur_id;
    }
  }
#if  1
  /* Sorry, I don't understand this well... Please fix.
   * The purpose of this seems to be:
   *
   *   Map 8-bit char codes in subfont to 16-bit code with SFD mapping
   *   and map subfonts to single OVF font.
   *
   * But it apparently only does TFM -> OVF mapping but no character
   * code mapping. Please see dvi_set(), you can't have both font->type
   * VIRTUAL and font->subfont_id >= 0. Am I missing something?
   */
  else if (subfont_id >= 0 && mrec->map_name)
  {
    fontmap_rec  *mrec1 = pdf_lookup_fontmap_record(mrec->map_name);
    /* enc_name=NULL should be used only for 'built-in' encoding.
     * Please fix this!
     */
    if (mrec1 && !mrec1->enc_name) {
      font_id = vf_locate_font(mrec1->font_name, ptsize);
      if (font_id < 0)
        WARN("Could not locate Omega Virtual Font \"%s\" for \"%s\".",
              mrec1->font_name, tfm_name);
      else {
        loaded_fonts[cur_id].type    = VIRTUAL;
        loaded_fonts[cur_id].font_id = font_id;
        if (verbose)
          MESG("(OVF)>");
        return  cur_id;
      }
    }
  }
#endif  /* 1 */

  /* Failed to load a virtual font so we try to load a physical font. */

  /* If mrec->map_name is not NULL, font name identified in PDF output
   * is different than tfm_name, this can happen for subfonts grouped
   * into a single "intermediate" font foo@SFD@.
   * This is necessary for optimal output; to avoid unnecessary creation
   * of multiple instances of a same font, to avoid frequent font selection
   * and break of string_mode.
   */
  if (mrec && mrec->map_name) {
    name = mrec->map_name;
  } else {
    name = tfm_name;
  }

  /* We need ptsize for PK font creation. */
  font_id = pdf_dev_locate_font(name, ptsize);
  if (font_id < 0) {
    WARN("Could not locate a virtual/physical font for TFM \"%s\".", tfm_name);
    if (mrec && mrec->map_name) { /* has map_name */
      fontmap_rec  *mrec1 = pdf_lookup_fontmap_record(mrec->map_name);
      WARN(">> This font is mapped to an intermediate 16-bit font \"%s\" with SFD charmap=<%s,%s>,",
           mrec->map_name, mrec->charmap.sfd_name, mrec->charmap.subfont_id);
      if (!mrec1)
        WARN(">> but I couldn't find font mapping for \"%s\".", mrec->map_name);
      else {
        WARN(">> and then mapped to a physical font \"%s\" by fontmap.", mrec1->font_name);
        WARN(">> Please check if kpathsea library can find this font: %s", mrec1->font_name);
      }
    } else if (mrec && !mrec->map_name) {
      WARN(">> This font is mapped to a physical font \"%s\".", mrec->font_name);
      WARN(">> Please check if kpathsea library can find this font: %s", mrec->font_name);
    } else {
      WARN(">> There are no valid font mapping entry for this font.");
      WARN(">> Font file name \"%s\" was assumed but failed to locate that font.", tfm_name);
    }
    ERROR("Cannot proceed without .vf or \"physical\" font for PDF output...");
  }
  loaded_fonts[cur_id].type    = PHYSICAL;
  loaded_fonts[cur_id].font_id = font_id;

  if (verbose)
    MESG(">");

  return  cur_id;
}

double
dvi_dev_xpos (void)
{
  return dvi_state.h * dvi2pts;
}

double
dvi_dev_ypos (void)
{
  return -(dvi_state.v * dvi2pts);
}

static void do_moveto (SIGNED_QUAD x, SIGNED_QUAD y)
{
  dvi_state.h = x;
  dvi_state.v = y;
}

void dvi_right (SIGNED_QUAD x)
{
  switch (dvi_state.d) {
  case 0:
    dvi_state.h += x; break;
  case 1:
    dvi_state.v += x; break;
  case 3:
    dvi_state.v -= x; break;
  }
}

void dvi_down (SIGNED_QUAD y)
{
  switch (dvi_state.d) {
  case 0:
    dvi_state.v += y; break;
  case 1:
    dvi_state.h -= y; break;
  case 3:
    dvi_state.h += y; break;
  }
}

/* Please remove this.
 * Optimization for 8-bit encodings.
 */
static void
do_string (unsigned char *s, int len)
{
  struct loaded_font *font;
  spt_t  width, height, depth;
  int    i;

  if (current_font < 0)
    ERROR("No font selected!");

  font  = &loaded_fonts[current_font];

  width = tfm_string_width(font->tfm_id, s, len);
  width = sqxfw(font->size, width);

  switch (font->type) {
  case PHYSICAL:
    if (font->subfont_id < 0) {
      pdf_dev_set_string(dvi_state.h, -dvi_state.v, s, len,
                         width, font->font_id, 1);
      if (compute_boxes && link_annot &&
          marked_depth >= tagged_depth) {
        pdf_rect rect;

        height = tfm_string_height(font->tfm_id, s, len);
        depth  = tfm_string_depth (font->tfm_id, s, len);
        height = sqxfw(font->size, height);
        depth  = sqxfw(font->size, depth);

        pdf_dev_set_rect  (&rect, dvi_state.h, -dvi_state.v,
                            width, height, depth);
        pdf_doc_expand_box(&rect);
      }
    } else { /* Subfonts */
      dvi_push();
      for (i = 0; i < len; i++) {
        dvi_set(s[i]);
      }
      dvi_pop();
    }
    break;
  case VIRTUAL:
    dvi_push();
    for (i = 0; i < len; i++) {
      dvi_set(s[i]);
    }
    dvi_pop();
  }
  switch (dvi_state.d) {
  case 0:
    dvi_state.h += width; break;
  case 1:
    dvi_state.v += width; break;
  case 3:
    dvi_state.v -= width; break;
  }
}

/* _FIXME_
 * CMap decoder wants multibyte strings as input but
 * how DVI char codes are converted to multibyte sting
 * is not clear.
 */
void
dvi_set (SIGNED_QUAD ch)
{
  struct loaded_font *font;
  spt_t               width, height, depth;
  unsigned char       wbuf[4];

  if (current_font < 0) {
    ERROR("No font selected!");
  }
  /* The division by dvi2pts seems strange since we actually know the
   * "dvi" size of the fonts contained in the DVI file.  In other
   * words, we converted from DVI units to pts and back again!
   * The problem comes from fonts defined in VF files where we don't know
   * the DVI size.  It's keeping me sane to keep *point sizes* of *all*
   * fonts in the dev.c file and convert them back if necessary.
   */ 
  font  = &loaded_fonts[current_font];

  width = tfm_get_fw_width(font->tfm_id, ch);
  width = sqxfw(font->size, width);

  switch (font->type) {
  case  PHYSICAL:
    if (ch > 65535) { /* _FIXME_ */
      wbuf[0] = (UTF32toUTF16HS(ch) >> 8) & 0xff;
      wbuf[1] =  UTF32toUTF16HS(ch)       & 0xff;
      wbuf[2] = (UTF32toUTF16LS(ch) >> 8) & 0xff;
      wbuf[3] =  UTF32toUTF16LS(ch)       & 0xff;
      pdf_dev_set_string(dvi_state.h, -dvi_state.v, wbuf, 4,
			 width, font->font_id, 2);
    } else if (ch > 255) { /* _FIXME_ */
      wbuf[0] = (ch >> 8) & 0xff;
      wbuf[1] =  ch & 0xff;
      pdf_dev_set_string(dvi_state.h, -dvi_state.v, wbuf, 2,
			 width, font->font_id, 2);
    } else if (font->subfont_id >= 0) {
      unsigned short uch = lookup_sfd_record(font->subfont_id, (unsigned char) ch);
      wbuf[0] = (uch >> 8) & 0xff;
      wbuf[1] =  uch & 0xff;
      pdf_dev_set_string(dvi_state.h, -dvi_state.v, wbuf, 2,
			 width, font->font_id, 2);
    } else {
      wbuf[0] = (unsigned char) ch;
      pdf_dev_set_string(dvi_state.h, -dvi_state.v, wbuf, 1,
			 width, font->font_id, 1);
    }
    if (compute_boxes && link_annot &&
	marked_depth >= tagged_depth) {
      pdf_rect rect;

      height = tfm_get_fw_height(font->tfm_id, ch);
      depth  = tfm_get_fw_depth (font->tfm_id, ch);
      height = sqxfw(font->size, height);
      depth  = sqxfw(font->size, depth);

      pdf_dev_set_rect  (&rect, dvi_state.h, -dvi_state.v,
			 width, height, depth);
      pdf_doc_expand_box(&rect);
    }
    break;
  case  VIRTUAL:
#if  0
    /* See comment in locate_font() */
    if (font->subfont_id >= 0)
      ch = lookup_sfd_record(font->subfont_id, (unsigned char) ch);
#endif /* 0 */
    vf_set_char(ch, font->font_id); /* push/pop invoked */
    break;
  }
  switch (dvi_state.d) {
  case 0:
    dvi_state.h += width; break;
  case 1:
    dvi_state.v += width; break;
  case 3:
    dvi_state.v -= width; break;
  }
}

void
dvi_put (SIGNED_QUAD ch)
{
  struct loaded_font *font;
  spt_t               width, height, depth;
  unsigned char       wbuf[4];

  if (current_font < 0) {
    ERROR("No font selected!");
  }

  font = &loaded_fonts[current_font];

  switch (font->type) {
  case  PHYSICAL:
    width = tfm_get_fw_width(font->tfm_id, ch);
    width = sqxfw(font->size, width);

    /* Treat a single character as a one byte string and use the
     * string routine.
     */
    if (ch > 65535) { /* _FIXME_ */
      wbuf[0] = (UTF32toUTF16HS(ch) >> 8) & 0xff;
      wbuf[1] =  UTF32toUTF16HS(ch)       & 0xff;
      wbuf[2] = (UTF32toUTF16LS(ch) >> 8) & 0xff;
      wbuf[3] =  UTF32toUTF16LS(ch)       & 0xff;
      pdf_dev_set_string(dvi_state.h, -dvi_state.v, wbuf, 4,
			 width, font->font_id, 2);
    } else if (ch > 255) { /* _FIXME_ */
      wbuf[0] = (ch >> 8) & 0xff;
      wbuf[1] =  ch & 0xff;
      pdf_dev_set_string(dvi_state.h, -dvi_state.v, wbuf, 2,
			 width, font->font_id, 2);
    } else if (font->subfont_id >= 0) {
      unsigned int uch;

      uch = lookup_sfd_record(font->subfont_id, (unsigned char) ch);
      wbuf[0] = (uch >> 8) & 0xff;
      wbuf[1] =  uch & 0xff;
      pdf_dev_set_string(dvi_state.h, -dvi_state.v, wbuf, 2,
			 width, font->font_id, 2);
    } else {
      wbuf[0] = (unsigned char) ch;
      pdf_dev_set_string(dvi_state.h, -dvi_state.v, wbuf, 1,
			 width, font->font_id, 1);
    }
    if (compute_boxes && link_annot &&
	marked_depth >= tagged_depth) {
      pdf_rect rect;

      height = tfm_get_fw_height(font->tfm_id, ch);
      depth  = tfm_get_fw_depth (font->tfm_id, ch);
      height = sqxfw(font->size, height);
      depth  = sqxfw(font->size, depth);

      pdf_dev_set_rect  (&rect, dvi_state.h, -dvi_state.v,
			 width, height, depth);
      pdf_doc_expand_box(&rect);
    }
    break;
  case  VIRTUAL:    
#if  0
    /* See comment in locate_font() */
    if (font->subfont_id >= 0)
      ch = lookup_sfd_record(font->subfont_id, (unsigned char) ch);
#endif /* 0 */
    vf_set_char(ch, font->font_id);
    break;
  }

  return;
}


void
dvi_rule (SIGNED_QUAD width, SIGNED_QUAD height)
{
  do_moveto(dvi_state.h, dvi_state.v);

  switch (dvi_state.d) {
  case 0:
    pdf_dev_set_rule(dvi_state.h, -dvi_state.v,  width, height);
    break;
  case 1:
    pdf_dev_set_rule(dvi_state.h, -dvi_state.v - width, height, width);
    break;
  case 3: 
    pdf_dev_set_rule(dvi_state.h - height, -dvi_state.v , height, width);
    break;
  }
}

void
dvi_dir (UNSIGNED_BYTE dir) /* how? */
{
  fprintf(stderr, "  > dvi_dir %d\n", dir);
  dvi_state.d = dir;
  pdf_dev_set_dirmode(dvi_state.d); /* 0: horizontal, 1: vertical */
}

static void
do_set1 (void)
{
  dvi_set(get_unsigned_byte(dvi_file));
}

static void
do_set2 (void)
{
  dvi_set(get_unsigned_pair(dvi_file));
}

static void
do_set3 (void)
{
  dvi_set(get_unsigned_triple(dvi_file));
}

static void
do_setrule (void)
{
  SIGNED_QUAD  width, height;

  height = get_signed_quad(dvi_file);
  width  = get_signed_quad(dvi_file);
  if (width > 0 && height > 0) {
    dvi_rule(width, height);
  }
  dvi_right(width);
}

static void
do_putrule (void)
{
  SIGNED_QUAD width, height;

  height = get_signed_quad (dvi_file);
  width  = get_signed_quad (dvi_file);
  if (width > 0 && height > 0) {
    dvi_rule(width, height);
  }
}

static void
do_put1 (void)
{
  dvi_put(get_unsigned_byte(dvi_file));
}

static void
do_put2 (void)
{
  dvi_put(get_unsigned_pair(dvi_file));
}

static void
do_put3 (void)
{
  dvi_put(get_unsigned_triple(dvi_file));
}

void
dvi_push (void) 
{
  if (dvi_stack_depth >= DVI_STACK_DEPTH_MAX)
    ERROR("DVI stack exceeded limit.");

  dvi_stack[dvi_stack_depth++] = dvi_state;
}

void
dvi_pop (void)
{
  if (dvi_stack_depth <= 0)
    ERROR ("Tried to pop an empty stack.");

  dvi_state = dvi_stack[--dvi_stack_depth];
  do_moveto(dvi_state.h, dvi_state.v);
  pdf_dev_set_dirmode(dvi_state.d); /* 0: horizontal, 1: vertical */
}


static void
do_right1 (void)
{
  dvi_right(get_signed_byte(dvi_file));
}

static void
do_right2 (void)
{
  dvi_right(get_signed_pair(dvi_file));
}

static void
do_right3 (void)
{
  dvi_right(get_signed_triple(dvi_file));
}

static void
do_right4 (void)
{
  dvi_right(get_signed_quad(dvi_file));
}

void
dvi_w (SIGNED_QUAD ch)
{
  dvi_state.w = ch;
  dvi_right(ch);
}

void
dvi_w0 (void)
{
  dvi_right(dvi_state.w);
}

static void
do_w1 (void)
{
  dvi_w(get_signed_byte(dvi_file));
}

static void
do_w2 (void)
{
  dvi_w(get_signed_pair(dvi_file));
}

static void
do_w3 (void)
{
  dvi_w(get_signed_triple(dvi_file));
}

static void
do_w4 (void)
{
  dvi_w(get_signed_quad(dvi_file));
}

void
dvi_x (SIGNED_QUAD ch)
{
  dvi_state.x = ch;
  dvi_right(ch);
}

void
dvi_x0 (void)
{
  dvi_right(dvi_state.x);
}

static void
do_x1 (void)
{
  dvi_x(get_signed_byte(dvi_file));
}

static void
do_x2 (void)
{
  dvi_x(get_signed_pair(dvi_file));
}

static void
do_x3 (void)
{
  dvi_x(get_signed_triple(dvi_file));
}

static void
do_x4 (void)
{
  dvi_x(get_signed_quad(dvi_file));
}

static void
do_down1 (void)
{
  dvi_down(get_signed_byte(dvi_file));
}

static void
do_down2 (void)
{
  dvi_down(get_signed_pair(dvi_file));
}

static void
do_down3 (void)
{
  dvi_down(get_signed_triple(dvi_file));
}

static void
do_down4 (void)
{
  dvi_down(get_signed_quad(dvi_file));
}

void
dvi_y (SIGNED_QUAD ch)
{
  dvi_state.y = ch;
  dvi_down(ch);
}

void
dvi_y0 (void)
{
  dvi_down(dvi_state.y);
}

static
void do_y1 (void)
{
  dvi_y(get_signed_byte(dvi_file));
}

static
void do_y2 (void)
{
  dvi_y(get_signed_pair(dvi_file));
}

static
void do_y3 (void)
{
  dvi_y(get_signed_triple(dvi_file));
}

static
void do_y4 (void)
{
  dvi_y(get_signed_quad(dvi_file));
}

void
dvi_z (SIGNED_QUAD ch)
{
  dvi_state.z = ch;
  dvi_down(ch);
}

void
dvi_z0 (void)
{
  dvi_down(dvi_state.z);
}

static void
do_z1 (void)
{
  dvi_z(get_signed_byte(dvi_file));
}

static void
do_z2 (void)
{
  dvi_z(get_signed_pair(dvi_file));
}

static void
do_z3 (void)
{
  dvi_z(get_signed_triple(dvi_file));
}

static void
do_z4 (void)
{
  dvi_z(get_signed_quad(dvi_file));
}

static void
do_fntdef (void)
{
  int area_len, name_len, i;

  get_signed_quad(dvi_file);
  get_signed_quad(dvi_file);
  get_signed_quad(dvi_file);
  area_len = get_unsigned_byte(dvi_file);
  name_len = get_unsigned_byte(dvi_file);
  for (i = 0; i < area_len + name_len; i++) {
    get_unsigned_byte(dvi_file);
  }
}

static void
do_fntdef1 (void)
{
  get_unsigned_byte(dvi_file);
  do_fntdef();
}

static void
do_fntdef2 (void)
{
  get_unsigned_pair(dvi_file);
  do_fntdef();
}

static void
do_fntdef3 (void)
{
  get_unsigned_triple(dvi_file);
  do_fntdef();
}

static void
do_fntdef4 (void)
{
  get_signed_quad(dvi_file);
  do_fntdef();
}

void
dvi_set_font (int font_id)
{
  current_font = font_id;
}

static void
do_fnt (SIGNED_QUAD tex_id)
{
  int  i;

  for (i = 0; i < num_def_fonts; i++) {
    if (def_fonts[i].tex_id == tex_id)
      break;
  }

  if (i == num_def_fonts) {
    ERROR("Tried to select a font that hasn't been defined: id=%ld", tex_id);
  }

  if (!def_fonts[i].used) {
    int  font_id;

    font_id = dvi_locate_font(def_fonts[i].font_name, def_fonts[i].point_size);
    loaded_fonts[font_id].source = DVI;
    def_fonts[i].used    = 1;
    def_fonts[i].font_id = font_id;
  }
  current_font = def_fonts[i].font_id;
}

static void
do_fnt1 (void)
{
  SIGNED_QUAD tex_id;

  tex_id = get_unsigned_byte(dvi_file);
  do_fnt(tex_id);
}

static void
do_fnt2 (void)
{
  SIGNED_QUAD tex_id;

  tex_id = get_unsigned_pair(dvi_file);
  do_fnt(tex_id);
}

static void
do_fnt3 (void)
{
  SIGNED_QUAD tex_id;

  tex_id = get_unsigned_triple(dvi_file);
  do_fnt(tex_id);
}

static void
do_fnt4 (void)
{
  SIGNED_QUAD tex_id;

  tex_id = get_signed_quad(dvi_file);
  do_fnt(tex_id);
}

static void
do_xxx (UNSIGNED_QUAD size) 
{
  UNSIGNED_QUAD i;
  Ubyte  *buffer;

  buffer = NEW(size+1, Ubyte);
  for (i = 0; i < size; i++) {
    buffer[i] = get_unsigned_byte(dvi_file);
  }
  dvi_do_special(buffer, size);
  RELEASE(buffer);
}

static void
do_xxx1 (void)
{
  SIGNED_QUAD size;

  size = get_unsigned_byte(dvi_file);
  do_xxx(size);
}

static void
do_xxx2 (void)
{
  SIGNED_QUAD size;

  size = get_unsigned_pair(dvi_file);
  do_xxx(size);
}

static void
do_xxx3 (void)
{
  SIGNED_QUAD size;

  size = get_unsigned_triple(dvi_file);
  do_xxx(size);
}

static void
do_xxx4 (void)
{
  SIGNED_QUAD size;

  size = get_unsigned_quad(dvi_file);
  do_xxx(size);
}

static void
do_bop (void)
{
  int  i;

  if (processing_page) 
    ERROR("Got a bop in the middle of a page!");

  /* For now, ignore TeX's count registers */
  for (i = 0; i < 10; i++) {
    get_signed_quad(dvi_file);
  }
  /* Ignore previous page pointer since we have already
   * saved this information
   */
  get_signed_quad(dvi_file);
  clear_state();
  processing_page = 1;

  pdf_doc_begin_page(dvi_tell_mag(), dev_origin_x, dev_origin_y);
  spc_exec_at_begin_page();

  return;
}

static void
do_eop (void)
{
  processing_page = 0;

  if (dvi_stack_depth != 0) {
    ERROR("DVI stack depth is not zero at end of page");
  }
  spc_exec_at_end_page();

  pdf_doc_end_page();

  return;
}

static void
do_dir (void)
{
  dvi_state.d = get_unsigned_byte(dvi_file);
  pdf_dev_set_dirmode(dvi_state.d); /* 0: horizontal, 1: vertical */
}

/* Note to be absolutely certain that the string escape buffer doesn't
 * hit its limit, FORMAT_BUF_SIZE should set to 4 times S_BUFFER_SIZE
 * in pdfobj.c.  Is there any application that genenerate words with
 * 1k characters?
 */

#define SBUF_SIZE 1024

/* Most of the work of actually interpreting
 * the dvi file is here.
 */
void
dvi_do_page (unsigned n,
             double paper_width, double paper_height,
             double hmargin,     double vmargin)
{
  unsigned char opcode;
  unsigned char sbuf[SBUF_SIZE];
  unsigned int  slen = 0;

  if (!page_loc || n >= num_pages)
    ERROR("Tried to process non-existent page: %ld", n);

  /* DVI coordinate */
  dev_origin_x = hmargin;
  dev_origin_y = paper_height - vmargin;

  seek_absolute(dvi_file, page_loc[n]);

  dvi_stack_depth = 0;
  for (;;) {
    /* The most likely opcodes are individual setchars.
     * These are buffered for speed. */
    slen  = 0;
    while ((opcode = fgetc(dvi_file)) <= SET_CHAR_127 &&
            slen < SBUF_SIZE) {
      sbuf[slen++] = opcode;
    }
    if (slen > 0) {
      do_string(sbuf, slen);
    }
    if (slen == SBUF_SIZE)
      continue;

    /* If we are here, we have an opcode that is something
     * other than SET_CHAR.
     */
    if (opcode >= FNT_NUM_0 && opcode <= FNT_NUM_63) {
      do_fnt(opcode - FNT_NUM_0);
      continue;
    }

    switch (opcode) {
    case SET1: do_set1(); break;
    case SET2: do_set2(); break;
    case SET3: do_set3(); break;
    case SET4:
      ERROR("Multibyte (>24 bits) character not supported!");
      break;

    case SET_RULE:
      do_setrule();
      break;

    case PUT1: do_put1(); break;
    case PUT2: do_put2(); break;
    case PUT3: do_put3(); break;
    case PUT4:
      ERROR ("Multibyte character (>24 bits) not supported!");
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
      break;

    case PUSH:
      dvi_push();
      /* The following line needs to go here instead of in
       * dvi_push() since logical structure of document is
       * oblivous to virtual fonts. For example the last line on a
       * page could be at stack level 3 and the page footer should
       * be at stack level 3.  However, if the page footer contains
       * virtual fonts (or other nested constructions), it could
       * fool the link breaker into thinking it was a continuation
       * of the link */
      dvi_mark_depth();
      break;
    case POP:
      dvi_pop();
      /* Above explanation holds for following line too */
      dvi_mark_depth();
      break;

    case RIGHT1: do_right1(); break;
    case RIGHT2: do_right2(); break;
    case RIGHT3: do_right3(); break;
    case RIGHT4: do_right4(); break;

    case W0: dvi_w0(); break;
    case W1: do_w1 (); break;
    case W2: do_w2 (); break;
    case W3: do_w3 (); break;
    case W4: do_w4 (); break;

    case X0: dvi_x0(); break;
    case X1: do_x1 (); break;
    case X2: do_x2 (); break;
    case X3: do_x3 (); break;
    case X4: do_x4 (); break;

    case DOWN1: do_down1(); break;
    case DOWN2: do_down2(); break;
    case DOWN3: do_down3(); break;
    case DOWN4: do_down4(); break;

    case Y0: dvi_y0(); break;
    case Y1: do_y1 (); break;
    case Y2: do_y2 (); break;
    case Y3: do_y3 (); break;
    case Y4: do_y4 (); break;

    case Z0: dvi_z0(); break;
    case Z1: do_z1 (); break;
    case Z2: do_z2 (); break;
    case Z3: do_z3 (); break;
    case Z4: do_z4 (); break;

    case FNT1: do_fnt1(); break;
    case FNT2: do_fnt2(); break;
    case FNT3: do_fnt3(); break;
    case FNT4: do_fnt4(); break;

      /* Specials */
    case XXX1: do_xxx1(); break;
    case XXX2: do_xxx2(); break;
    case XXX3: do_xxx3(); break;
    case XXX4: do_xxx4(); break;

      /* Font definition - skipped. */
    case FNT_DEF1: do_fntdef1(); break;
    case FNT_DEF2: do_fntdef2(); break;
    case FNT_DEF3: do_fntdef3(); break;
    case FNT_DEF4: do_fntdef4(); break;

      /* pTeX extension */
    case PTEXDIR:
      do_dir();
      break;

    case PRE: case POST: case POST_POST:
      ERROR("Unexpected preamble or postamble in dvi file");
      break;
    default:
      ERROR("Unexpected opcode or DVI file ended prematurely");
    }
  }
}

double
dvi_init (const char *dvi_filename, double mag)
{
  long  post_location;

  dvi_file = MFOPEN(dvi_filename, FOPEN_RBIN_MODE);
  if (!dvi_file) {
    ERROR("Could not open specified DVI file: %s", dvi_filename);
    return 0.0;
  }

  /* DVI files are most easily read backwards by
   * searching for post_post and then post opcode.
   */
  post_location = find_post();
  get_dvi_info(post_location);
  do_scales(mag);
  get_page_info(post_location);
  get_comment();
  get_dvi_fonts(post_location);
  clear_state();

  return dvi2pts;
}

void
dvi_close (void)
{
  int   i;

  /* We add comment in dvi_close instead of dvi_init so user
   * has a change to overwrite it.  The docinfo dictionary is
   * treated as a write-once record.
   */

  /* Do some house cleaning */
  MFCLOSE(dvi_file);
  dvi_file = NULL;

  if (def_fonts) {
    for (i = 0; i < num_def_fonts; i++) {
      if (def_fonts[i].font_name)
        RELEASE(def_fonts[i].font_name);
      def_fonts[i].font_name = NULL;
    }
    RELEASE(def_fonts);
  }
  def_fonts = NULL;

  if (page_loc)
    RELEASE(page_loc);
  page_loc  = NULL;
  num_pages = 0;

  if (loaded_fonts)
    RELEASE(loaded_fonts);
  loaded_fonts     = NULL;
  num_loaded_fonts = 0;

  vf_close_all_fonts();
  tfm_close_all ();
}

/* The following are need to implement virtual fonts
   According to documentation, the vf "subroutine"
   must have state pushed and must have
   w,v,y, and z set to zero.  The current font
   is determined by the virtual font header, which
   may be undefined */

static int saved_dvi_font[VF_NESTING_MAX];
static int num_saved_fonts = 0;

void
dvi_vf_init (int dev_font_id)
{
  dvi_push();

  dvi_state.w = 0; dvi_state.x = 0;
  dvi_state.y = 0; dvi_state.z = 0;

  /* do not reset dvi_state.d. */
  if (num_saved_fonts < VF_NESTING_MAX) {
    saved_dvi_font[num_saved_fonts++] = current_font;
  } else
    ERROR("Virtual fonts nested too deeply!");
  current_font = dev_font_id;
}

/* After VF subroutine is finished, we simply pop the DVI stack */
void
dvi_vf_finish (void)
{
  dvi_pop();
  if (num_saved_fonts > 0) 
    current_font = saved_dvi_font[--num_saved_fonts];
  else {
    ERROR("Tried to pop an empty font stack");
  }
}


/* Scan various specials */
#include "dpxutil.h"

/* This need to allow 'true' prefix for unit and
 * length value must be divided by current magnification.
 */
static int
read_length (double *vp, double mag, const char **pp, const char *endptr)
{
  char   *q;
  const char *p = *pp;
  double  v, u = 1.0;
  const char *_ukeys[] = {
#define K_UNIT__PT  0
#define K_UNIT__IN  1
#define K_UNIT__CM  2
#define K_UNIT__MM  3
#define K_UNIT__BP  4
    "pt", "in", "cm", "mm", "bp",
     NULL
  };
  int     k, error = 0;

  q = parse_float_decimal(&p, endptr);
  if (!q) {
    *vp = 0.0; *pp = p;
    return  -1;
  }

  v = atof(q);
  RELEASE(q);

  q = parse_c_ident(&p, endptr);
  if (q) {
    char *qq = q;
    if (strlen(q) > strlen("true") &&
        !memcmp(q, "true", strlen("true"))) {
      u /= mag != 0.0 ? mag : 1.0; /* inverse magnify */
      q += strlen("true");
    }
    for (k = 0; _ukeys[k] && strcmp(_ukeys[k], q); k++);
    switch (k) {
    case K_UNIT__PT: u *= 72.0 / 72.27; break;
    case K_UNIT__IN: u *= 72.0; break;
    case K_UNIT__CM: u *= 72.0 / 2.54 ; break;
    case K_UNIT__MM: u *= 72.0 / 25.4 ; break;
    case K_UNIT__BP: u *= 1.0 ; break;
    default:
      WARN("Unknown unit of measure: %s", qq);
      error = -1;
      break;
    }
    RELEASE(qq);
  }

  *vp = v * u; *pp = p;
  return  error;
}


static int
scan_special (double *wd, double *ht, double *xo, double *yo, char *lm,
	      unsigned *minorversion,
	      int *do_enc, unsigned *key_bits, unsigned *permission, char *owner_pw, char *user_pw,
	      const char *buf, UNSIGNED_QUAD size)
{
  char  *q;
  const char *p = buf, *endptr;
  int    ns_pdf = 0, error = 0;

  endptr = p + size;

  skip_white(&p, endptr);

  q = parse_c_ident(&p, endptr);
  if (q && !strcmp(q, "pdf")) {
    skip_white(&p, endptr);
    if (p < endptr && *p == ':') {
      p++;
      skip_white(&p, endptr);
      RELEASE(q);
      q = parse_c_ident(&p, endptr); ns_pdf = 1;
    }
  }
  skip_white(&p, endptr);

  if (q) {
    if (!strcmp(q, "landscape")) {
      *lm = 1;
    } else if (ns_pdf && !strcmp(q, "pagesize")) {
      while (!error && p < endptr) {
        char  *kp = parse_c_ident(&p, endptr);
        if (!kp)
          break;
        else {
          skip_white(&p, endptr);
          if (!strcmp(kp, "width")) {
            error = read_length(wd, dvi_tell_mag(), &p, endptr);
            if (!error)
              *wd *= dvi_tell_mag();
          } else if (!strcmp(kp, "height")) {
            error = read_length(ht, dvi_tell_mag(), &p, endptr);
            if (!error)
              *ht *= dvi_tell_mag();
          } else if (!strcmp(kp, "xoffset")) {
            error = read_length(xo, dvi_tell_mag(), &p, endptr);
            if (!error)
              *xo *= dvi_tell_mag();
          } else if (!strcmp(kp, "yoffset")) {
            error = read_length(yo, dvi_tell_mag(), &p, endptr);
            if (!error)
              *yo *= dvi_tell_mag();
          }
          RELEASE(kp);
        }
        skip_white(&p, endptr);
      }
    } else if (!strcmp(q, "papersize")) {
      char  qchr = 0;
      if (*p == '=') p++;
      skip_white(&p, endptr);
      if (p < endptr && (*p == '\'' || *p == '\"')) {
        qchr = *p; p++;
        skip_white(&p, endptr);
      }
      error = read_length(wd, 1.0, &p, endptr);
      if (!error) {
        skip_white(&p, endptr);
        if (p < endptr && *p == ',') {
          p++; skip_white(&p, endptr);
        }
        error = read_length(ht, 1.0, &p, endptr);
        skip_white(&p, endptr);
      }
      if (!error && qchr) { /* Check if properly quoted */
        if (p >= endptr || *p != qchr)
          error = -1;
      }
    } else if (minorversion && ns_pdf && !strcmp(q, "minorversion")) {
      char *kv;
      if (*p == '=') p++;
      skip_white(&p, endptr);
      kv = parse_float_decimal(&p, endptr);
      if (kv) {
        *minorversion = (unsigned)strtol(kv, NULL, 10);
        RELEASE(kv);
      }
    } else if (ns_pdf && !strcmp(q, "encrypt") && do_enc) {
      *do_enc = 1;
      *owner_pw = *user_pw = 0;
      while (!error && p < endptr) {
        char  *kp = parse_c_ident(&p, endptr);
        if (!kp)
          break;
        else {
	  pdf_obj *obj;
          skip_white(&p, endptr);
          if (!strcmp(kp, "ownerpw")) {
            if ((obj = parse_pdf_string(&p, endptr))) {
	      strncpy(owner_pw, pdf_string_value(obj), MAX_PWD_LEN); 
	      pdf_release_obj(obj);
	    } else
	      error = -1;
          } else if (!strcmp(kp, "userpw")) {
            if ((obj = parse_pdf_string(&p, endptr))) {
	      strncpy(user_pw, pdf_string_value(obj), MAX_PWD_LEN);
	      pdf_release_obj(obj);
	    } else
	      error = -1;
          } else if (!strcmp(kp, "length")) {
            if ((obj = parse_pdf_number(&p, endptr)) && PDF_OBJ_NUMBERTYPE(obj)) {
	      *key_bits = (unsigned) pdf_number_value(obj);
	    } else
	      error = -1;
	    if (obj)
	      pdf_release_obj(obj);
          } else if (!strcmp(kp, "perm")) {
            if ((obj = parse_pdf_number(&p, endptr)) && PDF_OBJ_NUMBERTYPE(obj)) {
	      *permission = (unsigned) pdf_number_value(obj);
	    } else
	      error = -1;
	    if (obj)
	      pdf_release_obj(obj);
          } else
	    error = -1;
          RELEASE(kp);
        }
        skip_white(&p, endptr);
      }
    }
    RELEASE(q);
  }

  return  error;
}


void
dvi_scan_specials (unsigned page_no,
                   double *page_width, double *page_height,
                   double *x_offset, double *y_offset, char *landscape,
                   unsigned *minorversion,
		   int *do_enc, unsigned *key_bits, unsigned *permission, char *owner_pw, char *user_pw)
{
  FILE          *fp = dvi_file;
  long           offset;
  unsigned char  opcode;

  if (page_no >= num_pages)
    ERROR("Invalid page number: %u", page_no);
  offset = page_loc[page_no];

  seek_absolute(fp, offset);
  while ((opcode = fgetc(fp)) != EOP) {

    if (opcode <= SET_CHAR_127 ||
        (opcode >= FNT_NUM_0 && opcode <= FNT_NUM_63))
      continue;
    else if (opcode == XXX1 || opcode == XXX2 ||
             opcode == XXX3 || opcode == XXX4) {
      UNSIGNED_QUAD  size = 0UL;
      char  buf[1024];
      switch (opcode) {
      case XXX1: size = get_unsigned_byte(fp);   break;
      case XXX2: size = get_unsigned_pair(fp);   break;
      case XXX3: size = get_unsigned_triple(fp); break;
      case XXX4: size = get_unsigned_quad(fp);   break;
      }
      size = MIN(size, 1024);
      if (fread(buf, sizeof(char), size, fp) != size)
        ERROR("Reading DVI file failed!");
      if (scan_special(page_width, page_height, x_offset, y_offset, landscape,
		       minorversion, do_enc, key_bits, permission, owner_pw, user_pw, buf, size))
	WARN("Reading special command failed: \"%.*s\"", size, buf);
      continue;
    }

    /* Skipping... */
    switch (opcode) {
    case BOP:
      seek_relative(fp, 44);
      break;
    case NOP: case PUSH: case POP:
    case W0: case X0: case Y0: case Z0:
      break;
    case SET1: case PUT1: case RIGHT1:  case DOWN1:
    case W1: case X1: case Y1: case Z1: case FNT1:
      get_unsigned_byte(fp);
      break;

    case SET2: case PUT2: case RIGHT2: case DOWN2:
    case W2: case X2: case Y2: case Z2: case FNT2:
      get_signed_pair(fp);
      break;

    case SET3: case PUT3: case RIGHT3: case DOWN3:
    case W3: case X3: case Y3: case Z3: case FNT3:
      get_signed_triple(fp);
      break;

    case SET4: case PUT4: case RIGHT4: case DOWN4:
    case W4: case X4: case Y4: case Z4: case FNT4:
      get_signed_quad(fp);
      break;

    case SET_RULE: case PUT_RULE:
      seek_relative(fp, 8);
      break;

    case FNT_DEF1: do_fntdef1(); break;
    case FNT_DEF2: do_fntdef2(); break;
    case FNT_DEF3: do_fntdef3(); break;
    case FNT_DEF4: do_fntdef4(); break;

    case PTEXDIR:
      get_unsigned_byte(fp);
      break;

    default: /* case PRE: case POST: case POST_POST: and others */
      ERROR("Unexpected opcode %d at pos=0x%x", opcode, tell_position(fp));
      break;
    }
  }

  return;
}

