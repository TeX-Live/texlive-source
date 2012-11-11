/****************************************************************************\
 Part of the XeTeX typesetting system
 copyright (c) 1994-2008 by SIL International
 copyright (c) 2009-2012 by Jonathan Kew
 copyright (c) 2012 by Khaled Hosny

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

@x
\let\maybe=\iffalse
@y
\let\maybe=\iftrue
@z

@x [1] m.2 l.188 - banner
@d banner==TeX_banner
@d banner_k==TeX_banner_k
@y
@d banner==XeTeX_banner
@d banner_k==XeTeX_banner
@z

@x
xchr: array [ASCII_code] of text_char;
   { specifies conversion of output characters }
xprn: array [ASCII_code] of ASCII_code;
   { non zero iff character is printable }
@y
@!xchr: ^text_char;
  {dummy variable so tangle doesn't complain; not actually used}
@z

@x
{Initialize |xchr| to the identity mapping.}
for i:=0 to @'37 do xchr[i]:=i;
for i:=@'177 to @'377 do xchr[i]:=i;
@y
@z

@x
for i:=0 to @'176 do xord[xchr[i]]:=i;
{Set |xprn| for printable ASCII, unless |eight_bit_p| is set.}
for i:=0 to 255 do xprn[i]:=(eight_bit_p or ((i>=" ")and(i<="~")));

{The idea for this dynamic translation comes from the patch by
 Libor Skarvada \.{<libor@@informatics.muni.cz>}
 and Petr Sojka \.{<sojka@@informatics.muni.cz>}. I didn't use any of the
 actual code, though, preferring a more general approach.}

{This updates the |xchr|, |xord|, and |xprn| arrays from the provided
 |translate_filename|.  See the function definition in \.{texmfmp.c} for
 more comments.}
if translate_filename then read_tcx_file;
@y
@z

@x
@!name_of_file:^text_char;
@y
@!name_of_file:^UTF8_code; {we build filenames in utf8 to pass to the OS}
@z

@x
@!name_of_file16:array[1..file_name_size] of UTF16_code;@;@/
@y
@!name_of_file16:^UTF16_code;
@z

@x
@!buffer:^ASCII_code; {lines of characters being read}
@y
@!buffer:^UnicodeScalar; {lines of characters being read}
@z

@x
@d term_in==stdin {the terminal as an input file}
@y
@z

@x
@!bound_default:integer; {temporary for setup}
@y
@!term_in:unicode_file;
@#
@!bound_default:integer; {temporary for setup}
@z

@x
if translate_filename then begin
  wterm(' (');
  fputs(translate_filename, stdout);
  wterm_ln(')');
@y
if translate_filename then begin
  wterm(' (WARNING: translate-file "');
  fputs(translate_filename, stdout);
  wterm_ln('" ignored)');
@z

@x
k:=first; while k < last do begin print_buffer(k) end;
@y
if last<>first then for k:=first to last-1 do print(buffer[k]);
@z

@x [6.84] l.1904 - Implement the switch-to-editor option.
    begin edit_name_start:=str_start[edit_file.name_field];
    edit_name_length:=str_start[edit_file.name_field+1] -
                      str_start[edit_file.name_field];
@y
    begin edit_name_start:=str_start_macro(edit_file.name_field);
    edit_name_length:=str_start_macro(edit_file.name_field+1) -
                      str_start_macro(edit_file.name_field);
@z

@x [8.110] l.2423
@d max_halfword==@"FFFFFFF {largest allowable value in a |halfword|}
@y
@d max_halfword==@"3FFFFFFF {largest allowable value in a |halfword|}
@z

@x [15.209] l.4165
@d shorthand_def=95 {code definition ( \.{\\chardef}, \.{\\countdef}, etc.~)}
@y
@d shorthand_def=97 {code definition ( \.{\\chardef}, \.{\\countdef}, etc.~)}
@z

@x [17.222] l.4523 - frozen_special, for source specials.
@d frozen_null_font=frozen_control_sequence+11
@y
@d frozen_primitive=frozen_control_sequence+11
  {permanent `\.{\\primitive}'}
@d frozen_null_font=frozen_control_sequence+12
@z

@x [17.230] l.4731
@d char_sub_code_base=math_code_base+256 {table of character substitutions}
@d int_base=char_sub_code_base+256 {beginning of region 5}
@y
@d char_sub_code_base=math_code_base+number_usvs {table of character substitutions}
@d int_base=char_sub_code_base+number_usvs {beginning of region 5}
@z

@x [17.236] l.4960 - first web2c, then e-TeX additional integer parameters
@d int_pars=web2c_int_pars {total number of integer parameters}
@#
@d etex_int_base=tex_int_pars {base for \eTeX's integer parameters}
@y
@d etex_int_base=web2c_int_pars {base for \eTeX's integer parameters}
@z

@x
@!input_file : ^alpha_file;
@y
@!input_file : ^unicode_file;
@z

@x
@* \[29] File names.
@y
@* \[29] File names.
@z

@x [29.516] l.9992 - filenames: more_name
begin if (c=" ") and stop_at_space and (not quoted_filename) then
  more_name:=false
else  if c="""" then begin
  quoted_filename:=not quoted_filename;
@y
begin if stop_at_space and (c=" ") and (file_name_quote_char=0) then
  more_name:=false
else if stop_at_space and (file_name_quote_char<>0) and (c=file_name_quote_char) then begin
  file_name_quote_char:=0;
  more_name:=true;
  end
else if stop_at_space and (file_name_quote_char=0) and ((c="""") or (c="'") or (c="(")) then begin
  if c="(" then file_name_quote_char:=")"
  else file_name_quote_char:=c;
  quoted_filename:=true;
@z

@x [29.517] l.10002 - end_name: string recycling
@!j,@!s,@!t: pool_pointer; {running indices}
@!must_quote:boolean; {whether we need to quote a string}
@y
@!j: pool_pointer; {running index}
@z
@x [29.517] l.10002 - end_name: string recycling
str_room(6); {Room for quotes, if needed.}
{add quotes if needed}
if area_delimiter<>0 then begin
  {maybe quote |cur_area|}
  must_quote:=false;
  s:=str_start[str_ptr];
  t:=str_start[str_ptr]+area_delimiter;
  j:=s;
  while (not must_quote) and (j<t) do begin
    must_quote:=str_pool[j]=" "; incr(j);
    end;
  if must_quote then begin
    for j:=pool_ptr-1 downto t do str_pool[j+2]:=str_pool[j];
    str_pool[t+1]:="""";
    for j:=t-1 downto s do str_pool[j+1]:=str_pool[j];
    str_pool[s]:="""";
    if ext_delimiter<>0 then ext_delimiter:=ext_delimiter+2;
    area_delimiter:=area_delimiter+2;
    pool_ptr:=pool_ptr+2;
    end;
  end;
{maybe quote |cur_name|}
s:=str_start[str_ptr]+area_delimiter;
if ext_delimiter=0 then t:=pool_ptr else t:=str_start[str_ptr]+ext_delimiter-1;
must_quote:=false;
j:=s;
while (not must_quote) and (j<t) do begin
  must_quote:=str_pool[j]=" "; incr(j);
  end;
if must_quote then begin
  for j:=pool_ptr-1 downto t do str_pool[j+2]:=str_pool[j];
  str_pool[t+1]:="""";
  for j:=t-1 downto s do str_pool[j+1]:=str_pool[j];
  str_pool[s]:="""";
  if ext_delimiter<>0 then ext_delimiter:=ext_delimiter+2;
  pool_ptr:=pool_ptr+2;
  end;
if ext_delimiter<>0 then begin
  {maybe quote |cur_ext|}
  s:=str_start[str_ptr]+ext_delimiter-1;
  t:=pool_ptr;
  must_quote:=false;
  j:=s;
  while (not must_quote) and (j<t) do begin
    must_quote:=str_pool[j]=" "; incr(j);
    end;
  if must_quote then begin
    str_pool[t+1]:="""";
    for j:=t-1 downto s do str_pool[j+1]:=str_pool[j];
    str_pool[s]:="""";
    pool_ptr:=pool_ptr+2;
    end;
  end;
@y
@z

@x
    for j:=str_start[str_ptr+1] to pool_ptr-1 do
@y
    for j:=str_start_macro(str_ptr+1) to pool_ptr-1 do
@z

@x
    for j:=str_start[str_ptr+1] to pool_ptr-1 do
@y
    for j:=str_start_macro(str_ptr+1) to pool_ptr-1 do
@z

@x [29.518] l.10042 - print_file_name: quote if spaces in names.
@d check_quoted(#) == {check if string |#| needs quoting}
if #<>0 then begin
  j:=str_start[#];
  while (not must_quote) and (j<str_start[#+1]) do begin
    must_quote:=str_pool[j]=" "; incr(j);
  end;
end
@y
@d check_quoted(#) == {check if string |#| needs quoting}
if #<>0 then begin
  j:=str_start_macro(#);
  while ((not must_quote) or (quote_char=0)) and (j<str_start_macro(#+1)) do begin
    if (str_pool[j]=" ") then must_quote:=true
    else if (str_pool[j]="""") or (str_pool[j]="'") then begin
      must_quote:=true;
      quote_char:="""" + "'" - str_pool[j];
    end;
    incr(j);
  end;
end
@z

@x [29.518] l.10042 - print_file_name: quote if spaces in names.
@d print_quoted(#) == {print string |#|, omitting quotes}
if #<>0 then
  for j:=str_start[#] to str_start[#+1]-1 do
    if so(str_pool[j])<>"""" then
      print(so(str_pool[j]))
@y
@d print_quoted(#) == {print string |#|, omitting quotes}
if #<>0 then
  for j:=str_start_macro(#) to str_start_macro(#+1)-1 do begin
    if str_pool[j]=quote_char then begin
      print(quote_char);
      quote_char:="""" + "'" - quote_char;
      print(quote_char);
    end;
    print(str_pool[j]);
  end
@z

@x [29.518] l.10042 - print_file_name: quote if spaces in names.
@!j:pool_pointer; {index into |str_pool|}
begin
must_quote:=false;
check_quoted(a); check_quoted(n); check_quoted(e);
{FIXME: Alternative is to assume that any filename that has to be quoted has
 at least one quoted component...if we pick this, a number of insertions
 of |print_file_name| should go away.
|must_quote|:=((|a|<>0)and(|str_pool|[|str_start|[|a|]]=""""))or
              ((|n|<>0)and(|str_pool|[|str_start|[|n|]]=""""))or
              ((|e|<>0)and(|str_pool|[|str_start|[|e|]]=""""));}
if must_quote then print_char("""");
print_quoted(a); print_quoted(n); print_quoted(e);
if must_quote then print_char("""");
@y
@!quote_char: integer; {current quote char (single or double)}
@!j:pool_pointer; {index into |str_pool|}
begin
must_quote:=false;
quote_char:=0;
check_quoted(a); check_quoted(n); check_quoted(e);
if must_quote then begin
  if quote_char=0 then quote_char:="""";
  print_char(quote_char);
end;
print_quoted(a); print_quoted(n); print_quoted(e);
if quote_char<>0 then print_char(quote_char);
@z

@x
@d append_to_name(#)==begin c:=#; if not (c="""") then begin incr(k);
  if k<=file_name_size then name_of_file[k]:=xchr[c];
  end end
@y
@d append_to_name(#)==begin c:=#; incr(k);
  if k<=file_name_size then begin
      if (c < 128) then name_of_file[k]:=c
      else if (c < @"800) then begin
        name_of_file[k]:=@"C0 + c div @"40; incr(k);
        name_of_file[k]:=@"80 + c mod @"40;
      end else begin
		name_of_file[k]:=@"E0 + c div @"1000; incr(k);
		name_of_file[k]:=@"80 + (c mod @"1000) div @"40; incr(k);
		name_of_file[k]:=@"80 + (c mod @"1000) mod @"40;
      end
    end
  end
@z

@x
name_of_file:= xmalloc_array (ASCII_code, length(a)+length(n)+length(e)+1);
@y
name_of_file:= xmalloc_array (UTF8_code, (length(a)+length(n)+length(e))*3+1);
@z

@x
name_of_file := xmalloc_array (ASCII_code, n+(b-a+1)+format_ext_length+1);
for j:=1 to n do append_to_name(xord[ucharcast(TEX_format_default[j])]);
@y
name_of_file := xmalloc_array (UTF8_code, n+(b-a+1)+format_ext_length+1);
for j:=1 to n do append_to_name(TEX_format_default[j]);
@z

@x
  append_to_name(xord[ucharcast(TEX_format_default[j])]);
@y
  append_to_name(TEX_format_default[j]);
@z

@x
  k:=1;
  while (k<=name_length)and(more_name(name_of_file[k])) do
@y
  k:=0;
  while (k<name_length16)and(more_name(name_of_file16[k])) do
@z

@x
  {If |cur_chr| is a space and we're not scanning a token list, check
   whether we're at the end of the buffer. Otherwise we end up adding
   spurious spaces to file names in some cases.}
  if (cur_chr=" ") and (state<>token_list) and (loc>limit) then goto done;
@y
@z

@x
  wlog(' (');
  fputs(translate_filename, log_file);
  wlog(')');
@y
  wlog(' (WARNING: translate-file "');
  fputs(translate_filename, log_file);
  wlog('" ignored)');
@z

@x
     and a_open_in(cur_file, kpse_tex_format) then
    goto done;
@y
     and u_open_in(cur_file, kpse_tex_format, XeTeX_default_input_mode, XeTeX_default_input_encoding) then
    {At this point |name_of_file| contains the actual name found, as a UTF8 string.
     We convert to UTF16, then extract the |cur_area|, |cur_name|, and |cur_ext| from it.}
    begin
    make_utf16_name;
    name_in_progress:=true;
    begin_name;
    stop_at_space:=false;
    k:=0;
    while (k<name_length16)and(more_name(name_of_file16[k])) do
      incr(k);
    stop_at_space:=true;
    end_name;
    name_in_progress:=false;
    goto done;
    end;
@z

@x
@* \[30] Font metric data.
@y
@* \[30] Font metric data.
@z

@x
@!font_bc: ^eight_bits;
  {beginning (smallest) character code}
@!font_ec: ^eight_bits;
  {ending (largest) character code}
@y
@!font_bc: ^UTF16_code;
  {beginning (smallest) character code}
@!font_ec: ^UTF16_code;
  {ending (largest) character code}
@z

@x
@!font_false_bchar: ^nine_bits;
  {|font_bchar| if it doesn't exist in the font, otherwise |non_char|}
@y
@!font_false_bchar: ^nine_bits;
  {|font_bchar| if it doesn't exist in the font, otherwise |non_char|}
@#
@!font_layout_engine: ^void_pointer; { either an ATSUStyle or a XeTeXLayoutEngine }
@!font_mapping: ^void_pointer; { |TECkit_Converter| or 0 }
@!font_flags: ^char; { flags:
  0x01: |font_colored|
  0x02: |font_vertical| }
@!font_letter_space: ^scaled; { letterspacing to be applied to the font }
@!loaded_font_mapping: void_pointer; { used by |load_native_font| to return mapping, if any }
@!loaded_font_flags: char; { used by |load_native_font| to return flags }
@!loaded_font_letter_space: scaled;
@!loaded_font_design_size: scaled;
@!mapped_text: ^UTF16_code; { scratch buffer used while applying font mappings }
@!xdv_buffer: ^char; { scratch buffer used in generating XDV output }
@z

@x
  print_char("="); print_file_name(nom,aire,"");
@y
  print_char("=");
  if file_name_quote_char=")" then print_char("(")
  else if file_name_quote_char<>0 then print_char(file_name_quote_char);
  print_file_name(nom,aire,cur_ext);
  if file_name_quote_char<>0 then print_char(file_name_quote_char);
@z

@x
else print(" not loadable: Metric (TFM) file not found");
@y
else print(" not loadable: Metric (TFM) file or installed font not found");
@z

@x
@ @<Open |tfm_file| for input...@>=
file_opened:=false;
@y
@ @<Open |tfm_file| for input...@>=
@z

@x
@ When \TeX\ wants to typeset a character that doesn't exist, the
character node is not created; thus the output routine can assume
that characters exist when it sees them. The following procedure
prints a warning message unless the user has suppressed it.

@p procedure char_warning(@!f:internal_font_number;@!c:eight_bits);
var old_setting: integer; {saved value of |tracing_online|}
begin if tracing_lost_chars>0 then
 begin old_setting:=tracing_online;
 if eTeX_ex and(tracing_lost_chars>1) then tracing_online:=1;
  begin begin_diagnostic;
  print_nl("Missing character: There is no ");
@.Missing character@>
  if c < @"10000 then print_ASCII(c)
  else begin { non-Plane 0 Unicodes can't be sent through |print_ASCII| }
    print("character number ");
    print_hex(c);
  end;
  print(" in font ");
  slow_print(font_name[f]); print_char("!"); end_diagnostic(false);
  end;
 tracing_online:=old_setting;
 end;
end;
@y
@ Procedure |char_warning| has been moved in the source.
@z

@x
@p function new_character(@!f:internal_font_number;@!c:eight_bits):pointer;
@y
@p function new_character(@!f:internal_font_number;@!c:ASCII_code):pointer;
@z

@x
begin ec:=effective_char(false,f,qi(c));
@y
begin
if is_native_font(f) then
  begin new_character:=new_native_character(f,c); return;
  end;
ec:=effective_char(false,f,qi(c));
@z

@x
@* \[32] Shipping pages out.
@y
@* \[32] Shipping pages out.
@z

@x
  for s:=str_start[str_ptr] to pool_ptr-1 do dvi_out(so(str_pool[s]));
  pool_ptr:=str_start[str_ptr]; {flush the current string}
@y
  for s:=str_start_macro(str_ptr) to pool_ptr-1 do dvi_out(so(str_pool[s]));
  pool_ptr:=str_start_macro(str_ptr); {flush the current string}
@z

@x
label reswitch, move_past, fin_rule, next_p, continue, found;
@y
label reswitch, move_past, fin_rule, next_p, continue, found, check_next, end_node_run;
@z

@x
if list_ptr(p)=null then cur_v:=cur_v+height(p)+depth(p)
@y
if list_ptr(p)=null then begin
    if upwards then cur_v:=cur_v-depth(p)-height(p) else cur_v:=cur_v+height(p)+depth(p);
  end
@z

@x
  print_nl("Output written on "); print_file_name(0, output_file_name, 0);
@.Output written on x@>
  print(" ("); print_int(total_pages);
  if total_pages<>1 then print(" pages")
  else print(" page");
  print(", "); print_int(dvi_offset+dvi_ptr); print(" bytes).");
  b_close(dvi_file);
@y
  k:=dvi_close(dvi_file);
  if k=0 then begin
    print_nl("Output written on "); print(output_file_name);
@.Output written on x@>
    print(" ("); print_int(total_pages);
    if total_pages<>1 then print(" pages")
    else print(" page");
    if no_pdf_output then begin
      print(", "); print_int(dvi_offset+dvi_ptr); print(" bytes).");
    end else print(").");
  end else begin
    print_nl("Error "); print_int(k); print(" (");
    if no_pdf_output then print_c_string(strerror(k))
    else print("driver return code");
    print(") generating output;");
    print_nl("file "); print(output_file_name); print(" may not be valid.");
    end;
@z

@x
@* \[36] Typesetting math formulas.
@y
@* \[36] Typesetting math formulas.
@z

@x [36.749] l.14638 - MLTeX: avoid substitution in |make_op|
    begin c:=rem_byte(cur_i); i:=orig_char_info(cur_f)(c);
@y
      begin c:=rem_byte(cur_i); i:=orig_char_info(cur_f)(c);
@z

@x
@* \[42] Hyphenation.
@y
@* \[42] Hyphenation.
@z

% Conflict with ../tex.ch
@x
  begin j:=1; u:=str_start[k];
@y
  begin j:=1; u:=str_start_macro(k);
@z

% Conflict with ../tex.ch
@x
u:=str_start[k]; v:=str_start[s];
@y
u:=str_start_macro(k); v:=str_start_macro(s);
@z

% Conflict with ../tex.ch
@x
until u=str_start[k+1];
@y
until u=str_start_macro(k+1);
@z

@x
@* \[43] Initializing the hyphenation tables.
@y
@* \[43] Initializing the hyphenation tables.
@z

% Conflict with ../tex.ch
@x
@!trie_used:array[ASCII_code] of trie_opcode;
@y
@!trie_used:array[0..biggest_lang] of trie_opcode;
@z

% Conflict with ../tex.ch
@x
@!trie_op_lang:array[1..trie_op_size] of ASCII_code;
@y
@!trie_op_lang:array[1..trie_op_size] of 0..biggest_lang;
@z

% Conflict with ../tex.ch
@x
for k:=0 to 255 do trie_used[k]:=min_trie_op;
@y
for k:=0 to biggest_lang do trie_used[k]:=min_trie_op;
@z

% Conflict with ../tex.ch
@x
  begin for r:=0 to 256 do clear_trie;
  trie_max:=256;
@y
  begin for r:=0 to max_hyph_char do clear_trie;
  trie_max:=max_hyph_char;
@z

@x
@* \[49] Mode-independent processing.
@y
@* \[49] Mode-independent processing.
@z

@x [49.1222] l.22833 - MLTeX: \charsubdef primitive
else begin n:=cur_chr; get_r_token; p:=cur_cs; define(p,relax,256);
@y
else begin n:=cur_chr; get_r_token; p:=cur_cs; define(p,relax,too_big_usv);
@z

@x [49.1275] l.23441 - Same stuff as for \input, this time for \openin.
     and a_open_in(read_file[n], kpse_tex_format) then
    read_open[n]:=just_open;
@y
     and u_open_in(read_file[n], kpse_tex_format, XeTeX_default_input_mode, XeTeX_default_input_encoding) then
    begin
    make_utf16_name;
    name_in_progress:=true;
    begin_name;
    stop_at_space:=false;
    k:=0;
    while (k<name_length16)and(more_name(name_of_file16[k])) do
      incr(k);
    stop_at_space:=true;
    end_name;
    name_in_progress:=false;
    read_open[n]:=just_open;
    end;
@z

@x
@* \[50] Dumping and undumping the tables.
@y
@* \[50] Dumping and undumping the tables.
@z

@x
@!format_engine: ^text_char;
@y
@!format_engine: ^char;
@z

@x
@!format_engine: ^text_char;
@y
@!format_engine: ^char;
@z

@x
@!dummy_xord: ASCII_code;
@!dummy_xchr: text_char;
@!dummy_xprn: ASCII_code;
@y
@z

@x
format_engine:=xmalloc_array(text_char,x+4);
@y
format_engine:=xmalloc_array(char,x+4);
@z

@x
@<Dump |xord|, |xchr|, and |xprn|@>;
@y
@z

@x
format_engine:=xmalloc_array(text_char, x);
@y
format_engine:=xmalloc_array(char, x);
@z

@x
@<Undump |xord|, |xchr|, and |xprn|@>;
@y
@z

@x
dump_things(str_start[0], str_ptr+1);
@y
dump_things(str_start_macro(too_big_char), str_ptr+1-too_big_char);
@z

@x
undump_checked_things(0, pool_ptr, str_start[0], str_ptr+1);@/
@y
undump_checked_things(0, pool_ptr, str_start_macro(too_big_char), str_ptr+1-too_big_char);@/
@z

@x
  print_file_name(font_name[k],font_area[k],"");
@y
  if is_native_font(k) or (font_mapping[k]<>0) then
    begin print_file_name(font_name[k],"","");
    print_err("Can't \dump a format with native fonts or font-mappings");
    help3("You really, really don't want to do this.")
    ("It won't work, and only confuses me.")
    ("(Load them at runtime, not as part of the format file.)");
    error;
    end
  else print_file_name(font_name[k],font_area[k],"");
@z

@x
begin {Allocate the font arrays}
@y
begin {Allocate the font arrays}
font_mapping:=xmalloc_array(void_pointer, font_max);
font_layout_engine:=xmalloc_array(void_pointer, font_max);
font_flags:=xmalloc_array(char, font_max);
font_letter_space:=xmalloc_array(scaled, font_max);
@z

@x
font_bc:=xmalloc_array(eight_bits, font_max);
font_ec:=xmalloc_array(eight_bits, font_max);
@y
font_bc:=xmalloc_array(UTF16_code, font_max);
font_ec:=xmalloc_array(UTF16_code, font_max);
@z

@x
undump_things(font_check[null_font], font_ptr+1-null_font);
@y
for k:=null_font to font_ptr do font_mapping[k]:=0;
undump_things(font_check[null_font], font_ptr+1-null_font);
@z

@x
@* \[51] The main program.
@y
@* \[51] The main program.
@z

@x
  setup_bound_var (15000)('max_strings')(max_strings);
@y
  setup_bound_var (15000)('max_strings')(max_strings);
  max_strings:=max_strings+too_big_char; {the |max_strings| value doesn't include the 64K synthetic strings}
@z

@x
  buffer:=xmalloc_array (ASCII_code, buf_size);
@y
  buffer:=xmalloc_array (UnicodeScalar, buf_size);
@z

@x
  input_file:=xmalloc_array (alpha_file, max_in_open);
@y
  input_file:=xmalloc_array (unicode_file, max_in_open);
@z

@x [51.1332] l.24203 (ca.) texarray
  line_stack:=xmalloc_array (integer, max_in_open);
@y
  line_stack:=xmalloc_array (integer, max_in_open);
  eof_seen:=xmalloc_array (boolean, max_in_open);
  grp_stack:=xmalloc_array (save_pointer, max_in_open);
  if_stack:=xmalloc_array (pointer, max_in_open);
@z

@x
    print_file_name(0, log_name, 0); print_char(".");
@y
    print(log_name); print_char(".");
@z

@x [51.1337] l.24371 (ca.) texarray
  trie_root:=0; trie_c[0]:=si(0); trie_ptr:=0;
@y
  trie_root:=0; trie_c[0]:=si(0); trie_ptr:=0;
  hyph_root:=0; hyph_start:=0;
@z

@x
  {Allocate and initialize font arrays}
@y
  {Allocate and initialize font arrays}
  font_mapping:=xmalloc_array(void_pointer, font_max);
  font_layout_engine:=xmalloc_array(void_pointer, font_max);
  font_flags:=xmalloc_array(char, font_max);
  font_letter_space:=xmalloc_array(scaled, font_max);
@z

@x
  font_bc:=xmalloc_array(eight_bits, font_max);
  font_ec:=xmalloc_array(eight_bits, font_max);
@y
  font_bc:=xmalloc_array(UTF16_code, font_max);
  font_ec:=xmalloc_array(UTF16_code, font_max);
@z

@x
  param_base[null_font]:=-1;
@y
  font_mapping[null_font]:=0;
  param_base[null_font]:=-1;
@z

@x
@* \[53] Extensions.
@y
@* \[53] Extensions.
@z

% Revert ../tex.ch
% i and j are unused by TeX but required for XeTeX
@x [53.1348] (do_extension)
var k:integer; {all-purpose integers}
@y
var i,@!j,@!k:integer; {all-purpose integers}
@z

@x
    print(so(str_pool[str_start[str_ptr]+d])); {N.B.: not |print_char|}
@y
    print(so(str_pool[str_start_macro(str_ptr)+d])); {N.B.: not |print_char|}
@z

@x
        str_pool[str_start[str_ptr]+d]:=xchr[str_pool[str_start[str_ptr]+d]];
        if (str_pool[str_start[str_ptr]+d]=null_code)
@y
      if (str_pool[str_start_macro(str_ptr)+d]=null_code)
@z

@x convert from UTF-16 to UTF-8 for system(3).
      runsystem_ret := runsystem(conststringcast(addressof(
                                              str_pool[str_start[str_ptr]])));
@y
      if name_of_file then libc_free(name_of_file);
      name_of_file := xmalloc(cur_length * 3 + 2);
      k := 0;
      for d:=0 to cur_length-1 do
        append_to_name(str_pool[str_start_macro(str_ptr)+d]);
      name_of_file[k+1] := 0;
      runsystem_ret := runsystem(conststringcast(name_of_file+1));
@z

@x
  pool_ptr:=str_start[str_ptr];  {erase the string}
@y
  pool_ptr:=str_start_macro(str_ptr);  {erase the string}
@z

@x [53a.1379] l.??? -etex command line switch
@!init if (buffer[loc]="*")and(format_ident=" (INITEX)") then
@y
@!init if (etex_p or(buffer[loc]="*"))and(format_ident=" (INITEX)") then
@z

@x [53a.1379] l.??? -etex command line switch
  incr(loc); eTeX_mode:=1; {enter extended mode}
@y
  if (buffer[loc]="*") then incr(loc);
  eTeX_mode:=1; {enter extended mode}
@z

@x [53a.1383] l.??? -etex command line switch
@!eTeX_mode: 0..1; {identifies compatibility and extended mode}
@y
@!eTeX_mode: 0..1; {identifies compatibility and extended mode}
@!etex_p: boolean; {was the -etex option specified}
@z

@x [53a.1391] l.??? texarray
@!eof_seen : array[1..max_in_open] of boolean; {has eof been seen?}
@y
@!eof_seen : ^boolean; {has eof been seen?}
@z

@x [53a.1506] l.??? texarray
@!grp_stack : array[0..max_in_open] of save_pointer; {initial |cur_boundary|}
@!if_stack : array[0..max_in_open] of pointer; {initial |cond_ptr|}
@y
@!grp_stack : ^save_pointer; {initial |cur_boundary|}
@!if_stack : ^pointer; {initial |cond_ptr|}
@z

@x
      for c := str_start[text(h)] to str_start[text(h) + 1] - 1
@y
      for c := str_start_macro(text(h)) to str_start_macro(text(h) + 1) - 1
@z

@x
@ Dumping the |xord|, |xchr|, and |xprn| arrays.  We dump these always
in the format, so a TCX file loaded during format creation can set a
default for users of the format.

@<Dump |xord|, |xchr|, and |xprn|@>=
dump_things(xord[0], 256);
dump_things(xchr[0], 256);
dump_things(xprn[0], 256);

@ Undumping the |xord|, |xchr|, and |xprn| arrays.  This code is more
complicated, because we want to ensure that a TCX file specified on
the command line will override whatever is in the format.  Since the
tcx file has already been loaded, that implies throwing away the data
in the format.  Also, if no |translate_filename| is given, but
|eight_bit_p| is set we have to make all characters printable.

@<Undump |xord|, |xchr|, and |xprn|@>=
if translate_filename then begin
  for k:=0 to 255 do undump_things(dummy_xord, 1);
  for k:=0 to 255 do undump_things(dummy_xchr, 1);
  for k:=0 to 255 do undump_things(dummy_xprn, 1);
  end
else begin
  undump_things(xord[0], 256);
  undump_things(xchr[0], 256);
  undump_things(xprn[0], 256);
  if eight_bit_p then
    for k:=0 to 255 do
      xprn[k]:=1;
end;


@y
@z

@x
  while s>255 do  {first 256 strings depend on implementation!!}
@y
  while s>65535 do  {first 64K strings don't really exist in the pool!}
@z

@x
@!mltex_enabled_p:boolean;  {enable character substitution}
@y
@!mltex_enabled_p:boolean;  {enable character substitution}
@!native_font_type_flag:integer; {used by XeTeX font loading code to record which font technology was used}
@!xtx_ligature_present:boolean; {to suppress tfm font mapping of char codes from ligature nodes (already mapped)}
@z

@x [54/web2c.???] l.??? needed earlier
replacement, but always existing character |font_bc[f]|.
@^inner loop@>

@<Declare additional functions for ML\TeX@>=
function effective_char(@!err_p:boolean;
@y
replacement, but always existing character |font_bc[f]|.
@^inner loop@>

@<Declare \eTeX\ procedures for sc...@>=
function effective_char(@!err_p:boolean;
@z

@x
begin result:=c;  {return |c| unless it does not exist in the font}
@y
begin if (not xtx_ligature_present) and (font_mapping[f]<>nil) then
  c:=apply_tfm_font_mapping(font_mapping[f],c);
xtx_ligature_present:=false;
result:=c;  {return |c| unless it does not exist in the font}
@z

@x
begin if not mltex_enabled_p then
@y
begin if (not xtx_ligature_present) and (font_mapping[f]<>nil) then
  c:=apply_tfm_font_mapping(font_mapping[f],c);
xtx_ligature_present:=false;
if not mltex_enabled_p then
@z

@x
effective_char_info:=null_character;
exit:end;
@y
effective_char_info:=null_character;
exit:end;

{ the following procedure has been moved so that |new_native_character| can call it }

procedure char_warning(@!f:internal_font_number;@!c:integer);
var old_setting: integer; {saved value of |tracing_online|}
begin if tracing_lost_chars>0 then
 begin old_setting:=tracing_online;
 if eTeX_ex and(tracing_lost_chars>1) then tracing_online:=1;
  begin begin_diagnostic;
  print_nl("Missing character: There is no ");
@.Missing character@>
  if c < @"10000 then print_ASCII(c)
  else begin { non-Plane 0 Unicodes can't be sent through |print_ASCII| }
    print("character number ");
    print_hex(c);
  end;
  print(" in font ");
  slow_print(font_name[f]); print_char("!"); end_diagnostic(false);
  end;
 tracing_online:=old_setting;
 end;
end;

{ additional functions for native font support }

function new_native_word_node(@!f:internal_font_number;@!n:integer):pointer;
	{ note that this function creates the node, but does not actually set its metrics;
		call |set_native_metrics(node)| if that is required! }
var
	l:	integer;
	q:	pointer;
begin
	l := native_node_size + (n * sizeof(UTF16_code) + sizeof(memory_word) - 1) div sizeof(memory_word);

	q := get_node(l);
	type(q) := whatsit_node;
	subtype(q) := native_word_node;

	native_size(q) := l;
	native_font(q) := f;
	native_length(q) := n;

	native_glyph_count(q) := 0;
	native_glyph_info_ptr(q) := null_ptr;

	new_native_word_node := q;
end;

function new_native_character(@!f:internal_font_number;@!c:UnicodeScalar):pointer;
var
	p:	pointer;
	i, len: integer;
begin
	if font_mapping[f] <> 0 then begin
		if c > @"FFFF then begin
			str_room(2);
			append_char((c - @"10000) div 1024 + @"D800);
			append_char((c - @"10000) mod 1024 + @"DC00);
		end
		else begin
			str_room(1);
			append_char(c);
		end;
		len := apply_mapping(font_mapping[f], addressof(str_pool[str_start_macro(str_ptr)]), cur_length);
		pool_ptr := str_start_macro(str_ptr); { flush the string, as we'll be using the mapped text instead }

		i := 0;
		while i < len do begin
			if (mapped_text[i] >= @"D800) and (mapped_text[i] < @"DC00) then begin
				c := (mapped_text[i] - @"D800) * 1024 + mapped_text[i+1] - @"DC00 + @"10000;
				if map_char_to_glyph(f, c) = 0 then begin
					char_warning(f, c);
				end;
				i := i + 2;
			end
			else begin
				if map_char_to_glyph(f, mapped_text[i]) = 0 then begin
					char_warning(f, mapped_text[i]);
				end;
				i := i + 1;
			end;
		end;

		p := new_native_word_node(f, len);
		for i := 0 to len-1 do begin
			set_native_char(p, i, mapped_text[i]);
		end
	end
	else begin
		if tracing_lost_chars > 0 then
			if map_char_to_glyph(f, c) = 0 then begin
				char_warning(f, c);
			end;

		p := get_node(native_node_size + 1);
		type(p) := whatsit_node;
		subtype(p) := native_word_node;

		native_size(p) := native_node_size + 1;
		native_glyph_count(p) := 0;
		native_glyph_info_ptr(p) := null_ptr;
		native_font(p) := f;

		if c > @"FFFF then begin
			native_length(p) := 2;
			set_native_char(p, 0, (c - @"10000) div 1024 + @"D800);
			set_native_char(p, 1, (c - @"10000) mod 1024 + @"DC00);
		end
		else begin
			native_length(p) := 1;
			set_native_char(p, 0, c);
		end;
	end;

	set_native_metrics(p, XeTeX_use_glyph_metrics);

	new_native_character := p;
end;

procedure font_feature_warning(featureNameP:void_pointer; featLen:integer;
	settingNameP:void_pointer; setLen:integer);
var
	i: integer;
begin
	begin_diagnostic;
	print_nl("Unknown ");
	if setLen > 0 then begin
		print("selector `");
		print_utf8_str(settingNameP, setLen);
		print("' for ");
	end;
	print("feature `");
	print_utf8_str(featureNameP, featLen);
	print("' in font `");
	i := 1;
	while ord(name_of_file[i]) <> 0 do begin
		print_visible_char(name_of_file[i]); { this is already UTF-8 }
		incr(i);
	end;
	print("'.");
	end_diagnostic(false);
end;

procedure font_mapping_warning(mappingNameP:void_pointer;
	mappingNameLen:integer;
	warningType:integer); { 0: just logging; 1: file not found; 2: can't load }
var
	i: integer;
begin
	begin_diagnostic;
	if warningType=0 then print_nl("Loaded mapping `")
	else print_nl("Font mapping `");
	print_utf8_str(mappingNameP, mappingNameLen);
	print("' for font `");
	i := 1;
	while ord(name_of_file[i]) <> 0 do begin
		print_visible_char(name_of_file[i]); { this is already UTF-8 }
		incr(i);
	end;
	case warningType of
		1: print("' not found.");
		2: begin print("' not usable;");
		     print_nl("bad mapping file or incorrect mapping type.");
		   end;
		othercases print("'.")
	endcases;
	end_diagnostic(false);
end;

procedure graphite_warning;
var
	i: integer;
begin
	begin_diagnostic;
	print_nl("Font `");
	i := 1;
	while ord(name_of_file[i]) <> 0 do begin
		print_visible_char(name_of_file[i]); { this is already UTF-8 }
		incr(i);
	end;
	print("' does not support Graphite. Trying ICU layout instead.");
	end_diagnostic(false);
end;

function load_native_font(u: pointer; nom, aire:str_number; s: scaled): internal_font_number;
label
	done;
const
	first_math_fontdimen = 10;
var
	k, num_font_dimens: integer;
	font_engine: void_pointer;	{really an ATSUStyle or XeTeXLayoutEngine}
	actual_size: scaled;	{|s| converted to real size, if it was negative}
	p: pointer;	{for temporary |native_char| node we'll create}
	ascent, descent, font_slant, x_ht, cap_ht: scaled;
	f: internal_font_number;
	full_name: str_number;
begin
	{ on entry here, the full name is packed into |name_of_file| in UTF8 form }

	load_native_font := null_font;

	font_engine := find_native_font(name_of_file + 1, s);
	if font_engine = 0 then goto done;
	
	if s>=0 then
		actual_size := s
	else begin
		if (s <> -1000) then
			actual_size := xn_over_d(loaded_font_design_size,-s,1000)
		else
			actual_size := loaded_font_design_size;
	end;

	{ look again to see if the font is already loaded, now that we know its canonical name }
	str_room(name_length);
	for k := 1 to name_length do
		append_char(name_of_file[k]);
    full_name := make_string; { not |slow_make_string| because we'll flush it if the font was already loaded }

	for f:=font_base+1 to font_ptr do
  		if (font_area[f] = native_font_type_flag) and str_eq_str(font_name[f], full_name) and (font_size[f] = actual_size) then begin
  		    release_font_engine(font_engine, native_font_type_flag);
  			flush_string;
  		    load_native_font := f;
  		    goto done;
        end;

	if (native_font_type_flag = otgr_font_flag) and isOpenTypeMathFont(font_engine) then
		num_font_dimens := first_math_fontdimen + lastMathConstant
	else
		num_font_dimens := 8;

	if (font_ptr = font_max) or (fmem_ptr + num_font_dimens > font_mem_size) then begin
		@<Apologize for not loading the font, |goto done|@>;
	end;

	{ we've found a valid installed font, and have room }
	incr(font_ptr);
	font_area[font_ptr] := native_font_type_flag; { set by |find_native_font| to either |aat_font_flag| or |ot_font_flag| }

	{ store the canonical name }
	font_name[font_ptr] := full_name;

	font_check[font_ptr].b0 := 0;
	font_check[font_ptr].b1 := 0;
	font_check[font_ptr].b2 := 0;
	font_check[font_ptr].b3 := 0;
	font_glue[font_ptr] := null;
	font_dsize[font_ptr] := loaded_font_design_size;
	font_size[font_ptr] := actual_size;

	if (native_font_type_flag = aat_font_flag) then begin
		atsu_get_font_metrics(font_engine, addressof(ascent), addressof(descent),
								addressof(x_ht), addressof(cap_ht), addressof(font_slant))
	end else begin
		ot_get_font_metrics(font_engine, addressof(ascent), addressof(descent),
								addressof(x_ht), addressof(cap_ht), addressof(font_slant));
	end;

	height_base[font_ptr] := ascent;
	depth_base[font_ptr] := -descent;

	font_params[font_ptr] := num_font_dimens;	{ we add an extra fontdimen: \#8 -> |cap_height|;
													then OT math fonts have a bunch more }
	font_bc[font_ptr] := 0;
	font_ec[font_ptr] := 65535;
	font_used[font_ptr] := false;
	hyphen_char[font_ptr] := default_hyphen_char;
	skew_char[font_ptr] := default_skew_char;
	param_base[font_ptr] := fmem_ptr-1;

	font_layout_engine[font_ptr] := font_engine;
	font_mapping[font_ptr] := 0; { don't use the mapping, if any, when measuring space here }
	font_letter_space[font_ptr] := loaded_font_letter_space;

	{measure the width of the space character and set up font parameters}
	p := new_native_character(font_ptr, " ");
	s := width(p) + loaded_font_letter_space;
	free_node(p, native_size(p));

	font_info[fmem_ptr].sc := font_slant;							{|slant|}
	incr(fmem_ptr);
	font_info[fmem_ptr].sc := s;									{|space| = width of space character}
	incr(fmem_ptr);
	font_info[fmem_ptr].sc := s div 2;								{|space_stretch| = 1/2 * space}
	incr(fmem_ptr);
	font_info[fmem_ptr].sc := s div 3;								{|space_shrink| = 1/3 * space}
	incr(fmem_ptr);
	font_info[fmem_ptr].sc := x_ht;									{|x_height|}
	incr(fmem_ptr);
	font_info[fmem_ptr].sc := font_size[font_ptr];					{|quad| = font size}
	incr(fmem_ptr);
	font_info[fmem_ptr].sc := s div 3;								{|extra_space| = 1/3 * space}
	incr(fmem_ptr);
	font_info[fmem_ptr].sc := cap_ht;								{|cap_height|}
	incr(fmem_ptr);

	if num_font_dimens = first_math_fontdimen + lastMathConstant then begin
		font_info[fmem_ptr].int := num_font_dimens; { \fontdimen9 = number of assigned fontdimens }
		incr(fmem_ptr);
		for k := 0 to lastMathConstant do begin
			font_info[fmem_ptr].sc := get_ot_math_constant(font_ptr, k);
			incr(fmem_ptr);
		end;
	end;

	font_mapping[font_ptr] := loaded_font_mapping;
	font_flags[font_ptr] := loaded_font_flags;

	load_native_font := font_ptr;
done:
end;

procedure do_locale_linebreaks(s: integer; len: integer);
var
	offs, prevOffs, i: integer;
	use_penalty, use_skip: boolean;
begin
	if (XeTeX_linebreak_locale = 0) or (len = 1) then begin
		link(tail) := new_native_word_node(main_f, len);
		tail := link(tail);
		for i := 0 to len - 1 do
			set_native_char(tail, i, native_text[s + i]);
		set_native_metrics(tail, XeTeX_use_glyph_metrics);
	end else begin
		use_skip := XeTeX_linebreak_skip <> zero_glue;
		use_penalty := XeTeX_linebreak_penalty <> 0 or not use_skip;
		if (is_gr_font(main_f)) and (str_eq_str(XeTeX_linebreak_locale, "G")) then begin
			initGraphiteBreaking(font_layout_engine[main_f], native_text + s, len);
			offs := 0;
			repeat
				prevOffs := offs;
				offs := findNextGraphiteBreak(offs, 15); {klbWordBreak = 15}
				if offs > 0 then begin
					if prevOffs <> 0 then begin
						if use_penalty then
							tail_append(new_penalty(XeTeX_linebreak_penalty));
						if use_skip then
							tail_append(new_param_glue(XeTeX_linebreak_skip_code));
					end;
					link(tail) := new_native_word_node(main_f, offs - prevOffs);
					tail := link(tail);
					for i := prevOffs to offs - 1 do
						set_native_char(tail, i - prevOffs, native_text[s + i]);
					set_native_metrics(tail, XeTeX_use_glyph_metrics);
				end;
			until offs < 0;
		end
		else begin
			linebreak_start(XeTeX_linebreak_locale, native_text + s, len);
			offs := 0;
			repeat
				prevOffs := offs;
				offs := linebreak_next;
				if offs > 0 then begin
					if prevOffs <> 0 then begin
						if use_penalty then
							tail_append(new_penalty(XeTeX_linebreak_penalty));
						if use_skip then
							tail_append(new_param_glue(XeTeX_linebreak_skip_code));
					end;
					link(tail) := new_native_word_node(main_f, offs - prevOffs);
					tail := link(tail);
					for i := prevOffs to offs - 1 do
						set_native_char(tail, i - prevOffs, native_text[s + i]);
					set_native_metrics(tail, XeTeX_use_glyph_metrics);
				end;
			until offs < 0;
		end
	end
end;

procedure bad_utf8_warning;
begin
	begin_diagnostic;
	print_nl("Invalid UTF-8 byte or sequence");
	if terminal_input then print(" in terminal input")
	else begin
		print(" at line ");
		print_int(line);
	end;
	print(" replaced by U+FFFD.");
	end_diagnostic(false);
end;

function get_input_normalization_state: integer;
begin
	if eqtb=nil then get_input_normalization_state:=0 { may be called before eqtb is initialized }
	else get_input_normalization_state:=XeTeX_input_normalization_state;
end;

function get_tracing_fonts_state: integer;
begin
	get_tracing_fonts_state:=XeTeX_tracing_fonts_state;
end;

@z
