//========================================================================
//
// TiffWriter.cc
//
// This file is licensed under the GPLv2 or later
//
// Copyright (C) 2010, 2012 William Bader <williambader@hotmail.com>
// Copyright (C) 2012 Albert Astals Cid <aacid@kde.org>
//
//========================================================================

#include "TiffWriter.h"

#if ENABLE_LIBTIFF

#include <string.h>

TiffWriter::~TiffWriter()
{
  // no cleanup needed
}

TiffWriter::TiffWriter()
{
  f = NULL;
  numRows = 0;
  curRow = 0;
  compressionString = NULL;
  splashMode = splashModeRGB8;
}

// Set the compression type

void TiffWriter::setCompressionString(const char *compressionStringArg)
{
  compressionString = compressionStringArg;
}

// Set the bitmap mode

void TiffWriter::setSplashMode(SplashColorMode splashModeArg)
{
  splashMode = splashModeArg;
}

// Write a TIFF file.

bool TiffWriter::init(FILE *openedFile, int width, int height, int hDPI, int vDPI)
{
  unsigned int compression;
  uint16 photometric;
  uint32 rowsperstrip = (uint32) -1;
  int bitspersample;
  uint16 samplesperpixel;
  const struct compression_name_tag {
    const char *compressionName;		// name of the compression option from the command line
    unsigned int compressionCode;		// internal libtiff code
    const char *compressionDescription;		// descriptive name
  } compressionList[] = {
    { "none",	COMPRESSION_NONE,	"no compression" },
    { "ccittrle", COMPRESSION_CCITTRLE,	"CCITT modified Huffman RLE" },
    { "ccittfax3", COMPRESSION_CCITTFAX3,"CCITT Group 3 fax encoding" },
    { "ccittt4", COMPRESSION_CCITT_T4,	"CCITT T.4 (TIFF 6 name)" },
    { "ccittfax4", COMPRESSION_CCITTFAX4, "CCITT Group 4 fax encoding" },
    { "ccittt6", COMPRESSION_CCITT_T6,	"CCITT T.6 (TIFF 6 name)" },
    { "lzw",	COMPRESSION_LZW,	"Lempel-Ziv  & Welch" },
    { "ojpeg",	COMPRESSION_OJPEG,	"!6.0 JPEG" },
    { "jpeg",	COMPRESSION_JPEG,	"%JPEG DCT compression" },
    { "next",	COMPRESSION_NEXT,	"NeXT 2-bit RLE" },
    { "packbits", COMPRESSION_PACKBITS,	"Macintosh RLE" },
    { "ccittrlew", COMPRESSION_CCITTRLEW, "CCITT modified Huffman RLE w/ word alignment" },
    { "deflate", COMPRESSION_DEFLATE,	"Deflate compression" },
    { "adeflate", COMPRESSION_ADOBE_DEFLATE, "Deflate compression, as recognized by Adobe" },
    { "dcs",	COMPRESSION_DCS,	"Kodak DCS encoding" },
    { "jbig",	COMPRESSION_JBIG,	"ISO JBIG" },
    { "jp2000",	COMPRESSION_JP2000,	"Leadtools JPEG2000" },
    { NULL,	0,			NULL }
  };

  // Initialize

  f = NULL;
  curRow = 0;

  // Store the number of rows

  numRows = height;

  // Set the compression

  compression = COMPRESSION_NONE;

  if (compressionString == NULL || strcmp(compressionString, "") == 0) {
    compression = COMPRESSION_NONE;
  } else {
    int i;
    for (i = 0; compressionList[i].compressionName != NULL; i++) {
      if (strcmp(compressionString, compressionList[i].compressionName) == 0) {
	compression = compressionList[i].compressionCode;
	break;
      }
    }
    if (compressionList[i].compressionName == NULL) {
      fprintf(stderr, "TiffWriter: Unknown compression type '%.10s', using 'none'.\n", compressionString);
      fprintf(stderr, "Known compression types (the tiff library might not support every type)\n");
      for (i = 0; compressionList[i].compressionName != NULL; i++) {
	fprintf(stderr, "%10s %s\n", compressionList[i].compressionName, compressionList[i].compressionDescription);
      }
    }
  }

  // Set bits per sample, samples per pixel, and photometric type from the splash mode

  bitspersample = (splashMode == splashModeMono1? 1: 8);

  switch (splashMode) {

  case splashModeMono1:
  case splashModeMono8:
    samplesperpixel = 1;
    photometric = PHOTOMETRIC_MINISBLACK;
    break;

  case splashModeRGB8:
  case splashModeBGR8:
    samplesperpixel = 3;
    photometric = PHOTOMETRIC_RGB;
    break;

#if SPLASH_CMYK
  case splashModeCMYK8:
  case splashModeDeviceN8:
    samplesperpixel = 4;
    photometric = PHOTOMETRIC_SEPARATED;
    break;
#endif

  default:
    fprintf(stderr, "TiffWriter: Mode %d not supported\n", splashMode);
    return false;
  }

  // Open the file

  if (openedFile == NULL) {
    fprintf(stderr, "TiffWriter: No output file given.\n");
    return false;
  }

  f = TIFFFdOpen(fileno(openedFile), "-", "w");

  if (!f) {
    return false;
  }

  // Set TIFF tags

  TIFFSetField(f, TIFFTAG_IMAGEWIDTH,  width);
  TIFFSetField(f, TIFFTAG_IMAGELENGTH, height);
  TIFFSetField(f, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
  TIFFSetField(f, TIFFTAG_SAMPLESPERPIXEL, samplesperpixel);
  TIFFSetField(f, TIFFTAG_BITSPERSAMPLE, bitspersample);
  TIFFSetField(f, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(f, TIFFTAG_PHOTOMETRIC, photometric);
  TIFFSetField(f, TIFFTAG_COMPRESSION, (uint16) compression);
  TIFFSetField(f, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(f, rowsperstrip));
  TIFFSetField(f, TIFFTAG_XRESOLUTION, (double) hDPI);
  TIFFSetField(f, TIFFTAG_YRESOLUTION, (double) vDPI);
  TIFFSetField(f, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);

#if SPLASH_CMYK
  if (splashMode == splashModeCMYK8 || splashMode == splashModeDeviceN8) {
    TIFFSetField(f, TIFFTAG_INKSET, INKSET_CMYK);
    TIFFSetField(f, TIFFTAG_NUMBEROFINKS, 4);
  }
#endif

  return true;
}

bool TiffWriter::writePointers(unsigned char **rowPointers, int rowCount)
{
  // Write all rows to the file

  for (int row = 0; row < rowCount; row++) {
    if (TIFFWriteScanline(f, rowPointers[row], row, 0) < 0) {
      fprintf(stderr, "TiffWriter: Error writing tiff row %d\n", row);
      return false;
    }
  }

  return true;
}

bool TiffWriter::writeRow(unsigned char **rowData)
{
  // Add a single row

  if (TIFFWriteScanline(f, *rowData, curRow, 0) < 0) {
    fprintf(stderr, "TiffWriter: Error writing tiff row %d\n", curRow);
    return false;
  }

  curRow++;

  return true;
}

bool TiffWriter::close()
{
  // Close the file

  TIFFClose(f);

  return true;
}

#endif
