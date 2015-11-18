/* limglib.c

   Copyright 2006-2013 Taco Hoekwater <taco@luatex.org>

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

#include "ptexlib.h"
#include "lua/luatex-api.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "lua.h"
#include "lauxlib.h"

#define IMG_ENV "image.env"

/**********************************************************************/

#ifdef DEBUG
void stackDump(lua_State * L, char *s)
{
    int i, t, top = lua_gettop(L);
    printf("\n=== stackDump <%s>: ", s);
    for (i = top; i >= 1; i--) {
        t = lua_type(L, i);
        printf("%d: ", i);
        switch (t) {
            case LUA_TSTRING:
                printf("`%s'", lua_tostring(L, i));
                break;
            case LUA_TBOOLEAN:
                printf(lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:
                printf("%g", (double) lua_tonumber(L, i));
                break;
            default:
                printf("%s", lua_typename(L, t));
                break;
        }
        /* insert a separator */
        printf("  ");
    }
    printf("\n");
}
#endif

/**********************************************************************/

typedef enum { P__ZERO, P_ATTR, P_BBOX, P_COLORDEPTH, P_COLORSPACE, P_DEPTH,
    P_FILENAME, P_FILEPATH, P_HEIGHT, P_IMAGETYPE, P_INDEX, P_OBJNUM,
    P_PAGEBOX, P_PAGE, P_TOTALPAGES, P_ROTATION, P_STREAM, P_TRANSFORM,
    P_VISIBLEFILENAME, P_WIDTH, P_XRES, P_XSIZE, P_YRES, P_YSIZE, P__SENTINEL
} parm_idx;

static const parm_struct img_parms[] = {
    {NULL, P__ZERO},            /* dummy; lua indices run from 1 */
    {"attr", P_ATTR},
    {"bbox", P_BBOX},
    {"colordepth", P_COLORDEPTH},
    {"colorspace", P_COLORSPACE},
    {"depth", P_DEPTH},
    {"filename", P_FILENAME},
    {"filepath", P_FILEPATH},
    {"height", P_HEIGHT},
    {"imagetype", P_IMAGETYPE},
    {"index", P_INDEX},
    {"objnum", P_OBJNUM},
    {"pagebox", P_PAGEBOX},
    {"page", P_PAGE},
    {"pages", P_TOTALPAGES},
    {"rotation", P_ROTATION},
    {"stream", P_STREAM},
    {"transform", P_TRANSFORM},
    {"visiblefilename", P_VISIBLEFILENAME},
    {"width", P_WIDTH},
    {"xres", P_XRES},
    {"xsize", P_XSIZE},
    {"yres", P_YRES},
    {"ysize", P_YSIZE},
    {NULL, P__SENTINEL}
};

#define imgtype_max 7

const char *imgtype_s[] = {
    "none",
    "pdf",
    "png",
    "jpg",
    "jp2",
    "jbig2",
    "stream",
    "memstream",
    NULL
};

#define pagebox_max 5

const char *pdfboxspec_s[] = {
    "none",
    "media",
    "crop",
    "bleed",
    "trim",
    "art",
    NULL
};

/**********************************************************************/

static void image_to_lua(lua_State * L, image * a)
{                               /* key user ... */
    int i, j;
    image_dict *d = img_dict(a);
    assert(d != NULL);
    lua_pushstring(L, IMG_ENV); /* s k u ... */
    lua_gettable(L, LUA_REGISTRYINDEX); /* t k u ... */
    lua_pushvalue(L, -2);       /* k t k u ... */
    lua_gettable(L, -2);        /* i? t k u ... */
    if (lua_type(L, -1) != LUA_TNUMBER)   /* !i t k u ... */
        luaL_error(L, "image_to_lua(): %s is not a valid image key", lua_tostring(L, -3));
    i = (int) lua_tointeger(L, -1);     /* i t k u ... */
    lua_pop(L, 3);              /* u ... */
    switch (i) {
    case P_WIDTH:
        if (is_wd_running(a))
            lua_pushnil(L);
        else
            lua_pushinteger(L, img_width(a));
        break;
    case P_HEIGHT:
        if (is_ht_running(a))
            lua_pushnil(L);
        else
            lua_pushinteger(L, img_height(a));
        break;
    case P_DEPTH:
        if (is_dp_running(a))
            lua_pushnil(L);
        else
            lua_pushinteger(L, img_depth(a));
        break;
    case P_TRANSFORM:
        lua_pushinteger(L, img_transform(a));
        break;
        /* now follow all image_dict entries */
    case P_FILENAME:
        if (img_filename(d) == NULL || strlen(img_filename(d)) == 0)
            lua_pushnil(L);
        else
            lua_pushstring(L, img_filename(d));
        break;
    case P_VISIBLEFILENAME:
        if (img_visiblefilename(d) == NULL
            || strlen(img_visiblefilename(d)) == 0)
            lua_pushnil(L);
        else
            lua_pushstring(L, img_visiblefilename(d));
        break;
    case P_FILEPATH:
        if (img_filepath(d) == NULL || strlen(img_filepath(d)) == 0)
            lua_pushnil(L);
        else
            lua_pushstring(L, img_filepath(d));
        break;
    case P_ATTR:
        if (img_attr(d) == NULL || strlen(img_attr(d)) == 0)
            lua_pushnil(L);
        else
            lua_pushstring(L, img_attr(d));
        break;
    case P_PAGE:
        if (img_pagename(d) != NULL && strlen(img_pagename(d)) != 0)
            lua_pushstring(L, img_pagename(d));
        else
            lua_pushinteger(L, img_pagenum(d));
        break;
    case P_TOTALPAGES:
        lua_pushinteger(L, img_totalpages(d));
        break;
    case P_XSIZE:              /* Modify by /Rotate only for output */
        if ((img_rotation(d) & 1) == 0)
            lua_pushinteger(L, img_xsize(d));
        else
            lua_pushinteger(L, img_ysize(d));
        break;
    case P_YSIZE:              /* Modify by /Rotate only for output */
        if ((img_rotation(d) & 1) == 0)
            lua_pushinteger(L, img_ysize(d));
        else
            lua_pushinteger(L, img_xsize(d));
        break;
    case P_XRES:
        lua_pushinteger(L, img_xres(d));
        break;
    case P_YRES:
        lua_pushinteger(L, img_yres(d));
        break;
    case P_ROTATION:
        lua_pushinteger(L, img_rotation(d));
        break;
    case P_COLORSPACE:
        if (img_colorspace(d) == 0)
            lua_pushnil(L);
        else
            lua_pushinteger(L, img_colorspace(d));
        break;
    case P_COLORDEPTH:
        if (img_colordepth(d) == 0)
            lua_pushnil(L);
        else
            lua_pushinteger(L, img_colordepth(d));
        break;
    case P_IMAGETYPE:
        j = img_type(d);
        if (j >= 0 && j <= imgtype_max) {
            if (j == IMG_TYPE_NONE)
                lua_pushnil(L);
            else
                lua_pushstring(L, imgtype_s[j]);
        } else
            assert(0);
        break;
    case P_PAGEBOX:
        j = img_pagebox(d);
        if (j >= 0 && j <= pagebox_max) {
            if (j == PDF_BOX_SPEC_NONE)
                lua_pushnil(L);
            else
                lua_pushstring(L, pdfboxspec_s[j]);
        } else
            assert(0);
        break;
    case P_BBOX:
        if (!img_is_bbox(d)) {
            img_bbox(d)[0] = img_xorig(d);
            img_bbox(d)[1] = img_yorig(d);
            img_bbox(d)[2] = img_xorig(d) + img_xsize(d);
            img_bbox(d)[3] = img_yorig(d) + img_ysize(d);
        }
        lua_newtable(L);
        lua_pushinteger(L, 1);
        lua_pushinteger(L, img_bbox(d)[0]);
        lua_settable(L, -3);
        lua_pushinteger(L, 2);
        lua_pushinteger(L, img_bbox(d)[1]);
        lua_settable(L, -3);
        lua_pushinteger(L, 3);
        lua_pushinteger(L, img_bbox(d)[2]);
        lua_settable(L, -3);
        lua_pushinteger(L, 4);
        lua_pushinteger(L, img_bbox(d)[3]);
        lua_settable(L, -3);
        break;
    case P_OBJNUM:
        if (img_objnum(d) == 0)
            lua_pushnil(L);
        else
            lua_pushinteger(L, img_objnum(d));
        break;
    case P_INDEX:
        if (img_index(d) == 0)
            lua_pushnil(L);
        else
            lua_pushinteger(L, img_index(d));
        break;
    case P_STREAM:
        if (img_type(d) != IMG_TYPE_PDFSTREAM || img_pdfstream_ptr(d) == NULL
            || img_pdfstream_stream(d) == NULL
            || strlen(img_pdfstream_stream(d)) == 0)
            lua_pushnil(L);
        else
            lua_pushstring(L, img_pdfstream_stream(d));
        break;
    default:
        assert(0);
    }                           /* v u ... */
}

static void lua_to_image(lua_State * L, image * a)
{                               /* value key table ... */
    int i;
    image_dict *d = img_dict(a);
    assert(d != NULL);
    lua_pushstring(L, IMG_ENV); /* s v k t ... */
    lua_gettable(L, LUA_REGISTRYINDEX); /* t v k t ... */
    lua_pushvalue(L, -3);       /* k t v k t ... */
    lua_gettable(L, -2);        /* i? t v k t ... */
    if (lua_type(L, -1) != LUA_TNUMBER)   /* !i t v k t ... */
        luaL_error(L, "lua_to_image(): %s is not a valid image key", lua_tostring(L, -4));
    i = (int) lua_tointeger(L, -1);     /* i t v k t ... */
    lua_pop(L, 2);              /* v k t ... */
    switch (i) {
    case P_WIDTH:
        if (lua_isnil(L, -1))
            set_wd_running(a);
        else if (lua_type(L, -1) == LUA_TNUMBER)
            img_width(a) = (int) lua_tointeger(L, -1);
        else if (lua_type(L, -1) == LUA_TSTRING)
            img_width(a) = dimen_to_number(L, lua_tostring(L, -1));
        else
            luaL_error(L, "image.width needs integer or nil value or dimension string");
        break;
    case P_HEIGHT:
        if (lua_isnil(L, -1))
            set_ht_running(a);
        else if (lua_type(L, -1) == LUA_TNUMBER)
            img_height(a) = (int) lua_tointeger(L, -1);
        else if (lua_type(L, -1) == LUA_TSTRING)
            img_height(a) = dimen_to_number(L, lua_tostring(L, -1));
        else
            luaL_error(L, "image.height needs integer or nil value or dimension string");
        break;
    case P_DEPTH:
        if (lua_isnil(L, -1))
            set_dp_running(a);
        else if (lua_type(L, -1) == LUA_TNUMBER)
            img_depth(a) = (int) lua_tointeger(L, -1);
        else if (lua_type(L, -1) == LUA_TSTRING)
            img_depth(a) = dimen_to_number(L, lua_tostring(L, -1));
        else
            luaL_error(L, "image.depth needs integer or nil value or dimension string");
        break;
    case P_TRANSFORM:
        if (lua_type(L, -1) == LUA_TNUMBER)
            img_transform(a) = (int) lua_tointeger(L, -1);
        else
            luaL_error(L, "image.transform needs integer value");
        break;
        /* now follow all image_dict entries */
    case P_FILENAME:
        if (img_state(d) >= DICT_FILESCANNED)
            luaL_error(L, "image.filename is now read-only");
        if (img_type(d) == IMG_TYPE_PDFSTREAM)
            luaL_error(L, "image.filename can't be used with image.stream");
        if (lua_type(L, -1) == LUA_TSTRING) {
            xfree(img_filename(d));
            img_filename(d) = xstrdup(lua_tostring(L, -1));
        } else
            luaL_error(L, "image.filename needs string value");
        break;
    case P_VISIBLEFILENAME:
        if (img_state(d) >= DICT_FILESCANNED)
            luaL_error(L, "image.visiblefilename is now read-only");
        if (img_type(d) == IMG_TYPE_PDFSTREAM)
            luaL_error(L, "image.visiblefilename can't be used with image.stream");
        if (lua_type(L, -1) == LUA_TSTRING) {
            xfree(img_visiblefilename(d));
            img_visiblefilename(d) = xstrdup(lua_tostring(L, -1));
        } else
            luaL_error(L, "image.visiblefilename needs string value");
        break;
    case P_ATTR:
        if (img_state(d) >= DICT_FILESCANNED)
            luaL_error(L, "image.attr is now read-only");
        if (lua_type(L, -1) == LUA_TSTRING) {
            xfree(img_attr(d));
            img_attr(d) = xstrdup(lua_tostring(L, -1));
        } else if (lua_type(L, -1) == LUA_TNIL) {
            xfree(img_attr(d));
        } else
            luaL_error(L, "image.attr needs string or nil value");
        break;
    case P_PAGE:
        if (img_state(d) >= DICT_FILESCANNED)
            luaL_error(L, "image.page is now read-only");
        if (lua_type(L, -1) == LUA_TSTRING) {
            xfree(img_pagename(d));
            img_pagename(d) = xstrdup(lua_tostring(L, -1));
            img_pagenum(d) = 0;
        } else if (lua_type(L, -1) == LUA_TNUMBER) {
            img_pagenum(d) = (int) lua_tointeger(L, -1);
            xfree(img_pagename(d));
        } else
            luaL_error(L, "image.page needs integer or string value");
        break;
    case P_COLORSPACE:
        if (img_state(d) >= DICT_FILESCANNED)
            luaL_error(L, "image.colorspace is now read-only");
        if (lua_isnil(L, -1))
            img_colorspace(d) = 0;
        else if (lua_type(L, -1) == LUA_TNUMBER)
            img_colorspace(d) = (int) lua_tointeger(L, -1);
        else
            luaL_error(L, "image.colorspace needs integer or nil value");
        break;
    case P_PAGEBOX:
        if (img_state(d) >= DICT_FILESCANNED)
            luaL_error(L, "image.pagebox is now read-only");
        if (lua_type(L, -1) == LUA_TNIL)
            img_pagebox(d) = PDF_BOX_SPEC_NONE;
        else if (lua_type(L, -1) == LUA_TSTRING)
            img_pagebox(d) = luaL_checkoption(L, -1, "none", pdfboxspec_s);
        else
            luaL_error(L, "image.pagebox needs string or nil value");
        break;
    case P_BBOX:
        if (img_state(d) >= DICT_FILESCANNED)
            luaL_error(L, "image.bbox is now read-only");
        if (!lua_istable(L, -1))
            luaL_error(L, "image.bbox needs table value");
        if (lua_rawlen(L, -1) != 4)
            luaL_error(L, "image.bbox table must have exactly 4 elements");
        for (i = 1; i <= 4; i++) {      /* v k t ... */
            lua_pushinteger(L, i);      /* idx v k t ... */
            lua_gettable(L, -2);        /* int v k t ... */
            if (lua_type(L, -1) == LUA_TNUMBER)
                img_bbox(d)[i - 1] = (int) lua_tointeger(L, -1);
            else if (lua_type(L, -1) == LUA_TSTRING)
                img_bbox(d)[i - 1] = dimen_to_number(L, lua_tostring(L, -1));
            else
                luaL_error(L, "image.bbox table needs integer value or dimension string elements");
            lua_pop(L, 1);      /* v k t ... */
        }
        img_set_bbox(d);
        break;
    case P_STREAM:
        if (img_filename(d) != NULL)
            luaL_error(L, "image.stream can't be used with image.filename");
        if (img_state(d) >= DICT_FILESCANNED)
            luaL_error(L, "image.stream is now read-only");
        if (img_pdfstream_ptr(d) == NULL)
            new_img_pdfstream_struct(d);
        xfree(img_pdfstream_stream(d));
        img_pdfstream_stream(d) = xstrdup(lua_tostring(L, -1));
        img_type(d) = IMG_TYPE_PDFSTREAM;
        break;
    case P_FILEPATH:
    case P_TOTALPAGES:
    case P_XSIZE:
    case P_YSIZE:
    case P_XRES:
    case P_YRES:
    case P_ROTATION:
    case P_IMAGETYPE:
    case P_OBJNUM:
    case P_INDEX:
    case P_COLORDEPTH:
        luaL_error(L, "image.%s is a read-only variable", img_parms[i].name);
        break;
    default:
        assert(0);
    }                           /* v k t ... */
}

/**********************************************************************/

static void copy_image(lua_State * L, lua_Number scale)
{
    image *a, **aa, *b, **bb;
    if (lua_gettop(L) != 1)
        luaL_error(L, "img.copy() needs exactly 1 argument");
    aa = (image **) luaL_checkudata(L, 1, TYPE_IMG);    /* a */
    lua_pop(L, 1);              /* - */
    a = *aa;
    bb = (image **) lua_newuserdata(L, sizeof(image *));        /* b */
    luaL_getmetatable(L, TYPE_IMG);     /* m b */
    lua_setmetatable(L, -2);    /* b */
    b = *bb = new_image();
    if (!is_wd_running(a))
        img_width(b) = do_zround(img_width(a) * scale);
    if (!is_ht_running(a))
        img_height(b) = do_zround(img_height(a) * scale);
    if (!is_dp_running(a))
        img_depth(b) = do_zround(img_depth(a) * scale);
    img_transform(b) = img_transform(a);
    img_dict(b) = img_dict(a);
    if (img_dictref(a) != LUA_NOREF) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, img_dictref(a));      /* ad b */
        img_dictref(b) = luaL_ref(L, LUA_REGISTRYINDEX);        /* b */
    } else
        assert(img_state(img_dict(a)) >= DICT_REFERED);
}

