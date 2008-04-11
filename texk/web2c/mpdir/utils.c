/* Copyright (c) 2006 Taco Hoekwater <taco@elvenkind.com>

This file is part of MetaPost.

MetaPost is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

MetaPost is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with MetaPost; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

$Id $
*/

#ifdef HAVE_ASPRINTF            /* asprintf is not defined in openbsd-compat.h, but in stdio.h */
#  include <stdio.h>
#endif
#include <sys/types.h>

#include <kpathsea/c-proto.h>
#include <kpathsea/c-stat.h>
#include <kpathsea/c-fopen.h>
#include <string.h>
#include <time.h>
#include <float.h>              /* for DBL_EPSILON */
#include "mplib.h"

/* define char_ptr, char_array & char_limit */
typedef char char_entry;
define_array (char);

#define Z_NULL  0  

typedef unsigned char  Byte;
typedef Byte  Bytef;

#define check_nprintf(size_get, size_want) \
    if ((unsigned)(size_get) >= (unsigned)(size_want)) \
        pdftex_fail ("snprintf failed: file %s, line %d", __FILE__, __LINE__);

/*@unused@*/
static const char perforce_id[] =
    "$Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/utils.c#24 $";

char *cur_file_name = NULL;
strnumber last_tex_string;
static char print_buf[PRINTF_BUF_SIZE];
static char *job_id_string = NULL;
extern string ptexbanner;       /* from web2c/lib/texmfmp.c */
extern string versionstring;    /* from web2c/lib/version.c */
extern KPSEDLL string kpathsea_version_string;  /* from kpathsea/version.c */

size_t last_ptr_index;          /* for use with alloc_array */



void pdf_puts (const char *s)
{    
  while (*s)
    fputc(*s++,psfile);
}

void pdf_printf (const char *fmt, ...)
{   
    va_list args;
    va_start (args, fmt);
    vsnprintf (print_buf, PRINTF_BUF_SIZE, fmt, args);
    pdf_puts (print_buf);
    va_end (args);
}

strnumber maketexstring (const char *s)
{
    size_t l;
    if (s == NULL || *s == 0)
        return getnullstr ();
    l = strlen (s);
    check_buf (poolptr + l, poolsize);
    while (l-- > 0)
        strpool[poolptr++] = *s++;
    last_tex_string = makestring ();
    return last_tex_string;
}

void mp_printf (const char *fmt, ...)
{
    va_list args;
    int saved_selector;
    saved_selector = selector;
    va_start (args, fmt);
    vsnprintf (print_buf, PRINTF_BUF_SIZE, fmt, args);
    selector = gettermandlogid();
    print (maketexstring (print_buf));
    flushstr (last_tex_string);
    xfflush (stdout);
    va_end (args);
    selector = saved_selector; /* ps_file_only, probably */
}

/* pdftex_fail may be called when a buffer overflow has happened/is
   happening, therefore may not call mktexstring.  However, with the
   current implementation it appears that error messages are misleading,
   possibly because pool overflows are detected too late. 
   
   The output format of this fuction must be the same as pdf_error in
   pdftex.web! */

void pdftex_fail (const char *fmt, ...)
{
    va_list args;
    va_start (args, fmt);
    fputs ("\nError: module writet1", stderr);
    if (cur_file_name)
        fprintf (stderr, " (file %s)", cur_file_name);
    fputs (": ", stderr);
    vsprintf (print_buf, fmt, args);
    fputs (print_buf, stderr);
    fputs
        ("\n ==> Fatal error occurred, the output PS file is not finished!\n",
         stderr);
    va_end (args);
    exit (1);
}

/* The output format of this fuction must be the same as pdf_warn in
   pdftex.web! */
void pdftex_warn (const char *fmt, ...)
{
    va_list args;
    va_start (args, fmt);
    fputs ("\nWarning: module writet1", stderr);
    if (cur_file_name)
        fprintf (stderr, " (file %s)", cur_file_name);
    fputs (": ", stderr);
    vsprintf (print_buf, fmt, args);
    fputs (print_buf, stderr);
    fputs ("\n", stderr);
    va_end (args);
}


