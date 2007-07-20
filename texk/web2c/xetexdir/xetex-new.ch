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

% Changes for XeTeX
% =================
%
% Procedure to build xetex from web sources:
%
% (1) build etex-web2c.web:
%	./tie -m etex.web ../../../../TeX/texk/web2c/tex.web ../../../../TeX/texk/web2c/etexdir/etex.ch ../../../../TeX/texk/web2c/etexdir/etex.fix
%
% (2) add xetex features, and remove enctex ones
%	./tie -m xetex.web etex.web xetex-new.ch xetex-noenc.ch
%
% (4) use otangle, web2c, etc....
%	./otangle xetex.web
%	./web2c ........

@x
@* \[1] Introduction.
@y
@* \[1] Introduction.
@z

@x
@d eTeX_version_string=='-2.2' {current \eTeX\ version}
@y
@d eTeX_version_string=='-2.2' {current \eTeX\ version}

@d XeTeX_version=0
@d XeTeX_revision==".996"
@d XeTeX_version_string=='-0.996' {current \XeTeX\ version}
@z

@x
@d eTeX_banner_k=='This is e-TeXk, Version 3.141592',eTeX_version_string
@d eTeX_banner=='This is e-TeX, Version 3.141592',eTeX_version_string
  {printed when \eTeX\ starts}
@#
@d TeX_banner_k=='This is TeXk, Version 3.141592' {printed when \TeX\ starts}
@d TeX_banner=='This is TeX, Version 3.141592' {printed when \TeX\ starts}
@#
@d banner==eTeX_banner
@d banner_k==eTeX_banner_k
@y
@d XeTeX_banner=='This is XeTeX, Version 3.141592',eTeX_version_string,XeTeX_version_string
  {printed when \XeTeX\ starts}
@d XeTeX_banner_k=='This is XeTeXk, Version 3.141592',eTeX_version_string,XeTeX_version_string
@#
@d banner==XeTeX_banner
@d banner_k==XeTeX_banner_k
@z

@x
@d TEX==ETEX {change program name into |ETEX|}
@y
@d TEX==XETEX {change program name into |XETEX|}
@z

@x
@d TeXXeT_code=0 {the \TeXXeT\ feature is optional}
@#
@d eTeX_states=1 {number of \eTeX\ state variables in |eqtb|}
@y
@d TeXXeT_code=0 {the \TeXXeT\ feature is optional}
@#
@d XeTeX_dash_break_code			= 1 {non-zero to enable breaks after en- and em-dashes}
@#
@d XeTeX_default_input_mode_code    = 2 {input mode for newly opened files}
@d XeTeX_input_mode_auto    = 0
@d XeTeX_input_mode_utf8    = 1
@d XeTeX_input_mode_utf16be = 2
@d XeTeX_input_mode_utf16le = 3
@d XeTeX_input_mode_raw     = 4
@d XeTeX_input_mode_icu_mapping = 5
@#
@d XeTeX_default_input_encoding_code = 3 {str_number of encoding name if mode = ICU}
@#
@d eTeX_states=4 {number of \eTeX\ state variables in |eqtb|}
@z

@x
@d hyph_prime=607 {another prime for hashing \.{\\hyphenation} exceptions;
                if you change this, you should also change |iinf_hyphen_size|.}
@y
@d hyph_prime=607 {another prime for hashing \.{\\hyphenation} exceptions;
                if you change this, you should also change |iinf_hyphen_size|.}

@d biggest_char=65535 {the largest allowed character number;
   must be |<=max_quarterword|}
@d too_big_char=65536 {|biggest_char+1|}
@d special_char=65537 {|biggest_char+2|}
@d number_chars=65536 {|biggest_char+1|}
@d biggest_reg=255 {the largest allowed register number;
   must be |<=max_quarterword|}
@d number_regs=256 {|biggest_reg+1|}
@d font_biggest=255 {the real biggest font}
@d number_fonts=font_biggest-font_base+2
@d number_math_families=256
@d number_math_fonts=number_math_families+number_math_families+number_math_families
@d math_font_biggest=number_math_fonts-1
@d text_size=0 {size code for the largest size in a family}
@d script_size=number_math_families {size code for the medium size in a family}
@d script_script_size=number_math_families+number_math_families {size code for the smallest size in a family}
@d biggest_lang=255
@d too_big_lang=256
@z

@x
@* \[2] The character set.
@y
@* \[2] The character set.
@z

@x
@ Characters of text that have been converted to \TeX's internal form
are said to be of type |ASCII_code|, which is a subrange of the integers.

@<Types...@>=
@!ASCII_code=0..255; {eight-bit numbers}
@y
@ Characters of text that have been converted to \TeX's internal form
are said to be of type |ASCII_code|, which is a subrange of the integers.
For xetex, we rename |ASCII_code| as |UTF16_code|. But we also have a
new type |UTF8_code|, used when we construct filenames to pass to the
system libraries.

@d ASCII_code==UTF16_code
@d packed_ASCII_code==packed_UTF16_code

@<Types...@>=
@!ASCII_code=0..biggest_char; {16-bit numbers}
@!UTF8_code=0..255; {8-bit numbers}
@!UnicodeScalar=0..@"10FFFF; {Unicode scalars}
@z

@x
@d last_text_char=255 {ordinal number of the largest element of |text_char|}
@y
@d last_text_char=biggest_char {ordinal number of the largest element of |text_char|}
@z

@x
@* \[3] Input and output.
@y
@* \[3] Input and output.
@z

@x
@!name_of_file:^text_char;
@!name_length:0..file_name_size;@/{this many characters are actually
  relevant in |name_of_file| (the rest are blank)}
@y
@!name_of_file:^UTF8_code; {we build filenames in utf8 to pass to the OS}
@!name_of_file16:^UTF16_code; {but sometimes we need a utf16 version of the name}
@!name_length:0..file_name_size;@/{this many characters are actually
  relevant in |name_of_file| (the rest are blank)}
@!name_length16:0..file_name_size;
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
@* \[4] String handling.
@y
@* \[4] String handling.
@z

@x
|str_start[s]<=j<str_start[s+1]|. Additional integer variables
@y
|str_start_macro[s]<=j<str_start_macro[s+1]|. Additional integer variables
@z

@x
|str_pool[pool_ptr]| and |str_start[str_ptr]| are
@y
|str_pool[pool_ptr]| and |str_start_macro[str_ptr]| are
@z

