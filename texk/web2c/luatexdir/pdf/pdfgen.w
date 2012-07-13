% pdfgen.w

% Copyright 2009-2010 Taco Hoekwater <taco@@luatex.org>

% This file is part of LuaTeX.

% LuaTeX is free software; you can redistribute it and/or modify it under
% the terms of the GNU General Public License as published by the Free
% Software Foundation; either version 2 of the License, or (at your
% option) any later version.

% LuaTeX is distributed in the hope that it will be useful, but WITHOUT
% ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
% FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
% License for more details.

% You should have received a copy of the GNU General Public License along
% with LuaTeX; if not, see <http://www.gnu.org/licenses/>.

@ @c
static const char _svn_version[] =
    "$Id: pdfgen.w 4253 2011-05-09 11:13:00Z taco $"
    "$URL: http://foundry.supelec.fr/svn/luatex/branches/0.70.x/source/texk/web2c/luatexdir/pdf/pdfgen.w $";

#include "ptexlib.h"

@ @c
#include <kpathsea/c-dir.h>
#include <kpathsea/c-ctype.h>
#include "lua/luatex-api.h"
#include "md5.h"

#define is_hex_char isxdigit

#define check_nprintf(size_get, size_want) \
    if ((unsigned)(size_get) >= (unsigned)(size_want)) \
        pdftex_fail ("snprintf failed: file %s, line %d", __FILE__, __LINE__);

PDF static_pdf = NULL;

static char *jobname_cstr = NULL;

@ commandline interface
@c
int pdf_output_option;
int pdf_output_value;
int pdf_draftmode_option;
int pdf_draftmode_value;

halfword pdf_info_toks;         /* additional keys of Info dictionary */
halfword pdf_catalog_toks;      /* additional keys of Catalog dictionary */
halfword pdf_catalog_openaction;
halfword pdf_names_toks;        /* additional keys of Names dictionary */
halfword pdf_trailer_toks;      /* additional keys of Trailer dictionary */
shipping_mode_e global_shipping_mode = NOT_SHIPPING;       /* set to |shipping_mode| when |ship_out| starts */

@ |init_pdf_struct()| is called early, only once, from maincontrol.w

@c
PDF init_pdf_struct(PDF pdf)
{
    assert(pdf == NULL);
    pdf = xtalloc(1, pdf_output_file);
    memset(pdf, 0, sizeof(pdf_output_file));

    pdf->o_mode = OMODE_NONE;   /* will be set by |fix_o_mode()| */
    pdf->o_state = ST_INITIAL;

    pdf->os_obj = xtalloc(pdf_os_max_objs, os_obj_data);
    pdf->os_buf_size = inf_pdf_os_buf_size;
    pdf->os_buf = xtalloc(pdf->os_buf_size, unsigned char);
    pdf->op_buf_size = inf_pdf_op_buf_size;
    pdf->op_buf = xtalloc(pdf->op_buf_size, unsigned char);

    pdf->buf_size = pdf->op_buf_size;
    pdf->buf = pdf->op_buf;

    /* Sometimes it is neccesary to allocate memory for PDF output that cannot
       be deallocated then, so we use |mem| for this purpose. */
    pdf->mem_size = inf_pdf_mem_size;   /* allocated size of |mem| array */
    pdf->mem = xtalloc(pdf->mem_size, int);
    pdf->mem_ptr = 1;           /* the first word is not used so we can use zero as a value for testing
                                   whether a pointer to |mem| is valid  */
    pdf->pstruct = NULL;

    pdf->posstruct = xtalloc(1, posstructure);
    pdf->posstruct->pos.h = 0;
    pdf->posstruct->pos.v = 0;
    pdf->posstruct->dir = dir_TLT;

    pdf->obj_tab_size = (unsigned) inf_obj_tab_size;    /* allocated size of |obj_tab| array */
    pdf->obj_tab = xtalloc(pdf->obj_tab_size + 1, obj_entry);
    memset(pdf->obj_tab, 0, sizeof(obj_entry));

    pdf->minor_version = -1;    /* unset */
    pdf->decimal_digits = 4;
    pdf->gamma = 65536;
    pdf->image_gamma = 65536;
    pdf->image_hicolor = 1;
    pdf->image_apply_gamma = 0;
    pdf->objcompresslevel = 0;
    pdf->compress_level = 0;
    pdf->draftmode = 0;
    pdf->inclusion_copy_font = 1;
    pdf->replace_font = 0;
    pdf->pk_resolution = 0;
    pdf->pk_scale_factor = 0;

    init_dest_names(pdf);
    pdf->page_resources = NULL;

    init_pdf_pagecalculations(pdf);
    pdf->pdflua_ref = new_pdflua();

    return pdf;
}

@ @c
static void pdf_shipout_begin(void)
{
    pos_stack_used = 0;         /* start with empty stack */

    if (global_shipping_mode == SHIPPING_PAGE) {
        colorstackpagestart();
    }
}

static void pdf_shipout_end(void)
{
    if (pos_stack_used > 0) {
        pdftex_fail("%u unmatched \\pdfsave after %s shipout",
                    (unsigned int) pos_stack_used,
                    ((global_shipping_mode == SHIPPING_PAGE) ? "page" : "form"));
    }
}

@  We use |pdf_get_mem| to allocate memory in |mem|.

@c
int pdf_get_mem(PDF pdf, int s)
{                               /* allocate |s| words in |mem| */
    int a;
    int ret;
    if (s > sup_pdf_mem_size - pdf->mem_ptr)
        overflow("PDF memory size (pdf_mem_size)", (unsigned) pdf->mem_size);
    if (pdf->mem_ptr + s > pdf->mem_size) {
        a = pdf->mem_size / 5;
        if (pdf->mem_ptr + s > pdf->mem_size + a) {
            pdf->mem_size = pdf->mem_ptr + s;
        } else if (pdf->mem_size < sup_pdf_mem_size - a) {
            pdf->mem_size = pdf->mem_size + a;
        } else {
            pdf->mem_size = sup_pdf_mem_size;
        }
        pdf->mem = xreallocarray(pdf->mem, int, (unsigned) pdf->mem_size);
    }
    ret = pdf->mem_ptr;
    pdf->mem_ptr = pdf->mem_ptr + s;
    return ret;
}

@ |get_o_mode| translates from |pdf_output| to |o_mode|.

@c
static output_mode get_o_mode(void)
{
    output_mode o_mode;
    if (pdf_output > 0) {
        if (pdf_output == 2009)
            o_mode = OMODE_LUA;
        else
            o_mode = OMODE_PDF;
    } else
        o_mode = OMODE_DVI;
    return o_mode;
}

@ |fix_o_mode| freezes |pdf->o_mode| as soon as anything goes through
the backend, be it \.{PDF}, \.{DVI}, or \.{Lua}.

@c
void fix_o_mode(PDF pdf)
{
    output_mode o_mode = get_o_mode();
    if (pdf->o_mode == OMODE_NONE)
        pdf->o_mode = o_mode;
    else if (pdf->o_mode != o_mode)
        pdf_error("setup",
                  "\\pdfoutput can only be changed before anything is written to the output");
}

@ This ensures that |pdfminorversion| is set only before any bytes have
been written to the generated \.{PDF} file. Here also all variables for
\.{PDF} output are initialized, the \.{PDF} file is opened by |ensure_pdf_open|,
and the \.{PDF} header is written.

@c
void fix_pdf_minorversion(PDF pdf)
{
    if (pdf->minor_version < 0) {       /* unset */
        if ((pdf_minor_version < 0) || (pdf_minor_version > 9)) {
            const char *hlp[] =
                { "The pdfminorversion must be between 0 and 9.",
                "I changed this to 4.", NULL
            };
            char msg[256];
            (void) snprintf(msg, 255,
                            "LuaTeX error (illegal pdfminorversion %d)",
                            (int) pdf_minor_version);
            tex_error(msg, hlp);
            pdf_minor_version = 4;
        }
        pdf->minor_version = pdf_minor_version;
    } else {
        /* Check that variables for \.{PDF} output are unchanged */
        if (pdf->minor_version != pdf_minor_version)
            pdf_error("setup",
                      "\\pdfminorversion cannot be changed after data is written to the PDF file");
        if (pdf->draftmode != pdf_draftmode)
            pdf_error("setup",
                      "\\pdfdraftmode cannot be changed after data is written to the PDF file");
    }
    if (pdf->draftmode != 0) {
        pdf->compress_level = 0;        /* re-fix it, might have been changed inbetween */
        pdf->objcompresslevel = 0;
    }
}

@ @c
#define ZIP_BUF_SIZE  32768

#define check_err(f, fn)                        \
  if (f != Z_OK)                                \
    pdftex_fail("zlib: %s() failed (error code %d)", fn, f)

@ @c
static void write_zip(PDF pdf)
{
    int flush, err = Z_OK;
    uInt zip_len;
    z_stream *s = pdf->c_stream;
    boolean finish = pdf->zip_write_state == zip_finish;
    assert(pdf->compress_level > 0);
    /* This was just to suppress the filename report in |pdftex_fail|
       but zlib errors are rare enough (especially now that the
       compress level is fixed) that I don't care about the slightly
       ugly error message that could result.
     */
#if 0
    cur_file_name = NULL;
#endif
    if (pdf->stream_length == 0) {
        if (s == NULL) {
            s = pdf->c_stream = xtalloc(1, z_stream);
            s->zalloc = (alloc_func) 0;
            s->zfree = (free_func) 0;
            s->opaque = (voidpf) 0;
            check_err(deflateInit(s, pdf->compress_level), "deflateInit");
            assert(pdf->zipbuf == NULL);
            pdf->zipbuf = xtalloc(ZIP_BUF_SIZE, char);
        } else
            check_err(deflateReset(s), "deflateReset");
        s->next_out = (Bytef *) pdf->zipbuf;
        s->avail_out = ZIP_BUF_SIZE;
    }
    assert(s != NULL);
    assert(pdf->zipbuf != NULL);
    s->next_in = pdf->buf;
    s->avail_in = (uInt) pdf->ptr;
    while (true) {
        if (s->avail_out == 0 || (finish && s->avail_out < ZIP_BUF_SIZE)) {
            zip_len = ZIP_BUF_SIZE - s->avail_out;
            pdf->gone += (off_t) xfwrite(pdf->zipbuf, 1, zip_len, pdf->file);
            pdf->last_byte = pdf->zipbuf[zip_len - 1];
            s->next_out = (Bytef *) pdf->zipbuf;
            s->avail_out = ZIP_BUF_SIZE;
        }
        if (finish) {
            if (err == Z_STREAM_END) {
                assert(s->avail_in == 0);
                assert(s->avail_out == ZIP_BUF_SIZE);
                xfflush(pdf->file);
                break;
            }
            flush = Z_FINISH;
        } else {
            if (s->avail_in == 0)
                break;
            flush = Z_NO_FLUSH;
        }
        err = deflate(s, flush);
        if (err != Z_OK && err != Z_STREAM_END)
            pdftex_fail("zlib: deflate() failed (error code %d)", err);
    }
    pdf->stream_length = (off_t) s->total_out;
}

