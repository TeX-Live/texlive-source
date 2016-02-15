//========================================================================
//
// SplashState.cc
//
// Copyright 2003-2013 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <string.h>
#include "gmem.h"
#include "SplashPattern.h"
#include "SplashScreen.h"
#include "SplashClip.h"
#include "SplashBitmap.h"
#include "SplashState.h"

//------------------------------------------------------------------------
// SplashState
//------------------------------------------------------------------------

// number of components in each color mode
int splashColorModeNComps[] = {
  1, 1, 3, 3
#if SPLASH_CMYK
  , 4
#endif
};

SplashState::SplashState(int width, int height, GBool vectorAntialias,
			 SplashScreenParams *screenParams) {
  SplashColor color;
  int i;

  matrix[0] = 1;  matrix[1] = 0;
  matrix[2] = 0;  matrix[3] = 1;
  matrix[4] = 0;  matrix[5] = 0;
  memset(&color, 0, sizeof(SplashColor));
  strokePattern = new SplashSolidColor(color);
  fillPattern = new SplashSolidColor(color);
  screen = new SplashScreen(screenParams);
  blendFunc = NULL;
  strokeAlpha = 1;
  fillAlpha = 1;
  lineWidth = 1;
  lineCap = splashLineCapButt;
  lineJoin = splashLineJoinMiter;
  miterLimit = 10;
  flatness = 1;
  lineDash = NULL;
  lineDashLength = 0;
  lineDashPhase = 0;
  strokeAdjust = gFalse;
  clip = new SplashClip(0, 0, width, height);
  clipIsShared = gFalse;
  softMask = NULL;
  deleteSoftMask = gFalse;
  inNonIsolatedGroup = gFalse;
  inKnockoutGroup = gFalse;
  for (i = 0; i < 256; ++i) {
    rgbTransferR[i] = (Guchar)i;
    rgbTransferG[i] = (Guchar)i;
    rgbTransferB[i] = (Guchar)i;
    grayTransfer[i] = (Guchar)i;
    cmykTransferC[i] = (Guchar)i;
    cmykTransferM[i] = (Guchar)i;
    cmykTransferY[i] = (Guchar)i;
    cmykTransferK[i] = (Guchar)i;
  }
  overprintMask = 0xffffffff;
  next = NULL;
}

SplashState::SplashState(int width, int height, GBool vectorAntialias,
			 SplashScreen *screenA) {
  SplashColor color;
  int i;

  matrix[0] = 1;  matrix[1] = 0;
  matrix[2] = 0;  matrix[3] = 1;
  matrix[4] = 0;  matrix[5] = 0;
  memset(&color, 0, sizeof(SplashColor));
  strokePattern = new SplashSolidColor(color);
  fillPattern = new SplashSolidColor(color);
  screen = screenA->copy();
  blendFunc = NULL;
  strokeAlpha = 1;
  fillAlpha = 1;
  lineWidth = 1;
  lineCap = splashLineCapButt;
  lineJoin = splashLineJoinMiter;
  miterLimit = 10;
  flatness = 1;
  lineDash = NULL;
  lineDashLength = 0;
  lineDashPhase = 0;
  strokeAdjust = gFalse;
  clip = new SplashClip(0, 0, width, height);
  clipIsShared = gFalse;
  softMask = NULL;
  deleteSoftMask = gFalse;
  inNonIsolatedGroup = gFalse;
  inKnockoutGroup = gFalse;
  for (i = 0; i < 256; ++i) {
    rgbTransferR[i] = (Guchar)i;
    rgbTransferG[i] = (Guchar)i;
    rgbTransferB[i] = (Guchar)i;
    grayTransfer[i] = (Guchar)i;
    cmykTransferC[i] = (Guchar)i;
    cmykTransferM[i] = (Guchar)i;
    cmykTransferY[i] = (Guchar)i;
    cmykTransferK[i] = (Guchar)i;
  }
  overprintMask = 0xffffffff;
  next = NULL;
}

