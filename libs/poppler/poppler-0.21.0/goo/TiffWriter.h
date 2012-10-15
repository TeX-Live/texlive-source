//========================================================================
//
// TiffWriter.h
//
// This file is licensed under the GPLv2 or later
//
// Copyright (C) 2010, 2012 William Bader <williambader@hotmail.com>
// Copyright (C) 2011 Albert Astals Cid <aacid@kde.org>
//
//========================================================================

#ifndef TIFFWRITER_H
#define TIFFWRITER_H

#include "poppler-config.h"

#ifdef ENABLE_LIBTIFF

#include <sys/types.h>
#include "ImgWriter.h"
#include "splash/SplashTypes.h"

extern "C" {
#include "tiffio.h"
}

class TiffWriter : public ImgWriter
{
	public:
		TiffWriter();
		~TiffWriter();
		
		void setCompressionString(const char *compressionStringArg);
		void setSplashMode(SplashColorMode splashModeArg);

		bool init(FILE *openedFile, int width, int height, int hDPI, int vDPI);
		
		bool writePointers(unsigned char **rowPointers, int rowCount);
		bool writeRow(unsigned char **rowData);
		
		bool supportCMYK() { return true; }

		bool close();
	
	private:
		TIFF *f;				// LibTiff file context
		int numRows;				// number of rows in the image
		int curRow;				// number of rows written
		const char *compressionString;		// compression type
		SplashColorMode splashMode;		// format of image data

};

#endif

#endif
