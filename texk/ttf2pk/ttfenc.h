/*
 *   ttfenc.h
 *
 *   This file is part of the ttf2pk package.
 *
 *   Copyright 1997-1999, 2000 by
 *     Frederic Loyer <loyer@ensta.fr>
 *     Werner Lemberg <wl@gnu.org>
 */

#ifndef TTFENC_H
#define TTFENC_H

#include "ttf2tfm.h"


enum _EncodingScheme
{
  encUnicode,
  encMac,
  encFontSpecific
};
typedef enum _EncodingScheme EncodingScheme;


void set_encoding_scheme(EncodingScheme e, Font *fnt);

char *code_to_adobename(long code);
long adobename_to_code(char *s);

ttfinfo *findglyph(unsigned short g, ttfinfo *p);
ttfinfo *findadobe(char *p, ttfinfo *ap);
ttfinfo *findmappedadobe(char *p, ttfinfo **array);

void replace_glyphs(Font *fnt);
void restore_glyph(encoding *enc, Font *fnt);

#endif /* TTFENC_H */


/* end */
