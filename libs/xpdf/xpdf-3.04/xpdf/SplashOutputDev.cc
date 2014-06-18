//========================================================================
//
// SplashOutputDev.cc
//
// Copyright 2003-2013 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <string.h>
#include <math.h>
#include <limits.h>
#include "gfile.h"
#include "GlobalParams.h"
#include "Error.h"
#include "Object.h"
#include "Gfx.h"
#include "GfxFont.h"
#include "Link.h"
#include "CharCodeToUnicode.h"
#include "FontEncodingTables.h"
#include "BuiltinFont.h"
#include "BuiltinFontTables.h"
#include "FoFiTrueType.h"
#include "JPXStream.h"
#include "SplashBitmap.h"
#include "SplashGlyphBitmap.h"
#include "SplashPattern.h"
#include "SplashScreen.h"
#include "SplashPath.h"
#include "SplashState.h"
#include "SplashErrorCodes.h"
#include "SplashFontEngine.h"
#include "SplashFont.h"
#include "SplashFontFile.h"
#include "SplashFontFileID.h"
#include "Splash.h"
#include "SplashOutputDev.h"

#ifdef VMS
#if (__VMS_VER < 70000000)
extern "C" int unlink(char *filename);
#endif
#endif

//------------------------------------------------------------------------

// Type 3 font cache size parameters
#define type3FontCacheAssoc   8
#define type3FontCacheMaxSets 8
#define type3FontCacheSize    (128*1024)

//------------------------------------------------------------------------

// Divide a 16-bit value (in [0, 255*255]) by 255, returning an 8-bit result.
static inline Guchar div255(int x) {
  return (Guchar)((x + (x >> 8) + 0x80) >> 8);
}

//------------------------------------------------------------------------
// Blend functions
//------------------------------------------------------------------------

static void splashOutBlendMultiply(SplashColorPtr src, SplashColorPtr dest,
				   SplashColorPtr blend, SplashColorMode cm) {
  int i;

  for (i = 0; i < splashColorModeNComps[cm]; ++i) {
    blend[i] = (dest[i] * src[i]) / 255;
  }
}

static void splashOutBlendScreen(SplashColorPtr src, SplashColorPtr dest,
				 SplashColorPtr blend, SplashColorMode cm) {
  int i;

  for (i = 0; i < splashColorModeNComps[cm]; ++i) {
    blend[i] = dest[i] + src[i] - (dest[i] * src[i]) / 255;
  }
}

static void splashOutBlendOverlay(SplashColorPtr src, SplashColorPtr dest,
				  SplashColorPtr blend, SplashColorMode cm) {
  int i;

  for (i = 0; i < splashColorModeNComps[cm]; ++i) {
    blend[i] = dest[i] < 0x80
                 ? (src[i] * 2 * dest[i]) / 255
                 : 255 - 2 * ((255 - src[i]) * (255 - dest[i])) / 255;
  }
}

static void splashOutBlendDarken(SplashColorPtr src, SplashColorPtr dest,
				 SplashColorPtr blend, SplashColorMode cm) {
  int i;

  for (i = 0; i < splashColorModeNComps[cm]; ++i) {
    blend[i] = dest[i] < src[i] ? dest[i] : src[i];
  }
}

static void splashOutBlendLighten(SplashColorPtr src, SplashColorPtr dest,
				  SplashColorPtr blend, SplashColorMode cm) {
  int i;

  for (i = 0; i < splashColorModeNComps[cm]; ++i) {
    blend[i] = dest[i] > src[i] ? dest[i] : src[i];
  }
}

static void splashOutBlendColorDodge(SplashColorPtr src, SplashColorPtr dest,
				     SplashColorPtr blend,
				     SplashColorMode cm) {
  int i, x;

  for (i = 0; i < splashColorModeNComps[cm]; ++i) {
    if (dest[i] == 0) {
      blend[i] = 0;
    } else if (src[i] == 255) {
      blend[i] = 255;
    } else {
      x = (dest[i] * 255) / (255 - src[i]);
      blend[i] = x <= 255 ? x : 255;
    }
  }
}

static void splashOutBlendColorBurn(SplashColorPtr src, SplashColorPtr dest,
				    SplashColorPtr blend, SplashColorMode cm) {
  int i, x;

  for (i = 0; i < splashColorModeNComps[cm]; ++i) {
    if (dest[i] == 255) {
      blend[i] = 255;
    } else if (src[i] == 0) {
      blend[i] = 0;
    } else {
      x = ((255 - dest[i]) * 255) / src[i];
      blend[i] = x <= 255 ? 255 - x : 0;
    }
  }
}

static void splashOutBlendHardLight(SplashColorPtr src, SplashColorPtr dest,
				    SplashColorPtr blend, SplashColorMode cm) {
  int i;

  for (i = 0; i < splashColorModeNComps[cm]; ++i) {
    blend[i] = src[i] < 0x80
                 ? (dest[i] * 2 * src[i]) / 255
                 : 255 - 2 * ((255 - dest[i]) * (255 - src[i])) / 255;
  }
}

static void splashOutBlendSoftLight(SplashColorPtr src, SplashColorPtr dest,
				    SplashColorPtr blend, SplashColorMode cm) {
  int i, x;

  for (i = 0; i < splashColorModeNComps[cm]; ++i) {
    if (src[i] < 0x80) {
      blend[i] = dest[i] - (255 - 2 * src[i]) * dest[i] * (255 - dest[i]) /
	         (255 * 255);
    } else {
      if (dest[i] < 0x40) {
	x = (((((16 * dest[i] - 12 * 255) * dest[i]) / 255)
	      + 4 * 255) * dest[i]) / 255;
      } else {
	x = (int)sqrt(255.0 * dest[i]);
      }
      blend[i] = dest[i] + (2 * src[i] - 255) * (x - dest[i]) / 255;
    }
  }
}

static void splashOutBlendDifference(SplashColorPtr src, SplashColorPtr dest,
				     SplashColorPtr blend,
				     SplashColorMode cm) {
  int i;

  for (i = 0; i < splashColorModeNComps[cm]; ++i) {
    blend[i] = dest[i] < src[i] ? src[i] - dest[i] : dest[i] - src[i];
  }
}

static void splashOutBlendExclusion(SplashColorPtr src, SplashColorPtr dest,
				    SplashColorPtr blend, SplashColorMode cm) {
  int i;

  for (i = 0; i < splashColorModeNComps[cm]; ++i) {
    blend[i] = dest[i] + src[i] - (2 * dest[i] * src[i]) / 255;
  }
}

static int getLum(int r, int g, int b) {
  return (int)(0.3 * r + 0.59 * g + 0.11 * b);
}

static int getSat(int r, int g, int b) {
  int rgbMin, rgbMax;

  rgbMin = rgbMax = r;
  if (g < rgbMin) {
    rgbMin = g;
  } else if (g > rgbMax) {
    rgbMax = g;
  }
  if (b < rgbMin) {
    rgbMin = b;
  } else if (b > rgbMax) {
    rgbMax = b;
  }
  return rgbMax - rgbMin;
}

static void clipColor(int rIn, int gIn, int bIn,
		      Guchar *rOut, Guchar *gOut, Guchar *bOut) {
  int lum, rgbMin, rgbMax;

  lum = getLum(rIn, gIn, bIn);
  rgbMin = rgbMax = rIn;
  if (gIn < rgbMin) {
    rgbMin = gIn;
  } else if (gIn > rgbMax) {
    rgbMax = gIn;
  }
  if (bIn < rgbMin) {
    rgbMin = bIn;
  } else if (bIn > rgbMax) {
    rgbMax = bIn;
  }
  if (rgbMin < 0) {
    *rOut = (Guchar)(lum + ((rIn - lum) * lum) / (lum - rgbMin));
    *gOut = (Guchar)(lum + ((gIn - lum) * lum) / (lum - rgbMin));
    *bOut = (Guchar)(lum + ((bIn - lum) * lum) / (lum - rgbMin));
  } else if (rgbMax > 255) {
    *rOut = (Guchar)(lum + ((rIn - lum) * (255 - lum)) / (rgbMax - lum));
    *gOut = (Guchar)(lum + ((gIn - lum) * (255 - lum)) / (rgbMax - lum));
    *bOut = (Guchar)(lum + ((bIn - lum) * (255 - lum)) / (rgbMax - lum));
  } else {
    *rOut = rIn;
    *gOut = gIn;
    *bOut = bIn;
  }
}

static void setLum(Guchar rIn, Guchar gIn, Guchar bIn, int lum,
		   Guchar *rOut, Guchar *gOut, Guchar *bOut) {
  int d;

  d = lum - getLum(rIn, gIn, bIn);
  clipColor(rIn + d, gIn + d, bIn + d, rOut, gOut, bOut);
}

static void setSat(Guchar rIn, Guchar gIn, Guchar bIn, int sat,
		   Guchar *rOut, Guchar *gOut, Guchar *bOut) {
  int rgbMin, rgbMid, rgbMax;
  Guchar *minOut, *midOut, *maxOut;

  if (rIn < gIn) {
    rgbMin = rIn;  minOut = rOut;
    rgbMid = gIn;  midOut = gOut;
  } else {
    rgbMin = gIn;  minOut = gOut;
    rgbMid = rIn;  midOut = rOut;
  }
  if (bIn > rgbMid) {
    rgbMax = bIn;  maxOut = bOut;
  } else if (bIn > rgbMin) {
    rgbMax = rgbMid;  maxOut = midOut;
    rgbMid = bIn;     midOut = bOut;
  } else {
    rgbMax = rgbMid;  maxOut = midOut;
    rgbMid = rgbMin;  midOut = minOut;
    rgbMin = bIn;     minOut = bOut;
  }
  if (rgbMax > rgbMin) {
    *midOut = (Guchar)((rgbMid - rgbMin) * sat) / (rgbMax - rgbMin);
    *maxOut = (Guchar)sat;
  } else {
    *midOut = *maxOut = 0;
  }
  *minOut = 0;
}

static void splashOutBlendHue(SplashColorPtr src, SplashColorPtr dest,
			      SplashColorPtr blend, SplashColorMode cm) {
  Guchar r0, g0, b0;
#if SPLASH_CMYK
  Guchar r1, g1, b1;
#endif

  switch (cm) {
  case splashModeMono1:
  case splashModeMono8:
    blend[0] = dest[0];
    break;
  case splashModeRGB8:
  case splashModeBGR8:
    setSat(src[0], src[1], src[2], getSat(dest[0], dest[1], dest[2]),
	   &r0, &g0, &b0);
    setLum(r0, g0, b0, getLum(dest[0], dest[1], dest[2]),
	   &blend[0], &blend[1], &blend[2]);
    break;
#if SPLASH_CMYK
  case splashModeCMYK8:
    // NB: inputs have already been converted to additive mode
    setSat(src[0], src[1], src[2], getSat(dest[0], dest[1], dest[2]),
	   &r0, &g0, &b0);
    setLum(r0, g0, b0, getLum(dest[0], dest[1], dest[2]),
	   &r1, &g1, &b1);
    blend[0] = r1;
    blend[1] = g1;
    blend[2] = b1;
    blend[3] = dest[3];
    break;
#endif
  }
}

static void splashOutBlendSaturation(SplashColorPtr src, SplashColorPtr dest,
				     SplashColorPtr blend,
				     SplashColorMode cm) {
  Guchar r0, g0, b0;
#if SPLASH_CMYK
  Guchar r1, g1, b1;
#endif

  switch (cm) {
  case splashModeMono1:
  case splashModeMono8:
    blend[0] = dest[0];
    break;
  case splashModeRGB8:
  case splashModeBGR8:
    setSat(dest[0], dest[1], dest[2], getSat(src[0], src[1], src[2]),
	   &r0, &g0, &b0);
    setLum(r0, g0, b0, getLum(dest[0], dest[1], dest[2]),
	   &blend[0], &blend[1], &blend[2]);
    break;
#if SPLASH_CMYK
  case splashModeCMYK8:
    // NB: inputs have already been converted to additive mode
    setSat(dest[0], dest[1], dest[2], getSat(src[0], src[1], src[2]),
	   &r0, &g0, &b0);
    setLum(r0, g0, b0, getLum(dest[0], dest[1], dest[2]),
	   &r1, &g1, &b1);
    blend[0] = r1;
    blend[1] = g1;
    blend[2] = b1;
    blend[3] = dest[3];
    break;
#endif
  }
}

static void splashOutBlendColor(SplashColorPtr src, SplashColorPtr dest,
				SplashColorPtr blend, SplashColorMode cm) {
#if SPLASH_CMYK
  Guchar r, g, b;
#endif

  switch (cm) {
  case splashModeMono1:
  case splashModeMono8:
    blend[0] = dest[0];
    break;
  case splashModeRGB8:
  case splashModeBGR8:
    setLum(src[0], src[1], src[2], getLum(dest[0], dest[1], dest[2]),
	   &blend[0], &blend[1], &blend[2]);
    break;
#if SPLASH_CMYK
  case splashModeCMYK8:
    // NB: inputs have already been converted to additive mode
    setLum(src[0], src[1], src[2], getLum(dest[0], dest[1], dest[2]),
	   &r, &g, &b);
    blend[0] = r;
    blend[1] = g;
    blend[2] = b;
    blend[3] = dest[3];
    break;
#endif
  }
}

static void splashOutBlendLuminosity(SplashColorPtr src, SplashColorPtr dest,
				     SplashColorPtr blend,
				     SplashColorMode cm) {
#if SPLASH_CMYK
  Guchar r, g, b;
#endif

  switch (cm) {
  case splashModeMono1:
  case splashModeMono8:
    blend[0] = dest[0];
    break;
  case splashModeRGB8:
  case splashModeBGR8:
    setLum(dest[0], dest[1], dest[2], getLum(src[0], src[1], src[2]),
	   &blend[0], &blend[1], &blend[2]);
    break;
#if SPLASH_CMYK
  case splashModeCMYK8:
    // NB: inputs have already been converted to additive mode
    setLum(dest[0], dest[1], dest[2], getLum(src[0], src[1], src[2]),
	   &r, &g, &b);
    blend[0] = r;
    blend[1] = g;
    blend[2] = b;
    blend[3] = src[3];
    break;
#endif
  }
}

// NB: This must match the GfxBlendMode enum defined in GfxState.h.
SplashBlendFunc splashOutBlendFuncs[] = {
  NULL,
  &splashOutBlendMultiply,
  &splashOutBlendScreen,
  &splashOutBlendOverlay,
  &splashOutBlendDarken,
  &splashOutBlendLighten,
  &splashOutBlendColorDodge,
  &splashOutBlendColorBurn,
  &splashOutBlendHardLight,
  &splashOutBlendSoftLight,
  &splashOutBlendDifference,
  &splashOutBlendExclusion,
  &splashOutBlendHue,
  &splashOutBlendSaturation,
  &splashOutBlendColor,
  &splashOutBlendLuminosity
};

//------------------------------------------------------------------------
// SplashOutFontFileID
//------------------------------------------------------------------------

class SplashOutFontFileID: public SplashFontFileID {
public:

  SplashOutFontFileID(Ref *rA) {
    r = *rA;
    substIdx = -1;
    oblique = 0;
  }

  ~SplashOutFontFileID() {}

  GBool matches(SplashFontFileID *id) {
    return ((SplashOutFontFileID *)id)->r.num == r.num &&
           ((SplashOutFontFileID *)id)->r.gen == r.gen;
  }

