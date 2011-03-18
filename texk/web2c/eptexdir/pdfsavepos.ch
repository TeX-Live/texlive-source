% last update: 2011/3/15

@x
@d dimen_pars=23 {total number of dimension parameters}
@y
@d pdf_page_width_code=23  {page width}
@d pdf_page_height_code=24 {page height}
@d dimen_pars=25 {total number of dimension parameters}
@z

@x
@d emergency_stretch==dimen_par(emergency_stretch_code)
@y
@d emergency_stretch==dimen_par(emergency_stretch_code)
@d pdf_page_width==dimen_par(pdf_page_width_code)
@d pdf_page_height==dimen_par(pdf_page_height_code)
@z
 
@x
emergency_stretch_code:print_esc("emergencystretch");
@y
emergency_stretch_code:print_esc("emergencystretch");
pdf_page_width_code:    print_esc("pdfpagewidth");
pdf_page_height_code:   print_esc("pdfpageheight");
@z

@x
@d badness_code=input_line_no_code+1 {code for \.{\\badness}}
@y
@d badness_code=input_line_no_code+1 {code for \.{\\badness}}
@d pdf_last_x_pos_code=badness_code+1 {code for \.{\\pdflastxpos}}
@d pdf_last_y_pos_code=pdf_last_x_pos_code+1 {code for \.{\\pdflastypos}}
@z

@x
@d eTeX_int=badness_code+1 {first of \eTeX\ codes for integers}
@y
@d eTeX_int=pdf_last_y_pos_code+1 {first of \eTeX\ codes for integers}
@z

