/* lpdflib.c

   Copyright 2006-2010 Taco Hoekwater <taco@luatex.org>

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
    "$Id: lpdflib.c 3941 2010-11-01 23:31:27Z hhenkel $ "
    "$URL: http://foundry.supelec.fr/svn/luatex/tags/beta-0.66.0/source/texk/web2c/luatexdir/lua/lpdflib.c $";

#include "ptexlib.h"
#include "lua/luatex-api.h"

static int luapdfprint(lua_State * L)
{
    int n;
    const_lstring st, modestr;
    ctm_transform_modes literal_mode;
    st.s = modestr.s = NULL;
    n = lua_gettop(L);
    if (!lua_isstring(L, -1)) {
        luaL_error(L, "no string to print");
    }
    literal_mode = set_origin;
    if (n == 2) {
        if (!lua_isstring(L, -2)) {
            luaL_error(L, "invalid argument for print literal mode");
        } else {
            modestr.s = lua_tolstring(L, -2, &modestr.l);
            if (modestr.l == 6 && strncmp(modestr.s, "direct", 6) == 0)
                literal_mode = direct_always;
            else if (modestr.l == 4 && strncmp(modestr.s, "page", 4) == 0)
                literal_mode = direct_page;
            else {
                luaL_error(L, "invalid argument for print literal mode");
            }
        }
    } else {
        if (n != 1) {
            luaL_error(L, "invalid number of arguments");
        }
    }
    check_o_mode(static_pdf, "pdf.print()", 1 << OMODE_PDF, true);
    switch (literal_mode) {
    case (set_origin):
        pdf_goto_pagemode(static_pdf);
        pdf_set_pos(static_pdf, static_pdf->posstruct->pos);
        (void) calc_pdfpos(static_pdf->pstruct, static_pdf->posstruct->pos);
        break;
    case (direct_page):
        pdf_goto_pagemode(static_pdf);
        (void) calc_pdfpos(static_pdf->pstruct, static_pdf->posstruct->pos);
        break;
    case (direct_always):
        pdf_end_string_nl(static_pdf);
        break;
    default:
        assert(0);
    }
    st.s = lua_tolstring(L, n, &st.l);
    pdf_out_block(static_pdf, st.s, st.l);
    return 0;
}

static unsigned char *fread_to_buf(lua_State * L, const char *filename,
                                   size_t * len)
{
    int ilen = 0;
    FILE *f;
    unsigned char *buf = NULL;
    if ((f = fopen(filename, "rb")) == NULL)
        luaL_error(L, "pdf.immediateobj() cannot open input file");
    if (readbinfile(f, &buf, &ilen) == 0)
        luaL_error(L, "pdf.immediateobj() cannot read input file");
    fclose(f);
    *len = (size_t) ilen;
    return buf;
}

static int l_immediateobj(lua_State * L)
{
    int n, first_arg = 1;
    int k;
    lstring buf;
    const_lstring st1, st2, st3;
    st1.s = st2.s = st3.s = NULL;
    check_o_mode(static_pdf, "immediateobj()", 1 << OMODE_PDF, true);
    if (global_shipping_mode != NOT_SHIPPING)
        luaL_error(L, "pdf.immediateobj() can not be used with \\latelua");
    n = lua_gettop(L);
    if (n > 0 && lua_type(L, 1) == LUA_TNUMBER) {
        first_arg++;
        lua_number2int(k, lua_tonumber(L, 1));
        check_obj_type(static_pdf, obj_type_obj, k);
        if (is_obj_scheduled(static_pdf, k) || obj_data_ptr(static_pdf, k) != 0)
            luaL_error(L, "pdf.immediateobj() object in use");
    } else {
        static_pdf->obj_count++;
        k = pdf_create_obj(static_pdf, obj_type_obj, static_pdf->obj_ptr + 1);
    }
    pdf_last_obj = k;
    switch (n - first_arg + 1) {
    case 0:
        luaL_error(L, "pdf.immediateobj() needs at least one argument");
        break;
    case 1:
        if (!lua_isstring(L, first_arg))
            luaL_error(L, "pdf.immediateobj() 1st argument must be string");
        pdf_begin_obj(static_pdf, k, 1);
        st1.s = lua_tolstring(L, first_arg, &st1.l);
        pdf_out_block(static_pdf, st1.s, st1.l);
        if (st1.s[st1.l - 1] != '\n')
            pdf_puts(static_pdf, "\n");
        pdf_end_obj(static_pdf);
        break;
    case 2:
    case 3:
        if (!lua_isstring(L, first_arg))
            luaL_error(L, "pdf.immediateobj() 1st argument must be string");
        if (!lua_isstring(L, first_arg + 1))
            luaL_error(L, "pdf.immediateobj() 2nd argument must be string");
        st1.s = lua_tolstring(L, first_arg, &st1.l);
        st2.s = lua_tolstring(L, first_arg + 1, &st2.l);
        if (st1.l == 4 && strncmp((const char *) st1.s, "file", 4) == 0) {
            if (n == first_arg + 2)
                luaL_error(L,
                           "pdf.immediateobj() 3rd argument forbidden in file mode");
            pdf_begin_obj(static_pdf, k, 1);
            buf.s = fread_to_buf(L, st2.s, &buf.l);
            pdf_out_block(static_pdf, (const char *) buf.s, buf.l);
            if (buf.s[buf.l - 1] != '\n')
                pdf_puts(static_pdf, "\n");
            xfree(buf.s);
            pdf_end_obj(static_pdf);
        } else {
            pdf_begin_dict(static_pdf, k, 0);   /* 0 = not an object stream candidate! */
            if (n == first_arg + 2) {   /* write attr text */
                if (!lua_isstring(L, first_arg + 2))
                    luaL_error(L,
                               "pdf.immediateobj() 3rd argument must be string");
                st3.s = lua_tolstring(L, first_arg + 2, &st3.l);
                pdf_out_block(static_pdf, st3.s, st3.l);
                if (st3.s[st3.l - 1] != '\n')
                    pdf_puts(static_pdf, "\n");
            }
            pdf_begin_stream(static_pdf);
            if (st1.l == 6 && strncmp((const char *) st1.s, "stream", 6) == 0) {
                pdf_out_block(static_pdf, st2.s, st2.l);
            } else if (st1.l == 10
                       && strncmp((const char *) st1.s, "streamfile",
                                  10) == 0) {
                buf.s = fread_to_buf(L, st2.s, &buf.l);
                pdf_out_block(static_pdf, (const char *) buf.s, buf.l);
                xfree(buf.s);
            } else
                luaL_error(L, "pdf.immediateobj() invalid argument");
            pdf_end_stream(static_pdf);
        }
        break;
    default:
        luaL_error(L, "pdf.immediateobj() allows max. 3 arguments");
    }
    lua_pushinteger(L, k);
    return 1;
}

