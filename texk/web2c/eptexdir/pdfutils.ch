%% Support for some primitives defined in pdfTeX
%%
%% \pdfstrcmp: need for LaTeX3
%%   In comparison, Japanese characters will be always encoded in UTF-8.
%%
%% \pdffilemoddate and co.: for standalone package
%%   (\pdfcreationdate, \pdffilemoddate, \pdffilesize)
%%
%% \pdfsavepos and co.
%%   (\pdfsavepos, \pdfpage{width,height}, \pdflast{x,y}pos)
%%   papersize special automatically sets \pdfpage{width,height} (quick hack).
%%
%% \pdffiledump: for bmpsize package by Heiko Oberdiek
%%
%% \pdfshellescape: by doraTeX's request
%%
%% \pdfmdfivesum: by Akira's request
%%   As \pdfstrcmp, Japanese characters will be always encoded in UTF-8 in
%%   \pdfmdfivesum {...}. (no conversion for \pdfmdfivesum file <filename>)
%%
%% \pdfprimitive and \ifpdfprimitive: for LaTeX3 (2015/07/15)

@x
@* \[12] Displaying boxes.
@y
@<Declare procedures that need to be declared forward for \pdfTeX@>@;

@* \[12] Displaying boxes.
@z

@x \[if]pdfprimitive
@d frozen_special=frozen_control_sequence+10
  {permanent `\.{\\special}'}
@d frozen_null_font=frozen_control_sequence+11
  {permanent `\.{\\nullfont}'}
@y
@d frozen_special=frozen_control_sequence+10
  {permanent `\.{\\special}'}
@d frozen_primitive=frozen_control_sequence+11
  {permanent `\.{\\pdfprimitive}'}
@d frozen_null_font=frozen_control_sequence+12
  {permanent `\.{\\nullfont}'}
@z

@x
@d dimen_pars=23 {total number of dimension parameters}
@y
@d pdf_page_width_code=23  {page width}
@d pdf_page_height_code=24 {page height}
@d dimen_pars=25 {total number of dimension parameters}
@z

@x \pdfpage{width,height}
@d emergency_stretch==dimen_par(emergency_stretch_code)
@y
@d emergency_stretch==dimen_par(emergency_stretch_code)
@d pdf_page_width==dimen_par(pdf_page_width_code)
@d pdf_page_height==dimen_par(pdf_page_height_code)
@z

@x \pdfpage{width,height}
emergency_stretch_code:print_esc("emergencystretch");
@y
emergency_stretch_code:print_esc("emergencystretch");
pdf_page_width_code:    print_esc("pdfpagewidth");
pdf_page_height_code:   print_esc("pdfpageheight");
@z

@x \[if]pdfprimitive
@!cs_count:integer; {total number of known identifiers}
@y
@!cs_count:integer; {total number of known identifiers}

@ Primitive support needs a few extra variables and definitions

@d prim_size=2100 {maximum number of primitives }
@d prim_prime=1777 {about 85\pct! of |primitive_size|}
@d prim_base=1
@d prim_next(#) == prim[#].lh {link for coalesced lists}
@d prim_text(#) == prim[#].rh {string number for control sequence name}
@d prim_is_full == (prim_used=prim_base) {test if all positions are occupied}
@d prim_eq_level_field(#)==#.hh.b1
@d prim_eq_type_field(#)==#.hh.b0
@d prim_equiv_field(#)==#.hh.rh
@d prim_eq_level(#)==prim_eq_level_field(prim_eqtb[#]) {level of definition}
@d prim_eq_type(#)==prim_eq_type_field(prim_eqtb[#]) {command code for equivalent}
@d prim_equiv(#)==prim_equiv_field(prim_eqtb[#]) {equivalent value}
@d undefined_primitive=0

@<Glob...@>=
@!prim: array [0..prim_size] of two_halves;  {the primitives table}
@!prim_used:pointer; {allocation pointer for |prim|}
@!prim_eqtb:array[0..prim_size] of memory_word;
@z

@x \[if]pdfprimitive
@ @<Set init...@>=
no_new_control_sequence:=true; {new identifiers are usually forbidden}
@y
@ @<Set init...@>=
no_new_control_sequence:=true; {new identifiers are usually forbidden}
prim_next(0):=0; prim_text(0):=0;
for k:=1 to prim_size do prim[k]:=prim[0];
prim_eq_level(0) := level_zero;
prim_eq_type(0) := undefined_cs;
prim_equiv(0) := null;
for k:=1 to prim_size do prim_eqtb[k]:=prim_eqtb[0];
@z

@x \[if]pdfprimitive
text(frozen_dont_expand):="notexpanded:";
@.notexpanded:@>
@y
prim_used:=prim_size; {nothing is used}
text(frozen_dont_expand):="notexpanded:";
@.notexpanded:@>
eq_type(frozen_primitive):=ignore_spaces;
equiv(frozen_primitive):=1;
eq_level(frozen_primitive):=level_one;
text(frozen_primitive):="pdfprimitive";
@z

@x \[if]pdfprimitive
@ Single-character control sequences do not need to be looked up in a hash
table, since we can use the character code itself as a direct address.
@y
@ Here is the subroutine that searches the primitive table for an identifier

@p function prim_lookup(@!s:str_number):pointer; {search the primitives table}
label found; {go here if you found it}
var h:integer; {hash code}
@!p:pointer; {index in |hash| array}
@!k:pointer; {index in string pool}
@!j,@!l:integer;
begin
if s<256 then begin
  p := s;
  if (p<0) or (prim_eq_level(p)<>level_one) then
    p := undefined_primitive;
end
else begin
  j:=str_start[s];
  if s = str_ptr then l := cur_length else l := length(s);
  @<Compute the primitive code |h|@>;
  p:=h+prim_base; {we start searching here; note that |0<=h<hash_prime|}
  loop@+begin if prim_text(p)>0 then if length(prim_text(p))=l then
    if str_eq_str(prim_text(p),s) then goto found;
    if prim_next(p)=0 then
      begin if no_new_control_sequence then
        p:=undefined_primitive
      else @<Insert a new primitive after |p|, then make
        |p| point to it@>;
      goto found;
      end;
    p:=prim_next(p);
    end;
  end;
found: prim_lookup:=p;
end;

@ @<Insert a new primitive...@>=
begin if prim_text(p)>0 then
  begin repeat if prim_is_full then overflow("primitive size",prim_size);
@:TeX capacity exceeded primitive size}{\quad primitive size@>
  decr(prim_used);
  until prim_text(prim_used)=0; {search for an empty location in |prim|}
  prim_next(p):=prim_used; p:=prim_used;
  end;
prim_text(p):=s;
end

@ The value of |prim_prime| should be roughly 85\pct! of
|prim_size|, and it should be a prime number.

@<Compute the primitive code |h|@>=
h:=str_pool[j];
for k:=j+1 to j+l-1 do
  begin h:=h+h+str_pool[k];
  while h>=prim_prime do h:=h-prim_prime;
  end

@ Single-character control sequences do not need to be looked up in a hash
table, since we can use the character code itself as a direct address.
@z

@x \[if]pdfprimitive
@p @!init procedure primitive(@!s:str_number;@!c:quarterword;@!o:halfword);
var k:pool_pointer; {index into |str_pool|}
@y
@p @!init procedure primitive(@!s:str_number;@!c:quarterword;@!o:halfword);
var k:pool_pointer; {index into |str_pool|}
@!prim_val:integer; {needed to fill |prim_eqtb|}
@z

@x \[if]pdfprimitive
begin if s<256 then cur_val:=s+single_base
@y
begin if s<256 then begin
  cur_val:=s+single_base;
  prim_val:=s;
end
@z

@x \[if]pdfprimitive
  flush_string; text(cur_val):=s; {we don't want to have the string twice}
  end;
eq_level(cur_val):=level_one; eq_type(cur_val):=c; equiv(cur_val):=o;
end;
tini
@y
  flush_string; text(cur_val):=s; {we don't want to have the string twice}
  prim_val:=prim_lookup(s);
  end;
eq_level(cur_val):=level_one; eq_type(cur_val):=c; equiv(cur_val):=o;
prim_eq_level(prim_val):=level_one;
prim_eq_type(prim_val):=c;
prim_equiv(prim_val):=o;
end;
tini
@z

@x \[if]pdfprimitive
ignore_spaces: print_esc("ignorespaces");
@y
ignore_spaces: if chr_code=0 then print_esc("ignorespaces") else print_esc("pdfprimitive");
@z

@x \[if]pdfprimitive
no_expand: print_esc("noexpand");
@y
no_expand: if chr_code=0 then print_esc("noexpand")
   else print_esc("pdfprimitive");
@z

@x
no_expand:@<Suppress expansion of the next token@>;
@y
no_expand: if cur_chr=0 then @<Suppress expansion of the next token@>
  else @<Implement \.{\\pdfprimitive}@>;
@z

@x
@<Suppress expansion...@>=
begin save_scanner_status:=scanner_status; scanner_status:=normal;
get_token; scanner_status:=save_scanner_status; t:=cur_tok;
back_input; {now |start| and |loc| point to the backed-up token |t|}
if t>=cs_token_flag then
  begin p:=get_avail; info(p):=cs_token_flag+frozen_dont_expand;
  link(p):=loc; start:=p; loc:=p;
  end;
end
@y
@<Suppress expansion...@>=
begin save_scanner_status:=scanner_status; scanner_status:=normal;
get_token; scanner_status:=save_scanner_status; t:=cur_tok;
back_input; {now |start| and |loc| point to the backed-up token |t|}
if t>=cs_token_flag then
  begin p:=get_avail; info(p):=cs_token_flag+frozen_dont_expand;
  link(p):=loc; start:=p; loc:=p;
  end;
end

@ The \.{\\pdfprimitive} handling. If the primitive meaning of the next
token is an expandable command, it suffices to replace the current
token with the primitive one and restart |expand|/

Otherwise, the token we just read has to be pushed back, as well
as a token matching the internal form of \.{\\pdfprimitive}, that is
sneaked in as an alternate form of |ignore_spaces|.
@!@:pdfprimitive_}{\.{\\pdfprimitive} primitive (internalized)@>

Simply pushing back a token that matches the correct internal command
does not work, because approach would not survive roundtripping to a
temporary file.

@<Implement \.{\\pdfprimitive}@>=
begin save_scanner_status := scanner_status; scanner_status:=normal;
get_token; scanner_status:=save_scanner_status;
if cur_cs < hash_base then
  cur_cs := prim_lookup(cur_cs-257)
else
  cur_cs := prim_lookup(text(cur_cs));
if cur_cs<>undefined_primitive then begin
  t := prim_eq_type(cur_cs);
  if t>max_command then begin
    cur_cmd := t;
    cur_chr := prim_equiv(cur_cs);
    cur_tok := (cur_cmd*@'400)+cur_chr;
    cur_cs  := 0;
    goto reswitch;
    end
  else begin
    back_input; { now |loc| and |start| point to a one-item list }
    p:=get_avail; info(p):=cs_token_flag+frozen_primitive;
    link(p):=loc; loc:=p; start:=p;
    end;
  end
else begin
  print_err("Missing primitive name");
  help2("The control sequence marked <to be read again> does not")@/
    ("represent any known primitive.");
  back_error;
  end;
end

@ This block deals with unexpandable \.{\\primitive} appearing at a spot where
an integer or an internal values should have been found. It fetches the
next token then resets |cur_cmd|, |cur_cs|, and |cur_tok|, based on the
primitive value of that token. No expansion takes place, because the
next token may be all sorts of things. This could trigger further
expansion creating new errors.

@<Reset |cur_tok| for unexpandable primitives, goto restart @>=
begin
get_token;
cur_cs := prim_lookup(text(cur_cs));
if cur_cs<>undefined_primitive then begin
  cur_cmd := prim_eq_type(cur_cs);
  cur_chr := prim_equiv(cur_cs);
  cur_tok := (cur_cmd*@'400)+cur_chr;
  end
else begin
  cur_cmd := relax;
  cur_chr := 0;
  cur_tok := cs_token_flag+frozen_relax;
  cur_cs  := frozen_relax;
  end;
goto restart;
end
@z

@x \[if]pdfprimitive : scan_something_internal
procedure scan_something_internal(@!level:small_number;@!negative:boolean);
  {fetch an internal parameter}
label exit;
@y
procedure scan_something_internal(@!level:small_number;@!negative:boolean);
  {fetch an internal parameter}
label exit, restart;
@z

@x \[if]pdfprimitive : scan_something_internal
begin m:=cur_chr;
@y
begin restart: m:=cur_chr;
@z

@x \[if]pdfprimitive : scan_something_internal
last_item: @<Fetch an item in the current node, if appropriate@>;
@y
last_item: @<Fetch an item in the current node, if appropriate@>;
ignore_spaces: {trap unexpandable primitives}
  if cur_chr=1 then @<Reset |cur_tok| for unexpandable primitives, goto restart@>;
@z

@x
@d badness_code=input_line_no_code+1 {code for \.{\\badness}}
@y
@d badness_code=input_line_no_code+1 {code for \.{\\badness}}
@d pdf_last_x_pos_code=badness_code+1 {code for \.{\\pdflastxpos}}
@d pdf_last_y_pos_code=pdf_last_x_pos_code+1 {code for \.{\\pdflastypos}}
@d pdf_shell_escape_code=pdf_last_y_pos_code+1 {code for \.{\\pdflastypos}}
@z

@x
@d eTeX_int=badness_code+1 {first of \eTeX\ codes for integers}
@y
@d eTeX_int=pdf_shell_escape_code+1 {first of \eTeX\ codes for integers}
@z

@x \[if]pdfprimitive: scan_int
@p procedure scan_int; {sets |cur_val| to an integer}
label done;
@y
@p procedure scan_int; {sets |cur_val| to an integer}
label done, restart;
@z

@x \[if]pdfprimitive: scan_int
if cur_tok=alpha_token then @<Scan an alphabetic character code into |cur_val|@>
@y
restart:
if cur_tok=alpha_token then @<Scan an alphabetic character code into |cur_val|@>
else if cur_tok=cs_token_flag+frozen_primitive then
  @<Reset |cur_tok| for unexpandable primitives, goto restart@>
@z

@x
@d etex_convert_codes=etex_convert_base+1 {end of \eTeX's command codes}
@y
@d pdf_strcmp_code          = etex_convert_base+1 {command code for \.{\\pdfstrcmp}}
@d pdf_creation_date_code   = etex_convert_base+2 {command code for \.{\\pdfcreationdate}}
@d pdf_file_mod_date_code   = etex_convert_base+3 {command code for \.{\\pdffilemoddate}}
@d pdf_file_size_code       = etex_convert_base+4 {command code for \.{\\pdffilesize}}
@d pdf_mdfive_sum_code      = etex_convert_base+5 {command code for \.{\\pdfmdfivesum}}
@d pdf_file_dump_code       = etex_convert_base+6 {command code for \.{\\pdffiledump}}
@d etex_convert_codes=etex_convert_base+7 {end of \eTeX's command codes}
@z

@x
  eTeX_revision_code: print_esc("eTeXrevision");
@y
  eTeX_revision_code: print_esc("eTeXrevision");
  pdf_strcmp_code:        print_esc("pdfstrcmp");
  pdf_creation_date_code: print_esc("pdfcreationdate");
  pdf_file_mod_date_code: print_esc("pdffilemoddate");
  pdf_file_size_code:     print_esc("pdffilesize");
  pdf_mdfive_sum_code:    print_esc("pdfmdfivesum");
  pdf_file_dump_code:     print_esc("pdffiledump");
@z

@x
@p procedure conv_toks;
@y

The extra temp string |u| is needed because |pdf_scan_ext_toks| incorporates
any pending string in its output. In order to save such a pending string,
we have to create a temporary string that is destroyed immediately after.

@d save_cur_string==if str_start[str_ptr]<pool_ptr then u:=make_string else u:=0
@d restore_cur_string==if u<>0 then decr(str_ptr)

@p procedure conv_toks;
@z

@x
@!save_scanner_status:small_number; {|scanner_status| upon entry}
@y
@!save_scanner_status:small_number; {|scanner_status| upon entry}
@!save_def_ref: pointer; {|def_ref| upon entry, important if inside `\.{\\message}'}
@!save_warning_index: pointer;
@!bool: boolean; {temp boolean}
@!u: str_number; {saved current string string}
@!s: str_number; {first temp string}
@!i: integer;
@!j: integer;
@z

@x
begin c:=cur_chr; @<Scan the argument for command |c|@>;
@y
begin c:=cur_chr; @<Scan the argument for command |c|@>;
u:=0; { will become non-nil if a string is already being built}
@z

@x
eTeX_revision_code: do_nothing;
@y
eTeX_revision_code: do_nothing;
pdf_strcmp_code:
  begin
    save_scanner_status := scanner_status;
    save_def_ref := def_ref;
    save_cur_string;
    compare_strings;
    def_ref := save_def_ref;
    scanner_status := save_scanner_status;
    restore_cur_string;
  end;
pdf_creation_date_code:
  begin
    b := pool_ptr;
    getcreationdate;
    link(garbage) := str_toks(b);
    ins_list(link(temp_head));
    return;
  end;
pdf_file_mod_date_code:
  begin
    save_scanner_status := scanner_status;
    save_warning_index := warning_index;
    save_def_ref := def_ref;
    save_cur_string;
    scan_pdf_ext_toks;
    s := tokens_to_string(def_ref);
    delete_token_ref(def_ref);
    def_ref := save_def_ref;
    warning_index := save_warning_index;
    scanner_status := save_scanner_status;
    b := pool_ptr;
    getfilemoddate(s);
    link(garbage) := str_toks(b);
    flush_str(s);
    ins_list(link(temp_head));
    restore_cur_string;
    return;
  end;
pdf_file_size_code:
  begin
    save_scanner_status := scanner_status;
    save_warning_index := warning_index;
    save_def_ref := def_ref;
    save_cur_string;
    scan_pdf_ext_toks;
    s := tokens_to_string(def_ref);
    delete_token_ref(def_ref);
    def_ref := save_def_ref;
    warning_index := save_warning_index;
    scanner_status := save_scanner_status;
    b := pool_ptr;
    getfilesize(s);
    link(garbage) := str_toks(b);
    flush_str(s);
    ins_list(link(temp_head));
    restore_cur_string;
    return;
  end;
pdf_mdfive_sum_code:
  begin
    save_scanner_status := scanner_status;
    save_warning_index := warning_index;
    save_def_ref := def_ref;
    save_cur_string;
    bool := scan_keyword("file");
    scan_pdf_ext_toks;
    if bool then s := tokens_to_string(def_ref)
    else begin
      isprint_utf8:=true; s := tokens_to_string(def_ref); isprint_utf8:=false;
    end;
    delete_token_ref(def_ref);
    def_ref := save_def_ref;
    warning_index := save_warning_index;
    scanner_status := save_scanner_status;
    b := pool_ptr;
    getmd5sum(s, bool);
    link(garbage) := str_toks(b);
    flush_str(s);
    ins_list(link(temp_head));
    restore_cur_string;
    return;
  end;
pdf_file_dump_code:
  begin
    save_scanner_status := scanner_status;
    save_warning_index := warning_index;
    save_def_ref := def_ref;
    save_cur_string;
    {scan offset}
    cur_val := 0;
    if (scan_keyword("offset")) then begin
      scan_int;
      if (cur_val < 0) then begin
        print_err("Bad file offset");
@.Bad file offset@>
        help2("A file offset must be between 0 and 2^{31}-1,")@/
          ("I changed this one to zero.");
        int_error(cur_val);
        cur_val := 0;
      end;
    end;
    i := cur_val;
    {scan length}
    cur_val := 0;
    if (scan_keyword("length")) then begin
      scan_int;
      if (cur_val < 0) then begin
        print_err("Bad dump length");
@.Bad dump length@>
        help2("A dump length must be between 0 and 2^{31}-1,")@/
          ("I changed this one to zero.");
        int_error(cur_val);
        cur_val := 0;
      end;
    end;
    j := cur_val;
    {scan file name}
    scan_pdf_ext_toks;
    s := tokens_to_string(def_ref);
    delete_token_ref(def_ref);
    def_ref := save_def_ref;
    warning_index := save_warning_index;
    scanner_status := save_scanner_status;
    b := pool_ptr;
    getfiledump(s, i, j);
    link(garbage) := str_toks(b);
    flush_str(s);
    ins_list(link(temp_head));
    restore_cur_string;
    return;
  end;
@z

@x
eTeX_revision_code: print(eTeX_revision);
@y
eTeX_revision_code: print(eTeX_revision);
pdf_strcmp_code: print_int(cur_val);
@z

@x \[if]pdfprimitive
@d if_dbox_code=if_ybox_code+1 { `\.{\\ifdbox}' }
@y
@d if_dbox_code=if_ybox_code+1 { `\.{\\ifdbox}' }
@#
@d if_pdfprimitive_code=if_dbox_code+1 { `\.{\\ifpdfprimitive}' }
@z

@x \[if]pdfprimitive
  if_dbox_code:print_esc("ifdbox");
@y
  if_dbox_code:print_esc("ifdbox");
  if_pdfprimitive_code:print_esc("ifpdfprimitive");
@z

@x \[if]pdfprimitive
if_void_code, if_hbox_code, if_vbox_code, if_tbox_code, if_ybox_code, if_dbox_code:
  @<Test box register status@>;
@y
if_void_code, if_hbox_code, if_vbox_code, if_tbox_code, if_ybox_code, if_dbox_code:
  @<Test box register status@>;
if_pdfprimitive_code: begin
  save_scanner_status:=scanner_status;
  scanner_status:=normal;
  get_next;
  scanner_status:=save_scanner_status;
  if cur_cs < hash_base then
    m := prim_lookup(cur_cs-257)
  else
    m := prim_lookup(text(cur_cs));
  b :=((cur_cmd<>undefined_cs) and
       (m<>undefined_primitive) and
       (cur_cmd=prim_eq_type(m)) and
       (cur_chr=prim_equiv(m)));
  end;
@z

@x
@ @<Initialize variables as |ship_out| begins@>=
@y
@ @<Initialize variables as |ship_out| begins@>=
@<Calculate DVI page dimensions and margins@>;
@z

@x
@!old_setting:0..max_selector; {saved |selector| setting}
@y
@!old_setting:0..max_selector; {saved |selector| setting}
@!t: scaled;
@z

@x \[if]pdfprimitive: main_loop
any_mode(ignore_spaces): begin @<Get the next non-blank non-call...@>;
  goto reswitch;
  end;
@y
any_mode(ignore_spaces): begin
  if cur_chr = 0 then begin
    @<Get the next non-blank non-call...@>;
    goto reswitch;
  end
  else begin
    t:=scanner_status;
    scanner_status:=normal;
    get_next;
    scanner_status:=t;
    if cur_cs < hash_base then
      cur_cs := prim_lookup(cur_cs-257)
    else
      cur_cs  := prim_lookup(text(cur_cs));
    if cur_cs<>undefined_primitive then begin
      cur_cmd := prim_eq_type(cur_cs);
      cur_chr := prim_equiv(cur_cs);
      goto reswitch;
      end;
    end;
  end;
@z

@x \[if]pdfprimitive: dump prim table
@<Dump the hash table@>=
@y
@<Dump the hash table@>=
for p:=0 to prim_size do dump_hh(prim[p]);
for p:=0 to prim_size do dump_wd(prim_eqtb[p]);
@z

@x \[if]pdfprimitive: undump prim table
@ @<Undump the hash table@>=
@y
@ @<Undump the hash table@>=
for p:=0 to prim_size do undump_hh(prim[p]);
for p:=0 to prim_size do undump_wd(prim_eqtb[p]);
@z

@x
fix_date_and_time;@/
@y
fix_date_and_time;@/
isprint_utf8:=false;
@z

@x
@d set_language_code=5 {command modifier for \.{\\setlanguage}}
@y
@d set_language_code=5 {command modifier for \.{\\setlanguage}}
@d TeX_extension_end=5
@d pdf_save_pos_node=TeX_extension_end+1
@z

@x
  set_language_code:print_esc("setlanguage");
@y
  set_language_code:print_esc("setlanguage");
  pdf_save_pos_node: print_esc("pdfsavepos");
@z

@x
set_language_code:@<Implement \.{\\setlanguage}@>;
@y
set_language_code:@<Implement \.{\\setlanguage}@>;
pdf_save_pos_node: @<Implement \.{\\pdfsavepos}@>;
@z

@x \pdfsavepos
  print_int(what_lhm(p)); print_char(",");
  print_int(what_rhm(p)); print_char(")");
  end;
@y
  print_int(what_lhm(p)); print_char(",");
  print_int(what_rhm(p)); print_char(")");
  end;
pdf_save_pos_node: print_esc("pdfsavepos");
@z

@x \pdfsavepos
close_node,language_node: begin r:=get_node(small_node_size);
  words:=small_node_size;
  end;
@y
close_node,language_node: begin r:=get_node(small_node_size);
  words:=small_node_size;
  end;
pdf_save_pos_node:
   r := get_node(small_node_size);
@z

@x \pdfsavepos
close_node,language_node: free_node(p,small_node_size);
@y
close_node,language_node: free_node(p,small_node_size);
pdf_save_pos_node: free_node(p, small_node_size);
@z

@x
procedure special_out(@!p:pointer);
var old_setting:0..max_selector; {holds print |selector|}
@!k:pool_pointer; {index into |str_pool|}
@y
procedure special_out(@!p:pointer);
label done;
var old_setting:0..max_selector; {holds print |selector|}
@!k:pool_pointer; {index into |str_pool|}
@!s,@!t,@!cw: scaled;
@!bl: boolean;
@!i: small_number;
@z

@x
pool_ptr:=str_start[str_ptr]; {erase the string}
@y
@<Determine whether this \.{\\special} is a papersize special@>;
done: pool_ptr:=str_start[str_ptr]; {erase the string}
@z

@x
language_node:do_nothing;
@y
language_node:do_nothing;
pdf_save_pos_node:
  @<Save current position in DVI mode@>;
@z

@x
primitive("eTeXrevision",convert,eTeX_revision_code);@/
@!@:eTeX_revision_}{\.{\\eTeXrevision} primitive@>
@y
primitive("eTeXrevision",convert,eTeX_revision_code);@/
@!@:eTeX_revision_}{\.{\\eTeXrevision} primitive@>
primitive("pdfprimitive",no_expand,1);@/
@!@:pdfprimitive_}{\.{\\pdfprimitive} primitive@>
primitive("pdfstrcmp",convert,pdf_strcmp_code);@/
@!@:pdf_strcmp_}{\.{\\pdfstrcmp} primitive@>
primitive("pdfcreationdate",convert,pdf_creation_date_code);@/
@!@:pdf_creation_date_}{\.{\\pdfcreationdate} primitive@>
primitive("pdffilemoddate",convert,pdf_file_mod_date_code);@/
@!@:pdf_file_mod_date_}{\.{\\pdffilemoddate} primitive@>
primitive("pdffilesize",convert,pdf_file_size_code);@/
@!@:pdf_file_size_}{\.{\\pdffilesize} primitive@>
primitive("pdfmdfivesum",convert,pdf_mdfive_sum_code);@/
@!@:pdf_mdfive_sum_}{\.{\\pdfmdfivesum} primitive@>
primitive("pdffiledump",convert,pdf_file_dump_code);@/
@!@:pdf_file_dump_}{\.{\\pdffiledump} primitive@>
primitive("pdfsavepos",extension,pdf_save_pos_node);@/
@!@:pdf_save_pos_}{\.{\\pdfsavepos} primitive@>
primitive("pdfpagewidth",assign_dimen,dimen_base+pdf_page_width_code);@/
@!@:pdf_page_width_}{\.{\\pdfpagewidth} primitive@>
primitive("pdfpageheight",assign_dimen,dimen_base+pdf_page_height_code);@/
@!@:pdf_page_height_}{\.{\\pdfpageheight} primitive@>
primitive("pdflastxpos",last_item,pdf_last_x_pos_code);@/
@!@:pdf_last_x_pos_}{\.{\\pdflastxpos} primitive@>
primitive("pdflastypos",last_item,pdf_last_y_pos_code);@/
@!@:pdf_last_y_pos_}{\.{\\pdflastypos} primitive@>
primitive("pdfshellescape",last_item,pdf_shell_escape_code);
@!@:pdf_shell_escape_}{\.{\\pdfshellescape} primitive@>
primitive("ifpdfprimitive",if_test,if_pdfprimitive_code);
@!@:if_pdfprimitive_}{\.{\\ifpdfprimitive} primitive@>
@z

@x
eTeX_version_code: print_esc("eTeXversion");
@y
eTeX_version_code: print_esc("eTeXversion");
pdf_last_x_pos_code:  print_esc("pdflastxpos");
pdf_last_y_pos_code:  print_esc("pdflastypos");
pdf_shell_escape_code: print_esc("pdfshellescape");
@z

@x
eTeX_version_code: cur_val:=eTeX_version;
@y
eTeX_version_code: cur_val:=eTeX_version;
pdf_last_x_pos_code: cur_val := pdf_last_x_pos;
pdf_last_y_pos_code: cur_val := pdf_last_y_pos;
pdf_shell_escape_code:
  begin
  if shellenabledp then begin
    if restrictedshell then cur_val :=2
    else cur_val := 1;
  end
  else cur_val := 0;
  end;
@z

@x
procedure print_kanji(@!s:KANJI_code); {prints a single character}
begin
if s>255 then
  begin print_char(Hi(s)); print_char(Lo(s));
  end else print_char(s);
end;
@y
procedure print_kanji(@!s:integer); {prints a single character}
begin
if s>255 then begin
  if isprint_utf8 then begin
    s:=UCStoUTF8(toUCS(s));
    if BYTE1(s)<>0 then print_char(BYTE1(s));
    if BYTE2(s)<>0 then print_char(BYTE2(s));
    if BYTE3(s)<>0 then print_char(BYTE3(s));
                        print_char(BYTE4(s));
  end
  else begin print_char(Hi(s)); print_char(Lo(s)); end;
end
else print_char(s);
end;


@z

@x
@* \[54] System-dependent changes.
@y
@* \[54/pdf\TeX] System-dependent changes for {\tt\char"5Cpdfstrcmp}.
@d call_func(#) == begin if # <> 0 then do_nothing end
@d flushable(#) == (# = str_ptr - 1)

@<Glob...@>=
@!isprint_utf8: boolean;

@ @<Declare procedures that need to be declared forward for \pdfTeX@>=
procedure pdf_error(t, p: str_number);
begin
    normalize_selector;
    print_err("pdfTeX error");
    if t <> 0 then begin
        print(" (");
        print(t);
        print(")");
    end;
    print(": "); print(p);
    succumb;
end;

@ @<Declare procedures needed in |do_ext...@>=

procedure compare_strings; {to implement \.{\\pdfstrcmp}}
label done;
var s1, s2: str_number;
    i1, i2, j1, j2: pool_pointer;
begin
    call_func(scan_toks(false, true));
    isprint_utf8:=true; s1 := tokens_to_string(def_ref); isprint_utf8:=false;
    delete_token_ref(def_ref);
    call_func(scan_toks(false, true));
    isprint_utf8:=true; s2 := tokens_to_string(def_ref); isprint_utf8:=false;
    delete_token_ref(def_ref);
    i1 := str_start[s1];
    j1 := str_start[s1 + 1];
    i2 := str_start[s2];
    j2 := str_start[s2 + 1];
    while (i1 < j1) and (i2 < j2) do begin
        if str_pool[i1] < str_pool[i2] then begin
            cur_val := -1;
            goto done;
        end;
        if str_pool[i1] > str_pool[i2] then begin
            cur_val := 1;
            goto done;
        end;
        incr(i1);
        incr(i2);
    end;
    if (i1 = j1) and (i2 = j2) then
        cur_val := 0
    else if i1 < j1 then
        cur_val := 1
    else
        cur_val := -1;
done:
    flush_str(s2);
    flush_str(s1);
    cur_val_level := int_val;
end;

@ Next, we implement \.{\\pdfsavepos} and related primitives.

@<Glob...@>=
@!cur_page_width: scaled; {width of page being shipped}
@!cur_page_height: scaled; {height of page being shipped}
@!pdf_last_x_pos: integer;
@!pdf_last_y_pos: integer;

@ @<Implement \.{\\pdfsavepos}@>=
begin
    new_whatsit(pdf_save_pos_node, small_node_size);
end

@ @<Save current position in DVI mode@>=
begin
  case dvi_dir of
  dir_yoko: begin pdf_last_x_pos := cur_h;  pdf_last_y_pos := cur_v;  end;
  dir_tate: begin pdf_last_x_pos := -cur_v; pdf_last_y_pos := cur_h;  end;
  dir_dtou: begin pdf_last_x_pos := cur_v;  pdf_last_y_pos := -cur_h; end;
  endcases;
  pdf_last_x_pos := pdf_last_x_pos + 4736286;
  case dvi_dir of
  dir_tate,dir_dtou:
    pdf_last_y_pos := cur_page_width - pdf_last_y_pos - 4736286;
  dir_yoko:
    pdf_last_y_pos := cur_page_height - pdf_last_y_pos - 4736286;
  endcases;
  {4736286 = 1in, the funny DVI origin offset}
end

@ @<Calculate DVI page dimensions and margins@>=
  if pdf_page_height <> 0 then
    cur_page_height := pdf_page_height
  else
    cur_page_height := height(p) + depth(p) + 2*v_offset + 2*4736286;
  if pdf_page_width <> 0 then
    cur_page_width := pdf_page_width
  else
    cur_page_width := width(p) + 2*h_offset + 2*4736286;
  if (type(p)=dir_node) then
    if (box_dir(list_ptr(p))=dir_tate)or(box_dir(list_ptr(p))=dir_dtou) then begin
      t:=cur_page_height; cur_page_height:=cur_page_width;
      cur_page_width:=t; end;


@ Of course \epTeX\ can produce a \.{DVI} file only, not a PDF file.
A \.{DVI} file does not have the information of the page height,
which is needed to implement \.{\\pdflastypos} correctly.
To keep the information of the page height, I (H.~Kitagawa)
adopted \.{\\pdfpageheight} primitive from pdf\TeX. However, in p\TeX (and \hbox{\epTeX}),
the papersize special \.{\\special\{papersize=|width|,|height|\}} is commonly used
for specifying page width/height. Hence, I decided that the papersize special also
changes the value of \.{\\pdfpagewidth} and \.{\\pdfpageheight}.
The following routine does this.

In present implementation, the ``valid'' papersize special, which can be interpreted by
this routine, is in the following form:
$$\hbox{\.{\\special\char"7Bpapersize=}$x$\.{pt,}$y$\.{pt\char"7D}}$$
where $x$\thinspace pt and $y$\thinspace pt are positive dimensions which \TeX\ can comprehend.
No spaces are allowed in the above form, and only ``pt'' is allowed for unit, for simplicity.

@d ifps==if k=pool_ptr then goto done else if
@d sop(#)==so(str_pool[(#)])

@<Determine whether this \.{\\special} is a papersize special@>=
if pool_ptr-str_start[str_ptr]<=10 then goto done;
k:=str_start[str_ptr];
if (sop(k+0)<>'p')or(sop(k+1)<>'a')or(sop(k+2)<>'p')or
   (sop(k+3)<>'e')or(sop(k+4)<>'r')or(sop(k+5)<>'s')or
   (sop(k+6)<>'i')or(sop(k+7)<>'z')or(sop(k+8)<>'e') then goto done;
k:=k+9; ifps sop(k)='=' then incr(k);
@<Read dimensions in the argument in the papersize special@>;
incr(k); ifps sop(k)<>',' then goto done else incr(k); cw:=t;
@<Read dimensions in the argument in the papersize special@>;
geq_word_define(dimen_base+pdf_page_width_code,cw);
geq_word_define(dimen_base+pdf_page_height_code,t);
cur_page_height := t; cur_page_width := cw;
if (dvi_dir=dir_tate)or(dvi_dir=dir_dtou) then begin
    t:=cur_page_height; cur_page_height:=cur_page_width;
    cur_page_width:=t; end;

@ @<Read dimensions in the argument in the papersize special@>=
s:=1; t:=0; bl:=true;
while (k<pool_ptr)and(bl)  do begin
  if (sop(k)>='0')and (sop(k)<='9') then begin t:=10*t+sop(k)-'0'; incr(k); end
  else begin bl:=false; end;
end;
t:=t*unity;
ifps sop(k)='.' then begin incr(k); bl:=true; i:=0;
  while (k<pool_ptr)and(bl)and(i<=17)  do begin
    if (sop(k)>='0')and (sop(k)<='9') then begin
      dig[i]:=sop(k)-'0'; incr(k); incr(i); end
    else begin bl:=false; incr(k); incr(i); end;
  end;
  t:=s*(t+round_decimals(i-1));
end
else if (sop(k)>='0')and(sop(k)<='9') then goto done
else begin t:=s*t; incr(k); end;
ifps sop(k-1)<>'p' then goto done; ifps sop(k)<>'t' then goto done;

@ Finally, we declare some routine needed for \.{\\pdffilemoddate}.

@<Glob...@>=
@!isprint_utf8: boolean;
@!last_tokens_string: str_number; {the number of the most recently string
created by |tokens_to_string|}

@ @<Declare procedures needed in |do_ext...@>=
procedure scan_pdf_ext_toks;
begin
    call_func(scan_toks(false, true)); {like \.{\\special}}
end;

@ @<Declare procedures that need to be declared forward for \pdfTeX@>=
function tokens_to_string(p: pointer): str_number; {return a string from tokens
list}
begin
    if selector = new_string then
        pdf_error("tokens", "tokens_to_string() called while selector = new_string");
    old_setting:=selector; selector:=new_string;
    show_token_list(link(p),null,pool_size-pool_ptr);
    selector:=old_setting;
    last_tokens_string := make_string;
    tokens_to_string := last_tokens_string;
end;
procedure flush_str(s: str_number); {flush a string if possible}
begin
    if flushable(s) then
        flush_string;
end;

@* \[54] System-dependent changes.
@z