@ @c
void zip_free(PDF pdf)
{
    if (pdf->zipbuf != NULL) {
        check_err(deflateEnd(pdf->c_stream), "deflateEnd");
        xfree(pdf->zipbuf);
    }
    xfree(pdf->c_stream);
}

@ The PDF buffer is flushed by calling |pdf_flush|, which checks the
variable |zip_write_state| and will compress the buffer before flushing if
neccesary. We call |pdf_begin_stream| to begin a stream  and |pdf_end_stream|
to finish it. The stream contents will be compressed if compression is turn on.

@c
void pdf_flush(PDF pdf)
{                               /* flush out the |pdf_buf| */

    off_t saved_pdf_gone;
    if (!pdf->os_mode) {
        saved_pdf_gone = pdf->gone;
        switch (pdf->zip_write_state) {
        case no_zip:
            if (pdf->ptr > 0) {
                if (pdf->draftmode == 0)
                    (void) xfwrite((char *) pdf->buf, sizeof(char),
                                   (size_t) pdf->ptr, pdf->file);
                pdf->gone += pdf->ptr;
                pdf->last_byte = pdf->buf[pdf->ptr - 1];
            }
            break;
        case zip_writing:
            if (pdf->draftmode == 0)
                write_zip(pdf);
            break;
        case zip_finish:
            if (pdf->draftmode == 0)
                write_zip(pdf);
            pdf->zip_write_state = no_zip;
            break;
        }
        pdf->ptr = 0;
        if (saved_pdf_gone > pdf->gone)
            pdf_error("file size",
                      "File size exceeds architectural limits (pdf_gone wraps around)");
    }
}

@ switch between PDF stream and object stream mode 

@c
static void pdf_os_switch(PDF pdf, boolean pdf_os)
{
    if (pdf_os && pdf->os_enable) {
        if (!pdf->os_mode) {    /* back up PDF stream variables */
            pdf->op_ptr = pdf->ptr;
            pdf->ptr = pdf->os_ptr;
            pdf->buf = pdf->os_buf;
            pdf->buf_size = pdf->os_buf_size;
            pdf->os_mode = true;        /* switch to object stream */
        }
    } else {
        if (pdf->os_mode) {     /* back up object stream variables */
            pdf->os_ptr = pdf->ptr;
            pdf->ptr = pdf->op_ptr;
            pdf->buf = pdf->op_buf;
            pdf->buf_size = pdf->op_buf_size;
            pdf->os_mode = false;       /* switch to PDF stream */
        }
    }
}

@ create new \.{/ObjStm} object if required, and set up cross reference info 

@c
static void pdf_os_prepare_obj(PDF pdf, int i, int pdf_os_level)
{
    pdf_os_switch(pdf, ((pdf_os_level > 0)
                        && (pdf->objcompresslevel >= pdf_os_level)));
    if (pdf->os_mode) {
        if (pdf->os_cur_objnum == 0) {
            pdf->os_cur_objnum =
                pdf_create_obj(pdf, obj_type_objstm, pdf->obj_ptr + 1);
            pdf->os_cntr++;     /* only for statistics */
            pdf->os_idx = 0;
            pdf->ptr = 0;       /* start fresh object stream */
        } else {
            pdf->os_idx++;
        }
        obj_os_idx(pdf, i) = pdf->os_idx;
        obj_offset(pdf, i) = pdf->os_cur_objnum;
        pdf->os_obj[pdf->os_idx].num = i;
        pdf->os_obj[pdf->os_idx].off = pdf->ptr;
    } else {
        obj_offset(pdf, i) = pdf_offset(pdf);
        obj_os_idx(pdf, i) = -1;        /* mark it as not included in object stream */
    }
}

@* low-level buffer checkers.

@ check that |s| bytes more fit into |pdf_os_buf|; increase it if required 
@c
static void pdf_os_get_os_buf(PDF pdf, int s)
{
    int a;
    if (s > sup_pdf_os_buf_size - pdf->ptr)
        overflow("PDF object stream buffer", (unsigned) pdf->os_buf_size);
    if (pdf->ptr + s > pdf->os_buf_size) {
        a = pdf->os_buf_size / 5;
        if (pdf->ptr + s > pdf->os_buf_size + a)
            pdf->os_buf_size = pdf->ptr + s;
        else if (pdf->os_buf_size < sup_pdf_os_buf_size - a)
            pdf->os_buf_size = pdf->os_buf_size + a;
        else
            pdf->os_buf_size = sup_pdf_os_buf_size;
        pdf->os_buf =
            xreallocarray(pdf->os_buf, unsigned char,
                          (unsigned) pdf->os_buf_size);
        pdf->buf = pdf->os_buf;
        pdf->buf_size = pdf->os_buf_size;
    }
}

@ make sure that there are at least |n| bytes free in PDF buffer 
@c
void pdf_room(PDF pdf, int n)
{
    if (pdf->os_mode && (n + pdf->ptr > pdf->buf_size))
        pdf_os_get_os_buf(pdf, n);
    else if ((!pdf->os_mode) && (n > pdf->buf_size))
        overflow("PDF output buffer", (unsigned) pdf->op_buf_size);
    else if ((!pdf->os_mode) && (n + pdf->ptr > pdf->buf_size))
        pdf_flush(pdf);
}


@ print out a character to PDF buffer; the character will be printed in octal
 form in the following cases: chars <= 32, backslash (92), left parenthesis
 (40) and  right parenthesis (41)
 
@c
#define pdf_print_escaped(c)                                  \
  if ((c)<=32||(c)=='\\'||(c)=='('||(c)==')'||(c)>127) {      \
    pdf_room(pdf,4);                                          \
    pdf_quick_out(pdf,'\\');                                  \
    pdf_quick_out(pdf,(unsigned char)('0' + (((c)>>6) & 0x3)));         \
    pdf_quick_out(pdf,(unsigned char)('0' + (((c)>>3) & 0x7)));         \
    pdf_quick_out(pdf,(unsigned char)('0' + ( (c)     & 0x7)));         \
  } else {                                                    \
    pdf_out(pdf,(c));                                         \
  }

void pdf_print_char(PDF pdf, int c)
{
    if (c > 255)
        return;
    pdf_print_escaped(c);
}

@ @c
void pdf_out_block(PDF pdf, const char *s, size_t n)
{
    size_t l;
    do {
        l = n;
        if ((int) l > pdf->buf_size)
            l = (size_t) pdf->buf_size;
        pdf_room(pdf, (int) l);
        (void) memcpy(pdf->buf + pdf->ptr, s, l);
        pdf->ptr += (int) l;
        s += l;
        n -= l;
    } while (n > 0);
}

void pdf_print_wide_char(PDF pdf, int c)
{
    char hex[5];
    snprintf(hex, 5, "%04X", c);
    pdf_out_block(pdf, (const char *) hex, 4);
}

@ @c
__attribute__ ((format(printf, 2, 3)))
void pdf_printf(PDF pdf, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    if (pdf->printf_buf == NULL) {
        pdf->printf_buf = xtalloc(PRINTF_BUF_SIZE, char);
    }
    (void) vsnprintf(pdf->printf_buf, PRINTF_BUF_SIZE, fmt, args);
    pdf_puts(pdf, pdf->printf_buf);
    va_end(args);
}

@ print out a string to PDF buffer
@c
void pdf_print(PDF pdf, str_number s)
{
    const char *ss;
    size_t l;
    if (s >= STRING_OFFSET) {
        ss = (const char *) str_string(s);
        l = str_length(s);
        pdf_out_block(pdf, ss, l);
    } else {
        assert(s < 256);
        pdf_out(pdf, s);
    }
}

@ print out a integer to PDF buffer
@c
void pdf_print_int(PDF pdf, longinteger n)
{
    register int k = 0;         /*  current digit; we assume that $|n|<10^{23}$ */
    int dig[24];
    if (n < 0) {
        pdf_out(pdf, '-');
        if (n < -0x7FFFFFFF) {  /* need to negate |n| more carefully */
            register longinteger m;
            k++;
            m = -1 - n;
            n = m / 10;
            m = (m % 10) + 1;
            if (m < 10) {
                dig[0] = (int) m;
            } else {
                dig[0] = 0;
                n++;
            }
        } else {
            n = -n;
        }
    }
    do {
        dig[k++] = (int) (n % 10);
        n /= 10;
    } while (n != 0);
    pdf_room(pdf, k);
    while (k-- > 0) {
        pdf_quick_out(pdf, (unsigned char) ('0' + dig[k]));
    }
}

@ print $m/10^d$ as real
@c
void pdf_print_real(PDF pdf, int m, int d)
{
    if (m < 0) {
        pdf_out(pdf, '-');
        m = -m;
    };
    pdf_print_int(pdf, m / ten_pow[d]);
    m = m % ten_pow[d];
    if (m > 0) {
        pdf_out(pdf, '.');
        d--;
        while (m < ten_pow[d]) {
            pdf_out(pdf, '0');
            d--;
        }
        while (m % 10 == 0)
            m /= 10;
        pdf_print_int(pdf, m);
    }
}

@ print out |s| as string in PDF output 
@c
void pdf_print_str(PDF pdf, const char *s)
{
    const char *orig = s;
    int l = (int) strlen(s) - 1;        /* last string index */
    if (l < 0) {
        pdf_puts(pdf, "()");
        return;
    }
    /* the next is not really safe, the string could be "(a)xx(b)" */
    if ((s[0] == '(') && (s[l] == ')')) {
        pdf_puts(pdf, s);
        return;
    }
    if ((s[0] != '<') || (s[l] != '>') || odd((l + 1))) {
        pdf_out(pdf, '(');
        pdf_puts(pdf, s);
        pdf_out(pdf, ')');
        return;
    }
    s++;
    while (is_hex_char(*s))
        s++;
    if (s != orig + l) {
        pdf_out(pdf, '(');
        pdf_puts(pdf, orig);
        pdf_out(pdf, ')');
        return;
    }
    pdf_puts(pdf, orig);        /* it was a hex string after all  */
}

