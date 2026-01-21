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
// PCX Loader
//
// ==========================================================

#include "bitmap.h"
#include "utilities.h"

// ----------------------------------------------------------
//   Constants + headers
// ----------------------------------------------------------

#define IO_BUF_SIZE	2048

// ----------------------------------------------------------

#ifdef _WIN32
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef struct tagPCXHEADER {
	BYTE  manufacturer;		// Magic number (0x0A = ZSoft Z)
	BYTE  version;			// Version	0 == 2.5
							//          2 == 2.8 with palette info
							//          3 == 2.8 without palette info
							//          5 == 3.0 with palette info
	BYTE  encoding;			// Encoding: 0 = uncompressed, 1 = PCX rle compressed
	BYTE  bpp;				// Bits per pixel per plane (only 1 or 8)
	WORD  window[4];		// left, upper, right,lower pixel coord.
	WORD  hdpi;				// Horizontal resolution
	WORD  vdpi;				// Vertical resolution
	BYTE  color_map[48];	// Colormap for 16-color images
	BYTE  reserved;
	BYTE  planes;			// Number of planes (1, 3 or 4)
	WORD  bytes_per_line;	// Bytes per row (always even)
	WORD  palette_info;		// Palette information (1 = color or b&w; 2 = gray scale)
	WORD  h_screen_size;
	WORD  v_screen_size;
	BYTE  filler[54];		// Reserved filler
} PCXHEADER;
		
#ifdef _WIN32
#pragma pack(pop)
#else
#pragma pack()
#endif

// ==========================================================
// Internal functions
// ==========================================================

