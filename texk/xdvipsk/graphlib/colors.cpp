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
// Color manipulation routines
//
// ==========================================================

#include "bitmap.h"
#include "utilities.h"

// ----------------------------------------------------------
//   Macros + structures
// ----------------------------------------------------------

#define GET_HI_NIBBLE(byte)     ((byte) >> 4)
#define SET_HI_NIBBLE(byte, n)  byte &= 0x0F, byte |= ((n) << 4)
#define GET_LO_NIBBLE(byte)     ((byte) & 0x0F)
#define SET_LO_NIBBLE(byte, n)  byte &= 0xF0, byte |= ((n) & 0x0F)
#define GET_NIBBLE(cn, byte)    ((cn) ? (GET_HI_NIBBLE(byte)) : (GET_LO_NIBBLE(byte))) 
#define SET_NIBBLE(cn, byte, n) if (cn) SET_HI_NIBBLE(byte, n); else SET_LO_NIBBLE(byte, n) 

// ----------------------------------------------------------


/** @brief Inverts each pixel data.

@param src Input image to be processed.
@return Returns TRUE if successful, FALSE otherwise.
*/
BOOL
Bitmap_Invert(FIBITMAP *src) {

	if (!Bitmap_HasPixels(src)) return FALSE;
	
	unsigned i, x, y, k;
	
	const unsigned width = Bitmap_GetWidth(src);
	const unsigned height = Bitmap_GetHeight(src);
	const unsigned bpp = Bitmap_GetBPP(src);

	BITMAP_TYPE image_type = Bitmap_GetImageType(src);

	if(image_type == FIT_BITMAP) {
		switch(bpp) {
			case 1 :
			case 4 :
			case 8 :
			{
				// if the dib has a colormap, just invert it
				// else, keep the linear grayscale

				if (Bitmap_GetColorType(src) == FIC_PALETTE) {
					RGBQUAD *pal = Bitmap_GetPalette(src);

					for (i = 0; i < Bitmap_GetColorsUsed(src); i++) {
						pal[i].rgbRed	= 255 - pal[i].rgbRed;
						pal[i].rgbGreen = 255 - pal[i].rgbGreen;
						pal[i].rgbBlue	= 255 - pal[i].rgbBlue;
					}
				} else {
					for(y = 0; y < height; y++) {
						BYTE *bits = Bitmap_GetScanLine(src, y);

						for (x = 0; x < Bitmap_GetLine(src); x++) {
							bits[x] = ~bits[x];
						}
					}
				}

				break;
			}		

			case 24 :
			case 32 :
			{
				// Calculate the number of bytes per pixel (3 for 24-bit or 4 for 32-bit)
				const unsigned bytespp = Bitmap_GetLine(src) / width;

				for(y = 0; y < height; y++) {
					BYTE *bits = Bitmap_GetScanLine(src, y);
					for(x = 0; x < width; x++) {
						for(k = 0; k < bytespp; k++) {
							bits[k] = ~bits[k];
						}
						bits += bytespp;
					}
				}

				break;
			}
			default:
				return FALSE;
		}
	}
	else {
		// anything else ... 
		return FALSE;
	}
		
	return TRUE;
}
