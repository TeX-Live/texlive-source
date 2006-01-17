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

	
#include <ctype.h>
#include <string.h>
#include "system.h"
#include "config.h"
#include "mem.h"
#include "error.h"
#include "mfileio.h"
#include "pdflimits.h"
#include "pdfobj.h"
#include "pdfspecial.h"
#include "pdfparse.h"
#include "twiddle.h"

#ifdef HAVE_ZLIB
#include <zlib.h>
#endif /* HAVE_ZLIB */

FILE *pdf_output_file = NULL;
FILE *pdf_input_file = NULL;
unsigned long pdf_output_file_position = 0, compression_saved = 0;
int pdf_output_line_position = 0;
#define FORMAT_BUF_SIZE 4096
char format_buffer[FORMAT_BUF_SIZE];

static struct xref_entry 
{
  unsigned long file_position;
  pdf_obj *pdf_obj;
} *output_xref = NULL;
static unsigned long pdf_max_ind_objects = 0;
static unsigned long next_label = 1;

static unsigned long startxref;

static unsigned pdf_root_obj = 0, pdf_info_obj = 0;

/* Internal static routines */

static void pdf_flush_obj (FILE *file, const pdf_obj *object);
static void pdf_label_obj (pdf_obj *object);

static void pdf_out_char (FILE *file, char c);
static void pdf_out (FILE *file, void *buffer, int length);

static void release_indirect (pdf_indirect *data);
static void write_indirect (FILE *file, const pdf_indirect *indirect);

static void release_boolean (pdf_obj *data);
static void write_boolean (FILE *file, const pdf_boolean *data);

static void write_null (FILE *file);
static void release_null (void *data);

static void release_number (pdf_number *data);
static void write_number (FILE *file, const pdf_number
			  *number);

static void write_string (FILE *file, const pdf_string *string);
static void release_string (pdf_string *data);

static void write_name (FILE *file, const pdf_name *name);
static void release_name (pdf_name *data);

static void write_array (FILE *file, const pdf_array *array);
static void release_array (pdf_array *data);

static void write_dict (FILE *file, const pdf_dict *dict);
static void release_dict (pdf_dict *data);

static void write_stream (FILE *file, pdf_stream *stream);
static void release_stream (pdf_stream *stream);
static int pdf_match_name (const pdf_obj *name_obj, const char *name);  /* Name does not include the / */

static unsigned char debug = 0, verbose = 0;
static char compression_level = 9;

void pdf_obj_set_compression (int level)
{
  if (level >= 0 && level <= 9) 
    compression_level = level;
  else {
    ERROR("set_compression: compression level");
  }
#ifndef HAVE_ZLIB_COMPRESS2
  if (level != 0) 
    fprintf (stderr, "Unable to set compression level--your zlib doesn't have compress2()\n");
#endif
  return;
}

static unsigned pdf_version = 3;
void pdf_set_version (unsigned version)
{
  if (version >= 2 && version <= 3) {
    pdf_version = version;
  }
}

void pdf_obj_set_debug(void)
{
  debug = 1;
}

void pdf_obj_set_verbose(void)
{
  if (verbose < 255)
    verbose += 1;
}


void pdf_out_init (const char *filename)
{
  char v;
#ifdef MEM_DEBUG
MEM_START
#endif
  if (!(pdf_output_file = MFOPEN (filename, FOPEN_WBIN_MODE))) {
    if (strlen(filename) < 128) {
      sprintf (format_buffer, "Unable to open %s\n", filename);
    } else
      sprintf (format_buffer, "Unable to open file");
    ERROR (format_buffer);
  }
  pdf_out (pdf_output_file, "%PDF-1.", strlen("%PDF-1."));
  v = '0'+pdf_version;
  pdf_out (pdf_output_file, &v, 1);
  pdf_out (pdf_output_file, "\n", 1);
#ifdef MEM_DEBUG
MEM_END
#endif
}

static void dump_xref(void)
{
  int length;
  unsigned long i;
  startxref = pdf_output_file_position;	/* Record where this xref is for
				   trailer */
  pdf_out (pdf_output_file, "xref\n", 5);
  sprintf (format_buffer, "%d %lu\n", 0, next_label);
  length = strlen (format_buffer);
  pdf_out (pdf_output_file, format_buffer, length);
  sprintf (format_buffer, "%010ld %05ld f \n", 0L, 65535L);
  length = strlen (format_buffer);
  /* Every space counts.  The space after the 'f' and 'n' is
   *essential*.  The PDF spec says the lines must be 20 characters
   long including the end of line character. */
  pdf_out (pdf_output_file, format_buffer, length);
  for (i=1; i<next_label; i++){
    sprintf (format_buffer, "%010ld %05ld n \n",
	     output_xref[i-1].file_position, 0L);
    length = strlen (format_buffer);
    pdf_out (pdf_output_file, format_buffer, length);
  }
  /* Done with xref table */
  RELEASE (output_xref);
}

static void dump_trailer(void)
{
  int length;
  unsigned long starttrailer;
  starttrailer = pdf_output_file_position;
  pdf_out (pdf_output_file, "trailer\n", 8);
  pdf_out (pdf_output_file, "<<\n", 3);
  sprintf (format_buffer, "/Size %lu\n",
	   next_label);
  length = strlen (format_buffer);
  pdf_out (pdf_output_file, format_buffer, length);
  if (pdf_root_obj == 0) 
    ERROR ("dump_trailer:  Invalid root object");
  sprintf (format_buffer, "/Root %u %u R\n", pdf_root_obj, 0);
  length = strlen (format_buffer);
  pdf_out (pdf_output_file, format_buffer, length);
  if (pdf_info_obj != 0) {
    sprintf (format_buffer, "/Info %u %u R\n", pdf_info_obj, 0);
    length = strlen (format_buffer);
    pdf_out (pdf_output_file, format_buffer, length);
  }
  pdf_out (pdf_output_file, ">>\n", 3);
  pdf_out (pdf_output_file, "startxref\n", 10);
  sprintf (format_buffer, "%lu\n", startxref);
  length = strlen (format_buffer);
  pdf_out (pdf_output_file, format_buffer, length);
  pdf_out (pdf_output_file, "%%EOF\n", 6);
}

void pdf_out_flush (void)
{
  if (pdf_output_file) {
    if (debug) fprintf (stderr, "pdf_obj_out_flush:  dumping xref\n");
    dump_xref();
    if (debug) fprintf (stderr, "pdf_obj_out_flush:  dumping trailer\n");
    dump_trailer();
    if (verbose) {
      if (compression_level>0) {
	fprintf (stderr, "\nCompression eliminated approximately %lu bytes",
		 compression_saved);
      }
    }
    fprintf (stderr, "\n%lu bytes written",
	     pdf_output_file_position);
    MFCLOSE (pdf_output_file);
  }
}

