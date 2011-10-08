//========================================================================
//
// JpegWriter.cc
//
// This file is licensed under the GPLv2 or later
//
// Copyright (C) 2009 Stefan Thomas <thomas@eload24.com>
// Copyright (C) 2010 Adrian Johnson <ajohnson@redneon.com>
// Copyright (C) 2010 Harry Roberts <harry.roberts@midnight-labs.org>
// Copyright (C) 2011 Thomas Freitag <Thomas.Freitag@alfa.de>
//
//========================================================================

#include "JpegWriter.h"

#ifdef ENABLE_LIBJPEG

#include "poppler/Error.h"

void outputMessage(j_common_ptr cinfo)
{
	char buffer[JMSG_LENGTH_MAX];

	// Create the message
	(*cinfo->err->format_message) (cinfo, buffer);

	// Send it to poppler's error handler
	error(-1, "%s", buffer);
}

JpegWriter::JpegWriter(int q, bool p, J_COLOR_SPACE cm)
: progressive(p), quality(q), colorMode(cm)
{
}

JpegWriter::JpegWriter(J_COLOR_SPACE cm)
: progressive(false), quality(-1), colorMode(cm)
{
}

JpegWriter::~JpegWriter()
{
	// cleanup
	jpeg_destroy_compress(&cinfo);
}

bool JpegWriter::init(FILE *f, int width, int height, int hDPI, int vDPI)
{
	// Setup error handler
	cinfo.err = jpeg_std_error(&jerr);
	jerr.output_message = &outputMessage;

	// Initialize libjpeg
	jpeg_create_compress(&cinfo);
	
	// Set destination file
	jpeg_stdio_dest(&cinfo, f);
	
	// Set libjpeg configuration
	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.density_unit = 1; // dots per inch
	cinfo.X_density = hDPI;
	cinfo.Y_density = vDPI;
	cinfo.in_color_space = colorMode; /* colorspace of input image */
	/* # of color components per pixel */
	switch (colorMode) {
		case JCS_GRAYSCALE:
			cinfo.input_components = 1;     
			break;
		case JCS_RGB:
			cinfo.input_components = 3;     
			break;
		case JCS_CMYK:
			cinfo.input_components = 4;     
			break;
		default:
			return false;
	}
	jpeg_set_defaults(&cinfo);
	if (cinfo.in_color_space == JCS_CMYK) {
		jpeg_set_colorspace(&cinfo, JCS_YCCK);
		cinfo.write_JFIF_header = TRUE;
	}
	
	// Set quality
	if( quality >= 0 && quality <= 100 ) { 
		jpeg_set_quality(&cinfo, quality, true);
	}
	
	// Use progressive mode
	if( progressive) {
		jpeg_simple_progression(&cinfo);
	}
	
	// Get ready for data
	jpeg_start_compress(&cinfo, TRUE);
	
	return true;
}

bool JpegWriter::writePointers(unsigned char **rowPointers, int rowCount)
{
	if (colorMode == JCS_CMYK) {
		for (int y = 0; y < rowCount; y++) {
			unsigned char *row = rowPointers[y];
			for (unsigned int x = 0; x < cinfo.image_width; x++) {
				for (int n = 0; n < 4; n++) {
					*row = 0xff - *row;
					row++;
				}
			}
		}
	}
	// Write all rows to the file
	jpeg_write_scanlines(&cinfo, rowPointers, rowCount);
	
	return true;
}

bool JpegWriter::writeRow(unsigned char **rowPointer)
{
	if (colorMode == JCS_CMYK) {
		unsigned char *row = rowPointer[0];
		for (unsigned int x = 0; x < cinfo.image_width; x++) {
			for (int n = 0; n < 4; n++) {
				*row = 0xff - *row;
				row++;
			}
		}
	}
	// Write the row to the file
	jpeg_write_scanlines(&cinfo, rowPointer, 1);
	
	return true;
}

bool JpegWriter::close()
{
	jpeg_finish_compress(&cinfo);
	
	return true;
}

#endif
