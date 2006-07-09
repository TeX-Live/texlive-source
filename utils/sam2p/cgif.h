/* cgif.h -- a merge of some GIF-decoding files from giflib by E.S.Raymond
 * by pts@fazekas.hu at Wed Feb 27 13:18:04 CET 2002

The GIFLIB distribution is Copyright (c) 1997  Eric S. Raymond

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.  
 */

/******************************************************************************
* In order to make life a little bit easier when using the GIF file format,   *
* this library was written, and which does all the dirty work...	      *
*									      *
*					Written by Gershon Elber,  Jun. 1989  *
*					Hacks by Eric S. Raymond,  Sep. 1992  *
*******************************************************************************
* History:								      *
* 14 Jun 89 - Version 1.0 by Gershon Elber.				      *
*  3 Sep 90 - Version 1.1 by Gershon Elber (Support for Gif89, Unique names). *
* 15 Sep 90 - Version 2.0 by Eric S. Raymond (Changes to suoport GIF slurp)   *
* 26 Jun 96 - Version 3.0 by Eric S. Raymond (Full GIF89 support)
******************************************************************************/

#ifndef CGIF_H
#define CGIF_H

#ifdef __GNUC__
#pragma interface
#endif

#define GIF_LIB_VERSION	" Version 3.0, "

#define	GIF_ERROR	0
#define GIF_OK		1

#ifndef TRUE
#define TRUE		1
#define FALSE		0
#endif

#ifndef NULL
#define NULL		0
#endif /* NULL */

#if _MSC_VER > 1000
# undef  __PROTOTYPES__
# define __PROTOTYPES__ 1
# pragma warning(disable: 4127) /* conditional expression is constant */
# pragma warning(disable: 4244) /* =' : conversion from 'int ' to 'unsigned char ', possible loss of data */
#endif

/* at Wed Dec 11 15:33:22 CET 2002 */
/* #define USE_CGIF_FDOPEN 1 */
#undef USE_CGIF_FDOPEN

