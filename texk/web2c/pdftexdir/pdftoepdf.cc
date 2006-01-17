/*
Copyright 1996-2004 Han The Thanh, <thanh@pdftex.org>

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

$Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/pdftoepdf.cc#53 $
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

static const char perforce_id[] = 
    "$Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/pdftoepdf.cc#53 $";

/* we avoid reading all necessary kpathsea headers, but we need xstrdup */
#ifdef __cplusplus
extern "C" {
  extern KPSEDLL char *xstrdup (const char *);
}
#else
  extern KPSEDLL char *xstrdup (const char *);
#endif

#ifdef __cplusplus
extern "C" {
// FIXME: This definition is a duplicate from ../pdftexcoerce.h, which is
// not usable here because of different macro definitions.
extern integer pdflastpdfboxspec ;
}
#endif

/*
 * Used flags below:
 * PDFTEX_COPY_PAGEGROUP
 *      if pdfTeX should copy a page group (a new feature in pdf1.4 for
 *      transparency) of an included file. The current support for this is
 *      most likely broken. pdfTeX will at least give a warning if this flag
 *      is not set. Surprisingly Acrobat and Jaws display files without a
 *      page group correctly, so it might be safe to not set the flag.
 *  See also PDFTEX_COPY_PAGEGROUP_NOFAIL
 * PDFTEX_COPY_PAGEGROUP_NOFAIL
 *      if set to false pdfTeX will treat a page group in an included file as
 *      an error and abort gracefully. This is only evaluated if
 *      PDFTEX_COPY_PAGEGROUP is set.
 */

// FIXME: These definitions are duplicates from pdftexd.h
/* #define PDF_PDF_BOX_SPEC_MEDIA 0 */
/* #define PDF_PDF_BOX_SPEC_CROP  1 */
/* #define PDF_PDF_BOX_SPEC_BLEED 2 */
/* #define PDF_PDF_BOX_SPEC_TRIM  3 */
/* #define PDF_PDF_BOX_SPEC_ART   4 */
#define pdfpdfboxspecmedia ( 0 ) 
#define pdfpdfboxspeccrop ( 1 ) 
#define pdfpdfboxspecbleed ( 2 ) 
#define pdfpdfboxspectrim ( 3 ) 
#define pdfpdfboxspecart ( 4 ) 

// The prefix for the PDF keys special to pdfTeX
// This has been registered with Adobe by Hans Hagen
#define pdfkeyprefix "PTEX"

// PdfObject encapsulates the xpdf Object type, and properly 
// frees its resources on destruction.
// Use obj-> to access members of the Object, and
// &obj to get a pointer to the object. 
// It is no longer necessary to call Object::free explicitely.

class PdfObject {
public:
  PdfObject() { /* nothing */ }
  ~PdfObject() { iObject.free(); }
  Object* operator->() { return &iObject; }
  Object* operator&() { return &iObject; }
private: // no copying or assigning
  PdfObject(const PdfObject &);
  void operator=(const PdfObject &);
public:
  Object iObject;
};

// when copying the Resources of the selected page, all objects are copied
// recusively top-down. Indirect objects however are not fetched during
// copying, but get a new object number from pdftex and then will be
// appended into a linked list. Duplicates are checked and removed from the
// list of indirect objects during appending.

enum InObjType {
    objFont,
    objFontDesc,
    objOther
};

struct InObj {
    Ref ref;            // ref in original PDF
    InObjType type;     // object type
    InObj *next;        // next entry in list of indirect objects
    integer num;        // new object number in output PDF
    fm_entry * fontmap; // pointer to font map entry
    integer enc_objnum;   // Encoding for objFont      
    int written;        // has it been written to output PDF?
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
    int occurences;     // number of references to the document; the doc can be
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
        fprintf(stderr, "\nIncrementing %s (%d)\n", p->file_name, p->occurences);
#endif
        return p;
    }
    p = new PdfDocument;
    p->file_name = xstrdup(file_name);
    p->xref = xref = 0;
    p->occurences = 0;
#ifdef DEBUG
    fprintf(stderr, "\nCreating %s (%d)\n", p->file_name, p->occurences);
