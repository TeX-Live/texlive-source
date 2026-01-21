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


#ifndef BITMAP_H
#define BITMAP_H

// Compiler options ---------------------------------------------------------

// Endianness:
// Some versions of gcc may have BYTE_ORDER or __BYTE_ORDER defined.
// If your big endian system isn't being detected, add an OS specific check
// or define any of BITMAP_BIGENDIAN and BITMAP_LITTLEENDIAN directly
// to specify the desired endianness.
#if (!defined(BITMAP_BIGENDIAN) && !defined(BITMAP_LITTLEENDIAN))
	#if (defined(BYTE_ORDER) && BYTE_ORDER==BIG_ENDIAN) || (defined(__BYTE_ORDER) && __BYTE_ORDER==__BIG_ENDIAN) || defined(__BIG_ENDIAN__)
		#define BITMAP_BIGENDIAN
	#endif // BYTE_ORDER
#endif // !BITMAP_[BIG|LITTLE]ENDIAN

// Color-Order:
// The specified order of color components red, green and blue affects 24-
// and 32-bit images of type FIT_BITMAP as well as the colors that are part
// of a color palette. All other images always use RGB order. By default,
// color order is coupled to endianness:
// little-endian -> BGR
// big-endian    -> RGB
// However, you can always define BITMAP_COLORORDER to any of the known
// orders BITMAP_COLORORDER_BGR (0) and BITMAP_COLORORDER_RGB (1) to
// specify your preferred color order.
#define BITMAP_COLORORDER_BGR    0
#define BITMAP_COLORORDER_RGB    1
#define BITMAP_COLORORDER    BITMAP_COLORORDER_RGB
#if (!defined(BITMAP_COLORORDER)) || ((BITMAP_COLORORDER != BITMAP_COLORORDER_BGR) && (BITMAP_COLORORDER != BITMAP_COLORORDER_RGB))
	#if defined(BITMAP_BIGENDIAN)
		#define BITMAP_COLORORDER BITMAP_COLORORDER_RGB
	#else
		#define BITMAP_COLORORDER BITMAP_COLORORDER_BGR
	#endif // BITMAP_BIGENDIAN
#endif // BITMAP_COLORORDER

// Ensure 4-byte enums if we're using Borland C++ compilers
#if defined(__BORLANDC__)
#pragma option push -b
#endif

// For C compatibility --------------------------------------------------------

#ifdef __cplusplus
#define FI_DEFAULT(x)	= x
#define FI_ENUM(x)      enum x
#define FI_STRUCT(x)	struct x
#else
#define FI_DEFAULT(x)
#define FI_ENUM(x)      typedef int x; enum x
#define FI_STRUCT(x)	typedef struct x x; struct x
#endif

// Bitmap types -------------------------------------------------------------

FI_STRUCT (FIBITMAP) { void *data; };

// Types used in the library (directly copied from Windows) -----------------

#if defined(__MINGW32__) && defined(_WINDOWS_H)
#define _WINDOWS_	// prevent a bug in MinGW32
#endif // __MINGW32__

#ifndef _WINDOWS_
#define _WINDOWS_

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef NULL
#define NULL 0
#endif

#ifndef SEEK_SET
#define SEEK_SET  0
#define SEEK_CUR  1
#define SEEK_END  2
#endif

#ifndef _MSC_VER
// define portable types for 32-bit / 64-bit OS
#include <inttypes.h>
typedef int32_t BOOL;
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef int32_t LONG;
typedef int64_t INT64;
typedef uint64_t UINT64;
#else
// MS is not C99 ISO compliant
typedef long BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;
typedef signed __int64 INT64;
typedef unsigned __int64 UINT64;
#endif // _MSC_VER

#if (defined(_WIN32) || defined(__WIN32__))
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif // WIN32

typedef struct tagRGBQUAD {
#if BITMAP_COLORORDER == BITMAP_COLORORDER_BGR
  BYTE rgbBlue;
  BYTE rgbGreen;
  BYTE rgbRed;
#else
  BYTE rgbRed;
  BYTE rgbGreen;
  BYTE rgbBlue;
#endif // BITMAP_COLORORDER
  BYTE rgbReserved;
} RGBQUAD;

typedef struct tagRGBTRIPLE {
#if BITMAP_COLORORDER == BITMAP_COLORORDER_BGR
  BYTE rgbtBlue;
  BYTE rgbtGreen;
  BYTE rgbtRed;
#else
  BYTE rgbtRed;
  BYTE rgbtGreen;
  BYTE rgbtBlue;
#endif // BITMAP_COLORORDER
} RGBTRIPLE;

