% comcop.ch: Local adaptations for file omocp.ch
%
% This file is part of the Omega project, which
% is based on the web2c distribution of TeX.
% 
% Copyright (c) 1994--2000 John Plaice and Yannis Haralambous
% 
% This library is free software; you can redistribute it and/or
% modify it under the terms of the GNU Library General Public
% License as published by the Free Software Foundation; either
% version 2 of the License, or (at your option) any later version.
% 
% This library is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
% Library General Public License for more details.
% 
% You should have received a copy of the GNU Library General Public
% License along with this library; if not, write to the Free Software
% Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
%
%---------------------------------------
@x
@!inf_hyph_size = iinf_hyphen_size; {Must be not less than |hyph_prime|!}
@y
@!inf_hyph_size = iinf_hyphen_size; {Must be not less than |hyph_prime|!}
@!sup_ocp_list_size = 1000000;
@!inf_ocp_list_size = 1000;
@z
%---------------------------------------
@x
@!max_print_line:integer;
  {width of longest text lines output; should be at least 60}
@y
@!max_print_line:integer;
  {width of longest text lines output; should be at least 60}
@!ocp_list_size:integer;
@z
%---------------------------------------
@x
if not b_open_in(ocp_file) then ocp_abort("opening file");
@y
if not ocp_open_in(ocp_file) then ocp_abort("opening file");
@z
%---------------------------------------
@x
@d ocpget==get(ocp_file)
@d ocpbyte==ocp_file^
@y
@d ocpget==ocp_temp:=getc(ocp_file)
@d ocpbyte==ocp_temp
@z
%---------------------------------------
@x
@!ocp_list_info:array[ocp_list_index] of memory_word;
  {the big collection of ocp list data}
@!ocp_listmem_ptr:ocp_list_index; {first unused word of |ocp_list_info|}
@!ocp_listmem_run_ptr:ocp_list_index; {temp unused word of |ocp_list_info|}
@!ocp_lstack_info:array[ocp_lstack_index] of memory_word;
  {the big collection of ocp lstack data}
@!ocp_lstackmem_ptr:ocp_lstack_index; {first unused word of |ocp_lstack_info|}
@!ocp_lstackmem_run_ptr:ocp_lstack_index; {temp unused word of |ocp_lstack_info|}
@!ocp_list_ptr:internal_ocp_list_number; {largest internal ocp list number in use}
@!ocp_list_list:array[internal_ocp_list_number] of ocp_list_index;
@y
@!ocp_list_info:^memory_word;
  {the big collection of ocp list data}
@!ocp_listmem_ptr:ocp_list_index; {first unused word of |ocp_list_info|}
@!ocp_listmem_run_ptr:ocp_list_index; {temp unused word of |ocp_list_info|}
@!ocp_lstack_info:^memory_word;
  {the big collection of ocp lstack data}
@!ocp_lstackmem_ptr:ocp_lstack_index; {first unused word of |ocp_lstack_info|}
@!ocp_lstackmem_run_ptr:ocp_lstack_index; {temp unused word of |ocp_lstack_info|}
@!ocp_list_ptr:internal_ocp_list_number; {largest internal ocp list number in use}
@!ocp_list_list:^ocp_list_index;
@z
%---------------------------------------
@x
@ @<Dump the ocp list information@>=
dump_int(ocp_listmem_ptr);
for k:=0 to ocp_listmem_ptr-1 do dump_wd(ocp_list_info[k]);
dump_int(ocp_list_ptr);
for k:=null_ocp_list to ocp_list_ptr do begin
  dump_int(ocp_list_list[k]);
  print_nl("\ocplist"); 
  print_esc(ocp_list_id_text(k)); 
  print_char("=");
  print_ocp_list(ocp_list_list[k]);
  end;
dump_int(ocp_lstackmem_ptr);
for k:=0 to ocp_lstackmem_ptr-1 do dump_wd(ocp_lstack_info[k])
@y
@ @<Dump the ocp list information@>=
dump_int(ocp_listmem_ptr);
dump_things(ocp_list_info[0], ocp_listmem_ptr);
dump_int(ocp_list_ptr);
dump_things(ocp_list_list[null_ocp_list], ocp_list_ptr+1-null_ocp_list);
for k:=null_ocp_list to ocp_list_ptr do begin
  print_nl("\ocplist"); 
  print_esc(ocp_list_id_text(k)); 
  print_char("=");
  print_ocp_list(ocp_list_list[k]);
  end;
dump_int(ocp_lstackmem_ptr);  
dump_things(ocp_lstack_info[0], ocp_lstackmem_ptr)
@z
%---------------------------------------
@x
@ @<Undump the ocp list information@>=
undump_size(1)(1000000)('ocp list mem size')(ocp_listmem_ptr);
for k:=0 to ocp_listmem_ptr-1 do undump_wd(ocp_list_info[k]);
undump_size(ocp_list_base)(ocp_list_biggest)('ocp list max')(ocp_list_ptr);
for k:=null_ocp_list to ocp_list_ptr do
  undump_int(ocp_list_list[k]);
undump_size(1)(1000000)('ocp lstack mem size')(ocp_lstackmem_ptr);
for k:=0 to ocp_lstackmem_ptr-1 do undump_wd(ocp_lstack_info[k])
@y
@ @<Undump the ocp list information@>=
undump_size(1)(1000000)('ocp list mem size')(ocp_listmem_ptr);
undump_things(ocp_list_info[0], ocp_listmem_ptr);
undump_size(0)(1000000)('ocp list max')(ocp_list_ptr);
undump_things(ocp_list_list[null_ocp_list], ocp_list_ptr+1-null_ocp_list);
undump_size(0)(1000000)('ocp lstack mem size')(ocp_lstackmem_ptr);
undump_things(ocp_lstack_info[0], ocp_lstackmem_ptr)
@z
%---------------------------------------
@x
  setup_bound_var (79)('max_print_line')(max_print_line);
@y
  setup_bound_var (79)('max_print_line')(max_print_line);
  setup_bound_var(1000)('ocp_list_size')(ocp_list_size);
@z
%---------------------------------------
@x
  hyph_link:=xmalloc_array (hyph_pointer, hyph_size);
@y
  hyph_link:=xmalloc_array (hyph_pointer, hyph_size);
  ocp_list_info:=xmalloc_array (memory_word, ocp_list_size);
  ocp_lstack_info:=xmalloc_array (memory_word, ocp_list_size);
  ocp_list_list:=xmalloc_array (ocp_list_index, ocp_list_size);
@z
