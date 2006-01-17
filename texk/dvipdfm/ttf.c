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

/* This is tailored for PDF */

#include "config.h"

#ifdef HAVE_TTF_FORMATS
#include "ttf.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "system.h"
#include "mem.h"
#include "error.h"
#include "mfileio.h"
#include "pdfobj.h"
#include "numbers.h"
#include "tfm.h"
#include "pdflimits.h"
#include "twiddle.h"
#include "encodings.h"

static unsigned char verbose = 0;

void ttf_set_verbose(void)
{
  if (verbose < 255) {
    verbose += 1;
  }
}

static char partial_enabled = 0;

void ttf_disable_partial (void)
{
  partial_enabled = 0;
}


/* Convert ttf "fixed" type to double */

#define fixed(a) ((double)((a)%0x10000L)/(double)(0x10000L)+ \
 (((a)/0x10000L) > 0x8000L? 0x10000L - ((a)/0x10000L): ((a)/0x10000L)))

/* Convert four-byte number to big endianess in a machine independent
   way */
static void convert_tag (char *tag, unsigned long u_tag)
{
  int i;
  for (i=3; i>= 0; i--) {
    tag[i] = u_tag % 256;
    u_tag /= 256;
  }
  tag[4] = 0;
}

struct table_header
{ 
  char tag[5];
  UNSIGNED_QUAD check_sum, offset, length;
  int omit;	/* If an application call sets omit=1, this table will
		   not be written to the output by ttf_build_font() */
  void *table_data;
};

struct table_directory
{
  SIGNED_QUAD version;
  UNSIGNED_PAIR num_tables, search_range,
    entry_selector, range_shift;
  struct table_header *tables;
};

static void release_directory (struct table_directory *r) 
{
  if (r && r->tables) {
    RELEASE (r->tables);
  }
  if (r)
    RELEASE (r);
}

static int put_big_endian (char *s, UNSIGNED_QUAD q, int n)
{
  int i;
  for (i=n-1; i>=0; i--) {
    s[i] = (char) q%256;
    q/=256L;
  }
  return n;
}

static unsigned max2floor(unsigned n)
/* Computes the max power of 2 <= n */
{
  int i = 1;
  while (n > 1) {
    n /= 2;
    i *= 2;
  }
  return i;
}

static unsigned log2floor(unsigned  n)
/* Computes the log2 of the max power of 2 <= n */
{
  unsigned i = 0;
  while (n > 1) {
    n /= 2;
    i += 1;
  }
  return i;
}


static char *ttf_build_font (FILE *ttf_file, struct table_directory
			     *td, long *size) 
{
  char *result, *p;
  long font_size = 0, offset = 0;
  int i, num_kept_tables = 0, new_search_range;
  for (i=0; i<(td->num_tables); i++) {
    if (!(td->tables)[i].omit) {
      font_size += (td->tables[i].length);
      num_kept_tables += 1;
    }
  }
  font_size += (td->num_tables)*16; /* 16 bytes per table entry */
  font_size += 12; /* 12 bytes for the directory */
  *size = font_size;
  result = NEW (font_size, char);
  p = result;
  { /* Header */
    p += put_big_endian (p, td->version, 4);
    p += put_big_endian (p, num_kept_tables, 2);
    new_search_range = max2floor(num_kept_tables) * 16;
    p += put_big_endian (p, new_search_range, 2);
    p += put_big_endian (p, log2floor(num_kept_tables), 2);
    p += put_big_endian (p, num_kept_tables*16-new_search_range, 2);
  }
  /* Computer start of actual tables (after headers) */
  offset = 12 + 16 * num_kept_tables;
  for (i=0; i<(td->num_tables); i++) {
    if (!(td->tables)[i].omit) {
      sprintf (p, "%4s", (td->tables)[i].tag);
      p += strlen (p);
      p += put_big_endian (p, (td->tables)[i].check_sum, 4);
      p += put_big_endian (p, offset, 4);
      p += put_big_endian (p, (td->tables)[i].length, 4);
      /* Be careful here.  Offset to the right place in the file
	 using the old offset and read the data into the right
	 place in the buffer using the new offset */
      seek_absolute (ttf_file, (td->tables)[i].offset);
      fread (result+offset, (td->tables)[i].length, sizeof(char), ttf_file);
      /* Set offset for next table */
      offset += (td->tables)[i].length;
      if (offset > font_size )
	ERROR ("Uh oh");
    }
  }
  return result;
}