/**********************************************************************/
/* for LUA_ENVIRONINDEX table lookup (instead of repeated strcmp()) */

typedef enum { P__ZERO,
    P_CATALOG,
    P_H,
    P_INFO,
    P_NAMES,
    P_PDFCATALOG,
    P_PDFINFO,
    P_PDFNAMES,
    P_PDFTRAILER,
    P_RAW,
    P_STREAM,
    P_TRAILER,
    P_V,
    P__SENTINEL
} parm_idx;

static const parm_struct pdf_parms[] = {
    {NULL, P__ZERO},            /* dummy; lua indices run from 1 */
    {"catalog", P_CATALOG},
    {"h", P_H},
    {"info", P_INFO},
    {"names", P_NAMES},
    {"pdfcatalog", P_PDFCATALOG},       /* obsolescent */
    {"pdfinfo", P_PDFINFO},     /* obsolescent */
    {"pdfnames", P_PDFNAMES},   /* obsolescent */
    {"pdftrailer", P_PDFTRAILER},       /* obsolescent */
    {"raw", P_RAW},
    {"stream", P_STREAM},
    {"trailer", P_TRAILER},
    {"v", P_V},
    {NULL, P__SENTINEL}
};

/**********************************************************************/

static int table_obj(lua_State * L)
{
    int k, type;
    int compress_level = -1;    /* unset */
    int os_level = 1;           /* default: put non-stream objects into object streams */
    int saved_compress_level = static_pdf->compress_level;
    const_lstring attr, st;
    lstring buf;
    int immediate = 0;          /* default: not immediate */
    attr.s = st.s = NULL;
    attr.l = 0;
    assert(lua_istable(L, 1));  /* t */

    /* get object "type" */

    lua_pushstring(L, "type");  /* ks t */
    lua_gettable(L, -2);        /* vs? t */
    if (lua_isnil(L, -1))       /* !vs t */
        luaL_error(L, "pdf.obj(): object \"type\" missing");
    if (!lua_isstring(L, -1))   /* !vs t */
        luaL_error(L, "pdf.obj(): object \"type\" must be string");
    lua_pushvalue(L, -1);       /* vs vs t */
    lua_gettable(L, LUA_ENVIRONINDEX);  /* i? vs t */
    if (!lua_isnumber(L, -1))   /* !i vs t */
        luaL_error(L, "pdf.obj(): \"%s\" is not a valid object type",
                   lua_tostring(L, -2));
    type = (int) lua_tointeger(L, -1);  /* i vs t */
    switch (type) {
    case P_RAW:
    case P_STREAM:
        break;
    default:
        luaL_error(L, "pdf.obj(): \"%s\" is not a valid object type", lua_tostring(L, -2));     /* i vs t */
    }
    lua_pop(L, 2);              /* t */

    /* get optional "immediate" */

    lua_pushstring(L, "immediate");     /* ks t */
    lua_gettable(L, -2);        /* b? t */
    if (!lua_isnil(L, -1)) {    /* b? t */
        if (!lua_isboolean(L, -1))      /* !b t */
            luaL_error(L, "pdf.obj(): \"immediate\" must be boolean");
        immediate = lua_toboolean(L, -1);       /* 0 or 1 */
    }
    lua_pop(L, 1);              /* t */

    /* is a reserved object referenced by "objnum"? */

    lua_pushstring(L, "objnum");        /* ks t */
    lua_gettable(L, -2);        /* vi? t */
    if (!lua_isnil(L, -1)) {    /* vi? t */
        if (!lua_isnumber(L, -1))       /* !vi t */
            luaL_error(L, "pdf.obj(): \"objnum\" must be integer");
        k = (int) lua_tointeger(L, -1); /* vi t */
        check_obj_type(static_pdf, obj_type_obj, k);
        if (is_obj_scheduled(static_pdf, k) || obj_data_ptr(static_pdf, k) != 0)
            luaL_error(L, "pdf.obj() object in use");
    } else {
        static_pdf->obj_count++;
        k = pdf_create_obj(static_pdf, obj_type_obj, static_pdf->obj_ptr + 1);
    }
    pdf_last_obj = k;
    if (immediate == 0) {
        obj_data_ptr(static_pdf, k) = pdf_get_mem(static_pdf, pdfmem_obj_size);
        init_obj_obj(static_pdf, k);
    }
    lua_pop(L, 1);              /* t */

    /* get optional "attr" (allowed only for stream case) */

    lua_pushstring(L, "attr");  /* ks t */
    lua_gettable(L, -2);        /* attr-s? t */
    if (!lua_isnil(L, -1)) {    /* attr-s? t */
        if (type != P_STREAM)
            luaL_error(L,
                       "pdf.obj(): \"attr\" key not allowed for non-stream object");
        if (!lua_isstring(L, -1))       /* !attr-s t */
            luaL_error(L, "pdf.obj(): object \"attr\" must be string");
        if (immediate == 1) {
            attr.s = lua_tolstring(L, -1, &attr.l);     /* attr-s t */
            lua_pop(L, 1);      /* t */
        } else
            obj_obj_stream_attr(static_pdf, k) = luaL_ref(Luas, LUA_REGISTRYINDEX);     /* t */
    } else
        lua_pop(L, 1);          /* t */

    /* get optional "compresslevel" (allowed only for stream case) */

    lua_pushstring(L, "compresslevel"); /* ks t */
    lua_gettable(L, -2);        /* vi? t */
    if (!lua_isnil(L, -1)) {    /* vi? t */
        if (type == P_RAW)
            luaL_error(L,
                       "pdf.obj(): \"compresslevel\" key not allowed for raw object");
        if (!lua_isnumber(L, -1))       /* !vi t */
            luaL_error(L, "pdf.obj(): \"compresslevel\" must be integer");
        compress_level = (int) lua_tointeger(L, -1);    /* vi t */
        if (compress_level > 9)
            luaL_error(L, "pdf.obj(): \"compresslevel\" must be <= 9");
        else if (compress_level < 0)
            luaL_error(L, "pdf.obj(): \"compresslevel\" must be >= 0");
        if (immediate == 0)
            obj_obj_pdfcompresslevel(static_pdf, k) = compress_level;
    }
    lua_pop(L, 1);              /* t */

    /* get optional "objcompression" (allowed only for non-stream case) */

    lua_pushstring(L, "objcompression");        /* ks t */
    lua_gettable(L, -2);        /* b? t */
    if (!lua_isnil(L, -1)) {    /* b? t */
        if (type == P_STREAM)
            luaL_error(L,
                       "pdf.obj(): \"objcompression\" key not allowed for stream object");
        if (!lua_isboolean(L, -1))      /* !b t */
            luaL_error(L, "pdf.obj(): \"objcompression\" must be boolean");
        os_level = lua_toboolean(L, -1);        /* 0 or 1 */
        /* 0: never compress; 1: depends then on \pdfobjcompresslevel */
        if (immediate == 0)
            obj_obj_pdfoslevel(static_pdf, k) = os_level;
    }
    lua_pop(L, 1);              /* t */

    /* now the object contents for all cases are handled */

    lua_pushstring(L, "string");        /* ks t */
    lua_gettable(L, -2);        /* string-s? t */
    lua_pushstring(L, "file");  /* ks string-s? t */
    lua_gettable(L, -3);        /* file-s? string-s? t */
    if (!lua_isnil(L, -1) && !lua_isnil(L, -2)) /* file-s? string-s? t */
        luaL_error(L,
                   "pdf.obj(): \"string\" and \"file\" must not be given together");
    if (lua_isnil(L, -1) && lua_isnil(L, -2))   /* nil nil t */
        luaL_error(L, "pdf.obj(): no \"string\" or \"file\" given");

    switch (type) {
    case P_RAW:
        if (immediate == 1)
            pdf_begin_obj(static_pdf, k, os_level);
        if (!lua_isnil(L, -2)) {        /* file-s? string-s? t */
            /* from string */
            lua_pop(L, 1);      /* string-s? t */
            if (!lua_isstring(L, -1))   /* !string-s t */
                luaL_error(L,
                           "pdf.obj(): \"string\" must be string for raw object");
            if (immediate == 1) {
                st.s = lua_tolstring(L, -1, &st.l);
                pdf_out_block(static_pdf, st.s, st.l);
                if (st.s[st.l - 1] != '\n')
                    pdf_puts(static_pdf, "\n");
            } else
                obj_obj_data(static_pdf, k) = luaL_ref(L, LUA_REGISTRYINDEX);   /* t */
        } else {
            /* from file */
            if (!lua_isstring(L, -1))   /* !file-s nil t */
                luaL_error(L,
                           "pdf.obj(): \"file\" name must be string for raw object");
            if (immediate == 1) {
                st.s = lua_tolstring(L, -1, &st.l);     /* file-s nil t */
                buf.s = fread_to_buf(L, st.s, &buf.l);
                pdf_out_block(static_pdf, (const char *) buf.s, buf.l);
                if (buf.s[buf.l - 1] != '\n')
                    pdf_puts(static_pdf, "\n");
                xfree(buf.s);
            } else {
                set_obj_obj_is_file(static_pdf, k);
                obj_obj_data(static_pdf, k) = luaL_ref(L, LUA_REGISTRYINDEX);   /* nil t */
            }
        }
        if (immediate == 1)
            pdf_end_obj(static_pdf);
        break;
    case P_STREAM:
        if (immediate == 1) {
            pdf_begin_dict(static_pdf, k, 0);   /* 0 = not an object stream candidate! */
            if (attr.s != NULL) {
                pdf_out_block(static_pdf, attr.s, attr.l);
                if (attr.s[attr.l - 1] != '\n')
                    pdf_puts(static_pdf, "\n");
            }
            if (compress_level > -1)
                static_pdf->compress_level = compress_level;
            pdf_begin_stream(static_pdf);
        } else {
            set_obj_obj_is_stream(static_pdf, k);
            if (compress_level > -1)
                obj_obj_pdfcompresslevel(static_pdf, k) = compress_level;
        }
        if (!lua_isnil(L, -2)) {        /* file-s? string-s? t */
            /* from string */
            lua_pop(L, 1);      /* string-s? t */
            if (!lua_isstring(L, -1))   /* !string-s t */
                luaL_error(L,
                           "pdf.obj(): \"string\" must be string for stream object");
            if (immediate == 1) {
                st.s = lua_tolstring(L, -1, &st.l);     /* string-s t */
                pdf_out_block(static_pdf, st.s, st.l);
            } else
                obj_obj_data(static_pdf, k) = luaL_ref(L, LUA_REGISTRYINDEX);   /* t */
        } else {
            /* from file */
            if (!lua_isstring(L, -1))   /* !file-s nil t */
                luaL_error(L,
                           "pdf.obj(): \"file\" name must be string for stream object");
            if (immediate == 1) {
                st.s = lua_tolstring(L, -1, &st.l);     /* file-s nil t */
                buf.s = fread_to_buf(L, st.s, &buf.l);
                pdf_out_block(static_pdf, (const char *) buf.s, buf.l);
                xfree(buf.s);
            } else {
                set_obj_obj_is_file(static_pdf, k);
                obj_obj_data(static_pdf, k) = luaL_ref(L, LUA_REGISTRYINDEX);   /* nil t */
            }
        }
        if (immediate == 1)
            pdf_end_stream(static_pdf);
        break;
    default:
        assert(0);
    }
    static_pdf->compress_level = saved_compress_level;
    return k;
}