  void setOblique(double obliqueA) { oblique = obliqueA; }
  double getOblique() { return oblique; }
  void setSubstIdx(int substIdxA) { substIdx = substIdxA; }
  int getSubstIdx() { return substIdx; }

private:

  Ref r;
  double oblique;
  int substIdx;
};

//------------------------------------------------------------------------
// T3FontCache
//------------------------------------------------------------------------

struct T3FontCacheTag {
  Gushort code;
  Gushort mru;			// valid bit (0x8000) and MRU index
};

class T3FontCache {
public:

  T3FontCache(Ref *fontID, double m11A, double m12A,
	      double m21A, double m22A,
	      int glyphXA, int glyphYA, int glyphWA, int glyphHA,
	      GBool aa, GBool validBBoxA);
  ~T3FontCache();
  GBool matches(Ref *idA, double m11A, double m12A,
		double m21A, double m22A)
    { return fontID.num == idA->num && fontID.gen == idA->gen &&
	     m11 == m11A && m12 == m12A && m21 == m21A && m22 == m22A; }

  Ref fontID;			// PDF font ID
  double m11, m12, m21, m22;	// transform matrix
  int glyphX, glyphY;		// pixel offset of glyph bitmaps
  int glyphW, glyphH;		// size of glyph bitmaps, in pixels
  GBool validBBox;		// false if the bbox was [0 0 0 0]
  int glyphSize;		// size of glyph bitmaps, in bytes
  int cacheSets;		// number of sets in cache
  int cacheAssoc;		// cache associativity (glyphs per set)
  Guchar *cacheData;		// glyph pixmap cache
  T3FontCacheTag *cacheTags;	// cache tags, i.e., char codes
};

T3FontCache::T3FontCache(Ref *fontIDA, double m11A, double m12A,
			 double m21A, double m22A,
			 int glyphXA, int glyphYA, int glyphWA, int glyphHA,
			 GBool validBBoxA, GBool aa) {
  int i;

  fontID = *fontIDA;
  m11 = m11A;
  m12 = m12A;
  m21 = m21A;
  m22 = m22A;
  glyphX = glyphXA;
  glyphY = glyphYA;
  glyphW = glyphWA;
  glyphH = glyphHA;
  validBBox = validBBoxA;
  // sanity check for excessively large glyphs (which most likely
  // indicate an incorrect BBox)
  i = glyphW * glyphH;
  if (i > 100000 || glyphW > INT_MAX / glyphH || glyphW <= 0 || glyphH <= 0) {
    glyphW = glyphH = 100;
    validBBox = gFalse;
  }
  if (aa) {
    glyphSize = glyphW * glyphH;
  } else {
    glyphSize = ((glyphW + 7) >> 3) * glyphH;
  }
  cacheAssoc = type3FontCacheAssoc;
  for (cacheSets = type3FontCacheMaxSets;
       cacheSets > 1 &&
	 cacheSets * cacheAssoc * glyphSize > type3FontCacheSize;
       cacheSets >>= 1) ;
  cacheData = (Guchar *)gmallocn(cacheSets * cacheAssoc, glyphSize);
  cacheTags = (T3FontCacheTag *)gmallocn(cacheSets * cacheAssoc,
					 sizeof(T3FontCacheTag));
  for (i = 0; i < cacheSets * cacheAssoc; ++i) {
    cacheTags[i].mru = i & (cacheAssoc - 1);
  }
}

T3FontCache::~T3FontCache() {
  gfree(cacheData);
  gfree(cacheTags);
}

struct T3GlyphStack {
  Gushort code;			// character code

  GBool haveDx;			// set after seeing a d0/d1 operator
  GBool doNotCache;		// set if we see a gsave/grestore before
				//   the d0/d1

  //----- cache info
  T3FontCache *cache;		// font cache for the current font
  T3FontCacheTag *cacheTag;	// pointer to cache tag for the glyph
  Guchar *cacheData;		// pointer to cache data for the glyph

  //----- saved state
  SplashBitmap *origBitmap;
  Splash *origSplash;
  double origCTM4, origCTM5;

  T3GlyphStack *next;		// next object on stack
};

//------------------------------------------------------------------------
// SplashTransparencyGroup
//------------------------------------------------------------------------

struct SplashTransparencyGroup {
  int tx, ty;			// translation coordinates
  SplashBitmap *tBitmap;	// bitmap for transparency group
  GfxColorSpace *blendingColorSpace;
  GBool isolated;

  //----- saved state
  SplashBitmap *origBitmap;
  Splash *origSplash;

  SplashTransparencyGroup *next;
};

//------------------------------------------------------------------------
// SplashOutputDev
//------------------------------------------------------------------------

SplashOutputDev::SplashOutputDev(SplashColorMode colorModeA,
				 int bitmapRowPadA,
				 GBool reverseVideoA,
				 SplashColorPtr paperColorA,
				 GBool bitmapTopDownA,
				 GBool allowAntialiasA) {
  colorMode = colorModeA;
  bitmapRowPad = bitmapRowPadA;
  bitmapTopDown = bitmapTopDownA;
  bitmapUpsideDown = gFalse;
  noComposite = gFalse;
  allowAntialias = allowAntialiasA;
  vectorAntialias = allowAntialias &&
		      globalParams->getVectorAntialias() &&
		      colorMode != splashModeMono1;
  setupScreenParams(72.0, 72.0);
  reverseVideo = reverseVideoA;
  splashColorCopy(paperColor, paperColorA);
  skipHorizText = gFalse;
  skipRotatedText = gFalse;

  xref = NULL;

  bitmap = new SplashBitmap(1, 1, bitmapRowPad, colorMode,
			    colorMode != splashModeMono1, bitmapTopDown);
  splash = new Splash(bitmap, vectorAntialias, &screenParams);
  splash->setMinLineWidth(globalParams->getMinLineWidth());
  splash->setStrokeAdjust(globalParams->getStrokeAdjust());
  splash->clear(paperColor, 0);

  fontEngine = NULL;

  nT3Fonts = 0;
  t3GlyphStack = NULL;

  font = NULL;
  needFontUpdate = gFalse;
  textClipPath = NULL;

  transpGroupStack = NULL;

  nestCount = 0;
}

void SplashOutputDev::setupScreenParams(double hDPI, double vDPI) {
  screenParams.size = globalParams->getScreenSize();
  screenParams.dotRadius = globalParams->getScreenDotRadius();
  screenParams.gamma = (SplashCoord)globalParams->getScreenGamma();
  screenParams.blackThreshold =
      (SplashCoord)globalParams->getScreenBlackThreshold();
  screenParams.whiteThreshold =
      (SplashCoord)globalParams->getScreenWhiteThreshold();
  switch (globalParams->getScreenType()) {
  case screenDispersed:
    screenParams.type = splashScreenDispersed;
    if (screenParams.size < 0) {
      screenParams.size = 4;
    }
    break;
  case screenClustered:
    screenParams.type = splashScreenClustered;
    if (screenParams.size < 0) {
      screenParams.size = 10;
    }
    break;
  case screenStochasticClustered:
    screenParams.type = splashScreenStochasticClustered;
    if (screenParams.size < 0) {
      screenParams.size = 64;
    }
    if (screenParams.dotRadius < 0) {
      screenParams.dotRadius = 2;
    }
    break;
  case screenUnset:
  default:
    // use clustered dithering for resolution >= 300 dpi
    // (compare to 299.9 to avoid floating point issues)
    if (hDPI > 299.9 && vDPI > 299.9) {
      screenParams.type = splashScreenStochasticClustered;
      if (screenParams.size < 0) {
	screenParams.size = 64;
      }
      if (screenParams.dotRadius < 0) {
	screenParams.dotRadius = 2;
      }
    } else {
      screenParams.type = splashScreenDispersed;
      if (screenParams.size < 0) {
	screenParams.size = 4;
      }
    }
  }
}

SplashOutputDev::~SplashOutputDev() {
  int i;

  for (i = 0; i < nT3Fonts; ++i) {
    delete t3FontCache[i];
  }
  if (fontEngine) {
    delete fontEngine;
  }
  if (splash) {
    delete splash;
  }
  if (bitmap) {
    delete bitmap;
  }
}

void SplashOutputDev::startDoc(XRef *xrefA) {
  int i;

  xref = xrefA;
  if (fontEngine) {
    delete fontEngine;
  }
  fontEngine = new SplashFontEngine(
#if HAVE_FREETYPE_FREETYPE_H || HAVE_FREETYPE_H
				    globalParams->getEnableFreeType(),
				    globalParams->getDisableFreeTypeHinting()
				      ? splashFTNoHinting : 0,
#endif
				    allowAntialias &&
				      globalParams->getAntialias() &&
				      colorMode != splashModeMono1);
  for (i = 0; i < nT3Fonts; ++i) {
    delete t3FontCache[i];
  }
  nT3Fonts = 0;
}

void SplashOutputDev::startPage(int pageNum, GfxState *state) {
  int w, h;
  double *ctm;
  SplashCoord mat[6];
  SplashColor color;

  if (state) {
    setupScreenParams(state->getHDPI(), state->getVDPI());
    w = (int)(state->getPageWidth() + 0.5);
    if (w <= 0) {
      w = 1;
    }
    h = (int)(state->getPageHeight() + 0.5);
    if (h <= 0) {
      h = 1;
    }
  } else {
    w = h = 1;
  }
  if (splash) {
    delete splash;
    splash = NULL;
  }
  if (!bitmap || w != bitmap->getWidth() || h != bitmap->getHeight()) {
    if (bitmap) {
      delete bitmap;
      bitmap = NULL;
    }
    bitmap = new SplashBitmap(w, h, bitmapRowPad, colorMode,
			      colorMode != splashModeMono1, bitmapTopDown);
  }
  splash = new Splash(bitmap, vectorAntialias, &screenParams);
  splash->setMinLineWidth(globalParams->getMinLineWidth());
  if (state) {
    ctm = state->getCTM();
    mat[0] = (SplashCoord)ctm[0];
    mat[1] = (SplashCoord)ctm[1];
    mat[2] = (SplashCoord)ctm[2];
    mat[3] = (SplashCoord)ctm[3];
    mat[4] = (SplashCoord)ctm[4];
    mat[5] = (SplashCoord)ctm[5];
    splash->setMatrix(mat);
  }
  switch (colorMode) {
  case splashModeMono1:
  case splashModeMono8:
    color[0] = 0;
    break;
  case splashModeRGB8:
  case splashModeBGR8:
    color[0] = color[1] = color[2] = 0;
    break;
#if SPLASH_CMYK
  case splashModeCMYK8:
    color[0] = color[1] = color[2] = color[3] = 0;
    break;
#endif
  }
  splash->setStrokePattern(new SplashSolidColor(color));
  splash->setFillPattern(new SplashSolidColor(color));
  splash->setLineCap(splashLineCapButt);
  splash->setLineJoin(splashLineJoinMiter);
  splash->setLineDash(NULL, 0, 0);
  splash->setMiterLimit(10);
  splash->setFlatness(1);
  // the SA parameter supposedly defaults to false, but Acrobat
  // apparently hardwires it to true
  splash->setStrokeAdjust(globalParams->getStrokeAdjust());
  splash->clear(paperColor, 0);
}

void SplashOutputDev::endPage() {
  if (colorMode != splashModeMono1 && !noComposite) {
    splash->compositeBackground(paperColor);
  }
}

void SplashOutputDev::saveState(GfxState *state) {
  splash->saveState();
  if (t3GlyphStack && !t3GlyphStack->haveDx) {
    t3GlyphStack->doNotCache = gTrue;
    error(errSyntaxWarning, -1,
	  "Save (q) operator before d0/d1 in Type 3 glyph");
  }
}

void SplashOutputDev::restoreState(GfxState *state) {
  splash->restoreState();
  needFontUpdate = gTrue;
  if (t3GlyphStack && !t3GlyphStack->haveDx) {
    t3GlyphStack->doNotCache = gTrue;
    error(errSyntaxWarning, -1,
	  "Restore (Q) operator before d0/d1 in Type 3 glyph");
  }
}

void SplashOutputDev::updateAll(GfxState *state) {
  updateLineDash(state);
  updateLineJoin(state);
  updateLineCap(state);
  updateLineWidth(state);
  updateFlatness(state);
  updateMiterLimit(state);
  updateStrokeAdjust(state);
  updateFillColor(state);
  updateStrokeColor(state);
  needFontUpdate = gTrue;
}

void SplashOutputDev::updateCTM(GfxState *state, double m11, double m12,
				double m21, double m22,
				double m31, double m32) {
  double *ctm;
  SplashCoord mat[6];

  ctm = state->getCTM();
  mat[0] = (SplashCoord)ctm[0];
  mat[1] = (SplashCoord)ctm[1];
  mat[2] = (SplashCoord)ctm[2];
  mat[3] = (SplashCoord)ctm[3];
  mat[4] = (SplashCoord)ctm[4];
  mat[5] = (SplashCoord)ctm[5];
  splash->setMatrix(mat);
}

void SplashOutputDev::updateLineDash(GfxState *state) {
  double *dashPattern;
  int dashLength;
  double dashStart;
  SplashCoord dash[20];
  int i;

  state->getLineDash(&dashPattern, &dashLength, &dashStart);
  if (dashLength > 20) {
    dashLength = 20;
  }
  for (i = 0; i < dashLength; ++i) {
    dash[i] = (SplashCoord)dashPattern[i];
    if (dash[i] < 0) {
      dash[i] = 0;
    }
  }
  splash->setLineDash(dash, dashLength, (SplashCoord)dashStart);
}

void SplashOutputDev::updateFlatness(GfxState *state) {
#if 0 // Acrobat ignores the flatness setting, and always renders curves
      // with a fairly small flatness value
  splash->setFlatness(state->getFlatness());
#endif
}

void SplashOutputDev::updateLineJoin(GfxState *state) {
  splash->setLineJoin(state->getLineJoin());
}

void SplashOutputDev::updateLineCap(GfxState *state) {
  splash->setLineCap(state->getLineCap());
}

void SplashOutputDev::updateMiterLimit(GfxState *state) {
  splash->setMiterLimit(state->getMiterLimit());
}

void SplashOutputDev::updateLineWidth(GfxState *state) {
  splash->setLineWidth(state->getLineWidth());
}

void SplashOutputDev::updateStrokeAdjust(GfxState *state) {
#if 0 // the SA parameter supposedly defaults to false, but Acrobat
      // apparently hardwires it to true
  splash->setStrokeAdjust(state->getStrokeAdjust());
#endif
}

void SplashOutputDev::updateFillColor(GfxState *state) {
  GfxGray gray;
  GfxRGB rgb;
#if SPLASH_CMYK
  GfxCMYK cmyk;
#endif

  switch (colorMode) {
  case splashModeMono1:
  case splashModeMono8:
    state->getFillGray(&gray);
    splash->setFillPattern(getColor(gray));
    break;
  case splashModeRGB8:
  case splashModeBGR8:
    state->getFillRGB(&rgb);
    splash->setFillPattern(getColor(&rgb));
    break;
#if SPLASH_CMYK
  case splashModeCMYK8:
    state->getFillCMYK(&cmyk);
    splash->setFillPattern(getColor(&cmyk));
    break;
#endif
  }
}

