@x
@!dvi_buf_size:integer; {size of the output buffer; must be a multiple of 8}
@!quoted_filename:boolean; {current filename is quoted}
@y
@!dvi_buf_size:integer; {size of the output buffer; must be a multiple of 8}
@!quoted_filename:boolean; {current filename is quoted}
@!special_loc:pointer;
@!special_token:halfword;

@!parse_first_line_p:cinttype; {parse the first line for options}
@!file_line_error_style_p:cinttype; {format messages as file:line:error}
@!halt_on_error_p:boolean; {allow only on error.}

@!src_specials_p : boolean;
@!insert_src_special_auto : boolean;
@!insert_src_special_every_par : boolean;
@!insert_src_special_every_parend : boolean;
@!insert_src_special_every_cr : boolean;
@!insert_src_special_every_math : boolean;
@!insert_src_special_every_hbox : boolean;
@!insert_src_special_every_vbox : boolean;
@!insert_src_special_every_display : boolean;
@z

@x
wterm(eTeX_banner);
wterm(version_string);
if format_ident=0 then wterm_ln(' (preloaded format=',dump_name,')')
else  begin slow_print(format_ident); print_ln;
  end;
@y
if src_specials_p or file_line_error_style_p or parse_first_line_p then
  wterm(banner_k)
else
  wterm(eTeX_banner);
wterm(version_string);
if format_ident=0 then wterm_ln(' (preloaded format=',dump_name,')')
else  begin slow_print(format_ident); print_ln;
  end;
if shellenabledp then begin
  wterm(' ');
  if restrictedshell then begin
    wterm('restricted ');
  end;
  wterm_ln('\write18 enabled.');
end;
if src_specials_p then begin
  wterm_ln(' Source specials enabled.')
end;
@z