static struct table_directory *read_directory(FILE *ttf_file)
{
  unsigned long i;
  struct table_directory *td = NEW (1, struct table_directory);
  rewind (ttf_file);
  td -> version = get_unsigned_quad(ttf_file);
  td -> num_tables = get_unsigned_pair (ttf_file);
  td -> search_range = get_unsigned_pair (ttf_file);
  td -> entry_selector = get_unsigned_pair (ttf_file);
  td -> range_shift = get_unsigned_pair (ttf_file);
  if (verbose > 3) {
    fprintf (stdout, "File Header\n");
    fprintf (stdout, "\tVersion: %.5f\n", fixed(td -> version));
    fprintf (stdout, "\tNumber of tables: %d\n",
	     td -> num_tables);
    fprintf (stdout, "\tSearch Range: %d\n", td -> search_range);
    fprintf (stdout, "\tEntry Selector: %d\n",
	     td -> entry_selector);
    fprintf (stdout, "\tRange Shift: %d\n",
	     td -> range_shift);
  }
  td->tables = NEW (td -> num_tables, struct table_header);
  for (i=0; i < td->num_tables; i++) {
    unsigned long u_tag;
    if (verbose > 3) {
      fprintf (stdout, "New Table\n");
    }
    u_tag = get_unsigned_quad (ttf_file);
    convert_tag ((td->tables)[i].tag, u_tag);
    (td->tables)[i].check_sum = get_unsigned_quad (ttf_file);
    (td->tables)[i].offset = get_unsigned_quad (ttf_file);
    (td->tables)[i].length = get_unsigned_quad (ttf_file);
    if (verbose) {
      fprintf (stdout, "\tTag: %4s\n", (td->tables)[i].tag);
      fprintf (stdout, "\tChecksum: %lx\n", (td->tables)[i].check_sum);
      fprintf (stdout, "\tOffset: %lx\n", (td->tables)[i].offset);
      fprintf (stdout, "\tLength: %lx\n", (td->tables)[i].length);
    }
    (td->tables)[i].omit = 0;
    (td->tables)[i].table_data = NULL;
  }
  return td;
}

static int find_table_index (struct table_directory *td, char *tag)
{
  int result, i;
  for (i=0; i < td->num_tables; i++) {
    if (!strncmp ((td->tables)[i].tag, tag, 4)) {
      break;
    }
  }
  if (i < td-> num_tables)
    result = i;
  else 
    result = -1;
  return result;
}


static long find_table_pos (FILE *ttf_file, struct table_directory *td, char *tag) 
{
  int i;
  long result = -1;
  if ((i=find_table_index (td, tag)) >= 0) {
    result = (td->tables)[i].offset;
  }
  return result;
}

static struct post_header
{
  UNSIGNED_QUAD format;
  UNSIGNED_QUAD italicAngle;
  SIGNED_PAIR underlinePosition;
  SIGNED_PAIR underlineThickness;
  UNSIGNED_QUAD isFixedPitch;
} post_header;

void read_post_table(FILE *ttf_file, struct table_directory *td)
{
  long post_offset = find_table_pos (ttf_file, td, "post");
  seek_absolute (ttf_file, post_offset);
  if (verbose > 3) {
    fprintf (stdout, "Table type: post\n");
    fprintf (stderr, "post table @ %ld\n", post_offset);
  }
  post_header.format = get_unsigned_quad(ttf_file);
  post_header.italicAngle = get_unsigned_quad (ttf_file);
  post_header.underlinePosition = get_signed_pair (ttf_file);
  post_header.underlineThickness = get_signed_pair (ttf_file);
  post_header.isFixedPitch = get_unsigned_quad (ttf_file);
  if (verbose > 3) {
    fprintf (stdout, "\tVersion: %f(%lx)\n",
	     fixed(post_header.format),
	     post_header.format);
    fprintf (stdout, "\tItalic Angle: %f\n",
	     fixed(post_header.italicAngle));
    fprintf (stdout, "\tUnderline Position: %d\n",
	     post_header.underlinePosition);
    fprintf (stdout, "\tUnderline Thickness: %d\n",
	     post_header.underlineThickness);
    fprintf (stdout, "\tIs Fixed Pitch?: %ld\n",
	     post_header.isFixedPitch);
  }
  return;
}

struct horz_header
{
  UNSIGNED_QUAD version;
  SIGNED_PAIR ascender, descender, line_gap;
  UNSIGNED_PAIR advanceWidthMax;
  SIGNED_PAIR minLeftSideBearing, minRightSideBearing, xMaxExtent;
  SIGNED_PAIR caretSlopeRise, caretSlopeRun;
  UNSIGNED_PAIR numberOfHMetrics;
} horz_header;

