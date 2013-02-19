/****************************************************************************\
 Part of the XeTeX typesetting system
 Copyright (c) 1994-2008 by SIL International
 Copyright (c) 2009 by Jonathan Kew

 SIL Author(s): Jonathan Kew

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the copyright holders
shall not be used in advertising or otherwise to promote the sale,
use or other dealings in this Software without prior written
authorization from the copyright holders.
\****************************************************************************/

/* from ICU sample code, extended by JK for XeTeX */

/*
 **********************************************************************
 *   Copyright (C) 2003, International Business Machines
 *   Corporation and others.  All Rights Reserved.
 **********************************************************************
 */

#include "FontTableCache.h"

#define TABLE_CACHE_INIT 5
#define TABLE_CACHE_GROW 5

struct FontTableCacheEntry
{
    OTTag tag;
    const void *table;
    uint32_t size;
};

FontTableCache::FontTableCache()
    : fTableCacheCurr(0), fTableCacheSize(TABLE_CACHE_INIT)
{
	initialize();
}

void
FontTableCache::initialize()
{
    fTableCache = (FontTableCacheEntry *) xmalloc((fTableCacheSize) * sizeof(FontTableCacheEntry));

    if (fTableCache == NULL) {
        fTableCacheSize = 0;
        return;
    }
}

FontTableCache::~FontTableCache()
{
	dispose();
}

void FontTableCache::dispose()
{
    for (int i = fTableCacheCurr - 1; i >= 0; i -= 1) {
        free((void *) (fTableCache[i].table));
    }

	free(fTableCache);

    fTableCacheCurr = 0;
}

const void *FontTableCache::find(OTTag tableTag, uint32_t *tableSize) const
{
	int lo = 0, hi = fTableCacheCurr;
	while (lo < hi) {
		int i = (lo + hi) >> 1;
		const FontTableCacheEntry *e = fTableCache + i;
		if (e->tag < tableTag)
			lo = i + 1;
		else if (e->tag > tableTag)
			hi = i;
		else {
            if (tableSize != NULL)
                *tableSize = e->size;
            return e->table;
		}
	}

    uint32_t  length;
    const void *table = readFontTable(tableTag, length);

    ((FontTableCache *) this)->add(tableTag, table, length);
    if (tableSize != NULL)
        *tableSize = length;

    return table;
}

void FontTableCache::add(OTTag tableTag, const void *table, uint32_t length)
{
    if (fTableCacheCurr >= fTableCacheSize) {
        int32_t newSize = fTableCacheSize + TABLE_CACHE_GROW;

        fTableCache = (FontTableCacheEntry *) realloc(fTableCache, newSize * sizeof(FontTableCacheEntry));
        fTableCacheSize = newSize;
    }

	int i;
	for (i = fTableCacheCurr; i > 0; --i) {
		if (fTableCache[i-1].tag < tableTag)
			break;
		fTableCache[i] = fTableCache[i-1];
	}
    fTableCache[i].tag   = tableTag;
    fTableCache[i].table = table;
    fTableCache[i].size  = length;

    fTableCacheCurr += 1;
}

void FontTableCache::flush()
{
	if (fTableCacheSize > 0) {
		dispose();
		initialize();
	}
}
