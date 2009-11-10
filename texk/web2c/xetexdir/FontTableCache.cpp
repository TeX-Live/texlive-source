/****************************************************************************\
 Part of the XeTeX typesetting system
 copyright (c) 1994-2008 by SIL International
 copyright (c) 2009 by Jonathan Kew

 Written by Jonathan Kew

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

#include "layout/LETypes.h"

#include "FontTableCache.h"

#define TABLE_CACHE_INIT 5
#define TABLE_CACHE_GROW 5

struct FontTableCacheEntry
{
    LETag tag;
    const void *table;
    le_uint32 size;
};

FontTableCache::FontTableCache()
    : fTableCacheCurr(0), fTableCacheSize(TABLE_CACHE_INIT)
{
	initialize();
}

void
FontTableCache::initialize()
{
    fTableCache = LE_NEW_ARRAY(FontTableCacheEntry, fTableCacheSize);

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
        LE_DELETE_ARRAY(fTableCache[i].table);
    }

    fTableCacheCurr = 0;
}

const void *FontTableCache::find(LETag tableTag, le_uint32 *tableSize) const
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

    le_uint32  length;
    const void *table = readFontTable(tableTag, length);

    ((FontTableCache *) this)->add(tableTag, table, length);
    if (tableSize != NULL)
        *tableSize = length;

    return table;
}

void FontTableCache::add(LETag tableTag, const void *table, le_uint32 length)
{
    if (fTableCacheCurr >= fTableCacheSize) {
        le_int32 newSize = fTableCacheSize + TABLE_CACHE_GROW;

        fTableCache = (FontTableCacheEntry *) LE_GROW_ARRAY(fTableCache, newSize);

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
