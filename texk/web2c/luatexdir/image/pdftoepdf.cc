/* pdftoepdf.cc
   
   Copyright 1996-2006 Han The Thanh <thanh@pdftex.org>
   Copyright 2006-2008 Taco Hoekwater <taco@luatex.org>

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

#include <stdlib.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <aconf.h>
#include <GString.h>
#include <gmem.h>
#include <gfile.h>
#include <assert.h>
#include "Object.h"
#include "Stream.h"
#include "Array.h"
#include "Dict.h"
#include "XRef.h"
#include "Catalog.h"
#include "Link.h"
#include "Page.h"
#include "GfxFont.h"
#include "PDFDoc.h"
#include "GlobalParams.h"
#include "Error.h"

#include "epdf.h"

static const char _svn_version[] =
    "$Id: pdftoepdf.cc 1539 2008-10-06 09:52:07Z taco $ $URL: http://scm.foundry.supelec.fr/svn/luatex/trunk/src/texk/web2c/luatexdir/image/pdftoepdf.cc $";

#define one_hundred_bp  6578176 /* 7227 * 65536 / 72 */

// This file is mostly C and not very much C++; it's just used to interface
// the functions of xpdf, which happens to be written in C++.

// The prefix "PTEX" for the PDF keys is special to pdfTeX;
// this has been registered with Adobe by Hans Hagen.

#define pdfkeyprefix "PTEX"

// PdfObject encapsulates the xpdf Object type,
// and properly frees its resources on destruction.
// Use obj-> to access members of the Object,
// and &obj to get a pointer to the object.
// It is no longer necessary to call Object::free explicitely.

/* *INDENT-OFF* */
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
/* *INDENT-ON* */

// When copying the Resources of the selected page, all objects are copied
// recusively top-down. Indirect objects however are not fetched during
// copying, but get a new object number from pdfTeX and then will be
// appended into a linked list. Duplicates are checked and removed from the
// list of indirect objects during appending.

enum InObjType { objFont, objFontDesc, objOther };

struct InObj {
    Ref ref;                    // ref in original PDF
    InObjType type;             // object type
    integer num;                // new object number in output PDF
    fd_entry *fd;               // pointer to /FontDescriptor object structure
    integer enc_objnum;         // Encoding for objFont
    int written;                // has it been written to output PDF?
    InObj *next;                // next entry in list of indirect objects
};

struct UsedEncoding {
    integer enc_objnum;
    GfxFont *font;
    UsedEncoding *next;
};

static XRef *xref = NULL;
static InObj *inObjList = NULL;
static UsedEncoding *encodingList;
static GBool isInit = gFalse;
static bool groupIsIndirect;
static PdfObject lastGroup;

//**********************************************************************
// Maintain AVL tree of open embedded PDF files

static avl_table *PdfDocumentTree = NULL;

struct PdfDocument {
    char *file_path;            // full file name including path
    PDFDoc *doc;
    XRef *xref;
    InObj *inObjList;
    int occurences;             // number of references to the PdfDocument; it can be deleted when occurences == 0
};

/* AVL sort PdfDocument into PdfDocumentTree by file_path */

static int CompPdfDocument(const void *pa, const void *pb, void *p)
{
    return strcmp(((const PdfDocument *) pa)->file_path,
                  ((const PdfDocument *) pb)->file_path);
}

// Returns pointer to PdfDocument structure for PDF file.

static PdfDocument *findPdfDocument(char *file_path)
{
    PdfDocument *pdf_doc, tmp;
    assert(file_path != NULL);
    if (PdfDocumentTree == NULL)
        PdfDocumentTree = avl_create(CompPdfDocument, NULL, &avl_xallocator);
    assert(PdfDocumentTree != NULL);
    tmp.file_path = file_path;
    pdf_doc = (PdfDocument *) avl_find(PdfDocumentTree, &tmp);
    return pdf_doc;
}

// Returns pointer to PdfDocument structure for PDF file.
// Creates a new structure if it doesn't exist yet.

