/*
Copyright (c) 2010 Han The Thanh, <thanh@pdftex.org>

This file is part of pdfTeX.

pdfTeX is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

pdfTeX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with pdfTeX; if not, write to the Free Software Foundation, Inc., 51
Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

/* Whereas the C code uses:     #include "ptexlib.h"
   pdftoepdf.cc uses:           #include "epdf.h"

   Unfortunately these to headers are incompatible; thus they
  include this file with prototypes required in both cases.
*/

/* epdf.c */
#ifdef __cplusplus
# define fd_entry struct fd_entry
# define fm_entry struct fm_entry
#endif
extern void embed_whole_font(fd_entry *);
extern fd_entry *epdf_create_fontdescriptor(fm_entry *, int);
extern void epdf_free(void);
extern void epdf_mark_glyphs(fd_entry *, char *);
extern int get_fd_objnum(fd_entry *);
extern int get_fn_objnum(fd_entry *);
extern int is_subsetable(fm_entry *);
#undef fd_entry
#undef fm_entry

/* pdftoepdf.cc */
extern void epdf_check_mem(void);
extern void epdf_delete(void);
extern int read_pdf_info(char *, char *, int, int, int, int);

/* utils.c */
extern char *convertStringToPDFString(const char *in, int len);
extern void pdf_newline(void);
__attribute__ ((format(printf, 1, 2)))
extern void pdf_printf(const char *, ...);
extern void pdf_puts(const char *);
__attribute__ ((noreturn, format(printf, 1, 2)))
extern void pdftex_fail(const char *, ...);
__attribute__ ((format(printf, 1, 2)))
extern void pdftex_warn(const char *, ...);
extern char *stripzeros(char *a);
__attribute__ ((format(printf, 1, 2)))
extern void tex_printf(const char *, ...);
extern void write_epdf(void);

/* writeenc.c */
extern void epdf_write_enc(char **, int);