#endif
    GString *docName = new GString(p->file_name);
    p->doc = new PDFDoc(docName);  // takes ownership of docName
    if (!p->doc->isOk() || !p->doc->okToPrint()) {
        pdftex_fail("xpdf: reading PDF image failed");
        }
    p->inObjList = 0;
    p->next = pdfDocuments;
    pdfDocuments = p;
    return p;
}

// Deallocate a PdfDocument with all its resources
static void delete_document(PdfDocument *pdf_doc)
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
static void initDictFromDict (PdfObject &obj, Dict *dict)
{
    obj->initDict(xref);
    for (int i = 0,
         l = dict->getLength(); 
         i < l; 
         i++) {
        Object obj1;
        obj->dictAdd(copyString(dict->getKey(i)), dict->getValNF(i, &obj1));
        }
}    

// --------------------------------------------------------------------

static int addEncoding(GfxFont *gfont)
{
    UsedEncoding *n;
    n = new UsedEncoding;
    n->next = encodingList;
    encodingList = n;
    n->font = gfont;
    n->enc_objnum = pdfnewobjnum();
    return n->enc_objnum;
}

#define addFont(ref, fontmap, enc_objnum) \
        addInObj(objFont, ref, fontmap, enc_objnum)

#define addFontDesc(ref, fontmap) \
        addInObj(objFontDesc, ref, fontmap, 0)

#define addOther(ref) \
        addInObj(objOther, ref, 0, 0)

static int addInObj(InObjType type, Ref ref, fm_entry *f, integer e)
{
    InObj *p, *q, *n = new InObj;
    if (ref.num == 0)
        pdftex_fail("pdf inclusion: invalid reference");
    n->ref = ref;
    n->type = type;
    n->next = 0;
    n->fontmap = f;
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
    n->num = pdfnewobjnum();
    return n->num;
}

static void copyObject(Object *);

static void copyName(char *s)
{
    pdf_puts("/");
    for (; *s != 0; s++) {
        if (isdigit(*s) || isupper(*s) || islower(*s) || *s == '_' ||
        *s == '.' || *s == '-' )
            pdfout(*s);
        else
            pdf_printf("#%.2X", *s & 0xFF);
    }
}

static void copyDictEntry(Object *obj, int i)
{
    PdfObject obj1;
    copyName(obj->dictGetKey(i));
    pdf_puts(" ");
    obj->dictGetValNF(i, &obj1);
    copyObject(&obj1);
    pdf_puts("\n");
}

static void copyDict(Object *obj)
{
    int i, l;
    if (!obj->isDict())
        pdftex_fail("pdf inclusion: invalid dict type <%s>", 
                    obj->getTypeName());
    for (i = 0, l = obj->dictGetLength(); i < l; ++i)
        copyDictEntry(obj, i);
}

static void copyFontDict(Object *obj, InObj *r)
{
    int i, l;
    char *key;
    if (!obj->isDict())
        pdftex_fail("pdf inclusion: invalid dict type <%s>", 
                    obj->getTypeName());
    pdf_puts("<<\n");
    if (r->type == objFont) { // Font dict
        for (i = 0, l = obj->dictGetLength(); i < l; ++i) {
            key = obj->dictGetKey(i);
            if (strcmp("BaseFont", key) == 0 || 
                strcmp("Encoding", key) == 0)
                continue; // skip original values
            copyDictEntry(obj, i);
        }
        // write new BaseFont and Encoding
        pdf_printf("/BaseFont %i 0 R\n", (int)get_fontname(r->fontmap)); 
        pdf_printf("/Encoding %i 0 R\n", (int)r->enc_objnum); 
    }
    else { // FontDescriptor dict
        for (i = 0, l = obj->dictGetLength(); i < l; ++i) {
            key = obj->dictGetKey(i);
            if (strcmp("FontName", key) == 0 ||
                strncmp("FontFile", key, strlen("FontFile")) == 0)
                continue; // ignore original FontFile/FontName
            if (strcmp("CharSet", key) == 0)
                continue; // ignore CharSet
            copyDictEntry(obj, i);
        }
        // write new FontName and FontFile
        pdf_printf("/FontName %i 0 R\n", (int)get_fontname(r->fontmap)); 
        pdf_printf("/FontFile %i 0 R\n", (int)get_fontfile(r->fontmap));
    }
    pdf_puts(">>");
}

