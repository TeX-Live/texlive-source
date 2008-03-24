Copyright (c) 2008 jerome DOT laurens AT u-bourgogne DOT fr

This file is part of the SyncTeX package.

License:
--------
Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE

Except as contained in this notice, the name of the copyright holder  
shall not be used in advertising or otherwise to promote the sale,  
use or other dealings in this Software without prior written  
authorization from the copyright holder.

Notice:
-------
This file contains TeX changes for core synctex support.
It is missing some implementation details that are deferred to either
sync-pdftex.ch or sync-xetex.ch
synctex.ch is based on tex.web after the eTeX changes have been applied.
So it might not apply as is to pdftex.web due to some typographical changes.
In the building process, one may have to apply some suitable tex.ch0 first.
See pdftexdir for details.

@x
found: link(r):=null; {this node is now nonempty}
@!stat var_used:=var_used+s; {maintain usage statistics}
tats@;@/
get_node:=r;
exit:end;
@y
found: link(r):=null; {this node is now nonempty}
@!stat var_used:=var_used+s; {maintain usage statistics}
tats@;@/
@<Initialize bigger nodes with {\sl synctex} information@>;
get_node:=r;
exit:end;
@z

The |synctex_field_size| depends on the TeX implementation.
pdfTeX uses 2 additional words whereas XeTeX will only use one,
because it naturally has bigger words of memory.
@x
@d hlist_node=0 {|type| of hlist nodes}
@d box_node_size=7 {number of words to allocate for a box node}
@y
@d synctex_engine_pdftex=0
@d synctex_engine_xetex=1
@d synctex_field_size=MISSING IMPLEMENTATION

@d hlist_node=0 {|type| of hlist nodes}
@d box_node_size=7+synctex_field_size {number of words to allocate for a box node}
@z

@x
@d small_node_size=2 {number of words to allocate for most node types}
@y
@d small_node_size=2 {number of words to allocate for most node types}
@d medium_node_size=small_node_size+synctex_field_size {number of words to
           allocate for synchronized node types like math, kern and glue nodes}
@z

@x
@p function new_math(@!w:scaled;@!s:small_number):pointer;
var p:pointer; {the new node}
begin p:=get_node(small_node_size); type(p):=math_node;
subtype(p):=s; width(p):=w; new_math:=p;
end;
@y
@p function new_math(@!w:scaled;@!s:small_number):pointer;
var p:pointer; {the new node}
begin p:=get_node(medium_node_size); type(p):=math_node;
                             {{\sl synctex} node size watch point: proper size}
subtype(p):=s; width(p):=w; new_math:=p;
end;
@z

@x
@p function new_param_glue(@!n:small_number):pointer;
var p:pointer; {the new node}
@!q:pointer; {the glue specification}
begin p:=get_node(small_node_size); type(p):=glue_node; subtype(p):=n+1;
@y
@p function new_param_glue(@!n:small_number):pointer;
var p:pointer; {the new node}
@!q:pointer; {the glue specification}
begin p:=get_node(medium_node_size); type(p):=glue_node; subtype(p):=n+1;
                                       {{\sl synctex} watch point: proper size}
@z


@x
@p function new_glue(@!q:pointer):pointer;
var p:pointer; {the new node}
begin p:=get_node(small_node_size); type(p):=glue_node; subtype(p):=normal;
@y
@p function new_glue(@!q:pointer):pointer;
var p:pointer; {the new node}
begin p:=get_node(medium_node_size); type(p):=glue_node; subtype(p):=normal;
                                            {{\sl synctex} watch point: proper size}
@z


@x
@p function new_kern(@!w:scaled):pointer;
var p:pointer; {the new node}
begin p:=get_node(small_node_size); type(p):=kern_node;
@y
@p function new_kern(@!w:scaled):pointer;
var p:pointer; {the new node}
begin p:=get_node(medium_node_size); type(p):=kern_node;
                              {{\sl synctex} whatch point: proper |kern_node| size}
@z


@x
    glue_node: begin fast_delete_glue_ref(glue_ptr(p));
      if leader_ptr(p)<>null then flush_node_list(leader_ptr(p));
      end;
    kern_node,math_node,penalty_node: do_nothing;