@ begin a stream 
@c
void pdf_begin_stream(PDF pdf)
{
    assert(pdf->os_mode == false);
    pdf_puts(pdf, "/Length           \n");
    pdf->seek_write_length = true;      /* fill in length at |pdf_end_stream| call */
    pdf->stream_length_offset = pdf_offset(pdf) - 11;
    pdf->stream_length = 0;
    pdf->last_byte = 0;
    if (pdf->compress_level > 0) {
        pdf_puts(pdf, "/Filter /FlateDecode\n");
        pdf_puts(pdf, ">>\n");
        pdf_puts(pdf, "stream\n");
        pdf_flush(pdf);
        pdf->zip_write_state = zip_writing;
    } else {
        pdf_puts(pdf, ">>\n");
        pdf_puts(pdf, "stream\n");
        pdf_save_offset(pdf);
    }
}

@ @c
static void write_stream_length(PDF pdf, int length, longinteger offset)
{
    if (jobname_cstr == NULL)
        jobname_cstr = makecstring(job_name);
    if (pdf->draftmode == 0) {
        xfseeko(pdf->file, (off_t) offset, SEEK_SET, jobname_cstr);
        fprintf(pdf->file, "%i", length);
        xfseeko(pdf->file, (off_t) pdf_offset(pdf), SEEK_SET, jobname_cstr);
    }
}

@ end a stream 
@c
void pdf_end_stream(PDF pdf)
{
    if (pdf->zip_write_state == zip_writing)
        pdf->zip_write_state = zip_finish;
    else
        pdf->stream_length = pdf_offset(pdf) - pdf_saved_offset(pdf);
    pdf_flush(pdf);
    if (pdf->seek_write_length)
        write_stream_length(pdf, (int) pdf->stream_length,
                            pdf->stream_length_offset);
    pdf->seek_write_length = false;
    if (pdf->last_byte != pdf_newline_char)
        pdf_out(pdf, pdf_newline_char);
    pdf_puts(pdf, "endstream\n");
    pdf_end_obj(pdf);
}

void pdf_remove_last_space(PDF pdf)
{
    if ((pdf->ptr > 0) && (pdf->buf[pdf->ptr - 1] == ' '))
        pdf->ptr--;
}

@ To print |scaled| value to PDF output we need some subroutines to ensure
accurary.

@c
#define max_integer 0x7FFFFFFF  /* $2^{31}-1$ */

/* scaled value corresponds to 100in, exact, 473628672 */
scaled one_hundred_inch = 7227 * 65536;

/* scaled value corresponds to 1in (rounded to 4736287) */
scaled one_inch = (7227 * 65536 + 50) / 100;

/* scaled value corresponds to 1truein (rounded!) */
scaled one_true_inch = (7227 * 65536 + 50) / 100;

/* scaled value corresponds to 100bp */
scaled one_hundred_bp = (7227 * 65536) / 72;

/* scaled value corresponds to 1bp (rounded to 65782) */
/* changed on 20110411 to be exactly 65781, as in tex itself,
  because this value is also used for \pdfpxdimen */
scaled one_bp = 65781;

/* $10^0..10^9$ */
int ten_pow[10] = {
    1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000
};


@ The function |divide_scaled| divides |s| by |m| using |dd| decimal
digits of precision. It is defined in C because it is a good candidate
for optimizations that are not possible in pascal.

@c
scaled round_xn_over_d(scaled x, int n, unsigned int d)
{
    boolean positive;           /* was |x>=0|? */
    unsigned t, u, v;           /* intermediate quantities */
    if (x >= 0) {
        positive = true;
    } else {
        x = -(x);
        positive = false;
    }
    t = (unsigned) ((x % 0100000) * n);
    u = (unsigned) (((unsigned) (x) / 0100000) * (unsigned) n + (t / 0100000));
    v = (u % d) * 0100000 + (t % 0100000);
    if (u / d >= 0100000)
        arith_error = true;
    else
        u = 0100000 * (u / d) + (v / d);
    v = v % d;
    if (2 * v >= d)
        u++;
    if (positive)
        return (scaled) u;
    else
        return (-(scaled) u);
}

@ @c
#define lround(a) (long) floor((a) + 0.5)

void pdf_print_bp(PDF pdf, scaled s)
{                               /* print scaled as |bp| */
    pdffloat a;
    pdfstructure *p = pdf->pstruct;
    assert(p != NULL);
    a.m = lround(s * p->k1);
    a.e = pdf->decimal_digits;
    print_pdffloat(pdf, a);
}

void pdf_print_mag_bp(PDF pdf, scaled s)
{                               /* take |mag| into account */
    pdffloat a;
    pdfstructure *p = pdf->pstruct;
    prepare_mag();
    if (int_par(mag_code) != 1000)
        a.m = lround(s * (double) int_par(mag_code) / 1000.0 * p->k1);
    else
        a.m = lround(s * p->k1);
    a.e = pdf->decimal_digits;
    print_pdffloat(pdf, a);
}

#define set_p_or_return(a) do {                 \
        p = a;                                  \
        if (p==NULL) {                          \
            a = item;                           \
            return;                             \
        }                                       \
    } while (0)


@* handling page resources.

@c
typedef struct {
    int obj_type;
    pdf_object_list *list;
} pr_entry;

@ @c
static int comp_page_resources(const void *pa, const void *pb, void *p)
{
    int a, b;
    (void) p;
    a = ((const pr_entry *) pa)->obj_type;
    b = ((const pr_entry *) pb)->obj_type;
    if (a > b)
        return 1;
    if (a < b)
        return -1;
    return 0;
}

@ @c
void addto_page_resources(PDF pdf, pdf_obj_type t, int k)
{
    pdf_resource_struct *re;
    pr_entry *pr, tmp;
    void **pp;
    pdf_object_list *p, *item = NULL;
    assert(pdf != NULL);
    re = pdf->page_resources;
    assert(re != NULL);
    assert(t <= PDF_OBJ_TYPE_MAX);
    if (re->resources_tree == NULL) {
        re->resources_tree =
            avl_create(comp_page_resources, NULL, &avl_xallocator);
        if (re->resources_tree == NULL)
            pdftex_fail
                ("addto_page_resources(): avl_create() page_resource_tree failed");
    }
    tmp.obj_type = t;
    pr = (pr_entry *) avl_find(re->resources_tree, &tmp);
    if (pr == NULL) {
        pr = xtalloc(1, pr_entry);
        pr->obj_type = t;
        pr->list = NULL;
        pp = avl_probe(re->resources_tree, pr);
        if (pp == NULL)
            pdftex_fail
                ("addto_page_resources(): avl_probe() out of memory in insertion");
    }
    if (pr->list == NULL) {
        item = xtalloc(1, pdf_object_list);
        item->link = NULL;
        item->info = k;
        pr->list = item;
        if (obj_type(pdf, k) == (int)t)
            set_obj_scheduled(pdf, k);  /* k is an object number */
    } else {
        for (p = pr->list; p->info != k && p->link != NULL; p = p->link);
        if (p->info != k) {
            item = xtalloc(1, pdf_object_list);
            item->link = NULL;
            item->info = k;
            p->link = item;
            if (obj_type(pdf, k) == (int)t)
                set_obj_scheduled(pdf, k);
        }
    }
}

@ @c
pdf_object_list *get_page_resources_list(PDF pdf, pdf_obj_type t)
{
    pdf_resource_struct *re = pdf->page_resources;
    pr_entry *pr, tmp;
    if (re == NULL || re->resources_tree == NULL)
        return NULL;
    tmp.obj_type = t;
    pr = (pr_entry *) avl_find(re->resources_tree, &tmp);
    if (pr == NULL)
        return NULL;
    return pr->list;
}

@ @c
static void reset_page_resources(PDF pdf)
{
    pdf_resource_struct *re = pdf->page_resources;
    pr_entry *p;
    struct avl_traverser t;
    pdf_object_list *l1, *l2;
    if (re == NULL || re->resources_tree == NULL)
        return;
    avl_t_init(&t, re->resources_tree);
    for (p = avl_t_first(&t, re->resources_tree); p != NULL; p = avl_t_next(&t)) {
        if (p->list != NULL) {
            for (l1 = p->list; l1 != NULL; l1 = l2) {
                l2 = l1->link;
                free(l1);
            }
            p->list = NULL;     /* but the AVL tree remains */
        }
    }
}

@ @c
static void destroy_pg_res_tree(void *pa, void *param)
{
    (void) param;
    xfree(pa);
}

@ @c
static void destroy_page_resources_tree(PDF pdf)
{
    pdf_resource_struct *re = pdf->page_resources;
    reset_page_resources(pdf);
    if (re->resources_tree != NULL)
        avl_destroy(re->resources_tree, destroy_pg_res_tree);
    re->resources_tree = NULL;
}

@* Subroutines to print out various PDF objects.

@ print out an integer |n| with fixed width |w|; used for outputting cross-reference table 
@c
static void pdf_print_fw_int(PDF pdf, longinteger n, size_t w)
{
    int k;                      /* $0\le k\le23$ */
    unsigned char digits[24];
    k = (int) w;
    do {
        k--;
        digits[k] = (unsigned char) ('0' + (n % 10));
        n /= 10;
    } while (k != 0);
    pdf_out_block(pdf, (const char *) digits, w);
}

@ print out an integer |n| as a fixed number |w| of bytes; used for outputting \.{/XRef} cross-reference stream 
@c
static void pdf_out_bytes(PDF pdf, longinteger n, size_t w)
{
    int k;
    unsigned char bytes[8];     /* digits in a number being output */
    k = (int) w;
    do {
        k--;
        bytes[k] = (unsigned char) (n % 256);
        n /= 256;
    } while (k != 0);
    pdf_out_block(pdf, (const char *) bytes, w);
}

@ print out an entry in dictionary with integer value to PDF buffer 
@c
void pdf_int_entry(PDF pdf, const char *s, int v)
{
    pdf_printf(pdf, "/%s ", s);
    pdf_print_int(pdf, v);
}

void pdf_int_entry_ln(PDF pdf, const char *s, int v)
{

    pdf_int_entry(pdf, s, v);
    pdf_print_nl(pdf);
}

@  print out an indirect entry in dictionary
@c
void pdf_indirect(PDF pdf, const char *s, int o)
{
    pdf_printf(pdf, "/%s %d 0 R", s, (int) o);
}

void pdf_indirect_ln(PDF pdf, const char *s, int o)
{

    pdf_indirect(pdf, s, o);
    pdf_print_nl(pdf);
}

@ print out |s| as string in PDF output 
@c
void pdf_print_str_ln(PDF pdf, const char *s)
{
    pdf_print_str(pdf, s);
    pdf_print_nl(pdf);
}

