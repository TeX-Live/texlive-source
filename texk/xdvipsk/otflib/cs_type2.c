/* This is xdvipsk, an eXtended version of dvips(k) by Tomas Rokicki.

	Copyright (C) 2016 by VTeX Ltd (www.vtex.lt),
	the xdvipsk project team - Sigitas Tolusis and Arunas Povilaitis.

    Program original code copyright (C) 2007-2014 by Jin-Hwan Cho and 
	Shunsaku Hirata, the dvipdfmx project team.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

/*
 * Type 2 Charstring support:
 *  Decode and encode Type 2 charstring
 *
 * All local/global subroutine calls in a given charstring is replace by the
 * content of subroutine charstrings. We do this because some PostScript RIP
 * may have problems with sparse subroutine array. Workaround for this is to
 * re-order subroutine array so that no gap appears in the subroutine array,
 * or put dummy charstrings that contains only `return' in the gap. However,
 * re-ordering of subroutine is rather difficult for Type 2 charstrings due
 * to the bias which depends on the total number of subroutines. Replacing
 * callgsubr/callsubr calls with the content of the corresponding subroutine
 * charstring may be more efficient than putting dummy subroutines in the
 * case of subsetted font. Adobe distiller seems doing same thing.
 *
 * And also note that subroutine numbers within subroutines can depend on the
 * content of operand stack as follows:
 *
 *   ... l m callsubr << subr #(m+bias): n add callsubr >> ...
 *
 * I've not implemented the `random' operator which generates a pseudo-random
 * number in the range (0, 1] and push them into argument stack.
 * How pseudo-random sequences are generated is not documented in the Type 2
 * charstring spec..
 */

#include <config.h>

#include <stdio.h>

/* fabs, sqrt ... */
#include <math.h>

#include "mem.h"
#include "error.h"

/* data types, limits */
#include "cff_types.h"
#include "cff_limits.h"

#include "cs_type2.h"

#define CS_TYPE2_DEBUG_STR "Type2 Charstring Parser"
#define CS_TYPE2_DEBUG     5

static UT_icd double_icd = {sizeof(double), NULL, NULL, NULL };

#define STACK_SIZE     48
#define PS_STACK_SIZE  24
#define MAX_SUBR_DEPTH 10
#define SCRATCH_SIZE   32

/* Type1CharstringGenInterp status codes */
#define S_CGI_INITIAL 0
#define S_CGI_OPEN    1
#define S_CGI_CLOSED  2
#define S_CGI_SEAC    3

/* Type1CharstringGen status codes */
#define S_CHG_INITIAL 0
#define S_CHG_GEN     1

/* CharstringInterp status codes */
#define S_CCI_INITIAL  0
#define S_CCI_SEAC     1
#define S_CCI_SBW      2
#define S_CCI_HSTEM    3
#define S_CCI_VSTEM    4
#define S_CCI_HINTMASK 5
#define S_CCI_IPATH    6
#define S_CCI_PATH     7

/* CharstringInterp error codes */
#define CCI_errOK                0
#define CCI_errInternal         -1
#define CCI_errRunoff           -2
#define CCI_errUnimplemented    -3
#define CCI_errOverflow         -4
#define CCI_errUnderflow        -5
#define CCI_errVector           -6
#define CCI_errValue            -7
#define CCI_errSubr             -8
#define CCI_errGlyph            -9
#define CCI_errCurrentPoint    -10
#define CCI_errFlex            -11
#define CCI_errMultipleMaster  -12
#define CCI_errOpenStroke      -13
#define CCI_errLateSidebearing -14
#define CCI_errOthersubr       -15
#define CCI_errOrdering        -16
#define CCI_errHintmask        -17
#define CCI_errSubrDepth       -18
#define CCI_errLastError       -18


/* Global variables */
static double cs_f_precision = 5.0;
static int cs_precision = 5;
static boolean direct_hr = false;
static UT_array *ut_glyphs = NULL;
static UT_array *ut_subrs = NULL;
static double default_width_x;
static double nominal_width_x;
static int hr_firstsubr = 5;
static cff_index *gsubr_idx; 
static cff_index *subr_idx;
static card8 *t1_dst;
static card8 *t1_limit;
static long t1_dstlen;
static card8 *curr_t1_dst;
static card8 *curr_t1_limit;
static long curr_t1_dstlen;


/* Type1CharstringGenInterp variables */
static int status_cgi = S_CGI_INITIAL;
static cs_point width = {0.0, 0.0};
static boolean in_hr = false;
static double max_flex_height = 0;
static boolean bad_flex = false;
// hints and hint replacement
static UT_array *stem_pos = NULL;
static UT_array *stem_width = NULL;
static UT_array *stem_hstem = NULL;
static card8 *last_hints = NULL;


/* Type1CharstringGen variables */
static int status_chg = S_CHG_INITIAL;
static cs_point cs_true = {0.0, 0.0};
static cs_point cs_false = {0.0, 0.0};


/* CharstringInterp variables */
static int status_cci = S_CCI_INITIAL;
static int cs_error;
static int cs_error_data;
static boolean done;
static boolean careful;
static double cs_s[STACK_SIZE];
static int cs_sp;
static int subr_depth;
static double cs_trn[SCRATCH_SIZE];
static cs_point lsb = {0.0, 0.0};
static cs_point cp = {0.0, 0.0};
static cs_point seac_origin = {0.0, 0.0};
static boolean flex;
static int t2nhints;
static double double_for_error;


#ifdef WIN32
# define random() rand()
#endif

#define CHECK_STACK(numargs)    do { if (cs_sp < numargs) ERROR("Type2 CharString Parser Error: stack underflow"); } while (0)
#define CHECK_STATE()           do { if (status_cci < S_CCI_IPATH) ERROR("Type2 CharString Parser Error: ordering constraints violated"); } while (0)
#define CHECK_PATH_START()      do { status_cci = S_CCI_PATH; } while (0)
#define CHECK_PATH_END()        do { if (status_cci == S_CCI_PATH) { act_closepath(cmd); } status_cci = S_CCI_IPATH; } while (0)
#define CHECK_SCRATCH(idx)      do { if (idx >= SCRATCH_SIZE ) ERROR("Type2 CharString Parser Error: transient array underflow"); } while (0)

#define UNKDOUBLE               -9.79797e97
#define MIN_KNOWN_DOUBLE        -9.69696e97
#define KNOWN(d)                ((d) >= MIN_KNOWN_DOUBLE)


/*
 * Type 1/2 CharString encoding
 */

/*
 * 1-byte CharString operators:
 *  cs_escape is first byte of two-byte operator
 */

