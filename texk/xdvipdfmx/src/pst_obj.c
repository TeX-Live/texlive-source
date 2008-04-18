/*  $Header: /home/cvsroot/dvipdfmx/src/pst_obj.c,v 1.5 2004/07/27 12:08:47 hirata Exp $

    This is dvipdfmx, an eXtended version of dvipdfm by Mark A. Wicks.

    Copyright (C) 2002 by Jin-Hwan Cho and Shunsaku Hirata,
    the dvipdfmx project team <dvipdfmx@project.ktug.or.kr>

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

#include <string.h>
#include "system.h"
#include "mem.h"
#include "error.h"
#include "dpxutil.h"
#include "pst.h"
#include "pst_obj.h"

struct pst_obj
{
  pst_type type;
  void    *data;
};

static const char *pst_const_null  = "null";
static const char *pst_const_mark  = "mark";
/*
static const char *pst_const_true  = "true";
static const char *pst_const_false = "false";
*/

typedef char *                     pst_null;
typedef struct { char    value; }  pst_boolean;
typedef struct { long    value; }  pst_integer;
typedef struct { double  value; }  pst_real;
typedef struct { char   *value; }  pst_name;
typedef struct
{
  long    length;
  unsigned char *value;
} pst_string;


/* BOOLEAN */
static pst_boolean *pst_boolean_new     (char value)      ;
static void         pst_boolean_release (pst_boolean *obj);
static long         pst_boolean_IV      (pst_boolean *obj);
static double       pst_boolean_RV      (pst_boolean *obj);
static unsigned char *pst_boolean_SV      (pst_boolean *obj);
static long         pst_boolean_length  (pst_boolean *obj);
static void        *pst_boolean_data_ptr(pst_boolean *obj);

/* NUMBERS */
static long   parse_long (unsigned char **inbuf, unsigned char *inbufend, unsigned char base, unsigned char *ndigits);
static double cvtdbl     (long ipart, long dpart, unsigned char nddigits, long epart);

static pst_integer *pst_integer_new     (long value)      ;
static void         pst_integer_release (pst_integer *obj);
static long         pst_integer_IV      (pst_integer *obj);
static double       pst_integer_RV      (pst_integer *obj);
static unsigned char      *pst_integer_SV      (pst_integer *obj);
static unsigned int        pst_integer_length  (pst_integer *obj);
static void        *pst_integer_data_ptr(pst_integer *obj);

static pst_real *pst_real_new      (double value) ;
static void      pst_real_release  (pst_real *obj);
static long      pst_real_IV       (pst_real *obj);
static double    pst_real_RV       (pst_real *obj);
static unsigned char   *pst_real_SV       (pst_real *obj);
static void     *pst_real_data_ptr (pst_real *obj);
static unsigned int     pst_real_length   (pst_real *obj);

/* NAME */
static pst_name *pst_name_new      (const char *name) ;
static void      pst_name_release  (pst_name *obj);
static long      pst_name_IV       (pst_name *obj);
static double    pst_name_RV       (pst_name *obj);
static unsigned char   *pst_name_SV       (pst_name *obj);
static void     *pst_name_data_ptr (pst_name *obj);
static unsigned int     pst_name_length   (pst_name *obj);

/* STRING */
static pst_string *pst_string_parse_literal (unsigned char **inbuf, unsigned char *inbufend);
static pst_string *pst_string_parse_hex     (unsigned char **inbuf, unsigned char *inbufend);

static pst_string *pst_string_new      (unsigned char *str, unsigned int len);
static void        pst_string_release  (pst_string *obj)       ;
static long        pst_string_IV       (pst_string *obj)       ;
static double      pst_string_RV       (pst_string *obj)       ;
static unsigned char     *pst_string_SV       (pst_string *obj)       ;
static void       *pst_string_data_ptr (pst_string *obj)       ;
static unsigned int       pst_string_length   (pst_string *obj)       ;


