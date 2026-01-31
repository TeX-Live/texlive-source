//========================================================================
//
// SplashBitmap.h
//
// Copyright 2003-2013 Glyph & Cog, LLC
//
//========================================================================

#ifndef SPLASHBITMAP_H
#define SPLASHBITMAP_H

#include <aconf.h>

#include <stdio.h>
#include <limits.h>
// older compilers won't define SIZE_MAX in stdint.h without this
#ifndef __STDC_LIMIT_MACROS
#  define __STDC_LIMIT_MACROS 1
#endif
#include <stdint.h>
#include "SplashTypes.h"

//------------------------------------------------------------------------

// ssize_t isn't well-defined, so define our own
#if SIZE_MAX == UINT_MAX
  typedef int SplashBitmapRowSize;
# define SplashBitmapRowSizeMax INT_MAX
#else
  typedef long long SplashBitmapRowSize;
# define SplashBitmapRowSizeMax LLONG_MAX
#endif

//------------------------------------------------------------------------
// SplashBitmapMemCache
//------------------------------------------------------------------------

#define splashBitmapMemCacheSize 4

struct SplashBitmapMemCacheEntry {
  void *data;
  int height;
  SplashBitmapRowSize rowSize;
};

// This holds onto a small number of freed bitmaps so they can be
// reused, avoiding system overhead for (re)allocating large chunks of
// memory. It's not thread-safe, so it needs to be owned by a single
// thread.
class SplashBitmapMemCache {
public:

  SplashBitmapMemCache();
  ~SplashBitmapMemCache();

private:

  void *alloc(int height, SplashBitmapRowSize rowSize);
  void free(void *data, int height, SplashBitmapRowSize rowSize);

  SplashBitmapMemCacheEntry cache[splashBitmapMemCacheSize];

  friend class SplashBitmap;
  friend class SplashAlphaBitmap;
};

//------------------------------------------------------------------------
// SplashBitmap
//------------------------------------------------------------------------

class SplashBitmap {
public:

  // Create a new bitmap.  It will have <widthA> x <heightA> pixels in
  // color mode <modeA>.  Rows will be padded out to a multiple of
  // <rowPad> bytes.  If <topDown> is false, the bitmap will be stored
  // upside-down, i.e., with the last row first in memory.
  SplashBitmap(int widthA, int heightA, int rowPad,
	       SplashColorMode modeA, GBool alphaA,
	       GBool topDown, SplashBitmapMemCache *cacheA);

  ~SplashBitmap();

  int getWidth() { return width; }
  int getHeight() { return height; }
  SplashBitmapRowSize getRowSize() { return rowSize; }
  size_t getAlphaRowSize() { return alphaRowSize; }
  SplashColorMode getMode() { return mode; }
  SplashColorPtr getDataPtr() { return data; }
  Guchar *getAlphaPtr() { return alpha; }

  SplashError writePNMFile(char *fileName);
  SplashError writePNMFile(FILE *f);
  SplashError writeAlphaPGMFile(char *fileName);

  void getPixel(int x, int y, SplashColorPtr pixel);
  Guchar getAlpha(int x, int y);

  // Caller takes ownership of the bitmap data.  The SplashBitmap
  // object is no longer valid -- the next call should be to the
  // destructor.
  SplashColorPtr takeData();

  void doNotCache();

private:

  int width, height;		// size of bitmap
  SplashBitmapRowSize rowSize;	// size of one row of data, in bytes
				//   - negative for bottom-up bitmaps
  size_t alphaRowSize;		// size of one row of alpha, in bytes
  SplashColorMode mode;		// color mode
  SplashColorPtr data;		// pointer to row zero of the color data
  Guchar *alpha;		// pointer to row zero of the alpha data
				//   (always top-down)

  SplashBitmapMemCache *cache;

  friend class Splash;
};


//------------------------------------------------------------------------
// SplashAlphaBitmap
//------------------------------------------------------------------------

class SplashAlphaBitmap {
public:

  // Create a new alpha bitmap with <widthA> x <heightA> pixels.
  SplashAlphaBitmap(int widthA, int heightA, SplashBitmapMemCache *cacheA);

  ~SplashAlphaBitmap();

  int getWidth() { return width; }
  int getHeight() { return height; }
  size_t getAlphaRowSize() { return alphaRowSize; }
  Guchar *getAlphaPtr() { return alpha; }

private:

  int width, height;		// size of bitmap
  size_t alphaRowSize;		// size of one row, in bytes
  Guchar *alpha;		// pointer to the alpha data

  SplashBitmapMemCache *cache;

  friend class Splash;
};

#endif