void read_hhea_table (FILE *ttf_file)
{
  horz_header.version = get_unsigned_quad(ttf_file);
  horz_header.ascender = get_signed_pair (ttf_file);
  horz_header.descender = get_signed_pair (ttf_file);
  horz_header.line_gap = get_signed_pair (ttf_file);
  if (verbose > 3) {
    fprintf (stdout, "Table type: hhea\n");
    fprintf (stdout, "\tVersion: %f\n", fixed(horz_header.version));
    fprintf (stdout, "\tAscender: %d\n", horz_header.ascender);
    fprintf (stdout, "\tDescender: %d\n", horz_header.descender);
    fprintf (stdout, "\tLine Gap: %d\n", horz_header.line_gap);
  }
}


struct font_header 
{
  double version, revision;
  UNSIGNED_QUAD check_sum;
  UNSIGNED_QUAD magic;
  UNSIGNED_PAIR flags, units_per_em;
  SIGNED_PAIR xMin, yMin, xMax, yMax;
  UNSIGNED_PAIR style, minsize;
  SIGNED_PAIR fontDirectionHint, indexToLocFormat;
  SIGNED_PAIR glyphDataFormat;
};

struct font_header *read_head_table(FILE *ttf_file, struct table_directory *td)
{
  struct font_header *r;
  int i;
  unsigned long fw;
  long head_offset = find_table_pos (ttf_file, td, "head");
  seek_absolute (ttf_file, head_offset);
  r = NEW (1, struct font_header);
  fw = get_unsigned_quad(ttf_file);
  (r->version) = fixed(fw);
  fw = get_unsigned_quad(ttf_file);
  (r->revision) = fixed(fw);
  r->check_sum = get_unsigned_quad(ttf_file);
  r->magic = get_unsigned_quad(ttf_file);
  r->flags = get_unsigned_pair(ttf_file);
  r->units_per_em = get_unsigned_pair(ttf_file);
  if (verbose > 3) {
    fprintf (stdout, "Table type: head\n");
    fprintf (stdout, "\tVersion: %f\n", r->version);
    fprintf (stdout, "\tRevision: %f\n", r->revision);
    fprintf (stdout, "\tChecksum: %lx\n", r->check_sum);
    fprintf (stdout, "\tMagic: %lx\n", r->magic);
    fprintf (stdout, "\tFlags: %x\n", r->flags);
    fprintf (stdout, "\tunits_per_em: %d\n", r->units_per_em);
  }
  /* Skip Dates */
  for (i=0; i<4; i++) {
    get_unsigned_quad (ttf_file);
  }
  r->xMin = get_signed_pair(ttf_file);
  r->yMin = get_signed_pair(ttf_file);
  r->xMax = get_signed_pair(ttf_file);
  r->yMax = get_signed_pair(ttf_file);
  r->style = get_unsigned_pair(ttf_file);
  r->minsize = get_unsigned_pair(ttf_file);
  r->fontDirectionHint = get_signed_pair(ttf_file);
  r->indexToLocFormat = get_signed_pair(ttf_file);
  r->glyphDataFormat = get_signed_pair(ttf_file);
  if (verbose > 3) {
    fprintf (stdout, "\txMin: %d\n", r->xMin);
    fprintf (stdout, "\tyMin: %d\n", r->yMin);
    fprintf (stdout, "\txMax: %d\n", r->xMax);
    fprintf (stdout, "\tyMax: %d\n", r->yMax);
    fprintf (stdout, "\tyStyle: %d\n", r->style);
    fprintf (stdout, "\tyMin readable size (pixels): %d\n", r->minsize);
    fprintf (stdout, "\tDirection Hint: %d\n", r->fontDirectionHint);
    fprintf (stdout, "\tIndex Format: %d\n",
	     r->indexToLocFormat);
    fprintf (stdout, "\tData Format: %d\n",
	     r->glyphDataFormat);
  }
  return r;
}


static unsigned num_ttfs = 0;
static unsigned max_ttfs = 0;
struct a_ttf
{
  char *ttf_name;
  char *fontname;
  struct table_directory *directory;
  pdf_obj *direct, *indirect, *descriptor;
  char **used_glyphs;
  char **int_encoding;
  char *used_def_enc_chars;	/* The positions used from the default
				   encoding.  When a default encoding
				   is used, the glyph names will not
				   be known until the font is actually
				   read.  Since the glyph names are
				   unknown, only the positions of the
				   used chars are stored when the
				   default encoding is used */
  unsigned n_used_glyphs, max_used_glyphs;
} *ttfs = NULL;

