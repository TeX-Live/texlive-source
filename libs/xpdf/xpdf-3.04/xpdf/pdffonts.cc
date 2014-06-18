//========================================================================
//
// pdffonts.cc
//
// Copyright 2001-2007 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "parseargs.h"
#include "GString.h"
#include "gmem.h"
#include "GlobalParams.h"
#include "Error.h"
#include "Object.h"
#include "Dict.h"
#include "GfxFont.h"
#include "Annot.h"
#include "Form.h"
#include "PDFDoc.h"
#include "config.h"

// NB: this must match the definition of GfxFontType in GfxFont.h.
static const char *fontTypeNames[] = {
  "unknown",
  "Type 1",
  "Type 1C",
  "Type 1C (OT)",
  "Type 3",
  "TrueType",
  "TrueType (OT)",
  "CID Type 0",
  "CID Type 0C",
  "CID Type 0C (OT)",
  "CID TrueType",
  "CID TrueType (OT)"
};

static void scanFonts(Object *obj, PDFDoc *doc);
static void scanFonts(Dict *resDict, PDFDoc *doc);
static void scanFont(GfxFont *font, PDFDoc *doc);

static int firstPage = 1;
static int lastPage = 0;
static GBool showFontLoc = gFalse;
static GBool showFontLocPS = gFalse;
static char ownerPassword[33] = "\001";
static char userPassword[33] = "\001";
static char cfgFileName[256] = "";
static GBool printVersion = gFalse;
static GBool printHelp = gFalse;

static ArgDesc argDesc[] = {
  {"-f",      argInt,      &firstPage,     0,
   "first page to examine"},
  {"-l",      argInt,      &lastPage,      0,
   "last page to examine"},
  {"-loc",    argFlag,     &showFontLoc,   0,
   "print extended info on font location"},
  {"-locPS",  argFlag,     &showFontLocPS, 0,
   "print extended info on font location for PostScript conversion"},
  {"-opw",    argString,   ownerPassword,  sizeof(ownerPassword),
   "owner password (for encrypted files)"},
  {"-upw",    argString,   userPassword,   sizeof(userPassword),
   "user password (for encrypted files)"},
  {"-cfg",        argString,      cfgFileName,    sizeof(cfgFileName),
   "configuration file to use in place of .xpdfrc"},
  {"-v",      argFlag,     &printVersion,  0,
   "print copyright and version info"},
  {"-h",      argFlag,     &printHelp,     0,
   "print usage information"},
  {"-help",   argFlag,     &printHelp,     0,
   "print usage information"},
  {"--help",  argFlag,     &printHelp,     0,
   "print usage information"},
  {"-?",      argFlag,     &printHelp,     0,
   "print usage information"},
  {NULL}
};

static Ref *fonts;
static int fontsLen;
static int fontsSize;

static Ref *seenObjs;
static int seenObjsLen;
static int seenObjsSize;