/**********************************************************************/

int l_new_image(lua_State * L)
{
    image *a, **aa;
    image_dict **add;
    if (lua_gettop(L) > 1)
        luaL_error(L, "img.new() needs maximum 1 argument");
    if (lua_gettop(L) == 1 && !lua_istable(L, -1))
        luaL_error(L, "img.new() needs table as optional argument");    /* (t) */
    aa = (image **) lua_newuserdata(L, sizeof(image *));        /* i (t) */
    luaL_getmetatable(L, TYPE_IMG);     /* m i (t) */
    lua_setmetatable(L, -2);    /* i (t) */
    a = *aa = new_image();
    add = (image_dict **) lua_newuserdata(L, sizeof(image_dict *));     /* ad i (t) */
    luaL_getmetatable(L, TYPE_IMG_DICT);        /* m ad i (t) */
    lua_setmetatable(L, -2);    /* ad i (t) */
    img_dict(a) = *add = new_image_dict();
    img_dictref(a) = luaL_ref(L, LUA_REGISTRYINDEX);    /* i (t) */
    if (lua_gettop(L) == 2) {   /* i t, else just i */
        lua_insert(L, -2);      /* t i */
        lua_pushnil(L);         /* n t i (1st key for iterator) */
        while (lua_next(L, -2) != 0) {  /* v k t i */
            lua_to_image(L, a); /* v k t i */
            lua_pop(L, 1);      /* k t i */
        }                       /* t i */
        lua_pop(L, 1);          /* i */
    }                           /* i */
    return 1;                   /* i */
}

