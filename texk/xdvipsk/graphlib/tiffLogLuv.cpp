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
// XYZ to RGB TIFF conversion routines
//
// ==========================================================

#include "bitmap.h"
#include "utilities.h"

void tiff_ConvertLineXYZToRGB(BYTE *target, BYTE *source, double stonits, int width_in_pixels) {
	FIRGBF *rgbf = (FIRGBF*)target;
	float *xyz = (float*)source;
	
	for (int cols = 0; cols < width_in_pixels; cols++) {
		// assume CCIR-709 primaries (matrix from tif_luv.c)
		// LOG Luv XYZ (D65) -> sRGB (CIE Illuminant E)
		rgbf->red	= (float)( 2.690*xyz[0] + -1.276*xyz[1] + -0.414*xyz[2]);
		rgbf->green	= (float)(-1.022*xyz[0] +  1.978*xyz[1] +  0.044*xyz[2]);
		rgbf->blue	= (float)( 0.061*xyz[0] + -0.224*xyz[1] +  1.163*xyz[2]);
		
		/*
		if (stonits != 0.0) {
			rgbf->red	= (float)(rgbf->red   * stonits);
			rgbf->green	= (float)(rgbf->green * stonits);
			rgbf->blue	= (float)(rgbf->blue  * stonits);
		} 
		*/

		rgbf++;
		xyz += 3;
	}
}

void tiff_ConvertLineRGBToXYZ(BYTE *target, BYTE *source, int width_in_pixels) {
	FIRGBF *rgbf = (FIRGBF*)source;
	float *xyz = (float*)target;
	
	for (int cols = 0; cols < width_in_pixels; cols++) {
		// assume CCIR-709 primaries, whitepoint x = 1/3 y = 1/3 (D_E)
		// "The LogLuv Encoding for Full Gamut, High Dynamic Range Images" <G.Ward>
		// sRGB ( CIE Illuminant E ) -> LOG Luv XYZ (D65)
		xyz[0] =  (float)(0.497*rgbf->red +  0.339*rgbf->green +  0.164*rgbf->blue);
		xyz[1] =  (float)(0.256*rgbf->red +  0.678*rgbf->green +  0.066*rgbf->blue);
		xyz[2] =  (float)(0.023*rgbf->red +  0.113*rgbf->green +  0.864*rgbf->blue);

		rgbf++;
		xyz += 3;
	}
}