@y
    glue_node: begin fast_delete_glue_ref(glue_ptr(p));
	    if leader_ptr(p)<>null then flush_node_list(leader_ptr(p));
		free_node(p, medium_node_size); {{\sl synctex} watch point: proper size}
        goto done;
      end;
    kern_node,math_node:begin
        free_node(p, medium_node_size); {{\sl synctex} watch point: proper size}
        goto done;
      end;
    penalty_node: do_nothing;
@z


@x
@ @<Case statement to copy...@>=
case type(p) of
hlist_node,vlist_node,unset_node: begin r:=get_node(box_node_size);
@y
@ @<Case statement to copy...@>=
case type(p) of
hlist_node,vlist_node,unset_node: begin r:=get_node(box_node_size);
  @<Copy the box {\sl synctex} information@>;
@z


@x
glue_node: begin r:=get_node(small_node_size); add_glue_ref(glue_ptr(p));
  glue_ptr(r):=glue_ptr(p); leader_ptr(r):=copy_node_list(leader_ptr(p));
  end;
kern_node,math_node,penalty_node: begin r:=get_node(small_node_size);
  words:=small_node_size;
  end;
@y
glue_node: begin r:=get_node(medium_node_size); add_glue_ref(glue_ptr(p));
                                 {{\sl synctex} node size watch point: proper size}
  @<Copy the glue {\sl synctex} information@>;
  glue_ptr(r):=glue_ptr(p); leader_ptr(r):=copy_node_list(leader_ptr(p));
  end;
kern_node,math_node:
begin
  words:=medium_node_size;
                {{\sl synctex}: proper size, do not copy the {\sl synctex} information}
  r:=get_node(words);
end;
penalty_node: begin r:=get_node(small_node_size);
  words:=small_node_size;
  end;
@z