/*      RESERVED      0 */
#define cs_hstem      1
/*      RESERVED      2 */
#define cs_vstem      3
#define cs_vmoveto    4
#define cs_rlineto    5
#define cs_hlineto    6
#define cs_vlineto    7
#define cs_rrcurveto  8
#define cs_closepath  9  /* TYPE1 */
#define cs_callsubr   10
#define cs_return     11
#define cs_escape     12
#define cs_hsbw       13 /* TYPE1 */
#define cs_endchar    14
/*      RESERVED      15 */
#define cs_blend      16
/*      RESERVED      17 */
#define cs_hstemhm    18
#define cs_hintmask   19
#define cs_cntrmask   20
#define cs_rmoveto    21
#define cs_hmoveto    22
#define cs_vstemhm    23
#define cs_rcurveline 24
#define cs_rlinecurve 25
#define cs_vvcurveto  26
#define cs_hhcurveto  27
#define cs_shortint   28
#define cs_callgsubr  29
#define cs_vhcurveto  30
#define cs_hvcurveto  31
#define cs_escapedelta 32

/*
 * 2-byte CharString operators:
 *  "dotsection" is obsoleted in Type 2 charstring.
 */

#define cs_dotsection 32 + 0
#define cs_vstem3     32 + 1 /* TYPE1 */
#define cs_hstem3     32 + 2 /* TYPE1 */
#define cs_and        32 + 3
#define cs_or         32 + 4
#define cs_not        32 + 5
#define cs_seac       32 + 6 /* TYPE1 */
#define cs_sbw        32 + 7 /* TYPE1 */
#define cs_store      32 + 8
#define cs_abs        32 + 9
#define cs_add        32 + 10
#define cs_sub        32 + 11
#define cs_div        32 + 12
#define cs_load       32 + 13
#define cs_neg        32 + 14
#define cs_eq         32 + 15
#define cs_callothersubr 32 + 16 /* TYPE1 */
#define cs_pop        32 + 17 /* TYPE1 */
#define cs_drop       32 + 18
/*      RESERVED      19 */
#define cs_put        32 + 20
#define cs_get        32 + 21
#define cs_ifelse     32 + 22 
#define cs_random     32 + 23
#define cs_mul        32 + 24
/*      RESERVED      25 */
#define cs_sqrt       32 + 26
#define cs_dup        32 + 27
#define cs_exch       32 + 28
#define cs_index      32 + 29
#define cs_roll       32 + 30
#define cs_setcurrentpoint 32 + 31 /* TYPE1 */
/*      RESERVED      32 */
/*      RESERVED      33 */
#define cs_hflex      32 + 34
#define cs_flex       32 + 35
#define cs_hflex1     32 + 36
#define cs_flex1      32 + 37


static void act_hintmask(int cmd, card8 *data, int nhints);
static boolean do_charstring (card8 **t2_data, long length);


static void
shift_point(cs_point *p, double dx, double dy)
{
	p->x += dx;
	p->y += dy;
}

static void
calc_shifted_point(cs_point *src, cs_point *dst, double dx, double dy)
{
	dst->x = src->x + dx;
	dst->y = src->y + dy;
}

static void
append_t1chr(card8 val)
{
	if (curr_t1_limit >= curr_t1_dst + 1)
		curr_t1_dst[curr_t1_dstlen++] = val;
	else
		ERROR("Type2 CharString Parser Error: Max possible charstring length exceeded");
}

static boolean s_push(double d)
{
    if (cs_sp < STACK_SIZE)
        cs_s[cs_sp++] = d;
    else
        ERROR("Type2 CharString Parser Error: Stack overflow");
    return true;
}

static double s_pop(int n)
{
	cs_sp -= n;
	return cs_s[cs_sp];
}

static double s_top(int n)
{
	return cs_s[cs_sp - n - 1];
}

static void
gen_rational(int big_val, int divisor)
{
	int base, w;
	long l;

    int frac = big_val % divisor;
    int val = (frac == 0 ? big_val / divisor : big_val);

	if (val >= -107 && val <= 107)
        append_t1chr((card8)(val + 139));

    else if (val >= -1131 && val <= 1131) {
        base = val < 0 ? 251 : 247;
        if (val < 0) val = -val;
        val -= 108;
        w = val % 256;
        val = (val - w) / 256;
        append_t1chr((card8)(val + base));
        append_t1chr((card8)w);

    } else {
        append_t1chr('\377');
        l = val;
        append_t1chr((card8)((l >> 24) & 0xFF));
        append_t1chr((card8)((l >> 16) & 0xFF));
        append_t1chr((card8)((l >> 8) & 0xFF));
        append_t1chr((card8)((l >> 0) & 0xFF));
    }

    if (frac != 0) {
        append_t1chr((card8)(divisor + 139));
        append_t1chr((card8)cs_escape);
        append_t1chr((card8)(cs_div - cs_escapedelta));
    }
}

static void
gen_number(double float_val, int kind)
{
	int big_val;

    switch (kind) {
      case 'x':
        cs_true.x += float_val;
        float_val = cs_true.x - cs_false.x;
        break;
      case 'y':
        cs_true.y += float_val;
        float_val = cs_true.y - cs_false.y;
        break;
      case 'X':
        cs_true.x = float_val;
        break;
      case 'Y':
        cs_true.y = float_val;
        break;
    }

    // 30.Jul.2003 - Avoid rounding differences between platforms with the
    // extra 0.00001.
    big_val = (int)floor(float_val * cs_f_precision + 0.50001);

    gen_rational(big_val, cs_precision);

    float_val = big_val / cs_f_precision;
    switch (kind) {
      case 'x':
        cs_false.x += float_val;
        break;
      case 'y':
        cs_false.y += float_val;
        break;
      case 'X':
        cs_false.x = float_val;
        break;
      case 'Y':
        cs_false.y = float_val;
        break;
    }
}


static void
gen_command(int command)
{
    if (command >= cs_escapedelta) {
        append_t1chr((card8)cs_escape);
        append_t1chr((card8)(command - cs_escapedelta));
        if (command != cs_sbw)
            status_chg = S_CHG_GEN;
    } else {
        append_t1chr((card8)command);
        if (command > cs_vmoveto && command != cs_hsbw)
            status_chg = S_CHG_GEN;
    }
}

