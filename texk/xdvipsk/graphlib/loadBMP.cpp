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
// BMP Loader
//
// ==========================================================

#include "bitmap.h"
#include "utilities.h"

// ----------------------------------------------------------
//   Constants + headers
// ----------------------------------------------------------

static const BYTE RLE_COMMAND     = 0;
static const BYTE RLE_ENDOFLINE   = 0;
static const BYTE RLE_ENDOFBITMAP = 1;
static const BYTE RLE_DELTA       = 2;

static const BYTE BI_RGB            = 0;	// compression: none
static const BYTE BI_RLE8           = 1;	// compression: RLE 8-bit/pixel
static const BYTE BI_RLE4           = 2;	// compression: RLE 4-bit/pixel
static const BYTE BI_BITFIELDS      = 3;	// compression: Bit field or Huffman 1D compression for BITMAPCOREHEADER2
static const BYTE BI_JPEG           = 4;	// compression: JPEG or RLE-24 compression for BITMAPCOREHEADER2
static const BYTE BI_PNG            = 5;	// compression: PNG
static const BYTE BI_ALPHABITFIELDS = 6;	// compression: Bit field (this value is valid in Windows CE .NET 4.0 and later)

// ----------------------------------------------------------

#ifdef _WIN32
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef struct tagBITMAPCOREHEADER {
  DWORD   bcSize;
  WORD    bcWidth;
  WORD    bcHeight;
  WORD    bcPlanes;
  WORD    bcBitCnt;
} BITMAPCOREHEADER, *PBITMAPCOREHEADER; 

typedef struct tagBITMAPINFOOS2_1X_HEADER {
  DWORD  biSize;
  WORD   biWidth;
  WORD   biHeight; 
  WORD   biPlanes; 
  WORD   biBitCount;
} BITMAPINFOOS2_1X_HEADER, *PBITMAPINFOOS2_1X_HEADER; 

