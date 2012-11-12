% Compensate recent changes in ../tex.ch

@x [5.71] encTeX - native buffer printing
if last<>first then for k:=first to last-1 do print(buffer[k]);
@y
k:=first; while k < last do begin print_buffer(k) end;
@z

@x [29.518] l.10042 - print_file_name: quote if spaces in names.
some operating systems put the file area last instead of first.)
@^system dependencies@>

@d check_quoted(#) == {check if string |#| needs quoting}
if #<>0 then begin
  j:=str_start[#];
  while (not must_quote) and (j<str_start[#+1]) do begin
    must_quote:=str_pool[j]=" "; incr(j);
  end;
end
@#
@d print_quoted(#) == {print string |#|, omitting quotes}
if #<>0 then
  for j:=str_start[#] to str_start[#+1]-1 do
    if so(str_pool[j])<>"""" then
      print(so(str_pool[j]))
@y
some operating systems put the file area last instead of first.)
@^system dependencies@>
@z

@x [29.518] l.10042 - print_file_name: quote if spaces in names.
check_quoted(a); check_quoted(n); check_quoted(e);
@y
if a<>0 then begin
  j:=str_start[a];
  while (not must_quote) and (j<str_start[a+1]) do begin
    must_quote:=str_pool[j]=" "; incr(j);
  end;
end;
if n<>0 then begin
  j:=str_start[n];
  while (not must_quote) and (j<str_start[n+1]) do begin
    must_quote:=str_pool[j]=" "; incr(j);
  end;
end;
if e<>0 then begin
  j:=str_start[e];
  while (not must_quote) and (j<str_start[e+1]) do begin
    must_quote:=str_pool[j]=" "; incr(j);
  end;
end;
@z

@x [29.518] l.10042 - print_file_name: quote if spaces in names.
print_quoted(a); print_quoted(n); print_quoted(e);
@y
if a<>0 then
  for j:=str_start[a] to str_start[a+1]-1 do
    if so(str_pool[j])<>"""" then
      print(so(str_pool[j]));
if n<>0 then
  for j:=str_start[n] to str_start[n+1]-1 do
    if so(str_pool[j])<>"""" then
      print(so(str_pool[j]));
if e<>0 then
  for j:=str_start[e] to str_start[e+1]-1 do
    if so(str_pool[j])<>"""" then
      print(so(str_pool[j]));
@z

@x [48.1138] l.21648 - encTeX: \endmubyte primitive
begin print_err("Extra "); print_esc("endcsname");
@.Extra \\endcsname@>
help1("I'm ignoring this, since I wasn't doing a \csname.");
@y
begin
if cur_chr = 10 then
begin
  print_err("Extra "); print_esc("endmubyte");
@.Extra \\endmubyte@>
  help1("I'm ignoring this, since I wasn't doing a \mubyte.");
end else begin
  print_err("Extra "); print_esc("endcsname");
@.Extra \\endcsname@>
  help1("I'm ignoring this, since I wasn't doing a \csname.");
end;
@z