static void
gen_moveto(cs_point *p, boolean closepath, boolean always)
{
	cs_point d;
	int big_dx, big_dy;

    // make sure we generate some moveto on the first command

	d.x = p->x - cs_true.x;
	d.y = p->y - cs_true.y;
    big_dx = (int)floor(d.x * cs_f_precision + 0.50001);
    big_dy = (int)floor(d.y * cs_f_precision + 0.50001);

    if (big_dx == 0 && big_dy == 0 && status_chg != S_CHG_INITIAL && !always)
        /* do nothing */;
    else {
        if (closepath)
            gen_command(cs_closepath);
        if (big_dy == 0) {
            gen_number(d.x, 'x');
            gen_command(cs_hmoveto);
        } else if (big_dx == 0) {
            gen_number(d.y, 'y');
            gen_command(cs_vmoveto);
        } else {
            gen_number(d.x, 'x');
            gen_number(d.y, 'y');
            gen_command(cs_rmoveto);
        }
    }

    cs_true = *p;
}


/*
 * Subroutines:
 *  The bias for subroutine number is introduced in type 2 charstrings.
 *
 * subr:     set to a pointer to the subroutine charstring.
 * len:      set to the length of subroutine charstring.
 * subr_idx: CFF INDEX data that contains subroutines.
 * id:       biased subroutine number.
 */
static void
get_subr (card8 **subr, long *len, cff_index *subr_idx, long id)
{
  card16 count;

  if (subr_idx == NULL)
    ERROR("%s: Subroutine called but no subroutine found.", CS_TYPE2_DEBUG_STR);

  count = subr_idx->count;

  /* Adding bias number */
  if (count < 1240) {
    id += 107;
  } else if (count < 33900) {
    id += 1131;
  } else {
    id += 32768;
  }

  if (id > count)
    ERROR("%s: Invalid Subr index: %ld (max=%u)", CS_TYPE2_DEBUG_STR, id, count);

  *len = (subr_idx->offset)[id + 1] - (subr_idx->offset)[id];
  *subr = subr_idx->data + (subr_idx->offset)[id] - 1;

  return;
}

static void
act_width(int cmd, const cs_point *p)
{
	width.x = p->x;
	width.y = p->y;
}

static void
act_default_width(int cmd)
{
	cs_point p;
    double d = default_width_x;
	if (KNOWN(d)) {
		p.x = d;
		p.y = 0;
        act_width(cmd, &p);
	}
}

static void
act_nominal_width_delta(int cmd, double delta)
{
	cs_point p;
    double d = nominal_width_x;
	if (KNOWN(d)) {
		p.x = d + delta;
		p.y = 0;
        act_width(cmd, &p);
	}
}

static void
gen_sbw(boolean hints_follow)
{
    if (!hints_follow && utarray_len(stem_hstem))
        act_hintmask(cs_hintmask, 0, utarray_len(stem_hstem));
    else if (lsb.y == 0 && width.y == 0) {
        gen_number(lsb.x, 'X');
        gen_number(width.x, 0);
        gen_command(cs_hsbw);
    } else {
        gen_number(lsb.x, 'X');
        gen_number(lsb.y, 'Y');
        gen_number(width.x, 0);
        gen_number(width.y, 0);
        gen_command(cs_sbw);
    }
    status_cgi = S_CGI_CLOSED;
}


static long
gen_hints(const card8 *data, int nhints, card8 **dst)
{
	card8 *stem_dst, *tmp_dst;
	card8 *stem_limit, *tmp_limit;
	long stem_dstlen, tmp_dstlen;
    unsigned char mask = 0x80;
	double offset, *d;
	int i, *p;

	tmp_dst = curr_t1_dst;
	tmp_limit = curr_t1_limit;
	tmp_dstlen = curr_t1_dstlen;

	stem_dst = curr_t1_dst = NEW(CS_STR_LEN_MAX, card8);
	stem_limit = curr_t1_limit = stem_dst + CS_STR_LEN_MAX;
	stem_dstlen = curr_t1_dstlen = 0;

    for (i = 0; i < nhints; i++) {
        if (*data & mask) {
			p = (int *)utarray_eltptr(stem_hstem,i);
            offset = (*p ? lsb.y : lsb.x);
			d = (double *)utarray_eltptr(stem_pos,i);
            gen_number(*d - offset, 0);
			d = (double *)utarray_eltptr(stem_width,i);
            gen_number(*d, 0);
            gen_command(*p ? cs_hstem : cs_vstem);
        }
        if ((mask >>= 1) == 0)
            data++, mask = 0x80;
    }

	stem_dstlen = curr_t1_dstlen;
	*dst = NEW(stem_dstlen,card8);
	memcpy(*dst,stem_dst,stem_dstlen);
	free(stem_dst);

	curr_t1_dst = tmp_dst;
	curr_t1_limit = tmp_limit;
	curr_t1_dstlen = tmp_dstlen;

    return stem_dstlen;
}

static void
act_hintmask(int cmd, card8 *data, int nhints)
{
    card8 *data_holder, *hints;
	long hints_len, j;
	int subrno, nsubrs, i;
	cs_type1subr *p, sb;

    if (cmd == cs_cntrmask || nhints > utarray_len(stem_hstem))
        return;

    if (!data) {
        data_holder = NEW((((nhints - 1) >> 3) + 1),card8);
		memset(data_holder,'\377',(((nhints - 1) >> 3) + 1));
        data = data_holder;
    }
	else
		data_holder = NULL;

    hints_len = gen_hints(data, nhints, &hints);
    in_hr = false;

    if (status_cgi == S_CGI_INITIAL || direct_hr) {
        if (status_cgi == S_CGI_INITIAL)
            gen_sbw(true);
		for ( j=0; j<hints_len; j++ )
			append_t1chr(hints[j]);
		free(hints);
    } else if (ut_subrs) {
		hints_len++;
		hints = RENEW(hints,hints_len,card8);
        hints[hints_len - 1] = (card8)(cs_return);

        subrno = -1, nsubrs = utarray_len(ut_subrs);
		for (i = hr_firstsubr; i < nsubrs; i++) {
			p = (cs_type1subr *)utarray_eltptr(ut_subrs,i);
			if ( !p ) break;
			if ( p->size == hints_len ) {
				if ( memcmp(p->data,hints,hints_len) == 0 ) {
                    subrno = i;
                    break;
				}
			}
		}

		if (subrno < 0) {
			sb.size = hints_len;
			sb.data = NEW(hints_len,card8);
			memcpy(sb.data,hints,hints_len);
			utarray_push_back(ut_subrs,&sb);
            subrno = nsubrs;
		}
		free(hints);

        if (subrno >= 0) {
            gen_number(subrno, 0);
            gen_number(4, 0);
            gen_command(cs_callsubr);
        }
    }
	if ( data_holder )
		free(data_holder);
}
static void
act_seac(int cmd, double asb, double adx, double ady, int bchar, int achar)
{
    if (status_cgi == S_CGI_INITIAL)
        gen_sbw(false);
    gen_number(asb, 0);
    gen_number(adx, 0);
    gen_number(ady, 0);
    gen_number(bchar, 0);
    gen_number(achar, 0);
    gen_command(cs_seac);
    status_cgi = S_CGI_SEAC;
}