#define TYPE_ERROR() ERROR("Operation not defined for this type of object.")

pst_obj *
pst_new_obj (pst_type type, void *data)
{
  pst_obj *obj;

  obj = NEW(1, struct pst_obj);
  obj->type = type;
  obj->data = data;

  return obj;
}

pst_obj *
pst_new_mark (void)
{
  return pst_new_obj(PST_TYPE_MARK, (void *)pst_const_mark);
}

void
pst_release_obj (pst_obj *obj)
{
  ASSERT(obj);
  switch (obj->type) {
  case PST_TYPE_BOOLEAN: pst_boolean_release(obj->data); break;
  case PST_TYPE_INTEGER: pst_integer_release(obj->data); break;
  case PST_TYPE_REAL:    pst_real_release(obj->data);    break;
  case PST_TYPE_NAME:    pst_name_release(obj->data);    break;
  case PST_TYPE_STRING:  pst_string_release(obj->data);  break;
  case PST_TYPE_NULL:
  case PST_TYPE_MARK:
    break;
  case PST_TYPE_UNKNOWN:
    if (obj->data)
      RELEASE(obj->data);
    break;
  default:
    ERROR("Unrecognized object type: %d", obj->type);
  }
  RELEASE(obj);
}

pst_type
pst_type_of (pst_obj *obj)
{
  ASSERT(obj);
  return obj->type;
}

long
pst_length_of (pst_obj *obj)
{
  long len = 0;

  ASSERT(obj);
  switch (obj->type) {
  case PST_TYPE_BOOLEAN: len = pst_boolean_length(obj->data); break;
  case PST_TYPE_INTEGER: len = pst_integer_length(obj->data); break;
  case PST_TYPE_REAL:    len = pst_real_length(obj->data);    break;
  case PST_TYPE_NAME:    len = pst_name_length(obj->data);    break;
  case PST_TYPE_STRING:  len = pst_string_length(obj->data);  break;
  case PST_TYPE_NULL:
  case PST_TYPE_MARK:
    TYPE_ERROR();                     
    break;
  case PST_TYPE_UNKNOWN:
    len = strlen(obj->data);
    break;
  default:
    ERROR("Unrecognized object type: %d", obj->type);
  }

  return len;
}

long
pst_getIV (pst_obj *obj)
{
  long iv = 0;

  ASSERT(obj);
  switch (obj->type) {
  case PST_TYPE_BOOLEAN: iv = pst_boolean_IV(obj->data); break;
  case PST_TYPE_INTEGER: iv = pst_integer_IV(obj->data); break;
  case PST_TYPE_REAL:    iv = pst_real_IV(obj->data);    break;
  case PST_TYPE_NAME:    iv = pst_name_IV(obj->data);    break;
  case PST_TYPE_STRING:  iv = pst_string_IV(obj->data);  break;
  case PST_TYPE_NULL:
  case PST_TYPE_MARK: 
    TYPE_ERROR(); 
    break;
  case PST_TYPE_UNKNOWN:
    ERROR("Cannot convert object of type UNKNOWN to integer value.");
    break;
  default:
    ERROR("Unrecognized object type: %d", obj->type);
  }

  return iv;
}

double
pst_getRV (pst_obj *obj)
{
  double rv = 0.0;

  ASSERT(obj);
  switch (obj->type) {
  case PST_TYPE_BOOLEAN: rv = pst_boolean_RV(obj->data); break;
  case PST_TYPE_INTEGER: rv = pst_integer_RV(obj->data); break;
  case PST_TYPE_REAL:    rv = pst_real_RV(obj->data);    break;
  case PST_TYPE_NAME:    rv = pst_name_RV(obj->data);    break;
  case PST_TYPE_STRING:  rv = pst_string_RV(obj->data);  break;
  case PST_TYPE_NULL:
  case PST_TYPE_MARK:
    TYPE_ERROR();                  
    break;
  case PST_TYPE_UNKNOWN:
    ERROR("Cannot convert object of type UNKNOWN to real value.");
    break;
  default:
    ERROR("Unrecognized object type: %d", obj->type);
  }

  return rv;
}

