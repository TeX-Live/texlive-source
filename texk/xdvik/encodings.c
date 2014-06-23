/* The unicode2adobe structure and the adobe2unicode_table mapping is taken
   from the catdvi (v0.12) `adobetbl.h' source file; it has the following copyright:

   catdvi - get text from DVI files
   Copyright (C) 1999 Antti-Juhani Kaijanaho <gaia@iki.fi>
   Copyright (C) 2001 Bjoern Brill <brill@fs.math.uni-frankfurt.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* Other parts are adapted from the GNU `iconv' library,
   (search for `iconv'), which has the following copyright:
 
   Copyright (C) 1999-2001 Free Software Foundation, Inc.
 
   The GNU LIBICONV Library is free software; you can redistribute it
   and/or modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.
   
   The GNU LIBICONV Library is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the GNU LIBICONV Library; see the file COPYING.LIB.
   If not, write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA.
*/


/* The rest of the file is Copyright (c) 2003-2013 the xdvik development team

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

#include "xdvi-config.h"
#include "xdvi.h"

#if HAVE_ICONV_H
# include <iconv.h>
static iconv_t m_iconv_gb_ucs4 = (iconv_t)(-1);
#endif /* HAVE_ICONV_H */

#include <ctype.h>

#include "util.h"
#include "encodings.h"
#include "my-snprintf.h"
#include "message-window.h"
#include "exit-handlers.h"

#define MY_DEBUG 0

#define HAS_PREFIX(src,dst) (memcmp(src, dst, strlen(dst)) == 0)

#if MY_DEBUG
# define TRACE_FIND_VERBOSE(x) TRACE_FIND(x)
#else
# define TRACE_FIND_VERBOSE(x) /* as nothing */
#endif

/*
  The following encoding vectors are copied from catdvi's enc/xyz.h files;
  only the `.notdef' values have been replaced by 0 instead of 0x003f
  (question mark) to make the mapping more flexible.
*/

