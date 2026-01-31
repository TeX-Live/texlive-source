//========================================================================
//
// LocalParams.h
//
// Copyright 2024 Glyph & Cog, LLC
//
//========================================================================

#ifndef LOCALPARAMS_H
#define LOCALPARAMS_H

#include <aconf.h>

#include "GfxState.h"

//------------------------------------------------------------------------

class LocalParams {
public:

  LocalParams();
  ~LocalParams();

  GfxRenderingIntent getDefaultRenderingIntent()
    { return defaultRenderingIntent; }
  void setDefaultRenderingIntent(GfxRenderingIntent ri)
    { defaultRenderingIntent = ri; }

  GBool getForceRenderingIntent()
    { return forceRenderingIntent; }
  void setForceRenderingIntent(GBool force)
    { forceRenderingIntent = force; }

private:

  GfxRenderingIntent defaultRenderingIntent;
  GBool forceRenderingIntent;
};

#endif