static char *cstrbuf = NULL;

char *makecstring (integer s)
{
    char *p;
    static int allocsize;
    int allocgrow, i, l ;
    if (s==0)
      return "";
    l = strstart[nextstr[s]] - strstart[s];
	if (l==0)
	  return "";
    check_buf (l + 1, MAX_CSTRING_LEN);
    if (cstrbuf == NULL) {
        allocsize = l + 1;
        cstrbuf = xmallocarray (char, allocsize);
    } else if (l + 1 > allocsize) {
        allocgrow = allocsize * 0.2;
        if (l + 1 - allocgrow > allocsize)
            allocsize = l + 1;
        else if (allocsize < MAX_CSTRING_LEN - allocgrow)
            allocsize += allocgrow;
        else
            allocsize = MAX_CSTRING_LEN;
        cstrbuf = xrealloc (cstrbuf, sizeof (char) * allocsize);
    }
    p = cstrbuf;
    for (i = 0; i < l; i++)
        *p++ = strpool[i + strstart[s]];
    *p = 0;
    return cstrbuf;
}

size_t xfwrite (void *ptr, size_t size, size_t nmemb, FILE * stream)
{
    if (fwrite (ptr, size, nmemb, stream) != nmemb)
        pdftex_fail ("fwrite() failed");
    return nmemb;
}

int xfflush (FILE * stream)
{
    if (fflush (stream) != 0)
        pdftex_fail ("fflush() failed");
    return 0;
}

int xgetc (FILE * stream)
{
    int c = getc (stream);
    if (c < 0 && c != EOF)
        pdftex_fail ("getc() failed");
    return c;
}

int xputc (int c, FILE * stream)
{
    int i = putc (c, stream);
    if (i < 0)
        pdftex_fail ("putc() failed");
    return i;
}


scaled extxnoverd (scaled x, scaled n, scaled d)
{
    double r = (((double) x) * ((double) n)) / ((double) d);
    if (r > DBL_EPSILON)
        r += 0.5;
    else
        r -= 0.5;
    if (r >= (double) maxinteger || r <= -(double) maxinteger)
        pdftex_warn ("arithmetic: number too big");
    return (scaled) r;
}

/* Converts any string given in in in an allowed PDF string which can be
 * handled by printf et.al.: \ is escaped to \\, parenthesis are escaped and
 * control characters are octal encoded.
 * This assumes that the string does not contain any already escaped
 * characters!
 */
char *convertStringToPDFString (const char *in, int len)
{
    static char pstrbuf[MAX_PSTRING_LEN];
    char *out = pstrbuf;
    int i, j = 0, k;
    char buf[5];
    for (i = 0; i < len; i++) {
        check_buf (j + sizeof (buf), MAX_PSTRING_LEN);
        if (((unsigned char) in[i] < '!') || ((unsigned char) in[i] > '~')) {
            /* convert control characters into oct */
            k = snprintf (buf, sizeof (buf),
                          "\\%03o", (unsigned int) (unsigned char) in[i]);
            check_nprintf (k, sizeof (buf));
            out[j++] = buf[0];
            out[j++] = buf[1];
            out[j++] = buf[2];
            out[j++] = buf[3];
        } else if ((in[i] == '(') || (in[i] == ')')) {
            /* escape paranthesis */
            out[j++] = '\\';
            out[j++] = in[i];
        } else if (in[i] == '\\') {
            /* escape backslash */
            out[j++] = '\\';
            out[j++] = '\\';
        } else {
            /* copy char :-) */
            out[j++] = in[i];
        }
    }
    out[j] = '\0';
    return pstrbuf;
}


/* Converts any string given in in in an allowed PDF string which can be
 * handled by printf et.al.: \ is escaped to \\, parenthesis are escaped and
 * control characters are octal encoded.
 * This assumes that the string does not contain any already escaped
 * characters!
 *
 * See escapename for parameter description.
 */
