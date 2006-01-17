% Backport some 1.23 dir primitives to 1.15
% namely, \boxdir
% NOT \nextfakemath, which requires too many changes

@x
@d assign_dir=register+1 {(\.{\\pagedir}, \.{\\textdir})}
@d max_internal=assign_dir
   {the largest code that can follow \.{\\the}}
@y
@d assign_next_fake_math=register+1 {(\.{\\nextfakemath})} {unused}
@d assign_box_dir=register+2 {(\.{\\boxdir})}
@d assign_dir=register+3 {(\.{\\pagedir}, \.{\\textdir})}
@d max_internal=assign_dir
   {the largest code that can follow \.{\\the}}
@z

@x
assign_int: scanned_result(new_eqtb_int(m))(int_val);
assign_dir: scanned_result(new_eqtb_int(m))(dir_val);
@y
assign_int: scanned_result(new_eqtb_int(m))(int_val);
assign_box_dir: begin
  scan_eight_bit_int;
  m:=cur_val;
  if box(m)<>null then cur_val:=box_dir(box(m))
  else cur_val:=0;
  cur_val_level:=dir_val;
  end;
assign_dir: scanned_result(new_eqtb_int(m))(dir_val);
@z

@x [47] m.1071
primitive("xleaders",leader_ship,x_leaders);
@!@:x_leaders_}{\.{\\xleaders} primitive@>
@y
primitive("xleaders",leader_ship,x_leaders);
@!@:x_leaders_}{\.{\\xleaders} primitive@>
primitive("boxdir",assign_box_dir,0);
@!@:box_dir}{\.{\\boxdir} primitive@>
@z

@x [47] m.1073
any_mode(make_box): begin_box(0);
@y
any_mode(make_box): begin_box(0);
any_mode(assign_box_dir): begin scan_eight_bit_int;
  cur_box:=box(cur_val);
  scan_optional_equals; scan_dir;
  if cur_box<>null then box_dir(cur_box):=cur_val;
  end;
@z

