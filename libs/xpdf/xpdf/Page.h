//========================================================================
//
// Page.h
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

/* ------------------------------------------------------------------------
* Changed by Martin Schröder <martin@pdftex.org>
* $Id: Page.h 421 2008-04-26 21:59:55Z oneiros $
* Changelog:
* ------------------------------------------------------------------------
* r151 | ms | 2007-06-25 18:53:17 +0200 (Mo, 25 Jun 2007) | 3 lines
* 
* Merging xpdf 3.02 from HEAD into stable
* svn merge -r149:150 --dry-run svn+ssh://svn/srv/svn/repos/pdftex/trunk/source/src/libs/xpdf .
* 
* ------------------------------------------------------------------------
* r38 | ms | 2005-08-21 14:00:00 +0200 (So, 21 Aug 2005) | 2 lines
* 
* 1.30.1
* 
* ------------------------------------------------------------------------
* r11 | ms | 2004-09-06 14:01:00 +0200 (Mo, 06 Sep 2004) | 2 lines
* 
* 1.20a
* 
* ------------------------------------------------------------------------
* r6 | ms | 2003-10-06 14:01:00 +0200 (Mo, 06 Okt 2003) | 2 lines
* 
* released v1.11b
* 
* ------------------------------------------------------------------------
* r4 | ms | 2003-10-05 14:00:00 +0200 (So, 05 Okt 2003) | 2 lines
* 
* Moved sources to src
* 
* ------------------------------------------------------------------------
* r1 | ms | 2003-08-02 14:00:00 +0200 (Sa, 02 Aug 2003) | 1 line
* 
* 1.11a
* ------------------------------------------------------------------------ */

#ifndef PAGE_H
#define PAGE_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "Object.h"

class Dict;
class XRef;
class OutputDev;
class Links;
class Catalog;

//------------------------------------------------------------------------

class PDFRectangle {
public:
  double x1, y1, x2, y2;

  PDFRectangle() { x1 = y1 = x2 = y2 = 0; }
  PDFRectangle(double x1A, double y1A, double x2A, double y2A)
    { x1 = x1A; y1 = y1A; x2 = x2A; y2 = y2A; }
  GBool isValid() { return x1 != 0 || y1 != 0 || x2 != 0 || y2 != 0; }
  void clipTo(PDFRectangle *rect);
};

//------------------------------------------------------------------------
// PageAttrs
//------------------------------------------------------------------------

class PageAttrs {
public:

  // Construct a new PageAttrs object by merging a dictionary
  // (of type Pages or Page) into another PageAttrs object.  If
  // <attrs> is NULL, uses defaults.
  PageAttrs(PageAttrs *attrs, Dict *dict);

  // Destructor.
  ~PageAttrs();

  // Accessors.
  PDFRectangle *getMediaBox() { return &mediaBox; }
  PDFRectangle *getCropBox() { return &cropBox; }
  GBool isCropped() { return haveCropBox; }
  PDFRectangle *getBleedBox() { return &bleedBox; }
  PDFRectangle *getTrimBox() { return &trimBox; }
  PDFRectangle *getArtBox() { return &artBox; }
  int getRotate() { return rotate; }
  GString *getLastModified()
    { return lastModified.isString()
	? lastModified.getString() : (GString *)NULL; }
  Dict *getBoxColorInfo()
    { return boxColorInfo.isDict() ? boxColorInfo.getDict() : (Dict *)NULL; }
  Dict *getGroup()
    { return group.isDict() ? group.getDict() : (Dict *)NULL; }
  Stream *getMetadata()
    { return metadata.isStream() ? metadata.getStream() : (Stream *)NULL; }
  Dict *getPieceInfo()
    { return pieceInfo.isDict() ? pieceInfo.getDict() : (Dict *)NULL; }
  Dict *getSeparationInfo()
    { return separationInfo.isDict()
	? separationInfo.getDict() : (Dict *)NULL; }
  Dict *getResourceDict()
    { return resources.isDict() ? resources.getDict() : (Dict *)NULL; }

private:

  GBool readBox(Dict *dict, char *key, PDFRectangle *box);

  PDFRectangle mediaBox;
  PDFRectangle cropBox;
  GBool haveCropBox;
  PDFRectangle bleedBox;
  PDFRectangle trimBox;
  PDFRectangle artBox;
  int rotate;
  Object lastModified;
  Object boxColorInfo;
  Object group;
  Object metadata;
  Object pieceInfo;
  Object separationInfo;
  Object resources;
};

//------------------------------------------------------------------------
// Page
//------------------------------------------------------------------------

class Page {
public:

  // Constructor.
  Page(XRef *xrefA, int numA, Dict *pageDict, PageAttrs *attrsA);

  // Destructor.
  ~Page();

  // Is page valid?
  GBool isOk() { return ok; }

  // Get page parameters.
  int getNum() { return num; }
  PDFRectangle *getMediaBox() { return attrs->getMediaBox(); }
  PDFRectangle *getCropBox() { return attrs->getCropBox(); }
  GBool isCropped() { return attrs->isCropped(); }
  double getMediaWidth() 
    { return attrs->getMediaBox()->x2 - attrs->getMediaBox()->x1; }
  double getMediaHeight()
    { return attrs->getMediaBox()->y2 - attrs->getMediaBox()->y1; }
  double getCropWidth() 
    { return attrs->getCropBox()->x2 - attrs->getCropBox()->x1; }
  double getCropHeight()
    { return attrs->getCropBox()->y2 - attrs->getCropBox()->y1; }
  PDFRectangle *getBleedBox() { return attrs->getBleedBox(); }
  PDFRectangle *getTrimBox() { return attrs->getTrimBox(); }
  PDFRectangle *getArtBox() { return attrs->getArtBox(); }
  int getRotate() { return attrs->getRotate(); }
  GString *getLastModified() { return attrs->getLastModified(); }
  Dict *getBoxColorInfo() { return attrs->getBoxColorInfo(); }
  Dict *getGroup() { return attrs->getGroup(); }
  Stream *getMetadata() { return attrs->getMetadata(); }
  Dict *getPieceInfo() { return attrs->getPieceInfo(); }
  Dict *getSeparationInfo() { return attrs->getSeparationInfo(); }

  // Get resource dictionary.
  Dict *getResourceDict() { return attrs->getResourceDict(); }

  // Get annotations array.
  Object *getAnnots(Object *obj) { return annots.fetch(xref, obj); }

  // Return a list of links.
  Links *getLinks(Catalog *catalog);

  // Get contents.
  Object *getContents(Object *obj) { return contents.fetch(xref, obj); }

  // Display a page.
  void display(OutputDev *out, double hDPI, double vDPI,
	       int rotate, GBool useMediaBox, GBool crop,
	       GBool printing, Catalog *catalog,
	       GBool (*abortCheckCbk)(void *data) = NULL,
	       void *abortCheckCbkData = NULL);

  // Display part of a page.
  void displaySlice(OutputDev *out, double hDPI, double vDPI,
		    int rotate, GBool useMediaBox, GBool crop,
		    int sliceX, int sliceY, int sliceW, int sliceH,
		    GBool printing, Catalog *catalog,
		    GBool (*abortCheckCbk)(void *data) = NULL,
		    void *abortCheckCbkData = NULL);

  void makeBox(double hDPI, double vDPI, int rotate,
	       GBool useMediaBox, GBool upsideDown,
	       double sliceX, double sliceY, double sliceW, double sliceH,
	       PDFRectangle *box, GBool *crop);

  void processLinks(OutputDev *out, Catalog *catalog);

#ifndef PDF_PARSER_ONLY
  // Get the page's default CTM.
  void getDefaultCTM(double *ctm, double hDPI, double vDPI,
		     int rotate, GBool useMediaBox, GBool upsideDown);
#endif

private:

  XRef *xref;			// the xref table for this PDF file
  int num;			// page number
  PageAttrs *attrs;		// page attributes
  Object annots;		// annotations array
  Object contents;		// page contents
  GBool ok;			// true if page is valid
};

#endif
