/*
Copyright (c) 1996-2005 Han The Thanh, <thanh@pdftex.org>

This file is part of pdfTeX.

pdfTeX is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

pdfTeX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with pdfTeX; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

$Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/utils.c#25 $
*/

#include "sys/types.h"
#include "regex.h"
#include "ptexlib.h"
#include "zlib.h"
#include "md5.h"
#include <kpathsea/c-vararg.h>
#include <kpathsea/c-proto.h>
#include <kpathsea/c-stat.h>
#include <kpathsea/c-fopen.h>
#include <time.h>

static const char perforce_id[] = 
    "$Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/utils.c#25 $";

char *cur_file_name = NULL;
strnumber last_tex_string;
static char print_buf[PRINTF_BUF_SIZE];
static char *jobname_cstr = NULL;
static char *job_id_string = NULL;
extern string ptexbanner; /* from web2c/lib/texmfmp.c */
extern string versionstring; /* from web2c/lib/version.c */         
extern KPSEDLL string kpathsea_version_string; /* from kpathsea/version.c */

size_t last_ptr_index; /* for use with alloc_array */

/* define fb_ptr, fb_array & fb_limit */
typedef char fb_entry;
define_array(fb);   

/* define char_ptr, char_array & char_limit */
typedef char char_entry;
define_array(char);   

integer fb_offset(void)
{
    return fb_ptr - fb_array;
}

void fb_seek(integer offset)
{
     fb_ptr = fb_array + offset;
}

void fb_putchar(eightbits b)
{
    alloc_array(fb, 1, SMALL_ARRAY_SIZE);
    *fb_ptr++ = b;
}

void fb_flush(void)
{
    fb_entry *p;
    integer n;
    for (p = fb_array; p < fb_ptr;) {
        n = pdfbufsize - pdfptr;
        if (fb_ptr - p < n)
            n = fb_ptr - p;
        memcpy(pdfbuf + pdfptr, p, (unsigned)n);
        pdfptr += n;
        if (pdfptr == pdfbufsize)
            pdfflush();
        p += n;
    }
    fb_ptr = fb_array;
}

static void fnstr_append(const char *s)
{
    int l = strlen(s) + 1;
    alloc_array(char, l, SMALL_ARRAY_SIZE);
    strcat(char_ptr, s);
    char_ptr = strend(char_ptr);
}

void make_subset_tag(fm_entry *fm_cur, char **glyph_names)
{
    char tag[7];
    unsigned long crc;
    int i, l = strlen(job_id_string) + 1;
    alloc_array(char, l, SMALL_ARRAY_SIZE);
    strcpy(char_array, job_id_string);
    char_ptr = strend(char_array);
    if (fm_cur->tfm_name != NULL) {
        fnstr_append(" TFM name: ");
        fnstr_append(fm_cur->tfm_name);
    }
    fnstr_append(" PS name: ");
    if (font_keys[FONTNAME_CODE].valid)
        fnstr_append(fontname_buf);
    else if (fm_cur->ps_name != NULL)
        fnstr_append(fm_cur->ps_name);
    fnstr_append(" Encoding: ");
    if (fm_cur->encoding != NULL && (fm_cur->encoding)->name != NULL)
        fnstr_append((fm_cur->encoding)->name);
    else
        fnstr_append("built-in");
    fnstr_append(" CharSet: ");
    assert(glyph_names != NULL);
    for (i = 0; i <= MAX_CHAR_CODE; i++)
        if (pdfcharmarked(tex_font, i) && glyph_names[i] != notdef) {
            fnstr_append(" /");
            fnstr_append(glyph_names[i]);
        }
    if (fm_cur->charset != NULL) {
        fnstr_append(" Extra CharSet: ");
        fnstr_append(fm_cur->charset);
    }
    crc = crc32(0L, Z_NULL, 0);
    crc = crc32(crc, (Bytef*)char_array, strlen(char_array));
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
    fm_cur->subset_tag = xstrdup(tag);
}

void pdf_puts(const char *s)
{
    pdfroom(strlen(s) + 1);
    while (*s)
        pdfbuf[pdfptr++] = *s++;
}

void pdf_printf(const char *fmt,...)
{
    va_list args;
    va_start(args, fmt);
    vsprintf(print_buf, fmt, args);
    pdf_puts(print_buf);                                    
    va_end(args);
}