static BOOL 
pcx_validate(BitmapIO *io, fi_handle handle) {
	BYTE pcx_signature = 0x0A;
	BYTE signature[4] = { 0, 0, 0, 0 };

	if(io->read_proc(&signature, 1, 4, handle) != 4) {
		return FALSE;
	}
	// magic number (0x0A = ZSoft Z)
	if(signature[0] == pcx_signature) {
		// version
		if(signature[1] <= 5) {
			// encoding
			if((signature[2] == 0) || (signature[2] == 1)) {
				// bits per pixel per plane
				if((signature[3] == 1) || (signature[3] == 8)) {
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

static unsigned
readline(BitmapIO *io, fi_handle handle, BYTE *buffer, unsigned length, BOOL rle, BYTE * ReadBuf, int * ReadPos) {
	// -----------------------------------------------------------//
	// Read either run-length encoded or normal image data        //
	//                                                            //
	//       THIS IS HOW RUNTIME LENGTH ENCODING WORKS IN PCX:    //
	//                                                            //
	//  1) If the upper 2 bits of a byte are set,                 //
	//     the lower 6 bits specify the count for the next byte   //
	//                                                            //
	//  2) If the upper 2 bits of the byte are clear,             //
	//     the byte is actual data with a count of 1              //
	//                                                            //
	//  Note that a scanline always has an even number of bytes   //
	// -------------------------------------------------------------

	BYTE count = 0, value = 0;
	unsigned written = 0;

	if (rle) {
		// run-length encoded read

		while (length--) {
			if (count == 0) {
				if (*ReadPos >= IO_BUF_SIZE - 1 ) {
					if (*ReadPos == IO_BUF_SIZE - 1) {
						// we still have one BYTE, copy it to the start pos

						*ReadBuf = ReadBuf[IO_BUF_SIZE - 1];

						io->read_proc(ReadBuf + 1, 1, IO_BUF_SIZE - 1, handle);
					} else {
						// read the complete buffer

						io->read_proc(ReadBuf, 1, IO_BUF_SIZE, handle);
					}

					*ReadPos = 0;
				}

				value = *(ReadBuf + (*ReadPos)++);

				if ((value & 0xC0) == 0xC0) {
					count = value & 0x3F;
					value = *(ReadBuf + (*ReadPos)++);
				} else {
					count = 1;
				}
			}

			count--;

			*(buffer + written++) = value;
		}

	} else {
		// normal read

		written = io->read_proc(buffer, length, 1, handle);
	}

	return written;
}

#ifdef BITMAP_BIGENDIAN
static void
SwapHeader(PCXHEADER *header) {
	SwapShort(&header->window[0]);
	SwapShort(&header->window[1]);
	SwapShort(&header->window[2]);
	SwapShort(&header->window[3]);
	SwapShort(&header->hdpi);
	SwapShort(&header->vdpi);
	SwapShort(&header->bytes_per_line);
	SwapShort(&header->palette_info);
	SwapShort(&header->h_screen_size);
	SwapShort(&header->v_screen_size);
}
#endif


/*!
    Validates a bitmap by reading the first few bytes
	and comparing them with a known bitmap signature.
	TRUE is returned if the bytes match the signature, FALSE otherwise.
	The Validate function is used by using Bitmap_GetFileType.
	
	Note: a plugin can safely read data any data from the bitmap without seeking back
	to the original entry point; the entry point is stored prior to calling this
	function and restored after.

    Note: because of Bitmap's io redirection support, the header for the bitmap
	must be on the start of the bitmap or at least on a known part in the bitmap. It is
	forbidden to seek to the end of the bitmap or to a point relative to the end of a bitmap,
	because the end of the bitmap is not always known.
*/

BOOL
ValidatePCX(BitmapIO *io, fi_handle handle) {
	return pcx_validate(io, handle);
}

/*!
    Loads a bitmap into memory. On entry it is assumed that
	the bitmap to be loaded is of the correct type. If the bitmap
	is of an incorrect type, the plugin might not gracefully fail but
	crash or enter an endless loop. It is also assumed that all
	the bitmap data is available at one time. If the bitmap is not complete,
	for example because it is being downloaded while loaded, the plugin
	might also not gracefully fail.

	The Load function has the following parameters:

    The first parameter (BitmapIO *io) is a structure providing
	function pointers in order to make use of Bitmap's IO redirection. Using
	Bitmap's file i/o functions instead of standard ones it is garantueed
	that all bitmap types, both current and future ones, can be loaded from
	memory, file cabinets, the internet and more. The second parameter (fi_handle handle)
	is a companion of BitmapIO and can be best compared with the standard FILE* type,
	in a generalized form.

	The third parameter (int page) indicates wether we will be loading a certain page
	in the bitmap or if we will load the default one. This parameter is only used if
	the plugin supports multi-paged bitmaps, e.g. cabinet bitmaps that contain a series
	of images or pages. If the plugin does support multi-paging, the page parameter
	can contain either a number higher or equal to 0 to load a certain page, or -1 to 
	load the default page. If the plugin does not support multi-paging,
	the page parameter is always -1.
	
	The fourth parameter (int flags) manipulates the load function to load a bitmap
	in a certain way. Every plugin has a different flag parameter with different meanings.

	The last parameter (void *data) can contain a special data block used when
	the file is read multi-paged. Because not every plugin supports multi-paging
	not every plugin will use the data parameter and it will be set to NULL.However,
	when the plugin does support multi-paging the parameter contains a pointer to a
	block of data allocated by the Open function.
*/

FIBITMAP *
LoadPCX(BitmapIO *io, fi_handle handle, int page, int flags) {
	FIBITMAP *dib = NULL;
	BYTE *bits;			  // Pointer to dib data
	RGBQUAD *pal;		  // Pointer to dib palette
	BYTE *line = NULL;	  // PCX raster line
	BYTE *ReadBuf = NULL; // buffer;
	BOOL bIsRLE;		  // True if the file is run-length encoded

	if(!handle) {
		return NULL;
	}

	BOOL header_only = (flags & FIF_LOAD_NOPIXELS) == FIF_LOAD_NOPIXELS;

	try {
		// check PCX identifier

		long start_pos = io->tell_proc(handle);
		BOOL validated = pcx_validate(io, handle);		
		io->seek_proc(handle, start_pos, SEEK_SET);
		if(!validated) {
			throw FI_MSG_ERROR_MAGIC_NUMBER;
		}

		// process the header

		PCXHEADER header;

		if(io->read_proc(&header, sizeof(PCXHEADER), 1, handle) != 1) {
			throw FI_MSG_ERROR_PARSING;
		}
#ifdef BITMAP_BIGENDIAN
		SwapHeader(&header);
#endif

		// allocate a new DIB

		unsigned width = header.window[2] - header.window[0] + 1;
		unsigned height = header.window[3] - header.window[1] + 1;
		unsigned bitcount = header.bpp * header.planes;

		if (bitcount == 24) {
			dib = Bitmap_AllocateHeader(header_only, width, height, bitcount, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
		} else {
			dib = Bitmap_AllocateHeader(header_only, width, height, bitcount);
		}

		// if the dib couldn't be allocated, throw an error

		if (!dib) {
			throw FI_MSG_ERROR_DIB_MEMORY;
		}

		// metrics handling code

		Bitmap_SetDotsPerMeterX(dib, (unsigned)(((float)header.hdpi) / 0.0254000 + 0.5));
		Bitmap_SetDotsPerMeterY(dib, (unsigned)(((float)header.vdpi) / 0.0254000 + 0.5));

		// Set up the palette if needed
		// ----------------------------

		switch(bitcount) {
			case 1:
			{
				pal = Bitmap_GetPalette(dib);
				pal[0].rgbRed = pal[0].rgbGreen = pal[0].rgbBlue = 0;
				pal[1].rgbRed = pal[1].rgbGreen = pal[1].rgbBlue = 255;
				break;
			}

			case 4:
			{
				pal = Bitmap_GetPalette(dib);

				BYTE *pColormap = &header.color_map[0];

				for (int i = 0; i < 16; i++) {
					pal[i].rgbRed   = pColormap[0];
					pal[i].rgbGreen = pColormap[1];
					pal[i].rgbBlue  = pColormap[2];
					pColormap += 3;
				}

				break;
			}

			case 8:
			{
				BYTE palette_id;

				io->seek_proc(handle, -769L, SEEK_END);
				io->read_proc(&palette_id, 1, 1, handle);

				if (palette_id == 0x0C) {
					BYTE *cmap = (BYTE*)malloc(768 * sizeof(BYTE));
					io->read_proc(cmap, 768, 1, handle);

					pal = Bitmap_GetPalette(dib);
					BYTE *pColormap = &cmap[0];

					for(int i = 0; i < 256; i++) {
						pal[i].rgbRed   = pColormap[0];
						pal[i].rgbGreen = pColormap[1];
						pal[i].rgbBlue  = pColormap[2];
						pColormap += 3;
					}

					free(cmap);
				}

				// wrong palette ID, perhaps a gray scale is needed ?

				else if (header.palette_info == 2) {
					pal = Bitmap_GetPalette(dib);

					for(int i = 0; i < 256; i++) {
						pal[i].rgbRed   = (BYTE)i;
						pal[i].rgbGreen = (BYTE)i;
						pal[i].rgbBlue  = (BYTE)i;
					}
				}

				io->seek_proc(handle, (long)sizeof(PCXHEADER), SEEK_SET);
			}
			break;
		}

		if(header_only) {
			// header only mode
			return dib;
		}

		// calculate the line length for the PCX and the DIB

		// length of raster line in bytes
		unsigned linelength = header.bytes_per_line * header.planes;
		// length of DIB line (rounded to DWORD) in bytes
		unsigned pitch = Bitmap_GetPitch(dib);

		// run-length encoding ?

		bIsRLE = (header.encoding == 1) ? TRUE : FALSE;

		// load image data
		// ---------------

		line = (BYTE*)malloc(linelength * sizeof(BYTE));
		if(!line) throw FI_MSG_ERROR_MEMORY;
		
		ReadBuf = (BYTE*)malloc(IO_BUF_SIZE * sizeof(BYTE));
		if(!ReadBuf) throw FI_MSG_ERROR_MEMORY;
		
		bits = Bitmap_GetScanLine(dib, height - 1);

		int ReadPos = IO_BUF_SIZE;

		if ((header.planes == 1) && ((header.bpp == 1) || (header.bpp == 8))) {
			BYTE skip;
			unsigned written;

			for (unsigned y = 0; y < height; y++) {
				written = readline(io, handle, bits, linelength, bIsRLE, ReadBuf, &ReadPos);

				// skip trailing garbage at the end of the scanline

				for (unsigned count = written; count < linelength; count++) {
					if (ReadPos < IO_BUF_SIZE) {
						ReadPos++;
					} else {
						io->read_proc(&skip, sizeof(BYTE), 1, handle);
					}
				}

				bits -= pitch;
			}
		} else if ((header.planes == 4) && (header.bpp == 1)) {
			BYTE bit,  mask, skip;
			unsigned index;
			BYTE *buffer;
			unsigned x, y, written;

			buffer = (BYTE*)malloc(width * sizeof(BYTE));
			if(!buffer) throw FI_MSG_ERROR_MEMORY;

			for (y = 0; y < height; y++) {
				written = readline(io, handle, line, linelength, bIsRLE, ReadBuf, &ReadPos);

				// build a nibble using the 4 planes

				memset(buffer, 0, width * sizeof(BYTE));

				for(int plane = 0; plane < 4; plane++) {
					bit = (BYTE)(1 << plane);

					for (x = 0; x < width; x++) {
						index = (unsigned)((x / 8) + plane * header.bytes_per_line);
						mask = (BYTE)(0x80 >> (x & 0x07));
						buffer[x] |= (line[index] & mask) ? bit : 0;
					}
				}

				// then write the DIB row

				for (x = 0; x < width / 2; x++) {
					bits[x] = (buffer[2*x] << 4) | buffer[2*x+1];
				}

				// skip trailing garbage at the end of the scanline

				for (unsigned count = written; count < linelength; count++) {
					if (ReadPos < IO_BUF_SIZE) {
						ReadPos++;
					} else {
						io->read_proc(&skip, sizeof(BYTE), 1, handle);
					}
				}

				bits -= pitch;
			}

			free(buffer);

		} else if((header.planes == 3) && (header.bpp == 8)) {
			BYTE *pline;

			for (unsigned y = 0; y < height; y++) {
				readline(io, handle, line, linelength, bIsRLE, ReadBuf, &ReadPos);

				// convert the plane stream to BGR (RRRRGGGGBBBB -> BGRBGRBGRBGR)
				// well, now with the FI_RGBA_x macros, on BIGENDIAN we convert to RGB

				pline = line;
				unsigned x;

				for (x = 0; x < width; x++) {
					bits[x * 3 + FI_RGBA_RED] = pline[x];						
				}
				pline += header.bytes_per_line;

				for (x = 0; x < width; x++) {
					bits[x * 3 + FI_RGBA_GREEN] = pline[x];
				}
				pline += header.bytes_per_line;

				for (x = 0; x < width; x++) {
					bits[x * 3 + FI_RGBA_BLUE] = pline[x];
				}
				pline += header.bytes_per_line;

				bits -= pitch;
			}
		} else {
			throw FI_MSG_ERROR_UNSUPPORTED_FORMAT;
		}

		free(line);
		free(ReadBuf);

		return dib;

	} catch (const char *text) {
		// free allocated memory

		if (dib != NULL) {
			Bitmap_Unload(dib);
		}
		if (line != NULL) {
			free(line);
		}
		if (ReadBuf != NULL) {
			free(ReadBuf);
		}

		Bitmap_OutputMessageProc(FIF_PCX, text);
	}
	
	return NULL;
}
