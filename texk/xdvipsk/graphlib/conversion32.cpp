/* This is xdvipsk, an eXtended version of dvips(k) by Tomas Rokicki.

	Copyright (C) 2016 by VTeX Ltd (www.vtex.lt),
	the xdvipsk project team - Sigitas Tolusis and Arunas Povilaitis.

    Program original code copyright by Floris van den Berg,
	Hervé Drolon and Karl-Heinz Bussian, the FreeImage 3 project team.

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/


#include "bitmap.h"
#include "utilities.h"

// ----------------------------------------------------------
//  internal conversions X to 32 bits
// ----------------------------------------------------------

void
Bitmap_ConvertLine1To32(BYTE *target, BYTE *source, int width_in_pixels, RGBQUAD *palette) {
	for (int cols = 0; cols < width_in_pixels; cols++) {
		int index = (source[cols>>3] & (0x80 >> (cols & 0x07))) != 0 ? 1 : 0;

		target[FI_RGBA_BLUE]	= palette[index].rgbBlue;
		target[FI_RGBA_GREEN]	= palette[index].rgbGreen;
		target[FI_RGBA_RED]		= palette[index].rgbRed;
		target[FI_RGBA_ALPHA]	= 0xFF;
		target += 4;
	}	
}

void
Bitmap_ConvertLine4To32(BYTE *target, BYTE *source, int width_in_pixels, RGBQUAD *palette) {
	BOOL low_nibble = FALSE;
	int x = 0;

	for (int cols = 0 ; cols < width_in_pixels ; ++cols) {
		if (low_nibble) {
			target[FI_RGBA_BLUE]	= palette[LOWNIBBLE(source[x])].rgbBlue;
			target[FI_RGBA_GREEN]	= palette[LOWNIBBLE(source[x])].rgbGreen;
			target[FI_RGBA_RED]		= palette[LOWNIBBLE(source[x])].rgbRed;

			x++;
		} else {
			target[FI_RGBA_BLUE]	= palette[HINIBBLE(source[x]) >> 4].rgbBlue;
			target[FI_RGBA_GREEN]	= palette[HINIBBLE(source[x]) >> 4].rgbGreen;
			target[FI_RGBA_RED]		= palette[HINIBBLE(source[x]) >> 4].rgbRed;
		}

		low_nibble = !low_nibble;

		target[FI_RGBA_ALPHA] = 0xFF;
		target += 4;
	}
}

void
Bitmap_ConvertLine8To32(BYTE *target, BYTE *source, int width_in_pixels, RGBQUAD *palette) {
	for (int cols = 0; cols < width_in_pixels; cols++) {
		target[FI_RGBA_BLUE]	= palette[source[cols]].rgbBlue;
		target[FI_RGBA_GREEN]	= palette[source[cols]].rgbGreen;
		target[FI_RGBA_RED]		= palette[source[cols]].rgbRed;
		target[FI_RGBA_ALPHA]	= 0xFF;
		target += 4;
	}
}

void
Bitmap_ConvertLine16To32_555(BYTE *target, BYTE *source, int width_in_pixels) {
	WORD *bits = (WORD *)source;

	for (int cols = 0; cols < width_in_pixels; cols++) {
		target[FI_RGBA_RED]   = (BYTE)((((bits[cols] & FI16_555_RED_MASK) >> FI16_555_RED_SHIFT) * 0xFF) / 0x1F);
		target[FI_RGBA_GREEN] = (BYTE)((((bits[cols] & FI16_555_GREEN_MASK) >> FI16_555_GREEN_SHIFT) * 0xFF) / 0x1F);
		target[FI_RGBA_BLUE]  = (BYTE)((((bits[cols] & FI16_555_BLUE_MASK) >> FI16_555_BLUE_SHIFT) * 0xFF) / 0x1F);
		target[FI_RGBA_ALPHA] = 0xFF;
		target += 4;
	}
}

void
Bitmap_ConvertLine16To32_565(BYTE *target, BYTE *source, int width_in_pixels) {
	WORD *bits = (WORD *)source;

	for (int cols = 0; cols < width_in_pixels; cols++) {
		target[FI_RGBA_RED]   = (BYTE)((((bits[cols] & FI16_565_RED_MASK) >> FI16_565_RED_SHIFT) * 0xFF) / 0x1F);
		target[FI_RGBA_GREEN] = (BYTE)((((bits[cols] & FI16_565_GREEN_MASK) >> FI16_565_GREEN_SHIFT) * 0xFF) / 0x3F);
		target[FI_RGBA_BLUE]  = (BYTE)((((bits[cols] & FI16_565_BLUE_MASK) >> FI16_565_BLUE_SHIFT) * 0xFF) / 0x1F);
		target[FI_RGBA_ALPHA] = 0xFF;
		target += 4;
	}
}

/**
This unoptimized version of the conversion function avoid an undetermined bug with VC++ SP6. 
The bug occurs in release mode only, when the image height is equal to 537 
(try e.g. a size of 432x537 to reproduce the bug with the optimized function).
*/
void
Bitmap_ConvertLine24To32(BYTE *target, BYTE *source, int width_in_pixels) {
	for (int cols = 0; cols < width_in_pixels; cols++) {
		target[FI_RGBA_RED]   = source[FI_RGBA_RED];
		target[FI_RGBA_GREEN] = source[FI_RGBA_GREEN];
		target[FI_RGBA_BLUE]  = source[FI_RGBA_BLUE];
		target[FI_RGBA_ALPHA] = 0xFF;
		target += 4;
		source += 3;
	}
}

