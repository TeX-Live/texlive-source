/* lepdflib.cc

   Copyright 2009-2012 Taco Hoekwater <taco@luatex.org>
   Copyright 2009-2012 Hartmut Henkel <hartmut@luatex.org>

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
    "$Id: lepdflib.cc 4415 2012-05-09 19:56:06Z oneiros $ "
    "$URL: http://foundry.supelec.fr/svn/luatex/trunk/source/texk/web2c/luatexdir/lua/lepdflib.cc $";

#include "image/epdf.h"

// define DEBUG

//**********************************************************************
// TODO: add more poppler functions (many are still missing)

//**********************************************************************
// objects allocated by poppler may not be deleted in the lepdflib

typedef enum { ALLOC_POPPLER, ALLOC_LEPDF } alloctype;

typedef struct {
    void *d;
    alloctype atype;            // was it allocated by poppler or the lepdflib.cc?
    PdfDocument *pd;            // reference to PdfDocument, or NULL
    unsigned long pc;           // counter to detect PDFDoc change
} udstruct;

static const char *ErrorCodeNames[] = { "None", "OpenFile", "BadCatalog",
    "Damaged", "Encrypted", "HighlightFile", "BadPrinter", "Printing",
    "Permission", "BadPageNum", "FileIO", NULL
};

//**********************************************************************

#define M_Annot            "Annot"
#define M_Annots           "Annots"
#define M_Array            "Array"
#define M_Catalog          "Catalog"
#define M_Dict             "Dict"
#define M_GooString        "GooString"
#define M_LinkDest         "LinkDest"
#define M_Link             "Link"
#define M_Links            "Links"
#define M_Object           "Object"
#define M_Page             "Page"
#define M_PDFDoc           "PDFDoc"
#define M_PDFRectangle     "PDFRectangle"
#define M_Ref              "Ref"
#define M_Stream           "Stream"
#define M_XRefEntry        "XRefEntry"
#define M_XRef             "XRef"

//**********************************************************************

#define new_poppler_userdata(type)                                              \
static udstruct *new_##type##_userdata(lua_State * L)                           \
{                                                                               \
    udstruct *a;                                                                \
    a = (udstruct *) lua_newuserdata(L, sizeof(udstruct));  /* udstruct ... */  \
    a->atype = ALLOC_POPPLER;                                                   \
    luaL_getmetatable(L, M_##type);     /* m udstruct ... */                    \
    lua_setmetatable(L, -2);    /* udstruct ... */                              \
    return a;                                                                   \
}

new_poppler_userdata(PDFDoc);

new_poppler_userdata(Annot);
new_poppler_userdata(Array);
new_poppler_userdata(Catalog);
new_poppler_userdata(Dict);
new_poppler_userdata(LinkDest);
new_poppler_userdata(Links);
new_poppler_userdata(Object);
new_poppler_userdata(Page);
new_poppler_userdata(PDFRectangle);
new_poppler_userdata(Ref);
new_poppler_userdata(Stream);
new_poppler_userdata(XRef);

//**********************************************************************

static void pdfdoc_changed_error(lua_State * L)
{
    luaL_error(L, "PDFDoc changed or gone");
}

static void pdfdoc_differs_error(lua_State * L)
{
    luaL_error(L, "PDFDoc differs between arguments");
}

//**********************************************************************

static int l_open_PDFDoc(lua_State * L)
{
    const char *file_path;
    udstruct *uout;
    PdfDocument *d;
    file_path = luaL_checkstring(L, 1); // path
    d = refPdfDocument((char *) file_path, FE_RETURN_NULL);
    if (d == NULL)
        lua_pushnil(L);
    else {
        uout = new_PDFDoc_userdata(L);
        uout->d = d;
        uout->atype = ALLOC_LEPDF;
        uout->pc = d->pc;
        uout->pd = d;
    }
    return 1;                   // doc path
}

static int l_new_Array(lua_State * L)
{
    udstruct *uxref, *uout;
    uxref = (udstruct *) luaL_checkudata(L, 1, M_XRef);
    if (uxref->pd != NULL && uxref->pd->pc != uxref->pc)
        pdfdoc_changed_error(L);
    uout = new_Array_userdata(L);
    uout->d = new Array((XRef *) uxref->d);     // automatic init to length 0
    uout->atype = ALLOC_LEPDF;
    uout->pc = uxref->pc;
    uout->pd = uxref->pd;
    return 1;
}

static int l_new_Dict(lua_State * L)
{
    udstruct *uxref, *uout;
    uxref = (udstruct *) luaL_checkudata(L, 1, M_XRef);
    if (uxref->pd != NULL && uxref->pd->pc != uxref->pc)
        pdfdoc_changed_error(L);
    uout = new_Dict_userdata(L);
    uout->d = new Dict((XRef *) uxref->d);      // automatic init to length 0
    uout->atype = ALLOC_LEPDF;
    uout->pc = uxref->pc;
    uout->pd = uxref->pd;
    return 1;
}

static int l_new_Object(lua_State * L)
{
    udstruct *uout;
    uout = new_Object_userdata(L);
    uout->d = new Object();     // automatic init to type "none"
    uout->atype = ALLOC_LEPDF;
    uout->pc = 0;
    uout->pd = NULL;            // not connected to any PDFDoc
    return 1;
}

// PDFRectangle see Page.h

static int l_new_PDFRectangle(lua_State * L)
{
    udstruct *uout;
    uout = new_PDFRectangle_userdata(L);
    uout->d = new PDFRectangle();       // automatic init to [0, 0, 0, 0]
    uout->atype = ALLOC_LEPDF;
    uout->pc = 0;
    uout->pd = NULL;
    return 1;
}

static const struct luaL_Reg epdflib[] = {
    {"open", l_open_PDFDoc},
    {"Array", l_new_Array},
    {"Dict", l_new_Dict},
    {"Object", l_new_Object},
    {"PDFRectangle", l_new_PDFRectangle},
    {NULL, NULL}                // sentinel
};

//**********************************************************************

