/* ltexlib.c

   Copyright 2006-2012 Taco Hoekwater <taco@luatex.org>

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

/* hh-ls: Because the lists start with a temp node, we have to set the prev link
to nil because otherwise at the lua end we expose temp which can create havoc. In the
setter no prev link is created so we can presume that it's not used later on. */

#include "ptexlib.h"
#include "lua/luatex-api.h"


#define attribute(A) eqtb[attribute_base+(A)].hh.rh
#define dimen(A) eqtb[scaled_base+(A)].hh.rh
#undef skip
#define skip(A) eqtb[skip_base+(A)].hh.rh
#define mu_skip(A) eqtb[mu_skip_base+(A)].hh.rh
#define count(A) eqtb[count_base+(A)].hh.rh
#define box(A) equiv(box_base+(A))

/* tex random generators */
extern int unif_rand(int );
extern int norm_rand(void );
extern void init_randoms(int );


typedef struct {
    char *text;
    unsigned int tsize;
    void *next;
    boolean partial;
    int cattable;
} rope;

typedef struct {
    rope *head;
    rope *tail;
    char complete;              /* currently still writing ? */
} spindle;

#define  PARTIAL_LINE       1
#define  FULL_LINE          0

#define  write_spindle spindles[spindle_index]
#define  read_spindle  spindles[(spindle_index-1)]

static int spindle_size = 0;
static spindle *spindles = NULL;
static int spindle_index = 0;



static void luac_store(lua_State * L, int i, int partial, int cattable)
{
    char *st;
    const char *sttemp;
    size_t tsize;
    rope *rn = NULL;
    sttemp = lua_tolstring(L, i, &tsize);
    st = xmalloc((unsigned) (tsize + 1));
    memcpy(st, sttemp, (tsize + 1));
    if (st) {
        luacstrings++;
        rn = (rope *) xmalloc(sizeof(rope));
        rn->text = st;
        rn->tsize = (unsigned) tsize;
        rn->partial = partial;
        rn->cattable = cattable;
        rn->next = NULL;
        if (write_spindle.head == NULL) {
            assert(write_spindle.tail == NULL);
            write_spindle.head = rn;
        } else {
            write_spindle.tail->next = rn;
        }
        write_spindle.tail = rn;
        write_spindle.complete = 0;
    }
}


static int do_luacprint(lua_State * L, int partial, int deftable)
{
    int i, n;
    int cattable = deftable;
    int startstrings = 1;
    n = lua_gettop(L);
    if (cattable != NO_CAT_TABLE) {
        if (lua_type(L, 1) == LUA_TNUMBER && n > 1) {
            cattable=(int)lua_tonumber(L, 1);
            startstrings = 2;
            if (cattable != -1 && cattable != -2 && !valid_catcode_table(cattable)) {
       cattable = DEFAULT_CAT_TABLE;
     }
        }
    }
    if (lua_type(L, startstrings) == LUA_TTABLE) {
        for (i = 1;; i++) {
            lua_rawgeti(L, startstrings, i);
            if (lua_isstring(L, -1)) {
                luac_store(L, -1, partial, cattable);
                lua_pop(L, 1);
            } else {
                break;
            }
        }
    } else {
        for (i = startstrings; i <= n; i++) {
            if (!lua_isstring(L, i)) {
                luaL_error(L, "no string to print");
            }
            luac_store(L, i, partial, cattable);
        }
        /* hh: We could use this but it makes not much different, apart from allocating more ropes so less
           memory. To be looked into: lua 5.2 buffer mechanism as now we still hash the concatination. This
           test was part of the why-eis-luajit-so-slow on crited experiments. */
        /*
        if (startstrings == n) {
            luac_store(L, n, partial, cattable);
        } else {
            lua_concat(L,n-startstrings+1);
            luac_store(L, startstrings, partial, cattable);
        }
        */
    }
    return 0;
}




static int luacwrite(lua_State * L)
{
    return do_luacprint(L, FULL_LINE, NO_CAT_TABLE);
}

static int luacprint(lua_State * L)
{
    return do_luacprint(L, FULL_LINE, DEFAULT_CAT_TABLE);
}

static int luacsprint(lua_State * L)
{
    return do_luacprint(L, PARTIAL_LINE, DEFAULT_CAT_TABLE);
}

static int luactprint(lua_State * L)
{
    int i, j, n;
    int cattable, startstrings;
    n = lua_gettop(L);
    for (i = 1; i <= n; i++) {
        cattable = DEFAULT_CAT_TABLE;
        startstrings = 1;
        if (lua_type(L, i) != LUA_TTABLE) {
            luaL_error(L, "no string to print");
        }
        lua_pushvalue(L, i);    /* push the table */
        lua_pushnumber(L, 1);
        lua_gettable(L, -2);
        if (lua_type(L, -1) == LUA_TNUMBER) {
            cattable=(int)lua_tonumber(L, -1);
            startstrings = 2;
            if (cattable != -1 && cattable != -2 && !valid_catcode_table(cattable)) {
	      cattable = DEFAULT_CAT_TABLE;
	    }
        }
        lua_pop(L, 1);

        for (j = startstrings;; j++) {
            lua_pushnumber(L, j);
            lua_gettable(L, -2);
            if (lua_type(L, -1) == LUA_TSTRING) {
                luac_store(L, -1, PARTIAL_LINE, cattable);
                lua_pop(L, 1);
            } else {
                lua_pop(L, 1);
                break;
            }
        }
        lua_pop(L, 1);          /* pop the table */
    }
    return 0;
}


int luacstring_cattable(void)
{
    return (int) read_spindle.tail->cattable;
}

int luacstring_partial(void)
{
    return read_spindle.tail->partial;
}

int luacstring_final_line(void)
{
    return (read_spindle.tail->next == NULL);
}

int luacstring_input(void)
{
    char *st;
    int ret;
    rope *t = read_spindle.head;
    if (!read_spindle.complete) {
        read_spindle.complete = 1;
        read_spindle.tail = NULL;
    }
    if (t == NULL) {
        if (read_spindle.tail != NULL)
            free(read_spindle.tail);
        read_spindle.tail = NULL;
        return 0;
    }
    if (t->text != NULL) {
        st = t->text;
        /* put that thing in the buffer */
        last = first;
        ret = last;
        check_buffer_overflow(last + (int) t->tsize);

        while (t->tsize-- > 0)
            buffer[last++] = (packed_ASCII_code) * st++;
        if (!t->partial) {
            while (last - 1 > ret && buffer[last - 1] == ' ')
                last--;
        }
        free(t->text);
        t->text = NULL;
    }
    if (read_spindle.tail != NULL) {    /* not a one-liner */
        free(read_spindle.tail);
    }
    read_spindle.tail = t;
    read_spindle.head = t->next;
    return 1;
}

/* open for reading, and make a new one for writing */
void luacstring_start(int n)
{
    (void) n;                   /* for -W */
    spindle_index++;
    if (spindle_size == spindle_index) {        /* add a new one */
        spindles =
            xrealloc(spindles,
                     (unsigned) (sizeof(spindle) *
                                 (unsigned) (spindle_size + 1)));
        spindles[spindle_index].head = NULL;
        spindles[spindle_index].tail = NULL;
        spindles[spindle_index].complete = 0;
        spindle_size++;
    }
}

/* close for reading */

void luacstring_close(int n)
{
    rope *next, *t;
    (void) n;                   /* for -W */
    next = read_spindle.head;
    while (next != NULL) {
        if (next->text != NULL)
            free(next->text);
        t = next;
        next = next->next;
        if (t==read_spindle.tail) {
	    read_spindle.tail = NULL; // prevent double-free
	}
        free(t);
    }
    read_spindle.head = NULL;
    if (read_spindle.tail != NULL)
        free(read_spindle.tail);
    read_spindle.tail = NULL;
    read_spindle.complete = 0;
    spindle_index--;
}

/* local (static) versions */

#define check_index_range(j,s)						\
  if (j<0 || j > 65535) {							\
    luaL_error(L, "incorrect index value %d for tex.%s()", (int)j, s);  }


static const char *scan_integer_part(lua_State * L, const char *ss, int *ret,
                                     int *radix_ret)
{
    boolean negative = false;   /* should the answer be negated? */
    int m;                      /* |$2^{31}$ / radix|, the threshold of danger */
    int d;                      /* the digit just scanned */
    boolean vacuous;            /* have no digits appeared? */
    boolean OK_so_far;          /* has an error message been issued? */
    int radix1 = 0;              /* the radix of the integer */
    int c = 0;                  /* the current character */
    const char *s;              /* where we stopped in the string |ss| */
    integer val = 0;        /* return value */
    s = ss;
    do {
        do {
            c = *s++;
        } while (c && c == ' ');
        if (c == '-') {
            negative = !negative;
            c = '+';
        }
    } while (c == '+');


    radix1 = 10;
    m = 214748364;
    if (c == '\'') {
        radix1 = 8;
        m = 02000000000;
        c = *s++;
    } else if (c == '"') {
        radix1 = 16;
        m = 01000000000;
        c = *s++;
    }
    vacuous = true;
    val = 0;
    OK_so_far = true;

    /* Accumulate the constant until |cur_tok| is not a suitable digit */
    while (1) {
        if ((c < '0' + radix1) && (c >= '0') && (c <= '0' + 9)) {
            d = c - '0';
        } else if (radix1 == 16) {
            if ((c <= 'A' + 5) && (c >= 'A')) {
                d = c - 'A' + 10;
            } else if ((c <= 'a' + 5) && (c >= 'a')) {
                d = c - 'a' + 10;
            } else {
                break;
            }
        } else {
            break;
        }
        vacuous = false;
        if ((val >= m) && ((val > m) || (d > 7) || (radix1 != 10))) {
            if (OK_so_far) {
		luaL_error(L, "Number too big");
                val = infinity;
                OK_so_far = false;
            }
        } else {
            val = val * radix1 + d;
        }
        c = *s++;
    }
    if (vacuous) {
        /* Express astonishment that no number was here */
        luaL_error(L, "Missing number, treated as zero");
    }
    if (negative)
        val = -val;
    *ret = val;
    *radix_ret = radix1;
    if (c != ' ' && s > ss)
        s--;
    return s;
}

#define set_conversion(A,B) do { num=(A); denom=(B); } while(0)