void escapestring (poolpointer in)
{
    const poolpointer out = poolptr;
    unsigned char ch;
    while (in < out) {
        if (poolptr + 4 >= poolsize) {
            poolptr = poolsize;
            /* error by str_toks that calls str_room(1) */
            return;
        }

        ch = (unsigned char) strpool[in++];

        if ((ch < '!') || (ch > '~')) {
            /* convert control characters into oct */
            int i = snprintf ((char *) &strpool[poolptr], 5,
                              "\\%.3o", (unsigned int) ch);
            check_nprintf (i, 5);
            poolptr += i;
            continue;
        }
        if ((ch == '(') || (ch == ')') || (ch == '\\')) {
            /* escape parenthesis and backslash */
            strpool[poolptr++] = '\\';
        }
        /* copy char :-) */
        strpool[poolptr++] = ch;
    }
}

/* Convert any given string in a PDF name using escaping mechanism
   of PDF 1.2. The result does not include the leading slash.
   
   PDF specification 1.6, section 3.2.6 "Name Objects" explains:
   <blockquote>
    Beginning with PDF 1.2, any character except null (character code 0) may
    be included in a name by writing its 2-digit hexadecimal code, preceded
    by the number sign character (#); see implementation notes 3 and 4 in
    Appendix H. This syntax is required to represent any of the delimiter or
    white-space characters or the number sign character itself; it is
    recommended but not required for characters whose codes are outside the
    range 33 (!) to 126 (~).
   </blockquote>
   The following table shows the conversion that are done by this
   function:
     code      result   reason
     -----------------------------------
     0         ignored  not allowed
     1..32     escaped  must for white-space:
                          9 (tab), 10 (lf), 12 (ff), 13 (cr), 32 (space)
                        recommended for the other control characters
     35        escaped  escape char "#"
     37        escaped  delimiter "%"
     40..41    escaped  delimiters "(" and ")"
     47        escaped  delimiter "/"
     60        escaped  delimiter "<"
     62        escaped  delimiter ">"
     91        escaped  delimiter "["
     93        escaped  delimiter "]"
     123       escaped  delimiter "{"
     125       escaped  delimiter "}"
     127..255  escaped  recommended
     else      copy     regular characters
     
   Parameter "in" is a pointer into the string pool where
   the input string is located. The output string is written
   as temporary string right after the input string.
   Thus at the begin of the procedure the global variable
   "poolptr" points to the start of the output string and
   after the end when the procedure returns.
*/
void escapename (poolpointer in)
{
    const poolpointer out = poolptr;
    unsigned char ch;
    int i;

    while (in < out) {
        if (poolptr + 3 >= poolsize) {
            poolptr = poolsize;
            /* error by str_toks that calls str_room(1) */
            return;
        }

        ch = (unsigned char) strpool[in++];

        if ((ch >= 1 && ch <= 32) || ch >= 127) {
            /* escape */
            i = snprintf ((char *) &strpool[poolptr], 4,
                          "#%.2X", (unsigned int) ch);
            check_nprintf (i, 4);
            poolptr += i;
            continue;
        }
        switch (ch) {
        case 0:
            /* ignore */
            break;
        case 35:
        case 37:
        case 40:
        case 41:
        case 47:
        case 60:
        case 62:
        case 91:
        case 93:
        case 123:
        case 125:
            /* escape */
            i = snprintf ((char *) &strpool[poolptr], 4,
                          "#%.2X", (unsigned int) ch);
            check_nprintf (i, 4);
            poolptr += i;
            break;
        default:
            /* copy */
            strpool[poolptr++] = ch;
        }
    }
}