#define m_poppler_get_poppler(in, out, function)               \
static int m_##in##_##function(lua_State * L)                  \
{                                                              \
    out *o;                                                    \
    udstruct *uin, *uout;                                      \
    uin = (udstruct *) luaL_checkudata(L, 1, M_##in);          \
    if (uin->pd != NULL && uin->pd->pc != uin->pc)             \
        pdfdoc_changed_error(L);                               \
    o = ((in *) uin->d)->function();                           \
    if (o != NULL) {                                           \
        uout = new_##out##_userdata(L);                        \
        uout->d = o;                                           \
        uout->pc = uin->pc;                                    \
        uout->pd = uin->pd;                                    \
    } else                                                     \
        lua_pushnil(L);                                        \
    return 1;                                                  \
}

#define m_poppler_get_BOOL(in, function)                       \
static int m_##in##_##function(lua_State * L)                  \
{                                                              \
    udstruct *uin;                                             \
    uin = (udstruct *) luaL_checkudata(L, 1, M_##in);          \
    if (uin->pd != NULL && uin->pd->pc != uin->pc)             \
        pdfdoc_changed_error(L);                               \
    if (((in *) uin->d)->function())                           \
        lua_pushboolean(L, 1);                                 \
    else                                                       \
        lua_pushboolean(L, 0);                                 \
    return 1;                                                  \
}

#define m_poppler_get_INT(in, function)                        \
static int m_##in##_##function(lua_State * L)                  \
{                                                              \
    int i;                                                     \
    udstruct *uin;                                             \
    uin = (udstruct *) luaL_checkudata(L, 1, M_##in);          \
    if (uin->pd != NULL && uin->pd->pc != uin->pc)             \
        pdfdoc_changed_error(L);                               \
    i = (int) ((in *) uin->d)->function();                     \
    lua_pushinteger(L, i);                                     \
    return 1;                                                  \
}

#define m_poppler_get_DOUBLE(in, function)                     \
static int m_##in##_##function(lua_State * L)                  \
{                                                              \
    double d;                                                  \
    udstruct *uin;                                             \
    uin = (udstruct *) luaL_checkudata(L, 1, M_##in);          \
    if (uin->pd != NULL && uin->pd->pc != uin->pc)             \
        pdfdoc_changed_error(L);                               \
    d = (double) ((in *) uin->d)->function();                  \
    lua_pushnumber(L, d);                                      \
    return 1;                                                  \
}

#define m_poppler_get_GOOSTRING(in, function)                  \
static int m_##in##_##function(lua_State * L)                  \
{                                                              \
    GooString *gs;                                             \
    udstruct *uin;                                             \
    uin = (udstruct *) luaL_checkudata(L, 1, M_##in);          \
    if (uin->pd != NULL && uin->pd->pc != uin->pc)             \
        pdfdoc_changed_error(L);                               \
    gs = ((in *) uin->d)->function();                          \
    if (gs != NULL)                                            \
        lua_pushlstring(L, gs->getCString(), gs->getLength()); \
    else                                                       \
        lua_pushnil(L);                                        \
    return 1;                                                  \
}

#define m_poppler_get_OBJECT(in, function)                     \
static int m_##in##_##function(lua_State * L)                  \
{                                                              \
    udstruct *uin, *uout;                                      \
    uin = (udstruct *) luaL_checkudata(L, 1, M_##in);          \
    if (uin->pd != NULL && uin->pd->pc != uin->pc)             \
        pdfdoc_changed_error(L);                               \
    uout = new_Object_userdata(L);                             \
    uout->d = new Object();                                    \
    ((in *) uin->d)->function((Object *) uout->d);             \
    uout->atype = ALLOC_LEPDF;                                 \
    uout->pc = uin->pc;                                        \
    uout->pd = uin->pd;                                        \
    return 1;                                                  \
}

#define m_poppler_do(in, function)                             \
static int m_##in##_##function(lua_State * L)                  \
{                                                              \
    udstruct *uin;                                             \
    uin = (udstruct *) luaL_checkudata(L, 1, M_##in);          \
    if (uin->pd != NULL && uin->pd->pc != uin->pc)             \
        pdfdoc_changed_error(L);                               \
    ((in *) uin->d)->function();                               \
    return 0;                                                  \
}

#define m_poppler__tostring(type)                              \
static int m_##type##__tostring(lua_State * L)                 \
{                                                              \
    udstruct *uin;                                             \
    uin = (udstruct *) luaL_checkudata(L, 1, M_##type);        \
    if (uin->pd != NULL && uin->pd->pc != uin->pc)             \
        pdfdoc_changed_error(L);                               \
    lua_pushfstring(L, "%s: %p", #type, (type *) uin->d);      \
    return 1;                                                  \
}

#define m_poppler_check_string(in, function)                   \
static int m_##in##_##function(lua_State * L)                  \
{                                                              \
    const char *s;                                             \
    udstruct *uin;                                             \
    uin = (udstruct *) luaL_checkudata(L, 1, M_##in);          \
    if (uin->pd != NULL && uin->pd->pc != uin->pc)             \
        pdfdoc_changed_error(L);                               \
    s = luaL_checkstring(L, 2);                                \
    if (((in *) uin->d)->function((char *) s))                 \
        lua_pushboolean(L, 1);                                 \
    else                                                       \
        lua_pushboolean(L, 0);                                 \
    return 1;                                                  \
}

//**********************************************************************
// Annot

m_poppler_get_BOOL(Annot, isOk);

static int m_Annot_match(lua_State * L)
{
    udstruct *uin, *uref;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Annot);
    uref = (udstruct *) luaL_checkudata(L, 2, M_Ref);
    if (uin->pd != NULL && uref->pd != NULL && uin->pd != uref->pd)
        pdfdoc_differs_error(L);
    if ((uin->pd != NULL && uin->pd->pc != uin->pc)
        || (uref->pd != NULL && uref->pd->pc != uref->pc))
        pdfdoc_changed_error(L);
    lua_pushboolean(L, ((Annot *) uin->d)->match((Ref *) uref->d));
    return 1;
}

m_poppler__tostring(Annot);

static int m_Annot__gc(lua_State * L)
{
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Annot);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
#ifdef DEBUG
    printf("\n===== Annot GC ===== uin=<%p>\n", uin);
#endif
    if (uin->atype == ALLOC_LEPDF)
#ifdef HAVE_ANNOTDECREFCNT
        ((Annot *) uin->d)->decRefCnt();
#else
        delete(Annot *) uin->d;
#endif
    return 0;
}

static const struct luaL_Reg Annot_m[] = {
    {"isOk", m_Annot_isOk},
    {"match", m_Annot_match},
    {"__tostring", m_Annot__tostring},
    {"__gc", m_Annot__gc},
    {NULL, NULL}                // sentinel
};

//**********************************************************************
// Annots

m_poppler_get_INT(Annots, getNumAnnots);

static int m_Annots_getAnnot(lua_State * L)
{
    int i, annots;
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Annots);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = luaL_checkint(L, 2);
    annots = ((Annots *) uin->d)->getNumAnnots();
    if (i > 0 && i <= annots) {
        uout = new_Annot_userdata(L);
        uout->d = ((Annots *) uin->d)->getAnnot(i);
        uout->pc = uin->pc;
        uout->pd = uin->pd;
    } else
        lua_pushnil(L);
    return 1;
}

m_poppler__tostring(Annots);

static const struct luaL_Reg Annots_m[] = {
    {"getNumAnnots", m_Annots_getNumAnnots},
    {"getAnnot", m_Annots_getAnnot},
    {"__tostring", m_Annots__tostring},
    {NULL, NULL}                // sentinel
};

//**********************************************************************
// Array

static int m_Array_incRef(lua_State * L)
{
    int i;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Array);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = ((Array *) uin->d)->incRef();
    lua_pushinteger(L, i);
    return 1;
}

static int m_Array_decRef(lua_State * L)
{
    int i;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Array);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = ((Array *) uin->d)->decRef();
    lua_pushinteger(L, i);
    return 1;
}

m_poppler_get_INT(Array, getLength);

static int m_Array_add(lua_State * L)
{
    udstruct *uin, *uobj;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Array);
    uobj = (udstruct *) luaL_checkudata(L, 2, M_Object);
    if (uin->pd != NULL && uobj->pd != NULL && uin->pd != uobj->pd)
        pdfdoc_differs_error(L);
    if ((uin->pd != NULL && uin->pd->pc != uin->pc)
        || (uobj->pd != NULL && uobj->pd->pc != uobj->pc))
        pdfdoc_changed_error(L);
    ((Array *) uin->d)->add(((Object *) uobj->d));
    return 0;
}

static int m_Array_get(lua_State * L)
{
    int i, len;
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Array);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = luaL_checkint(L, 2);
    len = ((Array *) uin->d)->getLength();
    if (i > 0 && i <= len) {
        uout = new_Object_userdata(L);
        uout->d = new Object();
        ((Array *) uin->d)->get(i - 1, (Object *) uout->d);
        uout->atype = ALLOC_LEPDF;
        uout->pc = uin->pc;
        uout->pd = uin->pd;
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Array_getNF(lua_State * L)
{
    int i, len;
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Array);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = luaL_checkint(L, 2);
    len = ((Array *) uin->d)->getLength();
    if (i > 0 && i <= len) {
        uout = new_Object_userdata(L);
        uout->d = new Object();
        ((Array *) uin->d)->getNF(i - 1, (Object *) uout->d);
        uout->atype = ALLOC_LEPDF;
        uout->pc = uin->pc;
        uout->pd = uin->pd;
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Array_getString(lua_State * L)
{
    GooString *gs;
    int i, len;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Array);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = luaL_checkint(L, 2);
    len = ((Array *) uin->d)->getLength();
    if (i > 0 && i <= len) {
        gs = new GooString();
        if (((Array *) uin->d)->getString(i - 1, gs))
            lua_pushlstring(L, gs->getCString(), gs->getLength());
        else
            lua_pushnil(L);
        delete gs;
    } else
        lua_pushnil(L);
    return 1;
}

m_poppler__tostring(Array);

static const struct luaL_Reg Array_m[] = {
    {"incRef", m_Array_incRef},
    {"decRef", m_Array_decRef},
    {"getLength", m_Array_getLength},
    {"add", m_Array_add},
    {"get", m_Array_get},
    {"getNF", m_Array_getNF},
    {"getString", m_Array_getString},
    {"__tostring", m_Array__tostring},
    {NULL, NULL}                // sentinel
};

//**********************************************************************
// Catalog

m_poppler_get_BOOL(Catalog, isOk);
m_poppler_get_INT(Catalog, getNumPages);

static int m_Catalog_getPage(lua_State * L)
{
    int i, pages;
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Catalog);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = luaL_checkint(L, 2);
    pages = ((Catalog *) uin->d)->getNumPages();
    if (i > 0 && i <= pages) {
        uout = new_Page_userdata(L);
        uout->d = ((Catalog *) uin->d)->getPage(i);
        uout->pc = uin->pc;
        uout->pd = uin->pd;
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Catalog_getPageRef(lua_State * L)
{
    int i, pages;
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Catalog);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = luaL_checkint(L, 2);
    pages = ((Catalog *) uin->d)->getNumPages();
    if (i > 0 && i <= pages) {
        uout = new_Ref_userdata(L);
        uout->d = (Ref *) gmalloc(sizeof(Ref));
        ((Ref *) uout->d)->num = ((Catalog *) uin->d)->getPageRef(i)->num;
        ((Ref *) uout->d)->gen = ((Catalog *) uin->d)->getPageRef(i)->gen;
        uout->atype = ALLOC_LEPDF;
        uout->pc = uin->pc;
        uout->pd = uin->pd;
    } else
        lua_pushnil(L);
    return 1;
}

m_poppler_get_GOOSTRING(Catalog, getBaseURI);
m_poppler_get_GOOSTRING(Catalog, readMetadata);
m_poppler_get_poppler(Catalog, Object, getStructTreeRoot);

static int m_Catalog_findPage(lua_State * L)
{
    int num, gen, i;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Catalog);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    num = luaL_checkint(L, 2);
    gen = luaL_checkint(L, 3);
    i = ((Catalog *) uin->d)->findPage(num, gen);
    if (i > 0)
        lua_pushinteger(L, i);
    else
        lua_pushnil(L);
    return 1;
}

static int m_Catalog_findDest(lua_State * L)
{
    GooString *name;
    LinkDest *dest;
    const char *s;
    size_t len;
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Catalog);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    s = luaL_checklstring(L, 2, &len);
    name = new GooString(s, len);
    dest = ((Catalog *) uin->d)->findDest(name);
    if (dest != NULL) {
        uout = new_LinkDest_userdata(L);
        uout->d = dest;
        uout->pc = uin->pc;
        uout->pd = uin->pd;
    } else
        lua_pushnil(L);
    delete name;
    return 1;
}

m_poppler_get_poppler(Catalog, Object, getDests);
m_poppler_get_INT(Catalog, numEmbeddedFiles);

m_poppler_get_INT(Catalog, numJS);

static int m_Catalog_getJS(lua_State * L)
{
    GooString *gs;
    int i, len;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Catalog);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = luaL_checkint(L, 2);
    len = ((Catalog *) uin->d)->numJS();
    if (i > 0 && i <= len) {
        gs = ((Catalog *) uin->d)->getJS(i - 1);
        if (gs != NULL)
            lua_pushlstring(L, gs->getCString(), gs->getLength());
        else
            lua_pushnil(L);
        delete gs;
    } else
        lua_pushnil(L);
    return 1;
}

m_poppler_get_poppler(Catalog, Object, getOutline);
m_poppler_get_poppler(Catalog, Object, getAcroForm);

m_poppler__tostring(Catalog);

static const struct luaL_Reg Catalog_m[] = {
    {"isOk", m_Catalog_isOk},
    {"getNumPages", m_Catalog_getNumPages},
    {"getPage", m_Catalog_getPage},
    {"getPageRef", m_Catalog_getPageRef},
    {"getBaseURI", m_Catalog_getBaseURI},
    {"readMetadata", m_Catalog_readMetadata},
    {"getStructTreeRoot", m_Catalog_getStructTreeRoot},
    {"findPage", m_Catalog_findPage},
    {"findDest", m_Catalog_findDest},
    {"getDests", m_Catalog_getDests},
    {"numEmbeddedFiles", m_Catalog_numEmbeddedFiles},
    {"numJS", m_Catalog_numJS},
    {"getJS", m_Catalog_getJS},
    {"getOutline", m_Catalog_getOutline},
    {"getAcroForm", m_Catalog_getAcroForm},
    {"__tostring", m_Catalog__tostring},
    {NULL, NULL}                // sentinel
};

//**********************************************************************
// Dict

static int m_Dict_incRef(lua_State * L)
{
    int i;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Dict);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = ((Dict *) uin->d)->incRef();
    lua_pushinteger(L, i);
    return 1;
}

static int m_Dict_decRef(lua_State * L)
{
    int i;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Dict);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = ((Dict *) uin->d)->decRef();
    lua_pushinteger(L, i);
    return 1;
}