static void
swap_stem_hints()
{
    utarray_clear(stem_pos);
    utarray_clear(stem_width);
    utarray_clear(stem_hstem);
    in_hr = true;
}

static void
act_hstem(int cmd, double pos, double width)
{
	int kind = 1;
    if (status_cgi != S_CGI_INITIAL && !in_hr)
        swap_stem_hints();
	utarray_push_back(stem_pos,&pos);
	utarray_push_back(stem_width,&width);
	utarray_push_back(stem_hstem,&kind);
}

static void
act_vstem(int cmd, double pos, double width)
{
	int kind = 0;
    if (status_cgi != S_CGI_INITIAL && !in_hr)
        swap_stem_hints();
	utarray_push_back(stem_pos,&pos);
	utarray_push_back(stem_width,&width);
	utarray_push_back(stem_hstem,&kind);
}

static int
type2_handle_width(int cmd, boolean have_width)
{
	cp.x = lsb.x = seac_origin.x;
	cp.y = lsb.y = seac_origin.y;
    if (status_cci != S_CCI_INITIAL)
        /* ignore width */;
    else if (have_width)
        act_nominal_width_delta(cmd, cs_s[0]);
    else
        act_default_width(cmd);
    return (have_width ? 1 : 0);
}

static void
act_line(int cmd, cs_point *a, cs_point *b)
{
    if (status_cgi == S_CGI_INITIAL)
        gen_sbw(false);
    else if (in_hr)
        act_hintmask(cmd, 0, utarray_len(stem_hstem));
    gen_moveto(a, status_cgi == S_CGI_OPEN, false);
    status_cgi = S_CGI_OPEN;
    if (a->x == b->x) {
        gen_number(b->y - a->y, 'y');
        gen_command(cs_vlineto);
    } else if (a->y == b->y) {
        gen_number(b->x - a->x, 'x');
        gen_command(cs_hlineto);
    } else {
        gen_number(b->x - a->x, 'x');
        gen_number(b->y - a->y, 'y');
        gen_command(cs_rlineto);
    }
}

static void
act_curve(int cmd, cs_point *a, cs_point *b, cs_point *c, cs_point *d)
{
    if (status_cgi == S_CGI_INITIAL)
        gen_sbw(false);
    else if (in_hr)
        act_hintmask(cmd, 0,utarray_len(stem_hstem));
    gen_moveto(a, status_cgi == S_CGI_OPEN, false);
    status_cgi = S_CGI_OPEN;
    if (b->y == a->y && d->x == c->x) {
        gen_number(b->x - a->x, 'x');
        gen_number(c->x - b->x, 'x');
        gen_number(c->y - b->y, 'y');
        gen_number(d->y - c->y, 'y');
        gen_command(cs_hvcurveto);
    } else if (b->x == a->x && d->y == c->y) {
        gen_number(b->y - a->y, 'y');
        gen_number(c->x - a->x, 'x');
        gen_number(c->y - b->y, 'y');
        gen_number(d->x - c->x, 'x');
        gen_command(cs_vhcurveto);
    } else {
        gen_number(b->x - a->x, 'x');
        gen_number(b->y - a->y, 'y');
        gen_number(c->x - b->x, 'x');
        gen_number(c->y - b->y, 'y');
        gen_number(d->x - c->x, 'x');
        gen_number(d->y - c->y, 'y');
        gen_command(cs_rrcurveto);
    }
}

static void
act_flex(int cmd, cs_point *p0, cs_point *p1, cs_point *p2, cs_point *p3_4, cs_point *p5,cs_point *p6, cs_point *p7, double flex_depth)
{
	boolean v_ok, h_ok;
	double distance;
	int sign;
	cs_point p_reference;
	double flex_height;

    if (status_cgi == S_CGI_INITIAL)
        gen_sbw(false);
    else if (in_hr)
        act_hintmask(cmd, 0, utarray_len(stem_hstem));
    gen_moveto(p0, status_cgi == S_CGI_OPEN, false);
    status_cgi = S_CGI_OPEN;

    // 1. Outer endpoints must have same x (or y) coordinate
    v_ok = (p0->x == p7->x);
    h_ok = (p0->y == p7->y);

    // 2. Join point and its neighboring controls must be at an extreme
    if (v_ok && p2->x == p3_4->x && p3_4->x == p5->x) {
        distance = fabs(p3_4->x - p0->x);
        sign = (p3_4->x < p0->x ? -1 : 1);
        if (sign * (p1->x - p0->x) < 0 || sign * (p1->x - p0->x) > distance
            || sign * (p6->x - p0->x) < 0 || sign * (p6->x - p0->x) > distance)
            v_ok = false;
    } else
        v_ok = false;

    if (h_ok && p2->y == p3_4->y && p3_4->y == p5->y) {
        distance = fabs(p3_4->y - p0->y);
        sign = (p3_4->y < p0->y ? -1 : 1);
        if (sign * (p1->y - p0->y) < 0 || sign * (p1->y - p0->y) > distance
            || sign * (p6->y - p0->y) < 0 || sign * (p6->y - p0->y) > distance)
            h_ok = false;
    } else
        h_ok = false;

    // 3. Flex height <= 20
    if (v_ok && fabs(p3_4->x - p0->x) > 20)
        v_ok = false;
    if (h_ok && fabs(p3_4->y - p0->y) > 20)
        h_ok = false;

    // generate flex commands
    if (v_ok || h_ok) {
		if ( h_ok ) {
			p_reference.x = p3_4->x;
			p_reference.y = p0->y;
		}
		else {
			p_reference.x = p0->x;
			p_reference.y = p3_4->y;
		}

        gen_number(1, 0);
        gen_command(cs_callsubr);

        gen_moveto(&p_reference, false, true);
        gen_number(2, 0);
        gen_command(cs_callsubr);

        gen_moveto(p1, false, true);
        gen_number(2, 0);
        gen_command(cs_callsubr);

        gen_moveto(p2, false, true);
        gen_number(2, 0);
        gen_command(cs_callsubr);

        gen_moveto(p3_4, false, true);
        gen_number(2, 0);
        gen_command(cs_callsubr);

        gen_moveto(p5, false, true);
        gen_number(2, 0);
        gen_command(cs_callsubr);

        gen_moveto(p6, false, true);
        gen_number(2, 0);
        gen_command(cs_callsubr);

        gen_moveto(p7, false, true);
        gen_number(2, 0);
        gen_command(cs_callsubr);

        gen_number(flex_depth, 0);
        gen_number(p7->x, 'X');
        gen_number(p7->y, 'Y');
        gen_number(0, 0);
        gen_command(cs_callsubr);

        flex_height = fabs(h_ok ? p3_4->y - p0->y : p3_4->x - p0->x);
        if (flex_height > max_flex_height)
            max_flex_height = flex_height;
    } else {
        bad_flex = true;
        act_curve(cmd, p0, p1, p2, p3_4);
        act_curve(cmd, p3_4, p5, p6, p7);
    }
}