static int l_copy_image(lua_State * L)
{
    if (lua_gettop(L) != 1)
        luaL_error(L, "img.copy() needs exactly 1 argument");
    if (lua_istable(L, 1))
        (void) l_new_image(L);  /* image --- if everything worked well */
    else
        (void) copy_image(L, 1.0);      /* image */
    return 1;                   /* image */
}

static void read_scale_img(image * a)
{
    image_dict *ad;
    assert(a != NULL);
    ad = img_dict(a);
    assert(ad != NULL);
    if (img_state(ad) == DICT_NEW) {
        if (img_type(ad) == IMG_TYPE_PDFSTREAM)
            check_pdfstream_dict(ad);
        else {
            fix_pdf_minorversion(static_pdf);
            read_img(static_pdf, ad, pdf_minor_version, pdf_inclusion_errorlevel);
        }
    }
    if (is_wd_running(a) || is_ht_running(a) || is_dp_running(a))
        img_dimen(a) = scale_img(ad, img_dimen(a), img_transform(a));
}

static int l_scan_image(lua_State * L)
{
    image *a, **aa;
    if (lua_gettop(L) != 1)
        luaL_error(L, "img.scan() needs exactly 1 argument");
    if (lua_istable(L, 1))
        (void) l_new_image(L);  /* image --- if everything worked well */
    aa = (image **) luaL_checkudata(L, 1, TYPE_IMG);    /* image */
    a = *aa;
    check_o_mode(static_pdf, "img.scan()", 1 << OMODE_PDF, false);
    /* flush_str(last_tex_string); *//* ?? */
    read_scale_img(a);
    return 1;                   /* image */
}