static PdfDocument *refPdfDocument(char *file_path)
{
    PdfDocument *pdf_doc = findPdfDocument(file_path);
    if (pdf_doc == NULL) {
        pdf_doc = new PdfDocument;
        pdf_doc->file_path = xstrdup(file_path);
        void **aa = avl_probe(PdfDocumentTree, pdf_doc);
        assert(aa != NULL);
        GString *docName = new GString(pdf_doc->file_path);
        pdf_doc->doc = new PDFDoc(docName);     // takes ownership of docName
        if (!pdf_doc->doc->isOk() || !pdf_doc->doc->okToPrint())
            pdftex_fail("xpdf: reading PDF image failed");
        pdf_doc->xref = NULL;
        pdf_doc->inObjList = NULL;
        pdf_doc->occurences = 0;        // 0 = unreferenced
#ifdef DEBUG
        fprintf(stderr, "\nluatex Debug: Creating %s (%d)\n",
                pdf_doc->file_path, pdf_doc->occurences);
#endif
    }
    pdf_doc->occurences++;
#ifdef DEBUG
    fprintf(stderr, "\nluatex Debug: Incrementing %s (%d)\n",
            pdf_doc->file_path, pdf_doc->occurences);
#endif
    return pdf_doc;
}

static void deletePdfDocument(PdfDocument *);

// Called when an image has been written and its resources in image_tab are
// freed and it's not referenced anymore.

void unrefPdfDocument(char *file_path)
{
    PdfDocument *pdf_doc = findPdfDocument(file_path);
    assert(pdf_doc != NULL);
    assert(pdf_doc->occurences > 0);    // aim for point landing
    pdf_doc->occurences--;
#ifdef DEBUG
    fprintf(stderr, "\nluatex Debug: Decrementing %s (%d)\n",
            pdf_doc->file_path, pdf_doc->occurences);
#endif
    if (pdf_doc->occurences == 0) {
#ifdef DEBUG
        fprintf(stderr, "\nluatex Debug: Deleting %s\n", pdf_doc->file_path);
#endif
        void *a = avl_delete(PdfDocumentTree, pdf_doc);
        assert((PdfDocument *) a == pdf_doc);
        deletePdfDocument(pdf_doc);
    }
}

//**********************************************************************

// Replacement for
//      Object *initDict(Dict *dict1){ initObj(objDict); dict = dict1; return this; }

static void initDictFromDict(PdfObject & obj, Dict * dict)
{
    obj->initDict(xref);
    for (int i = 0, l = dict->getLength(); i < l; i++) {
        Object obj1;
        obj->dictAdd(copyString(dict->getKey(i)), dict->getValNF(i, &obj1));
    }
}

static int addEncoding(GfxFont * gfont)
{
    UsedEncoding *n;
    n = new UsedEncoding;
    n->next = encodingList;
    encodingList = n;
    n->font = gfont;
    n->enc_objnum = pdf_new_objnum();
    return n->enc_objnum;
}

#define addFont(ref, fd, enc_objnum) \
        addInObj(objFont, ref, fd, enc_objnum)

// addFontDesc is only used to avoid writing the original FontDescriptor
// from the PDF file.

#define addFontDesc(ref, fd) \
        addInObj(objFontDesc, ref, fd, 0)

#define addOther(ref) \
        addInObj(objOther, ref, NULL, 0)

static int addInObj(InObjType type, Ref ref, fd_entry * fd, integer e)
{
    InObj *p, *q, *n = new InObj;
    if (ref.num == 0)
        pdftex_fail("PDF inclusion: invalid reference");
    n->ref = ref;
    n->type = type;
    n->next = NULL;
    n->fd = fd;
    n->enc_objnum = e;
    n->written = 0;
    if (inObjList == NULL)
        inObjList = n;
    else {
        for (p = inObjList; p != NULL; p = p->next) {
            if (p->ref.num == ref.num && p->ref.gen == ref.gen) {
                delete n;
                return p->num;
            }
            q = p;
        }
        // it is important to add new objects at the end of the list,
        // because new objects are being added while the list is being
        // written out by writeRefs().
        q->next = n;
    }
    if (type == objFontDesc)
        n->num = get_fd_objnum(fd);
    else
        n->num = pdf_new_objnum();
    return n->num;
}

static void copyName(char *s)
{
    pdf_puts("/");
    for (; *s != 0; s++) {
        if (isdigit(*s) || isupper(*s) || islower(*s) || *s == '_' ||
            *s == '.' || *s == '-' || *s == '+')
            pdfout(*s);
        else
            pdf_printf("#%.2X", *s & 0xFF);
    }
}

static int getNewObjectNumber(Ref ref)
{
    InObj *p;
    if (inObjList == 0) {
        pdftex_fail("No objects copied yet");
    } else {
        for (p = inObjList; p != 0; p = p->next) {
            if (p->ref.num == ref.num && p->ref.gen == ref.gen) {
                return p->num;
            }
        }
        pdftex_fail("Object not yet copied: %i %i", ref.num, ref.gen);
    }
}

static void copyObject(Object *);