static void init_a_ttf (struct a_ttf *ttf)
{
  int i;
  ttf -> n_used_glyphs = 0;
  ttf -> max_used_glyphs = 0;
  ttf -> used_glyphs = NULL;
  ttf->ttf_name = NULL;
  ttf->fontname = NULL;
  ttf->direct = NULL;
  ttf->indirect = NULL;
  ttf->descriptor = NULL;
  ttf -> used_def_enc_chars = NULL;
  if (partial_enabled) {
    ttf -> int_encoding = NEW (256, char *);
    for (i=0; i<256; i++) {
      (ttf -> int_encoding)[i] = NULL;
    }
  } else {
    ttf -> int_encoding = NULL;
  }
}

#include "macglyphs.h"


#define FIXED_WIDTH 1
#define SERIF 2
#define STANDARD 32
#define ITALIC 64
#define SYMBOLIC 4   /* Fonts that don't have Adobe encodings (e.g.,
			cmr, should be set to be symbolic */
#define STEMV 80


extern int CDECL glyph_cmp(const void *v1, const void *v2);
extern int CDECL glyph_match(const void *key, const void *v);


static void dump_glyphs( char **glyphs, int n, int show_index)
{
  int i;
  for (i=0; i<n; i++) {
    if (show_index)
      fprintf (stderr, "(%d", i);
    if (glyphs[i])
      fprintf (stderr, "/%s", glyphs[i]);
    else
      fprintf (stderr, "(null)");
    if (show_index)
      fprintf (stderr, ")");
  }
  return;
}

static void dump_used( char *used_chars)
{
  int i;
  for (i=0; i<256; i++)
    fprintf (stderr, "(%d/%d)", i, used_chars[i]);
  return;
}

static unsigned int glyph_length (char **glyphs) 
{
  int i;
  unsigned result = 0;
  for (i=0; i<256; i++) {
    result += strlen (glyphs[i]);
  }
  return result;
}

static char *ttf_find_name (FILE *ttf_file, struct table_directory *td)
{
  char *result = NULL;
  unsigned num_names, string_offset;
  long name_offset;
  int i;
  name_offset = find_table_pos (ttf_file, td, "name");
  if (name_offset >= 0) {
    seek_absolute (ttf_file, name_offset);
    if (get_unsigned_pair (ttf_file)) 
      ERROR ("Expecting zero in ttf_find_name()");
    num_names = get_unsigned_pair (ttf_file);
    string_offset = get_unsigned_pair (ttf_file);
    for (i=0; i<num_names; i++) {
      unsigned platform_id, plat_encoding_id;
      unsigned language_id, name_id;
      unsigned length, offset;
      platform_id = get_unsigned_pair (ttf_file);
      plat_encoding_id = get_unsigned_pair (ttf_file);
      language_id = get_unsigned_pair (ttf_file);
      name_id = get_unsigned_pair (ttf_file);
      length = get_unsigned_pair (ttf_file);
      offset = get_unsigned_pair (ttf_file);
      if (platform_id == 1 &&
	  name_id == 6) { /* For now, return the PS font name */
	seek_absolute (ttf_file, name_offset+string_offset+offset);
	result = NEW (length+1, char);
	fread (result, length, sizeof(char), ttf_file);
	result[length] = 0;
	if (verbose > 3) {
	  fprintf (stdout, "TTF internal name is %s\n", result);
	}
	break;
      }
    }
    if (i == num_names) {
      ERROR ("Couldn't find ttf font name in a platform/encoding I could understand");
    }
  } else {
    fprintf (stdout, "Name table not found!\n");
  }
  return result;
}

static pdf_obj *ttf_fontfile (int ttf_id) 
{
  if (ttf_id >= 0 && ttf_id < num_ttfs) 
    return pdf_link_obj (ttfs[ttf_id].indirect);
  else
    return NULL;
}

static char *ttf_fontname (int ttf_id) 
{
  if (ttf_id >= 0 && ttf_id < num_ttfs) 
    return ttfs[ttf_id].fontname;
  else
    return NULL;
}


static void ttf_add_to_used_glyphs (int ttf_id, char *glyph)
{
  if (ttf_id >= 0 && ttf_id < num_ttfs && glyph) {
    if (ttfs[ttf_id].n_used_glyphs == 0 ||
	!bsearch (glyph, ttfs[ttf_id].used_glyphs,
		  ttfs[ttf_id].n_used_glyphs,
		  sizeof (char *), glyph_match)) {
      if (ttfs[ttf_id].n_used_glyphs+1 >=
	  ttfs[ttf_id].max_used_glyphs) {
	ttfs[ttf_id].max_used_glyphs += 16;
	ttfs[ttf_id].used_glyphs = RENEW (ttfs[ttf_id].used_glyphs,
					  ttfs[ttf_id].max_used_glyphs,
					  char *);
      }
      (ttfs[ttf_id].used_glyphs)[ttfs[ttf_id].n_used_glyphs] = 
	NEW (strlen(glyph)+1, char);
      strcpy((ttfs[ttf_id].used_glyphs)[ttfs[ttf_id].n_used_glyphs],
	     glyph);
      ttfs[ttf_id].n_used_glyphs += 1;
      qsort (ttfs[ttf_id].used_glyphs, ttfs[ttf_id].n_used_glyphs, 
	     sizeof (char *), glyph_cmp);
    }
  }
}