void pdf_error_cleanup (void)
{
  /* This routine is the cleanup required for an abnormal exit.
     For now, simply close the file. */
  if (pdf_output_file)
    MFCLOSE (pdf_output_file);
}


void pdf_set_root (pdf_obj *object)
{
  if (pdf_root_obj != 0) {
    ERROR ("pdf_set_root:  root object already set");
  }
  if (object -> label == 0) {  /* Make sure this object has a label */
    pdf_label_obj (object);
  }
  pdf_root_obj = object -> label;
}

void pdf_set_info (pdf_obj *object)
{
  if (pdf_info_obj != 0) {
    ERROR ("pdf_set_info:  info object already set");
  }
  if (object -> label == 0) {  /* Make sure this object has a label */
    pdf_label_obj (object);
  }
  pdf_info_obj = object -> label;
}

static void pdf_out_char (FILE *file, char c)
{
  fputc (c, file);
  /* Keep tallys for xref table *only* if writing a pdf file */
  if (file == pdf_output_file) {
    pdf_output_file_position += 1;
    pdf_output_line_position += 1;
  }
  if (file == pdf_output_file && c == '\n')
    pdf_output_line_position = 0;
}

static void pdf_out (FILE *file, void *buffer, int length)
{
  fwrite (buffer, 1, length, file);
  /* Keep tallys for xref table *only* if writing a pdf file */
  if (file == pdf_output_file) {
    pdf_output_file_position += length;
    pdf_output_line_position += length;
    if (length > 0 && ((char *)buffer)[length-1] == '\n')
      pdf_output_line_position = 0;
  }
}

static void pdf_out_white (FILE *file)
{
  if (file == pdf_output_file && pdf_output_line_position >= 80) {
    pdf_out_char (file, '\n');
  } else {
    pdf_out_char (file, ' ');
  }
  return;
}

pdf_obj *pdf_new_obj(pdf_obj_type type)
{
  pdf_obj *result;
  result = NEW (1, pdf_obj);
  result -> type = type;
  result -> data = NULL;
  result -> label = 0;
  result -> generation = 0;
  result -> refcount = 1;
  return result;
}

static void pdf_label_obj (pdf_obj *object)
{
  if (object == NULL)
    return;
  if (next_label > pdf_max_ind_objects) {
    pdf_max_ind_objects += IND_OBJECTS_ALLOC_SIZE;
    output_xref = RENEW (output_xref, pdf_max_ind_objects,
			 struct xref_entry);
  }
  if (object -> label == 0) {  /* Don't change label on an already labeled
				  object.  Ignore such calls */
    /* Save so we can lookup this object by its number */
    output_xref[next_label-1].pdf_obj = object;
    object -> label = next_label++;
    object -> generation = 0;
  }
}

/* This doesn't really copy the object, but allows 
   it to be used without fear that somebody else will free it */

pdf_obj *pdf_link_obj (pdf_obj *object)
{
  if (object == NULL)
    ERROR ("pdf_link_obj passed null pointer");
  object -> refcount += 1;
  return object;
}


pdf_obj *pdf_ref_obj(pdf_obj *object)
{
  pdf_obj *result;
  pdf_indirect *indirect;
  
  if (object == NULL)
    ERROR ("pdf_ref_obj passed null pointer");
  
  if (object -> refcount == 0) {
    fprintf (stderr, "\npdf_ref_obj:  Called with already released object");
    pdf_write_obj (stderr, object);
    ERROR ("Fatal Error\n");
  }
  result = pdf_new_obj (PDF_INDIRECT);
  indirect = NEW (1, pdf_indirect);
  result -> data = indirect;
  if (object -> type == PDF_INDIRECT) { /* If an object is already an indirect reference,
					   reference the original
					   object, not the indirect
					   one */
    indirect -> label = ((pdf_indirect *) (object -> data)) -> label;
    indirect -> generation = ((pdf_indirect *) (object -> data)) -> generation;
    indirect -> dirty = ((pdf_indirect *) (object -> data)) -> dirty;
    indirect -> dirty_file = ((pdf_indirect *) (object -> data)) -> dirty_file;
  } else {
    if (object -> label == 0) {
      pdf_label_obj (object);
    }
    indirect -> label = object -> label;
    indirect -> generation = object -> generation;
    indirect -> dirty = 0;
    indirect -> dirty_file = NULL;
  }
  return result;
}

static void release_indirect (pdf_indirect *data)
{
  RELEASE (data);
}

static void write_indirect (FILE *file, const pdf_indirect *indirect)
{
  int length;
#ifdef MEM_DEBUG
MEM_START
#endif
  if (indirect -> dirty) {
    if (file == stderr) {
      pdf_out (file, "{d}", 3);
      sprintf (format_buffer, "%d %d R", indirect -> label,
	       indirect -> generation);
      length = strlen (format_buffer);
      pdf_out (file, format_buffer, length);
    }
    else {
      pdf_obj *clean;
      if (indirect -> dirty_file != pdf_input_file) {
	fprintf (stderr, "\nwrite_indirect, label=%d, from_file=%p, current_file=%p\n", indirect -> label, indirect->dirty_file, pdf_input_file);
	ERROR ("write_indirect:  input PDF file doesn't match object");
      }
      clean = pdf_ref_file_obj (indirect -> label);
      pdf_write_obj (file, clean);
      pdf_release_obj (clean);
    }
  } else {
    sprintf (format_buffer, "%d %d R", indirect -> label,
	     indirect -> generation);
    length = strlen (format_buffer);
    pdf_out (file, format_buffer, length);
  }
#ifdef MEM_DEBUG
MEM_END
#endif
}

pdf_obj *pdf_new_null (void)
{
  pdf_obj *result;
  result = pdf_new_obj (PDF_NULL);
  result -> data = NULL;
  return result;
}

static void release_null (void *data)
{
  return;
}

static void write_null (FILE *file)
{
  pdf_out (file, "null", 4);
}

pdf_obj *pdf_new_boolean (char value)
{
  pdf_obj *result;
  pdf_boolean *data;
  result = pdf_new_obj (PDF_BOOLEAN);
  data = NEW (1, pdf_boolean);
  result -> data = data;
  data -> value = value;
  return result;
}

static void release_boolean (pdf_obj *data)
{
  RELEASE (data);
}

static void write_boolean (FILE *file, const pdf_boolean *data)
{
  if (data -> value) {
    pdf_out (file, "true", 4);
  }
  else {
    pdf_out (file, "false", 5);
  }
}

void pdf_set_boolean (pdf_obj *object, char value)
{
   if (object == NULL || object -> type != PDF_BOOLEAN) {
     ERROR ("pdf_set_boolean:  Passed non-boolean object");
   }
   ((pdf_boolean *) (object -> data)) -> value = value;
}

