//========================================================================
//
// HTMLGen.h
//
// Copyright 2010 Glyph & Cog, LLC
//
//========================================================================

#ifndef HTMLGEN_H
#define HTMLGEN_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

class GString;
class PDFDoc;
class TextOutputDev;
class TextFontInfo;
class SplashOutputDev;

//------------------------------------------------------------------------

class HTMLGen {
public:

  HTMLGen(double backgroundResolutionA);
  ~HTMLGen();

  GBool isOk() { return ok; }

  double getBackgroundResolution() { return backgroundResolution; }
  void setBackgroundResolution(double backgroundResolutionA)
    { backgroundResolution = backgroundResolutionA; }

  GBool getDrawInvisibleText() { return drawInvisibleText; }
  void setDrawInvisibleText(GBool drawInvisibleTextA)
    { drawInvisibleText = drawInvisibleTextA; }

  void startDoc(PDFDoc *docA);
  int convertPage(int pg, const char *pngURL,
		  int (*writeHTML)(void *stream, const char *data, int size),
		  void *htmlStream,
		  int (*writePNG)(void *stream, const char *data, int size),
		  void *pngStream);

private:

  GString *getFontDefn(TextFontInfo *font, double *scale);

  double backgroundResolution;
  GBool drawInvisibleText;

  PDFDoc *doc;
  TextOutputDev *textOut;
  SplashOutputDev *splashOut;

  GBool ok;
};

#endif
