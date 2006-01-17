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

#include <stdio.h>
#include "system.h"
#include "mem.h"
#include "error.h"
#include "mfileio.h"
#include "numbers.h"
#include "pkfont.h"
#include "pdfobj.h"
#include "pdflimits.h"
#include "tfm.h"
#include "ctype.h"

pdf_obj *pk_encoding_ref = NULL;
static unsigned char verbose = 0, debug = 0;
static unsigned font_dpi = 600;

void pk_set_verbose(void)
{
  if (verbose < 255) {
    verbose += 1;
  }
}

void pk_set_dpi (int dpi)
{
  font_dpi = dpi;
}


static void make_pk_encoding_ref (void)
{
  static char first = 1;
  if (first) {
    int i;
    pdf_obj *encoding, *differences;
    encoding = pdf_new_dict();
    pdf_add_dict (encoding, 
		  pdf_new_name ("Type"),
		  pdf_new_name ("Encoding"));
    differences = pdf_new_array();
    pdf_add_dict (encoding,
		  pdf_new_name ("Differences"),
		  differences);
    pdf_add_array (differences, pdf_new_number (0.0));
    for (i=0; i<256; i++) {
      sprintf (work_buffer, "x%x", i);
      pdf_add_array (differences, pdf_new_name(work_buffer));
    }
    pk_encoding_ref = pdf_ref_obj (encoding);
    pdf_release_obj (encoding);
  }
  first = 0;
  return;
}

static void release_pk_encoding_ref (void)
{
  pdf_release_obj (pk_encoding_ref);
}

struct a_pk_font 
{
  pdf_obj *direct, *indirect;
  char *tex_name, *pk_file_name;
  double ptsize;
  char *used_chars;
} *pk_fonts;

void init_pk_record (struct a_pk_font *p)
{
  int i;
  p->used_chars = NEW (256, char);
  for (i=0; i<256; i++) {
    (p->used_chars)[i] = 0;
  }
  p->tex_name = NULL;
  p->pk_file_name = NULL;
}


int num_pk_fonts = 0, max_pk_fonts = 0;

