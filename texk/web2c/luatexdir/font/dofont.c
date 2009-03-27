/* dofont.c
   
   Copyright 2006-2008 Taco Hoekwater <taco@luatex.org>

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

#include "luatex-api.h"

static const char _svn_version[] =
    "$Id: dofont.c 2086 2009-03-22 15:32:08Z oneiros $ $URL: http://scm.foundry.supelec.fr/svn/luatex/trunk/src/texk/web2c/luatexdir/font/dofont.c $";

#define TIMERS 0

#if TIMERS
#  include <sys/time.h>
#endif


/* a bit more interfacing is needed for proper error reporting */

static char *font_error_message(pointer u, char *nom, scaled s)
{
    char *str = xmalloc(256);
    char *c = makecstring(zget_cs_text(u));
    char *extra = "metric data not found or bad";
    if (s >= 0) {
        snprintf(str, 255, "Font \\%s=%s at %gpt not loadable: %s", c, nom,
                 (double) s / 65536, extra);
    } else if (s != -1000) {
        snprintf(str, 255, "Font \\%s=%s scaled %d not loadable: %s", c, nom,
                 (int) (-s), extra);
    } else {
        snprintf(str, 255, "Font \\%s=%s not loadable: %s", c, nom, extra);
    }
    return str;
}

static int
do_define_font(integer f, char *cnom, char *caire, scaled s,
               integer natural_dir)
{

    boolean res;                /* was the callback successful? */
    integer callback_id;
    char *cnam;
#if TIMERS
    struct timeval tva;
    struct timeval tvb;
    double tvdiff;
#endif
    int r;
    res = 0;

    callback_id = callback_defined(define_font_callback);
    if (callback_id > 0) {
        if (caire == NULL || strlen(caire) == 0) {
            cnam = xstrdup(cnom);
        } else {
            cnam = xmalloc(strlen(cnom) + strlen(caire) + 2);
            sprintf(cnam, "%s/%s", caire, cnom);
        }
#if TIMERS
        gettimeofday(&tva, NULL);
#endif
        callback_id = run_and_save_callback(callback_id, "Sdd->", cnam, s, f);
#if TIMERS
        gettimeofday(&tvb, NULL);
        tvdiff = tvb.tv_sec * 1000000.0;
        tvdiff += (double) tvb.tv_usec;
        tvdiff -= (tva.tv_sec * 1000000.0);
        tvdiff -= (double) tva.tv_usec;
        tvdiff /= 1000000;
        fprintf(stdout, "\ncallback('define_font',%s,%i): %f seconds\n", cnam,
                f, tvdiff);
#endif
        free(cnam);
        if (callback_id > 0) {  /* success */
            luaL_checkstack(Luas, 1, "out of stack space");
            lua_rawgeti(Luas, LUA_REGISTRYINDEX, callback_id);
            if (lua_istable(Luas, -1)) {
#if TIMERS
                gettimeofday(&tva, NULL);
#endif
                res = font_from_lua(Luas, f);
                destroy_saved_callback(callback_id);
#if TIMERS
                gettimeofday(&tvb, NULL);
                tvdiff = tvb.tv_sec * 1000000.0;
                tvdiff += (double) tvb.tv_usec;
                tvdiff -= (tva.tv_sec * 1000000.0);
                tvdiff -= (double) tva.tv_usec;
                tvdiff /= 1000000;
                fprintf(stdout, "font_from_lua(%s,%i): %f seconds\n",
                        font_name(f), f, tvdiff);
#endif
                /* lua_pop(Luas, 1); *//* done by font_from_lua */
            } else if (lua_isnumber(Luas, -1)) {
                r = lua_tonumber(Luas, -1);
                destroy_saved_callback(callback_id);
                delete_font(f);
                lua_pop(Luas, 1);
                return r;
            } else {
                lua_pop(Luas, 1);
                delete_font(f);
                return 0;
            }
        }
    } else {
        res = read_tfm_info(f, cnom, caire, s);
        if (res) {
            set_hyphen_char(f, get_default_hyphen_char());
            set_skew_char(f, get_default_skew_char());
        }
    }
    if (res) {
        if (font_type(f) != virtual_font_type) {        /* implies lua */
            do_vf(f);
            set_font_natural_dir(f, natural_dir);
        }
        return f;
    } else {
        delete_font(f);
        return 0;
    }

}

int read_font_info(pointer u, strnumber nom, scaled s, integer natural_dir)
{
    integer f;
    char *cnom;
    char *msg;
    cnom = xstrdup(makecstring(nom));

    f = new_font();
    if ((f = do_define_font(f, cnom, NULL, s, natural_dir))) {
        free(cnom);
        return f;
    } else {
        char *help[] = { "I wasn't able to read the size data for this font,",
            "so I will ignore the font specification.",
            "[Wizards can fix TFM files using TFtoPL/PLtoTF.]",
            "You might try inserting a different font spec;",
            "e.g., type `I\font<same font id>=<substitute font name>'.",
            NULL
        };
        if (!get_suppress_fontnotfound_error()) {
            msg = font_error_message(u, cnom, s);
            tex_error(msg, help);
            free(msg);
        }
        free(cnom);
        return 0;
    }
}

/* TODO This function is a placeholder. There can easily appears holes in 
   the |font_tables| array, and we could attempt to reuse those
*/

int find_font_id(char *nom, char *aire, scaled s)
{
    integer f;
    f = new_font();
    if ((f = do_define_font(f, nom, aire, s, -1))) {
        return f;
    } else {
        return 0;
    }
}
