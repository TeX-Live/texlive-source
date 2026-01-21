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


#ifdef _MSC_VER 
#pragma warning (disable : 4786) // identifier was truncated to 'number' characters
#endif 

#include <stdlib.h>
#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW32__)
#include <malloc.h>
#endif // _WIN32 || _WIN64 || __MINGW32__

#include "bitmap.h"
#include "bitmapIO.h"
#include "utilities.h"
#include "loaders.h"


/**
Constants for the BITMAPINFOHEADER::biCompression field
BI_RGB:
The bitmap is in uncompressed red green blue (RGB) format that is not compressed and does not use color masks.
BI_BITFIELDS:
The bitmap is not compressed and the color table consists of three DWORD color masks that specify the red, green, and blue components, 
respectively, of each pixel. This is valid when used with 16 and 32-bits per pixel bitmaps.
*/
#ifndef _WINGDI_
#define BI_RGB       0L
#define BI_BITFIELDS 3L
#endif // _WINGDI_

// ----------------------------------------------------------
//  FIBITMAP definition
// ----------------------------------------------------------

/**
FreeImage header structure
*/
FI_STRUCT (BITMAPHEADER) {
	/** data type - bitmap, array of long, double, complex, etc */
	BITMAP_TYPE type;

	/** background color used for RGB transparency */
	RGBQUAD bkgnd_color;

	/**@name transparency management */
	//@{
	/**
	why another table ? for easy transparency table retrieval !
	transparency could be stored in the palette, which is better
	overall, but it requires quite some changes and it will render
	FreeImage_GetTransparencyTable obsolete in its current form;
	*/
	BYTE transparent_table[256];
	/** number of transparent colors */
	int  transparency_count;
	/** TRUE if the image is transparent */
	BOOL transparent;
	//@}

	/** space to hold ICC profile */
	FIICCPROFILE iccProfile;

	/** FALSE if the FIBITMAP only contains the header and no pixel data */
	BOOL has_pixels;

	//BYTE filler[1];			 // fill to 32-bit alignment
};

// ----------------------------------------------------------
//  FREEIMAGERGBMASKS definition
// ----------------------------------------------------------

/**
RGB mask structure - mainly used for 16-bit RGB555 / RGB 565 FIBITMAP
*/
FI_STRUCT (BITMAPRGBMASKS) {
	unsigned red_mask;		//! bit layout of the red components
	unsigned green_mask;	//! bit layout of the green components
	unsigned blue_mask;		//! bit layout of the blue components
};

// ----------------------------------------------------------
//  Memory allocation on a specified alignment boundary
// ----------------------------------------------------------

#if (defined(_WIN32) || defined(_WIN64)) && !defined(__MINGW32__)

void* Bitmap_Aligned_Malloc(size_t amount, size_t alignment) {
	assert(alignment == FIBITMAP_ALIGNMENT);
	return _aligned_malloc(amount, alignment);
}

void Bitmap_Aligned_Free(void* mem) {
	_aligned_free(mem);
}

#elif defined (__MINGW32__)

void* Bitmap_Aligned_Malloc(size_t amount, size_t alignment) {
	assert(alignment == FIBITMAP_ALIGNMENT);
	return __mingw_aligned_malloc (amount, alignment);
}

void Bitmap_Aligned_Free(void* mem) {
	__mingw_aligned_free (mem);
}

#else

void* Bitmap_Aligned_Malloc(size_t amount, size_t alignment) {
	assert(alignment == FIBITMAP_ALIGNMENT);
	/*
	In some rare situations, the malloc routines can return misaligned memory. 
	The routine FreeImage_Aligned_Malloc allocates a bit more memory to do
	aligned writes.  Normally, it *should* allocate "alignment" extra memory and then writes
	one dword back the true pointer.  But if the memory manager returns a
	misaligned block that is less than a dword from the next alignment, 
	then the writing back one dword will corrupt memory.

	For example, suppose that alignment is 16 and malloc returns the address 0xFFFF.

	16 - 0xFFFF % 16 + 0xFFFF = 16 - 15 + 0xFFFF = 0x10000.

	Now, you subtract one dword from that and write and that will corrupt memory.

	That's why the code below allocates *two* alignments instead of one. 
	*/
	void* mem_real = malloc(amount + 2 * alignment);
	if(!mem_real) return NULL;
	char* mem_align = (char*)((unsigned long)(2 * alignment - (unsigned long)mem_real % (unsigned long)alignment) + (unsigned long)mem_real);
	*((long*)mem_align - 1) = (long)mem_real;
	return mem_align;
}

void Bitmap_Aligned_Free(void* mem) {
	free((void*)*((long*)mem - 1));
}

#endif // _WIN32 || _WIN64

// ----------------------------------------------------------
//  FIBITMAP memory management
// ----------------------------------------------------------