static void copyDictEntry(Object * obj, int i)
{
    PdfObject obj1;
    copyName(obj->dictGetKey(i));
    pdf_puts(" ");
    obj->dictGetValNF(i, &obj1);
    copyObject(&obj1);
    pdf_puts("\n");
}

static void copyDict(Object * obj)
{
    int i, l;
    if (!obj->isDict())
        pdftex_fail("PDF inclusion: invalid dict type <%s>",
                    obj->getTypeName());
    for (i = 0, l = obj->dictGetLength(); i < l; ++i)
        copyDictEntry(obj, i);
}

static void copyFontDict(Object * obj, InObj * r)
{
    int i, l;
    char *key;
    if (!obj->isDict())
        pdftex_fail("PDF inclusion: invalid dict type <%s>",
                    obj->getTypeName());
    pdf_puts("<<\n");
    assert(r->type == objFont); // FontDescriptor is in fd_tree
    for (i = 0, l = obj->dictGetLength(); i < l; ++i) {
        key = obj->dictGetKey(i);
        if (strncmp("FontDescriptor", key, strlen("FontDescriptor")) == 0
            || strncmp("BaseFont", key, strlen("BaseFont")) == 0
            || strncmp("Encoding", key, strlen("Encoding")) == 0)
            continue;           // skip original values
        copyDictEntry(obj, i);
    }
    // write new FontDescriptor, BaseFont, and Encoding
    pdf_printf("/FontDescriptor %d 0 R\n", (int) get_fd_objnum(r->fd));
    pdf_printf("/BaseFont %d 0 R\n", (int) get_fn_objnum(r->fd));
    pdf_printf("/Encoding %d 0 R\n", (int) r->enc_objnum);
    pdf_puts(">>");
}

static void copyStream(Stream * str)
{
    int c;
    str->reset();
    while ((c = str->getChar()) != EOF) {
        pdfout(c);
        pdf_last_byte = c;
    }
}

static void copyProcSet(Object * obj)
{
    int i, l;
    PdfObject procset;
    if (!obj->isArray())
        pdftex_fail("PDF inclusion: invalid ProcSet array type <%s>",
                    obj->getTypeName());
    pdf_puts("/ProcSet [ ");
    for (i = 0, l = obj->arrayGetLength(); i < l; ++i) {
        obj->arrayGetNF(i, &procset);
        if (!procset->isName())
            pdftex_fail("PDF inclusion: invalid ProcSet entry type <%s>",
                        procset->getTypeName());
        copyName(procset->getName());
        pdf_puts(" ");
    }
    pdf_puts("]\n");
}

#define REPLACE_TYPE1C true

static void copyFont(char *tag, Object * fontRef)
{
    PdfObject fontdict, subtype, basefont, fontdescRef, fontdesc, charset,
        fontfile, ffsubtype, stemV;
    GfxFont *gfont;
    fd_entry *fd;
    fm_entry *fontmap;
    // Check whether the font has already been embedded before analysing it.
    InObj *p;
    Ref ref = fontRef->getRef();
    for (p = inObjList; p; p = p->next) {
        if (p->ref.num == ref.num && p->ref.gen == ref.gen) {
            copyName(tag);
            pdf_printf(" %d 0 R ", (int) p->num);
            return;
        }
    }
    // Only handle included Type1 (and Type1C) fonts; anything else will be copied.
    // Type1C fonts are replaced by Type1 fonts, if REPLACE_TYPE1C is true.
    if (!fixed_inclusion_copy_font && fontRef->fetch(xref, &fontdict)->isDict()
        && fontdict->dictLookup("Subtype", &subtype)->isName()
        && !strcmp(subtype->getName(), "Type1")
        && fontdict->dictLookup("BaseFont", &basefont)->isName()
        && fontdict->dictLookupNF("FontDescriptor", &fontdescRef)->isRef()
        && fontdescRef->fetch(xref, &fontdesc)->isDict()
        && (fontdesc->dictLookup("FontFile", &fontfile)->isStream()
            || (REPLACE_TYPE1C
                && fontdesc->dictLookup("FontFile3", &fontfile)->isStream()
                && fontfile->streamGetDict()->lookup("Subtype",
                                                     &ffsubtype)->isName()
                && !strcmp(ffsubtype->getName(), "Type1C")))
        && (fontmap = lookup_fontmap(basefont->getName())) != NULL) {
        // copy the value of /StemV
        fontdesc->dictLookup("StemV", &stemV);
        fd = epdf_create_fontdescriptor(fontmap, stemV->getInt());
        if (fontdesc->dictLookup("CharSet", &charset) &&
            charset->isString() && is_subsetable(fontmap))
            epdf_mark_glyphs(fd, charset->getString()->getCString());
        else
            embed_whole_font(fd);
        addFontDesc(fontdescRef->getRef(), fd);
        copyName(tag);
        gfont = GfxFont::makeFont(xref, tag, fontRef->getRef(),
                                  fontdict->getDict());
        pdf_printf(" %d 0 R ", addFont(fontRef->getRef(), fd,
                                       addEncoding(gfont)));
    } else {
        copyName(tag);
        pdf_puts(" ");
        copyObject(fontRef);
    }
}