static const char *scan_dimen_part(lua_State * L, const char *ss, int *ret)
/* sets |cur_val| to a dimension */
{
    boolean negative = false;   /* should the answer be negated? */
    int f = 0;                  /* numerator of a fraction whose denominator is $2^{16}$ */
    int num, denom;             /* conversion ratio for the scanned units */
    int k;                      /* number of digits in a decimal fraction */
    scaled v;                   /* an internal dimension */
    int save_cur_val;           /* temporary storage of |cur_val| */
    int c;                      /* the current character */
    const char *s = ss;         /* where we are in the string */
    int radix1 = 0;              /* the current radix */
    int rdig[18];               /* to save the |dig[]| array */
    int saved_tex_remainder;    /* to save |tex_remainder|  */
    int saved_arith_error;      /* to save |arith_error|  */
    int saved_cur_val;          /* to save the global |cur_val| */
    saved_tex_remainder = tex_remainder;
    saved_arith_error = arith_error;
    saved_cur_val = cur_val;
    /* Get the next non-blank non-sign... */
    do {
        /* Get the next non-blank non-call token */
        do {
            c = *s++;
        } while (c && c == ' ');
        if (c == '-') {
            negative = !negative;
            c = '+';
        }
    } while (c == '+');

    if (c == ',') {
        c = '.';
    }
    if (c != '.') {
        s = scan_integer_part(L, (s > ss ? (s - 1) : ss), &cur_val, &radix1);
        c = *s;
    } else {
        radix1 = 10;
        cur_val = 0;
        c = *(--s);
    }
    if (c == ',')
        c = '.';
    if ((radix1 == 10) && (c == '.')) {
        /* Scan decimal fraction */
        for (k = 0; k < 18; k++)
            rdig[k] = dig[k];
        k = 0;
        s++;                    /* get rid of the '.' */
        while (1) {
            c = *s++;
            if ((c > '0' + 9) || (c < '0'))
                break;
            if (k < 17) {       /* digits for |k>=17| cannot affect the result */
                dig[k++] = c - '0';
            }
        }
        f = round_decimals(k);
        if (c != ' ')
            c = *(--s);
        for (k = 0; k < 18; k++)
            dig[k] = rdig[k];
    }
    if (cur_val < 0) {          /* in this case |f=0| */
        negative = !negative;
        cur_val = -cur_val;
    }

    /* Scan for (u)units that are internal dimensions;
       |goto attach_sign| with |cur_val| set if found */
    save_cur_val = cur_val;
    /* Get the next non-blank non-call... */
    do {
        c = *s++;
    } while (c && c == ' ');
    if (c != ' ')
        c = *(--s);
    if (strncmp(s, "em", 2) == 0) {
        s += 2;
        v = (quad(get_cur_font()));
    } else if (strncmp(s, "ex", 2) == 0) {
        s += 2;
        v = (x_height(get_cur_font()));
    } else if (strncmp(s, "px", 2) == 0) {
        s += 2;
        v = dimen_par(pdf_px_dimen_code);
    } else {
        goto NOT_FOUND;
    }
    c = *s++;
    if (c != ' ') {
        c = *(--s);
    }
    cur_val = nx_plus_y(save_cur_val, v, xn_over_d(v, f, 0200000));
    goto ATTACH_SIGN;
  NOT_FOUND:

    /* Scan for (m)\.{mu} units and |goto attach_fraction| */
    if (strncmp(s, "mu", 2) == 0) {
        s += 2;
        goto ATTACH_FRACTION;
    }
    if (strncmp(s, "true", 4) == 0) {
        /* Adjust (f)for the magnification ratio */
        s += 4;
        prepare_mag();
        if (int_par(mag_code) != 1000) {
            cur_val = xn_over_d(cur_val, 1000, int_par(mag_code));
            f = (1000 * f + 0200000 * tex_remainder) / int_par(mag_code);
            cur_val = cur_val + (f / 0200000);
            f = f % 0200000;
        }
        do {
            c = *s++;
        } while (c && c == ' ');
        c = *(--s);
    }
    if (strncmp(s, "pt", 2) == 0) {
        s += 2;
        goto ATTACH_FRACTION;   /* the easy case */
    }
    /* Scan for (a)all other units and adjust |cur_val| and |f| accordingly;
       |goto done| in the case of scaled points */

    if (strncmp(s, "in", 2) == 0) {
        s += 2;
        set_conversion(7227, 100);
    } else if (strncmp(s, "pc", 2) == 0) {
        s += 2;
        set_conversion(12, 1);
    } else if (strncmp(s, "cm", 2) == 0) {
        s += 2;
        set_conversion(7227, 254);
    } else if (strncmp(s, "mm", 2) == 0) {
        s += 2;
        set_conversion(7227, 2540);
    } else if (strncmp(s, "bp", 2) == 0) {
        s += 2;
        set_conversion(7227, 7200);
    } else if (strncmp(s, "dd", 2) == 0) {
        s += 2;
        set_conversion(1238, 1157);
    } else if (strncmp(s, "cc", 2) == 0) {
        s += 2;
        set_conversion(14856, 1157);
    } else if (strncmp(s, "nd", 2) == 0) {
        s += 2;
        set_conversion(685, 642);
    } else if (strncmp(s, "nc", 2) == 0) {
        s += 2;
        set_conversion(1370, 107);
    } else if (strncmp(s, "sp", 2) == 0) {
        s += 2;
        goto DONE;
    } else {
        /* Complain about unknown unit and |goto done2| */
        luaL_error(L, "Illegal unit of measure (pt inserted)");
        goto DONE2;
    }
    cur_val = xn_over_d(cur_val, num, denom);
    f = (num * f + 0200000 * tex_remainder) / denom;
    cur_val = cur_val + (f / 0200000);
    f = f % 0200000;
  DONE2:
  ATTACH_FRACTION:
    if (cur_val >= 040000)
        arith_error = true;
    else
        cur_val = cur_val * 65536 + f;
  DONE:
    /* Scan an optional space */
    c = *s++;
    if (c != ' ')
        s--;
  ATTACH_SIGN:
    if (arith_error || (abs(cur_val) >= 010000000000)) {
        /* Report that this dimension is out of range */
        luaL_error(L, "Dimension too large");
        cur_val = max_dimen;
    }
    if (negative)
        cur_val = -cur_val;
    *ret = cur_val;
    tex_remainder = saved_tex_remainder;
    arith_error = saved_arith_error;
    cur_val = saved_cur_val;
    return s;
}

int dimen_to_number(lua_State * L, const char *s)
{
    int j = 0;
    const char *d = scan_dimen_part(L, s, &j);
    if (*d) {
        luaL_error(L, "conversion failed (trailing junk?)");
        j = 0;
    }
    return j;
}


static int tex_scaledimen(lua_State * L)
{                               /* following vsetdimen() */
    int sp;
    if (!lua_isnumber(L, 1)) {
        if (lua_isstring(L, 1)) {
            sp = dimen_to_number(L, lua_tostring(L, 1));
        } else {
            luaL_error(L, "argument must be a string or a number");
            return 0;
        }
    } else {
        sp=(int)lua_tonumber(L, 1);
    }
    lua_pushnumber(L, sp);
    return 1;
}

static int texerror (lua_State * L)
{
    int i, n, l;
    const char **errhlp = NULL;
    const char *error = luaL_checkstring(L,1);
    n = lua_gettop(L);
    if (n==2 && lua_type(L, n) == LUA_TTABLE) {
        l = 1; /* |errhlp| is terminated by a NULL entry */
        for (i = 1;; i++) {
            lua_rawgeti(L, n, i);
            if (lua_isstring(L, -1)) {
                l++;
                lua_pop(L, 1);
            } else {
                lua_pop(L, 1);
                break;
            }
        }
        if (l>1) {
          errhlp = xmalloc(l * sizeof(char *));
          memset(errhlp,0,l * sizeof(char *));
          for (i = 1;; i++) {
            lua_rawgeti(L, n, i);
            if (lua_isstring(L, -1)) {
                errhlp[(i-1)] = lua_tostring(L,-1);
                lua_pop(L, 1);
            } else {
                break;
            }
	  }
	}
    }
    deletions_allowed = false;
    tex_error(error, errhlp);
    if (errhlp)
      xfree(errhlp);
    deletions_allowed = true;
    return 0;
}

static int get_item_index(lua_State * L, int i, int base)
{
    size_t kk;
    int k;
    int cur_cs1;
    const char *s;
    switch (lua_type(L, i)) {
    case LUA_TSTRING:
        s = lua_tolstring(L, i, &kk);
        cur_cs1 = string_lookup(s, kk);
        if (cur_cs1 == undefined_control_sequence || cur_cs1 == undefined_cs_cmd)
            k = -1;             /* guarandeed invalid */
        else
            k = (equiv(cur_cs1) - base);
        break;
    case LUA_TNUMBER:
        k = (int) luaL_checkinteger(L, i);
        break;
    default:
        luaL_error(L, "argument must be a string or a number");
        k = -1;                 /* not a valid index */
    }
    return k;
}

static int vsetdimen(lua_State * L, int is_global)
{
    int i, j, err;
    int k;
    int save_global_defs = int_par(global_defs_code);
    if (is_global)
        int_par(global_defs_code) = 1;
    i = lua_gettop(L);
    j = 0;
    /* find the value */
    if (!lua_isnumber(L, i)) {
        if (lua_isstring(L, i)) {
            j = dimen_to_number(L, lua_tostring(L, i));
        } else {
            luaL_error(L, "unsupported value type");
        }
    } else {
        j=(int)lua_tonumber(L, i);
    }
    k = get_item_index(L, (i - 1), scaled_base);
    check_index_range(k, "setdimen");
    err = set_tex_dimen_register(k, j);
    int_par(global_defs_code) = save_global_defs;
    if (err) {
        luaL_error(L, "incorrect value");
    }
    return 0;
}

static int setdimen(lua_State * L)
{
    int isglobal = 0;
    int n = lua_gettop(L);
    if (n == 3 && lua_isstring(L, 1)) {
        const char *s = lua_tostring(L, 1);
        if (lua_key_eq(s,global))
            isglobal = 1;
    }
    return vsetdimen(L, isglobal);
}

static int getdimen(lua_State * L)
{
    int j;
    int k;
    k = get_item_index(L, lua_gettop(L), scaled_base);
    check_index_range(k, "getdimen");
    j = get_tex_dimen_register(k);
    lua_pushnumber(L, j);
    return 1;
}

