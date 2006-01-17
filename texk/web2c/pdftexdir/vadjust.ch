% WEB change file containing vadjust extension for pdfTeX 
%
% Copyright (c) 1996-2004 Han Th\^e\llap{\raise 0.5ex\hbox{\'{}}} Th\`anh, <thanh@pdftex.org>
%
% This file is part of pdfTeX.
%
% pdfTeX is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2 of the License, or
% (at your option) any later version.
%
% pdfTeX is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with pdfTeX; if not, write to the Free Software
% Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
%
% $Id: vadjust.ch,v 1.2 2004/08/01 12:12:23 olaf Exp $

@x [142] - pre-vadjust
@d adjust_node=5 {|type| of an adjust node}
@y
@d adjust_node=5 {|type| of an adjust node}
@d adjust_pre == subtype  {pre-adjustment?}
@#{|append_list| is used to append a list to |tail|}
@d append_list(#) == begin link(tail) := link(#); append_list_end
@d append_list_end(#) == tail := #; end

@z

@x [162] - pre-vadjust
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

@x [197] - pre-vadjust
begin print_esc("vadjust"); node_list_display(adjust_ptr(p)); {recursive call}
@y
begin print_esc("vadjust"); if adjust_pre(p) <> 0 then print(" pre ");
node_list_display(adjust_ptr(p)); {recursive call}
@z

@x [649] - pre-vadjust
if adjust_tail<>null then link(adjust_tail):=null;
@y
if adjust_tail<>null then link(adjust_tail):=null;
if pre_adjust_tail<>null then link(pre_adjust_tail):=null;
@z

@x [655] - pre-vadjust
@<Transfer node |p| to the adjustment list@>=
begin while link(q)<>p do q:=link(q);
if type(p)=adjust_node then
  begin link(adjust_tail):=adjust_ptr(p);
  while link(adjust_tail)<>null do adjust_tail:=link(adjust_tail);
  p:=link(p); free_node(link(q),small_node_size);
  end
@y
@<Glob...@>=
@!pre_adjust_tail: pointer;

@ @<Set init...@>=
pre_adjust_tail := null;

@ Materials in \.{\\vadjust} used with \.{pre} keyword will be appended to
|pre_adjust_tail| instead of |adjust_tail|.

@d update_adjust_list(#) == begin
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
@z

@x [770] - pre-vadjust
@d align_stack_node_size=5 {number of |mem| words to save alignment states}
@y
@d align_stack_node_size=6 {number of |mem| words to save alignment states}
@z

@x [770] - pre-vadjust
@!cur_head,@!cur_tail:pointer; {adjustment list pointers}
@y
@!cur_head,@!cur_tail:pointer; {adjustment list pointers}
@!cur_pre_head,@!cur_pre_tail:pointer; {pre-adjustment list pointers}
@z

@x [771] - pre-vadjust
cur_head:=null; cur_tail:=null;
@y
cur_head:=null; cur_tail:=null;
cur_pre_head:=null; cur_pre_tail:=null;
@z

@x [772] - pre-vadjust
info(p+4):=cur_head; link(p+4):=cur_tail;
@y
info(p+4):=cur_head; link(p+4):=cur_tail;
info(p+5):=cur_pre_head; link(p+5):=cur_pre_tail;
@z

@x [771] - pre-vadjust
cur_tail:=link(p+4); cur_head:=info(p+4);
@y
cur_tail:=link(p+4); cur_head:=info(p+4);
cur_pre_tail:=link(p+5); cur_pre_head:=info(p+5);
@z

@x [786] - pre-vadjust
cur_align:=link(preamble); cur_tail:=cur_head; init_span(cur_align);
@y
cur_align:=link(preamble); cur_tail:=cur_head; cur_pre_tail:=cur_pre_head;
init_span(cur_align);
@z

@x [791] - pre-vadjust
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

@x [799] - pre-vadjust
  pop_nest; append_to_vlist(p);
  if cur_head<>cur_tail then
    begin link(tail):=link(cur_head); tail:=cur_tail;
    end;
@y
  pop_nest;
  if cur_pre_head <> cur_pre_tail then
      append_list(cur_pre_head)(cur_pre_tail);
  append_to_vlist(p);
  if cur_head <> cur_tail then
      append_list(cur_head)(cur_tail);
@z

@x [888] - pre-vadjust
append_to_vlist(just_box);
if adjust_head<>adjust_tail then
  begin link(tail):=link(adjust_head); tail:=adjust_tail;
   end;
adjust_tail:=null
@y
if pre_adjust_head <> pre_adjust_tail then
    append_list(pre_adjust_head)(pre_adjust_tail);
pre_adjust_tail := null;
append_to_vlist(just_box);
if adjust_head <> adjust_tail then
    append_list(adjust_head)(adjust_tail);
adjust_tail := null
@z

@x
adjust_tail:=adjust_head;
@y
adjust_tail:=adjust_head;
pre_adjust_tail := pre_adjust_head;
@z

@x [1076] - pre-vadjust
    begin append_to_vlist(cur_box);
    if adjust_tail<>null then
      begin if adjust_head<>adjust_tail then
        begin link(tail):=link(adjust_head); tail:=adjust_tail;
        end;
      adjust_tail:=null;
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
            adjust_tail := null;
        end;
@z

@x [1085] - pre-vadjust
adjusted_hbox_group: begin adjust_tail:=adjust_head; package(0);
@y
adjusted_hbox_group: begin adjust_tail:=adjust_head; 
    pre_adjust_tail:=pre_adjust_head; package(0);
@z

@x [1099] - pre-vadjust
saved(0):=cur_val; incr(save_ptr);
new_save_level(insert_group); scan_left_brace; normal_paragraph;
push_nest; mode:=-vmode; prev_depth:=ignore_depth;
end;
@y
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

@x [1100] - pre-vadjust
  d:=split_max_depth; f:=floating_penalty; unsave; decr(save_ptr);
@y
  d:=split_max_depth; f:=floating_penalty; unsave; save_ptr := save_ptr - 2;
@z

@x [1100] - pre-vadjust
    subtype(tail):=0; {the |subtype| is not used}
@y
    adjust_pre(tail) := saved(1); {the |subtype| is used for |adjust_pre|}
@z

@x [1198] - pre-vadjust
@!t:pointer; {tail of adjustment list}
@y
@!t:pointer; {tail of adjustment list}
@!pre_t:pointer; {tail of pre-adjustment list}
@z

@x [1199] - pre-vadjust
adjust_tail:=adjust_head; b:=hpack(p,natural); p:=list_ptr(b);
t:=adjust_tail; adjust_tail:=null;@/
@y
adjust_tail:=adjust_head; pre_adjust_tail:=pre_adjust_head;
b:=hpack(p,natural); p:=list_ptr(b);
t:=adjust_tail; adjust_tail:=null;@/
pre_t:=pre_adjust_tail; pre_adjust_tail:=null;@/
@z

@x [1205] - pre-vadjust
if t<>adjust_head then {migrating material comes after equation number}
  begin link(tail):=link(adjust_head); tail:=t;
  end;
@y
if t<>adjust_head then {migrating material comes after equation number}
  begin link(tail):=link(adjust_head); tail:=t;
  end;
if pre_t<>pre_adjust_head then
  begin link(tail):=link(pre_adjust_head); tail:=pre_t;
  end;
@z
