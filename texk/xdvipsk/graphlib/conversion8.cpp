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

// ==========================================================
// Bitmap conversion routines
//
// ==========================================================

#include "bitmap.h"
#include "utilities.h"

// ----------------------------------------------------------
//  internal conversions X to 8 bits
// ----------------------------------------------------------

void
Bitmap_ConvertLine1To8(BYTE *target, BYTE *source, int width_in_pixels) {
	for (unsigned cols = 0; cols < (unsigned)width_in_pixels; cols++)
		target[cols] = (source[cols >> 3] & (0x80 >> (cols & 0x07))) != 0 ? 255 : 0;	
}

void
Bitmap_ConvertLine4To8(BYTE *target, BYTE *source, int width_in_pixels) {
	unsigned count_new = 0;
	unsigned count_org = 0;
	BOOL hinibble = TRUE;

	while (count_new < (unsigned)width_in_pixels) {
		if (hinibble) {
			target[count_new] = (source[count_org] >> 4);
		} else {
			target[count_new] = (source[count_org] & 0x0F);
			count_org++;
		}
		hinibble = !hinibble;
		count_new++;
	}
}

void
Bitmap_ConvertLine16To8_555(BYTE *target, BYTE *source, int width_in_pixels) {
	const WORD *const bits = (WORD *)source;
	for (unsigned cols = 0; cols < (unsigned)width_in_pixels; cols++) {
		target[cols] = GREY((((bits[cols] & FI16_555_RED_MASK) >> FI16_555_RED_SHIFT) * 0xFF) / 0x1F,
			                (((bits[cols] & FI16_555_GREEN_MASK) >> FI16_555_GREEN_SHIFT) * 0xFF) / 0x1F,
							(((bits[cols] & FI16_555_BLUE_MASK) >> FI16_555_BLUE_SHIFT) * 0xFF) / 0x1F);
	}
}

void
Bitmap_ConvertLine16To8_565(BYTE *target, BYTE *source, int width_in_pixels) {
	const WORD *const bits = (WORD *)source;
	for (unsigned cols = 0; cols < (unsigned)width_in_pixels; cols++) {
		target[cols] = GREY((((bits[cols] & FI16_565_RED_MASK) >> FI16_565_RED_SHIFT) * 0xFF) / 0x1F,
			        (((bits[cols] & FI16_565_GREEN_MASK) >> FI16_565_GREEN_SHIFT) * 0xFF) / 0x3F,
					(((bits[cols] & FI16_565_BLUE_MASK) >> FI16_565_BLUE_SHIFT) * 0xFF) / 0x1F);
	}
}

void
Bitmap_ConvertLine24To8(BYTE *target, BYTE *source, int width_in_pixels) {
	for (unsigned cols = 0; cols < (unsigned)width_in_pixels; cols++) {
		target[cols] = GREY(source[FI_RGBA_RED], source[FI_RGBA_GREEN], source[FI_RGBA_BLUE]);
		source += 3;
	}
}

void
Bitmap_ConvertLine32To8(BYTE *target, BYTE *source, int width_in_pixels) {
	for (unsigned cols = 0; cols < (unsigned)width_in_pixels; cols++) {
		target[cols] = GREY(source[FI_RGBA_RED], source[FI_RGBA_GREEN], source[FI_RGBA_BLUE]);
		source += 4;
	}
}

// ----------------------------------------------------------
//   smart convert X to 8 bits
// ----------------------------------------------------------

