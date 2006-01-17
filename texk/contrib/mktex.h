/* mktex.h: definitions for mktex lib exports.

Copyright (C) 1997 Fabrice POPINEAU.
Adapted to MS-DOS/DJGPP by Eli Zaretskii <eliz@is.elta.co.il>.

Time-stamp: <02/12/23 00:33:08 popineau>

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

#ifndef _MKTEX_H_
#define _MKTEX_H_

#include "mktexlib.h"

/* Pointer to functions that accept argc and argv[] */
typedef int (*fp)(int, string*);

typedef struct _program_description {
  string name;
  int arg_min;
  int arg_max;
  fp prog;
} program_description;

extern MKTEXDLL int program_number;
extern MKTEXDLL string progname;
extern MKTEXDLL char empty_str[];
extern MKTEXDLL FILE *fout;
extern MKTEXDLL FILE *fnul;
extern MKTEXDLL string output;
extern MKTEXDLL string progname;
extern MKTEXDLL char tmpdir[];
extern MKTEXDLL void mktexinit(int, char**);
extern MKTEXDLL int mktexpk(int, char**);
extern MKTEXDLL int mktextfm(int, char**);
extern MKTEXDLL int mktextex(int, char**);
extern MKTEXDLL int mktexmf(int, char**);
extern MKTEXDLL int mktexlsr(int, char**);
extern MKTEXDLL int mktexupdate(int, char**);
extern MKTEXDLL int mktexmkdir(int, char**);
extern MKTEXDLL int mktexrmdir(int, char**);
extern MKTEXDLL int mktexnames(int, char**);
extern MKTEXDLL int mktex_opt(int, char**, program_description *);
extern MKTEXDLL program_description makedesc[];
#endif /* _MKTEX_H_ */