strnumber maketexstring(const char *s)
{
    int l;
    if (s == NULL || *s == 0)
        return getnullstr();
    l = strlen(s);
    check_buf(poolptr + l, poolsize);
    while (l-- > 0)
        strpool[poolptr++] = *s++;
    last_tex_string = makestring();
    return last_tex_string;
}

void tex_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsprintf(print_buf, fmt, args);
    print(maketexstring(print_buf));
    flushstr(last_tex_string);
    xfflush(stdout);
    va_end(args);
}

/* Helper for pdftex_fail. */
static void safe_print(const char *str)
{
    const char *c;
    for (c = str; *c; ++c)
        print(*c);
}

/* pdftex_fail may be called when a buffer overflow has happened/is
   happening, therefore may not call mktexstring.  However, with the
   current implementation it appears that error messages are misleading,
   possibly because pool overflows are detected too late. */
void pdftex_fail(const char *fmt,...)
{
    va_list args;
    va_start(args, fmt);
    println();
    safe_print("Error: ");
    safe_print(program_invocation_name);
    if (cur_file_name) {
        safe_print(" (file ");
        safe_print(cur_file_name);
        safe_print(")");
    }
    safe_print(": ");
    vsprintf(print_buf, fmt, args);
    safe_print(print_buf);
    va_end(args);
    println();
    safe_print(" ==> Fatal error occurred, the output PDF file is not finished!");
    println();
    exit(-1);
}

void pdftex_warn(const char *fmt,...)
{
    va_list args;
    va_start(args, fmt);
    println();
    tex_printf("Warning: %s", program_invocation_name);
    if (cur_file_name)
        tex_printf(" (file %s)", cur_file_name);
    tex_printf(": ");
    vsprintf(print_buf, fmt, args);
    print(maketexstring(print_buf));
    flushstr(last_tex_string);
    va_end(args);
    println();
}

char *makecstring(integer s)
{
    static char cstrbuf[MAX_CSTRING_LEN];
    char *p = cstrbuf;
    int i, l = strstart[s + 1] - strstart[s];
    check_buf(l + 1, MAX_CSTRING_LEN);
    for (i = 0; i < l; i++)
        *p++ = strpool[i + strstart[s]];
    *p = 0;
    return cstrbuf;
}

/*
boolean str_eq_cstr(strnumber n, char *s)
{
    int l;
    if (s == NULL || n == 0)
        return false;
    l = strstart[n];
    while (*s && l < strstart[n + 1] && *s == strpool[l])
        l++, s++;
    return !*s && l == strstart[n + 1];
}
*/

void setjobid(int year, int month, int day, int time, int pdftexversion, int pdftexrevision)
{
    char *name_string, *format_string, *s;

    if (job_id_string != NULL)
        return;

    name_string = xstrdup(makecstring(jobname));
    format_string = xstrdup(makecstring(formatident));
    s = xtalloc(SMALL_BUF_SIZE + 
                strlen(name_string) + 
                strlen(format_string) + 
                strlen(ptexbanner) + 
                strlen(versionstring) + 
                strlen(kpathsea_version_string), char);
    /* The Web2c version string starts with a space.  */
    sprintf(s, "%.4d/%.2d/%.2d %.2d:%.2d %s %s %s%s %s",
            year, month, day, time/60, time%60, 
            name_string, format_string, ptexbanner, 
            versionstring, kpathsea_version_string);
    job_id_string = xstrdup(s);
    xfree(s);
    xfree(name_string);
    xfree(format_string);
}

void makepdftexbanner(void)
{
    static boolean pdftexbanner_init = false;
    char *s;

    if (pdftexbanner_init)
        return;

    s = xtalloc(SMALL_BUF_SIZE + 
                strlen(ptexbanner) + 
                strlen(versionstring) + 
                strlen(kpathsea_version_string), char);
    /* The Web2c version string starts with a space.  */
    sprintf(s, "%s%s %s", ptexbanner, versionstring, kpathsea_version_string);
    pdftexbanner = maketexstring(s);
    xfree(s);
    pdftexbanner_init = true;
}