static halfword img_to_node(image * a)
{
    image_dict *ad;
    halfword n;
    assert(a != NULL);
    ad = img_dict(a);
    assert(ad != NULL);
    assert(img_objnum(ad) != 0);
    n = new_rule(image_rule);
    rule_index(n) = img_index(ad);
    width(n) = img_width(a);
    height(n) = img_height(a);
    depth(n) = img_depth(a);
    rule_transform(n) = img_transform(a);
    return n;
}

typedef enum {
    WR_WRITE,
    WR_IMMEDIATEWRITE,
    WR_NODE,
    WR_VF_IMG
} wrtype_e;

const char *wrtype_s[] = {
    "img.write()", "img.immediatewrite()", "img.node()", "write vf image"
};

static void setup_image(PDF pdf, image * a, wrtype_e writetype)
{
    image_dict *ad;
    assert(a != NULL);
    ad = img_dict(a);
    check_o_mode(pdf, wrtype_s[writetype], 1 << OMODE_PDF, false);
    /* flush_str(last_tex_string); *//* ?? */
    read_scale_img(a);
    if (img_objnum(ad) == 0) {  /* latest needed just before out_img() */
        pdf->ximage_count++;
        img_objnum(ad) = pdf_create_obj(pdf, obj_type_ximage, pdf->ximage_count);
        img_index(ad) = pdf->ximage_count;
        idict_to_array(ad);     /* now ad is read-only */
        obj_data_ptr(pdf, pdf->obj_ptr) = img_index(ad);
    }
}