static void copyFontResources(Object * obj)
{
    PdfObject fontRef;
    int i, l;
    if (!obj->isDict())
        pdftex_fail("PDF inclusion: invalid font resources dict type <%s>",
                    obj->getTypeName());
    pdf_puts("/Font << ");
    for (i = 0, l = obj->dictGetLength(); i < l; ++i) {
        obj->dictGetValNF(i, &fontRef);
        if (fontRef->isRef())
            copyFont(obj->dictGetKey(i), &fontRef);
        else
            pdftex_fail("PDF inclusion: invalid font in reference type <%s>",
                        fontRef->getTypeName());
    }
    pdf_puts(">>\n");
}

static void copyOtherResources(Object * obj, char *key)
{
    // copies all other resources (write_epdf handles Fonts and ProcSets),
    // but gives a warning if an object is not a dictionary.

    if (!obj->isDict())
        //FIXME: Write the message only to the log file
        pdftex_warn("PDF inclusion: invalid other resource which is no dict"
                    " (key '%s', type <%s>); copying it anyway.",
                    key, obj->getTypeName());
    copyName(key);
    pdf_puts(" ");
    copyObject(obj);
}

// Function onverts double to string; very small and very large numbers
// are NOT converted to scientific notation.
// n must be a number or real conforming to the implementation limits
// of PDF as specified in appendix C.1 of the PDF Ref.
// These are:
// maximum value of ints is +2^32
// maximum value of reals is +2^15
// smalles values of reals is 1/(2^16)

static char *convertNumToPDF(double n)
{
    static const int precision = 6;
    static const int fact = (int) 1E6;  // must be 10^precision
    static const double epsilon = 0.5E-6;       // 2epsilon must be 10^-precision
    static char buf[64];
    // handle very small values: return 0
    if (fabs(n) < epsilon) {
        buf[0] = '0';
        buf[1] = '\0';
    } else {
        char ints[64];
        int bindex = 0, sindex = 0;
        int ival, fval;
        // handle the sign part if n is negative
        if (n < 0) {
            buf[bindex++] = '-';
            n = -n;
        }
        n += epsilon;           // for rounding
        // handle the integer part, simply with sprintf
        ival = (int) floor(n);
        n -= ival;
        sprintf(ints, "%d", ival);
        while (ints[sindex] != 0)
            buf[bindex++] = ints[sindex++];
        // handle the fractional part up to 'precision' digits
        fval = (int) floor(n * fact);
        if (fval) {
            // set a dot
            buf[bindex++] = '.';
            sindex = bindex + precision;
            buf[sindex--] = '\0';
            // fill up trailing zeros with the string terminator NULL
            while (((fval % 10) == 0) && (sindex >= bindex)) {
                buf[sindex--] = '\0';
                fval /= 10;
            }
            // fill up the fractional part back to front
            while (sindex >= bindex) {
                buf[sindex--] = (fval % 10) + '0';
                fval /= 10;
            }
        } else
            buf[bindex++] = '\0';
    }
    return (char *) buf;
}

