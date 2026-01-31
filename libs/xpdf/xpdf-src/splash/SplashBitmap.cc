//========================================================================
//
// SplashBitmap.cc
//
// Copyright 2003-2013 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#include <stdio.h>
#include <limits.h>
#include "gmem.h"
#include "gmempp.h"
#include "gfile.h"
#include "Trace.h"
#include "SplashErrorCodes.h"
#include "SplashBitmap.h"

//------------------------------------------------------------------------
// SplashBitmapMemCache
//------------------------------------------------------------------------

#define splashBitmapMemCacheThreshold 4000000

SplashBitmapMemCache::SplashBitmapMemCache() {
  for (int i = 0; i < splashBitmapMemCacheSize; ++i) {
    cache[i].data = NULL;
    cache[i].height = 0;
    cache[i].rowSize = 0;
  }
}

SplashBitmapMemCache::~SplashBitmapMemCache() {
  for (int i = 0; i < splashBitmapMemCacheSize; ++i) {
    if (cache[i].data) {
      gfree(cache[i].data);
      cache[i].data = NULL;
      cache[i].height = 0;
      cache[i].rowSize = 0;
    }
  }
}

void *SplashBitmapMemCache::alloc(int height, SplashBitmapRowSize rowSize) {
  // if the block is small, just allocate it
  if (rowSize < splashBitmapMemCacheThreshold / height) {
    return gmallocn64(height, rowSize);
  }

  // if a match is found in the cache, return it
  for (int i = splashBitmapMemCacheSize - 1; i >= 0; --i) {
    if (cache[i].data &&
	cache[i].height == height &&
	cache[i].rowSize == rowSize) {
      void *data = cache[i].data;
      for (int j = i; j < splashBitmapMemCacheSize - 1; ++j) {
	cache[j] = cache[j + 1];
      }
      cache[splashBitmapMemCacheSize - 1].data = NULL;
      cache[splashBitmapMemCacheSize - 1].height = 0;
      cache[splashBitmapMemCacheSize - 1].rowSize = 0;
      return data;
    }
  }

  // if no match was found, clear the cache and allcoate the block
  for (int i = 0; i < splashBitmapMemCacheSize; ++i) {
    if (cache[i].data) {
      gfree(cache[i].data);
      cache[i].data = NULL;
      cache[i].height = 0;
      cache[i].rowSize = 0;
    }
  }
  return gmallocn64(height, rowSize);
}

void SplashBitmapMemCache::free(void *data, int height,
				SplashBitmapRowSize rowSize) {
  if (rowSize < splashBitmapMemCacheThreshold / height) {
    gfree(data);
    return;
  }
  if (cache[0].data) {
    gfree(cache[0].data);
  }
  for (int i = 0; i < splashBitmapMemCacheSize - 1; ++i) {
    cache[i] = cache[i + 1];
  }
  cache[splashBitmapMemCacheSize - 1].data = data;
  cache[splashBitmapMemCacheSize - 1].height = height;
  cache[splashBitmapMemCacheSize - 1].rowSize = rowSize;
}

//------------------------------------------------------------------------
// SplashBitmap
//------------------------------------------------------------------------

SplashBitmap::SplashBitmap(int widthA, int heightA, int rowPad,
			   SplashColorMode modeA, GBool alphaA,
			   GBool topDown, SplashBitmapMemCache *cacheA) {
  // NB: this code checks that rowSize fits in a signed 32-bit
  // integer, because some code (outside this class) makes that
  // assumption
  width = widthA;
  height = heightA;
  mode = modeA;
  rowSize = 0; // make gcc happy
  switch (mode) {
  case splashModeMono1:
    if (width <= 0) {
      gMemError("invalid bitmap width");
    }
    rowSize = (width + 7) >> 3;
    break;
  case splashModeMono8:
    if (width <= 0) {
      gMemError("invalid bitmap width");
    }
    rowSize = width;
    break;
  case splashModeRGB8:
  case splashModeBGR8:
    if (width <= 0 || width > INT_MAX / 3) {
      gMemError("invalid bitmap width");
    }
    rowSize = (SplashBitmapRowSize)width * 3;
    break;
#if SPLASH_CMYK
  case splashModeCMYK8:
    if (width <= 0 || width > INT_MAX / 4) {
      gMemError("invalid bitmap width");
    }
    rowSize = (SplashBitmapRowSize)width * 4;
    break;
#endif
  }
  rowSize += rowPad - 1;
  rowSize -= rowSize % rowPad;

  traceAlloc(this, "alloc bitmap: %d x %d x %d %s -> %lld bytes",
	     width, height, splashColorModeNComps[mode],
	     alphaA ? "with alpha" : "without alpha",
	     height * rowSize + (alphaA ? height * width : 0));

  cache = cacheA;
  if (cache) {
    data = (SplashColorPtr)cache->alloc(height, rowSize);
  } else {
    data = (SplashColorPtr)gmallocn64(height, rowSize);
  }
  if (!topDown) {
    data += (height - 1) * rowSize;
    rowSize = -rowSize;
  }
  if (alphaA) {
    alphaRowSize = width;
    if (cache) {
      alpha = (Guchar *)cache->alloc(height, alphaRowSize);
    } else {
      alpha = (Guchar *)gmallocn64(height, alphaRowSize);
    }
  } else {
    alphaRowSize = 0;
    alpha = NULL;
  }
}

