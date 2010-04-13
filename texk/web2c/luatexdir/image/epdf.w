% epdf.w

% Copyright 1996-2006 Han The Thanh <thanh@@pdftex.org>
% Copyright 2006-2009 Taco Hoekwater <taco@@luatex.org>

% This file is part of LuaTeX.

% LuaTeX is free software; you can redistribute it and/or modify it under
% the terms of the GNU General Public License as published by the Free
% Software Foundation; either version 2 of the License, or (at your
% option) any later version.

% LuaTeX is distributed in the hope that it will be useful, but WITHOUT
% ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
% FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
% License for more details.

% You should have received a copy of the GNU General Public License along
% with LuaTeX; if not, see <http://www.gnu.org/licenses/>. 

@ @c
static const char _svn_version[] =
    "$Id: epdf.w 3584 2010-04-02 17:45:55Z hhenkel $ "
    "$URL: http://foundry.supelec.fr/svn/luatex/branches/0.60.x/source/texk/web2c/luatexdir/image/epdf.w $";

#include "ptexlib.h"

@ This must the be shortest C file we have that actually does something.

@c
void epdf_free(void)
{
    epdf_check_mem();
}