pdf_obj *pdf_new_number (double value)
{
  pdf_obj *result;
  pdf_number *data;
  result = pdf_new_obj (PDF_NUMBER);
  data = NEW (1, pdf_number);
  result -> data = data;
  data -> value = value;
  return result;
}

static void release_number (pdf_number *data)
{
  RELEASE (data);
}

static void write_number (FILE *file, const pdf_number *number)
{
  int count;
  sprintf (format_buffer, "%.10g", number -> value);
  count = strlen (format_buffer);
  pdf_out (file, format_buffer, count);
}


void pdf_set_number (pdf_obj *object, double value)
{
   if (object == NULL || object -> type != PDF_NUMBER) {
     ERROR ("pdf_set_number:  Passed non-number object");
   }
   ((pdf_number *) (object -> data)) -> value = value;
}

double pdf_number_value (pdf_obj *object)
{
  if (object == NULL || object -> type != PDF_NUMBER) {
    ERROR ("pdf_obj_number_value:  Passed non-number object");
  }
  return ((pdf_number *)(object -> data)) -> value;
}

pdf_obj *pdf_new_string (const void *string, unsigned length)
{
  pdf_obj *result;
  pdf_string *data;
  result = pdf_new_obj (PDF_STRING);
  data = NEW (1, pdf_string);
  result -> data = data;
  if (length != 0) {
    data -> length = length;
    data -> string = NEW (length+1, unsigned char);
    memcpy (data -> string, string, length);
    data -> string[length] = 0;
  } else {
    data -> length = 0;
    data -> string = NULL;
  }
  return result;
}

void *pdf_string_value (pdf_obj *a_pdf_string)
{
  pdf_string *data;
  data = a_pdf_string -> data;
  return data -> string;
}

unsigned int pdf_string_length (pdf_obj *a_pdf_string)
{
  pdf_string *data;
  data = a_pdf_string -> data;
  return (data -> length);
}

/* This routine escapes non printable characters and control
   characters in an output string.  It optionally remaps
   the problem characters in the encoding */

int pdfobj_escape_str (char *buffer, int bufsize, unsigned char *s,
		       int len, int remap)
{
  int result = 0, i;
  unsigned char ch;
  for (i=0; i<len; i++) {
    ch = remap? twiddle(s[i]): s[i];
    /* Exit as fast as possible for printable characters */
    if (result+4 > bufsize)
      ERROR ("pdfobj_escape_str: Buffer overflow");
    if (ch < 32 || ch > 126) {
      buffer[result++] = '\\';
      sprintf (buffer+result, "%03o", ch);
      result += strlen (buffer+result);
      continue;
    }
    switch (ch) {
    case '(':
      buffer[result++] = '\\';
      buffer[result++] = '(';
      break;
    case ')':
      buffer[result++] = '\\';
      buffer[result++] = ')';
      break;
    case '\\':
      buffer[result++] = '\\';
      buffer[result++] = '\\';
      break;
    default:
      buffer[result++] = ch;
      break;
    }
  }
  return result;
}


static void write_string (FILE *file, const pdf_string *string)
{
  unsigned char *s = string -> string;
  int count, i;
  pdf_out_char (file, '(');
    /* This section of code probably isn't speed critical.  Escaping the
     characters in the string one at a time may seem slow, but it's
     safe if the formatted string length exceeds FORMAT_BUF_SIZE.
     Occasionally you see some long strings in PDF.  pdfobj_escape_str
     is also used for strings of text with no kerning.  These must be
     handled as quickly as possible since there are so many of them.  */ 
  for (i=0; i<string->length; i++) {
    count = pdfobj_escape_str (format_buffer, FORMAT_BUF_SIZE, s+i,
			       1, 0);
    pdf_out (file, format_buffer, count);
  }
  pdf_out_char (file, ')');
}

static void release_string (pdf_string *data)
{
  if (data -> string != NULL)
    RELEASE (data -> string);
  RELEASE (data);
}

void pdf_set_string (pdf_obj *object, unsigned char *string, unsigned length)
{
  pdf_string *data;
  if (object == NULL || object -> type != PDF_STRING) {
     ERROR ("pdf_set_string:  Passed non-string object");
  }
  data = object -> data;
  if (data -> length != 0) {
    RELEASE (data -> string);
  }
  if (length != 0) {
    data -> length = length;
    data -> string = NEW (length+1, unsigned char);
    memcpy (data -> string, string, length);
    (data->string)[length] = 0;
  } else {
    data -> length = 0;
    data -> string = NULL;
  }
  return;
}

int pdf_check_name(const char *name)
{
  static char *valid_chars =
    "!\"$&'*+,-.0123456789:;=?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\^_`abcdefghijklmnopqrstuvwxyz|~";
  if (strspn (name, valid_chars) == strlen (name))
    return 1;
  else
    return 0;
}

pdf_obj *pdf_new_name (const char *name)  /* name does *not* include the / */ 
{
  pdf_obj *result;
  unsigned length = strlen (name);
  pdf_name *data;
  if (!pdf_check_name (name)) {
    fprintf (stderr, "Invalid PDF name \"%s\"\n", name);
    ERROR ("pdf_new_name:  invalid PDF name");
  }
  result = pdf_new_obj (PDF_NAME);
  data = NEW (1, pdf_name);
  result -> data = data;
  if (length != 0) {
    data -> name = NEW (length+1, char);
    memcpy (data -> name, name, length);
    (data->name)[length] = 0;
  } else 
    data -> name = NULL;
  return result;
}

static void write_name (FILE *file, const pdf_name *name)
{
  char *s = name -> name;
  int i, length;
  pdf_out_char (file, '/');
  if (name -> name == NULL)
    length = 0;
  else
    length = strlen (name -> name);
  for (i=0; i < length; i++) {
    if (isprint (s[i]) &&
	s[i] != '/' &&
	s[i] != '%' &&
	s[i] != '(' &&
	s[i] != ')' &&
	s[i] != '[' && 
	s[i] != ']' && 
	s[i] != '#')
      pdf_out_char (file, s[i]);
  }
}


static void release_name (pdf_name *data)
{
  if (data -> name != NULL)
    RELEASE (data -> name);
  RELEASE (data);
}

void pdf_set_name (pdf_obj *object, char *name)
{
  pdf_name *data;
  unsigned length = strlen (name);
  if (object == NULL || object -> type != PDF_NAME) {
     ERROR ("pdf_set_name:  Passed non-name object");
  }
  data = object -> data;
  if (data -> name != NULL) {
    RELEASE (data -> name);
  }
  if (length != 0) {
    data -> name = NEW (length+1, char);
    memcpy (data -> name, name, length);
    (data->name)[length] = 0;
  } else {
    data -> name = NULL;
  }
}

