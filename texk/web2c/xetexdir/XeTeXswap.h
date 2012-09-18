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

#ifndef __XeTeXswap_H
#define __XeTeXswap_H

#include "unicode/platform.h"	// ICU's platform.h defines U_IS_BIG_ENDIAN for us

static inline UInt16
SWAP16(const UInt16 p)
{
#if U_IS_BIG_ENDIAN
	return p;
#else
	return (p >> 8) + (p << 8);
#endif
}

static inline UInt32
SWAP32(const UInt32 p)
{
#if U_IS_BIG_ENDIAN
	return p;
#else
	return (p >> 24) + ((p >> 8) & 0x0000ff00) + ((p << 8) & 0x00ff0000) + (p << 24);
#endif
}

#ifdef __cplusplus
static inline UInt16
SWAP(UInt16 p)
{
	return SWAP16(p);
}

static inline UInt32
SWAP(UInt32 p)
{
	return SWAP32(p);
}

static inline SInt16
SWAP(SInt16 p)
{
	return (SInt16)SWAP16((UInt16)p);
}

static inline SInt32
SWAP(SInt32 p)
{
	return (SInt32)SWAP32((UInt32)p);
}
#endif

#endif
