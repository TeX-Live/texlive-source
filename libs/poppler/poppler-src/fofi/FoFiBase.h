//========================================================================
//
// FoFiBase.h
//
// Copyright 1999-2003 Glyph & Cog, LLC
//
//========================================================================

//========================================================================
//
// Modified under the Poppler project - http://poppler.freedesktop.org
//
// All changes made under the Poppler project to this file are licensed
// under GPL version 2 or later
//
// Copyright (C) 2018 Albert Astals Cid <aacid@kde.org>
//
// To see a description of the changes please see the Changelog file that
// came with your tarball or type make ChangeLog if you are building from git
//
//========================================================================

#ifndef FOFIBASE_H
#define FOFIBASE_H

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "goo/gtypes.h"

//------------------------------------------------------------------------

typedef void (*FoFiOutputFunc)(void *stream, const char *data, int len);

//------------------------------------------------------------------------
// FoFiBase
//------------------------------------------------------------------------

class FoFiBase {
public:
  FoFiBase(const FoFiBase &) = delete;
  FoFiBase& operator=(const FoFiBase &other) = delete;

  virtual ~FoFiBase();

protected:

  FoFiBase(char *fileA, int lenA, GBool freeFileDataA);
  static char *readFile(char *fileName, int *fileLen);

  // S = signed / U = unsigned
  // 8/16/32/Var = word length, in bytes
  // BE = big endian
  int getS8(int pos, GBool *ok);
  int getU8(int pos, GBool *ok);
  int getS16BE(int pos, GBool *ok);
  int getU16BE(int pos, GBool *ok);
  int getS32BE(int pos, GBool *ok);
  Guint getU32BE(int pos, GBool *ok);
  Guint getU32LE(int pos, GBool *ok);
  Guint getUVarBE(int pos, int size, GBool *ok);

  GBool checkRegion(int pos, int size);

  Guchar *fileData;
  Guchar *file;
  int len;
  GBool freeFileData;
};

#endif