static void copyObject(Object * obj)
{
    PdfObject obj1;
    int i, l, c;
    Ref ref;
    char *p;
    GString *s;
    if (obj->isBool()) {
        pdf_printf("%s", obj->getBool()? "true" : "false");
    } else if (obj->isInt()) {
        pdf_printf("%i", obj->getInt());
    } else if (obj->isReal()) {
        pdf_printf("%s", convertNumToPDF(obj->getReal()));
    } else if (obj->isNum()) {
        pdf_printf("%s", convertNumToPDF(obj->getNum()));
    } else if (obj->isString()) {
        s = obj->getString();
        p = s->getCString();
        l = s->getLength();
        if (strlen(p) == (unsigned int) l) {
            pdf_puts("(");
            for (; *p != 0; p++) {
                c = (unsigned char) *p;
                if (c == '(' || c == ')' || c == '\\')
                    pdf_printf("\\%c", c);
                else if (c < 0x20 || c > 0x7F)
                    pdf_printf("\\%03o", c);
                else
                    pdfout(c);
            }
            pdf_puts(")");
        } else {
            pdf_puts("<");
            for (i = 0; i < l; i++) {
                c = s->getChar(i) & 0xFF;
                pdf_printf("%.2x", c);
            }
            pdf_puts(">");
        }
    } else if (obj->isName()) {
        copyName(obj->getName());
    } else if (obj->isNull()) {
        pdf_puts("null");
    } else if (obj->isArray()) {
        pdf_puts("[");
        for (i = 0, l = obj->arrayGetLength(); i < l; ++i) {
            obj->arrayGetNF(i, &obj1);
            if (!obj1->isName())
                pdf_puts(" ");
            copyObject(&obj1);
        }
        pdf_puts("]");
    } else if (obj->isDict()) {
        pdf_puts("<<\n");
        copyDict(obj);
        pdf_puts(">>");
    } else if (obj->isStream()) {
        initDictFromDict(obj1, obj->streamGetDict());
        obj->streamGetDict()->incRef();
        pdf_puts("<<\n");
        copyDict(&obj1);
        pdf_puts(">>\n");
        pdf_puts("stream\n");
        copyStream(obj->getStream()->getUndecodedStream());
        if (pdf_last_byte != '\n')
            pdf_puts("\n");
        pdf_puts("endstream");  // can't simply write pdf_end_stream()
    } else if (obj->isRef()) {
        ref = obj->getRef();
        if (ref.num == 0) {
            pdftex_fail
                ("PDF inclusion: reference to invalid object"
                 " (is the included pdf broken?)");
        } else
            pdf_printf("%d 0 R", addOther(ref));
    } else {
        pdftex_fail("PDF inclusion: type <%s> cannot be copied",
                    obj->getTypeName());
    }
}

static void writeRefs()
{
    InObj *r;
    for (r = inObjList; r != NULL; r = r->next) {
        if (!r->written) {
            Object obj1;
            r->written = 1;
            xref->fetch(r->ref.num, r->ref.gen, &obj1);
            if (r->type == objFont) {
                assert(!obj1.isStream());
                zpdf_begin_obj(r->num, 2);      // \pdfobjcompresslevel = 2 is for this
                copyFontDict(&obj1, r);
                pdf_puts("\n");
                pdf_end_obj();
            } else if (r->type != objFontDesc) {        // /FontDescriptor is written via write_fontdescriptor()
                if (obj1.isStream())
                    zpdf_begin_obj(r->num, 0);
                else
                    zpdf_begin_obj(r->num, 2);  // \pdfobjcompresslevel = 2 is for this
                copyObject(&obj1);
                pdf_puts("\n");
                pdf_end_obj();
            }
            obj1.free();
        }
    }
}

static void writeEncodings()
{
    UsedEncoding *r, *n;
    char *glyphNames[256], *s;
    int i;
    for (r = encodingList; r != NULL; r = r->next) {
        for (i = 0; i < 256; i++) {
            if (r->font->isCIDFont()) {
                pdftex_fail
                    ("PDF inclusion: CID fonts are not supported"
                     " (try to disable font replacement to fix this)");
            }
            if ((s = ((Gfx8BitFont *) r->font)->getCharName(i)) != NULL)
                glyphNames[i] = s;
            else
                glyphNames[i] = notdef;
        }
        epdf_write_enc(glyphNames, r->enc_objnum);
    }
    for (r = encodingList; r != NULL; r = n) {
        n = r->next;
        delete r->font;
        delete r;
    }
}

// get the pagebox according to the pagebox_spec

static PDFRectangle *get_pagebox(Page * page, integer pagebox_spec)
{
    if (pagebox_spec == pdf_box_spec_media)
        return page->getMediaBox();
    else if (pagebox_spec == pdf_box_spec_crop)
        return page->getCropBox();
    else if (pagebox_spec == pdf_box_spec_bleed)
        return page->getBleedBox();
    else if (pagebox_spec == pdf_box_spec_trim)
        return page->getTrimBox();
    else if (pagebox_spec == pdf_box_spec_art)
        return page->getArtBox();
    else
        pdftex_fail("PDF inclusion: unknown value of pagebox spec (%i)",
                    (int) pagebox_spec);
    return page->getMediaBox(); // to make the compiler happy
}

// Reads various information about the PDF and sets it up for later inclusion.
// This will fail if the PDF version of the PDF is higher than
// minor_pdf_version_wanted or page_name is given and can not be found.
// It makes no sense to give page_name _and_ page_num.
// Returns the page number.