strnumber getresnameprefix(void)
{
/*     static char name_str[] = */
/* "!\"$&'*+,-.0123456789:;=?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\" */
/* "^_`abcdefghijklmnopqrstuvwxyz|~"; */
    static char name_str[] =
"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    char prefix[7]; /* make a tag of 6 chars long */
    unsigned long crc;
    int i, base = strlen(name_str);
    crc = crc32(0L, Z_NULL, 0);
    crc = crc32(crc, (Bytef*)job_id_string, strlen(job_id_string));
    for (i = 0; i < 6; i++) {
        prefix[i] = name_str[crc % base];
        crc /= base;
    }
    prefix[6] = 0;
    return maketexstring(prefix);
}

size_t xfwrite(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    if (fwrite(ptr, size, nmemb, stream) != nmemb)
        pdftex_fail("fwrite() failed");
    return nmemb;
}

int xfflush(FILE *stream)
{
    if (fflush(stream) != 0)
        pdftex_fail("fflush() failed");
    return 0;
}

int xgetc(FILE *stream)
{
    int c = getc(stream);
    if (c < 0 && c != EOF)
        pdftex_fail("getc() failed");
    return c;
}

int xputc(int c, FILE *stream)
{
    int i = putc(c, stream);
    if (i < 0)
        pdftex_fail("putc() failed");
    return i;
}

void writestreamlength(integer length, integer offset)
{
    integer save_offset;
    if (jobname_cstr == NULL)
        jobname_cstr = xstrdup(makecstring(jobname));
    save_offset = xftell(pdffile, jobname_cstr);
    xfseek(pdffile, offset, SEEK_SET, jobname_cstr);
    fprintf(pdffile, "%li", (long int)length);
    xfseek(pdffile, pdfoffset(), SEEK_SET, jobname_cstr);
}

scaled extxnoverd(scaled x, scaled n, scaled d)
{
    double r = (((double)x)*((double)n))/((double)d);
    if (r > 0)
        r += 0.5;
    else
        r -= 0.5;
    if (r >= (double)maxinteger || r <= -(double)maxinteger)
        pdftex_warn("arithmetic: number too big");
    return r;
}

void libpdffinish()
{
    xfree(fb_array);
    xfree(char_array);
    xfree(job_id_string);
    fm_free();
    t1_free();
    enc_free();
    img_free();
    vf_free();
    epdf_free();
}

/* Converts any string given in in in an allowed PDF string which can be
 * handled by printf et.al.: \ is escaped to \\, parenthesis are escaped and
 * control characters are octal encoded.
 * This assumes that the string does not contain any already escaped
 * characters!
 */
