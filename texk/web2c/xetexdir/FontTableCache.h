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

#ifndef __FONTTABLECACHE_H

#define __FONTTABLECACHE_H

#include "layout/LETypes.h"

struct FontTableCacheEntry;

class FontTableCache
{
public:
    FontTableCache();

    virtual ~FontTableCache();

    const void *find(LETag tableTag, le_uint32 *tableSize = NULL) const;

	void flush();

protected:
    virtual const void *readFontTable(LETag tableTag) const = 0;
    virtual const void *readFontTable(LETag tableTag, le_uint32 &length) const = 0;

private:

	void initialize();
	void dispose();

    void add(LETag tableTag, const void *table, le_uint32 length);

    FontTableCacheEntry *fTableCache;
    le_int32 fTableCacheCurr;
    le_int32 fTableCacheSize;
};

#endif

