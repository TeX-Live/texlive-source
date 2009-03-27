/* utils.c

   Copyright 1996-2006 Han The Thanh <thanh@pdftex.org>
   Copyright 2006-2009 Taco Hoekwater <taco@luatex.org>

   This file is part of LuaTeX.

   LuaTeX is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2 of the License, or (at your
   option) any later version.

   LuaTeX is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
   License for more details.

   You should have received a copy of the GNU General Public License along
   with LuaTeX; if not, see <http://www.gnu.org/licenses/>. */

#include "openbsd-compat.h"
#ifdef HAVE_ASPRINTF            /* asprintf is not defined in openbsd-compat.h, but in stdio.h */
#  include <stdio.h>
#endif

#include "sys/types.h"
#ifndef __MINGW32__
#  include "sysexits.h"
#else
#  define EX_SOFTWARE 70
#endif
#include "md5.h"
#include <kpathsea/c-proto.h>
#include <kpathsea/c-stat.h>
#include <kpathsea/c-fopen.h>
#include <string.h>
#include <time.h>
#include <float.h>              /* for DBL_EPSILON */
#include "zlib.h"
#include "ptexlib.h"

#include "png.h"
#include "xpdf/config.h"        /* just to get the xpdf version */

static const char __svn_version[] =
    "$Id: utils.c 2085 2009-03-22 15:21:50Z hhenkel $ $URL: http://scm.foundry.supelec.fr/svn/luatex/trunk/src/texk/web2c/luatexdir/utils/utils.c $";

#define check_nprintf(size_get, size_want) \
    if ((unsigned)(size_get) >= (unsigned)(size_want)) \
        pdftex_fail ("snprintf failed: file %s, line %d", __FILE__, __LINE__);

char *cur_file_name = NULL;
strnumber last_tex_string;
static char print_buf[PRINTF_BUF_SIZE];
static char *jobname_cstr = NULL;
static char *job_id_string = NULL;
extern string ptexbanner;       /* from web2c/lib/texmfmp.c */
extern string versionstring;    /* from web2c/lib/version.c */
extern KPSEDLL string kpathsea_version_string;  /* from kpathsea/version.c */

size_t last_ptr_index;          /* for use with alloc_array */

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

void fb_putchar(eight_bits b)
{
    alloc_array(fb, 1, SMALL_ARRAY_SIZE);
    *fb_ptr++ = b;
}

void fb_flush(void)
{
    fb_entry *p;
    integer n;
    for (p = fb_array; p < fb_ptr;) {
        n = pdf_buf_size - pdf_ptr;
        if (fb_ptr - p < n)
            n = fb_ptr - p;
        memcpy(pdf_buf + pdf_ptr, p, (unsigned) n);
        pdf_ptr += n;
        if (pdf_ptr == pdf_buf_size)
            pdf_flush();
        p += n;
    }
    fb_ptr = fb_array;
}

#define SUBSET_TAG_LENGTH 6
void make_subset_tag(fd_entry * fd)
{
    int i, j = 0, a[SUBSET_TAG_LENGTH];
    md5_state_t pms;
    char *glyph;
    glw_entry *glw_glyph;
    struct avl_traverser t;
    md5_byte_t digest[16];
    void **aa;
    static struct avl_table *st_tree = NULL;
    if (st_tree == NULL)
        st_tree = avl_create(comp_string_entry, NULL, &avl_xallocator);
    assert(fd != NULL);
    assert(fd->gl_tree != NULL);
    assert(fd->fontname != NULL);
    assert(fd->subset_tag == NULL);
    fd->subset_tag = xtalloc(SUBSET_TAG_LENGTH + 1, char);
    do {
        md5_init(&pms);
        avl_t_init(&t, fd->gl_tree);
        if (is_cidkeyed(fd->fm)) {      /* glw_entry items */
            for (glw_glyph = (glw_entry *) avl_t_first(&t, fd->gl_tree);
                 glw_glyph != NULL; glw_glyph = (glw_entry *) avl_t_next(&t)) {
                glyph = malloc(24);
                sprintf(glyph, "%05u%05u ", glw_glyph->id, glw_glyph->wd);
                md5_append(&pms, (md5_byte_t *) glyph, strlen(glyph));
                free(glyph);
            }
        } else {
            for (glyph = (char *) avl_t_first(&t, fd->gl_tree); glyph != NULL;
                 glyph = (char *) avl_t_next(&t)) {
                md5_append(&pms, (md5_byte_t *) glyph, strlen(glyph));
                md5_append(&pms, (md5_byte_t *) " ", 1);
            }
        }
        md5_append(&pms, (md5_byte_t *) fd->fontname, strlen(fd->fontname));
        md5_append(&pms, (md5_byte_t *) & j, sizeof(int));      /* to resolve collision */
        md5_finish(&pms, digest);
        for (a[0] = 0, i = 0; i < 13; i++)
            a[0] += digest[i];
        for (i = 1; i < SUBSET_TAG_LENGTH; i++)
            a[i] = a[i - 1] - digest[i - 1] + digest[(i + 12) % 16];
        for (i = 0; i < SUBSET_TAG_LENGTH; i++)
            fd->subset_tag[i] = a[i] % 26 + 'A';
        fd->subset_tag[SUBSET_TAG_LENGTH] = '\0';
        j++;
        assert(j < 100);
    }
    while ((char *) avl_find(st_tree, fd->subset_tag) != NULL);
    aa = avl_probe(st_tree, fd->subset_tag);
    assert(aa != NULL);
    if (j > 2)
        pdftex_warn
            ("\nmake_subset_tag(): subset-tag collision, resolved in round %d.\n",
             j);
}