@x
@d si(#) == # {convert from |ASCII_code| to |packed_ASCII_code|}
@d so(#) == # {convert from |packed_ASCII_code| to |ASCII_code|}
@y
@d si(#) == # {convert from |ASCII_code| to |packed_ASCII_code|}
@d so(#) == # {convert from |packed_ASCII_code| to |ASCII_code|}
@d str_start_macro(#) == str_start[(#) - too_big_char]
@z

@x
@!packed_ASCII_code = 0..255; {elements of |str_pool| array}
@y
@!packed_ASCII_code = 0..65535; {elements of |str_pool| array}
@z

@x
@d length(#)==(str_start[#+1]-str_start[#]) {the number of characters
  in string number \#}
@y
@p function length(s:str_number):integer;
   {the number of characters in string number |s|}
begin if (s>=@"10000) then length:=str_start_macro(s+1)-str_start_macro(s)
else if (s>=@"20) and (s<@"7F) then length:=1
else if (s<=@"7F) then length:=3
else if (s<@"100) then length:=4
else length:=8
end;
@z

@x
@d cur_length == (pool_ptr - str_start[str_ptr])
@y
@d cur_length == (pool_ptr - str_start_macro(str_ptr))
@z

@x
incr(str_ptr); str_start[str_ptr]:=pool_ptr;
@y
incr(str_ptr); str_start_macro(str_ptr):=pool_ptr;
@z

@x
@d flush_string==begin decr(str_ptr); pool_ptr:=str_start[str_ptr];
@y
@d flush_string==begin decr(str_ptr); pool_ptr:=str_start_macro(str_ptr);
@z

@x
begin j:=str_start[s];
while j<str_start[s+1] do
@y
begin j:=str_start_macro(s);
while j<str_start_macro(s+1) do
@z

@x
j:=str_start[s]; k:=str_start[t];
while j<str_start[s+1] do
  begin if str_pool[j]<>str_pool[k] then goto not_found;
  incr(j); incr(k);
@y
if (length(s)=1) then begin
  if s<65536 then begin
    if t<65536 then begin
      if s<>t then goto not_found;
      end
    else begin
      if s<>str_pool[str_start_macro(t)] then goto not_found;
      end;
    end
  else begin
    if t<65536 then begin
      if str_pool[str_start_macro(s)]<>t then goto not_found;
      end
    else begin
      if str_pool[str_start_macro(s)]<>str_pool[str_start_macro(t)] then
        goto not_found;
      end;
    end;
  end
else begin
  j:=str_start_macro(s); k:=str_start_macro(t);
  while j<str_start_macro(s+1) do
    begin if str_pool[j]<>str_pool[k] then goto not_found;
    incr(j); incr(k);
    end;
@z

@x
begin pool_ptr:=0; str_ptr:=0; str_start[0]:=0;
@y
begin pool_ptr:=0; str_ptr:=0;
@z

@x
@ @d app_lc_hex(#)==l:=#;
  if l<10 then append_char(l+"0")@+else append_char(l-10+"a")
@y
@ The first 65536 strings will consist of a single character only.
But we don't actually make them; they're simulated on the fly.
@z

@x
for k:=0 to 255 do
  begin if (@<Character |k| cannot be printed@>) then
    begin append_char("^"); append_char("^");
    if k<@'100 then append_char(k+@'100)
    else if k<@'200 then append_char(k-@'100)
    else begin app_lc_hex(k div 16); app_lc_hex(k mod 16);
      end;
    end
  else append_char(k);
  g:=make_string;
  end
@y
begin
str_ptr:=too_big_char;
str_start_macro(str_ptr):=pool_ptr;
end
@z

@x
@<Character |k| cannot be printed@>=
  (k<" ")or(k>"~")
@y
@<Character |k| cannot be printed@>=
{ this module is not used }
@z

@x
name_of_file := xmalloc_array (ASCII_code, name_length + 1);
@y
name_of_file := xmalloc_array (UTF8_code, name_length + 1);
@z

@x
else  begin if (xord[m]<"0")or(xord[m]>"9")or@|
      (xord[n]<"0")or(xord[n]>"9") then
@y
else  begin if (m<"0")or(m>"9")or@|
      (n<"0")or(n>"9") then
@z

@x
  l:=xord[m]*10+xord[n]-"0"*11; {compute the length}
@y
  l:=m*10+n-"0"*11; {compute the length}
@z

@x
    append_char(xord[m]);
@y
    append_char(m);
@z

@x
loop@+  begin if (xord[n]<"0")or(xord[n]>"9") then
@y
loop@+  begin if (n<"0")or(n>"9") then
@z

@x
  a:=10*a+xord[n]-"0";
@y
  a:=10*a+n-"0";
@z

@x
@* \[5] On-line and off-line printing.
@y
@* \[5] On-line and off-line printing.
@z

@x
procedure print_char(@!s:ASCII_code); {prints a single character}
label exit;
begin if @<Character |s| is the current new-line character@> then
 if selector<pseudo then
  begin print_ln; return;
  end;
case selector of
term_and_log: begin wterm(xchr[s]); wlog(xchr[s]);
  incr(term_offset); incr(file_offset);
  if term_offset=max_print_line then
    begin wterm_cr; term_offset:=0;
    end;
  if file_offset=max_print_line then
    begin wlog_cr; file_offset:=0;
    end;
  end;
log_only: begin wlog(xchr[s]); incr(file_offset);
  if file_offset=max_print_line then print_ln;
  end;
term_only: begin wterm(xchr[s]); incr(term_offset);
  if term_offset=max_print_line then print_ln;
  end;
no_print: do_nothing;
pseudo: if tally<trick_count then trick_buf[tally mod error_line]:=s;
new_string: begin if pool_ptr<pool_size then append_char(s);
  end; {we drop characters if the string space is full}
othercases write(write_file[selector],xchr[s])
endcases;@/
incr(tally);
exit:end;
@y
procedure print_visible_char(@!s:ASCII_code); {prints a single character}
label exit; {label is not used but nonetheless kept (for other changes?)}
begin
case selector of
term_and_log: begin wterm(xchr[s]); wlog(xchr[s]);
  incr(term_offset); incr(file_offset);
  if term_offset=max_print_line then
    begin wterm_cr; term_offset:=0;
    end;
  if file_offset=max_print_line then
    begin wlog_cr; file_offset:=0;
    end;
  end;
log_only: begin wlog(xchr[s]); incr(file_offset);
  if file_offset=max_print_line then print_ln;
  end;
term_only: begin wterm(xchr[s]); incr(term_offset);
  if term_offset=max_print_line then print_ln;
  end;
no_print: do_nothing;
pseudo: if tally<trick_count then trick_buf[tally mod error_line]:=s;
new_string: begin if pool_ptr<pool_size then append_char(s);
  end; {we drop characters if the string space is full}
othercases write(write_file[selector],xchr[s])
endcases;@/
incr(tally);
exit:end;

@ The |print_char| procedure sends one character to the desired destination.
Control sequence names, file names and string constructed with
\.{\\string} might contain |ASCII_code| values that can't
be printed using |print_visible_char|.  These characters will be printed
in three- or four-symbol form like `\.{\^\^A}' or `\.{\^\^e4}',
unless the -8bit option is enabled.
Output that goes to the terminal and/or log file is treated differently
when it comes to determining whether a character is printable.

@d print_lc_hex(#)==l:=#;
  if l<10 then print_visible_char(l+"0")@+else print_visible_char(l-10+"a")

@<Basic printing...@>=
procedure print_char(@!s:ASCII_code); {prints a single character}
label exit;
var l: small_number;
begin if (selector>pseudo) and (not doing_special) then {"printing" to a new string, don't encode chars}
  begin print_visible_char(s); return;
  end;
if @<Character |s| is the current new-line character@> then
 if selector<pseudo then
  begin print_ln; return;
  end;
if (s < 32) and (eight_bit_p = 0) and (not doing_special) then begin
	{ control char: ^^X }
	print_visible_char("^"); print_visible_char("^"); print_visible_char(s+64);
end else if s < 127 then
	{ printable ASCII }
	print_visible_char(s)
else if (s = 127) then begin
	{ DEL }
	if (eight_bit_p = 0) and (not doing_special) then begin
		print_visible_char("^"); print_visible_char("^"); print_visible_char("?")
	end else
		print_visible_char(s)
end else if (s < @"A0) and (eight_bit_p = 0) and (not doing_special) then begin { C1 controls: ^^xx }
	print_visible_char("^"); print_visible_char("^");
	print_lc_hex((s mod @"100) div @"10); print_lc_hex(s mod @"10);
end else begin
	{ char >= 128: encode as UTF8 }
	if s<@"800 then begin
		print_visible_char(@"C0 + s div @"40);
		print_visible_char(@"80 + s mod @"40);
	end
	else begin
		print_visible_char(@"E0 + s div @"1000);
		print_visible_char(@"80 + (s mod @"1000) div @"40);
		print_visible_char(@"80 + (s mod @"1000) mod @"40);
	end
end;
exit:end;

@ @<Glob...@>=
doing_special: boolean;

@ @<Set init...@>=
doing_special:=false;
@z

@x
procedure print(@!s:integer); {prints string |s|}
label exit;
var j:pool_pointer; {current character code position}
@!nl:integer; {new-line character to restore}
begin if s>=str_ptr then s:="???" {this can't happen}
@.???@>
else if s<256 then
  if s<0 then s:="???" {can't happen}
  else begin if (selector>pseudo) and (not special_printing)
                 and (not message_printing) then
      begin print_char(s); return; {internal strings are not expanded}
      end;
    if (@<Character |s| is the current new-line character@>) then
      if selector<pseudo then
        begin print_ln; no_convert := false; return;
        end
      else if message_printing then
        begin print_char(s); no_convert := false; return;
        end;
    if (mubyte_log>0) and (not no_convert) and (mubyte_write[s]>0) then
      s := mubyte_write[s]
    else if xprn[s] or special_printing then
      begin print_char(s); no_convert := false; return; end;
    no_convert := false;
    nl:=new_line_char; new_line_char:=-1;
      {temporarily disable new-line character}
    j:=str_start[s];
    while j<str_start[s+1] do
      begin print_char(so(str_pool[j])); incr(j);
      end;
    new_line_char:=nl; return;
    end;
j:=str_start[s];
while j<str_start[s+1] do
  begin print_char(so(str_pool[j])); incr(j);
  end;
exit:end;
@y
procedure print(@!s:integer); {prints string |s|}
label exit;
var j:pool_pointer; {current character code position}
@!nl:integer; {new-line character to restore}
begin if s>=str_ptr then s:="???" {this can't happen}
@.???@>
else if s<biggest_char then
  if s<0 then s:="???" {can't happen}
  else begin if selector>pseudo then
      begin print_char(s); return; {internal strings are not expanded}
      end;
    if (@<Character |s| is the current new-line character@>) then
      if selector<pseudo then
        begin print_ln; return;
        end;
    nl:=new_line_char;
    new_line_char:=-1;
    print_char(s);
    new_line_char:=nl;
    return;
    end;
j:=str_start_macro(s);
while j<str_start_macro(s+1) do
  begin print_char(so(str_pool[j])); incr(j);
  end;
exit:end;
@z

@x
@ Control sequence names, file names, and strings constructed with
\.{\\string} might contain |ASCII_code| values that can't
be printed using |print_char|. Therefore we use |slow_print| for them:

@<Basic print...@>=
procedure slow_print(@!s:integer); {prints string |s|}
var j:pool_pointer; {current character code position}
begin if (s>=str_ptr) or (s<256) then print(s)
else begin j:=str_start[s];
  while j<str_start[s+1] do
    begin print(so(str_pool[j])); incr(j);
    end;
  end;
end;
@y
@ Old versions of \TeX\ needed a procedure called |slow_print| whose function
is now subsumed by |print| and the new functionality of |print_char| and
|print_visible_char|.  We retain the old name |slow_print| here as a
possible aid to future software arch\ae ologists.

@d slow_print == print
@z

@x
begin  @<Set variable |c| to the current escape character@>;
if c>=0 then if c<256 then print(c);
@y
begin  @<Set variable |c| to the current escape character@>;
if c>=0 then if c<=biggest_char then print_char(c);
@z

@x
begin j:=str_start["m2d5c2l5x2v5i"]; v:=1000;
@y
begin j:=str_start_macro("m2d5c2l5x2v5i"); v:=1000;
@z

@x
@p procedure print_current_string; {prints a yet-unmade string}
var j:pool_pointer; {points to current character code}
begin j:=str_start[str_ptr];
@y
@p procedure print_current_string; {prints a yet-unmade string}
var j:pool_pointer; {points to current character code}
begin j:=str_start_macro(str_ptr);
@z

@x
k:=first; while k < last do begin print_buffer(k) end;
@y
if last<>first then for k:=first to last-1 do print(buffer[k]);
@z

@x
@* \[6] Reporting errors.
@y
@* \[6] Reporting errors.
@z

@x
    begin edit_name_start:=str_start[edit_file.name_field];
    edit_name_length:=str_start[edit_file.name_field+1] -
                      str_start[edit_file.name_field];
@y
    begin edit_name_start:=str_start_macro(edit_file.name_field);
    edit_name_length:=str_start_macro(edit_file.name_field+1) -
                      str_start_macro(edit_file.name_field);
@z

@x
@* \[7] Arithmetic with scaled dimensions.
@y
@* \[7] Arithmetic with scaled dimensions.
@z

@x
@* \[8] Packed data.
@y
@* \[8] Packed data.
@z

@x
@d min_quarterword=0 {smallest allowable value in a |quarterword|}
@d max_quarterword=255 {largest allowable value in a |quarterword|}
@d min_halfword==-@"FFFFFFF {smallest allowable value in a |halfword|}
@d max_halfword==@"FFFFFFF {largest allowable value in a |halfword|}
@y
@d min_quarterword=0 {smallest allowable value in a |quarterword|}
@d max_quarterword=@"FFFF {largest allowable value in a |quarterword|}
@d min_halfword==-@"FFFFFFF {smallest allowable value in a |halfword|}
@d max_halfword==@"3FFFFFFF {largest allowable value in a |halfword|}
@z

@x
if (min_quarterword>0)or(max_quarterword<127) then bad:=11;
if (min_halfword>0)or(max_halfword<32767) then bad:=12;
@y
if (min_quarterword>0)or(max_quarterword<@"7FFF) then bad:=11;
if (min_halfword>0)or(max_halfword<@"3FFFFFFF) then bad:=12;
@z

@x
if max_quarterword-min_quarterword<255 then bad:=19;
@y
if max_quarterword-min_quarterword<@"FFFF then bad:=19;
@z

@x
@* \[9] Dynamic memory allocation.
@y
@* \[9] Dynamic memory allocation.
@z

@x
@* \[10] Data structures for boxes and their friends.
@y
@* \[10] Data structures for boxes and their friends.
@z

@x
@d whatsit_node=8 {|type| of special extension nodes}
@y
@d whatsit_node=8 {|type| of special extension nodes}

{ added stuff here for native_word and picture nodes }
@d native_word_node=40 {|subtype| in whatsits that hold native_font words
	(0-3 are used for open, write, close, special; 4 is language; pdfTeX uses up through 30-something)

To support ``native'' fonts, we build |native_word_nodes|, which are variable size whatsits.
These have the same |width|, |depth|, and |height| fields as a |box_node|, at offsets 1-3,
and then a word containing a size field for the node, a font number, and a length.
Then there is a field containing two halfwords, a glyph count and a C pointer to a glyph info array;
these are set by |set_native_metrics|. Copying and freeing of these nodes needs to take account of this!
This is followed by |length| bytes, for the actual characters of the string.
(Yes, we count in bytes, even though what we store there is UTF-16.)

So |native_node_size|, which does not include any space for the actual text, is 6.}

@d deleted_native_node=41 {native words that have been superseded by their successors}

@d native_node_size=6 {size of a native_word node (plus the actual chars) -- see also xetex.h}
@d native_size(#)==mem[#+4].hh.b0
@d native_font(#)==mem[#+4].hh.b1
@d native_length(#)==mem[#+4].hh.rh
@d native_glyph_count(#)==mem[#+5].hh.lh
@d native_glyph_info_ptr(#)==mem[#+5].hh.rh
@d native_glyph_info_size=10 { number of bytes of info per glyph: 16-bit glyph ID, 32-bit x and y coords }

@d free_native_glyph_info(#) ==
  begin
    if native_glyph_info_ptr(#) <> 0 then begin
      libc_free(cast_to_ptr(native_glyph_info_ptr(#)));
      native_glyph_info_ptr(#) := 0;
      native_glyph_count(#) := 0;
    end
  end

@p procedure copy_native_glyph_info(src:pointer; dest:pointer);
var glyph_count:integer;
begin
  if native_glyph_info_ptr(src) <> 0 then begin
    glyph_count := native_glyph_count(src);
    native_glyph_info_ptr(dest) := cast_to_integer(xmalloc_array(char, glyph_count * native_glyph_info_size));
    memcpy(cast_to_ptr(native_glyph_info_ptr(dest)), cast_to_ptr(native_glyph_info_ptr(src)), glyph_count * native_glyph_info_size);
    native_glyph_count(dest) := glyph_count;
  end
end;

@ There are also |glyph_nodes|; these are like |native_word_nodes| in having |width|, |depth|, and |height| fields,
but then they contain a glyph ID rather than size and length fields, and there's no subsidiary C pointer.

@d glyph_node_size=5
@d native_glyph==native_length {in |glyph_node|s, we store the glyph number here}

@d pic_node=42 {|subtype| in whatsits that hold picture file references}
@d pdf_node=43 {|subtype| in whatsits that hold PDF page references}
@d glyph_node=44 {|subtype| in whatsits that hold glyph numbers}

@d pdfbox_crop = 1 { |pic_box_type| values in PDF nodes }
@d pdfbox_media = 2
@d pdfbox_bleed = 3
@d pdfbox_trim = 4
@d pdfbox_art = 5


{Picture files are handled with nodes that include fields for the transform associated
with the picture, and a pathname for the picture file itself.
They also have
the |width|, |depth|, and |height| fields of a |box_node| at offsets 1-3. (|depth| will
always be zero, as it happens.)

So |pic_node_size|, which does not include any space for the picture file pathname, is 7.

pdf_nodes are just like pic_nodes, but generate a different xdv file code.}

@d pic_node_size=8 { must sync with xetex.h }
@d pic_path_length(#)==mem[#+4].hh.b0
@d pic_page(#)==mem[#+4].hh.b1
@d pic_box_type(#)==mem[#+4].hh.rh { for PDF, unused in picfile }
@d pic_transform1(#)==mem[#+5].hh.lh
@d pic_transform2(#)==mem[#+5].hh.rh
@d pic_transform3(#)==mem[#+6].hh.lh
@d pic_transform4(#)==mem[#+6].hh.rh
@d pic_transform5(#)==mem[#+7].hh.lh
@d pic_transform6(#)==mem[#+7].hh.rh
@z

@x
@* \[11] Memory layout.
@y
@* \[11] Memory layout.
@z

@x
@* \[12] Displaying boxes.
@y
@* \[12] Displaying boxes.
@z

@x
@ @<Print a short indication of the contents of node |p|@>=
case type(p) of
hlist_node,vlist_node,ins_node,whatsit_node,mark_node,adjust_node,
  unset_node: print("[]");
@y
@ @<Print a short indication of the contents of node |p|@>=
case type(p) of
hlist_node,vlist_node,ins_node,mark_node,adjust_node,
  unset_node: print("[]");
whatsit_node: if subtype(p)=native_word_node then begin
	if native_font(p)<>font_in_short_display then begin
		print_esc(font_id_text(native_font(p)));
		print_char(" ");
		font_in_short_display:=native_font(p);
	end;
	print_native_word(p);
end else
	print("[]");
@z

@x
@p procedure show_node_list(@!p:integer); {prints a node list symbolically}
label exit;
var n:integer; {the number of items already printed at this level}
@y
@p procedure show_node_list(@!p:integer); {prints a node list symbolically}
label exit;
var n:integer; {the number of items already printed at this level}
i:integer; {temp index for printing chars of picfile paths}
@z

@x
@* \[15] The command codes.
@y
@* \[15] The command codes.
@z

@x
@d math_given=69 {math code defined by \.{\\mathchardef}}
@d last_item=70 {most recent item ( \.{\\lastpenalty},
  \.{\\lastkern}, \.{\\lastskip} )}
@d max_non_prefixed_command=70 {largest command code that can't be \.{\\global}}

@ The next codes are special; they all relate to mode-independent
assignment of values to \TeX's internal registers or tables.
Codes that are |max_internal| or less represent internal quantities
that might be expanded by `\.{\\the}'.

@d toks_register=71 {token list register ( \.{\\toks} )}
@d assign_toks=72 {special token list ( \.{\\output}, \.{\\everypar}, etc.~)}
@d assign_int=73 {user-defined integer ( \.{\\tolerance}, \.{\\day}, etc.~)}
@d assign_dimen=74 {user-defined length ( \.{\\hsize}, etc.~)}
@d assign_glue=75 {user-defined glue ( \.{\\baselineskip}, etc.~)}
@d assign_mu_glue=76 {user-defined muglue ( \.{\\thinmuskip}, etc.~)}
@d assign_font_dimen=77 {user-defined font dimension ( \.{\\fontdimen} )}
@d assign_font_int=78 {user-defined font integer ( \.{\\hyphenchar},
  \.{\\skewchar} )}
@d set_aux=79 {specify state info ( \.{\\spacefactor}, \.{\\prevdepth} )}
@d set_prev_graf=80 {specify state info ( \.{\\prevgraf} )}
@d set_page_dimen=81 {specify state info ( \.{\\pagegoal}, etc.~)}
@d set_page_int=82 {specify state info ( \.{\\deadcycles},
  \.{\\insertpenalties} )}
  {( or \.{\\interactionmode} )}
@d set_box_dimen=83 {change dimension of box ( \.{\\wd}, \.{\\ht}, \.{\\dp} )}
@d set_shape=84 {specify fancy paragraph shape ( \.{\\parshape} )}
  {(or \.{\\interlinepenalties}, etc.~)}
@d def_code=85 {define a character code ( \.{\\catcode}, etc.~)}
@d def_family=86 {declare math fonts ( \.{\\textfont}, etc.~)}
@d set_font=87 {set current font ( font identifiers )}
@d def_font=88 {define a font file ( \.{\\font} )}
@d register=89 {internal register ( \.{\\count}, \.{\\dimen}, etc.~)}
@d max_internal=89 {the largest code that can follow \.{\\the}}
@d advance=90 {advance a register or parameter ( \.{\\advance} )}
@d multiply=91 {multiply a register or parameter ( \.{\\multiply} )}
@d divide=92 {divide a register or parameter ( \.{\\divide} )}
@d prefix=93 {qualify a definition ( \.{\\global}, \.{\\long}, \.{\\outer} )}
  {( or \.{\\protected} )}
@d let=94 {assign a command code ( \.{\\let}, \.{\\futurelet} )}
@d shorthand_def=95 {code definition ( \.{\\chardef}, \.{\\countdef}, etc.~)}
  {or \.{\\charsubdef}}
@d read_to_cs=96 {read into a control sequence ( \.{\\read} )}
  {( or \.{\\readline} )}
@d def=97 {macro definition ( \.{\\def}, \.{\\gdef}, \.{\\xdef}, \.{\\edef} )}
@d set_box=98 {set a box ( \.{\\setbox} )}
@d hyph_data=99 {hyphenation data ( \.{\\hyphenation}, \.{\\patterns} )}
@d set_interaction=100 {define level of interaction ( \.{\\batchmode}, etc.~)}
@d max_command=100 {the largest command code seen at |big_switch|}
@y
@d math_given=69 {math code defined by \.{\\mathchardef}}
@d XeTeX_math_given=70
@d last_item=71 {most recent item ( \.{\\lastpenalty},
  \.{\\lastkern}, \.{\\lastskip} )}
@d max_non_prefixed_command=71 {largest command code that can't be \.{\\global}}

@ The next codes are special; they all relate to mode-independent
assignment of values to \TeX's internal registers or tables.
Codes that are |max_internal| or less represent internal quantities
that might be expanded by `\.{\\the}'.

@d toks_register=72 {token list register ( \.{\\toks} )}
@d assign_toks=73 {special token list ( \.{\\output}, \.{\\everypar}, etc.~)}
@d assign_int=74 {user-defined integer ( \.{\\tolerance}, \.{\\day}, etc.~)}
@d assign_dimen=75 {user-defined length ( \.{\\hsize}, etc.~)}
@d assign_glue=76 {user-defined glue ( \.{\\baselineskip}, etc.~)}
@d assign_mu_glue=77 {user-defined muglue ( \.{\\thinmuskip}, etc.~)}
@d assign_font_dimen=78 {user-defined font dimension ( \.{\\fontdimen} )}
@d assign_font_int=79 {user-defined font integer ( \.{\\hyphenchar},
  \.{\\skewchar} )}
@d set_aux=80 {specify state info ( \.{\\spacefactor}, \.{\\prevdepth} )}
@d set_prev_graf=81 {specify state info ( \.{\\prevgraf} )}
@d set_page_dimen=82 {specify state info ( \.{\\pagegoal}, etc.~)}
@d set_page_int=83 {specify state info ( \.{\\deadcycles},
  \.{\\insertpenalties} )}
  {( or \.{\\interactionmode} )}
@d set_box_dimen=84 {change dimension of box ( \.{\\wd}, \.{\\ht}, \.{\\dp} )}
@d set_shape=85 {specify fancy paragraph shape ( \.{\\parshape} )}
  {(or \.{\\interlinepenalties}, etc.~)}
@d def_code=86 {define a character code ( \.{\\catcode}, etc.~)}
@d XeTeX_def_code=87 {\.{\\XeTeXmathcode}, \.{\\XeTeXdelcode}}
@d def_family=88 {declare math fonts ( \.{\\textfont}, etc.~)}
@d set_font=89 {set current font ( font identifiers )}
@d def_font=90 {define a font file ( \.{\\font} )}
@d register=91 {internal register ( \.{\\count}, \.{\\dimen}, etc.~)}
@d max_internal=91 {the largest code that can follow \.{\\the}}
@d advance=92 {advance a register or parameter ( \.{\\advance} )}
@d multiply=93 {multiply a register or parameter ( \.{\\multiply} )}
@d divide=94 {divide a register or parameter ( \.{\\divide} )}
@d prefix=95 {qualify a definition ( \.{\\global}, \.{\\long}, \.{\\outer} )}
  {( or \.{\\protected} )}
@d let=96 {assign a command code ( \.{\\let}, \.{\\futurelet} )}
@d shorthand_def=97 {code definition ( \.{\\chardef}, \.{\\countdef}, etc.~)}
  {or \.{\\charsubdef}}
@d read_to_cs=98 {read into a control sequence ( \.{\\read} )}
  {( or \.{\\readline} )}
@d def=99 {macro definition ( \.{\\def}, \.{\\gdef}, \.{\\xdef}, \.{\\edef} )}
@d set_box=100 {set a box ( \.{\\setbox} )}
@d hyph_data=101 {hyphenation data ( \.{\\hyphenation}, \.{\\patterns} )}
@d set_interaction=102 {define level of interaction ( \.{\\batchmode}, etc.~)}
@d max_command=102 {the largest command code seen at |big_switch|}
@z

@x
@* \[17] The table of equivalents.
@y
@* \[17] The table of equivalents.
@z

@x
In the first region we have 256 equivalents for ``active characters'' that
act as control sequences, followed by 256 equivalents for single-character
control sequences.
@y
In the first region we have |number_chars| equivalents for ``active characters''
that act as control sequences, followed by |number_chars| equivalents for
single-character control sequences.
@z

@x
@d single_base=active_base+256 {equivalents of one-character control sequences}
@d null_cs=single_base+256 {equivalent of \.{\\csname\\endcsname}}
@y
@d single_base=active_base+number_chars
   {equivalents of one-character control sequences}
@d null_cs=single_base+number_chars {equivalent of \.{\\csname\\endcsname}}
@z

@x
@ Region 3 of |eqtb| contains the 256 \.{\\skip} registers, as well as the
glue parameters defined here. It is important that the ``muskip''
parameters have larger numbers than the others.
@y
@ Region 3 of |eqtb| contains the |number_regs| \.{\\skip} registers,
as well as the glue parameters defined here. It is important that the
``muskip'' parameters have larger numbers than the others.
@z

@x
@d par_fill_skip_code=14 {glue on last line of paragraph}
@d thin_mu_skip_code=15 {thin space in math formula}
@d med_mu_skip_code=16 {medium space in math formula}
@d thick_mu_skip_code=17 {thick space in math formula}
@d glue_pars=18 {total number of glue parameters}
@y
@d par_fill_skip_code=14 {glue on last line of paragraph}
@d XeTeX_linebreak_skip_code=15 {glue introduced at potential linebreak location}
@d thin_mu_skip_code=16 {thin space in math formula}
@d med_mu_skip_code=17 {medium space in math formula}
@d thick_mu_skip_code=18 {thick space in math formula}
@d glue_pars=19 {total number of glue parameters}
@z

@x
@d skip_base=glue_base+glue_pars {table of 256 ``skip'' registers}
@d mu_skip_base=skip_base+256 {table of 256 ``muskip'' registers}
@d local_base=mu_skip_base+256 {beginning of region 4}
@y
@d skip_base=glue_base+glue_pars {table of |number_regs| ``skip'' registers}
@d mu_skip_base=skip_base+number_regs
   {table of |number_regs| ``muskip'' registers}
@d local_base=mu_skip_base+number_regs {beginning of region 4}
@z

@x
@d par_fill_skip==glue_par(par_fill_skip_code)
@y
@d par_fill_skip==glue_par(par_fill_skip_code)
@d XeTeX_linebreak_skip==glue_par(XeTeX_linebreak_skip_code)
@z

@x
par_fill_skip_code: print_esc("parfillskip");
@y
par_fill_skip_code: print_esc("parfillskip");
XeTeX_linebreak_skip_code: print_esc("XeTeXlinebreakskip");
@z

@x
primitive("parfillskip",assign_glue,glue_base+par_fill_skip_code);@/
@!@:par_fill_skip_}{\.{\\parfillskip} primitive@>
@y
primitive("parfillskip",assign_glue,glue_base+par_fill_skip_code);@/
@!@:par_fill_skip_}{\.{\\parfillskip} primitive@>
primitive("XeTeXlinebreakskip",assign_glue,glue_base+XeTeX_linebreak_skip_code);@/
@z

@x
@d toks_base=etex_toks {table of 256 token list registers}
@#
@d etex_pen_base=toks_base+256 {start of table of \eTeX's penalties}
@d inter_line_penalties_loc=etex_pen_base {additional penalties between lines}
@d club_penalties_loc=etex_pen_base+1 {penalties for creating club lines}
@d widow_penalties_loc=etex_pen_base+2 {penalties for creating widow lines}
@d display_widow_penalties_loc=etex_pen_base+3 {ditto, just before a display}
@d etex_pens=etex_pen_base+4 {end of table of \eTeX's penalties}
@#
@d box_base=etex_pens {table of 256 box registers}
@d cur_font_loc=box_base+256 {internal font number outside math mode}
@d xord_code_base=cur_font_loc+1
@d xchr_code_base=xord_code_base+1
@d xprn_code_base=xchr_code_base+1
@d math_font_base=xprn_code_base+1
@d cat_code_base=math_font_base+48
  {table of 256 command codes (the ``catcodes'')}
@d lc_code_base=cat_code_base+256 {table of 256 lowercase mappings}
@d uc_code_base=lc_code_base+256 {table of 256 uppercase mappings}
@d sf_code_base=uc_code_base+256 {table of 256 spacefactor mappings}
@d math_code_base=sf_code_base+256 {table of 256 math mode mappings}
@d char_sub_code_base=math_code_base+256 {table of character substitutions}
@d int_base=char_sub_code_base+256 {beginning of region 5}
@y
@d toks_base=etex_toks {table of number_regs token list registers}
@#
@d etex_pen_base=toks_base+number_regs {start of table of \eTeX's penalties}
@d inter_line_penalties_loc=etex_pen_base {additional penalties between lines}
@d club_penalties_loc=etex_pen_base+1 {penalties for creating club lines}
@d widow_penalties_loc=etex_pen_base+2 {penalties for creating widow lines}
@d display_widow_penalties_loc=etex_pen_base+3 {ditto, just before a display}
@d etex_pens=etex_pen_base+4 {end of table of \eTeX's penalties}
@#
@d box_base=etex_pens {table of number_regs box registers}
@d cur_font_loc=box_base+number_regs {internal font number outside math mode}
@d xord_code_base=cur_font_loc+1
@d xchr_code_base=xord_code_base+1
@d xprn_code_base=xchr_code_base+1
@d math_font_base=xprn_code_base+1
@d cat_code_base=math_font_base+number_math_fonts
  {table of number_chars command codes (the ``catcodes'')}
@d lc_code_base=cat_code_base+number_chars {table of number_chars lowercase mappings}
@d uc_code_base=lc_code_base+number_chars {table of number_chars uppercase mappings}
@d sf_code_base=uc_code_base+number_chars {table of number_chars spacefactor mappings}
@d math_code_base=sf_code_base+number_chars {table of number_chars math mode mappings}
@d char_sub_code_base=math_code_base+number_chars {table of character substitutions}
@d int_base=char_sub_code_base+number_chars {beginning of region 5}
@z

@x
@d var_code==@'70000 {math code meaning ``use the current family''}
@y
@d var_fam_class = 7
@d active_math_char = @"1FFFFF
@d is_active_math_char(#) == math_char_field(#) = active_math_char
@d is_var_family(#) == math_class_field(#) = 7
@z

@x
for k:=output_routine_loc to toks_base+255 do
@y
for k:=output_routine_loc to toks_base+number_regs-1 do
@z

@x
for k:=box_base+1 to box_base+255 do eqtb[k]:=eqtb[box_base];
@y
for k:=box_base+1 to box_base+number_regs-1 do eqtb[k]:=eqtb[box_base];
@z

@x
for k:=math_font_base to math_font_base+47 do eqtb[k]:=eqtb[cur_font_loc];
@y
for k:=math_font_base to math_font_base+number_math_fonts-1 do eqtb[k]:=eqtb[cur_font_loc];
@z

@x
for k:=0 to 255 do
@y
for k:=0 to number_chars-1 do
@z

@x
for k:="0" to "9" do math_code(k):=hi(k+var_code);
@y
for k:="0" to "9" do math_code(k):=hi(k + set_class_field(var_fam_class));
@z

@x
  math_code(k):=hi(k+var_code+@"100);
  math_code(k+"a"-"A"):=hi(k+"a"-"A"+var_code+@"100);@/
@y
  math_code(k) := hi(k + set_family_field(1) + set_class_field(var_fam_class));
  math_code(k+"a"-"A") := hi(k+"a"-"A" + set_family_field(1) + set_class_field(var_fam_class));@/
@z

@x
begin if n=cur_font_loc then print("current font")
else if n<math_font_base+16 then
  begin print_esc("textfont"); print_int(n-math_font_base);
  end
else if n<math_font_base+32 then
  begin print_esc("scriptfont"); print_int(n-math_font_base-16);
  end
else  begin print_esc("scriptscriptfont"); print_int(n-math_font_base-32);
@y
begin if n=cur_font_loc then print("current font")
else if n<math_font_base+script_size then
  begin print_esc("textfont"); print_int(n-math_font_base);
  end
else if n<math_font_base+script_script_size then
  begin print_esc("scriptfont"); print_int(n-math_font_base-script_size);
  end
else  begin print_esc("scriptscriptfont");
  print_int(n-math_font_base-script_script_size);
@z

@x
@d eTeX_state_code=etex_int_base+9 {\eTeX\ state variables}
@d etex_int_pars=eTeX_state_code+eTeX_states {total number of \eTeX's integer parameters}
@y
@d XeTeX_linebreak_locale_code=etex_int_base+9 {string number of locale to use for linebreak locations}
@d XeTeX_linebreak_penalty_code=etex_int_base+10 {penalty to use at locale-dependent linebreak locations}
@d eTeX_state_code=etex_int_base+11 {\eTeX\ state variables}
@d etex_int_pars=eTeX_state_code+eTeX_states {total number of \eTeX's integer parameters}
@z

@x
@d count_base=int_base+int_pars {256 user \.{\\count} registers}
@d del_code_base=count_base+256 {256 delimiter code mappings}
@d dimen_base=del_code_base+256 {beginning of region 6}
@y
@d count_base=int_base+int_pars {number_regs user \.{\\count} registers}
@d del_code_base=count_base+number_regs {number_chars delimiter code mappings}
@d dimen_base=del_code_base+number_chars {beginning of region 6}
@z

@x
@d saving_hyph_codes==int_par(saving_hyph_codes_code)
@y
@d saving_hyph_codes==int_par(saving_hyph_codes_code)
@d XeTeX_linebreak_locale==int_par(XeTeX_linebreak_locale_code)
@d XeTeX_linebreak_penalty==int_par(XeTeX_linebreak_penalty_code)
@z

@x
error_context_lines_code:print_esc("errorcontextlines");
@y
error_context_lines_code:print_esc("errorcontextlines");
{XeTeX_linebreak_locale_code:print_esc("XeTeXlinebreaklocale");}
XeTeX_linebreak_penalty_code:print_esc("XeTeXlinebreakpenalty");
@z

@x
primitive("errorcontextlines",assign_int,int_base+error_context_lines_code);@/
@!@:error_context_lines_}{\.{\\errorcontextlines} primitive@>
@y
primitive("errorcontextlines",assign_int,int_base+error_context_lines_code);@/
@!@:error_context_lines_}{\.{\\errorcontextlines} primitive@>
primitive("XeTeXlinebreakpenalty",assign_int,int_base+XeTeX_linebreak_penalty_code);@/
@z

@x
for k:=0 to 255 do del_code(k):=-1;
del_code("."):=0; {this null delimiter is used in error recovery}
@y
for k:=0 to number_chars-1 do del_code(k):=-1;
del_code("."):=0; {this null delimiter is used in error recovery}
@z

@x
@ The final region of |eqtb| contains the dimension parameters defined
here, and the 256 \.{\\dimen} registers.
@y
@ The final region of |eqtb| contains the dimension parameters defined
here, and the |number_regs| \.{\\dimen} registers.
@z

@x
@d emergency_stretch_code=20 {reduces badnesses on final pass of line-breaking}
@d dimen_pars=21 {total number of dimension parameters}
@y
@d emergency_stretch_code=20 {reduces badnesses on final pass of line-breaking}
@d pdf_page_width_code=21 {page width of the PDF output}
@d pdf_page_height_code=22 {page height of the PDF output}
@d dimen_pars=23 {total number of dimension parameters}
@z

@x
@d scaled_base=dimen_base+dimen_pars
  {table of 256 user-defined \.{\\dimen} registers}
@d eqtb_size=scaled_base+255 {largest subscript of |eqtb|}
@y
@d scaled_base=dimen_base+dimen_pars
  {table of |number_regs| user-defined \.{\\dimen} registers}
@d eqtb_size=scaled_base+biggest_reg {largest subscript of |eqtb|}
@z

@x
@d emergency_stretch==dimen_par(emergency_stretch_code)
@y
@d emergency_stretch==dimen_par(emergency_stretch_code)
@d pdf_page_width    == dimen_par(pdf_page_width_code)
@d pdf_page_height   == dimen_par(pdf_page_height_code)
@z

@x
emergency_stretch_code:print_esc("emergencystretch");
@y
emergency_stretch_code:print_esc("emergencystretch");
pdf_page_width_code:    print_esc("pdfpagewidth");
pdf_page_height_code:   print_esc("pdfpageheight");
@z

@x
primitive("emergencystretch",assign_dimen,dimen_base+emergency_stretch_code);@/
@!@:emergency_stretch_}{\.{\\emergencystretch} primitive@>
@y
primitive("emergencystretch",assign_dimen,dimen_base+emergency_stretch_code);@/
@!@:emergency_stretch_}{\.{\\emergencystretch} primitive@>

primitive("pdfpagewidth",assign_dimen,dimen_base+pdf_page_width_code);@/
@!@:pdf_page_width_}{\.{\\pdfpagewidth} primitive@>
primitive("pdfpageheight",assign_dimen,dimen_base+pdf_page_height_code);@/
@!@:pdf_page_height_}{\.{\\pdfpageheight} primitive@>
@z

@x
for q:=active_base to box_base+255 do
@y
for q:=active_base to box_base+biggest_reg do
@z

@x
@* \[18] The hash table.
@y
@* \[18] The hash table.
@z

@x
while pool_ptr>str_start[str_ptr] do
@y
while pool_ptr>str_start_macro(str_ptr) do
@z

@x
The conversion from control sequence to byte sequence for enc\TeX is
implemented here. Of course, the simplest way is to implement an array
of string pointers with |hash_size| length, but we assume that only a
few control sequences will need to be converted. So |mubyte_cswrite|,
an array with only 128 items, is used. The items point to the token
lists. First token includes a csname number and the second points the
string to be output. The third token includes the number of another
csname and fourth token its pointer to the string etc. We need to do
the sequential searching in one of the 128 token lists.
@y
@z

@x
else  begin k:=str_start[s]; l:=str_start[s+1]-k;
@y
else  begin k:=str_start_macro(s); l:=str_start_macro(s+1)-k;
@z

@x
primitive("delimiter",delim_num,0);@/
@y
primitive("delimiter",delim_num,0);@/
primitive("XeTeXdelimiter",delim_num,1);@/
@z

@x
primitive("mathaccent",math_accent,0);@/
@y
primitive("mathaccent",math_accent,0);@/
primitive("XeTeXmathaccent",math_accent,1);@/
@z

@x
primitive("mathchar",math_char_num,0);@/
@!@:math_char_}{\.{\\mathchar} primitive@>
@y
primitive("mathchar",math_char_num,0);@/
primitive("XeTeXmathcharnum",math_char_num,1);@/
primitive("XeTeXmathchar",math_char_num,2);@/
@!@:math_char_}{\.{\\mathchar} primitive@>
@z

@x
primitive("radical",radical,0);@/
@y
primitive("radical",radical,0);@/
primitive("XeTeXradical",radical,1);@/
@z

@x
primitive("relax",relax,256); {cf.\ |scan_file_name|}
@y
primitive("relax",relax,too_big_char); {cf.\ |scan_file_name|}
@z

@x
delim_num: print_esc("delimiter");
@y
delim_num: if chr_code=1 then print_esc("XeTeXdelimiter")
  else print_esc("delimiter");
@z

@x
end_cs_name: if chr_code = 10 then print_esc("endmubyte")
             else print_esc("endcsname");
@y
end_cs_name: print_esc("endcsname");
@z

@x
math_accent: print_esc("mathaccent");
@y
math_accent: if chr_code=1 then print_esc("XeTeXmathaccent")
  else print_esc("mathaccent");
@z

@x
math_char_num: print_esc("mathchar");
@y
math_char_num: if chr_code=2 then print_esc("XeTeXmathchar")
  else if chr_code=1 then print_esc("XeTeXmathcharnum")
  else print_esc("mathchar");
@z

@x
radical: print_esc("radical");
@y
radical: if chr_code=1 then print_esc("XeTeXradical") else print_esc("radical");
@z

@x
@* \[19] Saving and restoring equivalents.
@y
@* \[19] Saving and restoring equivalents.
@z

-- based on Omega; not needed with new xetex delimiter coding
 x
@ The |eq_define| and |eq_word_define| routines take care of local definitions.
 y
@#
procedure eq_word_define1(@!p:pointer;@!w:integer);
label exit;
begin if eTeX_ex and(read_cint1(eqtb[p])=w) then
  begin assign_trace(p,"reassigning")@;@/
  return;
  end;
assign_trace(p,"changing")@;@/
if xeq_level[p]<>cur_level then
  begin eq_save(p,xeq_level[p]); xeq_level[p]:=cur_level;
  end;
set_cint1(eqtb[p],w);
assign_trace(p,"into")@;@/
exit:end;

@ The |eq_define| and |eq_word_define| routines take care of local definitions.
 z

 x
@ Subroutine |save_for_after| puts a token on the stack for save-keeping.
 y
@#
procedure geq_word_define1(@!p:pointer;@!w:integer); {global |eq_word_define1|}
begin assign_trace(p,"globally changing")@;@/
begin set_cint1(eqtb[p],w); xeq_level[p]:=level_one;
end;
assign_trace(p,"into")@;@/
end;

@ Subroutine |save_for_after| puts a token on the stack for save-keeping.
 z

@x
@* \[20] Token lists.
@y
@* \[20] Token lists.
@z

@x
A \TeX\ token is either a character or a control sequence, and it is
@^token@>
represented internally in one of two ways: (1)~A character whose ASCII
code number is |c| and whose command code is |m| is represented as the
number $2^8m+c$; the command code is in the range |1<=m<=14|. (2)~A control
sequence whose |eqtb| address is |p| is represented as the number
|cs_token_flag+p|. Here |cs_token_flag=@t$2^{12}-1$@>| is larger than
$2^8m+c$, yet it is small enough that |cs_token_flag+p< max_halfword|;
thus, a token fits comfortably in a halfword.
@y
A \TeX\ token is either a character or a control sequence, and it is
@^token@>
represented internally in one of two ways: (1)~A character whose ASCII
code number is |c| and whose command code is |m| is represented as the
number $2^16m+c$; the command code is in the range |1<=m<=14|. (2)~A control
sequence whose |eqtb| address is |p| is represented as the number
|cs_token_flag+p|. Here |cs_token_flag=@t$2^{20}-1$@>| is larger than
$2^8m+c$, yet it is small enough that |cs_token_flag+p< max_halfword|;
thus, a token fits comfortably in a halfword.
@z

@x
@d cs_token_flag==@'7777 {amount added to the |eqtb| location in a
  token that stands for a control sequence; is a multiple of~256, less~1}
@d left_brace_token=@'0400 {$2^8\cdot|left_brace|$}
@d left_brace_limit=@'1000 {$2^8\cdot(|left_brace|+1)$}
@d right_brace_token=@'1000 {$2^8\cdot|right_brace|$}
@d right_brace_limit=@'1400 {$2^8\cdot(|right_brace|+1)$}
@d math_shift_token=@'1400 {$2^8\cdot|math_shift|$}
@d tab_token=@'2000 {$2^8\cdot|tab_mark|$}
@d out_param_token=@'2400 {$2^8\cdot|out_param|$}
@d space_token=@'5040 {$2^8\cdot|spacer|+|" "|$}
@d letter_token=@'5400 {$2^8\cdot|letter|$}
@d other_token=@'6000 {$2^8\cdot|other_char|$}
@d match_token=@'6400 {$2^8\cdot|match|$}
@d end_match_token=@'7000 {$2^8\cdot|end_match|$}
@d protected_token=@'7001 {$2^8\cdot|end_match|+1$}
@y
@d cs_token_flag=@"FFFFF {amount added to the |eqtb| location in a
  token that stands for a control sequence; is a multiple of~65536, less~1}
@d max_char_val=@"10000 {to separate char and command code}
@d left_brace_token=@"10000 {$2^16\cdot|left_brace|$}
@d left_brace_limit=@"20000 {$2^16\cdot(|left_brace|+1)$}
@d right_brace_token=@"20000 {$2^16\cdot|right_brace|$}
@d right_brace_limit=@"30000 {$2^16\cdot(|right_brace|+1)$}
@d math_shift_token=@"30000 {$2^16\cdot|math_shift|$}
@d tab_token=@"40000 {$2^16\cdot|tab_mark|$}
@d out_param_token=@"50000 {$2^16\cdot|out_param|$}
@d space_token=@"A0020 {$2^16\cdot|spacer|+|" "|$}
@d letter_token=@"B0000 {$2^16\cdot|letter|$}
@d other_token=@"C0000 {$2^16\cdot|other_char|$}
@d match_token=@"D0000 {$2^16\cdot|match|$}
@d end_match_token=@"E0000 {$2^16\cdot|end_match|$}

@d protected_token=end_match_token+1 {$2^8\cdot|end_match|+1$}
@z

@x
else  begin m:=info(p) div @'400; c:=info(p) mod @'400;
@y
else  begin m:=info(p) div max_char_val; c:=info(p) mod max_char_val;
@z

@x
@* \[21] Introduction to the syntactic routines.
@y
@* \[21] Introduction to the syntactic routines.
@z

@x
procedure print_cmd_chr(@!cmd:quarterword;@!chr_code:halfword);
var n:integer; {temp variable}
@y
procedure print_cmd_chr(@!cmd:quarterword;@!chr_code:halfword);
var n:integer; {temp variable}
@!font_name_str:str_number; {local vars for \.{\\fontname} quoting extension}
@!quote_char:UTF16_code;
@z

@x
@* \[22] Input stacks and states.
@y
@* \[22] Input stacks and states.
@z

@x
@!input_file : ^alpha_file;
@y
@!input_file : ^unicode_file;
@z

@x
for q:=p to first_count-1 do print_char(trick_buf[q mod error_line]);
print_ln;
for q:=1 to n do print_char(" "); {print |n| spaces to begin line~2}
if m+n<=error_line then p:=first_count+m else p:=first_count+(error_line-n-3);
for q:=first_count to p-1 do print_char(trick_buf[q mod error_line]);
@y
for q:=p to first_count-1 do print_visible_char(trick_buf[q mod error_line]);
print_ln;
for q:=1 to n do print_visible_char(" "); {print |n| spaces to begin line~2}
if m+n<=error_line then p:=first_count+m else p:=first_count+(error_line-n-3);
for q:=first_count to p-1 do print_visible_char(trick_buf[q mod error_line]);
@z

@x
@* \[23] Maintaining the input stacks.
@y
@* \[23] Maintaining the input stacks.
@z

@x
if name>17 then a_close(cur_file); {forget it}
@y
if name>17 then u_close(cur_file); {forget it}
@z

@x
@* \[24] Getting the next token.
@y
@* \[24] Getting the next token.
@z

@x
primitive("par",par_end,256); {cf. |scan_file_name|}
@y
primitive("par",par_end,too_big_char); {cf. |scan_file_name|}
@z

@x
@!c,@!cc:ASCII_code; {constituents of a possible expanded code}
@!d:2..3; {number of excess characters in an expanded code}
@y
@!c,@!cc,@!ccc,@!cccc:ASCII_code; {constituents of a possible expanded code}
@!d:2..7; {number of excess characters in an expanded code}
@z

@x
@d hex_to_cur_chr==
  if c<="9" then cur_chr:=c-"0" @+else cur_chr:=c-"a"+10;
  if cc<="9" then cur_chr:=16*cur_chr+cc-"0"
  else cur_chr:=16*cur_chr+cc-"a"+10
@y
@d hex_to_cur_chr==
  if c<="9" then cur_chr:=c-"0" @+else cur_chr:=c-"a"+10;
  if cc<="9" then cur_chr:=16*cur_chr+cc-"0"
  else cur_chr:=16*cur_chr+cc-"a"+10
@d long_hex_to_cur_chr==
  if c<="9" then cur_chr:=c-"0" @+else cur_chr:=c-"a"+10;
  if cc<="9" then cur_chr:=16*cur_chr+cc-"0"
  else cur_chr:=16*cur_chr+cc-"a"+10;
  if ccc<="9" then cur_chr:=16*cur_chr+ccc-"0"
  else cur_chr:=16*cur_chr+ccc-"a"+10;
  if cccc<="9" then cur_chr:=16*cur_chr+cccc-"0"
  else cur_chr:=16*cur_chr+cccc-"a"+10
@z

@x
  begin c:=buffer[loc+1]; @+if c<@'200 then {yes we have an expanded char}
@y
  begin if (cur_chr=buffer[loc+1]) and (cur_chr=buffer[loc+2]) and
           ((loc+6)<=limit) then
     begin c:=buffer[loc+3]; cc:=buffer[loc+4];
       ccc:=buffer[loc+5]; cccc:=buffer[loc+6];
       if is_hex(c) and is_hex(cc) and is_hex(ccc) and is_hex(cccc) then
       begin loc:=loc+7; long_hex_to_cur_chr; goto reswitch;
       end;
     end;
  c:=buffer[loc+1]; @+if c<@'200 then {yes we have an expanded char}
@z

@x
begin if buffer[k]=cur_chr then @+if cat=sup_mark then @+if k<limit then
  begin c:=buffer[k+1]; @+if c<@'200 then {yes, one is indeed present}
    begin d:=2;
    if is_hex(c) then @+if k+2<=limit then
      begin cc:=buffer[k+2]; @+if is_hex(cc) then incr(d);
      end;
    if d>2 then
      begin hex_to_cur_chr; buffer[k-1]:=cur_chr;
      end
    else if c<@'100 then buffer[k-1]:=c+@'100
    else buffer[k-1]:=c-@'100;
    limit:=limit-d; first:=first-d;
    while k<=limit do
      begin buffer[k]:=buffer[k+d]; incr(k);
      end;
    goto start_cs;
    end;
  end;
end
@y
begin if buffer[k]=cur_chr then @+if cat=sup_mark then @+if k<limit then
  begin if (cur_chr=buffer[k+1]) and (cur_chr=buffer[k+2]) and
           ((k+6)<=limit) then
     begin c:=buffer[k+3]; cc:=buffer[k+4];
       ccc:=buffer[k+5]; cccc:=buffer[k+6];
       if is_hex(c) and is_hex(cc) and is_hex(ccc) and is_hex(cccc) then
       begin d:=7; long_hex_to_cur_chr; buffer[k-1]:=cur_chr;
             while k<=limit do
                begin buffer[k]:=buffer[k+d]; incr(k);
                end;
             goto start_cs;
       end
     end
     else begin
       c:=buffer[k+1]; @+if c<@'200 then {yes, one is indeed present}
       begin
          d:=2;
          if is_hex(c) then @+if k+2<=limit then
            begin cc:=buffer[k+2]; @+if is_hex(cc) then incr(d);
            end;
          if d>2 then
            begin hex_to_cur_chr; buffer[k-1]:=cur_chr;
            end
          else if c<@'100 then buffer[k-1]:=c+@'100
          else buffer[k-1]:=c-@'100;
          limit:=limit-d; first:=first-d;
          while k<=limit do
            begin buffer[k]:=buffer[k+d]; incr(k);
            end;
          goto start_cs;
       end
     end
  end
end
@z

@x
  else  begin cur_cmd:=t div @'400; cur_chr:=t mod @'400;
@y
  else  begin cur_cmd:=t div max_char_val; cur_chr:=t mod max_char_val;
@z

@x
@d no_expand_flag=257 {this characterizes a special variant of |relax|}
@y
@d no_expand_flag=special_char {this characterizes a special variant of |relax|}
@z

@x
  k := start;
  while k < limit do begin print_buffer(k) end;
@y
  if start<limit then for k:=start to limit-1 do print(buffer[k]);
@z

@x
if cur_cs=0 then cur_tok:=(cur_cmd*@'400)+cur_chr
@y
if cur_cs=0 then cur_tok:=(cur_cmd*max_char_val)+cur_chr
@z

@x
  begin eq_define(cur_cs,relax,256); {N.B.: The |save_stack| might change}
@y
  begin eq_define(cur_cs,relax,too_big_char);
        {N.B.: The |save_stack| might change}
@z

@x
  buffer[j]:=info(p) mod @'400; incr(j); p:=link(p);
@y
  buffer[j]:=info(p) mod max_char_val; incr(j); p:=link(p);
@z

@x
done: if cur_cs=0 then cur_tok:=(cur_cmd*@'400)+cur_chr
@y
done: if cur_cs=0 then cur_tok:=(cur_cmd*max_char_val)+cur_chr
@z

@x
if cur_cs=0 then cur_tok:=(cur_cmd*@'400)+cur_chr
@y
if cur_cs=0 then cur_tok:=(cur_cmd*max_char_val)+cur_chr
@z

@x
if (info(r)>match_token+255)or(info(r)<match_token) then s:=null
@y
if (info(r)>=end_match_token)or(info(r)<match_token) then s:=null
@z

@x
@* \[26] Basic scanning subroutines.
@y
@* \[26] Basic scanning subroutines.
@z

@x
begin p:=backup_head; link(p):=null; k:=str_start[s];
while k<str_start[s+1] do
@y
begin p:=backup_head; link(p):=null;
if s<too_big_char then begin
  while true do
    begin get_x_token; {recursion is possible here}
@^recursion@>
    if (cur_cs=0)and@|
       ((cur_chr=s)or(cur_chr=s-"a"+"A")) then
      begin store_new_token(cur_tok);
      flush_list(link(backup_head)); scan_keyword:=true; return;
      end
    else if (cur_cmd<>spacer)or(p<>backup_head) then
      begin back_input;
      if p<>backup_head then back_list(link(backup_head));
      scan_keyword:=false; return;
      end;
    end;
  end;
k:=str_start_macro(s);
while k<str_start_macro(s+1) do
@z

@x
@!cur_val:integer; {value returned by numeric scanners}
@y
@!cur_val:integer; {value returned by numeric scanners}
@!cur_val1:integer; {value returned by numeric scanners}
@z

@x
var m:halfword; {|chr_code| part of the operand token}
@y
var m:halfword; {|chr_code| part of the operand token}
    n, k, kk: integer; {accumulators}
@z

@x
def_code: @<Fetch a character code from some table@>;
@y
def_code: @<Fetch a character code from some table@>;
XeTeX_def_code:
  begin
    scan_char_num;
    if m=math_code_base then begin
      scanned_result(ho(math_code(cur_val)))(int_val)
    end
    else if m=math_code_base+1 then begin
      print_err("Can't use \XeTeXmathcode as a number");
      help2("\XeTeXmathcode is for setting a mathcode from separate values;")@/
      ("use \XeTeXmathcodenum to access them as single values."); error;
      scanned_result(0)(int_val)
    end
    else if m=del_code_base then begin
      scanned_result(ho(del_code(cur_val)))(int_val)
    end else begin
      print_err("Can't use \XeTeXdelcode as a number");
      help2("\XeTeXdelcode is for setting a delcode from separate values;")@/
      ("use \XeTeXdelcodenum to access them as single values."); error;
      scanned_result(0)(int_val);
    end;
  end;
@z

@x
if m=xord_code_base then scanned_result(xord[cur_val])(int_val)
else if m=xchr_code_base then scanned_result(xchr[cur_val])(int_val)
else if m=xprn_code_base then scanned_result(xprn[cur_val])(int_val)
else if m=math_code_base then scanned_result(ho(math_code(cur_val)))(int_val)
@y
if m=math_code_base then begin
  cur_val1:=ho(math_code(cur_val));
  if is_active_math_char(cur_val1) then
    cur_val1:=@"8000
  else if (math_class_field(cur_val1)>7) or
     (math_fam_field(cur_val1)>15) or
     (math_char_field(cur_val1)>255) then
    begin print_err("Extended mathchar used as mathchar");
@.Bad mathchar@>
    help2("A mathchar number must be between 0 and ""7FFF.")@/
      ("I changed this one to zero."); int_error(cur_val1); cur_val1:=0;
    end;
  cur_val1:=(math_class_field(cur_val1)*@"1000) +
            (math_fam_field(cur_val1)*@"100) +
            math_char_field(cur_val1);
  scanned_result(cur_val1)(int_val)
  end
else if m=del_code_base then begin
  cur_val1:=del_code(cur_val);
  if cur_val1>=@"40000000 then begin
    print_err("Extended delcode used as delcode");
@.Bad delcode@>
    help2("A delimiter code must be between 0 and ""7FFFFFF.")@/
      ("I changed this one to zero."); error;
    scanned_result(0)(int_val);
  end else begin
    scanned_result(cur_val1)(int_val);
  end
end
@z

@x
@d eTeX_dim=eTeX_int+8 {first of \eTeX\ codes for dimensions}
@y
@#
@d XeTeX_int=eTeX_int+8 {first of \XeTeX\ codes for integers}
@#
@d eTeX_dim=XeTeX_int+29 {first of \eTeX\ codes for dimensions}
 {changed for \XeTeX\ to make room for \XeTeX\ integers}
@z

@x
procedure scan_eight_bit_int;
begin scan_int;
if (cur_val<0)or(cur_val>255) then
  begin print_err("Bad register code");
@.Bad register code@>
  help2("A register number must be between 0 and 255.")@/
@y
procedure scan_eight_bit_int; {only used for insertion numbers now}
begin scan_int;
if (cur_val<0)or(cur_val>255) then
  begin print_err("Bad register code");
@.Bad register code@>
  help2("An insertion number must be between 0 and 255.")@/
@z

@x
procedure scan_char_num;
begin scan_int;
if (cur_val<0)or(cur_val>255) then
@y
procedure scan_usv_num;
begin scan_int;
if (cur_val<0)or(cur_val>@"10FFFF) then
  begin print_err("Bad USV code");
@.Bad character code@>
  help2("A Unicode Scalar Value must be between 0 and ""10FFFF.")@/
    ("I changed this one to zero."); int_error(cur_val); cur_val:=0;
  end;
end;

procedure scan_char_num;
begin scan_int;
if (cur_val<0)or(cur_val>biggest_char) then
@z

@x
  help2("A character number must be between 0 and 255.")@/
@y
  help2("A character number must be between 0 and 65535.")@/
@z

@x
procedure scan_four_bit_int;
@y
procedure scan_xetex_math_char_int;
begin scan_int;
  if is_active_math_char(cur_val) then begin
    if cur_val <> active_math_char then begin
      print_err("Bad active XeTeX math code");
      help2("Since I ignore class and family for active math chars,")@/
      ("I changed this one to ""1FFFFF."); int_error(cur_val);
      cur_val := active_math_char;
    end
  end else if math_char_field(cur_val) > @"10FFFF then begin
    print_err("Bad XeTeX math character code");
    help2("Since I expected a character number between 0 and ""10FFFF,")@/
    ("I changed this one to zero."); int_error(cur_val); cur_val:=0;
  end;
end;

procedure scan_math_class_int;
begin scan_int;
if (cur_val<0)or(cur_val>7) then
  begin print_err("Bad math class");
@.Bad number@>
  help2("Since I expected to read a number between 0 and 7,")@/
    ("I changed this one to zero."); int_error(cur_val); cur_val:=0;
  end;
end;

procedure scan_math_fam_int;
begin scan_int;
if (cur_val<0)or(cur_val>number_math_families-1) then
  begin print_err("Bad math family");
@.Bad number@>
  help2("Since I expected to read a number between 0 and 255,")@/
    ("I changed this one to zero."); int_error(cur_val); cur_val:=0;
  end;
end;

procedure scan_four_bit_int;
@z

@x
procedure scan_twenty_seven_bit_int;
begin scan_int;
if (cur_val<0)or(cur_val>@'777777777) then
  begin print_err("Bad delimiter code");
@.Bad delimiter code@>
  help2("A numeric delimiter code must be between 0 and 2^{27}-1.")@/
    ("I changed this one to zero."); int_error(cur_val); cur_val:=0;
  end;
end;
@y
procedure scan_delimiter_int;
begin scan_int;
if (cur_val<0)or(cur_val>@'777777777) then
  begin print_err("Bad delimiter code");
@.Bad delimiter code@>
  help2("A numeric delimiter code must be between 0 and 2^{27}-1.")@/
    ("I changed this one to zero."); int_error(cur_val); cur_val:=0;
  end;
end;
@z

@x
if cur_val>255 then
@y
if cur_val>biggest_char then
@z

@x
@p procedure scan_dimen(@!mu,@!inf,@!shortcut:boolean);
@y
@p procedure xetex_scan_dimen(@!mu,@!inf,@!shortcut,@!requires_units:boolean);
@z

@x
@<Scan units and set |cur_val| to $x\cdot(|cur_val|+f/2^{16})$, where there
  are |x| sp per unit; |goto attach_sign| if the units are internal@>;
@<Scan an optional space@>;
@y
if requires_units then begin
@<Scan units and set |cur_val| to $x\cdot(|cur_val|+f/2^{16})$, where there
  are |x| sp per unit; |goto attach_sign| if the units are internal@>;
@<Scan an optional space@>;
end else begin
 if cur_val>=@'40000 then arith_error:=true
 else cur_val:=cur_val*unity+f;
end;
@z

@x
@ @<Fetch an internal dimension and |goto attach_sign|...@>=
@y
procedure scan_dimen(@!mu,@!inf,@!shortcut:boolean);
begin
  xetex_scan_dimen(mu,inf,shortcut,true);
end;

@ For XeTeX, we have an additional version |scan_decimal|, like |scan_dimen| 
but without any scanning of units.

@p procedure scan_decimal;
  {sets |cur_val| to a quantity expressed as a decimal fraction}
begin
 xetex_scan_dimen(false, false, false, false);
end;

@ @<Fetch an internal dimension and |goto attach_sign|...@>=
@z

@x
@d etex_convert_base=5 {base for \eTeX's command codes}
@d eTeX_revision_code=etex_convert_base {command code for \.{\\eTeXrevision}}
@d etex_convert_codes=etex_convert_base+1 {end of \eTeX's command codes}
@y
@d etex_convert_base=5 {base for \eTeX's command codes}
@d eTeX_revision_code=etex_convert_base {command code for \.{\\eTeXrevision}}

@d XeTeX_revision_code=6
@d XeTeX_variation_name_code=7	{ must match codes in xetexmac.c }
@d XeTeX_feature_name_code=8
@d XeTeX_selector_name_code=9
@d XeTeX_glyph_name_code=10

@d etex_convert_codes=XeTeX_glyph_name_code+1 {end of \eTeX's command codes}
@z

@x
  eTeX_revision_code: print_esc("eTeXrevision");
@y
  @/@<Cases of |convert| for |print_cmd_chr|@>@/
@z

@x
@!c:number_code..job_name_code; {desired type of conversion}
@y
@!c:small_number; {desired type of conversion}
@z

@x
@!b:pool_pointer; {base of temporary string}
@y
@!b:pool_pointer; {base of temporary string}
@!fnt,@!arg1,@!arg2:integer; {args for \XeTeX\ extensions}
@!font_name_str:str_number; {local vars for \.{\\fontname} quoting extension}
@!i:small_number;
@!quote_char:UTF16_code;
@z

@x
eTeX_revision_code: do_nothing;
@y
@/@<Cases of `Scan the argument for command |c|'@>@/
@z

@x
font_name_code: begin print(font_name[cur_val]);
@y
font_name_code: begin
  font_name_str:=font_name[cur_val];
  if is_native_font(cur_val) then begin
    quote_char:="""";
    for i:=0 to length(font_name_str) - 1 do
     if str_pool[str_start_macro(font_name_str) + i] = """" then quote_char:="'";
    print_char(quote_char);
    print(font_name_str);
    print_char(quote_char);
  end else
    print(font_name_str);
@z

@x
eTeX_revision_code: print(eTeX_revision);
@y
@/@<Cases of `Print the result of command |c|'@>@/
@z

@x
job_name_code: print(job_name);
@y
job_name_code: print_file_name(job_name, 0, 0);
@z

@x
@!read_file:array[0..15] of alpha_file; {used for \.{\\read}}
@y
@!read_file:array[0..15] of unicode_file; {used for \.{\\read}}
@z

@x
else  begin a_close(read_file[m]); read_open[m]:=closed;
@y
else  begin u_close(read_file[m]); read_open[m]:=closed;
@z

@x
  begin a_close(read_file[m]); read_open[m]:=closed;
@y
  begin u_close(read_file[m]); read_open[m]:=closed;
@z

@x
if (cur_cmd>active_char)or(cur_chr>255) then {not a character}
  begin m:=relax; n:=256;
@y
if (cur_cmd>active_char)or(cur_chr>biggest_char) then {not a character}
  begin m:=relax; n:=too_big_char;
@z

@x
if (cur_cmd>active_char)or(cur_chr>255) then
  begin cur_cmd:=relax; cur_chr:=256;
@y
if (cur_cmd>active_char)or(cur_chr>biggest_char) then
  begin cur_cmd:=relax; cur_chr:=too_big_char;
@z

@x
@* \[29] File names.
@y
@* \[29] File names.
@z

@x
The following procedures don't allow spaces to be part of
file names; but some users seem to like names that are spaced-out.
System-dependent changes to allow such things should probably
be made with reluctance, and only when an entire file name that
includes spaces is ``quoted'' somehow.

@y
@z

@x
@!ext_delimiter:pool_pointer; {the most recent `\..', if any}
@y
@!ext_delimiter:pool_pointer; {the most recent `\..', if any}
@!file_name_quote_char:UTF16_code;
@z

@x
begin area_delimiter:=0; ext_delimiter:=0; quoted_filename:=false;
@y
begin area_delimiter:=0; ext_delimiter:=0; quoted_filename:=false;
file_name_quote_char:=0;
@z

@x
@p function more_name(@!c:ASCII_code):boolean;
begin if (c=" ") and stop_at_space and (not quoted_filename) then
  more_name:=false
else  if c="""" then begin
  quoted_filename:=not quoted_filename;
  more_name:=true;
  end
@y
@p function more_name(@!c:ASCII_code):boolean;
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
  more_name:=true;
  end
@z

@x
@p procedure end_name;
var temp_str: str_number; {result of file name cache lookups}
@!j,@!s,@!t: pool_pointer; {running indices}
@!must_quote:boolean; {whether we need to quote a string}
begin if str_ptr+3>max_strings then
  overflow("number of strings",max_strings-init_str_ptr);
@:TeX capacity exceeded number of strings}{\quad number of strings@>
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
@p procedure end_name;
var temp_str: str_number; {result of file name cache lookups}
@!j: pool_pointer; {running index}
begin if str_ptr+3>max_strings then
  overflow("number of strings",max_strings-init_str_ptr);
@:TeX capacity exceeded number of strings}{\quad number of strings@>
@z

@x
  str_start[str_ptr+1]:=str_start[str_ptr]+area_delimiter; incr(str_ptr);
@y
  str_start_macro(str_ptr+1):=str_start_macro(str_ptr)+area_delimiter; incr(str_ptr);
@z

@x
    for j:=str_start[str_ptr+1] to pool_ptr-1 do
@y
    for j:=str_start_macro(str_ptr+1) to pool_ptr-1 do
@z

@x
  str_start[str_ptr+1]:=str_start[str_ptr]+ext_delimiter-area_delimiter-1;
@y
  str_start_macro(str_ptr+1):=str_start_macro(str_ptr)+ext_delimiter-area_delimiter-1;
@z

@x
    for j:=str_start[str_ptr+1] to pool_ptr-1 do
@y
    for j:=str_start_macro(str_ptr+1) to pool_ptr-1 do
@z

@x
procedure print_file_name(@!n,@!a,@!e:integer);
var must_quote: boolean; {whether to quote the filename}
@!j:pool_pointer; {index into |str_pool|}
begin
must_quote:=false;
if a<>0 then begin
  j:=str_start[a];
  while (not must_quote) and (j<str_start[a+1]) do begin
    must_quote:=str_pool[j]=" "; incr(j);
  end;
end;
if n<>0 then begin
  j:=str_start[n];
  while (not must_quote) and (j<str_start[n+1]) do begin
    must_quote:=str_pool[j]=" "; incr(j);
  end;
end;
if e<>0 then begin
  j:=str_start[e];
  while (not must_quote) and (j<str_start[e+1]) do begin
    must_quote:=str_pool[j]=" "; incr(j);
  end;
end;
{FIXME: Alternative is to assume that any filename that has to be quoted has
 at least one quoted component...if we pick this, a number of insertions
 of |print_file_name| should go away.
|must_quote|:=((|a|<>0)and(|str_pool|[|str_start|[|a|]]=""""))or
              ((|n|<>0)and(|str_pool|[|str_start|[|n|]]=""""))or
              ((|e|<>0)and(|str_pool|[|str_start|[|e|]]=""""));}
if must_quote then print_char("""");
if a<>0 then
  for j:=str_start[a] to str_start[a+1]-1 do
    if so(str_pool[j])<>"""" then
      print(so(str_pool[j]));
if n<>0 then
  for j:=str_start[n] to str_start[n+1]-1 do
    if so(str_pool[j])<>"""" then
      print(so(str_pool[j]));
if e<>0 then
  for j:=str_start[e] to str_start[e+1]-1 do
    if so(str_pool[j])<>"""" then
      print(so(str_pool[j]));
if must_quote then print_char("""");
end;
@y
procedure print_file_name(@!n,@!a,@!e:integer);
var @!must_quote: boolean; {whether to quote the filename}
@!quote_char: integer; {current quote char (single or double)}
@!j:pool_pointer; {index into |str_pool|}
begin
must_quote:=false;
quote_char:=0;
if a<>0 then begin
  j:=str_start_macro(a);
  while ((not must_quote) or (quote_char=0)) and (j<>str_start_macro(a+1)) do begin
    if (str_pool[j]=" ") then must_quote:=true
    else if (str_pool[j]="""") or (str_pool[j]="'") then begin
      must_quote:=true;
      quote_char:="""" + "'" - str_pool[j];
    end;
    incr(j);
  end;
end;
if n<>0 then begin
  j:=str_start_macro(n);
  while ((not must_quote) or (quote_char=0)) and (j<>str_start_macro(n+1)) do begin
    if (str_pool[j]=" ") then must_quote:=true
    else if (str_pool[j]="""") or (str_pool[j]="'") then begin
      must_quote:=true;
      quote_char:="""" + "'" - str_pool[j];
    end;
    incr(j);
  end;
end;
if e<>0 then begin
  j:=str_start_macro(e);
  while ((not must_quote) or (quote_char=0)) and (j<>str_start_macro(e+1)) do begin
    if (str_pool[j]=" ") then must_quote:=true
    else if (str_pool[j]="""") or (str_pool[j]="'") then begin
      must_quote:=true;
      quote_char:="""" + "'" - str_pool[j];
    end;
    incr(j);
  end;
end;
if must_quote then begin
  if quote_char=0 then quote_char:="""";
  print_char(quote_char);
end;
if a<>0 then
  for j:=str_start_macro(a) to str_start_macro(a+1)-1 do begin
    if str_pool[j]=quote_char then begin
      print(quote_char);
      quote_char:="""" + "'" - quote_char;
      print(quote_char);
    end;
    print(str_pool[j]);
  end;
if n<>0 then
  for j:=str_start_macro(n) to str_start_macro(n+1)-1 do begin
    if str_pool[j]=quote_char then begin
      print(quote_char);
      quote_char:="""" + "'" - quote_char;
      print(quote_char);
    end;
    print(str_pool[j]);
  end;
if e<>0 then
  for j:=str_start_macro(e) to str_start_macro(e+1)-1 do begin
    if str_pool[j]=quote_char then begin
      print(quote_char);
      quote_char:="""" + "'" - quote_char;
      print(quote_char);
    end;
    print(str_pool[j]);
  end;
if quote_char<>0 then print_char(quote_char);
end;
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
for j:=str_start[a] to str_start[a+1]-1 do append_to_name(so(str_pool[j]));
for j:=str_start[n] to str_start[n+1]-1 do append_to_name(so(str_pool[j]));
for j:=str_start[e] to str_start[e+1]-1 do append_to_name(so(str_pool[j]));
@y
name_of_file:= xmalloc_array (UTF8_code, (length(a)+length(n)+length(e))*3+1);
for j:=str_start_macro(a) to str_start_macro(a+1)-1 do append_to_name(so(str_pool[j]));
for j:=str_start_macro(n) to str_start_macro(n+1)-1 do append_to_name(so(str_pool[j]));
for j:=str_start_macro(e) to str_start_macro(e+1)-1 do append_to_name(so(str_pool[j]));
@z

@x
name_of_file := xmalloc_array (ASCII_code, n+(b-a+1)+format_ext_length+1);
for j:=1 to n do append_to_name(xord[TEX_format_default[j]]);
@y
name_of_file := xmalloc_array (UTF8_code, n+(b-a+1)+format_ext_length+1);
for j:=1 to n do append_to_name(TEX_format_default[j]);
@z

@x
  append_to_name(xord[TEX_format_default[j]]);
@y
  append_to_name(TEX_format_default[j]);
@z

@x
@p function make_name_string:str_number;
var k:1..file_name_size; {index into |name_of_file|}
begin if (pool_ptr+name_length>pool_size)or(str_ptr=max_strings)or
 (cur_length>0) then
  make_name_string:="?"
else  begin for k:=1 to name_length do append_char(xord[name_of_file[k]]);
  make_name_string:=make_string;
  end;
  {At this point we also set |cur_name|, |cur_ext|, and |cur_area| to
   match the contents of |name_of_file|.}
  k:=1;
  name_in_progress:=true;
  begin_name;
  stop_at_space:=false;
  while (k<=name_length)and(more_name(name_of_file[k])) do
    incr(k);
  stop_at_space:=true;
  end_name;
  name_in_progress:=false;
end;
@y
@p function make_name_string:str_number;
var k:1..file_name_size; {index into |name_of_file|}
begin if (pool_ptr+name_length>pool_size)or(str_ptr=max_strings)or
 (cur_length>0) then
  make_name_string:="?"
else  begin
  make_utf16_name;
  for k:=0 to name_length16-1 do append_char(name_of_file16[k]);
  make_name_string:=make_string;
  end;
end;
function u_make_name_string(var f:unicode_file):str_number;
begin u_make_name_string:=make_name_string;
end;
@z

@x
loop@+begin if (cur_cmd>other_char)or(cur_chr>255) then {not a character}
@y
loop@+begin if (cur_cmd>other_char)or(cur_chr>biggest_char) then
    {not a character}
@z

@x
  {If |cur_chr| is a space and we're not scanning a token list, check
   whether we're at the end of the buffer. Otherwise we end up adding
   spurious spaces to file names in some cases.}
  if (cur_chr=" ") and (state<>token_list) and (loc>limit) then goto done;
@y
@z

@x
  pack_job_name(".dvi");
  while not b_open_out(dvi_file) do
    prompt_file_name("file name for output",".dvi");
@y
  pack_job_name(output_file_extension);
  while not dvi_open_out(dvi_file) do
    prompt_file_name("file name for output",output_file_extension);
@z

@x
@!dvi_file: byte_file; {the device-independent output goes here}
@y
@!output_file_extension: str_number;
@!no_pdf_output: boolean;
@!dvi_file: byte_file; {the device-independent output goes here}
@z

@x
@ @<Initialize the output...@>=output_file_name:=0;
@y
@ @<Initialize the output...@>=
  output_file_name:=0;
  if no_pdf_output then output_file_extension:=".xdv"
  else output_file_extension:=".pdf";
@z

@x
  if open_in_name_ok(stringcast(name_of_file+1))
     and a_open_in(cur_file, kpse_tex_format) then
    goto done;
@y
  if open_in_name_ok(stringcast(name_of_file+1))
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
@d non_char==qi(256) {a |halfword| code that can't match a real character}
@y
@d ot_font_flag=65534
@d aat_font_flag=65535
@d is_atsu_font(#)==(font_area[#]=aat_font_flag)
@d is_ot_font(#)==(font_area[#]=ot_font_flag)
@d is_native_font(#)==(is_atsu_font(#) or is_ot_font(#))
	{native fonts have font_area = 65534 or 65535,
	 which would be a string containing an invalid Unicode character}

@d non_char==qi(too_big_char) {a |halfword| code that can't match a real character}
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
@!font_mapping: ^void_pointer; { TECkit_Converter or 0 }
@!font_flags: ^char; { flags:
  0x01: font_colored
  0x02: font_vertical }
@!font_letter_space: ^scaled; { letterspacing to be applied to the font }
@!loaded_font_mapping: void_pointer; { used by load_native_font to return mapping, if any }
@!loaded_font_flags: char; { used by load_native_font to return flags }
@!loaded_font_letter_space: scaled;
@!mapped_text: ^UTF16_code; { scratch buffer used while applying font mappings }
@!xdv_buffer: ^char; { scratch buffer used in generating XDV output }
@z

@x
@<Read and check the font data; |abort| if the \.{TFM} file is
@y
file_opened:=false;
pack_file_name(nom,aire,cur_ext);
if quoted_filename then begin
  { quoted name, so try for a native font }
  g:=load_native_font(u,nom,aire,s);
  if g=null_font then goto bad_tfm else goto done;
end;
{ it was an unquoted name, so try for a TFM file }
@<Read and check the font data if file exists;
  |abort| if the \.{TFM} file is
@z

@x
bad_tfm: @<Report that the font won't be loaded@>;
@y
if g<>null_font then goto done;
if file_name_quote_char=0 then begin
  { we failed to find a TFM file, so try for a native font }
  g:=load_native_font(u,nom,aire,s);
  if g<>null_font then goto done
end;
bad_tfm:
if (not file_opened) and (file_name_quote_char<>0) then begin
  @<Report that native font couldn't be found, and |goto done|@>;
end;
@<Report that the font won't be loaded@>;
@z

@x
@d start_font_error_message==print_err("Font "); sprint_cs(u);
  print_char("="); print_file_name(nom,aire,"");
@y
@d start_font_error_message==print_err("Font "); sprint_cs(u);
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
@ @<Read and check...@>=
@<Open |tfm_file| for input@>;
@y
@ @<Report that native font couldn't be found, and |goto done|@>=
start_font_error_message;
@.Font x=xx not loadable...@>
print(" not loadable: installed font not found");
help4("I wasn't able to find this font in the Mac OS,")@/
("so I will ignore the font specification.")@/
("You might try inserting a different font spec;")@/
("e.g., type `I\font<same font id>=<substitute font name>'.");
error;
goto done

@ @<Read and check...@>=
@<Open |tfm_file| for input and |begin|@>;
@z

@x
@<Make final adjustments and |goto done|@>
@y
@<Make final adjustments and |goto done|@>;
end
@z

@x
@ @<Open |tfm_file| for input@>=
file_opened:=false;
@y
@ @<Open |tfm_file| for input...@>=
@z

@x
if not b_open_in(tfm_file) then abort;
file_opened:=true
@y
if b_open_in(tfm_file) then begin
  file_opened:=true
@z

@x we have to move this before new_native_character
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
  print_ASCII(c); print(" in font ");
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
@* \[31] Device-independent file format.
@y
@* \[31] Device-independent file format.
@z

@x
\yskip\noindent Commands 250--255 are undefined at the present time.
@y
\yskip\hang|set_glyph_string| 254 w[4] k[2] x[4k] g[2k].

\yskip\hang|set_glyph_array| 253 w[4] k[2] xy[8k] g[2k]

\yskip\hang|define_native_font| 252 k[4] s[4] flags[2]
	lenps[1] lenfam[1] lensty[1] ps[lenps] fam[lenfam] sty[lensty]
	if (flags & COLORED):
		rgba[4]
	if (flags & VARIATIONS):
		numvars[2]
		axes[4nv]
		values[4nv]
	if (flags & MATRIX):
		ta[4] tb[4] tc[4] td[4] tx[4] ty[4]

\yskip\hang|pic_file| 251 flags[1] t[4][6] p[2] len[2] path[l]
	flags = 0 for raster image, 1 for PDF
	t is transform matrix
	p is page # from the graphic file (0-based)
	len is length of pathname
	path is pathname of graphic file

\yskip\noindent Commands 250 and 255 are undefined at the present time (but 255 is used by pTeX).
@z

@x
@d post_post=249 {postamble ending}
@y
@d post_post=249 {postamble ending}

@d set_glyph_string=254 {sequence of glyphs, all at the current y-position}
@d set_glyph_array=253 {sequence of glyphs with individual x-y coordinates}
@d define_native_font=252 {define native font}
@d pic_file=251 {embed picture or PDF}
@z

@x
@d id_byte=2 {identifies the kind of \.{DVI} files described here}
@y
XeTeX changes the DVI version to 5,
as we have new DVI opcodes like |set_glyph_array| for native font text;
I used version 3 in an earlier extension of TeX,
and 4 in pre-1.0 XeTeX releases using Mac OS-specific data types.

@d id_byte=5 {identifies the kind of \.{DVI} files described here}
@z

@x
@* \[32] Shipping pages out.
@y
@* \[32] Shipping pages out.
@z

@x
@ A mild optimization of the output is performed by the |dvi_pop|
@y
procedure dvi_two(s: UTF16_code);
begin
	dvi_out(s div @'400);
	dvi_out(s mod @'400);
end;

@ A mild optimization of the output is performed by the |dvi_pop|
@z

@x
@p procedure dvi_font_def(@!f:internal_font_number);
@y
@p procedure dvi_native_font_def(@!f:internal_font_number);
var
	font_def_length, i: integer;
begin
	dvi_out(define_native_font);
	dvi_four(f-font_base-1);
	font_def_length := make_font_def(f);
	for i := 0 to font_def_length - 1 do dvi_out(xdv_buffer[i]);
end;

procedure dvi_font_def(@!f:internal_font_number);
@z

@x
begin if f<=256+font_base then
@y
begin if is_native_font(f) then dvi_native_font_def(f) else
begin if f<=256+font_base then
@z

@x
@<Output the font name whose internal number is |f|@>;
@y
@<Output the font name whose internal number is |f|@>;
end;
@z

@x
@ @<Output the font name whose internal number is |f|@>=
for k:=str_start[font_area[f]] to str_start[font_area[f]+1]-1 do
  dvi_out(so(str_pool[k]));
for k:=str_start[font_name[f]] to str_start[font_name[f]+1]-1 do
  dvi_out(so(str_pool[k]))
@y
@ @<Output the font name whose internal number is |f|@>=
for k:=str_start_macro(font_area[f]) to str_start_macro(font_area[f]+1)-1 do
  dvi_out(so(str_pool[k]));
for k:=str_start_macro(font_name[f]) to str_start_macro(font_name[f]+1)-1 do
  dvi_out(so(str_pool[k]))
@z

@x
@ @<Initialize variables as |ship_out| begins@>=
dvi_h:=0; dvi_v:=0; cur_h:=h_offset; dvi_f:=null_font;
@y
@ @<Initialize variables as |ship_out| begins@>=
dvi_h:=0; dvi_v:=0; cur_h:=h_offset; dvi_f:=null_font;
@<Calculate page dimensions and margins@>;
@z

@x
  print(" TeX output "); print_int(year); print_char(".");
@y
  print(" XeTeX output "); print_int(year); print_char(".");
@z

@x
  for s:=str_start[str_ptr] to pool_ptr-1 do dvi_out(so(str_pool[s]));
  pool_ptr:=str_start[str_ptr]; {flush the current string}
@y
  for s:=str_start_macro(str_ptr) to pool_ptr-1 do dvi_out(so(str_pool[s]));
  pool_ptr:=str_start_macro(str_ptr); {flush the current string}
@z

@x
@d next_p=15 {go to this label when finished with node |p|}
@y
@d next_p=15 {go to this label when finished with node |p|}

@d check_next=1236
@d end_node_run=1237
@z

@x
label reswitch, move_past, fin_rule, next_p, continue, found;
@y
label reswitch, move_past, fin_rule, next_p, continue, found, check_next, end_node_run;
@z

@x
@!prev_p:pointer; {one step behind |p|}
@y
@!prev_p:pointer; {one step behind |p|}
@!len: integer; { length of scratch string for native word output }
@!q,@!r: pointer;
@!k,@!j: integer;
@z

@x
g_sign:=glue_sign(this_box); p:=list_ptr(this_box);
@y
g_sign:=glue_sign(this_box);
@<Merge sequences of words using AAT fonts and inter-word spaces into single nodes@>;
p:=list_ptr(this_box);
@z

@x
@ We ought to give special care to the efficiency of one part of |hlist_out|,
@y
@ Extra stuff for justifiable AAT text; need to merge runs of words and normal spaces.

@d is_native_word_node(#) == (not is_char_node(#)) and (type(#) = whatsit_node) and (subtype(#) = native_word_node)

@<Merge sequences of words using AAT fonts and inter-word spaces into single nodes@>=
p := list_ptr(this_box);
prev_p := this_box+list_offset;
while p<>null do begin
  if link(p) <> null then begin {not worth looking ahead at the end}
    if is_native_word_node(p) and (font_area[native_font(p)] = aat_font_flag)
        and (font_letter_space[native_font(p)] = 0) then begin
      {got a word in an AAT font, might be the start of a run}
      r := p; {|r| is start of possible run}
      k := native_length(r);
      q := link(p);
check_next:
      @<Advance |q| past ignorable nodes@>;
      if (q <> null) and not is_char_node(q) then begin
        if (type(q) = glue_node) and (subtype(q) = normal) and (glue_ptr(q) = font_glue[native_font(r)]) then begin
          {found a normal space; if the next node is another word in the same font, we'll merge}
          q := link(q);
          @<Advance |q| past ignorable nodes@>;
          if (q <> null) and is_native_word_node(q) and (native_font(q) = native_font(r)) then begin
            p := q; {record new tail of run in |p|}
            k := k + 1 + native_length(q);
            q := link(q);
            goto check_next;
          end;
          goto end_node_run;
        end;
        {@<Advance |q| past ignorable nodes@>;}
        if (q <> null) and is_native_word_node(q) and (native_font(q) = native_font(r)) then begin
          p := q; {record new tail of run in |p|}
          q := link(q);
          goto check_next;
        end
      end;
end_node_run: {now |r| points to first |native_word_node| of the run, and |p| to the last}
      if p <> r then begin {merge nodes from |r| to |p| inclusive; total text length is |k|}
        str_room(k);
        k := 0; {now we'll use this as accumulator for total width}
        q := r;
        loop begin
          if type(q) = whatsit_node then begin
            if subtype(q) = native_word_node then begin
              for j := 0 to native_length(q)-1 do
                append_char(get_native_char(q, j));
              k := k + width(q);
            end
          end else if type(q) = glue_node then begin
            append_char(" ");
            g := glue_ptr(q);
            k := k + width(g);
            if g_sign <> normal then begin
              if g_sign = stretching then begin
                if stretch_order(g) = g_order then begin
                  k := k + round(float(glue_set(this_box)) * stretch(g))
                end
              end else begin
                if shrink_order(g) = g_order then begin
                  k := k - round(float(glue_set(this_box)) * shrink(g))
                end
              end
            end
          end;
          {discretionary and deleted nodes can be discarded here}
          if q = p then break
          else q := link(q);
        end;
done:
        q := new_native_word_node(native_font(r), cur_length);
        link(prev_p) := q;
        for j := 0 to cur_length - 1 do
          set_native_char(q, j, str_pool[str_start_macro(str_ptr) + j]);
        link(q) := link(p);
        link(p) := null;
        flush_node_list(r);
        width(q) := k;
		set_justified_native_glyphs(q);
        p := q;
        pool_ptr := str_start_macro(str_ptr); {flush the temporary string data}
      end
    end;
    prev_p := p;
  end;
  p := link(p);
end

@ @<Advance |q| past ignorable nodes@>=
while (q <> null) and (not is_char_node(q))
  and ( (type(q) = disc_node) or ((type(q) = whatsit_node) and (subtype(q) = deleted_native_node)) ) do
    q := link(q)

@ We ought to give special care to the efficiency of one part of |hlist_out|,
@z

@x
dvi_four(last_bop); last_bop:=page_loc;
@y
dvi_four(last_bop); last_bop:=page_loc;
if (pdf_page_width > 0) and (pdf_page_height > 0) then begin
  { generate a papersize \special at start of page }
  old_setting:=selector; selector:=new_string;
  print("papersize ");
  if mag=1000 then print_scaled(pdf_page_width)
  else print_scaled(xn_over_d(pdf_page_width,mag,1000));
  print("pt"); print(",");
  if mag=1000 then print_scaled(pdf_page_height)
  else print_scaled(xn_over_d(pdf_page_height,mag,1000));
  print("pt");
  selector:=old_setting;
  dvi_out(xxx1); dvi_out(cur_length);
  for s:=str_start_macro(str_ptr) to pool_ptr-1 do dvi_out(so(str_pool[s]));
  pool_ptr:=str_start_macro(str_ptr); {erase the string}
end;
@z

@x
dvi_out(eop); incr(total_pages); cur_s:=-1;
@y
dvi_out(eop); incr(total_pages); cur_s:=-1;
if not no_pdf_output then fflush(dvi_file);
@z

@x
  print_nl("Output written on "); print_file_name(0, output_file_name, 0);
@y
  print_nl("Output written on "); print(output_file_name);
@z

@x
  print(", "); print_int(dvi_offset+dvi_ptr); print(" bytes).");
  b_close(dvi_file);
@y
  if no_pdf_output then begin
    print(", "); print_int(dvi_offset+dvi_ptr); print(" bytes).");
  end else print(").");
  dvi_close(dvi_file);
@z

@x
@* \[33] Packaging.
@y
@* \[33] Packaging.
@z

@x
@p function hpack(@!p:pointer;@!w:scaled;@!m:small_number):pointer;
label reswitch, common_ending, exit;
@y
@p function hpack(@!p:pointer;@!w:scaled;@!m:small_number):pointer;
label reswitch, common_ending, exit, restart;
@z

@x
@!hd:eight_bits; {height and depth indices for a character}
@y
@!hd:eight_bits; {height and depth indices for a character}
@!pp,@!ppp: pointer;
@!total_chars, @!k: integer;
@z

@x
@* \[34] Data structures for math mode.
@y
@* \[34] Data structures for math mode.
@z

@x
@d fam==font {a |quarterword| in |mem|}
@y
@d plane_and_fam_field==font {a |quarterword| in |mem|}
@d fam(#) == (plane_and_fam_field(#) mod @"100)
@z

@x
@d small_fam(#)==mem[#].qqqq.b0 {|fam| for ``small'' delimiter}
@d small_char(#)==mem[#].qqqq.b1 {|character| for ``small'' delimiter}
@d large_fam(#)==mem[#].qqqq.b2 {|fam| for ``large'' delimiter}
@d large_char(#)==mem[#].qqqq.b3 {|character| for ``large'' delimiter}
@y
@d small_fam(#)==(mem[#].qqqq.b0 mod @"100) {|fam| for ``small'' delimiter}
@d small_char(#)==(mem[#].qqqq.b1 + (mem[#].qqqq.b0 div @"100) * @"10000) {|character| for ``small'' delimiter}
@d large_fam(#)==(mem[#].qqqq.b2 mod @"100) {|fam| for ``large'' delimiter}
@d large_char(#)==(mem[#].qqqq.b3 + (mem[#].qqqq.b2 div @"100) * @"10000) {|character| for ``large'' delimiter}
@d small_plane_and_fam_field(#)==mem[#].qqqq.b0
@d small_char_field(#)==mem[#].qqqq.b1
@d large_plane_and_fam_field(#)==mem[#].qqqq.b2
@d large_char_field(#)==mem[#].qqqq.b3
@z

@x
procedure print_fam_and_char(@!p:pointer); {prints family and character}
begin print_esc("fam"); print_int(fam(p)); print_char(" ");
print_ASCII(qo(character(p)));
@y
procedure print_fam_and_char(@!p:pointer); {prints family and character}
begin print_esc("fam"); print_int(fam(p) mod @"100); print_char(" ");
print_ASCII(qo(character(p)) + (fam(p) div @"100) * @"10000);
@z

@x
@* \[35] Subroutines for math mode.
@y
@* \[35] Subroutines for math mode.
@z

@x
@d text_size=0 {size code for the largest size in a family}
@d script_size=16 {size code for the medium size in a family}
@d script_script_size=32 {size code for the smallest size in a family}
@y
@z

@x
else cur_size:=16*((cur_style-text_style) div 2);
@y
else cur_size:=script_size*((cur_style-text_style) div 2);
@z

@x
function var_delimiter(@!d:pointer;@!s:small_number;@!v:scaled):pointer;
@y
function var_delimiter(@!d:pointer;@!s:integer;@!v:scaled):pointer;
@z

@x
@!z: small_number; {runs through font family members}
@y
@!z: integer; {runs through font family members}
@z

@x
  begin z:=z+s+16;
  repeat z:=z-16; g:=fam_fnt(z);
@y
  begin z:=z+s+script_size;
  repeat z:=z-script_size; g:=fam_fnt(z);
@z

@x
  until z<16;
@y
  until z<script_size;
@z

@x
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
function char_box(@!f:internal_font_number;@!c:integer):pointer;
var q:four_quarters;
@!hd:eight_bits; {|height_depth| byte}
@!b,@!p:pointer; {the new box and its character node}
begin
if is_native_font(f) then begin
  b:=new_null_box;
  p:=new_native_character(f, c);
  list_ptr(b):=p;
  height(b):=height(p); width(b):=width(p);
  if depth(p)<0 then depth(b):=0 else depth(b):=depth(p);
  end
else begin
  q:=char_info(f)(c); hd:=height_depth(q);
  b:=new_null_box; width(b):=char_width(f)(q)+char_italic(f)(q);
  height(b):=char_height(f)(hd); depth(b):=char_depth(f)(hd);
  p:=get_avail; character(p):=c; font(p):=f;
  end;
list_ptr(b):=p; char_box:=b;
end;
@z

@x
@* \[36] Typesetting math formulas.
@y
@* \[36] Typesetting math formulas.
@z

@x
@!cur_size:small_number; {size code corresponding to |cur_style|}
@y with 256 families, this can be up to 768
@!cur_size:integer; {size code corresponding to |cur_style|}
@z

@x
@p procedure fetch(@!a:pointer); {unpack the |math_char| field |a|}
begin cur_c:=character(a); cur_f:=fam_fnt(fam(a)+cur_size);
if cur_f=null_font then
  @<Complain about an undefined family and set |cur_i| null@>
else  begin if (qo(cur_c)>=font_bc[cur_f])and(qo(cur_c)<=font_ec[cur_f]) then
    cur_i:=orig_char_info(cur_f)(cur_c)
@y
@p procedure fetch(@!a:pointer); {unpack the |math_char| field |a|}
begin cur_c:=cast_to_ushort(character(a)); cur_f:=fam_fnt(fam(a)+cur_size);
cur_c:=cur_c + (plane_and_fam_field(a) div @"100) * @"10000;
if cur_f=null_font then
  @<Complain about an undefined family and set |cur_i| null@>
else if is_native_font(cur_f) then begin
  cur_i:=null_character;
end else begin if (qo(cur_c)>=font_bc[cur_f])and(qo(cur_c)<=font_ec[cur_f]) then
    cur_i:=orig_char_info(cur_f)(cur_c)
@z

@x
@!cur_c:quarterword; {the |character| field of a |math_char|}
@y
@!cur_c:integer; {the |character| field of a |math_char|}
@z

@x
procedure make_math_accent(@!q:pointer);
label done,done1;
var p,@!x,@!y:pointer; {temporary registers for box construction}
@!a:integer; {address of lig/kern instruction}
@!c:quarterword; {accent character}
@!f:internal_font_number; {its font}
@!i:four_quarters; {its |char_info|}
@!s:scaled; {amount to skew the accent to the right}
@!h:scaled; {height of character being accented}
@!delta:scaled; {space to remove between accent and accentee}
@!w:scaled; {width of the accentee, not including sub/superscripts}
begin fetch(accent_chr(q));
if char_exists(cur_i) then
  begin i:=cur_i; c:=cur_c; f:=cur_f;@/
  @<Compute the amount of skew@>;
  x:=clean_box(nucleus(q),cramped_style(cur_style)); w:=width(x); h:=height(x);
  @<Switch to a larger accent if available and appropriate@>;
@y
procedure make_math_accent(@!q:pointer);
label done,done1;
var p,@!x,@!y:pointer; {temporary registers for box construction}
@!a:integer; {address of lig/kern instruction}
@!c:integer; {accent character}
@!f:internal_font_number; {its font}
@!i:four_quarters; {its |char_info|}
@!s:scaled; {amount to skew the accent to the right}
@!h:scaled; {height of character being accented}
@!delta:scaled; {space to remove between accent and accentee}
@!w:scaled; {width of the accentee, not including sub/superscripts}
begin fetch(accent_chr(q));
x:=null;
if is_native_font(cur_f) then
  begin c:=cur_c; f:=cur_f;
  s:=0; {@<Compute the amount of skew@>;}
  x:=clean_box(nucleus(q),cramped_style(cur_style)); w:=width(x); h:=height(x);
  end
else if char_exists(cur_i) then
  begin i:=cur_i; c:=cur_c; f:=cur_f;@/
  @<Compute the amount of skew@>;
  x:=clean_box(nucleus(q),cramped_style(cur_style)); w:=width(x); h:=height(x);
  @<Switch to a larger accent if available and appropriate@>;
  end;
if x<>null then begin
@z

@x
      character(nucleus(r)):=rem_byte(cur_i);
      fam(nucleus(r)):=fam(nucleus(q));@/
@y
      character(nucleus(r)):=rem_byte(cur_i);
      plane_and_fam_field(nucleus(r)):=fam(nucleus(q));@/
@z

@x
@ @<Create a character node |p| for |nucleus(q)|...@>=
begin fetch(nucleus(q));
if char_exists(cur_i) then
@y
@ @<Create a character node |p| for |nucleus(q)|...@>=
begin fetch(nucleus(q));
if is_native_font(cur_f) then begin
  delta:=0; p:=new_native_character(cur_f, qo(cur_c));
end else if char_exists(cur_i) then
@z

@x
procedure make_scripts(@!q:pointer;@!delta:scaled);
var p,@!x,@!y,@!z:pointer; {temporary registers for box construction}
@!shift_up,@!shift_down,@!clr:scaled; {dimensions in the calculation}
@!t:small_number; {subsidiary size code}
@y
procedure make_scripts(@!q:pointer;@!delta:scaled);
var p,@!x,@!y,@!z:pointer; {temporary registers for box construction}
@!shift_up,@!shift_down,@!clr:scaled; {dimensions in the calculation}
@!t:integer; {subsidiary size code}
@z

@x
magic_offset:=str_start[math_spacing]-9*ord_noad
@y
magic_offset:=str_start_macro(math_spacing)-9*ord_noad
@z

@x
@* \[37] Alignment.
@y
@* \[37] Alignment.
@z

@x
@d span_code=256 {distinct from any character}
@d cr_code=257 {distinct from |span_code| and from any character}
@y
@d span_code=special_char {distinct from any character}
@d cr_code=span_code+1 {distinct from |span_code| and from any character}
@z

@x
if n>max_quarterword then confusion("256 spans"); {this can happen, but won't}
@^system dependencies@>
@:this can't happen 256 spans}{\quad 256 spans@>
@y
if n>max_quarterword then confusion("too many spans");
   {this can happen, but won't}
@^system dependencies@>
@:this can't happen too many spans}{\quad too many spans@>
@z

@x
@* \[38] Breaking paragraphs into lines.
@y
@* \[38] Breaking paragraphs into lines.
@z

@x
label done,done1,done2,done3,done4,done5,continue;
@y
label done,done1,done2,done3,done4,done5,done6,continue, restart;
@z

@x
  othercases confusion("disc1")
@y
  whatsit_node:
    if (subtype(v)=native_word_node)
    or (subtype(v)=glyph_node)
    or (subtype(v)=pic_node)
    or (subtype(v)=pdf_node)
    then break_width[1]:=break_width[1]-width(v)
	else if subtype(v)=deleted_native_node then do_nothing
	else confusion("disc1a");
  othercases confusion("disc1")
@z

@x
  othercases confusion("disc2")
@y
  whatsit_node:
    if (subtype(s)=native_word_node)
    or (subtype(s)=glyph_node)
    or (subtype(s)=pic_node)
    or (subtype(s)=pdf_node)
    then break_width[1]:=break_width[1]+width(s)
	else if subtype(s)=deleted_native_node then do_nothing
	else confusion("disc2a");
  othercases confusion("disc2")
@z

@x
@* \[39] Breaking paragraphs into lines, continued.
@y
@* \[39] Breaking paragraphs into lines, continued.
@z

@x
  othercases confusion("disc3")
@y
  whatsit_node:
    if (subtype(s)=native_word_node)
    or (subtype(s)=glyph_node)
    or (subtype(s)=pic_node)
    or (subtype(s)=pdf_node)
    then disc_width:=disc_width+width(s)
	else if subtype(s)=deleted_native_node then do_nothing
	else confusion("disc3a");
  othercases confusion("disc3")
@z

@x
  othercases confusion("disc4")
@y
  whatsit_node:
    if (subtype(s)=native_word_node)
    or (subtype(s)=glyph_node)
    or (subtype(s)=pic_node)
    or (subtype(s)=pdf_node)
    then act_width:=act_width+width(s)
	else if subtype(s)=deleted_native_node then do_nothing
	else confusion("disc4a");
  othercases confusion("disc4")
@z

@x
@* \[40] Pre-hyphenation.
@y
@* \[40] Pre-hyphenation.
@z

@x
@!hc:array[0..65] of 0..256; {word to be hyphenated}
@y
@!hc:array[0..65] of 0..too_big_char; {word to be hyphenated}
@z

@x
@!hu:array[0..63] of 0..256; {like |hc|, before conversion to lowercase}
@y
@!hu:array[0..63] of 0..too_big_char;
     {like |hc|, before conversion to lowercase}
@z

@x
@!cur_lang,@!init_cur_lang:ASCII_code; {current hyphenation table of interest}
@y
@!cur_lang,@!init_cur_lang:0..biggest_lang;
     {current hyphenation table of interest}
@z

@x
@!hyf_bchar:halfword; {boundary character after $c_n$}
@y
@!hyf_bchar:halfword; {boundary character after $c_n$}
@!max_hyph_char:integer;

@ @<Set initial values of key variables@>=
max_hyph_char:=too_big_lang;
@z

@x
@!c:0..255; {character being considered for hyphenation}
@y
@!c:ASCII_code; {character being considered for hyphenation}
@z

@x
  @<Skip to node |hb|, putting letters into |hu| and |hc|@>;
@y
if (not is_char_node(ha)) and (type(ha) = whatsit_node) and (subtype(ha) = native_word_node) then begin
  @<Check that nodes after |native_word| permit hyphenation; if not, |goto done1|@>;
  @<Prepare a |native_word_node| for hyphenation@>;
end else begin
  @<Skip to node |hb|, putting letters into |hu| and |hc|@>;
end;
@z

@x
@ The first thing we need to do is find the node |ha| just before the
first letter.
@y
@ @<Check that nodes after |native_word| permit hyphenation; if not, |goto done1|@>=
s := link(ha);
loop@+  begin if not(is_char_node(s)) then
    case type(s) of
    ligature_node: do_nothing;
    kern_node: if subtype(s)<>normal then goto done6;
    whatsit_node,glue_node,penalty_node,ins_node,adjust_node,mark_node:
      goto done6;
    othercases goto done1
    endcases;
  s:=link(s);
  end;
done6:

@ @<Prepare a |native_word_node| for hyphenation@>=
{ note that if there are chars with |lccode = 0|, we split them out into separate |native_word| nodes }
hn := 0;
restart:
for l := 0 to native_length(ha)-1 do begin
  c := get_native_char(ha, l);
  set_lc_code(c);
  if (hc[0] = 0) {or (hc[0] > max_hyph_char) -- no, there can be letters > max_hyph_char in the word}
  then begin
    if (hn > 0) then begin
      { we've got some letters, and now found a non-letter, so break off the tail of the |native_word|
        and link it after this node, and goto done3 }
      @<Split the |native_word_node| at |l| and link the second part after |ha|@>;
      goto done3;
    end
  end else if (hn = 0) and (l > 0) then begin
    { we've found the first letter after some non-letters, so break off the head of the |native_word| and restart }
    @<Split the |native_word_node| at |l| and link the second part after |ha|@>;
    ha := link(ha);
    goto restart;
  end else if (hn = 63) then
    { reached max hyphenatable length }
    goto done3
  else begin
    { found a letter that is part of a potentially hyphenatable sequence }
    incr(hn); hu[hn] := c; hc[hn] := hc[0]; hyf_bchar := non_char;
  end
end;

@ @<Split the |native_word_node| at |l| and link the second part after |ha|@>=
  q := new_native_word_node(hf, native_length(ha) - l);
  for i := l to native_length(ha) - 1 do
    set_native_char(q, i - l, get_native_char(ha, i));
  set_native_metrics(q, XeTeX_use_glyph_metrics);
  link(q) := link(ha);
  link(ha) := q;
  { truncate text in node |ha| }
  native_length(ha) := l;
  set_native_metrics(ha, XeTeX_use_glyph_metrics);

@ @<Local variables for line breaking@>=
l: integer;
i: integer;

@ The first thing we need to do is find the node |ha| just before the
first letter.
@z

@x
    begin @<Advance \(p)past a whatsit node in the \(p)pre-hyphenation loop@>;
    goto continue;
@y
    begin
      if subtype(s) = native_word_node then begin
        { we only consider the node if it contains at least one letter, otherwise we'll skip it }
        for l:=0 to native_length(s) - 1 do begin
          c := get_native_char(s, l);
          if lc_code(c) <> 0 then begin
            hf := native_font(s);
            prev_s := s;
            goto done2;
          end
        end
      end;
      @<Advance \(p)past a whatsit node in the \(p)pre-hyphenation loop@>;
      goto continue
@z

@x
if hyf_char>255 then goto done1;
@y
if hyf_char>biggest_char then goto done1;
@z

@x
    if hc[0]=0 then goto done3;
@y
    if hc[0]=0 then goto done3;
    if hc[0]>max_hyph_char then goto done3;
@z

@x
  if hc[0]=0 then goto done3;
@y
  if hc[0]=0 then goto done3;
  if hc[0]>max_hyph_char then goto done3;
@z

@x
@* \[41] Post-hyphenation.
@y
@* \[41] Post-hyphenation.
@z

@x
@<Replace nodes |ha..hb| by a sequence of nodes...@>=
@y
@<Replace nodes |ha..hb| by a sequence of nodes...@>=
if (not is_char_node(ha)) and (type(ha) = whatsit_node) and (subtype(ha) = native_word_node) then begin
 @<Hyphenate the |native_word_node| at |ha|@>;
end else begin
@z

@x
      begin hu[0]:=256; init_lig:=false;
@y
      begin hu[0]:=max_hyph_char; init_lig:=false;
@z

@x
found2: s:=ha; j:=0; hu[0]:=256; init_lig:=false; init_list:=null;
@y
found2: s:=ha; j:=0; hu[0]:=max_hyph_char; init_lig:=false; init_list:=null;
@z

@x
flush_list(init_list)
@y
flush_list(init_list);
end

@ @<Hyphenate the |native_word_node| at |ha|@>=
{ find the node immediately before the word to be hyphenated }
s := cur_p; {we have |cur_p<>ha| because |type(cur_p)=glue_node|}
while link(s) <> ha do s := link(s);

{ for each hyphen position,
  create a |native_word_node| fragment for the text before this point,
  and a |disc_node| for the break, with the |hyf_char| in the |pre_break| text
}

hyphen_passed := 0; { location of last hyphen we saw }

for j := l_hyf to hn - r_hyf do begin
  { if this is a valid break.... }
  if odd(hyf[j]) then begin
  
	{ make a |native_word_node| for the fragment before the hyphen }
	q := new_native_word_node(hf, j - hyphen_passed);
	for i := 0 to j - hyphen_passed - 1 do
	  set_native_char(q, i, get_native_char(ha, i + hyphen_passed));
	set_native_metrics(q, XeTeX_use_glyph_metrics);
	link(s) := q; { append the new node }
	s := q;
	
	{ make the |disc_node| for the hyphenation point }
    q := new_disc;
	pre_break(q) := new_native_character(hf, hyf_char);
	link(s) := q;
	s := q;
	
	hyphen_passed := j;
  end
end;

{ make a |native_word_node| for the last fragment of the word }
hn := native_length(ha); { ensure trailing punctuation is not lost! }
q := new_native_word_node(hf, hn - hyphen_passed);
for i := 0 to hn - hyphen_passed - 1 do
  set_native_char(q, i, get_native_char(ha, i + hyphen_passed));
set_native_metrics(q, XeTeX_use_glyph_metrics);
link(s) := q; { append the new node }
s := q;

q := link(ha);
link(s) := q;
link(ha) := null;
flush_node_list(ha);
@z

@x
  begin decr(l); c:=hu[l]; c_loc:=l; hu[l]:=256;
@y
  begin decr(l); c:=hu[l]; c_loc:=l; hu[l]:=max_hyph_char;
@z

@x
@* \[42] Hyphenation.
@y
@* \[42] Hyphenation.
@z

@x
@!op_start:array[ASCII_code] of 0..trie_op_size; {offset for current language}
@y
@!op_start:array[0..biggest_lang] of 0..trie_op_size; {offset for current language}
@z

@x
hc[0]:=0; hc[hn+1]:=0; hc[hn+2]:=256; {insert delimiters}
@y
hc[0]:=0; hc[hn+1]:=0; hc[hn+2]:=max_hyph_char; {insert delimiters}
@z

@x
  begin j:=1; u:=str_start[k];
@y
  begin j:=1; u:=str_start_macro(k);
@z

@x
  else if language>255 then cur_lang:=0
@y
  else if language>biggest_lang then cur_lang:=0
@z

@x
u:=str_start[k]; v:=str_start[s];
@y
u:=str_start_macro(k); v:=str_start_macro(s);
@z

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

@x
@!trie_used:array[ASCII_code] of trie_opcode;
@y
@!trie_used:array[0..biggest_lang] of trie_opcode;
@z

@x
@!trie_op_lang:array[1..trie_op_size] of ASCII_code;
@y
@!trie_op_lang:array[1..trie_op_size] of 0..biggest_lang;
@z

@x
for j:=1 to 255 do op_start[j]:=op_start[j-1]+qo(trie_used[j-1]);
@y
for j:=1 to biggest_lang do op_start[j]:=op_start[j-1]+qo(trie_used[j-1]);
@z

@x
for k:=0 to 255 do trie_used[k]:=min_trie_op;
@y
for k:=0 to biggest_lang do trie_used[k]:=min_trie_op;
@z

@x
for p:=0 to 255 do trie_min[p]:=p+1;
@y
for p:=0 to biggest_char do trie_min[p]:=p+1;
@z

@x
@!ll:1..256; {upper limit of |trie_min| updating}
@y
@!ll:1..too_big_char; {upper limit of |trie_min| updating}
@z

@x
  @<Ensure that |trie_max>=h+256|@>;
@y
  @<Ensure that |trie_max>=h+max_hyph_char|@>;
@z

@x
@ By making sure that |trie_max| is at least |h+256|, we can be sure that
@y
@ By making sure that |trie_max| is at least |h+max_hyph_char|,
we can be sure that
@z

@x
@<Ensure that |trie_max>=h+256|@>=
if trie_max<h+256 then
  begin if trie_size<=h+256 then overflow("pattern memory",trie_size);
@y
@<Ensure that |trie_max>=h+max_hyph_char|@>=
if trie_max<h+max_hyph_char then
  begin if trie_size<=h+max_hyph_char then overflow("pattern memory",trie_size);
@z

@x
  until trie_max=h+256;
@y
  until trie_max=h+max_hyph_char;
@z

@x
if l<256 then
  begin if z<256 then ll:=z @+else ll:=256;
@y
if l<max_hyph_char then
  begin if z<max_hyph_char then ll:=z @+else ll:=max_hyph_char;
@z

@x
  begin for r:=0 to 256 do clear_trie;
  trie_max:=256;
@y
  begin for r:=0 to max_hyph_char do clear_trie;
  trie_max:=max_hyph_char;
@z

@x
  if k<63 then
@y
    if cur_chr>max_hyph_char then max_hyph_char:=cur_chr;
  if k<63 then
@z

@x
begin @<Get ready to compress the trie@>;
@y
begin
incr(max_hyph_char);
@<Get ready to compress the trie@>;
@z

@x
@* \[44] Breaking vertical lists into pages.
@y
@* \[44] Breaking vertical lists into pages.
@z

@x
@* \[45] The page builder.
@y
@* \[45] The page builder.
@z

@x
@!n:min_quarterword..255; {insertion box number}
@y
@!n:min_quarterword..biggest_reg; {insertion box number}
@z

@x
@!n:min_quarterword..255; {insertion box number}
@y
@!n:min_quarterword..biggest_reg; {insertion box number}
@z

@x
@* \[46] The chief executive.
@y
@* \[46] The chief executive.
@z

@x
@d main_loop=70 {go here to typeset a string of consecutive characters}
@y
@d main_loop=70 {go here to typeset a string of consecutive characters}
@d collect_native=71 {go here to collect characters in a "native" font string}
@z

@x
hmode+char_num: begin scan_char_num; cur_chr:=cur_val; goto main_loop;@+end;
@y
hmode+char_num: begin scan_usv_num; cur_chr:=cur_val; goto main_loop;@+end;
@z

@x
@!main_p:pointer; {temporary register for list manipulation}
@y
@!main_p:pointer; {temporary register for list manipulation}
@!main_pp:pointer; {another temporary register for list manipulation}
@!main_h:pointer; {temp for hyphen offset in native-font text}
@!is_hyph:boolean; {whether the last char seen is the font's hyphenchar}
@z

@x
adjust_space_factor;@/
@y

{ added code for native font support }
if is_native_font(cur_font) then begin
	if mode>0 then if language<>clang then fix_language;

	main_h := 0;
	main_f := cur_font;

collect_native:
	adjust_space_factor;
	if (cur_chr > @"FFFF) then begin
		str_room(2);
		append_char((cur_chr - @"10000) div 1024 + @"D800);
		append_char((cur_chr - @"10000) mod 1024 + @"DC00);
	end else begin
		str_room(1);
		append_char(cur_chr);
	end;
	is_hyph := (cur_chr = hyphen_char[main_f])
		or (XeTeX_dash_break_en and (cur_chr = @"2014) or (cur_chr = @"2013));
	if (main_h = 0) and is_hyph then main_h := cur_length;

	{try to collect as many chars as possible in the same font}
	get_next;
	if (cur_cmd=letter) or (cur_cmd=other_char) or (cur_cmd=char_given) then goto collect_native;
	x_token;
	if (cur_cmd=letter) or (cur_cmd=other_char) or (cur_cmd=char_given) then goto collect_native;
	if cur_cmd=char_num then begin
		scan_usv_num;
		cur_chr:=cur_val;
		goto collect_native;
	end;

	if (font_mapping[main_f] <> 0) then begin
		main_k := apply_mapping(font_mapping[main_f], address_of(str_pool[str_start_macro(str_ptr)]), cur_length);
		pool_ptr := str_start_macro(str_ptr); { flush the string, as we'll be using the mapped text instead }
		str_room(main_k);
		main_h := 0;
		for main_p := 0 to main_k - 1 do begin
			append_char(mapped_text[main_p]);
			if (main_h = 0) and ((mapped_text[main_p] = hyphen_char[main_f])
				or (XeTeX_dash_break_en and ((mapped_text[main_p] = @"2014) or (mapped_text[main_p] = @"2013)) ) )
			then main_h := cur_length;
		end
	end;

	if tracing_lost_chars > 0 then begin
		temp_ptr := str_start_macro(str_ptr);
		main_p := temp_ptr + cur_length;
		while (temp_ptr < main_p) do begin
			main_k := str_pool[temp_ptr];
			incr(temp_ptr);
			if (main_k >= @"D800) and (main_k < @"DC00) then begin
				main_k := @"10000 + (main_k - @"D800) * 1024;
				main_k := main_k + str_pool[temp_ptr] - @"DC00;
				incr(temp_ptr);
			end;
			if map_char_to_glyph(main_f, main_k) = 0 then
				char_warning(main_f, main_k);
		end
	end;

	main_k := cur_length;
	main_pp := tail;

	if mode=hmode then begin

		temp_ptr := str_start_macro(str_ptr);
		repeat
			if main_h = 0 then main_h := main_k;

			if (not is_char_node(main_pp)) and (type(main_pp)=whatsit_node) and (subtype(main_pp)=native_word_node) and (native_font(main_pp)=main_f) then begin

				{ make a new temp string that contains the concatenated text of |tail| + the current word/fragment }
				main_k := main_h + native_length(main_pp);
				str_room(main_k);
				
				temp_ptr := pool_ptr;
				for main_p := 0 to native_length(main_pp) - 1 do
					append_char(get_native_char(main_pp, main_p));
				for main_p := str_start_macro(str_ptr) to temp_ptr - 1 do
					append_char(str_pool[main_p]);

				do_locale_linebreaks(temp_ptr, main_k);

				pool_ptr := temp_ptr;	{ discard the temp string }
				main_k := cur_length - main_h;	{ and set main_k to remaining length of new word }
				temp_ptr := str_start_macro(str_ptr) + main_h;	{ pointer to remaining fragment }

				main_h := 0;
				while (main_h < main_k) and (str_pool[temp_ptr + main_h] <> hyphen_char[main_f])
					and ( (not XeTeX_dash_break_en)
						or ((str_pool[temp_ptr + main_h] <> @"2014) and (str_pool[temp_ptr + main_h] <> @"2013)) )
				do incr(main_h);	{ look for next hyphen or end of text }
				if (main_h < main_k) then incr(main_h);

				{ flag the previous node as no longer valid }
				free_native_glyph_info(main_pp);
				subtype(main_pp) := deleted_native_node;

			end else begin

				do_locale_linebreaks(temp_ptr, main_h);	{ append fragment of current word }

				temp_ptr := temp_ptr + main_h;	{ advance ptr to remaining fragment }
				main_k := main_k - main_h;	{ decrement remaining length }

				main_h := 0;
				while (main_h < main_k) and (str_pool[temp_ptr + main_h] <> hyphen_char[main_f])
					and ( (not XeTeX_dash_break_en)
						or ((str_pool[temp_ptr + main_h] <> @"2014) and (str_pool[temp_ptr + main_h] <> @"2013)) )
				do incr(main_h);	{ look for next hyphen or end of text }
				if (main_h < main_k) then incr(main_h);

			end;
			
			if (main_k > 0) or is_hyph then begin
				tail_append(new_disc);	{ add a break if we aren't at end of text (must be a hyphen),
											or if last char in original text was a hyphen }
			end;
		until main_k = 0;
		
	end else begin
		{ must be restricted hmode, so no need for line-breaking or discretionaries }
		if (not is_char_node(main_pp)) and (type(main_pp)=whatsit_node) and (subtype(main_pp)=native_word_node) and (native_font(main_pp)=main_f) then begin
			{ total string length for the new merged whatsit }
			link(main_pp) := new_native_word_node(main_f, main_k + native_length(main_pp));
			tail := link(main_pp);
	
			{ copy text from the old one into the new }
			for main_p := 0 to native_length(main_pp) - 1 do
				set_native_char(tail, main_p, get_native_char(main_pp, main_p));
			{ append the new text }
			for main_p := 0 to main_k - 1 do
				set_native_char(tail, main_p + native_length(main_pp), str_pool[str_start_macro(str_ptr) + main_p]);
			set_native_metrics(tail, XeTeX_use_glyph_metrics);

			{ flag the previous node as no longer valid }
			free_native_glyph_info(main_pp);
			subtype(main_pp) := deleted_native_node;
		end else begin
			{ package the current string into a |native_word| whatsit }
			link(main_pp) := new_native_word_node(main_f, main_k);
			tail := link(main_pp);
			for main_p := 0 to main_k - 1 do
				set_native_char(tail, main_p, str_pool[str_start_macro(str_ptr) + main_p]);
			set_native_metrics(tail, XeTeX_use_glyph_metrics);
		end
	end;
	
	pool_ptr := str_start_macro(str_ptr);
	goto reswitch;
end;
{ End of added code for native fonts }

adjust_space_factor;@/
@z

@x
non_math(math_given), non_math(math_comp), non_math(delim_num),
@y
non_math(math_given), non_math(XeTeX_math_given), non_math(math_comp), non_math(delim_num),
@z

@x
procedure append_italic_correction;
label exit;
var p:pointer; {|char_node| at the tail of the current list}
@!f:internal_font_number; {the font in the |char_node|}
begin if tail<>head then
  begin if is_char_node(tail) then p:=tail
  else if type(tail)=ligature_node then p:=lig_char(tail)
  else return;
@y
procedure append_italic_correction;
label exit;
var p:pointer; {|char_node| at the tail of the current list}
@!f:internal_font_number; {the font in the |char_node|}
begin if tail<>head then
  begin if is_char_node(tail) then p:=tail
  else if type(tail)=ligature_node then p:=lig_char(tail)
  else if (type(tail)=whatsit_node) then begin
    if (subtype(tail)=native_word_node) then begin
      tail_append(new_kern(get_native_italic_correction(tail))); subtype(tail):=explicit;
    end
    else if (subtype(tail)=glyph_node) then begin
      tail_append(new_kern(get_native_glyph_italic_correction(tail))); subtype(tail):=explicit;
    end;
    return;
  end
  else return;
@z

@x
  if c>=0 then if c<256 then pre_break(tail):=new_character(cur_font,c);
@y
  if c>=0 then if c<=biggest_char then pre_break(tail):=new_character(cur_font,c);
@z

@x
    if type(p)<>kern_node then if type(p)<>ligature_node then
      begin print_err("Improper discretionary list");
@y
    if type(p)<>kern_node then if type(p)<>ligature_node then
	if (type(p)<>whatsit_node) or ((subtype(p)<>native_word_node)
									 and (subtype(p)<>deleted_native_node)
									 and (subtype(p)<>glyph_node)) then
      begin print_err("Improper discretionary list");
@z

@x
@!a,@!h,@!x,@!w,@!delta:scaled; {heights and widths, as explained above}
@y
@!a,@!h,@!x,@!w,@!delta,@!lsb,@!rsb:scaled; {heights and widths, as explained above}
@z

@x
  a:=char_width(f)(char_info(f)(character(p)));@/
@y
  if is_native_font(f) then
    begin a:=width(p);
    if a=0 then get_native_char_sidebearings(f, cur_val, address_of(lsb), address_of(rsb))
    end
  else a:=char_width(f)(char_info(f)(character(p)));@/
@z

@x
if (cur_cmd=letter)or(cur_cmd=other_char)or(cur_cmd=char_given) then
  q:=new_character(f,cur_chr)
@y
if (cur_cmd=letter)or(cur_cmd=other_char)or(cur_cmd=char_given) then
  begin q:=new_character(f,cur_chr); cur_val:=cur_chr
  end
@z

@x
i:=char_info(f)(character(q));
w:=char_width(f)(i); h:=char_height(f)(height_depth(i));
@y
if is_native_font(f) then begin
  w:=width(q);
  get_native_char_height_depth(f, cur_val, address_of(h), address_of(delta))
    {using delta as scratch space for the unneeded depth value}
end else begin
  i:=char_info(f)(character(q));
  w:=char_width(f)(i); h:=char_height(f)(height_depth(i))
end;
@z

@x
delta:=round((w-a)/float_constant(2)+h*t-x*s);
@y
if is_native_font(f) and (a=0) then { special case for non-spacing marks }
  delta:=round((w-lsb+rsb)/float_constant(2)+h*t-x*s)
else delta:=round((w-a)/float_constant(2)+h*t-x*s);
@z

@x
whatsit_node: @<Let |d| be the width of the whatsit |p|@>;
@y
whatsit_node: @<Let |d| be the width of the whatsit |p|, and |goto found| if ``visible''@>;
@z

@x
letter,other_char,char_given: begin c:=ho(math_code(cur_chr));
    if c=@'100000 then
@y
letter,other_char,char_given: begin c:=ho(math_code(cur_chr));
    if is_active_math_char(c) then
@z

@x
math_char_num: begin scan_fifteen_bit_int; c:=cur_val;
  end;
@y
math_char_num:
  if cur_chr = 2 then begin { \XeTeXmathchar }
    scan_math_class_int; c := set_class_field(cur_val);
    scan_math_fam_int;   c := c + set_family_field(cur_val);
    scan_usv_num;        c := c + cur_val;
  end else if cur_chr = 1 then begin { \XeTeXmathcharnum }
    scan_xetex_math_char_int; c := cur_val;
  end else begin scan_fifteen_bit_int;
    c := set_class_field(cur_val div @"1000) +
           set_family_field((cur_val mod @"1000) div @"100) +
           (cur_val mod @"100);
  end;
@z

@x
math_given: c:=cur_chr;
delim_num: begin scan_twenty_seven_bit_int; c:=cur_val div @'10000;
@y
math_given: begin
  c := set_class_field(cur_chr div @"1000) +
       set_family_field((cur_chr mod @"1000) div @"100) +
       (cur_chr mod @"100);
  end;
XeTeX_math_given: c:=cur_chr;
delim_num: begin
  if cur_chr=1 then begin {\XeTeXdelimiter <cls> <fam> <usv>}
    scan_math_class_int; c := set_class_field(cur_val);
    scan_math_fam_int;   c := c + set_family_field(cur_val);
    scan_usv_num;        c := c + cur_val;
  end else begin {\delimiter <27-bit delcode>}
    scan_delimiter_int;
    c := cur_val div @'10000; {get the 'small' delimiter field}
    c := set_class_field(c div @"1000) +
       set_family_field((c mod @"1000) div @"100) +
       (c mod @"100); {and convert it to a xetex mathchar code}
  end;
@z

@x
math_type(p):=math_char; character(p):=qi(c mod 256);
if (c>=var_code)and fam_in_range then fam(p):=cur_fam
else fam(p):=(c div 256) mod 16;
@y
math_type(p):=math_char; character(p):=qi(c mod @"10000);
if (is_var_family(c)) and fam_in_range then plane_and_fam_field(p):=cur_fam
else plane_and_fam_field(p):=(math_fam_field(c));
plane_and_fam_field(p) := plane_and_fam_field(p) + (math_char_field(c) div @"10000) * @"100;
@z

@x
mmode+math_char_num: begin scan_fifteen_bit_int; set_math_char(cur_val);
  end;
@y
mmode+math_char_num: if cur_chr = 2 then begin { \XeTeXmathchar }
    scan_math_class_int; t := set_class_field(cur_val);
    scan_math_fam_int; t := t + set_family_field(cur_val);
    scan_usv_num; t := t + cur_val;
    set_math_char(t);
  end else if cur_chr = 1 then begin { \XeTeXmathcharnum }
    scan_xetex_math_char_int; set_math_char(cur_val);
  end else begin scan_fifteen_bit_int;
    set_math_char(set_class_field(cur_val div @"1000) +
           set_family_field((cur_val mod @"1000) div @"100) +
           (cur_val mod @"100));
  end;
@z

@x
mmode+math_given: set_math_char(cur_chr);
mmode+delim_num: begin scan_twenty_seven_bit_int;
  set_math_char(cur_val div @'10000);
@y
mmode+math_given: begin
  set_math_char(set_class_field(cur_chr div @"1000) +
                set_family_field((cur_chr mod @"1000) div @"100) +
                (cur_chr mod @"100));
  end;
mmode+XeTeX_math_given: set_math_char(cur_chr);
mmode+delim_num: begin
  if cur_chr=1 then begin {\XeTeXdelimiter}
    scan_math_class_int; t := set_class_field(cur_val);
    scan_math_fam_int; t := t + set_family_field(cur_val);
    scan_usv_num; t := t + cur_val;
    set_math_char(t);
  end else begin
    scan_delimiter_int;
    cur_val:=cur_val div @'10000; {discard the large delimiter code}
    set_math_char(set_class_field(cur_val div @"1000) +
         set_family_field((cur_val mod @"1000) div @"100) +
         (cur_val mod @"100));
  end;
@z

@x
procedure set_math_char(@!c:integer);
var p:pointer; {the new noad}
begin if c>=@'100000 then
  @<Treat |cur_chr|...@>
else  begin p:=new_noad; math_type(nucleus(p)):=math_char;
  character(nucleus(p)):=qi(c mod 256);
  fam(nucleus(p)):=(c div 256) mod 16;
  if c>=var_code then
    begin if fam_in_range then fam(nucleus(p)):=cur_fam;
    type(p):=ord_noad;
    end
  else  type(p):=ord_noad+(c div @'10000);
@y
procedure set_math_char(@!c:integer);
var p,q,r:pointer; {the new noad}
  ch: UnicodeScalar;
begin if is_active_math_char(c) then
  @<Treat |cur_chr|...@>
else  begin p:=new_noad; math_type(nucleus(p)):=math_char;
  ch:=math_char_field(c);
  character(nucleus(p)):=qi(ch mod @"10000);
  plane_and_fam_field(nucleus(p)):=math_fam_field(c);
  if is_var_family(c) then
    begin if fam_in_range then plane_and_fam_field(nucleus(p)):=cur_fam;
    type(p):=ord_noad;
    end
  else  type(p):=ord_noad+math_class_field(c);
  plane_and_fam_field(nucleus(p)) := plane_and_fam_field(nucleus(p)) + (ch div @"10000) * @"100;
@z

@x
procedure scan_delimiter(@!p:pointer;@!r:boolean);
begin if r then scan_twenty_seven_bit_int
@y
procedure scan_delimiter(@!p:pointer;@!r:boolean);
begin
  if r then begin
    if cur_chr=1 then begin {\XeTeXradical}
      cur_val1 := @"40000000; {extended delcode flag}
      scan_math_fam_int;   cur_val1 := cur_val1 + cur_val * @"200000;
      scan_usv_num;        cur_val := cur_val1 + cur_val;
    end else {\radical}
      scan_delimiter_int;
  end
@z

@x
  letter,other_char: cur_val:=del_code(cur_chr);
  delim_num: scan_twenty_seven_bit_int;
  othercases cur_val:=-1
@y
  letter,other_char: begin
    cur_val:=del_code(cur_chr);
    end;
  delim_num: if cur_chr=1 then begin {\XeTeXdelimiter}
    cur_val1 := @"40000000; {extended delcode flag}
    scan_math_class_int; {discarded}
    scan_math_fam_int;   cur_val1 := cur_val1 + cur_val * @"200000;
    scan_usv_num;        cur_val := cur_val1 + cur_val;
  end else scan_delimiter_int; {normal \delimiter}
  othercases begin cur_val:=-1; end;
@z

@x
if cur_val<0 then @<Report that an invalid delimiter code is being changed
   to null; set~|cur_val:=0|@>;
small_fam(p):=(cur_val div @'4000000) mod 16;
small_char(p):=qi((cur_val div @'10000) mod 256);
large_fam(p):=(cur_val div 256) mod 16;
large_char(p):=qi(cur_val mod 256);
@y
if cur_val<0 then begin @<Report that an invalid delimiter code is being changed
   to null; set~|cur_val:=0|@>;
  end;
if cur_val>=@"40000000 then begin {extended delcode, only one size}
  small_plane_and_fam_field(p) := ((cur_val mod @"200000) div @"10000) * @"100 {plane}
                                  + (cur_val div @"200000) mod @"100; {family}
  small_char_field(p) := qi(cur_val mod @"10000);
  large_plane_and_fam_field(p) := 0;
  large_char_field(p) := 0;
end else begin {standard delcode, 4-bit families and 8-bit char codes}
  small_plane_and_fam_field(p) := (cur_val div @'4000000) mod 16;
  small_char_field(p) := qi((cur_val div @'10000) mod 256);
  large_plane_and_fam_field(p) := (cur_val div 256) mod 16;
  large_char_field(p) := qi(cur_val mod 256);
end;
@z

@x
procedure math_ac;
@y
procedure math_ac;
var c: integer;
@z

@x
scan_fifteen_bit_int;
character(accent_chr(tail)):=qi(cur_val mod 256);
if (cur_val>=var_code)and fam_in_range then fam(accent_chr(tail)):=cur_fam
else fam(accent_chr(tail)):=(cur_val div 256) mod 16;
@y
if cur_chr=1 then begin
  scan_math_class_int; c := set_class_field(cur_val);
  scan_math_fam_int;   c := c + set_family_field(cur_val);
  scan_usv_num;        cur_val := cur_val + c;
end
else begin
  scan_fifteen_bit_int;
  cur_val := set_class_field(cur_val div @"1000) +
             set_family_field((cur_val mod @"1000) div @"100) +
             (cur_val mod @"100);
end;
character(accent_chr(tail)):=qi(cur_val mod @"10000);
if (is_var_family(cur_val))and fam_in_range then plane_and_fam_field(accent_chr(tail)):=cur_fam
else plane_and_fam_field(accent_chr(tail)):=math_fam_field(cur_val);
plane_and_fam_field(accent_chr(tail))
  := plane_and_fam_field(accent_chr(tail)) + (math_char_field(cur_val) div @"10000) * @"100;
@z

@x
@* \[49] Mode-independent processing.
@y
@* \[49] Mode-independent processing.
@z

@x
any_mode(def_code),
@y
any_mode(def_code),
any_mode(XeTeX_def_code),
@z

@x
@d word_define(#)==if global then geq_word_define(#)@+else eq_word_define(#)
@y
@d word_define(#)==if global then geq_word_define(#)@+else eq_word_define(#)
@d word_define1(#)==if global then geq_word_define1(#)@+else eq_word_define1(#)
@z

@x
@d char_def_code=0 {|shorthand_def| for \.{\\chardef}}
@d math_char_def_code=1 {|shorthand_def| for \.{\\mathchardef}}
@d count_def_code=2 {|shorthand_def| for \.{\\countdef}}
@d dimen_def_code=3 {|shorthand_def| for \.{\\dimendef}}
@d skip_def_code=4 {|shorthand_def| for \.{\\skipdef}}
@d mu_skip_def_code=5 {|shorthand_def| for \.{\\muskipdef}}
@d toks_def_code=6 {|shorthand_def| for \.{\\toksdef}}
@d char_sub_def_code=7 {|shorthand_def| for \.{\\charsubdef}}
@y
@d char_def_code=0 {|shorthand_def| for \.{\\chardef}}
@d math_char_def_code=1 {|shorthand_def| for \.{\\mathchardef}}
@d count_def_code=2 {|shorthand_def| for \.{\\countdef}}
@d dimen_def_code=3 {|shorthand_def| for \.{\\dimendef}}
@d skip_def_code=4 {|shorthand_def| for \.{\\skipdef}}
@d mu_skip_def_code=5 {|shorthand_def| for \.{\\muskipdef}}
@d toks_def_code=6 {|shorthand_def| for \.{\\toksdef}}
@d char_sub_def_code=7 {|shorthand_def| for \.{\\charsubdef}}
@d XeTeX_math_char_num_def_code=8
@d XeTeX_math_char_def_code=9
@z

@x
primitive("mathchardef",shorthand_def,math_char_def_code);@/
@!@:math_char_def_}{\.{\\mathchardef} primitive@>
@y
primitive("mathchardef",shorthand_def,math_char_def_code);@/
primitive("XeTeXmathcharnumdef",shorthand_def,XeTeX_math_char_num_def_code);@/
primitive("XeTeXmathchardef",shorthand_def,XeTeX_math_char_def_code);@/
@!@:math_char_def_}{\.{\\mathchardef} primitive@>
@z

@x
  math_char_def_code: print_esc("mathchardef");
@y
  math_char_def_code: print_esc("mathchardef");
  XeTeX_math_char_def_code: print_esc("XeTeXmathchardef");
  XeTeX_math_char_num_def_code: print_esc("XeTeXmathcharnumdef");
@z

@x
math_given: begin print_esc("mathchar"); print_hex(chr_code);
  end;
@y
math_given: begin print_esc("mathchar"); print_hex(chr_code);
  end;
XeTeX_math_given: begin print_esc("XeTeXmathchar"); print_hex(chr_code);
  end;
@z

@x
else begin n:=cur_chr; get_r_token; p:=cur_cs; define(p,relax,256);
@y
else begin n:=cur_chr; get_r_token; p:=cur_cs; define(p,relax,too_big_char);
@z

@x
  char_def_code: begin scan_char_num; define(p,char_given,cur_val);
@y
  char_def_code: begin scan_usv_num; define(p,char_given,cur_val);
@z

@x
  math_char_def_code: begin scan_fifteen_bit_int; define(p,math_given,cur_val);
    end;
@y
  math_char_def_code: begin scan_fifteen_bit_int; define(p,math_given,cur_val);
    end;
  XeTeX_math_char_num_def_code: begin scan_xetex_math_char_int;
    define(p, XeTeX_math_given, cur_val);
    end;
  XeTeX_math_char_def_code: begin
      scan_math_class_int; n := set_class_field(cur_val);
      scan_math_fam_int;   n := n + set_family_field(cur_val);
      scan_usv_num;        n := n + cur_val;
      define(p, XeTeX_math_given, n);
    end;
@z

@x
primitive("mathcode",def_code,math_code_base);
@y
primitive("mathcode",def_code,math_code_base);
primitive("XeTeXmathcodenum",XeTeX_def_code,math_code_base);
primitive("XeTeXmathcode",XeTeX_def_code,math_code_base+1);
@z

@x
primitive("delcode",def_code,del_code_base);
@y
primitive("delcode",def_code,del_code_base);
primitive("XeTeXdelcodenum",XeTeX_def_code,del_code_base);
primitive("XeTeXdelcode",XeTeX_def_code,del_code_base+1);
@z

@x
def_family: print_size(chr_code-math_font_base);
@y
XeTeX_def_code: if chr_code=math_code_base then print_esc("XeTeXmathcodenum")
  else if chr_code=math_code_base+1 then print_esc("XeTeXmathcode")
  else if chr_code=del_code_base then print_esc("XeTeXdelcodenum")
  else print_esc("XeTeXdelcode");
def_family: print_size(chr_code-math_font_base);
@z

@x
def_code: begin @<Let |n| be the largest legal code value, based on |cur_chr|@>;
@y
XeTeX_def_code: begin
    if cur_chr = math_code_base then begin
      p:=cur_chr; scan_char_num;
      p:=p+cur_val;
      scan_optional_equals;
      scan_xetex_math_char_int;
      define(p,data,hi(cur_val));
    end
    else if cur_chr = math_code_base+1 then begin
      p:=cur_chr-1; scan_char_num;
      p:=p+cur_val;
      scan_optional_equals;
      scan_math_class_int; n := set_class_field(cur_val);
      scan_math_fam_int;   n := n + set_family_field(cur_val);
      scan_usv_num;        n := n + cur_val;
      define(p,data,hi(n));
    end
    else if cur_chr = del_code_base then begin
      p:=cur_chr; scan_char_num;
      p:=p+cur_val;
      scan_optional_equals;
      scan_int; {scan_xetex_del_code_int; !!FIXME!!}
      word_define(p,hi(cur_val));
    end else begin
{
bit usage in delcode values:
original layout: @"00cffCFF  small/LARGE family & char
extended:        @"40000000                      FLAG
                 +  ff << 21 (mult by @"200000)  FAMILY
                 +   1ccccc (21 bits)            USV
}
      p:=cur_chr-1; scan_char_num;
      p:=p+cur_val;
      scan_optional_equals;
      n := @"40000000; {extended delcode flag}
      scan_math_fam_int;   n := n + cur_val * @"200000;
      scan_usv_num;        n := n + cur_val;
      word_define(p,hi(n));
    end;
  end;
def_code: begin @<Let |n| be the largest legal code value, based on |cur_chr|@>;
@z

@x
  if p<256 then xord[p]:=cur_val
  else if p<512 then xchr[p-256]:=cur_val
  else if p<768 then xprn[p-512]:=cur_val
  else if p<math_code_base then define(p,data,cur_val)
  else if p<del_code_base then define(p,data,hi(cur_val))
@y
  if p<math_code_base then define(p,data,cur_val)
  else if p<del_code_base then begin
    if cur_val=@"8000 then cur_val:=active_math_char
    else cur_val:=set_class_field(cur_val div @"1000) +
                  set_family_field((cur_val mod @"1000) div @"100) +
                  (cur_val mod @"100); {!!FIXME!! check how this is used}
    define(p,data,hi(cur_val));
    end
@z

-- from Omega; not needed with new xetex delimiter coding
 x
  else word_define(p,cur_val);
 y
  else begin
    cur_val1 := cur_val mod @"1000; { large delim code }
    cur_val1 := set_family_field(cur_val1 div @"100) + cur_val1 mod @"100;
    cur_val := cur_val div @"1000;
    cur_val := set_class_field((cur_val div @"1000) mod 8) +
               set_family_field((cur_val div @"100) mod @"10) +
               (cur_val mod @"100);
    word_define(p, cur_val);
    word_define1(p, cur_val1);
  end;
 z

@x
else n:=255
@y
else n:=biggest_char
@z

@x
def_family: begin p:=cur_chr; scan_four_bit_int; p:=p+cur_val;
@y
def_family: begin p:=cur_chr; scan_math_fam_int; p:=p+cur_val;
@z

@x
  if str_eq_str(font_name[f],cur_name)and str_eq_str(font_area[f],cur_area) then
@y
  if str_eq_str(font_name[f],cur_name) and
    (((cur_area = "") and is_native_font(f)) or str_eq_str(font_area[f],cur_area)) then
@z

@x
set_font:begin print("select font "); slow_print(font_name[chr_code]);
@y
set_font:begin print("select font ");
  font_name_str:=font_name[chr_code];
  if is_native_font(chr_code) then begin
    quote_char:="""";
    for n:=0 to length(font_name_str) - 1 do
     if str_pool[str_start_macro(font_name_str) + n] = """" then quote_char:="'";
    print_char(quote_char);
    slow_print(font_name_str);
    print_char(quote_char);
  end else
    slow_print(font_name_str);
@z

@x
  begin a_close(read_file[n]); read_open[n]:=closed;
@y
  begin u_close(read_file[n]); read_open[n]:=closed;
@z

@x
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
@!c:eight_bits; {character code}
@y
@!c:ASCII_code; {character code}
@z

@x
  begin c:=t mod 256;
@y
  begin c:=t mod max_char_val;
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
format_engine:=xmalloc_array(text_char,x+4);
@y
format_engine:=xmalloc_array(char,x+4);
@z

@x
format_engine:=xmalloc_array(text_char, x);
@y
format_engine:=xmalloc_array(char, x);
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
  if is_native_font(k) then
    begin print_file_name(font_name[k],"","");
    print_err("Can't \dump a format with preloaded native fonts");
    help2("You really, really don't want to do this.")
    ("It won't work, and only confuses me.");
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
dump_int(trie_op_ptr);
@y
dump_int(max_hyph_char);
dump_int(trie_op_ptr);
@z

@x
for k:=255 downto 0 do if trie_used[k]>min_quarterword then
@y
for k:=biggest_lang downto 0 do if trie_used[k]>min_quarterword then
@z

@x
undump_size(0)(trie_op_size)('trie op size')(j); @+init trie_op_ptr:=j;@+tini
@y
undump_int(max_hyph_char);
undump_size(0)(trie_op_size)('trie op size')(j); @+init trie_op_ptr:=j;@+tini
@z

@x
init for k:=0 to 255 do trie_used[k]:=min_quarterword;@+tini@;@/
k:=256;
@y
init for k:=0 to biggest_lang do trie_used[k]:=min_quarterword;@+tini@;@/
k:=biggest_lang+1;
@z

@x
  setup_bound_var (15000)('max_strings')(max_strings);
@y
  setup_bound_var (15000)('max_strings')(max_strings);
  max_strings:=max_strings+too_big_char; {the max_strings value doesn't include the 64K synthetic strings}
@z

@x
  input_file:=xmalloc_array (alpha_file, max_in_open);
@y
  input_file:=xmalloc_array (unicode_file, max_in_open);
@z

@x
    print_file_name(0, log_name, 0); print_char(".");
@y
    print(log_name); print_char(".");
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
@* \[53] Extensions.
@y
@* \[53] Extensions.
@z

@x
@d write_stream(#) == type(#+1) {stream number (0 to 17)}
@d mubyte_zero == 64
@d write_mubyte(#) == subtype(#+1) {mubyte value + |mubyte_zero|}
@y
@d write_stream(#) == info(#+1) {stream number (0 to 17)}
@z

@x
@d set_language_code=5 {command modifier for \.{\\setlanguage}}
@y
@d set_language_code=5 {command modifier for \.{\\setlanguage}}

@d pdftex_first_extension_code = 6
@d pdf_save_pos_node           == pdftex_first_extension_code + 0

@d pic_file_code=41 { command modifier for \.{\\XeTeXpicfile}, skipping codes pdfTeX might use }
@d pdf_file_code=42 { command modifier for \.{\\XeTeXpdffile} }
@d glyph_code=43 { command modifier for \.{\\XeTeXglyph} }

@d XeTeX_input_encoding_extension_code=44
@d XeTeX_default_encoding_extension_code=45
@d XeTeX_linebreak_locale_extension_code=46
@z

@x
@!@:set_language_}{\.{\\setlanguage} primitive@>
@y
@!@:set_language_}{\.{\\setlanguage} primitive@>

@ The \.{\\XeTeXpicfile} and \.{\\XeTeXpdffile} primitives are only defined in extended mode.

@<Generate all \eTeX\ primitives@>=
primitive("XeTeXpicfile",extension,pic_file_code);@/
primitive("XeTeXpdffile",extension,pdf_file_code);@/
primitive("XeTeXglyph",extension,glyph_code);@/
primitive("XeTeXlinebreaklocale", extension, XeTeX_linebreak_locale_extension_code);@/

primitive("pdfsavepos",extension,pdf_save_pos_node);@/
@z

@x
  set_language_code:print_esc("setlanguage");
@y
  set_language_code:print_esc("setlanguage");
  pic_file_code:print_esc("XeTeXpicfile");
  pdf_file_code:print_esc("XeTeXpdffile");
  glyph_code:print_esc("XeTeXglyph");
  XeTeX_linebreak_locale_extension_code:print_esc("XeTeXlinebreaklocale");

  pdf_save_pos_node: print_esc("pdfsavepos");
@z

@x
set_language_code:@<Implement \.{\\setlanguage}@>;
@y
set_language_code:@<Implement \.{\\setlanguage}@>;
pic_file_code:@<Implement \.{\\XeTeXpicfile}@>;
pdf_file_code:@<Implement \.{\\XeTeXpdffile}@>;
glyph_code:@<Implement \.{\\XeTeXglyph}@>;
XeTeX_input_encoding_extension_code:@<Implement \.{\\XeTeXinputencoding}@>;
XeTeX_default_encoding_extension_code:@<Implement \.{\\XeTeXdefaultencoding}@>;
XeTeX_linebreak_locale_extension_code:@<Implement \.{\\XeTeXlinebreaklocale}@>;

pdf_save_pos_node: @<Implement \.{\\pdfsavepos}@>;
@z

@x
@ @<Display the whatsit...@>=
@y
procedure print_native_word(@!p:pointer);
var i:integer;
begin
	for i:=0 to native_length(p) - 1 do print_char(get_native_char(p,i));
end;

@ @<Display the whatsit...@>=
@z

@x
if write_stream(p) <> mubyte_zero then
begin
  print_char ("<"); print_int (write_stream(p)-mubyte_zero);
  if (write_stream(p)-mubyte_zero = 2) or
     (write_stream(p)-mubyte_zero = 3) then
  begin
    print_char (":"); print_int (write_mubyte(p)-mubyte_zero);
  end;
  print_char (">");
end;
@y
@z

@x
othercases print("whatsit?")
@y
native_word_node:begin
	print_esc(font_id_text(native_font(p)));
	print_char(" ");
	print_native_word(p);
  end;
deleted_native_node:
	print("[DELETED]");
glyph_node:begin
    print_esc(font_id_text(native_font(p)));
    print(" glyph#");
    print_int(native_glyph(p));
  end;
pic_node,pdf_node: begin
	if subtype(p) = pic_node then print_esc("XeTeXpicfile")
	else print_esc("XeTeXpdffile");
	print(" """);
	for i:=0 to pic_path_length(p)-1 do
	  print_visible_char(pic_path_byte(p, i));
	print("""");
  end;
pdf_save_pos_node: print_esc("pdfsavepos");
othercases print("whatsit?")
@z

@x
@ @<Make a partial copy of the whatsit...@>=
@y
@ Picture nodes are tricky in that they are variable size.
@d total_pic_node_size(#) == (pic_node_size + (pic_path_length(#) + sizeof(memory_word) - 1) div sizeof(memory_word))

@<Make a partial copy of the whatsit...@>=
@z

@x
othercases confusion("ext2")
@y
native_word_node: begin words:=native_size(p);
  r:=get_node(words);
  while words > 0 do
    begin decr(words); mem[r+words]:=mem[p+words]; end;
  native_glyph_info_ptr(r):=0; native_glyph_count(r):=0;
  copy_native_glyph_info(p, r);
  end;
deleted_native_node: begin words:=native_size(p);
  r:=get_node(words);
  end;
glyph_node: begin r:=get_node(glyph_node_size);
  words:=glyph_node_size;
  end;
pic_node,pdf_node: begin words:=total_pic_node_size(p);
  r:=get_node(words);
  end;
pdf_save_pos_node:
    r := get_node(small_node_size);
othercases confusion("ext2")
@z

@x
othercases confusion("ext3")
@y
native_word_node: begin free_native_glyph_info(p); free_node(p,native_size(p)); end;
deleted_native_node: free_node(p,native_size(p));
glyph_node: free_node(p,glyph_node_size);
pic_node,pdf_node: free_node(p,total_pic_node_size(p));
pdf_save_pos_node:
    free_node(p, small_node_size);
othercases confusion("ext3")
@z

@x
@ @<Incorporate a whatsit node into a vbox@>=do_nothing
@y
@ @<Incorporate a whatsit node into a vbox@>=
begin
	if (subtype(p)=pic_node)
	or (subtype(p)=pdf_node)
	then begin
		x := x + d + height(p);
		d := depth(p);
		if width(p) > w then w := width(p);
	end;
end
@z

@x
@ @<Incorporate a whatsit node into an hbox@>=do_nothing
@y
@ @<Incorporate a whatsit node into an hbox@>=
begin
	case subtype(p) of

	native_word_node:
		begin
			{ merge with any following word fragments in same font, discarding discretionary breaks }
			while (link(q) <> p) do q := link(q); { bring q up in preparation for deletion of nodes starting at p }
			pp := link(p);
		restart:
			if (pp <> null) and (not is_char_node(pp)) then begin
				if (type(pp) = whatsit_node)
					and (subtype(pp) = native_word_node)
					and (native_font(pp) = native_font(p)) then begin
					pp := link(pp);
					goto restart;
				end
				else if (type(pp) = disc_node) then begin
					ppp := link(pp);
					if (ppp <> null) and (not is_char_node(ppp))
							and (type(ppp) = whatsit_node)
							and (subtype(ppp) = native_word_node)
							and (native_font(ppp) = native_font(p)) then begin
						pp := link(ppp);
						goto restart;
					end
				end
			end;

			{ now pp points to the non-native_word node that ended the chain, or null }

			{ we can just check type(p)=whatsit_node below, as we know that the chain
			  contains only discretionaries and native_word nodes, no other whatsits or char_nodes }

			if (pp <> link(p)) then begin
				{ found a chain of at least two pieces starting at p }
				total_chars := 0;
				p := link(q); { the first fragment }
				while (p <> pp) do begin
					if (type(p) = whatsit_node) then
						total_chars := total_chars + native_length(p); { accumulate char count }
					ppp := p; { remember last node seen }
					p := link(p); { point to next fragment or discretionary or terminator }
				end;

				p := link(q); { the first fragment again }
				pp := new_native_word_node(native_font(p), total_chars); { make new node for merged word }
				link(q) := pp; { link to preceding material }
				link(pp) := link(ppp); { attach remainder of hlist to it }
				link(ppp) := null; { and detach from the old fragments }

				{ copy the chars into new node }
				total_chars := 0;
				ppp := p;
				repeat
					if (type(ppp) = whatsit_node) then
						for k := 0 to native_length(ppp)-1 do begin
							set_native_char(pp, total_chars, get_native_char(ppp, k));
							incr(total_chars);
						end;
					ppp := link(ppp);
				until (ppp = null);

				flush_node_list(p); { delete the fragments }
				p := link(q); { update p to point to the new node }
				set_native_metrics(p, XeTeX_use_glyph_metrics); { and measure it (i.e., re-do the OT layout) }
			end;

			{ now incorporate the native_word node measurements into the box we're packing }
			if height(p) > h then
				h := height(p);
			if depth(p) > d then
				d := depth(p);
			x := x + width(p);
		end;

	glyph_node, pic_node, pdf_node:
		begin
			if height(p) > h then
				h := height(p);
			if depth(p) > d then
				d := depth(p);
			x := x + width(p);
		end;

	othercases
		do_nothing

	endcases
end
@z

@x
@ @<Let |d| be the width of the whatsit |p|@>=d:=0
@y
@ @<Let |d| be the width of the whatsit |p|, and |goto found| if ``visible''@>=
if (subtype(p)=native_word_node)
or (subtype(p)=glyph_node)
or (subtype(p)=pic_node)
or (subtype(p)=pdf_node)
then begin
	d:=width(p);
	goto found;
end else
    d := 0
@z

@x
@ @d adv_past(#)==@+if subtype(#)=language_node then
    begin cur_lang:=what_lang(#); l_hyf:=what_lhm(#); r_hyf:=what_rhm(#);@+end
@y
@ @d adv_past(#)==@+if subtype(#)=language_node then
    begin cur_lang:=what_lang(#); l_hyf:=what_lhm(#); r_hyf:=what_rhm(#);@+end
  else if (subtype(#)=native_word_node)
  or (subtype(#)=glyph_node)
  or (subtype(#)=pic_node)
  or (subtype(#)=pdf_node)
  then
    begin act_width:=act_width+width(#); end
@z

@x
@ @<Prepare to move whatsit |p| to the current page, then |goto contribute|@>=
goto contribute
@y
@ @<Prepare to move whatsit |p| to the current page, then |goto contribute|@>=
begin
	if (subtype(p)=pic_node)
	or (subtype(p)=pdf_node)
	then begin
		page_total := page_total + page_depth + height(p);
		page_depth := depth(p);
	end;
	goto contribute;
end
@z

@x
@ @<Process whatsit |p| in |vert_break| loop, |goto not_found|@>=
goto not_found
@y
@ @<Process whatsit |p| in |vert_break| loop, |goto not_found|@>=
begin
	if (subtype(p)=pic_node)
	or (subtype(p)=pdf_node)
	then begin
		cur_height := cur_height + prev_dp + height(p); prev_dp := depth(p);
	end;
	goto not_found;
end
@z

@x
@ @<Output the whatsit node |p| in a vlist@>=
out_what(p)
@y
@ @<Output the whatsit node |p| in a vlist@>=
begin
	case subtype(p) of
	pic_node, pdf_node: begin
		save_h:=dvi_h; save_v:=dvi_v;
		cur_v:=cur_v+height(p);
		pic_out(p, subtype(p) = pdf_node);
		dvi_h:=save_h; dvi_v:=save_v;
		cur_v:=save_v+depth(p); cur_h:=left_edge;
	end;
	
	pdf_save_pos_node:
		@<Save current position to |pdf_last_x_pos|, |pdf_last_y_pos|@>;
	
	othercases
		out_what(p)
	
	endcases
end
@z

@x
@ @<Output the whatsit node |p| in an hlist@>=
out_what(p)
@y
@ @<Save current position to |pdf_last_x_pos|, |pdf_last_y_pos|@>=
begin
    pdf_last_x_pos := cur_h + cur_h_offset;
	pdf_last_y_pos := cur_page_height - cur_v - cur_v_offset
end

@ @<Calculate page dimensions and margins@>=
cur_h_offset := h_offset + (unity * 7227) / 100;
cur_v_offset := v_offset + (unity * 7227) / 100;
if pdf_page_width <> 0 then
    cur_page_width := pdf_page_width
else
    cur_page_width := width(p) + 2*cur_h_offset;
if pdf_page_height <> 0 then
    cur_page_height := pdf_page_height
else
    cur_page_height := height(p) + depth(p) + 2*cur_v_offset

@ @<Glob...@>=
@!cur_page_width: scaled; {width of page being shipped}
@!cur_page_height: scaled; {height of page being shipped}
@!cur_h_offset: scaled; {horizontal offset of page being shipped}
@!cur_v_offset: scaled; {vertical offset of page being shipped}

@ @<Output the whatsit node |p| in an hlist@>=
begin
	case subtype(p) of
	native_word_node, glyph_node: begin
		{ synch DVI state to TeX state }
		synch_h; synch_v;
		f := native_font(p);
		if f<>dvi_f then @<Change font |dvi_f| to |f|@>;
		
		if subtype(p) = glyph_node then begin
			dvi_out(set_glyph_string);
			dvi_four(width(p));
			dvi_two(1); { glyph count }
			dvi_four(0); { x-offset as fixed point }
			dvi_two(native_glyph(p));
			cur_h := cur_h + width(p);
		end else begin
			if native_glyph_info_ptr(p) <> 0 then begin
				len := make_xdv_glyph_array_data(p);
				for k := 0 to len-1 do
					dvi_out(xdv_buffer_byte(k));
			end;
			cur_h := cur_h + width(p);
		end;
		
		dvi_h := cur_h;
	end;
	
	pic_node, pdf_node: begin
		save_h:=dvi_h; save_v:=dvi_v;
		cur_v:=base_line;
		edge:=cur_h+width(p);
		if cur_dir=right_to_left then cur_h:=edge;
		pic_out(p, subtype(p) = pdf_node);
		dvi_h:=save_h; dvi_v:=save_v;
		cur_h:=edge; cur_v:=base_line;
	end;
	
	pdf_save_pos_node:
		@<Save current position to |pdf_last_x_pos|, |pdf_last_y_pos|@>;
	
	othercases
		out_what(p)
	
	endcases
end
@z

@x
procedure special_out(@!p:pointer);
var old_setting:0..max_selector; {holds print |selector|}
@!k:pool_pointer; {index into |str_pool|}
begin synch_h; synch_v;@/
@y
procedure special_out(@!p:pointer);
var old_setting:0..max_selector; {holds print |selector|}
@!k:pool_pointer; {index into |str_pool|}
begin synch_h; synch_v;@/
doing_special := true;
@z

@x
for k:=str_start[str_ptr] to pool_ptr-1 do dvi_out(so(str_pool[k]));
spec_out := spec_sout; mubyte_out := mubyte_sout; mubyte_log := mubyte_slog;
special_printing := false; cs_converting := false;
active_noconvert := false;
pool_ptr:=str_start[str_ptr]; {erase the string}
@y
for k:=str_start_macro(str_ptr) to pool_ptr-1 do dvi_out(so(str_pool[k]));
pool_ptr:=str_start_macro(str_ptr); {erase the string}
doing_special := false;
@z

@x
@!j:small_number; {write stream number}
@y
@!j:small_number; {write stream number}
@!k:integer;
@z

@x
    print(so(str_pool[str_start[str_ptr]+d])); {N.B.: not |print_char|}
@y
    print(so(str_pool[str_start_macro(str_ptr)+d])); {N.B.: not |print_char|}
@z

@x
      begin str_pool[str_start[str_ptr]+d]:=xchr[str_pool[str_start[str_ptr]+d]];
      if (str_pool[str_start[str_ptr]+d]=null_code)
@y
      begin
      if (str_pool[str_start_macro(str_ptr)+d]=null_code)
@z

@x
      system(stringcast(address_of(str_pool[str_start[str_ptr]])));
@y
      if name_of_file then libc_free(name_of_file);
      name_of_file := xmalloc(cur_length * 3 + 2);
      k := 0;
      for d:=0 to cur_length-1 do append_to_name(str_pool[str_start_macro(str_ptr)+d]);
      name_of_file[k+1] := 0;
      system(name_of_file + 1);
@z

@x
  pool_ptr:=str_start[str_ptr];  {erase the string}
@y
  pool_ptr:=str_start_macro(str_ptr);  {erase the string}
@z

@x
@<Declare procedures needed in |hlist_out|, |vlist_out|@>=
@y
@<Declare procedures needed in |hlist_out|, |vlist_out|@>=
procedure pic_out(@!p:pointer; @!is_pdf:boolean);
var
  i:integer;
begin
synch_h; synch_v;
dvi_out(pic_file);
if is_pdf then
	dvi_out(pic_box_type(p))
else
	dvi_out(0);
dvi_four(pic_transform1(p));
dvi_four(pic_transform2(p));
dvi_four(pic_transform3(p));
dvi_four(pic_transform4(p));
dvi_four(pic_transform5(p));
dvi_four(pic_transform6(p));
dvi_two(pic_page(p));
dvi_two(pic_path_length(p));
for i:=0 to pic_path_length(p)-1 do
  dvi_out(pic_path_byte(p, i));
end;

@z

@x
language_node:do_nothing;
@y
language_node,deleted_native_node:do_nothing;
@z

@x
@ @<Finish the extensions@>=
for k:=0 to 15 do if write_open[k] then a_close(write_file[k])
@y
@ @<Finish the extensions@>=
terminate_font_manager;
for k:=0 to 15 do if write_open[k] then a_close(write_file[k])

@ @<Implement \.{\\XeTeXpicfile}@>=
if abs(mode)=mmode then report_illegal_case
else load_picture(false)

@ @<Implement \.{\\XeTeXpdffile}@>=
if abs(mode)=mmode then report_illegal_case
else load_picture(true)

@ @<Implement \.{\\XeTeXglyph}@>=
begin
 if abs(mode)=vmode then begin
  back_input;
  new_graf(true);
 end else if abs(mode)=mmode then report_illegal_case
 else begin
  if is_native_font(cur_font) then begin
   new_whatsit(glyph_node,glyph_node_size);
   scan_int;
   if (cur_val<0)or(cur_val>65535) then
     begin print_err("Bad glyph number");
     help2("A glyph number must be between 0 and 65535.")@/
     ("I changed this one to zero."); int_error(cur_val); cur_val:=0;
   end;
   native_font(tail):=cur_font;
   native_glyph(tail):=cur_val;
   set_native_glyph_metrics(tail, XeTeX_use_glyph_metrics);
  end else not_native_font_error(extension, glyph_code, cur_font);
 end
end

@ Load a picture file and handle following keywords.

@d calc_min_and_max==
	begin
		xmin := 1000000.0;
		xmax := -xmin;
		ymin := xmin;
		ymax := xmax;
		for i := 0 to 3 do begin
			if xCoord(corners[i]) < xmin then xmin := xCoord(corners[i]);
			if xCoord(corners[i]) > xmax then xmax := xCoord(corners[i]);
			if yCoord(corners[i]) < ymin then ymin := yCoord(corners[i]);
			if yCoord(corners[i]) > ymax then ymax := yCoord(corners[i]);
		end;
	end

@d update_corners==
	for i := 0 to 3 do
		transform_point(address_of(corners[i]), address_of(t2))

@d do_size_requests==begin
	{ calculate current width and height }
	calc_min_and_max;
	if x_size_req = 0.0 then begin
		make_scale(address_of(t2), y_size_req / (ymax - ymin), y_size_req / (ymax - ymin));
	end else if y_size_req = 0.0 then begin
		make_scale(address_of(t2), x_size_req / (xmax - xmin), x_size_req / (xmax - xmin));
	end else begin
		make_scale(address_of(t2), x_size_req / (xmax - xmin), y_size_req / (ymax - ymin));
	end;
	update_corners;
	x_size_req := 0.0;
	y_size_req := 0.0;
	transform_concat(address_of(t), address_of(t2));
end

@<Declare procedures needed in |do_extension|@>=
procedure load_picture(@!is_pdf:boolean);
var
	pic_path: ^char;
	bounds: real_rect;
	t, t2: transform;
	corners: array[0..3] of real_point;
	x_size_req,y_size_req: real;
	check_keywords: boolean;
	x_size, y_size: real;
	xmin,xmax,ymin,ymax: real;
	i: small_number;
	page: integer;
	pdf_box_type: integer;
	result: integer;
begin
	{ scan the filename and pack into name_of_file }
	scan_file_name;
	pack_cur_name;

    pdf_box_type := 0;
	page := 0;
	if is_pdf then begin
		if scan_keyword("page") then begin
			scan_int;
			page := cur_val;
		end;
		pdf_box_type := pdfbox_crop;
		if scan_keyword("crop") then do_nothing
		else if scan_keyword("media") then pdf_box_type := pdfbox_media
		else if scan_keyword("bleed") then pdf_box_type := pdfbox_bleed
		else if scan_keyword("trim") then pdf_box_type := pdfbox_trim
		else if scan_keyword("art") then pdf_box_type := pdfbox_art;
	end;

	{ access the picture file and check its size }
	result := find_pic_file(address_of(pic_path), address_of(bounds), pdf_box_type, page);

	setPoint(corners[0], xField(bounds) * 72.27 / 72.0, yField(bounds) * 72.27 / 72.0);
	setPoint(corners[1], xField(corners[0]), (yField(bounds) + htField(bounds)) * 72.27 / 72.0);
	setPoint(corners[2], (xField(bounds) + wdField(bounds)) * 72.27 / 72.0, yField(corners[1]));
	setPoint(corners[3], xField(corners[2]), yField(corners[0]));

	x_size_req := 0.0;
	y_size_req := 0.0;

	{ look for any scaling requests for this picture }
	make_identity(address_of(t));
	
	check_keywords := true;
	while check_keywords do begin
		if scan_keyword("scaled") then begin
			scan_int;
			if (x_size_req = 0.0) and (y_size_req = 0.0) then begin
				make_scale(address_of(t2), float(cur_val) / 1000.0, float(cur_val) / 1000.0);
				update_corners;
				transform_concat(address_of(t), address_of(t2));
			end
		end else if scan_keyword("xscaled") then begin
			scan_int;
			if (x_size_req = 0.0) and (y_size_req = 0.0) then begin
				make_scale(address_of(t2), float(cur_val) / 1000.0, 1.0);
				update_corners;
				transform_concat(address_of(t), address_of(t2));
			end
		end else if scan_keyword("yscaled") then begin
			scan_int;
			if (x_size_req = 0.0) and (y_size_req = 0.0) then begin
				make_scale(address_of(t2), 1.0, float(cur_val) / 1000.0);
				update_corners;
				transform_concat(address_of(t), address_of(t2));
			end
		end else if scan_keyword("width") then begin
			scan_normal_dimen;
			if cur_val <= 0 then begin
				print_err("Improper image ");
				print("size (");
				print_scaled(cur_val);
				print("pt) will be ignored");
				help2("I can't scale images to zero or negative sizes, ")
					 ("so I'm ignoring this.");
				error;
			end else
				x_size_req := Fix2X(cur_val);
		end else if scan_keyword("height") then begin
			scan_normal_dimen;
			if cur_val <= 0 then begin
				print_err("Improper image ");
				print("size (");
				print_scaled(cur_val);
				print("pt) will be ignored");
				help2("I can't scale images to zero or negative sizes, ")
					 ("so I'm ignoring this.");
				error;
			end else
				y_size_req := Fix2X(cur_val);
		end else if scan_keyword("rotated") then begin
			scan_decimal;
			if (x_size_req <> 0.0) or (y_size_req <> 0.0) then do_size_requests;
			make_rotation(address_of(t2), Fix2X(cur_val) * 3.141592653589793 / 180.0);
			update_corners;
			calc_min_and_max;
			setPoint(corners[0], xmin, ymin);
			setPoint(corners[1], xmin, ymax);
			setPoint(corners[2], xmax, ymax);
			setPoint(corners[3], xmax, ymin);
			transform_concat(address_of(t), address_of(t2));
		end else
			check_keywords := false;
	end;

	if (x_size_req <> 0.0) or (y_size_req <> 0.0) then do_size_requests;
	
	calc_min_and_max;
	make_translation(address_of(t2), -xmin, -ymin);
	transform_concat(address_of(t), address_of(t2));
	
	if result = 0 then begin

		new_whatsit(pic_node, pic_node_size + (strlen(pic_path) + sizeof(memory_word) - 1) div sizeof(memory_word));
		if is_pdf then begin
		  subtype(tail) := pdf_node;
		  pic_box_type(tail) := pdf_box_type;
		end;
		pic_path_length(tail) := strlen(pic_path);
		pic_page(tail) := page;
			
		width(tail) := X2Fix(xmax - xmin);
		height(tail) := X2Fix(ymax - ymin);
		depth(tail) := 0;
	
		pic_transform1(tail) := X2Fix(aField(t));
		pic_transform2(tail) := X2Fix(bField(t));
		pic_transform3(tail) := X2Fix(cField(t));
		pic_transform4(tail) := X2Fix(dField(t));
		pic_transform5(tail) := X2Fix(txField(t));
		pic_transform6(tail) := X2Fix(tyField(t));
	
		memcpy(address_of(mem[tail + pic_node_size]), pic_path, strlen(pic_path));
		libc_free(pic_path);
	
	end else begin

		print_err("Unable to load picture or PDF file '");
		print_file_name(cur_name,cur_area,cur_ext); print("'");
		if result = -43 then begin { Mac OS file not found error }
			help2("The requested image couldn't be read because ")
				 ("the file was not found.");
		end
		else begin { otherwise assume GraphicImport failed }
			help2("The requested image couldn't be read because ")
				 ("it was not a recognized image format.");
		end;
		error;

	end;

end;

@ @<Implement \.{\\XeTeXinputencoding}@>=
begin
	{ scan a filename-like arg for the input encoding }
	scan_and_pack_name;
	
	{ convert it to "mode" and "encoding" values, and apply to the current input file }
	i := get_encoding_mode_and_info(address_of(j));
	if i = XeTeX_input_mode_auto then begin
	  print_err("Encoding mode `auto' is not valid for \XeTeXinputencoding.");
      help2("You can't use `auto' encoding here, only for \XeTeXdefaultencoding. ")
           ("I'll ignore this and leave the current encoding unchanged.");
	  error;
	end	else set_input_file_encoding(input_file[in_open], i, j);
end

@ @<Implement \.{\\XeTeXdefaultencoding}@>=
begin
	{ scan a filename-like arg for the input encoding }
	scan_and_pack_name;
	
	{ convert it to "mode" and "encoding" values, and store them as defaults for new input files }
	XeTeX_default_input_mode := get_encoding_mode_and_info(address_of(j));
	XeTeX_default_input_encoding := j;
end

@ @<Implement \.{\\XeTeXlinebreaklocale}@>=
begin
	{ scan a filename-like arg for the locale name }
	scan_file_name;
	if length(cur_name) = 0 then XeTeX_linebreak_locale := 0
	else XeTeX_linebreak_locale := cur_name; { we ignore the area and extension! }
end

@ @<Glob...@>=
@!pdf_last_x_pos: integer;
@!pdf_last_y_pos: integer;

@ @<Implement \.{\\pdfsavepos}@>=
begin
    new_whatsit(pdf_save_pos_node, small_node_size);
end

@z

@x
@d eTeX_version_code=eTeX_int {code for \.{\\eTeXversion}}
@y
@d eTeX_version_code=eTeX_int {code for \.{\\eTeXversion}}

@d XeTeX_version_code=XeTeX_int {code for \.{\\XeTeXversion}}

{ these are also in xetexmac.c and must correspond! }
@d XeTeX_count_glyphs_code=XeTeX_int+1

@d XeTeX_count_variations_code=XeTeX_int+2
@d XeTeX_variation_code=XeTeX_int+3
@d XeTeX_find_variation_by_name_code=XeTeX_int+4
@d XeTeX_variation_min_code=XeTeX_int+5
@d XeTeX_variation_max_code=XeTeX_int+6
@d XeTeX_variation_default_code=XeTeX_int+7

@d XeTeX_count_features_code=XeTeX_int+8
@d XeTeX_feature_code_code=XeTeX_int+9
@d XeTeX_find_feature_by_name_code=XeTeX_int+10
@d XeTeX_is_exclusive_feature_code=XeTeX_int+11
@d XeTeX_count_selectors_code=XeTeX_int+12
@d XeTeX_selector_code_code=XeTeX_int+13
@d XeTeX_find_selector_by_name_code=XeTeX_int+14
@d XeTeX_is_default_selector_code=XeTeX_int+15

@d XeTeX_OT_count_scripts_code=XeTeX_int+16
@d XeTeX_OT_count_languages_code=XeTeX_int+17
@d XeTeX_OT_count_features_code=XeTeX_int+18
@d XeTeX_OT_script_code=XeTeX_int+19
@d XeTeX_OT_language_code=XeTeX_int+20
@d XeTeX_OT_feature_code=XeTeX_int+21

@d XeTeX_map_char_to_glyph_code=XeTeX_int+22
@d XeTeX_glyph_index_code=XeTeX_int+23
@d XeTeX_font_type_code=XeTeX_int+24

@d XeTeX_first_char_code=XeTeX_int+25
@d XeTeX_last_char_code=XeTeX_int+26

@d pdf_last_x_pos_code        = XeTeX_int+27
@d pdf_last_y_pos_code        = XeTeX_int+28

{ NB: must update eTeX_dim when items are added here! }
@z

@x
@!@:eTeX_revision_}{\.{\\eTeXrevision} primitive@>
@y
@!@:eTeX_revision_}{\.{\\eTeXrevision} primitive@>

primitive("XeTeXversion",last_item,XeTeX_version_code);
@!@:XeTeX_version_}{\.{\\XeTeXversion} primitive@>
primitive("XeTeXrevision",convert,XeTeX_revision_code);@/
@!@:XeTeXrevision_}{\.{\\XeTeXrevision} primitive@>

primitive("XeTeXcountglyphs",last_item,XeTeX_count_glyphs_code);

primitive("XeTeXcountvariations",last_item,XeTeX_count_variations_code);
primitive("XeTeXvariation",last_item,XeTeX_variation_code);
primitive("XeTeXfindvariationbyname",last_item,XeTeX_find_variation_by_name_code);
primitive("XeTeXvariationmin",last_item,XeTeX_variation_min_code);
primitive("XeTeXvariationmax",last_item,XeTeX_variation_max_code);
primitive("XeTeXvariationdefault",last_item,XeTeX_variation_default_code);

primitive("XeTeXcountfeatures",last_item,XeTeX_count_features_code);
primitive("XeTeXfeaturecode",last_item,XeTeX_feature_code_code);
primitive("XeTeXfindfeaturebyname",last_item,XeTeX_find_feature_by_name_code);
primitive("XeTeXisexclusivefeature",last_item,XeTeX_is_exclusive_feature_code);
primitive("XeTeXcountselectors",last_item,XeTeX_count_selectors_code);
primitive("XeTeXselectorcode",last_item,XeTeX_selector_code_code);
primitive("XeTeXfindselectorbyname",last_item,XeTeX_find_selector_by_name_code);
primitive("XeTeXisdefaultselector",last_item,XeTeX_is_default_selector_code);

primitive("XeTeXvariationname",convert,XeTeX_variation_name_code);
primitive("XeTeXfeaturename",convert,XeTeX_feature_name_code);
primitive("XeTeXselectorname",convert,XeTeX_selector_name_code);

primitive("XeTeXOTcountscripts",last_item,XeTeX_OT_count_scripts_code);
primitive("XeTeXOTcountlanguages",last_item,XeTeX_OT_count_languages_code);
primitive("XeTeXOTcountfeatures",last_item,XeTeX_OT_count_features_code);
primitive("XeTeXOTscripttag",last_item,XeTeX_OT_script_code);
primitive("XeTeXOTlanguagetag",last_item,XeTeX_OT_language_code);
primitive("XeTeXOTfeaturetag",last_item,XeTeX_OT_feature_code);

primitive("XeTeXcharglyph", last_item, XeTeX_map_char_to_glyph_code);
primitive("XeTeXglyphindex", last_item, XeTeX_glyph_index_code);

primitive("XeTeXglyphname",convert,XeTeX_glyph_name_code);

primitive("XeTeXfonttype", last_item, XeTeX_font_type_code);

primitive("XeTeXfirstfontchar", last_item, XeTeX_first_char_code);
primitive("XeTeXlastfontchar", last_item, XeTeX_last_char_code);

primitive("pdflastxpos",last_item,pdf_last_x_pos_code);
primitive("pdflastypos",last_item,pdf_last_y_pos_code);
@z

@x
eTeX_version_code: print_esc("eTeXversion");
@y
eTeX_version_code: print_esc("eTeXversion");
XeTeX_version_code: print_esc("XeTeXversion");

XeTeX_count_glyphs_code: print_esc("XeTeXcountglyphs");

XeTeX_count_variations_code: print_esc("XeTeXcountvariations");
XeTeX_variation_code: print_esc("XeTeXvariation");
XeTeX_find_variation_by_name_code: print_esc("XeTeXfindvariationbyname");
XeTeX_variation_min_code: print_esc("XeTeXvariationmin");
XeTeX_variation_max_code: print_esc("XeTeXvariationmax");
XeTeX_variation_default_code: print_esc("XeTeXvariationdefault");

XeTeX_count_features_code: print_esc("XeTeXcountfeatures");
XeTeX_feature_code_code: print_esc("XeTeXfeaturecode");
XeTeX_find_feature_by_name_code: print_esc("XeTeXfindfeaturebyname");
XeTeX_is_exclusive_feature_code: print_esc("XeTeXisexclusivefeature");
XeTeX_count_selectors_code: print_esc("XeTeXcountselectors");
XeTeX_selector_code_code: print_esc("XeTeXselectorcode");
XeTeX_find_selector_by_name_code: print_esc("XeTeXfindselectorbyname");
XeTeX_is_default_selector_code: print_esc("XeTeXisdefaultselector");

XeTeX_OT_count_scripts_code: print_esc("XeTeXOTcountscripts");
XeTeX_OT_count_languages_code: print_esc("XeTeXOTcountlanguages");
XeTeX_OT_count_features_code: print_esc("XeTeXOTcountfeatures");
XeTeX_OT_script_code: print_esc("XeTeXOTscripttag");
XeTeX_OT_language_code: print_esc("XeTeXOTlanguagetag");
XeTeX_OT_feature_code: print_esc("XeTeXOTfeaturetag");

XeTeX_map_char_to_glyph_code: print_esc("XeTeXcharglyph");
XeTeX_glyph_index_code: print_esc("XeTeXglyphindex");

XeTeX_font_type_code: print_esc("XeTeXfonttype");

XeTeX_first_char_code: print_esc("XeTeXfirstfontchar");
XeTeX_last_char_code: print_esc("XeTeXlastfontchar");

  pdf_last_x_pos_code:  print_esc("pdflastxpos");
  pdf_last_y_pos_code:  print_esc("pdflastypos");
@z

@x
eTeX_version_code: cur_val:=eTeX_version;
@y
eTeX_version_code: cur_val:=eTeX_version;
XeTeX_version_code: cur_val:=XeTeX_version;

XeTeX_count_glyphs_code:
  begin
    scan_font_ident; n:=cur_val;
    if is_atsu_font(n) then
      cur_val:=atsu_font_get(m - XeTeX_int, font_layout_engine[n])
    else if is_ot_font(n) then
      cur_val:=ot_font_get(m - XeTeX_int, font_layout_engine[n])
    else
      cur_val:=0;
  end;

XeTeX_count_variations_code,
XeTeX_count_features_code:
  begin
    scan_font_ident; n:=cur_val;
    if is_atsu_font(n) then
      cur_val:=atsu_font_get(m - XeTeX_int, font_layout_engine[n])
    else begin
      cur_val:=0;
    end;
  end;

XeTeX_variation_code,
XeTeX_variation_min_code,
XeTeX_variation_max_code,
XeTeX_variation_default_code,
XeTeX_feature_code_code,
XeTeX_is_exclusive_feature_code,
XeTeX_count_selectors_code:
  begin
    scan_font_ident; n:=cur_val;
    if is_atsu_font(n) then begin
      scan_int; k:=cur_val;
      cur_val:=atsu_font_get_1(m - XeTeX_int, font_layout_engine[n], k);
    end else begin
      not_atsu_font_error(last_item, m, n); cur_val:=-1;
    end;
  end;

XeTeX_selector_code_code,
XeTeX_is_default_selector_code:
  begin
    scan_font_ident; n:=cur_val;
    if is_atsu_font(n) then begin
      scan_int; k:=cur_val; scan_int;
      cur_val:=atsu_font_get_2(m - XeTeX_int, font_layout_engine[n], k, cur_val);
    end else begin
      not_atsu_font_error(last_item, m, n); cur_val:=-1;
    end;
  end;

XeTeX_find_variation_by_name_code,
XeTeX_find_feature_by_name_code:
  begin
    scan_font_ident; n:=cur_val;
    if is_atsu_font(n) then begin
      scan_and_pack_name;
      cur_val:=atsu_font_get_named(m - XeTeX_int, font_layout_engine[n]);
    end else begin
      not_atsu_font_error(last_item, m, n); cur_val:=-1;
    end;
  end;

XeTeX_find_selector_by_name_code:
  begin
    scan_font_ident; n:=cur_val;
    if is_atsu_font(n) then begin
      scan_int; k:=cur_val; scan_and_pack_name;
      cur_val:=atsu_font_get_named_1(m - XeTeX_int, font_layout_engine[n], k);
    end else begin
      not_atsu_font_error(last_item, m, n); cur_val:=-1;
    end;
  end;

XeTeX_OT_count_scripts_code:
  begin
    scan_font_ident; n:=cur_val;
    if is_ot_font(n) then
      cur_val:=ot_font_get(m - XeTeX_int, font_layout_engine[n])
    else begin
{
      not_ot_font_error(last_item, m, n); cur_val:=-1;
}
      cur_val:=0;
    end;
  end;
  
XeTeX_OT_count_languages_code,
XeTeX_OT_script_code:
  begin
    scan_font_ident; n:=cur_val;
    if is_ot_font(n) then begin
      scan_int; k:=cur_val;
      cur_val:=ot_font_get_1(m - XeTeX_int, font_layout_engine[n], k);
    end else begin
      not_ot_font_error(last_item, m, n); cur_val:=-1;
    end;
  end;

XeTeX_OT_count_features_code,
XeTeX_OT_language_code:
  begin
    scan_font_ident; n:=cur_val;
    if is_ot_font(n) then begin
      scan_int; k:=cur_val; scan_int;
      cur_val:=ot_font_get_2(m - XeTeX_int, font_layout_engine[n], k, cur_val);
    end else begin
      not_ot_font_error(last_item, m, n); cur_val:=-1;
    end;
  end;

XeTeX_OT_feature_code:
  begin
    scan_font_ident; n:=cur_val;
    if is_ot_font(n) then begin
      scan_int; k:=cur_val; scan_int; kk:=cur_val; scan_int;
      cur_val:=ot_font_get_3(m - XeTeX_int, font_layout_engine[n], k, kk, cur_val);
    end else begin
      not_ot_font_error(last_item, m, n); cur_val:=-1;
    end;
  end;

XeTeX_map_char_to_glyph_code:
  begin
    if is_native_font(cur_font) then begin
      scan_int; n:=cur_val; cur_val:=map_char_to_glyph(cur_font, n)
    end else begin
      not_native_font_error(last_item, m, cur_font); cur_val:=0
    end
  end;

XeTeX_glyph_index_code:
  begin
    if is_native_font(cur_font) then begin
      scan_and_pack_name;
      cur_val:=map_glyph_to_index(cur_font)
    end else begin
      not_native_font_error(last_item, m, cur_font); cur_val:=0
    end
  end;

XeTeX_font_type_code:
  begin
    scan_font_ident; n:=cur_val;
    if is_atsu_font(n) then cur_val:=1
    else begin
      if is_ot_font(n) then cur_val:=2
      else cur_val:=0
    end
  end;

XeTeX_first_char_code,XeTeX_last_char_code:
  begin
    scan_font_ident; n:=cur_val;
    if is_native_font(n) then
      cur_val:=get_font_char_range(n, m = XeTeX_first_char_code)
    else begin
      if m = XeTeX_first_char_code then cur_val:=font_bc[n]
      else cur_val:=font_ec[n];
    end
  end;

  pdf_last_x_pos_code:  cur_val := pdf_last_x_pos;
  pdf_last_y_pos_code:  cur_val := pdf_last_y_pos;

@ Slip in an extra procedure here and there....

@<Error hand...@>=
procedure scan_and_pack_name; forward;

@ @<Declare procedures needed in |do_extension|@>=
procedure scan_and_pack_name;
begin
  scan_file_name; pack_cur_name;
end;

@ @<Declare the procedure called |print_cmd_chr|@>=
procedure not_atsu_font_error(cmd, c: integer; f: integer);
begin
  print_err("Cannot use "); print_cmd_chr(cmd, c);
  print(" with "); print(font_name[f]);
  print("; not an AAT font");
  error;
end;

procedure not_ot_font_error(cmd, c: integer; f: integer);
begin
  print_err("Cannot use "); print_cmd_chr(cmd, c);
  print(" with "); print(font_name[f]);
  print("; not an OpenType Layout font");
  error;
end;

procedure not_native_font_error(cmd, c: integer; f: integer);
begin
  print_err("Cannot use "); print_cmd_chr(cmd, c);
  print(" with "); print(font_name[f]);
  print("; not a native platform font");
  error;
end;

@ @<Cases of |convert| for |print_cmd_chr|@>=
eTeX_revision_code: print_esc("eTeXrevision");
XeTeX_revision_code: print_esc("XeTeXrevision");

XeTeX_variation_name_code: print_esc("XeTeXvariationname");
XeTeX_feature_name_code: print_esc("XeTeXfeaturename");
XeTeX_selector_name_code: print_esc("XeTeXselectorname");
XeTeX_glyph_name_code: print_esc("XeTeXglyphname");

@ @<Cases of `Scan the argument for command |c|'@>=
eTeX_revision_code: do_nothing;
XeTeX_revision_code: do_nothing;

XeTeX_variation_name_code,
XeTeX_feature_name_code:
  begin
    scan_font_ident; fnt:=cur_val;
    if is_atsu_font(fnt) then begin
      scan_int; arg1:=cur_val; arg2:=0;
    end else
      not_atsu_font_error(convert, c, fnt);
  end;

XeTeX_selector_name_code:
  begin
    scan_font_ident; fnt:=cur_val;
    if is_atsu_font(fnt) then begin
      scan_int; arg1:=cur_val; scan_int; arg2:=cur_val;
    end else
      not_atsu_font_error(convert, c, fnt);
  end;

XeTeX_glyph_name_code:
  begin
    scan_font_ident; fnt:=cur_val;
    if is_native_font(fnt) then begin
      scan_int; arg1:=cur_val;
    end else
      not_native_font_error(convert, c, fnt);
  end;

@ @<Cases of `Print the result of command |c|'@>=
eTeX_revision_code: print(eTeX_revision);
XeTeX_revision_code: print(XeTeX_revision);

XeTeX_variation_name_code,
XeTeX_feature_name_code,
XeTeX_selector_name_code:
    if is_atsu_font(fnt) then
      atsu_print_font_name(c, font_layout_engine[fnt], arg1, arg2);

XeTeX_glyph_name_code:
    if is_native_font(fnt) then print_glyph_name(fnt, arg1);
@z

@x
font_char_wd_code,
font_char_ht_code,
font_char_dp_code,
font_char_ic_code: begin scan_font_ident; q:=cur_val; scan_char_num;
  if (font_bc[q]<=cur_val)and(font_ec[q]>=cur_val) then
    begin i:=char_info(q)(qi(cur_val));
    case m of
    font_char_wd_code: cur_val:=char_width(q)(i);
    font_char_ht_code: cur_val:=char_height(q)(height_depth(i));
    font_char_dp_code: cur_val:=char_depth(q)(height_depth(i));
    font_char_ic_code: cur_val:=char_italic(q)(i);
    end; {there are no other cases}
    end
  else cur_val:=0;
  end;
@y
font_char_wd_code,
font_char_ht_code,
font_char_dp_code,
font_char_ic_code: begin scan_font_ident; q:=cur_val; scan_usv_num;
  if is_native_font(q) then begin
    case m of
    font_char_wd_code: cur_val := getnativecharwd(q, cur_val);
    font_char_ht_code: cur_val := getnativecharht(q, cur_val);
    font_char_dp_code: cur_val := getnativechardp(q, cur_val);
    font_char_ic_code: cur_val := getnativecharic(q, cur_val);
    end; {there are no other cases}
  end else begin
    if (font_bc[q]<=cur_val)and(font_ec[q]>=cur_val) then
      begin i:=char_info(q)(qi(cur_val));
      case m of
      font_char_wd_code: cur_val:=char_width(q)(i);
      font_char_ht_code: cur_val:=char_height(q)(height_depth(i));
      font_char_dp_code: cur_val:=char_depth(q)(height_depth(i));
      font_char_ic_code: cur_val:=char_italic(q)(i);
      end; {there are no other cases}
      end
    else cur_val:=0;
    end
  end;
@z

@x
@d TeXXeT_en==(TeXXeT_state>0) {is \TeXXeT\ enabled?}
@y
@d TeXXeT_en==(TeXXeT_state>0) {is \TeXXeT\ enabled?}

@d XeTeX_dash_break_state == eTeX_state(XeTeX_dash_break_code)
@d XeTeX_dash_break_en == (XeTeX_dash_break_state>0)

@d XeTeX_default_input_mode == eTeX_state(XeTeX_default_input_mode_code)
@d XeTeX_default_input_encoding == eTeX_state(XeTeX_default_input_encoding_code)
@z

@x
eTeX_state_code+TeXXeT_code:print_esc("TeXXeTstate");
@y
eTeX_state_code+TeXXeT_code:print_esc("TeXXeTstate");
eTeX_state_code+XeTeX_dash_break_code:print_esc("XeTeXdashbreakstate");
@z

@x
primitive("TeXXeTstate",assign_int,eTeX_state_base+TeXXeT_code);
@!@:TeXXeT_state_}{\.{\\TeXXeT_state} primitive@>
@y
primitive("TeXXeTstate",assign_int,eTeX_state_base+TeXXeT_code);
@!@:TeXXeT_state_}{\.{\\TeXXeT_state} primitive@>

primitive("XeTeXdashbreakstate",assign_int,eTeX_state_base+XeTeX_dash_break_code);
@!@:XeTeX_dash_break_state_}{\.{\\XeTeX_dash_break_state} primitive@>

primitive("XeTeXinputencoding",extension,XeTeX_input_encoding_extension_code);
primitive("XeTeXdefaultencoding",extension,XeTeX_default_encoding_extension_code);
@z

@x
@ Here we compute the effective width of a glue node as in |hlist_out|.

@<Cases of |reverse|...@>=
glue_node: begin round_glue;
  @<Handle a glue node for mixed...@>;
  end;
@y
@ Need to measure native_word and picture nodes when reversing!
@<Cases of |reverse|...@>=
whatsit_node:
  if (subtype(p)=native_word_node)
  or (subtype(p)=glyph_node)
  or (subtype(p)=pic_node)
  or (subtype(p)=pdf_node)
  then
    rule_wd:=width(p)
  else
    goto next_p;

@ Here we compute the effective width of a glue node as in |hlist_out|.
@z

@x
str_pool[pool_ptr]:=si(" "); l:=str_start[s];
@y
str_pool[pool_ptr]:=si(" "); l:=str_start_macro(s);
@z

@x
if_font_char_code:begin scan_font_ident; n:=cur_val; scan_char_num;
  if (font_bc[n]<=cur_val)and(font_ec[n]>=cur_val) then
    b:=char_exists(char_info(n)(qi(cur_val)))
  else b:=false;
  end;
@y
if_font_char_code:begin scan_font_ident; n:=cur_val; scan_usv_num;
  if is_native_font(n) then
    b := (map_char_to_glyph(n, cur_val) > 0)
  else begin
    if (font_bc[n]<=cur_val)and(font_ec[n]>=cur_val) then
      b:=char_exists(char_info(n)(qi(cur_val)))
    else b:=false;
    end;
  end;
@z

@x
      for c := str_start[text(h)] to str_start[text(h) + 1] - 1
@y
      for c := str_start_macro(text(h)) to str_start_macro(text(h) + 1) - 1
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
@z

@x
effective_char_info:=null_character;
exit:end;
@y
effective_char_info:=null_character;
exit:end;

{ the following procedure has been moved so that new_native_character can call it }

procedure char_warning(@!f:internal_font_number;@!c:integer);
var old_setting: integer; {saved value of |tracing_online|}
begin if tracing_lost_chars>0 then
 begin old_setting:=tracing_online;
 if eTeX_ex and(tracing_lost_chars>1) then tracing_online:=1;
  begin begin_diagnostic;
  print_nl("Missing character: There is no ");
@.Missing character@>
  if c < @"10000 then print_ASCII(c)
  else begin { non-Plane 0 Unicodes can't be sent through print_ASCII }
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
		call set_native_metrics(node) if that is required! }
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
	native_glyph_info_ptr(q) := 0;

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
		len := apply_mapping(font_mapping[f], address_of(str_pool[str_start_macro(str_ptr)]), cur_length);
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
		native_glyph_info_ptr(p) := 0;
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

procedure font_mapping_warning(mappingNameP:void_pointer; mappingNameLen:integer);
var
	i: integer;
begin
	begin_diagnostic;
	print_nl("Font mapping `");
	print_utf8_str(mappingNameP, mappingNameLen);
	print("' for font `");
	i := 1;
	while ord(name_of_file[i]) <> 0 do begin
		print_visible_char(name_of_file[i]); { this is already UTF-8 }
		incr(i);
	end;
	print("' not found.");
	end_diagnostic(false);
end;

function load_native_font(u: pointer; nom, aire:str_number; s: scaled): internal_font_number;
label
	done;
var
	k: integer;
	font_engine: void_pointer;	{really an ATSUStyle or XeTeXLayoutEngine}
	actual_size: scaled;	{|s| converted to real size, if it was negative}
	p: pointer;	{for temporary |native_char| node we'll create}
	ascent, descent, font_slant, x_ht, cap_ht: scaled;
	f: internal_font_number;
	full_name: str_number;
begin
	{ on entry here, the full name is packed into name_of_file in UTF8 form }

	load_native_font := null_font;

	if (s < 0) then actual_size := -s * unity div 100 else actual_size := s;
	font_engine := find_native_font(name_of_file + 1, actual_size);
	if font_engine = 0 then goto done;
	
	{ look again to see if the font is already loaded, now that we know its canonical name }
	str_room(name_length);
	for k := 1 to name_length do
		append_char(name_of_file[k]);
    full_name := make_string; { not slow_make_string because we'll flush it if the font was already loaded }
    
	for f:=font_base+1 to font_ptr do
  		if (font_area[f] = native_font_type_flag) and str_eq_str(font_name[f], full_name) and (font_size[f] = actual_size) then begin
  		    release_font_engine(font_engine, native_font_type_flag);
  			flush_string;
  		    load_native_font := f;
  		    goto done;
        end;
	
	if (font_ptr = font_max) or (fmem_ptr + 8 > font_mem_size) then begin
		@<Apologize for not loading the font, |goto done|@>;
	end;

	{ we've found a valid installed font, and have room }
	incr(font_ptr);
	font_area[font_ptr] := native_font_type_flag; { set by find_native_font to either aat_font_flag or ot_font_flag }

	{ store the canonical name }
	font_name[font_ptr] := full_name;
	
	font_check[font_ptr].b0 := 0;
	font_check[font_ptr].b1 := 0;
	font_check[font_ptr].b2 := 0;
	font_check[font_ptr].b3 := 0;
	font_glue[font_ptr] := null;
	font_dsize[font_ptr] := 10 * unity;
	font_size[font_ptr] := actual_size;

	if (native_font_type_flag = aat_font_flag) then begin
		atsu_get_font_metrics(font_engine, address_of(ascent), address_of(descent),
								address_of(x_ht), address_of(cap_ht), address_of(font_slant))
	end else begin
		ot_get_font_metrics(font_engine, address_of(ascent), address_of(descent),
								address_of(x_ht), address_of(cap_ht), address_of(font_slant));
	end;

	height_base[font_ptr] := ascent;
	depth_base[font_ptr] := -descent;

	font_params[font_ptr] := 8;		{ we add an extra \fontdimen: #8 -> cap_height }
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
	
	font_info[fmem_ptr].sc := font_slant;							{slant}
	incr(fmem_ptr);
	font_info[fmem_ptr].sc := s;									{space = width of space character}
	incr(fmem_ptr);
	font_info[fmem_ptr].sc := s div 2;								{space_stretch = 1/2 * space}
	incr(fmem_ptr);
	font_info[fmem_ptr].sc := s div 3;								{space_shrink = 1/3 * space}
	incr(fmem_ptr);
	font_info[fmem_ptr].sc := x_ht;									{x_height}
	incr(fmem_ptr);
	font_info[fmem_ptr].sc := font_size[font_ptr];					{quad = font size}
	incr(fmem_ptr);
	font_info[fmem_ptr].sc := s div 3;								{extra_space = 1/3 * space}
	incr(fmem_ptr);
	font_info[fmem_ptr].sc := cap_ht;								{cap_height}
	incr(fmem_ptr);
	
	font_mapping[font_ptr] := loaded_font_mapping;
	font_flags[font_ptr] := loaded_font_flags;

	load_native_font := font_ptr;
done:
end;

procedure do_locale_linebreaks(s: pointer; len: integer);
var
	offs, prevOffs, i: integer;
	use_penalty, use_skip: boolean;
begin
	if XeTeX_linebreak_locale = 0 then begin
		link(tail) := new_native_word_node(main_f, len);
		tail := link(tail);
		for i := 0 to len - 1 do
			set_native_char(tail, i, str_pool[s + i]);
		set_native_metrics(tail, XeTeX_use_glyph_metrics);
	end else begin
		use_skip := XeTeX_linebreak_skip <> zero_glue;
		use_penalty := XeTeX_linebreak_penalty <> 0 or not use_skip;
		linebreak_start(XeTeX_linebreak_locale, address_of(str_pool[s]), len);
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
					set_native_char(tail, i - prevOffs, str_pool[s + i]);
				set_native_metrics(tail, XeTeX_use_glyph_metrics);
			end;
		until offs < 0;
	end
end;

@z