/* Convert any given string in a PDF hexadecimal string. The
   result does not include the angle brackets.
   
   This procedure uses uppercase hexadecimal letters.
   
   See escapename for description of parameters.
*/
void escapehex (poolpointer in)
{
    const poolpointer out = poolptr;
    unsigned char ch;
    int i;

    while (in < out) {
        if (poolptr + 2 >= poolsize) {
            poolptr = poolsize;
            /* error by str_toks that calls str_room(1) */
            return;
        }

        ch = (unsigned char) strpool[in++];

        i = snprintf ((char *) &strpool[poolptr], 3, "%.2X", (unsigned int) ch);
        check_nprintf (i, 3);
        poolptr += 2;
    }
}

/* Unescape any given hexadecimal string.

   Last hex digit can be omitted, it is replaced by zero, see
   PDF specification.
   
   Invalid digits are silently ignored.

   See escapename for description of parameters.
*/
void unescapehex (poolpointer in)
{
    const poolpointer out = poolptr;
    unsigned char ch;
    unsigned char a = 0;        /* to avoid warning about uninitialized use of a */
    boolean first = true;

    while (in < out) {
        if (poolptr + 1 >= poolsize) {
            poolptr = poolsize;
            /* error by str_toks that calls str_room(1) */
            return;
        }

        ch = (unsigned char) strpool[in++];

        if ((ch >= '0') && (ch <= '9')) {
            ch -= '0';
        } else if ((ch >= 'A') && (ch <= 'F')) {
            ch -= 'A' - 10;
        } else if ((ch >= 'a') && (ch <= 'f')) {
            ch -= 'a' - 10;
        } else {
            continue;           /* ignore wrong character */
        }

        if (first) {
            a = ch << 4;
            first = false;
            continue;
        }

        strpool[poolptr++] = a + ch;
        first = true;
    }
    if (!first) {               /* last hex digit is omitted */
        strpool[poolptr++] = ch << 4;
    }
}


/* makecfilename
  input/ouput same as makecstring:
    input: string number
    output: C string with quotes removed.
    That means, file names that are legal on some operation systems
    cannot any more be used since pdfTeX version 1.30.4.
*/
char *makecfilename (strnumber s)
{
    char *name = makecstring (s);
    char *p = name;
    char *q = name;

    while (*p) {
        if (*p != '"')
            *q++ = *p;
        p++;
    }
    *q = '\0';
    return name;
}

/* function strips trailing zeros in string with numbers; */
/* leading zeros are not stripped (as in real life) */
char *stripzeros (char *a)
{
    enum { NONUM, DOTNONUM, INT, DOT, LEADDOT, FRAC } s = NONUM, t = NONUM;
    char *p, *q, *r;
    for (p = q = r = a; *p != '\0';) {
        switch (s) {
        case NONUM:
            if (*p >= '0' && *p <= '9')
                s = INT;
            else if (*p == '.')
                s = LEADDOT;
            break;
        case DOTNONUM:
            if (*p != '.' && (*p < '0' || *p > '9'))
                s = NONUM;
            break;
        case INT:
            if (*p == '.')
                s = DOT;
            else if (*p < '0' || *p > '9')
                s = NONUM;
            break;
        case DOT:
        case LEADDOT:
            if (*p >= '0' && *p <= '9')
                s = FRAC;
            else if (*p == '.')
                s = DOTNONUM;
            else
                s = NONUM;
            break;
        case FRAC:
            if (*p == '.')
                s = DOTNONUM;
            else if (*p < '0' || *p > '9')
                s = NONUM;
            break;
        default:;
        }
        switch (s) {
        case DOT:
            r = q;
            break;
        case LEADDOT:
            r = q + 1;
            break;
        case FRAC:
            if (*p > '0')
                r = q + 1;
            break;
        case NONUM:
            if ((t == FRAC || t == DOT) && r != a) {
                q = r--;
                if (*r == '.')  /* was a LEADDOT */
                    *r = '0';
                r = a;
            }
            break;
        default:;
        }
        *q++ = *p++;
        t = s;
    }
    *q = '\0';
    return a;
}

void stripspaces (char *p)
{
    char *q;
    for (q = p; *p != '\0'; p++) {
        if (*p != ' ')
            *q++ = *p;
    }
    *q = '\0';
}

void initversionstring (char **versions)
{
}