SplashState::SplashState(SplashState *state) {
  memcpy(matrix, state->matrix, 6 * sizeof(SplashCoord));
  strokePattern = state->strokePattern->copy();
  fillPattern = state->fillPattern->copy();
  screen = state->screen->copy();
  blendFunc = state->blendFunc;
  strokeAlpha = state->strokeAlpha;
  fillAlpha = state->fillAlpha;
  lineWidth = state->lineWidth;
  lineCap = state->lineCap;
  lineJoin = state->lineJoin;
  miterLimit = state->miterLimit;
  flatness = state->flatness;
  if (state->lineDash) {
    lineDashLength = state->lineDashLength;
    lineDash = (SplashCoord *)gmallocn(lineDashLength, sizeof(SplashCoord));
    memcpy(lineDash, state->lineDash, lineDashLength * sizeof(SplashCoord));
  } else {
    lineDash = NULL;
    lineDashLength = 0;
  }
  lineDashPhase = state->lineDashPhase;
  strokeAdjust = state->strokeAdjust;
  clip = state->clip;
  clipIsShared = gTrue;
  softMask = state->softMask;
  deleteSoftMask = gFalse;
  inNonIsolatedGroup = state->inNonIsolatedGroup;
  inKnockoutGroup = state->inKnockoutGroup;
  memcpy(rgbTransferR, state->rgbTransferR, 256);
  memcpy(rgbTransferG, state->rgbTransferG, 256);
  memcpy(rgbTransferB, state->rgbTransferB, 256);
  memcpy(grayTransfer, state->grayTransfer, 256);
  memcpy(cmykTransferC, state->cmykTransferC, 256);
  memcpy(cmykTransferM, state->cmykTransferM, 256);
  memcpy(cmykTransferY, state->cmykTransferY, 256);
  memcpy(cmykTransferK, state->cmykTransferK, 256);
  overprintMask = state->overprintMask;
  next = NULL;
}

SplashState::~SplashState() {
  delete strokePattern;
  delete fillPattern;
  delete screen;
  gfree(lineDash);
  if (!clipIsShared) {
    delete clip;
  }
  if (deleteSoftMask && softMask) {
    delete softMask;
  }
}

void SplashState::setStrokePattern(SplashPattern *strokePatternA) {
  delete strokePattern;
  strokePattern = strokePatternA;
}

void SplashState::setFillPattern(SplashPattern *fillPatternA) {
  delete fillPattern;
  fillPattern = fillPatternA;
}

void SplashState::setScreen(SplashScreen *screenA) {
  delete screen;
  screen = screenA;
}

void SplashState::setLineDash(SplashCoord *lineDashA, int lineDashLengthA,
			      SplashCoord lineDashPhaseA) {
  gfree(lineDash);
  lineDashLength = lineDashLengthA;
  if (lineDashLength > 0) {
    lineDash = (SplashCoord *)gmallocn(lineDashLength, sizeof(SplashCoord));
    memcpy(lineDash, lineDashA, lineDashLength * sizeof(SplashCoord));
  } else {
    lineDash = NULL;
  }
  lineDashPhase = lineDashPhaseA;
}

void SplashState::clipResetToRect(SplashCoord x0, SplashCoord y0,
				  SplashCoord x1, SplashCoord y1) {
  if (clipIsShared) {
    clip = clip->copy();
    clipIsShared = gFalse;
  }
  clip->resetToRect(x0, y0, x1, y1);
}

SplashError SplashState::clipToRect(SplashCoord x0, SplashCoord y0,
				    SplashCoord x1, SplashCoord y1) {
  if (clipIsShared) {
    clip = clip->copy();
    clipIsShared = gFalse;
  }
  return clip->clipToRect(x0, y0, x1, y1);
}

SplashError SplashState::clipToPath(SplashPath *path, GBool eo) {
  if (clipIsShared) {
    clip = clip->copy();
    clipIsShared = gFalse;
  }
  return clip->clipToPath(path, matrix, flatness, eo);
}

void SplashState::setSoftMask(SplashBitmap *softMaskA) {
  if (deleteSoftMask) {
    delete softMask;
  }
  softMask = softMaskA;
  deleteSoftMask = gTrue;
}

void SplashState::setTransfer(Guchar *red, Guchar *green, Guchar *blue,
			      Guchar *gray) {
  int i;

  memcpy(rgbTransferR, red, 256);
  memcpy(rgbTransferG, green, 256);
  memcpy(rgbTransferB, blue, 256);
  memcpy(grayTransfer, gray, 256);
  for (i = 0; i < 256; ++i) {
    cmykTransferC[i] = 255 - rgbTransferR[255 - i];
    cmykTransferM[i] = 255 - rgbTransferG[255 - i];
    cmykTransferY[i] = 255 - rgbTransferB[255 - i];
    cmykTransferK[i] = 255 - grayTransfer[255 - i];
  }
}
