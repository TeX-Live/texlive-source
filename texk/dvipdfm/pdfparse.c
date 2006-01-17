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
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "system.h"
#include "mem.h"
#include "mfileio.h"
#include "numbers.h"
#include "dvi.h"
#include "pdfparse.h"
#include "pdfspecial.h"
#include "pdfobj.h"
#include "pdfdoc.h"
#include "pdfdev.h"

#define verbose 0
#define debug 0

#define DUMP_LIMIT 50
void dump(char *start, char *end)
{
  char *p = start;
  fprintf (stderr, "\nCurrent input buffer is ");
  fprintf (stderr, "-->");
  while (p < end && p < start+DUMP_LIMIT)
    fprintf (stderr, "%c", *(p++));
  if (p == start+DUMP_LIMIT)
    fprintf (stderr, "...\n");
  fprintf (stderr, "<--\n");
}

void skip_white (char **start, char *end)
{
  while (*start < end && (isspace (**start) || **start == '%')) {
    if (**start == '%') 
      skip_line (start, end);
    else /* Skip the white char  */
      (*start)++;
  }
  return;
}

void skip_line (char **start, char *end)
{
  /* Note: PDF spec says that all platforms must end line with '\n'
     after a "stream" keyword */
  while (*start < end && **start != '\n' && **start != '\r')
    (*start)++;
  if (*start < end && **start == '\r')
    (*start) += 1;
  if (*start < end && **start == '\n')
    (*start) += 1;
  return;
}

void parse_crap (char **start, char *end)
{
  skip_white(start, end);
  if (*start != end) {
    fprintf (stderr, "\nCrap left over after object!!\n");
    dump(*start, end);
  }
}

int is_an_int(const char *s)
{
  int i;
  for (i=0; i<strlen(s); i++) {
    if (i == 0 && s[i] == '-')
      continue;
    if (!isdigit (s[i]))
      return 0;
  }
  return 1;
}

int is_a_number(const char *s)
{
  int i, period = 0;
  for (i=0; i<strlen(s); i++) {
    if (s[i] == '-' && i == 0)
      continue;
    if (s[i] == '.' && !period) {
      period = 1;
      continue;
    }
    if (!isdigit (s[i]))
      return 0;
  }
  return 1;
}


pdf_obj *parse_pdf_dict (char **start, char *end)
{
  pdf_obj *result, *tmp1, *tmp2;
  char *save = *start;
  skip_white(start, end);
  if (*((*start)++) != '<' ||
      *((*start)++) != '<') {
    *start = save;
    dump (*start, end);
    return NULL;
  }
  result = pdf_new_dict ();
    skip_white(start, end);
  while (*start < end &&
	 **start != '>') {
    if ((tmp1 = parse_pdf_name (start, end)) == NULL) {
      pdf_release_obj (result); 
      {
	*start = save;
	dump (*start, end);
	return NULL;
      }
    };
    if ((tmp2 = parse_pdf_object (start, end)) == NULL) {
      pdf_release_obj (result);
      pdf_release_obj (tmp1); 
      {
	*start = save;
	dump (*start, end);
	return NULL;
      }
    }
    pdf_add_dict (result, tmp1, tmp2);
    skip_white(start, end);
  }
  if (*start >= end) {
    pdf_release_obj (result);
    *start = save;
    dump (*start, end);
    return NULL;
  }
  if (*((*start)++) == '>' &&
      *((*start)++) == '>') {
    return result;
  } else {
    pdf_release_obj (result);
    fprintf (stderr, "\nDictionary object ended prematurely\n");
    *start = save;
    dump (*start, end);
    return NULL;
  }
}

pdf_obj *parse_pdf_array (char **start, char *end)
{
  pdf_obj *result, *tmp1;
#ifdef MEM_DEBUG
MEM_START
#endif
  skip_white(start, end);
  if (*((*start)++) != '[')
    return NULL;
  result = pdf_new_array ();
  skip_white(start, end);
  while (*start < end &&
	 **start != ']') {
    if ((tmp1 = parse_pdf_object (start, end)) == NULL) {
      pdf_release_obj (result);
      return NULL;
    };
    pdf_add_array (result, tmp1);
    skip_white(start, end);
  }
  if (*start >= end) {
    pdf_release_obj (result);
    fprintf (stderr, "\nArray ended prematurely\n");
    return NULL;
  }
  (*start)++;
#ifdef MEM_DEBUG
MEM_END
#endif
  return result;
}

