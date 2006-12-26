/*
Copyright 1996-2006 Han The Thanh, <thanh@pdftex.org>

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

$Id: pdftoepdf.cc,v 1.9 2006/09/01 18:06:52 hahe Exp hahe $
*/

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
#include <config.h>
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

// This file is mostly C and not very much C++; it's just used to interface
// the functions of xpdf, which happens to be written in C++.

// Used flags below:
//   PDFTEX_COPY_PAGEGROUP
//     If pdfTeX should copy a page group (a new feature in PDF 1.4 for
//     transparency) of an included file. The current support for this is
//     most likely broken. pdfTeX will at least give a warning if this flag
//     is not set. Surprisingly Acrobat and Jaws display files without a
//     page group correctly, so it might be safe to not set the flag.
//     See also PDFTEX_COPY_PAGEGROUP_NOFAIL.
//   PDFTEX_COPY_PAGEGROUP_NOFAIL
//     If set to false, pdfTeX will treat a page group in an included file
//     as an error and abort gracefully. This is only evaluated if
//     PDFTEX_COPY_PAGEGROUP is set.

// The prefix "PTEX" for the PDF keys is special to pdfTeX;
// this has been registered with Adobe by Hans Hagen.

#define pdfkeyprefix "PTEX"

// PdfObject encapsulates the xpdf Object type,
// and properly frees its resources on destruction.
// Use obj-> to access members of the Object,
// and &obj to get a pointer to the object.
// It is no longer necessary to call Object::free explicitely.