int main(int argc, char *argv[]) {
  PDFDoc *doc;
  GString *fileName;
  GString *ownerPW, *userPW;
  GBool ok;
  Page *page;
  Dict *resDict;
  Annots *annots;
  Form *form;
  Object obj1, obj2;
  int pg, i, j;
  int exitCode;

  exitCode = 99;

  // parse args
  ok = parseArgs(argDesc, &argc, argv);
  if (!ok || argc != 2 || printVersion || printHelp) {
    fprintf(stderr, "pdffonts version %s\n", xpdfVersion);
    fprintf(stderr, "%s\n", xpdfCopyright);
    if (!printVersion) {
      printUsage("pdffonts", "<PDF-file>", argDesc);
    }
    goto err0;
  }
  fileName = new GString(argv[1]);

  // read config file
  globalParams = new GlobalParams(cfgFileName);
  globalParams->setupBaseFonts(NULL);

  // open PDF file
  if (ownerPassword[0] != '\001') {
    ownerPW = new GString(ownerPassword);
  } else {
    ownerPW = NULL;
  }
  if (userPassword[0] != '\001') {
    userPW = new GString(userPassword);
  } else {
    userPW = NULL;
  }
  doc = new PDFDoc(fileName, ownerPW, userPW);
  if (userPW) {
    delete userPW;
  }
  if (ownerPW) {
    delete ownerPW;
  }
  if (!doc->isOk()) {
    exitCode = 1;
    goto err1;
  }

  // get page range
  if (firstPage < 1) {
    firstPage = 1;
  }
  if (lastPage < 1 || lastPage > doc->getNumPages()) {
    lastPage = doc->getNumPages();
  }

  // scan the fonts
  if (showFontLoc || showFontLocPS) {
    printf("name                                 type              emb sub uni object ID location\n");
    printf("------------------------------------ ----------------- --- --- --- --------- --------\n");
  } else {
    printf("name                                 type              emb sub uni object ID\n");
    printf("------------------------------------ ----------------- --- --- --- ---------\n");
  }
  fonts = NULL;
  fontsLen = fontsSize = 0;
  seenObjs = NULL;
  seenObjsLen = seenObjsSize = 0;
  for (pg = firstPage; pg <= lastPage; ++pg) {
    page = doc->getCatalog()->getPage(pg);
    if ((resDict = page->getResourceDict())) {
      scanFonts(resDict, doc);
    }
    annots = new Annots(doc, page->getAnnots(&obj1));
    obj1.free();
    for (i = 0; i < annots->getNumAnnots(); ++i) {
      if (annots->getAnnot(i)->getAppearance(&obj1)->isStream()) {
	obj1.streamGetDict()->lookupNF("Resources", &obj2);
	scanFonts(&obj2, doc);
	obj2.free();
      }
      obj1.free();
    }
    delete annots;
  }
  if ((form = doc->getCatalog()->getForm())) {
    for (i = 0; i < form->getNumFields(); ++i) {
      form->getField(i)->getResources(&obj1);
      if (obj1.isArray()) {
	for (j = 0; j < obj1.arrayGetLength(); ++j) {
	  obj1.arrayGetNF(j, &obj2);
	  scanFonts(&obj2, doc);
	  obj2.free();
	}
      } else if (obj1.isDict()) {
	scanFonts(obj1.getDict(), doc);
      }
      obj1.free();
    }
  }

  exitCode = 0;

  // clean up
  gfree(fonts);
  gfree(seenObjs);
 err1:
  delete doc;
  delete globalParams;
 err0:

  // check for memory leaks
  Object::memCheck(stderr);
  gMemReport(stderr);

  return exitCode;
}

static void scanFonts(Object *obj, PDFDoc *doc) {
  Object obj2;
  int i;

  if (obj->isRef()) {
    for (i = 0; i < seenObjsLen; ++i) {
      if (obj->getRefNum() == seenObjs[i].num &&
	  obj->getRefGen() == seenObjs[i].gen) {
	return;
      }
    }
    if (seenObjsLen == seenObjsSize) {
      if (seenObjsSize <= INT_MAX - 32) {
	seenObjsSize += 32;
      } else {
	// let greallocn throw an exception
	seenObjsSize = -1;
      }
      seenObjs = (Ref *)greallocn(seenObjs, seenObjsSize, sizeof(Ref));
    }
    seenObjs[seenObjsLen++] = obj->getRef();
  }
  if (obj->fetch(doc->getXRef(), &obj2)->isDict()) {
    scanFonts(obj2.getDict(), doc);
  }
  obj2.free();
}

static void scanFonts(Dict *resDict, PDFDoc *doc) {
  Object obj1, obj2, xObjDict, xObj;
  Object patternDict, pattern, gsDict, gs, smask, smaskGroup, resObj;
  Ref r;
  GfxFontDict *gfxFontDict;
  GfxFont *font;
  int i;

  // scan the fonts in this resource dictionary
  gfxFontDict = NULL;
  resDict->lookupNF("Font", &obj1);
  if (obj1.isRef()) {
    obj1.fetch(doc->getXRef(), &obj2);
    if (obj2.isDict()) {
      r = obj1.getRef();
      gfxFontDict = new GfxFontDict(doc->getXRef(), &r, obj2.getDict());
    }
    obj2.free();
  } else if (obj1.isDict()) {
    gfxFontDict = new GfxFontDict(doc->getXRef(), NULL, obj1.getDict());
  }
  if (gfxFontDict) {
    for (i = 0; i < gfxFontDict->getNumFonts(); ++i) {
      if ((font = gfxFontDict->getFont(i))) {
	scanFont(font, doc);
      }
    }
    delete gfxFontDict;
  }
  obj1.free();

  // recursively scan any resource dictionaries in XObjects in this
  // resource dictionary
  resDict->lookup("XObject", &xObjDict);
  if (xObjDict.isDict()) {
    for (i = 0; i < xObjDict.dictGetLength(); ++i) {
      xObjDict.dictGetVal(i, &xObj);
      if (xObj.isStream()) {
	xObj.streamGetDict()->lookupNF("Resources", &resObj);
	scanFonts(&resObj, doc);
	resObj.free();
      }
      xObj.free();
    }
  }
  xObjDict.free();

  // recursively scan any resource dictionaries in Patterns in this
  // resource dictionary
  resDict->lookup("Pattern", &patternDict);
  if (patternDict.isDict()) {
    for (i = 0; i < patternDict.dictGetLength(); ++i) {
      patternDict.dictGetVal(i, &pattern);
      if (pattern.isStream()) {
	pattern.streamGetDict()->lookupNF("Resources", &resObj);
	scanFonts(&resObj, doc);
	resObj.free();
      }
      pattern.free();
    }
  }
  patternDict.free();

  // recursively scan any resource dictionaries in ExtGStates in this
  // resource dictionary
  resDict->lookup("ExtGState", &gsDict);
  if (gsDict.isDict()) {
    for (i = 0; i < gsDict.dictGetLength(); ++i) {
      if (gsDict.dictGetVal(i, &gs)->isDict()) {
	if (gs.dictLookup("SMask", &smask)->isDict()) {
	  if (smask.dictLookup("G", &smaskGroup)->isStream()) {
	    smaskGroup.streamGetDict()->lookupNF("Resources", &resObj);
	    scanFonts(&resObj, doc);
	    resObj.free();
	  }
	  smaskGroup.free();
	}
	smask.free();
      }
      gs.free();
    }
  }
  gsDict.free();
}