static void copyStream(Stream *str)
{
    int c;
    str->reset();
    while ((c = str->getChar()) != EOF)
        pdfout(c);
}

static void copyProcSet(Object *obj)
{
    int i, l;
    PdfObject procset;
    if (!obj->isArray())
        pdftex_fail("pdf inclusion: invalid ProcSet array type <%s>", 
                    obj->getTypeName());
    pdf_puts("/ProcSet [ ");
    for (i = 0, l = obj->arrayGetLength(); i < l; ++i) {
        obj->arrayGetNF(i, &procset);
        if (!procset->isName())
            pdftex_fail("pdf inclusion: invalid ProcSet entry type <%s>", 
                        procset->getTypeName());
        copyName(procset->getName());
        pdf_puts(" ");
    }
    pdf_puts("]\n");
}

static void copyFont(char *tag, Object *fontRef)
{
    PdfObject fontdict, subtype, basefont, fontdescRef, fontdesc, charset, fontfile;
    GfxFont *gfont;
    fm_entry *fontmap;
    // Check whether the font has already been embedded before analysing it.
    InObj *p;
    Ref ref = fontRef->getRef();
    for (p = inObjList; p; p = p->next) {
        if (p->ref.num == ref.num && p->ref.gen == ref.gen) {
            copyName(tag);
            pdf_printf(" %d 0 R ", p->num);
            return;
        }
    }
    /* only handle included Type1 fonts; anything else will be copied */
    if (fontRef->fetch(xref, &fontdict) &&
        fontdict->isDict() &&
        fontdict->dictLookup("Subtype", &subtype) &&
        subtype->isName() &&
        fontdict->dictLookup("BaseFont", &basefont) &&
        basefont->isName() &&
        fontdict->dictLookupNF("FontDescriptor", &fontdescRef) &&
        fontdescRef->isRef() &&
        fontdescRef->fetch(xref, &fontdesc) &&
        fontdesc->isDict() &&
        fontdesc->dictLookupNF("FontDescriptor", &fontdescRef) &&
        ((strcmp(subtype->getName(), "Type1") == 0 && 
          fontdesc->dictLookup("FontFile", &fontfile)) ||
         (strcmp(subtype->getName(), "Type1C") == 0 && 
          fontdesc->dictLookup("FontFile3", &fontfile))) &&
        fontfile->isStream()
       ) {
            fontmap = lookup_fontmap(basefont->getName());
            if (fontmap != NULL) {
                if (fontdesc->dictLookup("CharSet", &charset) && 
                    charset->isString() && is_subsetable(fontmap))
                    mark_glyphs(fontmap, charset->getString()->getCString());
                else
                    embed_whole_font(fontmap);
                addFontDesc(fontdescRef->getRef(), fontmap);
                copyName(tag);
                gfont = GfxFont::makeFont(xref, tag, fontRef->getRef(), 
                                          fontdict->getDict());
                pdf_printf(" %d 0 R ", addFont(fontRef->getRef(), fontmap,
                                               addEncoding(gfont)));
                return;
            }
    }
    copyName(tag);
    pdf_puts(" ");
    copyObject(fontRef);
}

static void copyFontResources(Object *obj)
{
    PdfObject fontRef;
    int i, l;
    if (!obj->isDict())
        pdftex_fail("pdf inclusion: invalid font resources dict type <%s>", 
                    obj->getTypeName());
    pdf_puts("/Font << ");
    for (i = 0, l = obj->dictGetLength(); i < l; ++i) {
        obj->dictGetValNF(i, &fontRef);
        if (fontRef->isRef())
            copyFont(obj->dictGetKey(i), &fontRef);
        else
            pdftex_fail("pdf inclusion: invalid font in reference type <%s>", 
                        fontRef->getTypeName());
    }
    pdf_puts(">>\n");
}

static void copyOtherResources(Object *obj, char *key)
{
    /* copies all other resources (write_epdf handles Fonts and ProcSets),
     * but gives a warning if an object is not a dictionary.
     */
    if (!obj->isDict())
        //FIXME: Write the message only to the log file
        pdftex_warn("pdf inclusion: invalid other resource which is no dict"
                    " (key '%s', type <%s>); copying it anyway.", 
                    key,
                    obj->getTypeName());
    copyName(key);
    pdf_puts(" ");
    copyObject(obj);
}