static void write_image_or_node(lua_State * L, wrtype_e writetype)
{
    image *a, **aa;
    image_dict *ad;
    halfword n;
    if (lua_gettop(L) != 1)
        luaL_error(L, "%s needs exactly 1 argument", wrtype_s[writetype]);
    if (lua_istable(L, 1))
        (void) l_new_image(L);  /* image --- if everything worked well */
    aa = (image **) luaL_checkudata(L, 1, TYPE_IMG);    /* image */
    a = *aa;
    ad = img_dict(a);
    setup_image(static_pdf, a, writetype);
    switch (writetype) {
    case WR_WRITE:
        n = img_to_node(a);
        tail_append(n);
        break;                  /* image */
    case WR_IMMEDIATEWRITE:
        write_img(static_pdf, ad);
        break;                  /* image */
    case WR_NODE:              /* image */
        lua_pop(L, 1);          /* - */
        n = img_to_node(a);
        lua_nodelib_push_fast(L, n);
        break;                  /* node */
    default:
        assert(0);
    }
    if (img_state(ad) < DICT_REFERED)
        img_state(ad) = DICT_REFERED;
}

static int l_write_image(lua_State * L)
{
    write_image_or_node(L, WR_WRITE);
    return 1;                   /* image */
}

static int l_immediatewrite_image(lua_State * L)
{
    check_o_mode(static_pdf, "img.immediatewrite()", 1 << OMODE_PDF, true);
    if (global_shipping_mode != NOT_SHIPPING) {
        luaL_error(L, "pdf.immediatewrite() can not be used with \\latelua");
    } else {
        write_image_or_node(L, WR_IMMEDIATEWRITE);
    }
    return 1;                   /* image */
}