#ifdef __cplusplus
class CGIF { public:
#define EXTERN static
#else
#define EXTERN extern
#endif

typedef	int		GifBooleanType;
typedef	unsigned char	GifPixelType;
typedef unsigned char *	GifRowType;
typedef unsigned char	GifByteType;

#define GIF_MESSAGE(Msg) fprintf(stderr, "\n%s: %s\n", PROGRAM_NAME, Msg)
#define GIF_EXIT(Msg)	{ GIF_MESSAGE(Msg); exit(-3); }

#ifdef SYSV
#define VoidPtr char *
#else
#define VoidPtr void *
#endif /* SYSV */

typedef struct GifColorType {
    GifByteType Red, Green, Blue;
} GifColorType;

typedef struct ColorMapObject
{
    int	ColorCount;
    int BitsPerPixel;
    GifColorType *Colors;		/* on malloc(3) heap */
}
ColorMapObject;

typedef struct GifImageDesc {
    int Left, Top, Width, Height,	/* Current image dimensions. */
	Interlace;			/* Sequential/Interlaced lines. */
    ColorMapObject *ColorMap;		/* The local color map */
} GifImageDesc;

struct SavedImage;
typedef struct GifFileType {
    int SWidth, SHeight,		/* Screen dimensions. */
	SColorResolution, 		/* How many colors can we generate? */
	SBackGroundColor;		/* I hope you understand this one... */
    ColorMapObject *SColorMap;		/* NULL if not exists. */
    int ImageCount;			/* Number of current image */
    /*GifImageDesc Image; */		/* Block describing current image */
    struct SavedImage *SavedImages;	/* Use this to accumulate file state */
    VoidPtr Private;	  		/* Don't mess with this! */
} GifFileType;

typedef enum {
    UNDEFINED_RECORD_TYPE,
    SCREEN_DESC_RECORD_TYPE,
    IMAGE_DESC_RECORD_TYPE,		/* Begin with ',' */
    EXTENSION_RECORD_TYPE,		/* Begin with '!' */
    TERMINATE_RECORD_TYPE		/* Begin with ';' */
} GifRecordType;

/******************************************************************************
*  GIF89 extension function codes                                             *
******************************************************************************/

#define COMMENT_EXT_FUNC_CODE		0xfe	/* comment */
#define GRAPHICS_EXT_FUNC_CODE		0xf9	/* graphics control */
#define PLAINTEXT_EXT_FUNC_CODE		0x01	/* plaintext */
#define APPLICATION_EXT_FUNC_CODE	0xff	/* application block */

/******************************************************************************
* O.K., here are the routines one can access in order to decode GIF file:     *
* (GIF_LIB file DGIF_LIB.C).						      *
******************************************************************************/

EXTERN GifFileType *DGifOpenFileName(const char *GifFileName);
#if USE_CGIF_FDOPEN
EXTERN GifFileType *DGifOpenFileHandle(int GifFileHandle);
#endif
EXTERN GifFileType *DGifOpenFILE(void/*FILE*/ *f);
EXTERN int DGifSlurp(GifFileType *GifFile);
EXTERN int DGifGetScreenDesc(GifFileType *GifFile);
EXTERN int DGifGetRecordType(GifFileType *GifFile, GifRecordType *GifType);
EXTERN int DGifGetImageDesc(GifFileType *GifFile);
EXTERN int DGifGetLine(GifFileType *GifFile, GifPixelType *GifLine, int GifLineLen);
EXTERN int DGifGetPixel(GifFileType *GifFile, GifPixelType GifPixel);
EXTERN int DGifGetComment(GifFileType *GifFile, char *GifComment);
EXTERN int DGifGetExtension(GifFileType *GifFile, int *GifExtCode,
						GifByteType **GifExtension);
EXTERN int DGifGetExtensionNext(GifFileType *GifFile, GifByteType **GifExtension);
EXTERN int DGifGetCode(GifFileType *GifFile, int *GifCodeSize,
						GifByteType **GifCodeBlock);
EXTERN int DGifGetCodeNext(GifFileType *GifFile, GifByteType **GifCodeBlock);
EXTERN int DGifGetLZCodes(GifFileType *GifFile, int *GifCode);
EXTERN int DGifCloseFile(GifFileType *GifFile);

#define	D_GIF_ERR_OPEN_FAILED	101		/* And DGif possible errors. */
#define	D_GIF_ERR_READ_FAILED	102
#define	D_GIF_ERR_NOT_GIF_FILE	103
#define D_GIF_ERR_NO_SCRN_DSCR	104
#define D_GIF_ERR_NO_IMAG_DSCR	105
#define D_GIF_ERR_NO_COLOR_MAP	106
#define D_GIF_ERR_WRONG_RECORD	107
#define D_GIF_ERR_DATA_TOO_BIG	108
#define D_GIF_ERR_NOT_ENOUGH_MEM 109
#define D_GIF_ERR_CLOSE_FAILED	110
#define D_GIF_ERR_NOT_READABLE	111
#define D_GIF_ERR_IMAGE_DEFECT	112
#define D_GIF_ERR_EOF_TOO_SOON	113


/******************************************************************************
* O.K., here are the routines from GIF_LIB file GIF_ERR.C.		      *
******************************************************************************/
EXTERN void PrintGifError(void);
EXTERN char *GetGifError(void);
EXTERN int GifLastError(void);

/*****************************************************************************
 *
 * Everything below this point is new after version 1.2, supporting `slurp
 * mode' for doing I/O in two big belts with all the image-bashing in core.
 *
 *****************************************************************************/

/******************************************************************************
* Color Map handling from ALLOCGIF.C					      *
******************************************************************************/

EXTERN ColorMapObject *MakeMapObject(int ColorCount, GifColorType *ColorMap);
EXTERN void FreeMapObject(ColorMapObject *Object);

/******************************************************************************
* Support for the in-core structures allocation (slurp mode).		      *
******************************************************************************/

/* This is the in-core version of an extension record */
typedef struct {
    /** Extension code:
     * 0xf9 Graphics Control extension (not stored!)
     * 0xfe Comment extension
     * 0xff Netscape Loop extension (not stored!)
     * .... ???
     */
    int		code;
    int		ByteCount;
    /**** pts ****/
    GifByteType	*Bytes;		/* on malloc(3) heap */
} ExtensionBlock;

/* This holds an image header, its unpacked raster bits, and extensions */
typedef struct SavedImage {
    GifImageDesc	ImageDesc;

    /**** pts ****/
    GifPixelType	*RasterBits;		/* on malloc(3) heap */
    int			ExtensionBlockCount;
    ExtensionBlock	*ExtensionBlocks;	/* on malloc(3) heap */

    /**** pts ****/
    /** from Graphics Control extension: 0..63 */
    unsigned char dispose; 
    /** from Graphics Control extension: 0..65535 */
    unsigned short delay;
    /** from Graphics Control extension: transparency index: -1 or 0..255 */
    signed short transp;
    /** from Netscape Loop extension: iteration count: 0..65535 */
    unsigned short iter;
} SavedImage;

EXTERN void MakeExtension(SavedImage *New, int Function);
EXTERN int AddExtensionBlock(SavedImage *New, int Len, GifByteType ExtData[]);
EXTERN void FreeExtension(SavedImage *Image);

EXTERN SavedImage *MakeSavedImage(GifFileType *GifFile, SavedImage *CopyFrom);
EXTERN void FreeSavedImages(GifFileType *GifFile);

#ifdef __cplusplus
  static int _GifError;
};
#else
  extern int _GifError;
#endif

#endif /* CGIF_H */
