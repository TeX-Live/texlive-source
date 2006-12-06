/****************************************************************************\
 Part of the XeTeX typesetting system
 copyright (c) 1994-2006 by SIL International
 written by Jonathan Kew

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
NONINFRINGEMENT. IN NO EVENT SHALL SIL INTERNATIONAL BE LIABLE FOR  
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF  
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION  
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of SIL International  
shall not be used in advertising or otherwise to promote the sale,  
use or other dealings in this Software without prior written  
authorization from SIL International.
\****************************************************************************/

#ifndef __XETEX_OT_MATH__
#define __XETEX_OT_MATH__

#include "MathTable.h"

#include "LEFontInstance.h"

/* public "C" APIs for calling from Web(-to-C) code */
extern "C" {
	int getnativemathsyparam(int f, int n);
	int getnativemathexparam(int f, int n);
	int getotmathconstant(int f, int n);
	int getotmathvariant(int f, int g, int v, int* adv);
	void* getotassemblyptr(int f, int g);
	int getotmathitalcorr(int f, int g);
	int otpartcount(const GlyphAssembly* a);
	int otpartglyph(const GlyphAssembly* a, int i);
	int otpartisextender(const GlyphAssembly* a, int i);
	int otpartstartconnector(int f, const GlyphAssembly* a, int i);
	int otpartendconnector(int f, const GlyphAssembly* a, int i);
	int otpartfulladvance(int f, const GlyphAssembly* a, int i);
	int otminconnectoroverlap(int f);
};


/* internal functions */

/* get a math font constant, scaled according to the font size */
int getMathConstant(LEFontInstance*	fontInst, mathConstantIndex whichConstant);


#endif
