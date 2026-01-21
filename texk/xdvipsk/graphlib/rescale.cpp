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
// Upsampling / downsampling routine
//
// ==========================================================

#include "resize.h"

FIBITMAP *
Bitmap_RescaleRect(FIBITMAP *src, int dst_width, int dst_height, int src_left, int src_top, int src_right, int src_bottom, BITMAP_FILTER filter, unsigned flags) {
	FIBITMAP *dst = NULL;

	const int src_width = Bitmap_GetWidth(src);
	const int src_height = Bitmap_GetHeight(src);

	if (!Bitmap_HasPixels(src) || (dst_width <= 0) || (dst_height <= 0) || (src_width <= 0) || (src_height <= 0)) {
		return NULL;
	}

	// normalize the rectangle
	if (src_right < src_left) {
		int tmp = src_left;
		src_left = src_right;
		src_right = tmp;
	}
	if (src_bottom < src_top) {
		int tmp = src_top;
		src_top = src_bottom;
		src_bottom = tmp;
	}

	// check the size of the sub image
	if((src_left < 0) || (src_right > src_width) || (src_top < 0) || (src_bottom > src_height)) {
		return NULL;
	}

	// select the filter
	CGenericFilter *pFilter = NULL;
	switch (filter) {
		case FILTER_BOX:
			pFilter = new CBoxFilter();
			break;
		case FILTER_BICUBIC:
			pFilter = new CBicubicFilter();
			break;
		case FILTER_BILINEAR:
			pFilter = new CBilinearFilter();
			break;
		case FILTER_BSPLINE:
			pFilter = new CBSplineFilter();
			break;
		case FILTER_CATMULLROM:
			pFilter = new CCatmullRomFilter();
			break;
		case FILTER_LANCZOS3:
			pFilter = new CLanczos3Filter();
			break;
	}

	if (!pFilter) {
		return NULL;
	}

	CResizeEngine Engine(pFilter);

	dst = Engine.scale(src, dst_width, dst_height, src_left, src_top,
			src_right - src_left, src_bottom - src_top, flags);

	delete pFilter;

	return dst;
}

FIBITMAP *
Bitmap_Rescale(FIBITMAP *src, int dst_width, int dst_height, BITMAP_FILTER filter) {
	return Bitmap_RescaleRect(src, dst_width, dst_height, 0, 0, Bitmap_GetWidth(src), Bitmap_GetHeight(src), filter, FI_RESCALE_DEFAULT);
}