/* converts double to string; very small and very large numbers are NOT
 * converted to scientific notation.
 * n must be a number or real confirming to the implementation limits of PDF as
 * specified in appendix C.1 of the pdf ref.  
 * These are:
 * maximum value of ints is +2^32
 * maximum value of reals is +2^15
 * smalles values of reals is 1/(2^16)
 */
static char *
convertNumToPDF(double n) 
{
    static const int precision = 6;
    static const int fact = (int) 1E6;        /* must be 10^precision */
    static const double epsilon = 0.5E-6; /* 2epsilon must be 10^-precision */
    static char buf[64];
    // handle very small values: return 0
    if (fabs(n) < epsilon) {buf[0] = '0'; buf[1] = '\0';}
    else {
        char ints[64];
        int bindex = 0, sindex = 0;
        int ival, fval; 
        // handle the sign part if n is negative
        if (n < 0) {
            buf[bindex++] = '-';
            n = -n;    
        }
        n += epsilon; // for rounding
        // handle the integer part, simply with sprintf
        ival = (int)floor(n);
        n -= ival;
        sprintf(ints, "%d", ival);
        while (ints[sindex] != 0) buf[bindex++] = ints[sindex++];
        // handle the fractional part up to 'precision' digits
        fval = (int)floor(n*fact);
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
        } else buf[bindex++] = 0;
    }
    return (char *)buf;
}

static void copyObject(Object *obj)
{
    PdfObject obj1;
    int  i, l, c;
    Ref ref;
    char *p;
    GString *s;
    if (obj->isBool()) {
        pdf_printf("%s", obj->getBool() ? "true" : "false");
    }
    else if (obj->isInt()) {
        pdf_printf("%i", obj->getInt());
    }
    else if (obj->isReal()) {
        pdf_printf("%s", convertNumToPDF(obj->getReal()));
    }
    else if (obj->isNum()) {
        pdf_printf("%s", convertNumToPDF(obj->getNum()));
    }
    else if (obj->isString()) {
        s = obj->getString();
        p = s->getCString();
        l = s->getLength();
        if (strlen(p) == (unsigned int)l) {
            pdf_puts("(");
            for (; *p != 0; p++) {
                c = (unsigned char)*p;
                if (c == '(' || c == ')' || c == '\\')
                    pdf_printf("\\%c", c);
                else if (c < 0x20 || c > 0x7F)
                    pdf_printf("\\%03o", c);
                else
                    pdfout(c);
            }
            pdf_puts(")");
        }
        else {
            pdf_puts("<");
            for (i = 0; i < l; i++) {
                c = s->getChar(i) & 0xFF;
                pdf_printf("%.2x", c);
            }
            pdf_puts(">");
        }
    }
    else if (obj->isName()) {
        copyName(obj->getName());
    }
    else if (obj->isNull()) {
        pdf_puts("null");
    }
    else if (obj->isArray()) {
        pdf_puts("[");
        for (i = 0, l = obj->arrayGetLength(); i < l; ++i) {
            obj->arrayGetNF(i, &obj1);
            if (!obj1->isName())
                pdf_puts(" ");
            copyObject(&obj1);
        }
        pdf_puts("]");
    }
    else if (obj->isDict()) {
        pdf_puts("<<\n");
        copyDict(obj);
        pdf_puts(">>");
    }
    else if (obj->isStream()) {
        initDictFromDict (obj1, obj->streamGetDict());
        obj->streamGetDict()->incRef();
        pdf_puts("<<\n");
        copyDict(&obj1);
        pdf_puts(">>\n");
        pdf_puts("stream\n");
        copyStream(obj->getStream()->getBaseStream());
        pdf_puts("endstream");
    }
    else if (obj->isRef()) {
        ref = obj->getRef();
        if (ref.num == 0) {
            pdftex_warn("pdf inclusion: reference to invalid object was replaced by <null>");
            pdf_puts("null");
        }
        else
            pdf_printf("%d 0 R", addOther(ref));
    }
    else {
        pdftex_fail("pdf inclusion: type <%s> cannot be copied", 
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
        zpdfbeginobj(r->num);
        xref->fetch(r->ref.num, r->ref.gen, &obj1);
        if (r->type == objFont || r->type == objFontDesc)
            copyFontDict(&obj1, r);
        else
                copyObject(&obj1);
        pdf_puts("\n");
        pdfendobj();
        obj1.free();
    }
    }
}