char *pdf_name_value (pdf_obj *object)
{
  pdf_name *data;
  if (object == NULL || object -> type != PDF_NAME) {
     ERROR ("pdf_name_value:  Passed non-name object");
  }
  data = object -> data;
  if (data -> name == NULL)
    return NULL;
  return data -> name;
}


pdf_obj *pdf_new_array (void)
{
  pdf_obj *result;
  pdf_array *data;
  result = pdf_new_obj (PDF_ARRAY);
  data = NEW (1, pdf_array);
  data -> values = NULL;
  data -> max = 0;
  data -> size = 0;
  result -> data = data;
  return result;
}

static void write_array (FILE *file, const pdf_array *array)
{
  if (array -> size > 0) {
    unsigned long i;
    pdf_out_char (file, '[');
    for (i=0; i<array->size; i++) {
      if (i != 0 &&
	  ((array->values)[i]->type) != PDF_STRING &&
	  ((array->values)[i]->type) != PDF_NAME &&
	  ((array->values)[i]->type) != PDF_ARRAY)
	pdf_out_white (file);
      pdf_write_obj (file, (array->values)[i]);
    }
    pdf_out_char (file, ']');
  } else {
    write_null (file);
  }
}

pdf_obj *pdf_get_array (pdf_obj *array, unsigned long index)
{
  pdf_array *data;
  pdf_obj *result = NULL;
  if (array == NULL) {
    ERROR ("pdf_get_array: passed NULL object");
  }
  if (array -> type != PDF_ARRAY) {
    ERROR ("pdf_get_array: passed non array object");
  }
  data = array -> data;
  if (index < data -> size) {
    result = (data->values)[index];
  }
  return result;
}

static void release_array (pdf_array *data)
{
  unsigned long i;
  for (i=0; i<data->size; i++) {
    pdf_release_obj ((data ->values)[i]);
  }
  if (data->size > 0)
    RELEASE (data->values);
  RELEASE (data);
}

void pdf_add_array (pdf_obj *array, pdf_obj *object) /* Array is ended
							by a node with NULL
							this pointer */
{
  pdf_array *data;
  if (array == NULL || array -> type != PDF_ARRAY) {
     ERROR ("pdf_add_array:  Passed non-array object");
  }
  data = array -> data;
  if (data -> size >= data -> max) {
    data->max += ARRAY_ALLOC_SIZE;
    data->values = RENEW (data->values, data->max, pdf_obj *);
  }
  (data->values)[data->size++] = object;
  return;
}


static void write_dict (FILE *file, const pdf_dict *dict)
{
  pdf_out (file, "<<\n", 3);
  while (dict -> key != NULL) {
    pdf_write_obj (file, dict -> key);
    if (((dict -> value) -> type) == PDF_BOOLEAN ||
	((dict -> value) -> type) == PDF_NUMBER ||
	((dict -> value) -> type) == PDF_INDIRECT ||
	((dict -> value) -> type) == PDF_NULL)
	pdf_out_white (file);
    pdf_write_obj (file, dict -> value);
    dict = dict -> next;
    pdf_out_char (file, '\n');
  }
  pdf_out (file, ">>", 2);
}

pdf_obj *pdf_new_dict (void)
{
  pdf_obj *result;
  pdf_dict *data;
  result = pdf_new_obj (PDF_DICT);
  data = NEW (1, pdf_dict);
  data -> key = NULL;
  data -> value = NULL;
  data -> next = NULL;
  result -> data = data;
  return result;
}

static void release_dict (pdf_dict *data)
{
  pdf_dict *next;
  while (data != NULL && data -> key != NULL) {
    pdf_release_obj (data -> key);
    pdf_release_obj (data -> value);
    next = data -> next;
    RELEASE (data);
    data = next;
  }
  RELEASE (data);
}

void pdf_add_dict (pdf_obj *dict, pdf_obj *key, pdf_obj *value) /* Array is ended
								   by a node with NULL
								   this pointer */
{
  pdf_dict *data;
  pdf_dict *new_node;
  if (key == NULL || key -> type != PDF_NAME ) {
    ERROR ("pdf_add_dict: Passed invalid key");
  }
  if (value != NULL &&
      (value -> type == 0 || value -> type > PDF_INDIRECT )) {
    ERROR ("pdf_add_dict: Passed invalid value");
  }
  if (dict == NULL || dict -> type != PDF_DICT) {
    fprintf (stderr, "key:");
    pdf_write_obj (stderr, key);
    fprintf (stderr, "value:");
    pdf_write_obj (stderr, value);
    ERROR ("pdf_add_dict:  Passed non-dict object");
  }
  data = dict -> data;
  /* If this key already exists, simply replace the value */
  while (data -> key != NULL) {
    if (!strcmp (pdf_name_value(key), pdf_name_value(data->key))) {
      /* Release the old value */
      pdf_release_obj (data->value);
      /* Release the new key (we don't need it) */
      pdf_release_obj (key);
      data->value = value;
      break;
    }
    data = data -> next;
  }
  /* If we didn't find the key, build a new "end" node and add
     the new key just before the end */
  if (data -> key == NULL) {
    new_node = NEW (1, pdf_dict);
    new_node -> key = NULL;
    new_node -> value = NULL;
    new_node -> next = NULL;
    data -> next = new_node;
    data -> key = key;
    data -> value = value;
  }
  return;
}

/* pdf_merge_dict makes a link for each item in dict2 before
   stealing it */
void pdf_merge_dict (pdf_obj *dict1, pdf_obj *dict2)
{
  pdf_dict *data;
  if (dict1 == NULL || dict1 -> type != PDF_DICT) 
    ERROR ("pdf_merge_dict:  Passed invalid first dictionary");
  if (dict2 == NULL || dict2 -> type != PDF_DICT)
    ERROR ("pdf_merge_dict:  Passed invalid second dictionary");
  data = dict2 -> data;
  while (data -> key != NULL) {
    pdf_add_dict (dict1, pdf_link_obj(data -> key),
		  pdf_link_obj (data -> value));
    data = data -> next;
  }
}

static int pdf_match_name (const pdf_obj *name_obj, const char *name_string)
{
  pdf_name *data;
  data = name_obj -> data;
  return (!strcmp (data -> name, name_string));
}

pdf_obj *pdf_lookup_dict (const pdf_obj *dict, const char *name)
{
  pdf_dict *data;
  if (dict == NULL || dict ->type != PDF_DICT) 
    ERROR ("pdf_lookup_dict:  Passed invalid dictionary");
  data = dict -> data;
  while (data -> key != NULL) {
    if (pdf_match_name (data -> key, name))
      return data -> value;
    data = data -> next;
  }
  return NULL;
}

