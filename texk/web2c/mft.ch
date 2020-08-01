% mft.ch for C compilation with web2c.
%
% From Pierre Mackay's version for pc, which was in turn based on Howard
% Trickey's and Pavel Curtis's change file for weave.
% Support for MP included from a changefile by Wlodek Bzyl.
% This file is in the public domain.

@x [0] l.16 - WEAVE: use logo font to get the S
\font\logo=manfnt % font used for the METAFONT logo
@y
\font\logo=logo10 % font used for the METAFONT logo
\def\MP{{\logo META}\-{\logo POST}}
@z

@x [0] WEAVE: print changes only.
\pageno=\contentspagenumber \advance\pageno by 1
@y
\pageno=\contentspagenumber \advance\pageno by 1
\let\maybe=\iffalse
\def\title{MFT changes for C and METAPOST}
@z

@x [1]
This program converts a \MF\ source file to a \TeX\ file. It was written
@y
This program converts a \MF\ or \MP\ source file to a \TeX\ file.
It was written
@z
@x [1]
@^Knuth, Donald Ervin@>
been developed in January, 1980.
@y
@^Knuth, Donald Ervin@>
been developed in January, 1980.
@^Bzyl, Wlodek@>
Changes for \MP\ by W.~Bzyl in July, 2001.
@z

@x [2] Define my_name
@d banner=='This is MFT, Version 2.0'
@y
@d my_name=='mft'
@d banner=='This is MFT, Version 2.0'
@z

% [3] No need for the final label in C.
% AIX defines `class' in <math.h>, so let's take this opportunity to
% define that away.
@x
calls the `|jump_out|' procedure, which goes to the label |end_of_MFT|.

@d end_of_MFT = 9999 {go here to wrap it up}
@y
calls the `|jump_out|' procedure.

@d class == class_var
@z

@x [3] No global labels.
label end_of_MFT; {go here to finish}
@y
@z

@x [3]
procedure initialize;
  var @<Local variables for initialization@>@/
  begin @<Set initial values@>@/
@y
@<Define |parse_arguments|@>
procedure initialize;
  var @<Local variables for initialization@>@/
begin
  kpse_set_program_name (argv[0], my_name);
  parse_arguments;
  @<Set initial values@>;
@z

@x [8] Increase constants.
@!max_bytes=10000; {the number of bytes in tokens; must be less than 65536}
@!max_names=1000; {number of tokens}
@y
@!max_bytes=60000; {the number of bytes in tokens; must be less than 65536}
@!max_names=6000; {number of tokens}
@z
@x
@!buf_size=100; {maximum length of input line}
@!line_length=80; {lines of \TeX\ output have at most this many characters,
@y
@!buf_size=3000; {maximum length of input line}
@!line_length=79; {lines of \TeX\ output have at most this many characters,
@z

% [13] The text_char type is used as an array index into xord.  The
% default type `char' produces signed integers, which are bad array
% indices in C.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d text_char == char {the data type of characters in text files}
@y
@d text_char == ASCII_code {the data type of characters in text files}
@z

@x [17] Allow any input character.
for i:=0 to @'37 do xchr[i]:=' ';
for i:=@'177 to @'377 do xchr[i]:=' ';
@y
for i:=1 to @'37 do xchr[i]:=chr(i);
for i:=@'177 to @'377 do xchr[i]:=chr(i);
@z