void pdf_puts(const char *s)
{
    pdfroom(strlen(s) + 1);
    while (*s)
        pdf_buf[pdf_ptr++] = *s++;
}

__attribute__ ((format(printf, 1, 2)))
void pdf_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(print_buf, PRINTF_BUF_SIZE, fmt, args);
    pdf_puts(print_buf);
    va_end(args);
}

strnumber maketexstring(const char *s)
{
    if (s == NULL || *s == 0)
        return get_nullstr();
    return maketexlstring(s, strlen(s));
}

strnumber maketexlstring(const char *s, size_t l)
{
    if (s == NULL || l == 0)
        return get_nullstr();
    check_pool_overflow(pool_ptr + l);
    while (l-- > 0)
        str_pool[pool_ptr++] = *s++;
    last_tex_string = make_string();
    return last_tex_string;
}

/* print a C string through TeX */
void print_string(char *j)
{
    while (*j) {
        print_char(*j);
        j++;
    }
}

/* append a C string to a TeX string */
void append_string(char *s)
{
    if (s == NULL || *s == 0)
        return;
    check_buf(pool_ptr + strlen(s), pool_size);
    while (*s)
        str_pool[pool_ptr++] = *s++;
    return;
}

__attribute__ ((format(printf, 1, 2)))
void tex_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(print_buf, PRINTF_BUF_SIZE, fmt, args);
    print_string(print_buf);
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

void remove_pdffile(void)
{
    if (!kpathsea_debug && output_file_name && !fixed_pdf_draftmode) {
        xfclose(pdf_file, makecstring(output_file_name));
        remove(makecstring(output_file_name));
    }
}

/* pdftex_fail may be called when a buffer overflow has happened/is
   happening, therefore may not call mktexstring.  However, with the
   current implementation it appears that error messages are misleading,
   possibly because pool overflows are detected too late.

   The output format of this fuction must be the same as pdf_error in
   pdftex.web! */

__attribute__ ((noreturn, format(printf, 1, 2)))
void pdftex_fail(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    print_ln();
    safe_print("!LuaTeX error");
    if (cur_file_name) {
        safe_print(" (file ");
        safe_print(cur_file_name);
        safe_print(")");
    }
    safe_print(": ");
    vsnprintf(print_buf, PRINTF_BUF_SIZE, fmt, args);
    safe_print(print_buf);
    va_end(args);
    print_ln();
    remove_pdffile();
    safe_print(" ==> Fatal error occurred, no output PDF file produced!");
    print_ln();
    if (kpathsea_debug) {
        abort();
    } else {
        exit(EX_SOFTWARE);
    }
}

/* The output format of this fuction must be the same as pdf_warn in
   pdftex.web! */

__attribute__ ((format(printf, 1, 2)))
void pdftex_warn(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    print_ln();
    tex_printf("LuaTeX warning");
    if (cur_file_name)
        tex_printf(" (file %s)", cur_file_name);
    tex_printf(": ");
    vsnprintf(print_buf, PRINTF_BUF_SIZE, fmt, args);
    print_string(print_buf);
    va_end(args);
    print_ln();
}

void tex_error(char *msg, char **hlp)
{
    strnumber aa = 0, bb = 0, cc = 0, dd = 0, ee = 0;
    int k = 0;
    if (hlp != NULL) {
        while (hlp[k] != NULL)
            k++;
        if (k > 0)
            aa = maketexstring(hlp[0]);
        if (k > 1)
            bb = maketexstring(hlp[1]);
        if (k > 2)
            cc = maketexstring(hlp[2]);
        if (k > 3)
            dd = maketexstring(hlp[3]);
        if (k > 4)
            ee = maketexstring(hlp[4]);
    }
    do_print_err(maketexstring(msg));
    flush_str(last_tex_string);

/* *INDENT-OFF* */
    switch (k) {
    case 0: dohelp0(); break;
    case 1: dohelp1(aa); break;
    case 2: dohelp2(aa, bb); break;
    case 3: dohelp3(aa, bb, cc); break;
    case 4: dohelp4(aa, bb, cc, dd); break;
    case 5: dohelp5(aa, bb, cc, dd, ee); break;
    }
/* *INDENT-ON* */
    error();

    if (ee)
        flush_str(ee);
    if (dd)
        flush_str(dd);
    if (cc)
        flush_str(cc);
    if (bb)
        flush_str(bb);
    if (aa)
        flush_str(aa);
}

void garbage_warning(void)
{
    pdftex_warn("dangling objects discarded, no output file produced.");
    remove_pdffile();
}

char *makecstring(integer s)
{
    size_t l;
    return makeclstring(s, &l);
}