static int l_image_node(lua_State * L)
{
    write_image_or_node(L, WR_NODE);
    return 1;                   /* node */
}

static int l_image_keys(lua_State * L)
{
    const parm_struct *p = img_parms + 1;
    if (lua_gettop(L) != 0)
        luaL_error(L, "img.keys() goes without argument");
    lua_newtable(L);            /* t */
    for (; p->name != NULL; p++) {
        lua_pushinteger(L, (int) p->idx);       /* k t */
        lua_pushstring(L, p->name);     /* v k t */
        lua_settable(L, -3);    /* t */
    }
    return 1;
}

static int l_image_types(lua_State * L)
{
    int i;
    const char **p;
    if (lua_gettop(L) != 0)
        luaL_error(L, "img.types() goes without argument");
    lua_newtable(L);            /* t */
    for (i = 1, p = (const char **) (imgtype_s + 1); *p != NULL; p++, i++) {
        lua_pushinteger(L, (int) i);    /* k t */
        lua_pushstring(L, *p);  /* v k t */
        lua_settable(L, -3);    /* t */
    }
    return 1;
}

static int l_image_boxes(lua_State * L)
{
    int i;
    const char **p;
    if (lua_gettop(L) != 0)
        luaL_error(L, "img.boxes() goes without argument");
    lua_newtable(L);            /* t */
    for (i = 1, p = (const char **) (pdfboxspec_s + 1); *p != NULL; p++, i++) {
        lua_pushinteger(L, (int) i);    /* k t */
        lua_pushstring(L, *p);  /* v k t */
        lua_settable(L, -3);    /* t */
    }
    return 1;
}