static void
act_closepath(int cmd)
{
    if (in_hr)
        act_hintmask(cmd, 0, utarray_len(stem_hstem));
    gen_command(cs_closepath);
    status_cgi = S_CGI_CLOSED;
}

static void
actp_rmoveto(int cmd, double dx, double dy)
{
	shift_point(&cp, dx, dy);
}

static void
actp_rlineto(int cmd, double dx, double dy)
{
    cs_point p0 = cp;
	shift_point(&cp, dx, dy);
    act_line(cmd, &p0, &cp);
}

static void
actp_rrcurveto(int cmd, double dx1, double dy1, double dx2, double dy2, double dx3, double dy3)
{
	cs_point p1, p2;
    cs_point p0 = cp;
	p1.x = p0.x + dx1;
	p1.y = p0.y + dy1;
	p2.x = p1.x + dx2;
	p2.y = p1.y + dy2;
	calc_shifted_point(&p2, &cp, dx3, dy3);
    act_curve(cmd, &p0, &p1, &p2, &cp);
}

static void
actp_rrflex(int cmd, double dx1, double dy1, double dx2, double dy2, double dx3, double dy3, double dx4, double dy4, double dx5, double dy5, double dx6, double dy6, double flex_depth)
{
	cs_point p1, p2, p3_4, p5, p6;
    cs_point p0 = cp;
	p1.x = p0.x + dx1;
	p1.y = p0.y + dy1;
	p2.x = p1.x + dx2;
	p2.y = p1.y + dy2;
	p3_4.x = p2.x + dx3;
	p3_4.y = p2.y + dy3;
	p5.x = p3_4.x + dx4;
	p5.y = p3_4.y + dy4;
	p6.x = p5.x + dx5;
	p6.y = p5.y + dy5;
	calc_shifted_point(&p6, &cp, dx6, dy6);
    act_flex(cmd, &p0, &p1, &p2, &p3_4, &p5, &p6, &cp, flex_depth);
}

static boolean
vector_command(int cmd)
{
    int vectoroff, offset, num, i;
    double v;

    switch (cmd) {

      case cs_put:
        CHECK_STACK(2);
        offset = (int)cs_s[cs_sp - 1];
		CHECK_SCRATCH(offset);
		cs_trn[offset] = cs_s[cs_sp - 2];
        cs_sp -= 2;
        break;

      case cs_get:
        CHECK_STACK(1);
        offset = (int)cs_s[cs_sp - 1];
		CHECK_SCRATCH(offset);
        cs_s[cs_sp - 1] = cs_trn[offset];
        break;

      default:
	    ERROR("Type2 CharString Parser Error: unimplemented command found");

    }

    return true;
}

static boolean
roll_command()
{
	int amount, n, i, base;
    double copy_stack[STACK_SIZE];

    CHECK_STACK(2);
    amount = (int)s_pop(1);
    n = (int)s_pop(1);
    if (n <= 0)
	    ERROR("Type2 CharString Parser Error: stack underflow");
    CHECK_STACK(n);

    base = cs_sp - n;
    while (amount < 0)
        amount += n;

    for (i = 0; i < n; i++)
        copy_stack[i] = cs_s[ base + (i+amount) % n ];
    for (i = 0; i < n; i++)
        cs_s[base + i] = copy_stack[i];

    return true;
}

static boolean
arith_command(int cmd)
{
    int i;
    double d;

    switch (cmd) {

      case cs_abs:
        CHECK_STACK(1);
        if (s_top(0) < 0)
            cs_s[cs_sp - 1] = -s_top(0);
        break;

      case cs_add:
        CHECK_STACK(1);
        d = s_pop(1);
        cs_s[cs_sp - 1] += d;
        break;

      case cs_sub:
        CHECK_STACK(1);
        d = s_pop(1);
        cs_s[cs_sp - 1] -= d;
        break;

      case cs_div:
        CHECK_STACK(2);
        d = s_pop(1);
        cs_s[cs_sp - 1] /= d;
        break;

      case cs_neg:
        CHECK_STACK(1);
        cs_s[cs_sp - 1] = -s_top(0);
        break;

      case cs_random: {
          double d;
          do {
              d = random() / ((double)RAND_MAX);
          } while (d == 0);
          s_push(d);
          break;
      }

      case cs_mul:
        CHECK_STACK(2);
        d = s_pop(1);
        cs_s[cs_sp - 1] *= d;
        break;

      case cs_sqrt:
        CHECK_STACK(1);
        if (s_top(0) < 0)
		    ERROR("Type2 CharString Parser Error: bad value");
        cs_s[cs_sp - 1] = sqrt(s_top(0));
        break;

      case cs_drop:
        CHECK_STACK(1);
        s_pop(1);
        break;

      case cs_exch:
        CHECK_STACK(2);
        d = s_top(0);
        cs_s[cs_sp - 1] = s_top(1);
        cs_s[cs_sp - 2] = d;
        break;

      case cs_index:
        CHECK_STACK(1);
        i = (int)s_top(0);
        if (i < 0)
		    ERROR("Type2 CharString Parser Error: bad value");
        CHECK_STACK(i + 2);
        cs_s[cs_sp - 1] = s_top(i+1);
        break;

      case cs_roll:
        return roll_command();

      case cs_dup:
        CHECK_STACK(1);
        s_push(s_top(0));
        break;

      case cs_and:
        CHECK_STACK(2);
        d = s_pop(1);
        cs_s[cs_sp - 1] = (s_top(0) != 0) && (d != 0);
        break;

      case cs_or:
        CHECK_STACK(2);
        d = s_pop(1);
        cs_s[cs_sp - 1] = (s_top(0) != 0) || (d != 0);
        break;

      case cs_not:
        CHECK_STACK(1);
        cs_s[cs_sp - 1] = (s_top(0) == 0);
        break;

      case cs_eq:
        CHECK_STACK(2);
        d = s_pop(1);
        cs_s[cs_sp - 1] = (s_top(0) == d);
        break;

      case cs_ifelse:
        CHECK_STACK(4);
        if (s_top(1) > s_top(0))
            cs_s[cs_sp - 4] = cs_s[cs_sp - 3];
        s_pop(3);
        break;

      default:
	    ERROR("Type2 CharString Parser Error: unimplemented command found");

    }

    return true;
}