static char *new_used_chars (void)
{
  char *result;
  int i;
  result = NEW (256, char);
  for (i=0; i<256; i++) {
    result[i] = 0;
  }
  return result;
}

/* Mark the character at position "code" as used in the ttf font
   corresponding to "ttf_id" */
static void ttf_add_to_used_chars (int ttf_id, unsigned code)
{
  if (ttf_id >= 0 && ttf_id < num_ttfs && code < 256) {
    if (!ttfs[ttf_id].used_def_enc_chars) {
      ttfs[ttf_id].used_def_enc_chars = new_used_chars();
    }
    (ttfs[ttf_id].used_def_enc_chars)[code] = 1;
  }
  if (code >= 256)
    ERROR ("ttf_add_to_used_chars(): code >= 256");
  return;
}

/* Mangle_fontname mangles the name in place.  fontname
   must be big enough to add seven characters */

static void mangle_fontname(char *fontname)
{
  int i;
  char ch;
  static char first = 1;
  memmove (fontname+7, fontname, strlen(fontname)+1);
  /* The following procedure isn't very random, but it
     doesn't need to be for this application. */
  if (first) {
    srand (time(NULL));
    first = 0;
  }
  for (i=0; i<6; i++) {
    ch = rand() % 26;
    fontname[i] = ch+'A';
  }
  fontname[6] = '+';
}


/* This routine builds a default font descriptor with dummy values
   filled in for the required keys.  As the ttf file is parsed,
   any values that are found are rewritten.  By doing this,
   all the required fields are found in the font descriptor
   even if the ttf is somewhat defective. This approach is
   conservative, with the cost of keeping the names around in memory 
   for a while.
*/

static void ttf_start_font_descriptor (FILE *ttf_file, int ttf_id)
{
  pdf_obj *tmp1;
  struct font_header *r;
  ttfs[ttf_id].descriptor = pdf_new_dict ();
  
  pdf_add_dict (ttfs[ttf_id].descriptor,
		pdf_new_name ("Type"),
		pdf_new_name ("FontDescriptor"));
  /* For now, insert dummy values */
  pdf_add_dict (ttfs[ttf_id].descriptor,
		pdf_new_name ("CapHeight"),
		pdf_new_number (850.0)); /* This number is arbitrary */
  pdf_add_dict (ttfs[ttf_id].descriptor,
		pdf_new_name ("Ascent"),
		pdf_new_number (850.0));	/* This number is arbitrary */
  pdf_add_dict (ttfs[ttf_id].descriptor,
		pdf_new_name ("Descent"),
		pdf_new_number (-200.0));
  r = read_head_table(ttf_file, ttfs[ttf_id].directory);
  tmp1 = pdf_new_array ();
  pdf_add_array (tmp1, pdf_new_number ((double) r->xMin));
  pdf_add_array (tmp1, pdf_new_number ((double) r->yMin));
  pdf_add_array (tmp1, pdf_new_number ((double) r->xMax));
  pdf_add_array (tmp1, pdf_new_number ((double) r->yMax));
  RELEASE (r);
  pdf_add_dict (ttfs[ttf_id].descriptor, pdf_new_name ("FontBBox"), tmp1);
  pdf_add_dict (ttfs[ttf_id].descriptor,
		pdf_new_name ("FontName"),
 		pdf_new_name (ttf_fontname(ttf_id)));

  pdf_add_dict (ttfs[ttf_id].descriptor,
		pdf_new_name ("ItalicAngle"),
		pdf_new_number(0.0));
  pdf_add_dict (ttfs[ttf_id].descriptor,
		pdf_new_name ("StemV"),  /* StemV is required, StemH
					    is not */
		pdf_new_number (STEMV)); /* Use a default value */
  /* You don't need a fontfile for the standard fonts */
  if (ttf_id >= 0)
    pdf_add_dict (ttfs[ttf_id].descriptor,
		  pdf_new_name ("FontFile2"),
		  ttf_fontfile (ttf_id));

  pdf_add_dict (ttfs[ttf_id].descriptor,
		pdf_new_name ("Flags"),
		pdf_new_number (SYMBOLIC));  /* Treat all fonts as symbolic */
  return;
}

