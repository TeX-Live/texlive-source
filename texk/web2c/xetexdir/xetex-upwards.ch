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

{ This file implements the experimental feature \XeTeXupwardsmode.
  It also adds the state variable \XeTeXuseglyphmetrics, for use
  from xetex-new.ch }

@x
@d XeTeX_default_input_mode_code    = 2 {input mode for newly opened files}
@y
@d XeTeX_upwards_code = 2 {non-zero if the main vertical list is being built upwards}
@d XeTeX_use_glyph_metrics_code = 3 {non-zero to use exact glyph height/depth}
@#
@d XeTeX_default_input_mode_code    = 4 {input mode for newly opened files}
@z

@x
@d XeTeX_default_input_encoding_code = 3 {str_number of encoding name if mode = ICU}
@#
@d eTeX_states=4 {number of \eTeX\ state variables in |eqtb|}
@y
@d XeTeX_default_input_encoding_code = 5 {str_number of encoding name if mode = ICU}
@#
@d eTeX_states=6 {number of \eTeX\ state variables in |eqtb|}
@z

@x { to ensure following change doesn't get applied to hlist_out() }
@p procedure vlist_out; {output a |vlist_node| box}
@y
@p procedure vlist_out; {output a |vlist_node| box}
@z

@x
@!cur_g:scaled; {rounded equivalent of |cur_glue| times the glue ratio}
begin cur_g:=0; cur_glue:=float_constant(0);
this_box:=temp_ptr; g_order:=glue_order(this_box);
g_sign:=glue_sign(this_box); p:=list_ptr(this_box);
incr(cur_s);
if cur_s>0 then dvi_out(push);
if cur_s>max_push then max_push:=cur_s;
save_loc:=dvi_offset+dvi_ptr; left_edge:=cur_h; cur_v:=cur_v-height(this_box);
@y
@!cur_g:scaled; {rounded equivalent of |cur_glue| times the glue ratio}
@!upwards:boolean; {whether we're stacking upwards}
begin cur_g:=0; cur_glue:=float_constant(0);
this_box:=temp_ptr; g_order:=glue_order(this_box);
g_sign:=glue_sign(this_box); p:=list_ptr(this_box);
upwards:=(subtype(this_box)=min_quarterword+1);
incr(cur_s);
if cur_s>0 then dvi_out(push);
if cur_s>max_push then max_push:=cur_s;
save_loc:=dvi_offset+dvi_ptr; left_edge:=cur_h;
if upwards then cur_v:=cur_v+depth(this_box) else cur_v:=cur_v-height(this_box);
@z

@x
kern_node:cur_v:=cur_v+width(p);
@y
kern_node:if upwards then cur_v:=cur_v-width(p) else cur_v:=cur_v+width(p);
@z

@x
move_past: cur_v:=cur_v+rule_ht;
@y
move_past: if upwards then cur_v:=cur_v-rule_ht else cur_v:=cur_v+rule_ht;
@z

@x
@<Output a box in a vlist@>=
if list_ptr(p)=null then cur_v:=cur_v+height(p)+depth(p)
else  begin cur_v:=cur_v+height(p); synch_v;
  save_h:=dvi_h; save_v:=dvi_v;
  if cur_dir=right_to_left then cur_h:=left_edge-shift_amount(p)
  else cur_h:=left_edge+shift_amount(p); {shift the box right}
  temp_ptr:=p;
  if type(p)=vlist_node then vlist_out@+else hlist_out;
  dvi_h:=save_h; dvi_v:=save_v;
  cur_v:=save_v+depth(p); cur_h:=left_edge;
  end
@y
@<Output a box in a vlist@>=
if list_ptr(p)=null then
  if upwards then cur_v:=cur_v-height(p)-depth(p)
  else cur_v:=cur_v+height(p)+depth(p)
else  begin if upwards then cur_v:=cur_v-depth(p) else cur_v:=cur_v+height(p); synch_v;
  save_h:=dvi_h; save_v:=dvi_v;
  if cur_dir=right_to_left then cur_h:=left_edge-shift_amount(p)
  else cur_h:=left_edge+shift_amount(p); {shift the box right}
  temp_ptr:=p;
  if type(p)=vlist_node then vlist_out@+else hlist_out;
  dvi_h:=save_h; dvi_v:=save_v;
  if upwards then cur_v:=save_v-height(p) else cur_v:=save_v+depth(p); cur_h:=left_edge;
  end
@z

@x
cur_v:=cur_v+rule_ht;
@y
if upwards then cur_v:=cur_v-rule_ht else cur_v:=cur_v+rule_ht;
@z

@x
cur_v:=height(p)+v_offset; temp_ptr:=p;
@y
cur_v:=height(p)+v_offset; { does this need changing for upwards mode ???? }
temp_ptr:=p;
@z

@x { to ensure following change doesn't get applied to hpack() }
@d vpack(#)==vpackage(#,max_dimen) {special case of unconstrained depth}
@y
@d vpack(#)==vpackage(#,max_dimen) {special case of unconstrained depth}
@z

@x
subtype(r):=min_quarterword; shift_amount(r):=0;
@y
if XeTeX_upwards then subtype(r):=min_quarterword+1 else subtype(r):=min_quarterword;
shift_amount(r):=0;
@z

@x
@p procedure append_to_vlist(@!b:pointer);
var d:scaled; {deficiency of space between baselines}
@!p:pointer; {a new glue specification}
begin if prev_depth>ignore_depth then
  begin d:=width(baseline_skip)-prev_depth-height(b);
  if d<line_skip_limit then p:=new_param_glue(line_skip_code)
  else  begin p:=new_skip_param(baseline_skip_code);
    width(temp_ptr):=d; {|temp_ptr=glue_ptr(p)|}
    end;
  link(tail):=p; tail:=p;
  end;
link(tail):=b; tail:=b; prev_depth:=depth(b);
end;
@y
@p procedure append_to_vlist(@!b:pointer);
var d:scaled; {deficiency of space between baselines}
@!p:pointer; {a new glue specification}
@!upwards:boolean;
begin upwards:=XeTeX_upwards;
  if prev_depth>ignore_depth then
  begin if upwards then d:=width(baseline_skip)-prev_depth-depth(b)
  else d:=width(baseline_skip)-prev_depth-height(b);
  if d<line_skip_limit then p:=new_param_glue(line_skip_code)
  else  begin p:=new_skip_param(baseline_skip_code);
    width(temp_ptr):=d; {|temp_ptr=glue_ptr(p)|}
    end;
  link(tail):=p; tail:=p;
  end;
link(tail):=b; tail:=b; if upwards then prev_depth:=height(b) else prev_depth:=depth(b);
end;
@z

@x
begin d:=box_max_depth; unsave; save_ptr:=save_ptr-3;
if mode=-hmode then cur_box:=hpack(link(head),saved(2),saved(1))
else  begin cur_box:=vpackage(link(head),saved(2),saved(1),d);
  if c=vtop_code then @<Readjust the height and depth of |cur_box|,
    for \.{\\vtop}@>;
  end;
pop_nest; box_end(saved(0));
@y
@!u,v:integer; {saved values for upwards mode flag}
begin d:=box_max_depth; u:=XeTeX_upwards_state; unsave; save_ptr:=save_ptr-3;
v:=XeTeX_upwards_state; XeTeX_upwards_state:=u;
if mode=-hmode then cur_box:=hpack(link(head),saved(2),saved(1))
else  begin cur_box:=vpackage(link(head),saved(2),saved(1),d);
  if c=vtop_code then @<Readjust the height and depth of |cur_box|,
    for \.{\\vtop}@>;
  end;
XeTeX_upwards_state:=v;
pop_nest; box_end(saved(0));
@z

@x
@ @<Dump the \eTeX\ state@>=
dump_int(eTeX_mode);
for j:=0 to eTeX_states-1 do eTeX_state(j):=0; {disable all enhancements}
@y
@ @<Dump the \eTeX\ state@>=
dump_int(eTeX_mode);
{ in a deliberate change from e-TeX, we allow non-zero state variables to be dumped }
@z

@x
@d TeXXeT_en==(TeXXeT_state>0) {is \TeXXeT\ enabled?}
@y
@d TeXXeT_en==(TeXXeT_state>0) {is \TeXXeT\ enabled?}

@d XeTeX_upwards_state==eTeX_state(XeTeX_upwards_code)
@d XeTeX_upwards==(XeTeX_upwards_state>0)

@d XeTeX_use_glyph_metrics_state==eTeX_state(XeTeX_use_glyph_metrics_code)
@d XeTeX_use_glyph_metrics==(XeTeX_use_glyph_metrics_state>0)
@z

@x
eTeX_state_code+TeXXeT_code:print_esc("TeXXeTstate");
@y
eTeX_state_code+TeXXeT_code:print_esc("TeXXeTstate");
eTeX_state_code+XeTeX_upwards_code:print_esc("XeTeXupwardsmode");
eTeX_state_code+XeTeX_use_glyph_metrics_code:print_esc("XeTeXuseglyphmetrics");
@z

@x
primitive("TeXXeTstate",assign_int,eTeX_state_base+TeXXeT_code);
@!@:TeXXeT_state_}{\.{\\TeXXeT_state} primitive@>
@y
primitive("TeXXeTstate",assign_int,eTeX_state_base+TeXXeT_code);
@!@:TeXXeT_state_}{\.{\\TeXXeT_state} primitive@>
primitive("XeTeXupwardsmode",assign_int,eTeX_state_base+XeTeX_upwards_code);
@!@:XeTeX_upwards_mode_}{\.{\\XeTeX_upwards_mode} primitive@>
primitive("XeTeXuseglyphmetrics",assign_int,eTeX_state_base+XeTeX_use_glyph_metrics_code);
@!@:XeTeX_use_glyph_metrics_}{\.{\\XeTeX_use_glyph_metrics} primitive@>
@z