char *makeclstring(integer s, size_t * len)
{
    static char *cstrbuf = NULL;
    char *p;
    static int allocsize;
    int allocgrow, i, l;
    if (s >= 2097152) {
        s -= 2097152;
        l = str_start[s + 1] - str_start[s];
        *len = l;
        check_buf(l + 1, MAX_CSTRING_LEN);
        if (cstrbuf == NULL) {
            allocsize = l + 1;
            cstrbuf = xmallocarray(char, allocsize);
        } else if (l + 1 > allocsize) {
            allocgrow = allocsize * 0.2;
            if (l + 1 - allocgrow > allocsize)
                allocsize = l + 1;
            else if (allocsize < MAX_CSTRING_LEN - allocgrow)
                allocsize += allocgrow;
            else
                allocsize = MAX_CSTRING_LEN;
            cstrbuf = xreallocarray(cstrbuf, char, allocsize);
        }
        p = cstrbuf;
        for (i = 0; i < l; i++)
            *p++ = str_pool[i + str_start[s]];
        *p = 0;
    } else {
        if (cstrbuf == NULL) {
            allocsize = 5;
            cstrbuf = xmallocarray(char, allocsize);
        }
        if (s <= 0x7F) {
            cstrbuf[0] = s;
            cstrbuf[1] = 0;
            *len = 1;
        } else if (s <= 0x7FF) {
            cstrbuf[0] = 0xC0 + (s / 0x40);
            cstrbuf[1] = 0x80 + (s % 0x40);
            cstrbuf[2] = 0;
            *len = 2;
        } else if (s <= 0xFFFF) {
            cstrbuf[0] = 0xE0 + (s / 0x1000);
            cstrbuf[1] = 0x80 + ((s % 0x1000) / 0x40);
            cstrbuf[2] = 0x80 + ((s % 0x1000) % 0x40);
            cstrbuf[3] = 0;
            *len = 3;
        } else {
            if (s >= 0x10FF00) {
                cstrbuf[0] = s - 0x10FF00;
                cstrbuf[1] = 0;
                *len = 1;
            } else {
                cstrbuf[0] = 0xF0 + (s / 0x40000);
                cstrbuf[1] = 0x80 + ((s % 0x40000) / 0x1000);
                cstrbuf[2] = 0x80 + (((s % 0x40000) % 0x1000) / 0x40);
                cstrbuf[3] = 0x80 + (((s % 0x40000) % 0x1000) % 0x40);
                cstrbuf[4] = 0;
                *len = 4;
            }
        }
    }
    return cstrbuf;
}

void set_job_id(int year, int month, int day, int time)
{
    char *name_string, *format_string, *s;
    size_t slen;
    int i;

    if (job_id_string != NULL)
        return;

    name_string = xstrdup(makecstring(job_name));
    format_string = xstrdup(makecstring(format_ident));
    slen = SMALL_BUF_SIZE +
        strlen(name_string) +
        strlen(format_string) +
        strlen(ptexbanner) +
        strlen(versionstring) + strlen(kpathsea_version_string);
    s = xtalloc(slen, char);
    /* The Web2c version string starts with a space.  */
    i = snprintf(s, slen,
                 "%.4d/%.2d/%.2d %.2d:%.2d %s %s %s%s %s",
                 year, month, day, time / 60, time % 60,
                 name_string, format_string, ptexbanner,
                 versionstring, kpathsea_version_string);
    check_nprintf(i, slen);
    job_id_string = xstrdup(s);
    xfree(s);
    xfree(name_string);
    xfree(format_string);
}

void make_pdftex_banner(void)
{
    static boolean pdftexbanner_init = false;
    char *s;
    size_t slen;
    int i;

    if (pdftexbanner_init)
        return;

    slen = SMALL_BUF_SIZE +
        strlen(ptexbanner) +
        strlen(versionstring) + strlen(kpathsea_version_string);
    s = xtalloc(slen, char);
    /* The Web2c version string starts with a space.  */
    i = snprintf(s, slen,
                 "%s%s %s", ptexbanner, versionstring, kpathsea_version_string);
    check_nprintf(i, slen);
    pdftex_banner = maketexstring(s);
    xfree(s);
    pdftexbanner_init = true;
}

strnumber get_resname_prefix(void)
{
/*     static char name_str[] = */
/* "!\"$&'*+,-.0123456789:;=?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\" */
/* "^_`abcdefghijklmnopqrstuvwxyz|~"; */
    static char name_str[] =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    char prefix[7];             /* make a tag of 6 chars long */
    unsigned long crc;
    short i;
    size_t base = strlen(name_str);
    crc = crc32(0L, Z_NULL, 0);
    crc = crc32(crc, (Bytef *) job_id_string, strlen(job_id_string));
    for (i = 0; i < 6; i++) {
        prefix[i] = name_str[crc % base];
        crc /= base;
    }
    prefix[6] = 0;
    return maketexstring(prefix);
}

size_t xfwrite(void *ptr, size_t size, size_t nmemb, FILE * stream)
{
    if (fwrite(ptr, size, nmemb, stream) != nmemb)
        pdftex_fail("fwrite() failed");
    return nmemb;
}

int xfflush(FILE * stream)
{
    if (fflush(stream) != 0)
        pdftex_fail("fflush() failed (%s)", strerror(errno));
    return 0;
}

int xgetc(FILE * stream)
{
    int c = getc(stream);
    if (c < 0 && c != EOF)
        pdftex_fail("getc() failed (%s)", strerror(errno));
    return c;
}

int xputc(int c, FILE * stream)
{
    int i = putc(c, stream);
    if (i < 0)
        pdftex_fail("putc() failed (%s)", strerror(errno));
    return i;
}

void write_stream_length(integer length, longinteger offset)
{
    if (jobname_cstr == NULL)
        jobname_cstr = xstrdup(makecstring(job_name));
    if (fixed_pdf_draftmode == 0) {
        xfseeko(pdf_file, (off_t) offset, SEEK_SET, jobname_cstr);
        fprintf(pdf_file, "%li", (long int) length);
        xfseeko(pdf_file, (off_t) pdfoffset(), SEEK_SET, jobname_cstr);
    }
}

scaled ext_xn_over_d(scaled x, scaled n, scaled d)
{
    double r = (((double) x) * ((double) n)) / ((double) d);
    if (r > DBL_EPSILON)
        r += 0.5;
    else
        r -= 0.5;
    if (r >= (double) maxinteger || r <= -(double) maxinteger)
        pdftex_warn("arithmetic: number too big");
    return (scaled) r;
}

