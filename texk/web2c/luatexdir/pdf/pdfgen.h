/* pdfgen.h

   Copyright 2009-2010 Taco Hoekwater <taco@luatex.org>

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

/* $Id: pdfgen.h 3940 2010-11-01 22:46:24Z hhenkel $ */

#ifndef PDFGEN_H
#  define PDFGEN_H

#  define PROCSET_PDF     (1 << 0)
#  define PROCSET_TEXT    (1 << 1)
#  define PROCSET_IMAGE_B (1 << 2)
#  define PROCSET_IMAGE_C (1 << 3)
#  define PROCSET_IMAGE_I (1 << 4)

#  define inf_pdf_mem_size 10000        /* min size of the |mem| array */
#  define sup_pdf_mem_size 10000000     /* max size of the |mem| array */

extern PDF static_pdf;

extern int pdf_get_mem(PDF pdf, int s);

/*
We use the similiar subroutines to handle the output buffer for
PDF output. When compress is used, the state of writing to buffer
is held in |zip_write_state|. We must write the header of PDF
output file in initialization to ensure that it will be the first
written bytes.
*/

#  define inf_pdf_op_buf_size 16384     /* size of the PDF output buffer */
#  define sup_pdf_op_buf_size 16384     /* size of the PDF output buffer */
#  define inf_pdf_os_buf_size 1 /* initial value of |pdf_os_buf_size| */
#  define sup_pdf_os_buf_size 5000000   /* arbitrary upper hard limit of |pdf_os_buf_size| */
#  define max_single_pdf_print 8192     /* Max size that can be get from pdf_room() at once.
                                           the value is on the conservative side, but should be
                                           large enough to cover most uses */
#  define pdf_os_max_objs 100   /* maximum number of objects in object stream */

#  define inf_obj_tab_size 1000 /* min size of the cross-reference table for PDF output */
#  define sup_obj_tab_size 8388607      /* max size of the cross-reference table for PDF output */

/* The following macros are similar as for \.{DVI} buffer handling */

#  define pdf_offset(pdf) (pdf->gone + pdf->ptr)
                                        /* the file offset of last byte in PDF
                                           buffer that |pdf_ptr| points to */
#  define pdf_save_offset(pdf) pdf->save_offset=(pdf->gone + pdf->ptr)
#  define pdf_saved_offset(pdf) pdf->save_offset

#  define set_ff(A)  do {                       \
        if (pdf_font_num(A) < 0)                \
            ff = -pdf_font_num(A);              \
        else                                    \
            ff = A;                             \
    } while (0)

typedef enum {
    no_zip = 0,                 /* no \.{ZIP} compression */
    zip_writing = 1,            /* \.{ZIP} compression being used */
    zip_finish = 2              /* finish \.{ZIP} compression */
} zip_write_states;

typedef enum { NOT_SHIPPING, SHIPPING_PAGE, SHIPPING_FORM } shipping_mode_e;

extern int pdf_output_option;
extern int pdf_output_value;
extern int pdf_draftmode_option;
extern int pdf_draftmode_value;

extern scaled one_hundred_inch;
extern scaled one_inch;
extern scaled one_true_inch;
extern scaled one_hundred_bp;
extern scaled one_bp;
extern int ten_pow[10];

extern void pdf_flush(PDF);
extern void pdf_room(PDF, int);

extern void fix_pdf_minorversion(PDF);

 /* output a byte to PDF buffer without checking of overflow */
#  define pdf_quick_out(pdf,A) pdf->buf[pdf->ptr++]=(unsigned char)A

/* do the same as |pdf_quick_out| and flush the PDF buffer if necessary */
#  define pdf_out(pdf,A) do { pdf_room(pdf,1); pdf_quick_out(pdf,A); } while (0)

#  if 0
/* see function pdf_out_block() */
#    define pdf_out_block_macro(pdf,A,n) do {               \
        pdf_room(pdf,(int)(n));                             \
        (void)memcpy((pdf->buf+pdf->ptr),(A),(size_t)(n));  \
        pdf->ptr+=(int)(n);                                 \
    } while (0)
#  endif

/*
Basic printing procedures for PDF output are very similiar to \TeX\ basic
printing ones but the output is going to PDF buffer. Subroutines with
suffix |_ln| append a new-line character to the PDF output.
*/

#  define pdf_newline_char 10   /* new-line character '\n' for UNIX platforms */

/* output a new-line character to PDF buffer */
#  define pdf_print_nl(pdf) pdf_out(pdf,pdf_newline_char)

