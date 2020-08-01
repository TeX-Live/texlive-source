% mf.ch for C compilation with web2c, derived from various other change
% files: INITEX.CH for Berkeley Unix TeX 1.1 (by Howard Trickey and
% Pavel Curtis), by Paul Richards.  web2c modifications by Tim Morgan, et al.
%
% (more recent changes in ChangeLog)
% Revision 2.0  90/3/27   20:20:00  ken        To version 2.0.
% Revision 1.9  90/1/20   09:05:32  karl       To version 1.9.
% Revision 1.8  89/11/30  09:08:16  karl       To version 1.8.
% Revision 1.7  88/12/27  15:02:24  mackay     Cosmetic upgrade for version 1.7
% Revision 1.6  88/12/11  15:59:15  morgan     Brought up to MF version 1.6
% Revision 1.5  88/03/02  13:25:44  morgan     More C changes
% Revision 1.4  87/12/09  12:50:00  hesse      Changes for C version
% Revision 1.3  87/03/07  21:15:21  mackay
% 	Minor changes found on archive version on SCORE
% Revision 1.2  86/09/29  21:46:43  mackay
%	Made no-debug the default, and changed version number
%	to correspond with improved mf.web file
%	(Got rid of debug code to avoid bug in range check
%	code of VAX4.3 BSD and SUN3 version 3.1 Os pc interpreter)
% Revision 1.0  86/01/31  15:46:08  richards
% 	Incorporates: New binary I/O library, separate optimized
% 	arithmetic for takefraction/makefraction, new graphics interface.

@x [0] WEAVE: print changes only.
\def\botofcontents{\vskip 0pt plus 1fil minus 1.5in}
@y
\def\botofcontents{\vskip 0pt plus 1fil minus 1.5in}
\let\maybe=\iffalse
\def\title{\MF\ changes for C}
\def\glob{13}\def\gglob{20, 25} % these are defined in module 1
@z

@x [1.6] Purge non-local 'goto' label.
@d end_of_MF=9998 {go here to close files and terminate gracefully}
@y
@z
@x
start_of_MF@t\hskip-2pt@>, end_of_MF@t\hskip-2pt@>,@,final_end;
@y
start_of_MF@t\hskip-2pt@>,@,final_end;
@z

