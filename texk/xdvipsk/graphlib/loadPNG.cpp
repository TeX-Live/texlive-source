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
// PNG Loader
//
// ==========================================================

#ifdef _MSC_VER 
#pragma warning (disable : 4786) // identifier was truncated to 'number' characters
#endif

#include "bitmap.h"
#include "utilities.h"

// ----------------------------------------------------------

#define PNG_BYTES_TO_CHECK 8

#undef PNG_Z_DEFAULT_COMPRESSION	// already used in ../LibPNG/pnglibconf.h

// ----------------------------------------------------------

#include "zlib.h"
#include "png.h"

// ----------------------------------------------------------

typedef struct {
	BitmapIO *s_io;
    fi_handle    s_handle;
} fi_ioStructure, *pfi_ioStructure;

// ==========================================================
// libpng interface
// ==========================================================

static void
_ReadProc(png_structp png_ptr, unsigned char *data, png_size_t size) {
    pfi_ioStructure pfio = (pfi_ioStructure)png_get_io_ptr(png_ptr);
	unsigned n = pfio->s_io->read_proc(data, (unsigned int)size, 1, pfio->s_handle);
	if(size && (n == 0)) {
		throw "Read error: invalid or corrupted PNG file";
	}
}

static void
_WriteProc(png_structp png_ptr, unsigned char *data, png_size_t size) {
    pfi_ioStructure pfio = (pfi_ioStructure)png_get_io_ptr(png_ptr);
    pfio->s_io->write_proc(data, (unsigned int)size, 1, pfio->s_handle);
}

static void
_FlushProc(png_structp png_ptr) {
	// empty flush implementation
}

static void
error_handler(png_structp png_ptr, const char *error) {
	throw error;
}

// in Bitmap warnings disabled

static void
warning_handler(png_structp png_ptr, const char *warning) {
}


