/****************************************************************************\
 Part of the XeTeX typesetting system
 copyright (c) 1994-2008 by SIL International
 copyright (c) 2009, 2011 by Jonathan Kew

 Written by Jonathan Kew

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the copyright holders
shall not be used in advertising or otherwise to promote the sale,
use or other dealings in this Software without prior written
authorization from the copyright holders.
\****************************************************************************/

% When updating code here, also increment version number/string in xetex.ch

% \vadjust
@x l.3142
@d adjust_node=5 {|type| of an adjust node}
@y
@d adjust_node=5 {|type| of an adjust node}
@d adjust_pre == subtype  {<>0 => pre-adjustment}
@#{|append_list| is used to append a list to |tail|}
@d append_list(#) == begin link(tail) := link(#); append_list_end
@d append_list_end(#) == tail := #; end
@z

% \vadjust
@x l.3581
@d backup_head==mem_top-13 {head of token list built by |scan_keyword|}
@d hi_mem_stat_min==mem_top-13 {smallest statically allocated word in
  the one-word |mem|}
@d hi_mem_stat_usage=14 {the number of one-word nodes always present}
@y
@d backup_head==mem_top-13 {head of token list built by |scan_keyword|}
@d pre_adjust_head==mem_top-14  {head of pre-adjustment list returned by |hpack|}
@d hi_mem_stat_min==mem_top-14 {smallest statically allocated word in
  the one-word |mem|}
@d hi_mem_stat_usage=15 {the number of one-word nodes always present}
@z

% \vadjust
@x l.4145
begin print_esc("vadjust"); node_list_display(adjust_ptr(p)); {recursive call}
@y
begin print_esc("vadjust"); if adjust_pre(p) <> 0 then print(" pre ");
node_list_display(adjust_ptr(p)); {recursive call}
@z

% \primitive
@x l.4868
@d frozen_null_font=frozen_control_sequence+11
  {permanent `\.{\\nullfont}'}
@y
@d frozen_primitive=frozen_control_sequence+11
  {permanent `\.{\\primitive}'}
@d frozen_null_font=frozen_control_sequence+12
  {permanent `\.{\\nullfont}'}
@z

% \primitive
% \ifprimitive
@x l.5942
@!cs_count:integer; {total number of known identifiers}

@ @<Set init...@>=
no_new_control_sequence:=true; {new identifiers are usually forbidden}

@ @<Initialize table entries...@>=
hash_used:=frozen_control_sequence; {nothing is used}
hash_high:=0;
cs_count:=0;
eq_type(frozen_dont_expand):=dont_expand;
text(frozen_dont_expand):="notexpanded:";
@.notexpanded:@>
@y
@!cs_count:integer; {total number of known identifiers}

@ Primitive support needs a few extra variables and definitions

@d prim_size=480 {maximum number of primitives }
@d prim_prime=409 {about 85\pct! of |primitive_size|}
@d prim_base=1
@d prim_next(#) == prim[#].lh {link for coalesced lists}
@d prim_text(#) == prim[#].rh {string number for control sequence name}
@d prim_is_full == (prim_used=prim_base) {test if all positions are occupied}
@d prim_eq_level_field(#)==#.hh.b1
@d prim_eq_type_field(#)==#.hh.b0
@d prim_equiv_field(#)==#.hh.rh
@d prim_eq_level(#)==prim_eq_level_field(prim_eqtb[#]) {level of definition}
@d prim_eq_type(#)==prim_eq_type_field(prim_eqtb[#]) {command code for equivalent}
@d prim_equiv(#)==prim_equiv_field(prim_eqtb[#]) {equivalent value}
@d undefined_primitive=0

@<Glob...@>=
@!prim: array [0..prim_size] of two_halves;  {the primitives table}
@!prim_used:pointer; {allocation pointer for |prim|}
@!prim_eqtb:array[0..prim_size] of memory_word;

@ @<Set init...@>=
no_new_control_sequence:=true; {new identifiers are usually forbidden}
prim_next(0):=0; prim_text(0):=0;
for k:=1 to prim_size do prim[k]:=prim[0];
prim_eq_level(0) := level_zero;
prim_eq_type(0) := undefined_cs;
prim_equiv(0) := null;
for k:=1 to prim_size do prim_eqtb[k]:=prim_eqtb[0];

@ @<Initialize table entries...@>=
prim_used:=prim_size; {nothing is used}
hash_used:=frozen_control_sequence; {nothing is used}
hash_high:=0;
cs_count:=0;
eq_type(frozen_dont_expand):=dont_expand;
text(frozen_dont_expand):="notexpanded:";
@.notexpanded:@>

eq_type(frozen_primitive):=ignore_spaces;
equiv(frozen_primitive):=1;
eq_level(frozen_primitive):=level_one;
text(frozen_primitive):="primitive";
@z

% \primitive
% \ifprimitive
@x l.6029
@ Single-character control sequences do not need to be looked up in a hash
@y
@ Here is the subroutine that searches the primitive table for an identifier

@p function prim_lookup(@!s:str_number):pointer; {search the primitives table}
label found; {go here if you found it}
var h:integer; {hash code}
@!p:pointer; {index in |hash| array}
@!k:pointer; {index in string pool}
@!j,@!l:integer;
begin
if s<256 then begin
  p := s;
  if (p<0) or (prim_eq_level(p)<>level_one) then
    p := undefined_primitive;
end
else begin
  j:=str_start_macro(s);
  if s = str_ptr then l := cur_length else l := length(s);
  @<Compute the primitive code |h|@>;
  p:=h+prim_base; {we start searching here; note that |0<=h<hash_prime|}
  loop@+begin if prim_text(p)>0 then if length(prim_text(p))=l then
    if str_eq_str(prim_text(p),s) then goto found;
    if prim_next(p)=0 then
      begin if no_new_control_sequence then
        p:=undefined_primitive
      else @<Insert a new primitive after |p|, then make
        |p| point to it@>;
      goto found;
      end;
    p:=prim_next(p);
    end;
  end;
found: prim_lookup:=p;
end;

@ @<Insert a new primitive...@>=
begin if prim_text(p)>0 then
  begin repeat if prim_is_full then overflow("primitive size",prim_size);
@:TeX capacity exceeded primitive size}{\quad primitive size@>
  decr(prim_used);
  until prim_text(prim_used)=0; {search for an empty location in |prim|}
  prim_next(p):=prim_used; p:=prim_used;
  end;
prim_text(p):=s;
end

@ The value of |prim_prime| should be roughly 85\pct! of
|prim_size|, and it should be a prime number.

@<Compute the primitive code |h|@>=
h:=str_pool[j];
for k:=j+1 to j+l-1 do
  begin h:=h+h+str_pool[k];
  while h>=prim_prime do h:=h-prim_prime;
  end

@ Single-character control sequences do not need to be looked up in a hash
@z

% \primitive
@x
@p @!init procedure primitive(@!s:str_number;@!c:quarterword;@!o:halfword);
var k:pool_pointer; {index into |str_pool|}
@!j:0..buf_size; {index into |buffer|}
@!l:small_number; {length of the string}
begin if s<256 then cur_val:=s+single_base
else  begin k:=str_start_macro(s); l:=str_start_macro(s+1)-k;
    {we will move |s| into the (possibly non-empty) |buffer|}
  if first+l>buf_size+1 then
      overflow("buffer size",buf_size);
@:TeX capacity exceeded buffer size}{\quad buffer size@>
  for j:=0 to l-1 do buffer[first+j]:=so(str_pool[k+j]);
  cur_val:=id_lookup(first,l); {|no_new_control_sequence| is |false|}
  flush_string; text(cur_val):=s; {we don't want to have the string twice}
  end;
eq_level(cur_val):=level_one; eq_type(cur_val):=c; equiv(cur_val):=o;
end;
@y
@p @!init procedure primitive(@!s:str_number;@!c:quarterword;@!o:halfword);
var k:pool_pointer; {index into |str_pool|}
@!j:0..buf_size; {index into |buffer|}
@!l:small_number; {length of the string}
@!prim_val:integer; {needed to fill |prim_eqtb|}
begin if s<256 then begin
  cur_val:=s+single_base;
  prim_val:=s;
end
else  begin k:=str_start_macro(s); l:=str_start_macro(s+1)-k;
    {we will move |s| into the (possibly non-empty) |buffer|}
  if first+l>buf_size+1 then
      overflow("buffer size",buf_size);
@:TeX capacity exceeded buffer size}{\quad buffer size@>
  for j:=0 to l-1 do buffer[first+j]:=so(str_pool[k+j]);
  cur_val:=id_lookup(first,l); {|no_new_control_sequence| is |false|}
  flush_string; text(cur_val):=s; {we don't want to have the string twice}
  prim_val:=prim_lookup(s);
  end;
eq_level(cur_val):=level_one; eq_type(cur_val):=c; equiv(cur_val):=o;
prim_eq_level(prim_val):=level_one;
prim_eq_type(prim_val):=c;
prim_equiv(prim_val):=o;
end;
@z

% \primitive
@x l.6163
@!@:no_expand_}{\.{\\noexpand} primitive@>
@y
@!@:no_expand_}{\.{\\noexpand} primitive@>
primitive("primitive",no_expand,1);@/
@!@:primitive_}{\.{\\primitive} primitive@>
@z

% \primitive
@x l.6224
ignore_spaces: print_esc("ignorespaces");
@y
ignore_spaces: if chr_code=0 then print_esc("ignorespaces") else print_esc("primitive");
@z

% \primitive
@x l.6239
no_expand: print_esc("noexpand");
@y
no_expand: if chr_code=0 then print_esc("noexpand")
   else print_esc("primitive");
@z

% \ifincsname
@x
var t:halfword; {token that is being ``expanded after''}
@y
var t:halfword; {token that is being ``expanded after''}
@!b:boolean; {keep track of nested csnames}
@z

% \ifincsname
@x l.8348
@ @<Expand a nonmacro@>=
@y
@ @<Glob...@>=
@!is_in_csname: boolean;

@ @<Set init...@>=
is_in_csname := false;

@ @<Expand a nonmacro@>=
@z

% \primitive
@x l.8354
no_expand:@<Suppress expansion of the next token@>;
@y
no_expand: if cur_chr=0 then @<Suppress expansion of the next token@>
  else @<Implement \.{\\primitive}@>;
@z

% \primitive
@x l.8391
@ @<Complain about an undefined macro@>=
@y
@ The \.{\\primitive} handling. If the primitive meaning of the next
token is an expandable command, it suffices to replace the current
token with the primitive one and restart |expand|/

Otherwise, the token we just read has to be pushed back, as well
as a token matching the internal form of \.{\\primitive}, that is
sneaked in as an alternate form of |ignore_spaces|.
@!@:primitive_}{\.{\\primitive} primitive (internalized)@>

Simply pushing back a token that matches the correct internal command
does not work, because approach would not survive roundtripping to a
temporary file.

@<Implement \.{\\primitive}@>=
begin save_scanner_status := scanner_status; scanner_status:=normal;
get_token; scanner_status:=save_scanner_status;
if cur_cs < hash_base then
  cur_cs := prim_lookup(cur_cs-257)
else
  cur_cs := prim_lookup(text(cur_cs));
if cur_cs<>undefined_primitive then begin
  t := prim_eq_type(cur_cs);
  if t>max_command then begin
    cur_cmd := t;
    cur_chr := prim_equiv(cur_cs);
    cur_tok := (cur_cmd*max_char_val)+cur_chr;
    cur_cs  := 0;
    goto reswitch;
    end
  else begin
    back_input; { now |loc| and |start| point to a one-item list }
    p:=get_avail; info(p):=cs_token_flag+frozen_primitive;
    link(p):=loc; loc:=p; start:=p;
    end;
  end;
end

@ @<Complain about an undefined macro@>=
@z

% \ifincsname
@x
begin r:=get_avail; p:=r; {head of the list of characters}
@y
begin r:=get_avail; p:=r; {head of the list of characters}
b := is_in_csname; is_in_csname := true;
@z

% \ifincsname
@x
if cur_cmd<>end_cs_name then @<Complain about missing \.{\\endcsname}@>;
@y
if cur_cmd<>end_cs_name then @<Complain about missing \.{\\endcsname}@>;
is_in_csname := b;
@z

% \strcmp
% \shellescape
@x l.29172
@d pdf_last_x_pos_code        = XeTeX_int+27
@d pdf_last_y_pos_code        = XeTeX_int+28

@d XeTeX_pdf_page_count_code  = XeTeX_int+29

@#
@d XeTeX_dim=XeTeX_int+30 {first of \XeTeX\ codes for dimensions}
@y
@d pdf_last_x_pos_code        = XeTeX_int+27
@d pdf_last_y_pos_code        = XeTeX_int+28
@d pdf_strcmp_code            = XeTeX_int+29
@d pdf_shell_escape_code      = XeTeX_int+30

@d XeTeX_pdf_page_count_code  = XeTeX_int+31

@#
@d XeTeX_dim=XeTeX_int+32 {first of \XeTeX\ codes for dimensions}
@z

% \shellescape
@x l.9244
  input_line_no_code: print_esc("inputlineno");
@y
  input_line_no_code: print_esc("inputlineno");
  pdf_shell_escape_code: print_esc("shellescape");
@z

@x l.9322
  badness_code: cur_val:=last_badness;
@y
  badness_code: cur_val:=last_badness;
  pdf_shell_escape_code:
    begin
    if shellenabledp then begin
      if restrictedshell then cur_val := 2
      else cur_val := 1;
    end
    else cur_val := 0;
    end;
@z

% \strcmp
@x l.10165
  font_name_code: print_esc("fontname");
@y
  font_name_code:  print_esc("fontname");
  pdf_strcmp_code: print_esc("strcmp");
@z

% \strcmp
@x l.10174
@p procedure conv_toks;
var old_setting:0..max_selector; {holds |selector| setting}
@y
The extra temp string |u| is needed because |pdf_scan_ext_toks| incorporates
any pending string in its output. In order to save such a pending string,
we have to create a temporary string that is destroyed immediately after.

@d save_cur_string==if str_start_macro(str_ptr)<pool_ptr then u:=make_string else u:=0
@d restore_cur_string==if u<>0 then decr(str_ptr)

@p procedure conv_toks;
var old_setting:0..max_selector; {holds |selector| setting}
@!save_warning_index, @!save_def_ref:pointer;
@!u: str_number;
@z

% \ifprimitive
@x l.10483
@d if_case_code=16 { `\.{\\ifcase}' }
@y
@d if_case_code=16 { `\.{\\ifcase}' }
@d if_primitive_code=21 { `\.{\\ifprimitive}' }
@z

% \ifprimitive
@x l.10519
@!@:if_case_}{\.{\\ifcase} primitive@>
@y
@!@:if_case_}{\.{\\ifcase} primitive@>
primitive("ifprimitive",if_test,if_primitive_code);
@!@:if_primitive_}{\.{\\ifprimitive} primitive@>
@z

% \ifprimitive
@x l.10539
  if_case_code:print_esc("ifcase");
@y
  if_case_code:print_esc("ifcase");
  if_primitive_code:print_esc("ifprimitive");
@z

% \ifincsname
@x
var b:boolean; {is the condition true?}
@y
var b:boolean; {is the condition true?}
@!e:boolean; {keep track of nested csnames}
@z

% \ifprimitive
@x l.10723
if_case_code: @<Select the appropriate case
  and |return| or |goto common_ending|@>;
@y
if_case_code: @<Select the appropriate case
  and |return| or |goto common_ending|@>;
if_primitive_code: begin
  save_scanner_status:=scanner_status;
  scanner_status:=normal;
  get_next;
  scanner_status:=save_scanner_status;
  if cur_cs < hash_base then
    m := prim_lookup(cur_cs-257)
  else
    m := prim_lookup(text(cur_cs));
  b :=((cur_cmd<>undefined_cs) and
       (m<>undefined_primitive) and
       (cur_cmd=prim_eq_type(m)) and
       (cur_chr=prim_equiv(m)));
  end;
@z

% \vadjust
@x l.14527
if adjust_tail<>null then link(adjust_tail):=null;
@y
if adjust_tail<>null then link(adjust_tail):=null;
if pre_adjust_tail<>null then link(pre_adjust_tail):=null;
@z

% \vadjust
@x
  ins_node,mark_node,adjust_node: if adjust_tail<>null then
@y
  ins_node,mark_node,adjust_node: if (adjust_tail<>null) or (pre_adjust_tail<> null) then
@z


% \vadjust
@x l.14607
@<Transfer node |p| to the adjustment list@>=
begin while link(q)<>p do q:=link(q);
if type(p)=adjust_node then
  begin link(adjust_tail):=adjust_ptr(p);
  while link(adjust_tail)<>null do adjust_tail:=link(adjust_tail);
  p:=link(p); free_node(link(q),small_node_size);
  end
else  begin link(adjust_tail):=p; adjust_tail:=p; p:=link(p);
  end;
link(q):=p; p:=q;
end
@y
@<Glob...@>=
@!pre_adjust_tail: pointer;

@ @<Set init...@>=
pre_adjust_tail := null;

@ Materials in \.{\\vadjust} used with \.{pre} keyword will be appended to
|pre_adjust_tail| instead of |adjust_tail|.

@d update_adjust_list(#) == begin
    if # = null then
        confusion("pre vadjust");
    link(#) := adjust_ptr(p);
    while link(#) <> null do
        # := link(#);
end

@<Transfer node |p| to the adjustment list@>=
begin while link(q)<>p do q:=link(q);
    if type(p) = adjust_node then begin
        if adjust_pre(p) <> 0 then
            update_adjust_list(pre_adjust_tail)
        else
            update_adjust_list(adjust_tail);
        p := link(p); free_node(link(q), small_node_size);
    end
else  begin link(adjust_tail):=p; adjust_tail:=p; p:=link(p);
  end;
link(q):=p; p:=q;
end
@z

% \vadjust
@x l.17583
@d align_stack_node_size=5 {number of |mem| words to save alignment states}
@y
@d align_stack_node_size=6 {number of |mem| words to save alignment states}
@z

% \vadjust
@x l.17590
@!cur_head,@!cur_tail:pointer; {adjustment list pointers}
@y
@!cur_head,@!cur_tail:pointer; {adjustment list pointers}
@!cur_pre_head,@!cur_pre_tail:pointer; {pre-adjustment list pointers}
@z

% \vadjust
@x l.17596
cur_head:=null; cur_tail:=null;
@y
cur_head:=null; cur_tail:=null;
cur_pre_head:=null; cur_pre_tail:=null;
@z

% \vadjust
@x l.17607
info(p+4):=cur_head; link(p+4):=cur_tail;
align_ptr:=p;
cur_head:=get_avail;
@y
info(p+4):=cur_head; link(p+4):=cur_tail;
info(p+5):=cur_pre_head; link(p+5):=cur_pre_tail;
align_ptr:=p;
cur_head:=get_avail;
cur_pre_head:=get_avail;
@z

% \vadjust
@x l.17616
begin free_avail(cur_head);
p:=align_ptr;
cur_tail:=link(p+4); cur_head:=info(p+4);
@y
begin free_avail(cur_head);
free_avail(cur_pre_head);
p:=align_ptr;
cur_tail:=link(p+4); cur_head:=info(p+4);
cur_pre_tail:=link(p+5); cur_pre_head:=info(p+5);
@z

% \vadjust
@x
cur_align:=link(preamble); cur_tail:=cur_head; init_span(cur_align);
@y
cur_align:=link(preamble); cur_tail:=cur_head; cur_pre_tail:=cur_pre_head;
init_span(cur_align);
@z

% \vadjust
@x l.17690
  begin adjust_tail:=cur_tail; u:=hpack(link(head),natural); w:=width(u);
  cur_tail:=adjust_tail; adjust_tail:=null;
  end
@y
  begin adjust_tail:=cur_tail; pre_adjust_tail:=cur_pre_tail;
  u:=hpack(link(head),natural); w:=width(u);
  cur_tail:=adjust_tail; adjust_tail:=null;
  cur_pre_tail:=pre_adjust_tail; pre_adjust_tail:=null;
  end
@z

% \vadjust
@x
  pop_nest; append_to_vlist(p);
  if cur_head<>cur_tail then
    begin link(tail):=link(cur_head); tail:=cur_tail;
    end;
  end
@y
  pop_nest;
  if cur_pre_head <> cur_pre_tail then
      append_list(cur_pre_head)(cur_pre_tail);
  append_to_vlist(p);
  if cur_head <> cur_tail then
      append_list(cur_head)(cur_tail);
  end
@z

% \vadjust
@x l.19508
append_to_vlist(just_box);
if adjust_head<>adjust_tail then
  begin link(tail):=link(adjust_head); tail:=adjust_tail;
   end;
@y
if pre_adjust_head <> pre_adjust_tail then
    append_list(pre_adjust_head)(pre_adjust_tail);
pre_adjust_tail := null;
append_to_vlist(just_box);
if adjust_head <> adjust_tail then
    append_list(adjust_head)(adjust_tail);
@z

% \vadjust
@x 19529
adjust_tail:=adjust_head; just_box:=hpack(q,cur_width,exactly);
@y
adjust_tail:=adjust_head;
pre_adjust_tail := pre_adjust_head;
just_box:=hpack(q,cur_width,exactly);
@z

% \primitive
@x
any_mode(ignore_spaces): begin @<Get the next non-blank non-call...@>;
  goto reswitch;
  end;
@y
any_mode(ignore_spaces): begin
  if cur_chr = 0 then begin
    @<Get the next non-blank non-call...@>;
    goto reswitch;
  end
  else begin
    t:=scanner_status;
    scanner_status:=normal;
    get_next;
    scanner_status:=t;
    if cur_cs < hash_base then
      cur_cs := prim_lookup(cur_cs-257)
    else
      cur_cs  := prim_lookup(text(cur_cs));
    if cur_cs<>undefined_primitive then begin
      cur_cmd := prim_eq_type(cur_cs);
      cur_chr := prim_equiv(cur_cs);
      goto reswitch;
      end;
    end;
  end;
@z

% \vadjust
@x l.23545
    begin append_to_vlist(cur_box);
    if adjust_tail<>null then
      begin if adjust_head<>adjust_tail then
        begin link(tail):=link(adjust_head); tail:=adjust_tail;
        end;
@y
    begin
        if pre_adjust_tail <> null then begin
            if pre_adjust_head <> pre_adjust_tail then
                append_list(pre_adjust_head)(pre_adjust_tail);
            pre_adjust_tail := null;
        end;
        append_to_vlist(cur_box);
        if adjust_tail <> null then begin
            if adjust_head <> adjust_tail then
                append_list(adjust_head)(adjust_tail);
@z

% \vadjust
@x l.23716
adjusted_hbox_group: begin adjust_tail:=adjust_head; package(0);
@y
adjusted_hbox_group: begin adjust_tail:=adjust_head;
    pre_adjust_tail:=pre_adjust_head; package(0);
@z

% \vadjust
@x l.23870
    help1("I'm changing to \insert0; box 255 is special.");
    error; cur_val:=0;
    end;
  end;
saved(0):=cur_val; incr(save_ptr);
new_save_level(insert_group); scan_left_brace; normal_paragraph;
push_nest; mode:=-vmode; prev_depth:=ignore_depth;
end;
@y
    help1("I'm changing to \insert0; box 255 is special.");
    error; cur_val:=0;
    end;
  end;
saved(0) := cur_val;
if (cur_cmd = vadjust) and scan_keyword("pre") then
    saved(1) := 1
else
    saved(1) := 0;
save_ptr := save_ptr + 2;
new_save_level(insert_group); scan_left_brace; normal_paragraph;
push_nest; mode:=-vmode; prev_depth:=ignore_depth;
end;
@z

% \vadjust
@x
  d:=split_max_depth; f:=floating_penalty; unsave; decr(save_ptr);
@y
  d:=split_max_depth; f:=floating_penalty; unsave; save_ptr:=save_ptr-2;
@z

% \vadjust
@x l.23892
    subtype(tail):=0; {the |subtype| is not used}
@y
    adjust_pre(tail) := saved(1); {the |subtype| is used for |adjust_pre|}
@z

% \vadjust
@x
@!t:pointer; {tail of adjustment list}
@y
@!t:pointer; {tail of adjustment list}
@!pre_t:pointer; {tail of pre-adjustment list}
@z

% \vadjust
@x l.25339
adjust_tail:=adjust_head; b:=hpack(p,natural); p:=list_ptr(b);
t:=adjust_tail; adjust_tail:=null;@/
@y
adjust_tail:=adjust_head; pre_adjust_tail:=pre_adjust_head;
b:=hpack(p,natural); p:=list_ptr(b);
t:=adjust_tail; adjust_tail:=null;@/
pre_t:=pre_adjust_tail; pre_adjust_tail:=null;@/
@z

% \vadjust
@x l.25448
tail_append(new_penalty(post_display_penalty));
@y
if pre_t<>pre_adjust_head then
  begin link(tail):=link(pre_adjust_head); tail:=pre_t;
  end;
tail_append(new_penalty(post_display_penalty));
@z

% \primitive
@x
@<Dump the hash table@>=
@y
@<Dump the hash table@>=
for p:=0 to prim_size do dump_hh(prim[p]);
for p:=0 to prim_size do dump_wd(prim_eqtb[p]);
@z

% \primitive
@x
@ @<Undump the hash table@>=
@y
@ @<Undump the hash table@>=
for p:=0 to prim_size do undump_hh(prim[p]);
for p:=0 to prim_size do undump_wd(prim_eqtb[p]);
@z

% \strcmp
@x l.28174
@ Each new type of node that appears in our data structure must be capable
@y
@ @d call_func(#) == begin if # <> 0 then do_nothing end
@d flushable(#) == (# = str_ptr - 1)

@p procedure flush_str(s: str_number); {flush a string if possible}
begin
    if flushable(s) then
        flush_string;
end;

procedure pdf_error(t, p: str_number);
begin
    normalize_selector;
    print_err("Error");
    if t <> 0 then begin
        print(" (");
        print(t);
        print(")");
    end;
    print(": "); print(p);
    succumb;
end;

function tokens_to_string(p: pointer): str_number; {return a string from tokens
list}
begin
    if selector = new_string then
        pdf_error("tokens", "tokens_to_string() called while selector = new_string");
    old_setting:=selector; selector:=new_string;
    show_token_list(link(p),null,pool_size-pool_ptr);
    selector:=old_setting;
{
    last_tokens_string := make_string;
    tokens_to_string := last_tokens_string;
}
    tokens_to_string := make_string;
end;

procedure compare_strings; {to implement \.{\\strcmp}}
label done;
var s1, s2: str_number;
    i1, i2, j1, j2: pool_pointer;
begin
    call_func(scan_toks(false, true));
    s1 := tokens_to_string(def_ref);
    delete_token_ref(def_ref);
    call_func(scan_toks(false, true));
    s2 := tokens_to_string(def_ref);
    delete_token_ref(def_ref);
    i1 := str_start_macro(s1);
    j1 := str_start_macro(s1 + 1);
    i2 := str_start_macro(s2);
    j2 := str_start_macro(s2 + 1);
    while (i1 < j1) and (i2 < j2) do begin
        if str_pool[i1] < str_pool[i2] then begin
            cur_val := -1;
            goto done;
        end;
        if str_pool[i1] > str_pool[i2] then begin
            cur_val := 1;
            goto done;
        end;
        incr(i1);
        incr(i2);
    end;
    if (i1 = j1) and (i2 = j2) then
        cur_val := 0
    else if i1 < j1 then
        cur_val := 1
    else
        cur_val := -1;
done:
    flush_str(s2);
    flush_str(s1);
    cur_val_level := int_val;
end;

@ Each new type of node that appears in our data structure must be capable
@z

% \strcmp
% \shellescape
@x l.29232
primitive("pdflastypos",last_item,pdf_last_y_pos_code);
@y
primitive("pdflastypos",last_item,pdf_last_y_pos_code);
primitive("strcmp",convert,pdf_strcmp_code);
primitive("shellescape",last_item,pdf_shell_escape_code);
@z

% \strcmp
@x
eTeX_revision_code: do_nothing;
@y
eTeX_revision_code: do_nothing;
pdf_strcmp_code:
  begin
    save_scanner_status := scanner_status;
    save_warning_index := warning_index;
    save_def_ref := def_ref;
    save_cur_string;
    compare_strings;
    def_ref := save_def_ref;
    warning_index := save_warning_index;
    scanner_status := save_scanner_status;
    restore_cur_string;
  end;
@z

% \strcmp
@x
eTeX_revision_code: print(eTeX_revision);
@y
eTeX_revision_code: print(eTeX_revision);
pdf_strcmp_code: print_int(cur_val);
@z

% \ifincsname
@x l.31105
@d if_font_char_code=19 { `\.{\\iffontchar}' }
@y
@d if_font_char_code=19 { `\.{\\iffontchar}' }
@d if_in_csname_code=20 { `\.{\\ifincsname}' }
@z

% \ifincsname
@x l.31115
@!@:if_font_char_}{\.{\\iffontchar} primitive@>
@y
@!@:if_font_char_}{\.{\\iffontchar} primitive@>
primitive("ifincsname",if_test,if_in_csname_code);
@!@:if_in_csname_}{\.{\\ifincsname} primitive@>
@z

% \ifincsname
@x l.31123
if_font_char_code:print_esc("iffontchar");
@y
if_font_char_code:print_esc("iffontchar");
if_in_csname_code:print_esc("ifincsname");
@z

% \ifincsname
@x
if_cs_code:begin n:=get_avail; p:=n; {head of the list of characters}
@y
if_cs_code:begin n:=get_avail; p:=n; {head of the list of characters}
  e := is_in_csname; is_in_csname := true;
@z

% \ifincsname
@x
  b:=(eq_type(cur_cs)<>undefined_cs);
@y
  b:=(eq_type(cur_cs)<>undefined_cs);
  is_in_csname := e;
@z

% \ifincsname
@x l.31189
if_font_char_code:begin scan_font_ident; n:=cur_val; scan_usv_num;
@y
if_in_csname_code: b := is_in_csname;
if_font_char_code:begin scan_font_ident; n:=cur_val; scan_usv_num;
@z