static const struct luaL_Reg imglib_f[] = {
    {"new", l_new_image},
    {"copy", l_copy_image},
    {"scan", l_scan_image},
    {"write", l_write_image},
    {"immediatewrite", l_immediatewrite_image},
    {"node", l_image_node},
    {"keys", l_image_keys},
    {"types", l_image_types},
    {"boxes", l_image_boxes},
    {NULL, NULL}                /* sentinel */
};

/**********************************************************************/

void vf_out_image(PDF pdf, unsigned i)
{
    image *a, **aa;
    image_dict *ad;
    lua_State *L = Luas;        /* ... */
    lua_rawgeti(L, LUA_REGISTRYINDEX, (int) i); /* image ... */
    aa = (image **) luaL_checkudata(L, -1, TYPE_IMG);
    a = *aa;
    ad = img_dict(a);
    assert(ad != NULL);
    setup_image(pdf, a, WR_VF_IMG);     /* image ... */
    place_img(pdf, ad, img_dimen(a), img_transform(a));
    lua_pop(L, 1);              /* ... */
}

/**********************************************************************/
/* Metamethods for image */

static int m_img_get(lua_State * L)
{
    image **aa = (image **) luaL_checkudata(L, 1, TYPE_IMG);    /* k u */
    image_to_lua(L, *aa);       /* v u */
    return 1;
}