static boolean
callsubr_command()
{
	int which;
	card8 *subr_cs;
	long subr_len;
	boolean tmp_done, loc_done;

    CHECK_STACK(1);
    which = (int)s_pop(1);

	subr_cs = NULL;
    get_subr(&subr_cs, &subr_len, subr_idx, which);
    if (!subr_cs)
	    ERROR("Type2 CharString Parser Error: bad subroutine number %d",which);

    if (subr_depth >= MAX_SUBR_DEPTH)
	    ERROR("Type2 CharString Parser Error: subrs nested too deep at '%d'",which);
    subr_depth++;

	tmp_done = done;
	done = false;
    do_charstring(&subr_cs,subr_len);
	loc_done = done;
	done = tmp_done;

    subr_depth--;
    if (cs_error != CCI_errOK)
        return false;
    return !loc_done;
}

static boolean
callgsubr_command()
{
	int which;
	card8 *subr_cs;
	long subr_len;
	boolean tmp_done, loc_done;

    CHECK_STACK(1);
    which = (int)s_pop(1);

	subr_cs = NULL;
    get_subr(&subr_cs, &subr_len, gsubr_idx, which);
    if (!subr_cs)
	    ERROR("Type2 CharString Parser Error: bad subroutine number %d",which);

    if (subr_depth >= MAX_SUBR_DEPTH)
	    ERROR("Type2 CharString Parser Error: subrs nested too deep at '%d'",which);
    subr_depth++;

	tmp_done = done;
	done = false;
    do_charstring(&subr_cs,subr_len);
	loc_done = done;
	done = tmp_done;

    subr_depth--;
    if (cs_error != CCI_errOK)
        return false;
    return !loc_done;
}

