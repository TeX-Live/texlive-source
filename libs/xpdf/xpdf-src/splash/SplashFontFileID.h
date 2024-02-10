//========================================================================
//
// SplashFontFileID.h
//
// Copyright 2003-2013 Glyph & Cog, LLC
//
//========================================================================

#ifndef SPLASHFONTFILEID_H
#define SPLASHFONTFILEID_H

#include <aconf.h>

#include "gtypes.h"

//------------------------------------------------------------------------
// SplashFontFileID
//------------------------------------------------------------------------

class SplashFontFileID {
public:

  SplashFontFileID();
  virtual ~SplashFontFileID();
  virtual GBool matches(SplashFontFileID *id) = 0;
};

#endif
