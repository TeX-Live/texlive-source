% $Id: dvitomp.ch,v 1.7 2005/04/28 06:45:21 taco Exp $
% dvitomp.ch for C compilation with web2c.  Public domain.
%
%   Change file for the DVItoMP processor, for use with WEB to C
%   This file was created by John Hobby.  It is loosely based on the
%   change file for the WEB to C version of dvitype (due to Howard
%   Trickey and Pavel Curtis).
%
%   3/11/90  (JDH) Original version.
%   4/30/90  (JDH) Update to handle virtual fonts
%   4/16/93  (JDH) Make output go to standard output and require mpx file
%                  to be a command line argument.
%
%   1/18/95  (UV)  Update based on dvitype.ch for web2c-6.1
%   4/13/95  (UV)  Cosmetic changes for release of web2c-mp
%  10/08/95  (UV)  Bug fix: need to replace abs() with floating-point arg 
%                  by fabs() because of different definition in cpascal.h
%                  as reported by Dane Dwyer <dwyer@geisel.csl.uiuc.edu>.

@x [0] WEAVE: print changes only.
\pageno=\contentspagenumber \advance\pageno by 1
@y
\pageno=\contentspagenumber \advance\pageno by 1
\let\maybe=\iffalse
\def\title{DVI$\,$\lowercase{to}MP changes for C}
@z

@x [1] Duplicate banner line for use in |print_version_and_exit|.
@d banner=='% Written by DVItoMP, Version 0.992'
  {the first line of the output file}
@y
@d banner=='% Written by DVItoMP, Version 0.992/color'
  {the first line of the output file}
@d term_banner=='This is DVItoMP, Version 0.992/color'
  {the same in the usual format, as it would be shown on a terminal}
@z

@x [3] Set up kpathsea.
procedure initialize; {this procedure gets things started properly}
  var i:integer; {loop index for initializations}
  begin @<Set initial values@>@/
@y
@<Define |parse_arguments|@>
procedure initialize; {this procedure gets things started properly}
  var i:integer; {loop index for initializations}
  begin
    kpse_set_progname (argv[0]); {initialize for the filename searches}
    parse_arguments;
    @<Set initial values@>@/
@z

@x [5] Increase parameter(s).
@!virtual_space=10000;
@y
@!virtual_space=100000;
@z