m_poppler_get_INT(Dict, getLength);

static int m_Dict_add(lua_State * L)
{
    char *s;
    udstruct *uin, *uobj;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Dict);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    s = copyString((char *) luaL_checkstring(L, 2));
    uobj = (udstruct *) luaL_checkudata(L, 3, M_Object);
    ((Dict *) uin->d)->add(s, ((Object *) uobj->d));
    return 0;
}

static int m_Dict_set(lua_State * L)
{
    char *s;
    udstruct *uin, *uobj;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Dict);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    s = (char *) luaL_checkstring(L, 2);
    uobj = (udstruct *) luaL_checkudata(L, 3, M_Object);
    ((Dict *) uin->d)->set(s, ((Object *) uobj->d));
    return 0;
}

static int m_Dict_remove(lua_State * L)
{
    char *s;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Dict);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    s = (char *) luaL_checkstring(L, 2);
    ((Dict *) uin->d)->remove(s);
    return 0;
}

m_poppler_check_string(Dict, is);

static int m_Dict_lookup(lua_State * L)
{
    const char *s;
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Dict);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    s = luaL_checkstring(L, 2);
    uout = new_Object_userdata(L);
    uout->d = new Object();
    ((Dict *) uin->d)->lookup((char *) s, (Object *) uout->d);
    uout->atype = ALLOC_LEPDF;
    uout->pc = uin->pc;
    uout->pd = uin->pd;
    return 1;
}

static int m_Dict_lookupNF(lua_State * L)
{
    const char *s;
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Dict);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    s = luaL_checkstring(L, 2);
    uout = new_Object_userdata(L);
    uout->d = new Object();
    ((Dict *) uin->d)->lookupNF((char *) s, (Object *) uout->d);
    uout->atype = ALLOC_LEPDF;
    uout->pc = uin->pc;
    uout->pd = uin->pd;
    return 1;
}

static int m_Dict_lookupInt(lua_State * L)
{
    const char *s1, *s2;
    int i;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Dict);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    s1 = luaL_checkstring(L, 2);
    s2 = luaL_checkstring(L, 3);
    if (((Dict *) uin->d)->lookupInt(s1, s2, &i))
        lua_pushinteger(L, i);
    else
        lua_pushnil(L);
    return 1;
}

static int m_Dict_getKey(lua_State * L)
{
    int i, len;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Dict);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = luaL_checkint(L, 2);
    len = ((Dict *) uin->d)->getLength();
    if (i > 0 && i <= len)
        lua_pushstring(L, ((Dict *) uin->d)->getKey(i - 1));
    else
        lua_pushnil(L);
    return 1;
}

static int m_Dict_getVal(lua_State * L)
{
    int i, len;
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Dict);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = luaL_checkint(L, 2);
    len = ((Dict *) uin->d)->getLength();
    if (i > 0 && i <= len) {
        uout = new_Object_userdata(L);
        uout->d = new Object();
        ((Dict *) uin->d)->getVal(i - 1, (Object *) uout->d);
        uout->atype = ALLOC_LEPDF;
        uout->pc = uin->pc;
        uout->pd = uin->pd;
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Dict_getValNF(lua_State * L)
{
    int i, len;
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Dict);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = luaL_checkint(L, 2);
    len = ((Dict *) uin->d)->getLength();
    if (i > 0 && i <= len) {
        uout = new_Object_userdata(L);
        uout->d = new Object();
        ((Dict *) uin->d)->getValNF(i - 1, (Object *) uout->d);
        uout->atype = ALLOC_LEPDF;
        uout->pc = uin->pc;
        uout->pd = uin->pd;
    } else
        lua_pushnil(L);
    return 1;
}

m_poppler_check_string(Dict, hasKey);

m_poppler__tostring(Dict);

static const struct luaL_Reg Dict_m[] = {
    {"incRef", m_Dict_incRef},
    {"decRef", m_Dict_decRef},
    {"getLength", m_Dict_getLength},
    {"add", m_Dict_add},
    {"set", m_Dict_set},
    {"remove", m_Dict_remove},
    {"is", m_Dict_is},
    {"lookup", m_Dict_lookup},
    {"lookupNF", m_Dict_lookupNF},
    {"lookupInt", m_Dict_lookupInt},
    {"getKey", m_Dict_getKey},
    {"getVal", m_Dict_getVal},
    {"getValNF", m_Dict_getValNF},
    {"hasKey", m_Dict_hasKey},
    {"__tostring", m_Dict__tostring},
    {NULL, NULL}                // sentinel
};

//**********************************************************************
// GooString

static int m_GooString__tostring(lua_State * L)
{
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_GooString);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    lua_pushlstring(L, ((GooString *) uin->d)->getCString(),
                    ((GooString *) uin->d)->getLength());
    return 1;
}

static const struct luaL_Reg GooString_m[] = {
    {"__tostring", m_GooString__tostring},
    {NULL, NULL}                // sentinel
};

//**********************************************************************
// LinkDest

static const char *LinkDestKindNames[] =
    { "XYZ", "Fit", "FitH", "FitV", "FitR", "FitB", "FitBH", "FitBV", NULL };

m_poppler_get_BOOL(LinkDest, isOk);

static int m_LinkDest_getKind(lua_State * L)
{
    int i;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_LinkDest);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = (int) ((LinkDest *) uin->d)->getKind();
    lua_pushinteger(L, i);
    return 1;
}

static int m_LinkDest_getKindName(lua_State * L)
{
    int i;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_LinkDest);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = (int) ((LinkDest *) uin->d)->getKind();
    lua_pushstring(L, LinkDestKindNames[i]);
    return 1;
}

m_poppler_get_BOOL(LinkDest, isPageRef);
m_poppler_get_INT(LinkDest, getPageNum);

static int m_LinkDest_getPageRef(lua_State * L)
{
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_LinkDest);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    uout = new_Ref_userdata(L);
    uout->d = (Ref *) gmalloc(sizeof(Ref));
    ((Ref *) uout->d)->num = ((LinkDest *) uin->d)->getPageRef().num;
    ((Ref *) uout->d)->gen = ((LinkDest *) uin->d)->getPageRef().gen;
    uout->atype = ALLOC_LEPDF;
    uout->pc = uin->pc;
    uout->pd = uin->pd;
    return 1;
}

m_poppler_get_DOUBLE(LinkDest, getLeft);
m_poppler_get_DOUBLE(LinkDest, getBottom);
m_poppler_get_DOUBLE(LinkDest, getRight);
m_poppler_get_DOUBLE(LinkDest, getTop);
m_poppler_get_DOUBLE(LinkDest, getZoom);
m_poppler_get_BOOL(LinkDest, getChangeLeft);
m_poppler_get_BOOL(LinkDest, getChangeTop);
m_poppler_get_BOOL(LinkDest, getChangeZoom);

m_poppler__tostring(LinkDest);

