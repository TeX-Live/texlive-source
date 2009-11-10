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

/***************************************************************************
*
*   Copyright (C) 1998-2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
************************************************************************/


#ifndef __CMAPS_H
#define __CMAPS_H

#include "layout/LETypes.h"
#include "sfnt.h"

class CMAPMapper
{
public:
    virtual LEGlyphID unicodeToGlyph(LEUnicode32 unicode32) const = 0;

    virtual ~CMAPMapper()
		{
			LE_DELETE_ARRAY(fcmap);
		}

    static CMAPMapper *createUnicodeMapper(const CMAPTable *cmap);

protected:
    CMAPMapper(const CMAPTable *cmap)
		: fcmap(cmap)
		{
			// nothing else to do
		}

    CMAPMapper() {};

private:
    const CMAPTable *fcmap;
};

class CMAPFormat4Mapper : public CMAPMapper
{
public:
    CMAPFormat4Mapper(const CMAPTable *cmap, const CMAPFormat4Encoding *header);

    virtual ~CMAPFormat4Mapper();

    virtual LEGlyphID unicodeToGlyph(LEUnicode32 unicode32) const;

protected:
    CMAPFormat4Mapper() {};

private:
    le_uint16       fEntrySelector;
    le_uint16       fRangeShift;
    const le_uint16 *fEndCodes;
    const le_uint16 *fStartCodes;
    const le_uint16 *fIdDelta;
    const le_uint16 *fIdRangeOffset;
};

class CMAPGroupMapper : public CMAPMapper
{
public:
    CMAPGroupMapper(const CMAPTable *cmap, const CMAPGroup *groups, le_uint32 nGroups);

    virtual ~CMAPGroupMapper();

    virtual LEGlyphID unicodeToGlyph(LEUnicode32 unicode32) const;

protected:
    CMAPGroupMapper() {};

private:
    le_int32 fPower;
    le_int32 fRangeOffset;
    const CMAPGroup *fGroups;
};

#endif

