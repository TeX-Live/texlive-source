% Change file for BibTeX in C, originally by Howard Trickey.
%
% 05/28/84      Initial implementation, version 0.41 of BibTeX
% 07/01/84      Version 0.41a of BibTeX.
% 12/17/84      Version 0.97c of BibTeX.
% 02/12/85      Version 0.98c of BibTeX.
% 02/25/85      Newer version 0.98c of BibTeX.
% 03/25/85      Version 0.98f of BibTeX
% 05/23/85      Version 0.98i of BibTeX
% 02/11/88      Version 0.99b of BibTeX
% 04/04/88      Version 0.99c; converted for use with web2c (ETM).
% 11/30/89      Use FILENAMESIZE instead of 1024 (KB).
% 03/09/90	`int' is a bad variable name for C.
% (more recent changes in the ChangeLog)

% [0] Let bibtex.tex work with latest webmac (which defines \ET, hence
% making E active loses).
@x
\catcode`E=13 \uppercase{\def E{e}}
\def\\#1{\hbox{\let E=\drop\it#1\/\kern.05em}} % italic type for identifiers
@y
\let\maybe = \iffalse % process only changed sections
@z

@x [1] Define my_name
@d banner=='This is BibTeX, Version 0.99d' {printed when the program starts}
@y
@d my_name=='bibtex'
@d banner=='This is BibTeX, Version 0.99d' {printed when the program starts}
@z

% [2] `term_in' and `term_out' are standard input and output.  But
% there is a complication: BibTeX passes `term_out' to some routines as
% a var parameter.  web2c turns a var parameter f into &f at the calling
% side -- and stdout is sometimes implemented as `&_iob[1]' or some
% such.  An address of an address is invalid. Therefore, we define
% variables `standardinput' and `standardoutput' in openinout.c.
@x
@d term_out == tty
@d term_in == tty
@y
@d term_out == standard_output
@d term_in == standard_input

@<Globals in the outer block@>=
standard_input, standard_output: text;
@z