/* from enc/texmsym.h */
static uint32_t m_cm_symbol_encoding[256] = {
    0x2212, 0x00b7, 0x00d7, 0x2217, 0x00f7, 0x22c4, 0x00b1, 0x2213,
    0x2295, 0x2296, 0x2297, 0x2298, 0x2299, 0x25ef, 0x25e6, 0x2022,
    /* 0x10 */
    0x224d, 0x2261, 0x2286, 0x2287, 0x2264, 0x2265, 0x227c, 0x227d,
    0x223c, 0x2248, 0x2282, 0x2283, 0x226a, 0x226b, 0x227a, 0x227b,
    /* 0x20 */
    0x2190, 0x2192, 0x2191, 0x2193, 0x2194, 0x2197, 0x2198, 0x2243,
    0x21d0, 0x21d2, 0x21d1, 0x21d3, 0x21d4, 0x2196, 0x2199, 0x221d,
    /* 0x30 */
    0x2032, 0x221e, 0x2208, 0x220b, 0x25b3, 0x25bd, 0x10fffc, 0,
    0x2200, 0x2203, 0x00ac, 0x2205, 0x211c, 0x2111, 0x22a4, 0x22a5,
    /* 0x40 */
    0x2135, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
    /* 0x50 */
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005a, 0x222a, 0x2229, 0x228e, 0x2227, 0x2228,
    /* 0x60 */
    0x22a2, 0x22a3, 0x230a, 0x230b, 0x2308, 0x2309, 0x007b, 0x007d,
    0x2329, 0x232a, 0x007c, 0x2225, 0x2195, 0x21d5, 0x005c, 0x2240,
    /* 0x70 */
    0x221a, 0x2210, 0x2207, 0x222b, 0x2294, 0x2293, 0x2291, 0x2292,
    0x00a7, 0x2020, 0x2021, 0x00b6, 0x2663, 0x2666, 0x2665, 0x2660,
    /* 0x80 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x90 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xa0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0b0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xc0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xd0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xe0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xf0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

/* cbgreek, greek encoding */
static uint32_t m_cb_greek_encoding[256] = {
    0x2013, 0x20032f, 0x10144, 0x10145, 0x10146, 0x10147, 0x03db, 0x03db,
    0x1fbe, 0x1fbc, 0x1fcc, 0x1ffc, 0x0391, 0x03ab, 0x03b1, 0x03cb,
    /* 0x10 */
    0x02cf, 0x02ce, 0x03df, 0x03d9, 0x20032e, 0x03d8, 0x03da, 0x03e0,
    0x20ac, 0x2030, 0x0259, 0x03e1, 0x2018, 0x2019, 0x02d8, 0x00af,
    /* 0x20 */
    0x1fc1, 0x0021, 0x00a8, 0x0385, 0x1fed, 0x0025, 0x00b7, 0x0384,
    0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
    /* 0x30 */
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003a, 0x0387, 0x1ffe, 0x003d, 0x1fbf, 0x003b,
    /* 0x40 */
    0x1fdf, 0x0391, 0x0392, 0x1fdd, 0x0394, 0x0395, 0x03a6, 0x0393,
    0x0397, 0x0399, 0x0398, 0x039a, 0x039b, 0x039c, 0x039d, 0x039f,
    /* 0x50 */
    0x03a0, 0x03a7, 0x03a1, 0x03a3, 0x03a4, 0x03d2, 0x1fde, 0x03a9,
    0x039e, 0x03a8, 0x0396, 0x005b, 0x1fcf, 0x005d, 0x1fce, 0x1fcd,
    /* 0x60 */
    0x1fef, 0x03b1, 0x03b2, 0x03c2, 0x03b4, 0x03b5, 0x03c6, 0x03b3,
    0x03b7, 0x03b9, 0x03b8, 0x03ba, 0x03bb, 0x03bc, 0x03bd, 0x03bf,
    /* 0x70 */
    0x03c0, 0x03c7, 0x03c1, 0x03c3, 0x03c4, 0x03c5, 0, 0x03c9, 
    0x03be, 0x03c8, 0x03b6, 0x00ab, 0x037a, 0x00bb, 0x1fc0, 0x2014,
    /* 0x80 */
    0x1f70, 0x1f01, 0x1f00, 0x1f03, 0x1fb2, 0x1f81, 0x1f80, 0x1f83,
    0x03ac, 0x1f05, 0x1f04, 0x1f02, 0x1fb4, 0x1f85, 0x1f84, 0x1f82,
    /* 0x90 */
    0x1fb6, 0x1f07, 0x1f06, 0x03dd, 0x1fb7, 0x1f87, 0x1f86, 0,
    0x1f74, 0x1f21, 0x1f20, 0, 0x1fc2, 0x1f91, 0x1f90, 0,
    /* 0xa0 */
    0x03ae, 0x1f25, 0x1f24, 0x1f23, 0x1fc4, 0x1f95, 0x1f94, 0x1f93,
    0x1fc6, 0x1f27, 0x1f26, 0x1f22, 0x1fc7, 0x1f97, 0x1f96, 0x1f92,
    /* 0b0 */
    0x1f7c, 0x1f61, 0x1f60, 0x1f63, 0x1ff2, 0x1fa1, 0x1fa0, 0x1fa3,
    0x03ce, 0x1f65, 0x1f64, 0x1f62, 0x1ff4, 0x1fa5, 0x1fa4, 0x1fa2,
    /* 0xc0 */
    0x1ff6, 0x1f67, 0x1f66, 0x03dc, 0x1ff7, 0x1fa7, 0x1fa6, 0,
    0x1f76, 0x1f31, 0x1f30, 0x1f33, 0x1f7a, 0x1f51, 0x1f50, 0x1f53,
    /* 0xd0 */
    0x03af, 0x1f35, 0x1f34, 0x1f32, 0x03cd, 0x1f55, 0x1f54, 0x1f52,
    0x1fd6, 0x1f37, 0x1f36, 0x03aa, 0x1fe6, 0x1f57, 0x1f56, 0x03ab,
    /* 0xe0 */
    0x1f72, 0x1f11, 0x1f10, 0x1f13, 0x1f78, 0x1f41, 0x1f40, 0x1f43,
    0x03ad, 0x1f15, 0x1f14, 0x1f12, 0x03cc, 0x1f45, 0x1f44, 0x1f42,
    /* 0xf0 */
    0x03ca, 0x1fd2, 0x0390, 0x1fd7, 0x03cb, 0x1fe2, 0x03b0, 0x1fe7,
    0x1fb3, 0x1fc3, 0x1ff3, 0x1fe5, 0x1fe4, 0, 0x0374, 0x0375
};

/* from enc/texmital.h */
static uint32_t m_cm_math_italics_encoding[256] = {
    /* 0x00 */
    0x0393, 0x0394, 0x0398, 0x039b, 0x039e, 0x03a0, 0x03a3, 0x03a5,
    0x03a6, 0x03a8, 0x03a9, 0x03b1, 0x03b2, 0x03b3, 0x03b4, 0x03f5,
    /* 0x10 */
    0x03b6, 0x03b7, 0x03b8, 0x03b9, 0x03ba, 0x03bb, 0x03bc, 0x03bd,
    0x03be, 0x03c0, 0x03c1, 0x03c3, 0x03c4, 0x03c5, 0x03d5, 0x03c7,
    /* 0x20 */
    0x03c8, 0x03c9, 0x03b5, 0x03d1, 0x03d6, 0x03f1, 0x03c2, 0x03c6,
    0x21bc, 0x21bd, 0x21c0, 0x21c1, 0, 0, 0x22b3, 0x22b2,
    /* 0x30 */
    0xf730, 0xf731, 0xf732, 0xf733, 0xf734, 0xf735, 0xf736, 0xf737,
    0xf738, 0xf739, 0x002e, 0x002c, 0x003c, 0x002f, 0x003e, 0x22c6,
    /* 0x40 */
    0x2202, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
    /* 0x50 */
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005a, 0x266d, 0x266e, 0x266f, 0x2323, 0x2322,
    /* 0x60 */
    0x2113, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
    /* 0x70 */
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007a, 0x0131, 0xf6be, 0x2118, 0x10fffb, 0x2040,
    /* 0x80 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x90 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xa0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xb0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xc0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xd0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xe0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xf0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};


static uint32_t m_bbold_encoding[256] = {
    /* 0x00 */
    0x0393, 0x0394, 0x0398, 0x039b, 0x039e, 0x03a0, 0x03a3, 0x03a5,
    0x03a6, 0x03a8, 0x03a9, 0x03b1, 0x03b2, 0x03b3, 0x03b4, 0x03f5,
    /* 0x10 */
    0x03b6, 0x03b7, 0x03b8, 0x03b9, 0x03ba, 0x03bb, 0x03bc, 0x03bd,
    0x03be, 0x03c0, 0x03c1, 0x03c3, 0x03c4, 0x03c5, 0x03d5, 0x03c7,
    /* 0x20 */
    0x03c8, 0x0021, 0x201c, 0x0023, 0x0024, 0x0025, 0x0026, 0x2019,
    0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
    /* 0x30 */
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003a, 0x003b, 0x00a1, 0x003d, 0x00bf, 0x003f,
    /* 0x40 */
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
    /* 0x50 */
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x0028, 0x0029,
    /* 0x60 */
    0x2018, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
    /* 0x70 */
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007a, 0x002d, 0x007c, 0x2013, 0x201d, 0x03c9,
    /* 0x80 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x90 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xa0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xb0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xc0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xd0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xe0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xf0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

/* from enc/texmext.h */
static uint32_t m_cm_math_extended_encoding[256] = {
    /* 0x00 */
    0x10ff00, 0x10ff01, 0x10ff02, 0x10ff03, 0x10ff04, 0x10ff05, 0x10ff06, 0x10ff07,
    0x10ff08, 0x10ff09, 0x10ff0a, 0x10ff0b, 0x10ff0c, 0x10ff0d, 0x10ff0e, 0x10ff0f,
    /* 0x10 */
    0x10ff10, 0x10ff11, 0x10ff12, 0x10ff13, 0x10ff14, 0x10ff15, 0x10ff16, 0x10ff17,
    0x10ff18, 0x10ff19, 0x10ff1a, 0x10ff1b, 0x10ff1c, 0x10ff1d, 0x10ff1e, 0x10ff1f,
    /* 0x20 */
    0x10ff20, 0x10ff21, 0x10ff22, 0x10ff23, 0x10ff24, 0x10ff25, 0x10ff26, 0x10ff27,
    0x10ff28, 0x10ff29, 0x10ff2a, 0x10ff2b, 0x10ff2c, 0x10ff2d, 0x10ff2e, 0x10ff2f,
    /* 0x30 */
    0xf8eb, 0xf8f6, 0xf8ee, 0xf8f9, 0xf8f0, 0xf8fb, 0xf8ef, 0xf8fa,
    0xf8f1, 0xf8fc, 0xf8f3, 0xf8fe, 0xf8f2, 0xf8fd, 0xf8f4, 0xf8e6,
    /* 0x40 */
    0xf8ed, 0xf8f8, 0xf8ec, 0xf8f7, 0x10ff44, 0x10ff45, 0x10ff46, 0x10ff47,
    0x10ff48, 0x10ff49, 0x10ff4a, 0x10ff4b, 0x10ff4c, 0x10ff4d, 0x10ff4e, 0x10ff4f,
    /* 0x50 */
    0x10ff50, 0x10ff51, 0x10ff52, 0x10ff53, 0x10ff54, 0x10ff55, 0x10ff56, 0x10ff57,
    0x10ff58, 0x10ff59, 0x10ff5a, 0x10ff5b, 0x10ff5c, 0x10ff5d, 0x10ff5e, 0x10ff5f,
    /* 0x60 */
    0x10ff60, 0x10ff61, 0x10ff62, 0x10ff63, 0x10ff64, 0x10ff65, 0x10ff66, 0x10ff67,
    0x10ff68, 0x10ff69, 0x10ff6a, 0x10ff6b, 0x10ff6c, 0x10ff6d, 0x10ff6e, 0x10ff6f,
    /* 0x70 */
    0x10ff70, 0x10ff71, 0x10ff72, 0x10ff73, 0x10ff74, 0x10ff75, 0x10ff76, 0x10ff77,
    0x10ff78, 0x10ff79, 0x10ff7a, 0x10ff7b, 0x10ff7c, 0x10ff7d, 0x10ff7e, 0x10ff7f,
    /* 0x80 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x90 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xa0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xb0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xc0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xd0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xe0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xf0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

/* from enc/textt.h */
static uint32_t m_cm_typewriter_encoding[256] = {
    /* 0x00 */
    0x0393, 0x0394, 0x0398, 0x039b, 0x039e, 0x03a0, 0x03a3, 0x03a5,
    0x03a6, 0x03a8, 0x03a9, 0x2191, 0x2193, 0x0027, 0x00a1, 0x00bf,
    /* 0x10 */
    0x0131, 0xf6be, 0x0060, 0x00b4, 0x02c7, 0x02d8, 0x00af, 0x02da,
    0x00b8, 0x00df, 0x00e6, 0x0153, 0x00f8, 0x00c6, 0x0152, 0x00d8,
    /* 0x20 */
    0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x2019,
    0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
    /* 0x30 */
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f,
    /* 0x40 */
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
    /* 0x50 */
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x02c6, 0x005f,
    /* 0x60 */
    0x2018, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
    /* 0x70 */
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x02dc, 0x00a8,
    /* 0x80 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x90 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xa0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xb0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xc0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xd0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xe0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xf0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

/* from enc/ot1.h */
static uint32_t m_ot1_encoding[256] = {
    /* 0x00 */
    0x0393, 0x0394, 0x0398, 0x039b, 0x039e, 0x03a0, 0x03a3, 0x03a5,
    0x03a6, 0x03a8, 0x03a9, 0xfb00, 0xfb01, 0xfb02, 0xfb03, 0xfb04,
    /* 0x10 */
    0x0131, 0xf6be, 0x0060, 0x00b4, 0x02c7, 0x02d8, 0x00af, 0x02da,
    0x00b8, 0x00df, 0x00e6, 0x0153, 0x00f8, 0x00c6, 0x0152, 0x00d8,
    /* 0x20 */
    0x0020, 0x0021, 0x201c, 0x0023, 0x0024, 0x0025, 0x0026, 0x2019,
    0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
    /* 0x30 */
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003a, 0x003b, 0x00a1, 0x003d, 0x00bf, 0x003f,
    /* 0x40 */
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
    /* 0x50 */
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005a, 0x005b, 0x201d, 0x005d, 0x02c6, 0x02d9,
    /* 0x60 */
    0x2018, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
    /* 0x70 */
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007a, 0x2013, 0x2014, 0x0022, 0x02dc, 0x00a8,
    /* 0x80 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x90 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xa0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xb0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xc0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xd0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xe0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xf0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

/* Cyrilic e.g. fro larm1000.mf; from
   http://www.tug.org/tex-archive/macros/latex/contrib/t2/enc-maps/OT2uni.map */
static uint32_t m_t2_encoding[256] = {
    0x0060, 0x00b4, 0x02c6, 0x02dc, 0x00a8, 0x02dd, 0x02da, 0x02c7,
    0x02d8, 0x00af, 0x02d9, 0x00b8, 0x02db, 0x04c0, 0x2329, 0x232a, 
    /* 0x10 */
    0x201c, 0x201d, 0xf6d5, 0xf6d6, 0xf6d4, 0x2013, 0x2014, 0xfeff,
    0x2080, 0x0131, 0xf6be, 0xfb00, 0xfb01, 0xfb02, 0xfb03, 0xfb04,
    /* 0x20 */
    0x2420, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x2019,
    0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
    /* 0x30 */
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f,
    /* 0x40 */
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
    /* 0x50 */
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f,
    /* 0x60 */
    0x2018, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
    /* 0x70 */
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x002d,
    /* 0x80 */
    0x0490, 0x0492, 0x0402, 0x040b, 0x04ba, 0x0496, 0x0498, 0x0409,
    0x0407, 0x049a, 0x04a0, 0x049c, 0x04d4, 0x04a2, 0x04a4, 0x0405,
    /* 0x90 */
    0x04e8, 0x04aa, 0x040e, 0x04ae, 0x04b0, 0x04b2, 0x040f, 0x04b8,
    0x04b6, 0x0404, 0x04d8, 0x040a, 0x0401, 0x2116, 0x00a4, 0x00a7,
    /* 0xa0 */
    0x0491, 0x0493, 0x0452, 0x045b, 0x04bb, 0x0497, 0x0499, 0x0459,
    0x0457, 0x049b, 0x04a1, 0x049d, 0x04d5, 0x04a3, 0x04a5, 0x0455,
    /* 0xb0 */
    0x04e9, 0x04ab, 0x045e, 0x04af, 0x04b1, 0x04b3, 0x045f, 0x04b9,
    0x04b7, 0x0454, 0x04d9, 0x045a, 0x0451, 0x201e, 0x00ab, 0x00bb,
    /* 0xc0 */
    0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
    0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e, 0x041f,
    /* 0xd0 */
    0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
    0x0428, 0x0429, 0x042a, 0x042b, 0x042c, 0x042d, 0x042e, 0x042f,
    /* 0xe0 */
    0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
    0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e, 0x043f,
    /* 0xf0 */
    0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
    0x0448, 0x0449, 0x044a, 0x044b, 0x044c, 0x044d, 0x044e, 0x044f,
};

/* from enc/cork.h */
static uint32_t m_cork_encoding[256] = {
    /* 0x00 */
    0x0060, 0x00b4, 0x02c6, 0x02dc, 0x00a8, 0x02dd, 0x02da, 0x02c7,
    0x02d8, 0x00af, 0x02d9, 0x00b8, 0x02db, 0x201a, 0x2039, 0x203a,
    /* 0x10 */
    0x201c, 0x201d, 0x201e, 0x00ab, 0x00bb, 0x2013, 0x2014, 0x0020,
    0x0030, 0x0131, 0xf6be, 0xfb00, 0xfb01, 0xfb02, 0xfb03, 0xfb04,
    /* 0x20 */
    0x2420, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x2019,
    0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
    /* 0x30 */
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f,
    /* 0x40 */
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
    /* 0x50 */
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f,
    /* 0x60 */
    0x2018, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
    /* 0x70 */
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x002d,
    /* 0x80 */
    0x0102, 0x0104, 0x0106, 0x010c, 0x010e, 0x011a, 0x0118, 0x011e,
    0x0139, 0x013d, 0x0141, 0x0143, 0x0147, 0x10ffff, 0x0150, 0x0154,
    /* 0x90 */
    0x0158, 0x015a, 0x0160, 0x015e, 0x0164, 0x0162, 0x0170, 0x016e,
    0x0178, 0x0179, 0x017d, 0x017b, 0x0132, 0x0130, 0x0111, 0x00a7,
    /* 0xa0 */
    0x0103, 0x0105, 0x0107, 0x010d, 0x010f, 0x011b, 0x0119, 0x011f,
    0x013a, 0x013e, 0x0142, 0x0144, 0x0148, 0x10fffe, 0x0151, 0x0155,
    /* 0xb0 */
    0x0159, 0x015b, 0x0161, 0x015f, 0x0165, 0x0163, 0x0171, 0x016f,
    0x00FF, 0x017a, 0x017e, 0x017c, 0x0133, 0x00a1, 0x00bf, 0x00a3,
    /* 0xc0 */
    0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7,
    0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf,
    /* 0xd0 */
    0x00d0, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x0152,
    0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x00de, 0x10fffd,
    /* 0xe0 */
    0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7,
    0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef,
    /* 0xf0 */
    0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x0153,
    0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00df
};

/* Newly created ts1-lm.h, which we use for TC (textcomp) fonts.
   This only gets the accents and stuff from iso-latin1 right; lots of
   the special characters aren't defined in the Adobe lookup list
   used by catdvi's `pse2unic'.
*/
static uint32_t m_ts1_encoding[256] = {
    /* 0x00 */
    0x0060, 0x00b4, 0x02c6, 0x02dc, 0x00a8, 0x02dd, 0x02da, 0x02c7,
    0x02d8, 0, 0x02d9, 0x00b8, 0x02db, 0, 0, 0,
    /* 0x10 */
    0, 0, 0, 0, 0, 0x2013, 0xf6de, 0,
    0x2190, 0x2192, 0, 0, 0, 0, 0, 0,
    /* 0x20 */
    0, 0, 0, 0, 0x0024, 0, 0, 0,
    0, 0, 0x2217, 0, 0x002c, 0, 0x002e, 0,
    /* 0x30 */
    0xf730, 0xf731, 0xf732, 0xf733, 0xf734, 0xf735, 0xf736, 0xf737,
    0xf738, 0xf739, 0, 0, 0x2329, 0x2212, 0x232a, 0,
    /* 0x40 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x50 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0x2191, 0x2193,
    /* 0x60 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0x266a, 0,
    /* 0x70 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0x007e, 0x003d,
    /* 0x80 */
    0, 0, 0, 0, 0x2020, 0x2021, 0, 0x2030,
    0x2022, 0, 0xf724, 0xf7a2, 0x0192, 0x20a1, 0, 0,
    /* 0x90 */
    0, 0, 0x20a4, 0, 0, 0, 0x20ab, 0x2122,
    0, 0, 0, 0, 0, 0x212e, 0x25e6, 0,
    /* 0xa0 */
    0, 0, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7,
    0, 0x00a9, 0x00aa, 0, 0x00ac, 0, 0x00ae, 0,
    /* 0xb0 */
    0x00b0, 0x00b1, 0x00b2, 0x00b3, 0, 0, 0x00b6, 0x00b7,
    0, 0x00b9, 0x00ba, 0x221a, 0x00bc, 0x00bd, 0x00be, 0x20ac,
    /* 0xc0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xd0 */
    0, 0, 0, 0, 0, 0, 0x00d7, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xe0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xf0 */
    0, 0, 0, 0, 0, 0, 0x00f7, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

/* blackletter fonts with funny encodings
 */
static uint32_t m_yfrak_encoding[176] = {
    /* 0x00 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x10 */
    0x0131, 0xF6BE, 0x0060, 0x00b4, 0x02c7, 0x02d8, 0x00af, 0x02da,
    0x00b8, 0, 0x00df, 0, 0, 0, 0, 0,
    /* 0x20 */
    0, 0x0021, 0x0022, 0x0023, 0, 0x0025, 0, 0x2019,
    0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
    /* 0x30 */
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003a, 0, 0x003c, 0x003d, 0, 0x003f,
    /* 0x40 */
    0, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
    /* 0x50 */
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005a, 0x005b, 0x00ab, 0x005d, 0x005e, 0x02d9,
    /* 0x60 */
    0x2018, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
    /* 0x70 */
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007a, 0x2013, 0x2014, 0x00a8, 0x02dd, 0,
    /* 0x80 */ /* ch/ck ligatures treated as special cases */
    0, 0xfb00, 0xfb05, 0xfb00, 0xfb00, 0, 0, 0,
    0, 0x00e4, 0x00e4, 0, 0, 0x0073, 0, 0,
    /* 0x90 */
    0x00eb, 0x00eb, 0, 0, 0, 0, 0, 0,
    0, 0x00f6, 0x00f6, 0, 0, 0, 0x00fc, 0x00fc,
    /* 0xa0 */
    0, 0, 0, 0, 0x00B6, 0, 0, 0 /* sz */,
    0, 0, 0, 0, 0, 0, 0, 0
};

/*
  Mapping table of adobe character names (from .enc files) to Unicode IDs.
  Taken from catdvi's adobetbl.h; the original glyph list is available from:
  http://partners.adobe.com/asn/tech/type/glyphlist.txt
  All `private space' entries have been removed (I think they are of not
  much use).
  
  The names beginning with `$' have been added by the catdvi developers.

  Changes:
  - The macros DUP1/DUP2 have been #if 0'ed because I didn't understand them.
  - `.notdef' has been replaced by 0 to remove ambiguity with the real question mark.
  
  The list is sorted for strcmp() so that we can use bsearch() on it.
*/
static struct adobe2unicode adobe2unicode_table[] = {
    { "$Delta" , 0x0394 }, /* GREEK CAPITAL LETTER DELTA; distinguish Adobe duplicates */
    { "$Ohm" , 0x2126 }, /* OHM SIGN; distinguish Adobe duplicates */
    { "$Omega" , 0x03A9 }, /* GREEK CAPITAL LETTER OMEGA; distinguish Adobe duplicates */
    { "$Scedilla" , 0x015E }, /* LATIN CAPITAL LETTER S WITH CEDILLA; distinguish Adobe duplicates */
    { "$Tcedilla", 0x0162 }, /* LATIN CAPITAL LETTER T WITH CEDILLA; distinguish Adobe duplicates */
    { "$Tcommaaccent", 0x021A }, /* LATIN CAPITAL LETTER T WITH COMMA BELOW; distinguish Adobe duplicates */
    { "$acutemodifier", 0x02CA }, /* MODIFIER LETTER ACUTE ACCENT */
    { "$arrowdblupdn", 0x21D5 }, /* UP DOWN DOUBLE ARROW */
    { "$arrownortheast", 0x2197 }, /* NORTH EAST ARROW */
    { "$arrownorthwest", 0x2196 }, /* NORTH WEST ARROW */
    { "$arrowsoutheast", 0x2198 }, /* SOUTH EAST ARROW */
    { "$arrowsouthwest", 0x2199 }, /* SOUTH WEST ARROW */
    { "$brevecomb", 0x0306 }, /* COMBINING BREVE */
    { "$bulletmath" , 0x2219 }, /* BULLET OPERATOR; distinguish Adobe duplicates */
    { "$caroncomb", 0x030C }, /* COMBINING CARON */
    { "$cedillacomb", 0x0327 }, /* COMBINING CEDILLA */
    { "$ceilingleft", 0x2308 }, /* LEFT CEILING */
    { "$ceilingright", 0x2309 }, /* RIGHT CEILING */
    { "$circlecomb", 0x20DD }, /* COMBINING ENCLOSING CIRCLE */
    { "$circledivide", 0x2298 }, /* CIRCLED DIVISION SLASH */
    { "$circledot", 0x2299 }, /* CIRCLED DOT OPERATOR */
    { "$circlelarge", 0x25EF }, /* LARGE CIRCLE */
    { "$circleminus", 0x2296 }, /* CIRCLED MINUS */
    { "$circumflexcomb", 0x0302 }, /* COMBINING CIRCUMFLEX ACCENT */
    { "$contintegral", 0x222E }, /* CONTOUR INTEGRAL */
    { "$coproduct", 0x2210 }, /* N-ARY COPRODUCT */
    { "$diamondmath", 0x22C4 }, /* DIAMOND OPERATOR */
    { "$dieresiscomb", 0x0308 }, /* COMBINING DIAERESIS */
    { "$divisionslash", 0x2215 }, /* DIVISION SLASH; distinguish Adobe duplicates */
    { "$dotaccentcomb", 0x0307 }, /* COMBINING DOT ABOVE */
    { "$epsilon1", 0x03F5 }, /* GREEK LUNATE EPSILON SYMBOL */
    { "$equivasymptotic", 0x224D }, /* EQUIVALENT TO */
    { "$flat", 0x266D }, /* MUSIC FLAT SIGN */
    { "$floorleft", 0x230A }, /* LEFT FLOOR */
    { "$floorright", 0x230B }, /* RIGHT FLOOR */
    { "$follows", 0x227B }, /* SUCCEEDS */
    { "$followsequal", 0x227D }, /* SUCCEEDS OR EQUAL TO */
    { "$fractionslash", 0x2044 }, /* FRACTION SLASH; distinguish Adobe duplicates */
    { "$frown", 0x2322 }, /* FROWN */
    { "$gravemodifier", 0x02CB }, /* MODIFIER LETTER GRAVE ACCENT */
    { "$greatermuch", 0x226B }, /* MUCH GREATER-THAN */
    { "$harpoonleftbarbdown", 0x21BD }, /* LEFTWARDS HARPOON WITH BARB DOWNWARDS */
    { "$harpoonleftbarbup", 0x21BC }, /* LEFTWARDS HARPOON WITH BARB UPWARDS */
    { "$harpoonrightbarbdown", 0x21C1 }, /* RIGHTWARDS HARPOON WITH BARB DOWNWARDS */
    { "$harpoonrightbarbup", 0x21C0 }, /* RIGHTWARDS HARPOON WITH BARB UPWARDS */
    { "$hyphen" , 0x002D }, /* HYPHEN-MINUS; distinguish Adobe duplicates */
    { "$hyphensoft" , 0x00AD }, /* SOFT HYPHEN; distinguish Adobe duplicates */
    { "$increment" , 0x2206 }, /* INCREMENT; distinguish Adobe duplicates */
    { "$intersectionsq", 0x2293 }, /* SQUARE CAP */
    { "$latticetop", 0x22A4 }, /* DOWN TACK */
    { "$lessmuch", 0x226A }, /* MUCH-LESS THAN */
    { "$longst", 0xFB05 }, /* LATIN SMALL LIGATURE LONG S T */
    { "$lscript", 0x2113 }, /* SCRIPT SMALL L; Adobe has this as "afii61289" */
    { "$macron", 0x00AF }, /* MACRON; distinguish Adobe duplicates */
    { "$macroncomb", 0x0304 }, /* COMBINING MACRON */
    { "$macronmodifier", 0x02C9 }, /* MODIFIER LETTER MACRON */
    { "$micro", 0x00B5 }, /* MICRO SIGN; distinguish Adobe duplicates */
    { "$minusplus", 0x2213 }, /* MINUS-OR-PLUS SIGN */
    { "$mu", 0x03BC }, /* GREEK SMALL LETTER MU; distinguish Adobe duplicates */
    { "$natural", 0x266E }, /* MUSIC NATURAL SIGN */
    { "$normalin", 0x22B2 }, /* NORMAL SUBGROUP OF */
    { "$normalizes", 0x22B3 }, /* CONTAINS AS NORMAL SUBGROUP */
    { "$ogonekcomb", 0x0328 }, /* COMBINING OGONEK */
    { "$overlinecomb", 0x0305 }, /* COMBINING OVERLINE */
    { "$parallel", 0x2225 }, /* PARALLEL TO */
    { "$periodcentered" , 0x00B7 }, /* MIDDLE DOT; distinguish Adobe duplicates */
    { "$pi1", 0x03D6 }, /* GREEK PI SYMBOL; Adobe has this as "omega1" which is too confusing */
    { "$precedes", 0x227A }, /* PRECEDES */
    { "$precedesequal", 0x227C }, /* PRECEDES OR EQUAL TO */
    { "$quotedblreversed", 0x201F }, /* DOUBLE HIGH-REVERSED-9 QUOTATION MARK */
    { "$reflexnormalin", 0x22B4 }, /* NORMAL SUBGROUP OF OR EQUAL TO */
    { "$reflexnormalizes", 0x22B5 }, /* CONTAINS AS NORMAL SUBGROUP OR EQUAL TO */
    { "$reflexsubsetsq", 0x2291 }, /* SQUARE IMAGE OF OR EQUAL TO */
    { "$reflexsupersetsq", 0x2292 }, /* SQUARE ORIGINAL OF OR EQUAL TO */
    { "$rho1", 0x03F1 }, /* GREEK RHO SYMBOL */
    { "$ringcomb", 0x030A }, /* COMBINING RING ABOVE */
    { "$scedilla" , 0x015F }, /* LATIN SMALL LETTER S WITH CEDILLA; distinguish Adobe duplicates */
    { "$sharp", 0x266F }, /* MUSIC SHARP SIGN */
    { "$similarequal", 0x2243 }, /* ASYMPTOTICALLY EQUAL TO */
    { "$slashlongcomb", 0x0338 }, /* COMBINING LONG SOLIDUS OVERLAY */
    { "$smile", 0x2323 }, /* SMILE */
    { "$space" , 0x0020 }, /* SPACE; distinguish Adobe duplicates */
    { "$spacenobreak" , 0x00A0 }, /* NO-BREAK SPACE; distinguish Adobe duplicates */
    { "$spacesymbol", 0x2420 }, /* SYMBOL FOR SPACE */
    { "$st", 0xFB06 }, /* LATIN SMALL LIGATURE ST */
    { "$starmath", 0x22C6 }, /* STAR OPERATOR */
    { "$tcedilla", 0x0163 }, /* LATIN SMALL LETTER T WITH CEDILLA; distinguish Adobe duplicates */
    { "$tcommaaccent", 0x021B }, /* LATIN SMALL LETTER T WITH COMMA BELOW; distinguish Adobe duplicates */
    { "$tie", 0x2040 }, /* CHARACTER TIE */
    { "$triagwhitedn", 0x25BD }, /* WHITE DOWN-POINTING TRIANGLE */
    { "$triagwhiteup", 0x25B3 }, /* WHITE UP-POINTING TRIANGLE */
    { "$turnstileleft", 0x22A2 }, /* RIGHT TACK */
    { "$turnstileright", 0x22A3 }, /* LEFT TACK */
    { "$unionmulti", 0x228E }, /* MULTISET UNION */
    { "$unionsq", 0x2294 }, /* SQUARE CUP */
    { "$vectorcomb", 0x20D7 }, /* COMBINING RIGHT ARROW ABOVE */
    { "$wreathproduct", 0x2240 }, /* WREATH PRODUCT */
    { ".notdef", 0 }, /* was: QUESTION MARK; changed to 0 */
    { "A", 0x0041 }, /* LATIN CAPITAL LETTER A */
    { "AE", 0x00C6 }, /* LATIN CAPITAL LETTER AE */
    { "AEacute", 0x01FC }, /* LATIN CAPITAL LETTER AE WITH ACUTE */
    { "AEsmall", 0xF7E6 }, /* LATIN SMALL CAPITAL LETTER AE */
    { "Aacute", 0x00C1 }, /* LATIN CAPITAL LETTER A WITH ACUTE */
    { "Aacutesmall", 0xF7E1 }, /* LATIN SMALL CAPITAL LETTER A WITH ACUTE */
    { "Abreve", 0x0102 }, /* LATIN CAPITAL LETTER A WITH BREVE */
    { "Acircumflex", 0x00C2 }, /* LATIN CAPITAL LETTER A WITH CIRCUMFLEX */
    { "Acircumflexsmall", 0xF7E2 }, /* LATIN SMALL CAPITAL LETTER A WITH CIRCUMFLEX */
    { "Acute", 0xF6C9 }, /* CAPITAL ACUTE ACCENT */
    { "Acutesmall", 0xF7B4 }, /* SMALL CAPITAL ACUTE ACCENT */
    { "Adieresis", 0x00C4 }, /* LATIN CAPITAL LETTER A WITH DIAERESIS */
    { "Adieresissmall", 0xF7E4 }, /* LATIN SMALL CAPITAL LETTER A WITH DIAERESIS */
    { "Agrave", 0x00C0 }, /* LATIN CAPITAL LETTER A WITH GRAVE */
    { "Agravesmall", 0xF7E0 }, /* LATIN SMALL CAPITAL LETTER A WITH GRAVE */
    { "Alpha", 0x0391 }, /* GREEK CAPITAL LETTER ALPHA */
    { "Alphatonos", 0x0386 }, /* GREEK CAPITAL LETTER ALPHA WITH TONOS */
    { "Amacron", 0x0100 }, /* LATIN CAPITAL LETTER A WITH MACRON */
    { "Aogonek", 0x0104 }, /* LATIN CAPITAL LETTER A WITH OGONEK */
    { "Aring", 0x00C5 }, /* LATIN CAPITAL LETTER A WITH RING ABOVE */
    { "Aringacute", 0x01FA }, /* LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE */
    { "Aringsmall", 0xF7E5 }, /* LATIN SMALL CAPITAL LETTER A WITH RING ABOVE */
    { "Asmall", 0xF761 }, /* LATIN SMALL CAPITAL LETTER A */
    { "Atilde", 0x00C3 }, /* LATIN CAPITAL LETTER A WITH TILDE */
    { "Atildesmall", 0xF7E3 }, /* LATIN SMALL CAPITAL LETTER A WITH TILDE */
    { "B", 0x0042 }, /* LATIN CAPITAL LETTER B */
    { "Beta", 0x0392 }, /* GREEK CAPITAL LETTER BETA */
    { "Brevesmall", 0xF6F4 }, /* SMALL CAPITAL BREVE */
    { "Bsmall", 0xF762 }, /* LATIN SMALL CAPITAL LETTER B */
    { "C", 0x0043 }, /* LATIN CAPITAL LETTER C */
    { "Cacute", 0x0106 }, /* LATIN CAPITAL LETTER C WITH ACUTE */
    { "Caron", 0xF6CA }, /* CAPITAL CARON */
    { "Caronsmall", 0xF6F5 }, /* SMALL CAPITAL CARON */
    { "Ccaron", 0x010C }, /* LATIN CAPITAL LETTER C WITH CARON */
    { "Ccedilla", 0x00C7 }, /* LATIN CAPITAL LETTER C WITH CEDILLA */
    { "Ccedillasmall", 0xF7E7 }, /* LATIN SMALL CAPITAL LETTER C WITH CEDILLA */
    { "Ccircumflex", 0x0108 }, /* LATIN CAPITAL LETTER C WITH CIRCUMFLEX */
    { "Cdotaccent", 0x010A }, /* LATIN CAPITAL LETTER C WITH DOT ABOVE */
    { "Cedillasmall", 0xF7B8 }, /* SMALL CAPITAL CEDILLA */
    { "Chi", 0x03A7 }, /* GREEK CAPITAL LETTER CHI */
    { "Circumflexsmall", 0xF6F6 }, /* SMALL CAPITAL MODIFIER LETTER CIRCUMFLEX ACCENT */
    { "Csmall", 0xF763 }, /* LATIN SMALL CAPITAL LETTER C */
    { "D", 0x0044 }, /* LATIN CAPITAL LETTER D */
    { "Dcaron", 0x010E }, /* LATIN CAPITAL LETTER D WITH CARON */
    { "Dcroat", 0x0110 }, /* LATIN CAPITAL LETTER D WITH STROKE */
    { "Delta", 0x2206 }, /* INCREMENT */
#if 0
    { "Delta" DUP2, 0x0394 }, /* GREEK CAPITAL LETTER DELTA;Duplicate */
#endif /* 0 */
    { "Dieresis", 0xF6CB }, /* CAPITAL DIAERESIS */
    { "DieresisAcute", 0xF6CC }, /* CAPITAL DIAERESIS ACUTE ACCENT */
    { "DieresisGrave", 0xF6CD }, /* CAPITAL DIAERESIS GRAVE ACCENT */
    { "Dieresissmall", 0xF7A8 }, /* SMALL CAPITAL DIAERESIS */
    { "Dotaccentsmall", 0xF6F7 }, /* SMALL CAPITAL DOT ABOVE */
    { "Dsmall", 0xF764 }, /* LATIN SMALL CAPITAL LETTER D */
    { "E", 0x0045 }, /* LATIN CAPITAL LETTER E */
    { "Eacute", 0x00C9 }, /* LATIN CAPITAL LETTER E WITH ACUTE */
    { "Eacutesmall", 0xF7E9 }, /* LATIN SMALL CAPITAL LETTER E WITH ACUTE */
    { "Ebreve", 0x0114 }, /* LATIN CAPITAL LETTER E WITH BREVE */
    { "Ecaron", 0x011A }, /* LATIN CAPITAL LETTER E WITH CARON */
    { "Ecircumflex", 0x00CA }, /* LATIN CAPITAL LETTER E WITH CIRCUMFLEX */
    { "Ecircumflexsmall", 0xF7EA }, /* LATIN SMALL CAPITAL LETTER E WITH CIRCUMFLEX */
    { "Edieresis", 0x00CB }, /* LATIN CAPITAL LETTER E WITH DIAERESIS */
    { "Edieresissmall", 0xF7EB }, /* LATIN SMALL CAPITAL LETTER E WITH DIAERESIS */
    { "Edotaccent", 0x0116 }, /* LATIN CAPITAL LETTER E WITH DOT ABOVE */
    { "Egrave", 0x00C8 }, /* LATIN CAPITAL LETTER E WITH GRAVE */
    { "Egravesmall", 0xF7E8 }, /* LATIN SMALL CAPITAL LETTER E WITH GRAVE */
    { "Emacron", 0x0112 }, /* LATIN CAPITAL LETTER E WITH MACRON */
    { "Eng", 0x014A }, /* LATIN CAPITAL LETTER ENG */
    { "Eogonek", 0x0118 }, /* LATIN CAPITAL LETTER E WITH OGONEK */
    { "Epsilon", 0x0395 }, /* GREEK CAPITAL LETTER EPSILON */
    { "Epsilontonos", 0x0388 }, /* GREEK CAPITAL LETTER EPSILON WITH TONOS */
    { "Esmall", 0xF765 }, /* LATIN SMALL CAPITAL LETTER E */
    { "Eta", 0x0397 }, /* GREEK CAPITAL LETTER ETA */
    { "Etatonos", 0x0389 }, /* GREEK CAPITAL LETTER ETA WITH TONOS */
    { "Eth", 0x00D0 }, /* LATIN CAPITAL LETTER ETH */
    { "Ethsmall", 0xF7F0 }, /* LATIN SMALL CAPITAL LETTER ETH */
    { "Euro", 0x20AC }, /* EURO SIGN */
    { "F", 0x0046 }, /* LATIN CAPITAL LETTER F */
    { "Fsmall", 0xF766 }, /* LATIN SMALL CAPITAL LETTER F */
    { "G", 0x0047 }, /* LATIN CAPITAL LETTER G */
    { "Gamma", 0x0393 }, /* GREEK CAPITAL LETTER GAMMA */
    { "Gbreve", 0x011E }, /* LATIN CAPITAL LETTER G WITH BREVE */
    { "Gcaron", 0x01E6 }, /* LATIN CAPITAL LETTER G WITH CARON */
    { "Gcircumflex", 0x011C }, /* LATIN CAPITAL LETTER G WITH CIRCUMFLEX */
    { "Gcommaaccent", 0x0122 }, /* LATIN CAPITAL LETTER G WITH CEDILLA */
    { "Gdotaccent", 0x0120 }, /* LATIN CAPITAL LETTER G WITH DOT ABOVE */
    { "Grave", 0xF6CE }, /* CAPITAL GRAVE ACCENT */
    { "Gravesmall", 0xF760 }, /* SMALL CAPITAL GRAVE ACCENT */
    { "Gsmall", 0xF767 }, /* LATIN SMALL CAPITAL LETTER G */
    { "H", 0x0048 }, /* LATIN CAPITAL LETTER H */
    { "H18533", 0x25CF }, /* BLACK CIRCLE */
    { "H18543", 0x25AA }, /* BLACK SMALL SQUARE */
    { "H18551", 0x25AB }, /* WHITE SMALL SQUARE */
    { "H22073", 0x25A1 }, /* WHITE SQUARE */
    { "Hbar", 0x0126 }, /* LATIN CAPITAL LETTER H WITH STROKE */
    { "Hcircumflex", 0x0124 }, /* LATIN CAPITAL LETTER H WITH CIRCUMFLEX */
    { "Hsmall", 0xF768 }, /* LATIN SMALL CAPITAL LETTER H */
    { "Hungarumlaut", 0xF6CF }, /* CAPITAL DOUBLE ACUTE ACCENT */
    { "Hungarumlautsmall", 0xF6F8 }, /* SMALL CAPITAL DOUBLE ACUTE ACCENT */
    { "I", 0x0049 }, /* LATIN CAPITAL LETTER I */
    { "IJ", 0x0132 }, /* LATIN CAPITAL LIGATURE IJ */
    { "Iacute", 0x00CD }, /* LATIN CAPITAL LETTER I WITH ACUTE */
    { "Iacutesmall", 0xF7ED }, /* LATIN SMALL CAPITAL LETTER I WITH ACUTE */
    { "Ibreve", 0x012C }, /* LATIN CAPITAL LETTER I WITH BREVE */
    { "Icircumflex", 0x00CE }, /* LATIN CAPITAL LETTER I WITH CIRCUMFLEX */
    { "Icircumflexsmall", 0xF7EE }, /* LATIN SMALL CAPITAL LETTER I WITH CIRCUMFLEX */
    { "Idieresis", 0x00CF }, /* LATIN CAPITAL LETTER I WITH DIAERESIS */
    { "Idieresissmall", 0xF7EF }, /* LATIN SMALL CAPITAL LETTER I WITH DIAERESIS */
    { "Idotaccent", 0x0130 }, /* LATIN CAPITAL LETTER I WITH DOT ABOVE */
    { "Ifraktur", 0x2111 }, /* BLACK-LETTER CAPITAL I */
    { "Igrave", 0x00CC }, /* LATIN CAPITAL LETTER I WITH GRAVE */
    { "Igravesmall", 0xF7EC }, /* LATIN SMALL CAPITAL LETTER I WITH GRAVE */
    { "Imacron", 0x012A }, /* LATIN CAPITAL LETTER I WITH MACRON */
    { "Iogonek", 0x012E }, /* LATIN CAPITAL LETTER I WITH OGONEK */
    { "Iota", 0x0399 }, /* GREEK CAPITAL LETTER IOTA */
    { "Iotadieresis", 0x03AA }, /* GREEK CAPITAL LETTER IOTA WITH DIALYTIKA */
    { "Iotatonos", 0x038A }, /* GREEK CAPITAL LETTER IOTA WITH TONOS */
    { "Ismall", 0xF769 }, /* LATIN SMALL CAPITAL LETTER I */
    { "Itilde", 0x0128 }, /* LATIN CAPITAL LETTER I WITH TILDE */
    { "J", 0x004A }, /* LATIN CAPITAL LETTER J */
    { "Jcircumflex", 0x0134 }, /* LATIN CAPITAL LETTER J WITH CIRCUMFLEX */
    { "Jsmall", 0xF76A }, /* LATIN SMALL CAPITAL LETTER J */
    { "K", 0x004B }, /* LATIN CAPITAL LETTER K */
    { "Kappa", 0x039A }, /* GREEK CAPITAL LETTER KAPPA */
    { "Kcommaaccent", 0x0136 }, /* LATIN CAPITAL LETTER K WITH CEDILLA */
    { "Ksmall", 0xF76B }, /* LATIN SMALL CAPITAL LETTER K */
    { "L", 0x004C }, /* LATIN CAPITAL LETTER L */
    { "LL", 0xF6BF }, /* LATIN CAPITAL LETTER LL */
    { "Lacute", 0x0139 }, /* LATIN CAPITAL LETTER L WITH ACUTE */
    { "Lambda", 0x039B }, /* GREEK CAPITAL LETTER LAMDA */
    { "Lcaron", 0x013D }, /* LATIN CAPITAL LETTER L WITH CARON */
    { "Lcommaaccent", 0x013B }, /* LATIN CAPITAL LETTER L WITH CEDILLA */
    { "Ldot", 0x013F }, /* LATIN CAPITAL LETTER L WITH MIDDLE DOT */
    { "Lslash", 0x0141 }, /* LATIN CAPITAL LETTER L WITH STROKE */
    { "Lslashsmall", 0xF6F9 }, /* LATIN SMALL CAPITAL LETTER L WITH STROKE */
    { "Lsmall", 0xF76C }, /* LATIN SMALL CAPITAL LETTER L */
    { "M", 0x004D }, /* LATIN CAPITAL LETTER M */
    { "Macron", 0xF6D0 }, /* CAPITAL MACRON */
    { "Macronsmall", 0xF7AF }, /* SMALL CAPITAL MACRON */
    { "Msmall", 0xF76D }, /* LATIN SMALL CAPITAL LETTER M */
    { "Mu", 0x039C }, /* GREEK CAPITAL LETTER MU */
    { "N", 0x004E }, /* LATIN CAPITAL LETTER N */
    { "Nacute", 0x0143 }, /* LATIN CAPITAL LETTER N WITH ACUTE */
    { "Ncaron", 0x0147 }, /* LATIN CAPITAL LETTER N WITH CARON */
    { "Ncommaaccent", 0x0145 }, /* LATIN CAPITAL LETTER N WITH CEDILLA */
    { "Nsmall", 0xF76E }, /* LATIN SMALL CAPITAL LETTER N */
    { "Ntilde", 0x00D1 }, /* LATIN CAPITAL LETTER N WITH TILDE */
    { "Ntildesmall", 0xF7F1 }, /* LATIN SMALL CAPITAL LETTER N WITH TILDE */
    { "Nu", 0x039D }, /* GREEK CAPITAL LETTER NU */
    { "O", 0x004F }, /* LATIN CAPITAL LETTER O */
    { "OE", 0x0152 }, /* LATIN CAPITAL LIGATURE OE */
    { "OEsmall", 0xF6FA }, /* LATIN SMALL CAPITAL LIGATURE OE */
    { "Oacute", 0x00D3 }, /* LATIN CAPITAL LETTER O WITH ACUTE */
    { "Oacutesmall", 0xF7F3 }, /* LATIN SMALL CAPITAL LETTER O WITH ACUTE */
    { "Obreve", 0x014E }, /* LATIN CAPITAL LETTER O WITH BREVE */
    { "Ocircumflex", 0x00D4 }, /* LATIN CAPITAL LETTER O WITH CIRCUMFLEX */
    { "Ocircumflexsmall", 0xF7F4 }, /* LATIN SMALL CAPITAL LETTER O WITH CIRCUMFLEX */
    { "Odieresis", 0x00D6 }, /* LATIN CAPITAL LETTER O WITH DIAERESIS */
    { "Odieresissmall", 0xF7F6 }, /* LATIN SMALL CAPITAL LETTER O WITH DIAERESIS */
    { "Ogoneksmall", 0xF6FB }, /* SMALL CAPITAL OGONEK */
    { "Ograve", 0x00D2 }, /* LATIN CAPITAL LETTER O WITH GRAVE */
    { "Ogravesmall", 0xF7F2 }, /* LATIN SMALL CAPITAL LETTER O WITH GRAVE */
    { "Ohorn", 0x01A0 }, /* LATIN CAPITAL LETTER O WITH HORN */
    { "Ohungarumlaut", 0x0150 }, /* LATIN CAPITAL LETTER O WITH DOUBLE ACUTE */
    { "Omacron", 0x014C }, /* LATIN CAPITAL LETTER O WITH MACRON */
    { "Omega", 0x2126 }, /* OHM SIGN */
#if 0
    { "Omega" DUP2, 0x03A9 }, /* GREEK CAPITAL LETTER OMEGA;Duplicate */
#endif /* 0 */
    { "Omegatonos", 0x038F }, /* GREEK CAPITAL LETTER OMEGA WITH TONOS */
    { "Omicron", 0x039F }, /* GREEK CAPITAL LETTER OMICRON */
    { "Omicrontonos", 0x038C }, /* GREEK CAPITAL LETTER OMICRON WITH TONOS */
    { "Oslash", 0x00D8 }, /* LATIN CAPITAL LETTER O WITH STROKE */
    { "Oslashacute", 0x01FE }, /* LATIN CAPITAL LETTER O WITH STROKE AND ACUTE */
    { "Oslashsmall", 0xF7F8 }, /* LATIN SMALL CAPITAL LETTER O WITH STROKE */
    { "Osmall", 0xF76F }, /* LATIN SMALL CAPITAL LETTER O */
    { "Otilde", 0x00D5 }, /* LATIN CAPITAL LETTER O WITH TILDE */
    { "Otildesmall", 0xF7F5 }, /* LATIN SMALL CAPITAL LETTER O WITH TILDE */
    { "P", 0x0050 }, /* LATIN CAPITAL LETTER P */
    { "Phi", 0x03A6 }, /* GREEK CAPITAL LETTER PHI */
    { "Pi", 0x03A0 }, /* GREEK CAPITAL LETTER PI */
    { "Psi", 0x03A8 }, /* GREEK CAPITAL LETTER PSI */
    { "Psmall", 0xF770 }, /* LATIN SMALL CAPITAL LETTER P */
    { "Q", 0x0051 }, /* LATIN CAPITAL LETTER Q */
    { "Qsmall", 0xF771 }, /* LATIN SMALL CAPITAL LETTER Q */
    { "R", 0x0052 }, /* LATIN CAPITAL LETTER R */
    { "Racute", 0x0154 }, /* LATIN CAPITAL LETTER R WITH ACUTE */
    { "Rcaron", 0x0158 }, /* LATIN CAPITAL LETTER R WITH CARON */
    { "Rcommaaccent", 0x0156 }, /* LATIN CAPITAL LETTER R WITH CEDILLA */
    { "Rfraktur", 0x211C }, /* BLACK-LETTER CAPITAL R */
    { "Rho", 0x03A1 }, /* GREEK CAPITAL LETTER RHO */
    { "Ringsmall", 0xF6FC }, /* SMALL CAPITAL RING ABOVE */
    { "Rsmall", 0xF772 }, /* LATIN SMALL CAPITAL LETTER R */
    { "S", 0x0053 }, /* LATIN CAPITAL LETTER S */
    { "SF010000", 0x250C }, /* BOX DRAWINGS LIGHT DOWN AND RIGHT */
    { "SF020000", 0x2514 }, /* BOX DRAWINGS LIGHT UP AND RIGHT */
    { "SF030000", 0x2510 }, /* BOX DRAWINGS LIGHT DOWN AND LEFT */
    { "SF040000", 0x2518 }, /* BOX DRAWINGS LIGHT UP AND LEFT */
    { "SF050000", 0x253C }, /* BOX DRAWINGS LIGHT VERTICAL AND HORIZONTAL */
    { "SF060000", 0x252C }, /* BOX DRAWINGS LIGHT DOWN AND HORIZONTAL */
    { "SF070000", 0x2534 }, /* BOX DRAWINGS LIGHT UP AND HORIZONTAL */
    { "SF080000", 0x251C }, /* BOX DRAWINGS LIGHT VERTICAL AND RIGHT */
    { "SF090000", 0x2524 }, /* BOX DRAWINGS LIGHT VERTICAL AND LEFT */
    { "SF100000", 0x2500 }, /* BOX DRAWINGS LIGHT HORIZONTAL */
    { "SF110000", 0x2502 }, /* BOX DRAWINGS LIGHT VERTICAL */
    { "SF190000", 0x2561 }, /* BOX DRAWINGS VERTICAL SINGLE AND LEFT DOUBLE */
    { "SF200000", 0x2562 }, /* BOX DRAWINGS VERTICAL DOUBLE AND LEFT SINGLE */
    { "SF210000", 0x2556 }, /* BOX DRAWINGS DOWN DOUBLE AND LEFT SINGLE */
    { "SF220000", 0x2555 }, /* BOX DRAWINGS DOWN SINGLE AND LEFT DOUBLE */
    { "SF230000", 0x2563 }, /* BOX DRAWINGS DOUBLE VERTICAL AND LEFT */
    { "SF240000", 0x2551 }, /* BOX DRAWINGS DOUBLE VERTICAL */
    { "SF250000", 0x2557 }, /* BOX DRAWINGS DOUBLE DOWN AND LEFT */
    { "SF260000", 0x255D }, /* BOX DRAWINGS DOUBLE UP AND LEFT */
    { "SF270000", 0x255C }, /* BOX DRAWINGS UP DOUBLE AND LEFT SINGLE */
    { "SF280000", 0x255B }, /* BOX DRAWINGS UP SINGLE AND LEFT DOUBLE */
    { "SF360000", 0x255E }, /* BOX DRAWINGS VERTICAL SINGLE AND RIGHT DOUBLE */
    { "SF370000", 0x255F }, /* BOX DRAWINGS VERTICAL DOUBLE AND RIGHT SINGLE */
    { "SF380000", 0x255A }, /* BOX DRAWINGS DOUBLE UP AND RIGHT */
    { "SF390000", 0x2554 }, /* BOX DRAWINGS DOUBLE DOWN AND RIGHT */
    { "SF400000", 0x2569 }, /* BOX DRAWINGS DOUBLE UP AND HORIZONTAL */
    { "SF410000", 0x2566 }, /* BOX DRAWINGS DOUBLE DOWN AND HORIZONTAL */
    { "SF420000", 0x2560 }, /* BOX DRAWINGS DOUBLE VERTICAL AND RIGHT */
    { "SF430000", 0x2550 }, /* BOX DRAWINGS DOUBLE HORIZONTAL */
    { "SF440000", 0x256C }, /* BOX DRAWINGS DOUBLE VERTICAL AND HORIZONTAL */
    { "SF450000", 0x2567 }, /* BOX DRAWINGS UP SINGLE AND HORIZONTAL DOUBLE */
    { "SF460000", 0x2568 }, /* BOX DRAWINGS UP DOUBLE AND HORIZONTAL SINGLE */
    { "SF470000", 0x2564 }, /* BOX DRAWINGS DOWN SINGLE AND HORIZONTAL DOUBLE */
    { "SF480000", 0x2565 }, /* BOX DRAWINGS DOWN DOUBLE AND HORIZONTAL SINGLE */
    { "SF490000", 0x2559 }, /* BOX DRAWINGS UP DOUBLE AND RIGHT SINGLE */
    { "SF500000", 0x2558 }, /* BOX DRAWINGS UP SINGLE AND RIGHT DOUBLE */
    { "SF510000", 0x2552 }, /* BOX DRAWINGS DOWN SINGLE AND RIGHT DOUBLE */
    { "SF520000", 0x2553 }, /* BOX DRAWINGS DOWN DOUBLE AND RIGHT SINGLE */
    { "SF530000", 0x256B }, /* BOX DRAWINGS VERTICAL DOUBLE AND HORIZONTAL SINGLE */
    { "SF540000", 0x256A }, /* BOX DRAWINGS VERTICAL SINGLE AND HORIZONTAL DOUBLE */
    { "Sacute", 0x015A }, /* LATIN CAPITAL LETTER S WITH ACUTE */
    { "Scaron", 0x0160 }, /* LATIN CAPITAL LETTER S WITH CARON */
    { "Scaronsmall", 0xF6FD }, /* LATIN SMALL CAPITAL LETTER S WITH CARON */
    { "Scedilla", 0x015E }, /* LATIN CAPITAL LETTER S WITH CEDILLA */
#if 0
    { "Scedilla" DUP2, 0xF6C1 }, /* LATIN CAPITAL LETTER S WITH CEDILLA;Duplicate */
#endif /* 0 */
    { "Scircumflex", 0x015C }, /* LATIN CAPITAL LETTER S WITH CIRCUMFLEX */
    { "Scommaaccent", 0x0218 }, /* LATIN CAPITAL LETTER S WITH COMMA BELOW */
    { "Sigma", 0x03A3 }, /* GREEK CAPITAL LETTER SIGMA */
    { "Ssmall", 0xF773 }, /* LATIN SMALL CAPITAL LETTER S */
    { "T", 0x0054 }, /* LATIN CAPITAL LETTER T */
    { "Tau", 0x03A4 }, /* GREEK CAPITAL LETTER TAU */
    { "Tbar", 0x0166 }, /* LATIN CAPITAL LETTER T WITH STROKE */
    { "Tcaron", 0x0164 }, /* LATIN CAPITAL LETTER T WITH CARON */
    { "Tcommaaccent", 0x0162 }, /* LATIN CAPITAL LETTER T WITH CEDILLA */
#if 0
    { "Tcommaaccent" DUP2, 0x021A }, /* LATIN CAPITAL LETTER T WITH COMMA BELOW;Duplicate */
#endif /* 0 */
    { "Theta", 0x0398 }, /* GREEK CAPITAL LETTER THETA */
    { "Thorn", 0x00DE }, /* LATIN CAPITAL LETTER THORN */
    { "Thornsmall", 0xF7FE }, /* LATIN SMALL CAPITAL LETTER THORN */
    { "Tildesmall", 0xF6FE }, /* SMALL CAPITAL SMALL TILDE */
    { "Tsmall", 0xF774 }, /* LATIN SMALL CAPITAL LETTER T */
    { "U", 0x0055 }, /* LATIN CAPITAL LETTER U */
    { "Uacute", 0x00DA }, /* LATIN CAPITAL LETTER U WITH ACUTE */
    { "Uacutesmall", 0xF7FA }, /* LATIN SMALL CAPITAL LETTER U WITH ACUTE */
    { "Ubreve", 0x016C }, /* LATIN CAPITAL LETTER U WITH BREVE */
    { "Ucircumflex", 0x00DB }, /* LATIN CAPITAL LETTER U WITH CIRCUMFLEX */
    { "Ucircumflexsmall", 0xF7FB }, /* LATIN SMALL CAPITAL LETTER U WITH CIRCUMFLEX */
    { "Udieresis", 0x00DC }, /* LATIN CAPITAL LETTER U WITH DIAERESIS */
    { "Udieresissmall", 0xF7FC }, /* LATIN SMALL CAPITAL LETTER U WITH DIAERESIS */
    { "Ugrave", 0x00D9 }, /* LATIN CAPITAL LETTER U WITH GRAVE */
    { "Ugravesmall", 0xF7F9 }, /* LATIN SMALL CAPITAL LETTER U WITH GRAVE */
    { "Uhorn", 0x01AF }, /* LATIN CAPITAL LETTER U WITH HORN */
    { "Uhungarumlaut", 0x0170 }, /* LATIN CAPITAL LETTER U WITH DOUBLE ACUTE */
    { "Umacron", 0x016A }, /* LATIN CAPITAL LETTER U WITH MACRON */
    { "Uogonek", 0x0172 }, /* LATIN CAPITAL LETTER U WITH OGONEK */
    { "Upsilon", 0x03A5 }, /* GREEK CAPITAL LETTER UPSILON */
    { "Upsilon1", 0x03D2 }, /* GREEK UPSILON WITH HOOK SYMBOL */
    { "Upsilondieresis", 0x03AB }, /* GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA */
    { "Upsilontonos", 0x038E }, /* GREEK CAPITAL LETTER UPSILON WITH TONOS */
    { "Uring", 0x016E }, /* LATIN CAPITAL LETTER U WITH RING ABOVE */
    { "Usmall", 0xF775 }, /* LATIN SMALL CAPITAL LETTER U */
    { "Utilde", 0x0168 }, /* LATIN CAPITAL LETTER U WITH TILDE */
    { "V", 0x0056 }, /* LATIN CAPITAL LETTER V */
    { "Vsmall", 0xF776 }, /* LATIN SMALL CAPITAL LETTER V */
    { "W", 0x0057 }, /* LATIN CAPITAL LETTER W */
    { "Wacute", 0x1E82 }, /* LATIN CAPITAL LETTER W WITH ACUTE */
    { "Wcircumflex", 0x0174 }, /* LATIN CAPITAL LETTER W WITH CIRCUMFLEX */
    { "Wdieresis", 0x1E84 }, /* LATIN CAPITAL LETTER W WITH DIAERESIS */
    { "Wgrave", 0x1E80 }, /* LATIN CAPITAL LETTER W WITH GRAVE */
    { "Wsmall", 0xF777 }, /* LATIN SMALL CAPITAL LETTER W */
    { "X", 0x0058 }, /* LATIN CAPITAL LETTER X */
    { "Xi", 0x039E }, /* GREEK CAPITAL LETTER XI */
    { "Xsmall", 0xF778 }, /* LATIN SMALL CAPITAL LETTER X */
    { "Y", 0x0059 }, /* LATIN CAPITAL LETTER Y */
    { "Yacute", 0x00DD }, /* LATIN CAPITAL LETTER Y WITH ACUTE */
    { "Yacutesmall", 0xF7FD }, /* LATIN SMALL CAPITAL LETTER Y WITH ACUTE */
    { "Ycircumflex", 0x0176 }, /* LATIN CAPITAL LETTER Y WITH CIRCUMFLEX */
    { "Ydieresis", 0x0178 }, /* LATIN CAPITAL LETTER Y WITH DIAERESIS */
    { "Ydieresissmall", 0xF7FF }, /* LATIN SMALL CAPITAL LETTER Y WITH DIAERESIS */
    { "Ygrave", 0x1EF2 }, /* LATIN CAPITAL LETTER Y WITH GRAVE */
    { "Ysmall", 0xF779 }, /* LATIN SMALL CAPITAL LETTER Y */
    { "Z", 0x005A }, /* LATIN CAPITAL LETTER Z */
    { "Zacute", 0x0179 }, /* LATIN CAPITAL LETTER Z WITH ACUTE */
    { "Zcaron", 0x017D }, /* LATIN CAPITAL LETTER Z WITH CARON */
    { "Zcaronsmall", 0xF6FF }, /* LATIN SMALL CAPITAL LETTER Z WITH CARON */
    { "Zdotaccent", 0x017B }, /* LATIN CAPITAL LETTER Z WITH DOT ABOVE */
    { "Zeta", 0x0396 }, /* GREEK CAPITAL LETTER ZETA */
    { "Zsmall", 0xF77A }, /* LATIN SMALL CAPITAL LETTER Z */
    { "a", 0x0061 }, /* LATIN SMALL LETTER A */
    { "aacute", 0x00E1 }, /* LATIN SMALL LETTER A WITH ACUTE */
    { "abreve", 0x0103 }, /* LATIN SMALL LETTER A WITH BREVE */
    { "acircumflex", 0x00E2 }, /* LATIN SMALL LETTER A WITH CIRCUMFLEX */
    { "acute", 0x00B4 }, /* ACUTE ACCENT */
    { "acutecomb", 0x0301 }, /* COMBINING ACUTE ACCENT */
    { "adieresis", 0x00E4 }, /* LATIN SMALL LETTER A WITH DIAERESIS */
    { "ae", 0x00E6 }, /* LATIN SMALL LETTER AE */
    { "aeacute", 0x01FD }, /* LATIN SMALL LETTER AE WITH ACUTE */
    { "afii00208", 0x2015 }, /* HORIZONTAL BAR */
    { "afii10017", 0x0410 }, /* CYRILLIC CAPITAL LETTER A */
    { "afii10018", 0x0411 }, /* CYRILLIC CAPITAL LETTER BE */
    { "afii10019", 0x0412 }, /* CYRILLIC CAPITAL LETTER VE */
    { "afii10020", 0x0413 }, /* CYRILLIC CAPITAL LETTER GHE */
    { "afii10021", 0x0414 }, /* CYRILLIC CAPITAL LETTER DE */
    { "afii10022", 0x0415 }, /* CYRILLIC CAPITAL LETTER IE */
    { "afii10023", 0x0401 }, /* CYRILLIC CAPITAL LETTER IO */
    { "afii10024", 0x0416 }, /* CYRILLIC CAPITAL LETTER ZHE */
    { "afii10025", 0x0417 }, /* CYRILLIC CAPITAL LETTER ZE */
    { "afii10026", 0x0418 }, /* CYRILLIC CAPITAL LETTER I */
    { "afii10027", 0x0419 }, /* CYRILLIC CAPITAL LETTER SHORT I */
    { "afii10028", 0x041A }, /* CYRILLIC CAPITAL LETTER KA */
    { "afii10029", 0x041B }, /* CYRILLIC CAPITAL LETTER EL */
    { "afii10030", 0x041C }, /* CYRILLIC CAPITAL LETTER EM */
    { "afii10031", 0x041D }, /* CYRILLIC CAPITAL LETTER EN */
    { "afii10032", 0x041E }, /* CYRILLIC CAPITAL LETTER O */
    { "afii10033", 0x041F }, /* CYRILLIC CAPITAL LETTER PE */
    { "afii10034", 0x0420 }, /* CYRILLIC CAPITAL LETTER ER */
    { "afii10035", 0x0421 }, /* CYRILLIC CAPITAL LETTER ES */
    { "afii10036", 0x0422 }, /* CYRILLIC CAPITAL LETTER TE */
    { "afii10037", 0x0423 }, /* CYRILLIC CAPITAL LETTER U */
    { "afii10038", 0x0424 }, /* CYRILLIC CAPITAL LETTER EF */
    { "afii10039", 0x0425 }, /* CYRILLIC CAPITAL LETTER HA */
    { "afii10040", 0x0426 }, /* CYRILLIC CAPITAL LETTER TSE */
    { "afii10041", 0x0427 }, /* CYRILLIC CAPITAL LETTER CHE */
    { "afii10042", 0x0428 }, /* CYRILLIC CAPITAL LETTER SHA */
    { "afii10043", 0x0429 }, /* CYRILLIC CAPITAL LETTER SHCHA */
    { "afii10044", 0x042A }, /* CYRILLIC CAPITAL LETTER HARD SIGN */
    { "afii10045", 0x042B }, /* CYRILLIC CAPITAL LETTER YERU */
    { "afii10046", 0x042C }, /* CYRILLIC CAPITAL LETTER SOFT SIGN */
    { "afii10047", 0x042D }, /* CYRILLIC CAPITAL LETTER E */
    { "afii10048", 0x042E }, /* CYRILLIC CAPITAL LETTER YU */
    { "afii10049", 0x042F }, /* CYRILLIC CAPITAL LETTER YA */
    { "afii10050", 0x0490 }, /* CYRILLIC CAPITAL LETTER GHE WITH UPTURN */
    { "afii10051", 0x0402 }, /* CYRILLIC CAPITAL LETTER DJE */
    { "afii10052", 0x0403 }, /* CYRILLIC CAPITAL LETTER GJE */
    { "afii10053", 0x0404 }, /* CYRILLIC CAPITAL LETTER UKRAINIAN IE */
    { "afii10054", 0x0405 }, /* CYRILLIC CAPITAL LETTER DZE */
    { "afii10055", 0x0406 }, /* CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I */
    { "afii10056", 0x0407 }, /* CYRILLIC CAPITAL LETTER YI */
    { "afii10057", 0x0408 }, /* CYRILLIC CAPITAL LETTER JE */
    { "afii10058", 0x0409 }, /* CYRILLIC CAPITAL LETTER LJE */
    { "afii10059", 0x040A }, /* CYRILLIC CAPITAL LETTER NJE */
    { "afii10060", 0x040B }, /* CYRILLIC CAPITAL LETTER TSHE */
    { "afii10061", 0x040C }, /* CYRILLIC CAPITAL LETTER KJE */
    { "afii10062", 0x040E }, /* CYRILLIC CAPITAL LETTER SHORT U */
    { "afii10063", 0xF6C4 }, /* CYRILLIC SMALL LETTER GHE VARIANT */
    { "afii10064", 0xF6C5 }, /* CYRILLIC SMALL LETTER BE VARIANT */
    { "afii10065", 0x0430 }, /* CYRILLIC SMALL LETTER A */
    { "afii10066", 0x0431 }, /* CYRILLIC SMALL LETTER BE */
    { "afii10067", 0x0432 }, /* CYRILLIC SMALL LETTER VE */
    { "afii10068", 0x0433 }, /* CYRILLIC SMALL LETTER GHE */
    { "afii10069", 0x0434 }, /* CYRILLIC SMALL LETTER DE */
    { "afii10070", 0x0435 }, /* CYRILLIC SMALL LETTER IE */
    { "afii10071", 0x0451 }, /* CYRILLIC SMALL LETTER IO */
    { "afii10072", 0x0436 }, /* CYRILLIC SMALL LETTER ZHE */
    { "afii10073", 0x0437 }, /* CYRILLIC SMALL LETTER ZE */
    { "afii10074", 0x0438 }, /* CYRILLIC SMALL LETTER I */
    { "afii10075", 0x0439 }, /* CYRILLIC SMALL LETTER SHORT I */
    { "afii10076", 0x043A }, /* CYRILLIC SMALL LETTER KA */
    { "afii10077", 0x043B }, /* CYRILLIC SMALL LETTER EL */
    { "afii10078", 0x043C }, /* CYRILLIC SMALL LETTER EM */
    { "afii10079", 0x043D }, /* CYRILLIC SMALL LETTER EN */
    { "afii10080", 0x043E }, /* CYRILLIC SMALL LETTER O */
    { "afii10081", 0x043F }, /* CYRILLIC SMALL LETTER PE */
    { "afii10082", 0x0440 }, /* CYRILLIC SMALL LETTER ER */
    { "afii10083", 0x0441 }, /* CYRILLIC SMALL LETTER ES */
    { "afii10084", 0x0442 }, /* CYRILLIC SMALL LETTER TE */
    { "afii10085", 0x0443 }, /* CYRILLIC SMALL LETTER U */
    { "afii10086", 0x0444 }, /* CYRILLIC SMALL LETTER EF */
    { "afii10087", 0x0445 }, /* CYRILLIC SMALL LETTER HA */
    { "afii10088", 0x0446 }, /* CYRILLIC SMALL LETTER TSE */
    { "afii10089", 0x0447 }, /* CYRILLIC SMALL LETTER CHE */
    { "afii10090", 0x0448 }, /* CYRILLIC SMALL LETTER SHA */
    { "afii10091", 0x0449 }, /* CYRILLIC SMALL LETTER SHCHA */
    { "afii10092", 0x044A }, /* CYRILLIC SMALL LETTER HARD SIGN */
    { "afii10093", 0x044B }, /* CYRILLIC SMALL LETTER YERU */
    { "afii10094", 0x044C }, /* CYRILLIC SMALL LETTER SOFT SIGN */
    { "afii10095", 0x044D }, /* CYRILLIC SMALL LETTER E */
    { "afii10096", 0x044E }, /* CYRILLIC SMALL LETTER YU */
    { "afii10097", 0x044F }, /* CYRILLIC SMALL LETTER YA */
    { "afii10098", 0x0491 }, /* CYRILLIC SMALL LETTER GHE WITH UPTURN */
    { "afii10099", 0x0452 }, /* CYRILLIC SMALL LETTER DJE */
    { "afii10100", 0x0453 }, /* CYRILLIC SMALL LETTER GJE */
    { "afii10101", 0x0454 }, /* CYRILLIC SMALL LETTER UKRAINIAN IE */
    { "afii10102", 0x0455 }, /* CYRILLIC SMALL LETTER DZE */
    { "afii10103", 0x0456 }, /* CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I */
    { "afii10104", 0x0457 }, /* CYRILLIC SMALL LETTER YI */
    { "afii10105", 0x0458 }, /* CYRILLIC SMALL LETTER JE */
    { "afii10106", 0x0459 }, /* CYRILLIC SMALL LETTER LJE */
    { "afii10107", 0x045A }, /* CYRILLIC SMALL LETTER NJE */
    { "afii10108", 0x045B }, /* CYRILLIC SMALL LETTER TSHE */
    { "afii10109", 0x045C }, /* CYRILLIC SMALL LETTER KJE */
    { "afii10110", 0x045E }, /* CYRILLIC SMALL LETTER SHORT U */
    { "afii10145", 0x040F }, /* CYRILLIC CAPITAL LETTER DZHE */
    { "afii10146", 0x0462 }, /* CYRILLIC CAPITAL LETTER YAT */
    { "afii10147", 0x0472 }, /* CYRILLIC CAPITAL LETTER FITA */
    { "afii10148", 0x0474 }, /* CYRILLIC CAPITAL LETTER IZHITSA */
    { "afii10192", 0xF6C6 }, /* CYRILLIC SMALL LETTER DE VARIANT */
    { "afii10193", 0x045F }, /* CYRILLIC SMALL LETTER DZHE */
    { "afii10194", 0x0463 }, /* CYRILLIC SMALL LETTER YAT */
    { "afii10195", 0x0473 }, /* CYRILLIC SMALL LETTER FITA */
    { "afii10196", 0x0475 }, /* CYRILLIC SMALL LETTER IZHITSA */
    { "afii10831", 0xF6C7 }, /* CYRILLIC SMALL LETTER PE VARIANT */
    { "afii10832", 0xF6C8 }, /* CYRILLIC SMALL LETTER TE VARIANT */
    { "afii10846", 0x04D9 }, /* CYRILLIC SMALL LETTER SCHWA */
    { "afii299", 0x200E }, /* LEFT-TO-RIGHT MARK */
    { "afii300", 0x200F }, /* RIGHT-TO-LEFT MARK */
    { "afii301", 0x200D }, /* ZERO WIDTH JOINER */
    { "afii57381", 0x066A }, /* ARABIC PERCENT SIGN */
    { "afii57388", 0x060C }, /* ARABIC COMMA */
    { "afii57392", 0x0660 }, /* ARABIC-INDIC DIGIT ZERO */
    { "afii57393", 0x0661 }, /* ARABIC-INDIC DIGIT ONE */
    { "afii57394", 0x0662 }, /* ARABIC-INDIC DIGIT TWO */
    { "afii57395", 0x0663 }, /* ARABIC-INDIC DIGIT THREE */
    { "afii57396", 0x0664 }, /* ARABIC-INDIC DIGIT FOUR */
    { "afii57397", 0x0665 }, /* ARABIC-INDIC DIGIT FIVE */
    { "afii57398", 0x0666 }, /* ARABIC-INDIC DIGIT SIX */
    { "afii57399", 0x0667 }, /* ARABIC-INDIC DIGIT SEVEN */
    { "afii57400", 0x0668 }, /* ARABIC-INDIC DIGIT EIGHT */
    { "afii57401", 0x0669 }, /* ARABIC-INDIC DIGIT NINE */
    { "afii57403", 0x061B }, /* ARABIC SEMICOLON */
    { "afii57407", 0x061F }, /* ARABIC QUESTION MARK */
    { "afii57409", 0x0621 }, /* ARABIC LETTER HAMZA */
    { "afii57410", 0x0622 }, /* ARABIC LETTER ALEF WITH MADDA ABOVE */
    { "afii57411", 0x0623 }, /* ARABIC LETTER ALEF WITH HAMZA ABOVE */
    { "afii57412", 0x0624 }, /* ARABIC LETTER WAW WITH HAMZA ABOVE */
    { "afii57413", 0x0625 }, /* ARABIC LETTER ALEF WITH HAMZA BELOW */
    { "afii57414", 0x0626 }, /* ARABIC LETTER YEH WITH HAMZA ABOVE */
    { "afii57415", 0x0627 }, /* ARABIC LETTER ALEF */
    { "afii57416", 0x0628 }, /* ARABIC LETTER BEH */
    { "afii57417", 0x0629 }, /* ARABIC LETTER TEH MARBUTA */
    { "afii57418", 0x062A }, /* ARABIC LETTER TEH */
    { "afii57419", 0x062B }, /* ARABIC LETTER THEH */
    { "afii57420", 0x062C }, /* ARABIC LETTER JEEM */
    { "afii57421", 0x062D }, /* ARABIC LETTER HAH */
    { "afii57422", 0x062E }, /* ARABIC LETTER KHAH */
    { "afii57423", 0x062F }, /* ARABIC LETTER DAL */
    { "afii57424", 0x0630 }, /* ARABIC LETTER THAL */
    { "afii57425", 0x0631 }, /* ARABIC LETTER REH */
    { "afii57426", 0x0632 }, /* ARABIC LETTER ZAIN */
    { "afii57427", 0x0633 }, /* ARABIC LETTER SEEN */
    { "afii57428", 0x0634 }, /* ARABIC LETTER SHEEN */
    { "afii57429", 0x0635 }, /* ARABIC LETTER SAD */
    { "afii57430", 0x0636 }, /* ARABIC LETTER DAD */
    { "afii57431", 0x0637 }, /* ARABIC LETTER TAH */
    { "afii57432", 0x0638 }, /* ARABIC LETTER ZAH */
    { "afii57433", 0x0639 }, /* ARABIC LETTER AIN */
    { "afii57434", 0x063A }, /* ARABIC LETTER GHAIN */
    { "afii57440", 0x0640 }, /* ARABIC TATWEEL */
    { "afii57441", 0x0641 }, /* ARABIC LETTER FEH */
    { "afii57442", 0x0642 }, /* ARABIC LETTER QAF */
    { "afii57443", 0x0643 }, /* ARABIC LETTER KAF */
    { "afii57444", 0x0644 }, /* ARABIC LETTER LAM */
    { "afii57445", 0x0645 }, /* ARABIC LETTER MEEM */
    { "afii57446", 0x0646 }, /* ARABIC LETTER NOON */
    { "afii57448", 0x0648 }, /* ARABIC LETTER WAW */
    { "afii57449", 0x0649 }, /* ARABIC LETTER ALEF MAKSURA */
    { "afii57450", 0x064A }, /* ARABIC LETTER YEH */
    { "afii57451", 0x064B }, /* ARABIC FATHATAN */
    { "afii57452", 0x064C }, /* ARABIC DAMMATAN */
    { "afii57453", 0x064D }, /* ARABIC KASRATAN */
    { "afii57454", 0x064E }, /* ARABIC FATHA */
    { "afii57455", 0x064F }, /* ARABIC DAMMA */
    { "afii57456", 0x0650 }, /* ARABIC KASRA */
    { "afii57457", 0x0651 }, /* ARABIC SHADDA */
    { "afii57458", 0x0652 }, /* ARABIC SUKUN */
    { "afii57470", 0x0647 }, /* ARABIC LETTER HEH */
    { "afii57505", 0x06A4 }, /* ARABIC LETTER VEH */
    { "afii57506", 0x067E }, /* ARABIC LETTER PEH */
    { "afii57507", 0x0686 }, /* ARABIC LETTER TCHEH */
    { "afii57508", 0x0698 }, /* ARABIC LETTER JEH */
    { "afii57509", 0x06AF }, /* ARABIC LETTER GAF */
    { "afii57511", 0x0679 }, /* ARABIC LETTER TTEH */
    { "afii57512", 0x0688 }, /* ARABIC LETTER DDAL */
    { "afii57513", 0x0691 }, /* ARABIC LETTER RREH */
    { "afii57514", 0x06BA }, /* ARABIC LETTER NOON GHUNNA */
    { "afii57519", 0x06D2 }, /* ARABIC LETTER YEH BARREE */
    { "afii57534", 0x06D5 }, /* ARABIC LETTER AE */
    { "afii57636", 0x20AA }, /* NEW SHEQEL SIGN */
    { "afii57645", 0x05BE }, /* HEBREW PUNCTUATION MAQAF */
    { "afii57658", 0x05C3 }, /* HEBREW PUNCTUATION SOF PASUQ */
    { "afii57664", 0x05D0 }, /* HEBREW LETTER ALEF */
    { "afii57665", 0x05D1 }, /* HEBREW LETTER BET */
    { "afii57666", 0x05D2 }, /* HEBREW LETTER GIMEL */
    { "afii57667", 0x05D3 }, /* HEBREW LETTER DALET */
    { "afii57668", 0x05D4 }, /* HEBREW LETTER HE */
    { "afii57669", 0x05D5 }, /* HEBREW LETTER VAV */
    { "afii57670", 0x05D6 }, /* HEBREW LETTER ZAYIN */
    { "afii57671", 0x05D7 }, /* HEBREW LETTER HET */
    { "afii57672", 0x05D8 }, /* HEBREW LETTER TET */
    { "afii57673", 0x05D9 }, /* HEBREW LETTER YOD */
    { "afii57674", 0x05DA }, /* HEBREW LETTER FINAL KAF */
    { "afii57675", 0x05DB }, /* HEBREW LETTER KAF */
    { "afii57676", 0x05DC }, /* HEBREW LETTER LAMED */
    { "afii57677", 0x05DD }, /* HEBREW LETTER FINAL MEM */
    { "afii57678", 0x05DE }, /* HEBREW LETTER MEM */
    { "afii57679", 0x05DF }, /* HEBREW LETTER FINAL NUN */
    { "afii57680", 0x05E0 }, /* HEBREW LETTER NUN */
    { "afii57681", 0x05E1 }, /* HEBREW LETTER SAMEKH */
    { "afii57682", 0x05E2 }, /* HEBREW LETTER AYIN */
    { "afii57683", 0x05E3 }, /* HEBREW LETTER FINAL PE */
    { "afii57684", 0x05E4 }, /* HEBREW LETTER PE */
    { "afii57685", 0x05E5 }, /* HEBREW LETTER FINAL TSADI */
    { "afii57686", 0x05E6 }, /* HEBREW LETTER TSADI */
    { "afii57687", 0x05E7 }, /* HEBREW LETTER QOF */
    { "afii57688", 0x05E8 }, /* HEBREW LETTER RESH */
    { "afii57689", 0x05E9 }, /* HEBREW LETTER SHIN */
    { "afii57690", 0x05EA }, /* HEBREW LETTER TAV */
    { "afii57694", 0xFB2A }, /* HEBREW LETTER SHIN WITH SHIN DOT */
    { "afii57695", 0xFB2B }, /* HEBREW LETTER SHIN WITH SIN DOT */
    { "afii57700", 0xFB4B }, /* HEBREW LETTER VAV WITH HOLAM */
    { "afii57705", 0xFB1F }, /* HEBREW LIGATURE YIDDISH YOD YOD PATAH */
    { "afii57716", 0x05F0 }, /* HEBREW LIGATURE YIDDISH DOUBLE VAV */
    { "afii57717", 0x05F1 }, /* HEBREW LIGATURE YIDDISH VAV YOD */
    { "afii57718", 0x05F2 }, /* HEBREW LIGATURE YIDDISH DOUBLE YOD */
    { "afii57723", 0xFB35 }, /* HEBREW LETTER VAV WITH DAGESH */
    { "afii57793", 0x05B4 }, /* HEBREW POINT HIRIQ */
    { "afii57794", 0x05B5 }, /* HEBREW POINT TSERE */
    { "afii57795", 0x05B6 }, /* HEBREW POINT SEGOL */
    { "afii57796", 0x05BB }, /* HEBREW POINT QUBUTS */
    { "afii57797", 0x05B8 }, /* HEBREW POINT QAMATS */
    { "afii57798", 0x05B7 }, /* HEBREW POINT PATAH */
    { "afii57799", 0x05B0 }, /* HEBREW POINT SHEVA */
    { "afii57800", 0x05B2 }, /* HEBREW POINT HATAF PATAH */
    { "afii57801", 0x05B1 }, /* HEBREW POINT HATAF SEGOL */
    { "afii57802", 0x05B3 }, /* HEBREW POINT HATAF QAMATS */
    { "afii57803", 0x05C2 }, /* HEBREW POINT SIN DOT */
    { "afii57804", 0x05C1 }, /* HEBREW POINT SHIN DOT */
    { "afii57806", 0x05B9 }, /* HEBREW POINT HOLAM */
    { "afii57807", 0x05BC }, /* HEBREW POINT DAGESH OR MAPIQ */
    { "afii57839", 0x05BD }, /* HEBREW POINT METEG */
    { "afii57841", 0x05BF }, /* HEBREW POINT RAFE */
    { "afii57842", 0x05C0 }, /* HEBREW PUNCTUATION PASEQ */
    { "afii57929", 0x02BC }, /* MODIFIER LETTER APOSTROPHE */
    { "afii61248", 0x2105 }, /* CARE OF */
    { "afii61289", 0x2113 }, /* SCRIPT SMALL L */
    { "afii61352", 0x2116 }, /* NUMERO SIGN */
    { "afii61573", 0x202C }, /* POP DIRECTIONAL FORMATTING */
    { "afii61574", 0x202D }, /* LEFT-TO-RIGHT OVERRIDE */
    { "afii61575", 0x202E }, /* RIGHT-TO-LEFT OVERRIDE */
    { "afii61664", 0x200C }, /* ZERO WIDTH NON-JOINER */
    { "afii63167", 0x066D }, /* ARABIC FIVE POINTED STAR */
    { "afii64937", 0x02BD }, /* MODIFIER LETTER REVERSED COMMA */
    { "agrave", 0x00E0 }, /* LATIN SMALL LETTER A WITH GRAVE */
    { "aleph", 0x2135 }, /* ALEF SYMBOL */
    { "alpha", 0x03B1 }, /* GREEK SMALL LETTER ALPHA */
    { "alphatonos", 0x03AC }, /* GREEK SMALL LETTER ALPHA WITH TONOS */
    { "amacron", 0x0101 }, /* LATIN SMALL LETTER A WITH MACRON */
    { "ampersand", 0x0026 }, /* AMPERSAND */
    { "ampersandsmall", 0xF726 }, /* SMALL CAPITAL AMPERSAND */
    { "angle", 0x2220 }, /* ANGLE */
    { "angleleft", 0x2329 }, /* LEFT-POINTING ANGLE BRACKET */
    { "angleright", 0x232A }, /* RIGHT-POINTING ANGLE BRACKET */
    { "anoteleia", 0x0387 }, /* GREEK ANO TELEIA */
    { "aogonek", 0x0105 }, /* LATIN SMALL LETTER A WITH OGONEK */
    { "approxequal", 0x2248 }, /* ALMOST EQUAL TO */
    { "aring", 0x00E5 }, /* LATIN SMALL LETTER A WITH RING ABOVE */
    { "aringacute", 0x01FB }, /* LATIN SMALL LETTER A WITH RING ABOVE AND ACUTE */
    { "arrowboth", 0x2194 }, /* LEFT RIGHT ARROW */
    { "arrowdblboth", 0x21D4 }, /* LEFT RIGHT DOUBLE ARROW */
    { "arrowdbldown", 0x21D3 }, /* DOWNWARDS DOUBLE ARROW */
    { "arrowdblleft", 0x21D0 }, /* LEFTWARDS DOUBLE ARROW */
    { "arrowdblright", 0x21D2 }, /* RIGHTWARDS DOUBLE ARROW */
    { "arrowdblup", 0x21D1 }, /* UPWARDS DOUBLE ARROW */
    { "arrowdown", 0x2193 }, /* DOWNWARDS ARROW */
    { "arrowhorizex", 0xF8E7 }, /* HORIZONTAL ARROW EXTENDER */
    { "arrowleft", 0x2190 }, /* LEFTWARDS ARROW */
    { "arrowright", 0x2192 }, /* RIGHTWARDS ARROW */
    { "arrowup", 0x2191 }, /* UPWARDS ARROW */
    { "arrowupdn", 0x2195 }, /* UP DOWN ARROW */
    { "arrowupdnbse", 0x21A8 }, /* UP DOWN ARROW WITH BASE */
    { "arrowvertex", 0xF8E6 }, /* VERTICAL ARROW EXTENDER */
    { "asciicircum", 0x005E }, /* CIRCUMFLEX ACCENT */
    { "asciitilde", 0x007E }, /* TILDE */
    { "asterisk", 0x002A }, /* ASTERISK */
    { "asteriskmath", 0x2217 }, /* ASTERISK OPERATOR */
    { "asuperior", 0xF6E9 }, /* SUPERSCRIPT LATIN SMALL LETTER A */
    { "at", 0x0040 }, /* COMMERCIAL AT */
    { "atilde", 0x00E3 }, /* LATIN SMALL LETTER A WITH TILDE */
    { "b", 0x0062 }, /* LATIN SMALL LETTER B */
    { "backslash", 0x005C }, /* REVERSE SOLIDUS */
    { "bar", 0x007C }, /* VERTICAL LINE */
    { "beta", 0x03B2 }, /* GREEK SMALL LETTER BETA */
    { "block", 0x2588 }, /* FULL BLOCK */
    { "braceex", 0xF8F4 }, /* CURLY BRACKET EXTENDER */
    { "braceleft", 0x007B }, /* LEFT CURLY BRACKET */
    { "braceleftbt", 0xF8F3 }, /* LEFT CURLY BRACKET BOTTOM */
    { "braceleftmid", 0xF8F2 }, /* LEFT CURLY BRACKET MID */
    { "bracelefttp", 0xF8F1 }, /* LEFT CURLY BRACKET TOP */
    { "braceright", 0x007D }, /* RIGHT CURLY BRACKET */
    { "bracerightbt", 0xF8FE }, /* RIGHT CURLY BRACKET BOTTOM */
    { "bracerightmid", 0xF8FD }, /* RIGHT CURLY BRACKET MID */
    { "bracerighttp", 0xF8FC }, /* RIGHT CURLY BRACKET TOP */
    { "bracketleft", 0x005B }, /* LEFT SQUARE BRACKET */
    { "bracketleftbt", 0xF8F0 }, /* LEFT SQUARE BRACKET BOTTOM */
    { "bracketleftex", 0xF8EF }, /* LEFT SQUARE BRACKET EXTENDER */
    { "bracketlefttp", 0xF8EE }, /* LEFT SQUARE BRACKET TOP */
    { "bracketright", 0x005D }, /* RIGHT SQUARE BRACKET */
    { "bracketrightbt", 0xF8FB }, /* RIGHT SQUARE BRACKET BOTTOM */
    { "bracketrightex", 0xF8FA }, /* RIGHT SQUARE BRACKET EXTENDER */
    { "bracketrighttp", 0xF8F9 }, /* RIGHT SQUARE BRACKET TOP */
    { "breve", 0x02D8 }, /* BREVE */
    { "brokenbar", 0x00A6 }, /* BROKEN BAR */
    { "bsuperior", 0xF6EA }, /* SUPERSCRIPT LATIN SMALL LETTER B */
    { "bullet", 0x2022 }, /* BULLET */
    { "c", 0x0063 }, /* LATIN SMALL LETTER C */
    { "cacute", 0x0107 }, /* LATIN SMALL LETTER C WITH ACUTE */
    { "caron", 0x02C7 }, /* CARON */
    { "carriagereturn", 0x21B5 }, /* DOWNWARDS ARROW WITH CORNER LEFTWARDS */
    { "ccaron", 0x010D }, /* LATIN SMALL LETTER C WITH CARON */
    { "ccedilla", 0x00E7 }, /* LATIN SMALL LETTER C WITH CEDILLA */
    { "ccircumflex", 0x0109 }, /* LATIN SMALL LETTER C WITH CIRCUMFLEX */
    { "cdotaccent", 0x010B }, /* LATIN SMALL LETTER C WITH DOT ABOVE */
    { "cedilla", 0x00B8 }, /* CEDILLA */
    { "cent", 0x00A2 }, /* CENT SIGN */
    { "centinferior", 0xF6DF }, /* SUBSCRIPT CENT SIGN */
    { "centoldstyle", 0xF7A2 }, /* OLDSTYLE CENT SIGN */
    { "centsuperior", 0xF6E0 }, /* SUPERSCRIPT CENT SIGN */
    { "chi", 0x03C7 }, /* GREEK SMALL LETTER CHI */
    { "circle", 0x25CB }, /* WHITE CIRCLE */
    { "circlemultiply", 0x2297 }, /* CIRCLED TIMES */
    { "circleplus", 0x2295 }, /* CIRCLED PLUS */
    /*     { "circumflex", 0x02C6 }, /\* MODIFIER LETTER CIRCUMFLEX ACCENT *\/ */
    { "circumflex", 0x005E }, /* MODIFIER LETTER CIRCUMFLEX ACCENT */
    { "club", 0x2663 }, /* BLACK CLUB SUIT */
    { "colon", 0x003A }, /* COLON */
    { "colonmonetary", 0x20A1 }, /* COLON SIGN */
    { "comma", 0x002C }, /* COMMA */
    { "commaaccent", 0xF6C3 }, /* COMMA BELOW */
    { "commainferior", 0xF6E1 }, /* SUBSCRIPT COMMA */
    { "commasuperior", 0xF6E2 }, /* SUPERSCRIPT COMMA */
    { "congruent", 0x2245 }, /* APPROXIMATELY EQUAL TO */
    { "copyright", 0x00A9 }, /* COPYRIGHT SIGN */
    { "copyrightsans", 0xF8E9 }, /* COPYRIGHT SIGN SANS SERIF */
    { "copyrightserif", 0xF6D9 }, /* COPYRIGHT SIGN SERIF */
    { "currency", 0x00A4 }, /* CURRENCY SIGN */
    { "cyrBreve", 0xF6D1 }, /* CAPITAL CYRILLIC BREVE */
    { "cyrFlex", 0xF6D2 }, /* CAPITAL CYRILLIC CIRCUMFLEX */
    { "cyrbreve", 0xF6D4 }, /* CYRILLIC BREVE */
    { "cyrflex", 0xF6D5 }, /* CYRILLIC CIRCUMFLEX */
    { "d", 0x0064 }, /* LATIN SMALL LETTER D */
    { "dagger", 0x2020 }, /* DAGGER */
    { "daggerdbl", 0x2021 }, /* DOUBLE DAGGER */
    { "dblGrave", 0xF6D3 }, /* CAPITAL DOUBLE GRAVE ACCENT */
    { "dblgrave", 0xF6D6 }, /* DOUBLE GRAVE ACCENT */
    { "dcaron", 0x010F }, /* LATIN SMALL LETTER D WITH CARON */
    { "dcroat", 0x0111 }, /* LATIN SMALL LETTER D WITH STROKE */
    { "degree", 0x00B0 }, /* DEGREE SIGN */
    { "delta", 0x03B4 }, /* GREEK SMALL LETTER DELTA */
    { "diamond", 0x2666 }, /* BLACK DIAMOND SUIT */
    { "dieresis", 0x00A8 }, /* DIAERESIS */
    { "dieresisacute", 0xF6D7 }, /* DIAERESIS ACUTE ACCENT */
    { "dieresisgrave", 0xF6D8 }, /* DIAERESIS GRAVE ACCENT */
    { "dieresistonos", 0x0385 }, /* GREEK DIALYTIKA TONOS */
    { "divide", 0x00F7 }, /* DIVISION SIGN */
    { "dkshade", 0x2593 }, /* DARK SHADE */
    { "dnblock", 0x2584 }, /* LOWER HALF BLOCK */
    { "dollar", 0x0024 }, /* DOLLAR SIGN */
    { "dollarinferior", 0xF6E3 }, /* SUBSCRIPT DOLLAR SIGN */
    { "dollaroldstyle", 0xF724 }, /* OLDSTYLE DOLLAR SIGN */
    { "dollarsuperior", 0xF6E4 }, /* SUPERSCRIPT DOLLAR SIGN */
    { "dong", 0x20AB }, /* DONG SIGN */
    { "dotaccent", 0x02D9 }, /* DOT ABOVE */
    { "dotbelowcomb", 0x0323 }, /* COMBINING DOT BELOW */
    { "dotlessi", 0x0131 }, /* LATIN SMALL LETTER DOTLESS I */
    { "dotlessj", 0xF6BE }, /* LATIN SMALL LETTER DOTLESS J */
    { "dotmath", 0x22C5 }, /* DOT OPERATOR */
    { "dsuperior", 0xF6EB }, /* SUPERSCRIPT LATIN SMALL LETTER D */
    { "e", 0x0065 }, /* LATIN SMALL LETTER E */
    { "eacute", 0x00E9 }, /* LATIN SMALL LETTER E WITH ACUTE */
    { "ebreve", 0x0115 }, /* LATIN SMALL LETTER E WITH BREVE */
    { "ecaron", 0x011B }, /* LATIN SMALL LETTER E WITH CARON */
    { "ecircumflex", 0x00EA }, /* LATIN SMALL LETTER E WITH CIRCUMFLEX */
    { "edieresis", 0x00EB }, /* LATIN SMALL LETTER E WITH DIAERESIS */
    { "edotaccent", 0x0117 }, /* LATIN SMALL LETTER E WITH DOT ABOVE */
    { "egrave", 0x00E8 }, /* LATIN SMALL LETTER E WITH GRAVE */
    { "eight", 0x0038 }, /* DIGIT EIGHT */
    { "eightinferior", 0x2088 }, /* SUBSCRIPT EIGHT */
    { "eightoldstyle", 0xF738 }, /* OLDSTYLE DIGIT EIGHT */
    { "eightsuperior", 0x2078 }, /* SUPERSCRIPT EIGHT */
    { "element", 0x2208 }, /* ELEMENT OF */
    { "ellipsis", 0x2026 }, /* HORIZONTAL ELLIPSIS */
    { "emacron", 0x0113 }, /* LATIN SMALL LETTER E WITH MACRON */
    { "emdash", 0x2014 }, /* EM DASH */
    { "emptyset", 0x2205 }, /* EMPTY SET */
    { "endash", 0x2013 }, /* EN DASH */
    { "eng", 0x014B }, /* LATIN SMALL LETTER ENG */
    { "eogonek", 0x0119 }, /* LATIN SMALL LETTER E WITH OGONEK */
    { "epsilon", 0x03B5 }, /* GREEK SMALL LETTER EPSILON */
    { "epsilontonos", 0x03AD }, /* GREEK SMALL LETTER EPSILON WITH TONOS */
    { "equal", 0x003D }, /* EQUALS SIGN */
    { "equivalence", 0x2261 }, /* IDENTICAL TO */
    { "estimated", 0x212E }, /* ESTIMATED SYMBOL */
    { "esuperior", 0xF6EC }, /* SUPERSCRIPT LATIN SMALL LETTER E */
    { "eta", 0x03B7 }, /* GREEK SMALL LETTER ETA */
    { "etatonos", 0x03AE }, /* GREEK SMALL LETTER ETA WITH TONOS */
    { "eth", 0x00F0 }, /* LATIN SMALL LETTER ETH */
    { "exclam", 0x0021 }, /* EXCLAMATION MARK */
    { "exclamdbl", 0x203C }, /* DOUBLE EXCLAMATION MARK */
    { "exclamdown", 0x00A1 }, /* INVERTED EXCLAMATION MARK */
    { "exclamdownsmall", 0xF7A1 }, /* SMALL CAPITAL INVERTED EXCLAMATION MARK */
    { "exclamsmall", 0xF721 }, /* SMALL CAPITAL EXCLAMATION MARK */
    { "existential", 0x2203 }, /* THERE EXISTS */
    { "f", 0x0066 }, /* LATIN SMALL LETTER F */
    { "female", 0x2640 }, /* FEMALE SIGN */
    { "ff", 0xFB00 }, /* LATIN SMALL LIGATURE FF */
    { "ffi", 0xFB03 }, /* LATIN SMALL LIGATURE FFI */
    { "ffl", 0xFB04 }, /* LATIN SMALL LIGATURE FFL */
    { "fi", 0xFB01 }, /* LATIN SMALL LIGATURE FI */
    { "figuredash", 0x2012 }, /* FIGURE DASH */
    { "filledbox", 0x25A0 }, /* BLACK SQUARE */
    { "filledrect", 0x25AC }, /* BLACK RECTANGLE */
    { "five", 0x0035 }, /* DIGIT FIVE */
    { "fiveeighths", 0x215D }, /* VULGAR FRACTION FIVE EIGHTHS */
    { "fiveinferior", 0x2085 }, /* SUBSCRIPT FIVE */
    { "fiveoldstyle", 0xF735 }, /* OLDSTYLE DIGIT FIVE */
    { "fivesuperior", 0x2075 }, /* SUPERSCRIPT FIVE */
    { "fl", 0xFB02 }, /* LATIN SMALL LIGATURE FL */
    { "florin", 0x0192 }, /* LATIN SMALL LETTER F WITH HOOK */
    { "four", 0x0034 }, /* DIGIT FOUR */
    { "fourinferior", 0x2084 }, /* SUBSCRIPT FOUR */
    { "fouroldstyle", 0xF734 }, /* OLDSTYLE DIGIT FOUR */
    { "foursuperior", 0x2074 }, /* SUPERSCRIPT FOUR */
    { "fraction", 0x2044 }, /* FRACTION SLASH */
#if 0
    { "fraction" DUP2, 0x2215 }, /* DIVISION SLASH;Duplicate */
#endif /* 0 */
    { "franc", 0x20A3 }, /* FRENCH FRANC SIGN */
    { "ft", 0xFB05 }, /* LATIN SMALL LIGATURE FT */
    { "g", 0x0067 }, /* LATIN SMALL LETTER G */
    { "gamma", 0x03B3 }, /* GREEK SMALL LETTER GAMMA */
    { "gbreve", 0x011F }, /* LATIN SMALL LETTER G WITH BREVE */
    { "gcaron", 0x01E7 }, /* LATIN SMALL LETTER G WITH CARON */
    { "gcircumflex", 0x011D }, /* LATIN SMALL LETTER G WITH CIRCUMFLEX */
    { "gcommaaccent", 0x0123 }, /* LATIN SMALL LETTER G WITH CEDILLA */
    { "gdotaccent", 0x0121 }, /* LATIN SMALL LETTER G WITH DOT ABOVE */
    { "germandbls", 0x00DF }, /* LATIN SMALL LETTER SHARP S */
    { "gradient", 0x2207 }, /* NABLA */
    { "grave", 0x0060 }, /* GRAVE ACCENT */
    { "gravecomb", 0x0300 }, /* COMBINING GRAVE ACCENT */
    { "greater", 0x003E }, /* GREATER-THAN SIGN */
    { "greaterequal", 0x2265 }, /* GREATER-THAN OR EQUAL TO */
    { "guillemotleft", 0x00AB }, /* LEFT-POINTING DOUBLE ANGLE QUOTATION MARK */
    { "guillemotright", 0x00BB }, /* RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK */
    { "guilsinglleft", 0x2039 }, /* SINGLE LEFT-POINTING ANGLE QUOTATION MARK */
    { "guilsinglright", 0x203A }, /* SINGLE RIGHT-POINTING ANGLE QUOTATION MARK */
    { "h", 0x0068 }, /* LATIN SMALL LETTER H */
    { "hbar", 0x0127 }, /* LATIN SMALL LETTER H WITH STROKE */
    { "hcircumflex", 0x0125 }, /* LATIN SMALL LETTER H WITH CIRCUMFLEX */
    { "heart", 0x2665 }, /* BLACK HEART SUIT */
    { "hookabovecomb", 0x0309 }, /* COMBINING HOOK ABOVE */
    { "house", 0x2302 }, /* HOUSE */
    { "hungarumlaut", 0x02DD }, /* DOUBLE ACUTE ACCENT */
    { "hyphen", 0x002D }, /* HYPHEN-MINUS */
#if 0
    { "hyphen" DUP2, 0x00AD }, /* SOFT HYPHEN;Duplicate */
#endif /* 0 */
    { "hypheninferior", 0xF6E5 }, /* SUBSCRIPT HYPHEN-MINUS */
    { "hyphensuperior", 0xF6E6 }, /* SUPERSCRIPT HYPHEN-MINUS */
    { "i", 0x0069 }, /* LATIN SMALL LETTER I */
    { "iacute", 0x00ED }, /* LATIN SMALL LETTER I WITH ACUTE */
    { "ibreve", 0x012D }, /* LATIN SMALL LETTER I WITH BREVE */
    { "icircumflex", 0x00EE }, /* LATIN SMALL LETTER I WITH CIRCUMFLEX */
    { "idieresis", 0x00EF }, /* LATIN SMALL LETTER I WITH DIAERESIS */
    { "igrave", 0x00EC }, /* LATIN SMALL LETTER I WITH GRAVE */
    { "ij", 0x0133 }, /* LATIN SMALL LIGATURE IJ */
    { "imacron", 0x012B }, /* LATIN SMALL LETTER I WITH MACRON */
    { "infinity", 0x221E }, /* INFINITY */
    { "integral", 0x222B }, /* INTEGRAL */
    { "integralbt", 0x2321 }, /* BOTTOM HALF INTEGRAL */
    { "integralex", 0xF8F5 }, /* INTEGRAL EXTENDER */
    { "integraltp", 0x2320 }, /* TOP HALF INTEGRAL */
    { "intersection", 0x2229 }, /* INTERSECTION */
    { "invbullet", 0x25D8 }, /* INVERSE BULLET */
    { "invcircle", 0x25D9 }, /* INVERSE WHITE CIRCLE */
    { "invsmileface", 0x263B }, /* BLACK SMILING FACE */
    { "iogonek", 0x012F }, /* LATIN SMALL LETTER I WITH OGONEK */
    { "iota", 0x03B9 }, /* GREEK SMALL LETTER IOTA */
    { "iotadieresis", 0x03CA }, /* GREEK SMALL LETTER IOTA WITH DIALYTIKA */
    { "iotadieresistonos", 0x0390 }, /* GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS */
    { "iotatonos", 0x03AF }, /* GREEK SMALL LETTER IOTA WITH TONOS */
    { "isuperior", 0xF6ED }, /* SUPERSCRIPT LATIN SMALL LETTER I */
    { "itilde", 0x0129 }, /* LATIN SMALL LETTER I WITH TILDE */
    { "j", 0x006A }, /* LATIN SMALL LETTER J */
    { "jcircumflex", 0x0135 }, /* LATIN SMALL LETTER J WITH CIRCUMFLEX */
    { "k", 0x006B }, /* LATIN SMALL LETTER K */
    { "kappa", 0x03BA }, /* GREEK SMALL LETTER KAPPA */
    { "kcommaaccent", 0x0137 }, /* LATIN SMALL LETTER K WITH CEDILLA */
    { "kgreenlandic", 0x0138 }, /* LATIN SMALL LETTER KRA */
    { "l", 0x006C }, /* LATIN SMALL LETTER L */
    { "lacute", 0x013A }, /* LATIN SMALL LETTER L WITH ACUTE */
    { "lambda", 0x03BB }, /* GREEK SMALL LETTER LAMDA */
    { "lcaron", 0x013E }, /* LATIN SMALL LETTER L WITH CARON */
    { "lcommaaccent", 0x013C }, /* LATIN SMALL LETTER L WITH CEDILLA */
    { "ldot", 0x0140 }, /* LATIN SMALL LETTER L WITH MIDDLE DOT */
    { "less", 0x003C }, /* LESS-THAN SIGN */
    { "lessequal", 0x2264 }, /* LESS-THAN OR EQUAL TO */
    { "lfblock", 0x258C }, /* LEFT HALF BLOCK */
    { "lira", 0x20A4 }, /* LIRA SIGN */
    { "ll", 0xF6C0 }, /* LATIN SMALL LETTER LL */
    { "logicaland", 0x2227 }, /* LOGICAL AND */
    { "logicalnot", 0x00AC }, /* NOT SIGN */
    { "logicalor", 0x2228 }, /* LOGICAL OR */
    { "longs", 0x017F }, /* LATIN SMALL LETTER LONG S */
    { "lozenge", 0x25CA }, /* LOZENGE */
    { "lslash", 0x0142 }, /* LATIN SMALL LETTER L WITH STROKE */
    { "lsuperior", 0xF6EE }, /* SUPERSCRIPT LATIN SMALL LETTER L */
    { "ltshade", 0x2591 }, /* LIGHT SHADE */
    { "m", 0x006D }, /* LATIN SMALL LETTER M */
    { "macron", 0x00AF }, /* MACRON */
#if 0
    { "macron" DUP2, 0x02C9 }, /* MODIFIER LETTER MACRON;Duplicate */
#endif /* 0 */
    { "male", 0x2642 }, /* MALE SIGN */
    { "minus", 0x2212 }, /* MINUS SIGN */
    { "minute", 0x2032 }, /* PRIME */
    { "msuperior", 0xF6EF }, /* SUPERSCRIPT LATIN SMALL LETTER M */
    { "mu", 0x00B5 }, /* MICRO SIGN */
#if 0
    { "mu" DUP2, 0x03BC }, /* GREEK SMALL LETTER MU;Duplicate */
#endif /* 0 */
    { "multiply", 0x00D7 }, /* MULTIPLICATION SIGN */
    { "musicalnote", 0x266A }, /* EIGHTH NOTE */
    { "musicalnotedbl", 0x266B }, /* BEAMED EIGHTH NOTES */
    { "n", 0x006E }, /* LATIN SMALL LETTER N */
    { "nacute", 0x0144 }, /* LATIN SMALL LETTER N WITH ACUTE */
    { "napostrophe", 0x0149 }, /* LATIN SMALL LETTER N PRECEDED BY APOSTROPHE */
    { "ncaron", 0x0148 }, /* LATIN SMALL LETTER N WITH CARON */
    { "ncommaaccent", 0x0146 }, /* LATIN SMALL LETTER N WITH CEDILLA */
    { "nine", 0x0039 }, /* DIGIT NINE */
    { "nineinferior", 0x2089 }, /* SUBSCRIPT NINE */
    { "nineoldstyle", 0xF739 }, /* OLDSTYLE DIGIT NINE */
    { "ninesuperior", 0x2079 }, /* SUPERSCRIPT NINE */
    { "notelement", 0x2209 }, /* NOT AN ELEMENT OF */
    { "notequal", 0x2260 }, /* NOT EQUAL TO */
    { "notsubset", 0x2284 }, /* NOT A SUBSET OF */
    { "nsuperior", 0x207F }, /* SUPERSCRIPT LATIN SMALL LETTER N */
    { "ntilde", 0x00F1 }, /* LATIN SMALL LETTER N WITH TILDE */
    { "nu", 0x03BD }, /* GREEK SMALL LETTER NU */
    { "numbersign", 0x0023 }, /* NUMBER SIGN */
    { "o", 0x006F }, /* LATIN SMALL LETTER O */
    { "oacute", 0x00F3 }, /* LATIN SMALL LETTER O WITH ACUTE */
    { "obreve", 0x014F }, /* LATIN SMALL LETTER O WITH BREVE */
    { "ocircumflex", 0x00F4 }, /* LATIN SMALL LETTER O WITH CIRCUMFLEX */
    { "odieresis", 0x00F6 }, /* LATIN SMALL LETTER O WITH DIAERESIS */
    { "oe", 0x0153 }, /* LATIN SMALL LIGATURE OE */
    { "ogonek", 0x02DB }, /* OGONEK */
    { "ograve", 0x00F2 }, /* LATIN SMALL LETTER O WITH GRAVE */
    { "ohorn", 0x01A1 }, /* LATIN SMALL LETTER O WITH HORN */
    { "ohungarumlaut", 0x0151 }, /* LATIN SMALL LETTER O WITH DOUBLE ACUTE */
    { "omacron", 0x014D }, /* LATIN SMALL LETTER O WITH MACRON */
    { "omega", 0x03C9 }, /* GREEK SMALL LETTER OMEGA */
    { "omega1", 0x03D6 }, /* GREEK PI SYMBOL */
    { "omegatonos", 0x03CE }, /* GREEK SMALL LETTER OMEGA WITH TONOS */
    { "omicron", 0x03BF }, /* GREEK SMALL LETTER OMICRON */
    { "omicrontonos", 0x03CC }, /* GREEK SMALL LETTER OMICRON WITH TONOS */
    { "one", 0x0031 }, /* DIGIT ONE */
    { "onedotenleader", 0x2024 }, /* ONE DOT LEADER */
    { "oneeighth", 0x215B }, /* VULGAR FRACTION ONE EIGHTH */
    { "onefitted", 0xF6DC }, /* PROPORTIONAL DIGIT ONE */
    { "onehalf", 0x00BD }, /* VULGAR FRACTION ONE HALF */
    { "oneinferior", 0x2081 }, /* SUBSCRIPT ONE */
    { "oneoldstyle", 0xF731 }, /* OLDSTYLE DIGIT ONE */
    { "onequarter", 0x00BC }, /* VULGAR FRACTION ONE QUARTER */
    { "onesuperior", 0x00B9 }, /* SUPERSCRIPT ONE */
    { "onethird", 0x2153 }, /* VULGAR FRACTION ONE THIRD */
    { "openbullet", 0x25E6 }, /* WHITE BULLET */
    { "ordfeminine", 0x00AA }, /* FEMININE ORDINAL INDICATOR */
    { "ordmasculine", 0x00BA }, /* MASCULINE ORDINAL INDICATOR */
    { "orthogonal", 0x221F }, /* RIGHT ANGLE */
    { "oslash", 0x00F8 }, /* LATIN SMALL LETTER O WITH STROKE */
    { "oslashacute", 0x01FF }, /* LATIN SMALL LETTER O WITH STROKE AND ACUTE */
    { "osuperior", 0xF6F0 }, /* SUPERSCRIPT LATIN SMALL LETTER O */
    { "otilde", 0x00F5 }, /* LATIN SMALL LETTER O WITH TILDE */
    { "p", 0x0070 }, /* LATIN SMALL LETTER P */
    { "paragraph", 0x00B6 }, /* PILCROW SIGN */
    { "parenleft", 0x0028 }, /* LEFT PARENTHESIS */
    { "parenleftbt", 0xF8ED }, /* LEFT PAREN BOTTOM */
    { "parenleftex", 0xF8EC }, /* LEFT PAREN EXTENDER */
    { "parenleftinferior", 0x208D }, /* SUBSCRIPT LEFT PARENTHESIS */
    { "parenleftsuperior", 0x207D }, /* SUPERSCRIPT LEFT PARENTHESIS */
    { "parenlefttp", 0xF8EB }, /* LEFT PAREN TOP */
    { "parenright", 0x0029 }, /* RIGHT PARENTHESIS */
    { "parenrightbt", 0xF8F8 }, /* RIGHT PAREN BOTTOM */
    { "parenrightex", 0xF8F7 }, /* RIGHT PAREN EXTENDER */
    { "parenrightinferior", 0x208E }, /* SUBSCRIPT RIGHT PARENTHESIS */
    { "parenrightsuperior", 0x207E }, /* SUPERSCRIPT RIGHT PARENTHESIS */
    { "parenrighttp", 0xF8F6 }, /* RIGHT PAREN TOP */
    { "partialdiff", 0x2202 }, /* PARTIAL DIFFERENTIAL */
    { "percent", 0x0025 }, /* PERCENT SIGN */
    { "period", 0x002E }, /* FULL STOP */
    { "periodcentered", 0x00B7 }, /* MIDDLE DOT */
#if 0
    { "periodcentered" DUP2, 0x2219 }, /* BULLET OPERATOR;Duplicate */
#endif /* 0 */
    { "periodinferior", 0xF6E7 }, /* SUBSCRIPT FULL STOP */
    { "periodsuperior", 0xF6E8 }, /* SUPERSCRIPT FULL STOP */
    { "perpendicular", 0x22A5 }, /* UP TACK */
    { "perthousand", 0x2030 }, /* PER MILLE SIGN */
    { "peseta", 0x20A7 }, /* PESETA SIGN */
    { "phi", 0x03C6 }, /* GREEK SMALL LETTER PHI */
    { "phi1", 0x03D5 }, /* GREEK PHI SYMBOL */
    { "pi", 0x03C0 }, /* GREEK SMALL LETTER PI */
    { "plus", 0x002B }, /* PLUS SIGN */
    { "plusminus", 0x00B1 }, /* PLUS-MINUS SIGN */
    { "prescription", 0x211E }, /* PRESCRIPTION TAKE */
    { "product", 0x220F }, /* N-ARY PRODUCT */
    { "propersubset", 0x2282 }, /* SUBSET OF */
    { "propersuperset", 0x2283 }, /* SUPERSET OF */
    { "proportional", 0x221D }, /* PROPORTIONAL TO */
    { "psi", 0x03C8 }, /* GREEK SMALL LETTER PSI */
    { "q", 0x0071 }, /* LATIN SMALL LETTER Q */
    { "question", 0x003F }, /* QUESTION MARK */
    { "questiondown", 0x00BF }, /* INVERTED QUESTION MARK */
    { "questiondownsmall", 0xF7BF }, /* SMALL CAPITAL INVERTED QUESTION MARK */
    { "questionsmall", 0xF73F }, /* SMALL CAPITAL QUESTION MARK */
    { "quotedbl", 0x0022 }, /* QUOTATION MARK */
    { "quotedblbase", 0x201E }, /* DOUBLE LOW-9 QUOTATION MARK */
    { "quotedblleft", 0x201C }, /* LEFT DOUBLE QUOTATION MARK */
    { "quotedblright", 0x201D }, /* RIGHT DOUBLE QUOTATION MARK */
    { "quoteleft", 0x2018 }, /* LEFT SINGLE QUOTATION MARK */
    { "quotereversed", 0x201B }, /* SINGLE HIGH-REVERSED-9 QUOTATION MARK */
    { "quoteright", 0x2019 }, /* RIGHT SINGLE QUOTATION MARK */
    { "quotesinglbase", 0x201A }, /* SINGLE LOW-9 QUOTATION MARK */
    { "quotesingle", 0x0027 }, /* APOSTROPHE */
    { "r", 0x0072 }, /* LATIN SMALL LETTER R */
    { "racute", 0x0155 }, /* LATIN SMALL LETTER R WITH ACUTE */
    { "radical", 0x221A }, /* SQUARE ROOT */
    { "radicalex", 0xF8E5 }, /* RADICAL EXTENDER */
    { "rcaron", 0x0159 }, /* LATIN SMALL LETTER R WITH CARON */
    { "rcommaaccent", 0x0157 }, /* LATIN SMALL LETTER R WITH CEDILLA */
    { "reflexsubset", 0x2286 }, /* SUBSET OF OR EQUAL TO */
    { "reflexsuperset", 0x2287 }, /* SUPERSET OF OR EQUAL TO */
    { "registered", 0x00AE }, /* REGISTERED SIGN */
    { "registersans", 0xF8E8 }, /* REGISTERED SIGN SANS SERIF */
    { "registerserif", 0xF6DA }, /* REGISTERED SIGN SERIF */
    { "revlogicalnot", 0x2310 }, /* REVERSED NOT SIGN */
    { "rho", 0x03C1 }, /* GREEK SMALL LETTER RHO */
    { "ring", 0x02DA }, /* RING ABOVE */
    { "rsuperior", 0xF6F1 }, /* SUPERSCRIPT LATIN SMALL LETTER R */
    { "rtblock", 0x2590 }, /* RIGHT HALF BLOCK */
    { "rupiah", 0xF6DD }, /* RUPIAH SIGN */
    { "s", 0x0073 }, /* LATIN SMALL LETTER S */
    { "sacute", 0x015B }, /* LATIN SMALL LETTER S WITH ACUTE */
    { "scaron", 0x0161 }, /* LATIN SMALL LETTER S WITH CARON */
    { "scedilla", 0x015F }, /* LATIN SMALL LETTER S WITH CEDILLA */
#if 0
    { "scedilla" DUP2, 0xF6C2 }, /* LATIN SMALL LETTER S WITH CEDILLA;Duplicate */
#endif /* 0 */
    { "scircumflex", 0x015D }, /* LATIN SMALL LETTER S WITH CIRCUMFLEX */
    { "scommaaccent", 0x0219 }, /* LATIN SMALL LETTER S WITH COMMA BELOW */
    { "second", 0x2033 }, /* DOUBLE PRIME */
    { "section", 0x00A7 }, /* SECTION SIGN */
    { "semicolon", 0x003B }, /* SEMICOLON */
    { "seven", 0x0037 }, /* DIGIT SEVEN */
    { "seveneighths", 0x215E }, /* VULGAR FRACTION SEVEN EIGHTHS */
    { "seveninferior", 0x2087 }, /* SUBSCRIPT SEVEN */
    { "sevenoldstyle", 0xF737 }, /* OLDSTYLE DIGIT SEVEN */
    { "sevensuperior", 0x2077 }, /* SUPERSCRIPT SEVEN */
    { "shade", 0x2592 }, /* MEDIUM SHADE */
    { "sigma", 0x03C3 }, /* GREEK SMALL LETTER SIGMA */
    { "sigma1", 0x03C2 }, /* GREEK SMALL LETTER FINAL SIGMA */
    { "similar", 0x223C }, /* TILDE OPERATOR */
    { "six", 0x0036 }, /* DIGIT SIX */
    { "sixinferior", 0x2086 }, /* SUBSCRIPT SIX */
    { "sixoldstyle", 0xF736 }, /* OLDSTYLE DIGIT SIX */
    { "sixsuperior", 0x2076 }, /* SUPERSCRIPT SIX */
    { "slash", 0x002F }, /* SOLIDUS */
    { "smileface", 0x263A }, /* WHITE SMILING FACE */
    { "space", 0x0020 }, /* SPACE */
#if 0
    { "space" DUP2, 0x00A0 }, /* NO-BREAK SPACE;Duplicate */
#endif /* 0 */
    { "spade", 0x2660 }, /* BLACK SPADE SUIT */
    { "ssuperior", 0xF6F2 }, /* SUPERSCRIPT LATIN SMALL LETTER S */
    { "st", 0xFB06 }, /* LATIN SMALL LIGATURE ST */
    { "sterling", 0x00A3 }, /* POUND SIGN */
    { "suchthat", 0x220B }, /* CONTAINS AS MEMBER */
    { "summation", 0x2211 }, /* N-ARY SUMMATION */
    { "summationtext", 0x2211 }, /* N-ARY SUMMATION - apparently this is the name used in cmex? */
    { "sun", 0x263C }, /* WHITE SUN WITH RAYS */
    { "t", 0x0074 }, /* LATIN SMALL LETTER T */
    { "tau", 0x03C4 }, /* GREEK SMALL LETTER TAU */
    { "tbar", 0x0167 }, /* LATIN SMALL LETTER T WITH STROKE */
    { "tcaron", 0x0165 }, /* LATIN SMALL LETTER T WITH CARON */
    { "tcommaaccent", 0x0163 }, /* LATIN SMALL LETTER T WITH CEDILLA */
#if 0
    { "tcommaaccent" DUP2, 0x021B }, /* LATIN SMALL LETTER T WITH COMMA BELOW;Duplicate */
#endif /* 0 */
    { "therefore", 0x2234 }, /* THEREFORE */
    { "theta", 0x03B8 }, /* GREEK SMALL LETTER THETA */
    { "theta1", 0x03D1 }, /* GREEK THETA SYMBOL */
    { "thorn", 0x00FE }, /* LATIN SMALL LETTER THORN */
    { "three", 0x0033 }, /* DIGIT THREE */
    { "threeeighths", 0x215C }, /* VULGAR FRACTION THREE EIGHTHS */
    { "threeinferior", 0x2083 }, /* SUBSCRIPT THREE */
    { "threeoldstyle", 0xF733 }, /* OLDSTYLE DIGIT THREE */
    { "threequarters", 0x00BE }, /* VULGAR FRACTION THREE QUARTERS */
    { "threequartersemdash", 0xF6DE }, /* THREE QUARTERS EM DASH */
    { "threesuperior", 0x00B3 }, /* SUPERSCRIPT THREE */
    /*     { "tilde", 0x02DC }, /\* SMALL TILDE *\/ */
    { "tilde", 0x007E }, /* ASCII TILDE */
    { "tildecomb", 0x0303 }, /* COMBINING TILDE */
    { "tonos", 0x0384 }, /* GREEK TONOS */
    { "trademark", 0x2122 }, /* TRADE MARK SIGN */
    { "trademarksans", 0xF8EA }, /* TRADE MARK SIGN SANS SERIF */
    { "trademarkserif", 0xF6DB }, /* TRADE MARK SIGN SERIF */
    { "triagdn", 0x25BC }, /* BLACK DOWN-POINTING TRIANGLE */
    { "triaglf", 0x25C4 }, /* BLACK LEFT-POINTING POINTER */
    { "triagrt", 0x25BA }, /* BLACK RIGHT-POINTING POINTER */
    { "triagup", 0x25B2 }, /* BLACK UP-POINTING TRIANGLE */
    { "tsuperior", 0xF6F3 }, /* SUPERSCRIPT LATIN SMALL LETTER T */
    { "two", 0x0032 }, /* DIGIT TWO */
    { "twodotenleader", 0x2025 }, /* TWO DOT LEADER */
    { "twoinferior", 0x2082 }, /* SUBSCRIPT TWO */
    { "twooldstyle", 0xF732 }, /* OLDSTYLE DIGIT TWO */
    { "twosuperior", 0x00B2 }, /* SUPERSCRIPT TWO */
    { "twothirds", 0x2154 }, /* VULGAR FRACTION TWO THIRDS */
    { "u", 0x0075 }, /* LATIN SMALL LETTER U */
    { "uacute", 0x00FA }, /* LATIN SMALL LETTER U WITH ACUTE */
    { "ubreve", 0x016D }, /* LATIN SMALL LETTER U WITH BREVE */
    { "ucircumflex", 0x00FB }, /* LATIN SMALL LETTER U WITH CIRCUMFLEX */
    { "udieresis", 0x00FC }, /* LATIN SMALL LETTER U WITH DIAERESIS */
    { "ugrave", 0x00F9 }, /* LATIN SMALL LETTER U WITH GRAVE */
    { "uhorn", 0x01B0 }, /* LATIN SMALL LETTER U WITH HORN */
    { "uhungarumlaut", 0x0171 }, /* LATIN SMALL LETTER U WITH DOUBLE ACUTE */
    { "umacron", 0x016B }, /* LATIN SMALL LETTER U WITH MACRON */
    { "underscore", 0x005F }, /* LOW LINE */
    { "underscoredbl", 0x2017 }, /* DOUBLE LOW LINE */
    { "union", 0x222A }, /* UNION */
    { "universal", 0x2200 }, /* FOR ALL */
    { "uogonek", 0x0173 }, /* LATIN SMALL LETTER U WITH OGONEK */
    { "upblock", 0x2580 }, /* UPPER HALF BLOCK */
    { "upsilon", 0x03C5 }, /* GREEK SMALL LETTER UPSILON */
    { "upsilondieresis", 0x03CB }, /* GREEK SMALL LETTER UPSILON WITH DIALYTIKA */
    { "upsilondieresistonos", 0x03B0 }, /* GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS */
    { "upsilontonos", 0x03CD }, /* GREEK SMALL LETTER UPSILON WITH TONOS */
    { "uring", 0x016F }, /* LATIN SMALL LETTER U WITH RING ABOVE */
    { "utilde", 0x0169 }, /* LATIN SMALL LETTER U WITH TILDE */
    { "v", 0x0076 }, /* LATIN SMALL LETTER V */
    { "w", 0x0077 }, /* LATIN SMALL LETTER W */
    { "wacute", 0x1E83 }, /* LATIN SMALL LETTER W WITH ACUTE */
    { "wcircumflex", 0x0175 }, /* LATIN SMALL LETTER W WITH CIRCUMFLEX */
    { "wdieresis", 0x1E85 }, /* LATIN SMALL LETTER W WITH DIAERESIS */
    { "weierstrass", 0x2118 }, /* SCRIPT CAPITAL P */
    { "wgrave", 0x1E81 }, /* LATIN SMALL LETTER W WITH GRAVE */
    { "x", 0x0078 }, /* LATIN SMALL LETTER X */
    { "xi", 0x03BE }, /* GREEK SMALL LETTER XI */
    { "y", 0x0079 }, /* LATIN SMALL LETTER Y */
    { "yacute", 0x00FD }, /* LATIN SMALL LETTER Y WITH ACUTE */
    { "ycircumflex", 0x0177 }, /* LATIN SMALL LETTER Y WITH CIRCUMFLEX */
    { "ydieresis", 0x00FF }, /* LATIN SMALL LETTER Y WITH DIAERESIS */
    { "yen", 0x00A5 }, /* YEN SIGN */
    { "ygrave", 0x1EF3 }, /* LATIN SMALL LETTER Y WITH GRAVE */
    { "z", 0x007A }, /* LATIN SMALL LETTER Z */
    { "zacute", 0x017A }, /* LATIN SMALL LETTER Z WITH ACUTE */
    { "zcaron", 0x017E }, /* LATIN SMALL LETTER Z WITH CARON */
    { "zdotaccent", 0x017C }, /* LATIN SMALL LETTER Z WITH DOT ABOVE */
    { "zero", 0x0030 }, /* DIGIT ZERO */
    { "zeroinferior", 0x2080 }, /* SUBSCRIPT ZERO */
    { "zerooldstyle", 0xF730 }, /* OLDSTYLE DIGIT ZERO */
    { "zerosuperior", 0x2070 }, /* SUPERSCRIPT ZERO */
    { "zeta", 0x03B6 } /* GREEK SMALL LETTER ZETA */
};

#if 0
static struct unicode2adobe unicode2adobe_table[] = {
    { 0, ".notdef" }, /* was: QUESTION MARK; changed to 0 */
    { 0x0020, "space" }, /* SPACE */
    { 0x0021, "exclam" }, /* EXCLAMATION MARK */
    { 0x0022, "quotedbl" }, /* QUOTATION MARK */
    { 0x0023, "numbersign" }, /* NUMBER SIGN */
    { 0x0024, "dollar" }, /* DOLLAR SIGN */
    { 0x0025, "percent" }, /* PERCENT SIGN */
    { 0x0026, "ampersand" }, /* AMPERSAND */
    { 0x0027, "quotesingle" }, /* APOSTROPHE */
    { 0x0028, "parenleft" }, /* LEFT PARENTHESIS */
    { 0x0029, "parenright" }, /* RIGHT PARENTHESIS */
    { 0x002A, "asterisk" }, /* ASTERISK */
    { 0x002B, "plus" }, /* PLUS SIGN */
    { 0x002C, "comma" }, /* COMMA */
    { 0x002D, "hyphen" }, /* HYPHEN-MINUS */
    { 0x002D, "$hyphen"  }, /* HYPHEN-MINUS; distinguish Adobe duplicates */
    { 0x002E, "period" }, /* FULL STOP */
    { 0x002F, "slash" }, /* SOLIDUS */
    { 0x0030, "zero" }, /* DIGIT ZERO */
    { 0x0031, "one" }, /* DIGIT ONE */
    { 0x0032, "two" }, /* DIGIT TWO */
    { 0x0033, "three" }, /* DIGIT THREE */
    { 0x0034, "four" }, /* DIGIT FOUR */
    { 0x0035, "five" }, /* DIGIT FIVE */
    { 0x0036, "six" }, /* DIGIT SIX */
    { 0x0037, "seven" }, /* DIGIT SEVEN */
    { 0x0038, "eight" }, /* DIGIT EIGHT */
    { 0x0039, "nine" }, /* DIGIT NINE */
    { 0x003A, "colon" }, /* COLON */
    { 0x003B, "semicolon" }, /* SEMICOLON */
    { 0x003C, "less" }, /* LESS-THAN SIGN */
    { 0x003D, "equal" }, /* EQUALS SIGN */
    { 0x003E, "greater" }, /* GREATER-THAN SIGN */
    { 0x003F, "question" }, /* QUESTION MARK */
    { 0x0040, "at" }, /* COMMERCIAL AT */
    { 0x0041, "A" }, /* LATIN CAPITAL LETTER A */
    { 0x0042, "B" }, /* LATIN CAPITAL LETTER B */
    { 0x0043, "C" }, /* LATIN CAPITAL LETTER C */
    { 0x0044, "D" }, /* LATIN CAPITAL LETTER D */
    { 0x0045, "E" }, /* LATIN CAPITAL LETTER E */
    { 0x0046, "F" }, /* LATIN CAPITAL LETTER F */
    { 0x0047, "G" }, /* LATIN CAPITAL LETTER G */
    { 0x0048, "H" }, /* LATIN CAPITAL LETTER H */
    { 0x0049, "I" }, /* LATIN CAPITAL LETTER I */
    { 0x004A, "J" }, /* LATIN CAPITAL LETTER J */
    { 0x004B, "K" }, /* LATIN CAPITAL LETTER K */
    { 0x004C, "L" }, /* LATIN CAPITAL LETTER L */
    { 0x004D, "M" }, /* LATIN CAPITAL LETTER M */
    { 0x004E, "N" }, /* LATIN CAPITAL LETTER N */
    { 0x004F, "O" }, /* LATIN CAPITAL LETTER O */
    { 0x0050, "P" }, /* LATIN CAPITAL LETTER P */
    { 0x0051, "Q" }, /* LATIN CAPITAL LETTER Q */
    { 0x0052, "R" }, /* LATIN CAPITAL LETTER R */
    { 0x0053, "S" }, /* LATIN CAPITAL LETTER S */
    { 0x0054, "T" }, /* LATIN CAPITAL LETTER T */
    { 0x0055, "U" }, /* LATIN CAPITAL LETTER U */
    { 0x0056, "V" }, /* LATIN CAPITAL LETTER V */
    { 0x0057, "W" }, /* LATIN CAPITAL LETTER W */
    { 0x0058, "X" }, /* LATIN CAPITAL LETTER X */
    { 0x0059, "Y" }, /* LATIN CAPITAL LETTER Y */
    { 0x005A, "Z" }, /* LATIN CAPITAL LETTER Z */
    { 0x005B, "bracketleft" }, /* LEFT SQUARE BRACKET */
    { 0x005C, "backslash" }, /* REVERSE SOLIDUS */
    { 0x005D, "bracketright" }, /* RIGHT SQUARE BRACKET */
    { 0x005E, "circumflex" }, /* CIRCUMFLEX ACCENT */ 	/* SU: renamed from asciicircum */
    { 0x005F, "underscore" }, /* LOW LINE */
    { 0x0060, "grave" }, /* GRAVE ACCENT */
    { 0x0061, "a" }, /* LATIN SMALL LETTER A */
    { 0x0062, "b" }, /* LATIN SMALL LETTER B */
    { 0x0063, "c" }, /* LATIN SMALL LETTER C */
    { 0x0064, "d" }, /* LATIN SMALL LETTER D */
    { 0x0065, "e" }, /* LATIN SMALL LETTER E */
    { 0x0066, "f" }, /* LATIN SMALL LETTER F */
    { 0x0067, "g" }, /* LATIN SMALL LETTER G */
    { 0x0068, "h" }, /* LATIN SMALL LETTER H */
    { 0x0069, "i" }, /* LATIN SMALL LETTER I */
    { 0x006A, "j" }, /* LATIN SMALL LETTER J */
    { 0x006B, "k" }, /* LATIN SMALL LETTER K */
    { 0x006C, "l" }, /* LATIN SMALL LETTER L */
    { 0x006D, "m" }, /* LATIN SMALL LETTER M */
    { 0x006E, "n" }, /* LATIN SMALL LETTER N */
    { 0x006F, "o" }, /* LATIN SMALL LETTER O */
    { 0x0070, "p" }, /* LATIN SMALL LETTER P */
    { 0x0071, "q" }, /* LATIN SMALL LETTER Q */
    { 0x0072, "r" }, /* LATIN SMALL LETTER R */
    { 0x0073, "s" }, /* LATIN SMALL LETTER S */
    { 0x0074, "t" }, /* LATIN SMALL LETTER T */
    { 0x0075, "u" }, /* LATIN SMALL LETTER U */
    { 0x0076, "v" }, /* LATIN SMALL LETTER V */
    { 0x0077, "w" }, /* LATIN SMALL LETTER W */
    { 0x0078, "x" }, /* LATIN SMALL LETTER X */
    { 0x0079, "y" }, /* LATIN SMALL LETTER Y */
    { 0x007A, "z" }, /* LATIN SMALL LETTER Z */
    { 0x007B, "braceleft" }, /* LEFT CURLY BRACKET */
    { 0x007C, "bar" }, /* VERTICAL LINE */
    { 0x007D, "braceright" }, /* RIGHT CURLY BRACKET */
    { 0x007E, "tilde" }, /* TILDE */ 			/* SU: renamed from asciitilde */
    { 0x00A0, "$spacenobreak"  }, /* NO-BREAK SPACE; distinguish Adobe duplicates */
    { 0x00A1, "exclamdown" }, /* INVERTED EXCLAMATION MARK */
    { 0x00A2, "cent" }, /* CENT SIGN */
    { 0x00A3, "sterling" }, /* POUND SIGN */
    { 0x00A4, "currency" }, /* CURRENCY SIGN */
    { 0x00A5, "yen" }, /* YEN SIGN */
    { 0x00A6, "brokenbar" }, /* BROKEN BAR */
    { 0x00A7, "section" }, /* SECTION SIGN */
    { 0x00A8, "dieresis" }, /* DIAERESIS */
    { 0x00A9, "copyright" }, /* COPYRIGHT SIGN */
    { 0x00AA, "ordfeminine" }, /* FEMININE ORDINAL INDICATOR */
    { 0x00AB, "guillemotleft" }, /* LEFT-POINTING DOUBLE ANGLE QUOTATION MARK */
    { 0x00AC, "logicalnot" }, /* NOT SIGN */
    { 0x00AD, "$hyphensoft"  }, /* SOFT HYPHEN; distinguish Adobe duplicates */
    { 0x00AE, "registered" }, /* REGISTERED SIGN */
    { 0x00AF, "macron" }, /* MACRON */
    { 0x00AF, "$macron" }, /* MACRON; distinguish Adobe duplicates */
    { 0x00B0, "degree" }, /* DEGREE SIGN */
    { 0x00B1, "plusminus" }, /* PLUS-MINUS SIGN */
    { 0x00B2, "twosuperior" }, /* SUPERSCRIPT TWO */
    { 0x00B3, "threesuperior" }, /* SUPERSCRIPT THREE */
    { 0x00B4, "acute" }, /* ACUTE ACCENT */
    { 0x00B5, "$micro" }, /* MICRO SIGN; distinguish Adobe duplicates */
    { 0x00B5, "mu" }, /* MICRO SIGN */
    { 0x00B6, "paragraph" }, /* PILCROW SIGN */
    { 0x00B7, "periodcentered" }, /* MIDDLE DOT */
    { 0x00B7, "$periodcentered"  }, /* MIDDLE DOT; distinguish Adobe duplicates */
    { 0x00B8, "cedilla" }, /* CEDILLA */
    { 0x00B9, "onesuperior" }, /* SUPERSCRIPT ONE */
    { 0x00BA, "ordmasculine" }, /* MASCULINE ORDINAL INDICATOR */
    { 0x00BB, "guillemotright" }, /* RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK */
    { 0x00BC, "onequarter" }, /* VULGAR FRACTION ONE QUARTER */
    { 0x00BD, "onehalf" }, /* VULGAR FRACTION ONE HALF */
    { 0x00BE, "threequarters" }, /* VULGAR FRACTION THREE QUARTERS */
    { 0x00BF, "questiondown" }, /* INVERTED QUESTION MARK */
    { 0x00C0, "Agrave" }, /* LATIN CAPITAL LETTER A WITH GRAVE */
    { 0x00C1, "Aacute" }, /* LATIN CAPITAL LETTER A WITH ACUTE */
    { 0x00C2, "Acircumflex" }, /* LATIN CAPITAL LETTER A WITH CIRCUMFLEX */
    { 0x00C3, "Atilde" }, /* LATIN CAPITAL LETTER A WITH TILDE */
    { 0x00C4, "Adieresis" }, /* LATIN CAPITAL LETTER A WITH DIAERESIS */
    { 0x00C5, "Aring" }, /* LATIN CAPITAL LETTER A WITH RING ABOVE */
    { 0x00C6, "AE" }, /* LATIN CAPITAL LETTER AE */
    { 0x00C7, "Ccedilla" }, /* LATIN CAPITAL LETTER C WITH CEDILLA */
    { 0x00C8, "Egrave" }, /* LATIN CAPITAL LETTER E WITH GRAVE */
    { 0x00C9, "Eacute" }, /* LATIN CAPITAL LETTER E WITH ACUTE */
    { 0x00CA, "Ecircumflex" }, /* LATIN CAPITAL LETTER E WITH CIRCUMFLEX */
    { 0x00CB, "Edieresis" }, /* LATIN CAPITAL LETTER E WITH DIAERESIS */
    { 0x00CC, "Igrave" }, /* LATIN CAPITAL LETTER I WITH GRAVE */
    { 0x00CD, "Iacute" }, /* LATIN CAPITAL LETTER I WITH ACUTE */
    { 0x00CE, "Icircumflex" }, /* LATIN CAPITAL LETTER I WITH CIRCUMFLEX */
    { 0x00CF, "Idieresis" }, /* LATIN CAPITAL LETTER I WITH DIAERESIS */
    { 0x00D0, "Eth" }, /* LATIN CAPITAL LETTER ETH */
    { 0x00D1, "Ntilde" }, /* LATIN CAPITAL LETTER N WITH TILDE */
    { 0x00D2, "Ograve" }, /* LATIN CAPITAL LETTER O WITH GRAVE */
    { 0x00D3, "Oacute" }, /* LATIN CAPITAL LETTER O WITH ACUTE */
    { 0x00D4, "Ocircumflex" }, /* LATIN CAPITAL LETTER O WITH CIRCUMFLEX */
    { 0x00D5, "Otilde" }, /* LATIN CAPITAL LETTER O WITH TILDE */
    { 0x00D6, "Odieresis" }, /* LATIN CAPITAL LETTER O WITH DIAERESIS */
    { 0x00D7, "multiply" }, /* MULTIPLICATION SIGN */
    { 0x00D8, "Oslash" }, /* LATIN CAPITAL LETTER O WITH STROKE */
    { 0x00D9, "Ugrave" }, /* LATIN CAPITAL LETTER U WITH GRAVE */
    { 0x00DA, "Uacute" }, /* LATIN CAPITAL LETTER U WITH ACUTE */
    { 0x00DB, "Ucircumflex" }, /* LATIN CAPITAL LETTER U WITH CIRCUMFLEX */
    { 0x00DC, "Udieresis" }, /* LATIN CAPITAL LETTER U WITH DIAERESIS */
    { 0x00DD, "Yacute" }, /* LATIN CAPITAL LETTER Y WITH ACUTE */
    { 0x00DE, "Thorn" }, /* LATIN CAPITAL LETTER THORN */
    { 0x00DF, "germandbls" }, /* LATIN SMALL LETTER SHARP S */
    { 0x00E0, "agrave" }, /* LATIN SMALL LETTER A WITH GRAVE */
    { 0x00E1, "aacute" }, /* LATIN SMALL LETTER A WITH ACUTE */
    { 0x00E2, "acircumflex" }, /* LATIN SMALL LETTER A WITH CIRCUMFLEX */
    { 0x00E3, "atilde" }, /* LATIN SMALL LETTER A WITH TILDE */
    { 0x00E4, "adieresis" }, /* LATIN SMALL LETTER A WITH DIAERESIS */
    { 0x00E5, "aring" }, /* LATIN SMALL LETTER A WITH RING ABOVE */
    { 0x00E6, "ae" }, /* LATIN SMALL LETTER AE */
    { 0x00E7, "ccedilla" }, /* LATIN SMALL LETTER C WITH CEDILLA */
    { 0x00E8, "egrave" }, /* LATIN SMALL LETTER E WITH GRAVE */
    { 0x00E9, "eacute" }, /* LATIN SMALL LETTER E WITH ACUTE */
    { 0x00EA, "ecircumflex" }, /* LATIN SMALL LETTER E WITH CIRCUMFLEX */
    { 0x00EB, "edieresis" }, /* LATIN SMALL LETTER E WITH DIAERESIS */
    { 0x00EC, "igrave" }, /* LATIN SMALL LETTER I WITH GRAVE */
    { 0x00ED, "iacute" }, /* LATIN SMALL LETTER I WITH ACUTE */
    { 0x00EE, "icircumflex" }, /* LATIN SMALL LETTER I WITH CIRCUMFLEX */
    { 0x00EF, "idieresis" }, /* LATIN SMALL LETTER I WITH DIAERESIS */
    { 0x00F0, "eth" }, /* LATIN SMALL LETTER ETH */
    { 0x00F1, "ntilde" }, /* LATIN SMALL LETTER N WITH TILDE */
    { 0x00F2, "ograve" }, /* LATIN SMALL LETTER O WITH GRAVE */
    { 0x00F3, "oacute" }, /* LATIN SMALL LETTER O WITH ACUTE */
    { 0x00F4, "ocircumflex" }, /* LATIN SMALL LETTER O WITH CIRCUMFLEX */
    { 0x00F5, "otilde" }, /* LATIN SMALL LETTER O WITH TILDE */
    { 0x00F6, "odieresis" }, /* LATIN SMALL LETTER O WITH DIAERESIS */
    { 0x00F7, "divide" }, /* DIVISION SIGN */
    { 0x00F8, "oslash" }, /* LATIN SMALL LETTER O WITH STROKE */
    { 0x00F9, "ugrave" }, /* LATIN SMALL LETTER U WITH GRAVE */
    { 0x00FA, "uacute" }, /* LATIN SMALL LETTER U WITH ACUTE */
    { 0x00FB, "ucircumflex" }, /* LATIN SMALL LETTER U WITH CIRCUMFLEX */
    { 0x00FC, "udieresis" }, /* LATIN SMALL LETTER U WITH DIAERESIS */
    { 0x00FD, "yacute" }, /* LATIN SMALL LETTER Y WITH ACUTE */
    { 0x00FE, "thorn" }, /* LATIN SMALL LETTER THORN */
    { 0x00FF, "ydieresis" }, /* LATIN SMALL LETTER Y WITH DIAERESIS */
    { 0x0100, "Amacron" }, /* LATIN CAPITAL LETTER A WITH MACRON */
    { 0x0101, "amacron" }, /* LATIN SMALL LETTER A WITH MACRON */
    { 0x0102, "Abreve" }, /* LATIN CAPITAL LETTER A WITH BREVE */
    { 0x0103, "abreve" }, /* LATIN SMALL LETTER A WITH BREVE */
    { 0x0104, "Aogonek" }, /* LATIN CAPITAL LETTER A WITH OGONEK */
    { 0x0105, "aogonek" }, /* LATIN SMALL LETTER A WITH OGONEK */
    { 0x0106, "Cacute" }, /* LATIN CAPITAL LETTER C WITH ACUTE */
    { 0x0107, "cacute" }, /* LATIN SMALL LETTER C WITH ACUTE */
    { 0x0108, "Ccircumflex" }, /* LATIN CAPITAL LETTER C WITH CIRCUMFLEX */
    { 0x0109, "ccircumflex" }, /* LATIN SMALL LETTER C WITH CIRCUMFLEX */
    { 0x010A, "Cdotaccent" }, /* LATIN CAPITAL LETTER C WITH DOT ABOVE */
    { 0x010B, "cdotaccent" }, /* LATIN SMALL LETTER C WITH DOT ABOVE */
    { 0x010C, "Ccaron" }, /* LATIN CAPITAL LETTER C WITH CARON */
    { 0x010D, "ccaron" }, /* LATIN SMALL LETTER C WITH CARON */
    { 0x010E, "Dcaron" }, /* LATIN CAPITAL LETTER D WITH CARON */
    { 0x010F, "dcaron" }, /* LATIN SMALL LETTER D WITH CARON */
    { 0x0110, "Dcroat" }, /* LATIN CAPITAL LETTER D WITH STROKE */
    { 0x0111, "dcroat" }, /* LATIN SMALL LETTER D WITH STROKE */
    { 0x0112, "Emacron" }, /* LATIN CAPITAL LETTER E WITH MACRON */
    { 0x0113, "emacron" }, /* LATIN SMALL LETTER E WITH MACRON */
    { 0x0114, "Ebreve" }, /* LATIN CAPITAL LETTER E WITH BREVE */
    { 0x0115, "ebreve" }, /* LATIN SMALL LETTER E WITH BREVE */
    { 0x0116, "Edotaccent" }, /* LATIN CAPITAL LETTER E WITH DOT ABOVE */
    { 0x0117, "edotaccent" }, /* LATIN SMALL LETTER E WITH DOT ABOVE */
    { 0x0118, "Eogonek" }, /* LATIN CAPITAL LETTER E WITH OGONEK */
    { 0x0119, "eogonek" }, /* LATIN SMALL LETTER E WITH OGONEK */
    { 0x011A, "Ecaron" }, /* LATIN CAPITAL LETTER E WITH CARON */
    { 0x011B, "ecaron" }, /* LATIN SMALL LETTER E WITH CARON */
    { 0x011C, "Gcircumflex" }, /* LATIN CAPITAL LETTER G WITH CIRCUMFLEX */
    { 0x011D, "gcircumflex" }, /* LATIN SMALL LETTER G WITH CIRCUMFLEX */
    { 0x011E, "Gbreve" }, /* LATIN CAPITAL LETTER G WITH BREVE */
    { 0x011F, "gbreve" }, /* LATIN SMALL LETTER G WITH BREVE */
    { 0x0120, "Gdotaccent" }, /* LATIN CAPITAL LETTER G WITH DOT ABOVE */
    { 0x0121, "gdotaccent" }, /* LATIN SMALL LETTER G WITH DOT ABOVE */
    { 0x0122, "Gcommaaccent" }, /* LATIN CAPITAL LETTER G WITH CEDILLA */
    { 0x0123, "gcommaaccent" }, /* LATIN SMALL LETTER G WITH CEDILLA */
    { 0x0124, "Hcircumflex" }, /* LATIN CAPITAL LETTER H WITH CIRCUMFLEX */
    { 0x0125, "hcircumflex" }, /* LATIN SMALL LETTER H WITH CIRCUMFLEX */
    { 0x0126, "Hbar" }, /* LATIN CAPITAL LETTER H WITH STROKE */
    { 0x0127, "hbar" }, /* LATIN SMALL LETTER H WITH STROKE */
    { 0x0128, "Itilde" }, /* LATIN CAPITAL LETTER I WITH TILDE */
    { 0x0129, "itilde" }, /* LATIN SMALL LETTER I WITH TILDE */
    { 0x012A, "Imacron" }, /* LATIN CAPITAL LETTER I WITH MACRON */
    { 0x012B, "imacron" }, /* LATIN SMALL LETTER I WITH MACRON */
    { 0x012C, "Ibreve" }, /* LATIN CAPITAL LETTER I WITH BREVE */
    { 0x012D, "ibreve" }, /* LATIN SMALL LETTER I WITH BREVE */
    { 0x012E, "Iogonek" }, /* LATIN CAPITAL LETTER I WITH OGONEK */
    { 0x012F, "iogonek" }, /* LATIN SMALL LETTER I WITH OGONEK */
    { 0x0130, "Idotaccent" }, /* LATIN CAPITAL LETTER I WITH DOT ABOVE */
    { 0x0131, "dotlessi" }, /* LATIN SMALL LETTER DOTLESS I */
    { 0x0132, "IJ" }, /* LATIN CAPITAL LIGATURE IJ */
    { 0x0133, "ij" }, /* LATIN SMALL LIGATURE IJ */
    { 0x0134, "Jcircumflex" }, /* LATIN CAPITAL LETTER J WITH CIRCUMFLEX */
    { 0x0135, "jcircumflex" }, /* LATIN SMALL LETTER J WITH CIRCUMFLEX */
    { 0x0136, "Kcommaaccent" }, /* LATIN CAPITAL LETTER K WITH CEDILLA */
    { 0x0137, "kcommaaccent" }, /* LATIN SMALL LETTER K WITH CEDILLA */
    { 0x0138, "kgreenlandic" }, /* LATIN SMALL LETTER KRA */
    { 0x0139, "Lacute" }, /* LATIN CAPITAL LETTER L WITH ACUTE */
    { 0x013A, "lacute" }, /* LATIN SMALL LETTER L WITH ACUTE */
    { 0x013B, "Lcommaaccent" }, /* LATIN CAPITAL LETTER L WITH CEDILLA */
    { 0x013C, "lcommaaccent" }, /* LATIN SMALL LETTER L WITH CEDILLA */
    { 0x013D, "Lcaron" }, /* LATIN CAPITAL LETTER L WITH CARON */
    { 0x013E, "lcaron" }, /* LATIN SMALL LETTER L WITH CARON */
    { 0x013F, "Ldot" }, /* LATIN CAPITAL LETTER L WITH MIDDLE DOT */
    { 0x0140, "ldot" }, /* LATIN SMALL LETTER L WITH MIDDLE DOT */
    { 0x0141, "Lslash" }, /* LATIN CAPITAL LETTER L WITH STROKE */
    { 0x0142, "lslash" }, /* LATIN SMALL LETTER L WITH STROKE */
    { 0x0143, "Nacute" }, /* LATIN CAPITAL LETTER N WITH ACUTE */
    { 0x0144, "nacute" }, /* LATIN SMALL LETTER N WITH ACUTE */
    { 0x0145, "Ncommaaccent" }, /* LATIN CAPITAL LETTER N WITH CEDILLA */
    { 0x0146, "ncommaaccent" }, /* LATIN SMALL LETTER N WITH CEDILLA */
    { 0x0147, "Ncaron" }, /* LATIN CAPITAL LETTER N WITH CARON */
    { 0x0148, "ncaron" }, /* LATIN SMALL LETTER N WITH CARON */
    { 0x0149, "napostrophe" }, /* LATIN SMALL LETTER N PRECEDED BY APOSTROPHE */
    { 0x014A, "Eng" }, /* LATIN CAPITAL LETTER ENG */
    { 0x014B, "eng" }, /* LATIN SMALL LETTER ENG */
    { 0x014C, "Omacron" }, /* LATIN CAPITAL LETTER O WITH MACRON */
    { 0x014D, "omacron" }, /* LATIN SMALL LETTER O WITH MACRON */
    { 0x014E, "Obreve" }, /* LATIN CAPITAL LETTER O WITH BREVE */
    { 0x014F, "obreve" }, /* LATIN SMALL LETTER O WITH BREVE */
    { 0x0150, "Ohungarumlaut" }, /* LATIN CAPITAL LETTER O WITH DOUBLE ACUTE */
    { 0x0151, "ohungarumlaut" }, /* LATIN SMALL LETTER O WITH DOUBLE ACUTE */
    { 0x0152, "OE" }, /* LATIN CAPITAL LIGATURE OE */
    { 0x0153, "oe" }, /* LATIN SMALL LIGATURE OE */
    { 0x0154, "Racute" }, /* LATIN CAPITAL LETTER R WITH ACUTE */
    { 0x0155, "racute" }, /* LATIN SMALL LETTER R WITH ACUTE */
    { 0x0156, "Rcommaaccent" }, /* LATIN CAPITAL LETTER R WITH CEDILLA */
    { 0x0157, "rcommaaccent" }, /* LATIN SMALL LETTER R WITH CEDILLA */
    { 0x0158, "Rcaron" }, /* LATIN CAPITAL LETTER R WITH CARON */
    { 0x0159, "rcaron" }, /* LATIN SMALL LETTER R WITH CARON */
    { 0x015A, "Sacute" }, /* LATIN CAPITAL LETTER S WITH ACUTE */
    { 0x015B, "sacute" }, /* LATIN SMALL LETTER S WITH ACUTE */
    { 0x015C, "Scircumflex" }, /* LATIN CAPITAL LETTER S WITH CIRCUMFLEX */
    { 0x015D, "scircumflex" }, /* LATIN SMALL LETTER S WITH CIRCUMFLEX */
    { 0x015E, "Scedilla" }, /* LATIN CAPITAL LETTER S WITH CEDILLA */
    { 0x015E, "$Scedilla"  }, /* LATIN CAPITAL LETTER S WITH CEDILLA; distinguish Adobe duplicates */
    { 0x015F, "scedilla" }, /* LATIN SMALL LETTER S WITH CEDILLA */
    { 0x015F, "$scedilla"  }, /* LATIN SMALL LETTER S WITH CEDILLA; distinguish Adobe duplicates */
    { 0x0160, "Scaron" }, /* LATIN CAPITAL LETTER S WITH CARON */
    { 0x0161, "scaron" }, /* LATIN SMALL LETTER S WITH CARON */
    { 0x0162, "$Tcedilla" }, /* LATIN CAPITAL LETTER T WITH CEDILLA; distinguish Adobe duplicates */
    { 0x0162, "Tcommaaccent" }, /* LATIN CAPITAL LETTER T WITH CEDILLA */
    { 0x0163, "$tcedilla" }, /* LATIN SMALL LETTER T WITH CEDILLA; distinguish Adobe duplicates */
    { 0x0163, "tcommaaccent" }, /* LATIN SMALL LETTER T WITH CEDILLA */
    { 0x0164, "Tcaron" }, /* LATIN CAPITAL LETTER T WITH CARON */
    { 0x0165, "tcaron" }, /* LATIN SMALL LETTER T WITH CARON */
    { 0x0166, "Tbar" }, /* LATIN CAPITAL LETTER T WITH STROKE */
    { 0x0167, "tbar" }, /* LATIN SMALL LETTER T WITH STROKE */
    { 0x0168, "Utilde" }, /* LATIN CAPITAL LETTER U WITH TILDE */
    { 0x0169, "utilde" }, /* LATIN SMALL LETTER U WITH TILDE */
    { 0x016A, "Umacron" }, /* LATIN CAPITAL LETTER U WITH MACRON */
    { 0x016B, "umacron" }, /* LATIN SMALL LETTER U WITH MACRON */
    { 0x016C, "Ubreve" }, /* LATIN CAPITAL LETTER U WITH BREVE */
    { 0x016D, "ubreve" }, /* LATIN SMALL LETTER U WITH BREVE */
    { 0x016E, "Uring" }, /* LATIN CAPITAL LETTER U WITH RING ABOVE */
    { 0x016F, "uring" }, /* LATIN SMALL LETTER U WITH RING ABOVE */
    { 0x0170, "Uhungarumlaut" }, /* LATIN CAPITAL LETTER U WITH DOUBLE ACUTE */
    { 0x0171, "uhungarumlaut" }, /* LATIN SMALL LETTER U WITH DOUBLE ACUTE */
    { 0x0172, "Uogonek" }, /* LATIN CAPITAL LETTER U WITH OGONEK */
    { 0x0173, "uogonek" }, /* LATIN SMALL LETTER U WITH OGONEK */
    { 0x0174, "Wcircumflex" }, /* LATIN CAPITAL LETTER W WITH CIRCUMFLEX */
    { 0x0175, "wcircumflex" }, /* LATIN SMALL LETTER W WITH CIRCUMFLEX */
    { 0x0176, "Ycircumflex" }, /* LATIN CAPITAL LETTER Y WITH CIRCUMFLEX */
    { 0x0177, "ycircumflex" }, /* LATIN SMALL LETTER Y WITH CIRCUMFLEX */
    { 0x0178, "Ydieresis" }, /* LATIN CAPITAL LETTER Y WITH DIAERESIS */
    { 0x0179, "Zacute" }, /* LATIN CAPITAL LETTER Z WITH ACUTE */
    { 0x017A, "zacute" }, /* LATIN SMALL LETTER Z WITH ACUTE */
    { 0x017B, "Zdotaccent" }, /* LATIN CAPITAL LETTER Z WITH DOT ABOVE */
    { 0x017C, "zdotaccent" }, /* LATIN SMALL LETTER Z WITH DOT ABOVE */
    { 0x017D, "Zcaron" }, /* LATIN CAPITAL LETTER Z WITH CARON */
    { 0x017E, "zcaron" }, /* LATIN SMALL LETTER Z WITH CARON */
    { 0x017F, "longs" }, /* LATIN SMALL LETTER LONG S */
    { 0x0192, "florin" }, /* LATIN SMALL LETTER F WITH HOOK */
    { 0x01A0, "Ohorn" }, /* LATIN CAPITAL LETTER O WITH HORN */
    { 0x01A1, "ohorn" }, /* LATIN SMALL LETTER O WITH HORN */
    { 0x01AF, "Uhorn" }, /* LATIN CAPITAL LETTER U WITH HORN */
    { 0x01B0, "uhorn" }, /* LATIN SMALL LETTER U WITH HORN */
    { 0x01E6, "Gcaron" }, /* LATIN CAPITAL LETTER G WITH CARON */
    { 0x01E7, "gcaron" }, /* LATIN SMALL LETTER G WITH CARON */
    { 0x01FA, "Aringacute" }, /* LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE */
    { 0x01FB, "aringacute" }, /* LATIN SMALL LETTER A WITH RING ABOVE AND ACUTE */
    { 0x01FC, "AEacute" }, /* LATIN CAPITAL LETTER AE WITH ACUTE */
    { 0x01FD, "aeacute" }, /* LATIN SMALL LETTER AE WITH ACUTE */
    { 0x01FE, "Oslashacute" }, /* LATIN CAPITAL LETTER O WITH STROKE AND ACUTE */
    { 0x01FF, "oslashacute" }, /* LATIN SMALL LETTER O WITH STROKE AND ACUTE */
    { 0x0218, "Scommaaccent" }, /* LATIN CAPITAL LETTER S WITH COMMA BELOW */
    { 0x0219, "scommaaccent" }, /* LATIN SMALL LETTER S WITH COMMA BELOW */
    { 0x021A, "$Tcommaaccent" }, /* LATIN CAPITAL LETTER T WITH COMMA BELOW; distinguish Adobe duplicates */
    { 0x021B, "$tcommaaccent" }, /* LATIN SMALL LETTER T WITH COMMA BELOW; distinguish Adobe duplicates */
    { 0x02BC, "afii57929" }, /* MODIFIER LETTER APOSTROPHE */
    { 0x02BD, "afii64937" }, /* MODIFIER LETTER REVERSED COMMA */
    { 0x02C6, "circumflex" }, /* MODIFIER LETTER CIRCUMFLEX ACCENT */
    { 0x02C7, "caron" }, /* CARON */
    { 0x02C9, "$macronmodifier" }, /* MODIFIER LETTER MACRON */
    { 0x02CA, "$acutemodifier" }, /* MODIFIER LETTER ACUTE ACCENT */
    { 0x02CB, "$gravemodifier" }, /* MODIFIER LETTER GRAVE ACCENT */
    { 0x02D8, "breve" }, /* BREVE */
    { 0x02D9, "dotaccent" }, /* DOT ABOVE */
    { 0x02DA, "ring" }, /* RING ABOVE */
    { 0x02DB, "ogonek" }, /* OGONEK */
    { 0x02DC, "tilde" }, /* SMALL TILDE */
    { 0x02DD, "hungarumlaut" }, /* DOUBLE ACUTE ACCENT */
    { 0x0300, "gravecomb" }, /* COMBINING GRAVE ACCENT */
    { 0x0301, "acutecomb" }, /* COMBINING ACUTE ACCENT */
    { 0x0302, "$circumflexcomb" }, /* COMBINING CIRCUMFLEX ACCENT */
    { 0x0303, "tildecomb" }, /* COMBINING TILDE */
    { 0x0304, "$macroncomb" }, /* COMBINING MACRON */
    { 0x0305, "$overlinecomb" }, /* COMBINING OVERLINE */
    { 0x0306, "$brevecomb" }, /* COMBINING BREVE */
    { 0x0307, "$dotaccentcomb" }, /* COMBINING DOT ABOVE */
    { 0x0308, "$dieresiscomb" }, /* COMBINING DIAERESIS */
    { 0x0309, "hookabovecomb" }, /* COMBINING HOOK ABOVE */
    { 0x030A, "$ringcomb" }, /* COMBINING RING ABOVE */
    { 0x030C, "$caroncomb" }, /* COMBINING CARON */
    { 0x0323, "dotbelowcomb" }, /* COMBINING DOT BELOW */
    { 0x0327, "$cedillacomb" }, /* COMBINING CEDILLA */
    { 0x0328, "$ogonekcomb" }, /* COMBINING OGONEK */
    { 0x0338, "$slashlongcomb" }, /* COMBINING LONG SOLIDUS OVERLAY */
    { 0x0384, "tonos" }, /* GREEK TONOS */
    { 0x0385, "dieresistonos" }, /* GREEK DIALYTIKA TONOS */
    { 0x0386, "Alphatonos" }, /* GREEK CAPITAL LETTER ALPHA WITH TONOS */
    { 0x0387, "anoteleia" }, /* GREEK ANO TELEIA */
    { 0x0388, "Epsilontonos" }, /* GREEK CAPITAL LETTER EPSILON WITH TONOS */
    { 0x0389, "Etatonos" }, /* GREEK CAPITAL LETTER ETA WITH TONOS */
    { 0x038A, "Iotatonos" }, /* GREEK CAPITAL LETTER IOTA WITH TONOS */
    { 0x038C, "Omicrontonos" }, /* GREEK CAPITAL LETTER OMICRON WITH TONOS */
    { 0x038E, "Upsilontonos" }, /* GREEK CAPITAL LETTER UPSILON WITH TONOS */
    { 0x038F, "Omegatonos" }, /* GREEK CAPITAL LETTER OMEGA WITH TONOS */
    { 0x0390, "iotadieresistonos" }, /* GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS */
    { 0x0391, "Alpha" }, /* GREEK CAPITAL LETTER ALPHA */
    { 0x0392, "Beta" }, /* GREEK CAPITAL LETTER BETA */
    { 0x0393, "Gamma" }, /* GREEK CAPITAL LETTER GAMMA */
    { 0x0394, "$Delta"  }, /* GREEK CAPITAL LETTER DELTA; distinguish Adobe duplicates */
    { 0x0395, "Epsilon" }, /* GREEK CAPITAL LETTER EPSILON */
    { 0x0396, "Zeta" }, /* GREEK CAPITAL LETTER ZETA */
    { 0x0397, "Eta" }, /* GREEK CAPITAL LETTER ETA */
    { 0x0398, "Theta" }, /* GREEK CAPITAL LETTER THETA */
    { 0x0399, "Iota" }, /* GREEK CAPITAL LETTER IOTA */
    { 0x039A, "Kappa" }, /* GREEK CAPITAL LETTER KAPPA */
    { 0x039B, "Lambda" }, /* GREEK CAPITAL LETTER LAMDA */
    { 0x039C, "Mu" }, /* GREEK CAPITAL LETTER MU */
    { 0x039D, "Nu" }, /* GREEK CAPITAL LETTER NU */
    { 0x039E, "Xi" }, /* GREEK CAPITAL LETTER XI */
    { 0x039F, "Omicron" }, /* GREEK CAPITAL LETTER OMICRON */
    { 0x03A0, "Pi" }, /* GREEK CAPITAL LETTER PI */
    { 0x03A1, "Rho" }, /* GREEK CAPITAL LETTER RHO */
    { 0x03A3, "Sigma" }, /* GREEK CAPITAL LETTER SIGMA */
    { 0x03A4, "Tau" }, /* GREEK CAPITAL LETTER TAU */
    { 0x03A5, "Upsilon" }, /* GREEK CAPITAL LETTER UPSILON */
    { 0x03A6, "Phi" }, /* GREEK CAPITAL LETTER PHI */
    { 0x03A7, "Chi" }, /* GREEK CAPITAL LETTER CHI */
    { 0x03A8, "Psi" }, /* GREEK CAPITAL LETTER PSI */
    { 0x03A9, "$Omega"  }, /* GREEK CAPITAL LETTER OMEGA; distinguish Adobe duplicates */
    { 0x03AA, "Iotadieresis" }, /* GREEK CAPITAL LETTER IOTA WITH DIALYTIKA */
    { 0x03AB, "Upsilondieresis" }, /* GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA */
    { 0x03AC, "alphatonos" }, /* GREEK SMALL LETTER ALPHA WITH TONOS */
    { 0x03AD, "epsilontonos" }, /* GREEK SMALL LETTER EPSILON WITH TONOS */
    { 0x03AE, "etatonos" }, /* GREEK SMALL LETTER ETA WITH TONOS */
    { 0x03AF, "iotatonos" }, /* GREEK SMALL LETTER IOTA WITH TONOS */
    { 0x03B0, "upsilondieresistonos" }, /* GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS */
    { 0x03B1, "alpha" }, /* GREEK SMALL LETTER ALPHA */
    { 0x03B2, "beta" }, /* GREEK SMALL LETTER BETA */
    { 0x03B3, "gamma" }, /* GREEK SMALL LETTER GAMMA */
    { 0x03B4, "delta" }, /* GREEK SMALL LETTER DELTA */
    { 0x03B5, "epsilon" }, /* GREEK SMALL LETTER EPSILON */
    { 0x03B6, "zeta" }, /* GREEK SMALL LETTER ZETA */
    { 0x03B7, "eta" }, /* GREEK SMALL LETTER ETA */
    { 0x03B8, "theta" }, /* GREEK SMALL LETTER THETA */
    { 0x03B9, "iota" }, /* GREEK SMALL LETTER IOTA */
    { 0x03BA, "kappa" }, /* GREEK SMALL LETTER KAPPA */
    { 0x03BB, "lambda" }, /* GREEK SMALL LETTER LAMDA */
    { 0x03BC, "$mu" }, /* GREEK SMALL LETTER MU; distinguish Adobe duplicates */
    { 0x03BD, "nu" }, /* GREEK SMALL LETTER NU */
    { 0x03BE, "xi" }, /* GREEK SMALL LETTER XI */
    { 0x03BF, "omicron" }, /* GREEK SMALL LETTER OMICRON */
    { 0x03C0, "pi" }, /* GREEK SMALL LETTER PI */
    { 0x03C1, "rho" }, /* GREEK SMALL LETTER RHO */
    { 0x03C2, "sigma1" }, /* GREEK SMALL LETTER FINAL SIGMA */
    { 0x03C3, "sigma" }, /* GREEK SMALL LETTER SIGMA */
    { 0x03C4, "tau" }, /* GREEK SMALL LETTER TAU */
    { 0x03C5, "upsilon" }, /* GREEK SMALL LETTER UPSILON */
    { 0x03C6, "phi" }, /* GREEK SMALL LETTER PHI */
    { 0x03C7, "chi" }, /* GREEK SMALL LETTER CHI */
    { 0x03C8, "psi" }, /* GREEK SMALL LETTER PSI */
    { 0x03C9, "omega" }, /* GREEK SMALL LETTER OMEGA */
    { 0x03CA, "iotadieresis" }, /* GREEK SMALL LETTER IOTA WITH DIALYTIKA */
    { 0x03CB, "upsilondieresis" }, /* GREEK SMALL LETTER UPSILON WITH DIALYTIKA */
    { 0x03CC, "omicrontonos" }, /* GREEK SMALL LETTER OMICRON WITH TONOS */
    { 0x03CD, "upsilontonos" }, /* GREEK SMALL LETTER UPSILON WITH TONOS */
    { 0x03CE, "omegatonos" }, /* GREEK SMALL LETTER OMEGA WITH TONOS */
    { 0x03D1, "theta1" }, /* GREEK THETA SYMBOL */
    { 0x03D2, "Upsilon1" }, /* GREEK UPSILON WITH HOOK SYMBOL */
    { 0x03D5, "phi1" }, /* GREEK PHI SYMBOL */
    { 0x03D6, "omega1" }, /* GREEK PI SYMBOL */
    { 0x03D6, "$pi1" }, /* GREEK PI SYMBOL; Adobe has this as "omega1" which is too confusing */
    { 0x03F1, "$rho1" }, /* GREEK RHO SYMBOL */
    { 0x03F5, "$epsilon1" }, /* GREEK LUNATE EPSILON SYMBOL */
    { 0x0401, "afii10023" }, /* CYRILLIC CAPITAL LETTER IO */
    { 0x0402, "afii10051" }, /* CYRILLIC CAPITAL LETTER DJE */
    { 0x0403, "afii10052" }, /* CYRILLIC CAPITAL LETTER GJE */
    { 0x0404, "afii10053" }, /* CYRILLIC CAPITAL LETTER UKRAINIAN IE */
    { 0x0405, "afii10054" }, /* CYRILLIC CAPITAL LETTER DZE */
    { 0x0406, "afii10055" }, /* CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I */
    { 0x0407, "afii10056" }, /* CYRILLIC CAPITAL LETTER YI */
    { 0x0408, "afii10057" }, /* CYRILLIC CAPITAL LETTER JE */
    { 0x0409, "afii10058" }, /* CYRILLIC CAPITAL LETTER LJE */
    { 0x040A, "afii10059" }, /* CYRILLIC CAPITAL LETTER NJE */
    { 0x040B, "afii10060" }, /* CYRILLIC CAPITAL LETTER TSHE */
    { 0x040C, "afii10061" }, /* CYRILLIC CAPITAL LETTER KJE */
    { 0x040E, "afii10062" }, /* CYRILLIC CAPITAL LETTER SHORT U */
    { 0x040F, "afii10145" }, /* CYRILLIC CAPITAL LETTER DZHE */
    { 0x0410, "afii10017" }, /* CYRILLIC CAPITAL LETTER A */
    { 0x0411, "afii10018" }, /* CYRILLIC CAPITAL LETTER BE */
    { 0x0412, "afii10019" }, /* CYRILLIC CAPITAL LETTER VE */
    { 0x0413, "afii10020" }, /* CYRILLIC CAPITAL LETTER GHE */
    { 0x0414, "afii10021" }, /* CYRILLIC CAPITAL LETTER DE */
    { 0x0415, "afii10022" }, /* CYRILLIC CAPITAL LETTER IE */
    { 0x0416, "afii10024" }, /* CYRILLIC CAPITAL LETTER ZHE */
    { 0x0417, "afii10025" }, /* CYRILLIC CAPITAL LETTER ZE */
    { 0x0418, "afii10026" }, /* CYRILLIC CAPITAL LETTER I */
    { 0x0419, "afii10027" }, /* CYRILLIC CAPITAL LETTER SHORT I */
    { 0x041A, "afii10028" }, /* CYRILLIC CAPITAL LETTER KA */
    { 0x041B, "afii10029" }, /* CYRILLIC CAPITAL LETTER EL */
    { 0x041C, "afii10030" }, /* CYRILLIC CAPITAL LETTER EM */
    { 0x041D, "afii10031" }, /* CYRILLIC CAPITAL LETTER EN */
    { 0x041E, "afii10032" }, /* CYRILLIC CAPITAL LETTER O */
    { 0x041F, "afii10033" }, /* CYRILLIC CAPITAL LETTER PE */
    { 0x0420, "afii10034" }, /* CYRILLIC CAPITAL LETTER ER */
    { 0x0421, "afii10035" }, /* CYRILLIC CAPITAL LETTER ES */
    { 0x0422, "afii10036" }, /* CYRILLIC CAPITAL LETTER TE */
    { 0x0423, "afii10037" }, /* CYRILLIC CAPITAL LETTER U */
    { 0x0424, "afii10038" }, /* CYRILLIC CAPITAL LETTER EF */
    { 0x0425, "afii10039" }, /* CYRILLIC CAPITAL LETTER HA */
    { 0x0426, "afii10040" }, /* CYRILLIC CAPITAL LETTER TSE */
    { 0x0427, "afii10041" }, /* CYRILLIC CAPITAL LETTER CHE */
    { 0x0428, "afii10042" }, /* CYRILLIC CAPITAL LETTER SHA */
    { 0x0429, "afii10043" }, /* CYRILLIC CAPITAL LETTER SHCHA */
    { 0x042A, "afii10044" }, /* CYRILLIC CAPITAL LETTER HARD SIGN */
    { 0x042B, "afii10045" }, /* CYRILLIC CAPITAL LETTER YERU */
    { 0x042C, "afii10046" }, /* CYRILLIC CAPITAL LETTER SOFT SIGN */
    { 0x042D, "afii10047" }, /* CYRILLIC CAPITAL LETTER E */
    { 0x042E, "afii10048" }, /* CYRILLIC CAPITAL LETTER YU */
    { 0x042F, "afii10049" }, /* CYRILLIC CAPITAL LETTER YA */
    { 0x0430, "afii10065" }, /* CYRILLIC SMALL LETTER A */
    { 0x0431, "afii10066" }, /* CYRILLIC SMALL LETTER BE */
    { 0x0432, "afii10067" }, /* CYRILLIC SMALL LETTER VE */
    { 0x0433, "afii10068" }, /* CYRILLIC SMALL LETTER GHE */
    { 0x0434, "afii10069" }, /* CYRILLIC SMALL LETTER DE */
    { 0x0435, "afii10070" }, /* CYRILLIC SMALL LETTER IE */
    { 0x0436, "afii10072" }, /* CYRILLIC SMALL LETTER ZHE */
    { 0x0437, "afii10073" }, /* CYRILLIC SMALL LETTER ZE */
    { 0x0438, "afii10074" }, /* CYRILLIC SMALL LETTER I */
    { 0x0439, "afii10075" }, /* CYRILLIC SMALL LETTER SHORT I */
    { 0x043A, "afii10076" }, /* CYRILLIC SMALL LETTER KA */
    { 0x043B, "afii10077" }, /* CYRILLIC SMALL LETTER EL */
    { 0x043C, "afii10078" }, /* CYRILLIC SMALL LETTER EM */
    { 0x043D, "afii10079" }, /* CYRILLIC SMALL LETTER EN */
    { 0x043E, "afii10080" }, /* CYRILLIC SMALL LETTER O */
    { 0x043F, "afii10081" }, /* CYRILLIC SMALL LETTER PE */
    { 0x0440, "afii10082" }, /* CYRILLIC SMALL LETTER ER */
    { 0x0441, "afii10083" }, /* CYRILLIC SMALL LETTER ES */
    { 0x0442, "afii10084" }, /* CYRILLIC SMALL LETTER TE */
    { 0x0443, "afii10085" }, /* CYRILLIC SMALL LETTER U */
    { 0x0444, "afii10086" }, /* CYRILLIC SMALL LETTER EF */
    { 0x0445, "afii10087" }, /* CYRILLIC SMALL LETTER HA */
    { 0x0446, "afii10088" }, /* CYRILLIC SMALL LETTER TSE */
    { 0x0447, "afii10089" }, /* CYRILLIC SMALL LETTER CHE */
    { 0x0448, "afii10090" }, /* CYRILLIC SMALL LETTER SHA */
    { 0x0449, "afii10091" }, /* CYRILLIC SMALL LETTER SHCHA */
    { 0x044A, "afii10092" }, /* CYRILLIC SMALL LETTER HARD SIGN */
    { 0x044B, "afii10093" }, /* CYRILLIC SMALL LETTER YERU */
    { 0x044C, "afii10094" }, /* CYRILLIC SMALL LETTER SOFT SIGN */
    { 0x044D, "afii10095" }, /* CYRILLIC SMALL LETTER E */
    { 0x044E, "afii10096" }, /* CYRILLIC SMALL LETTER YU */
    { 0x044F, "afii10097" }, /* CYRILLIC SMALL LETTER YA */
    { 0x0451, "afii10071" }, /* CYRILLIC SMALL LETTER IO */
    { 0x0452, "afii10099" }, /* CYRILLIC SMALL LETTER DJE */
    { 0x0453, "afii10100" }, /* CYRILLIC SMALL LETTER GJE */
    { 0x0454, "afii10101" }, /* CYRILLIC SMALL LETTER UKRAINIAN IE */
    { 0x0455, "afii10102" }, /* CYRILLIC SMALL LETTER DZE */
    { 0x0456, "afii10103" }, /* CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I */
    { 0x0457, "afii10104" }, /* CYRILLIC SMALL LETTER YI */
    { 0x0458, "afii10105" }, /* CYRILLIC SMALL LETTER JE */
    { 0x0459, "afii10106" }, /* CYRILLIC SMALL LETTER LJE */
    { 0x045A, "afii10107" }, /* CYRILLIC SMALL LETTER NJE */
    { 0x045B, "afii10108" }, /* CYRILLIC SMALL LETTER TSHE */
    { 0x045C, "afii10109" }, /* CYRILLIC SMALL LETTER KJE */
    { 0x045E, "afii10110" }, /* CYRILLIC SMALL LETTER SHORT U */
    { 0x045F, "afii10193" }, /* CYRILLIC SMALL LETTER DZHE */
    { 0x0462, "afii10146" }, /* CYRILLIC CAPITAL LETTER YAT */
    { 0x0463, "afii10194" }, /* CYRILLIC SMALL LETTER YAT */
    { 0x0472, "afii10147" }, /* CYRILLIC CAPITAL LETTER FITA */
    { 0x0473, "afii10195" }, /* CYRILLIC SMALL LETTER FITA */
    { 0x0474, "afii10148" }, /* CYRILLIC CAPITAL LETTER IZHITSA */
    { 0x0475, "afii10196" }, /* CYRILLIC SMALL LETTER IZHITSA */
    { 0x0490, "afii10050" }, /* CYRILLIC CAPITAL LETTER GHE WITH UPTURN */
    { 0x0491, "afii10098" }, /* CYRILLIC SMALL LETTER GHE WITH UPTURN */
    { 0x04D9, "afii10846" }, /* CYRILLIC SMALL LETTER SCHWA */
    { 0x05B0, "afii57799" }, /* HEBREW POINT SHEVA */
    { 0x05B1, "afii57801" }, /* HEBREW POINT HATAF SEGOL */
    { 0x05B2, "afii57800" }, /* HEBREW POINT HATAF PATAH */
    { 0x05B3, "afii57802" }, /* HEBREW POINT HATAF QAMATS */
    { 0x05B4, "afii57793" }, /* HEBREW POINT HIRIQ */
    { 0x05B5, "afii57794" }, /* HEBREW POINT TSERE */
    { 0x05B6, "afii57795" }, /* HEBREW POINT SEGOL */
    { 0x05B7, "afii57798" }, /* HEBREW POINT PATAH */
    { 0x05B8, "afii57797" }, /* HEBREW POINT QAMATS */
    { 0x05B9, "afii57806" }, /* HEBREW POINT HOLAM */
    { 0x05BB, "afii57796" }, /* HEBREW POINT QUBUTS */
    { 0x05BC, "afii57807" }, /* HEBREW POINT DAGESH OR MAPIQ */
    { 0x05BD, "afii57839" }, /* HEBREW POINT METEG */
    { 0x05BE, "afii57645" }, /* HEBREW PUNCTUATION MAQAF */
    { 0x05BF, "afii57841" }, /* HEBREW POINT RAFE */
    { 0x05C0, "afii57842" }, /* HEBREW PUNCTUATION PASEQ */
    { 0x05C1, "afii57804" }, /* HEBREW POINT SHIN DOT */
    { 0x05C2, "afii57803" }, /* HEBREW POINT SIN DOT */
    { 0x05C3, "afii57658" }, /* HEBREW PUNCTUATION SOF PASUQ */
    { 0x05D0, "afii57664" }, /* HEBREW LETTER ALEF */
    { 0x05D1, "afii57665" }, /* HEBREW LETTER BET */
    { 0x05D2, "afii57666" }, /* HEBREW LETTER GIMEL */
    { 0x05D3, "afii57667" }, /* HEBREW LETTER DALET */
    { 0x05D4, "afii57668" }, /* HEBREW LETTER HE */
    { 0x05D5, "afii57669" }, /* HEBREW LETTER VAV */
    { 0x05D6, "afii57670" }, /* HEBREW LETTER ZAYIN */
    { 0x05D7, "afii57671" }, /* HEBREW LETTER HET */
    { 0x05D8, "afii57672" }, /* HEBREW LETTER TET */
    { 0x05D9, "afii57673" }, /* HEBREW LETTER YOD */
    { 0x05DA, "afii57674" }, /* HEBREW LETTER FINAL KAF */
    { 0x05DB, "afii57675" }, /* HEBREW LETTER KAF */
    { 0x05DC, "afii57676" }, /* HEBREW LETTER LAMED */
    { 0x05DD, "afii57677" }, /* HEBREW LETTER FINAL MEM */
    { 0x05DE, "afii57678" }, /* HEBREW LETTER MEM */
    { 0x05DF, "afii57679" }, /* HEBREW LETTER FINAL NUN */
    { 0x05E0, "afii57680" }, /* HEBREW LETTER NUN */
    { 0x05E1, "afii57681" }, /* HEBREW LETTER SAMEKH */
    { 0x05E2, "afii57682" }, /* HEBREW LETTER AYIN */
    { 0x05E3, "afii57683" }, /* HEBREW LETTER FINAL PE */
    { 0x05E4, "afii57684" }, /* HEBREW LETTER PE */
    { 0x05E5, "afii57685" }, /* HEBREW LETTER FINAL TSADI */
    { 0x05E6, "afii57686" }, /* HEBREW LETTER TSADI */
    { 0x05E7, "afii57687" }, /* HEBREW LETTER QOF */
    { 0x05E8, "afii57688" }, /* HEBREW LETTER RESH */
    { 0x05E9, "afii57689" }, /* HEBREW LETTER SHIN */
    { 0x05EA, "afii57690" }, /* HEBREW LETTER TAV */
    { 0x05F0, "afii57716" }, /* HEBREW LIGATURE YIDDISH DOUBLE VAV */
    { 0x05F1, "afii57717" }, /* HEBREW LIGATURE YIDDISH VAV YOD */
    { 0x05F2, "afii57718" }, /* HEBREW LIGATURE YIDDISH DOUBLE YOD */
    { 0x060C, "afii57388" }, /* ARABIC COMMA */
    { 0x061B, "afii57403" }, /* ARABIC SEMICOLON */
    { 0x061F, "afii57407" }, /* ARABIC QUESTION MARK */
    { 0x0621, "afii57409" }, /* ARABIC LETTER HAMZA */
    { 0x0622, "afii57410" }, /* ARABIC LETTER ALEF WITH MADDA ABOVE */
    { 0x0623, "afii57411" }, /* ARABIC LETTER ALEF WITH HAMZA ABOVE */
    { 0x0624, "afii57412" }, /* ARABIC LETTER WAW WITH HAMZA ABOVE */
    { 0x0625, "afii57413" }, /* ARABIC LETTER ALEF WITH HAMZA BELOW */
    { 0x0626, "afii57414" }, /* ARABIC LETTER YEH WITH HAMZA ABOVE */
    { 0x0627, "afii57415" }, /* ARABIC LETTER ALEF */
    { 0x0628, "afii57416" }, /* ARABIC LETTER BEH */
    { 0x0629, "afii57417" }, /* ARABIC LETTER TEH MARBUTA */
    { 0x062A, "afii57418" }, /* ARABIC LETTER TEH */
    { 0x062B, "afii57419" }, /* ARABIC LETTER THEH */
    { 0x062C, "afii57420" }, /* ARABIC LETTER JEEM */
    { 0x062D, "afii57421" }, /* ARABIC LETTER HAH */
    { 0x062E, "afii57422" }, /* ARABIC LETTER KHAH */
    { 0x062F, "afii57423" }, /* ARABIC LETTER DAL */
    { 0x0630, "afii57424" }, /* ARABIC LETTER THAL */
    { 0x0631, "afii57425" }, /* ARABIC LETTER REH */
    { 0x0632, "afii57426" }, /* ARABIC LETTER ZAIN */
    { 0x0633, "afii57427" }, /* ARABIC LETTER SEEN */
    { 0x0634, "afii57428" }, /* ARABIC LETTER SHEEN */
    { 0x0635, "afii57429" }, /* ARABIC LETTER SAD */
    { 0x0636, "afii57430" }, /* ARABIC LETTER DAD */
    { 0x0637, "afii57431" }, /* ARABIC LETTER TAH */
    { 0x0638, "afii57432" }, /* ARABIC LETTER ZAH */
    { 0x0639, "afii57433" }, /* ARABIC LETTER AIN */
    { 0x063A, "afii57434" }, /* ARABIC LETTER GHAIN */
    { 0x0640, "afii57440" }, /* ARABIC TATWEEL */
    { 0x0641, "afii57441" }, /* ARABIC LETTER FEH */
    { 0x0642, "afii57442" }, /* ARABIC LETTER QAF */
    { 0x0643, "afii57443" }, /* ARABIC LETTER KAF */
    { 0x0644, "afii57444" }, /* ARABIC LETTER LAM */
    { 0x0645, "afii57445" }, /* ARABIC LETTER MEEM */
    { 0x0646, "afii57446" }, /* ARABIC LETTER NOON */
    { 0x0647, "afii57470" }, /* ARABIC LETTER HEH */
    { 0x0648, "afii57448" }, /* ARABIC LETTER WAW */
    { 0x0649, "afii57449" }, /* ARABIC LETTER ALEF MAKSURA */
    { 0x064A, "afii57450" }, /* ARABIC LETTER YEH */
    { 0x064B, "afii57451" }, /* ARABIC FATHATAN */
    { 0x064C, "afii57452" }, /* ARABIC DAMMATAN */
    { 0x064D, "afii57453" }, /* ARABIC KASRATAN */
    { 0x064E, "afii57454" }, /* ARABIC FATHA */
    { 0x064F, "afii57455" }, /* ARABIC DAMMA */
    { 0x0650, "afii57456" }, /* ARABIC KASRA */
    { 0x0651, "afii57457" }, /* ARABIC SHADDA */
    { 0x0652, "afii57458" }, /* ARABIC SUKUN */
    { 0x0660, "afii57392" }, /* ARABIC-INDIC DIGIT ZERO */
    { 0x0661, "afii57393" }, /* ARABIC-INDIC DIGIT ONE */
    { 0x0662, "afii57394" }, /* ARABIC-INDIC DIGIT TWO */
    { 0x0663, "afii57395" }, /* ARABIC-INDIC DIGIT THREE */
    { 0x0664, "afii57396" }, /* ARABIC-INDIC DIGIT FOUR */
    { 0x0665, "afii57397" }, /* ARABIC-INDIC DIGIT FIVE */
    { 0x0666, "afii57398" }, /* ARABIC-INDIC DIGIT SIX */
    { 0x0667, "afii57399" }, /* ARABIC-INDIC DIGIT SEVEN */
    { 0x0668, "afii57400" }, /* ARABIC-INDIC DIGIT EIGHT */
    { 0x0669, "afii57401" }, /* ARABIC-INDIC DIGIT NINE */
    { 0x066A, "afii57381" }, /* ARABIC PERCENT SIGN */
    { 0x066D, "afii63167" }, /* ARABIC FIVE POINTED STAR */
    { 0x0679, "afii57511" }, /* ARABIC LETTER TTEH */
    { 0x067E, "afii57506" }, /* ARABIC LETTER PEH */
    { 0x0686, "afii57507" }, /* ARABIC LETTER TCHEH */
    { 0x0688, "afii57512" }, /* ARABIC LETTER DDAL */
    { 0x0691, "afii57513" }, /* ARABIC LETTER RREH */
    { 0x0698, "afii57508" }, /* ARABIC LETTER JEH */
    { 0x06A4, "afii57505" }, /* ARABIC LETTER VEH */
    { 0x06AF, "afii57509" }, /* ARABIC LETTER GAF */
    { 0x06BA, "afii57514" }, /* ARABIC LETTER NOON GHUNNA */
    { 0x06D2, "afii57519" }, /* ARABIC LETTER YEH BARREE */
    { 0x06D5, "afii57534" }, /* ARABIC LETTER AE */
    { 0x1E80, "Wgrave" }, /* LATIN CAPITAL LETTER W WITH GRAVE */
    { 0x1E81, "wgrave" }, /* LATIN SMALL LETTER W WITH GRAVE */
    { 0x1E82, "Wacute" }, /* LATIN CAPITAL LETTER W WITH ACUTE */
    { 0x1E83, "wacute" }, /* LATIN SMALL LETTER W WITH ACUTE */
    { 0x1E84, "Wdieresis" }, /* LATIN CAPITAL LETTER W WITH DIAERESIS */
    { 0x1E85, "wdieresis" }, /* LATIN SMALL LETTER W WITH DIAERESIS */
    { 0x1EF2, "Ygrave" }, /* LATIN CAPITAL LETTER Y WITH GRAVE */
    { 0x1EF3, "ygrave" }, /* LATIN SMALL LETTER Y WITH GRAVE */
    { 0x200C, "afii61664" }, /* ZERO WIDTH NON-JOINER */
    { 0x200D, "afii301" }, /* ZERO WIDTH JOINER */
    { 0x200E, "afii299" }, /* LEFT-TO-RIGHT MARK */
    { 0x200F, "afii300" }, /* RIGHT-TO-LEFT MARK */
    { 0x2012, "figuredash" }, /* FIGURE DASH */
    { 0x2013, "endash" }, /* EN DASH */
    { 0x2014, "emdash" }, /* EM DASH */
    { 0x2015, "afii00208" }, /* HORIZONTAL BAR */
    { 0x2017, "underscoredbl" }, /* DOUBLE LOW LINE */
    { 0x2018, "quoteleft" }, /* LEFT SINGLE QUOTATION MARK */
    { 0x2019, "quoteright" }, /* RIGHT SINGLE QUOTATION MARK */
    { 0x201A, "quotesinglbase" }, /* SINGLE LOW-9 QUOTATION MARK */
    { 0x201B, "quotereversed" }, /* SINGLE HIGH-REVERSED-9 QUOTATION MARK */
    { 0x201C, "quotedblleft" }, /* LEFT DOUBLE QUOTATION MARK */
    { 0x201D, "quotedblright" }, /* RIGHT DOUBLE QUOTATION MARK */
    { 0x201E, "quotedblbase" }, /* DOUBLE LOW-9 QUOTATION MARK */
    { 0x201F, "$quotedblreversed" }, /* DOUBLE HIGH-REVERSED-9 QUOTATION MARK */
    { 0x2020, "dagger" }, /* DAGGER */
    { 0x2021, "daggerdbl" }, /* DOUBLE DAGGER */
    { 0x2022, "bullet" }, /* BULLET */
    { 0x2024, "onedotenleader" }, /* ONE DOT LEADER */
    { 0x2025, "twodotenleader" }, /* TWO DOT LEADER */
    { 0x2026, "ellipsis" }, /* HORIZONTAL ELLIPSIS */
    { 0x202C, "afii61573" }, /* POP DIRECTIONAL FORMATTING */
    { 0x202D, "afii61574" }, /* LEFT-TO-RIGHT OVERRIDE */
    { 0x202E, "afii61575" }, /* RIGHT-TO-LEFT OVERRIDE */
    { 0x2030, "perthousand" }, /* PER MILLE SIGN */
    { 0x2032, "minute" }, /* PRIME */
    { 0x2033, "second" }, /* DOUBLE PRIME */
    { 0x2039, "guilsinglleft" }, /* SINGLE LEFT-POINTING ANGLE QUOTATION MARK */
    { 0x203A, "guilsinglright" }, /* SINGLE RIGHT-POINTING ANGLE QUOTATION MARK */
    { 0x203C, "exclamdbl" }, /* DOUBLE EXCLAMATION MARK */
    { 0x2040, "$tie" }, /* CHARACTER TIE */
    { 0x2044, "fraction" }, /* FRACTION SLASH */
    { 0x2044, "$fractionslash" }, /* FRACTION SLASH; distinguish Adobe duplicates */
    { 0x2070, "zerosuperior" }, /* SUPERSCRIPT ZERO */
    { 0x2074, "foursuperior" }, /* SUPERSCRIPT FOUR */
    { 0x2075, "fivesuperior" }, /* SUPERSCRIPT FIVE */
    { 0x2076, "sixsuperior" }, /* SUPERSCRIPT SIX */
    { 0x2077, "sevensuperior" }, /* SUPERSCRIPT SEVEN */
    { 0x2078, "eightsuperior" }, /* SUPERSCRIPT EIGHT */
    { 0x2079, "ninesuperior" }, /* SUPERSCRIPT NINE */
    { 0x207D, "parenleftsuperior" }, /* SUPERSCRIPT LEFT PARENTHESIS */
    { 0x207E, "parenrightsuperior" }, /* SUPERSCRIPT RIGHT PARENTHESIS */
    { 0x207F, "nsuperior" }, /* SUPERSCRIPT LATIN SMALL LETTER N */
    { 0x2080, "zeroinferior" }, /* SUBSCRIPT ZERO */
    { 0x2081, "oneinferior" }, /* SUBSCRIPT ONE */
    { 0x2082, "twoinferior" }, /* SUBSCRIPT TWO */
    { 0x2083, "threeinferior" }, /* SUBSCRIPT THREE */
    { 0x2084, "fourinferior" }, /* SUBSCRIPT FOUR */
    { 0x2085, "fiveinferior" }, /* SUBSCRIPT FIVE */
    { 0x2086, "sixinferior" }, /* SUBSCRIPT SIX */
    { 0x2087, "seveninferior" }, /* SUBSCRIPT SEVEN */
    { 0x2088, "eightinferior" }, /* SUBSCRIPT EIGHT */
    { 0x2089, "nineinferior" }, /* SUBSCRIPT NINE */
    { 0x208D, "parenleftinferior" }, /* SUBSCRIPT LEFT PARENTHESIS */
    { 0x208E, "parenrightinferior" }, /* SUBSCRIPT RIGHT PARENTHESIS */
    { 0x20A1, "colonmonetary" }, /* COLON SIGN */
    { 0x20A3, "franc" }, /* FRENCH FRANC SIGN */
    { 0x20A4, "lira" }, /* LIRA SIGN */
    { 0x20A7, "peseta" }, /* PESETA SIGN */
    { 0x20AA, "afii57636" }, /* NEW SHEQEL SIGN */
    { 0x20AB, "dong" }, /* DONG SIGN */
    { 0x20AC, "Euro" }, /* EURO SIGN */
    { 0x20D7, "$vectorcomb" }, /* COMBINING RIGHT ARROW ABOVE */
    { 0x20DD, "$circlecomb" }, /* COMBINING ENCLOSING CIRCLE */
    { 0x2105, "afii61248" }, /* CARE OF */
    { 0x2111, "Ifraktur" }, /* BLACK-LETTER CAPITAL I */
    { 0x2113, "afii61289" }, /* SCRIPT SMALL L */
    { 0x2113, "$lscript" }, /* SCRIPT SMALL L; Adobe has this as "afii61289" */
    { 0x2116, "afii61352" }, /* NUMERO SIGN */
    { 0x2118, "weierstrass" }, /* SCRIPT CAPITAL P */
    { 0x211C, "Rfraktur" }, /* BLACK-LETTER CAPITAL R */
    { 0x211E, "prescription" }, /* PRESCRIPTION TAKE */
    { 0x2122, "trademark" }, /* TRADE MARK SIGN */
    { 0x2126, "$Ohm"  }, /* OHM SIGN; distinguish Adobe duplicates */
    { 0x2126, "Omega" }, /* OHM SIGN */
    { 0x212E, "estimated" }, /* ESTIMATED SYMBOL */
    { 0x2135, "aleph" }, /* ALEF SYMBOL */
    { 0x2153, "onethird" }, /* VULGAR FRACTION ONE THIRD */
    { 0x2154, "twothirds" }, /* VULGAR FRACTION TWO THIRDS */
    { 0x215B, "oneeighth" }, /* VULGAR FRACTION ONE EIGHTH */
    { 0x215C, "threeeighths" }, /* VULGAR FRACTION THREE EIGHTHS */
    { 0x215D, "fiveeighths" }, /* VULGAR FRACTION FIVE EIGHTHS */
    { 0x215E, "seveneighths" }, /* VULGAR FRACTION SEVEN EIGHTHS */
    { 0x2190, "arrowleft" }, /* LEFTWARDS ARROW */
    { 0x2191, "arrowup" }, /* UPWARDS ARROW */
    { 0x2192, "arrowright" }, /* RIGHTWARDS ARROW */
    { 0x2193, "arrowdown" }, /* DOWNWARDS ARROW */
    { 0x2194, "arrowboth" }, /* LEFT RIGHT ARROW */
    { 0x2195, "arrowupdn" }, /* UP DOWN ARROW */
    { 0x2196, "$arrownorthwest" }, /* NORTH WEST ARROW */
    { 0x2197, "$arrownortheast" }, /* NORTH EAST ARROW */
    { 0x2198, "$arrowsoutheast" }, /* SOUTH EAST ARROW */
    { 0x2199, "$arrowsouthwest" }, /* SOUTH WEST ARROW */
    { 0x21A8, "arrowupdnbse" }, /* UP DOWN ARROW WITH BASE */
    { 0x21B5, "carriagereturn" }, /* DOWNWARDS ARROW WITH CORNER LEFTWARDS */
    { 0x21BC, "$harpoonleftbarbup" }, /* LEFTWARDS HARPOON WITH BARB UPWARDS */
    { 0x21BD, "$harpoonleftbarbdown" }, /* LEFTWARDS HARPOON WITH BARB DOWNWARDS */
    { 0x21C0, "$harpoonrightbarbup" }, /* RIGHTWARDS HARPOON WITH BARB UPWARDS */
    { 0x21C1, "$harpoonrightbarbdown" }, /* RIGHTWARDS HARPOON WITH BARB DOWNWARDS */
    { 0x21D0, "arrowdblleft" }, /* LEFTWARDS DOUBLE ARROW */
    { 0x21D1, "arrowdblup" }, /* UPWARDS DOUBLE ARROW */
    { 0x21D2, "arrowdblright" }, /* RIGHTWARDS DOUBLE ARROW */
    { 0x21D3, "arrowdbldown" }, /* DOWNWARDS DOUBLE ARROW */
    { 0x21D4, "arrowdblboth" }, /* LEFT RIGHT DOUBLE ARROW */
    { 0x21D5, "$arrowdblupdn" }, /* UP DOWN DOUBLE ARROW */
    { 0x2200, "universal" }, /* FOR ALL */
    { 0x2202, "partialdiff" }, /* PARTIAL DIFFERENTIAL */
    { 0x2203, "existential" }, /* THERE EXISTS */
    { 0x2205, "emptyset" }, /* EMPTY SET */
    { 0x2206, "Delta" }, /* INCREMENT */
    { 0x2206, "$increment"  }, /* INCREMENT; distinguish Adobe duplicates */
    { 0x2207, "gradient" }, /* NABLA */
    { 0x2208, "element" }, /* ELEMENT OF */
    { 0x2209, "notelement" }, /* NOT AN ELEMENT OF */
    { 0x220B, "suchthat" }, /* CONTAINS AS MEMBER */
    { 0x220F, "product" }, /* N-ARY PRODUCT */
    { 0x2210, "$coproduct" }, /* N-ARY COPRODUCT */
    { 0x2211, "summation" }, /* N-ARY SUMMATION */
    { 0x2212, "minus" }, /* MINUS SIGN */
    { 0x2213, "$minusplus" }, /* MINUS-OR-PLUS SIGN */
    { 0x2215, "$divisionslash" }, /* DIVISION SLASH; distinguish Adobe duplicates */
    { 0x2217, "asteriskmath" }, /* ASTERISK OPERATOR */
    { 0x2219, "$bulletmath"  }, /* BULLET OPERATOR; distinguish Adobe duplicates */
    { 0x221A, "radical" }, /* SQUARE ROOT */
    { 0x221D, "proportional" }, /* PROPORTIONAL TO */
    { 0x221E, "infinity" }, /* INFINITY */
    { 0x221F, "orthogonal" }, /* RIGHT ANGLE */
    { 0x2220, "angle" }, /* ANGLE */
    { 0x2225, "$parallel" }, /* PARALLEL TO */
    { 0x2227, "logicaland" }, /* LOGICAL AND */
    { 0x2228, "logicalor" }, /* LOGICAL OR */
    { 0x2229, "intersection" }, /* INTERSECTION */
    { 0x222A, "union" }, /* UNION */
    { 0x222B, "integral" }, /* INTEGRAL */
    { 0x222E, "$contintegral" }, /* CONTOUR INTEGRAL */
    { 0x2234, "therefore" }, /* THEREFORE */
    { 0x223C, "similar" }, /* TILDE OPERATOR */
    { 0x2240, "$wreathproduct" }, /* WREATH PRODUCT */
    { 0x2243, "$similarequal" }, /* ASYMPTOTICALLY EQUAL TO */
    { 0x2245, "congruent" }, /* APPROXIMATELY EQUAL TO */
    { 0x2248, "approxequal" }, /* ALMOST EQUAL TO */
    { 0x224D, "$equivasymptotic" }, /* EQUIVALENT TO */
    { 0x2260, "notequal" }, /* NOT EQUAL TO */
    { 0x2261, "equivalence" }, /* IDENTICAL TO */
    { 0x2264, "lessequal" }, /* LESS-THAN OR EQUAL TO */
    { 0x2265, "greaterequal" }, /* GREATER-THAN OR EQUAL TO */
    { 0x226A, "$lessmuch" }, /* MUCH-LESS THAN */
    { 0x226B, "$greatermuch" }, /* MUCH GREATER-THAN */
    { 0x227A, "$precedes" }, /* PRECEDES */
    { 0x227B, "$follows" }, /* SUCCEEDS */
    { 0x227C, "$precedesequal" }, /* PRECEDES OR EQUAL TO */
    { 0x227D, "$followsequal" }, /* SUCCEEDS OR EQUAL TO */
    { 0x2282, "propersubset" }, /* SUBSET OF */
    { 0x2283, "propersuperset" }, /* SUPERSET OF */
    { 0x2284, "notsubset" }, /* NOT A SUBSET OF */
    { 0x2286, "reflexsubset" }, /* SUBSET OF OR EQUAL TO */
    { 0x2287, "reflexsuperset" }, /* SUPERSET OF OR EQUAL TO */
    { 0x228E, "$unionmulti" }, /* MULTISET UNION */
    { 0x2291, "$reflexsubsetsq" }, /* SQUARE IMAGE OF OR EQUAL TO */
    { 0x2292, "$reflexsupersetsq" }, /* SQUARE ORIGINAL OF OR EQUAL TO */
    { 0x2293, "$intersectionsq" }, /* SQUARE CAP */
    { 0x2294, "$unionsq" }, /* SQUARE CUP */
    { 0x2295, "circleplus" }, /* CIRCLED PLUS */
    { 0x2296, "$circleminus" }, /* CIRCLED MINUS */
    { 0x2297, "circlemultiply" }, /* CIRCLED TIMES */
    { 0x2298, "$circledivide" }, /* CIRCLED DIVISION SLASH */
    { 0x2299, "$circledot" }, /* CIRCLED DOT OPERATOR */
    { 0x22A2, "$turnstileleft" }, /* RIGHT TACK */
    { 0x22A3, "$turnstileright" }, /* LEFT TACK */
    { 0x22A4, "$latticetop" }, /* DOWN TACK */
    { 0x22A5, "perpendicular" }, /* UP TACK */
    { 0x22B2, "$normalin" }, /* NORMAL SUBGROUP OF */
    { 0x22B3, "$normalizes" }, /* CONTAINS AS NORMAL SUBGROUP */
    { 0x22B4, "$reflexnormalin" }, /* NORMAL SUBGROUP OF OR EQUAL TO */
    { 0x22B5, "$reflexnormalizes" }, /* CONTAINS AS NORMAL SUBGROUP OR EQUAL TO */
    { 0x22C4, "$diamondmath" }, /* DIAMOND OPERATOR */
    { 0x22C5, "dotmath" }, /* DOT OPERATOR */
    { 0x22C6, "$starmath" }, /* STAR OPERATOR */
    { 0x2302, "house" }, /* HOUSE */
    { 0x2308, "$ceilingleft" }, /* LEFT CEILING */
    { 0x2309, "$ceilingright" }, /* RIGHT CEILING */
    { 0x230A, "$floorleft" }, /* LEFT FLOOR */
    { 0x230B, "$floorright" }, /* RIGHT FLOOR */
    { 0x2310, "revlogicalnot" }, /* REVERSED NOT SIGN */
    { 0x2320, "integraltp" }, /* TOP HALF INTEGRAL */
    { 0x2321, "integralbt" }, /* BOTTOM HALF INTEGRAL */
    { 0x2322, "$frown" }, /* FROWN */
    { 0x2323, "$smile" }, /* SMILE */
    { 0x2329, "angleleft" }, /* LEFT-POINTING ANGLE BRACKET */
    { 0x232A, "angleright" }, /* RIGHT-POINTING ANGLE BRACKET */
    { 0x2420, "$spacesymbol" }, /* SYMBOL FOR SPACE */
    { 0x2500, "SF100000" }, /* BOX DRAWINGS LIGHT HORIZONTAL */
    { 0x2502, "SF110000" }, /* BOX DRAWINGS LIGHT VERTICAL */
    { 0x250C, "SF010000" }, /* BOX DRAWINGS LIGHT DOWN AND RIGHT */
    { 0x2510, "SF030000" }, /* BOX DRAWINGS LIGHT DOWN AND LEFT */
    { 0x2514, "SF020000" }, /* BOX DRAWINGS LIGHT UP AND RIGHT */
    { 0x2518, "SF040000" }, /* BOX DRAWINGS LIGHT UP AND LEFT */
    { 0x251C, "SF080000" }, /* BOX DRAWINGS LIGHT VERTICAL AND RIGHT */
    { 0x2524, "SF090000" }, /* BOX DRAWINGS LIGHT VERTICAL AND LEFT */
    { 0x252C, "SF060000" }, /* BOX DRAWINGS LIGHT DOWN AND HORIZONTAL */
    { 0x2534, "SF070000" }, /* BOX DRAWINGS LIGHT UP AND HORIZONTAL */
    { 0x253C, "SF050000" }, /* BOX DRAWINGS LIGHT VERTICAL AND HORIZONTAL */
    { 0x2550, "SF430000" }, /* BOX DRAWINGS DOUBLE HORIZONTAL */
    { 0x2551, "SF240000" }, /* BOX DRAWINGS DOUBLE VERTICAL */
    { 0x2552, "SF510000" }, /* BOX DRAWINGS DOWN SINGLE AND RIGHT DOUBLE */
    { 0x2553, "SF520000" }, /* BOX DRAWINGS DOWN DOUBLE AND RIGHT SINGLE */
    { 0x2554, "SF390000" }, /* BOX DRAWINGS DOUBLE DOWN AND RIGHT */
    { 0x2555, "SF220000" }, /* BOX DRAWINGS DOWN SINGLE AND LEFT DOUBLE */
    { 0x2556, "SF210000" }, /* BOX DRAWINGS DOWN DOUBLE AND LEFT SINGLE */
    { 0x2557, "SF250000" }, /* BOX DRAWINGS DOUBLE DOWN AND LEFT */
    { 0x2558, "SF500000" }, /* BOX DRAWINGS UP SINGLE AND RIGHT DOUBLE */
    { 0x2559, "SF490000" }, /* BOX DRAWINGS UP DOUBLE AND RIGHT SINGLE */
    { 0x255A, "SF380000" }, /* BOX DRAWINGS DOUBLE UP AND RIGHT */
    { 0x255B, "SF280000" }, /* BOX DRAWINGS UP SINGLE AND LEFT DOUBLE */
    { 0x255C, "SF270000" }, /* BOX DRAWINGS UP DOUBLE AND LEFT SINGLE */
    { 0x255D, "SF260000" }, /* BOX DRAWINGS DOUBLE UP AND LEFT */
    { 0x255E, "SF360000" }, /* BOX DRAWINGS VERTICAL SINGLE AND RIGHT DOUBLE */
    { 0x255F, "SF370000" }, /* BOX DRAWINGS VERTICAL DOUBLE AND RIGHT SINGLE */
    { 0x2560, "SF420000" }, /* BOX DRAWINGS DOUBLE VERTICAL AND RIGHT */
    { 0x2561, "SF190000" }, /* BOX DRAWINGS VERTICAL SINGLE AND LEFT DOUBLE */
    { 0x2562, "SF200000" }, /* BOX DRAWINGS VERTICAL DOUBLE AND LEFT SINGLE */
    { 0x2563, "SF230000" }, /* BOX DRAWINGS DOUBLE VERTICAL AND LEFT */
    { 0x2564, "SF470000" }, /* BOX DRAWINGS DOWN SINGLE AND HORIZONTAL DOUBLE */
    { 0x2565, "SF480000" }, /* BOX DRAWINGS DOWN DOUBLE AND HORIZONTAL SINGLE */
    { 0x2566, "SF410000" }, /* BOX DRAWINGS DOUBLE DOWN AND HORIZONTAL */
    { 0x2567, "SF450000" }, /* BOX DRAWINGS UP SINGLE AND HORIZONTAL DOUBLE */
    { 0x2568, "SF460000" }, /* BOX DRAWINGS UP DOUBLE AND HORIZONTAL SINGLE */
    { 0x2569, "SF400000" }, /* BOX DRAWINGS DOUBLE UP AND HORIZONTAL */
    { 0x256A, "SF540000" }, /* BOX DRAWINGS VERTICAL SINGLE AND HORIZONTAL DOUBLE */
    { 0x256B, "SF530000" }, /* BOX DRAWINGS VERTICAL DOUBLE AND HORIZONTAL SINGLE */
    { 0x256C, "SF440000" }, /* BOX DRAWINGS DOUBLE VERTICAL AND HORIZONTAL */
    { 0x2580, "upblock" }, /* UPPER HALF BLOCK */
    { 0x2584, "dnblock" }, /* LOWER HALF BLOCK */
    { 0x2588, "block" }, /* FULL BLOCK */
    { 0x258C, "lfblock" }, /* LEFT HALF BLOCK */
    { 0x2590, "rtblock" }, /* RIGHT HALF BLOCK */
    { 0x2591, "ltshade" }, /* LIGHT SHADE */
    { 0x2592, "shade" }, /* MEDIUM SHADE */
    { 0x2593, "dkshade" }, /* DARK SHADE */
    { 0x25A0, "filledbox" }, /* BLACK SQUARE */
    { 0x25A1, "H22073" }, /* WHITE SQUARE */
    { 0x25AA, "H18543" }, /* BLACK SMALL SQUARE */
    { 0x25AB, "H18551" }, /* WHITE SMALL SQUARE */
    { 0x25AC, "filledrect" }, /* BLACK RECTANGLE */
    { 0x25B2, "triagup" }, /* BLACK UP-POINTING TRIANGLE */
    { 0x25B3, "$triagwhiteup" }, /* WHITE UP-POINTING TRIANGLE */
    { 0x25BA, "triagrt" }, /* BLACK RIGHT-POINTING POINTER */
    { 0x25BC, "triagdn" }, /* BLACK DOWN-POINTING TRIANGLE */
    { 0x25BD, "$triagwhitedn" }, /* WHITE DOWN-POINTING TRIANGLE */
    { 0x25C4, "triaglf" }, /* BLACK LEFT-POINTING POINTER */
    { 0x25CA, "lozenge" }, /* LOZENGE */
    { 0x25CB, "circle" }, /* WHITE CIRCLE */
    { 0x25CF, "H18533" }, /* BLACK CIRCLE */
    { 0x25D8, "invbullet" }, /* INVERSE BULLET */
    { 0x25D9, "invcircle" }, /* INVERSE WHITE CIRCLE */
    { 0x25E6, "openbullet" }, /* WHITE BULLET */
    { 0x25EF, "$circlelarge" }, /* LARGE CIRCLE */
    { 0x263A, "smileface" }, /* WHITE SMILING FACE */
    { 0x263B, "invsmileface" }, /* BLACK SMILING FACE */
    { 0x263C, "sun" }, /* WHITE SUN WITH RAYS */
    { 0x2640, "female" }, /* FEMALE SIGN */
    { 0x2642, "male" }, /* MALE SIGN */
    { 0x2660, "spade" }, /* BLACK SPADE SUIT */
    { 0x2663, "club" }, /* BLACK CLUB SUIT */
    { 0x2665, "heart" }, /* BLACK HEART SUIT */
    { 0x2666, "diamond" }, /* BLACK DIAMOND SUIT */
    { 0x266A, "musicalnote" }, /* EIGHTH NOTE */
    { 0x266B, "musicalnotedbl" }, /* BEAMED EIGHTH NOTES */
    { 0x266D, "$flat" }, /* MUSIC FLAT SIGN */
    { 0x266E, "$natural" }, /* MUSIC NATURAL SIGN */
    { 0x266F, "$sharp" }, /* MUSIC SHARP SIGN */
    { 0xF6BE, "dotlessj" }, /* LATIN SMALL LETTER DOTLESS J */
    { 0xF6BF, "LL" }, /* LATIN CAPITAL LETTER LL */
    { 0xF6C0, "ll" }, /* LATIN SMALL LETTER LL */
    { 0xF6C3, "commaaccent" }, /* COMMA BELOW */
    { 0xF6C4, "afii10063" }, /* CYRILLIC SMALL LETTER GHE VARIANT */
    { 0xF6C5, "afii10064" }, /* CYRILLIC SMALL LETTER BE VARIANT */
    { 0xF6C6, "afii10192" }, /* CYRILLIC SMALL LETTER DE VARIANT */
    { 0xF6C7, "afii10831" }, /* CYRILLIC SMALL LETTER PE VARIANT */
    { 0xF6C8, "afii10832" }, /* CYRILLIC SMALL LETTER TE VARIANT */
    { 0xF6C9, "Acute" }, /* CAPITAL ACUTE ACCENT */
    { 0xF6CA, "Caron" }, /* CAPITAL CARON */
    { 0xF6CB, "Dieresis" }, /* CAPITAL DIAERESIS */
    { 0xF6CC, "DieresisAcute" }, /* CAPITAL DIAERESIS ACUTE ACCENT */
    { 0xF6CD, "DieresisGrave" }, /* CAPITAL DIAERESIS GRAVE ACCENT */
    { 0xF6CE, "Grave" }, /* CAPITAL GRAVE ACCENT */
    { 0xF6CF, "Hungarumlaut" }, /* CAPITAL DOUBLE ACUTE ACCENT */
    { 0xF6D0, "Macron" }, /* CAPITAL MACRON */
    { 0xF6D1, "cyrBreve" }, /* CAPITAL CYRILLIC BREVE */
    { 0xF6D2, "cyrFlex" }, /* CAPITAL CYRILLIC CIRCUMFLEX */
    { 0xF6D3, "dblGrave" }, /* CAPITAL DOUBLE GRAVE ACCENT */
    { 0xF6D4, "cyrbreve" }, /* CYRILLIC BREVE */
    { 0xF6D5, "cyrflex" }, /* CYRILLIC CIRCUMFLEX */
    { 0xF6D6, "dblgrave" }, /* DOUBLE GRAVE ACCENT */
    { 0xF6D7, "dieresisacute" }, /* DIAERESIS ACUTE ACCENT */
    { 0xF6D8, "dieresisgrave" }, /* DIAERESIS GRAVE ACCENT */
    { 0xF6D9, "copyrightserif" }, /* COPYRIGHT SIGN SERIF */
    { 0xF6DA, "registerserif" }, /* REGISTERED SIGN SERIF */
    { 0xF6DB, "trademarkserif" }, /* TRADE MARK SIGN SERIF */
    { 0xF6DC, "onefitted" }, /* PROPORTIONAL DIGIT ONE */
    { 0xF6DD, "rupiah" }, /* RUPIAH SIGN */
    { 0xF6DE, "threequartersemdash" }, /* THREE QUARTERS EM DASH */
    { 0xF6DF, "centinferior" }, /* SUBSCRIPT CENT SIGN */
    { 0xF6E0, "centsuperior" }, /* SUPERSCRIPT CENT SIGN */
    { 0xF6E1, "commainferior" }, /* SUBSCRIPT COMMA */
    { 0xF6E2, "commasuperior" }, /* SUPERSCRIPT COMMA */
    { 0xF6E3, "dollarinferior" }, /* SUBSCRIPT DOLLAR SIGN */
    { 0xF6E4, "dollarsuperior" }, /* SUPERSCRIPT DOLLAR SIGN */
    { 0xF6E5, "hypheninferior" }, /* SUBSCRIPT HYPHEN-MINUS */
    { 0xF6E6, "hyphensuperior" }, /* SUPERSCRIPT HYPHEN-MINUS */
    { 0xF6E7, "periodinferior" }, /* SUBSCRIPT FULL STOP */
    { 0xF6E8, "periodsuperior" }, /* SUPERSCRIPT FULL STOP */
    { 0xF6E9, "asuperior" }, /* SUPERSCRIPT LATIN SMALL LETTER A */
    { 0xF6EA, "bsuperior" }, /* SUPERSCRIPT LATIN SMALL LETTER B */
    { 0xF6EB, "dsuperior" }, /* SUPERSCRIPT LATIN SMALL LETTER D */
    { 0xF6EC, "esuperior" }, /* SUPERSCRIPT LATIN SMALL LETTER E */
    { 0xF6ED, "isuperior" }, /* SUPERSCRIPT LATIN SMALL LETTER I */
    { 0xF6EE, "lsuperior" }, /* SUPERSCRIPT LATIN SMALL LETTER L */
    { 0xF6EF, "msuperior" }, /* SUPERSCRIPT LATIN SMALL LETTER M */
    { 0xF6F0, "osuperior" }, /* SUPERSCRIPT LATIN SMALL LETTER O */
    { 0xF6F1, "rsuperior" }, /* SUPERSCRIPT LATIN SMALL LETTER R */
    { 0xF6F2, "ssuperior" }, /* SUPERSCRIPT LATIN SMALL LETTER S */
    { 0xF6F3, "tsuperior" }, /* SUPERSCRIPT LATIN SMALL LETTER T */
    { 0xF6F4, "Brevesmall" }, /* SMALL CAPITAL BREVE */
    { 0xF6F5, "Caronsmall" }, /* SMALL CAPITAL CARON */
    { 0xF6F6, "Circumflexsmall" }, /* SMALL CAPITAL MODIFIER LETTER CIRCUMFLEX ACCENT */
    { 0xF6F7, "Dotaccentsmall" }, /* SMALL CAPITAL DOT ABOVE */
    { 0xF6F8, "Hungarumlautsmall" }, /* SMALL CAPITAL DOUBLE ACUTE ACCENT */
    { 0xF6F9, "Lslashsmall" }, /* LATIN SMALL CAPITAL LETTER L WITH STROKE */
    { 0xF6FA, "OEsmall" }, /* LATIN SMALL CAPITAL LIGATURE OE */
    { 0xF6FB, "Ogoneksmall" }, /* SMALL CAPITAL OGONEK */
    { 0xF6FC, "Ringsmall" }, /* SMALL CAPITAL RING ABOVE */
    { 0xF6FD, "Scaronsmall" }, /* LATIN SMALL CAPITAL LETTER S WITH CARON */
    { 0xF6FE, "Tildesmall" }, /* SMALL CAPITAL SMALL TILDE */
    { 0xF6FF, "Zcaronsmall" }, /* LATIN SMALL CAPITAL LETTER Z WITH CARON */
    { 0xF721, "exclamsmall" }, /* SMALL CAPITAL EXCLAMATION MARK */
    { 0xF724, "dollaroldstyle" }, /* OLDSTYLE DOLLAR SIGN */
    { 0xF726, "ampersandsmall" }, /* SMALL CAPITAL AMPERSAND */
    { 0xF730, "zerooldstyle" }, /* OLDSTYLE DIGIT ZERO */
    { 0xF731, "oneoldstyle" }, /* OLDSTYLE DIGIT ONE */
    { 0xF732, "twooldstyle" }, /* OLDSTYLE DIGIT TWO */
    { 0xF733, "threeoldstyle" }, /* OLDSTYLE DIGIT THREE */
    { 0xF734, "fouroldstyle" }, /* OLDSTYLE DIGIT FOUR */
    { 0xF735, "fiveoldstyle" }, /* OLDSTYLE DIGIT FIVE */
    { 0xF736, "sixoldstyle" }, /* OLDSTYLE DIGIT SIX */
    { 0xF737, "sevenoldstyle" }, /* OLDSTYLE DIGIT SEVEN */
    { 0xF738, "eightoldstyle" }, /* OLDSTYLE DIGIT EIGHT */
    { 0xF739, "nineoldstyle" }, /* OLDSTYLE DIGIT NINE */
    { 0xF73F, "questionsmall" }, /* SMALL CAPITAL QUESTION MARK */
    { 0xF760, "Gravesmall" }, /* SMALL CAPITAL GRAVE ACCENT */
    { 0xF761, "Asmall" }, /* LATIN SMALL CAPITAL LETTER A */
    { 0xF762, "Bsmall" }, /* LATIN SMALL CAPITAL LETTER B */
    { 0xF763, "Csmall" }, /* LATIN SMALL CAPITAL LETTER C */
    { 0xF764, "Dsmall" }, /* LATIN SMALL CAPITAL LETTER D */
    { 0xF765, "Esmall" }, /* LATIN SMALL CAPITAL LETTER E */
    { 0xF766, "Fsmall" }, /* LATIN SMALL CAPITAL LETTER F */
    { 0xF767, "Gsmall" }, /* LATIN SMALL CAPITAL LETTER G */
    { 0xF768, "Hsmall" }, /* LATIN SMALL CAPITAL LETTER H */
    { 0xF769, "Ismall" }, /* LATIN SMALL CAPITAL LETTER I */
    { 0xF76A, "Jsmall" }, /* LATIN SMALL CAPITAL LETTER J */
    { 0xF76B, "Ksmall" }, /* LATIN SMALL CAPITAL LETTER K */
    { 0xF76C, "Lsmall" }, /* LATIN SMALL CAPITAL LETTER L */
    { 0xF76D, "Msmall" }, /* LATIN SMALL CAPITAL LETTER M */
    { 0xF76E, "Nsmall" }, /* LATIN SMALL CAPITAL LETTER N */
    { 0xF76F, "Osmall" }, /* LATIN SMALL CAPITAL LETTER O */
    { 0xF770, "Psmall" }, /* LATIN SMALL CAPITAL LETTER P */
    { 0xF771, "Qsmall" }, /* LATIN SMALL CAPITAL LETTER Q */
    { 0xF772, "Rsmall" }, /* LATIN SMALL CAPITAL LETTER R */
    { 0xF773, "Ssmall" }, /* LATIN SMALL CAPITAL LETTER S */
    { 0xF774, "Tsmall" }, /* LATIN SMALL CAPITAL LETTER T */
    { 0xF775, "Usmall" }, /* LATIN SMALL CAPITAL LETTER U */
    { 0xF776, "Vsmall" }, /* LATIN SMALL CAPITAL LETTER V */
    { 0xF777, "Wsmall" }, /* LATIN SMALL CAPITAL LETTER W */
    { 0xF778, "Xsmall" }, /* LATIN SMALL CAPITAL LETTER X */
    { 0xF779, "Ysmall" }, /* LATIN SMALL CAPITAL LETTER Y */
    { 0xF77A, "Zsmall" }, /* LATIN SMALL CAPITAL LETTER Z */
    { 0xF7A1, "exclamdownsmall" }, /* SMALL CAPITAL INVERTED EXCLAMATION MARK */
    { 0xF7A2, "centoldstyle" }, /* OLDSTYLE CENT SIGN */
    { 0xF7A8, "Dieresissmall" }, /* SMALL CAPITAL DIAERESIS */
    { 0xF7AF, "Macronsmall" }, /* SMALL CAPITAL MACRON */
    { 0xF7B4, "Acutesmall" }, /* SMALL CAPITAL ACUTE ACCENT */
    { 0xF7B8, "Cedillasmall" }, /* SMALL CAPITAL CEDILLA */
    { 0xF7BF, "questiondownsmall" }, /* SMALL CAPITAL INVERTED QUESTION MARK */
    { 0xF7E0, "Agravesmall" }, /* LATIN SMALL CAPITAL LETTER A WITH GRAVE */
    { 0xF7E1, "Aacutesmall" }, /* LATIN SMALL CAPITAL LETTER A WITH ACUTE */
    { 0xF7E2, "Acircumflexsmall" }, /* LATIN SMALL CAPITAL LETTER A WITH CIRCUMFLEX */
    { 0xF7E3, "Atildesmall" }, /* LATIN SMALL CAPITAL LETTER A WITH TILDE */
    { 0xF7E4, "Adieresissmall" }, /* LATIN SMALL CAPITAL LETTER A WITH DIAERESIS */
    { 0xF7E5, "Aringsmall" }, /* LATIN SMALL CAPITAL LETTER A WITH RING ABOVE */
    { 0xF7E6, "AEsmall" }, /* LATIN SMALL CAPITAL LETTER AE */
    { 0xF7E7, "Ccedillasmall" }, /* LATIN SMALL CAPITAL LETTER C WITH CEDILLA */
    { 0xF7E8, "Egravesmall" }, /* LATIN SMALL CAPITAL LETTER E WITH GRAVE */
    { 0xF7E9, "Eacutesmall" }, /* LATIN SMALL CAPITAL LETTER E WITH ACUTE */
    { 0xF7EA, "Ecircumflexsmall" }, /* LATIN SMALL CAPITAL LETTER E WITH CIRCUMFLEX */
    { 0xF7EB, "Edieresissmall" }, /* LATIN SMALL CAPITAL LETTER E WITH DIAERESIS */
    { 0xF7EC, "Igravesmall" }, /* LATIN SMALL CAPITAL LETTER I WITH GRAVE */
    { 0xF7ED, "Iacutesmall" }, /* LATIN SMALL CAPITAL LETTER I WITH ACUTE */
    { 0xF7EE, "Icircumflexsmall" }, /* LATIN SMALL CAPITAL LETTER I WITH CIRCUMFLEX */
    { 0xF7EF, "Idieresissmall" }, /* LATIN SMALL CAPITAL LETTER I WITH DIAERESIS */
    { 0xF7F0, "Ethsmall" }, /* LATIN SMALL CAPITAL LETTER ETH */
    { 0xF7F1, "Ntildesmall" }, /* LATIN SMALL CAPITAL LETTER N WITH TILDE */
    { 0xF7F2, "Ogravesmall" }, /* LATIN SMALL CAPITAL LETTER O WITH GRAVE */
    { 0xF7F3, "Oacutesmall" }, /* LATIN SMALL CAPITAL LETTER O WITH ACUTE */
    { 0xF7F4, "Ocircumflexsmall" }, /* LATIN SMALL CAPITAL LETTER O WITH CIRCUMFLEX */
    { 0xF7F5, "Otildesmall" }, /* LATIN SMALL CAPITAL LETTER O WITH TILDE */
    { 0xF7F6, "Odieresissmall" }, /* LATIN SMALL CAPITAL LETTER O WITH DIAERESIS */
    { 0xF7F8, "Oslashsmall" }, /* LATIN SMALL CAPITAL LETTER O WITH STROKE */
    { 0xF7F9, "Ugravesmall" }, /* LATIN SMALL CAPITAL LETTER U WITH GRAVE */
    { 0xF7FA, "Uacutesmall" }, /* LATIN SMALL CAPITAL LETTER U WITH ACUTE */
    { 0xF7FB, "Ucircumflexsmall" }, /* LATIN SMALL CAPITAL LETTER U WITH CIRCUMFLEX */
    { 0xF7FC, "Udieresissmall" }, /* LATIN SMALL CAPITAL LETTER U WITH DIAERESIS */
    { 0xF7FD, "Yacutesmall" }, /* LATIN SMALL CAPITAL LETTER Y WITH ACUTE */
    { 0xF7FE, "Thornsmall" }, /* LATIN SMALL CAPITAL LETTER THORN */
    { 0xF7FF, "Ydieresissmall" }, /* LATIN SMALL CAPITAL LETTER Y WITH DIAERESIS */
    { 0xF8E5, "radicalex" }, /* RADICAL EXTENDER */
    { 0xF8E6, "arrowvertex" }, /* VERTICAL ARROW EXTENDER */
    { 0xF8E7, "arrowhorizex" }, /* HORIZONTAL ARROW EXTENDER */
    { 0xF8E8, "registersans" }, /* REGISTERED SIGN SANS SERIF */
    { 0xF8E9, "copyrightsans" }, /* COPYRIGHT SIGN SANS SERIF */
    { 0xF8EA, "trademarksans" }, /* TRADE MARK SIGN SANS SERIF */
    { 0xF8EB, "parenlefttp" }, /* LEFT PAREN TOP */
    { 0xF8EC, "parenleftex" }, /* LEFT PAREN EXTENDER */
    { 0xF8ED, "parenleftbt" }, /* LEFT PAREN BOTTOM */
    { 0xF8EE, "bracketlefttp" }, /* LEFT SQUARE BRACKET TOP */
    { 0xF8EF, "bracketleftex" }, /* LEFT SQUARE BRACKET EXTENDER */
    { 0xF8F0, "bracketleftbt" }, /* LEFT SQUARE BRACKET BOTTOM */
    { 0xF8F1, "bracelefttp" }, /* LEFT CURLY BRACKET TOP */
    { 0xF8F2, "braceleftmid" }, /* LEFT CURLY BRACKET MID */
    { 0xF8F3, "braceleftbt" }, /* LEFT CURLY BRACKET BOTTOM */
    { 0xF8F4, "braceex" }, /* CURLY BRACKET EXTENDER */
    { 0xF8F5, "integralex" }, /* INTEGRAL EXTENDER */
    { 0xF8F6, "parenrighttp" }, /* RIGHT PAREN TOP */
    { 0xF8F7, "parenrightex" }, /* RIGHT PAREN EXTENDER */
    { 0xF8F8, "parenrightbt" }, /* RIGHT PAREN BOTTOM */
    { 0xF8F9, "bracketrighttp" }, /* RIGHT SQUARE BRACKET TOP */
    { 0xF8FA, "bracketrightex" }, /* RIGHT SQUARE BRACKET EXTENDER */
    { 0xF8FB, "bracketrightbt" }, /* RIGHT SQUARE BRACKET BOTTOM */
    { 0xF8FC, "bracerighttp" }, /* RIGHT CURLY BRACKET TOP */
    { 0xF8FD, "bracerightmid" }, /* RIGHT CURLY BRACKET MID */
    { 0xF8FE, "bracerightbt" }, /* RIGHT CURLY BRACKET BOTTOM */
    { 0xFB00, "ff" }, /* LATIN SMALL LIGATURE FF */
    { 0xFB01, "fi" }, /* LATIN SMALL LIGATURE FI */
    { 0xFB02, "fl" }, /* LATIN SMALL LIGATURE FL */
    { 0xFB03, "ffi" }, /* LATIN SMALL LIGATURE FFI */
    { 0xFB04, "ffl" }, /* LATIN SMALL LIGATURE FFL */
    { 0xFB05, "$longst" }, /* LATIN SMALL LIGATURE LONG S T */
    { 0xFB06, "$st" }, /* LATIN SMALL LIGATURE ST */
    { 0xFB1F, "afii57705" }, /* HEBREW LIGATURE YIDDISH YOD YOD PATAH */
    { 0xFB2A, "afii57694" }, /* HEBREW LETTER SHIN WITH SHIN DOT */
    { 0xFB2B, "afii57695" }, /* HEBREW LETTER SHIN WITH SIN DOT */
    { 0xFB35, "afii57723" }, /* HEBREW LETTER VAV WITH DAGESH */
    { 0xFB4B, "afii57700" } /* HEBREW LETTER VAV WITH HOLAM */
};
#endif /* 0 */

static int
adobe_name_cmp(const void *s1, const void *s2)
{
    const struct adobe2unicode *a = s1;
    const struct adobe2unicode *b = s2;
    
    return strcmp(a->adobe_name, b->adobe_name);
}

#if 0
static int
unicode_id_cmp(const void *s1, const void *s2)
{
    const struct unicode2adobe *a = s1;
    const struct unicode2adobe *b = s2;

    return a->unicode - b->unicode;
}
#endif /* 0 */

#if HAVE_ICONV_H
static void
close_iconv(void *dummy)
{
    UNUSED(dummy);
    if (m_iconv_gb_ucs4 != (iconv_t)(-1)) {
	iconv_close(m_iconv_gb_ucs4);
	m_iconv_gb_ucs4 = (iconv_t)(-1);
    }
}
#endif /* HAVE_ICONV_H */

/* convert a CJK char to unicode (UCS-4) using iconv() */
static uint32_t
cjk2unicode(unsigned char *cjk)
{
#if HAVE_ICONV_H
    uint32_t u = 0;
    unsigned char unicode[4];
    size_t from = 2;
    size_t to = sizeof unicode;
    static Boolean initialized = False;
    char *from_ptr = (char *)cjk;
    char *to_ptr = (char *)unicode;
    
    if (m_iconv_gb_ucs4 == (iconv_t)(-1)) {
	if (initialized)
	    return 0;
	initialized = True;
	m_iconv_gb_ucs4 = iconv_open(
#ifdef WORDS_BIGENDIAN
				     "UCS-4BE",
#else
				     "UCS-4LE",
#endif
				     "GB18030");
	if (m_iconv_gb_ucs4 == (iconv_t)(-1)) {
	    XDVI_ERROR((stderr, "cjk2unicode: iconv_open() failed: %s", strerror(errno)));
	    return 0;
	}

	register_exit_handler(close_iconv, NULL);
	
    }
    if (iconv(m_iconv_gb_ucs4, (iconv_char_pptrT)&from_ptr, &from, &to_ptr, &to) == (size_t)(-1)) {
	XDVI_ERROR((stderr, "cjk2unicode: can't convert GBK to unicode: %s", strerror(errno)));
	return 0;
    }
    memcpy(&u, unicode, sizeof unicode);
    return u;
#else /* HAVE_ICONV_H */
    static Boolean warned = False;

    if (!warned) {
        popup_message(globals.widgets.top_level,
                      MSG_WARN, NULL, "This version of xdvi has been compiled without iconv support - "
                      "cannot convert CJK character to UTF-8");
        warned = True;
    }
    return 0;
#endif /* HAVE_ICONV_H */
}

/*
 * Return the unicode ID of adobe_name, or 0 if it isn't found.
 * Uses the lookup table adobe2unicode_table, or iconv for CJK fonts.
 */
uint32_t
adobe2unicode_name(const char *adobe_name)
{
    struct adobe2unicode search_item;
    struct adobe2unicode *match_item;

    if (HAS_PREFIX(adobe_name, "cjk")) {
	/* Special case for CJK fonts (Chinese) - ZLB: the Adobe names in
	 * the Chinese T1 fonts are of the form 'cjkXXXX' where 'XXXX' are
	 * the hex number of the GBK/GB18030 encoding */
	unsigned char cjk[2], xx[3];
	xx[0] = adobe_name[3];
	xx[1] = adobe_name[4];
	xx[2] = '\0';
	cjk[0] = strtoul((char *)xx, NULL, 16);
	xx[0] = adobe_name[5];
	xx[1] = adobe_name[6];
	cjk[1] = strtoul((char *)xx, NULL, 16);
	/* convert GBK ==> unicode */
	return cjk2unicode(cjk);
    }
    else {
	search_item.adobe_name = adobe_name;
    
	match_item = bsearch(&search_item, adobe2unicode_table,
			     sizeof adobe2unicode_table / sizeof adobe2unicode_table[0],
			     sizeof adobe2unicode_table[0],
			     adobe_name_cmp);
	if (match_item != NULL)
	    return match_item->unicode;
	else
	    return 0;
    }
}

#if 0
/*
 * Return the adobe_name of unicode ID, or NULL if it isn't found.
 * Uses the lookup table unicode2adobe_table.
 */
const char *
unicode2adobe_name(uint32_t unicode)
{
    struct unicode2adobe search_item;
    struct unicode2adobe *match_item;
    
    search_item.unicode = unicode;
    
    match_item = bsearch(&search_item, unicode2adobe_table,
			 sizeof unicode2adobe_table / sizeof unicode2adobe_table[0],
			 sizeof unicode2adobe_table[0],
			 unicode_id_cmp);
    if (match_item != NULL)
	return match_item->adobe_name;
    else
	return NULL;
}
#endif /* 0 */


static uint32_t
ucs4_lowercase(uint32_t c)
{
    /*
      This table was produced by:
      
      cat uni2adobe | while read a b; do lc_b=`echo $b | tr 'A-Z' 'a-z'`; \
      if [ "$lc_b" != "$b" ] ; then res=`egrep " $lc_b\$" uni2adobe` ; \
      if [ -n "$res" ]; then echo "RES: $a $b -> $res"; fi; fi; done | grep 'RES' > uni2adobe-map

      And then, some Emacs keyboard macros.
    */
    switch (c) {
    case 0x0041: /* A */                return 0x0061; /* a */
    case 0x0042: /* B */                return 0x0062; /* b */
    case 0x0043: /* C */                return 0x0063; /* c */
    case 0x0044: /* D */                return 0x0064; /* d */
    case 0x0045: /* E */                return 0x0065; /* e */
    case 0x0046: /* F */                return 0x0066; /* f */
    case 0x0047: /* G */                return 0x0067; /* g */
    case 0x0048: /* H */                return 0x0068; /* h */
    case 0x0049: /* I */                return 0x0069; /* i */
    case 0x004A: /* J */                return 0x006A; /* j */
    case 0x004B: /* K */                return 0x006B; /* k */
    case 0x004C: /* L */                return 0x006C; /* l */
    case 0x004D: /* M */                return 0x006D; /* m */
    case 0x004E: /* N */                return 0x006E; /* n */
    case 0x004F: /* O */                return 0x006F; /* o */
    case 0x0050: /* P */                return 0x0070; /* p */
    case 0x0051: /* Q */                return 0x0071; /* q */
    case 0x0052: /* R */                return 0x0072; /* r */
    case 0x0053: /* S */                return 0x0073; /* s */
    case 0x0054: /* T */                return 0x0074; /* t */
    case 0x0055: /* U */                return 0x0075; /* u */
    case 0x0056: /* V */                return 0x0076; /* v */
    case 0x0057: /* W */                return 0x0077; /* w */
    case 0x0058: /* X */                return 0x0078; /* x */
    case 0x0059: /* Y */                return 0x0079; /* y */
    case 0x005A: /* Z */                return 0x007A; /* z */
    case 0x00C0: /* Agrave */           return 0x00E0; /* agrave */
    case 0x00C1: /* Aacute */           return 0x00E1; /* aacute */
    case 0x00C2: /* Acircumflex */      return 0x00E2; /* acircumflex */
    case 0x00C3: /* Atilde */           return 0x00E3; /* atilde */
    case 0x00C4: /* Adieresis */        return 0x00E4; /* adieresis */
    case 0x00C5: /* Aring */            return 0x00E5; /* aring */
    case 0x00C6: /* AE */               return 0x00E6; /* ae */
    case 0x00C7: /* Ccedilla */         return 0x00E7; /* ccedilla */
    case 0x00C8: /* Egrave */           return 0x00E8; /* egrave */
    case 0x00C9: /* Eacute */           return 0x00E9; /* eacute */
    case 0x00CA: /* Ecircumflex */      return 0x00EA; /* ecircumflex */
    case 0x00CB: /* Edieresis */        return 0x00EB; /* edieresis */
    case 0x00CC: /* Igrave */           return 0x00EC; /* igrave */
    case 0x00CD: /* Iacute */           return 0x00ED; /* iacute */
    case 0x00CE: /* Icircumflex */      return 0x00EE; /* icircumflex */
    case 0x00CF: /* Idieresis */        return 0x00EF; /* idieresis */
    case 0x00D0: /* Eth */              return 0x00F0; /* eth */
    case 0x00D1: /* Ntilde */           return 0x00F1; /* ntilde */
    case 0x00D2: /* Ograve */           return 0x00F2; /* ograve */
    case 0x00D3: /* Oacute */           return 0x00F3; /* oacute */
    case 0x00D4: /* Ocircumflex */      return 0x00F4; /* ocircumflex */
    case 0x00D5: /* Otilde */           return 0x00F5; /* otilde */
    case 0x00D6: /* Odieresis */        return 0x00F6; /* odieresis */
    case 0x00D8: /* Oslash */           return 0x00F8; /* oslash */
    case 0x00D9: /* Ugrave */           return 0x00F9; /* ugrave */
    case 0x00DA: /* Uacute */           return 0x00FA; /* uacute */
    case 0x00DB: /* Ucircumflex */      return 0x00FB; /* ucircumflex */
    case 0x00DC: /* Udieresis */        return 0x00FC; /* udieresis */
    case 0x00DD: /* Yacute */           return 0x00FD; /* yacute */
    case 0x00DE: /* Thorn */            return 0x00FE; /* thorn */
    case 0x0100: /* Amacron */          return 0x0101; /* amacron */
    case 0x0102: /* Abreve */           return 0x0103; /* abreve */
    case 0x0104: /* Aogonek */          return 0x0105; /* aogonek */
    case 0x0106: /* Cacute */           return 0x0107; /* cacute */
    case 0x0108: /* Ccircumflex */      return 0x0109; /* ccircumflex */
    case 0x010A: /* Cdotaccent */       return 0x010B; /* cdotaccent */
    case 0x010C: /* Ccaron */           return 0x010D; /* ccaron */
    case 0x010E: /* Dcaron */           return 0x010F; /* dcaron */
    case 0x0110: /* Dcroat */           return 0x0111; /* dcroat */
    case 0x0112: /* Emacron */          return 0x0113; /* emacron */
    case 0x0114: /* Ebreve */           return 0x0115; /* ebreve */
    case 0x0116: /* Edotaccent */       return 0x0117; /* edotaccent */
    case 0x0118: /* Eogonek */          return 0x0119; /* eogonek */
    case 0x011A: /* Ecaron */           return 0x011B; /* ecaron */
    case 0x011C: /* Gcircumflex */      return 0x011D; /* gcircumflex */
    case 0x011E: /* Gbreve */           return 0x011F; /* gbreve */
    case 0x0120: /* Gdotaccent */       return 0x0121; /* gdotaccent */
    case 0x0122: /* Gcommaaccent */     return 0x0123; /* gcommaaccent */
    case 0x0124: /* Hcircumflex */      return 0x0125; /* hcircumflex */
    case 0x0126: /* Hbar */             return 0x0127; /* hbar */
    case 0x0128: /* Itilde */           return 0x0129; /* itilde */
    case 0x012A: /* Imacron */          return 0x012B; /* imacron */
    case 0x012C: /* Ibreve */           return 0x012D; /* ibreve */
    case 0x012E: /* Iogonek */          return 0x012F; /* iogonek */
    case 0x0132: /* IJ */               return 0x0133; /* ij */
    case 0x0134: /* Jcircumflex */      return 0x0135; /* jcircumflex */
    case 0x0136: /* Kcommaaccent */     return 0x0137; /* kcommaaccent */
    case 0x0139: /* Lacute */           return 0x013A; /* lacute */
    case 0x013B: /* Lcommaaccent */     return 0x013C; /* lcommaaccent */
    case 0x013D: /* Lcaron */           return 0x013E; /* lcaron */
    case 0x013F: /* Ldot */             return 0x0140; /* ldot */
    case 0x0141: /* Lslash */           return 0x0142; /* lslash */
    case 0x0143: /* Nacute */           return 0x0144; /* nacute */
    case 0x0145: /* Ncommaaccent */     return 0x0146; /* ncommaaccent */
    case 0x0147: /* Ncaron */           return 0x0148; /* ncaron */
    case 0x014A: /* Eng */              return 0x014B; /* eng */
    case 0x014C: /* Omacron */          return 0x014D; /* omacron */
    case 0x014E: /* Obreve */           return 0x014F; /* obreve */
    case 0x0150: /* Ohungarumlaut */    return 0x0151; /* ohungarumlaut */
    case 0x0152: /* OE */               return 0x0153; /* oe */
    case 0x0154: /* Racute */           return 0x0155; /* racute */
    case 0x0156: /* Rcommaaccent */     return 0x0157; /* rcommaaccent */
    case 0x0158: /* Rcaron */           return 0x0159; /* rcaron */
    case 0x015A: /* Sacute */           return 0x015B; /* sacute */
    case 0x015C: /* Scircumflex */      return 0x015D; /* scircumflex */
    case 0x015E: /* Scedilla */         return 0x015F; /* scedilla */
    case 0x0160: /* Scaron */           return 0x0161; /* scaron */
    case 0x0162: /* Tcommaaccent */     return 0x0163; /* tcommaaccent */
    case 0x0164: /* Tcaron */           return 0x0165; /* tcaron */
    case 0x0166: /* Tbar */             return 0x0167; /* tbar */
    case 0x0168: /* Utilde */           return 0x0169; /* utilde */
    case 0x016A: /* Umacron */          return 0x016B; /* umacron */
    case 0x016C: /* Ubreve */           return 0x016D; /* ubreve */
    case 0x016E: /* Uring */            return 0x016F; /* uring */
    case 0x0170: /* Uhungarumlaut */    return 0x0171; /* uhungarumlaut */
    case 0x0172: /* Uogonek */          return 0x0173; /* uogonek */
    case 0x0174: /* Wcircumflex */      return 0x0175; /* wcircumflex */
    case 0x0176: /* Ycircumflex */      return 0x0177; /* ycircumflex */
    case 0x0178: /* Ydieresis */        return 0x00FF; /* ydieresis */
    case 0x0179: /* Zacute */           return 0x017A; /* zacute */
    case 0x017B: /* Zdotaccent */       return 0x017C; /* zdotaccent */
    case 0x017D: /* Zcaron */           return 0x017E; /* zcaron */
    case 0x01A0: /* Ohorn */            return 0x01A1; /* ohorn */
    case 0x01AF: /* Uhorn */            return 0x01B0; /* uhorn */
    case 0x01E6: /* Gcaron */           return 0x01E7; /* gcaron */
    case 0x01FA: /* Aringacute */       return 0x01FB; /* aringacute */
    case 0x01FC: /* AEacute */          return 0x01FD; /* aeacute */
    case 0x01FE: /* Oslashacute */      return 0x01FF; /* oslashacute */
    case 0x0218: /* Scommaaccent */     return 0x0219; /* scommaaccent */
    case 0x0386: /* Alphatonos */       return 0x03AC; /* alphatonos */
    case 0x0388: /* Epsilontonos */     return 0x03AD; /* epsilontonos */
    case 0x0389: /* Etatonos */         return 0x03AE; /* etatonos */
    case 0x038A: /* Iotatonos */        return 0x03AF; /* iotatonos */
    case 0x038C: /* Omicrontonos */     return 0x03CC; /* omicrontonos */
    case 0x038E: /* Upsilontonos */     return 0x03CD; /* upsilontonos */
    case 0x038F: /* Omegatonos */       return 0x03CE; /* omegatonos */
    case 0x0391: /* Alpha */            return 0x03B1; /* alpha */
    case 0x0392: /* Beta */             return 0x03B2; /* beta */
    case 0x0393: /* Gamma */            return 0x03B3; /* gamma */
    case 0x0395: /* Epsilon */          return 0x03B5; /* epsilon */
    case 0x0396: /* Zeta */             return 0x03B6; /* zeta */
    case 0x0397: /* Eta */              return 0x03B7; /* eta */
    case 0x0398: /* Theta */            return 0x03B8; /* theta */
    case 0x0399: /* Iota */             return 0x03B9; /* iota */
    case 0x039A: /* Kappa */            return 0x03BA; /* kappa */
    case 0x039B: /* Lambda */           return 0x03BB; /* lambda */
    case 0x039C: /* Mu */               return 0x00B5; /* mu */
    case 0x039D: /* Nu */               return 0x03BD; /* nu */
    case 0x039E: /* Xi */               return 0x03BE; /* xi */
    case 0x039F: /* Omicron */          return 0x03BF; /* omicron */
    case 0x03A0: /* Pi */               return 0x03C0; /* pi */
    case 0x03A1: /* Rho */              return 0x03C1; /* rho */
    case 0x03A3: /* Sigma */            return 0x03C3; /* sigma */
    case 0x03A4: /* Tau */              return 0x03C4; /* tau */
    case 0x03A5: /* Upsilon */          return 0x03C5; /* upsilon */
    case 0x03A6: /* Phi */              return 0x03C6; /* phi */
    case 0x03A7: /* Chi */              return 0x03C7; /* chi */
    case 0x03A8: /* Psi */              return 0x03C8; /* psi */
    case 0x03AA: /* Iotadieresis */     return 0x03CA; /* iotadieresis */
    case 0x03AB: /* Upsilondieresis */  return 0x03CB; /* upsilondieresis */
    case 0x1E80: /* Wgrave */           return 0x1E81; /* wgrave */
    case 0x1E82: /* Wacute */           return 0x1E83; /* wacute */
    case 0x1E84: /* Wdieresis */        return 0x1E85; /* wdieresis */
    case 0x1EF2: /* Ygrave */           return 0x1EF3; /* ygrave */
    case 0x2126: /* Omega */            return 0x03C9; /* omega */
    case 0x2206: /* Delta */            return 0x03B4; /* delta */
    case 0xF6BF: /* LL */               return 0xF6C0; /* ll */
    case 0xF6C9: /* Acute */            return 0x00B4; /* acute */
    case 0xF6CA: /* Caron */            return 0x02C7; /* caron */
    case 0xF6CB: /* Dieresis */         return 0x00A8; /* dieresis */
    case 0xF6CC: /* DieresisAcute */    return 0xF6D7; /* dieresisacute */
    case 0xF6CD: /* DieresisGrave */    return 0xF6D8; /* dieresisgrave */
    case 0xF6CE: /* Grave */            return 0x0060; /* grave */
    case 0xF6CF: /* Hungarumlaut */     return 0x02DD; /* hungarumlaut */
    case 0xF6D0: /* Macron */           return 0x00AF; /* macron */
    case 0xF6D1: /* cyrBreve */         return 0xF6D4; /* cyrbreve */
    case 0xF6D2: /* cyrFlex */          return 0xF6D5; /* cyrflex */
    case 0xF6D3: /* dblGrave */         return 0xF6D6; /* dblgrave */
    default: return c;
    }
}

/*
 * For the unicode IDs of an accent and a base_glyph, return the
 * unicode ID of the composed (accented) glyph. If there is no
 * suitable composed glyph, return 0.
 */
uint32_t
get_accented_glyph(uint32_t accent, uint32_t base_glyph)
{
    TRACE_FIND((stderr, "get_accented_glyph: %lu, %lu",
		(unsigned long)accent, (unsigned long)base_glyph));
    switch(accent) {
    case 0x0060: /* grave */
	switch (base_glyph) {
	case 0x0041: /* A */ return 0x00C0;
	case 0x0045: /* E */ return 0x00C8;
	case 0x0049: /* I */ return 0x00CC;
	case 0x004F: /* O */ return 0x00D2;
	case 0x0055: /* U */ return 0x00D9;
	case 0x0057: /* W */ return 0x1E80;
	case 0x0059: /* Y */ return 0x1EF2;
	case 0x0061: /* a */ return 0x00E0;
	case 0x0065: /* e */ return 0x00E8;
	case 0x0069: /* i */ return 0x00EC;
	case 0x006F: /* o */ return 0x00F2;
	case 0x0075: /* u */ return 0x00F9;
	case 0x0077: /* w */ return 0x1E81;
	case 0x0079: /* y */ return 0x1EF3;
	default: return 0;
	}
    case 0x00B4: /* acute */
	switch (base_glyph) {
	case 0x0041: /* A */ return 0x00C1;
	case 0x0043: /* C */ return 0x0106;
	case 0x0045: /* E */ return 0x00C9;
	case 0x0049: /* I */ return 0x00CD;
	case 0x004C: /* L */ return 0x0139;
	case 0x004E: /* N */ return 0x0143;
	case 0x004F: /* O */ return 0x00D3;
	case 0x0052: /* R */ return 0x0154;
	case 0x0053: /* S */ return 0x015A;
	case 0x0055: /* U */ return 0x00DA;
	case 0x0057: /* W */ return 0x1E82;
	case 0x0059: /* Y */ return 0x00DD;
	case 0x0060: /* Z */ return 0x0179;
	case 0x0061: /* a */ return 0x00E1;
	case 0x0063: /* c */ return 0x0107;
	case 0x0065: /* e */ return 0x00E9;
	case 0x0069: /* i */ return 0x00ED;
	case 0x006C: /* l */ return 0x013A;
	case 0x006E: /* n */ return 0x0144;
	case 0x006F: /* o */ return 0x00F3;
	case 0x0072: /* r */ return 0x0155;
	case 0x0073: /* s */ return 0x015B;
	case 0x0075: /* u */ return 0x00FA;
	case 0x0077: /* w */ return 0x1E83;
	case 0x0079: /* y */ return 0x00FD;
	case 0x0080: /* z */ return 0x017A;
	default: return 0;
	}
    case 0x02C6: /* circumflex */
    case 0x005E: /* asciicircum */
	switch (base_glyph) {
	case 0x0041: /* A */ return 0x00C2;
	case 0x0045: /* E */ return 0x00CA;
	case 0x0047: /* G */ return 0x011C;
	case 0x0048: /* H */ return 0x0124;
	case 0x0049: /* I */ return 0x00CE;
	case 0x0050: /* H */ return 0x0124;
	case 0x004F: /* O */ return 0x00D4;
	case 0x0055: /* U */ return 0x00DB;
	case 0x0061: /* a */ return 0x00E2;
	case 0x0065: /* e */ return 0x00EA;
	case 0x0067: /* g */ return 0x011D;
	case 0x0068: /* h */ return 0x0125;
	case 0x0069: /* i */ return 0x00EE;
	case 0x0070: /* j */ return 0x0135;
	case 0x006F: /* o */ return 0x00F4;
	case 0x0075: /* u */ return 0x00FB;
	default: return 0;
	}
    case 0x02DC: /* tilde */
    case 0x007E: /* asciitilde */
	switch (base_glyph) {
	case 0x0041: /* A */ return 0x00C3;
	case 0x0049: /* I */ return 0x0128;
	case 0x004E: /* N */ return 0x00D1;
	case 0x004F: /* O */ return 0x00D5;
	case 0x0055: /* U */ return 0x0168;
	case 0x0061: /* a */ return 0x00E3;
	case 0x0069: /* i */ return 0x0129;
	case 0x006E: /* n */ return 0x00F1;
	case 0x006F: /* o */ return 0x00F5;
	case 0x0075: /* u */ return 0x0169;
	default: return 0;
	}
    case 0x00A8: /* dieresis */
	switch (base_glyph) {
	case 0x0041: /* A */ return 0x00C4;
	case 0x0045: /* E */ return 0x00CB;
	case 0x0049: /* I */ return 0x00CF;
	case 0x004F: /* O */ return 0x00D6;
	case 0x0055: /* U */ return 0x00DC;
	case 0x0057: /* w */ return 0x1E84;
	case 0x0061: /* a */ return 0x00E4;
	case 0x0065: /* e */ return 0x00EB;
	case 0x0069: /* i */ return 0x00EF;
	case 0x006F: /* o */ return 0x00F6;
	case 0x0075: /* u */ return 0x00FC;
	case 0x0077: /* w */ return 0x1E85;
	case 0x0079: /* y */ return 0x00FF;
	default: return 0;
	}
    case 0x02DA: /* ring */
	switch (base_glyph) {
	case 0x0041: /* A */ return 0x00C5;
	case 0x0061: /* a */ return 0x00E5;
	case 0x0055: /* U */ return 0x016E;
	case 0x0075: /* u */ return 0x016F;
	default: return 0;
	}
    case 0x00B8: /* cedilla */
	switch (base_glyph) {
	case 0x0043: /* C */ return 0x00C7;
	case 0x0063: /* c */ return 0x00E7;
	case 0x0053: /* S */ return 0x015E;
	case 0x0073: /* s */ return 0x015F;
	default: return 0;
	}
    case 0x02DB: /* ogonek */
	switch (base_glyph) {
	case 0x0041: /* A */ return 0x0104;
	case 0x0045: /* E */ return 0x0118;
	case 0x0049: /* I */ return 0x012E;
	case 0x0055: /* U */ return 0x0172;
	case 0x0061: /* a */ return 0x0105;
	case 0x0065: /* e */ return 0x0119;
	case 0x0069: /* i */ return 0x012F;
	case 0x006F: /* o */ return 0x02DB;
	case 0x0075: /* u */ return 0x0173;
	default: return 0;
	}
    case 0x002F: /* solidus */
	switch (base_glyph) {
	case 0x004C: /* L */ return 0x0141;
	case 0x004F: /* O */ return 0x00D8;
	case 0x006C: /* l */ return 0x0142;
	case 0x006F: /* o */ return 0x00F8;
	default: return 0;
	}
    case 0x02C7: /* caron */
	switch (base_glyph) {
	case 0x0043: /* C */ return 0x010C;
	case 0x0044: /* D */ return 0x010E;
	case 0x0045: /* E */ return 0x011A;
	case 0x0047: /* G */ return 0x01E6;
	case 0x004C: /* L */ return 0x013D;
	case 0x004E: /* N */ return 0x0147;
	case 0x0052: /* R */ return 0x0158;
	case 0x0053: /* S */ return 0x0160;
	case 0x0054: /* T */ return 0x0164;
	case 0x005A: /* Z */ return 0x017D;
	case 0x0063: /* c */ return 0x010D;
	case 0x0064: /* d */ return 0x010F;
	case 0x0065: /* e */ return 0x011B;
	case 0x0067: /* g */ return 0x01E7;
	case 0x006C: /* l */ return 0x013E;
	case 0x006E: /* n */ return 0x0148;
	case 0x0072: /* r */ return 0x0159;
	case 0x0073: /* s */ return 0x0161;
	case 0x0074: /* t */ return 0x0165;
	case 0x007A: /* z */ return 0x017E;
	default: return 0;
	}
    case 0x02D8: /* breve */
	switch (base_glyph) {
	case 0x0041: /* A */ return 0x0102;
	case 0x0045: /* E */ return 0x0114;
	case 0x0047: /* G */ return 0x011E;
	case 0x0049: /* I */ return 0x012C;
	case 0x004F: /* O */ return 0x014E;
	case 0x0055: /* U */ return 0x016C;
	case 0x0061: /* a */ return 0x0103;
	case 0x0065: /* e */ return 0x0115;
	case 0x0067: /* g */ return 0x011F;
	case 0x0069: /* i */ return 0x012D;
	case 0x006F: /* o */ return 0x014F;
	case 0x0075: /* u */ return 0x016D;
	default: return 0;
	}	
    case 0x02DD: /* hungarumlaut */
	switch (base_glyph) {
	case 0x004F: /* O */ return 0x0150;
	case 0x0055: /* U */ return 0x0170;
	case 0x006F: /* o */ return 0x0151;
	case 0x0075: /* u */ return 0x0171;
	default: return 0;
	}
    case 0x00AF: /* macron */
	switch (base_glyph) {
	case 0x0041: /* A */ return 0x0100;
	case 0x0045: /* E */ return 0x0112;
	case 0x0049: /* I */ return 0x012A;
	case 0x004F: /* O */ return 0x014C;
	case 0x0055: /* U */ return 0x016A;
	case 0x0061: /* a */ return 0x0101;
	case 0x0065: /* e */ return 0x0113;
	case 0x0069: /* i */ return 0x012B;
	case 0x006D: /* m */ return 0x00AF;
	case 0x006F: /* o */ return 0x014D;
	case 0x0075: /* u */ return 0x016B;
	default: return 0;
	}
	/* special cases: accent - char inverted */
    case 0x0043: /* C; special case: cedilla is set after C in OT1 */
	switch (base_glyph) {
	case 0x00B8: /* cedilla */ return 0x00C7;
	default: return 0;
	}
    case 0x0063: /* c; see above */
	switch (base_glyph) {
	case 0x00B8: /* cedilla */ return 0x00E7;
	default: return 0;
	}
    case 0x0053: /* S; see above */
	switch (base_glyph) {
	case 0x00B8: /* cedilla */ return 0x015E;
	default: return 0;
	}
    case 0x0073: /* s; see above */
	switch (base_glyph) {
	case 0x00B8: /* cedilla */ return 0x015F;
	default: return 0;
	}
    }
    return 0;
}

uint32_t
guess_encoding(wide_ubyte ch, const char *fontname, char *retbuf)
{
    uint32_t i;
    static hashTableT unknown_font_hash;
    static Boolean hash_initialized = False;
    size_t dummy = 0;
    
    TRACE_FIND_VERBOSE((stderr, "guess_encoding: |%s|, char 0x%.4X", fontname, ch));

    /* our encoding vectors only have size 256 */
    if (ch > 255) {
	XDVI_WARNING((stderr, "guess_encoding: font index %lu too large", (unsigned long)ch));
	return 0;
    }

    if (HAS_PREFIX(fontname, "gbk")
	&& isdigit((int)fontname[(i=strlen(fontname))-1])
	&& isdigit((int)fontname[i-2]))
    {
	unsigned char cjk[2];
	TRACE_FIND_VERBOSE((stderr, "guess_encoding: CJK fonts (GBK encoding)"));
	i = atoi(fontname + i - 2);		/* font no */
	i = (i - 1) * 256 + (uint32_t)ch;	/* char index */
	cjk[0] = i / 190 + 129;
	cjk[1] = i % 190 + 64;
	if (cjk[1] >= 128)
	    cjk[1]++;
	return cjk2unicode(cjk);
    }

    if (HAS_PREFIX(fontname, "cmsy") ||
	HAS_PREFIX(fontname, "xccsy")
	) {
	TRACE_FIND_VERBOSE((stderr, "guess_encoding: %s => m_cm_symbol", fontname));
	return m_cm_symbol_encoding[ch];
    }
    if (HAS_PREFIX(fontname, "cmmi") ||
	HAS_PREFIX(fontname, "xccmi")
	) {
	TRACE_FIND_VERBOSE((stderr, "guess_encoding: %s => m_cm_math_italics", fontname));
	return m_cm_math_italics_encoding[ch];
    }
    if (HAS_PREFIX(fontname, "cmex") ||
	HAS_PREFIX(fontname, "xccex")
	) {
	TRACE_FIND_VERBOSE((stderr, "guess_encoding: %s => m_cm_math_extended", fontname));
	return m_cm_math_extended_encoding[ch];
    }
    if (HAS_PREFIX(fontname, "cmtt")) {
	TRACE_FIND_VERBOSE((stderr, "guess_encoding: %s => m_cm_typewriter", fontname));
	return m_cm_typewriter_encoding[ch];
    }
    /* following to cover cmsl, cmb, cmbx, cmti, cmdunghill, whatever ...
       hope it doesn't overgenerate ;-) */
    if (HAS_PREFIX(fontname, "cm")
	|| HAS_PREFIX(fontname, "ccr")
	|| HAS_PREFIX(fontname, "lcmss") /* lcmss8 etc. */
	|| HAS_PREFIX(fontname, "ygoth")
	|| HAS_PREFIX(fontname, "yinit")
	|| HAS_PREFIX(fontname, "logo")
	|| HAS_PREFIX(fontname, "rsfs")
	|| HAS_PREFIX(fontname, "bbm")
	) {
	TRACE_FIND_VERBOSE((stderr, "guess_encoding: %s => m_ot1", fontname));
	return m_ot1_encoding[ch];
    }
    /* cyrillic fonts */
    if (HAS_PREFIX(fontname, "la")) {
	TRACE_FIND_VERBOSE((stderr, "guess_encoding: %s => m_t2", fontname));
	return m_t2_encoding[ch];
    }
    if (HAS_PREFIX(fontname, "ec")
	|| HAS_PREFIX(fontname, "eo")
	|| HAS_PREFIX(fontname, "eb")) {
	TRACE_FIND_VERBOSE((stderr, "guess_encoding: %s => m_cork", fontname));
	/* FIXME: why cork and not EC? Are there fonts that actually use EC?
	   The only difference seems that dvips' EC.enc has `ldot' at 0xb8,
	   whereas cork.enc has `ydieresis' there. A document with
	   \usepackage[T1]{fontenc}
	   also produces a ydieresis.
	*/
	return m_cork_encoding[ch];
    }
    if (HAS_PREFIX(fontname, "tc")) {
	TRACE_FIND_VERBOSE((stderr, "guess_encoding: %s => m_ts1", fontname));
	return m_ts1_encoding[ch];
    }
    /* blackletter fonts with funny encoding */
    if (HAS_PREFIX(fontname, "ysmfrak")
	|| HAS_PREFIX(fontname, "yswab")) {
	TRACE_FIND_VERBOSE((stderr, "guess_encoding: %s => m_yfrak", fontname));
	/* special cases for ligatures */
	switch (ch) {
	case 0x85: strcpy(retbuf, "ch"); return 0;
	case 0x86: strcpy(retbuf, "ck"); return 0;
	case 0xA7: strcpy(retbuf, "sz"); return 0;
	default: return m_yfrak_encoding[ch];
	}
    }
    /* euler mathematical */
    if (HAS_PREFIX(fontname, "eufm")
	|| HAS_PREFIX(fontname, "eusm")) {
	switch (ch) {
	case 0x0: case 0x1: return 'd'; break;
	case 0x2: case 0x3: return 'f'; break;
	case 0x4: return 'g'; break;
	case 0x5: return 'k'; break;
	case 0x6: return 't'; break;
	case 0x7: return 'u'; break;
	default: return m_ot1_encoding[ch];
	}
    }
    /* stuff that doesn't have a good ASCII representation */
    if (HAS_PREFIX(fontname, "lcircle")
	|| HAS_PREFIX(fontname, "line")
	|| HAS_PREFIX(fontname, "fmvr8x")
	|| HAS_PREFIX(fontname, "feymr")
	|| HAS_PREFIX(fontname, "msbm")
	|| HAS_PREFIX(fontname, "msam")
	|| HAS_PREFIX(fontname, "wasy")
	|| HAS_PREFIX(fontname, "txsy")
	) {
	return 0;
    }
    if (HAS_PREFIX(fontname, "bbold")) {
	return m_bbold_encoding[ch];
    }
    if (HAS_PREFIX(fontname, "gli")
	|| HAS_PREFIX(fontname, "glj")
	|| HAS_PREFIX(fontname, "glm")
	|| HAS_PREFIX(fontname, "glt")
	|| HAS_PREFIX(fontname, "glw")
	|| HAS_PREFIX(fontname, "glx")
	|| HAS_PREFIX(fontname, "gmm")
	|| HAS_PREFIX(fontname, "gmt")
	|| HAS_PREFIX(fontname, "gmx")
	|| HAS_PREFIX(fontname, "gom")
	|| HAS_PREFIX(fontname, "gox")
	|| HAS_PREFIX(fontname, "grb")
	|| HAS_PREFIX(fontname, "grm")
	|| HAS_PREFIX(fontname, "grx")
	|| HAS_PREFIX(fontname, "gsm")
	|| HAS_PREFIX(fontname, "gsx")
	|| HAS_PREFIX(fontname, "gtt")
	) {
	TRACE_FIND_VERBOSE((stderr, "guess_encoding: %s => m_cb_greek_encoding", fontname));
	return m_cb_greek_encoding[ch];
    }
    if (HAS_PREFIX(fontname, "wasy")) {
	/* these are all special symbols */
	return 0;
    }
    if (HAS_PREFIX(fontname, "to")) {
	if (ch >= '0' && ch <= '9') {
	    return ch;
	}
	else { /* special symbols */
	    return 0;
	}
    }
    
    /* TODO:
       txfonts
    */
    
    /* default: assume cork encoding, and print out a warning for each font */
    if (!hash_initialized) {
	unknown_font_hash = hash_create(1031);
	hash_initialized = True;
    }
    if (!find_str_int_hash(&unknown_font_hash, fontname, &dummy)) {
	if (resource.freetype) {
	    XDVI_WARNING((stderr,
			  "guess_encoding(): nothing suitable for \"%s\", assuming Cork encoding.\n"
			  "(Please tell us about this at "
			  "http://sourceforge.net/tracker/?group_id=23164&atid=377580)", fontname));
	}
	else {
	    XDVI_INFO((stderr,
		       "FreeType2 not enabled, assuming Cork encoding for font \"%s\".\n",
		       fontname));
	}
	put_str_int_hash(&unknown_font_hash, fontname, dummy);
    }
    return m_cork_encoding[ch];
}

/*
  Mapping of unicode glyphs to ASCII `equivalents'
  that are useful when searching for text.
*/
const char *
search_normalize_chars(uint32_t unicode)
{
    const char *ret = NULL;
    switch(unicode) {
    case 0x2212: ret = "-";	break;
    case 0x2022: ret = "\xb7";	break; /* middle dot */
    default: ret = NULL;	break;
    }
    if (ret != NULL) {
        TRACE_FIND((stderr, "expand_searchchars: 0x%X --> `%s'",
                    (unsigned int)unicode, ret));
    }
    return ret;
}

/*
  Expand a unicode ligature character consisting of several
  `ordinary' characters to a string of those characters, and
  map some unicode glyphs to ASCII `equivalents'.
  
  TODO: move the more ambiguous replacements (e.g.
  guilsinglleft vs. angleleft) into a user-defined table?
*/
const char *
expand_ligature(uint32_t unicode)
{
    const char *ret = NULL;
    switch(unicode) {
    case 0xFB00: ret = "ff";	break;
    case 0xFB01: ret = "fi";	break;
    case 0xFB02: ret = "fl";	break;
    case 0xFB03: ret = "ffi";	break;
    case 0xFB04: ret = "ffl";	break;
    case 0xFB06: ret = "st";	break;
    case 0x0133: ret = "ij";	break;
    case 0x2013: ret = "--";	break;
    case 0x2014: ret = "---";	break;
    case 0x2039:
    case 0x2329: ret = "<";	break;
    case 0x203A:
    case 0x232A: ret = ">";	break;
    case 0x2018: ret = "`";	break;
    case 0x2019: ret = "'";	break;
    case 0x201C: ret = "``";	break;
    case 0x201D: ret = "''";	break;
    case 0x2026: ret = "...";	break;
    case 0x10ff28: ret = "{";	break;
    case 0x10ff29: ret = "}";	break;
    default: ret = NULL;	break;
    }
    if (ret != NULL) {
        TRACE_FIND((stderr, "expand_ligature: 0x%X --> `%s'",
                    (unsigned int)unicode, ret));
    }
    return ret;
}

/*
 * Convert the utf8-string passed as argument to lowercase, by
 * first converting to UCS4, then converting back, lowercasing the
 * UCS4 characters as we go (it's easier like that).
 */
Boolean
utf8_lowercase(char *utf8)
{
    size_t utf8_len = strlen(utf8) + 1; /* also convert terminating byte (acutally not needed ...) */
    size_t ucs4_len = utf8_len * 6; /* ample ... */
    int i = 0;
    uint32_t *ucs4 = xmalloc(ucs4_len);
    uint32_t *ucs4_start = ucs4; /* save for free()ing */
    for (; *utf8 != '\0'; ucs4++, utf8 += i) {
	size_t conv_len;
	if ((i = utf8_to_ucs4(utf8, ucs4, 6 /* don't care about character len */)) < 0) {
	    XDVI_ERROR((stderr, "Error in utf8_lowercase: Illegal UTF-8 sequence"));
	    free(ucs4_start);
	    return False;
	}
	ucs4_to_utf8(*ucs4, utf8, &conv_len, True); /* lowercases it */
	if ((int)conv_len != i) {
	    XDVI_ERROR((stderr, "Error in utf8_lowercase: length after UCS4 conversion (%lu)\n"
			"differs from length after utf8 conversion(%lu) (string: %s)\n",
			(unsigned long)conv_len, (unsigned long)i, utf8));
	    free(ucs4_start);
	    return False;
	}
    }
    free(ucs4_start);
    return True;
}

/*
 * Adapted from iconv's utf8_wctomb in lib/utf8.h
 * Converts the character sequence pointed to by utf8 of maximum length `len'
 * into an UCS4 character pointed to by ucs4.
 * TODO: maybe implement this as a macro??
 */
int
utf8_to_ucs4(const char *utf8, uint32_t *ucs4, size_t len)
{
    const unsigned char *str = (const unsigned char *)utf8;
    unsigned char c = *str;

    if (c < 0x80) {
	*ucs4 = c;
	return 1;
    }
    else if (c < 0xc2) {
	return -1; /* illegal UTF8; shouldn't happen */
    }
    else if (c < 0xe0) {
	if (len < 2)
	    return -1; /* len too short */
	if (!((str[1] ^ 0x80) < 0x40)) {
	    return -1; /* illegal UTF8; shouldn't happen */
	}
	*ucs4 = ((uint32_t) (c & 0x1f) << 6)
	    | (uint32_t) (str[1] ^ 0x80);
	return 2;
    }
    else if (c < 0xf0) {
	if (len < 3)
	    return -1; /* len too short */
	if (!((str[1] ^ 0x80) < 0x40
	      && (str[2] ^ 0x80) < 0x40
	      && (c >= 0xe1 || str[1] >= 0xa0))) {
	    return -1; /* illegal UTF8; shouldn't happen */
	}
	*ucs4 = ((uint32_t) (c & 0x0f) << 12)
	    | ((uint32_t) (str[1] ^ 0x80) << 6)
	    | (uint32_t) (str[2] ^ 0x80);
	return 3;
    }
    else if (c < 0xf8 && sizeof(uint32_t) * 8 >= 32) {
	if (len < 4)
	    return -1; /* len too short */
	if (!((str[1] ^ 0x80) < 0x40
	      && (str[2] ^ 0x80) < 0x40
	      && (str[3] ^ 0x80) < 0x40
	      && (c >= 0xf1 || str[1] >= 0x90))) {
	    return -1; /* illegal UTF8; shouldn't happen */
	}
	*ucs4 = ((uint32_t) (c & 0x07) << 18)
	    | ((uint32_t) (str[1] ^ 0x80) << 12)
	    | ((uint32_t) (str[2] ^ 0x80) << 6)
	    | (uint32_t) (str[3] ^ 0x80);
	return 4;
    }
    else if (c < 0xfc && sizeof(uint32_t)*8 >= 32) {
	if (len < 5)
	    return -1; /* len too short */
	if (!((str[1] ^ 0x80) < 0x40
	      && (str[2] ^ 0x80) < 0x40
	      && (str[3] ^ 0x80) < 0x40
	      && (str[4] ^ 0x80) < 0x40
	      && (c >= 0xf9 || str[1] >= 0x88))) {
	    return -1; /* illegal UTF8; shouldn't happen */
	}
	*ucs4 = ((uint32_t) (c & 0x03) << 24)
	    | ((uint32_t) (str[1] ^ 0x80) << 18)
	    | ((uint32_t) (str[2] ^ 0x80) << 12)
	    | ((uint32_t) (str[3] ^ 0x80) << 6)
	    | (uint32_t) (str[4] ^ 0x80);
	return 5;
    }
    else if (c < 0xfe && sizeof(uint32_t)*8 >= 32) {
	if (len < 6)
	    return -1; /* len too short */
	if (!((str[1] ^ 0x80) < 0x40
	      && (str[2] ^ 0x80) < 0x40
	      && (str[3] ^ 0x80) < 0x40
	      && (str[4] ^ 0x80) < 0x40
	      && (str[5] ^ 0x80) < 0x40
	      && (c >= 0xfd || str[1] >= 0x84))) {
	    return -1; /* illegal UTF8; shouldn't happen */
	}
	*ucs4 = ((uint32_t) (c & 0x01) << 30)
	    | ((uint32_t) (str[1] ^ 0x80) << 24)
	    | ((uint32_t) (str[2] ^ 0x80) << 18)
	    | ((uint32_t) (str[3] ^ 0x80) << 12)
	    | ((uint32_t) (str[4] ^ 0x80) << 6)
	    | (uint32_t) (str[5] ^ 0x80);
	return 6;
    }
    else {
	return -1; /* illegal UTF8 */
    }
}

/* adapted from iconv's utf8_wctomb in lib/utf8.h */
/* TODO: maybe implement this as a macro?? */
void
ucs4_to_utf8(uint32_t ucs4, char *utf8, size_t *len, Boolean do_lowercase)
{
    if (do_lowercase) {
	ucs4 = ucs4_lowercase(ucs4);
	TRACE_FIND_VERBOSE((stderr, "Lowercasing of 0x%lx --> 0x%lx\n", ucs4_bak, ucs4));
    }
    else {
	TRACE_FIND_VERBOSE((stderr, "NOT lowercasing 0x%lx\n", ucs4));
    }
    
    if (ucs4 < 0x80)
	*len = 1;
    else if (ucs4 < 0x800)
	*len = 2;
    else if (ucs4 < 0x10000)
	*len = 3;
    else if (ucs4 < 0x200000)
	*len = 4;
    else if (ucs4 < 0x4000000)
	*len = 5;
    else if (ucs4 <= 0x7fffffff)
	*len = 6;

    switch(*len) { /* note: code falls through cases! */
    case 6: utf8[5] = 0x80 | (ucs4 & 0x3f); ucs4 = ucs4 >> 6; ucs4 |= 0x4000000;
    case 5: utf8[4] = 0x80 | (ucs4 & 0x3f); ucs4 = ucs4 >> 6; ucs4 |= 0x200000;
    case 4: utf8[3] = 0x80 | (ucs4 & 0x3f); ucs4 = ucs4 >> 6; ucs4 |= 0x10000;
    case 3: utf8[2] = 0x80 | (ucs4 & 0x3f); ucs4 = ucs4 >> 6; ucs4 |= 0x800;
    case 2: utf8[1] = 0x80 | (ucs4 & 0x3f); ucs4 = ucs4 >> 6; ucs4 |= 0xc0;
    case 1: utf8[0] = ucs4;
    }
}



/*
 * (Lossy) conversion of utf8 sequence to an iso-latin1; produces
 * `?' if the character is not representable in iso-latin1.
 * Returns the number of converted characters in `len'.
 */
unsigned char
utf8_to_iso_8859_1(const char *utf8, size_t *len)
{
    unsigned char c = *utf8;
    uint32_t wc;

    if (c < 0x80) {
	*len = 1;
	return c;
    }
    else if (c < 0xe0) {
	*len = 2;
	wc = ((unsigned char)(c & 0x1f) << 6) | (unsigned char)(utf8[1] ^ 0x80);
	if (wc <= 0xff)
	    return (unsigned char)wc;
	else
	    return '?';
    }
    else if (c < 0xf0) {
	*len = 3;
	return '?';
    }
    else if (c < 0xf8) {
	*len = 4;
	return '?';
    }
    else if (c < 0xfc) {
	*len = 5;
	return '?';
    }
    else if (c < 0xfe) {
	*len = 6;
	return '?';
    }
    else
	return '?';
}

/* convert an iso_8859_1 character to utf8, writing the result
   to the buffer `utf8' (which should have at least length 2),
   and storing the length of the converted utf-8 sequence in *len.
*/
void
iso_8859_1_to_utf8(unsigned char c, char *utf8, size_t *len)
{
    if (c < 0x80) {
	*len = 1;
	utf8[0] = c;
    }
    else {
	*len = 2;
	utf8[1] = 0x80 | (c & 0x3f);
	c >>= 6;
	utf8[0] = 0xc0 | (c & 0x1f);
    }
}

/* Convert an iso_latin-1 string to a UTF-8 string (including the
   terminating `\0'), not writing more than `len' characters to
   `utf8'. Returns the number of written characters, or -1 if len was
   too small to convert the entire input string.
*/
int
str_iso_8859_1_to_utf8(const char *latin1, char *utf8, size_t len)
{
    size_t i = 0;

    while (i < len && *latin1 != '\0') {
	char tmpbuf[2];
	size_t tmp_len = 0;
	iso_8859_1_to_utf8((unsigned char)*latin1, tmpbuf, &tmp_len);
	if (i + tmp_len >= len)
	    return -1;
	memcpy(utf8 + i, tmpbuf, tmp_len);
	i += tmp_len;
	latin1++;
    }
    /* terminate utf8 */
    if (i < len)
	utf8[i++] = '\0';
    else
	return -1;
    
    return i;
}

Boolean
is_hyphenchar(uint32_t u)
{
    if (u == 0x002D || u ==  0x00AD)
	return True;
    return False;
}

/*
 * Determine ideographic property according to
 * http://www.unicode.org/Public/UNIDATA/PropList.txt:
 * 
 * 3006          ; Ideographic # Lo       IDEOGRAPHIC CLOSING MARK
 * 3007          ; Ideographic # Nl       IDEOGRAPHIC NUMBER ZERO
 * 3021..3029    ; Ideographic # Nl   [9] HANGZHOU NUMERAL ONE..HANGZHOU NUMERAL NINE
 * 3038..303A    ; Ideographic # Nl   [3] HANGZHOU NUMERAL TEN..HANGZHOU NUMERAL THIRTY
 * 3400..4DB5    ; Ideographic # Lo [6582] CJK UNIFIED IDEOGRAPH-3400..CJK UNIFIED IDEOGRAPH-4DB5
 * 4E00..9FA5    ; Ideographic # Lo [20902] CJK UNIFIED IDEOGRAPH-4E00..CJK UNIFIED IDEOGRAPH-9FA5
 * F900..FA2D    ; Ideographic # Lo [302] CJK COMPATIBILITY IDEOGRAPH-F900..CJK COMPATIBILITY IDEOGRAPH-FA2D
 * 20000..2A6D6  ; Ideographic # Lo [42711] CJK UNIFIED IDEOGRAPH-20000..CJK UNIFIED IDEOGRAPH-2A6D6
 * 2F800..2FA1D  ; Ideographic # Lo [542] CJK COMPATIBILITY IDEOGRAPH-2F800..CJK COMPATIBILITY IDEOGRAPH-2FA1D
 * 
 * # Total code points: 71053
 *
 * plus space, comma and full stop.
 *
 */
Boolean
is_ideograph(uint32_t u)
{
    return (u >= 0x3000 && u <= 0x3002) || /* IDEOGRAPHIC SPACE, COMMA, FULL STOP */
	u == 0xFF61 || /* HALFWIDTH IDEOGRAPHIC FULL STOP */
	u == 0xFF64 || /* HALFWIDTH IDEOGRAPHIC COMMA */
	(u >= 0x3006 && u <= 0x3007) ||
	(u >= 0x3021 && u <= 0x3029) ||
	(u >= 0x3038 && u <= 0x303A) ||
	(u >= 0x3400 && u <= 0x4DB5) ||
	(u >= 0x4E00 && u <= 0x9FA5) ||
	(u >= 0xF900 && u <= 0xFA2D) ||
	(u >= 0x20000 && u <= 0x2A6D6) ||
	(u >= 0x2F800 && u <= 0x2FA1D);
}

char *
str_utf8_to_iso_8859_1(const char *utf8)
{
    size_t utf8_len = strlen(utf8), i = 0, offset = 0;
    char *buf = xmalloc(4 * utf8_len + 1); /* worst case of non-printables */

    while (i < utf8_len) {
	uint32_t ucs4;
	const char *ret;

	/*  fprintf(stderr, "offset: %d\n", (int)offset); */
	/* first apply normalization heurisitcs also used by search */
	size_t len = utf8_to_ucs4(utf8 + i, &ucs4, utf8_len + 1);
	if ((ret = search_normalize_chars(ucs4)) != NULL) {
	    size_t len_ret = strlen(ret);
	    memcpy(buf + offset, ret, len_ret);
	    offset += len_ret;
	}
	else if (ucs4 <= 0xff) { /* in iso-latin1 range */
	    buf[offset++] = (unsigned char)ucs4;
	}
	else {
	    sprintf(buf + offset, "\\%.4lX", (unsigned long)ucs4);
	    offset += 4;
	}
	i += len;
    }
    buf[offset] = '\0';
    
    return buf;
}