@ print out an entry in dictionary with string value to PDF buffer 
@c
void pdf_str_entry(PDF pdf, const char *s, const char *v)
{
    if (v == 0)
        return;
    pdf_printf(pdf, "/%s ", s);
    pdf_print_str(pdf, v);
}

void pdf_str_entry_ln(PDF pdf, const char *s, const char *v)
{
    if (v == 0)
        return;
    pdf_str_entry(pdf, s, v);
    pdf_print_nl(pdf);
}

@ @c
void pdf_print_toks(PDF pdf, halfword p)
{
    int len = 0;
    char *s = tokenlist_to_cstring(p, true, &len);
    if (len > 0)
        pdf_puts(pdf, s);
    xfree(s);
}

void pdf_print_toks_ln(PDF pdf, halfword p)
{
    int len = 0;
    char *s = tokenlist_to_cstring(p, true, &len);
    if (len > 0) {
        pdf_puts(pdf, s);
        pdf_print_nl(pdf);
    }
    xfree(s);
}

@ prints a rect spec 
@c
void pdf_print_rect_spec(PDF pdf, halfword r)
{
    pdf_print_mag_bp(pdf, pdf_ann_left(r));
    pdf_out(pdf, ' ');
    pdf_print_mag_bp(pdf, pdf_ann_bottom(r));
    pdf_out(pdf, ' ');
    pdf_print_mag_bp(pdf, pdf_ann_right(r));
    pdf_out(pdf, ' ');
    pdf_print_mag_bp(pdf, pdf_ann_top(r));
}

@ output a rectangle specification to PDF file 
@c
void pdf_rectangle(PDF pdf, halfword r)
{
    prepare_mag();
    pdf_puts(pdf, "/Rect [");
    pdf_print_rect_spec(pdf, r);
    pdf_puts(pdf, "]\n");
}

@ @c
static void init_pdf_outputparameters(PDF pdf)
{
    assert(pdf->o_mode == OMODE_PDF);
    pdf->draftmode = fix_int(pdf_draftmode, 0, 1);
    pdf->compress_level = fix_int(pdf_compress_level, 0, 9);
    pdf->decimal_digits = fix_int(pdf_decimal_digits, 0, 4);
    pdf->gamma = fix_int(pdf_gamma, 0, 1000000);
    pdf->image_gamma = fix_int(pdf_image_gamma, 0, 1000000);
    pdf->image_hicolor = fix_int(pdf_image_hicolor, 0, 1);
    pdf->image_apply_gamma = fix_int(pdf_image_apply_gamma, 0, 1);
    pdf->objcompresslevel = fix_int(pdf_objcompresslevel, 0, 3);
    pdf->inclusion_copy_font = fix_int(pdf_inclusion_copy_font, 0, 1);
    pdf->replace_font = fix_int(pdf_replace_font, 0, 1);
    pdf->pk_resolution = fix_int(pdf_pk_resolution, 72, 8000);
    if ((pdf->minor_version >= 5) && (pdf->objcompresslevel > 0)) {
        pdf->os_enable = true;
    } else {
        if (pdf->objcompresslevel > 0) {
            pdf_warning("Object streams",
                        "\\pdfobjcompresslevel > 0 requires \\pdfminorversion > 4. Object streams disabled now.",
                        true, true);
            pdf->objcompresslevel = 0;
        }
        pdf->os_enable = false;
    }
    if (pdf->pk_resolution == 0)        /* if not set from format file or by user */
        pdf->pk_resolution = pk_dpi;    /* take it from \.{texmf.cnf} */
    pdf->pk_scale_factor =
        divide_scaled(72, pdf->pk_resolution, 5 + pdf->decimal_digits);
    if (!callback_defined(read_pk_file_callback)) {
        if (pdf_pk_mode != null) {
            char *s = tokenlist_to_cstring(pdf_pk_mode, true, NULL);
            kpseinitprog("PDFTEX", (unsigned) pdf->pk_resolution, s, nil);
            xfree(s);
        } else {
            kpseinitprog("PDFTEX", (unsigned) pdf->pk_resolution, nil, nil);
        }
        if (!kpsevarvalue("MKTEXPK"))
            kpsesetprogramenabled(kpsepkformat, 1, kpsesrccmdline);
    }
    set_job_id(pdf, int_par(year_code),
               int_par(month_code), int_par(day_code), int_par(time_code));
    if ((pdf_unique_resname > 0) && (pdf->resname_prefix == NULL))
        pdf->resname_prefix = get_resname_prefix(pdf);
}

@ Checks that we have a name for the generated PDF file and that it's open. 

@c
static void ensure_output_file_open(PDF pdf, const char *ext)
{
    char *fn;
    if (pdf->file_name != NULL)
        return;
    if (job_name == 0)
        open_log_file();
    fn = pack_job_name(ext);
    if (pdf->draftmode == 0 || pdf->o_mode == OMODE_DVI) {
        while (!lua_b_open_out(&pdf->file, fn))
            fn = prompt_file_name("file name for output", ext);
    }
    pdf->file_name = fn;
}

@ @c
static void ensure_pdf_header_written(PDF pdf)
{
    assert(pdf->o_state == ST_FILE_OPEN);
    assert(pdf->o_mode == OMODE_PDF);
    /* Initialize variables for \.{PDF} output */
    fix_pdf_minorversion(pdf);
    init_pdf_outputparameters(pdf);
    /* Write \.{PDF} header */
    pdf_printf(pdf, "%%PDF-1.%d\n", pdf->minor_version);
    pdf_out(pdf, '%');
    pdf_out(pdf, 'P' + 128);
    pdf_out(pdf, 'T' + 128);
    pdf_out(pdf, 'E' + 128);
    pdf_out(pdf, 'X' + 128);
    pdf_print_nl(pdf);
}

@ @c
void ensure_output_state(PDF pdf, output_state s)
{
    if (pdf->o_state < s) {
        if (s > ST_INITIAL)
            ensure_output_state(pdf, s - 1);
        switch (s - 1) {
        case ST_INITIAL:
            fix_o_mode(pdf);
            break;
        case ST_OMODE_FIX:
            switch (pdf->o_mode) {
            case OMODE_DVI:
                ensure_output_file_open(pdf, ".dvi");
                break;
            case OMODE_PDF:
                ensure_output_file_open(pdf, ".pdf");
                break;
            case OMODE_LUA:
                break;
            default:
                assert(0);
            }
            break;
        case ST_FILE_OPEN:
            switch (pdf->o_mode) {
            case OMODE_DVI:
                ensure_dvi_header_written(pdf);
                break;
            case OMODE_PDF:
                ensure_pdf_header_written(pdf);
                break;
            case OMODE_LUA:
                break;
            default:
                assert(0);
            }
            break;
        case ST_HEADER_WRITTEN:
            break;
        case ST_FILE_CLOSED:
            break;
        default:
            assert(0);
        }
        pdf->o_state++;
    }
}

@ Write out an accumulated object stream.

First the object number and byte offset pairs are generated
and appended to the ready buffered object stream.
By this the value of \.{/First} can be calculated.
Then a new \.{/ObjStm} object is generated, and everything is
copied to the PDF output buffer, where also compression is done.
When calling this procedure, |pdf_os_mode| must be |true|.

@c
static void pdf_os_write_objstream(PDF pdf)
{
    halfword i, j, p, q;
    if (pdf->os_cur_objnum == 0)        /* no object stream started */
        return;
    p = pdf->ptr;
    i = 0;
    j = 0;
    while (i <= pdf->os_idx) {  /* assemble object number and byte offset pairs */
        pdf_printf(pdf, "%d %d", (int) pdf->os_obj[i].num,
                   (int) pdf->os_obj[i].off);
        if (j == 9) {           /* print out in groups of ten for better readability */
            pdf_out(pdf, pdf_newline_char);
            j = 0;
        } else {
            pdf_out(pdf, ' ');
            j++;
        }
        i++;
    }
    pdf->buf[pdf->ptr - 1] = pdf_newline_char;  /* no risk of flush, as we are in |pdf_os_mode| */
    q = pdf->ptr;
    pdf_begin_dict(pdf, pdf->os_cur_objnum, 0); /* switch to PDF stream writing */
    pdf_puts(pdf, "/Type /ObjStm\n");
    pdf_printf(pdf, "/N %d\n", (int) (pdf->os_idx + 1));
    pdf_printf(pdf, "/First %d\n", (int) (q - p));
    pdf_begin_stream(pdf);
    /* write object number and byte offset pairs;
       |q - p| should always fit into the PDF output buffer */
    pdf_out_block(pdf, (const char *) (pdf->os_buf + p), (size_t) (q - p));
    i = 0;
    while (i < p) {
        q = i + pdf->buf_size;
        if (q > p)
            q = p;
        pdf_room(pdf, q - i);
        while (i < q) {         /* write the buffered objects */
            pdf_quick_out(pdf, pdf->os_buf[i]);
            i++;
        }
    }
    pdf_end_stream(pdf);
    pdf->os_cur_objnum = 0;     /* to force object stream generation next time */
}

@ begin a PDF dictionary object
@c
void pdf_begin_dict(PDF pdf, int i, int pdf_os_level)
{
    ensure_output_state(pdf, ST_HEADER_WRITTEN);
    pdf_os_prepare_obj(pdf, i, pdf_os_level);
    if (!pdf->os_mode) {
        pdf_printf(pdf, "%d 0 obj\n<<\n", (int) i);
    } else {
        if (pdf->compress_level == 0)
            pdf_printf(pdf, "%% %d 0 obj\n", (int) i);  /* debugging help */
        pdf_puts(pdf, "<<\n");
    }
}

@ begin a new PDF dictionary object 
@c
int pdf_new_dict(PDF pdf, int t, int i, int pdf_os_level)
{
    int k = pdf_create_obj(pdf, t, i);
    pdf_begin_dict(pdf, k, pdf_os_level);
    return k;
}

@ end a PDF dictionary object
@c
void pdf_end_dict(PDF pdf)
{
    if (pdf->os_mode) {
        pdf_puts(pdf, ">>\n");
        if (pdf->os_idx == pdf_os_max_objs - 1)
            pdf_os_write_objstream(pdf);
    } else {
        pdf_puts(pdf, ">>\nendobj\n");
    }
}

@ begin a PDF object 
@c
void pdf_begin_obj(PDF pdf, int i, int pdf_os_level)
{
    ensure_output_state(pdf, ST_HEADER_WRITTEN);
    pdf_os_prepare_obj(pdf, i, pdf_os_level);
    if (!pdf->os_mode) {
        pdf_printf(pdf, "%d 0 obj\n", (int) i);
    } else if (pdf->compress_level == 0) {
        pdf_printf(pdf, "%% %d 0 obj\n", (int) i);      /* debugging help */
    }
}

