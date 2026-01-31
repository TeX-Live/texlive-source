//========================================================================
//
// pdfimages.cc
//
// Copyright 1998-2003 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "gmem.h"
#include "gmempp.h"
#include "parseargs.h"
#include "GString.h"
#include "GlobalParams.h"
#include "Object.h"
#include "Stream.h"
#include "Array.h"
#include "Dict.h"
#include "XRef.h"
#include "Catalog.h"
#include "Page.h"
#include "PDFDoc.h"
#include "ImageOutputDev.h"
#include "Error.h"
#include "config.h"

static int firstPage = 1;
static int lastPage = 0;
static GBool dumpJPEG = gFalse;
static GBool dumpJPX = gFalse;
static GBool dumpRaw = gFalse;
static GBool unique = gFalse;
static GBool list = gFalse;
static GBool listOnly = gFalse;
static char ownerPassword[33] = "\001";
static char userPassword[33] = "\001";
static GBool verbose = gFalse;
static GBool quiet = gFalse;
static char cfgFileName[256] = "";
static GBool printVersion = gFalse;
static GBool printHelp = gFalse;

static ArgDesc argDesc[] = {
  {"-f",      argInt,      &firstPage,     0,
   "first page to convert"},
  {"-l",      argInt,      &lastPage,      0,
   "last page to convert"},
  {"-j",      argFlag,     &dumpJPEG,      0,
   "write JPEG images as JPEG files"},
  {"-J",      argFlag,     &dumpJPX,       0,
   "write JPEG 2000 images as JP2 files"},
  {"-raw",    argFlag,     &dumpRaw,       0,
   "write raw data in PDF-native formats"},
  {"-u",      argFlag,     &unique,        0,
   "write only one copy of each unique image"},
  {"-list",   argFlag,     &list,          0,
   "write information to stdout for each image"},
  {"-listonly", argFlag,   &listOnly,      0,
   "only write image information to stdout; no image-root arg needed"},
  {"-opw",    argString,   ownerPassword,  sizeof(ownerPassword),
   "owner password (for encrypted files)"},
  {"-upw",    argString,   userPassword,   sizeof(userPassword),
   "user password (for encrypted files)"},
  {"-verbose", argFlag,    &verbose,       0,
   "print per-page status information"},
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

int main(int argc, char *argv[]) {
#if USE_EXCEPTIONS
  try {
#endif

  PDFDoc *doc;
  char *fileName;
  char *imgRoot;
  GString *ownerPW, *userPW;
  ImageOutputDev *imgOut;
  GBool ok;
  int exitCode;

  exitCode = 99;

  // parse args
  fixCommandLine(&argc, &argv);
  ok = parseArgs(argDesc, &argc, argv);
  if (printVersion) {
    printf("pdfimages version %s [www.xpdfreader.com]\n", xpdfVersion);
    printf("%s\n", xpdfCopyright);
    goto err0;
  }
  if (!ok || argc != (listOnly ? 2 : 3) || printHelp) {
    fprintf(stderr, "pdfimages version %s [www.xpdfreader.com]\n", xpdfVersion);
    fprintf(stderr, "%s\n", xpdfCopyright);
    printUsage("pdfimages", "<PDF-file> <image-root>", argDesc);
    goto err0;
  }
  fileName = argv[1];
  imgRoot = listOnly ? NULL : argv[2];

  // read config file
  if (cfgFileName[0] && !pathIsFile(cfgFileName)) {
    error(errConfig, -1, "Config file '{0:s}' doesn't exist or isn't a file",
	  cfgFileName);
  }
  globalParams = new GlobalParams(cfgFileName);
  if (verbose) {
    globalParams->setPrintStatusInfo(verbose);
  }
  if (quiet) {
    globalParams->setErrQuiet(quiet);
  }

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

  // check for copy permission
  if (!listOnly && !doc->okToCopy()) {
    error(errNotAllowed, -1,
	  "Copying of images from this document is not allowed.");
    exitCode = 3;
    goto err1;
  }

  // get page range
  if (firstPage < 1)
    firstPage = 1;
  if (lastPage < 1 || lastPage > doc->getNumPages())
    lastPage = doc->getNumPages();

  // write image files
  imgOut = new ImageOutputDev(imgRoot, dumpJPEG, dumpJPX, dumpRaw, unique,
			      list, listOnly);
  if (imgOut->isOk()) {
    imgOut->startDoc(doc->getXRef());
    doc->displayPages(imgOut, NULL, firstPage, lastPage, 72, 72, 0,
		      gFalse, gTrue, gFalse);
  }
  delete imgOut;

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

#if USE_EXCEPTIONS
  } catch (GMemException e) {
    fprintf(stderr, "Out of memory\n");
    return 98;
  }
#endif
}