void pdf_remove_dict (pdf_obj *dict, const char *name)
{
  pdf_dict *data, **data_p;
  if (dict == NULL || dict->type != PDF_DICT)
    ERROR ("pdf_remove_dict:  Passed invalid dictionary");
  data = (dict -> data);
  data_p = (pdf_dict **) &(dict->data);
  while (data->key != NULL) {
    if (pdf_match_name (data -> key, name)) {
      pdf_release_obj (data -> key);
      pdf_release_obj (data -> value);
      *data_p = data -> next;
      RELEASE (data);
      break;
    }
    data_p = &(data->next);
    data = data -> next;
  }
  return;
}

char *pdf_get_dict (const pdf_obj *dict, int index)
{
  pdf_dict *data;
  char *result;
  if (dict == NULL) {
    ERROR ("pdf_get_dict: passed NULL object");
  }
  if (dict -> type != PDF_DICT) {
    ERROR ("pdf_get_dict: passed non array object");
  }
  data = dict -> data;
  while (index-- > 0 && data -> next != NULL)
    data = data -> next;
  if (data -> next == NULL)
    return NULL;
  result = pdf_name_value (data -> key);
  return result;
}

pdf_obj *pdf_new_stream (int flags)
{
  pdf_obj *result;
#ifdef HAVE_ZLIB  
  pdf_obj *filters = NULL;
#endif /* HAVE_ZLIB */
  pdf_stream *data;
  result = pdf_new_obj (PDF_STREAM);
  data = NEW (1, pdf_stream);
  result -> data = data;
  data -> dict = pdf_new_dict ();  /* Although we are using an arbitrary
				      pdf_object here, it must have
				      type=PDF_DICT and cannot be an
				      indirect reference.  This will
				      be checked by the output routine 
				   */
  data -> _flags = flags;
#ifdef HAVE_ZLIB
  if ((flags & STREAM_COMPRESS) && compression_level > 0) {
    if (!filters) {
      filters = pdf_new_array();
      pdf_add_dict (data -> dict, pdf_new_name ("Filter"), filters);
    }
    pdf_add_array (filters, pdf_new_name ("FlateDecode"));
  }
#endif /* HAVE_ZLIB */  

  data -> stream_length = 0;
  data -> max_length = 0;
  data -> stream = NULL;
  return result;
}


static void write_stream (FILE *file, pdf_stream *stream)
{
#define COMPRESS_LEVEL 9
#define THRESHOLD 100
  unsigned char *filtered;
  unsigned long filtered_length;
#ifdef HAVE_ZLIB
  unsigned long buffer_length;
  unsigned char *buffer;
#endif /* HAVE_ZLIB */
  /* Always work from a copy of the stream */
  /* All filters read from "filtered" and leave their result in
     "filtered" */
  filtered = NEW (stream->stream_length+1, unsigned char);
  memcpy (filtered, stream->stream, stream->stream_length);
  filtered_length = stream->stream_length;

#ifdef HAVE_ZLIB
  /* Apply compression filter if requested */
  if ((stream -> _flags & STREAM_COMPRESS) && compression_level > 0) {
    buffer_length = filtered_length + filtered_length/1000 + 14;
    buffer = NEW (buffer_length, unsigned char);
#ifdef HAVE_ZLIB_COMPRESS2    
    if (compress2 (buffer, &buffer_length, filtered,
		   filtered_length, compression_level))
      ERROR ("Zlib error");
#else 
    if (compress (buffer, &buffer_length, filtered,
		  filtered_length))
      ERROR ("Zlib error");
#endif /* HAVE_ZLIB_COMPRESS2 */
    RELEASE (filtered);
    filtered = buffer;
    compression_saved += (filtered_length-buffer_length)-strlen("/Filter [/FlateDecode]\n");
    filtered_length = buffer_length;
  }
#endif /* HAVE_ZLIB */
  /* Add a '\n' if the last character wasn't one */
  if (filtered_length > 0 && filtered[filtered_length-1] != '\n') {
    filtered[filtered_length++] = '\n';
  }
  pdf_add_dict (stream->dict,
		pdf_new_name ("Length"),
		pdf_new_number(filtered_length));
  pdf_write_obj (file, stream -> dict);
  pdf_out (file, "\nstream\n", 8);
  
  if (filtered_length > 0) {
    pdf_out (file, filtered, filtered_length);
  }

  RELEASE (filtered);
  /* This stream length "object" gets reset every time write_stream is
     called for the stream object */
  /* If this stream gets written more than once with different
     filters, this could be a problem */
  pdf_out (file, "endstream", 9);
  return;
}

static void release_stream (pdf_stream *stream)
{
  pdf_release_obj (stream -> dict);
  if (stream -> stream_length > 0)
    RELEASE (stream -> stream);
  RELEASE (stream);
}

pdf_obj *pdf_stream_dict (pdf_obj *stream)
{
  pdf_stream *data;
  if (stream == NULL || stream -> type != PDF_STREAM) {
     ERROR ("pdf_stream_dict:  Passed non-stream object");
  }
  data = stream -> data;
  return data -> dict;
}

void pdf_add_stream (pdf_obj *stream, char *stream_data, unsigned length)
{
  pdf_stream *data;
  if (stream == NULL || stream -> type != PDF_STREAM) {
     ERROR ("pdf_add_stream:  Passed non-stream object");
  }
  if (length == 0)
    return;
  data = stream -> data;
  if (data -> stream_length + length > data -> max_length) {
    data -> max_length += length + STREAM_ALLOC_SIZE;
    data -> stream = RENEW (data -> stream, data->max_length, char);
  }
  memcpy ((data->stream)+(data->stream_length), stream_data,
	  length);
  data->stream_length += length;
}

void pdf_write_obj (FILE *file, const pdf_obj *object)
{
  if (object == NULL) {
    write_null(file);
    return;
  }
  if (object -> type > PDF_INDIRECT) {
    fprintf (stderr, "Object type = %d\n", object -> type);
    ERROR ("pdf_write_obj:  Called with invalid object");
  }
  if (file == stderr)
    fprintf (stderr, "{%d}", object -> refcount);
  switch (object -> type) {
  case PDF_BOOLEAN:
    write_boolean (file, object -> data);
    break;
  case PDF_NUMBER:
    write_number (file, object -> data);
    break;
  case PDF_STRING:
    write_string (file, object -> data);
    break;
  case PDF_NAME:
    write_name (file, object -> data);
    break;
  case PDF_ARRAY:
    write_array (file, object -> data);
    break;
  case PDF_DICT:
    write_dict (file, object -> data);
    break;
  case PDF_STREAM:
    write_stream (file, object -> data);
    break;
  case PDF_NULL:
    write_null (file);
    break;
  case PDF_INDIRECT:
    write_indirect (file, object -> data);
    break;
  }
}

