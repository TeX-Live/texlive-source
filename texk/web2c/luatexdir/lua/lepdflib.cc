/* lepdflib.cc

   Copyright 2009-2010 Taco Hoekwater <taco@luatex.org>

   This file is part of LuaTeX.

   LuaTeX is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2 of the License, or (at your
   option) any later version.

   LuaTeX is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
   License for more details.

   You should have received a copy of the GNU General Public License along
   with LuaTeX; if not, see <http://www.gnu.org/licenses/>. */

static const char _svn_version[] =
    "$Id: lepdflib.cc 3366 2010-01-20 14:49:31Z taco $ "
    "$URL: http://foundry.supelec.fr/svn/luatex/branches/0.60.x/source/texk/web2c/luatexdir/lua/lepdflib.cc $";

#include "image/epdf.h"

//**********************************************************************

int l_new_pdfdoc(lua_State * L)
{
    PdfDocument *pdf_doc;
    char *file_path;
    if (lua_gettop(L) != 1)
        luaL_error(L, "epdf.new() needs exactly 1 argument");
    if (!lua_isstring(L, -1))
        luaL_error(L, "epdf.new() needs filename (string)");
    file_path = (char *) lua_tostring(L, -1);

    printf("\n======================== 1 <%s>\n", file_path);

    pdf_doc = refPdfDocument(file_path);

    return 0;
}

//**********************************************************************

static const struct luaL_Reg epdflib[] = {
    // {"new", l_new_pdfdoc},
    {NULL, NULL}                /* sentinel */
};

int luaopen_epdf(lua_State * L)
{
    luaL_register(L, "epdf", epdflib);
    return 1;
}

//**********************************************************************