void SplashOutputDev::updateStrokeColor(GfxState *state) {
  GfxGray gray;
  GfxRGB rgb;
#if SPLASH_CMYK
  GfxCMYK cmyk;
#endif

  switch (colorMode) {
  case splashModeMono1:
  case splashModeMono8:
    state->getStrokeGray(&gray);
    splash->setStrokePattern(getColor(gray));
    break;
  case splashModeRGB8:
  case splashModeBGR8:
    state->getStrokeRGB(&rgb);
    splash->setStrokePattern(getColor(&rgb));
    break;
#if SPLASH_CMYK
  case splashModeCMYK8:
    state->getStrokeCMYK(&cmyk);
    splash->setStrokePattern(getColor(&cmyk));
    break;
#endif
  }
}

SplashPattern *SplashOutputDev::getColor(GfxGray gray) {
  SplashColor color;

  if (reverseVideo) {
    gray = gfxColorComp1 - gray;
  }
  color[0] = colToByte(gray);
  return new SplashSolidColor(color);
}

SplashPattern *SplashOutputDev::getColor(GfxRGB *rgb) {
  GfxColorComp r, g, b;
  SplashColor color;

  if (reverseVideo) {
    r = gfxColorComp1 - rgb->r;
    g = gfxColorComp1 - rgb->g;
    b = gfxColorComp1 - rgb->b;
  } else {
    r = rgb->r;
    g = rgb->g;
    b = rgb->b;
  }
  color[0] = colToByte(r);
  color[1] = colToByte(g);
  color[2] = colToByte(b);
  return new SplashSolidColor(color);
}

#if SPLASH_CMYK
SplashPattern *SplashOutputDev::getColor(GfxCMYK *cmyk) {
  SplashColor color;

  color[0] = colToByte(cmyk->c);
  color[1] = colToByte(cmyk->m);
  color[2] = colToByte(cmyk->y);
  color[3] = colToByte(cmyk->k);
  return new SplashSolidColor(color);
}
#endif


void SplashOutputDev::setOverprintMask(GfxColorSpace *colorSpace,
				       GBool overprintFlag,
				       int overprintMode,
				       GfxColor *singleColor) {
#if SPLASH_CMYK
  Guint mask;
  GfxCMYK cmyk;

  if (overprintFlag && globalParams->getOverprintPreview()) {
    mask = colorSpace->getOverprintMask();
    // The OPM (overprintMode) setting is only relevant when the color
    // space is DeviceCMYK or is "implicitly converted to DeviceCMYK".
    // Per the PDF spec, this happens with ICCBased color spaces only
    // if the profile matches the output device -- Acrobat's output
    // preview mode does NOT honor OPM=1 for ICCBased CMYK color
    // spaces.  To change the behavior here, use:
    //    if (singleColor && overprintMode &&
    //        (colorSpace->getMode() == csDeviceCMYK ||
    //         (colorSpace->getMode() == csICCBased &&
    //          colorSpace->getNComps() == 4 &&
    //          <...the profile matches...>)))
    if (singleColor && overprintMode &&
	colorSpace->getMode() == csDeviceCMYK) {
      colorSpace->getCMYK(singleColor, &cmyk);
      if (cmyk.c == 0) {
	mask &= ~1;
      }
      if (cmyk.m == 0) {
	mask &= ~2;
      }
      if (cmyk.y == 0) {
	mask &= ~4;
      }
      if (cmyk.k == 0) {
	mask &= ~8;
      }
    }
  } else {
    mask = 0xffffffff;
  }
  splash->setOverprintMask(mask);
#endif
}

void SplashOutputDev::updateBlendMode(GfxState *state) {
  splash->setBlendFunc(splashOutBlendFuncs[state->getBlendMode()]);
}

void SplashOutputDev::updateFillOpacity(GfxState *state) {
  splash->setFillAlpha((SplashCoord)state->getFillOpacity());
}

void SplashOutputDev::updateStrokeOpacity(GfxState *state) {
  splash->setStrokeAlpha((SplashCoord)state->getStrokeOpacity());
}

void SplashOutputDev::updateTransfer(GfxState *state) {
  Function **transfer;
  Guchar red[256], green[256], blue[256], gray[256];
  double x, y;
  int i;

  transfer = state->getTransfer();
  if (transfer[0] &&
      transfer[0]->getInputSize() == 1 &&
      transfer[0]->getOutputSize() == 1) {
    if (transfer[1] &&
	transfer[1]->getInputSize() == 1 &&
	transfer[1]->getOutputSize() == 1 &&
	transfer[2] &&
	transfer[2]->getInputSize() == 1 &&
	transfer[2]->getOutputSize() == 1 &&
	transfer[3] &&
	transfer[3]->getInputSize() == 1 &&
	transfer[3]->getOutputSize() == 1) {
      for (i = 0; i < 256; ++i) {
	x = i / 255.0;
	transfer[0]->transform(&x, &y);
	red[i] = (Guchar)(y * 255.0 + 0.5);
	transfer[1]->transform(&x, &y);
	green[i] = (Guchar)(y * 255.0 + 0.5);
	transfer[2]->transform(&x, &y);
	blue[i] = (Guchar)(y * 255.0 + 0.5);
	transfer[3]->transform(&x, &y);
	gray[i] = (Guchar)(y * 255.0 + 0.5);
      }
    } else {
      for (i = 0; i < 256; ++i) {
	x = i / 255.0;
	transfer[0]->transform(&x, &y);
	red[i] = green[i] = blue[i] = gray[i] = (Guchar)(y * 255.0 + 0.5);
      }
    }
  } else {
    for (i = 0; i < 256; ++i) {
      red[i] = green[i] = blue[i] = gray[i] = (Guchar)i;
    }
  }
  splash->setTransfer(red, green, blue, gray);
}

void SplashOutputDev::updateFont(GfxState *state) {
  needFontUpdate = gTrue;
}