static void writeEncodings()
{
    UsedEncoding *r, *n;
    char *glyphNames[MAX_CHAR_CODE + 1], *s;
    int i;
    for (r = encodingList; r != 0; r = r->next) {
      for (i = 0; i <= MAX_CHAR_CODE; i++) {
      if (r->font->isCIDFont()) {
        pdftex_warn("pdf inclusion: CID font included, encoding maybe wrong");
      }
      if ((s = ((Gfx8BitFont *)r->font)->getCharName(i)) != 0)
        glyphNames[i] = s;
      else
        glyphNames[i] = notdef;
      }
        write_enc(glyphNames, NULL, r->enc_objnum);
    }
    for (r = encodingList; r != 0; r = n) {
        n = r->next;
        delete r->font;
        delete r;
    }
}

/* Reads various information about the pdf and sets it up for later inclusion.
 * This will fail if the pdf version of the pdf is higher than
 * minor_pdf_version_wanted or page_name is given and can not be found.
 * It makes no sense to give page_name _and_ page_num.
 * Returns the page number.
 */
integer 
read_pdf_info(char *image_name, char *page_name, integer page_num,
              integer minor_pdf_version_wanted, integer always_use_pdf_pagebox,
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
#ifdef DEBUG
    fprintf(stderr, "\nReading information on %s\n", pdf_doc->file_name);
#endif
    // check pdf version
    // this works only for pdf 1.x -- but since any versions of pdf newer
    // than 1.x will not be backwards compatible to pdf 1.x, pdfTeX will
    // then have to changed drastically anyway.
    pdf_version_found = pdf_doc->doc->getPDFVersion();
    pdf_version_wanted = 1 + (minor_pdf_version_wanted * 0.1);
    if (pdf_version_found > pdf_version_wanted) {
        char msg[] = "pdf inclusion: found pdf version <%.1f>, but at most version <%.1f> allowed";
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
            pdftex_fail("pdf inclusion: invalid destination <%s>",
                page_name);
        Ref ref = link->getPageRef();
        page_num = pdf_doc->doc->getCatalog()->findPage(ref.num, ref.gen);
        if (page_num == 0)
            pdftex_fail("pdf inclusion: destination is not a page <%s>",
                page_name);
        delete link;
    } else {
        // get page by number
        if (page_num <= 0 || page_num > epdf_num_pages)
        pdftex_fail("pdf inclusion: required page does not exist <%i>", 
            epdf_num_pages);
    }
    // get the required page
    page = pdf_doc->doc->getCatalog()->getPage(page_num);

    // get the pagebox (media, crop...) to use.
    // always_use_pdf_pagebox can set in the config file to override the
    // setting through pdfximage.
    if (always_use_pdf_pagebox < 1) {
        switch (pdflastpdfboxspec) {
        case pdfpdfboxspeccrop:
            pagebox = page->getCropBox();
            break;
        
        case pdfpdfboxspecbleed:
            pagebox = page->getBleedBox();
            break;
        
        case pdfpdfboxspectrim:
            pagebox = page->getTrimBox();
            break;
        
        case pdfpdfboxspecart:
            pagebox = page->getArtBox();
            break;

        default:
            pagebox = page->getMediaBox();
            }
        }
    else {
        switch (always_use_pdf_pagebox) {
        case 1 : 
            pagebox = page->getMediaBox();
            break;
        case 2 : 
            pagebox = page->getCropBox();
            break;
        case 3 : 
            pagebox = page->getBleedBox();
            break;
        case 4 : 
            pagebox = page->getTrimBox();
            break;
        default : // 5 and larger
            pagebox = page->getArtBox();
            }
        }
#ifdef DEBUG
    fprintf(stderr, 
            "\npagebox->x1: %.8f, pagebox->x2: %.8f, pagebox->y1: %.8f, pagebox->y2: %.8f\n", 
            pagebox->x1, pagebox->x2, pagebox->y1, pagebox->y2);
#endif
    epdf_width = pagebox->x2 - pagebox->x1;
    epdf_height = pagebox->y2 - pagebox->y1;
    epdf_orig_x = pagebox->x1;
    epdf_orig_y = pagebox->y1;
    
    rotate = page->getRotate();
    // handle page rotation and adjust dimens as needed
    if (rotate != 0) {
        if (rotate % 90 == 0) {
            // handle only the simple case: multiple of 90s.
            // these are the only values allowed according to the
            // reference (v1.3, p.78).
            // 180 needs no special treatment here
            register float f;
            switch (rotate) {
                case  90: f = epdf_height; epdf_height = epdf_width; epdf_width = f;  break;
                case 270: f = epdf_height; epdf_height = epdf_width; epdf_width = f;  break;
                }
            }
        }
    pdf_doc->xref = pdf_doc->doc->getXRef();
    return page_num;
}

