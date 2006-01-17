%----------------------------------------
% Fix some compilation quirks
% l. 4780
@x
  if local_par_bool then begin end {tail_append(local_par)}
@y
  if local_par_bool then begin end {|tail_append(local_par)|}
@z
% l. 8515
@x
      if (type(tail)<>math_node)or(subtype(tail)<>end_M_code) then
@y
      if (type(tail)<>math_node) then
@z
% l.28263
@x
pack_direction:=-1;
{
set_new_eqtb_sc(dimen_base+page_height_code,55380984); {29.7cm}
set_new_eqtb_sc(dimen_base+page_width_code,39158276); {21cm}
}
set_new_eqtb_sc(dimen_base+page_height_code,15961652); {for talk}
set_new_eqtb_sc(dimen_base+page_width_code,23718717); {for talk}
@y
pack_direction:=-1;

set_new_eqtb_sc(dimen_base+page_height_code,55380984); {29.7cm}
set_new_eqtb_sc(dimen_base+page_width_code,39158276); {21cm}
{
| set_new_eqtb_sc(dimen_base+page_height_code,15961652); | {for talk}
| set_new_eqtb_sc(dimen_base+page_width_code,23718717);  | {for talk}
}
@z
% l. 28336
@x
primitive("pagewidth",assign_dimen,dimen_base+page_width_code);@/
{
primitive("pagerightoffset",assign_dimen,dimen_base+page_right_offset_code);@/
primitive("pagebottomoffset",assign_dimen,dimen_base+page_bottom_offset_code);@/
}
@y
primitive("pagewidth",assign_dimen,dimen_base+page_width_code);@/
{
|primitive("pagerightoffset",assign_dimen,dimen_base+page_right_offset_code);|
|primitive("pagebottomoffset",assign_dimen,dimen_base+page_bottom_offset_code);|
}
@z
@x

@d decr(#)==#:=#-1

@y

@z