// ----------------------------------------------------------

inline void 
Bitmap_ConvertLine1To32MapTransparency(BYTE *target, BYTE *source, int width_in_pixels, RGBQUAD *palette, BYTE *table, int transparent_pixels) {
	for (int cols = 0; cols < width_in_pixels; cols++) {
		int index = (source[cols>>3] & (0x80 >> (cols & 0x07))) != 0 ? 1 : 0;

		target[FI_RGBA_BLUE]	= palette[index].rgbBlue;
		target[FI_RGBA_GREEN]	= palette[index].rgbGreen;
		target[FI_RGBA_RED]		= palette[index].rgbRed;
		target[FI_RGBA_ALPHA] = (index < transparent_pixels) ? table[index] : 255;		
		target += 4;
	}	
}

inline void 
Bitmap_ConvertLine4To32MapTransparency(BYTE *target, BYTE *source, int width_in_pixels, RGBQUAD *palette, BYTE *table, int transparent_pixels) {
	BOOL low_nibble = FALSE;
	int x = 0;

	for (int cols = 0 ; cols < width_in_pixels ; ++cols) {
		if (low_nibble) {
			target[FI_RGBA_BLUE]	= palette[LOWNIBBLE(source[x])].rgbBlue;
			target[FI_RGBA_GREEN]	= palette[LOWNIBBLE(source[x])].rgbGreen;
			target[FI_RGBA_RED]		= palette[LOWNIBBLE(source[x])].rgbRed;
			target[FI_RGBA_ALPHA]	= (LOWNIBBLE(source[x]) < transparent_pixels) ? table[LOWNIBBLE(source[x])] : 255;

			x++;
		} else {
			target[FI_RGBA_BLUE]	= palette[HINIBBLE(source[x]) >> 4].rgbBlue;
			target[FI_RGBA_GREEN]	= palette[HINIBBLE(source[x]) >> 4].rgbGreen;
			target[FI_RGBA_RED]		= palette[HINIBBLE(source[x]) >> 4].rgbRed;
			target[FI_RGBA_ALPHA]	= (HINIBBLE(source[x] >> 4) < transparent_pixels) ? table[HINIBBLE(source[x]) >> 4] : 255;
		}

		low_nibble = !low_nibble;
				
		target += 4;
	}
}

inline void 
Bitmap_ConvertLine8To32MapTransparency(BYTE *target, BYTE *source, int width_in_pixels, RGBQUAD *palette, BYTE *table, int transparent_pixels) {
	for (int cols = 0; cols < width_in_pixels; cols++) {
		target[FI_RGBA_BLUE]	= palette[source[cols]].rgbBlue;
		target[FI_RGBA_GREEN]	= palette[source[cols]].rgbGreen;
		target[FI_RGBA_RED]		= palette[source[cols]].rgbRed;
		target[FI_RGBA_ALPHA] = (source[cols] < transparent_pixels) ? table[source[cols]] : 255;
		target += 4;		
	}
}