static int m_img_set(lua_State * L)
{
    image **aa = (image **) luaL_checkudata(L, 1, TYPE_IMG);    /* value key user */
    lua_to_image(L, *aa);       /* v k u */
    return 0;
}

static int m_img_mul(lua_State * L)
{
    lua_Number scale;
    if (lua_type(L, 1) == LUA_TNUMBER) {   /* u? n */
        (void) luaL_checkudata(L, 2, TYPE_IMG); /* u n */
        lua_insert(L, -2);      /* n a */
    } else if (lua_type(L, 2) != LUA_TNUMBER) {    /* n u? */
        (void) luaL_checkudata(L, 1, TYPE_IMG); /* n a */
    }                           /* n a */
    scale = lua_tonumber(L, 2); /* n a */
    lua_pop(L, 1);              /* a */
    copy_image(L, scale);       /* b */
    return 1;
}

static int m_img_print(lua_State * L)
{
    image **aa;
    image_dict *d;
    aa = (image **) luaL_checkudata(L, 1, TYPE_IMG);
    d = img_dict(*aa);
    if (img_pagename(d) != NULL && strlen(img_pagename(d)) != 0)
        lua_pushfstring(L, "<img{filename=\"%s\", page=\"%s\"}>", img_filename(d), img_pagename(d));
    else
        lua_pushfstring(L, "<img{filename=\"%s\", page=%d}>", img_filename(d), img_pagenum(d));
    return 1;
}

static int m_img_gc(lua_State * L)
{
    image *a, **aa;
    image_dict *d;
    aa = (image **) luaL_checkudata(L, 1, TYPE_IMG);
    a = *aa;
    d = img_dict(*aa);
#ifdef DEBUG
    printf("\n===== IMG GC ===== a=%d ad=%d\n", a, img_dict(a));
#endif
    luaL_unref(L, LUA_REGISTRYINDEX, img_dictref(a));
    if (!img_is_refered(d))
        xfree(a);
    return 0;
}

static const struct luaL_Reg img_m[] = {
    {"__index", m_img_get},
    {"__newindex", m_img_set},
    {"__mul", m_img_mul},
    {"__tostring", m_img_print},
    {"__gc", m_img_gc},         /* finalizer */
    {NULL, NULL}                /* sentinel */
};

/**********************************************************************/
/* Metamethods for image_dict */

static int m_img_dict_gc(lua_State * L)
{
    image_dict *ad, **add;
    add = (image_dict **) luaL_checkudata(L, 1, TYPE_IMG_DICT);
    ad = *add;
#ifdef DEBUG
    printf("\n===== IMG_DICT GC FREE ===== ad=%d\n", ad);
#endif
    if (img_state(ad) < DICT_REFERED)
        free_image_dict(ad);
    return 0;
}

static const struct luaL_Reg img_dict_m[] = {
    {"__gc", m_img_dict_gc},    /* finalizer */
    {NULL, NULL}                /* sentinel */
};

/**********************************************************************/

int luaopen_img(lua_State * L)
{
    preset_environment(L, img_parms, IMG_ENV);
    luaL_newmetatable(L, TYPE_IMG);
#ifdef LuajitTeX
    luaL_register(L, NULL, img_m);
    luaL_newmetatable(L, TYPE_IMG_DICT);
    luaL_register(L, NULL, img_dict_m);
    luaL_register(L, "img", imglib_f);
#else
    luaL_setfuncs(L, img_m, 0);
    luaL_newmetatable(L, TYPE_IMG_DICT);
    luaL_setfuncs(L, img_dict_m, 0);
    luaL_newlib(L, imglib_f);
#endif
    return 1;
}

/**********************************************************************/
