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
// Input/Output functions
//
// ==========================================================

#ifndef BITMAPIO_H
#define BITMAPIO_H

#ifndef BITMAP_H
#include "bitmap.h"
#endif

// ----------------------------------------------------------

FI_STRUCT (FIMEMORYHEADER) {
	/**
	Flag used to remember to delete the 'data' buffer.
	When the buffer is a wrapped buffer, it is read-only, no need to delete it. 
	When the buffer is a read/write buffer, it is allocated dynamically and must be deleted when no longer needed.
	*/
	BOOL delete_me;
	/**
	file_length is equal to the input buffer size when the buffer is a wrapped buffer, i.e. file_length == data_length. 
	file_length is the amount of the written bytes when the buffer is a read/write buffer.
	*/
	long file_length;
	/**
	When using read-only input buffers, data_length is equal to the input buffer size, i.e. the file_length.
	When using read/write buffers, data_length is the size of the allocated buffer, 
	whose size is greater than or equal to file_length.
	*/
	long data_length;
	/**
	start buffer address
	*/
	void *data;
	/**
	Current position into the memory stream
	*/
	long current_position;
};

void SetDefaultIO(BitmapIO *io);

void SetMemoryIO(BitmapIO *io);

#endif // !BITMAPIO_H