/* print out a string to PDF buffer followed by a new-line character */
#  define pdf_print_ln(pdf,A) do {                 \
        pdf_print(pdf,A);                          \
        pdf_print_nl(pdf);                         \
    } while (0)

/* print out an integer to PDF buffer followed by a new-line character */
#  define pdf_print_int_ln(pdf,A) do {            \
        pdf_print_int(pdf,A);                     \
        pdf_print_nl(pdf);                        \
    } while (0)

extern __attribute__ ((format(printf, 2, 3)))
void pdf_printf(PDF, const char *, ...);

extern void pdf_print_char(PDF, int);
extern void pdf_print_wide_char(PDF, int);
extern void pdf_print(PDF, str_number);
extern void pdf_print_int(PDF, longinteger);
extern void pdf_print_real(PDF, int, int);
extern void pdf_print_str(PDF, const char *);

extern void pdf_begin_stream(PDF);
extern void pdf_end_stream(PDF);
extern void pdf_remove_last_space(PDF);

extern void pdf_print_bp(PDF, scaled);
extern void pdf_print_mag_bp(PDF, scaled);

/* This is for the resource lists */

extern void addto_page_resources(PDF pdf, pdf_obj_type t, int k);
extern pdf_object_list *get_page_resources_list(PDF pdf, pdf_obj_type t);

extern void pdf_out_block(PDF pdf, const char *s, size_t n);

#  define pdf_puts(pdf, s) pdf_out_block((pdf), (s), strlen(s))

#  define pdf_print_resname_prefix(pdf) do {        \
        if (pdf->resname_prefix != NULL)            \
            pdf_puts(pdf, pdf->resname_prefix);     \
    } while (0)

extern void pdf_int_entry(PDF, const char *, int);
extern void pdf_int_entry_ln(PDF, const char *, int);
extern void pdf_indirect(PDF, const char *, int);
extern void pdf_indirect_ln(PDF, const char *, int);
extern void pdf_print_str_ln(PDF, const char *);
extern void pdf_str_entry(PDF, const char *, const char *);
extern void pdf_str_entry_ln(PDF, const char *, const char *);

extern void pdf_print_toks(PDF, halfword);
extern void pdf_print_toks_ln(PDF, halfword);

extern void pdf_print_rect_spec(PDF, halfword);
extern void pdf_rectangle(PDF, halfword);

extern void pdf_begin_obj(PDF, int, int);
extern int pdf_new_obj(PDF, int, int, int);
extern void pdf_end_obj(PDF);

extern void pdf_begin_dict(PDF, int, int);
extern int pdf_new_dict(PDF, int, int, int);
extern void pdf_end_dict(PDF);

extern void remove_pdffile(PDF);

extern int fb_offset(PDF);
extern void fb_flush(PDF);
extern void fb_putchar(PDF, eight_bits);
extern void fb_seek(PDF, int);
extern void fb_free(PDF);

extern void zip_free(PDF);

/* functions that do not output stuff */

extern scaled round_xn_over_d(scaled x, int n, unsigned int d);
extern char *convertStringToPDFString(const char *in, int len);

extern void init_start_time(PDF);
extern char *getcreationdate(PDF);

extern void pdf_error(const char *t, const char *p);
extern void pdf_warning(const char *t, const char *p, boolean pr, boolean ap);
extern void check_o_mode(PDF pdf, const char *s, int o_mode, boolean errorflag);

extern void set_job_id(PDF, int, int, int, int);
extern char *get_resname_prefix(PDF);
extern void pdf_begin_page(PDF pdf);
extern void pdf_end_page(PDF pdf);
extern void print_pdf_table_string(PDF pdf, const char *s);

extern void fix_o_mode(PDF pdf);
extern void ensure_output_state(PDF pdf, output_state s);
extern PDF init_pdf_struct(PDF pdf);

extern halfword pdf_info_toks;  /* additional keys of Info dictionary */
extern halfword pdf_catalog_toks;       /* additional keys of Catalog dictionary */
extern halfword pdf_catalog_openaction;
extern halfword pdf_names_toks; /* additional keys of Names dictionary */
extern halfword pdf_trailer_toks;       /* additional keys of Trailer dictionary */
extern void scan_pdfcatalog(PDF pdf);
extern void finish_pdf_file(PDF pdf, int luatex_version,
                            str_number luatex_revision);

extern shipping_mode_e global_shipping_mode;

#endif                          /* PDFGEN_H */