/**
Calculate the size of a FreeImage image. 
Align the palette and the pixels on a FIBITMAP_ALIGNMENT bytes alignment boundary.
This function includes a protection against malicious images, based on a KISS integer overflow detection mechanism. 

@param header_only If TRUE, calculate a 'header only' FIBITMAP size, otherwise calculate a full FIBITMAP size
@param width Image width
@param height Image height
@param bpp Number of bits-per-pixel
@param need_masks We only store the masks (and allocate memory for them) for 16-bit images of type FIT_BITMAP
@return Returns a size in BYTE units
@see FreeImage_AllocateBitmap
*/
static size_t 
Bitmap_GetInternalImageSize(BOOL header_only, unsigned width, unsigned height, unsigned bpp, BOOL need_masks) {
	size_t dib_size = sizeof(BITMAPHEADER);
	dib_size += (dib_size % FIBITMAP_ALIGNMENT ? FIBITMAP_ALIGNMENT - dib_size % FIBITMAP_ALIGNMENT : 0);
	dib_size += FIBITMAP_ALIGNMENT - sizeof(BITMAPINFOHEADER) % FIBITMAP_ALIGNMENT;
	dib_size += sizeof(BITMAPINFOHEADER);
	// palette is aligned on a 16 bytes boundary
	dib_size += sizeof(RGBQUAD) * CalculateUsedPaletteEntries(bpp);
	// we both add palette size and masks size if need_masks is true, since CalculateUsedPaletteEntries
	// always returns 0 if need_masks is true (which is only true for 16 bit images).
	dib_size += need_masks ? sizeof(DWORD) * 3 : 0;
	dib_size += (dib_size % FIBITMAP_ALIGNMENT ? FIBITMAP_ALIGNMENT - dib_size % FIBITMAP_ALIGNMENT : 0);

	if(!header_only) {
		const size_t header_size = dib_size;

		// pixels are aligned on a 16 bytes boundary
		dib_size += (size_t)CalculatePitch(CalculateLine(width, bpp)) * (size_t)height;

		// check for possible malloc overflow using a KISS integer overflow detection mechanism
		{
			const double dPitch = floor( ((double)bpp * width + 31.0) / 32.0 ) * 4.0;
			const double dImageSize = (double)header_size + dPitch * height;
			if(dImageSize != (double)dib_size) {
				// here, we are sure to encounter a malloc overflow: try to avoid it ...
				return 0;
			}

			/*
			The following constant take into account the additionnal memory used by 
			aligned malloc functions as well as debug malloc functions. 
			It is supposed here that using a (8 * FIBITMAP_ALIGNMENT) risk margin will be enough
			for the target compiler. 
			*/
			const double FIBITMAP_MAX_MEMORY = (double)((size_t)-1) - 8 * FIBITMAP_ALIGNMENT;

			if(dImageSize > FIBITMAP_MAX_MEMORY) {
				// avoid possible overflow inside C allocation functions
				return 0;
			}
		}
	}

	return dib_size;
}

/**
Helper for 16-bit FIT_BITMAP
Returns a pointer to the bitmap's red-, green- and blue masks.
@param dib The bitmap to obtain masks from.
@return Returns a pointer to the bitmap's red-, green- and blue masks
or NULL, if no masks are present (e.g. for 24 bit images).
*/
static BITMAPRGBMASKS *
Bitmap_GetRGBMasks(FIBITMAP *dib) {
	return Bitmap_HasRGBMasks(dib) ? (BITMAPRGBMASKS *)(((BYTE *)Bitmap_GetInfoHeader(dib)) + sizeof(BITMAPINFOHEADER)) : NULL;
}