void
read_pdf_info(image_dict * idict, integer minor_pdf_version_wanted,
              integer pdf_inclusion_errorlevel)
{
    PdfDocument *pdf_doc;
    Page *page;
    int rotate;
    PDFRectangle *pagebox;
    float pdf_version_found, pdf_version_wanted, xsize, ysize, xorig, yorig;
    assert(idict != NULL);
    assert(img_type(idict) == IMG_TYPE_PDF);
    // initialize
    if (isInit == gFalse) {
        globalParams = new GlobalParams();
        globalParams->setErrQuiet(gFalse);
        isInit = gTrue;
    }
    // open PDF file
    pdf_doc = refPdfDocument(img_filepath(idict));
    // check PDF version
    // this works only for PDF 1.x -- but since any versions of PDF newer
    // than 1.x will not be backwards compatible to PDF 1.x, pdfTeX will
    // then have to changed drastically anyway.
    pdf_version_found = pdf_doc->doc->getPDFVersion();
    pdf_version_wanted = 1 + (minor_pdf_version_wanted * 0.1);
    if (pdf_version_found > pdf_version_wanted + 0.01) {
        char msg[] =
            "PDF inclusion: found PDF version <%.1f>, but at most version <%.1f> allowed";
        if (pdf_inclusion_errorlevel > 0) {
            pdftex_fail(msg, pdf_version_found, pdf_version_wanted);
        } else {
            pdftex_warn(msg, pdf_version_found, pdf_version_wanted);
        }
    }
    img_totalpages(idict) = pdf_doc->doc->getCatalog()->getNumPages();
    if (img_pagename(idict)) {
        // get page by name
        GString name(img_pagename(idict));
        LinkDest *link = pdf_doc->doc->findDest(&name);
        if (link == NULL || !link->isOk())
            pdftex_fail("PDF inclusion: invalid destination <%s>",
                        img_pagename(idict));
        Ref ref = link->getPageRef();
        img_pagenum(idict) =
            pdf_doc->doc->getCatalog()->findPage(ref.num, ref.gen);
        if (img_pagenum(idict) == 0)
            pdftex_fail("PDF inclusion: destination is not a page <%s>",
                        img_pagename(idict));
        delete link;
    } else {
        // get page by number
        if (img_pagenum(idict) <= 0
            || img_pagenum(idict) > img_totalpages(idict))
            pdftex_fail("PDF inclusion: required page <%i> does not exist",
                        (int) img_pagenum(idict));
    }
    pdf_doc->xref = pdf_doc->doc->getXRef();
    // get the required page
    page = pdf_doc->doc->getCatalog()->getPage(img_pagenum(idict));

    // get the pagebox (media, crop...) to use.
    pagebox = get_pagebox(page, img_pagebox(idict));
    if (pagebox->x2 > pagebox->x1) {
        xorig = pagebox->x1;
        xsize = pagebox->x2 - pagebox->x1;
    } else {
        xorig = pagebox->x2;
        xsize = pagebox->x1 - pagebox->x2;
    }
    if (pagebox->y2 > pagebox->y1) {
        yorig = pagebox->y1;
        ysize = pagebox->y2 - pagebox->y1;
    } else {
        yorig = pagebox->y2;
        ysize = pagebox->y1 - pagebox->y2;
    }
    // The following 4 parameters are raw. Do _not_ modify by /Rotate!
    img_xsize(idict) = bp2int(xsize);
    img_ysize(idict) = bp2int(ysize);
    img_xorig(idict) = bp2int(xorig);
    img_yorig(idict) = bp2int(yorig);

    // Handle /Rotate parameter. Only multiples of 90 deg. are allowed
    // (PDF Ref. v1.3, p. 78).
    rotate = page->getRotate();
    switch (((rotate % 360) + 360) % 360) {     // handles also neg. angles
    case 0:
        img_rotation(idict) = 0;
        break;
    case 90:
        img_rotation(idict) = 3;        // PDF counts clockwise!
        break;
    case 180:
        img_rotation(idict) = 2;
        break;
    case 270:
        img_rotation(idict) = 1;
        break;
    default:
        pdftex_warn
            ("PDF inclusion: "
             "/Rotate parameter in PDF file not multiple of 90 degrees.");
    }
    if (page->getGroup() != NULL) {
        initDictFromDict(lastGroup, page->getGroup());
        if (lastGroup->dictGetLength() > 0) {
            groupIsIndirect = lastGroup->isRef();
            if (groupIsIndirect) {
                // FIXME: Here we already copy the object. It would be
                // better to do this only after write_epdf, otherwise we
                // may copy ununsed /Group objects
                copyObject(&lastGroup);
                epdf_lastGroupObjectNum =
                    getNewObjectNumber(lastGroup->getRef());
            } else {
                // make the group an indirect object; copying is done later
                // by write_additional_epdf_objects after write_epdf
                epdf_lastGroupObjectNum = pdf_new_objnum();
            }
            pdf_puts("\n");
        }
    } else {
        epdf_lastGroupObjectNum = 0;
    }

}

