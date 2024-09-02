% tangle.ch for C compilation with web2c.
%
%  10/9/82 (HT) Original version
%  11/29   (HT) New version, with conversion to lowercase handled properly
%               Also, new control sequence:
%                       @=...text...@>   Put ...text... verbatim on a line
%                                        by itself in the Pascal output.
%                                        (argument must fit on one line)
%               This control sequence facilitates putting #include "gcons.h"
%               (for example) in files meant for the pc compiler.
%               Also, changed command line usage, so that the absence of a
%               change file implies no change file, rather than one with the
%               same name as the web file, with .ch at the end.
%  1/15/83 (HT) Changed to work with version 1.2, which incorporates the
%               above change (though unbundling the output line breaking),
%               so mainly had to remove stuff.
%  2/17    (HT) Fixed bug that caused 0-9 in identifiers to be converted to
%               Q-Y on output.
%  3/18    (HT) Brought up to work with Version 1.5.  Added -r command line
%               flag to cause a .rpl file to be written with all the lines
%               of the .web file that were replaced because of the .ch file
%               (useful for comparing with previous .rpl files, to see if a
%               change file will still work with a new version of a .web file)
%               Also, made it write a newline just before exit.
%  4/12    (PC) Merged with Pavel's version, including adding a call to exit()
%               at the end depending upon the value of history.
%  4/16    (PC) Brought up to date with version 1.5 released April, 1983.
%  6/28   (HWT) Brought up to date with version 1.7 released June, 1983.
%               With new change file format, the -r option is now unnecessary.
%  7/17   (HWT) Brought up to date with version 2.0 released July, 1983.
% 12/18/83 (ETM) Brought up to date with version 2.5 released November, 1983.
% 11/07/84 (ETM) Brought up to date with version 2.6.
% 12/15/85 (ETM) Brought up to date with version 2.8.
% 03/07/88 (ETM) Converted for use with WEB2C
% 01/02/89 (PAM) Cosmetic upgrade to version 2.9
% 11/30/89 (KB)  Version 4.
% (more recent changes in the ChangeLog)

@x [0.0] l.34 - Replace program title.
\def\title{TANGLE}
@y
\def\title{TANGLE changes for C}
@z

@x [0.0] l.43 - Print only changes.
\pageno=\contentspagenumber \advance\pageno by 1
@y
\pageno=\contentspagenumber \advance\pageno by 1
\let\maybe=\iffalse
@z

@x [1.1] l.70 - Define my_name
@d banner=='This is TANGLE, Version 4.6'
@y
@d my_name=='tangle'
@d banner=='This is TANGLE, Version 4.6'
@z

@x [1.2] l.79 - Eliminate the |end_of_TANGLE| label.
calls the `|jump_out|' procedure, which goes to the label |end_of_TANGLE|.

@d end_of_TANGLE = 9999 {go here to wrap it up}
@y
calls the `|jump_out|' procedure.
@z
@x [1.2] l.85
label end_of_TANGLE; {go here to finish}
@y
@z

@x [1.2] l.90 - Define and call parse_arguments.
procedure initialize;
  var @<Local variables for initialization@>@/
  begin @<Set initial values@>@/