int pk_font (const char *tex_name, double ptsize, int tfm_font_id, char
	     *res_name)
{
  int i, j;
  int firstchar, lastchar;
  pdf_obj *tmp1;
  for (i=0; i<num_pk_fonts; i++) {
    if (!strcmp (tex_name, pk_fonts[i].tex_name) &&
	(ptsize == pk_fonts[i].ptsize))
      break;
  }
  if (i == num_pk_fonts) {
    char *pk_file_name;
    kpse_glyph_file_type kpse_file_info;
    if ((pk_file_name = kpse_find_glyph(tex_name,
					font_dpi*ptsize/tfm_get_design_size(tfm_font_id)+0.5,
					kpse_pk_format,
					&kpse_file_info))) {
      /* Make sure there is enough room in pk_fonts for this entry */
      if (num_pk_fonts >= max_pk_fonts) {
	max_pk_fonts += MAX_FONTS;
	pk_fonts = RENEW (pk_fonts, max_pk_fonts, struct a_pk_font);
      }
      init_pk_record (pk_fonts+i);
      pk_fonts[i].pk_file_name = NEW (strlen(pk_file_name)+1, char);
      strcpy (pk_fonts[i].pk_file_name, pk_file_name);
      pk_fonts[i].tex_name = NEW (strlen (tex_name)+1, char);
      strcpy (pk_fonts[i].tex_name, tex_name);
      pk_fonts[i].ptsize = ptsize;
      pk_fonts[i].direct = pdf_new_dict ();
      pdf_add_dict (pk_fonts[i].direct,
		    pdf_new_name ("Type"),
		    pdf_new_name ("Font"));
      pdf_add_dict (pk_fonts[i].direct,
		    pdf_new_name ("Name"),
		    pdf_new_name (res_name));
      pdf_add_dict (pk_fonts[i].direct,
		    pdf_new_name ("Subtype"),
		    pdf_new_name ("Type3"));
      firstchar = tfm_get_firstchar(tfm_font_id);
      pdf_add_dict (pk_fonts[i].direct,
		    pdf_new_name ("FirstChar"),
		    pdf_new_number (firstchar));
      lastchar = tfm_get_lastchar(tfm_font_id);
      pdf_add_dict (pk_fonts[i].direct,
		    pdf_new_name ("LastChar"),
		    pdf_new_number (lastchar));
      tmp1 = pdf_new_array ();
      for (j=firstchar; j<=lastchar; j++) {
	pdf_add_array (tmp1,
		       pdf_new_number(ROUND(tfm_get_width (tfm_font_id, j)*1000.0,0.01)));
      }
      pdf_add_dict (pk_fonts[i].direct,
		    pdf_new_name ("Widths"),
		    tmp1);
      {
	double max_height, max_depth, max_width;
	max_height = tfm_get_max_height (tfm_font_id)*1000.0;
	max_depth = tfm_get_max_depth (tfm_font_id)*1000.0;
	max_width = tfm_get_max_width (tfm_font_id)*1000.0;
	max_height = ROUND(max_height, 1.0);
	max_depth = ROUND(max_depth, 1.0);
	max_width = ROUND(max_width, 1.0);
	tmp1 = pdf_new_array();
	pdf_add_array (tmp1, pdf_new_number(-0.10*max_width));
	pdf_add_array (tmp1,
		       pdf_new_number(-max_depth-0.20*(max_height+max_depth)));
	pdf_add_array (tmp1, pdf_new_number(1.10*max_width));
	pdf_add_array (tmp1, pdf_new_number(max_height+0.10*(max_height+max_depth)));
	pdf_add_dict (pk_fonts[i].direct, 
		      pdf_new_name ("FontBBox"), tmp1);
      }
      {
	tmp1 = pdf_new_array();
	pdf_add_array (tmp1, pdf_new_number (0.001));
	pdf_add_array (tmp1, pdf_new_number (0.0));
	pdf_add_array (tmp1, pdf_new_number (0.0));
	pdf_add_array (tmp1, pdf_new_number (0.001));
	pdf_add_array (tmp1, pdf_new_number (0.0));
	pdf_add_array (tmp1, pdf_new_number (0.0));
	pdf_add_dict (pk_fonts[i].direct, pdf_new_name ("FontMatrix"),
		      tmp1);
      }
      if (pk_encoding_ref == NULL) {
	make_pk_encoding_ref();
      }
      pdf_add_dict (pk_fonts[i].direct,
		    pdf_new_name ("Encoding"),
		    pdf_link_obj (pk_encoding_ref));
      pk_fonts[i].indirect = pdf_ref_obj (pk_fonts[i].direct);
      num_pk_fonts += 1;
    } else { /* Found no glyph file */
      i = -1;
    }
  }
  return i;
}

pdf_obj *pk_font_resource (int pk_id)
{
  if (pk_id <0 || pk_id >= num_pk_fonts) {
    fprintf (stderr, "pk_id = %d\n", pk_id);
    ERROR ("pk_font_resource:  Invalid pk_id\n");
  }
  return pdf_link_obj (pk_fonts[pk_id].indirect);
}

char *pk_font_used (int pk_id)
{
  if (pk_id < 0 || pk_id >= num_pk_fonts) {
    fprintf (stderr, "pk_id = %d\n", pk_id);
    ERROR ("pk_font_used:  Invalid pk_id\n");
  }
  return pk_fonts[pk_id].used_chars;
}


static FILE *pk_file;

static void do_skip(unsigned long length) 
{
  unsigned long i;
  for (i=0; i<length; i++) 
    fgetc (pk_file);
  return;
}

static void add_raster_data (pdf_obj *glyph, long w, long h,
			     int dyn_f, int run_color,
			     unsigned char *pk_data,
			     unsigned char *eod)

     /* First define some macros to be used as "in-line" functions */
#define advance_nybble() \
{ \
  if (partial_byte) { \
    partial_byte=0; \
    pk_data++; \
  } else \
    partial_byte=1; \
}

#define current_nybble() (partial_byte? (*pk_data%16): (*pk_data/16))
#define set_bits(n) {\
  int i; \
  for (i=0; i<(n); i++) { \
    row[next_col] |= (128u>>next_bit++); \
    if (next_bit > 7) { \
      next_bit = 0; \
      next_col += 1; \
    } \
  } \
}

