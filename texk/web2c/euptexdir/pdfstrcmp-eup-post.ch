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