// ----------------------------------------------------------

FIBITMAP * 
Bitmap_ConvertTo32Bits(FIBITMAP *dib) {
	if (!Bitmap_HasPixels(dib)) return NULL;

	const int bpp = Bitmap_GetBPP(dib);
	const BITMAP_TYPE image_type = Bitmap_GetImageType(dib);
	
	if(image_type != FIT_BITMAP) {
		return NULL;
	}
	
	const int width = Bitmap_GetWidth(dib);
	const int height = Bitmap_GetHeight(dib);

	if(image_type == FIT_BITMAP) {

		if(bpp == 32) {
			return Bitmap_Clone(dib);
		}

		FIBITMAP *new_dib = Bitmap_Allocate(width, height, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
		if(new_dib == NULL) {
			return NULL;
		}

		BOOL bIsTransparent = Bitmap_IsTransparent(dib);

		switch(bpp) {
			case 1:
			{
				if(bIsTransparent) {
					for (int rows = 0; rows < height; rows++) {
						Bitmap_ConvertLine1To32MapTransparency(Bitmap_GetScanLine(new_dib, rows), Bitmap_GetScanLine(dib, rows), width, Bitmap_GetPalette(dib), Bitmap_GetTransparencyTable(dib), Bitmap_GetTransparencyCount(dib));
					}
				} else {
					for (int rows = 0; rows < height; rows++) {
						Bitmap_ConvertLine1To32(Bitmap_GetScanLine(new_dib, rows), Bitmap_GetScanLine(dib, rows), width, Bitmap_GetPalette(dib));
					}					
				}

				return new_dib;
			}

			case 4:
			{
				if(bIsTransparent) {
					for (int rows = 0; rows < height; rows++) {
						Bitmap_ConvertLine4To32MapTransparency(Bitmap_GetScanLine(new_dib, rows), Bitmap_GetScanLine(dib, rows), width, Bitmap_GetPalette(dib), Bitmap_GetTransparencyTable(dib), Bitmap_GetTransparencyCount(dib));
					}
				} else {
					for (int rows = 0; rows < height; rows++) {
						Bitmap_ConvertLine4To32(Bitmap_GetScanLine(new_dib, rows), Bitmap_GetScanLine(dib, rows), width, Bitmap_GetPalette(dib));
					}					
				}

				return new_dib;
			}
				
			case 8:
			{
				if(bIsTransparent) {
					for (int rows = 0; rows < height; rows++) {
						Bitmap_ConvertLine8To32MapTransparency(Bitmap_GetScanLine(new_dib, rows), Bitmap_GetScanLine(dib, rows), width, Bitmap_GetPalette(dib), Bitmap_GetTransparencyTable(dib), Bitmap_GetTransparencyCount(dib));
					}
				} else {
					for (int rows = 0; rows < height; rows++) {
						Bitmap_ConvertLine8To32(Bitmap_GetScanLine(new_dib, rows), Bitmap_GetScanLine(dib, rows), width, Bitmap_GetPalette(dib));
					}					
				}

				return new_dib;
			}

			case 16:
			{
				for (int rows = 0; rows < height; rows++) {
					if ((Bitmap_GetRedMask(dib) == FI16_565_RED_MASK) && (Bitmap_GetGreenMask(dib) == FI16_565_GREEN_MASK) && (Bitmap_GetBlueMask(dib) == FI16_565_BLUE_MASK)) {
						Bitmap_ConvertLine16To32_565(Bitmap_GetScanLine(new_dib, rows), Bitmap_GetScanLine(dib, rows), width);
					} else {
						// includes case where all the masks are 0
						Bitmap_ConvertLine16To32_555(Bitmap_GetScanLine(new_dib, rows), Bitmap_GetScanLine(dib, rows), width);
					}
				}

				return new_dib;
			}

			case 24:
			{
				for (int rows = 0; rows < height; rows++) {
					Bitmap_ConvertLine24To32(Bitmap_GetScanLine(new_dib, rows), Bitmap_GetScanLine(dib, rows), width);
				}

				return new_dib;
			}
		}

	}
	
	return NULL;
}