static int orig_obj(lua_State * L)
{
    int n, first_arg = 1;
    int k;
    const_lstring st;
    st.s = NULL;
    n = lua_gettop(L);
    if (n > 0 && lua_type(L, 1) == LUA_TNUMBER) {
        first_arg++;
        lua_number2int(k, lua_tonumber(L, 1));
        check_obj_type(static_pdf, obj_type_obj, k);
        if (is_obj_scheduled(static_pdf, k) || obj_data_ptr(static_pdf, k) != 0)
            luaL_error(L, "pdf.obj() object in use");
    } else {
        static_pdf->obj_count++;
        k = pdf_create_obj(static_pdf, obj_type_obj, static_pdf->obj_ptr + 1);
    }
    pdf_last_obj = k;
    obj_data_ptr(static_pdf, k) = pdf_get_mem(static_pdf, pdfmem_obj_size);
    init_obj_obj(static_pdf, k);
    switch (n - first_arg + 1) {
    case 0:
        luaL_error(L, "pdf.obj() needs at least one argument");
        break;
    case 1:
        if (!lua_isstring(L, first_arg))
            luaL_error(L, "pdf.obj() 1st argument must be string");
        break;
    case 2:
    case 3:
        if (!lua_isstring(L, first_arg))
            luaL_error(L, "pdf.obj() 1st argument must be string");
        if (!lua_isstring(L, first_arg + 1))
            luaL_error(L, "pdf.obj() 2nd argument must be string");
        st.s = lua_tolstring(L, first_arg, &st.l);
        if (st.l == 4 && strncmp((const char *) st.s, "file", 4) == 0) {
            if (n == first_arg + 2)
                luaL_error(L, "pdf.obj() 3rd argument forbidden in file mode");
            set_obj_obj_is_file(static_pdf, k);
        } else {
            if (n == first_arg + 2) {   /* write attr text */
                if (!lua_isstring(L, -1))
                    luaL_error(L, "pdf.obj() 3rd argument must be string");
                obj_obj_stream_attr(static_pdf, k) =
                    luaL_ref(Luas, LUA_REGISTRYINDEX);
            }
            if (st.l == 6 && strncmp((const char *) st.s, "stream", 6) == 0) {
                set_obj_obj_is_stream(static_pdf, k);
            } else if (st.l == 10
                       && strncmp((const char *) st.s, "streamfile", 10) == 0) {
                set_obj_obj_is_stream(static_pdf, k);
                set_obj_obj_is_file(static_pdf, k);
            } else
                luaL_error(L, "pdf.obj() invalid argument");
        }
        break;
    default:
        luaL_error(L, "pdf.obj() allows max. 3 arguments");
    }
    obj_obj_data(static_pdf, k) = luaL_ref(L, LUA_REGISTRYINDEX);
    return k;
}