static const struct luaL_Reg LinkDest_m[] = {
    {"isOk", m_LinkDest_isOk},
    {"getKind", m_LinkDest_getKind},
    {"getKindName", m_LinkDest_getKindName},    // not poppler
    {"isPageRef", m_LinkDest_isPageRef},
    {"getPageNum", m_LinkDest_getPageNum},
    {"getPageRef", m_LinkDest_getPageRef},
    {"getLeft", m_LinkDest_getLeft},
    {"getBottom", m_LinkDest_getBottom},
    {"getRight", m_LinkDest_getRight},
    {"getTop", m_LinkDest_getTop},
    {"getZoom", m_LinkDest_getZoom},
    {"getChangeLeft", m_LinkDest_getChangeLeft},
    {"getChangeTop", m_LinkDest_getChangeTop},
    {"getChangeZoom", m_LinkDest_getChangeZoom},
    {"__tostring", m_LinkDest__tostring},
    {NULL, NULL}                // sentinel
};

//**********************************************************************
// Links

m_poppler_get_INT(Links, getNumLinks);

m_poppler__tostring(Links);

static const struct luaL_Reg Links_m[] = {
    {"getNumLinks", m_Links_getNumLinks},
    //{"getLink", m_Links_getLink},
    {"__tostring", m_Links__tostring},
    {NULL, NULL}                // sentinel
};

//**********************************************************************
// Object

// Special type checking.
#define m_Object_isType(function)                                          \
static int m_Object_##function(lua_State * L)                              \
{                                                                          \
    udstruct *uin;                                                         \
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);                    \
    if (uin->pd != NULL && uin->pd->pc != uin->pc)                         \
        pdfdoc_changed_error(L);                                           \
    if (lua_gettop(L) >= 2) {                                              \
        if (lua_isstring(L, 2)                                             \
            && ((Object *) uin->d)->function((char *) lua_tostring(L, 2))) \
            lua_pushboolean(L, 1);                                         \
        else                                                               \
            lua_pushboolean(L, 0);                                         \
    } else {                                                               \
        if (((Object *) uin->d)->function())                               \
            lua_pushboolean(L, 1);                                         \
        else                                                               \
            lua_pushboolean(L, 0);                                         \
    }                                                                      \
    return 1;                                                              \
}

static int m_Object_initBool(lua_State * L)
{
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    luaL_checktype(L, 2, LUA_TBOOLEAN);
    if (lua_toboolean(L, 2) != 0)
        ((Object *) uin->d)->initBool(gTrue);
    else
        ((Object *) uin->d)->initBool(gFalse);
    return 0;
}

static int m_Object_initInt(lua_State * L)
{
    int i;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = luaL_checkint(L, 2);
    ((Object *) uin->d)->initInt(i);
    return 0;
}

static int m_Object_initReal(lua_State * L)
{
    double d;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    d = luaL_checknumber(L, 2);
    ((Object *) uin->d)->initReal(d);
    return 0;
}

static int m_Object_initString(lua_State * L)
{
    GooString *gs;
    const char *s;
    size_t len;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    s = luaL_checklstring(L, 2, &len);
    gs = new GooString(s, len);
    ((Object *) uin->d)->initString(gs);
    return 0;
}

static int m_Object_initName(lua_State * L)
{
    const char *s;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    s = luaL_checkstring(L, 2);
    ((Object *) uin->d)->initName((char *) s);
    return 0;
}

static int m_Object_initNull(lua_State * L)
{
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    ((Object *) uin->d)->initNull();
    return 0;
}

static int m_Object_initArray(lua_State * L)
{
    udstruct *uin, *uxref;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    uxref = (udstruct *) luaL_checkudata(L, 2, M_XRef);
    if (uin->pd != NULL && uxref->pd != NULL && uin->pd != uxref->pd)
        pdfdoc_differs_error(L);
    if ((uin->pd != NULL && uin->pd->pc != uin->pc)
        || (uxref->pd != NULL && uxref->pd->pc != uxref->pc))
        pdfdoc_changed_error(L);
    ((Object *) uin->d)->initArray((XRef *) uxref->d);
    return 0;
}

// TODO: decide betweeen
//   Object *initDict(XRef *xref);
//   Object *initDict(Dict *dictA);

static int m_Object_initDict(lua_State * L)
{
    udstruct *uin, *uxref;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    uxref = (udstruct *) luaL_checkudata(L, 2, M_XRef);
    if (uin->pd != NULL && uxref->pd != NULL && uin->pd != uxref->pd)
        pdfdoc_differs_error(L);
    if ((uin->pd != NULL && uin->pd->pc != uin->pc)
        || (uxref->pd != NULL && uxref->pd->pc != uxref->pc))
        pdfdoc_changed_error(L);
    ((Object *) uin->d)->initDict((XRef *) uxref->d);
    return 0;
}

static int m_Object_initStream(lua_State * L)
{
    udstruct *uin, *ustream;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    ustream = (udstruct *) luaL_checkudata(L, 2, M_Stream);
    if (uin->pd != NULL && ustream->pd != NULL && uin->pd != ustream->pd)
        pdfdoc_differs_error(L);
    if ((uin->pd != NULL && uin->pd->pc != uin->pc)
        || (ustream->pd != NULL && ustream->pd->pc != ustream->pc))
        pdfdoc_changed_error(L);
    ((Object *) uin->d)->initStream((Stream *) ustream->d);
    return 0;
}

static int m_Object_initRef(lua_State * L)
{
    int num, gen;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    num = luaL_checkint(L, 2);
    gen = luaL_checkint(L, 3);
    ((Object *) uin->d)->initRef(num, gen);
    return 0;
}

static int m_Object_initCmd(lua_State * L)
{
    const char *s;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    s = luaL_checkstring(L, 2);
    ((Object *) uin->d)->initCmd((char *) s);
    return 0;
}

static int m_Object_initError(lua_State * L)
{
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    ((Object *) uin->d)->initError();
    return 0;
}

static int m_Object_initEOF(lua_State * L)
{
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    ((Object *) uin->d)->initEOF();
    return 0;
}

static int m_Object_fetch(lua_State * L)
{
    udstruct *uin, *uxref, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    uxref = (udstruct *) luaL_checkudata(L, 2, M_XRef);
    if (uin->pd != NULL && uxref->pd != NULL && uin->pd != uxref->pd)
        pdfdoc_differs_error(L);
    if ((uin->pd != NULL && uin->pd->pc != uin->pc)
        || (uxref->pd != NULL && uxref->pd->pc != uxref->pc))
        pdfdoc_changed_error(L);
    uout = new_Object_userdata(L);
    uout->d = new Object();
    ((Object *) uin->d)->fetch((XRef *) uxref->d, (Object *) uout->d);
    uout->atype = ALLOC_LEPDF;
    uout->pc = uin->pc;
    uout->pd = uin->pd;
    return 1;
}

static int m_Object_getType(lua_State * L)
{
    ObjType t;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    t = ((Object *) uin->d)->getType();
    lua_pushinteger(L, (int) t);
    return 1;
}

static int m_Object_getTypeName(lua_State * L)
{
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    lua_pushstring(L, ((Object *) uin->d)->getTypeName());
    return 1;
}

m_poppler_get_BOOL(Object, isBool);
m_poppler_get_BOOL(Object, isInt);
m_poppler_get_BOOL(Object, isReal);
m_poppler_get_BOOL(Object, isNum);
m_poppler_get_BOOL(Object, isString);
m_Object_isType(isName);
m_poppler_get_BOOL(Object, isNull);
m_poppler_get_BOOL(Object, isArray);
m_Object_isType(isDict);
m_Object_isType(isStream);
m_poppler_get_BOOL(Object, isRef);
m_Object_isType(isCmd);
m_poppler_get_BOOL(Object, isError);
m_poppler_get_BOOL(Object, isEOF);
m_poppler_get_BOOL(Object, isNone);

static int m_Object_getBool(lua_State * L)
{
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((Object *) uin->d)->isBool()) {
        if (((Object *) uin->d)->getBool())
            lua_pushboolean(L, 1);
        else
            lua_pushboolean(L, 0);
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Object_getInt(lua_State * L)
{
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((Object *) uin->d)->isInt())
        lua_pushnumber(L, ((Object *) uin->d)->getInt());
    else
        lua_pushnil(L);
    return 1;
}

static int m_Object_getReal(lua_State * L)
{
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((Object *) uin->d)->isReal())
        lua_pushnumber(L, ((Object *) uin->d)->getReal());
    else
        lua_pushnil(L);
    return 1;
}

static int m_Object_getNum(lua_State * L)
{
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((Object *) uin->d)->isNum())
        lua_pushnumber(L, ((Object *) uin->d)->getNum());
    else
        lua_pushnil(L);
    return 1;
}

static int m_Object_getString(lua_State * L)
{
    GooString *gs;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((Object *) uin->d)->isString()) {
        gs = ((Object *) uin->d)->getString();
        lua_pushlstring(L, gs->getCString(), gs->getLength());
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Object_getName(lua_State * L)
{
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((Object *) uin->d)->isName())
        lua_pushstring(L, ((Object *) uin->d)->getName());
    else
        lua_pushnil(L);
    return 1;
}

