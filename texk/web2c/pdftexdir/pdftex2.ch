% WEB change file containing pdftex codes that are conveninent to be here
% instead of pdftex.ch due to changes files coming after pdftex.ch (hz.ch &
% vadjust.ch)
%
% Copyright (c) 1996-2005 Han Th\^e\llap{\raise 0.5ex\hbox{\'{}}} Th\`anh, <thanh@pdftex.org>
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
% $Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/pdftex2.ch#5 $

@x [230] - tagcode
@d ef_code_base == 4
@y
@d ef_code_base == 4
@d tag_code == 5
@z

@x [230] - tagcode
    ef_code_base: scanned_result(get_ef_code(n, k))(int_val);
@y
    ef_code_base: scanned_result(get_ef_code(n, k))(int_val);
    tag_code: scanned_result(get_tag_code(n, k))(int_val);
@z

@x [???] - tagcode
function init_font_base(v: integer): integer;
@y
procedure set_tag_code(f: internal_font_number; c: eight_bits; i: integer);
var fixedi:integer;
begin
    if is_valid_char(c) then
    begin fixedi := abs(fix_int(i,-7,0));
      if fixedi >= 4 then begin
          if char_tag(char_info(f)(c)) = ext_tag then
          op_byte(char_info(f)(c)) := (op_byte(char_info(f)(c))) - ext_tag;
        fixedi := fixedi - 4;
      end;
      if fixedi >= 2 then begin
        if char_tag(char_info(f)(c)) = list_tag then
          op_byte(char_info(f)(c)) := (op_byte(char_info(f)(c))) - list_tag;
        fixedi := fixedi - 2;
      end;
      if fixedi >= 1 then begin
        if char_tag(char_info(f)(c)) = lig_tag then
          op_byte(char_info(f)(c)) := (op_byte(char_info(f)(c))) - lig_tag;
    end;
  end;
end;

function init_font_base(v: integer): integer;
@z

@x [1253] - tagcode
    ef_code_base: set_ef_code(f, p, cur_val);
@y
    ef_code_base: set_ef_code(f, p, cur_val);
    tag_code: set_tag_code(f, p, cur_val);
@z

@x [1254] - tagcode
primitive("efcode",assign_font_int,ef_code_base);
@!@:ef_code_}{\.{\\efcode} primitive@>
@y
primitive("efcode",assign_font_int,ef_code_base);
@!@:ef_code_}{\.{\\efcode} primitive@>
primitive("tagcode",assign_font_int,tag_code);
@!@:tag_code_}{\.{\\tagcode} primitive@>
@z

@x [1255]
ef_code_base: print_esc("efcode");
@y
ef_code_base: print_esc("efcode");
tag_code: print_esc("tagcode");
@z
