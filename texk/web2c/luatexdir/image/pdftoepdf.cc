/* pdftoepdf.cc

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

static const char _svn_version[] =
    "$Id: pdftoepdf.cc 3606 2010-04-10 12:58:25Z taco $ "
    "$URL: http://foundry.supelec.fr/svn/luatex/branches/0.60.x/source/texk/web2c/luatexdir/image/pdftoepdf.cc $";

#include "epdf.h"

#define one_hundred_bp  6578176 // one_hundred_bp = 7227 * 65536 / 72

// This file is mostly C and not very much C++; it's just used to interface
// the functions of xpdf, which happens to be written in C++.

// The prefix "PTEX" for the PDF keys is special to pdfTeX;
// this has been registered with Adobe by Hans Hagen.

#define pdfkeyprefix "PTEX"

static GBool isInit = gFalse;

//**********************************************************************
// Maintain AVL tree of all PDF files for embedding

static avl_table *PdfDocumentTree = NULL;

// AVL sort PdfDocument into PdfDocumentTree by file_path

static int CompPdfDocument(const void *pa, const void *pb, void * /*p */ )
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
        return NULL;
    tmp.file_path = file_path;
    pdf_doc = (PdfDocument *) avl_find(PdfDocumentTree, &tmp);
    return pdf_doc;
}

// Returns pointer to PdfDocument structure for PDF file.
// Creates a new structure if it doesn't exist yet.

char *get_file_checksum(char *a);

PdfDocument *refPdfDocument(char *file_path)
{
    PdfDocument *pdf_doc = findPdfDocument(file_path);
    if (pdf_doc == NULL) {
        if (PdfDocumentTree == NULL)
            PdfDocumentTree =
                avl_create(CompPdfDocument, NULL, &avl_xallocator);
        pdf_doc = new PdfDocument;
        pdf_doc->file_path = xstrdup(file_path);
        pdf_doc->checksum = get_file_checksum(file_path);
        void **aa = avl_probe(PdfDocumentTree, pdf_doc);
        assert(aa != NULL);
        pdf_doc->ObjMapTree = NULL;
        pdf_doc->doc = NULL;
    }
    if (pdf_doc->doc == NULL) {
        GString *docName = new GString(pdf_doc->file_path);
        pdf_doc->doc = new PDFDoc(docName);     // takes ownership of docName
        if (!pdf_doc->doc->isOk() || !pdf_doc->doc->okToPrint())
            pdftex_fail("xpdf: reading PDF image failed");
        pdf_doc->inObjList = NULL;
        pdf_doc->occurences = 0;        // 0 = unreferenced
    }
#ifdef DEBUG
    fprintf(stderr, "\nluatex Debug: Creating %s (%d)\n",
            pdf_doc->file_path, pdf_doc->occurences);
#endif
    pdf_doc->occurences++;
#ifdef DEBUG
    fprintf(stderr, "\nluatex Debug: Incrementing %s (%d)\n",
            pdf_doc->file_path, pdf_doc->occurences);
#endif
    return pdf_doc;
}

//**********************************************************************
// AVL sort ObjMap into ObjMapTree by object number and generation

// keep the ObjMap struct small, as these are accumulated until the end

struct ObjMap {
    Ref in;                     // object num/gen in orig. PDF file
    int out_num;                // object num after embedding (gen == 0)
};

static int CompObjMap(const void *pa, const void *pb, void * /*p */ )
{
    const Ref *a = &(((const ObjMap *) pa)->in);
    const Ref *b = &(((const ObjMap *) pb)->in);
    if (a->num > b->num)
        return 1;
    if (a->num < b->num)
        return -1;
    if (a->gen == b->gen)       // most likely gen == 0 anyway
        return 0;
    if (a->gen < b->gen)
        return -1;
    return 1;
}

static ObjMap *findObjMap(PdfDocument * pdf_doc, Ref in)
{
    ObjMap *obj_map, tmp;
    assert(pdf_doc != NULL);
    if (pdf_doc->ObjMapTree == NULL)
        return NULL;
    tmp.in = in;
    obj_map = (ObjMap *) avl_find(pdf_doc->ObjMapTree, &tmp);
    return obj_map;
}

static void addObjMap(PdfDocument * pdf_doc, Ref in, int out_num)
{
    ObjMap *obj_map = NULL;
    assert(findObjMap(pdf_doc, in) == NULL);
    if (pdf_doc->ObjMapTree == NULL)
        pdf_doc->ObjMapTree = avl_create(CompObjMap, NULL, &avl_xallocator);
    obj_map = new ObjMap;
    obj_map->in = in;
    obj_map->out_num = out_num;
    void **aa = avl_probe(pdf_doc->ObjMapTree, obj_map);
    assert(aa != NULL);
}

// When copying the Resources of the selected page, all objects are
// copied recursively top-down.  The findObjMap() function checks if an
// object has already been copied; if so, instead of copying just the
// new object number will be referenced.  The ObjMapTree guarantees,
// that during the entire LuaTeX run any object from any embedded PDF
// file will end up max. once in the output PDF file.  Indirect objects
// are not fetched during copying, but get a new object number from
// LuaTeX and then will be appended into a linked list.

static int addInObj(PDF pdf, PdfDocument * pdf_doc, Ref ref)
{
    ObjMap *obj_map;
    InObj *p, *q, *n;
    if (ref.num == 0) {
        pdftex_fail("PDF inclusion: reference to invalid object"
                    " (is the included pdf broken?)");
    }
    if ((obj_map = findObjMap(pdf_doc, ref)) != NULL)
        return obj_map->out_num;
    n = new InObj;
    n->ref = ref;
    n->next = NULL;
    n->num = pdf_new_objnum(pdf);
    addObjMap(pdf_doc, ref, n->num);
    if (pdf_doc->inObjList == NULL)
        pdf_doc->inObjList = n;
    else {
        // it is important to add new objects at the end of the list,
        // because new objects are being added while the list is being
        // written out by writeRefs().
        for (p = pdf_doc->inObjList; p != NULL; p = p->next)
            q = p;
        q->next = n;
    }
    return n->num;
}

//**********************************************************************
// Function converts double to string; very small and very large numbers
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

static void copyObject(PDF, PdfDocument *, Object *);

static void copyString(PDF pdf, GString * string)
{
    char *p;
    unsigned char c;
    size_t i, l;
    p = string->getCString();
    l = (size_t) string->getLength();
    if (strlen(p) == l) {
        pdf_puts(pdf, "(");
        for (; *p != 0; p++) {
            c = (unsigned char) *p;
            if (c == '(' || c == ')' || c == '\\')
                pdf_printf(pdf, "\\%c", c);
            else if (c < 0x20 || c > 0x7F)
                pdf_printf(pdf, "\\%03o", (int) c);
            else
                pdf_out(pdf, c);
        }
        pdf_puts(pdf, ")");
    } else {
        pdf_puts(pdf, "<");
        for (i = 0; i < l; i++) {
            c = (unsigned char) string->getChar(i);
            pdf_printf(pdf, "%.2x", (int) c);
        }
        pdf_puts(pdf, ">");
    }
}

static void copyName(PDF pdf, char *s)
{
    pdf_puts(pdf, "/");
    for (; *s != 0; s++) {
        if (isdigit(*s) || isupper(*s) || islower(*s) || *s == '_' ||
            *s == '.' || *s == '-' || *s == '+')
            pdf_out(pdf, *s);
        else
            pdf_printf(pdf, "#%.2X", *s & 0xFF);
    }
}

static void copyArray(PDF pdf, PdfDocument * pdf_doc, Array * array)
{
    int i, l;
    PdfObject obj1;
    pdf_puts(pdf, "[");
    for (i = 0, l = array->getLength(); i < l; ++i) {
        array->getNF(i, &obj1);
        if (!obj1->isName())
            pdf_puts(pdf, " ");
        copyObject(pdf, pdf_doc, &obj1);
    }
    pdf_puts(pdf, "]");
}

static void copyDict(PDF pdf, PdfDocument * pdf_doc, Dict * dict)
{
    int i, l;
    PdfObject obj1;
    pdf_puts(pdf, "<<");
    for (i = 0, l = dict->getLength(); i < l; ++i) {
        copyName(pdf, dict->getKey(i));
        pdf_puts(pdf, " ");
        dict->getValNF(i, &obj1);
        copyObject(pdf, pdf_doc, &obj1);
        pdf_puts(pdf, "\n");
    }
    pdf_puts(pdf, ">>");
}

static void copyStreamStream(PDF pdf, Stream * str)
{
    int c;
    str->reset();
    while ((c = str->getChar()) != EOF) {
        pdf_out(pdf, c);
        pdf->last_byte = c;
    }
}

static void copyStream(PDF pdf, PdfDocument * pdf_doc, Stream * stream)
{
    copyDict(pdf, pdf_doc, stream->getDict());
    pdf_puts(pdf, "stream\n");
    copyStreamStream(pdf, stream->getUndecodedStream());
    if (pdf->last_byte != '\n')
        pdf_puts(pdf, "\n");
    pdf_puts(pdf, "endstream"); // can't simply write pdf_end_stream()
}

static void copyObject(PDF pdf, PdfDocument * pdf_doc, Object * obj)
{
    switch (obj->getType()) {
    case objBool:
        pdf_printf(pdf, "%s", obj->getBool()? "true" : "false");
        break;
    case objInt:
        pdf_printf(pdf, "%i", obj->getInt());
        break;
    case objReal:
        pdf_printf(pdf, "%s", convertNumToPDF(obj->getReal()));
        break;
        // not needed:
        // GBool isNum() { return type == objInt || type == objReal; }
    case objString:
        copyString(pdf, obj->getString());
        break;
    case objName:
        copyName(pdf, obj->getName());
        break;
    case objNull:
        pdf_puts(pdf, "null");
        break;
    case objArray:
        copyArray(pdf, pdf_doc, obj->getArray());
        break;
    case objDict:
        copyDict(pdf, pdf_doc, obj->getDict());
        break;
    case objStream:
        copyStream(pdf, pdf_doc, obj->getStream());
        break;
    case objRef:
        pdf_printf(pdf, "%d 0 R", addInObj(pdf, pdf_doc, obj->getRef()));
        break;
    case objCmd:
    case objError:
    case objEOF:
    case objNone:
        pdftex_fail("PDF inclusion: type <%s> cannot be copied",
                    obj->getTypeName());
        break;
    default:
        assert(0);              // xpdf doesn't have any other types
    }
}

//**********************************************************************

static void writeRefs(PDF pdf, PdfDocument * pdf_doc)
{
    InObj *r, *n;
    XRef *xref;
    xref = pdf_doc->doc->getXRef();
    for (r = pdf_doc->inObjList; r != NULL;) {
        PdfObject obj1;
        xref->fetch(r->ref.num, r->ref.gen, &obj1);
        if (obj1->isStream())
            pdf_begin_obj(pdf, r->num, 0);
        else
            pdf_begin_obj(pdf, r->num, 2);      // \pdfobjcompresslevel = 2 is for this
        copyObject(pdf, pdf_doc, &obj1);
        pdf_puts(pdf, "\n");
        pdf_end_obj(pdf);
        n = r->next;
        delete r;
        pdf_doc->inObjList = r = n;
    }
}

// get the pagebox coordinates according to the pagebox_spec

static PDFRectangle *get_pagebox(Page * page, int pagebox_spec)
{
    switch (pagebox_spec) {
    case PDF_BOX_SPEC_MEDIA:
        return page->getMediaBox();
        break;
    case PDF_BOX_SPEC_CROP:
        return page->getCropBox();
        break;
    case PDF_BOX_SPEC_BLEED:
        return page->getBleedBox();
        break;
    case PDF_BOX_SPEC_TRIM:
        return page->getTrimBox();
        break;
    case PDF_BOX_SPEC_ART:
        return page->getArtBox();
        break;
    default:
        pdftex_fail("PDF inclusion: unknown value of pagebox spec (%i)",
                    (int) pagebox_spec);
    }
    return page->getMediaBox(); // to make the compiler happy
}

#define PDF_CHECKSUM_SIZE 32
char *get_file_checksum(char *a)
{
    struct stat finfo;
    char *ck = NULL;
    if (stat(a, &finfo) == 0) {
        off_t size = finfo.st_size;
        time_t mtime = finfo.st_mtime;
        ck = (char *) malloc(PDF_CHECKSUM_SIZE);
        if (ck == NULL)
            pdftex_fail("PDF inclusion: out of memory while processing '%s'",
                        a);
        snprintf(ck, PDF_CHECKSUM_SIZE, "%llu_%llu", (unsigned long long) size,
                 (unsigned long long) mtime);
    } else {
        pdftex_fail("PDF inclusion: could not stat() file '%s'", a);
    }
    return ck;
}

// Reads various information about the PDF and sets it up for later inclusion.
// This will fail if the PDF version of the PDF is higher than
// minor_pdf_version_wanted or page_name is given and can not be found.
// It makes no sense to give page_name _and_ page_num.
// Returns the page number.

void
read_pdf_info(image_dict * idict, int minor_pdf_version_wanted,
              int pdf_inclusion_errorlevel, img_readtype_e readtype)
{
    PdfDocument *pdf_doc;
    Page *page;
    int rotate;
    char *checksum;
    PDFRectangle *pagebox;
    float pdf_version_found, pdf_version_wanted, xsize, ysize, xorig, yorig;
    assert(idict != NULL);
    assert(img_type(idict) == IMG_TYPE_PDF);
    assert(readtype == IMG_CLOSEINBETWEEN);     // only this is implemented
    // initialize
    if (isInit == gFalse) {
        globalParams = new GlobalParams();
        globalParams->setErrQuiet(gFalse);
        isInit = gTrue;
    }
    // calculate a checksum string
    checksum = get_file_checksum(img_filepath(idict));
    // open PDF file
    pdf_doc = refPdfDocument(img_filepath(idict));
    if (strncmp(pdf_doc->checksum, checksum, PDF_CHECKSUM_SIZE) != 0) {
        pdftex_fail("PDF inclusion: file has changed '%s'",
                    img_filename(idict));
    }
    free(checksum);
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
    // get the required page
    page = pdf_doc->doc->getCatalog()->getPage(img_pagenum(idict));

    // get the pagebox coordinates (media, crop,...) to use.
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

    // currently unused info whether PDF contains a /Group
    if (page->getGroup() != NULL)
        img_set_group(idict);

    if (readtype == IMG_CLOSEINBETWEEN)
        unrefPdfDocument(img_filepath(idict));
}

//**********************************************************************
// Writes the current epf_doc.
// Here the included PDF is copied, so most errors that can happen
// during PDF inclusion will arise here.

static void write_epdf1(PDF pdf, image_dict * idict)
{
    PdfDocument *pdf_doc;
    Page *page;
    Ref *pageref;
    Dict *pageDict;
    PdfObject info, contents, obj1, pageObj, dictObj;
    PDFRectangle *pagebox;
    int i, l;
    float bbox[4];
    char *checksum;
    char s[256];
    char *pagedictkeys[] =
        { "Group", "LastModified", "Metadata", "PieceInfo", "Resources",
        "SeparationInfo", NULL
    };

    assert(idict != NULL);
    // calculate a checksum string
    checksum = get_file_checksum(img_filepath(idict));
    // open PDF file
    pdf_doc = refPdfDocument(img_filepath(idict));
    if (strncmp(pdf_doc->checksum, checksum, PDF_CHECKSUM_SIZE) != 0) {
        pdftex_fail("PDF inclusion: file has changed '%s'",
                    img_filename(idict));
    }
    free(checksum);
    page = pdf_doc->doc->getCatalog()->getPage(img_pagenum(idict));
    pageref = pdf_doc->doc->getCatalog()->getPageRef(img_pagenum(idict));
    assert(pageref != NULL);    // was checked already in read_pdf_info()
    pdf_doc->doc->getXRef()->fetch(pageref->num, pageref->gen, &pageObj);
    pageDict = pageObj->getDict();

    // write the Page header
    pdf_puts(pdf, "/Type /XObject\n/Subtype /Form\n");
    if (img_attr(idict) != NULL && strlen(img_attr(idict)) > 0)
        pdf_printf(pdf, "%s\n", img_attr(idict));
    pdf_puts(pdf, "/FormType 1\n");

    // write additional information
    pdf_printf(pdf, "/%s.FileName (%s)\n", pdfkeyprefix,
               convertStringToPDFString(pdf_doc->file_path,
                                        strlen(pdf_doc->file_path)));
    pdf_printf(pdf, "/%s.PageNumber %i\n", pdfkeyprefix,
               (int) img_pagenum(idict));
    pdf_doc->doc->getDocInfoNF(&info);
    if (info->isRef()) {
        // the info dict must be indirect (PDF Ref p. 61)
        pdf_printf(pdf, "/%s.InfoDict ", pdfkeyprefix);
        pdf_printf(pdf, "%d 0 R\n", addInObj(pdf, pdf_doc, info->getRef()));
    }
    if (img_is_bbox(idict)) {
        bbox[0] = int2bp(img_bbox(idict)[0]);
        bbox[1] = int2bp(img_bbox(idict)[1]);
        bbox[2] = int2bp(img_bbox(idict)[2]);
        bbox[3] = int2bp(img_bbox(idict)[3]);
    } else {
        // get the pagebox coordinates (media, crop,...) to use.
        pagebox = get_pagebox(page, img_pagebox(idict));
        bbox[0] = pagebox->x1;
        bbox[1] = pagebox->y1;
        bbox[2] = pagebox->x2;
        bbox[3] = pagebox->y2;
    }
    sprintf(s, "/BBox [%.8f %.8f %.8f %.8f]\n", bbox[0], bbox[1], bbox[2],
            bbox[3]);
    pdf_puts(pdf, stripzeros(s));
    // The /Matrix calculation is replaced by transforms in out_img().

    // Now all relevant parts of the Page dictionary are copied:

    // Resources validity check
    pageDict->lookupNF("Resources", &dictObj);
    if (dictObj->isNull()) {
        // Resources can be missing (files without them have been spotted
        // in the wild); in which case the /Resouces of the /Page will be used.
        // "This practice is not recommended".
        pdftex_warn
            ("PDF inclusion: /Resources missing. 'This practice is not recommended' (PDF Ref.)");
    }

    // Metadata validity check (as a stream it must be indirect)
    pageDict->lookupNF("Metadata", &dictObj);
    if (!dictObj->isNull() && !dictObj->isRef())
        pdftex_warn("PDF inclusion: /Metadata must be indirect object");

    // copy selected items in Page dictionary
    for (i = 0; pagedictkeys[i] != NULL; i++) {
        pageDict->lookupNF(pagedictkeys[i], &dictObj);
        if (!dictObj->isNull()) {
            pdf_printf(pdf, "/%s ", pagedictkeys[i]);
            copyObject(pdf, pdf_doc, &dictObj); // preserves indirection
        }
    }

    // write the Page contents
    page->getContents(&contents);
    if (contents->isStream()) {
        // Variant A: get stream and recompress under control
        // of \pdfcompresslevel
        //
        // pdfbeginstream();
        // copyStreamStream(contents->getStream());
        // pdfendstream();

        // Variant B: copy stream without recompressing
        //
        contents->streamGetDict()->lookup((char *) "F", &obj1);
        if (!obj1->isNull()) {
            pdftex_fail("PDF inclusion: Unsupported external stream");
        }
        contents->streamGetDict()->lookup((char *) "Length", &obj1);
        assert(!obj1->isNull());
        pdf_puts(pdf, "/Length ");
        copyObject(pdf, pdf_doc, &obj1);
        pdf_puts(pdf, "\n");
        contents->streamGetDict()->lookup((char *) "Filter", &obj1);
        if (!obj1->isNull()) {
            pdf_puts(pdf, "/Filter ");
            copyObject(pdf, pdf_doc, &obj1);
            pdf_puts(pdf, "\n");
            contents->streamGetDict()->lookup((char *) "DecodeParms", &obj1);
            if (!obj1->isNull()) {
                pdf_puts(pdf, "/DecodeParms ");
                copyObject(pdf, pdf_doc, &obj1);
                pdf_puts(pdf, "\n");
            }
        }
        pdf_puts(pdf, ">>\nstream\n");
        copyStreamStream(pdf, contents->getStream()->getBaseStream());
        pdf_end_stream(pdf);
    } else if (contents->isArray()) {
        pdf_begin_stream(pdf);
        for (i = 0, l = contents->arrayGetLength(); i < l; ++i) {
            PdfObject contentsobj;
            copyStreamStream(pdf,
                             (contents->
                              arrayGet(i, &contentsobj))->getStream());
            if (i < (l - 1)) {
                // put a space between streams to be on the safe side (streams
                // should have a trailing space here, but one never knows)
                pdf_puts(pdf, " ");
            }
        }
        pdf_end_stream(pdf);
    } else {                    // the contents are optional, but we need to include an empty stream
        pdf_begin_stream(pdf);
        pdf_end_stream(pdf);
    }
    // write out all indirect objects
    writeRefs(pdf, pdf_doc);
}

// this relay function is needed to keep some destructor quiet (???)

void write_epdf(PDF pdf, image_dict * idict)
{
    write_epdf1(pdf, idict);
    unrefPdfDocument(img_filepath(idict));
}

//**********************************************************************
// Deallocate a PdfDocument with all its resources

static void deletePdfDocumentPdfDoc(PdfDocument * pdf_doc)
{
    InObj *r, *n;
    assert(pdf_doc != NULL);
    // this may be probably needed for an emergency destroyPdfDocument()
    for (r = pdf_doc->inObjList; r != NULL; r = n) {
        n = r->next;
        delete r;
    }
    delete pdf_doc->doc;
    pdf_doc->doc = NULL;
}

static void destroyPdfDocument(void *pa, void * /*pb */ )
{
    PdfDocument *pdf_doc = (PdfDocument *) pa;
    deletePdfDocumentPdfDoc(pdf_doc);
    // TODO: delete rest of pdf_doc
}

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
        assert(pdf_doc->inObjList == NULL);     // should be eaten up already
        deletePdfDocumentPdfDoc(pdf_doc);
    }
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