static void pdf_flush_obj (FILE *file, const pdf_obj *object) 
     /* Write the object to the file */ 
{
  int length;
  /* Record file position.  No object is numbered 0, so subtract 1
     when using as an array index */
  output_xref[object->label-1].file_position = pdf_output_file_position;
  sprintf (format_buffer, "%lu %d obj\n", object -> label ,
	   object -> generation);
  length = strlen (format_buffer);
  pdf_out (file, format_buffer, length);
  pdf_write_obj (file, object);
  pdf_out (file, "\nendobj\n", 8);
}


void pdf_release_obj (pdf_obj *object)
{
  if (object == NULL)
    return;
  if (object -> type > PDF_INDIRECT ||
      object -> refcount <= 0) {
    fprintf (stderr, "pdf_release_obj: object = %p, type = %d\n", object, object ->
	     type);
    pdf_write_obj (stderr, object);
    ERROR ("pdf_release_obj:  Called with invalid object");
  }
  object -> refcount -= 1;
    if (object -> refcount == 0) { /* Nothing is using this object so it's okay to
				    remove it */
    /* Nonzero "label" means object needs to be written before it's destroyed*/
    if (object -> label && pdf_output_file != NULL) { 
      pdf_flush_obj (pdf_output_file, object);
    }
    switch (object -> type) {
    case PDF_BOOLEAN:
      release_boolean (object -> data);
      break;
    case PDF_NULL:
      release_null (object -> data);
      break;
    case PDF_NUMBER:
      release_number (object -> data);
      break;
    case PDF_STRING:
      release_string (object -> data);
      break;
    case PDF_NAME:
      release_name (object -> data);
      break;
    case PDF_ARRAY:
      release_array (object -> data);
      break;
    case PDF_DICT:
      release_dict (object -> data);
      break;
    case PDF_STREAM:
      release_stream (object -> data);
      break;
    case PDF_INDIRECT:
      release_indirect (object -> data);
      break;
    }
  /* This might help detect freeing already freed objects */
    /*  object -> type = -1;*/
    RELEASE (object);
  }
}

static int backup_line (void)
{
  int ch;
  ch = -1;
  if (debug) {
    fprintf (stderr, "\nbackup_line:\n");
  }
  /* Note: this code should work even if \r\n is eol.
     It could fail on a machine where \n is eol and
     there is a \r in the stream---Highly unlikely
     in the last few bytes where this is likely to be used.
  */
  if (tell_position (pdf_input_file) > 1)
    do {
      seek_relative (pdf_input_file, -2);
      if (debug)
	fprintf (stderr, "%c", ch);
    } while (tell_position (pdf_input_file) > 0 &&
	     (ch = fgetc (pdf_input_file)) >= 0 &&
	     (ch != '\n' && ch != '\r' ));
  if (debug)
    fprintf (stderr, "<-\n");
  if (ch < 0) {
    return 0;
  }
  return 1;
}

static unsigned long pdf_file_size = 0;

static long find_xref(void)
{
  long currentpos, xref_pos;
  int tries = 10;
  char *start, *end, *number;
  if (debug)
    fprintf (stderr, "(find_xref");
  seek_end (pdf_input_file);
  pdf_file_size = tell_position (pdf_input_file);
  do {
    if (!backup_line()) {
      tries = 0;
      break;
    }
    currentpos = tell_position(pdf_input_file);
    fread (work_buffer, sizeof(char), strlen("startxref"),
	   pdf_input_file);
    if (debug) {
      work_buffer[strlen("startxref")] = 0;
      fprintf (stderr, "[%s]\n", work_buffer);
    }
    seek_absolute(pdf_input_file, currentpos);
    tries--;
  } while (tries > 0 && strncmp (work_buffer, "startxref", strlen ("startxref")));
  if (tries <= 0)
    return 0;
  /* Skip rest of this line */
  mfgets (work_buffer, WORK_BUFFER_SIZE, pdf_input_file);
  /* Next line of input file should contain actual xref location */
  mfgets (work_buffer, WORK_BUFFER_SIZE, pdf_input_file);
  if (debug) {
    fprintf (stderr, "\n->[%s]<-\n", work_buffer);
  }
  start = work_buffer;
  end = start+strlen(work_buffer);
  skip_white(&start, end);
  xref_pos = (long) atof (number = parse_number (&start, end));
  RELEASE (number);
  if (debug) {
    fprintf (stderr, ")\n");
    fprintf (stderr, "xref @ %lu\n", xref_pos);
  }
  return xref_pos;
}

pdf_obj *parse_trailer (void)
{
  char *start;
#ifdef MEM_DEBUG
MEM_START
#endif
  /* This routine must be called with the file pointer located at
     the start of the trailer */
  /* Fill work_buffer and hope trailer fits.  This should
     be made a bit more robust sometime */
  if (fread (work_buffer, sizeof(char), WORK_BUFFER_SIZE,
	     pdf_input_file) == 0 ||
      strncmp (work_buffer, "trailer", strlen("trailer"))) {
    fprintf (stderr, "No trailer.  Are you sure this is a PDF file?\n");
    fprintf (stderr, "\nbuffer:\n->%s<-\n", work_buffer);
    return NULL;
  }
  start = work_buffer + strlen("trailer");
  skip_white(&start, work_buffer+WORK_BUFFER_SIZE);
#ifdef MEM_DEBUG
MEM_END
#endif
  return (parse_pdf_dict (&start, work_buffer+WORK_BUFFER_SIZE));
}

struct object 
{
  unsigned long position;
  unsigned generation;
  /* Object numbers in original file and new file must have different
     object numbers.
     new_ref provides a reference for the object in the new file
     object space.  When it is first set, an object in the old file
     is copied to the new file with a new number.  new_ref remains set
     until the file is closed so that future references can access the
     object via new_ref instead of copying the object again */
  pdf_obj *direct;
  pdf_obj *indirect;
  int used;
} *xref_table = NULL;
long num_input_objects;

long next_object (unsigned long obj)
{
  /* routine tries to estimate an upper bound for character position
     of the end of the object, so it knows how big the buffer must be.
     The parsing routines require that the entire object be read into
     memory. It would be a major pain to rewrite them.  The worst case
     is that an object before an xref table will grab the whole table
     :-( */
  unsigned long i;
  long this_position, result = pdf_file_size;  /* Worst case */
  this_position = xref_table[obj].position;
  /* Check all other objects to find next one */
  for (i=0; i<num_input_objects; i++) {
    if ((xref_table[i].used) &&
	xref_table[i].position > this_position &&
	xref_table[i].position < result)
      result = xref_table[i].position;
  }
  return result;
}

