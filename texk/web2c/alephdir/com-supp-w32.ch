@x
@!file_line_error_style_p:boolean; {output error messages with a C style. }
@y
@!file_line_error_style_p:boolean; {output error messages with a C style. }
@!halt_on_error_p:boolean; {allow only one error.}
@z

@x
   ready_already:=0;
@y
   ready_already:=0;
   texmf_finish_job;
@z

@x
print_char("."); show_context;
@y
if not file_line_error_style_p then begin
  print_char("."); show_context
end;

if (halt_on_error_p) then begin
  history:=fatal_error_stop; jump_out;
end;

@z