@x [1.7] Convert `debug..gubed' and `stat..tats' into #ifdefs.
@d debug==@{ {change this to `$\\{debug}\equiv\null$' when debugging}
@d gubed==@t@>@} {change this to `$\\{gubed}\equiv\null$' when debugging}
@y
@d debug==ifdef('TEXMF_DEBUG')
@d gubed==endif('TEXMF_DEBUG')
@z

@x [1.7]
@d stat==@{ {change this to `$\\{stat}\equiv\null$' when gathering
  usage statistics}
@d tats==@t@>@} {change this to `$\\{tats}\equiv\null$' when gathering
  usage statistics}
@y
@d stat==ifdef('STAT')
@d tats==endif('STAT')
@z

@x [1.8] Same, for `init..tini'.
@d init== {change this to `$\\{init}\equiv\.{@@\{}$' in the production version}
@d tini== {change this to `$\\{tini}\equiv\.{@@\}}$' in the production version}
@y
@d init==ifdef('INIMF')
@d tini==endif('INIMF')
@z

% [1.11] Compile-time constants.  Although we only change a few of
% these, listing them all makes the patch file for a big Metafont simpler.
% 16K for BSD I/O; file_name_size is set from the system constant.
@x
@<Constants...@>=
@!mem_max=30000; {greatest index in \MF's internal |mem| array;
  must be strictly less than |max_halfword|;
  must be equal to |mem_top| in \.{INIMF}, otherwise |>=mem_top|}
@!max_internal=100; {maximum number of internal quantities}
@!buf_size=500; {maximum number of characters simultaneously present in
  current lines of open files; must not exceed |max_halfword|}
@!error_line=72; {width of context lines on terminal error messages}
@!half_error_line=42; {width of first lines of contexts in terminal
  error messages; should be between 30 and |error_line-15|}
@!max_print_line=79; {width of longest text lines output; should be at least 60}
@!screen_width=768; {number of pixels in each row of screen display}
@!screen_depth=1024; {number of pixels in each column of screen display}
@!stack_size=30; {maximum number of simultaneous input sources}
@!max_strings=2000; {maximum number of strings; must not exceed |max_halfword|}
@!string_vacancies=8000; {the minimum number of characters that should be
  available for the user's identifier names and strings,
  after \MF's own error messages are stored}
@!pool_size=32000; {maximum number of characters in strings, including all
  error messages and help texts, and the names of all identifiers;
  must exceed |string_vacancies| by the total
  length of \MF's own strings, which is currently about 22000}
@!move_size=5000; {space for storing moves in a single octant}
@!max_wiggle=300; {number of autorounded points per cycle}
@!gf_buf_size=800; {size of the output buffer, must be a multiple of 8}
@!file_name_size=40; {file names shouldn't be longer than this}
@!pool_name='MFbases:MF.POOL                         ';
  {string of length |file_name_size|; tells where the string pool appears}
@.MFbases@>
@!path_size=300; {maximum number of knots between breakpoints of a path}
@!bistack_size=785; {size of stack for bisection algorithms;
  should probably be left at this value}
@!header_size=100; {maximum number of \.{TFM} header words, times~4}
@!lig_table_size=5000; {maximum number of ligature/kern steps, must be
  at least 255 and at most 32510}
@!max_kerns=500; {maximum number of distinct kern amounts}
@!max_font_dimen=50; {maximum number of \&{fontdimen} parameters}
@y
@d file_name_size == maxint
@d ssup_error_line = 255
@d ssup_screen_width = 32767
@d ssup_screen_depth = 32767

@<Constants...@>=
@!max_internal=300; {maximum number of internal quantities}
@!stack_size=300; {maximum number of simultaneous input sources}
@!max_strings=7500; {maximum number of strings; must not exceed |max_halfword|}
@!string_vacancies=74000; {the minimum number of characters that should be
  available for the user's identifier names and strings,
  after \MF's own error messages are stored}
@!pool_size=100000; {maximum number of characters in strings, including all
  error messages and help texts, and the names of all identifiers;
  must exceed |string_vacancies| by the total
  length of \MF's own strings, which is currently about 22000}
@!move_size=20000; {space for storing moves in a single octant}
@!max_wiggle=1000; {number of autorounded points per cycle}
@!pool_name=TEXMF_POOL_NAME;
  {string that tells where the string pool appears}
@!engine_name=TEXMF_ENGINE_NAME; {the name of this engine}
@!path_size=1000; {maximum number of knots between breakpoints of a path}
@!bistack_size=1500; {size of stack for bisection algorithms;
  should probably be left at this value}
@!header_size=100; {maximum number of \.{TFM} header words, times~4}
@!lig_table_size=15000; {maximum number of ligature/kern steps, must be
  at least 255 and at most 32510}
@!max_kerns=2500; {maximum number of distinct kern amounts}
@!max_font_dimen=60; {maximum number of \&{fontdimen} parameters}
@#
@!inf_main_memory = 3000;
@!sup_main_memory = 8000000;

@!inf_buf_size = 500;
@!sup_buf_size = 30000000;
@z

@x [1.12] Constants defined as WEB macros.
@d mem_min=0 {smallest index in the |mem| array, must not be less
  than |min_halfword|}
@d mem_top==30000 {largest index in the |mem| array dumped by \.{INIMF};
  must be substantially larger than |mem_min|
  and not greater than |mem_max|}
@d hash_size=2100 {maximum number of symbolic tokens,
  must be less than |max_halfword-3*param_size|}
@d hash_prime=1777 {a prime number equal to about 85\pct! of |hash_size|}
@d max_in_open=6 {maximum number of input files and error insertions that
  can be going on simultaneously}
@d param_size=150 {maximum number of simultaneous macro parameters}
@y
@d mem_min=0 {smallest index in the |mem| array, must not be less
  than |min_halfword|}
@d hash_size=9500 {maximum number of symbolic tokens,
  must be less than |max_halfword-3*param_size|}
@d hash_prime=7919 {a prime number equal to about 85\pct! of |hash_size|}
@d max_in_open=15 {maximum number of input files and error insertions that
  can be going on simultaneously}
@d param_size=150 {maximum number of simultaneous macro parameters}
@z

@x [1.13] Global parameters that can be changed in texmf.cnf.
@<Glob...@>=
@!bad:integer; {is some ``constant'' wrong?}
@y
@<Glob...@>=
@!bad:integer; {is some ``constant'' wrong?}
@#
@!init
@!ini_version:boolean; {are we \.{INIMF}? Set in \.{lib/texmfmp.c}}
@!dump_option:boolean; {was the dump name option used?}
@!dump_line:boolean; {was a \.{\%\AM base} line seen?}
tini@/
@#
@!dump_name:const_cstring; {base name for terminal display}
@#
@!bound_default:integer; {temporary for setup}
@!bound_name:const_cstring; {temporary for setup}
@#
@!main_memory:integer; {total memory words allocated in initex}
@!mem_top:integer; {largest index in the |mem| array dumped by \.{INIMF};
  must be substantially larger than |mem_bot|,
  equal to |mem_max| in \.{INIMF}, else not greater than |mem_max|}
@!mem_max:integer; {greatest index in \MF's internal |mem| array;
  must be strictly less than |max_halfword|;
  must be equal to |mem_top| in \.{INIMF}, otherwise |>=mem_top|}
@!buf_size:integer; {maximum number of characters simultaneously present in
  current lines of open files; must not exceed |max_halfword|}
@!error_line:integer; {width of context lines on terminal error messages}
@!half_error_line:integer; {width of first lines of contexts in terminal
  error messages; should be between 30 and |error_line-15|}
@!max_print_line:integer; {width of longest text lines output;
  should be at least 60}
@!screen_width:integer; {number of pixels in each row of screen display}
@!screen_depth:integer; {number of pixels in each column of screen display}
@!gf_buf_size:integer; {size of the output buffer, must be a multiple of 8}
@!parse_first_line_p:c_int_type; {parse the first line for options}
@!file_line_error_style_p:c_int_type; {output file:line:error style errors.}
@!eight_bit_p:c_int_type; {make all characters printable by default}
@!halt_on_error_p:c_int_type; {stop at first error}
@!quoted_filename:boolean; {current filename is quoted}
@z

@x [1.16] Use C macros for `incr' and `decr'.
@d incr(#) == #:=#+1 {increase a variable by unity}
@d decr(#) == #:=#-1 {decrease a variable by unity}
@y
@z

% [2.19] The text_char type is used as an array index into xord.  The
% default type `char' produces signed integers, which are bad array
% indices in C.
@x
@d text_char == char {the data type of characters in text files}
@y
@d text_char == ASCII_code {the data type of characters in text files}
@z

@x [2.22] Allow any character as input.
@^character set dependencies@>
@^system dependencies@>

@<Set init...@>=
for i:=0 to @'37 do xchr[i]:=' ';
for i:=@'177 to @'377 do xchr[i]:=' ';
@y
@^character set dependencies@>
@^system dependencies@>

@d tab = @'11 { ASCII horizontal tab }
@d form_feed = @'14 { ASCII form feed }

@<Set init...@>=
{Initialize |xchr| to the identity mapping.}
for i:=0 to @'37 do xchr[i]:=i;
for i:=@'177 to @'377 do xchr[i]:=i;
@z

@x [2.23]
for i:=0 to @'176 do xord[xchr[i]]:=i;
@y
for i:=0 to @'176 do xord[xchr[i]]:=i;
{Set |xprn| for printable ASCII, unless |eight_bit_p| is set.}
for i:=0 to 255 do xprn[i]:=(eight_bit_p or ((i>=" ")and(i<="~")));

{The idea for this dynamic translation comes from the patch by
 Libor Skarvada \.{<libor@@informatics.muni.cz>}
 and Petr Sojka \.{<sojka@@informatics.muni.cz>}. I didn't use any of the
 actual code, though, preferring a more general approach.}

{This updates the |xchr|, |xord|, and |xprn| arrays from the provided
 |translate_filename|.  See the function definition in \.{texmfmp.c} for
 more comments.}
if translate_filename then read_tcx_file;
@z

% [3.25] Declare name_of_file as a C string.  See comments in tex.ch for
% why we change the element type to text_char.
@x
@!name_of_file:packed array[1..file_name_size] of char;@;@/
  {on some systems this may be a \&{record} variable}
@y
@!name_of_file:^text_char;
@z

@x [3.26] Do file opening in C.
@ The \ph\ compiler with which the present version of \MF\ was prepared has
extended the rules of \PASCAL\ in a very convenient way. To open file~|f|,
we can write
$$\vbox{\halign{#\hfil\qquad&#\hfil\cr
|reset(f,@t\\{name}@>,'/O')|&for input;\cr
|rewrite(f,@t\\{name}@>,'/O')|&for output.\cr}}$$
The `\\{name}' parameter, which is of type `\ignorespaces|packed
array[@t\<\\{any}>@>] of text_char|', stands for the name of
the external file that is being opened for input or output.
Blank spaces that might appear in \\{name} are ignored.

The `\.{/O}' parameter tells the operating system not to issue its own
error messages if something goes wrong. If a file of the specified name
cannot be found, or if such a file cannot be opened for some other reason
(e.g., someone may already be trying to write the same file), we will have
|@!erstat(f)<>0| after an unsuccessful |reset| or |rewrite|.  This allows
\MF\ to undertake appropriate corrective action.
@:PASCAL H}{\ph@>
@^system dependencies@>

\MF's file-opening procedures return |false| if no file identified by
|name_of_file| could be opened.

@d reset_OK(#)==erstat(#)=0
@d rewrite_OK(#)==erstat(#)=0

@p function a_open_in(var @!f:alpha_file):boolean;
  {open a text file for input}
begin reset(f,name_of_file,'/O'); a_open_in:=reset_OK(f);
end;
@#
function a_open_out(var @!f:alpha_file):boolean;
  {open a text file for output}
begin rewrite(f,name_of_file,'/O'); a_open_out:=rewrite_OK(f);
end;
@#
function b_open_out(var @!f:byte_file):boolean;
  {open a binary file for output}
begin rewrite(f,name_of_file,'/O'); b_open_out:=rewrite_OK(f);
end;
@#
function w_open_in(var @!f:word_file):boolean;
  {open a word file for input}
begin reset(f,name_of_file,'/O'); w_open_in:=reset_OK(f);
end;
@#
function w_open_out(var @!f:word_file):boolean;
  {open a word file for output}
begin rewrite(f,name_of_file,'/O'); w_open_out:=rewrite_OK(f);
end;
@y
@ All of the file opening functions are defined in C.
@z

@x [3.27] Do file closing in C.
@ Files can be closed with the \ph\ routine `|close(f)|', which
@:PASCAL H}{\ph@>
@^system dependencies@>
should be used when all input or output with respect to |f| has been completed.
This makes |f| available to be opened again, if desired; and if |f| was used for
output, the |close| operation makes the corresponding external file appear
on the user's area, ready to be read.

@p procedure a_close(var @!f:alpha_file); {close a text file}
begin close(f);
end;
@#
procedure b_close(var @!f:byte_file); {close a binary file}
begin close(f);
end;
@#
procedure w_close(var @!f:word_file); {close a word file}
begin close(f);
end;
@y
@ And all the file closing routines as well.
@z

@x [3.29] Array size of input buffer is determined at runtime.
@!buffer:array[0..buf_size] of ASCII_code; {lines of characters being read}
@y
@!buffer:^ASCII_code; {lines of characters being read}
@z

@x [3.30] Do `input_ln' in C.
Standard \PASCAL\ says that a file should have |eoln| immediately
before |eof|, but \MF\ needs only a weaker restriction: If |eof|
occurs in the middle of a line, the system function |eoln| should return
a |true| result (even though |f^| will be undefined).

@p function input_ln(var @!f:alpha_file;@!bypass_eoln:boolean):boolean;
  {inputs the next line or returns |false|}
var @!last_nonblank:0..buf_size; {|last| with trailing blanks removed}
begin if bypass_eoln then if not eof(f) then get(f);
  {input the first character of the line into |f^|}
last:=first; {cf.\ Matthew 19\thinspace:\thinspace30}
if eof(f) then input_ln:=false
else  begin last_nonblank:=first;
  while not eoln(f) do
    begin if last>=max_buf_stack then
      begin max_buf_stack:=last+1;
      if max_buf_stack=buf_size then
        @<Report overflow of the input buffer, and abort@>;
      end;
    buffer[last]:=xord[f^]; get(f); incr(last);
    if buffer[last-1]<>" " then last_nonblank:=last;
    end;
  last:=last_nonblank; input_ln:=true;
  end;
end;
@y
We define |input_ln| in C, for efficiency.  Nevertheless we quote the module
`Report overflow of the input buffer, and abort' here in order to make
\.{WEAVE} happy.

@p @{ @<Report overflow of the input buffer, and abort@> @}
@z

@x [3.31] `term_in' and `term_out' are standard input and output.
@<Glob...@>=
@!term_in:alpha_file; {the terminal as an input file}
@!term_out:alpha_file; {the terminal as an output file}
@y
@d term_in==stdin {the terminal as an input file}
@d term_out==stdout {the terminal as an output file}
@z

@x [3.32] We don't need to open the terminal files.
@ Here is how to open the terminal files
in \ph. The `\.{/I}' switch suppresses the first |get|.
@:PASCAL H}{\ph@>
@^system dependencies@>

@d t_open_in==reset(term_in,'TTY:','/O/I') {open the terminal for text input}
@d t_open_out==rewrite(term_out,'TTY:','/O')
 {open the terminal for text output}
@y
@ Here is how to open the terminal files.  |t_open_out| does nothing.
|t_open_in|, on the other hand, does the work of ``rescanning,'' or getting
any command line arguments the user has provided.  It's defined in C.

@d t_open_out == {output already open for text output}
@z

@x [3.33] Flushing output.
these operations can be specified in \ph:
@:PASCAL H}{\ph@>
@^system dependencies@>

@d update_terminal == break(term_out) {empty the terminal output buffer}
@d clear_terminal == break_in(term_in,true) {clear the terminal input buffer}
@y
these operations can be specified with {\mc UNIX}.  |update_terminal|
does an |fflush|. |clear_terminal| is redefined
to do nothing, since the user should control the terminal.
@^system dependencies@>

@d update_terminal == fflush(term_out)
@d clear_terminal == do_nothing
@z

@x [3.36] Reading the command line.
@ The following program does the required initialization
without retrieving a possible command line.
It should be clear how to modify this routine to deal with command lines,
if the system permits them.
@^system dependencies@>

@p function init_terminal:boolean; {gets the terminal input started}
label exit;
begin t_open_in;
loop@+begin wake_up_terminal; write(term_out,'**'); update_terminal;
@.**@>
  if not input_ln(term_in,true) then {this shouldn't happen}
    begin write_ln(term_out);
    write(term_out,'! End of file on the terminal... why?');
@.End of file on the terminal@>
    init_terminal:=false; return;
    end;
  loc:=first;
  while (loc<last)and(buffer[loc]=" ") do incr(loc);
  if loc<last then
    begin init_terminal:=true;
    return; {return unless the line was all blank}
    end;
  write_ln(term_out,'Please type the name of your input file.');
  end;
exit:end;
@y
@ The following program does the required initialization.
Iff anything has been specified on the command line, then |t_open_in|
will return with |last > first|.
@^system dependencies@>

@p
function init_terminal:boolean; {gets the terminal input started}
label exit;
begin
    t_open_in;
    if last > first then begin
        loc := first;
        while (loc < last) and (buffer[loc]=' ') do
	    incr(loc);
        if loc < last then begin
            init_terminal := true;
            goto exit;
        end;
    end;
    loop@+begin
        wake_up_terminal; write(term_out, '**'); update_terminal;
@.**@>
        if not input_ln(term_in,true) then begin {this shouldn't happen}
            write_ln(term_out);
            write_ln(term_out, '! End of file on the terminal... why?');
@.End of file on the terminal@>
            init_terminal:=false;
	    return;
        end;

        loc:=first;
        while (loc<last)and(buffer[loc]=" ") do
            incr(loc);

        if loc<last then begin
           init_terminal:=true;
           return; {return unless the line was all blank}
        end;
        write_ln(term_out, 'Please type the name of your input file.');
    end;
exit:
end;
@z

@x [4.49] l.1239 -- change documentation (probably needed in more places)
would like string @'32 to be the single character @'32 instead of the
@y
would like string @'32 to be printed as the single character @'32 instead
of the
@z

% [4.51] Open the pool file using a path, and can't do string
% assignments directly.  (`strcpy' and `strlen' work here because
% `pool_name' is a constant string, and thus ends in a null and doesn't
% start with a space.)
@x
name_of_file:=pool_name; {we needn't set |name_length|}
if a_open_in(pool_file) then
@y
name_length := strlen (pool_name);
name_of_file := xmalloc_array (ASCII_code, 1 + name_length);
strcpy (stringcast(name_of_file+1), pool_name); {copy the string}
if a_open_in (pool_file, kpse_mfpool_format) then
@z

@x [4.51,52,53] Make `MF.POOL' lowercase, and change how it's read.
else  bad_pool('! I can''t read MF.POOL.')
@y
else  bad_pool('! I can''t read ', pool_name, '; bad path?')
@z
@x
begin if eof(pool_file) then bad_pool('! MF.POOL has no check sum.');
@.MF.POOL has no check sum@>
read(pool_file,m,n); {read two digits of string length}
@y
begin if eof(pool_file) then bad_pool('! ', pool_name, ' has no check sum.');
@.MF.POOL has no check sum@>
read(pool_file,m); read(pool_file,n); {read two digits of string length}
@z
@x
    bad_pool('! MF.POOL line doesn''t begin with two digits.');
@y
    bad_pool('! ', pool_name, ' line doesn''t begin with two digits.');
@z
@x
  bad_pool('! MF.POOL check sum doesn''t have nine digits.');
@y
  bad_pool('! ', pool_name, ' check sum doesn''t have nine digits.');
@z
@x
done: if a<>@$ then bad_pool('! MF.POOL doesn''t match; TANGLE me again.');
@y
done: if a<>@$ then
  bad_pool('! ', pool_name, ' doesn''t match; tangle me again (or fix the path).');
@z

@x [5.54] error_line is a variable, so can't be a subrange array bound
@!trick_buf:array[0..error_line] of ASCII_code; {circular buffer for
@y
@!trick_buf:array[0..ssup_error_line] of ASCII_code; {circular buffer for
@z

@x [5.58] Do not expand printable characters.
if (s<256)and(selector>pseudo) then print_char(s)
@y
if (s<256)and((selector>pseudo)or xprn[s]) then print_char(s)
@z

@x [5.59] Do not expand printable characters.
if (s<256)and(selector>pseudo) then print_char(s)
@y
if (s<256)and((selector>pseudo) or xprn[s])then print_char(s)
@z

@x [5.61] Print rest of banner.
wterm(banner);
if base_ident=0 then wterm_ln(' (no base preloaded)')
else  begin slow_print(base_ident); print_ln;
  end;
@y
wterm (banner);
wterm (version_string);
if base_ident=0 then wterm_ln(' (preloaded base=',dump_name,')')
else  begin slow_print(base_ident); print_ln;
  end;
if translate_filename then begin
  wterm('(');
  fputs(translate_filename, stdout);
  wterm_ln(')');
end;
@z

@x [6.68] l.1603 - Add unspecified_mode.
@d error_stop_mode=3 {stops at every opportunity to interact}
@y
@d error_stop_mode=3 {stops at every opportunity to interact}
@d unspecified_mode=4 {extra value for command-line switch}
@z

@x [6.68] l.1605 - file:line:error style messages.
  print_nl("! "); print(#);
@y
  if (file_line_error_style_p and not terminal_input) then
  begin
    print_nl ("");
    print (full_source_filename_stack[in_open]);
    print (":"); print_int (line); print (": ");
    print (#);
  end
  else begin print_nl("! "); print(#) end;
@z

@x [6.68] l.1610 - Add interaction_option.
@!interaction:batch_mode..error_stop_mode; {current level of interaction}
@y
@!interaction:batch_mode..error_stop_mode; {current level of interaction}
@!interaction_option:batch_mode..unspecified_mode; {set from command line}
@z

@x [6.69] l.1612 - Allow override by command line switch.
@ @<Set init...@>=interaction:=error_stop_mode;
@y
@ @<Set init...@>=if interaction_option=unspecified_mode then
  interaction:=error_stop_mode
else
  interaction:=interaction_option;
@z

@x [6.76] Eliminate non-local goto.
@ The |jump_out| procedure just cuts across all active procedure levels and
goes to |end_of_MF|. This is the only nontrivial |@!goto| statement in the
whole program. It is used when there is no recovery from a particular error.

Some \PASCAL\ compilers do not implement non-local |goto| statements.
@^system dependencies@>
In such cases the body of |jump_out| should simply be
`|close_files_and_terminate|;\thinspace' followed by a call on some system
procedure that quietly terminates the program.

@<Error hand...@>=
procedure jump_out;
begin goto end_of_MF;
end;
@y
@ The |jump_out| procedure just cuts across all active procedure levels.
The body of |jump_out| simply calls
`|close_files_and_terminate|;\thinspace' followed by a call on some system
procedure that quietly terminates the program.
@^system dependencies@>

@f noreturn==procedure

@d do_final_end==begin
   update_terminal;
   ready_already:=0;
   if (history <> spotless) and (history <> warning_issued) then
       uexit(1)
   else
       uexit(0);
   end
@<Error hand...@>=
noreturn procedure jump_out;
begin
close_files_and_terminate;
do_final_end;
end;
@z

@x [6.77] l.1736 -- halt on error?
print_char("."); show_context;
@y
print_char("."); show_context;
if (halt_on_error_p) then begin
  history:=fatal_error_stop; jump_out;
end;
@z

@x [6.79] Handle the switch-to-editor option.
line ready to be edited. But such an extension requires some system
wizardry, so the present implementation simply types out the name of the
file that should be
edited and the relevant line number.
@^system dependencies@>

There is a secret `\.D' option available when the debugging routines haven't
been commented~out.
@^debugging@>
@y
line ready to be edited.
We do this by calling the external procedure |call_edit| with a pointer to
the filename, its length, and the line number.
However, here we just set up the variables that will be used as arguments,
since we don't want to do the switch-to-editor until after \MF\ has closed
its files.
@^system dependencies@>

There is a secret `\.D' option available when the debugging routines have
not been commented out.
@^debugging@>
@d edit_file==input_stack[file_ptr]
@z
@x
"E": if file_ptr>0 then
  begin print_nl("You want to edit file ");
@.You want to edit file x@>
  slow_print(input_stack[file_ptr].name_field);
  print(" at line "); print_int(line);@/
  interaction:=scroll_mode; jump_out;
@y
"E": if file_ptr>0 then
    begin
    edit_name_start:=str_start[edit_file.name_field];
    edit_name_length:=str_start[edit_file.name_field+1] -
    		      str_start[edit_file.name_field];
    edit_line:=line;
    jump_out;
@z

@x [6.88] Declare fatal_error as noreturn.
procedure fatal_error(@!s:str_number); {prints |s|, and that's it}
@y
noreturn procedure fatal_error(@!s:str_number); {prints |s|, and that's it}
@z

@x [6.89] Declare overflow as noreturn.
procedure overflow(@!s:str_number;@!n:integer); {stop due to finiteness}
@y
noreturn procedure overflow(@!s:str_number;@!n:integer); {stop due to finiteness}
@z

@x [6.90] Declare confusion as noreturn.
procedure confusion(@!s:str_number);
@y
noreturn procedure confusion(@!s:str_number);
@z

@x [7.96] Do half in cpascal.h. And add halfp as in MetaPost for speed.
@d half(#)==(#) div 2
@y
@z

@x [7.102] Use halfp.
round_decimals:=half(a+1);
@y
round_decimals:=halfp(a+1);
@z

@x [7.107-7.114] Optionally replace make_fraction etc. with external routines
@p function make_fraction(@!p,@!q:integer):fraction;
@y
In the C version, there are external routines that use double precision
floating point to simulate functions such as |make_fraction|.  This is
carefully done to be virtually machine-independent and it gives up to 12
times speed-up on machines with hardware floating point.  Since some
machines do not have fast double-precision floating point, we provide a
C preprocessor switch that allows selecting the standard versions given
below. (There's no configure option to select FIXPT, however, since I
don't expect anyone will actually notice.)

@p ifdef('FIXPT')@/
function make_fraction(@!p,@!q:integer):fraction;
@z
@x [7.107]
  if negative then make_fraction:=-(f+n)@+else make_fraction:=f+n;
  end;
end;
@y
  if negative then make_fraction:=-(f+n)@+else make_fraction:=f+n;
  end;
end;@/
endif('FIXPT')
@z
@x [7.109]
@p function take_fraction(@!q:integer;@!f:fraction):integer;
@y
@p ifdef('FIXPT')@/
function take_fraction(@!q:integer;@!f:fraction):integer;
@z
@x [7.109]
else take_fraction:=n+p;
end;
@y
else take_fraction:=n+p;
end;@/
endif('FIXPT')
@z

@x [7.111] Use halfp.
  repeat if odd(f) then p:=half(p+q)@+else p:=half(p);
  f:=half(f);
  until f=1
else  repeat if odd(f) then p:=p+half(q-p)@+else p:=half(p);
  f:=half(f);
@y
  repeat if odd(f) then p:=halfp(p+q)@+else p:=halfp(p);
  f:=halfp(f);
  until f=1
else  repeat if odd(f) then p:=p+halfp(q-p)@+else p:=halfp(p);
  f:=halfp(f);
@z

@x [7.112]
@p function take_scaled(@!q:integer;@!f:scaled):integer;
@y
@p ifdef('FIXPT')@/
function take_scaled(@!q:integer;@!f:scaled):integer;
@z
@x [7.112]
else take_scaled:=n+p;
end;
@y
else take_scaled:=n+p;
end;@/
endif('FIXPT')
@z

@x [7.113] Use halfp.
  repeat if odd(f) then p:=half(p+q)@+else p:=half(p);
  f:=half(f);
  until f=1
else  repeat if odd(f) then p:=p+half(q-p)@+else p:=half(p);
  f:=half(f);
@y
  repeat if odd(f) then p:=halfp(p+q)@+else p:=halfp(p);
  f:=halfp(f);
  until f=1
else  repeat if odd(f) then p:=p+halfp(q-p)@+else p:=halfp(p);
  f:=halfp(f);
@z

@x [7.114]
operands are positive. \ (This procedure is not used especially often,
so it is not part of \MF's inner loop.)

@p function make_scaled(@!p,@!q:integer):scaled;
@y
operands are positive. \ (This procedure is not used especially often,
so it is not part of \MF's inner loop, but we might as well allow for
an external C routine.)

@p ifdef('FIXPT')@/
function make_scaled(@!p,@!q:integer):scaled;
@z
@x [7.114]
  if negative then make_scaled:=-(f+n)@+else make_scaled:=f+n;
  end;
end;
@y
  if negative then make_scaled:=-(f+n)@+else make_scaled:=f+n;
  end;
end;@/
endif('FIXPT')
@z

@x [7.119] Do floor_scaled, floor_unscaled, round_unscaled, round_fraction in C.
@p function floor_scaled(@!x:scaled):scaled;
  {$2^{16}\lfloor x/2^{16}\rfloor$}
var @!be_careful:integer; {temporary register}
begin if x>=0 then floor_scaled:=x-(x mod unity)
else  begin be_careful:=x+1;
  floor_scaled:=x+((-be_careful) mod unity)+1-unity;
  end;
end;
@#
function floor_unscaled(@!x:scaled):integer;
  {$\lfloor x/2^{16}\rfloor$}
var @!be_careful:integer; {temporary register}
begin if x>=0 then floor_unscaled:=x div unity
else  begin be_careful:=x+1; floor_unscaled:=-(1+((-be_careful) div unity));
  end;
end;
@#
function round_unscaled(@!x:scaled):integer;
  {$\lfloor x/2^{16}+.5\rfloor$}
var @!be_careful:integer; {temporary register}
begin if x>=half_unit then round_unscaled:=1+((x-half_unit) div unity)
else if x>=-half_unit then round_unscaled:=0
else  begin be_careful:=x+1;
  round_unscaled:=-(1+((-be_careful-half_unit) div unity));
  end;
end;
@#
function round_fraction(@!x:fraction):scaled;
  {$\lfloor x/2^{12}+.5\rfloor$}
var @!be_careful:integer; {temporary register}
begin if x>=2048 then round_fraction:=1+((x-2048) div 4096)
else if x>=-2048 then round_fraction:=0
else  begin be_careful:=x+1;
  round_fraction:=-(1+((-be_careful-2048) div 4096));
  end;
end;
@y
@z

@x [8.121] Use halfp.
  square_rt:=half(q);
@y
  square_rt:=halfp(q);
@z

@x [8.126] Use halfp.
  else  begin a:=half(a); b:=half(b); big:=true;
@y
  else  begin a:=halfp(a); b:=halfp(b); big:=true;
@z

@x [8.133] Use halfp.
  begin z:=half(z+1); k:=k+1;
@y
  begin z:=halfp(z+1); k:=k+1;
@z

@x [8.142] Use halfp.
  begin x:=half(x); y:=half(y);
@y
  begin x:=halfp(x); y:=halfp(y);
@z

@x [8.150] Use halfp.
while j>=fraction_one do j:=half(j);
@y
while j>=fraction_one do j:=halfp(j);
@z

@x [9.153] Increase memory size.
@d min_quarterword=0 {smallest allowable value in a |quarterword|}
@d max_quarterword=255 {largest allowable value in a |quarterword|}
@d min_halfword==0 {smallest allowable value in a |halfword|}
@d max_halfword==65535 {largest allowable value in a |halfword|}
@y
@d min_quarterword=0 {smallest allowable value in a |quarterword|}
@d max_quarterword=255 {largest allowable value in a |quarterword|}
@d min_halfword==0 {smallest allowable value in a |halfword|}
@d max_halfword==@"FFFFFFF {largest allowable value in a |halfword|}
@z

@x [9.155] Don't bother to subtract zero.
@d ho(#)==#-min_halfword
  {to take a sixteen-bit item from a halfword}
@d qo(#)==#-min_quarterword {to read eight bits from a quarterword}
@d qi(#)==#+min_quarterword {to store eight bits in a quarterword}
@y
@d ho(#)==#
@d qo(#)==#
@d qi(#)==#
@z

@x [9.156] memory_word is defined externally.
@!two_halves = packed record@;@/
  @!rh:halfword;
  case two_choices of
  1: (@!lh:halfword);
  2: (@!b0:quarterword; @!b1:quarterword);
  end;
@!four_quarters = packed record@;@/
  @!b0:quarterword;
  @!b1:quarterword;
  @!b2:quarterword;
  @!b3:quarterword;
  end;
@!memory_word = record@;@/
  case three_choices of
  1: (@!int:integer);
  2: (@!hh:two_halves);
  3: (@!qqqq:four_quarters);
  end;
@y
@=#include "texmfmem.h";@>
@z

@x [10.159] mem is dynamically allocated.
@!mem : array[mem_min..mem_max] of memory_word; {the big dynamic storage area}
@y
@!mem : ^memory_word; {the big dynamic storage area}
@z

% [11.178] Change the word `free' so that it doesn't conflict with the
% standard C library routine of the same name. Also change arrays that
% use mem_max, since that's a variable now, effectively disabling the feature.
@x
are debugging.)

@<Glob...@>=
@!debug @!free: packed array [mem_min..mem_max] of boolean; {free cells}
@t\hskip1em@>@!was_free: packed array [mem_min..mem_max] of boolean;
@y
are debugging.)

@d free==free_arr
@<Glob...@>=
@!debug @!free: packed array [0..1] of boolean; {free cells; this loses}
@t\hskip1em@>@!was_free: packed array [0..1] of boolean; {this loses too}
@z

@x [11.182] Eliminate unsigned comparisons to zero.
repeat if (p>=lo_mem_max)or(p<mem_min) then clobbered:=true
  else if (rlink(p)>=lo_mem_max)or(rlink(p)<mem_min) then clobbered:=true
@y
repeat if (p>=lo_mem_max) then clobbered:=true
  else if (rlink(p)>=lo_mem_max) then clobbered:=true
@z

@x [12.194] Do `fix_date_and_time' in C.
@ The following procedure, which is called just before \MF\ initializes its
input and output, establishes the initial values of the date and time.
@^system dependencies@>
Since standard \PASCAL\ cannot provide such information, something special
is needed. The program here simply specifies July 4, 1776, at noon; but
users probably want a better approximation to the truth.

Note that the values are |scaled| integers. Hence \MF\ can no longer
be used after the year 32767.

@p procedure fix_date_and_time;
begin internal[time]:=12*60*unity; {minutes since midnight}
internal[day]:=4*unity; {fourth day of the month}
internal[month]:=7*unity; {seventh month of the year}
internal[year]:=1776*unity; {Anno Domini}
end;
@y
@ The following procedure, which is called just before \MF\ initializes its
input and output, establishes the initial values of the date and time.
It is calls an externally defined |date_and_time|, even though it could
be done from Pascal.
The external procedure also sets up interrupt catching.
@^system dependencies@>

Note that the values are |scaled| integers. Hence \MF\ can no longer
be used after the year 32767.

@p procedure fix_date_and_time;
begin
    date_and_time(internal[time],internal[day],internal[month],internal[year]);
    internal[time] := internal[time] * unity;
    internal[day] := internal[day] * unity;
    internal[month] := internal[month] * unity;
    internal[year] := internal[year] * unity;
end;
@z

@x [12.198] Change class to c_class to avoid C++ keyword.
@d max_class=20 {the largest class number}
@y
@d max_class=20 {the largest class number}
@d class==c_class
@z

@x [12.199] Allow tab and form feed as input.
for k:=127 to 255 do char_class[k]:=invalid_class;
@y
for k:=127 to 255 do char_class[k]:=invalid_class;
char_class[tab]:=space_class;
char_class[form_feed]:=space_class;
@z

@x [15.232] Use halfp.
name_type(q+s):=half(s)+x_part_sector; link(q+s):=null;
@y
name_type(q+s):=halfp(s)+x_part_sector; link(q+s):=null;
@z

 [20.329] |valid_range| uses |abs|, which we have defined as a C
% macro.  Some C preprocessors cannot expand the giant argument here.
% So we add a temporary.
@x
@p procedure edge_prep(@!ml,@!mr,@!nl,@!nr:integer);
var @!delta:halfword; {amount of change}
@y
@p procedure edge_prep(@!ml,@!mr,@!nl,@!nr:integer);
var @!delta:halfword; {amount of change}
temp:integer;
@z
@x
if not valid_range(m_min(cur_edges)+m_offset(cur_edges)-zero_field) or@|
 not valid_range(m_max(cur_edges)+m_offset(cur_edges)-zero_field) then
@y
temp := m_offset (cur_edges) - zero_field;
if not valid_range (m_min (cur_edges) + temp)
   or not valid_range (m_max (cur_edges) + temp)
then
@z

@x [21.442] Use halfp.
if odd(right_type(q)) then a:=good_val(b,pen_edge+half(cur_gran))
else a:=good_val(b-1,pen_edge+half(cur_gran));
@y
if odd(right_type(q)) then a:=good_val(b,pen_edge+halfp(cur_gran))
else a:=good_val(b-1,pen_edge+halfp(cur_gran));
@z

@x [24.509] i18n fix
print(" ("); print_int(info(h)); print(" offset");
if info(h)<>1 then print_char("s");
@y
print(" ("); print_int(info(h));
if info(h)<>1 then print(" offsets")
else print(" offset");
@z

% [25.530] |make_fraction| and |take_fraction| arguments are too long for
% some preprocessors, when they were defined as macros, just as in the
% previous change.
@x
  alpha:=take_fraction(take_fraction(major_axis,
      make_fraction(gamma,beta)),n_cos)@|
    -take_fraction(take_fraction(minor_axis,
      make_fraction(delta,beta)),n_sin);
  alpha:=(alpha+half_unit) div unity;
  gamma:=pyth_add(take_fraction(major_axis,n_cos),
    take_fraction(minor_axis,n_sin));
@y
  alpha := make_fraction (gamma, beta);
  alpha := take_fraction (major_axis, alpha);
  alpha := take_fraction (alpha, n_cos);
  alpha := (alpha+half_unit) div unity;
  gamma := take_fraction (minor_axis, n_sin);
  gamma := pyth_add (take_fraction (major_axis, n_cos), gamma);
@z

@x [26.556] Use halfp.
        begin cur_t:=half(cur_t+1); cur_tt:=half(cur_tt+1); return;
@y
        begin cur_t:=halfp(cur_t+1); cur_tt:=halfp(cur_tt+1); return;
@z

@x [26.561] Use halfp.
begin cur_t:=half(cur_t); cur_tt:=half(cur_tt);
@y
begin cur_t:=halfp(cur_t); cur_tt:=halfp(cur_tt);
@z

@x [27.564] The window functions are defined externally, in C.
@p function init_screen:boolean;
begin init_screen:=false;
end;
@#
procedure update_screen; {will be called only if |init_screen| returns |true|}
begin @!init wlog_ln('Calling UPDATESCREEN');@+tini {for testing only}
end;
@y
{These functions/procedures are defined externally in C.}
@z

@x [27.565] screen_row, screen_col are variables, so can't be subrange array bounds.
@!screen_row=0..screen_depth; {a row number on the screen}
@!screen_col=0..screen_width; {a column number on the screen}
@!trans_spec=array[screen_col] of screen_col; {a transition spec, see below}
@y
@!screen_row=0..ssup_screen_depth; {a row number on the screen}
@!screen_col=0..ssup_screen_width; {a column number on the screen}
@!trans_spec=^screen_col; {a transition spec, see below}
@z

@x [27.567]
@p procedure blank_rectangle(@!left_col,@!right_col:screen_col;
  @!top_row,@!bot_row:screen_row);
var @!r:screen_row;
@!c:screen_col;
begin @{@+for r:=top_row to bot_row-1 do
  for c:=left_col to right_col-1 do
    screen_pixel[r,c]:=white;@+@}@/
@!init wlog_cr; {this will be done only after |init_screen=true|}
wlog_ln('Calling BLANKRECTANGLE(',left_col:1,',',
  right_col:1,',',top_row:1,',',bot_row:1,')');@+tini
end;
@y
{Same thing.}
@z

@x [27.568]
@p procedure paint_row(@!r:screen_row;@!b:pixel_color;var @!a:trans_spec;
  @!n:screen_col);
var @!k:screen_col; {an index into |a|}
@!c:screen_col; {an index into |screen_pixel|}
begin @{ k:=0; c:=a[0];
repeat incr(k);
  repeat screen_pixel[r,c]:=b; incr(c);
  until c=a[k];
  b:=black-b; {$|black|\swap|white|$}
  until k=n;@+@}@/
@!init wlog('Calling PAINTROW(',r:1,',',b:1,';');
  {this is done only after |init_screen=true|}
for k:=0 to n do
  begin wlog(a[k]:1); if k<>n then wlog(',');
  end;
wlog_ln(')');@+tini
end;
@y
{Same thing}
@z

@x [28.596] Use halfp.
if abs(v)>half(threshold) then
@y
if abs(v)>halfp(threshold) then
@z

@x [31.631] l.13346 - Add datastructures for file:line:error.
@!line_stack : array[1..max_in_open] of integer;
@y
@!line_stack : array[1..max_in_open] of integer;
@!source_filename_stack : ^str_number;
@!full_source_filename_stack : ^str_number;
@z

@x [38.768] Area and extension rules.
@ The file names we shall deal with for illustrative purposes have the
following structure:  If the name contains `\.>' or `\.:', the file area
consists of all characters up to and including the final such character;
otherwise the file area is null.  If the remaining file name contains
`\..', the file extension consists of all such characters from the first
remaining `\..' to the end, otherwise the file extension is null.
@^system dependencies@>

We can scan such file names easily by using two global variables that keep track
of the occurrences of area and extension delimiters:

@<Glob...@>=
@!area_delimiter:pool_pointer; {the most recent `\.>' or `\.:', if any}
@!ext_delimiter:pool_pointer; {the relevant `\..', if any}
@y
@ The file names we shall deal with for illustrative purposes have the
following structure:  If the name contains `\./', the file area
consists of all characters up to and including the final such character;
otherwise the file area is null.  If the remaining file name contains
`\..', the file extension consists of all such characters from the first
remaining `\..' to the end, otherwise the file extension is null.
@^system dependencies@>

We can scan such file names easily by using two global variables that keep
track of the occurrences of area and extension delimiters:

@<Glob...@>=
@!area_delimiter:pool_pointer; {the most recent `\./', if any}
@!ext_delimiter:pool_pointer; {the most recent `\..', if any}
@z

@x [38.769] MF area directories.
@d MF_area=="MFinputs:"
@.MFinputs@>
@y
In C, the default paths are specified separately.
@z

@x [38.770] filenames: quoted
begin area_delimiter:=0; ext_delimiter:=0;
@y
begin area_delimiter:=0; ext_delimiter:=0; quoted_filename:=false;
@z

@x [38.771] more_name
begin if c=" " then more_name:=false
else  begin if (c=">")or(c=":") then
@y
begin
if c="""" then begin
  quoted_filename:=not quoted_filename;
  more_name:=true;
  end
else if ((c=" ")or(c=tab)) and stop_at_space and (not quoted_filename) then
  more_name:=false
else  begin
  if IS_DIR_SEP (c) then
@z

@x [38.771] more_name
  else if (c=".")and(ext_delimiter=0) then ext_delimiter:=pool_ptr;
@y
  else if c="." then ext_delimiter:=pool_ptr;
@z

@x [38.772] end_name: quote if spaces in names.
@p procedure end_name;
@y
@d pool_seq_check(#) == {set |s:=str_start[str_ptr]| and |t:=#|,
      then check if sequence of pool bytes |s<=j<t| needs quoting}
   must_quote:=false;
   s:=str_start[str_ptr];
   t:=#;
   j:=s;
   while (not must_quote) and (j<t) do begin
     must_quote:=str_pool[j]=" "; incr(j);
     end
@d pool_seq_quote_move == {quote sequence of pool bytes |s<=j<t|,
      first moving up pool bytes |t<=j<pool_ptr|}
   for j:=pool_ptr-1 downto t do str_pool[j+2]:=str_pool[j];
   pool_seq_quote
@d pool_seq_quote == {quote sequence of pool bytes |s<=j<t|}
   str_pool[t+1]:="""";
   for j:=t-1 downto s do str_pool[j+1]:=str_pool[j];
   str_pool[s]:="""";
   pool_ptr:=pool_ptr+2

@p procedure end_name;
var must_quote:boolean; {whether we need to quote a string}
@!j,@!s,@!t: pool_pointer; {running indices}
@z

@x [38.772] end_name: quote if spaces in names.
if area_delimiter=0 then cur_area:=""
else  begin cur_area:=str_ptr; incr(str_ptr);
  str_start[str_ptr]:=area_delimiter+1;
  end;
if ext_delimiter=0 then
  begin cur_ext:=""; cur_name:=make_string;
  end
else  begin cur_name:=str_ptr; incr(str_ptr);
  str_start[str_ptr]:=ext_delimiter; cur_ext:=make_string;
  end;
@y
str_room(6); {room for quotes, if they are needed}
if area_delimiter=0 then cur_area:=""
else  begin {maybe quote |cur_area|}
  pool_seq_check(area_delimiter+1);
  if must_quote then begin
    pool_seq_quote_move;
    area_delimiter:=area_delimiter+2;
    if ext_delimiter<>0 then ext_delimiter:=ext_delimiter+2;
    end;
  cur_area:=str_ptr; incr(str_ptr);
  str_start[str_ptr]:=area_delimiter+1;
  end;
if ext_delimiter=0 then cur_ext:=""
else  begin {maybe quote |cur_name| followed by |cur_ext|}
  pool_seq_check(ext_delimiter);
  if must_quote then begin
    pool_seq_quote_move;
    ext_delimiter:=ext_delimiter+2;
    end;
  cur_name:=str_ptr; incr(str_ptr);
  str_start[str_ptr]:=ext_delimiter;
  end;
{maybe quote |cur_ext| if present or |cur_name| otherwise}
pool_seq_check(pool_ptr);
if must_quote then begin
  pool_seq_quote;
  end;
if ext_delimiter=0 then cur_name:=make_string
else cur_ext:=make_string;
@z

@x [38.773] print_file_name: quote if spaces in names.
@<Basic printing...@>=
procedure print_file_name(@!n,@!a,@!e:integer);
begin slow_print(a); slow_print(n); slow_print(e);
@y
@d string_check(#) == {check if string |#| needs quoting}
   if #<>0 then begin
     j:=str_start[#];
     while (not must_quote) and (j<str_start[#+1]) do begin
       must_quote:=str_pool[j]=" "; incr(j);
     end;
   end
@d print_quoted(#) == {print string |#|, omitting quotes}
   if #<>0 then
     for j:=str_start[#] to str_start[#+1]-1 do
       if so(str_pool[j])<>"""" then
         print(so(str_pool[j]))

@<Basic printing...@>=
procedure print_file_name(@!n,@!a,@!e:integer);
var must_quote: boolean; {whether to quote the filename}
@!j:pool_pointer; {index into |str_pool|}
begin
must_quote:=false;
string_check(a); string_check(n); string_check(e);
if must_quote then slow_print("""");
print_quoted(a); print_quoted(n); print_quoted(e);
if must_quote then slow_print("""");
@z

@x [38.774] have append_to_name skip quotes.
@d append_to_name(#)==begin c:=#; incr(k);
  if k<=file_name_size then name_of_file[k]:=xchr[c];
  end
@y
@d append_to_name(#)==begin c:=#; if not (c="""") then begin incr(k);
  if k<=file_name_size then name_of_file[k]:=xchr[c];
  end end
@z

@x [38.774] (pack_file_name) malloc and null terminate name_of_file.
for j:=str_start[a] to str_start[a+1]-1 do append_to_name(so(str_pool[j]));
@y
if name_of_file then libc_free (name_of_file);
name_of_file := xmalloc_array (ASCII_code, length(a)+length(n)+length(e)+1);
for j:=str_start[a] to str_start[a+1]-1 do append_to_name(so(str_pool[j]));
@z
@x
for k:=name_length+1 to file_name_size do name_of_file[k]:=' ';
@y
name_of_file[name_length + 1] := 0;
@z

@x [38.775] The default base.
@d base_default_length=18 {length of the |MF_base_default| string}
@d base_area_length=8 {length of its area part}
@y
@d base_area_length=0 {no fixed area in C}
@z

@x [38.776] Where `plain.base' is.
@!MF_base_default:packed array[1..base_default_length] of char;

@ @<Set init...@>=
MF_base_default:='MFbases:plain.base';
@y
@!base_default_length: integer;
@!MF_base_default: cstring;

@ We set the name of the default format file and the length of that name
in \.{texmfmp.c}, since we want them to depend on the name of the
program.
@z

@x [38.778] Change to pack_buffered_name as with pack_file_name.
for j:=1 to n do append_to_name(xord[MF_base_default[j]]);
for j:=a to b do append_to_name(buffer[j]);
for j:=base_default_length-base_ext_length+1 to base_default_length do
  append_to_name(xord[MF_base_default[j]]);
if k<=file_name_size then name_length:=k@+else name_length:=file_name_size;
for k:=name_length+1 to file_name_size do name_of_file[k]:=' ';
@y
if name_of_file then libc_free (name_of_file);
name_of_file := xmalloc_array (ASCII_code,  n + (b-a+1) + base_ext_length + 1);
for j:=1 to n do append_to_name(xord[ucharcast(MF_base_default[j])]);
for j:=a to b do append_to_name(buffer[j]);
for j:=base_default_length-base_ext_length+1 to base_default_length do
  append_to_name(xord[ucharcast(MF_base_default[j])]);
if k<=file_name_size then name_length:=k@+else name_length:=file_name_size;
name_of_file[name_length + 1] := 0;
@z

@x [38.779] Base file opening: do path searching for the default, not plain.
  pack_buffered_name(0,loc,j-1); {try first without the system file area}
  if w_open_in(base_file) then goto found;
  pack_buffered_name(base_area_length,loc,j-1);
    {now try the system base file area}
  if w_open_in(base_file) then goto found;
@y
  pack_buffered_name(0,loc,j-1);
  if w_open_in(base_file) then goto found;
@z
@x
  wterm_ln('Sorry, I can''t find that base;',' will try PLAIN.');
@y
  wterm ('Sorry, I can''t find the base `');
  fputs (stringcast(name_of_file + 1), stdout);
  wterm ('''; will try `');
  fputs (MF_base_default + 1, stdout);
  wterm_ln ('''.');
@z
@x
  wterm_ln('I can''t find the PLAIN base file!');
@.I can't find PLAIN...@>
@y
  wterm ('I can''t find the base file `');
  fputs (MF_base_default + 1, stdout);
  wterm_ln ('''!');
@.I can't find the base...@>
@z

@x [38.780] make_name_string
  make_name_string:=make_string;
  end;
@y
  make_name_string:=make_string;
  end;
  {At this point we also set |cur_name|, |cur_ext|, and |cur_area| to
   match the contents of |name_of_file|.}
  k:=1;
  begin_name;
  stop_at_space:=false;
  while (k<=name_length)and(more_name(name_of_file[k])) do
    incr(k);
  stop_at_space:=true;
  end_name;
@z

@x [38.781] Make scan_file_name ignore leading tabs as well as spaces.
while buffer[loc]=" " do incr(loc);
@y
while (buffer[loc]=" ")or(buffer[loc]=tab) do incr(loc);
@z

@x [38.782] `logname' is declared in <unistd.h> on some systems.
`\.{.base}' and `\.{.tfm}' in the names of \MF's output files.
@y
`\.{.base}' and `\.{.tfm}' in the names of \MF's output files.
@d log_name == texmf_log_name
@z

@x [38.786] prompt_file_name: avoid empty filenames.
var @!k:0..buf_size; {index into |buffer|}
@y
var @!k:0..buf_size; {index into |buffer|}
@!saved_cur_name:str_number; {to catch empty terminal input}
@z

@x [38.786] prompt_file_name: avoid empty filenames.
clear_terminal; prompt_input(": "); @<Scan file name in the buffer@>;
if cur_ext="" then cur_ext:=e;
@y
saved_cur_name:=cur_name;
clear_terminal; prompt_input(": "); @<Scan file name in the buffer@>;
if cur_ext="" then cur_ext:=e;
if length(cur_name)=0 then cur_name:=saved_cur_name;
@z

@x [38.787] <Scan file name...> needs similar leading tab treatment.
while (buffer[k]=" ")and(k<last) do incr(k);
@y
while ((buffer[k]=" ")or(buffer[k]=tab))and(k<last) do incr(k);
@z

@x [38.788] Adjust for C string conventions.
@!months:packed array [1..36] of char; {abbreviations of month names}
@y
@!months:const_cstring;
@z

@x [38.788] Set correct filename for recorder.
if job_name=0 then job_name:="mfput";
@.mfput@>
pack_job_name(".log");
@y
if job_name=0 then job_name:=get_job_name("mfput");
@.mfput@>
pack_job_name(".fls");
recorder_change_filename(stringcast(name_of_file+1));
pack_job_name(".log");
@z

@x [38.790]
begin wlog(banner);
slow_print(base_ident); print("  ");
print_int(round_unscaled(internal[day])); print_char(" ");
months:='JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC';
@y
begin wlog(banner);
wlog (version_string);
slow_print(base_ident); print("  ");
print_int(round_unscaled(internal[day])); print_char(" ");
months := ' JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC';
@z

@x [38.790] l.15898 - Print TCX name, if given.
end
@y
if translate_filename then begin
  wlog_cr;
  wlog('(');
  fputs(translate_filename, log_file);
  wlog(')');
end;
end
@z

@x [38.793] (start_input) a_open_in of input file needs path specifier.
begin @<Put the desired file name in |(cur_name,cur_ext,cur_area)|@>;
if cur_ext="" then cur_ext:=".mf";
pack_cur_name;
loop@+  begin begin_file_reading; {set up |cur_file| and new level of input}
  if a_open_in(cur_file) then goto done;
  if cur_area="" then
    begin pack_file_name(cur_name,MF_area,cur_ext);
    if a_open_in(cur_file) then goto done;
    end;
@y Don't assume a single . in filenames.
begin @<Put the desired file name in |(cur_name,cur_ext,cur_area)|@>;
pack_cur_name;
loop@+begin
  begin_file_reading; {set up |cur_file| and new level of input}
  if cur_ext = ".mf" then begin
    cur_ext := "";
    pack_cur_name;
    end;
  {Kpathsea tries all the various ways to get the file.}
  if kpse_in_name_ok(stringcast(name_of_file+1))
     and a_open_in(cur_file, kpse_mf_format) then
    goto done;
@z

@x [38.793] l.15938 - The job name may have been given on the command line.
  begin job_name:=cur_name; open_log_file;
@y
  begin job_name:=get_job_name(cur_name); open_log_file;
@z

@x [38.793] Can't return name to string pool because of editor option?
if name=str_ptr-1 then {we can conserve string pool space now}
  begin flush_string(name); name:=cur_name;
  end;
@y
@z

@x [41.866] Use halfp.
major_axis:=half(a_minus_b+a_plus_b); minor_axis:=half(abs(a_plus_b-a_minus_b));
if major_axis=minor_axis then theta:=0 {circle}
else theta:=half(n_arg(txx-tyy,tyx+txy)+n_arg(txx+tyy,tyx-txy));
@y
major_axis:=halfp(a_minus_b+a_plus_b); minor_axis:=halfp(abs(a_plus_b-a_minus_b));
if major_axis=minor_axis then theta:=0 {circle}
else theta:=half(n_arg(txx-tyy,tyx+txy)+n_arg(txx+tyy,tyx-txy));
@z

@x [44.1023] if batchmode, MakeTeX... scripts should be silent.
mode_command: begin print_ln; interaction:=cur_mod;
@y
mode_command: begin print_ln; interaction:=cur_mod;
if interaction = batch_mode
then kpse_make_tex_discard_errors := 1
else kpse_make_tex_discard_errors := 0;
@z

% [45.1120] `threshold' is both a function and a variable.  Since the
% function is used much less often than the variable, we'll change that
@x
@p function threshold(@!m:integer):scaled;
var @!d:scaled; {lower bound on the smallest interval size}
begin excess:=min_cover(0)-m;
if excess<=0 then threshold:=0
else  begin repeat d:=perturbation;
  until min_cover(d+d)<=m;
  while min_cover(d)>m do d:=perturbation;
  threshold:=d;
@y
@p function threshold_fn(@!m:integer):scaled;
var @!d:scaled; {lower bound on the smallest interval size}
begin excess:=min_cover(0)-m;
if excess<=0 then threshold_fn:=0
else  begin repeat d:=perturbation;
  until min_cover(d+d)<=m;
  while min_cover(d)>m do d:=perturbation;
  threshold_fn:=d;
@z

@x [45.1121] Change the call to the threshold function.
begin d:=threshold(m); perturbation:=0;
@y
begin d:=threshold_fn(m); perturbation:=0;
@z

@x [45.1122] Use halfp.
v:=l+half(value(p)-l);
@y
v:=l+halfp(value(p)-l);
@z

@x [45.1133] Use C macros to do the TFM writing, to avoid casting(?) problems.
@d tfm_out(#)==write(tfm_file,#) {output one byte to |tfm_file|}

@p procedure tfm_two(@!x:integer); {output two bytes to |tfm_file|}
begin tfm_out(x div 256); tfm_out(x mod 256);
end;
@#
procedure tfm_four(@!x:integer); {output four bytes to |tfm_file|}
begin if x>=0 then tfm_out(x div three_bytes)
else  begin x:=x+@'10000000000; {use two's complement for negative values}
  x:=x+@'10000000000;
  tfm_out((x div three_bytes) + 128);
  end;
x:=x mod three_bytes; tfm_out(x div unity);
x:=x mod unity; tfm_out(x div @'400);
tfm_out(x mod @'400);
end;
@#
procedure tfm_qqqq(@!x:four_quarters); {output four quarterwords to |tfm_file|}
@y
The default definitions for |tfm_two| and |tfm_four| don't work.
I don't know why not. Some casting problem?

@d tfm_out(#) == put_byte (#, tfm_file)
@d tfm_two(#) == put_2_bytes (tfm_file, #)
@d tfm_four(#) == put_4_bytes (tfm_file, #)

@p procedure tfm_qqqq(@!x:four_quarters); {output four quarterwords to |tfm_file|}
@z

@x [45.1134] print_file_name
print_nl("Font metrics written on "); slow_print(metric_file_name);
@y
print_nl("Font metrics written on "); print_file_name(0,metric_file_name,0);
@z

@x [47.1152] declare gf_buf as a pointer, for dynamic allocated
@!gf_buf:array[gf_index] of eight_bits; {buffer for \.{GF} output}
@y
@!gf_buf:^eight_bits; {dynamically-allocated buffer for \.{GF} output}
@z

@x [47.1154] omit write_gf
@<Declare generic font output procedures@>=
procedure write_gf(@!a,@!b:gf_index);
var k:gf_index;
begin for k:=a to b do write(gf_file,gf_buf[k]);
end;
@y
In C, we use a macro to call |fwrite| or |write| directly, writing all
the bytes to be written in one shot.  Much better than writing four
bytes at a time.
@z

@x [47.1155] check gf file size
each time, we use the macro |gf_out|.
@y
each time, we use the macro |gf_out|.

The length of |gf_file| should not exceed |@"7FFFFFFF|; we set
|gf_prev_ptr:=0| to prevent further \.{GF} output causing infinite
recursion.
@z

@x [47.1155] gf_swap: check gf file size
begin if gf_limit=gf_buf_size then
@y
begin if gf_ptr>(@"7FFFFFFF-gf_offset) then
  begin gf_prev_ptr:=0;
  fatal_error("gf length exceeds ""7FFFFFFF");
@.gf length exceeds...@>
  end;
if gf_limit=gf_buf_size then
@z

@x [47.1156] empty the last bytes: check gf file size
if gf_ptr>0 then write_gf(0,gf_ptr-1)
@y
if gf_ptr>(@"7FFFFFFF-gf_offset) then
  begin gf_prev_ptr:=0;
  fatal_error("gf length exceeds ""7FFFFFFF");
@.gf length exceeds...@>
  end;
if gf_ptr>0 then write_gf(0,gf_ptr-1)
@z

@x [47.1163] C needs k to be 0..256 instead of 0..255.
procedure init_gf;
var @!k:eight_bits; {runs through all possible character codes}
@y
procedure init_gf;
var @!k:0..256; {runs through all possible character codes}
@z

@x [47.1182] print_file_name
print_nl("Output written on "); slow_print(output_file_name);
@y
print_nl("Output written on "); print_file_name(0,output_file_name,0);
@z

@x [47.1182] i18n fix
print(" ("); print_int(total_chars); print(" character");
if total_chars<>1 then print_char("s");
@y
print(" ("); print_int(total_chars);
if total_chars<>1 then print(" characters")
else print(" character");
@z

@x [48.1185] INI = VIR.
base_ident:=" (INIMF)";
@y
if ini_version then base_ident:=" (INIMF)";
@z

@x [48.1186] Add base_engine.
@!w: four_quarters; {four ASCII codes}
@y
@!w: four_quarters; {four ASCII codes}
@!base_engine: ^text_char;
@z

@x [48.1187] Add base_engine.
@!w: four_quarters; {four ASCII codes}
@y
@!w: four_quarters; {four ASCII codes}
@!base_engine: ^text_char;
@!dummy_xord: ASCII_code;
@!dummy_xchr: text_char;
@!dummy_xprn: ASCII_code;
@z

@x [48.1188] Reading and writing of `base_file' is done in C.
@d dump_wd(#)==begin base_file^:=#; put(base_file);@+end
@d dump_int(#)==begin base_file^.int:=#; put(base_file);@+end
@d dump_hh(#)==begin base_file^.hh:=#; put(base_file);@+end
@d dump_qqqq(#)==begin base_file^.qqqq:=#; put(base_file);@+end
@y
@z

@x [48.1189]
@d undump_wd(#)==begin get(base_file); #:=base_file^;@+end
@d undump_int(#)==begin get(base_file); #:=base_file^.int;@+end
@d undump_hh(#)==begin get(base_file); #:=base_file^.hh;@+end
@d undump_qqqq(#)==begin get(base_file); #:=base_file^.qqqq;@+end
@y
@z

@x [48.1190] Dump engine name.
dump_int(@$);@/
@y
dump_int(@"57324D46);  {Web2C \MF's magic constant: "W2MF"}
{Align engine to 4 bytes with one or more trailing NUL}
x:=strlen(engine_name);
base_engine:=xmalloc_array(text_char,x+4);
strcpy(stringcast(base_engine), engine_name);
for k:=x to x+3 do base_engine[k]:=0;
x:=x+4-(x mod 4);
dump_int(x);dump_things(base_engine[0], x);
libc_free(base_engine);@/
dump_int(@$);@/
@<Dump |xord|, |xchr|, and |xprn|@>;
@z

@x [48.1191] Avoid Pascal file convention.
x:=base_file^.int;
if x<>@$ then goto off_base; {check that strings are the same}
undump_int(x);
if x<>mem_min then goto off_base;
undump_int(x);
if x<>mem_top then goto off_base;
@y
undump_int(x);
if x<>@"57324D46 then goto off_base; {not a base file}
undump_int(x);
if (x<0) or (x>256) then goto off_base; {corrupted base file}
base_engine:=xmalloc_array(text_char, x);
undump_things(base_engine[0], x);
base_engine[x-1]:=0; {force string termination, just in case}
if strcmp(engine_name, stringcast(base_engine)) then
  begin wake_up_terminal;
  wterm_ln('---! ', stringcast(name_of_file+1), ' was written by ', stringcast(base_engine));
  libc_free(base_engine);
  goto off_base;
end;
libc_free(base_engine);
undump_int(x);
if x<>@$ then begin {check that strings are the same}
  wake_up_terminal;
  wterm_ln('---! ', stringcast(name_of_file+1),
           ' made by different executable version');
  goto off_base;
end;
@<Undump |xord|, |xchr|, and |xprn|@>;
undump_int(x);
if x<>mem_min then goto off_base;
{Now we deal with dynamically allocating the memory. We don't provide
 all the fancy features \.{tex.ch} does---all that matters is enough to
 run the trap test with a memory size of 3000.}
@+init
if ini_version then begin
  {We allocated this at start-up, but now we need to reallocate.}
  libc_free (mem);
end;
@+tini
undump_int (mem_top); {Overwrite whatever we had.}
if mem_max < mem_top then mem_max:=mem_top; {Use at least what we dumped.}
if mem_min+1100>mem_top then goto off_base;
mem:=xmalloc_array (memory_word, mem_max - mem_min + 1);
@z

@x [48.1199] l.22750 - Allow command line to override dumped value.
undump(batch_mode)(error_stop_mode)(interaction);
@y
undump(batch_mode)(error_stop_mode)(interaction);
if interaction_option<>unspecified_mode then interaction:=interaction_option;
@z

@x [48.1199] l.22755 - Test for end-of-file already done by undump.
undump_int(x);@+if (x<>69069)or eof(base_file) then goto off_base
@y
undump_int(x);@+if x<>69069 then goto off_base
@z

@x [49.1204] Dynamic allocation.
@p begin @!{|start_here|}
@y
@d const_chk(#) == begin if # < inf@&# then # := inf@&# else
                         if # > sup@&# then # := sup@&# end
{|setup_bound_var| stuff duplicated in \.{tex.ch}.}
@d setup_bound_var(#) == bound_default := #; setup_bound_var_end
@d setup_bound_var_end(#) == bound_name := #; setup_bound_var_end_end
@d setup_bound_var_end_end(#) ==
  setup_bound_variable (address_of (#), bound_name, bound_default)

@p begin @!{|start_here|}
  {See comments in \.{tex.ch} for why the name has to be duplicated.}
  setup_bound_var (250000)('main_memory')(main_memory);
    {|memory_word|s for |mem| in \.{INIMF}}
  setup_bound_var (3000)('buf_size')(buf_size);
  setup_bound_var (79)('error_line')(error_line);
  setup_bound_var (50)('half_error_line')(half_error_line);
  setup_bound_var (79)('max_print_line')(max_print_line);
  setup_bound_var (768)('screen_width')(screen_width);
  setup_bound_var (1024)('screen_depth')(screen_depth);
  setup_bound_var (16384)('gf_buf_size')(gf_buf_size);
  if error_line > ssup_error_line then error_line := ssup_error_line;
  if screen_width > ssup_screen_width then screen_width := ssup_screen_width;
  if screen_depth > ssup_screen_depth then screen_depth := ssup_screen_depth;

  const_chk (main_memory);
  {|mem_top| is an index, |main_memory| is a size}
  mem_top := mem_min + main_memory - 1;
  mem_max := mem_top;
  const_chk (buf_size);

  buffer:=xmalloc_array (ASCII_code, buf_size);
  row_transition:=xmalloc_array (screen_col, screen_width);
  gf_buf:=xmalloc_array (eight_bits, gf_buf_size);
  source_filename_stack:=xmalloc_array (str_number, max_in_open);
  full_source_filename_stack:=xmalloc_array (str_number, max_in_open);

@+init
if ini_version then begin
  mem:=xmalloc_array (memory_word, mem_top - mem_min + 1);
end;
@+tini
@z

@x [49.1204] Only do get_strings_started if ini.
@!init if not get_strings_started then goto final_end;
init_tab; {initialize the tables}
init_prim; {call |primitive| for each primitive}
init_str_ptr:=str_ptr; init_pool_ptr:=pool_ptr;@/
max_str_ptr:=str_ptr; max_pool_ptr:=pool_ptr; fix_date_and_time;
tini@/
@y  22833
@!init
if ini_version then begin
if not get_strings_started then goto final_end;
init_tab; {initialize the tables}
init_prim; {call |primitive| for each primitive}
init_str_ptr:=str_ptr; init_pool_ptr:=pool_ptr;@/
max_str_ptr:=str_ptr; max_pool_ptr:=pool_ptr; fix_date_and_time;
end;
tini@/
@z

@x [49.1204]
end_of_MF: close_files_and_terminate;
final_end: ready_already:=0;
@y
close_files_and_terminate;
final_end: do_final_end;
@z

% [49.1205] close_files_and_terminate: Print new line before
% termination; switch to editor if necessary.
@x
    slow_print(log_name); print_char(".");
    end;
  end;
@y
    print_file_name(0,log_name,0); print_char(".");
    end;
  end;
print_ln;
if (edit_name_start<>0) and (interaction>batch_mode) then
    call_edit(str_pool,edit_name_start,edit_name_length,edit_line);
@z

@x [49.1209] Only do dump if ini.
  begin @!init store_base_file; return;@+tini@/
@y
  begin
    @!init if ini_version then begin store_base_file; return;end;@+tini@/
@z

%@x [49.1211] l.23002 - Handle %&base line.
%if (base_ident=0)or(buffer[loc]="&") then
%@y
%if (base_ident=0)or(buffer[loc]="&")or dump_line then
%@z

@x [51.1214] Add editor-switch variable to globals.
This section should be replaced, if necessary, by any special
modifications of the program
that are necessary to make \MF\ work at a particular installation.
It is usually best to design your change file so that all changes to
previous sections preserve the section numbering; then everybody's version
will be consistent with the published program. More extensive changes,
which introduce new sections, can be inserted here; then only the index
itself will get a new section number.
@^system dependencies@>
@y
Here are the variables used to hold ``switch-to-editor'' information.
@^system dependencies@>

@<Global...@>=
@!edit_name_start: pool_pointer;
@!edit_name_length,@!edit_line: integer;
@!xprn: array[ASCII_code] of ASCII_code; {use \.{\^\^} notation?}
@!stop_at_space: boolean; {whether |more_name| returns false for space}

@ The |edit_name_start| will be set to point into |str_pool| somewhere after
its beginning if \MF\ is supposed to switch to an editor on exit.

@<Set init...@>=
edit_name_start:=0;
stop_at_space:=true;

@ Dumping the |xord|, |xchr|, and |xprn| arrays.  We dump these always
in the format, so a TCX file loaded during format creation can set a
default for users of the format.

@<Dump |xord|, |xchr|, and |xprn|@>=
dump_things(xord[0], 256);
dump_things(xchr[0], 256);
dump_things(xprn[0], 256);

@ Undumping the |xord|, |xchr|, and |xprn| arrays.  This code is more
complicated, because we want to ensure that a TCX file specified on
the command line will override whatever is in the format.  Since the
tcx file has already been loaded, that implies throwing away the data
in the format.  Also, if no |translate_filename| is given, but
|eight_bit_p| is set we have to make all characters printable.

@<Undump |xord|, |xchr|, and |xprn|@>=
if translate_filename then begin
  for k:=0 to 255 do undump_things(dummy_xord, 1);
  for k:=0 to 255 do undump_things(dummy_xchr, 1);
  for k:=0 to 255 do undump_things(dummy_xprn, 1);
  end
else begin
  undump_things(xord[0], 256);
  undump_things(xchr[0], 256);
  undump_things(xprn[0], 256);
  if eight_bit_p then
    for k:=0 to 255 do
      xprn[k]:=1;
end;
@z