@x
  print_nl("! "); print(#);
@y
  if file_line_error_style_p then print_file_line
  else print_nl("! ");
  print(#);
@z

@x
print_char("."); show_context;
@y
print_char("."); show_context;
if (halt_on_error_p) then begin
  history:=fatal_error_stop; jump_out;
end;
@z

@x
@!in_open : 0..max_in_open; {the number of lines in the buffer, less one}
@!open_parens : 0..max_in_open; {the number of open text files}
@!input_file : ^alpha_file;
@!input_file_mode : ^halfword;
@!input_file_translation : ^halfword;
@!line : integer; {current line number in the current source file}
@!line_stack : ^integer;
@y
@!in_open : 0..max_in_open; {the number of lines in the buffer, less one}
@!open_parens : 0..max_in_open; {the number of open text files}
@!input_file : ^alpha_file;
@!input_file_mode : ^halfword;
@!input_file_translation : ^halfword;
@!line : integer; {current line number in the current source file}
@!line_stack : ^integer;
@!source_filename_stack : ^str_number;
@!full_source_filename_stack : ^str_number;
@z

@x
begin wlog(eTeX_banner);
wlog(version_string);
@y
begin if src_specials_p or file_line_error_style_p or parse_first_line_p
then
  wlog(banner_k)
else
  wlog(eTeX_banner);
wlog(version_string);
if shellenabledp then begin
  wlog_cr;
  wlog(' ');
  if restrictedshell then begin
    wlog('restricted ');
  end;
  wlog('\write18 enabled.')
  end;
if src_specials_p then begin
  wlog_cr;
  wlog(' Source specials enabled.')
end;
if file_line_error_style_p then begin
  wlog_cr;
  wlog(' file:line:error style messages enabled.')
end;
if parse_first_line_p then begin
  wlog_cr;
  wlog(' %&-line parsing enabled.');
end;
@z

%% **   537  ******************************************************************

@x
done: name:=a_make_name_string(cur_file);
if name=str_ptr-1 then {we can try to conserve string pool space now}
  begin temp_str:=search_string(name);
  if temp_str>0 then
    begin name:=temp_str; flush_string;
    end;
  end;
@y
done: name:=a_make_name_string(cur_file);
source_filename_stack[in_open]:=name;
full_source_filename_stack[in_open]:=make_full_name_string;
if name=str_ptr-1 then {we can try to conserve string pool space now}
  begin temp_str:=search_string(name);
  if temp_str>0 then
    begin name:=temp_str; flush_string;
    end;
  end;
@z

%%@x
%%if term_offset+length(name)>max_print_line-2 then print_ln
%%else if (term_offset>0)or(file_offset>0) then print_char(" ");
%%print_char("("); incr(open_parens); slow_print(name); update_terminal;
%%@y
%%if term_offset+length(full_source_filename_stack[in_open])>max_print_line-2 then print_ln
%%else if (term_offset>0)or(file_offset>0) then print_char(" ");
%%print_char("("); incr(open_parens); slow_print(full_source_filename_stack[in_open]); update_terminal;
%%@z

%% **   774  ******************************************************************

 @x
if every_cr<>null then begin_token_list(every_cr,every_cr_text);
 @y
if (insert_src_special_every_cr and head<>tail) then insert_src_special;
if every_cr<>null then begin_token_list(every_cr,every_cr_text);
 @z

%% **   799  ******************************************************************

 @x
if every_cr<>null then begin_token_list(every_cr,every_cr_text);
 @y
if (insert_src_special_every_cr) then insert_src_special;
if every_cr<>null then begin_token_list(every_cr,every_cr_text);
 @z

%% **  1034  ******************************************************************

@x
@<Append character |cur_chr|...@>=
adjust_space_factor;@/
@y
@<Append character |cur_chr|...@>=
if ((head=tail) and (mode>0)) then begin
  if (insert_src_special_auto) then append_src_special;
end;
adjust_space_factor;@/
@z

%% **  1083  ******************************************************************

 @x
  if every_vbox<>null then begin_token_list(every_vbox,every_vbox_text);
 @y
  if (insert_src_special_every_vbox) then insert_src_special;
  if every_vbox<>null then begin_token_list(every_vbox,every_vbox_text);
 @z

 @x
  if every_hbox<>null then begin_token_list(every_hbox,every_hbox_text);
 @y
  if (insert_src_special_every_hbox) then insert_src_special;
  if every_hbox<>null then begin_token_list(every_hbox,every_hbox_text);
 @z

%% **  1091  ******************************************************************
% Disabled, prevents merge with web2c's tex.ch.
 @x
 if indented then begin
   p:=new_null_box; box_dir(p):=par_direction;
   width(p):=par_indent;@+
   tail_append(p);
   end;
 @y
 if indented then begin
   p:=new_null_box; box_dir(p):=par_direction;
   width(p):=par_indent;@+
   tail_append(p);
   if (insert_src_special_every_par) then insert_src_special;@+
   end;
 @z

%% insert source special at par because this is disabled above
@x
if indented then begin
  p:=new_null_box; box_dir(p):=par_direction;
  width(p):=par_indent;@+
  tail_append(p);
  end;
@y
if indented then begin
  p:=new_null_box; box_dir(p):=par_direction;
  width(p):=par_indent;@+
  tail_append(p);
  if (insert_src_special_every_par) then insert_src_special;@+
  end;
@z

%% **  1096  ******************************************************************

 @x fixme: etex conflict
  else line_break(widow_penalty);
 @y
  else begin
    if (insert_src_special_every_parend) then insert_src_special;
    line_break(widow_penalty);
  end;
 @z

%% **  1139  ******************************************************************

@x
if every_math<>null then begin_token_list(every_math,every_math_text);
@y
if (insert_src_special_every_math) then insert_src_special;
if every_math<>null then begin_token_list(every_math,every_math_text);
@z

%% **  1145  ******************************************************************

 @x
if every_display<>null then begin_token_list(every_display,every_display_text);
 @y
if (insert_src_special_every_display) then append_src_special;
if every_display<>null then begin_token_list(every_display,every_display_text);
 @z

%% **  1167  ******************************************************************

@x
  if every_vbox<>null then begin_token_list(every_vbox,every_vbox_text);
@y
  if (insert_src_special_every_vbox) then insert_src_special;
  if every_vbox<>null then begin_token_list(every_vbox,every_vbox_text);
@z

%% **  1313  ******************************************************************

@x
dump_int(par_loc); dump_int(write_loc);@/
@y
dump_int(par_loc); dump_int(write_loc); dump_int(special_loc);@/
@z

%% **  1314  ******************************************************************

@x
undump(hash_base)(frozen_control_sequence)(write_loc);@/
@y
undump(hash_base)(frozen_control_sequence)(write_loc);@/
undump(hash_base)(frozen_control_sequence)(special_loc);
special_token:=cs_token_flag+special_loc;@/
@z

@x
  line_stack:=xmalloc_array (integer, max_in_open);
@y
  line_stack:=xmalloc_array (integer, max_in_open);
  source_filename_stack:=xmalloc_array (str_number, max_in_open);
  full_source_filename_stack:=xmalloc_array (str_number, max_in_open);
@z

%% **  1344  ******************************************************************

@x
primitive("special",extension,special_node);@/
@y
primitive("special",extension,special_node);@/
special_loc:=cur_val; special_token:=cs_token_flag+special_loc;@/
@z

%% **  1379  ******************************************************************

@x
@* \[55] Index.
@y

@ @<Declare action procedures for use by |main_control|@>=

procedure insert_src_special;
var toklist, p, q : pointer;
begin
  if (source_filename_stack[in_open] > 0 and is_new_source (source_filename_stack[in_open], line)) then begin
    toklist := get_avail;
    p := toklist;
    info(p) := special_token;
    link(p) := get_avail; p := link(p);
    info(p) := left_brace_token+"{";
    q := str_toks (make_src_special (source_filename_stack[in_open], line));
    link(p) := link(temp_head);
    p := q;
    link(p) := get_avail; p := link(p);
    info(p) := right_brace_token+"}";
    ins_list (toklist);
    remember_source_info (source_filename_stack[in_open], line);
  end;
end;

procedure append_src_special;
var q : pointer;
begin
  if (source_filename_stack[in_open] > 0 and is_new_source (source_filename_stack[in_open], line)) then begin
    new_whatsit (special_node, write_node_size);
    write_stream(tail) := null;
    def_ref := get_avail;
    token_ref_count(def_ref) := null;
    q := str_toks (make_src_special (source_filename_stack[in_open], line));
    link(def_ref) := link(temp_head);
    write_tokens(tail) := def_ref;
    remember_source_info (source_filename_stack[in_open], line);
  end;
end;

@* \[55] Index.
@z