/* writes the current epf_doc.
 * Here the included pdf is copied, so most errors that can happen during pdf
 * inclusion will arise here.
 */
void 
write_epdf(void)
{
    Page *page;
    PdfObject contents, obj1, obj2;
    PdfObject group, metadata, pieceinfo, separationInfo;
    Object info;
    char *key;
    int i, l;
    int rotate;
    double scale[6] = {0, 0, 0, 0, 0, 0};
    bool writematrix = false;
    PdfDocument *pdf_doc = (PdfDocument *) epdf_doc;
    (pdf_doc->occurences)--;
#ifdef DEBUG
    fprintf(stderr, "\nDecrementing %s (%d)\n", pdf_doc->file_name, pdf_doc->occurences);
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
    pdf_printf("/%s.PageNumber %i\n", pdfkeyprefix, epdf_selected_page);
    pdf_doc->doc->getDocInfoNF(&info);
    if (info.isRef()) {
        // the info dict must be indirect (pdf ref p.61)
        pdf_printf("/%s.InfoDict ", pdfkeyprefix);
        pdf_printf("%d 0 R \n", addOther(info.getRef()));
        }
  
    // get the pagebox (media, crop...) to use.
    // epdf_always_use_pdf_pagebox is a copy of always_use_pdf_pagebox which
    // can set in the config file to override the setting through pdfximage.
    if (epdf_always_use_pdf_pagebox < 1) {
        switch (epdf_page_box) {
        case pdfpdfboxspeccrop:
            pagebox = page->getCropBox();
            break;
      
        case pdfpdfboxspecbleed:
            pagebox = page->getBleedBox();
            break;
        
        case pdfpdfboxspectrim:
            pagebox = page->getTrimBox();
            break;
        
        case pdfpdfboxspecart:
            pagebox = page->getArtBox();
            break;
       
        default:
            pagebox = page->getMediaBox();
            }
        }
    else {
        switch (epdf_always_use_pdf_pagebox) {
        case 1 : 
            pagebox = page->getMediaBox();
            break;
        case 2 : 
            pagebox = page->getCropBox();
            break;
        case 3 : 
            pagebox = page->getBleedBox();
            break;
        case 4 : 
            pagebox = page->getTrimBox();
            break;
        default : // 5 and larger
            pagebox = page->getArtBox();
            }
        }
#ifdef DEBUG
    fprintf(stderr, 
            "\npagebox->x1: %.8f, pagebox->x2: %.8f, pagebox->y1: %.8f, pagebox->y2: %.8f\n", 
            pagebox->x1, pagebox->x2, pagebox->y1, pagebox->y2);
#endif

    // handle page rotation
    if (rotate != 0) {
        if (rotate % 90 == 0) {
            // this handles only the simple case: multiple of 90s but these
            // are the only values allowed according to the reference
            // (v1.3, p.78).
            // the image is rotated around its center.
            // the /Rotate key is clockwise while the matrix is
            // counterclockwise :-%
            tex_printf (", page is rotated %d degrees", rotate);
            switch (rotate) {
                case  90: scale[1] = -1; scale[2] = 1; scale[4] = pagebox->x1 - pagebox->y1; scale[5] = pagebox->y1 + pagebox->x2; writematrix = true; break;
                case 180: scale[0] = scale[3] = -1;    scale[4] = pagebox->x1 + pagebox->x2; scale[5] = pagebox->y1 + pagebox->y2; writematrix = true; break; // width and height are exchanged
                case 270: scale[1] = 1; scale[2] = -1; scale[4] = pagebox->x1 + pagebox->y2; scale[5] = pagebox->y1 - pagebox->x1; writematrix = true; break;
                }
            if (writematrix) { // The matrix is only written if the image is rotated.
                pdf_printf("/Matrix [%.8f %.8f %.8f %.8f %.8f %.8f]\n",
                    scale[0],
                    scale[1],
                    scale[2],
                    scale[3],
                    scale[4],
                    scale[5]);
                }
            }
        }

    pdf_printf("/BBox [%.8f %.8f %.8f %.8f]\n",
               pagebox->x1,
               pagebox->y1,
               pagebox->x2,
               pagebox->y2);

    // write the page Group if it's there
    if (page->getGroup() != NULL) {
#if PDFTEX_COPY_PAGEGROUP
#   if PDFTEX_COPY_PAGEGROUP_NOFAIL
        // FIXME: This will most likely produce incorrect PDFs :-(
        initDictFromDict(group, page->getGroup());
        if (group->dictGetLength() > 0) {
            pdf_puts("/Group ");
            copyObject (&group);
            pdf_puts("\n");
        }
#   else
        // FIXME: currently we don't know how to handle Page Groups so we abort gracefully :-(
        pdftex_fail("pdf inclusion: Page Group detected which pdfTeX can't handle. Sorry.");
#   endif
#else
        // FIXME: currently we don't know how to handle Page Groups so we at least give a warning :-(
        pdftex_warn("pdf inclusion: Page Group detected which pdfTeX can't handle. Ignoring it.");
#endif
    }
    
    // write the page Metadata if it's there
    if (page->getMetadata() != NULL) {
        metadata->initStream(page->getMetadata());
        pdf_puts("/Metadata ");
        copyObject (&metadata);
        pdf_puts("\n");
    }
    
    // write the page PieceInfo if it's there
    if (page->getPieceInfo() != NULL) {
        initDictFromDict (pieceinfo, page->getPieceInfo());
        if (pieceinfo->dictGetLength() > 0) {
            pdf_puts("/PieceInfo ");
            copyObject (&pieceinfo);
            pdf_puts("\n");
        }
    }
    
    // write the page SeparationInfo if it's there
    if (page->getSeparationInfo() != NULL) {
        initDictFromDict (separationInfo, page->getSeparationInfo());
        if (separationInfo->dictGetLength() > 0) {
            pdf_puts("/SeparationInfo ");
            copyObject (&separationInfo);
            pdf_puts("\n");
        }
    }
    
    // write the Resources dictionary
    if (page->getResourceDict() == NULL) {
        // Resources can be missing (files without them have been spotted
        // in the wild). This violates the pdf spec, which claims they are
        // required, but all RIPs accept them.  
        // We "replace" them with empty Resources.
        pdftex_warn("pdf inclusion: no /Resources detected. Replacing with empty /Resources.");
        pdf_puts("/Resources <<>>\n");
        }
    else {
        initDictFromDict (obj1, page->getResourceDict());
        page->getResourceDict()->incRef();
        if (!obj1->isDict())
            pdftex_fail("pdf inclusion: invalid resources dict type <%s>", 
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
        initDictFromDict (obj1, contents->streamGetDict());
        contents->streamGetDict()->incRef();
        copyDict(&obj1);
        pdf_puts(">>\nstream\n");
        copyStream(contents->getStream()->getBaseStream());
        pdf_puts("endstream\n");
        pdfendobj();
    }
    else if (contents->isArray()) {
        pdfbeginstream();
        for (i = 0, l = contents->arrayGetLength(); i < l; ++i) {
        Object contentsobj;
            copyStream((contents->arrayGet(i, &contentsobj))->getStream());
        contentsobj.free();
        }
        pdfendstream();
    }
    else {// the contents are optional, but we need to include an empty stream
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
void 
epdf_delete()
{
    PdfDocument *pdf_doc = (PdfDocument *) epdf_doc;
    xref = pdf_doc->xref;
    if (pdf_doc->occurences < 0) {
#ifdef DEBUG
        fprintf(stderr, "\nDeleting %s\n", pdf_doc->file_name);
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
// vi:ts=4:tw=79:expandtab:ai:
