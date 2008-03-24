/*
Copyright (c) 1996-2006 Han The Thanh, <thanh@pdftex.org>

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

$Id$
*/

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <aconf.h>
#include <assert.h>
#include <GString.h>
#include <gmem.h>
#include <gfile.h>
#include "Object.h"
#include "Stream.h"
#include "Array.h"
#include "Dict.h"
#include "XRef.h"
#include "Catalog.h"
#include "Page.h"
#include "GfxFont.h"
#include "PDFDoc.h"
#include "GlobalParams.h"
#include "Error.h"

static XRef *xref = 0;

int main(int argc, char *argv[])
{
    char *p, buf[1024];
    PDFDoc *doc;
    GString *fileName;
    Stream *s;
    Object srcStream, srcName, catalogDict;
    FILE *outfile;
    char *outname;
    int objnum = 0, objgen = 0;
    bool extract_xref_table = false;
    int c;
    fprintf(stderr, "pdftosrc version %s\n", xpdfVersion);
    if (argc < 2) {
        fprintf(stderr,
                "Usage: pdftosrc <PDF-file> [<stream-object-number>]\n");
        exit(1);
    }
    fileName = new GString(argv[1]);
    globalParams = new GlobalParams();
    doc = new PDFDoc(fileName);
    if (!doc->isOk()) {
        fprintf(stderr, "Invalid PDF file\n");
        exit(1);
    }
    if (argc >= 3) {
        objnum = atoi(argv[2]);
        if (argc >= 4)
            objgen = atoi(argv[3]);
    }
    xref = doc->getXRef();
    catalogDict.initNull();
    xref->getCatalog(&catalogDict);
    if (!catalogDict.isDict("Catalog")) {
        fprintf(stderr, "No Catalog found\n");
        exit(1);
    }
    srcStream.initNull();
    if (objnum == 0) {
        catalogDict.dictLookup("SourceObject", &srcStream);
        if (!srcStream.isStream("SourceFile")) {
            fprintf(stderr, "No SourceObject found\n");
            exit(1);
        }
        srcName.initNull();
        srcStream.getStream()->getDict()->lookup("SourceName", &srcName);
        if (!srcName.isString()) {
            fprintf(stderr, "No SourceName found\n");
            exit(1);
        }
        outname = srcName.getString()->getCString();
        // We cannot free srcName, as objname shares its string.
        // srcName.free();
    } else if (objnum > 0) {
        xref->fetch(objnum, objgen, &srcStream);
        if (!srcStream.isStream()) {
            fprintf(stderr, "Not a Stream object\n");
            exit(1);
        }
        sprintf(buf, "%s", fileName->getCString());
        if ((p = strrchr(buf, '.')) == 0)
            p = strchr(buf, 0);
        if (objgen == 0)
            sprintf(p, ".%i", objnum);
        else
            sprintf(p, ".%i+%i", objnum, objgen);
        outname = buf;
    } else {                    // objnum < 0 means we are extracting the XRef table
        extract_xref_table = true;
        sprintf(buf, "%s", fileName->getCString());
        if ((p = strrchr(buf, '.')) == 0)
            p = strchr(buf, 0);
        sprintf(p, ".xref");
        outname = buf;
    }
    if (!(outfile = fopen(outname, "wb"))) {
        fprintf(stderr, "Cannot open file \"%s\" for writing\n", outname);
        exit(1);
    }
    if (extract_xref_table) {
        int size = xref->getSize();
        int i;
        for (i = 0; i < size; i++) {
            if (xref->getEntry(i)->offset == 0xffffffff)
                break;
        }
        size = i;
        fprintf(outfile, "xref\n");
        fprintf(outfile, "0 %i\n", size);
        for (i = 0; i < size; i++) {
            XRefEntry *e = xref->getEntry(i);
            if (e->type != xrefEntryCompressed)
                fprintf(outfile, "%.10lu %.5i %s\n",
                        (long unsigned) e->offset, e->gen,
                        (e->type == xrefEntryFree ? "f" : "n"));
            else {              // e->offset is the object number of the object stream
                // e->gen is the local index inside that object stream
                //int objStrOffset = xref->getEntry(e->offset)->offset;
                Object tmpObj;

                xref->fetch(i, e->gen, &tmpObj);        // to ensure xref->objStr is set
                ObjectStream *objStr = xref->getObjStr();
                assert(objStr != NULL);
                int *localOffsets = objStr->getOffsets();
                assert(localOffsets != NULL);
//                 fprintf(outfile, "%0.10lu %i n\n",
//                         (long unsigned) (objStrOffset), e->gen);
                fprintf(outfile, "%.10lu 00000 n\n",
                        (long unsigned) (objStr->getFirstOffset() +
                                         localOffsets[e->gen]));
//                         (long unsigned) (objStrOffset + objStr->getStart() + localOffsets[e->gen]));
                tmpObj.free();
            }
        }
    } else {
        s = srcStream.getStream();
        s->reset();
        while ((c = s->getChar()) != EOF)
            fputc(c, outfile);
        srcStream.free();
    }
    if (objnum == 0)
        fprintf(stderr, "Source file extracted to %s\n", outname);
    else if (objnum > 0)
        fprintf(stderr, "Stream object extracted to %s\n", outname);
    else
        fprintf(stderr, "Cross-reference table extracted to %s\n", outname);
    fclose(outfile);
    catalogDict.free();
    delete doc;
    delete globalParams;
}
