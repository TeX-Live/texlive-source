#ifndef __TTF_GCACHE_H
#define __TTF_GCACHE_H

#include "tables.h"

/* $Id: gcache.h,v 1.2 1998/07/06 06:07:01 werner Exp $ */

typedef struct _gcache
{
  ULONG offset;                 /* offset of the glyph; this is the key
                                 * for sorting/searching with the glyph
                                 * cache */
  struct _gcache *prev, *next;  /* pointer to previous and next element on
                                 * the LRU list */
  struct _gcache *left, *right; /* pointer to left and right subtree on
                                 * the binary search tree */
  GLYF glyf;                    /* the actual place to hold the glyph data */
}
GlyphCache, *GlyphCachePtr;

#endif /* __TTF_GCACHE_H */


/* end of gcache.h */