FIBITMAP *
Bitmap_ConvertTo8Bits(FIBITMAP *dib) {
	if (!Bitmap_HasPixels(dib)) {
		return NULL;
	}

	const BITMAP_TYPE image_type = Bitmap_GetImageType(dib);
	if (image_type != FIT_BITMAP) {
		return NULL;
	}

	const unsigned bpp = Bitmap_GetBPP(dib);

	if (bpp != 8) {

		const unsigned width = Bitmap_GetWidth(dib);
		const unsigned height = Bitmap_GetHeight(dib);

		// Allocate a destination image
		FIBITMAP *new_dib = Bitmap_Allocate(width, height, 8);
		if (new_dib == NULL) {
			return NULL;
		}

		// Palette of destination image has already been initialized
		RGBQUAD *new_pal = Bitmap_GetPalette(new_dib);

		BITMAP_COLOR_TYPE color_type = Bitmap_GetColorType(dib);

		switch(bpp) {
			case 1:
			{
				if (color_type == FIC_PALETTE) {
					// Copy the palette
					RGBQUAD *old_pal = Bitmap_GetPalette(dib);
					new_pal[0] = old_pal[0];
					new_pal[255] = old_pal[1];

				} else if (color_type == FIC_MINISWHITE) {
					// Create a reverse grayscale palette
					CREATE_GREYSCALE_PALETTE_REVERSE(new_pal, 256);
				}

				// Expand and copy the bitmap data
				for (unsigned rows = 0; rows < height; rows++) {
					Bitmap_ConvertLine1To8(Bitmap_GetScanLine(new_dib, rows), Bitmap_GetScanLine(dib, rows), width);
				}
				return new_dib;
			}

			case 4 :
			{
				if (color_type == FIC_PALETTE) {
					// Copy the palette
					memcpy(new_pal, Bitmap_GetPalette(dib), 16 * sizeof(RGBQUAD));
				}

				// Expand and copy the bitmap data
				for (unsigned rows = 0; rows < height; rows++) {
					Bitmap_ConvertLine4To8(Bitmap_GetScanLine(new_dib, rows), Bitmap_GetScanLine(dib, rows), width);
				}
				return new_dib;
			}

			case 16 :
			{
				// Expand and copy the bitmap data
				if (IS_FORMAT_RGB565(dib)) {
					for (unsigned rows = 0; rows < height; rows++) {
						Bitmap_ConvertLine16To8_565(Bitmap_GetScanLine(new_dib, rows), Bitmap_GetScanLine(dib, rows), width);
					}
				} else {
					for (unsigned rows = 0; rows < height; rows++) {
						Bitmap_ConvertLine16To8_555(Bitmap_GetScanLine(new_dib, rows), Bitmap_GetScanLine(dib, rows), width);
					}
				}
				return new_dib;
			}

			case 24 :
			{
				// Expand and copy the bitmap data
				for (unsigned rows = 0; rows < height; rows++) {
					Bitmap_ConvertLine24To8(Bitmap_GetScanLine(new_dib, rows), Bitmap_GetScanLine(dib, rows), width);
				}
				return new_dib;
			}

			case 32 :
			{
				// Expand and copy the bitmap data
				for (unsigned rows = 0; rows < height; rows++) {
					Bitmap_ConvertLine32To8(Bitmap_GetScanLine(new_dib, rows), Bitmap_GetScanLine(dib, rows), width);
				}
				return new_dib;
			}
		}

	} // bpp != 8

	return Bitmap_Clone(dib);
}

FIBITMAP *
Bitmap_ConvertToGreyscale(FIBITMAP *dib) {
	if (!Bitmap_HasPixels(dib)) {
		return NULL;
	}

	BITMAP_COLOR_TYPE color_type = Bitmap_GetColorType(dib);

	if (color_type == FIC_PALETTE || color_type == FIC_MINISWHITE) {

		const unsigned bpp = Bitmap_GetBPP(dib);
		const unsigned width = Bitmap_GetWidth(dib);
		const unsigned height = Bitmap_GetHeight(dib);

		FIBITMAP *new_dib = Bitmap_Allocate(width, height, 8);
		if (new_dib == NULL) {
			return NULL;
		}

		// Create a greyscale palette
		BYTE grey_pal[256];
		const RGBQUAD *pal = Bitmap_GetPalette(dib);
		const unsigned size = CalculateUsedPaletteEntries(bpp);
		for (unsigned i = 0; i < size; i++) {
			grey_pal[i] = GREY(pal->rgbRed, pal->rgbGreen, pal->rgbBlue);
			pal++;
		}

		const BYTE *src_bits = Bitmap_GetBits(dib);
		BYTE *dst_bits = Bitmap_GetBits(new_dib);

		const unsigned src_pitch = Bitmap_GetPitch(dib);
		const unsigned dst_pitch = Bitmap_GetPitch(new_dib);

		switch(bpp) {
			case 1:
			{
				for (unsigned y = 0; y < height; y++) {
					for (unsigned x = 0; x < width; x++) {
						const unsigned pixel = (src_bits[x >> 3] & (0x80 >> (x & 0x07))) != 0;
						dst_bits[x] = grey_pal[pixel];
					}
					src_bits += src_pitch;
					dst_bits += dst_pitch;
				}
			}
			break;

			case 4:
			{
				for (unsigned y = 0; y < height; y++) {
					for (unsigned x = 0; x < width; x++) {
						const unsigned pixel = x & 0x01 ? src_bits[x >> 1] & 0x0F : src_bits[x >> 1] >> 4;
						dst_bits[x] = grey_pal[pixel];
					}
					src_bits += src_pitch;
					dst_bits += dst_pitch;
				}
			}
			break;

			case 8:
			{
				for (unsigned y = 0; y < height; y++) {
					for (unsigned x = 0; x < width; x++) {
						dst_bits[x] = grey_pal[src_bits[x]];
					}
					src_bits += src_pitch;
					dst_bits += dst_pitch;
				}
			}
			break;
		}
		return new_dib;
	} 
	
	// Convert the bitmap to 8-bit greyscale
	return Bitmap_ConvertTo8Bits(dib);
}
