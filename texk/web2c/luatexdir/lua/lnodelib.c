/* lnodelib.c

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
    with LuaTeX; if not, see <http://www.gnu.org/licenses/>.

*/

/*

    The node getter and setter are adapted a bit by Hans and Luigi so blame
    them! On the agenda: check all keys, maybe hide some fields that are not
    supposed to be seen and set (scratch fields for the backend and par
    builder).

    After doing lots of tests with luatex and luajittex, with and without jit,
    and with and without ffi, we came to the conclusion that userdata prevents
    a speedup. We also found that the checking of metatables as well as assignment
    comes with overhead that can't be neglected. This is normally not really a
    problem but when processing fonts for more complex scripts it's quite some
    overhead.

    Because the userdata approach has some benefits, we stick to this. We did
    some experiments with fast access (assuming nodes) and kept some of the code
    commented here, but eventually settled for the direct approach. For code that
    is proven to be okay, one can use the direct variants and operate on nodes
    more directly. Currently these are numbers, but that might become light
    userdata at one point, so *never* rely on that property. An important aspect
    is that one cannot mix both methods, although with node.direct.tonode and
    node.direct.todirect one can cast both representations.

    So the advice is: use the indexed approach when possible and investigate the
    direct one when speed might be an issue. For that reason we also provide the
    get* and set* functions in the top level node namespace. There is a limited set
    of getters.

    getnext    : parsing nodelist always involves this one
    getprev    : used less but is logical companion to getnext
    getid      : consulted a lot
    getsubtype : consulted less but also a topper
    getfont    : used a lot in otf handling (glyph nodes are consulted a lot)
    getchar    : idem and also in other places
    getlist    : we often parse nested lists so this is a convenient one too (only hlist/vlist !)
    getleader  : comparable to list, seldom used in tex (but needs consulting
                 like lists)
    getfield   : generic getter, sufficient for the rest (other field names are
                 often shared so a specific getter makes no sense then)

    Keep in mind that these only make sense when we're calling them millions of
    times (which happens in font processing for instance). Setters are less important
    as documents have not that many content related nodes (and setting many thousands
    of properties is hardly a burden contrary to millions of consultations.)

    Another change is that __index and __newindex are (as expected) exposed to
    users but do no checking. The getfield and setfield functions do check. In
    fact, fast mode can be simulated by fast_getfield = __index but the (measured)
    benefit on average runs is not that large (some 5% when we also use the other
    fast ones) which is easily nilled by inefficient coding. The direct variants
    on the other hand can be significantly faster but with the drawback of lack
    of userdata features. With respect to speed: keep in mind that measuring
    a speedup on these functions is not representative for a normal run, where
    much more happens.

    maybe : setnext setprev setfont setchar (only makes sense for some scripts)

    todo  : check and optimize the direct function when possible

    todo  : once the direct ones are proven we can redefine some of the less
            critical normal ones to call the direct ones after checking for
            a first argument being a node (like protect/unprotect)

    The code below has quite some duplicated code but this is also a prelude
    to light userdata for diretc nodes so we prefer this method. Some userdata
    variants could call the direct functions but not now (also because we don't
    want to touch the originals too much). As usual: blame Luigi and Hans for
    issues with this code. You can blame HH for weird or inaccurate comments.

    Todo: as a prelude to lua 5.3 we should use integer instead of number when
    possible. A boring job. We can use the direct variants for testing this.

    Hans Hagen, Luigi Scarso (2011-2013)

*/

static const char _svn_version[] =
    "$Id: lnodelib.c 4775 2014-02-07 12:36:34Z luigi $ "
    "$URL: https://foundry.supelec.fr/svn/luatex/trunk/source/texk/web2c/luatexdir/lua/lnodelib.c $";

#include "ptexlib.h"
#include "lua/luatex-api.h"

/*

    These macros create and access pointers (indices) to keys which is faster. The
    shortcuts are created as part of the initialization.

*/

#define init_luaS_index(a) do {                        \
    lua_pushliteral(L,#a);                             \
    luaS_##a##_ptr = lua_tostring(L,-1);               \
    luaS_##a##_index = luaL_ref (L,LUA_REGISTRYINDEX); \
} while (0)

#define init_luaS_index_s(a,b) do {                    \
    lua_pushliteral(L,b);                              \
    luaS_##a##_ptr = lua_tostring(L,-1);               \
    luaS_##a##_index = luaL_ref (L,LUA_REGISTRYINDEX); \
} while (0)

#define make_luaS_index(a)                    \
    static int luaS_##a##_index = 0;          \
    static const char * luaS_##a##_ptr = NULL

#define luaS_index(a) luaS_##a##_index

#define luaS_ptr_eq(a,b) (a==luaS_##b##_ptr)

/*

    When the first argument to an accessor is a node, we can use it's metatable
    entry when we are returning nodes, which saves a lookup.

*/

#define fast_metatable(n) do {                              \
     a = (halfword *) lua_newuserdata(L, sizeof(halfword)); \
     *a = n;                                                \
     lua_getmetatable(L,1);                                 \
     lua_setmetatable(L,-2);                                \
} while (0)

#define fast_metatable_or_nil(n) do {                          \
     if (n) {                                                  \
        a = (halfword *) lua_newuserdata(L, sizeof(halfword)); \
        *a = n;                                                \
        lua_getmetatable(L,1);                                 \
        lua_setmetatable(L,-2);                                \
    } else {                                                   \
        lua_pushnil(L);                                        \
    }                                                          \
} while (0)

#define fast_metatable_or_nil_alink(n) do {                    \
     if (n) {                                                  \
        alink(n) = null;                                       \
        a = (halfword *) lua_newuserdata(L, sizeof(halfword)); \
        *a = n;                                                \
        lua_getmetatable(L,1);                                 \
        lua_setmetatable(L,-2);                                \
    } else {                                                   \
    lua_pushnil(L);                                            \
   }                                                           \
} while (0)

#define fast_metatable_top(n) do {                          \
     a = (halfword *) lua_newuserdata(L, sizeof(halfword)); \
     *a = n;                                                \
     lua_getmetatable(L,-2);                                \
     lua_setmetatable(L,-2);                                \
} while (0)

#define lua_push_node_metatablelua do {                         \
    lua_rawgeti(L, LUA_REGISTRYINDEX, luaS_index(luatex_node)); \
    lua_gettable(L, LUA_REGISTRYINDEX);                         \
} while (0)

/*

    This is a first step towards abstract direct nodes. When we have Lua 5.3 we
    need to check all returned values for being integers. This might be another
    level of abtraction.

*/

#define nodelib_pushdirect(n) lua_pushnumber(L,n)
#define nodelib_popdirect(n) lua_tointeger(L,n)

#define nodelib_pushdirect_or_nil(n) do { \
    if (n==null) {                        \
        lua_pushnil(L);                   \
    } else {                              \
        lua_pushnumber(L,n);              \
    }                                     \
} while (0)

#define nodelib_pushdirect_or_nil_alink(n) do { \
    if (n==null) {                              \
        lua_pushnil(L);                         \
    } else {                                    \
        alink(n) = null;                        \
        lua_pushnumber(L,n);                    \
    }                                           \
} while (0)

/* handy sometimes: */

/* static void stackDump (lua_State * L) { */
/*     int i; */
/*     int top = lua_gettop(L); */
/*     for (i = top; i >= 1; i--) { */
/*         int t = lua_type(L, i); */
/*         switch (t) { */
/*             case LUA_TSTRING: */
/*                 printf("[%d] '%s'\n", i, lua_tostring(L, i)); */
/*                 break; */
/*             case LUA_TBOOLEAN: */
/*                 printf("[%d] %s\n",i, lua_toboolean(L, i) ? "true" : "false"); */
/*                 break; */
/*             case LUA_TNUMBER: */
/*                 printf("[%d] %g\n", i, lua_tonumber(L, i)); */
/*                 break; */
/*             default: */
/*                 printf("[%d][%d] %s &d=0x%x *d=%d\n",i-top-1, i, lua_typename(L, t), lua_touserdata(L,i), *((int *)lua_touserdata(L,i))); */
/*                 break; */
/*         } */
/*     } */
/* } */


#define nodelib_gettoks(L,a)   tokenlist_from_lua(L)

#define nodelib_getspec        nodelib_getlist
#define nodelib_getaction      nodelib_getlist

/* a quick helper for dir nodes; there is only a small set of possible values */