#define skip_bits(n) {\
  next_col += (n)/8; \
  next_bit += (n)%8; \
  if (next_bit > 7) { \
    next_bit -= 8; \
    next_col += 1; \
  } \
}

#define get_bit(p,n) (p[(n)/8] & (128u>>((n)%8)))

{
  long i, w_bytes, repeat_count, run_count = 0;
  int partial_byte = 0;
  unsigned char *row;
  w_bytes = (w%8 == 0)? w/8: (w/8+1);
  row = NEW (w_bytes, unsigned char);
  /* Make sure we output "h" rows of data */
  if (dyn_f == 14) {
    for (i=0; i<h; i++) {
      int next_col = 0, next_bit = 0;
      int j;
      for (j=0; j<w_bytes; j++) row[j] = 0;
      for (j=0; j<w; j++) {
	if (get_bit(pk_data,i*w+j)) {
	  skip_bits(1);
	}
	else {
	  set_bits(1);
	}
      }
      pdf_add_stream (glyph, (char *) row, w_bytes);
    }
  } else
    for (i=0; i<h; i++) {
      int next_col = 0, next_bit = 0;
      int j, row_bits_left = w;
      /* Initialize row to all zeros */
      for (j=0; j<w_bytes; j++) row[j] = 0;
      repeat_count = 0;
      /* Fill any run left over from previous rows */
      if (run_count != 0) {
	int nbits;
	nbits = MIN (w, run_count);
	run_count -= nbits;
	switch (run_color) {
	case 1:  /* This is actually white ! */
	  set_bits(nbits);
	  break;
	case 0:
	  skip_bits(nbits);
	  break;
	}
	row_bits_left -= nbits;
      }
      /* Read nybbles until we have a full row */
      while (pk_data < eod && row_bits_left>0) {
	int com_nyb;
	long packed = 0;
	com_nyb = current_nybble();
	if (com_nyb == 15) {
	  repeat_count = 1;
	  advance_nybble();
	  continue;
	}
	if (com_nyb == 14) {
	  advance_nybble();
	}
	/* Get a packed number */
	{
	  int nyb;
	  nyb = current_nybble();
	  /* Test for single nybble case */
	  if (nyb > 0 && nyb <= dyn_f) {
	    packed = nyb;
	    advance_nybble();
	  }
	  if (nyb > dyn_f) {
	    advance_nybble();
	    packed = (nyb-dyn_f-1)*16+current_nybble()+dyn_f+1; 
	    advance_nybble();
	  }
	  if (nyb == 0) {
	    int nnybs = 1;
	    while (current_nybble() == 0) {
	      advance_nybble();
	      nnybs += 1;
	    }
	    packed = 0;
	    while (nnybs) {
	      packed = packed*16 + current_nybble();
	      advance_nybble();
	      nnybs -= 1;
	    }
	    packed += (13-dyn_f)*16-15+dyn_f;
	  }
	}
	if (com_nyb == 14) {
	  repeat_count = packed;
	  continue;
	}
	{
	  int nbits;
	  run_count = packed;    
	  run_color = !run_color;
	  nbits = MIN (row_bits_left, run_count);
	  run_count -= nbits;
	  row_bits_left -= nbits;
	  switch (run_color) {
	  case 1: 
	    set_bits(nbits);
	    break;
	  case 0:
	    skip_bits(nbits);
	    break;
	  }
	}
	continue;
      }
      pdf_add_stream (glyph, (char *) row, w_bytes);
      /* Duplicate the row "repeat_count" times */
      for (j=0; j<repeat_count; j++) {
	pdf_add_stream (glyph, (char *) row, w_bytes);
      }
      /* Skip repeat_count interations */
      i += repeat_count;
    }
  RELEASE (row);
}

static void do_preamble(void)
{
  /* Check for id byte */
  if (fgetc (pk_file) == 89) {
    /* Skip comment */
    do_skip (get_unsigned_byte (pk_file));
    /* Skip other header info.  It's normally used for verifying this
       is the file wethink it is */
    do_skip (16);
  } else {
    ERROR ("embed_pk_font: PK ID byte is incorrect.  Are you sure this is a PK file?");
  }
  return;
}

