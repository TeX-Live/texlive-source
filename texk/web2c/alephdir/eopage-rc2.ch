% vim ft=ch
% Fix page dimension bugs
% TODO : not there yet for BLB?
@x
emergency_stretch_code:print_esc("emergencystretch");
othercases print("[unknown dimen parameter!]")
endcases;
end;
@y
emergency_stretch_code:print_esc("emergencystretch");
page_width_code:print_esc("pagewidth");
page_height_code:print_esc("pageheight");
page_right_offset_code:print_esc("pagerightoffset");
page_bottom_offset_code:print_esc("pagebottomoffset");
othercases print("[unknown dimen parameter!]")
endcases;
end;
@z

@x
@ @<Ship box |p| out@>=
@<Update the values of |max_h| and |max_v|; but if the page is too large,
  |goto done|@>;
@<Initialize variables as |ship_out| begins@>;
page_loc:=dvi_offset+dvi_ptr;
dvi_out(bop);
for k:=0 to 9 do dvi_four(count(k));
dvi_four(last_bop); last_bop:=page_loc;
dvi_direction:=page_direction;
case box_direction(dvi_direction) of
dir_TL_,dir_LT_: begin
  end;
dir_TR_,dir_RT_: begin
  dvi_right(page_right_offset);
  end;
dir_RB_,dir_BR_: begin
  dvi_right(page_right_offset);
  dvi_down(page_bottom_offset);
  end;
dir_BL_,dir_LB_: begin
  dvi_down(page_bottom_offset);
  end;
end;
cur_h:=h_offset;
cur_v:=height(p)+v_offset;
case box_direction(dvi_direction) of
dir_TL_: begin
  dvi_down(cur_v);
  dvi_right(cur_h);
  end;
dir_TR_: begin
  dvi_down(cur_v);
  dvi_right(-cur_h);
  end;
dir_LT_: begin
  dvi_right(cur_v);
  dvi_down(cur_h);
  end;
dir_LB_: begin
  dvi_right(cur_v);
  dvi_down(-cur_h);
  end;
dir_BL_: begin
  dvi_down(-cur_v);
  dvi_right(cur_h);
  end;
dir_BR_: begin
  dvi_down(-cur_v);
  dvi_right(-cur_h);
  end;
dir_RT_: begin
  dvi_right(-cur_v);
  dvi_down(cur_h);
  end;
dir_RB_: begin
  dvi_right(-cur_v);
  dvi_down(-cur_h);
  end;
end;
dvi_h:=cur_h;
dvi_v:=cur_v;
temp_ptr:=p;
if type(p)=vlist_node then vlist_out@+else hlist_out;
dvi_out(eop); incr(total_pages); cur_s:=-1;
done:

@y
@ @<Ship box |p| out@>=
@<Update the values of |max_h| and |max_v|; but if the page is too large,
  |goto done|@>;
@<Initialize variables as |ship_out| begins@>;
page_loc:=dvi_offset+dvi_ptr;
dvi_out(bop);
for k:=0 to 9 do dvi_four(count(k));
dvi_four(last_bop); last_bop:=page_loc;
dvi_direction:=page_direction;
case box_direction(dvi_direction) of
dir_TL_,dir_LT_: begin
  end;
dir_TR_,dir_RT_: begin
  dvi_right(page_width-page_right_offset);
  dvi_h:=-page_right_offset;
  end;
dir_RB_,dir_BR_: begin
  dvi_right(page_width-page_right_offset);
  dvi_down(page_height-page_bottom_offset);
  dvi_h:=-page_right_offset;
  dvi_v:=-page_bottom_offset;
  end;
dir_BL_,dir_LB_: begin
  dvi_down(page_height-page_bottom_offset);
  dvi_v:=-page_bottom_offset;
  end;
end;
cur_h:=h_offset;
cur_v:=height(p)+v_offset;
case box_direction(dvi_direction) of
dir_TL_: begin
  dvi_right(cur_h);
  dvi_down(cur_v);
  end;
dir_TR_: begin
  dvi_right(-cur_h);
  dvi_down(cur_v);
  end;
dir_LT_: begin
  dvi_right(cur_v);
  dvi_down(cur_h);
  end;
dir_LB_: begin
  dvi_right(cur_v);
  dvi_down(-cur_h);
  end;
dir_BL_: begin
  dvi_right(cur_h);
  dvi_down(-cur_v);
  end;
dir_BR_: begin
  dvi_right(-cur_h);
  dvi_down(-cur_v);
  end;
dir_RT_: begin
  dvi_right(-cur_v);
  dvi_down(cur_h);
  end;
dir_RB_: begin
  dvi_right(-cur_v);
  dvi_down(-cur_h);
  end;
end;
temp_ptr:=p;
dvi_h:=dvi_h+cur_h;
dvi_v:=dvi_v+cur_v;
if type(p)=vlist_node then vlist_out@+else hlist_out;
dvi_out(eop); incr(total_pages); cur_s:=-1;
done:

@z

@x
set_new_eqtb_sc(dimen_base+page_bottom_offset_code,page_height-9472573);
                {-2 inches}
set_new_eqtb_sc(dimen_base+page_right_offset_code,page_width-9472573);
                {-2 inches}
@y
set_new_eqtb_sc(dimen_base+page_bottom_offset_code,4736287); {1 inch}
set_new_eqtb_sc(dimen_base+page_right_offset_code,4736287); {1 inch}
@z

@x
{
|primitive("pagerightoffset",assign_dimen,dimen_base+page_right_offset_code);|
|primitive("pagebottomoffset",assign_dimen,dimen_base+page_bottom_offset_code);|
}
@y
primitive("pagerightoffset",assign_dimen,dimen_base+page_right_offset_code);
primitive("pagebottomoffset",assign_dimen,dimen_base+page_bottom_offset_code);
@z