#define RETURN_DIR_VALUES(a)     \
if (s==luaS_##a##_ptr) {         \
    return (dir_##a);            \
} else if (!absolute_only)  {    \
    if (s==luaS_p##a##_ptr)      \
        return (dir_##a);        \
    else if (s==luaS_m##a##_ptr) \
        return ((dir_##a)-64);   \
}

/* fetching a field from a node .. we can often use the reuse bot-of-stack metatable */

#define nodelib_pushlist(L,n) { lua_pushnumber(L,n); lua_nodelib_push(L); }      /* can be: fast_metatable_or_nil(n) */
#define nodelib_pushattr(L,n) { lua_pushnumber(L,n); lua_nodelib_push(L); }      /* can be: fast_metatable_or_nil(n) */
#define nodelib_pushspec(L,n) { lua_pushnumber(L,n); lua_nodelib_push_spec(L); } /* can be: fast_metatable_or_nil(n) - different criterium? */
#define nodelib_pushaction(L,n) { lua_pushnumber(L,n); lua_nodelib_push(L); }    /* can be: fast_metatable_or_nil(n) */
#define nodelib_pushstring(L,n) { char *ss=makecstring(n); lua_pushstring(L,ss); free(ss); }

/* find prev, and fix backlinks .. can be a macro instead (only used a few times) */

#define set_t_to_prev(head,current)      \
t = head;                                \
while (vlink(t)!=current && t != null) { \
    if (vlink(t)!=null)                  \
        alink(vlink(t)) = t;             \
    t = vlink(t);                        \
}

#define get_node_type_id(L,n)    do_get_node_type_id(L,n,node_data)
#define get_node_subtype_id(L,n) do_get_node_type_id(L,n,whatsit_node_data)

#define box(A) eqtb[box_base+(A)].hh.rh
#define direct_check_index_range(j,s)                                      \
    if (j<0 || j > 65535) {                                                \
        luaL_error(L, "incorrect index value %d for tex.%s()", (int)j, s); \
    }

#define NODE_METATABLE  "luatex_node"

#define DEBUG 0
#define DEBUG_OUT stdout

make_luaS_index(luatex_node);

make_luaS_index(accent);
make_luaS_index(action);
make_luaS_index(action_id);
make_luaS_index(action_type);
make_luaS_index(area);
make_luaS_index(attr);
make_luaS_index(best_ins_ptr);
make_luaS_index(bot_accent);
make_luaS_index(box_left);
make_luaS_index(box_left_width);
make_luaS_index(box_right);
make_luaS_index(box_right_width);
make_luaS_index(broken_ins);
make_luaS_index(broken_ptr);
make_luaS_index(char);
make_luaS_index(class);
make_luaS_index(command);
make_luaS_index(components);
make_luaS_index(cost);
make_luaS_index(count);
make_luaS_index(data);
make_luaS_index(degree);
make_luaS_index(delim);
make_luaS_index(denom);
make_luaS_index(depth);
make_luaS_index(dest_id);
make_luaS_index(dest_type);
make_luaS_index(dir);
make_luaS_index(dir_h);
make_luaS_index(display);
make_luaS_index(dvi_ptr);
make_luaS_index(expansion_factor);
make_luaS_index(ext);
make_luaS_index(fam);
make_luaS_index(fast);
make_luaS_index(file);
make_luaS_index(font);
make_luaS_index(global);
make_luaS_index(glue_order);
make_luaS_index(glue_set);
make_luaS_index(glue_sign);
make_luaS_index(glyph);
make_luaS_index(head);
make_luaS_index(height);
make_luaS_index(id);
make_luaS_index(index);
make_luaS_index(kern);
make_luaS_index(lang);
make_luaS_index(large_char);
make_luaS_index(large_fam);
make_luaS_index(last_ins_ptr);
make_luaS_index(leader);
make_luaS_index(left);
make_luaS_index(level);
make_luaS_index(link_attr);
make_luaS_index(list);
make_luaS_index(mark);
make_luaS_index(mode);
make_luaS_index(name);
make_luaS_index(named_id);
make_luaS_index(new_window);
make_luaS_index(next);
make_luaS_index(nucleus);
make_luaS_index(num);
make_luaS_index(number);
make_luaS_index(objnum);
make_luaS_index(pen_broken);
make_luaS_index(pen_inter);
make_luaS_index(penalty);
make_luaS_index(post);
make_luaS_index(pre);
make_luaS_index(prev);
make_luaS_index(ref_count);
make_luaS_index(reg);
make_luaS_index(replace);
make_luaS_index(right);
make_luaS_index(script);
make_luaS_index(scriptscript);
make_luaS_index(shift);
make_luaS_index(shrink);
make_luaS_index(shrink_order);
make_luaS_index(small_char);
make_luaS_index(small_fam);
make_luaS_index(spec);
make_luaS_index(stack);
make_luaS_index(stream);
make_luaS_index(stretch);
make_luaS_index(stretch_order);
make_luaS_index(string);
make_luaS_index(style);
make_luaS_index(sub);
make_luaS_index(subtype);
make_luaS_index(sup);
make_luaS_index(surround);
make_luaS_index(text);
make_luaS_index(thread_attr);
make_luaS_index(thread_id);
make_luaS_index(transform);
make_luaS_index(type);
make_luaS_index(uchyph);
make_luaS_index(user_id);
make_luaS_index(value);
make_luaS_index(width);
make_luaS_index(writable);
make_luaS_index(xoffset);
make_luaS_index(xyz_zoom);
make_luaS_index(yoffset);

make_luaS_index(additional);
make_luaS_index(cal_expand_ratio);
make_luaS_index(exactly);
make_luaS_index(subst_ex_font);

make_luaS_index(TLT);
make_luaS_index(TRT);
make_luaS_index(LTL);
make_luaS_index(RTT);

make_luaS_index(pTLT);
make_luaS_index(pTRT);
make_luaS_index(pLTL);
make_luaS_index(pRTT);

make_luaS_index(mTLT);
make_luaS_index(mTRT);
make_luaS_index(mLTL);
make_luaS_index(mRTT);

static void initialize_luaS_indexes(lua_State * L)
{
    init_luaS_index(luatex_node);

    init_luaS_index(accent);
    init_luaS_index(action_id);
    init_luaS_index(action);
    init_luaS_index(action_type);
    init_luaS_index(area);
    init_luaS_index(attr);
    init_luaS_index(best_ins_ptr);
    init_luaS_index(bot_accent);
    init_luaS_index(box_left);
    init_luaS_index(box_left_width);
    init_luaS_index(box_right);
    init_luaS_index(box_right_width);
    init_luaS_index(broken_ins);
    init_luaS_index(broken_ptr);
    init_luaS_index(char);
    init_luaS_index(class);
    init_luaS_index(command);
    init_luaS_index(components);
    init_luaS_index(cost);
    init_luaS_index(count);
    init_luaS_index(data);
    init_luaS_index(degree);
    init_luaS_index(delim);
    init_luaS_index(denom);
    init_luaS_index(depth);
    init_luaS_index(dest_id);
    init_luaS_index(dest_type);
    init_luaS_index(dir);
    init_luaS_index(dir_h);
    init_luaS_index(display);
    init_luaS_index(dvi_ptr);
    init_luaS_index(expansion_factor);
    init_luaS_index(ext);
    init_luaS_index(fam);
    init_luaS_index(fast);
    init_luaS_index(file);
    init_luaS_index(font);
    init_luaS_index(global);
    init_luaS_index(glue_order);
    init_luaS_index(glue_set);
    init_luaS_index(glue_sign);
    init_luaS_index(glyph);
    init_luaS_index(head);
    init_luaS_index(height);
    init_luaS_index(id);
    init_luaS_index(index);
    init_luaS_index(kern);
    init_luaS_index(lang);
    init_luaS_index(large_char);
    init_luaS_index(large_fam);
    init_luaS_index(last_ins_ptr);
    init_luaS_index(leader);
    init_luaS_index(left);
    init_luaS_index(level);
    init_luaS_index(link_attr);
    init_luaS_index(list);
    init_luaS_index(mark);
    init_luaS_index(mode);
    init_luaS_index(name);
    init_luaS_index(named_id);
    init_luaS_index(new_window);
    init_luaS_index(next);
    init_luaS_index(nucleus);
    init_luaS_index(num);
    init_luaS_index(number);
    init_luaS_index(objnum);
    init_luaS_index(pen_broken);
    init_luaS_index(pen_inter);
    init_luaS_index(penalty);
    init_luaS_index(post);
    init_luaS_index(pre);
    init_luaS_index(prev);
    init_luaS_index(ref_count);
    init_luaS_index(reg);
    init_luaS_index(replace);
    init_luaS_index(right);
    init_luaS_index(script);
    init_luaS_index(scriptscript);
    init_luaS_index(shift);
    init_luaS_index(shrink);
    init_luaS_index(shrink_order);
    init_luaS_index(small_char);
    init_luaS_index(small_fam);
    init_luaS_index(spec);
    init_luaS_index(stack);
    init_luaS_index(stream);
    init_luaS_index(stretch);
    init_luaS_index(stretch_order);
    init_luaS_index(string);
    init_luaS_index(style);
    init_luaS_index(sub);
    init_luaS_index(subtype);
    init_luaS_index(sup);
    init_luaS_index(surround);
    init_luaS_index(text);
    init_luaS_index(thread_attr);
    init_luaS_index(thread_id);
    init_luaS_index(transform);
    init_luaS_index(type);
    init_luaS_index(uchyph);
    init_luaS_index(user_id);
    init_luaS_index(value);
    init_luaS_index(width);
    init_luaS_index(writable);
    init_luaS_index(xoffset);
    init_luaS_index(xyz_zoom);
    init_luaS_index(yoffset);

    init_luaS_index(additional);
    init_luaS_index(cal_expand_ratio);
    init_luaS_index(exactly);
    init_luaS_index(subst_ex_font);

    init_luaS_index(TLT);
    init_luaS_index(TRT);
    init_luaS_index(LTL);
    init_luaS_index(RTT);
    init_luaS_index_s(pTLT,"+TLT");
    init_luaS_index_s(pTRT,"+TRT");
    init_luaS_index_s(pLTL,"+LTL");
    init_luaS_index_s(pRTT,"+RTT");
    init_luaS_index_s(mTLT,"-TLT");
    init_luaS_index_s(mTRT,"-TRT");
    init_luaS_index_s(mLTL,"-LTL");
    init_luaS_index_s(mRTT,"-RTT");

}

/* maybe these qualify as macros, not functions */

static halfword *maybe_isnode(lua_State * L, int ud)
{
    halfword *p = lua_touserdata(L, ud);
    if (p != NULL) {
        if (lua_getmetatable(L, ud)) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, luaS_index(luatex_node));
            lua_gettable(L, LUA_REGISTRYINDEX);
            if (!lua_rawequal(L, -1, -2))
                p = NULL;
            lua_pop(L, 2);
        }
    }
    return p;
}

/* we could make the message a function and just inline the rest (via a macro) */

halfword *check_isnode(lua_State * L, int ud)
{
    halfword *p = maybe_isnode(L, ud);
    if (p != NULL)
        return p;
    pdftex_fail("There should have been a lua <node> here, not an object with type %s!", luaL_typename(L, ud));
    return NULL;
}

/*

    This routine finds the numerical value of a string (or number) at
    lua stack index |n|. If it is not a valid node type, returns -1

    Todo: create a lua table instead at initialization time and then
    use that hash, so that we don't need to loop for each lookup.

    HH: can crash on noad 9

*/

static int do_get_node_type_id(lua_State * L, int n, node_info * data)
{
    int j;
       /* switch (lua_type(L, n)) { */
       /* case LUA_TNIL:  */
       /*    printf("LUA_TNIL\n"); */
       /*    break; */
       /* case LUA_TNUMBER: */
       /*    printf("LUA_TNUMBER\n"); */
       /*    break; */
       /* case LUA_TBOOLEAN:  */
       /*    printf("LUA_TBOOLEAN\n"); */
       /*    break; */
       /* case LUA_TSTRING:  */
       /*    printf("LUA_TSTRING\n"); */
       /*    break; */
       /* case LUA_TTABLE:  */
       /*    printf("LUA_TTABLE\n"); */
       /*    break; */
       /* case LUA_TFUNCTION:  */
       /*    printf("LUA_TFUNCTION\n"); */
       /*    break; */
       /* case LUA_TUSERDATA:  */
       /*    printf("LUA_TUSERDATA\n"); */
       /*    break; */
       /* case LUA_TTHREAD:  */
       /*    printf("LUA_TTHREAD\n"); */
       /*    break; */
       /* case  LUA_TLIGHTUSERDATA: */
       /*    printf("LUA_TLIGHTUSERDATA\n"); */
       /*    break; */
       /* } */
    if (lua_type(L, n) == LUA_TSTRING) {
        const char *s = lua_tostring(L, n);
        for (j = 0; data[j].id != -1; j++) {
            if (strcmp(s, data[j].name) == 0)
                return j;
        }
    } else if (lua_type(L, n) == LUA_TNUMBER) {
        int i = (int) lua_tointeger(L, n);
        for (j = 0; data[j].id != -1; j++) {
            if (data[j].id == i)
                return j;
        }
    }
    return -1;
}

static int get_valid_node_type_id(lua_State * L, int n)
{
    int i = get_node_type_id(L, n);
    if (i == -1) {
        if (lua_type(L, n) == LUA_TSTRING) {
            luaL_error(L, "Invalid node type id: %s", lua_tostring(L, n));
        } else {
            luaL_error(L, "Invalid node type id: %d", lua_tonumber(L, n));
        }
    }
    return i;
}

static int get_valid_node_subtype_id(lua_State * L, int n)
{
    int i = get_node_subtype_id(L, n);
    if (i == -1) {
        if (lua_type(L, n) == LUA_TSTRING) {
            luaL_error(L, "Invalid whatsit node id: %s", lua_tostring(L, n));
        } else {
            luaL_error(L, "Invalid whatsit node id: %d", lua_tonumber(L, n));
        }
    }
    return i;
}

/* two simple helpers to speed up and simplify lua code (replaced by getnext and getprev) */

static int lua_nodelib_next(lua_State * L)
{
    halfword *p = maybe_isnode(L,1);
    if (p != NULL && *p && vlink(*p)) {
        lua_nodelib_push_fast(L,vlink(*p));
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int lua_nodelib_prev(lua_State * L)
{
    halfword *p = maybe_isnode(L,1);
    if (p != NULL && *p && alink(*p)) {
        lua_nodelib_push_fast(L,alink(*p));
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/*

    static void lua_nodelib_push_simple(lua_State * L, halfword p)
    {
        halfword *a;
        a = (halfword *) lua_newuserdata(L, sizeof(halfword));
        *a = p;
        lua_rawgeti(L, LUA_REGISTRYINDEX, luaS_index(luatex_node));
        lua_gettable(L, LUA_REGISTRYINDEX);
        lua_setmetatable(L, -2);
    }

*/

/*

    Creates a userdata object for a number found at the stack top, if it is
    representing a node (i.e. an pointer into |varmem|). It replaces the
    stack entry with the new userdata, or pushes |nil| if the number is |null|,
    or if the index is definately out of range. This test could be improved.

*/

void lua_nodelib_push(lua_State * L)
{
    halfword n;
    halfword *a;
    n = -1;
    if (lua_isnumber(L, -1))
        n = (int) lua_tointeger(L, -1);
    lua_pop(L, 1);
    if ((n == null) || (n < 0) || (n > var_mem_max)) {
        lua_pushnil(L);
    } else {
        a = lua_newuserdata(L, sizeof(halfword));
        *a = n;
        lua_rawgeti(L, LUA_REGISTRYINDEX, luaS_index(luatex_node));
        lua_gettable(L, LUA_REGISTRYINDEX);
        lua_setmetatable(L, -2);
    }
    return;
}

/* |spec_ptr| fields can legally be zero, which is why there is a special function. */

static void lua_nodelib_push_spec(lua_State * L)
{
    halfword n;
    halfword *a;
    n = -1;
    if (lua_isnumber(L, -1))
        n = (halfword) lua_tointeger(L, -1);
    lua_pop(L, 1);
    if ((n < 0) || (n > var_mem_max)) {
        lua_pushnil(L);
    } else {
        a = lua_newuserdata(L, sizeof(halfword));
        *a = n;
        lua_rawgeti(L, LUA_REGISTRYINDEX, luaS_index(luatex_node));
        lua_gettable(L, LUA_REGISTRYINDEX);
        lua_setmetatable(L, -2);
    }
    return;
}

void lua_nodelib_push_fast(lua_State * L, halfword n)
{
    halfword *a;
    a = lua_newuserdata(L, sizeof(halfword));
    *a = n;
    lua_rawgeti(L, LUA_REGISTRYINDEX, luaS_index(luatex_node));
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_setmetatable(L, -2);
    return;
}

/* converts type strings to type ids */

static int lua_nodelib_id(lua_State * L)
{
    int i = get_node_type_id(L, 1);
    if (i >= 0) {
        lua_pushnumber(L, i);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/* node.getid */

static int lua_nodelib_getid(lua_State * L)
{
    /* [given-node] [...] */
    halfword *p = lua_touserdata(L, 1);
    if ( (p == NULL) || (! lua_getmetatable(L,1)) ) {
        lua_pushnil(L);
        return 1;
    }
    /* [given-node] [mt-given-node] */
    lua_rawgeti(L, LUA_REGISTRYINDEX, luaS_index(luatex_node));
    lua_gettable(L, LUA_REGISTRYINDEX);
    /* [given-node] [mt-given-node] [mt-node] */
    if (!lua_rawequal(L, -1, -2)) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, type(*p));
    }
    return 1;
}

/* node.fast.getid

    static int lua_nodelib_fast_getid(lua_State * L)
    {
        halfword *n;
        n = (halfword *) lua_touserdata(L, 1);
        lua_pushnumber(L, type(*n));
        return 1;
    }

*/

/* node.direct.getid */

static int lua_nodelib_direct_getid(lua_State * L)
{
    halfword n;
    n = (halfword) lua_tonumber(L, 1);
    if (n == null) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, type(n));
    }
    return 1;
}

/* node.getsubtype */

static int lua_nodelib_getsubtype(lua_State * L)
{
    halfword *p = lua_touserdata(L, 1);
    if ( (p == NULL) || (! lua_getmetatable(L,1)) ) {
        lua_pushnil(L);
    } else {
        lua_push_node_metatablelua;
        if ( (!lua_rawequal(L, -1, -2)) || (! nodetype_has_subtype(*p))) {
            lua_pushnil(L);
        } else {
            lua_pushnumber(L, subtype(*p));
        }
    }
    return 1;
}

/* node.fast.getsubtype

    static int lua_nodelib_fast_getsubtype(lua_State * L)
    {
        halfword *n;
        n = (halfword *) lua_touserdata(L, 1);
        lua_pushnumber(L, subtype(*n));
        return 1;
    }

*/

/* node.direct.getsubtype */

static int lua_nodelib_direct_getsubtype(lua_State * L)
{
    halfword n;
    n = (halfword) lua_tonumber(L, 1);
    if (n == null) { /* no check, we assume sane use */
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, subtype(n));
    }
    return 1;
}

/* node.getfont */

static int lua_nodelib_getfont(lua_State * L)
{
    halfword *p = lua_touserdata(L, 1);
    if ( (p == NULL) || (! lua_getmetatable(L,1)) ) {
        lua_pushnil(L);
    } else {
        lua_push_node_metatablelua;
        if ( (!lua_rawequal(L, -1, -2)) || (type(*p) != glyph_node) ) {
            lua_pushnil(L);
        } else {
            lua_pushnumber(L, font(*p));
        }
    }
    return 1;
}

/* node.fast.getfont

    static int lua_nodelib_fast_getfont(lua_State * L)
    {
        halfword *n;
        n = (halfword *) lua_touserdata(L, 1);
        if (type(*n) != glyph_node) {
            lua_pushnil(L);
        } else {
            lua_pushnumber(L, font(*n));
        }
        return 1;
    }

*/

/* node.direct.getfont */

static int lua_nodelib_direct_getfont(lua_State * L)
{
    halfword n;
    n = (halfword) lua_tonumber(L, 1);
    if ((n == null) || (type(n) != glyph_node)) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, font(n));
    }
    return 1;
}

/* node.getchar */

static int lua_nodelib_getcharacter(lua_State * L)
{
    halfword *n = lua_touserdata(L, 1);
    if ( (n == NULL) || (! lua_getmetatable(L,1)) ) {
        lua_pushnil(L);
    } else if (type(*n) == glyph_node) {
        lua_pushnumber(L, character(*n));
    } else if ((type(*n) == math_char_node) || (type(*n) == math_text_char_node)) {
        lua_pushnumber(L, math_character(*n));
    }
    return 1;
}

/* node.fast.getchar

    static int lua_nodelib_fast_getcharacter(lua_State * L)
    {
        halfword *n;
        n = (halfword *) lua_touserdata(L, 1);
        if (type(*n) == glyph_node) {
            lua_pushnumber(L, character(*n));
        } else if ((type(*n) == math_char_node) || (type(*n) == math_text_char_node)) {
            lua_pushnumber(L, math_character(*n));
        } else {
            lua_pushnil(L);
        }
        return 1;
    }

*/

/* node.direct.getchar */

static int lua_nodelib_direct_getcharacter(lua_State * L)
{
    halfword n;
    n = (halfword) lua_tonumber(L, 1);
    if (n == null) {
        lua_pushnil(L);
    } else if (type(n) == glyph_node) {
        lua_pushnumber(L, character(n));
    } else if ((type(n) == math_char_node) || (type(n) == math_text_char_node)) {
        lua_pushnumber(L, math_character(n));
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/* node.getlist */

static int lua_nodelib_getlist(lua_State * L)
{
    halfword *a;
    halfword *n = lua_touserdata(L, 1);
    if ( (n == NULL) || (! lua_getmetatable(L,1)) ) {
        lua_pushnil(L);
    } else if ((type(*n) == hlist_node) || (type(*n) == vlist_node)) {
        fast_metatable_or_nil_alink(list_ptr(*n));
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/* node.direct.getlist */

static int lua_nodelib_direct_getlist(lua_State * L)
{
    halfword n;
    n = (halfword) lua_tonumber(L, 1);
    if ( n == null) {
        lua_pushnil(L);
    } else if ((type(n) == hlist_node) || (type(n) == vlist_node)) {
        nodelib_pushdirect_or_nil_alink(list_ptr(n));
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/* node.getleader */

static int lua_nodelib_getleader(lua_State * L)
{
    halfword *a;
    halfword *n = lua_touserdata(L, 1);
    if ( (n == NULL) || (! lua_getmetatable(L,1)) ) {
        lua_pushnil(L);
    } else if (type(*n) == glue_node) {
        fast_metatable_or_nil(leader_ptr(*n));
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/* node.direct.getleader */

static int lua_nodelib_direct_getleader(lua_State * L)
{
    halfword n;
    n = (halfword) lua_tonumber(L, 1);
    if ( n == null) {
        lua_pushnil(L);
    } else if (type(n) == glue_node) {
        nodelib_pushdirect_or_nil(leader_ptr(n));
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/* node.getnext */

static int lua_nodelib_getnext(lua_State * L)
{
    halfword *a;
    /* [given-node] [...]*/
    halfword *p = lua_touserdata(L, 1);
    if ( (p == NULL) || (! lua_getmetatable(L,1)) ) {
        lua_pushnil(L);
    } else {
        /* [given-node] [mt-given-node]*/
        lua_rawgeti(L, LUA_REGISTRYINDEX, luaS_index(luatex_node));
        lua_gettable(L, LUA_REGISTRYINDEX);
        /* [given-node] [mt-given-node] [mt-node]*/
        if (!lua_rawequal(L, -1, -2)) {
            lua_pushnil(L);
        } else {
            fast_metatable_or_nil(vlink(*p));
        }
    }
    return 1; /* just one*/
}

/* node.fast.getnext

    static int lua_nodelib_fast_getnext(lua_State * L)
    {
        halfword *a;
        //  [given-node] [...]
        halfword *p = lua_touserdata(L, 1);
        if ((p == NULL) || (!vlink(*p))){
            lua_pushnil(L);
        } else {
            lua_settop(L,1);
            //  [given-node]
            lua_getmetatable(L,1);
            //  [given-node] [mt-node]
            a = lua_newuserdata(L, sizeof(halfword));
            //  [given-node] [mt-node] [next-node]
            *a = vlink(*p);
            lua_replace(L,1);
            //  [next-node] [mt-node]
            lua_setmetatable(L,1);
            //  [next-node-with-mt]
        }
        return 1;
    }

*/

/* node.direct.getnext */

static int lua_nodelib_direct_getnext(lua_State * L)
{
    halfword p;
    p = (halfword) lua_tonumber(L, 1);
    if (p == null) {
        lua_pushnil(L);
    } else {
        nodelib_pushdirect_or_nil(vlink(p));
    }
    return 1;
}

/* node.getprev */

static int lua_nodelib_getprev(lua_State * L)
{
    halfword *a;
    halfword *p = lua_touserdata(L, 1);
    if ( (p == NULL) || (! lua_getmetatable(L,1)) ) {
        lua_pushnil(L);
    } else {
        lua_rawgeti(L, LUA_REGISTRYINDEX, luaS_index(luatex_node));
        lua_gettable(L, LUA_REGISTRYINDEX);
        if (!lua_rawequal(L, -1, -2)) {
            lua_pushnil(L);
        } else {
            fast_metatable_or_nil(alink(*p));
        }
    }
    return 1;
}


/* node.fast.getprev

    static int lua_nodelib_fast_getprev(lua_State * L)
    {
        halfword *a;
        //  [given-node] [...]
        halfword *p = lua_touserdata(L, 1);
        if ((p == NULL) || (!alink(*p))) {
            lua_pushnil(L);
        } else {
            lua_settop(L,1);
            //  [given-node]
            lua_getmetatable(L,1);
            //  [given-node] [mt-node]
            a = lua_newuserdata(L, sizeof(halfword));
            //  [given-node] [mt-node] [next-node]
            *a = alink(*p);
            lua_replace(L,1);
            //  [next-node] [mt-node]
            lua_setmetatable(L,1);
            //  [next-node-with-mt]
        }
        return 1;
    }

*/

/* node.direct.getprev */

static int lua_nodelib_direct_getprev(lua_State * L)
{
    halfword p;
    p = (halfword) lua_tonumber(L, 1);
    if (p == null) {
        lua_pushnil(L);
    } else {
        nodelib_pushdirect_or_nil(alink(p));
    }
    return 1;
}

/* node.subtype */

static int lua_nodelib_subtype(lua_State * L)
{
    int i = get_node_subtype_id(L, 1);
    if (i >= 0) {
        lua_pushnumber(L, i);
    } else {
        lua_pushnil(L); /* return 0; */
    }
    return 1;
}

/* node.type (converts id numbers to type names) */

static int lua_nodelib_type(lua_State * L)
{
    if (lua_type(L,1) == LUA_TNUMBER) {
        int i = get_node_type_id(L, 1);
        if (i >= 0) {
            lua_pushstring(L, node_data[i].name);
            return 1;
        }
    } else if (maybe_isnode(L, 1) != NULL) {
        lua_pushstring(L,"node");
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

/* node.new (allocate a new node) */

static int lua_nodelib_new(lua_State * L)
{
    int i, j;
    halfword n = null;
    i = get_valid_node_type_id(L, 1);
    if (i == whatsit_node) {
        j = -1;
        if (lua_gettop(L) > 1)
            j = get_valid_node_subtype_id(L, 2);
        if (j < 0)
            luaL_error(L, "Creating a whatsit requires the subtype number as a second argument");
    } else {
        j = 0;
        if (lua_gettop(L) > 1)
            j = (int) lua_tointeger(L, 2);
    }
    n = new_node(i, j);
    lua_nodelib_push_fast(L, n);
    return 1;
}

/* node.direct.new (still with checking) */

static int lua_nodelib_direct_new(lua_State * L)
{
    int i, j;
    halfword n ;
    i = get_valid_node_type_id(L, 1);
    if (i == whatsit_node) {
        j = -1;
        if (lua_gettop(L) > 1)
            j = get_valid_node_subtype_id(L, 2);
        if (j < 0)
            luaL_error(L, "Creating a whatsit requires the subtype number as a second argument");
    } else {
        j = 0;
        if (lua_gettop(L) > 1)
            j = (int) lua_tointeger(L, 2);
    }
    n = new_node(i, j);
    lua_pushnumber(L,n);
    return 1;
}

/* node.free (this function returns the 'next' node, because that may be helpful) */

static int lua_nodelib_free(lua_State * L)
{
    halfword *n;
    halfword p;
    if (lua_gettop(L) < 1) {
        lua_pushnil(L);
        return 1;
    } else if (lua_isnil(L, 1)) {
        return 1;               /* the nil itself */
    }
    n = check_isnode(L, 1);
    p = vlink(*n);
    flush_node(*n);
    /* can be: lua_nodelib_push_fast(L, p); */
    lua_pushnumber(L, p);
    lua_nodelib_push(L);
    return 1;
}

/* node.direct.free */

static int lua_nodelib_direct_free(lua_State * L)
{
    halfword n;
    halfword p;
    n = (halfword) lua_tonumber(L,1);
    if (n == null) {
        lua_pushnil(L);
    } else {
        p = vlink(n);
        flush_node(n);
        if (p == 0) {
            lua_pushnil(L);
        } else {
            lua_pushnumber(L,p);
        }
    }
    return 1;
}

/* node.flush_node (no next returned) */

static int lua_nodelib_flush_node(lua_State * L)
{
    halfword *n;
    if ((lua_gettop(L) < 1) || lua_isnil(L, 1))
        return 0;
    n = check_isnode(L, 1);
    // no check if n?
    flush_node(*n);
    return 0;
}

/* node.direct.flush_node */

static int lua_nodelib_direct_flush_node(lua_State * L)
{
    halfword n;
    n = (halfword) lua_tonumber(L,1);
    if (n == null)
        return 0;
    flush_node(n);
    return 0;
}

/* node.flush_list */

static int lua_nodelib_flush_list(lua_State * L)
{
    halfword *n_ptr;
    if ((lua_gettop(L) < 1) || lua_isnil(L, 1))
        return 0;
    n_ptr = check_isnode(L, 1);
    flush_node_list(*n_ptr);
    return 0;
}

/* node.direct.flush_list */

static int lua_nodelib_direct_flush_list(lua_State * L)
{
    halfword n;
    n = (halfword) lua_tonumber(L,1);
    if (n == null)
        return 0;
    flush_node_list(n);
    return 0;
}

/* remove a node from a list */

#if DEBUG

    static void show_node_links (halfword l, const char * p)
    {
        halfword t = l;
        while (t) {
            fprintf(DEBUG_OUT, "%s t = %d, prev = %d, next = %d\n", p, (int)t, (int)alink(t), (int)vlink(t));
            t = vlink(t);
        }
    }

#endif

/* node.remove */

static int lua_nodelib_remove(lua_State * L)
{
    halfword head, current, t;
    if (lua_gettop(L) < 2)
        luaL_error(L, "Not enough arguments for node.remove()");
    head = *(check_isnode(L, 1));
#if DEBUG
    show_node_links(head, "before");
#endif
    if (lua_isnil(L, 2))
        return 2;               /* the arguments, as they are */
    current = *(check_isnode(L, 2));
    if (head == current) {
      if (alink(current)){
        vlink(alink(current)) = vlink(current); // vlink(prev) = next 
      }
      if (vlink(current)){
        alink( vlink(current)) = alink(current); // alink(next) = prev
      }

      head = vlink(current);     //head = next
      current = vlink(current);  //current = next
    } else {                    /* head != current */
        t = alink(current);
        if (t == null || vlink(t) != current) {
            set_t_to_prev(head, current);
            if (t == null)     /* error! */
                luaL_error(L,"Attempt to node.remove() a non-existing node");
        }
        /* t is now the previous node */
        vlink(t) = vlink(current);
        if (vlink(current) != null)
            alink(vlink(current)) = t;
        current = vlink(current);
    }
#if DEBUG
    show_node_links(head, "after");
#endif
    /* can be: lua_nodelib_push_fast(L, head); */
    lua_pushnumber(L, head);
    lua_nodelib_push(L);
    /* can be: lua_nodelib_push_fast(L, current); */
    lua_pushnumber(L, current);
    lua_nodelib_push(L);
    return 2;
}

/* node.direct.remove */

static int lua_nodelib_direct_remove(lua_State * L)
{
    halfword head, current , t;
    head = (halfword) lua_tonumber(L,1);
    if (head == null) {
        lua_pushnil(L);
        lua_pushnil(L);
        return 2 ;
    }
    current = (halfword) lua_tonumber(L,2);
    if (current == null) {
        lua_pushnumber(L, head);
        lua_pushnil(L);
        return 2 ;
    }
    if (head == current) {
      if (alink(current)){
        vlink( alink(current) ) = vlink(current); // vlink(prev) = next 
      }
      if (vlink(current)){
        alink( vlink(current) ) = alink(current); // alink(next) = prev
      }
      head = vlink(current);     //head = next
      current = vlink(current);  //current = next
    } else {
        t = alink(current);
        if (t == null || vlink(t) != current) {
            set_t_to_prev(head, current);
            if (t == null) {
                luaL_error(L,"Attempt to node.direct.remove() a non-existing node");
            }
        }
        vlink(t) = vlink(current);
        if (vlink(current) != null)
            alink(vlink(current)) = t;
        current = vlink(current);
    }
    if (head == null) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, head);
    }
    if (current == null) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, current);
    }
    return 2;
}

/* node.insert_before (insert a node in a list) */

static int lua_nodelib_insert_before(lua_State * L)
{
    halfword head, current, n, t;
    if (lua_gettop(L) < 3) {
        luaL_error(L, "Not enough arguments for node.insert_before()");
    }
    if (lua_isnil(L, 3)) {
        lua_pop(L, 1);
        return 2;
    } else {
        n = *(check_isnode(L, 3));
    }
    if (lua_isnil(L, 1)) {      /* no head */
        vlink(n) = null;
        alink(n) = null;
        lua_nodelib_push_fast(L, n);
        lua_pushvalue(L, -1);
        return 2;
    } else {
        head = *(check_isnode(L, 1));
    }
    if (lua_isnil(L, 2)) {
        current = tail_of_list(head);
    } else {
        current = *(check_isnode(L, 2));
    }
    if (head != current) {
        t = alink(current);
        if (t == null || vlink(t) != current) {
            set_t_to_prev(head, current);
            if (t == null) {    /* error! */
                luaL_error(L, "Attempt to node.insert_before() a non-existing node");
            }
        }
        couple_nodes(t, n);
    }
    couple_nodes(n, current);
    if (head == current) {
        lua_nodelib_push_fast(L, n);
    } else {
        lua_nodelib_push_fast(L, head);
    }
    lua_nodelib_push_fast(L, n);
    return 2;
}

/* node.direct.insert_before */

static int lua_nodelib_direct_insert_before(lua_State * L)
{
    halfword head, current, n, t;
    n = (halfword) lua_tonumber(L,3);
    if (n == null){
        /* no node */
        lua_pop(L, 1);
        return 2 ;
    }
    head = (halfword) lua_tonumber(L,1);
    current = (halfword) lua_tonumber(L,2);
    /* no head, ignore current */
    if (head == null) {
        vlink(n) = null;
        alink(n) = null;
        lua_pushnumber(L, n);
        lua_pushvalue(L, -1);
        /* n, n */
        return 2;
    }
    /* no current */
    if (current == null)
        current = tail_of_list(head);
    if (head != current) {
        t = alink(current);
        if (t == null || vlink(t) != current)
            set_t_to_prev(head, current);
        couple_nodes(t, n);
    }
    couple_nodes(n, current);
    if (head == current) {
        lua_pushnumber(L, n);
    } else {
        lua_pushnumber(L, head);
    }
    lua_pushnumber(L, n);
    return 2;
}

/* node.insert_after */

static int lua_nodelib_insert_after(lua_State * L)
{
    halfword head, current, n;
    if (lua_gettop(L) < 3) {
        luaL_error(L, "Not enough arguments for node.insert_after()");
    }
    if (lua_isnil(L, 3)) {
        lua_pop(L, 1);
        return 2;
    } else {
        n = *(check_isnode(L, 3));
    }
    if (lua_isnil(L, 1)) {      /* no head */
        vlink(n) = null;
        alink(n) = null;
        lua_nodelib_push_fast(L, n);
        lua_pushvalue(L, -1);
        return 2;
    } else {
        head = *(check_isnode(L, 1));
    }
    if (lua_isnil(L, 2)) {
        current = head;
        while (vlink(current) != null)
            current = vlink(current);
    } else {
        current = *(check_isnode(L, 2));
    }
    try_couple_nodes(n, vlink(current));
    couple_nodes(current, n);

    lua_pop(L, 2);
    lua_nodelib_push_fast(L, n);
    return 2;
}

/* node.direct.insert_after */

static int lua_nodelib_direct_insert_after(lua_State * L)
{
    halfword head, current, n;
    /*[head][current][new]*/
    n = (halfword) lua_tonumber(L,3);
    if (n == null) {
        /* no node */
        return 2 ;
    }
    head = (halfword) lua_tonumber(L,1);
    current = (halfword) lua_tonumber(L,2);
    if (head == null) {
        /* no head, ignore current */
        vlink(n) = null;
        alink(n) = null;
        lua_pushnumber(L,n);
        lua_pushvalue(L, -1);
        /* n, n */
        return 2;
    }
    if (current == null) {
        /* no current */
        current = head;
        while (vlink(current) != null)
            current = vlink(current);
    }
    try_couple_nodes(n, vlink(current));
    couple_nodes(current, n);
    lua_pop(L, 2);
    lua_pushnumber(L, n);
    return 2;
}

/* node.copy_list */

static int lua_nodelib_copy_list(lua_State * L)
{
    halfword n, s = null;
    halfword m;
    if (lua_isnil(L, 1))
        return 1;               /* the nil itself */
    n = *check_isnode(L, 1);
    if ((lua_gettop(L) > 1) && (!lua_isnil(L,2)))
        s = *check_isnode(L, 2);
    m = do_copy_node_list(n, s);
    lua_pushnumber(L, m);
    lua_nodelib_push(L);
    return 1;
}

/* node.direct.copy_list */

static int lua_nodelib_direct_copy_list(lua_State * L)
{
    halfword n, s, m;
    n = (halfword) lua_tonumber(L,1);
    if (n == null) {
        lua_pushnil(L);
    } else {
        s = (halfword) lua_tonumber(L,2);
        if (s == null) {
            m = do_copy_node_list(n,null);
        } else {
            m = do_copy_node_list(n,s);
        }
        lua_pushnumber(L,m);
    }
    return 1;
}

/* node.copy (deep copy) */

static int lua_nodelib_copy(lua_State * L)
{
    halfword *n;
    halfword m;
    if (lua_isnil(L, 1))
        return 1;               /* the nil itself */
    n = check_isnode(L, 1);
    m = copy_node(*n);
    lua_nodelib_push_fast(L, m);
    return 1;
}

/* node.direct.copy (deep copy) */

static int lua_nodelib_direct_copy(lua_State * L)
{
    halfword n;
    halfword m;
    if (lua_isnil(L, 1))
        return 1;               /* the nil itself */
    /* beware, a glue node can have number 0 (zeropt) so we cannot test for null) */
    n = (halfword) lua_tonumber(L, 1);
    m = copy_node(n);
    lua_pushnumber(L, m);
    return 1;
}


/* node.write (output a node to tex's processor) */

static int lua_nodelib_append(lua_State * L)
{
    halfword *n;
    halfword m;
    int i, j;
    j = lua_gettop(L);
    for (i = 1; i <= j; i++) {
        n = check_isnode(L, i);
        m = *n;
        tail_append(m);
        while (vlink(m) != null) {
            m = vlink(m);
            tail_append(m);
        }
    }
    return 0;
}

/* node.direct.write */

static int lua_nodelib_direct_append(lua_State * L)
{
    halfword n;
    halfword m;
    int i, j;
    j = lua_gettop(L);
    for (i = 1; i <= j; i++) {
        n = (halfword) lua_tonumber(L,i); /*lua_getnumber(L, i);*/
        if (n != null) {
            m = n ;
            tail_append(m);
            while (vlink(m) != null) {
                m = vlink(m);
                tail_append(m);
            }
        }
    }
    return 0;
}

/* node.last */

static int lua_nodelib_last_node(lua_State * L)
{
    halfword m;
    m = pop_tail();
    /* can be: lua_nodelib_push_fast(L, m); */
    lua_pushnumber(L, m);
    lua_nodelib_push(L);
    return 1;
}

/* node.direct.last */

static int lua_nodelib_direct_last_node(lua_State * L)
{
    halfword m;
    m = pop_tail();
    lua_pushnumber(L, m);
    return 1;
}

/* node.hpack (build a hbox) */

static int lua_nodelib_hpack(lua_State * L)
{
    halfword n, p;
    const char *s;
    int w = 0;
    int m = 1;
    int d = -1;
    n = *(check_isnode(L, 1));
    if (lua_gettop(L) > 1) {
        w = (int) lua_tointeger(L, 2);
        if (lua_gettop(L) > 2) {
            if (lua_type(L, 3) == LUA_TSTRING) {
                s = lua_tostring(L, 3);
                if (luaS_ptr_eq(s, additional)) {
                    m = 1;
                } else if (luaS_ptr_eq(s, exactly)) {
                    m = 0;
                } else if (luaS_ptr_eq(s, cal_expand_ratio)) {
                    m = 2;
                } else if (luaS_ptr_eq(s, subst_ex_font)) {
                    m = 3;
                } else {
                    luaL_error(L, "3rd argument should be either additional or exactly");
                }
            } else if (lua_type(L, 3) == LUA_TNUMBER) {
                m=(int)lua_tonumber(L, 3);
            } else {
                lua_pushstring(L, "incorrect 3rd argument");
            }
            if (lua_gettop(L) > 3) {
                if (lua_type(L, 4) == LUA_TSTRING) {
                    d = nodelib_getdir(L, 4, 1);
                } else {
                    lua_pushstring(L, "incorrect 4th argument");
                }
            }
        }
    }
    p = hpack(n, w, m, d);
    lua_nodelib_push_fast(L, p);
    lua_pushnumber(L, last_badness);
    return 2;
}

/* node.direct.hpack */

static int lua_nodelib_direct_hpack(lua_State * L)
{
    halfword n, p;
    const char *s;
    int w = 0;
    int m = 1;
    int d = -1;
    n = (halfword) lua_tonumber(L,1);
    /* could be macro */
    if (lua_gettop(L) > 1) {
        w = (int) lua_tointeger(L, 2);
        if (lua_gettop(L) > 2) {
            if (lua_type(L, 3) == LUA_TSTRING) {
                s = lua_tostring(L, 3);
                if (luaS_ptr_eq(s, additional)) {
                    m = 1;
                } else if (luaS_ptr_eq(s, exactly)) {
                    m = 0;
                } else if (luaS_ptr_eq(s, cal_expand_ratio)) {
                    m = 2;
                } else if (luaS_ptr_eq(s, subst_ex_font)) {
                    m = 3;
                } else {
                    luaL_error(L, "3rd argument should be either additional or exactly");
                }
            } else if (lua_type(L, 3) == LUA_TNUMBER) {
                m=(int)lua_tonumber(L, 3);
            } else {
                lua_pushstring(L, "incorrect 3rd argument");
            }
            if (lua_gettop(L) > 3) {
                if (lua_type(L, 4) == LUA_TSTRING) {
                    d = nodelib_getdir(L, 4, 1);
                } else {
                    lua_pushstring(L, "incorrect 4th argument");
                }
            }
        }
    }
    /* till here */
    p = hpack(n, w, m, d);
    lua_pushnumber(L, p);
    lua_pushnumber(L, last_badness);
    return 2;
}

/* node.vpack (build a vbox) */

static int lua_nodelib_vpack(lua_State * L)
{
    halfword n, p;
    const char *s;
    int w = 0;
    int m = 1;
    int d = -1;
    n = *(check_isnode(L, 1));
    if (lua_gettop(L) > 1) {
        w = (int) lua_tointeger(L, 2);
        if (lua_gettop(L) > 2) {
            if (lua_type(L, 3) == LUA_TSTRING) {
                s = lua_tostring(L, 3);
                if (luaS_ptr_eq(s, additional)) {
                    m = 1;
                } else if (luaS_ptr_eq(s, exactly)) {
                    m = 0;
                } else {
                    luaL_error(L, "3rd argument should be either additional or exactly");
                }

                if (lua_gettop(L) > 3) {
                    if (lua_type(L, 4) == LUA_TSTRING) {
                        d = nodelib_getdir(L, 4, 1);
                    } else {
                        lua_pushstring(L, "incorrect 4th argument");
                    }
                }
            }

            else if (lua_type(L, 3) == LUA_TNUMBER) {
                m=(int)lua_tonumber(L, 3);
            } else {
                lua_pushstring(L, "incorrect 3rd argument");
            }
        }
    }
    p = vpackage(n, w, m, max_dimen, d);
    lua_nodelib_push_fast(L, p);
    lua_pushnumber(L, last_badness);
    return 2;
}

/* node.direct.vpack */

static int lua_nodelib_direct_vpack(lua_State * L)
{
    halfword n, p;
    const char *s;
    int w = 0;
    int m = 1;
    int d = -1;
    n = (halfword) lua_tonumber(L,1);
    if (lua_gettop(L) > 1) {
        w = (int) lua_tointeger(L, 2);
        if (lua_gettop(L) > 2) {
            if (lua_type(L, 3) == LUA_TSTRING) {
                s = lua_tostring(L, 3);
                if (luaS_ptr_eq(s, additional)) {
                    m = 1;
                } else if (luaS_ptr_eq(s, exactly)) {
                    m = 0;
                } else {
                    luaL_error(L, "3rd argument should be either additional or exactly");
                }

                if (lua_gettop(L) > 3) {
                    if (lua_type(L, 4) == LUA_TSTRING) {
                        d = nodelib_getdir(L, 4, 1);
                    } else {
                        lua_pushstring(L, "incorrect 4th argument");
                    }
                }
            }

            else if (lua_type(L, 3) == LUA_TNUMBER) {
                m=(int)lua_tonumber(L, 3);
            } else {
                lua_pushstring(L, "incorrect 3rd argument");
            }
        }
    }
    p = vpackage(n, w, m, max_dimen, d);
    lua_pushnumber(L, p);
    lua_pushnumber(L, last_badness);
    return 2;
}

/* node.dimensions (of a hlist or vlist) */

static int lua_nodelib_dimensions(lua_State * L)
{
    int top;
    top = lua_gettop(L);
    if (top > 0) {
        scaled_whd siz;
        glue_ratio g_mult = 1.0;
        int g_sign = normal;
        int g_order = normal;
        int i = 1;
        int d = -1;
        halfword n = null, p = null;
        /* maybe be more restrictive: LUA_TNUMBER i.e. it's not good to mix numbers and strings with digits */
        if (lua_isnumber(L, 1)) {
            if (top < 4) {
                lua_pushnil(L);
                return 1;
            }
            i += 3;
            g_mult = (glue_ratio) lua_tonumber(L, 1);
            g_sign=(int)lua_tonumber(L, 2);
            g_order=(int)lua_tonumber(L, 3);
        }
        n = *(check_isnode(L, i));
        if (lua_gettop(L) > i && !lua_isnil(L, (i + 1))) {
            if (lua_type(L, (i + 1)) == LUA_TSTRING) {
                d = nodelib_getdir(L, (i + 1), 1);
            } else {
                p = *(check_isnode(L, (i + 1)));
            }
        }
        if (lua_gettop(L) > (i + 1) && lua_type(L, (i + 2)) == LUA_TSTRING) {
            d = nodelib_getdir(L, (i + 2), 1);
        }
        siz = natural_sizes(n, p, g_mult, g_sign, g_order, d);
        lua_pushnumber(L, siz.wd);
        lua_pushnumber(L, siz.ht);
        lua_pushnumber(L, siz.dp);
        return 3;
    } else {
        luaL_error(L, "missing argument to 'dimensions' (node expected)");
    }
    return 0;                   /* not reached */
}

/* node.direct.dimensions*/

static int lua_nodelib_direct_dimensions(lua_State * L)
{
    int top;
    top = lua_gettop(L);
    if (top > 0) {
        scaled_whd siz;
        glue_ratio g_mult = 1.0;
        int g_sign = normal;
        int g_order = normal;
        int i = 1;
        int d = -1;
        halfword n = null, p = null;
        if (top > 3) {
            i += 3;
            g_mult = (glue_ratio) lua_tonumber(L, 1);
            g_sign=(int)lua_tonumber(L, 2);
            g_order=(int)lua_tonumber(L, 3);
        }
        n = (halfword) lua_tonumber(L,i);
        if (lua_gettop(L) > i && !lua_isnil(L, (i + 1))) {
            if (lua_type(L, (i + 1)) == LUA_TSTRING) {
                d = nodelib_getdir(L, (i + 1), 1);
            } else {
                p = (halfword) lua_tonumber(L,i+1);
            }
        }
        if (lua_gettop(L) > (i + 1) && lua_type(L, (i + 2)) == LUA_TSTRING)
            d = nodelib_getdir(L, (i + 2), 1);
        siz = natural_sizes(n, p, g_mult, g_sign, g_order, d);
        lua_pushnumber(L, siz.wd);
        lua_pushnumber(L, siz.ht);
        lua_pushnumber(L, siz.dp);
        return 3;
    } else {
        luaL_error(L, "missing argument to 'dimensions' (node expected)");
    }
    return 0;                   /* not reached */
}

/* node.mlist_to_hlist (create a hlist from a formula) */

static int lua_nodelib_mlist_to_hlist(lua_State * L)
{
    halfword n;
    int w;
    boolean m;
    n = *(check_isnode(L, 1));
    w = luaL_checkoption(L, 2, "text", math_style_names);
    luaL_checkany(L, 3);
    m = lua_toboolean(L, 3);
    mlist_to_hlist_args(n, w, m);
    alink(vlink(temp_head)) = null; /*hh-ls */
    lua_nodelib_push_fast(L, vlink(temp_head));
    return 1;
}

/* node.family_font */

static int lua_nodelib_mfont(lua_State * L)
{
    int f, s;
    f = (int) luaL_checkinteger(L, 1);
    if (lua_gettop(L) == 2)
        s = (int) lua_tointeger(L, 2);  /* this should be a multiple of 256 ! */
    else
        s = 0;
    lua_pushnumber(L, fam_fnt(f, s));
    return 1;
}

/*
    This function is similar to |get_node_type_id|, for field
    identifiers.  It has to do some more work, because not all
    identifiers are valid for all types of nodes.

    If really needed we can optimize this one using a big if ..
    .. else like with the getter and setter.

*/

static int get_node_field_id(lua_State * L, int n, int node)
{
    int t = type(node);
    const char *s = lua_tostring(L, n);

    if (s == NULL)
        return -2;

    if (luaS_ptr_eq(s, next)) {
        return 0;
    } else if (luaS_ptr_eq(s, id)) {
        return 1;
    } else if (luaS_ptr_eq(s, subtype)) {
        if (nodetype_has_subtype(t)) {
            return 2;
        }
    } else if (luaS_ptr_eq(s, attr)) {
        if (nodetype_has_attributes(t)) {
            return 3;
        }
    } else if (luaS_ptr_eq(s, prev)) {
        if (nodetype_has_prev(t)) {
            return -1;
        }
    } else {
        int j;
        const char **fields = node_data[t].fields;
        if (t == whatsit_node) {
            fields = whatsit_node_data[subtype(node)].fields;
        }
        if (luaS_ptr_eq(s, list)) {
            /* head and list are equivalent; we don't catch extra virtual fields */
            s = luaS_head_ptr;
        }
        if (fields != NULL) {
            for (j = 0; fields[j] != NULL; j++) {
                if (strcmp(s, fields[j]) == 0) {
                    return j + 3;
                }
            }
        }
    }
    return -2;
}

/* node.has_field */

static int lua_nodelib_has_field(lua_State * L)
{
    int i = -2;
    if (!lua_isnil(L, 1))
        i = get_node_field_id(L, 2, *(check_isnode(L, 1)));
    lua_pushboolean(L, (i != -2));
    return 1;
}

/* node.direct.has_field */

static int lua_nodelib_direct_has_field(lua_State * L)
{
    int i = -2;
    halfword n;
    n = (halfword) lua_tonumber(L, 1);
    if (n != null)
        i = get_node_field_id(L, 2, n);
    lua_pushboolean(L, (i != -2));
    return 1;
}

/* fetch the list of valid node types */

static int do_lua_nodelib_types(lua_State * L, node_info * data)
{
    int i;
    lua_newtable(L);
    for (i = 0; data[i].id != -1; i++) {
        lua_pushstring(L, data[i].name);
        lua_rawseti(L, -2, data[i].id);
    }
    return 1;
}

/* node.types */

static int lua_nodelib_types(lua_State * L)
{
    return do_lua_nodelib_types(L, node_data);
}

/* node.whatsits */

static int lua_nodelib_whatsits(lua_State * L)
{
    return do_lua_nodelib_types(L, whatsit_node_data);
}

/* node.fields (fetch the list of valid fields) */

static int lua_nodelib_fields(lua_State * L)
{
    int i = -1;
    int offset = 2;
    const char **fields;
    int t = get_valid_node_type_id(L, 1);
    if (t == whatsit_node) {
        t = get_valid_node_subtype_id(L, 2);
        fields = whatsit_node_data[t].fields;
    } else {
        fields = node_data[t].fields;
    }
    lua_checkstack(L, 2);
    lua_newtable(L);
    lua_pushstring(L, "next"); /* next, id, subtype, prev could be predefined string */
    lua_rawseti(L, -2, 0);
    lua_pushstring(L, "id");
    lua_rawseti(L, -2, 1);
    if (nodetype_has_subtype(t)) {
      lua_pushstring(L, "subtype");
      lua_rawseti(L, -2, 2);
      offset++;
    }
    if (fields != NULL) {
        if (nodetype_has_prev(t)) {
          lua_pushstring(L, "prev");
          lua_rawseti(L, -2, -1);
        }
        for (i = 0; fields[i] != NULL; i++) {
            lua_pushstring(L, fields[i]);
            lua_rawseti(L, -2, (i + offset));
        }
    }
    return 1;
}

/* node.slide (find the end of a list and add prev links) */

static int lua_nodelib_slide(lua_State * L)
{
    halfword *n;
    halfword t;
    if (lua_isnil(L, 1))
        return 1;               /* the nil itself */
    n = check_isnode(L, 1);
    t = *n;
    if (t == null)
        return 1;               /* the old userdata */
    /* alink(t) = null; */ /* don't do this, |t|'s |alink| may be a valid pointer */
    while (vlink(t) != null) {
        alink(vlink(t)) = t;
        t = vlink(t);
    }
    lua_nodelib_push_fast(L, t);
    return 1;
}

/* node.direct.slide */

static int lua_nodelib_direct_slide(lua_State * L)
{
    halfword n;
    n = (halfword) lua_tonumber(L, 1);
    if (n == null) {
        lua_pushnil(L);
    } else {
        while (vlink(n) != null) {
            alink(vlink(n)) = n;
            n = vlink(n);
        }
        lua_pushnumber(L, n);
    }
    return 1;
}

/* node.tail (find the end of a list) */

static int lua_nodelib_tail(lua_State * L)
{
    halfword *n;
    halfword t;
    if (lua_isnil(L, 1))
        return 1;               /* the nil itself */
    n = check_isnode(L, 1);
    t = *n;
    if (t == null)
        return 1;               /* the old userdata */
    while (vlink(t) != null)
        t = vlink(t);
    lua_nodelib_push_fast(L, t);
    return 1;
}

/* node.direct.tail */

static int lua_nodelib_direct_tail(lua_State * L)
{
    halfword n;
    n = (halfword) lua_tonumber(L, 1);
    if (n == null) {
        lua_pushnil(L);
    } else {
        while (vlink(n) != null)
            n = vlink(n);
        lua_pushnumber(L, n);
    }
    return 1;
}

/* node.end_of_math (skip over math and return last) */

static int lua_nodelib_end_of_math(lua_State * L)
{
    halfword *n;
    halfword t;
    if (lua_isnil(L, 1))
        return 1;               /* the nil itself */
    n = check_isnode(L, 1);
    t = *n;
    if (t == null)
        return 1;               /* the old userdata */
    while (vlink(t) != null) {
        t = vlink(t); /* skip first node */
        if (t && type(t)==math_node) {
            lua_nodelib_push_fast(L, t);
            return 1;
        }
    }
    return 0;
}

/* node.direct.end_of_math */

static int lua_nodelib_direct_end_of_math(lua_State * L)
{
    halfword n;
    n = (halfword) lua_tonumber(L, 1);
    if (n != null) {
        while (vlink(n) != null) {
            n = vlink(n); /* skip first node */
            if (n && type(n)==math_node) {
                lua_pushnumber(L, n);
                return 1;
            }
        }
    }
    lua_pushnil(L);
    return 0;
}

/* node.has_attribute (gets attribute) */

static int lua_nodelib_has_attribute(lua_State * L)
{
    halfword *n;
    int i, val;
    n = check_isnode(L, 1);
    if (n != NULL) {
        i = (int) lua_tointeger(L, 2);
        val = (int) luaL_optinteger(L, 3, UNUSED_ATTRIBUTE);
        if ((val = has_attribute(*n, i, val)) > UNUSED_ATTRIBUTE) {
            lua_pushnumber(L, val);
            return 1;
        }
    }
    lua_pushnil(L);
    return 1;
}

/* node.direct.has_attribute */

static int lua_nodelib_direct_has_attribute(lua_State * L)
{
    halfword n;
    int i, val;
    n = (halfword) lua_tonumber(L, 1);
    if (n != null) {
        i = (int) lua_tointeger(L, 2);
        val = (int) luaL_optinteger(L, 3, UNUSED_ATTRIBUTE);
        if ((val = has_attribute(n, i, val)) > UNUSED_ATTRIBUTE) {
            lua_pushnumber(L, val);
            return 1;
        }
    }
    lua_pushnil(L);
    return 1;
}

/* node.set_attribute */

static int lua_nodelib_set_attribute(lua_State * L)
{
    halfword *n;
    int i, val;
    if (lua_gettop(L) == 3) {
        i = (int) lua_tointeger(L, 2);
        val = (int) lua_tointeger(L, 3);
        n = check_isnode(L, 1);
        if (val == UNUSED_ATTRIBUTE) {
            (void) unset_attribute(*n, i, val);
        } else {
            set_attribute(*n, i, val);
        }
    } else {
        luaL_error(L, "incorrect number of arguments");
    }
    return 0;
}

/* node.direct.set_attribute */

static int lua_nodelib_direct_set_attribute(lua_State * L)
{
    halfword n;
    int i, val;
    n = (halfword) lua_tonumber(L, 1);
    if (n == null)
        return 0;
    if (lua_gettop(L) == 3) {
        i = (int) lua_tointeger(L, 2);
        val = (int) lua_tointeger(L, 3);
        if (val == UNUSED_ATTRIBUTE) {
            (void) unset_attribute(n, i, val);
        } else {
            set_attribute(n, i, val);
        }
    } else {
        luaL_error(L, "incorrect number of arguments");
    }
    return 0;
}

/* node.unset_attribute */

static int lua_nodelib_unset_attribute(lua_State * L)
{
    halfword *n;
    int i, val, ret;
    if (lua_gettop(L) <= 3) {
        i=(int)luaL_checknumber(L, 2);
        val=(int)luaL_optnumber(L, 3, UNUSED_ATTRIBUTE);
        n = check_isnode(L, 1);
        ret = unset_attribute(*n, i, val);
        if (ret > UNUSED_ATTRIBUTE) {
            lua_pushnumber(L, ret);
        } else {
            lua_pushnil(L);
        }
        return 1;
    } else {
        return luaL_error(L, "incorrect number of arguments");
    }
}

/* node.direct.unset_attribute */

static int lua_nodelib_direct_unset_attribute(lua_State * L)
{
    halfword n;
    int i, val, ret;
    n = (halfword) lua_tonumber(L, 1);
    if (n == null) {
        lua_pushnil(L);
    } else if (lua_gettop(L) <= 3) { /* a useless test, we never test for that elsewhere */
        i=(int)luaL_checknumber(L, 2);
        val=(int)luaL_optnumber(L, 3, UNUSED_ATTRIBUTE);
        ret = unset_attribute(n, i, val);
        if (ret > UNUSED_ATTRIBUTE) {
            lua_pushnumber(L, ret);
        } else {
            lua_pushnil(L);
        }
    } else { /* can go */
        return luaL_error(L, "incorrect number of arguments");
    }
    return 1;
}

/* iteration */

static int nodelib_aux_nil(lua_State * L)
{
    lua_pushnil(L);
    return 1;
}

/* node.traverse_id */

static int nodelib_aux_next_filtered(lua_State * L)
{
    halfword t;        /* traverser */
    halfword *a;
    int i = (int) lua_tointeger(L, lua_upvalueindex(1));
    if (lua_isnil(L, 2)) {      /* first call */
        t = *check_isnode(L, 1);
        lua_settop(L,1);
    } else {
        t = *check_isnode(L, 2);
        t = vlink(t);
        lua_settop(L,2);
    }
    while (t != null && type(t) != i) {
        t = vlink(t);
    }
    if (t == null) {
        lua_pushnil(L);
    } else {
        fast_metatable_top(t);
    }
    return 1;
}

static int lua_nodelib_traverse_filtered(lua_State * L)
{
    halfword n;
    if (lua_isnil(L, 2)) {
        lua_pushcclosure(L, nodelib_aux_nil, 0);
        return 1;
    }
    n = *(check_isnode(L, 2));
    lua_pop(L, 1);              /* the node, integer remains */
    lua_pushcclosure(L, nodelib_aux_next_filtered, 1);
    lua_nodelib_push_fast(L, n);
    lua_pushnil(L);
    return 3;
}

/* node.direct.traverse_id */

static int nodelib_direct_aux_next_filtered(lua_State * L)
{
    halfword t;        /* traverser */
    int i = (int) lua_tointeger(L, lua_upvalueindex(1));
    if (lua_isnil(L, 2)) {      /* first call */
        t = lua_tonumber(L,1) ;
        lua_settop(L,1);
    } else {
        t = lua_tonumber(L,2) ;
        t = vlink(t);
        lua_settop(L,2);
    }
    while (t != null && type(t) != i) {
        t = vlink(t);
    }
    if (t == null) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L,t);
    }
    return 1;
}

static int lua_nodelib_direct_traverse_filtered(lua_State * L)
{
    halfword n;
    if (lua_isnil(L, 2)) {
        lua_pushcclosure(L, nodelib_aux_nil, 0);
        return 1;
    }
    n = (halfword) lua_tonumber(L, 2);
    if (n == null)
        return 0;
    n = (halfword) lua_tonumber(L, 2);
    lua_pop(L, 1);
    lua_pushcclosure(L, nodelib_direct_aux_next_filtered, 1);
    lua_pushnumber(L,n);
    lua_pushnil(L);
    return 3;
}

/* node.traverse */

static int nodelib_aux_next(lua_State * L)
{
    halfword t;        /* traverser */
    halfword *a;        /* a or *a */
    if (lua_isnil(L, 2)) {      /* first call */
        t = *check_isnode(L, 1);
        lua_settop(L,1);
    } else {
        t = *check_isnode(L, 2);
        t = vlink(t);
        lua_settop(L,2);
    }
    if (t == null) {
        lua_pushnil(L);
    } else {
        fast_metatable_top(t);
    }
    return 1;
}

static int lua_nodelib_traverse(lua_State * L)
{
    halfword n;
    if (lua_isnil(L, 1)) {
        lua_pushcclosure(L, nodelib_aux_nil, 0);
        return 1;
    }
    n = *(check_isnode(L, 1));
    lua_pushcclosure(L, nodelib_aux_next, 0);
    lua_nodelib_push_fast(L, n);
    lua_pushnil(L);
    return 3;
}

/* node.direct.traverse */

static int nodelib_direct_aux_next(lua_State * L)
{
    halfword t;        /* traverser */
    /*int i = (int) lua_tointeger(L, lua_upvalueindex(1));*/
    if (lua_isnil(L, 2)) {      /* first call */
        t = lua_tonumber(L,1) ;
        lua_settop(L,1);
    } else {
        t = lua_tonumber(L,2) ;
        t = vlink(t);
        lua_settop(L,2);
    }
    if (t == null) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L,t);
    }
    return 1;
}

static int lua_nodelib_direct_traverse(lua_State * L)
{
    halfword n;
    if (lua_isnil(L, 1)) {
        lua_pushcclosure(L, nodelib_aux_nil, 0);
        return 1;
    }
    n = (halfword) lua_tonumber(L, 1);
    if (n == null) {
        lua_pushcclosure(L, nodelib_aux_nil, 0);
        return 1;
    }
    lua_pushcclosure(L, nodelib_direct_aux_next, 0);
    lua_pushnumber(L,n);
    lua_pushnil(L);
    return 3;
}


/* counting */

static int do_lua_nodelib_count(lua_State * L, halfword match, int i, halfword first1)
{
    int count = 0;
    int t = first1;
    while (t != match) {
        if (i < 0 || type(t) == i)
            count++;
        t = vlink(t);
    }
    lua_pushnumber(L, count);
    return 1;
}

/* node.length */

static int lua_nodelib_length(lua_State * L)
{
    halfword n;
    halfword m = null;
    if (lua_isnil(L, 1)) {
        lua_pushnumber(L, 0);
        return 1;
    }
    n = *(check_isnode(L, 1));
    if (lua_gettop(L) == 2)
        m = *(check_isnode(L, 2));
    return do_lua_nodelib_count(L, m, -1, n);
}

/* node.direct.length */

static int lua_nodelib_direct_length(lua_State * L)
{
    halfword n, m;
    n = (halfword) lua_tonumber(L, 1);
    if (n == 0) {
        lua_pushnumber(L, 0);
        return 1;
    }
    m = (halfword) lua_tonumber(L, 2);
    return do_lua_nodelib_count(L, m, -1, n);
}

/* node.count */

static int lua_nodelib_count(lua_State * L)
{
    halfword n;
    halfword m = null;
    int i = -1;
    i = (int) lua_tointeger(L, 1);
    if (lua_isnil(L, 2)) {
        lua_pushnumber(L, 0);
        return 1;
    }
    n = *(check_isnode(L, 2));
    if (lua_gettop(L) == 3)
        m = *(check_isnode(L, 3));
    return do_lua_nodelib_count(L, m, i, n);
}

/* node.direct.count */

static int lua_nodelib_direct_count(lua_State * L)
{
    return do_lua_nodelib_count(L,
        (halfword) lua_tonumber(L, 3), /* m */
        (int) lua_tointeger(L, 1),     /* i */
        (halfword) lua_tonumber(L, 2)  /* n */
    );
}

/* directions (could be a macro) */

static void nodelib_pushdir(lua_State * L, int n, boolean dirnode)
{
    if (dirnode) {
        lua_pushstring(L, dir_strings[n+64]);
    } else {
        lua_pushstring(L, dir_strings[n+64]+1);
    }
}

/* getting and setting fields (helpers) */

static int nodelib_getlist(lua_State * L, int n)
{
    halfword *m;
    if (lua_isuserdata(L, n)) {
        m = check_isnode(L, n);
        return *m;
    } else {
        return null;
    }
}

int nodelib_getdir(lua_State * L, int n, int absolute_only)
{
    if (lua_type(L, n) == LUA_TSTRING) {
        const char *s = lua_tostring(L, n);
        RETURN_DIR_VALUES(TRT);
        RETURN_DIR_VALUES(TLT);
        RETURN_DIR_VALUES(LTL);
        RETURN_DIR_VALUES(RTT);
        luaL_error(L, "Bad direction specifier %s", s);
    } else {
        luaL_error(L, "Direction specifiers have to be strings");
    }
    return 0;
}

static str_number nodelib_getstring(lua_State * L, int a)
{
    size_t k;
    const char *s = lua_tolstring(L, a, &k);
    return maketexlstring(s, k);
}

static void nodelib_setattr(lua_State * L, int stackindex, halfword n)
{
    halfword p;
    p = nodelib_getlist(L, stackindex);
    if (node_attr(n) != p) {
        if (node_attr(n) != null)
            delete_attribute_ref(node_attr(n));
        node_attr(n) = p;
    if (p != null)
        attr_list_ref(p)++;
    }
}

static int nodelib_cantset(lua_State * L, int n, const char *s)
{
    luaL_error(L,"You cannot set field %s in a node of type %s",s,node_data[type(n)].name);
    return 0;
}

/* node.direct.getfield */

static void lua_nodelib_getfield_whatsit(lua_State * L, int n, const char *s)
{
    int t ;
    t = subtype(n);

    if (t == dir_node) {
        if (luaS_ptr_eq(s, dir)) {
            nodelib_pushdir(L, dir_dir(n), true);
        } else if (luaS_ptr_eq(s, level)) {
            lua_pushnumber(L, dir_level(n));
        } else if (luaS_ptr_eq(s, dvi_ptr)) {
            lua_pushnumber(L, dir_dvi_ptr(n));
        } else if (luaS_ptr_eq(s, dir_h)) {
            lua_pushnumber(L, dir_dvi_h(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == user_defined_node) {
        if (luaS_ptr_eq(s, user_id)) {
            lua_pushnumber(L, user_node_id(n));
        } else if (luaS_ptr_eq(s, type)) {
            lua_pushnumber(L, user_node_type(n));
        } else if (luaS_ptr_eq(s, value)) {
            switch (user_node_type(n)) {
            case 'a':
                /* fast_metatable(n) */
                nodelib_pushlist(L, user_node_value(n));
                break;
            case 'd':
                lua_pushnumber(L, user_node_value(n));
                break;
            case 'n':
                /* fast_metatable(n) */
                nodelib_pushlist(L, user_node_value(n));
                break;
            case 's':
                nodelib_pushstring(L, user_node_value(n));
                break;
            case 't':
                tokenlist_to_lua(L, user_node_value(n));
                break;
            default:
                lua_pushnumber(L, user_node_value(n));
                break;
            }
        } else {
            lua_pushnil(L);
        }
    } else if (t == local_par_node) {
        if (luaS_ptr_eq(s, pen_inter)) {
            lua_pushnumber(L, local_pen_inter(n));
        } else if (luaS_ptr_eq(s, pen_broken)) {
            lua_pushnumber(L, local_pen_broken(n));
        } else if (luaS_ptr_eq(s, dir)) {
            nodelib_pushdir(L, local_par_dir(n), false);
        } else if (luaS_ptr_eq(s, box_left)) {
            /* can be: fast_metatable_or_nil(local_box_left(n)) */
            nodelib_pushlist(L, local_box_left(n));
        } else if (luaS_ptr_eq(s, box_left_width)) {
            lua_pushnumber(L, local_box_left_width(n));
        } else if (luaS_ptr_eq(s, box_right)) {
            /* can be: fast_metatable_or_nil(local_box_right(n)) */
            nodelib_pushlist(L, local_box_right(n));
        } else if (luaS_ptr_eq(s, box_right_width)) {
            lua_pushnumber(L, local_box_right_width(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == pdf_literal_node) {
        if (luaS_ptr_eq(s, mode)) {
            lua_pushnumber(L, pdf_literal_mode(n));
        } else if (luaS_ptr_eq(s, data)) {
            if (pdf_literal_type(n) == lua_refid_literal) {
                lua_rawgeti(Luas, LUA_REGISTRYINDEX, pdf_literal_data(n));
            } else {
                tokenlist_to_luastring(L, pdf_literal_data(n));
            }
        } else {
            lua_pushnil(L);
        }
    } else if (t == late_lua_node) {
        if (luaS_ptr_eq(s, name)) {
            tokenlist_to_luastring(L, late_lua_name(n));
        } else if (luaS_ptr_eq(s, data) || luaS_ptr_eq(s, string)) {
            if (late_lua_type(n) == lua_refid_literal) {
                lua_rawgeti(Luas, LUA_REGISTRYINDEX, late_lua_data(n));
            } else {
                tokenlist_to_luastring(L, late_lua_data(n));
            }
        } else {
            lua_pushnil(L);
        }
    } else if (t == pdf_annot_node) {
        if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, depth)) {
            lua_pushnumber(L, depth(n));
        } else if (luaS_ptr_eq(s, height)) {
            lua_pushnumber(L, height(n));
        } else if (luaS_ptr_eq(s, objnum)) {
            lua_pushnumber(L, pdf_annot_objnum(n));
        } else if (luaS_ptr_eq(s, data)) {
            tokenlist_to_luastring(L, pdf_annot_data(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == pdf_dest_node) {
        if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, depth)) {
            lua_pushnumber(L, depth(n));
        } else if (luaS_ptr_eq(s, height)) {
            lua_pushnumber(L, height(n));
        } else if (luaS_ptr_eq(s, named_id)) {
            lua_pushnumber(L, pdf_dest_named_id(n));
        } else if (luaS_ptr_eq(s, dest_id)) {
            if (pdf_dest_named_id(n) == 1)
                tokenlist_to_luastring(L, pdf_dest_id(n));
            else
                lua_pushnumber(L, pdf_dest_id(n));
        } else if (luaS_ptr_eq(s, dest_type)) {
            lua_pushnumber(L, pdf_dest_type(n));
        } else if (luaS_ptr_eq(s, xyz_zoom)) {
            lua_pushnumber(L, pdf_dest_xyz_zoom(n));
        } else if (luaS_ptr_eq(s, objnum)) {
            lua_pushnumber(L, pdf_dest_objnum(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == pdf_setmatrix_node) {
        if (luaS_ptr_eq(s, data)) {
            tokenlist_to_luastring(L, pdf_setmatrix_data(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == pdf_colorstack_node) {
        if (luaS_ptr_eq(s, stack)) {
            lua_pushnumber(L, pdf_colorstack_stack(n));
        } else if (luaS_ptr_eq(s, command)) {
            lua_pushnumber(L, pdf_colorstack_cmd(n));
        } else if (luaS_ptr_eq(s, data)) {
            tokenlist_to_luastring(L, pdf_colorstack_data(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == pdf_refobj_node) {
        if (luaS_ptr_eq(s, objnum)) {
            lua_pushnumber(L, pdf_obj_objnum(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == pdf_refxform_node) {
        if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, depth)) {
            lua_pushnumber(L, depth(n));
        } else if (luaS_ptr_eq(s, height)) {
            lua_pushnumber(L, height(n));
        } else if (luaS_ptr_eq(s, objnum)) {
            lua_pushnumber(L, pdf_xform_objnum(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == pdf_refximage_node) {
        if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, depth)) {
            lua_pushnumber(L, depth(n));
        } else if (luaS_ptr_eq(s, height)) {
            lua_pushnumber(L, height(n));
        } else if (luaS_ptr_eq(s, transform)) {
            lua_pushnumber(L, pdf_ximage_transform(n));
        } else if (luaS_ptr_eq(s, index)) {
            lua_pushnumber(L, pdf_ximage_index(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == write_node) {
        if (luaS_ptr_eq(s, stream)) {
            lua_pushnumber(L, write_stream(n));
        } else if (luaS_ptr_eq(s, data)) {
            tokenlist_to_lua(L, write_tokens(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == special_node) {
        if (luaS_ptr_eq(s, data)) {
            tokenlist_to_luastring(L, write_tokens(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == pdf_start_link_node) {
        if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, depth)) {
            lua_pushnumber(L, depth(n));
        } else if (luaS_ptr_eq(s, height)) {
            lua_pushnumber(L, height(n));
        } else if (luaS_ptr_eq(s, objnum)) {
            lua_pushnumber(L, pdf_link_objnum(n));
        } else if (luaS_ptr_eq(s, link_attr)) {
            tokenlist_to_luastring(L, pdf_link_attr(n));
        } else if (luaS_ptr_eq(s, action)) {
            nodelib_pushaction(L, pdf_link_action(n));
        } else {
            lua_pushnil(L);
        }
    } else if ((t == pdf_thread_node) || (t == pdf_start_thread_node)) {
        if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, depth)) {
            lua_pushnumber(L, depth(n));
        } else if (luaS_ptr_eq(s, height)) {
            lua_pushnumber(L, height(n));
        } else if (luaS_ptr_eq(s, named_id)) {
            lua_pushnumber(L, pdf_thread_named_id(n));
        } else if (luaS_ptr_eq(s, thread_id)) {
            if (pdf_thread_named_id(n) == 1) {
                tokenlist_to_luastring(L, pdf_thread_id(n));
            } else {
                lua_pushnumber(L, pdf_thread_id(n));
            }
        } else if (luaS_ptr_eq(s, thread_attr)) {
            tokenlist_to_luastring(L, pdf_thread_attr(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == open_node) {
        if (luaS_ptr_eq(s, stream)) {
            lua_pushnumber(L, write_stream(n));
        } else if (luaS_ptr_eq(s, name)) {
            nodelib_pushstring(L, open_name(n));
        } else if (luaS_ptr_eq(s, area)) {
            nodelib_pushstring(L, open_area(n));
        } else if (luaS_ptr_eq(s, ext)) {
            nodelib_pushstring(L, open_ext(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == close_node) {
        if (luaS_ptr_eq(s, stream)) {
            lua_pushnumber(L, write_stream(n));
        } else {
            lua_pushnil(L);
        }
    } else {
        lua_pushnil(L);
    }
}

static int lua_nodelib_fast_getfield(lua_State * L)
{
    /*
        the order is somewhat determined by the occurance of nodes and
        importance of fields
    */

      halfword n;
      halfword *a;
      const char *s;
      int t ;

      n = *((halfword *) lua_touserdata(L, 1));
      t = type(n);

    /*

        somenode[9] as interface to attributes ... 30% faster than has_attribute
        (1) because there is no lua function overhead, and (2) because we already
        know that we deal with a node so no checking is needed. The fast typecheck
        is needed (lua_check... is a slow down actually).

    */

    if (lua_type(L, 2) == LUA_TNUMBER) {

        halfword p;
        int i;

        if (! nodetype_has_attributes(t)) {
            lua_pushnil(L);
            return 1;
        }

        p = node_attr(n);
        if (p == null || vlink(p) == null) {
            lua_pushnil(L);
            return 1;
        }
        i = (int) lua_tointeger(L, 2);
        p = vlink(p);
        while (p != null) {
            if (attribute_id(p) == i) {
                lua_pushnumber(L, (int) attribute_value(p));
                return 1;
            } else if (attribute_id(p) > i) {
                lua_pushnil(L);
                return 1;
            }
            p = vlink(p);
        }
        lua_pushnil(L);
        return 1;
    }

    s = lua_tostring(L, 2);

    if (luaS_ptr_eq(s, id)) {
        lua_pushnumber(L, t);
    } else if (luaS_ptr_eq(s, next)) {
        fast_metatable_or_nil(vlink(n));
    } else if (luaS_ptr_eq(s, prev)) {
        fast_metatable_or_nil(alink(n));
    } else if (luaS_ptr_eq(s, attr)) {
        if (! nodetype_has_attributes(t)) {
            lua_pushnil(L);
        } else {
            nodelib_pushattr(L, node_attr(n));
        }
    } else if (t == glyph_node) {
      /* candidates: fontchar (font,char) whd (width,height,depth) */
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, font)) {
            lua_pushnumber(L, font(n));
        } else if (luaS_ptr_eq(s, char)) {
            lua_pushnumber(L, character(n));
        } else if (luaS_ptr_eq(s, xoffset)) {
            lua_pushnumber(L, x_displace(n));
        } else if (luaS_ptr_eq(s, yoffset)) {
            lua_pushnumber(L, y_displace(n));
        } else if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, char_width(font(n),character(n)));
        } else if (luaS_ptr_eq(s, height)) {
            lua_pushnumber(L, char_height(font(n),character(n)));
        } else if (luaS_ptr_eq(s, depth)) {
            lua_pushnumber(L, char_depth(font(n),character(n)));
        } else if (luaS_ptr_eq(s, expansion_factor)) {
            lua_pushnumber(L, ex_glyph(n));
        } else if (luaS_ptr_eq(s, components)) {
            fast_metatable_or_nil(lig_ptr(n));
        } else if (luaS_ptr_eq(s, lang)) {
            lua_pushnumber(L, char_lang(n));
        } else if (luaS_ptr_eq(s, left)) {
            lua_pushnumber(L, char_lhmin(n));
        } else if (luaS_ptr_eq(s, right)) {
            lua_pushnumber(L, char_rhmin(n));
        } else if (luaS_ptr_eq(s, uchyph)) {
            lua_pushnumber(L, char_uchyph(n));
        } else {
            lua_pushnil(L);
        }
    } else if ((t == hlist_node) || (t == vlist_node)) {
      /* candidates: whd (width,height,depth) */
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, list) || luaS_ptr_eq(s, head)) {
            fast_metatable_or_nil_alink(list_ptr(n));
        } else if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, height)) {
            lua_pushnumber(L, height(n));
        } else if (luaS_ptr_eq(s, depth)) {
            lua_pushnumber(L, depth(n));
        } else if (luaS_ptr_eq(s, dir)) {
            nodelib_pushdir(L, box_dir(n), false);
        } else if (luaS_ptr_eq(s, shift)) {
            lua_pushnumber(L, shift_amount(n));
        } else if (luaS_ptr_eq(s, glue_order)) {
            lua_pushnumber(L, glue_order(n));
        } else if (luaS_ptr_eq(s, glue_sign)) {
            lua_pushnumber(L, glue_sign(n));
        } else if (luaS_ptr_eq(s, glue_set)) {
            lua_pushnumber(L, (double) glue_set(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == disc_node) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, pre)) {
            fast_metatable_or_nil(vlink(pre_break(n)));
        } else if (luaS_ptr_eq(s, post)) {
            fast_metatable_or_nil(vlink(post_break(n)));
        } else if (luaS_ptr_eq(s, replace)) {
            fast_metatable_or_nil(vlink(no_break(n)));
        } else {
            lua_pushnil(L);
        }
    } else if (t == glue_node) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, spec)) {
            nodelib_pushspec(L, glue_ptr(n));
        } else if (luaS_ptr_eq(s, leader)) {
            fast_metatable_or_nil(leader_ptr(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == glue_spec_node) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, 0); /* dummy, the only one that prevents move up */
        } else if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, stretch)) {
            lua_pushnumber(L, stretch(n));
        } else if (luaS_ptr_eq(s, shrink)) {
            lua_pushnumber(L, shrink(n));
        } else if (luaS_ptr_eq(s, stretch_order)) {
            lua_pushnumber(L, stretch_order(n));
        } else if (luaS_ptr_eq(s, shrink_order)) {
            lua_pushnumber(L, shrink_order(n));
        } else if (luaS_ptr_eq(s, ref_count)) {
            lua_pushnumber(L, glue_ref_count(n));
        } else if (luaS_ptr_eq(s, writable)) {
            lua_pushboolean(L, valid_node(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == kern_node) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, kern)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, expansion_factor)) {
            lua_pushnumber(L, ex_kern(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == penalty_node) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, penalty)) {
            lua_pushnumber(L, penalty(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == rule_node) {
        /* candidates: whd (width,height,depth) */
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, height)) {
            lua_pushnumber(L, height(n));
        } else if (luaS_ptr_eq(s, depth)) {
            lua_pushnumber(L, depth(n));
        } else if (luaS_ptr_eq(s, dir)) {
            nodelib_pushdir(L, rule_dir(n), false);
        } else {
            lua_pushnil(L);
        }
    } else if (t == whatsit_node) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else {
            lua_nodelib_getfield_whatsit(L, n, s);
        }
    } else if (t == simple_noad) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, nucleus)) {
            fast_metatable_or_nil(nucleus(n));
        } else if (luaS_ptr_eq(s, sub)) {
            fast_metatable_or_nil(subscr(n));
        } else if (luaS_ptr_eq(s, sup)) {
            fast_metatable_or_nil(supscr(n));
        } else {
            lua_pushnil(L);
        }
    } else if ((t == math_char_node) || (t == math_text_char_node)) {
        /* candidates: famchar (fam,char) */
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, fam)) {
            lua_pushnumber(L, math_fam(n));
        } else if (luaS_ptr_eq(s, char)) {
            lua_pushnumber(L, math_character(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == mark_node) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, class)) {
            lua_pushnumber(L, mark_class(n));
        } else if (luaS_ptr_eq(s, mark)) {
            tokenlist_to_lua(L, mark_ptr(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == ins_node) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, cost)) {
            lua_pushnumber(L, float_cost(n));
        } else if (luaS_ptr_eq(s, depth)) {
            lua_pushnumber(L, depth(n));
        } else if (luaS_ptr_eq(s, height)) {
            lua_pushnumber(L, height(n));
        } else if (luaS_ptr_eq(s, spec)) {
            nodelib_pushspec(L, split_top_ptr(n));
        } else if ((luaS_ptr_eq(s, list)) || (luaS_ptr_eq(s, head))) {
            fast_metatable_or_nil_alink(ins_ptr(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == math_node) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, surround)) {
            lua_pushnumber(L, surround(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == fraction_noad) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, thickness(n));
        } else if (luaS_ptr_eq(s, num)) {
            fast_metatable_or_nil(numerator(n));
        } else if (luaS_ptr_eq(s, denom)) {
            fast_metatable_or_nil(denominator(n));
        } else if (luaS_ptr_eq(s, left)) {
            fast_metatable_or_nil(left_delimiter(n));
        } else if (luaS_ptr_eq(s, right)) {
            fast_metatable_or_nil(right_delimiter(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == style_node) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, style)) {
            lua_pushstring(L, math_style_names[subtype(n)]);
        } else {
            lua_pushnil(L);
        }
    } else if (t == accent_noad) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, nucleus)) {
            fast_metatable_or_nil(nucleus(n));
        } else if (luaS_ptr_eq(s, sub)) {
            fast_metatable_or_nil(subscr(n));
        } else if (luaS_ptr_eq(s, sup)) {
            fast_metatable_or_nil(supscr(n));
        } else if (luaS_ptr_eq(s, accent)) {
            fast_metatable_or_nil(accent_chr(n));
        } else if (luaS_ptr_eq(s, bot_accent)) {
            fast_metatable_or_nil(bot_accent_chr(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == fence_noad) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, delim)) {
            fast_metatable_or_nil(delimiter(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == delim_node) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, small_fam)) {
            lua_pushnumber(L, small_fam(n));
        } else if (luaS_ptr_eq(s, small_char)) {
            lua_pushnumber(L, small_char(n));
        } else if (luaS_ptr_eq(s, large_fam)) {
            lua_pushnumber(L, large_fam(n));
        } else if (luaS_ptr_eq(s, large_char)) {
            lua_pushnumber(L, large_char(n));
        } else {
            lua_pushnil(L);
        }
    } else if ((t == sub_box_node) || (t == sub_mlist_node)) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if ((luaS_ptr_eq(s, list)) || (luaS_ptr_eq(s, head))){
            fast_metatable_or_nil_alink(math_list(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == radical_noad) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, nucleus)) {
            fast_metatable_or_nil(nucleus(n));
        } else if (luaS_ptr_eq(s, sub)) {
            fast_metatable_or_nil(subscr(n));
        } else if (luaS_ptr_eq(s, sup)) {
            fast_metatable_or_nil(supscr(n));
        } else if (luaS_ptr_eq(s, left)) {
            fast_metatable_or_nil(left_delimiter(n));
        } else if (luaS_ptr_eq(s, degree)) {
            fast_metatable_or_nil(degree(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == margin_kern_node) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, glyph)) {
            fast_metatable_or_nil(margin_char(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == split_up_node) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, last_ins_ptr)) {
            fast_metatable_or_nil(last_ins_ptr(n));
        } else if (luaS_ptr_eq(s, best_ins_ptr)) {
            fast_metatable_or_nil(best_ins_ptr(n));
        } else if (luaS_ptr_eq(s, broken_ptr)) {
            fast_metatable_or_nil(broken_ptr(n));
        } else if (luaS_ptr_eq(s, broken_ins)) {
            fast_metatable_or_nil(broken_ins(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == choice_node) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, display)) {
            fast_metatable_or_nil(display_mlist(n));
        } else if (luaS_ptr_eq(s, text)) {
            fast_metatable_or_nil(text_mlist(n));
        } else if (luaS_ptr_eq(s, script)) {
            fast_metatable_or_nil(script_mlist(n));
        } else if (luaS_ptr_eq(s, scriptscript)) {
            fast_metatable_or_nil(script_script_mlist(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == inserting_node) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, last_ins_ptr)) {
            fast_metatable_or_nil(last_ins_ptr(n));
        } else if (luaS_ptr_eq(s, best_ins_ptr)) {
            fast_metatable_or_nil(best_ins_ptr(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == attribute_node) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, number)) {
            lua_pushnumber(L, attribute_id(n));
        } else if (luaS_ptr_eq(s, value)) {
            lua_pushnumber(L, attribute_value(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == adjust_node) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if ((luaS_ptr_eq(s, list)) || (luaS_ptr_eq(s, head))) {
            fast_metatable_or_nil_alink(adjust_ptr(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == action_node) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));/* dummy subtype */
        } else if (luaS_ptr_eq(s, action_type)) {
            lua_pushnumber(L, pdf_action_type(n));
        } else if (luaS_ptr_eq(s, named_id)) {
            lua_pushnumber(L, pdf_action_named_id(n));
        } else if (luaS_ptr_eq(s, action_id)) {
            if (pdf_action_named_id(n) == 1) {
                tokenlist_to_luastring(L, pdf_action_id(n));
            } else {
                lua_pushnumber(L, pdf_action_id(n));
            }
        } else if (luaS_ptr_eq(s, file)) {
            tokenlist_to_luastring(L, pdf_action_file(n));
        } else if (luaS_ptr_eq(s, new_window)) {
            lua_pushnumber(L, pdf_action_new_window(n));
        } else if (luaS_ptr_eq(s, data)) {
            tokenlist_to_luastring(L, pdf_action_tokens(n));
        } else if (luaS_ptr_eq(s, ref_count)) {
            lua_pushnumber(L, pdf_action_refcount(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == unset_node) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, height)) {
            lua_pushnumber(L, height(n));
        } else if (luaS_ptr_eq(s, depth)) {
            lua_pushnumber(L, depth(n));
        } else if (luaS_ptr_eq(s, dir)) {
            nodelib_pushdir(L, box_dir(n), false);
        } else if (luaS_ptr_eq(s, shrink)) {
            lua_pushnumber(L, glue_shrink(n));
        } else if (luaS_ptr_eq(s, glue_order)) {
            lua_pushnumber(L, glue_order(n));
        } else if (luaS_ptr_eq(s, glue_sign)) {
            lua_pushnumber(L, glue_sign(n));
        } else if (luaS_ptr_eq(s, stretch)) {
            lua_pushnumber(L, glue_stretch(n));
        } else if (luaS_ptr_eq(s, count)) {
            lua_pushnumber(L, span_count(n));
        } else if ((luaS_ptr_eq(s, list)) || (luaS_ptr_eq(s, head))){
            fast_metatable_or_nil_alink(list_ptr(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == attribute_list_node) {
        if (luaS_ptr_eq(s, subtype)) {
            lua_pushnumber(L, subtype(n));
        } else {
            lua_pushnil(L);
        }
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int lua_nodelib_getfield(lua_State * L)
{
    /* [given-node] [...]*/
    halfword *p = lua_touserdata(L, 1);
    if ( (p == NULL) || (! lua_getmetatable(L,1)) ) {
        lua_pushnil(L) ;
        return 1;
    }
    /* [given-node] [mt-given-node]*/
    lua_rawgeti(L, LUA_REGISTRYINDEX, luaS_index(luatex_node));
    lua_gettable(L, LUA_REGISTRYINDEX);
    /* [given-node] [mt-given-node] [mt-node]*/
    if (!lua_rawequal(L, -1, -2)) {
        lua_pushnil(L) ;
        return 1;
    }
    /* prune stack and call getfield */
    lua_settop(L,2);
    return lua_nodelib_fast_getfield(L);
}

/* node.direct.getfield */

static void lua_nodelib_direct_getfield_whatsit(lua_State * L, int n, const char *s)
{
    int t ;
    t = subtype(n);

    if (t == dir_node) {
        if (luaS_ptr_eq(s, dir)) {
            nodelib_pushdir(L, dir_dir(n), true);
        } else if (luaS_ptr_eq(s, level)) {
            lua_pushnumber(L, dir_level(n));
        } else if (luaS_ptr_eq(s, dvi_ptr)) {
            lua_pushnumber(L, dir_dvi_ptr(n));
        } else if (luaS_ptr_eq(s, dir_h)) {
            lua_pushnumber(L, dir_dvi_h(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == user_defined_node) {
        if (luaS_ptr_eq(s, user_id)) {
            lua_pushnumber(L, user_node_id(n));
        } else if (luaS_ptr_eq(s, type)) {
            lua_pushnumber(L, user_node_type(n));
        } else if (luaS_ptr_eq(s, value)) {
            switch (user_node_type(n)) {
            case 'a':
                nodelib_pushdirect(user_node_value(n));
                break;
            case 'd':
                lua_pushnumber(L, user_node_value(n));
                break;
            case 'n':
                nodelib_pushdirect(user_node_value(n));
                break;
            case 's':
                nodelib_pushstring(L, user_node_value(n));
                break;
            case 't':
                tokenlist_to_lua(L, user_node_value(n));
                break;
            default:
                lua_pushnumber(L, user_node_value(n));
                break;
            }
        } else {
            lua_pushnil(L);
        }
    } else if (t == local_par_node) {
        if (luaS_ptr_eq(s, pen_inter)) {
            lua_pushnumber(L, local_pen_inter(n));
        } else if (luaS_ptr_eq(s, pen_broken)) {
            lua_pushnumber(L, local_pen_broken(n));
        } else if (luaS_ptr_eq(s, dir)) {
            nodelib_pushdir(L, local_par_dir(n), false);
        } else if (luaS_ptr_eq(s, box_left)) {
            nodelib_pushdirect_or_nil(local_box_left(n));
        } else if (luaS_ptr_eq(s, box_left_width)) {
            lua_pushnumber(L, local_box_left_width(n));
        } else if (luaS_ptr_eq(s, box_right)) {
            nodelib_pushdirect_or_nil(local_box_right(n));
        } else if (luaS_ptr_eq(s, box_right_width)) {
            lua_pushnumber(L, local_box_right_width(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == pdf_literal_node) {
        if (luaS_ptr_eq(s, mode)) {
            lua_pushnumber(L, pdf_literal_mode(n));
        } else if (luaS_ptr_eq(s, data)) {
            if (pdf_literal_type(n) == lua_refid_literal) {
                lua_rawgeti(Luas, LUA_REGISTRYINDEX, pdf_literal_data(n));
            } else {
                tokenlist_to_luastring(L, pdf_literal_data(n));
            }
        } else {
            lua_pushnil(L);
        }
    } else if (t == late_lua_node) {
        if (luaS_ptr_eq(s, name)) {
            tokenlist_to_luastring(L, late_lua_name(n));
        } else if (luaS_ptr_eq(s, data) || luaS_ptr_eq(s, string)) {
            if (late_lua_type(n) == lua_refid_literal) {
                lua_rawgeti(Luas, LUA_REGISTRYINDEX, late_lua_data(n));
            } else {
                tokenlist_to_luastring(L, late_lua_data(n));
            }
        } else {
            lua_pushnil(L);
        }
    } else if (t == pdf_annot_node) {
        if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, depth)) {
            lua_pushnumber(L, depth(n));
        } else if (luaS_ptr_eq(s, height)) {
            lua_pushnumber(L, height(n));
        } else if (luaS_ptr_eq(s, objnum)) {
            lua_pushnumber(L, pdf_annot_objnum(n));
        } else if (luaS_ptr_eq(s, data)) {
            tokenlist_to_luastring(L, pdf_annot_data(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == pdf_dest_node) {
        if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, depth)) {
            lua_pushnumber(L, depth(n));
        } else if (luaS_ptr_eq(s, height)) {
            lua_pushnumber(L, height(n));
        } else if (luaS_ptr_eq(s, named_id)) {
            lua_pushnumber(L, pdf_dest_named_id(n));
        } else if (luaS_ptr_eq(s, dest_id)) {
            if (pdf_dest_named_id(n) == 1)
                tokenlist_to_luastring(L, pdf_dest_id(n));
            else
                lua_pushnumber(L, pdf_dest_id(n));
        } else if (luaS_ptr_eq(s, dest_type)) {
            lua_pushnumber(L, pdf_dest_type(n));
        } else if (luaS_ptr_eq(s, xyz_zoom)) {
            lua_pushnumber(L, pdf_dest_xyz_zoom(n));
        } else if (luaS_ptr_eq(s, objnum)) {
            lua_pushnumber(L, pdf_dest_objnum(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == pdf_setmatrix_node) {
        if (luaS_ptr_eq(s, data)) {
            tokenlist_to_luastring(L, pdf_setmatrix_data(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == pdf_colorstack_node) {
        if (luaS_ptr_eq(s, stack)) {
            lua_pushnumber(L, pdf_colorstack_stack(n));
        } else if (luaS_ptr_eq(s, command)) {
            lua_pushnumber(L, pdf_colorstack_cmd(n));
        } else if (luaS_ptr_eq(s, data)) {
            tokenlist_to_luastring(L, pdf_colorstack_data(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == pdf_refobj_node) {
        if (luaS_ptr_eq(s, objnum)) {
            lua_pushnumber(L, pdf_obj_objnum(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == pdf_refxform_node) {
        if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, depth)) {
            lua_pushnumber(L, depth(n));
        } else if (luaS_ptr_eq(s, height)) {
            lua_pushnumber(L, height(n));
        } else if (luaS_ptr_eq(s, objnum)) {
            lua_pushnumber(L, pdf_xform_objnum(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == pdf_refximage_node) {
        if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, depth)) {
            lua_pushnumber(L, depth(n));
        } else if (luaS_ptr_eq(s, height)) {
            lua_pushnumber(L, height(n));
        } else if (luaS_ptr_eq(s, transform)) {
            lua_pushnumber(L, pdf_ximage_transform(n));
        } else if (luaS_ptr_eq(s, index)) {
            lua_pushnumber(L, pdf_ximage_index(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == write_node) {
        if (luaS_ptr_eq(s, stream)) {
            lua_pushnumber(L, write_stream(n));
        } else if (luaS_ptr_eq(s, data)) {
            tokenlist_to_lua(L, write_tokens(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == special_node) {
        if (luaS_ptr_eq(s, data)) {
            tokenlist_to_luastring(L, write_tokens(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == pdf_start_link_node) {
        if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, depth)) {
            lua_pushnumber(L, depth(n));
        } else if (luaS_ptr_eq(s, height)) {
            lua_pushnumber(L, height(n));
        } else if (luaS_ptr_eq(s, objnum)) {
            lua_pushnumber(L, pdf_link_objnum(n));
        } else if (luaS_ptr_eq(s, link_attr)) {
            tokenlist_to_luastring(L, pdf_link_attr(n));
        } else if (luaS_ptr_eq(s, action)) {
            nodelib_pushaction(L, pdf_link_action(n));
        } else {
            lua_pushnil(L);
        }
    } else if ((t == pdf_thread_node) || (t == pdf_start_thread_node)) {
        if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, depth)) {
            lua_pushnumber(L, depth(n));
        } else if (luaS_ptr_eq(s, height)) {
            lua_pushnumber(L, height(n));
        } else if (luaS_ptr_eq(s, named_id)) {
            lua_pushnumber(L, pdf_thread_named_id(n));
        } else if (luaS_ptr_eq(s, thread_id)) {
            if (pdf_thread_named_id(n) == 1) {
                tokenlist_to_luastring(L, pdf_thread_id(n));
            } else {
                lua_pushnumber(L, pdf_thread_id(n));
            }
        } else if (luaS_ptr_eq(s, thread_attr)) {
            tokenlist_to_luastring(L, pdf_thread_attr(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == open_node) {
        if (luaS_ptr_eq(s, stream)) {
            lua_pushnumber(L, write_stream(n));
        } else if (luaS_ptr_eq(s, name)) {
            nodelib_pushstring(L, open_name(n));
        } else if (luaS_ptr_eq(s, area)) {
            nodelib_pushstring(L, open_area(n));
        } else if (luaS_ptr_eq(s, ext)) {
            nodelib_pushstring(L, open_ext(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == close_node) {
        if (luaS_ptr_eq(s, stream)) {
            lua_pushnumber(L, write_stream(n));
        } else {
            lua_pushnil(L);
        }
    } else {
        lua_pushnil(L);
    }
}

static int lua_nodelib_direct_getfield(lua_State * L)
{

    halfword n;
    const char *s;
    int t ;

    n = (halfword) lua_tonumber(L, 1);

    t = type(n);

    if (lua_type(L, 2) == LUA_TNUMBER) {

        halfword p;
        int i;

        if (! nodetype_has_attributes(t)) {
            lua_pushnil(L) ;
            return 1;
        }

        p = node_attr(n);
        if (p == null || vlink(p) == null) {
            lua_pushnil(L) ;
            return 1;
        }
        i = (int) lua_tointeger(L, 2);
        p = vlink(p);
        while (p != null) {
            if (attribute_id(p) == i) {
                lua_pushnumber(L, (int) attribute_value(p));
                return 1;
            } else if (attribute_id(p) > i) {
                lua_pushnil(L) ;
                return 1;
            }
            p = vlink(p);
        }
        lua_pushnil(L) ;
        return 1;
    }

    s = lua_tostring(L, 2);

    if (luaS_ptr_eq(s, id)) {
        lua_pushnumber(L, t);
    } else if (luaS_ptr_eq(s, next)) {
        nodelib_pushdirect_or_nil(vlink(n));
    } else if (luaS_ptr_eq(s, prev)) {
        nodelib_pushdirect_or_nil(alink(n));
    } else if (luaS_ptr_eq(s, attr)) {
        if (! nodetype_has_attributes(t)) {
            lua_pushnil(L);
        } else {
            nodelib_pushattr(L, node_attr(n));
        }
    } else if (luaS_ptr_eq(s, subtype)) {
        if (t == glue_spec_node) {
            lua_pushnumber(L, 0); /* dummy, the only one */
        } else {
            lua_pushnumber(L, subtype(n));
        }
    } else if (t == glyph_node) {
        if (luaS_ptr_eq(s, font)) {
            lua_pushnumber(L, font(n));
        } else if (luaS_ptr_eq(s, char)) {
            lua_pushnumber(L, character(n));
        } else if (luaS_ptr_eq(s, xoffset)) {
            lua_pushnumber(L, x_displace(n));
        } else if (luaS_ptr_eq(s, yoffset)) {
            lua_pushnumber(L, y_displace(n));
        } else if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, char_width(font(n),character(n)));
        } else if (luaS_ptr_eq(s, height)) {
            lua_pushnumber(L, char_height(font(n),character(n)));
        } else if (luaS_ptr_eq(s, depth)) {
            lua_pushnumber(L, char_depth(font(n),character(n)));
        } else if (luaS_ptr_eq(s, expansion_factor)) {
            lua_pushnumber(L, ex_glyph(n));
        } else if (luaS_ptr_eq(s, components)) {
            nodelib_pushdirect_or_nil(lig_ptr(n));
        } else if (luaS_ptr_eq(s, lang)) {
            lua_pushnumber(L, char_lang(n));
        } else if (luaS_ptr_eq(s, left)) {
            lua_pushnumber(L, char_lhmin(n));
        } else if (luaS_ptr_eq(s, right)) {
            lua_pushnumber(L, char_rhmin(n));
        } else if (luaS_ptr_eq(s, uchyph)) {
            lua_pushnumber(L, char_uchyph(n));
        } else {
            lua_pushnil(L);
        }
    } else if ((t == hlist_node) || (t == vlist_node)) {
        /* candidates: whd (width,height,depth) */
        if (luaS_ptr_eq(s, list) || luaS_ptr_eq(s, head)) {
            nodelib_pushdirect_or_nil_alink(list_ptr(n));
        } else if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, height)) {
            lua_pushnumber(L, height(n));
        } else if (luaS_ptr_eq(s, depth)) {
            lua_pushnumber(L, depth(n));
        } else if (luaS_ptr_eq(s, dir)) {
            nodelib_pushdir(L, box_dir(n), false);
        } else if (luaS_ptr_eq(s, shift)) {
            lua_pushnumber(L, shift_amount(n));
        } else if (luaS_ptr_eq(s, glue_order)) {
            lua_pushnumber(L, glue_order(n));
        } else if (luaS_ptr_eq(s, glue_sign)) {
            lua_pushnumber(L, glue_sign(n));
        } else if (luaS_ptr_eq(s, glue_set)) {
            lua_pushnumber(L, (double) glue_set(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == disc_node) {
        if (luaS_ptr_eq(s, pre)) {
            nodelib_pushdirect_or_nil(vlink(pre_break(n)));
        } else if (luaS_ptr_eq(s, post)) {
            nodelib_pushdirect_or_nil(vlink(post_break(n)));
        } else if (luaS_ptr_eq(s, replace)) {
            nodelib_pushdirect_or_nil(vlink(no_break(n)));
        } else {
            lua_pushnil(L);
        }
    } else if (t == glue_node) {
        if (luaS_ptr_eq(s, spec)) {
            nodelib_pushdirect(glue_ptr(n));
        } else if (luaS_ptr_eq(s, leader)) {
            nodelib_pushdirect_or_nil(leader_ptr(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == glue_spec_node) {
        if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, stretch)) {
            lua_pushnumber(L, stretch(n));
        } else if (luaS_ptr_eq(s, shrink)) {
            lua_pushnumber(L, shrink(n));
        } else if (luaS_ptr_eq(s, stretch_order)) {
            lua_pushnumber(L, stretch_order(n));
        } else if (luaS_ptr_eq(s, shrink_order)) {
            lua_pushnumber(L, shrink_order(n));
        } else if (luaS_ptr_eq(s, ref_count)) {
            lua_pushnumber(L, glue_ref_count(n));
        } else if (luaS_ptr_eq(s, writable)) {
            lua_pushboolean(L, valid_node(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == kern_node) {
        if (luaS_ptr_eq(s, kern)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, expansion_factor)) {
            lua_pushnumber(L, ex_kern(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == penalty_node) {
        if (luaS_ptr_eq(s, penalty)) {
            lua_pushnumber(L, penalty(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == rule_node) {
        /* candidates: whd (width,height,depth) */
        if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, height)) {
            lua_pushnumber(L, height(n));
        } else if (luaS_ptr_eq(s, depth)) {
            lua_pushnumber(L, depth(n));
        } else if (luaS_ptr_eq(s, dir)) {
            nodelib_pushdir(L, rule_dir(n), false);
        } else {
            lua_pushnil(L);
        }
    } else if (t == whatsit_node) {
        lua_nodelib_direct_getfield_whatsit(L, n, s);
    } else if (t == simple_noad) {
        if (luaS_ptr_eq(s, nucleus)) {
            nodelib_pushdirect_or_nil(nucleus(n));
        } else if (luaS_ptr_eq(s, sub)) {
            nodelib_pushdirect_or_nil(subscr(n));
        } else if (luaS_ptr_eq(s, sup)) {
            nodelib_pushdirect_or_nil(supscr(n));
        } else {
            lua_pushnil(L);
        }
    } else if ((t == math_char_node) || (t == math_text_char_node)) {
        if (luaS_ptr_eq(s, fam)) {
            lua_pushnumber(L, math_fam(n));
        } else if (luaS_ptr_eq(s, char)) {
            lua_pushnumber(L, math_character(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == mark_node) {
        if (luaS_ptr_eq(s, class)) {
            lua_pushnumber(L, mark_class(n));
        } else if (luaS_ptr_eq(s, mark)) {
            tokenlist_to_lua(L, mark_ptr(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == ins_node) {
        if (luaS_ptr_eq(s, cost)) {
            lua_pushnumber(L, float_cost(n));
        } else if (luaS_ptr_eq(s, depth)) {
            lua_pushnumber(L, depth(n));
        } else if (luaS_ptr_eq(s, height)) {
            lua_pushnumber(L, height(n));
        } else if (luaS_ptr_eq(s, spec)) {
            nodelib_pushdirect_or_nil(split_top_ptr(n));
        } else if ((luaS_ptr_eq(s, list)) || (luaS_ptr_eq(s, head))) {
            nodelib_pushdirect_or_nil_alink(ins_ptr(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == math_node) {
        if (luaS_ptr_eq(s, surround)) {
            lua_pushnumber(L, surround(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == fraction_noad) {
        if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, thickness(n));
        } else if (luaS_ptr_eq(s, num)) {
            nodelib_pushdirect_or_nil(numerator(n));
        } else if (luaS_ptr_eq(s, denom)) {
            nodelib_pushdirect_or_nil(denominator(n));
        } else if (luaS_ptr_eq(s, left)) {
            nodelib_pushdirect_or_nil(left_delimiter(n));
        } else if (luaS_ptr_eq(s, right)) {
            nodelib_pushdirect_or_nil(right_delimiter(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == style_node) {
        if (luaS_ptr_eq(s, style)) {
            lua_pushstring(L, math_style_names[subtype(n)]);
        } else {
            lua_pushnil(L);
        }
    } else if (t == accent_noad) {
        if (luaS_ptr_eq(s, nucleus)) {
            nodelib_pushdirect_or_nil(nucleus(n));
        } else if (luaS_ptr_eq(s, sub)) {
            nodelib_pushdirect_or_nil(subscr(n));
        } else if (luaS_ptr_eq(s, sup)) {
            nodelib_pushdirect_or_nil(supscr(n));
        } else if (luaS_ptr_eq(s, accent)) {
            nodelib_pushdirect_or_nil(accent_chr(n));
        } else if (luaS_ptr_eq(s, bot_accent)) {
            nodelib_pushdirect_or_nil(bot_accent_chr(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == fence_noad) {
        if (luaS_ptr_eq(s, delim)) {
            nodelib_pushdirect_or_nil(delimiter(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == delim_node) {
        if (luaS_ptr_eq(s, small_fam)) {
            lua_pushnumber(L, small_fam(n));
        } else if (luaS_ptr_eq(s, small_char)) {
            lua_pushnumber(L, small_char(n));
        } else if (luaS_ptr_eq(s, large_fam)) {
            lua_pushnumber(L, large_fam(n));
        } else if (luaS_ptr_eq(s, large_char)) {
            lua_pushnumber(L, large_char(n));
        } else {
            lua_pushnil(L);
        }
    } else if ((t == sub_box_node) || (t == sub_mlist_node)) {
        if ((luaS_ptr_eq(s, list)) || (luaS_ptr_eq(s, head))){
            nodelib_pushdirect_or_nil_alink(math_list(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == radical_noad) {
        if (luaS_ptr_eq(s, nucleus)) {
            nodelib_pushdirect_or_nil(nucleus(n));
        } else if (luaS_ptr_eq(s, sub)) {
            nodelib_pushdirect_or_nil(subscr(n));
        } else if (luaS_ptr_eq(s, sup)) {
            nodelib_pushdirect_or_nil(supscr(n));
        } else if (luaS_ptr_eq(s, left)) {
            nodelib_pushdirect_or_nil(left_delimiter(n));
        } else if (luaS_ptr_eq(s, degree)) {
            nodelib_pushdirect_or_nil(degree(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == margin_kern_node) {
        if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, glyph)) {
            nodelib_pushdirect_or_nil(margin_char(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == split_up_node) {
        if (luaS_ptr_eq(s, last_ins_ptr)) {
            nodelib_pushdirect_or_nil(last_ins_ptr(n));
        } else if (luaS_ptr_eq(s, best_ins_ptr)) {
            nodelib_pushdirect_or_nil(best_ins_ptr(n));
        } else if (luaS_ptr_eq(s, broken_ptr)) {
            nodelib_pushdirect_or_nil(broken_ptr(n));
        } else if (luaS_ptr_eq(s, broken_ins)) {
            nodelib_pushdirect_or_nil(broken_ins(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == choice_node) {
        if (luaS_ptr_eq(s, display)) {
            nodelib_pushdirect_or_nil(display_mlist(n));
        } else if (luaS_ptr_eq(s, text)) {
            nodelib_pushdirect_or_nil(text_mlist(n));
        } else if (luaS_ptr_eq(s, script)) {
            nodelib_pushdirect_or_nil(script_mlist(n));
        } else if (luaS_ptr_eq(s, scriptscript)) {
            nodelib_pushdirect_or_nil(script_script_mlist(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == inserting_node) {
        if (luaS_ptr_eq(s, last_ins_ptr)) {
            nodelib_pushdirect_or_nil(last_ins_ptr(n));
        } else if (luaS_ptr_eq(s, best_ins_ptr)) {
            nodelib_pushdirect_or_nil(best_ins_ptr(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == attribute_node) {
        if (luaS_ptr_eq(s, number)) {
            lua_pushnumber(L, attribute_id(n));
        } else if (luaS_ptr_eq(s, value)) {
            lua_pushnumber(L, attribute_value(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == adjust_node) {
        if ((luaS_ptr_eq(s, list)) || (luaS_ptr_eq(s, head))) {
            nodelib_pushdirect_or_nil_alink(adjust_ptr(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == action_node) {
        if (luaS_ptr_eq(s, action_type)) {
            lua_pushnumber(L, pdf_action_type(n));
        } else if (luaS_ptr_eq(s, named_id)) {
            lua_pushnumber(L, pdf_action_named_id(n));
        } else if (luaS_ptr_eq(s, action_id)) {
            if (pdf_action_named_id(n) == 1) {
                tokenlist_to_luastring(L, pdf_action_id(n));
            } else {
                lua_pushnumber(L, pdf_action_id(n));
            }
        } else if (luaS_ptr_eq(s, file)) {
            tokenlist_to_luastring(L, pdf_action_file(n));
        } else if (luaS_ptr_eq(s, new_window)) {
            lua_pushnumber(L, pdf_action_new_window(n));
        } else if (luaS_ptr_eq(s, data)) {
            tokenlist_to_luastring(L, pdf_action_tokens(n));
        } else if (luaS_ptr_eq(s, ref_count)) {
            lua_pushnumber(L, pdf_action_refcount(n));
        } else {
            lua_pushnil(L);
        }
    } else if (t == unset_node) {
        if (luaS_ptr_eq(s, width)) {
            lua_pushnumber(L, width(n));
        } else if (luaS_ptr_eq(s, height)) {
            lua_pushnumber(L, height(n));
        } else if (luaS_ptr_eq(s, depth)) {
            lua_pushnumber(L, depth(n));
        } else if (luaS_ptr_eq(s, dir)) {
            nodelib_pushdir(L, box_dir(n), false);
        } else if (luaS_ptr_eq(s, shrink)) {
            lua_pushnumber(L, glue_shrink(n));
        } else if (luaS_ptr_eq(s, glue_order)) {
            lua_pushnumber(L, glue_order(n));
        } else if (luaS_ptr_eq(s, glue_sign)) {
            lua_pushnumber(L, glue_sign(n));
        } else if (luaS_ptr_eq(s, stretch)) {
            lua_pushnumber(L, glue_stretch(n));
        } else if (luaS_ptr_eq(s, count)) {
            lua_pushnumber(L, span_count(n));
        } else if ((luaS_ptr_eq(s, list)) || (luaS_ptr_eq(s, head))){
            nodelib_pushdirect_or_nil_alink(list_ptr(n));
        } else {
            lua_pushnil(L);
        }
 /* } else if (t == attribute_list_node) { */
 /*     lua_pushnil(L); */
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/* msg could be preallocated and shared */

static void lua_nodelib_do_tostring(lua_State * L, halfword n, const char *tag)
{
    char *msg;
    char a[7] = { ' ', ' ', ' ', 'n', 'i', 'l', 0 };
    char v[7] = { ' ', ' ', ' ', 'n', 'i', 'l', 0 };
    msg = xmalloc(256);
    if (alink(n) != null)
        snprintf(a, 7, "%6d", (int) alink(n));
    if (vlink(n) != null)
        snprintf(v, 7, "%6d", (int) vlink(n));
    snprintf(msg, 255, "<%s %s < %6d > %s : %s %d>", tag, a, (int) n, v, node_data[type(n)].name, subtype(n));
    lua_pushstring(L, msg);
    free(msg);
    return ;
}

/* __tostring node.tostring */

static int lua_nodelib_tostring(lua_State * L)
{
    halfword *n;
    n = check_isnode(L, 1);
    lua_nodelib_do_tostring(L, *n, "node");
    return 1;
}

/* node.direct.tostring */

static int lua_nodelib_direct_tostring(lua_State * L)
{
    halfword n = (halfword) lua_tonumber(L,1);
    if (n==0) {
        lua_pushnil(L);
    } else {
        lua_nodelib_do_tostring(L, n, "direct");
    }
    return 1;
}

/* __eq */

static int lua_nodelib_equal(lua_State * L)
{
    halfword n, m;
    n = *((halfword *) lua_touserdata(L, 1));
    m = *((halfword *) lua_touserdata(L, 2));
    lua_pushboolean(L, (n == m));
    return 1;
}

/* node.ligaturing */

static int font_tex_ligaturing(lua_State * L)
{
    /* on the stack are two nodes and a direction */
    halfword tmp_head;
    halfword *h;
    halfword t = null;
    if (lua_gettop(L) < 1) {
        lua_pushnil(L);
        lua_pushboolean(L, 0);
        return 2;
    }
    h = check_isnode(L, 1);
    if (lua_gettop(L) > 1) {
        t = *(check_isnode(L, 2));
    }
    tmp_head = new_node(nesting_node, 1);
    couple_nodes(tmp_head, *h);
    tlink(tmp_head) = t;
    t = handle_ligaturing(tmp_head, t);
    alink(vlink(tmp_head)) = null ; /* hh-ls */
    lua_pushnumber(L, vlink(tmp_head));
    /* can be: lua_nodelib_push_fast(L, head); */
    flush_node(tmp_head);
    lua_nodelib_push(L);
    /* can be: lua_nodelib_push_fast(L, t); */
    lua_pushnumber(L, t);
    lua_nodelib_push(L);
    lua_pushboolean(L, 1);
    return 3;
}

/* node.kerning */

static int font_tex_kerning(lua_State * L)
{
    /* on the stack are two nodes and a direction */

    halfword tmp_head;
    halfword *h;
    halfword t = null;
    if (lua_gettop(L) < 1) {
        lua_pushnil(L);
        lua_pushboolean(L, 0);
        return 2;
    }
    h = check_isnode(L, 1);
    if (lua_gettop(L) > 1) {
        t = *(check_isnode(L, 2));
    }
    tmp_head = new_node(nesting_node, 1);
    couple_nodes(tmp_head, *h);
    tlink(tmp_head) = t;
    t = handle_kerning(tmp_head, t);
    alink(vlink(tmp_head)) = null ; /* hh-ls */
    lua_pushnumber(L, vlink(tmp_head));
    /* can be: lua_nodelib_push_fast(L, head); */
    flush_node(tmp_head);
    lua_nodelib_push(L);
    /* can be: lua_nodelib_push_fast(L, t); */
    lua_pushnumber(L, t);
    lua_nodelib_push(L);
    lua_pushboolean(L, 1);
    return 3;
}


/* node.protect_glyphs (returns also boolean because that signals callback) */

static int lua_nodelib_protect_glyphs(lua_State * L)
{
    int t = 0;
    halfword head = *(check_isnode(L, 1));
    while (head != null) {
        if (type(head) == glyph_node) {
            int s = subtype(head);
            if (s <= 256) {
                t = 1;
                subtype(head) = (quarterword) (s == 1 ? 256 : 256 + s);
            }
        }
        head = vlink(head);
    }
    lua_pushboolean(L, t);
    lua_pushvalue(L, 1);
    return 2;
}

/* node.direct.protect_glyphs */

static int lua_nodelib_direct_protect_glyphs(lua_State * L)
{
    int t = 0;
    halfword head = (halfword) lua_tonumber(L,1);
    while (head != null) {
        if (type(head) == glyph_node) {
            int s = subtype(head);
            if (s <= 256) {
                t = 1;
                subtype(head) = (quarterword) (s == 1 ? 256 : 256 + s);
            }
        }
        head = vlink(head);
    }
    lua_pushboolean(L, t);
    lua_pushvalue(L, 1);
    return 2;
}

/* node.unprotect_glyphs (returns also boolean because that signals callback) */

static int lua_nodelib_unprotect_glyphs(lua_State * L)
{
    int t = 0;
    halfword head = *(check_isnode(L, 1));
    while (head != null) {
        if (type(head) == glyph_node) {
            int s = subtype(head);
            if (s > 256) {
                t = 1;
                subtype(head) = (quarterword) (s - 256);
            }
        }
        head = vlink(head);
    }
    lua_pushboolean(L, t);
    lua_pushvalue(L, 1);
    return 2;
}

/* node.direct.unprotect_glyphs */

static int lua_nodelib_direct_unprotect_glyphs(lua_State * L)
{
    int t = 0;
    halfword head = (halfword) lua_tonumber(L,1);
    while (head != null) {
        if (type(head) == glyph_node) {
            int s = subtype(head);
            if (s > 256) {
                t = 1;
                subtype(head) = (quarterword) (s - 256);
            }
        }
        head = vlink(head);
    }
    lua_pushboolean(L, t);
    lua_pushvalue(L, 1);
    return 2;
}

/* node.first_glyph */

static int lua_nodelib_first_glyph(lua_State * L)
{
    /* on the stack are two nodes and a direction */
    halfword h, savetail = null, t = null;
    if (lua_gettop(L) < 1) {
        lua_pushnil(L);
        lua_pushboolean(L, 0);
        return 2;
    }
    h = *(check_isnode(L, 1));
    if (lua_gettop(L) > 1) {
        t = *(check_isnode(L, 2));
        savetail = vlink(t);
        vlink(t) = null;
    }
    while (h != null && (type(h) != glyph_node || !is_simple_character(h))) {
        h = vlink(h);
    }
    if (savetail != null) {
        vlink(t) = savetail;
    }
    lua_pushnumber(L, h);
    lua_nodelib_push(L);
    lua_pushboolean(L, (h == null ? 0 : 1));
    return 2;
}

/* node.direct.first_glyph */

static int lua_nodelib_direct_first_glyph(lua_State * L)
{
    halfword h,savetail,t;
    savetail = null;
    t = null;
    h = (halfword) lua_tonumber(L,1);
    if (h == null) {
        lua_pushnil(L);
        return 1;
    }
    t = (halfword) lua_tonumber(L,2);
    if (t != null) {
        savetail = vlink(t);
        vlink(t) = null;
    }
    while (h != null && (type(h) != glyph_node || !is_simple_character(h)))
        h = vlink(h);
    if (savetail != null)
        vlink(t) = savetail;
    lua_pushnumber(L, h);
    return 1; /* no need to also push a boolean if we have nil */
}

/* new, fast and dumb ones: only signals that something needs to be processed */

/* #define do_has_glyph(h) do {                                 \ */
/*     while (h != null) {                                      \ */
/*         if (type(h) == glyph_node) {                         \ */
/*             return h;                                        \ */
/*         } else {                                             \ */
/*             h = vlink(h);                                    \ */
/*         }                                                    \ */
/*     }                                                        \ */
/*     return null;                                             \ */
/* } while (0) */




/* node.has_glyph */

static int lua_nodelib_has_glyph(lua_State * L)
{
    halfword *a;
    halfword h = (halfword) *(check_isnode(L,1)) ;
    while (h != null) {
        if (type(h) == glyph_node) {
            fast_metatable(h);
            return 1;
        } else {
            h = vlink(h);
        }
    }
    lua_pushnil(L);
    return 1;
}

/* node.direct.has_glyph */

static int lua_nodelib_direct_has_glyph(lua_State * L)
{
    halfword h = (halfword) lua_tonumber(L,1) ;
    while (h != null) {
        if (type(h) == glyph_node) {
            nodelib_pushdirect(h);
            return 1;
        } else {
            h = vlink(h);
        }
    }
    lua_pushnil(L);
    return 1;
}


/* depricated */

static int lua_nodelib_first_character(lua_State * L)
{
    pdftex_warn("node.first_character() is deprecated, please update to node.first_glyph()");
    return lua_nodelib_first_glyph(L);
}

/* this is too simplistic, but it helps Hans to get going */

static halfword do_ligature_n(halfword prev, halfword stop, halfword lig)
{
    vlink(lig) = vlink(stop);
    vlink(stop) = null;
    lig_ptr(lig) = vlink(prev);
    vlink(prev) = lig;
    return lig;
}

/* node.do_ligature_n(node prev, node last, node lig) */

static int lua_nodelib_do_ligature_n(lua_State * L)
{
    halfword n, m, o, p, tmp_head;
    n = *(check_isnode(L, 1));
    m = *(check_isnode(L, 2));
    o = *(check_isnode(L, 3));
    if (alink(n) == null || vlink(alink(n)) != n) {
        tmp_head = new_node(temp_node, 0);
        couple_nodes(tmp_head, n);
        p = do_ligature_n(tmp_head, m, o);
        flush_node(tmp_head);
    } else {
        p = do_ligature_n(alink(n), m, o);
    }
    lua_pushnumber(L, p);
    lua_nodelib_push(L);
    return 1;
}

/* node.direct.do_ligature_n(node prev, node last, node lig) */

static int lua_nodelib_direct_do_ligature_n(lua_State * L)
{
    halfword n, m, o, p, tmp_head;
    n = (halfword) lua_tonumber(L, 1);
    m = (halfword) lua_tonumber(L, 2);
    o = (halfword) lua_tonumber(L, 3);
    if ((n == null) || (m == null) || (o == null)) {
        lua_pushnil(L);
    } else {
        if (alink(n) == null || vlink(alink(n)) != n) {
            tmp_head = new_node(temp_node, 0);
            couple_nodes(tmp_head, n);
            p = do_ligature_n(tmp_head, m, o);
            flush_node(tmp_head);
        } else {
            p = do_ligature_n(alink(n), m, o);
        }
        lua_pushnumber(L, p);
    }
    return 1;
}

/* node.usedlist */

static int lua_nodelib_usedlist(lua_State * L)
{
    lua_pushnumber(L, list_node_mem_usage());
    lua_nodelib_push(L);
    return 1;
}

/* node.direct.usedlist */

static int lua_nodelib_direct_usedlist(lua_State * L)
{
    lua_pushnumber(L, list_node_mem_usage());
    return 1;
}

/* node.protrusion_skipable(node m) */

static int lua_nodelib_cp_skipable(lua_State * L)
{
    halfword n;
    n = *(check_isnode(L, 1));
    lua_pushboolean(L, cp_skipable(n));
    return 1;
}

/* node.direct.protrusion_skipable(node m) */

static int lua_nodelib_direct_cp_skipable(lua_State * L)
{
    halfword n;
    n = (halfword) lua_tonumber(L, 1);
    if (n == null) {
        lua_pushnil(L);
    } else {
        lua_pushboolean(L, cp_skipable(n));
    }
    return 1;
}

/* node.currentattr(node m) */

static int lua_nodelib_currentattr(lua_State * L)
{
    int n = lua_gettop(L);
    if (n == null) {
        /* query */
        if (max_used_attr >= 0) {
            if (attr_list_cache == cache_disabled) {
                update_attribute_cache();
                if (attr_list_cache == null) {
                    lua_pushnil (L);
                    return 1;
                }
            }
            attr_list_ref(attr_list_cache)++;
            lua_pushnumber(L, attr_list_cache);
            lua_nodelib_push(L);
        } else {
            lua_pushnil(L);
        }
        return 1;
    } else {
        /* assign */
        pdftex_warn("Assignment via node.current_attr(<list>) is not supported (yet)");
        return 0;
    }
}

/* node.direct.currentattr(node m) */

static int lua_nodelib_direct_currentattr(lua_State * L)
{
    if (max_used_attr >= 0) {
        if (attr_list_cache == cache_disabled) {
            update_attribute_cache();
            if (attr_list_cache == null) {
                lua_pushnil (L);
                return 1;
            }
        }
        attr_list_ref(attr_list_cache)++;
        lua_pushnumber(L, attr_list_cache);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/* node.direct.todirect */

static int lua_nodelib_direct_todirect(lua_State * L)
{
    if (lua_type(L,1) != LUA_TNUMBER) {
        /* assume node, no further testing, used in known situations */
        void *n ; 
        n = lua_touserdata(L, 1);
        if (n == null) {
            lua_pushnil(L);
        } else {
            lua_pushnumber(L, *((halfword *)n) );
        }
    } /* else assume direct and returns argument */
    return 1;
}


/* node.direct.tonode */

static int lua_nodelib_direct_tonode(lua_State * L)
{
    halfword *a;
    halfword n;
    n = (halfword) lua_tonumber(L, 1);
    if (n != null) {
        a = (halfword *) lua_newuserdata(L, sizeof(halfword));
        *a=n;
        lua_push_node_metatablelua;
        lua_setmetatable(L,-2);
    } /* else assume node and return argument */
    return 1;
}

/* node.setfield */

static int lua_nodelib_setfield_whatsit(lua_State * L, int n, const char *s)
{
    int t ;
    t = subtype(n);

    if (t == dir_node) {
        if (luaS_ptr_eq(s, dir)) {
            dir_dir(n) = nodelib_getdir(L, 3, 0);
        } else if (luaS_ptr_eq(s, level)) {
            dir_level(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, dvi_ptr)) {
            dir_dvi_ptr(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, dir_h)) {
            dir_dvi_h(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == pdf_literal_node) {
        if (luaS_ptr_eq(s, mode)) {
            pdf_literal_mode(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, data)) {
            if (ini_version) {
                pdf_literal_data(n) = nodelib_gettoks(L, 3);
            } else {
                lua_pushvalue(L, 3);
                pdf_literal_data(n) = luaL_ref(L, LUA_REGISTRYINDEX);
                pdf_literal_type(n) = lua_refid_literal;
            }
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == late_lua_node) {
        if (luaS_ptr_eq(s, data)) {
            late_lua_data(n) = nodelib_gettoks(L, 3);
            late_lua_type(n) = normal;
        } else if (luaS_ptr_eq(s, name)) {
            late_lua_name(n) = nodelib_gettoks(L, 3);
        } else if (luaS_ptr_eq(s, string)) {
            if (ini_version) {
                late_lua_data(n) = nodelib_gettoks(L, 3);
                late_lua_type(n) = normal;
            } else {
                lua_pushvalue(L, 3);
                late_lua_data(n) = luaL_ref(L, LUA_REGISTRYINDEX);
                late_lua_type(n) = lua_refid_literal;
            }
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == user_defined_node) {
        if (luaS_ptr_eq(s, user_id)) {
            user_node_id(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, type)) {
            user_node_type(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, value)) {
            switch (user_node_type(n)) {
            case 'a':
                user_node_value(n) = nodelib_getlist(L, 3);
                break;
            case 'd':
                user_node_value(n) = (halfword) lua_tointeger(L, 3);
                break;
            case 'n':
                user_node_value(n) = nodelib_getlist(L, 3);
                break;
            case 's':
                user_node_value(n) = nodelib_getstring(L, 3);
                break;
            case 't':
                user_node_value(n) = nodelib_gettoks(L, 3);
                break;
            default:
                user_node_value(n) = (halfword) lua_tointeger(L, 3);
                break;
            }
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == pdf_annot_node) {
        if (luaS_ptr_eq(s, width)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, depth)) {
            depth(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, height)) {
            height(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, objnum)) {
            pdf_annot_objnum(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, data)) {
            pdf_annot_data(n) = nodelib_gettoks(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == pdf_dest_node) {
        if (luaS_ptr_eq(s, width)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, depth)) {
            depth(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, height)) {
            height(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, named_id)) {
            pdf_dest_named_id(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, dest_id)) {
            if (pdf_dest_named_id(n) == 1) {
                pdf_dest_id(n) = nodelib_gettoks(L, 3);
            } else {
                pdf_dest_id(n) = (halfword) lua_tointeger(L, 3);
            }
        } else if (luaS_ptr_eq(s, dest_type)) {
            pdf_dest_type(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, xyz_zoom)) {
            pdf_dest_xyz_zoom(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, objnum)) {
            pdf_dest_objnum(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == pdf_setmatrix_node) {
        if (luaS_ptr_eq(s, data)) {
            pdf_setmatrix_data(n) = nodelib_gettoks(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == pdf_refobj_node) {
        if (luaS_ptr_eq(s, objnum)) {
            pdf_obj_objnum(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == pdf_refxform_node) {
        if (luaS_ptr_eq(s, width)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, depth)) {
            depth(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, height)) {
            height(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, objnum)) {
            pdf_xform_objnum(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == pdf_refximage_node) {
        if (luaS_ptr_eq(s, width)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, depth)) {
            depth(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, height)) {
            height(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, transform)) {
            pdf_ximage_transform(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, index)) {
            pdf_ximage_index(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == local_par_node) {
        if (luaS_ptr_eq(s, pen_inter)) {
            local_pen_inter(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, pen_broken)) {
            local_pen_broken(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, dir)) {
            local_par_dir(n) = nodelib_getdir(L, 3, 1);
        } else if (luaS_ptr_eq(s, box_left)) {
            local_box_left(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, box_left_width)) {
            local_box_left_width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, box_right)) {
            local_box_right(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, box_right_width)) {
            local_box_right_width(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == pdf_start_link_node) {
        if (luaS_ptr_eq(s, width)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, depth)) {
            depth(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, height)) {
            height(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, objnum)) {
            pdf_link_objnum(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, link_attr)) {
            pdf_link_attr(n) = nodelib_gettoks(L, 3);
        } else if (luaS_ptr_eq(s, action)) {
            pdf_link_action(n) = nodelib_getaction(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == write_node) {
        if (luaS_ptr_eq(s, stream)) {
            write_stream(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, data)) {
            write_tokens(n) = nodelib_gettoks(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == pdf_colorstack_node) {
        if (luaS_ptr_eq(s, stack)) {
            pdf_colorstack_stack(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, command)) {
            pdf_colorstack_cmd(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, data)) {
            pdf_colorstack_data(n) = nodelib_gettoks(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if ((t == pdf_thread_node) || (t == pdf_start_thread_node)) {
        if (luaS_ptr_eq(s, width)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, depth)) {
            depth(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, height)) {
            height(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, named_id)) {
            pdf_thread_named_id(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, thread_id)) {
            if (pdf_thread_named_id(n) == 1) {
                pdf_thread_id(n) = nodelib_gettoks(L, 3);
            } else {
                pdf_thread_id(n) = (halfword) lua_tointeger(L, 3);
            }
        } else if (luaS_ptr_eq(s, thread_attr)) {
            pdf_thread_attr(n) = nodelib_gettoks(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == special_node) {
        if (luaS_ptr_eq(s, data)) {
            write_tokens(n) = nodelib_gettoks(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == open_node) {
        if (luaS_ptr_eq(s, stream)) {
            write_stream(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, name)) {
            open_name(n) = nodelib_getstring(L, 3);
        } else if (luaS_ptr_eq(s, area)) {
            open_area(n) = nodelib_getstring(L, 3);
        } else if (luaS_ptr_eq(s, ext)) {
            open_ext(n) = nodelib_getstring(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == close_node) {
        if (luaS_ptr_eq(s, stream)) {
            write_stream(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if ((t == pdf_end_link_node) || (t == pdf_end_thread_node) || (t == pdf_save_pos_node) ||
               (t == pdf_save_node)     || (t == pdf_restore_node)    || (t == cancel_boundary_node)) {
        return nodelib_cantset(L, n, s);
    } else {
        /* do nothing */
    }
    return 0;
}

static int lua_nodelib_fast_setfield(lua_State * L)
{
    halfword n;
    const char *s;
    int t ;

    n = *((halfword *) lua_touserdata(L, 1));
    t = type(n);

    if (lua_type(L, 2) == LUA_TNUMBER) {

        int i, val;

        if (lua_gettop(L) == 3) {
            i = (int) lua_tointeger(L, 2);
            val = (int) lua_tointeger(L, 3);
            if (val == UNUSED_ATTRIBUTE) {
                (void) unset_attribute(n, i, val);
            } else {
                set_attribute(n, i, val);
            }
        } else {
            luaL_error(L, "incorrect number of arguments");
        }
        return 0;
    }

    s = lua_tostring(L, 2);

    if (luaS_ptr_eq(s, id)) {
        lua_pushnumber(L, t);
    } else if (luaS_ptr_eq(s, next)) {
        halfword x = nodelib_getlist(L, 3);
        if (x>0 && type(x) == glue_spec_node) {
            return luaL_error(L, "You can't assign a %s node to a next field\n", node_data[type(x)].name);
        }
        vlink(n) = x;
    } else if (luaS_ptr_eq(s, prev)) {
        halfword x = nodelib_getlist(L, 3);
        if (x>0 && type(x) == glue_spec_node) {
            return luaL_error(L, "You can't assign a %s node to a prev field\n", node_data[type(x)].name);
        }
        alink(n) = x;
    } else if (luaS_ptr_eq(s, attr)) {
        if (nodetype_has_attributes(type(n))) {
            nodelib_setattr(L, 3, n);
        }
    } else if (t == glyph_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, font)) {
            font(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, char)) {
            character(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, xoffset)) {
            x_displace(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, yoffset)) {
            y_displace(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, width)) {
            /* not yet */
        } else if (luaS_ptr_eq(s, height)) {
            /* not yet */
        } else if (luaS_ptr_eq(s, depth)) {
            /* not yet */
        } else if (luaS_ptr_eq(s, expansion_factor)) {
            ex_glyph(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, components)) {
            lig_ptr(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, lang)) {
            set_char_lang(n, (halfword) lua_tointeger(L, 3));
        } else if (luaS_ptr_eq(s, left)) {
            set_char_lhmin(n, (halfword) lua_tointeger(L, 3));
        } else if (luaS_ptr_eq(s, right)) {
            set_char_rhmin(n, (halfword) lua_tointeger(L, 3));
        } else if (luaS_ptr_eq(s, uchyph)) {
            set_char_uchyph(n, (halfword) lua_tointeger(L, 3));
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if ((t == hlist_node) || (t == vlist_node)) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, list) || luaS_ptr_eq(s, head)) {
            list_ptr(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, width)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, height)) {
            height(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, depth)) {
            depth(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, dir)) {
            box_dir(n) = nodelib_getdir(L, 3, 1);
        } else if (luaS_ptr_eq(s, shift)) {
            shift_amount(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, glue_order)) {
            glue_order(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, glue_sign)) {
            glue_sign(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, glue_set)) {
            glue_set(n) = (glue_ratio) lua_tonumber(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == disc_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, pre)) {
            set_disc_field(pre_break(n), nodelib_getlist(L, 3));
        } else if (luaS_ptr_eq(s, post)) {
            set_disc_field(post_break(n), nodelib_getlist(L, 3));
        } else if (luaS_ptr_eq(s, replace)) {
            set_disc_field(no_break(n), nodelib_getlist(L, 3));
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == glue_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, spec)) {
            glue_ptr(n) = nodelib_getspec(L, 3);
        } else if (luaS_ptr_eq(s, leader)) {
            leader_ptr(n) = nodelib_getlist(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == glue_spec_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3); /* dummy, the only one that prevents move up */
        } else if (luaS_ptr_eq(s, width)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, stretch)) {
            stretch(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, shrink)) {
            shrink(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, stretch_order)) {
            stretch_order(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, shrink_order)) {
            lua_pushnumber(L, shrink_order(n));
    /* } else if (luaS_ptr_eq(s, ref_count)) {
            glue_ref_count(n) = (halfword) lua_tointeger(L, 3);
           } else if (luaS_ptr_eq(s, writable)) {
    */
            /* can't be set */
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == kern_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, kern)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, expansion_factor)) {
            ex_kern(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == penalty_node) {
        if (luaS_ptr_eq(s, subtype)) {
            /* dummy subtype */
        } else if (luaS_ptr_eq(s, penalty)) {
            penalty(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == rule_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, width)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, height)) {
            height(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, depth)) {
            depth(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, dir)) {
            rule_dir(n) = nodelib_getdir(L, 3, 1);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == whatsit_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else {
            lua_nodelib_setfield_whatsit(L, n, s);
        }
    } else if (t == simple_noad) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, nucleus)) {
            nucleus(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, sub)) {
            subscr(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, sup)) {
            supscr(n) = nodelib_getlist(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if ((t == math_char_node) || (t == math_text_char_node)) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, fam)) {
            math_fam(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, char)) {
            math_character(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == mark_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, class)) {
            mark_class(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, mark)) {
            mark_ptr(n) = nodelib_gettoks(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == ins_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, cost)) {
            float_cost(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, depth)) {
            depth(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, height)) {
            height(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, spec)) {
            split_top_ptr(n) = nodelib_getspec(L, 3);
        } else if ((luaS_ptr_eq(s, list)) || (luaS_ptr_eq(s, head))) {
            ins_ptr(n) = nodelib_getlist(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == math_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, surround)) {
            surround(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == fraction_noad) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, width)) {
            thickness(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, num)) {
            numerator(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, denom)) {
            denominator(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, left)) {
            left_delimiter(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, right)) {
            right_delimiter(n) = nodelib_getlist(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == style_node) {
        if (luaS_ptr_eq(s, subtype)) {
            /* dummy subtype */
        } else if (luaS_ptr_eq(s, style)) {
            lua_pushstring(L, math_style_names[subtype(n)]);
        } else {
            /* return nodelib_cantset(L, n, s); */
            subtype(n) = (quarterword) luaL_checkoption(L, 3, "text", math_style_names); /* not 2? */
        }
    } else if (t == accent_noad) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, nucleus)) {
            nucleus(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, sub)) {
            subscr(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, sup)) {
            supscr(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, accent)) {
            accent_chr(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, bot_accent)) {
            bot_accent_chr(n) = nodelib_getlist(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == fence_noad) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, delim)) {
            delimiter(n) = nodelib_getlist(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == delim_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, small_fam)) {
            small_fam(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, small_char)) {
            small_char(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, large_fam)) {
            large_fam(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, large_char)) {
            large_char(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if ((t == sub_box_node) || (t == sub_mlist_node)) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if ((luaS_ptr_eq(s, list)) || (luaS_ptr_eq(s, head))){
            math_list(n) = nodelib_getlist(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == radical_noad) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, nucleus)) {
            nucleus(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, sub)) {
            subscr(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, sup)) {
            supscr(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, left)) {
            left_delimiter(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, degree)) {
            degree(n) = nodelib_getlist(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == margin_kern_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, width)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, glyph)) {
            margin_char(n) = nodelib_getlist(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == split_up_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, last_ins_ptr)) {
            last_ins_ptr(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, best_ins_ptr)) {
            best_ins_ptr(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, broken_ptr)) {
            broken_ptr(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, broken_ins)) {
            broken_ins(n) = nodelib_getlist(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == choice_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, display)) {
            display_mlist(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, text)) {
            text_mlist(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, script)) {
            script_mlist(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, scriptscript)) {
            script_script_mlist(n) = nodelib_getlist(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == inserting_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, last_ins_ptr)) {
            last_ins_ptr(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, best_ins_ptr)) {
            best_ins_ptr(n) = nodelib_getlist(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == attribute_node) {
        if (luaS_ptr_eq(s, subtype)) {
            /* dummy subtype */
        } else if (luaS_ptr_eq(s, number)) {
            attribute_id(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, value)) {
            attribute_value(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == adjust_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if ((luaS_ptr_eq(s, list)) || (luaS_ptr_eq(s, head))) {
            adjust_ptr(n) = nodelib_getlist(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == action_node) {
        if (luaS_ptr_eq(s, subtype)) {
            /* dummy subtype */
        } else if (luaS_ptr_eq(s, action_type)) {
            pdf_action_type(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, named_id)) {
            pdf_action_named_id(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, action_id)) {
            if (pdf_action_named_id(n) == 1) {
                pdf_action_id(n) = nodelib_gettoks(L, 3);
            } else {
                pdf_action_id(n) = (halfword) lua_tointeger(L, 3);
            }
        } else if (luaS_ptr_eq(s, file)) {
            pdf_action_file(n) = nodelib_gettoks(L, 3);
        } else if (luaS_ptr_eq(s, new_window)) {
            pdf_action_new_window(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, data)) {
            pdf_action_tokens(n) = nodelib_gettoks(L, 3);
        /* } else if (luaS_ptr_eq(s, ref_count)) {
            pdf_action_refcount(n) = (halfword) lua_tointeger(L, 3); */
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == unset_node) {
        if (luaS_ptr_eq(s, subtype)) {
            /* dummy subtype */
        } else if (luaS_ptr_eq(s, width)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, height)) {
            height(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, depth)) {
            depth(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, dir)) {
            box_dir(n) = nodelib_getdir(L, 3, 1);
        } else if (luaS_ptr_eq(s, shrink)) {
            glue_shrink(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, glue_order)) {
            glue_order(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, glue_sign)) {
            glue_sign(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, stretch)) {
            glue_stretch(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, count)) {
            span_count(n) = (quarterword) lua_tointeger(L, 3);
        } else if ((luaS_ptr_eq(s, list)) || (luaS_ptr_eq(s, head))){
            list_ptr(n) = nodelib_getlist(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == attribute_list_node) {
        if (luaS_ptr_eq(s, subtype)) {
            /* dummy subtype */
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else {
        return luaL_error(L, "You can't assign to this %s node (%d)\n", node_data[t].name, n);
    }
    return 0;
}

static int lua_nodelib_setfield(lua_State * L)
{
    /* [given-node] [...]*/
    halfword *p = lua_touserdata(L, 1);
    if ( (p == NULL) || (! lua_getmetatable(L,1)) ) {
        return 0;
    }
    /* [given-node] [mt-given-node]*/
    lua_rawgeti(L, LUA_REGISTRYINDEX, luaS_index(luatex_node));
    lua_gettable(L, LUA_REGISTRYINDEX);
    /* [given-node] [mt-given-node] [mt-node]*/
    if ( (!lua_rawequal(L, -1, -2)) ) {
        return 0;
    }
    /* prune stack and call getfield */
    lua_settop(L,3);
    return lua_nodelib_fast_setfield(L);
}

/* node.direct.setfield */

static int lua_nodelib_direct_setfield_whatsit(lua_State * L, int n, const char *s)
{
    int t ;
    t = subtype(n);

    if (t == dir_node) {
        if (luaS_ptr_eq(s, dir)) {
            dir_dir(n) = nodelib_getdir(L, 3, 0);
        } else if (luaS_ptr_eq(s, level)) {
            dir_level(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, dvi_ptr)) {
            dir_dvi_ptr(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, dir_h)) {
            dir_dvi_h(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == pdf_literal_node) {
        if (luaS_ptr_eq(s, mode)) {
            pdf_literal_mode(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, data)) {
            if (ini_version) {
                pdf_literal_data(n) = nodelib_gettoks(L, 3);
            } else {
                lua_pushvalue(L, 3);
                pdf_literal_data(n) = luaL_ref(L, LUA_REGISTRYINDEX);
                pdf_literal_type(n) = lua_refid_literal;
            }
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == late_lua_node) {
        if (luaS_ptr_eq(s, data)) {
            late_lua_data(n) = nodelib_gettoks(L, 3);
            late_lua_type(n) = normal;
        } else if (luaS_ptr_eq(s, name)) {
            late_lua_name(n) = nodelib_gettoks(L, 3);
        } else if (luaS_ptr_eq(s, string)) {
            if (ini_version) {
                late_lua_data(n) = nodelib_gettoks(L, 3);
                late_lua_type(n) = normal;
            } else {
                lua_pushvalue(L, 3);
                late_lua_data(n) = luaL_ref(L, LUA_REGISTRYINDEX);
                late_lua_type(n) = lua_refid_literal;
            }
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == user_defined_node) {
        if (luaS_ptr_eq(s, user_id)) {
            user_node_id(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, type)) {
            user_node_type(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, value)) {
            switch (user_node_type(n)) {
            case 'a':
                user_node_value(n) = nodelib_getlist(L, 3);
                break;
            case 'd':
                user_node_value(n) = (halfword) lua_tointeger(L, 3);
                break;
            case 'n':
                user_node_value(n) = nodelib_getlist(L, 3);
                break;
            case 's':
                user_node_value(n) = nodelib_getstring(L, 3);
                break;
            case 't':
                user_node_value(n) = nodelib_gettoks(L, 3);
                break;
            default:
                user_node_value(n) = (halfword) lua_tointeger(L, 3);
                break;
            }
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == pdf_annot_node) {
        if (luaS_ptr_eq(s, width)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, depth)) {
            depth(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, height)) {
            height(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, objnum)) {
            pdf_annot_objnum(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, data)) {
            pdf_annot_data(n) = nodelib_gettoks(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == pdf_dest_node) {
        if (luaS_ptr_eq(s, width)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, depth)) {
            depth(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, height)) {
            height(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, named_id)) {
            pdf_dest_named_id(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, dest_id)) {
            if (pdf_dest_named_id(n) == 1) {
                pdf_dest_id(n) = nodelib_gettoks(L, 3);
            } else {
                pdf_dest_id(n) = (halfword) lua_tointeger(L, 3);
            }
        } else if (luaS_ptr_eq(s, dest_type)) {
            pdf_dest_type(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, xyz_zoom)) {
            pdf_dest_xyz_zoom(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, objnum)) {
            pdf_dest_objnum(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == pdf_setmatrix_node) {
        if (luaS_ptr_eq(s, data)) {
            pdf_setmatrix_data(n) = nodelib_gettoks(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == pdf_refobj_node) {
        if (luaS_ptr_eq(s, objnum)) {
            pdf_obj_objnum(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == pdf_refxform_node) {
        if (luaS_ptr_eq(s, width)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, depth)) {
            depth(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, height)) {
            height(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, objnum)) {
            pdf_xform_objnum(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == pdf_refximage_node) {
        if (luaS_ptr_eq(s, width)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, depth)) {
            depth(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, height)) {
            height(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, transform)) {
            pdf_ximage_transform(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, index)) {
            pdf_ximage_index(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == local_par_node) {
        if (luaS_ptr_eq(s, pen_inter)) {
            local_pen_inter(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, pen_broken)) {
            local_pen_broken(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, dir)) {
            local_par_dir(n) = nodelib_getdir(L, 3, 1);
        } else if (luaS_ptr_eq(s, box_left)) {
            local_box_left(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, box_left_width)) {
            local_box_left_width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, box_right)) {
            local_box_right(n) = nodelib_getlist(L, 3);
        } else if (luaS_ptr_eq(s, box_right_width)) {
            local_box_right_width(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == pdf_start_link_node) {
        if (luaS_ptr_eq(s, width)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, depth)) {
            depth(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, height)) {
            height(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, objnum)) {
            pdf_link_objnum(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, link_attr)) {
            pdf_link_attr(n) = nodelib_gettoks(L, 3);
        } else if (luaS_ptr_eq(s, action)) {
            pdf_link_action(n) = nodelib_popdirect(n);  /*nodelib_getaction(L, 3);*/
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == write_node) {
        if (luaS_ptr_eq(s, stream)) {
            write_stream(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, data)) {
            write_tokens(n) = nodelib_gettoks(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == pdf_colorstack_node) {
        if (luaS_ptr_eq(s, stack)) {
            pdf_colorstack_stack(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, command)) {
            pdf_colorstack_cmd(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, data)) {
            pdf_colorstack_data(n) = nodelib_gettoks(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if ((t == pdf_thread_node) || (t == pdf_start_thread_node)) {
        if (luaS_ptr_eq(s, width)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, depth)) {
            depth(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, height)) {
            height(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, named_id)) {
            pdf_thread_named_id(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, thread_id)) {
            if (pdf_thread_named_id(n) == 1) {
                pdf_thread_id(n) = nodelib_gettoks(L, 3);
            } else {
                pdf_thread_id(n) = (halfword) lua_tointeger(L, 3);
            }
        } else if (luaS_ptr_eq(s, thread_attr)) {
            pdf_thread_attr(n) = nodelib_gettoks(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == special_node) {
        if (luaS_ptr_eq(s, data)) {
            write_tokens(n) = nodelib_gettoks(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == open_node) {
        if (luaS_ptr_eq(s, stream)) {
            write_stream(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, name)) {
            open_name(n) = nodelib_getstring(L, 3);
        } else if (luaS_ptr_eq(s, area)) {
            open_area(n) = nodelib_getstring(L, 3);
        } else if (luaS_ptr_eq(s, ext)) {
            open_ext(n) = nodelib_getstring(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == close_node) {
        if (luaS_ptr_eq(s, stream)) {
            write_stream(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if ((t == pdf_end_link_node) || (t == pdf_end_thread_node) || (t == pdf_save_pos_node) ||
               (t == pdf_save_node)     || (t == pdf_restore_node)    || (t == cancel_boundary_node)) {
        return nodelib_cantset(L, n, s);
    } else {
        /* do nothing */
    }
    return 0;
}

static int lua_nodelib_direct_setfield(lua_State * L)
{
    halfword n;
    const char *s;
    int t ;

    n = (halfword ) lua_tonumber(L, 1);
    t = type(n);

    if (lua_type(L, 2) == LUA_TNUMBER) {

        int i, val;

        if (lua_gettop(L) == 3) {
            i = (int) lua_tointeger(L, 2);
            val = (int) lua_tointeger(L, 3);
            if (val == UNUSED_ATTRIBUTE) {
                (void) unset_attribute(n, i, val);
            } else {
                set_attribute(n, i, val);
            }
        } else {
            luaL_error(L, "incorrect number of arguments");
        }
        return 0;
    }

    s = lua_tostring(L, 2);

    if (luaS_ptr_eq(s, id)) {
        lua_pushnumber(L, t);
    } else if (luaS_ptr_eq(s, next)) {
        halfword x = nodelib_popdirect(3);
        if (x>0 && type(x) == glue_spec_node) {
            return luaL_error(L, "You can't assign a %s node to a next field\n", node_data[type(x)].name);
        }
        vlink(n) = x;
    } else if (luaS_ptr_eq(s, prev)) {
        halfword x = nodelib_popdirect(3);
        if (x>0 && type(x) == glue_spec_node) {
            return luaL_error(L, "You can't assign a %s node to a prev field\n", node_data[type(x)].name);
        }
        alink(n) = x;
    } else if (luaS_ptr_eq(s, attr)) {
        if (nodetype_has_attributes(type(n))) {
            nodelib_setattr(L, 3, n);
        }
    } else if (t == glyph_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, font)) {
            font(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, char)) {
            character(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, xoffset)) {
            x_displace(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, yoffset)) {
            y_displace(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, width)) {
            /* not yet */
        } else if (luaS_ptr_eq(s, height)) {
            /* not yet */
        } else if (luaS_ptr_eq(s, depth)) {
            /* not yet */
        } else if (luaS_ptr_eq(s, expansion_factor)) {
            ex_glyph(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, components)) {
            lig_ptr(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, lang)) {
            set_char_lang(n, (halfword) lua_tointeger(L, 3));
        } else if (luaS_ptr_eq(s, left)) {
            set_char_lhmin(n, (halfword) lua_tointeger(L, 3));
        } else if (luaS_ptr_eq(s, right)) {
            set_char_rhmin(n, (halfword) lua_tointeger(L, 3));
        } else if (luaS_ptr_eq(s, uchyph)) {
            set_char_uchyph(n, (halfword) lua_tointeger(L, 3));
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if ((t == hlist_node) || (t == vlist_node)) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, list) || luaS_ptr_eq(s, head)) {
            list_ptr(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, width)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, height)) {
            height(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, depth)) {
            depth(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, dir)) {
            box_dir(n) = nodelib_getdir(L, 3, 1);
        } else if (luaS_ptr_eq(s, shift)) {
            shift_amount(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, glue_order)) {
            glue_order(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, glue_sign)) {
            glue_sign(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, glue_set)) {
            glue_set(n) = (glue_ratio) lua_tonumber(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == disc_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, pre)) {
            set_disc_field(pre_break(n), nodelib_popdirect(3));
        } else if (luaS_ptr_eq(s, post)) {
            set_disc_field(post_break(n), nodelib_popdirect(3));
        } else if (luaS_ptr_eq(s, replace)) {
            set_disc_field(no_break(n), nodelib_popdirect(3));
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == glue_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, spec)) {
            glue_ptr(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, leader)) {
            leader_ptr(n) = nodelib_popdirect(3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == glue_spec_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3); /* dummy, the only one that prevents move up */
        } else if (luaS_ptr_eq(s, width)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, stretch)) {
            stretch(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, shrink)) {
            shrink(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, stretch_order)) {
            stretch_order(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, shrink_order)) {
            lua_pushnumber(L, shrink_order(n));
     /* } else if (luaS_ptr_eq(s, ref_count)) {
            glue_ref_count(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, writable)) {
     */
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == kern_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, kern)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, expansion_factor)) {
            ex_kern(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == penalty_node) {
        if (luaS_ptr_eq(s, subtype)) {
            /* dummy subtype */
        } else if (luaS_ptr_eq(s, penalty)) {
            penalty(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == rule_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, width)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, height)) {
            height(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, depth)) {
            depth(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, dir)) {
            rule_dir(n) = nodelib_getdir(L, 3, 1);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == whatsit_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else {
            lua_nodelib_direct_setfield_whatsit(L, n, s);
        }
    } else if (t == simple_noad) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, nucleus)) {
            nucleus(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, sub)) {
            subscr(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, sup)) {
            supscr(n) = nodelib_popdirect(3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if ((t == math_char_node) || (t == math_text_char_node)) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, fam)) {
            math_fam(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, char)) {
            math_character(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == mark_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, class)) {
            mark_class(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, mark)) {
            mark_ptr(n) = nodelib_gettoks(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == ins_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, cost)) {
            float_cost(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, depth)) {
            depth(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, height)) {
            height(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, spec)) {
            split_top_ptr(n) = nodelib_popdirect(3);
        } else if ((luaS_ptr_eq(s, list)) || (luaS_ptr_eq(s, head))) {
            ins_ptr(n) = nodelib_popdirect(3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == math_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, surround)) {
            surround(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == fraction_noad) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, width)) {
            thickness(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, num)) {
            numerator(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, denom)) {
            denominator(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, left)) {
            left_delimiter(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, right)) {
            right_delimiter(n) = nodelib_popdirect(3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == style_node) {
        if (luaS_ptr_eq(s, subtype)) {
            /* dummy subtype */
        } else if (luaS_ptr_eq(s, style)) {
            lua_pushstring(L, math_style_names[subtype(n)]);
        } else {
            /* return nodelib_cantset(L, n, s); */
            subtype(n) = (quarterword) luaL_checkoption(L, 2, "text", math_style_names); /* was 3 */
        }
    } else if (t == accent_noad) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, nucleus)) {
            nucleus(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, sub)) {
            subscr(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, sup)) {
            supscr(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, accent)) {
            accent_chr(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, bot_accent)) {
            bot_accent_chr(n) = nodelib_popdirect(3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == fence_noad) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, delim)) {
            delimiter(n) = nodelib_popdirect(3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == delim_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, small_fam)) {
            small_fam(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, small_char)) {
            small_char(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, large_fam)) {
            large_fam(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, large_char)) {
            large_char(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if ((t == sub_box_node) || (t == sub_mlist_node)) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if ((luaS_ptr_eq(s, list)) || (luaS_ptr_eq(s, head))){
            math_list(n) = nodelib_popdirect(3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == radical_noad) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, nucleus)) {
            nucleus(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, sub)) {
            subscr(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, sup)) {
            supscr(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, left)) {
            left_delimiter(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, degree)) {
            degree(n) = nodelib_popdirect(3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == margin_kern_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, width)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, glyph)) {
            margin_char(n) = nodelib_popdirect(3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == split_up_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, last_ins_ptr)) {
            last_ins_ptr(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, best_ins_ptr)) {
            best_ins_ptr(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, broken_ptr)) {
            broken_ptr(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, broken_ins)) {
            broken_ins(n) = nodelib_popdirect(3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == choice_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, display)) {
            display_mlist(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, text)) {
            text_mlist(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, script)) {
            script_mlist(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, scriptscript)) {
            script_script_mlist(n) = nodelib_popdirect(3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == inserting_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, last_ins_ptr)) {
            last_ins_ptr(n) = nodelib_popdirect(3);
        } else if (luaS_ptr_eq(s, best_ins_ptr)) {
            best_ins_ptr(n) = nodelib_popdirect(3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == attribute_node) {
        if (luaS_ptr_eq(s, subtype)) {
            /* dummy subtype */
        } else if (luaS_ptr_eq(s, number)) {
            attribute_id(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, value)) {
            attribute_value(n) = (halfword) lua_tointeger(L, 3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == adjust_node) {
        if (luaS_ptr_eq(s, subtype)) {
            subtype(n) = (quarterword) lua_tointeger(L, 3);
        } else if ((luaS_ptr_eq(s, list)) || (luaS_ptr_eq(s, head))) {
            adjust_ptr(n) = nodelib_popdirect(3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == action_node) {
        if (luaS_ptr_eq(s, subtype)) {
            /* dummy subtype */
        } else if (luaS_ptr_eq(s, action_type)) {
            pdf_action_type(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, named_id)) {
            pdf_action_named_id(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, action_id)) {
            if (pdf_action_named_id(n) == 1) {
                pdf_action_id(n) = nodelib_gettoks(L, 3);
            } else {
                pdf_action_id(n) = (halfword) lua_tointeger(L, 3);
            }
        } else if (luaS_ptr_eq(s, file)) {
            pdf_action_file(n) = nodelib_gettoks(L, 3);
        } else if (luaS_ptr_eq(s, new_window)) {
            pdf_action_new_window(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, data)) {
            pdf_action_tokens(n) = nodelib_gettoks(L, 3);
     /* } else if (luaS_ptr_eq(s, ref_count)) {
            pdf_action_refcount(n) = (halfword) lua_tointeger(L, 3); */
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == unset_node) {
        if (luaS_ptr_eq(s, subtype)) {
            /* dummy subtype */
        } else if (luaS_ptr_eq(s, width)) {
            width(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, height)) {
            height(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, depth)) {
            depth(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, dir)) {
            box_dir(n) = nodelib_getdir(L, 3, 1);
        } else if (luaS_ptr_eq(s, shrink)) {
            glue_shrink(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, glue_order)) {
            glue_order(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, glue_sign)) {
            glue_sign(n) = (quarterword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, stretch)) {
            glue_stretch(n) = (halfword) lua_tointeger(L, 3);
        } else if (luaS_ptr_eq(s, count)) {
            span_count(n) = (quarterword) lua_tointeger(L, 3);
        } else if ((luaS_ptr_eq(s, list)) || (luaS_ptr_eq(s, head))){
            list_ptr(n) = nodelib_popdirect(3);
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else if (t == attribute_list_node) {
        if (luaS_ptr_eq(s, subtype)) {
            /* dummy subtype */
        } else {
            return nodelib_cantset(L, n, s);
        }
    } else {
        return luaL_error(L, "You can't assign to this %s node (%d)\n", node_data[t].name, n);
    }
    return 0;
}

/* boxes */

static int direct_get_box_id(lua_State * L, int i)
{
    const char *s;
    int cur_cs1, cur_cmd1;
    size_t k = 0;
    int j = -1;
    switch (lua_type(L, i)) {
    case LUA_TSTRING:
        s = lua_tolstring(L, i, &k);
        cur_cs1 = string_lookup(s, k);
        cur_cmd1 = eq_type(cur_cs1);
        if (cur_cmd1 == char_given_cmd ||
            cur_cmd1 == math_given_cmd) {
            j = equiv(cur_cs1);
        }
        break;
    case LUA_TNUMBER:
        j=(int)lua_tonumber(L, (i));
        break;
    default:
        luaL_error(L, "argument must be a string or a number");
        j = -1;                 /* not a valid box id */
    }
    return j;
}

/* node.getbox = tex.getbox */

/* node.direct.getbox */

static int lua_nodelib_direct_getbox(lua_State * L)
{
    int k, t;
    k = direct_get_box_id(L, -1);
    direct_check_index_range(k, "getbox");
    t = get_tex_box_register(k);
    lua_pushnumber(L, t);
    return 1;
}

static int direct_vsetbox(lua_State * L, int is_global)
{
    int j, k, err;
    int save_global_defs = int_par(global_defs_code);
    if (is_global)
        int_par(global_defs_code) = 1;
    k = direct_get_box_id(L, -2);
    direct_check_index_range(k, "setbox");
    if (lua_isboolean(L, -1)) {
        j = lua_toboolean(L, -1);
        if (j == 0)
            j = null;
        else
            return 0;
    } else {
        j=nodelib_popdirect(-1);
        if (j != null && type(j) != hlist_node && type(j) != vlist_node) {
            luaL_error(L, "setbox: incompatible node type (%s)\n",get_node_name(type(j), subtype(j)));
            return 0;
        }

    }
    err = set_tex_box_register(k, j);
    int_par(global_defs_code) = save_global_defs;
    if (err) {
        luaL_error(L, "incorrect value");
    }
    return 0;
}

/* node.setbox = tex.setbox */
/* node.setbox */

static int lua_nodelib_direct_setbox(lua_State * L)
{
    int isglobal = 0;
    int n = lua_gettop(L);
    if (n == 3 && lua_isstring(L, 1)) {
        const char *s = lua_tostring(L, 1);
        if (luaS_ptr_eq(s, global))
            isglobal = 1;
    }
    return direct_vsetbox(L, isglobal);
}

/* node.is_node(n) */

static int lua_nodelib_is_node(lua_State * L)
{
    if (maybe_isnode(L,1) == NULL)
        lua_pushboolean(L,0);
    else
        lua_pushboolean(L,1);
    return 1;
}

/* node.direct.is_direct(n) (handy for mixed usage testing) */

static int lua_nodelib_direct_is_direct(lua_State * L)
{   /*
        quick and dirty and faster than not node.is_node, this helper
        returns false or <direct>
    */
    /*
        if (lua_type(L,1) == LUA_TNUMBER)
            lua_pushboolean(L,1);
        else
            lua_pushboolean(L,0);
    */
    if (lua_type(L,1) != LUA_TNUMBER)
        lua_pushboolean(L,0);
    /* else return direct */
    return 1;
}

/* node.direct.is_node(n) (handy for mixed usage testing) */

static int lua_nodelib_direct_is_node(lua_State * L)
{   /*
        quick and dirty, only checks userdata, node.is_node is slower but
        more exact, this helper returns false or <node>
    */
    /*
        halfword *n = lua_touserdata(L, 1);
        if (n != NULL) {
            lua_pushboolean(L,1);
        } else {
            lua_pushboolean(L,0);
        }
    */
    if (maybe_isnode(L,1) == NULL)
        lua_pushboolean(L,0);
    /* else assume and return node */
    return 1;
}

/* node.fast.*

    static const struct luaL_Reg fast_nodelib_f[] = {
        {"getid", lua_nodelib_fast_getid},
        {"getsubtype", lua_nodelib_fast_getsubtype},
        {"getfont", lua_nodelib_fast_getfont},
        {"getchar", lua_nodelib_fast_getcharacter},
        {"getnext", lua_nodelib_fast_getnext},
        {"getprev", lua_nodelib_fast_getprev},
        {"getfield", lua_nodelib_fast_getfield},
        {"setfield", lua_nodelib_fast_setfield},
        {NULL, NULL}
    };

*/

/* node.direct.* */

static const struct luaL_Reg direct_nodelib_f[] = {
    {"copy", lua_nodelib_direct_copy},
    {"copy_list", lua_nodelib_direct_copy_list},
    {"count", lua_nodelib_direct_count},
    {"current_attr", lua_nodelib_direct_currentattr},
    {"dimensions", lua_nodelib_direct_dimensions},
    {"do_ligature_n", lua_nodelib_direct_do_ligature_n},
    {"end_of_math", lua_nodelib_direct_end_of_math},
 /* {"family_font", lua_nodelib_mfont}, */                    /* no node argument */
 /* {"fields", lua_nodelib_fields}, */                        /* no node argument */
 /* {"first_character", lua_nodelib_first_character}, */      /* obsolete */
    {"first_glyph", lua_nodelib_direct_first_glyph},
    {"flush_list", lua_nodelib_direct_flush_list},
    {"flush_node", lua_nodelib_direct_flush_node},
    {"free", lua_nodelib_direct_free},
    {"getbox", lua_nodelib_direct_getbox},
    {"getchar", lua_nodelib_direct_getcharacter},
    {"getfield", lua_nodelib_direct_getfield},
    {"getfont", lua_nodelib_direct_getfont},
    {"getid", lua_nodelib_direct_getid},
    {"getnext", lua_nodelib_direct_getnext},
    {"getprev", lua_nodelib_direct_getprev},
    {"getlist", lua_nodelib_direct_getlist},
    {"getleader", lua_nodelib_direct_getleader},
    {"getsubtype", lua_nodelib_direct_getsubtype},
    {"has_glyph", lua_nodelib_direct_has_glyph},
    {"has_attribute", lua_nodelib_direct_has_attribute},
    {"has_field", lua_nodelib_direct_has_field},
    {"hpack", lua_nodelib_direct_hpack},
 /* {"id", lua_nodelib_id}, */                                /* no node argument */
    {"insert_after", lua_nodelib_direct_insert_after},
    {"insert_before", lua_nodelib_direct_insert_before},
    {"is_direct", lua_nodelib_direct_is_direct},
    {"is_node", lua_nodelib_direct_is_node},
 /* {"kerning", font_tex_kerning}, */                         /* maybe direct too (rather basic callback exposure) */
    {"last_node", lua_nodelib_direct_last_node},
    {"length", lua_nodelib_direct_length},
 /* {"ligaturing", font_tex_ligaturing}, */                   /* maybe direct too (rather basic callback exposure) */
 /* {"mlist_to_hlist", lua_nodelib_mlist_to_hlist}, */        /* maybe direct too (rather basic callback exposure) */
    {"new", lua_nodelib_direct_new},
 /* {"next", lua_nodelib_next}, */                            /* replaced by getnext */
 /* {"prev", lua_nodelib_prev}, */                            /* replaced by getprev */
    {"tostring", lua_nodelib_direct_tostring},
    {"protect_glyphs", lua_nodelib_direct_protect_glyphs},
    {"protrusion_skippable", lua_nodelib_direct_cp_skipable},
    {"remove", lua_nodelib_direct_remove},
    {"set_attribute", lua_nodelib_direct_set_attribute},
    {"setbox", lua_nodelib_direct_setbox},
    {"setfield", lua_nodelib_direct_setfield},
    {"slide", lua_nodelib_direct_slide},
 /* {"subtype", lua_nodelib_subtype}, */                      /* no node argument */
    {"tail", lua_nodelib_direct_tail},
    {"todirect",  lua_nodelib_direct_todirect},               /* not in node.* */
    {"tonode", lua_nodelib_direct_tonode},                    /* similar to node.* */
    {"traverse", lua_nodelib_direct_traverse},
    {"traverse_id", lua_nodelib_direct_traverse_filtered},
 /* {"type", lua_nodelib_type}, */                            /* no node argument */
 /* {"types", lua_nodelib_types}, */                          /* no node argument */
    {"unprotect_glyphs", lua_nodelib_direct_unprotect_glyphs},
    {"unset_attribute", lua_nodelib_direct_unset_attribute},
    {"usedlist", lua_nodelib_direct_usedlist},
    {"vpack", lua_nodelib_direct_vpack},
 /* {"whatsits", lua_nodelib_whatsits}, */                    /* no node argument */
    {"write", lua_nodelib_direct_append},
    {NULL, NULL} /* sentinel */
};

/* node.* */

static const struct luaL_Reg nodelib_f[] = {
    {"copy", lua_nodelib_copy},
    {"copy_list", lua_nodelib_copy_list},
    {"count", lua_nodelib_count},
    {"current_attr", lua_nodelib_currentattr},
    {"dimensions", lua_nodelib_dimensions},
    {"do_ligature_n", lua_nodelib_do_ligature_n},
    {"end_of_math", lua_nodelib_end_of_math},
    {"family_font", lua_nodelib_mfont},
    {"fields", lua_nodelib_fields},
    {"first_character", lua_nodelib_first_character},
    {"first_glyph", lua_nodelib_first_glyph},
    {"flush_list", lua_nodelib_flush_list},
    {"flush_node", lua_nodelib_flush_node},
    {"free", lua_nodelib_free},
 /* {"getbox", lua_nodelib_getbox}, */ /* tex.getbox */
    {"getnext", lua_nodelib_getnext},
    {"getprev", lua_nodelib_getprev},
    {"getlist", lua_nodelib_getlist},
    {"getleader", lua_nodelib_getleader},
    {"getid", lua_nodelib_getid},
    {"getfield", lua_nodelib_getfield},
    {"setfield", lua_nodelib_setfield},
    {"getsubtype", lua_nodelib_getsubtype},
    {"getfont", lua_nodelib_getfont},
    {"getchar", lua_nodelib_getcharacter},
    {"has_glyph", lua_nodelib_has_glyph},
    {"has_attribute", lua_nodelib_has_attribute},
    {"has_field", lua_nodelib_has_field},
    {"hpack", lua_nodelib_hpack},
    {"id", lua_nodelib_id},
    {"insert_after", lua_nodelib_insert_after},
    {"insert_before", lua_nodelib_insert_before},
    {"is_node", lua_nodelib_is_node},
    {"kerning", font_tex_kerning},
    {"last_node", lua_nodelib_last_node},
    {"length", lua_nodelib_length},
    {"ligaturing", font_tex_ligaturing},
    {"mlist_to_hlist", lua_nodelib_mlist_to_hlist},
    {"new", lua_nodelib_new},
    {"next", lua_nodelib_next}, /* getnext is somewhat more efficient, and get* fits better in direct compatibility */
    {"prev", lua_nodelib_prev}, /* getprev is somewhat more efficient, and get* fits better in direct compatibility */
    {"tostring", lua_nodelib_tostring},
    {"protect_glyphs", lua_nodelib_protect_glyphs},
    {"protrusion_skippable", lua_nodelib_cp_skipable},
    {"remove", lua_nodelib_remove},
 /* {"setbox", lua_nodelib_setbox}, */ /* tex.setbox */
    {"set_attribute", lua_nodelib_set_attribute},
    {"slide", lua_nodelib_slide},
    {"subtype", lua_nodelib_subtype},
    {"tail", lua_nodelib_tail},
    {"traverse", lua_nodelib_traverse},
    {"traverse_id", lua_nodelib_traverse_filtered},
    {"type", lua_nodelib_type},
    {"types", lua_nodelib_types},
    {"unprotect_glyphs", lua_nodelib_unprotect_glyphs},
    {"unset_attribute", lua_nodelib_unset_attribute},
    {"usedlist", lua_nodelib_usedlist},
    {"vpack", lua_nodelib_vpack},
    {"whatsits", lua_nodelib_whatsits},
    {"write", lua_nodelib_append},
    {NULL, NULL} /* sentinel */
};

static const struct luaL_Reg nodelib_m[] = {
    {"__index", lua_nodelib_fast_getfield},
    {"__newindex", lua_nodelib_fast_setfield},
    {"__tostring", lua_nodelib_tostring},
    {"__eq", lua_nodelib_equal},
    {NULL, NULL} /* sentinel */
};

int luaopen_node(lua_State * L)
{
    /* the main metatable of node userdata */
    luaL_newmetatable(L, NODE_METATABLE);
    /* node.* */
    luaL_register(L, NULL, nodelib_m);
    luaL_register(L, "node", nodelib_f);
    /* node.fast (experimental code)
        lua_pushstring(L,"fast");
        lua_newtable(L);
        luaL_register(L, NULL, fast_nodelib_f);
        lua_rawset(L,-3);
    */
    /* node.direct */
    lua_pushstring(L,"direct");
    lua_newtable(L);
    luaL_register(L, NULL, direct_nodelib_f);
    lua_rawset(L,-3);
    /* initialization of keywords */
    initialize_luaS_indexes(L);
    return 1;
}

void nodelist_to_lua(lua_State * L, int n)
{
    lua_pushnumber(L, n);
    lua_nodelib_push(L);
}

int nodelist_from_lua(lua_State * L)
{
    halfword *n;
    if (lua_isnil(L, -1))
        return null;
    n = check_isnode(L, -1);
    return (n ? *n : null);
}
