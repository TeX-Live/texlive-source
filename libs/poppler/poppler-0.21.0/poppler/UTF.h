//========================================================================
//
// UTF.h
//
// Copyright 2001-2003 Glyph & Cog, LLC
//
//========================================================================

//========================================================================
//
// Modified under the Poppler project - http://poppler.freedesktop.org
//
// All changes made under the Poppler project to this file are licensed
// under GPL version 2 or later
//
// Copyright (C) 2008 Koji Otani <sho@bbr.jp>
// Copyright (C) 2012 Adrian Johnson <ajohnson@redneon.com>
//
// To see a description of the changes please see the Changelog file that
// came with your tarball or type make ChangeLog if you are building from git
//
//========================================================================

#ifndef UTF_H
#define UTF_H

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include "goo/GooString.h"
#include "CharTypes.h"

// Convert a UTF-16 string to a UCS-4
//   utf16      - utf16 bytes
//   utf16_len  - number of UTF-16 characters
//   ucs4_out   - if not NULL, allocates and returns UCS-4 string. Free with gfree.
//   returns number of UCS-4 characters
int UTF16toUCS4(const Unicode *utf16, int utf16_len, Unicode **ucs4_out);

// Convert a PDF Text String to UCS-4
//   s          - PDF text string
//   ucs4       - if the number of UCS-4 characters is > 0, allocates and
//                returns UCS-4 string. Free with gfree.
//   returns number of UCS-4 characters
int TextStringToUCS4(GooString *textStr, Unicode **ucs4);


static int mapUTF8(Unicode u, char *buf, int bufSize) {
  if        (u <= 0x0000007f) {
    if (bufSize < 1) {
      return 0;
    }
    buf[0] = (char)u;
    return 1;
  } else if (u <= 0x000007ff) {
    if (bufSize < 2) {
      return 0;
    }
    buf[0] = (char)(0xc0 + (u >> 6));
    buf[1] = (char)(0x80 + (u & 0x3f));
    return 2;
  } else if (u <= 0x0000ffff) {
    if (bufSize < 3) {
      return 0;
    }
    buf[0] = (char)(0xe0 + (u >> 12));
    buf[1] = (char)(0x80 + ((u >> 6) & 0x3f));
    buf[2] = (char)(0x80 + (u & 0x3f));
    return 3;
  } else if (u <= 0x0010ffff) {
    if (bufSize < 4) {
      return 0;
    }
    buf[0] = (char)(0xf0 + (u >> 18));
    buf[1] = (char)(0x80 + ((u >> 12) & 0x3f));
    buf[2] = (char)(0x80 + ((u >> 6) & 0x3f));
    buf[3] = (char)(0x80 + (u & 0x3f));
    return 4;
  } else {
    return 0;
  }
}

static int mapUCS2(Unicode u, char *buf, int bufSize) {
  if (u <= 0xffff) {
    if (bufSize < 2) {
      return 0;
     }
    buf[0] = (char)((u >> 8) & 0xff);
    buf[1] = (char)(u & 0xff);
    return 2;
  } else if (u < 0x110000) {
    Unicode uu;

    /* using surrogate pair */
    if (bufSize < 4) {
      return 0;
    }
    uu = ((u - 0x10000) >> 10) + 0xd800;
    buf[0] = (char)((uu >> 8) & 0xff);
    buf[1] = (char)(uu & 0xff);
    uu = (u & 0x3ff)+0xdc00;
    buf[2] = (char)((uu >> 8) & 0xff);
    buf[3] = (char)(uu & 0xff);
    return 4;
  } else {
    return 0;
  }
}

#endif
