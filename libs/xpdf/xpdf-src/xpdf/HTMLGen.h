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

  double getZoom() { return zoom; }
  void setZoom(double zoomA) { zoom = zoomA; }

  GBool getDrawInvisibleText() { return drawInvisibleText; }
  void setDrawInvisibleText(GBool drawInvisibleTextA)
    { drawInvisibleText = drawInvisibleTextA; }

  GBool getAllTextInvisible() { return allTextInvisible; }
  void setAllTextInvisible(GBool allTextInvisibleA)
    { allTextInvisible = allTextInvisibleA; }

  void startDoc(PDFDoc *docA);
  int convertPage(int pg, const char *pngURL,
		  int (*writeHTML)(void *stream, const char *data, int size),
		  void *htmlStream,
		  int (*writePNG)(void *stream, const char *data, int size),
		  void *pngStream);

private:

  int findDirSpan(GList *words, int firstWordIdx, int primaryDir,
		  int *spanDir);
  void appendSpans(GList *words, int firstWordIdx, int lastWordIdx,
		   int primaryDir, int spanDir,
		   double base, GBool dropCapLine, GString *s);
  void appendUTF8(Unicode u, GString *s);
  GString *getFontDefn(TextFontInfo *font, double *scale);

  double backgroundResolution;
  double zoom;
  GBool drawInvisibleText;
  GBool allTextInvisible;

  PDFDoc *doc;
  TextOutputDev *textOut;
  SplashOutputDev *splashOut;

  GList *fonts;
  double *fontScales;

  GBool ok;
};

#endif
