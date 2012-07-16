@x
@d my_name=='ppltotf'
@d banner=='This is pPLtoTF, Version 3.5-p1.8'
@y
@d my_name=='uppltotf'
@d banner=='This is upPLtoTF, Version 3.5-p1.8-u1.10'
@z

@x
      usage_help (PPLTOTF_HELP, nil);
@y
      usage_help (UPPLTOTF_HELP, nil);
@z

@x
max_kanji=7237; { maximam number of 2byte characters }
@y
max_kanji=65535; { maximam number of 2byte characters }
@z

@x function get_next_raw
if multistrlen(ustringcast(buffer),loc+2,loc)=2 then cur_char:=" "
@y
if multistrlen(ustringcast(buffer),loc+3,loc)>1 then cur_char:=" "
@z

@x function valid_jis_code
var @!first_byte,@!second_byte:integer; { jis code bytes }
begin valid_jis_code:=true;
first_byte:=cx div @'400; second_byte:=cx mod @'400;
if (first_byte<@"21)
   or((first_byte>@"28)and(first_byte<@"30))
   or(first_byte>@"74) then valid_jis_code:=false;
if (second_byte<@"21)or(second_byte>@"7E) then valid_jis_code:=false;
@y
begin valid_jis_code:=true;
if (cx>@"FFFF)or(not is_char_kanji(fromDVI(cx)))
  or(toDVI(fromDVI(cx))<>cx) then valid_jis_code:=false;
@z

@x function jis_to_index
var @!first_byte,@!second_byte:integer; { jis code bytes }
begin
first_byte:=jis div @'400 -@"21;
second_byte:=jis mod @'400 -@"21;
if first_byte<8 then
  jis_to_index:=first_byte*94+second_byte
else { next |first_byte| start 16 }
  jis_to_index:=(first_byte-7)*94+second_byte;
@y
begin
jis_to_index:=jis;
@z

@x function index_to_jis
begin if ix<=8*94-1 then
  index_to_jis:=(ix div 94 +@"21)*@'400+(ix mod 94 +@"21)
else
  index_to_jis:=((ix+7*94) div 94 +@"21)*@'400+((ix+7*94) mod 94 +@"21);
@y
begin
index_to_jis:=ix;
@z

@x function get_kanji
else if multistrlen(ustringcast(buffer), loc+2, loc)=2 then
  begin jis_code:=toDVI(fromBUFF(ustringcast(buffer), loc+2, loc));
  incr(loc); cur_char:=" ";
@y
else if (ch='U')or(ch='u') then
  begin repeat ch:=get_next_raw; until ch<>' ';
  cx:=todig(xord[ch])*@"1000;
  incr(loc); ch:=xord[buffer[loc]]; cx:=cx+todig(ch)*@"100;
  incr(loc); ch:=xord[buffer[loc]]; cx:=cx+todig(ch)*@"10;
  incr(loc); ch:=xord[buffer[loc]]; cx:=cx+todig(ch);
  jis_code:=toDVI(fromUCS(cx));
  cur_char:=ch;
  if not valid_jis_code(jis_code) then
    err_print('jis code ', jis_code:1, ' is invalid');
  end
else if multistrlen(ustringcast(buffer), loc+4, loc)>1 then
  begin cur_char:=" ";
  jis_code:=toDVI(fromBUFF(ustringcast(buffer), loc+4, loc));
  loc:=loc+multistrlen(ustringcast(buffer), loc+4, loc)-1;
@z