/* Length can be obtained by pst_length_of(). */
unsigned char *
pst_getSV (pst_obj *obj)
{
  unsigned char *sv = NULL;

  ASSERT(obj);
  switch (obj->type) {
  case PST_TYPE_BOOLEAN: sv = pst_boolean_SV(obj->data); break;
  case PST_TYPE_INTEGER: sv = pst_integer_SV(obj->data); break;
  case PST_TYPE_REAL:    sv = pst_real_SV(obj->data);    break;
  case PST_TYPE_NAME:    sv = pst_name_SV(obj->data);    break;
  case PST_TYPE_STRING:  sv = pst_string_SV(obj->data);  break;
  case PST_TYPE_NULL:
  case PST_TYPE_MARK:
    TYPE_ERROR(); 
    break;
  case PST_TYPE_UNKNOWN:
    {
      long len;

      len = strlen((char *) obj->data);
      if (len > 0) {
	sv = NEW(len+1, unsigned char);
	memcpy(sv, obj->data, len);
	sv[len] = '\0';
      } else {
	sv = NULL;
      }
      break;
    }
  default:
    ERROR("Unrecognized object type: %d", obj->type);
  }

  return sv;
}

void *
pst_data_ptr (pst_obj *obj)
{
  char *p = NULL;

  ASSERT(obj);
  switch (obj->type) {
  case PST_TYPE_BOOLEAN: p = pst_boolean_data_ptr(obj->data); break;
  case PST_TYPE_INTEGER: p = pst_integer_data_ptr(obj->data); break;
  case PST_TYPE_REAL:    p = pst_real_data_ptr(obj->data);    break;
  case PST_TYPE_NAME:    p = pst_name_data_ptr(obj->data);    break;
  case PST_TYPE_STRING:  p = pst_string_data_ptr(obj->data);  break;
  case PST_TYPE_NULL:
  case PST_TYPE_MARK: 
    TYPE_ERROR();
    break;
  case PST_TYPE_UNKNOWN:
    p = obj->data;
    break;
  default:
    ERROR("Unrecognized object type: %d", obj->type);
  }

  return (void *)p;
}

/* BOOLEAN */
static pst_boolean *
pst_boolean_new (char value)
{
  pst_boolean *obj;
  obj = NEW(1, pst_boolean);
  obj->value = value;
  return obj;
}

static void
pst_boolean_release (pst_boolean *obj)
{
  ASSERT(obj);
  RELEASE(obj);
}

static long
pst_boolean_IV (pst_boolean *obj)
{
  ASSERT(obj);
  return (long) obj->value;
}

static double
pst_boolean_RV (pst_boolean *obj)
{
  ASSERT(obj);
  return (double) obj->value;
}

static unsigned char *
pst_boolean_SV (pst_boolean *obj)
{
  unsigned char *str;

  ASSERT(obj);

  if (obj->value) {
    str = NEW(5, unsigned char);
    memcpy(str, "true", 4);
    str[4] = '\0';
  } else {
    str = NEW(6, unsigned char);
    memcpy(str, "false", 5);
    str[5] = '\0';
  }

  return str;
}

static long
pst_boolean_length (pst_boolean *obj)
{
  TYPE_ERROR();
  return 0;
}

static void *
pst_boolean_data_ptr (pst_boolean *obj)
{
  ASSERT(obj);
  return (void*) &(obj->value);
}

pst_obj *
pst_parse_boolean (unsigned char **inbuf, unsigned char *inbufend)
{
  if (*inbuf + 4 <= inbufend &&
      memcmp(*inbuf, "true", 4) == 0 &&
      PST_TOKEN_END(*inbuf + 4, inbufend)) {
    *inbuf += 4;
    return pst_new_obj(PST_TYPE_BOOLEAN, pst_boolean_new(1));
  } else if (*inbuf + 5 <= inbufend &&
	     memcmp(*inbuf, "false", 5) == 0 &&
	     PST_TOKEN_END(*inbuf + 5, inbufend)) {
    *inbuf += 5;
    return pst_new_obj(PST_TYPE_BOOLEAN, pst_boolean_new(0));
  } else
    return NULL;
}


