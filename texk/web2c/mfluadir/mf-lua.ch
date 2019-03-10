@x [0] MFLua
% This program is copyright (C) 1984 by D. E. Knuth; all rights are reserved.
@y
% This program is MFLua, strictly based on the METAFONT 2.7182818 source code.
% What follow are the original comments of METAFONT 2.7182818.

% This program is copyright (C) 1984 by D. E. Knuth; all rights are reserved.
@z

@x [0] MFLua
\def\MF{{\tenlogo META}\-{\tenlogo FONT}}
@y
%\def\MF{{\tenlogo META}\-{\tenlogo FONT}}
\def\MF{{\tenlogo MF}{\mc LUA}}
@z

@x [1.2] MFLua
@d banner=='This is METAFONT, Version 2.7182818' {printed when \MF\ starts}
@y
@d METAFONT_banner=='This is METAFONT, Version 2.7182818' {printed when \MF\ starts}
@#
@d MFLua_version_string=='-0.9.1'
@#
@d MFLua_banner=='This is MFLua, Version 2.7182818', MFLua_version_string
  {printed when MFLua starts}
@#
@d banner==MFLua_banner
@z

@x [12.186] MFLua - runscript primitive
At any rate, here is the list, for future reference.

@d if_test=1 {conditional text (\&{if})}
@d fi_or_else=2 {delimiters for conditionals (\&{elseif}, \&{else}, \&{fi})}
@d input=3 {input a source file (\&{input}, \&{endinput})}
@d iteration=4 {iterate (\&{for}, \&{forsuffixes}, \&{forever}, \&{endfor})}
@d repeat_loop=5 {special command substituted for \&{endfor}}
@d exit_test=6 {premature exit from a loop (\&{exitif})}
@d relax=7 {do nothing (\.{\char`\\})}
@d scan_tokens=8 {put a string into the input buffer}
@d expand_after=9 {look ahead one token}
@d defined_macro=10 {a macro defined by the user}
@d min_command=defined_macro+1
@d display_command=11 {online graphic output (\&{display})}
@d save_command=12 {save a list of tokens (\&{save})}
@d interim_command=13 {save an internal quantity (\&{interim})}
@d let_command=14 {redefine a symbolic token (\&{let})}
@d new_internal=15 {define a new internal quantity (\&{newinternal})}
@d macro_def=16 {define a macro (\&{def}, \&{vardef}, etc.)}
@d ship_out_command=17 {output a character (\&{shipout})}
@d add_to_command=18 {add to edges (\&{addto})}
@d cull_command=19 {cull and normalize edges (\&{cull})}
@d tfm_command=20 {command for font metric info (\&{ligtable}, etc.)}
@d protection_command=21 {set protection flag (\&{outer}, \&{inner})}
@d show_command=22 {diagnostic output (\&{show}, \&{showvariable}, etc.)}
@d mode_command=23 {set interaction level (\&{batchmode}, etc.)}
@d random_seed=24 {initialize random number generator (\&{randomseed})}
@d message_command=25 {communicate to user (\&{message}, \&{errmessage})}
@d every_job_command=26 {designate a starting token (\&{everyjob})}
@d delimiters=27 {define a pair of delimiters (\&{delimiters})}
@d open_window=28 {define a window on the screen (\&{openwindow})}
@d special_command=29 {output special info (\&{special}, \&{numspecial})}
@d type_name=30 {declare a type (\&{numeric}, \&{pair}, etc.)}
@d max_statement_command=type_name
@d min_primary_command=type_name
@d left_delimiter=31 {the left delimiter of a matching pair}
@d begin_group=32 {beginning of a group (\&{begingroup})}
@d nullary=33 {an operator without arguments (e.g., \&{normaldeviate})}
@d unary=34 {an operator with one argument (e.g., \&{sqrt})}
@d str_op=35 {convert a suffix to a string (\&{str})}
@d cycle=36 {close a cyclic path (\&{cycle})}
@d primary_binary=37 {binary operation taking `\&{of}' (e.g., \&{point})}
@d capsule_token=38 {a value that has been put into a token list}
@d string_token=39 {a string constant (e.g., |"hello"|)}
@d internal_quantity=40 {internal numeric parameter (e.g., \&{pausing})}
@d min_suffix_token=internal_quantity
@d tag_token=41 {a symbolic token without a primitive meaning}
@d numeric_token=42 {a numeric constant (e.g., \.{3.14159})}
@d max_suffix_token=numeric_token
@d plus_or_minus=43 {either `\.+' or `\.-'}
@d max_primary_command=plus_or_minus {should also be |numeric_token+1|}
@d min_tertiary_command=plus_or_minus
@d tertiary_secondary_macro=44 {a macro defined by \&{secondarydef}}
@d tertiary_binary=45 {an operator at the tertiary level (e.g., `\.{++}')}
@d max_tertiary_command=tertiary_binary
@d left_brace=46 {the operator `\.{\char`\{}'}
@d min_expression_command=left_brace
@d path_join=47 {the operator `\.{..}'}
@d ampersand=48 {the operator `\.\&'}
@d expression_tertiary_macro=49 {a macro defined by \&{tertiarydef}}
@d expression_binary=50 {an operator at the expression level (e.g., `\.<')}
@d equals=51 {the operator `\.='}
@d max_expression_command=equals
@d and_command=52 {the operator `\&{and}'}
@d min_secondary_command=and_command
@d secondary_primary_macro=53 {a macro defined by \&{primarydef}}
@d slash=54 {the operator `\./'}
@d secondary_binary=55 {an operator at the binary level (e.g., \&{shifted})}
@d max_secondary_command=secondary_binary
@d param_type=56 {type of parameter (\&{primary}, \&{expr}, \&{suffix}, etc.)}
@d controls=57 {specify control points explicitly (\&{controls})}
@d tension=58 {specify tension between knots (\&{tension})}
@d at_least=59 {bounded tension value (\&{atleast})}
@d curl_command=60 {specify curl at an end knot (\&{curl})}
@d macro_special=61 {special macro operators (\&{quote}, \.{\#\AT!}, etc.)}
@d right_delimiter=62 {the right delimiter of a matching pair}
@d left_bracket=63 {the operator `\.['}
@d right_bracket=64 {the operator `\.]'}
@d right_brace=65 {the operator `\.{\char`\}}'}
@d with_option=66 {option for filling (\&{withpen}, \&{withweight})}
@d cull_op=67 {the operator `\&{keeping}' or `\&{dropping}'}
@d thing_to_add=68
  {variant of \&{addto} (\&{contour}, \&{doublepath}, \&{also})}
@d of_token=69 {the operator `\&{of}'}
@d from_token=70 {the operator `\&{from}'}
@d to_token=71 {the operator `\&{to}'}
@d at_token=72 {the operator `\&{at}'}
@d in_window=73 {the operator `\&{inwindow}'}
@d step_token=74 {the operator `\&{step}'}
@d until_token=75 {the operator `\&{until}'}
@d lig_kern_token=76
  {the operators `\&{kern}' and `\.{=:}' and `\.{=:\char'174}', etc.}
@d assignment=77 {the operator `\.{:=}'}
@d skip_to=78 {the operation `\&{skipto}'}
@d bchar_label=79 {the operator `\.{\char'174\char'174:}'}
@d double_colon=80 {the operator `\.{::}'}
@d colon=81 {the operator `\.:'}
@#
@d comma=82 {the operator `\.,', must be |colon+1|}
@d end_of_statement==cur_cmd>comma
@d semicolon=83 {the operator `\.;', must be |comma+1|}
@d end_group=84 {end a group (\&{endgroup}), must be |semicolon+1|}
@d stop=85 {end a job (\&{end}, \&{dump}), must be |end_group+1|}
@d max_command_code=stop
@d outer_tag=max_command_code+1 {protection code added to command code}
@y
At any rate, here is the list, for future reference.

@d delta_code=1 {n. of new primitives added }
@d if_test=1 {conditional text (\&{if})}
@d fi_or_else=2 {delimiters for conditionals (\&{elseif}, \&{else}, \&{fi})}
@d input=3 {input a source file (\&{input}, \&{endinput})}
@d iteration=4 {iterate (\&{for}, \&{forsuffixes}, \&{forever}, \&{endfor})}
@d repeat_loop=5 {special command substituted for \&{endfor}}
@d exit_test=6 {premature exit from a loop (\&{exitif})}
@d relax=7 {do nothing (\.{\char`\\})}
@d scan_tokens=8 {put a string into the input buffer}
@d run_script=8+delta_code {execute a Lua script}
@d expand_after=9+delta_code {look ahead one token}
@d defined_macro=10+delta_code {a macro defined by the user}
@d min_command=defined_macro+1
@d display_command=11+delta_code {online graphic output (\&{display})}
@d save_command=12+delta_code {save a list of tokens (\&{save})}
@d interim_command=13+delta_code {save an internal quantity (\&{interim})}
@d let_command=14+delta_code {redefine a symbolic token (\&{let})}
@d new_internal=15+delta_code {define a new internal quantity (\&{newinternal})}
@d macro_def=16+delta_code {define a macro (\&{def}, \&{vardef}, etc.)}
@d ship_out_command=17+delta_code {output a character (\&{shipout})}
@d add_to_command=18+delta_code {add to edges (\&{addto})}
@d cull_command=19+delta_code {cull and normalize edges (\&{cull})}
@d tfm_command=20+delta_code {command for font metric info (\&{ligtable}, etc.)}
@d protection_command=21+delta_code {set protection flag (\&{outer}, \&{inner})}
@d show_command=22+delta_code {diagnostic output (\&{show}, \&{showvariable}, etc.)}
@d mode_command=23+delta_code {set interaction level (\&{batchmode}, etc.)}
@d random_seed=24+delta_code {initialize random number generator (\&{randomseed})}
@d message_command=25+delta_code {communicate to user (\&{message}, \&{errmessage})}
@d every_job_command=26+delta_code {designate a starting token (\&{everyjob})}
@d delimiters=27+delta_code {define a pair of delimiters (\&{delimiters})}
@d open_window=28+delta_code {define a window on the screen (\&{openwindow})}
@d special_command=29+delta_code {output special info (\&{special}, \&{numspecial})}
@d type_name=30+delta_code {declare a type (\&{numeric}, \&{pair}, etc.)}
@d max_statement_command=type_name
@d min_primary_command=type_name
@d left_delimiter=31+delta_code {the left delimiter of a matching pair}
@d begin_group=32+delta_code {beginning of a group (\&{begingroup})}
@d nullary=33+delta_code {an operator without arguments (e.g., \&{normaldeviate})}
@d unary=34+delta_code {an operator with one argument (e.g., \&{sqrt})}
@d str_op=35+delta_code {convert a suffix to a string (\&{str})}
@d cycle=36+delta_code {close a cyclic path (\&{cycle})}
@d primary_binary=37+delta_code {binary operation taking `\&{of}' (e.g., \&{point})}
@d capsule_token=38+delta_code {a value that has been put into a token list}
@d string_token=39+delta_code {a string constant (e.g., |"hello"|)}
@d internal_quantity=40+delta_code {internal numeric parameter (e.g., \&{pausing})}
@d min_suffix_token=internal_quantity
@d tag_token=41+delta_code {a symbolic token without a primitive meaning}
@d numeric_token=42+delta_code {a numeric constant (e.g., \.{3.14159})}
@d max_suffix_token=numeric_token
@d plus_or_minus=43+delta_code {either `\.+' or `\.-'}
@d max_primary_command=plus_or_minus {should also be |numeric_token+1|}
@d min_tertiary_command=plus_or_minus
@d tertiary_secondary_macro=44+delta_code {a macro defined by \&{secondarydef}}
@d tertiary_binary=45+delta_code {an operator at the tertiary level (e.g., `\.{++}')}
@d max_tertiary_command=tertiary_binary
@d left_brace=46+delta_code {the operator `\.{\char`\{}'}
@d min_expression_command=left_brace
@d path_join=47+delta_code {the operator `\.{..}'}
@d ampersand=48+delta_code {the operator `\.\&'}
@d expression_tertiary_macro=49+delta_code {a macro defined by \&{tertiarydef}}
@d expression_binary=50+delta_code {an operator at the expression level (e.g., `\.<')}
@d equals=51+delta_code {the operator `\.='}
@d max_expression_command=equals
@d and_command=52+delta_code {the operator `\&{and}'}
@d min_secondary_command=and_command
@d secondary_primary_macro=53+delta_code {a macro defined by \&{primarydef}}
@d slash=54+delta_code {the operator `\./'}
@d secondary_binary=55+delta_code {an operator at the binary level (e.g., \&{shifted})}
@d max_secondary_command=secondary_binary
@d param_type=56+delta_code {type of parameter (\&{primary}, \&{expr}, \&{suffix}, etc.)}
@d controls=57+delta_code {specify control points explicitly (\&{controls})}
@d tension=58+delta_code {specify tension between knots (\&{tension})}
@d at_least=59+delta_code {bounded tension value (\&{atleast})}
@d curl_command=60+delta_code {specify curl at an end knot (\&{curl})}
@d macro_special=61+delta_code {special macro operators (\&{quote}, \.{\#\AT!}, etc.)}
@d right_delimiter=62+delta_code {the right delimiter of a matching pair}
@d left_bracket=63+delta_code {the operator `\.['}
@d right_bracket=64+delta_code {the operator `\.]'}
@d right_brace=65+delta_code {the operator `\.{\char`\}}'}
@d with_option=66+delta_code {option for filling (\&{withpen}, \&{withweight})}
@d cull_op=67+delta_code {the operator `\&{keeping}' or `\&{dropping}'}
@d thing_to_add=68+delta_code
  {variant of \&{addto} (\&{contour}, \&{doublepath}, \&{also})}
@d of_token=69+delta_code {the operator `\&{of}'}
@d from_token=70+delta_code {the operator `\&{from}'}
@d to_token=71+delta_code {the operator `\&{to}'}
@d at_token=72+delta_code {the operator `\&{at}'}
@d in_window=73+delta_code {the operator `\&{inwindow}'}
@d step_token=74+delta_code {the operator `\&{step}'}
@d until_token=75+delta_code {the operator `\&{until}'}
@d lig_kern_token=76+delta_code
  {the operators `\&{kern}' and `\.{=:}' and `\.{=:\char'174}', etc.}
@d assignment=77+delta_code {the operator `\.{:=}'}
@d skip_to=78+delta_code {the operation `\&{skipto}'}
@d bchar_label=79+delta_code {the operator `\.{\char'174\char'174:}'}
@d double_colon=80+delta_code {the operator `\.{::}'}
@d colon=81+delta_code {the operator `\.:'}
@#
@d comma=82+delta_code {the operator `\.,', must be |colon+1|}
@d end_of_statement==cur_cmd>comma
@d semicolon=83+delta_code {the operator `\.;', must be |comma+1|}
@d end_group=84+delta_code {end a group (\&{endgroup}), must be |semicolon+1|}
@d stop=85+delta_code {end a job (\&{end}, \&{dump}), must be |end_group+1|}
@d max_command_code=stop
@d outer_tag=max_command_code+1 {protection code added to command code}
@z

@x [13.211] MFLua - runscript primitive
primitive("randomseed",random_seed,0);@/
@!@:random_seed_}{\&{randomseed} primitive@>
@y
primitive("randomseed",random_seed,0);@/
@!@:random_seed_}{\&{randomseed} primitive@>
primitive("runscript",run_script,0);@/
@!@:run_script_}{\&{runscript} primitive@>
@z

@x [13.212] MFLua - runscript primitive
right_bracket:print("]");
@y
right_bracket:print("]");
run_script:print("runscript");
@z

@x [17.257] MFLua
begin print_diagnostic("Path",s,nuline); print_ln;
@y
begin 
mflua_printpath(h,s,nuline);
print_diagnostic("Path",s,nuline); print_ln;
@z

@x [20.332] MFLua
begin print_diagnostic("Edge structure",s,nuline);
@y
begin
{mflua\_printedges(s,nuline,x\_off,y\_off);}
print_diagnostic("Edge structure",s,nuline);
@z

@x [22.465] MFLua
  move_to_edges(m0,n0,m1,n1);
@y
  mflua_PRE_move_to_edges(p);
  move_to_edges(m0,n0,m1,n1);
  mflua_POST_move_to_edges(p);
@z

@x [24.491] MFLua
begin p:=c; n:=info(h); lh:=link(h); {now |lh| points to $w_0$}
@y
begin
{mflua\_offsetprep(c,h);}
p:=c; n:=info(h); lh:=link(h); {now |lh| points to $w_0$}
@z

@x [24.506] MFLua
offset_prep(p,h); {this may clobber node~|q|, if it becomes ``dead''}
@y
mfluaPRE_offset_prep(p,h);
offset_prep(p,h); {this may clobber node~|q|, if it becomes ``dead''}
mfluaPOST_offset_prep(p,h);
@z

@x [24.519] MFLua
  unskew(x1,y1,octant);@/
@y
  unskew(x1,y1,octant);@/
  mflua_print_retrograde_line(x0,y0,cur_x,cur_y);
@z

@x [24.515] MFLua
begin xx:=x_coord(r)+x_coord(w); yy:=y_coord(r)+y_coord(w)+half_unit;
@y
begin xx:=x_coord(r)+x_coord(w); yy:=y_coord(r)+y_coord(w)+half_unit;
mflua_print_transition_line_from(xx,yy-half_unit);
@z

@x [24.515] MFLua
else  begin decr(k); w:=knil(w);
  xp:=x_coord(r)+x_coord(w); yp:=y_coord(r)+y_coord(w)+half_unit;
  end;
@y
else  begin decr(k); w:=knil(w);
  xp:=x_coord(r)+x_coord(w); yp:=y_coord(r)+y_coord(w)+half_unit;
  end;
mflua_print_transition_line_to(xp,yp-half_unit);
@z

@x [24.521] MFLua
@<Insert a line segment dually to approach the correct offset@>=
begin xx:=x_coord(r)+x_coord(w); yy:=y_coord(r)+y_coord(w)+half_unit;
@y
@<Insert a line segment dually to approach the correct offset@>=
begin xx:=x_coord(r)+x_coord(w); yy:=y_coord(r)+y_coord(w)+half_unit;
mflua_print_transition_line_from(xx,yy-half_unit);
@z

@x [24.521] MFLua
else  begin incr(k); w:=link(w);
  xp:=x_coord(r)+x_coord(w); yp:=y_coord(r)+y_coord(w)+half_unit;
  end;
@y
else  begin incr(k); w:=link(w);
  xp:=x_coord(r)+x_coord(w); yp:=y_coord(r)+y_coord(w)+half_unit;
  end;
mflua_print_transition_line_to(xp,yp-half_unit);
@z

@x [31.630] MFLua - runscript primitive
moved into the buffer by \&{scantokens}.
@y
moved into the buffer by \&{scantokens} or \&{runscript}.
@z

@x [31.637] MFLua - runscript primitive
else if name=2 then print_nl("<scantokens>")
@y
else if name=2 then print_nl("<scantokens> or <runscript>")
@z

@x [33.679] MFLua - runscript primitive
     {text was inserted during error recovery or by \&{scantokens}}
@y
     {text was inserted during error recovery or by \&{scantokens} or \&{runscript}}
@z

@x [35.706] MFLua - runscript primitive
|exit_test|, |relax|, |scan_tokens|, |expand_after|, and |defined_macro|.
@y
|exit_test|, |relax|, |scan_tokens|, |run_script|, |expand_after|, and |defined_macro|.
@z

@x [35.707] MFLua - runscript primitive
scan_tokens: @<Put a string into the input buffer@>;
@y
scan_tokens: @<Put a string into the input buffer@>;
run_script:  @<Pass a string to the Lua interpreter and put the result string into the input buffer@>;
@z

@x [39.796] MFLua - runscript primitive
@!cur_exp:integer; {the value of the expression just found}
@y
@!cur_exp:integer; {the value of the expression just found}
@!lua_cur_exp:integer; {the value of the Lua expression returned by runscript}
@z

@x [41.866] MFLua
q:=make_ellipse(major_axis,minor_axis,theta);
if (tx<>0)or(ty<>0) then @<Shift the coordinates of path |q|@>;
@y
mfluaPRE_make_ellipse(major_axis,minor_axis,theta,tx,ty,0);
q:=make_ellipse(major_axis,minor_axis,theta);
if (tx<>0)or(ty<>0) then @<Shift the coordinates of path |q|@>;
mfluaPOST_make_ellipse(major_axis,minor_axis,theta,tx,ty,q);
@z

@x [41.891] MFLua
make_choices(p);
@y
mfluaPRE_make_choices(p);
make_choices(p);
mfluaPOST_make_choices(p);
@z

@x [44.1064] MFLua
  cur_wt:=w; rhs:=make_spec(rhs,max_offset(cur_pen),internal[tracing_specs]);
  @<Check the turning number@>;
  if max_offset(cur_pen)=0 then fill_spec(rhs)
  else fill_envelope(rhs);
  if lhs<>null then
    begin rev_turns:=true;
    lhs:=make_spec(lhs,max_offset(cur_pen),internal[tracing_specs]);
    rev_turns:=false;
    if max_offset(cur_pen)=0 then fill_spec(lhs)
    else fill_envelope(lhs);
@y
  cur_wt:=w; mfluaPRE_make_spec_rhs(rhs); rhs:=make_spec(rhs,max_offset(cur_pen),internal[tracing_specs]);mfluaPOST_make_spec_rhs(rhs);
  @<Check the turning number@>;
  if max_offset(cur_pen)=0 then begin mfluaPRE_fill_spec_rhs(rhs); fill_spec(rhs); mfluaPOST_fill_spec_rhs(rhs);end 
  else begin mfluaPRE_fill_envelope_rhs(rhs); fill_envelope(rhs); mfluaPOST_fill_envelope_rhs(rhs); end ;
  if lhs<>null then
    begin rev_turns:=true;
    mfluaPRE_make_spec_lhs(lhs);
    lhs:=make_spec(lhs,max_offset(cur_pen),internal[tracing_specs]);
    mfluaPOST_make_spec_lhs(lhs);
    rev_turns:=false;
    if max_offset(cur_pen)=0 then begin mfluaPRE_fill_spec_lhs(lhs); fill_spec(lhs); mfluaPOST_fill_spec_lhs(lhs); end
    else begin mfluaPRE_fill_envelope_lhs(lhs); fill_envelope(lhs); mfluaPOST_fill_envelope_lhs(lhs); end;
@z

@x [47.1165] MFLua
print_char("]"); update_terminal; {progress report}
@y
print_char("]"); update_terminal; {progress report}
mflua_printedges(" (just shipped out)",true,x_off,y_off);
@z

@x [49.1204] MFLua
@p begin @!{|start_here|}
@y
@p begin @!{|start_here|}
mflua_begin_program;
@z

@x [49.1204] MFLua
start_of_MF: @<Initialize the output routines@>;
@<Get the first line of input and prepare to start@>;
history:=spotless; {ready to go!}
if start_sym>0 then {insert the `\&{everyjob}' symbol}
  begin cur_sym:=start_sym; back_input;
  end;
main_control; {come to life}
final_cleanup; {prepare for death}
@y
mfluaPRE_start_of_MF;
start_of_MF: @<Initialize the output routines@>;
@<Get the first line of input and prepare to start@>;
history:=spotless; {ready to go!}
{This was the first sensor. Original comment: "remember to modify web2c/texmf.defines by adding mfuainitialize and then texmfmp.h and lib/texmfmp.c". Now obsolete}
mflua_initialize;
if start_sym>0 then {insert the `\&{everyjob}' symbol}
  begin cur_sym:=start_sym; back_input;
  end;
mfluaPRE_main_control;
main_control; {come to life}
mfluaPOST_main_control;
final_cleanup; {prepare for death}
mfluaPOST_final_cleanup;
@z

@x [49.1205] MFLua
  end;
end;
@y
  end;
mflua_end_program;
end;
@z

@x [51.1214] MFLua - additional sections to handle runscript
@* \[51] System-dependent changes.
@y
@ @<Pass a string to the Lua interpreter and put the result string into the input buffer@>=
begin get_x_next; scan_primary;
if cur_type<>string_type then
  begin disp_err(null,"Not a string");
@.Not a string@>
  help2("I'm going to flush this expression, since")@/
    ("runscript should be followed by a known string.");
  put_get_flush_error(0);
  end
else  begin back_input;
  if length(cur_exp)>0 then @<As for scantokens, pretend we're reading a new one-line file@>;
  end;
end

@ @<As for scantokens, pretend we're reading a new one-line file@>=
begin {begin\_file\_reading;} name:=2; {should be name:=3 but for the moment we keep 2}
k:=first+length(cur_exp);
j:=str_start[cur_exp]; limit:=k;
mflua_runscript(j,first,limit);
lua_cur_exp:=make_string; 
if length(lua_cur_exp)> 0 then 
begin
 begin_file_reading;
 add_str_ref(lua_cur_exp);
 j:=str_start[lua_cur_exp]; k:=first+length(lua_cur_exp);limit:=k;
 if k>=max_buf_stack then
 begin if k>=buf_size then
   begin max_buf_stack:=buf_size;
    overflow("buffer size",buf_size);
@:METAFONT capacity exceeded buffer size}{\quad buffer size@>
   end;
 max_buf_stack:=k+1;
 end;
 while first<limit do
  begin 
   {print("  loop");print\_ln;}
   buffer[first]:=so(str_pool[j]); {print(so(str\_pool[j]));} incr(j); incr(first);
  end;
  buffer[limit]:="%"; first:=limit+1; loc:=start; 
  delete_str_ref(lua_cur_exp);
end;
delete_str_ref(lua_cur_exp);
{flush_string(lua_cur_exp);}
flush_cur_exp(0);
end
@* \[51] System-dependent changes.
@z