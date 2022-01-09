Changes for WEAVE.WEB to mogrify WEAVE into TWILL.

See the 'git log' in https://github.com/ascherer/twill and
https://github.com/ascherer/web for details.

Public domain.  Originally written by Andreas Scherer, 2022.

Limbo.

@x 2,29c2,5
% Version 0 was released in December, 1981.
% Version 1 was released in September, 1982, with version 0 of TeX.
% Slight changes were made in October, 1982, for version 0.6 of TeX.
% Version 1.1 changed "_" to "\_" if not within an identifier (November, 1982).
% Version 1.2 added @@= and @@\ and marked changed modules (December, 1982).
% Version 1.3 marked and indexed changed modules better (January, 1983).
% Version 1.4 added "history" (February, 1983).
% Version 1.5 conformed to TeX version 0.96 (March, 1983).
% Version 1.6 conformed to TeX version 0.98 (May, 1983).
% Version 1.7 introduced the new change file format (June, 1983).
% Version 2 was released in July, 1983, with version 0.999 of TeX.
% Version 2.1 corrected a bug in changed_module reckoning (August, 1983).
% Version 2.2 corrected it better (August, 1983).
% Version 2.3 starts the output with \input webmac (August, 1983).
% Version 2.4 fixed a bug in compress(#) (September, 1983).
% Version 2.5 cleared xrefswitch after module names (November, 1983).
% Version 2.6 fixed a bug in declaration of trans array (January, 1984).
% Version 2.7 fixed a bug in real constants (August, 1984).
% Version 2.8 fixed a bug in change_buffer movement (August, 1985).
% Version 2.9 increased max_refs and max_toks to 30000 each (January, 1987).
% Version 3, for Sewell's book, fixed long-line bug in input_ln (March, 1989).
% Version 3.1 fixed a bug for programs with only one module (April, 1989).
% Version 4 was major change to allow 8-bit input (September, 1989).
% Version 4.1, for Breitenlohner, avoids English-only output (March, 1990).
% Version 4.2 conforms to ANSI standard for-loop rules (September, 1990).
% Version 4.3 catches extra } in input (Breitenlohner, September, 1991).
% Version 4.4 corrects changed_module logic, %-overflow (January, 1992).
% Version 4.5 corrects archaic @@z logic and empty change file (January, 2021).
@y
% But it is not polished either, so you probably shouldn't try....
% Version 2.8 (based on WEAVE 2.8) was hacked together on 28 Sep 1985.
% Version 2.9 fixed output for dtype_string (14 Jun 1987).
% Version 4.5 (based on WEAVE 4.5) was hacked together on 04 Jan 2022.
@z

@x 48c24
  \centerline{\titlefont The {\ttitlefont WEAVE} processor}
@y
  \centerline{\titlefont The {\ttitlefont TWILL} processor}
@z

@x 43c19
\let\maybe=\iffalse
\def\title{WEAVE changes for C}
@y
\let\maybe=\iftrue
\def\title{TWILL for \TeX~Live}
@z

Section 1.

@x 54a31,35
@* Introduction.
@y
@* Introduction.
[Apology: This modification of \.{WEAVE} was put together hastily at the
end of September, 1985, in order to prepare the listing of \TeX,
on the WAITS system. No attempt has been made to polish anything
or to make this code usable by anyone else but its author.]

@z

@x 77c58
@d banner=='This is WEAVE, Version 4.5'
@y
@d banner=='This is TWILL, Version 4.5'
@z

Section 8.

@x 203a185
@!stack_size=2000; {number of simultaneous output levels}
@y
@!stack_size=2000; {number of simultaneous output levels}
@!max_new_refs=200; {number of different references to other modules}
@z

Section 37.

@x 742a725
@!xref: array [0..max_names] of sixteen_bits; {heads of cross-reference lists}
@y
@!xref: array [0..max_names] of sixteen_bits; {heads of cross-reference lists}
@!def_val: array [0..max_names] of integer; {values of numeric macros}
@z

Section 46.