@x [7] Remove non-local goto.
@d abort(#)==begin err_print_ln('DVItoMP abort: ',#);
    history:=fatal_error; jump_out;
    end
@d bad_dvi(#)==abort('Bad DVI file: ',#,'!')
@.Bad DVI file@>
@d warn(#)==begin err_print_ln('DVItoMP warning: ',#);
    history:=warning_given;
    end

@p procedure jump_out;
begin goto final_end;
end;
@y
@d jump_out==uexit(history)
@d abort(#)==begin err_print_ln('DVItoMP abort: ',#);
    history:=fatal_error; jump_out;
    end
@d bad_dvi(#)==abort('Bad DVI file: ',#,'!')
@.Bad DVI file@>
@d warn(#)==begin err_print_ln('DVItoMP warning: ',#);
    history:=warning_given;
    end
@z

@x [11] Permissive input.
@!ASCII_code=" ".."~"; {a subrange of the integers}
@y
@!ASCII_code=0..255; {a subrange of the integers}
@z

% [12] The text_char type is used as an array index into `xord'.  The
% default type `char' produces signed integers, which are bad array
% indices in C.
@x
@d text_char == char {the data type of characters in text files}
@d first_text_char=0 {ordinal number of the smallest element of |text_char|}
@d last_text_char=127 {ordinal number of the largest element of |text_char|}
@y
@d text_char == ASCII_code {the data type of characters in text files}
@d first_text_char=0 {ordinal number of the smallest element of |text_char|}
@d last_text_char=255 {ordinal number of the largest element of |text_char|}
@z

@x [14] Fix up opening the files.
@p procedure open_mpx_file; {prepares to write text on |mpx_file|}
begin rewrite(mpx_file);
end;
@y
@p procedure open_mpx_file; {prepares to write text on |mpx_file|}
begin
   cur_name := extend_filename (mpx_name, 'mpx');
   rewrite (mpx_file, cur_name);
end;
@z

@x [19] More file opening.
@p procedure open_dvi_file; {prepares to read packed bytes in |dvi_file|}
begin reset(dvi_file);
if eof(dvi_file) then abort('DVI file not found');
end;
@#
function open_tfm_file:boolean; {prepares to read packed bytes in |tfm_file|}
begin reset(tfm_file,cur_name);
open_tfm_file:=(not eof(tfm_file));
end;
@#
function open_vf_file:boolean; {prepares to read packed bytes in |vf_file|}
begin reset(vf_file,cur_name);
open_vf_file:=(not eof(vf_file));
end;
@y
@p procedure open_dvi_file; {prepares to read packed bytes in |dvi_file|}
begin 
    cur_name := extend_filename (dvi_name, 'dvi');
    resetbin(dvi_file, cur_name);
end;
@#
function open_tfm_file:boolean; {prepares to read packed bytes in |tfm_file|}
begin 
  tfm_file := kpse_open_file (cur_name, kpse_tfm_format);
  free (cur_name); {We |xmalloc|'d this before we got called.}
  open_tfm_file := true; {If we get here, we succeeded.}
end;
@#
function open_vf_file:boolean; {prepares to read packed bytes in |tfm_file|}
var @!full_name:^char;
begin
  {It's ok if the \.{VF} file doesn't exist.}
  full_name := kpse_find_vf (cur_name);
  if full_name then begin
    resetbin (vf_file, full_name);
    free (cur_name);
    free (full_name);
    open_vf_file := true;
  end else
    open_vf_file := false;
end;
@z

@x [24] No arbitrary limit on filename length.
@!cur_name:packed array[1..name_length] of char; {external name,
  with no lower case letters}
@y
@!cur_name:^char; {external name}
@z

@x [26] Make get_n_bytes routines work with 16-bit math.
get_two_bytes:=a*256+b;
@y
get_two_bytes:=a*intcast(256)+b;
@z
@x
get_three_bytes:=(a*256+b)*256+c;
@y
get_three_bytes:=(a*intcast(256)+b)*256+c;
@z
@x
if a<128 then signed_trio:=(a*256+b)*256+c
else signed_trio:=((a-256)*256+b)*256+c;
@y
if a<128 then signed_trio:=(a*intcast(256)+b)*256+c
else signed_trio:=((a-intcast(256))*256+b)*256+c;
@z
@x
if a<128 then signed_quad:=((a*256+b)*256+c)*256+d
else signed_quad:=(((a-256)*256+b)*256+c)*256+d;
@y
if a<128 then signed_quad:=((a*intcast(256)+b)*256+c)*256+d
else signed_quad:=(((a-256)*intcast(256)+b)*256+c)*256+d;
@z

@x [32] l.672 Bugfix: local_only is indexed by internal font numbers.
@!local_only:array [0..max_fonts] of boolean; {|font_num| meaningless?}
@y
@!local_only:array [0..max_fnums] of boolean; {|font_num| meaningless?}
@z

@x [41] Fix abs() with floating-point arg.
      begin if abs(font_scaled_size[f]-font_scaled_size[ff])
@y
      begin if fabs(font_scaled_size[f]-font_scaled_size[ff])
@z

@x [43] Fix abs() with floating-point arg.
if abs(font_design_size[f]-font_design_size[ff]) > font_tolerance then
@y
if fabs(font_design_size[f]-font_design_size[ff]) > font_tolerance then
@z 

@x [43] Checksum warning: set history to cksum_trouble, not warning_given.
  font_warn('Checksum mismatch for ')(ff)
@.Checksum mismatch@>
@y
  begin err_print('DVItoMP warning: Checksum mismatch for ');
@.Checksum mismatch@>
  err_print_font(ff);
  if history=spotless then history:=cksum_trouble;
  end
@z

@x [46] Make 16-bit TFM calculations work.
read_tfm_word; lh:=b2*256+b3;
read_tfm_word; font_bc[f]:=b0*256+b1; font_ec[f]:=b2*256+b3;
@y
read_tfm_word; lh:=b2*intcast(256)+b3;
read_tfm_word; font_bc[f]:=b0*intcast(256)+b1; font_ec[f]:=b2*intcast(256)+b3;
@z
@x
    if b0<128 then tfm_check_sum:=((b0*256+b1)*256+b2)*256+b3
    else tfm_check_sum:=(((b0-256)*256+b1)*256+b2)*256+b3;
@y
    if b0<128 then tfm_check_sum:=((b0*intcast(256)+b1)*256+b2)*256+b3
    else tfm_check_sum:=(((b0-256)*intcast(256)+b1)*256+b2)*256+b3;
@z

% For the code to work, Hobby notes that we need |round|(|x| - 0.5)
% to be equal to $\lfloor x\rfloor$.  For Pascal's |round| function
% this is emphatically not true.
@x [49] We need floor, not round.
round(dvi_scale*font_scaled_size[f]*char_width(f)(c)-0.5)
@y
floor(dvi_scale*font_scaled_size[f]*char_width(f)(c))
@z

@x [50] We need floor, not round.
round(dvi_scale*font_scaled_size[cur_font]*char_width(cur_font)(p)-0.5)
@y
floor(dvi_scale*font_scaled_size[cur_font]*char_width(cur_font)(p))
@z

% [61] Don't set default_directory_name.
@x
@d default_directory_name=='TeXfonts:' {change this to the correct name}
@d default_directory_name_length=9 {change this to the correct length}

@<Glob...@>=
@!default_directory:packed array[1..default_directory_name_length] of char;
@y
There is no single |default_directory| with C.
@z

@x [62] Remove initialization of default_directory.
@ @<Set init...@>=
default_directory:=default_directory_name;
@y
@ (No initialization needs to be done.  Keep this module to preserve
numbering.)
@z

@x [63] Dynamically allocate cur_name, don't add .vf.
for k:=1 to name_length do cur_name[k]:=' ';
if area_length[f]=0 then
  begin for k:=1 to default_directory_name_length do
    cur_name[k]:=default_directory[k];
  l:=default_directory_name_length;
  end
else l:=0;
for k:=font_name[f] to font_name[f+1]-1 do
  begin incr(l);
  if l+3>name_length then
    abort('DVItoMP capacity exceeded (max font name length=',
      name_length:1,')!');
@.DVItoMP capacity exceeded...@>
  if (names[k]>="a")and(names[k]<="z") then
      cur_name[l]:=xchr[names[k]-@'40]
  else cur_name[l]:=xchr[names[k]];
  end;
cur_name[l+1]:='.'; cur_name[l+2]:='V'; cur_name[l+3]:='F'
@y
{This amounts to a string copy. }
cur_name := xmalloc_array (char, font_name[f+1] - font_name[f]);
for k:=font_name[f] to font_name[f+1]-1 do begin
  cur_name[k - font_name[f]] := xchr[names[k]];
end;
cur_name[font_name[f+1] - font_name[f]] := 0;
@z

@x [64] Since we didn't add .vf, don't need to change it to .tfm.
l:=area_length[f];
if l=0 then l:=default_directory_name_length;
l:=l+font_name[f+1]-font_name[f];
if l+4>name_length then
  abort('DVItoMP capacity exceeded (max font name length=',
    name_length:1,')!');
@.DVItoMP capacity exceeded...@>
cur_name[l+2]:='T'; cur_name[l+3]:='F'; cur_name[l+4]:='M'
@y
do_nothing
@z

@x [75] Handle colored text. (COLOR)
print_ln('vardef _s(expr _t,_f,_m,_x,_y)=');
print_ln('  addto _p also _t infont _f scaled _m shifted (_x,_y); enddef;');
@y
print_ln('vardef _s(expr _t,_f,_m,_x,_y)(text _c)=');
print_ln('  addto _p also _t infont _f scaled _m shifted (_x,_y) _c; enddef;');
@z

@x [78] Fix printing of real numbers, and add color (COLOR).
  if (abs(x)>=4096.0)or(abs(y)>=4096.0)or(m>=4096.0)or(m<0) then
    begin warn('text scaled ',m:1:1,@|
        ' at (',x:1:1,',',y:1:1,') is out of range');
    end_char_string(60);
    end
  else end_char_string(40);
  print_ln(',_n',str_f:1,',',m:1:5,',',x:1:4,',',y:1:4,');');
@y
  if (fabs(x)>=4096.0)or(fabs(y)>=4096.0)or(m>=4096.0)or(m<0) then
    begin warn('text is out of range');
    end_char_string(60);
    end
  else end_char_string(40);
  print(',_n',str_f:1,',');
  fprint_real(mpx_file, m,1,5); print(',');
  fprint_real(mpx_file, x,1,4); print(',');
  fprint_real(mpx_file, y,1,4); print(',');@/
  @<Print a \.{withcolor} specifier if appropriate@>@/
  print_ln(');');
@z

@x [79] Fix _r definition (COLOR).
    print_ln('vardef _r(expr _a,_w) =');
    print_ln('  addto _p doublepath _a withpen pencircle scaled _w enddef;');
@y
    print_ln('vardef _r(expr _a,_w)(text _t) =');
    print_ln('  addto _p doublepath _a withpen pencircle scaled _w _t enddef;');
@z

@x [79] Another fix for printing of real numbers, plus colors (COLOR).
  if (abs(xx1)>=4096.0)or(abs(yy1)>=4096.0)or@|
      (abs(xx2)>=4096.0)or(abs(yy2)>=4096.0)or(ww>=4096.0) then
    warn('hrule or vrule near (',xx1:1:1,',',yy1:1:1,') is out of range');
  print_ln('_r((',xx1:1:4,',',yy1:1:4,')..(',xx2:1:4,',',yy2:1:4,
      '), ',ww:1:4,');');
@y
  if (fabs(xx1)>=4096.0)or(fabs(yy1)>=4096.0)or@|
      (fabs(xx2)>=4096.0)or(fabs(yy2)>=4096.0)or(ww>=4096.0) then
    warn('hrule or vrule is out of range');
  print('_r((');
  fprint_real(mpx_file, xx1,1,4); print(',');
  fprint_real(mpx_file, yy1,1,4); print(')..(');
  fprint_real(mpx_file, xx2,1,4); print(',');
  fprint_real(mpx_file, yy2,1,4); print('), ');
  fprint_real(mpx_file, ww,1,4); print(',');
  @<Print a \.{withcolor} specifier if appropriate@>@/
  print_ln(');');
@z

@x [80] Yet another fix for printing of real numbers.
print_ln('setbounds _p to (0,',dd:1:4,')--(',w:1:4,',',dd:1:4,')--');
print_ln(' (',w:1:4,',',h:1:4,')--(0,',h:1:4,')--cycle;')
@y
print('setbounds _p to (0,');
fprint_real(mpx_file, dd,1,4); print(')--(');
fprint_real(mpx_file, w,1,4);  print(',');
fprint_real(mpx_file, dd,1,4); print_ln(')--');@/
print(' (');
fprint_real(mpx_file, w,1,4);  print(',');
fprint_real(mpx_file, h,1,4);  print(')--(0,');
fprint_real(mpx_file, h,1,4);  print_ln(')--cycle;')
@z

@x [88] push and pop commands (COLOR).
@p procedure do_push;
@y
@p @<Declare procedures to handle color commands@>
procedure do_push;
@z

@x [94] Additional cases for DVI commands (COLOR).
four_cases(xxx1): for k:=1 to p do
    down_the_drain:=get_byte;
@y
four_cases(xxx1): do_xxx(p);
@z

@x [98] Main program.
print_ln(banner);
@y
print (banner); 
print_ln (version_string);
@z
@x Exit with appropriate status.
final_end:end.
@y
if history<=cksum_trouble then uexit(0)
else uexit(history);
end.
@z

@x [103] System-dependent changes.
This section should be replaced, if necessary, by changes to the program
that are necessary to make \.{DVItoMP} work at a particular installation.
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
const n_options = 2; {Pascal won't count array lengths for us.}
var @!long_options: array[0..n_options] of getopt_struct;
    @!getopt_return_val: integer;
    @!option_index: c_int_type;
    @!current_option: 0..n_options;
begin
  @<Define the option table@>;
  repeat
    getopt_return_val := getopt_long_only (argc, argv, '', long_options,
                                           address_of (option_index));
    if getopt_return_val = -1 then begin
      {End of arguments; we exit the loop below.} ;
    
    end else if getopt_return_val = "?" then begin
      usage ('dvitomp');

    end else if argument_is ('help') then begin
      usage_help (DVITOMP_HELP, nil);

    end else if argument_is ('version') then begin
      print_version_and_exit (term_banner, 'AT&T Bell Laboraties', 'John Hobby.');

    end; {Else it was a flag; |getopt| has already done the assignment.}
  until getopt_return_val = -1;

  {Now |optind| is the index of first non-option on the command line.
   We must have one or two remaining arguments.}
  if (optind + 1 <> argc) and (optind + 2 <> argc) then begin
    write_ln (stderr, 'dvitomp: Need one or two file arguments.');
    usage ('dvitomp');
  end;

  dvi_name := cmdline (optind);
  
  if optind + 2 <= argc then begin
    mpx_name := cmdline (optind + 1); {The user specified the other name.}
  end else begin
    {User did not specify the other name; default it from the first.}
    mpx_name := basename_change_suffix (dvi_name, '.dvi', '.mpx');
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

@ An element with all zeros always ends the list.

@<Define the option...@> =
long_options[current_option].name := 0;
long_options[current_option].has_arg := 0;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;

@ Global filenames.

@<Global...@> =
@!dvi_name, @!mpx_name:c_string;



@* Color support.
These changes support \.{dvips}-style ``\.{color push NAME}'' and
``\.{color pop}'' specials. We store a list of named colors, sorted by
name, and decorate the relevant drawing commands with ``\.{withcolor
(r,g,b)}'' specifiers while a color is defined.

@ A constant bounding the size of the named-color array.

@<Constants...@> =
@!max_named_colors=100; {maximum number of distinct named colors}

@ Then we declare a record for color types.

@<Types...@> =
@!named_color_record=record@;@/
  @!name:c_string; {color name}
  @!value:c_string; {text to pass to MetaPost}
  end;

@ Declare the named-color array itself.

@<Globals...@> =
@!named_colors: array[1..max_named_colors] of named_color_record;
  {stores information about named colors, in sorted order by name}
@!num_named_colors:integer; {number of elements of |named_colors| that are valid}

@ This function, used only during initialization, defines a named color.

@<Define |parse_arguments|@> =
procedure def_named_color(n,v: c_string);
  begin
    if num_named_colors = max_named_colors then
      abort('too many named color definitions')
    else if (num_named_colors > 0)
            and (strcmp(n, named_colors[num_named_colors].name) <= 0)
    then
      abort('named colors added out of alphabetical order');
    incr(num_named_colors);
    named_colors[num_named_colors].name := n;
    named_colors[num_named_colors].value := v
  end;

@ During the initialization phase, we define values for all the named
colors defined in \.{colordvi.tex}. CMYK-to-RGB conversion by GhostScript.

@<Set initial values@> =
num_named_colors := 0;
def_named_color('Apricot', '(1.0, 0.680006, 0.480006)');
def_named_color('Aquamarine', '(0.180006, 1.0, 0.7)');
def_named_color('Bittersweet', '(0.760012, 0.0100122, 0.0)');
def_named_color('Black', '(0.0, 0.0, 0.0)');
def_named_color('Blue', '(0.0, 0.0, 1.0)');
def_named_color('BlueGreen', '(0.15, 1.0, 0.669994)');
def_named_color('BlueViolet', '(0.1, 0.05, 0.960012)');
def_named_color('BrickRed', '(0.719994, 0.0, 0.0)');
def_named_color('Brown', '(0.4, 0.0, 0.0)');
def_named_color('BurntOrange', '(1.0, 0.489988, 0.0)');
def_named_color('CadetBlue', '(0.380006, 0.430006, 0.769994)');
def_named_color('CarnationPink', '(1.0, 0.369994, 1.0)');
def_named_color('Cerulean', '(0.0600122, 0.889988, 1.0)');
def_named_color('CornflowerBlue', '(0.35, 0.869994, 1.0)');
def_named_color('Cyan', '(0.0, 1.0, 1.0)');
def_named_color('Dandelion', '(1.0, 0.710012, 0.160012)');
def_named_color('DarkOrchid', '(0.6, 0.2, 0.8)');
def_named_color('Emerald', '(0.0, 1.0, 0.5)');
def_named_color('ForestGreen', '(0.0, 0.880006, 0.0)');
def_named_color('Fuchsia', '(0.45, 0.00998169, 0.919994)');
def_named_color('Goldenrod', '(1.0, 0.9, 0.160012)');
def_named_color('Gray', '(0.5, 0.5, 0.5)');
def_named_color('Green', '(0.0, 1.0, 0.0)');
def_named_color('GreenYellow', '(0.85, 1.0, 0.310012)');
def_named_color('JungleGreen', '(0.0100122, 1.0, 0.480006)');
def_named_color('Lavender', '(1.0, 0.519994, 1.0)');
def_named_color('LimeGreen', '(0.5, 1.0, 0.0)');
def_named_color('Magenta', '(1.0, 0.0, 1.0)');
def_named_color('Mahogany', '(0.65, 0.0, 0.0)');
def_named_color('Maroon', '(0.680006, 0.0, 0.0)');
def_named_color('Melon', '(1.0, 0.539988, 0.5)');
def_named_color('MidnightBlue', '(0.0, 0.439988, 0.569994)');
def_named_color('Mulberry', '(0.640018, 0.0800061, 0.980006)');
def_named_color('NavyBlue', '(0.0600122, 0.460012, 1.0)');
def_named_color('OliveGreen', '(0.0, 0.6, 0.0)');
def_named_color('Orange', '(1.0, 0.389988, 0.130006)');
def_named_color('OrangeRed', '(1.0, 0.0, 0.5)');
def_named_color('Orchid', '(0.680006, 0.360012, 1.0)');
def_named_color('Peach', '(1.0, 0.5, 0.3)');
def_named_color('Periwinkle', '(0.430006, 0.45, 1.0)');
def_named_color('PineGreen', '(0.0, 0.75, 0.160012)');
def_named_color('Plum', '(0.5, 0.0, 1.0)');
def_named_color('ProcessBlue', '(0.0399878, 1.0, 1.0)');
def_named_color('Purple', '(0.55, 0.139988, 1.0)');
def_named_color('RawSienna', '(0.55, 0.0, 0.0)');
def_named_color('Red', '(1.0, 0.0, 0.0)');
def_named_color('RedOrange', '(1.0, 0.230006, 0.130006)');
def_named_color('RedViolet', '(0.590018, 0.0, 0.660012)');
def_named_color('Rhodamine', '(1.0, 0.180006, 1.0)');
def_named_color('RoyalBlue', '(0.0, 0.5, 1.0)');
def_named_color('RoyalPurple', '(0.25, 0.1, 1.0)');
def_named_color('RubineRed', '(1.0, 0.0, 0.869994)');
def_named_color('Salmon', '(1.0, 0.469994, 0.619994)');
def_named_color('SeaGreen', '(0.310012, 1.0, 0.5)');
def_named_color('Sepia', '(0.3, 0.0, 0.0)');
def_named_color('SkyBlue', '(0.380006, 1.0, 0.880006)');
def_named_color('SpringGreen', '(0.739988, 1.0, 0.239988)');
def_named_color('Tan', '(0.860012, 0.580006, 0.439988)');
def_named_color('TealBlue', '(0.119994, 0.980006, 0.640018)');
def_named_color('Thistle', '(0.880006, 0.410012, 1.0)');
def_named_color('Turquoise', '(0.15, 1.0, 0.8)');
def_named_color('Violet', '(0.210012, 0.119994, 1.0)');
def_named_color('VioletRed', '(1.0, 0.189988, 1.0)');
def_named_color('White', '(1.0, 1.0, 1.0)');
def_named_color('WildStrawberry', '(1.0, 0.0399878, 0.610012)');
def_named_color('Yellow', '(1.0, 1.0, 0.0)');
def_named_color('YellowGreen', '(0.560012, 1.0, 0.260012)');
def_named_color('YellowOrange', '(1.0, 0.580006, 0.0)');

@ Color commands get a separate warning procedure. |warn| sets |history :=
warning_given|, which causes a nonzero exit status; but color errors are
trivial and should leave the exit status zero.

@d color_warn(#)==begin err_print_ln('DVItoMP warning: ',#); if history < warning_given then history := cksum_trouble; end

@ The |do_xxx| procedure handles DVI specials (defined with the
|xxx1...xxx4| commands).

@<Declare procedures to handle color commands@> =
procedure do_xxx(p: integer);
label 9999; {exit procedure}
const bufsiz = 256; {FIXME: Fixed size buffer.}
var buf: packed array[0..bufsiz] of eight_bits;
    l, r, m, k, len: integer;
    found: boolean;
begin
  len := 0;
  while (p > 0) and (len < bufsiz) do begin
    buf[len] := get_byte;
    decr(p); incr(len);
  end;
  @<Check whether |buf| contains a color command; if not, |goto 9999|@>
  if p > 0 then begin
     color_warn('long "color" special ignored'); goto 9999; end;
  if @<|buf| contains a color pop command@> then begin
     @<Handle a color pop command@>
  end else if @<|buf| contains a color push command@> then begin
     @<Handle a color push command@>
  end else begin
     color_warn('unknown "color" special ignored'); goto 9999; end;
9999: for k := 1 to p do down_the_drain := get_byte;
end;

@ 

@<Check whether |buf| contains a color command; if not, |goto 9999|@> =
if (len <= 5)
   or (buf[0] <> "c")
   or (buf[1] <> "o")
   or (buf[2] <> "l")
   or (buf[3] <> "o")
   or (buf[4] <> "r")
   or (buf[5] <> " ")
  then goto 9999;

@ 

@<|buf| contains a color push command@> =
(len >= 11) and (buf[6] = "p") and (buf[7] = "u") and (buf[8] = "s") and (buf[9] = "h") and (buf[10] = " ")

@ 

@<|buf| contains a color pop command@> =
(len = 9) and (buf[6] = "p") and (buf[7] = "o") and (buf[8] = "p")

@ The \.{color push} and \.{pop} commands imply a color stack, so we need a
global variable to hold that stack.

@<Constants...@> =
max_color_stack_depth=10; {maximum depth of saved color stack}

@ Here's the actual stack variables.

@<Globals...@> =
color_stack_depth: integer; {current depth of saved color stack}
color_stack: array[1..max_color_stack_depth] of c_string; {saved color stack}

@ Initialize the stack to empty.

@<Set initial values@> =
color_stack_depth := 0;

@ \.{color pop} just pops the stack.

@<Handle a color pop command@> =
finish_last_char;
if color_stack_depth > 0 then begin
  free(color_stack[color_stack_depth]);
  decr(color_stack_depth)
end
else begin
  color_warn('color stack underflow')
end;

@ \.{color push} pushes a color onto the stack.

@<Handle a color push command@> =
finish_last_char;
if color_stack_depth >= max_color_stack_depth then
  abort('color stack overflow');
incr(color_stack_depth);
{ I don't know how to do string operations in Pascal. }
{ Skip over extra spaces after 'color push'. }
l := 11;
while (l < len - 1) and (buf[l] = " ") do incr(l);
if @<|buf[l]| contains an rgb command@> then begin
  @<Handle a color push rgb command@>
end else if @<|buf[l]| contains a cmyk command@> then begin
  @<Handle a color push cmyk command@>
end else if @<|buf[l]| contains a gray command@> then begin
  @<Handle a color push gray command@>
end else begin
  @<Handle a named color push command@>
end;

@ 

@<|buf[l]| contains an rgb command@> =
(l + 4 < len)
and (buf[l]   = "r")
and (buf[l+1] = "g")
and (buf[l+2] = "b")
and (buf[l+3] = " ")

@

@<Handle a color push rgb command@> =
l := l + 4;
while (l < len) and (buf[l] = " ") do incr(l);
{ Remove spaces at end of buf }
while (len > l) and (buf[len - 1] = " ") do decr(len);
color_stack[color_stack_depth]:=xmalloc_array(char,len-l+3);
k := 0;
@<Copy |buf[l]| to |color_stack[color_stack_depth][k]| in tuple form@>

@ 

@<|buf[l]| contains a gray command@> =
(l + 5 < len)
and (buf[l]   = "g")
and (buf[l+1] = "r")
and (buf[l+2] = "a")
and (buf[l+3] = "y")
and (buf[l+4] = " ")

@

@<Handle a color push gray command@> =
l := l + 5;
while (l < len) and (buf[l] = " ") do incr(l);
{ Remove spaces at end of buf }
while (len > l) and (buf[len - 1] = " ") do decr(len);
color_stack[color_stack_depth]:=xmalloc_array(char,len-l+9);
strcpy(color_stack[color_stack_depth],'white*');
k := 6;
@<Copy |buf[l]| to |color_stack[color_stack_depth][k]| in tuple form@>

@ 

@<|buf[l]| contains a cmyk command@> =
(l + 5 < len)
and (buf[l]   = "c")
and (buf[l+1] = "m")
and (buf[l+2] = "y")
and (buf[l+3] = "k")
and (buf[l+4] = " ")

@

@<Handle a color push cmyk command@> =
l := l + 5;
while (l < len) and (buf[l] = " ") do incr(l);
{ Remove spaces at end of buf }
while (len > l) and (buf[len - 1] = " ") do decr(len);
color_stack[color_stack_depth]:=xmalloc_array(char,len-l+7);
strcpy(color_stack[color_stack_depth],'cmyk');
k := 4;
@<Copy |buf[l]| to |color_stack[color_stack_depth][k]| in tuple form@>

@

@<Copy |buf[l]| to |color_stack[color_stack_depth][k]| in tuple form@> =
color_stack[color_stack_depth][k] := "(";
incr(k);
while l < len do
  if buf[l] = " " then begin
    color_stack[color_stack_depth][k] := ",";
    while (l < len) and (buf[l] = " ") do incr(l);
    incr(k);
  end else begin
    color_stack[color_stack_depth][k] := buf[l];
    incr(l);
    incr(k);
  end;
color_stack[color_stack_depth][k] := ")";
color_stack[color_stack_depth][k+1] := chr(0)

@ Binary-search the |named_colors| array, then push the found color onto
the stack.

@<Handle a named color push command@> =
for k := l to len - 1 do begin
  buf[k - l] := xchr[buf[k]];
end;
buf[len - l] := 0;
len := len - l;
l := 1; r := num_named_colors;
found := false;
while (l <= r) and not found do begin
  m := (l + r) / 2;
  k := strcmp(buf, named_colors[m].name);
  if k = 0 then begin
    color_stack[color_stack_depth]:=xstrdup(named_colors[m].value);
    found := true;
  end else if k < 0 then
    r := m - 1
  else
    l := m + 1;
end;
if not found then begin
   color_warn('non-hardcoded color "',stringcast(buf),'" in "color push" command');
   color_stack[color_stack_depth]:=xstrdup(buf);
end;

@ Last but not least, this code snippet prints a \.{withcolor} specifier
for the top of the color stack, if the stack is nonempty.

@<Print a \.{withcolor} specifier if appropriate@> =
if color_stack_depth > 0 then begin
  print(' withcolor ');
  fputs(color_stack[color_stack_depth], mpx_file);
end;

@z
