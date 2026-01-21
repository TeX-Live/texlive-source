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

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include "bitmap.h"
#include "bitmapIO.h"

// =====================================================================
// File IO functions
// =====================================================================

unsigned 
_ReadProc(void *buffer, unsigned size, unsigned count, fi_handle handle) {
	return (unsigned)fread(buffer, size, count, (FILE *)handle);
}

unsigned 
_WriteProc(void *buffer, unsigned size, unsigned count, fi_handle handle) {
	return (unsigned)fwrite(buffer, size, count, (FILE *)handle);
}

int
_SeekProc(fi_handle handle, long offset, int origin) {
	return fseek((FILE *)handle, offset, origin);
}

long
_TellProc(fi_handle handle) {
	return ftell((FILE *)handle);
}

// ----------------------------------------------------------

void
SetDefaultIO(BitmapIO *io) {
	io->read_proc  = _ReadProc;
	io->seek_proc  = _SeekProc;
	io->tell_proc  = _TellProc;
	io->write_proc = _WriteProc;
}

// =====================================================================
// Memory IO functions
// =====================================================================

unsigned
_MemoryReadProc(void *buffer, unsigned size, unsigned count, fi_handle handle) {
	unsigned x;

	FIMEMORYHEADER *mem_header = (FIMEMORYHEADER*)(((FIMEMORY*)handle)->data);

	for(x = 0; x < count; x++) {
		long remaining_bytes = mem_header->file_length - mem_header->current_position;
		//if there isn't size bytes left to read, set pos to eof and return a short count
		if( remaining_bytes < (long)size ) {
			if(remaining_bytes > 0) {
				memcpy( buffer, (char *)mem_header->data + mem_header->current_position, remaining_bytes );
			}
			mem_header->current_position = mem_header->file_length;
			break;
		}
		//copy size bytes count times
		memcpy( buffer, (char *)mem_header->data + mem_header->current_position, size );
		mem_header->current_position += size;
		buffer = (char *)buffer + size;
	}
	return x;
}

unsigned
_MemoryWriteProc(void *buffer, unsigned size, unsigned count, fi_handle handle) {
	void *newdata;
	long newdatalen;

	FIMEMORYHEADER *mem_header = (FIMEMORYHEADER*)(((FIMEMORY*)handle)->data);

	//double the data block size if we need to
	while( (mem_header->current_position + (long)(size * count)) >= mem_header->data_length ) {
		//if we are at or above 1G, we cant double without going negative
		if( mem_header->data_length & 0x40000000 ) {
			//max 2G
			if( mem_header->data_length == 0x7FFFFFFF ) {
				return 0;
			}
			newdatalen = 0x7FFFFFFF;
		} else if( mem_header->data_length == 0 ) {
			//default to 4K if nothing yet
			newdatalen = 4096;
		} else {
			//double size
			newdatalen = mem_header->data_length << 1;
		}
		newdata = realloc( mem_header->data, newdatalen );
		if( !newdata ) {
			return 0;
		}
		mem_header->data = newdata;
		mem_header->data_length = newdatalen;
	}
	memcpy( (char *)mem_header->data + mem_header->current_position, buffer, size * count );
	mem_header->current_position += size * count;
	if( mem_header->current_position > mem_header->file_length ) {
		mem_header->file_length = mem_header->current_position;
	}
	return count;
}

int
_MemorySeekProc(fi_handle handle, long offset, int origin) {
	FIMEMORYHEADER *mem_header = (FIMEMORYHEADER*)(((FIMEMORY*)handle)->data);

	// you can use _MemorySeekProc to reposition the pointer anywhere in a file
	// the pointer can also be positioned beyond the end of the file

	switch(origin) { //0 to filelen-1 are 'inside' the file
		default:
		case SEEK_SET: //can fseek() to 0-7FFFFFFF always
			if( offset >= 0 ) {
				mem_header->current_position = offset;
				return 0;
			}
			break;

		case SEEK_CUR:
			if( mem_header->current_position + offset >= 0 ) {
				mem_header->current_position += offset;
				return 0;
			}
			break;

		case SEEK_END:
			if( mem_header->file_length + offset >= 0 ) {
				mem_header->current_position = mem_header->file_length + offset;
				return 0;
			}
			break;
	}

	return -1;
}

long
_MemoryTellProc(fi_handle handle) {
	FIMEMORYHEADER *mem_header = (FIMEMORYHEADER*)(((FIMEMORY*)handle)->data);

	return mem_header->current_position;
}

// ----------------------------------------------------------

void
SetMemoryIO(BitmapIO *io) {
	io->read_proc  = _MemoryReadProc;
	io->seek_proc  = _MemorySeekProc;
	io->tell_proc  = _MemoryTellProc;
	io->write_proc = _MemoryWriteProc;
}
