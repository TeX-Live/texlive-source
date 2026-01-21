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
// Upsampling / downsampling classes
//
// ==========================================================

#include <stdlib.h>
#include <memory.h>
#include "bitmap.h"


static unsigned
CalculateUsedPaletteEntries(unsigned bit_count) {
	if ((bit_count >= 1) && (bit_count <= 8))
		return 1 << bit_count;

	return 0;
}


/**
Returns the color type of a bitmap. In contrast to FreeImage_GetColorType,
this function optionally supports a boolean OUT parameter, that receives TRUE,
if the specified bitmap is greyscale, that is, it consists of grey colors only.
Although it returns the same value as returned by FreeImage_GetColorType for all
image types, this extended function primarily is intended for palletized images,
since the boolean pointed to by 'bIsGreyscale' remains unchanged for RGB(A/F)
images. However, the outgoing boolean is properly maintained for palletized images,
as well as for any non-RGB image type, like FIT_UINTxx and FIT_DOUBLE, for example.
@param dib A pointer to a FreeImage bitmap to calculate the extended color type for
@param bIsGreyscale A pointer to a boolean, that receives TRUE, if the specified bitmap
is greyscale, that is, it consists of grey colors only. This parameter can be NULL.
@return the color type of the specified bitmap
*/
static BITMAP_COLOR_TYPE
GetExtendedColorType(FIBITMAP *dib, BOOL *bIsGreyscale) {
	const unsigned bpp = Bitmap_GetBPP(dib);
	const unsigned size = CalculateUsedPaletteEntries(bpp);
	const RGBQUAD * const pal = Bitmap_GetPalette(dib);
	BITMAP_COLOR_TYPE color_type = FIC_MINISBLACK;
	BOOL bIsGrey = TRUE;

	switch (bpp) {
		case 1:
		{
			for (unsigned i = 0; i < size; i++) {
				if ((pal[i].rgbRed != pal[i].rgbGreen) || (pal[i].rgbRed != pal[i].rgbBlue)) {
					color_type = FIC_PALETTE;
					bIsGrey = FALSE;
					break;
				}
			}
			if (bIsGrey) {
				if (pal[0].rgbBlue == 255 && pal[1].rgbBlue == 0) {
					color_type = FIC_MINISWHITE;
				} else if (pal[0].rgbBlue != 0 || pal[1].rgbBlue != 255) {
					color_type = FIC_PALETTE;
				}
			}
			break;
		}

		case 4:
		case 8:
		{
			for (unsigned i = 0; i < size; i++) {
				if ((pal[i].rgbRed != pal[i].rgbGreen) || (pal[i].rgbRed != pal[i].rgbBlue)) {
					color_type = FIC_PALETTE;
					bIsGrey = FALSE;
					break;
				}
				if (color_type != FIC_PALETTE && pal[i].rgbBlue != i) {
					if ((size - i - 1) != pal[i].rgbBlue) {
						color_type = FIC_PALETTE;
						if (!bIsGreyscale) {
							// exit loop if we're not setting
							// bIsGreyscale parameter
							break;
						}
					} else {
						color_type = FIC_MINISWHITE;
					}
				}
			}
			break;
		}

		default:
		{
			color_type = Bitmap_GetColorType(dib);
			bIsGrey = (color_type == FIC_MINISBLACK) ? TRUE : FALSE;
			break;
		}

	}
	if (bIsGreyscale) {
		*bIsGreyscale = bIsGrey;
	}

	return color_type;
}

/**
Returns a pointer to an RGBA palette, created from the specified bitmap.
The RGBA palette is a copy of the specified bitmap's palette, that, additionally
contains the bitmap's transparency information in the rgbReserved member
of the palette's RGBQUAD elements.
@param dib A pointer to a FreeImage bitmap to create the RGBA palette from.
@param buffer A pointer to the buffer to store the RGBA palette.
@return A pointer to the newly created RGBA palette or NULL, if the specified
bitmap is no palletized standard bitmap. If non-NULL, the returned value is
actually the pointer passed in parameter 'buffer'.
*/
static inline RGBQUAD *
GetRGBAPalette(FIBITMAP *dib, RGBQUAD * const buffer) {
	// clone the palette
	const unsigned ncolors = Bitmap_GetColorsUsed(dib);
	if (ncolors == 0) {
		return NULL;
	}
	memcpy(buffer, Bitmap_GetPalette(dib), ncolors * sizeof(RGBQUAD));
	// merge the transparency table
	unsigned tcount = Bitmap_GetTransparencyCount(dib);
	const unsigned ntransp = ncolors < tcount ? ncolors : tcount;
	const BYTE * const tt = Bitmap_GetTransparencyTable(dib);
	for (unsigned i = 0; i < ntransp; i++) {
		buffer[i].rgbReserved = tt[i];
	}
	for (unsigned i = ntransp; i < ncolors; i++) {
		buffer[i].rgbReserved = 255;
	}
	return buffer;
}