/**
Internal FIBITMAP allocation.

This function accepts (ext_bits, ext_pitch) arguments. If these are provided the FIBITMAP 
will be allocated as "header only", but bits and pitch will be stored within the FREEIMAGEHEADER 
and the resulting FIBITMAP will have pixels, i.e. HasPixels() will return TRUE.
- GetBits() and GetPitch return the correct values - either offsets or the stored values (user-provided bits and pitch).
- Clone() creates a new FIBITMAP with copy of the user pixel data.
- Unload's implementation does not need to change - it just release a "header only" dib.
Note that when using external data, the data does not need to have the same alignment as the default 4-byte alignment. 
This enables the possibility to access buffers with, for instance, stricter alignment,
like the ones used in low-level APIs like OpenCL or intrinsics.

@param header_only If TRUE, allocate a 'header only' FIBITMAP, otherwise allocate a full FIBITMAP
@param ext_bits Pointer to external user's pixel buffer if using wrapped buffer, NULL otherwise
@param ext_pitch Pointer to external user's pixel buffer pitch if using wrapped buffer, 0 otherwise
@param type Image type
@param width Image width
@param height Image height
@param bpp Number of bits per pixel
@param red_mask Image red mask 
@param green_mask Image green mask
@param blue_mask Image blue mask
@return Returns the allocated FIBITMAP if successful, returns NULL otherwise
*/
static FIBITMAP * 
Bitmap_AllocateBitmap(BOOL header_only, BYTE *ext_bits, unsigned ext_pitch, BITMAP_TYPE type, int width, int height, int bpp, unsigned red_mask, unsigned green_mask, unsigned blue_mask) {

	// check input variables
	width = abs(width);
	height = abs(height);
	if(!((width > 0) && (height > 0))) {
		return NULL;
	}
	if(ext_bits) {
		if(ext_pitch == 0) {
			return NULL;
		}
		assert(header_only == FALSE);
	}

	// we only store the masks (and allocate memory for them) for 16-bit images of type FIT_BITMAP
	BOOL need_masks = FALSE;

	// check pixel bit depth
	switch(type) {
		case FIT_BITMAP:
			switch(bpp) {
				case 1:
				case 4:
				case 8:
					break;
				case 16:
					need_masks = TRUE;
					break;
				case 24:
				case 32:
					break;
				default:
					bpp = 8;
					break;
			}
			break;
		default:
			return NULL;
	}

	FIBITMAP *bitmap = (FIBITMAP *)malloc(sizeof(FIBITMAP));

	if (bitmap != NULL) {

		// calculate the size of a FreeImage image
		// align the palette and the pixels on a FIBITMAP_ALIGNMENT bytes alignment boundary
		// palette is aligned on a 16 bytes boundary
		// pixels are aligned on a 16 bytes boundary

		// when using a user provided pixel buffer, force a 'header only' allocation

		size_t dib_size = Bitmap_GetInternalImageSize(header_only || ext_bits, width, height, bpp, need_masks);

		if(dib_size == 0) {
			// memory allocation will fail (probably a malloc overflow)
			free(bitmap);
			return NULL;
		}

		bitmap->data = (BYTE *)Bitmap_Aligned_Malloc(dib_size * sizeof(BYTE), FIBITMAP_ALIGNMENT);

		if (bitmap->data != NULL) {
			memset(bitmap->data, 0, dib_size);

			// write out the FREEIMAGEHEADER

			BITMAPHEADER *fih = (BITMAPHEADER *)bitmap->data;

			fih->type = type;

			memset(&fih->bkgnd_color, 0, sizeof(RGBQUAD));

			fih->transparent = FALSE;
			fih->transparency_count = 0;
			memset(fih->transparent_table, 0xff, 256);

			fih->has_pixels = header_only ? FALSE : TRUE;

			// initialize FIICCPROFILE link

			FIICCPROFILE *iccProfile = Bitmap_GetICCProfile(bitmap);
			iccProfile->size = 0;
			iccProfile->data = 0;
			iccProfile->flags = 0;

			// write out the BITMAPINFOHEADER

			BITMAPINFOHEADER *bih = Bitmap_GetInfoHeader(bitmap);
			bih->biSize             = sizeof(BITMAPINFOHEADER);
			bih->biWidth            = width;
			bih->biHeight           = height;
			bih->biPlanes           = 1;
			bih->biCompression      = need_masks ? BI_BITFIELDS : BI_RGB;
			bih->biBitCount         = (WORD)bpp;
			bih->biClrUsed          = CalculateUsedPaletteEntries(bpp);
			bih->biClrImportant     = bih->biClrUsed;
			bih->biXPelsPerMeter	= 2835;	// 72 dpi
			bih->biYPelsPerMeter	= 2835;	// 72 dpi

			if(bpp == 8) {
				// build a default greyscale palette (very useful for image processing)
				RGBQUAD *pal = Bitmap_GetPalette(bitmap);
				for(int i = 0; i < 256; i++) {
					pal[i].rgbRed	= (BYTE)i;
					pal[i].rgbGreen = (BYTE)i;
					pal[i].rgbBlue	= (BYTE)i;
				}
			}

			// just setting the masks (only if needed) just like the palette.
			if (need_masks) {
				BITMAPRGBMASKS *masks = Bitmap_GetRGBMasks(bitmap);
				masks->red_mask = red_mask;
				masks->green_mask = green_mask;
				masks->blue_mask = blue_mask;
			}

			return bitmap;
		}

		free(bitmap);
	}

	return NULL;
}

FIBITMAP *
Bitmap_AllocateHeaderT(BOOL header_only, BITMAP_TYPE type, int width, int height, int bpp, unsigned red_mask, unsigned green_mask, unsigned blue_mask) {
	return Bitmap_AllocateBitmap(header_only, NULL, 0, type, width, height, bpp, red_mask, green_mask, blue_mask);
}

FIBITMAP *
Bitmap_AllocateHeader(BOOL header_only, int width, int height, int bpp, unsigned red_mask, unsigned green_mask, unsigned blue_mask) {
	return Bitmap_AllocateBitmap(header_only, NULL, 0, FIT_BITMAP, width, height, bpp, red_mask, green_mask, blue_mask);
}

FIBITMAP *
Bitmap_Allocate(int width, int height, int bpp, unsigned red_mask, unsigned green_mask, unsigned blue_mask) {
	return Bitmap_AllocateBitmap(FALSE, NULL, 0, FIT_BITMAP, width, height, bpp, red_mask, green_mask, blue_mask);
}

FIBITMAP *
Bitmap_AllocateT(BITMAP_TYPE type, int width, int height, int bpp, unsigned red_mask, unsigned green_mask, unsigned blue_mask) {
	return Bitmap_AllocateBitmap(FALSE, NULL, 0, type, width, height, bpp, red_mask, green_mask, blue_mask);
}

void
Bitmap_Unload(FIBITMAP *dib) {
	if (NULL != dib) {	
		if (NULL != dib->data) {
			// delete possible icc profile ...
			if (Bitmap_GetICCProfile(dib)->data) {
				free(Bitmap_GetICCProfile(dib)->data);
			}

			// delete bitmap ...
			Bitmap_Aligned_Free(dib->data);
		}

		free(dib);		// ... and the wrapper
	}
}

// ----------------------------------------------------------