static int ttf_get_id (const char *ttf_name)
{
  int i;
  for (i=0; i<num_ttfs; i++) {
    if (ttfs[i].ttf_name && !strcmp (ttfs[i].ttf_name, ttf_name))
      break;
  }
  if (i == num_ttfs) { /* This font not previously called for */
    FILE *ttf_file;
    char *full_ttf_name, *short_fontname;
    if (!(full_ttf_name = kpse_find_file (ttf_name, kpse_truetype_format,
				    1)) || 
	!(ttf_file = MFOPEN (full_ttf_name, FOPEN_RBIN_MODE))) {
      return -1;
    }
    if (num_ttfs >= max_ttfs) {
      max_ttfs += MAX_FONTS;
      ttfs = RENEW (ttfs, max_ttfs, struct a_ttf);
    }
    num_ttfs += 1;
    init_a_ttf (ttfs+i);
    ttfs[i].directory = read_directory (ttf_file);
    short_fontname = ttf_find_name (ttf_file, ttfs[i].directory);
    ttfs[i].ttf_name = NEW (strlen(ttf_name)+1, char);
    strcpy (ttfs[i].ttf_name, ttf_name);
    ttfs[i].direct = pdf_new_stream(STREAM_COMPRESS);
    ttfs[i].indirect = pdf_ref_obj (ttfs[i].direct);
    if (partial_enabled) {
      ttfs[i].fontname = NEW (strlen(short_fontname)+8, char);
      strcpy (ttfs[i].fontname, short_fontname);
      mangle_fontname(ttfs[i].fontname);
    }
    else {
      ttfs[i].fontname = NEW (strlen(short_fontname)+1, char);
      strcpy (ttfs[i].fontname, short_fontname);
    }
    ttf_start_font_descriptor(ttf_file, i);
    if (short_fontname)
      RELEASE (short_fontname);
    MFCLOSE (ttf_file);
  }
  return i;
}

static void ttf_release (int id)
{
  if (id >= 0 && id < num_ttfs) {
    RELEASE (ttfs[id].ttf_name);
    pdf_release_obj (ttfs[id].indirect);
    release_directory (ttfs[id].directory);
    RELEASE (ttfs[id].fontname);

    if (ttfs[id].used_def_enc_chars)
      RELEASE (ttfs[id].used_def_enc_chars);
    if (ttfs[id].int_encoding){
      int i;
      for (i=0; i<256; i++) {
	if ((ttfs[id].int_encoding)[i])
	  RELEASE ((ttfs[id].int_encoding)[i]);
      }
      RELEASE (ttfs[id].int_encoding);
    }
    if (ttfs[id].used_glyphs) {
      unsigned i;
      for (i=0; i<ttfs[id].n_used_glyphs; i++) {
	RELEASE ((ttfs[id].used_glyphs)[i]);
      }
      RELEASE (ttfs[id].used_glyphs);
    }
  }
}

static void release_glyphs (char **glyphs)
{
  int i;
  for (i=0; i<256; i++) {
    RELEASE (glyphs[i]);
  }
}

static void omit_unecessary (struct table_directory *td) 
{
  int omitindex;
  if ((omitindex = find_table_index (td, "kern")) >= 0)
    (td->tables)[omitindex].omit = 1;
}

static void do_ttf (int ttf_id)
{
  char *full_ttf_name;
  FILE *ttf_file;
  pdf_obj *stream_dict;
  full_ttf_name = kpse_find_file (ttfs[ttf_id].ttf_name, kpse_truetype_format,
				  1);
  if (verbose == 1)
    fprintf (stderr, "(TTF:%s", ttfs[ttf_id].ttf_name);
  if (verbose > 1)
    fprintf (stderr, "(TTF:%s", full_ttf_name);
  if (full_ttf_name == NULL ||
      (ttf_file = MFOPEN (full_ttf_name, FOPEN_RBIN_MODE)) == NULL) {
    fprintf (stderr, "Unable to find or open binary font file (%s)",
	     ttfs[ttf_id].ttf_name);
    ERROR ("This existed when I checked it earlier!");
    return;
  }
  /* Following section doesn't hide PDF stream structure very well */

  /* The following section seems determines which, if any,
     glyphs were used via the internal encoding, which hasn't
     been known until now.*/
  if (partial_enabled) {
    int j;
    if (verbose > 2) {
      fprintf (stderr, "Default encoding:\n");
      dump_glyphs (ttfs[ttf_id].int_encoding, 256, 1);
    }
    if (ttfs[ttf_id].used_def_enc_chars) {
      if (verbose > 2)
	fprintf (stderr, "\nRetaining portion of default encoding:\n");
      for (j=0; j<256; j++) {
	if ((ttfs[ttf_id].used_def_enc_chars)[j]) {
	  if (verbose > 2)
	    fprintf (stderr, "(%d/%s)", j, (ttfs[ttf_id].int_encoding)[j]);
	  ttf_add_to_used_glyphs (ttf_id, (ttfs[ttf_id].int_encoding)[j]);
	}
      }
    }
  }
  {
    char *buffer;
    long size;
    omit_unecessary (ttfs[ttf_id].directory);
    buffer = ttf_build_font (ttf_file, ttfs[ttf_id].directory, &size);
    MFCLOSE (ttf_file);
    stream_dict = pdf_stream_dict (ttfs[ttf_id].direct);
    pdf_add_stream(ttfs[ttf_id].direct, buffer, size);
    RELEASE (buffer);
    pdf_add_dict (stream_dict, pdf_new_name ("Length1"),
		  pdf_new_number (size));
    pdf_release_obj (ttfs[ttf_id].direct);
  /* Finally, flush the descriptor */
    pdf_release_obj (ttfs[ttf_id].descriptor);
  }
  return;
}