class PdfObject {
  public:
    PdfObject() {               // nothing
    } ~PdfObject() {
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

// When copying the Resources of the selected page, all objects are copied
// recusively top-down. Indirect objects however are not fetched during
// copying, but get a new object number from pdfTeX and then will be
// appended into a linked list. Duplicates are checked and removed from the
// list of indirect objects during appending.

enum InObjType {
    objFont,
    objFontDesc,
    objOther
};

struct InObj {
    Ref ref;                    // ref in original PDF
    InObjType type;             // object type
    InObj *next;                // next entry in list of indirect objects
    integer num;                // new object number in output PDF
    fd_entry *fd;               // pointer to /FontDescriptor object structure
    integer enc_objnum;         // Encoding for objFont
    int written;                // has it been written to output PDF?
};

struct UsedEncoding {
    integer enc_objnum;
    GfxFont *font;
    UsedEncoding *next;
};

static InObj *inObjList;
static UsedEncoding *encodingList;
static GBool isInit = gFalse;

// --------------------------------------------------------------------
// Maintain list of open embedded PDF files
// --------------------------------------------------------------------

struct PdfDocument {
    char *file_name;
    PDFDoc *doc;
    XRef *xref;
    InObj *inObjList;
    int occurences;             // number of references to the document; the doc can be
    // deleted when this is negative
    PdfDocument *next;
};

static PdfDocument *pdfDocuments = 0;

static XRef *xref = 0;

// Returns pointer to PdfDocument record for PDF file.
// Creates a new record if it doesn't exist yet.
// xref is made current for the document.

static PdfDocument *find_add_document(char *file_name)
{
    PdfDocument *p = pdfDocuments;
    while (p && strcmp(p->file_name, file_name) != 0)
        p = p->next;
    if (p) {
        xref = p->xref;
        (p->occurences)++;
#ifdef DEBUG
        fprintf(stderr, "\npdfTeX Debug: Incrementing %s (%d)\n", p->file_name,
                p->occurences);
#endif
        return p;
    }
    p = new PdfDocument;
    p->file_name = xstrdup(file_name);
    p->xref = xref = 0;
    p->occurences = 0;
#ifdef DEBUG
    fprintf(stderr, "\npdfTeX Debug: Creating %s (%d)\n", p->file_name,
            p->occurences);
#endif
    GString *docName = new GString(p->file_name);
    p->doc = new PDFDoc(docName);       // takes ownership of docName
    if (!p->doc->isOk() || !p->doc->okToPrint()) {
        pdftex_fail("xpdf: reading PDF image failed");
    }
    p->inObjList = 0;
    p->next = pdfDocuments;
    pdfDocuments = p;
    return p;
}

// Deallocate a PdfDocument with all its resources

static void delete_document(PdfDocument * pdf_doc)
{
    PdfDocument **p = &pdfDocuments;
    while (*p && *p != pdf_doc)
        p = &((*p)->next);
    // should not happen:
    if (!*p)
        return;
    // unlink from list
    *p = pdf_doc->next;
    // free pdf_doc's resources
    InObj *r, *n;
    for (r = pdf_doc->inObjList; r != 0; r = n) {
        n = r->next;
        delete r;
    }
    xref = pdf_doc->xref;
    delete pdf_doc->doc;
    xfree(pdf_doc->file_name);
    delete pdf_doc;
}

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

// --------------------------------------------------------------------

static int addEncoding(GfxFont * gfont)
{
    UsedEncoding *n;
    n = new UsedEncoding;
    n->next = encodingList;
    encodingList = n;
    n->font = gfont;
    n->enc_objnum = pdfnewobjnum();
    return n->enc_objnum;
}

#define addFont(ref, fd, enc_objnum) \
        addInObj(objFont, ref, fd, enc_objnum)

// addFontDesc is only used to avoid writing the original FontDescriptor
// from the PDF file.

#define addFontDesc(ref, fd) \
        addInObj(objFontDesc, ref, fd, 0)

#define addOther(ref) \
        addInObj(objOther, ref, 0, 0)

static int addInObj(InObjType type, Ref ref, fd_entry * fd, integer e)
{
    InObj *p, *q, *n = new InObj;
    if (ref.num == 0)
        pdftex_fail("PDF inclusion: invalid reference");
    n->ref = ref;
    n->type = type;
    n->next = 0;
    n->fd = fd;
    n->enc_objnum = e;
    n->written = 0;
    if (inObjList == 0)
        inObjList = n;
    else {
        for (p = inObjList; p != 0; p = p->next) {
            if (p->ref.num == ref.num && p->ref.gen == ref.gen) {
                delete n;
                return p->num;
            }
            q = p;
        }
        // it is important to add new objects at the end of the list,
        // because new objects are being added while the list is being
        // written out.
        q->next = n;
    }
    if (type == objFontDesc)
        n->num = get_fd_objnum(fd);
    else
        n->num = pdfnewobjnum();
    return n->num;
}

static void copyObject(Object *);

static void copyName(char *s)
{
    pdf_puts("/");
    for (; *s != 0; s++) {
        if (isdigit(*s) || isupper(*s) || islower(*s) || *s == '_' ||
            *s == '.' || *s == '-')
            pdfout(*s);
        else
            pdf_printf("#%.2X", *s & 0xFF);
    }
}

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
    while ((c = str->getChar()) != EOF)
        pdfout(c);
    pdflastbyte = pdfbuf[pdfptr - 1];
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
        fontfile, ffsubtype;
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
    if (fontRef->fetch(xref, &fontdict)->isDict()
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
        fd = epdf_create_fontdescriptor(fontmap);
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
            buf[bindex++] = 0;
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
        copyStream(obj->getStream()->getBaseStream());
        if (pdflastbyte != '\n')
            pdf_puts("\n");
        pdf_puts("endstream");  // can't simply write pdfendstream()
    } else if (obj->isRef()) {
        ref = obj->getRef();
        if (ref.num == 0) {
            pdftex_warn
                ("PDF inclusion: reference to invalid object was replaced by <null>");
            pdf_puts("null");
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
    for (r = inObjList; r != 0; r = r->next) {
        if (!r->written) {
            Object obj1;
            r->written = 1;
            xref->fetch(r->ref.num, r->ref.gen, &obj1);
            if (r->type == objFont) {
                assert(!obj1.isStream());
                zpdfbeginobj(r->num, 2);        // \pdfobjcompresslevel = 2 is for this
                copyFontDict(&obj1, r);
                pdf_puts("\n");
                pdfendobj();
            } else if (r->type != objFontDesc) {        // /FontDescriptor is written via write_fontdescriptor()
                if (obj1.isStream())
                    zpdfbeginobj(r->num, 0);
                else
                    zpdfbeginobj(r->num, 2);    // \pdfobjcompresslevel = 2 is for this
                copyObject(&obj1);
                pdf_puts("\n");
                pdfendobj();
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
    for (r = encodingList; r != 0; r = r->next) {
        for (i = 0; i < 256; i++) {
            if (r->font->isCIDFont()) {
                pdftex_warn
                    ("PDF inclusion: CID font included, encoding maybe wrong");
            }
            if ((s = ((Gfx8BitFont *) r->font)->getCharName(i)) != 0)
                glyphNames[i] = s;
            else
                glyphNames[i] = notdef;
        }
        epdf_write_enc(glyphNames, r->enc_objnum);
    }
    for (r = encodingList; r != 0; r = n) {
        n = r->next;
        delete r->font;
        delete r;
    }
}

// get the pagebox according to the pagebox_spec
static PDFRectangle *get_pagebox(Page * page, integer pagebox_spec)
{
    if (pagebox_spec == pdfboxspecmedia)
        return page->getMediaBox();
    else if (pagebox_spec == pdfboxspeccrop)
        return page->getCropBox();
    else if (pagebox_spec == pdfboxspecbleed)
        return page->getBleedBox();
    else if (pagebox_spec == pdfboxspectrim)
        return page->getTrimBox();
    else if (pagebox_spec == pdfboxspecart)
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

integer
read_pdf_info(char *image_name, char *page_name, integer page_num,
              integer pagebox_spec, integer minor_pdf_version_wanted,
              integer pdf_inclusion_errorlevel)
{
    PdfDocument *pdf_doc;
    Page *page;
    int rotate;
    PDFRectangle *pagebox;
    float pdf_version_found, pdf_version_wanted;
    // initialize
    if (!isInit) {
        globalParams = new GlobalParams();
        globalParams->setErrQuiet(gFalse);
        isInit = gTrue;
    }
    // open PDF file
    pdf_doc = find_add_document(image_name);
    epdf_doc = (void *) pdf_doc;

    // check PDF version
    // this works only for PDF 1.x -- but since any versions of PDF newer
    // than 1.x will not be backwards compatible to PDF 1.x, pdfTeX will
    // then have to changed drastically anyway.
    pdf_version_found = pdf_doc->doc->getPDFVersion();
    pdf_version_wanted = 1 + (minor_pdf_version_wanted * 0.1);
    if (pdf_version_found > pdf_version_wanted) {
        char msg[] =
            "PDF inclusion: found PDF version <%.1f>, but at most version <%.1f> allowed";
        if (pdf_inclusion_errorlevel > 0) {
            pdftex_fail(msg, pdf_version_found, pdf_version_wanted);
        } else {
            pdftex_warn(msg, pdf_version_found, pdf_version_wanted);
        }
    }
    epdf_num_pages = pdf_doc->doc->getCatalog()->getNumPages();
    if (page_name) {
        // get page by name
        GString name(page_name);
        LinkDest *link = pdf_doc->doc->findDest(&name);
        if (link == 0 || !link->isOk())
            pdftex_fail("PDF inclusion: invalid destination <%s>", page_name);
        Ref ref = link->getPageRef();
        page_num = pdf_doc->doc->getCatalog()->findPage(ref.num, ref.gen);
        if (page_num == 0)
            pdftex_fail("PDF inclusion: destination is not a page <%s>",
                        page_name);
        delete link;
    } else {
        // get page by number
        if (page_num <= 0 || page_num > epdf_num_pages)
            pdftex_fail("PDF inclusion: required page does not exist <%i>",
                        (int) epdf_num_pages);
    }
    // get the required page
    page = pdf_doc->doc->getCatalog()->getPage(page_num);

    // get the pagebox (media, crop...) to use.
    pagebox = get_pagebox(page, pagebox_spec);
    if (pagebox->x2 > pagebox->x1) {
        epdf_orig_x = pagebox->x1;
        epdf_width = pagebox->x2 - pagebox->x1;
    } else {
        epdf_orig_x = pagebox->x2;
        epdf_width = pagebox->x1 - pagebox->x2;
    }
    if (pagebox->y2 > pagebox->y1) {
        epdf_orig_y = pagebox->y1;
        epdf_height = pagebox->y2 - pagebox->y1;
    } else {
        epdf_orig_y = pagebox->y2;
        epdf_height = pagebox->y1 - pagebox->y2;
    }

    rotate = page->getRotate();
    // handle page rotation and adjust dimens as needed
    if (rotate != 0) {
        if (rotate % 90 == 0) {
            // handle only the simple case: multiple of 90s.
            // these are the only values allowed according to the
            // reference (v1.3, p. 78).
            // 180 needs no special treatment here
            register float f;
            switch (rotate) {
            case 90:
                f = epdf_height;
                epdf_height = epdf_width;
                epdf_width = f;
                break;
            case 270:
                f = epdf_height;
                epdf_height = epdf_width;
                epdf_width = f;
                break;
            }
        }
    }
    pdf_doc->xref = pdf_doc->doc->getXRef();
    return page_num;
}

// writes the current epf_doc.
// Here the included PDF is copied, so most errors that can happen during PDF
// inclusion will arise here.

void write_epdf(void)
{
    Page *page;
    PdfObject contents, obj1, obj2;
    PdfObject group, metadata, pieceinfo, separationInfo;
    Object info;
    char *key;
    char s[256];
    int i, l;
    int rotate;
    double scale[6] = { 0, 0, 0, 0, 0, 0 };
    bool writematrix = false;
    PdfDocument *pdf_doc = (PdfDocument *) epdf_doc;
    (pdf_doc->occurences)--;
#ifdef DEBUG
    fprintf(stderr, "\npdfTeX Debug: Decrementing %s (%d)\n",
            pdf_doc->file_name, pdf_doc->occurences);
#endif
    xref = pdf_doc->xref;
    inObjList = pdf_doc->inObjList;
    encodingList = 0;
    page = pdf_doc->doc->getCatalog()->getPage(epdf_selected_page);
    rotate = page->getRotate();
    PDFRectangle *pagebox;
    // write the Page header
    pdf_puts("/Type /XObject\n");
    pdf_puts("/Subtype /Form\n");
    pdf_puts("/FormType 1\n");

    // write additional information
    pdf_printf("/%s.FileName (%s)\n", pdfkeyprefix,
               convertStringToPDFString(pdf_doc->file_name,
                                        strlen(pdf_doc->file_name)));
    pdf_printf("/%s.PageNumber %i\n", pdfkeyprefix, (int) epdf_selected_page);
    pdf_doc->doc->getDocInfoNF(&info);
    if (info.isRef()) {
        // the info dict must be indirect (PDF Ref p. 61)
        pdf_printf("/%s.InfoDict ", pdfkeyprefix);
        pdf_printf("%d 0 R\n", addOther(info.getRef()));
    }
    // get the pagebox (media, crop...) to use.
    pagebox = get_pagebox(page, epdf_page_box);

    // handle page rotation
    if (rotate != 0) {
        if (rotate % 90 == 0) {
            // this handles only the simple case: multiple of 90s but these
            // are the only values allowed according to the reference
            // (v1.3, p. 78).
            // the image is rotated around its center.
            // the /Rotate key is clockwise while the matrix is
            // counterclockwise :-%
            tex_printf(", page is rotated %d degrees", rotate);
            switch (rotate) {
            case 90:
                scale[1] = -1;
                scale[2] = 1;
                scale[4] = pagebox->x1 - pagebox->y1;
                scale[5] = pagebox->y1 + pagebox->x2;
                writematrix = true;
                break;
            case 180:
                scale[0] = scale[3] = -1;
                scale[4] = pagebox->x1 + pagebox->x2;
                scale[5] = pagebox->y1 + pagebox->y2;
                writematrix = true;
                break;          // width and height are exchanged
            case 270:
                scale[1] = 1;
                scale[2] = -1;
                scale[4] = pagebox->x1 + pagebox->y2;
                scale[5] = pagebox->y1 - pagebox->x1;
                writematrix = true;
                break;
            }
            if (writematrix) {  // The matrix is only written if the image is rotated.
                sprintf(s, "/Matrix [%.8f %.8f %.8f %.8f %.8f %.8f]\n",
                        scale[0],
                        scale[1], scale[2], scale[3], scale[4], scale[5]);
                pdf_printf(stripzeros(s));
            }
        }
    }

    sprintf(s, "/BBox [%.8f %.8f %.8f %.8f]\n",
            pagebox->x1, pagebox->y1, pagebox->x2, pagebox->y2);
    pdf_printf(stripzeros(s));

    // write the page Group if it's there
    if (page->getGroup() != NULL) {
#if PDFTEX_COPY_PAGEGROUP
#  if PDFTEX_COPY_PAGEGROUP_NOFAIL
        // FIXME: This will most likely produce incorrect PDFs :-(
        initDictFromDict(group, page->getGroup());
        if (group->dictGetLength() > 0) {
            pdf_puts("/Group ");
            copyObject(&group);
            pdf_puts("\n");
        }
#  else
        // FIXME: currently we don't know how to handle Page Groups so we abort gracefully :-(
        pdftex_fail
            ("PDF inclusion: Page Group detected which pdfTeX can't handle. Sorry.");
#  endif
#else
        // FIXME: currently we don't know how to handle Page Groups so we at least give a warning :-(
        pdftex_warn
            ("PDF inclusion: Page Group detected which pdfTeX can't handle. Ignoring it.");
#endif
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
        // in the wild). This violates the PDF Ref., which claims they are
        // required, but all RIPs accept them.
        // We "replace" them with empty /Resources, although in form xobjects
        // /Resources are not required.
        pdftex_warn
            ("PDF inclusion: no /Resources detected. Replacing with empty /Resources.");
        pdf_puts("/Resources <<>>\n");
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
        initDictFromDict(obj1, contents->streamGetDict());
        contents->streamGetDict()->incRef();
        copyDict(&obj1);
        pdf_puts(">>\nstream\n");
        copyStream(contents->getStream()->getBaseStream());
        pdfendstream();
    } else if (contents->isArray()) {
        pdfbeginstream();
        for (i = 0, l = contents->arrayGetLength(); i < l; ++i) {
            Object contentsobj;
            copyStream((contents->arrayGet(i, &contentsobj))->getStream());
            contentsobj.free();
        }
        pdfendstream();
    } else {                    // the contents are optional, but we need to include an empty stream
        pdfbeginstream();
        pdfendstream();
    }
    // write out all indirect objects
    writeRefs();
    // write out all used encodings (and delete list)
    writeEncodings();
    // save object list, xref
    pdf_doc->inObjList = inObjList;
    pdf_doc->xref = xref;
}

// Called when an image has been written and it's resources in image_tab are
// freed and it's not referenced anymore.

void epdf_delete()
{
    PdfDocument *pdf_doc = (PdfDocument *) epdf_doc;
    xref = pdf_doc->xref;
    if (pdf_doc->occurences < 0) {
#ifdef DEBUG
        fprintf(stderr, "\npdfTeX Debug: Deleting %s\n", pdf_doc->file_name);
#endif
        delete_document(pdf_doc);
    }
}

// Called when PDF embedding system is finalized.
// Now deallocate all remaining PdfDocuments.

void epdf_check_mem()
{
    if (isInit) {
        PdfDocument *p, *n;
        for (p = pdfDocuments; p; p = n) {
            n = p->next;
            delete_document(p);
        }
        // see above for globalParams
        delete globalParams;
    }
}