FIBITMAP *
Bitmap_Clone(FIBITMAP *dib) {
	if(!dib) {
		return NULL;
	}

	BITMAP_TYPE type = Bitmap_GetImageType(dib);
	unsigned width = Bitmap_GetWidth(dib);
	unsigned height = Bitmap_GetHeight(dib);
	unsigned bpp = Bitmap_GetBPP(dib);
	
	// check for pixel availability ...
	BOOL header_only = Bitmap_HasPixels(dib) ? FALSE : TRUE;

	// check whether this image has masks defined ...
	BOOL need_masks = (bpp == 16 && type == FIT_BITMAP) ? TRUE : FALSE;

	// allocate a new dib
	FIBITMAP *new_dib = Bitmap_AllocateHeaderT(header_only, type, width, height, bpp,
		Bitmap_GetRedMask(dib), Bitmap_GetGreenMask(dib), Bitmap_GetBlueMask(dib));

	if (new_dib) {
		// save ICC profile links
		FIICCPROFILE *src_iccProfile = Bitmap_GetICCProfile(dib);
		FIICCPROFILE *dst_iccProfile = Bitmap_GetICCProfile(new_dib);

		// calculate the size of the src image
		// align the palette and the pixels on a FIBITMAP_ALIGNMENT bytes alignment boundary
		// palette is aligned on a 16 bytes boundary
		// pixels are aligned on a 16 bytes boundary
		
		// when using a user provided pixel buffer, force a 'header only' calculation		

		size_t dib_size = Bitmap_GetInternalImageSize(header_only, width, height, bpp, need_masks);

		// copy the bitmap + internal pointers (remember to restore new_dib internal pointers later)
		memcpy(new_dib->data, dib->data, dib_size);

		// reset ICC profile link for new_dib
		memset(dst_iccProfile, 0, sizeof(FIICCPROFILE));

		// copy possible ICC profile
		Bitmap_CreateICCProfile(new_dib, src_iccProfile->data, src_iccProfile->size);
		dst_iccProfile->flags = src_iccProfile->flags;

		return new_dib;
	}

	return NULL;
}

// ----------------------------------------------------------

BYTE *
Bitmap_GetBits(FIBITMAP *dib) {
	if (!Bitmap_HasPixels(dib)) {
		return NULL;
	}

	// returns the pixels aligned on a FIBITMAP_ALIGNMENT bytes alignment boundary
	size_t lp = (size_t)Bitmap_GetInfoHeader(dib);
	lp += sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * Bitmap_GetColorsUsed(dib);
	lp += Bitmap_HasRGBMasks(dib) ? sizeof(DWORD) * 3 : 0;
	lp += (lp % FIBITMAP_ALIGNMENT ? FIBITMAP_ALIGNMENT - lp % FIBITMAP_ALIGNMENT : 0);
	return (BYTE *)lp;
}

BYTE *
Bitmap_GetScanLine(FIBITMAP *dib, int scanline) {
	if (!Bitmap_HasPixels(dib)) {
		return NULL;
	}
	return CalculateScanLine(Bitmap_GetBits(dib), Bitmap_GetPitch(dib), scanline);
}

// ----------------------------------------------------------
//  DIB information functions
// ----------------------------------------------------------

BITMAP_COLOR_TYPE
Bitmap_GetColorType(FIBITMAP *dib) {
	RGBQUAD *rgb;

	const BITMAP_TYPE image_type = Bitmap_GetImageType(dib);

	// special bitmap type
	if(image_type != FIT_BITMAP) {
		return FIC_MINISBLACK;
	}

	// standard image type
	switch (Bitmap_GetBPP(dib)) {
		case 1:
		{
			rgb = Bitmap_GetPalette(dib);

			if ((rgb->rgbRed == 0) && (rgb->rgbGreen == 0) && (rgb->rgbBlue == 0)) {
				rgb++;

				if ((rgb->rgbRed == 255) && (rgb->rgbGreen == 255) && (rgb->rgbBlue == 255)) {
					return FIC_MINISBLACK;
				}
			}

			if ((rgb->rgbRed == 255) && (rgb->rgbGreen == 255) && (rgb->rgbBlue == 255)) {
				rgb++;

				if ((rgb->rgbRed == 0) && (rgb->rgbGreen == 0) && (rgb->rgbBlue == 0)) {
					return FIC_MINISWHITE;
				}
			}

			return FIC_PALETTE;
		}

		case 4:
		case 8:	// Check if the DIB has a color or a greyscale palette
		{
			int ncolors = Bitmap_GetColorsUsed(dib);
		    int minisblack = 1;
			rgb = Bitmap_GetPalette(dib);

			for (int i = 0; i < ncolors; i++) {
				if ((rgb->rgbRed != rgb->rgbGreen) || (rgb->rgbRed != rgb->rgbBlue)) {
					return FIC_PALETTE;
				}

				// The DIB has a color palette if the greyscale isn't a linear ramp
				// Take care of reversed grey images
				if (rgb->rgbRed != i) {
					if ((ncolors-i-1) != rgb->rgbRed) {
						return FIC_PALETTE;
					} else {
						minisblack = 0;
					}
				}

				rgb++;
			}

			return minisblack ? FIC_MINISBLACK : FIC_MINISWHITE;
		}

		case 16:
		case 24:
			return FIC_RGB;

		case 32:
		{
			if (Bitmap_GetICCProfile(dib)->flags & FIICC_COLOR_IS_CMYK) {
				return FIC_CMYK;
			}

			if (Bitmap_HasPixels(dib)) {
				// check for fully opaque alpha layer
				for (unsigned y = 0; y < Bitmap_GetHeight(dib); y++) {
					rgb = (RGBQUAD *)Bitmap_GetScanLine(dib, y);

					for (unsigned x = 0; x < Bitmap_GetWidth(dib); x++) {
						if (rgb[x].rgbReserved != 0xFF) {
							return FIC_RGBALPHA;
						}
					}
				}
				return FIC_RGB;
			}

			return FIC_RGBALPHA;
		}
				
		default :
			return FIC_MINISBLACK;
	}
}