next change is etex based
@x
@d etex_int_pars=eTeX_state_code+eTeX_states {total number of \eTeX's integer parameters}
@#
@d int_pars=etex_int_pars {total number of integer parameters}
@y
@d etex_int_pars=eTeX_state_code+eTeX_states {total number of \eTeX's integer parameters}
@#
@d synctex_code=etex_int_pars {{\sl synctex}: for the \.{\\synctex} macro}
@d synctex_int_pars=synctex_code+1 {total number of \TeX's integer parameters}
@#
@d int_pars=synctex_int_pars {total number of integer parameters}
@z

next change is etex based
@x
@d tracing_assigns==int_par(tracing_assigns_code)
@y
@d synctex == int_par(synctex_code)
@#
@d tracing_assigns==int_par(tracing_assigns_code)
@z

next change is etex based
@x
@/@<Cases for |print_param|@>@/
othercases print("[unknown integer parameter!]")
endcases;
@y
@/@<synctex case for |print_param|@>@/
@/@<Cases for |print_param|@>@/
othercases print("[unknown integer parameter!]")
endcases;
@z

@x
@!in_state_record = record
  @!state_field, @!index_field: quarterword;
  @!start_field,@!loc_field, @!limit_field, @!name_field: halfword;
  end;
@y
@!in_state_record = record
  @!state_field, @!index_field: quarterword;
  @!start_field,@!loc_field, @!limit_field, @!name_field: halfword;
  @!synctex_tag_field: integer; {stack the tag of the current file}
  end;
@z

@x
@d name==cur_input.name_field {name of the current file}
@y
@d name==cur_input.name_field {name of the current file}
@d synctex_tag==cur_input.synctex_tag_field {{\sl synctex} tag of the current file}
@z


@x
name:=0; {|terminal_input| is now |true|}
end;
@y
name:=0; {|terminal_input| is now |true|}
@<Prepare terminal input {\sl synctex} information@>;
end;
@z


@x
@<Read the first line of the new file@>;
end;
@y
@<Prepare new file {\sl synctex} information@>;
@<Read the first line of the new file@>;
end;
@z

next change is etex based
@x
@<Initialize |hlist_out| for mixed direction typesetting@>;
left_edge:=cur_h;
while p<>null do @<Output node |p| for |hlist_out| and move to the next node,
  maintaining the condition |cur_v=base_line|@>;
@<Finish |hlist_out| for mixed direction typesetting@>;
@y
@<Initialize |hlist_out| for mixed direction typesetting@>;
left_edge:=cur_h;
@<Start hlist {\sl synctex} information record@>;
while p<>null do @<Output node |p| for |hlist_out| and move to the next node,
  maintaining the condition |cur_v=base_line|@>;
@<Finish hlist {\sl synctex} information record@>;
@<Finish |hlist_out| for mixed direction typesetting@>;
@z

@x
whatsit_node: @<Output the whatsit node |p| in an hlist@>;
glue_node: @<Move right or output leaders@>;
@y
whatsit_node: @<Output the whatsit node |p| in an hlist@>;
glue_node: begin
  @<Record |glue_node| {\sl synctex} information@>
  @<Move right or output leaders@>;
end;
@z

next change is etex based
@x
kern_node:cur_h:=cur_h+width(p);
math_node: @<Handle a math node in |hlist_out|@>;
@y
kern_node: begin
  @<Record |kern_node| {\sl synctex} information@>;
  cur_h:=cur_h+width(p);
end;
math_node: begin
  @<Record |math_node| {\sl synctex} information@>;
  @<Handle a math node in |hlist_out|@>;
end;
@z

@x
@!old_setting:0..max_selector; {saved |selector| setting}
begin if tracing_output>0 then
  begin print_nl(""); print_ln;
  print("Completed box being shipped out");
@.Completed box...@>
@y
@!old_setting:0..max_selector; {saved |selector| setting}
begin
@<Record sheet {\sl synctex} information@>
if tracing_output>0 then
  begin print_nl(""); print_ln;
  print("Completed box being shipped out");
@.Completed box...@>
@z

@x
  if r<>null then if link(r)=null then if not is_char_node(r) then
   if type(r)=kern_node then {unneeded italic correction}
    begin free_node(r,small_node_size); link(q):=null;
    end;
  end
@y
  if r<>null then if link(r)=null then if not is_char_node(r) then
   if type(r)=kern_node then {unneeded italic correction}
    begin free_node(r,medium_node_size); link(q):=null; {{\sl synctex}: r is a |kern_node|}
    end;
  end
@z


@x
  p:=lig_stack; lig_stack:=link(p); free_node(p,small_node_size);
@y
  p:=lig_stack; lig_stack:=link(p);
  free_node(p,small_node_size); {{\sl synctex} node size watch point: proper size!}
@z

@x
if w<>0 then
  begin link(t):=new_kern(w); t:=link(t); w:=0;
  end;
@y
if w<>0 then
  begin link(t):=new_kern(w); t:=link(t); w:=0;
    MISSING IMPLEMENTATION: 0 synctag for medium sized node at mem[t]
  end;
@z

@x
@<Initialize the special list...@>=
type(page_head):=glue_node; subtype(page_head):=normal;
@y
@<Initialize the special list...@>=
type(page_head):=glue_node; subtype(page_head):=normal;
                 {{\sl synctex} watch point: box(|page_head|) size >= |glue_node| size}
@z

@x
@<Move the cursor past a pseudo-ligature...@>=
main_p:=lig_ptr(lig_stack);
if main_p>null then tail_append(main_p); {append a single character}
temp_ptr:=lig_stack; lig_stack:=link(temp_ptr);
free_node(temp_ptr,small_node_size);
@y
@<Move the cursor past a pseudo-ligature...@>=
main_p:=lig_ptr(lig_stack);
if main_p>null then tail_append(main_p); {append a single character}
temp_ptr:=lig_stack; lig_stack:=link(temp_ptr);
free_node(temp_ptr,small_node_size); {{\sl synctex} node size watch point: proper size!}
@z

next change is etex based
@x
  if subtype(p)<a_leaders then
    begin type(p):=kern_node; width(p):=rule_wd;
    end
@y
  if subtype(p)<a_leaders then
    begin type(p):=kern_node; width(p):=rule_wd;
             {{\sl synctex} node size watch point: |glue_node| size == |kern_node| size}
    end
@z

next change is etex based
@x
 if (type(tail)=math_node)and(subtype(tail)=begin_M_code) then
  begin free_node(LR_temp,small_node_size); p:=head;
  while link(p)<>tail do p:=link(p);
  free_node(tail,small_node_size); link(p):=null; tail:=p; goto done;
  end;
@y
 if (type(tail)=math_node)and(subtype(tail)=begin_M_code) then
  begin free_node(LR_temp,small_node_size); p:=head;
  while link(p)<>tail do p:=link(p);
  free_node(tail,medium_node_size); link(p):=null; tail:=p; goto done;
					{{\sl synctex} node size watch point: proper size for |math_node|}
  end;
@z

next change is etex based
@x
@ @<Adjust \(t)the LR stack for the |hp...@>=
if end_LR(p) then
  if info(LR_ptr)=end_LR_type(p) then pop_LR
  else  begin incr(LR_problems); type(p):=kern_node; subtype(p):=explicit;
    end
else push_LR(p)
@y
@ @<Adjust \(t)the LR stack for the |hp...@>=
if end_LR(p) then
  if info(LR_ptr)=end_LR_type(p) then pop_LR
  else  begin incr(LR_problems); type(p):=kern_node; subtype(p):=explicit;
           {{\sl synctex} node size watch point: |math_node| size == |kern_node| size}
    end
else push_LR(p)
@z 

next change is etex based
@x
  if LR_dir(p)<>cur_dir then
    @<Reverse an hlist segment and |goto reswitch|@>;
  end;
type(p):=kern_node;
end
@y
  if LR_dir(p)<>cur_dir then
    @<Reverse an hlist segment and |goto reswitch|@>;
  end;
type(p):=kern_node;
             {{\sl synctex} node size watch point: |math_node| size == |kern_node| size}
end
@z

next change is etex based
@x
@<Reverse the complete hlist...@>=
begin save_h:=cur_h; temp_ptr:=p; p:=new_kern(0); link(prev_p):=p;
@y
@<Reverse the complete hlist...@>=
begin save_h:=cur_h; temp_ptr:=p; p:=new_kern(0);
MISSING IMPLEMENTATION: 0 synctag for medium sized node at mem[p]
link(prev_p):=p;
@z

next change is etex based
@x
@<Reverse an hlist segment...@>=
begin save_h:=cur_h; temp_ptr:=link(p); rule_wd:=width(p);
free_node(p,small_node_size);
@y
@<Reverse an hlist segment...@>=
begin save_h:=cur_h; temp_ptr:=link(p); rule_wd:=width(p);
free_node(p,medium_node_size); {{\sl synctex}: p is a |math_node|}
@z

next change is etex based
@x
if type(p)=kern_node then if (rule_wd=0)or(l=null) then
  begin free_node(p,small_node_size); p:=l;
  end;
@y
if type(p)=kern_node then if (rule_wd=0)or(l=null) then
  begin free_node(p,medium_node_size); p:=l;
						    {{\sl synctex} node size watch: proper size for |kern_node|}
  end;
@z

next change is etex based
@x
math_node: begin rule_wd:=width(p);
if end_LR(p) then
  if info(LR_ptr)<>end_LR_type(p) then
    begin type(p):=kern_node; incr(LR_problems);
    end
  else  begin pop_LR;
    if n>min_halfword then
      begin decr(n); decr(subtype(p)); {change |after| into |before|}
      end
    else  begin type(p):=kern_node;
      if m>min_halfword then decr(m)
      else @<Finish the reversed hlist segment and |goto done|@>;
      end;
@y
math_node: begin rule_wd:=width(p);
if end_LR(p) then
  if info(LR_ptr)<>end_LR_type(p) then
    begin type(p):=kern_node; incr(LR_problems);
	        {{\sl synctex} node size watch point: |math_node| size == |kern_node| size}
    end
  else  begin pop_LR;
    if n>min_halfword then
      begin decr(n); decr(subtype(p)); {change |after| into |before|}
      end
    else  begin type(p):=kern_node;
            {{\sl synctex} node size watch point: |math_node| size == |kern_node| size}
      if m>min_halfword then decr(m)
      else @<Finish the reversed hlist segment and |goto done|@>;
      end;
@z

next change is etex based
@x
  if (n>min_halfword)or(LR_dir(p)<>cur_dir) then
    begin incr(n); incr(subtype(p)); {change |before| into |after|}
    end
  else  begin type(p):=kern_node; incr(m);
    end;
  end;
end;
@y
  if (n>min_halfword)or(LR_dir(p)<>cur_dir) then
    begin incr(n); incr(subtype(p)); {change |before| into |after|}
    end
  else  begin type(p):=kern_node; incr(m);
           {{\sl synctex} node size watch point: |math_node| size == |kern_node| size}
    end;
  end;
end;
@z


next change is etex based
@x
@<Finish the reversed...@>=
begin free_node(p,small_node_size);
link(t):=q; width(t):=rule_wd; edge_dist(t):=-cur_h-rule_wd; goto done;
end
@y
@<Finish the reversed...@>=
begin free_node(p,medium_node_size); {{\sl synctex}: p is a |kern_node|}
link(t):=q; width(t):=rule_wd; edge_dist(t):=-cur_h-rule_wd; goto done;
end
@z

next change is etex based
@x
  if is_char_node(p) then r:=get_avail
  else case type(p) of
  hlist_node,vlist_node: begin r:=get_node(box_node_size);
    mem[r+6]:=mem[p+6]; mem[r+5]:=mem[p+5]; {copy the last two words}
    words:=5; list_ptr(r):=null; {this affects |mem[r+5]|}
    end;
@y
  if is_char_node(p) then r:=get_avail
  else case type(p) of
  hlist_node,vlist_node: begin r:=get_node(box_node_size);
	@<Copy the box {\sl synctex} information@>;
    mem[r+6]:=mem[p+6]; mem[r+5]:=mem[p+5]; {copy the last two words}
    words:=5; list_ptr(r):=null; {this affects |mem[r+5]|}
    end;
@z

next change is etex based
@x
  kern_node,math_node: begin r:=get_node(small_node_size);
    words:=small_node_size;
    end;
  glue_node: begin r:=get_node(small_node_size); add_glue_ref(glue_ptr(p));
    glue_ptr(r):=glue_ptr(p); leader_ptr(r):=null;
    end;
@y
  kern_node,math_node: begin
      words:=medium_node_size; {{\sl synctex}: proper size for math and kern}
	  r:=get_node(words);
    end;
  glue_node: begin r:=get_node(medium_node_size); add_glue_ref(glue_ptr(p));
                                                 {{\sl synctex}: proper size for glue}
    @<Copy the glue {\sl synctex} information@>;
    glue_ptr(r):=glue_ptr(p); leader_ptr(r):=null;
    end;
@z

next change is etex based
@x
found:width(t):=width(p); link(t):=q; free_node(p,small_node_size);
done:link(temp_head):=l;
end;

@ @<Adjust \(t)the LR stack for the |j...@>=
if end_LR(p) then
  if info(LR_ptr)<>end_LR_type(p) then
    begin type(p):=kern_node; incr(LR_problems);
    end
  else  begin pop_LR;
    if n>min_halfword then
      begin decr(n); decr(subtype(p)); {change |after| into |before|}
      end
    else  begin if m>min_halfword then decr(m)@+else goto found;
      type(p):=kern_node;
      end;
    end
else  begin push_LR(p);
  if (n>min_halfword)or(LR_dir(p)<>cur_dir) then
    begin incr(n); incr(subtype(p)); {change |before| into |after|}
    end
  else  begin type(p):=kern_node; incr(m);
    end;
  end
@y
found:width(t):=width(p); link(t):=q; free_node(p,small_node_size);
                                               {{\sl synctex}: Unused label, see below}
done:link(temp_head):=l;
end;

@ @<Adjust \(t)the LR stack for the |j...@>=
if end_LR(p) then
  if info(LR_ptr)<>end_LR_type(p) then
    begin type(p):=kern_node; incr(LR_problems);
	        {{\sl synctex} node size watch point: |math_node| size == |kern_node| size}
    end
  else  begin pop_LR;
    if n>min_halfword then
      begin decr(n); decr(subtype(p)); {change |after| into |before|}
      end
    else  begin if m>min_halfword then decr(m)@+else begin
	    width(t):=width(p); link(t):=q; free_node(p,medium_node_size);
							{{\sl synctex}: no more "goto found", and proper node size}
	    goto done;
	  end;
	  type(p):=kern_node;
            {{\sl synctex} node size watch point: |math_node| size == |kern_node| size}
      end;
    end
else  begin push_LR(p);
  if (n>min_halfword)or(LR_dir(p)<>cur_dir) then
    begin incr(n); incr(subtype(p)); {change |before| into |after|}
    end
  else  begin type(p):=kern_node; incr(m);
            {{\sl synctex} node size watch point: |math_node| size == |kern_node| size}
    end;
  end
@z 

next change is etex based
@x
if tracing_scan_tokens>0 then
  begin if term_offset>max_print_line-3 then print_ln
  else if (term_offset>0)or(file_offset>0) then print_char(" ");
  name:=19; print("( "); incr(open_parens); update_terminal;
  end
else name:=18
@y
if tracing_scan_tokens>0 then
  begin if term_offset>max_print_line-3 then print_ln
    else if (term_offset>0)or(file_offset>0) then print_char(" ");
	name:=19; print("( "); incr(open_parens); update_terminal;
  end
else begin
    name:=18;
	@<Prepare pseudo file {\sl synctex} information@>;
end
@z

@x
@* \[54] System-dependent changes.
This section should be replaced, if necessary, by any special
modifications of the program
@y
@* \[53b] The {\sl synchronize texnology}.
This section is devoted to the {\sl synchronize texnology}
- or simply {\sl synctex} - used to synchronize between input and output.
This section explains how synchronization basics are implemented.
Before we enter into more technical details,
let us recall in a few words what is synchronization.

\TeX\ typesetting system clearly separates the input and the output material,
and synchronization will provide a new link between both that can help
text editors and viewers to work together.
More precisely, forwards synchronization is the ability,
given a location in the input source file,
to find what is the corresponding place in the output.
Backwards synchronization just performs the opposite:
given a location in the output,
retrieve the corresponding material in the input source file.

For better code management and maintainance, we adopt a naming convention.
Throughout this program, code related to the {\sl synchronize texnology} is tagged
with the {\sl synctex} key word. Any code extract where {\sl synctex} plays
its part, either explicitly or implicitly, (should) contain the string {\sl synctex}.
This naming convention also holds for external files.
Moreover, all the code related to {\sl synctex} is gathered in this section,
except the definitions.

Synchronization is achieved with the help of an auxiliary file named
`\.{{\sl jobname}.synctex}' ({\sl jobname} is the contents of the
\.{\\jobname} macro), where a {\sl synctex} controller implemented
in the external |synctex.c| file will store geometrical information.
This {\sl synctex} controller will take care of every technical details
concerning the {\sl synctex} file, we will only focus on the messages
the controller will receive from the \TeX\ program.

The most accurate synchronization information should allow to map
any character of the input source file to the corresponding location
in the output, if relevant.
Ideally, the synchronization information of the input material consists of
the file name, the line and column numbers of every character.
The synchronization information in the output is simply the page number and
either point coordinates, or box dimensions and position.
The problem is that the mapping between these informations is only known at
ship out time, which means that we must keep track of the input
synchronization information until the pages ship out.

As \TeX\ only knows about file names and line numbers,
but forgets the column numbers, we only consider a
restricted input synchronization information called {\sl synctex information}.
It consists of a unique file name identifier, the {\sl synctex file tag},
and the line number.
 
Keeping track of such information,
should be different whether characters or nodes are involved.
Actually, only certain nodes are involved in {\sl synctex},
we call them {\sl synchronized nodes}.
Synchronized nodes store the {\sl synctex} information in their last two words:
the first one contains a {\sl synctex file tag} uniquely identifying
the input file, and the second one contains the current line number,
as returned by the \.{\\inputlineno} primitive.
The |synctex_field_size| macro contains the necessary size to store
the {\sl synctex} information in a node.

When declaring the size of a new node, it is recommanded to use the following
convention: if the node is synchronized, use a definition similar to
|my_synchronized_node_size|={\sl xxx}+|synctex_field_size|.
Moreover, one should expect that the {\sl synctex} information is always stored
in the last two words of a synchronized node.

@ By default, every node with a sufficiently big size is initialized
at creation time in the |get_node| routine with the current
{\sl synctex} information, whether or not the node is synchronized.
One purpose is to set this information very early in order to minimize code
dependencies, including forthcoming extensions.
Another purpose is to avoid the assumption that every node type has a dedicated getter,
where initialization should take place. Actually, it appears that some nodes are created
using directly the |get_node| routine and not the dedicated constructor.
And finally, initializing the node at only one place is less error prone.

@ @<Initialize bigger nodes with {\sl synctex} information@>=
if s>=medium_node_size then
begin
  MISSING IMPLEMENTATION: update the synctex information for node at mem[r] with size s
end;

@ Instead of storing the input file name, it is better to store just an identifier.
Each time \TeX\ opens a new file, it notifies the {\sl synctex} controller with
a |synctex_start_input| message.
This controller will create a new {\sl synctex} file tag and
will update the current input state record accordingly.
If the input comes from the terminal or a pseudo file, the |synctex_tag| is set to 0.
It results in automatically disabling synchronization for material
input from the terminal or pseudo files.

@ @<Prepare new file {\sl synctex} information@>=
synctex_start_input; {Give control to the {\sl synctex} controller}

@ @<Prepare terminal input {\sl synctex} information@>=
synctex_tag:=0;

@ @<Prepare pseudo file {\sl synctex} information@>=
synctex_tag:=0;

@ Synchronized nodes are boxes, math, kern and glue nodes.
Other nodes should be synchronized too, in particular math noads.
\TeX\ assumes that math, kern and glue nodes have the same size,
this is why both are synchronized.
{\sl In fine}, only horizontal lists are really used in {\sl synctex},
but all box nodes are considered the same with respect to synchronization,
because a box node type is allowed to change at execution time.

The next sections are the various messages sent to the {\sl synctex} controller.
The argument is either the box or the node currently shipped out.

@ @<Start hlist {\sl synctex} information record@>=
synctex_hlist(this_box);

@ @<Finish hlist {\sl synctex} information record@>=
synctex_tsilh(this_box);

@ @<Record |glue_node| {\sl synctex} information@>=
synctex_glue(p);

@ @<Record |kern_node| {\sl synctex} information@>=
synctex_kern(p);

@ @<Record |math_node| {\sl synctex} information@>=
synctex_math(p);

@ @<Record sheet {\sl synctex} information@>=
synctex_sheet(mag);

@ When making a copy of a synchronized node, we might also have to duplicate
the {\sl synctex} information by copying the two last words.
This is the case for a |box_node| and for a |glue_node|,
but not for a |math_node| nor a |kern_node|. These last two nodes always keep
the {\sl synctex} information they received at creation time.
 
@ @<Copy the box {\sl synctex} information@>=
MISSING IMPLEMENTATION: copy the synctex information for box node at mem[r] from node at mem[p]

@ @<Copy the glue {\sl synctex} information@>=
MISSING IMPLEMENTATION: copy the synctex information for glue node at mem[r] from node at mem[p]

@ Enabling synchronization should be performed from the command line,
|synctexoption| is used for that purpose.
This global integer variable is declared here but it is not used here.
This is just a placeholder where the command line controller will put
the {\sl {\sl synctex}} related options, and the {\sl synctex} controller will read them.

@ @<Glob...@>=
@!synctexoption:integer;

@ A convenient primitive is provided:
\.{\\synctex=1} in the input source file enables synchronization whereas
\.{\\synctex=0} disables it.
Its memory address is |synctex_code|.

@ @<Put each...@>=
primitive("synctex",assign_int,int_base+synctex_code);@/
@!@:synctex_}{\.{\\synctex} primitive@>

@ @<synctex case for |print_param|@>=
synctex_code:    print_esc("synctex");

@ In order to give the {\sl synctex} controller read and write access to
the contents of the \.{\\synctex} primitive, we declare |synctexoffset|,
such that |mem[synctexoffset]| and \.{\\synctex} correpond to
the same memory storage. |synctexoffset| is initialized to
the correct value when quite everything is initialized.

@ @<Glob...@>=
@!synctexoffset:integer; {holds the true value of |synctex_code|}

@ @<Initialize whatever...@>=
synctexoffset:=int_base+synctex_code;

@ {\sl Nota Bene:}
The {\sl synctex} code is very close to the memory model.
It is not connected to any other part of the code,
except for memory management. It is possible to neutralize the {\sl synctex} code
rather simply. The first step is to define a null |synctex_field_size|.
The second step is to comment out the code in ``Initialize bigger nodes...'' and every
``Copy ... {\sl synctex} information''.
The last step will be to comment out the |synctex_tag_field| related code in the
definition of |synctex_tag| and the various ``Prepare ... {\sl synctex} information''.
Then all the remaining code should be just harmless.
The resulting program would behave exactly the same as if absolutely no {\sl synctex}
related code was there, including memory management.
Of course, all this assumes that {\sl synctex} is turned off from the command line.
@^synctex@>
@^synchronization@>

@* \[54] System-dependent changes.
This section should be replaced, if necessary, by any special
modifications of the program
@z