static int l_obj(lua_State * L)
{
    int k, n;
    ensure_output_state(static_pdf, ST_HEADER_WRITTEN);
    n = lua_gettop(L);
    if (n == 1 && lua_istable(L, 1))
        k = table_obj(L);       /* new */
    else
        k = orig_obj(L);
    lua_pushinteger(L, k);
    return 1;
}

static int l_refobj(lua_State * L)
{
    int k, n;
    n = lua_gettop(L);
    if (n != 1)
        luaL_error(L, "pdf.refobj() needs exactly 1 argument");
    k = (int) luaL_checkinteger(L, 1);
    if (global_shipping_mode == NOT_SHIPPING)
        scan_refobj_lua(static_pdf, k);
    else
        pdf_ref_obj_lua(static_pdf, k);
    return 0;
}

static int l_reserveobj(lua_State * L)
{
    int n;
    const_lstring st;
    st.s = 0;
    n = lua_gettop(L);
    switch (n) {
    case 0:
        static_pdf->obj_count++;
        pdf_last_obj =
            pdf_create_obj(static_pdf, obj_type_obj, static_pdf->obj_ptr + 1);
        break;
    case 1:
        if (!lua_isstring(L, -1))
            luaL_error(L, "pdf.reserveobj() optional argument must be string");
        st.s = lua_tolstring(L, 1, &st.l);
        if (st.l == 5 && strncmp((const char *) st.s, "annot", 5) == 0) {
            pdf_last_annot = pdf_create_obj(static_pdf, obj_type_annot, 0);
        } else {
            luaL_error(L, "pdf.reserveobj() optional string must be \"annot\"");
        }
        lua_pop(L, 1);
        break;
    default:
        luaL_error(L, "pdf.reserveobj() allows max. 1 argument");
    }
    lua_pushinteger(L, static_pdf->obj_ptr);
    return 1;
}

