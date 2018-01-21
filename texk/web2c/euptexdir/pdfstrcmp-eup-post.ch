@x
    cur_tok := (cur_cmd*@'400)+cur_chr;
@y
    cur_tok := (cur_cmd*max_char_val)+cur_chr;
@z

@x
@d pdf_last_x_pos_code=eptex_version_code+1 {code for \.{\\pdflastxpos}}
@y
@d uptex_version_code=eptex_version_code+1 {code for \.{\\uptexversion}}
@d pdf_last_x_pos_code=uptex_version_code+1 {code for \.{\\pdflastxpos}}
@z

@x
@d uptex_version_code=ptex_minor_version_code+2 {code for \.{\\uptexversion}}
@y
@z

@x
procedure print_kanji(@!s:integer); {prints a single character}
begin
if s>255 then begin
  if isprint_utf8 then begin
    s:=UCStoUTF8(toUCS(s));
    if BYTE1(s)<>0 then print_char(BYTE1(s));
    if BYTE2(s)<>0 then print_char(BYTE2(s));
    if BYTE3(s)<>0 then print_char(BYTE3(s));
                        print_char(BYTE4(s));
  end
  else begin print_char(Hi(s)); print_char(Lo(s)); end;
end
else print_char(s);
end;
@y
procedure print_kanji(@!s:KANJI_code); {prints a single character}
begin
if isprint_utf8 then s:=UCStoUTF8(toUCS(s mod max_cjk_val))
else s:=toBUFF(s mod max_cjk_val);
if BYTE1(s)<>0 then print_char(BYTE1(s));
if BYTE2(s)<>0 then print_char(BYTE2(s));
if BYTE3(s)<>0 then print_char(BYTE3(s));
                    print_char(BYTE4(s));
end;
@z