typedef struct tagBITMAPFILEHEADER {
  WORD    bfType;		//! The file type
  DWORD   bfSize;		//! The size, in bytes, of the bitmap file
  WORD    bfReserved1;	//! Reserved; must be zero
  WORD    bfReserved2;	//! Reserved; must be zero
  DWORD   bfOffBits;	//! The offset, in bytes, from the beginning of the BITMAPFILEHEADER structure to the bitmap bits
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

#ifdef _WIN32
#pragma pack(pop)
#else
#pragma pack()
#endif


// ==========================================================
// Internal functions
// ==========================================================

#ifdef BITMAP_BIGENDIAN
static void
SwapInfoHeader(BITMAPINFOHEADER *header) {
	SwapLong(&header->biSize);
	SwapLong((DWORD *)&header->biWidth);
	SwapLong((DWORD *)&header->biHeight);
	SwapShort(&header->biPlanes);
	SwapShort(&header->biBitCount);
	SwapLong(&header->biCompression);
	SwapLong(&header->biSizeImage);
	SwapLong((DWORD *)&header->biXPelsPerMeter);
	SwapLong((DWORD *)&header->biYPelsPerMeter);
	SwapLong(&header->biClrUsed);
	SwapLong(&header->biClrImportant);
}

static void
SwapCoreHeader(BITMAPCOREHEADER *header) {
	SwapLong(&header->bcSize);
	SwapShort(&header->bcWidth);
	SwapShort(&header->bcHeight);
	SwapShort(&header->bcPlanes);
	SwapShort(&header->bcBitCnt);
}

static void
SwapOS21XHeader(BITMAPINFOOS2_1X_HEADER *header) {
	SwapLong(&header->biSize);
	SwapShort(&header->biWidth);
	SwapShort(&header->biHeight);
	SwapShort(&header->biPlanes);
	SwapShort(&header->biBitCount);
}

static void
SwapFileHeader(BITMAPFILEHEADER *header) {
	SwapShort(&header->bfType);
  	SwapLong(&header->bfSize);
  	SwapShort(&header->bfReserved1);
  	SwapShort(&header->bfReserved2);
	SwapLong(&header->bfOffBits);
}
#endif

// --------------------------------------------------------------------------

/**
Load uncompressed image pixels for 1-, 4-, 8-, 16-, 24- and 32-bit dib
@param io Bitmap IO
@param handle Bitmap IO handle
@param dib Image to be loaded 
@param height Image height
@param pitch Image pitch
@param bit_count Image bit-depth (1-, 4-, 8-, 16-, 24- or 32-bit)
@return Returns TRUE if successful, returns FALSE otherwise
*/
static BOOL 
LoadPixelData(BitmapIO *io, fi_handle handle, FIBITMAP *dib, int height, unsigned pitch, unsigned bit_count) {
	unsigned count = 0;

	// Load pixel data
	// NB: height can be < 0 for BMP data
	if (height > 0) {
		count = io->read_proc((void *)Bitmap_GetBits(dib), height * pitch, 1, handle);
		if(count != 1) {
			return FALSE;
		}
	} else {
		int positiveHeight = abs(height);
		for (int c = 0; c < positiveHeight; ++c) {
			count = io->read_proc((void *)Bitmap_GetScanLine(dib, positiveHeight - c - 1), pitch, 1, handle);
			if(count != 1) {
				return FALSE;
			}
		}
	}

	// swap as needed
#ifdef BITMAP_BIGENDIAN
	if (bit_count == 16) {
		for(unsigned y = 0; y < Bitmap_GetHeight(dib); y++) {
			WORD *pixel = (WORD *)Bitmap_GetScanLine(dib, y);
			for(unsigned x = 0; x < Bitmap_GetWidth(dib); x++) {
				SwapShort(pixel);
				pixel++;
			}
		}
	}
#endif
#if BITMAP_COLORORDER == BITMAP_COLORORDER_RGB
	if (bit_count == 24 || bit_count == 32) {
		for(unsigned y = 0; y < Bitmap_GetHeight(dib); y++) {
			BYTE *pixel = Bitmap_GetScanLine(dib, y);
			for(unsigned x = 0; x < Bitmap_GetWidth(dib); x++) {
				BYTE tmp = pixel[0];
				pixel[0] = pixel[2];
				pixel[2] = tmp;
				pixel += (bit_count >> 3);
			}
		}
	}
#endif

	return TRUE;
}

/**
Load image pixels for 4-bit RLE compressed dib
@param io Bitmap IO
@param handle Bitmap IO handle
@param width Image width
@param height Image height
@param dib Image to be loaded 
@return Returns TRUE if successful, returns FALSE otherwise
*/
static BOOL 
LoadPixelDataRLE4(BitmapIO *io, fi_handle handle, int width, int height, FIBITMAP *dib) {
	int status_byte = 0;
	BYTE second_byte = 0;
	int bits = 0;

	BYTE *pixels = NULL;	// temporary 8-bit buffer

	try {
		height = abs(height);

		pixels = (BYTE*)malloc(width * height * sizeof(BYTE));
		if(!pixels) throw(1);
		memset(pixels, 0, width * height * sizeof(BYTE));

		BYTE *q = pixels;
		BYTE *end = pixels + height * width;

		for (int scanline = 0; scanline < height; ) {
			if (q < pixels || q  >= end) {
				break;
			}
			if(io->read_proc(&status_byte, sizeof(BYTE), 1, handle) != 1) {
				throw(1);
			}
			if (status_byte != 0)	{
				status_byte = (int)MIN((size_t)status_byte, (size_t)(end - q));
				// Encoded mode
				if(io->read_proc(&second_byte, sizeof(BYTE), 1, handle) != 1) {
					throw(1);
				}
				for (int i = 0; i < status_byte; i++)	{
					*q++=(BYTE)((i & 0x01) ? (second_byte & 0x0f) : ((second_byte >> 4) & 0x0f));
				}
				bits += status_byte;
			}
			else {
				// Escape mode
				if(io->read_proc(&status_byte, sizeof(BYTE), 1, handle) != 1) {
					throw(1);
				}
				switch (status_byte) {
					case RLE_ENDOFLINE:
					{
						// End of line
						bits = 0;
						scanline++;
						q = pixels + scanline*width;
					}
					break;

					case RLE_ENDOFBITMAP:
						// End of bitmap
						q = end;
						break;

					case RLE_DELTA:
					{
						// read the delta values

						BYTE delta_x = 0;
						BYTE delta_y = 0;

						if(io->read_proc(&delta_x, sizeof(BYTE), 1, handle) != 1) {
							throw(1);
						}
						if(io->read_proc(&delta_y, sizeof(BYTE), 1, handle) != 1) {
							throw(1);
						}

						// apply them

						bits += delta_x;
						scanline += delta_y;
						q = pixels + scanline*width+bits;
					}
					break;

					default:
					{
						// Absolute mode
						status_byte = (int)MIN((size_t)status_byte, (size_t)(end - q));
						for (int i = 0; i < status_byte; i++) {
							if ((i & 0x01) == 0) {
								if(io->read_proc(&second_byte, sizeof(BYTE), 1, handle) != 1) {
									throw(1);
								}
							}
							*q++=(BYTE)((i & 0x01) ? (second_byte & 0x0f) : ((second_byte >> 4) & 0x0f));
						}
						bits += status_byte;
						// Read pad byte
						if (((status_byte & 0x03) == 1) || ((status_byte & 0x03) == 2)) {
							BYTE padding = 0;
							if(io->read_proc(&padding, sizeof(BYTE), 1, handle) != 1) {
								throw(1);
							}
						}
					}
					break;
				}
			}
		}
		
		{
			// Convert to 4-bit
			for(int y = 0; y < height; y++) {
				const BYTE *src = (BYTE*)pixels + y * width;
				BYTE *dst = Bitmap_GetScanLine(dib, y);

				BOOL hinibble = TRUE;

				for (int cols = 0; cols < width; cols++){
					if (hinibble) {
						dst[cols >> 1] = (src[cols] << 4);
					} else {
						dst[cols >> 1] |= src[cols];
					}

					hinibble = !hinibble;
				}
			}
		}

		free(pixels);

		return TRUE;

	} catch(int) {
		if(pixels) free(pixels);
		return FALSE;
	}
}

/**
Load image pixels for 8-bit RLE compressed dib
@param io Bitmap IO
@param handle Bitmap IO handle
@param width Image width
@param height Image height
@param dib Image to be loaded 
@return Returns TRUE if successful, returns FALSE otherwise
*/
static BOOL 
LoadPixelDataRLE8(BitmapIO *io, fi_handle handle, int width, int height, FIBITMAP *dib) {
	BYTE status_byte = 0;
	BYTE second_byte = 0;
	int scanline = 0;
	int bits = 0;

	for (;;) {
		if( io->read_proc(&status_byte, sizeof(BYTE), 1, handle) != 1) {
			return FALSE;
		}

		switch (status_byte) {
			case RLE_COMMAND :
				if(io->read_proc(&status_byte, sizeof(BYTE), 1, handle) != 1) {
					return FALSE;
				}

				switch (status_byte) {
					case RLE_ENDOFLINE :
						bits = 0;
						scanline++;
						break;

					case RLE_ENDOFBITMAP :
						return TRUE;

					case RLE_DELTA :
					{
						// read the delta values

						BYTE delta_x = 0;
						BYTE delta_y = 0;

						if(io->read_proc(&delta_x, sizeof(BYTE), 1, handle) != 1) {
							return FALSE;
						}
						if(io->read_proc(&delta_y, sizeof(BYTE), 1, handle) != 1) {
							return FALSE;
						}

						// apply them

						bits     += delta_x;
						scanline += delta_y;

						break;
					}

					default :
					{
						if(scanline >= abs(height)) {
							return TRUE;
						}

						int count = MIN((int)status_byte, width - bits);

						BYTE *sline = Bitmap_GetScanLine(dib, scanline);

						if(io->read_proc((void *)(sline + bits), sizeof(BYTE) * count, 1, handle) != 1) {
							return FALSE;
						}
						
						// align run length to even number of bytes 

						if ((status_byte & 1) == 1) {
							if(io->read_proc(&second_byte, sizeof(BYTE), 1, handle) != 1) {
								return FALSE;
							}
						}

						bits += status_byte;													

						break;	
					}
				}

				break;

			default :
			{
				if(scanline >= abs(height)) {
					return TRUE;
				}

				int count = MIN((int)status_byte, width - bits);

				BYTE *sline = Bitmap_GetScanLine(dib, scanline);

				if(io->read_proc(&second_byte, sizeof(BYTE), 1, handle) != 1) {
					return FALSE;
				}

				for (int i = 0; i < count; i++) {
					*(sline + bits) = second_byte;

					bits++;					
				}

				break;
			}
		}
	}
}

// --------------------------------------------------------------------------

static FIBITMAP *
LoadWindowsBMP(BitmapIO *io, fi_handle handle, int flags, unsigned bitmap_bits_offset, int type) {
	FIBITMAP *dib = NULL;

	try {
		BOOL header_only = (flags & FIF_LOAD_NOPIXELS) == FIF_LOAD_NOPIXELS;

		// load the info header

		BITMAPINFOHEADER bih;

		io->read_proc(&bih, sizeof(BITMAPINFOHEADER), 1, handle);
#ifdef BITMAP_BIGENDIAN
		SwapInfoHeader(&bih);
#endif

		// keep some general information about the bitmap

		unsigned used_colors	= bih.biClrUsed;
		int width				= bih.biWidth;
		int height				= bih.biHeight;		// WARNING: height can be < 0 => check each call using 'height' as a parameter
		unsigned bit_count		= bih.biBitCount;
		unsigned compression	= bih.biCompression;
		unsigned pitch			= CalculatePitch(CalculateLine(width, bit_count));

		switch (bit_count) {
			case 1 :
			case 4 :
			case 8 :
			{
				if ((used_colors == 0) || (used_colors > CalculateUsedPaletteEntries(bit_count))) {
					used_colors = CalculateUsedPaletteEntries(bit_count);
				}
				
				// allocate enough memory to hold the bitmap (header, palette, pixels) and read the palette

				dib = Bitmap_AllocateHeader(header_only, width, height, bit_count);
				if (dib == NULL) {
					throw FI_MSG_ERROR_DIB_MEMORY;
				}

				// set resolution information
				Bitmap_SetDotsPerMeterX(dib, bih.biXPelsPerMeter);
				Bitmap_SetDotsPerMeterY(dib, bih.biYPelsPerMeter);

				// seek to the end of the header (depending on the BMP header version)
				// type == sizeof(BITMAPVxINFOHEADER)
				switch(type) {
					case 40:	// sizeof(BITMAPINFOHEADER) - all Windows versions since Windows 3.0
						break;
					case 52:	// sizeof(BITMAPV2INFOHEADER) (undocumented)
					case 56:	// sizeof(BITMAPV3INFOHEADER) (undocumented)
					case 108:	// sizeof(BITMAPV4HEADER) - all Windows versions since Windows 95/NT4 (not supported)
					case 124:	// sizeof(BITMAPV5HEADER) - Windows 98/2000 and newer (not supported)
						io->seek_proc(handle, (long)(type - sizeof(BITMAPINFOHEADER)), SEEK_CUR);
						break;
				}
				
				// load the palette

				io->read_proc(Bitmap_GetPalette(dib), used_colors * sizeof(RGBQUAD), 1, handle);
#if BITMAP_COLORORDER == BITMAP_COLORORDER_RGB
				RGBQUAD *pal = Bitmap_GetPalette(dib);
				for(unsigned int i = 0; i < used_colors; i++) {
					BYTE tmp = pal[i].rgbRed;
					pal[i].rgbRed = pal[i].rgbBlue;
					pal[i].rgbBlue = tmp;
				}
#endif

				if(header_only) {
					// header only mode
					return dib;
				}

				// seek to the actual pixel data.
				// this is needed because sometimes the palette is larger than the entries it contains predicts
				io->seek_proc(handle, bitmap_bits_offset, SEEK_SET);

				// read the pixel data

				switch (compression) {
					case BI_RGB :
						if( LoadPixelData(io, handle, dib, height, pitch, bit_count) ) {
							return dib;
						} else {
							throw "Error encountered while decoding BMP data";
						}
						break;

					case BI_RLE4 :
						if( LoadPixelDataRLE4(io, handle, width, height, dib) ) {
							return dib;
						} else {
							throw "Error encountered while decoding RLE4 BMP data";
						}
						break;

					case BI_RLE8 :
						if( LoadPixelDataRLE8(io, handle, width, height, dib) ) {
							return dib;
						} else {
							throw "Error encountered while decoding RLE8 BMP data";
						}
						break;

					default :
						throw FI_MSG_ERROR_UNSUPPORTED_COMPRESSION;
				}
			}
			break; // 1-, 4-, 8-bit

			case 16 :
			{
				int use_bitfields = 0;
				if (bih.biCompression == BI_BITFIELDS) use_bitfields = 3;
				else if (bih.biCompression == BI_ALPHABITFIELDS) use_bitfields = 4;
				else if (type == 52) use_bitfields = 3;
				else if (type >= 56) use_bitfields = 4;
				
				if (use_bitfields > 0) {
 					DWORD bitfields[4];
					io->read_proc(bitfields, use_bitfields * sizeof(DWORD), 1, handle);
					dib = Bitmap_AllocateHeader(header_only, width, height, bit_count, bitfields[0], bitfields[1], bitfields[2]);
				} else {
					dib = Bitmap_AllocateHeader(header_only, width, height, bit_count, FI16_555_RED_MASK, FI16_555_GREEN_MASK, FI16_555_BLUE_MASK);
				}

				if (dib == NULL) {
					throw FI_MSG_ERROR_DIB_MEMORY;						
				}

				// set resolution information
				Bitmap_SetDotsPerMeterX(dib, bih.biXPelsPerMeter);
				Bitmap_SetDotsPerMeterY(dib, bih.biYPelsPerMeter);

				if(header_only) {
					// header only mode
					return dib;
				}
				
				// seek to the actual pixel data
				io->seek_proc(handle, bitmap_bits_offset, SEEK_SET);

				// load pixel data and swap as needed if OS is Big Endian
				LoadPixelData(io, handle, dib, height, pitch, bit_count);

				return dib;
			}
			break; // 16-bit

			case 24 :
			case 32 :
			{
				int use_bitfields = 0;
				if (bih.biCompression == BI_BITFIELDS) use_bitfields = 3;
				else if (bih.biCompression == BI_ALPHABITFIELDS) use_bitfields = 4;
				else if (type == 52) use_bitfields = 3;
				else if (type >= 56) use_bitfields = 4;

 				if (use_bitfields > 0) {
					DWORD bitfields[4];
					io->read_proc(bitfields, use_bitfields * sizeof(DWORD), 1, handle);
					dib = Bitmap_AllocateHeader(header_only, width, height, bit_count, bitfields[0], bitfields[1], bitfields[2]);
				} else {
					if( bit_count == 32 ) {
						dib = Bitmap_AllocateHeader(header_only, width, height, bit_count, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
					} else {
						dib = Bitmap_AllocateHeader(header_only, width, height, bit_count, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
					}
				}

				if (dib == NULL) {
					throw FI_MSG_ERROR_DIB_MEMORY;
				}

				// set resolution information
				Bitmap_SetDotsPerMeterX(dib, bih.biXPelsPerMeter);
				Bitmap_SetDotsPerMeterY(dib, bih.biYPelsPerMeter);

				if(header_only) {
					// header only mode
					return dib;
				}

				// Skip over the optional palette 
				// A 24 or 32 bit DIB may contain a palette for faster color reduction
				// i.e. you can have Bitmap_GetColorsUsed(dib) > 0)

				// seek to the actual pixel data
				io->seek_proc(handle, bitmap_bits_offset, SEEK_SET);

				// read in the bitmap bits
				// load pixel data and swap as needed if OS is Big Endian
				LoadPixelData(io, handle, dib, height, pitch, bit_count);

				// check if the bitmap contains transparency, if so enable it in the header

				Bitmap_SetTransparent(dib, (Bitmap_GetColorType(dib) == FIC_RGBALPHA));

				return dib;
			}
			break; // 24-, 32-bit
		}
	} catch(const char *message) {
		if(dib) {
			Bitmap_Unload(dib);
		}
		if(message) {
			Bitmap_OutputMessageProc(FIF_BMP, message);
		}
	}

	return NULL;
}

// --------------------------------------------------------------------------

static FIBITMAP *
LoadOS22XBMP(BitmapIO *io, fi_handle handle, int flags, unsigned bitmap_bits_offset) {
	FIBITMAP *dib = NULL;

	try {
		BOOL header_only = (flags & FIF_LOAD_NOPIXELS) == FIF_LOAD_NOPIXELS;

		// load the info header

		BITMAPINFOHEADER bih;

		io->read_proc(&bih, sizeof(BITMAPINFOHEADER), 1, handle);
#ifdef BITMAP_BIGENDIAN
		SwapInfoHeader(&bih);
#endif

		// keep some general information about the bitmap

		unsigned used_colors	= bih.biClrUsed;
		int width				= bih.biWidth;
		int height				= bih.biHeight;		// WARNING: height can be < 0 => check each read_proc using 'height' as a parameter
		unsigned bit_count		= bih.biBitCount;
		unsigned compression	= bih.biCompression;
		unsigned pitch			= CalculatePitch(CalculateLine(width, bit_count));
		
		switch (bit_count) {
			case 1 :
			case 4 :
			case 8 :
			{
				if ((used_colors == 0) || (used_colors > CalculateUsedPaletteEntries(bit_count)))
					used_colors = CalculateUsedPaletteEntries(bit_count);
					
				// allocate enough memory to hold the bitmap (header, palette, pixels) and read the palette

				dib = Bitmap_AllocateHeader(header_only, width, height, bit_count);

				if (dib == NULL) {
					throw FI_MSG_ERROR_DIB_MEMORY;
				}

				// set resolution information
				Bitmap_SetDotsPerMeterX(dib, bih.biXPelsPerMeter);
				Bitmap_SetDotsPerMeterY(dib, bih.biYPelsPerMeter);
				
				// load the palette
				// note that it may contain RGB or RGBA values : we will calculate this
				unsigned pal_size = (bitmap_bits_offset - sizeof(BITMAPFILEHEADER) - bih.biSize) / used_colors; 

				io->seek_proc(handle, sizeof(BITMAPFILEHEADER) + bih.biSize, SEEK_SET);

				RGBQUAD *pal = Bitmap_GetPalette(dib);

				if(pal_size == 4) {
					for (unsigned count = 0; count < used_colors; count++) {
						FILE_BGRA bgra;

						io->read_proc(&bgra, sizeof(FILE_BGRA), 1, handle);
						
						pal[count].rgbRed	= bgra.r;
						pal[count].rgbGreen = bgra.g;
						pal[count].rgbBlue	= bgra.b;
					} 
				} else if(pal_size == 3) {
					for (unsigned count = 0; count < used_colors; count++) {
						FILE_BGR bgr;

						io->read_proc(&bgr, sizeof(FILE_BGR), 1, handle);
						
						pal[count].rgbRed	= bgr.r;
						pal[count].rgbGreen = bgr.g;
						pal[count].rgbBlue	= bgr.b;
					} 
				}
				
				if(header_only) {
					// header only mode
					return dib;
				}

				// seek to the actual pixel data.
				// this is needed because sometimes the palette is larger than the entries it contains predicts

				if (bitmap_bits_offset > (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (used_colors * 3))) {
					io->seek_proc(handle, bitmap_bits_offset, SEEK_SET);
				}

				// read the pixel data

				switch (compression) {
					case BI_RGB :
						// load pixel data 
						LoadPixelData(io, handle, dib, height, pitch, bit_count);						
						return dib;

					case BI_RLE4 :
						if( LoadPixelDataRLE4(io, handle, width, height, dib) ) {
							return dib;
						} else {
							throw "Error encountered while decoding RLE4 BMP data";
						}
						break;

					case BI_RLE8 :
						if( LoadPixelDataRLE8(io, handle, width, height, dib) ) {
							return dib;
						} else {
							throw "Error encountered while decoding RLE8 BMP data";
						}
						break;

					default :		
						throw FI_MSG_ERROR_UNSUPPORTED_COMPRESSION;
				}	
			}

			case 16 :
			{
				if (bih.biCompression == 3) {
					DWORD bitfields[3];

					io->read_proc(bitfields, 3 * sizeof(DWORD), 1, handle);

					dib = Bitmap_AllocateHeader(header_only, width, height, bit_count, bitfields[0], bitfields[1], bitfields[2]);
				} else {
					dib = Bitmap_AllocateHeader(header_only, width, height, bit_count, FI16_555_RED_MASK, FI16_555_GREEN_MASK, FI16_555_BLUE_MASK);
				}

				if (dib == NULL) {
					throw FI_MSG_ERROR_DIB_MEMORY;
				}

				// set resolution information
				Bitmap_SetDotsPerMeterX(dib, bih.biXPelsPerMeter);
				Bitmap_SetDotsPerMeterY(dib, bih.biYPelsPerMeter);

				if(header_only) {
					// header only mode
					return dib;
				}

				if (bitmap_bits_offset > (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (used_colors * 3))) {
					io->seek_proc(handle, bitmap_bits_offset, SEEK_SET);
				}

				// load pixel data and swap as needed if OS is Big Endian
				LoadPixelData(io, handle, dib, height, pitch, bit_count);

				return dib;
			}

			case 24 :
			case 32 :
			{
				if( bit_count == 32 ) {
					dib = Bitmap_AllocateHeader(header_only, width, height, bit_count, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
				} else {
					dib = Bitmap_AllocateHeader(header_only, width, height, bit_count, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
				}

				if (dib == NULL) {
					throw FI_MSG_ERROR_DIB_MEMORY;
				}
				
				// set resolution information
				Bitmap_SetDotsPerMeterX(dib, bih.biXPelsPerMeter);
				Bitmap_SetDotsPerMeterY(dib, bih.biYPelsPerMeter);

				if(header_only) {
					// header only mode
					return dib;
				}

				// Skip over the optional palette 
				// A 24 or 32 bit DIB may contain a palette for faster color reduction

				if (bitmap_bits_offset > (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (used_colors * 3))) {
					io->seek_proc(handle, bitmap_bits_offset, SEEK_SET);
				}
				
				// read in the bitmap bits
				// load pixel data and swap as needed if OS is Big Endian
				LoadPixelData(io, handle, dib, height, pitch, bit_count);

				// check if the bitmap contains transparency, if so enable it in the header

				Bitmap_SetTransparent(dib, (Bitmap_GetColorType(dib) == FIC_RGBALPHA));

				return dib;
			}
		}
	} catch(const char *message) {
		if(dib)
			Bitmap_Unload(dib);

		Bitmap_OutputMessageProc(FIF_BMP, message);
	}

	return NULL;
}

// --------------------------------------------------------------------------

static FIBITMAP *
LoadOS21XBMP(BitmapIO *io, fi_handle handle, int flags, unsigned bitmap_bits_offset) {
	FIBITMAP *dib = NULL;

	try {
		BOOL header_only = (flags & FIF_LOAD_NOPIXELS) == FIF_LOAD_NOPIXELS;

		BITMAPINFOOS2_1X_HEADER bios2_1x;

		io->read_proc(&bios2_1x, sizeof(BITMAPINFOOS2_1X_HEADER), 1, handle);
#ifdef BITMAP_BIGENDIAN
		SwapOS21XHeader(&bios2_1x);
#endif
		// keep some general information about the bitmap

		unsigned used_colors = 0;
		unsigned width		= bios2_1x.biWidth;
		unsigned height		= bios2_1x.biHeight;	// WARNING: height can be < 0 => check each read_proc using 'height' as a parameter
		unsigned bit_count	= bios2_1x.biBitCount;
		unsigned pitch		= CalculatePitch(CalculateLine(width, bit_count));
		
		switch (bit_count) {
			case 1 :
			case 4 :
			case 8 :
			{
				used_colors = CalculateUsedPaletteEntries(bit_count);
				
				// allocate enough memory to hold the bitmap (header, palette, pixels) and read the palette

				dib = Bitmap_AllocateHeader(header_only, width, height, bit_count);

				if (dib == NULL) {
					throw FI_MSG_ERROR_DIB_MEMORY;
				}

				// set resolution information to default values (72 dpi in english units)
				Bitmap_SetDotsPerMeterX(dib, 2835);
				Bitmap_SetDotsPerMeterY(dib, 2835);
				
				// load the palette

				RGBQUAD *pal = Bitmap_GetPalette(dib);

				for (unsigned count = 0; count < used_colors; count++) {
					FILE_BGR bgr;

					io->read_proc(&bgr, sizeof(FILE_BGR), 1, handle);
					
					pal[count].rgbRed	= bgr.r;
					pal[count].rgbGreen = bgr.g;
					pal[count].rgbBlue	= bgr.b;
				}
				
				if(header_only) {
					// header only mode
					return dib;
				}

				// Skip over the optional palette 
				// A 24 or 32 bit DIB may contain a palette for faster color reduction

				io->seek_proc(handle, bitmap_bits_offset, SEEK_SET);
				
				// read the pixel data

				// load pixel data 
				LoadPixelData(io, handle, dib, height, pitch, bit_count);
						
				return dib;
			}

			case 16 :
			{
				dib = Bitmap_AllocateHeader(header_only, width, height, bit_count, FI16_555_RED_MASK, FI16_555_GREEN_MASK, FI16_555_BLUE_MASK);

				if (dib == NULL) {
					throw FI_MSG_ERROR_DIB_MEMORY;						
				}

				// set resolution information to default values (72 dpi in english units)
				Bitmap_SetDotsPerMeterX(dib, 2835);
				Bitmap_SetDotsPerMeterY(dib, 2835);

				if(header_only) {
					// header only mode
					return dib;
				}

				// load pixel data and swap as needed if OS is Big Endian
				LoadPixelData(io, handle, dib, height, pitch, bit_count);

				return dib;
			}

			case 24 :
			case 32 :
			{
				if( bit_count == 32 ) {
					dib = Bitmap_AllocateHeader(header_only, width, height, bit_count, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
				} else {
					dib = Bitmap_AllocateHeader(header_only, width, height, bit_count, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
				}

				if (dib == NULL) {
					throw FI_MSG_ERROR_DIB_MEMORY;						
				}

				// set resolution information to default values (72 dpi in english units)
				Bitmap_SetDotsPerMeterX(dib, 2835);
				Bitmap_SetDotsPerMeterY(dib, 2835);

				if(header_only) {
					// header only mode
					return dib;
				}

				// Skip over the optional palette 
				// A 24 or 32 bit DIB may contain a palette for faster color reduction

				// load pixel data and swap as needed if OS is Big Endian
				LoadPixelData(io, handle, dib, height, pitch, bit_count);

				// check if the bitmap contains transparency, if so enable it in the header

				Bitmap_SetTransparent(dib, (Bitmap_GetColorType(dib) == FIC_RGBALPHA));

				return dib;
			}
		}
	} catch(const char *message) {	
		if(dib)
			Bitmap_Unload(dib);

		Bitmap_OutputMessageProc(FIF_BMP, message);
	}

	return NULL;
}


BOOL
ValidateBMP(BitmapIO *io, fi_handle handle) {
	BYTE bmp_signature1[] = { 0x42, 0x4D };
	BYTE bmp_signature2[] = { 0x42, 0x41 };
	BYTE signature[2] = { 0, 0 };

	io->read_proc(signature, 1, sizeof(bmp_signature1), handle);

	if (memcmp(bmp_signature1, signature, sizeof(bmp_signature1)) == 0)
		return TRUE;

	if (memcmp(bmp_signature2, signature, sizeof(bmp_signature2)) == 0)
		return TRUE;

	return FALSE;
}

FIBITMAP *
LoadBMP(BitmapIO *io, fi_handle handle, int page, int flags) {
	if (handle != NULL) {
		BITMAPFILEHEADER bitmapfileheader;
		DWORD type = 0;

		// we use this offset value to make seemingly absolute seeks relative in the file
		
		long offset_in_file = io->tell_proc(handle);

		// read the fileheader

		io->read_proc(&bitmapfileheader, sizeof(BITMAPFILEHEADER), 1, handle);
#ifdef BITMAP_BIGENDIAN
		SwapFileHeader(&bitmapfileheader);
#endif

		// check the signature

		if((bitmapfileheader.bfType != 0x4D42) && (bitmapfileheader.bfType != 0x4142)) {
			Bitmap_OutputMessageProc(FIF_BMP, FI_MSG_ERROR_MAGIC_NUMBER);
			return NULL;
		}

		// read the first byte of the infoheader

		io->read_proc(&type, sizeof(DWORD), 1, handle);
		io->seek_proc(handle, 0 - (long)sizeof(DWORD), SEEK_CUR);
#ifdef BITMAP_BIGENDIAN
		SwapLong(&type);
#endif

		// call the appropriate load function for the found bitmap type

		switch(type) {
			case 12:
				// OS/2 and also all Windows versions since Windows 3.0
				return LoadOS21XBMP(io, handle, flags, offset_in_file + bitmapfileheader.bfOffBits);

			case 64:
				// OS/2
				return LoadOS22XBMP(io, handle, flags, offset_in_file + bitmapfileheader.bfOffBits);

			case 40:	// BITMAPINFOHEADER - all Windows versions since Windows 3.0
			case 52:	// BITMAPV2INFOHEADER (undocumented, partially supported)
			case 56:	// BITMAPV3INFOHEADER (undocumented, partially supported)
			case 108:	// BITMAPV4HEADER - all Windows versions since Windows 95/NT4 (partially supported)
			case 124:	// BITMAPV5HEADER - Windows 98/2000 and newer (partially supported)
				return LoadWindowsBMP(io, handle, flags, offset_in_file + bitmapfileheader.bfOffBits, type);

			default:
				break;
		}

		Bitmap_OutputMessageProc(FIF_BMP, "unknown bmp subtype with id %d", type);
	}

	return NULL;
}