// ----------------------------------------------------------

BITMAP_TYPE
Bitmap_GetImageType(FIBITMAP *dib) {
	return (dib != NULL) ? ((BITMAPHEADER *)dib->data)->type : FIT_UNKNOWN;
}

// ----------------------------------------------------------

BOOL
Bitmap_HasPixels(FIBITMAP *dib) {
	return (dib != NULL) ? ((BITMAPHEADER *)dib->data)->has_pixels : FALSE;
}

// ----------------------------------------------------------

BOOL
Bitmap_HasRGBMasks(FIBITMAP *dib) {
	return dib && Bitmap_GetInfoHeader(dib)->biCompression == BI_BITFIELDS;
}

unsigned
Bitmap_GetRedMask(FIBITMAP *dib) {
	BITMAPRGBMASKS *masks = NULL;
	BITMAP_TYPE image_type = Bitmap_GetImageType(dib);
	switch(image_type) {
		case FIT_BITMAP:
			// check for 16-bit RGB (565 or 555)
			masks = Bitmap_GetRGBMasks(dib);
			if (masks) {
				return masks->red_mask;
			}
			return Bitmap_GetBPP(dib) >= 24 ? FI_RGBA_RED_MASK : 0;
		default:
			return 0;
	}
}

unsigned
Bitmap_GetGreenMask(FIBITMAP *dib) {
	BITMAPRGBMASKS *masks = NULL;
	BITMAP_TYPE image_type = Bitmap_GetImageType(dib);
	switch(image_type) {
		case FIT_BITMAP:
			// check for 16-bit RGB (565 or 555)
			masks = Bitmap_GetRGBMasks(dib);
			if (masks) {
				return masks->green_mask;
			}
			return Bitmap_GetBPP(dib) >= 24 ? FI_RGBA_GREEN_MASK : 0;
		default:
			return 0;
	}
}

unsigned
Bitmap_GetBlueMask(FIBITMAP *dib) {
	BITMAPRGBMASKS *masks = NULL;
	BITMAP_TYPE image_type = Bitmap_GetImageType(dib);
	switch(image_type) {
		case FIT_BITMAP:
			// check for 16-bit RGB (565 or 555)
			masks = Bitmap_GetRGBMasks(dib);
			if (masks) {
				return masks->blue_mask;
			}
			return Bitmap_GetBPP(dib) >= 24 ? FI_RGBA_BLUE_MASK : 0;
		default:
			return 0;
	}
}

// ----------------------------------------------------------

BOOL
Bitmap_HasBackgroundColor(FIBITMAP *dib) {
	if(dib) {
		RGBQUAD *bkgnd_color = &((BITMAPHEADER *)dib->data)->bkgnd_color;
		return (bkgnd_color->rgbReserved != 0) ? TRUE : FALSE;
	}
	return FALSE;
}

BOOL
Bitmap_GetBackgroundColor(FIBITMAP *dib, RGBQUAD *bkcolor) {
	if(dib && bkcolor) {
		if (Bitmap_HasBackgroundColor(dib)) {
			// get the background color
			RGBQUAD *bkgnd_color = &((BITMAPHEADER *)dib->data)->bkgnd_color;
			memcpy(bkcolor, bkgnd_color, sizeof(RGBQUAD));
			// get the background index
			if (Bitmap_GetBPP(dib) == 8) {
				RGBQUAD *pal = Bitmap_GetPalette(dib);
				for (unsigned i = 0; i < Bitmap_GetColorsUsed(dib); i++) {
					if(bkgnd_color->rgbRed == pal[i].rgbRed) {
						if(bkgnd_color->rgbGreen == pal[i].rgbGreen) {
							if(bkgnd_color->rgbBlue == pal[i].rgbBlue) {
								bkcolor->rgbReserved = (BYTE)i;
								return TRUE;
							}
						}
					}
				}
			}

			bkcolor->rgbReserved = 0;

			return TRUE;
		}
	}

	return FALSE;
}

BOOL
Bitmap_SetBackgroundColor(FIBITMAP *dib, RGBQUAD *bkcolor) {
	if(dib) {
		RGBQUAD *bkgnd_color = &((BITMAPHEADER *)dib->data)->bkgnd_color;
		if(bkcolor) {
			// set the background color
			memcpy(bkgnd_color, bkcolor, sizeof(RGBQUAD));
			// enable the file background color
			bkgnd_color->rgbReserved = 1;
		} else {
			// clear and disable the file background color
			memset(bkgnd_color, 0, sizeof(RGBQUAD));
		}
		return TRUE;
	}

	return FALSE;
}