static int vsetskip(lua_State * L, int is_global)
{
    int i, err;
    halfword *j;
    int k;
    int save_global_defs = int_par(global_defs_code);
    if (is_global)
        int_par(global_defs_code) = 1;
    i = lua_gettop(L);
    j = check_isnode(L, i);     /* the value */
    k = get_item_index(L, (i - 1), skip_base);
    check_index_range(k, "setskip");    /* the index */
    err = set_tex_skip_register(k, *j);
    int_par(global_defs_code) = save_global_defs;
    if (err) {
        luaL_error(L, "incorrect value");
    }
    return 0;
}

static int setskip(lua_State * L)
{
    int isglobal = 0;
    int n = lua_gettop(L);
    if (n == 3 && lua_isstring(L, 1)) {
        const char *s = lua_tostring(L, 1);
        if (lua_key_eq(s,global))
            isglobal = 1;
    }
    return vsetskip(L, isglobal);
}

static int getskip(lua_State * L)
{
    halfword j;
    int k;
    k = get_item_index(L, lua_gettop(L), skip_base);
    check_index_range(k, "getskip");
    j = get_tex_skip_register(k);
    lua_nodelib_push_fast(L, j);
    return 1;
}



static int vsetcount(lua_State * L, int is_global)
{
    int i, j, err;
    int k;
    int save_global_defs = int_par(global_defs_code);
    if (is_global)
        int_par(global_defs_code) = 1;
    i = lua_gettop(L);
    j = (int) luaL_checkinteger(L, i);
    k = get_item_index(L, (i - 1), count_base);
    check_index_range(k, "setcount");
    err = set_tex_count_register(k, j);
    int_par(global_defs_code) = save_global_defs;
    if (err) {
        luaL_error(L, "incorrect value");
    }
    return 0;
}

static int setcount(lua_State * L)
{
    int isglobal = 0;
    int n = lua_gettop(L);
    if (n == 3 && lua_isstring(L, 1)) {
        const char *s = lua_tostring(L, 1);
        if (lua_key_eq(s,global))
            isglobal = 1;
    }
    return vsetcount(L, isglobal);
}

static int getcount(lua_State * L)
{
    int j;
    int k;
    k = get_item_index(L, lua_gettop(L), count_base);
    check_index_range(k, "getcount");
    j = get_tex_count_register(k);
    lua_pushnumber(L, j);
    return 1;
}


static int vsetattribute(lua_State * L, int is_global)
{
    int i, j, err;
    int k;
    int save_global_defs = int_par(global_defs_code);
    if (is_global)
        int_par(global_defs_code) = 1;
    i = lua_gettop(L);
    j = (int) luaL_checkinteger(L, i);
    k = get_item_index(L, (i - 1), attribute_base);
    check_index_range(k, "setattribute");
    err = set_tex_attribute_register(k, j);
    int_par(global_defs_code) = save_global_defs;
    if (err) {
        luaL_error(L, "incorrect value");
    }
    return 0;
}

static int setattribute(lua_State * L)
{
    int isglobal = 0;
    int n = lua_gettop(L);
    if (n == 3 && lua_isstring(L, 1)) {
        const char *s = lua_tostring(L, 1);
        if (lua_key_eq(s,global))
            isglobal = 1;
    }
    return vsetattribute(L, isglobal);
}

static int getattribute(lua_State * L)
{
    int j;
    int k;
    k = get_item_index(L, lua_gettop(L), attribute_base);
    check_index_range(k, "getattribute");
    j = get_tex_attribute_register(k);
    lua_pushnumber(L, j);
    return 1;
}

static int vsettoks(lua_State * L, int is_global)
{
    int i, err;
    int k;
    lstring str;
    char *s;
    const char *ss;
    int save_global_defs = int_par(global_defs_code);
    if (is_global)
        int_par(global_defs_code) = 1;
    i = lua_gettop(L);
    if (!lua_isstring(L, i)) {
        luaL_error(L, "unsupported value type");
    }
    ss = lua_tolstring(L, i, &str.l);
    s = xmalloc (str.l+1);
    memcpy (s, ss, str.l+1);
    str.s = (unsigned char *)s;
    k = get_item_index(L, (i - 1), toks_base);
    check_index_range(k, "settoks");
    err = set_tex_toks_register(k, str);
    xfree(str.s);
    int_par(global_defs_code) = save_global_defs;
    if (err) {
        luaL_error(L, "incorrect value");
    }
    return 0;
}

static int settoks(lua_State * L)
{
    int isglobal = 0;
    int n = lua_gettop(L);
    if (n == 3 && lua_isstring(L, 1)) {
        const char *s = lua_tostring(L, 1);
        if (lua_key_eq(s,global))
            isglobal = 1;
    }
    return vsettoks(L, isglobal);
}

static int gettoks(lua_State * L)
{
    int k;
    str_number t;
    char *ss;
    k = get_item_index(L, lua_gettop(L), toks_base);
    check_index_range(k, "gettoks");
    t = get_tex_toks_register(k);
    ss = makecstring(t);
    lua_pushstring(L, ss);
    free(ss);
    flush_str(t);
    return 1;
}

static int get_box_id(lua_State * L, int i)
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

static int getbox(lua_State * L)
{
    int k, t;
    k = get_box_id(L, -1);
    check_index_range(k, "getbox");
    t = get_tex_box_register(k);
    nodelist_to_lua(L, t);
    return 1;
}