void SplashOutputDev::doUpdateFont(GfxState *state) {
  GfxFont *gfxFont;
  GfxFontLoc *fontLoc;
  GfxFontType fontType;
  SplashOutFontFileID *id;
  SplashFontFile *fontFile;
  int fontNum;
  FoFiTrueType *ff;
  Ref embRef;
  Object refObj, strObj;
#if LOAD_FONTS_FROM_MEM
  GString *fontBuf;
  FILE *extFontFile;
#else
  GString *tmpFileName, *fileName;
  FILE *tmpFile;
#endif
  char blk[4096];
  int *codeToGID;
  CharCodeToUnicode *ctu;
  double *textMat;
  double m11, m12, m21, m22, fontSize, oblique;
  double fsx, fsy, w, fontScaleMin, fontScaleAvg, fontScale;
  Gushort ww;
  SplashCoord mat[4];
  char *name;
  Unicode uBuf[8];
  int substIdx, n, code, cmap, i;

  needFontUpdate = gFalse;
  font = NULL;
#if LOAD_FONTS_FROM_MEM
  fontBuf = NULL;
#else
  tmpFileName = NULL;
  fileName = NULL;
#endif
  substIdx = -1;

  if (!(gfxFont = state->getFont())) {
    goto err1;
  }
  fontType = gfxFont->getType();
  if (fontType == fontType3) {
    goto err1;
  }

  // sanity-check the font size - skip anything larger than 20 inches
  // (this avoids problems allocating memory for the font cache)
  state->textTransformDelta(state->getFontSize(), state->getFontSize(),
			    &fsx, &fsy);
  state->transformDelta(fsx, fsy, &fsx, &fsy);
  if (fabs(fsx) > 20 * state->getHDPI() ||
      fabs(fsy) > 20 * state->getVDPI()) {
    goto err1;
  }

  // check the font file cache
  id = new SplashOutFontFileID(gfxFont->getID());
  if ((fontFile = fontEngine->getFontFile(id))) {
    delete id;

  } else {

    fontNum = 0;

    if (!(fontLoc = gfxFont->locateFont(xref, gFalse))) {
      error(errSyntaxError, -1, "Couldn't find a font for '{0:s}'",
	    gfxFont->getName() ? gfxFont->getName()->getCString()
	                       : "(unnamed)");
      goto err2;
    }

    // embedded font
    if (fontLoc->locType == gfxFontLocEmbedded) {
      gfxFont->getEmbeddedFontID(&embRef);
#if LOAD_FONTS_FROM_MEM
      fontBuf = new GString();
      refObj.initRef(embRef.num, embRef.gen);
      refObj.fetch(xref, &strObj);
      refObj.free();
      if (!strObj.isStream()) {
	error(errSyntaxError, -1, "Embedded font object is wrong type");
	strObj.free();
	delete fontLoc;
	goto err2;
      }
      strObj.streamReset();
      while ((n = strObj.streamGetBlock(blk, sizeof(blk))) > 0) {
	fontBuf->append(blk, n);
      }
      strObj.streamClose();
      strObj.free();
#else
      if (!openTempFile(&tmpFileName, &tmpFile, "wb", NULL)) {
	error(errIO, -1, "Couldn't create temporary font file");
	delete fontLoc;
	goto err2;
      }
      refObj.initRef(embRef.num, embRef.gen);
      refObj.fetch(xref, &strObj);
      refObj.free();
      if (!strObj.isStream()) {
	error(errSyntaxError, -1, "Embedded font object is wrong type");
	strObj.free();
	fclose(tmpFile);
	delete fontLoc;
	goto err2;
      }
      strObj.streamReset();
      while ((n = strObj.streamGetBlock(blk, sizeof(blk))) > 0) {
	fwrite(blk, 1, n, tmpFile);
      }
      strObj.streamClose();
      strObj.free();
      fclose(tmpFile);
      fileName = tmpFileName;
#endif

    // external font
    } else { // gfxFontLocExternal
#if LOAD_FONTS_FROM_MEM
      if (!(extFontFile = fopen(fontLoc->path->getCString(), "rb"))) {
	error(errSyntaxError, -1, "Couldn't open external font file '{0:t}'",
	      fontLoc->path);
	delete fontLoc;
	goto err2;
      }
      fontBuf = new GString();
      while ((n = fread(blk, 1, sizeof(blk), extFontFile)) > 0) {
	fontBuf->append(blk, n);
      }
      fclose(extFontFile);
#else
      fileName = fontLoc->path;
#endif
      fontNum = fontLoc->fontNum;
      if (fontLoc->substIdx >= 0) {
	id->setSubstIdx(fontLoc->substIdx);
      }
      if (fontLoc->oblique != 0) {
	id->setOblique(fontLoc->oblique);
      }
    }

    // load the font file
    switch (fontLoc->fontType) {
    case fontType1:
      if (!(fontFile = fontEngine->loadType1Font(
		   id,
#if LOAD_FONTS_FROM_MEM
		   fontBuf,
#else
		   fileName->getCString(),
		   fileName == tmpFileName,
#endif
		   (const char **)((Gfx8BitFont *)gfxFont)->getEncoding()))) {
	error(errSyntaxError, -1, "Couldn't create a font for '{0:s}'",
	      gfxFont->getName() ? gfxFont->getName()->getCString()
	                         : "(unnamed)");
	delete fontLoc;
	goto err2;
      }
      break;
    case fontType1C:
      if (!(fontFile = fontEngine->loadType1CFont(
		   id,
#if LOAD_FONTS_FROM_MEM
		   fontBuf,
#else
		   fileName->getCString(),
		   fileName == tmpFileName,
#endif
		   (const char **)((Gfx8BitFont *)gfxFont)->getEncoding()))) {
	error(errSyntaxError, -1, "Couldn't create a font for '{0:s}'",
	      gfxFont->getName() ? gfxFont->getName()->getCString()
	                         : "(unnamed)");
	delete fontLoc;
	goto err2;
      }
      break;
    case fontType1COT:
      if (!(fontFile = fontEngine->loadOpenTypeT1CFont(
		   id,
#if LOAD_FONTS_FROM_MEM
		   fontBuf,
#else
		   fileName->getCString(),
		   fileName == tmpFileName,
#endif
		   (const char **)((Gfx8BitFont *)gfxFont)->getEncoding()))) {
	error(errSyntaxError, -1, "Couldn't create a font for '{0:s}'",
	      gfxFont->getName() ? gfxFont->getName()->getCString()
	                         : "(unnamed)");
	delete fontLoc;
	goto err2;
      }
      break;
    case fontTrueType:
    case fontTrueTypeOT:
#if LOAD_FONTS_FROM_MEM
      if ((ff = FoFiTrueType::make(fontBuf->getCString(), fontBuf->getLength(),
				   fontNum))) {
#else
      if ((ff = FoFiTrueType::load(fileName->getCString(), fontNum))) {
#endif
	codeToGID = ((Gfx8BitFont *)gfxFont)->getCodeToGIDMap(ff);
	n = 256;
	delete ff;
	// if we're substituting for a non-TrueType font, we need to mark
	// all notdef codes as "do not draw" (rather than drawing TrueType
	// notdef glyphs)
	if (gfxFont->getType() != fontTrueType &&
	    gfxFont->getType() != fontTrueTypeOT) {
	  for (i = 0; i < 256; ++i) {
	    if (codeToGID[i] == 0) {
	      codeToGID[i] = -1;
	    }
	  }
	}
      } else {
	codeToGID = NULL;
	n = 0;
      }
      if (!(fontFile = fontEngine->loadTrueTypeFont(
			   id,
#if LOAD_FONTS_FROM_MEM
			   fontBuf,
#else
			   fileName->getCString(),
			   fileName == tmpFileName,
#endif
			   fontNum, codeToGID, n,
			   gfxFont->getEmbeddedFontName()
			     ? gfxFont->getEmbeddedFontName()->getCString()
			     : (char *)NULL))) {
	error(errSyntaxError, -1, "Couldn't create a font for '{0:s}'",
	      gfxFont->getName() ? gfxFont->getName()->getCString()
	                         : "(unnamed)");
	delete fontLoc;
	goto err2;
      }
      break;
    case fontCIDType0:
    case fontCIDType0C:
      if (!(fontFile = fontEngine->loadCIDFont(
			   id,
#if LOAD_FONTS_FROM_MEM
			   fontBuf
#else
			   fileName->getCString(),
			   fileName == tmpFileName
#endif
			))) {

	error(errSyntaxError, -1, "Couldn't create a font for '{0:s}'",
	      gfxFont->getName() ? gfxFont->getName()->getCString()
	                         : "(unnamed)");
	delete fontLoc;
	goto err2;
      }
      break;
    case fontCIDType0COT:
      if (((GfxCIDFont *)gfxFont)->getCIDToGID()) {
	n = ((GfxCIDFont *)gfxFont)->getCIDToGIDLen();
	codeToGID = (int *)gmallocn(n, sizeof(int));
	memcpy(codeToGID, ((GfxCIDFont *)gfxFont)->getCIDToGID(),
	       n * sizeof(int));
      } else {
	codeToGID = NULL;
	n = 0;
      }
      if (!(fontFile = fontEngine->loadOpenTypeCFFFont(
			   id,
#if LOAD_FONTS_FROM_MEM
			   fontBuf,
#else
			   fileName->getCString(),
			   fileName == tmpFileName,
#endif
			   codeToGID, n))) {
	error(errSyntaxError, -1, "Couldn't create a font for '{0:s}'",
	      gfxFont->getName() ? gfxFont->getName()->getCString()
	                         : "(unnamed)");
	delete fontLoc;
	goto err2;
      }
      break;
    case fontCIDType2:
    case fontCIDType2OT:
      codeToGID = NULL;
      n = 0;
      if (fontLoc->locType == gfxFontLocEmbedded) {
	if (((GfxCIDFont *)gfxFont)->getCIDToGID()) {
	  n = ((GfxCIDFont *)gfxFont)->getCIDToGIDLen();
	  codeToGID = (int *)gmallocn(n, sizeof(int));
	  memcpy(codeToGID, ((GfxCIDFont *)gfxFont)->getCIDToGID(),
		 n * sizeof(int));
	}
      } else if (!globalParams->getMapExtTrueTypeFontsViaUnicode()) {
	codeToGID = NULL;
	n = 0;
      } else {
	// create a CID-to-GID mapping, via Unicode
	if ((ctu = ((GfxCIDFont *)gfxFont)->getToUnicode())) {
#if LOAD_FONTS_FROM_MEM
	  if ((ff = FoFiTrueType::make(fontBuf->getCString(),
				       fontBuf->getLength(), fontNum))) {
#else
	  if ((ff = FoFiTrueType::load(fileName->getCString(), fontNum))) {
#endif
	    // look for a Unicode cmap
	    for (cmap = 0; cmap < ff->getNumCmaps(); ++cmap) {
	      if ((ff->getCmapPlatform(cmap) == 3 &&
		   ff->getCmapEncoding(cmap) == 1) ||
		  ff->getCmapPlatform(cmap) == 0) {
		break;
	      }
	    }
	    if (cmap < ff->getNumCmaps()) {
	      // map CID -> Unicode -> GID
	      if (ctu->isIdentity()) {
		n = 65536;
	      } else {
		n = ctu->getLength();
	      }
	      codeToGID = (int *)gmallocn(n, sizeof(int));
	      for (code = 0; code < n; ++code) {
		if (ctu->mapToUnicode(code, uBuf, 8) > 0) {
		  codeToGID[code] = ff->mapCodeToGID(cmap, uBuf[0]);
		} else {
		  codeToGID[code] = -1;
		}
	      }
	    }
	    delete ff;
	  }
	  ctu->decRefCnt();
	} else {
	  error(errSyntaxError, -1,
		"Couldn't find a mapping to Unicode for font '{0:s}'",
		gfxFont->getName() ? gfxFont->getName()->getCString()
		                   : "(unnamed)");
	}
      }
      if (!(fontFile = fontEngine->loadTrueTypeFont(
			   id,
#if LOAD_FONTS_FROM_MEM
			   fontBuf,
#else
			   fileName->getCString(),
			   fileName == tmpFileName,
#endif
			   fontNum, codeToGID, n,
			   gfxFont->getEmbeddedFontName()
			     ? gfxFont->getEmbeddedFontName()->getCString()
			     : (char *)NULL))) {
	error(errSyntaxError, -1, "Couldn't create a font for '{0:s}'",
	      gfxFont->getName() ? gfxFont->getName()->getCString()
	                         : "(unnamed)");
	delete fontLoc;
	goto err2;
      }
      break;
    default:
      // this shouldn't happen
      goto err2;
    }

    delete fontLoc;
  }

  // get the font matrix
  textMat = state->getTextMat();
  fontSize = state->getFontSize();
  oblique = ((SplashOutFontFileID *)fontFile->getID())->getOblique();
  m11 = state->getHorizScaling() * textMat[0];
  m12 = state->getHorizScaling() * textMat[1];
  m21 = oblique * m11 + textMat[2];
  m22 = oblique * m12 + textMat[3];
  m11 *= fontSize;
  m12 *= fontSize;
  m21 *= fontSize;
  m22 *= fontSize;

  // for substituted fonts: adjust the font matrix -- compare the
  // widths of letters and digits (A-Z, a-z, 0-9) in the original font
  // and the substituted font
  substIdx = ((SplashOutFontFileID *)fontFile->getID())->getSubstIdx();
  if (substIdx >= 0 && substIdx < 12) {
    fontScaleMin = 1;
    fontScaleAvg = 0;
    n = 0;
    for (code = 0; code < 256; ++code) {
      if ((name = ((Gfx8BitFont *)gfxFont)->getCharName(code)) &&
	  name[0] && !name[1] &&
	  ((name[0] >= 'A' && name[0] <= 'Z') ||
	   (name[0] >= 'a' && name[0] <= 'z') ||
	   (name[0] >= '0' && name[0] <= '9'))) {
	w = ((Gfx8BitFont *)gfxFont)->getWidth(code);
	builtinFontSubst[substIdx]->widths->getWidth(name, &ww);
	if (w > 0.01 && ww > 10) {
	  w /= ww * 0.001;
	  if (w < fontScaleMin) {
	    fontScaleMin = w;
	  }
	  fontScaleAvg += w;
	  ++n;
	}
      }
    }
    // if real font is narrower than substituted font, reduce the font
    // size accordingly -- this currently uses a scale factor halfway
    // between the minimum and average computed scale factors, which
    // is a bit of a kludge, but seems to produce mostly decent
    // results
    if (n) {
      fontScaleAvg /= n;
      if (fontScaleAvg < 1) {
	fontScale = 0.5 * (fontScaleMin + fontScaleAvg);
	m11 *= fontScale;
	m12 *= fontScale;
      }
    }
  }

  // create the scaled font
  mat[0] = m11;  mat[1] = m12;
  mat[2] = m21;  mat[3] = m22;
  font = fontEngine->getFont(fontFile, mat, splash->getMatrix());

#if !LOAD_FONTS_FROM_MEM
  if (tmpFileName) {
    delete tmpFileName;
  }
#endif
  return;

 err2:
  delete id;
 err1:
#if LOAD_FONTS_FROM_MEM
  if (fontBuf) {
    delete fontBuf;
  }
#else
  if (tmpFileName) {
    unlink(tmpFileName->getCString());
    delete tmpFileName;
  }
#endif
  return;
}

void SplashOutputDev::stroke(GfxState *state) {
  SplashPath *path;

  if (state->getStrokeColorSpace()->isNonMarking()) {
    return;
  }
  setOverprintMask(state->getStrokeColorSpace(), state->getStrokeOverprint(),
		   state->getOverprintMode(), state->getStrokeColor());
  path = convertPath(state, state->getPath(), gFalse);
  splash->stroke(path);
  delete path;
}

void SplashOutputDev::fill(GfxState *state) {
  SplashPath *path;

  if (state->getFillColorSpace()->isNonMarking()) {
    return;
  }
  setOverprintMask(state->getFillColorSpace(), state->getFillOverprint(),
		   state->getOverprintMode(), state->getFillColor());
  path = convertPath(state, state->getPath(), gTrue);
  splash->fill(path, gFalse);
  delete path;
}

void SplashOutputDev::eoFill(GfxState *state) {
  SplashPath *path;

  if (state->getFillColorSpace()->isNonMarking()) {
    return;
  }
  setOverprintMask(state->getFillColorSpace(), state->getFillOverprint(),
		   state->getOverprintMode(), state->getFillColor());
  path = convertPath(state, state->getPath(), gTrue);
  splash->fill(path, gTrue);
  delete path;
}

void SplashOutputDev::tilingPatternFill(GfxState *state, Gfx *gfx,
					Object *strRef,
					int paintType, Dict *resDict,
					double *mat, double *bbox,
					int x0, int y0, int x1, int y1,
					double xStep, double yStep) {
  double tileXMin, tileYMin, tileXMax, tileYMax, tx, ty;
  int tileX0, tileY0, tileW, tileH, tileSize;
  SplashBitmap *origBitmap, *tileBitmap;
  Splash *origSplash;
  SplashColor color;
  double mat1[6];
  double xa, ya, xb, yb, xc, yc;
  int x, y, xx, yy, i;

  // transform the four corners of the bbox from pattern space to
  // device space and compute the device space bbox
  state->transform(bbox[0] * mat[0] + bbox[1] * mat[2] + mat[4],
		   bbox[0] * mat[1] + bbox[1] * mat[3] + mat[5],
		   &tx, &ty);
  tileXMin = tileXMax = tx;
  tileYMin = tileYMax = ty;
  state->transform(bbox[2] * mat[0] + bbox[1] * mat[2] + mat[4],
		   bbox[2] * mat[1] + bbox[1] * mat[3] + mat[5],
		   &tx, &ty);
  if (tx < tileXMin) {
    tileXMin = tx;
  } else if (tx > tileXMax) {
    tileXMax = tx;
  }
  if (ty < tileYMin) {
    tileYMin = ty;
  } else if (ty > tileYMax) {
    tileYMax = ty;
  }
  state->transform(bbox[2] * mat[0] + bbox[3] * mat[2] + mat[4],
		   bbox[2] * mat[1] + bbox[3] * mat[3] + mat[5],
		   &tx, &ty);
  if (tx < tileXMin) {
    tileXMin = tx;
  } else if (tx > tileXMax) {
    tileXMax = tx;
  }
  if (ty < tileYMin) {
    tileYMin = ty;
  } else if (ty > tileYMax) {
    tileYMax = ty;
  }
  state->transform(bbox[0] * mat[0] + bbox[3] * mat[2] + mat[4],
		   bbox[0] * mat[1] + bbox[3] * mat[3] + mat[5],
		   &tx, &ty);
  if (tx < tileXMin) {
    tileXMin = tx;
  } else if (tx > tileXMax) {
    tileXMax = tx;
  }
  if (ty < tileYMin) {
    tileYMin = ty;
  } else if (ty > tileYMax) {
    tileYMax = ty;
  }
  if (tileXMin == tileXMax || tileYMin == tileYMax) {
    return;
  }

  tileX0 = (int)floor(tileXMin);
  tileY0 = (int)floor(tileYMin);
  tileW = (int)ceil(tileXMax) - tileX0;
  tileH = (int)ceil(tileYMax) - tileY0;

  // check for an excessively large tile size
  tileSize = tileW * tileH;
  if (tileSize > 1000000 || tileSize < 0) {
    mat1[0] = mat[0];
    mat1[1] = mat[1];
    mat1[2] = mat[2];
    mat1[3] = mat[3];
    for (y = y0; y < y1; ++y) {
      for (x = x0; x < x1; ++x) {
	xa = x * xStep;
	ya = y * yStep;
	mat1[4] = xa * mat[0] + ya * mat[2] + mat[4];
	mat1[5] = xa * mat[1] + ya * mat[3] + mat[5];
	gfx->drawForm(strRef, resDict, mat1, bbox);
      }
    }
    return;
  }

  // create a temporary bitmap
  origBitmap = bitmap;
  origSplash = splash;
  bitmap = tileBitmap = new SplashBitmap(tileW, tileH, bitmapRowPad,
					 colorMode, gTrue, bitmapTopDown);
  splash = new Splash(bitmap, vectorAntialias, origSplash->getScreen());
  splash->setMinLineWidth(globalParams->getMinLineWidth());
  splash->setStrokeAdjust(globalParams->getStrokeAdjust());
  for (i = 0; i < splashMaxColorComps; ++i) {
    color[i] = 0;
  }
  splash->clear(color);
  ++nestCount;

  // copy the fill color (for uncolored tiling patterns)
  // (and stroke color, to handle buggy PDF files)
  splash->setFillPattern(origSplash->getFillPattern()->copy());
  splash->setStrokePattern(origSplash->getStrokePattern()->copy());

  // render the tile
  state->shiftCTM(-tileX0, -tileY0);
  updateCTM(state, 0, 0, 0, 0, 0, 0);
  gfx->drawForm(strRef, resDict, mat, bbox);
  state->shiftCTM(tileX0, tileY0);
  updateCTM(state, 0, 0, 0, 0, 0, 0);

  // restore the original bitmap
  --nestCount;
  delete splash;
  bitmap = origBitmap;
  splash = origSplash;
  splash->setOverprintMask(0xffffffff);

  // draw the tiles
  for (y = y0; y < y1; ++y) {
    for (x = x0; x < x1; ++x) {
      xa = x * xStep;
      ya = y * yStep;
      xb = xa * mat[0] + ya * mat[2];
      yb = xa * mat[1] + ya * mat[3];
      state->transformDelta(xb, yb, &xc, &yc);
      xx = (int)(xc + tileX0 + 0.5);
      yy = (int)(yc + tileY0 + 0.5);
      splash->composite(tileBitmap, 0, 0, xx, yy, tileW, tileH,
			gFalse, gFalse);
    }
  }

  delete tileBitmap;
}

void SplashOutputDev::clip(GfxState *state) {
  SplashPath *path;

  path = convertPath(state, state->getPath(), gTrue);
  splash->clipToPath(path, gFalse);
  delete path;
}

void SplashOutputDev::eoClip(GfxState *state) {
  SplashPath *path;

  path = convertPath(state, state->getPath(), gTrue);
  splash->clipToPath(path, gTrue);
  delete path;
}

void SplashOutputDev::clipToStrokePath(GfxState *state) {
  SplashPath *path, *path2;

  path = convertPath(state, state->getPath(), gFalse);
  path2 = splash->makeStrokePath(path, state->getLineWidth());
  delete path;
  splash->clipToPath(path2, gFalse);
  delete path2;
}

SplashPath *SplashOutputDev::convertPath(GfxState *state, GfxPath *path,
					 GBool dropEmptySubpaths) {
  SplashPath *sPath;
  GfxSubpath *subpath;
  int n, i, j;

  n = dropEmptySubpaths ? 1 : 0;
  sPath = new SplashPath();
  for (i = 0; i < path->getNumSubpaths(); ++i) {
    subpath = path->getSubpath(i);
    if (subpath->getNumPoints() > n) {
      sPath->moveTo((SplashCoord)subpath->getX(0),
		    (SplashCoord)subpath->getY(0));
      j = 1;
      while (j < subpath->getNumPoints()) {
	if (subpath->getCurve(j)) {
	  sPath->curveTo((SplashCoord)subpath->getX(j),
			 (SplashCoord)subpath->getY(j),
			 (SplashCoord)subpath->getX(j+1),
			 (SplashCoord)subpath->getY(j+1),
			 (SplashCoord)subpath->getX(j+2),
			 (SplashCoord)subpath->getY(j+2));
	  j += 3;
	} else {
	  sPath->lineTo((SplashCoord)subpath->getX(j),
			(SplashCoord)subpath->getY(j));
	  ++j;
	}
      }
      if (subpath->isClosed()) {
	sPath->close();
      }
    }
  }
  return sPath;
}

void SplashOutputDev::drawChar(GfxState *state, double x, double y,
			       double dx, double dy,
			       double originX, double originY,
			       CharCode code, int nBytes,
			       Unicode *u, int uLen) {
  SplashPath *path;
  int render;
  GBool doFill, doStroke, doClip, strokeAdjust;
  double m[4];
  GBool horiz;

  if (skipHorizText || skipRotatedText) {
    state->getFontTransMat(&m[0], &m[1], &m[2], &m[3]);
    horiz = m[0] > 0 && fabs(m[1]) < 0.001 &&
            fabs(m[2]) < 0.001 && m[3] < 0;
    if ((skipHorizText && horiz) || (skipRotatedText && !horiz)) {
      return;
    }
  }

  // check for invisible text -- this is used by Acrobat Capture
  render = state->getRender();
  if (render == 3) {
    return;
  }

  if (needFontUpdate) {
    doUpdateFont(state);
  }
  if (!font) {
    return;
  }

  x -= originX;
  y -= originY;

  doFill = !(render & 1) && !state->getFillColorSpace()->isNonMarking();
  doStroke = ((render & 3) == 1 || (render & 3) == 2) &&
             !state->getStrokeColorSpace()->isNonMarking();
  doClip = render & 4;

  path = NULL;
  if (doStroke || doClip) {
    if ((path = font->getGlyphPath(code))) {
      path->offset((SplashCoord)x, (SplashCoord)y);
    }
  }

  // don't use stroke adjustment when stroking text -- the results
  // tend to be ugly (because characters with horizontal upper or
  // lower edges get misaligned relative to the other characters)
  strokeAdjust = gFalse; // make gcc happy
  if (doStroke) {
    strokeAdjust = splash->getStrokeAdjust();
    splash->setStrokeAdjust(gFalse);
  }

  // fill and stroke
  if (doFill && doStroke) {
    if (path) {
      setOverprintMask(state->getFillColorSpace(), state->getFillOverprint(),
		       state->getOverprintMode(), state->getFillColor());
      splash->fill(path, gFalse);
      setOverprintMask(state->getStrokeColorSpace(),
		       state->getStrokeOverprint(),
		       state->getOverprintMode(),
		       state->getStrokeColor());
      splash->stroke(path);
    }

  // fill
  } else if (doFill) {
    setOverprintMask(state->getFillColorSpace(), state->getFillOverprint(),
		     state->getOverprintMode(), state->getFillColor());
    splash->fillChar((SplashCoord)x, (SplashCoord)y, code, font);

  // stroke
  } else if (doStroke) {
    if (path) {
      setOverprintMask(state->getStrokeColorSpace(),
		       state->getStrokeOverprint(),
		       state->getOverprintMode(),
		       state->getStrokeColor());
      splash->stroke(path);
    }
  }

  // clip
  if (doClip) {
    if (path) {
      if (textClipPath) {
	textClipPath->append(path);
      } else {
	textClipPath = path;
	path = NULL;
      }
    }
  }

  if (doStroke) {
    splash->setStrokeAdjust(strokeAdjust);
  }

  if (path) {
    delete path;
  }
}

GBool SplashOutputDev::beginType3Char(GfxState *state, double x, double y,
				      double dx, double dy,
				      CharCode code, Unicode *u, int uLen) {
  GfxFont *gfxFont;
  Ref *fontID;
  double *ctm, *bbox;
  T3FontCache *t3Font;
  T3GlyphStack *t3gs;
  GBool validBBox;
  double m[4];
  GBool horiz;
  double x1, y1, xMin, yMin, xMax, yMax, xt, yt;
  int i, j;

  if (skipHorizText || skipRotatedText) {
    state->getFontTransMat(&m[0], &m[1], &m[2], &m[3]);
    horiz = m[0] > 0 && fabs(m[1]) < 0.001 &&
            fabs(m[2]) < 0.001 && m[3] < 0;
    if ((skipHorizText && horiz) || (skipRotatedText && !horiz)) {
      return gTrue;
    }
  }

  if (!(gfxFont = state->getFont())) {
    return gFalse;
  }
  fontID = gfxFont->getID();
  ctm = state->getCTM();
  state->transform(0, 0, &xt, &yt);

  // is it the first (MRU) font in the cache?
  if (!(nT3Fonts > 0 &&
	t3FontCache[0]->matches(fontID, ctm[0], ctm[1], ctm[2], ctm[3]))) {

    // is the font elsewhere in the cache?
    for (i = 1; i < nT3Fonts; ++i) {
      if (t3FontCache[i]->matches(fontID, ctm[0], ctm[1], ctm[2], ctm[3])) {
	t3Font = t3FontCache[i];
	for (j = i; j > 0; --j) {
	  t3FontCache[j] = t3FontCache[j - 1];
	}
	t3FontCache[0] = t3Font;
	break;
      }
    }
    if (i >= nT3Fonts) {

      // create new entry in the font cache
      if (nT3Fonts == splashOutT3FontCacheSize) {
	delete t3FontCache[nT3Fonts - 1];
	--nT3Fonts;
      }
      for (j = nT3Fonts; j > 0; --j) {
	t3FontCache[j] = t3FontCache[j - 1];
      }
      ++nT3Fonts;
      bbox = gfxFont->getFontBBox();
      if (bbox[0] == 0 && bbox[1] == 0 && bbox[2] == 0 && bbox[3] == 0) {
	// unspecified bounding box -- just take a guess
	xMin = xt - 5;
	xMax = xMin + 30;
	yMax = yt + 15;
	yMin = yMax - 45;
	validBBox = gFalse;
      } else {
	state->transform(bbox[0], bbox[1], &x1, &y1);
	xMin = xMax = x1;
	yMin = yMax = y1;
	state->transform(bbox[0], bbox[3], &x1, &y1);
	if (x1 < xMin) {
	  xMin = x1;
	} else if (x1 > xMax) {
	  xMax = x1;
	}
	if (y1 < yMin) {
	  yMin = y1;
	} else if (y1 > yMax) {
	  yMax = y1;
	}
	state->transform(bbox[2], bbox[1], &x1, &y1);
	if (x1 < xMin) {
	  xMin = x1;
	} else if (x1 > xMax) {
	  xMax = x1;
	}
	if (y1 < yMin) {
	  yMin = y1;
	} else if (y1 > yMax) {
	  yMax = y1;
	}
	state->transform(bbox[2], bbox[3], &x1, &y1);
	if (x1 < xMin) {
	  xMin = x1;
	} else if (x1 > xMax) {
	  xMax = x1;
	}
	if (y1 < yMin) {
	  yMin = y1;
	} else if (y1 > yMax) {
	  yMax = y1;
	}
	validBBox = gTrue;
      }
      t3FontCache[0] = new T3FontCache(fontID, ctm[0], ctm[1], ctm[2], ctm[3],
	                               (int)floor(xMin - xt) - 2,
				       (int)floor(yMin - yt) - 2,
				       (int)ceil(xMax) - (int)floor(xMin) + 4,
				       (int)ceil(yMax) - (int)floor(yMin) + 4,
				       validBBox,
				       colorMode != splashModeMono1);
    }
  }
  t3Font = t3FontCache[0];

  // is the glyph in the cache?
  i = (code & (t3Font->cacheSets - 1)) * t3Font->cacheAssoc;
  for (j = 0; j < t3Font->cacheAssoc; ++j) {
    if ((t3Font->cacheTags[i+j].mru & 0x8000) &&
	t3Font->cacheTags[i+j].code == code) {
      drawType3Glyph(state, t3Font, &t3Font->cacheTags[i+j],
		     t3Font->cacheData + (i+j) * t3Font->glyphSize);
      return gTrue;
    }
  }

  // push a new Type 3 glyph record
  t3gs = new T3GlyphStack();
  t3gs->next = t3GlyphStack;
  t3GlyphStack = t3gs;
  t3GlyphStack->code = code;
  t3GlyphStack->cache = t3Font;
  t3GlyphStack->cacheTag = NULL;
  t3GlyphStack->cacheData = NULL;
  t3GlyphStack->haveDx = gFalse;
  t3GlyphStack->doNotCache = gFalse;

  return gFalse;
}

void SplashOutputDev::endType3Char(GfxState *state) {
  T3GlyphStack *t3gs;
  double *ctm;

  if (t3GlyphStack->cacheTag) {
    --nestCount;
    memcpy(t3GlyphStack->cacheData, bitmap->getDataPtr(),
	   t3GlyphStack->cache->glyphSize);
    delete bitmap;
    delete splash;
    bitmap = t3GlyphStack->origBitmap;
    splash = t3GlyphStack->origSplash;
    ctm = state->getCTM();
    state->setCTM(ctm[0], ctm[1], ctm[2], ctm[3],
		  t3GlyphStack->origCTM4, t3GlyphStack->origCTM5);
    updateCTM(state, 0, 0, 0, 0, 0, 0);
    drawType3Glyph(state, t3GlyphStack->cache,
		   t3GlyphStack->cacheTag, t3GlyphStack->cacheData);
  }
  t3gs = t3GlyphStack;
  t3GlyphStack = t3gs->next;
  delete t3gs;
}

void SplashOutputDev::type3D0(GfxState *state, double wx, double wy) {
  t3GlyphStack->haveDx = gTrue;
}

void SplashOutputDev::type3D1(GfxState *state, double wx, double wy,
			      double llx, double lly, double urx, double ury) {
  double *ctm;
  T3FontCache *t3Font;
  SplashColor color;
  double xt, yt, xMin, xMax, yMin, yMax, x1, y1;
  int i, j;

  // ignore multiple d0/d1 operators
  if (t3GlyphStack->haveDx) {
    return;
  }
  t3GlyphStack->haveDx = gTrue;
  // don't cache if we got a gsave/grestore before the d1
  if (t3GlyphStack->doNotCache) {
    return;
  }

  t3Font = t3GlyphStack->cache;

  // check for a valid bbox
  state->transform(0, 0, &xt, &yt);
  state->transform(llx, lly, &x1, &y1);
  xMin = xMax = x1;
  yMin = yMax = y1;
  state->transform(llx, ury, &x1, &y1);
  if (x1 < xMin) {
    xMin = x1;
  } else if (x1 > xMax) {
    xMax = x1;
  }
  if (y1 < yMin) {
    yMin = y1;
  } else if (y1 > yMax) {
    yMax = y1;
  }
  state->transform(urx, lly, &x1, &y1);
  if (x1 < xMin) {
    xMin = x1;
  } else if (x1 > xMax) {
    xMax = x1;
  }
  if (y1 < yMin) {
    yMin = y1;
  } else if (y1 > yMax) {
    yMax = y1;
  }
  state->transform(urx, ury, &x1, &y1);
  if (x1 < xMin) {
    xMin = x1;
  } else if (x1 > xMax) {
    xMax = x1;
  }
  if (y1 < yMin) {
    yMin = y1;
  } else if (y1 > yMax) {
    yMax = y1;
  }
  if (xMin - xt < t3Font->glyphX ||
      yMin - yt < t3Font->glyphY ||
      xMax - xt > t3Font->glyphX + t3Font->glyphW ||
      yMax - yt > t3Font->glyphY + t3Font->glyphH) {
    if (t3Font->validBBox) {
      error(errSyntaxWarning, -1, "Bad bounding box in Type 3 glyph");
    }
    return;
  }

  // allocate a cache entry
  i = (t3GlyphStack->code & (t3Font->cacheSets - 1)) * t3Font->cacheAssoc;
  for (j = 0; j < t3Font->cacheAssoc; ++j) {
    if ((t3Font->cacheTags[i+j].mru & 0x7fff) == t3Font->cacheAssoc - 1) {
      t3Font->cacheTags[i+j].mru = 0x8000;
      t3Font->cacheTags[i+j].code = t3GlyphStack->code;
      t3GlyphStack->cacheTag = &t3Font->cacheTags[i+j];
      t3GlyphStack->cacheData = t3Font->cacheData + (i+j) * t3Font->glyphSize;
    } else {
      ++t3Font->cacheTags[i+j].mru;
    }
  }

  // save state
  t3GlyphStack->origBitmap = bitmap;
  t3GlyphStack->origSplash = splash;
  ctm = state->getCTM();
  t3GlyphStack->origCTM4 = ctm[4];
  t3GlyphStack->origCTM5 = ctm[5];

  // create the temporary bitmap
  if (colorMode == splashModeMono1) {
    bitmap = new SplashBitmap(t3Font->glyphW, t3Font->glyphH, 1,
			      splashModeMono1, gFalse);
    splash = new Splash(bitmap, gFalse,
			t3GlyphStack->origSplash->getScreen());
    color[0] = 0;
    splash->clear(color);
    color[0] = 0xff;
  } else {
    bitmap = new SplashBitmap(t3Font->glyphW, t3Font->glyphH, 1,
			      splashModeMono8, gFalse);
    splash = new Splash(bitmap, vectorAntialias,
			t3GlyphStack->origSplash->getScreen());
    color[0] = 0x00;
    splash->clear(color);
    color[0] = 0xff;
  }
  splash->setMinLineWidth(globalParams->getMinLineWidth());
  splash->setStrokeAdjust(t3GlyphStack->origSplash->getStrokeAdjust());
  splash->setFillPattern(new SplashSolidColor(color));
  splash->setStrokePattern(new SplashSolidColor(color));
  //~ this should copy other state from t3GlyphStack->origSplash?
  //~ [this is likely the same situation as in beginTransparencyGroup()]
  state->setCTM(ctm[0], ctm[1], ctm[2], ctm[3],
		-t3Font->glyphX, -t3Font->glyphY);
  updateCTM(state, 0, 0, 0, 0, 0, 0);
  ++nestCount;
}

void SplashOutputDev::drawType3Glyph(GfxState *state, T3FontCache *t3Font,
				     T3FontCacheTag *tag, Guchar *data) {
  SplashGlyphBitmap glyph;

  setOverprintMask(state->getFillColorSpace(), state->getFillOverprint(),
		   state->getOverprintMode(), state->getFillColor());
  glyph.x = -t3Font->glyphX;
  glyph.y = -t3Font->glyphY;
  glyph.w = t3Font->glyphW;
  glyph.h = t3Font->glyphH;
  glyph.aa = colorMode != splashModeMono1;
  glyph.data = data;
  glyph.freeData = gFalse;
  splash->fillGlyph(0, 0, &glyph);
}

void SplashOutputDev::endTextObject(GfxState *state) {
  if (textClipPath) {
    splash->clipToPath(textClipPath, gFalse);
    delete textClipPath;
    textClipPath = NULL;
  }
}

struct SplashOutImageMaskData {
  ImageStream *imgStr;
  GBool invert;
  int width, height, y;
};

GBool SplashOutputDev::imageMaskSrc(void *data, SplashColorPtr line) {
  SplashOutImageMaskData *imgMaskData = (SplashOutImageMaskData *)data;
  Guchar *p;
  SplashColorPtr q;
  int x;

  if (imgMaskData->y == imgMaskData->height ||
      !(p = imgMaskData->imgStr->getLine())) {
    memset(line, 0, imgMaskData->width);
    return gFalse;
  }
  for (x = 0, q = line; x < imgMaskData->width; ++x) {
    *q++ = *p++ ^ imgMaskData->invert;
  }
  ++imgMaskData->y;
  return gTrue;
}

void SplashOutputDev::drawImageMask(GfxState *state, Object *ref, Stream *str,
				    int width, int height, GBool invert,
				    GBool inlineImg, GBool interpolate) {
  double *ctm;
  SplashCoord mat[6];
  SplashOutImageMaskData imgMaskData;

  if (state->getFillColorSpace()->isNonMarking()) {
    return;
  }
  setOverprintMask(state->getFillColorSpace(), state->getFillOverprint(),
		   state->getOverprintMode(), state->getFillColor());

  ctm = state->getCTM();
  mat[0] = ctm[0];
  mat[1] = ctm[1];
  mat[2] = -ctm[2];
  mat[3] = -ctm[3];
  mat[4] = ctm[2] + ctm[4];
  mat[5] = ctm[3] + ctm[5];

  reduceImageResolution(str, ctm, &width, &height);

  imgMaskData.imgStr = new ImageStream(str, width, 1, 1);
  imgMaskData.imgStr->reset();
  imgMaskData.invert = invert ? 0 : 1;
  imgMaskData.width = width;
  imgMaskData.height = height;
  imgMaskData.y = 0;

  splash->fillImageMask(&imageMaskSrc, &imgMaskData, width, height, mat,
			t3GlyphStack != NULL, interpolate);
  if (inlineImg) {
    while (imgMaskData.y < height) {
      imgMaskData.imgStr->getLine();
      ++imgMaskData.y;
    }
  }

  delete imgMaskData.imgStr;
  str->close();
}

void SplashOutputDev::setSoftMaskFromImageMask(GfxState *state,
					       Object *ref, Stream *str,
					       int width, int height,
					       GBool invert,
					       GBool inlineImg,
					       GBool interpolate) {
  double *ctm;
  SplashCoord mat[6];
  SplashOutImageMaskData imgMaskData;
  SplashBitmap *maskBitmap;
  Splash *maskSplash;
  SplashColor maskColor;

  ctm = state->getCTM();
  mat[0] = ctm[0];
  mat[1] = ctm[1];
  mat[2] = -ctm[2];
  mat[3] = -ctm[3];
  mat[4] = ctm[2] + ctm[4];
  mat[5] = ctm[3] + ctm[5];
  reduceImageResolution(str, ctm, &width, &height);
  imgMaskData.imgStr = new ImageStream(str, width, 1, 1);
  imgMaskData.imgStr->reset();
  imgMaskData.invert = invert ? 0 : 1;
  imgMaskData.width = width;
  imgMaskData.height = height;
  imgMaskData.y = 0;
  maskBitmap = new SplashBitmap(bitmap->getWidth(), bitmap->getHeight(),
				1, splashModeMono8, gFalse);
  maskSplash = new Splash(maskBitmap, gTrue);
  maskSplash->setStrokeAdjust(globalParams->getStrokeAdjust());
  clearMaskRegion(state, maskSplash, 0, 0, 1, 1);
  maskColor[0] = 0xff;
  maskSplash->setFillPattern(new SplashSolidColor(maskColor));
  maskSplash->fillImageMask(&imageMaskSrc, &imgMaskData,
			    width, height, mat, gFalse, interpolate);
  delete imgMaskData.imgStr;
  str->close();
  delete maskSplash;
  splash->setSoftMask(maskBitmap);
}

struct SplashOutImageData {
  ImageStream *imgStr;
  GfxImageColorMap *colorMap;
  SplashColorPtr lookup;
  int *maskColors;
  SplashColorMode colorMode;
  int width, height, y;
};

GBool SplashOutputDev::imageSrc(void *data, SplashColorPtr colorLine,
				Guchar *alphaLine) {
  SplashOutImageData *imgData = (SplashOutImageData *)data;
  Guchar *p;
  SplashColorPtr q, col;
  int nComps, x;

  if (imgData->y == imgData->height ||
      !(p = imgData->imgStr->getLine())) {
    memset(colorLine, 0,
	   imgData->width * splashColorModeNComps[imgData->colorMode]);
    return gFalse;
  }

  nComps = imgData->colorMap->getNumPixelComps();

  if (imgData->lookup) {
    switch (imgData->colorMode) {
    case splashModeMono1:
    case splashModeMono8:
      for (x = 0, q = colorLine; x < imgData->width; ++x, ++p) {
	*q++ = imgData->lookup[*p];
      }
      break;
    case splashModeRGB8:
    case splashModeBGR8:
      for (x = 0, q = colorLine; x < imgData->width; ++x, ++p) {
	col = &imgData->lookup[3 * *p];
	*q++ = col[0];
	*q++ = col[1];
	*q++ = col[2];
      }
      break;
#if SPLASH_CMYK
    case splashModeCMYK8:
      for (x = 0, q = colorLine; x < imgData->width; ++x, ++p) {
	col = &imgData->lookup[4 * *p];
	*q++ = col[0];
	*q++ = col[1];
	*q++ = col[2];
	*q++ = col[3];
      }
      break;
#endif
    }
  } else {
    switch (imgData->colorMode) {
    case splashModeMono1:
    case splashModeMono8:
      imgData->colorMap->getGrayByteLine(p, colorLine, imgData->width);
      break;
    case splashModeRGB8:
    case splashModeBGR8:
      imgData->colorMap->getRGBByteLine(p, colorLine, imgData->width);
      break;
#if SPLASH_CMYK
    case splashModeCMYK8:
      imgData->colorMap->getCMYKByteLine(p, colorLine, imgData->width);
      break;
#endif
    }
  }

  ++imgData->y;
  return gTrue;
}

GBool SplashOutputDev::alphaImageSrc(void *data, SplashColorPtr colorLine,
				     Guchar *alphaLine) {
  SplashOutImageData *imgData = (SplashOutImageData *)data;
  Guchar *p, *aq;
  SplashColorPtr q, col;
  GfxRGB rgb;
  GfxGray gray;
#if SPLASH_CMYK
  GfxCMYK cmyk;
#endif
  Guchar alpha;
  int nComps, x, i;

  if (imgData->y == imgData->height ||
      !(p = imgData->imgStr->getLine())) {
    memset(colorLine, 0,
	   imgData->width * splashColorModeNComps[imgData->colorMode]);
    memset(alphaLine, 0, imgData->width);
    return gFalse;
  }

  nComps = imgData->colorMap->getNumPixelComps();

  for (x = 0, q = colorLine, aq = alphaLine;
       x < imgData->width;
       ++x, p += nComps) {
    alpha = 0;
    for (i = 0; i < nComps; ++i) {
      if (p[i] < imgData->maskColors[2*i] ||
	  p[i] > imgData->maskColors[2*i+1]) {
	alpha = 0xff;
	break;
      }
    }
    if (imgData->lookup) {
      switch (imgData->colorMode) {
      case splashModeMono1:
      case splashModeMono8:
	*q++ = imgData->lookup[*p];
	break;
      case splashModeRGB8:
      case splashModeBGR8:
	col = &imgData->lookup[3 * *p];
	*q++ = col[0];
	*q++ = col[1];
	*q++ = col[2];
	break;
#if SPLASH_CMYK
      case splashModeCMYK8:
	col = &imgData->lookup[4 * *p];
	*q++ = col[0];
	*q++ = col[1];
	*q++ = col[2];
	*q++ = col[3];
	break;
#endif
      }
      *aq++ = alpha;
    } else {
      switch (imgData->colorMode) {
      case splashModeMono1:
      case splashModeMono8:
	imgData->colorMap->getGray(p, &gray);
	*q++ = colToByte(gray);
	break;
      case splashModeRGB8:
      case splashModeBGR8:
	imgData->colorMap->getRGB(p, &rgb);
	*q++ = colToByte(rgb.r);
	*q++ = colToByte(rgb.g);
	*q++ = colToByte(rgb.b);
	break;
#if SPLASH_CMYK
      case splashModeCMYK8:
	imgData->colorMap->getCMYK(p, &cmyk);
	*q++ = colToByte(cmyk.c);
	*q++ = colToByte(cmyk.m);
	*q++ = colToByte(cmyk.y);
	*q++ = colToByte(cmyk.k);
	break;
#endif
      }
      *aq++ = alpha;
    }
  }

  ++imgData->y;
  return gTrue;
}

void SplashOutputDev::drawImage(GfxState *state, Object *ref, Stream *str,
				int width, int height,
				GfxImageColorMap *colorMap,
				int *maskColors, GBool inlineImg,
				GBool interpolate) {
  double *ctm;
  SplashCoord mat[6];
  SplashOutImageData imgData;
  SplashColorMode srcMode;
  SplashImageSource src;
  GfxGray gray;
  GfxRGB rgb;
#if SPLASH_CMYK
  GfxCMYK cmyk;
#endif
  Guchar pix;
  int n, i;

  setOverprintMask(colorMap->getColorSpace(), state->getFillOverprint(),
		   state->getOverprintMode(), NULL);

  ctm = state->getCTM();
  mat[0] = ctm[0];
  mat[1] = ctm[1];
  mat[2] = -ctm[2];
  mat[3] = -ctm[3];
  mat[4] = ctm[2] + ctm[4];
  mat[5] = ctm[3] + ctm[5];

  reduceImageResolution(str, ctm, &width, &height);

  imgData.imgStr = new ImageStream(str, width,
				   colorMap->getNumPixelComps(),
				   colorMap->getBits());
  imgData.imgStr->reset();
  imgData.colorMap = colorMap;
  imgData.maskColors = maskColors;
  imgData.colorMode = colorMode;
  imgData.width = width;
  imgData.height = height;
  imgData.y = 0;

  // special case for one-channel (monochrome/gray/separation) images:
  // build a lookup table here
  imgData.lookup = NULL;
  if (colorMap->getNumPixelComps() == 1) {
    n = 1 << colorMap->getBits();
    switch (colorMode) {
    case splashModeMono1:
    case splashModeMono8:
      imgData.lookup = (SplashColorPtr)gmalloc(n);
      for (i = 0; i < n; ++i) {
	pix = (Guchar)i;
	colorMap->getGray(&pix, &gray);
	imgData.lookup[i] = colToByte(gray);
      }
      break;
    case splashModeRGB8:
    case splashModeBGR8:
      imgData.lookup = (SplashColorPtr)gmallocn(n, 3);
      for (i = 0; i < n; ++i) {
	pix = (Guchar)i;
	colorMap->getRGB(&pix, &rgb);
	imgData.lookup[3*i] = colToByte(rgb.r);
	imgData.lookup[3*i+1] = colToByte(rgb.g);
	imgData.lookup[3*i+2] = colToByte(rgb.b);
      }
      break;
#if SPLASH_CMYK
    case splashModeCMYK8:
      imgData.lookup = (SplashColorPtr)gmallocn(n, 4);
      for (i = 0; i < n; ++i) {
	pix = (Guchar)i;
	colorMap->getCMYK(&pix, &cmyk);
	imgData.lookup[4*i] = colToByte(cmyk.c);
	imgData.lookup[4*i+1] = colToByte(cmyk.m);
	imgData.lookup[4*i+2] = colToByte(cmyk.y);
	imgData.lookup[4*i+3] = colToByte(cmyk.k);
      }
      break;
#endif
    }
  }

  if (colorMode == splashModeMono1) {
    srcMode = splashModeMono8;
  } else if (colorMode == splashModeBGR8) {
    srcMode = splashModeRGB8;
  } else {
    srcMode = colorMode;
  }
  src = maskColors ? &alphaImageSrc : &imageSrc;
  splash->drawImage(src, &imgData, srcMode, maskColors ? gTrue : gFalse,
		    width, height, mat, interpolate);
  if (inlineImg) {
    while (imgData.y < height) {
      imgData.imgStr->getLine();
      ++imgData.y;
    }
  }

  gfree(imgData.lookup);
  delete imgData.imgStr;
  str->close();
}

struct SplashOutMaskedImageData {
  ImageStream *imgStr;
  GfxImageColorMap *colorMap;
  SplashBitmap *mask;
  SplashColorPtr lookup;
  SplashColorMode colorMode;
  int width, height, y;
};

GBool SplashOutputDev::maskedImageSrc(void *data, SplashColorPtr colorLine,
				      Guchar *alphaLine) {
  SplashOutMaskedImageData *imgData = (SplashOutMaskedImageData *)data;
  Guchar *p, *aq;
  SplashColorPtr q, col;
  GfxRGB rgb;
  GfxGray gray;
#if SPLASH_CMYK
  GfxCMYK cmyk;
#endif
  static Guchar bitToByte[2] = {0x00, 0xff};
  Guchar alpha;
  Guchar *maskPtr;
  int maskShift;
  int nComps, x;

  if (imgData->y == imgData->height ||
      !(p = imgData->imgStr->getLine())) {
    memset(colorLine, 0,
	   imgData->width * splashColorModeNComps[imgData->colorMode]);
    memset(alphaLine, 0, imgData->width);
    return gFalse;
  }

  nComps = imgData->colorMap->getNumPixelComps();

  maskPtr = imgData->mask->getDataPtr() +
              imgData->y * imgData->mask->getRowSize();
  maskShift = 7;
  for (x = 0, q = colorLine, aq = alphaLine;
       x < imgData->width;
       ++x, p += nComps) {
    alpha = bitToByte[(*maskPtr >> maskShift) & 1];
    maskPtr += (8 - maskShift) >> 3;
    maskShift = (maskShift - 1) & 7;
    if (imgData->lookup) {
      switch (imgData->colorMode) {
      case splashModeMono1:
      case splashModeMono8:
	*q++ = imgData->lookup[*p];
	break;
      case splashModeRGB8:
      case splashModeBGR8:
	col = &imgData->lookup[3 * *p];
	*q++ = col[0];
	*q++ = col[1];
	*q++ = col[2];
	break;
#if SPLASH_CMYK
      case splashModeCMYK8:
	col = &imgData->lookup[4 * *p];
	*q++ = col[0];
	*q++ = col[1];
	*q++ = col[2];
	*q++ = col[3];
	break;
#endif
      }
      *aq++ = alpha;
    } else {
      switch (imgData->colorMode) {
      case splashModeMono1:
      case splashModeMono8:
	imgData->colorMap->getGray(p, &gray);
	*q++ = colToByte(gray);
	break;
      case splashModeRGB8:
      case splashModeBGR8:
	imgData->colorMap->getRGB(p, &rgb);
	*q++ = colToByte(rgb.r);
	*q++ = colToByte(rgb.g);
	*q++ = colToByte(rgb.b);
	break;
#if SPLASH_CMYK
      case splashModeCMYK8:
	imgData->colorMap->getCMYK(p, &cmyk);
	*q++ = colToByte(cmyk.c);
	*q++ = colToByte(cmyk.m);
	*q++ = colToByte(cmyk.y);
	*q++ = colToByte(cmyk.k);
	break;
#endif
      }
      *aq++ = alpha;
    }
  }

  ++imgData->y;
  return gTrue;
}

void SplashOutputDev::drawMaskedImage(GfxState *state, Object *ref,
				      Stream *str, int width, int height,
				      GfxImageColorMap *colorMap,
				      Stream *maskStr, int maskWidth,
				      int maskHeight, GBool maskInvert,
				      GBool interpolate) {
  GfxImageColorMap *maskColorMap;
  Object maskDecode, decodeLow, decodeHigh;
  double *ctm;
  SplashCoord mat[6];
  SplashOutMaskedImageData imgData;
  SplashOutImageMaskData imgMaskData;
  SplashColorMode srcMode;
  SplashBitmap *maskBitmap;
  Splash *maskSplash;
  SplashColor maskColor;
  GfxGray gray;
  GfxRGB rgb;
#if SPLASH_CMYK
  GfxCMYK cmyk;
#endif
  Guchar pix;
  int n, i;

  setOverprintMask(colorMap->getColorSpace(), state->getFillOverprint(),
		   state->getOverprintMode(), NULL);

  ctm = state->getCTM();
  reduceImageResolution(str, ctm, &width, &height);
  reduceImageResolution(maskStr, ctm, &maskWidth, &maskHeight);

  // If the mask is higher resolution than the image, use
  // drawSoftMaskedImage() instead.
  if (maskWidth > width || maskHeight > height) {
    decodeLow.initInt(maskInvert ? 0 : 1);
    decodeHigh.initInt(maskInvert ? 1 : 0);
    maskDecode.initArray(xref);
    maskDecode.arrayAdd(&decodeLow);
    maskDecode.arrayAdd(&decodeHigh);
    maskColorMap = new GfxImageColorMap(1, &maskDecode,
					new GfxDeviceGrayColorSpace());
    maskDecode.free();
    drawSoftMaskedImage(state, ref, str, width, height, colorMap,
			maskStr, maskWidth, maskHeight, maskColorMap,
			interpolate);
    delete maskColorMap;

  } else {

    //----- scale the mask image to the same size as the source image

    mat[0] = (SplashCoord)width;
    mat[1] = 0;
    mat[2] = 0;
    mat[3] = (SplashCoord)height;
    mat[4] = 0;
    mat[5] = 0;
    imgMaskData.imgStr = new ImageStream(maskStr, maskWidth, 1, 1);
    imgMaskData.imgStr->reset();
    imgMaskData.invert = maskInvert ? 0 : 1;
    imgMaskData.width = maskWidth;
    imgMaskData.height = maskHeight;
    imgMaskData.y = 0;
    maskBitmap = new SplashBitmap(width, height, 1, splashModeMono1, gFalse);
    maskSplash = new Splash(maskBitmap, gFalse);
    maskSplash->setStrokeAdjust(globalParams->getStrokeAdjust());
    maskColor[0] = 0;
    maskSplash->clear(maskColor);
    maskColor[0] = 0xff;
    maskSplash->setFillPattern(new SplashSolidColor(maskColor));
    // use "glyph mode" here to get the correct scaled size
    maskSplash->fillImageMask(&imageMaskSrc, &imgMaskData,
			      maskWidth, maskHeight, mat, gTrue, interpolate);
    delete imgMaskData.imgStr;
    maskStr->close();
    delete maskSplash;

    //----- draw the source image

    mat[0] = ctm[0];
    mat[1] = ctm[1];
    mat[2] = -ctm[2];
    mat[3] = -ctm[3];
    mat[4] = ctm[2] + ctm[4];
    mat[5] = ctm[3] + ctm[5];

    imgData.imgStr = new ImageStream(str, width,
				     colorMap->getNumPixelComps(),
				     colorMap->getBits());
    imgData.imgStr->reset();
    imgData.colorMap = colorMap;
    imgData.mask = maskBitmap;
    imgData.colorMode = colorMode;
    imgData.width = width;
    imgData.height = height;
    imgData.y = 0;

    // special case for one-channel (monochrome/gray/separation) images:
    // build a lookup table here
    imgData.lookup = NULL;
    if (colorMap->getNumPixelComps() == 1) {
      n = 1 << colorMap->getBits();
      switch (colorMode) {
      case splashModeMono1:
      case splashModeMono8:
	imgData.lookup = (SplashColorPtr)gmalloc(n);
	for (i = 0; i < n; ++i) {
	  pix = (Guchar)i;
	  colorMap->getGray(&pix, &gray);
	  imgData.lookup[i] = colToByte(gray);
	}
	break;
      case splashModeRGB8:
      case splashModeBGR8:
	imgData.lookup = (SplashColorPtr)gmallocn(n, 3);
	for (i = 0; i < n; ++i) {
	  pix = (Guchar)i;
	  colorMap->getRGB(&pix, &rgb);
	  imgData.lookup[3*i] = colToByte(rgb.r);
	  imgData.lookup[3*i+1] = colToByte(rgb.g);
	  imgData.lookup[3*i+2] = colToByte(rgb.b);
	}
	break;
#if SPLASH_CMYK
      case splashModeCMYK8:
	imgData.lookup = (SplashColorPtr)gmallocn(n, 4);
	for (i = 0; i < n; ++i) {
	  pix = (Guchar)i;
	  colorMap->getCMYK(&pix, &cmyk);
	  imgData.lookup[4*i] = colToByte(cmyk.c);
	  imgData.lookup[4*i+1] = colToByte(cmyk.m);
	  imgData.lookup[4*i+2] = colToByte(cmyk.y);
	  imgData.lookup[4*i+3] = colToByte(cmyk.k);
	}
	break;
#endif
      }
    }

    if (colorMode == splashModeMono1) {
      srcMode = splashModeMono8;
    } else if (colorMode == splashModeBGR8) {
      srcMode = splashModeRGB8;
    } else {
      srcMode = colorMode;
    }
    splash->drawImage(&maskedImageSrc, &imgData, srcMode, gTrue,
		      width, height, mat, interpolate);

    delete maskBitmap;
    gfree(imgData.lookup);
    delete imgData.imgStr;
    str->close();
  }
}

void SplashOutputDev::drawSoftMaskedImage(GfxState *state, Object *ref,
					  Stream *str, int width, int height,
					  GfxImageColorMap *colorMap,
					  Stream *maskStr,
					  int maskWidth, int maskHeight,
					  GfxImageColorMap *maskColorMap,
					  GBool interpolate) {
  double *ctm;
  SplashCoord mat[6];
  SplashOutImageData imgData;
  SplashOutImageData imgMaskData;
  SplashColorMode srcMode;
  SplashBitmap *maskBitmap;
  Splash *maskSplash;
  GfxGray gray;
  GfxRGB rgb;
#if SPLASH_CMYK
  GfxCMYK cmyk;
#endif
  Guchar pix;
  int n, i;

  setOverprintMask(colorMap->getColorSpace(), state->getFillOverprint(),
		   state->getOverprintMode(), NULL);

  ctm = state->getCTM();
  mat[0] = ctm[0];
  mat[1] = ctm[1];
  mat[2] = -ctm[2];
  mat[3] = -ctm[3];
  mat[4] = ctm[2] + ctm[4];
  mat[5] = ctm[3] + ctm[5];

  reduceImageResolution(str, ctm, &width, &height);
  reduceImageResolution(maskStr, ctm, &maskWidth, &maskHeight);

  //----- set up the soft mask

  imgMaskData.imgStr = new ImageStream(maskStr, maskWidth,
				       maskColorMap->getNumPixelComps(),
				       maskColorMap->getBits());
  imgMaskData.imgStr->reset();
  imgMaskData.colorMap = maskColorMap;
  imgMaskData.maskColors = NULL;
  imgMaskData.colorMode = splashModeMono8;
  imgMaskData.width = maskWidth;
  imgMaskData.height = maskHeight;
  imgMaskData.y = 0;
  n = 1 << maskColorMap->getBits();
  imgMaskData.lookup = (SplashColorPtr)gmalloc(n);
  for (i = 0; i < n; ++i) {
    pix = (Guchar)i;
    maskColorMap->getGray(&pix, &gray);
    imgMaskData.lookup[i] = colToByte(gray);
  }
  maskBitmap = new SplashBitmap(bitmap->getWidth(), bitmap->getHeight(),
				1, splashModeMono8, gFalse);
  maskSplash = new Splash(maskBitmap, vectorAntialias);
  maskSplash->setStrokeAdjust(globalParams->getStrokeAdjust());
  clearMaskRegion(state, maskSplash, 0, 0, 1, 1);
  maskSplash->drawImage(&imageSrc, &imgMaskData, splashModeMono8, gFalse,
			maskWidth, maskHeight, mat, interpolate);
  delete imgMaskData.imgStr;
  maskStr->close();
  gfree(imgMaskData.lookup);
  delete maskSplash;
  splash->setSoftMask(maskBitmap);

  //----- draw the source image

  imgData.imgStr = new ImageStream(str, width,
				   colorMap->getNumPixelComps(),
				   colorMap->getBits());
  imgData.imgStr->reset();
  imgData.colorMap = colorMap;
  imgData.maskColors = NULL;
  imgData.colorMode = colorMode;
  imgData.width = width;
  imgData.height = height;
  imgData.y = 0;

  // special case for one-channel (monochrome/gray/separation) images:
  // build a lookup table here
  imgData.lookup = NULL;
  if (colorMap->getNumPixelComps() == 1) {
    n = 1 << colorMap->getBits();
    switch (colorMode) {
    case splashModeMono1:
    case splashModeMono8:
      imgData.lookup = (SplashColorPtr)gmalloc(n);
      for (i = 0; i < n; ++i) {
	pix = (Guchar)i;
	colorMap->getGray(&pix, &gray);
	imgData.lookup[i] = colToByte(gray);
      }
      break;
    case splashModeRGB8:
    case splashModeBGR8:
      imgData.lookup = (SplashColorPtr)gmallocn(n, 3);
      for (i = 0; i < n; ++i) {
	pix = (Guchar)i;
	colorMap->getRGB(&pix, &rgb);
	imgData.lookup[3*i] = colToByte(rgb.r);
	imgData.lookup[3*i+1] = colToByte(rgb.g);
	imgData.lookup[3*i+2] = colToByte(rgb.b);
      }
      break;
#if SPLASH_CMYK
    case splashModeCMYK8:
      imgData.lookup = (SplashColorPtr)gmallocn(n, 4);
      for (i = 0; i < n; ++i) {
	pix = (Guchar)i;
	colorMap->getCMYK(&pix, &cmyk);
	imgData.lookup[4*i] = colToByte(cmyk.c);
	imgData.lookup[4*i+1] = colToByte(cmyk.m);
	imgData.lookup[4*i+2] = colToByte(cmyk.y);
	imgData.lookup[4*i+3] = colToByte(cmyk.k);
      }
      break;
#endif
    }
  }

  if (colorMode == splashModeMono1) {
    srcMode = splashModeMono8;
  } else if (colorMode == splashModeBGR8) {
    srcMode = splashModeRGB8;
  } else {
    srcMode = colorMode;
  }
  splash->drawImage(&imageSrc, &imgData, srcMode, gFalse, width, height, mat,
		    interpolate);

  splash->setSoftMask(NULL);
  gfree(imgData.lookup);
  delete imgData.imgStr;
  str->close();
}

void SplashOutputDev::reduceImageResolution(Stream *str, double *ctm,
					    int *width, int *height) {
  double sw, sh;
  int reduction;

  if (str->getKind() == strJPX &&
      *width * *height > 10000000) {
    sw = (double)*width / (fabs(ctm[2]) + fabs(ctm[3]));
    sh = (double)*height / (fabs(ctm[0]) + fabs(ctm[1]));
    if (sw > 8 && sh > 8) {
      reduction = 3;
    } else if (sw > 4 && sh > 4) {
      reduction = 2;
    } else if (sw > 2 && sh > 2) {
      reduction = 1;
    } else {
      reduction = 0;
    }
    if (reduction > 0) {
      ((JPXStream *)str)->reduceResolution(reduction);
      *width >>= reduction;
      *height >>= reduction;
    }
  }
}

void SplashOutputDev::clearMaskRegion(GfxState *state,
				      Splash *maskSplash,
				      double xMin, double yMin,
				      double xMax, double yMax) {
  SplashBitmap *maskBitmap;
  double xxMin, yyMin, xxMax, yyMax, xx, yy;
  int xxMinI, yyMinI, xxMaxI, yyMaxI, y, n;
  Guchar *p;

  maskBitmap = maskSplash->getBitmap();
  xxMin = maskBitmap->getWidth();
  xxMax = 0;
  yyMin = maskBitmap->getHeight();
  yyMax = 0;
  state->transform(xMin, yMin, &xx, &yy);
  if (xx < xxMin) { xxMin = xx; }
  if (xx > xxMax) { xxMax = xx; }
  if (yy < yyMin) { yyMin = yy; }
  if (yy > yyMax) { yyMax = yy; }
  state->transform(xMin, yMax, &xx, &yy);
  if (xx < xxMin) { xxMin = xx; }
  if (xx > xxMax) { xxMax = xx; }
  if (yy < yyMin) { yyMin = yy; }
  if (yy > yyMax) { yyMax = yy; }
  state->transform(xMax, yMin, &xx, &yy);
  if (xx < xxMin) { xxMin = xx; }
  if (xx > xxMax) { xxMax = xx; }
  if (yy < yyMin) { yyMin = yy; }
  if (yy > yyMax) { yyMax = yy; }
  state->transform(xMax, yMax, &xx, &yy);
  if (xx < xxMin) { xxMin = xx; }
  if (xx > xxMax) { xxMax = xx; }
  if (yy < yyMin) { yyMin = yy; }
  if (yy > yyMax) { yyMax = yy; }
  xxMinI = (int)floor(xxMin);
  if (xxMinI < 0) {
    xxMinI = 0;
  }
  xxMaxI = (int)ceil(xxMax);
  if (xxMaxI > maskBitmap->getWidth()) {
    xxMaxI = maskBitmap->getWidth();
  }
  yyMinI = (int)floor(yyMin);
  if (yyMinI < 0) {
    yyMinI = 0;
  }
  yyMaxI = (int)ceil(yyMax);
  if (yyMaxI > maskBitmap->getHeight()) {
    yyMaxI = maskBitmap->getHeight();
  }
  p = maskBitmap->getDataPtr() + yyMinI * maskBitmap->getRowSize();
  if (maskBitmap->getMode() == splashModeMono1) {
    n = (xxMaxI + 7) / 8 - xxMinI / 8;
    p += xxMinI / 8;
  } else {
    n = xxMaxI - xxMinI;
    p += xxMinI;
  }
  if (xxMaxI > xxMinI) {
    for (y = yyMinI; y < yyMaxI; ++y) {
      memset(p, 0, n);
      p += maskBitmap->getRowSize();
    }
  }
}

void SplashOutputDev::beginTransparencyGroup(GfxState *state, double *bbox,
					     GfxColorSpace *blendingColorSpace,
					     GBool isolated, GBool knockout,
					     GBool forSoftMask) {
  SplashTransparencyGroup *transpGroup;
  SplashColor color;
  double xMin, yMin, xMax, yMax, x, y;
  int tx, ty, w, h, i;

  // transform the bbox
  state->transform(bbox[0], bbox[1], &x, &y);
  xMin = xMax = x;
  yMin = yMax = y;
  state->transform(bbox[0], bbox[3], &x, &y);
  if (x < xMin) {
    xMin = x;
  } else if (x > xMax) {
    xMax = x;
  }
  if (y < yMin) {
    yMin = y;
  } else if (y > yMax) {
    yMax = y;
  }
  state->transform(bbox[2], bbox[1], &x, &y);
  if (x < xMin) {
    xMin = x;
  } else if (x > xMax) {
    xMax = x;
  }
  if (y < yMin) {
    yMin = y;
  } else if (y > yMax) {
    yMax = y;
  }
  state->transform(bbox[2], bbox[3], &x, &y);
  if (x < xMin) {
    xMin = x;
  } else if (x > xMax) {
    xMax = x;
  }
  if (y < yMin) {
    yMin = y;
  } else if (y > yMax) {
    yMax = y;
  }
  tx = (int)floor(xMin);
  if (tx < 0) {
    tx = 0;
  } else if (tx >= bitmap->getWidth()) {
    tx = bitmap->getWidth() - 1;
  }
  ty = (int)floor(yMin);
  if (ty < 0) {
    ty = 0;
  } else if (ty >= bitmap->getHeight()) {
    ty = bitmap->getHeight() - 1;
  }
  w = (int)ceil(xMax) - tx + 1;
  if (tx + w > bitmap->getWidth()) {
    w = bitmap->getWidth() - tx;
  }
  if (w < 1) {
    w = 1;
  }
  h = (int)ceil(yMax) - ty + 1;
  if (ty + h > bitmap->getHeight()) {
    h = bitmap->getHeight() - ty;
  }
  if (h < 1) {
    h = 1;
  }

  // push a new stack entry
  transpGroup = new SplashTransparencyGroup();
  transpGroup->tx = tx;
  transpGroup->ty = ty;
  transpGroup->blendingColorSpace = blendingColorSpace;
  transpGroup->isolated = isolated;
  transpGroup->next = transpGroupStack;
  transpGroupStack = transpGroup;

  // save state
  transpGroup->origBitmap = bitmap;
  transpGroup->origSplash = splash;

  //~ this handles the blendingColorSpace arg for soft masks, but
  //~   not yet for transparency groups

  // switch to the blending color space
  if (forSoftMask && isolated && !knockout && blendingColorSpace) {
    if (blendingColorSpace->getMode() == csDeviceGray ||
	blendingColorSpace->getMode() == csCalGray ||
	(blendingColorSpace->getMode() == csICCBased &&
	 blendingColorSpace->getNComps() == 1)) {
      colorMode = splashModeMono8;
    } else if (blendingColorSpace->getMode() == csDeviceRGB ||
	       blendingColorSpace->getMode() == csCalRGB ||
	       (blendingColorSpace->getMode() == csICCBased &&
		blendingColorSpace->getNComps() == 3)) {
      //~ does this need to use BGR8?
      colorMode = splashModeRGB8;
#if SPLASH_CMYK
    } else if (blendingColorSpace->getMode() == csDeviceCMYK ||
	       (blendingColorSpace->getMode() == csICCBased &&
		blendingColorSpace->getNComps() == 4)) {
      colorMode = splashModeCMYK8;
#endif
    }
  }

  // create the temporary bitmap
  bitmap = new SplashBitmap(w, h, bitmapRowPad, colorMode, gTrue,
			    bitmapTopDown); 
  splash = new Splash(bitmap, vectorAntialias,
		      transpGroup->origSplash->getScreen());
  splash->setMinLineWidth(globalParams->getMinLineWidth());
  splash->setStrokeAdjust(globalParams->getStrokeAdjust());
  //~ Acrobat apparently copies at least the fill and stroke colors, and
  //~ maybe other state(?) -- but not the clipping path (and not sure
  //~ what else)
  //~ [this is likely the same situation as in type3D1()]
  splash->setFillPattern(transpGroup->origSplash->getFillPattern()->copy());
  splash->setStrokePattern(
		         transpGroup->origSplash->getStrokePattern()->copy());
  if (isolated) {
    for (i = 0; i < splashMaxColorComps; ++i) {
      color[i] = 0;
    }
    splash->clear(color, 0);
  } else {
    splash->blitTransparent(transpGroup->origBitmap, tx, ty, 0, 0, w, h);
  }
  splash->setInTransparencyGroup(transpGroup->origBitmap, tx, ty,
				 !isolated, knockout);
  transpGroup->tBitmap = bitmap;
  state->shiftCTM(-tx, -ty);
  updateCTM(state, 0, 0, 0, 0, 0, 0);
  ++nestCount;
}

void SplashOutputDev::endTransparencyGroup(GfxState *state) {
  // restore state
  --nestCount;
  delete splash;
  bitmap = transpGroupStack->origBitmap;
  colorMode = bitmap->getMode();
  splash = transpGroupStack->origSplash;
  state->shiftCTM(transpGroupStack->tx, transpGroupStack->ty);
  updateCTM(state, 0, 0, 0, 0, 0, 0);
}

void SplashOutputDev::paintTransparencyGroup(GfxState *state, double *bbox) {
  SplashBitmap *tBitmap;
  SplashTransparencyGroup *transpGroup;
  GBool isolated;
  int tx, ty;

  tx = transpGroupStack->tx;
  ty = transpGroupStack->ty;
  tBitmap = transpGroupStack->tBitmap;
  isolated = transpGroupStack->isolated;

  // paint the transparency group onto the parent bitmap
  // - the clip path was set in the parent's state)
  if (tx < bitmap->getWidth() && ty < bitmap->getHeight()) {
    splash->setOverprintMask(0xffffffff);
    splash->composite(tBitmap, 0, 0, tx, ty,
		      tBitmap->getWidth(), tBitmap->getHeight(),
		      gFalse, !isolated);
  }

  // pop the stack
  transpGroup = transpGroupStack;
  transpGroupStack = transpGroup->next;
  delete transpGroup;

  delete tBitmap;
}

void SplashOutputDev::setSoftMask(GfxState *state, double *bbox,
				  GBool alpha, Function *transferFunc,
				  GfxColor *backdropColor) {
  SplashBitmap *softMask, *tBitmap;
  Splash *tSplash;
  SplashTransparencyGroup *transpGroup;
  SplashColor color;
  SplashColorPtr p;
  GfxGray gray;
  GfxRGB rgb;
#if SPLASH_CMYK
  GfxCMYK cmyk;
#endif
  double backdrop, backdrop2, lum, lum2;
  int tx, ty, x, y;

  tx = transpGroupStack->tx;
  ty = transpGroupStack->ty;
  tBitmap = transpGroupStack->tBitmap;

  // composite with backdrop color
  backdrop = 0;
  if (!alpha && tBitmap->getMode() != splashModeMono1) {
    //~ need to correctly handle the case where no blending color
    //~ space is given
    tSplash = new Splash(tBitmap, vectorAntialias,
			 transpGroupStack->origSplash->getScreen());
    tSplash->setStrokeAdjust(globalParams->getStrokeAdjust());
    if (transpGroupStack->blendingColorSpace) {
      switch (tBitmap->getMode()) {
      case splashModeMono1:
	// transparency is not supported in mono1 mode
	break;
      case splashModeMono8:
	transpGroupStack->blendingColorSpace->getGray(backdropColor, &gray);
	backdrop = colToDbl(gray);
	color[0] = colToByte(gray);
	tSplash->compositeBackground(color);
	break;
      case splashModeRGB8:
      case splashModeBGR8:
	transpGroupStack->blendingColorSpace->getRGB(backdropColor, &rgb);
	backdrop = 0.3 * colToDbl(rgb.r) +
	           0.59 * colToDbl(rgb.g) +
	           0.11 * colToDbl(rgb.b);
	color[0] = colToByte(rgb.r);
	color[1] = colToByte(rgb.g);
	color[2] = colToByte(rgb.b);
	tSplash->compositeBackground(color);
	break;
#if SPLASH_CMYK
      case splashModeCMYK8:
	transpGroupStack->blendingColorSpace->getCMYK(backdropColor, &cmyk);
	backdrop = (1 - colToDbl(cmyk.k))
	           - 0.3 * colToDbl(cmyk.c)
	           - 0.59 * colToDbl(cmyk.m)
	           - 0.11 * colToDbl(cmyk.y);
	if (backdrop < 0) {
	  backdrop = 0;
	}
	color[0] = colToByte(cmyk.c);
	color[1] = colToByte(cmyk.m);
	color[2] = colToByte(cmyk.y);
	color[3] = colToByte(cmyk.k);
	tSplash->compositeBackground(color);
	break;
#endif
      }
      delete tSplash;
    }
  }
  if (transferFunc) {
    transferFunc->transform(&backdrop, &backdrop2);
  } else {
    backdrop2 = backdrop;
  }

  softMask = new SplashBitmap(bitmap->getWidth(), bitmap->getHeight(),
			      1, splashModeMono8, gFalse);
  memset(softMask->getDataPtr(), (int)(backdrop2 * 255.0 + 0.5),
	 softMask->getRowSize() * softMask->getHeight());
  if (tx < softMask->getWidth() && ty < softMask->getHeight()) {
    p = softMask->getDataPtr() + ty * softMask->getRowSize() + tx;
    for (y = 0; y < tBitmap->getHeight(); ++y) {
      for (x = 0; x < tBitmap->getWidth(); ++x) {
	if (alpha) {
	  lum = tBitmap->getAlpha(x, y) / 255.0;
	} else {
	  tBitmap->getPixel(x, y, color);
	  // convert to luminosity
	  switch (tBitmap->getMode()) {
	  case splashModeMono1:
	  case splashModeMono8:
	    lum = color[0] / 255.0;
	    break;
	  case splashModeRGB8:
	  case splashModeBGR8:
	    lum = (0.3 / 255.0) * color[0] +
	          (0.59 / 255.0) * color[1] +
	          (0.11 / 255.0) * color[2];
	    break;
#if SPLASH_CMYK
	  case splashModeCMYK8:
	    lum = (1 - color[3] / 255.0)
	          - (0.3 / 255.0) * color[0]
	          - (0.59 / 255.0) * color[1]
	          - (0.11 / 255.0) * color[2];
	    if (lum < 0) {
	      lum = 0;
	    }
	    break;
#endif
	  }
	}
	if (transferFunc) {
	  transferFunc->transform(&lum, &lum2);
	} else {
	  lum2 = lum;
	}
	p[x] = (int)(lum2 * 255.0 + 0.5);
      }
      p += softMask->getRowSize();
    }
  }
  splash->setSoftMask(softMask);

  // pop the stack
  transpGroup = transpGroupStack;
  transpGroupStack = transpGroup->next;
  delete transpGroup;

  delete tBitmap;
}

void SplashOutputDev::clearSoftMask(GfxState *state) {
  splash->setSoftMask(NULL);
}

void SplashOutputDev::setPaperColor(SplashColorPtr paperColorA) {
  splashColorCopy(paperColor, paperColorA);
}

int SplashOutputDev::getBitmapWidth() {
  return bitmap->getWidth();
}

int SplashOutputDev::getBitmapHeight() {
  return bitmap->getHeight();
}

SplashBitmap *SplashOutputDev::takeBitmap() {
  SplashBitmap *ret;

  ret = bitmap;
  bitmap = new SplashBitmap(1, 1, bitmapRowPad, colorMode,
			    colorMode != splashModeMono1, bitmapTopDown);
  return ret;
}

void SplashOutputDev::getModRegion(int *xMin, int *yMin,
				   int *xMax, int *yMax) {
  splash->getModRegion(xMin, yMin, xMax, yMax);
}

void SplashOutputDev::clearModRegion() {
  splash->clearModRegion();
}

void SplashOutputDev::setFillColor(int r, int g, int b) {
  GfxRGB rgb;
  GfxGray gray;
#if SPLASH_CMYK
  GfxCMYK cmyk;
#endif

  rgb.r = byteToCol(r);
  rgb.g = byteToCol(g);
  rgb.b = byteToCol(b);
  switch (colorMode) {
  case splashModeMono1:
  case splashModeMono8:
    gray = (GfxColorComp)(0.299 * rgb.r + 0.587 * rgb.g + 0.114 * rgb.g + 0.5);
    if (gray > gfxColorComp1) {
      gray = gfxColorComp1;
    }
    splash->setFillPattern(getColor(gray));
    break;
  case splashModeRGB8:
  case splashModeBGR8:
    splash->setFillPattern(getColor(&rgb));
    break;
#if SPLASH_CMYK
  case splashModeCMYK8:
    cmyk.c = gfxColorComp1 - rgb.r;
    cmyk.m = gfxColorComp1 - rgb.g;
    cmyk.y = gfxColorComp1 - rgb.b;
    cmyk.k = 0;
    splash->setFillPattern(getColor(&cmyk));
    break;
#endif
  }
}

SplashFont *SplashOutputDev::getFont(GString *name, SplashCoord *textMatA) {
  Ref ref;
  SplashOutFontFileID *id;
  GfxFontLoc *fontLoc;
#if LOAD_FONTS_FROM_MEM
  GString *fontBuf;
  FILE *extFontFile;
  char blk[4096];
  int n;
#endif
  SplashFontFile *fontFile;
  SplashFont *fontObj;
  FoFiTrueType *ff;
  int *codeToGID;
  Unicode u;
  SplashCoord textMat[4];
  SplashCoord oblique;
  int cmap, i;

  for (i = 0; i < nBuiltinFonts; ++i) {
    if (!name->cmp(builtinFonts[i].name)) {
      break;
    }
  }
  if (i == nBuiltinFonts) {
    return NULL;
  }
  ref.num = i;
  ref.gen = -1;
  id = new SplashOutFontFileID(&ref);

  // check the font file cache
  if ((fontFile = fontEngine->getFontFile(id))) {
    delete id;

  // load the font file
  } else {
    if (!(fontLoc = GfxFont::locateBase14Font(name))) {
      return NULL;
    }
#if LOAD_FONTS_FROM_MEM
    fontBuf = NULL;
    if (fontLoc->fontType == fontType1 ||
	fontLoc->fontType == fontTrueType) {
      if (!(extFontFile = fopen(fontLoc->path->getCString(), "rb"))) {
	delete fontLoc;
	delete id;
	return NULL;
      }
      fontBuf = new GString();
      while ((n = fread(blk, 1, sizeof(blk), extFontFile)) > 0) {
	fontBuf->append(blk, n);
      }
      fclose(extFontFile);
    }
#endif
    if (fontLoc->fontType == fontType1) {
      fontFile = fontEngine->loadType1Font(id,
#if LOAD_FONTS_FROM_MEM
					   fontBuf,
#else
					   fontLoc->path->getCString(),
					   gFalse,
#endif
					   winAnsiEncoding);
    } else if (fontLoc->fontType == fontTrueType) {
#if LOAD_FONTS_FROM_MEM
      if (!(ff = FoFiTrueType::make(fontBuf->getCString(),
				    fontBuf->getLength(),
				    fontLoc->fontNum))) {
#else
      if (!(ff = FoFiTrueType::load(fontLoc->path->getCString(),
				    fontLoc->fontNum))) {
#endif
	delete fontLoc;
	delete id;
	return NULL;
      }
      for (cmap = 0; cmap < ff->getNumCmaps(); ++cmap) {
	if ((ff->getCmapPlatform(cmap) == 3 &&
	     ff->getCmapEncoding(cmap) == 1) ||
	    ff->getCmapPlatform(cmap) == 0) {
	  break;
	}
      }
      if (cmap == ff->getNumCmaps()) {
	delete ff;
	delete fontLoc;
	delete id;
	return NULL;
      }
      codeToGID = (int *)gmallocn(256, sizeof(int));
      for (i = 0; i < 256; ++i) {
	codeToGID[i] = 0;
	if (winAnsiEncoding[i] &&
	    (u = globalParams->mapNameToUnicode(winAnsiEncoding[i]))) {
	  codeToGID[i] = ff->mapCodeToGID(cmap, u);
	}
      }
      delete ff;
      fontFile = fontEngine->loadTrueTypeFont(id,
#if LOAD_FONTS_FROM_MEM
					      fontBuf,
#else
					      fontLoc->path->getCString(),
					      gFalse,
#endif
					      fontLoc->fontNum,
					      codeToGID, 256, NULL);
    } else {
      delete fontLoc;
      delete id;
      return NULL;
    }
    delete fontLoc;
  }
  if (!fontFile) {
    return NULL;
  }

  // create the scaled font
  oblique = (SplashCoord)
              ((SplashOutFontFileID *)fontFile->getID())->getOblique();
  textMat[0] = (SplashCoord)textMatA[0];
  textMat[1] = (SplashCoord)textMatA[1];
  textMat[2] = oblique * textMatA[0] + textMatA[2];
  textMat[3] = oblique * textMatA[1] + textMatA[3];
  fontObj = fontEngine->getFont(fontFile, textMat, splash->getMatrix());

  return fontObj;
}

#if 1 //~tmp: turn off anti-aliasing temporarily
void SplashOutputDev::setInShading(GBool sh) {
  splash->setInShading(sh);
}
#endif
