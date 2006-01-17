% Copyright (c) 2005 Han Th\^e\llap{\raise 0.5ex\hbox{\'{}}} Th\`anh, <thanh@pdftex.org>
%
% This file is part of pdfTeX.
%
% pdfTeX is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2 of the License, or
% (at your option) any later version.
%
% pdfTeX is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with pdfTeX; if not, write to the Free Software
% Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
%
% $Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/pdftex.ch#163 $
%
% experimental patch to make \pdfstrcmp expandable
%
% put it after tex.pch
%
% x line numbers of file pdfetex-org.web before patch
%
%***********************************************************************

@x 9388
@d pdftex_last_item_codes     = pdftex_first_rint_code + 10 {end of \pdfTeX's command codes}
@y
@d pdf_shell_escape_code      = pdftex_first_rint_code + 11 {code for \.{\\pdfshellescape}}
@d pdftex_last_item_codes     = pdftex_first_rint_code + 11 {end of \pdfTeX's command codes}
@z

@x 9442
primitive("pdfelapsedtime",last_item,elapsed_time_code);
@!@:elapsed_time_}{\.{\\pdfelapsedtime} primitive@>
@y
primitive("pdfelapsedtime",last_item,elapsed_time_code);
@!@:elapsed_time_}{\.{\\pdfelapsedtime} primitive@>
primitive("pdfshellescape",last_item,pdf_shell_escape_code);
@!@:elapsed_time_}{\.{\\pdfshellescape} primitive@>
@z

@x 9470
  elapsed_time_code: print_esc("pdfelapsedtime");
@y
  elapsed_time_code: print_esc("pdfelapsedtime");
  pdf_shell_escape_code: print_esc("pdfshellescape");
@z

@x 9561
  elapsed_time_code: cur_val := get_microinterval;
@y
  elapsed_time_code: cur_val := get_microinterval;
  pdf_shell_escape_code:
    begin
    if shell_enabled_p then cur_val := 1
    else cur_val := 0;
    end;
@z    

@x 10332
@d pdf_last_escaped_string_code = pdftex_first_expand_code + 7 {command code for \.{\\pdflastescapedstring}}
@d pdf_last_escaped_name_code = pdftex_first_expand_code + 8 {command code for \.{\\pdflastescapename}}
@y
@d pdf_escape_string_code   = pdftex_first_expand_code + 7 {command code for \.{\\pdfescapestring}}
@d pdf_escape_name_code     = pdftex_first_expand_code + 8 {command code for \.{\\pdfescapename}}
@z

@x 10336
@d pdftex_convert_codes     = pdftex_first_expand_code + 11 {end of \pdfTeX's command codes}
@y
@d pdf_strcmp_code          = pdftex_first_expand_code + 11 {command code for \.{\\pdfstrcmp}}
@d pdf_escape_hex_code      = pdftex_first_expand_code + 12 {command code for \.{\\pdfescapehex}}
@d pdf_unescape_hex_code    = pdftex_first_expand_code + 13 {command code for \.{\\pdfunescapehex}}
@d pdf_creation_date_code   = pdftex_first_expand_code + 14 {command code for \.{\\pdfcreationdate}}
@d pdf_file_mod_date_code   = pdftex_first_expand_code + 15 {command code for \.{\\pdffilemoddate}}
@d pdf_file_size_code       = pdftex_first_expand_code + 16 {command code for \.{\\pdffilesize}}
@d pdf_mdfive_sum_code      = pdftex_first_expand_code + 17 {command code for \.{\\pdfmdfivesum}}
@d pdf_file_dump_code       = pdftex_first_expand_code + 18 {command code for \.{\\pdffiledump}}
@d pdf_match_code           = pdftex_first_expand_code + 19 {command code for \.{\\pdfmatch}}
@d pdf_last_match_code      = pdftex_first_expand_code + 20 {command code for \.{\\pdflastmatch}}
@d pdftex_convert_codes     = pdftex_first_expand_code + 21 {end of \pdfTeX's command codes}
@z

%***********************************************************************

@x 10369
primitive("pdflastescapedstring",convert,pdf_last_escaped_string_code);@/
@!@:pdf_last_escaped_string_}{\.{\\pdflastescapedstring} primitive@>
primitive("pdflastescapedname",convert,pdf_last_escaped_name_code);@/
@!@:pdf_last_escaped_name_}{\.{\\pdflastescapedname} primitive@>
@y
primitive("pdfescapestring",convert,pdf_escape_string_code);@/
@!@:pdf_escape_string_}{\.{\\pdfescapestring} primitive@>
primitive("pdfescapename",convert,pdf_escape_name_code);@/
@!@:pdf_escape_name_}{\.{\\pdfescapename} primitive@>
primitive("pdfescapehex",convert,pdf_escape_hex_code);@/
@!@:pdf_escape_hex_}{\.{\\pdfescapehex} primitive@>
primitive("pdfunescapehex",convert,pdf_unescape_hex_code);@/
@!@:pdf_unescape_hex_}{\.{\\pdfunescapehex} primitive@>
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
primitive("pdfmatch",convert,pdf_match_code);@/
@!@:pdf_match_}{\.{\\pdfmatch} primitive@>
primitive("pdflastmatch",convert,pdf_last_match_code);@/
@!@:pdf_last_match_}{\.{\\pdflastmatch} primitive@>
primitive("pdfstrcmp",convert,pdf_strcmp_code);@/
@!@:pdf_strcmp_}{\.{\\pdfstrcmp} primitive@>
@z

%***********************************************************************

@x 10394
  pdf_last_escaped_string_code:    print_esc("pdflastescapedstring");
  pdf_last_escaped_name_code:      print_esc("pdflastescapedname");
@y
  pdf_escape_string_code: print_esc("pdfescapestring");
  pdf_escape_name_code:   print_esc("pdfescapename");
  pdf_escape_hex_code:    print_esc("pdfescapehex");
  pdf_unescape_hex_code:  print_esc("pdfunescapehex");
  pdf_creation_date_code: print_esc("pdfcreationdate");
  pdf_file_mod_date_code: print_esc("pdffilemoddate");
  pdf_file_size_code:     print_esc("pdffilesize");
  pdf_mdfive_sum_code:    print_esc("pdfmdfivesum");
  pdf_file_dump_code:     print_esc("pdffiledump");
  pdf_match_code:         print_esc("pdfmatch");
  pdf_last_match_code:    print_esc("pdflastmatch");
  pdf_strcmp_code:        print_esc("pdfstrcmp");
@z

%***********************************************************************

@x 10403
@p procedure conv_toks;
var old_setting:0..max_selector; {holds |selector| setting}
p, q: pointer;
@!c:number_code..job_name_code; {desired type of conversion}
@!save_scanner_status:small_number; {|scanner_status| upon entry}
@!b:pool_pointer; {base of temporary string}
@!i, l: integer; {index to access escaped string or name}
begin c:=cur_chr; @<Scan the argument for command |c|@>;
old_setting:=selector; selector:=new_string; b:=pool_ptr;
@<Print the result of command |c|@>;
selector:=old_setting; link(garbage):=str_toks(b); ins_list(link(temp_head));
end;
@y
@p procedure conv_toks;
label exit;
var old_setting:0..max_selector; {holds |selector| setting}
p, q: pointer;
@!c:number_code..job_name_code; {desired type of conversion}
@!save_scanner_status:small_number; {|scanner_status| upon entry}
@!save_def_ref: pointer; {|def_ref| upon entry, important if inside `\.{\\message}'}
@!save_warning_index: pointer;
@!bool: boolean; {temp boolean}
@!i: integer; {first temp integer}
@!j: integer; {second temp integer}
@!b:pool_pointer; {base of temporary string}
@!s: str_number; {first temp string}
@!t: str_number; {second temp string}
begin c:=cur_chr; @<Scan the argument for command |c|@>;
old_setting:=selector; selector:=new_string; b:=pool_ptr;
@<Print the result of command |c|@>;
selector:=old_setting; link(garbage):=str_toks(b); ins_list(link(temp_head));
exit:end;
@z

@x 10450
pdf_last_escaped_string_code: do_nothing;
pdf_last_escaped_name_code: do_nothing;
job_name_code: if job_name=0 then open_log_file;
@y
pdf_escape_string_code:
  begin
    save_scanner_status := scanner_status;
    save_warning_index := warning_index;
    save_def_ref := def_ref;
    scan_pdf_ext_toks;
    s := tokens_to_string(def_ref);
    delete_token_ref(def_ref);
    def_ref := save_def_ref;
    warning_index := save_warning_index;
    scanner_status := save_scanner_status;
    b := pool_ptr;
    escapestring(str_start[s]);
    link(garbage) := str_toks(b);
    flush_str(s);
    ins_list(link(temp_head));
    return;
  end;
pdf_escape_name_code:
  begin
    save_scanner_status := scanner_status;
    save_warning_index := warning_index;
    save_def_ref := def_ref;
    scan_pdf_ext_toks;
    s := tokens_to_string(def_ref);
    delete_token_ref(def_ref);
    def_ref := save_def_ref;
    warning_index := save_warning_index;
    scanner_status := save_scanner_status;
    b := pool_ptr;
    escapename(str_start[s]);
    link(garbage) := str_toks(b);
    flush_str(s);
    ins_list(link(temp_head));
    return;
  end;
pdf_escape_hex_code:
  begin
    save_scanner_status := scanner_status;
    save_warning_index := warning_index;
    save_def_ref := def_ref;
    scan_pdf_ext_toks;
    s := tokens_to_string(def_ref);
    delete_token_ref(def_ref);
    def_ref := save_def_ref;
    warning_index := save_warning_index;
    scanner_status := save_scanner_status;
    b := pool_ptr;
    escapehex(str_start[s]);
    link(garbage) := str_toks(b);
    flush_str(s);
    ins_list(link(temp_head));
    return;
  end;
pdf_unescape_hex_code:
  begin
    save_scanner_status := scanner_status;
    save_warning_index := warning_index;
    save_def_ref := def_ref;
    scan_pdf_ext_toks;
    s := tokens_to_string(def_ref);
    delete_token_ref(def_ref);
    def_ref := save_def_ref;
    warning_index := save_warning_index;
    scanner_status := save_scanner_status;
    b := pool_ptr;
    unescapehex(str_start[s]);
    link(garbage) := str_toks(b);
    flush_str(s);
    ins_list(link(temp_head));
    return;
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
    return;
  end;
pdf_file_size_code:
  begin
    save_scanner_status := scanner_status;
    save_warning_index := warning_index;
    save_def_ref := def_ref;
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
    return;
  end;
pdf_mdfive_sum_code:
  begin
    save_scanner_status := scanner_status;
    save_warning_index := warning_index;
    save_def_ref := def_ref;
    bool := scan_keyword("file");
    scan_pdf_ext_toks;
    s := tokens_to_string(def_ref);
    delete_token_ref(def_ref);
    def_ref := save_def_ref;
    warning_index := save_warning_index;
    scanner_status := save_scanner_status;
    b := pool_ptr;
    getmd5sum(s, bool);
    link(garbage) := str_toks(b);
    flush_str(s);
    ins_list(link(temp_head));
    return;
  end;
pdf_file_dump_code:
  begin
    save_scanner_status := scanner_status;
    save_warning_index := warning_index;
    save_def_ref := def_ref;
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
    return;
  end;
pdf_match_code:
  begin
    save_scanner_status := scanner_status;
    save_warning_index := warning_index;
    save_def_ref := def_ref;
    {scan for icase}
    bool := scan_keyword("icase");
    {scan for subcount}
    i := -1; {default for subcount}
    if scan_keyword("subcount") then begin
      scan_int;
      i := cur_val;
    end;
    scan_pdf_ext_toks;
    s := tokens_to_string(def_ref);
    delete_token_ref(def_ref);
    scan_pdf_ext_toks;
    t := tokens_to_string(def_ref);
    delete_token_ref(def_ref);
    def_ref := save_def_ref;
    warning_index := save_warning_index;
    scanner_status := save_scanner_status;
    b := pool_ptr;
    matchstrings(s, t, i, bool);
    link(garbage) := str_toks(b);
    flush_str(t);
    flush_str(s);
    ins_list(link(temp_head));
    return;
  end;
pdf_last_match_code:
  begin
    scan_int;
    if cur_val < 0 then begin
      print_err("Bad match number");
@.Bad match number@>
      help2("Since I expected zero or a positive number,")@/
      ("I changed this one to zero.");
      int_error(cur_val);
      cur_val := 0;
    end;
    b := pool_ptr;
    getmatch(cur_val);
    link(garbage) := str_toks(b);
    ins_list(link(temp_head));
    return;
  end;
pdf_strcmp_code:
  begin
    save_scanner_status := scanner_status;
    save_warning_index := warning_index;
    save_def_ref := def_ref;
    compare_strings;
    def_ref := save_def_ref;
    warning_index := save_warning_index;
    scanner_status := save_scanner_status;
  end;
job_name_code: if job_name=0 then open_log_file;
@z

%***********************************************************************

@x 10512
pdf_last_escaped_string_code: begin
    l := escapedstrlen;
    for i := 0 to l - 1 do
        print_char(getescapedstrchar(i));
end;
pdf_last_escaped_name_code: begin
    l := escapednamelen;
    for i := 0 to l - 1 do
        print_char(getescapednamechar(i));
end;
job_name_code: print(job_name);
@y
pdf_strcmp_code: print_int(cur_val);
job_name_code: print(job_name);
@z

%***********************************************************************

@x 32168
@d pdf_strcmp_code             == pdftex_first_extension_code + 24
@d pdf_escape_string_code      == pdftex_first_extension_code + 25
@d pdf_escape_name_code        == pdftex_first_extension_code + 26
@d reset_timer_code            == pdftex_first_extension_code + 27
@d pdf_font_expand_code        == pdftex_first_extension_code + 28
@d pdftex_last_extension_code  == pdftex_first_extension_code + 28
@y
@d reset_timer_code            == pdftex_first_extension_code + 24
@d pdf_font_expand_code        == pdftex_first_extension_code + 25
@d pdftex_last_extension_code  == pdftex_first_extension_code + 25
@z

%***********************************************************************

@x 32239
primitive("pdfstrcmp",extension,pdf_strcmp_code);@/
@!@:pdf_strcmp_}{\.{\\pdfstrcmp} primitive@>
primitive("pdfescapestring",extension,pdf_escape_string_code);@/
@!@:pdf_escape_string_}{\.{\\pdfescapestring} primitive@>
primitive("pdfescapename",extension,pdf_escape_name_code);@/
@!@:pdf_escape_name_}{\.{\\pdfescapename} primitive@>
@y
@z

%***********************************************************************

@x 32287
  pdf_strcmp_code: print_esc("pdfstrcmp");
  pdf_escape_string_code: print_esc("pdfescapestring");
  pdf_escape_name_code: print_esc("pdfescapename");
@y
@z

%***********************************************************************

@x 32337
pdf_strcmp_code: @<Implement \.{\\pdfstrcmp}@>;
pdf_escape_string_code: @<Implement \.{\\pdfescapestring}@>;
pdf_escape_name_code: @<Implement \.{\\pdfescapename}@>;
@y
@z

%***********************************************************************

@x 32463
procedure compare_strings; {to implement \.{\\pdfstrcmp}}
label done;
var s1, s2: str_number;
    i1, i2, j1, j2: pool_pointer;
    result: integer;
begin
    call_func(scan_toks(false, true));
    s1 := tokens_to_string(def_ref);
    delete_token_ref(def_ref);
    call_func(scan_toks(false, true));
    s2 := tokens_to_string(def_ref);
    delete_token_ref(def_ref);
    i1 := str_start[s1];
    j1 := str_start[s1 + 1];
    i2 := str_start[s2];
    j2 := str_start[s2 + 1];
    while (i1 < j1) and (i2 < j2) do begin
        if str_pool[i1] < str_pool[i2] then begin
            result := -1;
            goto done;
        end;
        if str_pool[i1] > str_pool[i2] then begin
            result := 1;
            goto done;
        end;
        incr(i1);
        incr(i2);
    end;
    if (i1 = j1) and (i2 = j2) then
        result := 0
    else if i1 < j1 then
        result := 1
    else
        result := -1;
done:
    pdf_retval := result;
    flush_str(s2);
    flush_str(s1);
end;
@y
procedure compare_strings; {to implement \.{\\pdfstrcmp}}
label done;
var s1, s2: str_number;
    i1, i2, j1, j2: pool_pointer;
begin
    call_func(scan_toks(false, true));
    s1 := tokens_to_string(def_ref);
    delete_token_ref(def_ref);
    call_func(scan_toks(false, true));
    s2 := tokens_to_string(def_ref);
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
@z

%***********************************************************************

@x 33387
@ @<Implement \.{\\pdfescapestring}@>=
begin
    scan_pdf_ext_toks;
    escapestr(tokens_to_string(def_ref));
    flush_str(last_tokens_string);
    delete_token_ref(def_ref);
end

@ @<Implement \.{\\pdfescapename}@>=
begin
    scan_pdf_ext_toks;
    escapename(tokens_to_string(def_ref));
    flush_str(last_tokens_string);
    delete_token_ref(def_ref);
end
@y
@z

@x 33406
@ @<Implement \.{\\pdfstrcmp}@>=
compare_strings
@y
@z

@x 33409
@ @<Set initial values of key variables@>=
  seconds_and_micros(epochseconds,microseconds);
@y
@ @<Set initial values of key variables@>=
  seconds_and_micros(epochseconds,microseconds);
  init_start_time;
@z

%***********************************************************************