char *parse_number (char **start, char *end)
{
  char *number, *save;
#ifdef MEM_DEBUG
MEM_START
#endif
  skip_white(start, end);
  save = *start;
  if (*start < end && (**start == '+' || **start == '-')) {
    *start += 1;
  }
  while (*start < end &&
	 isdigit(**start))
    (*start)++;
  if (*start < end && **start == '.') {
    (*start)++;
    while (*start < end &&
	   isdigit(**start))
      (*start)++;
  }
  if (*start > save) {
    number = NEW ((*start-save)+1, char);
    memcpy (number, save, (*start-save));
    number[*start-save] = 0;
    return number;
  }
  *start = save;
#ifdef MEM_DEBUG
MEM_END
#endif
  return NULL;
}

char *parse_unsigned (char **start, char *end)
{
  char *number, *save;
#ifdef MEM_DEBUG
MEM_START
#endif
  skip_white(start, end);
  save = *start;
  while (*start < end &&
	 isdigit(**start))
    (*start)++;
  if (*start > save) {
    number = NEW ((*start-save)+1, char);
    memcpy (number, save, (*start-save));
    number[*start-save] = 0;
    return number;
  }
  *start = save;
#ifdef MEM_DEBUG
MEM_END
#endif
  return NULL;
}

static char *parse_gen_ident (char **start, char *end, char *valid_chars)
{
  char *ident, *save;
  save = *start;
  skip_white(start, end);
  while (*start < end && strchr (valid_chars, **start))
    (*start)++;
  if (save == *start)
    return NULL;
  ident = NEW (*start-save+1, char);
  memcpy (ident, save, *start-save);
  ident[*start-save] = 0;
  return ident;
}

char *parse_ident (char **start, char *end)
{
  static char *valid_chars =
    "!\"#$&'*+,-.0123456789:;=?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\^_`abcdefghijklmnopqrstuvwxyz|~";
  return parse_gen_ident (start, end, valid_chars);
}

char *parse_val_ident (char **start, char *end)
{
  static char *valid_chars =
    "!\"#$&'*+,-./0123456789:;?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\^_`abcdefghijklmnopqrstuvwxyz|~";
  return parse_gen_ident (start, end, valid_chars);
}

char *parse_c_ident (char **start, char *end)
{
  static char *valid_chars =
    "0123456789@ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz";
  return parse_gen_ident (start, end, valid_chars);
}

char *parse_opt_ident(char **start, char *end)
{
  if (*start  >= end || (**start) != '@')
    return NULL;
  (*start)++;
  return parse_ident(start, end);
}


pdf_obj *parse_pdf_name (char **start, char *end)
{
  pdf_obj *result;
  char *name;
  skip_white(start, end);
  if (**start != '/') {
    fprintf (stderr, "\nPDF Name expected and not found.\n");
    dump(*start, end);
    return NULL;
  }
  (*start)++;
  if ((name = parse_ident(start, end)) != NULL) {
    result = pdf_new_name (name);
    RELEASE (name);
    return result;
  }
  return NULL;
}

char *parse_pdf_reference(char **start, char *end)
{
  skip_white (start, end);
  if (**start != '@') {
    fprintf (stderr, "\nPDF Name expected and not found.\n");
    dump(*start, end);
    return NULL;
  }
  (*start)++;
  return parse_ident(start, end);
}

pdf_obj *parse_pdf_boolean (char **start, char *end)
{
  skip_white (start, end);
  if (end-*start > strlen ("true") &&
      !strncmp (*start, "true", strlen("true"))) {
    *start += strlen("true");
    return pdf_new_boolean (1);
  }
  if (end - *start > strlen ("false") &&
      !strncmp (*start, "false", strlen("false"))) {
    *start += strlen("false");
    return pdf_new_boolean (0);
  }
  return NULL;
}

pdf_obj *parse_pdf_null (char **start, char *end)
{
  char *save = *start;
  char *ident;
  skip_white (start, end);
  ident = parse_ident(start, end);
  if (!strcmp (ident, "null")) {
    RELEASE(ident);
    return pdf_new_null();
  }
  *start = save;
  fprintf (stderr, "\nNot a valid object\n");
  dump(*start, end);
  return NULL;
}

