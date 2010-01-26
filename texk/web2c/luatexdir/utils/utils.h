/* utils.h

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

/* $Id: utils.h 2333 2009-04-18 17:45:46Z hhenkel $ */

#ifndef UTILS_H
#  define UTILS_H

#  define overflow_string(a,b) { overflow(maketexstring(a),b); flush_str(last_tex_string); }

integer fb_offset(void);
void fb_flush(void);
void fb_putchar(eight_bits b);
void fb_seek(integer);
void make_subset_tag(fd_entry *);
void pdf_puts(const char *);
__attribute__ ((format(printf, 1, 2)))
void pdf_printf(const char *, ...);
str_number maketexstring(const char *);
str_number maketexlstring(const char *, size_t);
void print_string(const char *j);
void append_string(const char *s);
__attribute__ ((format(printf, 1, 2)))
void tex_printf(const char *, ...);
void remove_pdffile(void);
__attribute__ ((noreturn, format(printf, 1, 2)))
void pdftex_fail(const char *, ...);
__attribute__ ((format(printf, 1, 2)))
void pdftex_warn(const char *, ...);
void tex_error(const char *msg, const char **hlp);
void garbage_warning(void);
char *makecstring(integer);
char *makeclstring(integer, size_t *);
void set_job_id(int, int, int, int);
void make_pdftex_banner(void);
str_number get_resname_prefix(void);
size_t xfwrite(void *, size_t size, size_t nmemb, FILE *);
int xfflush(FILE *);
int xgetc(FILE *);
int xputc(int, FILE *);
void write_stream_length(integer, longinteger);
scaled ext_xn_over_d(scaled, scaled, scaled);
void libpdffinish(void);
char *convertStringToPDFString(const char *in, int len);
void escapestring(poolpointer in);
void escapename(poolpointer in);
void escapehex(poolpointer in);
void unescapehex(poolpointer in);
void print_ID(str_number);
void init_start_time(void);
void print_creation_date(void);
void print_mod_date(void);
void getcreationdate(void);
char *makecfilename(str_number s);
char *stripzeros(char *);
void initversionstring(char **versions);
extern void check_buffer_overflow(int wsize);
extern void check_pool_overflow(int wsize);

extern str_number last_tex_string;
extern char *cur_file_name;
extern size_t last_ptr_index;
extern char *fb_array;

/**********************************************************************/
/* color stack and matrix transformation support */

int newcolorstack(integer s, integer literal_mode, boolean pagestart);
int colorstackused(void);
integer colorstackset(int colstack_no, integer s);
integer colorstackpush(int colstack_no, integer s);
integer colorstackpop(int colstack_no);
integer colorstackcurrent(int colstack_no);
integer colorstackskippagestart(int colstack_no);
void checkpdfsave(scaledpos pos);
void checkpdfrestore(scaledpos pos);
void pdfshipoutbegin(boolean shipping_page);
void pdfshipoutend(boolean shipping_page);
void pdfsetmatrix(poolpointer in, scaledpos pos);
scaled getllx(void);
scaled getlly(void);
scaled geturx(void);
scaled getury(void);
void tconfusion(const char *s);
void tprint(const char *s);
void tprint_nl(const char *s);
void tprint_esc(const char *s);
void matrixtransformpoint(scaled x, scaled y);
void matrixtransformrect(scaled llx, scaled lly, scaled urx, scaled ury);
boolean matrixused(void);
void matrixrecalculate(scaled urx);

#endif                          /* UTILS_H */
