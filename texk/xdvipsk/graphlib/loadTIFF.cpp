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
// TIFF Loader
//
// ==========================================================

#ifdef _MSC_VER 
#pragma warning (disable : 4786) // identifier was truncated to 'number' characters
#endif

#ifdef unix
#undef unix
#endif
#ifdef __unix
#undef __unix
#endif

#include "bitmap.h"
#include "utilities.h"
#include "tiffiop.h"

#include "bitmapIO.h"


// --------------------------------------------------------------------------
//   LogLuv conversion functions interface (see TIFFLogLuv.cpp)
// --------------------------------------------------------------------------
void tiff_ConvertLineXYZToRGB(BYTE *target, BYTE *source, double stonits, int width_in_pixels);
void tiff_ConvertLineRGBToXYZ(BYTE *target, BYTE *source, int width_in_pixels);

// ----------------------------------------------------------

/** Supported loading methods */
typedef enum {
	LoadAsRBGA			= 0, 
	LoadAsCMYK			= 1, 
	LoadAs8BitTrns		= 2, 
	LoadAsGenericStrip	= 3, 
	LoadAsTiled			= 4,
	LoadAsLogLuv		= 5,
	LoadAsHalfFloat		= 6
} TIFFLoadMethod;

// ----------------------------------------------------------
//   local prototypes
// ----------------------------------------------------------

static tmsize_t _tiffReadProc(thandle_t handle, void* buf, tmsize_t size);
static tmsize_t _tiffWriteProc(thandle_t handle, void* buf, tmsize_t size);
static toff_t _tiffSeekProc(thandle_t handle, toff_t off, int whence);
static int _tiffCloseProc(thandle_t fd);
static int _tiffMapProc(thandle_t fd, void** pbase, toff_t* psize);
static void _tiffUnmapProc(thandle_t fd, void* base, toff_t size);

static uint16 CheckColormap(int n, uint16* r, uint16* g, uint16* b);
static uint16 GetPhotometric(FIBITMAP *dib);

static void ReadResolution(TIFF *tiff, FIBITMAP *dib);

static void ReadPalette(TIFF *tiff, uint16 photometric, uint16 bitspersample, FIBITMAP *dib);

static FIBITMAP* CreateImageType(BOOL header_only, BITMAP_TYPE fit, int width, int height, uint16 bitspersample, uint16 samplesperpixel);
static BITMAP_TYPE ReadImageType(TIFF *tiff, uint16 bitspersample, uint16 samplesperpixel);

static TIFFLoadMethod FindLoadMethod(TIFF *tif, uint16 photometric, uint16 bitspersample, uint16 samplesperpixel, BITMAP_TYPE image_type, int flags);


typedef struct {
	BitmapIO *io;
	fi_handle handle;
	TIFF *tif;
} fi_TIFFIO;

// ----------------------------------------------------------
//   libtiff interface 
// ----------------------------------------------------------

static tmsize_t 
_tiffReadProc(thandle_t handle, void *buf, tmsize_t size) {
	fi_TIFFIO *fio = (fi_TIFFIO*)handle;
	return fio->io->read_proc(buf, (unsigned)size, 1, fio->handle) * size;
}

static tmsize_t
_tiffWriteProc(thandle_t handle, void *buf, tmsize_t size) {
	fi_TIFFIO *fio = (fi_TIFFIO*)handle;
	return fio->io->write_proc(buf, (unsigned)size, 1, fio->handle) * size;
}

static toff_t
_tiffSeekProc(thandle_t handle, toff_t off, int whence) {
	fi_TIFFIO *fio = (fi_TIFFIO*)handle;
	fio->io->seek_proc(fio->handle, (long)off, whence);
	return fio->io->tell_proc(fio->handle);
}

static int
_tiffCloseProc(thandle_t fd) {
	return 0;
}

#include <sys/stat.h>

static toff_t
_tiffSizeProc(thandle_t handle) {
    fi_TIFFIO *fio = (fi_TIFFIO*)handle;
    long currPos = fio->io->tell_proc(fio->handle);
    fio->io->seek_proc(fio->handle, 0, SEEK_END);
    long fileSize = fio->io->tell_proc(fio->handle);
    fio->io->seek_proc(fio->handle, currPos, SEEK_SET);
    return fileSize;
}

static int
_tiffMapProc(thandle_t, void** base, toff_t* size) {
	return 0;
}

static void
_tiffUnmapProc(thandle_t, void* base, toff_t size) {
}

/**
Open a TIFF file descriptor for reading or writing
@param handle File handle
@param name Name of the file handle
@param mode Specifies if the file is to be opened for reading ("r") or writing ("w")
*/
TIFF *
TIFFFdOpen(thandle_t handle, const char *name, const char *mode) {
	TIFF *tif;
	
	// Open the file; the callback will set everything up
	tif = TIFFClientOpen(name, mode, handle,
	    _tiffReadProc, _tiffWriteProc, _tiffSeekProc, _tiffCloseProc,
	    _tiffSizeProc, _tiffMapProc, _tiffUnmapProc);

	return tif;
}

/**
Open a TIFF file for reading or writing
@param name
@param mode
*/
TIFF*
TIFFOpen(const char* name, const char* mode) {
	return 0;
}

// ----------------------------------------------------------
//   TIFF library Bitmap-specific routines.
// ----------------------------------------------------------

void*
_TIFFmalloc(tmsize_t s) {
	return malloc(s);
}

void*
_TIFFcalloc(tmsize_t nmemb, tmsize_t siz) {
	return calloc(nmemb, siz);
}

void
_TIFFfree(void *p) {
	free(p);
}

void*
_TIFFrealloc(void* p, tmsize_t s) {
	return realloc(p, s);
}

void
_TIFFmemset(void* p, int v, tmsize_t c) {
	memset(p, v, (size_t) c);
}

void
_TIFFmemcpy(void* d, const void* s, tmsize_t c) {
	memcpy(d, s, (size_t) c);
}

int
_TIFFmemcmp(const void* p1, const void* p2, tmsize_t c) {
	return (memcmp(p1, p2, (size_t) c));
}

// ----------------------------------------------------------
//   in Bitmap warnings and errors are disabled
// ----------------------------------------------------------

static void
msdosWarningHandler(const char* module, const char* fmt, va_list ap) {
}

TIFFErrorHandler _TIFFwarningHandler = msdosWarningHandler;

static void
msdosErrorHandler(const char* module, const char* fmt, va_list ap) {
	
	// use this for diagnostic only (do not use otherwise, even in DEBUG mode)
	/*
	if (module != NULL) {
		char msg[1024];
		vsprintf(msg, fmt, ap);
		Bitmap_OutputMessageProc(FIF_TIFF, "%s: %s", module, msg);
	}
	*/
}

