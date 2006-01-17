/* fmtutil.h: definitions for fmtutil lib exports.

Copyright (C) 1998 Fabrice POPINEAU.

Time-stamp: <02/04/03 15:15:00 popineau>

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

#ifndef _FMTUTIL_H_
#define _FMTUTIL_H_

#include <kpathsea/c-proto.h>
#include <kpathsea/debug.h>

#define FMTUTIL_DEBUG (KPSE_LAST_DEBUG+5)

typedef void (* process_fn )(string);

void move_log(string fname);
void move_fmt(string fname);
void remove_file(string fname);
void process_multiple_files(string pattern, process_fn);
void cache_vars();
int recreate_all();
int create_missing();
int create_one_format(string);
int recreate_by_hyphenfile(string);
int show_hyphen_file(string);
int edit_fmtutil_cnf();

#endif /* _FMTUTIL_H_ */
