//========================================================================
//
// LocalParams.cc
//
// Copyright 2024 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#include "LocalParams.h"

//------------------------------------------------------------------------

LocalParams::LocalParams() {
  defaultRenderingIntent = gfxRenderingIntentRelativeColorimetric;
  forceRenderingIntent = gFalse;
}

LocalParams::~LocalParams() {
}