void ttf_flush_all (void)
{
  int i;
  for (i=0; i<num_ttfs; i++) {
    do_ttf(i);
    ttf_release (i);
  }
  if (ttfs)
    RELEASE (ttfs);
}

struct a_ttf_font
{
  pdf_obj *indirect, *encoding;
  long ttf_id;
  int encoding_id;
  char *used_chars;
} *ttf_fonts = NULL;
int num_ttf_fonts = 0, max_ttf_fonts = 0;


static void init_a_ttf_font (struct a_ttf_font *this_ttf_font) 
{
  if (partial_enabled) {
    this_ttf_font -> used_chars = new_used_chars ();
  } else {
    this_ttf_font -> used_chars = NULL;
  }
}

pdf_obj *ttf_font_resource (int ttf_id)
{
  if (ttf_id>=0 && ttf_id<max_ttf_fonts)
    return pdf_link_obj(ttf_fonts[ttf_id].indirect);
  else {
    ERROR ("Invalid font id in ttf_font_resource");
    return NULL;
  }
}

char *ttf_font_used (int ttf_id)
{
  char *result;
  if (ttf_id>=0 && ttf_id<max_ttf_fonts) {
    result = ttf_fonts[ttf_id].used_chars;
  } else {
    fprintf (stderr, "ttf_font_used: ttf_id=%d\n", ttf_id);
    ERROR ("Invalid font id in ttf_font_used");
  }
  return result;
}