@x [3] Add log_pr macros.
@d trace_pr_newline == begin write_ln(log_file); end
@y
@d trace_pr_newline == begin write_ln(log_file); end
@#
@d log_pr(#) == trace_pr(#)
@d log_pr_ln(#) == trace_pr_ln(#)
@d log_pr_newline == trace_pr_newline
@z

@x [4] Turn debug..gubed et al. into #ifdef's.
@d debug == @{          { remove the `|@{|' when debugging }
@d gubed == @t@>@}      { remove the `|@}|' when debugging }
@f debug == begin
@f gubed == end
@#
@d stat == @{           { remove the `|@{|' when keeping statistics }
@d tats == @t@>@}       { remove the `|@}|' when keeping statistics }
@f stat == begin
@f tats == end
@#
@d trace == @{          { remove the `|@{|' when in |trace| mode }
@d ecart == @t@>@}      { remove the `|@}|' when in |trace| mode }
@f trace == begin
@f ecart == end
@y
@d debug == ifdef('TEXMF_DEBUG')
@d gubed == endif('TEXMF_DEBUG')
@f debug == begin
@f gubed == end
@#
@d stat == ifndef('NO_BIBTEX_STAT')
@d tats == endifn('NO_BIBTEX_STAT')
@f stat==begin
@f tats==end
@#
@d trace == ifdef@&('TRACE')
@d ecart == endif@&('TRACE')
@f trace == begin
@f ecart == end
@z

@x [10] Eliminate the |exit_program| label.
label   close_up_shop,@!exit_program @<Labels in the outer block@>;
@y
label   close_up_shop @<Labels in the outer block@>;
@z

@x [10] Don't print the banner unless verbose, and initialize dynamic arrays.
begin
initialize;
print_ln(banner);@/
@y
@<Define |parse_arguments|@>
begin
standard_input := stdin;
standard_output := stdout;
@#
pool_size := POOL_SIZE;
buf_size := BUF_SIZE;
max_bib_files := MAX_BIB_FILES;
max_glob_strs := MAX_GLOB_STRS;
max_fields := MAX_FIELDS;
max_cites := MAX_CITES;
wiz_fn_space := WIZ_FN_SPACE;
lit_stk_size := LIT_STK_SIZE;
@#
setup_params;
@#
{Add one to the sizes because that's what bibtex uses.}
bib_file   := XTALLOC (max_bib_files + 1, alpha_file);
bib_list   := XTALLOC (max_bib_files + 1, str_number);
entry_ints := nil;
entry_strs := nil;
wiz_functions := XTALLOC (wiz_fn_space + 1, hash_ptr2);
field_info := XTALLOC (max_fields + 1, str_number);
s_preamble := XTALLOC (max_bib_files + 1, str_number);
str_pool   := XTALLOC (pool_size + 1, ASCII_code);
buffer     := XTALLOC (buf_size + 1, ASCII_code);
sv_buffer  := XTALLOC (buf_size + 1, ASCII_code);
ex_buf     := XTALLOC (buf_size + 1, ASCII_code);
out_buf    := XTALLOC (buf_size + 1, ASCII_code);
name_tok   := XTALLOC (buf_size + 1, buf_pointer);
name_sep_char := XTALLOC (buf_size + 1, ASCII_code);
@#
glb_str_ptr := XTALLOC (max_glob_strs, str_number);
global_strs := XTALLOC (max_glob_strs * (glob_str_size + 1), ASCII_code);
glb_str_end := XTALLOC (max_glob_strs, integer);
@#
cite_list  := XTALLOC (max_cites + 1, str_number);
type_list  := XTALLOC (max_cites + 1, hash_ptr2);
entry_exists := XTALLOC (max_cites + 1, boolean);
cite_info  := XTALLOC (max_cites + 1, str_number);
@#
str_start  := XTALLOC (max_strings + 1, pool_pointer);
@#
hash_next  := XTALLOC (hash_max + 1, hash_pointer);
hash_text  := XTALLOC (hash_max + 1, str_number);
hash_ilk   := XTALLOC (hash_max + 1, str_ilk);
ilk_info   := XTALLOC (hash_max + 1, integer);
fn_type    := XTALLOC (hash_max + 1, fn_class);
@#
lit_stack  := XTALLOC (lit_stk_size + 1, integer);
lit_stk_type := XTALLOC (lit_stk_size + 1, stk_type);
@#
compute_hash_prime;
@#
initialize;
{This initializes the jmp9998 buffer, which can be used early}
hack0;
if verbose then begin
  print (banner);
  print_ln (version_string);
end
else begin
  log_pr (banner);
  log_pr_ln (version_string);
end;
log_pr_ln ('Capacity: max_strings=', max_strings:1,
           ', hash_size=', hash_size:1, ', hash_prime=', hash_prime:1);
@z

% [10] Possibly exit with bad status.  It doesn't seem worth it to move
% the definitions of the |history| values to above this module; hence the 1.
@x
exit_program:
end.
@y
if (history > 1) then uexit (history);
end.
@z

@x [13] Remove nonlocal goto.
    goto exit_program;
@y
    uexit (1);
@z

@x [14] Increase some constants, and uppercase others for dynamic arrays.
@<Constants in the outer block@>=
@y
@<Constants in the outer block@>=
@!hash_base = empty + 1;                {lowest numbered hash-table location}
@!quote_next_fn = hash_base - 1;  {special marker used in defining functions}
@z

@x [still 14]
@!buf_size=1000; {maximum number of characters in an input line (or string)}
@y
@!BUF_SIZE=20000; {initial maximum number of characters in an input line
                                                                (or string)}
@z

@x [still 14]
@!max_bib_files=20; {maximum number of \.{.bib} files allowed}
@!pool_size=65000; {maximum number of characters in strings}
@!max_strings=4000; {maximum number of strings, including pre-defined;
                                                        must be |<=hash_size|}
@!max_cites=750; {maximum number of distinct cite keys; must be
                                                        |<=max_strings|}
@!min_crossrefs=2; {minimum number of cross-refs required for automatic
                                                        |cite_list| inclusion}
@!wiz_fn_space=3000; {maximum amount of |wiz_defined|-function space}
@y [still 14]
@!MAX_BIB_FILES=20; {initial number of \.{.bib} files allowed}
@!POOL_SIZE=65000; {initial number of characters in strings}
@!MAX_STRINGS=4000; {minimum value for |max_strings|}
@!MAX_CITES=750; {initial number of distinct cite keys; must be
                                                        |<=max_strings|}
@!WIZ_FN_SPACE=3000; {initial amount of |wiz_defined|-function space}
{|min_crossrefs| can be set at runtime now.}
@z

@x [still 14]
@!single_fn_space=100; {maximum amount for a single |wiz_defined|-function}
@y
@!SINGLE_FN_SPACE=50; {initial amount for a single |wiz_defined|-function}
@z

@x [still 14] handle long citation strings
@!max_ent_ints=3000; {maximum number of |int_entry_var|s
                                        (entries $\times$ |int_entry_var|s)}
@!max_ent_strs=3000; {maximum number of |str_entry_var|s
                                        (entries $\times$ |str_entry_var|s)}
@!ent_str_size=100; {maximum size of a |str_entry_var|; must be |<=buf_size|}
@!glob_str_size=1000; {maximum size of a |str_global_var|;
                                                        must be |<=buf_size|}
@!max_fields=17250; {maximum number of fields (entries $\times$ fields,
@y
@!ENT_STR_SIZE=100; {maximum size of a |str_entry_var|; must be |<=buf_size|}
@!GLOB_STR_SIZE=1000; {maximum  size of a |str_global_var|;
                                                        must be |<=buf_size|}
@!MAX_GLOB_STRS=10;    {initial number of |str_global_var| names}
@!MAX_FIELDS=5000; {initial number of fields (entries $\times$ fields,
@z

@x [still 14]
@!lit_stk_size=100; {maximum number of literal functions on the stack}
@y
@!LIT_STK_SIZE=50; {initial space for literal functions on the stack}
@z

@x [15] Increase more constants in the web defines.
@d hash_size=5000       {must be |>= max_strings| and |>= hash_prime|}
@d hash_prime=4253      {a prime number about 85\% of |hash_size| and |>= 128|
                                                and |< @t$2^{14}-2^6$@>|}
@d file_name_size=40    {file names shouldn't be longer than this}
@d max_glob_strs=10     {maximum number of |str_global_var| names}
@d max_glb_str_minus_1 = max_glob_strs-1  {to avoid wasting a |str_global_var|}
@y
{|hash_size| and |hash_prime| are now computed.}
@d HASH_SIZE=5000       {minimum value for |hash_size|}
@#
@d file_name_size==maxint {file names have no arbitrary maximum length}
@#
{For dynamic allocation.}
@d x_entry_strs_tail(#) == (#)]
@d x_entry_strs(#) == entry_strs[(#) * (ent_str_size+1) + x_entry_strs_tail
@d x_global_strs_tail(#) == (#)]
@d x_global_strs(#) == global_strs[(#) * (glob_str_size+1) + x_global_strs_tail
@z

@x [16] Add new variables-that-used-to-be-constants for dynamic arrays.
@<Globals in the outer block@>=
@y
@<Globals in the outer block@>=
@!pool_size: integer;
@!max_bib_files: integer;
@!max_cites: integer;
@!wiz_fn_space: integer;
@!ent_str_size: integer;
@!glob_str_size: integer;
@!max_glob_strs: integer;
@!max_fields: integer;
@!lit_stk_size: integer;
@#
@!max_strings: integer;
@!hash_size: integer;
@!hash_prime: integer;
@#
@!hash_max: integer;     {highest numbered hash-table location}
@!end_of_def: integer;   {another special marker used in defining functions}
@!undefined: integer;    {a special marker used for |type_list|}
@z

@x [17] max_strings=hash_size settable at runtime.
if (hash_prime >= (16384-64)) then              bad:=10*bad+6;
@y
if (hash_base <> 1) then                        bad:=10*bad+6;
@z

@x [17] remove the following, since buf_size changes dynamically.
if (ent_str_size > buf_size) then               bad:=10*bad+9;
if (glob_str_size > buf_size) then              bad:=100*bad+11;
@y
@z

@x [22, 23, 27, 28] Allow any character as input. [22]
@!ASCII_code=0..127;    {seven-bit numbers}
@y
@!ASCII_code=0..255;    {eight-bit numbers}
@z

@x [23]
@d text_char == char    {the data type of characters in text files}
@d first_text_char=0    {ordinal number of the smallest element of |text_char|}
@d last_text_char=127   {ordinal number of the largest element of |text_char|}

@<Local variables for initialization@>=
i:0..last_text_char;    {this is the first one declared}
@y
@d text_char == ASCII_code    {the data type of characters in text files}
@d first_text_char=0    {ordinal number of the smallest element of |text_char|}
@d last_text_char=255   {ordinal number of the largest element of |text_char|}

@<Local variables for initialization@>=
i:integer;
@z

@x [27]
for i:=1 to @'37 do xchr[i]:=' ';
xchr[tab]:=chr(tab);
@y
for i:=0 to @'37 do xchr[i]:=chr(i);
for i:=@'177 to @'377 do xchr[i]:=chr(i);
@z

@x [28]
for i:=first_text_char to last_text_char do xord[chr(i)]:=invalid_code;
for i:=1 to @'176 do xord[xchr[i]]:=i;
@y
for i:=first_text_char to last_text_char do xord[xchr[i]]:=i;
@z

@x [32] Put the [128..255] range into lex_class alpha.
for i:=0 to @'177 do lex_class[i] := other_lex;
@y
for i:=0 to @'177 do lex_class[i] := other_lex;
for i:=@'200 to @'377 do lex_class[i] := alpha;
@z

% [32] Make RET a `white_space' character, so we won't choke on DOS
% files, which use CR/LF for line endings.
@x
lex_class[tab] := white_space;
@y
lex_class[tab] := white_space;
lex_class[13] := white_space;
@z

@x [33] Allow the [128..255] range in legal_id_char.
for i:=0 to @'177 do id_class[i] := legal_id_char;
@y
for i:=0 to @'377 do id_class[i] := legal_id_char;
@z

% [37] file_name_size no longer exists.  See comments in tex.ch for why
% we change the element type to text_char.
@x
@!name_of_file:packed array[1..file_name_size] of char;
                         {on some systems this is a \&{record} variable}
@!name_length:0..file_name_size;
  {this many characters are relevant in |name_of_file| (the rest are blank)}
@!name_ptr:0..file_name_size+1;         {index variable into |name_of_file|}
@y
@!name_of_file:^text_char;
@!name_length:integer;
  {this many characters are relevant in |name_of_file| }
@!name_ptr:integer;         {index variable into |name_of_file|}
@z

@x [38] File opening.
The \ph\ compiler with which the present version of \TeX\ was prepared has
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
\TeX\ to undertake appropriate corrective action.

\TeX's file-opening procedures return |false| if no file identified by
|name_of_file| could be opened.

@d reset_OK(#)==erstat(#)=0
@d rewrite_OK(#)==erstat(#)=0

@<Procedures and functions for file-system interacting@>=
function erstat(var f:file):integer; extern;    {in the runtime library}
@#@t\2@>
function a_open_in(var f:alpha_file):boolean;   {open a text file for input}
begin reset(f,name_of_file,'/O'); a_open_in:=reset_OK(f);
end;
@#
function a_open_out(var f:alpha_file):boolean;  {open a text file for output}
begin rewrite(f,name_of_file,'/O'); a_open_out:=rewrite_OK(f);
end;
@y
@ File opening will be done in C. But we want an auxiliary function to
change a \BibTeX\ string into a C string, to keep string pool stuff
out of the C code in @.{lib/openclose.c}.

@d no_file_path = -1

@<Procedures and functions for all file...@>=
function bib_makecstring(s:str_number):cstring;
var cstr:cstring;
    i:pool_pointer;
begin
  cstr := xmalloc_array (ASCII_code, length (s) + 1);
  for i := 0 to length(s) - 1 do begin
    cstr[i] := str_pool[str_start[s] + i];
  end;
  cstr[length(s)] := 0;
  bib_makecstring := cstr;
exit: end;
@z

@x [39] Do file closing in C.
@<Procedures and functions for file-system interacting@>=
procedure a_close(var f:alpha_file);            {close a text file}
begin close(f);
end;
@y
File closing will be done in C, too.
@z

@x [42] Dynamic buf_size.
@!buffer:buf_type;      {usually, lines of characters being read}
@y
@!buf_size:integer;     {size of buffer}
@!buffer:buf_type;      {usually, lines of characters being read}
@z

@x [43] Dyanmic buf_size.
@!buf_pointer = 0..buf_size;                    {an index into a |buf_type|}
@!buf_type = array[buf_pointer] of ASCII_code;  {for various buffers}
@y
@!buf_pointer = integer;                        {an index into a |buf_type|}
@!buf_type = ^ASCII_code;                       {for various buffers}
@z

@x [47] Dynamic buf_size.
overflow('buffer size ',buf_size);
@y
BIB_XRETALLOC_NOSET ('buffer', buffer, ASCII_code,
                     buf_size, buf_size + BUF_SIZE);
BIB_XRETALLOC_NOSET ('sv_buffer', sv_buffer, ASCII_code,
                     buf_size, buf_size + BUF_SIZE);
BIB_XRETALLOC_NOSET ('ex_buf', ex_buf, ASCII_code,
                     buf_size, buf_size + BUF_SIZE);
BIB_XRETALLOC_NOSET ('out_buf', out_buf, ASCII_code,
                     buf_size, buf_size + BUF_SIZE);
BIB_XRETALLOC_NOSET ('name_tok', name_tok, buf_pointer,
                     buf_size, buf_size + BUF_SIZE);
BIB_XRETALLOC ('name_sep_char', name_sep_char, ASCII_code,
               buf_size, buf_size + BUF_SIZE);
@z

@x [48] web2c doesn't understand f^.
    buffer[last]:=xord[f^];
    get(f); incr(last);
    end;
  get(f);
@y
    buffer[last] := xord[getc (f)];
    incr (last);
    end;
  vgetc (f); {skip the eol}
@z

@x [49] Dynamically allocate str_pool.
@!str_pool : packed array[pool_pointer] of ASCII_code;  {the characters}
@!str_start : packed array[str_number] of pool_pointer; {the starting pointers}
@y
@!str_pool : ^ASCII_code;   {the characters}
@!str_start :^pool_pointer; {the starting pointers}
@z

@x [50] pool_size is a variable now, so can't be used as a constant.
@!pool_pointer = 0..pool_size;  {for variables that point into |str_pool|}
@!str_number = 0..max_strings;  {for variables that point into |str_start|}
@y
@!pool_pointer = integer; {for variables that point into |str_pool|}
@!str_number = integer;   {for variables that point into |str_start|}
@z

@x [51] Add log_pr_pool_str macro.
@d trace_pr_pool_str(#) == begin
                           out_pool_str(log_file,#);
                           end
@y
@d trace_pr_pool_str(#) == begin
                           out_pool_str(log_file,#);
                           end
@#
@d log_pr_pool_str(#) == trace_pr_pool_str(#)
@z

@x [54] Reallocate str_pool.  We may need more than POOL_SIZE.
  if (pool_ptr+# > pool_size) then
@y
  while (pool_ptr+# > pool_size) do
@z
@x [54] Reallocate str_pool.
overflow('pool size ',pool_size);
@y
BIB_XRETALLOC ('str_pool', str_pool, ASCII_code, pool_size,
               pool_size + POOL_SIZE);
@z

% [59] (start_name) reallocate name_of_file for the new name and
% terminate with null.
@x
if (length(file_name) > file_name_size) then
    begin
    print ('File=');
    print_pool_str (file_name);
    print_ln (',');
    file_nm_size_overflow;
    end;
@y
free (name_of_file);
name_of_file := xmalloc_array (ASCII_code, length (file_name) + 1);
@z

@x
name_length := length(file_name);
@y
name_length := length(file_name);
name_of_file[name_length + 1] := 0;
@z

@x [60] (file_nm_size_overflow) This procedure is not used.
procedure file_nm_size_overflow;
begin
overflow('file name size ',file_name_size);
end;
@y
@z

% [61] (add_extension) Don't pad name_of_file with blanks, terminate
% with null. And junk the overflow check, since Web2c can't translate
% the print statement properly and it can never happen, anyway.
@x
if (name_length + length(ext) > file_name_size) then
    begin
    print ('File=',name_of_file,', extension=');
    print_pool_str (ext); print_ln (',');
    file_nm_size_overflow;
    end;
@y
@z
@x
name_ptr := name_length+1;
while (name_ptr <= file_name_size) do   {pad with blanks}
    begin
    name_of_file[name_ptr] := ' ';
    incr(name_ptr);
    end;
@y
name_of_file[name_length + 1] := 0;
@z

@x [62] (add_area) This procedure is not used.
procedure add_area(@!area:str_number);
var p_ptr: pool_pointer;        {running index}
begin
if (name_length + length(area) > file_name_size) then
    begin
    print ('File=');
    print_pool_str (area); print (name_of_file,',');
    file_nm_size_overflow;
    end;
name_ptr := name_length;
while (name_ptr > 0) do         {shift up name}
    begin
    name_of_file[name_ptr+length(area)] := name_of_file[name_ptr];
    decr(name_ptr);
    end;
name_ptr := 1;
p_ptr := str_start[area];
while (p_ptr < str_start[area+1]) do
    begin
    name_of_file[name_ptr] := chr (str_pool[p_ptr]);
    incr(name_ptr); incr(p_ptr);
    end;
name_length := name_length + length(area);
end;
@y
@z

@x [65] now Pascal consts or vars, instead of web macros.
@d hash_base = empty + 1                {lowest numbered hash-table location}
@d hash_max = hash_base + hash_size - 1 {highest numbered hash-table location}
@y
@z

@x [65] max_strings=hash_size settable at runtime.
@!hash_loc=hash_base..hash_max;         {a location within the hash table}
@!hash_pointer=empty..hash_max;         {either |empty| or a |hash_loc|}
@y
@!hash_loc=integer;             {a location within the hash table}
@!hash_pointer=integer;         {either |empty| or a |hash_loc|}
@z

@x [66] max_strings=hash_size settable at runtime.
@!hash_next : packed array[hash_loc] of hash_pointer;   {coalesced-list link}
@!hash_text : packed array[hash_loc] of str_number;     {pointer to a string}
@!hash_ilk : packed array[hash_loc] of str_ilk;         {the type of string}
@!ilk_info : packed array[hash_loc] of integer;         {|ilk|-specific info}
@!hash_used : hash_base..hash_max+1;    {allocation pointer for hash table}
@y
@!hash_next : ^hash_pointer;   {coalesced-list link}
@!hash_text : ^str_number;     {pointer to a string}
@!hash_ilk : ^str_ilk;         {the type of string}
@!ilk_info : ^integer;         {|ilk|-specific info}
@!hash_used : integer;    {allocation pointer for hash table}
@z

@x [69] Not used anymore.
@d max_hash_value = hash_prime+hash_prime-2+127         {|h|'s maximum value}
@y
@z

@x [69] max_strings=hash_size settable at runtime.
var h:0..max_hash_value;        {hash code}
@y
var h:integer;        {hash code}
@z

@x [69] str_lookup - Avoid 'uninitialized' warning.
@!old_string:boolean;   {set to |true| if it's an already encountered string}
@y
@z

@x [69] str_lookup - Avoid 'uninitialized' warning.
old_string := false;
@y
str_num := 0;           {set to |>0| if it's an already encountered string}
@z

@x [71] str_lookup - Avoid 'uninitialized' warning.
            old_string := true;
@y
@z

@x [72] str_lookup - Avoid 'uninitialized' warning.
if (old_string) then            {it's an already encountered string}
@y
if (str_num>0) then             {it's an already encountered string}
@z

@x [74] Pascal Web's char
@!pds_type = packed array [pds_loc] of char;
@y
@!pds_type = const_cstring;
@z

@x [78] C strings start at zero instead of one.
for i:=1 to len do
    buffer[i] := xord[pds[i]];
@y
for i:=1 to len do
    buffer[i] := xord[ucharcast(pds[i-1])];
@z

@x [98] Can't do this tangle-time arithmetic with file_name_size.
@!aux_name_length : 0..file_name_size+1;        {\.{.aux} name sans extension}
@y
@!aux_name_length : integer;
@z

@x [101] Reading the aux file name and command-line processing.
This procedure consists of a loop that reads and processes a (nonnull)
\.{.aux} file name.  It's this module and the next two that must be
changed on those systems using command-line arguments.  Note: The
|term_out| and |term_in| files are system dependent.

@<Procedures and functions for the reading and processing of input files@>=
procedure get_the_top_level_aux_file_name;
label aux_found,@!aux_not_found;
var @<Variables for possible command-line processing@>@/
begin
check_cmnd_line := false;                       {many systems will change this}
loop
    begin
    if (check_cmnd_line) then
        @<Process a possible command line@>
      else
        begin
        write (term_out,'Please type input file name (no extension)--');
        if (eoln(term_in)) then                 {so the first |read| works}
            read_ln (term_in);
        aux_name_length := 0;
        while (not eoln(term_in)) do
            begin
            if (aux_name_length = file_name_size) then
                begin
                while (not eoln(term_in)) do    {discard the rest of the line}
                    get(term_in);
                sam_you_made_the_file_name_too_long;
                end;
            incr(aux_name_length);
            name_of_file[aux_name_length] := term_in^;
            get(term_in);
            end;
        end;
    @<Handle this \.{.aux} name@>;
aux_not_found:
    check_cmnd_line := false;
    end;
aux_found:                      {now we're ready to read the \.{.aux} file}
end;
@y
@<Procedures and functions for the reading and processing of input files@>=
procedure get_the_top_level_aux_file_name;
label aux_found,@!aux_not_found;
begin
  @<Process a possible command line@>
  {Leave room for the \.., the extension, the junk byte at the
   beginning, and the null byte at the end.}
  name_of_file := xmalloc_array (ASCII_code, strlen (cmdline (optind)) + 5);
  strcpy (stringcast(name_of_file + 1), cmdline (optind));
  aux_name_length := strlen (stringcast(name_of_file + 1));
  @<Handle this \.{.aux} name@>;
aux_not_found:  uexit (1);
aux_found:                      {now we're ready to read the \.{.aux} file}
end;
@z

% [101] Don't need this variable; we use argc to check if we have a
% command line.
@x
@<Variables for possible command-line processing@>=
@!check_cmnd_line : boolean;    {|true| if we're to check the command line}
@y
@z

@x [103] Get the aux file name from the command line.
@<Process a possible command line@>=
begin
do_nothing;             {the ``default system'' doesn't use the command line}
end
@y
@<Process a possible command line@>=
parse_arguments;
@z

% [107] Don't use a path to find the aux file, and don't add the
% extension if it's already there.
@x
add_extension (s_aux_extension);        {this also sets |name_length|}
aux_ptr := 0;                           {initialize the \.{.aux} file stack}
if (not a_open_in(cur_aux_file)) then
@y
if (name_length < 4) or
   (strcmp (stringcast(name_of_file + 1 + name_length - 4), '.aux') <> 0)
then
  add_extension (s_aux_extension)        {this also sets |name_length|}
else
  aux_name_length := aux_name_length - 4; {set to length without \.{.aux}}
aux_ptr := 0;                           {initialize the \.{.aux} file stack}
if (not kpse_in_name_ok(stringcast(name_of_file+1)) or
    not a_open_in(cur_aux_file,no_file_path)) then
@z

@x [107] - Check log_file name.
if (not a_open_out(log_file)) then
@y
if (not kpse_out_name_ok(stringcast(name_of_file+1)) or
    not a_open_out(log_file)) then
@z

@x [107] - Check bbl_file name
if (not a_open_out(bbl_file)) then
@y
if (not kpse_out_name_ok(stringcast(name_of_file+1)) or
    not a_open_out(bbl_file)) then
@z

@x [109] Add log_pr_aux_name.
procedure print_aux_name;
begin
print_pool_str (cur_aux_str);
print_newline;
end;
@y
procedure print_aux_name;
begin
print_pool_str (cur_aux_str);
print_newline;
end;
@#
procedure log_pr_aux_name;
begin
log_pr_pool_str (cur_aux_str);
log_pr_newline;
end;
@z

@x [111] Be silent unless verbose.
print ('The top-level auxiliary file: ');
print_aux_name;
@y
if verbose then begin
  print ('The top-level auxiliary file: ');
  print_aux_name;
end
else begin
  log_pr ('The top-level auxiliary file: ');
  log_pr_aux_name;
end;
@z

@x [118] bib_list is dynamically allocated.
@!bib_list : array[bib_number] of str_number;   {the \.{.bib} file list}
@y
@!bib_list : ^str_number;   {the \.{.bib} file list}
@z
@x [still 118] bib_file also.
@!bib_file : array[bib_number] of alpha_file; {corresponding |file| variables}
@y
@!bib_file : ^alpha_file; {corresponding |file| variables}
@z

@x [119] max_bib_files is a variable now, so can't be used as a const.
@!bib_number = 0..max_bib_files;        {gives the |bib_list| range}
@y
@!bib_number = integer;        {gives the |bib_list| range}
@z

@x [122] Don't print extension twice; add log_pr_bib_name function.
procedure print_bib_name;
begin
print_pool_str (cur_bib_str);
print_pool_str (s_bib_extension);
print_newline;
end;
@y
{Return true if the |ext| string is at the end of the |s| string.  There
 are surely far more clever ways to do this, but it doesn't matter.}
function str_ends_with (@!s:str_number; @!ext:str_number) : boolean;
var i : integer;
    str_idx,ext_idx   : integer;
    str_char,ext_char : ASCII_code;
begin
  str_ends_with := false;
  if (length (ext) > length (s)) then
    return; {if extension is longer, they don't match}
  str_idx := length (s) - 1;
  ext_idx := length (ext) - 1;
  while (ext_idx >= 0) do begin {|>=| so we check the |'.'| char.}
    str_char := str_pool[str_start[s]+str_idx];
    ext_char := str_pool[str_start[ext]+ext_idx];
    if (str_char <> ext_char) then
      return;
    decr (str_idx);
    decr (ext_idx);
  end;
  str_ends_with := true;
exit: end;

{The above is needed because the file name specified in the
 \.{\\bibdata} command may or may not have the \.{.bib} extension. If it
 does, we don't want to print \.{.bib} twice.}
procedure print_bib_name;
begin
print_pool_str (cur_bib_str);
if not str_ends_with (cur_bib_str, s_bib_extension) then
  print_pool_str (s_bib_extension);
print_newline;
end;
@#
procedure log_pr_bib_name;
begin
log_pr_pool_str (cur_bib_str);
if not str_ends_with (cur_bib_str, s_bib_extension) then
  log_pr_pool_str (s_bib_extension);
log_pr_newline;
end;
@z

@x [124] Reallocate when we run out of bib files.
    overflow('number of database files ',max_bib_files);
@y
begin
  {Keep old value of |max_bib_files| for the last array.}
  BIB_XRETALLOC_NOSET ('bib_list', bib_list, str_number, max_bib_files,
                 max_bib_files + MAX_BIB_FILES);
  BIB_XRETALLOC_NOSET ('bib_file', bib_file, alpha_file, max_bib_files,
                 max_bib_files + MAX_BIB_FILES);
  BIB_XRETALLOC ('s_preamble', s_preamble, str_number, max_bib_files,
                 max_bib_files + MAX_BIB_FILES);
end;
@z

@x [still 124] Use BIBINPUTS to search for the .bib file.
add_extension (s_bib_extension);
if (not a_open_in(cur_bib_file)) then
    begin
    add_area (s_bib_area);
    if (not a_open_in(cur_bib_file)) then
        open_bibdata_aux_err ('I couldn''t open database file ');
    end;
@y
if (not kpse_in_name_ok(stringcast(name_of_file+1)) or
    not a_open_in(cur_bib_file, kpse_bib_format)) then
        open_bibdata_aux_err ('I couldn''t open database file ');
@z

@x [128] Use BSTINPUTS/TEXINPUTS to search for .bst files.
add_extension (s_bst_extension);
if (not a_open_in(bst_file)) then
    begin
    add_area (s_bst_area);
    if (not a_open_in(bst_file)) then
        begin
        print ('I couldn''t open style file ');
        print_bst_name;@/
        bst_str := 0;                           {mark as unused again}
        aux_err_return;
        end;
    end;
@y
if (not kpse_in_name_ok(stringcast(name_of_file+1)) or
    not a_open_in(bst_file, kpse_bst_format)) then
    begin
        print ('I couldn''t open style file ');
        print_bst_name;@/
        bst_str := 0;                           {mark as unused again}
        aux_err_return;
    end;
@z

@x [128] Be silent unless verbose.
print ('The style file: ');
print_bst_name;
@y
if verbose then begin
  print ('The style file: ');
  print_bst_name;
end
else begin
  log_pr ('The style file: ');
  log_pr_bst_name;
end;
@z

@x [129] Add log_pr_bst_name.
procedure print_bst_name;
begin
print_pool_str (bst_str);
print_pool_str (s_bst_extension);
print_newline;
end;
@y
procedure print_bst_name;
begin
print_pool_str (bst_str);
print_pool_str (s_bst_extension);
print_newline;
end;
@#
procedure log_pr_bst_name;
begin
log_pr_pool_str (bst_str);
log_pr_pool_str (s_bst_extension);
log_pr_newline;
end;
@z

@x [130] cite_list is dynamically allocated.
@!cite_list : packed array[cite_number] of str_number;  {the cite-key list}
@y
@!cite_list : ^str_number;  {the cite-key list}
@z

@x [131] max_cites is no longer const.
@!cite_number = 0..max_cites;   {gives the |cite_list| range}
@y
@!cite_number = integer;   {gives the |cite_list| range}
@z

@x [139] Dynamic max_cites.
if (last_cite = max_cites) then
    begin
    print_pool_str (hash_text[cite_loc]);
    print_ln (' is the key:');
    overflow('number of cite keys ',max_cites);
@y
if (last_cite = max_cites) then
    begin
    BIB_XRETALLOC_NOSET ('cite_list', cite_list, str_number,
                         max_cites, max_cites + MAX_CITES);
    BIB_XRETALLOC_NOSET ('type_list', type_list, hash_ptr2,
                         max_cites, max_cites + MAX_CITES);
    BIB_XRETALLOC_NOSET ('entry_exists', entry_exists, boolean,
                         max_cites, max_cites + MAX_CITES);
    BIB_XRETALLOC ('cite_info', cite_info, str_number,
                   max_cites, max_cites + MAX_CITES);
    while (last_cite < max_cites) do
        begin
        type_list[last_cite] := empty;@/
        cite_info[last_cite] := any_value;  {to appeas \PASCAL's boolean evaluation}
        incr(last_cite);
        end;
@z

% [142] Don't pad with blanks, terminate with null.
% Don't use a path to search for subsidiary aux files,
% but do check the directory of the main .aux file.
% 
% This last is useful, for example, when --output-dir is used and the
% .aux file has an \@input directive resulting from a LaTeX \include;
% see bibtex-auxinclude.test. It's necessary because BibTeX itself does
% not have --output-directory. Maybe it would be (have been?) better to
% add it, but seems too intrusive now? Different bbl location.
@x
while (name_ptr <= file_name_size) do   {pad with blanks}
    begin
    name_of_file[name_ptr] := ' ';
    incr(name_ptr);
    end;
if (not a_open_in(cur_aux_file)) then
@y
name_of_file[name_ptr] := 0;
if (not kpse_in_name_ok(stringcast(name_of_file+1))
    or (not a_open_in(cur_aux_file, no_file_path)
        and not a_open_in_with_dirname(cur_aux_file, no_file_path,
                                       bib_makecstring(top_lev_str)))
    ) then
@z

% [152] This goto gets turned into a setjmp/longjmp by ./convert --
% unfortunately, it is a nonlocal goto.  ekrell@ulysses.att.com
% implemented the conversion.
@x
buf_ptr2 := last;       {to get the first input line}
loop
    begin
    if (not eat_bst_white_space) then   {the end of the \.{.bst} file}
        goto bst_done;
    get_bst_command_and_process;
    end;
bst_done: a_close (bst_file);
@y
buf_ptr2 := last;       {to get the first input line}
hack1;
    begin
    if (not eat_bst_white_space) then   {the end of the \.{.bst} file}
        hack2;
    get_bst_command_and_process;
    end;
bst_done: a_close (bst_file);
@z

% max_ent_ints and max_ent_strs are gone, max_fields is no longer const.
@x [161] quote_next_fn and end_of_def are Pascal consts, instead of web macros.
@d quote_next_fn = hash_base - 1  {special marker used in defining functions}
@d end_of_def = hash_max + 1      {another such special marker}

@<Types in the outer block@>=
@!fn_class = 0..last_fn_class;          {the \.{.bst} function classes}
@!wiz_fn_loc = 0..wiz_fn_space;  {|wiz_defined|-function storage locations}
@!int_ent_loc = 0..max_ent_ints;        {|int_entry_var| storage locations}
@!str_ent_loc = 0..max_ent_strs;        {|str_entry_var| storage locations}
@!str_glob_loc = 0..max_glb_str_minus_1; {|str_global_var| storage locations}
@!field_loc = 0..max_fields;            {individual field storage locations}
@y
@<Types in the outer block@>=
@!fn_class = 0..last_fn_class;          {the \.{.bst} function classes}
@!wiz_fn_loc = integer;         {|wiz_defined|-function storage locations}
@!int_ent_loc = integer;        {|int_entry_var| storage locations}
@!str_ent_loc = integer;        {|str_entry_var| storage locations}
@!str_glob_loc = integer; {|str_global_var| storage locations}
@!field_loc = integer;            {individual field storage locations}
@z

@x [162] max_strings=hash_size settable at runtime.
@!fn_type : packed array[hash_loc] of fn_class;
@y
@!fn_type : ^fn_class;
@z

@x [162] Dynamically allocate wiz_functions.
@!wiz_functions : packed array[wiz_fn_loc] of hash_ptr2;
@y
@!wiz_functions : ^hash_ptr2;
@z

% [still 162] Convert entry_ints and entry_strs to dynamically-allocated
% one-dimensional arrays; too bad C and Pascal lag Fortran in supporting
% run-time dimensioning of multidimensional arrays.  Other changes that
% follow this one will convert every reference to entry_strs[p][q] to
% x_entry_strs(p)(q), the equivalent of entry_strs[p*(ent_str_size+1) +
% q], but hidden inside a macro to mask the addressing computation.
% Although WEB does not have multi-argument macros, webman.tex shows how
% to get the equivalent effect.
@x
@!entry_ints : array[int_ent_loc] of integer;
@!num_ent_ints : int_ent_loc;   {the number of distinct |int_entry_var| names}
@!str_ent_ptr : str_ent_loc;    {general |str_entry_var| location}
@!entry_strs : array[str_ent_loc] of
                                packed array[0..ent_str_size] of ASCII_code;
@y
@!entry_ints : ^integer; {dynamically-allocated array}
@!num_ent_ints : int_ent_loc;   {the number of distinct |int_entry_var| names}
@!str_ent_ptr : str_ent_loc;    {general |str_entry_var| location}
@!entry_strs : ^ASCII_code; {dynamically-allocated array}
@z

@x [still 162] Dynamically allocate global strings
@!str_glb_ptr : 0..max_glob_strs;       {general |str_global_var| location}
@!glb_str_ptr : array[str_glob_loc] of str_number;
@!global_strs : array[str_glob_loc] of array[0..glob_str_size] of ASCII_code;
@!glb_str_end : array[str_glob_loc] of 0..glob_str_size;        {end markers}
@!num_glb_strs : 0..max_glob_strs; {number of distinct |str_global_var| names}
@y
@!str_glb_ptr : integer;         {general |str_global_var| location}
@!glb_str_ptr : ^str_number;
@!global_strs : ^ASCII_code;
@!glb_str_end : ^integer;        {end markers}
@!num_glb_strs : integer;        {number of distinct |str_global_var| names}
@z

@x [still 162] Dynamically allocate field_info.
@!field_info : packed array[field_loc] of str_number;
@y
@!field_info : ^str_number;
@z

@x [188] Dynamically allocate singl_function.
type @!fn_def_loc = 0..single_fn_space; {for a single |wiz_defined|-function}
var singl_function : packed array[fn_def_loc] of hash_ptr2;
@y
type @!fn_def_loc = integer; {for a single |wiz_defined|-function}
var singl_function : ^hash_ptr2;
@!single_fn_space : integer; {space allocated for this |singl_function| instance}
@z

@x [still 188] Dynamically allocate singl_function.
begin
eat_bst_white_and_eof_check ('function');
@y
begin
single_fn_space := SINGLE_FN_SPACE;
singl_function := XTALLOC (single_fn_space + 1, hash_ptr2);
eat_bst_white_and_eof_check ('function');
@z

@x [still 188] Dynamically allocate singl_function.
exit:
end;
@y
exit:
libc_free (singl_function);
end;
@z

@x [189] Reallocate if out of single function space.
                            singl_fn_overflow;
@y
                            begin
                            BIB_XRETALLOC ('singl_function', singl_function, hash_ptr2,
                                           single_fn_space, single_fn_space + SINGLE_FN_SPACE);
                            end;
@z

@x [still 189] Procedure |singl_fn_overflow| replaced by inline code.
procedure singl_fn_overflow;
begin
overflow('single function space ',single_fn_space);
end;
@y
@z

@x [199] A variable named `int' is no good in C.
@<Procedures and functions for handling numbers, characters, and strings@>=
@y
@d int == the_int
@<Procedures and functions for handling numbers, characters, and strings@>=
@z

@x [201] Reallocate if out of wizard space.
if (single_ptr + wiz_def_ptr > wiz_fn_space) then
    begin
    print (single_ptr + wiz_def_ptr : 0,': ');
    overflow('wizard-defined function space ',wiz_fn_space);
    end;
@y
while (single_ptr + wiz_def_ptr > wiz_fn_space) do
    begin
    BIB_XRETALLOC ('wiz_functions', wiz_functions, hash_ptr2,
                    wiz_fn_space, wiz_fn_space + WIZ_FN_SPACE);
    end;
@z

@x [217] Reallocate if out of global strings.
    overflow('number of string global-variables ',max_glob_strs);
@y
    begin
    BIB_XRETALLOC_NOSET ('glb_str_ptr', glb_str_ptr, str_number,
                         max_glob_strs, max_glob_strs + MAX_GLOB_STRS);
    BIB_XRETALLOC_STRING ('global_strs', global_strs, glob_str_size,
                          max_glob_strs, max_glob_strs + MAX_GLOB_STRS);
    BIB_XRETALLOC ('glb_str_end', glb_str_end, integer,
                   max_glob_strs, max_glob_strs + MAX_GLOB_STRS);
    str_glb_ptr := num_glb_strs;
    while (str_glb_ptr < max_glob_strs) do      {make new |str_global_var|s empty}
        begin
        glb_str_ptr[str_glb_ptr] := 0;
        glb_str_end[str_glb_ptr] := 0;
        incr(str_glb_ptr);
        end;
    end;
@z

@x [220] undefined is now a Pascal const, instead of a web macro
@d undefined = hash_max + 1     {a special marker used for |type_list|}
@y
@z

@x [220] type_list is dynamically allocated.
@!type_list : packed array[cite_number] of hash_ptr2;
@y
@!type_list : ^hash_ptr2;
@z
@x [220] entry_exists also.
@!entry_exists : packed array[cite_number] of boolean;
@y
@!entry_exists : ^boolean;
@z
@x [220] cite_info also.
@!cite_info : packed array[cite_number] of str_number; {extra |cite_list| info}
@y
@!cite_info : ^str_number; {extra |cite_list| info}
@z

@x [225] Be silent unless verbose.
    print ('Database file #',bib_ptr+1:0,': ');
    print_bib_name;@/
@y
    if verbose then begin
      print ('Database file #',bib_ptr+1:0,': ');
      print_bib_name;
    end
    else begin
      log_pr ('Database file #',bib_ptr+1:0,': ');
      log_pr_bib_name;
    end;
@z

@x [227] Reallocate if out of fields.
procedure check_field_overflow (@!total_fields : integer);
begin
if (total_fields > max_fields) then
    begin
    print_ln (total_fields:0,' fields:');
    overflow('total number of fields ',max_fields);
@y
procedure check_field_overflow (@!total_fields : integer);
var @!f_ptr: field_loc;
    @!start_fields: field_loc;
begin
if (total_fields > max_fields) then
    begin
    start_fields := max_fields;
    BIB_XRETALLOC ('field_info', field_info, str_number, max_fields,
                   total_fields + MAX_FIELDS);
    {Initialize to |missing|.}
    for f_ptr := start_fields to max_fields - 1 do begin
      field_info[f_ptr] := missing;
    end;
@z

@x [243] Reallocate when we run out of s_preamble's.
    bib_err ('You''ve exceeded ',max_bib_files:0,' preamble commands');
@y
begin
  {Keep old value of |max_bib_files| for the last array.}
  BIB_XRETALLOC_NOSET ('bib_list', bib_list, str_number, max_bib_files,
                 max_bib_files + MAX_BIB_FILES);
  BIB_XRETALLOC_NOSET ('bib_file', bib_file, alpha_file, max_bib_files,
                 max_bib_files + MAX_BIB_FILES);
  BIB_XRETALLOC ('s_preamble', s_preamble, str_number, max_bib_files,
                 max_bib_files + MAX_BIB_FILES);
end;
@z

@x [264] Add check for fieldinfo[] overflow.
field_ptr := entry_cite_ptr * num_fields + fn_info[field_name_loc];
@y
field_ptr := entry_cite_ptr * num_fields + fn_info[field_name_loc];
if (field_ptr >= max_fields) then
    confusion ('field_info index is out of range');
@z

@x [266] Fix bug in the add_database_cite procedure.
check_field_overflow (num_fields*new_cite);
@y
check_field_overflow (num_fields*(new_cite+1));
@z

@x [278] Add check for fieldinfo[] overflow.
@<Add cross-reference information@>=
begin
@y
@<Add cross-reference information@>=
begin
if ((num_cites - 1) * num_fields + crossref_num >= max_fields) then
    confusion ('field_info index is out of range');
@z

@x [280] Add check for fieldinfo[] overflow.
@<Subtract cross-reference information@>=
begin
@y
@<Subtract cross-reference information@>=
begin
if ((num_cites - 1) * num_fields + crossref_num >= max_fields) then
    confusion ('field_info index is out of range');
@z

@x [286] Add check for fieldinfo[] overflow.
@<Slide this cite key down to its permanent spot@>=
begin
@y
@<Slide this cite key down to its permanent spot@>=
begin
if ((cite_xptr + 1) * num_fields > max_fields) then
  confusion ('field_info index is out of range');
@z

@x [288] Allocate entry_ints as needed.
if (num_ent_ints*num_cites > max_ent_ints) then
    begin
    print (num_ent_ints*num_cites,': ');
    overflow('total number of integer entry-variables ',max_ent_ints);
    end;
@y
entry_ints := XTALLOC ((num_ent_ints + 1) * (num_cites + 1), integer);
@z

@x [289] Allocate entry_strs as needed.
if (num_ent_strs*num_cites > max_ent_strs) then
    begin
    print (num_ent_strs*num_cites,': ');
    overflow('total number of string entry-variables ',max_ent_strs);
    end;
@y
entry_strs := XTALLOC ((num_ent_strs + 1) * (num_cites + 1) * (ent_str_size + 1), ASCII_code);
@z

@x [289] Macroize entry_strs[][].
    entry_strs[str_ent_ptr][0] := end_of_string;
@y
    x_entry_strs(str_ent_ptr)(0) := end_of_string;
@z

@x [291] Dynamic lit_stk_size.
@!lit_stack : array[lit_stk_loc] of integer;    {the literal function stack}
@!lit_stk_type : array[lit_stk_loc] of stk_type; {their corresponding types}
@y
@!lit_stack : ^integer;    {the literal function stack}
@!lit_stk_type : ^stk_type; {their corresponding types}
@z

@x [292] Dynamic lit_stk_size.
@!lit_stk_loc = 0..lit_stk_size;        {the stack range}
@y
@!lit_stk_loc = integer;        {the stack range}
@z

@x [302] Macroize entry_strs[][].
    char1 := entry_strs[ptr1][char_ptr];
    char2 := entry_strs[ptr2][char_ptr];
@y
    char1 := x_entry_strs(ptr1)(char_ptr);
    char2 := x_entry_strs(ptr2)(char_ptr);
@z

@x [308] Reallocate literal stack.
    overflow('literal-stack size ',lit_stk_size);
@y
    begin
    BIB_XRETALLOC_NOSET ('lit_stack', lit_stack, integer,
                         lit_stk_size, lit_stk_size + LIT_STK_SIZE);
    BIB_XRETALLOC ('lit_stk_type', lit_stk_type, stk_type,
                   lit_stk_size, lit_stk_size + LIT_STK_SIZE);
    end;
@z

@x [321] Dynamic buf_size.
if (out_buf_length+(p_ptr2-p_ptr1) > buf_size) then
    overflow('output buffer size ',buf_size);
@y
while (out_buf_length+(p_ptr2-p_ptr1) > buf_size) do
    buffer_overflow;
@z

@x [328] Add check for fieldinfo[] overflow.
    field_ptr := cite_ptr*num_fields + fn_info[ex_fn_loc];
@y
    field_ptr := cite_ptr*num_fields + fn_info[ex_fn_loc];
    if (field_ptr >= max_fields) then
        confusion ('field_info index is out of range');
@z

@x [330] Macroize entry_strs[][]
    while (entry_strs[str_ent_ptr][ex_buf_ptr] <> end_of_string) do
                                        {copy characters into the buffer}
        append_ex_buf_char (entry_strs[str_ent_ptr][ex_buf_ptr]);
@y
    while (x_entry_strs(str_ent_ptr)(ex_buf_ptr) <> end_of_string) do
                                        {copy characters into the buffer}
        append_ex_buf_char (x_entry_strs(str_ent_ptr)(ex_buf_ptr));
@z

@x [331] Macroize global_strs[][]
        append_char (global_strs[str_glb_ptr][glob_chr_ptr]);
@y
        append_char (x_global_strs(str_glb_ptr)(glob_chr_ptr));
@z

@x [335] Bug fix: remove duplicate entry.
build_in('width$      ',6,b_width,n_width);
build_in('while$      ',6,b_while,n_while);
build_in('width$      ',6,b_width,n_width);
@y
build_in('while$      ',6,b_while,n_while);
build_in('width$      ',6,b_width,n_width);
@z

@x [338] s_preamble is dynamically allocated.
@!s_preamble : array[bib_number] of str_number;
@y
@!s_preamble : ^str_number;
@z

@x [345] Dynamic buf_size.
@!name_tok : packed array[buf_pointer] of buf_pointer; {name-token ptr list}
@!name_sep_char : packed array[buf_pointer] of ASCII_code; {token-ending chars}
@y
@!name_tok : ^buf_pointer; {name-token ptr list}
@!name_sep_char : ^ASCII_code; {token-ending chars}
@z

@x [358] Macroize entry_strs[][].
    while (sp_ptr < sp_xptr1) do
        begin                   {copy characters into |entry_strs|}
        entry_strs[str_ent_ptr][ent_chr_ptr] := str_pool[sp_ptr];
        incr(ent_chr_ptr);
        incr(sp_ptr);
        end;
    entry_strs[str_ent_ptr][ent_chr_ptr] := end_of_string;
@y
    while (sp_ptr < sp_xptr1) do
        begin                   {copy characters into |entry_strs|}
        x_entry_strs(str_ent_ptr)(ent_chr_ptr) := str_pool[sp_ptr];
        incr(ent_chr_ptr);
        incr(sp_ptr);
        end;
    x_entry_strs(str_ent_ptr)(ent_chr_ptr) := end_of_string;
@z

@x [360] Macroize global_strs[][]
            global_strs[str_glb_ptr][glob_chr_ptr] := str_pool[sp_ptr];
@y
            x_global_strs(str_glb_ptr)(glob_chr_ptr) := str_pool[sp_ptr];
@z

% [389] bibtex.web has mutually exclusive tests here; Oren said he
% doesn't want to fix it until 1.0, since it's obviously of no practical
% import (or someone would have found it before GCC 2 did).  Changing
% the second `and' to an `or' makes all but the last of multiple authors
% be omitted in the bbl file, so I simply removed the statement.
@x
while ((ex_buf_xptr < ex_buf_ptr) and
                        (lex_class[ex_buf[ex_buf_ptr]] = white_space) and
                        (lex_class[ex_buf[ex_buf_ptr]] = sep_char)) do
        incr(ex_buf_xptr);                      {this removes leading stuff}
@y
@z

% Forgot to check for pool overflow here.  Triggered by test case linked
% from http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=520920.
@x [439]
while (sp_ptr < sp_end) do                      {shift the substring}
@y
str_room(sp_end - sp_ptr);
while (sp_ptr < sp_end) do                      {shift the substring}
@z

% Forgot to check for pool overflow here.  Triggered by bibtex-mem.test (3).
@x [445]
if (pop_lit2 >= cmd_str_ptr) then       {no shifting---merely change pointers}
@y
str_room(sp_brace_level + sp_end - sp_ptr);
if (pop_lit2 >= cmd_str_ptr) then       {no shifting---merely change pointers}
@z

% [460] Eliminate unreferenced statement label, because `undefined' is
% now a constant expression that is not evaluated at the Web level. If
% this label were ever required, it could be replaced by the constant
% 9997, which is not used as a statement label in BibTeX.
@x
    undefined : trace_pr ('unknown')
@y
    trace_pr ('unknown')
@z

@x [461] Macroize entry_strs[][].
        while (entry_strs[str_ent_ptr][ent_chr_ptr] <> end_of_string) do
            begin
            trace_pr (xchr[entry_strs[str_ent_ptr][ent_chr_ptr]]);
            incr(ent_chr_ptr);
            end;
@y
        while (x_entry_strs(str_ent_ptr)(ent_chr_ptr) <> end_of_string) do
            begin
            trace_pr (xchr[x_entry_strs(str_ent_ptr)(ent_chr_ptr)]);
            incr(ent_chr_ptr);
            end;
@z

@x [463] Add check for fieldinfo[] overflow.
    field_ptr := cite_ptr * num_fields;
    field_end_ptr := field_ptr + num_fields;
@y
    field_ptr := cite_ptr * num_fields;
    field_end_ptr := field_ptr + num_fields;
    if (field_end_ptr > max_fields) then
        confusion ('field_info index is out of range');
@z

@x [468] System-dependent changes.
This section should be replaced, if necessary, by changes to the program
that are necessary to make \BibTeX\ work at a particular installation.
It is usually best to design your change file so that all changes to
previous sections preserve the section numbering; then everybody's version
will be consistent with the printed program. More extensive changes,
which introduce new sections, can be inserted here; then only the index
itself will get a new section number.
@y
@d argument_is (#) == (strcmp (long_options[option_index].name, #) = 0)

@<Define |parse_arguments|@> =
procedure parse_arguments;
const n_options = 4; {Pascal won't count array lengths for us.}
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
      usage (my_name);

    end else if argument_is ('min-crossrefs') then begin
      min_crossrefs := atoi (optarg);

    end else if argument_is ('help') then begin
      usage_help (BIBTEX_HELP, nil);

    end else if argument_is ('version') then begin
      print_version_and_exit (banner, 'Oren Patashnik', nil, nil);

    end; {Else it was a flag; |getopt| has already done the assignment.}
  until getopt_return_val = -1;

  {Now |optind| is the index of first non-option on the command line.
   We must have one remaining argument.}
  if (optind + 1 <> argc) then begin
    write_ln (stderr, my_name, ': Need exactly one file argument.');
    usage (my_name);
  end;
end;

@ Here is the first of the options we allow.
@.-terse@>

@<Define the option...@> =
current_option := 0;
long_options[0].name := 'terse';
long_options[0].has_arg := 0;
long_options[0].flag := address_of (verbose);
long_options[0].val := 0;
incr (current_option);

@ The global variable |verbose| determines whether or not we print
progress information.

@<Glob...@> =
@!verbose: c_int_type;

@ Start off |true|, to match the default behavior.

@<Initialize the option...@> =
verbose := true;

@ Here is an option to change the minimum number of cross-refs required
for automatic |cite_list| inclusion.
@.-min-crossrefs@>

@<Define the option...@> =
long_options[current_option].name := 'min-crossrefs';
long_options[current_option].has_arg := 1;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;
incr (current_option);

@
@<Glob...@> =
@!min_crossrefs: integer;

@ Set |min_crossrefs| to two by default, so we match the
documentation (\.{btxdoc.tex}).

@<Initialize the option...@> =
min_crossrefs := 2;

@ One of the standard options.
@.-help@>

@<Define the option...@> =
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

@ Determine |ent_str_size|, |glob_str_size|, and |max_strings| from the
environment, configuration file, or default value.  Set
|hash_size:=max_strings|, but not less than |HASH_SIZE|.

{|setup_bound_var| stuff adapted from \.{tex.ch}.}
@d setup_bound_var(#)==bound_default:=#; setup_bound_var_end
@d setup_bound_var_end(#)==bound_name:=#; setup_bound_var_end_end
@d setup_bound_var_end_end(#)==
  setup_bound_variable(address_of(#), bound_name, bound_default);
  if # < bound_default then # := bound_default

@<Procedures and functions for about everything@>=
procedure setup_params;
var bound_default: integer; {for setup}
@!bound_name: const_cstring; {for setup}
begin kpse_set_program_name (argv[0], 'bibtex');
setup_bound_var (ENT_STR_SIZE)('ent_str_size')(ent_str_size);
setup_bound_var (GLOB_STR_SIZE)('glob_str_size')(glob_str_size);
setup_bound_var (MAX_STRINGS)('max_strings')(max_strings);
@#
hash_size := max_strings;
if hash_size < HASH_SIZE then hash_size := HASH_SIZE;
hash_max := hash_size + hash_base - 1;
end_of_def := hash_max + 1;
undefined := hash_max + 1;
end;

@ We use the algorithm from Knuth's \.{primes.web} to compute |hash_prime|
as the smallest prime number not less than 85\% of |hash_size| (and
|>=128|).

@d primes == hash_next {array holding the first |k| primes}
@d mult == hash_text {array holding odd multiples of the first |o| primes}

@<Procedures and functions for about everything@>=
procedure compute_hash_prime;
var hash_want: integer; {85\% of |hash_size|}
@!k: integer; {number of prime numbers $p_i$ in |primes|}
@!j: integer; {a prime number candidate}
@!o: integer; {number of odd multiples of primes in |mult|}
@!square: integer; {$p_o^2$}
@!n: integer; {loop index}
@!j_prime: boolean; {is |j| a prime?}
begin hash_want := (hash_size div 20) * 17;
j := 1;
k := 1;
hash_prime := 2;
primes[k] := hash_prime;
o := 2;
square := 9;
while hash_prime < hash_want do
  begin
  repeat
    j := j + 2;
    if j = square then
      begin
      mult[o] := j;
      j := j + 2;
      incr (o);
      square := primes[o] * primes[o];
      end;
    n := 2;
    j_prime := true;
    while (n < o) and j_prime do
      begin
      while mult[n] < j do mult[n] := mult[n] + 2 * primes[n];
      if mult[n] = j then j_prime := false;
      incr (n);
      end;
  until j_prime;
  incr (k);
  hash_prime := j;
  primes[k] := hash_prime;
  end;
end;
@z
