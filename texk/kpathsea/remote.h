/* remote.h: declarations for retrieving remote files

Copyright (C) 1994, 95 Karl Berry.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef REMOTE_H
#define REMOTE_H

#ifdef HAVE_GETURL
#include <wininet.h>

extern KPSEDLL string get_remote_file(const_string);
extern void cleanup_remote_files();
#endif

#endif /* REMOTE_H */
