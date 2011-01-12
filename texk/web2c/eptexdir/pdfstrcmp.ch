% For LaTeX3
% borrowed from pdftex.web
%
@x
@d etex_convert_codes=etex_convert_base+1 {end of \eTeX's command codes}
@y
@d pdf_strcmp_code=etex_convert_base+1 {command code for \.{\\pdfstrcmp}}
@d etex_convert_codes=etex_convert_base+2 {end of \eTeX's command codes}
@z

@x
  eTeX_revision_code: print_esc("eTeXrevision");
@y
  eTeX_revision_code: print_esc("eTeXrevision");
  pdf_strcmp_code:   print_esc("pdfstrcmp");
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
@!u: str_number; {saved current string string}
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
    { save_warning_index := warning_index;}
    save_def_ref := def_ref;
    save_cur_string;
    compare_strings;
    def_ref := save_def_ref;
    { warning_index := save_warning_index;}
    scanner_status := save_scanner_status;
    restore_cur_string;
  end;
@z

@x
eTeX_revision_code: print(eTeX_revision);
@y
eTeX_revision_code: print(eTeX_revision);
pdf_strcmp_code: print_int(cur_val);
@z

@x
fix_date_and_time;@/
@y
fix_date_and_time;@/
isprint_utf8:=false;
@z

@x
primitive("eTeXrevision",convert,eTeX_revision_code);@/
@!@:eTeX_revision_}{\.{\\eTeXrevision} primitive@>
@y
primitive("eTeXrevision",convert,eTeX_revision_code);@/
@!@:eTeX_revision_}{\.{\\eTeXrevision} primitive@>
primitive("pdfstrcmp",convert,pdf_strcmp_code);@/
@!@:pdf_strcmp_}{\.{\\pdfstrcmp} primitive@>
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
@* \[54/pdf\TeX] System-dependent changes for \.{\\pdfstrcmp}
@d call_func(#) == begin if # <> 0 then do_nothing end
@d flushable(#) == (# = str_ptr - 1)

@<Glob...@>=
@!isprint_utf8: boolean;

@ @<Declare procedures needed in |do_ext...@>=
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
procedure flush_str(s: str_number); {flush a string if possible}
begin
    if flushable(s) then
        flush_string;
end;
function tokens_to_string(p: pointer): str_number; {return a string from tokens
list}
begin
    if selector = new_string then
      pdf_error("tokens", "tokens_to_string() called while selector = new_string");
    old_setting:=selector; selector:=new_string;
    show_token_list(link(p),null,pool_size-pool_ptr);
    selector:=old_setting;
    tokens_to_string := make_string;
end;

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

@* \[54] System-dependent changes.
@z
