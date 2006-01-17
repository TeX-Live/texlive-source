% vim:ft=change
% This changefile fixes some minor bugs in (e-)Omega

@x l.4290 -- max words = 8 (required by local_par_node)
@p function copy_node_list(@!p:pointer):pointer; {makes a duplicate of the
  node list that starts at |p| and returns a pointer to the new list}
var h:pointer; {temporary head of copied list}
@!q:pointer; {previous position in new list}
@!r:pointer; {current node being fabricated for new list}
@!words:0..5; {number of words remaining to be copied}
@y
@p function copy_node_list(@!p:pointer):pointer; {makes a duplicate of the
  node list that starts at |p| and returns a pointer to the new list}
var h:pointer; {temporary head of copied list}
@!q:pointer; {previous position in new list}
@!r:pointer; {current node being fabricated for new list}
@!words:0..8; {number of words remaining to be copied}
@z
% ----------------------------------------
@x remove l.18956 -- 18994 and reference to new_hsize
function get_natural_width(first_ptr:pointer):scaled;
label reswitch, common_ending, exit;
var
@!p:pointer;
@!x:scaled; {height, depth, and natural width}
@!g:pointer; {points to a glue specification}
@!f:internal_font_number; {the font in a |char_node|}
@!i:four_quarters; {font information about a |char_node|}
begin
x:=0;
p:=first_ptr;
while p<>null do begin
  while is_char_node(p) do begin
    f:=font(p); i:=char_info(f)(character(p));
    x:=x+char_width(f)(i);
    p:=link(p);
    end;
  if p<>null then
    begin case type(p) of
    hlist_node,vlist_node,rule_node,unset_node:
      x:=x+width(p);
    ins_node,mark_node,adjust_node: do_nothing;
    whatsit_node: do_nothing;
    glue_node: begin
      g:=glue_ptr(p); x:=x+width(g);
      end;
    kern_node,math_node: x:=x+width(p);
    ligature_node: begin
      f:=font(p); i:=char_info(f)(character(lig_char(p)));
      x:=x+char_width(f)(i);
      end;
    othercases do_nothing
    endcases;
    p:=link(p);
    end;
  end;
get_natural_width:=x;
end;

procedure line_break(@!d:boolean);
label done,done1,done2,done3,done4,done5,continue;
var new_hsize: scaled;
@<Local variables for line breaking@>@;
@y
procedure line_break(@!d:boolean);
label done,done1,done2,done3,done4,done5,continue;
var @<Local variables for line breaking@>@;
@z
% ----------------------------------------
@x remove remaining refs to new_hsize (l.19726 etc)
@ We compute the values of |easy_line| and the other local variables relating
to line length when the |line_break| procedure is initializing itself.

@<Get ready to start...@>=
new_hsize := get_natural_width(link(temp_head)) div 7;
if new_hsize>hsize then new_hsize:=hsize;
if new_hsize<(hsize div 4) then new_hsize:=(hsize div 4);
new_hsize := hsize;
if par_shape_ptr=null then
  if hang_indent=0 then
    begin last_special_line:=0; second_width:=new_hsize;
    second_indent:=0;
    end
  else @<Set line length parameters in preparation for hanging indentation@>
else  begin last_special_line:=info(par_shape_ptr)-1;
  second_width:=mem[par_shape_ptr+2*(last_special_line+1)].sc;
  second_indent:=mem[par_shape_ptr+2*last_special_line+1].sc;
  end;
if looseness=0 then easy_line:=last_special_line
else easy_line:=max_halfword

@ @<Set line length parameters in preparation for hanging indentation@>=
begin last_special_line:=abs(hang_after);
if hang_after<0 then
  begin first_width:=new_hsize-abs(hang_indent);
  if hang_indent>=0 then first_indent:=hang_indent
  else first_indent:=0;
  second_width:=new_hsize; second_indent:=0;
  end
else  begin first_width:=new_hsize; first_indent:=0;
  second_width:=hsize-abs(hang_indent);
  if hang_indent>=0 then second_indent:=hang_indent
  else second_indent:=0;
  end;
end
@y
@ We compute the values of |easy_line| and the other local variables relating
to line length when the |line_break| procedure is initializing itself.

@<Get ready to start...@>=
if par_shape_ptr=null then
  if hang_indent=0 then
    begin last_special_line:=0; second_width:=hsize;
    second_indent:=0;
    end
  else @<Set line length parameters in preparation for hanging indentation@>
else  begin last_special_line:=info(par_shape_ptr)-1;
  second_width:=mem[par_shape_ptr+2*(last_special_line+1)].sc;
  second_indent:=mem[par_shape_ptr+2*last_special_line+1].sc;
  end;
if looseness=0 then easy_line:=last_special_line
else easy_line:=max_halfword

@ @<Set line length parameters in preparation for hanging indentation@>=
begin last_special_line:=abs(hang_after);
if hang_after<0 then
  begin first_width:=hsize-abs(hang_indent);
  if hang_indent>=0 then first_indent:=hang_indent
  else first_indent:=0;
  second_width:=hsize; second_indent:=0;
  end
else  begin first_width:=hsize; first_indent:=0;
  second_width:=hsize-abs(hang_indent);
  if hang_indent>=0 then second_indent:=hang_indent
  else second_indent:=0;
  end;
end
@z
% ----------------------------------------
@x l.28441 -- Correct wrong node size
dir_node: begin r:=get_node(dir_node_size);
  words:=small_node_size;
  end;
@y
dir_node: begin r:=get_node(dir_node_size);
  words:=dir_node_size;
  end;
@z
% ----------------------------------------