@x 889a873,876
@d xlink(#)==xmem[#].xlink_field
@y
@d xlink(#)==xmem[#].xlink_field
@d dtype(#)==xmem[#].dtype_field
@d dname(#)==xmem[#].dname_field
@d dback(#)==xmem[#].dback_field
@d dlink(#)==xmem[#].dlink_field
@z

Section 48.

@x 895c882,899
@ @<Globals...@>=
@y
@ Definitions are classified into 15 types.
@d dtype_none=0
@d dtype_macro=1
@d dtype_const=2
@d dtype_string=3
@d dtype_colon_bold=4
@d dtype_equal_bold=5
@d dtype_colon_packed=6
@d dtype_equal_packed=7
@d dtype_colon_ital=8
@d dtype_equal_ital=9
@d dtype_comma=10
@d dtype_colon_const_dots=11
@d dtype_equal_const_dots=12
@d dtype_colon_ital_dots=13
@d dtype_equal_ital_dots=14

@<Globals...@>=
@z

@x 898a903,906
  @!xlink_field: sixteen_bits; {pointer to the previous cross reference}
@y
  @!xlink_field: sixteen_bits; {pointer to the previous cross reference}
  @!dtype_field: sixteen_bits; {type of definition}
  @!dname_field: sixteen_bits; {identifier or constant}
  @!dback_field: sixteen_bits; {if nonzero, this is a reference to name}
  @!dlink_field: sixteen_bits; {link to definitions only}
@z

@x 901a910,912
@!xref_switch,@!mod_xref_switch:0..def_flag; {either zero or |def_flag|}
@y
@!xref_switch,@!mod_xref_switch:0..def_flag; {either zero or |def_flag|}
@!def_type,@!def_name,@!def_subtype,@!def_subname,
  @!const_name,@!packed_name:integer;
@!danger_zone:boolean;
@z

Section 49.

@x 903a915
@ @<Set init...@>=xref_ptr:=0; xref_switch:=0; mod_xref_switch:=0; num(0):=0;
@y
@ @<Set init...@>=xref_ptr:=0; xref_switch:=0; mod_xref_switch:=0; num(0):=0;
danger_zone:=false;
@z

Section 50.

@x 908a...
to one-letter identifiers or \PASCAL's reserved words.
@y
to one-letter identifiers or \PASCAL's reserved words.

The |new_blank_xref| is similar but it ignores the |xref_switch|.
@z

@x 911a924
  else  begin incr(xref_ptr); num(xref_ptr):=#;
@y
  else  begin incr(xref_ptr); num(xref_ptr):=#;
    dback(xref_ptr):=0;
@z

@x 925,926c938,942
    begin num(q):=m; return;
    end;
@y
    if (def_type>=dtype_comma) or danger_zone then
      q:=xlink(q) {delete entry}
    else begin num(q):=m; dtype(q):=def_type;
      dname(q):=def_name; dback(q):=p; return;
      end;
@z

@x 928,929c945,952
append_xref(m); xlink(xref_ptr):=q; xref[p]:=xref_ptr;
exit: end;
@y
append_xref(m); xlink(xref_ptr):=q; xref[p]:=xref_ptr;
if m>def_flag then
  begin dtype(xref_ptr):=def_type; dname(xref_ptr):=def_name;
  dback(xref_ptr):=p;
  if def_type>dtype_comma then
    begin append_xref(0); dtype(xref_ptr):=def_subtype;
    dname(xref_ptr):=def_subname;
    end;
  end;
exit: end;
@#
procedure new_blank_xref(@!p:integer);
var xs:integer;
begin xs:=xref_switch; xref_switch:=0; new_xref(p); xref_switch:=xs;
end;
@z

Section 62.

@x 1064a1096,1097
@ When we begin the following segment of the program, |p=name_ptr|.
@y
@ When we begin the following segment of the program, |p=name_ptr|.

@d undef_val==10000000

@z

@x 1074c1107
ilk[p]:=t; xref[p]:=0;
@y
ilk[p]:=t; xref[p]:=0; def_val[p]:=undef_val;
@z

Section 64.

@x 1112c1145
id5("c")("o")("n")("s")("t")(const_like);@/
@y
id5("c")("o")("n")("s")("t")(const_like); const_name:=cur_name;@/
@z

@x 1130c1163
id6("p")("a")("c")("k")("e")("d")(goto_like);@/
@y
id6("p")("a")("c")("k")("e")("d")(goto_like); packed_name:=cur_name;@/
@z

Section 71.

@x 1278,1279d1310
@!change_pending: boolean; {if |true|, the current change is not yet
  recorded in |changed_module[module_count]|}
@y
@z

Section 79.

@x 1371,1383d1401
When a match is found, the current module is marked as changed unless
the first line after the \.{@@x} and after the \.{@@y} both start with
either |'@@*'| or |'@@ '| (possibly preceded by whitespace).

@d if_module_start_then_make_change_pending(#)==
  loc:=0; buffer[limit]:="!";
  while (buffer[loc]=" ")or(buffer[loc]=tab_mark) do incr(loc);
  buffer[limit]:=" ";
  if buffer[loc]="@@" then
    if (buffer[loc+1]="*") or
       (buffer[loc+1]=" ") or (buffer[loc+1]=tab_mark) then
      change_pending:=#

@y
@z

@x 1389,1393d1406
change_pending:=false;
if not changed_module[module_count] then
  begin if_module_start_then_make_change_pending(true);
  if not change_pending then changed_module[module_count]:=true;
  end;
@y
@z

Section 82.

@x 1450,1454c1463,1469
begin restart:if changing then
  @<Read from |change_file| and maybe turn off |changing|@>;
if not changing then
  begin @<Read from |web_file| and maybe turn on |changing|@>;
  if changing then goto restart;
@y
begin restart: if changing then changed_module[module_count]:=true
else  @<Read from |web_file| and maybe turn on |changing|@>;
if changing then
  begin @<Read from |change_file| and maybe turn off |changing|@>;
  if not changing then
    begin changed_module[module_count]:=true; goto restart;
    end;
@z

Section 83.

@x 1462c1477,1479
else if change_limit>0 then check_change;
@y
else if limit=change_limit then
  if buffer[0]=change_buffer[0] then
    if change_limit>0 then check_change;
@z

Section 84.

@x 1472,1479c1489
if limit>0 then {check if the change has ended}
  begin if change_pending then
    begin if_module_start_then_make_change_pending(false);
    if change_pending then
      begin changed_module[module_count]:=true; change_pending:=false;
      end;
    end;
  buffer[limit]:=" ";
@y
if limit>1 then {check if the change has ended}
@z

@x 1491d1500
  end;
@y
@z

Section 88.

@x 1594a1604,1606
"0","1","2": begin tracing:=c-"0"; control_code:=ignore;
@y
"0","1","2": begin tracing:=c-"0"; control_code:=ignore;
"3": begin @{'*****************************************************'@}
  control_code:=ignore; {to set a breakpoint}
  end;
@z

Section 110.

@x 1965c2032
changed_module[module_count]:=changing;
@y
changed_module[module_count]:=false;
@z

Section 111.

@x 1991c...
@p procedure Pascal_xref; {makes cross references for \PASCAL\ identifiers}
@y
@p @<Functions |scan_const| and |scan_exp|@>
@#
procedure Pascal_xref; {makes cross references for \PASCAL\ identifiers}
@z

@x 1992c2059
label exit;
@y
label exit,done,found,not_found;
@z

@x 1993a2061
var p:name_pointer; {a referenced name}
@y
var p:name_pointer; {a referenced name}
@!eq:0..3; {addition to |dtype| code}
@z

@x 1997c2065,2069
    begin p:=id_lookup(next_control-identifier); new_xref(p);
@y
    begin p:=id_lookup(next_control-identifier);
    if next_control=identifier then if xref_switch<>0 then if ilk[p]=normal then
      @<Figure out the |def_type| and |def_name|, etc.@>;
    new_xref(p);
    danger_zone:=(def_type=dtype_comma);
@z

@x 1999a2072,2078
      xref_switch:=def_flag; {implied `\.{@@!}'}
@y
      xref_switch:=def_flag; {implied `\.{@@!}'}
    if ilk[p]=proc_like then
      begin def_name:=p; next_control:=get_next;
      if next_control<>identifier then goto done;
      if xref_switch=0 then goto done;
      p:=id_lookup(normal); def_type:=dtype_colon_bold;
      new_xref(p);
      end;
@z

@x 2002c2081
  if (next_control="|")or(next_control="{") then return;
@y
done:  if (next_control="|")or(next_control="{") then return;
@z

Section 115.

@x 2061c2208,2226
  if next_control=definition then next_control:=get_next
@y
  if next_control=definition then
    begin next_control:=get_next;
    if next_control=identifier then
      begin lhs:=id_lookup(normal);
      next_control:=get_next;
      if (next_control=equivalence_sign)or(next_control="(") then
        begin def_type:=dtype_macro; new_xref(lhs);
        end
      else if next_control="=" then
        begin xref_switch:=0; next_control:=get_next;
        def_val[lhs]:=scan_exp;
        def_name:=def_val[lhs]; def_type:=dtype_const;
        xref_switch:=def_flag;
        if abs(def_name)>=32768 then def_type:=dtype_macro
        else if def_name<0 then def_name:=def_name+65536;
        new_xref(lhs);
        end;
      end;
    end
@z

Section 124.

@x 2199,2201c2364
`\.{\\input webmac}'.
@.\\input webmac@>
@.webmac@>
@y
`\.{\\input twimac-web}'.
@.\\input twimac-web@>
@.twimac-web@>
@z

@x 2204c2367
out_ptr:=1; out_line:=1; out_buf[1]:="c"; write(tex_file,'\input webma');
@y
out_ptr:=1; out_line:=1; out_buf[1]:="b"; write(tex_file,'\input twimac-we');
@z

Section 125.

@x 2219a2383,2389
@d oot5(#)==oot(#)@,oot4
@y
@d oot5(#)==oot(#)@,oot4
@d oot6(#)==oot(#)@,oot5
@d oot7(#)==oot(#)@,oot6
@d oot8(#)==oot(#)@,oot7
@d oot9(#)==oot(#)@,oot8
@d oot10(#)==oot(#)@,oot9
@d oot11(#)==oot(#)@,oot10
@d oot12(#)==oot(#)@,oot11
@z

@x 2224a2395,2401
@d out5==@+begin oot5
@y
@d out5==@+begin oot5
@d out6==@+begin oot6
@d out7==@+begin oot7
@d out8==@+begin oot8
@d out9==@+begin oot9
@d out10==@+begin oot10
@d out11==@+begin oot11
@d out12==@+begin oot12
@z

Section 130.

@x 2273c...
@ The number to be converted by |out_mod| is known to be less than
|def_flag|, so it cannot have more than five decimal digits.  If
the module is changed, we output `\.{\\*}' just after the number.
@y
@ The number to be converted by |out_mod| is known to be less than
|def_flag|, so it cannot have more than five decimal digits.
@z

@x 2285d
if changed_module[m] then out2("\")("*");
@.\\*@>
@y
@z

Section 133.

@x 2333,2334c2510,2512
  out("@@");
  if c<>"@@" then err_print('! Double @@ required outside of sections');
@.Double \AT! required...@>
@y
  if (c<>"z")and(c<>"Z") then
    begin out("@@");
    if c<>"@@" then err_print('! Double @@ required outside of sections');
@.Double \AT! required...@>
    end;
@z

Section 146.

@x 2888c...
@ Token lists in |@!tok_mem| are composed of the following kinds of
@y
@ Token lists in |tok_mem| are composed of the following kinds of
@z

Section 183.

@x 3522a3702
@!p:name_pointer; {identifier designator}
@y
@!p:name_pointer; {identifier designator}
@!q,@!qq,@!r:integer; {registers for new reference insertion loop}
@z

Section 191.

@x 3698a3879
begin p:=id_lookup(normal);
@y
begin p:=id_lookup(normal);
if ilk[p]=normal then @<Insert a new reference, if this is new@>;
@z

Section 197.

@x 3816c...
@p function Pascal_translate: text_pointer;
@y
@d flaky=1
@d guaranteed=0

@p function Pascal_translate: text_pointer;
@z
@x 3819c4033
begin save_base:=scrap_base; scrap_base:=scrap_ptr+1;
@y
begin save_base:=scrap_base; scrap_base:=scrap_ptr+1; safety:=flaky;
@z

@x 3827c4041
Pascal_translate:=p;
@y
safety:=guaranteed; Pascal_translate:=p;
@z

Section 218.

@x 4226c4440
module_count:=0;
@y
module_count:=0; xx:=0;
@z

@x 4228a4443
finish_line; flush_buffer(0,false,false); {insert a blank line, it looks nice}
@y
finish_line; flush_buffer(0,false,false); {insert a blank line, it looks nice}
@<Prepare high-speed access to definitions via |dlink| and |def_val|@>;
@z

Section 220.

@x 4249a4492,4493
begin incr(module_count);@/
@y
begin incr(module_count);@/
mm:=module_count+def_flag; ref_link[0]:=0; ref_loc[0]:=0; new_ref_ptr:=0;
safety:=guaranteed;
@z

Section 236.

@x 4503a4749
if flag=0 then out("U")@+else out("A");
@y
if flag=0 then out("U")@+else out("A");
out8(" ")("s")("e")("c")("t")("i")("o")("n");
@z

@x 4514,4515c4760
@.\\As@>
@.\\Us@>
@y
out("~");
@z

Section 237.

@x 4519,4524c4764,4767
  if num(xlink(cur_xref))>flag then out2(",")(" ") {not the last}
  else begin out3("\")("E")("T"); {the last}
@.\\ET@>
    if cur_xref<>xlink(q) then out("s"); {the last of more than two}
@.\\ETs@>
    end;
@y
  if (num(xlink(cur_xref))>flag)or(cur_xref<>xlink(q)) then out(",");
    {not the last of two}
  out(" ");
  if num(xlink(cur_xref))<=flag then out4("a")("n")("d")("~"); {the last}
@z

Section 238.

@x 4529c4772,4776
out3("\")("f")("i"); finish_line;
@y
out6("\")("m")("i")("n")("i")("%");
@.\\mini@>
beta_out;
flush_buffer(out_ptr,false,false);
out4("}")("\")("F")("I"); finish_line;
@z

@x 4531c4778,4893
@.\\fi@>
@y
@.\\FI@>
@z

Section 241.

@x 4561a4924
k_module:=1;
@y
k_module:=1;
while not changed_module[k_module] do incr(k_module);
@z

@x 4563,4568d4925
while k_module<module_count do
  begin if changed_module[k_module] then
    begin out_mod(k_module); out2(",")(" ");
    end;
  incr(k_module);
  end;
@y
@z

@x 4569a4927,4929
out_mod(k_module);
@y
out_mod(k_module);
repeat repeat incr(k_module)@+ until changed_module[k_module];
  out2(",")(" "); out_mod(k_module);
until k_module=module_count;
@z

Section 254.

@x 4726c5086,5093
if cur_val<def_flag then out_mod(cur_val)
@y
if cur_val<def_flag then
  begin out_mod(cur_val);
  if cur_val+1=num(xlink(cur_xref)) then
    begin out2("-")("-");
    repeat cur_xref:=xlink(cur_xref); incr(cur_val);
    until cur_val+1<>num(xlink(cur_xref)); out_mod(cur_val);
    end;
  end
@z

Section 261.

@x 4842c...
@p procedure Phase_I;
@y
@p @<Functions |alpha_out|, |beta_out|, and helpers@>
@#
procedure Phase_I;
@z

@x 4846a5214
procedure Phase_II;
@y
procedure Phase_II;
var lhs:integer;
@z

Section 264.

@x WEAVE.CH
      usage_help (WEAVE_HELP, nil);
@y
      usage_help (TWILL_HELP, nil);
@z

@x
@!web_name,@!chg_name,@!tex_name:const_c_string;
@y
@!web_name,@!chg_name,@!tex_name:const_c_string;

@* New material for \.{TWILL}.
Here's a new subroutine needed for \.{TWILL}. Assuming that |next_control|
is the beginning of a numeric constant, and that string constants have
length~1, the |scan_const| function returns the value of the constant
and sets |next_control| to the following token.

@<Functions |scan_const| and |scan_exp|@>=
function scan_const:integer;
label done;
var radix,@!accum,p:integer;
begin if next_control=string then
  begin accum:=buffer[id_first+1]; next_control:=get_next; goto done;
  end
else if next_control=identifier then
  begin p:=id_lookup(normal); new_blank_xref(p);
  accum:=def_val[p]; next_control:=get_next; goto done;
  end
else  begin accum:=0;
  if next_control=hex then radix:=16
  else if next_control=octal then radix:=8
  else  begin radix:=10; accum:=next_control-"0";
    end;
  loop  begin next_control :=get_next;
    if next_control<"0" then goto done;
    if radix=16 then
      begin if (next_control>="A")and(next_control<="F") then
        next_control:=next_control-"A"+"0"+10
      else if next_control>"9" then goto done;
      end
    else if next_control>="0"+radix then goto done;
    accum:=accum*radix+next_control-"0";
    end;
  end;
done: scan_const:=accum;
end;

@ Simple linear arithmetic is handled by the following
subroutine, which doesn't complain about certain syntactic errors.

@d start_of_const(#)==(((#>="0")and(#<="9"))or(#=hex)or(#=octal)or(#=string)
  or(#=identifier))
@d sign(#)==(abs(#-",")=1)
@d start_of_num(#)==(start_of_const(#)or sign(#))

@<Functions |scan_const| and |scan_exp|@>=
function scan_exp:integer;
label done;
var @!accum,s:integer;
begin if sign(next_control) then accum:=0
else accum:=scan_const;
loop  begin if not sign(next_control) then goto done;
  s:=","-next_control; next_control:=get_next;
  if not start_of_const(next_control) then goto done;
  accum:=accum+s*scan_const;
  end;
done:scan_exp:=accum;
end;

@ @d found_it(#)==begin def_type:=#; goto found; end

@<Figure out the |def_type| and |def_name|, etc.@>=
begin next_control:=get_next;
if next_control="," then found_it(dtype_comma);
if(next_control=":")or(next_control="=") then
  @<Figure out a type and |goto| either |found| or |not_found|@>;
not_found:def_type:=dtype_none; new_xref(p); goto done;
found: end

@ @<Figure out a type and...@>=
begin eq:=(next_control-":") div ("="-":");
next_control:=get_next;
if next_control=identifier then
  @<Figure out a type starting with an identifier;
    |goto| |found| or |not_found| unless it's a subrange@>
else if (next_control=string)and(id_loc-id_first>2) then found_it(dtype_string)
else if start_of_num(next_control) then
  @<Figure out a type starting with a constant;
    |goto| |found| or |not_found| unless it's a subrange@>
else goto not_found;
next_control:=get_next;
if next_control=identifier then
  begin def_subname:=id_lookup(normal);
  if ilk[def_subname]<>normal then goto not_found;
  if def_val[def_subname]=undef_val then
    begin new_blank_xref(def_subname);
    def_subtype:=dtype_colon_ital_dots; goto found;
    end;
  end;
if start_of_num(next_control) then
  begin def_subname:=scan_exp; def_subtype:=dtype_colon_const_dots;
  if abs(def_subname)>=32768 then goto not_found;
  if def_subname<0 then def_subname:=def_subname+65536;
  goto found;
  end;
goto not_found;
end

@ @<Figure out a type starting with an identifier...@>=
begin def_name:=id_lookup(normal);
if ilk[def_name]=goto_like then {\&{packed}}
  begin next_control:=get_next; eq:=eq+2;
  if next_control<>identifier then goto not_found;
  def_name:=id_lookup(normal);
  end;
if (ilk[def_name]=array_like)or(ilk[def_name]=record_like) then
  found_it(dtype_colon_bold+eq);
if ilk[def_name]<>normal then goto found;
new_blank_xref(def_name);
if def_val[def_name]=undef_val then
  begin next_control:=get_next;
  if next_control=double_dot then def_type:=dtype_colon_ital_dots+eq
  else found_it(dtype_colon_ital+eq);
  end
else @<Figure out a type starting with a constant...@>;
end

@ @<Figure out a type starting with a constant...@>=
begin def_name:=scan_exp;
if next_control<>double_dot then
  if eq=1 then found_it(dtype_equal_bold; def_name:=const_name)
  else goto not_found;
if abs(def_name)>=32768 then goto not_found;
if def_name<0 then def_name:=def_name+65536;
def_type:=dtype_colon_const_dots+eq;
end

@ Some identifiers can be inserted during Phase II that weren't
seen in Phase I (namely, if they appear only in module names);
so we have to watch out that |def_val| might be |undef_val|.

@<Insert a new reference, if this is new@>=
begin q:=def_val[p];
if q<>0 then if q<>undef_val then if not phase_three then
  begin repeat r:=q; q:=dlink(q);
  until (q=0) or (num(q)>mm);
  if num(r)<>mm then
    begin qq:=0; q:=ref_link[0];
    while ref_loc[q]>r do
      begin qq:=q; q:=ref_link[qq];
      end;
    if ref_loc[q]=r then
      begin if safety=guaranteed then
       if ref_safety[q]=flaky then ref_safety[q]:=guaranteed;
      end
    else  begin if new_ref_ptr=max_new_refs then overflow('new references');
      incr(new_ref_ptr);
      ref_link[new_ref_ptr]:=q; ref_link[qq]:=new_ref_ptr;
      ref_loc[new_ref_ptr]:=r;
      if dlink(def_val[p])=0 then ref_safety[new_ref_ptr]:=guaranteed
      else ref_safety[new_ref_ptr]:=safety;
      end;
    end;
  end;
end

@ @<Prepare high-speed access to definitions via |dlink| and |def_val|@>=
for lhs:=1 to name_ptr do if ilk[lhs]=normal then
  begin def_val[lhs]:=0; rhs:=xref[lhs];
  while rhs<>0 do
    begin if num(rhs)>def_flag then
      begin dlink(rhs):=def_val[lhs];
      def_val[lhs]:=rhs;
      end;
    rhs:=xlink(rhs);
    end;
  end

@ We keep a separate list of all references made in the current module,
sorted by |xref| number. A reference is considered to need manual checking
if it appears only in a comment within the section and if the corresponding
identifier is multiply defined.

@<Glob...@>=
@!mm:integer; {current module number plus |def_flag|}
@!ref_loc:array[0..max_new_refs] of sixteen_bits;
@!ref_link:array[0..max_new_refs] of sixteen_bits;
@!ref_safety:array[0..max_new_refs] of guaranteed..flaky;
@!new_ref_ptr:0..max_new_refs;
@!safety:guaranteed..flaky;
@!xx:xref_number;

@ The |alpha_out| procedure makes entries for all identifiers defined
in the current module. (However, I no longer need these!)

@<Functions |alpha_out|, |beta_out|, and helpers@>=
procedure alpha_out;
label exit;
var p,w,k:integer;
begin loop begin
  if xx=xref_ptr then return;
  if num(xx+1)>mm then return;
  incr(xx);
        if num(xx)>def_flag then if dback(xx)>0 then
   if ilk[dback(xx)]=normal then
    begin flush_buffer(out_ptr,false,false);
    out5("\")("m")("i")("n")("i");
@.\\mini@>
    p:=dback(xx);
    w:=p mod ww;
    for k:=byte_start[p] to byte_start[p+ww]-1 do out(byte_mem[w,k]);
    if dtype(xx)>dtype_comma then incr(xx);
    end;
  end;
exit:end;

@ Here's a procedure that's very much like |out_mod|.

@<Functions |alpha_out|, |beta_out|, and helpers@>=
procedure out_const(@!n:sixteen_bits);
var a,k:integer;
begin a:=n; k:=0;
if a>=32768 then
  begin out("-"); a:=65536-a;
  end;
repeat dig[k]:=a mod 10; a:=a div 10; incr(k);
until a=0;
repeat decr(k); out(dig[k]+"0");
until k=0;
end;

@ And here's something that could have been made a subroutine earlier.

@<Functions |alpha_out|, |beta_out|, and helpers@>=
procedure out_id(@!p:integer);
begin out("\");
if ilk[p]=normal then
  if length(p)=1 then out("|")
@.\\|@>
  else out("\")
@.\\\\@>
else out("&");
@.\\\&@>
if length(p)=1 then out(byte_mem[p mod ww,byte_start[p]])
else out_name(p);
end;

@ The |beta_out| procedure makes entries for all identifiers used
but not defined in the current module.

@<Functions |alpha_out|, |beta_out|, and helpers@>=
procedure beta_out;
label done,888,found;
var k,p,q,w,xx,mmm:integer;
begin p:=ref_link[0];
while p<>0 do
  begin flush_buffer(out_ptr,false,false);
  out2("\")("[");
  xx:=ref_loc[p]; q:=dback(xx); w:=q mod ww;
  for k:=byte_start[q] to byte_start[q+ww]-1 do out(byte_mem[w,k]);
  out(" "); out_const(num(xx)-def_flag);
  out(" ");
  @<Move past commas@>;
  @<Output the reference, based on its |dtype|@>;
  if ref_safety[p]=flaky then out3(" ")("%")("?");
  p:=ref_link[p];
  end;
end;

@ @<Output the reference, based on its |dtype|@>=
case dtype(xx) of
dtype_none: out5("\")("n")("o")("n")("e");
dtype_macro: out6("=")("m")("a")("c")("r")("o");
dtype_const: begin out2("=")("$"); out_const(dname(xx)); out("$");
  end;
dtype_string:
  out12("\")(".")("{")("""")("s")("t")("r")("i")("n")("g")("""")("}");
dtype_colon_bold,dtype_colon_ital: begin out(":"); out_id(dname(xx)); end;
dtype_equal_bold,dtype_equal_ital: begin out("="); out_id(dname(xx)); end;
dtype_colon_packed: begin out(":"); out_id(packed_name); out2("\")(" ");
 out_id(dname(xx)); end;
dtype_equal_packed: begin out("="); out_id(packed_name); out2("\")(" ");
 out_id(dname(xx)); end;
dtype_colon_const_dots: begin out2(":")("$"); out_const(dname(xx));
 goto 888; end;
dtype_equal_const_dots: begin out2("=")("$"); out_const(dname(xx));
 goto 888; end;
dtype_colon_ital_dots: begin out2(":")("$"); out_id(dname(xx)); goto 888; end;
dtype_equal_ital_dots: begin out2("=")("$"); out_id(dname(xx)); goto 888; end;
end; {there are no other cases}
goto found;
888: out3("\")("t")("o");
 if dtype(xx+1)=dtype_colon_ital_dots then
  out_id(dname(xx+1))
 else out_const(dname(xx+1));
 out("$");
found:

@ @<Move past commas@>=
mmm:=num(xx);
loop  begin if dtype(xx)<>dtype_comma then if num(xx)=mmm then goto done;
  if xx=xref_ptr then
    begin dtype(xx):=dtype_none; goto done;
    end;
  if num(xx+1)>mmm then
    begin dtype(xx):=dtype_none; goto done;
    end;
  incr(xx);
  end;
done:
@z
