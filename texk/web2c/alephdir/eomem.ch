% vim ft=ch
% This file is part of the e-Omega project
% Purpouse of this file: adapting e-TeX to Omega memory management
% Most registers are reverted to the Omega model. The only exception
% is markings.
%---------------------------------------
 @x eobase.ch l.672
begin if eTeX_ex and(eqtb[p].int=w) then
 @y
begin if eTeX_ex and(new_eqtb_int(p)=w) then
 @z
%---------------------------------------
@x eobase.ch l.1071
    if m=mem_bot then
      begin scan_register_num;
      if cur_val<256 then cur_val:=equiv(toks_base+cur_val)
      else  begin find_sa_element(tok_val,cur_val,false);
        if cur_ptr=null then cur_val:=null
        else cur_val:=sa_ptr(cur_ptr);
        end;
      end
    else cur_val:=sa_ptr(m)
  else cur_val:=equiv(m);
  cur_val_level:=tok_val;
@y
    begin scan_eight_bit_int; m:=toks_base+cur_val;
    end;
  scanned_result(equiv(m))(tok_val);
@z
%----------------------------------------
@x l.1220
begin if (m<mem_bot)or(m>lo_mem_stat_max) then
  begin cur_val_level:=sa_type(m);
  if cur_val_level<glue_val then cur_val:=sa_int(m)
  else cur_val:=sa_ptr(m);
  end
else  begin scan_register_num; cur_val_level:=m-mem_bot;
  if cur_val>255 then
    begin find_sa_element(cur_val_level,cur_val,false);
    if cur_ptr=null then
      if cur_val_level<glue_val then cur_val:=0
      else cur_val:=zero_glue
    else if cur_val_level<glue_val then cur_val:=sa_int(cur_ptr)
    else cur_val:=sa_ptr(cur_ptr);
    end
  else
  case cur_val_level of
@y
begin scan_eight_bit_int;
case m of
@z
%---------------------------------------
@x
  end;
@y
cur_val_level:=m;
@z
%----------------------------------------
@x l.1420
 if eTeX_ex and(tracing_lost_chars>1) then tracing_online:=1;
@y
 if eTeX_ex and(tracing_lost_chars>1) then set_new_eqtb_int(int_base+tracing_online_code,1);
@z
%----------------------------------------
@x 
 tracing_online:=old_setting;
@y
 set_new_eqtb_int(int_base+tracing_online_code,old_setting);