void libpdffinish()
{
    xfree(fb_array);
    xfree(char_array);
    xfree(job_id_string);
    fm_free();
    t1_free();
    enc_free();
    epdf_free();
    ttf_free();
    sfd_free();
    glyph_unicode_free();
    zip_free();
}

/* Converts any string given in in in an allowed PDF string which can be
 * handled by printf et.al.: \ is escaped to \\, parenthesis are escaped and
 * control characters are octal encoded.
 * This assumes that the string does not contain any already escaped
 * characters!
 */
char *convertStringToPDFString(const char *in, int len)
{
    static char pstrbuf[MAX_PSTRING_LEN];
    char *out = pstrbuf;
    int i, j, k;
    char buf[5];
    j = 0;
    for (i = 0; i < len; i++) {
        check_buf(j + sizeof(buf), MAX_PSTRING_LEN);
        if (((unsigned char) in[i] < '!') || ((unsigned char) in[i] > '~')) {
            /* convert control characters into oct */
            k = snprintf(buf, sizeof(buf),
                         "\\%03o", (unsigned int) (unsigned char) in[i]);
            check_nprintf(k, sizeof(buf));
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
void escapestring(poolpointer in)
{
    const poolpointer out = pool_ptr;
    unsigned char ch;
    int i;
    while (in < out) {
        if (pool_ptr + 4 >= pool_size) {
            pool_ptr = pool_size;
            /* error by str_toks that calls str_room(1) */
            return;
        }

        ch = (unsigned char) str_pool[in++];

        if ((ch < '!') || (ch > '~')) {
            /* convert control characters into oct */
            i = snprintf((char *) &str_pool[pool_ptr], 5,
                         "\\%.3o", (unsigned int) ch);
            check_nprintf(i, 5);
            pool_ptr += i;
            continue;
        }
        if ((ch == '(') || (ch == ')') || (ch == '\\')) {
            /* escape parenthesis and backslash */
            str_pool[pool_ptr++] = '\\';
        }
        /* copy char :-) */
        str_pool[pool_ptr++] = ch;
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
   "pool_ptr" points to the start of the output string and
   after the end when the procedure returns.
*/
void escapename(poolpointer in)
{
    const poolpointer out = pool_ptr;
    unsigned char ch;
    int i;

    while (in < out) {
        if (pool_ptr + 3 >= pool_size) {
            pool_ptr = pool_size;
            /* error by str_toks that calls str_room(1) */
            return;
        }

        ch = (unsigned char) str_pool[in++];

        if ((ch >= 1 && ch <= 32) || ch >= 127) {
            /* escape */
            i = snprintf((char *) &str_pool[pool_ptr], 4,
                         "#%.2X", (unsigned int) ch);
            check_nprintf(i, 4);
            pool_ptr += i;
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
            i = snprintf((char *) &str_pool[pool_ptr], 4,
                         "#%.2X", (unsigned int) ch);
            check_nprintf(i, 4);
            pool_ptr += i;
            break;
        default:
            /* copy */
            str_pool[pool_ptr++] = ch;
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
    const poolpointer out = pool_ptr;
    unsigned char ch;
    int i;

    while (in < out) {
        if (pool_ptr + 2 >= pool_size) {
            pool_ptr = pool_size;
            /* error by str_toks that calls str_room(1) */
            return;
        }

        ch = (unsigned char) str_pool[in++];

        i = snprintf((char *) &str_pool[pool_ptr], 3, "%.2X",
                     (unsigned int) ch);
        check_nprintf(i, 3);
        pool_ptr += 2;
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
    const poolpointer out = pool_ptr;
    unsigned char ch;
    boolean first = true;
    unsigned char a = 0;        /* to avoid warning about uninitialized use of a */
    while (in < out) {
        if (pool_ptr + 1 >= pool_size) {
            pool_ptr = pool_size;
            /* error by str_toks that calls str_room(1) */
            return;
        }

        ch = (unsigned char) str_pool[in++];

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

        str_pool[pool_ptr++] = a + ch;
        first = true;
    }
    if (!first) {               /* last hex digit is omitted */
        str_pool[pool_ptr++] = a;
    }
}

/* Converts any string given in in in an allowed PDF string which is
 * hexadecimal encoded;
 * sizeof(out) should be at least lin*2+1.
 */
static void convertStringToHexString(const char *in, char *out, int lin)
{
    int i, j, k;
    char buf[3];
    j = 0;
    for (i = 0; i < lin; i++) {
        k = snprintf(buf, sizeof(buf),
                     "%02X", (unsigned int) (unsigned char) in[i]);
        check_nprintf(k, sizeof(buf));
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
void print_ID(strnumber filename)
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
    size = strftime(time_str, sizeof(time_str), "%Y%m%dT%H%M%SZ", gmtime(&t));
    md5_append(&state, (const md5_byte_t *) time_str, size);
    /* get the file name */
    if (getcwd(pwd, sizeof(pwd)) == NULL)
        pdftex_fail("getcwd() failed (%s), (path too long?)", strerror(errno));
    file_name = makecstring(filename);
    md5_append(&state, (const md5_byte_t *) pwd, strlen(pwd));
    md5_append(&state, (const md5_byte_t *) "/", 1);
    md5_append(&state, (const md5_byte_t *) file_name, strlen(file_name));
    /* finish md5 */
    md5_finish(&state, digest);
    /* write the IDs */
    convertStringToHexString((char *) digest, id, 16);
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

static time_t start_time = 0;
#define TIME_STR_SIZE 30
static char start_time_str[TIME_STR_SIZE];      /* minimum size for time_str is 24: "D:YYYYmmddHHMMSS+HH'MM'" */

static void makepdftime(time_t t, char *time_str)
{
    struct tm lt, gmt;
    size_t size;
    int i, off, off_hours, off_mins;

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
        time_str[16] = '\0';    /* for safety */
    }

    /* get the time zone offset */
    gmt = *gmtime(&t);

    /* this calculation method was found in exim's tod.c */
    off = 60 * (lt.tm_hour - gmt.tm_hour) + lt.tm_min - gmt.tm_min;
    if (lt.tm_year != gmt.tm_year) {
        off += (lt.tm_year > gmt.tm_year) ? 1440 : -1440;
    } else if (lt.tm_yday != gmt.tm_yday) {
        off += (lt.tm_yday > gmt.tm_yday) ? 1440 : -1440;
    }

    if (off == 0) {
        time_str[size++] = 'Z';
        time_str[size] = 0;
    } else {
        off_hours = off / 60;
        off_mins = abs(off - off_hours * 60);
        i = snprintf(&time_str[size], 9, "%+03d'%02d'", off_hours, off_mins);
        check_nprintf(i, 9);
    }
}

void init_start_time()
{
    if (start_time == 0) {
        start_time = time((time_t *) NULL);
        makepdftime(start_time, start_time_str);
    }
}

void print_creation_date()
{
    init_start_time();
    pdf_printf("/CreationDate (%s)\n", start_time_str);
}

void print_mod_date()
{
    init_start_time();
    pdf_printf("/ModDate (%s)\n", start_time_str);
}

void getcreationdate()
{
    /* put creation date on top of string pool and update pool_ptr */
    size_t len = strlen(start_time_str);

    init_start_time();

    if ((unsigned) (pool_ptr + len) >= (unsigned) pool_size) {
        pool_ptr = pool_size;
        /* error by str_toks that calls str_room(1) */
        return;
    }

    memcpy(&str_pool[pool_ptr], start_time_str, len);
    pool_ptr += len;
}

/* makecfilename
  input/ouput same as makecstring:
    input: string number
    output: C string with quotes removed.
    That means, file names that are legal on some operation systems
    cannot any more be used since pdfTeX version 1.30.4.
*/
char *makecfilename(strnumber s)
{
    char *name = makecstring(s);
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
char *stripzeros(char *a)
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

void initversionstring(char **versions)
{
    (void) asprintf(versions,
                    "Compiled with libpng %s; using libpng %s\n"
                    "Compiled with zlib %s; using zlib %s\n"
                    "Compiled with xpdf version %s\n",
                    PNG_LIBPNG_VER_STRING, png_libpng_ver,
                    ZLIB_VERSION, zlib_version, xpdfVersion);
}

/*************************************************/
/* Color Stack and Matrix Transformation Support */
/*************************************************/

/*
    In the following array and especially stack data structures are used.
    They have the following properties:
    - They automatically grow dynamically.
    - The size never decreases.
    - The variable with name ending in "size" contains the number how many
      entries the data structure can hold.
    - The variable with name ending in "used" contains the number of
      actually used entries.
    - Memory of strings in stack entries must be allocated and
      freed if the stack is cleared.
*/

/* Color Stack */

#define STACK_INCREMENT 8
#define MAX_COLORSTACKS 32768
/* The colorstack number is stored in two bytes (info field of the node) */
/* Condition (newcolorstack): MAX_COLORSTACKS mod STACK_INCREMENT = 0 */

#define COLOR_DEFAULT "0 g 0 G"
/* literal_modes, see pdftex.web */
#define SET_ORIGIN 0
#define DIRECT_PAGE 1
#define DIRECT_ALWAYS 2

/* remember shipout mode: page/form */
static boolean page_mode;

typedef struct {
    char **page_stack;
    char **form_stack;
    char *page_current;
    char *form_current;
    char *form_init;
    int page_size;
    int form_size;
    int page_used;
    int form_used;
    int literal_mode;
    boolean page_start;
} colstack_type;

static colstack_type *colstacks = NULL;
static int colstacks_size = 0;
static int colstacks_used = 0;

/*  Initialization is done, if the color stacks are used,
    init_colorstacks() is defined as macro to avoid unnecessary
    procedure calls.
*/
#define init_colorstacks() if (colstacks_size == 0) colstacks_first_init();
void colstacks_first_init()
{
    colstacks_size = STACK_INCREMENT;
    colstacks = xtalloc(colstacks_size, colstack_type);
    colstacks_used = 1;
    colstacks[0].page_stack = NULL;
    colstacks[0].form_stack = NULL;
    colstacks[0].page_size = 0;
    colstacks[0].form_size = 0;
    colstacks[0].page_used = 0;
    colstacks[0].form_used = 0;
    colstacks[0].page_current = xstrdup(COLOR_DEFAULT);
    colstacks[0].form_current = xstrdup(COLOR_DEFAULT);
    colstacks[0].form_init = xstrdup(COLOR_DEFAULT);
    colstacks[0].literal_mode = DIRECT_ALWAYS;
    colstacks[0].page_start = true;
}

int colorstackused()
{
    init_colorstacks();
    return colstacks_used;
}

/*  newcolorstack()
    A new color stack is setup with the given parameters.
    The stack number is returned or -1 in case of error (no room).
*/
int newcolorstack(integer s, integer literal_mode, boolean page_start)
{
    colstack_type *colstack;
    int colstack_num;
    char *str;

    init_colorstacks();

    /* make room */
    if (colstacks_used == MAX_COLORSTACKS) {
        return -1;
    }
    if (colstacks_used == colstacks_size) {
        colstacks_size += STACK_INCREMENT;
        /* If (MAX_COLORSTACKS mod STACK_INCREMENT = 0) then we don't
           need to check the case that size overruns MAX_COLORSTACKS. */
        colstacks = xreallocarray(colstacks, colstack_type, colstacks_size);
    }
    /* claim new color stack */
    colstack_num = colstacks_used++;
    colstack = &colstacks[colstack_num];
    /* configure the new color stack */
    colstack->page_stack = NULL;
    colstack->form_stack = NULL;
    colstack->page_size = 0;
    colstack->page_used = 0;
    colstack->form_size = 0;
    colstack->form_used = 0;
    colstack->literal_mode = literal_mode;
    colstack->page_start = page_start;
    str = makecstring(s);
    if (*str == 0) {
        colstack->page_current = NULL;
        colstack->form_current = NULL;
        colstack->form_init = NULL;
    } else {
        colstack->page_current = xstrdup(str);
        colstack->form_current = xstrdup(str);
        colstack->form_init = xstrdup(str);
    }
    return colstack_num;
}

#define get_colstack(n) (&colstacks[n])

/* Puts a string on top of the string pool and updates pool_ptr. */
void put_cstring_on_str_pool(poolpointer start, char *str)
{
    size_t len;

    if (str == NULL || *str == 0) {
        return;
    }

    len = strlen(str);
    pool_ptr = start + len;
    if (pool_ptr >= pool_size) {
        pool_ptr = pool_size;
        /* error by str_toks that calls str_room(1) */
        return;
    }
    memcpy(&str_pool[start], str, len);
}

integer colorstackset(int colstack_no, integer s)
{
    colstack_type *colstack = get_colstack(colstack_no);

    if (page_mode) {
        xfree(colstack->page_current);
        colstack->page_current = xstrdup(makecstring(s));
    } else {
        xfree(colstack->form_current);
        colstack->form_current = xstrdup(makecstring(s));
    }
    return colstack->literal_mode;
}

integer colorstackcurrent(int colstack_no)
{
    colstack_type *colstack = get_colstack(colstack_no);

    if (page_mode) {
        put_cstring_on_str_pool(pool_ptr, colstack->page_current);
    } else {
        put_cstring_on_str_pool(pool_ptr, colstack->form_current);
    }
    return colstack->literal_mode;
}

integer colorstackpush(int colstack_no, integer s)
{
    colstack_type *colstack = get_colstack(colstack_no);
    char *str;

    if (page_mode) {
        if (colstack->page_used == colstack->page_size) {
            colstack->page_size += STACK_INCREMENT;
            colstack->page_stack = xretalloc(colstack->page_stack,
                                             colstack->page_size, char *);
        }
        colstack->page_stack[colstack->page_used++] = colstack->page_current;
        str = makecstring(s);
        if (*str == 0) {
            colstack->page_current = NULL;
        } else {
            colstack->page_current = xstrdup(str);
        }
    } else {
        if (colstack->form_used == colstack->form_size) {
            colstack->form_size += STACK_INCREMENT;
            colstack->form_stack = xretalloc(colstack->form_stack,
                                             colstack->form_size, char *);
        }
        colstack->form_stack[colstack->form_used++] = colstack->form_current;
        str = makecstring(s);
        if (*str == 0) {
            colstack->form_current = NULL;
        } else {
            colstack->form_current = xstrdup(str);
        }
    }
    return colstack->literal_mode;
}

integer colorstackpop(int colstack_no)
{
    colstack_type *colstack = get_colstack(colstack_no);

    if (page_mode) {
        if (colstack->page_used == 0) {
            pdftex_warn("pop empty color page stack %u",
                        (unsigned int) colstack_no);
            return colstack->literal_mode;
        }
        xfree(colstack->page_current);
        colstack->page_current = colstack->page_stack[--colstack->page_used];
        put_cstring_on_str_pool(pool_ptr, colstack->page_current);
    } else {
        if (colstack->form_used == 0) {
            pdftex_warn("pop empty color form stack %u",
                        (unsigned int) colstack_no);
            return colstack->literal_mode;
        }
        xfree(colstack->form_current);
        colstack->form_current = colstack->form_stack[--colstack->form_used];
        put_cstring_on_str_pool(pool_ptr, colstack->form_current);
    }
    return colstack->literal_mode;
}

void colorstackpagestart()
{
    int i, j;
    colstack_type *colstack;

    if (page_mode) {
        /* see procedure pdf_out_colorstack_startpage */
        return;
    }

    for (i = 0; i < colstacks_used; i++) {
        colstack = &colstacks[i];
        for (j = 0; j < colstack->form_used; j++) {
            xfree(colstack->form_stack[j]);
        }
        colstack->form_used = 0;
        xfree(colstack->form_current);
        if (colstack->form_init == NULL) {
            colstack->form_current = NULL;
        } else {
            colstack->form_current = xstrdup(colstack->form_init);
        }
    }
}

integer colorstackskippagestart(int colstack_no)
{
    colstack_type *colstack = get_colstack(colstack_no);

    if (!colstack->page_start) {
        return 1;
    }
    if (colstack->page_current == NULL) {
        return 0;
    }
    if (strcmp(COLOR_DEFAULT, colstack->page_current) == 0) {
        return 2;
    }
    return 0;
}

/* stack for \pdfsetmatrix */

typedef struct {
    double a;
    double b;
    double c;
    double d;
    double e;
    double f;
} matrix_entry;
static matrix_entry *matrix_stack = 0;
static int matrix_stack_size = 0;
static int matrix_stack_used = 0;

boolean matrixused()
{
    return matrix_stack_used > 0;
}

/* stack for positions of \pdfsave */
typedef struct {
    scaledpos pos;
    int matrix_stack;
} pos_entry;
static pos_entry *pos_stack = 0;        /* the stack */
static int pos_stack_size = 0;  /* initially empty */
static int pos_stack_used = 0;  /* used entries */

void matrix_stack_room()
{
    matrix_entry *new_stack;

    if (matrix_stack_used >= matrix_stack_size) {
        matrix_stack_size += STACK_INCREMENT;
        new_stack = xtalloc(matrix_stack_size, matrix_entry);
        memcpy((void *) new_stack, (void *) matrix_stack,
               matrix_stack_used * sizeof(matrix_entry));
        xfree(matrix_stack);
        matrix_stack = new_stack;
    }
}

void checkpdfsave(scaledpos pos)
{
    pos_entry *new_stack;

    if (pos_stack_used >= pos_stack_size) {
        pos_stack_size += STACK_INCREMENT;
        new_stack = xtalloc(pos_stack_size, pos_entry);
        memcpy((void *) new_stack, (void *) pos_stack,
               pos_stack_used * sizeof(pos_entry));
        xfree(pos_stack);
        pos_stack = new_stack;
    }
    pos_stack[pos_stack_used].pos.h = pos.h;
    pos_stack[pos_stack_used].pos.v = pos.v;
    if (page_mode) {
        pos_stack[pos_stack_used].matrix_stack = matrix_stack_used;
    }
    pos_stack_used++;
}

void checkpdfrestore(scaledpos pos)
{
    scaledpos diff;
    if (pos_stack_used == 0) {
        pdftex_warn("%s", "\\pdfrestore: missing \\pdfsave");
        return;
    }
    pos_stack_used--;
    diff.h = pos.h - pos_stack[pos_stack_used].pos.h;
    diff.v = pos.v - pos_stack[pos_stack_used].pos.v;
    if (diff.h != 0 || diff.v != 0) {
        pdftex_warn("Misplaced \\pdfrestore by (%dsp, %dsp)", diff.h, diff.v);
    }
    if (page_mode) {
        matrix_stack_used = pos_stack[pos_stack_used].matrix_stack;
    }
}

void pdfshipoutbegin(boolean shipping_page)
{
    pos_stack_used = 0;         /* start with empty stack */

    page_mode = shipping_page;
    if (shipping_page) {
        colorstackpagestart();
    }
}

void pdfshipoutend(boolean shipping_page)
{
    if (pos_stack_used > 0) {
        pdftex_fail("%u unmatched \\pdfsave after %s shipout",
                    (unsigned int) pos_stack_used,
                    ((shipping_page) ? "page" : "form"));
    }
}

/*  \pdfsetmatrix{a b c d}
    e := pos.h
    f := pos.v
    M_top: current active matrix at the top of
           the matrix stack

    The origin of \pdfsetmatrix is the current point.
    The annotation coordinate system is the original
    page coordinate system. When pdfTeX calculates
    annotation rectangles it does not take into
    account this transformations, it uses the original
    coordinate system. To get the corrected values,
    first we go back to the origin, perform the
    transformation and go back:

    (  1   0  0 )   ( a b 0 )   ( 1 0 0 )
    (  0   1  0 ) x ( c d 0 ) x ( 0 1 0 ) x M_top
    ( -e  -f  1 )   ( 0 0 1 )   ( e f 1 )

    ( 1  0  0 )   (  a  b 0 )
  = ( 0  1  0 ) x (  c  d 0 ) x M_top
    ( e  f  1 )   ( -e -f 1 )

    ( a         b         0 )
  = ( c         d         0 ) x M_top
    ( e(1-a)-fc f(1-d)-eb 1 )

*/

void pdfsetmatrix(poolpointer in, scaledpos pos)
{
    /* Argument of \pdfsetmatrix starts with str_pool[in] and ends
       before str_pool[pool_ptr]. */

    matrix_entry x, *y, *z;

    if (page_mode) {
        if (sscanf((const char *) &str_pool[in], " %lf %lf %lf %lf ",
                   &x.a, &x.b, &x.c, &x.d) != 4) {
            pdftex_warn("Unrecognized format of \\pdfsetmatrix{%s}",
                        &str_pool[pool_ptr]);
            return;
        }
        /* calculate this transformation matrix */
        x.e = (double) pos.h * (1.0 - x.a) - (double) pos.v * x.c;
        x.f = (double) pos.v * (1.0 - x.d) - (double) pos.h * x.b;
        matrix_stack_room();
        z = &matrix_stack[matrix_stack_used];
        if (matrix_stack_used > 0) {
            y = &matrix_stack[matrix_stack_used - 1];
            z->a = x.a * y->a + x.b * y->c;
            z->b = x.a * y->b + x.b * y->d;
            z->c = x.c * y->a + x.d * y->c;
            z->d = x.c * y->b + x.d * y->d;
            z->e = x.e * y->a + x.f * y->c + y->e;
            z->f = x.e * y->b + x.f * y->d + y->f;
        } else {
            z->a = x.a;
            z->b = x.b;
            z->c = x.c;
            z->d = x.d;
            z->e = x.e;
            z->f = x.f;
        }
        matrix_stack_used++;
    }
}

/* Apply matrix to point (x,y)

               ( a b 0 )
   ( x y 1 ) x ( c d 0 ) = ( xa+yc+e xb+yd+f 1 )
               ( e f 1 )

   If \pdfsetmatrix wasn't used, then return the value unchanged.
*/

/* Return valeus for matrix tranform functions */
static scaled ret_llx;
static scaled ret_lly;
static scaled ret_urx;
static scaled ret_ury;

scaled getllx()
{
    return ret_llx;
}

scaled getlly()
{
    return ret_lly;
}

scaled geturx()
{
    return ret_urx;
}

scaled getury()
{
    return ret_ury;
}

static int last_llx;
static int last_lly;
static int last_urx;
static int last_ury;

#define DO_ROUND(x)  ((x > 0) ? (x + .5) : (x - .5))
#define DO_MIN(a, b) ((a < b) ? a : b)
#define DO_MAX(a, b) ((a > b) ? a : b)

void do_matrixtransform(scaled x, scaled y, scaled * retx, scaled * rety)
{
    matrix_entry *m = &matrix_stack[matrix_stack_used - 1];
    double x_old = x;
    double y_old = y;
    double x_new = x_old * m->a + y_old * m->c + m->e;
    double y_new = x_old * m->b + y_old * m->d + m->f;
    *retx = (scaled) DO_ROUND(x_new);
    *rety = (scaled) DO_ROUND(y_new);
}

void matrixtransformrect(scaled llx, scaled lly, scaled urx, scaled ury)
{
    scaled x1, x2, x3, x4, y1, y2, y3, y4;

    if (page_mode && matrix_stack_used > 0) {
        last_llx = llx;
        last_lly = lly;
        last_urx = urx;
        last_ury = ury;
        do_matrixtransform(llx, lly, &x1, &y1);
        do_matrixtransform(llx, ury, &x2, &y2);
        do_matrixtransform(urx, lly, &x3, &y3);
        do_matrixtransform(urx, ury, &x4, &y4);
        ret_llx = DO_MIN(DO_MIN(x1, x2), DO_MIN(x3, x4));
        ret_lly = DO_MIN(DO_MIN(y1, y2), DO_MIN(y3, y4));
        ret_urx = DO_MAX(DO_MAX(x1, x2), DO_MAX(x3, x4));
        ret_ury = DO_MAX(DO_MAX(y1, y2), DO_MAX(y3, y4));
    } else {
        ret_llx = llx;
        ret_lly = lly;
        ret_urx = urx;
        ret_ury = ury;
    }
}

void matrixtransformpoint(scaled x, scaled y)
{
    if (page_mode && matrix_stack_used > 0) {
        do_matrixtransform(x, y, &ret_llx, &ret_lly);
    } else {
        ret_llx = x;
        ret_lly = y;
    }
}

void matrixrecalculate(scaled urx)
{
    matrixtransformrect(last_llx, last_lly, urx, last_ury);
}

void check_buffer_overflow(int wsize)
{
    int nsize;
    if (wsize > buf_size) {
        nsize = buf_size + buf_size / 5 + 5;
        if (nsize < wsize) {
            nsize = wsize + 5;
        }
        buffer = (unsigned char *) xreallocarray(buffer, char, nsize);
        buf_size = nsize;
    }
}

#define EXTRA_STRING 500

void check_pool_overflow(int wsize)
{
    int nsize;
    if ((wsize - 1) > pool_size) {
        nsize = pool_size + pool_size / 5 + EXTRA_STRING;
        if (nsize < wsize) {
            nsize = wsize + EXTRA_STRING;
        }
        str_pool = (unsigned char *) xreallocarray(str_pool, char, nsize);
        pool_size = nsize;
    }
}

#define max_integer 0x7FFFFFFF

/* the return value is a decimal number with the point |dd| places from the back,
   |scaled_out| is the number of scaled points corresponding to that.
*/

scaled divide_scaled(scaled s, scaled m, integer dd)
{
    register scaled q;
    register scaled r;
    int i;
    int sign = 1;
    if (s < 0) {
        sign = -sign;
        s = -s;
    }
    if (m < 0) {
        sign = -sign;
        m = -m;
    }
    if (m == 0) {
        pdf_error(maketexstring("arithmetic"),
                  maketexstring("divided by zero"));
    } else if (m >= (max_integer / 10)) {
        pdf_error(maketexstring("arithmetic"), maketexstring("number too big"));
    }
    q = s / m;
    r = s % m;
    for (i = 1; i <= (int) dd; i++) {
        q = 10 * q + (10 * r) / m;
        r = (10 * r) % m;
    }
    /* rounding */
    if (2 * r >= m) {
        q++;
        r -= m;
    }
    return sign * q;
}

/* Same function, but using doubles instead of integers (faster) */

scaled divide_scaled_n(double sd, double md, double n)
{
    double dd, di = 0.0;
    dd = sd / md * n;
    if (dd > 0.0)
        di = floor(dd + 0.5);
    else if (dd < 0.0)
        di = -floor((-dd) + 0.5);
    return (scaled) di;
}


/* C print interface */

void tprint(char *s)
{
    while (*s)
        print_char(*s++);
}

void tprint_nl(char *s)
{
    print_nlp();
    tprint(s);
}

#define escape_char_code 45     /* escape character for token output */
#define int_par(a) zeqtb[static_int_base+(a)].cint      /* an integer parameter */
#define escape_char int_par(escape_char_code)

void tprint_esc(char *s)
{                               /* prints escape character, then |s| */
    int c = -1;                 /* the escape character code */
    if (zeqtb != NULL) {
        c = escape_char;
        if (c >= 0)
            print_char(c);
    }
    tprint(s);
}

void tconfusion(char *s)
{
    confusion(maketexstring(s));
}

#ifdef MSVC

#  include <math.h>
double rint(double x)
{
    double c, f, d1, d2;

    c = ceil(x);
    f = floor(x);
    d1 = fabs(c - x);
    d2 = fabs(x - f);
    if (d1 > d2)
        return f;
    else
        return c;
}

#endif