/* NULL */
pst_obj *
pst_parse_null (unsigned char **inbuf, unsigned char *inbufend)
{
  if (*inbuf + 4 <= inbufend &&
      memcmp(*inbuf, "null", 4) == 0 &&
      PST_TOKEN_END(*inbuf+4, inbufend)) {
    *inbuf += 4;
    return pst_new_obj(PST_TYPE_NULL, (void*)pst_const_null);
  } else
    return NULL;
}

/* INTEGER */
static pst_integer *
pst_integer_new (long value)
{
  pst_integer *obj;
  obj = NEW(1, pst_integer);
  obj->value = value;
  return obj;
}

static void
pst_integer_release (pst_integer *obj)
{
  ASSERT(obj);
  RELEASE(obj);
}

static long
pst_integer_IV (pst_integer *obj)
{
  ASSERT(obj);
  return (long) obj->value;
}

static double
pst_integer_RV (pst_integer *obj)
{
  ASSERT(obj);
  return (double) obj->value;
}

static unsigned char *
pst_integer_SV (pst_integer *obj)
{
  char *value;
  int   len;
  char  fmt_buf[PST_MAX_DIGITS+5];

  ASSERT(obj);

  len = sprintf(fmt_buf, "%ld", obj->value);

  value = NEW(len, char);
  strcpy(value, fmt_buf);

  return (unsigned char *) value;
}

static void *
pst_integer_data_ptr (pst_integer *obj)
{
  ASSERT(obj);
  return (void*) &(obj->value);
}

static unsigned int
pst_integer_length (pst_integer *obj)
{
  TYPE_ERROR();
  return 0;
}

/* REAL */
static pst_real *
pst_real_new (double value)
{
  pst_real *obj;

  obj = NEW(1, pst_real);
  obj->value = value;

  return obj;
}

static void
pst_real_release (pst_real *obj)
{
  ASSERT(obj);
  RELEASE(obj);
}

static long
pst_real_IV (pst_real *obj)
{
  ASSERT(obj);
  return (long) obj->value;
}

static double
pst_real_RV (pst_real *obj)
{
  ASSERT(obj);
  return (double) obj->value;
}

static unsigned char *
pst_real_SV (pst_real *obj)
{
  char *value;
  int   len;
  char  fmt_buf[PST_MAX_DIGITS+5];

  ASSERT(obj);

  len = sprintf(fmt_buf, "%.5g", obj->value);

  value = NEW(len, char);
  strcpy(value, fmt_buf);

  return (unsigned char *) value;
}

static void *
pst_real_data_ptr (pst_real *obj)
{
  ASSERT(obj);

  return (void*) &(obj->value);
}

static unsigned int
pst_real_length (pst_real *obj)
{
  TYPE_ERROR();
  return 0;
}

/*
 * Does not skip white spaces
 * base number must be greater than 1 and less than 33.
 * TODO: overflow and underflow
 */
static long
parse_long (unsigned char **inbuf, unsigned char *inbufend, unsigned char base, unsigned char *ndigits)
{
  long    val = 0, sign = 1;
  unsigned char *cur = *inbuf, c;
  int     count = 0;

  if (*cur == '+') {
    sign = 1;
    cur++;
  } else if (*cur == '-') {
    sign = -1;
    cur++;
  }
  while (cur < inbufend) {
    c = *cur;
    if (c >= '0' && c <= '0' + ((base <= 10) ? base-1 : 9)) {
      c = c - '0';
    } else if (base > 10) {
      if (c >= 'a' && c <= 'a' + (base - 11)) {
	c = c - 'a' + 10;
      } else if (c >= 'A' && c <= 'A' + (base - 11)) {
	c = c - 'A' + 10;
      } else {
	break;
      }
    } else {
      break;
    }
    if (val < (LONG_MAX - c) / base) {
      val = val * base + c;
      count++;
    }
    cur++;
  }

  if (ndigits != NULL)
    *ndigits = count;
  *inbuf = cur;
  return sign*val;
}

