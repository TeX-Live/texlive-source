/* epdf.h

   Copyright 1996-2006 Han The Thanh <thanh@pdftex.org>
   Copyright 2006-2010 Taco Hoekwater <taco@luatex.org>
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

/* $Id: epdf.h 4054 2011-01-10 19:05:54Z hhenkel $ */

// this is the common header file for C++ sources pdftoepdf.cc and lepdflib.cc

#ifndef EPDF_H
#  define EPDF_H
extern "C" {
#ifdef HAVE_CONFIG_H
#include <w2c/config.h>
#endif
}
#  include <stdlib.h>
#  include <math.h>
#  include <stddef.h>
#  include <stdio.h>
#  include <string.h>
#  include <kpathsea/c-ctype.h>
#  include <sys/stat.h>
#  include <dirent.h>
#  include <poppler-config.h>
#  include <goo/GooString.h>
#  include <goo/gmem.h>
#  include <goo/gfile.h>
#  include "Object.h"
#  include "Stream.h"
#  include "Gfx.h"
#  include "Annot.h"
#  include "Array.h"
#  include "Dict.h"
#  include "XRef.h"
#  include "Catalog.h"
#  include "Link.h"
#  include "Page.h"
#  include "GfxFont.h"
#  include "PDFDoc.h"
#  include "GlobalParams.h"
#  include "Error.h"

extern "C" {

#  include <kpathsea/c-auto.h>

    extern char *xstrdup(const char *);

    typedef enum { FE_FAIL, FE_RETURN_NULL } file_error_mode;

/* the following code is extremly ugly but needed for including web2c/config.h */

    typedef const char *const_string;   /* including kpathsea/types.h doesn't work on some systems */

#  define KPATHSEA_CONFIG_H     /* avoid including other kpathsea header files */
    /* from web2c/config.h */

#  ifdef CONFIG_H               /* CONFIG_H has been defined by some xpdf */
#    undef CONFIG_H             /* header file */
#  endif

#  include <c-auto.h>           /* define SIZEOF_LONG */

#  include "openbsd-compat.h"
#  include "image.h"
#  include "utils/avlstuff.h"
#  include "pdf/pdftypes.h"

#  include "lua51/lua.h"
#  include "lua51/lauxlib.h"

    /* pdfgen.c */
    __attribute__ ((format(printf, 2, 3)))
    extern void pdf_printf(PDF, const char *fmt, ...);
    extern void pdf_begin_obj(PDF, int, bool);
    extern void pdf_end_obj(PDF);
    extern void pdf_begin_stream(PDF);
    extern void pdf_end_stream(PDF);
    extern void pdf_room(PDF, int);
    extern void pdf_out_block(PDF pdf, const char *s, size_t n);

#  define pdf_out(B, A) do { pdf_room(B, 1); B->buf[B->ptr++] = A; } while (0)
#  define pdf_puts(pdf, s) pdf_out_block((pdf), (s), strlen(s))

    /* pdftables.c */
    extern int pdf_new_objnum(PDF);

    /* pdftoepdf.cc */
    extern void read_pdf_info(image_dict *, int, int, img_readtype_e);
    extern void write_epdf(PDF, image_dict *);
    extern void unrefPdfDocument(char *);
    extern void epdf_free(void);

    /* utils.c */
    __attribute__ ((format(printf, 1, 2)))
    extern void pdftex_warn(const char *fmt, ...);
    __attribute__ ((noreturn, format(printf, 1, 2)))
    extern void pdftex_fail(const char *fmt, ...);
    extern char *convertStringToPDFString(char *in, int len);
    extern char *stripzeros(char *a);

    /* lepdflib.c */
    int luaopen_epdf(lua_State * L);

};

/**********************************************************************/

// PdfObject encapsulates the xpdf Object type,
// and properly frees its resources on destruction.
// Use obj-> to access members of the Object,
// and &obj to get a pointer to the object.
// It is no longer necessary to call Object::free explicitely.

#  if 0
// PdfObject is replaced by xpdf's Object type, with manual obj.free()

// *INDENT-OFF*
class PdfObject {
  public:
    PdfObject() {               // nothing
    }
    ~PdfObject() {
        iObject.free();
    }
    Object *operator->() {
        return &iObject;
    }
    Object *operator&() {
        return &iObject;
    }
  private:                     // no copying or assigning
    PdfObject(const PdfObject &);
    void operator=(const PdfObject &);
  public:
    Object iObject;
};
// *INDENT-ON*
#  endif

/**********************************************************************/

struct InObj {
    Ref ref;                    // ref in original PDF
    int num;                    // new object number in output PDF
    InObj *next;                // next entry in list of indirect objects
};

struct PdfDocument {
    char *file_path;            // full file name including path
    char *checksum;             // for reopening
    PDFDoc *doc;
    InObj *inObjList;           // temporary linked list
    avl_table *ObjMapTree;      // permanent over luatex run
    unsigned int occurences;    // number of references to the PdfDocument; it can be deleted when occurences == 0
    unsigned int pc;            // counter to track PDFDoc generation or deletion
};

PdfDocument *refPdfDocument(char *file_path, file_error_mode fe);

#endif                          /* EPDF_H */