static int m_Object_getArray(lua_State * L)
{
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((Object *) uin->d)->isArray()) {
        uout = new_Array_userdata(L);
        uout->d = ((Object *) uin->d)->getArray();
        uout->pc = uin->pc;
        uout->pd = uin->pd;
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Object_getDict(lua_State * L)
{
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((Object *) uin->d)->isDict()) {
        uout = new_Dict_userdata(L);
        uout->d = ((Object *) uin->d)->getDict();
        uout->pc = uin->pc;
        uout->pd = uin->pd;
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Object_getStream(lua_State * L)
{
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((Object *) uin->d)->isStream()) {
        uout = new_Stream_userdata(L);
        uout->d = ((Object *) uin->d)->getStream();
        uout->pc = uin->pc;
        uout->pd = uin->pd;
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Object_getRef(lua_State * L)
{
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((Object *) uin->d)->isRef()) {
        uout = new_Ref_userdata(L);
        uout->d = (Ref *) gmalloc(sizeof(Ref));
        ((Ref *) uout->d)->num = ((Object *) uin->d)->getRef().num;
        ((Ref *) uout->d)->gen = ((Object *) uin->d)->getRef().gen;
        uout->atype = ALLOC_LEPDF;
        uout->pc = uin->pc;
        uout->pd = uin->pd;
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Object_getRefNum(lua_State * L)
{
    int i;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((Object *) uin->d)->isRef()) {
        i = ((Object *) uin->d)->getRef().num;
        lua_pushinteger(L, i);
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Object_getRefGen(lua_State * L)
{
    int i;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((Object *) uin->d)->isRef()) {
        i = ((Object *) uin->d)->getRef().gen;
        lua_pushinteger(L, i);
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Object_getCmd(lua_State * L)
{
    char *s;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((Object *) uin->d)->isCmd())
        lua_pushstring(L, ((Object *) uin->d)->getCmd());
    else
        lua_pushnil(L);
    return 1;
}

static int m_Object_arrayGetLength(lua_State * L)
{
    int len;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((Object *) uin->d)->isArray()) {
        len = ((Object *) uin->d)->arrayGetLength();
        lua_pushnumber(L, len);
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Object_arrayAdd(lua_State * L)
{
    udstruct *uin, *uobj;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    uobj = (udstruct *) luaL_checkudata(L, 2, M_Object);
    if (uin->pd != NULL && uobj->pd != NULL && uin->pd != uobj->pd)
        pdfdoc_differs_error(L);
    if ((uin->pd != NULL && uin->pd->pc != uin->pc)
        || (uobj->pd != NULL && uobj->pd->pc != uobj->pd->pc))
        pdfdoc_changed_error(L);
    if (!((Object *) uin->d)->isArray())
        luaL_error(L, "Object is not an Array");
    ((Object *) uin->d)->arrayAdd((Object *) uobj->d);
    return 0;
}

static int m_Object_arrayGet(lua_State * L)
{
    int i, len;
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = luaL_checkint(L, 2);
    if (((Object *) uin->d)->isArray()) {
        len = ((Object *) uin->d)->arrayGetLength();
        if (i > 0 && i <= len) {
            uout = new_Object_userdata(L);
            uout->d = new Object();
            ((Object *) uin->d)->arrayGet(i - 1, (Object *) uout->d);
            uout->atype = ALLOC_LEPDF;
            uout->pc = uin->pc;
            uout->pd = uin->pd;
        } else
            lua_pushnil(L);
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Object_arrayGetNF(lua_State * L)
{
    int i, len;
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = luaL_checkint(L, 2);
    if (((Object *) uin->d)->isArray()) {
        len = ((Object *) uin->d)->arrayGetLength();
        if (i > 0 && i <= len) {
            uout = new_Object_userdata(L);
            uout->d = new Object();
            ((Object *) uin->d)->arrayGetNF(i - 1, (Object *) uout->d);
            uout->atype = ALLOC_LEPDF;
            uout->pc = uin->pc;
            uout->pd = uin->pd;
        } else
            lua_pushnil(L);
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Object_dictGetLength(lua_State * L)
{
    int len;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((Object *) uin->d)->isDict()) {
        len = ((Object *) uin->d)->dictGetLength();
        lua_pushnumber(L, len);
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Object_dictAdd(lua_State * L)
{
    const char *s;
    udstruct *uin, *uobj;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    s = luaL_checkstring(L, 2);
    uobj = (udstruct *) luaL_checkudata(L, 3, M_Object);
    if (uin->pd != NULL && uobj->pd != NULL && uin->pd != uobj->pd)
        pdfdoc_differs_error(L);
    if ((uin->pd != NULL && uin->pd->pc != uin->pc)
        || (uobj->pd != NULL && uobj->pd->pc != uobj->pd->pc))
        pdfdoc_changed_error(L);
    if (!((Object *) uin->d)->isDict())
        luaL_error(L, "Object is not a Dict");
    ((Object *) uin->d)->dictAdd((char *) s, (Object *) uobj->d);
    return 0;
}

static int m_Object_dictSet(lua_State * L)
{
    const char *s;
    udstruct *uin, *uobj;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    s = luaL_checkstring(L, 2);
    uobj = (udstruct *) luaL_checkudata(L, 3, M_Object);
    if (uin->pd != NULL && uobj->pd != NULL && uin->pd != uobj->pd)
        pdfdoc_differs_error(L);
    if ((uin->pd != NULL && uin->pd->pc != uin->pc)
        || (uobj->pd != NULL && uobj->pd->pc != uobj->pd->pc))
        pdfdoc_changed_error(L);
    if (!((Object *) uin->d)->isDict())
        luaL_error(L, "Object is not a Dict");
    ((Object *) uin->d)->dictSet((char *) s, (Object *) uobj->d);
    return 0;
}

static int m_Object_dictLookup(lua_State * L)
{
    const char *s;
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    s = luaL_checkstring(L, 2);
    if (((Object *) uin->d)->isDict()) {
        uout = new_Object_userdata(L);
        uout->d = new Object();
        ((Object *) uin->d)->dictLookup((char *) s, (Object *) uout->d);
        uout->atype = ALLOC_LEPDF;
        uout->pc = uin->pc;
        uout->pd = uin->pd;
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Object_dictLookupNF(lua_State * L)
{
    const char *s;
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    s = luaL_checkstring(L, 2);
    if (((Object *) uin->d)->isDict()) {
        uout = new_Object_userdata(L);
        uout->d = new Object();
        ((Object *) uin->d)->dictLookupNF((char *) s, (Object *) uout->d);
        uout->atype = ALLOC_LEPDF;
        uout->pc = uin->pc;
        uout->pd = uin->pd;
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Object_dictGetKey(lua_State * L)
{
    int i, len;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = luaL_checkint(L, 2);
    if (((Object *) uin->d)->isDict()) {
        len = ((Object *) uin->d)->dictGetLength();
        if (i > 0 && i <= len)
            lua_pushstring(L, ((Object *) uin->d)->dictGetKey(i - 1));
        else
            lua_pushnil(L);
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Object_dictGetVal(lua_State * L)
{
    int i, len;
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = luaL_checkint(L, 2);
    if (((Object *) uin->d)->isDict()) {
        len = ((Object *) uin->d)->dictGetLength();
        if (i > 0 && i <= len) {
            uout = new_Object_userdata(L);
            uout->d = new Object();
            ((Object *) uin->d)->dictGetVal(i - 1, (Object *) uout->d);
            uout->atype = ALLOC_LEPDF;
            uout->pc = uin->pc;
            uout->pd = uin->pd;
        } else
            lua_pushnil(L);
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Object_dictGetValNF(lua_State * L)
{
    int i, len;
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = luaL_checkint(L, 2);
    if (((Object *) uin->d)->isDict()) {
        len = ((Object *) uin->d)->dictGetLength();
        if (i > 0 && i <= len) {
            uout = new_Object_userdata(L);
            uout->d = new Object();
            ((Object *) uin->d)->dictGetValNF(i - 1, (Object *) uout->d);
            uout->atype = ALLOC_LEPDF;
            uout->pc = uin->pc;
            uout->pd = uin->pd;
        } else
            lua_pushnil(L);
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Object_streamIs(lua_State * L)
{
    const char *s;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    s = luaL_checkstring(L, 2);
    if (((Object *) uin->d)->isStream()) {
        if (((Object *) uin->d)->streamIs((char *) s))
            lua_pushboolean(L, 1);
        else
            lua_pushboolean(L, 0);
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Object_streamReset(lua_State * L)
{
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((Object *) uin->d)->isStream())
        ((Object *) uin->d)->streamReset();
    return 0;
}

static int m_Object_streamGetChar(lua_State * L)
{
    int i;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((Object *) uin->d)->isStream()) {
        i = ((Object *) uin->d)->streamGetChar();
        lua_pushinteger(L, i);
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Object_streamLookChar(lua_State * L)
{
    int i;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((Object *) uin->d)->isStream()) {
        i = ((Object *) uin->d)->streamLookChar();
        lua_pushinteger(L, i);
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Object_streamGetPos(lua_State * L)
{
    int i;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((Object *) uin->d)->isStream()) {
        i = (int) ((Object *) uin->d)->streamGetPos();
        lua_pushinteger(L, i);
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Object_streamSetPos(lua_State * L)
{
    int i;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = luaL_checkint(L, 2);
    if (((Object *) uin->d)->isStream())
        ((Object *) uin->d)->streamSetPos(i);
    return 0;
}

static int m_Object_streamGetDict(lua_State * L)
{
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((Object *) uin->d)->isStream()) {
        uout = new_Dict_userdata(L);
        uout->d = ((Object *) uin->d)->streamGetDict();
        uout->pc = uin->pc;
        uout->pd = uin->pd;
    } else
        lua_pushnil(L);
    return 1;
}

static int m_Object__gc(lua_State * L)
{
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Object);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
#ifdef DEBUG
    printf("\n===== Object GC ===== uin=<%p>\n", uin);
#endif
    if (uin->atype == ALLOC_LEPDF)
        delete(Object *) uin->d;
    return 0;
}

m_poppler__tostring(Object);

static const struct luaL_Reg Object_m[] = {
    {"initBool", m_Object_initBool},
    {"initInt", m_Object_initInt},
    {"initReal", m_Object_initReal},
    {"initString", m_Object_initString},
    {"initName", m_Object_initName},
    {"initNull", m_Object_initNull},
    {"initArray", m_Object_initArray},
    {"initDict", m_Object_initDict},
    {"initStream", m_Object_initStream},
    {"initRef", m_Object_initRef},
    {"initCmd", m_Object_initCmd},
    {"initError", m_Object_initError},
    {"initEOF", m_Object_initEOF},
    // {"copy", m_Object_copy},
    {"fetch", m_Object_fetch},
    {"getType", m_Object_getType},
    {"getTypeName", m_Object_getTypeName},
    {"isBool", m_Object_isBool},
    {"isInt", m_Object_isInt},
    {"isReal", m_Object_isReal},
    {"isNum", m_Object_isNum},
    {"isString", m_Object_isString},
    {"isName", m_Object_isName},
    {"isNull", m_Object_isNull},
    {"isArray", m_Object_isArray},
    {"isDict", m_Object_isDict},
    {"isStream", m_Object_isStream},
    {"isRef", m_Object_isRef},
    {"isCmd", m_Object_isCmd},
    {"isError", m_Object_isError},
    {"isEOF", m_Object_isEOF},
    {"isNone", m_Object_isNone},
    {"getBool", m_Object_getBool},
    {"getInt", m_Object_getInt},
    {"getReal", m_Object_getReal},
    {"getNum", m_Object_getNum},
    {"getString", m_Object_getString},
    {"getName", m_Object_getName},
    {"getArray", m_Object_getArray},
    {"getDict", m_Object_getDict},
    {"getStream", m_Object_getStream},
    {"getRef", m_Object_getRef},
    {"getRefNum", m_Object_getRefNum},
    {"getRefGen", m_Object_getRefGen},
    {"getCmd", m_Object_getCmd},
    {"arrayGetLength", m_Object_arrayGetLength},
    {"arrayAdd", m_Object_arrayAdd},
    {"arrayGet", m_Object_arrayGet},
    {"arrayGetNF", m_Object_arrayGetNF},
    {"dictGetLength", m_Object_dictGetLength},
    {"dictAdd", m_Object_dictAdd},
    {"dictSet", m_Object_dictSet},
    {"dictLookup", m_Object_dictLookup},
    {"dictLookupNF", m_Object_dictLookupNF},
    {"dictGetKey", m_Object_dictGetKey},
    {"dictGetVal", m_Object_dictGetVal},
    {"dictGetValNF", m_Object_dictGetValNF},
    {"streamIs", m_Object_streamIs},
    {"streamReset", m_Object_streamReset},
    // {"streamClose", m_Object_streamClose},
    {"streamGetChar", m_Object_streamGetChar},
    {"streamLookChar", m_Object_streamLookChar},
    // {"streamGetLine", m_Object_streamGetLine},
    {"streamGetPos", m_Object_streamGetPos},
    {"streamSetPos", m_Object_streamSetPos},
    {"streamGetDict", m_Object_streamGetDict},
    {"__tostring", m_Object__tostring},
    {"__gc", m_Object__gc},     // finalizer
    {NULL, NULL}                // sentinel
};

//**********************************************************************
// Page

m_poppler_get_BOOL(Page, isOk);
m_poppler_get_INT(Page, getNum);
m_poppler_get_poppler(Page, PDFRectangle, getMediaBox);
m_poppler_get_poppler(Page, PDFRectangle, getCropBox);
m_poppler_get_BOOL(Page, isCropped);
m_poppler_get_DOUBLE(Page, getMediaWidth);
m_poppler_get_DOUBLE(Page, getMediaHeight);
m_poppler_get_DOUBLE(Page, getCropWidth);
m_poppler_get_DOUBLE(Page, getCropHeight);
m_poppler_get_poppler(Page, PDFRectangle, getBleedBox);
m_poppler_get_poppler(Page, PDFRectangle, getTrimBox);
m_poppler_get_poppler(Page, PDFRectangle, getArtBox);
m_poppler_get_INT(Page, getRotate);
m_poppler_get_GOOSTRING(Page, getLastModified);
m_poppler_get_poppler(Page, Dict, getBoxColorInfo);
m_poppler_get_poppler(Page, Dict, getGroup);
m_poppler_get_poppler(Page, Stream, getMetadata);
m_poppler_get_poppler(Page, Dict, getPieceInfo);
m_poppler_get_poppler(Page, Dict, getSeparationInfo);
m_poppler_get_poppler(Page, Dict, getResourceDict);
m_poppler_get_OBJECT(Page, getAnnots);

m_poppler_get_OBJECT(Page, getContents);

m_poppler__tostring(Page);

static const struct luaL_Reg Page_m[] = {
    {"isOk", m_Page_isOk},
    {"getNum", m_Page_getNum},
    {"getMediaBox", m_Page_getMediaBox},
    {"getCropBox", m_Page_getCropBox},
    {"isCropped", m_Page_isCropped},
    {"getMediaWidth", m_Page_getMediaWidth},
    {"getMediaHeight", m_Page_getMediaHeight},
    {"getCropWidth", m_Page_getCropWidth},
    {"getCropHeight", m_Page_getCropHeight},
    {"getBleedBox", m_Page_getBleedBox},
    {"getTrimBox", m_Page_getTrimBox},
    {"getArtBox", m_Page_getArtBox},
    {"getRotate", m_Page_getRotate},
    {"getLastModified", m_Page_getLastModified},
    {"getBoxColorInfo", m_Page_getBoxColorInfo},
    {"getGroup", m_Page_getGroup},
    {"getMetadata", m_Page_getMetadata},
    {"getPieceInfo", m_Page_getPieceInfo},
    {"getSeparationInfo", m_Page_getSeparationInfo},
    {"getResourceDict", m_Page_getResourceDict},
    {"getAnnots", m_Page_getAnnots},
    {"getContents", m_Page_getContents},
    {"__tostring", m_Page__tostring},
    {NULL, NULL}                // sentinel
};

//**********************************************************************
// PDFDoc

#define m_PDFDoc_BOOL(function)                         \
static int m_PDFDoc_##function(lua_State * L)           \
{                                                       \
    udstruct *uin;                                      \
    uin = (udstruct *) luaL_checkudata(L, 1, M_PDFDoc); \
    if (uin->pd != NULL && uin->pd->pc != uin->pc)      \
        pdfdoc_changed_error(L);                        \
    if (((PdfDocument *) uin->d)->doc->function())      \
        lua_pushboolean(L, 1);                          \
    else                                                \
        lua_pushboolean(L, 0);                          \
    return 1;                                           \
}

#define m_PDFDoc_INT(function)                          \
static int m_PDFDoc_##function(lua_State * L)           \
{                                                       \
    int i;                                              \
    udstruct *uin;                                      \
    uin = (udstruct *) luaL_checkudata(L, 1, M_PDFDoc); \
    if (uin->pd != NULL && uin->pd->pc != uin->pc)      \
        pdfdoc_changed_error(L);                        \
    i = ((PdfDocument *) uin->d)->doc->function();      \
    lua_pushinteger(L, i);                              \
    return 1;                                           \
}

m_PDFDoc_BOOL(isOk);
m_PDFDoc_INT(getErrorCode);

static int m_PDFDoc_getFileName(lua_State * L)
{
    GooString *gs;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_PDFDoc);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    gs = ((PdfDocument *) uin->d)->doc->getFileName();
    if (gs != NULL)
        lua_pushlstring(L, gs->getCString(), gs->getLength());
    else
        lua_pushnil(L);
    return 1;
}

static int m_PDFDoc_getErrorCodeName(lua_State * L)
{
    int i;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_PDFDoc);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = ((PdfDocument *) uin->d)->doc->getErrorCode();
    lua_pushstring(L, ErrorCodeNames[i]);
    return 1;
}

static int m_PDFDoc_getXRef(lua_State * L)
{
    XRef *xref;
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_PDFDoc);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    xref = ((PdfDocument *) uin->d)->doc->getXRef();
    if (xref->isOk()) {
        uout = new_XRef_userdata(L);
        uout->d = xref;
        uout->pc = uin->pc;
        uout->pd = uin->pd;
    } else
        lua_pushnil(L);
    return 1;
}

static int m_PDFDoc_getCatalog(lua_State * L)
{
    Catalog *cat;
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_PDFDoc);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    cat = ((PdfDocument *) uin->d)->doc->getCatalog();
    if (cat->isOk()) {
        uout = new_Catalog_userdata(L);
        uout->d = cat;
        uout->pc = uin->pc;
        uout->pd = uin->pd;
    } else
        lua_pushnil(L);
    return 1;
}

#define m_PDFDoc_PAGEDIMEN(function)                             \
static int m_PDFDoc_##function(lua_State * L)                    \
{                                                                \
    int i, pages;                                                \
    double d;                                                    \
    udstruct *uin;                                               \
    uin = (udstruct *) luaL_checkudata(L, 1, M_PDFDoc);          \
    if (uin->pd != NULL && uin->pd->pc != uin->pc)               \
        pdfdoc_changed_error(L);                                 \
    i = luaL_checkint(L, 2);                                     \
    pages = ((PdfDocument *) uin->d)->doc->getNumPages();        \
    if (i > 0 && i <= pages) {                                   \
        d = (double) ((PdfDocument *) uin->d)->doc->function(i); \
        lua_pushnumber(L, d);                                    \
    } else                                                       \
        lua_pushnil(L);                                          \
    return 1;                                                    \
}

m_PDFDoc_PAGEDIMEN(getPageMediaWidth);
m_PDFDoc_PAGEDIMEN(getPageMediaHeight);
m_PDFDoc_PAGEDIMEN(getPageCropWidth);
m_PDFDoc_PAGEDIMEN(getPageCropHeight);
m_PDFDoc_INT(getNumPages);

static int m_PDFDoc_readMetadata(lua_State * L)
{
    GooString *gs;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_PDFDoc);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((PdfDocument *) uin->d)->doc->getCatalog()->isOk()) {
        gs = ((PdfDocument *) uin->d)->doc->readMetadata();
        if (gs != NULL)
            lua_pushlstring(L, gs->getCString(), gs->getLength());
        else
            lua_pushnil(L);
    } else
        lua_pushnil(L);
    return 1;
}

static int m_PDFDoc_getStructTreeRoot(lua_State * L)
{
    Object *obj;
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_PDFDoc);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((PdfDocument *) uin->d)->doc->getCatalog()->isOk()) {
        obj = ((PdfDocument *) uin->d)->doc->getStructTreeRoot();
        uout = new_Object_userdata(L);
        uout->d = obj;
        uout->pc = uin->pc;
        uout->pd = uin->pd;
    } else
        lua_pushnil(L);
    return 1;
}

static int m_PDFDoc_findPage(lua_State * L)
{
    int num, gen, i;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_PDFDoc);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    num = luaL_checkint(L, 2);
    gen = luaL_checkint(L, 3);
    if (((PdfDocument *) uin->d)->doc->getCatalog()->isOk()) {
        i = ((PdfDocument *) uin->d)->doc->findPage(num, gen);
        if (i > 0)
            lua_pushinteger(L, i);
        else
            lua_pushnil(L);
    } else
        lua_pushnil(L);
    return 1;
}

static int m_PDFDoc_getLinks(lua_State * L)
{
    int i, pages;
    Links *links;
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_PDFDoc);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    i = luaL_checkint(L, 2);
    pages = ((PdfDocument *) uin->d)->doc->getNumPages();
    if (i > 0 && i <= pages) {
        links = ((PdfDocument *) uin->d)->doc->getLinks(i);
        if (links != NULL) {
            uout = new_Links_userdata(L);
            uout->d = links;
            uout->pc = uin->pc;
            uout->pd = uin->pd;
        } else
            lua_pushnil(L);
    } else
        lua_pushnil(L);
    return 1;
}

static int m_PDFDoc_findDest(lua_State * L)
{
    GooString *name;
    LinkDest *dest;
    const char *s;
    size_t len;
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_PDFDoc);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    s = luaL_checklstring(L, 2, &len);
    name = new GooString(s, len);
    if (((PdfDocument *) uin->d)->doc->getCatalog()->isOk()) {
        dest = ((PdfDocument *) uin->d)->doc->findDest(name);
        if (dest != NULL) {
            uout = new_LinkDest_userdata(L);
            uout->d = dest;
            uout->pc = uin->pc;
            uout->pd = uin->pd;
        } else
            lua_pushnil(L);
    } else
        lua_pushnil(L);
    delete name;
    return 1;
}

m_PDFDoc_BOOL(isEncrypted);
m_PDFDoc_BOOL(okToPrint);
m_PDFDoc_BOOL(okToChange);
m_PDFDoc_BOOL(okToCopy);
m_PDFDoc_BOOL(okToAddNotes);
m_PDFDoc_BOOL(isLinearized);

static int m_PDFDoc_getDocInfo(lua_State * L)
{
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_PDFDoc);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((PdfDocument *) uin->d)->doc->getXRef()->isOk()) {
        uout = new_Object_userdata(L);
        uout->d = new Object();
        ((PdfDocument *) uin->d)->doc->getDocInfo((Object *) uout->d);
        uout->atype = ALLOC_LEPDF;
        uout->pc = uin->pc;
        uout->pd = uin->pd;
    } else
        lua_pushnil(L);
    return 1;
}

static int m_PDFDoc_getDocInfoNF(lua_State * L)
{
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_PDFDoc);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    if (((PdfDocument *) uin->d)->doc->getXRef()->isOk()) {
        uout = new_Object_userdata(L);
        uout->d = new Object();
        ((PdfDocument *) uin->d)->doc->getDocInfoNF((Object *) uout->d);
        uout->atype = ALLOC_LEPDF;
        uout->pc = uin->pc;
        uout->pd = uin->pd;
    } else
        lua_pushnil(L);
    return 1;
}

m_PDFDoc_INT(getPDFMajorVersion);
m_PDFDoc_INT(getPDFMinorVersion);

m_poppler__tostring(PDFDoc);

static int m_PDFDoc__gc(lua_State * L)
{
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_PDFDoc);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
#ifdef DEBUG
    printf("\n===== PDFDoc GC ===== file_path=<%s>\n",
           ((PdfDocument *) uin->d)->file_path);
#endif
    assert(uin->atype == ALLOC_LEPDF);
    unrefPdfDocument(((PdfDocument *) uin->d)->file_path);
    return 0;
}

static const struct luaL_Reg PDFDoc_m[] = {
    {"isOk", m_PDFDoc_isOk},
    {"getErrorCode", m_PDFDoc_getErrorCode},
    {"getErrorCodeName", m_PDFDoc_getErrorCodeName},    // not poppler
    {"getFileName", m_PDFDoc_getFileName},
    {"getXRef", m_PDFDoc_getXRef},
    {"getCatalog", m_PDFDoc_getCatalog},
    // {"getBaseStream", m_PDFDoc_getBaseStream},
    {"getPageMediaWidth", m_PDFDoc_getPageMediaWidth},
    {"getPageMediaHeight", m_PDFDoc_getPageMediaHeight},
    {"getPageCropWidth", m_PDFDoc_getPageCropWidth},
    {"getPageCropHeight", m_PDFDoc_getPageCropHeight},
    {"getNumPages", m_PDFDoc_getNumPages},
    {"readMetadata", m_PDFDoc_readMetadata},
    {"getStructTreeRoot", m_PDFDoc_getStructTreeRoot},
    {"findPage", m_PDFDoc_findPage},
    {"getLinks", m_PDFDoc_getLinks},
    {"findDest", m_PDFDoc_findDest},
    {"isEncrypted", m_PDFDoc_isEncrypted},
    {"okToPrint", m_PDFDoc_okToPrint},
    {"okToChange", m_PDFDoc_okToChange},
    {"okToCopy", m_PDFDoc_okToCopy},
    {"okToAddNotes", m_PDFDoc_okToAddNotes},
    {"isLinearized", m_PDFDoc_isLinearized},
    {"getDocInfo", m_PDFDoc_getDocInfo},
    {"getDocInfoNF", m_PDFDoc_getDocInfoNF},
    {"getPDFMajorVersion", m_PDFDoc_getPDFMajorVersion},
    {"getPDFMinorVersion", m_PDFDoc_getPDFMinorVersion},
    {"__tostring", m_PDFDoc__tostring},
    {"__gc", m_PDFDoc__gc},     // finalizer
    {NULL, NULL}                // sentinel
};

//**********************************************************************
// PDFRectangle

m_poppler_get_BOOL(PDFRectangle, isValid);

m_poppler__tostring(PDFRectangle);

static int m_PDFRectangle__index(lua_State * L)
{
    const char *s;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_PDFRectangle);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    s = luaL_checkstring(L, 2);
    if (strlen(s) == 2) {
        if (s[0] == 'x') {
            if (s[1] == '1')
                lua_pushnumber(L, ((PDFRectangle *) uin->d)->x1);
            else if (s[1] == '2')
                lua_pushnumber(L, ((PDFRectangle *) uin->d)->x2);
            else
                lua_pushnil(L);
        } else if (s[0] == 'y') {
            if (s[1] == '1')
                lua_pushnumber(L, ((PDFRectangle *) uin->d)->y1);
            else if (s[1] == '2')
                lua_pushnumber(L, ((PDFRectangle *) uin->d)->y2);
            else
                lua_pushnil(L);
        } else
            lua_pushnil(L);
    } else
        lua_pushnil(L);
    return 1;
}

static int m_PDFRectangle__newindex(lua_State * L)
{
    double d;
    const char *s;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_PDFRectangle);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    s = luaL_checkstring(L, 2);
    d = luaL_checknumber(L, 3);
    if (strlen(s) == 2) {
        if (s[0] == 'x') {
            if (s[1] == '1')
                ((PDFRectangle *) uin->d)->x1 = d;
            else if (s[1] == '2')
                ((PDFRectangle *) uin->d)->x2 = d;
            else
                luaL_error(L, "wrong PDFRectangle coordinate (%s)", s);
        } else if (s[0] == 'y') {
            if (s[1] == '1')
                ((PDFRectangle *) uin->d)->y1 = d;
            else if (s[1] == '2')
                ((PDFRectangle *) uin->d)->y2 = d;
        } else
            luaL_error(L, "wrong PDFRectangle coordinate (%s)", s);
    } else
        luaL_error(L, "wrong PDFRectangle coordinate (%s)", s);
    return 0;
}

static int m_PDFRectangle__gc(lua_State * L)
{
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_PDFRectangle);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
#ifdef DEBUG
    printf("\n===== PDFRectangle GC ===== uin=<%p>\n", uin);
#endif
    if (uin->atype == ALLOC_LEPDF)
        delete(PDFRectangle *) uin->d;
    return 0;
}

static const struct luaL_Reg PDFRectangle_m[] = {
    {"isValid", m_PDFRectangle_isValid},
    {"__index", m_PDFRectangle__index},
    {"__newindex", m_PDFRectangle__newindex},
    {"__tostring", m_PDFRectangle__tostring},
    {"__gc", m_PDFRectangle__gc},
    {NULL, NULL}                // sentinel
};

//**********************************************************************
// Ref

static int m_Ref__index(lua_State * L)
{
    const char *s;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Ref);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    s = luaL_checkstring(L, 2);
    if (strcmp(s, "num") == 0)
        lua_pushinteger(L, ((Ref *) uin->d)->num);
    else if (strcmp(s, "gen") == 0)
        lua_pushinteger(L, ((Ref *) uin->d)->gen);
    else
        lua_pushnil(L);
    return 1;
}

m_poppler__tostring(Ref);

static int m_Ref__gc(lua_State * L)
{
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Ref);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
#ifdef DEBUG
    printf("\n===== Ref GC ===== uin=<%p>\n", uin);
#endif
    if (uin->atype == ALLOC_LEPDF && ((Ref *) uin->d) != NULL)
        gfree(((Ref *) uin->d));
    return 0;
}

static const struct luaL_Reg Ref_m[] = {
    {"__index", m_Ref__index},
    {"__tostring", m_Ref__tostring},
    {"__gc", m_Ref__gc},        // finalizer
    {NULL, NULL}                // sentinel
};

//**********************************************************************
// Stream

static const char *StreamKindNames[] =
    { "File", "ASCIIHex", "ASCII85", "LZW", "RunLength", "CCITTFax", "DCT",
    "Flate", "JBIG2", "JPX", "Weird", NULL
};

m_poppler_get_INT(Stream, getKind);

static int m_Stream_getKindName(lua_State * L)
{
    StreamKind t;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_Stream);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    t = ((Stream *) uin->d)->getKind();
    lua_pushstring(L, StreamKindNames[t]);
    return 1;
}

m_poppler_do(Stream, reset);
m_poppler_do(Stream, close);
m_poppler_get_INT(Stream, getChar);
m_poppler_get_INT(Stream, lookChar);
m_poppler_get_INT(Stream, getRawChar);
m_poppler_get_INT(Stream, getUnfilteredChar);
m_poppler_do(Stream, unfilteredReset);
m_poppler_get_INT(Stream, getPos);
m_poppler_get_BOOL(Stream, isBinary);
m_poppler_get_poppler(Stream, Stream, getUndecodedStream);
m_poppler_get_poppler(Stream, Dict, getDict);

m_poppler__tostring(Stream);

static const struct luaL_Reg Stream_m[] = {
    {"getKind", m_Stream_getKind},
    {"getKindName", m_Stream_getKindName},      // not poppler
    {"reset", m_Stream_reset},
    {"close", m_Stream_close},
    {"getUndecodedStream", m_Stream_getUndecodedStream},
    {"getChar", m_Stream_getChar},
    {"lookChar", m_Stream_lookChar},
    {"getRawChar", m_Stream_getRawChar},
    {"getUnfilteredChar", m_Stream_getUnfilteredChar},
    {"unfilteredReset", m_Stream_unfilteredReset},
    // {"getLine", m_Stream_getLine},
    {"getPos", m_Stream_getPos},
    {"isBinary", m_Stream_isBinary},
    {"getUndecodedStream", m_Stream_getUndecodedStream},
    {"getDict", m_Stream_getDict},
    {"__tostring", m_Stream__tostring},
    {NULL, NULL}                // sentinel
};

//**********************************************************************
// XRef

m_poppler_get_BOOL(XRef, isOk);
m_poppler_get_INT(XRef, getErrorCode);
m_poppler_get_BOOL(XRef, isEncrypted);
m_poppler_get_BOOL(XRef, okToPrint);
m_poppler_get_BOOL(XRef, okToPrintHighRes);
m_poppler_get_BOOL(XRef, okToChange);
m_poppler_get_BOOL(XRef, okToCopy);
m_poppler_get_BOOL(XRef, okToAddNotes);
m_poppler_get_BOOL(XRef, okToFillForm);
m_poppler_get_BOOL(XRef, okToAccessibility);
m_poppler_get_BOOL(XRef, okToAssemble);
m_poppler_get_OBJECT(XRef, getCatalog);

static int m_XRef_fetch(lua_State * L)
{
    int num, gen;
    udstruct *uin, *uout;
    uin = (udstruct *) luaL_checkudata(L, 1, M_XRef);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    num = luaL_checkint(L, 2);
    gen = luaL_checkint(L, 3);
    uout = new_Object_userdata(L);
    uout->d = new Object();
    ((XRef *) uin->d)->fetch(num, gen, (Object *) uout->d);
    uout->atype = ALLOC_LEPDF;
    uout->pc = uin->pc;
    uout->pd = uin->pd;
    return 1;
}

m_poppler_get_OBJECT(XRef, getDocInfo);
m_poppler_get_OBJECT(XRef, getDocInfoNF);
m_poppler_get_INT(XRef, getNumObjects);
m_poppler_get_INT(XRef, getRootNum);
m_poppler_get_INT(XRef, getRootGen);
// getStreamEnd

static int m_XRef_getNumEntry(lua_State * L)
{
    int i, offset;
    udstruct *uin;
    uin = (udstruct *) luaL_checkudata(L, 1, M_XRef);
    if (uin->pd != NULL && uin->pd->pc != uin->pc)
        pdfdoc_changed_error(L);
    offset = luaL_checkint(L, 2);
    i = ((XRef *) uin->d)->getNumEntry(offset);
    if (i >= 0)
        lua_pushinteger(L, i);
    else
        lua_pushnil(L);
    return 1;
}

m_poppler_get_poppler(XRef, Object, getTrailerDict);

m_poppler__tostring(XRef);

static const struct luaL_Reg XRef_m[] = {
    {"isOk", m_XRef_isOk},
    {"getErrorCode", m_XRef_getErrorCode},
    {"isEncrypted", m_XRef_isEncrypted},
    {"okToPrint", m_XRef_okToPrint},
    {"okToPrintHighRes", m_XRef_okToPrintHighRes},
    {"okToChange", m_XRef_okToChange},
    {"okToCopy", m_XRef_okToCopy},
    {"okToAddNotes", m_XRef_okToAddNotes},
    {"okToFillForm", m_XRef_okToFillForm},
    {"okToAccessibility", m_XRef_okToAccessibility},
    {"okToAssemble", m_XRef_okToAssemble},
    {"getCatalog", m_XRef_getCatalog},
    {"fetch", m_XRef_fetch},
    {"getDocInfo", m_XRef_getDocInfo},
    {"getDocInfoNF", m_XRef_getDocInfoNF},
    {"getNumObjects", m_XRef_getNumObjects},
    {"getRootNum", m_XRef_getRootNum},
    {"getRootGen", m_XRef_getRootGen},
    // {"getStreamEnd", m_XRef_getStreamEnd},
    {"getNumEntry", m_XRef_getNumEntry},
    {"getTrailerDict", m_XRef_getTrailerDict},
    {"__tostring", m_XRef__tostring},
    {NULL, NULL}                // sentinel
};

//**********************************************************************
// XRefEntry

m_poppler__tostring(XRefEntry);

static const struct luaL_Reg XRefEntry_m[] = {
    {"__tostring", m_XRefEntry__tostring},
    {NULL, NULL}                // sentinel
};

//**********************************************************************

#define register_meta(type)                 \
    luaL_newmetatable(L, M_##type);         \
    lua_pushvalue(L, -1);                   \
    lua_setfield(L, -2, "__index");         \
    lua_pushstring(L, "no user access");    \
    lua_setfield(L, -2, "__metatable");     \
    luaL_register(L, NULL, type##_m)

int luaopen_epdf(lua_State * L)
{
    register_meta(Annot);
    register_meta(Annots);
    register_meta(Array);
    register_meta(Catalog);
    register_meta(Dict);
    register_meta(GooString);
    register_meta(LinkDest);
    register_meta(Links);
    register_meta(Object);
    register_meta(Page);
    register_meta(PDFDoc);
    register_meta(PDFRectangle);
    register_meta(Ref);
    register_meta(Stream);
    register_meta(XRef);
    register_meta(XRefEntry);

    luaL_register(L, "epdf", epdflib);
    return 1;
}