/**
 This function performs zoom without filtering. It scales an image to the desired dimensions
 simply removing/adding lines and columns.<br>
 It works with 1-, 8-, 24- and 32-bit buffers.<br><br>

*/

static FIBITMAP* Sample(FIBITMAP *src, unsigned dst_width, unsigned dst_height, unsigned src_left, unsigned src_top, unsigned src_width, unsigned src_height) { 

	const BITMAP_TYPE image_type = Bitmap_GetImageType(src);
	const unsigned src_bpp = Bitmap_GetBPP(src);
	unsigned x, y;

	// determine the image's color type
	BOOL bIsGreyscale = FALSE;
	BITMAP_COLOR_TYPE color_type;
	if (src_bpp <= 8) {
		color_type = GetExtendedColorType(src, &bIsGreyscale);
	} else {
		color_type = FIC_RGB;
	}

	// determine the required bit depth of the destination image
	unsigned dst_bpp;
	if (color_type == FIC_PALETTE && !bIsGreyscale) {
		// non greyscale FIC_PALETTE images require a high-color destination
		// image (24- or 32-bits depending on the image's transparent state)
		dst_bpp = Bitmap_IsTransparent(src) ? 32 : 24;
	} else if ((src_bpp <= 8) && (src_bpp >= 2)) {
		// greyscale images require an 8-bit destination image
		// (or a 32-bit image if the image is transparent)
		dst_bpp = Bitmap_IsTransparent(src) ? 32 : 8;
	} else if (src_bpp == 16 && image_type == FIT_BITMAP) {
		// 16-bit 555 and 565 RGB images require a high-color destination image
		// (fixed to 24 bits, since 16-bit RGBs don't support transparency in FreeImage)
		dst_bpp = 24;
	} else {
		// bit depth remains unchanged for all other images
		dst_bpp = src_bpp;
	}

	// early exit if destination size is equal to source size
	if ((src_width == dst_width) && (src_height == dst_height)) {
		FIBITMAP *out = src;
		FIBITMAP *tmp = src;
		if ((src_width != Bitmap_GetWidth(src)) || (src_height != Bitmap_GetHeight(src))) {
			out = Bitmap_Copy(tmp, src_left, src_top, src_left + src_width, src_top + src_height);
			tmp = out;
		}
		if (src_bpp != dst_bpp) {
			switch (dst_bpp) {
				case 8:
					out = Bitmap_ConvertToGreyscale(tmp);
					if (tmp != src) {
						Bitmap_Unload(tmp);
					}
					break;

				case 24:
					out = Bitmap_ConvertTo24Bits(tmp);
					if (tmp != src) {
						Bitmap_Unload(tmp);
					}
					break;

				case 32:
					out = Bitmap_ConvertTo32Bits(tmp);
					if (tmp != src) {
						Bitmap_Unload(tmp);
					}
					break;
			}
		}

		return (out != src) ? out : Bitmap_Clone(src);
	}

	RGBQUAD pal_buffer[256];
	RGBQUAD *src_pal = NULL;

	// provide the source image's palette to the rescaler for
	// FIC_PALETTE type images (this includes palletized greyscale
	// images with an unordered palette)
	if (color_type == FIC_PALETTE) {
		if (dst_bpp == 32) {
			// a 32 bit destination image signals transparency, so
			// create an RGBA palette from the source palette
			src_pal = GetRGBAPalette(src, pal_buffer);
		} else {
			src_pal = Bitmap_GetPalette(src);
		}
	}

	// allocate the dst image
	FIBITMAP *dst = Bitmap_AllocateT(image_type, dst_width, dst_height, dst_bpp, 0, 0, 0);
	if (!dst) {
		return NULL;
	}
	FIICCPROFILE *icc_src = Bitmap_GetICCProfile(src);
	if ( icc_src == NULL ) {
		if (Bitmap_GetColorType(src) == FIC_CMYK) {
			FIICCPROFILE *icc = Bitmap_CreateICCProfile(dst, NULL, 0);
			icc->flags = FIICC_COLOR_IS_CMYK;
		}
	}
	else {
		if ( dst_bpp == src_bpp ) {
			FIICCPROFILE *icc = Bitmap_CreateICCProfile(dst, icc_src->data, icc_src->size);
			icc->flags = icc_src->flags;
		}
	}
	

	switch ( dst_bpp ) {
	case 1:
		// Build a monochrome palette
		{
			RGBQUAD *pal = Bitmap_GetPalette(dst);
			pal[0].rgbRed = pal[0].rgbGreen = pal[0].rgbBlue = 0;
			pal[1].rgbRed = pal[1].rgbGreen = pal[1].rgbBlue = 255;
		}
		break;
	case 8:
		{
			RGBQUAD * const dst_pal = Bitmap_GetPalette(dst);
			if (color_type == FIC_MINISWHITE) {
				// build an inverted greyscale palette
				for (unsigned i = 0, v = 0x00FFFFFF; i < 256; i++, v -= (0x00FFFFFF / (256 - 1))) {
					((unsigned *)dst_pal)[i] = v;
				}		
			} 
			/*
			else {
				// build a default greyscale palette
				// Currently, FreeImage_AllocateT already creates a default
				// greyscale palette for 8 bpp images, so we can skip this here.
				CREATE_GREYSCALE_PALETTE(dst_pal, 256);
			}
			*/
		}
	}

	// calculate x and y offsets; since FreeImage uses bottom-up bitmaps, the
	// value of src_offset_y is measured from the bottom of the image
	unsigned src_offset_x = src_left;
	unsigned src_offset_y;
	if (src_top > 0) {
		src_offset_y = Bitmap_GetHeight(src) - src_height - src_top;
	} else {
		src_offset_y = 0;
	}

	//Allocate line and column offset buffers.
	double *x_offset = (double *) malloc(dst_width*sizeof(double));
	double *y_offset = (double *) malloc(dst_height*sizeof(double));
	if ((x_offset == NULL) || (y_offset == NULL)) {
		Bitmap_Unload(dst);
		return NULL;
	}

	//Initialize pixel offsets.
	for (x=0; x < dst_width; x++)
		x_offset[x] = x*src_width/(double) dst_width + src_offset_x;
	for (y=0; y < dst_height; y++)
		y_offset[y] = y*src_height/(double) dst_height + src_offset_y;

	switch (Bitmap_GetImageType(src)) {
	case FIT_BITMAP:
		{
			switch (Bitmap_GetBPP(src)) {
			case 1:
			{
				for(y = 0; y < dst_height; y++) {
					// scale each row 
					BYTE *src_bits = Bitmap_GetScanLine(src, (unsigned)y_offset[y]);
					BYTE *dst_bits = Bitmap_GetScanLine(dst, y);

					for(x = 0; x < dst_width; x++) {
						// loop through row
						BYTE pixel = (src_bits[(unsigned)x_offset[x] >> 3] & (0x80 >> ((unsigned)x_offset[x] & 0x07))) != 0;
						if(pixel > 0) {
							// Set bit(x, y) to 0
							dst_bits[x >> 3] &= (0xFF7F >> (x & 0x7));
						} else {
							// Set bit(x, y) to 1
							dst_bits[x >> 3] |= (0x80 >> (x & 0x7));
						}
					} 
				}
			}
			break;
		case 8:
		case 24:
		case 32:
			{
			// Calculate the number of bytes per pixel (1 for 8-bit, 3 for 24-bit or 4 for 32-bit)
			unsigned bytespp = Bitmap_GetLine(src) / Bitmap_GetWidth(src);

			for(y = 0; y < dst_height; y++) {
				// scale each row 
				BYTE *src_bits = Bitmap_GetScanLine(src, (unsigned)y_offset[y]);
				BYTE *dst_bits = Bitmap_GetScanLine(dst, y);

				for(x = 0; x < dst_width; x++) {
					// loop through row
					for (unsigned j = 0; j < bytespp; j++) {
						dst_bits[j] = src_bits[(unsigned)x_offset[x] * bytespp + j];
					}
					dst_bits += bytespp;
				} 
			}
			}
		}
		}
		break;
		case FIT_UNKNOWN:
            break;
	}

	free(x_offset);
	free(y_offset);
	return dst;
}

FIBITMAP *Bitmap_Resample(FIBITMAP *src, int dst_width, int dst_height) {
	FIBITMAP *dst = NULL;

	if (!Bitmap_HasPixels(src) || (dst_width <= 0) || (dst_height <= 0) || (Bitmap_GetWidth(src) <= 0) || (Bitmap_GetHeight(src) <= 0)) {
		return NULL;
	}

	dst = Sample(src, dst_width, dst_height, 0, 0,
		Bitmap_GetWidth(src), Bitmap_GetHeight(src));
	
	return dst;
}
