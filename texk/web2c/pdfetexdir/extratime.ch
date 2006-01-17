%
% extratime.ch -- benchmarking support
%
% (c) 2005 Taco Hoekwater, taco@elvenkind.com
%
% This change file implements an interface to the precise system 
% time (if available) as an aid in benchmarking TeX macro code. 
%
% Two new primitives are provided:
%
% \item |\elapsedtime| is a read-only integer that (initially)
% returns the amount of time passed since the start of this run.
% This amount is given in `scaled seconds': the value 65536 
% counts as one second.
% If more time has passed than 32767 seconds, |\maxint| will be
% returned.
%
% \item |\resettime| updates the internal timer, such that
% subsequent calls to |\elapsedtime| will restart from 0.
%
% Besides this web change file,
% there is also a new C function needed in |texmfmp.{c,h}|, and a 
% new definition in |texmf.defines| to keep |convert| happy.

% This change file is intended to be used as the last one in 
% pdfetex's |tie| chains, a patch for |Makefile.in| is included.
% 
%

@x
@ The integer parameters should really be initialized by a macro package;
@y
@ @<Glob...@>=
@!epochseconds: integer;
@!microseconds: integer;

@ 
@p function get_microinterval:integer;
var s,@!m:integer; {seconds and microseconds}
begin
   seconds_and_micros(s,m);
   if (s-epochseconds)>32767 then
     get_microinterval := max_integer
   else if (microseconds<m)  then
     get_microinterval := ((s-1-epochseconds)*65536)+ (((m+1000000-microseconds)/100)*65536)/10000
   else
     get_microinterval := ((s-epochseconds)*65536)  + (((m-microseconds)/100)*65536)/10000;
end;

@ The integer parameters should really be initialized by a macro package;
@z

@x
@d badness_code=input_line_no_code+1 {code for \.{\\badness}}
@y
@d elapsed_time_code=input_line_no_code+1 {code for \.{\\elapsedtime}}
@d badness_code=input_line_no_code+2 {code for \.{\\badness}}
@z

@x l.9016
primitive("badness",last_item,badness_code);
@!@:badness_}{\.{\\badness} primitive@>
@y
primitive("badness",last_item,badness_code);
@!@:badness_}{\.{\\badness} primitive@>
primitive("elapsedtime",last_item,elapsed_time_code);
@!@:elapsed_time_}{\.{\\elapsedtime} primitive@>
@z

@x l.30672
@d pdftex_last_extension_code  == pdftex_first_extension_code + 24
@y
@d reset_timer_code == pdftex_first_extension_code + 25
@d pdftex_last_extension_code  == pdftex_first_extension_code + 25
@z


@x l.30735
primitive("pdftrailer",extension,pdf_trailer_code);@/
@!@:pdf_trailer_}{\.{\\pdftrailer} primitive@>
@y
primitive("pdftrailer",extension,pdf_trailer_code);@/
@!@:pdf_trailer_}{\.{\\pdftrailer} primitive@>
primitive("resettimer",extension,reset_timer_code);@/
@!@:reset_timer_}{\.{\\resettimer} primitive@>
@z


@x l.30776
  pdf_trailer_code: print_esc("pdftrailer");
@y
  pdf_trailer_code: print_esc("pdftrailer");
  reset_timer_code: print_esc("resettimer");
@z

@x l.30822
pdf_trailer_code: @<Implement \.{\\pdftrailer}@>;
@y
pdf_trailer_code: @<Implement \.{\\pdftrailer}@>;
reset_timer_code: @<Implement \.{\\resettimer}@>;
@z


@x l.31776
@ @<Implement \.{\\pdftrailer}@>=
@y
@ @<Set initial values of key variables@>=
  seconds_and_micros(epochseconds,microseconds);

@ @<Implement \.{\\resettimer}@>=
begin
  seconds_and_micros(epochseconds,microseconds);
end

@ @<Implement \.{\\pdftrailer}@>=
@z

@x l.33134
last_node_type_code: print_esc("lastnodetype");
@y
last_node_type_code: print_esc("lastnodetype");
elapsed_time_code: print_esc("elapsedtime");
@z

@x l.33124
eTeX_version_code: cur_val:=eTeX_version;
@y
eTeX_version_code: cur_val:=eTeX_version;
elapsed_time_code: cur_val:=get_microinterval;
@z