static boolean
type2_command(int cmd, card8 *data, int *left)
{
	double pos, dx, dy;
    int bottom = 0;

    switch (cmd) {

      case cs_hstem:
      case cs_hstemhm:
        CHECK_STACK(2);
        if (status_cci <= S_CCI_SEAC)
            bottom = type2_handle_width(cmd, (cs_sp % 2) == 1);
        if (status_cci > S_CCI_HSTEM)
		    ERROR("Type2 CharString Parser Error: ordering constraints violated");
        status_cci = S_CCI_HSTEM;
        for (pos = 0; bottom + 1 < cs_sp; bottom += 2) {
            t2nhints++;
            act_hstem(cmd, pos + cs_s[bottom], cs_s[bottom + 1]);
            pos += cs_s[bottom] + cs_s[bottom + 1];
        }
        break;

      case cs_vstem:
      case cs_vstemhm:
        CHECK_STACK(2);
        if (status_cci <= S_CCI_SEAC)
            bottom = type2_handle_width(cmd, (cs_sp % 2) == 1);
        if (status_cci > S_CCI_VSTEM)
		    ERROR("Type2 CharString Parser Error: ordering constraints violated");
        status_cci = S_CCI_VSTEM;
        for (pos = 0; bottom + 1 < cs_sp; bottom += 2) {
            t2nhints++;
            act_vstem(cmd, pos + cs_s[bottom], cs_s[bottom + 1]);
            pos += cs_s[bottom] + cs_s[bottom + 1];
        }
        break;

      case cs_hintmask:
      case cs_cntrmask:
        if (status_cci <= S_CCI_SEAC && cs_sp >= 1) {
            bottom = type2_handle_width(cmd, (cs_sp % 2) == 1);
            for (pos = 0; bottom + 1 < cs_sp; bottom += 2) {
                t2nhints++;
                act_hstem(cmd, pos + cs_s[bottom], cs_s[bottom + 1]);
                pos += cs_s[bottom] + cs_s[bottom + 1];
            }
        }
        if ((status_cci == S_CCI_HSTEM || status_cci == S_CCI_VSTEM) && cs_sp >= 2)
            for (pos = 0; bottom + 1 < cs_sp; bottom += 2) {
                t2nhints++;
                act_vstem(cmd, pos + cs_s[bottom], cs_s[bottom + 1]);
                pos += cs_s[bottom] + cs_s[bottom + 1];
            }
        if (status_cci < S_CCI_HINTMASK)
            status_cci = S_CCI_HINTMASK;
        if (t2nhints == 0)
		    ERROR("Type2 CharString Parser Error: inappropriate hintmask");
        if (!data || !left)
		    ERROR("Type2 CharString Parser Error: internal error");
        if (((t2nhints - 1) >> 3) + 1 > *left)
		    ERROR("Type2 CharString Parser Error: commands past end");
        act_hintmask(cmd, data, t2nhints);
        *left -= ((t2nhints - 1) >> 3) + 1;
        break;

      case cs_rmoveto:
        CHECK_STACK(2);
        if (status_cci <= S_CCI_SEAC)
            bottom = type2_handle_width(cmd, cs_sp > 2);
        CHECK_PATH_END();
        actp_rmoveto(cmd, cs_s[bottom], cs_s[bottom + 1]);
        break;

      case cs_hmoveto:
        CHECK_STACK(1);
        if (status_cci <= S_CCI_SEAC)
            bottom = type2_handle_width(cmd, cs_sp > 1);
        CHECK_PATH_END();
        actp_rmoveto(cmd, cs_s[bottom], 0);
        break;

      case cs_vmoveto:
        CHECK_STACK(1);
        if (status_cci <= S_CCI_SEAC)
            bottom = type2_handle_width(cmd, cs_sp > 1);
        CHECK_PATH_END();
        actp_rmoveto(cmd, 0, cs_s[bottom]);
        break;

      case cs_rlineto:
        CHECK_STACK(2);
        CHECK_STATE();
        CHECK_PATH_START();
        for (; bottom + 1 < cs_sp; bottom += 2)
            actp_rlineto(cmd, cs_s[bottom], cs_s[bottom + 1]);
        break;

      case cs_hlineto:
        CHECK_STACK(1);
        CHECK_STATE();
        CHECK_PATH_START();
        while (bottom < cs_sp) {
            actp_rlineto(cmd, cs_s[bottom++], 0);
            if (bottom < cs_sp)
                actp_rlineto(cmd, 0, cs_s[bottom++]);
        }
        break;

      case cs_vlineto:
        CHECK_STACK(1);
        CHECK_STATE();
        CHECK_PATH_START();
        while (bottom < cs_sp) {
            actp_rlineto(cmd, 0, cs_s[bottom++]);
            if (bottom < cs_sp)
                actp_rlineto(cmd, cs_s[bottom++], 0);
        }
        break;

      case cs_rrcurveto:
        CHECK_STACK(6);
        CHECK_STATE();
        CHECK_PATH_START();
        for (; bottom + 5 < cs_sp; bottom += 6)
            actp_rrcurveto(cmd, cs_s[bottom], cs_s[bottom + 1], cs_s[bottom + 2], cs_s[bottom + 3], cs_s[bottom + 4], cs_s[bottom + 5]);
        break;

      case cs_hhcurveto:
        CHECK_STACK(4);
        CHECK_STATE();
        CHECK_PATH_START();
        if (cs_sp % 2 == 1) {
            actp_rrcurveto(cmd, cs_s[bottom + 1], cs_s[bottom], cs_s[bottom + 2], cs_s[bottom + 3], cs_s[bottom + 4], 0);
            bottom += 5;
        }
        for (; bottom + 3 < cs_sp; bottom += 4)
            actp_rrcurveto(cmd, cs_s[bottom], 0, cs_s[bottom + 1], cs_s[bottom + 2], cs_s[bottom + 3], 0);
        break;

      case cs_hvcurveto:
        CHECK_STACK(4);
        CHECK_STATE();
        CHECK_PATH_START();
        while (bottom + 3 < cs_sp) {
            double dx3 = (bottom + 5 == cs_sp ? cs_s[bottom + 4] : 0);
            actp_rrcurveto(cmd, cs_s[bottom], 0, cs_s[bottom + 1], cs_s[bottom + 2], dx3, cs_s[bottom + 3]);
            bottom += 4;
            if (bottom + 3 < cs_sp) {
                double dy3 = (bottom + 5 == cs_sp ? cs_s[bottom + 4] : 0);
                actp_rrcurveto(cmd, 0, cs_s[bottom], cs_s[bottom + 1], cs_s[bottom + 2], cs_s[bottom + 3], dy3);
                bottom += 4;
            }
        }
        break;

      case cs_rcurveline:
        CHECK_STACK(8);
        CHECK_STATE();
        CHECK_PATH_START();
        for (; bottom + 7 < cs_sp; bottom += 6)
            actp_rrcurveto(cmd, cs_s[bottom], cs_s[bottom + 1], cs_s[bottom + 2], cs_s[bottom + 3], cs_s[bottom + 4], cs_s[bottom + 5]);
        actp_rlineto(cmd, cs_s[bottom], cs_s[bottom + 1]);
        break;

      case cs_rlinecurve:
        CHECK_STACK(8);
        CHECK_STATE();
        CHECK_PATH_START();
        for (; bottom + 7 < cs_sp; bottom += 2)
            actp_rlineto(cmd, cs_s[bottom], cs_s[bottom + 1]);
        actp_rrcurveto(cmd, cs_s[bottom], cs_s[bottom + 1], cs_s[bottom + 2], cs_s[bottom + 3], cs_s[bottom + 4], cs_s[bottom + 5]);
        break;

      case cs_vhcurveto:
        CHECK_STACK(4);
        CHECK_STATE();
        CHECK_PATH_START();
        while (bottom + 3 < cs_sp) {
            double dy3 = (bottom + 5 == cs_sp ? cs_s[bottom + 4] : 0);
            actp_rrcurveto(cmd, 0, cs_s[bottom], cs_s[bottom + 1], cs_s[bottom + 2], cs_s[bottom + 3], dy3);
            bottom += 4;
            if (bottom + 3 < cs_sp) {
                double dx3 = (bottom + 5 == cs_sp ? cs_s[bottom + 4] : 0);
                actp_rrcurveto(cmd, cs_s[bottom], 0, cs_s[bottom + 1], cs_s[bottom + 2], dx3, cs_s[bottom + 3]);
                bottom += 4;
            }
        }
        break;

      case cs_vvcurveto:
        CHECK_STACK(4);
        CHECK_STATE();
        CHECK_PATH_START();
        if (cs_sp % 2 == 1) {
            actp_rrcurveto(cmd, cs_s[bottom], cs_s[bottom + 1], cs_s[bottom + 2], cs_s[bottom + 3], 0, cs_s[bottom + 4]);
            bottom += 5;
        }
        for (; bottom + 3 < cs_sp; bottom += 4)
            actp_rrcurveto(cmd, 0, cs_s[bottom], cs_s[bottom + 1], cs_s[bottom + 2], 0, cs_s[bottom + 3]);
        break;

      case cs_flex:
        CHECK_STACK(13);
        CHECK_STATE();
        CHECK_PATH_START();
        assert(bottom == 0);
        actp_rrflex(cmd,
                    cs_s[0], cs_s[1], cs_s[2], cs_s[3], cs_s[4], cs_s[5],
					cs_s[6], cs_s[7], cs_s[8], cs_s[9], cs_s[10], cs_s[11],
                    cs_s[12]);
        break;

      case cs_hflex:
        CHECK_STACK(7);
        CHECK_STATE();
        CHECK_PATH_START();
        assert(bottom == 0);
        actp_rrflex(cmd,
                    cs_s[0], 0, cs_s[1], cs_s[2], cs_s[3], 0,
                    cs_s[4], 0, cs_s[5], -cs_s[2], cs_s[6], 0,
                    50);
        break;

      case cs_hflex1:
        CHECK_STACK(9);
        CHECK_STATE();
        CHECK_PATH_START();
        assert(bottom == 0);
        actp_rrflex(cmd,
                    cs_s[0], cs_s[1], cs_s[2], cs_s[3], cs_s[4], 0,
                    cs_s[5], 0, cs_s[6], cs_s[7], cs_s[8], -(cs_s[1] + cs_s[3] + cs_s[7]),
                    50);
        break;

      case cs_flex1: {
          CHECK_STACK(11);
          CHECK_STATE();
          CHECK_PATH_START();
          assert(bottom == 0);
          dx = cs_s[0] + cs_s[2] + cs_s[4] + cs_s[6] + cs_s[8];
          dy = cs_s[1] + cs_s[3] + cs_s[5] + cs_s[7] + cs_s[9];
          if (fabs(dx) > fabs(dy))
              actp_rrflex(cmd,
                          cs_s[0], cs_s[1], cs_s[2], cs_s[3], cs_s[4], cs_s[5],
                          cs_s[6], cs_s[7], cs_s[8], cs_s[9], cs_s[10], -dy,
                          50);
          else
              actp_rrflex(cmd,
                          cs_s[0], cs_s[1], cs_s[2], cs_s[3], cs_s[4], cs_s[5],
                          cs_s[6], cs_s[7], cs_s[8], cs_s[9], -dx, cs_s[10],
                          50);
          break;
      }

      case cs_endchar:
        if (status_cci <= S_CCI_SEAC)
            bottom = type2_handle_width(cmd, cs_sp > 0 && cs_sp != 4);
        if (bottom + 3 < cs_sp && status_cci == S_CCI_INITIAL)
            act_seac(cmd, 0, cs_s[bottom], cs_s[bottom + 1], (int)cs_s[bottom + 2], (int)cs_s[bottom + 3]);
        CHECK_PATH_END();
        done = true;
        cs_sp = 0;
        return false;

      case cs_return:
        return false;

      case cs_callsubr:
        return callsubr_command();

      case cs_callgsubr:
        return callgsubr_command();

      case cs_put:
      case cs_get:
        return vector_command(cmd);

      case cs_abs:
      case cs_add:
      case cs_sub:
      case cs_div:
      case cs_neg:
      case cs_random:
      case cs_mul:
      case cs_sqrt:
      case cs_drop:
      case cs_exch:
      case cs_index:
      case cs_roll:
      case cs_dup:
      case cs_and:
      case cs_or:
      case cs_not:
      case cs_eq:
      case cs_ifelse:
        return arith_command(cmd);

      case cs_dotsection:
        break;

      default:
	    ERROR("Type2 CharString Parser Error: unimplemented command found");

    }

    cs_sp = 0;
    return cs_error >= 0;
}