static pdf_obj *parse_pdf_number (char **start, char *end)
{
  char *number;
  pdf_obj *result;
  skip_white(start, end);
  if ((number = parse_number(start, end)) != NULL) {
    result = pdf_new_number (atof(number));
    RELEASE (number);
    return result;
  }
  return NULL;
}

int xtod (char c) 
{
  if (c >= '0' && c <= '9')
    return c-'0';
  if (c >= 'A' && c <= 'F')
    return (c-'A')+10;
  if (c >= 'a' && c <= 'f')
    return (c-'a')+10;
  return 0;
}

pdf_obj *parse_pdf_hex_string (char **start, char *end)
{
  pdf_obj *result;
  char *save;
  unsigned char *string = NULL;
  int strlength;
  skip_white (start, end);
  if (*start == end || *((*start)++) != '<')
    return NULL;
  save = *start;
  string = NEW ((end - *start)/2+2, unsigned char); /* A little excess here */
  strlength = 0;
  while (*start < end && **start != '>') {
    string[strlength] = xtod(**start) * 16;
    (*start) += 1;
    if (*start < end && **start != '>') {
      string[strlength] += xtod(**start);
      (*start) += 1;
    }
    skip_white (start, end);
    strlength += 1;
  }
  if (*start < end) {
     *start += 1;
     result = pdf_new_string (string, strlength);
  } else {
     result = NULL;
  }
  if (string)
     RELEASE(string);
  return result;
}

pdf_obj *parse_pdf_string (char **start, char *end)
{
  pdf_obj *result;
  int balance = 0;
  char *save;
  unsigned char *string;
  int strlength;
  skip_white(start, end);
  save = *start;
  if (*start == end || **start != '(') {
    return NULL;
  }
  ++(*start);
  string = NEW (end - *start, unsigned char);
  strlength = 0;
  balance = 0;
  while (*start < end &&
	 (**start != ')' || balance > 0)) {
    if (**start == '\\')
      switch (*(++(*start))) {
      case 'n':
	string[strlength++] = '\n';
	(*start)++;
	break;
      case 'r':
	string[strlength++] = '\r';
	(*start)++;
	break;
      case 't':
	string[strlength++] = '\t';
	(*start)++;
	break;
      case 'b':
	string[strlength++] = '\b';
	(*start)++;
	break;
      default:
	if (isdigit(**start)) {
	  int i;
	  string[strlength] = 0;
	  for (i=0; i<3; i++) 
	    string[strlength] = string[strlength]*8 + (*((*start)++)-'0');
	  strlength+= 1;
	} else {
	  string[strlength++] = *((*start)++);
	}
      }
    else {
      if (**start == '(')
	balance += 1;
      if (**start == ')')
	balance -= 1;
      string[strlength++] = *((*start)++);
    }
  }
  if (*start >= end) {
    fprintf (stderr, "\nString object ended prematurely\n");
    dump (save, *start);
    return NULL;
  }
  (*start)++;
  result = pdf_new_string (string, strlength);
  RELEASE (string);
  return result;
}

char *parse_c_string (char **start, char *end)
{
  char *string, *save;
  int strlength;
  skip_white(start, end);
  save = *start;
  if (*start == end || **start != '"') {
    return NULL;
  }
  ++(*start);
  string = NEW (end - *start, char);
  strlength = 0;
  while (*start < end && (**start != '"')) {
    if (**start == '\\')
      switch (*(++(*start))) {
      case '"':
	string[strlength++] = '"';
	(*start)++;
	break;
      case 'n':
	string[strlength++] = '\n';
	(*start)++;
	break;
      case 'r':
	string[strlength++] = '\r';
	(*start)++;
	break;
      case 't':
	string[strlength++] = '\t';
	(*start)++;
	break;
      case 'b':
	string[strlength++] = '\b';
	(*start)++;
	break;
      default:
	if (isdigit(**start)) {
	  int i;
	  string[strlength] = 0;
	  for (i=0; i<3; i++) 
	    string[strlength] = string[strlength]*8 + (*((*start)++)-'0');
	  strlength+= 1;
	} else {
	  string[strlength++] = *((*start)++);
	}
      }
    else {
      string[strlength++] = *((*start)++);
    }
    string[strlength]=0;
  }
  if (*start >= end) {
    fprintf (stderr, "\nString ended prematurely\n");
    dump (save, *start);
    return NULL;
  }
  string[strlength] = 0;
  (*start)++;
  return string;
}