SplashBitmap::~SplashBitmap() {
  traceFree(this, "free bitmap");
  if (data && rowSize < 0) {
    rowSize = -rowSize;
    data -= (height - 1) * rowSize;
  }
  if (cache) {
    cache->free(data, height, rowSize);
    if (alpha) {
      cache->free(alpha, height, alphaRowSize);
    }
  } else {
    gfree(data);
    gfree(alpha);
  }
}

SplashError SplashBitmap::writePNMFile(char *fileName) {
  FILE *f;
  SplashError err;

  if (!(f = openFile(fileName, "wb"))) {
    return splashErrOpenFile;
  }
  err = writePNMFile(f);
  fclose(f);
  return err;
}

SplashError SplashBitmap::writePNMFile(FILE *f) {
  SplashColorPtr row, p;
  int x, y;

  switch (mode) {

  case splashModeMono1:
    fprintf(f, "P4\n%d %d\n", width, height);
    row = data;
    for (y = 0; y < height; ++y) {
      p = row;
      for (x = 0; x < width; x += 8) {
	fputc(*p ^ 0xff, f);
	++p;
      }
      row += rowSize;
    }
    break;

  case splashModeMono8:
    fprintf(f, "P5\n%d %d\n255\n", width, height);
    row = data;
    for (y = 0; y < height; ++y) {
      fwrite(row, 1, width, f);
      row += rowSize;
    }
    break;

  case splashModeRGB8:
    fprintf(f, "P6\n%d %d\n255\n", width, height);
    row = data;
    for (y = 0; y < height; ++y) {
      fwrite(row, 1, 3 * width, f);
      row += rowSize;
    }
    break;

  case splashModeBGR8:
    fprintf(f, "P6\n%d %d\n255\n", width, height);
    row = data;
    for (y = 0; y < height; ++y) {
      p = row;
      for (x = 0; x < width; ++x) {
	fputc(splashBGR8R(p), f);
	fputc(splashBGR8G(p), f);
	fputc(splashBGR8B(p), f);
	p += 3;
      }
      row += rowSize;
    }
    break;

#if SPLASH_CMYK
  case splashModeCMYK8:
    fprintf(f, "P7\n");
    fprintf(f, "WIDTH %d\n", width);
    fprintf(f, "HEIGHT %d\n", height);
    fprintf(f, "DEPTH 4\n");
    fprintf(f, "MAXVAL 255\n");
    fprintf(f, "TUPLTYPE CMYK\n");
    fprintf(f, "ENDHDR\n");
    row = data;
    for (y = 0; y < height; ++y) {
      fwrite(row, 1, 4 * width, f);
      row += rowSize;
    }
    break;
#endif

  }

  return splashOk;
}

SplashError SplashBitmap::writeAlphaPGMFile(char *fileName) {
  FILE *f;

  if (!alpha) {
    return splashErrModeMismatch;
  }
  if (!(f = openFile(fileName, "wb"))) {
    return splashErrOpenFile;
  }
  fprintf(f, "P5\n%d %d\n255\n", width, height);
  fwrite(alpha, 1, width * height, f);
  fclose(f);
  return splashOk;
}


void SplashBitmap::getPixel(int x, int y, SplashColorPtr pixel) {
  SplashColorPtr p;

  if (y < 0 || y >= height || x < 0 || x >= width) {
    return;
  }
  switch (mode) {
  case splashModeMono1:
    p = &data[y * rowSize + (x >> 3)];
    pixel[0] = (p[0] & (0x80 >> (x & 7))) ? 0xff : 0x00;
    break;
  case splashModeMono8:
    p = &data[y * rowSize + x];
    pixel[0] = p[0];
    break;
  case splashModeRGB8:
    p = &data[y * rowSize + 3 * x];
    pixel[0] = p[0];
    pixel[1] = p[1];
    pixel[2] = p[2];
    break;
  case splashModeBGR8:
    p = &data[y * rowSize + 3 * x];
    pixel[0] = p[2];
    pixel[1] = p[1];
    pixel[2] = p[0];
    break;
#if SPLASH_CMYK
  case splashModeCMYK8:
    p = &data[y * rowSize + 4 * x];
    pixel[0] = p[0];
    pixel[1] = p[1];
    pixel[2] = p[2];
    pixel[3] = p[3];
    break;
#endif
  }
}

Guchar SplashBitmap::getAlpha(int x, int y) {
  return alpha[y * (size_t)width + x];
}

SplashColorPtr SplashBitmap::takeData() {
  SplashColorPtr data2;

  data2 = data;
  data = NULL;
  return data2;
}

void SplashBitmap::doNotCache() {
  cache = NULL;
}


//------------------------------------------------------------------------
// SplashAlphaBitmap
//------------------------------------------------------------------------

SplashAlphaBitmap::SplashAlphaBitmap(int widthA, int heightA,
				     SplashBitmapMemCache *cacheA) {
  width = widthA;
  height = heightA;
  alphaRowSize = width;

  traceAlloc(this, "alloc alpha bitmap: %d x %d -> %lld bytes",
	     width, height, height * alphaRowSize);

  cache = cacheA;
  if (cache) {
    alpha = (Guchar *)cache->alloc(height, alphaRowSize);
  } else {
    alpha = (Guchar *)gmallocn64(height, alphaRowSize);
  }
}

SplashAlphaBitmap::~SplashAlphaBitmap() {
  traceFree(this, "free alpha bitmap");
  if (cache) {
    cache->free(alpha, height, alphaRowSize);
  } else {
    gfree(alpha);
  }
}