char *convertStringToPDFString (char *in, int len)
{
    static char pstrbuf[MAX_PSTRING_LEN];
    char *out = pstrbuf;
    int i, j;
    char buf[5];
    j = 0;
    for (i = 0; i < len; i++) {
        check_buf(j + sizeof(buf), MAX_PSTRING_LEN);
        if (((unsigned char)in[i] < '!') || ((unsigned char)in[i] > '~')){
            /* convert control characters into oct */
            sprintf (buf, "\\%03o", (unsigned int)(unsigned char)in[i]);
            out[j++] = buf[0];
            out[j++] = buf[1];
            out[j++] = buf[2];
            out[j++] = buf[3];
            }
        else if ((in[i] == '(') || (in[i] == ')')) {
            /* escape paranthesis */
            out[j++] = '\\';
            out[j++] = in[i];
            }
        else if (in[i] == '\\') {
            /* escape backslash */
            out[j++] = '\\';
            out[j++] = '\\';
            }
        else {
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
void escapestring(poolpointer in)
{
    poolpointer out = poolptr;
    unsigned char ch;
    while (in < out) {
        if (poolptr + 4 >= poolsize) {
            poolptr = poolsize;
            /* error by str_toks that calls str_room(1) */
            return;
        }
        
        ch = (unsigned char)strpool[in++];
        
        if ((ch < '!') || (ch > '~')) {
            /* convert control characters into oct */
            sprintf (&strpool[poolptr], "\\%.3o", (unsigned int)ch);
            poolptr += 4;
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
void escapename(poolpointer in)
{
    poolpointer out = poolptr;
    unsigned char ch;
    
    while (in < out) {
        if (poolptr + 3 >= poolsize) {
            poolptr = poolsize;
            /* error by str_toks that calls str_room(1) */
            return;
        }
        
        ch = (unsigned char)strpool[in++];
        
        if ((ch >= 1 && ch <= 32) || ch >= 127) {
            /* escape */
            sprintf(&strpool[poolptr], "#%.2X", (unsigned int)ch);
            poolptr += 3;
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
            sprintf(&strpool[poolptr], "#%.2X", (unsigned int)ch);
            poolptr += 3;
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
void escapehex(poolpointer in)
{
    poolpointer out = poolptr;
    unsigned char ch;
    
    while (in < out) {
        if (poolptr + 2 >= poolsize) {
            poolptr = poolsize;
            /* error by str_toks that calls str_room(1) */
            return;
        }

        ch = (unsigned char)strpool[in++];
    
        sprintf(&strpool[poolptr], "%.2X", (unsigned int)ch);
        poolptr += 2;
    }
}

/* Unescape any given hexadecimal string.

   Last hex digit can be omitted, it is replaced by zero, see
   PDF specification.
   
   Invalid digits are silently ignored.

   See escapename for description of parameters.
*/
void unescapehex(poolpointer in)
{
    poolpointer out = poolptr;
    unsigned char ch, a;
    boolean first = true;
    
    while (in < out) {
        if (poolptr + 1 >= poolsize) {
            poolptr = poolsize;
            /* error by str_toks that calls str_room(1) */
            return;
        }

        ch = (unsigned char)strpool[in++];

        if ((ch >= '0') && (ch <= '9')) {
            ch -= '0';
        }
        else if ((ch >= 'A') && (ch <= 'F')) {
            ch -= 'A' - 10;
        }
        else if ((ch >= 'a') && (ch <= 'f')) {
            ch -= 'a' - 10;
        }
        else {
            continue; /* ignore wrong character */
        }
        
        if (first) {
            a = ch << 4;
            first = false;
            continue;
        }
        
        strpool[poolptr++] = a + ch;
        first = true;
    }
    if (!first) { /* last hex digit is omitted */
        strpool[poolptr++] = ch << 4;
    }
}


/* Converts any string given in in in an allowed PDF string which is 
 * hexadecimal encoded;
 * sizeof(out) should be at least lin*2+1.
 */
void convertStringToHexString (char *in, char *out, int lin)
{
    int i, j;
    char buf[3];
    j = 0;
    for (i = 0; i < lin; i++) {
        sprintf (buf, "%02X", (unsigned int)(unsigned char)in[i]);
        out[j++] = buf[0];
        out[j++] = buf[1];
        }
    out[j] = '\0';
}

/* Compute the ID string as per PDF1.4 9.3:
  <blockquote>
    File identifers are defined by the optional ID entry in a PDF file's
    trailer dictionary (see Section 3.4.4, "File Trailer"; see also
    implementation note 105 in Appendix H). The value of this entry is an
    array of two strings. The first string is a permanent identifier based
    on the contents of the file at the time it was originally created, and
    does not change when the file is incrementally updated. The second
    string is a changing identifier based on the file's contents at the
    time it was last updated. When a file is first written, both
    identifiers are set to the same value. If both identifiers match when a
    file reference is resolved, it is very likely that the correct file has
    been found; if only the first identifier matches, then a different
    version of the correct file has been found. 
        To help ensure the uniqueness of file identifiers, it is recommend
    that they be computed using a message digest algorithm such as MD5
    (described in Internet RFC 1321, The MD5 Message-Digest Algorithm; see
    the Bibliography), using the following information (see implementation
    note 106 in Appendix H): 
    - The current time 
    - A string representation of the file's location, usually a pathname 
    - The size of the file in bytes 
    - The values of all entries in the file's document information
      dictionary (see Section 9.2.1,  Document Information Dictionary )
  </blockquote>
  This stipulates only that the two IDs must be identical when the file is
  created and that they should be reasonably unique. Since it's difficult
  to get the file size at this point in the execution of pdfTeX and
  scanning the info dict is also difficult, we start with a simpler
  implementation using just the first two items.
 */
void printID (strnumber filename)
{
    time_t t;
    size_t size;
    char time_str[32];
    md5_state_t state;
    md5_byte_t digest[16];
    char id[64];
    char *file_name;
    char pwd[4096];
    /* start md5 */
    md5_init(&state);
    /* get the time */
    t = time(NULL);
    size = strftime (time_str, sizeof(time_str), "%Y%m%dT%H%M%SZ", gmtime(&t));
    md5_append(&state, (const md5_byte_t *)time_str, size);
    /* get the file name */
    if (getcwd(pwd, sizeof(pwd)) == NULL)
        pdftex_fail("getcwd() failed (path too long?)");
    file_name = makecstring(filename);
    md5_append(&state, (const md5_byte_t *)pwd, strlen(pwd));
    md5_append(&state, (const md5_byte_t *)"/", 1);
    md5_append(&state, (const md5_byte_t *)file_name, strlen(file_name));
    /* finish md5 */
    md5_finish(&state, digest);
    /* write the IDs */
    convertStringToHexString ((char*)digest, id, 16);
    pdf_printf("/ID [<%s> <%s>]", id, id);
}

/* Print the /CreationDate entry.

  PDF Reference, third edition says about the expected date format:
  <blockquote>
    3.8.2 Dates

      PDF defines a standard date format, which closely follows that of
      the international standard ASN.1 (Abstract Syntax Notation One),
      defined in ISO/IEC 8824 (see the Bibliography). A date is a string
      of the form

        (D:YYYYMMDDHHmmSSOHH'mm')

      where

        YYYY is the year
        MM is the month
        DD is the day (01-31)
        HH is the hour (00-23)
        mm is the minute (00-59)
        SS is the second (00-59)
        O is the relationship of local time to Universal Time (UT),
          denoted by one of the characters +, -, or Z (see below)
        HH followed by ' is the absolute value of the offset from UT
          in hours (00-23)
        mm followed by ' is the absolute value of the offset from UT
          in minutes (00-59)

      The apostrophe character (') after HH and mm is part of the syntax.
      All fields after the year are optional. (The prefix D:, although also
      optional, is strongly recommended.) The default values for MM and DD
      are both 01; all other numerical fields default to zero values.  A plus
      sign (+) as the value of the O field signifies that local time is
      later than UT, a minus sign (-) that local time is earlier than UT,
      and the letter Z that local time is equal to UT. If no UT information
      is specified, the relationship of the specified time to UT is
      considered to be unknown. Whether or not the time zone is known, the
      rest of the date should be specified in local time.

      For example, December 23, 1998, at 7:52 PM, U.S. Pacific Standard
      Time, is represented by the string

        D:199812231952-08'00'
  </blockquote>

  The main difficulty is get the time zone offset. strftime() does this in ISO
  C99 (e.g. newer glibc) with %z, but we have to work with other systems (e.g.
  Solaris 2.5). 
*/

time_t start_time = 0;
#define TIME_STR_SIZE 30
char start_time_str[TIME_STR_SIZE];
char time_str[TIME_STR_SIZE];
    /* minimum size for time_str is 24: "D:YYYYmmddHHMMSS+HH'MM'" */
    
void makepdftime(time_t t, char *time_str)
{
            
    struct tm lt, gmt;
    size_t size;
    int off, off_hours, off_mins;
 
    /* get the time */
    lt = *localtime(&t);
    size = strftime(time_str, TIME_STR_SIZE, "D:%Y%m%d%H%M%S", &lt);
    /* expected format: "YYYYmmddHHMMSS" */
    if (size == 0) {
        /* unexpected, contents of time_str is undefined */
        time_str[0] = '\0';
        return;
    }

    /* correction for seconds: %S can be in range 00..61,
       the PDF reference expects 00..59,   
       therefore we map "60" and "61" to "59" */
    if (time_str[14] == '6') {
        time_str[14] = '5';
        time_str[15] = '9';
        time_str[16] = '\0'; /* for safety */
    }

    /* get the time zone offset */
    gmt = *gmtime(&t);

    /* this calculation method was found in exim's tod.c */
    off = 60*(lt.tm_hour - gmt.tm_hour) + lt.tm_min - gmt.tm_min;
    if (lt.tm_year != gmt.tm_year) {
        off += (lt.tm_year > gmt.tm_year) ? 1440 : -1440;
    }
    else if (lt.tm_yday != gmt.tm_yday) {
        off += (lt.tm_yday > gmt.tm_yday) ? 1440 : -1440;
    }

    if (off == 0) {
        time_str[size++] = 'Z';
        time_str[size] = 0;
    }
    else {
        off_hours = off/60;
        off_mins = abs(off - off_hours*60);
        sprintf(&time_str[size], "%+03i'%02d'", off_hours, off_mins);
    }
}

void initstarttime() {
    if (start_time == 0) {
        start_time = time((time_t *) NULL);
        makepdftime(start_time, start_time_str);
    }
}

void printcreationdate() {
    initstarttime();
    pdf_printf("/CreationDate (%s)\n", start_time_str);
}

void getcreationdate()
{
    /* put creation date on top of string pool and update poolptr */
    poolpointer out = poolptr;
    unsigned char ch;
    int len = strlen(start_time_str);
    
    initstarttime();
    
    if (poolptr + len >= poolsize) {
        poolptr = poolsize;
        /* error by str_toks that calls str_room(1) */
        return;
    }

    memcpy(&strpool[poolptr], start_time_str, len);
    poolptr += len;
}

void getfilemoddate(strnumber s) {
    struct stat file_data;
    
    char *file_name = kpse_find_tex(makecfilename(s));
    if (file_name == NULL) {
        return; /* empty string */
    }
    
    /* get file status */
    if (stat(file_name, &file_data) == 0) {
        int len;
        
        makepdftime(file_data.st_mtime, time_str);
        len = strlen(time_str);
        if (poolptr + len >= poolsize) {
            poolptr = poolsize;
            /* error by str_toks that calls str_room(1) */
        }
        else {
            memcpy(&strpool[poolptr], time_str, len);
            poolptr += len;
        }
    }
    /* else { errno contains error code } */
    
    xfree(file_name);
}

void getfilesize(strnumber s) {
    struct stat file_data;
    
    char *file_name = kpse_find_tex(makecfilename(s));
    if (file_name == NULL) {
        return; /* empty string */
    }
    
    /* get file status */
    if (stat(file_name, &file_data) == 0) {
        int len;
        char buf[20];
        
        /* st_size has type off_t */
        sprintf(buf, "%lu", (long unsigned int)file_data.st_size);
        len = strlen(buf);
        if (poolptr + len >= poolsize) {
            poolptr = poolsize;
            /* error by str_toks that calls str_room(1) */
        }
        else {
            memcpy(&strpool[poolptr], buf, len);
            poolptr += len;
        }
    }
    /* else { errno contains error code } */
    
    xfree(file_name);
}

#define DIGEST_SIZE 16
#define FILE_BUF_SIZE 1024

void getmd5sum(strnumber s, boolean file) {
    md5_state_t state;
    md5_byte_t digest[DIGEST_SIZE];
    char outbuf[2 * DIGEST_SIZE + 1];
    int len = 2 * DIGEST_SIZE;
    
    if (file) {
        char file_buf[FILE_BUF_SIZE];
        int read = 0;
        FILE *f;
        
        char *file_name = kpse_find_tex(makecfilename(s));
        if (file_name == NULL) {
            return; /* empty string */
        }
        /* in case of error the empty string is returned,
           no need for xfopen that aborts on error.
        */
        f = fopen(file_name, FOPEN_RBIN_MODE);
        if (f == NULL) {
            xfree(file_name);
            return;
        }
        md5_init(&state);
        while ((read = fread(&file_buf, sizeof(char),
                             FILE_BUF_SIZE, f)) > 0) {
            md5_append(&state, (const md5_byte_t *)file_buf, read);
        }
        md5_finish(&state, digest);
        fclose(f);
        
        xfree(file_name);
    }
    else {
      /* s contains the data */
      md5_init(&state);
      md5_append(&state,
          (const md5_byte_t *)&strpool[strstart[s]],
          strstart[s + 1] - strstart[s]);
      md5_finish(&state, digest);
    }
    
    if (poolptr + len >= poolsize) {
        /* error by str_toks that calls str_room(1) */
        return;
    }
    convertStringToHexString((char*)digest, outbuf, DIGEST_SIZE);
    memcpy(&strpool[poolptr], outbuf, len);
    poolptr += len;
}

void getfiledump(strnumber s, int offset, int length) {
    FILE *f;
    int read;
    poolpointer data_ptr;
    poolpointer data_end;
    char *file_name;

    if (length == 0) {
        /* empty result string */
        return;
    }
    
    if (poolptr + 2 * length + 1 >= poolsize) {
        /* no place for result */
        poolptr = poolsize;
        /* error by str_toks that calls str_room(1) */
        return;
    }
    
    file_name = kpse_find_tex(makecfilename(s));
    if (file_name == NULL) {
        return; /* empty string */
    }
    
    /* read file data */
    f = fopen(file_name, FOPEN_RBIN_MODE);
    if (f == NULL) {
        xfree(file_name);
        return;
    }
    if (fseek(f, (long)offset, SEEK_SET) != 0) {
        xfree(file_name);
        return;
    }
    /* there is enough space in the string pool, the read
       data are put in the upper half of the result, thus
       the conversion to hex can be done without overwriting
       unconverted bytes. Be aware that sprintf also appends
       a nul byte at the end. */
    data_ptr = poolptr + length;
    read = fread(&strpool[data_ptr],
                 sizeof(char), length, f);
    fclose(f);

    /* convert to hex */
    data_end = data_ptr + read;
    for (; data_ptr < data_end; data_ptr++) {
        sprintf(&strpool[poolptr], "%.2X",
                (unsigned int)strpool[data_ptr]);
        poolptr += 2;
    }
    xfree(file_name);
}

#define DEFAULT_SUB_MATCH_COUNT 10
int sub_match_count = DEFAULT_SUB_MATCH_COUNT;
regmatch_t *pmatch = NULL;
char *match_string = NULL;

void matchstrings(strnumber s, strnumber t, int subcount, boolean icase) {
    regex_t preg;
    int cflags = REG_EXTENDED;
    int eflags = 0;
    int ret;
    char *str;
    
    if (icase) {
        cflags |= REG_ICASE;
    }
    
    if (poolptr + 10 >= poolsize) {
        poolptr = poolsize;
        return;
    }
    
    str = makecstring(s);
    ret = regcomp(&preg, str, cflags);
    if (ret != 0) {
        int size = regerror(ret, &preg, NULL, 0);
        str = xtalloc(size, char);
        regerror(ret, &preg, str, size);
        pdftex_warn("%s%s", "\\pdfmatch: ", str);
        xfree(str);
        strpool[poolptr++] = '-';
        strpool[poolptr++] = '1';
    }
    else {
        str = makecstring(t);
        sub_match_count = ((subcount < 0) ?
                           DEFAULT_SUB_MATCH_COUNT : subcount);
        xfree(pmatch);
        if (sub_match_count > 0) {
           pmatch = xtalloc(sub_match_count, regmatch_t);
        }
        ret = regexec(&preg, str, sub_match_count, pmatch, eflags);
        xfree(match_string);
        match_string = xstrdup(str);
        if (ret == 0) {
            strpool[poolptr++] = '1';
        }
        else { /* REG_NOMATCH */
            strpool[poolptr++] = '0';
        }
    }
    
    regfree(&preg);
}

void getmatch(int i) {
    int size = 0;
    int len = 0;
    
    boolean found = i < sub_match_count
                    && match_string != NULL
                    && pmatch[i].rm_so >= 0
                    && i >= 0;
    
    if (found) {
        len = pmatch[i].rm_eo - pmatch[i].rm_so;
        size = 20 + len;
        /* 20: place for integer number and '->' */
    }
    else {
        size = 4;
    }
    
    if (poolptr + size >= poolsize) {
        poolptr = poolsize;
        return;
    }
    
    if (found) {
        sprintf(&strpool[poolptr], "%d", pmatch[i].rm_so);
        poolptr += strlen(&strpool[poolptr]);
        strpool[poolptr++] = '-';
        strpool[poolptr++] = '>';
        memcpy(&strpool[poolptr], &match_string[pmatch[i].rm_so], len);
        poolptr += len;
        return;
    }
    
    strpool[poolptr++] = '-';
    strpool[poolptr++] = '1';
    strpool[poolptr++] = '-';
    strpool[poolptr++] = '>';
}


/* makecfilename
  input/ouput same as makecstring:
    input: string number
    output: C string with quotes removed.
*/
char *makecfilename(strnumber s) {
    char *name = makecstring(s);
    char *p = name;
    char *q = name;

    while (*p) {
        if (*p != '"')
            *q++ = *p;
        p++;
    }
    *q = '\0';
    fprintf(stderr, " %s\n", name);
    return name;
}