/* The following routine returns a reference to an object existing
   only in the input file.  It does this as follows.  If the object
   has never been referenced before, it reads the object
   in and creates a reference to it.  Then it writes
   the object out, keeping the existing reference. If the
   object has been read in (and written out) before, it simply
   returns the retained existing reference to that object */

pdf_obj *pdf_ref_file_obj (unsigned long obj_no)
{
  pdf_obj *direct, *indirect;
#ifdef MEM_DEBUG
MEM_START
#endif
  if (obj_no >= num_input_objects) {
    fprintf (stderr, "\n\npdf_ref_file_obj: nonexistent object\n");
    return NULL;
  }
  if (xref_table[obj_no].indirect != NULL) {
    return pdf_link_obj(xref_table[obj_no].indirect);
  }
  if ((direct = pdf_read_object (obj_no)) == NULL) {
    fprintf (stderr, "\npdf_ref_file_obj: Could not read object\n");
    return NULL;
  }
  indirect = pdf_ref_obj (direct);
  xref_table[obj_no].indirect = indirect;
  xref_table[obj_no].direct = direct;
  /* Make sure the caller can't doesn't free this object */
#ifdef MEM_DEBUG
MEM_END
#endif
  return pdf_link_obj(indirect);
}


pdf_obj *pdf_new_ref (unsigned long label, int generation) 
{
  pdf_obj *result;
  pdf_indirect *indirect;
  if (label >= num_input_objects || label < 0) {
    fprintf (stderr, "pdf_new_ref: Object doesn't exist\n");
    return NULL;
  }
  result = pdf_new_obj (PDF_INDIRECT);
  indirect = NEW (1, pdf_indirect);
  result -> data = indirect;
  indirect -> label = label;
  indirect -> generation = generation;
  indirect -> dirty = 1;
  indirect -> dirty_file = pdf_input_file;
  return result;
}

pdf_obj *pdf_read_object (unsigned long obj_no) 
{
  long start_pos, end_pos;
  char *buffer, *number, *parse_pointer, *end;
  pdf_obj *result;
#ifdef MEM_DEBUG
MEM_START
#endif
  if (debug) {
    fprintf (stderr, "\nread_object: obj=%lu\n", obj_no);
  }
  if (obj_no < 0 || obj_no >= num_input_objects) {
    fprintf (stderr, "\nTrying to read nonexistent object\n");
    return NULL;
  }
  if (!xref_table[obj_no].used) {
    fprintf (stderr, "\nTrying to read deleted object\n");
    return NULL;
  }
  if (debug) {
    fprintf (stderr, "\nobj@%lu\n", xref_table[obj_no].position);
  }
  seek_absolute (pdf_input_file, start_pos =
		 xref_table[obj_no].position);
  end_pos = next_object (obj_no);
  if (debug) {
    fprintf (stderr, "\nendobj@%lu\n", end_pos);
  }
  buffer = NEW (end_pos - start_pos+1, char);
  fread (buffer, sizeof(char), end_pos-start_pos, pdf_input_file);
  buffer[end_pos-start_pos] = 0;
  if (debug) {
    fprintf (stderr, "\nobject:\n%s", buffer);
  }
  parse_pointer = buffer;
  end = buffer+(end_pos-start_pos);
  skip_white (&parse_pointer, end);
  number = parse_number (&parse_pointer, end);
  if ((int) atof(number) != obj_no) {
    fprintf (stderr, "Object number doesn't match\n");
    RELEASE (buffer);
    return NULL;
  }
  if (number != NULL)
    RELEASE(number);
  skip_white (&parse_pointer, end);
  number = parse_number (&parse_pointer, end);
  if (number != NULL)
    RELEASE(number);
  skip_white(&parse_pointer, end);
  if (strncmp(parse_pointer, "obj", strlen("obj"))) {
    fprintf (stderr, "Didn't find \"obj\"\n");
    RELEASE (buffer);
    return (NULL);
  }
  parse_pointer += strlen("obj");
  result = parse_pdf_object (&parse_pointer, end);
  skip_white (&parse_pointer, end);
  if (strncmp(parse_pointer, "endobj", strlen("endobj"))) {
    fprintf (stderr, "Didn't find \"endobj\"\n");
    if (result != NULL)
      pdf_release_obj (result);
    result = NULL;
  }
  RELEASE (buffer);
  return (result);
#ifdef MEM_DEBUG
MEM_END
#endif
}
/* pdf_deref_obj always returns a link instead of the original */ 
pdf_obj *pdf_deref_obj (pdf_obj *obj)
{
  pdf_obj *result, *tmp;
  pdf_indirect *indirect;
  if (obj == NULL)
    return NULL;
  if (obj -> type != PDF_INDIRECT) {
    return pdf_link_obj (obj);
  }
  indirect = obj -> data;
  if (!(indirect -> dirty)) {
    ERROR ("Tried to deref a non-file object");
  }
  result = pdf_read_object (indirect -> label);

  if (debug){
    fprintf (stderr, "\npdf_deref_obj: read_object returned\n");
    pdf_write_obj (stderr, result);
  }
  
  while (result && result -> type == PDF_INDIRECT) {
    tmp = pdf_read_object (result -> label);
    pdf_release_obj (result);
    result = tmp;
  }
  return result;
}

/* extends the xref table if we get another segment
   with higher object numbers than the current object */
static void extend_xref (long new_size) 
{
  unsigned long i;
  xref_table = RENEW (xref_table, new_size,
		      struct object);
  for (i=num_input_objects; i<new_size; i++) {
    xref_table[i].direct = NULL;
    xref_table[i].indirect = NULL;
    xref_table[i].used = 0;
    xref_table[i].position = 0;
  }
  num_input_objects = new_size;
}



