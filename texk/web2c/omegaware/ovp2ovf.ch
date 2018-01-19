%
% This file is part of the Omega project, which
% is based in the web2c distribution of TeX.
%
% Copyright (c) 1994--1998 John Plaice and Yannis Haralambous
% applies only to the changes to the original vptovf.ch.
%
% vptovf.ch for C compilation with web2c.

@x [0] WEAVE: print changes only.
\pageno=\contentspagenumber \advance\pageno by 1
@y
\pageno=\contentspagenumber \advance\pageno by 1
%\let\maybe=\iffalse
%\def\title{OVP2OVF changes for C}
@z

@x [1] Define my_name
@d banner=='This is OVP2OVF, Version 1.13' {printed when the program starts}
@y
@d my_name=='ovp2ovf'
@d banner=='This is OVP2OVF, Version 1.13' {printed when the program starts}
@z

@x [2] Print all terminal output on stderr.
@d print(#)==write(#)
@d print_ln(#)==write_ln(#)
@y
@d print(#)==write(stderr,#)
@d print_ln(#)==write_ln(stderr,#)
@d print_real(#)==fprint_real(stderr,#)
@z

@x [2] Print the banner later.
procedure initialize; {this procedure gets things started properly}
  var @<Local variables for initialization@>@/
  begin print_ln(banner);@/
@y
@<Define |parse_arguments|@>
procedure initialize; {this procedure gets things started properly}
  var @<Local variables for initialization@>@/
  begin kpse_set_program_name (argv[0], my_name);
  parse_arguments;
@z

@x [3] Increase constants.
@!buf_size=60; {length of lines displayed in error messages}
@y
@!buf_size=3000; {length of lines displayed in error messages}
@z

@x [6] Open VPL file.
reset(vpl_file);
@y
reset (vpl_file, vpl_name);
if verbose then begin
  print (banner);
  print_ln (version_string);
end;
@z

@x [22] Open output files.
@ On some systems you may have to do something special to write a
packed file of bytes. For example, the following code didn't work
when it was first tried at Stanford, because packed files have to be
opened with a special switch setting on the \PASCAL\ that was used.
@^system dependencies@>

@<Set init...@>=
rewrite(vf_file); rewrite(tfm_file);
@y
@ On some systems you may have to do something special to write a
packed file of bytes.
@^system dependencies@>

@<Set init...@>=
rewritebin (vf_file, vf_name);
rewritebin (tfm_file, tfm_name);
@z

@x [23] Avoid name conflict; MinGW defines `byte' in <rpcndr.h>.
correspond to one-character constants like \.{"A"} in \.{WEB} language.
@y
correspond to one-character constants like \.{"A"} in \.{WEB} language.

@d byte == byte_type
@z

@x [24] Pascal Web's char
@d first_ord=0 {ordinal number of the smallest element of |char|}
@y
@d char == 0..255
@d first_ord=0 {ordinal number of the smallest element of |char|}
@z

@x [31] Non-zero return code in case of problems.
@!chars_on_line:0..9; {the number of characters printed on the current line}
@y
@!chars_on_line:0..9; {the number of characters printed on the current line}
@!perfect:boolean; {was the file free of errors?}
@z

@x [32] Non-zero return code in case of problems.
chars_on_line:=0;
@y
chars_on_line:=0;
perfect:=true; {innocent until proved guilty}
@z

@x [33] Non-zero return code in case of problems.
chars_on_line:=0;
@y
chars_on_line:=0;
perfect:=false;
@z

@x [37] (get_keyword_char) Unnecessary due to previous change.
begin while (loc=limit)and(not right_ln) do fill_buffer;
if loc=limit then cur_char:=" " {end-of-line counts as a delimiter}
else  begin cur_char:=xord[buffer[loc+1]];
@y
begin while loc=limit do fill_buffer;
  begin cur_char:=xord[buffer[loc+1]];
@z

% [89] `index' is not a good choice for an identifier on Unix systems.
% Neither is `class', on AIX.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
|k|th element of its list.
@y
|k|th element of its list.

@d index == index_var
@d class == class_var
@z

@x [130] No output (except errors) unless verbose.
@<Print |c| in hex notation@>;
@y
if verbose then @<Print |c| in hex notation@>;
@z

@x [133] No output (except errors) unless verbose.
@<Print |c| in hex notation@>;
@y
if verbose then @<Print |c| in hex notation@>;
@z
@x
print('-'); print_hex(c+crange); incr(chars_on_line);
@y [133]
if verbose then begin
  print('-'); print_hex(c+crange); incr(chars_on_line);
  end;
@z

@x [140]
@!HEX: packed array [1..16] of char;
@y
@!HEX: const_c_string;
@z

@x [141]
HEX:='0123456789ABCDEF';@/
@y
HEX:=' 0123456789ABCDEF';@/
@z

@x [144] Output of real numbers.
@ @d round_message(#)==if delta>0 then print_ln('I had to round some ',
@.I had to round...@>
  #,'s by ',(((delta+1) div 2)/@'4000000):1:7,' units.')
@y
@ @d round_message(#)==if delta>0 then begin print('I had to round some ',
@.I had to round...@>
  #,'s by '); print_real((((delta+1) div 2)/@'4000000),1,7);
  print_ln(' units.'); end
@z

@x [152] Fix up the mutually recursive procedures a la pltotf.
@p function f(@!h,@!x,@!y:indx):indx; forward;@t\2@>
  {compute $f$ for arguments known to be in |hash[h]|}
@y
@p
ifdef('notdef')
function f(@!h,@!x,@!y:indx):indx; begin end;@t\2@>
  {compute $f$ for arguments known to be in |hash[h]|}
endif('notdef')
@z

@x [153] Finish fixing up f.
@p function f;
@y
@p function f(@!h,@!x,@!y:indx):indx;
@z

@x [156] Change TFM-byte output to fix ranges.
@d out(#)==write(tfm_file,#)
@y
@d out(#)==putbyte(#,tfm_file)
@z

@x [???] Eliminate nonlocal goto, uexit with a bad exit code.
  goto final_end;
@y
  uexit(1);
@z

@x [165] Fix output of reals.
@p procedure out_scaled(x:fix_word); {outputs a scaled |fix_word|}
var @!n:byte; {the first byte after the sign}
@!m:0..65535; {the two least significant bytes}
begin if abs(x/design_units)>=16.0 then
  begin print_ln('The relative dimension ',x/@'4000000:1:3,
    ' is too large.');
@.The relative dimension...@>
  print('  (Must be less than 16*designsize');
  if design_units<>unity then print(' =',design_units/@'200000:1:3,
      ' designunits');
@y
@p procedure out_scaled(x:fix_word); {outputs a scaled |fix_word|}
var @!n:byte; {the first byte after the sign}
@!m:0..65535; {the two least significant bytes}
begin if fabs(x/design_units)>=16.0 then
  begin print('The relative dimension ');
    print_real(x/@'4000000,1,3);
    print_ln(' is too large.');
@.The relative dimension...@>
  print('  (Must be less than 16*designsize');
  if design_units<>unity then begin print(' =');
	print_real(design_units/@'200000,1,3);
	print(' designunits');
  end;
@z

@x [200] Change VF-byte output to fix ranges.
@d vout(#)==write(vf_file,#)
@y
@d vout(#)==putbyte(#,vf_file)
@z

@x [205] Eliminate unused variables.
var @!krn_ptr:0..max_kerns; {an index into |kern|}
@!c:byte; {runs through all character codes}
@y
@z

@x [206] Be quiet unless verbose.
read_input; print_ln('.');@/
@y
read_input;
if verbose then print_ln('.');
@z

@x [206] Non-zero return code in case of problems,
end.
@y
if not perfect then uexit(1);
end.
@z

@x [287] System-dependent changes.
This section should be replaced, if necessary, by changes to the program
that are necessary to make \.{VPtoVF} work at a particular installation.
It is usually best to design your change file so that all changes to
previous sections preserve the section numbering; then everybody's version
will be consistent with the printed program. More extensive changes,
which introduce new sections, can be inserted here; then only the index
itself will get a new section number.
@^system dependencies@>
@y
Parse a Unix-style command line.

@d argument_is (#) == (strcmp (long_options[option_index].name, #) = 0)

@<Define |parse_arguments|@> =
procedure parse_arguments;
const n_options = 3; {Pascal won't count array lengths for us.}
var @!long_options: array[0..n_options] of getopt_struct;
    @!getopt_return_val: integer;
    @!option_index: c_int_type;
    @!current_option: 0..n_options;
begin
  @<Initialize the option variables@>;
  @<Define the option table@>;
  repeat
    getopt_return_val := getopt_long_only (argc, argv, '', long_options,
                                           address_of (option_index));
    if getopt_return_val = -1 then begin
      {End of arguments; we exit the loop below.} ;

    end else if getopt_return_val = "?" then begin
      usage (my_name); {|getopt| has already given an error message.}

    end else if argument_is ('help') then begin
      usage_help (OVP2OVF_HELP, nil);

    end else if argument_is ('version') then begin
      print_version_and_exit
        (banner, nil, 'J. Plaice, Y. Haralambous, D.E. Knuth', nil);

    end; {Else it was a flag; |getopt| has already done the assignment.}
  until getopt_return_val = -1;

  {Now |optind| is the index of first non-option on the command line.
   We must have one to three remaining arguments.}
  if (optind + 1 <> argc) and (optind + 2 <> argc)
     and (optind + 3 <> argc) then begin
    write_ln (stderr, my_name, ': Need one to three file arguments.');
    usage (my_name);
  end;

  vpl_name := extend_filename (cmdline (optind), 'ovp');

  if optind + 2 <= argc then begin
    {Specified one or both of the output files.}
    vf_name := extend_filename (cmdline (optind + 1), 'ovf');
    if optind + 3 <= argc then begin {Both.}
      tfm_name := extend_filename (cmdline (optind + 2), 'ofm');
    end else begin {Just one.}
      tfm_name := make_suffix (cmdline (optind + 1), 'ofm');
    end;
  end else begin {Neither.}
    vf_name := basename_change_suffix (vpl_name, '.ovp', '.ovf');
    tfm_name := basename_change_suffix (vpl_name, '.ovp', '.ofm');
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

@ Print progress information?
@.-verbose@>

@<Define the option...@> =
long_options[current_option].name := 'verbose';
long_options[current_option].has_arg := 0;
long_options[current_option].flag := address_of (verbose);
long_options[current_option].val := 1;
incr (current_option);

@ The global variable |verbose| determines whether or not we print
progress information.

@<Glob...@> =
@!verbose: c_int_type;

@ It starts off |false|.

@<Initialize the option...@> =
verbose := false;

@ An element with all zeros always ends the list.

@<Define the option...@> =
long_options[current_option].name := 0;
long_options[current_option].has_arg := 0;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;

@ Global filenames.

@<Global...@> =
@!vpl_name, @!tfm_name, @!vf_name:const_c_string;
@z