@z
%---------------------------------------
@x l.2336
@d global_box_flag==box_flag+(number_regs+number_regs) {context code for `\.{\\global\\setbox0}'}
@d ship_out_flag==global_box_flag+1 {context code for `\.{\\shipout}'}
@y
@d global_box_flag==(box_flag+number_regs) {context code for `\.{\\global\\setbox0}'}
@d max_global_box_flag==(global_box_flag+number_regs)
@d ship_out_flag==(max_global_box_flag+1) {context code for `\.{\\shipout}'}
@z
%----------------------------------------
@x
@!a:small_number; {global prefix}
@y
@z
%----------------------------------------
@x eobase.ch l.2309
begin if box_context<global_box_flag then
  begin cur_val:=box_context-box_flag; a:=0;
  end
else  begin cur_val:=box_context-global_box_flag; a:=4;
  end;
if cur_val<256 then define(box_base+cur_val,box_ref,cur_box)
else sa_def_box;
end
@y
if box_context<global_box_flag then
  eq_define(box_base+box_context-box_flag,box_ref,cur_box)
else geq_define(box_base+box_context-global_box_flag,box_ref,cur_box)
@z
%---------------------------------------
@x eobase.ch l.2328
@!n:halfword; {a box number}
begin case cur_chr of
box_code: begin scan_register_num; fetch_box(cur_box);
  set_equiv(box_base+cur_val,null);
      {the box becomes void, at the same level}
  end;
copy_code: begin scan_register_num; fetch_box(q); cur_box:=copy_node_list(q);
@y
@!n:eight_bits; {a box number}
begin case cur_chr of
box_code: begin scan_eight_bit_int; cur_box:=box(cur_val);
  set_equiv(box_base+cur_val,null);
      {the box becomes void, at the same level}
  end;
copy_code: begin scan_eight_bit_int; cur_box:=copy_node_list(box(cur_val));
@z
%---------------------------------------
@x eobase.ch l.2359
begin scan_register_num; n:=cur_val;
@y
begin scan_eight_bit_int; n:=cur_val;
@z
%---------------------------------------
@x eobase.ch l.2422
begin if cur_chr>copy_code then @<Handle saved items and |goto done|@>;
c:=cur_chr; scan_register_num; fetch_box(p);
@y
begin if cur_chr>copy_code then @<Handle saved items and |goto done|@>;
c:=cur_chr; scan_eight_bit_int; p:=box(cur_val);
@z
%---------------------------------------
@x eobase.ch l.2431
else  begin link(tail):=list_ptr(p);
change_box(null);
@y
else  begin link(tail):=list_ptr(p);
set_equiv(box_base+cur_val,null);
@z
%----------------------------------------
@x eobase.ch l.2698
  othercases begin scan_register_num;
    if cur_val>255 then
      begin j:=n-count_def_code; {|int_val..box_val|}
      if j>mu_val then j:=tok_val; {|int_val..mu_val| or |tok_val|}
      find_sa_element(j,cur_val,true); add_sa_ref(cur_ptr);
      if j=tok_val then j:=toks_register@+else j:=register;
      define(p,j,cur_ptr);
      end
    else
@y
  othercases begin scan_eight_bit_int;
@z
%---------------------------------------
@x eobase.ch l.2726
  e:=false; {just in case, will be set |true| for sparse array elements}
  if cur_cmd=toks_register then
    if cur_chr=mem_bot then
      begin scan_register_num;
      if cur_val>255 then
        begin find_sa_element(tok_val,cur_val,true);
        cur_chr:=cur_ptr; e:=true;
        end
      else cur_chr:=toks_base+cur_val;
      end
    else e:=true;
  p:=cur_chr; {|p=every_par_loc| or |output_routine_loc| or \dots}
@y
  if cur_cmd=toks_register then
    begin scan_eight_bit_int; p:=toks_base+cur_val;
    end
  else p:=cur_chr; {|p=every_par_loc| or |output_routine_loc| or \dots}
@z
%---------------------------------------
@x eobase.ch l.2746
    begin sa_define(p,null)(p,undefined_cs,null); free_avail(def_ref);
    end
  else  begin if (p=output_routine_loc)and not e then {enclose in curlies}
@y
    begin define(p,undefined_cs,null); free_avail(def_ref);
    end
  else  begin if p=output_routine_loc then {enclose in curlies}
@z
%---------------------------------------
@x eobase.ch l.2754
    sa_define(p,def_ref)(p,call,def_ref);
@y
    define(p,call,def_ref);
@z
%---------------------------------------
@x eobase.ch l.2770
if (cur_cmd=toks_register)or(cur_cmd=assign_toks) then
  begin if cur_cmd=toks_register then
    if cur_chr=mem_bot then
      begin scan_register_num;
      if cur_val<256 then q:=equiv(toks_base+cur_val)
      else  begin find_sa_element(tok_val,cur_val,false);
        if cur_ptr=null then q:=null
        else q:=sa_ptr(cur_ptr);
        end;
      end
    else q:=sa_ptr(cur_ptr)
  else q:=equiv(cur_chr);
  if q=null then sa_define(p,null)(p,undefined_cs,null)
  else  begin add_token_ref(q); sa_define(p,q)(p,call,q);
    end;
  goto done;
  end
@y
begin if cur_cmd=toks_register then
  begin scan_eight_bit_int; cur_cmd:=assign_toks; cur_chr:=toks_base+cur_val;
  end;
if cur_cmd=assign_toks then
  begin q:=equiv(cur_chr);
  if q=null then define(p,undefined_cs,null)
  else  begin add_token_ref(q); define(p,call,q);
    end;
  goto done;
  end;
end
@z
%---------------------------------------
@x eobase.ch l.2792
@!e:boolean; {does |l| refer to a sparse array element?}
@!w:integer; {integer or dimen value of |l|}
begin q:=cur_cmd;
e:=false; {just in case, will be set |true| for sparse array elements}
@y
begin q:=cur_cmd;
@z
%----------------------------------------
@x eobase.ch l.2802
if p<glue_val then sa_word_define(l,cur_val)
else  begin trap_zero_glue; sa_define(l,cur_val)(l,glue_ref,cur_val);
@y
if p<glue_val then word_define(l,cur_val)
else  begin trap_zero_glue; define(l,glue_ref,cur_val);
@z
%---------------------------------------
@x eobase.ch l.2809
if (cur_chr<mem_bot)or(cur_chr>lo_mem_stat_max) then
  begin l:=cur_chr; p:=sa_type(l); e:=true;
  end
else  begin p:=cur_chr-mem_bot; scan_register_num;
  if cur_val>255 then
    begin find_sa_element(p,cur_val,true); l:=cur_ptr; e:=true;
    end
  else
@y
p:=cur_chr; scan_eight_bit_int;
@z
%---------------------------------------
@x eobase.ch l.2823
  end;
end;
found: if p<glue_val then@+if e then w:=sa_int(l)@+else w:=new_eqtb_int(l);
else if e then s:=sa_ptr(l)@+else s:=equiv(l)
@y
end;
found:
@z
%---------------------------------------
@x eobase.ch l.2832
  if q=advance then cur_val:=cur_val+w;
@y
  if q=advance then cur_val:=cur_val+new_eqtb_int(l);
@z
%---------------------------------------
@x eobase.ch l.2838
begin q:=new_spec(cur_val); r:=s;
@y
begin q:=new_spec(cur_val); r:=equiv(l);
@z
%---------------------------------------
@x eobase.ch l.2847
    if p=int_val then cur_val:=mult_integers(w,cur_val)
    else cur_val:=nx_plus_y(w,cur_val,0)
  else cur_val:=x_over_n(w,cur_val)
else  begin r:=new_spec(s);
@y
    if p=int_val then cur_val:=mult_integers(new_eqtb_int(l),cur_val)
    else cur_val:=nx_plus_y(new_eqtb_int(l),cur_val,0)
  else cur_val:=x_over_n(new_eqtb_int(l),cur_val)
else  begin s:=equiv(l); r:=new_spec(s);
@z
%---------------------------------------
@x eobase.ch l.2859
set_box: begin scan_register_num;
  if global then n:=global_box_flag+cur_val@+else n:=box_flag+cur_val;
  scan_optional_equals;
  if set_box_allowed then scan_box(n)
@y
set_box: begin scan_eight_bit_int;
  if global then n:=global_box_flag+cur_val@+else n:=box_flag+cur_val;
  scan_optional_equals;
  if set_box_allowed then scan_box(n)
@z
%---------------------------------------
@x eobase.ch l.2883
@!b:pointer; {box register}
begin c:=cur_chr; scan_register_num; fetch_box(b); scan_optional_equals;
scan_normal_dimen;
if b<>null then mem[b+c].sc:=cur_val;
@y
@!b:eight_bits; {box number}
begin c:=cur_chr; scan_eight_bit_int; b:=cur_val; scan_optional_equals;
scan_normal_dimen;
if box(b)<>null then mem[box(b)+c].sc:=cur_val;
@z
%---------------------------------------
@x [49] m.1296 l.23637 - e-TeX sparse arrays
begin scan_register_num; fetch_box(p); begin_diagnostic;
print_nl("> \box"); print_int(cur_val); print_char("=");
if p=null then print("void")@+else show_box(p);
@y
begin scan_eight_bit_int; begin_diagnostic;
print_nl("> \box"); print_int(cur_val); print_char("=");
if box(cur_val)=null then print("void")
else show_box(box(cur_val));
@z
%----------------------------------------
@x eobase.ch l.2979
if eTeX_ex then for k:=int_val to tok_val do dump_int(sa_root[k]);
@y
@z
%----------------------------------------
@x eobase.ch l.2986
if eTeX_ex then for k:=int_val to tok_val do
  undump(null)(lo_mem_max)(sa_root[k]);
@y
@z
@x l. 3132
for j:=0 to eTeX_states-1 do eTeX_state(j):=0; {disable all enhancements}
@y
for j:=0 to eTeX_states-1 do set_new_eqtb_int(eTeX_state_base+j,0); {disable all enhancements}
@z