// ----------------------------------------------------------

BOOL
Bitmap_IsTransparent(FIBITMAP *dib) {
	if (dib) {
		BITMAP_TYPE image_type = Bitmap_GetImageType(dib);
		switch (image_type) {
		case FIT_BITMAP:
			if (Bitmap_GetBPP(dib) == 32) {
				if (Bitmap_GetColorType(dib) == FIC_RGBALPHA) {
					return TRUE;
				}
			}
			else {
				return ((BITMAPHEADER *)dib->data)->transparent ? TRUE : FALSE;
			}
			break;
		default:
			break;
		}
	}
	return FALSE;
}

BYTE *
Bitmap_GetTransparencyTable(FIBITMAP *dib) {
	return dib ? ((BITMAPHEADER *)dib->data)->transparent_table : NULL;
}

void
Bitmap_SetTransparent(FIBITMAP *dib, BOOL enabled) {
	if (dib) {
		if ((Bitmap_GetBPP(dib) <= 8) || (Bitmap_GetBPP(dib) == 32)) {
			((BITMAPHEADER *)dib->data)->transparent = enabled;
		}
		else {
			((BITMAPHEADER *)dib->data)->transparent = FALSE;
		}
	}
}

unsigned
Bitmap_GetTransparencyCount(FIBITMAP *dib) {
	return dib ? ((BITMAPHEADER *)dib->data)->transparency_count : 0;
}

void
Bitmap_SetTransparencyTable(FIBITMAP *dib, BYTE *table, int count) {
	if (dib) {
		count = MAX(0, MIN(count, 256));
		if (Bitmap_GetBPP(dib) <= 8) {
			((BITMAPHEADER *)dib->data)->transparent = (count > 0) ? TRUE : FALSE;
			((BITMAPHEADER *)dib->data)->transparency_count = count;

			if (table) {
				memcpy(((BITMAPHEADER *)dib->data)->transparent_table, table, count);
			}
			else {
				memset(((BITMAPHEADER *)dib->data)->transparent_table, 0xff, count);
			}
		}
	}
}

// ----------------------------------------------------------

FIICCPROFILE *
Bitmap_GetICCProfile(FIBITMAP *dib) {
	FIICCPROFILE *profile = (dib) ? (FIICCPROFILE *)&((BITMAPHEADER *)dib->data)->iccProfile : NULL;
	return profile;
}

FIICCPROFILE *
Bitmap_CreateICCProfile(FIBITMAP *dib, void *data, long size) {
	// clear the profile but preserve profile->flags
	Bitmap_DestroyICCProfile(dib);
	// create the new profile
	FIICCPROFILE *profile = Bitmap_GetICCProfile(dib);
	if(size && profile) {
		profile->data = malloc(size);
		if(profile->data) {
			memcpy(profile->data, data, profile->size = size);
		}
	}
	return profile;
}

void
Bitmap_DestroyICCProfile(FIBITMAP *dib) {
	FIICCPROFILE *profile = Bitmap_GetICCProfile(dib);
	if(profile) {
		if (profile->data) {
			free (profile->data);
		}
		// clear the profile but preserve profile->flags
		profile->data = NULL;
		profile->size = 0;
	}
}

// ----------------------------------------------------------

unsigned
Bitmap_GetWidth(FIBITMAP *dib) {
	return dib ? Bitmap_GetInfoHeader(dib)->biWidth : 0;
}

unsigned
Bitmap_GetHeight(FIBITMAP *dib) {
	return (dib) ? Bitmap_GetInfoHeader(dib)->biHeight : 0;
}

unsigned
Bitmap_GetBPP(FIBITMAP *dib) {
	return dib ? Bitmap_GetInfoHeader(dib)->biBitCount : 0;
}

unsigned
Bitmap_GetLine(FIBITMAP *dib) {
	return dib ? ((Bitmap_GetWidth(dib) * Bitmap_GetBPP(dib)) + 7) / 8 : 0;
}

unsigned
Bitmap_GetPitch(FIBITMAP *dib) {
	if(dib) {
		BITMAPHEADER *fih = (BITMAPHEADER *)dib->data;
		return (Bitmap_GetLine(dib) + 3 & ~3);
	}
	return 0;
}

unsigned
Bitmap_GetColorsUsed(FIBITMAP *dib) {
	return dib ? Bitmap_GetInfoHeader(dib)->biClrUsed : 0;
}

unsigned
Bitmap_GetDIBSize(FIBITMAP *dib) {
	return (dib) ? sizeof(BITMAPINFOHEADER) + (Bitmap_GetColorsUsed(dib) * sizeof(RGBQUAD)) + (Bitmap_GetPitch(dib) * Bitmap_GetHeight(dib)) : 0;
}

RGBQUAD *
Bitmap_GetPalette(FIBITMAP *dib) {
	return (dib && Bitmap_GetBPP(dib) < 16) ? (RGBQUAD *)(((BYTE *)Bitmap_GetInfoHeader(dib)) + sizeof(BITMAPINFOHEADER)) : NULL;
}

