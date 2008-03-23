% omdir.ch: Primitives to deal with multiple-direction text
%
% This file is part of Omega,
% which is based on the web2c distribution of TeX,
% 
% Copyright (c) 1994--2001 John Plaice and Yannis Haralambous
%
% Omega is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2 of the License, or
% (at your option) any later version.
% 
% Omega is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
% 
% You should have received a copy of the GNU General Public License
% along with Omega; if not, write to the Free Software Foundation, Inc.,
% 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
%
%-------------------------
@x [10] m.135
@d hlist_node=0 {|type| of hlist nodes}
@d box_node_size=7 {number of words to allocate for a box node}
@d width_offset=1 {position of |width| field in a box node}
@d depth_offset=2 {position of |depth| field in a box node}
@d height_offset=3 {position of |height| field in a box node}
@d width(#) == mem[#+width_offset].sc {width of the box, in sp}
@d depth(#) == mem[#+depth_offset].sc {depth of the box, in sp}
@d height(#) == mem[#+height_offset].sc {height of the box, in sp}
@d shift_amount(#) == mem[#+4].sc {repositioning distance, in sp}
@d list_offset=5 {position of |list_ptr| field in a box node}
@d list_ptr(#) == link(#+list_offset) {beginning of the list inside the box}
@d glue_order(#) == subtype(#+list_offset) {applicable order of infinity}
@d glue_sign(#) == type(#+list_offset) {stretching or shrinking}
@d normal=0 {the most common case when several cases are named}
@d stretching = 1 {glue setting applies to the stretch components}
@d shrinking = 2 {glue setting applies to the shrink components}
@d glue_offset = 6 {position of |glue_set| in a box node}
@d glue_set(#) == mem[#+glue_offset].gr
  {a word of type |glue_ratio| for glue setting}
@y
@d hlist_node=0 {|type| of hlist nodes}
@d box_node_size=8 {number of words to allocate for a box node}
@d width_offset=1 {position of |width| field in a box node}
@d depth_offset=2 {position of |depth| field in a box node}
@d height_offset=3 {position of |height| field in a box node}
@d width(#) == mem[#+width_offset].sc {width of the box, in sp}
@d depth(#) == mem[#+depth_offset].sc {depth of the box, in sp}
@d height(#) == mem[#+height_offset].sc {height of the box, in sp}
@d shift_amount(#) == mem[#+4].sc {repositioning distance, in sp}
@d list_offset=5 {position of |list_ptr| field in a box node}
@d list_ptr(#) == link(#+list_offset) {beginning of the list inside the box}
@d glue_order(#) == subtype(#+list_offset) {applicable order of infinity}
@d glue_sign(#) == type(#+list_offset) {stretching or shrinking}
@d normal=0 {the most common case when several cases are named}
@d stretching = 1 {glue setting applies to the stretch components}
@d shrinking = 2 {glue setting applies to the shrink components}
@d glue_offset = 6 {position of |glue_set| in a box node}
@d glue_set(#) == mem[#+glue_offset].gr
  {a word of type |glue_ratio| for glue setting}
@d dir_offset = 7 {position of |box_dir| in a box node}
@d box_dir(#) == mem[#+dir_offset].int
@z
%-------------------------
@x [10] m.136
@p function new_null_box:pointer; {creates a new box node}
var p:pointer; {the new node}
begin p:=get_node(box_node_size); type(p):=hlist_node;
subtype(p):=min_quarterword;
width(p):=0; depth(p):=0; height(p):=0; shift_amount(p):=0; list_ptr(p):=null;
glue_sign(p):=normal; glue_order(p):=normal; set_glue_ratio_zero(glue_set(p));
@<LOCAL: Add info node pointer@>;
new_null_box:=p;
@y
@p function new_null_box:pointer; {creates a new box node}
var p:pointer; {the new node}
begin p:=get_node(box_node_size); type(p):=hlist_node;
subtype(p):=min_quarterword;
width(p):=0; depth(p):=0; height(p):=0; shift_amount(p):=0; list_ptr(p):=null;
glue_sign(p):=normal; glue_order(p):=normal; set_glue_ratio_zero(glue_set(p));
@<LOCAL: Add info node pointer@>;
box_dir(p):=text_direction; 
new_null_box:=p;
@z
%-------------------------
@x [10] m.138
@d rule_node=2 {|type| of rule nodes}
@d rule_node_size=4 {number of words to allocate for a rule node}
@d null_flag==-@'10000000000 {$-2^{30}$, signifies a missing item}
@d is_running(#) == (#=null_flag) {tests for a running dimension}
@y
@d rule_node=2 {|type| of rule nodes}
@d rule_node_size=5 {number of words to allocate for a rule node}
@d null_flag==-@'10000000000 {$-2^{30}$, signifies a missing item}
@d is_running(#) == (#=null_flag) {tests for a running dimension}
@d rule_dir(#)==info(#+4)
@z
%-------------------------
@x [10] m.139
@p function new_rule:pointer;
var p:pointer; {the new node}
begin p:=get_node(rule_node_size); type(p):=rule_node;
subtype(p):=0; {the |subtype| is not used}
width(p):=null_flag; depth(p):=null_flag; height(p):=null_flag;
@<LOCAL: Add info node pointer@>;
new_rule:=p;
end;
@y
@p function new_rule:pointer;
var p:pointer; {the new node}
begin p:=get_node(rule_node_size); type(p):=rule_node;
subtype(p):=0; {the |subtype| is not used}
width(p):=null_flag; depth(p):=null_flag; height(p):=null_flag;
@<LOCAL: Add info node pointer@>;
rule_dir(p):=-1;
new_rule:=p;
end;
@z
%-------------------------
@x [10] m.160
@d info_node=unset_node+2 {|type| to hold local information}

@d ptr_info(#)        == link(#-1) {pointer to the local info node}

@d info_size=5
@d info_ref_count(#)  == link(#+1) {reference count to this node}
@d info_pen_inter(#)  == info(#+2) {\.{\\localinterlinepenalty}}
@d info_pen_broken(#) == link(#+2) {\.{\\localbrokenpenalty}}
@d info_left_box(#)   == info(#+3) {\.{\\localleftbox}}
@d info_right_box(#)  == link(#+3) {\.{\\localrightbox}}
@d info_file(#)       == info(#+4) {file names and line numbers}
@y
@d dir_node=unset_node+2 {|type| to keep track of stacks of directions}

@d dir_size=3
@d dir_no(#) == info(#+1) {number of directions to keep track of}
@d dir_ref_count(#) == link(#+1) {reference count to this node}

@d dir_incr_ref_count(#)==begin
  if (#)<>null then incr(dir_ref_count(#));
  end
@d dir_decr_ref_count(#)==begin
  if (#)<>null then begin
    decr(dir_ref_count(#));
    if dir_ref_count(#) = 0 then begin
      free_node(#,file_size+dir_no(#));
      #:=null;
      end;
    end;
  end

@d info_node=unset_node+3 {|type| to hold local information}

@d ptr_info(#)        == link(#-1) {pointer to the local info node}

@d info_size=5
@d info_ref_count(#)  == link(#+1) {reference count to this node}
@d info_pen_inter(#)  == info(#+2) {\.{\\localinterlinepenalty}}
@d info_pen_broken(#) == link(#+2) {\.{\\localbrokenpenalty}}
@d info_left_box(#)   == info(#+3) {\.{\\localleftbox}}
@d info_right_box(#)  == link(#+3) {\.{\\localrightbox}}
@d info_file(#)       == info(#+4) {file names and line numbers}
@d info_dir(#)        == link(#+4) {direction}

@d get_dir(#)         == mem[info_dir(ptr_info(#))+
                             2*dir_no(info_dir(ptr_info(#)))+1].int
@z
%-------------------------
@x [10] m.160
      free_node(#,info_size);
@y
      dir_decr_ref_count(info_dir(#));
      info_dir(#):=null;
      free_node(#,info_size);
@z
%-------------------------
@x [10] m.176
@p procedure print_font_and_char(@!p:integer); {prints |char_node| data}
begin if p>mem_end then print_esc("CLOBBERED.")
else  begin if (font(p)<font_base)or(font(p)>font_max) then print_char("*")
@.*\relax@>
  else @<Print the font identifier for |font(p)|@>;
  print_char(" "); print_ASCII(qo(character(p)));
  end;
end;
@y
@p procedure print_dir(d:integer);
begin
print(dir_names[dir_primary[d]]);
print(dir_names[dir_secondary[d]]);
print(dir_names[dir_tertiary[d]]);
end;
@#
procedure print_dir_node(p:pointer);
var i:integer;
begin
if dir_no(p)=1 then print(" dir=")
else print(" dirs=");
for i:=1 to dir_no(p) do begin
  print_dir(mem[p+2*i+1].int);
  if i<>dir_no(p) then print(",");
  end;
end;
@#
procedure print_font_and_char(@!p:integer); {prints |char_node| data}
begin if p>mem_end then print_esc("CLOBBERED.")
else  begin if (font(p)<font_base)or(font(p)>font_max) then print_char("*")
@.*\relax@>
  else @<Print the font identifier for |font(p)|@>;
  print_char(" "); print_ASCII(qo(character(p)));
  end;
end;
@z
%-------------------------
@x [12] m.183
@ @<Display node |p|@>=
if is_char_node(p) then print_font_and_char(p)
@y
@ @<Display node |p|@>=
if is_char_node(p) then begin print_font_and_char(p);
  print_dir_node(info_dir(ptr_info(p)));
  {print(" dir="); print_dir(get_dir(p));}
  end
@z
%-------------------------
@x [12] m.184
@ @<Display box |p|@>=
begin if type(p)=hlist_node then print_esc("h")
else if type(p)=vlist_node then print_esc("v")
else print_esc("unset");
print("box("); print_scaled(height(p)); print_char("+");
print_scaled(depth(p)); print(")x"); print_scaled(width(p));
if type(p)=unset_node then
  @<Display special fields of the unset node |p|@>
else  begin @<Display the value of |glue_set(p)|@>;
  if shift_amount(p)<>0 then
    begin print(", shifted "); print_scaled(shift_amount(p));
    end;
  end;
node_list_display(list_ptr(p)); {recursive call}
end
@y
@ @<Display box |p|@>=
begin if type(p)=hlist_node then print_esc("h")
else if type(p)=vlist_node then print_esc("v")
else print_esc("unset");
print("box("); print_scaled(height(p)); print_char("+");
print_scaled(depth(p)); print(")x"); print_scaled(width(p));
if type(p)=unset_node then
  @<Display special fields of the unset node |p|@>
else  begin @<Display the value of |glue_set(p)|@>;
  if shift_amount(p)<>0 then
    begin print(", shifted "); print_scaled(shift_amount(p));
    end;
  print(", direction "); print_dir(box_dir(p));
  end;
node_list_display(list_ptr(p)); {recursive call}
end
@z
%-------------------------
@x [12] m.193
@ @<Display ligature |p|@>=
begin print_font_and_char(lig_char(p)); print(" (ligature ");
@y
@ @<Display ligature |p|@>=
begin print_font_and_char(lig_char(p));
print_dir_node(info_dir(ptr_info(p)));
{print(" dir="); print_dir(get_dir(p));}
print(" (ligature ");
@z
%-------------------------
@x [13] m.202
    info_node: begin
               @<LOCAL: Delete info node@>;
               goto done; end; {do not delete node at this time}
@y
    info_node: begin
               @<LOCAL: Delete info node@>;
               goto done; end; {do not delete node at this time}
    dir_node:  begin
               @<LOCAL: Delete direction node@>;
               goto done; end; {do not delete node at this time}
@z
%-------------------------
@x [13] m.206
hlist_node,vlist_node,unset_node: begin r:=get_node(box_node_size);
  mem[r+6]:=mem[p+6]; mem[r+5]:=mem[p+5]; {copy the last two words}
@y
hlist_node,vlist_node,unset_node: begin r:=get_node(box_node_size);
  mem[r+7]:=mem[p+7]; mem[r+6]:=mem[p+6]; mem[r+5]:=mem[p+5];
  {copy the last three words}
@z
%-------------------------
@x [15] m.209
@d max_internal=register
   {the largest code that can follow \.{\\the}}
@y
@d assign_next_fake_math=register+1 {(\.{\\nextfakemath})}
@d assign_box_dir=register+2 {(\.{\\boxdir})}
@d assign_dir=register+3 {(\.{\\pagedir}, \.{\\textdir})}
@d max_internal=assign_dir
   {the largest code that can follow \.{\\the}}
@z
%-------------------------
@x [17] m.230
@d local_par_info_base=local_right_box_base+1
@y
@d local_dir_base=local_right_box_base+1
@d local_par_info_base=local_dir_base+1
@z
%-------------------------
@x [17] m.230
@d local_par_info==equiv(local_par_info_base)
@y
@d local_dir==equiv(local_dir_base)
@d local_par_info==equiv(local_par_info_base)
@z
%-------------------------
@x [17] m.236
@d int_pars=58 {total number of integer parameters}
@d count_base=int_base+int_pars {|number_regs| user \.{\\count} registers}
@y
@d int_pars=58 {total number of integer parameters}
@d dir_base=int_base+int_pars
@d page_direction_code=0
@d body_direction_code=1
@d par_direction_code=2
@d text_direction_code=3
@d math_direction_code=4
@d dir_pars=5
@d count_base=dir_base+dir_pars {|number_regs| user \.{\\count} registers}
@z
%-------------------------
@x [17] m.238
@d local_broken_penalty==int_par(local_broken_penalty_code)
@y
@d local_broken_penalty==int_par(local_broken_penalty_code)
@d dir_par(#)==new_eqtb_int(dir_base+#) {a direction parameter}
@d page_direction==dir_par(page_direction_code)
@d body_direction==dir_par(body_direction_code)
@d par_direction==dir_par(par_direction_code)
@d text_direction==dir_par(text_direction_code)
@d math_direction==dir_par(math_direction_code)
@z
%-------------------------
@x [17] m.247
@d dimen_pars=21 {total number of dimension parameters}
@y
@d page_width_code=21
@d page_height_code=22
@d page_right_offset_code=23
@d page_bottom_offset_code=24
@d dimen_pars=25 {total number of dimension parameters}
@z
%-------------------------
@x [17] m.247
@d emergency_stretch==dimen_par(emergency_stretch_code)
@y
@d emergency_stretch==dimen_par(emergency_stretch_code)
@d page_width==dimen_par(page_width_code)
@d page_height==dimen_par(page_height_code)
@d page_right_offset==dimen_par(page_right_offset_code)
@d page_bottom_offset==dimen_par(page_bottom_offset_code)
@z
%-------------------------
@x [26] m.410
@d int_val=0 {integer values}
@d dimen_val=1 {dimension values}
@d glue_val=2 {glue specifications}
@d mu_val=3 {math glue specifications}
@d ident_val=4 {font identifier}
@d tok_val=5 {token lists}

@<Glob...@>=
@!cur_val:integer; {value returned by numeric scanners}
@!cur_val_level:int_val..tok_val; {the ``level'' of this value}
@y
@d int_val=0 {integer values}
@d dimen_val=1 {dimension values}
@d glue_val=2 {glue specifications}
@d mu_val=3 {math glue specifications}
@d dir_val=4 {directions}
@d ident_val=5 {font identifier}
@d tok_val=6 {token lists}

@<Glob...@>=
@!cur_val:integer; {value returned by numeric scanners}
@!cur_val1:integer; {delcodes are now 51 digits}
@!cur_val2:integer; {will be used in omxml.ch; this is cheating}
@!cur_val_level:int_val..dir_val; {the ``level'' of this value}
@!tmp_dir_stack:pointer;
@!par_dir_stack:pointer;
@!math_dir_stack:pointer;
@!align_dir_stack:pointer;
@z
%-------------------------
@x [26] m.413
assign_int: scanned_result(new_eqtb_int(m))(int_val);
@y
assign_int: scanned_result(new_eqtb_int(m))(int_val);
assign_box_dir: begin
  scan_eight_bit_int;
  m:=cur_val;
  if box(m)<>null then cur_val:=box_dir(box(m))
  else cur_val:=0;
  cur_val_level:=dir_val;
  end;
assign_dir: scanned_result(new_eqtb_int(m))(dir_val);
@z
%-------------------------
@x [26] m.437
@ @<Declare procedures that scan restricted classes of integers@>=
procedure scan_eight_bit_int;
@y
@
@d dir_T=0
@d dir_L=1
@d dir_B=2
@d dir_R=3
@d dir_eq_end(#)==(#)
@d dir_eq(#)==(#)=dir_eq_end
@d dir_ne_end(#)==(#)
@d dir_ne(#)==(#)<>dir_ne_end
@d dir_opposite_end(#)==((#) mod 4)
@d dir_opposite(#)==(((#)+2) mod 4)=dir_opposite_end
@d dir_parallel_end(#)==((#) mod 2)
@d dir_parallel(#)==((#) mod 2)=dir_parallel_end
@d dir_orthogonal_end(#)==(# mod 2)
@d dir_orthogonal(#)==((#) mod 2)<>dir_orthogonal_end
@d dir_next_end(#)==((#) mod 4)
@d dir_next(#)==(((#)+3) mod 4)=dir_next_end
@d dir_prev_end(#)==((#) mod 4)
@d dir_prev(#)==(((#)+1) mod 4)=dir_prev_end

{box directions}
@d dir_TL_=0
@d dir_TR_=1
@d dir_LT_=2
@d dir_LB_=3
@d dir_BL_=4
@d dir_BR_=5
@d dir_RT_=6
@d dir_RB_=7

{font directions}
@d dir__LT= 0
@d dir__LL= 1
@d dir__LB= 2
@d dir__LR= 3
@d dir__RT= 4
@d dir__RL= 5
@d dir__RB= 6
@d dir__RR= 7
@d dir__TT= 8
@d dir__TL= 9
@d dir__TB=10
@d dir__TR=11
@d dir__BT=12
@d dir__BL=13
@d dir__BB=14
@d dir__BR=15

@d is_mirrored(#)==dir_opposite(dir_primary[#])(dir_tertiary[#])
@d is_rotated(#)==dir_parallel(dir_secondary[#])(dir_tertiary[#])
@d font_direction(#)==(# mod 16)
@d box_direction(#)==(# div 4)

@d scan_single_dir(#)==begin
if scan_keyword("T") then #:=dir_T
else if scan_keyword("L") then #:=dir_L
else if scan_keyword("B") then #:=dir_B
else if scan_keyword("R") then #:=dir_R
else begin
  print_err("Bad direction"); cur_val:=0;
  goto exit;
  end
end

@<Declare procedures that scan restricted classes of integers@>=
@<DIR: Declare |change_dir|@>;
procedure scan_dir;
var d1,d2,d3: integer;
begin
get_x_token;
if cur_cmd=assign_dir then begin
  cur_val:=new_eqtb_int(cur_chr);
  goto exit;
  end
else back_input;
scan_single_dir(d1);
scan_single_dir(d2);
if dir_parallel(d1)(d2) then begin
  print_err("Bad direction"); cur_val:=0;
  goto exit;
  end;
scan_single_dir(d3);
cur_val:=d1*8+dir_rearrange[d2]*4+d3;
exit:
end;

@ @<Declare procedures that scan restricted classes of integers@>=
procedure scan_eight_bit_int;
@z
%-------------------------
@x [27] m.463
@p function scan_rule_spec:pointer;
label reswitch;
var q:pointer; {the rule node being created}
begin q:=new_rule; {|width|, |depth|, and |height| all equal |null_flag| now}
if cur_cmd=vrule then width(q):=default_rule
else  begin height(q):=default_rule; depth(q):=0;
  end;
@y
@p function scan_rule_spec:pointer;
label reswitch;
var q:pointer; {the rule node being created}
begin q:=new_rule; {|width|, |depth|, and |height| all equal |null_flag| now}
if cur_cmd=vrule then begin
  width(q):=default_rule;
  rule_dir(q):=body_direction;
  end
else begin
  height(q):=default_rule; depth(q):=0;
  rule_dir(q):=text_direction;
end;
@z
%-------------------------
@x [27] m.465
  int_val:print_int(cur_val);
@y
  int_val:print_int(cur_val);
  dir_val:print_dir(cur_val);
@z
%-------------------------
@x [30] m.550
@d offset_false_bchar=offset_bchar+1
@d offset_ivalues_start=offset_false_bchar+1
@y
@d offset_false_bchar=offset_bchar+1
@d offset_natural_dir=offset_false_bchar+1
@d offset_ivalues_start=offset_natural_dir+1
@z
%-------------------------
@x [30] m.550
@d font_false_bchar(#)==font_info(#)(offset_false_bchar).int
@y
@d font_false_bchar(#)==font_info(#)(offset_false_bchar).int
@d font_natural_dir(#)==font_info(#)(offset_natural_dir).int
@z
%-------------------------
@x [30] m.560
@p function read_font_info(@!u:pointer;@!nom,@!aire:str_number;
   @!s:scaled;offset:quarterword):internal_font_number; {input a \.{TFM} file}
@y
@p function read_font_info(@!u:pointer;@!nom,@!aire:str_number;
   @!s:scaled;offset:quarterword;natural_dir:integer):internal_font_number;
   {input a \.{TFM} file}
@z
%-------------------------
@x [30] m.560
font_offset(f):=offset;
@y
font_offset(f):=offset;
font_natural_dir(f):=natural_dir;
@z
%-------------------------
@x [31] m.586
@d set_rule=132 {typeset a rule and move right}
@y
@d set_rule=132 {typeset a rule and move right}
@d put1==133 {typeset a character without moving}
@z
%-------------------------
@x [31] m.586
@d right1=143 {move right}
@y
@d right1==143 {move right}
@d right4==146 {move right, 4 bytes}
@z
%-------------------------
@x [31] m.586
@d down1=157 {move down}
@y
@d down1=157 {move down}
@d down4=160 {move down, 4 bytes}
@z
%-------------------------
@x [32] m.607
@p procedure movement(@!w:scaled;@!o:eight_bits);
label exit,found,not_found,2,1;
var mstate:small_number; {have we seen a |y| or |z|?}
@!p,@!q:pointer; {current and top nodes on the stack}
@!k:integer; {index into |dvi_buf|, modulo |dvi_buf_size|}
begin q:=get_node(movement_node_size); {new node for the top of the stack}
width(q):=w; location(q):=dvi_offset+dvi_ptr;
if o=down1 then
  begin link(q):=down_ptr; down_ptr:=q;
  end
else  begin link(q):=right_ptr; right_ptr:=q;
  end;
@<Look at the other stack entries until deciding what sort of \.{DVI} command
  to generate; |goto found| if node |p| is a ``hit''@>;
@<Generate a |down| or |right| command for |w| and |return|@>;
found: @<Generate a |y0| or |z0| command in order to reuse a previous
  appearance of~|w|@>;
exit:end;
@y
@d dvi_set(#)==oval:=#-font_offset(f); ocmd:=set1; out_cmd
@d dvi_put(#)==oval:=#-font_offset(f); ocmd:=put1; out_cmd
@d dvi_set_rule_end(#)==dvi_four(#)
@d dvi_set_rule(#)==dvi_out(set_rule); dvi_four(#); dvi_set_rule_end
@d dvi_put_rule_end(#)==dvi_four(#)
@d dvi_put_rule(#)==dvi_out(put_rule); dvi_four(#); dvi_put_rule_end
@d dvi_right(#)==dvi_out(right4); dvi_four(#);
@d dvi_left(#)==dvi_out(right4); dvi_four(-#);
@d dvi_down(#)==dvi_out(down4); dvi_four(#);
@d dvi_up(#)==dvi_out(down4); dvi_four(-#);

@p procedure movement(@!w:scaled;@!o:eight_bits);
label exit,found,not_found,2,1;
var mstate:small_number; {have we seen a |y| or |z|?}
@!p,@!q:pointer; {current and top nodes on the stack}
@!k:integer; {index into |dvi_buf|, modulo |dvi_buf_size|}
begin
case box_direction(dvi_direction) of
  dir_TL_: begin
    end;
  dir_TR_: begin
    if o=right1 then negate(w);
    end;
  dir_LT_: begin
    if o=right1 then o:=down1
    else o:=right1;
    end;
  dir_LB_: begin
    if o=right1 then begin
      o:=down1; negate(w);
      end
    else o:=right1;
    end;
  dir_BL_: begin
    if o=down1 then negate(w);
    end;
  dir_BR_: begin
    negate(w);
    end;
  dir_RT_: begin
    if o=right1 then o:=down1
    else begin o:=right1; negate(w);
      end;
    end;
  dir_RB_: begin
    if o=right1 then o:=down1 else o:=right1;
    negate(w);
    end;
  end;
if false then begin
q:=get_node(movement_node_size); {new node for the top of the stack}
width(q):=w; location(q):=dvi_offset+dvi_ptr;
if o=down1 then
  begin link(q):=down_ptr; down_ptr:=q;
  end
else  begin link(q):=right_ptr; right_ptr:=q;
  end;
@<Look at the other stack entries until deciding what sort of \.{DVI} command
  to generate; |goto found| if node |p| is a ``hit''@>;
end;
@<Generate a |down| or |right| command for |w| and |return|@>;
found: @<Generate a |y0| or |z0| command in order to reuse a previous
  appearance of~|w|@>;
exit:end;
@z
%-------------------------
@x [32] m.610
info(q):=yz_OK;
@y
@z
%-------------------------
@x [32] m.617
dvi_h:=0; dvi_v:=0; cur_h:=h_offset; dvi_f:=null_font;
@y
dvi_h:=0; dvi_v:=0; cur_h:=0; cur_v:=0; dvi_f:=null_font;
@z
%-------------------------
@x [32] m.619
procedure hlist_out; {output an |hlist_node| box}
label reswitch, move_past, fin_rule, next_p;
var base_line: scaled; {the baseline coordinate for this box}
@y
procedure hlist_out; {output an |hlist_node| box}
label reswitch, move_past, fin_rule, next_p;
var base_line: scaled; {the baseline coordinate for this box}
c_wd,c_ht,c_dp: scaled;
  {the real width, height and depth of the character}
c_htdp: quarterword; {height-depth entry in |char_info|}
c_info: four_quarters; {|char_info| entry}
edge_v: scaled;
edge_h: scaled;
effective_horizontal: scaled;
basepoint_horizontal: scaled;
basepoint_vertical: scaled;
saving_h: scaled;
saving_v: scaled;
save_direction: integer;
dir_tmp,dir_ptr: pointer;
dvi_dir_h,dvi_dir_ptr,dvi_temp_ptr: integer;
char_first: integer;
cur_direction: integer;
new_dir_node,prev_dir_node: pointer;
i,d,dir_level,m: integer; {counters for the direction stuff}
dir_found: boolean;
@z
%-------------------------
@x [32] m.619
incr(cur_s);
if cur_s>0 then dvi_out(push);
if cur_s>max_push then max_push:=cur_s;
save_loc:=dvi_offset+dvi_ptr; base_line:=cur_v; left_edge:=cur_h;
if char_specials>0 then begin
  should_create_special:=true;
  chars_since_last_special:=char_specials;
  end
else begin
  should_create_special:=false;
  chars_since_last_special:=-1000000;
  end;
while p<>null do @<Output node |p| for |hlist_out| and move to the next node,
  maintaining the condition |cur_v=base_line|@>;
prune_movements(save_loc);
if cur_s>0 then dvi_pop(save_loc);
decr(cur_s);
@y
save_direction:=dvi_direction;
incr(cur_s);
if cur_s>0 then dvi_out(push);
if cur_s>max_push then max_push:=cur_s;
save_loc:=dvi_offset+dvi_ptr; base_line:=cur_v; left_edge:=cur_h;
if dvi_direction<>box_dir(this_box) then begin
  dvi_direction:=box_dir(this_box);
  synch_v; synch_h;
  saving_h:=dvi_h; saving_v:=dvi_v;
  end;
@<DIR: Initialize the text direction@>;
if char_specials>0 then begin
  should_create_special:=true;
  chars_since_last_special:=char_specials;
  end
else begin
  should_create_special:=false;
  chars_since_last_special:=-1000000;
  end;
while p<>null do @<Output node |p| for |hlist_out| and move to the next node,
  maintaining the condition |cur_v=base_line|@>;
@<DIR: Finalize the text direction@>;
if dvi_direction<>save_direction then begin
  dvi_direction:=save_direction;
  synch_v; synch_h;
  cur_h:=saving_h; cur_v:=saving_v;
  end;
prune_movements(save_loc);
if cur_s>0 then dvi_pop(save_loc);
decr(cur_s);
@z
%-------------------------
@x [32] m.620
@<Output node |p| for |hlist_out|...@>=
reswitch: if is_char_node(p) then
  begin synch_h; synch_v;
  if should_create_special then begin
    should_create_special:=false;
    if chars_since_last_special>=char_specials then
      if ptr_info(p)<>null then
        if info_file(ptr_info(p))<>null then begin
          @<LOCAL: Create special node@>;
          chars_since_last_special:=0;
          end;
    end;
  repeat f:=font(p); c:=character(p);
  incr(chars_since_last_special);
  if f<>dvi_f then @<Change font |dvi_f| to |f|@>;
  oval:=c-font_offset(f); ocmd:=set1; out_cmd;@/
  cur_h:=cur_h+char_width(f)(char_info(f)(c));
  p:=link(p);
  until not is_char_node(p);
  dvi_h:=cur_h;
  end
else @<Output the non-|char_node| |p| for |hlist_out|
    and move to the next node@>
@y
@<Output node |p| for |hlist_out|...@>=
reswitch: if is_char_node(p) then
  begin synch_h; synch_v;
  if should_create_special then begin
    should_create_special:=false;
    if chars_since_last_special>=char_specials then
      if ptr_info(p)<>null then
        if info_file(ptr_info(p))<>null then begin
          @<LOCAL: Create special node@>;
          chars_since_last_special:=0;
          end;
    end;
  repeat f:=font(p); c:=character(p);
  incr(chars_since_last_special);
  if f<>dvi_f then @<Change font |dvi_f| to |f|@>;
  c_info:=char_info(f)(c);
  c_htdp:=height_depth(c_info);
  if is_rotated(dvi_direction) then begin
    c_ht:=char_width(f)(c_info) div 2;
    c_wd:=char_height(f)(c_htdp)+char_depth(f)(c_htdp);
    end
  else begin
    c_ht:=char_height(f)(c_htdp);
    c_dp:=char_depth(f)(c_htdp);
    c_wd:=char_width(f)(c_info);
    end;
  cur_h:=cur_h+c_wd;
  if (font_natural_dir(f)<>-1) then
    case font_direction(dvi_direction) of
    dir__LT,dir__LB: begin
      dvi_set(c);
      end;
    dir__RT,dir__RB: begin
      dvi_put(c);
      dvi_left(c_wd);
      end;
    dir__TL,dir__TR: begin
      dvi_put(c);
      dvi_down(c_wd);
      end;
    dir__BL,dir__BR: begin
      dvi_put(c);
      dvi_up(c_wd);
      end;
    dir__LL,dir__LR: begin
      dvi_put(c);
      dvi_right(c_wd);
      end;
    dir__RL,dir__RR: begin
      dvi_put(c);
      dvi_left(c_wd);
      end;
    dir__TT,dir__TB: begin
      dvi_put(c);
      dvi_down(c_wd);
      end;
    dir__BT,dir__BB: begin
      dvi_put(c);
      dvi_up(c_wd);
      end;
    end
  else
    case font_direction(dvi_direction) of
    dir__LT: begin
      dvi_set(c);
      end;
    dir__LB: begin
      dvi_down(c_ht);
      dvi_set(c);
      dvi_up(c_ht);
      end;
    dir__RT: begin
      dvi_left(c_wd);
      dvi_put(c);
      end;
    dir__RB: begin
      dvi_left(c_wd);
      dvi_down(c_ht);
      dvi_put(c);
      dvi_up(c_ht);
      end;
    dir__TL: begin
      dvi_down(c_wd);
      dvi_left(c_ht);
      dvi_put(c);
      dvi_right(c_ht);
      end;
    dir__TR: begin
      dvi_down(c_wd);
      dvi_left(c_dp);
      dvi_put(c);
      dvi_right(c_dp);
      end;
    dir__BL: begin
      dvi_left(c_ht);
      dvi_put(c);
      dvi_right(c_ht);
      dvi_up(c_wd);
      end;
    dir__BR: begin
      dvi_left(c_dp);
      dvi_put(c);
      dvi_right(c_dp);
      dvi_up(c_wd);
      end;
    dir__LL,dir__LR: begin
      dvi_down(c_ht);
      dvi_put(c);
      dvi_up(c_ht);
      dvi_right(c_wd);
      end;
    dir__RL,dir__RR: begin
      dvi_left(c_wd);
      dvi_down(c_ht);
      dvi_put(c);
      dvi_up(c_ht);
      end;
    dir__TT,dir__TB: begin
      dvi_down(c_wd);
      dvi_left(c_ht);
      dvi_put(c);
      dvi_right(c_ht);
      end;
    dir__BT,dir__BB: begin
      dvi_left(c_ht);
      dvi_put(c);
      dvi_right(c_ht);
      dvi_up(c_wd);
      end;
    end;
  p:=link(p);
  dvi_h:=cur_h;
  @<DIR: Check if the text direction has changed@>;
  until not is_char_node(p);
  end
else @<Output the non-|char_node| |p| for |hlist_out|
    and move to the next node@>
@z
%-------------------------
@x [32] m.622
rule_node: begin rule_ht:=height(p); rule_dp:=depth(p); rule_wd:=width(p);
  goto fin_rule;
  end;
whatsit_node: @<Output the whatsit node |p| in an hlist@>;
glue_node: @<Move right or output leaders@>;
kern_node,math_node:cur_h:=cur_h+width(p);
ligature_node: @<Make node |p| look like a |char_node| and |goto reswitch|@>;
othercases do_nothing
endcases;@/
goto next_p;
fin_rule: @<Output a rule in an hlist@>;
move_past: cur_h:=cur_h+rule_wd;
next_p:
if type(p)<>kern_node then should_create_special:=true;
p:=link(p);
end
@y
rule_node: begin
  if rule_dir(p)=-1 then begin
    print("Rule direction is not defined"); print_ln;
    end;
  if not (dir_orthogonal(dir_primary[rule_dir(p)])
                        (dir_primary[dvi_direction])) then begin
    rule_ht:=height(p); rule_dp:=depth(p); rule_wd:=width(p);
    end
  else begin
    rule_ht:=width(p) div 2;
    rule_dp:=width(p) div 2;
    rule_wd:=height(p)+depth(p);
    end;
  goto fin_rule;
  end;
whatsit_node: @<Output the whatsit node |p| in an hlist@>;
glue_node: @<Move right or output leaders@>;
kern_node,math_node:cur_h:=cur_h+width(p);
ligature_node: @<Make node |p| look like a |char_node| and |goto reswitch|@>;
othercases do_nothing
endcases;@/
goto next_p;
fin_rule: @<Output a rule in an hlist@>;
move_past: cur_h:=cur_h+rule_wd;
next_p:
if type(p)<>kern_node then should_create_special:=true;
p:=link(p);
@<DIR: Check if the text direction has changed@>;
end
@z
%-------------------------
@x [32] m.623
if list_ptr(p)=null then cur_h:=cur_h+width(p)
else  begin save_h:=dvi_h; save_v:=dvi_v;
  cur_v:=base_line+shift_amount(p); {shift the box down}
  temp_ptr:=p; edge:=cur_h;
  if type(p)=vlist_node then vlist_out@+else hlist_out;
  dvi_h:=save_h; dvi_v:=save_v;
  cur_h:=edge+width(p); cur_v:=base_line;
  end
@y
begin
if not (dir_orthogonal(dir_primary[box_dir(p)])
                      (dir_primary[dvi_direction])) then begin
  effective_horizontal:=width(p);
  basepoint_vertical:=0;
  if dir_opposite(dir_secondary[box_dir(p)])
                 (dir_secondary[dvi_direction]) then
    basepoint_horizontal:=width(p)
  else
    basepoint_horizontal:=0;
  end
else begin
  effective_horizontal:=height(p)+depth(p);
  if not (is_mirrored(box_dir(p))) then
    if dir_eq(dir_primary[box_dir(p)])
             (dir_secondary[dvi_direction]) then
      basepoint_horizontal:=height(p)
    else
      basepoint_horizontal:=depth(p)
  else
    if dir_eq(dir_primary[box_dir(p)])
             (dir_secondary[dvi_direction]) then
      basepoint_horizontal:=height(p)
    else
      basepoint_horizontal:=depth(p);
  if dir_eq(dir_secondary[box_dir(p)])
           (dir_primary[dvi_direction]) then
    basepoint_vertical:= -(width(p) div 2)
  else
    basepoint_vertical:= (width(p) div 2);
  end;
if not (is_mirrored(dvi_direction)) then
  basepoint_vertical := basepoint_vertical + shift_amount(p)
    {shift the box `down'}
else
  basepoint_vertical := basepoint_vertical - shift_amount(p);
    {shift the box `up'}
if list_ptr(p)=null then cur_h:=cur_h + effective_horizontal
else begin
  temp_ptr:=p; edge:=cur_h; cur_h:=cur_h + basepoint_horizontal;
  edge_v:=cur_v; cur_v:=base_line + basepoint_vertical;
  {if dvi_direction<>box_dir(p) then begin}
    synch_h; synch_v;
    {end;}
  save_h:=dvi_h; save_v:=dvi_v;
  if type(p)=vlist_node then vlist_out@+else hlist_out;
  dvi_h:=save_h; dvi_v:=save_v;
  cur_h:=edge+effective_horizontal; cur_v:=base_line;
  end
end
@z
%-------------------------
@x [32] m.624
  dvi_out(set_rule); dvi_four(rule_ht); dvi_four(rule_wd);
@y
  case font_direction(dvi_direction) of
  dir__LT: begin
    dvi_set_rule(rule_ht)(rule_wd);
    end;
  dir__LB: begin
    dvi_down(rule_ht);
    dvi_set_rule(rule_ht)(rule_wd);
    dvi_up(rule_ht);
    end;
  dir__RT: begin
    dvi_left(rule_wd);
    dvi_put_rule(rule_ht)(rule_wd);
    end;
  dir__RB: begin
    dvi_left(rule_wd);
    dvi_down(rule_ht);
    dvi_put_rule(rule_ht)(rule_wd);
    dvi_up(rule_ht);
    end;
  dir__TL: begin
    dvi_down(rule_wd);
    dvi_left(rule_ht);
    dvi_set_rule(rule_wd)(rule_ht);
    end;
  dir__TR: begin
    dvi_down(rule_wd);
    dvi_put_rule(rule_wd)(rule_ht);
    end;
  dir__BL: begin
    dvi_left(rule_ht);
    dvi_set_rule(rule_wd)(rule_ht);
    dvi_up(rule_wd);
    end;
  dir__BR: begin
    dvi_put_rule(rule_wd)(rule_ht);
    dvi_up(rule_wd);
    end;
  end;
@z
%-------------------------
@x [32] m.626
leader_wd:=width(leader_box);
@y
if not (dir_orthogonal(dir_primary[box_dir(leader_box)])
                      (dir_primary[dvi_direction])) then
  leader_wd:=width(leader_box)
else
  leader_wd:=height(leader_box)+depth(leader_box);
@z
%-------------------------
@x [32] m.628
@<Output a leader box at |cur_h|, ...@>=
begin cur_v:=base_line+shift_amount(leader_box); synch_v; save_v:=dvi_v;@/
synch_h; save_h:=dvi_h; temp_ptr:=leader_box;
outer_doing_leaders:=doing_leaders; doing_leaders:=true;
if type(leader_box)=vlist_node then vlist_out@+else hlist_out;
doing_leaders:=outer_doing_leaders;
dvi_v:=save_v; dvi_h:=save_h; cur_v:=base_line;
cur_h:=save_h+leader_wd+lx;
end
@y
@<Output a leader box at |cur_h|, ...@>=
begin
if not (dir_orthogonal(dir_primary[box_dir(leader_box)])
                      (dir_primary[dvi_direction])) then begin
  basepoint_vertical:=0;
 if dir_opposite(dir_secondary[box_dir(leader_box)])
                (dir_secondary[dvi_direction]) then
    basepoint_horizontal:=width(leader_box)
  else
    basepoint_horizontal:=0;
  end
else begin
  if not (is_mirrored(box_dir(leader_box))) then
    if dir_eq(dir_primary[box_dir(leader_box)])
             (dir_secondary[dvi_direction]) then
      basepoint_horizontal:=height(leader_box)
    else
      basepoint_horizontal:=depth(leader_box)
  else
    if dir_eq(dir_primary[box_dir(leader_box)])
             (dir_secondary[dvi_direction]) then
      basepoint_horizontal:=depth(leader_box)
    else
      basepoint_horizontal:=height(leader_box);
  if dir_eq(dir_secondary[box_dir(leader_box)])
           (dir_primary[dvi_direction]) then
    basepoint_vertical:= -(width(leader_box) div 2)
  else
    basepoint_vertical:= (width(leader_box) div 2);
  end;
if not (is_mirrored(dvi_direction)) then
  basepoint_vertical := basepoint_vertical + shift_amount(leader_box)
    {shift the box `down'}
else
  basepoint_vertical := basepoint_vertical - shift_amount(leader_box);
    {shift the box `up'}
temp_ptr:=leader_box;
synch_h; save_h:=dvi_h;
cur_h:=cur_h + basepoint_horizontal;
edge_v:=cur_v; cur_v:=base_line + basepoint_vertical;
synch_v; save_v:=dvi_v;
outer_doing_leaders:=doing_leaders; doing_leaders:=true;
if type(leader_box)=vlist_node then vlist_out@+else hlist_out;
doing_leaders:=outer_doing_leaders;
dvi_v:=save_v; dvi_h:=save_h; cur_v:=base_line;
cur_h:=save_h+leader_wd+lx;
end
@z
%-------------------------
@x [32] m.629
@!cur_g:scaled; {rounded equivalent of |cur_glue| times the glue ratio}
begin cur_g:=0; cur_glue:=float_constant(0);
this_box:=temp_ptr; g_order:=glue_order(this_box);
g_sign:=glue_sign(this_box); p:=list_ptr(this_box);
incr(cur_s);
if cur_s>0 then dvi_out(push);
if cur_s>max_push then max_push:=cur_s;
save_loc:=dvi_offset+dvi_ptr; left_edge:=cur_h; cur_v:=cur_v-height(this_box);
top_edge:=cur_v;
while p<>null do @<Output node |p| for |vlist_out| and move to the next node,
  maintaining the condition |cur_h=left_edge|@>;
prune_movements(save_loc);
if cur_s>0 then dvi_pop(save_loc);
decr(cur_s);
@y
@!cur_g:scaled; {rounded equivalent of |cur_glue| times the glue ratio}
@!save_direction: integer;
@!effective_vertical: scaled;
@!basepoint_horizontal: scaled;
@!basepoint_vertical: scaled;
@!edge_v: scaled;
@!saving_v: scaled;
@!saving_h: scaled;
begin cur_g:=0; cur_glue:=float_constant(0);
this_box:=temp_ptr; g_order:=glue_order(this_box);
g_sign:=glue_sign(this_box); p:=list_ptr(this_box);
saving_v:=dvi_v; saving_h:=dvi_h;
save_direction:=dvi_direction;
if dvi_direction<>box_dir(this_box) then begin
  dvi_direction:=box_dir(this_box);
  synch_v; synch_h;
  saving_h:=dvi_h; saving_v:=dvi_v;
  end;
incr(cur_s);
if cur_s>0 then dvi_out(push);
if cur_s>max_push then max_push:=cur_s;
save_loc:=dvi_offset+dvi_ptr; left_edge:=cur_h; cur_v:=cur_v-height(this_box);
top_edge:=cur_v;
while p<>null do @<Output node |p| for |vlist_out| and move to the next node,
  maintaining the condition |cur_h=left_edge|@>;
prune_movements(save_loc);
if cur_s>0 then dvi_pop(save_loc);
decr(cur_s);
if dvi_direction<>save_direction then begin
  dvi_direction:=save_direction;
  synch_v; synch_h;
  cur_h:=saving_h; cur_v:=saving_v;
  end;
@z
%-------------------------
@x [32] m.631
rule_node: begin rule_ht:=height(p); rule_dp:=depth(p); rule_wd:=width(p);
  goto fin_rule;
  end;
@y
rule_node: begin
  if not (dir_orthogonal(dir_primary[rule_dir(p)])
                        (dir_primary[dvi_direction])) then begin
    rule_ht:=height(p); rule_dp:=depth(p); rule_wd:=width(p);
    end
  else begin
    rule_ht:=width(p) div 2;
    rule_dp:=width(p) div 2;
    rule_wd:=height(p)+depth(p);
    end;
  goto fin_rule;
  end;
@z
%-------------------------
@x [32] m.632
@<Output a box in a vlist@>=
if list_ptr(p)=null then cur_v:=cur_v+height(p)+depth(p)
else  begin cur_v:=cur_v+height(p); synch_v;
  save_h:=dvi_h; save_v:=dvi_v;
  cur_h:=left_edge+shift_amount(p); {shift the box right}
  temp_ptr:=p;
  if type(p)=vlist_node then vlist_out@+else hlist_out;
  dvi_h:=save_h; dvi_v:=save_v;
  cur_v:=save_v+depth(p); cur_h:=left_edge;
  end
@y
@<Output a box in a vlist@>=
begin
if not (dir_orthogonal(dir_primary[box_dir(p)])
                      (dir_primary[dvi_direction])) then begin
  effective_vertical:=height(p)+depth(p);
  if (is_mirrored(box_dir(p))) then
    basepoint_vertical:=depth(p)
  else
    basepoint_vertical:=height(p);
  if dir_opposite(dir_secondary[box_dir(p)])
                 (dir_secondary[dvi_direction]) then
    basepoint_horizontal:=width(p)
  else
    basepoint_horizontal:=0;
  end
else begin
  effective_vertical:=width(p);
  if not (is_mirrored(box_dir(p))) then
    if dir_eq(dir_primary[box_dir(p)])
             (dir_secondary[dvi_direction]) then
      basepoint_horizontal:=height(p)
    else
      basepoint_horizontal:=depth(p)
  else
    if dir_eq(dir_primary[box_dir(p)])
             (dir_secondary[dvi_direction]) then
      basepoint_horizontal:=height(p)
    else
      basepoint_horizontal:=depth(p);
  if dir_eq(dir_secondary[box_dir(p)])
           (dir_primary[dvi_direction]) then
    basepoint_vertical:=0
  else
    basepoint_vertical:=width(p);
  end;
basepoint_horizontal := basepoint_horizontal + shift_amount(p);
  {shift the box `right'}
if list_ptr(p)=null then begin
  cur_v:=cur_v+effective_vertical;
  end
else  begin
  edge_v:=cur_v;
  cur_h:=left_edge + basepoint_horizontal;
  cur_v:=cur_v + basepoint_vertical;
  if dvi_direction<>box_dir(p) then begin
    synch_h;
    end;
  synch_v;
  save_h:=dvi_h; save_v:=dvi_v;
  temp_ptr:=p;
  if type(p)=vlist_node then vlist_out@+else hlist_out;
  dvi_h:=save_h; dvi_v:=save_v;
  cur_h:=left_edge; cur_v:=edge_v + effective_vertical;
  end
end
@z
%-------------------------
@x [32] m.633
  dvi_out(put_rule); dvi_four(rule_ht); dvi_four(rule_wd);
@y
  case font_direction(dvi_direction) of
  dir__LT: begin
    dvi_put_rule(rule_ht)(rule_wd);
    end;
  dir__LB: begin
    dvi_down(rule_ht);
    dvi_put_rule(rule_ht)(rule_wd);
    dvi_up(rule_ht);
    end;
  dir__RT: begin
    dvi_left(rule_wd);
    dvi_set_rule(rule_ht)(rule_wd);
    end;
  dir__RB: begin
    dvi_down(rule_ht);
    dvi_left(rule_wd);
    dvi_set_rule(rule_ht)(rule_wd);
    dvi_up(rule_ht);
    end;
  dir__TL: begin
    dvi_down(rule_wd);
    dvi_left(rule_ht);
    dvi_set_rule(rule_wd)(rule_ht);
    dvi_up(rule_wd);
    end;
  dir__TR: begin
    dvi_down(rule_wd);
    dvi_put_rule(rule_wd)(rule_ht);
    dvi_up(rule_wd);
    end;
  dir__BL: begin
    dvi_left(rule_ht);
    dvi_set_rule(rule_wd)(rule_ht);
    end;
  dir__BR: begin
    dvi_put_rule(rule_wd)(rule_ht);
    end;
  end;
@z
%-------------------------
@x [32] m.635
leader_ht:=height(leader_box)+depth(leader_box);
@y
if not (dir_orthogonal(dir_primary[box_dir(leader_box)])
                      (dir_primary[dvi_direction])) then
  leader_ht:=height(leader_box)+depth(leader_box)
else
  leader_ht:=width(leader_box);
@z
%-------------------------
@x [32] m.637
@<Output a leader box at |cur_v|, ...@>=
begin cur_h:=left_edge+shift_amount(leader_box); synch_h; save_h:=dvi_h;@/
cur_v:=cur_v+height(leader_box); synch_v; save_v:=dvi_v;
temp_ptr:=leader_box;
outer_doing_leaders:=doing_leaders; doing_leaders:=true;
if type(leader_box)=vlist_node then vlist_out@+else hlist_out;
doing_leaders:=outer_doing_leaders;
dvi_v:=save_v; dvi_h:=save_h; cur_h:=left_edge;
cur_v:=save_v-height(leader_box)+leader_ht+lx;
end
@y
@<Output a leader box at |cur_v|, ...@>=
begin
if not (dir_orthogonal(dir_primary[box_dir(leader_box)])
                      (dir_primary[dvi_direction])) then begin
  effective_vertical:=height(leader_box)+depth(leader_box);
  if (is_mirrored(box_dir(leader_box))) then
    basepoint_vertical:=depth(leader_box)
  else
    basepoint_vertical:=height(leader_box);
  if dir_opposite(dir_secondary[box_dir(leader_box)])
                 (dir_secondary[dvi_direction]) then
    basepoint_horizontal:=width(leader_box)
  else
    basepoint_horizontal:=0;
  end
else begin
  effective_vertical:=width(leader_box);
  if not (is_mirrored(box_dir(leader_box))) then
    if dir_eq(dir_primary[box_dir(leader_box)])
             (dir_secondary[dvi_direction]) then
      basepoint_horizontal:=height(leader_box)
    else
      basepoint_horizontal:=depth(leader_box)
  else
    if dir_eq(dir_primary[box_dir(leader_box)])
             (dir_secondary[dvi_direction]) then
      basepoint_horizontal:=depth(leader_box)
    else
      basepoint_horizontal:=height(leader_box);
  if dir_eq(dir_secondary[box_dir(leader_box)])
           (dir_primary[dvi_direction]) then
    basepoint_vertical:= width(leader_box)
  else
    basepoint_vertical:= 0;
  end;
basepoint_horizontal := basepoint_horizontal + shift_amount(leader_box);
  {shift the box `right'}
temp_ptr:=leader_box;
edge_v:=cur_v;
cur_h:=left_edge + basepoint_horizontal;
cur_v:=cur_v + basepoint_vertical;
synch_h; synch_v; save_h:=dvi_h; save_v:=dvi_v;
outer_doing_leaders:=doing_leaders; doing_leaders:=true;
if type(leader_box)=vlist_node then vlist_out@+else hlist_out;
doing_leaders:=outer_doing_leaders;
dvi_h:=save_h; dvi_v:=save_v;
cur_h:=left_edge; cur_v:=edge_v+leader_ht+lx;
end
@z
%-------------------------
@x [32] m.640
cur_v:=height(p)+v_offset; temp_ptr:=p;
if type(p)=vlist_node then vlist_out@+else hlist_out;
@y
dvi_direction:=page_direction;
case box_direction(dvi_direction) of
dir_TL_,dir_LT_: begin
  end;
dir_TR_,dir_RT_: begin
  dvi_right(page_right_offset);
  end;
dir_RB_,dir_BR_: begin
  dvi_right(page_right_offset);
  dvi_down(page_bottom_offset);
  end;
dir_BL_,dir_LB_: begin
  dvi_down(page_bottom_offset);
  end;
end;
cur_h:=h_offset;
cur_v:=height(p)+v_offset;
case box_direction(dvi_direction) of
dir_TL_: begin
  dvi_down(cur_v);
  dvi_right(cur_h);
  end;
dir_TR_: begin
  dvi_down(cur_v);
  dvi_right(-cur_h);
  end;
dir_LT_: begin
  dvi_right(cur_v);
  dvi_down(cur_h);
  end;
dir_LB_: begin
  dvi_right(cur_v);
  dvi_down(-cur_h);
  end;
dir_BL_: begin
  dvi_down(-cur_v);
  dvi_right(cur_h);
  end;
dir_BR_: begin
  dvi_down(-cur_v);
  dvi_right(-cur_h);
  end;
dir_RT_: begin
  dvi_right(-cur_v);
  dvi_down(cur_h);
  end;
dir_RB_: begin
  dvi_right(-cur_v);
  dvi_down(-cur_h);
  end;
end;
dvi_h:=cur_h;
dvi_v:=cur_v;
temp_ptr:=p;
if type(p)=vlist_node then vlist_out@+else hlist_out;
@z
%-------------------------
@x [33] m.644
@d exactly=0 {a box dimension is pre-specified}
@d additional=1 {a box dimension is increased from the natural one}
@d natural==0,additional {shorthand for parameters to |hpack| and |vpack|}
@y
@d exactly=0 {a box dimension is pre-specified}
@d additional=1 {a box dimension is increased from the natural one}
@d natural==0,additional {shorthand for parameters to |hpack| and |vpack|}

@<Glob...@>=
spec_direction:integer;

@z
%-------------------------
@x [33] m.645
procedure scan_spec(@!c:group_code;@!three_codes:boolean);
  {scans a box specification and left brace}
label found;
var @!s:integer; {temporarily saved value}
@!spec_code:exactly..additional;
begin if three_codes then s:=saved(0);
if scan_keyword("to") then spec_code:=exactly
@.to@>
else if scan_keyword("spread") then spec_code:=additional
@.spread@>
else  begin spec_code:=additional; cur_val:=0;
  goto found;
  end;
scan_normal_dimen;
found: if three_codes then
  begin saved(0):=s; incr(save_ptr);
  end;
saved(0):=spec_code; saved(1):=cur_val; save_ptr:=save_ptr+2;
new_save_level(c); scan_left_brace;
end;
@y
procedure scan_spec(@!c:group_code;@!four_codes:boolean);
  {scans a box specification and left brace}
label found;
var @!s:integer; {temporarily saved value}
@!spec_code:exactly..additional;
begin if four_codes then s:=saved(0);
if abs(mode)=mmode then
  spec_direction:=math_direction
else
  spec_direction:=text_direction;
if scan_keyword("dir") then begin
  scan_dir; spec_direction:=cur_val;
  end;
if scan_keyword("to") then spec_code:=exactly
@.to@>
else if scan_keyword("spread") then spec_code:=additional
@.spread@>
else  begin spec_code:=additional; cur_val:=0;
  goto found;
  end;
scan_normal_dimen;
found: if four_codes then
  begin saved(0):=s; incr(save_ptr);
  end;
saved(0):=spec_code; saved(1):=cur_val;
saved(2):=spec_direction;
save_ptr:=save_ptr+3;
new_save_level(c); scan_left_brace;
eq_word_define(dir_base+text_direction_code,spec_direction);
make_local_direction(spec_direction,true);
end;
@z
%-------------------------
@x [33] m.649
@p function hpack(@!p:pointer;@!w:scaled;@!m:small_number):pointer;
label reswitch, common_ending, exit;
var r:pointer; {the box node that will be returned}
@!q:pointer; {trails behind |p|}
@!h,@!d,@!x:scaled; {height, depth, and natural width}
@!s:scaled; {shift amount}
@!g:pointer; {points to a glue specification}
@!o:glue_ord; {order of infinity}
@!f:internal_font_number; {the font in a |char_node|}
@!i:four_quarters; {font information about a |char_node|}
@!hd:eight_bits; {height and depth indices for a character}
begin last_badness:=0; r:=get_node(box_node_size); type(r):=hlist_node;
@y
@p function hpack(@!p:pointer;@!w:scaled;@!m:small_number;@!hpack_dir:integer):
pointer;
label reswitch, common_ending, exit;
var r:pointer; {the box node that will be returned}
@!q:pointer; {trails behind |p|}
@!h,@!d,@!x:scaled; {height, depth, and natural width}
@!s:scaled; {shift amount}
@!g:pointer; {points to a glue specification}
@!o:glue_ord; {order of infinity}
@!f:internal_font_number; {the font in a |char_node|}
@!i:four_quarters; {font information about a |char_node|}
@!hd:eight_bits; {height and depth indices for a character}
begin last_badness:=0; r:=get_node(box_node_size); type(r):=hlist_node;
box_dir(r):=hpack_dir;
if (hpack_dir<0) or (hpack_dir>31) then begin
  print_ln; print("hpack_dir="); print_int(hpack_dir); print_ln;
  end;
ptr_info(r):=local_par_info;
info_incr_ref_count(local_par_info);
@z
%-------------------------
@x [33] m.649
exit: hpack:=r;
end;
@y
exit:
{
if is_mirrored(hpack_dir) then begin
  hd:=depth(p); depth(p):=height(p); height(p):=hd;
  end;
}
hpack:=r;
end;
@z
%-------------------------
@x [33] m.653
@<Incorporate box dimensions into the dimensions of the hbox...@>=
begin x:=x+width(p);
if type(p)>=rule_node then s:=0 @+else s:=shift_amount(p);
if height(p)-s>h then h:=height(p)-s;
if depth(p)+s>d then d:=depth(p)+s;
end
@y
@<Incorporate box dimensions into the dimensions of the hbox...@>=
begin
if (type(p)=hlist_node) or (type(p)=vlist_node) then begin
  if dir_orthogonal(dir_primary[hpack_dir])
                   (dir_primary[box_dir(p)]) then begin
    x:=x+height(p)+depth(p);
    s:=shift_amount(p);
    if (width(p) div 2)-s>h then h:=(width(p) div 2)-s;
    if (width(p) div 2)+s>d then d:=(width(p) div 2)+s;
    end
  else if dir_opposite(dir_primary[hpack_dir])
                      (dir_primary[box_dir(p)]) then begin
    x:=x+width(p);
    s:=shift_amount(p);
    if depth(p)-s>h then h:=depth(p)-s;
    if height(p)+s>d then d:=height(p)+s;
    end
  else begin
    x:=x+width(p);
    s:=shift_amount(p);
    if height(p)-s>h then h:=height(p)-s;
    if depth(p)+s>d then d:=depth(p)+s;
    end
  end
else begin
  x:=x+width(p);
  if type(p)>=rule_node then s:=0 @+else s:=shift_amount(p);
  if height(p)-s>h then h:=height(p)-s;
  if depth(p)+s>d then d:=depth(p)+s;
  end;
end
@z
%-------------------------
@x [33] m.654
@<Incorporate character dimensions into the dimensions of the hbox...@>=
begin f:=font(p); i:=char_info(f)(character(p)); hd:=height_depth(i);
x:=x+char_width(f)(i);@/
s:=char_height(f)(hd);@+if s>h then h:=s;
s:=char_depth(f)(hd);@+if s>d then d:=s;
p:=link(p);
end
@y
@<Incorporate character dimensions into the dimensions of the hbox...@>=
begin f:=font(p); i:=char_info(f)(character(p)); hd:=height_depth(i);
if is_rotated(get_dir(p)) then begin
  x:=x+char_height(f)(hd)+char_depth(f)(hd);@/
  s:=char_width(f)(i) div 2;@+if s>h then h:=s;
  s:=char_width(f)(i) div 2;@+if s>d then d:=s;
  end
else if dir_opposite(dir_tertiary[hpack_dir])
                    (dir_tertiary[get_dir(p)]) then begin
  x:=x+char_width(f)(i);@/
  s:=char_depth(f)(hd);@+if s>h then h:=s;
  s:=char_height(f)(hd);@+if s>d then d:=s;
  end
else begin
  x:=x+char_width(f)(i);@/
  s:=char_height(f)(hd);@+if s>h then h:=s;
  s:=char_depth(f)(hd);@+if s>d then d:=s;
  end;
p:=link(p);
end
@z
%-------------------------
@x [33] m.666
@ @<Report an overfull hbox and |goto common_ending|, if...@>=
if (-x-total_shrink[normal]>hfuzz)or(hbadness<100) then
  begin if (overfull_rule>0)and(-x-total_shrink[normal]>hfuzz) then
    begin while link(q)<>null do q:=link(q);
    link(q):=new_rule;
    width(link(q)):=overfull_rule;
    end;
@y
@ @<Report an overfull hbox and |goto common_ending|, if...@>=
if (-x-total_shrink[normal]>hfuzz)or(hbadness<100) then
  begin if (overfull_rule>0)and(-x-total_shrink[normal]>hfuzz) then
    begin while link(q)<>null do q:=link(q);
    link(q):=new_rule; rule_dir(q):=box_dir(r);
    width(link(q)):=overfull_rule;
    end;
@z
%-------------------------
@x [33] m.668
@p function vpackage(@!p:pointer;@!h:scaled;@!m:small_number;@!l:scaled):
  pointer;
label common_ending, exit;
var r:pointer; {the box node that will be returned}
@!w,@!d,@!x:scaled; {width, depth, and natural height}
@!s:scaled; {shift amount}
@!g:pointer; {points to a glue specification}
@!o:glue_ord; {order of infinity}
begin last_badness:=0; r:=get_node(box_node_size); type(r):=vlist_node;
@y
@p function vpackage(@!p:pointer;@!h:scaled;@!m:small_number;@!l:scaled;
@!vpack_dir:integer):pointer;
label common_ending, exit;
var r:pointer; {the box node that will be returned}
@!w,@!d,@!x:scaled; {width, depth, and natural height}
@!s:scaled; {shift amount}
@!g:pointer; {points to a glue specification}
@!o:glue_ord; {order of infinity}
@!dh:scaled;
begin last_badness:=0; r:=get_node(box_node_size); type(r):=vlist_node;
box_dir(r):=vpack_dir;
if (vpack_dir<0) or (vpack_dir>31) then begin
  print_ln; print("vpack_dir="); print_int(vpack_dir); print_ln;
  end;
ptr_info(r):=local_par_info;
info_incr_ref_count(local_par_info);
@z
%-------------------------
@x [33] m.668
exit: vpackage:=r;
end;
@y
exit:
{
if is_mirrored(vpack_dir) then begin
  dh:=depth(r); depth(r):=height(r); height(r):=dh;
  end;
}
vpackage:=r;
end;
@z
%-------------------------
@x [33] m.670
@ @<Incorporate box dimensions into the dimensions of the vbox...@>=
begin x:=x+d+height(p); d:=depth(p);
if type(p)>=rule_node then s:=0 @+else s:=shift_amount(p);
if width(p)+s>w then w:=width(p)+s;
end
@y
@ @<Incorporate box dimensions into the dimensions of the vbox...@>=
begin
if (type(p)=hlist_node) or (type(p)=vlist_node) then begin
  if dir_orthogonal(dir_primary[vpack_dir])
                   (dir_primary[box_dir(p)]) then begin
    x:=x+d+(width(p) div 2); d:=width(p) div 2;
    s:=shift_amount(p);
    if depth(p)+height(p)+s>w then w:=depth(p)+height(p)+s;
    end
  else if dir_opposite(dir_primary[vpack_dir])
                      (dir_primary[box_dir(p)]) then begin
    x:=x+d+depth(p); d:=height(p);
    s:=shift_amount(p);
    if width(p)+s>w then w:=width(p)+s;
    end
  else begin
    x:=x+d+height(p); d:=depth(p);
    s:=shift_amount(p);
    if width(p)+s>w then w:=width(p)+s;
    end
  end  
else
  begin x:=x+d+height(p); d:=depth(p);
  if type(p)>=rule_node then s:=0 @+else s:=shift_amount(p);
  if width(p)+s>w then w:=width(p)+s;
  end;
end
@z
%-------------------------
@x [33] m.679
  begin d:=width(baseline_skip)-prev_depth-height(b);
  if d<line_skip_limit then p:=new_param_glue(line_skip_code)
  else  begin p:=new_skip_param(baseline_skip_code);
    width(temp_ptr):=d; {|temp_ptr=glue_ptr(p)|}
    end;
  link(tail):=p; tail:=p;
  end;
link(tail):=b; tail:=b; prev_depth:=depth(b);
@y
  begin
  if (type(b)=hlist_node) and (is_mirrored(box_dir(b))) then
    d:=width(baseline_skip)-prev_depth-depth(b)
  else
    d:=width(baseline_skip)-prev_depth-height(b);
  if d<line_skip_limit then p:=new_param_glue(line_skip_code)
  else  begin p:=new_skip_param(baseline_skip_code);
    width(temp_ptr):=d; {|temp_ptr=glue_ptr(p)|}
    end;
  link(tail):=p; tail:=p;
  end;
link(tail):=b; tail:=b;
if (type(b)=hlist_node) and (is_mirrored(box_dir(b))) then 
  prev_depth:=height(b)
else
  prev_depth:=depth(b);
@z
%-------------------------
@x [35] m.704
@p function fraction_rule(@!t:scaled):pointer;
  {construct the bar for a fraction}
var p:pointer; {the new node}
begin p:=new_rule; height(p):=t; depth(p):=0; fraction_rule:=p;
end;
@y
@p function fraction_rule(@!t:scaled):pointer;
  {construct the bar for a fraction}
var p:pointer; {the new node}
begin p:=new_rule; rule_dir(p):=info(math_dir_stack);
height(p):=t; depth(p):=0; fraction_rule:=p;
end;
@z
%-------------------------
@x [35] m.705
@p function overbar(@!b:pointer;@!k,@!t:scaled):pointer;
var p,@!q:pointer; {nodes being constructed}
begin p:=new_kern(k); link(p):=b; q:=fraction_rule(t); link(q):=p;
p:=new_kern(t); link(p):=q; overbar:=vpack(p,natural);
end;
@y
@p function overbar(@!b:pointer;@!k,@!t:scaled):pointer;
var p,@!q:pointer; {nodes being constructed}
begin p:=new_kern(k); link(p):=b; q:=fraction_rule(t); link(q):=p;
p:=new_kern(t); link(p):=q;
overbar:=vpackage(p,natural,max_dimen,info(math_dir_stack));
end;
@z
%-------------------------
@x [36] m.706
else  begin b:=new_null_box;
@y
else  begin b:=new_null_box; box_dir(b):=info(math_dir_stack);
@z
%-------------------------
@x [36] m.709
function char_box(@!f:internal_font_number;@!c:quarterword):pointer;
var q:four_quarters;
@!hd:eight_bits; {|height_depth| byte}
@!b,@!p:pointer; {the new box and its character node}
begin q:=char_info(f)(c); hd:=height_depth(q);
b:=new_null_box; width(b):=char_width(f)(q)+char_italic(f)(q);
height(b):=char_height(f)(hd); depth(b):=char_depth(f)(hd);
p:=get_avail; character(p):=c; font(p):=f; list_ptr(b):=p; char_box:=b;
end;
@y
function char_box(@!f:internal_font_number;@!c:quarterword):pointer;
var q:four_quarters;
@!hd:eight_bits; {|height_depth| byte}
@!b,@!p:pointer; {the new box and its character node}
begin q:=char_info(f)(c); hd:=height_depth(q);
b:=new_null_box; width(b):=char_width(f)(q)+char_italic(f)(q);
box_dir(b):=info(math_dir_stack);
height(b):=char_height(f)(hd); depth(b):=char_depth(f)(hd);
p:=get_avail; character(p):=c; font(p):=f; list_ptr(b):=p;
ptr_info(p):=local_par_info; info_incr_ref_count(local_par_info);
char_box:=b;
end;
@z
%-------------------------
@x [36] m.713
@ @<Construct an extensible...@>=
begin b:=new_null_box;
@y
@ @<Construct an extensible...@>=
begin b:=new_null_box;
box_dir(b):=info(math_dir_stack);
@z
%-------------------------
@x [36] m.715
@p function rebox(@!b:pointer;@!w:scaled):pointer;
var p:pointer; {temporary register for list manipulation}
@!f:internal_font_number; {font in a one-character box}
@!v:scaled; {width of a character without italic correction}
begin if (width(b)<>w)and(list_ptr(b)<>null) then
  begin if type(b)=vlist_node then b:=hpack(b,natural);
  p:=list_ptr(b);
  if (is_char_node(p))and(link(p)=null) then
    begin f:=font(p); v:=char_width(f)(char_info(f)(character(p)));
    if v<>width(b) then link(p):=new_kern(width(b)-v);
    end;
  free_node(b,box_node_size);
  b:=new_glue(ss_glue); link(b):=p;
  while link(p)<>null do p:=link(p);
  link(p):=new_glue(ss_glue);
  rebox:=hpack(b,w,exactly);
  end
else  begin width(b):=w; rebox:=b;
  end;
end;
@y
@p function rebox(@!b:pointer;@!w:scaled):pointer;
var p:pointer; {temporary register for list manipulation}
@!f:internal_font_number; {font in a one-character box}
@!v:scaled; {width of a character without italic correction}
begin if (width(b)<>w)and(list_ptr(b)<>null) then
  begin if type(b)=vlist_node then b:=hpack(b,natural,info(math_dir_stack));
  p:=list_ptr(b);
  if (is_char_node(p))and(link(p)=null) then
    begin f:=font(p); v:=char_width(f)(char_info(f)(character(p)));
    if v<>width(b) then link(p):=new_kern(width(b)-v);
    end;
  free_node(b,box_node_size);
  b:=new_glue(ss_glue); link(b):=p;
  while link(p)<>null do p:=link(p);
  link(p):=new_glue(ss_glue);
  rebox:=hpack(b,w,exactly,info(math_dir_stack));
  end
else  begin width(b):=w; rebox:=b;
  end;
end;
@z
%-------------------------
@x [36] m.720
function clean_box(@!p:pointer;@!s:integer):pointer;
label found;
var q:pointer; {beginning of a list to be boxed}
@!save_style:integer; {|cur_style| to be restored}
@!x:pointer; {box to be returned}
@!r:pointer; {temporary pointer}
begin case math_type(p) of
math_char: begin cur_mlist:=new_noad; mem[nucleus(cur_mlist)]:=mem[p];
  end;
sub_box: begin q:=info(p); goto found;
  end;
sub_mlist: cur_mlist:=info(p);
othercases begin q:=new_null_box; goto found;
  end
endcases;@/
save_style:=cur_style; cur_style:=s; mlist_penalties:=false;@/
mlist_to_hlist; q:=link(temp_head); {recursive call}
cur_style:=save_style; {restore the style}
@<Set up the values of |cur_size| and |cur_mu|, based on |cur_style|@>;
found: if is_char_node(q)or(q=null) then x:=hpack(q,natural)
  else if (link(q)=null)and(type(q)<=vlist_node)and(shift_amount(q)=0) then
    x:=q {it's already clean}
  else x:=hpack(q,natural);
@<Simplify a trivial box@>;
clean_box:=x;
end;
@y
function clean_box(@!p:pointer;@!s:integer):pointer;
label found;
var q:pointer; {beginning of a list to be boxed}
@!save_style:integer; {|cur_style| to be restored}
@!x:pointer; {box to be returned}
@!r:pointer; {temporary pointer}
begin case math_type(p) of
math_char: begin cur_mlist:=new_noad; mem[nucleus(cur_mlist)]:=mem[p];
  end;
sub_box: begin q:=info(p); goto found;
  end;
sub_mlist: cur_mlist:=info(p);
othercases begin q:=new_null_box; box_dir(q):=info(math_dir_stack); goto found;
  end
endcases;@/
save_style:=cur_style; cur_style:=s; mlist_penalties:=false;@/
mlist_to_hlist; q:=link(temp_head); {recursive call}
cur_style:=save_style; {restore the style}
@<Set up the values of |cur_size| and |cur_mu|, based on |cur_style|@>;
found: if is_char_node(q)or(q=null) then
    x:=hpack(q,natural,info(math_dir_stack))
  else if (link(q)=null)and(type(q)<=vlist_node)and(shift_amount(q)=0) then
    x:=q {it's already clean}
  else x:=hpack(q,natural,info(math_dir_stack));
@<Simplify a trivial box@>;
clean_box:=x;
end;
@z
%-------------------------
@x [36] m.727
@<Process node-or-noad...@>=
begin @<Do first-pass processing based on |type(q)|; |goto done_with_noad|
  if a noad has been fully processed, |goto check_dimensions| if it
  has been translated into |new_hlist(q)|, or |goto done_with_node|
  if a node has been fully processed@>;
check_dimensions: z:=hpack(new_hlist(q),natural);
if height(z)>max_h then max_h:=height(z);
if depth(z)>max_d then max_d:=depth(z);
free_node(z,box_node_size);
done_with_noad: r:=q; r_type:=type(r);
done_with_node: q:=link(q);
end
@y
@<Process node-or-noad...@>=
begin @<Do first-pass processing based on |type(q)|; |goto done_with_noad|
  if a noad has been fully processed, |goto check_dimensions| if it
  has been translated into |new_hlist(q)|, or |goto done_with_node|
  if a node has been fully processed@>;
check_dimensions: z:=hpack(new_hlist(q),natural,info(math_dir_stack));
if height(z)>max_h then max_h:=height(z);
if depth(z)>max_d then max_d:=depth(z);
free_node(z,box_node_size);
done_with_noad: r:=q; r_type:=type(r);
done_with_node: q:=link(q);
end
@z
%-------------------------
@x [36] m.735
procedure make_under(@!q:pointer);
var p,@!x,@!y: pointer; {temporary registers for box construction}
@!delta:scaled; {overall height plus depth}
begin x:=clean_box(nucleus(q),cur_style);
p:=new_kern(3*default_rule_thickness); link(x):=p;
link(p):=fraction_rule(default_rule_thickness);
y:=vpack(x,natural);
delta:=height(y)+depth(y)+default_rule_thickness;
height(y):=height(x); depth(y):=delta-height(y);
info(nucleus(q)):=y; math_type(nucleus(q)):=sub_box;
end;
@y
procedure make_under(@!q:pointer);
var p,@!x,@!y: pointer; {temporary registers for box construction}
@!delta:scaled; {overall height plus depth}
begin x:=clean_box(nucleus(q),cur_style);
p:=new_kern(3*default_rule_thickness); link(x):=p;
link(p):=fraction_rule(default_rule_thickness);
y:=vpackage(x,natural,max_dimen,info(math_dir_stack));
delta:=height(y)+depth(y)+default_rule_thickness;
height(y):=height(x); depth(y):=delta-height(y);
info(nucleus(q)):=y; math_type(nucleus(q)):=sub_box;
end;
@z
%-------------------------
@x [36] m.737
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
delta:=depth(y)-(height(x)+depth(x)+clr);
if delta>0 then clr:=clr+half(delta); {increase the actual clearance}
shift_amount(y):=-(height(x)+clr);
link(y):=overbar(x,clr,height(y));
info(nucleus(q)):=hpack(y,natural); math_type(nucleus(q)):=sub_box;
end;
@y
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
delta:=depth(y)-(height(x)+depth(x)+clr);
if delta>0 then clr:=clr+half(delta); {increase the actual clearance}
shift_amount(y):=-(height(x)+clr);
link(y):=overbar(x,clr,height(y));
info(nucleus(q)):=hpack(y,natural,info(math_dir_stack));
math_type(nucleus(q)):=sub_box;
end;
@z
%-------------------------
@x [36] m.738
  y:=vpack(y,natural); width(y):=width(x);
@y
  y:=vpackage(y,natural,max_dimen,info(math_dir_stack)); width(y):=width(x);
@z
%-------------------------
@x [36] m.747
@ @<Construct a vlist box for the fraction...@>=
v:=new_null_box; type(v):=vlist_node;
@y
@ @<Construct a vlist box for the fraction...@>=
v:=new_null_box; box_dir(v):=info(math_dir_stack); type(v):=vlist_node;
@z
%-------------------------
@x [36] m.748
@ @<Put the \(f)fraction into a box with its delimiters...@>=
if cur_style<text_style then delta:=delim1(cur_size)
else delta:=delim2(cur_size);
x:=var_delimiter(left_delimiter(q), cur_size, delta); link(x):=v;@/
z:=var_delimiter(right_delimiter(q), cur_size, delta); link(v):=z;@/
new_hlist(q):=hpack(x,natural)
@y
@ @<Put the \(f)fraction into a box with its delimiters...@>=
if cur_style<text_style then delta:=delim1(cur_size)
else delta:=delim2(cur_size);
x:=var_delimiter(left_delimiter(q), cur_size, delta); link(x):=v;@/
z:=var_delimiter(right_delimiter(q), cur_size, delta); link(v):=z;@/
new_hlist(q):=hpack(x,natural,info(math_dir_stack))
@z
%-------------------------
@x [36] m.750
@<Construct a box with limits above and below it...@>=
begin x:=clean_box(supscr(q),sup_style(cur_style));
y:=clean_box(nucleus(q),cur_style);
z:=clean_box(subscr(q),sub_style(cur_style));
v:=new_null_box; type(v):=vlist_node; width(v):=width(y);
@y
@<Construct a box with limits above and below it...@>=
begin x:=clean_box(supscr(q),sup_style(cur_style));
y:=clean_box(nucleus(q),cur_style);
z:=clean_box(subscr(q),sub_style(cur_style));
v:=new_null_box; box_dir(v):=info(math_dir_stack);
type(v):=vlist_node; width(v):=width(y);
@z
%-------------------------
@x [36] m.754
@<Convert \(n)|nucleus(q)| to an hlist and attach the sub/superscripts@>=
case math_type(nucleus(q)) of
math_char, math_text_char:
  @<Create a character node |p| for |nucleus(q)|, possibly followed
  by a kern node for the italic correction, and set |delta| to the
  italic correction if a subscript is present@>;
empty: p:=null;
sub_box: p:=info(nucleus(q));
sub_mlist: begin cur_mlist:=info(nucleus(q)); save_style:=cur_style;
  mlist_penalties:=false; mlist_to_hlist; {recursive call}
@^recursion@>
  cur_style:=save_style; @<Set up the values...@>;
  p:=hpack(link(temp_head),natural);
  end;
@y
@<Convert \(n)|nucleus(q)| to an hlist and attach the sub/superscripts@>=
case math_type(nucleus(q)) of
math_char, math_text_char:
  @<Create a character node |p| for |nucleus(q)|, possibly followed
  by a kern node for the italic correction, and set |delta| to the
  italic correction if a subscript is present@>;
empty: p:=null;
sub_box: p:=info(nucleus(q));
sub_mlist: begin cur_mlist:=info(nucleus(q)); save_style:=cur_style;
  mlist_penalties:=false; mlist_to_hlist; {recursive call}
@^recursion@>
  cur_style:=save_style; @<Set up the values...@>;
  p:=hpack(link(temp_head),natural,info(math_dir_stack));
  end;
@z
%-------------------------
@x [36] m.756
procedure make_scripts(@!q:pointer;@!delta:scaled);
var p,@!x,@!y,@!z:pointer; {temporary registers for box construction}
@!shift_up,@!shift_down,@!clr:scaled; {dimensions in the calculation}
@!t:integer; {subsidiary size code}
begin p:=new_hlist(q);
if is_char_node(p) then
  begin shift_up:=0; shift_down:=0;
  end
else  begin z:=hpack(p,natural);
@y
procedure make_scripts(@!q:pointer;@!delta:scaled);
var p,@!x,@!y,@!z:pointer; {temporary registers for box construction}
@!shift_up,@!shift_down,@!clr:scaled; {dimensions in the calculation}
@!t:integer; {subsidiary size code}
begin p:=new_hlist(q);
if is_char_node(p) then
  begin shift_up:=0; shift_down:=0;
  end
else  begin z:=hpack(p,natural,info(math_dir_stack));
@z
%-------------------------
@x [36] m.759
x:=vpack(x,natural); shift_amount(x):=shift_down;
@y
x:=vpackage(x,natural,max_dimen,info(math_dir_stack));
shift_amount(x):=shift_down;
@z
%-------------------------
@x [37] m.774
procedure init_align;
label done, done1, done2, continue;
var save_cs_ptr:pointer; {|warning_index| value for error messages}
@!p:pointer; {for short-term temporary use}
begin save_cs_ptr:=cur_cs; {\.{\\halign} or \.{\\valign}, usually}
push_alignment; align_state:=-1000000; {enter a new alignment level}
@<Check for improper alignment in displayed math@>;
push_nest; {enter a new semantic level}
@<Change current mode to |-vmode| for \.{\\halign}, |-hmode| for \.{\\valign}@>;
scan_spec(align_group,false);@/
@<Scan the preamble and record it in the |preamble| list@>;
new_save_level(align_group);
@y
procedure init_align;
label done, done1, done2, continue;
var save_cs_ptr:pointer; {|warning_index| value for error messages}
@!p:pointer; {for short-term temporary use}
begin save_cs_ptr:=cur_cs; {\.{\\halign} or \.{\\valign}, usually}
push_alignment; align_state:=-1000000; {enter a new alignment level}
@<Check for improper alignment in displayed math@>;
push_nest; {enter a new semantic level}
@<Change current mode to |-vmode| for \.{\\halign}, |-hmode| for \.{\\valign}@>;
scan_spec(align_group,false);@/
@<Scan the preamble and record it in the |preamble| list@>;
new_save_level(align_group);
tmp_dir_stack:=get_avail; info(tmp_dir_stack):=spec_direction;
link(tmp_dir_stack):=align_dir_stack;
align_dir_stack:=tmp_dir_stack;
@z
%-------------------------
@x [37] m.779
@ @<Scan preamble text until |cur_cmd| is |tab_mark| or |car_ret|...@>=
@<Scan the template \<u_j>, putting the resulting token list in |hold_head|@>;
link(cur_align):=new_null_box; cur_align:=link(cur_align); {a new alignrecord}
info(cur_align):=end_span; width(cur_align):=null_flag;
u_part(cur_align):=link(hold_head);
@<Scan the template \<v_j>, putting the resulting token list in |hold_head|@>;
v_part(cur_align):=link(hold_head)
@y
@ @<Scan preamble text until |cur_cmd| is |tab_mark| or |car_ret|...@>=
@<Scan the template \<u_j>, putting the resulting token list in |hold_head|@>;
link(cur_align):=new_null_box; cur_align:=link(cur_align); {a new alignrecord}
box_dir(cur_align):=info(align_dir_stack);
info(cur_align):=end_span; width(cur_align):=null_flag;
u_part(cur_align):=link(hold_head);
@<Scan the template \<v_j>, putting the resulting token list in |hold_head|@>;
v_part(cur_align):=link(hold_head)
@z
%-------------------------
@x [37] m.793
@ @<Lengthen the preamble...@>=
begin link(q):=new_null_box; p:=link(q); {a new alignrecord}
info(p):=end_span; width(p):=null_flag; cur_loop:=link(cur_loop);
@<Copy the templates from node |cur_loop| into node |p|@>;
cur_loop:=link(cur_loop);
link(p):=new_glue(glue_ptr(cur_loop));
end
@y
@ @<Lengthen the preamble...@>=
begin link(q):=new_null_box; p:=link(q); {a new alignrecord}
box_dir(p):=info(align_dir_stack);
info(p):=end_span; width(p):=null_flag; cur_loop:=link(cur_loop);
@<Copy the templates from node |cur_loop| into node |p|@>;
cur_loop:=link(cur_loop);
link(p):=new_glue(glue_ptr(cur_loop));
end
@z
%-------------------------
@x [37] m.796
@ @<Package an unset...@>=
begin if mode=-hmode then
  begin adjust_tail:=cur_tail; u:=hpack(link(head),natural); w:=width(u);
  cur_tail:=adjust_tail; adjust_tail:=null;
  end
else  begin u:=vpackage(link(head),natural,0); w:=height(u);
  end;
@y
@ @<Package an unset...@>=
begin if mode=-hmode then
  begin adjust_tail:=cur_tail;
  u:=hpack(link(head),natural,info(align_dir_stack));
  w:=width(u);
  cur_tail:=adjust_tail; adjust_tail:=null;
  end
else  begin u:=vpackage(link(head),natural,0,info(align_dir_stack));
  w:=height(u);
  end;
@z
%-------------------------
@x [37] m.799
@p procedure fin_row;
var p:pointer; {the new unset box}
begin if mode=-hmode then
  begin p:=hpack(link(head),natural);
  pop_nest; append_to_vlist(p);
  if cur_head<>cur_tail then
    begin link(tail):=link(cur_head); tail:=cur_tail;
    end;
  end
else  begin p:=vpack(link(head),natural); pop_nest;
  link(tail):=p; tail:=p; space_factor:=1000;
  end;
@y
@p procedure fin_row;
var p:pointer; {the new unset box}
begin if mode=-hmode then
  begin p:=hpack(link(head),natural,info(align_dir_stack));
  pop_nest; append_to_vlist(p);
  if cur_head<>cur_tail then
    begin link(tail):=link(cur_head); tail:=cur_tail;
    end;
  end
else  begin p:=vpackage(link(head),natural,max_dimen,info(align_dir_stack));
  pop_nest; link(tail):=p; tail:=p; space_factor:=1000;
  end;
@z
%-------------------------
@x [37] m.800
flush_node_list(p); pop_alignment;
@<Insert the \(c)current list into its environment@>;
@y
flush_node_list(p); pop_alignment;
tmp_dir_stack:=align_dir_stack;
align_dir_stack:=link(align_dir_stack);
free_avail(tmp_dir_stack);
@<Insert the \(c)current list into its environment@>;
@z
%-------------------------
@x [37] m.804
@<Package the preamble list...@>=
save_ptr:=save_ptr-2; pack_begin_line:=-mode_line;
if mode=-vmode then
  begin rule_save:=overfull_rule;
  set_new_eqtb_sc(dimen_base+overfull_rule_code,0);
      {prevent rule from being packaged}
  p:=hpack(preamble,saved(1),saved(0));
  set_new_eqtb_sc(dimen_base+overfull_rule_code,rule_save);
  end
else  begin q:=link(preamble);
  repeat height(q):=width(q); width(q):=0; q:=link(link(q));
  until q=null;
  p:=vpack(preamble,saved(1),saved(0));
  q:=link(preamble);
  repeat width(q):=height(q); height(q):=0; q:=link(link(q));
  until q=null;
  end;
@y
@<Package the preamble list...@>=
save_ptr:=save_ptr-3; pack_begin_line:=-mode_line;
if mode=-vmode then
  begin rule_save:=overfull_rule;
  set_new_eqtb_sc(dimen_base+overfull_rule_code,0);
      {prevent rule from being packaged}
  p:=hpack(preamble,saved(1),saved(0),saved(2));
  set_new_eqtb_sc(dimen_base+overfull_rule_code,rule_save);
  end
else  begin q:=link(preamble);
  repeat height(q):=width(q); width(q):=0; q:=link(link(q));
  until q=null;
  p:=vpackage(preamble,saved(1),saved(0),max_dimen,saved(2));
  q:=link(preamble);
  repeat width(q):=height(q); height(q):=0; q:=link(link(q));
  until q=null;
  end;
@z
%-------------------------
@x [37] m.806
@ @<Make the running dimensions in rule |q| extend...@>=
begin if is_running(width(q)) then width(q):=width(p);
if is_running(height(q)) then height(q):=height(p);
if is_running(depth(q)) then depth(q):=depth(p);
if o<>0 then
  begin r:=link(q); link(q):=null; q:=hpack(q,natural);
  shift_amount(q):=o; link(q):=r; link(s):=q;
  end;
end
@y
@ @<Make the running dimensions in rule |q| extend...@>=
begin if is_running(width(q)) then width(q):=width(p);
if is_running(height(q)) then height(q):=height(p);
if is_running(depth(q)) then depth(q):=depth(p);
if o<>0 then
  begin r:=link(q); link(q):=null; q:=hpack(q,natural,info(align_dir_stack));
  shift_amount(q):=o; link(q):=r; link(s):=q;
  end;
end
@z
%-------------------------
@x [37] m.809
@ @<Append tabskip glue and an empty box to list |u|...@>=
s:=link(s); v:=glue_ptr(s); link(u):=new_glue(v); u:=link(u);
subtype(u):=tab_skip_code+1; t:=t+width(v);
if glue_sign(p)=stretching then
  begin if stretch_order(v)=glue_order(p) then
    t:=t+round(float(glue_set(p))*stretch(v));
@^real multiplication@>
  end
else if glue_sign(p)=shrinking then
  begin if shrink_order(v)=glue_order(p) then
    t:=t-round(float(glue_set(p))*shrink(v));
  end;
s:=link(s); link(u):=new_null_box; u:=link(u); t:=t+width(s);
if mode=-vmode then width(u):=width(s)@+else
  begin type(u):=vlist_node; height(u):=width(s);
  end
@y
@ @<Append tabskip glue and an empty box to list |u|...@>=
s:=link(s); v:=glue_ptr(s); link(u):=new_glue(v); u:=link(u);
subtype(u):=tab_skip_code+1; t:=t+width(v);
if glue_sign(p)=stretching then
  begin if stretch_order(v)=glue_order(p) then
    t:=t+round(float(glue_set(p))*stretch(v));
@^real multiplication@>
  end
else if glue_sign(p)=shrinking then
  begin if shrink_order(v)=glue_order(p) then
    t:=t-round(float(glue_set(p))*shrink(v));
  end;
s:=link(s); link(u):=new_null_box; u:=link(u);
box_dir(u):=info(align_dir_stack);
t:=t+width(s);
if mode=-vmode then width(u):=width(s)@+else
  begin type(u):=vlist_node; height(u):=width(s);
  end
@z
%-------------------------
@x [38] m.814
@!init_local_par_info:pointer;
@!last_local_par_info:pointer;
@!current_file_node:pointer;

@ @<Set initial...@>=
@y
@!init_local_par_info:pointer;
@!last_local_par_info:pointer;
@!current_file_node:pointer;
@!break_c_htdp:quarterword; {height-depth entry in |char_info|}
@!next_fake_math_flag:integer;
@!line_break_dir:pointer;

@ @<Set initial...@>=
next_fake_math_flag:=0;
@z
%-------------------------
@x [38] m.815
@p@t\4@>@<Declare subprocedures for |line_break|@>
procedure line_break(@!final_widow_penalty:integer);
@y
@p@t\4@>@<Declare subprocedures for |line_break|@>
procedure line_break(@!final_widow_penalty:integer;@!break_dir:integer);
@z
%-------------------------
@x [38] m.816
link(tail):=new_param_glue(par_fill_skip_code);
@y
final_par_glue:=new_param_glue(par_fill_skip_code);
link(tail):=final_par_glue;
change_dir(final_par_glue,break_dir);
line_break_dir:=break_dir;
@z
%-------------------------
@x [38] m.816
init_cur_lang:=prev_graf mod @'200000;
init_l_hyf:=prev_graf div @'20000000;
init_r_hyf:=(prev_graf div @'200000) mod @'100;
@y
init_cur_lang:=prev_graf mod @'200000;
init_l_hyf:=prev_graf div @'20000000;
init_r_hyf:=(prev_graf div @'200000) mod @'100;
@z
%-------------------------
@x [39] m.821
@!pass_number:halfword; {the number of passive nodes allocated on this pass}
@y
@!pass_number:halfword; {the number of passive nodes allocated on this pass}
@!final_par_glue:pointer;
@z
%-------------------------
@x [38] m.841
@<Subtract the width of node |v|...@>=
if is_char_node(v) then
  begin f:=font(v);
  break_width[1]:=break_width[1]-char_width(f)(char_info(f)(character(v)));
  end
else  case type(v) of
  ligature_node: begin f:=font(lig_char(v));@/
    break_width[1]:=@|break_width[1]-
      char_width(f)(char_info(f)(character(lig_char(v))));
    end;
  hlist_node,vlist_node,rule_node,kern_node:
    break_width[1]:=break_width[1]-width(v);
@y
@<Subtract the width of node |v|...@>=
if is_char_node(v) then
  begin f:=font(v);
  if is_rotated(get_dir(v)) then begin
    break_c_htdp:=height_depth(char_info(f)(character(v)));
    break_width[1]:=break_width[1]-char_height(f)(break_c_htdp)
                                  -char_depth(f)(break_c_htdp);
    end
  else
    break_width[1]:=break_width[1]-char_width(f)(char_info(f)(character(v)));
  end
else  case type(v) of
  ligature_node: begin f:=font(lig_char(v));@/
   if is_rotated(get_dir(v)) then begin
     break_c_htdp:=height_depth(char_info(f)(character(lig_char(v))));
     break_width[1]:=break_width[1]-char_height(f)(break_c_htdp)
                                   -char_depth(f)(break_c_htdp);
     end
   else
     break_width[1]:=@|break_width[1]-
       char_width(f)(char_info(f)(character(lig_char(v))));
   end;
  hlist_node,vlist_node:
    if not (dir_orthogonal(dir_primary[box_dir(v)])
                          (dir_primary[line_break_dir])) then
      break_width[1]:=break_width[1]-width(v)
    else
      break_width[1]:=break_width[1]-(depth(v)+height(v));
  rule_node,kern_node:
    break_width[1]:=break_width[1]-width(v);
@z
%-------------------------
@x [38] m.842
@ @<Add the width of node |s| to |b...@>=
if is_char_node(s) then
  begin f:=font(s);
  break_width[1]:=@|break_width[1]+char_width(f)(char_info(f)(character(s)));
  end
else  case type(s) of
  ligature_node: begin f:=font(lig_char(s));
    break_width[1]:=break_width[1]+
      char_width(f)(char_info(f)(character(lig_char(s))));
    end;
  hlist_node,vlist_node,rule_node,kern_node:
    break_width[1]:=break_width[1]+width(s);
@y
@ @<Add the width of node |s| to |b...@>=
if is_char_node(s) then
  begin f:=font(s);
  if is_rotated(get_dir(s)) then begin
    break_c_htdp:=height_depth(char_info(f)(character(s)));
    break_width[1]:=break_width[1]+char_height(f)(break_c_htdp)
                                  +char_depth(f)(break_c_htdp);
    end
  else
    break_width[1]:=@|break_width[1]+char_width(f)(char_info(f)(character(s)));
  end
else  case type(s) of
  ligature_node: begin f:=font(lig_char(s));
   if is_rotated(get_dir(s)) then begin
     break_c_htdp:=height_depth(char_info(f)(character(lig_char(s))));
     break_width[1]:=break_width[1]+char_height(f)(break_c_htdp)
                                   +char_depth(f)(break_c_htdp);
     end
   else
     break_width[1]:=break_width[1]+
       char_width(f)(char_info(f)(character(lig_char(s))));
   end;
  hlist_node,vlist_node:
    if not (dir_orthogonal(dir_primary[box_dir(s)])
                          (dir_primary[line_break_dir])) then
      break_width[1]:=break_width[1]+width(s)
    else
      break_width[1]:=break_width[1]+(depth(s)+height(s));
  rule_node,kern_node:
    break_width[1]:=break_width[1]+width(s);
@z
%-------------------------
@x [38] m.866
hlist_node,vlist_node,rule_node: act_width:=act_width+width(cur_p);
@y
hlist_node,vlist_node:
if not (dir_orthogonal(dir_primary[box_dir(cur_p)])
                      (dir_primary[line_break_dir]))
then act_width:=act_width+width(cur_p)
else act_width:=act_width+(depth(cur_p)+height(cur_p));
rule_node: act_width:=act_width+width(cur_p);
@z
%-------------------------
@x [38] m.866
  act_width:=act_width+char_width(f)(char_info(f)(character(lig_char(cur_p))));
@y
  if is_rotated(get_dir(cur_p)) then begin
    break_c_htdp:=height_depth(char_info(f)(character(lig_char(cur_p))));
    act_width:=act_width+char_height(f)(break_c_htdp)
                        +char_depth(f)(break_c_htdp);
    end
  else
    act_width:=act_width+
               char_width(f)(char_info(f)(character(lig_char(cur_p))));
@z
%-------------------------
@x [38] m.867
act_width:=act_width+char_width(f)(char_info(f)(character(cur_p)));
@y
if is_rotated(get_dir(cur_p)) then begin
    break_c_htdp:=height_depth(char_info(f)(character(cur_p)));
    act_width:=act_width+char_height(f)(break_c_htdp)
                        +char_depth(f)(break_c_htdp);
  end
else
  act_width:=act_width+char_width(f)(char_info(f)(character(cur_p)));
@z
%-------------------------
@x [38] m.870
@ @<Add the width of node |s| to |disc_width|@>=
if is_char_node(s) then
  begin f:=font(s);
  disc_width:=disc_width+char_width(f)(char_info(f)(character(s)));
  end
else  case type(s) of
  ligature_node: begin f:=font(lig_char(s));
    disc_width:=disc_width+
      char_width(f)(char_info(f)(character(lig_char(s))));
    end;
  hlist_node,vlist_node,rule_node,kern_node:
    disc_width:=disc_width+width(s);
@y
@ @<Add the width of node |s| to |disc_width|@>=
if is_char_node(s) then
  begin f:=font(s);
  if is_rotated(get_dir(s)) then begin
      break_c_htdp:=height_depth(char_info(f)(character(s)));
      disc_width:=disc_width+char_height(f)(break_c_htdp)
                            +char_depth(f)(break_c_htdp);
    end
  else
    disc_width:=disc_width+char_width(f)(char_info(f)(character(s)));
  end
else  case type(s) of
  ligature_node: begin f:=font(lig_char(s));
    if is_rotated(get_dir(s)) then begin
        break_c_htdp:=height_depth(char_info(f)(character(lig_char(s))));
        disc_width:=disc_width+char_height(f)(break_c_htdp)
                              +char_depth(f)(break_c_htdp);
      end
    else
      disc_width:=disc_width+
        char_width(f)(char_info(f)(character(lig_char(s))));
    end;
  hlist_node,vlist_node:
   if not (dir_orthogonal(dir_primary[box_dir(s)])
                         (dir_primary[line_break_dir]))
    then disc_width:=disc_width+width(s)
    else disc_width:=disc_width+(depth(s)+height(s));
  rule_node,kern_node:
    disc_width:=disc_width+width(s);
@z
%-------------------------
@x [38] m.871
@ @<Add the width of node |s| to |act_width|@>=
if is_char_node(s) then
  begin f:=font(s);
  act_width:=act_width+char_width(f)(char_info(f)(character(s)));
  end
else  case type(s) of
  ligature_node: begin f:=font(lig_char(s));
    act_width:=act_width+
      char_width(f)(char_info(f)(character(lig_char(s))));
    end;
  hlist_node,vlist_node,rule_node,kern_node:
    act_width:=act_width+width(s);
@y
@ @<Add the width of node |s| to |act_width|@>=
if is_char_node(s) then
  begin f:=font(s);
  if is_rotated(get_dir(s)) then begin
      break_c_htdp:=height_depth(char_info(f)(character(s)));
      act_width:=act_width+char_height(f)(break_c_htdp)
                          +char_depth(f)(break_c_htdp);
    end
  else
    act_width:=act_width+char_width(f)(char_info(f)(character(s)));
  end
else  case type(s) of
  ligature_node: begin f:=font(lig_char(s));
    if is_rotated(get_dir(s)) then begin
        break_c_htdp:=height_depth(char_info(f)(character(lig_char(s))));
        act_width:=act_width+char_height(f)(break_c_htdp)
                            +char_depth(f)(break_c_htdp);
      end
    else
      act_width:=act_width+
        char_width(f)(char_info(f)(character(lig_char(s))));
    end;
  hlist_node,vlist_node:
   if not (dir_orthogonal(dir_primary[box_dir(s)])
                         (dir_primary[line_break_dir]))
    then act_width:=act_width+width(s)
    else act_width:=act_width+(depth(s)+height(s));
  rule_node,kern_node:
    act_width:=act_width+width(s);
@z
%-------------------------
@x [39] m.877
@ The total number of lines that will be set by |post_line_break|
is |best_line-prev_graf-1|. The last breakpoint is specified by
|break_node(best_bet)|, and this passive node points to the other breakpoints
via the |prev_break| links. The finishing-up phase starts by linking the
relevant passive nodes in forward order, changing |prev_break| to
|next_break|. (The |next_break| fields actually reside in the same memory
space as the |prev_break| fields did, but we give them a new name because
of their new significance.) Then the lines are justified, one by one.

@d next_break==prev_break {new name for |prev_break| after links are reversed}

@<Declare subprocedures for |line_break|@>=
procedure post_line_break(@!final_widow_penalty:integer);
label done,done1;
var q,@!r,@!s:pointer; {temporary registers for list manipulation}
@!disc_break:boolean; {was the current break at a discretionary node?}
@!post_disc_break:boolean; {and did it have a nonempty post-break part?}
@!cur_width:scaled; {width of line number |cur_line|}
@!cur_indent:scaled; {left margin of line number |cur_line|}
@!t:quarterword; {used for replacement counts in discretionary nodes}
@!pen:integer; {use when calculating penalties between lines}
@!cur_line: halfword; {the current line number being justified}
begin @<Reverse the links of the relevant passive nodes, setting |cur_p| to the
  first breakpoint@>;
cur_line:=prev_graf+1;
repeat @<Justify the line ending at breakpoint |cur_p|, and append it to the
  current vertical list, together with associated penalties and other
  insertions@>;
incr(cur_line); cur_p:=next_break(cur_p);
if cur_p<>null then if not post_disc_break then
  @<Prune unwanted nodes at the beginning of the next line@>;
until cur_p=null;
if (cur_line<>best_line)or(link(temp_head)<>null) then
  confusion("line breaking");
@:this can't happen line breaking}{\quad line breaking@>
prev_graf:=best_line-1;
end;
@y
@ The total number of lines that will be set by |post_line_break|
is |best_line-prev_graf-1|. The last breakpoint is specified by
|break_node(best_bet)|, and this passive node points to the other breakpoints
via the |prev_break| links. The finishing-up phase starts by linking the
relevant passive nodes in forward order, changing |prev_break| to
|next_break|. (The |next_break| fields actually reside in the same memory
space as the |prev_break| fields did, but we give them a new name because
of their new significance.) Then the lines are justified, one by one.

The |post_line_break| must also keep an dir stack, so that it can
output end direction instructions at the ends of lines
and begin direction instructions at the beginnings of lines.

@d next_break==prev_break {new name for |prev_break| after links are reversed}

@<Declare subprocedures for |line_break|@>=
procedure post_line_break(@!final_widow_penalty:integer);
label done,done1;
var q,@!r,@!s:pointer; {temporary registers for list manipulation}
@!disc_break:boolean; {was the current break at a discretionary node?}
@!post_disc_break:boolean; {and did it have a nonempty post-break part?}
@!cur_width:scaled; {width of line number |cur_line|}
@!cur_indent:scaled; {left margin of line number |cur_line|}
@!t:quarterword; {used for replacement counts in discretionary nodes}
@!pen:integer; {use when calculating penalties between lines}
@!cur_line: halfword; {the current line number being justified}
begin @<Reverse the links of the relevant passive nodes, setting |cur_p| to the
  first breakpoint@>;
cur_line:=prev_graf+1;
repeat @<Justify the line ending at breakpoint |cur_p|, and append it to the
  current vertical list, together with associated penalties and other
  insertions@>;
incr(cur_line); cur_p:=next_break(cur_p);
if cur_p<>null then if not post_disc_break then
  @<Prune unwanted nodes at the beginning of the next line@>;
until cur_p=null;
if (cur_line<>best_line)or(link(temp_head)<>null) then
  confusion("line breaking");
@:this can't happen line breaking}{\quad line breaking@>
prev_graf:=best_line-1;
end;
@z
%-------------------------
@x [39] m.889
adjust_tail:=adjust_head; just_box:=hpack(q,cur_width,exactly);
@y
adjust_tail:=adjust_head;
just_box:=hpack(q,cur_width,exactly,line_break_dir);
@z
%-------------------------
@x [40] m.892
@!ha,@!hb:pointer; {nodes |ha..hb| should be replaced by the hyphenated result}
@y
@!ha,@!hb:pointer; {nodes |ha..hb| should be replaced by the hyphenated result}
@!ha_local:pointer; {points to the local information for the first char node}
@z
%-------------------------
@x [40] m.896
@<Skip to node |ha|, or |goto done1|...@>=
loop@+  begin if is_char_node(s) then
    begin c:=qo(character(s)); hf:=font(s);
    end
  else if type(s)=ligature_node then
    if lig_ptr(s)=null then goto continue
    else begin q:=lig_ptr(s); c:=qo(character(q)); hf:=font(q);
      end
  else if (type(s)=kern_node)and(subtype(s)=normal) then goto continue
  else if type(s)=whatsit_node then
    begin @<Advance \(p)past a whatsit node in the \(p)pre-hyphenation loop@>;
    goto continue;
    end
  else goto done1;
  if lc_code(c)<>0 then
    if (lc_code(c)=c)or(uc_hyph>0) then goto done2
    else goto done1;
continue: prev_s:=s; s:=link(prev_s);
  end;
done2: hyf_char:=hyphen_char(hf);
if hyf_char<0 then goto done1;
if hyf_char>biggest_char then goto done1;
ha:=prev_s
@y
@<Skip to node |ha|, or |goto done1|...@>=
loop@+  begin if is_char_node(s) then
    begin c:=qo(character(s)); hf:=font(s);
    end
  else if type(s)=ligature_node then
    if lig_ptr(s)=null then goto continue
    else begin q:=lig_ptr(s); c:=qo(character(q)); hf:=font(q);
      end
  else if (type(s)=kern_node)and(subtype(s)=normal) then goto continue
  else if type(s)=whatsit_node then
    begin @<Advance \(p)past a whatsit node in the \(p)pre-hyphenation loop@>;
    goto continue;
    end
  else goto done1;
  if lc_code(c)<>0 then
    if (lc_code(c)=c)or(uc_hyph>0) then goto done2
    else goto done1;
continue: prev_s:=s; s:=link(prev_s);
  end;
done2: hyf_char:=hyphen_char(hf);
if hyf_char<0 then goto done1;
if hyf_char>biggest_char then goto done1;
ha:=prev_s;
ha_local:=ptr_info(s)
@z
%-------------------------
@x [40] m.908
@ @d append_charnode_to_t(#)== begin link(t):=get_avail; t:=link(t);
    font(t):=hf; character(t):=#;
    end
@y
@ @d append_charnode_to_t(#)== begin link(t):=get_avail; t:=link(t);
    ptr_info(t):=ha_local; info_incr_ref_count(ha_local);
    font(t):=hf; character(t):=#;
    end
@z
%-------------------------
@x [40] m.910
@ @d wrap_lig(#)==if ligature_present then
    begin p:=new_ligature(hf,cur_l,link(cur_q));
    if lft_hit then
      begin subtype(p):=2; lft_hit:=false;
      end;
    if # then if lig_stack=null then
      begin incr(subtype(p)); rt_hit:=false;
      end;
    link(cur_q):=p; t:=p; ligature_present:=false;
    end
@y
@ @d wrap_lig(#)==if ligature_present then
    begin p:=new_ligature(hf,cur_l,link(cur_q));
    info_decr_ref_count(ptr_info(p));
    ptr_info(p):=ha_local; info_incr_ref_count(ha_local);
    if lft_hit then
      begin subtype(p):=2; lft_hit:=false;
      end;
    if # then if lig_stack=null then
      begin incr(subtype(p)); rt_hit:=false;
      end;
    link(cur_q):=p; t:=p; ligature_present:=false;
    end
@z
%-------------------------
@x [40] m.910
wrap_lig(rt_hit);
if w<>0 then
  begin link(t):=new_kern(w); t:=link(t); w:=0;
  end;
@y
wrap_lig(rt_hit);
if w<>0 then
  begin link(t):=new_kern(w); t:=link(t); w:=0;
  info_decr_ref_count(ptr_info(t));
  ptr_info(t):=ha_local; info_incr_ref_count(ha_local);
  end;
@z
%-------------------------
@x [40] m.911
qi(2),qi(6):begin cur_r:=rem_byte(q); {\.{\?=:}, \.{\?=:>}}
  if lig_stack>null then character(lig_stack):=cur_r
  else begin lig_stack:=new_lig_item(cur_r);
    if j=n then bchar:=non_char
    else begin p:=get_avail; lig_ptr(lig_stack):=p;
      character(p):=qi(hu[j+1]); font(p):=hf;
      end;
    end;
  end;
qi(3):begin cur_r:=rem_byte(q); {\.{\?=:\?}}
  p:=lig_stack; lig_stack:=new_lig_item(cur_r); link(lig_stack):=p;
  end;
@y
qi(2),qi(6):begin cur_r:=rem_byte(q); {\.{\?=:}, \.{\?=:>}}
  if lig_stack>null then character(lig_stack):=cur_r
  else begin lig_stack:=new_lig_item(cur_r);
    info_decr_ref_count(ptr_info(lig_stack));
    ptr_info(lig_stack):=ha_local; info_incr_ref_count(ha_local);
    if j=n then bchar:=non_char
    else begin p:=get_avail; lig_ptr(lig_stack):=p;
      character(p):=qi(hu[j+1]); font(p):=hf;
      ptr_info(p):=ha_local; info_incr_ref_count(ha_local);
      end;
    end;
  end;
qi(3):begin cur_r:=rem_byte(q); {\.{\?=:\?}}
  p:=lig_stack; lig_stack:=new_lig_item(cur_r); link(lig_stack):=p;
  info_decr_ref_count(ptr_info(lig_stack));
  ptr_info(lig_stack):=ha_local; info_incr_ref_count(ha_local);
  end;
@z
%-------------------------
@x [40] m.914
@<Create and append a discretionary node as an alternative...@>=
repeat r:=get_node(small_node_size);
link(r):=link(hold_head); type(r):=disc_node;
@y
@<Create and append a discretionary node as an alternative...@>=
repeat r:=get_node(small_node_size);
link(r):=link(hold_head); type(r):=disc_node;
ptr_info(r):=ha_local; info_incr_ref_count(ha_local);
@z
%-------------------------
@x [44] m.977
@p function vsplit(@!n:eight_bits; @!h:scaled):pointer;
  {extracts a page of height |h| from box |n|}
label exit,done;
var v:pointer; {the box to be split}
p:pointer; {runs through the vlist}
q:pointer; {points to where the break occurs}
begin v:=box(n);
if split_first_mark<>null then
  begin delete_token_ref(split_first_mark); split_first_mark:=null;
  delete_token_ref(split_bot_mark); split_bot_mark:=null;
  end;
@<Dispense with trivial cases of void or bad boxes@>;
q:=vert_break(list_ptr(v),h,split_max_depth);
@<Look at all the marks in nodes before the break, and set the final
  link to |null| at the break@>;
q:=prune_page_top(q); p:=list_ptr(v); free_node(v,box_node_size);
if q=null then set_equiv(box_base+n,null)
    {the |eq_level| of the box stays the same}
else set_equiv(box_base+n,vpack(q,natural));
vsplit:=vpackage(p,h,exactly,split_max_depth);
exit: end;
@y
@p function vsplit(@!n:eight_bits; @!h:scaled):pointer;
  {extracts a page of height |h| from box |n|}
label exit,done;
var v:pointer; {the box to be split}
vdir:integer; {the direction of the box to be split}
p:pointer; {runs through the vlist}
q:pointer; {points to where the break occurs}
begin v:=box(n); vdir:=box_dir(v);
if split_first_mark<>null then
  begin delete_token_ref(split_first_mark); split_first_mark:=null;
  delete_token_ref(split_bot_mark); split_bot_mark:=null;
  end;
@<Dispense with trivial cases of void or bad boxes@>;
q:=vert_break(list_ptr(v),h,split_max_depth);
@<Look at all the marks in nodes before the break, and set the final
  link to |null| at the break@>;
q:=prune_page_top(q); p:=list_ptr(v); free_node(v,box_node_size);
if q=null then set_equiv(box_base+n,null)
    {the |eq_level| of the box stays the same}
else set_equiv(box_base+n,vpackage(q,natural,max_dimen,vdir));
vsplit:=vpackage(p,h,exactly,split_max_depth,vdir);
exit: end;
@z
%-------------------------
@x [45] m.1001
if width(temp_ptr)>height(p) then width(temp_ptr):=width(temp_ptr)-height(p)
else width(temp_ptr):=0;
@y
if (type(p)=hlist_node) and (is_mirrored(body_direction)) then begin
  if width(temp_ptr)>depth(p) then width(temp_ptr):=width(temp_ptr)-depth(p)
  else width(temp_ptr):=0;
  end
else begin
  if width(temp_ptr)>height(p) then width(temp_ptr):=width(temp_ptr)-height(p)
  else width(temp_ptr):=0;
  end;
@z
%-------------------------
@x [45] m.1002
begin page_total:=page_total+page_depth+height(p);
page_depth:=depth(p);
@y
begin
if (type(p)=hlist_node) and (is_mirrored(body_direction)) then begin
  page_total:=page_total+page_depth+depth(p);
  page_depth:=height(p);
  end
else begin
  page_total:=page_total+page_depth+height(p);
  page_depth:=depth(p);
  end;
@z
%-------------------------
@x [45] m.1017
set_equiv(box_base+255,
          vpackage(link(page_head),best_size,exactly,page_max_depth));
@y
set_equiv(box_base+255,
  vpackage(link(page_head),best_size,exactly,page_max_depth,body_direction));
@z
%-------------------------
@x [45] m.1018
@<Prepare all the boxes involved in insertions to act as queues@>=
begin r:=link(page_ins_head);
while r<>page_ins_head do
  begin if best_ins_ptr(r)<>null then
    begin n:=qo(subtype(r)); ensure_vbox(n);
    if box(n)=null then set_equiv(box_base+n,new_null_box);
@y
@<Prepare all the boxes involved in insertions to act as queues@>=
begin r:=link(page_ins_head);
while r<>page_ins_head do
  begin if best_ins_ptr(r)<>null then
    begin n:=qo(subtype(r)); ensure_vbox(n);
    if box(n)=null then set_equiv(box_base+n,new_null_box);
    box_dir(box(n)):=body_direction;
@z
%-------------------------
@x [45] m.1021
@ @<Wrap up the box specified by node |r|, splitting node |p| if...@>=
begin if type(r)=split_up then
  if (broken_ins(r)=p)and(broken_ptr(r)<>null) then
    begin while link(s)<>broken_ptr(r) do s:=link(s);
    link(s):=null;
    set_equiv(glue_base+split_top_skip_code,split_top_ptr(p));
    ins_ptr(p):=prune_page_top(broken_ptr(r));
    if ins_ptr(p)<>null then
      begin temp_ptr:=vpack(ins_ptr(p),natural);
      height(p):=height(temp_ptr)+depth(temp_ptr);
      free_node(temp_ptr,box_node_size); wait:=true;
      end;
    end;
best_ins_ptr(r):=null;
n:=qo(subtype(r));
temp_ptr:=list_ptr(box(n));
free_node(box(n),box_node_size);
set_equiv(box_base+n,vpack(temp_ptr,natural));
@y
@ @<Wrap up the box specified by node |r|, splitting node |p| if...@>=
begin if type(r)=split_up then
  if (broken_ins(r)=p)and(broken_ptr(r)<>null) then
    begin while link(s)<>broken_ptr(r) do s:=link(s);
    link(s):=null;
    set_equiv(glue_base+split_top_skip_code,split_top_ptr(p));
    ins_ptr(p):=prune_page_top(broken_ptr(r));
    if ins_ptr(p)<>null then
      begin temp_ptr:=vpackage(ins_ptr(p),natural,max_dimen,body_direction);
      height(p):=height(temp_ptr)+depth(temp_ptr);
      free_node(temp_ptr,box_node_size); wait:=true;
      end;
    end;
best_ins_ptr(r):=null;
n:=qo(subtype(r));
temp_ptr:=list_ptr(box(n));
free_node(box(n),box_node_size);
set_equiv(box_base+n,vpackage(temp_ptr,natural,max_dimen,body_direction));
@z
%-------------------------
@x [45] m.1054
function its_all_over:boolean; {do this when \.{\\end} or \.{\\dump} occurs}
label exit;
begin if privileged then
  begin if (page_head=page_tail)and(head=tail)and(dead_cycles=0) then
    begin its_all_over:=true; return;
    end;
  back_input; {we will try to end again after ejecting residual material}
  tail_append(new_null_box);
@y
function its_all_over:boolean; {do this when \.{\\end} or \.{\\dump} occurs}
label exit;
begin if privileged then
  begin if (page_head=page_tail)and(head=tail)and(dead_cycles=0) then
    begin its_all_over:=true; return;
    end;
  back_input; {we will try to end again after ejecting residual material}
  tail_append(new_null_box); box_dir(tail):=info(par_dir_stack);
@z
%-------------------------
@x [47] m.1071
primitive("xleaders",leader_ship,x_leaders);
@!@:x_leaders_}{\.{\\xleaders} primitive@>
@y
primitive("xleaders",leader_ship,x_leaders);
@!@:x_leaders_}{\.{\\xleaders} primitive@>
primitive("boxdir",assign_box_dir,0);
@!@:box_dir}{\.{\\boxdir} primitive@>
@z
%-------------------------
@x [47] m.1073
any_mode(make_box): begin_box(0);
@y
any_mode(make_box): begin_box(0);
any_mode(assign_box_dir): begin scan_eight_bit_int;
  cur_box:=box(cur_val);
  scan_optional_equals; scan_dir;
  if cur_box<>null then box_dir(cur_box):=cur_val;
  end;
@z
%-------------------------
@x [47] m.1076
@<Append box |cur_box| to the current...@>=
begin if cur_box<>null then
  begin shift_amount(cur_box):=box_context;
  if abs(mode)=vmode then
    begin append_to_vlist(cur_box);
    if adjust_tail<>null then
      begin if adjust_head<>adjust_tail then
        begin link(tail):=link(adjust_head); tail:=adjust_tail;
        end;
      adjust_tail:=null;
      end;
    if mode>0 then build_page;
    end
  else  begin if abs(mode)=hmode then space_factor:=1000
    else  begin p:=new_noad;
      math_type(nucleus(p)):=sub_box;
      info(nucleus(p)):=cur_box; cur_box:=p;
      end;
    link(tail):=cur_box; tail:=cur_box;
    end;
  end;
end
@y
@<Append box |cur_box| to the current...@>=
begin if cur_box<>null then
  begin shift_amount(cur_box):=box_context;
  ptr_info(cur_box):=local_par_info;
  info_incr_ref_count(local_par_info);
  if abs(mode)=vmode then
    begin append_to_vlist(cur_box);
    if adjust_tail<>null then
      begin if adjust_head<>adjust_tail then
        begin link(tail):=link(adjust_head); tail:=adjust_tail;
        end;
      adjust_tail:=null;
      end;
    if mode>0 then build_page;
    end
  else  begin if abs(mode)=hmode then space_factor:=1000
    else  begin p:=new_noad;
      math_type(nucleus(p)):=sub_box;
      info(nucleus(p)):=cur_box; cur_box:=p;
      end;
    link(tail):=cur_box; tail:=cur_box;
    end;
  end;
end
@z
%-------------------------
@x [47] m.1083
begin k:=cur_chr-vtop_code; saved(0):=box_context;
if k=hmode then
  if (box_context<box_flag)and(abs(mode)=vmode) then
    scan_spec(adjusted_hbox_group,true)
  else scan_spec(hbox_group,true)
else  begin if k=vmode then scan_spec(vbox_group,true)
  else  begin scan_spec(vtop_group,true); k:=vmode;
    end;
  normal_paragraph;
  end;
push_nest; mode:=-k;
@y
begin k:=cur_chr-vtop_code; saved(0):=box_context;
case abs(mode) of
vmode: spec_direction:=body_direction;
hmode: spec_direction:=text_direction;
mmode: spec_direction:=math_direction;
end;
if k=hmode then
  if (box_context<box_flag)and(abs(mode)=vmode) then
    scan_spec(adjusted_hbox_group,true)
  else scan_spec(hbox_group,true)
else  begin if k=vmode then scan_spec(vbox_group,true)
  else  begin scan_spec(vtop_group,true); k:=vmode;
    end;
  normal_paragraph;
  end;
push_nest; mode:=-k;
@z
%-------------------------
@x [47] m.1086
procedure package(@!c:small_number);
var h:scaled; {height of box}
@!p:pointer; {first node in a box}
@!d:scaled; {max depth}
begin d:=box_max_depth; unsave; save_ptr:=save_ptr-3;
if mode=-hmode then cur_box:=hpack(link(head),saved(2),saved(1))
else  begin cur_box:=vpackage(link(head),saved(2),saved(1),d);
  if c=vtop_code then @<Readjust the height and depth of |cur_box|,
    for \.{\\vtop}@>;
  end;
pop_nest; box_end(saved(0));
end;
@y
procedure package(@!c:small_number);
var h:scaled; {height of box}
@!p:pointer; {first node in a box}
@!d:scaled; {max depth}
begin d:=box_max_depth; unsave; save_ptr:=save_ptr-4;
if mode=-hmode then cur_box:=hpack(link(head),saved(2),saved(1),saved(3))
else  begin cur_box:=vpackage(link(head),saved(2),saved(1),d,saved(3));
  if c=vtop_code then @<Readjust the height and depth of |cur_box|,
    for \.{\\vtop}@>;
  end;
pop_nest; box_end(saved(0));
end;
@z
%-------------------------
@x [47] m.1091
procedure new_graf(@!indented:boolean);
begin prev_graf:=0;
if (mode=vmode)or(head<>tail) then
  tail_append(new_param_glue(par_skip_code));
push_nest; mode:=hmode; space_factor:=1000; set_cur_lang; clang:=cur_lang;
prev_graf:=(norm_min(left_hyphen_min)*@'100+norm_min(right_hyphen_min))
             *@'200000+cur_lang;
if indented then
  begin tail:=new_null_box; link(head):=tail; width(tail):=par_indent;@+
  end;
@y
procedure new_graf(@!indented:boolean);
begin prev_graf:=0;
if (mode=vmode)or(head<>tail) then
  tail_append(new_param_glue(par_skip_code));
push_nest; mode:=hmode; space_factor:=1000; set_cur_lang; clang:=cur_lang;
prev_graf:=(norm_min(left_hyphen_min)*@'100+norm_min(right_hyphen_min))
             *@'200000+cur_lang;
tmp_dir_stack:=get_avail; info(tmp_dir_stack):=par_direction;
link(tmp_dir_stack):=par_dir_stack;
par_dir_stack:=tmp_dir_stack;
if indented then
  begin tail:=new_null_box; link(head):=tail; width(tail):=par_indent;@+
  box_dir(tail):=info(par_dir_stack);
  change_dir(tail,info(par_dir_stack));
  end;
@z
%-------------------------
@x [47] m.1093
procedure indent_in_hmode;
var p,@!q:pointer;
begin if cur_chr>0 then {\.{\\indent}}
  begin p:=new_null_box; width(p):=par_indent;
@y
procedure indent_in_hmode;
var p,@!q:pointer;
begin if cur_chr>0 then {\.{\\indent}}
  begin p:=new_null_box; width(p):=par_indent;
  box_dir(p):=info(par_dir_stack);
  change_dir(p,info(par_dir_stack));
@z
%-------------------------
@x [47] m.1096
procedure end_graf;
begin if mode=hmode then
  begin if head=tail then pop_nest {null paragraphs are ignored}
  else line_break(widow_penalty);
  normal_paragraph;
  error_count:=0;
  end;
end;
@y
procedure end_graf;
begin if mode=hmode then
  begin if head=tail then pop_nest {null paragraphs are ignored}
  else line_break(widow_penalty,info(par_dir_stack));
  tmp_dir_stack:=par_dir_stack;
  par_dir_stack:=link(par_dir_stack);
  free_avail(tmp_dir_stack);
  normal_paragraph;
  error_count:=0;
  end;
end;
@z
%-------------------------
@x [48] m.1100
insert_group: begin end_graf; q:=split_top_skip; add_glue_ref(q);
  d:=split_max_depth; f:=floating_penalty; unsave; decr(save_ptr);
  {now |saved(0)| is the insertion number, or 255 for |vadjust|}
  p:=vpack(link(head),natural); pop_nest;
  if saved(0)<>255 then
    begin tail_append(get_node(ins_node_size));
    type(tail):=ins_node; subtype(tail):=qi(saved(0));
    height(tail):=height(p)+depth(p); ins_ptr(tail):=list_ptr(p);
    split_top_ptr(tail):=q; depth(tail):=d; float_cost(tail):=f;
    end
  else  begin tail_append(get_node(small_node_size));
    type(tail):=adjust_node;@/
    subtype(tail):=0; {the |subtype| is not used}
    adjust_ptr(tail):=list_ptr(p); delete_glue_ref(q);
    end;
  free_node(p,box_node_size);
  if nest_ptr=0 then build_page;
  end;
@y
insert_group: begin end_graf; q:=split_top_skip; add_glue_ref(q);
  d:=split_max_depth; f:=floating_penalty; unsave; decr(save_ptr);
  {now |saved(0)| is the insertion number, or 255 for |vadjust|}
  p:=vpackage(link(head),natural,max_dimen,body_direction); pop_nest;
  if saved(0)<>255 then
    begin tail_append(get_node(ins_node_size));
    type(tail):=ins_node; subtype(tail):=qi(saved(0));
    height(tail):=height(p)+depth(p); ins_ptr(tail):=list_ptr(p);
    split_top_ptr(tail):=q; depth(tail):=d; float_cost(tail):=f;
    end
  else  begin tail_append(get_node(small_node_size));
    type(tail):=adjust_node;@/
    subtype(tail):=0; {the |subtype| is not used}
    adjust_ptr(tail):=list_ptr(p); delete_glue_ref(q);
    end;
  free_node(p,box_node_size);
  if nest_ptr=0 then build_page;
  end;
@z
%-------------------------
@x [48] m.1119
procedure build_local_box;
var p:pointer;
    kind:integer;
begin
unsave;
kind:=saved(-1);
decr(save_ptr);
p:=link(head); pop_nest;
p:=hpack(p,natural);
@y
procedure build_local_box;
var p:pointer;
    kind:integer;
begin
unsave;
kind:=saved(-1);
decr(save_ptr);
p:=link(head); pop_nest;
p:=hpack(p,natural,text_direction);
@z
%-------------------------
@x [48] m.1125
@<Append the accent with appropriate kerns...@>=
begin t:=slant(f)/float_constant(65536);
@^real division@>
i:=char_info(f)(character(q));
w:=char_width(f)(i); h:=char_height(f)(height_depth(i));
if h<>x then {the accent must be shifted up or down}
  begin p:=hpack(p,natural); shift_amount(p):=x-h;
  end;
@y
@<Append the accent with appropriate kerns...@>=
begin t:=slant(f)/float_constant(65536);
@^real division@>
i:=char_info(f)(character(q));
w:=char_width(f)(i); h:=char_height(f)(height_depth(i));
if h<>x then {the accent must be shifted up or down}
  begin p:=hpack(p,natural,text_direction); shift_amount(p):=x-h;
  end;
@z
%-------------------------
@x [48] m.1136
procedure push_math(@!c:group_code);
begin push_nest; mode:=-mmode; incompleat_noad:=null; new_save_level(c);
end;
@y
procedure push_math(@!c:group_code);
begin
push_nest; mode:=-mmode; incompleat_noad:=null; new_save_level(c);
make_local_direction(info(math_dir_stack),true);
end;
@z
%-------------------------
@x [48] m.1138
procedure init_math;
label reswitch,found,not_found,done;
var w:scaled; {new or partial |pre_display_size|}
@!l:scaled; {new |display_width|}
@!s:scaled; {new |display_indent|}
@!p:pointer; {current node when calculating |pre_display_size|}
@!q:pointer; {glue specification when calculating |pre_display_size|}
@!f:internal_font_number; {font in current |char_node|}
@!n:integer; {scope of paragraph shape specification}
@!v:scaled; {|w| plus possible glue amount}
@!d:scaled; {increment to |v|}
begin get_token; {|get_x_token| would fail on \.{\\ifmmode}\thinspace!}
@y
procedure init_math;
label reswitch,found,not_found,done;
var w:scaled; {new or partial |pre_display_size|}
@!l:scaled; {new |display_width|}
@!s:scaled; {new |display_indent|}
@!p:pointer; {current node when calculating |pre_display_size|}
@!q:pointer; {glue specification when calculating |pre_display_size|}
@!f:internal_font_number; {font in current |char_node|}
@!n:integer; {scope of paragraph shape specification}
@!v:scaled; {|w| plus possible glue amount}
@!d:scaled; {increment to |v|}
begin get_token; {|get_x_token| would fail on \.{\\ifmmode}\thinspace!}
tmp_dir_stack:=get_avail;
if next_fake_math_flag=0 then 
  info(tmp_dir_stack):=math_direction
else begin
  info(tmp_dir_stack):=text_direction;
  next_fake_math_flag:=0;
  end;
link(tmp_dir_stack):=math_dir_stack;
math_dir_stack:=tmp_dir_stack;
@z
%-------------------------
@x [48] m.1145
@<Go into display math mode@>=
begin if head=tail then {`\.{\\noindent\$\$}' or `\.{\$\${ }\$\$}'}
  begin pop_nest; w:=-max_dimen;
  end
else  begin line_break(display_widow_penalty);@/
@y
@<Go into display math mode@>=
begin if head=tail then {`\.{\\noindent\$\$}' or `\.{\$\${ }\$\$}'}
  begin pop_nest; w:=-max_dimen;
  end
else  begin line_break(display_widow_penalty,info(par_dir_stack));@/
@z
%-------------------------
@x [48] m.1168
vcenter_group: begin end_graf; unsave; save_ptr:=save_ptr-2;
  p:=vpack(link(head),saved(1),saved(0)); pop_nest;
  tail_append(new_noad); type(tail):=vcenter_noad;
  math_type(nucleus(tail)):=sub_box; info(nucleus(tail)):=p;
  end;
@y
vcenter_group: begin end_graf; unsave; save_ptr:=save_ptr-3;
  p:=vpackage(link(head),saved(1),saved(0),max_dimen,saved(2)); pop_nest;
  tail_append(new_noad); type(tail):=vcenter_noad;
  math_type(nucleus(tail)):=sub_box; info(nucleus(tail)):=p;
  end;
@z
%-------------------------
@x [48] m.1174
begin unsave; p:=fin_mlist(null);
@y
begin unsave; p:=fin_mlist(null);
@z
%-------------------------
@x [48] m.1186
math_group: begin unsave; decr(save_ptr);@/
@y
math_group: begin unsave; decr(save_ptr);@/
@z
%-------------------------
@x [48] m.1191
  else  begin p:=fin_mlist(p); unsave; {end of |math_left_group|}
@y
  else  begin p:=fin_mlist(p); unsave; {end of |math_left_group|}
@z
%-------------------------
@x [48] m.1194
procedure after_math;
var l:boolean; {`\.{\\leqno}' instead of `\.{\\eqno}'}
@!danger:boolean; {not enough symbol fonts are present}
@!m:integer; {|mmode| or |-mmode|}
@!p:pointer; {the formula}
@!a:pointer; {box containing equation number}
@<Local variables for finishing a displayed formula@>@;
begin danger:=false;
@<Check that the necessary fonts for math symbols are present;
  if not, flush the current math lists and set |danger:=true|@>;
m:=mode; l:=false; p:=fin_mlist(null); {this pops the nest}
if mode=-m then {end of equation number}
  begin @<Check that another \.\$ follows@>;
  cur_mlist:=p; cur_style:=text_style; mlist_penalties:=false;
  mlist_to_hlist; a:=hpack(link(temp_head),natural);
  unsave; decr(save_ptr); {now |cur_group=math_shift_group|}
  if saved(0)=1 then l:=true;
@y
procedure after_math;
var l:boolean; {`\.{\\leqno}' instead of `\.{\\eqno}'}
@!danger:boolean; {not enough symbol fonts are present}
@!m:integer; {|mmode| or |-mmode|}
@!p:pointer; {the formula}
@!a:pointer; {box containing equation number}
@<Local variables for finishing a displayed formula@>@;
begin danger:=false;
@<Check that the necessary fonts for math symbols are present;
  if not, flush the current math lists and set |danger:=true|@>;
m:=mode; l:=false; p:=fin_mlist(null); {this pops the nest}
if mode=-m then {end of equation number}
  begin @<Check that another \.\$ follows@>;
  cur_mlist:=p; cur_style:=text_style; mlist_penalties:=false;
  mlist_to_hlist; a:=hpack(link(temp_head),natural,info(math_dir_stack));
  unsave; decr(save_ptr); {now |cur_group=math_shift_group|}
  if saved(0)=1 then l:=true;
@z
%-------------------------
@x [48] m.1196
@<Finish math in text@>=
begin tail_append(new_math(math_surround,before));
cur_mlist:=p; cur_style:=text_style; mlist_penalties:=(mode>0); mlist_to_hlist;
link(tail):=link(temp_head);
while link(tail)<>null do tail:=link(tail);
tail_append(new_math(math_surround,after));
space_factor:=1000; unsave;
end
@y
@<Finish math in text@> =
begin tail_append(new_math(math_surround,before));
cur_mlist:=p; cur_style:=text_style; mlist_penalties:=(mode>0); mlist_to_hlist;
link(tail):=link(temp_head);
while link(tail)<>null do tail:=link(tail);
tail_append(new_math(math_surround,after));
space_factor:=1000; unsave;
tmp_dir_stack:=math_dir_stack;
math_dir_stack:=link(math_dir_stack);
free_avail(tmp_dir_stack);
end
@z
%-------------------------
@x [48] m.1199
@<Finish displayed math@>=
cur_mlist:=p; cur_style:=display_style; mlist_penalties:=false;
mlist_to_hlist; p:=link(temp_head);@/
adjust_tail:=adjust_head; b:=hpack(p,natural); p:=list_ptr(b);
t:=adjust_tail; adjust_tail:=null;@/
w:=width(b); z:=display_width; s:=display_indent;
@y
@<Finish displayed math@>=
cur_mlist:=p; cur_style:=display_style; mlist_penalties:=false;
mlist_to_hlist; p:=link(temp_head);@/
adjust_tail:=adjust_head; b:=hpack(p,natural,info(math_dir_stack));
p:=list_ptr(b);
t:=adjust_tail; adjust_tail:=null;@/
w:=width(b); z:=display_width; s:=display_indent;
@z
%-------------------------
@x [48] m.1200
procedure resume_after_display;
begin if cur_group<>math_shift_group then confusion("display");
@:this can't happen display}{\quad display@>
unsave; prev_graf:=prev_graf+3;
@y
procedure resume_after_display;
begin if cur_group<>math_shift_group then confusion("display");
@:this can't happen display}{\quad display@>
unsave; prev_graf:=prev_graf+3;
tmp_dir_stack:=math_dir_stack;
math_dir_stack:=link(math_dir_stack);
free_avail(tmp_dir_stack);
@z
%-------------------------
@x [48] m.1201
@<Squeeze the equation as much as possible...@>=
begin if (e<>0)and((w-total_shrink[normal]+q<=z)or@|
   (total_shrink[sfi]<>0)or(total_shrink[fil]<>0)or
   (total_shrink[fill]<>0)or(total_shrink[filll]<>0)) then
  begin free_node(b,box_node_size);
  b:=hpack(p,z-q,exactly);
  end
else  begin e:=0;
  if w>z then
    begin free_node(b,box_node_size);
    b:=hpack(p,z,exactly);
    end;
  end;
w:=width(b);
end
@y
@<Squeeze the equation as much as possible...@>=
begin if (e<>0)and((w-total_shrink[normal]+q<=z)or@|
   (total_shrink[sfi]<>0)or(total_shrink[fil]<>0)or
   (total_shrink[fill]<>0)or(total_shrink[filll]<>0)) then
  begin free_node(b,box_node_size);
  b:=hpack(p,z-q,exactly,info(math_dir_stack));
  end
else  begin e:=0;
  if w>z then
    begin free_node(b,box_node_size);
    b:=hpack(p,z,exactly,info(math_dir_stack));
    end;
  end;
w:=width(b);
end
@z
%-------------------------
@x [49] m.1204
@ @<Append the display and perhaps also the equation number@>=
if e<>0 then
  begin r:=new_kern(z-w-e-d);
  if l then
    begin link(a):=r; link(r):=b; b:=a; d:=0;
    end
  else  begin link(b):=r; link(r):=a;
    end;
  b:=hpack(b,natural);
  end;
shift_amount(b):=s+d; append_to_vlist(b)
@y
@ @<Append the display and perhaps also the equation number@>=
if e<>0 then
  begin r:=new_kern(z-w-e-d);
  if l then
    begin link(a):=r; link(r):=b; b:=a; d:=0;
    end
  else  begin link(b):=r; link(r):=a;
    end;
  b:=hpack(b,natural,info(math_dir_stack));
  end;
shift_amount(b):=s+d; append_to_vlist(b)
@z
%-------------------------
@x [49] m.1210
any_mode(assign_int),
@y
any_mode(assign_next_fake_math),
any_mode(assign_int),
any_mode(assign_dir),
@z
%-------------------------
@x [49] m.1228
assign_dimen: begin p:=cur_chr; scan_optional_equals;
  scan_normal_dimen; word_define(p,cur_val);
  end;
@y
assign_next_fake_math: next_fake_math_flag:=1;
assign_dir: begin
  @<DIR: Assign direction codes@>
  end;
assign_dimen: begin p:=cur_chr; scan_optional_equals;
  scan_normal_dimen; word_define(p,cur_val);
  end;
@z
%-------------------------
@x [49] m.1257
@!flushable_string:str_number; {string not yet referenced}
@y
@!natural_dir:integer;{the natural direction of the font}
@!flushable_string:str_number; {string not yet referenced}
@z
%-------------------------
@x [49] m.1257
if scan_keyword("offset") then begin
  scan_int;
  offset:=cur_val;
  if (cur_val<0) then begin
    print_err("Illegal offset has been changed to 0");
    help1("The offset must be bigger than 0."); int_error(cur_val);
    offset:=0;
    end
  end
else offset:=0;
name_in_progress:=false;
@<If this font has already been loaded, set |f| to the internal
  font number and |goto common_ending|@>;
f:=read_font_info(u,cur_name,cur_area,s,offset);
@y
if scan_keyword("offset") then begin
  scan_int;
  offset:=cur_val;
  if (cur_val<0) then begin
    print_err("Illegal offset has been changed to 0");
    help1("The offset must be bigger than 0."); int_error(cur_val);
    offset:=0;
    end
  end
else offset:=0;
if scan_keyword("naturaldir") then begin
  scan_dir;
  natural_dir:=cur_val;
  end
else natural_dir:=-1;
name_in_progress:=false;
@<If this font has already been loaded, set |f| to the internal
  font number and |goto common_ending|@>;
f:=read_font_info(u,cur_name,cur_area,s,offset,natural_dir);
@z
%-------------------------
@x [51] m.1337
if (loc<limit)and(cat_code(buffer[loc])<>escape) then start_input;
  {\.{\\input} assumed}
@y
if (loc<limit)and(cat_code(buffer[loc])<>escape) then start_input;
  {\.{\\input} assumed}
@<DIR: Initialize direction stacks@>;
@z
%-------------------------
@x [53] m.1341
@d language_node=4 {|subtype| in whatsits that change the current language}
@y
@d language_node=4 {|subtype| in whatsits that change the current language}
@d dir_node_size=4 {number of words in change direction node}
@d dir_dir(#)==info(#+1) {keep the direction to be taken}
@d dir_dvi_ptr(#)==link(#+1) {for reflecting dvi output}
@d dir_dvi_h(#)==info(#+2) {for reflecting dvi output}
@z
%-------------------------
@x [53] m.1342
@!write_open:array[0..17] of boolean;
@y
@!write_open:array[0..17] of boolean;
@!neg_wd:scaled;
@!pos_wd:scaled;
@!neg_ht:scaled;
@!dvi_direction:integer;
@!dir_primary:array[0..31] of 0..31;
@!dir_secondary:array[0..31] of 0..31;
@!dir_tertiary:array[0..31] of 0..31;
@!dir_rearrange:array[0..3] of 0..31;
@!dir_names:array[0..3] of str_number;
@z
%-------------------------
@x [53] m.1343
for k:=0 to 17 do write_open[k]:=false;
@y
for k:=0 to 17 do write_open[k]:=false;
set_new_eqtb_int(dir_base+page_direction_code,0);
set_new_eqtb_int(dir_base+body_direction_code,0);
set_new_eqtb_int(dir_base+par_direction_code,0);
set_new_eqtb_int(dir_base+text_direction_code,0);
set_new_eqtb_int(dir_base+math_direction_code,0);
set_new_eqtb_sc(dimen_base+page_height_code,55380984); {29.7cm}
set_new_eqtb_sc(dimen_base+page_width_code,39158276); {21cm}
set_new_eqtb_sc(dimen_base+page_bottom_offset_code,page_height-9472573);
                {-2 inches}
set_new_eqtb_sc(dimen_base+page_right_offset_code,page_width-9472573);
                {-2 inches}
for k:= 0 to 7 do begin
  dir_primary  [k   ]:=dir_T;
  dir_primary  [k+ 8]:=dir_L;
  dir_primary  [k+16]:=dir_B;
  dir_primary  [k+24]:=dir_R;
  end;
for k:= 0 to 3 do begin
  dir_secondary[k   ]:=dir_L;
  dir_secondary[k+ 4]:=dir_R;
  dir_secondary[k+ 8]:=dir_T;
  dir_secondary[k+12]:=dir_B;

  dir_secondary[k+16]:=dir_L;
  dir_secondary[k+20]:=dir_R;
  dir_secondary[k+24]:=dir_T;
  dir_secondary[k+28]:=dir_B;
  end;
for k:=0 to 7 do begin
  dir_tertiary[k*4  ]:=dir_T;
  dir_tertiary[k*4+1]:=dir_L;
  dir_tertiary[k*4+2]:=dir_B;
  dir_tertiary[k*4+3]:=dir_R;
  end;
dir_rearrange[0]:=0;
dir_rearrange[1]:=0;
dir_rearrange[2]:=1;
dir_rearrange[3]:=1;
dir_names[0]:="T";
dir_names[1]:="L";
dir_names[2]:="B";
dir_names[3]:="R";
@z
%-------------------------
@x [53] m.1344
@d set_language_code=5 {command modifier for \.{\\setlanguage}}
@y
@d set_language_code=5 {command modifier for \.{\\setlanguage}}
@d dir_whatsit_node=6 {|subtype| in whatsits for change direction node}
@z
%-------------------------
@x [53] m.1344
primitive("localinterlinepenalty",assign_int,
          int_base+local_inter_line_penalty_code);@/
primitive("localbrokenpenalty",assign_int,
          int_base+local_broken_penalty_code);@/
@y
primitive("localinterlinepenalty",assign_int,
          int_base+local_inter_line_penalty_code);@/
primitive("localbrokenpenalty",assign_int,
          int_base+local_broken_penalty_code);@/
primitive("pagedir",assign_dir,dir_base+page_direction_code);@/
primitive("bodydir",assign_dir,dir_base+body_direction_code);@/
primitive("pardir",assign_dir,dir_base+par_direction_code);@/
primitive("textdir",assign_dir,dir_base+text_direction_code);@/
primitive("mathdir",assign_dir,dir_base+math_direction_code);@/
primitive("pageheight",assign_dimen,dimen_base+page_height_code);@/
primitive("pagewidth",assign_dimen,dimen_base+page_width_code);@/
primitive("nextfakemath",assign_next_fake_math,0);@/
{
primitive("pagerightoffset",assign_dimen,dimen_base+page_right_offset_code);@/
primitive("pagebottomoffset",assign_dimen,dimen_base+page_bottom_offset_code);@/
}
@z
%-------------------------
@x [53] m.1346
  set_language_code:print_esc("setlanguage");
  othercases print("[unknown extension!]")
  endcases;
@y
  set_language_code:print_esc("setlanguage");
  othercases print("[unknown extension!]")
  endcases;
@z
%-------------------------
@x [53] m.1348
set_language_code:@<Implement \.{\\setlanguage}@>;
@y
set_language_code:@<Implement \.{\\setlanguage}@>;
@z
%-------------------------
@x [53] m.1349
procedure new_whatsit(@!s:small_number;@!w:small_number);
var p:pointer; {the new node}
begin p:=get_node(w); type(p):=whatsit_node; subtype(p):=s;
link(tail):=p; tail:=p;
end;
@y
procedure new_whatsit(@!s:small_number;@!w:small_number);
var p:pointer; {the new node}
begin p:=get_node(w); type(p):=whatsit_node; subtype(p):=s;
@<LOCAL: Add info node pointer@>;
link(tail):=p; tail:=p;
end;
@z
%-------------------------
@x [53] m.1356
language_node:begin print_esc("setlanguage");
  print_int(what_lang(p)); print(" (hyphenmin ");
  print_int(what_lhm(p)); print_char(",");
  print_int(what_rhm(p)); print_char(")");
  end;
@y
language_node:begin print_esc("setlanguage");
  print_int(what_lang(p)); print(" (hyphenmin ");
  print_int(what_lhm(p)); print_char(",");
  print_int(what_rhm(p)); print_char(")");
  end;
dir_whatsit_node:
  if dir_dir(p)<0 then begin
    print_esc("enddir"); print(" "); print_dir(dir_dir(p)+64);
    end
  else begin
    print_esc("begindir"); print(" "); print_dir(dir_dir(p));
    end;
@z
%-------------------------
@x [53] m.1357
close_node,language_node: begin r:=get_node(small_node_size);
  words:=small_node_size;
  end;
@y
close_node,language_node: begin r:=get_node(small_node_size);
  words:=small_node_size;
  end;
dir_whatsit_node: begin r:=get_node(dir_node_size);
  words:=small_node_size;
  end;
@z
%-------------------------
@x [53] m.1358
close_node,language_node: free_node(p,small_node_size);
@y
close_node,language_node: free_node(p,small_node_size);
dir_whatsit_node: free_node(p,dir_node_size);
@z
%-------------------------
@x
procedure make_info_node;
{This function creates an |info_node|}
var p:pointer;
begin
p:=get_node(info_size); type(p):=info_node;
link(p):=null;
info_ref_count(p):=1;
info_pen_inter(p):=local_inter_line_penalty;
info_pen_broken(p):=local_broken_penalty;
if local_left_box=null then
  info_left_box(p):=null
else
  info_left_box(p):=copy_node_list(local_left_box);
if local_right_box=null then
  info_right_box(p):=null
else
  info_right_box(p):=copy_node_list(local_right_box);
if current_file_node=null then begin
  info_file(p):=null;
  end
else begin
  info_file(p):=current_file_node;
  file_incr_ref_count(current_file_node);
  end;
eq_define(local_par_info_base,data,p);
end;
@y
procedure init_local_direction; forward;
procedure make_info_node;
{This function creates an |info_node|}
var p:pointer;
begin
if local_dir=null then init_local_direction
else begin
  p:=get_node(info_size); type(p):=info_node;
  link(p):=null;
  info_ref_count(p):=1;
  info_pen_inter(p):=local_inter_line_penalty;
  info_pen_broken(p):=local_broken_penalty;
  if local_left_box=null then
    info_left_box(p):=null
  else
    info_left_box(p):=copy_node_list(local_left_box);
  if local_right_box=null then
    info_right_box(p):=null
  else
    info_right_box(p):=copy_node_list(local_right_box);
  if current_file_node=null then begin
    info_file(p):=null;
    end
  else begin
    info_file(p):=current_file_node;
    file_incr_ref_count(current_file_node);
    end;
  info_dir(p):=local_dir;
  dir_incr_ref_count(local_dir);
  eq_define(local_par_info_base,data,p);
  end;
end;

procedure init_local_direction;
var new_ptr:pointer;
    no_levels:integer;
begin
no_levels:=1;
new_ptr:=get_node(dir_size+no_levels*2);
type(new_ptr):=dir_node;
dir_no(new_ptr):=no_levels;
dir_ref_count(new_ptr):=1;
mem[new_ptr+2*no_levels].int:=cur_level;
mem[new_ptr+2*no_levels+1].int:=cur_val;
eq_define(local_dir_base,data,new_ptr);
make_info_node;
end;

procedure make_local_direction(new_val:integer;new_stack:boolean);
var old_ptr,new_ptr:pointer;
    i,no_levels:integer;
begin
old_ptr:=local_dir;
if new_stack then
  no_levels:=1
else if mem[old_ptr+2*dir_no(old_ptr)].int=cur_level then
  no_levels:=dir_no(old_ptr)
else
  no_levels:=dir_no(old_ptr)+1;
new_ptr:=get_node(dir_size+no_levels*2);
type(new_ptr):=dir_node;
dir_no(new_ptr):=no_levels;
dir_ref_count(new_ptr):=1;
for i:=1 to no_levels-1 do begin
  mem[new_ptr+2*i].int:=mem[old_ptr+2*i].int;
  mem[new_ptr+2*i+1].int:=mem[old_ptr+2*i+1].int;
  end;
mem[new_ptr+2*no_levels].int:=cur_level;
mem[new_ptr+2*no_levels+1].int:=new_val;
eq_define(local_dir_base,data,new_ptr);
make_info_node;
end;
@z
%-------------------------
@x
@ @<LOCAL: Change file node@>=
begin
make_file_node;
end

@y
@ @<LOCAL: Change file node@>=
begin
make_file_node;
end

@ @<LOCAL: Delete direction node@>=
begin
dir_decr_ref_count(p);
end

@ @<DIR: Initialize direction stacks@>=
begin
par_dir_stack:=null;
math_dir_stack:=null;
align_dir_stack:=null;
init_local_direction;
end

@ @<DIR: Assign direction codes@>=
case cur_chr of
dir_base+page_direction_code: begin
  if its_all_over then begin
    scan_dir;
    eq_word_define(dir_base+page_direction_code,cur_val);
    end;
  end;
dir_base+body_direction_code: begin
  if its_all_over then begin
    scan_dir;
    eq_word_define(dir_base+body_direction_code,cur_val);
    end;
  end;
dir_base+par_direction_code: begin
  scan_dir;
  eq_word_define(dir_base+par_direction_code,cur_val);
  end;
dir_base+text_direction_code: begin scan_dir;
  eq_word_define(dir_base+text_direction_code,cur_val);
  make_local_direction(cur_val,false);
  end;
dir_base+math_direction_code: begin scan_dir;
  eq_word_define(dir_base+math_direction_code,cur_val);
  end;
end;

@
@d push_dir_node(#)==
begin dir_tmp:=get_node(dir_node_size);
type(dir_tmp):=whatsit_node; subtype(dir_tmp):=dir_whatsit_node;
dir_dir(dir_tmp):=#;
link(dir_tmp):=dir_ptr; dir_ptr:=dir_tmp;
end

@d pop_dir_node==
begin dir_tmp:=dir_ptr;
dir_ptr:=link(dir_tmp);
free_node(dir_tmp,dir_node_size);
end

@ @<DIR: Declare |change_dir|@>=
procedure change_dir(p:pointer;dir:integer);
var new_info:pointer;
    old_info:pointer;
    new_dir_node:pointer;
begin
old_info:=ptr_info(p);
if old_info=null then confusion("changedir");
new_info:=get_node(info_size); type(new_info):=info_node;
link(new_info):=null;
info_ref_count(new_info):=1;
info_pen_inter(new_info):=info_pen_inter(old_info);
info_pen_broken(new_info):=info_pen_broken(old_info);
if info_left_box(old_info)=null then
  info_left_box(new_info):=null
else
  info_left_box(new_info):=copy_node_list(info_left_box(old_info));
if info_right_box(old_info)=null then
  info_right_box(new_info):=null
else
  info_right_box(new_info):=copy_node_list(info_right_box(old_info));
info_file(new_info):=info_file(old_info);
file_incr_ref_count(info_file(old_info));
new_dir_node:=get_node(dir_size+2);
type(new_dir_node):=dir_node;
dir_no(new_dir_node):=1;
dir_ref_count(new_dir_node):=2; {core dump if we put 1; why?}
mem[new_dir_node+2].int:=0;
mem[new_dir_node+3].int:=dir;
info_dir(new_info):=new_dir_node;
ptr_info(p):=new_info;
info_decr_ref_count(old_info);
end

@ @<DIR: Initialize the text direction@>=
begin
dir_ptr:=null;
cur_direction:=dvi_direction;
new_dir_node:=info_dir(ptr_info(p));
for i:=1 to dir_no(new_dir_node) do
  @<DIR: PUSH a new direction for mixed direction typesetting@>;
prev_dir_node:=new_dir_node;
end

@ @<DIR: Finalize the text direction@>=
begin
for i:=dir_no(prev_dir_node) downto 1 do
  @<DIR: POP a new direction for mixed direction typesetting@>;
end

@ @<DIR: Check if the text direction has changed@>=
begin
if p<>null then
if ptr_info(p)<>null then 
if info_dir(ptr_info(p))<>null then begin
  new_dir_node:=info_dir(ptr_info(p));
  if new_dir_node<>prev_dir_node then begin
    m:=dir_no(prev_dir_node);
    if dir_no(new_dir_node)<m then m:=dir_no(new_dir_node);
    i:=1; dir_level:=0; dir_found:=false;
    while not dir_found do begin
      if i>m then dir_found:=true
      else if mem[prev_dir_node+2*i+1].int = mem[new_dir_node+2*i+1].int then
        incr(dir_level)
      else dir_found:=true;
      incr(i);
      end;
    for i:=dir_no(prev_dir_node) downto dir_level+1 do
      @<DIR: POP a new direction for mixed direction typesetting@>;
    for i:=dir_level+1 to dir_no(new_dir_node) do
      @<DIR: PUSH a new direction for mixed direction typesetting@>;
    prev_dir_node:=new_dir_node;
    end
  end
else begin print("No local node"); print_ln; end
else begin print("No direction node"); print_ln; end
end

@ @<DIR: PUSH a new direction for mixed direction typesetting@>=begin
  synch_h; synch_v;
  d:=mem[new_dir_node+2*i+1].int;
  if not((i=1) and (cur_direction=d)) then begin
    push_dir_node(dvi_direction);
    if dir_opposite(dir_secondary[d])(dir_secondary[dvi_direction]) then begin
      if (dir_secondary[dvi_direction]=0) or
         (dir_secondary[dvi_direction]=2) then
        dvi_out(down4)
      else
        dvi_out(right4);
      dir_dvi_ptr(dir_ptr):=dvi_ptr;
      dir_dvi_h(dir_ptr):=dvi_h;
      dvi_four(0);
      cur_h:=0; dvi_h:=0;
      end
    else begin
      dir_dvi_ptr(dir_ptr):=dvi_ptr;
      dir_dvi_h(dir_ptr):=dvi_h;
      end;
    append_string("om:");
    append_string("startdir=");
    append_char(dir_names[dir_primary[d]]);
    append_char(dir_names[dir_secondary[d]]);
    append_char(dir_names[dir_tertiary[d]]);
    g:=make_string;
    if length(g)<256 then
      begin dvi_out(xxx1); dvi_out(length(g));
      end
    else  begin dvi_out(xxx4); dvi_four(length(g));
      end;
    for newk:=str_start(g) to pool_ptr-1 do dvi_out(so(str_pool[newk]));
    flush_string; {erase the string}
    dvi_direction:=d;
    end
  end

@ @<DIR: POP a new direction for mixed direction typesetting@>=begin
  synch_h; synch_v;
  d:=mem[prev_dir_node+2*i+1].int;
  if not((i=1) and (cur_direction=d)) then begin
    dvi_dir_h:=dir_dvi_h(dir_ptr);
    dvi_dir_ptr:=dir_dvi_ptr(dir_ptr);
    dvi_direction:=dir_dir(dir_ptr);
    pop_dir_node;
    if dir_opposite(dir_secondary[d])(dir_secondary[dvi_direction]) then begin
      dvi_temp_ptr:=dvi_ptr;
      dvi_ptr:=dvi_dir_ptr;
      if (dir_secondary[dvi_direction]=0) or
         (dir_secondary[dvi_direction]=1) then
        dvi_four(cur_h)
      else
        dvi_four(-cur_h);
      dvi_ptr:=dvi_temp_ptr;
      append_string("om:");
      append_string("enddir=");
      append_char(dir_names[dir_primary[d]]);
      append_char(dir_names[dir_secondary[d]]);
      append_char(dir_names[dir_tertiary[d]]);
      g:=make_string;
      if length(g)<256 then
        begin dvi_out(xxx1); dvi_out(length(g));
        end
      else  begin dvi_out(xxx4); dvi_four(length(g));
        end;
      for newk:=str_start(g) to pool_ptr-1 do dvi_out(so(str_pool[newk]));
      flush_string; {erase the string}
      if (dir_secondary[dvi_direction]=0) or
         (dir_secondary[dvi_direction]=2) then
        dvi_out(down4)
      else
        dvi_out(right4);
      if (dir_secondary[dvi_direction]=0) or
         (dir_secondary[dvi_direction]=1) then
        dvi_four(cur_h)
      else
        dvi_four(-cur_h);
      cur_h:=dvi_dir_h+cur_h; dvi_h:=cur_h;
      end
    else begin
      append_string("om:");
      append_string("enddir=");
      append_char(dir_names[dir_primary[d]]);
      append_char(dir_names[dir_secondary[d]]);
      append_char(dir_names[dir_tertiary[d]]);
      g:=make_string;
      if length(g)<256 then
        begin dvi_out(xxx1); dvi_out(length(g));
        end
      else  begin dvi_out(xxx4); dvi_four(length(g));
        end;
      for newk:=str_start(g) to pool_ptr-1 do dvi_out(so(str_pool[newk]));
      flush_string; {erase the string}
      end
    end
  end
@z
