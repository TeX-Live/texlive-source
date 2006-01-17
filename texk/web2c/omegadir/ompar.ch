% ompar.ch: Local paragraph information
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
@x [10] m.134
@d is_char_node(#) == (#>=hi_mem_min)
  {does the argument point to a |char_node|?}
@d font == type {the font code in a |char_node|}
@d character == subtype {the character code in a |char_node|}
@y
@d is_char_node(#) == (#>=hi_mem_min)
  {does the argument point to a |char_node|?}
@d font==type {the font code in a |char_node|}
@d character==subtype {the character code in a |char_node|}
@d file_col(#)==mem[#-1].hh.b1 {the column in which this character appeared}
@z
%-------------------------
@x [10] m.136
@p function new_null_box:pointer; {creates a new box node}
var p:pointer; {the new node}
begin p:=get_node(box_node_size); type(p):=hlist_node;
subtype(p):=min_quarterword;
width(p):=0; depth(p):=0; height(p):=0; shift_amount(p):=0; list_ptr(p):=null;
glue_sign(p):=normal; glue_order(p):=normal; set_glue_ratio_zero(glue_set(p));
new_null_box:=p;
end;
@y
@p function new_null_box:pointer; {creates a new box node}
var p:pointer; {the new node}
begin p:=get_node(box_node_size); type(p):=hlist_node;
subtype(p):=min_quarterword;
width(p):=0; depth(p):=0; height(p):=0; shift_amount(p):=0; list_ptr(p):=null;
glue_sign(p):=normal; glue_order(p):=normal; set_glue_ratio_zero(glue_set(p));
@<LOCAL: Add info node pointer@>;
new_null_box:=p;
end;
@z
%-------------------------
@x [10] m.139
@p function new_rule:pointer;
var p:pointer; {the new node}
begin p:=get_node(rule_node_size); type(p):=rule_node;
subtype(p):=0; {the |subtype| is not used}
width(p):=null_flag; depth(p):=null_flag; height(p):=null_flag;
new_rule:=p;
end;
@y
@p function new_rule:pointer;
var p:pointer; {the new node}
begin p:=get_node(rule_node_size); type(p):=rule_node;
subtype(p):=0; {the |subtype| is not used}
width(p):=null_flag; depth(p):=null_flag; height(p):=null_flag;
@<LOCAL: Add info node pointer@>;
new_rule:=p;
end;
@z
%-------------------------
@x [10] m.144
@p function new_ligature(@!f,@!c:quarterword; @!q:pointer):pointer;
var p:pointer; {the new node}
begin p:=get_node(small_node_size); type(p):=ligature_node;
font(lig_char(p)):=f; character(lig_char(p)):=c; lig_ptr(p):=q;
subtype(p):=0; new_ligature:=p;
end;
@#
function new_lig_item(@!c:quarterword):pointer;
var p:pointer; {the new node}
begin p:=get_node(small_node_size); character(p):=c; lig_ptr(p):=null;
new_lig_item:=p;
end;
@y
@d add_file_col(#)==if state>0 then file_col(#):=loc-start
                    else file_col(#):=stack_col

@p
function stack_col:integer;
label done;
var i:integer;
    col:integer;
begin
col:=0;
i:=input_ptr;
while (i>=0) do begin
  if input_stack[i].state_field>0 then begin
    col:=input_stack[i].loc_field-input_stack[i].start_field;
    goto done;
    end;
  decr(i);
  end;
done: stack_col:=col;
end;

function new_ligature(@!f,@!c:quarterword; @!q:pointer):pointer;
var p:pointer; {the new node}
begin p:=get_node(small_node_size); type(p):=ligature_node;
font(lig_char(p)):=f; character(lig_char(p)):=c; lig_ptr(p):=q;
@<LOCAL: Add info node pointer@>;
add_file_col(p);
subtype(p):=0; new_ligature:=p;
end;
@#
function new_lig_item(@!c:quarterword):pointer;
var p:pointer; {the new node}
begin p:=get_node(small_node_size); character(p):=c; lig_ptr(p):=null;
@<LOCAL: Add info node pointer@>;
new_lig_item:=p;
end;
@z
%-------------------------
@x [10] m.145
@p function new_disc:pointer; {creates an empty |disc_node|}
var p:pointer; {the new node}
begin p:=get_node(small_node_size); type(p):=disc_node;
replace_count(p):=0; pre_break(p):=null; post_break(p):=null;
new_disc:=p;
end;
@y
@p function new_disc:pointer; {creates an empty |disc_node|}
var p:pointer; {the new node}
begin p:=get_node(small_node_size); type(p):=disc_node;
replace_count(p):=0; pre_break(p):=null; post_break(p):=null;
@<LOCAL: Add info node pointer@>;
new_disc:=p;
end;
@z
%-------------------------
@x [10] m.147
@p function new_math(@!w:scaled;@!s:small_number):pointer;
var p:pointer; {the new node}
begin p:=get_node(small_node_size); type(p):=math_node;
subtype(p):=s; width(p):=w; new_math:=p;
end;
@y
@p function new_math(@!w:scaled;@!s:small_number):pointer;
var p:pointer; {the new node}
begin p:=get_node(small_node_size); type(p):=math_node;
@<LOCAL: Add info node pointer@>;
subtype(p):=s; width(p):=w; new_math:=p;
end;
@z
%-------------------------
@x [10] m.151
@p function new_spec(@!p:pointer):pointer; {duplicates a glue specification}
var q:pointer; {the new spec}
begin q:=get_node(glue_spec_size);@/
mem[q]:=mem[p]; glue_ref_count(q):=null;@/
width(q):=width(p); stretch(q):=stretch(p); shrink(q):=shrink(p);
new_spec:=q;
end;
@y
@p function new_spec(@!p:pointer):pointer; {duplicates a glue specification}
var q:pointer; {the new spec}
begin q:=get_node(glue_spec_size);@/
mem[q]:=mem[p]; glue_ref_count(q):=null;@/
width(q):=width(p); stretch(q):=stretch(p); shrink(q):=shrink(p);
new_spec:=q;
end;
@z
%-------------------------
@x [10] m.152
@p function new_param_glue(@!n:small_number):pointer;
var p:pointer; {the new node}
@!q:pointer; {the glue specification}
begin p:=get_node(small_node_size); type(p):=glue_node; subtype(p):=n+1;
leader_ptr(p):=null;@/
q:=@<Current |mem| equivalent of glue parameter number |n|@>@t@>;
glue_ptr(p):=q; incr(glue_ref_count(q));
new_param_glue:=p;
end;
@y
@p function new_param_glue(@!n:small_number):pointer;
var p:pointer; {the new node}
@!q:pointer; {the glue specification}
begin p:=get_node(small_node_size); type(p):=glue_node; subtype(p):=n+1;
leader_ptr(p):=null;@/
q:=@<Current |mem| equivalent of glue parameter number |n|@>@t@>;
glue_ptr(p):=q; incr(glue_ref_count(q));
@<LOCAL: Add info node pointer@>;
new_param_glue:=p;
end;
@z
%-------------------------
@x [10] m.153
@p function new_glue(@!q:pointer):pointer;
var p:pointer; {the new node}
begin p:=get_node(small_node_size); type(p):=glue_node; subtype(p):=normal;
leader_ptr(p):=null; glue_ptr(p):=q; incr(glue_ref_count(q));
new_glue:=p;
end;
@y
@p function new_glue(@!q:pointer):pointer;
var p:pointer; {the new node}
begin p:=get_node(small_node_size); type(p):=glue_node; subtype(p):=normal;
leader_ptr(p):=null; glue_ptr(p):=q; incr(glue_ref_count(q));
@<LOCAL: Add info node pointer@>;
new_glue:=p;
end;
@z
%-------------------------
@x [10] m.154
@p function new_skip_param(@!n:small_number):pointer;
var p:pointer; {the new node}
begin temp_ptr:=new_spec(@<Current |mem| equivalent of glue parameter...@>);
p:=new_glue(temp_ptr); glue_ref_count(temp_ptr):=null; subtype(p):=n+1;
new_skip_param:=p;
end;
@y
@p function new_skip_param(@!n:small_number):pointer;
var p:pointer; {the new node}
begin temp_ptr:=new_spec(@<Current |mem| equivalent of glue parameter...@>);
p:=new_glue(temp_ptr); glue_ref_count(temp_ptr):=null; subtype(p):=n+1;
@<LOCAL: Add info node pointer@>;
new_skip_param:=p;
end;
@z
%-------------------------
@x [10] m.156
@p function new_kern(@!w:scaled):pointer;
var p:pointer; {the new node}
begin p:=get_node(small_node_size); type(p):=kern_node;
subtype(p):=normal;
width(p):=w;
new_kern:=p;
end;
@y
@p function new_kern(@!w:scaled):pointer;
var p:pointer; {the new node}
begin p:=get_node(small_node_size); type(p):=kern_node;
subtype(p):=normal;
width(p):=w;
@<LOCAL: Add info node pointer@>;
new_kern:=p;
end;
@z
%-------------------------
@x [10] m.158
@p function new_penalty(@!m:integer):pointer;
var p:pointer; {the new node}
begin p:=get_node(small_node_size); type(p):=penalty_node;
subtype(p):=0; {the |subtype| is not used}
penalty(p):=m; new_penalty:=p;
end;
@y
@p function new_penalty(@!m:integer):pointer;
var p:pointer; {the new node}
begin p:=get_node(small_node_size); type(p):=penalty_node;
subtype(p):=0; {the |subtype| is not used}
@<LOCAL: Add info node pointer@>;
penalty(p):=m; new_penalty:=p;
end;
@z
%-------------------------
@x [10] m.160 l.3270
@ In fact, there are still more types coming. When we get to math formula
processing we will see that a |style_node| has |type=14|; and a number
of larger type codes will also be defined, for use in math mode only.
@y
@ In fact, there are still more types coming. When we get to math formula
processing we will see that a |style_node| has |type=14|; and a number
of larger type codes will also be defined, for use in math mode only.

@d file_node=unset_node+1 {|type| to hold file names and line numbers}

@d file_size=3
@d file_no(#) == info(#+1) {number of open files}
@d file_ref_count(#) == link(#+1) {reference count to this node}

@d file_incr_ref_count(#)==begin
  if (#)<>null then incr(file_ref_count(#));
  end
@d file_decr_ref_count(#)==begin
  if (#)<>null then begin
    decr(file_ref_count(#));
    if file_ref_count(#) = 0 then begin
      free_node(#,file_size+2*file_no(#));
      end;
    end;
  end

@d info_node=unset_node+2 {|type| to hold local information}

@d ptr_info(#)        == link(#-1) {pointer to the local info node}

@d info_size=5
@d info_ref_count(#)  == link(#+1) {reference count to this node}
@d info_pen_inter(#)  == info(#+2) {\.{\\localinterlinepenalty}}
@d info_pen_broken(#) == link(#+2) {\.{\\localbrokenpenalty}}
@d info_left_box(#)   == info(#+3) {\.{\\localleftbox}}
@d info_right_box(#)  == link(#+3) {\.{\\localrightbox}}
@d info_file(#)       == info(#+4) {file names and line numbers}

@d info_incr_ref_count(#)==begin
  if (#)<>null then incr(info_ref_count(#));
  end
@d info_decr_ref_count(#)==begin
  if (#)<>null then begin
    decr(info_ref_count(#));
    if info_ref_count(#) = 0 then begin
      if info_left_box(#)<>null then
        flush_node_list(info_left_box(#));
      if info_right_box(#)<>null then
        flush_node_list(info_right_box(#));
      if info_file(#)<>null then
        file_decr_ref_count(info_file(#));
      free_node(#,info_size);
      #:=null;
      end;
    end;
  end

@d biggest_ordinary_node=info_node

@z
%-------------------------
@x [10] m.162
@d zero_glue==mem_bot {specification for \.{0pt plus 0pt minus 0pt}}
@d sfi_glue==zero_glue+glue_spec_size {\.{0pt plus 1fi minus 0pt}}
@d fil_glue==sfi_glue+glue_spec_size {\.{0pt plus 1fil minus 0pt}}
@d fill_glue==fil_glue+glue_spec_size {\.{0pt plus 1fill minus 0pt}}
@d ss_glue==fill_glue+glue_spec_size {\.{0pt plus 1fil minus 1fil}}
@d fil_neg_glue==ss_glue+glue_spec_size {\.{0pt plus -1fil minus 0pt}}
@d lo_mem_stat_max==fil_neg_glue+glue_spec_size-1 {largest statically
  allocated word in the variable-size |mem|}
@#
@d page_ins_head==mem_top {list of insertion data for current page}
@d contrib_head==mem_top-1 {vlist of items not yet on current page}
@d page_head==mem_top-2 {vlist for current page}
@d temp_head==mem_top-3 {head of a temporary list of some kind}
@d hold_head==mem_top-4 {head of a temporary list of another kind}
@d adjust_head==mem_top-5 {head of adjustment list returned by |hpack|}
@d active==mem_top-7 {head of active list in |line_break|, needs two words}
@d align_head==mem_top-8 {head of preamble list for alignments}
@d end_span==mem_top-9 {tail of spanned-width lists}
@d omit_template==mem_top-10 {a constant token list}
@d null_list==mem_top-11 {permanently empty list}
@d lig_trick==mem_top-12 {a ligature masquerading as a |char_node|}
@d garbage==mem_top-12 {used for scrap information}
@d backup_head==mem_top-13 {head of token list built by |scan_keyword|}
@d hi_mem_stat_min==mem_top-13 {smallest statically allocated word in
  the one-word |mem|}
@d hi_mem_stat_usage=14 {the number of one-word nodes always present}
@y
@d zero_glue==mem_bot+1 {specification for \.{0pt plus 0pt minus 0pt}}
@d sfi_glue==zero_glue+glue_spec_size+1 {\.{0pt plus 1fi minus 0pt}}
@d fil_glue==sfi_glue+glue_spec_size+1 {\.{0pt plus 1fil minus 0pt}}
@d fill_glue==fil_glue+glue_spec_size+1 {\.{0pt plus 1fill minus 0pt}}
@d ss_glue==fill_glue+glue_spec_size+1 {\.{0pt plus 1fil minus 1fil}}
@d fil_neg_glue==ss_glue+glue_spec_size+1 {\.{0pt plus -1fil minus 0pt}}
@d lo_mem_stat_max==fil_neg_glue+glue_spec_size+1 {largest statically
  allocated word in the variable-size |mem|}
@#
@d page_ins_head==mem_top {list of insertion data for current page}
@d contrib_head==mem_top-2 {vlist of items not yet on current page}
@d page_head==mem_top-4 {vlist for current page}
@d temp_head==mem_top-6 {head of a temporary list of some kind}
@d hold_head==mem_top-8 {head of a temporary list of another kind}
@d adjust_head==mem_top-10 {head of adjustment list returned by |hpack|}
@d active==mem_top-14 {head of active list in |line_break|, needs two words}
@d align_head==mem_top-16 {head of preamble list for alignments}
@d end_span==mem_top-18 {tail of spanned-width lists}
@d omit_template==mem_top-20 {a constant token list}
@d null_list==mem_top-22 {permanently empty list}
@d lig_trick==mem_top-24 {a ligature masquerading as a |char_node|}
@d garbage==mem_top-24 {used for scrap information}
@d backup_head==mem_top-26 {head of token list built by |scan_keyword|}
@d hi_mem_stat_min==mem_top-28 {smallest statically allocated word in
  the one-word |mem|}
@d hi_mem_stat_usage=14 {the number of one-word nodes always present}
@z
%-------------------------
@x [13] m.202
@p procedure flush_node_list(@!p:pointer); {erase list of nodes starting at |p|}
label done; {go here when node |p| has been freed}
var q:pointer; {successor to node |p|}
begin while p<>null do
@^inner loop@>
  begin q:=link(p);
@y
@p procedure flush_node_list(@!p:pointer); {erase list of nodes starting at |p|}
label done; {go here when node |p| has been freed}
var q:pointer; {successor to node |p|}
begin while p<>null do
@^inner loop@>
  begin q:=link(p); @<LOCAL: Remove info node pointer@>;
@z
%-------------------------
@x [13] m.202
    othercases confusion("flushing")
@:this can't happen flushing}{\quad flushing@>
    endcases;@/
@y
    info_node: begin
               @<LOCAL: Delete info node@>;
               goto done; end; {do not delete node at this time}
    othercases confusion("flushing")
@:this can't happen flushing}{\quad flushing@>
    endcases;@/
@z
%-------------------------
@x [15] m.205
@ @<Make a copy of node |p|...@>=
words:=1; {this setting occurs in more branches than any other}
if is_char_node(p) then r:=get_avail
else @<Case statement to copy different types and set |words| to the number
  of initial words not yet copied@>;
while words>0 do
  begin decr(words); mem[r+words]:=mem[p+words];
  end
@y
@ @<Make a copy of node |p|...@>=
words:=1; {this setting occurs in more branches than any other}
if is_char_node(p) then r:=get_avail
else @<Case statement to copy different types and set |words| to the number
  of initial words not yet copied@>;
@<LOCAL: Copy info node pointer@>;
file_col(r):=file_col(p);
while words>0 do
  begin decr(words); mem[r+words]:=mem[p+words];
  end
@z
%-------------------------
@x [15] m.208 l.4170
@d min_internal=char_ghost+1 {the smallest code that can follow \.{\\the}}
@y
@d assign_local_box=char_ghost+1
   {box for guillemets \.{\\localleftbox} or \.{\\localrightbox}}
@d min_internal=assign_local_box+1 {the smallest code that can follow \.{\\the}}
@z
%-------------------------
@x [17] m.230 l.4860
@d box_base=toks_base+number_regs {table of |number_regs| box registers}
@y
@d local_left_box_base=toks_base+number_regs
@d local_right_box_base=local_left_box_base+1
@d local_par_info_base=local_right_box_base+1
@d box_base=local_par_info_base+1 {table of |number_regs| box registers}
@z
%-------------------------
@x [17] m.230 l.4888
@d box(#)==equiv(box_base+#)
@y
@d local_left_box==equiv(local_left_box_base)
@d local_right_box==equiv(local_right_box_base)
@d local_par_info==equiv(local_par_info_base)
@d box(#)==equiv(box_base+#)
@z
%-------------------------
@x [17] m.236 l.4954
@d int_pars=55 {total number of integer parameters}
@y
@d local_inter_line_penalty_code=55 {local \.{\\interlinepenalty}}
@d local_broken_penalty_code=56 {local \.{\\brokenpenalty}}
@d char_specials_code=57
   {number of characters between column marker specials in dvi file}
@d int_pars=58 {total number of integer parameters}
@z
%-------------------------
@x [17] m.236 l.5011
@d new_line_char==int_par(new_line_char_code)
@y
@d new_line_char==int_par(new_line_char_code)
@d local_inter_line_penalty==int_par(local_inter_line_penalty_code)
@d local_broken_penalty==int_par(local_broken_penalty_code)
@d char_specials==int_par(char_specials_code)
@z
%-------------------------
@x [19] m.269 l.5860
@d max_group_code=16
@y
@d local_box_group=17 {code for `\.{\\localleftbox...\\localrightbox}'}
@d max_group_code=17
@z
%-------------------------
@x [22] m.304
@!line : integer; {current line number in the current source file}
@!line_stack : array[1..max_in_open] of integer;
@y
@!line : integer; {current line number in the current source file}
@!line_stack : array[1..max_in_open] of integer;
@!line_numbers : array[1..max_in_open] of integer;
@!file_names : array[1..max_in_open] of integer;
@!no_of_files : integer; {number of currently open files}

@ @<Set init...@>=
no_of_files:=0;
@z
%-------------------------
@x [23] m.327
@p procedure begin_file_reading;
begin if in_open=max_in_open then overflow("text input levels",max_in_open);
@:TeX capacity exceeded text input levels}{\quad text input levels@>
if first=buf_size then overflow("buffer size",buf_size);
@:TeX capacity exceeded buffer size}{\quad buffer size@>
incr(in_open); push_input; index:=in_open;
line_stack[index]:=line; start:=first; state:=mid_line;
name:=0; {|terminal_input| is now |true|}
end;
@y
@p procedure begin_file_reading;
begin if in_open=max_in_open then overflow("text input levels",max_in_open);
@:TeX capacity exceeded text input levels}{\quad text input levels@>
if first=buf_size then overflow("buffer size",buf_size);
@:TeX capacity exceeded buffer size}{\quad buffer size@>
incr(in_open); push_input; index:=in_open;
line_stack[index]:=line; start:=first; state:=mid_line;
name:=0; {|terminal_input| is now |true|}
@<LOCAL: Change file node@>;
end;
@z
%-------------------------
@x [24] m.362
@ @<Read next line of file into |buffer|, or
  |goto restart| if the file has ended@>=
begin incr(line); first:=start;
if not force_eof then
  begin if input_ln(cur_file,true) then {not end of file}
    firm_up_the_line {this sets |limit|}
  else force_eof:=true;
  end;
if force_eof then
  begin print_char(")"); decr(open_parens);
  update_terminal; {show user that file has been read}
  force_eof:=false;
  end_file_reading; {resume previous level}
  check_outer_validity; goto restart;
  end;
if end_line_char_inactive then decr(limit)
else  buffer[limit]:=end_line_char;
first:=limit+1; loc:=start; {ready to read}
end
@y
@ @<Read next line of file into |buffer|, or
  |goto restart| if the file has ended@>=
begin incr(line); first:=start;
if not force_eof then
  begin if input_ln(cur_file,true) then {not end of file}
    firm_up_the_line {this sets |limit|}
  else force_eof:=true;
  end;
if force_eof then
  begin print_char(")"); decr(open_parens);
  update_terminal; {show user that file has been read}
  force_eof:=false;
  end_file_reading; {resume previous level}
  check_outer_validity;
  if no_of_files>0 then decr(no_of_files);
  @<LOCAL: Change file node@>;
  goto restart;
  end;
if end_line_char_inactive then decr(limit)
else  buffer[limit]:=end_line_char;
first:=limit+1; loc:=start; {ready to read}
line_numbers[no_of_files]:=line;
@<LOCAL: Change file node@>;
end
@z
%-------------------------
@x [29] m.537
done: name:=a_make_name_string(cur_file);
if job_name=0 then
  begin job_name:=cur_name; open_log_file;
  end; {|open_log_file| doesn't |show_context|, so |limit|
    and |loc| needn't be set to meaningful values yet}
if term_offset+length(name)>max_print_line-2 then print_ln
else if (term_offset>0)or(file_offset>0) then print_char(" ");
print_char("("); incr(open_parens); slow_print(name); update_terminal;
state:=new_line;
if name=str_ptr-1 then {we can conserve string pool space now}
  begin flush_string; name:=cur_name;
  end;
@<Read the first line of the new file@>;
@y
done: name:=a_make_name_string(cur_file);
if job_name=0 then
  begin job_name:=cur_name; open_log_file;
  end; {|open_log_file| doesn't |show_context|, so |limit|
    and |loc| needn't be set to meaningful values yet}
if term_offset+length(name)>max_print_line-2 then print_ln
else if (term_offset>0)or(file_offset>0) then print_char(" ");
print_char("("); incr(open_parens); slow_print(name); update_terminal;
state:=new_line;
if name=str_ptr-1 then {we can conserve string pool space now}
  begin flush_string; name:=cur_name;
  end;
incr(no_of_files);
file_names[no_of_files]:=name; {keep track of file names for output specials}
line_numbers[no_of_files]:=0; {keep track of file names for output specials}
@<Read the first line of the new file@>;
@z
%-------------------------
@x [29] m.538
@<Read the first line...@>=
begin line:=1;
if input_ln(cur_file,false) then do_nothing;
firm_up_the_line;
if end_line_char_inactive then decr(limit)
else  buffer[limit]:=end_line_char;
first:=limit+1; loc:=start;
end
@y
@<Read the first line...@>=
begin line:=1;
if input_ln(cur_file,false) then do_nothing;
firm_up_the_line;
if end_line_char_inactive then decr(limit)
else  buffer[limit]:=end_line_char;
first:=limit+1; loc:=start;
line_numbers[no_of_files]:=1; {keep track of file names for output specials}
@<LOCAL: Change file node@>;
end
@z
%-------------------------
@x [30] m.582
@p function new_character(@!f:internal_font_number;@!c:eight_bits):pointer;
label exit;
var p:pointer; {newly allocated node}
begin if font_bc(f)<=c then if font_ec(f)>=c then
  if char_exists(char_info(f)(qi(c))) then
    begin p:=get_avail; font(p):=f; character(p):=qi(c);
    new_character:=p; return;
    end;
char_warning(f,c);
new_character:=null;
exit:end;
@y
@p function new_character(@!f:internal_font_number;@!c:eight_bits):pointer;
label exit;
var p:pointer; {newly allocated node}
begin if font_bc(f)<=c then if font_ec(f)>=c then
  if char_exists(char_info(f)(qi(c))) then
    begin p:=get_avail; font(p):=f; character(p):=qi(c);
    ptr_info(p):=local_par_info; info_incr_ref_count(local_par_info);
    new_character:=p; return;
    end;
char_warning(f,c);
new_character:=null;
exit:end;
@z
%-------------------------
@x [32] m.617
@ @<Initialize variables as |ship_out| begins@>=
dvi_h:=0; dvi_v:=0; cur_h:=h_offset; dvi_f:=null_font;
ensure_dvi_open;
if total_pages=0 then
  begin dvi_out(pre); dvi_out(id_byte); {output the preamble}
@^preamble of \.{DVI} file@>
@y
@ @<Initialize variables as |ship_out| begins@>=
dvi_h:=0; dvi_v:=0; cur_h:=h_offset; dvi_f:=null_font;
ensure_dvi_open;
if total_pages=0 then
  begin dvi_out(pre); dvi_out(id_byte); {output the preamble}
@^preamble of \.{DVI} file@>
  ptr_info(lig_trick):=null;
@z
%-------------------------
@x [32] m.619
@!p:pointer; {current position in the hlist}
@y
@!p:pointer; {current position in the hlist}
@!fff:pointer; {current file position}
@!ffi:integer; {for iterations}
@!g:integer; {new string}
@!newk:integer;
@!should_create_special:boolean;
@!chars_since_last_special:integer;
@z
%-------------------------
@x [32] m.619
while p<>null do @<Output node |p| for |hlist_out| and move to the next node,
  maintaining the condition |cur_v=base_line|@>;
@y
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
@z
%-------------------------
@x [32] m.620
@<Output node |p| for |hlist_out|...@>=
reswitch: if is_char_node(p) then
  begin synch_h; synch_v;
  repeat f:=font(p); c:=character(p);
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
@z
%-------------------------
@x [32] m.622
move_past: cur_h:=cur_h+rule_wd;
next_p:p:=link(p);
end
@y
move_past: cur_h:=cur_h+rule_wd;
next_p:
if type(p)<>kern_node then should_create_special:=true;
p:=link(p);
end
@z
%-------------------------
@x [33] m.645
@p procedure scan_spec(@!c:group_code;@!three_codes:boolean);
@y
@p
@<LOCAL: Declare |make_info_node|@>;
procedure scan_spec(@!c:group_code;@!three_codes:boolean);
@z
%-------------------------
@x [34] m.652
@ @<Make node |p| look like a |char_node| and |goto reswitch|@>=
begin mem[lig_trick]:=mem[lig_char(p)]; link(lig_trick):=link(p);
p:=lig_trick; goto reswitch;
end
@y
@ @<Make node |p| look like a |char_node| and |goto reswitch|@>=
begin
info_decr_ref_count(ptr_info(lig_trick));
mem[lig_trick]:=mem[lig_char(p)];
ptr_info(lig_trick):=ptr_info(p);
info_incr_ref_count(ptr_info(p));
file_col(lig_trick):=file_col(p);
link(lig_trick):=link(p);
p:=lig_trick; goto reswitch;
end
@z
%-------------------------
@x [34] m.682 l.13395 - Omega
@d ord_noad=unset_node+3 {|type| of a noad classified Ord}
@y
@d ord_noad=biggest_ordinary_node+3 {|type| of a noad classified Ord}
@z
%-------------------------
@x [34] m.686
@p function new_noad:pointer;
var p:pointer;
begin p:=get_node(noad_size);
type(p):=ord_noad; subtype(p):=normal;
mem[nucleus(p)].hh:=empty_field;
mem[subscr(p)].hh:=empty_field;
mem[supscr(p)].hh:=empty_field;
new_noad:=p;
end;
@y
@p function new_noad:pointer;
var p:pointer;
begin p:=get_node(noad_size);
type(p):=ord_noad; subtype(p):=normal;
mem[nucleus(p)].hh:=empty_field;
mem[subscr(p)].hh:=empty_field;
mem[supscr(p)].hh:=empty_field;
@<LOCAL: Add info node pointer@>;
add_file_col(p);
new_noad:=p;
end;
@z
%---------------------------------------
@x [34] m.688
@d style_node=unset_node+1 {|type| of a style node}
@y
@d style_node=biggest_ordinary_node+1 {|type| of a style node}
@z
%---------------------------------------
@x [34] m.688
@p function new_style(@!s:small_number):pointer; {create a style node}
var p:pointer; {the new node}
begin p:=get_node(style_node_size); type(p):=style_node;
subtype(p):=s; width(p):=0; depth(p):=0; {the |width| and |depth| are not used}
new_style:=p;
end;
@y
@p function new_style(@!s:small_number):pointer; {create a style node}
var p:pointer; {the new node}
begin p:=get_node(style_node_size); type(p):=style_node;
subtype(p):=s; width(p):=0; depth(p):=0; {the |width| and |depth| are not used}
@<LOCAL: Add info node pointer@>;
add_file_col(p);
new_style:=p;
end;
@z
%---------------------------------------
@x [34] m.689
@d choice_node=unset_node+2 {|type| of a choice node}
@y
@d choice_node=biggest_ordinary_node+2 {|type| of a choice node}
@z
%---------------------------------------
@x [34] m.689
@p function new_choice:pointer; {create a choice node}
var p:pointer; {the new node}
begin p:=get_node(style_node_size); type(p):=choice_node;
subtype(p):=0; {the |subtype| is not used}
display_mlist(p):=null; text_mlist(p):=null; script_mlist(p):=null;
script_script_mlist(p):=null;
new_choice:=p;
end;
@y
@p function new_choice:pointer; {create a choice node}
var p:pointer; {the new node}
begin p:=get_node(style_node_size); type(p):=choice_node;
subtype(p):=0; {the |subtype| is not used}
display_mlist(p):=null; text_mlist(p):=null; script_mlist(p):=null;
script_script_mlist(p):=null;
@<LOCAL: Add info node pointer@>;
add_file_col(p);
new_choice:=p;
end;
@z
%---------------------------------------
@x [36] m.755
@ @<Create a character node |p| for |nucleus(q)|...@>=
begin fetch(nucleus(q));
if char_exists(cur_i) then
  begin delta:=char_italic(cur_f)(cur_i); p:=new_character(cur_f,qo(cur_c));
  if (math_type(nucleus(q))=math_text_char)and(space(cur_f)<>0) then
    delta:=0; {no italic correction in mid-word of text font}
  if (math_type(subscr(q))=empty)and(delta<>0) then
    begin link(p):=new_kern(delta); delta:=0;
    end;
  end
else p:=null;
end
@y
@ @<Create a character node |p| for |nucleus(q)|...@>=
begin fetch(nucleus(q));
if char_exists(cur_i) then
  begin delta:=char_italic(cur_f)(cur_i); p:=new_character(cur_f,qo(cur_c));
  info_decr_ref_count(ptr_info(p));
  ptr_info(p):=ptr_info(q);
  info_incr_ref_count(ptr_info(q));
  file_col(p):=file_col(q);
  if (math_type(nucleus(q))=math_text_char)and(space(cur_f)<>0) then
    delta:=0; {no italic correction in mid-word of text font}
  if (math_type(subscr(q))=empty)and(delta<>0) then
    begin link(p):=new_kern(delta); delta:=0;
    end;
  end
else p:=null;
end
@z
%---------------------------------------
@x [38] m.814 l.15994
@!just_box:pointer; {the |hlist_node| for the last line of the new paragraph}
@y
@!just_box:pointer; {the |hlist_node| for the last line of the new paragraph}
@!init_local_par_info:pointer;
@!last_local_par_info:pointer;
@!current_file_node:pointer;

@ @<Set initial...@>=
init_local_par_info:=null;
last_local_par_info:=null;
current_file_node:=null;

@z
%-------------------------
@x [38] m.821 l.16110
@ The passive node for a given breakpoint contains only four fields:

\yskip\hang|link| points to the passive node created just before this one,
if any, otherwise it is |null|.

\yskip\hang|cur_break| points to the position of this breakpoint in the
horizontal list for the paragraph being broken.

\yskip\hang|prev_break| points to the passive node that should precede this
one in an optimal path to this breakpoint.

\yskip\hang|serial| is equal to |n| if this passive node is the |n|th
one created during the current pass. (This field is used only when
printing out detailed statistics about the line-breaking calculations.)

\yskip\noindent
There is a global variable called |passive| that points to the most
recently created passive node. Another global variable, |printed_node|,
is used to help print out the paragraph when detailed information about
the line-breaking computation is being displayed.

@d passive_node_size=2 {number of words in passive nodes}
@d cur_break==rlink {in passive node, points to position of this breakpoint}
@d prev_break==llink {points to passive node that should precede this one}
@d serial==info {serial number for symbolic identification}
@y
@ The passive node for a given breakpoint contains EIGHT fields:

\yskip\hang|link| points to the passive node created just before this one,
if any, otherwise it is |null|.

\yskip\hang|cur_break| points to the position of this breakpoint in the
horizontal list for the paragraph being broken.

\yskip\hang|prev_break| points to the passive node that should precede this
one in an optimal path to this breakpoint.

\yskip\hang|serial| is equal to |n| if this passive node is the |n|th
one created during the current pass. (This field is used only when
printing out detailed statistics about the line-breaking calculations.)

\yskip\hang|passive_pen_inter| holds the current \.{\\localinterlinepenalty}

\yskip\hang|passive_pen_broken| holds the current \.{\\localbrokenpenalty}

\yskip\noindent
There is a global variable called |passive| that points to the most
recently created passive node. Another global variable, |printed_node|,
is used to help print out the paragraph when detailed information about
the line-breaking computation is being displayed.

@d passive_node_size=4 {number of words in passive nodes}
@d cur_break==rlink {in passive node, points to position of this breakpoint}
@d prev_break==llink {points to passive node that should precede this one}
@d serial==info {serial number for symbolic identification}
@d passive_local(#)==mem[#+2].int
@d passive_prev_local(#)==mem[#+3].int
@z
%-------------------------
@x [38] m.844 l.16595
@<Insert a new active node from |best_place[fit_class]| to |cur_p|@>=
begin q:=get_node(passive_node_size);
link(q):=passive; passive:=q; cur_break(q):=cur_p;
@!stat incr(pass_number); serial(q):=pass_number;@+tats@;@/
prev_break(q):=best_place[fit_class];@/
@y
@<Insert a new active node from |best_place[fit_class]| to |cur_p|@>=
begin q:=get_node(passive_node_size);
link(q):=passive; passive:=q; cur_break(q):=cur_p;
@!stat incr(pass_number); serial(q):=pass_number;@+tats@;@/
prev_break(q):=best_place[fit_class];@/
{Here we keep track of the subparagraph penalties in the break nodes}
passive_local(q):=last_local_par_info;
info_incr_ref_count(last_local_par_info);
if prev_break(q)=null then
  passive_prev_local(q):=init_local_par_info
else
  passive_prev_local(q):=passive_local(prev_break(q));
info_incr_ref_count(passive_prev_local(q));
@z
%-------------------------
@x [38] m.851 l.16706
@<Consider the demerits for a line from |r| to |cur_p|...@>=
begin artificial_demerits:=false;@/
@^inner loop@>
shortfall:=line_width-cur_active_width[1]; {we're this much too short}
@y
@<Consider the demerits for a line from |r| to |cur_p|...@>=
begin artificial_demerits:=false;@/
@^inner loop@>
shortfall:=line_width-cur_active_width[1]; {we're this much too short}
if break_node(r)=null then begin
  if init_local_par_info<>null then
    if info_left_box(init_local_par_info)<>null then
      shortfall:=shortfall-width(info_left_box(init_local_par_info));
  end
else if passive_local(break_node(r))<>null then
  if info_left_box(passive_local(break_node(r)))<>null then
    shortfall:=shortfall-width(info_left_box(passive_local(break_node(r))));
if last_local_par_info<>null then
  if info_right_box(last_local_par_info)<>null then
    shortfall:=shortfall-width(info_right_box(last_local_par_info));
@z
%-------------------------
@x [39] m.863 l.16932
loop@+  begin if threshold>inf_bad then threshold:=inf_bad;
  if second_pass then @<Initialize for hyphenating a paragraph@>;
  @<Create an active breakpoint representing the beginning of the paragraph@>;
  cur_p:=link(temp_head); auto_breaking:=true;@/
  prev_p:=cur_p; {glue at beginning is not a legal breakpoint}
@y
loop@+  begin if threshold>inf_bad then threshold:=inf_bad;
  if second_pass then @<Initialize for hyphenating a paragraph@>;
  @<Create an active breakpoint representing the beginning of the paragraph@>;
  cur_p:=link(temp_head); auto_breaking:=true;@/
  prev_p:=cur_p; {glue at beginning is not a legal breakpoint}
  @<LOCAL: Initialize local paragraph information@>;
@z
%-------------------------
@x [39] m.865
@ @<Clean...@>=
q:=link(active);
while q<>last_active do
  begin cur_p:=link(q);
  if type(q)=delta_node then free_node(q,delta_node_size)
  else free_node(q,active_node_size);
  q:=cur_p;
  end;
q:=passive;
while q<>null do
  begin cur_p:=link(q);
  free_node(q,passive_node_size);
  q:=cur_p;
  end
@y
@ @<Clean...@>=
q:=link(active);
while q<>last_active do
  begin cur_p:=link(q);
  if type(q)=delta_node then free_node(q,delta_node_size)
  else free_node(q,active_node_size);
  q:=cur_p;
  end;
q:=passive;
while q<>null do
  begin cur_p:=link(q);
  info_decr_ref_count(passive_local(q));
  info_decr_ref_count(passive_prev_local(q));
  free_node(q,passive_node_size);
  q:=cur_p;
  end
@z
%-------------------------
@x [39] m.866
@<Call |try_break| if |cur_p| is a legal breakpoint...@>=
begin if is_char_node(cur_p) then
  @<Advance \(c)|cur_p| to the node following the present
    string of characters@>;
@y
@<Call |try_break| if |cur_p| is a legal breakpoint...@>=
begin
@<LOCAL: Update local paragraph information@>;
if is_char_node(cur_p) then
  @<Advance \(c)|cur_p| to the node following the present
    string of characters@>;
@<LOCAL: Update local paragraph information@>;
@z
%-------------------------
@x [39] m.881 l.17289
q:=cur_break(cur_p); disc_break:=false; post_disc_break:=false;
if q<>null then {|q| cannot be a |char_node|}
  if type(q)=glue_node then
    begin delete_glue_ref(glue_ptr(q));
    glue_ptr(q):=right_skip;
    subtype(q):=right_skip_code+1; add_glue_ref(right_skip);
    goto done;
    end
  else  begin if type(q)=disc_node then
      @<Change discretionary to compulsory and set
        |disc_break:=true|@>
    else if (type(q)=math_node)or(type(q)=kern_node) then width(q):=0;
    end
else  begin q:=temp_head;
  while link(q)<>null do q:=link(q);
  end;
@<Put the \(r)\.{\\rightskip} glue after node |q|@>;
@y
q:=cur_break(cur_p); disc_break:=false; post_disc_break:=false;
if q<>null then {|q| cannot be a |char_node|}
  if type(q)=glue_node then begin
    if passive_local(cur_p)<>null then 
      if info_right_box(passive_local(cur_p))<>null then begin
        r:=temp_head;
        while link(r)<>q do r:=link(r);
        s:=copy_node_list(info_right_box(passive_local(cur_p)));
        link(r):=s;
        link(s):=q;
        end;
    delete_glue_ref(glue_ptr(q));
    glue_ptr(q):=right_skip;
    subtype(q):=right_skip_code+1; add_glue_ref(right_skip);
    goto done;
    end
  else  begin if type(q)=disc_node then
      @<Change discretionary to compulsory and set
        |disc_break:=true|@>
    else if (type(q)=math_node)or(type(q)=kern_node) then width(q):=0;
    end
else  begin q:=temp_head;
  while link(q)<>null do q:=link(q);
  end;
if passive_local(cur_p)<>null then 
  if info_right_box(passive_local(cur_p))<>null then begin
    r:=copy_node_list(info_right_box(passive_local(cur_p)));
    link(r):=link(q);
    link(q):=r;
    q:=r;
    end;
@<Put the \(r)\.{\\rightskip} glue after node |q|@>;
@z
%-------------------------
@x [39] m.887 l.17352
@<Put the \(l)\.{\\leftskip} glue at the left...@>=
r:=link(q); link(q):=null; q:=link(temp_head); link(temp_head):=r;
if left_skip<>zero_glue then
  begin r:=new_param_glue(left_skip_code);
  link(r):=q; q:=r;
  end
@y
@<Put the \(l)\.{\\leftskip} glue at the left...@>=
r:=link(q); link(q):=null; q:=link(temp_head); link(temp_head):=r;
if passive_prev_local(cur_p)<>null then
  if info_left_box(passive_prev_local(cur_p))<>null then begin
    r:=copy_node_list(info_left_box(passive_prev_local(cur_p)));
    s:=link(q);
    link(r):=q;
    q:=r;
    {adjustment for the indent node}
    if (cur_line=prev_graf+1) and (s<>null) then
      if type(s)=hlist_node then
        if list_ptr(s)=null then begin
          q:=link(q);
          link(r):=link(s);
          link(s):=r;
          end;
    end;
if left_skip<>zero_glue then
  begin r:=new_param_glue(left_skip_code);
  link(r):=q; q:=r;
  end
@z
%-------------------------
@x [39] m.890 l.17391
@<Append a penalty node, if a nonzero penalty is appropriate@>=
if cur_line+1<>best_line then
  begin pen:=inter_line_penalty;
  if cur_line=prev_graf+1 then pen:=pen+club_penalty;
  if cur_line+2=best_line then pen:=pen+final_widow_penalty;
  if disc_break then pen:=pen+broken_penalty;
@y
@<Append a penalty node, if a nonzero penalty is appropriate@>=
if cur_line+1<>best_line then begin
  if passive_local(cur_p)<>null then begin
    if info_pen_inter(passive_local(cur_p))<>0 then
      pen:=info_pen_inter(passive_local(cur_p))
    else pen:=inter_line_penalty;
    end
  else pen:=inter_line_penalty;
  if cur_line=prev_graf+1 then pen:=pen+club_penalty;
  if cur_line+2=best_line then pen:=pen+final_widow_penalty;
  if disc_break then
    if passive_local(cur_p)<>null then begin
      if info_pen_broken(passive_local(cur_p))<>0 then
        pen:=pen+info_pen_broken(passive_local(cur_p))
      else pen:=pen+broken_penalty;
      end
    else pen:=pen+broken_penalty;
@z
%-------------------------
@x [46] m.1034
fast_get_avail(lig_stack); font(lig_stack):=main_f; cur_l:=qi(cur_chr);
character(lig_stack):=cur_l;@/
@y
fast_get_avail(lig_stack); font(lig_stack):=main_f; cur_l:=qi(cur_chr);
ptr_info(lig_stack):=local_par_info; info_incr_ref_count(local_par_info);
add_file_col(lig_stack);
character(lig_stack):=cur_l;@/
@z
%-------------------------
@x [46] m.1038
fast_get_avail(lig_stack); font(lig_stack):=main_f;
cur_r:=qi(cur_chr); character(lig_stack):=cur_r;
@y
fast_get_avail(lig_stack); font(lig_stack):=main_f;
ptr_info(lig_stack):=local_par_info; info_incr_ref_count(local_par_info);
add_file_col(lig_stack);
cur_r:=qi(cur_chr); character(lig_stack):=cur_r;
@z
%-------------------------
@x [47] m.1114 l.21322
primitive("-",discretionary,1);
@!@:Single-character primitives -}{\quad\.{\\-}@>
primitive("discretionary",discretionary,0);
@!@:discretionary_}{\.{\\discretionary} primitive@>

@ @<Cases of |print_cmd_chr|...@>=
discretionary: if chr_code=1 then
  print_esc("-")@+else print_esc("discretionary");

@ @<Cases of |main_control| that build...@>=
hmode+discretionary,mmode+discretionary: append_discretionary;
@y
primitive("-",discretionary,1);
@!@:Single-character primitives -}{\quad\.{\\-}@>
primitive("discretionary",discretionary,0);
@!@:discretionary_}{\.{\\discretionary} primitive@>
primitive("localleftbox",assign_local_box,0);
@!@:assign_local_box}{\.{\\localleftbox} primitive@>
primitive("localrightbox",assign_local_box,1);
@!@:assign_local_box}{\.{\\localrightbox} primitive@>

@ @<Cases of |print_cmd_chr|...@>=
discretionary: if chr_code=1 then
  print_esc("-")@+else print_esc("discretionary");
assign_local_box: if chr_code=0 then print_esc("localleftbox")
                  else print_esc("localrightbox");

@ @<Cases of |main_control| that build...@>=
hmode+discretionary,mmode+discretionary: append_discretionary;
any_mode(assign_local_box): append_local_box(cur_chr);
@z
%-------------------------
@x [47] m.1117 l.21338
procedure append_discretionary;
var c:integer; {hyphen character}
begin tail_append(new_disc);
if cur_chr=1 then
  begin c:=hyphen_char(cur_font);
  if c>=0 then if c<=biggest_char then
     pre_break(tail):=new_character(cur_font,c);
  end
else  begin incr(save_ptr); saved(-1):=0; new_save_level(disc_group);
  scan_left_brace; push_nest; mode:=-hmode; space_factor:=1000;
  end;
end;
@y
procedure append_local_box(kind:integer);
begin 
incr(save_ptr); saved(-1):=kind; new_save_level(local_box_group);
scan_left_brace; push_nest; mode:=-hmode; space_factor:=1000;
end;

procedure append_discretionary;
var c:integer; {hyphen character}
begin tail_append(new_disc);
if cur_chr=1 then
  begin c:=hyphen_char(cur_font);
  if c>=0 then if c<=biggest_char then begin
     pre_break(tail):=new_character(cur_font,c);
     info_decr_ref_count(ptr_info(pre_break(tail)));
     ptr_info(pre_break(tail)):=local_par_info;
     info_incr_ref_count(local_par_info);
     add_file_col(pre_break(tail));
     end
  end
else  begin incr(save_ptr); saved(-1):=0; new_save_level(disc_group);
  scan_left_brace; push_nest; mode:=-hmode; space_factor:=1000;
  end;
end;
@z
%-------------------------
@x [47] m.1118 l.21355
disc_group: build_discretionary;
@y
disc_group: build_discretionary;
local_box_group: build_local_box;
@z
%-------------------------
@x [47] m.1119 l.21358
procedure build_discretionary;
@y
procedure build_local_box;
var p:pointer;
    kind:integer;
begin
unsave;
kind:=saved(-1);
decr(save_ptr);
p:=link(head); pop_nest;
p:=hpack(p,natural);
if kind=0 then
  eq_define(local_left_box_base,box_ref,p)
else
  eq_define(local_right_box_base,box_ref,p);
@<LOCAL: Change info node@>;
end;

procedure build_discretionary;
@z
%-------------------------
@x [47] m.1123
procedure make_accent;
var s,@!t: real; {amount of slant}
@!p,@!q,@!r:pointer; {character, box, and kern nodes}
@!f:internal_font_number; {relevant font}
@!a,@!h,@!x,@!w,@!delta:scaled; {heights and widths, as explained above}
@!i:four_quarters; {character information}
begin scan_char_num; f:=cur_font; p:=new_character(f,cur_val);
if p<>null then
  begin x:=x_height(f); s:=slant(f)/float_constant(65536);
@^real division@>
  a:=char_width(f)(char_info(f)(character(p)));@/
  do_assignments;@/
  @<Create a character node |q| for the next character,
    but set |q:=null| if problems arise@>;
  if q<>null then @<Append the accent with appropriate kerns,
      then set |p:=q|@>;
  link(tail):=p; tail:=p; space_factor:=1000;
  end;
end;
@y
procedure make_accent;
var s,@!t: real; {amount of slant}
@!p,@!q,@!r:pointer; {character, box, and kern nodes}
@!f:internal_font_number; {relevant font}
@!a,@!h,@!x,@!w,@!delta:scaled; {heights and widths, as explained above}
@!i:four_quarters; {character information}
begin scan_char_num; f:=cur_font; p:=new_character(f,cur_val);
@<LOCAL: Add info node pointer@>;
add_file_col(p);
if p<>null then
  begin x:=x_height(f); s:=slant(f)/float_constant(65536);
@^real division@>
  a:=char_width(f)(char_info(f)(character(p)));@/
  do_assignments;@/
  @<Create a character node |q| for the next character,
    but set |q:=null| if problems arise@>;
  if q<>null then @<Append the accent with appropriate kerns,
      then set |p:=q|@>;
  link(tail):=p; tail:=p; space_factor:=1000;
  end;
end;
@z
%-------------------------
@x [47] m.1124
@ @<Create a character node |q| for the next...@>=
q:=null; f:=cur_font;
if (cur_cmd=letter)or(cur_cmd=other_char)or(cur_cmd=char_given) then
  q:=new_character(f,cur_chr)
else if cur_cmd=char_num then
  begin scan_char_num; q:=new_character(f,cur_val);
  end
else back_input
@y
@ @<Create a character node |q| for the next...@>=
q:=null; f:=cur_font;
if (cur_cmd=letter)or(cur_cmd=other_char)or(cur_cmd=char_given) then begin
  q:=new_character(f,cur_chr);
  info_decr_ref_count(ptr_info(q));
  ptr_info(q):=local_par_info;
  info_incr_ref_count(local_par_info);
  add_file_col(q);
  end
else if cur_cmd=char_num then
  begin scan_char_num; q:=new_character(f,cur_val);
  info_decr_ref_count(ptr_info(q));
  ptr_info(q):=local_par_info;
  info_incr_ref_count(local_par_info);
  add_file_col(q);
  end
else back_input
@z
%-------------------------
@x [49] m.1228 l.22908
assign_int: begin p:=cur_chr; scan_optional_equals; scan_int;
  word_define(p,cur_val);
@y
assign_int: begin p:=cur_chr; scan_optional_equals; scan_int;
  word_define(p,cur_val);
{If we are defining subparagraph penalty levels, then we
 must change the local paragraph node.}
  if ((p=(int_base+local_inter_line_penalty_code)) or
      (p=(int_base+local_broken_penalty_code))) then
    @<LOCAL: Change info node@>;
@z
%-------------------------
@x [53] m.1344 l.24546
primitive("setlanguage",extension,set_language_code);@/
@!@:set_language_}{\.{\\setlanguage} primitive@>
@y
primitive("setlanguage",extension,set_language_code);@/
@!@:set_language_}{\.{\\setlanguage} primitive@>
primitive("localinterlinepenalty",assign_int,
          int_base+local_inter_line_penalty_code);@/
primitive("localbrokenpenalty",assign_int,
          int_base+local_broken_penalty_code);@/
primitive("charspecials",assign_int,
          int_base+char_specials_code);@/
@z
%-------------------------
@x [53] m.1354
@<Implement \.{\\special}@>=
begin new_whatsit(special_node,write_node_size); write_stream(tail):=null;
p:=scan_toks(false,true); write_tokens(tail):=def_ref;
end
@y
@<Implement \.{\\special}@>=
begin new_whatsit(special_node,write_node_size); write_stream(tail):=null;
ptr_info(tail):=local_par_info;
info_incr_ref_count(local_par_info);
add_file_col(tail);
p:=scan_toks(false,true); write_tokens(tail):=def_ref;
end
@z
%-------------------------
@x [53] m.1368
@<Declare procedures needed in |hlist_out|, |vlist_out|@>=
procedure special_out(@!p:pointer);
var old_setting:0..max_selector; {holds print |selector|}
@!k:pool_pointer; {index into |str_pool|}
begin synch_h; synch_v;@/
@y
@<Declare procedures needed in |hlist_out|, |vlist_out|@>=
procedure append_string(s:integer);
var p:pointer;
    i:integer;
begin
if (s<@"10000) then append_char(s)
else begin
  i:=str_start(s);
  while i<str_start(s+1) do begin
    append_char(str_pool[i]);
    incr(i);
    end;
  end;
end;

procedure append_int(n:integer);
var p:pointer;
    k:integer;
begin
k:=0;
repeat
  dig[k] := n mod 10;
  n := n div 10;
  incr(k);
until n=0;
while k>0 do begin
  decr(k);
  append_char("0"+dig[k]);
  end;
end;

procedure special_out(@!p:pointer);
var old_setting:0..max_selector; {holds print |selector|}
@!k:pool_pointer; {index into |str_pool|}
@!fff:pointer; {current file position}
@!ffi:integer; {for iterations}
@!g:integer; {new string}
@!newk:integer;
begin synch_h; synch_v;@/
if ptr_info(p)<>null then 
  if info_file(ptr_info(p))<>null then 
    @<LOCAL: Create special node@>;
@z
%-------------------------
@x [53] m.1378 l.24900
@ @<Finish the extensions@>=
for k:=0 to 15 do if write_open[k] then a_close(write_file[k])
@y
@ @<Finish the extensions@>=
for k:=0 to 15 do if write_open[k] then a_close(write_file[k])

@ @<LOCAL: Declare |make_info_node|@>=
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

procedure make_file_node;
var p:pointer;
    i:integer;
begin
p:=get_node(file_size+2*no_of_files); type(p):=file_node;
file_no(p):=no_of_files;
file_ref_count(p):=1;
for i := 1 to no_of_files do begin
  mem[p+(i*2)].int := file_names[i];
  mem[p+(i*2)+1].int := line_numbers[i];
  end;
file_decr_ref_count(current_file_node);
current_file_node:=p;
make_info_node;
end

@ @<LOCAL: Initialize local paragraph information@>=
begin
info_decr_ref_count(init_local_par_info);
init_local_par_info:=ptr_info(cur_p);
info_incr_ref_count(init_local_par_info);
info_decr_ref_count(last_local_par_info);
last_local_par_info:=ptr_info(cur_p);
info_incr_ref_count(last_local_par_info);
end

@ @<LOCAL: Update local paragraph information@>=
begin
if last_local_par_info<>ptr_info(cur_p) then begin
  info_decr_ref_count(last_local_par_info);
  last_local_par_info:=ptr_info(cur_p);
  info_incr_ref_count(last_local_par_info);
  end;
end

@ @<LOCAL: print out |local_paragraph| node@>=
info_node: begin
  print_esc("info");
  append_char(".");
  print_ln; print_current_string;
  print_esc("localinterlinepenalty"); print("=");
  print_int(info_pen_inter(p));
  print_ln; print_current_string;
  print_esc("localbrokenpenalty"); print("=");
  print_int(info_pen_broken(p));
  print_ln; print_current_string;
  print_esc("localleftbox");
  if info_left_box(p)=null then print("=null")
  else begin
    append_char(".");
    show_node_list(info_left_box(p));
    decr(pool_ptr);
    end;
  print_ln; print_current_string;
  print_esc("localrightbox");
  if info_right_box(p)=null then print("=null")
  else begin
    append_char(".");
    show_node_list(info_right_box(p));
    decr(pool_ptr);
    end;
  decr(pool_ptr);
  end

@ @<LOCAL: Change file node@>=
begin
make_file_node;
end

@ @<LOCAL: Change info node@>=
begin
make_info_node;
end

@ @<LOCAL: Delete info node@>=
begin
info_decr_ref_count(p);
end

@ @<LOCAL: Add info node pointer@>=
begin
ptr_info(p):=local_par_info;
info_incr_ref_count(local_par_info);
end

@ @<LOCAL: Copy info node pointer@>=
begin
ptr_info(r):=ptr_info(p);
info_incr_ref_count(ptr_info(p));
end

@ @<LOCAL: Remove info node pointer@>=
begin
info_decr_ref_count(ptr_info(p));
ptr_info(p):=null;
end

@ @<LOCAL: Create special node@>=
begin
fff:=info_file(ptr_info(p));
append_string("om:");
append_string("lines=");
for ffi:=1 to file_no(fff) do begin
  if mem[fff+ffi*2].int <> 0 then begin
    append_char("""");
    append_string(mem[fff+ffi*2].int);
    append_char("""");
    append_char(",");
    append_int(mem[fff+ffi*2+1].int);
    append_char(";");
    end;
  end;
g:=make_string;
if length(g)<256 then
  begin dvi_out(xxx1); dvi_out(length(g));
  end
else  begin dvi_out(xxx4); dvi_four(length(g));
  end;
for newk:=str_start(g) to pool_ptr-1 do dvi_out(so(str_pool[newk]));
flush_string; {erase the string}
append_string("om:");
append_string("col=");
append_int(file_col(p));
append_char(";");
g:=make_string;
if length(g)<256 then
  begin dvi_out(xxx1); dvi_out(length(g));
  end
else  begin dvi_out(xxx4); dvi_four(length(g));
  end;
for newk:=str_start(g) to pool_ptr-1 do dvi_out(so(str_pool[newk]));
flush_string; {erase the string}
end

@z
%-------------------------