TIFFErrorHandler _TIFFerrorHandler = msdosErrorHandler;

// ----------------------------------------------------------

#define CVT(x)      (((x) * 255L) / ((1L<<16)-1))
#define	SCALE(x)	(((x)*((1L<<16)-1))/255)

// ==========================================================
// Internal functions
// ==========================================================

static uint16
CheckColormap(int n, uint16* r, uint16* g, uint16* b) {
    while (n-- > 0) {
        if (*r++ >= 256 || *g++ >= 256 || *b++ >= 256) {
			return 16;
		}
	}

    return 8;
}

/**
Get the TIFFTAG_PHOTOMETRIC value from the dib
*/
static uint16
GetPhotometric(FIBITMAP *dib) {
	BITMAP_COLOR_TYPE color_type = Bitmap_GetColorType(dib);
	switch(color_type) {
		case FIC_MINISWHITE:	// min value is white
			return PHOTOMETRIC_MINISWHITE;
		case FIC_MINISBLACK:	// min value is black
			return PHOTOMETRIC_MINISBLACK;
		case FIC_PALETTE:		// color map indexed
			return PHOTOMETRIC_PALETTE;
		case FIC_RGB:			// RGB color model
		case FIC_RGBALPHA:		// RGB color model with alpha channel
			return PHOTOMETRIC_RGB;
		case FIC_CMYK:			// CMYK color model
			return PHOTOMETRIC_RGB;	// default to RGB unless the save flag is set to TIFF_CMYK
		default:
			return PHOTOMETRIC_MINISBLACK;
	}
}

/**
Get the resolution from the TIFF and fill the dib with universal units
*/
static void 
ReadResolution(TIFF *tiff, FIBITMAP *dib) {
	float fResX = 300.0;
	float fResY = 300.0;
	uint16 resUnit = RESUNIT_INCH;

	TIFFGetField(tiff, TIFFTAG_RESOLUTIONUNIT, &resUnit);
	TIFFGetField(tiff, TIFFTAG_XRESOLUTION, &fResX);
	TIFFGetField(tiff, TIFFTAG_YRESOLUTION, &fResY);
	
	// If we don't have a valid resolution unit and valid resolution is specified then assume inch
	if (resUnit == RESUNIT_NONE && fResX > 0.0 && fResY > 0.0) {
		resUnit = RESUNIT_INCH;
	}
	if (resUnit == RESUNIT_INCH) {
		Bitmap_SetDotsPerMeterX(dib, (unsigned)(fResX / 0.0254000 + 0.5));
		Bitmap_SetDotsPerMeterY(dib, (unsigned)(fResY / 0.0254000 + 0.5));
	} else if(resUnit == RESUNIT_CENTIMETER) {
		Bitmap_SetDotsPerMeterX(dib, (unsigned)(fResX*100.0 + 0.5));
		Bitmap_SetDotsPerMeterY(dib, (unsigned)(fResY*100.0 + 0.5));
	}
}

/**
Fill the dib palette according to the TIFF photometric
*/
static void 
ReadPalette(TIFF *tiff, uint16 photometric, uint16 bitspersample, FIBITMAP *dib) {
	RGBQUAD *pal = Bitmap_GetPalette(dib);

	switch(photometric) {
		case PHOTOMETRIC_MINISBLACK:	// bitmap and greyscale image types
		case PHOTOMETRIC_MINISWHITE:
			// Monochrome image

			if (bitspersample == 1) {
				if (photometric == PHOTOMETRIC_MINISWHITE) {
					pal[0].rgbRed = pal[0].rgbGreen = pal[0].rgbBlue = 255;
					pal[1].rgbRed = pal[1].rgbGreen = pal[1].rgbBlue = 0;
				} else {
					pal[0].rgbRed = pal[0].rgbGreen = pal[0].rgbBlue = 0;
					pal[1].rgbRed = pal[1].rgbGreen = pal[1].rgbBlue = 255;
				}

			} else if ((bitspersample == 4) ||(bitspersample == 8)) {
				// need to build the scale for greyscale images
				int ncolors = Bitmap_GetColorsUsed(dib);

				if (photometric == PHOTOMETRIC_MINISBLACK) {
					for (int i = 0; i < ncolors; i++) {
						pal[i].rgbRed	=
						pal[i].rgbGreen =
						pal[i].rgbBlue	= (BYTE)(i*(255/(ncolors-1)));
					}
				} else {
					for (int i = 0; i < ncolors; i++) {
						pal[i].rgbRed	=
						pal[i].rgbGreen =
						pal[i].rgbBlue	= (BYTE)(255-i*(255/(ncolors-1)));
					}
				}
			}

			break;

		case PHOTOMETRIC_PALETTE:	// color map indexed
			uint16 *red;
			uint16 *green;
			uint16 *blue;
			
			TIFFGetField(tiff, TIFFTAG_COLORMAP, &red, &green, &blue); 

			// load the palette in the DIB

			if (CheckColormap(1<<bitspersample, red, green, blue) == 16) {
				for (int i = (1 << bitspersample) - 1; i >= 0; i--) {
					pal[i].rgbRed =(BYTE) CVT(red[i]);
					pal[i].rgbGreen = (BYTE) CVT(green[i]);
					pal[i].rgbBlue = (BYTE) CVT(blue[i]);           
				}
			} else {
				for (int i = (1 << bitspersample) - 1; i >= 0; i--) {
					pal[i].rgbRed = (BYTE) red[i];
					pal[i].rgbGreen = (BYTE) green[i];
					pal[i].rgbBlue = (BYTE) blue[i];        
				}
			}

			break;
	}
}

