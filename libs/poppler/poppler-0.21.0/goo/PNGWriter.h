//========================================================================
//
// PNGWriter.h
//
// This file is licensed under the GPLv2 or later
//
// Copyright (C) 2009 Warren Toomey <wkt@tuhs.org>
// Copyright (C) 2009 Shen Liang <shenzhuxi@gmail.com>
// Copyright (C) 2009, 2011 Albert Astals Cid <aacid@kde.org>
// Copyright (C) 2009 Stefan Thomas <thomas@eload24.com>
// Copyright (C) 2010, 2011 Adrian Johnson <ajohnson@redneon.com>
//
//========================================================================

#ifndef PNGWRITER_H
#define PNGWRITER_H

#include "poppler-config.h"

#ifdef ENABLE_LIBPNG

#include <cstdio>
#include <png.h>
#include "ImgWriter.h"

class PNGWriter : public ImgWriter
{
	public:

		/* RGB        - 3 bytes/pixel
		 * RGBA       - 4 bytes/pixel
		 * GRAY       - 1 byte/pixel
		 * MONOCHROME - 1 byte/pixel. PNGWriter will bitpack to 8 pixels/byte
		 */
		enum Format { RGB, RGBA, GRAY, MONOCHROME };

		PNGWriter(Format format = RGB);
		~PNGWriter();

		void setICCProfile(const char *name, unsigned char *data, int size);
		void setSRGBProfile();

		
		bool init(FILE *f, int width, int height, int hDPI, int vDPI);
		
		bool writePointers(unsigned char **rowPointers, int rowCount);
		bool writeRow(unsigned char **row);
		
		bool close();
	
	private:
		Format format;
		png_structp png_ptr;
		png_infop info_ptr;
		unsigned char *icc_data;
		int icc_data_size;
		char *icc_name;
		bool sRGB_profile;
};

#endif

#endif