static int l_registerannot(lua_State * L)
{
    int n, i;
    n = lua_gettop(L);
    switch (n) {
    case 1:
        if (global_shipping_mode == NOT_SHIPPING)
            luaL_error(L, "pdf.registerannot() can only be used in late lua");
        i = (int) luaL_checkinteger(L, 1);
        if (i <= 0)
            luaL_error(L,
                       "pdf.registerannot() can only register positive object numbers");
        addto_page_resources(static_pdf, obj_type_annot, i);
        break;
    default:
        luaL_error(L, "pdf.registerannot() needs exactly 1 argument");
    }
    return 0;
}

static int getpdf(lua_State * L)
{
    char *s;
    int i, l;
    if (lua_isstring(L, 2) && (lua_tostring(L, 2) != NULL)) {
        lua_pushvalue(L, 2);    /* st ... */
        lua_gettable(L, LUA_ENVIRONINDEX);      /* i? ... */
        if (lua_isnumber(L, -1)) {      /* i ... */
            i = (int) lua_tointeger(L, -1);     /* i ... */
            lua_pop(L, 1);      /* ... */
            switch (i) {
            case P_PDFCATALOG:
            case P_CATALOG:
                s = tokenlist_to_cstring(pdf_catalog_toks, true, &l);
                lua_pushlstring(L, s, (size_t) l);
                break;
            case P_PDFINFO:
            case P_INFO:
                s = tokenlist_to_cstring(pdf_info_toks, true, &l);
                lua_pushlstring(L, s, (size_t) l);
                break;
            case P_PDFNAMES:
            case P_NAMES:
                s = tokenlist_to_cstring(pdf_names_toks, true, &l);
                lua_pushlstring(L, s, (size_t) l);
                break;
            case P_PDFTRAILER:
            case P_TRAILER:
                s = tokenlist_to_cstring(pdf_trailer_toks, true, &l);
                lua_pushlstring(L, s, (size_t) l);
                break;
            case P_H:
                lua_pushnumber(L, static_pdf->posstruct->pos.h);
                break;
            case P_V:
                lua_pushnumber(L, static_pdf->posstruct->pos.v);
                break;
            default:
                lua_rawget(L, -2);
            }
        } else {
            lua_pop(L, 1);      /* ... */
            lua_rawget(L, -2);
        }
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int setpdf(lua_State * L)
{
    int i;
    if (lua_gettop(L) != 3) {
        return 0;
    }
    (void) luaL_checkstring(L, 2);      /* ... */
    lua_pushvalue(L, 2);        /* st ... */
    lua_gettable(L, LUA_ENVIRONINDEX);  /* i? ... */
    if (lua_isnumber(L, -1)) {  /* i ... */
        i = (int) lua_tointeger(L, -1); /* i ... */
        lua_pop(L, 1);          /* ... */
        switch (i) {
        case P_PDFCATALOG:
        case P_CATALOG:
            pdf_catalog_toks = tokenlist_from_lua(L);
            break;
        case P_PDFINFO:
        case P_INFO:
            pdf_info_toks = tokenlist_from_lua(L);
            break;
        case P_PDFNAMES:
        case P_NAMES:
            pdf_names_toks = tokenlist_from_lua(L);
            break;
        case P_PDFTRAILER:
        case P_TRAILER:
            pdf_trailer_toks = tokenlist_from_lua(L);
            break;
        case P_H:
        case P_V:
            /* can't set |h| and |v| yet */
        default:
            lua_rawset(L, -3);
        }
    } else {
        lua_pop(L, 1);          /* ... */
        lua_rawset(L, -3);
    }
    return 0;
}

static int l_objtype(lua_State * L)
{
    int n = lua_gettop(L);
    if (n != 1)
        luaL_error(L, "pdf.objtype() needs exactly 1 argument");
    n = (int) luaL_checkinteger(L, 1);
    if (n < 0 || n > static_pdf->obj_ptr)
        lua_pushnil(L);
    else
        lua_pushstring(L, pdf_obj_typenames[obj_type(static_pdf, n)]);
    return 1;
}

static int l_maxobjnum(lua_State * L)
{
    int n = lua_gettop(L);
    if (n != 0)
        luaL_error(L, "pdf.maxobjnum() needs 0 arguments");
    lua_pushinteger(L, static_pdf->obj_ptr);
    return 1;
}

static int l_mapfile(lua_State * L)
{
    char *s;
    const char *st;
    if (lua_isstring(L, -1) && (st = lua_tostring(L, -1)) != NULL) {
        s = xstrdup(st);
        process_map_item(s, MAPFILE);
        free(s);
    }
    return 0;
}

static int l_mapline(lua_State * L)
{
    char *s;
    const char *st;
    if (lua_isstring(L, -1) && (st = lua_tostring(L, -1)) != NULL) {
        s = xstrdup(st);
        process_map_item(s, MAPLINE);
        free(s);
    }
    return 0;
}

static int l_pdfmapfile(lua_State * L)
{
    luaL_error(L, "pdf.pdfmapfile() is obsolete. Use pdf.mapfile() instead.");
    return 0;
}

static int l_pdfmapline(lua_State * L)
{
    luaL_error(L, "pdf.pdfmapline() is obsolete. Use pdf.mapline() instead.");
    return 0;
}

static int l_pageref(lua_State * L)
{
    int n = lua_gettop(L);
    if (n != 1)
        luaL_error(L, "pdf.pageref() needs exactly 1 argument");
    n = (int) luaL_checkinteger(L, 1);
    if (n <= 0)
        luaL_error(L, "pdf.pageref() needs page number > 0");
    n = get_obj(static_pdf, obj_type_page, n, false);
    lua_pushnumber(L, n);
    return 1;
}

static const struct luaL_reg pdflib[] = {
    {"immediateobj", l_immediateobj},
    {"mapfile", l_mapfile},
    {"mapline", l_mapline},
    {"maxobjnum", l_maxobjnum},
    {"obj", l_obj},
    {"objtype", l_objtype},
    {"pageref", l_pageref},
    {"pdfmapfile", l_pdfmapfile},       /* obsolete */
    {"pdfmapline", l_pdfmapline},       /* obsolete */
    {"print", luapdfprint},
    {"refobj", l_refobj},
    {"registerannot", l_registerannot},
    {"reserveobj", l_reserveobj},
    {NULL, NULL}                /* sentinel */
};

/**********************************************************************/

int luaopen_pdf(lua_State * L)
{
    preset_environment(L, pdf_parms);
    luaL_register(L, "pdf", pdflib);
    /* build meta table */
    luaL_newmetatable(L, "pdf_meta");
    lua_pushstring(L, "__index");
    lua_pushcfunction(L, getpdf);
    /* do these later, NYI */
    lua_settable(L, -3);
    lua_pushstring(L, "__newindex");
    lua_pushcfunction(L, setpdf);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);    /* meta to itself */
    return 1;
}