static double
cvtdbl (long ipart, long dpart, unsigned char nddigits, long epart)
{
  double dval = 0.0;
  int    i;

  if (dpart > 0 && nddigits > 0) {
    dval = dpart;
    for (i = 0; i < nddigits; i++) {
      dval *= 0.1;
    }
  }

  if (ipart < 0)
    dval = ipart - dval;
  else
    dval = ipart + dval;

  if (epart > 0) {
    for (i = 0; i < epart; i++)
      dval *= 10.0;
  } else if (epart < 0) {
    for (i = 0; i < -epart; i++)
      dval *= 0.1;
  }

  return dval;
}

pst_obj *
pst_parse_number (unsigned char **inbuf, unsigned char *inbufend)
{
  pst_obj *obj = NULL;
  unsigned char  *cur = *inbuf;
  unsigned char  ndigits = 0, nddigits = 0;
  int     type;
  long    ipart = 0, dpart = 0, epart = 0;

  if (cur >= inbufend)
    return NULL;

  ipart = parse_long(&cur, inbufend, 10, &ndigits);
  if (ndigits > 0 && PST_TOKEN_END(cur, inbufend)) {
    type  = PST_TYPE_INTEGER;
    dpart = epart = 0;
  } else if (ndigits > 0 && *cur == '#' &&
	     ipart >= 2 && ipart <= 36) { /* Radix */
    cur++;
    ipart = parse_long(&cur, inbufend, ipart, NULL);
    if (ipart >= 0 && PST_TOKEN_END(cur, inbufend)) { /* non-negative only ? */
      type  = PST_TYPE_INTEGER;
      dpart = epart = 0;
    } else
      type  = -1;
  } else if (*cur == '.') {
    cur++;
    dpart = parse_long(&cur, inbufend, 10, &nddigits);
    ndigits += nddigits;
    if (dpart < 0) {
      type  = -1;
    } else if (ndigits > 0 && PST_TOKEN_END(cur, inbufend)) {
      type  = PST_TYPE_REAL;
      epart = 0;
    } else if (ndigits > 0 && (*cur == 'e' || *cur == 'E')) {
      cur++;
      epart = parse_long(&cur, inbufend, 10, &ndigits);
      if (ndigits > 0 && PST_TOKEN_END(cur, inbufend))
	type = PST_TYPE_REAL;
      else
	type = -1;
    } else
      type = -1;
  } else if (ndigits > 0 && (*cur == 'e' || *cur == 'E')) {
    cur++;
    epart = parse_long(&cur, inbufend, 10, &ndigits);
    if (ndigits > 0 && PST_TOKEN_END(cur, inbufend)) {
      dpart  = 0; 
      type   = PST_TYPE_REAL;
    } else
      type  =  -1;
  } else {
    type = -1;
  }

  if (type < 0)
    return NULL;

  if (type == PST_TYPE_REAL)
    obj = pst_new_obj(type, pst_real_new(cvtdbl(ipart, dpart, nddigits, epart)));
  else if (type == PST_TYPE_INTEGER)
    obj = pst_new_obj(type, pst_integer_new(ipart));
  else
    return NULL;

  *inbuf = cur;
  return obj;
}

/* NAME */

/*
 * \0 is not allowed for name object.
 */

static pst_name *
pst_name_new (const char *name)
{
  pst_name *obj;

  obj = NEW(1, pst_name);
  obj->value = NEW(strlen(name)+1, char);
  strcpy(obj->value, name);

  return obj;
}

