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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include "system.h"
#include "mem.h"
#include "error.h"
#include "mfileio.h"
#include "pdfobj.h"
#include "numbers.h"
#include "type1.h"
#include "tfm.h"
#include "pdfparse.h"
#include "pdflimits.h"
#include "t1crypt.h"
#include "twiddle.h"
#include "encodings.h"

static unsigned char verbose = 0;

void type1_set_verbose(void)
{
  if (verbose < 255) {
    verbose += 1;
  }
}


static unsigned long get_low_endian_quad (FILE *file)
{
  unsigned long result;
  static unsigned bytes[4];
  int ch, i;
  for (i=0; i<4; i++) {
    if ((ch = fgetc (file)) < 0) {
      ERROR ("get_low_endian_quad:  Error reading file");
    }
    bytes[i] = ch;
  }
  result = bytes[3];
  for (i=2; i>=0; i--) {
    result = result*256u + bytes[i];
  }
  return result;
}

/* PFB section */

static char partial_enabled = 1;

void type1_disable_partial (void)
{
  partial_enabled = 0;
}

static unsigned num_pfbs = 0;
static unsigned max_pfbs = 0;
struct a_pfb
{
  char *pfb_name;
  char *fontname;
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
} *pfbs = NULL;

static void init_a_pfb (struct a_pfb *pfb)
{
  int i;
  pfb -> n_used_glyphs = 0;
  pfb -> max_used_glyphs = 0;
  pfb -> used_glyphs = NULL;
  pfb->pfb_name = NULL;
  pfb->fontname = NULL;
  pfb->direct = NULL;
  pfb->indirect = NULL;
  pfb->descriptor = NULL;
  pfb -> used_def_enc_chars = NULL;
  if (partial_enabled) {
    pfb -> int_encoding = NEW (256, char *);
    for (i=0; i<256; i++) {
      (pfb -> int_encoding)[i] = NULL;
    }
  } else {
    pfb -> int_encoding = NULL;
  }
}

#include "standardenc.h"

static void do_a_standard_enc(char **glyphs, char **encoding) 
{
  int i;
  for (i=0; i<256; i++) {
    RELEASE (glyphs[i]);
    glyphs[i] = NEW (strlen(encoding[i])+1, char);
    strcpy (glyphs[i], encoding[i]);
  }
}
#define FIXED_WIDTH 1
#define SERIF 2
#define STANDARD 32
#define ITALIC 64
#define SYMBOLIC 4   /* Fonts that don't have Adobe encodings (e.g.,
			cmr, should be set to be symbolic */
#define STEMV 80


int CDECL glyph_cmp (const void *v1, const void *v2)
{
  char *s1, *s2;
  s1 = *((char **) v1);
  s2 = *((char **) v2);
  return (strcmp (s1, s2));
}

int CDECL glyph_match (const void *key, const void *v)
{
  char *s;
  s = *((char **) v);
  return (strcmp (key, s));
}

