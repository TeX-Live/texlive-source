/*
Copyright (c) 1996-2007 Han The Thanh, <thanh@pdftex.org>

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

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef POPPLER_VERSION
#define GString GooString
#define xpdfVersion POPPLER_VERSION
#include <dirent.h>
#include <goo/GooString.h>
#include <goo/gmem.h>
#include <goo/gfile.h>
#else
#include <aconf.h>
#include <assert.h>
#include <GString.h>
#include <gmem.h>
#include <gfile.h>
#endif

#include "Object.h"
#include "Stream.h"
#include "Array.h"
#include "Dict.h"
#ifdef POPPLER_VERSION
// here we include a fixed version of XRef.h

//========================================================================
//
// XRef.h
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

//========================================================================
//
// Modified under the Poppler project - http://poppler.freedesktop.org
//
// All changes made under the Poppler project to this file are licensed
// under GPL version 2 or later
//
// Copyright (C) 2005 Brad Hards <bradh@frogmouth.net>
// Copyright (C) 2006, 2008 Albert Astals Cid <aacid@kde.org>
// Copyright (C) 2007-2008 Julien Rebetez <julienr@svn.gnome.org>
// Copyright (C) 2007 Carlos Garcia Campos <carlosgc@gnome.org>
//
// To see a description of the changes please see the Changelog file that
// came with your tarball or type make ChangeLog if you are building from git
//
//========================================================================

#ifndef XREF_H
#define XREF_H

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "goo/gtypes.h"
#include "Object.h"

class Dict;
class Stream;
class Parser;

class ObjectStream {
public:

  // Create an object stream, using object number <objStrNum>,
  // generation 0.
  ObjectStream(XRef *xref, int objStrNumA);

  ~ObjectStream();

  // Return the object number of this object stream.
  int getObjStrNum() { return objStrNum; }

  // Get the <objIdx>th object from this stream, which should be
  // object number <objNum>, generation 0.
  Object *getObject(int objIdx, int objNum, Object *obj);

  int *getOffsets() { return offsets; }
  Guint getFirstOffset() { return firstOffset; }

private:

  int objStrNum;		// object number of the object stream
  int nObjects;			// number of objects in the stream
  Object *objs;			// the objects (length = nObjects)
  int *objNums;			// the object numbers (length = nObjects)
  int *offsets;			// the object offsets (length = nObjects)
  Guint firstOffset;
};

//------------------------------------------------------------------------
// XRef
//------------------------------------------------------------------------

enum XRefEntryType {
  xrefEntryFree,
  xrefEntryUncompressed,
  xrefEntryCompressed
};

struct XRefEntry {
  Guint offset;
  int gen;
  XRefEntryType type;
  bool updated;
  Object obj; //if this entry was updated, obj will contains the updated object
};

class XRef {
public:

  // Constructor, create an empty XRef, used for PDF writing
  XRef();
  // Constructor.  Read xref table from stream.
  XRef(BaseStream *strA);

  // Destructor.
  ~XRef();

  // Is xref table valid?
  GBool isOk() { return ok; }

  // Get the error code (if isOk() returns false).
  int getErrorCode() { return errCode; }

  // Set the encryption parameters.
  void setEncryption(int permFlagsA, GBool ownerPasswordOkA,
		     Guchar *fileKeyA, int keyLengthA,
		     int encVersionA, int encRevisionA,
		     CryptAlgorithm encAlgorithmA);

  // Is the file encrypted?
  GBool isEncrypted() { return encrypted; }

  // Check various permissions.
  GBool okToPrint(GBool ignoreOwnerPW = gFalse);
  GBool okToPrintHighRes(GBool ignoreOwnerPW = gFalse);
  GBool okToChange(GBool ignoreOwnerPW = gFalse);
  GBool okToCopy(GBool ignoreOwnerPW = gFalse);
  GBool okToAddNotes(GBool ignoreOwnerPW = gFalse);
  GBool okToFillForm(GBool ignoreOwnerPW = gFalse);
  GBool okToAccessibility(GBool ignoreOwnerPW = gFalse);
  GBool okToAssemble(GBool ignoreOwnerPW = gFalse);

  // Get catalog object.
  Object *getCatalog(Object *obj) { return fetch(rootNum, rootGen, obj); }

  // Fetch an indirect reference.
  Object *fetch(int num, int gen, Object *obj);

  // Return the document's Info dictionary (if any).
  Object *getDocInfo(Object *obj);
  Object *getDocInfoNF(Object *obj);

  // Return the number of objects in the xref table.
  int getNumObjects() { return size; }

  // Return the offset of the last xref table.
  Guint getLastXRefPos() { return lastXRefPos; }

  // Return the catalog object reference.
  int getRootNum() { return rootNum; }
  int getRootGen() { return rootGen; }

  // Get end position for a stream in a damaged file.
  // Returns false if unknown or file is not damaged.
  GBool getStreamEnd(Guint streamStart, Guint *streamEnd);

  // Retuns the entry that belongs to the offset
  int getNumEntry(Guint offset) const;

  // Direct access.
  int getSize() { return size; }
  XRefEntry *getEntry(int i) { return &entries[i]; }
  Object *getTrailerDict() { return &trailerDict; }
  ObjectStream *getObjStr() { return objStr; }

  // Write access
  void setModifiedObject(Object* o, Ref r);
  Ref addIndirectObject (Object* o);
  void add(int num, int gen,  Guint offs, GBool used);
  void writeToFile(OutStream* outStr, GBool writeAllEntries);

private:

  BaseStream *str;		// input stream
  Guint start;			// offset in file (to allow for garbage
				//   at beginning of file)
  XRefEntry *entries;		// xref entries
  int size;			// size of <entries> array
  int rootNum, rootGen;		// catalog dict
  GBool ok;			// true if xref table is valid
  int errCode;			// error code (if <ok> is false)
  Object trailerDict;		// trailer dictionary
  Guint lastXRefPos;		// offset of last xref table
  Guint *streamEnds;		// 'endstream' positions - only used in
				//   damaged files
  int streamEndsLen;		// number of valid entries in streamEnds
  ObjectStream *objStr;		// cached object stream
  GBool encrypted;		// true if file is encrypted
  int encRevision;		
  int encVersion;		// encryption algorithm
  CryptAlgorithm encAlgorithm;	// encryption algorithm
  int keyLength;		// length of key, in bytes
  int permFlags;		// permission bits
  Guchar fileKey[16];		// file decryption key
  GBool ownerPasswordOk;	// true if owner password is correct

  Guint getStartXref();
  GBool readXRef(Guint *pos);
  GBool readXRefTable(Parser *parser, Guint *pos);
  GBool readXRefStreamSection(Stream *xrefStr, int *w, int first, int n);
  GBool readXRefStream(Stream *xrefStr, Guint *pos);
  GBool constructXRef();
  Guint strToUnsigned(char *s);
};

#endif
#else
#include "XRef.h"
#endif
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