static boolean
do_charstring (card8 **t2_data, long length)
{
    card8 *data = *t2_data;
    int left = length;

    while (left > 0) {
        boolean more;
        int ahead, val;
		int16_t val16;
		int32_t val32;
		int left_ptr;

        if (*data >= 32 && *data <= 246) {              // push small number
            more = s_push(data[0] - 139);
            ahead = 1;

        } else if (*data < 32) {                        // a command
            if (*data == cs_escape) {
                if (left < 2)
                    goto runoff_error;
                more = type2_command(cs_escapedelta + data[1], 0, 0);
                ahead = 2;
				if (more && (left - ahead <= 0))
					more = 0;
            } else if (*data == cs_shortint) { // short integer
                if (left < 3)
                    goto runoff_error;
                val16 = (data[1] << 8) | data[2];
                more = s_push(val16);
                ahead = 3;
            } else if (*data == cs_hintmask || *data == cs_cntrmask) {
                left_ptr = left - 1;
                more = type2_command(data[0], data + 1, &left_ptr);
                ahead = 1 + (left - 1) - left_ptr;
				if (more && (left - ahead <= 0))
					more = 0;
			}
			else {
                more = type2_command(data[0], 0, 0);
                ahead = 1;
				if (more && (left - ahead <= 0))
					more = 0;
			}

        } else if (*data >= 247 && *data <= 250) {      // push medium number
            if (left < 2)
                goto runoff_error;
            val = + ((data[0] - 247) << 8) + 108 + data[1];
            more = s_push(val);
            ahead = 2;

        } else if (*data >= 251 && *data <= 254) {      // push negative medium number
            if (left < 2)
                goto runoff_error;
            val = - ((data[0] - 251) << 8) - 108 - data[1];
            more = s_push(val);
            ahead = 2;

        } else {                                        // 255: push huge number
            if (left < 5)
                goto runoff_error;
            val32 = (data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4];
            more = s_push(val32 / 65536.);
            ahead = 5;
        }

        if (!more)
            return true;

        data += ahead;
        left -= ahead;
    }

  runoff_error:
    ERROR("Type2 CharString Parser Error: charstring commands past end");
    return false;
}

static void
cs_parse_init (void)
{
	cs_true.x = cs_false.x = 0;
	cs_true.y = cs_false.y = 0;
	status_chg = S_CHG_INITIAL;

    cs_sp = 0;
 	lsb.x = cp.x = seac_origin.x = 0;
	lsb.y = cp.y = seac_origin.y = 0;
    status_cci = S_CCI_INITIAL;
    flex = false;
    t2nhints = 0;
    subr_depth = 0;
    done = false;
    cs_error = CCI_errOK;
}

long
cs_convert_charstring (card8 *src, long srclen,
					   cff_index *gsubr, cff_index *subr,
					   double default_width, double nominal_width)
{
  cs_type1subr chr;

  t1_dst = curr_t1_dst = NEW(CS_STR_LEN_MAX, card8);
  t1_limit = curr_t1_limit = t1_dst + CS_STR_LEN_MAX;
  t1_dstlen = curr_t1_dstlen = 0;

  gsubr_idx = gsubr;
  subr_idx = subr;
  default_width_x = default_width; 
  nominal_width_x = nominal_width;

  cs_parse_init();

  width.x = 0.0;
  width.y = 0.0;
  swap_stem_hints();
  status_cgi = S_CGI_INITIAL;
  in_hr = false;

  /* expand call(g)subrs */
  do_charstring(&src, srclen);

  if (status_cgi == S_CGI_INITIAL)
    gen_sbw(false);
  else if (in_hr)
    act_hintmask(cs_endchar, 0, utarray_len(stem_hstem));
  if (status_cgi != S_CGI_SEAC)
    gen_command(cs_endchar);

  t1_dstlen = curr_t1_dstlen;
  chr.size = t1_dstlen;
  chr.data = NEW(t1_dstlen,card8);
  memcpy(chr.data,t1_dst,t1_dstlen);
  utarray_push_back(ut_glyphs,&chr);
  free(t1_dst);

  return t1_dstlen;
}

void
cs_start_conversion(int precision, boolean makeSubrs,
					UT_array *t1_glyphs, UT_array *t1_subrs)
{
    if (precision >= 1 && precision <= 107)
        cs_precision = precision;
    else
        cs_precision = 5;
    cs_f_precision = cs_precision;
	hr_firstsubr = utarray_len(t1_subrs);
	direct_hr = !makeSubrs;
	ut_glyphs = t1_glyphs;
	ut_subrs = t1_subrs;

	in_hr = false;
	max_flex_height = 0;
	bad_flex = false;
	last_hints = NULL;
	utarray_new(stem_pos,&double_icd);
	utarray_new(stem_width,&double_icd);
	utarray_new(stem_hstem,&ut_int_icd);
}

void
cs_end_conversion()
{
	utarray_free(stem_pos);
	utarray_free(stem_width);
	utarray_free(stem_hstem);
}