// Writes the current epf_doc.
// Here the included PDF is copied, so most errors that can happen during PDF
// inclusion will arise here.

static void write_epdf1(image_dict * idict)
{
    Page *page;
    PdfObject contents, obj1, obj2;
    PdfObject metadata, pieceinfo, separationInfo;
    Object info;
    char *key;
    char s[256];
    int i, l;
    PdfDocument *pdf_doc = (PdfDocument *) findPdfDocument(img_filepath(idict));
    assert(pdf_doc != NULL);
    xref = pdf_doc->xref;
    inObjList = pdf_doc->inObjList;
    encodingList = NULL;
    page = pdf_doc->doc->getCatalog()->getPage(img_pagenum(idict));
    PDFRectangle *pagebox;
    float bbox[4];
    // write the Page header
    pdf_puts("/Type /XObject\n/Subtype /Form\n");
    if (img_attr(idict) != NULL && strlen(img_attr(idict)) > 0)
        pdf_printf("%s\n", img_attr(idict));
    pdf_puts("/FormType 1\n");

    // write additional information
    pdf_printf("/%s.FileName (%s)\n", pdfkeyprefix,
               convertStringToPDFString(pdf_doc->file_path,
                                        strlen(pdf_doc->file_path)));
    pdf_printf("/%s.PageNumber %i\n", pdfkeyprefix, (int) img_pagenum(idict));
    pdf_doc->doc->getDocInfoNF(&info);
    if (info.isRef()) {
        // the info dict must be indirect (PDF Ref p. 61)
        pdf_printf("/%s.InfoDict ", pdfkeyprefix);
        pdf_printf("%d 0 R\n", addOther(info.getRef()));
    }
    if (img_is_bbox(idict)) {
        bbox[0] = int2bp(img_bbox(idict)[0]);
        bbox[1] = int2bp(img_bbox(idict)[1]);
        bbox[2] = int2bp(img_bbox(idict)[2]);
        bbox[3] = int2bp(img_bbox(idict)[3]);
    } else {
        // get the pagebox (media, crop...) to use.
        pagebox = get_pagebox(page, img_pagebox(idict));
        bbox[0] = pagebox->x1;
        bbox[1] = pagebox->y1;
        bbox[2] = pagebox->x2;
        bbox[3] = pagebox->y2;
    }
    sprintf(s, "/BBox [%.8f %.8f %.8f %.8f]\n", bbox[0], bbox[1], bbox[2],
            bbox[3]);
    pdf_puts(stripzeros(s));
    // The /Matrix calculation is replaced by transforms in out_img().

    // write the page Group if it's there
    if (epdf_lastGroupObjectNum > 0) {
      if (page->getGroup() != NULL) {
        initDictFromDict(lastGroup, page->getGroup());
        if (lastGroup->dictGetLength() > 0) {
            pdf_puts("/Group ");
            groupIsIndirect = lastGroup->isRef();
            pdf_printf("%d 0 R", epdf_lastGroupObjectNum);
            pdf_puts("\n");
        }
      }  
    }
    // write the page Metadata if it's there
    if (page->getMetadata() != NULL) {
        metadata->initStream(page->getMetadata());
        pdf_puts("/Metadata ");
        copyObject(&metadata);
        pdf_puts("\n");
    }
    // write the page PieceInfo if it's there
    if (page->getPieceInfo() != NULL) {
        initDictFromDict(pieceinfo, page->getPieceInfo());
        if (pieceinfo->dictGetLength() > 0) {
            pdf_puts("/PieceInfo ");
            copyObject(&pieceinfo);
            pdf_puts("\n");
        }
    }
    // copy LastModified (needed when PieceInfo is there)
    if (page->getLastModified() != NULL) {
        pdf_printf("/LastModified (%s)\n",
                   page->getLastModified()->getCString());
    }
    // write the page SeparationInfo if it's there
    if (page->getSeparationInfo() != NULL) {
        initDictFromDict(separationInfo, page->getSeparationInfo());
        if (separationInfo->dictGetLength() > 0) {
            pdf_puts("/SeparationInfo ");
            copyObject(&separationInfo);
            pdf_puts("\n");
        }
    }
    // write the Resources dictionary
    if (page->getResourceDict() == NULL) {
        // Resources can be missing (files without them have been spotted
        // in the wild); in which case the /Resouces of the /Page will be used.
        // "This practice is not recommended".
        pdftex_warn
            ("PDF inclusion: /Resources missing. 'This practice is not recommended' (PDF Ref)");
    } else {
        initDictFromDict(obj1, page->getResourceDict());
        page->getResourceDict()->incRef();
        if (!obj1->isDict())
            pdftex_fail("PDF inclusion: invalid resources dict type <%s>",
                        obj1->getTypeName());
        pdf_puts("/Resources <<\n");
        for (i = 0, l = obj1->dictGetLength(); i < l; ++i) {
            obj1->dictGetVal(i, &obj2);
            key = obj1->dictGetKey(i);
            if (strcmp("Font", key) == 0)
                copyFontResources(&obj2);
            else if (strcmp("ProcSet", key) == 0)
                copyProcSet(&obj2);
            else
                copyOtherResources(&obj2, key);
        }
        pdf_puts(">>\n");
    }
    // write the page contents
    page->getContents(&contents);
    if (contents->isStream()) {
        // Variant A: get stream and recompress under control
        // of \pdfcompresslevel
        //
        // pdfbeginstream();
        // copyStream(contents->getStream());
        // pdfendstream();

        // Variant B: copy stream without recompressing
        //
        contents->streamGetDict()->lookup("F", &obj1);
        if (!obj1->isNull()) {
            pdftex_fail("PDF inclusion: Unsupported external stream");
        }
        contents->streamGetDict()->lookup("Length", &obj1);
        assert(!obj1->isNull());
        pdf_puts("/Length ");
        copyObject(&obj1);
        pdf_puts("\n");
        contents->streamGetDict()->lookup("Filter", &obj1);
        if (!obj1->isNull()) {
            pdf_puts("/Filter ");
            copyObject(&obj1);
            pdf_puts("\n");
            contents->streamGetDict()->lookup("DecodeParms", &obj1);
            if (!obj1->isNull()) {
                pdf_puts("/DecodeParms ");
                copyObject(&obj1);
                pdf_puts("\n");
            }
        }
        pdf_puts(">>\nstream\n");
        copyStream(contents->getStream()->getBaseStream());
        pdf_end_stream();
    } else if (contents->isArray()) {
        pdf_begin_stream();
        for (i = 0, l = contents->arrayGetLength(); i < l; ++i) {
            Object contentsobj;
            copyStream((contents->arrayGet(i, &contentsobj))->getStream());
            contentsobj.free();
        }
        pdf_end_stream();
    } else {                    // the contents are optional, but we need to include an empty stream
        pdf_begin_stream();
        pdf_end_stream();
    }
    // write out all indirect objects
    writeRefs();
    // write out all used encodings (and delete list)
    writeEncodings();
    // save object list
    pdf_doc->inObjList = inObjList;
    assert(xref == pdf_doc->xref);      // xref should be unchanged
}

