/* mlist.c

   Copyright 2006-2009 Taco Hoekwater <taco@luatex.org>

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

#include "luatex-api.h"
#include <ptexlib.h>

#include "nodes.h"
#include "commands.h"

static const char _svn_version[] =
    "$Id: mlist.c 2099 2009-03-24 10:35:01Z taco $ $URL: http://scm.foundry.supelec.fr/svn/luatex/trunk/src/texk/web2c/luatexdir/tex/mlist.c $";

#define delimiter_factor     int_par(param_delimiter_factor_code)
#define delimiter_shortfall  dimen_par(param_delimiter_shortfall_code)
#define bin_op_penalty       int_par(param_bin_op_penalty_code)
#define rel_penalty          int_par(param_rel_penalty_code)
#define null_delimiter_space dimen_par(param_null_delimiter_space_code)
#define script_space         dimen_par(param_script_space_code)
#define disable_lig          int_par(param_disable_lig_code)
#define disable_kern         int_par(param_disable_kern_code)

#define reset_attributes(p,newatt) do {             \
    delete_attribute_ref(node_attr(p));             \
    node_attr(p) = newatt;                          \
    if (newatt!=null) {                             \
      assert(type(newatt)==attribute_list_node);    \
      add_node_attr_ref(node_attr(p));              \
    }                                               \
  } while (0)


#define DEFINE_MATH_PARAMETERS(A,B,C,D) do {                            \
    if (B==text_size) {                                                 \
      def_math_param(A, text_style, (C),D);                             \
      def_math_param(A, cramped_text_style, (C),D);                     \
    } else if (B==script_size) {                                        \
      def_math_param(A, script_style, (C),D);                           \
      def_math_param(A, cramped_script_style, (C),D);                   \
    } else if (B==script_script_size) {                                 \
      def_math_param(A, script_script_style, (C),D);                    \
      def_math_param(A, cramped_script_script_style, (C),D);            \
    }                                                                   \
  } while (0)

#define DEFINE_DMATH_PARAMETERS(A,B,C,D) do {                           \
    if (B==text_size) {                                                 \
      def_math_param(A, display_style,(C),D);                           \
      def_math_param(A, cramped_display_style,(C),D);                   \
    }                                                                   \
  } while (0)


#define is_new_mathfont(A) (font_math_params(A)>0)
#define is_old_mathfont(A,B) (font_math_params(A)==0 && font_params(A)>=(B))


#define font_MATH_par(a,b)                                                  \
  (font_math_params(a)>=b ? font_math_param(a,b) : undefined_math_parameter)


/* here are the math parameters that are font-dependant */

/*
@ Before an mlist is converted to an hlist, \TeX\ makes sure that
the fonts in family~2 have enough parameters to be math-symbol
fonts, and that the fonts in family~3 have enough parameters to be
math-extension fonts. The math-symbol parameters are referred to by using the
following macros, which take a size code as their parameter; for example,
|num1(cur_size)| gives the value of the |num1| parameter for the current size.
@^parameters for symbols@>
@^font parameters@>
*/

#define total_mathsy_params 22
#define total_mathex_params 13

#define mathsy(A,B) font_param(fam_fnt(2,A),B)
#define math_x_height(A) mathsy(A,5)    /* height of `\.x' */
#define math_quad(A) mathsy(A,6)        /* \.{18mu} */
#define num1(A) mathsy(A,8)     /* numerator shift-up in display styles */
#define num2(A) mathsy(A,9)     /* numerator shift-up in non-display, non-\.{\\atop} */
#define num3(A) mathsy(A,10)    /* numerator shift-up in non-display \.{\\atop} */
#define denom1(A) mathsy(A,11)  /* denominator shift-down in display styles */
#define denom2(A) mathsy(A,12)  /* denominator shift-down in non-display styles */
#define sup1(A) mathsy(A,13)    /* superscript shift-up in uncramped display style */
#define sup2(A) mathsy(A,14)    /* superscript shift-up in uncramped non-display */
#define sup3(A) mathsy(A,15)    /* superscript shift-up in cramped styles */
#define sub1(A) mathsy(A,16)    /* subscript shift-down if superscript is absent */
#define sub2(A) mathsy(A,17)    /* subscript shift-down if superscript is present */
#define sup_drop(A) mathsy(A,18)        /* superscript baseline below top of large box */
#define sub_drop(A) mathsy(A,19)        /* subscript baseline below bottom of large box */
#define delim1(A) mathsy(A,20)  /* size of \.{\\atopwithdelims} delimiters in display styles */
#define delim2(A) mathsy(A,21)  /* size of \.{\\atopwithdelims} delimiters in non-displays */
#define axis_height(A) mathsy(A,22)     /* height of fraction lines above the baseline */

/*
The math-extension parameters have similar macros, but the size code is
omitted (since it is always |cur_size| when we refer to such parameters).
@^parameters for symbols@>
@^font parameters@>
*/

#define mathex(A,B) font_param(fam_fnt(3,A),B)
#define default_rule_thickness(A) mathex(A,8)   /* thickness of \.{\\over} bars */
#define big_op_spacing1(A) mathex(A,9)  /* minimum clearance above a displayed op */
#define big_op_spacing2(A) mathex(A,10) /* minimum clearance below a displayed op */
#define big_op_spacing3(A) mathex(A,11) /* minimum baselineskip above displayed op */
#define big_op_spacing4(A) mathex(A,12) /* minimum baselineskip below displayed op */
#define big_op_spacing5(A) mathex(A,13) /* padding above and below displayed limits */

/* I made a bunch of extensions cf. the MATH table in OpenType, but some of
the MathConstants values have no matching usage in \LuaTeX\ right now.
 
ScriptPercentScaleDown,
ScriptScriptPercentScaleDown:
  These should be handled by the macro package, on the engine
  side there are three separate fonts

DelimitedSubFormulaMinHeight:
  This is perhaps related to word's natural math input? I have
  no idea what to do about it

MathLeading:
  LuaTeX does not currently handle multi-line displays, and
  the parameter does not seem to make much sense elsewhere

FlattenedAccentBaseHeight:
  This is based on the 'flac' GSUB feature. It would not be hard
  to support that, but proper math accent placements cf. MATH
  needs support for MathTopAccentAttachment table to be 
  implemented first

SkewedFractionHorizontalGap,
SkewedFractionVerticalGap:
  I am not sure it makes sense implementing skewed fractions,
  so I would like to see an example first

Also still TODO for OpenType Math:
  * cutins (math_kern)
  * extensible large operators
  * bottom accents
  * prescripts

*/

/* this is not really a math parameter at all */

void math_param_error(char *param, int style)
{
    char s[256];
    char *hlp[] = {
        "Sorry, but I can't typeset math unless various parameters have",
        "been set. This is normally done by loading special math fonts",
        "into the math family slots. Your font set is lacking at least",
        "the parameter mentioned earlier.",
        NULL
    };
    snprintf(s, 256, "Math error: parameter \\Umath%s\\%sstyle is not set",
             param, math_style_names[style]);
    tex_error(s, hlp);
    /* flush_math(); */
    return;
}


static scaled accent_base_height(integer f)
{
    scaled a;
    a = x_height(f);
    if (a == 0 && is_new_mathfont(f)) {
        a = font_MATH_par(f, AccentBaseHeight);
        if (a == undefined_math_parameter)
            a = 0;
    }
    return a;
}

/* the non-staticness of this function is for the benefit of |math.c| */

scaled get_math_quad(int var)
{
    scaled a = get_math_param(math_param_quad, var);
    if (a == undefined_math_parameter) {
        math_param_error("quad", var);
        a = 0;
    }
    return a;
}

/* this parameter is different because it is called with a size
   specifier instead of a style specifier. */

static scaled math_axis(int b)
{
    scaled a;
    int var;
    if (b == script_size)
        var = script_style;
    else if (b == script_script_size)
        var = script_script_style;
    else
        var = text_style;
    a = get_math_param(math_param_axis, var);
    if (a == undefined_math_parameter) {
        math_param_error("axis", var);
        a = 0;
    }
    return a;
}

static scaled minimum_operator_size(int var)
{
    scaled a = get_math_param(math_param_operator_size, var);
    return a;
}

/* Old-style fonts do not define the radical_rule. This allows |make_radical| to select
 * the backward compatibility code, and it means that we can't raise an error here.
 */

static scaled radical_rule(int var)
{
    scaled a = get_math_param(math_param_radical_rule, var);
    return a;
}

static scaled radical_degree_before(int var)
{
    scaled a = get_math_param(math_param_radical_degree_before, var);
    if (a == undefined_math_parameter) {
        math_param_error("radicaldegreebefore", var);
        a = 0;
    }
    return a;
}

static scaled connector_overlap_min(int var)
{
    scaled a = get_math_param(math_param_connector_overlap_min, var);
    if (a == undefined_math_parameter) {
        math_param_error("connectoroverlapmin", var);
        a = 0;
    }
    return a;
}


static scaled radical_degree_after(int var)
{
    scaled a = get_math_param(math_param_radical_degree_after, var);
    if (a == undefined_math_parameter) {
        math_param_error("radicaldegreeafter", var);
        a = 0;
    }
    return a;
}

static scaled radical_degree_raise(int var)
{
    scaled a = get_math_param(math_param_radical_degree_raise, var);
    if (a == undefined_math_parameter) {
        math_param_error("radicaldegreeraise", var);
        a = 0;
    }
    return a;
}


/* now follow all the trivial functions for the math parameters */

static scaled overbar_kern(int var)
{
    scaled a = get_math_param(math_param_overbar_kern, var);
    if (a == undefined_math_parameter) {
        math_param_error("overbarkern", var);
        a = 0;
    }
    return a;
}

static scaled overbar_rule(int var)
{
    scaled a = get_math_param(math_param_overbar_rule, var);
    if (a == undefined_math_parameter) {
        math_param_error("overbarrule", var);
        a = 0;
    }
    return a;
}

static scaled overbar_vgap(int var)
{
    scaled a = get_math_param(math_param_overbar_vgap, var);
    if (a == undefined_math_parameter) {
        math_param_error("overbarvgap", var);
        a = 0;
    }
    return a;
}

static scaled underbar_rule(int var)
{
    scaled a = get_math_param(math_param_underbar_rule, var);
    if (a == undefined_math_parameter) {
        math_param_error("underbarrule", var);
        a = 0;
    }
    return a;
}

static scaled underbar_vgap(int var)
{
    scaled a = get_math_param(math_param_underbar_vgap, var);
    if (a == undefined_math_parameter) {
        math_param_error("underbarvgap", var);
        a = 0;
    }
    return a;
}

static scaled underbar_kern(int var)
{
    scaled a = get_math_param(math_param_underbar_kern, var);
    if (a == undefined_math_parameter) {
        math_param_error("underbarkern", var);
        a = 0;
    }
    return a;
}

static scaled under_delimiter_vgap(int var)
{
    scaled a = get_math_param(math_param_under_delimiter_vgap, var);
    if (a == undefined_math_parameter) {
        math_param_error("underdelimitervgap", var);
        a = 0;
    }
    return a;
}

static scaled under_delimiter_bgap(int var)
{
    scaled a = get_math_param(math_param_under_delimiter_bgap, var);
    if (a == undefined_math_parameter) {
        math_param_error("underdelimiterbgap", var);
        a = 0;
    }
    return a;
}


static scaled over_delimiter_vgap(int var)
{
    scaled a = get_math_param(math_param_over_delimiter_vgap, var);
    if (a == undefined_math_parameter) {
        math_param_error("overdelimitervgap", var);
        a = 0;
    }
    return a;
}

static scaled over_delimiter_bgap(int var)
{
    scaled a = get_math_param(math_param_over_delimiter_bgap, var);
    if (a == undefined_math_parameter) {
        math_param_error("overdelimiterbgap", var);
        a = 0;
    }
    return a;
}



static scaled radical_vgap(int var)
{
    scaled a = get_math_param(math_param_radical_vgap, var);
    if (a == undefined_math_parameter) {
        math_param_error("radicalvgap", var);
        a = 0;
    }
    return a;
}


static scaled radical_kern(int var)
{
    scaled a = get_math_param(math_param_radical_kern, var);
    if (a == undefined_math_parameter) {
        math_param_error("radicalkern", var);
        a = 0;
    }
    return a;
}

static scaled stack_vgap(int var)
{
    scaled a = get_math_param(math_param_stack_vgap, var);
    if (a == undefined_math_parameter) {
        math_param_error("stackvgap", var);
        a = 0;
    }
    return a;
}

static scaled stack_num_up(int var)
{
    scaled a = get_math_param(math_param_stack_num_up, var);
    if (a == undefined_math_parameter) {
        math_param_error("stacknumup", var);
        a = 0;
    }
    return a;
}

static scaled stack_denom_down(int var)
{
    scaled a = get_math_param(math_param_stack_denom_down, var);
    if (a == undefined_math_parameter) {
        math_param_error("stackdenomdown", var);
        a = 0;
    }
    return a;
}

static scaled fraction_rule(int var)
{
    scaled a = get_math_param(math_param_fraction_rule, var);
    if (a == undefined_math_parameter) {
        math_param_error("fractionrule", var);
        a = 0;
    }
    return a;
}


static scaled fraction_num_vgap(int var)
{
    scaled a = get_math_param(math_param_fraction_num_vgap, var);
    if (a == undefined_math_parameter) {
        math_param_error("fractionnumvgap", var);
        a = 0;
    }
    return a;
}

static scaled fraction_denom_vgap(int var)
{
    scaled a = get_math_param(math_param_fraction_denom_vgap, var);
    if (a == undefined_math_parameter) {
        math_param_error("fractiondenomvgap", var);
        a = 0;
    }
    return a;
}

static scaled fraction_num_up(int var)
{
    scaled a = get_math_param(math_param_fraction_num_up, var);
    if (a == undefined_math_parameter) {
        math_param_error("fractionnumup", var);
        a = 0;
    }
    return a;
}

static scaled fraction_denom_down(int var)
{
    scaled a = get_math_param(math_param_fraction_denom_down, var);
    if (a == undefined_math_parameter) {
        math_param_error("fractiondenomdown", var);
        a = 0;
    }
    return a;
}

static scaled fraction_del_size(int var)
{
    scaled a = get_math_param(math_param_fraction_del_size, var);
    if (a == undefined_math_parameter) {
        math_param_error("fractiondelsize", var);
        a = 0;
    }
    return a;
}

static scaled limit_above_vgap(int var)
{
    scaled a = get_math_param(math_param_limit_above_vgap, var);
    if (a == undefined_math_parameter) {
        math_param_error("limitabovevgap", var);
        a = 0;
    }
    return a;
}

static scaled limit_above_bgap(int var)
{
    scaled a = get_math_param(math_param_limit_above_bgap, var);
    if (a == undefined_math_parameter) {
        math_param_error("limitabovebgap", var);
        a = 0;
    }
    return a;
}

static scaled limit_above_kern(int var)
{
    scaled a = get_math_param(math_param_limit_above_kern, var);
    if (a == undefined_math_parameter) {
        math_param_error("limitabovekern", var);
        a = 0;
    }
    return a;
}

static scaled limit_below_vgap(int var)
{
    scaled a = get_math_param(math_param_limit_below_vgap, var);
    if (a == undefined_math_parameter) {
        math_param_error("limitbelowvgap", var);
        a = 0;
    }
    return a;
}

static scaled limit_below_bgap(int var)
{
    scaled a = get_math_param(math_param_limit_below_bgap, var);
    if (a == undefined_math_parameter) {
        math_param_error("limitbelowbgap", var);
        a = 0;
    }
    return a;
}

static scaled limit_below_kern(int var)
{
    scaled a = get_math_param(math_param_limit_below_kern, var);
    if (a == undefined_math_parameter) {
        math_param_error("limitbelowkern", var);
        a = 0;
    }
    return a;
}


static scaled sub_shift_drop(int var)
{
    scaled a = get_math_param(math_param_sub_shift_drop, var);
    if (a == undefined_math_parameter) {
        math_param_error("subshiftdrop", var);
        a = 0;
    }
    return a;
}

static scaled sup_shift_drop(int var)
{
    scaled a = get_math_param(math_param_sup_shift_drop, var);
    if (a == undefined_math_parameter) {
        math_param_error("supshiftdrop", var);
        a = 0;
    }
    return a;
}

static scaled sub_shift_down(int var)
{
    scaled a = get_math_param(math_param_sub_shift_down, var);
    if (a == undefined_math_parameter) {
        math_param_error("subshiftdown", var);
        a = 0;
    }
    return a;
}

static scaled sub_sup_shift_down(int var)
{
    scaled a = get_math_param(math_param_sub_sup_shift_down, var);
    if (a == undefined_math_parameter) {
        math_param_error("subsupshiftdown", var);
        a = 0;
    }
    return a;
}

static scaled sub_top_max(int var)
{
    scaled a = get_math_param(math_param_sub_top_max, var);
    if (a == undefined_math_parameter) {
        math_param_error("subtopmax", var);
        a = 0;
    }
    return a;
}

static scaled sup_shift_up(int var)
{
    scaled a = get_math_param(math_param_sup_shift_up, var);
    if (a == undefined_math_parameter) {
        math_param_error("supshiftup", var);
        a = 0;
    }
    return a;
}

static scaled sup_bottom_min(int var)
{
    scaled a = get_math_param(math_param_sup_bottom_min, var);
    if (a == undefined_math_parameter) {
        math_param_error("supbottommin", var);
        a = 0;
    }
    return a;
}