BOOL
ValidatePNG(BitmapIO *io, fi_handle handle) {
	BYTE png_signature[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
	BYTE signature[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	io->read_proc(&signature, 1, 8, handle);

	return (memcmp(png_signature, signature, 8) == 0);
}

/**
Configure the decoder so that decoded pixels are compatible with a BITMAP_TYPE format. 
Set conversion instructions as needed. 
@param png_ptr PNG handle
@param info_ptr PNG info handle
@param flags Decoder flags
@param output_image_type Returned Bitmap converted image type
@return Returns TRUE if successful, returns FALSE otherwise
@see png_read_update_info
*/
static BOOL 
ConfigureDecoder(png_structp png_ptr, png_infop info_ptr, int flags, BITMAP_TYPE *output_image_type) {
	// get original image info
	const int color_type = png_get_color_type(png_ptr, info_ptr);
	const int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	const int pixel_depth = bit_depth * png_get_channels(png_ptr, info_ptr);

	BITMAP_TYPE image_type = FIT_BITMAP;	// assume standard image type

	// check for transparency table or single transparent color
	BOOL bIsTransparent = png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS) == PNG_INFO_tRNS ? TRUE : FALSE;

	// check allowed combinations of colour type and bit depth
	// then get converted Bitmap type

	switch(color_type) {
		case PNG_COLOR_TYPE_GRAY:		// color type '0', bitdepth = 1, 2, 4, 8, 16
			switch(bit_depth) {
				case 1:
				case 2:
				case 4:
				case 8:
					// expand grayscale images to the full 8-bit from 2-bit/pixel
					if (pixel_depth == 2) {
						png_set_expand_gray_1_2_4_to_8(png_ptr);
					}

					// if a tRNS chunk is provided, we must also expand the grayscale data to 8-bits,
					// this allows us to make use of the transparency table with existing Bitmap methods
					if (bIsTransparent && (pixel_depth < 8)) {
						png_set_expand_gray_1_2_4_to_8(png_ptr);
					}
					break;

				case 16:
					image_type = FIT_UNKNOWN;
					break;

				default:
					image_type = FIT_UNKNOWN;
					break;
			}
			break;

		case PNG_COLOR_TYPE_RGB:		// color type '2', bitdepth = 8, 16
			switch(bit_depth) {
				case 8:
					image_type = (pixel_depth == 24) ? FIT_BITMAP : FIT_UNKNOWN;
					break;
				case 16:
					image_type = FIT_UNKNOWN;
					break;
				default:
					image_type = FIT_UNKNOWN;
					break;
			}
			// sometimes, 24- or 48-bit images may contain transparency information
			// check for this use case and convert to an alpha-compatible format
			if (bIsTransparent && (image_type != FIT_UNKNOWN)) {
				// if the image is 24-bit RGB, mark it as 32-bit; if it is 48-bit, mark it as 64-bit
				image_type = (pixel_depth == 24) ? FIT_BITMAP : FIT_UNKNOWN;
				// expand tRNS chunk to alpha channel
				png_set_tRNS_to_alpha(png_ptr);
			}
			break;

		case PNG_COLOR_TYPE_PALETTE:	// color type '3', bitdepth = 1, 2, 4, 8
			switch(bit_depth) {
				case 1:
				case 2:
				case 4:
				case 8:
					// expand palette images to the full 8 bits from 2 bits/pixel
					if (pixel_depth == 2) {
						png_set_packing(png_ptr);
					}

					// if a tRNS chunk is provided, we must also expand the palletized data to 8-bits,
					// this allows us to make use of the transparency table with existing Bitmap methods
					if (bIsTransparent && (pixel_depth < 8)) {
						png_set_packing(png_ptr);
					}
					break;

				default:
					image_type = FIT_UNKNOWN;
					break;
			}
			break;

		case PNG_COLOR_TYPE_GRAY_ALPHA:	// color type '4', bitdepth = 8, 16
			switch(bit_depth) {
				case 8:
					// 8-bit grayscale + 8-bit alpha => convert to 32-bit RGBA
					image_type = (pixel_depth == 16) ? FIT_BITMAP : FIT_UNKNOWN;
					break;
				case 16:
					// 16-bit grayscale + 16-bit alpha => convert to 64-bit RGBA
					image_type = FIT_UNKNOWN;
					break;
				default:
					image_type = FIT_UNKNOWN;
					break;
			}
			// expand 8-bit greyscale + 8-bit alpha to 32-bit
			// expand 16-bit greyscale + 16-bit alpha to 64-bit
			png_set_gray_to_rgb(png_ptr);
			break;

		case PNG_COLOR_TYPE_RGB_ALPHA:	// color type '6', bitdepth = 8, 16
			switch(bit_depth) {
				case 8:
					break;
				case 16:
					image_type = FIT_UNKNOWN;
					break;
				default:
					image_type = FIT_UNKNOWN;
					break;
			}
			break;
	}

	// check for unknown or invalid formats
	if(image_type == FIT_UNKNOWN) {
		*output_image_type = image_type;
		return FALSE;
	}

#if BITMAP_COLORORDER == BITMAP_COLORORDER_BGR
	if((image_type == FIT_BITMAP) && ((color_type == PNG_COLOR_TYPE_RGB) || (color_type == PNG_COLOR_TYPE_RGB_ALPHA))) {
		// flip the RGB pixels to BGR (or RGBA to BGRA)
		png_set_bgr(png_ptr);
	}
#endif

	// gamma correction
	// unlike the example in the libpng documentation, we have *no* idea where
	// this file may have come from--so if it doesn't have a file gamma, don't
	// do any correction ("do no harm")

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_gAMA)) {
		double gamma = 0;
		double screen_gamma = 2.2;

		if (png_get_gAMA(png_ptr, info_ptr, &gamma) && ( flags & PNG_IGNOREGAMMA ) != PNG_IGNOREGAMMA) {
			png_set_gamma(png_ptr, screen_gamma, gamma);
		}
	}

	// all transformations have been registered; now update info_ptr data		
	png_read_update_info(png_ptr, info_ptr);

	// return the output image type
	*output_image_type = image_type;

	return TRUE;
}