unsigned
Bitmap_GetDotsPerMeterX(FIBITMAP *dib) {
	return (dib) ? Bitmap_GetInfoHeader(dib)->biXPelsPerMeter : 0;
}

unsigned
Bitmap_GetDotsPerMeterY(FIBITMAP *dib) {
	return (dib) ? Bitmap_GetInfoHeader(dib)->biYPelsPerMeter : 0;
}

void
Bitmap_SetDotsPerMeterX(FIBITMAP *dib, unsigned res) {
	if (dib) {
		Bitmap_GetInfoHeader(dib)->biXPelsPerMeter = res;
	}
}

void
Bitmap_SetDotsPerMeterY(FIBITMAP *dib, unsigned res) {
	if (dib) {
		Bitmap_GetInfoHeader(dib)->biYPelsPerMeter = res;
	}
}

BITMAPINFOHEADER *
Bitmap_GetInfoHeader(FIBITMAP *dib) {
	if(!dib) {
		return NULL;
	}
	size_t lp = (size_t)dib->data + sizeof(BITMAPHEADER);
	lp += (lp % FIBITMAP_ALIGNMENT ? FIBITMAP_ALIGNMENT - lp % FIBITMAP_ALIGNMENT : 0);
	lp += FIBITMAP_ALIGNMENT - sizeof(BITMAPINFOHEADER) % FIBITMAP_ALIGNMENT;
	return (BITMAPINFOHEADER *)lp;
}

BITMAPINFO *
Bitmap_GetInfo(FIBITMAP *dib) {
	return (BITMAPINFO *)Bitmap_GetInfoHeader(dib);
}

// ----------------------------------------------------------

unsigned
Bitmap_GetMemorySize(FIBITMAP *dib) {
	if (!dib) {
		return 0;
	}
	BITMAPHEADER *header = (BITMAPHEADER *)dib->data;
	BITMAPINFOHEADER *bih = Bitmap_GetInfoHeader(dib);

	BOOL header_only = !header->has_pixels;
	BOOL need_masks = bih->biCompression == BI_BITFIELDS;
	unsigned width = bih->biWidth;
	unsigned height = bih->biHeight;
	unsigned bpp = bih->biBitCount;
	
	// start off with the size of the FIBITMAP structure
	size_t size = sizeof(FIBITMAP);
	
	// add sizes of FREEIMAGEHEADER, BITMAPINFOHEADER, palette and DIB data
	size += Bitmap_GetInternalImageSize(header_only, width, height, bpp, need_masks);

	// add ICC profile size
	size += header->iccProfile.size;

	return (unsigned)size;
}

/**
Copy a sub part of the current image and returns it as a FIBITMAP*.
Works with any bitmap type.
@param left Specifies the left position of the cropped rectangle.
@param top Specifies the top position of the cropped rectangle.
@param right Specifies the right position of the cropped rectangle.
@param bottom Specifies the bottom position of the cropped rectangle.
@return Returns the subimage if successful, NULL otherwise.
*/
FIBITMAP *
Bitmap_Copy(FIBITMAP *src, int left, int top, int right, int bottom) {

	if (!Bitmap_HasPixels(src))
		return NULL;

	// normalize the rectangle
	if (right < left) {
		int tmp = right;
		right = left;
		left = tmp;
	}
	if (bottom < top) {
		int tmp = top;
		top = bottom;
		bottom = tmp;
	}
	// check the size of the sub image
	int src_width = Bitmap_GetWidth(src);
	int src_height = Bitmap_GetHeight(src);
	if ((left < 0) || (right > src_width) || (top < 0) || (bottom > src_height)) {
		return NULL;
	}

	// allocate the sub image
	unsigned bpp = Bitmap_GetBPP(src);
	int dst_width = (right - left);
	int dst_height = (bottom - top);

	FIBITMAP *dst =
		Bitmap_AllocateT(Bitmap_GetImageType(src),
		dst_width,
		dst_height,
		bpp,
		Bitmap_GetRedMask(src), Bitmap_GetGreenMask(src), Bitmap_GetBlueMask(src));

	if (NULL == dst) return NULL;

	// get the dimensions
	int dst_line = Bitmap_GetLine(dst);
	int dst_pitch = Bitmap_GetPitch(dst);
	int src_pitch = Bitmap_GetPitch(src);

	// get the pointers to the bits and such

	BYTE *src_bits = Bitmap_GetScanLine(src, src_height - top - dst_height);
	switch (bpp) {
	case 1:
		// point to x = 0
		break;

	case 4:
		// point to x = 0
		break;

	default:
	{
		// calculate the number of bytes per pixel
		unsigned bytespp = Bitmap_GetLine(src) / Bitmap_GetWidth(src);
		// point to x = left
		src_bits += left * bytespp;
	}
	break;
	}

	// point to x = 0
	BYTE *dst_bits = Bitmap_GetBits(dst);

	// copy the palette

	memcpy(Bitmap_GetPalette(dst), Bitmap_GetPalette(src), Bitmap_GetColorsUsed(src) * sizeof(RGBQUAD));

	// copy the bits
	if (bpp == 1) {
		BOOL value;
		unsigned y_src, y_dst;

		for (int y = 0; y < dst_height; y++) {
			y_src = y * src_pitch;
			y_dst = y * dst_pitch;
			for (int x = 0; x < dst_width; x++) {
				// get bit at (y, x) in src image
				value = (src_bits[y_src + ((left + x) >> 3)] & (0x80 >> ((left + x) & 0x07))) != 0;
				// set bit at (y, x) in dst image
				value ? dst_bits[y_dst + (x >> 3)] |= (0x80 >> (x & 0x7)) : dst_bits[y_dst + (x >> 3)] &= (0xff7f >> (x & 0x7));
			}
		}
	}

	else if (bpp == 4) {
		BYTE shift, value;
		unsigned y_src, y_dst;

		for (int y = 0; y < dst_height; y++) {
			y_src = y * src_pitch;
			y_dst = y * dst_pitch;
			for (int x = 0; x < dst_width; x++) {
				// get nibble at (y, x) in src image
				shift = (BYTE)((1 - (left + x) % 2) << 2);
				value = (src_bits[y_src + ((left + x) >> 1)] & (0x0F << shift)) >> shift;
				// set nibble at (y, x) in dst image
				shift = (BYTE)((1 - x % 2) << 2);
				dst_bits[y_dst + (x >> 1)] &= ~(0x0F << shift);
				dst_bits[y_dst + (x >> 1)] |= ((value & 0x0F) << shift);
			}
		}
	}

	else if (bpp >= 8) {
		for (int y = 0; y < dst_height; y++) {
			memcpy(dst_bits + (y * dst_pitch), src_bits + (y * src_pitch), dst_line);
		}
	}

	// copy transparency table 
	Bitmap_SetTransparencyTable(dst, Bitmap_GetTransparencyTable(src), Bitmap_GetTransparencyCount(src));

	// copy background color 
	RGBQUAD bkcolor;
	if (Bitmap_GetBackgroundColor(src, &bkcolor)) {
		Bitmap_SetBackgroundColor(dst, &bkcolor);
	}

	// clone resolution 
	Bitmap_SetDotsPerMeterX(dst, Bitmap_GetDotsPerMeterX(src));
	Bitmap_SetDotsPerMeterY(dst, Bitmap_GetDotsPerMeterY(src));

	// clone ICC profile 
	FIICCPROFILE *src_profile = Bitmap_GetICCProfile(src);
	FIICCPROFILE *dst_profile = Bitmap_CreateICCProfile(dst, src_profile->data, src_profile->size);
	dst_profile->flags = src_profile->flags;

	return dst;
}