@y
@<Define \(|parse_arguments|@>
procedure initialize;
  var @<Local variables for initialization@>@/
  begin
    kpse_set_program_name (argv[0], my_name);
    parse_arguments;
    @<Set initial values@>@/
@z

@x [1.8] l.183 - Constants: increase id lengths, for TeX--XeT and tex2pdf.
@!buf_size=100; {maximum length of input line}
@!max_bytes=45000; {|1/ww| times the number of bytes in identifiers,
  strings, and module names; must be less than 65536}
@!max_toks=65000; {|1/zz| times the number of bytes in compressed \PASCAL\ code;
  must be less than 65536}
@!max_names=4000; {number of identifiers, strings, module names;
  must be less than 10240}
@!max_texts=2000; {number of replacement texts, must be less than 10240}
@!hash_size=353; {should be prime}
@y
@!buf_size=1000; {maximum length of input line}
@!max_bytes=65535; {|1/ww| times the number of bytes in identifiers,
  strings, and module names; must be less than 65536}
@!max_toks=65535; {|1/zz| times the number of bytes in compressed \PASCAL\ code;
  must be less than 65536}
@!max_names=10239; {number of identifiers, strings, module names;
  must be less than 10240}
@!max_texts=10239; {number of replacement texts, must be less than 10240}
@!hash_size=8501; {should be prime}
@z

@x [1.8] l.195
@!stack_size=50; {number of simultaneous levels of macro expansion}
@!max_id_length=12; {long identifiers are chopped to this length, which must
  not exceed |line_length|}
@!unambig_length=7; {identifiers must be unique if chopped to this length}
  {note that 7 is more strict than \PASCAL's 8, but this can be varied}
@y
@!stack_size=100; {number of simultaneous levels of macro expansion}
@!max_id_length=50; {long identifiers are chopped to this length, which must
  not exceed |line_length|}
@!def_unambig_length=32; {identifiers must be unique if chopped to this length}
@z

% [12] The text_char type is used as an array index into xord.  The
% default type `char' may produce signed integers, which are bad array
% indices in C.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x [2.12] l.302
@d text_char == char {the data type of characters in text files}
@y
@d text_char == ASCII_code {the data type of characters in text files}
@z

@x [2.17] l.483 - enable maximum character set
for i:=1 to @'37 do xchr[i]:=' ';
for i:=@'200 to @'377 do xchr[i]:=' ';
@y
for i:=1 to @'37 do xchr[i]:=chr(i);
for i:=@'200 to @'377 do xchr[i]:=chr(i);
@z

@x [3.20] l.509 - terminal output: use standard i/o
@d print(#)==write(term_out,#) {`|print|' means write on the terminal}
@y
@d term_out==stdout
@d print(#)==write(term_out,#) {`|print|' means write on the terminal}
@z

@x [3.20] l.516
@<Globals...@>=
@!term_out:text_file; {the terminal as an output file}
@y
@z

@x [3.21] l.519 - init terminal
@ Different systems have different ways of specifying that the output on a
certain file will appear on the user's terminal. Here is one way to do this
on the \PASCAL\ system that was used in \.{TANGLE}'s initial development:
@^system dependencies@>

@<Set init...@>=
rewrite(term_out,'TTY:'); {send |term_out| output to the terminal}
@y
@ Different systems have different ways of specifying that the output on a
certain file will appear on the user's terminal.
@^system dependencies@>

@<Set init...@>=
 {Nothing need be done for C.}
@z

@x [3.22] l.532 - flush terminal buffer
@d update_terminal == break(term_out) {empty the terminal output buffer}
@y
@d update_terminal == fflush(term_out) {empty the terminal output buffer}
@z

@x [3.24] l.548 - open input files
begin reset(web_file); reset(change_file);
@y
begin web_file := kpse_open_file(web_name, kpse_web_format);
if chg_name then change_file := kpse_open_file(chg_name, kpse_web_format);
@z

@x [3.26] l.558 - Open output files (except for the pool file).
@ The following code opens |Pascal_file| and |pool|.
@y
@ The following code opens |Pascal_file|. Opening |pool| will be deferred
until section~\&{64}.
@z
@x [3.26] l.565
rewrite(Pascal_file); rewrite(pool);
@y
rewrite (Pascal_file, pascal_name);
@z

@x [3.28] l.591 - Fix f^.
    begin buffer[limit]:=xord[f^]; get(f);
    incr(limit);
    if buffer[limit-1]<>" " then final_limit:=limit;
    if limit=buf_size then
      begin while not eoln(f) do get(f);
@y
    begin buffer[limit]:=xord[getc(f)];
    incr(limit);
    if buffer[limit-1]<>" " then final_limit:=limit;
    if limit=buf_size then
      begin while not eoln(f) do vgetc(f);
@z

@x [4.31] l.640 - Fix typo as in weave.web.
procedure error; {prints '\..' and location of error message}
@y
procedure error; {prints `\..' and location of error message}
@z

@x [4.34] l.681 - Fix `jump_out'.
and jumps out of the program. This is the only non-local |goto| statement
in \.{TANGLE}. It is used when no recovery from a particular error has
been provided.

Some \PASCAL\ compilers do not implement non-local |goto| statements.
@^system dependencies@>
In such cases the code that appears at label |end_of_TANGLE| should be
copied into the |jump_out| procedure, followed by a call to a system procedure
that terminates the program.

@d fatal_error(#)==begin new_line; print(#); error; mark_fatal; jump_out;
  end

@<Error handling...@>=
procedure jump_out;
begin goto end_of_TANGLE;
end;
@y
and jumps out of the program.

@d jump_out==uexit(1)
@d fatal_error(#)==begin new_line; write(stderr, #);
     error; mark_fatal; jump_out;
  end
@z

@x [5.38] l.742 - Provide for a larger `byte_mem' and `tok_mem'. Extra capacity:
@d ww=2 {we multiply the byte capacity by approximately this amount}
@d zz=3 {we multiply the token capacity by approximately this amount}
@y
@d ww=3 {we multiply the byte capacity by approximately this amount}
@d zz=5 {we multiply the token capacity by approximately this amount}
@z

@x [5.38] l.753 - Provide for larger than 16bit numeric macros.
@!equiv: array [0..max_names] of sixteen_bits; {info corresponding to names}
@y
@!equiv: array [0..max_names] of integer; {info corresponding to names}
@z

@x [5.47] l.862 - Provide for larger than 16bit numeric macros.
their |equiv| value contains the corresponding numeric value plus $2^{15}$.
@y
their |equiv| value contains the corresponding numeric value plus $2^{30}$.
@z

@x [5.47] l.868 - Add parametric2 macros (macros that use [] to delimit arguments).
\yskip\hang |parametric| identifiers have been defined to be parametric macros;
like simple identifiers, their |equiv| value points to the replacement text.
@y
\yskip\hang |parametric| and |parametric2| identifiers have been defined to
be parametric macros; like simple identifiers, their |equiv| value points to
the replacement text.
@z
@x [5.47] l.874
@d parametric=3 {parametric macros have |parametric| ilk}
@y
@d parametric=3 {parametric macros have |parametric| ilk}
@d parametric2=4 {second type of parametric macros have this |ilk|}
@z

@x [6.50] l.945 - unambig_length is a variable now
@!chopped_id:array [0..unambig_length] of ASCII_code; {chopped identifier}
@y
@!chopped_id:array [0..max_id_length] of ASCII_code; {chopped identifier}
@z

@x [6.53] l.960 - Add parametric2 macros (macros that use [] to delimit arguments).
|numeric|, |simple|, or |parametric|.
@y
|numeric|, |simple|, |parametric|, or |parametric2|.
@z

@x [6.53] l.971 - unambig_length is a variable.
@!s:0..unambig_length; {index into |chopped_id|}
@y
@!s:0..max_id_length; {index into |chopped_id|}
@z

@x [6.58] l.1030 - Case smashing options/strict checking.
  begin if buffer[i]<>"_" then
    begin if buffer[i]>="a" then chopped_id[s]:=buffer[i]-@'40
@y
  begin if (buffer[i]<>"_") or (allow_underlines and not strict_mode) then
    begin if (strict_mode or force_uppercase) and (buffer[i]>="a") then
      chopped_id[s]:=buffer[i]-@'40
    else if (not strict_mode and force_lowercase)
           and (buffer[i]>="A") and (buffer[i]<="Z") then
      chopped_id[s]:=buffer[i]+@'40
@z

@x [6.61] l.1086
else @<Define and output a new string of the pool@>;
@y
else @<Define \(and output a new string of the pool@>;
@z

@x [6.63] l.1102 - Case smashing options/strict checking.
  if c<>"_" then
    begin if c>="a" then c:=c-@'40; {merge lowercase with uppercase}
@y
  if c<>"_" or (allow_underlines and not strict_mode) then
    begin if (strict_mode or force_uppercase) and (c>="a") then c:=c-@'40
    else if (not strict_mode and force_lowercase)
           and (c>="A") and (c<="Z") then
      c:=c+@'40;
@z

@x [6.64] l.1123 - Delayed pool file opening / larger numerics.
@<Define and output a new string...@>=
begin ilk[p]:=numeric; {strings are like numeric macros}
if l-double_chars=2 then {this string is for a single character}
  equiv[p]:=buffer[id_first+1]+@'100000
else  begin equiv[p]:=string_ptr+@'100000;
  l:=l-double_chars-1;
@y
@<Define \(and output a new string...@>=
begin ilk[p]:=numeric; {strings are like numeric macros}
if l-double_chars=2 then {this string is for a single character}
  equiv[p]:=buffer[id_first+1]+@'10000000000
else  begin
  {Avoid creating empty pool files.}
  if string_ptr = 256 then rewritebin (pool, pool_name);
  equiv[p]:=string_ptr+@'10000000000;
  l:=l-double_chars-1;
@z

@x [9.85] l.1502 - Add parametric2 macros (macros that use [] to delimit arguments).
  begin if ilk[cur_name]=parametric then
@y
  begin if (ilk[cur_name]=parametric) or (ilk[cur_name]=parametric2) then
@z

@x [9.89] l.1586 - Larger numerics.
numeric: begin cur_val:=equiv[a]-@'100000; a:=number;
@y
numeric: begin cur_val:=equiv[a]-@'10000000000; a:=number;
@z

@x [9.89] l.1590 - Add parametric2 macros (macros that use [] to delimit arguments).
parametric: begin @<Put a parameter on the parameter stack,
@y
parametric,parametric2: begin @<Put a parameter on the parameter stack,
@z

@x [9.90] l.1607 - Add parametric2 macros (macros that use [] to delimit arguments).
if (stack_ptr=0)or(tok_mem[zo,cur_byte]<>"(") then
@y
if (stack_ptr=0)or((ilk[a]=parametric)and(tok_mem[zo,cur_byte]<>"("))
    or((ilk[a]=parametric2)and(tok_mem[zo,cur_byte]<>"[")) then
@z

@x [9.93] l.1654 - Add parametric2 macros (macros that use [] to delimit arguments).
bal:=1; incr(cur_byte); {skip the opening `\.('}
@y
bal:=1; incr(cur_byte); {skip the opening `\.(' or `['}
@z
@x [9.93] l.1662
      "(": incr(bal);
      ")":  begin decr(bal);
@y
      "(": if ilk[a]=parametric then incr(bal);
      ")": if ilk[a]=parametric then begin decr(bal);
        if bal=0 then goto done;
        end;
      "[": if ilk[a]=parametric2 then incr(bal);
      "]": if ilk[a]=parametric2 then begin decr(bal);
@z

@x [10.105] l.1937 - Accept DIV, div, MOD, and mod
 (((out_contrib[1]="D")and(out_contrib[2]="I")and(out_contrib[3]="V")) or@|
 ((out_contrib[1]="M")and(out_contrib[2]="O")and(out_contrib[3]="D")) ))or@|
@^uppercase@>
@y
  (((out_contrib[1]="D")and(out_contrib[2]="I")and(out_contrib[3]="V")) or@|
  ((out_contrib[1]="d")and(out_contrib[2]="i")and(out_contrib[3]="v")) or@|
  ((out_contrib[1]="M")and(out_contrib[2]="O")and(out_contrib[3]="D")) or@|
  ((out_contrib[1]="m")and(out_contrib[2]="o")and(out_contrib[3]="d")) ))or@|
@z

@x [10.110] l.2005 - lowercase ids
@^uppercase@>
  if ((out_buf[out_ptr-3]="D")and(out_buf[out_ptr-2]="I")and
    (out_buf[out_ptr-1]="V"))or @/
     ((out_buf[out_ptr-3]="M")and(out_buf[out_ptr-2]="O")and
    (out_buf[out_ptr-1]="D")) then@/ goto bad_case
@y
  if ((out_buf[out_ptr-3]="D")and(out_buf[out_ptr-2]="I")and
    (out_buf[out_ptr-1]="V"))or @/
     ((out_buf[out_ptr-3]="d")and(out_buf[out_ptr-2]="i")and
    (out_buf[out_ptr-1]="v"))or @/
     ((out_buf[out_ptr-3]="M")and(out_buf[out_ptr-2]="O")and
    (out_buf[out_ptr-1]="D"))or @/
     ((out_buf[out_ptr-3]="m")and(out_buf[out_ptr-2]="o")and
    (out_buf[out_ptr-1]="d")) then@/ goto bad_case
@z

@x [11.114] l.2077 - lowercase operators (`and', `or', etc.)
and_sign: begin out_contrib[1]:="A"; out_contrib[2]:="N"; out_contrib[3]:="D";
@^uppercase@>
  send_out(ident,3);
  end;
not_sign: begin out_contrib[1]:="N"; out_contrib[2]:="O"; out_contrib[3]:="T";
  send_out(ident,3);
  end;
set_element_sign: begin out_contrib[1]:="I"; out_contrib[2]:="N";
  send_out(ident,2);
  end;
or_sign: begin out_contrib[1]:="O"; out_contrib[2]:="R"; send_out(ident,2);
@y
and_sign: begin out_contrib[1]:="a"; out_contrib[2]:="n"; out_contrib[3]:="d";
  send_out(ident,3);
  end;
not_sign: begin out_contrib[1]:="n"; out_contrib[2]:="o"; out_contrib[3]:="t";
  send_out(ident,3);
  end;
set_element_sign: begin out_contrib[1]:="i"; out_contrib[2]:="n";
  send_out(ident,2);
  end;
or_sign: begin out_contrib[1]:="o"; out_contrib[2]:="r"; send_out(ident,2);
@z

@x [11.116] l.2120 - Case smashing options.
@ Single-character identifiers represent themselves, while longer ones
appear in |byte_mem|. All must be converted to uppercase,
with underlines removed. Extremely long identifiers must be chopped.

(Some \PASCAL\ compilers work with lowercase letters instead of
uppercase. If this module of \.{TANGLE} is changed, it's also necessary
to change from uppercase to lowercase in the modules that are
listed in the index under ``uppercase''.)
@^system dependencies@>
@^uppercase@>

@d up_to(#)==#-24,#-23,#-22,#-21,#-20,#-19,#-18,#-17,#-16,#-15,#-14,
  #-13,#-12,#-11,#-10,#-9,#-8,#-7,#-6,#-5,#-4,#-3,#-2,#-1,#

@<Cases related to identifiers@>=
"A",up_to("Z"): begin out_contrib[1]:=cur_char; send_out(ident,1);
  end;
"a",up_to("z"): begin out_contrib[1]:=cur_char-@'40; send_out(ident,1);
  end;
identifier: begin k:=0; j:=byte_start[cur_val]; w:=cur_val mod ww;
  while (k<max_id_length)and(j<byte_start[cur_val+ww]) do
    begin incr(k); out_contrib[k]:=byte_mem[w,j]; incr(j);
    if out_contrib[k]>="a" then out_contrib[k]:=out_contrib[k]-@'40
    else if out_contrib[k]="_" then decr(k);
    end;
  send_out(ident,k);
  end;
@y
@ Single-character identifiers represent themselves, while longer ones
appear in |byte_mem|. All must be converted to lowercase,
with underlines removed. Extremely long identifiers must be chopped.
@^system dependencies@>

@d up_to(#)==#-24,#-23,#-22,#-21,#-20,#-19,#-18,#-17,#-16,#-15,#-14,
  #-13,#-12,#-11,#-10,#-9,#-8,#-7,#-6,#-5,#-4,#-3,#-2,#-1,#

@<Cases related to identifiers@>=
"A",up_to("Z"): begin if force_lowercase then out_contrib[1]:=cur_char+@'40
    else out_contrib[1]:=cur_char;
    send_out(ident,1);
  end;
"a",up_to("z"): begin if force_uppercase then out_contrib[1]:=cur_char-@'40
    else out_contrib[1]:=cur_char;
    send_out(ident,1);
  end;
identifier: begin k:=0; j:=byte_start[cur_val]; w:=cur_val mod ww;
  while (k<max_id_length)and(j<byte_start[cur_val+ww]) do
    begin incr(k); out_contrib[k]:=byte_mem[w,j]; incr(j);
    if force_uppercase and (out_contrib[k]>="a") then
      out_contrib[k]:=out_contrib[k]-@'40
    else if force_lowercase and (out_contrib[k]<="Z") then
      out_contrib[k]:=out_contrib[k]+@'40
    else if not allow_underlines and (out_contrib[k]="_") then decr(k);
    end;
  send_out(ident,k);
  end;
@z

@x [11.119] l.2199 - Stretch limits of constants to match what we set for expressions.
  if n>=@'2000000000 then err_print('! Constant too big')
@y
  if n>=@'10000000000 then err_print('! Constant too big')
@z
@x [11.119] l.2208
  if n>=@"8000000 then err_print('! Constant too big')
@y
  if n>=@"40000000 then err_print('! Constant too big')
@z

@x [14.157] l.2862 - Larger numerics.
if abs(accumulator)>=@'100000 then
@y
if abs(accumulator)>=@'10000000000 then
@z

@x [14.157] l.2866 - Larger numerics.
equiv[p]:=accumulator+@'100000; {name |p| now is defined to equal |accumulator|}
@y
equiv[p]:=accumulator+@'10000000000; {name |p| now is defined to equal |accumulator|}
@z

@x [14.158] l.2889 - Larger numerics.
    add_in(equiv[q]-@'100000);
@y
    add_in(equiv[q]-@'10000000000);
@z

@x [15.165] l.2964 - Add parametric2 macros (macros that use [] to delimit arguments).
  "(": incr(bal);
  ")": if bal=0 then err_print('! Extra )')
@.Extra )@>
    else decr(bal);
  "'": @<Copy a string from the buffer to |tok_mem|@>;
  "#": if t=parametric then a:=param;
@y
  "(": if t=parametric then incr(bal);
  ")": if t=parametric then if bal=0 then err_print('! Extra )')
@.Extra )@>
    else decr(bal);
  "[": if t=parametric2 then incr(bal);
  "]": if t=parametric2 then if bal=0 then err_print('! Extra ]')
@.Extra ]@>
    else decr(bal);
  "'": @<Copy a string from the buffer to |tok_mem|@>;
  "#": if (t=parametric)or(t=parametric2) then a:=param;
@z

@x [15.166] l.2988 - Add parametric2 macros (macros that use [] to delimit arguments).
  begin if bal=1 then err_print('! Missing )')
  else err_print('! Missing ',bal:1,' )''s');
@.Missing n )@>
  while bal>0 do
    begin app_repl(")"); decr(bal);
    end;
  end
@y
  if t=parametric then begin
    if bal=1 then err_print('! Missing )')
    else err_print('! Missing ',bal:1,' )''s');
@.Missing n )@>
    while bal>0 do
      begin app_repl(")"); decr(bal);
      end;
    end
  else begin
    if bal=1 then err_print('! Missing ]')
    else err_print('! Missing ',bal:1,' ]''s');
@.Missing n ]@>
    while bal>0 do
      begin app_repl("]"); decr(bal);
      end;
    end
@z

@x [16.173] l.3107 - Add parametric2 macros (macros that use [] to delimit arguments).
  else @<If the next text is `\.{(\#)==}', call |define_macro|
    and |goto continue|@>;
@y
  else @<If the next text is `\.{(\#)==}' or `\.{[\#]==}', call |define_macro|
    and |goto continue|@>;
@z

@x [16.174] l.3114 - Add parametric2 macros (macros that use [] to delimit arguments).
@ @<If the next text is `\.{(\#)==}'...@>=
if next_control="(" then
  begin next_control:=get_next;
  if next_control="#" then
    begin next_control:=get_next;
    if next_control=")" then
      begin next_control:=get_next;
      if next_control="=" then
        begin err_print('! Use == for macros');
@.Use == for macros@>
        next_control:=equivalence_sign;
        end;
      if next_control=equivalence_sign then
        begin define_macro(parametric); goto continue;
        end;
      end;
    end;
  end;
@y
@ @<If the next text is `\.{(\#)==}'...@>=
if next_control="(" then
  begin next_control:=get_next;
  if next_control="#" then
    begin next_control:=get_next;
    if next_control=")" then
      begin next_control:=get_next;
      if next_control="=" then
        begin err_print('! Use == for macros');
@.Use == for macros@>
        next_control:=equivalence_sign;
        end;
      if next_control=equivalence_sign then
        begin define_macro(parametric); goto continue;
        end;
      end;
    end;
  end
else if next_control="[" then
  begin next_control:=get_next;
  if next_control="#" then
    begin next_control:=get_next;
    if next_control="]" then
      begin next_control:=get_next;
      if next_control="=" then
        begin err_print('! Use == for macros');
@.Use == for macros@>
        next_control:=equivalence_sign;
        end;
      if next_control=equivalence_sign then
        begin define_macro(parametric2); goto continue;
        end;
      end;
    end;
  end
@z

@x [17.179] l.3190 - make term_in = input
any error stop will set |debug_cycle| to zero.
@y
any error stop will set |debug_cycle| to zero.

@d term_in==stdin
@z

@x [17.179] l.3198
@!term_in:text_file; {the user's terminal as an input file}
@y
@z

@x [17.180] l.3206 - remove term_in reset
reset(term_in,'TTY:','/I'); {open |term_in| as the terminal, don't do a |get|}
@y
@z

@x [18.182] l.3248 - write newline just before exit; use value of |history|
print_ln(banner); {print a ``banner line''}
@y
print (banner); {print a ``banner line''}
print_ln (version_string);
@z

@x [18.182] l.3252 - Eliminate the |end_of_TANGLE| label.
end_of_TANGLE:
@y
@z

@x [18.182] l.3256
@<Print the job |history|@>;
@y
@<Print the job |history|@>;
new_line;
if (history <> spotless) and (history <> harmless_message)
then uexit (1)
else uexit (0);
@z

@x [19.188] l.3308 - System-dependent changes.
This module should be replaced, if necessary, by changes to the program
that are necessary to make \.{TANGLE} work at a particular installation.
It is usually best to design your change file so that all changes to
previous modules preserve the module numbering; then everybody's version
will be consistent with the printed program. More extensive changes,
which introduce new modules, can be inserted here; then only the index
itself will get a new module number.
@^system dependencies@>
@y
Parse a Unix-style command line.

@d argument_is (#) == (strcmp (long_options[option_index].name, #) = 0)

@<Define \(|parse_arguments|@> =
procedure parse_arguments;
const n_options = 9; {Pascal won't count array lengths for us.}
var @!long_options: array[0..n_options] of getopt_struct;
    @!getopt_return_val: integer;
    @!option_index: c_int_type;
    @!current_option: 0..n_options;
    @!len: integer;
begin
  @<Define the option table@>;
  unambig_length := def_unambig_length;
  repeat
    getopt_return_val := getopt_long_only (argc, argv, '', long_options,
                                           address_of (option_index));
    if getopt_return_val = -1 then begin
      do_nothing; {End of arguments; we exit the loop below.}

    end else if getopt_return_val = "?" then begin
      usage (my_name);

    end else if argument_is ('help') then begin
      usage_help (TANGLE_HELP, nil);

    end else if argument_is ('version') then begin
      print_version_and_exit (banner, nil, 'D.E. Knuth', nil);

    end else if argument_is ('mixedcase') then begin
      force_uppercase := false;
      force_lowercase := false;

    end else if argument_is ('uppercase') then begin
      force_uppercase := true;
      force_lowercase := false;

    end else if argument_is ('lowercase') then begin
      force_uppercase := false;
      force_lowercase := true;

    end else if argument_is ('underlines') then begin
      allow_underlines := true;

    end else if argument_is ('strict') then begin
      strict_mode := true;

    end else if argument_is ('loose') then begin
      strict_mode := false;

    end else if argument_is ('length') then begin
      len := atoi(optarg);
      if (len<=0) or (len>max_id_length) then len := max_id_length;
      unambig_length := len;

    end; {Else it was a flag; |getopt| has already done the assignment.}
  until getopt_return_val = -1;

  @<Handle file name arguments@>@;
end;

@ Now |optind| is the index of first non-option on the command line.

@<Handle file name...@>=
if (optind + 1 > argc) or (optind + 3 < argc) then begin
  write_ln (stderr, my_name, ': Need one to three file arguments.');
  usage (my_name);
end;

{Supply |".web"| and |".ch"| extensions if necessary.}
web_name := extend_filename (cmdline (optind), 'web');
if optind + 2 <= argc then begin
  {|'-'| is shortcut for an empty changefile.}
  if strcmp(char_to_string('-'), cmdline (optind + 1)) <> 0 then
    chg_name := extend_filename (cmdline (optind + 1), 'ch');
end;

if optind + 3 = argc then begin
  {User has provided an explicit Pascal output file, possibly with path.}
  pascal_name := extend_filename (cmdline (optind + 2), char_to_string('p'));
  pool_name := extend_filename (remove_suffix (pascal_name), 'pool');
end
else begin
  {Change |".web"| to |".p"| and |".pool"| and use the current directory.}
  pascal_name := basename_change_suffix (web_name, '.web', '.p');
  pool_name := basename_change_suffix (web_name, '.web', '.pool');
end;

@ Here are the options we allow.  The first is one of the standard GNU options.
@.-help@>

@<Define the option...@> =
current_option := 0;
long_options[current_option].name := 'help';
long_options[current_option].has_arg := 0;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;
incr (current_option);

@ Another of the standard options.
@.-version@>

@<Define the option...@> =
long_options[current_option].name := 'version';
long_options[current_option].has_arg := 0;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;
incr (current_option);

@ Use all mixed case.
@.-mixedcase@>

@<Define the option...@> =
long_options[current_option].name := 'mixedcase';
long_options[current_option].has_arg := 0;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;
incr (current_option);

@ Use all uppercase.
@.-uppercase@>

@<Define the option...@> =
long_options[current_option].name := 'uppercase';
long_options[current_option].has_arg := 0;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;
incr (current_option);

@ Use all lowercase.
@.-lowercase@>

@<Define the option...@> =
long_options[current_option].name := 'lowercase';
long_options[current_option].has_arg := 0;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;
incr (current_option);

@ Allow underlines.
@.-underlines@>

@<Define the option...@> =
long_options[current_option].name := 'underlines';
long_options[current_option].has_arg := 0;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;
incr (current_option);

@ Strict comparisons.
@.-strict@>

@<Define the option...@> =
long_options[current_option].name := 'strict';
long_options[current_option].has_arg := 0;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;
incr (current_option);

@ Loose comparisons.
@.-loose@>

@<Define the option...@> =
long_options[current_option].name := 'loose';
long_options[current_option].has_arg := 0;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;
incr (current_option);

@ Loose comparisons.
@.-length@>

@<Define the option...@> =
long_options[current_option].name := 'length';
long_options[current_option].has_arg := 1;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;
incr (current_option);

@ An element with all zeros always ends the list.

@<Define the option...@> =
long_options[current_option].name := 0;
long_options[current_option].has_arg := 0;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;

@ Global filenames.

@<Globals...@>=
@!web_name,@!chg_name,@!pascal_name,@!pool_name:const_c_string;
@!force_uppercase,@!force_lowercase,@!allow_underlines,@!strict_mode:boolean;
@!unambig_length:0..max_id_length;
@z
