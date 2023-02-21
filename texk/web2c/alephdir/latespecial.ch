% latespecial.ch: Adding the "shipout" keyword to \special
%
% This file is part of the Omega project, which
% is based on the web2c distribution of TeX.
% 
% Copyright (c) 2023 Phelype Oleinik
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

@x
@d language_node=4 {|subtype| in whatsits that change the current language}
@y
@d latespecial_node=4 {|subtype| in whatsits for \.{\\special shipout}}
@d language_node=5 {|subtype| in whatsits that change the current language}
@z

@x
@d immediate_code=4 {command modifier for \.{\\immediate}}
@d set_language_code=5 {command modifier for \.{\\setlanguage}}
@y
@d immediate_code=5 {command modifier for \.{\\immediate}}
@d set_language_code=6 {command modifier for \.{\\setlanguage}}
@z

@x
begin new_whatsit(special_node,write_node_size); write_stream(tail):=null;
p:=scan_toks(false,true); write_tokens(tail):=def_ref;
end
@y
begin if scan_keyword("shipout") then
begin new_whatsit(latespecial_node,write_node_size); write_stream(tail):=null;
p:=scan_toks(false,false); write_tokens(tail):=def_ref;
end else
begin new_whatsit(special_node,write_node_size); write_stream(tail):=null;
p:=scan_toks(false,true); write_tokens(tail):=def_ref;
end; end
@z

@x
language_node:begin print_esc("setlanguage");
@y
latespecial_node:begin print_esc("special"); print(" shipout");
  print_mark(write_tokens(p));
  end;
language_node:begin print_esc("setlanguage");
@z

@x
write_node,special_node: begin r:=get_node(write_node_size);
@y
write_node,special_node,latespecial_node: begin r:=get_node(write_node_size);
@z

@x
write_node,special_node: begin delete_token_ref(write_tokens(p));
@y
write_node,special_node,latespecial_node: begin delete_token_ref(write_tokens(p));
@z

@x
begin synch_h; synch_v;@/
old_setting:=selector; selector:=new_string;
show_token_list(link(write_tokens(p)),null,pool_size-pool_ptr);
@y
@!h:halfword;
@!q,@!r:pointer; {temporary variables for list manipulation}
@!old_mode:integer; {saved |mode|}
begin synch_h; synch_v;@/
old_setting:=selector;
if subtype(p)=latespecial_node then
  begin @<Expand macros in the token list
    and make |link(def_ref)| point to the result@>;
    h:=def_ref;
  end
else h:=write_tokens(p);
selector:=new_string;
show_token_list(link(h),null,pool_size-pool_ptr);
@z

@x
pool_ptr:=str_start(str_ptr); {erase the string}
@y
pool_ptr:=str_start(str_ptr); {erase the string}
if subtype(p)=latespecial_node then
  flush_list(def_ref);
@z

@x
special_node:special_out(p);
@y
special_node,latespecial_node:special_out(p);
@z
