/****************************************************************************\
 Part of the XeTeX typesetting system
 copyright (c) 1994-2008 by SIL International
 copyright (c) 2009-2012 by Jonathan Kew
 copyright (c) 2012 by Khaled Hosny

 Written by Han The Thanh

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

@x
new_kern:=p;
end;
@y
new_kern:=p;
end;

@ Some stuff for character protrusion.
@# {memory structure for marginal kerns}
@d margin_kern_node = 40
@d margin_kern_node_size = 3
@d margin_char(#) == info(# + 2)    {unused for now; relevant for font expansion}

@# {|subtype| of marginal kerns}
@d left_side == 0
@d right_side == 1

@# {base for lp/rp codes starts from 2:
    0 for |hyphen_char|,
    1 for |skew_char|}
@d lp_code_base == 2
@d rp_code_base == 3

@d max_hlist_stack = 512 {maximum fill level for |hlist_stack|}
{maybe good if larger than |2 * max_quarterword|, so that box nesting level would overflow first}

@<Glob...@>=
@!last_leftmost_char: pointer;
@!last_rightmost_char: pointer;
@!hlist_stack:array[0..max_hlist_stack] of pointer; {stack for |find_protchar_left()| and |find_protchar_right()|}
@!hlist_stack_level:0..max_hlist_stack; {fill level for |hlist_stack|}
@!first_p: pointer; {to access the first node of the paragraph}
@!global_prev_p: pointer; {to access |prev_p| in |line_break|; should be kept in sync with |prev_p| by |update_prev_p|}
@z

@x
@<Search |hyph_list| for pointers to |p|@>;
end;
gubed
@y
@<Search |hyph_list| for pointers to |p|@>;
end;
gubed

@ Some stuff for character protrusion.

@d pdf_debug_on == true {change to |false| to suppress debugging info}
@p
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

procedure pdf_DEBUG(s: str_number);
begin
    if not pdf_debug_on then
        return;
    print_ln; print("DEBUG: "); print(s);
end;

procedure pdf_DEBUG_int(s: str_number; i: integer);
begin
    if not pdf_debug_on then
        return;
    print_ln; print("DEBUG: "); print(s); print_int(i);
end;

procedure pdf_DEBUG_str(s, s2: str_number);
begin
    if not pdf_debug_on then
        return;
    print_ln; print("DEBUG: "); print(s); print(s2);
end;

function prev_rightmost(s, e: pointer): pointer;
{finds the node preceding the rightmost node |e|; |s| is some node
before |e|}
var p: pointer;
begin
    prev_rightmost := null;
    p := s;
    if p = null then
        return;
    while link(p) <> e do begin
        p := link(p);
        if p = null then
            return;
    end;
    prev_rightmost := p;
end;

function round_xn_over_d(@!x:scaled; @!n,@!d:integer):scaled;
var positive:boolean; {was |x>=0|?}
@!t,@!u,@!v:nonnegative_integer; {intermediate quantities}
begin if x>=0 then positive:=true
else  begin negate(x); positive:=false;
  end;
t:=(x mod @'100000)*n;
u:=(x div @'100000)*n+(t div @'100000);
v:=(u mod d)*@'100000 + (t mod @'100000);
if u div d>=@'100000 then arith_error:=true
else u:=@'100000*(u div d) + (v div d);
v := v mod d;
if 2*v >= d then
    incr(u);
if positive then
    round_xn_over_d := u
else
    round_xn_over_d := -u;
end;
@z

@x
  kern_node: @<Display kern |p|@>;
@y
  kern_node: @<Display kern |p|@>;
  margin_kern_node: begin
    print_esc("kern");
    print_scaled(width(p));
    if subtype(p) = left_side then
        print(" (left margin)")
    else
        print(" (right margin)");
    end;
@z

@x
show_node_list(p); {the show starts at |p|}
print_ln;
end;
@y
show_node_list(p); {the show starts at |p|}
print_ln;
end;

procedure short_display_n(@!p, m:integer); {prints highlights of list |p|}
begin
    breadth_max := m;
    depth_threshold:=pool_size-pool_ptr-1;
    show_node_list(p); {the show starts at |p|}
end;
@z

@x
    kern_node,math_node,penalty_node:begin
        free_node(p, medium_node_size);
        goto done;
      end;
@y
    kern_node,math_node,penalty_node:begin
        free_node(p, medium_node_size);
        goto done;
      end;
    margin_kern_node: begin
{         free_avail(margin_char(p)); }
        free_node(p, margin_kern_node_size);
        goto done;
      end;
@z

@x
  words:=medium_node_size;
  end;
@y
  words:=medium_node_size;
  end;
margin_kern_node: begin
    r := get_node(margin_kern_node_size);
{     fast_get_avail(margin_char(r)); }
{     font(margin_char(r)) := font(margin_char(p)); }
{     character(margin_char(r)) := character(margin_char(p)); }
    words := margin_kern_node_size;
  end;
@z

@x
@d XeTeX_linebreak_penalty_code=etex_int_base+11 {penalty to use at locale-dependent linebreak locations}
@d eTeX_state_code=etex_int_base+12 {\eTeX\ state variables}
@y
@d XeTeX_linebreak_penalty_code=etex_int_base+11 {penalty to use at locale-dependent linebreak locations}
@d XeTeX_protrude_chars_code=etex_int_base+12 {protrude chars at left/right edge of paragraphs}
@d eTeX_state_code=etex_int_base+13 {\eTeX\ state variables}
@z

@x
@d XeTeX_linebreak_penalty==int_par(XeTeX_linebreak_penalty_code)
@y
@d XeTeX_linebreak_penalty==int_par(XeTeX_linebreak_penalty_code)
@d XeTeX_protrude_chars==int_par(XeTeX_protrude_chars_code)
@z

@x
XeTeX_linebreak_penalty_code:print_esc("XeTeXlinebreakpenalty");
@y
XeTeX_linebreak_penalty_code:print_esc("XeTeXlinebreakpenalty");
XeTeX_protrude_chars_code:print_esc("XeTeXprotrudechars");
@z

@x
primitive("XeTeXlinebreakpenalty",assign_int,int_base+XeTeX_linebreak_penalty_code);@/
@y
primitive("XeTeXlinebreakpenalty",assign_int,int_base+XeTeX_linebreak_penalty_code);@/
primitive("XeTeXprotrudechars",assign_int,int_base+XeTeX_protrude_chars_code);@/
@!@:XeTeX_protrude_chars_}{\.{\\XeTeXprotrudechars} primitive@>
@z

@x
if m=0 then scanned_result(hyphen_char[cur_val])(int_val)
else scanned_result(skew_char[cur_val])(int_val);
@y
if m=0 then scanned_result(hyphen_char[cur_val])(int_val)
else if m=1 then scanned_result(skew_char[cur_val])(int_val)
else begin
    n := cur_val;
    if is_native_font(n) then scan_glyph_number(n)
    else scan_char_num;
    k := cur_val;
    case m of
    lp_code_base: scanned_result(get_cp_code(n, k, left_side))(int_val);
    rp_code_base: scanned_result(get_cp_code(n, k, right_side))(int_val);
    end;
end;
@z

@x
|scan_something_internal|.

@ @<Declare procedures that scan restricted classes of integers@>=
@y
|scan_something_internal|.

@ @<Declare procedures that scan restricted classes of integers@>=

procedure scan_glyph_number(f: internal_font_number);
{ scan a glyph ID for native font |f|, identified by Unicode value or name or glyph number }
begin
  if scan_keyword("/") then {set cp value by glyph name}
  begin
    scan_and_pack_name; {result is in |nameoffile|}
    scanned_result(map_glyph_to_index(f))(int_val);
  end else if scan_keyword("u") then {set cp value by unicode}
  begin
    scan_char_num;
    scanned_result(map_char_to_glyph(f,cur_val))(int_val);
  end else
    scan_int;
end;

@z

@x
@d XeTeX_glyph_name_code=10

@d etex_convert_codes=XeTeX_glyph_name_code+1 {end of \eTeX's command codes}
@d job_name_code=etex_convert_codes {command code for \.{\\jobname}}

@<Put each...@>=
primitive("number",convert,number_code);@/
@!@:number_}{\.{\\number} primitive@>
primitive("romannumeral",convert,roman_numeral_code);@/
@!@:roman_numeral_}{\.{\\romannumeral} primitive@>
primitive("string",convert,string_code);@/
@!@:string_}{\.{\\string} primitive@>
primitive("meaning",convert,meaning_code);@/
@!@:meaning_}{\.{\\meaning} primitive@>
primitive("fontname",convert,font_name_code);@/
@!@:font_name_}{\.{\\fontname} primitive@>
primitive("jobname",convert,job_name_code);@/
@!@:job_name_}{\.{\\jobname} primitive@>

@ @<Cases of |print_cmd_chr|...@>=
convert: case chr_code of
  number_code: print_esc("number");
  roman_numeral_code: print_esc("romannumeral");
  string_code: print_esc("string");
  meaning_code: print_esc("meaning");
  font_name_code:  print_esc("fontname");
  pdf_strcmp_code: print_esc("strcmp");
@y
@d XeTeX_glyph_name_code=10

@d left_margin_kern_code=11
@d right_margin_kern_code=12

@d etex_convert_codes=right_margin_kern_code+1 {end of \eTeX's command codes}
@d job_name_code=etex_convert_codes {command code for \.{\\jobname}}

@<Put each...@>=
primitive("number",convert,number_code);@/
@!@:number_}{\.{\\number} primitive@>
primitive("romannumeral",convert,roman_numeral_code);@/
@!@:roman_numeral_}{\.{\\romannumeral} primitive@>
primitive("string",convert,string_code);@/
@!@:string_}{\.{\\string} primitive@>
primitive("meaning",convert,meaning_code);@/
@!@:meaning_}{\.{\\meaning} primitive@>
primitive("fontname",convert,font_name_code);@/
@!@:font_name_}{\.{\\fontname} primitive@>
primitive("jobname",convert,job_name_code);@/
@!@:job_name_}{\.{\\jobname} primitive@>
primitive("leftmarginkern",convert,left_margin_kern_code);@/
@!@:left_margin_kern_}{\.{\\leftmarginkern} primitive@>
primitive("rightmarginkern",convert,right_margin_kern_code);@/
@!@:right_margin_kern_}{\.{\\rightmarginkern} primitive@>

@ @<Cases of |print_cmd_chr|...@>=
convert: case chr_code of
  number_code: print_esc("number");
  roman_numeral_code: print_esc("romannumeral");
  string_code: print_esc("string");
  meaning_code: print_esc("meaning");
  font_name_code:  print_esc("fontname");
  pdf_strcmp_code: print_esc("strcmp");
  left_margin_kern_code:    print_esc("leftmarginkern");
  right_margin_kern_code:   print_esc("rightmarginkern");
@z

@x
@!i:small_number;
@!quote_char:UTF16_code;
@y
@!i:small_number;
@!quote_char:UTF16_code;
p, q: pointer;
@z

@x
glue_node: @<Move right or output leaders@>;
@y
glue_node: @<Move right or output leaders@>;
margin_kern_node: begin
{     if subtype(p) =  right_side then }
{         pdf_DEBUG_int("right margin kern:", width(p)) }
{     else }
{         pdf_DEBUG_int("left margin kern:", width(p)); }
    cur_h:=cur_h+width(p);
end;
@z

@x
@ @<Set init...@>=adjust_tail:=null; last_badness:=0;
@y
@ @<Set init...@>=adjust_tail:=null; last_badness:=0;

@ Some stuff for character protrusion.

@d left_pw(#) == char_pw(#, left_side)
@d right_pw(#) == char_pw(#, right_side)

@p
function char_pw(p: pointer; side: small_number): scaled;
var f: internal_font_number;
    c: integer;
begin
    char_pw := 0;
    if side = left_side then
        last_leftmost_char := null
    else
        last_rightmost_char := null;
    if p = null then
        return;

    { native word }
    if is_native_word_node(p) then begin
        if native_glyph_info_ptr(p) <> null_ptr then begin
            f := native_font(p);
            char_pw := round_xn_over_d(quad(f), get_native_word_cp(p, side), 1000);
        end;
        return;
    end;

    { glyph node }
    if is_glyph_node(p) then begin
        f := native_font(p);
        char_pw := round_xn_over_d(quad(f), get_cp_code(f, native_glyph(p), side), 1000);
        return;
    end;

    { char node or ligature; same like pdftex }
    if not is_char_node(p) then begin
        if type(p) = ligature_node then
            p := lig_char(p)
        else
            return;
    end;
    f := font(p);
    c := get_cp_code(f, character(p), side);
    case side of
    left_side:
        last_leftmost_char := p;
    right_side:
        last_rightmost_char := p;
    endcases;
    if c = 0 then
        return;
    char_pw :=
        round_xn_over_d(quad(f), c, 1000);
end;

function new_margin_kern(w: scaled; p: pointer; side: small_number): pointer;
var k: pointer;
begin
    k := get_node(margin_kern_node_size);
    type(k) := margin_kern_node;
    subtype(k) := side;
    width(k) := w;
{     if p = null then }
{         pdf_error("margin kerning", "invalid pointer to marginal char node"); }
{     fast_get_avail(margin_char(k)); }
{     character(margin_char(k)) := character(p); }
{     font(margin_char(k)) := font(p); }
    new_margin_kern := k;
end;
@z

@x
  kern_node: x:=x+width(p);
@y
  kern_node: x:=x+width(p);
  margin_kern_node: x:=x+width(p);
@z

@x
@d deactivate=60 {go here when node |r| should be deactivated}

@<Declare subprocedures for |line_break|@>=
@y
@d deactivate=60 {go here when node |r| should be deactivated}

@d cp_skipable(#) == {skipable nodes at the margins during character protrusion}
(
    not is_char_node(#) and
    (
        (type(#) = ins_node)
        or (type(#) = mark_node)
        or (type(#) = adjust_node)
        or (type(#) = penalty_node)
        or ((type(#) = disc_node) and
            (pre_break(#) = null) and
            (post_break(#) = null) and
            (replace_count(#) = 0)) {an empty |disc_node|}
        or ((type(#) = math_node) and (width(#) = 0))
        or ((type(#) = kern_node) and
            ((width(#) = 0) or (subtype(#) = normal)))
        or ((type(#) = glue_node) and (glue_ptr(#) = zero_glue))
        or ((type(#) = hlist_node) and (width(#) = 0) and (height(#) = 0) and
            (depth(#) = 0) and (list_ptr(#) = null))
    )
)

@<Declare subprocedures for |line_break|@>=
procedure push_node(p: pointer);
begin
    if hlist_stack_level > max_hlist_stack then
        pdf_error("push_node", "stack overflow");
    hlist_stack[hlist_stack_level] := p;
    hlist_stack_level := hlist_stack_level + 1;
end;

function pop_node: pointer;
begin
    hlist_stack_level := hlist_stack_level - 1;
    if hlist_stack_level < 0 then {would point to some bug}
        pdf_error("pop_node", "stack underflow (internal error)");
    pop_node := hlist_stack[hlist_stack_level];
end;

function find_protchar_left(l: pointer; d: boolean): pointer;
{searches left to right from list head |l|, returns 1st non-skipable item}
var t: pointer;
    run: boolean;
begin
    if (link(l) <> null) and (type(l) = hlist_node) and (width(l) = 0)
        and (height(l) = 0) and (depth(l) = 0) and (list_ptr(l) = null) then
        l := link(l) {for paragraph start with \.{\\parindent = 0pt}}
    else if d then
            while (link(l) <> null) and (not (is_char_node(l) or non_discardable(l))) do
                l := link(l); {std.\ discardables at line break, \TeX book, p 95}
    hlist_stack_level := 0;
    run := true;
    repeat
        t := l;
        while run and (type(l) = hlist_node) and (list_ptr(l) <> null) do begin
            push_node(l);
            l := list_ptr(l);
        end;
        while run and cp_skipable(l) do begin
            while (link(l) = null) and (hlist_stack_level > 0) do begin
                l := pop_node; {don't visit this node again}
            end;
            if link(l) <> null then
                l := link(l)
            else if hlist_stack_level = 0 then run := false
        end;
    until t = l;
    find_protchar_left := l;
end;

function find_protchar_right(l, r: pointer): pointer;
{searches right to left from list tail |r| to head |l|, returns 1st non-skipable item}
var t: pointer;
    run: boolean;
begin
    find_protchar_right := null;
    if r = null then return;
    hlist_stack_level := 0;
    run := true;
    repeat
        t := r;
        while run and (type(r) = hlist_node) and (list_ptr(r) <> null) do begin
            push_node(l);
            push_node(r);
            l := list_ptr(r);
            r := l;
            while link(r) <> null do
                r := link(r);
        end;
        while run and cp_skipable(r) do begin
            while (r = l) and (hlist_stack_level > 0) do begin
                r := pop_node; {don't visit this node again}
                l := pop_node;
            end;
            if (r <> l) and (r <> null) then
                r := prev_rightmost(l, r)
            else if (r = l) and (hlist_stack_level = 0) then run := false
        end;
    until t = r;
    find_protchar_right := r;
end;

function total_pw(q, p: pointer): scaled;
{returns the total width of character protrusion of a line;
|cur_break(break_node(q))| and |p| is the leftmost resp. rightmost node in the
horizontal list representing the actual line}
var l, r: pointer;
    n: integer;
begin
    if break_node(q) = null then
        l := first_p
    else
        l := cur_break(break_node(q));
    r := prev_rightmost(global_prev_p, p); {get |link(r)=p|}
    {let's look at the right margin first}
    if (p <> null) and (type(p) = disc_node) and (pre_break(p) <> null) then
    {a |disc_node| with non-empty |pre_break|, protrude the last char of |pre_break|}
    begin
        r := pre_break(p);
        while link(r) <> null do
            r := link(r);
    end else r := find_protchar_right(l, r);
    {now the left margin}
    if (l <> null) and (type(l) = disc_node) then begin
        if post_break(l) <> null then begin
            l := post_break(l); {protrude the first char}
            goto done;
        end else {discard |replace_count(l)| nodes}
        begin
            n := replace_count(l);
            l := link(l);
            while n > 0 do begin
                if link(l) <> null then
                    l := link(l);
                decr(n);
            end;
        end;
    end;
    l := find_protchar_left(l, true);
done:
    total_pw := left_pw(l) + right_pw(r);
end;
@z

@x
shortfall:=line_width-cur_active_width[1]; {we're this much too short}
@y
shortfall:=line_width-cur_active_width[1]; {we're this much too short}
if XeTeX_protrude_chars > 1 then
    shortfall := shortfall + total_pw(r, cur_p);
@z

@x
entire paragraph.

@<Find optimal breakpoints@>=
threshold:=pretolerance;
if threshold>=0 then
  begin @!stat if tracing_paragraphs>0 then
    begin begin_diagnostic; print_nl("@@firstpass");@+end;@;@+tats@;@/
  second_pass:=false; final_pass:=false;
  end
else  begin threshold:=tolerance; second_pass:=true;
  final_pass:=(emergency_stretch<=0);
  @!stat if tracing_paragraphs>0 then begin_diagnostic;@+tats@;
  end;
loop@+  begin if threshold>inf_bad then threshold:=inf_bad;
  if second_pass then @<Initialize for hyphenating a paragraph@>;
  @<Create an active breakpoint representing the beginning of the paragraph@>;
  cur_p:=link(temp_head); auto_breaking:=true;@/
  prev_p:=cur_p; {glue at beginning is not a legal breakpoint}
@y
entire paragraph.

@d update_prev_p == begin
    prev_p := cur_p;
    global_prev_p := cur_p;
end

@<Find optimal breakpoints@>=
threshold:=pretolerance;
if threshold>=0 then
  begin @!stat if tracing_paragraphs>0 then
    begin begin_diagnostic; print_nl("@@firstpass");@+end;@;@+tats@;@/
  second_pass:=false; final_pass:=false;
  end
else  begin threshold:=tolerance; second_pass:=true;
  final_pass:=(emergency_stretch<=0);
  @!stat if tracing_paragraphs>0 then begin_diagnostic;@+tats@;
  end;
loop@+  begin if threshold>inf_bad then threshold:=inf_bad;
  if second_pass then @<Initialize for hyphenating a paragraph@>;
  @<Create an active breakpoint representing the beginning of the paragraph@>;
  cur_p:=link(temp_head); auto_breaking:=true;@/
  update_prev_p; {glue at beginning is not a legal breakpoint}
  first_p := cur_p; {to access the first node of paragraph as the first active
                     node has |break_node=null|}
@z

@x
@:this can't happen paragraph}{\quad paragraph@>
endcases;@/
prev_p:=cur_p; cur_p:=link(cur_p);
done5:end

@ The code that passes over the characters of words in a paragraph is
part of \TeX's inner loop, so it has been streamlined for speed. We use
the fact that `\.{\\parfillskip}' glue appears at the end of each paragraph;
it is therefore unnecessary to check if |link(cur_p)=null| when |cur_p| is a
character node.
@^inner loop@>

@<Advance \(c)|cur_p| to the node following the present string...@>=
begin prev_p:=cur_p;
@y
@:this can't happen paragraph}{\quad paragraph@>
endcases;@/
update_prev_p; cur_p:=link(cur_p);
done5:end

@ The code that passes over the characters of words in a paragraph is
part of \TeX's inner loop, so it has been streamlined for speed. We use
the fact that `\.{\\parfillskip}' glue appears at the end of each paragraph;
it is therefore unnecessary to check if |link(cur_p)=null| when |cur_p| is a
character node.
@^inner loop@>

@<Advance \(c)|cur_p| to the node following the present string...@>=
begin update_prev_p;
@z

@x
  decr(r); s:=link(s);
  end;
prev_p:=cur_p; cur_p:=s; goto done5;
@y
  decr(r); s:=link(s);
  end;
update_prev_p; cur_p:=s; goto done5;
@z

@x
var q,@!r,@!s:pointer; {temporary registers for list manipulation}
@y
var q,@!r,@!s:pointer; {temporary registers for list manipulation}
    p, k: pointer;
    w: scaled;
    glue_break: boolean; {was a break at glue?}
    ptmp: pointer;
@z

@x
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
    else if type(q)=kern_node then width(q):=0
    else if type(q)=math_node then
      begin width(q):=0;
      if TeXXeT_en then @<Adjust \(t)the LR stack for the |p...@>;
      end;
    end
else  begin q:=temp_head;
  while link(q)<>null do q:=link(q);
  end;
@<Put the \(r)\.{\\rightskip} glue after node |q|@>;
done:
@y
q:=cur_break(cur_p); disc_break:=false; post_disc_break:=false;
glue_break := false;
if q<>null then {|q| cannot be a |char_node|}
  if type(q)=glue_node then
    begin delete_glue_ref(glue_ptr(q));
    glue_ptr(q):=right_skip;
    subtype(q):=right_skip_code+1; add_glue_ref(right_skip);
    glue_break := true;
    goto done;
    end
  else  begin if type(q)=disc_node then
      @<Change discretionary to compulsory and set
        |disc_break:=true|@>
    else if type(q)=kern_node then width(q):=0
    else if type(q)=math_node then
      begin width(q):=0;
      if TeXXeT_en then @<Adjust \(t)the LR stack for the |p...@>;
      end;
    end
else  begin q:=temp_head;
  while link(q)<>null do q:=link(q);
  end;
done:
{at this point |q| is the rightmost breakpoint; the only exception is the case
of a discretionary break with non-empty |pre_break|, then |q| has been changed
to the last node of the |pre_break| list}
if XeTeX_protrude_chars > 0 then begin
{     short_display_n(q, 5); }
    if disc_break and (is_char_node(q) or (type(q) <> disc_node))
    {|q| has been reset to the last node of |pre_break|}
    then begin
        p := q;
        ptmp := p;
    end else begin
        p := prev_rightmost(link(temp_head), q); {get |link(p) = q|}
        ptmp := p;
        p := find_protchar_right(link(temp_head), p);
    end;
{     short_display_n(p, 5); }
    w := right_pw(p);
    if w <> 0 then {we have found a marginal kern, append it after |ptmp|}
    begin
{         pdf_DEBUG_int("right margin:", w); }
        k := new_margin_kern(-w, last_rightmost_char, right_side);
        link(k) := link(ptmp);
        link(ptmp) := k;
        if (ptmp = q) then
            q := link(q);
    end;
end;
{if |q| was not a breakpoint at glue and has been reset to |rightskip| then
 we append |rightskip| after |q| now}
if not glue_break then begin
    @<Put the \(r)\.{\\rightskip} glue after node |q|@>;
end;
@z

@x
r:=link(q); link(q):=null; q:=link(temp_head); link(temp_head):=r;
@y
r:=link(q); link(q):=null; q:=link(temp_head); link(temp_head):=r;
{at this point |q| is the leftmost node; all discardable nodes have been discarded}
if XeTeX_protrude_chars > 0 then begin
    p := q;
    p := find_protchar_left(p, false); {no more discardables}
    w := left_pw(p);
    if w <> 0 then begin
        k := new_margin_kern(-w, last_leftmost_char, left_side);
        link(k) := q;
        q := k;
    end;
end;
@z

@x
var p:pointer; {the box}
@!c:box_code..copy_code; {should we copy?}
begin if cur_chr>copy_code then @<Handle saved items and |goto done|@>;
c:=cur_chr; scan_register_num; fetch_box(p);
if p=null then return;
if (abs(mode)=mmode)or((abs(mode)=vmode)and(type(p)<>vlist_node))or@|
   ((abs(mode)=hmode)and(type(p)<>hlist_node)) then
  begin print_err("Incompatible list can't be unboxed");
@.Incompatible list...@>
  help3("Sorry, Pandora. (You sneaky devil.)")@/
  ("I refuse to unbox an \hbox in vertical mode or vice versa.")@/
  ("And I can't open any boxes in math mode.");@/
  error; return;
  end;
if c=copy_code then link(tail):=copy_node_list(list_ptr(p))
else  begin link(tail):=list_ptr(p); change_box(null);
  free_node(p,box_node_size);
  end;
done:
while link(tail)<>null do tail:=link(tail);
@y
var p:pointer; {the box}
    r: pointer; {to remove marginal kern nodes}
@!c:box_code..copy_code; {should we copy?}
begin if cur_chr>copy_code then @<Handle saved items and |goto done|@>;
c:=cur_chr; scan_register_num; fetch_box(p);
if p=null then return;
if (abs(mode)=mmode)or((abs(mode)=vmode)and(type(p)<>vlist_node))or@|
   ((abs(mode)=hmode)and(type(p)<>hlist_node)) then
  begin print_err("Incompatible list can't be unboxed");
@.Incompatible list...@>
  help3("Sorry, Pandora. (You sneaky devil.)")@/
  ("I refuse to unbox an \hbox in vertical mode or vice versa.")@/
  ("And I can't open any boxes in math mode.");@/
  error; return;
  end;
if c=copy_code then link(tail):=copy_node_list(list_ptr(p))
else  begin link(tail):=list_ptr(p); change_box(null);
  free_node(p,box_node_size);
  end;
done:
while link(tail) <> null do begin
    r := link(tail);
    if not is_char_node(r) and (type(r) = margin_kern_node) then begin
        link(tail) := link(r);
{         free_avail(margin_char(r)); }
        free_node(r, margin_kern_node_size);
    end;
    tail:=link(tail);
end;
@z

@x
kern_node: d:=width(p);
@y
kern_node: d:=width(p);
margin_kern_node: d:=width(p);
@z

@x
assign_font_int: begin n:=cur_chr; scan_font_ident; f:=cur_val;
  scan_optional_equals; scan_int;
  if n=0 then hyphen_char[f]:=cur_val@+else skew_char[f]:=cur_val;
  end;

@ @<Put each...@>=
primitive("hyphenchar",assign_font_int,0);
@!@:hyphen_char_}{\.{\\hyphenchar} primitive@>
primitive("skewchar",assign_font_int,1);
@!@:skew_char_}{\.{\\skewchar} primitive@>

@ @<Cases of |print_cmd_chr|...@>=
assign_font_int: if chr_code=0 then print_esc("hyphenchar")
  else print_esc("skewchar");
@y
assign_font_int: begin n:=cur_chr; scan_font_ident; f:=cur_val;
  if n < lp_code_base then begin
    scan_optional_equals; scan_int;
    if n=0 then hyphen_char[f]:=cur_val@+else skew_char[f]:=cur_val;
  end else begin
    if is_native_font(f) then
      scan_glyph_number(f) {for native fonts, the value is a glyph id}
    else scan_char_num; {for |tfm| fonts it's the same like pdftex}
    p := cur_val;
    scan_optional_equals; scan_int;
    case n of
      lp_code_base: set_cp_code(f, p, left_side, cur_val);
      rp_code_base: set_cp_code(f, p, right_side, cur_val);
    endcases;
  end;
end;

@ @<Put each...@>=
primitive("hyphenchar",assign_font_int,0);
@!@:hyphen_char_}{\.{\\hyphenchar} primitive@>
primitive("skewchar",assign_font_int,1);
@!@:skew_char_}{\.{\\skewchar} primitive@>
primitive("lpcode",assign_font_int,lp_code_base);
@!@:lp_code_}{\.{\\lpcode} primitive@>
primitive("rpcode",assign_font_int,rp_code_base);
@!@:rp_code_}{\.{\\rpcode} primitive@>

@ @<Cases of |print_cmd_chr|...@>=
assign_font_int: case chr_code of
0: print_esc("hyphenchar");
1: print_esc("skewchar");
lp_code_base: print_esc("lpcode");
rp_code_base: print_esc("rpcode");
endcases;
@z

@x
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
@y
        flush_string;
end;
@z

@x
      not_native_font_error(convert, c, fnt);
  end;

@ @<Cases of `Print the result of command |c|'@>=
eTeX_revision_code: print(eTeX_revision);
pdf_strcmp_code: print_int(cur_val);
XeTeX_revision_code: print(XeTeX_revision);

XeTeX_variation_name_code:
    if is_atsu_font(fnt) then
      atsu_print_font_name(c, font_layout_engine[fnt], arg1, arg2);

XeTeX_feature_name_code,
XeTeX_selector_name_code:
    if is_atsu_font(fnt) then
      atsu_print_font_name(c, font_layout_engine[fnt], arg1, arg2)
    else if is_gr_font(fnt) then
      gr_print_font_name(c, font_layout_engine[fnt], arg1, arg2);

XeTeX_glyph_name_code:
    if is_native_font(fnt) then print_glyph_name(fnt, arg1);
@y
      not_native_font_error(convert, c, fnt);
  end;

left_margin_kern_code, right_margin_kern_code: begin
    scan_register_num;
    fetch_box(p);
    if (p = null) or (type(p) <> hlist_node) then
        pdf_error("marginkern", "a non-empty hbox expected")
end;

@ @<Cases of `Print the result of command |c|'@>=
eTeX_revision_code: print(eTeX_revision);
pdf_strcmp_code: print_int(cur_val);
XeTeX_revision_code: print(XeTeX_revision);

XeTeX_variation_name_code:
    if is_atsu_font(fnt) then
      atsu_print_font_name(c, font_layout_engine[fnt], arg1, arg2);

XeTeX_feature_name_code,
XeTeX_selector_name_code:
    if is_atsu_font(fnt) then
      atsu_print_font_name(c, font_layout_engine[fnt], arg1, arg2)
    else if is_gr_font(fnt) then
      gr_print_font_name(c, font_layout_engine[fnt], arg1, arg2);

XeTeX_glyph_name_code:
    if is_native_font(fnt) then print_glyph_name(fnt, arg1);

left_margin_kern_code: begin
    p := list_ptr(p);
    while (p <> null) and
          (cp_skipable(p) or
           ((not is_char_node(p)) and (type(p) = glue_node) and (subtype(p) = left_skip_code + 1)))
    do
       p := link(p);
    if (p <> null) and (not is_char_node(p)) and
       (type(p) = margin_kern_node) and (subtype(p) = left_side) then
        print_scaled(width(p))
    else
        print("0");
    print("pt");
end;

right_margin_kern_code: begin
    q := list_ptr(p);
    p := prev_rightmost(q, null);
    while (p <> null) and
          (cp_skipable(p) or
           ((not is_char_node(p)) and (type(p) = glue_node) and (subtype(p) = right_skip_code + 1)))
    do
        p := prev_rightmost(q, p);
    if (p <> null) and (not is_char_node(p)) and
       (type(p) = margin_kern_node) and (subtype(p) = right_side) then
        print_scaled(width(p))
    else
        print("0");
    print("pt");
end;
@z