BOOL
Bitmap_ValidateFromHandle(BITMAP_FORMAT fif, BitmapIO *io, fi_handle handle) {
	BOOL valid = FALSE;
	if ((fif >= 0) && (fif < FIF_LAST)) {
		long tell = io->tell_proc(handle);
		switch (fif) {
		case FIF_BMP:
			valid = ValidateBMP(io, handle);
			break;
		case FIF_PCX:
			valid = ValidatePCX(io, handle);
			break;
		case FIF_JPEG:
			valid = ValidateJPEG(io, handle);
			break;
		case FIF_PNG:
			valid = ValidatePNG(io, handle);
			break;
		case FIF_TIFF:
			valid = ValidateTIFF(io, handle);
			break;
		case FIF_UNKNOWN:
			break;
		}
		io->seek_proc(handle, tell, SEEK_SET);
	}

	return valid;
}

BOOL
Bitmap_Validate(BITMAP_FORMAT fif, const char *filename) {
	BitmapIO io;
	SetDefaultIO(&io);

	FILE *handle = fopen(filename, "rb");

	if (handle) {
		BOOL valid = Bitmap_ValidateFromHandle(fif, &io, (fi_handle)handle);

		fclose(handle);

		return valid;
	}
	else {
		Bitmap_OutputMessageProc((int)fif, "Bitmap_Validate: failed to open file %s", filename);
	}

	return FALSE;
}

FIBITMAP *
Bitmap_LoadFromHandle(BITMAP_FORMAT fif, BitmapIO *io, fi_handle handle, int flags) {
	FIBITMAP *bitmap = NULL;
	if ((fif >= 0) && (fif < FIF_LAST)) {
		switch (fif) {
		case FIF_BMP:
			bitmap = LoadBMP(io, handle, -1, flags);
			break;
		case FIF_PCX:
			bitmap = LoadPCX(io, handle, -1, flags);
			break;
		case FIF_JPEG:
			bitmap = LoadJPEG(io, handle, -1, flags);
			break;
		case FIF_PNG:
			bitmap = LoadPNG(io, handle, -1, flags);
			break;
		case FIF_TIFF:
			bitmap = LoadTIFF(io, handle, -1, flags);
			break;
		case FIF_UNKNOWN:
			break;
		}
	}

	return bitmap;
}

FIBITMAP *
Bitmap_Load(BITMAP_FORMAT fif, const char *filename, int flags) {
	BitmapIO io;
	SetDefaultIO(&io);

	FILE *handle = fopen(filename, "rb");

	if (handle) {
		FIBITMAP *bitmap = Bitmap_LoadFromHandle(fif, &io, (fi_handle)handle, flags);

		fclose(handle);

		return bitmap;
	}
	else {
		Bitmap_OutputMessageProc((int)fif, "Bitmap_Load: failed to open file %s", filename);
	}

	return NULL;
}

