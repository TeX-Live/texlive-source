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

#ifndef __XETEX_OT_MATH__
#define __XETEX_OT_MATH__

#include "XeTeX_ext.h"

#ifdef XETEX_OT_MATH_IMPLEMENTATION
#include "MathTable.h"
#include "layout/LEFontInstance.h"
#else
#define GlyphAssembly void /* used when we're just declaring the functions for xetex.ch */
#endif

/* public "C" APIs for calling from Web(-to-C) code */
#ifdef __cplusplus
extern "C" {
#endif
	int getnativemathsyparam(int f, int n);
	int getnativemathexparam(int f, int n);
	int getotmathconstant(int f, int n);
	int getotmathvariant(int f, int g, int v, integer* adv, int horiz);
	void* getotassemblyptr(int f, int g, int horiz);
	int getotmathitalcorr(int f, int g);
	int getotmathaccentpos(int f, int g);
	int otpartcount(const GlyphAssembly* a);
	int otpartglyph(const GlyphAssembly* a, int i);
	int otpartisextender(const GlyphAssembly* a, int i);
	int otpartstartconnector(int f, const GlyphAssembly* a, int i);
	int otpartendconnector(int f, const GlyphAssembly* a, int i);
	int otpartfulladvance(int f, const GlyphAssembly* a, int i);
	int otminconnectoroverlap(int f);
#ifdef __cplusplus
};
#endif

#endif