/** 
Allocate a FIBITMAP
@param header_only If TRUE, allocate a 'header only' FIBITMAP, otherwise allocate a full FIBITMAP
@param fit Image type
@param width Image width in pixels
@param height Image height in pixels
@param bitspersample # bits per sample
@param samplesperpixel # samples per pixel
@return Returns the allocated image if successful, returns NULL otherwise
*/
static FIBITMAP* 
CreateImageType(BOOL header_only, BITMAP_TYPE fit, int width, int height, uint16 bitspersample, uint16 samplesperpixel) {
	FIBITMAP *dib = NULL;

	if((width < 0) || (height < 0)) {
		// check for malicious images
		return NULL;
	}

	int bpp = bitspersample * samplesperpixel;

	if(fit == FIT_BITMAP) {
		// standard bitmap type 

		if(bpp == 16) {
			
			if((samplesperpixel == 2) && (bitspersample == 8)) {
				// 8-bit indexed + 8-bit alpha channel -> convert to 8-bit transparent
				dib = Bitmap_AllocateHeader(header_only, width, height, 8);
			} else {
				// 16-bit RGB -> expect it to be 565
				dib = Bitmap_AllocateHeader(header_only, width, height, bpp, FI16_565_RED_MASK, FI16_565_GREEN_MASK, FI16_565_BLUE_MASK);
			}
			
		}
		else {

			dib = Bitmap_AllocateHeader(header_only, width, height, MIN(bpp, 32), FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
		}


	} else {
		// other bitmap types
		
		dib = Bitmap_AllocateHeaderT(header_only, fit, width, height, bpp);
	}

	return dib;
}

/** 
Read the TIFFTAG_SAMPLEFORMAT tag and convert to FREE_IMAGE_TYPE
@param tiff LibTIFF TIFF Handle
@param bitspersample # bit per sample
@param samplesperpixel # samples per pixel
@return Returns the image type as a FREE_IMAGE_TYPE value
*/
static BITMAP_TYPE 
ReadImageType(TIFF *tiff, uint16 bitspersample, uint16 samplesperpixel) {
	uint16 sampleformat = 0;
	BITMAP_TYPE fit = FIT_BITMAP ; 

	uint16 bpp = bitspersample * samplesperpixel;

	// try the sampleformat tag
    if(TIFFGetField(tiff, TIFFTAG_SAMPLEFORMAT, &sampleformat)) {

        switch (sampleformat) {
			case SAMPLEFORMAT_UINT:
				switch (bpp) {
					case 1:
					case 4:
					case 8:
					case 24:
						fit = FIT_BITMAP;
						break;
					case 16:
						// 8-bit + alpha or 16-bit greyscale
						if(samplesperpixel == 2) {
							fit = FIT_BITMAP;
						} else {
							fit = FIT_UNKNOWN;
						}
						break;
					case 32:
						if(samplesperpixel == 4) {
							fit = FIT_BITMAP;
						} else {
							fit = FIT_UNKNOWN;
						}
						break;
					case 48:
						if(samplesperpixel == 3) {
							fit = FIT_UNKNOWN;
						}
						break;
					case 64:
						if(samplesperpixel == 4) {
							fit = FIT_UNKNOWN;
						}
						break;
				}
				break;

			case SAMPLEFORMAT_INT:
				switch (bpp) {
					case 16:
						if(samplesperpixel == 3) {
							fit = FIT_BITMAP;
						} else {
							fit = FIT_UNKNOWN;
						}
						break;
					case 32:
						fit = FIT_UNKNOWN;
						break;
				}
				break;

			case SAMPLEFORMAT_IEEEFP:
				switch (bpp) {
					case 32:
						fit = FIT_UNKNOWN;
						break;
					case 48:
						// 3 x half float => convert to RGBF
						if((samplesperpixel == 3) && (bitspersample == 16)) {
							fit = FIT_UNKNOWN;
						}
						break;
					case 64:
						if(samplesperpixel == 2) {
							fit = FIT_UNKNOWN;
						} else {
							fit = FIT_UNKNOWN;
						}
						break;
					case 96:
						fit = FIT_UNKNOWN;
						break;
					default:
						if(bpp >= 128) {
							fit = FIT_UNKNOWN;
						}
					break;
				}
				break;
			case SAMPLEFORMAT_COMPLEXIEEEFP:
				switch (bpp) {
					case 64:
						break;
					case 128:
						fit = FIT_UNKNOWN;
						break;
				}
				break;

			}
    }
	// no sampleformat tag : assume SAMPLEFORMAT_UINT
	else {
		if(samplesperpixel == 1) {
			switch (bpp) {
				case 16:
					fit = FIT_UNKNOWN;
					break;
					
				case 32:
					fit = FIT_UNKNOWN;
					break;
			}
		}
		else if(samplesperpixel == 3) {
			if (bpp == 48) fit = FIT_UNKNOWN;
		}
		else if(samplesperpixel >= 4) { 
			if(bitspersample == 16) {
				fit = FIT_UNKNOWN;
			}
		}

	}

    return fit;
}


// ----------------------------------------------------------


BOOL
ValidateTIFF(BitmapIO *io, fi_handle handle) {	
	BYTE tiff_id1[] = { 0x49, 0x49, 0x2A, 0x00 };	// Classic TIFF, little-endian
	BYTE tiff_id2[] = { 0x4D, 0x4D, 0x00, 0x2A };	// Classic TIFF, big-endian
	BYTE tiff_id3[] = { 0x49, 0x49, 0x2B, 0x00 };	// Big TIFF, little-endian
	BYTE tiff_id4[] = { 0x4D, 0x4D, 0x00, 0x2B };	// Big TIFF, big-endian
	BYTE signature[4] = { 0, 0, 0, 0 };

	io->read_proc(signature, 1, 4, handle);

	if(memcmp(tiff_id1, signature, 4) == 0)
		return TRUE;
	if(memcmp(tiff_id2, signature, 4) == 0)
		return TRUE;
	if(memcmp(tiff_id3, signature, 4) == 0)
		return TRUE;
	if(memcmp(tiff_id4, signature, 4) == 0)
		return TRUE;

	return FALSE;
}

// ----------------------------------------------------------

static void *
Open(BitmapIO *io, fi_handle handle, BOOL read) {
	// wrapper for TIFF I/O
	fi_TIFFIO *fio = (fi_TIFFIO*)malloc(sizeof(fi_TIFFIO));
	if(!fio) return NULL;
	fio->io = io;
	fio->handle = handle;

	if (read) {
		fio->tif = TIFFFdOpen((thandle_t)fio, "", "r");
	} else {
		// mode = "w"	: write Classic TIFF
		// mode = "w8"	: write Big TIFF
		fio->tif = TIFFFdOpen((thandle_t)fio, "", "w");
	}
	if(fio->tif == NULL) {
		free(fio);
		Bitmap_OutputMessageProc(FIF_TIFF, "Error while opening TIFF: data is invalid");
		return NULL;
	}
	return fio;
}

static void
Close(BitmapIO *io, fi_handle handle, void *data) {
	if(data) {
		fi_TIFFIO *fio = (fi_TIFFIO*)data;
		TIFFClose(fio->tif);
		free(fio);
	}
}

// ----------------------------------------------------------

static int
PageCount(BitmapIO *io, fi_handle handle, void *data) {
	if(data) {
		fi_TIFFIO *fio = (fi_TIFFIO*)data;
		TIFF *tif = (TIFF *)fio->tif;
		int nr_ifd = 0;

		do {
			nr_ifd++;
		} while (TIFFReadDirectory(tif));
				
		return nr_ifd;
	}

	return 0;
}

// ----------------------------------------------------------

/**
check for uncommon bitspersample values (e.g. 10, 12, ...)
@param photometric TIFFTAG_PHOTOMETRIC tiff tag
@param bitspersample TIFFTAG_BITSPERSAMPLE tiff tag
@return Returns FALSE if a uncommon bit-depth is encountered, returns TRUE otherwise
*/
static BOOL 
IsValidBitsPerSample(uint16 photometric, uint16 bitspersample) {

	switch(bitspersample) {
		case 1:
		case 4:
			if((photometric == PHOTOMETRIC_MINISWHITE) || (photometric == PHOTOMETRIC_MINISBLACK) || (photometric == PHOTOMETRIC_PALETTE)) { 
				return TRUE;
			} else {
				return FALSE;
			}
			break;
		case 8:
			return TRUE;
		case 16:
			if(photometric != PHOTOMETRIC_PALETTE) { 
				return FALSE;
			} else {
				return FALSE;
			}
			break;
		case 32:
			if((photometric == PHOTOMETRIC_MINISWHITE) || (photometric == PHOTOMETRIC_MINISBLACK) || (photometric == PHOTOMETRIC_LOGLUV)) { 
				return FALSE;
			} else {
				return FALSE;
			}
			break;
		case 64:
		case 128:
			if(photometric == PHOTOMETRIC_MINISBLACK) { 
				return FALSE;
			} else {
				return FALSE;
			}
			break;
		default:
			return FALSE;
	}
}

static TIFFLoadMethod  
FindLoadMethod(TIFF *tif, BITMAP_TYPE image_type, int flags) {
	uint16 bitspersample	= (uint16)-1;
	uint16 samplesperpixel	= (uint16)-1;
	uint16 photometric		= (uint16)-1;
	uint16 planar_config	= (uint16)-1;

	TIFFLoadMethod loadMethod = LoadAsGenericStrip;

	TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric);
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
	TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitspersample);
	TIFFGetFieldDefaulted(tif, TIFFTAG_PLANARCONFIG, &planar_config);

	BOOL bIsTiled = (TIFFIsTiled(tif) == 0) ? FALSE:TRUE;

	switch(photometric) {
		// convert to 24 or 32 bits RGB if the image is full color
		case PHOTOMETRIC_RGB:
			break;
		case PHOTOMETRIC_YCBCR:
		case PHOTOMETRIC_CIELAB:
		case PHOTOMETRIC_ICCLAB:
		case PHOTOMETRIC_ITULAB:
			loadMethod = LoadAsRBGA;
			break;
		case PHOTOMETRIC_LOGLUV:
			loadMethod = LoadAsLogLuv;
			break;
		case PHOTOMETRIC_SEPARATED:
			// if image is PHOTOMETRIC_SEPARATED _and_ comes with an ICC profile, 
			// then the image should preserve its original (CMYK) colour model and 
			// should be read as CMYK (to keep the match of pixel and profile and 
			// to avoid multiple conversions. Conversion can be done by changing 
			// the profile from it's original CMYK to an RGB profile with an 
			// apropriate color management system. Works with non-tiled TIFFs.
			if(!bIsTiled) {
				loadMethod = LoadAsCMYK;
			}
			break;
		case PHOTOMETRIC_MINISWHITE:
		case PHOTOMETRIC_MINISBLACK:
		case PHOTOMETRIC_PALETTE:
			// When samplesperpixel = 2 and bitspersample = 8, set the image as a
			// 8-bit indexed image + 8-bit alpha layer image
			// and convert to a 8-bit image with a transparency table
			if((samplesperpixel > 1) && (bitspersample == 8)) {
				loadMethod = LoadAs8BitTrns;
			} else {
				loadMethod = LoadAsGenericStrip;
			}
			break;
		default:
			loadMethod = LoadAsGenericStrip;
			break;
	}

	if((loadMethod == LoadAsGenericStrip) && bIsTiled) {
		loadMethod = LoadAsTiled;
	}

	return loadMethod;
}