#define SHORT_FORM 1
#define MED_FORM 2
#define LONG_FORM 3
static void do_character (unsigned char flag, int pk_id, pdf_obj *char_procs)
{
  int format;
  unsigned long packet_length = 0, code = 0;
  /* Last three bits of flag determine packet size in a complex way */
  if ((flag & 4) == 0) {
    format = SHORT_FORM;
  } else if ((flag & 7) == 7) {
    format = LONG_FORM;
  } else {
    format = MED_FORM;
  }
  if (debug) {
    fprintf (stderr, "packet format: %d\n", format);
  }
  switch (format) {
  case SHORT_FORM:
    packet_length = (flag & 3) * 256u + get_unsigned_byte (pk_file);
    code = get_unsigned_byte (pk_file);
    break;
  case MED_FORM:
    packet_length = (flag & 3) * 65536ul + get_unsigned_pair(pk_file);
    code = get_unsigned_byte (pk_file);
    break;
  case LONG_FORM:
    packet_length = get_unsigned_quad (pk_file);
    code = get_unsigned_quad (pk_file);
    if (code > 255)
      ERROR ("Unable to handle long characters in PK files");
    break;
  }
  if (debug) {
    fprintf (stderr, "\npk_do_character: code=%lu, packet_length=%lu\n",
	     code, packet_length);
  }
  if ((pk_fonts[pk_id].used_chars)[code%256]) {
    int dyn_f;
    unsigned long tfm_width = 0;
    long dm=0, dx=0, dy=0, w=0, h=0;
    int hoff=0, voff=0;
    dyn_f = flag/16;
    switch (format) {
    case SHORT_FORM:
      tfm_width = get_unsigned_triple (pk_file);
      dm = get_unsigned_byte (pk_file);
      w = get_unsigned_byte (pk_file);
      h = get_unsigned_byte (pk_file);
      hoff = get_signed_byte (pk_file);
      voff = get_signed_byte (pk_file);
      packet_length -= 8;
      break;
    case MED_FORM:
      tfm_width = get_unsigned_triple (pk_file);
      dm = get_unsigned_pair (pk_file);
      w = get_unsigned_pair (pk_file);
      h = get_unsigned_pair (pk_file);
      hoff = get_signed_pair (pk_file);
      voff = get_signed_pair (pk_file);
      packet_length -= 13;
      break;
    case LONG_FORM:
      tfm_width = get_signed_quad (pk_file);
      dx = get_signed_quad (pk_file);
      dy = get_signed_quad (pk_file);
      w = get_signed_quad (pk_file);
      h = get_signed_quad (pk_file);
      hoff = get_signed_quad (pk_file);
      voff = get_signed_quad (pk_file);
      packet_length -= 28;
      break;
    }
    {
      pdf_obj *glyph;
      double char_width, llx, lly, urx, ury;
      double pix2charu;
      int len;
      pix2charu = 72000.0/((double) font_dpi)/pk_fonts[pk_id].ptsize;
      char_width = tfm_width / (double) (1<<20) * 1000.0;
      char_width = ROUND (char_width, 0.01);
      llx = -hoff*pix2charu;
      lly = (voff-h)*pix2charu;
      urx = (w-hoff)*pix2charu;
      ury = voff*pix2charu;
      glyph = pdf_new_stream(STREAM_COMPRESS);
      /* The following line is a "metric" for the PDF reader */
      sprintf (work_buffer, "%.2f %.2f %.2f %.2f %.2f %.2f d1",
	       char_width, 0.0,
	       llx, lly, urx, ury);
      len = strlen (work_buffer);
      pdf_add_stream (glyph, work_buffer, len);
      /* For now add an outline box for debugging purposes */
      /*      len = sprintf (work_buffer, " %.2f %.2f m %.2f %.2f l %.2f %.2f l %.2f %.2f l s",
		     llx, lly, urx, lly, urx, ury, llx, ury);
		     pdf_add_stream (glyph, work_buffer, len); */
      /* Scale and translate origin to lower left corner for raster data */
      sprintf (work_buffer, " q %.2f 0 0 %.2f %.2f %.2f cm",
	       w*pix2charu, h*pix2charu, llx, lly);
      len = strlen (work_buffer);
      pdf_add_stream (glyph, work_buffer, len);
      if (w != 0 && h != 0 && packet_length != 0) {
	unsigned char *pk_data;
	long read_len;
	sprintf (work_buffer, "\nBI\n/W %ld\n/H %ld\n/IM true\n/BPC 1\n/I true\n", w, h);
        len = strlen (work_buffer);
	pdf_add_stream (glyph, work_buffer, len);
	sprintf (work_buffer, "ID ");
        len = strlen (work_buffer);
	pdf_add_stream (glyph, work_buffer, len);
	pk_data = NEW (packet_length, unsigned char);
	if ((read_len=fread (pk_data, 1, packet_length, pk_file))!=
	    packet_length) {
	  fprintf (stderr, "packet length should be %ld, but only %ld bytes were read\n",
		   packet_length, read_len);
	  ERROR ("Error reading character packet from PK file\n");
	}
	add_raster_data (glyph, w, h, dyn_f, (flag&8)>>3, pk_data, pk_data+packet_length);
	RELEASE (pk_data);
	sprintf (work_buffer, "\nEI");
        len = strlen (work_buffer);
	pdf_add_stream (glyph, work_buffer, len);
      } /* Otherwise we embed an empty stream :-( */
      sprintf (work_buffer, "\nQ");
      len = strlen (work_buffer);
      pdf_add_stream (glyph, work_buffer, len);
      sprintf (work_buffer, "x%x", (int)code%256);
      pdf_add_dict (char_procs, pdf_new_name (work_buffer),
		    pdf_ref_obj (glyph));
      pdf_release_obj (glyph);
    }
  } else {
    if (debug)
      fprintf (stderr, "\npk_do_character: Skipping code=%ld, length=%ld\n", code,
	       packet_length);
    do_skip (packet_length);
  }
}

