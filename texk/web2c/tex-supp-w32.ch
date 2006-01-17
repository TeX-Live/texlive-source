@x
@!file_line_error_style_p:boolean; {format error messages as file:line:error}
@y
@!file_line_error_style_p:boolean; {format error messages as file:line:error}
@!halt_on_error_p:boolean; {stop at first error.}
@!quoted_filename : boolean;
@z

@x l:1927
   ready_already:=0;
   if (history <> spotless) and (history <> warning_issued) then
@y
   ready_already:=0;
   texmf_finish_job;
   if (history <> spotless) and (history <> warning_issued) then
@z

@x l:1951
else begin print_char("."); show_context end;
@y
else begin print_char("."); show_context end;
if (halt_on_error_p) then begin
  history:=fatal_error_stop; jump_out;
end;
@z

@x l:10101
begin area_delimiter:=0; ext_delimiter:=0;
@y
begin area_delimiter:=0; ext_delimiter:=0;
quoted_filename:=false;
@z

@x l:10111
begin if stop_at_space and (c=" ") then more_name:=false
else  begin str_room(1); append_char(c); {contribute |c| to the current string}
@y
begin
if c="""" then begin
  quoted_filename:=not quoted_filename;
  more_name:=true;
end;
if (c=" ") and stop_at_space and (not quoted_filename) then more_name:=false
else  begin
  if (c="*") then c:=" "
  else if (c="?") then c:="~";
  str_room(1); append_char(c); {contribute |c| to the current string}
@z

@x l:10175
begin slow_print(a); slow_print(n); slow_print(e);
@y
begin texmf_print_filename (a, n, e);
@z

@x l:10184
@d append_to_name(#)==begin c:=#; incr(k);
  if k<=file_name_size then name_of_file[k]:=xchr[c];
  end
@y
@d append_to_name(#)==begin c:=#; if not (c="""") then begin incr(k);
  if k<=file_name_size then name_of_file[k]:=xchr[c];
  end end
@z

@x l:13060
  print_nl("Output written on "); slow_print(output_file_name);
@y
  print_nl("Output written on "); texmf_print_filename(0, output_file_name, 0);
@z

@x
begin @!{|start_here|}
@y
begin @!{|start_here|}

  texmf_start_job;
@z

@x l:24891
    slow_print(log_name); print_char(".");
    end;
@y
    texmf_print_filename(0, log_name, 0); print_char(".");
    end;
@z
