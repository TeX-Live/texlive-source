
#include <stdio.h>
#include "pdflimits.h"
#include "pdfobj.h"
#include "mem.h"
#include "error.h"
#include "mfileio.h"
#include "pdfparse.h"
#include "encodings.h"


static unsigned char verbose = 0;

void encoding_set_verbose(void)
{
  if (verbose < 255) {
    verbose += 1;
  }
}

struct encoding {
  char *enc_name;
  /* The following array isn't very efficient. It is constructed
     by peeling the names from the encoding object.  It makes
     it easier to construct an array in this format when the
     encoding must be obtained directly from the PFB file */
  char *glyphs[256];
  pdf_obj *encoding_ref;
} *encodings;
int num_encodings = 0, max_encodings=0;

#include "winansi.h"

pdf_obj *find_encoding_differences (pdf_obj *encoding)
{
  char filling = 0;
  int i;
  pdf_obj *result = pdf_new_array ();
  pdf_obj *tmp;
  for (i=0; i<256; i++) {
    tmp = pdf_get_array (encoding, i);
    if (tmp == NULL || tmp -> type != PDF_NAME) {
      ERROR ("Encoding file may be incorrect\n");
    }
    if (!strcmp (winansi_encoding[i],
		 pdf_name_value(tmp)))
      filling = 0;
    else{
      if (!filling)
	pdf_add_array (result, pdf_new_number (i));
      filling = 1;
      pdf_add_array (result, pdf_link_obj(tmp));
    }
  }
  return result;
}

pdf_obj *make_differences_encoding (pdf_obj *encoding)
{
  int i;
  int skipping = 1;
  pdf_obj *tmp, *result = pdf_new_array ();
  for (i=0; i<256; i++) {
    tmp = pdf_get_array (encoding, i);
    if (tmp && tmp -> type == PDF_NAME) {
      if (strcmp (".notdef", pdf_name_value (tmp))) { /* If not
							 .notdef */
	if (skipping) {
	  pdf_add_array (result, pdf_new_number (i));
	}
	pdf_add_array (result, pdf_link_obj(tmp));
	  skipping = 0;
      } else {
	skipping = 1;
      }
    } else {
      ERROR ("Encoding file may be incorrect\n");
    }
  }
  return result;
}

static void save_glyphs (char **glyph, pdf_obj *encoding)
{
  int i;
  char *glyph_name;
  for (i=0; i<256; i++) {
    glyph_name = pdf_string_value (pdf_get_array(encoding, i));
    glyph[i] = NEW (strlen(glyph_name)+1, char);
    strcpy (glyph[i], glyph_name);
  }
}

int get_encoding (const char *enc_name)
{
  FILE *encfile = NULL;
  char *full_enc_filename, *tmp;
  pdf_obj *result, *result_ref;
  long filesize;
  int i;
  /* See if we already have this saved */
  for (i=0; i<num_encodings; i++)
    if (!strcmp (enc_name, encodings[i].enc_name))
      return i;
  /* Guess not.. */
  /* Try base name before adding .enc.  Someday maybe kpse will do
     this */
  strcpy (tmp = NEW (strlen(enc_name)+5, char), enc_name);
  strcat (tmp, ".enc");
  if ((full_enc_filename = kpse_find_file (enc_name,
					   kpse_enc_format,
					   1)) == NULL &&
      (full_enc_filename = kpse_find_file (enc_name,
					   kpse_program_text_format,
					   1)) == NULL &&
      (full_enc_filename = kpse_find_file (tmp,
					   kpse_enc_format,
					   1)) == NULL &&
      (full_enc_filename = kpse_find_file (tmp,
					   kpse_program_text_format,
					   1)) == NULL) {
    sprintf (work_buffer, "Can't find encoding file: %s", enc_name) ;
    ERROR (work_buffer);
  }
  RELEASE (tmp);
  if ((encfile = MFOPEN (full_enc_filename, FOPEN_R_MODE)) == NULL ||
      (filesize = file_size (encfile)) == 0) {
    sprintf (work_buffer, "Error opening encoding file: %s", enc_name) ;
    ERROR (work_buffer);
  }
  if (verbose == 1)
    fprintf (stderr, "(ENC:%s", enc_name);
  if (verbose > 1)
    fprintf (stderr, "(ENC:%s", full_enc_filename);
  {  /* Got one and opened it */
    char *buffer, *start, *end, *junk_ident;
    pdf_obj *junk_obj, *encoding, *differences;
    buffer = NEW (filesize, char); 
    fread (buffer, sizeof (char), filesize, encfile);
    MFCLOSE (encfile);
    start = buffer;
    end = buffer + filesize;
    start[filesize-1] = 0;
    skip_white (&start, end);
    while (start < end && *start != '[') {
      if ((junk_ident = parse_ident (&start, end)) != NULL)
	RELEASE (junk_ident);
      else if ((junk_obj = parse_pdf_object (&start, end)) != NULL)
	pdf_release_obj (junk_obj);
      skip_white(&start, end);
    }
    if (start >= end ||
	(encoding = parse_pdf_array (&start, end)) == NULL) {
      fprintf (stderr, "%s: ", enc_name);
      ERROR ("Can't find an encoding in this file!\n");
    }
    RELEASE (buffer);
    /* Done reading file */
    if (verbose) {
      fprintf (stderr, ")");
    }
    /*    differences = find_encoding_differences (encoding); */
    differences = make_differences_encoding (encoding);
    /* Put the glyph names into a conventional array */
    if (num_encodings >= max_encodings) {
       max_encodings += MAX_ENCODINGS;
       encodings = RENEW (encodings, max_encodings, struct encoding);
    }
    save_glyphs (encodings[num_encodings].glyphs, encoding);
    pdf_release_obj (encoding);
    result = pdf_new_dict();
    pdf_add_dict (result, pdf_new_name ("Type"),
		  pdf_new_name ("Encoding"));
    /* Some software doesn't like BaseEncoding key (e.g., FastLane) 
       so this code is commented out for the moment.  It may reemerge in the
       future */
    /*    pdf_add_dict (result, pdf_new_name ("BaseEncoding"),
	  pdf_new_name ("WinAnsiEncoding")); */
    pdf_add_dict (result, pdf_new_name ("Differences"),
		  differences);
  }
  {
    result_ref = pdf_ref_obj (result);
    pdf_release_obj (result);
    encodings[num_encodings].encoding_ref = result_ref;
    encodings[num_encodings].enc_name = NEW (strlen(enc_name)+1, char);
    strcpy (encodings[num_encodings].enc_name, enc_name);
    return num_encodings++;
  }
}

pdf_obj *encoding_ref (int encoding_id) 
{
  pdf_obj *result = NULL;
  if (encoding_id >=0 && encoding_id < num_encodings) {
    result = pdf_link_obj (encodings[encoding_id].encoding_ref);
  }
  return result;
}

void encoding_flush_all (void) 
{
  int i, j;
  for (i=0; i<num_encodings; i++) {
    RELEASE (encodings[i].enc_name);
    pdf_release_obj (encodings[i].encoding_ref);
    /* Release glyph names for this encoding */
    for (j=0; j<256; j++) {
      RELEASE ((encodings[i].glyphs)[j]);
    }
  }
  if (encodings)
    RELEASE (encodings);
}

char *encoding_glyph (int encoding_id, unsigned code) 
{
  char *result = NULL;
  if (encoding_id >= 0 && encoding_id < num_encodings &&
      code < 256) {
    result = (encodings[encoding_id].glyphs)[code];
  }
  return result;
}