static unsigned long parse_header (unsigned char *filtered, unsigned char *buffer,
				   unsigned long length, int pfb_id)
{
  /* If the encoding has been overridden, this routine should eliminate any
     built-in encoding (other than things like StandardEncoding) in
     the header to save space */

  /* On second thought, that's the way it _should_ work, but the
     reader doesn't seem to agree.  The reader is happy if you don't
     specify an encoding as long as you actually change the locations
     in the overriding encoding. The reader is unhappy if you don't
     specify an encoding and don't change the location of the
     characters in the overriding encoding.  Ghostscript doesn't
     have a problem with it. */

  unsigned char *filtered_pointer;
  int state = 0;
  char *start, *end, *lead;
  int copy = 1;
  int last_number = 0;
  char *glyph = NULL;
#ifdef MEM_DEBUG
  MEM_START
#endif
  /* This routine uses a state machine parser rather than trying to
     interpret Postcript the way mpost.c does.  There are only
     a few key parameters it is trying to find */

  /* State definitions 
     state 0: Initial state
     state 1: Saw /FontName
     state 2: Saw /Encoding
     state 3: Saw "dup" in state 2
     state 4: Saw a number in state 3
     state 5: Saw /glyphname in state 4
     state 6: Saw /FontBBox in state 0
     state 7: Saw a '{' or a '[' in state 6  
     state 8: Saw /ItalicAngle in state 0 */
  /* As the parser operates, start always points to the next point in
     the buffer */
  start = (char *) buffer;
  end = start+length;
  filtered_pointer = filtered;
  /* When the parser decides to keep text, the text
     between lead and start is copied to the output buffer.  To
     keep a block of text from being copied, the parser simply
     sets copy = 0 */
  lead = start;
  skip_white (&start, end);
  if (filtered && lead != start) {
    memcpy (filtered_pointer, lead, start-lead);
    filtered_pointer += start-lead;
    lead = start;
  }
  while (start < end) {
    char *ident;
    pdf_obj *pdfobj;
    copy = 1; /* By default, we copy most things */
    switch (state) {
      /* First three states are very similar.  In most cases we just
	 ignore other postscript junk and don't change state */
    case 0:
    case 1:
    case 2:
      switch (*start) {
	/* Ignore arrays and procedures */
      case '[':
      case ']':  
      case '{':
      case '}':
	start += 1;
	if (state >= 2) 
	  state = 2;
	break;
      case '(':
	pdfobj = parse_pdf_string (&start, end);
	if (pdfobj == NULL) {
	  ERROR ("parse_header:  Error parsing a string in pfb header");
	}
	pdf_release_obj (pdfobj);
	if (state == 1) {
	  if (filtered)
	    sprintf ((char *)filtered_pointer, "/%s ", pfbs[pfb_id].fontname);
            filtered_pointer += strlen ((char *)filtered_pointer);
	  copy = 0; /* Don't copy old string to output */
	  lead = start; /* Forget what we've seen */
	  state = 0;
	}
	if (state >= 2)
	  state = 2;
	break;
      case '/':
	start += 1;
	ident = parse_ident (&start, end);
	if (state == 0 && !strcmp (ident, "FontName")) {
	  state = 1;
	} else if (state == 0 && !strcmp (ident, "Encoding")) {
	  state = 2;
	  if (filtered && !pfbs[pfb_id].used_def_enc_chars) {
	    sprintf ((char *)filtered_pointer, "/Encoding StandardEncoding readonly ");
            filtered_pointer += strlen ((char *)filtered_pointer);
	  }
	} else if (state == 0 && !strcmp (ident, "FontBBox")) {
	  state = 6;
	} else if (state == 0 && !strcmp (ident, "ItalicAngle")) {
	  state = 8;
	} else if (state == 1) {
	  if (filtered) {
	    sprintf ((char *)filtered_pointer, "/%s ", pfbs[pfb_id].fontname);
	    filtered_pointer += strlen ((char *)filtered_pointer);
          }
	  copy = 0;	/* Don't copy old string to putput */
	  lead = start;	/* Forget the name we've seen  */
	  state = 0;
	}
	RELEASE (ident);
	break;
      default:
	ident = parse_ident (&start, end);
	if (state == 2 && !strcmp (ident, "def")) {
	  /* Assume this is the end of the encoding */
	  state = 0;
	} else if (state == 2 &&
	    !strcmp (ident, "dup")) {
	  copy = 0;	/* Don't copy this to output buffer until we
			   know if we want to keep it */  
	  state = 3;
	} else if (state == 2 &&
		   !strcmp (ident, "StandardEncoding") &&
		   pfbs[pfb_id].int_encoding) {
	  do_a_standard_enc(pfbs[pfb_id].int_encoding, standardencoding);
	} else if (state == 2 &&
		   !strcmp (ident, "ISOLatin1Encoding") &&
		   pfbs[pfb_id].int_encoding) {
	  do_a_standard_enc(pfbs[pfb_id].int_encoding, isoencoding);
	}
	RELEASE (ident);
	break;
      }
      break;
    case 3:
      ident = parse_ident (&start, end);
      if (is_an_int (ident)) {
	last_number = (int) atof (ident);
	copy = 0;	/* We still don't know if we want to keep it */
	state = 4;
      } else {
	state = 2;	/* Contents back to "lead" will be flushed */
      }
      RELEASE (ident);
      break;
    case 4:
      if (*start == '/') {
	start += 1;
	glyph = parse_ident (&start, end);
	copy = 0;	/* We still don't know if we want to keep it.
			 Wait for a complete sequence before making
			 that decision */
	state = 5;
      } else {
	state = 2;
      }
      break;
    case 5:
      ident = parse_ident (&start, end);
      /* Here we either decide to keep or remove the encoding entry */
      if (ident != NULL && !strcmp (ident, "put") && 
	  (int) last_number < 256 && (int) last_number >= 0) {
	skip_white(&start, end); /* Remove white space */
	lead = start;  /* Remove this entry (it may or may not be
			  replaced with a rewritten entry) */
	copy = 0;
	if (filtered && 
 	    pfbs[pfb_id].used_def_enc_chars &&
 	    (pfbs[pfb_id].used_def_enc_chars)[last_number]) {
 	  sprintf((char *) filtered_pointer, "dup %d /%s put\n",
		  last_number,
		  glyph);
          filtered_pointer += strlen ((char *) filtered_pointer);
	}
	/* Add this glyph to the internal encoding table for the pfb
	 */
	if (pfbs[pfb_id].int_encoding &&
	    (!(pfbs[pfb_id].int_encoding)[last_number])) {
	  if ((pfbs[pfb_id].int_encoding)[last_number]) {
	    RELEASE ((pfbs[pfb_id].int_encoding)[last_number]);
	  }
	  (pfbs[pfb_id].int_encoding)[last_number] = glyph;
	  glyph = NULL; /* Prevent glyph from being released */
	}
      }
      if (glyph)
	RELEASE (glyph);
      if (ident != NULL)
	RELEASE (ident);
      state = 2;
      break;
    case 6:
      switch (*start) {
      case '[':
      case '{': 
	start += 1 ;
	state = 7;
	break;
      default:
	state = 0;	/* Something's probably wrong */
	fprintf (stderr, "\nUnexpected token after FontBBox.   Struggling along\n");
	dump (start, end);
      }
      break;
    case 7:
      switch (*start) {
      case ']':
      case '}':
	start += 1 ;
	state = 0;
	break;
      case '{':
      case '[':
      case '(':
      case '/':
	state = 0;	/* Something's probably wrong */
	fprintf (stderr, "\nUnexpected token in FontBBox array.  Struggling along\n");
	dump (start, end);
	break;
      default:
	ident = parse_ident (&start, end);
	if ((ident) && is_a_number (ident)) {
	  pdf_obj *tmp = pdf_lookup_dict (pfbs[pfb_id].descriptor,
					  "FontBBox");
	  pdf_add_array (tmp, pdf_new_number (atof (ident)));
	}
	if (ident)
	  RELEASE (ident);
      }
      break;
    case 8:
      switch (*start) {
      case '{': case '}': case '[': case ']': case '/':
	state = 0;
	break;
      default:
	ident = parse_ident (&start, end);
	if ((ident) && is_a_number (ident)) {
	  double italic = atof(ident);
	  if (italic != 0.0) {
	    int flags = (int) pdf_number_value(pdf_lookup_dict (pfbs[pfb_id].descriptor,
								"Flags"));
	    pdf_add_dict (pfbs[pfb_id].descriptor, 
			  pdf_new_name ("ItalicAngle"),
			  pdf_new_number (italic));
	    pdf_add_dict (pfbs[pfb_id].descriptor,
			  pdf_new_name ("Flags"),
			  pdf_new_number (flags+ITALIC));
	  }
	}
	if (ident)
	  RELEASE (ident);
	state = 0;
      }
    }
    skip_white (&start, end);
    if (state >=2 && state <= 5 && !pfbs[pfb_id].used_def_enc_chars) {
      lead = start;
    }
    if (copy && start != lead) { /* Flush everything back to "lead" */
      if (filtered) {
	memcpy (filtered_pointer, lead, start-lead);
	filtered_pointer += start-lead;
      }
      lead = start;
    }
  }
#ifdef MEM_DEBUG
  MEM_END
#endif /* MEM_DEBUG */
  return filtered? filtered_pointer-filtered: length;
}

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