static void
pst_name_release (pst_name *obj)
{
  ASSERT(obj);
  if (obj->value)
    RELEASE(obj->value);
  RELEASE(obj);
}

int
pst_name_is_valid (const char *name)
{
  static const char *valid_chars =
    "!\"#$&'*+,-.0123456789:;=?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\^_`abcdefghijklmnopqrstuvwxyz|~";
  if (strspn(name, valid_chars) == strlen(name))
    return 1;
  else
    return 0;
}

char *
pst_name_encode (const char *name)
{
  char *encoded_name, *p;
  int   i, len;
  char  c;

  len = strlen(name);
  if (len > PST_NAME_LEN_MAX) {
    WARN("Input string too long for name object. String will be truncated.");
    len = PST_NAME_LEN_MAX;
  }

  p = encoded_name = NEW(3*len+1, char);
  for (i = 0; i < len; i++) {
    c = name[i];
    if (c < '!'  || c > '~' ||
	c == '#' || is_delim(c) || is_space(c)) {
      *p++ = '#';
      putxpair(c, &p);
    } else {
      *p++ = c;
    }
  }
  *p = '\0';

  return encoded_name;
}

pst_obj *
pst_parse_name (unsigned char **inbuf, unsigned char *inbufend) /* / is required */
{
  unsigned char  wbuf[PST_NAME_LEN_MAX+1];
  unsigned char  c, *p = wbuf, *cur = *inbuf;
  int     len = 0;

  if (*cur != '/')
    return NULL;
  cur++;

  while (!PST_TOKEN_END(cur, inbufend)) {
    c = *cur++;
    if (c == '#') {
      int val;
      if (cur + 2 >= inbufend) {
	WARN("Premature end of input name string.");
	break;
      }
      val = getxpair(&cur);
      if (val <= 0) {
	WARN("Invalid char for name object. (ignored)");
	continue;
      } else
	c = (unsigned char) val;
    }
    if (len < PST_NAME_LEN_MAX)
      *p++ = c;
    len++;
  }
  *p = '\0';

  if (len > PST_NAME_LEN_MAX)
    WARN("String too long for name object. Output will be truncated.");

  *inbuf = cur;
  return pst_new_obj(PST_TYPE_NAME, pst_name_new((char *)wbuf));
}

static long
pst_name_IV (pst_name *obj)
{
  TYPE_ERROR();
  return 0;
}

static double
pst_name_RV (pst_name *obj)
{
  TYPE_ERROR();
  return 0;
}

static unsigned char *
pst_name_SV (pst_name *obj)
{
  char *value;

  value = NEW(strlen(obj->value)+1, char);
  strcpy(value, obj->value);

  return (unsigned char *) value;
}

static void *
pst_name_data_ptr (pst_name *obj)
{
  ASSERT(obj);
  return obj->value;
}

static unsigned int
pst_name_length (pst_name *obj)
{
  ASSERT(obj);
  return strlen(obj->value);
}


/* STRING */

/*
 * TODO: ascii85 string <~ .... ~>
 */
static pst_string *
pst_string_new (unsigned char *str, unsigned int len)
{
  pst_string *obj;
  obj = NEW(1, pst_string);
  obj->length  = len;
  obj->value = NULL;
  if (len > 0) {
    obj->value = NEW(len, unsigned char);
    if (str)
      memcpy(obj->value, str, len);
  }
  return obj;
}

static void
pst_string_release (pst_string *obj)
{
  ASSERT(obj);
  if (obj->value)
    RELEASE(obj->value);
  RELEASE(obj);
}

pst_obj *
pst_parse_string (unsigned char **inbuf, unsigned char *inbufend)
{
  if (*inbuf + 2 >= inbufend) {
    return NULL;
  } else if (**inbuf == '(')
    return pst_new_obj(PST_TYPE_STRING, pst_string_parse_literal(inbuf, inbufend));
  else if (**inbuf == '<' && *(*inbuf+1) == '~')
    ERROR("ASCII85 string not supported yet.");
  else if (**inbuf == '<')
    return pst_new_obj(PST_TYPE_STRING, pst_string_parse_hex(inbuf, inbufend));
  return NULL;
}