// --------------------------------------------------------------------------

FIBITMAP *
LoadTIFF(BitmapIO *io, fi_handle handle, int page, int flags) {
	if (!handle) {
		return NULL;
	}
	
	TIFF   *tif = NULL;
	uint32 height = 0; 
	uint32 width = 0; 
	uint16 bitspersample = 1;
	uint16 samplesperpixel = 1;
	uint32 rowsperstrip = (uint32)-1;  
	uint16 photometric = PHOTOMETRIC_MINISWHITE;
	uint16 compression = (uint16)-1;
	uint16 planar_config;

	FIBITMAP *dib = NULL;
	uint32 iccSize = 0;		// ICC profile length
	void *iccBuf = NULL;	// ICC profile data		

	const BOOL header_only = (flags & FIF_LOAD_NOPIXELS) == FIF_LOAD_NOPIXELS;
	void *data = NULL;

	try {	
		data = Open(io, handle, TRUE);
		fi_TIFFIO *fio = (fi_TIFFIO*)data;
		tif = fio->tif;

		if (page != -1) {
			if (!tif || !TIFFSetDirectory(tif, (uint16)page)) {
				throw "Error encountered while opening TIFF file";			
			}
		}
		
		const BOOL asCMYK = (flags & TIFF_CMYK) == TIFF_CMYK;

		// first, get the photometric, the compression and basic metadata
		// ---------------------------------------------------------------------------------

		TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric);
		TIFFGetField(tif, TIFFTAG_COMPRESSION, &compression);

		// check for HDR formats
		// ---------------------------------------------------------------------------------

		if(photometric == PHOTOMETRIC_LOGLUV) {
			// check the compression
			if(compression != COMPRESSION_SGILOG && compression != COMPRESSION_SGILOG24) {
				throw "Only support SGILOG compressed LogLuv data";
			}
			// set decoder to output in IEEE 32-bit float XYZ values
			TIFFSetField(tif, TIFFTAG_SGILOGDATAFMT, SGILOGDATAFMT_FLOAT);
		}

		// ---------------------------------------------------------------------------------

		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
		TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
		TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitspersample);
		TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);   			
		TIFFGetField(tif, TIFFTAG_ICCPROFILE, &iccSize, &iccBuf);
		TIFFGetFieldDefaulted(tif, TIFFTAG_PLANARCONFIG, &planar_config);

		// check for unsupported formats
		// ---------------------------------------------------------------------------------

		if(IsValidBitsPerSample(photometric, bitspersample) == FALSE) {
			Bitmap_OutputMessageProc(FIF_TIFF,
				"Unable to handle this format: bitspersample = %d, samplesperpixel = %d, photometric = %d", 
				(int)bitspersample, (int)samplesperpixel, (int)photometric);
			throw (char*)NULL;
		}

		// ---------------------------------------------------------------------------------

		// get image data type

		BITMAP_TYPE image_type = ReadImageType(tif, bitspersample, samplesperpixel);

		// get the most appropriate loading method

		TIFFLoadMethod loadMethod = FindLoadMethod(tif, image_type, flags);

		// ---------------------------------------------------------------------------------

		if(loadMethod == LoadAsRBGA) {
			// ---------------------------------------------------------------------------------
			// RGB[A] loading using the TIFFReadRGBAImage() API
			// ---------------------------------------------------------------------------------

			BOOL has_alpha = FALSE;   

			// Read the whole image into one big RGBA buffer and then 
			// convert it to a DIB. This is using the traditional
			// TIFFReadRGBAImage() API that we trust.
			
			uint32 *raster = NULL;

			if(!header_only) {

				raster = (uint32*)_TIFFmalloc(width * height * sizeof(uint32));
				if (raster == NULL) {
					throw FI_MSG_ERROR_MEMORY;
				}

				// read the image in one chunk into an RGBA array

				if (!TIFFReadRGBAImage(tif, width, height, raster, 1)) {
					_TIFFfree(raster);
					throw FI_MSG_ERROR_UNSUPPORTED_FORMAT;
				}
			}
			// TIFFReadRGBAImage always deliveres 3 or 4 samples per pixel images
			// (RGB or RGBA, see below). Cut-off possibly present channels (additional 
			// alpha channels) from e.g. Photoshop. Any CMYK(A..) is now treated as RGB,
			// any additional alpha channel on RGB(AA..) is lost on conversion to RGB(A)

			if(samplesperpixel > 4) { // TODO Write to Extra Channels
				Bitmap_OutputMessageProc(FIF_TIFF, "Warning: %d additional alpha channel(s) ignored", samplesperpixel - 4);
				samplesperpixel = 4;
			}

			// create a new DIB (take care of different samples-per-pixel in case 
			// of converted CMYK image (RGB conversion is on sample per pixel less)

			if (photometric == PHOTOMETRIC_SEPARATED && samplesperpixel == 4) {
				samplesperpixel = 3;
			}

			dib = CreateImageType(header_only, image_type, width, height, bitspersample, samplesperpixel);
			if (dib == NULL) {
				// free the raster pointer and output an error if allocation failed
				if(raster) {
					_TIFFfree(raster);
				}
				throw FI_MSG_ERROR_DIB_MEMORY;
			}
			
			// fill in the resolution (english or universal)

			ReadResolution(tif, dib);

			if(!header_only) {

				// read the raster lines and save them in the DIB
				// with RGB mode, we have to change the order of the 3 samples RGB
				// We use macros for extracting components from the packed ABGR 
				// form returned by TIFFReadRGBAImage.

				uint32 *row = &raster[0];

				if (samplesperpixel == 4) {
					// 32-bit RGBA
					for (uint32 y = 0; y < height; y++) {
						BYTE *bits = Bitmap_GetScanLine(dib, y);
						for (uint32 x = 0; x < width; x++) {
							bits[FI_RGBA_BLUE]	= (BYTE)TIFFGetB(row[x]);
							bits[FI_RGBA_GREEN] = (BYTE)TIFFGetG(row[x]);
							bits[FI_RGBA_RED]	= (BYTE)TIFFGetR(row[x]);
							bits[FI_RGBA_ALPHA] = (BYTE)TIFFGetA(row[x]);

							if (bits[FI_RGBA_ALPHA] != 0) {
								has_alpha = TRUE;
							}

							bits += 4;
						}
						row += width;
					}
				} else {
					// 24-bit RGB
					for (uint32 y = 0; y < height; y++) {
						BYTE *bits = Bitmap_GetScanLine(dib, y);
						for (uint32 x = 0; x < width; x++) {
							bits[FI_RGBA_BLUE]	= (BYTE)TIFFGetB(row[x]);
							bits[FI_RGBA_GREEN] = (BYTE)TIFFGetG(row[x]);
							bits[FI_RGBA_RED]	= (BYTE)TIFFGetR(row[x]);

							bits += 3;
						}
						row += width;
					}
				}

				_TIFFfree(raster);
			}
			
			// ### Not correct when header only
			Bitmap_SetTransparent(dib, has_alpha);

		} else if(loadMethod == LoadAs8BitTrns) {
			// ---------------------------------------------------------------------------------
			// 8-bit + 8-bit alpha layer loading
			// ---------------------------------------------------------------------------------

			// create a new 8-bit DIB
			dib = CreateImageType(header_only, image_type, width, height, bitspersample, MIN(2, samplesperpixel));
			if (dib == NULL) {
				throw FI_MSG_ERROR_MEMORY;
			}

			// fill in the resolution (english or universal)

			ReadResolution(tif, dib);

			// set up the colormap based on photometric	

			ReadPalette(tif, photometric, bitspersample, dib);

			// calculate the line + pitch (separate for scr & dest)

			const tmsize_t src_line = TIFFScanlineSize(tif);
			// here, the pitch is 2x less than the original as we only keep the first layer				
			int dst_pitch = Bitmap_GetPitch(dib);

			// transparency table for 8-bit + 8-bit alpha images

			BYTE trns[256]; 
			// clear the transparency table
			memset(trns, 0xFF, 256 * sizeof(BYTE));

			// In the tiff file the lines are saved from up to down 
			// In a DIB the lines must be saved from down to up

			BYTE *bits = Bitmap_GetScanLine(dib, height - 1);

			// read the tiff lines and save them in the DIB

			if(planar_config == PLANARCONFIG_CONTIG && !header_only) {

				BYTE *buf = (BYTE*)malloc(TIFFStripSize(tif) * sizeof(BYTE));
				if(buf == NULL) {
					throw FI_MSG_ERROR_MEMORY;
				}

				for (uint32 y = 0; y < height; y += rowsperstrip) {
					int32 nrow = (y + rowsperstrip > height ? height - y : rowsperstrip);

					if (TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, y, 0), buf, nrow * src_line) == -1) {
						free(buf);
						throw FI_MSG_ERROR_PARSING;
					}
					for (int l = 0; l < nrow; l++) {
						BYTE *p = bits;
						BYTE *b = buf + l * src_line;

						for(uint32 x = 0; x < (uint32)(src_line / samplesperpixel); x++) {
							// copy the 8-bit layer
							*p = b[0];
							// convert the 8-bit alpha layer to a trns table
							trns[ b[0] ] = b[1];

							p++;
							b += samplesperpixel;
						}
						bits -= dst_pitch;
					}
				}

				free(buf);
			}
			else if(planar_config == PLANARCONFIG_SEPARATE && !header_only) {
				tmsize_t stripsize = TIFFStripSize(tif) * sizeof(BYTE);
				BYTE *buf = (BYTE*)malloc(2 * stripsize);
				if(buf == NULL) {
					throw FI_MSG_ERROR_MEMORY;
				}
				BYTE *grey = buf;
				BYTE *alpha = buf + stripsize;

				for (uint32 y = 0; y < height; y += rowsperstrip) {
					int32 nrow = (y + rowsperstrip > height ? height - y : rowsperstrip);

					if (TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, y, 0), grey, nrow * src_line) == -1) {
						free(buf);
						throw FI_MSG_ERROR_PARSING;
					} 
					if (TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, y, 1), alpha, nrow * src_line) == -1) {
						free(buf);
						throw FI_MSG_ERROR_PARSING;
					} 

					for (int l = 0; l < nrow; l++) {
						BYTE *p = bits;
						BYTE *g = grey + l * src_line;
						BYTE *a = alpha + l * src_line;

						for(uint32 x = 0; x < (uint32)(src_line); x++) {
							// copy the 8-bit layer
							*p = g[0];
							// convert the 8-bit alpha layer to a trns table
							trns[ g[0] ] = a[0];

							p++;
							g++;
							a++;
						}
						bits -= dst_pitch;
					}
				}

				free(buf);

			}
			
			Bitmap_SetTransparencyTable(dib, &trns[0], 256);
			Bitmap_SetTransparent(dib, TRUE);

		} else if(loadMethod == LoadAsCMYK) {
			// ---------------------------------------------------------------------------------
			// CMYK loading
			// ---------------------------------------------------------------------------------

			// At this place, samplesperpixel could be > 4, esp. when a CMYK(A) format
			// is recognized. Where all other formats are handled straight-forward, this
			// format has to be handled special 

			BOOL isCMYKA = (photometric == PHOTOMETRIC_SEPARATED) && (samplesperpixel > 4);

			// We use a temp dib to store the alpha for the CMYKA to RGBA conversion
			// NOTE this is until we have Extra channels implementation.
			// Also then it will be possible to merge LoadAsCMYK with LoadAsGenericStrip
			
			FIBITMAP *alpha = NULL;
			unsigned alpha_pitch = 0;
			BYTE *alpha_bits = NULL;
			unsigned alpha_Bpp = 0;

			if(isCMYKA && !asCMYK && !header_only) {
				if(bitspersample == 16) {
					alpha = NULL;//Bitmap_AllocateT(FIT_UINT16, width, height);
				} else if (bitspersample == 8) {
					alpha = Bitmap_Allocate(width, height, 8);
				}
					
				if(!alpha) {
					Bitmap_OutputMessageProc(FIF_TIFF, "Failed to allocate temporary alpha channel");
				} else {
					alpha_bits = Bitmap_GetScanLine(alpha, height - 1);
					alpha_pitch = Bitmap_GetPitch(alpha);
					alpha_Bpp = Bitmap_GetBPP(alpha) / 8;
				}
				
			}
			
			// create a new DIB
			const uint16 chCount = MIN(samplesperpixel, 4);
			dib = CreateImageType(header_only, image_type, width, height, bitspersample, chCount);
			if (dib == NULL) {
				Bitmap_Unload(alpha);
				throw FI_MSG_ERROR_MEMORY;
			}

			// fill in the resolution (english or universal)

			ReadResolution(tif, dib);

			if(!header_only) {

				// calculate the line + pitch (separate for scr & dest)

				const tmsize_t src_line = TIFFScanlineSize(tif);
				const tmsize_t dst_line = Bitmap_GetLine(dib);
				const unsigned dib_pitch = Bitmap_GetPitch(dib);
				const unsigned dibBpp = Bitmap_GetBPP(dib) / 8;
				const unsigned Bpc = dibBpp / chCount;
				const unsigned srcBpp = bitspersample * samplesperpixel / 8;

				assert(Bpc <= 2); //< CMYK is only BYTE or SHORT 
				
				// In the tiff file the lines are save from up to down 
				// In a DIB the lines must be saved from down to up

				BYTE *bits = Bitmap_GetScanLine(dib, height - 1);

				// read the tiff lines and save them in the DIB

				BYTE *buf = (BYTE*)malloc(TIFFStripSize(tif) * sizeof(BYTE));
				if(buf == NULL) {
					Bitmap_Unload(alpha);
					throw FI_MSG_ERROR_MEMORY;
				}

				if(planar_config == PLANARCONFIG_CONTIG) {
					
					// - loop for strip blocks -
					
					for (uint32 y = 0; y < height; y += rowsperstrip) {
						const int32 strips = (y + rowsperstrip > height ? height - y : rowsperstrip);

						if (TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, y, 0), buf, strips * src_line) == -1) {
							free(buf);
							Bitmap_Unload(alpha);
							throw FI_MSG_ERROR_PARSING;
						} 
						
						// - loop for strips -
						
						if(src_line != dst_line) {
							// CMYKA+
							if(alpha) {
								for (int l = 0; l < strips; l++) {					
									for(BYTE *pixel = bits, *al_pixel = alpha_bits, *src_pixel =  buf + l * src_line; pixel < bits + dib_pitch; pixel += dibBpp, al_pixel += alpha_Bpp, src_pixel += srcBpp) {
										// copy pixel byte by byte
										BYTE b = 0;
										for( ; b < dibBpp; ++b) {
											pixel[b] =  src_pixel[b];
										}
										// TODO write the remaining bytes to extra channel(s)
										
										// HACK write the first alpha to a separate dib (assume BYTE or WORD)
										al_pixel[0] = src_pixel[b];
										if(Bpc > 1) {
											al_pixel[1] = src_pixel[b + 1];
										}
										
									}
									bits -= dib_pitch;
									alpha_bits -= alpha_pitch;
								}
							}
							else {
								// alpha/extra channels alloc failed
								for (int l = 0; l < strips; l++) {
									for(BYTE* pixel = bits, * src_pixel =  buf + l * src_line; pixel < bits + dst_line; pixel += dibBpp, src_pixel += srcBpp) {
										AssignPixel(pixel, src_pixel, dibBpp);
									}
									bits -= dib_pitch;
								}
							}
						}
						else { 
							// CMYK to CMYK
							for (int l = 0; l < strips; l++) {
								BYTE *b = buf + l * src_line;
								memcpy(bits, b, src_line);
								bits -= dib_pitch;
							}
						}

					} // height
				
				}
				else if(planar_config == PLANARCONFIG_SEPARATE) {

					BYTE *dib_strip = bits;
					BYTE *al_strip = alpha_bits;

					// - loop for strip blocks -
					
					for (uint32 y = 0; y < height; y += rowsperstrip) {
						const int32 strips = (y + rowsperstrip > height ? height - y : rowsperstrip);
						
						// - loop for channels (planes) -
						
						for(uint16 sample = 0; sample < samplesperpixel; sample++) {
							
							if (TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, y, sample), buf, strips * src_line) == -1) {
								free(buf);
								Bitmap_Unload(alpha);
								throw FI_MSG_ERROR_PARSING;
							} 
									
							BYTE *dst_strip = dib_strip;
							unsigned dst_pitch = dib_pitch;
							uint16 ch = sample;
							unsigned Bpp = dibBpp;

							if(sample >= chCount) {
								// TODO Write to Extra Channel
								
								// HACK redirect write to temp alpha
								if(alpha && sample == chCount) {

									dst_strip = al_strip;
									dst_pitch = alpha_pitch;

									ch = 0;
									Bpp = alpha_Bpp;
								}
								else {
									break; 
								}
							}
							
							const unsigned channelOffset = ch * Bpc;			
							
							// - loop for strips in block -
							
							BYTE *src_line_begin = buf;
							BYTE *dst_line_begin = dst_strip;
							for (int l = 0; l < strips; l++, src_line_begin += src_line, dst_line_begin -= dst_pitch ) {
								// - loop for pixels in strip -
								
								const BYTE* const src_line_end = src_line_begin + src_line;
								for (BYTE *src_bits = src_line_begin, * dst_bits = dst_line_begin; src_bits < src_line_end; src_bits += Bpc, dst_bits += Bpp) {
									AssignPixel(dst_bits + channelOffset, src_bits, Bpc);									
								} // line
								
							} // strips
															
						} // channels
							
						// done with a strip block, incr to the next
						dib_strip -= strips * dib_pitch;
						al_strip -= strips * alpha_pitch;
							
					} //< height
					
				}

				free(buf);
			
				if(!asCMYK) {
					ConvertCMYKtoRGBA(dib);
					
					// The ICC Profile is invalid, clear it
					iccSize = 0;
					iccBuf = NULL;
					
					if(isCMYKA) {
						// HACK until we have Extra channels. (ConvertCMYKtoRGBA will then do the work)
						
//						Bitmap_SetChannel(dib, alpha, FICC_ALPHA);
						Bitmap_Unload(alpha);
						alpha = NULL;
					}
					else {
						FIBITMAP *t = RemoveAlphaChannel(dib);
						if(t) {
							Bitmap_Unload(dib);
							dib = t;
						}
						else {
							Bitmap_OutputMessageProc(FIF_TIFF, "Cannot allocate memory for buffer. CMYK image converted to RGB + pending Alpha");
						}
					}
				}
				
			} // !header_only
			
		} else if(loadMethod == LoadAsGenericStrip) {
			// ---------------------------------------------------------------------------------
			// Generic loading
			// ---------------------------------------------------------------------------------

			// create a new DIB
			const uint16 chCount = MIN(samplesperpixel, 4);
			dib = CreateImageType(header_only, image_type, width, height, bitspersample, chCount);
			if (dib == NULL) {
				throw FI_MSG_ERROR_MEMORY;
			}

			// fill in the resolution (english or universal)

			ReadResolution(tif, dib);

			// set up the colormap based on photometric	

			ReadPalette(tif, photometric, bitspersample, dib);
	
			if(!header_only) {
				// calculate the line + pitch (separate for scr & dest)

				const tmsize_t src_line = TIFFScanlineSize(tif);
				const tmsize_t dst_line = Bitmap_GetLine(dib);
				const unsigned dst_pitch = Bitmap_GetPitch(dib);
				const unsigned Bpp = Bitmap_GetBPP(dib) / 8;
				const unsigned srcBpp = bitspersample * samplesperpixel / 8;

				// In the tiff file the lines are save from up to down 
				// In a DIB the lines must be saved from down to up

				BYTE *bits = Bitmap_GetScanLine(dib, height - 1);

				// read the tiff lines and save them in the DIB

				BYTE *buf = (BYTE*)malloc(TIFFStripSize(tif) * sizeof(BYTE));
				if(buf == NULL) {
					throw FI_MSG_ERROR_MEMORY;
				}
				memset(buf, 0, TIFFStripSize(tif) * sizeof(BYTE));
				
				BOOL bThrowMessage = FALSE;
				
				if(planar_config == PLANARCONFIG_CONTIG) {

					for (uint32 y = 0; y < height; y += rowsperstrip) {
						int32 strips = (y + rowsperstrip > height ? height - y : rowsperstrip);

						if (TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, y, 0), buf, strips * src_line) == -1) {
							// ignore errors as they can be frequent and not really valid errors, especially with fax images
							bThrowMessage = TRUE;							
							/*
							free(buf);
							throw FI_MSG_ERROR_PARSING;
							*/
						} 
						if(src_line == dst_line) {
							// channel count match
							for (int l = 0; l < strips; l++) {							
								memcpy(bits, buf + l * src_line, src_line);
								bits -= dst_pitch;
							}
						}
						else {
							for (int l = 0; l < strips; l++) {
								for(BYTE *pixel = bits, *src_pixel =  buf + l * src_line; pixel < bits + dst_pitch; pixel += Bpp, src_pixel += srcBpp) {
									AssignPixel(pixel, src_pixel, Bpp);
								}
								bits -= dst_pitch;
							}
						}
					}
				}
				else if(planar_config == PLANARCONFIG_SEPARATE) {
					
					const unsigned Bpc = bitspersample / 8;
					BYTE* dib_strip = bits;
					// - loop for strip blocks -
					
					for (uint32 y = 0; y < height; y += rowsperstrip) {
						const int32 strips = (y + rowsperstrip > height ? height - y : rowsperstrip);
						
						// - loop for channels (planes) -
						
						for(uint16 sample = 0; sample < samplesperpixel; sample++) {
							
							if (TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, y, sample), buf, strips * src_line) == -1) {
								// ignore errors as they can be frequent and not really valid errors, especially with fax images
								bThrowMessage = TRUE;	
							} 
									
							if(sample >= chCount) {
								// TODO Write to Extra Channel
								break; 
							}
							
							const unsigned channelOffset = sample * Bpc;			
							
							// - loop for strips in block -
							
							BYTE* src_line_begin = buf;
							BYTE* dst_line_begin = dib_strip;
							for (int l = 0; l < strips; l++, src_line_begin += src_line, dst_line_begin -= dst_pitch ) {
									
								// - loop for pixels in strip -
								
								const BYTE* const src_line_end = src_line_begin + src_line;

								for (BYTE* src_bits = src_line_begin, * dst_bits = dst_line_begin; src_bits < src_line_end; src_bits += Bpc, dst_bits += Bpp) {
									// actually assigns channel
									AssignPixel(dst_bits + channelOffset, src_bits, Bpc); 
								} // line

							} // strips

						} // channels
							
						// done with a strip block, incr to the next
						dib_strip -= strips * dst_pitch;
							
					} // height

				}
				free(buf);
				
				if(bThrowMessage) {
					Bitmap_OutputMessageProc(FIF_TIFF, "Warning: parsing error. Image may be incomplete or contain invalid data !");
				}
				