/** 96-bit RGB Float
*/
typedef struct tagFIRGBF {
	float red;
	float green;
	float blue;
} FIRGBF;

/** 128-bit RGBA Float
*/
typedef struct tagFIRGBAF {
	float red;
	float green;
	float blue;
	float alpha;
} FIRGBAF;

#if (defined(_WIN32) || defined(__WIN32__))
#pragma pack(pop)
#else
#pragma pack()
#endif // WIN32

typedef struct tagBITMAPINFOHEADER{
  DWORD biSize;
  LONG  biWidth; 
  LONG  biHeight; 
  WORD  biPlanes; 
  WORD  biBitCount;
  DWORD biCompression; 
  DWORD biSizeImage; 
  LONG  biXPelsPerMeter; 
  LONG  biYPelsPerMeter; 
  DWORD biClrUsed; 
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER; 

typedef struct tagBITMAPINFO { 
  BITMAPINFOHEADER bmiHeader; 
  RGBQUAD          bmiColors[1];
} BITMAPINFO, *PBITMAPINFO;

#endif // _WINDOWS_

// Indexes for byte arrays, masks and shifts for treating pixels as words ---
// These coincide with the order of RGBQUAD and RGBTRIPLE -------------------

#ifndef BITMAP_BIGENDIAN
#if BITMAP_COLORORDER == BITMAP_COLORORDER_BGR
// Little Endian (x86 / MS Windows, Linux) : BGR(A) order
#define FI_RGBA_RED				2
#define FI_RGBA_GREEN			1
#define FI_RGBA_BLUE			0
#define FI_RGBA_ALPHA			3
#define FI_RGBA_RED_MASK		0x00FF0000
#define FI_RGBA_GREEN_MASK		0x0000FF00
#define FI_RGBA_BLUE_MASK		0x000000FF
#define FI_RGBA_ALPHA_MASK		0xFF000000
#define FI_RGBA_RED_SHIFT		16
#define FI_RGBA_GREEN_SHIFT		8
#define FI_RGBA_BLUE_SHIFT		0
#define FI_RGBA_ALPHA_SHIFT		24
#else
// Little Endian (x86 / MaxOSX) : RGB(A) order
#define FI_RGBA_RED				0
#define FI_RGBA_GREEN			1
#define FI_RGBA_BLUE			2
#define FI_RGBA_ALPHA			3
#define FI_RGBA_RED_MASK		0x000000FF
#define FI_RGBA_GREEN_MASK		0x0000FF00
#define FI_RGBA_BLUE_MASK		0x00FF0000
#define FI_RGBA_ALPHA_MASK		0xFF000000
#define FI_RGBA_RED_SHIFT		0
#define FI_RGBA_GREEN_SHIFT		8
#define FI_RGBA_BLUE_SHIFT		16
#define FI_RGBA_ALPHA_SHIFT		24
#endif // BITMAP_COLORORDER
#else
#if BITMAP_COLORORDER == BITMAP_COLORORDER_BGR
// Big Endian (PPC / none) : BGR(A) order
#define FI_RGBA_RED				2
#define FI_RGBA_GREEN			1
#define FI_RGBA_BLUE			0
#define FI_RGBA_ALPHA			3
#define FI_RGBA_RED_MASK		0x0000FF00
#define FI_RGBA_GREEN_MASK		0x00FF0000
#define FI_RGBA_BLUE_MASK		0xFF000000
#define FI_RGBA_ALPHA_MASK		0x000000FF
#define FI_RGBA_RED_SHIFT		8
#define FI_RGBA_GREEN_SHIFT		16
#define FI_RGBA_BLUE_SHIFT		24
#define FI_RGBA_ALPHA_SHIFT		0
#else
// Big Endian (PPC / Linux, MaxOSX) : RGB(A) order
#define FI_RGBA_RED				0
#define FI_RGBA_GREEN			1
#define FI_RGBA_BLUE			2
#define FI_RGBA_ALPHA			3
#define FI_RGBA_RED_MASK		0xFF000000
#define FI_RGBA_GREEN_MASK		0x00FF0000
#define FI_RGBA_BLUE_MASK		0x0000FF00
#define FI_RGBA_ALPHA_MASK		0x000000FF
#define FI_RGBA_RED_SHIFT		24
#define FI_RGBA_GREEN_SHIFT		16
#define FI_RGBA_BLUE_SHIFT		8
#define FI_RGBA_ALPHA_SHIFT		0
#endif // BITMAP_COLORORDER
#endif // BITMAP_BIGENDIAN

#define FI_RGBA_RGB_MASK		(FI_RGBA_RED_MASK|FI_RGBA_GREEN_MASK|FI_RGBA_BLUE_MASK)

// The 16bit macros only include masks and shifts, since each color element is not byte aligned

#define FI16_555_RED_MASK		0x7C00
#define FI16_555_GREEN_MASK		0x03E0
#define FI16_555_BLUE_MASK		0x001F
#define FI16_555_RED_SHIFT		10
#define FI16_555_GREEN_SHIFT	5
#define FI16_555_BLUE_SHIFT		0
#define FI16_565_RED_MASK		0xF800
#define FI16_565_GREEN_MASK		0x07E0
#define FI16_565_BLUE_MASK		0x001F
#define FI16_565_RED_SHIFT		11
#define FI16_565_GREEN_SHIFT	5
#define FI16_565_BLUE_SHIFT		0

// ICC profile support ------------------------------------------------------

#define FIICC_DEFAULT			0x00
#define FIICC_COLOR_IS_CMYK		0x01

FI_STRUCT (FIICCPROFILE) { 
	WORD    flags;	//! info flag
	DWORD	size;	//! profile's size measured in bytes
	void   *data;	//! points to a block of contiguous memory containing the profile
};

// Important enums ----------------------------------------------------------

/** I/O image format identifiers.
*/
FI_ENUM(BITMAP_FORMAT) {
	FIF_UNKNOWN = -1,
	FIF_BMP		= 0,
	FIF_JPEG	= 1,
	FIF_PCX		= 2,
	FIF_PNG		= 3,
	FIF_TIFF	= 4,
	FIF_PSD		= 5,
	FIF_LAST	= 6
};

/** Image type used.
*/
FI_ENUM(BITMAP_TYPE) {
	FIT_UNKNOWN = 0,	//! unknown type
	FIT_BITMAP  = 1,	//! standard image			: 1-, 4-, 8-, 16-, 24-, 32-bit
};

/** Image color type used.
*/
FI_ENUM(BITMAP_COLOR_TYPE) {
	FIC_MINISWHITE = 0,		//! min value is white
    FIC_MINISBLACK = 1,		//! min value is black
    FIC_RGB        = 2,		//! RGB color model
    FIC_PALETTE    = 3,		//! color map indexed
	FIC_RGBALPHA   = 4,		//! RGB color model with alpha channel
	FIC_CMYK       = 5		//! CMYK color model
};

/** Upsampling / downsampling filters. 
Constants used in Rescale.
*/
FI_ENUM(BITMAP_FILTER) {
	FILTER_BOX		  = 0,	//! Box, pulse, Fourier window, 1st order (constant) b-spline
	FILTER_BICUBIC	  = 1,	//! Mitchell & Netravali's two-param cubic filter
	FILTER_BILINEAR   = 2,	//! Bilinear filter
	FILTER_BSPLINE	  = 3,	//! 4th order (cubic) b-spline
	FILTER_CATMULLROM = 4,	//! Catmull-Rom spline, Overhauser spline
	FILTER_LANCZOS3	  = 5	//! Lanczos3 filter
};

// File IO routines ---------------------------------------------------------

#ifndef BITMAP_IO
#define BITMAP_IO

typedef void* fi_handle;
typedef unsigned (*FI_ReadProc) (void *buffer, unsigned size, unsigned count, fi_handle handle);
typedef unsigned (*FI_WriteProc) (void *buffer, unsigned size, unsigned count, fi_handle handle);
typedef int (*FI_SeekProc) (fi_handle handle, long offset, int origin);
typedef long (*FI_TellProc) (fi_handle handle);

#if (defined(_WIN32) || defined(__WIN32__))
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif // WIN32

FI_STRUCT(BitmapIO) {
	FI_ReadProc  read_proc;     //! pointer to the function used to read data
    FI_WriteProc write_proc;    //! pointer to the function used to write data
    FI_SeekProc  seek_proc;     //! pointer to the function used to seek
    FI_TellProc  tell_proc;     //! pointer to the function used to aquire the current position
};

#if (defined(_WIN32) || defined(__WIN32__))
#pragma pack(pop)
#else
#pragma pack()
#endif // WIN32

/**
Handle to a memory I/O stream
*/
FI_STRUCT (FIMEMORY) { void *data; };

#endif // BITMAP_IO

// Load flag constants -----------------------------------------------

#define FIF_LOAD_NOPIXELS 0x8000	//! loading: load the image header only (not supported by all plugins, default to full loading)

#define JPEG_DEFAULT        0		
#define JPEG_FAST           0x0001	//! load the file as fast as possible, sacrificing some quality
#define JPEG_ACCURATE       0x0002	//! load the file with the best quality, sacrificing some speed
#define JPEG_CMYK			0x0004	//! load separated CMYK "as is" (use | to combine with other load flags)
#define JPEG_EXIFROTATE		0x0008	//! load and rotate according to Exif 'Orientation' tag if available
#define JPEG_GREYSCALE		0x0010	//! load and convert to a 8-bit greyscale image
#define PNG_DEFAULT         0
#define PNG_IGNOREGAMMA		1		//! loading: avoid gamma correction
#define TIFF_DEFAULT        0
#define TIFF_CMYK			0x0001	//! reads/stores tags for separated CMYK (use | to combine with compression flags)
#define PSD_DEFAULT         0
#define PSD_CMYK			1		//! reads tags for separated CMYK (default is conversion to RGB)
#define PSD_LAB				2		//! reads tags for CIELab (default is conversion to RGB)

// Constants used in FreeImage_RescaleEx

#define FI_RESCALE_DEFAULT			0x00    //! default options; none of the following other options apply
#define FI_RESCALE_TRUE_COLOR		0x01	//! for non-transparent greyscale images, convert to 24-bit if src bitdepth <= 8 (default is a 8-bit greyscale image). 
#define FI_RESCALE_OMIT_METADATA	0x02	//! do not copy metadata to the rescaled image



#ifdef __cplusplus
extern "C" {
#endif

// Message output functions -------------------------------------------------

typedef void (*Bitmap_OutputMessageFunction)(BITMAP_FORMAT fif, const char *msg);
typedef void (*Bitmap_OutputMessageFunctionStdCall)(BITMAP_FORMAT fif, const char *msg); 

extern void Bitmap_SetOutputMessageStdCall(Bitmap_OutputMessageFunctionStdCall omf);
extern void Bitmap_SetOutputMessage(Bitmap_OutputMessageFunction omf);
extern void Bitmap_OutputMessageProc(int fif, const char *fmt, ...);

// Allocate / Clone / Unload routines ---------------------------------------

extern FIBITMAP* Bitmap_Allocate(int width, int height, int bpp, unsigned red_mask FI_DEFAULT(0), unsigned green_mask FI_DEFAULT(0), unsigned blue_mask FI_DEFAULT(0));
extern FIBITMAP* Bitmap_AllocateT(BITMAP_TYPE type, int width, int height, int bpp FI_DEFAULT(8), unsigned red_mask FI_DEFAULT(0), unsigned green_mask FI_DEFAULT(0), unsigned blue_mask FI_DEFAULT(0));
extern FIBITMAP* Bitmap_Clone(FIBITMAP *dib);
extern void Bitmap_Unload(FIBITMAP *dib);

// Header loading routines
extern BOOL Bitmap_HasPixels(FIBITMAP *dib);

// Validate/Load routines -----------------------------------------------------

extern BOOL Bitmap_Validate(BITMAP_FORMAT fif, const char *filename);
extern BOOL Bitmap_ValidateFromHandle(BITMAP_FORMAT fif, BitmapIO *io, fi_handle handle);
extern FIBITMAP* Bitmap_Load(BITMAP_FORMAT fif, const char *filename, int flags FI_DEFAULT(0));
extern FIBITMAP* Bitmap_LoadFromHandle(BITMAP_FORMAT fif, BitmapIO *io, fi_handle handle, int flags FI_DEFAULT(0));

// Filetype request routines ------------------------------------------------

extern BITMAP_FORMAT Bitmap_GetFileType(const char *filename, int size FI_DEFAULT(0));
extern BITMAP_FORMAT Bitmap_GetFileTypeFromHandle(BitmapIO *io, fi_handle handle, int size FI_DEFAULT(0));

// Image type request routine -----------------------------------------------

extern BITMAP_TYPE Bitmap_GetImageType(FIBITMAP *dib);

// Bitmap helper routines ------------------------------------------------

extern BOOL Bitmap_IsLittleEndian(void);

// Pixel access routines ----------------------------------------------------

extern BYTE* Bitmap_GetBits(FIBITMAP *dib);
extern BYTE* Bitmap_GetScanLine(FIBITMAP *dib, int scanline);

// DIB info routines --------------------------------------------------------

extern unsigned Bitmap_GetColorsUsed(FIBITMAP *dib);
extern unsigned Bitmap_GetBPP(FIBITMAP *dib);
extern unsigned Bitmap_GetWidth(FIBITMAP *dib);
extern unsigned Bitmap_GetHeight(FIBITMAP *dib);
extern unsigned Bitmap_GetLine(FIBITMAP *dib);
extern unsigned Bitmap_GetPitch(FIBITMAP *dib);
extern unsigned Bitmap_GetDIBSize(FIBITMAP *dib);
extern unsigned Bitmap_GetMemorySize(FIBITMAP *dib);
extern RGBQUAD* Bitmap_GetPalette(FIBITMAP *dib);

extern unsigned Bitmap_GetDotsPerMeterX(FIBITMAP *dib);
extern unsigned Bitmap_GetDotsPerMeterY(FIBITMAP *dib);
extern void Bitmap_SetDotsPerMeterX(FIBITMAP *dib, unsigned res);
extern void Bitmap_SetDotsPerMeterY(FIBITMAP *dib, unsigned res);

extern BITMAPINFOHEADER* Bitmap_GetInfoHeader(FIBITMAP *dib);
extern BITMAPINFO* Bitmap_GetInfo(FIBITMAP *dib);
extern BITMAP_COLOR_TYPE Bitmap_GetColorType(FIBITMAP *dib);

extern unsigned Bitmap_GetRedMask(FIBITMAP *dib);
extern unsigned Bitmap_GetGreenMask(FIBITMAP *dib);
extern unsigned Bitmap_GetBlueMask(FIBITMAP *dib);

extern unsigned Bitmap_GetTransparencyCount(FIBITMAP *dib);
extern BYTE * Bitmap_GetTransparencyTable(FIBITMAP *dib);
extern void Bitmap_SetTransparent(FIBITMAP *dib, BOOL enabled);
extern void Bitmap_SetTransparencyTable(FIBITMAP *dib, BYTE *table, int count);
extern BOOL Bitmap_IsTransparent(FIBITMAP *dib);

extern BOOL Bitmap_HasBackgroundColor(FIBITMAP *dib);
extern BOOL Bitmap_GetBackgroundColor(FIBITMAP *dib, RGBQUAD *bkcolor);
extern BOOL Bitmap_SetBackgroundColor(FIBITMAP *dib, RGBQUAD *bkcolor);

// ICC profile routines -----------------------------------------------------

extern FIICCPROFILE* Bitmap_GetICCProfile(FIBITMAP *dib);
extern FIICCPROFILE* Bitmap_CreateICCProfile(FIBITMAP *dib, void *data, long size);
extern void Bitmap_DestroyICCProfile(FIBITMAP *dib);

// Smart conversion routines ------------------------------------------------

extern FIBITMAP* Bitmap_ConvertTo8Bits(FIBITMAP *dib);
extern FIBITMAP* Bitmap_ConvertToGreyscale(FIBITMAP *dib);
extern FIBITMAP * Bitmap_ConvertTo24Bits(FIBITMAP *dib);
extern FIBITMAP * Bitmap_ConvertRGBATo24Bits(FIBITMAP *dib);
extern FIBITMAP * Bitmap_ConvertTo32Bits(FIBITMAP *dib);


// --------------------------------------------------------------------------
// Image manipulation toolkit
// --------------------------------------------------------------------------

// upsampling / downsampling
extern FIBITMAP* Bitmap_Rescale(FIBITMAP *dib, int dst_width, int dst_height, BITMAP_FILTER filter FI_DEFAULT(FILTER_CATMULLROM));
extern FIBITMAP* Bitmap_Resample(FIBITMAP *dib, int dst_width, int dst_height);

// color manipulation routines (point operations)
extern BOOL Bitmap_Invert(FIBITMAP *dib);

// copy routines
extern FIBITMAP * Bitmap_Copy(FIBITMAP *dib, int left, int top, int right, int bottom);

// restore the borland-specific enum size option
#if defined(__BORLANDC__)
#pragma option pop
#endif

#ifdef __cplusplus
}
#endif

#endif // BITMAP_H