static int parse_xref (void)
{
  unsigned long first_obj, num_table_objects;
  unsigned long i;
  /* This routine reads one xref segment.  It must be called
     positioned at the beginning of an xref table.  It may be called
     multiple times on the same file.  xref tables sometimes come in
     pieces */
  mfgets (work_buffer, WORK_BUFFER_SIZE, pdf_input_file);
  if (strncmp (work_buffer, "xref", strlen("xref"))) {
    fprintf (stderr, "No xref.  Are you sure this is a PDF file?\n");
    return 0;
  }
  /* Next line in file has first item and size of table */
  for (;;) {
    unsigned long current_pos;
    current_pos = tell_position (pdf_input_file);
    if (mfgets (work_buffer, WORK_BUFFER_SIZE, pdf_input_file) ==
	NULL)
      ERROR ("parse_xref: premature end of PDF file while parsing xref");
    if (!strncmp (work_buffer, "trailer", strlen ("trailer"))) {
      /* Backup... This is ugly, but it seems like the safest thing to
	 do.  It is possible the trailer dictionary starts on the same
	 logical line as the word trailer.  In that case, the mfgets
	 call might have started to read the trailer dictionary and
	 parse_trailer would fail */
      seek_absolute (pdf_input_file, current_pos);
      break;
    }
    sscanf (work_buffer, "%lu %lu", &first_obj, &num_table_objects);
    if (num_input_objects < first_obj+num_table_objects) {
      extend_xref (first_obj+num_table_objects);
    }
    if (debug) {
      fprintf (stderr, "\nfirstobj=%lu,number=%lu\n",
	       first_obj,num_table_objects);
    }
    for (i=first_obj; i<first_obj+num_table_objects; i++) {
      fread (work_buffer, sizeof(char), 20, pdf_input_file);
      /* Don't overwrite positions that have already been set by a
	 modified xref table.  We are working our way backwards
	 through the reference table, so we only set "position" 
	 if it hasn't been set yet. */
      if (xref_table[i].position == 0) {
	work_buffer[19] = 0;
	sscanf (work_buffer, "%lu %u", &(xref_table[i].position), 
		&(xref_table[i].generation));
      }
      if (debug) {
	fprintf (stderr, "pos: %lu gen: %u\n", xref_table[i].position,
		 xref_table[i].generation);
      }
      if (work_buffer[17] != 'n' && work_buffer[17] != 'f') {
	fprintf (stderr, "PDF file is corrupt\n");
	fprintf (stderr, "[%s]\n", work_buffer);
	return 0;
      }
      if (work_buffer[17] == 'n')
	xref_table[i].used = 1;
      else
	xref_table[i].used = 0;
      xref_table[i].direct = NULL;
      xref_table[i].indirect = NULL;
    }
  }
  return 1;
}

pdf_obj *read_xref (void)
{
  pdf_obj *main_trailer, *prev_trailer, *prev_xref, *xref_size;
  long xref_pos;
#ifdef MEM_DEBUG
MEM_START
#endif  
  if ((xref_pos = find_xref()) == 0) {
    fprintf (stderr, "Can't find xref table.\n");
    return NULL;
  }
  if (debug) {
    fprintf(stderr, "xref@%lu\n", xref_pos);
  }
  /* Read primary xref table */
  seek_absolute (pdf_input_file, xref_pos);
  if (!parse_xref()) {
    fprintf (stderr,
	     "\nCouldn't read xref table.  Is this a correct PDF file?\n");
    return NULL;
  }
  if ((main_trailer = parse_trailer()) == NULL) {
    fprintf (stderr,
	     "\nCouldn't read xref trailer.  Is this a correct PDF file?\n");
    return NULL;
  }
  if (pdf_lookup_dict (main_trailer, "Root") == NULL ||
      (xref_size = pdf_lookup_dict (main_trailer, "Size")) == NULL) {
    fprintf (stderr,
	     "\nTrailer doesn't have catalog or a size.  Is this a correct PDF file?\n");
    return NULL;
  }
  if (num_input_objects < pdf_number_value (xref_size)) {
    extend_xref (pdf_number_value (xref_size));
  }
  /* Read any additional xref tables */
  prev_trailer = pdf_link_obj (main_trailer);
  while ((prev_xref = pdf_lookup_dict (prev_trailer, "Prev")) != NULL) {
    xref_pos = pdf_number_value (prev_xref);
    seek_absolute (pdf_input_file, xref_pos);
    pdf_release_obj (prev_trailer);
    if (!parse_xref()) {
      fprintf (stderr,
	       "\nCouldn't read xref table.  Is this a correct PDF file?\n");
      return NULL;
    }
    if ((prev_trailer = parse_trailer()) == NULL) {
      fprintf (stderr,
	       "\nCouldn't read xref trailer.  Is this a correct PDF file?\n");
      return NULL;
    }
    if (debug) {
      fprintf (stderr, "\nprev_trailer:\n");
      pdf_write_obj (stderr, prev_trailer);
    }
  }
#ifdef MEM_DEBUG
MEM_END
#endif  
  pdf_release_obj (prev_trailer);
  return main_trailer;
}

static char any_open = 0;

pdf_obj *pdf_open (FILE *file)
{
  pdf_obj *trailer;
#ifdef MEM_DEBUG
MEM_START
#endif
  if (any_open) {
    fprintf (stderr, "\nOnly one PDF file may be open at one time.\n");
    any_open = 1;
    exit(1);
  }
  pdf_input_file = file;
  if (!check_for_pdf (pdf_input_file)) {
    fprintf (stderr, "pdf_open: Not a PDF 1.[1-3] file\n");
    return NULL;
  }
  if ((trailer = read_xref()) == NULL) {
    fprintf (stderr, "No trailer.\n");
    pdf_close ();
    return NULL;
  }
  if (debug) {
    fprintf (stderr, "\nDone with xref:\n");
  }
#ifdef MEM_DEBUG
  MEM_END
#endif
  return trailer;
}

void pdf_close (void)
{
  /* Following loop must be iterated because each write could trigger
     an additional indirect reference of an object with a lower
     number! */
  unsigned long i;
  int done;
  if (debug) {
    fprintf (stderr, "\npdf_close:\n");
    fprintf (stderr, "pdf_input_file=%p\n", pdf_input_file);
  }
  do {
    done = 1;
    for (i=0; i<num_input_objects; i++) {
      if (xref_table[i].direct != NULL) {
	pdf_release_obj (xref_table[i].direct);
	xref_table[i].direct = NULL;
	done = 0;
      }
    }
  } while (!done);
  /* Now take care of the indirect objects
     They had to be left around until all the direct
     objects were flushed */
  for (i=0; i<num_input_objects; i++) {
    if (xref_table[i].indirect != NULL) {
      pdf_release_obj (xref_table[i].indirect);
    }
  }
  RELEASE (xref_table);
  xref_table = NULL;
  num_input_objects = 0;
  any_open = 0;
  pdf_input_file = NULL;
  if (debug) {
    fprintf (stderr, "\nexiting pdf_close:\n");
  }
}

int check_for_pdf (FILE *file) 
{
  int result = 0;
  rewind (file);
  if (fread (work_buffer, sizeof(char), strlen("%PDF-1.x"), file) ==
      strlen("%PDF-1.x") &&
      !strncmp(work_buffer, "%PDF-1.", strlen("%PDF-1."))) {
    if (work_buffer[7] >= '0' && work_buffer[7] <= '0'+pdf_version)
      result = 1;
    else {
      fprintf (stderr, "\nVersion of PDF file (1.%c) is newer than version limit specification.\n", work_buffer[7]);
    }
  }
  return result;
}
