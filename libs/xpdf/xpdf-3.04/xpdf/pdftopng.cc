//========================================================================
//
// pdftopng.cc
//
// Copyright 2009 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>
#include <stdlib.h>
#include <stdio.h>
#include <png.h>
#include "parseargs.h"
#include "gmem.h"
#include "GString.h"
#include "GlobalParams.h"
#include "Object.h"
#include "PDFDoc.h"
#include "SplashBitmap.h"
#include "Splash.h"
#include "SplashOutputDev.h"
#include "config.h"

static int firstPage = 1;
static int lastPage = 0;
static int resolution = 150;
static GBool mono = gFalse;
static GBool gray = gFalse;
static char enableFreeTypeStr[16] = "";
static char antialiasStr[16] = "";
static char vectorAntialiasStr[16] = "";
static char ownerPassword[33] = "";
static char userPassword[33] = "";
static GBool quiet = gFalse;
static char cfgFileName[256] = "";
static GBool printVersion = gFalse;
static GBool printHelp = gFalse;

static ArgDesc argDesc[] = {
  {"-f",      argInt,      &firstPage,     0,
   "first page to print"},
  {"-l",      argInt,      &lastPage,      0,
   "last page to print"},
  {"-r",      argInt,      &resolution,    0,
   "resolution, in DPI (default is 150)"},
  {"-mono",   argFlag,     &mono,          0,
   "generate a monochrome PBM file"},
  {"-gray",   argFlag,     &gray,          0,
   "generate a grayscale PGM file"},
#if HAVE_FREETYPE_FREETYPE_H | HAVE_FREETYPE_H
  {"-freetype",   argString,      enableFreeTypeStr, sizeof(enableFreeTypeStr),
   "enable FreeType font rasterizer: yes, no"},
#endif
  {"-aa",         argString,      antialiasStr,   sizeof(antialiasStr),
   "enable font anti-aliasing: yes, no"},
  {"-aaVector",   argString,      vectorAntialiasStr, sizeof(vectorAntialiasStr),
   "enable vector anti-aliasing: yes, no"},
  {"-opw",    argString,   ownerPassword,  sizeof(ownerPassword),
   "owner password (for encrypted files)"},
  {"-upw",    argString,   userPassword,   sizeof(userPassword),
   "user password (for encrypted files)"},
  {"-q",      argFlag,     &quiet,         0,
   "don't print any messages or errors"},
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

static void setupPNG(png_structp *png, png_infop *pngInfo, FILE *f,
		     int bitDepth, int colorType,
		     SplashBitmap *bitmap);
static void writePNGData(png_structp png, SplashBitmap *bitmap);
static void finishPNG(png_structp *png, png_infop *pngInfo);

int main(int argc, char *argv[]) {
  PDFDoc *doc;
  GString *fileName;
  char *pngRoot;
  GString *pngFile;
  GString *ownerPW, *userPW;
  SplashColor paperColor;
  SplashOutputDev *splashOut;
  GBool ok;
  int exitCode;
  int pg;
  png_structp png;
  png_infop pngInfo;
  FILE *f;

  exitCode = 99;

  // parse args
  ok = parseArgs(argDesc, &argc, argv);
  if (mono && gray) {
    ok = gFalse;
  }
  if (!ok || argc != 3 || printVersion || printHelp) {
    fprintf(stderr, "pdftopng version %s\n", xpdfVersion);
    fprintf(stderr, "%s\n", xpdfCopyright);
    if (!printVersion) {
      printUsage("pdftopng", "<PDF-file> <PNG-root>", argDesc);
    }
    goto err0;
  }
  fileName = new GString(argv[1]);
  pngRoot = argv[2];

  // read config file
  globalParams = new GlobalParams(cfgFileName);
  globalParams->setupBaseFonts(NULL);
  if (enableFreeTypeStr[0]) {
    if (!globalParams->setEnableFreeType(enableFreeTypeStr)) {
      fprintf(stderr, "Bad '-freetype' value on command line\n");
    }
  }
  if (antialiasStr[0]) {
    if (!globalParams->setAntialias(antialiasStr)) {
      fprintf(stderr, "Bad '-aa' value on command line\n");
    }
  }
  if (vectorAntialiasStr[0]) {
    if (!globalParams->setVectorAntialias(vectorAntialiasStr)) {
      fprintf(stderr, "Bad '-aaVector' value on command line\n");
    }
  }
  if (quiet) {
    globalParams->setErrQuiet(quiet);
  }

  // open PDF file
  if (ownerPassword[0]) {
    ownerPW = new GString(ownerPassword);
  } else {
    ownerPW = NULL;
  }
  if (userPassword[0]) {
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
  if (firstPage < 1)
    firstPage = 1;
  if (lastPage < 1 || lastPage > doc->getNumPages())
    lastPage = doc->getNumPages();


  // write PNG files
  if (mono) {
    paperColor[0] = 0xff;
    splashOut = new SplashOutputDev(splashModeMono1, 1, gFalse, paperColor);
  } else if (gray) {
    paperColor[0] = 0xff;
    splashOut = new SplashOutputDev(splashModeMono8, 1, gFalse, paperColor);
  } else {
    paperColor[0] = paperColor[1] = paperColor[2] = 0xff;
    splashOut = new SplashOutputDev(splashModeRGB8, 1, gFalse, paperColor);
  }
  splashOut->startDoc(doc->getXRef());
  for (pg = firstPage; pg <= lastPage; ++pg) {
    doc->displayPage(splashOut, pg, resolution, resolution, 0,
		     gFalse, gTrue, gFalse);
    if (mono) {
      if (!strcmp(pngRoot, "-")) {
	f = stdout;
      } else {
	pngFile = GString::format("{0:s}-{1:06d}.png", pngRoot, pg);
	if (!(f = fopen(pngFile->getCString(), "wb"))) {
	  exit(2);
	}
	delete pngFile;
      }
      setupPNG(&png, &pngInfo, f,
	       1, PNG_COLOR_TYPE_GRAY, splashOut->getBitmap());
      writePNGData(png, splashOut->getBitmap());
      finishPNG(&png, &pngInfo);
      fclose(f);
    } else if (gray) {
      if (!strcmp(pngRoot, "-")) {
	f = stdout;
      } else {
	pngFile = GString::format("{0:s}-{1:06d}.png", pngRoot, pg);
	if (!(f = fopen(pngFile->getCString(), "wb"))) {
	  exit(2);
	}
	delete pngFile;
      }
      setupPNG(&png, &pngInfo, f,
	       8, PNG_COLOR_TYPE_GRAY, splashOut->getBitmap());
      writePNGData(png, splashOut->getBitmap());
      finishPNG(&png, &pngInfo);
      fclose(f);
    } else { // RGB
      if (!strcmp(pngRoot, "-")) {
	f = stdout;
      } else {
	pngFile = GString::format("{0:s}-{1:06d}.png", pngRoot, pg);
	if (!(f = fopen(pngFile->getCString(), "wb"))) {
	  exit(2);
	}
	delete pngFile;
      }
      setupPNG(&png, &pngInfo, f,
	       8, PNG_COLOR_TYPE_RGB, splashOut->getBitmap());
      writePNGData(png, splashOut->getBitmap());
      finishPNG(&png, &pngInfo);
      fclose(f);
    }
  }
  delete splashOut;

  exitCode = 0;

  // clean up
 err1:
  delete doc;
  delete globalParams;
 err0:

  // check for memory leaks
  Object::memCheck(stderr);
  gMemReport(stderr);

  return exitCode;
}

static void setupPNG(png_structp *png, png_infop *pngInfo, FILE *f,
		     int bitDepth, int colorType,
		     SplashBitmap *bitmap) {
  if (!(*png = png_create_write_struct(PNG_LIBPNG_VER_STRING,
				       NULL, NULL, NULL)) ||
      !(*pngInfo = png_create_info_struct(*png))) {
    exit(2);
  }
  if (setjmp(png_jmpbuf(*png))) {
    exit(2);
  }
  png_init_io(*png, f);
  png_set_IHDR(*png, *pngInfo, bitmap->getWidth(), bitmap->getHeight(),
	       bitDepth, colorType, PNG_INTERLACE_NONE,
	       PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
  png_write_info(*png, *pngInfo);
}

static void writePNGData(png_structp png, SplashBitmap *bitmap) {
  Guchar *p;
  int y;

  if (setjmp(png_jmpbuf(png))) {
    exit(2);
  }
  p = bitmap->getDataPtr();
  for (y = 0; y < bitmap->getHeight(); ++y) {
    png_write_row(png, (png_bytep)p);
    p += bitmap->getRowSize();
  }
}



static void finishPNG(png_structp *png, png_infop *pngInfo) {
  if (setjmp(png_jmpbuf(*png))) {
    exit(2);
  }
  png_write_end(*png, *pngInfo);
  png_destroy_write_struct(png, pngInfo);
}
