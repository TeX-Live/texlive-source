@x
@!file_line_error_style_p:boolean; {file:line:error style messages.}
@y
@!file_line_error_style_p:boolean; {output file:line:error style errors.}
@!halt_on_error_p:boolean; {stop at first error.}
@!quoted_filename : boolean;
@z

@x
   ready_already:=0;
   if (history <> spotless) and (history <> warning_issued) then
@y
   ready_already:=0;
   texmf_finish_job;
   if (history <> spotless) and (history <> warning_issued) then
@z

@x
else begin print_char("."); show_context end;
@y
else begin print_char("."); show_context end;
if (halt_on_error_p) then begin
  history:=fatal_error_stop; jump_out;
end;
@z

@x
area_delimiter:=-1; ext_delimiter:=-1;
end;
@y
area_delimiter:=-1; ext_delimiter:=-1;
quoted_filename:=false;
end;
@z

@x
begin if stop_at_space and ((c=" ")or(c=tab)) then more_name:=false
else  begin if IS_DIR_SEP (c) then
@y
begin
if c="""" then begin
  quoted_filename:=not quoted_filename;
  more_name:=true;
end;
if ((c=" ")or(c=tab)) and stop_at_space and (not quoted_filename) then more_name:=false
else  begin
  if (c="*") then c:=" "
  else if (c="?") then c:="~";
  if IS_DIR_SEP (c) then
@z

@x
@<Basic printing...@>=
procedure print_file_name(@!n,@!a,@!e:integer);
begin print(a); print(n); print(e);
end;
@y
@d print_file_name==texmf_print_file_name
@z

@x
@d append_to_name(#)==begin c:=#; incr(k);
  if k<=file_name_size then name_of_file[k]:=xchr[c];
  end
@y
@d append_to_name(#)==begin c:=#; if not (c="""") then begin incr(k);
  if k<=file_name_size then name_of_file[k]:=xchr[c];
  end end
@z

@x
@p begin @!{|start_here|}
@y
@p begin @!{|start_here|}

  texmf_start_job;
@z