@x
@d etex_convert_codes=etex_convert_base+2 {end of \eTeX's command codes}
@y
@d etex_convert_codes=etex_convert_base+3 {end of \eTeX's command codes}
@z

@x
@ @<Initialize variables as |ship_out| begins@>=
@y
@ @<Initialize variables as |ship_out| begins@>=
@<Calculate DVI page dimensions and margins@>;
@z

@x
@!old_setting:0..max_selector; {saved |selector| setting}
begin if tracing_output>0 then
@y
@!old_setting:0..max_selector; {saved |selector| setting}
@!t: scaled;
begin if tracing_output>0 then
@z

@x
@d set_language_code=5 {command modifier for \.{\\setlanguage}}
@y
@d set_language_code=5 {command modifier for \.{\\setlanguage}}
@d TeX_extension_end=5 
@d pdf_save_pos_node=TeX_extension_end+1
@z

@x
  set_language_code:print_esc("setlanguage");
@y
  set_language_code:print_esc("setlanguage");
  pdf_save_pos_node: print_esc("pdfsavepos");
@z

@x
set_language_code:@<Implement \.{\\setlanguage}@>;
@y
set_language_code:@<Implement \.{\\setlanguage}@>;
pdf_save_pos_node: @<Implement \.{\\pdfsavepos}@>;
@z

@x
  print_int(what_lhm(p)); print_char(",");
  print_int(what_rhm(p)); print_char(")");
  end;
@y
  print_int(what_lhm(p)); print_char(",");
  print_int(what_rhm(p)); print_char(")");
  end;
pdf_save_pos_node: print_esc("pdfsavepos");
@z

@x
close_node,language_node: begin r:=get_node(small_node_size);
  words:=small_node_size;
  end;
@y
close_node,language_node: begin r:=get_node(small_node_size);
  words:=small_node_size;
  end;
pdf_save_pos_node:
   r := get_node(small_node_size);
@z

@x
close_node,language_node: free_node(p,small_node_size);
@y
close_node,language_node: free_node(p,small_node_size);
pdf_save_pos_node: free_node(p, small_node_size);
@z

@x
procedure special_out(@!p:pointer);
var old_setting:0..max_selector; {holds print |selector|}
@!k:pool_pointer; {index into |str_pool|}
@y
procedure special_out(@!p:pointer);
label done;
var old_setting:0..max_selector; {holds print |selector|}
@!k:pool_pointer; {index into |str_pool|}
@!s,@!t,@!cw: scaled;
@!bl: boolean;
@!i: small_number;
@z
 
@x
pool_ptr:=str_start[str_ptr]; {erase the string}
@y
@<Determine whether this \.{\\special} is a papersize special@>;
done: pool_ptr:=str_start[str_ptr]; {erase the string}
@z
 
@x
language_node:do_nothing;
@y
language_node:do_nothing;
pdf_save_pos_node:
  @<Save current position in DVI mode@>;
@z

@x
primitive("pdfstrcmp",convert,pdf_strcmp_code);@/
@!@:pdf_strcmp_}{\.{\\pdfstrcmp} primitive@>
@y
primitive("pdfstrcmp",convert,pdf_strcmp_code);@/
@!@:pdf_strcmp_}{\.{\\pdfstrcmp} primitive@>
primitive("pdfsavepos",extension,pdf_save_pos_node);@/
@!@:pdf_save_pos_}{\.{\\pdfsavepos} primitive@>
primitive("pdfpagewidth",assign_dimen,dimen_base+pdf_page_width_code);@/
@!@:pdf_page_width_}{\.{\\pdfpagewidth} primitive@>
primitive("pdfpageheight",assign_dimen,dimen_base+pdf_page_height_code);@/
@!@:pdf_page_height_}{\.{\\pdfpageheight} primitive@>
primitive("pdflastxpos",last_item,pdf_last_x_pos_code);@/
@!@:pdf_last_x_pos_}{\.{\\pdflastxpos} primitive@>
primitive("pdflastypos",last_item,pdf_last_y_pos_code);@/
@!@:pdf_last_y_pos_}{\.{\\pdflastypos} primitive@>
@z
 
@x
eTeX_version_code: print_esc("eTeXversion");
@y
eTeX_version_code: print_esc("eTeXversion");
pdf_last_x_pos_code:  print_esc("pdflastxpos");
pdf_last_y_pos_code:  print_esc("pdflastypos");
@z
 
@x
eTeX_version_code: cur_val:=eTeX_version;
@y
eTeX_version_code: cur_val:=eTeX_version;
pdf_last_x_pos_code: cur_val := pdf_last_x_pos;
pdf_last_y_pos_code: cur_val := pdf_last_y_pos;
@z


@x
@* \[54] System-dependent changes.
@y
@ Next, we implement \.{\\pdfsavepos} and its related primitives.

@<Glob...@>=
@!cur_page_width: scaled; {width of page being shipped}
@!cur_page_height: scaled; {height of page being shipped}
@!pdf_last_x_pos: integer;
@!pdf_last_y_pos: integer;

@ @<Implement \.{\\pdfsavepos}@>=
begin
    new_whatsit(pdf_save_pos_node, small_node_size);
end

@ @<Save current position in DVI mode@>=
begin
  case dvi_dir of
  dir_yoko: begin pdf_last_x_pos := cur_h;  pdf_last_y_pos := cur_v;  end;
  dir_tate: begin pdf_last_x_pos := -cur_v; pdf_last_y_pos := cur_h;  end;
  dir_dtou: begin pdf_last_x_pos := cur_v;  pdf_last_y_pos := -cur_h; end;
  endcases;
  pdf_last_x_pos := pdf_last_x_pos + 4736286;
  case dvi_dir of
  dir_tate,dir_dtou: 
    pdf_last_y_pos := cur_page_width - pdf_last_y_pos - 4736286;
  dir_yoko:
    pdf_last_y_pos := cur_page_height - pdf_last_y_pos - 4736286;
  endcases;
  {4736286 = 1in, the funny DVI origin offset}
end

@ @<Calculate DVI page dimensions and margins@>=
  if pdf_page_height <> 0 then
    cur_page_height := pdf_page_height
  else
    cur_page_height := height(p) + depth(p) + 2*v_offset + 2*4736286;
  if pdf_page_width <> 0 then
    cur_page_width := pdf_page_width
  else
    cur_page_width := width(p) + 2*h_offset + 2*4736286;
  if (type(p)=dir_node) then
    if (box_dir(list_ptr(p))=dir_tate)or(box_dir(list_ptr(p))=dir_dtou) then begin 
      t:=cur_page_height; cur_page_height:=cur_page_width;
      cur_page_width:=t; end;


@ Of course \epTeX\ can produce a \.{DVI} file only, not a PDF file. 
A \.{DVI} file does not have the information of the page height,
which is needed to implement \.{\\pdflastypos} correctly.
To keep the information of the page height, I (H.~Kitagawa) 
adopted \.{\\pdfpageheight} primitive from pdf\TeX. However, in \pTeX (and \epTeX), 
the papersize special \.{\\special\{papersize=|width|,|height|\}} is commonly used 
for specifying page width/height. Hence, I decided that the papersize special also 
changes the value of \.{\\pdfpagewidth} and \.{\\pdfpageheight}. 
The following routine does this.

In present implementation, the ``valid'' papersize special, which can be interpreted by
this routine, is in the following form:
$$\hbox{.{\\special\{papersize=$x$pt,$y$pt\}}}$$
where $x$\,pt and $y$\, pt are positive dimensions which \TeX\ can comprehend.
No spaces are allowed in the above form, and only ``pt'' is allowed for unit, for simplicity.

@d ifps==if k=pool_ptr then goto done else if
@d sop(#)==so(str_pool[(#)])

@<Determine whether this \.{\\special} is a papersize special@>=
if pool_ptr-str_start[str_ptr]<=10 then goto done;
k:=str_start[str_ptr];
if (sop(k+0)<>'p')or(sop(k+1)<>'a')or(sop(k+2)<>'p')or 
   (sop(k+3)<>'e')or(sop(k+4)<>'r')or(sop(k+5)<>'s')or 
   (sop(k+6)<>'i')or(sop(k+7)<>'z')or(sop(k+8)<>'e') then goto done;
k:=k+9; ifps sop(k)='=' then incr(k);
@<Read dimensions in the argument in the papersize special@>;
incr(k); ifps sop(k)<>',' then goto done else incr(k); cw:=t;
@<Read dimensions in the argument in the papersize special@>;
geq_word_define(dimen_base+pdf_page_width_code,cw);
geq_word_define(dimen_base+pdf_page_height_code,t);
cur_page_height := t; cur_page_width := cw;
if (dvi_dir=dir_tate)or(dvi_dir=dir_dtou) then begin 
    t:=cur_page_height; cur_page_height:=cur_page_width;
    cur_page_width:=t; end;

@ @<Read dimensions in the argument in the papersize special@>=
s:=1; t:=0; bl:=true;
while (k<pool_ptr)and(bl)  do begin 
  if (sop(k)>='0')and (sop(k)<='9') then begin t:=10*t+sop(k)-'0'; incr(k); end
  else begin bl:=false; end;
end; 
t:=t*unity; 
ifps sop(k)='.' then begin incr(k); bl:=true; i:=0;
  while (k<pool_ptr)and(bl)and(i<=17)  do begin
    if (sop(k)>='0')and (sop(k)<='9') then begin 
      dig[i]:=sop(k)-'0'; incr(k); incr(i); end
    else begin bl:=false; incr(k); incr(i); end;
  end;
  t:=s*(t+round_decimals(i-1));
end 
else if (sop(k)>='0')and(sop(k)<='9') then goto done
else begin t:=s*t; incr(k); end;
ifps sop(k-1)<>'p' then goto done; ifps sop(k)<>'t' then goto done;

@* \[54] System-dependent changes.
@z