FIBITMAP *
LoadPNG(BitmapIO *io, fi_handle handle, int page, int flags) {
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_uint_32 width, height;
	int color_type;
	int bit_depth;
	int pixel_depth = 0;	// pixel_depth = bit_depth * channels

	FIBITMAP *dib = NULL;
	png_bytepp row_pointers = NULL;

    fi_ioStructure fio;
    fio.s_handle = handle;
	fio.s_io = io;
    
	if (handle) {
		BOOL header_only = (flags & FIF_LOAD_NOPIXELS) == FIF_LOAD_NOPIXELS;

		try {		
			// check to see if the file is in fact a PNG file

			BYTE png_check[PNG_BYTES_TO_CHECK];

			io->read_proc(png_check, PNG_BYTES_TO_CHECK, 1, handle);

			if (png_sig_cmp(png_check, (png_size_t)0, PNG_BYTES_TO_CHECK) != 0) {
				return NULL;	// Bad signature
			}
			
			// create the chunk manage structure

			png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL, error_handler, warning_handler);

			if (!png_ptr) {
				return NULL;			
			}

			// create the info structure

		    info_ptr = png_create_info_struct(png_ptr);

			if (!info_ptr) {
				png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
				return NULL;
			}

			// init the IO

			png_set_read_fn(png_ptr, &fio, _ReadProc);

            if (setjmp(png_jmpbuf(png_ptr))) {
				png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
				return NULL;
			}

			// because we have already read the signature...

			png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);

			// read the IHDR chunk

			png_read_info(png_ptr, info_ptr);
			png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

			// configure the decoder

			BITMAP_TYPE image_type = FIT_BITMAP;

			if(!ConfigureDecoder(png_ptr, info_ptr, flags, &image_type)) {
				throw FI_MSG_ERROR_UNSUPPORTED_FORMAT;
			}

			// update image info

			color_type = png_get_color_type(png_ptr, info_ptr);
			bit_depth = png_get_bit_depth(png_ptr, info_ptr);
			pixel_depth = bit_depth * png_get_channels(png_ptr, info_ptr);

			// create a dib and write the bitmap header
			// set up the dib palette, if needed

			switch (color_type) {
				case PNG_COLOR_TYPE_RGB:
				case PNG_COLOR_TYPE_RGB_ALPHA:
					dib = Bitmap_AllocateHeaderT(header_only, image_type, width, height, pixel_depth, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
					break;

				case PNG_COLOR_TYPE_PALETTE:
					dib = Bitmap_AllocateHeaderT(header_only, image_type, width, height, pixel_depth, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
					if(dib) {
						png_colorp png_palette = NULL;
						int palette_entries = 0;

						png_get_PLTE(png_ptr,info_ptr, &png_palette, &palette_entries);

						palette_entries = MIN((unsigned)palette_entries, Bitmap_GetColorsUsed(dib));

						// store the palette

						RGBQUAD *palette = Bitmap_GetPalette(dib);
						for(int i = 0; i < palette_entries; i++) {
							palette[i].rgbRed   = png_palette[i].red;
							palette[i].rgbGreen = png_palette[i].green;
							palette[i].rgbBlue  = png_palette[i].blue;
						}
					}
					break;

				case PNG_COLOR_TYPE_GRAY:
					dib = Bitmap_AllocateHeaderT(header_only, image_type, width, height, pixel_depth, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);

					if(dib && (pixel_depth <= 8)) {
						RGBQUAD *palette = Bitmap_GetPalette(dib);
						const int palette_entries = 1 << pixel_depth;

						for(int i = 0; i < palette_entries; i++) {
							palette[i].rgbRed   =
							palette[i].rgbGreen =
							palette[i].rgbBlue  = (BYTE)((i * 255) / (palette_entries - 1));
						}
					}
					break;

				default:
					throw FI_MSG_ERROR_UNSUPPORTED_FORMAT;
			}

			if(!dib) {
				throw FI_MSG_ERROR_DIB_MEMORY;
			}

			// store the transparency table

			if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
				// array of alpha (transparency) entries for palette
				png_bytep trans_alpha = NULL;
				// number of transparent entries
				int num_trans = 0;						
				// graylevel or color sample values of the single transparent color for non-paletted images
				png_color_16p trans_color = NULL;

				png_get_tRNS(png_ptr, info_ptr, &trans_alpha, &num_trans, &trans_color);

				if((color_type == PNG_COLOR_TYPE_GRAY) && trans_color) {
					// single transparent color
					if (trans_color->gray < 256) { 
						BYTE table[256]; 
						memset(table, 0xFF, 256); 
						table[trans_color->gray] = 0; 
						Bitmap_SetTransparencyTable(dib, table, 256);
					}
					// check for a full transparency table, too
					else if ((trans_alpha) && (pixel_depth <= 8)) {
						Bitmap_SetTransparencyTable(dib, (BYTE *)trans_alpha, num_trans);
					}

				} else if((color_type == PNG_COLOR_TYPE_PALETTE) && trans_alpha) {
					// transparency table
					Bitmap_SetTransparencyTable(dib, (BYTE *)trans_alpha, num_trans);
				}
			}

			// store the background color (only supported for FIT_BITMAP types)

			if ((image_type == FIT_BITMAP) && png_get_valid(png_ptr, info_ptr, PNG_INFO_bKGD)) {
				// Get the background color to draw transparent and alpha images over.
				// Note that even if the PNG file supplies a background, you are not required to
				// use it - you should use the (solid) application background if it has one.

				png_color_16p image_background = NULL;
				RGBQUAD rgbBkColor;

				if (png_get_bKGD(png_ptr, info_ptr, &image_background)) {
					rgbBkColor.rgbRed      = (BYTE)image_background->red;
					rgbBkColor.rgbGreen    = (BYTE)image_background->green;
					rgbBkColor.rgbBlue     = (BYTE)image_background->blue;
					rgbBkColor.rgbReserved = 0;

					Bitmap_SetBackgroundColor(dib, &rgbBkColor);
				}
			}

			// get physical resolution

			if (png_get_valid(png_ptr, info_ptr, PNG_INFO_pHYs)) {
				png_uint_32 res_x, res_y;
				
				// we'll overload this var and use 0 to mean no phys data,
				// since if it's not in meters we can't use it anyway

				int res_unit_type = PNG_RESOLUTION_UNKNOWN;

				png_get_pHYs(png_ptr,info_ptr, &res_x, &res_y, &res_unit_type);

				if (res_unit_type == PNG_RESOLUTION_METER) {
					Bitmap_SetDotsPerMeterX(dib, res_x);
					Bitmap_SetDotsPerMeterY(dib, res_y);
				}
			}

			// get possible ICC profile

			if (png_get_valid(png_ptr, info_ptr, PNG_INFO_iCCP)) {
				png_charp profile_name = NULL;
				png_bytep profile_data = NULL;
				png_uint_32 profile_length = 0;
				int  compression_type;

				png_get_iCCP(png_ptr, info_ptr, &profile_name, &compression_type, &profile_data, &profile_length);

				// copy ICC profile data (must be done after Bitmap_AllocateHeader)

				Bitmap_CreateICCProfile(dib, profile_data, profile_length);
			}

			// --- header only mode => clean-up and return

			if (header_only) {
				if (png_ptr) {
					// clean up after the read, and free any memory allocated - REQUIRED
					png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
				}
				return dib;
			}

			// set the individual row_pointers to point at the correct offsets

			row_pointers = (png_bytepp)malloc(height * sizeof(png_bytep));

			if (!row_pointers) {
				png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
				Bitmap_Unload(dib);
				return NULL;
			}

			// read in the bitmap bits via the pointer table
			// allow loading of PNG with minor errors (such as images with several IDAT chunks)

			for (png_uint_32 k = 0; k < height; k++) {
				row_pointers[height - 1 - k] = Bitmap_GetScanLine(dib, k);
			}

			png_set_benign_errors(png_ptr, 1);
			png_read_image(png_ptr, row_pointers);

			// check if the bitmap contains transparency, if so enable it in the header

			if (Bitmap_GetBPP(dib) == 32) {
				if (Bitmap_GetColorType(dib) == FIC_RGBALPHA) {
					Bitmap_SetTransparent(dib, TRUE);
				} else {
					Bitmap_SetTransparent(dib, FALSE);
				}
			}
				
			// cleanup

			if (row_pointers) {
				free(row_pointers);
				row_pointers = NULL;
			}

			// read the rest of the file, getting any additional chunks in info_ptr

			png_read_end(png_ptr, info_ptr);

			if (png_ptr) {
				// clean up after the read, and free any memory allocated - REQUIRED
				png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
			}

			return dib;

		} catch (const char *text) {
			if (png_ptr) {
				png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
			}
			if (row_pointers) {
				free(row_pointers);
			}
			if (dib) {
				Bitmap_Unload(dib);
			}
			Bitmap_OutputMessageProc(FIF_PNG, text);
			
			return NULL;
		}
	}			

	return NULL;
}