#if BITMAP_COLORORDER == BITMAP_COLORORDER_BGR
				SwapRedBlue32(dib);
#endif

			} // !header only
			
		} else if(loadMethod == LoadAsTiled) {
			// ---------------------------------------------------------------------------------
			// Tiled image loading
			// ---------------------------------------------------------------------------------

			uint32 tileWidth, tileHeight;
			uint32 src_line = 0;

			// create a new DIB
			dib = CreateImageType( header_only, image_type, width, height, bitspersample, samplesperpixel);
			if (dib == NULL) {
				throw FI_MSG_ERROR_MEMORY;
			}

			// fill in the resolution (english or universal)

			ReadResolution(tif, dib);

			// set up the colormap based on photometric	

			ReadPalette(tif, photometric, bitspersample, dib);

			// get the tile geometry
			if(!TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tileWidth) || !TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileHeight)) {
				throw "Invalid tiled TIFF image";
			}

			// read the tiff lines and save them in the DIB

			if(planar_config == PLANARCONFIG_CONTIG && !header_only) {
				
				// get the maximum number of bytes required to contain a tile
				tmsize_t tileSize = TIFFTileSize(tif);

				// allocate tile buffer
				BYTE *tileBuffer = (BYTE*)malloc(tileSize * sizeof(BYTE));
				if(tileBuffer == NULL) {
					throw FI_MSG_ERROR_MEMORY;
				}

				// calculate src line and dst pitch
				int dst_pitch = Bitmap_GetPitch(dib);
				uint32 tileRowSize = (uint32)TIFFTileRowSize(tif);
				uint32 imageRowSize = (uint32)TIFFScanlineSize(tif);


				// In the tiff file the lines are saved from up to down 
				// In a DIB the lines must be saved from down to up

				BYTE *bits = Bitmap_GetScanLine(dib, height - 1);
				
				for (uint32 y = 0; y < height; y += tileHeight) {						
					int32 nrows = (y + tileHeight > height ? height - y : tileHeight);					

					for (uint32 x = 0, rowSize = 0; x < width; x += tileWidth, rowSize += tileRowSize) {
						memset(tileBuffer, 0, tileSize);

						// read one tile
						if (TIFFReadTile(tif, tileBuffer, x, y, 0, 0) < 0) {
							free(tileBuffer);
							throw "Corrupted tiled TIFF file";
						}
						// convert to strip
						if(x + tileWidth > width) {
							src_line = imageRowSize - rowSize;
						} else {
							src_line = tileRowSize;
						}
						BYTE *src_bits = tileBuffer;
						BYTE *dst_bits = bits + rowSize;
						for(int k = 0; k < nrows; k++) {
							memcpy(dst_bits, src_bits, src_line);
							src_bits += tileRowSize;
							dst_bits -= dst_pitch;
						}
					}

					bits -= nrows * dst_pitch;
				}

#if BITMAP_COLORORDER == BITMAP_COLORORDER_BGR
				SwapRedBlue32(dib);
#endif
				free(tileBuffer);
			}
			else if(planar_config == PLANARCONFIG_SEPARATE) {
				throw "Separated tiled TIFF images are not supported"; 
			}


		} else if(loadMethod == LoadAsLogLuv) {
			// ---------------------------------------------------------------------------------
			// RGBF LogLuv compressed loading
			// ---------------------------------------------------------------------------------

			double	stonits;	// input conversion to nits
			if (!TIFFGetField(tif, TIFFTAG_STONITS, &stonits)) {
				stonits = 1;
			}
			
			// create a new DIB
			dib = CreateImageType(header_only, image_type, width, height, bitspersample, samplesperpixel);
			if (dib == NULL) {
				throw FI_MSG_ERROR_MEMORY;
			}

			// fill in the resolution (english or universal)

			ReadResolution(tif, dib);

			if(planar_config == PLANARCONFIG_CONTIG && !header_only) {
				// calculate the line + pitch (separate for scr & dest)

				tmsize_t src_line = TIFFScanlineSize(tif);
				int dst_pitch = Bitmap_GetPitch(dib);

				// In the tiff file the lines are save from up to down 
				// In a DIB the lines must be saved from down to up

				BYTE *bits = Bitmap_GetScanLine(dib, height - 1);

				// read the tiff lines and save them in the DIB

				BYTE *buf = (BYTE*)malloc(TIFFStripSize(tif) * sizeof(BYTE));
				if(buf == NULL) {
					throw FI_MSG_ERROR_MEMORY;
				}

				for (uint32 y = 0; y < height; y += rowsperstrip) {
					int32 nrow = (y + rowsperstrip > height ? height - y : rowsperstrip);

					if (TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, y, 0), buf, nrow * src_line) == -1) {
						free(buf);
						throw FI_MSG_ERROR_PARSING;
					} 
					// convert from XYZ to RGB
					for (int l = 0; l < nrow; l++) {						
						tiff_ConvertLineXYZToRGB(bits, buf + l * src_line, stonits, width);
						bits -= dst_pitch;
					}
				}

				free(buf);
			}
			else if(planar_config == PLANARCONFIG_SEPARATE) {
				// this cannot happen according to the LogLuv specification
				throw "Unable to handle PLANARCONFIG_SEPARATE LogLuv images";
			}

		} else {
			// ---------------------------------------------------------------------------------
			// Unknown or unsupported format
			// ---------------------------------------------------------------------------------

			throw FI_MSG_ERROR_UNSUPPORTED_FORMAT;
		}

		// copy ICC profile data (must be done after Bitmap_Allocate)

		Bitmap_CreateICCProfile(dib, iccBuf, iccSize);
		if (photometric == PHOTOMETRIC_SEPARATED && asCMYK) {
			Bitmap_GetICCProfile(dib)->flags |= FIICC_COLOR_IS_CMYK;
		}			

		Close(io, handle, data);

		return (FIBITMAP *)dib;

	} catch (const char *message) {			
		if(dib)	{
			Bitmap_Unload(dib);
		}
		if(message) {
			Bitmap_OutputMessageProc(FIF_TIFF, message);
		}
		if (data) {
			Close(io, handle, data);
		}
		return NULL;
	}
  
}