int ttf_font (const char *map_name, int tfm_font_id, char
	      *resource_name, int encoding_id, int remap) 
{
  int i, result = -1;
  int tfm_firstchar, tfm_lastchar;
  int pdf_firstchar, pdf_lastchar;
  int ttf_id = -1;
  pdf_obj *font_resource=NULL, *tmp1, *font_encoding_ref;

  if (num_ttf_fonts >= max_ttf_fonts) {
    max_ttf_fonts = MAX (max_ttf_fonts+MAX_FONTS, num_ttf_fonts+1);
    ttf_fonts = RENEW (ttf_fonts, max_ttf_fonts, struct
			 a_ttf_font);
  }

  if ((ttf_id = ttf_get_id(map_name)) >= 0) {
    /* Looks like we have a physical font (either a reader font or a
       Type 1 font binary file).  */
    init_a_ttf_font (ttf_fonts+num_ttf_fonts);
    ttf_fonts[num_ttf_fonts].ttf_id = ttf_id;
    ttf_fonts[num_ttf_fonts].encoding_id = encoding_id;
  /* Allocate a dictionary for the physical font */
    font_resource = pdf_new_dict ();
    if (ttf_fonts[num_ttf_fonts].encoding_id >= 0) {
      font_encoding_ref = encoding_ref (encoding_id);
      pdf_add_dict (font_resource,
		    pdf_new_name ("Encoding"),
		    font_encoding_ref);
    }
    pdf_add_dict (font_resource,
		  pdf_new_name ("Type"),
		  pdf_new_name ("Font"));
    pdf_add_dict (font_resource,
		  pdf_new_name ("Subtype"),
		  pdf_new_name ("TrueType"));
    pdf_add_dict (font_resource, 
		  pdf_new_name ("Name"),
		  pdf_new_name (resource_name));
    if (ttf_fonts[num_ttf_fonts].ttf_id >= 0) {
      pdf_add_dict (font_resource, 
		    pdf_new_name ("FontDescriptor"),
 		    pdf_ref_obj(ttfs[ttf_fonts[num_ttf_fonts].ttf_id].descriptor));
    }
      /* If we are embedding this font, it may have been used by another virtual
	 font and we need to use the same mangled name.  Mangled
	 names are known only to the ttf module, so we call it to get
	 the name */
    if (ttf_fonts[num_ttf_fonts].ttf_id >= 0) {
      pdf_add_dict (font_resource, 
		    pdf_new_name ("BaseFont"),
		    pdf_new_name
		    (ttf_fontname(ttf_fonts[num_ttf_fonts].ttf_id)));
	/* Otherwise we use the base name */
    } else {
      pdf_add_dict (font_resource,
		    pdf_new_name ("BaseFont"),
		    pdf_new_name (map_name));
    }
    tfm_firstchar = tfm_get_firstchar(tfm_font_id);
    tfm_lastchar = tfm_get_lastchar(tfm_font_id);
    if (partial_enabled && remap) {
      unsigned char t;
      pdf_firstchar=255; pdf_lastchar=0;
      for (i=tfm_firstchar; i<=tfm_lastchar; i++) {
	if ((t=twiddle(i)) < pdf_firstchar)
	  pdf_firstchar = t;
	if (t > pdf_lastchar)
	  pdf_lastchar = t;
      }
    } else {
      pdf_firstchar = tfm_firstchar;
      pdf_lastchar = tfm_lastchar;
    }
    pdf_add_dict (font_resource,
		  pdf_new_name ("FirstChar"),
		  pdf_new_number (pdf_firstchar));
    pdf_add_dict (font_resource,
		  pdf_new_name ("LastChar"),
		  pdf_new_number (pdf_lastchar));
    tmp1 = pdf_new_array ();
    for (i=pdf_firstchar; i<=pdf_lastchar; i++) {
      if (partial_enabled && remap) {
	int t;
	if ((t=untwiddle(i)) <= tfm_lastchar && t>=tfm_firstchar)
	  pdf_add_array (tmp1,
			 pdf_new_number(ROUND(tfm_get_width
					      (tfm_font_id,t)*1000.0,0.1)));
	else
	  pdf_add_array (tmp1,
			 pdf_new_number(0.0));
      } else
	pdf_add_array (tmp1,
		       pdf_new_number(ROUND(tfm_get_width
					    (tfm_font_id, i)*1000.0,0.1)));
    }
    pdf_add_dict (font_resource,
		  pdf_new_name ("Widths"),
		  tmp1);
    ttf_fonts[num_ttf_fonts].indirect = pdf_ref_obj(font_resource);
    pdf_release_obj (font_resource);
    result = num_ttf_fonts;
    num_ttf_fonts += 1;
  }
  return result;
}


void ttf_close_all (void)
{
  int i, j;
  /* Three arrays are created by this module and need to be released */
  /* First, each TeX font name that ends up as a postscript font gets
     added to ttf_fonts (yes, even Times-Roman, etc.) */
  /* The first thing to do is to resolve all character references to 
     actual glyph references.  If an external encoding is specified,
     we simply look up the glyph name in the encoding.  If the internal
     encoding is being used, we add it to the used_chars array of
     the internal encoding */
  for (i=0; i<num_ttf_fonts; i++) {
    /* If font subsetting is enabled, each used character needs
       to be added to the used_glyphs array in the corresponding ttf
    */
    if (partial_enabled) {
      /* We always consider .notdef to be used */
      ttf_add_to_used_glyphs (ttf_fonts[i].ttf_id, ".notdef");
      for (j=0; j<256; j++) {
	char *glyph;
	if (ttf_fonts[i].ttf_id >= 0 &&
	    ttf_fonts[i].encoding_id >= 0 &&
	    (ttf_fonts[i].used_chars)[j]) {
	  glyph = encoding_glyph (ttf_fonts[i].encoding_id,
					j);
	  ttf_add_to_used_glyphs (ttf_fonts[i].ttf_id, glyph);
	}
	if (ttf_fonts[i].ttf_id >= 0 &&
	    ttf_fonts[i].encoding_id < 0 &&
	    (ttf_fonts[i].used_chars)[j])
	  ttf_add_to_used_chars (ttf_fonts[i].ttf_id, j);
      }
    }
    if (ttf_fonts[i].used_chars)
      RELEASE (ttf_fonts[i].used_chars);
    pdf_release_obj (ttf_fonts[i].indirect);
  }
  if (ttf_fonts)
    RELEASE (ttf_fonts);
  /* Second every distinct ttf name ends up in ttfs.  It is possible
     that two distinct tex names map to the same ttf name.  That's why
     there is a separate array for ttfs */
  /* Read any necessary font files and flush them */
  ttf_flush_all();
}


#endif /* HAVE_TTF_FORMATS */