static pdf_obj *parse_pdf_stream (char **start, char *end, pdf_obj
				  *dict)
{
  pdf_obj *result, *new_dict, *tmp1, *length_obj;
  unsigned long length;
  if (pdf_lookup_dict(dict, "F")) {
    fprintf (stderr, "File streams not implemented (yet)");
    return NULL;
  }
  if ((tmp1 = pdf_lookup_dict(dict, "Length")) == NULL) {
    fprintf (stderr, "No length specified");
    return NULL;
  }
  length = pdf_number_value (length_obj = pdf_deref_obj (tmp1));
  pdf_release_obj (length_obj);
  skip_white(start, end);
  skip_line(start, end);
  result = pdf_new_stream(0);
  new_dict = pdf_stream_dict(result);
  pdf_merge_dict (new_dict, dict);
  pdf_release_obj (dict);
  pdf_add_stream (result, *start, length);
  *start += length;
  skip_white(start, end);
  if (*start+strlen("endstream") > end ||
      strncmp(*start, "endstream", strlen("endstream"))) {
    fprintf (stderr, "\nendstream not found\n");
    return NULL;
  }
  *start += strlen("endstream");
  return result;
}

pdf_obj *parse_pdf_object (char **start, char *end)
{
  pdf_obj *result, *tmp1=NULL, *tmp2=NULL;
  char *save = *start;
  char *position2;
  skip_white(start, end);
  if (*start >= end)
    return NULL;
  switch (**start) {
  case '<': 
    /* Check for those troublesome strings starting with '<' */
    if (*start+1 < end && *(*start+1) != '<') {
      result = parse_pdf_hex_string (start, end);
      break;
    }
    result = parse_pdf_dict (start, end);
    skip_white(start, end);
    if (end - *start > strlen("stream") &&
	!strncmp(*start, "stream", strlen("stream"))) {
      result = parse_pdf_stream (start, end, result);
    }
    /* Check for stream */
    break;
  case '(':
    result = parse_pdf_string(start, end);
    break;
  case '[':
    result = parse_pdf_array(start, end);
    break;
  case '/':
    result = parse_pdf_name(start, end);
    break;
  case '@':
    result = get_reference(start, end);
    break;
  case 't':
  case 'f':
    result = parse_pdf_boolean(start, end);
    break;
  default:
    /* This is a bit of a hack, but PDF doesn't easily allow you to
       tell a number from an indirect object reference with some
       serious looking ahead */
    
    if (*start < end && 
	(isdigit(**start) || **start == '+' || **start == '-' || **start == '.')) {
      tmp1 = parse_pdf_number(start, end);
      tmp2 = NULL;
      /* This could be a # # R type reference.  We can't be sure unless
	 we look ahead for the second number and the 'R' */
      skip_white(start, end);
      position2 = *start;
      if (*start < end && isdigit(**start)) {
	tmp2 = parse_pdf_number(start, end);
      } else
	tmp2 = NULL;
      skip_white(start, end);
      if (tmp1 != NULL && tmp2 != NULL && *start < end && *((*start)++) == 'R') {
	result = pdf_new_ref ((unsigned long) pdf_number_value (tmp1), 
			      (int) pdf_number_value (tmp2));
	pdf_release_obj (tmp1);
	pdf_release_obj (tmp2);
	break;
      }
      /* Following checks if we got two numbers, but not 'r' */
      if (tmp1 != NULL && tmp2 != NULL) {
	pdf_release_obj (tmp2);
	*start = position2;
      }
      result = tmp1;
      break;
    }
    if (*start < end && **start == 'n') {
      result = parse_pdf_null(start, end);
      break;
    }
    result = NULL;
    break;
  }
  if (result == NULL) {
    fprintf (stderr, "\nExpecting an object, but didn't find one");
    *start = save;
    dump(*start, end);
  }
  return result;
}

void parse_key_val (char **start, char *end, char **key, char **val) 
{
  *key = NULL;
  *val = NULL;
  skip_white (start, end);
  if ((*key = parse_c_ident (start, end))) {
    skip_white (start, end);
    if (*start < end && **start == '=')
      {
	(*start) += 1;
	skip_white (start, end);
	if (*start < end) switch (**start) {
	case '"':
	  *val = parse_c_string (start, end);
	  break;
	default:
	  *val = parse_val_ident (start, end);
	}
      }
  }
}