#define ASCII 1
#define BINARY 2

static int is_a_pfb (FILE *file)
{
  int result, ch;
  if ((ch = fgetc (file)) == 128 &&
      ((ch = fgetc (file)) == ASCII ||
       (ch == BINARY))) {
    result = 1;
  } else 
    result = 0;
  rewind (file);
  return result;
}

static unsigned char *get_pfb_segment (unsigned long *length,
				       FILE *file, int expected_type)
{
  unsigned char *buffer = NULL;
  unsigned long nread;
  unsigned long new_length;
  int stream_type, ch;

  *length = 0;
  /* Unfortunately, there can be several segments that need to be
     concatenated, so we loop through all of them */
  for (;;) {
    if ((ch = fgetc (file)) < 0) /* Some files don't terminate
				    properly */
      break;
    if (ch != 128){
      sprintf (work_buffer, "get_pfb_segment:  Not a pfb file.\n");
      sprintf (work_buffer, "get_pfb_segment:  pfb header has %d, expecting 128\n", ch);
      ERROR (work_buffer);
    }
    if ((stream_type = fgetc (file)) < 0 || stream_type != expected_type) {
      seek_relative (file, -2); /* Backup up two (yuck!) */
      break;
    }
    new_length = get_low_endian_quad (file);
    if (verbose > 4) {
      fprintf (stderr, "Length of next PFB segment: %ld\n",
	       new_length);
    }
    buffer = RENEW (buffer, (*length)+new_length, unsigned char);
    if ((nread = fread(buffer+(*length), sizeof(unsigned char), new_length, file)) !=
	new_length) {
      fprintf (stderr, "Found only %ld/%ld bytes\n", nread, new_length);
      ERROR ("type1_do_pfb_segment:  Are you sure this is a pfb?");
    }
    *length += new_length;
  }
  if (*length == 0) {
    ERROR ("type1_get_pfb_segment: Segment length is zero");
  }
  if (expected_type == ASCII) {
    int i;
    for (i=0; i<(*length); i++) {
      if (buffer[i] == '\r')
	buffer[i] = '\n';  /* Show my Unix prejudice */
    }
  }
  return buffer;
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

static char *pfb_find_name (FILE *pfb_file) 
{
  unsigned char *buffer;
  unsigned long length = 0;
  char *start, *end, *fontname;
  int state = 0;
#ifdef MEM_DEBUG
  MEM_START
#endif
  rewind (pfb_file);
  buffer = get_pfb_segment (&length, pfb_file, ASCII);
  /* State definitions 
     state 0: Initial state
     state 1: Saw /FontName */
  start = (char *) buffer;
  end = start+length;
  skip_white (&start, end);
  fontname = NULL;
  while (start < end && fontname == NULL) {
    char *ident;
    pdf_obj *pdfobj;
    switch (*start) {
      /* Ignore arrays and procedures */
    case '[':
    case ']':  
    case '{':
    case '}':
      start += 1;
      if (state == 1) {
	ERROR ("Garbage following /FontName");
      }
      break;
    case '(':
      pdfobj = parse_pdf_string (&start, end);
      if (pdfobj == NULL) {
	ERROR ("parse_header:  Error parsing a string in pfb header");
      }
      if (state == 1) { /* This string must be the font name */
	char *tmp = pdf_string_value (pdfobj);
	fontname = NEW (strlen(tmp)+1, char);
	memcpy (fontname, tmp, strlen(tmp)+1);
      }
      pdf_release_obj (pdfobj);
      break;
    case '/':
      start += 1;
      ident = parse_ident (&start, end);
      if (state == 0 && !strcmp (ident, "FontName")) {
	state = 1;
      } else if (state == 1) {
	fontname = NEW (strlen(ident)+1, char);
	memcpy (fontname, ident, strlen(ident)+1);
      }
      RELEASE (ident);
      break;
    default:
      ident = parse_ident (&start, end);
      RELEASE (ident);
      break;
    }
    skip_white (&start, end);
  }
  RELEASE (buffer);
#ifdef MEM_DEBUG
  MEM_END
#endif /* MEM_DEBUG */
  return fontname;
}

static void pfb_add_to_used_glyphs (int pfb_id, char *glyph)
{
  if (pfb_id >= 0 && pfb_id < num_pfbs && glyph) {
    if (pfbs[pfb_id].n_used_glyphs == 0 ||
	!bsearch (glyph, pfbs[pfb_id].used_glyphs,
		  pfbs[pfb_id].n_used_glyphs,
		  sizeof (char *), glyph_match)) {
      if (pfbs[pfb_id].n_used_glyphs+1 >=
	  pfbs[pfb_id].max_used_glyphs) {
	pfbs[pfb_id].max_used_glyphs += 16;
	pfbs[pfb_id].used_glyphs = RENEW (pfbs[pfb_id].used_glyphs,
					  pfbs[pfb_id].max_used_glyphs,
					  char *);
      }
      (pfbs[pfb_id].used_glyphs)[pfbs[pfb_id].n_used_glyphs] = 
	NEW (strlen(glyph)+1, char);
      strcpy((pfbs[pfb_id].used_glyphs)[pfbs[pfb_id].n_used_glyphs],
	     glyph);
      pfbs[pfb_id].n_used_glyphs += 1;
      qsort (pfbs[pfb_id].used_glyphs, pfbs[pfb_id].n_used_glyphs, 
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

/* Mark the character at position "code" as used in the pfb font
   corresponding to "pfb_id" */
static void pfb_add_to_used_chars (int pfb_id, unsigned code)
{
  if (pfb_id >= 0 && pfb_id < num_pfbs && code < 256) {
    if (!pfbs[pfb_id].used_def_enc_chars) {
      pfbs[pfb_id].used_def_enc_chars = new_used_chars();
    }
    (pfbs[pfb_id].used_def_enc_chars)[code] = 1;
  }
  if (code >= 256)
    ERROR ("pfb_add_to_used_chars(): code >= 256");
  return;
}

static unsigned long do_pfb_header (FILE *file, int pfb_id)
{
  unsigned char *buffer, *filtered = NULL;
  unsigned long length = 0;
#ifdef MEM_DEBUG
MEM_START
#endif
  buffer = get_pfb_segment (&length, file, ASCII);
  if (partial_enabled) {
    filtered = NEW (length+strlen(pfbs[pfb_id].fontname)+1+1024, unsigned
		    char);
  }
  /* We must parse the header even if not doing font subsetting so
     that we can determine the parameters for the font descriptor.
     parse_head() won't write to a null pointer */
  length = parse_header (filtered, buffer, length, pfb_id);
  if (filtered) {	
    pdf_add_stream (pfbs[pfb_id].direct, (char *) filtered, length);
    RELEASE (filtered);
  } else {
    pdf_add_stream (pfbs[pfb_id].direct, (char *) buffer, length);
  }
  RELEASE (buffer);
#ifdef MEM_DEBUG
MEM_END
#endif
  return length;
}


static unsigned long parse_body (unsigned char *filtered, unsigned char
				 *unfiltered, unsigned long length, 
				 char **used_glyphs, unsigned *n_used,
				 pdf_obj *descriptor)
{
  char *start, *end, *tail, *ident;
  unsigned char *filtered_pointer;
  double last_number = 0.0;
  int state = 0;
  if (verbose > 2) {
    fprintf (stderr, "\nSearching for following glyphs in font:\n");
    dump_glyphs (used_glyphs, *n_used, 0);
  }
  start = (char *) unfiltered, end = (char *) unfiltered+length;
  /* Skip first four bytes */
  tail = start; filtered_pointer = filtered;
  start += 4;
  /* Skip everything up to the charstrings section */
  while (start < end) {
    pdf_obj *pdfobj;
    skip_white (&start, end);
    /* Very simple state machine
       state = 0: nothing special
       state = 1: Saw StdVW, but waiting for number after it */
    switch (*start) {
    case '[':
    case ']':
    case '{':
    case '}':
      start += 1;
      continue;
    case '(':
      pdfobj = parse_pdf_string (&start, end);
      if (pdfobj == NULL) {
	ERROR ("Error processing a string in a PFB file.");
      }
      pdf_release_obj (pdfobj);
      continue;
    case '/':
      start += 1;
      if ((ident = parse_ident (&start, end)) &&
	  !strcmp ((char *) ident, "CharStrings")) {
	RELEASE (ident);
	break;
      } else if (ident && !strcmp ((char *) ident, "StdVW")) {
	state = 1; /* Saw StdVW */
      }
      if (ident) {
	RELEASE (ident);
      } else {
	fprintf (stderr, "\nError processing identifier in PFB file.\n");
	dump (start, end);
      }
      continue;
    default:
      ident = parse_ident (&start, end);
      if (ident == NULL)
	ERROR ("Error processing a symbol in the PFB file.");
      if (is_an_int(ident))
	if (state == 1) {
	  pdf_add_dict (descriptor, pdf_new_name ("StemV"),
			pdf_new_number (atof (ident)));
	  state = 0;  /* Return to normal processing */
	} else
	  last_number = atof (ident); /* Might be start of RD */
      else {
	if (!strcmp (ident, "RD") ||
	    !strcmp (ident, "-|")) {
	  start += ((unsigned long) last_number) + 1;
	}
      }
      RELEASE (ident);
      continue;
    }
    break;
  }
  if (start >= end)
    ERROR ("Unexpected end of binary portion of PFB file");
  /* Copy what we have so far over to the new buffer */
  if (filtered) {
    memcpy (filtered_pointer, tail, start-tail);
    /* Advance pointer into new buffer */
    filtered_pointer += (start-tail);
  }
  /* At this point, start is positioned just before the beginning of the glyphs, just after
     the word /CharStrings.  The earlier portion of the input buffer has
     been copied to the output.  The remainder of the routine need not
     be executed if not doing font subsetting */  
  if (filtered) {
    char **this_glyph;
    sprintf ((char *) filtered_pointer, " %d", *n_used);
    filtered_pointer += strlen ((char *) filtered_pointer);
    skip_white(&start, end);
    /* The following ident *should* be the number of glyphs in this
       file */
    ident = parse_ident (&start, end);
    if (verbose>1) {
      fprintf (stderr, "\n  Embedding %d of %s glyphs\n", *n_used, ident);
    }
    if (ident == NULL || !is_an_int (ident) || *n_used > atof (ident)) 
      ERROR ("More glyphs needed than present in file");
    RELEASE (ident);
    tail = start;
    while (start < end && *start != '/') start++;
    if (filtered) {
      memcpy (filtered_pointer, tail, start-tail);
      filtered_pointer += (start-tail);
    }
    /* Now we are exactly at the beginning of the glyphs */
    while (start < end && *start == '/') {
      char *glyph;
      tail = start;
      start += 1;
      glyph = parse_ident (&start, end);
      /* Get the number that should follow the glyph name */
      skip_white(&start, end);
      ident = parse_ident (&start, end);
      if (!is_an_int (ident))
	ERROR ("Expecting an integer after glyph name");
      last_number = atof (ident);
      RELEASE (ident);
      /* The next identifier should be a "RD" or a "-|".  We don't
	 really care what it is */
      skip_white(&start, end);
      ident = parse_ident (&start, end);
      RELEASE (ident);
      /* Skip a blank */
      start += 1;
      /* Skip the binary stream */
      start += (unsigned long) last_number;
      /* Skip the "ND" or "|-" terminator */
      skip_white(&start, end);
      ident = parse_ident (&start, end);
      RELEASE (ident);
      skip_white (&start, end);
      if (*n_used != 0 && (this_glyph = bsearch (glyph, used_glyphs, *n_used, sizeof (char *),
						glyph_match))) {
	memcpy (filtered_pointer, tail, start-tail);
	filtered_pointer += start-tail;
	(*n_used)--;
	/* Remove glyph and rearrange used_glyphs array */
	{
	  RELEASE (*this_glyph);
	  (*this_glyph) = used_glyphs[*n_used];
	  qsort (used_glyphs, *n_used, sizeof (char *), glyph_cmp);
	}
      }
      RELEASE (glyph);
    }
    if (*n_used != 0) {
      fprintf (stderr,  "Didn't find following required glyphs in the font:\n");
      dump_glyphs (used_glyphs, *n_used, 0);
      ERROR ("Possibly the encoding is incorrect.");
    }
    /* Include the rest of the file verbatim */
    if (start < end){
      memcpy (filtered_pointer, start, end-start);
      filtered_pointer += end-start;
    }
    if (verbose>1) {
      fprintf (stderr, "  (subsetting eliminated %ld bytes)", length-(filtered_pointer-filtered));
    }
  }
  return (filtered? filtered_pointer-filtered: length);
}

static unsigned long do_pfb_body (FILE *file, int pfb_id)
{
  int i;
  unsigned char *buffer=NULL, *filtered=NULL;
  unsigned long length=0;
#ifdef MEM_DEBUG
  MEM_START
#endif
  buffer = get_pfb_segment (&length, file, BINARY);
  /* We need to decrypt the binary
     portion of the pfb */
  t1_crypt_init(EEKEY);
  for (i=0; i<length; i++) {
    buffer[i] = t1_decrypt(buffer[i]);
  }
  if (partial_enabled) {
    filtered = NEW (length, unsigned char);
  }
  length = parse_body (filtered, buffer, length, 
	 	       pfbs[pfb_id].used_glyphs,
		       &pfbs[pfb_id].n_used_glyphs,
		       pfbs[pfb_id].descriptor);
  /* And reencrypt the whole thing */
  t1_crypt_init (EEKEY);
  for (i=0; i<length; i++) {
    buffer[i] = t1_encrypt(partial_enabled? filtered[i]: buffer[i]);
  }
  if (filtered)
    RELEASE (filtered);
  pdf_add_stream (pfbs[pfb_id].direct, (char *) buffer, length);
  RELEASE (buffer);
#ifdef MEM_DEBUG
  MEM_START
#endif
  return length;
}

static unsigned long do_pfb_trailer (FILE *file, pdf_obj *stream)
{
  unsigned char *buffer;
  unsigned long length;
  buffer = get_pfb_segment (&length, file, ASCII);
  pdf_add_stream (stream, (char *) buffer, length);
  RELEASE (buffer);
  return length;
}


static pdf_obj *type1_fontfile (int pfb_id) 
{
  if (pfb_id >= 0 && pfb_id < num_pfbs)
    return pdf_link_obj(pfbs[pfb_id].indirect);
  else
    return NULL;
}

static char *type1_fontname (int pfb_id)
{
  if (pfb_id >= 0 && pfb_id < num_pfbs)
    return pfbs[pfb_id].fontname;
  else
    return NULL;
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
   filled in for the required keys.  As the pfb file is parsed,
   any values that are found are rewritten.  By doing this,
   all the required fields are found in the font descriptor
   even if the pfb is somewhat defective. This approach is
   conservative, with the cost of keeping the names around in memory 
   for a while.
*/

static void type1_start_font_descriptor (int pfb_id)
{
  pdf_obj *tmp1;
  pfbs[pfb_id].descriptor = pdf_new_dict ();
  pdf_add_dict (pfbs[pfb_id].descriptor,
		pdf_new_name ("Type"),
		pdf_new_name ("FontDescriptor"));
  /* For now, insert dummy values */
  pdf_add_dict (pfbs[pfb_id].descriptor,
		pdf_new_name ("CapHeight"),
		pdf_new_number (850.0)); /* This number is arbitrary */
  pdf_add_dict (pfbs[pfb_id].descriptor,
		pdf_new_name ("Ascent"),
		pdf_new_number (850.0));	/* This number is arbitrary */
  pdf_add_dict (pfbs[pfb_id].descriptor,
		pdf_new_name ("Descent"),
		pdf_new_number (-200.0));
  tmp1 = pdf_new_array ();
  pdf_add_dict (pfbs[pfb_id].descriptor, pdf_new_name ("FontBBox"), tmp1);
  pdf_add_dict (pfbs[pfb_id].descriptor,
		pdf_new_name ("FontName"),
		pdf_new_name (type1_fontname(pfb_id)));

  pdf_add_dict (pfbs[pfb_id].descriptor,
		pdf_new_name ("ItalicAngle"),
		pdf_new_number(0.0));
  pdf_add_dict (pfbs[pfb_id].descriptor,
		pdf_new_name ("StemV"),  /* StemV is required, StemH
					    is not */
		pdf_new_number (STEMV)); /* Use a default value */
  /* You don't need a fontfile for the standard fonts */
  if (pfb_id >= 0)
    pdf_add_dict (pfbs[pfb_id].descriptor,
		  pdf_new_name ("FontFile"),
		  type1_fontfile (pfb_id));

  pdf_add_dict (pfbs[pfb_id].descriptor,
		pdf_new_name ("Flags"),
		pdf_new_number (SYMBOLIC));  /* Treat all fonts as symbolic */
  return;
}

static int pfb_get_id (const char *pfb_name)
{
  int i;
  for (i=0; i<num_pfbs; i++) {
    if (pfbs[i].pfb_name && !strcmp (pfbs[i].pfb_name, pfb_name))
      break;
  }
  if (i == num_pfbs) { /* This font not previously called for */
    FILE *pfb_file = NULL;
    char *full_pfb_name, *short_fontname;
    if (!(full_pfb_name = kpse_find_file (pfb_name, kpse_type1_format,
				    1)) || 
	!(pfb_file = MFOPEN (full_pfb_name, FOPEN_RBIN_MODE)) ||
	!(is_a_pfb (pfb_file))) {
      if (pfb_file)
	MFCLOSE (pfb_file);
      i = -1;
    } else {
      short_fontname = pfb_find_name (pfb_file);
      MFCLOSE (pfb_file);
      if (num_pfbs >= max_pfbs) {
	max_pfbs += MAX_FONTS;
	pfbs = RENEW (pfbs, max_pfbs, struct a_pfb);
      }
      num_pfbs += 1;
      init_a_pfb (pfbs+i);
      pfbs[i].pfb_name = NEW (strlen(pfb_name)+1, char);
      strcpy (pfbs[i].pfb_name, pfb_name);
      pfbs[i].direct = pdf_new_stream(STREAM_COMPRESS);
      pfbs[i].indirect = pdf_ref_obj (pfbs[i].direct);
      if (partial_enabled) {
	pfbs[i].fontname = NEW (strlen(short_fontname)+8, char);
	strcpy (pfbs[i].fontname, short_fontname);
	mangle_fontname(pfbs[i].fontname);
      }
      else {
	pfbs[i].fontname = NEW (strlen(short_fontname)+1, char);
	strcpy (pfbs[i].fontname, short_fontname);
      }
      type1_start_font_descriptor(i);
      if (short_fontname)
	RELEASE (short_fontname);
    }
  }
  return i;
}

static void pfb_release (int id)
{
  if (id >= 0 && id < num_pfbs) {
    pdf_release_obj (pfbs[id].direct);
    RELEASE (pfbs[id].pfb_name);
    pdf_release_obj (pfbs[id].indirect);
    RELEASE (pfbs[id].fontname);
    if (pfbs[id].used_def_enc_chars)
      RELEASE (pfbs[id].used_def_enc_chars);
    if (pfbs[id].int_encoding){
      int i;
      for (i=0; i<256; i++) {
	if ((pfbs[id].int_encoding)[i])
	  RELEASE ((pfbs[id].int_encoding)[i]);
      }
      RELEASE (pfbs[id].int_encoding);
    }
    if (pfbs[id].used_glyphs) {
      unsigned i;
      for (i=0; i<pfbs[id].n_used_glyphs; i++) {
	RELEASE ((pfbs[id].used_glyphs)[i]);
      }
      RELEASE (pfbs[id].used_glyphs);
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

static void do_pfb (int pfb_id)
{
  char *full_pfb_name;
  FILE *type1_binary_file;
  pdf_obj *stream_dict;
  unsigned long length1, length2, length3;
  int ch;
  full_pfb_name = kpse_find_file (pfbs[pfb_id].pfb_name, kpse_type1_format,
				  1);
  if (verbose == 1)
    fprintf (stderr, "(PFB:%s", pfbs[pfb_id].pfb_name);
  if (verbose > 1)
    fprintf (stderr, "(PFB:%s", full_pfb_name);
  if (full_pfb_name == NULL ||
      (type1_binary_file = MFOPEN (full_pfb_name, FOPEN_RBIN_MODE)) == NULL) {
    fprintf (stderr, "type1_fontfile:  Unable to find or open binary font file (%s)",
	     pfbs[pfb_id].pfb_name);
    ERROR ("This existed when I checked it earlier!");
    return;
  }
  /* Following section doesn't hide PDF stream structure very well */
  length1 = do_pfb_header (type1_binary_file, pfb_id);
  /* The following section seems determines which, if any,
     glyphs were used via the internal encoding, which hasn't
     been known until now.*/
  if (partial_enabled) {
    int j;
    if (verbose > 2) {
      fprintf (stderr, "Default encoding:\n");
      dump_glyphs (pfbs[pfb_id].int_encoding, 256, 1);
    }
    if (pfbs[pfb_id].used_def_enc_chars) {
      if (verbose > 2)
	fprintf (stderr, "\nRetaining portion of default encoding:\n");
      for (j=0; j<256; j++) {
	if ((pfbs[pfb_id].used_def_enc_chars)[j]) {
	  if (verbose > 2)
	    fprintf (stderr, "(%d/%s)", j, (pfbs[pfb_id].int_encoding)[j]);
	  pfb_add_to_used_glyphs (pfb_id, (pfbs[pfb_id].int_encoding)[j]);
	}
      }
    }
  }
  length2 = do_pfb_body (type1_binary_file, pfb_id);
  length3 = do_pfb_trailer (type1_binary_file, pfbs[pfb_id].direct);
  if ((ch = fgetc (type1_binary_file)) != 128 ||
      (ch = fgetc (type1_binary_file)) != 3) {
    fprintf (stderr, "\n\nWarning:  PFB file may be improperly terminated\n");
  }
  /* Got entire file! */
  if (verbose > 1)
    fprintf (stderr, "\n  Embedded size: %ld bytes\n", length1+length2+length3);
  if (verbose) {
    fprintf (stderr, ")");
  }
  MFCLOSE (type1_binary_file);
  stream_dict = pdf_stream_dict (pfbs[pfb_id].direct);
  pdf_add_dict (stream_dict, pdf_new_name("Length1"),
		pdf_new_number (length1));
  pdf_add_dict (stream_dict, pdf_new_name("Length2"),
		pdf_new_number (length2));
  pdf_add_dict (stream_dict, pdf_new_name("Length3"),
		pdf_new_number (length3));
  /* Finally, flush the descriptor */
  pdf_release_obj (pfbs[pfb_id].descriptor);
  return;
}

void pfb_flush_all (void)
{
  int i;
  for (i=0; i<num_pfbs; i++) {
    do_pfb(i);
    pfb_release (i);
  }
  if (pfbs)
    RELEASE (pfbs);
}

struct a_type1_font
{
  pdf_obj *indirect, *encoding;
  long pfb_id;
  int encoding_id;
  char *used_chars;
} *type1_fonts = NULL;
int num_type1_fonts = 0, max_type1_fonts = 0;


static void init_a_type1_font (struct a_type1_font *this_type1_font) 
{
  if (partial_enabled) {
    this_type1_font -> used_chars = new_used_chars ();
  } else {
    this_type1_font -> used_chars = NULL;
  }
}

pdf_obj *type1_font_resource (int type1_id)
{
  if (type1_id>=0 && type1_id<max_type1_fonts)
    return pdf_link_obj(type1_fonts[type1_id].indirect);
  else {
    ERROR ("Invalid font id in type1_font_resource");
    return NULL;
  }
}

char *type1_font_used (int type1_id)
{
  char *result;
  if (type1_id>=0 && type1_id<max_type1_fonts) {
    result = type1_fonts[type1_id].used_chars;
  } else {
    fprintf (stderr, "type1_font_used: type1_id=%d\n", type1_id);
    ERROR ("Invalid font id in type1_font_used");
  }
  return result;
}


static int is_a_base_font (const char *name)
{
  static char *basefonts[] = {
    "Courier",			"Courier-Bold",		"Courier-Oblique",
    "Courier-BoldOblique",	"Helvetica",		"Helvetica-Bold",
    "Helvetica-Oblique",	"Helvetica-BoldOblique",	"Symbol",
    "Times-Roman",		"Times-Bold",		"Times-Italic",
    "Times-BoldItalic",		"ZapfDingbats"
  };
  int i;
  for (i=0; i<14; i++) {
    if (!strcmp (name, basefonts[i]))
      return 1;
  }
  return 0;
}

int type1_font (const char *map_name, int tfm_font_id, char
		*resource_name, int encoding_id, int remap) 
{
  int i, result = -1;
  int tfm_firstchar, tfm_lastchar;
  int pdf_firstchar, pdf_lastchar;
  int pfb_id = -1;
  pdf_obj *font_resource, *tmp1, *font_encoding_ref;

  if (num_type1_fonts >= max_type1_fonts) {
    max_type1_fonts = MAX (max_type1_fonts+MAX_FONTS, num_type1_fonts+1);
    type1_fonts = RENEW (type1_fonts, max_type1_fonts, struct
			 a_type1_font);
  }

  if ((map_name && is_a_base_font(map_name)) ||
      (pfb_id = pfb_get_id(map_name)) >= 0) {
    /* Looks like we have a physical font (either a reader font or a
       Type 1 font binary file).  */
    init_a_type1_font (type1_fonts+num_type1_fonts);
    type1_fonts[num_type1_fonts].pfb_id = pfb_id;
    type1_fonts[num_type1_fonts].encoding_id = encoding_id;
  /* Allocate a dictionary for the physical font */
    font_resource = pdf_new_dict ();
    if (type1_fonts[num_type1_fonts].encoding_id >= 0) {
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
		  pdf_new_name ("Type1"));
    pdf_add_dict (font_resource, 
		  pdf_new_name ("Name"),
		  pdf_new_name (resource_name));
    if (type1_fonts[num_type1_fonts].pfb_id >= 0) {
      pdf_add_dict (font_resource, 
		    pdf_new_name ("FontDescriptor"),
		    pdf_ref_obj(pfbs[type1_fonts[num_type1_fonts].pfb_id].descriptor));
    }
      /* If we are embedding this font, it may have been used by another virtual
	 font and we need to use the same mangled name.  Mangled
	 names are known only to the pfb module, so we call it to get
	 the name */
    if (type1_fonts[num_type1_fonts].pfb_id >= 0) {
      pdf_add_dict (font_resource, 
		    pdf_new_name ("BaseFont"),
		    pdf_new_name
		    (type1_fontname(type1_fonts[num_type1_fonts].pfb_id)));
	/* Otherwise we use the base name */
    } else {
      pdf_add_dict (font_resource,
		    pdf_new_name ("BaseFont"),
		    pdf_new_name (map_name));
    }
    if (!(map_name && is_a_base_font (map_name))) {
      tfm_firstchar = tfm_get_firstchar(tfm_font_id);
      tfm_lastchar = tfm_get_lastchar(tfm_font_id);
      if (remap) {
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
	if (remap) {
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
    }
    type1_fonts[num_type1_fonts].indirect = pdf_ref_obj(font_resource);
    pdf_release_obj (font_resource);
    result = num_type1_fonts;
    num_type1_fonts += 1;
  } else { /* Don't have a physical font */
    result = -1;
  }
  return result;
}


void type1_close_all (void)
{
  int i, j;
  /* Three arrays are created by this module and need to be released */
  /* First, each TeX font name that ends up as a postscript font gets
     added to type1_fonts (yes, even Times-Roman, etc.) */
  /* The first thing to do is to resolve all character references to 
     actual glyph references.  If an external encoding is specified,
     we simply look up the glyph name in the encoding.  If the internal
     encoding is being used, we add it to the used_chars array of
     the internal encoding */
  for (i=0; i<num_type1_fonts; i++) {
    /* If font subsetting is enabled, each used character needs
       to be added to the used_glyphs array in the corresponding pfb
    */
    if (partial_enabled) {
      /* We always consider .notdef to be used */
      pfb_add_to_used_glyphs (type1_fonts[i].pfb_id, ".notdef");
      for (j=0; j<256; j++) {
	char *glyph;
	if (type1_fonts[i].pfb_id >= 0 &&
	    type1_fonts[i].encoding_id >= 0 &&
	    (type1_fonts[i].used_chars)[j]) {
	  glyph = encoding_glyph (type1_fonts[i].encoding_id,
					j);
	  pfb_add_to_used_glyphs (type1_fonts[i].pfb_id, glyph);
	}
	if (type1_fonts[i].pfb_id >= 0 &&
	    type1_fonts[i].encoding_id < 0 &&
	    (type1_fonts[i].used_chars)[j])
	  pfb_add_to_used_chars (type1_fonts[i].pfb_id, j);
      }
    }
    if (type1_fonts[i].used_chars)
      RELEASE (type1_fonts[i].used_chars);
    pdf_release_obj (type1_fonts[i].indirect);
  }
  RELEASE (type1_fonts);
  /* Second every distinct pfb name ends up in pfbs.  It is possible
     that two distinct tex names map to the same pfb name.  That's why
     there is a separate array for pfbs */

  /* Read any necessary font files and flush them */
  pfb_flush_all();

}