@x [20] Terminal I/O.
@d print(#)==write(term_out,#) {`|print|' means write on the terminal}
@y
@d term_out==stdout
@d print(#)==write(term_out,#) {`|print|' means write on the terminal}
@z

% [20] Remove term_out.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@<Globals...@>=
@!term_out:text_file; {the terminal as an output file}
@y
@z

@x [21] Don't initialize the terminal.
@ Different systems have different ways of specifying that the output on a
certain file will appear on the user's terminal. Here is one way to do this
on the \PASCAL\ system that was used in \.{WEAVE}'s initial development:
@^system dependencies@>

@<Set init...@>=
rewrite(term_out,'TTY:'); {send |term_out| output to the terminal}
@y
@ Different systems have different ways of specifying that the output on a
certain file will appear on the user's terminal.
@^system dependencies@>

@<Set init...@>=
{nothing need be done}
@z

@x [22] `break' is `fflush'.
@d update_terminal == break(term_out) {empty the terminal output buffer}
@y
@d update_terminal == fflush(term_out) {empty the terminal output buffer}
@z

@x [24] Open input files.
@ The following code opens the input files.  Since these files were listed
in the program header, we assume that the \PASCAL\ runtime system has
already checked that suitable file names have been given; therefore no
additional error checking needs to be done.
@^system dependencies@>

@p procedure open_input; {prepare to read the inputs}
begin reset(mf_file); reset(change_file); reset(style_file);
end;
@y
@ The following code opens the input files.
@^system dependencies@>

@p procedure open_input; {prepare to read inputs}
begin
  if metapost then
    mf_file := kpse_open_file (cmdline (optind), kpse_mp_format)
  else
    mf_file := kpse_open_file (cmdline (optind), kpse_mf_format);
  if change_name then begin
    if metapost then
      change_file := kpse_open_file (change_name, kpse_mp_format)
    else
      change_file := kpse_open_file (change_name, kpse_mf_format);
  end;
  style_file := kpse_open_file (style_name[0], kpse_mft_format);
  i_style_name := 1;
end;
@z

@x [26] Opening the .tex output file.
rewrite(tex_file);
@y
rewrite (tex_file, tex_name);
@z

@x [28] Fix f^.
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

@x [31] Fix jump_out.
@ The |jump_out| procedure just cuts across all active procedure levels
and jumps out of the program. This is the only non-local \&{goto} statement
in \.{MFT}. It is used when no recovery from a particular error has
been provided.

Some \PASCAL\ compilers do not implement non-local |goto| statements.
@^system dependencies@>
In such cases the code that appears at label |end_of_MFT| should be
copied into the |jump_out| procedure, followed by a call to a system procedure
that terminates the program.

@d fatal_error(#)==begin new_line; print(#); error; mark_fatal; jump_out;
  end

@<Error handling...@>=
procedure jump_out;
begin goto end_of_MFT;
end;
@y
@ The |jump_out| procedure cleans up, prints appropriate messages,
and exits back to the operating system.

@d fatal_error(#)==begin new_line; print(#); error; mark_fatal; jump_out;
	end

@<Error handling...@>=
procedure jump_out;
begin
{here files should be closed if the operating system requires it}
  @<Print the job |history|@>;
  new_line;
  if (history <> spotless) and (history <> harmless_message) then
    uexit(1)
  else
    uexit(0);
end;
@z

@x [47] -- allow for multiple style files
@ @<Read from |style_file|...@>=
begin incr(line);
if not input_ln(style_file) then
  begin styling:=false; line:=0;
  end;
end
@y
@ @<Read from |style_file|...@>=
begin incr(line);
if not input_ln(style_file) then begin
  if i_style_name <> n_style_name then begin
    xfclose(style_file,style_name[i_style_name-1]);
    style_file := kpse_open_file (style_name[i_style_name], kpse_mft_format);
    i_style_name := i_style_name + 1;
  end
  else begin
    styling:=false;
  end;
  line:=0;
  end;
end
@z

% MFT incorrectly handles TeX code contained in groups:
%
%   btex <TeX material> etex
%   verbatimtex <TeX material> etex
%
% Ulrik Vieth (using ideas of Andreas Scherer) developed a Unix
% shell script which tries to correct errors made by MFT when
% prettyprinting TeX material.
% His script converts TeX material to a `TeXnical MFT comment',
% which means that it is extracted and prettyprinted as string.
% Unfortunately, his script assumes that these groups may not
% extend across line breaks and there may be only one such
% group per line, which is sometimes hard to obey and
% makes prettyprinted code harder to read.
% Moreover, any synonym for btex/verbatimtex/etex would not be
% recognized. So, if we introduce a change in MFT's formatting
% rules with
%   %%% btex my_btex
% i.e. we instruct MFT to format token `my_btex' according the
% current convention for `btex' token,
% then this new convention would not be obeyed. It is so,
% because the script has hardcoded btex/verbatimtex/etex names.

@x [63]
@d input_command=31 {internal code for tokens like `\.{input}'}
@d special_tag=32 {internal code for tags that take at most one subscript}
@d tag=33 {internal code for nonprimitive tokens}
@y
@d input_command=31 {internal code for tokens like `\.{input}'}
@d btex_code=32 {begin \TeX\ material (\.{btex})}
@d verbatim_code=33 {begin \TeX\ material (\.{verbatimtex})}
@d etex_marker=34 {end \TeX\ material (\.{etex})}
@d special_tag=35 {internal code for tags that take at most one subscript}
@d tag=36 {internal code for nonprimitive tokens}
@z

% Now follows the simplest change -- although the bigest one.
% The primitives of \MF\ and \MP\ are reshuffled into
% three categories:
%   -- primitives common for \MF\ and \MP
%   -- primitives specific for \MF
%   -- primitives specific for \MP

@x [65]
@ The intended use of the macros above might not be immediately obvious,
but the riddle is answered by the following:

@<Store all the primitives@>=
id_loc:=18;@/
pr2(".")(".")(path_join);@/
pr1("[")(as_is);@/
pr1("]")(as_is);@/
pr1("}")(as_is);@/
pr1("{")(as_is);@/
pr1(":")(colon);@/
pr2(":")(":")(colon);@/
pr3("|")("|")(":")(colon);@/
pr2(":")("=")(as_is);@/
pr1(",")(as_is);@/
pr1(";")(semicolon);@/
pr1("\")(backslash);@/
pr2("\")("\")(double_back);@/
pr5("a")("d")("d")("t")("o")(command);@/
pr2("a")("t")(bbinary);@/
pr7("a")("t")("l")("e")("a")("s")("t")(op);@/
pr10("b")("e")("g")("i")("n")("g")("r")("o")("u")("p")(command);
pr8("c")("o")("n")("t")("r")("o")("l")("s")(op);@/
pr4("c")("u")("l")("l")(command);@/
pr4("c")("u")("r")("l")(op);@/
pr10("d")("e")("l")("i")("m")("i")("t")("e")("r")("s")(command);@/
pr7("d")("i")("s")("p")("l")("a")("y")(command);@/
pr8("e")("n")("d")("g")("r")("o")("u")("p")(endit);@/
pr8("e")("v")("e")("r")("y")("j")("o")("b")(command);@/
pr6("e")("x")("i")("t")("i")("f")(command);@/
pr11("e")("x")("p")("a")("n")("d")("a")("f")("t")("e")("r")(command);@/
pr4("f")("r")("o")("m")(bbinary);@/
pr8("i")("n")("w")("i")("n")("d")("o")("w")(bbinary);@/
pr7("i")("n")("t")("e")("r")("i")("m")(command);@/
pr3("l")("e")("t")(command);@/
pr11("n")("e")("w")("i")("n")("t")("e")("r")("n")("a")("l")(command);@/
pr2("o")("f")(command);@/
pr10("o")("p")("e")("n")("w")("i")("n")("d")("o")("w")(command);@/
pr10("r")("a")("n")("d")("o")("m")("s")("e")("e")("d")(command);@/
pr4("s")("a")("v")("e")(command);@/
pr10("s")("c")("a")("n")("t")("o")("k")("e")("n")("s")(command);@/
pr7("s")("h")("i")("p")("o")("u")("t")(command);@/
pr4("s")("t")("e")("p")(bbinary);@/
pr3("s")("t")("r")(command);@/
pr7("t")("e")("n")("s")("i")("o")("n")(op);@/
pr2("t")("o")(bbinary);@/
pr5("u")("n")("t")("i")("l")(bbinary);@/
pr3("d")("e")("f")(command);@/
pr6("v")("a")("r")("d")("e")("f")(command);@/

@ (There are so many primitives, it's necessary to break this long
initialization code up into pieces so as not to overflow \.{WEAVE}'s capacity.)

@<Store all the primitives@>=
pr10("p")("r")("i")("m")("a")("r")("y")("d")("e")("f")(command);@/
pr12("s")("e")("c")("o")("n")("d")("a")("r")("y")("d")("e")("f")(command);@/
pr11("t")("e")("r")("t")("i")("a")("r")("y")("d")("e")("f")(command);@/
pr6("e")("n")("d")("d")("e")("f")(endit);@/
pr3("f")("o")("r")(command);@/
pr11("f")("o")("r")("s")("u")("f")("f")("i")("x")("e")("s")(command);@/
pr7("f")("o")("r")("e")("v")("e")("r")(command);@/
pr6("e")("n")("d")("f")("o")("r")(endit);@/
pr5("q")("u")("o")("t")("e")(command);@/
pr4("e")("x")("p")("r")(command);@/
pr6("s")("u")("f")("f")("i")("x")(command);@/
pr4("t")("e")("x")("t")(command);@/
pr7("p")("r")("i")("m")("a")("r")("y")(command);@/
pr9("s")("e")("c")("o")("n")("d")("a")("r")("y")(command);@/
pr8("t")("e")("r")("t")("i")("a")("r")("y")(command);@/
pr5("i")("n")("p")("u")("t")(input_command);@/
pr8("e")("n")("d")("i")("n")("p")("u")("t")(bold);@/
pr2("i")("f")(command);@/
pr2("f")("i")(endit);@/
pr4("e")("l")("s")("e")(command);@/
pr6("e")("l")("s")("e")("i")("f")(command);@/
pr4("t")("r")("u")("e")(bold);@/
pr5("f")("a")("l")("s")("e")(bold);@/
pr11("n")("u")("l")("l")("p")("i")("c")("t")("u")("r")("e")(bold);@/
pr7("n")("u")("l")("l")("p")("e")("n")(bold);@/
pr7("j")("o")("b")("n")("a")("m")("e")(bold);@/
pr10("r")("e")("a")("d")("s")("t")("r")("i")("n")("g")(bold);@/
pr9("p")("e")("n")("c")("i")("r")("c")("l")("e")(bold);@/
pr4("g")("o")("o")("d")(special_tag);@/
pr2("=")(":")(as_is);@/
pr3("=")(":")("|")(as_is);@/
pr4("=")(":")("|")(">")(as_is);@/
pr3("|")("=")(":")(as_is);@/
pr4("|")("=")(":")(">")(as_is);@/
pr4("|")("=")(":")("|")(as_is);@/
pr5("|")("=")(":")("|")(">")(as_is);@/
pr6("|")("=")(":")("|")(">")(">")(as_is);@/
pr4("k")("e")("r")("n")(binary);
pr6("s")("k")("i")("p")("t")("o")(command);@/

@ (Does anybody out there remember the commercials that went \.{LS-MFT}?)

@<Store all the prim...@>=
pr13("n")("o")("r")("m")("a")("l")("d")("e")("v")("i")("a")("t")("e")(op);@/
pr3("o")("d")("d")(op);@/
pr5("k")("n")("o")("w")("n")(op);@/
pr7("u")("n")("k")("n")("o")("w")("n")(op);@/
pr3("n")("o")("t")(op);@/
pr7("d")("e")("c")("i")("m")("a")("l")(op);@/
pr7("r")("e")("v")("e")("r")("s")("e")(op);@/
pr8("m")("a")("k")("e")("p")("a")("t")("h")(op);@/
pr7("m")("a")("k")("e")("p")("e")("n")(op);@/
pr11("t")("o")("t")("a")("l")("w")("e")("i")("g")("h")("t")(op);@/
pr3("o")("c")("t")(op);@/
pr3("h")("e")("x")(op);@/
pr5("A")("S")("C")("I")("I")(op);@/
pr4("c")("h")("a")("r")(op);@/
pr6("l")("e")("n")("g")("t")("h")(op);@/
pr13("t")("u")("r")("n")("i")("n")("g")("n")("u")("m")("b")("e")("r")(op);@/
pr5("x")("p")("a")("r")("t")(op);@/
pr5("y")("p")("a")("r")("t")(op);@/
pr6("x")("x")("p")("a")("r")("t")(op);@/
pr6("x")("y")("p")("a")("r")("t")(op);@/
pr6("y")("x")("p")("a")("r")("t")(op);@/
pr6("y")("y")("p")("a")("r")("t")(op);@/
pr4("s")("q")("r")("t")(op);@/
pr4("m")("e")("x")("p")(op);@/
pr4("m")("l")("o")("g")(op);@/
pr4("s")("i")("n")("d")(op);@/
pr4("c")("o")("s")("d")(op);@/
pr5("f")("l")("o")("o")("r")(op);@/
pr14("u")("n")("i")("f")("o")("r")("m")("d")("e")("v")("i")("a")("t")("e")(op);
  @/
pr10("c")("h")("a")("r")("e")("x")("i")("s")("t")("s")(op);@/
pr5("a")("n")("g")("l")("e")(op);@/
pr5("c")("y")("c")("l")("e")(op);@/

@ (If you think this \.{WEB} code is ugly, you should see the Pascal code
it produces.)

@<Store all the primitives@>=
pr13("t")("r")("a")("c")("i")("n")("g")
 ("t")("i")("t")("l")("e")("s")(internal);@/
pr16("t")("r")("a")("c")("i")("n")("g")
 ("e")("q")("u")("a")("t")("i")("o")("n")("s")(internal);@/
pr15("t")("r")("a")("c")("i")("n")("g")
 ("c")("a")("p")("s")("u")("l")("e")("s")(internal);@/
pr14("t")("r")("a")("c")("i")("n")("g")
 ("c")("h")("o")("i")("c")("e")("s")(internal);@/
pr12("t")("r")("a")("c")("i")("n")("g")
 ("s")("p")("e")("c")("s")(internal);@/
pr11("t")("r")("a")("c")("i")("n")("g")
 ("p")("e")("n")("s")(internal);@/
pr15("t")("r")("a")("c")("i")("n")("g")
 ("c")("o")("m")("m")("a")("n")("d")("s")(internal);@/
pr13("t")("r")("a")("c")("i")("n")("g")
 ("m")("a")("c")("r")("o")("s")(internal);@/
pr12("t")("r")("a")("c")("i")("n")("g")
 ("e")("d")("g")("e")("s")(internal);@/
pr13("t")("r")("a")("c")("i")("n")("g")
 ("o")("u")("t")("p")("u")("t")(internal);@/
pr12("t")("r")("a")("c")("i")("n")("g")
 ("s")("t")("a")("t")("s")(internal);@/
pr13("t")("r")("a")("c")("i")("n")("g")
 ("o")("n")("l")("i")("n")("e")(internal);@/

@ @<Store all the primitives@>=
pr4("y")("e")("a")("r")(internal);@/
pr5("m")("o")("n")("t")("h")(internal);@/
pr3("d")("a")("y")(internal);@/
pr4("t")("i")("m")("e")(internal);@/
pr8("c")("h")("a")("r")("c")("o")("d")("e")(internal);@/
pr7("c")("h")("a")("r")("f")("a")("m")(internal);@/
pr6("c")("h")("a")("r")("w")("d")(internal);@/
pr6("c")("h")("a")("r")("h")("t")(internal);@/
pr6("c")("h")("a")("r")("d")("p")(internal);@/
pr6("c")("h")("a")("r")("i")("c")(internal);@/
pr6("c")("h")("a")("r")("d")("x")(internal);@/
pr6("c")("h")("a")("r")("d")("y")(internal);@/
pr10("d")("e")("s")("i")("g")("n")("s")("i")("z")("e")(internal);@/
pr4("h")("p")("p")("p")(internal);@/
pr4("v")("p")("p")("p")(internal);@/
pr7("x")("o")("f")("f")("s")("e")("t")(internal);@/
pr7("y")("o")("f")("f")("s")("e")("t")(internal);@/
pr7("p")("a")("u")("s")("i")("n")("g")(internal);@/
pr12("s")("h")("o")("w")
 ("s")("t")("o")("p")("p")("i")("n")("g")(internal);@/
pr10("f")("o")("n")("t")("m")("a")("k")("i")("n")("g")(internal);@/
pr8("p")("r")("o")("o")("f")("i")("n")("g")(internal);@/
pr9("s")("m")("o")("o")("t")("h")("i")("n")("g")(internal);@/
pr12("a")("u")("t")("o")("r")("o")("u")("n")("d")("i")("n")("g")(internal);@/
pr11("g")("r")("a")("n")("u")("l")("a")("r")("i")("t")("y")(internal);@/
pr6("f")("i")("l")("l")("i")("n")(internal);@/
pr12("t")("u")("r")("n")("i")("n")("g")("c")("h")("e")("c")("k")(internal);@/
pr12("w")("a")("r")("n")("i")("n")("g")("c")("h")("e")("c")("k")(internal);@/
pr12("b")("o")("u")("n")("d")("a")("r")("y")("c")("h")("a")("r")(internal);@/

@ Still more.

@<Store all the prim...@>=
pr1("+")(abinary);@/
pr1("-")(abinary);@/
pr1("*")(abinary);@/
pr1("/")(as_is);@/
pr2("+")("+")(binary);@/
pr3("+")("-")("+")(pyth_sub);@/
pr3("a")("n")("d")(binary);@/
pr2("o")("r")(binary);@/
pr1("<")(as_is);@/
pr2("<")("=")(less_or_equal);@/
pr1(">")(as_is);@/
pr2(">")("=")(greater_or_equal);@/
pr1("=")(as_is);@/
pr2("<")(">")(not_equal);@/
pr9("s")("u")("b")("s")("t")("r")("i")("n")("g")(command);@/
pr7("s")("u")("b")("p")("a")("t")("h")(command);@/
pr13("d")("i")("r")("e")("c")("t")("i")("o")("n")@|
 ("t")("i")("m")("e")(command);@/
pr5("p")("o")("i")("n")("t")(command);@/
pr10("p")("r")("e")("c")("o")("n")("t")("r")("o")("l")(command);@/
pr11("p")("o")("s")("t")("c")("o")("n")("t")("r")("o")("l")(command);@/
pr9("p")("e")("n")("o")("f")("f")("s")("e")("t")(command);@/
pr1("&")(ampersand);@/
pr7("r")("o")("t")("a")("t")("e")("d")(binary);@/
pr7("s")("l")("a")("n")("t")("e")("d")(binary);@/
pr6("s")("c")("a")("l")("e")("d")(binary);@/
pr7("s")("h")("i")("f")("t")("e")("d")(binary);@/
pr11("t")("r")("a")("n")("s")("f")("o")("r")("m")("e")("d")(binary);@/
pr7("x")("s")("c")("a")("l")("e")("d")(binary);@/
pr7("y")("s")("c")("a")("l")("e")("d")(binary);@/
pr7("z")("s")("c")("a")("l")("e")("d")(binary);@/
pr17("i")("n")("t")("e")("r")("s")("e")("c")("t")("i")("o")("n")@|
 ("t")("i")("m")("e")("s")(binary);@/
pr7("n")("u")("m")("e")("r")("i")("c")(type_name);@/
pr6("s")("t")("r")("i")("n")("g")(type_name);@/
pr7("b")("o")("o")("l")("e")("a")("n")(type_name);@/
pr4("p")("a")("t")("h")(type_name);@/
pr3("p")("e")("n")(type_name);@/
pr7("p")("i")("c")("t")("u")("r")("e")(type_name);@/
pr9("t")("r")("a")("n")("s")("f")("o")("r")("m")(type_name);@/
pr4("p")("a")("i")("r")(type_name);@/

@ At last we are done with the tedious initialization of primitives.

@<Store all the prim...@>=
pr3("e")("n")("d")(endit);@/
pr4("d")("u")("m")("p")(endit);@/
pr9("b")("a")("t")("c")("h")("m")("o")("d")("e")(bold);
pr11("n")("o")("n")("s")("t")("o")("p")("m")("o")("d")("e")(bold);
pr10("s")("c")("r")("o")("l")("l")("m")("o")("d")("e")(bold);
pr13("e")("r")("r")("o")("r")("s")("t")("o")("p")@|
 ("m")("o")("d")("e")(bold);
pr5("i")("n")("n")("e")("r")(command);@/
pr5("o")("u")("t")("e")("r")(command);@/
pr9("s")("h")("o")("w")("t")("o")("k")("e")("n")(command);@/
pr9("s")("h")("o")("w")("s")("t")("a")("t")("s")(bold);@/
pr4("s")("h")("o")("w")(command);@/
pr12("s")("h")("o")("w")("v")("a")("r")("i")("a")("b")("l")("e")(command);@/
pr16("s")("h")("o")("w")@|
 ("d")("e")("p")("e")("n")("d")("e")("n")("c")("i")("e")("s")(bold);@/
pr7("c")("o")("n")("t")("o")("u")("r")(command);@/
pr10("d")("o")("u")("b")("l")("e")("p")("a")("t")("h")(command);@/
pr4("a")("l")("s")("o")(command);@/
pr7("w")("i")("t")("h")("p")("e")("n")(command);@/
pr10("w")("i")("t")("h")("w")("e")("i")("g")("h")("t")(command);@/
pr8("d")("r")("o")("p")("p")("i")("n")("g")(command);@/
pr7("k")("e")("e")("p")("i")("n")("g")(command);@/
pr7("m")("e")("s")("s")("a")("g")("e")(command);@/
pr10("e")("r")("r")("m")("e")("s")("s")("a")("g")("e")(command);@/
pr7("e")("r")("r")("h")("e")("l")("p")(command);@/
pr8("c")("h")("a")("r")("l")("i")("s")("t")(command);@/
pr8("l")("i")("g")("t")("a")("b")("l")("e")(command);@/
pr10("e")("x")("t")("e")("n")("s")("i")("b")("l")("e")(command);@/
pr10("h")("e")("a")("d")("e")("r")("b")("y")("t")("e")(command);@/
pr9("f")("o")("n")("t")("d")("i")("m")("e")("n")(command);@/
pr7("s")("p")("e")("c")("i")("a")("l")(command);@/
pr10("n")("u")("m")("s")("p")("e")("c")("i")("a")("l")(command);@/
pr1("%")(comment);@/
pr2("%")("%")(verbatim);@/
pr3("%")("%")("%")(set_format);@/
pr4("%")("%")("%")("%")(mft_comment);@/
pr1("#")(sharp);@/
@y
@ We begin with primitives common to \MF\ and \MP.

The intended use of the macros above might not be immediately obvious,
but the riddle is answered by the following:

@<Store all the primitives@>=
id_loc:=18;@/
pr2(".")(".")(path_join);@/
pr1("[")(as_is);@/
pr1("]")(as_is);@/
pr1("}")(as_is);@/
pr1("{")(as_is);@/
pr1(":")(colon);@/
pr2(":")(":")(colon);@/
pr3("|")("|")(":")(colon);@/
pr2(":")("=")(as_is);@/
pr1(",")(as_is);@/
pr1(";")(semicolon);@/
pr1("\")(backslash);@/
pr2("\")("\")(double_back);@/
pr5("a")("d")("d")("t")("o")(command);@/
pr2("a")("t")(bbinary);@/
pr7("a")("t")("l")("e")("a")("s")("t")(op);@/
pr10("b")("e")("g")("i")("n")("g")("r")("o")("u")("p")(command);
pr8("c")("o")("n")("t")("r")("o")("l")("s")(op);@/
pr4("c")("u")("l")("l")(command);@/
pr4("c")("u")("r")("l")(op);@/
pr10("d")("e")("l")("i")("m")("i")("t")("e")("r")("s")(command);@/
pr8("e")("n")("d")("g")("r")("o")("u")("p")(endit);@/
pr8("e")("v")("e")("r")("y")("j")("o")("b")(command);@/
pr6("e")("x")("i")("t")("i")("f")(command);@/
pr11("e")("x")("p")("a")("n")("d")("a")("f")("t")("e")("r")(command);@/
pr4("f")("r")("o")("m")(bbinary);@/
pr7("i")("n")("t")("e")("r")("i")("m")(command);@/
pr3("l")("e")("t")(command);@/
pr11("n")("e")("w")("i")("n")("t")("e")("r")("n")("a")("l")(command);@/
pr2("o")("f")(command);@/
pr10("r")("a")("n")("d")("o")("m")("s")("e")("e")("d")(command);@/
pr4("s")("a")("v")("e")(command);@/
pr10("s")("c")("a")("n")("t")("o")("k")("e")("n")("s")(command);@/
pr7("s")("h")("i")("p")("o")("u")("t")(command);@/
pr4("s")("t")("e")("p")(bbinary);@/
pr3("s")("t")("r")(command);@/
pr7("t")("e")("n")("s")("i")("o")("n")(op);@/
pr2("t")("o")(bbinary);@/
pr5("u")("n")("t")("i")("l")(bbinary);@/
pr3("d")("e")("f")(command);@/
pr6("v")("a")("r")("d")("e")("f")(command);@/

@ (There are so many primitives, it's necessary to break this long
initialization code up into pieces so as not to overflow \.{WEAVE}'s capacity.)

@<Store all the primitives@>=
pr10("p")("r")("i")("m")("a")("r")("y")("d")("e")("f")(command);@/
pr12("s")("e")("c")("o")("n")("d")("a")("r")("y")("d")("e")("f")(command);@/
pr11("t")("e")("r")("t")("i")("a")("r")("y")("d")("e")("f")(command);@/
pr6("e")("n")("d")("d")("e")("f")(endit);@/
pr3("f")("o")("r")(command);@/
pr11("f")("o")("r")("s")("u")("f")("f")("i")("x")("e")("s")(command);@/
pr7("f")("o")("r")("e")("v")("e")("r")(command);@/
pr6("e")("n")("d")("f")("o")("r")(endit);@/
pr5("q")("u")("o")("t")("e")(command);@/
pr4("e")("x")("p")("r")(command);@/
pr6("s")("u")("f")("f")("i")("x")(command);@/
pr4("t")("e")("x")("t")(command);@/
pr7("p")("r")("i")("m")("a")("r")("y")(command);@/
pr9("s")("e")("c")("o")("n")("d")("a")("r")("y")(command);@/
pr8("t")("e")("r")("t")("i")("a")("r")("y")(command);@/
pr5("i")("n")("p")("u")("t")(input_command);@/
pr8("e")("n")("d")("i")("n")("p")("u")("t")(bold);@/
pr2("i")("f")(command);@/
pr2("f")("i")(endit);@/
pr4("e")("l")("s")("e")(command);@/
pr6("e")("l")("s")("e")("i")("f")(command);@/
pr4("t")("r")("u")("e")(bold);@/
pr5("f")("a")("l")("s")("e")(bold);@/
pr11("n")("u")("l")("l")("p")("i")("c")("t")("u")("r")("e")(bold);@/
pr7("n")("u")("l")("l")("p")("e")("n")(bold);@/
pr7("j")("o")("b")("n")("a")("m")("e")(bold);@/
pr10("r")("e")("a")("d")("s")("t")("r")("i")("n")("g")(bold);@/
pr9("p")("e")("n")("c")("i")("r")("c")("l")("e")(bold);@/
pr2("=")(":")(as_is);@/
pr3("=")(":")("|")(as_is);@/
pr4("=")(":")("|")(">")(as_is);@/
pr3("|")("=")(":")(as_is);@/
pr4("|")("=")(":")(">")(as_is);@/
pr4("|")("=")(":")("|")(as_is);@/
pr5("|")("=")(":")("|")(">")(as_is);@/
pr6("|")("=")(":")("|")(">")(">")(as_is);@/
pr4("k")("e")("r")("n")(binary);
pr6("s")("k")("i")("p")("t")("o")(command);@/

@ (Does anybody out there remember the commercials that went \.{LS-MFT}?)

@<Store all the prim...@>=
pr13("n")("o")("r")("m")("a")("l")("d")("e")("v")("i")("a")("t")("e")(op);@/
pr3("o")("d")("d")(op);@/
pr5("k")("n")("o")("w")("n")(op);@/
pr7("u")("n")("k")("n")("o")("w")("n")(op);@/
pr3("n")("o")("t")(op);@/
pr7("d")("e")("c")("i")("m")("a")("l")(op);@/
pr7("r")("e")("v")("e")("r")("s")("e")(op);@/
pr8("m")("a")("k")("e")("p")("a")("t")("h")(op);@/
pr7("m")("a")("k")("e")("p")("e")("n")(op);@/
pr3("o")("c")("t")(op);@/
pr3("h")("e")("x")(op);@/
pr5("A")("S")("C")("I")("I")(op);@/
pr4("c")("h")("a")("r")(op);@/
pr6("l")("e")("n")("g")("t")("h")(op);@/
pr13("t")("u")("r")("n")("i")("n")("g")("n")("u")("m")("b")("e")("r")(op);@/
pr5("x")("p")("a")("r")("t")(op);@/
pr5("y")("p")("a")("r")("t")(op);@/
pr6("x")("x")("p")("a")("r")("t")(op);@/
pr6("x")("y")("p")("a")("r")("t")(op);@/
pr6("y")("x")("p")("a")("r")("t")(op);@/
pr6("y")("y")("p")("a")("r")("t")(op);@/
pr4("s")("q")("r")("t")(op);@/
pr4("m")("e")("x")("p")(op);@/
pr4("m")("l")("o")("g")(op);@/
pr4("s")("i")("n")("d")(op);@/
pr4("c")("o")("s")("d")(op);@/
pr5("f")("l")("o")("o")("r")(op);@/
pr14("u")("n")("i")("f")("o")("r")("m")("d")("e")("v")("i")("a")("t")("e")(op);
  @/
pr10("c")("h")("a")("r")("e")("x")("i")("s")("t")("s")(op);@/
pr5("a")("n")("g")("l")("e")(op);@/
pr5("c")("y")("c")("l")("e")(op);@/

@ (If you think this \.{WEB} code is ugly, you should see the Pascal code
it produces.)

@<Store all the primitives@>=
pr13("t")("r")("a")("c")("i")("n")("g")
 ("t")("i")("t")("l")("e")("s")(internal);@/
pr16("t")("r")("a")("c")("i")("n")("g")
 ("e")("q")("u")("a")("t")("i")("o")("n")("s")(internal);@/
pr15("t")("r")("a")("c")("i")("n")("g")
 ("c")("a")("p")("s")("u")("l")("e")("s")(internal);@/
pr14("t")("r")("a")("c")("i")("n")("g")
 ("c")("h")("o")("i")("c")("e")("s")(internal);@/
pr12("t")("r")("a")("c")("i")("n")("g")
 ("s")("p")("e")("c")("s")(internal);@/
pr11("t")("r")("a")("c")("i")("n")("g")
 ("p")("e")("n")("s")(internal);@/
pr15("t")("r")("a")("c")("i")("n")("g")
 ("c")("o")("m")("m")("a")("n")("d")("s")(internal);@/
pr13("t")("r")("a")("c")("i")("n")("g")
 ("m")("a")("c")("r")("o")("s")(internal);@/
pr13("t")("r")("a")("c")("i")("n")("g")
 ("o")("u")("t")("p")("u")("t")(internal);@/
pr12("t")("r")("a")("c")("i")("n")("g")
 ("s")("t")("a")("t")("s")(internal);@/
pr13("t")("r")("a")("c")("i")("n")("g")
 ("o")("n")("l")("i")("n")("e")(internal);@/

@ @<Store all the primitives@>=
pr4("y")("e")("a")("r")(internal);@/
pr5("m")("o")("n")("t")("h")(internal);@/
pr3("d")("a")("y")(internal);@/
pr4("t")("i")("m")("e")(internal);@/
pr8("c")("h")("a")("r")("c")("o")("d")("e")(internal);@/
pr6("c")("h")("a")("r")("w")("d")(internal);@/
pr6("c")("h")("a")("r")("h")("t")(internal);@/
pr6("c")("h")("a")("r")("d")("p")(internal);@/
pr6("c")("h")("a")("r")("i")("c")(internal);@/
pr6("c")("h")("a")("r")("d")("x")(internal);@/
pr6("c")("h")("a")("r")("d")("y")(internal);@/
pr10("d")("e")("s")("i")("g")("n")("s")("i")("z")("e")(internal);@/
pr7("x")("o")("f")("f")("s")("e")("t")(internal);@/
pr7("y")("o")("f")("f")("s")("e")("t")(internal);@/
pr7("p")("a")("u")("s")("i")("n")("g")(internal);@/
pr12("s")("h")("o")("w")
 ("s")("t")("o")("p")("p")("i")("n")("g")(internal);@/
pr10("f")("o")("n")("t")("m")("a")("k")("i")("n")("g")(internal);@/
pr8("p")("r")("o")("o")("f")("i")("n")("g")(internal);@/
pr12("t")("u")("r")("n")("i")("n")("g")("c")("h")("e")("c")("k")(internal);@/
pr12("w")("a")("r")("n")("i")("n")("g")("c")("h")("e")("c")("k")(internal);@/
pr12("b")("o")("u")("n")("d")("a")("r")("y")("c")("h")("a")("r")(internal);@/

@ Still more.

@<Store all the prim...@>=
pr1("+")(abinary);@/
pr1("-")(abinary);@/
pr1("*")(abinary);@/
pr1("/")(as_is);@/
pr2("+")("+")(binary);@/
pr3("+")("-")("+")(pyth_sub);@/
pr3("a")("n")("d")(binary);@/
pr2("o")("r")(binary);@/
pr1("<")(as_is);@/
pr2("<")("=")(less_or_equal);@/
pr1(">")(as_is);@/
pr2(">")("=")(greater_or_equal);@/
pr1("=")(as_is);@/
pr2("<")(">")(not_equal);@/
pr9("s")("u")("b")("s")("t")("r")("i")("n")("g")(command);@/
pr7("s")("u")("b")("p")("a")("t")("h")(command);@/
pr13("d")("i")("r")("e")("c")("t")("i")("o")("n")@|
 ("t")("i")("m")("e")(command);@/
pr5("p")("o")("i")("n")("t")(command);@/
pr10("p")("r")("e")("c")("o")("n")("t")("r")("o")("l")(command);@/
pr11("p")("o")("s")("t")("c")("o")("n")("t")("r")("o")("l")(command);@/
pr9("p")("e")("n")("o")("f")("f")("s")("e")("t")(command);@/
pr1("&")(ampersand);@/
pr7("r")("o")("t")("a")("t")("e")("d")(binary);@/
pr7("s")("l")("a")("n")("t")("e")("d")(binary);@/
pr6("s")("c")("a")("l")("e")("d")(binary);@/
pr7("s")("h")("i")("f")("t")("e")("d")(binary);@/
pr11("t")("r")("a")("n")("s")("f")("o")("r")("m")("e")("d")(binary);@/
pr7("x")("s")("c")("a")("l")("e")("d")(binary);@/
pr7("y")("s")("c")("a")("l")("e")("d")(binary);@/
pr7("z")("s")("c")("a")("l")("e")("d")(binary);@/
pr17("i")("n")("t")("e")("r")("s")("e")("c")("t")("i")("o")("n")@|
 ("t")("i")("m")("e")("s")(binary);@/
pr7("n")("u")("m")("e")("r")("i")("c")(type_name);@/
pr6("s")("t")("r")("i")("n")("g")(type_name);@/
pr7("b")("o")("o")("l")("e")("a")("n")(type_name);@/
pr4("p")("a")("t")("h")(type_name);@/
pr3("p")("e")("n")(type_name);@/
pr7("p")("i")("c")("t")("u")("r")("e")(type_name);@/
pr9("t")("r")("a")("n")("s")("f")("o")("r")("m")(type_name);@/
pr4("p")("a")("i")("r")(type_name);@/

@ At last we are done with the tedious initialization of primitives.

@<Store all the prim...@>=
pr3("e")("n")("d")(endit);@/
pr4("d")("u")("m")("p")(endit);@/
pr9("b")("a")("t")("c")("h")("m")("o")("d")("e")(bold);
pr11("n")("o")("n")("s")("t")("o")("p")("m")("o")("d")("e")(bold);
pr10("s")("c")("r")("o")("l")("l")("m")("o")("d")("e")(bold);
pr13("e")("r")("r")("o")("r")("s")("t")("o")("p")@|
 ("m")("o")("d")("e")(bold);
pr5("i")("n")("n")("e")("r")(command);@/
pr5("o")("u")("t")("e")("r")(command);@/
pr9("s")("h")("o")("w")("t")("o")("k")("e")("n")(command);@/
pr9("s")("h")("o")("w")("s")("t")("a")("t")("s")(bold);@/
pr4("s")("h")("o")("w")(command);@/
pr12("s")("h")("o")("w")("v")("a")("r")("i")("a")("b")("l")("e")(command);@/
pr16("s")("h")("o")("w")@|
 ("d")("e")("p")("e")("n")("d")("e")("n")("c")("i")("e")("s")(bold);@/
pr7("c")("o")("n")("t")("o")("u")("r")(command);@/
pr10("d")("o")("u")("b")("l")("e")("p")("a")("t")("h")(command);@/
pr4("a")("l")("s")("o")(command);@/
pr7("w")("i")("t")("h")("p")("e")("n")(command);@/
pr7("m")("e")("s")("s")("a")("g")("e")(command);@/
pr10("e")("r")("r")("m")("e")("s")("s")("a")("g")("e")(command);@/
pr7("e")("r")("r")("h")("e")("l")("p")(command);@/
pr8("c")("h")("a")("r")("l")("i")("s")("t")(command);@/
pr8("l")("i")("g")("t")("a")("b")("l")("e")(command);@/
pr10("e")("x")("t")("e")("n")("s")("i")("b")("l")("e")(command);@/
pr10("h")("e")("a")("d")("e")("r")("b")("y")("t")("e")(command);@/
pr9("f")("o")("n")("t")("d")("i")("m")("e")("n")(command);@/
pr7("s")("p")("e")("c")("i")("a")("l")(command);@/
pr1("%")(comment);@/
pr2("%")("%")(verbatim);@/
pr3("%")("%")("%")(set_format);@/
pr4("%")("%")("%")("%")(mft_comment);@/
pr1("#")(sharp);@/
pr4("g")("o")("o")("d")(special_tag);@/
@z

% The code below follows ideas from the section 646 of `mp.web'.
@x [75]
two global variables, |cur_type| and |cur_tok|.
@y
two global variables, |cur_type| and |cur_tok|.

The \&{btex}$\,\ldots\,$\&{etex} and \&{verbatimtex}$\,\ldots\,$\&{etex}
features need to be implemented at a low level in the scanning
process. This is implemented by changing the behavior of the
scanner via |scanner_status| global variable.

@d normal=0 {|scanner_status| at ``quiet times''}
@d verbatimtex_flushing=1 {|scanner_status| when moving verbatim \TeX\ material}
@d btex_flushing=2 {|scanner_status| when moving \TeX\ code}
@z

@x [75]
@!prev_tok:integer; {previous value of |cur_tok|}
@y
@!prev_tok:integer; {previous value of |cur_tok|}
@!scanner_status:normal..btex_flushing; {are we scanning at high speed?}
@z

@x [79] Allow any 8 bit character in input.
for i:=0 to " "-1 do char_class[i]:=invalid_class;
char_class[carriage_return]:=end_line_class;@/
for i:=127 to 255 do char_class[i]:=invalid_class;
@y
for i:=0 to " "-1 do char_class[i]:=letter_class;
for i:=127 to 255 do char_class[i]:=letter_class;
char_class[carriage_return]:=end_line_class;
char_class[@'11]:=space_class; {tab}
char_class[@'14]:=space_class; {form feed}
@z

% When dealing with a TeX material we must copy spaces to output.
@x [81]
space_class:if start_of_line then emit(indentation)
  else goto switch;
@y
space_class:if start_of_line or scanner_status>normal then emit(indentation)
  else goto switch;
@z

% Set the default set of macros for \MP.
@x [88]
so that the first line of the output file will be `\.{\\input mftmac}'.
@.\\input mftmac@>
@.mftmac@>

@<Set init...@>=
out_ptr:=1; out_buf[1]:=" "; out_line:=1; write(tex_file,'\input mftmac');
@y
so that the first line of the output file will be `\.{\\input mftmac}'
or `\.{\\input mptmac}' if a \MP\ file is converted.
@.\\input mftmac@>
@.mftmac@>
@.\\input mptmac@>
@.mptmac@>
@<Set init...@>=
out_ptr:=1; out_buf[1]:=" "; out_line:=1;
if metapost then begin write(tex_file,'\input mptmac'); end
else write(tex_file,'\input mftmac');
@z

@x [97]
  string_token:@<Translate a string token@>;
@y
  string_token:@<Translate a string token@>;
  verbatim_code,btex_code:@<Copy \TeX\ material@>;
@z

% [112] Print newline at end of run, exit based upon value of history,
% and remove the end_of_MFT label.
@x
print_ln(banner); {print a ``banner line''}
@y
print (banner); {print a ``banner line''}
print_ln (version_string);
@z

@x
end_of_MFT:{here files should be closed if the operating system requires it}
@<Print the job |history|@>;
end.
@y
@<Print the job |history|@>;
new_line;
if (history <> spotless) and (history <> harmless_message)
then uexit (1)
else uexit (0);
end.
@z

@x [114] System-dependent changes.
This module should be replaced, if necessary, by changes to the program
that are necessary to make \.{MFT} work at a particular installation.
It is usually best to design your change file so that all changes to
previous modules preserve the module numbering; then everybody's version
will be consistent with the printed program. More extensive changes,
which introduce new modules, can be inserted here; then only the index
itself will get a new module number.
@^system dependencies@>
@y
The user calls \.{MFT} with arguments on the command line.  These are
either filenames or flags (beginning with `\.-').  The following globals
are for communicating the user's desires to the rest of the program. The
various |name| variables contain strings with the full names of those
files, as UNIX knows them.

@d max_style_name = 32

@<Globals...@>=
@!change_name,@!tex_name:const_c_string;
@!style_name:array[0..max_style_name-1] of const_c_string;
@!n_style_name:c_int_type;  {Number of values in |style_name| array.}
@!i_style_name:c_int_type;  {The next |style_name|.}
@!metapost:c_int_type;  {|true| for \MP, |false| for \MF}

@ Look at the command line arguments and set the |name| variables accordingly.

At least one file name must be present as the first argument: the \.{mf}
file.  It may have an extension, or it may omit it to get |'.mf'| added.
If there is only one file name, the output file name is formed by
replacing the \.{mf} file name extension by |'.tex'|.  Thus, the command
line \.{mf foo} implies the use of the \MF\ input file \.{foo.mf}
and the output file \.{foo.tex}.  If this style of command line, with
only one argument, is used, the default style file, |plain.mft|, will be
used to provide basic formatting.

@d argument_is (#) == (strcmp (long_options[option_index].name, #) = 0)

@<Define |parse_arguments|@> =
procedure parse_arguments;
const n_options = 5; {Pascal won't count array lengths for us.}
var @!long_options: array[0..n_options] of getopt_struct;
    @!getopt_return_val: integer;
    @!option_index: c_int_type;
    @!current_option: 0..n_options;
    @!suffix: const_c_string;
begin
  @<Initialize the option variables@>;
  @<Define the option table@>;
  n_style_name := 0;
  repeat
    getopt_return_val := getopt_long_only (argc, argv, '', long_options,
                                           address_of (option_index));
    if getopt_return_val = -1 then begin
      do_nothing; {End of arguments; we exit the loop below.}

    end else if getopt_return_val = "?" then begin
      usage (my_name);

    end else if argument_is ('help') then begin
      usage_help (MFT_HELP, nil);

    end else if argument_is ('version') then begin
      print_version_and_exit (banner, nil,
      			      'D.E. Knuth  (MP changes by W. Bzyl)',
			      nil);

    end else if argument_is ('change') then begin
      change_name := extend_filename (optarg, 'ch');

    end else if argument_is ('style') then begin
      if (n_style_name = max_style_name) then begin
	fatal_error ('Too many style files specified.');
        usage (my_name);
      end;
      style_name[n_style_name] := extend_filename (optarg, 'mft');
      n_style_name := n_style_name + 1;

    end; {Else it was a flag; |getopt| has already done the assignment.}
  until getopt_return_val = -1;

  {Now |optind| is the index of first non-option on the command line.
   We must have exactly one remaining argument.}
  if (optind + 1 <> argc) then begin
    write_ln (stderr, my_name, ': Need exactly one file argument.');
    usage (my_name);
  end;

  suffix := find_suffix (cmdline (optind));
  if suffix and (strcmp (suffix, 'mp') = 0) then begin
    metapost := true;
    tex_name := basename_change_suffix (cmdline (optind), '.mp', '.tex');
  end else begin
    tex_name := basename_change_suffix (cmdline (optind), '.mf', '.tex');
  end;

  if (n_style_name = 0) then begin
    if metapost then
      style_name[0] := 'mplain.mft'
    else
      style_name[0] := 'plain.mft';
    n_style_name := 1;
  end;
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

@ Here is the option to set a change file.
@.-change@>

@<Define the option...@> =
long_options[current_option].name := 'change';
long_options[current_option].has_arg := 1;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;
incr (current_option);

@ Here is the option to set the style file.
@.-style@>

@<Define the option...@> =
long_options[current_option].name := 'style';
long_options[current_option].has_arg := 1;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;
incr (current_option);

@ The option to set a \MP\ file processing.

@<Define the option...@> =
long_options[current_option].name := 'metapost';
long_options[current_option].has_arg := 0;
long_options[current_option].flag := address_of (metapost);
long_options[current_option].val := 1;
incr (current_option);

@ |metapost| defaults to |false|; will become |true| for \MP.

@<Initialize the option...@> =
metapost:=false;

@ An element with all zeros always ends the list of options.

@<Define the option...@> =
long_options[current_option].name := 0;
long_options[current_option].has_arg := 0;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;

@ Store primitives specific for \MF.

@<Store all the prim...@>=
if not metapost then begin
  pr12("a")("u")("t")("o")("r")("o")("u")("n")("d")("i")("n")("g")(internal);@/
  pr7("c")("h")("a")("r")("f")("a")("m")(internal);@/
  pr8("d")("r")("o")("p")("p")("i")("n")("g")(command);@/
  pr7("d")("i")("s")("p")("l")("a")("y")(command);@/
  pr6("f")("i")("l")("l")("i")("n")(internal);@/
  pr11("g")("r")("a")("n")("u")("l")("a")("r")("i")("t")("y")(internal);@/
  pr8("i")("n")("w")("i")("n")("d")("o")("w")(bbinary);@/
  pr4("h")("p")("p")("p")(internal);@/
  pr7("k")("e")("e")("p")("i")("n")("g")(command);@/
  pr10("n")("u")("m")("s")("p")("e")("c")("i")("a")("l")(command);@/
  pr10("o")("p")("e")("n")("w")("i")("n")("d")("o")("w")(command);@/
  pr9("s")("m")("o")("o")("t")("h")("i")("n")("g")(internal);@/
  pr4("v")("p")("p")("p")(internal);@/
  pr11("t")("o")("t")("a")("l")("w")("e")("i")("g")("h")("t")(op);@/
  pr12("t")("r")("a")("c")("i")("n")("g")("e")("d")("g")("e")("s")(internal);@/
  pr10("w")("i")("t")("h")("w")("e")("i")("g")("h")("t")(command);@/
end;

@ Store primitives specific for \MP.

@<Store all the prim...@>=
if metapost then begin
  pr9("a")("r")("c")("l")("e")("n")("g")("t")("h")(op);@/
  pr7("a")("r")("c")("t")("i")("m")("e")(command);@/
  pr8("b")("l")("u")("e")("p")("a")("r")("t")(op);@/
  pr7("b")("o")("u")("n")("d")("e")("d")(op);@/
  pr4("b")("t")("e")("x")(btex_code);@/
  pr4("c")("l")("i")("p")(command);@/
  pr7("c")("l")("i")("p")("p")("e")("d")(op);@/
  pr9("c")("l")("o")("s")("e")("f")("r")("o")("m")(input_command);@/
  pr5("c")("o")("l")("o")("r")(type_name);@/
  pr6("d")("a")("s")("h")("e")("d")(command);@/
  pr8("d")("a")("s")("h")("p")("a")("r")("t")(op);@/
  pr4("e")("t")("e")("x")(etex_marker);@/
  pr6("f")("i")("l")("l")("e")("d")(op);@/
  pr8("f")("o")("n")("t")("p")("a")("r")("t")(op);@/
  pr8("f")("o")("n")("t")("s")("i")("z")("e")(op);@/
  pr9("g")("r")("e")("e")("n")("p")("a")("r")("t")(op);@/
  pr6("i")("n")("f")("o")("n")("t")(binary);@/
  pr8("l")("i")("n")("e")("j")("o")("i")("n")(internal);@/
  pr7("l")("i")("n")("e")("c")("a")("p")(internal);@/
  pr8("l")("l")("c")("o")("r")("n")("e")("r")(op);@/
  pr8("l")("r")("c")("o")("r")("n")("e")("r")(op);@/
  pr10("m")("i")("t")("e")("r")("l")("i")("m")("i")("t")(internal);@/
  pr8("m")("p")("x")("b")("r")("e")("a")("k")(bold);@/
  pr8("p")("a")("t")("h")("p")("a")("r")("t")(op);@/
  pr7("p")("e")("n")("p")("a")("r")("t")(op);@/
  pr9("p")("r")("o")("l")("o")("g")("u")("e")("s")(internal);@/
  pr7("r")("e")("d")("p")("a")("r")("t")(op);@/
  pr8("r")("e")("a")("d")("f")("r")("o")("m")(input_command);@/
  pr9("s")("e")("t")("b")("o")("u")("n")("d")("s")(command);@/
  pr7("s")("t")("r")("o")("k")("e")("d")(op);@/
  pr8("t")("e")("x")("t")("p")("a")("r")("t")(op);@/
  pr7("t")("e")("x")("t")("u")("a")("l")(op);@/
  pr16("t")("r")("a")("c")("i")("n")("g")
    ("l")("o")("s")("t")("c")("h")("a")("r")("s")(internal);@/
  pr11("t")("r")("u")("e")("c")("o")("r")("n")("e")("r")("s")(internal);@/
  pr8("u")("l")("c")("o")("r")("n")("e")("r")(op);@/
  pr8("u")("r")("c")("o")("r")("n")("e")("r")(op);@/
  pr11("v")("e")("r")("b")("a")("t")("i")("m")("t")("e")("x")(verbatim_code);@/
  pr6("w")("i")("t")("h")("i")("n")(bbinary);@/
  pr9("w")("i")("t")("h")("c")("o")("l")("o")("r")(command);@/
  pr5("w")("r")("i")("t")("e")(command);@/
end;

@ Here an extra section is added.
`\.{btex}' token is translated to `\.{\\mftbeginB}' and
`\.{verbatimtex}' to `\.{\\mftbeginV}'.
`\.{etex}' is translated to `\.{\\mftend}'.
These \TeX\ macros are defined in \.{mptmac.tex}.
@.mptmac@>
@<Copy \TeX\ material@>=
  begin out4("\")("m")("f")("t"); out5("b")("e")("g")("i")("n");
    if cur_type=verbatim_code then begin
      out("V");
      scanner_status:=verbatimtex_flushing;
    end else if cur_type=btex_code then begin
      out("B");
      scanner_status:=btex_flushing;
    end;
    out("{"); out_name(cur_tok); out("}");
    get_next;
    while cur_type<>etex_marker do begin
      if cur_type=indentation then begin out(" "); end
      else if cur_type=end_of_line then begin
        flush_buffer(out_ptr,false);
        empty_buffer:=true;
      end
      else copy(id_first);
      get_next;
    end;
    out4("\")("m")("f")("t"); out3("e")("n")("d");
    out("{"); out_name(cur_tok); out("}");
    if scanner_status=verbatimtex_flushing then out("$");
    scanner_status:=normal;
  end
@z
