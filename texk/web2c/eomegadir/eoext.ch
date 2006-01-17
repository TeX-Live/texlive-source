% vim ft=ch
% Dealing with extended formats
% and changing the last Omega references to e-Omega
@x
@<Read the other strings from the \.{OMEGA.POOL} file and return |true|,
@y
@<Read the other strings from the \.{EOMEGA.POOL} file and return |true|,
@z
%----------------------------------------
@x
else  bad_pool('! I can''t read OMEGA.POOL.')
@.I can't read OMEGA.POOL@>

@ @<Read one string...@>=
begin if eof(pool_file) then bad_pool('! OMEGA.POOL has no check sum.');
@.OMEGA.POOL has no check sum@>
read(pool_file,m,n); {read two digits of string length}
if m='*' then @<Check the pool check sum@>
else  begin if (m<"0")or(m>"9")or@|
      (n<"0")or(n>"9") then
    bad_pool('! OMEGA.POOL line doesn''t begin with two digits.');
@.OMEGA.POOL line doesn't...@>
@y
else  bad_pool('! I can''t read EOMEGA.POOL.')
@.I can't read EOMEGA.POOL@>

@ @<Read one string...@>=
begin if eof(pool_file) then bad_pool('! EOMEGA.POOL has no check sum.');
@.EOMEGA.POOL has no check sum@>
read(pool_file,m,n); {read two digits of string length}
if m='*' then @<Check the pool check sum@>
else  begin if (m<"0")or(m>"9")or@|
      (n<"0")or(n>"9") then
    bad_pool('! EOMEGA.POOL line doesn''t begin with two digits.');
@.EOMEGA.POOL line doesn't...@>
@z
%----------------------------------------
@x
  bad_pool('! OMEGA.POOL check sum doesn''t have nine digits.');
@.OMEGA.POOL check sum...@>
  a:=10*a+n-"0";
  if k=9 then goto done;
  incr(k); read(pool_file,n);
  end;
done: if a<>@$ then bad_pool('! OMEGA.POOL doesn''t match; OTANGLE me again.');
@.OMEGA.POOL doesn't match@>
@y
  bad_pool('! EOMEGA.POOL check sum doesn''t have nine digits.');
@.EOMEGA.POOL check sum...@>
  a:=10*a+n-"0";
  if k=9 then goto done;
  incr(k); read(pool_file,n);
  end;
done: if a<>@$ then bad_pool('! EOMEGA.POOL doesn''t match; OTANGLE me again.');
@.EOMEGA.POOL doesn't match@>
@z
%----------------------------------------
@x
print_err("Omega capacity exceeded, sorry [");
@y
print_err("e-Omega capacity exceeded, sorry [");
@z
%----------------------------------------
@x
  print("Omega output, Version 3.141592--1.15, ");
@y
  print("e-Omega output, Version 3.141592--1.15--2.1, ");
@z
%----------------------------------------
@x
    print_err("Patterns can be loaded only by INIOMEGA");
@y
    print_err("Patterns can be loaded only by INIEOMEGA");
@z
%----------------------------------------
@x
format_ident:=" (INIOMEGA)";
@y
format_ident:=" (INIEOMEGA)";
@z
%----------------------------------------
@x
  print_nl("(\dump is performed only by INIOMEGA)"); return;
@:dump_}{\.{\\dump...only by INIOMEGA}@>
@y
  print_nl("(\dump is performed only by INIEOMEGA)"); return;
@:dump_}{\.{\\dump...only by INIEOMEGA}@>
@z
%----------------------------------------
@x
@!init if (buffer[loc]="*")and(format_ident=" (INITEX)") then
@y
@!init if (buffer[loc]="*")and(format_ident=" (INIEOMEGA)") then
@z
%%% The following lines would let e-Omega start in enhanced mode
% %----------------------------------------
% @x
%   incr(loc); eTeX_mode:=1; {enter extended mode}
% @y
%   if (buffer[loc]="*") then incr(loc);
%   eTeX_mode:=1; {enter extended mode}
% @z
% %----------------------------------------
% @x
% eTeX_mode:=0; {initially we are in compatibility mode}
% @y
% eTeX_mode:=1; {initially we are in extended mode}
% @z
% %----------------------------------------
% @x
% for j:=0 to eTeX_states-1 do set_new_eqtb_int(eTeX_state_base+j,0); {disable all enhancements}
% @y
% for j:=0 to eTeX_states-1 do set_new_eqtb_int(eTeX_state_base+j,1); {enable all enhancements}
% @z
