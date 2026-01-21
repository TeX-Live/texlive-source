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

#ifndef LOADERS_H
#define LOADERS_H

#include "bitmap.h"


// ==========================================================
//   Internal validators/loaders
// ==========================================================

BOOL ValidateBMP(BitmapIO *io, fi_handle handle);
FIBITMAP *LoadBMP(BitmapIO *io, fi_handle handle, int page, int flags);

BOOL ValidatePCX(BitmapIO *io, fi_handle handle);
FIBITMAP *LoadPCX(BitmapIO *io, fi_handle handle, int page, int flags);

BOOL ValidateJPEG(BitmapIO *io, fi_handle handle);
FIBITMAP *LoadJPEG(BitmapIO *io, fi_handle handle, int page, int flags);

BOOL ValidatePNG(BitmapIO *io, fi_handle handle);
FIBITMAP *LoadPNG(BitmapIO *io, fi_handle handle, int page, int flags);

BOOL ValidateTIFF(BitmapIO *io, fi_handle handle);
FIBITMAP *LoadTIFF(BitmapIO *io, fi_handle handle, int page, int flags);

#endif //!LOADERS_H