void write_epdf(image_dict * idict)
{
    assert(idict != NULL);
    write_epdf1(idict);
    unrefPdfDocument(img_filepath(idict));
}

//**********************************************************************
// Deallocate a PdfDocument with all its resources

static void deletePdfDocument(PdfDocument * pdf_doc)
{
    assert(pdf_doc != NULL);
    // free PdfDocument's resources
    InObj *r, *n;
    for (r = pdf_doc->inObjList; r != NULL; r = n) {
        n = r->next;
        delete r;
    }
    delete pdf_doc->doc;
    xfree(pdf_doc->file_path);
    delete pdf_doc;
}

static void destroyPdfDocument(void *pa, void *pb)
{
    PdfDocument *pdf_doc = (PdfDocument *) pa;
    deletePdfDocument(pdf_doc);
}

// Called when PDF embedding system is finalized.
// Now deallocate all remaining PdfDocuments.

void epdf_check_mem()
{
    if (PdfDocumentTree != NULL)
        avl_destroy(PdfDocumentTree, destroyPdfDocument);
    PdfDocumentTree = NULL;
    if (isInit == gTrue)
        delete globalParams;
    isInit = gFalse;
}

// Called after the xobject generated by write_epdf has been finished; used to
// write out objects that have been made indirect
void write_additional_epdf_objects(void)
{
    if ((epdf_lastGroupObjectNum > 0) && !groupIsIndirect) {
        zpdf_begin_obj(epdf_lastGroupObjectNum, 2);
        copyObject(&lastGroup);
        pdf_end_obj();
    }
}