static pst_string *
pst_string_parse_literal (unsigned char **inbuf, unsigned char *inbufend)
{
  unsigned char  wbuf[PST_STRING_LEN_MAX];
  unsigned char *cur = *inbuf, c = 0;
  long    len = 0, balance = 1;

  if (cur + 2 > inbufend || *cur != '(')
    return NULL;

  cur++;
  while (cur < inbufend && len < PST_STRING_LEN_MAX && balance > 0) {
    c = *(cur++);
    switch (c) {
    case '\\':
      {
	unsigned char unescaped, valid;
	unescaped = esctouc(&cur, inbufend, &valid);
	if (valid)
	  wbuf[len++] = unescaped;
      }
      break;
    case '(':
      balance++;
      wbuf[len++] = '(';
      break;
    case ')':
      balance--;
      if (balance > 0)
	wbuf[len++] = ')';
      break;
      /*
       * An end-of-line marker (\n, \r or \r\n), not preceeded by a backslash,
       * must be converted to single \n.
       */
    case '\r':
      if (cur < inbufend && *cur == '\n')
	cur++;
      wbuf[len++] = '\n';
      break;
    default:
      wbuf[len++] = c;
    }
  }
  if (c != ')')
    return NULL;

  *inbuf  = cur;
  return pst_string_new(wbuf, len);
}

static pst_string *
pst_string_parse_hex (unsigned char **inbuf, unsigned char *inbufend)
{
  unsigned char  wbuf[PST_STRING_LEN_MAX];
  unsigned char *cur = *inbuf;
  unsigned long  len = 0;

  if (cur + 2 > inbufend || *cur != '<' ||
      (*cur == '<' && *(cur+1) == '<'))
    return NULL;

  cur++;
  /* PDF Reference does not specify how to treat invalid char */  
  while (cur < inbufend && len < PST_STRING_LEN_MAX) {
    int    hi, lo;
    skip_white_spaces(&cur, inbufend);
    if (*cur == '>')
      break;
    hi = xtoi(*(cur++));
    if (hi < 0) {
      WARN("Invalid char for hex string <%x> treated as <0>.", *(cur-1));
      hi = 0;
    }
    skip_white_spaces(&cur, inbufend);
    if (*cur == '>')
      break;
    /* 0 is appended if final hex digit is missing */
    lo = (cur < inbufend) ? xtoi(*(cur++)) : 0;
    if (lo < 0) {
      WARN("Invalid char for hex string <%x> treated as <0>.", *(cur-1));
      lo = 0;
    }
    wbuf[len++] = (hi << 4) | lo;
  }
  if (*cur++ != '>')
    return NULL;

  *inbuf = cur;
  return pst_string_new(wbuf, len);
}

static long
pst_string_IV (pst_string *obj)
{
  return (long) pst_string_RV(obj);
}

static double
pst_string_RV (pst_string *obj)
{
  pst_obj *nobj;
  unsigned char  *p, *end;
  double   rv;

  ASSERT(obj);
  p   = obj->value;
  end = p + obj->length;
  nobj = pst_parse_number(&p, end);
  if (nobj == NULL || p != end)
    ERROR("Cound not convert string to real value.");
  rv = pst_getRV(nobj);
  pst_release_obj(nobj);

  return rv;
}

static unsigned char *
pst_string_SV (pst_string *obj)
{
  unsigned char *str = NULL;
  ASSERT(obj);
  str = NEW(obj->length + 1, unsigned char);
  memcpy(str, obj->value, obj->length);
  str[obj->length] = '\0';
  return str;
}

static void *
pst_string_data_ptr (pst_string *obj)
{
  ASSERT(obj);
  return obj->value;
}

static unsigned int
pst_string_length (pst_string *obj)
{
  ASSERT(obj);
  return obj->length;
}