@ begin a new PDF object
@c
int pdf_new_obj(PDF pdf, int t, int i, int pdf_os_level)
{
    int k = pdf_create_obj(pdf, t, i);
    pdf_begin_obj(pdf, k, pdf_os_level);
    return k;
}

@ end a PDF object 
@c
void pdf_end_obj(PDF pdf)
{
    if (pdf->os_mode) {
        if (pdf->os_idx == pdf_os_max_objs - 1)
            pdf_os_write_objstream(pdf);
    } else {
        pdf_puts(pdf, "endobj\n");      /* end a PDF object */
    }
}

@ Converts any string given in in in an allowed PDF string which can be
 handled by printf et.al.: \.{\\} is escaped to \.{\\\\}, parenthesis are escaped and
 control characters are octal encoded.
 This assumes that the string does not contain any already escaped
 characters!
 
@c
char *convertStringToPDFString(const char *in, int len)
{
    static char pstrbuf[MAX_PSTRING_LEN];
    char *out = pstrbuf;
    int i, j, k;
    char buf[5];
    j = 0;
    for (i = 0; i < len; i++) {
        check_buf((unsigned) j + sizeof(buf), MAX_PSTRING_LEN);
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

@ Converts any string given in in in an allowed PDF string which is
 hexadecimal encoded;
 |sizeof(out)| should be at least $|lin|*2+1$.
 
@c
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

@ Compute the ID string as per PDF1.4 9.3:
\medskip
{\obeylines\obeyspaces
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
}
\medskip
  This stipulates only that the two IDs must be identical when the file is
  created and that they should be reasonably unique. Since it's difficult
  to get the file size at this point in the execution of pdfTeX and
  scanning the info dict is also difficult, we start with a simpler
  implementation using just the first two items.

@c
static void print_ID(PDF pdf, const char *file_name)
{
    time_t t;
    size_t size;
    char time_str[32];
    md5_state_t state;
    md5_byte_t digest[16];
    char id[64];
    char pwd[4096];
    /* start md5 */
    md5_init(&state);
    /* get the time */
    t = time(NULL);
    size = strftime(time_str, sizeof(time_str), "%Y%m%dT%H%M%SZ", gmtime(&t));
    md5_append(&state, (const md5_byte_t *) time_str, (int) size);
    /* get the file name */
    if (getcwd(pwd, sizeof(pwd)) == NULL)
        pdftex_fail("getcwd() failed (%s), (path too long?)", strerror(errno));
    md5_append(&state, (const md5_byte_t *) pwd, (int) strlen(pwd));
    md5_append(&state, (const md5_byte_t *) "/", 1);
    md5_append(&state, (const md5_byte_t *) file_name, (int) strlen(file_name));
    /* finish md5 */
    md5_finish(&state, digest);
    /* write the IDs */
    convertStringToHexString((char *) digest, id, 16);
    pdf_printf(pdf, "/ID [<%s> <%s>]", id, id);
}

@ Print the /CreationDate entry.

  PDF Reference, third edition says about the expected date format:
\medskip
{\obeylines\obeyspaces
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
}

  The main difficulty is get the time zone offset. |strftime()| does this in ISO
  C99 (e.g. newer glibc) with \%z, but we have to work with other systems (e.g.
  Solaris 2.5).

@c
#define TIME_STR_SIZE 30        /* minimum size for |time_str| is 24: |"D:YYYYmmddHHMMSS+HH'MM'"| */

static void makepdftime(PDF pdf)
{
    struct tm lt, gmt;
    size_t size;
    int i, off, off_hours, off_mins;
    time_t t = pdf->start_time;
    char *time_str = pdf->start_time_str;

    /* get the time */
    lt = *localtime(&t);
    size = strftime(time_str, TIME_STR_SIZE, "D:%Y%m%d%H%M%S", &lt);
    /* expected format: "YYYYmmddHHMMSS" */
    if (size == 0) {
        /* unexpected, contents of |time_str| is undefined */
        time_str[0] = '\0';
        return;
    }

    /* correction for seconds: \%S can be in range 00..61,
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
    pdf->start_time = t;
}

@ @c
void init_start_time(PDF pdf)
{
    assert(pdf);
    if (pdf->start_time == 0) {
        pdf->start_time = time((time_t *) NULL);
        pdf->start_time_str = xtalloc(TIME_STR_SIZE, char);
        makepdftime(pdf);
    }
}

@ @c
static void print_creation_date(PDF pdf)
{
    init_start_time(pdf);
    pdf_printf(pdf, "/CreationDate (%s)\n", pdf->start_time_str);
}

@ @c
static void print_mod_date(PDF pdf)
{
    init_start_time(pdf);
    pdf_printf(pdf, "/ModDate (%s)\n", pdf->start_time_str);
}

@ @c
char *getcreationdate(PDF pdf)
{
    assert(pdf);
    init_start_time(pdf);
    return pdf->start_time_str;
}

@ @c
void remove_pdffile(PDF pdf)
{
    if (pdf != NULL) {
        if (!kpathsea_debug && pdf->file_name && (pdf->draftmode != 0)) {
            xfclose(pdf->file, pdf->file_name);
            remove(pdf->file_name);
        }
    }
}

@ @c
static void realloc_fb(PDF pdf)
{
    if (pdf->fb_array == NULL) {
        pdf->fb_limit = SMALL_ARRAY_SIZE;
        pdf->fb_array = xtalloc(pdf->fb_limit, char);
        pdf->fb_ptr = pdf->fb_array;
    } else if ((size_t) (pdf->fb_ptr - pdf->fb_array + 1) > pdf->fb_limit) {
        size_t last_ptr_index = (size_t) (pdf->fb_ptr - pdf->fb_array);
        pdf->fb_limit *= 2;
        if ((size_t) (pdf->fb_ptr - pdf->fb_array + 1) > pdf->fb_limit)
            pdf->fb_limit = (size_t) (pdf->fb_ptr - pdf->fb_array + 1);
        xretalloc(pdf->fb_array, pdf->fb_limit, char);
        pdf->fb_ptr = pdf->fb_array + last_ptr_index;
    }
}

@ @c
int fb_offset(PDF pdf)
{
    return (int) (pdf->fb_ptr - pdf->fb_array);
}

@ @c
void fb_seek(PDF pdf, int offset)
{
    pdf->fb_ptr = pdf->fb_array + offset;
}

@ @c
void fb_putchar(PDF pdf, eight_bits b)
{
    if ((size_t) (pdf->fb_ptr - pdf->fb_array + 1) > pdf->fb_limit)
        realloc_fb(pdf);
    *(pdf->fb_ptr)++ = (char) b;
}

@ @c
void fb_flush(PDF pdf)
{
    char *p;
    int n;
    for (p = pdf->fb_array; p < pdf->fb_ptr;) {
        n = pdf->buf_size - pdf->ptr;
        if (pdf->fb_ptr - p < n)
            n = (int) (pdf->fb_ptr - p);
        memcpy(pdf->buf + pdf->ptr, p, (unsigned) n);
        pdf->ptr += n;
        if (pdf->ptr == pdf->buf_size)
            pdf_flush(pdf);
        p += n;
    }
    pdf->fb_ptr = pdf->fb_array;
}

@ @c
void fb_free(PDF pdf)
{
    xfree(pdf->fb_array);
}

@ @c
void pdf_error(const char *t, const char *p)
{
    normalize_selector();
    print_err("LuaTeX error");
    if (t != NULL) {
        tprint(" (");
        tprint(t);
        tprint(")");
    }
    tprint(": ");
    if (p != NULL)
        tprint(p);
    succumb();
}

@ @c
void pdf_warning(const char *t, const char *p, boolean prepend_nl,
                 boolean append_nl)
{
    if (prepend_nl)
        print_ln();
    tprint("LuaTeX warning");
    if (t != NULL) {
        tprint(" (");
        tprint(t);
        tprint(")");
    }
    tprint(": ");
    if (p != NULL)
        tprint(p);
    if (append_nl)
        print_ln();
    if (history == spotless)
        history = warning_issued;
}

@ Use |check_o_mode()| in the backend-specific "Implement..." chunks 

@c
void check_o_mode(PDF pdf, const char *s, int o_mode_bitpattern, boolean strict)
{

    char warn_string[100];
    output_mode o_mode;
    const char *m = NULL;

    /* in warn mode (strict == false):
       only check, don't do |fix_o_mode()| here! |pdf->o_mode| is left
       in possibly wrong state until real output, ok.
     */

    if (pdf->o_mode == OMODE_NONE)
        o_mode = get_o_mode();
    else
        o_mode = pdf->o_mode;
    if (!((1 << o_mode) & o_mode_bitpattern)) { /* warning or error */
        switch (o_mode) {
        case OMODE_DVI:
            m = "DVI";
            break;
        case OMODE_PDF:
            m = "PDF";
            break;
        case OMODE_LUA:
            m = "Lua";
            break;
        default:
            assert(0);
        }
        snprintf(warn_string, 99, "not allowed in %s mode (\\pdfpoutput = %d)",
                 m, (int) pdf_output);
        if (strict)
            pdf_error(s, warn_string);
        else
            pdf_warning(s, warn_string, true, true);
    } else if (strict)
        ensure_output_state(pdf, ST_HEADER_WRITTEN);
}

@ @c
void set_job_id(PDF pdf, int year, int month, int day, int time)
{
    char *name_string, *format_string, *s;
    size_t slen;
    int i;

    if (pdf->job_id_string != NULL)
        return;

    name_string = makecstring(job_name);
    format_string = makecstring(format_ident);
    make_pdftex_banner();
    slen = SMALL_BUF_SIZE +
        strlen(name_string) + strlen(format_string) + strlen(pdftex_banner);
    s = xtalloc(slen, char);
    /* The Web2c version string starts with a space.  */
    i = snprintf(s, slen,
                 "%.4d/%.2d/%.2d %.2d:%.2d %s %s %s",
                 year, month, day, time / 60, time % 60,
                 name_string, format_string, pdftex_banner);
    check_nprintf(i, slen);
    pdf->job_id_string = xstrdup(s);
    xfree(s);
    xfree(name_string);
    xfree(format_string);
}

@ @c
char *get_resname_prefix(PDF pdf)
{
    static char name_str[] =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static char prefix[7];      /* make a tag of 6 chars long */
    unsigned long crc;
    short i;
    size_t base = strlen(name_str);
    crc = crc32(0L, Z_NULL, 0);
    crc =
        crc32(crc, (Bytef *) pdf->job_id_string,
              (uInt) strlen(pdf->job_id_string));
    for (i = 0; i < 6; i++) {
        prefix[i] = name_str[crc % base];
        crc /= base;
    }
    prefix[6] = '\0';
    return prefix;
}

@ @c
#define mag int_par(mag_code)

#define pdf_xform_attr equiv(pdf_xform_attr_loc)
#define pdf_xform_resources equiv(pdf_xform_resources_loc)

void pdf_begin_page(PDF pdf)
{
    scaled form_margin = 0;     /* was one_bp until SVN4066 */
    ensure_output_state(pdf, ST_HEADER_WRITTEN);
    init_pdf_pagecalculations(pdf);

    if (pdf->page_resources == NULL) {
        pdf->page_resources = xtalloc(1, pdf_resource_struct);
        pdf->page_resources->resources_tree = NULL;
    }
    pdf->page_resources->last_resources = pdf_new_objnum(pdf);
    reset_page_resources(pdf);

    if (global_shipping_mode == SHIPPING_PAGE) {
        pdf->last_page = get_obj(pdf, obj_type_page, total_pages + 1, 0);
        set_obj_aux(pdf, pdf->last_page, 1);    /* mark that this page has been created */
        pdf->last_stream = pdf_new_dict(pdf, obj_type_pagestream, 0, 0);
        pdf->last_thread = null;
        pdflua_begin_page(pdf);
    } else {
        assert(global_shipping_mode == SHIPPING_FORM);
        pdf_begin_dict(pdf, pdf_cur_form, 0);
        pdf->last_stream = pdf_cur_form;

        /* Write out Form stream header */
        pdf_puts(pdf, "/Type /XObject\n");
        pdf_puts(pdf, "/Subtype /Form\n");
        if (pdf_xform_attr != null)
            pdf_print_toks_ln(pdf, pdf_xform_attr);
        if (obj_xform_attr(pdf, pdf_cur_form) != null) {
            pdf_print_toks_ln(pdf, obj_xform_attr(pdf, pdf_cur_form));
            delete_token_ref(obj_xform_attr(pdf, pdf_cur_form));
            set_obj_xform_attr(pdf, pdf_cur_form, null);
        }
        pdf_puts(pdf, "/BBox [");
        pdf_print_bp(pdf, -form_margin);
        pdf_out(pdf, ' ');
        pdf_print_bp(pdf, -form_margin);
        pdf_out(pdf, ' ');
        pdf_print_bp(pdf, cur_page_size.h + form_margin);
        pdf_out(pdf, ' ');
        pdf_print_bp(pdf, cur_page_size.v + form_margin);
        pdf_puts(pdf, "]\n");
        pdf_puts(pdf, "/FormType 1\n");
        pdf_puts(pdf, "/Matrix [1 0 0 1 0 0]\n");
        pdf_indirect_ln(pdf, "Resources", pdf->page_resources->last_resources);
    }
    /* Start stream of page/form contents */
    pdf_begin_stream(pdf);
    if (global_shipping_mode == SHIPPING_PAGE) {
        /* Adjust transformation matrix for the magnification ratio */
        if (mag != 1000) {
            pdf_print_real(pdf, mag, 3);
            pdf_puts(pdf, " 0 0 ");
            pdf_print_real(pdf, mag, 3);
            pdf_puts(pdf, " 0 0 cm\n");
        }
    }
    pdf_shipout_begin();

    if (global_shipping_mode == SHIPPING_PAGE)
        pdf_out_colorstack_startpage(pdf);
}

@ @c
void print_pdf_table_string(PDF pdf, const char *s)
{
    size_t len;
    const char *ls;
    lua_getglobal(Luas, "pdf");     /* t ... */
    lua_pushstring(Luas, s);    /* s t ... */
    lua_gettable(Luas, -2);     /* s? t ... */
    if (lua_isstring(Luas, -1)) {       /* s t ... */
        ls = lua_tolstring(Luas, -1, &len);
        pdf_out_block(pdf, ls, len);
        pdf_out(pdf, '\n');
    }
    lua_pop(Luas, 2);           /* ... */
}

@ @c
#define pdf_page_attr equiv(pdf_page_attr_loc)
#define pdf_page_resources equiv(pdf_page_resources_loc)

void pdf_end_page(PDF pdf)
{
    int j, annots = 0, beads = 0;
    pdf_resource_struct *res_p = pdf->page_resources;
    pdf_resource_struct local_page_resources;
    pdf_object_list *annot_list, *bead_list, *link_list, *ol, *ol1;
    scaledpos save_cur_page_size;       /* to save |cur_page_size| during flushing pending forms */
    shipping_mode_e save_shipping_mode;
    int procset = PROCSET_PDF;

    /* Finish stream of page/form contents */
    pdf_goto_pagemode(pdf);
    pdf_shipout_end();
    pdf_end_stream(pdf);

    if (global_shipping_mode == SHIPPING_PAGE) {
        pdf->last_pages = pdf_do_page_divert(pdf, pdf->last_page, 0);

        /* Write out /Page object */
        pdf_begin_dict(pdf, pdf->last_page, 1);
        pdf_puts(pdf, "/Type /Page\n");
        pdf_indirect_ln(pdf, "Contents", pdf->last_stream);
        pdf_indirect_ln(pdf, "Resources", res_p->last_resources);
        pdf_puts(pdf, "/MediaBox [0 0 ");
        pdf_print_mag_bp(pdf, cur_page_size.h);
        pdf_out(pdf, ' ');
        pdf_print_mag_bp(pdf, cur_page_size.v);
        pdf_puts(pdf, "]\n");
        if (pdf_page_attr != null)
            pdf_print_toks_ln(pdf, pdf_page_attr);
        print_pdf_table_string(pdf, "pageattributes");
        pdf_indirect_ln(pdf, "Parent", pdf->last_pages);
        if (pdf->img_page_group_val != 0) {
            assert(pdf->img_page_group_val > 0);
            pdf_printf(pdf, "/Group %d 0 R\n", pdf->img_page_group_val);
        }
        annot_list = get_page_resources_list(pdf, obj_type_annot);
        link_list = get_page_resources_list(pdf, obj_type_link);
        if (annot_list != NULL || link_list != NULL) {
            annots = pdf_create_obj(pdf, obj_type_annots, 0);
            pdf_indirect_ln(pdf, "Annots", annots);
        }
        bead_list = get_page_resources_list(pdf, obj_type_bead);
        if (bead_list != NULL) {
            beads = pdf_create_obj(pdf, obj_type_beads, 0);
            pdf_indirect_ln(pdf, "B", beads);
        }
        pdf_end_dict(pdf);
        pdflua_end_page(pdf, annots, beads);

        pdf->img_page_group_val = 0;

        /* Generate array of annotations or beads in page */
        if (annot_list != NULL || link_list != NULL) {
            pdf_begin_obj(pdf, annots, 1);
            pdf_puts(pdf, "[");
            while (annot_list != NULL) {
                assert(annot_list->info > 0);
                pdf_print_int(pdf, annot_list->info);
                pdf_puts(pdf, " 0 R ");
                annot_list = annot_list->link;
            }
            while (link_list != NULL) {
                pdf_print_int(pdf, link_list->info);
                pdf_puts(pdf, " 0 R ");
                link_list = link_list->link;
            }
            pdf_puts(pdf, "]\n");
            pdf_end_obj(pdf);
        }
        if (bead_list != NULL) {
            pdf_begin_dict(pdf, beads, 1);
            pdf_puts(pdf, "[");
            while (bead_list != NULL) {
                pdf_print_int(pdf, bead_list->info);
                pdf_printf(pdf, " 0 R ");
                bead_list = bead_list->link;
            }
            pdf_printf(pdf, "]\n");
            pdf_end_dict(pdf);
        }
    }

    /* Write out resource lists */
    /* Write out pending raw objects */
    ol = get_page_resources_list(pdf, obj_type_obj);
    while (ol != NULL) {
        if (!is_obj_written(pdf, ol->info))
            pdf_write_obj(pdf, ol->info);
        ol = ol->link;
    }

    /* Write out pending forms */
    /* When flushing pending forms we need to save and restore resource lists
       which are also used by page shipping.
       Saving and restoring |cur_page_size| is needed for proper
       writing out pending PDF marks. */
    ol = get_page_resources_list(pdf, obj_type_xform);
    while (ol != NULL) {
        if (!is_obj_written(pdf, ol->info)) {
            pdf_cur_form = ol->info;
            save_cur_page_size = cur_page_size;
            save_shipping_mode = global_shipping_mode;
            pdf->page_resources = &local_page_resources;
            local_page_resources.resources_tree = NULL;
            ship_out(pdf, obj_xform_box(pdf, pdf_cur_form), SHIPPING_FORM);
            /* Restore page size and page resources */
            cur_page_size = save_cur_page_size;
            global_shipping_mode = save_shipping_mode;
            destroy_page_resources_tree(pdf);
            pdf->page_resources = res_p;
        }
        ol = ol->link;
    }

    /* Write out pending images */
    ol = get_page_resources_list(pdf, obj_type_ximage);
    while (ol != NULL) {
        if (!is_obj_written(pdf, ol->info))
            pdf_write_image(pdf, ol->info);
        ol = ol->link;
    }

    if (global_shipping_mode == SHIPPING_PAGE) {
        /* Write out pending PDF marks */
        /* Write out PDF annotations */
        ol = get_page_resources_list(pdf, obj_type_annot);
        while (ol != NULL) {
            if (ol->info > 0 && obj_type(pdf, ol->info) == obj_type_annot) {
                j = obj_annot_ptr(pdf, ol->info);       /* |j| points to |pdf_annot_node| */
                pdf_begin_dict(pdf, ol->info, 1);
                pdf_puts(pdf, "/Type /Annot\n");
                pdf_print_toks_ln(pdf, pdf_annot_data(j));
                pdf_rectangle(pdf, j);
                pdf_end_dict(pdf);
            }
            ol = ol->link;
        }

        /* Write out PDF link annotations */
        if ((ol = get_page_resources_list(pdf, obj_type_link)) != NULL) {
            while (ol != NULL) {
                j = obj_annot_ptr(pdf, ol->info);
                pdf_begin_dict(pdf, ol->info, 1);
                pdf_puts(pdf, "/Type /Annot\n");
                if (pdf_action_type(pdf_link_action(j)) != pdf_action_user)
                    pdf_puts(pdf, "/Subtype /Link\n");
                if (pdf_link_attr(j) != null)
                    pdf_print_toks_ln(pdf, pdf_link_attr(j));
                pdf_rectangle(pdf, j);
                if (pdf_action_type(pdf_link_action(j)) != pdf_action_user)
                    pdf_puts(pdf, "/A ");
                write_action(pdf, pdf_link_action(j));
                pdf_end_dict(pdf);
                ol = ol->link;
            }
            /* Flush |pdf_start_link_node|'s created by |append_link| */
            ol = get_page_resources_list(pdf, obj_type_link);
            while (ol != NULL) {
                j = obj_annot_ptr(pdf, ol->info);
                /* nodes with |subtype = pdf_link_data_node| were created by |append_link| and
                   must be flushed here, as they are not linked in any list */
                if (subtype(j) == pdf_link_data_node)
                    flush_node(j);
                ol = ol->link;
            }
        }

        /* Write out PDF mark destinations */
        write_out_pdf_mark_destinations(pdf);
        /* Write out PDF bead rectangle specifications */
        print_bead_rectangles(pdf);

    }
    /* Write out resources dictionary */
    pdf_begin_dict(pdf, res_p->last_resources, 1);
    /* Print additional resources */
    if (global_shipping_mode == SHIPPING_PAGE) {
        if (pdf_page_resources != null)
            pdf_print_toks_ln(pdf, pdf_page_resources);
        print_pdf_table_string(pdf, "pageresources");
    } else {
        if (pdf_xform_resources != null)
            pdf_print_toks_ln(pdf, pdf_xform_resources);
        if (obj_xform_resources(pdf, pdf_cur_form) != null) {
            pdf_print_toks_ln(pdf, obj_xform_resources(pdf, pdf_cur_form));
            delete_token_ref(obj_xform_resources(pdf, pdf_cur_form));
            set_obj_xform_resources(pdf, pdf_cur_form, null);
        }
    }

    /* Generate font resources */
    if ((ol = get_page_resources_list(pdf, obj_type_font)) != NULL) {
        pdf_puts(pdf, "/Font << ");
        while (ol != NULL) {
            assert(ol->info > 0);       /* always base font: an object number */
            pdf_puts(pdf, "/F");
            pdf_print_int(pdf, obj_info(pdf, ol->info));
            pdf_print_resname_prefix(pdf);
            pdf_out(pdf, ' ');
            pdf_print_int(pdf, ol->info);
            pdf_puts(pdf, " 0 R ");
            ol = ol->link;
        }
        pdf_puts(pdf, ">>\n");
        procset |= PROCSET_TEXT;
    }

    /* Generate XObject resources */
    ol = get_page_resources_list(pdf, obj_type_xform);
    ol1 = get_page_resources_list(pdf, obj_type_ximage);
    if (ol != NULL || ol1 != NULL) {
        pdf_puts(pdf, "/XObject << ");
        while (ol != NULL) {
            pdf_printf(pdf, "/Fm");
            pdf_print_int(pdf, obj_info(pdf, ol->info));
            pdf_print_resname_prefix(pdf);
            pdf_out(pdf, ' ');
            pdf_print_int(pdf, ol->info);
            pdf_puts(pdf, " 0 R ");
            ol = ol->link;
        }
        while (ol1 != null) {
            pdf_puts(pdf, "/Im");
            pdf_print_int(pdf, obj_data_ptr(pdf, ol1->info));
            pdf_print_resname_prefix(pdf);
            pdf_out(pdf, ' ');
            pdf_print_int(pdf, ol1->info);
            pdf_puts(pdf, " 0 R ");
            procset |= img_procset(idict_array[obj_data_ptr(pdf, ol1->info)]);
            ol1 = ol1->link;
        }
        pdf_puts(pdf, ">>\n");
    }

    /* Generate ProcSet */
    pdf_puts(pdf, "/ProcSet [");
    if ((procset & PROCSET_PDF) != 0)
        pdf_puts(pdf, " /PDF");
    if ((procset & PROCSET_TEXT) != 0)
        pdf_puts(pdf, " /Text");
    if ((procset & PROCSET_IMAGE_B) != 0)
        pdf_puts(pdf, " /ImageB");
    if ((procset & PROCSET_IMAGE_C) != 0)
        pdf_puts(pdf, " /ImageC");
    if ((procset & PROCSET_IMAGE_I) != 0)
        pdf_puts(pdf, " /ImageI");
    pdf_puts(pdf, " ]\n");

    pdf_end_dict(pdf);
}

@* Finishing the PDF output file. 

@ Destinations that have been referenced but don't exists have
|obj_dest_ptr=null|. Leaving them undefined might cause troubles for
PDF browsers, so we need to fix them; they point to the last page.

@c
static void check_nonexisting_destinations(PDF pdf)
{
    int k;
    for (k = pdf->head_tab[obj_type_dest]; k != 0; k = obj_link(pdf, k)) {
        if (obj_dest_ptr(pdf, k) == null) {
            pdf_warning("dest", NULL, false, false);
            if (obj_info(pdf, k) < 0) {
                tprint("name{");
                print(-obj_info(pdf, k));
                tprint("}");
            } else {
                tprint("num");
                print_int(obj_info(pdf, k));
            }
            tprint
                (" has been referenced but does not exist, replaced by a fixed one");
            print_ln();
            print_ln();
            pdf_begin_obj(pdf, k, 1);
            pdf_out(pdf, '[');
            pdf_print_int(pdf, pdf->last_page);
            pdf_puts(pdf, " 0 R /Fit]\n");
            pdf_end_obj(pdf);
        }
    }
}

@ @c
static void check_nonexisting_pages(PDF pdf)
{
    struct avl_traverser t;
    oentry *p;
    struct avl_table *page_tree = pdf->obj_tree[obj_type_page];
    avl_t_init(&t, page_tree);
    /* search from the end backward until the last real page is found */
    for (p = avl_t_last(&t, page_tree);
         p != NULL && obj_aux(pdf, p->objptr) == 0; p = avl_t_prev(&t)) {
        pdf_warning("dest", "Page ", false, false);
        print_int(obj_info(pdf, p->objptr));
        tprint(" has been referenced but does not exist!");
        print_ln();
        print_ln();
    }
}

@ If the same keys in a dictionary are given several times, then it is not
defined which value is choosen by an application.  Therefore the keys
|/Producer| and |/Creator| are only set if the token list
|pdf_info_toks| converted to a string does not contain these key strings.

@c
static boolean substr_of_str(const char *s, const char *t)
{
    if (strstr(t, s) == NULL)
        return false;
    return true;
}

static int pdf_print_info(PDF pdf, int luatex_version,
                          str_number luatex_revision)
{                               /* print info object */
    boolean creator_given, producer_given, creationdate_given, moddate_given,
        trapped_given;
    char *s = NULL;
    int k, len = 0;
    k = pdf_new_dict(pdf, obj_type_info, 0, 3); /* keep Info readable unless explicitely forced */
    creator_given = false;
    producer_given = false;
    creationdate_given = false;
    moddate_given = false;
    trapped_given = false;
    if (pdf_info_toks != 0) {
        s = tokenlist_to_cstring(pdf_info_toks, true, &len);
        creator_given = substr_of_str("/Creator", s);
        producer_given = substr_of_str("/Producer", s);
        creationdate_given = substr_of_str("/CreationDate", s);
        moddate_given = substr_of_str("/ModDate", s);
        trapped_given = substr_of_str("/Trapped", s);
    }
    if (!producer_given) {
        /* Print the Producer key */
        pdf_puts(pdf, "/Producer (LuaTeX-");
        pdf_print_int(pdf, luatex_version / 100);
        pdf_out(pdf, '.');
        pdf_print_int(pdf, luatex_version % 100);
        pdf_out(pdf, '.');
        pdf_print(pdf, luatex_revision);
        pdf_puts(pdf, ")\n");
    }
    if (pdf_info_toks != null) {
        if (len > 0) {
            pdf_puts(pdf, s);
            pdf_print_nl(pdf);
            xfree(s);
        }
        delete_token_ref(pdf_info_toks);
        pdf_info_toks = null;
    }
    if (!creator_given)
        pdf_str_entry_ln(pdf, "Creator", "TeX");
    if (!creationdate_given) {
        print_creation_date(pdf);
    }
    if (!moddate_given) {
        print_mod_date(pdf);
    }
    if (!trapped_given) {
        pdf_puts(pdf, "/Trapped /False\n");
    }
    pdf_str_entry_ln(pdf, "PTEX.Fullbanner", pdftex_banner);
    pdf_end_dict(pdf);
    return k;
}

static void build_free_object_list(PDF pdf)
{
    int k, l;
    l = 0;
    set_obj_fresh(pdf, l);      /* null object at begin of list of free objects */
    for (k = 1; k <= pdf->obj_ptr; k++) {
        if (!is_obj_written(pdf, k)) {
            set_obj_link(pdf, l, k);
            l = k;
        }
    }
    set_obj_link(pdf, l, 0);
}

@ Now the finish of PDF output file. At this moment all Page objects
are already written completely to PDF output file.

@c
void finish_pdf_file(PDF pdf, int luatex_version, str_number luatex_revision)
{
    boolean res;
    int i, j, k;
    int root, info, xref_stm = 0, outlines, threads, names_tree;
    size_t xref_offset_width;
    int callback_id = callback_defined(stop_run_callback);
    int callback_id1 = callback_defined(finish_pdffile_callback);

    if (total_pages == 0) {
        if (callback_id == 0) {
            tprint_nl("No pages of output.");
            print_ln();
        } else if (callback_id > 0) {
            res = run_callback(callback_id, "->");
        }
        if (pdf->gone > 0)
            garbage_warning();
    } else {
        if (pdf->draftmode == 0) {
            pdf_flush(pdf);     /* to make sure that the output file name has been already created */
            flush_jbig2_page0_objects(pdf);     /* flush page 0 objects from JBIG2 images, if any */
            if (callback_id1 > 0)
                res = run_callback(callback_id1, "->");

            check_nonexisting_pages(pdf);
            check_nonexisting_destinations(pdf);

            /* Output fonts definition */
            for (k = 1; k <= max_font_id(); k++) {
                if (font_used(k) && (pdf_font_num(k) < 0)) {
                    i = -pdf_font_num(k);
                    assert(pdf_font_num(i) > 0);
                    for (j = font_bc(k); j <= font_ec(k); j++)
                        if (quick_char_exists(k, j) && pdf_char_marked(k, j))
                            pdf_mark_char(i, j);
                    if ((pdf_font_attr(i) == 0) && (pdf_font_attr(k) != 0)) {
                        set_pdf_font_attr(i, pdf_font_attr(k));
                    } else if ((pdf_font_attr(k) == 0)
                               && (pdf_font_attr(i) != 0)) {
                        set_pdf_font_attr(k, pdf_font_attr(i));
                    } else if ((pdf_font_attr(i) != 0)
                               && (pdf_font_attr(k) != 0)
                               &&
                               (!str_eq_str
                                (pdf_font_attr(i), pdf_font_attr(k)))) {
                        pdf_warning("\\pdffontattr", "fonts ", false, false);
                        print_font_identifier(i);
                        tprint(" and ");
                        print_font_identifier(k);
                        tprint
                            (" have conflicting attributes; I will ignore the attributes assigned to ");
                        print_font_identifier(i);
                        print_ln();
                        print_ln();
                    }
                }
            }
            pdf->gen_tounicode = pdf_gen_tounicode;
            k = pdf->head_tab[obj_type_font];
            while (k != 0) {
                f = obj_info(pdf, k);
                assert(pdf_font_num(f) > 0);
                assert(pdf_font_num(f) == k);
                do_pdf_font(pdf, f);
                k = obj_link(pdf, k);
            }
            write_fontstuff(pdf);

            pdf->last_pages = output_pages_tree(pdf);
            pdflua_output_pages_tree(pdf);
            /* Output outlines */
            outlines = print_outlines(pdf);

            /* Output name tree */
            /* The name tree is very similiar to Pages tree so its construction should be
               certain from Pages tree construction. For intermediate node |obj_info| will be
               the first name and |obj_link| will be the last name in \.{\\Limits} array.
               Note that |pdf_dest_names_ptr| will be less than |obj_ptr|, so we test if
               |k < pdf_dest_names_ptr| then |k| is index of leaf in |dest_names|; else
               |k| will be index in |obj_tab| of some intermediate node.
             */
            names_tree = output_name_tree(pdf);

            /* Output article threads */
            if (pdf->head_tab[obj_type_thread] != 0) {
                threads = pdf_new_obj(pdf, obj_type_others, 0, 1);
                pdf_out(pdf, '[');
                k = pdf->head_tab[obj_type_thread];
                while (k != 0) {
                    pdf_print_int(pdf, k);
                    pdf_puts(pdf, " 0 R ");
                    k = obj_link(pdf, k);
                }
                pdf_remove_last_space(pdf);
                pdf_puts(pdf, "]\n");
                pdf_end_obj(pdf);
                k = pdf->head_tab[obj_type_thread];
                while (k != 0) {
                    out_thread(pdf, k);
                    k = obj_link(pdf, k);
                }
            } else {
                threads = 0;
            }

            /* Output the /Catalog object */
            root = pdf_new_dict(pdf, obj_type_catalog, 0, 1);
            pdf_puts(pdf, "/Type /Catalog\n");
            pdf_indirect_ln(pdf, "Pages", pdf->last_pages);
            if (threads != 0)
                pdf_indirect_ln(pdf, "Threads", threads);
            if (outlines != 0)
                pdf_indirect_ln(pdf, "Outlines", outlines);
            if (names_tree != 0)
                pdf_indirect_ln(pdf, "Names", names_tree);
            if (pdf_catalog_toks != null) {
                pdf_print_toks_ln(pdf, pdf_catalog_toks);
                delete_token_ref(pdf_catalog_toks);
                pdf_catalog_toks = null;
            }
            if (pdf_catalog_openaction != 0)
                pdf_indirect_ln(pdf, "OpenAction", pdf_catalog_openaction);
            pdf_end_dict(pdf);

            /* last candidate for object stream */
            info = pdf_print_info(pdf, luatex_version, luatex_revision);        /* final object for pdf->os_enable == false */

            if (pdf->os_enable) {
                pdf_os_switch(pdf, true);
                pdf_os_write_objstream(pdf);
                pdf_flush(pdf);
                pdf_os_switch(pdf, false);
                /* Output the cross-reference stream dictionary */
                xref_stm = pdf_new_dict(pdf, obj_type_others, 0, 0);    /* final object for pdf->os_enable == true */
                if ((obj_offset(pdf, pdf->obj_ptr) / 256) > 16777215)
                    xref_offset_width = 5;
                else if (obj_offset(pdf, pdf->obj_ptr) > 16777215)
                    xref_offset_width = 4;
                else if (obj_offset(pdf, pdf->obj_ptr) > 65535)
                    xref_offset_width = 3;
                else
                    xref_offset_width = 2;
                /* Build a linked list of free objects */
                build_free_object_list(pdf);
                pdf_puts(pdf, "/Type /XRef\n");
                pdf_puts(pdf, "/Index [0 ");
                pdf_print_int(pdf, pdf->obj_ptr + 1);
                pdf_puts(pdf, "]\n");
                pdf_int_entry_ln(pdf, "Size", pdf->obj_ptr + 1);
                pdf_puts(pdf, "/W [1 ");
                pdf_print_int(pdf, (int) xref_offset_width);
                pdf_puts(pdf, " 1]\n");
                pdf_indirect_ln(pdf, "Root", root);
                pdf_indirect_ln(pdf, "Info", info);
                if (pdf_trailer_toks != null) {
                    pdf_print_toks_ln(pdf, pdf_trailer_toks);
                    delete_token_ref(pdf_trailer_toks);
                    pdf_trailer_toks = null;
                }
                print_ID(pdf, pdf->file_name);
                pdf_print_nl(pdf);
                pdf_begin_stream(pdf);
                for (k = 0; k <= pdf->obj_ptr; k++) {
                    if (!is_obj_written(pdf, k)) {      /* a free object */
                        pdf_out(pdf, 0);
                        pdf_out_bytes(pdf, obj_link(pdf, k), xref_offset_width);
                        pdf_out(pdf, 255);
                    } else if (obj_os_idx(pdf, k) == -1) {      /* object not in object stream */
                        pdf_out(pdf, 1);
                        pdf_out_bytes(pdf, obj_offset(pdf, k),
                                      xref_offset_width);
                        pdf_out(pdf, 0);
                    } else {    /* object in object stream */
                        pdf_out(pdf, 2);
                        pdf_out_bytes(pdf, obj_offset(pdf, k),
                                      xref_offset_width);
                        pdf_out(pdf, obj_os_idx(pdf, k));
                    }
                }
                pdf_end_stream(pdf);
                /* TODO: generate a debug version of the crossref */

                pdf_flush(pdf);
            } else {
                /* Output the |obj_tab| */
                /* Build a linked list of free objects */
                build_free_object_list(pdf);

                pdf_save_offset(pdf);
                pdf_puts(pdf, "xref\n");
                pdf_puts(pdf, "0 ");
                pdf_print_int_ln(pdf, pdf->obj_ptr + 1);
                pdf_print_fw_int(pdf, obj_link(pdf, 0), 10);
                pdf_puts(pdf, " 65535 f \n");
                for (k = 1; k <= pdf->obj_ptr; k++) {
                    if (!is_obj_written(pdf, k)) {
                        pdf_print_fw_int(pdf, obj_link(pdf, k), 10);
                        pdf_puts(pdf, " 00000 f \n");
                    } else {
                        pdf_print_fw_int(pdf, obj_offset(pdf, k), 10);
                        pdf_puts(pdf, " 00000 n \n");
                    }
                }

            }

            /* Output the trailer */
            if (!pdf->os_enable) {
                pdf_puts(pdf, "trailer\n");
                pdf_puts(pdf, "<< ");
                pdf_int_entry_ln(pdf, "Size", pdf->obj_ptr + 1);
                pdf_indirect_ln(pdf, "Root", root);
                pdf_indirect_ln(pdf, "Info", info);
                if (pdf_trailer_toks != null) {
                    pdf_print_toks_ln(pdf, pdf_trailer_toks);
                    delete_token_ref(pdf_trailer_toks);
                    pdf_trailer_toks = null;
                }
                print_ID(pdf, pdf->file_name);
                pdf_puts(pdf, " >>\n");
            }
            pdf_puts(pdf, "startxref\n");
            if (pdf->os_enable)
                pdf_print_int_ln(pdf, obj_offset(pdf, xref_stm));
            else
                pdf_print_int_ln(pdf, pdf_saved_offset(pdf));
            pdf_puts(pdf, "%%EOF\n");

            pdf_flush(pdf);
            if (callback_id == 0) {
                tprint_nl("Output written on ");
                tprint(pdf->file_name);
                tprint(" (");
                print_int(total_pages);
                tprint(" page");
                if (total_pages != 1)
                    print_char('s');
                tprint(", ");
                print_int(pdf_offset(pdf));
                tprint(" bytes).");
                print_ln();
            } else if (callback_id > 0) {
                res = run_callback(callback_id, "->");
            }
        }
        libpdffinish(pdf);
        if (pdf->draftmode == 0)
            close_file(pdf->file);
        else
            pdf_warning(NULL,
                        "\\pdfdraftmode enabled, not changing output pdf",
                        true, true);
    }

    if (callback_id == 0) {
        if (log_opened) {
            fprintf(log_file,
                    "\nPDF statistics: %d PDF objects out of %d (max. %d)\n",
                    (int) pdf->obj_ptr, (int) pdf->obj_tab_size,
                    (int) sup_obj_tab_size);
            if (pdf->os_cntr > 0) {
                fprintf(log_file,
                        " %d compressed objects within %d object stream%s\n",
                        (int) ((pdf->os_cntr - 1) * pdf_os_max_objs +
                               pdf->os_idx + 1), (int) pdf->os_cntr,
                        (pdf->os_cntr > 1 ? "s" : ""));
            }
            fprintf(log_file, " %d named destinations out of %d (max. %d)\n",
                    (int) pdf->dest_names_ptr, (int) pdf->dest_names_size,
                    (int) sup_dest_names_size);
            fprintf(log_file,
                    " %d words of extra memory for PDF output out of %d (max. %d)\n",
                    (int) pdf->mem_ptr, (int) pdf->mem_size,
                    (int) sup_pdf_mem_size);
        }
    }
}

@ @c
void scan_pdfcatalog(PDF pdf)
{
    halfword p;
    scan_pdf_ext_toks();
    pdf_catalog_toks = concat_tokens(pdf_catalog_toks, def_ref);
    if (scan_keyword("openaction")) {
        if (pdf_catalog_openaction != 0) {
            pdf_error("ext1", "duplicate of openaction");
        } else {
            check_o_mode(pdf, "\\pdfcatalog", 1 << OMODE_PDF, true);
            p = scan_action(pdf);
            pdf_catalog_openaction = pdf_new_obj(pdf, obj_type_others, 0, 1);
            write_action(pdf, p);
            pdf_end_obj(pdf);
            delete_action_ref(p);
        }
    }
}
