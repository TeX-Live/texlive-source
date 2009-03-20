% Fix the Omega \char<..> primitives
%---------------------------------------
@x
@d assign_font_dimen=assign_mu_glue+1
   {user-defined font dimension ( \.{\\fontdimen} )}
@y
@d assign_font_dimen=assign_mu_glue+1
   {font dimension ( \.{\\fontdimen} ), \.{\\charwd}, \.{\\charht}, etc}
@d font_dimen_code=0
@d char_width_code=font_dimen_code+1
@d char_height_code=char_width_code+1
@d char_depth_code=char_height_code+1
@d char_italic_code=char_depth_code+1
@z
%---------------------------------------
@x [26] m.416 l.8407 - Omega char dimensions
primitive("fontdimen",assign_font_dimen,0);@/
@!@:font_dimen_}{\.{\\fontdimen} primitive@>
@y
primitive("fontdimen",assign_font_dimen,font_dimen_code);@/
@!@:font_dimen_}{\.{\\fontdimen} primitive@>
primitive("charwd",assign_font_dimen,char_width_code);@/
@!@:char_width_}{\.{\\charwd} primitive@>
primitive("charht",assign_font_dimen,char_height_code);@/
@!@:char_height_}{\.{\\charht} primitive@>
primitive("chardp",assign_font_dimen,char_depth_code);@/
@!@:char_depth_}{\.{\\chardp} primitive@>
primitive("charit",assign_font_dimen,char_italic_code);@/
@!@:char_italic_}{\.{\\charit} primitive@>
@z
%---------------------------------------
@x [26] m.416 l.8425 - Omega char dimensions
assign_font_dimen: print_esc("fontdimen");
@y
assign_font_dimen: if chr_code=font_dimen_code then print_esc("fontdimen")
else if chr_code=char_width_code then print_esc("charwd")
else if chr_code=char_height_code then print_esc("charht")
else if chr_code=char_depth_code then print_esc("chardp")
else if chr_code=char_italic_code then print_esc("charit")
else print("[unknown fontdimen code!]");
@z
%---------------------------------------
@x CLEAN UP FROM OMEGA
primitive("charwd",set_box_dimen,(width_offset+3));
@!@:charwd_}{\.{\\charwd} primitive@>
primitive("charht",set_box_dimen,(height_offset+3));
@!@:charht_}{\.{\\charht} primitive@>
primitive("chardp",set_box_dimen,(depth_offset+3));
@!@:chardp_}{\.{\\chardp} primitive@>
primitive("charit",set_box_dimen,(depth_offset+4));
@!@:charit_}{\.{\\charit} primitive@>
@y
@z
%---------------------------------------
@x
else if chr_code=(width_offset+3) then print_esc("charwd")
else if chr_code=(height_offset+3) then print_esc("charht")
else if chr_code=(depth_offset+3) then print_esc("chardp")
else print_esc("charit");
@y
else print("[unknown box dimen!]");
@z
%---------------------------------------
@x
@ @<Fetch a box dimension@>=
if m<=3 then
begin
   scan_register_num;
   fetch_box(q);
   if q=null then cur_val:=0 @+else
   cur_val:=mem[q+m].sc;
   cur_val_level:=dimen_val;
end
else
begin
   scan_char_num;
   if m=(width_offset+3) then begin
      cur_val:= char_width(main_f)(char_info(main_f)(cur_val))
      end
   else if m=(height_offset+3) then begin
      cur_val:= char_height(main_f)(height_depth(char_info(main_f)(cur_val)))
      end
   else if m=(depth_offset+3) then begin
      cur_val:= char_depth(main_f)(height_depth(char_info(main_f)(cur_val)))
      end
   else begin
      cur_val:= char_italic(main_f)(char_info(main_f)(cur_val));
      end;
   cur_val_level:=dimen_val;
end
@y
@ @<Fetch a box dimension@>=
if m<=height_offset then begin
   scan_register_num;
   fetch_box(q);
   if q=null then cur_val:=0 @+else
   cur_val:=mem[q+m].sc;
   cur_val_level:=dimen_val;
end
else begin
   confusion("boxdimen");
   { something wrong with the code ...}
end
@z
%---------------------------------------
@x [26] m.419 l.8461 - Omega char dimensions
@ @<Fetch a font dimension@>=
begin find_font_dimen(false);
font_info(dimen_font)(font_file_size(dimen_font)).sc:=0;
scanned_result(font_info(dimen_font)(cur_val).sc)(dimen_val);
end
@y
@ @<Fetch a font dimension@>=
if m=font_dimen_code then begin find_font_dimen(false);
  font_info(dimen_font)(font_file_size(dimen_font)).sc:=0;
  scanned_result(font_info(dimen_font)(cur_val).sc)(dimen_val);
  end
else
  begin scan_char_num;
     if m=char_width_code then begin
        cur_val:= char_width(main_f)(char_info(main_f)(cur_val))
        end
     else if m=char_height_code then begin
        cur_val:= char_height(main_f)(height_depth(char_info(main_f)(cur_val)))
        end
     else if m=char_depth_code then begin
        cur_val:= char_depth(main_f)(height_depth(char_info(main_f)(cur_val)))
        end
     else if m=char_italic_code then begin
        cur_val:= char_italic(main_f)(char_info(main_f)(cur_val));
        end
     else confusion("fontdimen");
     cur_val_level:=dimen_val;
  end
@z
%---------------------------------------
@x
assign_font_dimen: begin find_font_dimen(true); k:=cur_val;
  scan_optional_equals; scan_normal_dimen;
  font_info(dimen_font)(k).sc:=cur_val;
  end;
@y
assign_font_dimen: begin
  n:=cur_chr;
  if n=font_dimen_code then begin
    find_font_dimen(true); k:=cur_val;
    scan_optional_equals; scan_normal_dimen;
    font_info(dimen_font)(k).sc:=cur_val;
    end
  else begin
    print_err("Invalid dimension assignment");
    help2("You tried to assign a character dimension.")@/
    ("This is not allowed. Proceed; I'll ignore the assignment.");
    error;
    scan_char_num; scan_optional_equals; scan_normal_dimen;
  end
end;
@z