static scaled sup_sub_bottom_max(int var)
{
    scaled a = get_math_param(math_param_sup_sub_bottom_max, var);
    if (a == undefined_math_parameter) {
        math_param_error("supsubbottommax", var);
        a = 0;
    }
    return a;
}

static scaled subsup_vgap(int var)
{
    scaled a = get_math_param(math_param_subsup_vgap, var);
    if (a == undefined_math_parameter) {
        math_param_error("subsupvgap", var);
        a = 0;
    }
    return a;
}

static scaled space_after_script(int var)
{
    scaled a = get_math_param(math_param_space_after_script, var);
    if (a == undefined_math_parameter) {
        math_param_error("spaceafterscript", var);
        a = 0;
    }
    return a;
}


/* This function is no longer useful */

boolean check_necessary_fonts(void)
{
    return false;               /* temp */
}

void fixup_math_parameters(integer fam_id, integer size_id, integer f,
                           integer lvl)
{
    if (is_new_mathfont(f)) {   /* fix all known parameters */

        DEFINE_MATH_PARAMETERS(math_param_quad, size_id, font_size(f), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_quad, size_id, font_size(f), lvl);
        DEFINE_MATH_PARAMETERS(math_param_axis, size_id,
                               font_MATH_par(f, AxisHeight), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_axis, size_id,
                                font_MATH_par(f, AxisHeight), lvl);
        DEFINE_MATH_PARAMETERS(math_param_overbar_kern, size_id,
                               font_MATH_par(f, OverbarExtraAscender), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_overbar_kern, size_id,
                                font_MATH_par(f, OverbarExtraAscender), lvl);
        DEFINE_MATH_PARAMETERS(math_param_overbar_rule, size_id,
                               font_MATH_par(f, OverbarRuleThickness), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_overbar_rule, size_id,
                                font_MATH_par(f, OverbarRuleThickness), lvl);
        DEFINE_MATH_PARAMETERS(math_param_overbar_vgap, size_id,
                               font_MATH_par(f, OverbarVerticalGap), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_overbar_vgap, size_id,
                                font_MATH_par(f, OverbarVerticalGap), lvl);
        DEFINE_MATH_PARAMETERS(math_param_underbar_kern, size_id,
                               font_MATH_par(f, UnderbarExtraDescender), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_underbar_kern, size_id,
                                font_MATH_par(f, UnderbarExtraDescender), lvl);
        DEFINE_MATH_PARAMETERS(math_param_underbar_rule, size_id,
                               font_MATH_par(f, UnderbarRuleThickness), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_underbar_rule, size_id,
                                font_MATH_par(f, UnderbarRuleThickness), lvl);
        DEFINE_MATH_PARAMETERS(math_param_underbar_vgap, size_id,
                               font_MATH_par(f, UnderbarVerticalGap), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_underbar_vgap, size_id,
                                font_MATH_par(f, UnderbarVerticalGap), lvl);

        DEFINE_MATH_PARAMETERS(math_param_under_delimiter_vgap, size_id,
                               font_MATH_par(f, StretchStackGapAboveMin), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_under_delimiter_vgap, size_id,
                                font_MATH_par(f, StretchStackGapAboveMin), lvl);
        DEFINE_MATH_PARAMETERS(math_param_under_delimiter_bgap, size_id,
                               font_MATH_par(f, StretchStackBottomShiftDown),
                               lvl);
        DEFINE_DMATH_PARAMETERS(math_param_under_delimiter_bgap, size_id,
                                font_MATH_par(f, StretchStackBottomShiftDown),
                                lvl);

        DEFINE_MATH_PARAMETERS(math_param_over_delimiter_vgap, size_id,
                               font_MATH_par(f, StretchStackGapBelowMin), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_over_delimiter_vgap, size_id,
                                font_MATH_par(f, StretchStackGapBelowMin), lvl);
        DEFINE_MATH_PARAMETERS(math_param_over_delimiter_bgap, size_id,
                               font_MATH_par(f, StretchStackTopShiftUp), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_over_delimiter_bgap, size_id,
                                font_MATH_par(f, StretchStackTopShiftUp), lvl);


        DEFINE_MATH_PARAMETERS(math_param_stack_num_up, size_id,
                               font_MATH_par(f, StackTopShiftUp), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_stack_num_up, size_id,
                                font_MATH_par(f, StackTopDisplayStyleShiftUp),
                                lvl);
        DEFINE_MATH_PARAMETERS(math_param_stack_denom_down, size_id,
                               font_MATH_par(f, StackBottomShiftDown), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_stack_denom_down, size_id,
                                font_MATH_par(f,
                                              StackBottomDisplayStyleShiftDown),
                                lvl);
        DEFINE_MATH_PARAMETERS(math_param_stack_vgap, size_id,
                               font_MATH_par(f, StackGapMin), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_stack_vgap, size_id,
                                font_MATH_par(f, StackDisplayStyleGapMin), lvl);

        DEFINE_MATH_PARAMETERS(math_param_radical_kern, size_id,
                               font_MATH_par(f, RadicalExtraAscender), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_radical_kern, size_id,
                                font_MATH_par(f, RadicalExtraAscender), lvl);

        DEFINE_DMATH_PARAMETERS(math_param_operator_size, size_id,
                                font_MATH_par(f, DisplayOperatorMinHeight),
                                lvl);

        DEFINE_MATH_PARAMETERS(math_param_radical_rule, size_id,
                               font_MATH_par(f, RadicalRuleThickness), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_radical_rule, size_id,
                                font_MATH_par(f, RadicalRuleThickness), lvl);
        DEFINE_MATH_PARAMETERS(math_param_radical_vgap, size_id,
                               font_MATH_par(f, RadicalVerticalGap), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_radical_vgap, size_id,
                                font_MATH_par(f,
                                              RadicalDisplayStyleVerticalGap),
                                lvl);
        DEFINE_MATH_PARAMETERS(math_param_radical_degree_before, size_id,
                               font_MATH_par(f, RadicalKernBeforeDegree), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_radical_degree_before, size_id,
                                font_MATH_par(f, RadicalKernBeforeDegree), lvl);
        DEFINE_MATH_PARAMETERS(math_param_radical_degree_after, size_id,
                               font_MATH_par(f, RadicalKernAfterDegree), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_radical_degree_after, size_id,
                                font_MATH_par(f, RadicalKernAfterDegree), lvl);
        DEFINE_MATH_PARAMETERS(math_param_radical_degree_raise, size_id,
                               font_MATH_par(f,
                                             RadicalDegreeBottomRaisePercent),
                               lvl);
        DEFINE_DMATH_PARAMETERS(math_param_radical_degree_raise, size_id,
                                font_MATH_par(f,
                                              RadicalDegreeBottomRaisePercent),
                                lvl);
        if (size_id == text_size) {
            def_math_param(math_param_sup_shift_up, display_style,
                           font_MATH_par(f, SuperscriptShiftUp), lvl);
            def_math_param(math_param_sup_shift_up, cramped_display_style,
                           font_MATH_par(f, SuperscriptShiftUpCramped), lvl);
            def_math_param(math_param_sup_shift_up, text_style,
                           font_MATH_par(f, SuperscriptShiftUp), lvl);
            def_math_param(math_param_sup_shift_up, cramped_text_style,
                           font_MATH_par(f, SuperscriptShiftUpCramped), lvl);
        } else if (size_id == script_size) {
            def_math_param(math_param_sup_shift_up, script_style,
                           font_MATH_par(f, SuperscriptShiftUp), lvl);
            def_math_param(math_param_sup_shift_up, cramped_script_style,
                           font_MATH_par(f, SuperscriptShiftUpCramped), lvl);
        } else if (size_id == script_script_size) {
            def_math_param(math_param_sup_shift_up, script_script_style,
                           font_MATH_par(f, SuperscriptShiftUp), lvl);
            def_math_param(math_param_sup_shift_up, cramped_script_script_style,
                           font_MATH_par(f, SuperscriptShiftUpCramped), lvl);
        }

        DEFINE_MATH_PARAMETERS(math_param_sub_shift_drop, size_id,
                               font_MATH_par(f, SubscriptBaselineDropMin), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_sub_shift_drop, size_id,
                                font_MATH_par(f, SubscriptBaselineDropMin),
                                lvl);
        DEFINE_MATH_PARAMETERS(math_param_sup_shift_drop, size_id,
                               font_MATH_par(f, SuperscriptBaselineDropMax),
                               lvl);
        DEFINE_DMATH_PARAMETERS(math_param_sup_shift_drop, size_id,
                                font_MATH_par(f, SuperscriptBaselineDropMax),
                                lvl);
        DEFINE_MATH_PARAMETERS(math_param_sub_shift_down, size_id,
                               font_MATH_par(f, SubscriptShiftDown), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_sub_shift_down, size_id,
                                font_MATH_par(f, SubscriptShiftDown), lvl);
        DEFINE_MATH_PARAMETERS(math_param_sub_sup_shift_down, size_id,
                               font_MATH_par(f, SubscriptShiftDown), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_sub_sup_shift_down, size_id,
                                font_MATH_par(f, SubscriptShiftDown), lvl);
        DEFINE_MATH_PARAMETERS(math_param_sub_top_max, size_id,
                               font_MATH_par(f, SubscriptTopMax), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_sub_top_max, size_id,
                                font_MATH_par(f, SubscriptTopMax), lvl);
        DEFINE_MATH_PARAMETERS(math_param_sup_bottom_min, size_id,
                               font_MATH_par(f, SuperscriptBottomMin), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_sup_bottom_min, size_id,
                                font_MATH_par(f, SuperscriptBottomMin), lvl);
        DEFINE_MATH_PARAMETERS(math_param_sup_sub_bottom_max, size_id,
                               font_MATH_par(f,
                                             SuperscriptBottomMaxWithSubscript),
                               lvl);
        DEFINE_DMATH_PARAMETERS(math_param_sup_sub_bottom_max, size_id,
                                font_MATH_par(f,
                                              SuperscriptBottomMaxWithSubscript),
                                lvl);
        DEFINE_MATH_PARAMETERS(math_param_subsup_vgap, size_id,
                               font_MATH_par(f, SubSuperscriptGapMin), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_subsup_vgap, size_id,
                                font_MATH_par(f, SubSuperscriptGapMin), lvl);

        DEFINE_MATH_PARAMETERS(math_param_limit_above_vgap, size_id,
                               font_MATH_par(f, UpperLimitGapMin), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_limit_above_vgap, size_id,
                                font_MATH_par(f, UpperLimitGapMin), lvl);
        DEFINE_MATH_PARAMETERS(math_param_limit_above_bgap, size_id,
                               font_MATH_par(f, UpperLimitBaselineRiseMin),
                               lvl);
        DEFINE_DMATH_PARAMETERS(math_param_limit_above_bgap, size_id,
                                font_MATH_par(f, UpperLimitBaselineRiseMin),
                                lvl);
        DEFINE_MATH_PARAMETERS(math_param_limit_above_kern, size_id, 0, lvl);
        DEFINE_DMATH_PARAMETERS(math_param_limit_above_kern, size_id, 0, lvl);
        DEFINE_MATH_PARAMETERS(math_param_limit_below_vgap, size_id,
                               font_MATH_par(f, LowerLimitGapMin), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_limit_below_vgap, size_id,
                                font_MATH_par(f, LowerLimitGapMin), lvl);
        DEFINE_MATH_PARAMETERS(math_param_limit_below_bgap, size_id,
                               font_MATH_par(f, LowerLimitBaselineDropMin),
                               lvl);
        DEFINE_DMATH_PARAMETERS(math_param_limit_below_bgap, size_id,
                                font_MATH_par(f, LowerLimitBaselineDropMin),
                                lvl);
        DEFINE_MATH_PARAMETERS(math_param_limit_below_kern, size_id, 0, lvl);
        DEFINE_DMATH_PARAMETERS(math_param_limit_below_kern, size_id, 0, lvl);

        DEFINE_MATH_PARAMETERS(math_param_fraction_rule, size_id,
                               font_MATH_par(f, FractionRuleThickness), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_fraction_rule, size_id,
                                font_MATH_par(f, FractionRuleThickness), lvl);
        DEFINE_MATH_PARAMETERS(math_param_fraction_num_vgap, size_id,
                               font_MATH_par(f, FractionNumeratorGapMin), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_fraction_num_vgap, size_id,
                                font_MATH_par(f,
                                              FractionNumeratorDisplayStyleGapMin),
                                lvl);
        DEFINE_MATH_PARAMETERS(math_param_fraction_num_up, size_id,
                               font_MATH_par(f, FractionNumeratorShiftUp), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_fraction_num_up, size_id,
                                font_MATH_par(f,
                                              FractionNumeratorDisplayStyleShiftUp),
                                lvl);
        DEFINE_MATH_PARAMETERS(math_param_fraction_denom_vgap, size_id,
                               font_MATH_par(f, FractionDenominatorGapMin),
                               lvl);
        DEFINE_DMATH_PARAMETERS(math_param_fraction_denom_vgap, size_id,
                                font_MATH_par(f,
                                              FractionDenominatorDisplayStyleGapMin),
                                lvl);
        DEFINE_MATH_PARAMETERS(math_param_fraction_denom_down, size_id,
                               font_MATH_par(f, FractionDenominatorShiftDown),
                               lvl);
        DEFINE_DMATH_PARAMETERS(math_param_fraction_denom_down, size_id,
                                font_MATH_par(f,
                                              FractionDenominatorDisplayStyleShiftDown),
                                lvl);
        DEFINE_MATH_PARAMETERS(math_param_fraction_del_size, size_id, 0, lvl);
        DEFINE_DMATH_PARAMETERS(math_param_fraction_del_size, size_id, 0, lvl);

        DEFINE_MATH_PARAMETERS(math_param_space_after_script, size_id,
                               font_MATH_par(f, SpaceAfterScript), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_space_after_script, size_id,
                                font_MATH_par(f, SpaceAfterScript), lvl);

        DEFINE_MATH_PARAMETERS(math_param_connector_overlap_min, size_id,
                               font_MATH_par(f, MinConnectorOverlap), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_connector_overlap_min, size_id,
                                font_MATH_par(f, MinConnectorOverlap), lvl);


    } else if (fam_id == 2 && is_old_mathfont(f, total_mathsy_params)) {
        /* fix old-style |sy| parameters */
        DEFINE_MATH_PARAMETERS(math_param_quad, size_id, math_quad(size_id),
                               lvl);
        DEFINE_DMATH_PARAMETERS(math_param_quad, size_id, math_quad(size_id),
                                lvl);
        DEFINE_MATH_PARAMETERS(math_param_axis, size_id, axis_height(size_id),
                               lvl);
        DEFINE_DMATH_PARAMETERS(math_param_axis, size_id, axis_height(size_id),
                                lvl);
        DEFINE_MATH_PARAMETERS(math_param_stack_num_up, size_id, num3(size_id),
                               lvl);
        DEFINE_DMATH_PARAMETERS(math_param_stack_num_up, size_id, num1(size_id),
                                lvl);
        DEFINE_MATH_PARAMETERS(math_param_stack_denom_down, size_id,
                               denom2(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_stack_denom_down, size_id,
                                denom1(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_fraction_num_up, size_id,
                               num2(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_fraction_num_up, size_id,
                                num1(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_fraction_denom_down, size_id,
                               denom2(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_fraction_denom_down, size_id,
                                denom1(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_fraction_del_size, size_id,
                               delim2(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_fraction_del_size, size_id,
                                delim1(size_id), lvl);
        if (size_id == text_size) {
            def_math_param(math_param_sup_shift_up, display_style,
                           sup1(size_id), lvl);
            def_math_param(math_param_sup_shift_up, cramped_display_style,
                           sup3(size_id), lvl);
            def_math_param(math_param_sup_shift_up, text_style, sup2(size_id),
                           lvl);
            def_math_param(math_param_sup_shift_up, cramped_text_style,
                           sup3(size_id), lvl);
        } else if (size_id == script_size) {
            def_math_param(math_param_sub_shift_drop, display_style,
                           sub_drop(size_id), lvl);
            def_math_param(math_param_sub_shift_drop, cramped_display_style,
                           sub_drop(size_id), lvl);
            def_math_param(math_param_sub_shift_drop, text_style,
                           sub_drop(size_id), lvl);
            def_math_param(math_param_sub_shift_drop, cramped_text_style,
                           sub_drop(size_id), lvl);
            def_math_param(math_param_sup_shift_drop, display_style,
                           sup_drop(size_id), lvl);
            def_math_param(math_param_sup_shift_drop, cramped_display_style,
                           sup_drop(size_id), lvl);
            def_math_param(math_param_sup_shift_drop, text_style,
                           sup_drop(size_id), lvl);
            def_math_param(math_param_sup_shift_drop, cramped_text_style,
                           sup_drop(size_id), lvl);
            def_math_param(math_param_sup_shift_up, script_style, sup2(size_id),
                           lvl);
            def_math_param(math_param_sup_shift_up, cramped_script_style,
                           sup3(size_id), lvl);
        } else if (size_id == script_script_size) {
            def_math_param(math_param_sub_shift_drop, script_style,
                           sub_drop(size_id), lvl);
            def_math_param(math_param_sub_shift_drop, cramped_script_style,
                           sub_drop(size_id), lvl);
            def_math_param(math_param_sub_shift_drop, script_script_style,
                           sub_drop(size_id), lvl);
            def_math_param(math_param_sub_shift_drop,
                           cramped_script_script_style, sub_drop(size_id), lvl);
            def_math_param(math_param_sup_shift_drop, script_style,
                           sup_drop(size_id), lvl);
            def_math_param(math_param_sup_shift_drop, cramped_script_style,
                           sup_drop(size_id), lvl);
            def_math_param(math_param_sup_shift_drop, script_script_style,
                           sup_drop(size_id), lvl);
            def_math_param(math_param_sup_shift_drop,
                           cramped_script_script_style, sup_drop(size_id), lvl);
            def_math_param(math_param_sup_shift_up, script_script_style,
                           sup2(size_id), lvl);
            def_math_param(math_param_sup_shift_up, cramped_script_script_style,
                           sup3(size_id), lvl);
        }
        DEFINE_MATH_PARAMETERS(math_param_sub_shift_down, size_id,
                               sub1(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_sub_shift_down, size_id,
                                sub1(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_sub_sup_shift_down, size_id,
                               sub2(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_sub_sup_shift_down, size_id,
                                sub2(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_sub_top_max, size_id,
                               (abs(math_x_height(size_id) * 4) / 5), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_sub_top_max, size_id,
                                (abs(math_x_height(size_id) * 4) / 5), lvl);
        DEFINE_MATH_PARAMETERS(math_param_sup_bottom_min, size_id,
                               (abs(math_x_height(size_id)) / 4), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_sup_bottom_min, size_id,
                                (abs(math_x_height(size_id)) / 4), lvl);
        DEFINE_MATH_PARAMETERS(math_param_sup_sub_bottom_max, size_id,
                               (abs(math_x_height(size_id) * 4) / 5), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_sup_sub_bottom_max, size_id,
                                (abs(math_x_height(size_id) * 4) / 5), lvl);

    } else if (fam_id == 3 && is_old_mathfont(f, total_mathex_params)) {
        /* fix old-style |ex| parameters */
        DEFINE_MATH_PARAMETERS(math_param_overbar_kern, size_id,
                               default_rule_thickness(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_overbar_rule, size_id,
                               default_rule_thickness(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_overbar_vgap, size_id,
                               3 * default_rule_thickness(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_overbar_kern, size_id,
                                default_rule_thickness(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_overbar_rule, size_id,
                                default_rule_thickness(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_overbar_vgap, size_id,
                                3 * default_rule_thickness(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_underbar_kern, size_id,
                               default_rule_thickness(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_underbar_rule, size_id,
                               default_rule_thickness(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_underbar_vgap, size_id,
                               3 * default_rule_thickness(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_underbar_kern, size_id,
                                default_rule_thickness(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_underbar_rule, size_id,
                                default_rule_thickness(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_underbar_vgap, size_id,
                                3 * default_rule_thickness(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_radical_kern, size_id,
                               default_rule_thickness(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_radical_kern, size_id,
                                default_rule_thickness(size_id), lvl);
        /* The display-size radical_vgap is done in |finalize_math_pamaters| because it needs values from both the
           sy and the ex font. */
        DEFINE_MATH_PARAMETERS(math_param_radical_vgap, size_id,
                               (default_rule_thickness(size_id) +
                                (abs(default_rule_thickness(size_id)) / 4)),
                               lvl);

        DEFINE_MATH_PARAMETERS(math_param_stack_vgap, size_id,
                               3 * default_rule_thickness(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_stack_vgap, size_id,
                                7 * default_rule_thickness(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_fraction_rule, size_id,
                               default_rule_thickness(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_fraction_rule, size_id,
                                default_rule_thickness(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_fraction_num_vgap, size_id,
                               default_rule_thickness(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_fraction_num_vgap, size_id,
                                3 * default_rule_thickness(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_fraction_denom_vgap, size_id,
                               default_rule_thickness(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_fraction_denom_vgap, size_id,
                                3 * default_rule_thickness(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_limit_above_vgap, size_id,
                               big_op_spacing1(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_limit_above_vgap, size_id,
                                big_op_spacing1(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_limit_above_bgap, size_id,
                               big_op_spacing3(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_limit_above_bgap, size_id,
                                big_op_spacing3(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_limit_above_kern, size_id,
                               big_op_spacing5(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_limit_above_kern, size_id,
                                big_op_spacing5(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_limit_below_vgap, size_id,
                               big_op_spacing2(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_limit_below_vgap, size_id,
                                big_op_spacing2(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_limit_below_bgap, size_id,
                               big_op_spacing4(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_limit_below_bgap, size_id,
                                big_op_spacing4(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_limit_below_kern, size_id,
                               big_op_spacing5(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_limit_below_kern, size_id,
                                big_op_spacing5(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_subsup_vgap, size_id,
                               4 * default_rule_thickness(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_subsup_vgap, size_id,
                                4 * default_rule_thickness(size_id), lvl);
        /* All of the |space_after_script|s are done in finalize_math_parameters because the
           \.{\\scriptspace} may have been altered by the user
         */
        DEFINE_MATH_PARAMETERS(math_param_connector_overlap_min, size_id, 0,
                               lvl);
        DEFINE_DMATH_PARAMETERS(math_param_connector_overlap_min, size_id, 0,
                                lvl);

        DEFINE_MATH_PARAMETERS(math_param_under_delimiter_vgap, size_id,
                               big_op_spacing2(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_under_delimiter_vgap, size_id,
                                big_op_spacing2(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_under_delimiter_bgap, size_id,
                               big_op_spacing4(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_under_delimiter_bgap, size_id,
                                big_op_spacing4(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_over_delimiter_vgap, size_id,
                               big_op_spacing1(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_over_delimiter_vgap, size_id,
                                big_op_spacing1(size_id), lvl);
        DEFINE_MATH_PARAMETERS(math_param_over_delimiter_bgap, size_id,
                               big_op_spacing3(size_id), lvl);
        DEFINE_DMATH_PARAMETERS(math_param_over_delimiter_bgap, size_id,
                                big_op_spacing3(size_id), lvl);


    }
}

/* this needs to be called just at the start of |mlist_to_hlist| */
void finalize_math_parameters(void)
{
    integer saved_trace = int_par(param_tracing_assigns_code);
    int_par(param_tracing_assigns_code) = 0;
    if (get_math_param(math_param_space_after_script, display_style) ==
        undefined_math_parameter) {
        def_math_param(math_param_space_after_script, display_style,
                       script_space, cur_level);
        def_math_param(math_param_space_after_script, text_style, script_space,
                       cur_level);
        def_math_param(math_param_space_after_script, script_style,
                       script_space, cur_level);
        def_math_param(math_param_space_after_script, script_script_style,
                       script_space, cur_level);
        def_math_param(math_param_space_after_script, cramped_display_style,
                       script_space, cur_level);
        def_math_param(math_param_space_after_script, cramped_text_style,
                       script_space, cur_level);
        def_math_param(math_param_space_after_script, cramped_script_style,
                       script_space, cur_level);
        def_math_param(math_param_space_after_script,
                       cramped_script_script_style, script_space, cur_level);
    }
    if (get_math_param(math_param_radical_vgap, display_style) ==
        undefined_math_parameter) {
        def_math_param(math_param_radical_vgap, display_style,
                       (default_rule_thickness(text_size) +
                        (abs(math_x_height(text_size)) / 4)), cur_level);
        def_math_param(math_param_radical_vgap, cramped_display_style,
                       (default_rule_thickness(text_size) +
                        (abs(math_x_height(text_size)) / 4)), cur_level);
    }
    if (get_math_param(math_param_radical_degree_raise, display_style) ==
        undefined_math_parameter) {
        DEFINE_MATH_PARAMETERS(math_param_radical_degree_raise,
                               script_script_size, 60, cur_level);
        DEFINE_MATH_PARAMETERS(math_param_radical_degree_raise, script_size, 60,
                               cur_level);
        DEFINE_MATH_PARAMETERS(math_param_radical_degree_raise, text_size, 60,
                               cur_level);
        DEFINE_DMATH_PARAMETERS(math_param_radical_degree_raise, text_size, 60,
                                cur_level);
    }
    if (get_math_param(math_param_radical_degree_before, display_style) ==
        undefined_math_parameter) {
        def_math_param(math_param_radical_degree_before,
                       cramped_script_script_style,
                       xn_over_d(get_math_quad(cramped_script_script_style), 5,
                                 18), cur_level);
        def_math_param(math_param_radical_degree_before, script_script_style,
                       xn_over_d(get_math_quad(script_script_style), 5, 18),
                       cur_level);
        def_math_param(math_param_radical_degree_before, cramped_script_style,
                       xn_over_d(get_math_quad(cramped_script_style), 5, 18),
                       cur_level);
        def_math_param(math_param_radical_degree_before, script_style,
                       xn_over_d(get_math_quad(script_style), 5, 18),
                       cur_level);
        def_math_param(math_param_radical_degree_before, cramped_text_style,
                       xn_over_d(get_math_quad(cramped_text_style), 5, 18),
                       cur_level);
        def_math_param(math_param_radical_degree_before, text_style,
                       xn_over_d(get_math_quad(text_style), 5, 18), cur_level);
        def_math_param(math_param_radical_degree_before, cramped_display_style,
                       xn_over_d(get_math_quad(cramped_display_style), 5, 18),
                       cur_level);
        def_math_param(math_param_radical_degree_before, display_style,
                       xn_over_d(get_math_quad(display_style), 5, 18),
                       cur_level);
    }

    if (get_math_param(math_param_radical_degree_after, display_style) ==
        undefined_math_parameter) {
        def_math_param(math_param_radical_degree_after,
                       cramped_script_script_style,
                       -xn_over_d(get_math_quad(cramped_script_script_style),
                                  10, 18), cur_level);
        def_math_param(math_param_radical_degree_after, script_script_style,
                       -xn_over_d(get_math_quad(script_script_style), 10, 18),
                       cur_level);
        def_math_param(math_param_radical_degree_after, cramped_script_style,
                       -xn_over_d(get_math_quad(cramped_script_style), 10, 18),
                       cur_level);
        def_math_param(math_param_radical_degree_after, script_style,
                       -xn_over_d(get_math_quad(script_style), 10, 18),
                       cur_level);
        def_math_param(math_param_radical_degree_after, cramped_text_style,
                       -xn_over_d(get_math_quad(cramped_text_style), 10, 18),
                       cur_level);
        def_math_param(math_param_radical_degree_after, text_style,
                       -xn_over_d(get_math_quad(text_style), 10, 18),
                       cur_level);
        def_math_param(math_param_radical_degree_after, cramped_display_style,
                       -xn_over_d(get_math_quad(cramped_display_style), 10, 18),
                       cur_level);
        def_math_param(math_param_radical_degree_after, display_style,
                       -xn_over_d(get_math_quad(display_style), 10, 18),
                       cur_level);
    }
    int_par(param_tracing_assigns_code) = saved_trace;

}


/* 
In order to convert mlists to hlists, i.e., noads to nodes, we need several
subroutines that are conveniently dealt with now.

Let us first introduce the macros that make it easy to get at the parameters and
other font information. A size code, which is a multiple of 256, is added to a
family number to get an index into the table of internal font numbers
for each combination of family and size.  (Be alert: Size codes get
larger as the type gets smaller.)
*/


char *math_size_string(integer s)
{
    if (s == text_size)
        return "textfont";
    else if (s == script_size)
        return "scriptfont";
    else
        return "scriptscriptfont";
}

void print_size(integer s)
{
    if (s == text_size)
        tprint_esc("textfont");
    else if (s == script_size)
        tprint_esc("scriptfont");
    else
        tprint_esc("scriptscriptfont");
}


/*
  @ When the style changes, the following piece of program computes associated
  information:
*/

#define setup_cur_size_and_mu() do {                                    \
    if (cur_style==script_style ||                                      \
        cur_style==cramped_script_style)                                \
      cur_size=script_size;                                             \
    else if (cur_style==script_script_style ||                          \
             cur_style==cramped_script_script_style)                    \
      cur_size=script_script_size;                                      \
    else cur_size=text_size;                                            \
    cur_mu=x_over_n(get_math_quad(cur_size),18);                        \
  } while (0)

/* a simple routine that creates a flat copy of a nucleus */

pointer math_clone(pointer q)
{
    pointer x;
    if (q == null)
        return null;
    x = new_node(type(q), 0);
    reset_attributes(x, node_attr(q));
    if (type(q) == math_char_node) {
        math_fam(x) = math_fam(q);
        math_character(x) = math_character(q);
    } else {
        math_list(x) = math_list(q);
    }
    return x;
}




/*
  @ Here is a function that returns a pointer to a rule node having a given
  thickness |t|. The rule will extend horizontally to the boundary of the vlist
  that eventually contains it.
*/

pointer do_fraction_rule(scaled t, pointer att)
{
    pointer p;                  /* the new node */
    p = new_rule();
    rule_dir(p) = math_direction;
    height(p) = t;
    depth(p) = 0;
    reset_attributes(p, att);
    return p;
}

/*
  @ The |overbar| function returns a pointer to a vlist box that consists of
  a given box |b|, above which has been placed a kern of height |k| under a
  fraction rule of thickness |t| under additional space of height |ht|.
*/

pointer overbar(pointer b, scaled k, scaled t, scaled ht, pointer att)
{
    pointer p, q;               /* nodes being constructed */
    p = new_kern(k);
    vlink(p) = b;
    reset_attributes(p, att);
    q = do_fraction_rule(t, att);
    vlink(q) = p;
    p = new_kern(ht);
    reset_attributes(p, att);
    vlink(p) = q;
    pack_direction = math_direction;
    q = vpackage(p, 0, additional, max_dimen);
    reset_attributes(q, att);
    return q;
}

/*
  Here is a subroutine that creates a new box, whose list contains a
  single character, and whose width includes the italic correction for
  that character. The height or depth of the box will be negative, if
  the height or depth of the character is negative; thus, this routine
  may deliver a slightly different result than |hpack| would produce.
*/

static pointer char_box(internal_font_number f, integer c, pointer bb)
{
    pointer b, p;               /* the new box and its character node */
    b = new_null_box();
    width(b) = char_width(f, c) + char_italic(f, c);
    height(b) = char_height(f, c);
    depth(b) = char_depth(f, c);
    reset_attributes(b, bb);
    p = new_char(f, c);
    reset_attributes(p, bb);
    list_ptr(b) = p;
    return b;
}

/*
 Another handy subroutine computes the height plus depth of
 a given character:
*/

scaled height_plus_depth(internal_font_number f, integer c)
{
    return (char_height(f, c) + char_depth(f, c));
}


/*
  When we build an extensible character, it's handy to have the
  following subroutine, which puts a given character on top
  of the characters already in box |b|:
*/

scaled stack_into_box(pointer b, internal_font_number f, integer c)
{
    pointer p;                  /* new node placed into |b| */
    p = char_box(f, c, node_attr(b));
    vlink(p) = list_ptr(b);
    list_ptr(b) = p;
    height(b) = height(p);
    return height_plus_depth(f, c);
}


scaled stack_into_hbox(pointer b, internal_font_number f, integer c)
{
    pointer p, q;               /* new node placed into |b| */
    p = char_box(f, c, node_attr(b));
    q = list_ptr(b);
    if (q == null) {
        list_ptr(b) = p;
    } else {
        while (vlink(q) != null)
            q = vlink(q);
        vlink(q) = p;
    }
    if (height(b) < height(p))
        height(b) = height(p);
    if (depth(b) < depth(p))
        depth(b) = depth(p);
    return char_width(f, c);
}



void add_delim_kern(pointer b, scaled s)
{
    pointer p;                  /* new node placed into |b| */
    p = new_kern(s);
    reset_attributes(p, node_attr(b));
    vlink(p) = list_ptr(b);
    list_ptr(b) = p;
}

void add_delim_hkern(pointer b, scaled s)
{
    pointer p, q;               /* new node placed into |b| */
    p = new_kern(s);
    reset_attributes(p, node_attr(b));
    q = list_ptr(b);
    if (q == null) {
        list_ptr(b) = p;
    } else {
        while (vlink(q) != null)
            q = vlink(q);
        vlink(q) = p;
    }
}



/* */

pointer get_delim_box(extinfo * ext, internal_font_number f, scaled v,
                      pointer att, int boxtype)
{
    pointer b;
    extinfo *cur;
    scaled min_overlap, prev_overlap;
    scaled b_max;               /* natural (maximum) height of the stack */
    scaled s_max;               /* amount of possible shrink in the stack */
    scaled a, wd, ht, dp, last_ht;
    integer cc;                 /* a temporary character code for extensibles  */
    integer i;                  /* a temporary counter number of extensible pieces */
    int with_extenders;
    int num_extenders, num_normal, num_total;
    scaled c, d, u;
    scaled *max_shrinks = NULL;
    assert(ext != NULL);
    b = new_null_box();
    type(b) = boxtype;
    reset_attributes(b, att);
    min_overlap = connector_overlap_min(cur_style);
    assert(min_overlap >= 0);
    with_extenders = 0;
    num_extenders = 0;
    num_normal = 0;
    cur = ext;
    while (cur != NULL) {
        if (!char_exists(f, cur->glyph)) {
            char *hlp[] = {
                "Each glyph part in an extensible item should exist in the font.",
                "I will give up trying to find a suitable size for now. Fix your font!",
                NULL
            };
            tex_error("Variant part doesn't exist.", hlp);
            width(b) = null_delimiter_space;
            return b;
        }
        if (cur->extender > 0)
            num_extenders++;
        else
            num_normal++;
        /* no negative overlaps or advances are allowed */
        if (cur->start_overlap < 0 || cur->end_overlap < 0 || cur->advance < 0) {
            char *hlp[] = {
                "All measurements in extensible items should be positive.",
                "To get around this problem, I have changed the font metrics.",
                "Fix your font!",
                NULL
            };
            tex_error("Extensible recipe has negative fields.", hlp);
            if (cur->start_overlap < 0)
                cur->start_overlap = 0;
            if (cur->end_overlap < 0)
                cur->end_overlap = 0;
            if (cur->advance < 0)
                cur->advance = 0;
        }
        cur = cur->next;
    }
    if (num_normal == 0) {
        char *hlp[] = {
            "Each extensible recipe should have at least one non-repeatable part.",
            "To get around this problem, I have changed the first part to be",
            "non-repeatable. Fix your font!",
            NULL
        };
        tex_error("Extensible recipe has no fixed parts.", hlp);
        ext->extender = 0;
        num_normal = 1;
        num_extenders--;
    }
    /* |ext| holds a linked list of numerous items that may or may not be 
       repeatable. For the total height, we have to figure out how many items
       are needed to create a stack of at least |v|. 
       The next |while| loop does  that. It has two goals: it finds out 
       the natural height |b_max|  of the all the parts needed to reach
       at least |v|,  and it sets |with_extenders| to the number of times 
       each of the repeatable items in |ext| has to be repeated to reach 
       that height.
     */
    cur = ext;
    prev_overlap = -1;
    b_max = 0;
    s_max = 0;
    for (cur = ext; cur != NULL; cur = cur->next) {
        /* substract width of the current overlap if this is not the first */
        if (cur->extender == 0) {       /* not an extender */
            a = cur->advance;
            if (a == 0) {
                if (boxtype == vlist_node)
                    a = height_plus_depth(f, cur->glyph);       /* for tfm fonts */
                else
                    a = char_width(f, cur->glyph);      /* for tfm fonts */
                assert(a > 0);
            }
            b_max += a;         /* add the advance value */
            if (prev_overlap >= 0) {
                c = min_overlap;
                if (c >= a)
                    c = (a - 1);
                b_max -= c;
                d = c;
                if (prev_overlap > cur->start_overlap) {
                    if (cur->start_overlap > d)
                        d = cur->start_overlap;
                } else {
                    if (prev_overlap > d)
                        d = prev_overlap;
                }
                s_max += (d - c);
            }
            prev_overlap = cur->end_overlap;
        }
    }
    if (b_max < v && num_extenders > 0) {       /* not large enough, but can grow */
      RETRY:
        with_extenders++;
        cur = ext;
        prev_overlap = -1;
        b_max = 0;
        s_max = 0;
        i = with_extenders;
        while (cur != NULL) {
            a = cur->advance;
            if (a == 0) {
                if (boxtype == vlist_node)
                    a = height_plus_depth(f, cur->glyph);       /* for tfm fonts */
                else
                    a = char_width(f, cur->glyph);
                assert(a >= 0);
            }
            /* substract width of the current overlap if this is not the first */
            if (prev_overlap >= 0) {
                c = min_overlap;
                if (c >= a)
                    c = (a - 1);
                b_max -= c;
                d = c;
                if (prev_overlap > cur->start_overlap) {
                    if (cur->start_overlap > d)
                        d = cur->start_overlap;
                } else {
                    if (prev_overlap > d)
                        d = prev_overlap;
                }
                s_max += (d - c);
            }
            if (cur->extender == 0) {   /* not an extender */
                i = 0;
                prev_overlap = cur->end_overlap;
            } else {
                i--;
                prev_overlap = cur->end_overlap;
            }
            b_max += a;         /* add the advance value */
            if (i <= 0) {       /* can be $-1$ if the first glyph is an extender  */
                cur = cur->next;
                i = with_extenders;
            }
        }
        if (b_max < v) {        /* not large enough, but can grow */
            goto RETRY;
        }
    }
    /* now |b_max| is the natural height or width, |with_extenders| holds
       the count of each extender that is needed, and the maximum 
       amount the stack can shrink by is |s_max|.

       |(b_max-v)| is the total amount of extra height or width that needs
       to be gotten rid of, and the total number of items in the stack is
       |(num_extenders*with_extenders)+num_normal|
     */
    /* create an array of maximum shrinks and fill it */
    num_total = ((num_extenders * with_extenders) + num_normal);
    if (num_total == 1) {
        /* weird, but could happen */
        cc = ext->glyph;
        (void) stack_into_box(b, f, cc);
        width(b) = char_width(f, cc);
        height(b) = char_height(f, cc);
        depth(b) = char_depth(f, cc);
        return b;
    }
    max_shrinks = xcalloc(num_total, sizeof(scaled));
    cur = ext;
    prev_overlap = -1;
    c = 0;
    i = 0;
  REDO:
    while (cur != NULL) {
        if (cur->extender == 0 || with_extenders) {
            if (prev_overlap >= 0) {
                d = prev_overlap;
                if (d > cur->start_overlap)
                    d = cur->start_overlap;
                if (d < min_overlap)
                    d = min_overlap;
                max_shrinks[c++] = (d - min_overlap);
            }
            prev_overlap = cur->end_overlap;
            if (cur->extender == 0) {
                /* simple char, just reset |i| */
                i = 0;
            } else {
                if (i == 0) {   /* first in loop */
                    i = with_extenders;
                    if (i != 1)
                        goto REDO;
                } else if (i == 1) {
                    /* done */
                    i = 0;
                } else {
                    i--;
                    if (i != 1)
                        goto REDO;
                }
            }
        }
        cur = cur->next;
    }
    /* now create the box contents */
    cur = ext;
    wd = 0;
    d = 0;
    ht = 0;
    dp = 0;
    if (boxtype == vlist_node) {
        while (cur != NULL) {
            cc = cur->glyph;
            if (char_width(f, cc) > wd)
                wd = char_width(f, cc);
            if (cur->extender > 0) {
                i = with_extenders;
                while (i > 0) {
                    ht += stack_into_box(b, f, cc);
                    if (d < (num_total - 1)) {
                        u = min_overlap;
                        if (s_max != 0)
                            u += xn_over_d(max_shrinks[d], (b_max - v), s_max);
                        add_delim_kern(b, -u);
                        ht -= u;
                    }
                    d++;
                    i--;
                }
            } else {
                ht += stack_into_box(b, f, cc);
                if (d < (num_total - 1)) {
                    u = min_overlap;
                    if (s_max != 0)
                        u += xn_over_d(max_shrinks[d], (b_max - v), s_max);
                    add_delim_kern(b, -u);
                    ht -= u;
                }
                d++;
            }
            cur = cur->next;
        }
        xfree(max_shrinks);
        /* it is important to use |ht| here instead of |v| because  if there
           was not enough shrink to get the correct size, it has to be centered
           based on its actual height. That actual height is not the same as
           |b_max| either because |min_overlap| can have ben set by the user
           outside of the font's control.
         */
        last_ht = 0;
        height(b) = ht;
        depth(b) = 0;
        /* the next correction is needed for radicals */
        if (list_ptr(b) != null && type(list_ptr(b)) == hlist_node && list_ptr(list_ptr(b)) != null && type(list_ptr(list_ptr(b))) == glyph_node) {     /* and it should be */
            last_ht =
                char_height(font(list_ptr(list_ptr(b))),
                            character(list_ptr(list_ptr(b))));
            height(b) = last_ht;
            depth(b) = ht - last_ht;
        }
        /*
           fprintf (stdout,"v=%f,b_max=%f,ht=%f,n=%d\n", (float)v/65536.0,
           (float)b_max/65536.0,(float)height(b)/65536.0,num_total);
         */
        width(b) = wd;
    } else {
        /* horizontal version */

        while (cur != NULL) {
            cc = cur->glyph;
            if (char_height(f, cc) > ht)
                ht = char_height(f, cc);
            if (char_depth(f, cc) > dp)
                dp = char_depth(f, cc);
            if (cur->extender > 0) {
                i = with_extenders;
                while (i > 0) {
                    wd += stack_into_hbox(b, f, cc);
                    if (d < (num_total - 1)) {
                        u = min_overlap;
                        if (s_max != 0)
                            u += xn_over_d(max_shrinks[d], (b_max - v), s_max);
                        add_delim_hkern(b, -u);
                        wd -= u;
                    }
                    d++;
                    i--;
                }
            } else {
                wd += stack_into_hbox(b, f, cc);
                if (d < (num_total - 1)) {
                    u = min_overlap;
                    if (s_max != 0)
                        u += xn_over_d(max_shrinks[d], (b_max - v), s_max);
                    add_delim_hkern(b, -u);
                    wd -= u;
                }
                d++;
            }
            cur = cur->next;
        }
        xfree(max_shrinks);
        /* it is important to use |wd| here instead of |v| because  if there
           was not enough shrink to get the correct size, it has to be centered
           based on its actual width. That actual width is not the same as
           |b_max| either because |min_overlap| can have ben set by the user
           outside of the font's control.
         */
        width(b) = wd;
    }
    return b;
}

pointer get_delim_vbox(extinfo * ext, internal_font_number f, scaled v,
                       pointer att)
{
    return get_delim_box(ext, f, v, att, vlist_node);
}

pointer get_delim_hbox(extinfo * ext, internal_font_number f, scaled v,
                       pointer att)
{
    return get_delim_box(ext, f, v, att, hlist_node);
}



/*
  @ The |var_delimiter| function, which finds or constructs a sufficiently
  large delimiter, is the most interesting of the auxiliary functions that
  currently concern us. Given a pointer |d| to a delimiter field in some noad,
  together with a size code |s| and a vertical distance |v|, this function
  returns a pointer to a box that contains the smallest variant of |d| whose
  height plus depth is |v| or more. (And if no variant is large enough, it
  returns the largest available variant.) In particular, this routine will
  construct arbitrarily large delimiters from extensible components, if
  |d| leads to such characters.
  
  The value returned is a box whose |shift_amount| has been set so that
  the box is vertically centered with respect to the axis in the given size.
  If a built-up symbol is returned, the height of the box before shifting
  will be the height of its topmost component.
*/

void endless_loop_error(internal_font_number g, integer y)
{
    char s[256];
    char *hlp[] = {
        "You managed to create a seemingly endless charlist chain in the current",
        "font. I have counted until 10000 already and still have not escaped, so"
            "I will jump out of the loop all by myself now. Fix your font!",
        NULL
    };
    snprintf(s, 256, "Math error: endless loop in charlist (U+%04x in %s)",
             (int) y, font_name(g));
    tex_error(s, hlp);
}

pointer var_delimiter(pointer d, integer s, scaled v)
{
    /* label found,continue; */
    pointer b;                  /* the box that will be constructed */
    internal_font_number f, g;  /* best-so-far and tentative font codes */
    integer c, i, x, y;         /* best-so-far and tentative character codes */
    scaled u;                   /* height-plus-depth of a tentative character */
    scaled w;                   /* largest height-plus-depth so far */
    integer z;                  /* runs through font family members */
    boolean large_attempt;      /* are we trying the ``large'' variant? */
    pointer att;                /* to save the current attribute list */
    extinfo *ext;
    att = null;
    f = null_font;
    c = 0;
    w = 0;
    large_attempt = false;
    if (d == null)
        goto FOUND;
    z = small_fam(d);
    x = small_char(d);
    i = 0;
    while (true) {
        /* The search process is complicated slightly by the facts that some of the
           characters might not be present in some of the fonts, and they might not
           be probed in increasing order of height. */
        if ((z != 0) || (x != 0)) {
            g = fam_fnt(z, s);
            if (g != null_font) {
                y = x;
              CONTINUE:
                i++;
                if (char_exists(g, y)) {
                    if (char_tag(g, y) == ext_tag) {
                        f = g;
                        c = y;
                        goto FOUND;
                    }
                    u = height_plus_depth(g, y);
                    if (u > w) {
                        f = g;
                        c = y;
                        w = u;
                        if (u >= v)
                            goto FOUND;
                    }
                    if (i > 10000) {
                        /* endless loop */
                        endless_loop_error(g, y);
                        goto FOUND;
                    }
                    if (char_tag(g, y) == list_tag) {
                        y = char_remainder(g, y);
                        goto CONTINUE;
                    }
                }
            }
        }
        if (large_attempt)
            goto FOUND;         /* there were none large enough */
        large_attempt = true;
        z = large_fam(d);
        x = large_char(d);
    }
  FOUND:
    if (d != null) {
        att = node_attr(d);
        node_attr(d) = null;
        flush_node(d);
    }
    if (f != null_font) {
        /* When the following code is executed, |char_tag(q)| will be equal to
           |ext_tag| if and only if a built-up symbol is supposed to be returned.
         */
        ext = NULL;
        if ((char_tag(f, c) == ext_tag) &&
            ((ext = get_charinfo_vert_variants(char_info(f, c))) != NULL)) {
            b = get_delim_vbox(ext, f, v, att);
            width(b) += char_italic(f, c);
        } else {
            b = char_box(f, c, att);
        }
    } else {
        b = new_null_box();
        reset_attributes(b, att);
        width(b) = null_delimiter_space;        /* use this width if no delimiter was found */
    }
    shift_amount(b) = half(height(b) - depth(b)) - math_axis(s);
    delete_attribute_ref(att);
    return b;
}

pointer flat_var_delimiter(pointer d, integer s, scaled v)
{
    /* label found,continue; */
    pointer b;                  /* the box that will be constructed */
    internal_font_number f, g;  /* best-so-far and tentative font codes */
    integer c, i, x, y;         /* best-so-far and tentative character codes */
    scaled u;                   /* height-plus-depth of a tentative character */
    scaled w;                   /* largest height-plus-depth so far */
    integer z;                  /* runs through font family members */
    boolean large_attempt;      /* are we trying the ``large'' variant? */
    pointer att;                /* to save the current attribute list */
    extinfo *ext;
    f = null_font;
    c = 0;
    w = 0;
    att = null;
    large_attempt = false;
    if (d == null)
        goto FOUND;
    z = small_fam(d);
    x = small_char(d);
    i = 0;
    while (true) {
        /* The search process is complicated slightly by the facts that some of the
           characters might not be present in some of the fonts, and they might not
           be probed in increasing order of height. */
        if ((z != 0) || (x != 0)) {
            g = fam_fnt(z, s);
            if (g != null_font) {
                y = x;
              CONTINUE:
                i++;
                if (char_exists(g, y)) {
                    if (char_tag(g, y) == ext_tag) {
                        f = g;
                        c = y;
                        goto FOUND;
                    }
                    u = char_width(g, y);
                    if (u > w) {
                        f = g;
                        c = y;
                        w = u;
                        if (u >= v)
                            goto FOUND;
                    }
                    if (i > 10000) {
                        /* endless loop */
                        endless_loop_error(g, y);
                        goto FOUND;
                    }
                    if (char_tag(g, y) == list_tag) {
                        y = char_remainder(g, y);
                        goto CONTINUE;
                    }
                }
            }
        }
        if (large_attempt)
            goto FOUND;         /* there were none large enough */
        large_attempt = true;
        z = large_fam(d);
        x = large_char(d);
    }
  FOUND:
    if (d != null) {
        att = node_attr(d);
        node_attr(d) = null;
        flush_node(d);
    }
    if (f != null_font) {
        /* When the following code is executed, |char_tag(q)| will be equal to
           |ext_tag| if and only if a built-up symbol is supposed to be returned.
         */
        ext = NULL;
        if ((char_tag(f, c) == ext_tag) &&
            ((ext = get_charinfo_hor_variants(char_info(f, c))) != NULL)) {
            b = get_delim_hbox(ext, f, v, att);
            width(b) += char_italic(f, c);
        } else {
            b = char_box(f, c, att);
        }
    } else {
        b = new_null_box();
        reset_attributes(b, att);
        width(b) = 0;           /* use this width if no delimiter was found */
    }
    delete_attribute_ref(att);
    return b;
}


/*
The next subroutine is much simpler; it is used for numerators and
denominators of fractions as well as for displayed operators and
their limits above and below. It takes a given box~|b| and
changes it so that the new box is centered in a box of width~|w|.
The centering is done by putting \.{\\hss} glue at the left and right
of the list inside |b|, then packaging the new box; thus, the
actual box might not really be centered, if it already contains
infinite glue.


The given box might contain a single character whose italic correction
has been added to the width of the box; in this case a compensating
kern is inserted.
*/

pointer rebox(pointer b, scaled w)
{
    pointer p, q, r, att;       /* temporary registers for list manipulation */
    internal_font_number f;     /* font in a one-character box */
    scaled v;                   /* width of a character without italic correction */

    if ((width(b) != w) && (list_ptr(b) != null)) {
        if (type(b) == vlist_node) {
            p = hpack(b, 0, additional);
            reset_attributes(p, node_attr(b));
            b = p;
        }
        p = list_ptr(b);
        att = node_attr(b);
        add_node_attr_ref(att);
        if ((is_char_node(p)) && (vlink(p) == null)) {
            f = font(p);
            v = char_width(f, character(p));
            if (v != width(b)) {
                q = new_kern(width(b) - v);
                reset_attributes(q, att);
                vlink(p) = q;
            }
        }
        list_ptr(b) = null;
        flush_node(b);
        b = new_glue(ss_glue);
        reset_attributes(b, att);
        vlink(b) = p;
        while (vlink(p) != null)
            p = vlink(p);
        q = new_glue(ss_glue);
        reset_attributes(q, att);
        vlink(p) = q;
        r = hpack(b, w, exactly);
        reset_attributes(r, att);
        delete_attribute_ref(att);
        return r;
    } else {
        width(b) = w;
        return b;
    }
}

/*
 Here is a subroutine that creates a new glue specification from another
one that is expressed in `\.{mu}', given the value of the math unit.
*/

#define mu_mult(A) mult_and_add(n,(A),xn_over_d((A),f,unity),max_dimen)

pointer math_glue(pointer g, scaled m)
{
    pointer p;                  /* the new glue specification */
    integer n;                  /* integer part of |m| */
    scaled f;                   /* fraction part of |m| */
    n = x_over_n(m, unity);
    f = tex_remainder;
    if (f < 0) {
        decr(n);
        f = f + unity;
    }
    p = new_node(glue_spec_node, 0);
    width(p) = mu_mult(width(g));       /* convert \.{mu} to \.{pt} */
    stretch_order(p) = stretch_order(g);
    if (stretch_order(p) == normal)
        stretch(p) = mu_mult(stretch(g));
    else
        stretch(p) = stretch(g);
    shrink_order(p) = shrink_order(g);
    if (shrink_order(p) == normal)
        shrink(p) = mu_mult(shrink(g));
    else
        shrink(p) = shrink(g);
    return p;
}

/*
The |math_kern| subroutine removes |mu_glue| from a kern node, given
the value of the math unit.
*/

void math_kern(pointer p, scaled m)
{
    integer n;                  /* integer part of |m| */
    scaled f;                   /* fraction part of |m| */
    if (subtype(p) == mu_glue) {
        n = x_over_n(m, unity);
        f = tex_remainder;
        if (f < 0) {
            decr(n);
            f = f + unity;
        }
        width(p) = mu_mult(width(p));
        subtype(p) = explicit;
    }
}

/*
 \TeX's most important routine for dealing with formulas is called
|mlist_to_hlist|.  After a formula has been scanned and represented as an
mlist, this routine converts it to an hlist that can be placed into a box
or incorporated into the text of a paragraph. There are three implicit
parameters, passed in global variables: |cur_mlist| points to the first
node or noad in the given mlist (and it might be |null|); |cur_style| is a
style code; and |mlist_penalties| is |true| if penalty nodes for potential
line breaks are to be inserted into the resulting hlist. After
|mlist_to_hlist| has acted, |vlink(temp_head)| points to the translated hlist.

Since mlists can be inside mlists, the procedure is recursive. And since this
is not part of \TeX's inner loop, the program has been written in a manner
that stresses compactness over efficiency.
@^recursion@>
*/

pointer cur_mlist;              /* beginning of mlist to be translated */
integer cur_style;              /* style code at current place in the list */
integer cur_size;               /* size code corresponding to |cur_style|  */
scaled cur_mu;                  /* the math unit width corresponding to |cur_size| */
boolean mlist_penalties;        /* should |mlist_to_hlist| insert penalties? */

void run_mlist_to_hlist(pointer p, integer m_style, boolean penalties)
{
    int callback_id;
    int a, sfix;
    lua_State *L = Luas;
    finalize_math_parameters();
    callback_id = callback_defined(mlist_to_hlist_callback);
    if (p != null && callback_id > 0) {
        sfix = lua_gettop(L);
        if (!get_callback(L, callback_id)) {
            lua_settop(L, sfix);
            return;
        }
        nodelist_to_lua(L, p);  /* arg 1 */
        lua_pushstring(L, math_style_names[m_style]);   /* arg 2 */
        lua_pushboolean(L, penalties);  /* arg 3 */
        if (lua_pcall(L, 3, 1, 0) != 0) {       /* 3 args, 1 result */
            fprintf(stdout, "error: %s\n", lua_tostring(L, -1));
            lua_settop(L, sfix);
            error();
            return;
        }
        a = nodelist_from_lua(L);
        lua_settop(L, sfix);
        vlink(temp_head) = a;
    } else {
        cur_mlist = p;
        cur_style = m_style;
        mlist_penalties = penalties;
        mlist_to_hlist();
    }
}


/*
@ The recursion in |mlist_to_hlist| is due primarily to a subroutine
called |clean_box| that puts a given noad field into a box using a given
math style; |mlist_to_hlist| can call |clean_box|, which can call
|mlist_to_hlist|.
@^recursion@>


The box returned by |clean_box| is ``clean'' in the
sense that its |shift_amount| is zero.
*/

pointer clean_box(pointer p, integer s)
{
    pointer q;                  /* beginning of a list to be boxed */
    integer save_style;         /* |cur_style| to be restored */
    pointer x;                  /* box to be returned */
    pointer r;                  /* temporary pointer */
    switch (type(p)) {
    case math_char_node:
        cur_mlist = new_noad();
        r = math_clone(p);
        nucleus(cur_mlist) = r;
        break;
    case sub_box_node:
        q = math_list(p);
        goto FOUND;
        break;
    case sub_mlist_node:
        cur_mlist = math_list(p);
        break;
    default:
        q = new_null_box();
        goto FOUND;
    }
    save_style = cur_style;
    cur_style = s;
    mlist_penalties = false;
    mlist_to_hlist();
    q = vlink(temp_head);       /* recursive call */
    cur_style = save_style;     /* restore the style */
    setup_cur_size_and_mu();
  FOUND:
    if (is_char_node(q) || (q == null))
        x = hpack(q, 0, additional);
    else if ((vlink(q) == null) && (type(q) <= vlist_node)
             && (shift_amount(q) == 0))
        x = q;                  /* it's already clean */
    else
        x = hpack(q, 0, additional);
    if (x != q && q != null)
        reset_attributes(x, node_attr(q));
    /* Here we save memory space in a common case. */
    q = list_ptr(x);
    if (is_char_node(q)) {
        r = vlink(q);
        if (r != null) {
            if (vlink(r) == null) {
                if (!is_char_node(r)) {
                    if (type(r) == kern_node) {
                        /* unneeded italic correction */
                        flush_node(r);
                        vlink(q) = null;
                    }
                }
            }
        }
    }
    return x;
}

/*
 It is convenient to have a procedure that converts a |math_char|
field to an ``unpacked'' form. The |fetch| routine sets |cur_f| and |cur_c|
to the font code and character code of a given noad field. 
It also takes care of issuing error messages for
nonexistent characters; in such cases, |char_exists(cur_f,cur_c)| will be |false|
after |fetch| has acted, and the field will also have been reset to |null|.
*/
/* The outputs of |fetch| are placed in global variables. */

internal_font_number cur_f;     /* the |font| field of a |math_char| */
integer cur_c;                  /* the |character| field of a |math_char| */

void fetch(pointer a)
{                               /* unpack the |math_char| field |a| */
    cur_c = math_character(a);
    cur_f = fam_fnt(math_fam(a), cur_size);
    if (cur_f == null_font) {
        char *msg;
        char *hlp[] = {
            "Somewhere in the math formula just ended, you used the",
            "stated character from an undefined font family. For example,",
            "plain TeX doesn't allow \\it or \\sl in subscripts. Proceed,",
            "and I'll try to forget that I needed that character.",
            NULL
        };
        msg = xmalloc(256);
        snprintf(msg, 255, "\\%s%d is undefined (character %d)",
                 math_size_string(cur_size), (int) math_fam(a), (int) cur_c);
        tex_error(msg, hlp);
        free(msg);
    } else {
        if (!(char_exists(cur_f, cur_c))) {
            char_warning(cur_f, cur_c);
        }
    }
}


/*
We need to do a lot of different things, so |mlist_to_hlist| makes two
passes over the given mlist.

The first pass does most of the processing: It removes ``mu'' spacing from
glue, it recursively evaluates all subsidiary mlists so that only the
top-level mlist remains to be handled, it puts fractions and square roots
and such things into boxes, it attaches subscripts and superscripts, and
it computes the overall height and depth of the top-level mlist so that
the size of delimiters for a |fence_noad| will be known.
The hlist resulting from each noad is recorded in that noad's |new_hlist|
field, an integer field that replaces the |nucleus| or |thickness|.
@^recursion@>

The second pass eliminates all noads and inserts the correct glue and
penalties between nodes.
*/

void assign_new_hlist(pointer q, pointer r)
{
    switch (type(q)) {
    case fraction_noad:
        math_list(numerator(q)) = null;
        flush_node(numerator(q));
        numerator(q) = null;
        math_list(denominator(q)) = null;
        flush_node(denominator(q));
        denominator(q) = null;
        break;
    case radical_noad:
    case ord_noad:
    case op_noad:
    case bin_noad:
    case rel_noad:
    case open_noad:
    case close_noad:
    case punct_noad:
    case inner_noad:
    case over_noad:
    case under_noad:
    case vcenter_noad:
    case accent_noad:
        if (nucleus(q) != null) {
            math_list(nucleus(q)) = null;
            flush_node(nucleus(q));
            nucleus(q) = null;
        }
        break;
    }
    new_hlist(q) = r;
}

#define choose_mlist(A) do { p=A(q); A(q)=null; } while (0)

/*
Most of the actual construction work of |mlist_to_hlist| is done
by procedures with names
like |make_fraction|, |make_radical|, etc. To illustrate
the general setup of such procedures, let's begin with a couple of
simple ones.
*/

void make_over(pointer q)
{
    pointer p;
    p = overbar(clean_box(nucleus(q), cramped_style(cur_style)),
                overbar_vgap(cur_style),
                overbar_rule(cur_style),
                overbar_kern(cur_style), node_attr(nucleus(q)));
    math_list(nucleus(q)) = p;
    type(nucleus(q)) = sub_box_node;
}

void make_under(pointer q)
{
    pointer p, x, y, r;         /* temporary registers for box construction */
    scaled delta;               /* overall height plus depth */
    x = clean_box(nucleus(q), cur_style);
    p = new_kern(underbar_vgap(cur_style));
    reset_attributes(p, node_attr(q));
    vlink(x) = p;
    r = do_fraction_rule(underbar_rule(cur_style), node_attr(q));
    vlink(p) = r;
    pack_direction = math_direction;
    y = vpackage(x, 0, additional, max_dimen);
    reset_attributes(y, node_attr(q));
    delta = height(y) + depth(y) + underbar_kern(cur_style);
    height(y) = height(x);
    depth(y) = delta - height(y);
    math_list(nucleus(q)) = y;
    type(nucleus(q)) = sub_box_node;
}

void make_vcenter(pointer q)
{
    pointer v;                  /* the box that should be centered vertically */
    scaled delta;               /* its height plus depth */
    v = math_list(nucleus(q));
    if (type(v) != vlist_node)
        tconfusion("vcenter");  /* this can't happen vcenter */
    delta = height(v) + depth(v);
    height(v) = math_axis(cur_size) + half(delta);
    depth(v) = delta - height(v);
}

/*
@ According to the rules in the \.{DVI} file specifications, we ensure alignment
@^square roots@>
between a square root sign and the rule above its nucleus by assuming that the
baseline of the square-root symbol is the same as the bottom of the rule. The
height of the square-root symbol will be the thickness of the rule, and the
depth of the square-root symbol should exceed or equal the height-plus-depth
of the nucleus plus a certain minimum clearance~|psi|. The symbol will be
placed so that the actual clearance is |psi| plus half the excess.
*/

void make_radical(pointer q)
{
    pointer x, y, p;            /* temporary registers for box construction */
    scaled delta, clr, theta, h;        /* dimensions involved in the calculation */
    x = clean_box(nucleus(q), cramped_style(cur_style));
    clr = radical_vgap(cur_style);
    theta = radical_rule(cur_style);
    if (theta == undefined_math_parameter) {
        theta = fraction_rule(cur_style);
        y = var_delimiter(left_delimiter(q), cur_size,
                          height(x) + depth(x) + clr + theta);
        left_delimiter(q) = null;
        theta = height(y);
        delta = depth(y) - (height(x) + depth(x) + clr);
        if (delta > 0)
            clr = clr + half(delta);    /* increase the actual clearance */
        shift_amount(y) = -(height(x) + clr);
        h = shift_amount(y) + height(y);
    } else {
        y = var_delimiter(left_delimiter(q), cur_size,
                          height(x) + depth(x) + clr + theta);
        left_delimiter(q) = null;
        delta = height(y) - (height(x) + clr + theta);
        shift_amount(y) = delta;
        h = -(height(y) - shift_amount(y));
    }
    p = overbar(x, clr, theta, radical_kern(cur_style), node_attr(y));
    vlink(y) = p;
    if (degree(q) != null) {
        scaled wr, br, ar;
        pointer r = clean_box(degree(q), script_script_style);
        reset_attributes(r, node_attr(degree(q)));
        wr = width(r);
        if (wr == 0) {
            flush_node(r);
        } else {
            br = radical_degree_before(cur_style);
            ar = radical_degree_after(cur_style);
            if (-ar > (wr + br))
                ar = -(wr + br);
            x = new_kern(ar);
            reset_attributes(x, node_attr(degree(q)));
            vlink(x) = y;
            shift_amount(r) =
                (xn_over_d(h, radical_degree_raise(cur_style), 100));
            vlink(r) = x;
            x = new_kern(br);
            reset_attributes(x, node_attr(degree(q)));
            vlink(x) = r;
            y = x;
        }
        degree(q) = null;
    }
    p = hpack(y, 0, additional);
    reset_attributes(p, node_attr(q));
    math_list(nucleus(q)) = p;
    type(nucleus(q)) = sub_box_node;
}


/* Construct a vlist box */
static pointer 
wrapup_delimiter (pointer x, pointer y, pointer q, 
                  scaled shift_up, scaled shift_down) {
    pointer p;  /* temporary register for box construction */
    pointer v = new_null_box();
    type(v) = vlist_node;
    height(v) = shift_up + height(x);
    depth(v) = depth(y) + shift_down;
    reset_attributes(v, node_attr(q));
    p = new_kern((shift_up - depth(x)) - (height(y) - shift_down));
    reset_attributes(p, node_attr(q));
    vlink(p) = y;
    vlink(x) = p;
    list_ptr(v) = x;
    return v;
}

#define fixup_widths(x,y) do {                      \
        if (width(y) >= width(x)) {                 \
            width(x) = width(y);                    \
        } else {                                    \
            width(y) = width(x);                    \
        }                                           \
    } while (0)

/* this has the |nucleus| box |x| as a limit above an extensible delimiter |y| */

void make_over_delimiter(pointer q)
{
    pointer x, y, v;         /* temporary registers for box construction */
    scaled shift_up, shift_down, clr, delta;
    x = clean_box(nucleus(q), sub_style(cur_style));
    y = flat_var_delimiter(left_delimiter(q), cur_size, width(x));
    left_delimiter(q) = null;
    fixup_widths(x,y);
    shift_up = over_delimiter_bgap(cur_style);
    shift_down = 0;             /* under_delimiter_bgap(cur_style); */
    clr = over_delimiter_vgap(cur_style);
    delta = clr - ((shift_up - depth(x)) - (height(y) - shift_down));
    if (delta > 0) {
        shift_up = shift_up + delta;
    }
    v = wrapup_delimiter (x, y, q, shift_up, shift_down);
    width(v) = width(x);        /* this also equals |width(y)| */
    math_list(nucleus(q)) = v;
    type(nucleus(q)) = sub_box_node;
}

/* this has the extensible delimiter |x| as a limit above |nucleus| box |y|  */

void make_delimiter_over(pointer q)
{
    pointer x, y, v;         /* temporary registers for box construction */
    scaled shift_up, shift_down, clr, delta;
    y = clean_box(nucleus(q), cur_style);
    x = flat_var_delimiter(left_delimiter(q), cur_size+(cur_size==script_script_size?0:1), width(y));
    left_delimiter(q) = null;
    fixup_widths(x,y);
    shift_up = over_delimiter_bgap(cur_style);
    shift_down = 0; 
    clr = over_delimiter_vgap(cur_style);
    delta = clr - ((shift_up - depth(x)) - (height(y) - shift_down));
    if (delta > 0) {
        shift_up = shift_up + delta;
    }
    v = wrapup_delimiter (x, y, q, shift_up, shift_down);
    width(v) = width(x);        /* this also equals |width(y)| */
    math_list(nucleus(q)) = v;
    type(nucleus(q)) = sub_box_node;
}


/* this has the extensible delimiter |y| as a limit below a |nucleus| box |x| */

void make_delimiter_under(pointer q)
{
    pointer x, y, v;         /* temporary registers for box construction */
    scaled shift_up, shift_down, clr, delta;
    x = clean_box(nucleus(q), cur_style);
    y = flat_var_delimiter(left_delimiter(q), cur_size+(cur_size==script_script_size?0:1) , width(x));
    left_delimiter(q) = null;
    fixup_widths(x,y);
    shift_up = 0;               /* over_delimiter_bgap(cur_style); */
    shift_down = under_delimiter_bgap(cur_style);
    clr = under_delimiter_vgap(cur_style);
    delta = clr - ((shift_up - depth(x)) - (height(y) - shift_down));
    if (delta > 0) {
        shift_down = shift_down + delta;
    }
    v = wrapup_delimiter (x, y, q, shift_up, shift_down);
    width(v) = width(y);        /* this also equals |width(y)| */
    math_list(nucleus(q)) = v;
    type(nucleus(q)) = sub_box_node;

}

/* this has the extensible delimiter |x| as a limit below |nucleus| box |y| */

void make_under_delimiter(pointer q)
{
    pointer x, y, v;         /* temporary registers for box construction */
    scaled shift_up, shift_down, clr, delta;
    y = clean_box(nucleus(q), sup_style(cur_style));
    x = flat_var_delimiter(left_delimiter(q), cur_size, width(y));
    left_delimiter(q) = null;
    fixup_widths(x,y);
    shift_up = 0;               /* over_delimiter_bgap(cur_style); */
    shift_down = under_delimiter_bgap(cur_style);
    clr = under_delimiter_vgap(cur_style);
    delta = clr - ((shift_up - depth(x)) - (height(y) - shift_down));
    if (delta > 0) {
        shift_down = shift_down + delta;
    }
    v = wrapup_delimiter (x, y, q, shift_up, shift_down);
    width(v) = width(y);        /* this also equals |width(y)| */
    math_list(nucleus(q)) = v;
    type(nucleus(q)) = sub_box_node;

}

/*
Slants are not considered when placing accents in math mode. The accenter is
centered over the accentee, and the accent width is treated as zero with
respect to the size of the final box.
*/

#define TOP_CODE 1
#define BOT_CODE 2

void do_make_math_accent(pointer q, internal_font_number f, integer c,
                         int top_or_bot)
{
    pointer p, r, x, y;         /* temporary registers for box construction */
    scaled s;                   /* amount to skew the accent to the right */
    scaled h;                   /* height of character being accented */
    scaled delta;               /* space to remove between accent and accentee */
    scaled w;                   /* width of the accentee, not including sub/superscripts */
    boolean s_is_absolute;      /* will be true if a top-accent is placed in |s| */
    extinfo *ext;
    pointer attr_p;
    attr_p = (top_or_bot == TOP_CODE ? accent_chr(q) : bot_accent_chr(q));
    s_is_absolute = false;
    c = cur_c;
    f = cur_f;
    /* Compute the amount of skew, or set |s| to an alignment point */
    s = 0;
    if (type(nucleus(q)) == math_char_node) {
        fetch(nucleus(q));
        if (top_or_bot == TOP_CODE) {
            s = char_top_accent(cur_f, cur_c);
            if (s != 0) {
                s_is_absolute = true;
            } else {
                s = get_kern(cur_f, cur_c, skew_char(cur_f));
            }
        } else {                /* new skewchar madness for bot accents */
            s = char_bot_accent(cur_f, cur_c);
            if (s != 0) {
                s_is_absolute = true;
            }
        }
    }
    x = clean_box(nucleus(q), cramped_style(cur_style));
    w = width(x);
    h = height(x);
    /* Switch to a larger accent if available and appropriate */
    y = null;
    while (1) {
        ext = NULL;
        if ((char_tag(f, c) == ext_tag) &&
            ((ext = get_charinfo_hor_variants(char_info(f, c))) != NULL)) {
            scaled w1 = xn_over_d(w, delimiter_factor, 1000);
            if (w - w1 > delimiter_shortfall)
                w1 = w - delimiter_shortfall;
            y = get_delim_hbox(ext, f, w1, node_attr(attr_p));
            break;
        } else if (char_tag(f, c) != list_tag) {
            break;
        } else {
            integer yy = char_remainder(f, c);
            if (!char_exists(f, yy))
                break;
            if (char_width(f, yy) > w)
                break;
            c = yy;
        }
    }
    if (y == null) {
        y = char_box(f, c, node_attr(attr_p));
    }
    if (top_or_bot == TOP_CODE) {
        if (h < accent_base_height(f))
            delta = h;
        else
            delta = accent_base_height(f);
    } else {
        delta = 0;              /* hm */
    }
    if ((supscr(q) != null) || (subscr(q) != null)) {
        if (type(nucleus(q)) == math_char_node) {
            /* Swap the subscript and superscript into box |x| */
            flush_node_list(x);
            x = new_noad();
            r = math_clone(nucleus(q));
            nucleus(x) = r;
            supscr(x) = supscr(q);
            supscr(q) = null;
            subscr(x) = subscr(q);
            subscr(q) = null;
            type(nucleus(q)) = sub_mlist_node;
            math_list(nucleus(q)) = x;
            x = clean_box(nucleus(q), cur_style);
            delta = delta + height(x) - h;
            h = height(x);
        }
    }
    if (s_is_absolute) {
        scaled sa;
        if (top_or_bot == TOP_CODE)
            sa = char_top_accent(f, c);
        else
            sa = char_bot_accent(f, c);
        if (sa == 0) {
            sa = half(width(y));        /* just take the center */
        }
        shift_amount(y) = s - sa;
    } else {
        shift_amount(y) = s + half(w - width(y));
    }
    width(y) = 0;
    if (top_or_bot == TOP_CODE) {
        p = new_kern(-delta);
        vlink(p) = x;
        vlink(y) = p;
    } else {
        /*
           p = new_kern(-delta);
           vlink(x) = p;
           vlink(p) = y;
           y = x;
         */
        vlink(x) = y;
        y = x;
    }
    pack_direction = math_direction;
    r = vpackage(y, 0, additional, max_dimen);
    reset_attributes(r, node_attr(q));
    width(r) = w;
    y = r;
    if (top_or_bot == TOP_CODE) {
        if (height(y) < h) {
            /* Make the height of box |y| equal to |h| */
            p = new_kern(h - height(y));
            reset_attributes(p, node_attr(q));
            vlink(p) = list_ptr(y);
            list_ptr(y) = p;
            height(y) = h;
        }
    } else {
        shift_amount(y) = -(h - height(y));
    }
    math_list(nucleus(q)) = y;
    type(nucleus(q)) = sub_box_node;
}

void make_math_accent(pointer q)
{
    if (accent_chr(q) != null) {
        fetch(accent_chr(q));
        if (char_exists(cur_f, cur_c)) {
            do_make_math_accent(q, cur_f, cur_c, TOP_CODE);
        }
        flush_node(accent_chr(q));
        accent_chr(q) = null;
    }
    if (bot_accent_chr(q) != null) {
        fetch(bot_accent_chr(q));
        if (char_exists(cur_f, cur_c)) {
            do_make_math_accent(q, cur_f, cur_c, BOT_CODE);
        }
        flush_node(bot_accent_chr(q));
        bot_accent_chr(q) = null;
    }
}

/*
The |make_fraction| procedure is a bit different because it sets
|new_hlist(q)| directly rather than making a sub-box.
*/

void make_fraction(pointer q)
{
    pointer p, v, x, y, z;      /* temporary registers for box construction */
    scaled delta, delta1, delta2, shift_up, shift_down, clr;
    /* dimensions for box calculations */
    if (thickness(q) == default_code)
        thickness(q) = fraction_rule(cur_style);
    /* Create equal-width boxes |x| and |z| for the numerator and denominator,
       and compute the default amounts |shift_up| and |shift_down| by which they
       are displaced from the baseline */
    x = clean_box(numerator(q), num_style(cur_style));
    z = clean_box(denominator(q), denom_style(cur_style));
    if (width(x) < width(z))
        x = rebox(x, width(z));
    else
        z = rebox(z, width(x));
    if (thickness(q) == 0) {
        shift_up = stack_num_up(cur_style);
        shift_down = stack_denom_down(cur_style);
        /* The numerator and denominator must be separated by a certain minimum
           clearance, called |clr| in the following program. The difference between
           |clr| and the actual clearance is |2delta|. */
        clr = stack_vgap(cur_style);
        delta = half(clr - ((shift_up - depth(x)) - (height(z) - shift_down)));
        if (delta > 0) {
            shift_up = shift_up + delta;
            shift_down = shift_down + delta;
        }
    } else {
        shift_up = fraction_num_up(cur_style);
        shift_down = fraction_denom_down(cur_style);
        /* In the case of a fraction line, the minimum clearance depends on the actual
           thickness of the line. */
        delta = half(thickness(q));
        clr = fraction_num_vgap(cur_style);
        clr = ext_xn_over_d(clr, thickness(q), fraction_rule(cur_style));
        delta1 = clr - ((shift_up - depth(x)) - (math_axis(cur_size) + delta));
        if (delta1 > 0)
            shift_up = shift_up + delta1;
        clr = fraction_denom_vgap(cur_style);
        clr = ext_xn_over_d(clr, thickness(q), fraction_rule(cur_style));
        delta2 =
            clr - ((math_axis(cur_size) - delta) - (height(z) - shift_down));
        if (delta2 > 0)
            shift_down = shift_down + delta2;
    }
    /* Construct a vlist box for the fraction, according to |shift_up| and |shift_down| */
    v = new_null_box();
    type(v) = vlist_node;
    height(v) = shift_up + height(x);
    depth(v) = depth(z) + shift_down;
    width(v) = width(x);        /* this also equals |width(z)| */
    reset_attributes(v, node_attr(q));
    if (thickness(q) == 0) {
        p = new_kern((shift_up - depth(x)) - (height(z) - shift_down));
        vlink(p) = z;
    } else {
        y = do_fraction_rule(thickness(q), node_attr(q));
        p = new_kern((math_axis(cur_size) - delta) - (height(z) - shift_down));
        reset_attributes(p, node_attr(q));
        vlink(y) = p;
        vlink(p) = z;
        p = new_kern((shift_up - depth(x)) - (math_axis(cur_size) + delta));
        vlink(p) = y;
    }
    reset_attributes(p, node_attr(q));
    vlink(x) = p;
    list_ptr(v) = x;
    /* Put the fraction into a box with its delimiters, and make |new_hlist(q)|
       point to it */
    delta = fraction_del_size(cur_style);
    x = var_delimiter(left_delimiter(q), cur_size, delta);
    left_delimiter(q) = null;
    vlink(x) = v;
    z = var_delimiter(right_delimiter(q), cur_size, delta);
    right_delimiter(q) = null;
    vlink(v) = z;
    y = hpack(x, 0, additional);
    reset_attributes(y, node_attr(q));
    assign_new_hlist(q, y);
}


/*
If the nucleus of an |op_noad| is a single character, it is to be
centered vertically with respect to the axis, after first being enlarged
(via a character list in the font) if we are in display style.  The normal
convention for placing displayed limits is to put them above and below the
operator in display style.

The italic correction is removed from the character if there is a subscript
and the limits are not being displayed. The |make_op|
routine returns the value that should be used as an offset between
subscript and superscript.

After |make_op| has acted, |subtype(q)| will be |limits| if and only if
the limits have been set above and below the operator. In that case,
|new_hlist(q)| will already contain the desired final box.
*/

scaled make_op(pointer q)
{
    scaled delta;               /* offset between subscript and superscript */
    pointer p, v, x, y, z;      /* temporary registers for box construction */
    integer c;                  /* register for character examination */
    scaled shift_up, shift_down;        /* dimensions for box calculation */
    scaled ok_size;
    if ((subtype(q) == normal) && (cur_style < text_style))
        subtype(q) = limits;
    if (type(nucleus(q)) == math_char_node) {
        fetch(nucleus(q));
        if (cur_style < text_style) {   /* try to make it larger */
            if (minimum_operator_size(cur_style) != undefined_math_parameter)
                ok_size = minimum_operator_size(cur_style);
            else
                ok_size = height_plus_depth(cur_f, cur_c) + 1;
            while ((char_tag(cur_f, cur_c) == list_tag) &&
                   height_plus_depth(cur_f, cur_c) < ok_size) {
                c = char_remainder(cur_f, cur_c);
                if (!char_exists(cur_f, c))
                    break;
                cur_c = c;
                math_character(nucleus(q)) = c;
            }
        }
        delta = char_italic(cur_f, cur_c);
        x = clean_box(nucleus(q), cur_style);
        if ((subscr(q) != null) && (subtype(q) != limits))
            width(x) = width(x) - delta;        /* remove italic correction */
        shift_amount(x) = half(height(x) - depth(x)) - math_axis(cur_size);
        /* center vertically */
        type(nucleus(q)) = sub_box_node;
        math_list(nucleus(q)) = x;
    } else {
        delta = 0;
    }
    if (subtype(q) == limits) {
        /* The following program builds a vlist box |v| for displayed limits. The
           width of the box is not affected by the fact that the limits may be skewed. */
        x = clean_box(supscr(q), sup_style(cur_style));
        y = clean_box(nucleus(q), cur_style);
        z = clean_box(subscr(q), sub_style(cur_style));
        v = new_null_box();
        reset_attributes(v, node_attr(q));
        type(v) = vlist_node;
        width(v) = width(y);
        if (width(x) > width(v))
            width(v) = width(x);
        if (width(z) > width(v))
            width(v) = width(z);
        x = rebox(x, width(v));
        y = rebox(y, width(v));
        z = rebox(z, width(v));
        shift_amount(x) = half(delta);
        shift_amount(z) = -shift_amount(x);
        height(v) = height(y);
        depth(v) = depth(y);
        /* Attach the limits to |y| and adjust |height(v)|, |depth(v)| to
           account for their presence */
        /* We use |shift_up| and |shift_down| in the following program for the
           amount of glue between the displayed operator |y| and its limits |x| and
           |z|. The vlist inside box |v| will consist of |x| followed by |y| followed
           by |z|, with kern nodes for the spaces between and around them. */
        if (supscr(q) == null) {
            list_ptr(x) = null;
            flush_node(x);
            list_ptr(v) = y;
        } else {
            shift_up = limit_above_bgap(cur_style) - depth(x);
            if (shift_up < limit_above_vgap(cur_style))
                shift_up = limit_above_vgap(cur_style);
            p = new_kern(shift_up);
            reset_attributes(p, node_attr(q));
            vlink(p) = y;
            vlink(x) = p;
            p = new_kern(limit_above_kern(cur_style));
            reset_attributes(p, node_attr(q));
            vlink(p) = x;
            list_ptr(v) = p;
            height(v) =
                height(v) + limit_above_kern(cur_style) + height(x) + depth(x) +
                shift_up;
        }
        if (subscr(q) == null) {
            list_ptr(z) = null;
            flush_node(z);
        } else {
            shift_down = limit_below_bgap(cur_style) - height(z);
            if (shift_down < limit_below_vgap(cur_style))
                shift_down = limit_below_vgap(cur_style);
            p = new_kern(shift_down);
            reset_attributes(p, node_attr(q));
            vlink(y) = p;
            vlink(p) = z;
            p = new_kern(limit_below_kern(cur_style));
            reset_attributes(p, node_attr(q));
            vlink(z) = p;
            depth(v) =
                depth(v) + limit_below_kern(cur_style) + height(z) + depth(z) +
                shift_down;
        }
        if (subscr(q) != null) {
            math_list(subscr(q)) = null;
            flush_node(subscr(q));
            subscr(q) = null;
        }
        if (supscr(q) != null) {
            math_list(supscr(q)) = null;
            flush_node(supscr(q));
            supscr(q) = null;
        }
        assign_new_hlist(q, v);
    }
    return delta;
}

/*
A ligature found in a math formula does not create a ligature, because
there is no question of hyphenation afterwards; the ligature will simply be
stored in an ordinary |glyph_node|, after residing in an |ord_noad|.

The |type| is converted to |math_text_char| here if we would not want to
apply an italic correction to the current character unless it belongs
to a math font (i.e., a font with |space=0|).

No boundary characters enter into these ligatures.
*/

void make_ord(pointer q)
{
    integer a;                  /* the left-side character for lig/kern testing */
    pointer p, r, s;            /* temporary registers for list manipulation */
    scaled k;                   /* a kern */
    liginfo lig;                /* a ligature */
  RESTART:
    if (subscr(q) == null &&
        supscr(q) == null && type(nucleus(q)) == math_char_node) {
        p = vlink(q);
        if ((p != null) &&
            (type(p) >= ord_noad) && (type(p) <= punct_noad) &&
            (type(nucleus(p)) == math_char_node) &&
            (math_fam(nucleus(p)) == math_fam(nucleus(q)))) {
            type(nucleus(q)) = math_text_char_node;
            fetch(nucleus(q));
            a = cur_c;
            if ((has_kern(cur_f, a)) || (has_lig(cur_f, a))) {
                cur_c = math_character(nucleus(p));
                /* If character |a| has a kern with |cur_c|, attach
                   the kern after~|q|; or if it has a ligature with |cur_c|, combine
                   noads |q| and~|p| appropriately; then |return| if the cursor has
                   moved past a noad, or |goto restart| */

                /* Note that a ligature between an |ord_noad| and another kind of noad
                   is replaced by an |ord_noad|, when the two noads collapse into one.
                   But we could make a parenthesis (say) change shape when it follows
                   certain letters. Presumably a font designer will define such
                   ligatures only when this convention makes sense. */

                if (disable_lig == 0 && has_lig(cur_f, a)) {
                    lig = get_ligature(cur_f, a, cur_c);
                    if (is_valid_ligature(lig)) {
                        check_interrupt();      /* allow a way out of infinite ligature loop */
                        switch (lig_type(lig)) {
                        case 1:
                        case 5:
                            math_character(nucleus(q)) = lig_replacement(lig);  /* \.{=:|}, \.{=:|>} */
                            break;
                        case 2:
                        case 6:
                            math_character(nucleus(p)) = lig_replacement(lig);  /* \.{|=:}, \.{|=:>} */
                            break;
                        case 3:
                        case 7:
                        case 11:
                            r = new_noad();     /* \.{|=:|}, \.{|=:|>}, \.{|=:|>>} */
                            reset_attributes(r, node_attr(q));
                            s = new_node(math_char_node, 0);
                            reset_attributes(s, node_attr(q));
                            nucleus(r) = s;
                            math_character(nucleus(r)) = lig_replacement(lig);
                            math_fam(nucleus(r)) = math_fam(nucleus(q));
                            vlink(q) = r;
                            vlink(r) = p;
                            if (lig_type(lig) < 11)
                                type(nucleus(r)) = math_char_node;
                            else
                                type(nucleus(r)) = math_text_char_node; /* prevent combination */
                            break;
                        default:
                            vlink(q) = vlink(p);
                            math_character(nucleus(q)) = lig_replacement(lig);  /* \.{=:} */
                            s = math_clone(subscr(p));
                            subscr(q) = s;
                            s = math_clone(supscr(p));
                            supscr(q) = s;
                            math_reset(subscr(p));      /* just in case */
                            math_reset(supscr(p));
                            flush_node(p);
                            break;
                        }
                        if (lig_type(lig) > 3)
                            return;
                        type(nucleus(q)) = math_char_node;
                        goto RESTART;
                    }
                }
                if (disable_kern == 0 && has_kern(cur_f, a)) {
                    k = get_kern(cur_f, a, cur_c);
                    if (k != 0) {
                        p = new_kern(k);
                        reset_attributes(p, node_attr(q));
                        vlink(p) = vlink(q);
                        vlink(q) = p;
                        return;
                    }
                }
            }
        }
    }
}


/*
The purpose of |make_scripts(q,delta)| is to attach the subscript and/or
superscript of noad |q| to the list that starts at |new_hlist(q)|,
given that subscript and superscript aren't both empty. The superscript
will appear to the right of the subscript by a given distance |delta|.

We set |shift_down| and |shift_up| to the minimum amounts to shift the
baseline of subscripts and superscripts based on the given nucleus.
*/

void make_scripts(pointer q, scaled delta)
{
    pointer p, x, y, z;         /* temporary registers for box construction */
    scaled shift_up, shift_down, clr;   /* dimensions in the calculation */
    p = new_hlist(q);
    if (is_char_node(p)) {
        shift_up = 0;
        shift_down = 0;
    } else {
        z = hpack(p, 0, additional);
        shift_up = height(z) - sup_shift_drop(cur_style);       /* r18 */
        shift_down = depth(z) + sub_shift_drop(cur_style);      /* r19 */
        list_ptr(z) = null;
        flush_node(z);
    }
    if (supscr(q) == null) {
        /* Construct a subscript box |x| when there is no superscript */
        /* When there is a subscript without a superscript, the top of the subscript
           should not exceed the baseline plus four-fifths of the x-height. */
        x = clean_box(subscr(q), sub_style(cur_style));
        width(x) = width(x) + space_after_script(cur_style);
        if (shift_down < sub_shift_down(cur_style))
            shift_down = sub_shift_down(cur_style);
        clr = height(x) - sub_top_max(cur_style);
        if (shift_down < clr)
            shift_down = clr;
        shift_amount(x) = shift_down;
    } else {
        /* Construct a superscript box |x| */
        /*The bottom of a superscript should never descend below the baseline plus
           one-fourth of the x-height. */
        x = clean_box(supscr(q), sup_style(cur_style));
        width(x) = width(x) + space_after_script(cur_style);
        clr = sup_shift_up(cur_style);
        if (shift_up < clr)
            shift_up = clr;
        clr = depth(x) + sup_bottom_min(cur_style);
        if (shift_up < clr)
            shift_up = clr;

        if (subscr(q) == null) {
            shift_amount(x) = -shift_up;
        } else {
            /* Construct a sub/superscript combination box |x|, with the
               superscript offset by |delta| */
            /* When both subscript and superscript are present, the subscript must be
               separated from the superscript by at least four times |default_rule_thickness|.
               If this condition would be violated, the subscript moves down, after which
               both subscript and superscript move up so that the bottom of the superscript
               is at least as high as the baseline plus four-fifths of the x-height. */

            y = clean_box(subscr(q), sub_style(cur_style));
            width(y) = width(y) + space_after_script(cur_style);
            if (shift_down < sub_sup_shift_down(cur_style))
                shift_down = sub_sup_shift_down(cur_style);
            clr = subsup_vgap(cur_style) -
                ((shift_up - depth(x)) - (height(y) - shift_down));
            if (clr > 0) {
                shift_down = shift_down + clr;
                clr = sup_sub_bottom_max(cur_style) - (shift_up - depth(x));
                if (clr > 0) {
                    shift_up = shift_up + clr;
                    shift_down = shift_down - clr;
                }
            }
            shift_amount(x) = delta;    /* superscript is |delta| to the right of the subscript */
            p = new_kern((shift_up - depth(x)) - (height(y) - shift_down));
            reset_attributes(p, node_attr(q));
            vlink(x) = p;
            vlink(p) = y;
            pack_direction = math_direction;
            x = vpackage(x, 0, additional, max_dimen);
            reset_attributes(x, node_attr(q));
            shift_amount(x) = shift_down;
        }
    }
    if (new_hlist(q) == null) {
        new_hlist(q) = x;
    } else {
        p = new_hlist(q);
        while (vlink(p) != null)
            p = vlink(p);
        vlink(p) = x;
    }
    if (subscr(q) != null) {
        math_list(subscr(q)) = null;
        flush_node(subscr(q));
        subscr(q) = null;
    }
    if (supscr(q) != null) {
        math_list(supscr(q)) = null;
        flush_node(supscr(q));
        supscr(q) = null;
    }

}

/* 
The |make_left_right| function constructs a left or right delimiter of
the required size and returns the value |open_noad| or |close_noad|. The
|left_noad_side| and |right_noad_side| will both be based on the original |style|,
so they will have consistent sizes.
*/

small_number make_left_right(pointer q, integer style, scaled max_d,
                             scaled max_hv)
{
    scaled delta, delta1, delta2;       /* dimensions used in the calculation */
    pointer tmp;
    cur_style = style;
    setup_cur_size_and_mu();
    delta2 = max_d + math_axis(cur_size);
    delta1 = max_hv + max_d - delta2;
    if (delta2 > delta1)
        delta1 = delta2;        /* |delta1| is max distance from axis */
    delta = (delta1 / 500) * delimiter_factor;
    delta2 = delta1 + delta1 - delimiter_shortfall;
    if (delta < delta2)
        delta = delta2;
    tmp = var_delimiter(delimiter(q), cur_size, delta);
    delimiter(q) = null;
    assign_new_hlist(q, tmp);
    if (subtype(q) == left_noad_side)
        return open_noad;
    else
        return close_noad;
}


/*
The inter-element spacing in math formulas depends on a $8\times8$ table that
\TeX\ preloads as a 64-digit string. The elements of this string have the
following significance:
$$\vbox{\halign{#\hfil\cr
\.0 means no space;\cr
\.1 means a conditional thin space (\.{\\nonscript\\mskip\\thinmuskip});\cr
\.2 means a thin space (\.{\\mskip\\thinmuskip});\cr
\.3 means a conditional medium space
  (\.{\\nonscript\\mskip\\medmuskip});\cr
\.4 means a conditional thick space
  (\.{\\nonscript\\mskip\\thickmuskip});\cr
\.* means an impossible case.\cr}}$$
This is all pretty cryptic, but {\sl The \TeX book\/} explains what is
supposed to happen, and the string makes it happen.
@:TeXbook}{\sl The \TeX book@>
*/

char math_spacing[] =
    "0234000122*4000133**3**344*0400400*000000234000111*1111112341011";

#define level_one 1
#define thin_mu_skip param_thin_mu_skip_code
#define med_mu_skip param_med_mu_skip_code
#define thick_mu_skip param_thick_mu_skip_code


#define TEXT_STYLES(A,B) do {					\
    def_math_param(A,display_style,(B),level_one);		\
    def_math_param(A,cramped_display_style,(B),level_one);	\
    def_math_param(A,text_style,(B),level_one);			\
    def_math_param(A,cramped_text_style,(B),level_one);		\
  } while (0)

#define SCRIPT_STYLES(A,B) do {						\
    def_math_param(A,script_style,(B),level_one);			\
    def_math_param(A,cramped_script_style,(B),level_one);		\
    def_math_param(A,script_script_style,(B),level_one);		\
    def_math_param(A,cramped_script_script_style,(B),level_one);	\
  } while (0)

#define ALL_STYLES(A,B) do {			\
    TEXT_STYLES(A,(B));				\
    SCRIPT_STYLES(A,(B));			\
  } while (0)

#define SPLIT_STYLES(A,B,C) do {		\
    TEXT_STYLES(A,(B));				\
    SCRIPT_STYLES(A,(C));			\
  } while (0)


void initialize_math_spacing(void)
{
    /* *INDENT-OFF* */
    ALL_STYLES   (math_param_ord_ord_spacing,     0);
    ALL_STYLES   (math_param_ord_op_spacing,      thin_mu_skip);
    SPLIT_STYLES (math_param_ord_bin_spacing,     med_mu_skip, 0);
    SPLIT_STYLES (math_param_ord_rel_spacing,     thick_mu_skip, 0);
    ALL_STYLES   (math_param_ord_open_spacing,    0);
    ALL_STYLES   (math_param_ord_close_spacing,   0);
    ALL_STYLES   (math_param_ord_punct_spacing,   0);
    SPLIT_STYLES (math_param_ord_inner_spacing,   thin_mu_skip, 0);

    ALL_STYLES   (math_param_op_ord_spacing,      thin_mu_skip);
    ALL_STYLES   (math_param_op_op_spacing,       thin_mu_skip);
    ALL_STYLES   (math_param_op_bin_spacing,      0);
    SPLIT_STYLES (math_param_op_rel_spacing,      thick_mu_skip, 0);
    ALL_STYLES   (math_param_op_open_spacing,     0);
    ALL_STYLES   (math_param_op_close_spacing,    0);
    ALL_STYLES   (math_param_op_punct_spacing,    0);
    SPLIT_STYLES (math_param_op_inner_spacing,    thin_mu_skip, 0);

    SPLIT_STYLES (math_param_bin_ord_spacing,     med_mu_skip, 0);
    SPLIT_STYLES (math_param_bin_op_spacing,      med_mu_skip, 0);
    ALL_STYLES   (math_param_bin_bin_spacing,     0);
    ALL_STYLES   (math_param_bin_rel_spacing,     0);
    SPLIT_STYLES (math_param_bin_open_spacing,    med_mu_skip, 0);
    ALL_STYLES   (math_param_bin_close_spacing,   0);
    ALL_STYLES   (math_param_bin_punct_spacing,   0);
    SPLIT_STYLES (math_param_bin_inner_spacing,   med_mu_skip, 0);

    SPLIT_STYLES (math_param_rel_ord_spacing,     thick_mu_skip, 0);
    SPLIT_STYLES (math_param_rel_op_spacing,      thick_mu_skip, 0);
    ALL_STYLES   (math_param_rel_bin_spacing,     0);
    ALL_STYLES   (math_param_rel_rel_spacing,     0);
    SPLIT_STYLES (math_param_rel_open_spacing,    thick_mu_skip, 0);
    ALL_STYLES   (math_param_rel_close_spacing,   0);
    ALL_STYLES   (math_param_rel_punct_spacing,   0);
    SPLIT_STYLES (math_param_rel_inner_spacing,   thick_mu_skip, 0);

    ALL_STYLES   (math_param_open_ord_spacing,    0);
    ALL_STYLES   (math_param_open_op_spacing,     0);
    ALL_STYLES   (math_param_open_bin_spacing,    0);
    ALL_STYLES   (math_param_open_rel_spacing,    0);
    ALL_STYLES   (math_param_open_open_spacing,   0);
    ALL_STYLES   (math_param_open_close_spacing,  0);
    ALL_STYLES   (math_param_open_punct_spacing,  0);
    ALL_STYLES   (math_param_open_inner_spacing,  0);

    ALL_STYLES   (math_param_close_ord_spacing,   0);
    ALL_STYLES   (math_param_close_op_spacing,    thin_mu_skip);
    SPLIT_STYLES (math_param_close_bin_spacing,   med_mu_skip, 0);
    SPLIT_STYLES (math_param_close_rel_spacing,   thick_mu_skip, 0);
    ALL_STYLES   (math_param_close_open_spacing,  0);
    ALL_STYLES   (math_param_close_close_spacing, 0);
    ALL_STYLES   (math_param_close_punct_spacing, 0);
    SPLIT_STYLES (math_param_close_inner_spacing, thin_mu_skip, 0);

    SPLIT_STYLES (math_param_punct_ord_spacing,   thin_mu_skip, 0);
    SPLIT_STYLES (math_param_punct_op_spacing,    thin_mu_skip, 0);
    ALL_STYLES   (math_param_punct_bin_spacing,   0);
    SPLIT_STYLES (math_param_punct_rel_spacing,   thin_mu_skip, 0);
    SPLIT_STYLES (math_param_punct_open_spacing,  thin_mu_skip, 0);
    SPLIT_STYLES (math_param_punct_close_spacing, thin_mu_skip, 0);
    SPLIT_STYLES (math_param_punct_punct_spacing, thin_mu_skip, 0);
    SPLIT_STYLES (math_param_punct_inner_spacing, thin_mu_skip, 0);

    SPLIT_STYLES (math_param_inner_ord_spacing,   thin_mu_skip, 0);
    ALL_STYLES   (math_param_inner_op_spacing,    thin_mu_skip);
    SPLIT_STYLES (math_param_inner_bin_spacing,   med_mu_skip, 0);
    SPLIT_STYLES (math_param_inner_rel_spacing,   thick_mu_skip, 0);
    SPLIT_STYLES (math_param_inner_open_spacing,  thin_mu_skip, 0);
    ALL_STYLES   (math_param_inner_close_spacing, 0);
    SPLIT_STYLES (math_param_inner_punct_spacing, thin_mu_skip, 0);
    SPLIT_STYLES (math_param_inner_inner_spacing, thin_mu_skip, 0);
    /* *INDENT-ON* */
}


pointer math_spacing_glue_old(int l_type, int r_type, int m_style)
{
    pointer y, z;
    int x;
    z = null;
    switch (math_spacing[l_type * 8 + r_type - 9 * ord_noad]) {
    case '0':
        x = 0;
        break;
    case '1':
        x = (m_style < script_style ? param_thin_mu_skip_code : 0);
        break;
    case '2':
        x = param_thin_mu_skip_code;
        break;
    case '3':
        x = (m_style < script_style ? param_med_mu_skip_code : 0);
        break;
    case '4':
        x = (m_style < script_style ? param_thick_mu_skip_code : 0);
        break;
    default:
        tconfusion("mlist4");   /* this can't happen mlist4 */
        x = -1;
        break;
    }
    if (x != 0) {
        y = math_glue(glue_par(x), cur_mu);
        z = new_glue(y);
        glue_ref_count(y) = null;
        subtype(z) = x + 1;     /* store a symbolic subtype */
    }
    return z;
}

#define both_types(A,B) ((A)*8+B)

pointer math_spacing_glue(int l_type, int r_type, int m_style)
{
    int x = -1;
    pointer z = null;
    switch (both_types(l_type, r_type)) {
    /* *INDENT-OFF* */
    case both_types(ord_noad,  ord_noad  ):  x = get_math_param(math_param_ord_ord_spacing,m_style); break;
    case both_types(ord_noad,  op_noad   ):  x = get_math_param(math_param_ord_op_spacing,m_style); break;
    case both_types(ord_noad,  bin_noad  ):  x = get_math_param(math_param_ord_bin_spacing,m_style); break;
    case both_types(ord_noad,  rel_noad  ):  x = get_math_param(math_param_ord_rel_spacing,m_style); break;
    case both_types(ord_noad,  open_noad ):  x = get_math_param(math_param_ord_open_spacing,m_style); break;
    case both_types(ord_noad,  close_noad):  x = get_math_param(math_param_ord_close_spacing,m_style); break;
    case both_types(ord_noad,  punct_noad):  x = get_math_param(math_param_ord_punct_spacing,m_style); break;
    case both_types(ord_noad,  inner_noad):  x = get_math_param(math_param_ord_inner_spacing,m_style); break;
    case both_types(op_noad,   ord_noad  ):  x = get_math_param(math_param_op_ord_spacing,m_style); break;
    case both_types(op_noad,   op_noad   ):  x = get_math_param(math_param_op_op_spacing,m_style); break;
      /*case both_types(op_noad,   bin_noad  ):  x = get_math_param(math_param_op_bin_spacing,m_style); break;*/
    case both_types(op_noad,   rel_noad  ):  x = get_math_param(math_param_op_rel_spacing,m_style); break;
    case both_types(op_noad,   open_noad ):  x = get_math_param(math_param_op_open_spacing,m_style); break;
    case both_types(op_noad,   close_noad):  x = get_math_param(math_param_op_close_spacing,m_style); break;
    case both_types(op_noad,   punct_noad):  x = get_math_param(math_param_op_punct_spacing,m_style); break;
    case both_types(op_noad,   inner_noad):  x = get_math_param(math_param_op_inner_spacing,m_style); break;
    case both_types(bin_noad,  ord_noad  ):  x = get_math_param(math_param_bin_ord_spacing,m_style); break;
    case both_types(bin_noad,  op_noad   ):  x = get_math_param(math_param_bin_op_spacing,m_style); break;
      /*case both_types(bin_noad,  bin_noad  ):  x = get_math_param(math_param_bin_bin_spacing,m_style); break;*/
      /*case both_types(bin_noad,  rel_noad  ):  x = get_math_param(math_param_bin_rel_spacing,m_style); break;*/
    case both_types(bin_noad,  open_noad ):  x = get_math_param(math_param_bin_open_spacing,m_style); break;
      /*case both_types(bin_noad,  close_noad):  x = get_math_param(math_param_bin_close_spacing,m_style); break;*/
      /*case both_types(bin_noad,  punct_noad):  x = get_math_param(math_param_bin_punct_spacing,m_style); break;*/
    case both_types(bin_noad,  inner_noad):  x = get_math_param(math_param_bin_inner_spacing,m_style); break;
    case both_types(rel_noad,  ord_noad  ):  x = get_math_param(math_param_rel_ord_spacing,m_style); break;
    case both_types(rel_noad,  op_noad   ):  x = get_math_param(math_param_rel_op_spacing,m_style); break;
      /*case both_types(rel_noad,  bin_noad  ):  x = get_math_param(math_param_rel_bin_spacing,m_style); break;*/
    case both_types(rel_noad,  rel_noad  ):  x = get_math_param(math_param_rel_rel_spacing,m_style); break;
    case both_types(rel_noad,  open_noad ):  x = get_math_param(math_param_rel_open_spacing,m_style); break;
    case both_types(rel_noad,  close_noad):  x = get_math_param(math_param_rel_close_spacing,m_style); break;
    case both_types(rel_noad,  punct_noad):  x = get_math_param(math_param_rel_punct_spacing,m_style); break;
    case both_types(rel_noad,  inner_noad):  x = get_math_param(math_param_rel_inner_spacing,m_style); break;
    case both_types(open_noad, ord_noad  ):  x = get_math_param(math_param_open_ord_spacing,m_style); break;
    case both_types(open_noad, op_noad   ):  x = get_math_param(math_param_open_op_spacing,m_style); break;
      /*case both_types(open_noad, bin_noad  ):  x = get_math_param(math_param_open_bin_spacing,m_style); break;*/
    case both_types(open_noad, rel_noad  ):  x = get_math_param(math_param_open_rel_spacing,m_style); break;
    case both_types(open_noad, open_noad ):  x = get_math_param(math_param_open_open_spacing,m_style); break;
    case both_types(open_noad, close_noad):  x = get_math_param(math_param_open_close_spacing,m_style); break;
    case both_types(open_noad, punct_noad):  x = get_math_param(math_param_open_punct_spacing,m_style); break;
    case both_types(open_noad, inner_noad):  x = get_math_param(math_param_open_inner_spacing,m_style); break;
    case both_types(close_noad,ord_noad  ):  x = get_math_param(math_param_close_ord_spacing,m_style); break;
    case both_types(close_noad,op_noad   ):  x = get_math_param(math_param_close_op_spacing,m_style); break;
    case both_types(close_noad,bin_noad  ):  x = get_math_param(math_param_close_bin_spacing,m_style); break;
    case both_types(close_noad,rel_noad  ):  x = get_math_param(math_param_close_rel_spacing,m_style); break;
    case both_types(close_noad,open_noad ):  x = get_math_param(math_param_close_open_spacing,m_style); break;
    case both_types(close_noad,close_noad):  x = get_math_param(math_param_close_close_spacing,m_style); break;
    case both_types(close_noad,punct_noad):  x = get_math_param(math_param_close_punct_spacing,m_style); break;
    case both_types(close_noad,inner_noad):  x = get_math_param(math_param_close_inner_spacing,m_style); break;
    case both_types(punct_noad,ord_noad  ):  x = get_math_param(math_param_punct_ord_spacing,m_style); break;
    case both_types(punct_noad,op_noad   ):  x = get_math_param(math_param_punct_op_spacing,m_style); break;
      /*case both_types(punct_noad,bin_noad  ):  x = get_math_param(math_param_punct_bin_spacing,m_style); break;*/
    case both_types(punct_noad,rel_noad  ):  x = get_math_param(math_param_punct_rel_spacing,m_style); break;
    case both_types(punct_noad,open_noad ):  x = get_math_param(math_param_punct_open_spacing,m_style); break;
    case both_types(punct_noad,close_noad):  x = get_math_param(math_param_punct_close_spacing,m_style); break;
    case both_types(punct_noad,punct_noad):  x = get_math_param(math_param_punct_punct_spacing,m_style); break;
    case both_types(punct_noad,inner_noad):  x = get_math_param(math_param_punct_inner_spacing,m_style); break;
    case both_types(inner_noad,ord_noad  ):  x = get_math_param(math_param_inner_ord_spacing,m_style); break;
    case both_types(inner_noad,op_noad   ):  x = get_math_param(math_param_inner_op_spacing,m_style); break;
    case both_types(inner_noad,bin_noad  ):  x = get_math_param(math_param_inner_bin_spacing,m_style); break;
    case both_types(inner_noad,rel_noad  ):  x = get_math_param(math_param_inner_rel_spacing,m_style); break;
    case both_types(inner_noad,open_noad ):  x = get_math_param(math_param_inner_open_spacing,m_style); break;
    case both_types(inner_noad,close_noad):  x = get_math_param(math_param_inner_close_spacing,m_style); break;
    case both_types(inner_noad,punct_noad):  x = get_math_param(math_param_inner_punct_spacing,m_style); break;
    case both_types(inner_noad,inner_noad):  x = get_math_param(math_param_inner_inner_spacing,m_style); break;
    /* *INDENT-ON* */
    }
    if (x < 0) {
        tconfusion("mathspacing");
    }
    if (x != 0) {
        pointer y;
        if (x <= thick_mu_skip) {       /* trap thin/med/thick settings cf. old TeX */
            y = math_glue(glue_par(x), cur_mu);
            z = new_glue(y);
            glue_ref_count(y) = null;
            subtype(z) = x + 1; /* store a symbolic subtype */
        } else {
            y = math_glue(x, cur_mu);
            z = new_glue(y);
            glue_ref_count(y) = null;
        }
    }
    return z;
}



/* Here is the overall plan of |mlist_to_hlist|, and the list of its
   local variables.
*/

void mlist_to_hlist(void)
{
    pointer mlist;              /* beginning of the given list */
    boolean penalties;          /* should penalty nodes be inserted? */
    integer style;              /* the given style */
    integer save_style;         /* holds |cur_style| during recursion */
    pointer q;                  /* runs through the mlist */
    pointer r;                  /* the most recent noad preceding |q| */
    integer r_type;             /* the |type| of noad |r|, or |op_noad| if |r=null| */
    integer r_subtype;          /* the |subtype| of noad |r| if |r_type| is |fence_noad| */
    integer t;                  /* the effective |type| of noad |q| during the second pass */
    pointer p, x, y, z;         /* temporary registers for list construction */
    integer pen;                /* a penalty to be inserted */
    integer s;                  /* the size of a noad to be deleted */
    scaled max_hl, max_d;       /* maximum height and depth of the list translated so far */
    scaled delta;               /* offset between subscript and superscript */
    mlist = cur_mlist;
    penalties = mlist_penalties;
    style = cur_style;          /* tuck global parameters away as local variables */
    q = mlist;
    r = null;
    r_type = op_noad;
    r_subtype = 0;
    max_hl = 0;
    max_d = 0;
    x = null;
    p = null;
    setup_cur_size_and_mu();
    while (q != null) {
        /* We use the fact that no character nodes appear in an mlist, hence
           the field |type(q)| is always present. */

        /* One of the things we must do on the first pass is change a |bin_noad| to
           an |ord_noad| if the |bin_noad| is not in the context of a binary operator.
           The values of |r| and |r_type| make this fairly easy. */
      RESWITCH:
        delta = 0;
        switch (type(q)) {
        case bin_noad:
            switch (r_type) {
            case bin_noad:
            case op_noad:
            case rel_noad:
            case open_noad:
            case punct_noad:
                type(q) = ord_noad;
                goto RESWITCH;
                break;
            case fence_noad:
                if (r_subtype == left_noad_side) {
                    type(q) = ord_noad;
                    goto RESWITCH;
                }
            }
            break;
        case rel_noad:
        case close_noad:
        case punct_noad:
            if (r_type == bin_noad)
                type(r) = ord_noad;
            break;
        case fence_noad:
            if (subtype(q) != left_noad_side)
                if (r_type == bin_noad)
                    type(r) = ord_noad;
            goto DONE_WITH_NOAD;
            break;
        case fraction_noad:
            make_fraction(q);
            goto CHECK_DIMENSIONS;
            break;
        case op_noad:
            delta = make_op(q);
            if (subtype(q) == limits)
                goto CHECK_DIMENSIONS;
            break;
        case ord_noad:
            make_ord(q);
            break;
        case open_noad:
        case inner_noad:
            break;
        case radical_noad:
            if (subtype(q) == 4)
                make_under_delimiter(q);
            else if (subtype(q) == 5)
                make_over_delimiter(q);
            else if (subtype(q) == 6)
                make_delimiter_under(q);
            else if (subtype(q) == 7)
                make_delimiter_over(q);
            else
                make_radical(q);
            break;
        case over_noad:
            make_over(q);
            break;
        case under_noad:
            make_under(q);
            break;
        case accent_noad:
            make_math_accent(q);
            break;
        case vcenter_noad:
            make_vcenter(q);
            break;
        case style_node:
            cur_style = subtype(q);
            setup_cur_size_and_mu();
            goto DONE_WITH_NODE;
            break;
        case choice_node:
            switch (cur_style / 2) {
            case 0:
                choose_mlist(display_mlist);
                break;          /* |display_style=0| */
            case 1:
                choose_mlist(text_mlist);
                break;          /* |text_style=2| */
            case 2:
                choose_mlist(script_mlist);
                break;          /* |script_style=4| */
            case 3:
                choose_mlist(script_script_mlist);
                break;          /* |script_script_style=6| */
            }                   /* there are no other cases */
            flush_node_list(display_mlist(q));
            flush_node_list(text_mlist(q));
            flush_node_list(script_mlist(q));
            flush_node_list(script_script_mlist(q));
            type(q) = style_node;
            subtype(q) = cur_style;
            if (p != null) {
                z = vlink(q);
                vlink(q) = p;
                while (vlink(p) != null)
                    p = vlink(p);
                vlink(p) = z;
            }
            goto DONE_WITH_NODE;
            break;
        case ins_node:
        case mark_node:
        case adjust_node:
        case whatsit_node:
        case penalty_node:
        case disc_node:
            goto DONE_WITH_NODE;
            break;
        case rule_node:
            if (height(q) > max_hl)
                max_hl = height(q);
            if (depth(q) > max_d)
                max_d = depth(q);
            goto DONE_WITH_NODE;
            break;
        case glue_node:
            /*
               Conditional math glue (`\.{\\nonscript}') results in a |glue_node|
               pointing to |zero_glue|, with |subtype(q)=cond_math_glue|; in such a case
               the node following will be eliminated if it is a glue or kern node and if the
               current size is different from |text_size|. Unconditional math glue
               (`\.{\\muskip}') is converted to normal glue by multiplying the dimensions
               by |cur_mu|.
               @:non_script_}{\.{\\nonscript} primitive@>
             */
            if (subtype(q) == mu_glue) {
                x = glue_ptr(q);
                y = math_glue(x, cur_mu);
                delete_glue_ref(x);
                glue_ptr(q) = y;
                subtype(q) = normal;
            } else if ((cur_size != text_size)
                       && (subtype(q) == cond_math_glue)) {
                p = vlink(q);
                if (p != null)
                    if ((type(p) == glue_node) || (type(p) == kern_node)) {
                        vlink(q) = vlink(p);
                        vlink(p) = null;
                        flush_node_list(p);
                    }
            }
            goto DONE_WITH_NODE;
            break;
        case kern_node:
            math_kern(q, cur_mu);
            goto DONE_WITH_NODE;
            break;
        default:
            tconfusion("mlist1");       /* this can't happen mlist1 */
        }
        /* When we get to the following part of the program, we have ``fallen through''
           from cases that did not lead to |check_dimensions| or |done_with_noad| or
           |done_with_node|. Thus, |q|~points to a noad whose nucleus may need to be
           converted to an hlist, and whose subscripts and superscripts need to be
           appended if they are present.

           If |nucleus(q)| is not a |math_char|, the variable |delta| is the amount
           by which a superscript should be moved right with respect to a subscript
           when both are present.
           @^subscripts@>
           @^superscripts@>
         */
        switch (type(nucleus(q))) {
        case math_char_node:
        case math_text_char_node:
            fetch(nucleus(q));
            if (char_exists(cur_f, cur_c)) {
                delta = char_italic(cur_f, cur_c);
                p = new_glyph(cur_f, cur_c);
                reset_attributes(p, node_attr(nucleus(q)));
                if ((type(nucleus(q)) == math_text_char_node)
                    && (space(cur_f) != 0))
                    delta = 0;  /* no italic correction in mid-word of text font */
                if ((subscr(q) == null) && (delta != 0)) {
                    x = new_kern(delta);
                    reset_attributes(x, node_attr(nucleus(q)));
                    vlink(p) = x;
                    delta = 0;
                }
            } else {
                p = null;
            }
            break;
        case sub_box_node:
            p = math_list(nucleus(q));
            break;
        case sub_mlist_node:
            cur_mlist = math_list(nucleus(q));
            save_style = cur_style;
            mlist_penalties = false;
            mlist_to_hlist();   /* recursive call */
            cur_style = save_style;
            setup_cur_size_and_mu();
            p = hpack(vlink(temp_head), 0, additional);
            reset_attributes(p, node_attr(nucleus(q)));
            break;
        default:
            tconfusion("mlist2");       /* this can't happen mlist2 */
        }
        assign_new_hlist(q, p);
        if ((subscr(q) == null) && (supscr(q) == null))
            goto CHECK_DIMENSIONS;
        make_scripts(q, delta);
      CHECK_DIMENSIONS:
        z = hpack(new_hlist(q), 0, additional);
        if (height(z) > max_hl)
            max_hl = height(z);
        if (depth(z) > max_d)
            max_d = depth(z);
        list_ptr(z) = null;
        flush_node(z);          /* only drop the \hbox */
      DONE_WITH_NOAD:
        r = q;
        r_type = type(r);
        if (r_type == fence_noad) {
            r_subtype = left_noad_side;
            cur_style = style;
            setup_cur_size_and_mu();
        }
      DONE_WITH_NODE:
        q = vlink(q);
    }
    if (r_type == bin_noad)
        type(r) = ord_noad;
    /* Make a second pass over the mlist, removing all noads and inserting the
       proper spacing and penalties */

    /* We have now tied up all the loose ends of the first pass of |mlist_to_hlist|.
       The second pass simply goes through and hooks everything together with the
       proper glue and penalties. It also handles the |fence_noad|s that
       might be present, since |max_hl| and |max_d| are now known. Variable |p| points
       to a node at the current end of the final hlist.
     */
    p = temp_head;
    vlink(p) = null;
    q = mlist;
    r_type = 0;
    cur_style = style;
    setup_cur_size_and_mu();
  NEXT_NODE:
    while (q != null) {
        /* If node |q| is a style node, change the style and |goto delete_q|;
           otherwise if it is not a noad, put it into the hlist,
           advance |q|, and |goto done|; otherwise set |s| to the size
           of noad |q|, set |t| to the associated type (|ord_noad..
           inner_noad|), and set |pen| to the associated penalty */
        /* Just before doing the big |case| switch in the second pass, the program
           sets up default values so that most of the branches are short. */
        t = ord_noad;
        s = noad_size;
        pen = inf_penalty;
        switch (type(q)) {
        case op_noad:
        case open_noad:
        case close_noad:
        case punct_noad:
        case inner_noad:
            t = type(q);
            break;
        case bin_noad:
            t = bin_noad;
            pen = bin_op_penalty;
            break;
        case rel_noad:
            t = rel_noad;
            pen = rel_penalty;
            break;
        case ord_noad:
        case vcenter_noad:
        case over_noad:
        case under_noad:
            break;
        case radical_noad:
            s = radical_noad_size;
            break;
        case accent_noad:
            s = accent_noad_size;
            break;
        case fraction_noad:
            t = inner_noad;
            s = fraction_noad_size;
            break;
        case fence_noad:
            t = make_left_right(q, style, max_d, max_hl);
            break;
        case style_node:
            /* Change the current style and |goto delete_q| */
            cur_style = subtype(q);
            s = style_node_size;
            setup_cur_size_and_mu();
            goto DELETE_Q;
            break;
        case whatsit_node:
        case penalty_node:
        case rule_node:
        case disc_node:
        case adjust_node:
        case ins_node:
        case mark_node:
        case glue_node:
        case kern_node:
            vlink(p) = q;
            p = q;
            q = vlink(q);
            vlink(p) = null;
            goto NEXT_NODE;
            break;
        default:
            tconfusion("mlist3");       /* this can't happen mlist3 */
        }
        /* Append inter-element spacing based on |r_type| and |t| */
        if (r_type > 0) {       /* not the first noad */
            z = math_spacing_glue(r_type, t, cur_style);
            if (z != null) {
                reset_attributes(z, node_attr(p));
                vlink(p) = z;
                p = z;
            }
        }

        /* Append any |new_hlist| entries for |q|, and any appropriate penalties */
        /* We insert a penalty node after the hlist entries of noad |q| if |pen|
           is not an ``infinite'' penalty, and if the node immediately following |q|
           is not a penalty node or a |rel_noad| or absent entirely. */

        if (new_hlist(q) != null) {
            vlink(p) = new_hlist(q);
            do {
                p = vlink(p);
            } while (vlink(p) != null);
        }
        if (penalties && vlink(q) != null && pen < inf_penalty) {
            r_type = type(vlink(q));
            if (r_type != penalty_node && r_type != rel_noad) {
                z = new_penalty(pen);
                reset_attributes(z, node_attr(q));
                vlink(p) = z;
                p = z;
            }
        }
        if (type(q) == fence_noad && subtype(q) == right_noad_side)
            t = open_noad;
        r_type = t;
      DELETE_Q:
        r = q;
        q = vlink(q);
        /* The m-to-hlist conversion takes place in-place, 
           so the various dependant fields may not be freed 
           (as would happen if |flush_node| was called).
           A low-level |free_node| is easier than attempting
           to nullify such dependant fields for all possible
           node and noad types.
         */
        if (nodetype_has_attributes(type(r)))
            delete_attribute_ref(node_attr(r));
        free_node(r, get_node_size(type(r), subtype(r)));
    }
}