static void scanFont(GfxFont *font, PDFDoc *doc) {
  Ref fontRef, embRef;
  Object fontObj, toUnicodeObj;
  GString *name;
  GBool emb, subset, hasToUnicode;
  GfxFontLoc *loc;
  int i;

  fontRef = *font->getID();

  // check for an already-seen font
  for (i = 0; i < fontsLen; ++i) {
    if (fontRef.num == fonts[i].num && fontRef.gen == fonts[i].gen) {
      return;
    }
  }

  // font name
  name = font->getName();

  // check for an embedded font
  if (font->getType() == fontType3) {
    emb = gTrue;
  } else {
    emb = font->getEmbeddedFontID(&embRef);
  }

  // look for a ToUnicode map
  hasToUnicode = gFalse;
  if (doc->getXRef()->fetch(fontRef.num, fontRef.gen, &fontObj)->isDict()) {
    hasToUnicode = fontObj.dictLookup("ToUnicode", &toUnicodeObj)->isStream();
    toUnicodeObj.free();
  }
  fontObj.free();

  // check for a font subset name: capital letters followed by a '+'
  // sign
  subset = gFalse;
  if (name) {
    for (i = 0; i < name->getLength(); ++i) {
      if (name->getChar(i) < 'A' || name->getChar(i) > 'Z') {
	break;
      }
    }
    subset = i > 0 && i < name->getLength() && name->getChar(i) == '+';
  }

  // print the font info
  printf("%-36s %-17s %-3s %-3s %-3s",
	 name ? name->getCString() : "[none]",
	 fontTypeNames[font->getType()],
	 emb ? "yes" : "no",
	 subset ? "yes" : "no",
	 hasToUnicode ? "yes" : "no");
  if (fontRef.gen >= 100000) {
    printf(" [none]");
  } else {
    printf(" %6d %2d", fontRef.num, fontRef.gen);
  }
  if (showFontLoc || showFontLocPS) {
    if (font->getType() == fontType3) {
      printf(" embedded");
    } else {
      loc = font->locateFont(doc->getXRef(), showFontLocPS);
      if (loc) {
	if (loc->locType == gfxFontLocEmbedded) {
	  printf(" embedded");
	} else if (loc->locType == gfxFontLocExternal) {
	  if (loc->path) {
	    printf(" external: %s", loc->path->getCString());
	  } else {
	    printf(" unavailable");
	  }
	} else if (loc->locType == gfxFontLocResident) {
	  if (loc->path) {
	    printf(" resident: %s", loc->path->getCString());
	  } else {
	    printf(" unavailable");
	  }
	}
      } else {
	printf(" unknown");
      }
      delete loc;
    }
  }
  printf("\n");

  // add this font to the list
  if (fontsLen == fontsSize) {
    if (fontsSize <= INT_MAX - 32) {
      fontsSize += 32;
    } else {
      // let greallocn throw an exception
      fontsSize = -1;
    }
    fonts = (Ref *)greallocn(fonts, fontsSize, sizeof(Ref));
  }
  fonts[fontsLen++] = *font->getID();
}
