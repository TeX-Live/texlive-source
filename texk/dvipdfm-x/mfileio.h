/* This is dvipdfmx, an eXtended version of dvipdfm by Mark A. Wicks.

    Copyright (C) 2002-2014 by Jin-Hwan Cho and Shunsaku Hirata,
    the dvipdfmx project team.
    
    Copyright (C) 1998, 1999 by Mark A. Wicks <mwicks@kettering.edu>

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

#ifndef _MFILEIO_H_
#define _MFILEIO_H_

#include <stdio.h>
#include "numbers.h"

#ifdef IODEBUG
FILE *mfopen (const char *name, const char *mode,
	      const char *function, int line);
int mfclose (FILE *file, const char *function, int line);
#define MFOPEN(name,mode) \
   mfopen((name),(mode),__FUNCTION__,__LINE__)
#define MFCLOSE(file) \
   mfclose((file),__FUNCTION__,__LINE__)
#else
#if defined(WIN32)
#define MFOPEN(name,mode) fsyscp_fopen((name),(mode))
#else
#define MFOPEN(name,mode) fopen((name),(mode))
#endif
#define MFCLOSE(file) fclose(file)
#endif

extern UNSIGNED_BYTE read_byte (FILE *);

extern void seek_absolute (FILE *file, long pos);
extern void seek_relative (FILE *file, long pos);

extern void seek_end (FILE *file);

extern long tell_position (FILE *file);

extern long file_size (FILE *file);

extern char *mfgets (char *buffer, unsigned long size, FILE *file);

extern char work_buffer[];

#define WORK_BUFFER_SIZE 1024

#endif /* _MFILEIO_H_ */
