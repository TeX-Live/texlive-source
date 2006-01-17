% Copyright (c) 2005 Han Th\^e\llap{\raise 0.5ex\hbox{\'{}}} Th\`anh, <thanh@pdftex.org>
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
% $Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/pdftex.ch#163 $
@ This change file creates an extra primitive named \.{\\pdfnoligatures}, allowing
you to destroy all ligatures of a loaded font. The command code is
|assign_font_int|, with the new modifier value |no_lig_code|.

Nothing other than the |char_tag| value is changed, and nothing is saved. 
The primitive works directly on |font_info|, so all changes are global,
the same as for the other font assignment primitives. Usage is quite
straightforward: \.{\\pdfnoligatures \\f } disables ligatures for font \.{\\f}.
The expansion of \.{\\pdfnoligatures \\f } is either |1|, when the font has
no ligatures, or |0|, when the font does have ligatures.

Thanh: this patch uses |orig_char_info| because it is to be inserted
after tex.pch. If you merge this into pdftex.ch, you may want to change
back to |char_info|. It would not compile without using the 
|orig| trick because |effective_char| is not yet known at the spot 
where |test_no_ligatures| is used. sigh.

@x [230]
@d ef_code_base == 4
@y
@d ef_code_base == 4
@d no_lig_code == 6 {|tag_code| is 5 }
@z

@x [230]
begin scan_font_ident;
if m=0 then scanned_result(hyphen_char[cur_val])(int_val)
else if m=1 then scanned_result(skew_char[cur_val])(int_val)
@y
begin scan_font_ident;
if m=0 then scanned_result(hyphen_char[cur_val])(int_val)
else if m=1 then scanned_result(skew_char[cur_val])(int_val)
else if m=no_lig_code then scanned_result(test_no_ligatures(cur_val))(int_val)
@z

@x [577]
@<Declare procedures that scan font-related stuff@>=
function get_tag_code(f: internal_font_number; c: eight_bits): integer;
@y
@<Declare procedures that scan font-related stuff@>=
function test_no_ligatures(f: internal_font_number): integer;
label exit;
var c:integer; 
begin
 test_no_ligatures:= 1;
 for c := font_bc[f] to font_ec[f] do
    if char_exists(orig_char_info(f)(c)) then
      if odd(char_tag(orig_char_info(f)(c))) then begin
        test_no_ligatures:= 0;
        return;
      end;
 exit:
end; 

function get_tag_code(f: internal_font_number; c: eight_bits): integer;
@z

@x [???]
function init_font_base(v: integer): integer;
@y
procedure set_no_ligatures(f: internal_font_number);
var c:integer; 
begin
    for c := font_bc[f] to font_ec[f] do
        if char_exists(orig_char_info(f)(c)) then
            if char_tag(orig_char_info(f)(c))=lig_tag then
                op_byte(orig_char_info(f)(c)) := (op_byte(orig_char_info(f)(c))) - lig_tag;
end;

function init_font_base(v: integer): integer;
@z

@x [1253]
assign_font_int: begin n:=cur_chr; scan_font_ident; f:=cur_val;
  if n < lp_code_base then begin
@y
assign_font_int: begin n:=cur_chr; scan_font_ident; f:=cur_val;
  if n = no_lig_code then set_no_ligatures(f)
  else if n < lp_code_base then begin
@z

@x [1254]
primitive("efcode",assign_font_int,ef_code_base);
@!@:ef_code_}{\.{\\efcode} primitive@>
@y
primitive("efcode",assign_font_int,ef_code_base);
@!@:ef_code_}{\.{\\efcode} primitive@>
primitive("pdfnoligatures",assign_font_int,no_lig_code);
@!@:no_lig_code_}{\.{\\pdfnoligatures} primitive@>
@z

@x [1255]
ef_code_base: print_esc("efcode");
@y
ef_code_base: print_esc("efcode");
no_lig_code: print_esc("pdfnoligatures");
@z