#define PK_XXX1 240
#define PK_XXX2 241
#define PK_XXX3 242
#define PK_XXX4 243
#define PK_YYY 244
#define PK_POST 245
#define PK_NO_OP 246
#define PK_PRE 247

static void embed_pk_font (int pk_id)
{
  if (verbose)
    fprintf (stderr, "(%s", pk_fonts[pk_id].pk_file_name);
  if ((pk_file = MFOPEN (pk_fonts[pk_id].pk_file_name,
			FOPEN_RBIN_MODE))) {
    int pk_command_byte;
    pdf_obj *char_procs;
    char_procs = pdf_new_dict();
    while ((pk_command_byte = fgetc(pk_file)) >= 0 &&
	   pk_command_byte != PK_POST) {
      if (pk_command_byte < 240) {
	do_character (pk_command_byte, pk_id, char_procs);
      } else  /* A command byte */
	switch (pk_command_byte) {
	case PK_NO_OP:
	  break;
	case PK_XXX1:
	  do_skip(get_unsigned_byte(pk_file));
	  break;
	case PK_XXX2:
	  do_skip(get_unsigned_pair(pk_file));
	  break;
	case PK_XXX3:
	  do_skip(get_unsigned_triple(pk_file));
	  break;
	case PK_XXX4:
	  do_skip(get_unsigned_quad(pk_file));
	  break;
	case PK_YYY:
	  do_skip(4);
	  break;
	case PK_PRE:
	  do_preamble();
	  break;
	}
    }
    MFCLOSE (pk_file);
    if (verbose) 
      fprintf (stderr, ")\n");
    pdf_add_dict (pk_fonts[pk_id].direct,
		  pdf_new_name ("CharProcs"),
		  pdf_ref_obj(char_procs));
    pdf_release_obj (char_procs);
  } else {
    ERROR ("embed_pk_font: Failed to open PK file");
  }
}

void pk_close_all (void)
{
  int i;
  if (pk_encoding_ref)
    pdf_release_obj (pk_encoding_ref);
  for (i=0; i<num_pk_fonts; i++) {
    embed_pk_font (i);
    pdf_release_obj (pk_fonts[i].direct);
    pdf_release_obj (pk_fonts[i].indirect);
    RELEASE (pk_fonts[i].tex_name);
    RELEASE (pk_fonts[i].pk_file_name);
    RELEASE (pk_fonts[i].used_chars);
  }
  if (pk_fonts)
    RELEASE (pk_fonts);
}