static int vsetbox(lua_State * L, int is_global)
{
    int j, k, err;
    int save_global_defs = int_par(global_defs_code);
    if (is_global)
        int_par(global_defs_code) = 1;
    k = get_box_id(L, -2);
    check_index_range(k, "setbox");
    if (lua_isboolean(L, -1)) {
        j = lua_toboolean(L, -1);
        if (j == 0)
            j = null;
        else
            return 0;
    } else {
        j = nodelist_from_lua(L);
        if (j != null && type(j) != hlist_node && type(j) != vlist_node) {
            luaL_error(L, "setbox: incompatible node type (%s)\n",
                            get_node_name(type(j), subtype(j)));
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

static int setbox(lua_State * L)
{
    int isglobal = 0;
    int n = lua_gettop(L);
    if (n == 3 && lua_isstring(L, 1)) {
        const char *s = lua_tostring(L, 1);
        if (lua_key_eq(s,global))
            isglobal = 1;
    }
    return vsetbox(L, isglobal);
}

#define check_char_range(j,s,lim)					\
    if (j<0 || j >= lim) {						\
	luaL_error(L, "incorrect character value %d for tex.%s()", (int)j, s);  }


static int setcode (lua_State *L, void (*setone)(int,halfword,quarterword),
		    void (*settwo)(int,halfword,quarterword), const char *name, int lim)
{
    int ch;
    halfword val, ucval;
    int level = cur_level;
    int n = lua_gettop(L);
    int f = 1;
    if (n>1 && lua_type(L,1) == LUA_TTABLE)
	f++;
    if (n>2 && lua_isstring(L, f)) {
        const char *s = lua_tostring(L, f);
        if (lua_key_eq(s,global)) {
            level = level_one;
	    f++;
	}
    }
    ch = (int) luaL_checkinteger(L, f);
    check_char_range(ch, name, 65536*17);
    val = (halfword) luaL_checkinteger(L, f+1);
    check_char_range(val, name, lim);
    (setone)(ch, val, level);
    if (settwo != NULL && n-f == 2) {
	ucval = (halfword) luaL_checkinteger(L, f+2);
        check_char_range(ucval, name, lim);
	(settwo)(ch, ucval, level);
    }
    return 0;
}

static int setlccode(lua_State * L)
{
    return setcode(L, &set_lc_code, &set_uc_code, "setlccode",  65536*17);
}

static int getlccode(lua_State * L)
{
    int ch = (int) luaL_checkinteger(L, -1);
    check_char_range(ch, "getlccode", 65536*17);
    lua_pushnumber(L, get_lc_code(ch));
    return 1;
}

static int setuccode(lua_State * L)
{
    return setcode(L, &set_uc_code, &set_lc_code, "setuccode", 65536*17);
}

static int getuccode(lua_State * L)
{
    int ch = (int) luaL_checkinteger(L, -1);
    check_char_range(ch, "getuccode",  65536*17);
    lua_pushnumber(L, get_uc_code(ch));
    return 1;
}

static int setsfcode(lua_State * L)
{
    return setcode(L, &set_sf_code, NULL, "setsfcode", 32768);
}

static int getsfcode(lua_State * L)
{
    int ch = (int) luaL_checkinteger(L, -1);
    check_char_range(ch, "getsfcode",  65536*17);
    lua_pushnumber(L, get_sf_code(ch));
    return 1;
}

static int setcatcode(lua_State * L)
{
    int ch;
    halfword val;
    int level = cur_level;
    int cattable = int_par(cat_code_table_code);
    int n = lua_gettop(L);
    int f = 1;
    if (n>1 && lua_type(L,1) == LUA_TTABLE)
	f++;
    if (n>2 && lua_isstring(L, f)) {
        const char *s = lua_tostring(L, f);
        if (lua_key_eq(s,global)) {
            level = level_one;
	    f++;
	}
    }
    if (n-f == 2) {
	cattable = (int) luaL_checkinteger(L, -3);
    }
    ch = (int) luaL_checkinteger(L, -2);
    check_char_range(ch, "setcatcode", 65536*17);
    val = (halfword) luaL_checkinteger(L, -1);
    check_char_range(val, "setcatcode", 16);
    set_cat_code(cattable, ch, val, level);
    return 0;
}

static int getcatcode(lua_State * L)
{
    int cattable = int_par(cat_code_table_code);
    int ch = (int) luaL_checkinteger(L, -1);
    if (lua_gettop(L)>=2 && lua_type(L,-2)==LUA_TNUMBER) {
	cattable = luaL_checkinteger(L, -2);
    }
    check_char_range(ch, "getcatcode",  65536*17);
    lua_pushnumber(L, get_cat_code(cattable, ch));
    return 1;
}


static int setmathcode(lua_State * L)
{
    int ch;
    halfword cval, fval, chval;
    int level = cur_level;
    int n = lua_gettop(L);
    int f = 1;
    if (n>1 && lua_type(L,1) == LUA_TTABLE)
	f++;
    if (n>2 && lua_isstring(L, f)) {
        const char *s = lua_tostring(L, f);
        if (lua_key_eq(s,global)) {
            level = level_one;
	    f++;
	}
    }
    if (n-f!=1 || lua_type(L,f+1) != LUA_TTABLE) {
	luaL_error(L, "Bad arguments for tex.setmathcode()");
    }
    ch = (int) luaL_checkinteger(L, -2);
    check_char_range(ch, "setmathcode", 65536*17);

    lua_rawgeti(L, -1, 1);
    cval = (halfword) luaL_checkinteger(L, -1);
    lua_rawgeti(L, -2, 2);
    fval = (halfword) luaL_checkinteger(L, -1);
    lua_rawgeti(L, -3, 3);
    chval = (halfword) luaL_checkinteger(L, -1);
    lua_pop(L,3);

    check_char_range(cval, "setmathcode", 8);
    check_char_range(fval, "setmathcode", 256);
    check_char_range(chval, "setmathcode", 65536*17);
    set_math_code(ch, umath_mathcode, cval,fval, chval, (quarterword) (level));
    return 0;
}

static int getmathcode(lua_State * L)
{
    mathcodeval mval = { 0, 0, 0, 0 };
    int ch = (int) luaL_checkinteger(L, -1);
    check_char_range(ch, "getmathcode",  65536*17);
    mval = get_math_code(ch);
    lua_newtable(L);
    lua_pushnumber(L,mval.class_value);
    lua_rawseti(L, -2, 1);
    lua_pushnumber(L,mval.family_value);
    lua_rawseti(L, -2, 2);
    lua_pushnumber(L,mval.character_value);
    lua_rawseti(L, -2, 3);
    return 1;
}



static int setdelcode(lua_State * L)
{
    int ch;
    halfword sfval, scval, lfval, lcval;
    int level = cur_level;
    int n = lua_gettop(L);
    int f = 1;
    if (n>1 && lua_type(L,1) == LUA_TTABLE)
	f++;
    if (n>2 && lua_isstring(L, f)) {
        const char *s = lua_tostring(L, f);
        if (lua_key_eq(s,global)) {
            level = level_one;
	    f++;
	}
    }
    if (n-f!=1 || lua_type(L,f+1) != LUA_TTABLE) {
	luaL_error(L, "Bad arguments for tex.setdelcode()");
    }
    ch = (int) luaL_checkinteger(L, -2);
    check_char_range(ch, "setdelcode", 65536*17);
    lua_rawgeti(L, -1, 1);
    sfval = (halfword) luaL_checkinteger(L, -1);
    lua_rawgeti(L, -2, 2);
    scval = (halfword) luaL_checkinteger(L, -1);
    lua_rawgeti(L, -3, 3);
    lfval = (halfword) luaL_checkinteger(L, -1);
    lua_rawgeti(L, -4, 4);
    lcval = (halfword) luaL_checkinteger(L, -1);
    lua_pop(L,4);

    check_char_range(sfval, "setdelcode", 256);
    check_char_range(scval, "setdelcode", 65536*17);
    check_char_range(lfval, "setdelcode", 256);
    check_char_range(lcval, "setdelcode", 65536*17);
    set_del_code(ch, umath_mathcode, sfval, scval, lfval, lcval, (quarterword) (level));

    return 0;
}

static int getdelcode(lua_State * L)
{
    delcodeval mval = { 0, 0, 0, 0, 0, 0 };
    int ch = (int) luaL_checkinteger(L, -1);
    check_char_range(ch, "getdelcode",  65536*17);
    mval = get_del_code(ch);
    /* lua_pushnumber(L, mval.class_value); */
    /* lua_pushnumber(L, mval.origin_value); */
    lua_newtable(L);
    lua_pushnumber(L,mval.small_family_value);
    lua_rawseti(L, -2, 1);
    lua_pushnumber(L,mval.small_character_value);
    lua_rawseti(L, -2, 2);
    lua_pushnumber(L,mval.large_family_value);
    lua_rawseti(L, -2, 3);
    lua_pushnumber(L,mval.large_character_value);
    lua_rawseti(L, -2, 4);
    return 1;
}



static int settex(lua_State * L)
{
    const char *st;
    int i, j, texstr;
    size_t k;
    int cur_cs1, cur_cmd1;
    int isglobal = 0;
    j = 0;
    i = lua_gettop(L);
    if (lua_isstring(L, (i - 1))) {
        st = lua_tolstring(L, (i - 1), &k);
        texstr = maketexlstring(st, k);
        if (is_primitive(texstr)) {
            if (i == 3 && lua_isstring(L, 1)) {
                const char *s = lua_tostring(L, 1);
                if (lua_key_eq(s,global))
                    isglobal = 1;
            }
            cur_cs1 = string_lookup(st, k);
            flush_str(texstr);
            cur_cmd1 = eq_type(cur_cs1);
            if (is_int_assign(cur_cmd1)) {
                if (lua_isnumber(L, i)) {
                    int luai;
                    luai=(int)lua_tonumber(L, i);
                    assign_internal_value((isglobal ? 4 : 0),
                                          equiv(cur_cs1), luai);
                } else {
                    luaL_error(L, "unsupported value type");
                }
            } else if (is_dim_assign(cur_cmd1)) {
                if (!lua_isnumber(L, i)) {
                    if (lua_isstring(L, i)) {
                        j = dimen_to_number(L, lua_tostring(L, i));
                    } else {
                        luaL_error(L, "unsupported value type");
                    }
                } else {
                    j=(int)lua_tonumber(L, i);
                }
                assign_internal_value((isglobal ? 4 : 0), equiv(cur_cs1), j);
            } else if (is_glue_assign(cur_cmd1)) {
                halfword *j1 = check_isnode(L, i);     /* the value */
                    { int a = isglobal;
   		      define(equiv(cur_cs1), assign_glue_cmd, *j1);
                    }
            } else if (is_toks_assign(cur_cmd1)) {
                if (lua_isstring(L, i)) {
                    j = tokenlist_from_lua(L);  /* uses stack -1 */
                    assign_internal_value((isglobal ? 4 : 0), equiv(cur_cs1), j);

                } else {
                    luaL_error(L, "unsupported value type");
                }

            } else {
		/* people may want to add keys that are also primitives
		   (|tex.wd| for example) so creating an error is not
		   right here */
		if (lua_istable(L, (i - 2)))
		    lua_rawset(L, (i - 2));
                /* luaL_error(L, "unsupported tex internal assignment"); */
            }
        } else {
            if (lua_istable(L, (i - 2)))
                lua_rawset(L, (i - 2));
        }
    } else {
        if (lua_istable(L, (i - 2)))
            lua_rawset(L, (i - 2));
    }
    return 0;
}

static int do_convert(lua_State * L, int cur_code)
{
    int texstr;
    int i = -1;
    char *str = NULL;
    switch (cur_code) {
    case pdf_creation_date_code:       /* ? */
    case pdf_insert_ht_code:   /* arg <register int> */
    case pdf_ximage_bbox_code: /* arg 2 ints */
    case lua_code:             /* arg complex */
    case lua_escape_string_code:       /* arg token list */
    case pdf_colorstack_init_code:     /* arg complex */
    case left_margin_kern_code:        /* arg box */
    case right_margin_kern_code:       /* arg box */
        break;
    case string_code:          /* arg token */
    case meaning_code:         /* arg token */
        break;

        /* the next fall through, and come from 'official' indices! */
    case font_name_code:       /* arg fontid */
    case font_identifier_code: /* arg fontid */
    case pdf_font_name_code:   /* arg fontid */
    case pdf_font_objnum_code: /* arg fontid */
    case pdf_font_size_code:   /* arg fontid */
    case uniform_deviate_code: /* arg int */
    case number_code:          /* arg int */
    case roman_numeral_code:   /* arg int */
    case pdf_page_ref_code:    /* arg int */
    case pdf_xform_name_code:  /* arg int */
        if (lua_gettop(L) < 1) {
            /* error */
        }
        i=(int)lua_tonumber(L, 1);  /* these fall through! */
    default:
        texstr = the_convert_string(cur_code, i);
        if (texstr) {
            str = makecstring(texstr);
            flush_str(texstr);
        }
    }
    if (str) {
        lua_pushstring(L, str);
        free(str);
    } else {
        lua_pushnil(L);
    }
    return 1;
}


static int do_scan_internal(lua_State * L, int cur_cmd1, int cur_code)
{
    int texstr;
    char *str = NULL;
    int save_cur_val, save_cur_val_level;
    save_cur_val = cur_val;
    save_cur_val_level = cur_val_level;
    scan_something_simple(cur_cmd1, cur_code);

    if (cur_val_level == int_val_level ||
        cur_val_level == dimen_val_level || cur_val_level == attr_val_level) {
        lua_pushnumber(L, cur_val);
    } else if (cur_val_level == glue_val_level) {
        lua_nodelib_push_fast(L, cur_val);
    } else {                    /* dir_val_level, mu_val_level, tok_val_level */
        texstr = the_scanned_result();
        str = makecstring(texstr);
        if (str) {
            lua_pushstring(L, str);
            free(str);
        } else {
            lua_pushnil(L);
        }
        flush_str(texstr);
    }
    cur_val = save_cur_val;
    cur_val_level = save_cur_val_level;
    return 1;
}

static int do_lastitem(lua_State * L, int cur_code)
{
    int retval = 1;
    switch (cur_code) {
        /* the next two do not actually exist */
    case lastattr_code:
    case attrexpr_code:
        lua_pushnil(L);
        break;
        /* the expressions do something complicated with arguments, yuck */
    case numexpr_code:
    case dimexpr_code:
    case glueexpr_code:
    case muexpr_code:
        lua_pushnil(L);
        break;
        /* these read a glue or muglue, todo */
    case mu_to_glue_code:
    case glue_to_mu_code:
    case glue_stretch_order_code:
    case glue_shrink_order_code:
    case glue_stretch_code:
    case glue_shrink_code:
        lua_pushnil(L);
        break;
        /* these read a fontid and a char, todo */
    case font_char_wd_code:
    case font_char_ht_code:
    case font_char_dp_code:
    case font_char_ic_code:
        lua_pushnil(L);
        break;
        /* these read an integer, todo */
    case par_shape_length_code:
    case par_shape_indent_code:
    case par_shape_dimen_code:
        lua_pushnil(L);
        break;
    case lastpenalty_code:
    case lastkern_code:
    case lastskip_code:
    case last_node_type_code:
    case input_line_no_code:
    case badness_code:
    case pdf_last_obj_code:
    case pdf_last_xform_code:
    case pdf_last_ximage_code:
    case pdf_last_ximage_pages_code:
    case pdf_last_annot_code:
    case last_x_pos_code:
    case last_y_pos_code:
    case pdf_retval_code:
    case pdf_last_ximage_colordepth_code:
    case random_seed_code:
    case pdf_last_link_code:
    case luatex_version_code:
    case eTeX_minor_version_code:
    case eTeX_version_code:
    case current_group_level_code:
    case current_group_type_code:
    case current_if_level_code:
    case current_if_type_code:
    case current_if_branch_code:
        retval = do_scan_internal(L, last_item_cmd, cur_code);
        break;
    default:
        lua_pushnil(L);
        break;
    }
    return retval;
}

static int tex_setmathparm(lua_State * L)
{
    int i, j;
    int k;
    int n;
    int l = cur_level;
    n = lua_gettop(L);

    if ((n == 3) || (n == 4)) {
        if (n == 4 && lua_isstring(L, 1)) {
            const char *s = lua_tostring(L, 1);
            if (lua_key_eq(s,global))
                l = 1;
        }
        i = luaL_checkoption(L, (n - 2), NULL, math_param_names);
        j = luaL_checkoption(L, (n - 1), NULL, math_style_names);
        if (!lua_isnumber(L, n))
            luaL_error(L, "argument must be a number");
        k=(int)lua_tonumber(L, n);
        def_math_param(i, j, (scaled) k, l);
    }
    return 0;
}

static int tex_getmathparm(lua_State * L)
{
    int i, j;
    scaled k;
    if ((lua_gettop(L) == 2)) {
        i = luaL_checkoption(L, 1, NULL, math_param_names);
        j = luaL_checkoption(L, 2, NULL, math_style_names);
        k = get_math_param(i, j);
        lua_pushnumber(L, k);
    }
    return 1;
}

static int getfontname(lua_State * L)
{
    return do_convert(L, font_name_code);
}

static int getfontidentifier(lua_State * L)
{
    return do_convert(L, font_identifier_code);
}

static int getpdffontname(lua_State * L)
{
    return do_convert(L, pdf_font_name_code);
}

static int getpdffontobjnum(lua_State * L)
{
    return do_convert(L, pdf_font_objnum_code);
}

static int getpdffontsize(lua_State * L)
{
    return do_convert(L, pdf_font_size_code);
}

static int getuniformdeviate(lua_State * L)
{
    return do_convert(L, uniform_deviate_code);
}

static int getnumber(lua_State * L)
{
    return do_convert(L, number_code);
}

static int getromannumeral(lua_State * L)
{
    return do_convert(L, roman_numeral_code);
}

static int getpdfpageref(lua_State * L)
{
    return do_convert(L, pdf_page_ref_code);
}

static int getpdfxformname(lua_State * L)
{
    return do_convert(L, pdf_xform_name_code);
}


static int get_parshape(lua_State * L)
{
    int n;
    halfword par_shape_ptr = equiv(par_shape_loc);
    if (par_shape_ptr != 0) {
        int m = 1;
        n = vinfo(par_shape_ptr + 1);
        lua_createtable(L, n, 0);
        while (m <= n) {
            lua_createtable(L, 2, 0);
            lua_pushnumber(L, vlink((par_shape_ptr) + (2 * (m - 1)) + 2));
            lua_rawseti(L, -2, 1);
            lua_pushnumber(L, vlink((par_shape_ptr) + (2 * (m - 1)) + 3));
            lua_rawseti(L, -2, 2);
            lua_rawseti(L, -2, m);
            m++;
        }
    } else {
        lua_pushnil(L);
    }
    return 1;
}


static int gettex(lua_State * L)
{
    int cur_cs1 = -1;
    int retval = 1;             /* default is to return nil  */
    int t = lua_gettop(L);
    if (lua_isstring(L, t)) {   /* 1 == 'tex', 2 == 'boxmaxdepth', or 1 == 'boxmaxdepth' */
        int texstr;
        size_t k;
        const char *st = lua_tolstring(L, t, &k);
        texstr = maketexlstring(st, k);
        cur_cs1 = prim_lookup(texstr);   /* not found == relax == 0 */
        flush_str(texstr);
    }
    if (cur_cs1 > 0) {
        int cur_cmd1, cur_code;
        cur_cmd1 = get_prim_eq_type(cur_cs1);
        cur_code = get_prim_equiv(cur_cs1);
        switch (cur_cmd1) {
        case last_item_cmd:
            retval = do_lastitem(L, cur_code);
            break;
        case convert_cmd:
            retval = do_convert(L, cur_code);
            break;
        case assign_toks_cmd:
        case assign_int_cmd:
        case assign_attr_cmd:
        case assign_dir_cmd:
        case assign_dimen_cmd:
        case assign_glue_cmd:
        case assign_mu_glue_cmd:
        case set_aux_cmd:
        case set_prev_graf_cmd:
        case set_page_int_cmd:
        case set_page_dimen_cmd:
        case char_given_cmd:
        case math_given_cmd:
            retval = do_scan_internal(L, cur_cmd1, cur_code);
            break;
        case set_tex_shape_cmd:
            retval = get_parshape(L);
            break;
        default:
            lua_pushnil(L);
            break;
        }
    } else {
	if (t == 2) {
	    lua_rawget(L, 1);
	}
    }
    return retval;
}


static int getlist(lua_State * L)
{
    const char *str;
    if (lua_isstring(L, 2)) {
        str = lua_tostring(L, 2);
        if (lua_key_eq(str,page_ins_head)) {
            if (vlink(page_ins_head) == page_ins_head)
                lua_pushnumber(L, null);
            else
                lua_pushnumber(L, vlink(page_ins_head));
            lua_nodelib_push(L);
        } else if (lua_key_eq(str,contrib_head)) {
	    alink(vlink(contrib_head)) = null ;
            lua_pushnumber(L, vlink(contrib_head));
            lua_nodelib_push(L);
        } else if (lua_key_eq(str,page_head)) {
	    alink(vlink(page_head)) = null ;/*hh-ls */
            lua_pushnumber(L, vlink(page_head));
            lua_nodelib_push(L);
        } else if (lua_key_eq(str,temp_head)) {
	    alink(vlink(temp_head)) = null ;/*hh-ls */
            lua_pushnumber(L, vlink(temp_head));
            lua_nodelib_push(L);
        } else if (lua_key_eq(str,hold_head)) {
            alink(vlink(hold_head)) = null ;/*hh-ls */
            lua_pushnumber(L, vlink(hold_head));
            lua_nodelib_push(L);
        } else if (lua_key_eq(str,adjust_head)) {
            alink(vlink(adjust_head)) = null ;/*hh-ls */
            lua_pushnumber(L, vlink(adjust_head));
            lua_nodelib_push(L);
        } else if (lua_key_eq(str,best_page_break)) {
            lua_pushnumber(L, best_page_break);
            lua_nodelib_push(L);
        } else if (lua_key_eq(str,least_page_cost)) {
            lua_pushnumber(L, least_page_cost);
        } else if (lua_key_eq(str,best_size)) {
            lua_pushnumber(L, best_size);
        } else if (lua_key_eq(str,pre_adjust_head)) {
            alink(vlink(pre_adjust_head)) = null ;/*hh-ls */
            lua_pushnumber(L, vlink(pre_adjust_head));
            lua_nodelib_push(L);
        } else if (lua_key_eq(str,align_head)) {
            alink(vlink(align_head)) = null ;/*hh-ls */
            lua_pushnumber(L, vlink(align_head));
            lua_nodelib_push(L);
        } else {
            lua_pushnil(L);
        }
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int setlist(lua_State * L)
{
    halfword *n_ptr;
    const char *str;
    halfword n = 0;
    if (lua_isstring(L, 2)) {
        str = lua_tostring(L, 2);
        if (lua_key_eq(str,best_size)) {
            best_size = (int) lua_tointeger(L, 3);
        } else if (lua_key_eq(str,least_page_cost)) {
            least_page_cost = (int) lua_tointeger(L, 3);
        } else {
            if (!lua_isnil(L, 3)) {
                n_ptr = check_isnode(L, 3);
                n = *n_ptr;
            }
            if (lua_key_eq(str,page_ins_head)) {
                if (n == 0) {
                    vlink(page_ins_head) = page_ins_head;
                } else {
                    halfword m;
                    vlink(page_ins_head) = n;
                    m = tail_of_list(n);
                    vlink(m) = page_ins_head;
                }
            } else if (lua_key_eq(str,contrib_head)) {
                vlink(contrib_head) = n;
                if (n == 0) {
                    contrib_tail = contrib_head;
                }
            } else if (lua_key_eq(str,best_page_break)) {
                best_page_break = n;
            } else if (lua_key_eq(str,page_head)) {
                vlink(page_head) = n;
                page_tail = (n == 0 ? page_head : tail_of_list(n));
            } else if (lua_key_eq(str,temp_head)) {
                vlink(temp_head) = n;
            } else if (lua_key_eq(str,hold_head)) {
                vlink(hold_head) = n;
            } else if (lua_key_eq(str,adjust_head)) {
                vlink(adjust_head) = n;
                adjust_tail = (n == 0 ? adjust_head : tail_of_list(n));
            } else if (lua_key_eq(str,pre_adjust_head)) {
                vlink(pre_adjust_head) = n;
                pre_adjust_tail = (n == 0 ? pre_adjust_head : tail_of_list(n));
            } else if (lua_key_eq(str,align_head)) {
                vlink(align_head) = n;
            }
        }
    }
    return 0;
}

#define NEST_METATABLE "luatex.nest"

static int lua_nest_getfield(lua_State * L)
{
    list_state_record *r, **rv = lua_touserdata(L, -2);
    const char *field = lua_tostring(L, -1);
    r = *rv;
    if (lua_key_eq(field,mode)) {
        lua_pushnumber(L, r->mode_field);
    } else if (lua_key_eq(field,head)) {
        lua_nodelib_push_fast(L, r->head_field);
    } else if (lua_key_eq(field,tail)) {
        lua_nodelib_push_fast(L, r->tail_field);
    } else if (lua_key_eq(field,delimptr)) {
        lua_pushnumber(L, r->eTeX_aux_field);
        lua_nodelib_push(L);
    } else if (lua_key_eq(field,prevgraf)) {
        lua_pushnumber(L, r->pg_field);
    } else if (lua_key_eq(field,modeline)) {
        lua_pushnumber(L, r->ml_field);
    } else if (lua_key_eq(field,prevdepth)) {
        lua_pushnumber(L, r->prev_depth_field);
    } else if (lua_key_eq(field,spacefactor)) {
        lua_pushnumber(L, r->space_factor_field);
    } else if (lua_key_eq(field,noad)) {
        lua_pushnumber(L, r->incompleat_noad_field);
        lua_nodelib_push(L);
    } else if (lua_key_eq(field,dirs)) {
        lua_pushnumber(L, r->dirs_field);
        lua_nodelib_push(L);
    } else if (lua_key_eq(field,mathdir)) {
        lua_pushboolean(L, r->math_field);
    } else if (lua_key_eq(field,mathstyle)) {
        lua_pushnumber(L, r->math_style_field);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int lua_nest_setfield(lua_State * L)
{
    halfword *n;
    int i;
    list_state_record *r, **rv = lua_touserdata(L, -3);
    const char *field = lua_tostring(L, -2);
    r = *rv;
    if (lua_key_eq(field,mode)) {
        i=(int)lua_tonumber(L, -1);
        r->mode_field = i;
    } else if (lua_key_eq(field,head)) {
        n = check_isnode(L, -1);
        r->head_field = *n;
    } else if (lua_key_eq(field,tail)) {
        n = check_isnode(L, -1);
        r->tail_field = *n;
    } else if (lua_key_eq(field,delimptr)) {
        n = check_isnode(L, -1);
        r->eTeX_aux_field = *n;
    } else if (lua_key_eq(field,prevgraf)) {
        i=(int)lua_tonumber(L, -1);
        r->pg_field = i;
    } else if (lua_key_eq(field,modeline)) {
        i=(int)lua_tonumber(L, -1);
        r->ml_field = i;
    } else if (lua_key_eq(field,prevdepth)) {
        i=(int)lua_tonumber(L, -1);
        r->prev_depth_field = i;
    } else if (lua_key_eq(field,spacefactor)) {
        i=(int)lua_tonumber(L, -1);
        r->space_factor_field = i;
    } else if (lua_key_eq(field,noad)) {
        n = check_isnode(L, -1);
        r->incompleat_noad_field = *n;
    } else if (lua_key_eq(field,dirs)) {
        n = check_isnode(L, -1);
        r->dirs_field = *n;
    } else if (lua_key_eq(field,mathdir)) {
        r->math_field = lua_toboolean(L, -1);
    } else if (lua_key_eq(field,mathstyle)) {
        i=(int)lua_tonumber(L, -1);
        r->math_style_field = i;
    }
    return 0;
}

static const struct luaL_Reg nest_m[] = {
    {"__index", lua_nest_getfield},
    {"__newindex", lua_nest_setfield},
    {NULL, NULL}                /* sentinel */
};

static void init_nest_lib(lua_State * L)
{
    luaL_newmetatable(L, NEST_METATABLE);
    luaL_register(L, NULL, nest_m);
    lua_pop(L, 1);
}

static int getnest(lua_State * L)
{
    int ptr;
    list_state_record **nestitem;
    if (lua_isnumber(L, 2)) {
        ptr=(int)lua_tonumber(L, 2);
        if (ptr >= 0 && ptr <= nest_ptr) {
            nestitem = lua_newuserdata(L, sizeof(list_state_record *));
            *nestitem = &nest[ptr];
            luaL_getmetatable(L, NEST_METATABLE);
            lua_setmetatable(L, -2);
        } else {
            lua_pushnil(L);
        }
    } else if (lua_isstring(L, 2)) {
        const char *s = lua_tostring(L, 2);
        if (lua_key_eq(s,ptr)) {
            lua_pushnumber(L, nest_ptr);
        } else {
            lua_pushnil(L);
        }
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int setnest(lua_State * L)
{
    luaL_error(L, "You can't modify the semantic nest array directly");
    return 2;
}

static int do_integer_error(double m)
{
    const char *help[] =
        { "I can only go up to 2147483647='17777777777=" "7FFFFFFF,",
        "so I'm using that number instead of yours.",
        NULL
    };
    tex_error("Number too big", help);
    return (m > 0.0 ? infinity : -infinity);
}


static int tex_roundnumber(lua_State * L)
{
    double m = (double) lua_tonumber(L, 1) + 0.5;
    if (abs(m) > (double) infinity)
        lua_pushnumber(L, do_integer_error(m));
    else
        lua_pushnumber(L, floor(m));
    return 1;
}

static int tex_scaletable(lua_State * L)
{
    double delta = luaL_checknumber(L, 2);
    if (lua_istable(L, 1)) {
        lua_newtable(L);        /* the new table is at index 3 */
        lua_pushnil(L);
        while (lua_next(L, 1) != 0) {   /* numeric value */
            lua_pushvalue(L, -2);
            lua_insert(L, -2);
            if (lua_isnumber(L, -1)) {
                double m = (double) lua_tonumber(L, -1) * delta + 0.5;
                lua_pop(L, 1);
                if (abs(m) > (double) infinity)
                    lua_pushnumber(L, do_integer_error(m));
                else
                    lua_pushnumber(L, floor(m));
            }
            lua_rawset(L, 3);
        }
    } else if (lua_isnumber(L, 1)) {
        double m = (double) lua_tonumber(L, 1) * delta + 0.5;
        if (abs(m) > (double) infinity)
            lua_pushnumber(L, do_integer_error(m));
        else
            lua_pushnumber(L, floor(m));
    } else {
        lua_pushnil(L);
    }
    return 1;
}

#define hash_text(A) hash[(A)].rh

static int tex_definefont(lua_State * L)
{
    const char *csname;
    int f, u;
    str_number t;
    size_t l;
    int i = 1;
    int a = 0;
    if (!no_new_control_sequence) {
        const char *help[] =
            { "You can't create a new font inside a \\csname\\endcsname pair",
            NULL
        };
        tex_error("Definition active", help);
    }
    if ((lua_gettop(L) == 3) && lua_isboolean(L, 1)) {
        a = lua_toboolean(L, 1);
        i = 2;
    }
    csname = luaL_checklstring(L, i, &l);
    f = (int) luaL_checkinteger(L, (i + 1));
    t = maketexlstring(csname, l);
    no_new_control_sequence = 0;
    u = string_lookup(csname, l);
    no_new_control_sequence = 1;
    if (a)
        geq_define(u, set_font_cmd, f);
    else
        eq_define(u, set_font_cmd, f);
    eqtb[font_id_base + f] = eqtb[u];
    hash_text(font_id_base + f) = t;
    return 0;
}

static int tex_hashpairs(lua_State * L)
{
    int cmd, chr;
    str_number s = 0;
    int cs = 1;
    lua_newtable(L);
    while (cs < hash_size) {
        s = hash_text(cs);
        if (s > 0) {
            char *ss = makecstring(s);
            lua_pushstring(L, ss);
            free(ss);
            cmd = eq_type(cs);
            chr = equiv(cs);
            make_token_table(L, cmd, chr, cs);
            lua_rawset(L, -3);
        }
        cs++;
    }
    return 1;
}

static int tex_primitives(lua_State * L)
{
    int cmd, chr;
    str_number s = 0;
    int cs = 0;
    lua_newtable(L);
    while (cs < prim_size) {
        s = get_prim_text(cs);
        if (s > 0) {
            char *ss = makecstring(s);
            lua_pushstring(L, ss);
            free(ss);
            cmd = get_prim_eq_type(cs);
            chr = get_prim_equiv(cs);
            make_token_table(L, cmd, chr, 0);
            lua_rawset(L, -3);
        }
        cs++;
    }
    return 1;
}

static int tex_extraprimitives(lua_State * L)
{
    int n, i;
    int mask = 0;
    int cs = 0;
    n = lua_gettop(L);
    if (n == 0) {
        mask = etex_command + pdftex_command + luatex_command + umath_command;
    } else {
        for (i = 1; i <= n; i++) {
            if (lua_isstring(L, i)) {
                const char *s = lua_tostring(L, i);
                if (lua_key_eq(s,etex)) {
                    mask |= etex_command;
                } else if (lua_key_eq(s,tex)) {
                    mask |= tex_command;
                } else if (lua_key_eq(s,core)) {
                    mask |= core_command;
                } else if (lua_key_eq(s,pdftex)) {
                    mask |= pdftex_command;
                } else if (lua_key_eq(s,luatex)) {
                    mask |= luatex_command | umath_command;
                } else if (lua_key_eq(s,umath)) {
                    mask |= umath_command;
                }
            }
        }
    }
    lua_newtable(L);
    i = 1;
    while (cs < prim_size) {
	str_number s = 0;
        s = get_prim_text(cs);
        if (s > 0) {
            if (get_prim_origin(cs) & mask) {
                char *ss = makecstring(s);
                lua_pushstring(L, ss);
                free(ss);
                lua_rawseti(L, -2, i++);
            }
        }
        cs++;
    }
    return 1;
}

static int tex_enableprimitives(lua_State * L)
{
    int n = lua_gettop(L);
    if (n != 2) {
        luaL_error(L, "wrong number of arguments");
    } else {
        size_t l;
        int i;
        const char *pre = luaL_checklstring(L, 1, &l);
        if (lua_istable(L, 2)) {
            int nncs = no_new_control_sequence;
            no_new_control_sequence = true;
            i = 1;
            while (1) {
                lua_rawgeti(L, 2, i);
                if (lua_isstring(L, 3)) {
                    const char *prim = lua_tostring(L, 3);
                    str_number s = maketexstring(prim);
                    halfword prim_val = prim_lookup(s);
                    if (prim_val != undefined_primitive) {
                        char *newprim;
                        int val;
                        size_t newl;
                        halfword cur_cmd1 = get_prim_eq_type(prim_val);
                        halfword cur_chr1 = get_prim_equiv(prim_val);
                        if (strncmp(pre, prim, l) != 0) {       /* not a prefix */
                            newl = strlen(prim) + l;
                            newprim = (char *) xmalloc((unsigned) (newl + 1));
                            strcpy(newprim, pre);
                            strcat(newprim + l, prim);
                        } else {
                            newl = strlen(prim);
                            newprim = (char *) xmalloc((unsigned) (newl + 1));
                            strcpy(newprim, prim);
                        }
                        val = string_lookup(newprim, newl);
                        if (val == undefined_control_sequence ||
                            eq_type(val) == undefined_cs_cmd) {
                            primitive_def(newprim, newl, (quarterword) cur_cmd1,
                                          cur_chr1);
                        }
                        free(newprim);
                    }
                    flush_str(s);
                } else {
                    lua_pop(L, 1);
                    break;
                }
                lua_pop(L, 1);
                i++;
            }
            lua_pop(L, 1);      /* the table */
            no_new_control_sequence = nncs;
        } else {
            luaL_error(L, "Expected an array of names as second argument");
        }
    }
    return 0;
}

#define get_int_par(A,B,C)  do {			\
    	lua_pushstring(L,(A));				\
	lua_gettable(L,-2);				\
	if (lua_type(L, -1) == LUA_TNUMBER) {		\
	    B=(int)lua_tonumber(L, -1);			\
	} else {					\
	    B = (C);					\
	}						\
	lua_pop(L,1);					\
    } while (0)


#define get_intx_par(A,B,C,D,E)  do {			\
    	lua_pushstring(L,(A));				\
	lua_gettable(L,-2);				\
	if (lua_type(L, -1) == LUA_TNUMBER) {		\
	    B=(int)lua_tonumber(L, -1);			\
	    D = null;					\
	} else if (lua_type(L, -1) == LUA_TTABLE){	\
	    B = 0;					\
	    D = nodelib_topenalties(L, lua_gettop(L));	\
	} else {					\
	    B = (C);					\
	    D = (E);					\
	}						\
	lua_pop(L,1);					\
    } while (0)

#define get_dimen_par(A,B,C)  do {			\
    	lua_pushstring(L,(A));				\
	lua_gettable(L,-2);				\
	if (lua_type(L, -1) == LUA_TNUMBER) {		\
	    B=(int)lua_tonumber(L, -1);			\
	} else {					\
	    B = (C);					\
	}						\
	lua_pop(L,1);					\
    } while (0)


#define get_glue_par(A,B,C)  do {			\
    	lua_pushstring(L,(A));				\
	lua_gettable(L,-2);				\
	if (lua_type(L, -1) != LUA_TNIL) {		\
	    B = *check_isnode(L, -1);			\
	} else {					\
	    B = (C);					\
	}						\
	lua_pop(L,1);					\
    } while (0)


static halfword nodelib_toparshape(lua_State * L, int i)
{
    halfword p;
    int n = 0;
    int width, indent, j;
    /* find |n| */
    lua_pushnil(L);
    while (lua_next(L, i) != 0) {
        n++;
        lua_pop(L, 1);
    }
    if (n == 0)
        return null;
    p = new_node(shape_node, 2 * (n + 1) + 1);
    vinfo(p + 1) = n;
    /* fill |p| */
    lua_pushnil(L);
    j = 0;
    while (lua_next(L, i) != 0) {
        /* don't give an error for non-tables, we may add special syntaxes at some point */
        j++;
        if (lua_type(L, i) == LUA_TTABLE) {
            lua_rawgeti(L, -1, 1);      /* indent */
            if (lua_type(L, -1) == LUA_TNUMBER) {
                indent=(int)lua_tonumber(L, -1);
                lua_pop(L, 1);
                lua_rawgeti(L, -1, 2);  /* width */
                if (lua_type(L, -1) == LUA_TNUMBER) {
                    width=(int)lua_tonumber(L, -1);
                    lua_pop(L, 1);
                    varmem[p + 2 * j].cint = indent;
                    varmem[p + 2 * j + 1].cint = width;
                }
            }
        }
        lua_pop(L, 1);
    }
    return p;
}

/* penalties */

static halfword nodelib_topenalties(lua_State * L, int i)
{
    halfword p;
    int n = 0;
    int j;
    /* find |n| */
    lua_pushnil(L);
    while (lua_next(L, i) != 0) {
        n++;
        lua_pop(L, 1);
    }
    if (n == 0)
        return null;
    p = new_node(shape_node, 2 * ((n / 2) + 1) + 1 + 1);
    vinfo(p + 1) = (n / 2) + 1;
    varmem[p + 2].cint = n;
    lua_pushnil(L);
    j = 2;
    while (lua_next(L, i) != 0) {
	j++;
	if (lua_isnumber(L, -1)) {
	    int pen = 0;
	    pen=(int)lua_tonumber(L, -1);
	    varmem[p+j].cint = pen;
	}
	lua_pop(L, 1);
    }
    if (!odd(n))
	varmem[p+j+1].cint = 0;
    return p;
}




static int tex_run_linebreak(lua_State * L)
{

    halfword *j;
    halfword p;
    halfword final_par_glue;
    int paragraph_dir = 0;
    /* locally initialized parameters for line breaking */
    int pretolerance, tracingparagraphs, tolerance, looseness,
        /*
        hyphenpenalty, exhyphenpenalty,
        */
        adjustspacing, adjdemerits, protrudechars,
        linepenalty, lastlinefit, doublehyphendemerits, finalhyphendemerits,
        hangafter, interlinepenalty, widowpenalty, clubpenalty, brokenpenalty;
    halfword emergencystretch, hangindent, hsize, leftskip, rightskip,parshape;
    int fewest_demerits = 0, actual_looseness = 0;
    halfword clubpenalties, interlinepenalties, widowpenalties;
    int save_vlink_tmp_head;
    /* push a new nest level */
    push_nest();
    save_vlink_tmp_head = vlink(temp_head);

    j = check_isnode(L, 1);     /* the value */
    vlink(temp_head) = *j;
    p = *j;
    if ((!is_char_node(vlink(*j)))
        && ((type(vlink(*j)) == whatsit_node)
            && (subtype(vlink(*j)) == local_par_node))) {
        paragraph_dir = local_par_dir(vlink(*j));
    }

    while (vlink(p) != null)
        p = vlink(p);
    final_par_glue = p;

    /* initialize local parameters */

    if (lua_gettop(L) != 2 || lua_type(L, 2) != LUA_TTABLE) {
        lua_checkstack(L, 3);
        lua_newtable(L);
    }
    lua_pushstring(L, "pardir");
    lua_gettable(L, -2);
    if (lua_type(L, -1) == LUA_TSTRING) {
        paragraph_dir = nodelib_getdir(L, -1, 1);
    }
    lua_pop(L, 1);

    lua_pushstring(L, "parshape");
    lua_gettable(L, -2);
    if (lua_type(L, -1) == LUA_TTABLE) {
        parshape = nodelib_toparshape(L, lua_gettop(L));
    } else {
        parshape = equiv(par_shape_loc);
    }
    lua_pop(L, 1);

    get_int_par("pretolerance", pretolerance, int_par(pretolerance_code));
    get_int_par("tracingparagraphs", tracingparagraphs,
                int_par(tracing_paragraphs_code));
    get_int_par("tolerance", tolerance, int_par(tolerance_code));
    get_int_par("looseness", looseness, int_par(looseness_code));
    /*
    get_int_par("hyphenpenalty", hyphenpenalty, int_par(hyphen_penalty_code));
    get_int_par("exhyphenpenalty", exhyphenpenalty,
                int_par(ex_hyphen_penalty_code));
    */
    get_int_par("adjustspacing", adjustspacing,
                int_par(adjust_spacing_code));
    get_int_par("adjdemerits", adjdemerits, int_par(adj_demerits_code));
    get_int_par("protrudechars", protrudechars,
                int_par(protrude_chars_code));
    get_int_par("linepenalty", linepenalty, int_par(line_penalty_code));
    get_int_par("lastlinefit", lastlinefit, int_par(last_line_fit_code));
    get_int_par("doublehyphendemerits", doublehyphendemerits,
                int_par(double_hyphen_demerits_code));
    get_int_par("finalhyphendemerits", finalhyphendemerits,
                int_par(final_hyphen_demerits_code));
    get_int_par("hangafter", hangafter, int_par(hang_after_code));
    get_intx_par("interlinepenalty", interlinepenalty,int_par(inter_line_penalty_code),
		 interlinepenalties, equiv(inter_line_penalties_loc));
    get_intx_par("clubpenalty", clubpenalty, int_par(club_penalty_code),
		 clubpenalties, equiv(club_penalties_loc));
    get_intx_par("widowpenalty", widowpenalty, int_par(widow_penalty_code),
		 widowpenalties, equiv(widow_penalties_loc));
    get_int_par("brokenpenalty", brokenpenalty, int_par(broken_penalty_code));
    get_dimen_par("emergencystretch", emergencystretch,
                  dimen_par(emergency_stretch_code));
    get_dimen_par("hangindent", hangindent, dimen_par(hang_indent_code));
    get_dimen_par("hsize", hsize, dimen_par(hsize_code));
    get_glue_par("leftskip", leftskip, glue_par(left_skip_code));
    get_glue_par("rightskip", rightskip, glue_par(right_skip_code));

    ext_do_line_break(paragraph_dir,
                      pretolerance, tracingparagraphs, tolerance,
                      emergencystretch, looseness,
                      /*
                      hyphenpenalty, exhyphenpenalty,
                      */
                      adjustspacing,
                      parshape,
                      adjdemerits, protrudechars,
                      linepenalty, lastlinefit,
                      doublehyphendemerits, finalhyphendemerits,
                      hangindent, hsize, hangafter, leftskip, rightskip,
                      interlinepenalties,
                      interlinepenalty, clubpenalty,
                      clubpenalties,
                      widowpenalties,
                      widowpenalty, brokenpenalty,
                      final_par_glue);

    /* return the generated list, and its prevdepth */
    get_linebreak_info (&fewest_demerits, &actual_looseness) ;
    lua_nodelib_push_fast(L, vlink(cur_list.head_field));
    lua_newtable(L);
    lua_pushstring(L, "demerits");
    lua_pushnumber(L, fewest_demerits);
    lua_settable(L, -3);
    lua_pushstring(L, "looseness");
    lua_pushnumber(L, actual_looseness);
    lua_settable(L, -3);
    lua_pushstring(L, "prevdepth");
    lua_pushnumber(L, cur_list.prev_depth_field);
    lua_settable(L, -3);
    lua_pushstring(L, "prevgraf");
    lua_pushnumber(L, cur_list.pg_field);
    lua_settable(L, -3);

    /* restore nest stack */
    vlink(temp_head) = save_vlink_tmp_head;
    pop_nest();
    if (parshape != equiv(par_shape_loc))
        flush_node(parshape);
    return 2;
}

static int tex_shipout(lua_State * L)
{
    int boxnum = get_box_id(L, 1);
    ship_out(static_pdf, box(boxnum), SHIPPING_PAGE);
    box(boxnum) = null;
    return 0;
}

static int tex_badness(lua_State * L)
{
    scaled t,s;
    t=(int)lua_tonumber(L,1);
    s=(int)lua_tonumber(L,2);
    lua_pushnumber(L, badness(t,s));
    return 1;
}


static int tex_run_boot(lua_State * L)
{
    int n = lua_gettop(L);
    const char *format = NULL;
    if (n >= 1) {
        ini_version = 0;
        format = luaL_checkstring(L, 1);
    } else {
        ini_version = 1;
    }
    if (main_initialize()) {    /* > 0 = failure */
        lua_pushboolean(L, 0);  /* false */
        return 1;
    }
    if (format) {
        if (!zopen_w_input(&fmt_file, format, DUMP_FORMAT, FOPEN_RBIN_MODE)) {
            lua_pushboolean(L, 0);      /* false */
            return 1;
        }
        if (!load_fmt_file(format)) {
            zwclose(fmt_file);
            lua_pushboolean(L, 0);      /* false */
            return 1;
        }
        zwclose(fmt_file);
    }
    fix_date_and_time();
    random_seed = (microseconds * 1000) + (epochseconds % 1000000);
    init_randoms(random_seed);
    initialize_math();
    fixup_selector(log_opened_global);
    check_texconfig_init();
    text_dir_ptr = new_dir(0);
    history = spotless;         /* ready to go! */
    /* Initialize synctex primitive */
    synctexinitcommand();
    /* tex is ready to go, now */
    unhide_lua_table(Luas, "tex", tex_table_id);
    unhide_lua_table(Luas, "pdf", pdf_table_id);
    unhide_lua_table(Luas, "newtoken", newtoken_table_id);
    unhide_lua_table(Luas, "token", token_table_id);
    unhide_lua_table(Luas, "node", node_table_id);

    lua_pushboolean(L, 1);      /* true */
    return 1;

}

/* tex random generators */
static int tex_init_rand(lua_State * L)
{
  int sp;
  if (!lua_isnumber(L, 1)) {
      luaL_error(L, "argument must be a number");
      return 0;
  }
  sp=(int)lua_tonumber(L, 1);
  init_randoms(sp);
  return 0;
}

static int tex_unif_rand(lua_State * L)
{
  int sp;
  if (!lua_isnumber(L, 1)) {
      luaL_error(L, "argument must be a number");
      return 0;
  }
  sp=(int)lua_tonumber(L, 1);
  lua_pushnumber(L, unif_rand(sp));
  return 1;
}

static int tex_norm_rand(lua_State * L)
{
    lua_pushnumber(L, norm_rand());
    return 1;
}

/* Same as lua but  with tex rng */
static int lua_math_random (lua_State *L)
{
  lua_Number rand_max = 0x7fffffff ;
  lua_Number r =  unif_rand(rand_max) ;
  r = (r>=0 ? 0+r : 0-r) ;
  r = r / rand_max;
  switch (lua_gettop(L)) {  /* check number of arguments */
    case 0: {  /* no arguments */
      lua_pushnumber(L, r);  /* Number between 0 and 1 */
      break;
    }
    case 1: {  /* only upper limit */
      lua_Number u = luaL_checknumber(L, 1);
      luaL_argcheck(L, (lua_Number)1.0 <= u, 1, "interval is empty");
      lua_pushnumber(L, floor(r*u) + (lua_Number)(1.0));  /* [1, u] */
      break;
    }
    case 2: {  /* lower and upper limits */
      lua_Number l = luaL_checknumber(L, 1);
      lua_Number u = luaL_checknumber(L, 2);
      luaL_argcheck(L, l <= u, 2, "interval is empty");
      lua_pushnumber(L, floor(r*(u-l+1)) + l);  /* [l, u] */
      break;
    }
    default: return luaL_error(L, "wrong number of arguments");
  }
  return 1;
}



/* Experimental code can either become permanent or disappear. It is
undocumented and mostly present in the experimental branch but for
practical reasons we also have the setup code in the regular binary.
The experimental_code array is indexed by i with 1<= i <= max_experimental_code,
position 0 is not used */
int experimental_code[MAX_EXPERIMENTAL_CODE_SIZE] = { 0 };


static int set_experimental_code(lua_State *L)
{
    int e, b, i ;

    if (lua_isboolean(L,1)) {
        e = 0 ;
        b = lua_toboolean(L,1) ;
    } else if (lua_isnumber(L,1) && lua_isboolean(L,2)) {
        e = (int) lua_tonumber(L, 1);
        b = lua_toboolean(L,2) ;
    } else {
        return luaL_error(L, "boolean or number and boolean expected");
    }
    if (e==0) {
        for (i=1;i<=max_experimental_code;i++) {
            experimental_code[i] = b;
        }
    } else if (0<e && e<=max_experimental_code  ) {
        experimental_code[e] = b;
    } else {
      return luaL_error(L, "first number out of range");
    }
    return 0;
}

static int tex_run_main(lua_State * L)
{
    (void) L;
    main_control();
    return 0;
}

static int tex_run_end(lua_State * L)
{
    (void) L;
    final_cleanup();            /* prepare for death */
    close_files_and_terminate();
    do_final_end();
    return 0;
}

static int tex_show_context(lua_State * L)
{
    (void) L;
    show_context();
    return 0;
}

void init_tex_table(lua_State * L)
{
    lua_createtable(L, 0, 3);
    lua_pushcfunction(L, tex_run_boot);
    lua_setfield(L, -2, "initialize");
    lua_pushcfunction(L, tex_run_main);
    lua_setfield(L, -2, "run");
    lua_pushcfunction(L, tex_run_end);
    lua_setfield(L, -2, "finish");
    lua_setglobal(L, "tex");
}




static const struct luaL_Reg texlib[] = {
    {"run", tex_run_main},      /* may be needed  */
    {"finish", tex_run_end},    /* may be needed  */
    {"write", luacwrite},
    {"print", luacprint},
    {"tprint", luactprint},
    {"error", texerror},
    {"sprint", luacsprint},
    {"set", settex},
    {"get", gettex},
    {"setdimen", setdimen},
    {"getdimen", getdimen},
    {"setskip", setskip},
    {"getskip", getskip},
    {"setattribute", setattribute},
    {"getattribute", getattribute},
    {"setcount", setcount},
    {"getcount", getcount},
    {"settoks", settoks},
    {"gettoks", gettoks},
    {"setbox", setbox},
    {"getbox", getbox},
    {"setlist", setlist},
    {"getlist", getlist},
    {"setnest", setnest},
    {"getnest", getnest},
    {"setcatcode", setcatcode},
    {"getcatcode", getcatcode},
    {"setdelcode", setdelcode},
    {"getdelcode", getdelcode},
    {"setlccode", setlccode},
    {"getlccode", getlccode},
    {"setmathcode", setmathcode},
    {"getmathcode", getmathcode},
    {"setsfcode", setsfcode},
    {"getsfcode", getsfcode},
    {"setuccode", setuccode},
    {"getuccode", getuccode},
    {"round", tex_roundnumber},
    {"scale", tex_scaletable},
    {"sp", tex_scaledimen},
    {"fontname", getfontname},
    {"fontidentifier", getfontidentifier},
    {"pdffontname", getpdffontname},
    {"pdffontobjnum", getpdffontobjnum},
    {"pdffontsize", getpdffontsize},
    {"uniformdeviate", getuniformdeviate},
    {"number", getnumber},
    {"romannumeral", getromannumeral},
    {"pdfpageref", getpdfpageref},
    {"pdfxformname", getpdfxformname},
    {"definefont", tex_definefont},
    {"hashtokens", tex_hashpairs},
    {"primitives", tex_primitives},
    {"extraprimitives", tex_extraprimitives},
    {"enableprimitives", tex_enableprimitives},
    {"shipout", tex_shipout},
    {"badness", tex_badness},
    {"setmath", tex_setmathparm},
    {"getmath", tex_getmathparm},
    {"linebreak", tex_run_linebreak},
    /* tex random generators     */
    {"init_rand",   tex_init_rand},
    {"uniform_rand",tex_unif_rand},
    {"normal_rand", tex_norm_rand},
    {"lua_math_randomseed", tex_init_rand}, /* syntactic sugar  */
    {"lua_math_random", lua_math_random},
    {"set_experimental_code",set_experimental_code},
    {"show_context", tex_show_context},
    {NULL, NULL}                /* sentinel */
};

int luaopen_tex(lua_State * L)
{
    luaL_register(L, "tex", texlib);
    /* *INDENT-OFF* */
    make_table(L, "attribute", "tex.attribute"   ,"getattribute", "setattribute");
    make_table(L, "skip",      "tex.skip"        ,"getskip",      "setskip");
    make_table(L, "dimen",     "tex.dimen"       ,"getdimen",     "setdimen");
    make_table(L, "count",     "tex.count"       ,"getcount",     "setcount");
    make_table(L, "toks",      "tex.toks"        ,"gettoks",      "settoks");
    make_table(L, "box",       "tex.box"         ,"getbox",       "setbox");
    make_table(L, "sfcode",    "tex.sfcode"      ,"getsfcode",    "setsfcode");
    make_table(L, "lccode",    "tex.lccode"      ,"getlccode",    "setlccode");
    make_table(L, "uccode",    "tex.uccode"      ,"getuccode",    "setuccode");
    make_table(L, "catcode",   "tex.catcode"     ,"getcatcode",   "setcatcode");
    make_table(L, "mathcode",   "tex.mathcode"    ,"getmathcode",  "setmathcode");
    make_table(L, "delcode",   "tex.delcode"     ,"getdelcode",   "setdelcode");
    make_table(L, "lists",     "tex.lists"       ,"getlist",      "setlist");
    make_table(L, "nest",      "tex.nest"        ,"getnest",      "setnest");
    /* *INDENT-ON* */
    init_nest_lib(L);
    /* make the meta entries */
    /* fetch it back */
    luaL_newmetatable(L, "tex.meta");
    lua_pushstring(L, "__index");
    lua_pushcfunction(L, gettex);
    lua_settable(L, -3);
    lua_pushstring(L, "__newindex");
    lua_pushcfunction(L, settex);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);    /* meta to itself */
    /* initialize the I/O stack: */
    spindles = xmalloc(sizeof(spindle));
    spindle_index = 0;
    spindles[0].head = NULL;
    spindles[0].tail = NULL;
    spindle_size = 1;
    /* a somewhat odd place for this assert, maybe */
    assert(command_names[data_cmd].command_offset == data_cmd);
    return 1;
}