void setjobid (int year, int month, int day, int time)
{
    char *name_string, *format_string, *s;
    size_t slen;
    int i;

    if (job_id_string != NULL)
        return;

    name_string = xstrdup (makecstring (jobname));
	format_string = xstrdup (makecstring (memident));
	
    slen = SMALL_BUF_SIZE +
        strlen (name_string) +
        strlen (format_string) +
      //strlen (banner) +
        strlen (versionstring) + strlen (kpathsea_version_string);
    s = xtalloc (slen, char);
    /* The Web2c version string starts with a space.  */
    i = snprintf (s, slen,
                  "%.4d/%.2d/%.2d %.2d:%.2d %s %s%s %s",
                  (year>>16), (month>>16), (day>>16), (time>>16) / 60, (time>>16) % 60,
                  name_string, format_string, //ptexbanner,
                  versionstring, kpathsea_version_string);
    check_nprintf (i, slen);
    job_id_string = xstrdup (s);
    xfree (s);
    xfree (name_string);
    xfree (format_string);
}



static void fnstr_append (const char *s)
{
    size_t l = strlen (s) + 1;
    alloc_array (char, l, SMALL_ARRAY_SIZE);
    strcat (char_ptr, s);
    char_ptr = strend (char_ptr);
}

/* this is not really a true crc32, but it should be just enough to keep
   subsets prefixes somewhat disjunct */
unsigned long crc32 (int oldcrc, const Byte *buf, int len) {
  unsigned long ret = 0;
  int i;
  if (oldcrc==0)
	ret = (23<<24)+(45<<16)+(67<<8)+89;
  else
	for (i=0;i<len;i++)
	  ret = (ret<<2)+buf[i];
  return ret;
}

void make_subset_tag (fm_entry * fm_cur, char **glyph_names, int tex_font)
{
    char tag[7];
    unsigned long crc;
    int i;
    size_t l ;
    if (job_id_string ==NULL)
      exit(1);
    l = strlen (job_id_string) + 1;
    
    alloc_array (char, l, SMALL_ARRAY_SIZE);
    strcpy (char_array, job_id_string);
    char_ptr = strend (char_array);
    if (fm_cur->tfm_name != NULL) {
        fnstr_append (" TFM name: ");
        fnstr_append (fm_cur->tfm_name);
    }
    fnstr_append (" PS name: ");
    //if (font_keys[FONTNAME_CODE].valid)
    //    fnstr_append (fontname_buf);
    //else 
    if (fm_cur->ps_name != NULL)
        fnstr_append (fm_cur->ps_name);
    fnstr_append (" Encoding: ");
    if (fm_cur->encoding != NULL && (fm_cur->encoding)->name != NULL)
        fnstr_append ((fm_cur->encoding)->name);
    else
        fnstr_append ("built-in");
    fnstr_append (" CharSet: ");
    assert (glyph_names != NULL);
    for (i = 0; i < 256; i++)
        if (mpcharmarked (tex_font, i) && glyph_names[i] != notdef) {
			if (glyph_names[i]!=NULL) {
			  fnstr_append ("/");
			  fnstr_append (glyph_names[i]);
			}
        }
    if (fm_cur->charset != NULL) {
        fnstr_append (" Extra CharSet: ");
        fnstr_append (fm_cur->charset);
    }
    crc = crc32 (0L, Z_NULL, 0);
    crc = crc32 (crc, (Bytef *) char_array, strlen (char_array));
    /* we need to fit a 32-bit number into a string of 6 uppercase chars long;
     * there are 26 uppercase chars ==> each char represents a number in range
     * 0..25. The maximal number that can be represented by the tag is
     * 26^6 - 1, which is a number between 2^28 and 2^29. Thus the bits 29..31
     * of the CRC must be dropped out.
     */
    for (i = 0; i < 6; i++) {
        tag[i] = 'A' + crc % 26;
        crc /= 26;
    }
    tag[6] = 0;
    fm_cur->subset_tag = xstrdup (tag);
}

