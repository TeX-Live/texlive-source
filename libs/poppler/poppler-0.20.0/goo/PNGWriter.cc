//========================================================================
//
// PNGWriter.cc
//
// This file is licensed under the GPLv2 or later
//
// Copyright (C) 2009 Warren Toomey <wkt@tuhs.org>
// Copyright (C) 2009 Shen Liang <shenzhuxi@gmail.com>
// Copyright (C) 2009, 2011 Albert Astals Cid <aacid@kde.org>
// Copyright (C) 2009 Stefan Thomas <thomas@eload24.com>
// Copyright (C) 2010, 2011 Adrian Johnson <ajohnson@redneon.com>
// Copyright (C) 2011 Thomas Klausner <wiz@danbala.tuwien.ac.at>
//
//========================================================================

#include "PNGWriter.h"

#ifdef ENABLE_LIBPNG

#include <zlib.h>
#include <stdlib.h>
#include <string.h>

#include "poppler/Error.h"
#include "goo/gmem.h"

PNGWriter::PNGWriter(Format formatA) : format(formatA)
{
	icc_data = NULL;
	icc_data_size = 0;
	icc_name = NULL;
	sRGB_profile = false;
}

PNGWriter::~PNGWriter()
{
	/* cleanup heap allocation */
	png_destroy_write_struct(&png_ptr, &info_ptr);
	if (icc_data) {
		gfree(icc_data);
		free(icc_name);
	}
}

void PNGWriter::setICCProfile(const char *name, unsigned char *data, int size)
{
	icc_data = (unsigned char *)gmalloc(size);
	memcpy(icc_data, data, size);
	icc_data_size = size;
	icc_name = strdup(name);
}

void PNGWriter::setSRGBProfile()
{
	sRGB_profile = true;
}

bool PNGWriter::init(FILE *f, int width, int height, int hDPI, int vDPI)
{
  /* libpng changed the png_set_iCCP() prototype in 1.5.0 */
#if PNG_LIBPNG_VER < 10500
        png_charp icc_data_ptr = (png_charp)icc_data;
#else
        png_const_bytep icc_data_ptr = (png_const_bytep)icc_data;
#endif

	/* initialize stuff */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		error(errInternal, -1, "png_create_write_struct failed");
		return false;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		error(errInternal, -1, "png_create_info_struct failed");
		return false;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		error(errInternal, -1, "png_jmpbuf failed");
		return false;
	}

	/* write header */
	png_init_io(png_ptr, f);
	if (setjmp(png_jmpbuf(png_ptr))) {
		error(errInternal, -1, "Error during writing header");
		return false;
	}
	
	// Set up the type of PNG image and the compression level
	png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

	// Silence silly gcc
	png_byte bit_depth = -1;
	png_byte color_type = -1;
	switch (format) {
		case RGB:
			bit_depth = 8;
			color_type = PNG_COLOR_TYPE_RGB;
			break;
		case RGBA:
			bit_depth = 8;
			color_type = PNG_COLOR_TYPE_RGB_ALPHA;
			break;
		case GRAY:
			bit_depth = 8;
			color_type = PNG_COLOR_TYPE_GRAY;
			break;
		case MONOCHROME:
			bit_depth = 1;
			color_type = PNG_COLOR_TYPE_GRAY;
			break;
	}
	png_byte interlace_type = PNG_INTERLACE_NONE;

	png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type, interlace_type, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	png_set_pHYs(png_ptr, info_ptr, hDPI/0.0254, vDPI/0.0254, PNG_RESOLUTION_METER);

	if (icc_data)
		png_set_iCCP(png_ptr, info_ptr, icc_name, PNG_COMPRESSION_TYPE_BASE, icc_data_ptr, icc_data_size);
	else if (sRGB_profile)
		png_set_sRGB(png_ptr, info_ptr, PNG_sRGB_INTENT_RELATIVE);

	png_write_info(png_ptr, info_ptr);
	if (setjmp(png_jmpbuf(png_ptr))) {
		error(errInternal, -1, "error during writing png info bytes");
		return false;
	}

	// pack 1 pixel/byte rows into 8 pixels/byte
	if (format == MONOCHROME)
		png_set_packing(png_ptr);

	return true;
}

bool PNGWriter::writePointers(unsigned char **rowPointers, int rowCount)
{
	png_write_image(png_ptr, rowPointers);
	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr))) {
		error(errInternal, -1, "Error during writing bytes");
		return false;
	}
	
	return true;
}

bool PNGWriter::writeRow(unsigned char **row)
{
	// Write the row to the file
	png_write_rows(png_ptr, row, 1);
	if (setjmp(png_jmpbuf(png_ptr))) {
		error(errInternal, -1, "error during png row write");
		return false;
	}
	
	return true;
}

bool PNGWriter::close()
{
	/* end write */
	png_write_end(png_ptr, info_ptr);
	if (setjmp(png_jmpbuf(png_ptr))) {
		error(errInternal, -1, "Error during end of write");
		return false;
	}
	
	return true;
}

#endif
