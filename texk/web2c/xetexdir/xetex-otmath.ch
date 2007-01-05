% /****************************************************************************\
%  Part of the XeTeX typesetting system
%  copyright (c) 1994-2006 by SIL International
%  written by Jonathan Kew
% 
% Permission is hereby granted, free of charge, to any person obtaining  
% a copy of this software and associated documentation files (the  
% "Software"), to deal in the Software without restriction, including  
% without limitation the rights to use, copy, modify, merge, publish,  
% distribute, sublicense, and/or sell copies of the Software, and to  
% permit persons to whom the Software is furnished to do so, subject to  
% the following conditions:
%
% The above copyright notice and this permission notice shall be  
% included in all copies or substantial portions of the Software.
%
% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,  
% EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF  
% MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND  
% NONINFRINGEMENT. IN NO EVENT SHALL SIL INTERNATIONAL BE LIABLE FOR  
% ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF  
% CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION  
% WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
%
% Except as contained in this notice, the name of SIL International  
% shall not be used in advertising or otherwise to promote the sale,  
% use or other dealings in this Software without prior written  
% authorization from SIL International.
% \****************************************************************************/

-- redefine mathsy() and mathex() to go to the MATH table for OT fonts
@x
@d mathsy_end(#)==fam_fnt(2+#)]].sc
@d mathsy(#)==font_info[#+param_base[mathsy_end
@d math_x_height==mathsy(5) {height of `\.x'}
@d math_quad==mathsy(6) {\.{18mu}}
@d num1==mathsy(8) {numerator shift-up in display styles}
@d num2==mathsy(9) {numerator shift-up in non-display, non-\.{\\atop}}
@d num3==mathsy(10) {numerator shift-up in non-display \.{\\atop}}
@d denom1==mathsy(11) {denominator shift-down in display styles}
@d denom2==mathsy(12) {denominator shift-down in non-display styles}
@d sup1==mathsy(13) {superscript shift-up in uncramped display style}
@d sup2==mathsy(14) {superscript shift-up in uncramped non-display}
@d sup3==mathsy(15) {superscript shift-up in cramped styles}
@d sub1==mathsy(16) {subscript shift-down if superscript is absent}
@d sub2==mathsy(17) {subscript shift-down if superscript is present}
@d sup_drop==mathsy(18) {superscript baseline below top of large box}
@d sub_drop==mathsy(19) {subscript baseline below bottom of large box}
@d delim1==mathsy(20) {size of \.{\\atopwithdelims} delimiters
  in display styles}
@d delim2==mathsy(21) {size of \.{\\atopwithdelims} delimiters in non-displays}
@d axis_height==mathsy(22) {height of fraction lines above the baseline}
@d total_mathsy_params=22
@y
NB: the access functions here must all put the font # into /f/ for mathsy().

The accessors are defined with
define_mathsy_accessor(NAME)(fontdimen-number)(NAME)
because I can't see how to only give the name once, with WEB's limited
macro capabilities. This seems a bit ugly, but it works.

@d total_mathsy_params=22

{the following are OpenType MATH constant indices for use with OT math fonts}
@d	scriptPercentScaleDown = 0
@d	scriptScriptPercentScaleDown = 1
@d	delimitedSubFormulaMinHeight = 2
@d	displayOperatorMinHeight = 3
@d	mathLeading = 4
@d	firstMathValueRecord = mathLeading
@d	axisHeight = 5
@d	accentBaseHeight = 6
@d	flattenedAccentBaseHeight = 7
@d	subscriptShiftDown = 8
@d	subscriptTopMax = 9
@d	subscriptBaselineDropMin = 10
@d	superscriptShiftUp = 11
@d	superscriptShiftUpCramped = 12
@d	superscriptBottomMin = 13
@d	superscriptBaselineDropMax = 14
@d	subSuperscriptGapMin = 15
@d	superscriptBottomMaxWithSubscript = 16
@d	spaceAfterScript = 17
@d	upperLimitGapMin = 18
@d	upperLimitBaselineRiseMin = 19
@d	lowerLimitGapMin = 20
@d	lowerLimitBaselineDropMin = 21
@d	stackTopShiftUp = 22
@d	stackTopDisplayStyleShiftUp = 23
@d	stackBottomShiftDown = 24
@d	stackBottomDisplayStyleShiftDown = 25
@d	stackGapMin = 26
@d	stackDisplayStyleGapMin = 27
@d	stretchStackTopShiftUp = 28
@d	stretchStackBottomShiftDown = 29
@d	stretchStackGapAboveMin = 30
@d	stretchStackGapBelowMin = 31
@d	fractionNumeratorShiftUp = 32
@d	fractionNumeratorDisplayStyleShiftUp = 33
@d	fractionDenominatorShiftDown = 34
@d	fractionDenominatorDisplayStyleShiftDown = 35
@d	fractionNumeratorGapMin = 36
@d	fractionNumDisplayStyleGapMin = 37
@d	fractionRuleThickness = 38
@d	fractionDenominatorGapMin = 39
@d	fractionDenomDisplayStyleGapMin = 40
@d	skewedFractionHorizontalGap = 41
@d	skewedFractionVerticalGap = 42
@d	overbarVerticalGap = 43
@d	overbarRuleThickness = 44
@d	overbarExtraAscender = 45
@d	underbarVerticalGap = 46
@d	underbarRuleThickness = 47
@d	underbarExtraDescender = 48
@d	radicalVerticalGap = 49
@d	radicalDisplayStyleVerticalGap = 50
@d	radicalRuleThickness = 51
@d	radicalExtraAscender = 52
@d	radicalKernBeforeDegree = 53
@d	radicalKernAfterDegree = 54
@d	lastMathValueRecord = radicalKernAfterDegree
@d	radicalDegreeBottomRaisePercent = 55
@d	lastMathConstant = radicalDegreeBottomRaisePercent


@d mathsy(#)==font_info[#+param_base[f]].sc

@d define_mathsy_end(#)==
    # := rval;
  end
@d define_mathsy_body(#)==
  var
    f: integer;
    rval: scaled;
  begin
    f := fam_fnt(2 + size_code);
    if is_ot_font(f) then
      rval := get_native_mathsy_param(f, #)
    else
      rval := mathsy(#);
    define_mathsy_end
@d define_mathsy_accessor(#)==function #(size_code: integer): scaled; define_mathsy_body

@p define_mathsy_accessor(math_x_height)(5)(math_x_height);
define_mathsy_accessor(math_quad)(6)(math_quad);
define_mathsy_accessor(num1)(8)(num1);
define_mathsy_accessor(num2)(9)(num2);
define_mathsy_accessor(num3)(10)(num3);
define_mathsy_accessor(denom1)(11)(denom1);
define_mathsy_accessor(denom2)(12)(denom2);
define_mathsy_accessor(sup1)(13)(sup1);
define_mathsy_accessor(sup2)(14)(sup2);
define_mathsy_accessor(sup3)(15)(sup3);
define_mathsy_accessor(sub1)(16)(sub1);
define_mathsy_accessor(sub2)(17)(sub2);
define_mathsy_accessor(sup_drop)(18)(sup_drop);
define_mathsy_accessor(sub_drop)(19)(sub_drop);
define_mathsy_accessor(delim1)(20)(delim1);
define_mathsy_accessor(delim2)(21)(delim2);
define_mathsy_accessor(axis_height)(22)(axis_height);
@z

@x
@d mathex(#)==font_info[#+param_base[fam_fnt(3+cur_size)]].sc
@d default_rule_thickness==mathex(8) {thickness of \.{\\over} bars}
@d big_op_spacing1==mathex(9) {minimum clearance above a displayed op}
@d big_op_spacing2==mathex(10) {minimum clearance below a displayed op}
@d big_op_spacing3==mathex(11) {minimum baselineskip above displayed op}
@d big_op_spacing4==mathex(12) {minimum baselineskip below displayed op}
@d big_op_spacing5==mathex(13) {padding above and below displayed limits}
@d total_mathex_params=13
@y
@d total_mathex_params=13

@d mathex(#)==font_info[#+param_base[f]].sc

@d define_mathex_end(#)==
    # := rval;
  end
@d define_mathex_body(#)==
  var
    f: integer;
    rval: scaled;
  begin
    f := fam_fnt(3 + cur_size);
    if is_ot_font(f) then
      rval := get_native_mathex_param(f, #)
    else
      rval := mathex(#);
  define_mathex_end
@d define_mathex_accessor(#)==function #:scaled; define_mathex_body

@p define_mathex_accessor(default_rule_thickness)(8)(default_rule_thickness);
define_mathex_accessor(big_op_spacing1)(9)(big_op_spacing1);
define_mathex_accessor(big_op_spacing2)(10)(big_op_spacing2);
define_mathex_accessor(big_op_spacing3)(11)(big_op_spacing3);
define_mathex_accessor(big_op_spacing4)(12)(big_op_spacing4);
define_mathex_accessor(big_op_spacing5)(13)(big_op_spacing5);
@z

-- var_delimiter needs to use MathVariants
@x
function var_delimiter(@!d:pointer;@!s:integer;@!v:scaled):pointer;
label found,continue;
var b:pointer; {the box that will be constructed}
@y
procedure stack_glyph_into_box(@!b:pointer;@!f:internal_font_number;@!g:integer);
var p,q:pointer;
begin
  p:=get_node(glyph_node_size);
  type(p):=whatsit_node; subtype(p):=glyph_node;
  native_font(p):=f; native_glyph(p):=g;
  set_native_glyph_metrics(p, 1);
  link(p):=list_ptr(b); list_ptr(b):=p;
  height(b):=height(p); width(b):=width(p);
end;

procedure stack_glue_into_box(@!b:pointer;@!min,max:scaled);
var p,q:pointer;
begin
  q:=new_spec(zero_glue);
  width(q):=min;
  stretch(q):=max-min;
  p:=new_glue(q);
  link(p):=list_ptr(b); list_ptr(b):=p;
  height(b):=height(p); width(b):=width(p);
end;

function build_opentype_assembly(@!f:internal_font_number;@!a:void_pointer;@!h:scaled):pointer;
  {return a box with height at least |h|, using font |f|, with glyph assembly info from |a|}
var
  b:pointer; {the box we're constructing}
  n:integer; {the number of repetitions of each extender}
  i,j:integer; {indexes}
  g:integer; {glyph code}
  p:pointer; {temp pointer}
  h_max,o,oo,prev_o,min_o:scaled;
  no_extenders: boolean;
  nat,str:scaled; {natural height, stretch}
begin
  b:=new_null_box;
  type(b):=vlist_node;

  {figure out how many repeats of each extender to use}
  n:=-1;
  no_extenders:=true;
  min_o:=ot_min_connector_overlap(f);
  repeat
    n:=n+1;
    {calc max possible height with this number of extenders}
    h_max:=0;
    prev_o:=0;
    for i:=0 to ot_part_count(a)-1 do begin
      if ot_part_is_extender(a, i) then begin
        no_extenders:=false;
        for j:=1 to n do begin
          o:=ot_part_start_connector(f, a, i);
          if min_o<o then o:=min_o;
          if prev_o<o then o:=prev_o;
          h_max:=h_max-o+ot_part_full_advance(f, a, i);
          prev_o:=ot_part_end_connector(f, a, i);
        end
      end else begin
        o:=ot_part_start_connector(f, a, i);
        if min_o<o then o:=min_o;
        if prev_o<o then o:=prev_o;
        h_max:=h_max-o+ot_part_full_advance(f, a, i);
        prev_o:=ot_part_end_connector(f, a, i);
      end;
    end;
  until (h_max>=h) or no_extenders;

  {assemble box using |n| copies of each extender,
   with appropriate glue wherever an overlap occurs}
  prev_o:=0;
  for i:=0 to ot_part_count(a)-1 do begin
    if ot_part_is_extender(a, i) then begin
      for j:=1 to n do begin
        o:=ot_part_start_connector(f, a, i);
        if prev_o<o then o:=prev_o;
        oo:=o; {max overlap}
        if min_o<o then o:=min_o;
        if oo>0 then stack_glue_into_box(b, -oo, -o);
        g:=ot_part_glyph(a, i);
        stack_glyph_into_box(b, f, g);
        prev_o:=ot_part_end_connector(f, a, i);
      end
    end else begin
      o:=ot_part_start_connector(f, a, i);
      if prev_o<o then o:=prev_o;
      oo:=o; {max overlap}
      if min_o<o then o:=min_o;
      if oo>0 then stack_glue_into_box(b, -oo, -o);
      g:=ot_part_glyph(a, i);
      stack_glyph_into_box(b, f, g);
      prev_o:=ot_part_end_connector(f, a, i);
    end;
  end;

  {find natural height and total stretch of the box} 
  p:=list_ptr(b); nat:=0; str:=0;
  while p<>null do begin
    if type(p)=whatsit_node then nat:=nat+height(p)+depth(p)
    else if type(p)=glue_node then begin
      nat:=nat+width(glue_ptr(p));
      str:=str+stretch(glue_ptr(p));
    end;
    p:=link(p);
  end;

  {set glue so as to stretch the connections if needed}
  depth(b):=0;
  if (h>nat) and (str>0) then begin
    glue_order(b):=normal; glue_sign(b):=stretching;
    glue_set(b):=unfloat((h-nat)/str);
    height(b):=nat+round(str*float(glue_set(b)));
  end else height(b):=nat;

  build_opentype_assembly:=b;
end;

function var_delimiter(@!d:pointer;@!s:integer;@!v:scaled):pointer;
label found,continue;
var b:pointer; {the box that will be constructed}
ot_assembly_ptr:void_pointer;
@z

@x
loop@+  begin @<Look at the variants of |(z,x)|; set |f| and |c| whenever
    a better character is found; |goto found| as soon as a
    large enough variant is encountered@>;
  if large_attempt then goto found; {there were none large enough}
  large_attempt:=true; z:=large_fam(d); x:=large_char(d);
  end;
found: if f<>null_font then
  @<Make variable |b| point to a box for |(f,c)|@>
else  begin b:=new_null_box;
  width(b):=null_delimiter_space; {use this width if no delimiter was found}
  end;
@y
ot_assembly_ptr:=nil;
loop@+  begin @<Look at the variants of |(z,x)|; set |f| and |c| whenever
    a better character is found; |goto found| as soon as a
    large enough variant is encountered@>;
  if large_attempt then goto found; {there were none large enough}
  large_attempt:=true; z:=large_fam(d); x:=large_char(d);
  end;
found: if f<>null_font then begin
  if not is_ot_font(f) then
    @<Make variable |b| point to a box for |(f,c)|@>
  else begin
    {for OT fonts, c is the glyph ID to use}
    if ot_assembly_ptr<>nil then
      b:=build_opentype_assembly(f, ot_assembly_ptr, v)
    else begin
      b:=new_null_box; type(b):=vlist_node; list_ptr(b):=get_node(glyph_node_size);
      type(list_ptr(b)):=whatsit_node; subtype(list_ptr(b)):=glyph_node;
      native_font(list_ptr(b)):=f; native_glyph(list_ptr(b)):=c;
      set_native_glyph_metrics(list_ptr(b), 1);
      width(b):=width(list_ptr(b));
      height(b):=height(list_ptr(b));
      depth(b):=depth(list_ptr(b));
    end
  end
end else  begin b:=new_null_box;
  width(b):=null_delimiter_space; {use this width if no delimiter was found}
  end;
@z

@x
@ @<Look at the list of characters starting with |x|...@>=
@y
@ @<Look at the list of characters starting with |x|...@>=
if is_ot_font(g) then begin
  b:=new_native_character(g, x);
  x:=get_native_glyph(b, 0);
  free_node(b, native_size(b));
  f:=g; c:=x; w:=0; n:=0;
  repeat
    y:=get_ot_math_variant(g, x, n, address_of(u));
    if u>w then begin
      c:=y; w:=u;
      if u>=v then goto found;
    end;
    n:=n+1;
  until u<0;
  {if we get here, then we didn't find a big enough glyph; check if the char is extensible}
  ot_assembly_ptr:=get_ot_assembly_ptr(g, x);
  if ot_assembly_ptr<>nil then goto found;
end else
@z

-- get radical rule thickness etc from OT
@x
procedure make_radical(@!q:pointer);
var x,@!y:pointer; {temporary registers for box construction}
@!delta,@!clr:scaled; {dimensions involved in the calculation}
begin x:=clean_box(nucleus(q),cramped_style(cur_style));
if cur_style<text_style then {display style}
  clr:=default_rule_thickness+(abs(math_x_height(cur_size)) div 4)
else  begin clr:=default_rule_thickness; clr:=clr + (abs(clr) div 4);
  end;
y:=var_delimiter(left_delimiter(q),cur_size,height(x)+depth(x)+clr+
  default_rule_thickness);
@y
procedure make_radical(@!q:pointer);
var x,@!y:pointer; {temporary registers for box construction}
f:internal_font_number;
rule_thickness:scaled; {rule thickness}
@!delta,@!clr:scaled; {dimensions involved in the calculation}
begin f:=fam_fnt(small_fam(left_delimiter(q)) + cur_size);
if is_ot_font(f) then rule_thickness:=get_ot_math_constant(f,radicalRuleThickness)
else rule_thickness:=default_rule_thickness;
x:=clean_box(nucleus(q),cramped_style(cur_style));
if is_ot_font(f) then begin
  if cur_style<text_style then {display style}
    clr:=get_ot_math_constant(f,radicalDisplayStyleVerticalGap)
  else clr:=get_ot_math_constant(f,radicalVerticalGap);
end else begin
  if cur_style<text_style then {display style}
    clr:=rule_thickness+(abs(math_x_height(cur_size)) div 4)
  else  begin clr:=rule_thickness; clr:=clr + (abs(clr) div 4);
    end;
end;
y:=var_delimiter(left_delimiter(q),cur_size,height(x)+depth(x)+clr+rule_thickness);
if is_ot_font(f) then begin
  depth(y):=height(y)+depth(y)-rule_thickness;
  height(y):=rule_thickness;
end;
@z

-- use OT spacing parameters from extension font (fam3) rather than hard-coded values
@x
@<Adjust \(s)|shift_up| and |shift_down| for the case of no fraction line@>=
begin if cur_style<text_style then clr:=7*default_rule_thickness
else clr:=3*default_rule_thickness;
@y
@<Adjust \(s)|shift_up| and |shift_down| for the case of no fraction line@>=
begin if is_ot_font(fam_fnt(3+cur_size)) then begin
  if cur_style<text_style then clr:=get_ot_math_constant(fam_fnt(3+cur_size), stackDisplayStyleGapMin)
  else clr:=get_ot_math_constant(fam_fnt(3+cur_size), stackGapMin);
end else begin
  if cur_style<text_style then clr:=7*default_rule_thickness
  else clr:=3*default_rule_thickness;
end;
@z

@x
@<Adjust \(s)|shift_up| and |shift_down| for the case of a fraction line@>=
begin if cur_style<text_style then clr:=3*thickness(q)
else clr:=thickness(q);
delta:=half(thickness(q));
delta1:=clr-((shift_up-depth(x))-(axis_height(cur_size)+delta));
delta2:=clr-((axis_height(cur_size)-delta)-(height(z)-shift_down));
@y
@<Adjust \(s)|shift_up| and |shift_down| for the case of a fraction line@>=
begin if is_ot_font(fam_fnt(3+cur_size)) then begin
  delta:=half(thickness(q));
  if cur_style<text_style then clr:=get_ot_math_constant(fam_fnt(3+cur_size), fractionNumDisplayStyleGapMin)
  else clr:=get_ot_math_constant(fam_fnt(3+cur_size), fractionNumeratorGapMin);
  delta1:=clr-((shift_up-depth(x))-(axis_height(cur_size)+delta));
  if cur_style<text_style then clr:=get_ot_math_constant(fam_fnt(3+cur_size), fractionDenomDisplayStyleGapMin)
  else clr:=get_ot_math_constant(fam_fnt(3+cur_size), fractionDenominatorGapMin);
  delta2:=clr-((axis_height(cur_size)-delta)-(height(z)-shift_down));
end else begin
  if cur_style<text_style then clr:=3*thickness(q)
  else clr:=thickness(q);
  delta:=half(thickness(q));
  delta1:=clr-((shift_up-depth(x))-(axis_height(cur_size)+delta));
  delta2:=clr-((axis_height(cur_size)-delta)-(height(z)-shift_down));
end;
@z

-- make_op needs to use opentype variant tables to find display size glyphs
@x
function make_op(@!q:pointer):scaled;
var delta:scaled; {offset between subscript and superscript}
@!p,@!v,@!x,@!y,@!z:pointer; {temporary registers for box construction}
@!c:quarterword;@+@!i:four_quarters; {registers for character examination}
@!shift_up,@!shift_down:scaled; {dimensions for box calculation}
begin if (subtype(q)=normal)and(cur_style<text_style) then
  subtype(q):=limits;
if math_type(nucleus(q))=math_char then
  begin fetch(nucleus(q));
  if (cur_style<text_style)and(char_tag(cur_i)=list_tag) then {make it larger}
    begin c:=rem_byte(cur_i); i:=orig_char_info(cur_f)(c);
    if char_exists(i) then
      begin cur_c:=c; cur_i:=i; character(nucleus(q)):=c;
      end;
    end;
  delta:=char_italic(cur_f)(cur_i); x:=clean_box(nucleus(q),cur_style);
  if (math_type(subscr(q))<>empty)and(subtype(q)<>limits) then
    width(x):=width(x)-delta; {remove italic correction}
  shift_amount(x):=half(height(x)-depth(x)) - axis_height(cur_size);
    {center vertically}
  math_type(nucleus(q)):=sub_box; info(nucleus(q)):=x;
  end
else delta:=0;
if subtype(q)=limits then
  @<Construct a box with limits above and below it, skewed by |delta|@>;
make_op:=delta;
end;
@y
function make_op(@!q:pointer):scaled;
var delta:scaled; {offset between subscript and superscript}
@!p,@!v,@!x,@!y,@!z:pointer; {temporary registers for box construction}
@!c:quarterword;@+@!i:four_quarters; {registers for character examination}
@!shift_up,@!shift_down:scaled; {dimensions for box calculation}
@!h1,@!h2:scaled; {height of original text-style symbol and possible replacement}
@!n,@!g:integer; {potential variant index and glyph code}
begin if (subtype(q)=normal)and(cur_style<text_style) then
  subtype(q):=limits;
delta:=0;
if math_type(nucleus(q))=math_char then
  begin fetch(nucleus(q));
  if not is_ot_font(cur_f) then begin
    if (cur_style<text_style)and(char_tag(cur_i)=list_tag) then {make it larger}
      begin c:=rem_byte(cur_i); i:=orig_char_info(cur_f)(c);
      if char_exists(i) then
        begin cur_c:=c; cur_i:=i; character(nucleus(q)):=c;
        end;
      end;
    delta:=char_italic(cur_f)(cur_i);
  end;
  x:=clean_box(nucleus(q),cur_style);
  if is_ot_font(cur_f) then begin
    p:=list_ptr(x);
    if (type(p)=whatsit_node) and (subtype(p)=glyph_node) then begin
      if cur_style<text_style then begin
        {try to replace the operator glyph with a variant at least 25% taller}
        h1:=height(p)+depth(p);
        c:=native_glyph(p);
        n:=1;
        repeat
          g:=get_ot_math_variant(cur_f, c, n, address_of(h2));
          if h2>0 then native_glyph(p):=g;
          incr(n);
        until (h2<0) or (h2>(h1*5)/4);
        set_native_glyph_metrics(p, 1);
      end;
      width(x):=width(p); height(x):=height(p); depth(x):=depth(p);
      delta:=get_ot_math_ital_corr(cur_f, native_glyph(p));
    end
  end;
  if (math_type(subscr(q))<>empty)and(subtype(q)<>limits) then
    width(x):=width(x)-delta; {remove italic correction}
  shift_amount(x):=half(height(x)-depth(x)) - axis_height(cur_size);
    {center vertically}
  math_type(nucleus(q)):=sub_box; info(nucleus(q)):=x;
  end;
if subtype(q)=limits then
  @<Construct a box with limits above and below it, skewed by |delta|@>;
make_op:=delta;
end;
@z

-- make glyph nodes rather than native_word_nodes here
@x
@ @<Create a character node |p| for |nucleus(q)|...@>=
begin fetch(nucleus(q));
if is_native_font(cur_f) then begin
  delta:=0; p:=new_native_character(cur_f, qo(cur_c));
@y
@ @<Create a character node |p| for |nucleus(q)|...@>=
begin fetch(nucleus(q));
if is_native_font(cur_f) then begin
  z:=new_native_character(cur_f, qo(cur_c));
  p:=get_node(glyph_node_size);
  type(p):=whatsit_node; subtype(p):=glyph_node;
  native_font(p):=cur_f; native_glyph(p):=get_native_glyph(z, 0);
  set_native_glyph_metrics(p, 1);
  free_node(z, native_size(z));
  delta:=0; {FIXME}
@z

-- make_scripts needs to treat native_text like char_node, not box
@x
@ The purpose of |make_scripts(q,delta)| is to attach the subscript and/or
superscript of noad |q| to the list that starts at |new_hlist(q)|,
given that subscript and superscript aren't both empty. The superscript
will appear to the right of the subscript by a given distance |delta|.

We set |shift_down| and |shift_up| to the minimum amounts to shift the
baseline of subscripts and superscripts based on the given nucleus.

@<Declare math...@>=
procedure make_scripts(@!q:pointer;@!delta:scaled);
var p,@!x,@!y,@!z:pointer; {temporary registers for box construction}
@!shift_up,@!shift_down,@!clr:scaled; {dimensions in the calculation}
@!t:integer; {subsidiary size code}
begin p:=new_hlist(q);
if is_char_node(p) then
@y
@ The purpose of |make_scripts(q,delta)| is to attach the subscript and/or
superscript of noad |q| to the list that starts at |new_hlist(q)|,
given that subscript and superscript aren't both empty. The superscript
will appear to the right of the subscript by a given distance |delta|.

We set |shift_down| and |shift_up| to the minimum amounts to shift the
baseline of subscripts and superscripts based on the given nucleus.

@<Declare math...@>=
procedure make_scripts(@!q:pointer;@!delta:scaled);
var p,@!x,@!y,@!z:pointer; {temporary registers for box construction}
@!shift_up,@!shift_down,@!clr:scaled; {dimensions in the calculation}
@!t:integer; {subsidiary size code}
begin p:=new_hlist(q);
if is_char_node(p) or (p<>null and is_native_word_node(p)) then
@z

-- we allow either TFMs with lots of fontdimens or OpenType fonts
@x
@ @<Check that the necessary fonts...@>=
if (font_params[fam_fnt(2+text_size)]<total_mathsy_params)or@|
   (font_params[fam_fnt(2+script_size)]<total_mathsy_params)or@|
   (font_params[fam_fnt(2+script_script_size)]<total_mathsy_params) then
@y
@ @<Check that the necessary fonts...@>=
if ((font_params[fam_fnt(2+text_size)]<total_mathsy_params)
    and (not is_ot_font(fam_fnt(2+text_size)))) or@|
   ((font_params[fam_fnt(2+script_size)]<total_mathsy_params)
    and (not is_ot_font(fam_fnt(2+script_size)))) or@|
   ((font_params[fam_fnt(2+script_script_size)]<total_mathsy_params)
    and (not is_ot_font(fam_fnt(2+script_script_size)))) then
@z

@x
else if (font_params[fam_fnt(3+text_size)]<total_mathex_params)or@|
   (font_params[fam_fnt(3+script_size)]<total_mathex_params)or@|
   (font_params[fam_fnt(3+script_script_size)]<total_mathex_params) then
@y
else if ((font_params[fam_fnt(3+text_size)]<total_mathex_params)
    and (not is_ot_font(fam_fnt(3+text_size)))) or@|
   ((font_params[fam_fnt(3+script_size)]<total_mathex_params)
    and (not is_ot_font(fam_fnt(3+script_size)))) or@|
   ((font_params[fam_fnt(3+script_script_size)]<total_mathex_params)
    and (not is_ot_font(fam_fnt(3+script_script_size)))) then
@z

-- need to be able to output glyph nodes in a vlist (for extensible stacks)
@x
@ @<Output the whatsit node |p| in a vlist@>=
begin
	case subtype(p) of
@y
@ @<Output the whatsit node |p| in a vlist@>=
begin
	case subtype(p) of
	glyph_node: begin
		cur_v:=cur_v+height(p);
		cur_h:=left_edge;

		{ synch DVI state to TeX state }
		synch_h; synch_v;
		f := native_font(p);
		if f<>dvi_f then @<Change font |dvi_f| to |f|@>;

		dvi_out(set_glyph_string);
		dvi_four(0); { width }
		dvi_two(1); { glyph count }
		dvi_four(0); { x-offset as fixed point }
		dvi_two(native_glyph(p));

		cur_v:=cur_v+depth(p);
		cur_h:=left_edge;
	end;
@z
