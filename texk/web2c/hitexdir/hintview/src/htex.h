/*1410:*/
#line 25709 "htex.w"

#define banner "This is TeX, Version 3.141592653 (HINT)" \

#define odd(X) ((X) &1) 
#define chr(X) ((unsigned char) (X) ) 
#define ord(X) ((unsigned int) (X) ) 
#define abs(X) ((X) > -(X) ?(X) :-(X) ) 
#define round(X) ((int) ((X) >=0.0?floor((X) +0.5) :ceil((X) -0.5) ) )  \

#define mem_bot 0 \

#define mem_top mem_max \
 \

#define font_base 0 \

#define hash_size 15000 \

#define hash_prime 12721
#define hyph_size 607 \
 \

#define incr(A) A= A+1
#define decr(A) A= A-1
#define negate(A) A= -A
#define loop while(true) 
#define do_nothing 
#define empty 0 \

#define text_char unsigned char
#define first_text_char 0
#define last_text_char 255 \

#define null_code 00
#define carriage_return 015
#define invalid_code 0177 \

#define reset_OK(A) erstat(A) ==0
#define rewrite_OK(A) erstat(A) ==0 \

#define t_open_in term_in.f= stdin
#define t_open_out term_out.f= stdout \

#define update_terminal fflush(term_out.f) 
#define clear_terminal fflush(term_in.f) 
#define wake_up_terminal do_nothing \

#define loc cur_input.loc_field \

#define si(A) A
#define so(A) A \

#define length(A) (str_start[A+1]-str_start[A])  \
 \

#define cur_length (pool_ptr-str_start[str_ptr])  \

#define append_char(A)  \
{str_pool[pool_ptr]= si(A) ;incr(pool_ptr) ; \
}
#define flush_char decr(pool_ptr) 
#define str_room(A)  \
{if(pool_ptr+A> pool_size)  \
overflow("pool size",pool_size-init_pool_ptr) ; \
 \
} \

#define flush_string {decr(str_ptr) ;pool_ptr= str_start[str_ptr]; \
} \

#define app_lc_hex(A) l= A; \
if(l<10) append_char(l+'0') else append_char(l-10+'a')  \

#define no_print 16
#define term_only 17
#define log_only 18
#define term_and_log 19
#define pseudo 20
#define new_string 21
#define max_selector 21 \

#define print_lc_hex(A)  \
if((A) <10) print_char((A) +'0') ;else print_char((A) -10+'a') 
#define print_ASCII(k)  \
if((k<' ') ||(k> '~') )  \
{print("^^") ; \
if(k<0100) print_char(k+0100) ; \
else if(k<0200) print_char(k-0100) ; \
else{print_lc_hex(k/16) ;print_lc_hex(k%16) ;} \
} \
else print_char(k)  \

#define prompt_input(A) {wake_up_terminal;print(A) ;term_input() ; \
} \

#define batch_mode 0
#define nonstop_mode 1
#define scroll_mode 2
#define error_stop_mode 3 \

#define spotless 0
#define warning_issued 1
#define error_message_issued 2
#define fatal_error_stop 3 \

#define hlp1(A) help_line[0]= A;}
#define hlp2(A,B) help_line[1]= A;help_line[0]= B;}
#define hlp3(A,B,C) help_line[2]= A;help_line[1]= B;help_line[0]= C;}
#define hlp4(A,B,C,D) help_line[3]= A;help_line[2]= B;help_line[1]= C;help_line[0]= D;}
#define hlp5(A,B,C,D,E) help_line[4]= A;help_line[3]= B;help_line[2]= C;help_line[1]= D;help_line[0]= E;}
#define hlp6(A,B,C,D,E,F) help_line[5]= A;help_line[4]= B;help_line[3]= C;help_line[2]= D;help_line[1]= E;help_line[0]= F;}
#define help0 help_ptr= 0
#define help1(A) {help_ptr= 1;hlp1(A) 
#define help2(A,B) {help_ptr= 2;hlp2(A,B) 
#define help3(A,B,C) {help_ptr= 3;hlp3(A,B,C) 
#define help4(A,B,C,D) {help_ptr= 4;hlp4(A,B,C,D) 
#define help5(A,B,C,D,E) {help_ptr= 5;hlp5(A,B,C,D,E) 
#define help6(A,B,C,D,E,F) {help_ptr= 6;hlp6(A,B,C,D,E,F)  \

#define succumb {if(interaction==error_stop_mode)  \
interaction= scroll_mode; \
if(log_opened) error() ; \
if(interaction> batch_mode) debug_help() ; \
history= fatal_error_stop;jump_out() ; \
} \

#define check_interrupt {if(interrupt!=0) pause_for_instructions() ; \
} \

#define unity 0200000
#define two 0400000 \

#define nx_plus_y(A,B,C) mult_and_add(A,B,C,07777777777) 
#define mult_integers(A,B) mult_and_add(A,B,0,017777777777)  \

#define inf_bad 10000 \

#define set_glue_ratio_zero(A) A= 0.0
#define set_glue_ratio_one(A) A= 1.0
#define unfix(A) ((double) (A) ) 
#define fix(A) ((glue_ratio) (A) ) 
#define float_constant(A) ((double) (A) )  \

#define min_quarterword 0
#define max_quarterword 255
#define min_halfword 0
#define max_halfword 65535 \

#define qi(A) A+min_quarterword \

#define qo(A) A-min_quarterword \

#define hi(A) A+min_halfword \

#define ho(A) A-min_halfword \
 \

#define sc i \

#define pointer halfword
#define null min_halfword \

#define link(A) mem[A].hh.rh
#define info(A) mem[A].hh.lh \

#define mem_end mem_top
#define free_avail(A)  \
{link(A) = avail;avail= A; \
decr_dyn_used; \
} \

#define fast_get_avail(A)  \
{A= avail; \
if(A==null) A= get_avail() ; \
else{avail= link(A) ;link(A) = null; \
incr_dyn_used; \
} \
} \

#define empty_flag max_halfword
#define is_empty(A) (link(A) ==empty_flag) 
#define node_size(A) info(A) 
#define llink(A) info(A+1) 
#define rlink(A) link(A+1)  \

#define type(A) mem[A].hh.b0
#define subtype(A) mem[A].hh.b1 \

#define is_char_node(A) (A>=hi_mem_min)  \

#define font(A) type(A) 
#define character(A) subtype(A)  \

#define hlist_node 0
#define box_node_size 9
#define width_offset 1
#define depth_offset 2
#define height_offset 3
#define width(A) mem[A+width_offset].sc
#define depth(A) mem[A+depth_offset].sc
#define height(A) mem[A+height_offset].sc
#define shift_amount(A) mem[A+4].sc
#define list_offset 5
#define list_ptr(A) link(A+list_offset) 
#define glue_order(A) subtype(A+list_offset) 
#define glue_sign(A) type(A+list_offset) 
#define normal 0
#define stretching 1
#define shrinking 2
#define glue_offset 6
#define glue_set(A) mem[A+glue_offset].gr \
 \

#define vlist_node 1 \

#define rule_node 2
#define rule_node_size 4
#define null_flag -010000000000
#define is_running(A) (A==null_flag)  \

#define ins_node 3
#define ins_node_size 5
#define float_cost(A) mem[A+1].i
#define ins_ptr(A) info(A+4) 
#define split_top_ptr(A) link(A+4)  \

#define mark_node 4
#define small_node_size 2
#define mark_ptr(A) mem[A+1].i \

#define adjust_node 5
#define adjust_ptr(A) mark_ptr(A)  \

#define ligature_node 6
#define lig_char(A) A+1
#define lig_ptr(A) link(lig_char(A) )  \

#define disc_node 7
#define replace_count(A) (subtype(A) &0x7F) 
#define set_replace_count(A,B) (subtype(A) = (B) &0x7F) 
#define set_auto_disc(A) (subtype(A) |= 0x80) 
#define is_auto_disc(A) (subtype(A) &0x80) 
#define pre_break(A) llink(A) 
#define post_break(A) rlink(A)  \

#define whatsit_node 8 \

#define math_node 9
#define before 0
#define after 1 \

#define precedes_break(A) (type(A) <math_node) 
#define non_discardable(A) (type(A) <math_node)  \

#define glue_node 10
#define cond_math_glue 98
#define mu_glue 99
#define a_leaders 100
#define c_leaders 101
#define x_leaders 102
#define glue_ptr(A) llink(A) 
#define leader_ptr(A) rlink(A)  \

#define glue_spec_size 4
#define glue_ref_count(A) link(A) 
#define stretch(A) mem[A+2].sc
#define shrink(A) mem[A+3].sc
#define stretch_order(A) type(A) 
#define shrink_order(A) subtype(A) 
#define fil 1
#define fill 2
#define filll 3 \

#define kern_node 11
#define explicit 1
#define acc_kern 2 \

#define penalty_node 12
#define inf_penalty inf_bad
#define eject_penalty (-inf_penalty) 
#define penalty(A) mem[A+1].i \

#define unset_node 13
#define glue_stretch(A) mem[A+glue_offset].sc
#define glue_shrink(A) shift_amount(A) 
#define span_count(A) subtype(A)  \

#define zero_glue mem_bot
#define lo_mem_stat_max zero_glue+glue_spec_size-1 \

#define page_ins_head mem_top
#define contrib_head mem_top-1
#define page_head mem_top-2
#define temp_head mem_top-3
#define hold_head mem_top-4
#define adjust_head mem_top-5
#define active mem_top-7
#define align_head mem_top-8
#define end_span mem_top-9
#define omit_template mem_top-10
#define null_list mem_top-11
#define lig_trick mem_top-12
#define garbage mem_top-12
#define backup_head mem_top-13
#define hi_mem_stat_min mem_top-13 \

#define hi_mem_stat_usage 14 \

#define node_list_display(A)  \
{depth_level++;show_node_list(A) ;depth_level--; \
}
#define print_current_indent  \
{int i; \
for(i= 0;i<depth_level;i++)  \
print_char('.') ;} \

#define token_ref_count(A) info(A)  \

#define fast_delete_glue_ref(A)  \
{if(glue_ref_count(A) ==null) free_node(A,glue_spec_size) ; \
else decr(glue_ref_count(A) ) ; \
} \

#define add_token_ref(A) incr(token_ref_count(A) ) 
#define add_glue_ref(A) incr(glue_ref_count(A) ) 
#define add_xdimen_ref(A) if(A!=null) incr(xdimen_ref_count(A) )  \

#define escape 0 \

#define relax 0
#define left_brace 1
#define right_brace 2
#define math_shift 3
#define tab_mark 4
#define car_ret 5
#define out_param 5
#define mac_param 6
#define sup_mark 7
#define sub_mark 8
#define ignore 9
#define endv 9
#define spacer 10
#define letter 11
#define other_char 12
#define active_char 13
#define par_end 13
#define match 13
#define comment 14
#define end_match 14
#define stop 14
#define invalid_char 15
#define delim_num 15
#define max_char_code 15 \

#define char_num 16
#define math_char_num 17
#define mark 18
#define xray 19
#define make_box 20
#define hmove 21
#define vmove 22
#define un_hbox 23
#define un_vbox 24
#define remove_item 25 \

#define hskip 26
#define vskip 27
#define mskip 28
#define kern 29
#define mkern 30
#define leader_ship 31
#define halign 32
#define valign 33
#define no_align 34
#define vrule 35
#define hrule 36
#define insert 37
#define vadjust 38
#define ignore_spaces 39
#define after_assignment 40
#define after_group 41
#define break_penalty 42
#define start_par 43
#define ital_corr 44
#define accent 45
#define math_accent 46
#define discretionary 47
#define eq_no 48
#define left_right 49
#define math_comp 50
#define limit_switch 51
#define above 52
#define math_style 53
#define math_choice 54
#define non_script 55
#define vcenter 56
#define case_shift 57
#define message 58
#define extension 59
#define in_stream 60
#define begin_group 61
#define end_group 62
#define omit 63
#define ex_space 64
#define no_boundary 65
#define radical 66
#define end_cs_name 67
#define min_internal 68
#define char_given 68
#define math_given 69
#define last_item 70 \

#define max_non_prefixed_command 70 \

#define toks_register 71
#define assign_toks 72
#define assign_int 73
#define assign_dimen 74
#define assign_glue 75
#define assign_mu_glue 76
#define assign_font_dimen 77
#define assign_font_int 78 \

#define set_aux 79
#define set_prev_graf 80
#define set_page_dimen 81
#define set_page_int 82 \

#define set_box_dimen 83
#define set_shape 84
#define def_code 85
#define def_family 86
#define set_font 87
#define def_font 88
#define internal_register 89
#define max_internal 89
#define advance 90
#define multiply 91
#define divide 92
#define prefix 93
#define let 94
#define shorthand_def 95
#define read_to_cs 96
#define def 97
#define set_box 98
#define hyph_data 99
#define set_interaction 100
#define max_command 100 \

#define undefined_cs (max_command+1) 
#define expand_after (max_command+2) 
#define no_expand (max_command+3) 
#define input (max_command+4) 
#define if_test (max_command+5) 
#define fi_or_else (max_command+6) 
#define cs_name (max_command+7) 
#define convert (max_command+8) 
#define the (max_command+9) 
#define top_bot_mark (max_command+10) 
#define call (max_command+11) 
#define long_call (max_command+12) 
#define outer_call (max_command+13) 
#define long_outer_call (max_command+14) 
#define end_template (max_command+15) 
#define dont_expand (max_command+16) 
#define glue_ref (max_command+17) 
#define shape_ref (max_command+18) 
#define box_ref (max_command+19) 
#define data (max_command+20)  \

#define vmode 1
#define hmode (vmode+max_command+1) 
#define mmode (hmode+max_command+1)  \

#define ignore_depth -65536000 \

#define mode cur_list.mode_field
#define head cur_list.head_field
#define tail cur_list.tail_field
#define prev_graf cur_list.pg_field
#define aux cur_list.aux_field
#define prev_depth aux.sc
#define space_factor aux.hh.lh
#define clang aux.hh.rh
#define incompleat_noad aux.i
#define cur_bs cur_list.bs_field
#define cur_ls cur_list.ls_field
#define cur_lsl cur_list.lsl_field
#define needs_bs (cur_list.bs_pos!=NULL) 
#define prev_height cur_list.ht_field
#define node_pos cur_list.np_field
#define node_pos1 (nest_ptr==0?0:nest[nest_ptr-1].np_field)  \

#define tail_append(A) {link(tail) = A;tail= link(tail) ; \
} \

#define eq_level_field(A) A.hh.b1
#define eq_type_field(A) A.hh.b0
#define equiv_field(A) A.hh.rh
#define eq_level(A) eq_level_field(eqtb[A]) 
#define eq_type(A) eq_type_field(eqtb[A]) 
#define equiv(A) equiv_field(eqtb[A]) 
#define level_zero min_quarterword
#define level_one (level_zero+1)  \

#define active_base 1
#define single_base (active_base+256) 
#define null_cs (single_base+256) 
#define hash_base (null_cs+1) 
#define frozen_control_sequence (hash_base+hash_size) 
#define frozen_protection frozen_control_sequence
#define frozen_cr (frozen_control_sequence+1) 
#define frozen_end_group (frozen_control_sequence+2) 
#define frozen_right (frozen_control_sequence+3) 
#define frozen_fi (frozen_control_sequence+4) 
#define frozen_end_template (frozen_control_sequence+5) 
#define frozen_endv (frozen_control_sequence+6) 
#define frozen_relax (frozen_control_sequence+7) 
#define end_write (frozen_control_sequence+8) 
#define frozen_dont_expand (frozen_control_sequence+9)  \

#define frozen_null_font (frozen_control_sequence+10)  \

#define font_id_base (frozen_null_font-font_base)  \

#define undefined_control_sequence (frozen_null_font+257) 
#define glue_base (undefined_control_sequence+1)  \

#define line_skip_code 0
#define baseline_skip_code 1
#define par_skip_code 2
#define above_display_skip_code 3
#define below_display_skip_code 4
#define above_display_short_skip_code 5 \

#define below_display_short_skip_code 6 \

#define left_skip_code 7
#define right_skip_code 8
#define top_skip_code 9
#define split_top_skip_code 10
#define tab_skip_code 11
#define space_skip_code 12
#define xspace_skip_code 13
#define par_fill_skip_code 14
#define thin_mu_skip_code 15
#define med_mu_skip_code 16
#define thick_mu_skip_code 17
#define glue_pars 18
#define skip_base (glue_base+glue_pars) 
#define mu_skip_base (skip_base+256) 
#define local_base (mu_skip_base+256)  \

#define mu_skip(A) equiv(mu_skip_base+A) 
#define glue_par(A) equiv(glue_base+A) 
#define line_skip pointer_def[glue_kind][line_skip_no]
#define baseline_skip pointer_def[glue_kind][baseline_skip_no]
#define above_display_skip pointer_def[glue_kind][above_display_skip_no]
#define below_display_skip pointer_def[glue_kind][below_display_skip_no]
#define above_display_short_skip pointer_def[glue_kind][above_display_short_skip_no]
#define below_display_short_skip pointer_def[glue_kind][below_display_short_skip_no]
#define left_skip pointer_def[glue_kind][left_skip_no]
#define right_skip pointer_def[glue_kind][right_skip_no]
#define top_skip pointer_def[glue_kind][top_skip_no]
#define split_top_skip pointer_def[glue_kind][split_top_skip_no]
#define tab_skip pointer_def[glue_kind][tab_skip_no]
#define par_fill_skip pointer_def[glue_kind][par_fill_skip_no]
#define thin_mu_skip glue_par(thin_mu_skip_code) 
#define med_mu_skip glue_par(med_mu_skip_code) 
#define thick_mu_skip glue_par(thick_mu_skip_code)  \

#define par_shape_loc local_base
#define output_routine_loc (local_base+1) 
#define every_par_loc (local_base+2) 
#define every_math_loc (local_base+3) 
#define every_display_loc (local_base+4) 
#define every_hbox_loc (local_base+5) 
#define every_vbox_loc (local_base+6) 
#define every_job_loc (local_base+7) 
#define every_cr_loc (local_base+8) 
#define err_help_loc (local_base+9) 
#define toks_base (local_base+10) 
#define box_base (toks_base+256) 
#define cur_font_loc (box_base+256) 
#define math_font_base (cur_font_loc+1) 
#define cat_code_base (math_font_base+48)  \

#define lc_code_base (cat_code_base+256) 
#define uc_code_base (lc_code_base+256) 
#define sf_code_base (uc_code_base+256) 
#define math_code_base (sf_code_base+256) 
#define int_base (math_code_base+256)  \

#define par_shape_ptr null
#define output_routine equiv(output_routine_loc) 
#define every_par equiv(every_par_loc) 
#define every_math equiv(every_math_loc) 
#define every_display equiv(every_display_loc) 
#define every_hbox equiv(every_hbox_loc) 
#define every_vbox equiv(every_vbox_loc) 
#define every_job equiv(every_job_loc) 
#define every_cr equiv(every_cr_loc) 
#define err_help equiv(err_help_loc) 
#define toks(X) equiv(toks_base+X) 
#define box(A) (*box_ptr(A) ) 
#define cur_font equiv(cur_font_loc) 
#define fam_fnt(A) equiv(math_font_base+A) 
#define cat_code(A) equiv(cat_code_base+A) 
#define lc_code(A) equiv(lc_code_base+A) 
#define uc_code(A) equiv(uc_code_base+A) 
#define sf_code(A) equiv(sf_code_base+A) 
#define math_code(A) equiv(math_code_base+A)  \
 \

#define null_font font_base
#define var_code 070000 \

#define pretolerance_code 0
#define tolerance_code 1
#define line_penalty_code 2
#define hyphen_penalty_code 3
#define ex_hyphen_penalty_code 4
#define club_penalty_code 5
#define widow_penalty_code 6
#define display_widow_penalty_code 7
#define broken_penalty_code 8
#define bin_op_penalty_code 9
#define rel_penalty_code 10
#define pre_display_penalty_code 11 \

#define post_display_penalty_code 12 \

#define inter_line_penalty_code 13
#define double_hyphen_demerits_code 14
#define final_hyphen_demerits_code 15
#define adj_demerits_code 16
#define mag_code 17
#define delimiter_factor_code 18
#define looseness_code 19
#define time_code 20
#define day_code 21
#define month_code 22
#define year_code 23
#define show_box_breadth_code 24
#define show_box_depth_code 25
#define hbadness_code 26
#define vbadness_code 27
#define pausing_code 28
#define tracing_online_code 29
#define tracing_macros_code 30
#define tracing_stats_code 31
#define tracing_paragraphs_code 32
#define tracing_pages_code 33
#define tracing_output_code 34
#define tracing_lost_chars_code 35
#define tracing_commands_code 36
#define tracing_restores_code 37
#define uc_hyph_code 38
#define output_penalty_code 39
#define max_dead_cycles_code 40
#define hang_after_code 41
#define floating_penalty_code 42
#define global_defs_code 43
#define cur_fam_code 44
#define escape_char_code 45
#define default_hyphen_char_code 46
#define default_skew_char_code 47
#define end_line_char_code 48
#define new_line_char_code 49
#define language_code 50
#define left_hyphen_min_code 51
#define right_hyphen_min_code 52
#define holding_inserts_code 53
#define error_context_lines_code 54
#define int_pars 55
#define count_base (int_base+int_pars) 
#define del_code_base (count_base+256) 
#define dimen_base (del_code_base+256)  \

#define del_code(A) eqtb[del_code_base+A].i
#define int_par(A) eqtb[int_base+A].i
#define pretolerance integer_def[pretolerance_no]
#define tolerance integer_def[tolerance_no]
#define line_penalty integer_def[line_penalty_no]
#define hyphen_penalty integer_def[hyphen_penalty_no]
#define ex_hyphen_penalty integer_def[ex_hyphen_penalty_no]
#define club_penalty integer_def[club_penalty_no]
#define widow_penalty integer_def[widow_penalty_no]
#define display_widow_penalty integer_def[display_widow_penalty_no]
#define broken_penalty integer_def[broken_penalty_no]
#define pre_display_penalty integer_def[pre_display_penalty_no]
#define post_display_penalty integer_def[post_display_penalty_no]
#define inter_line_penalty integer_def[inter_line_penalty_no]
#define double_hyphen_demerits integer_def[double_hyphen_demerits_no]
#define final_hyphen_demerits integer_def[final_hyphen_demerits_no]
#define adj_demerits integer_def[adj_demerits_no]
#define looseness integer_def[looseness_no]
#define time integer_def[time_no]
#define day integer_def[day_no]
#define month integer_def[month_no]
#define year integer_def[year_no]
#define show_box_breadth int_par(show_box_breadth_code) 
#define show_box_depth int_par(show_box_depth_code) 
#define hbadness int_par(hbadness_code) 
#define vbadness int_par(vbadness_code) 
#define pausing int_par(pausing_code) 
#define tracing_online int_par(tracing_online_code) 
#define tracing_macros int_par(tracing_macros_code) 
#define tracing_stats int_par(tracing_stats_code) 
#define tracing_paragraphs (debugflags&DBGTEX) 
#define tracing_pages (debugflags&DBGPAGE) 
#define tracing_output int_par(tracing_output_code) 
#define tracing_lost_chars int_par(tracing_lost_chars_code) 
#define tracing_commands int_par(tracing_commands_code) 
#define tracing_restores int_par(tracing_restores_code) 
#define uc_hyph int_par(uc_hyph_code) 
#define output_penalty int_par(output_penalty_code) 
#define max_dead_cycles int_par(max_dead_cycles_code) 
#define hang_after integer_def[hang_after_no]
#define floating_penalty int_par(floating_penalty_code) 
#define global_defs int_par(global_defs_code) 
#define cur_fam int_par(cur_fam_code) 
#define escape_char int_par(escape_char_code) 
#define default_hyphen_char int_par(default_hyphen_char_code) 
#define default_skew_char int_par(default_skew_char_code) 
#define end_line_char int_par(end_line_char_code) 
#define new_line_char int_par(new_line_char_code) 
#define language int_par(language_code) 
#define left_hyphen_min int_par(left_hyphen_min_code) 
#define right_hyphen_min int_par(right_hyphen_min_code) 
#define error_context_lines int_par(error_context_lines_code)  \

#define par_indent_code 0
#define math_surround_code 1
#define line_skip_limit_code 2
#define hsize_code 3
#define vsize_code 4
#define max_depth_code 5
#define split_max_depth_code 6
#define box_max_depth_code 7
#define hfuzz_code 8
#define vfuzz_code 9
#define delimiter_shortfall_code 10
#define null_delimiter_space_code 11
#define script_space_code 12
#define pre_display_size_code 13
#define display_width_code 14
#define display_indent_code 15
#define overfull_rule_code 16
#define hang_indent_code 17
#define h_offset_code 18
#define v_offset_code 19
#define emergency_stretch_code 20
#define dimen_pars 21
#define scaled_base (dimen_base+dimen_pars)  \

#define eqtb_size (scaled_base+255)  \

#define dimen_par(A) eqtb[dimen_base+A].sc
#define par_indent dimen_par(par_indent_code) 
#define math_surround dimen_par(math_surround_code) 
#define line_skip_limit dimen_def[line_skip_limit_no]
#define hsize dimen_par(hsize_code) 
#define vsize dimen_par(vsize_code) 
#define max_depth dimen_def[max_depth_no]
#define split_max_depth dimen_par(split_max_depth_code) 
#define box_max_depth dimen_par(box_max_depth_code) 
#define hfuzz dimen_par(hfuzz_code) 
#define vfuzz dimen_par(vfuzz_code) 
#define delimiter_shortfall dimen_par(delimiter_shortfall_code) 
#define null_delimiter_space dimen_par(null_delimiter_space_code) 
#define script_space dimen_par(script_space_code) 
#define pre_display_size cur_list.ds_field
#define display_width cur_list.dw_field
#define display_indent cur_list.di_field
#define overfull_rule dimen_par(overfull_rule_code) 
#define hang_indent dimen_def[hang_indent_no]
#define h_offset dimen_par(h_offset_code) 
#define v_offset dimen_par(v_offset_code) 
#define emergency_stretch dimen_def[emergency_stretch_no] \

#define next(A) hash[A].lh
#define text(A) hash[A].rh
#define hash_is_full (hash_used==hash_base) 
#define font_id_text(A) text(font_id_base+A)  \

#define save_type(A) save_stack[A].hh.b0
#define save_level(A) save_stack[A].hh.b1 \

#define save_index(A) save_stack[A].hh.rh \

#define restore_old_value 0
#define restore_zero 1
#define insert_token 2
#define level_boundary 3 \

#define bottom_level 0
#define simple_group 1
#define hbox_group 2
#define adjusted_hbox_group 3
#define vbox_group 4
#define vtop_group 5
#define align_group 6
#define no_align_group 7
#define output_group 8
#define math_group 9
#define disc_group 10
#define insert_group 11
#define vcenter_group 12
#define math_choice_group 13
#define semi_simple_group 14
#define math_shift_group 15
#define math_left_group 16
#define max_group_code 16 \

#define check_full_save_stack if(save_ptr> max_save_stack)  \
{max_save_stack= save_ptr; \
if(max_save_stack> save_size-6) overflow("save size",save_size) ; \
 \
} \

#define saved(A) save_stack[save_ptr+A].i \

#define cs_token_flag 07777 \

#define left_brace_token 00400
#define left_brace_limit 01000
#define right_brace_token 01000
#define right_brace_limit 01400
#define math_shift_token 01400
#define tab_token 02000
#define out_param_token 02400
#define space_token 05040
#define letter_token 05400
#define other_token 06000
#define match_token 06400
#define end_match_token 07000 \

#define chr_cmd(A) {print(A) ;print_ASCII(chr_code) ; \
} \

#define state cur_input.state_field
#define index cur_input.index_field
#define start cur_input.start_field
#define limit cur_input.limit_field
#define name cur_input.name_field \

#define mid_line 1
#define skip_blanks (2+max_char_code) 
#define new_line (3+max_char_code+max_char_code)  \

#define terminal_input (name==0) 
#define cur_file input_file[index] \

#define skipping 1
#define defining 2
#define matching 3
#define aligning 4
#define absorbing 5 \

#define token_list 0
#define token_type index
#define param_start limit
#define parameter 0
#define u_template 1
#define v_template 2
#define backed_up 3
#define inserted 4
#define macro 5
#define output_text 6
#define every_par_text 7
#define every_math_text 8
#define every_display_text 9
#define every_hbox_text 10
#define every_vbox_text 11
#define every_job_text 12
#define every_cr_text 13
#define mark_text 14
#define write_text 15 \

#define begin_pseudoprint  \
{l= tally;tally= 0;selector= pseudo; \
trick_count= 1000000; \
}
#define set_trick_count  \
{first_count= tally; \
trick_count= tally+1+error_line-half_error_line; \
if(trick_count<error_line) trick_count= error_line; \
} \

#define push_input  \
{if(input_ptr> max_in_stack)  \
{max_in_stack= input_ptr; \
if(input_ptr==stack_size) overflow("input stack size",stack_size) ; \
 \
} \
input_stack[input_ptr]= cur_input; \
incr(input_ptr) ; \
} \

#define pop_input  \
{decr(input_ptr) ;cur_input= input_stack[input_ptr]; \
} \

#define back_list(A) begin_token_list(A,backed_up) 
#define ins_list(A) begin_token_list(A,inserted)  \

#define any_state_plus(A) case mid_line+A: \
case skip_blanks+A:case new_line+A \

#define add_delims_to(A) A+math_shift:A+tab_mark:A+mac_param: \
A+sub_mark:A+letter:A+other_char \

#define is_hex(A) (((A>='0') &&(A<='9') ) ||((A>='a') &&(A<='f') ) ) 
#define hex_to_cur_chr  \
if(c<='9') cur_chr= c-'0';else cur_chr= c-'a'+10; \
if(cc<='9') cur_chr= 16*cur_chr+cc-'0'; \
else cur_chr= 16*cur_chr+cc-'a'+10 \

#define no_expand_flag 257 \

#define end_line_char_inactive (end_line_char<0) ||(end_line_char> 255)  \

#define store_new_token(A) {q= get_avail() ;link(p) = q;info(q) = A; \
p= q; \
}
#define fast_store_new_token(A) {fast_get_avail(q) ;link(p) = q;info(q) = A; \
p= q; \
} \

#define top_mark_code 0
#define first_mark_code 1
#define bot_mark_code 2
#define split_first_mark_code 3
#define split_bot_mark_code 4
#define top_mark cur_mark[top_mark_code]
#define first_mark cur_mark[first_mark_code]
#define bot_mark cur_mark[bot_mark_code]
#define split_first_mark cur_mark[split_first_mark_code]
#define split_bot_mark cur_mark[split_bot_mark_code] \

#define int_val 0
#define dimen_val 1
#define glue_val 2
#define mu_val 3
#define ident_val 4
#define tok_val 5 \

#define scanned_result(A,B) {cur_val= A;cur_val_level= B;} \

#define input_line_no_code (glue_val+1) 
#define badness_code (glue_val+2)  \

#define max_dimen 07777777777 \

#define octal_token (other_token+'\'') 
#define hex_token (other_token+'"') 
#define alpha_token (other_token+'`') 
#define point_token (other_token+'.') 
#define continental_point_token (other_token+',')  \

#define infinity 017777777777
#define zero_token (other_token+'0') 
#define A_token (letter_token+'A') 
#define other_A_token (other_token+'A')  \

#define scan_normal_dimen scan_dimen(false,false,false)  \

#define set_conversion(A,B) {num= A;denom= B;} \

#define default_rule 26214 \

#define number_code 0
#define roman_numeral_code 1
#define string_code 2
#define meaning_code 3
#define font_name_code 4
#define job_name_code 5 \

#define closed 2
#define just_open 1 \

#define if_char_code 0
#define if_cat_code 1
#define if_int_code 2
#define if_dim_code 3
#define if_odd_code 4
#define if_vmode_code 5
#define if_hmode_code 6
#define if_mmode_code 7
#define if_inner_code 8
#define if_void_code 9
#define if_hbox_code 10
#define if_vbox_code 11
#define ifx_code 12
#define if_eof_code 13
#define if_true_code 14
#define if_false_code 15
#define if_case_code 16 \

#define if_node_size 2
#define if_line_field(A) mem[A+1].i
#define if_code 1
#define fi_code 2
#define else_code 3
#define or_code 4 \

#define get_x_token_or_active_char  \
{get_x_token() ; \
if(cur_cmd==relax) if(cur_chr==no_expand_flag)  \
{cur_cmd= active_char; \
cur_chr= cur_tok-cs_token_flag-active_base; \
} \
} \

#define TEX_area "TeXinputs/" \

#define TEX_font_area "TeXfonts/" \
 \

#define MAX_CUR_FILE_NAME 1024 \

#define append_to_name(A) {c= A;incr(k) ; \
if(k<=file_name_size) name_of_file[k]= xchr[c]; \
} \

#define format_default_length 20
#define format_area_length 11
#define format_ext_length 4
#define format_extension ".fmt" \

#define pack_cur_name pack_file_name(cur_name,cur_area,cur_ext)  \

#define ensure_dvi_open if(output_file_name==0)  \
{if(job_name==0) open_log_file() ; \
pack_job_name(".dvi") ; \
while(!b_open_out(&dvi_file) )  \
prompt_file_name("file name for output",".dvi") ; \
output_file_name= b_make_name_string(&dvi_file) ; \
} \

#define no_tag 0
#define lig_tag 1
#define list_tag 2
#define ext_tag 3 \

#define stop_flag qi(128) 
#define kern_flag qi(128) 
#define skip_byte(A) A.b0
#define next_char(A) A.b1
#define op_byte(A) A.b2
#define rem_byte(A) A.b3 \

#define ext_top(A) A.b0
#define ext_mid(A) A.b1
#define ext_bot(A) A.b2
#define ext_rep(A) A.b3 \

#define slant_code 1
#define space_code 2
#define space_stretch_code 3
#define space_shrink_code 4
#define x_height_code 5
#define quad_code 6
#define extra_space_code 7 \

#define non_char qi(256) 
#define non_address 0 \

#define char_info(A,B) font_info[char_base[A]+B].qqqq
#define char_width(A,B) font_info[width_base[A]+B.b0].sc
#define char_exists(A) (A.b0> min_quarterword) 
#define char_italic(A,B) font_info[italic_base[A]+(qo(B.b2) ) /4].sc
#define height_depth(A) qo(A.b1) 
#define char_height(A,B) font_info[height_base[A]+(B) /16].sc
#define char_depth(A,B) font_info[depth_base[A]+(B) %16].sc
#define char_tag(A) ((qo(A.b2) ) %4)  \

#define char_kern(A,B) font_info[kern_base[A]+256*op_byte(B) +rem_byte(B) ].sc
#define kern_base_offset 256*(128+min_quarterword) 
#define lig_kern_start(A,B) lig_kern_base[A]+B.b3
#define lig_kern_restart(A,B) lig_kern_base[A]+256*op_byte(B) +rem_byte(B) +32768-kern_base_offset \

#define param_end(A) param_base[A]].sc
#define param(A) font_info[A+param_end
#define slant param(slant_code) 
#define space param(space_code) 
#define space_stretch param(space_stretch_code) 
#define space_shrink param(space_shrink_code) 
#define x_height param(x_height_code) 
#define quad param(quad_code) 
#define extra_space param(extra_space_code)  \

#define abort goto bad_tfm \

#define start_font_error_message print_err("Font ") ;sprint_cs(u) ; \
print_char('=') ;print_file_name(nom,aire,"") ; \
if(s>=0)  \
{print(" at ") ;print_scaled(s) ;print("pt") ; \
} \
else if(s!=-1000)  \
{print(" scaled ") ;print_int(-s) ; \
} \

#define fget (hpos++) 
#define fbyte (*hpos) 
#define read_sixteen(A) {A= fbyte; \
if(A> 127) abort; \
fget;A= A*0400+fbyte; \
}
#define store_four_quarters(A) {fget;a= fbyte;qw.b0= qi(a) ; \
fget;b= fbyte;qw.b1= qi(b) ; \
fget;c= fbyte;qw.b2= qi(c) ; \
fget;d= fbyte;qw.b3= qi(d) ; \
A= qw; \
} \

#define check_byte_range(A) {if((A<bc) ||(A> ec) ) abort;}
#define current_character_being_worked_on k+bc-fmem_ptr \

#define store_scaled(A) {fget;a= fbyte;fget;b= fbyte; \
fget;c= fbyte;fget;d= fbyte; \
sw= (((((d*z) /0400) +(c*z) ) /0400) +(b*z) ) /beta; \
if(a==0) A= sw;else if(a==255) A= sw-alpha;else abort; \
} \

#define check_existence(A)  \
{check_byte_range(A) ; \
qw= char_info(f,A) ; \
if(!char_exists(qw) ) abort; \
} \

#define adjust(A) A[f]= qo(A[f])  \
 \

#define set_char_0 0
#define set1 128
#define set_rule 132
#define put_rule 137
#define nop 138
#define bop 139
#define eop 140
#define push 141
#define pop 142
#define right1 143
#define w0 147
#define w1 148
#define x0 152
#define x1 153
#define down1 157
#define y0 161
#define y1 162
#define z0 166
#define z1 167
#define fnt_num_0 171
#define fnt1 235
#define xxx1 239
#define xxx4 242
#define fnt_def1 243
#define pre 247
#define post 248
#define post_post 249 \

#define id_byte 2 \

#define dvi_out(A) {dvi_buf[dvi_ptr]= A;incr(dvi_ptr) ; \
if(dvi_ptr==dvi_limit) dvi_swap() ; \
} \

#define movement_node_size 3
#define location(A) mem[A+2].i \

#define y_here 1
#define z_here 2
#define yz_OK 3
#define y_OK 4
#define z_OK 5
#define d_fixed 6 \

#define none_seen 0
#define y_seen 6
#define z_seen 12 \

#define synch_h if(cur_h!=dvi_h)  \
{movement(cur_h-dvi_h,right1) ;dvi_h= cur_h; \
}
#define synch_v if(cur_v!=dvi_v)  \
{movement(cur_v-dvi_v,down1) ;dvi_v= cur_v; \
} \

#define billion float_constant(1000000000) 
#define vet_glue(A) glue_temp= A; \
if(glue_temp> billion)  \
glue_temp= billion; \
else if(glue_temp<-billion)  \
glue_temp= -billion \

#define exactly 0
#define additional 1
#define natural 0,additional \

#define vpack(A,B) vpackage(A,B,max_dimen)  \

#define noad_size 4
#define nucleus(A) A+1
#define supscr(A) A+2
#define subscr(A) A+3
#define math_type(A) link(A) 
#define fam font
#define math_char 1
#define sub_box 2
#define sub_mlist 3
#define math_text_char 4 \

#define ord_noad (unset_node+3) 
#define op_noad (ord_noad+1) 
#define bin_noad (ord_noad+2) 
#define rel_noad (ord_noad+3) 
#define open_noad (ord_noad+4) 
#define close_noad (ord_noad+5) 
#define punct_noad (ord_noad+6) 
#define inner_noad (ord_noad+7) 
#define limits 1
#define no_limits 2 \

#define left_delimiter(A) A+4
#define right_delimiter(A) A+5
#define radical_noad (inner_noad+1) 
#define radical_noad_size 5
#define fraction_noad (radical_noad+1) 
#define fraction_noad_size 6
#define small_fam(A) mem[A].qqqq.b0
#define small_char(A) mem[A].qqqq.b1
#define large_fam(A) mem[A].qqqq.b2
#define large_char(A) mem[A].qqqq.b3
#define thickness(A) width(A) 
#define default_code 010000000000
#define numerator(A) supscr(A) 
#define denominator(A) subscr(A)  \

#define under_noad (fraction_noad+1) 
#define over_noad (under_noad+1) 
#define accent_noad (over_noad+1) 
#define accent_noad_size 5
#define accent_chr(A) A+4
#define vcenter_noad (accent_noad+1) 
#define left_noad (vcenter_noad+1) 
#define right_noad (left_noad+1) 
#define delimiter(A) nucleus(A) 
#define scripts_allowed(A) (type(A) >=ord_noad) &&(type(A) <left_noad)  \

#define style_node (unset_node+1) 
#define style_node_size 3
#define display_style 0
#define text_style 2
#define script_style 4
#define script_script_style 6
#define cramped 1 \

#define choice_node (unset_node+2) 
#define display_mlist(A) info(A+1) 
#define text_mlist(A) link(A+1) 
#define script_mlist(A) info(A+2) 
#define script_script_mlist(A) link(A+2)  \

#define text_size 0
#define script_size 16
#define script_script_size 32 \

#define mathsy_end(A) fam_fnt(2+A) ]].sc
#define mathsy(A) font_info[A+param_base[mathsy_end
#define math_x_height mathsy(5) 
#define math_quad dimen_def[math_quad_no]
#define num1 mathsy(8) 
#define num2 mathsy(9) 
#define num3 mathsy(10) 
#define denom1 mathsy(11) 
#define denom2 mathsy(12) 
#define sup1 mathsy(13) 
#define sup2 mathsy(14) 
#define sup3 mathsy(15) 
#define sub1 mathsy(16) 
#define sub2 mathsy(17) 
#define sup_drop mathsy(18) 
#define sub_drop mathsy(19) 
#define delim1 mathsy(20)  \

#define delim2 mathsy(21) 
#define axis_height mathsy(22) 
#define total_mathsy_params 22 \

#define mathex(A) font_info[A+param_base[fam_fnt(3+cur_size) ]].sc
#define default_rule_thickness mathex(8) 
#define big_op_spacing1 mathex(9) 
#define big_op_spacing2 mathex(10) 
#define big_op_spacing3 mathex(11) 
#define big_op_spacing4 mathex(12) 
#define big_op_spacing5 mathex(13) 
#define total_mathex_params 13 \

#define cramped_style(A) 2*(A/2) +cramped
#define sub_style(A) 2*(A/4) +script_style+cramped
#define sup_style(A) 2*(A/4) +script_style+(A%2) 
#define num_style(A) A+2-2*(A/6) 
#define denom_style(A) 2*(A/2) +cramped+2-2*(A/6)  \

#define mu_mult(A) nx_plus_y(n,A,xn_over_d(A,f,0200000) )  \

#define new_hlist(A) mem[nucleus(A) ].i \

#define choose_mlist(A) {p= A(q) ;A(q) = null;} \

#define math_spacing  \
 \
"0234000122*4000133**3**344*0400400*000000234000111*1111112341011" \
 \

#define u_part(A) mem[A+height_offset].i
#define v_part(A) mem[A+depth_offset].i
#define extra_info(A) info(A+list_offset)  \

#define preamble link(align_head) 
#define align_stack_node_size 5 \

#define span_code 256
#define cr_code 257
#define cr_cr_code (cr_code+1) 
#define end_template_token cs_token_flag+frozen_end_template \

#define span_node_size 2 \

#define tight_fit 3 \

#define loose_fit 1 \

#define very_loose_fit 0 \

#define decent_fit 2 \

#define active_node_size 3
#define fitness(A) subtype(A) 
#define break_node(A) rlink(A) 
#define line_number(A) llink(A) 
#define total_demerits(A) mem[A+2].i
#define unhyphenated 0
#define hyphenated 1
#define last_active active \

#define passive_node_size 2
#define cur_break(A) rlink(A) 
#define prev_break(A) llink(A) 
#define serial(A) info(A)  \

#define delta_node_size 7
#define delta_node 2 \

#define do_all_six(A) A(1) ;A(2) ;A(3) ;A(4) ;A(5) ;A(6)  \

#define check_shrinkage(A) if((shrink_order(A) !=normal) &&(shrink(A) !=0) )  \
{A= finite_shrink(A) ; \
} \

#define copy_to_cur_active(A) cur_active_width[A]= active_width[A]
#define update_width(A)  \
cur_active_width[A]= cur_active_width[A]+mem[r+A].sc \

#define awful_bad 07777777777 \

#define set_break_width_to_background(A) break_width[A]= background[A] \

#define convert_to_break_width(A)  \
mem[prev_r+A].sc= mem[prev_r+A].sc \
-cur_active_width[A]+break_width[A]
#define store_break_width(A) active_width[A]= break_width[A]
#define new_delta_to_break_width(A)  \
mem[q+A].sc= break_width[A]-cur_active_width[A] \

#define new_delta_from_break_width(A) mem[q+A].sc=  \
cur_active_width[A]-break_width[A] \

#define combine_two_deltas(A) mem[prev_r+A].sc= mem[prev_r+A].sc+mem[r+A].sc
#define downdate_width(A) cur_active_width[A]= cur_active_width[A]- \
mem[prev_r+A].sc \

#define update_active(A) active_width[A]= active_width[A]+mem[r+A].sc \

#define store_background(A) active_width[A]= background[A] \

#define act_width active_width[1]
#define kern_break {if(!is_char_node(link(cur_p) ) &&auto_breaking)  \
if(type(link(cur_p) ) ==glue_node) try_break(0,unhyphenated) ; \
act_width= act_width+width(cur_p) ; \
} \

#define next_break prev_break \

#define append_charnode_to_t(A) {link(t) = get_avail() ;t= link(t) ; \
font(t) = hf;character(t) = A; \
}
#define set_cur_r {if(j<n) cur_r= qi(hu[j+1]) ;else cur_r= bchar; \
if(odd(hyf[j]) ) cur_rh= hchar;else cur_rh= non_char; \
} \

#define wrap_lig(A) if(ligature_present)  \
{p= new_ligature(hf,cur_l,link(cur_q) ) ; \
if(lft_hit)  \
{subtype(p) = 2;lft_hit= false; \
} \
if(A) if(lig_stack==null)  \
{incr(subtype(p) ) ;rt_hit= false; \
} \
link(cur_q) = p;t= p;ligature_present= false; \
}
#define pop_lig_stack {if(lig_ptr(lig_stack) > null)  \
{link(t) = lig_ptr(lig_stack) ; \
t= link(t) ;incr(j) ; \
} \
p= lig_stack;lig_stack= link(p) ;free_node(p,small_node_size) ; \
if(lig_stack==null) set_cur_r else cur_r= character(lig_stack) ; \
} \

#define advance_major_tail {major_tail= link(major_tail) ;incr(r_count) ; \
} \

#define trie_link(A) trie[A].rh
#define trie_char(A) trie[A].b1
#define trie_op(A) trie[A].b0 \

#define set_cur_lang if(language<=0) cur_lang= 0; \
else if(language> 255) cur_lang= 0; \
else cur_lang= language \

#define trie_root trie_l[0] \

#define trie_ref trie_hash
#define trie_back(A) trie[A].lh \

#define active_height active_width
#define cur_height active_height[1]
#define set_height_zero(A) active_height[A]= 0 \

#define deplorable 100000 \

#define inserts_only 1 \

#define box_there 2 \

#define page_ins_node_size 4
#define inserting 0
#define split_up 1
#define broken_ptr(A) link(A+1)  \

#define broken_ins(A) info(A+1) 
#define last_ins_ptr(A) link(A+2) 
#define best_ins_ptr(A) info(A+2)  \

#define page_goal page_so_far[0]
#define page_total page_so_far[1]
#define page_shrink page_so_far[6]
#define page_depth page_so_far[7] \

#define print_plus(A,B) if(page_so_far[A]!=0)  \
{print(" plus ") ;print_scaled(page_so_far[A]) ;print(B) ;} \

#define set_page_so_far_zero(A) page_so_far[A]= 0 \

#define contrib_tail nest[0].tail_field \

#define adjust_space_factor  \
main_s= sf_code(cur_chr) ; \
if(main_s==1000) space_factor= 1000; \
else if(main_s<1000)  \
{if(main_s> 0) space_factor= main_s; \
} \
else if(space_factor<1000) space_factor= 1000; \
else space_factor= main_s \

#define pack_lig(X)  \
{main_p= new_ligature(main_f,cur_l,link(cur_q) ) ; \
if(lft_hit)  \
{subtype(main_p) = 2;lft_hit= false; \
} \
if(X) if(lig_stack==null)  \
{incr(subtype(main_p) ) ;rt_hit= false; \
} \
link(cur_q) = main_p;tail= main_p;ligature_present= false; \
} \

#define wrapup(A) if(cur_l<non_char)  \
{if(link(cur_q) > null)  \
if(character(tail) ==qi(hyphen_char[main_f]) ) ins_disc= true; \
if(ligature_present) pack_lig(A) ; \
if(ins_disc)  \
{ins_disc= false; \
if(mode> 0) tail_append(new_disc() ) ; \
} \
} \

#define any_mode(A) case vmode+A:case hmode+A: \
case mmode+A \

#define non_math(A) case vmode+A:case hmode+A \

#define fil_code 0
#define fill_code 1
#define ss_code 2
#define fil_neg_code 3
#define skip_code 4
#define mskip_code 5 \

#define box_flag 010000000000
#define ship_out_flag box_flag+512
#define leader_flag box_flag+513
#define box_code 0
#define copy_code 1
#define last_box_code 2
#define vsplit_code 3
#define vtop_code 4 \

#define fam_in_range ((cur_fam>=0) &&(cur_fam<16) )  \

#define above_code 0
#define over_code 1
#define atop_code 2
#define delimited_code 3 \

#define global (a>=4) 
#define define(A,B,C) if(global) geq_define(A,B,C) ;else eq_define(A,B,C) 
#define word_define(A,B) if(global) geq_word_define(A,B) ;else eq_word_define(A,B)  \

#define char_def_code 0
#define math_char_def_code 1
#define count_def_code 2
#define dimen_def_code 3
#define skip_def_code 4
#define mu_skip_def_code 5
#define toks_def_code 6 \

#define show_code 0
#define show_box_code 1
#define show_the_code 2
#define show_lists_code 3 \

#define too_small(X) {wake_up_terminal; \
wterm_ln("---! Must increase the %s",X) ; \
 \
goto bad_fmt; \
} \

#define dump_wd(A) {fmt_file.d= A;put(fmt_file) ;}
#define dump_int(A) {fmt_file.d.i= A;put(fmt_file) ;}
#define dump_hh(A) {fmt_file.d.hh= A;put(fmt_file) ;}
#define dump_qqqq(A) {fmt_file.d.qqqq= A;put(fmt_file) ;} \

#define undump_wd(A) {get(fmt_file) ;A= fmt_file.d;}
#define undump_int(A) {get(fmt_file) ;A= fmt_file.d.i;}
#define undump_hh(A) {get(fmt_file) ;A= fmt_file.d.hh;}
#define undump_qqqq(A) {get(fmt_file) ;A= fmt_file.d.qqqq;}
#define undump(A,B,C) {undump_int(x) ;if((x<A) ||(x> B) ) goto bad_fmt;else C= x;}
#define undump_size(A,B,C,D) {undump_int(x) ; \
if(x<A) goto bad_fmt;if(x> B) too_small(C) else D= x;} \

#define dump_four_ASCII  \
w.b0= qi(so(str_pool[k]) ) ;w.b1= qi(so(str_pool[k+1]) ) ; \
w.b2= qi(so(str_pool[k+2]) ) ;w.b3= qi(so(str_pool[k+3]) ) ; \
dump_qqqq(w)  \

#define undump_four_ASCII  \
undump_qqqq(w) ; \
str_pool[k]= si(qo(w.b0) ) ;str_pool[k+1]= si(qo(w.b1) ) ; \
str_pool[k+2]= si(qo(w.b2) ) ;str_pool[k+3]= si(qo(w.b3) )  \

#define write_node_size 2
#define open_node_size 3
#define open_node 0
#define write_node 1
#define close_node 2
#define special_node 3
#define language_node 4
#define what_lang(A) link(A+1) 
#define what_lhm(A) type(A+1) 
#define what_rhm(A) subtype(A+1) 
#define write_tokens(A) link(A+1) 
#define write_stream(A) info(A+1) 
#define open_name(A) link(A+1) 
#define open_area(A) info(A+2) 
#define open_ext(A) link(A+2)  \

#define hitex_ext language_node+1
#define param_node hitex_ext
#define param_node_size 3
#define param_type(A) type(A+1) 
#define int_type 0
#define dimen_type 1
#define glue_type 2
#define param_no(A) subtype(A+1) 
#define param_value(A) mem[A+2] \

#define par_node hitex_ext+1
#define par_node_size 5
#define par_penalty(A) mem[A+1].i
#define par_extent(A) link(A+3) 
#define par_params(A) info(A+4) 
#define par_list(A) link(A+4)  \

#define disp_node hitex_ext+2
#define disp_node_size 3
#define display_left(A) type(A+1) 
#define display_no_bs(A) subtype(A+1) 
#define display_params(A) link(A+1) 
#define display_formula(A) link(A+2) 
#define display_eqno(A) info(A+2)  \

#define baseline_node hitex_ext+3
#define baseline_node_size small_node_size
#define baseline_node_no(A) mem[A+1].i \

#define image_node hitex_ext+4
#define image_node_size 6
#define image_width(A) mem[A+1].sc
#define image_height(A) mem[A+2].sc
#define image_no(A) link(A+3) 
#define image_name(A) info(A+3) 
#define image_area(A) info(A+4) 
#define image_ext(A) link(A+4) 
#define image_alt(A) link(A+5)  \

#define hpack_node hitex_ext+5
#define vpack_node hitex_ext+6
#define pack_node_size box_node_size
#define pack_m(A) type(A+list_offset) 
#define pack_limit(A) mem[(A) +1+list_offset].sc
#define pack_extent(A) link(A+2+list_offset)  \

#define hset_node hitex_ext+7
#define vset_node hitex_ext+8
#define set_node_size box_node_size
#define set_stretch_order glue_sign
#define set_shrink_order glue_order
#define set_stretch(A) mem[(A) +1+list_offset].sc
#define set_extent(A) pack_extent(A) 
#define set_shrink(A) mem[(A) +3+list_offset].sc \

#define align_node hitex_ext+9
#define align_node_size 4
#define align_extent(A) link(A+2) 
#define align_m(A) type(A+2) 
#define align_v(A) subtype(A+2) 
#define align_preamble(A) info(A+3) 
#define align_list(A) link(A+3)  \

#define setpage_node hitex_ext+10
#define setpage_node_size 6
#define setpage_name(A) link(A+1) 
#define setpage_number(A) type(A+1) 
#define setpage_id(A) subtype(A+1) 
#define setpage_priority(A) info(A+2) 
#define setpage_topskip(A) link(A+2) 
#define setpage_depth(A) mem[A+3].sc
#define setpage_height(A) info(A+4) 
#define setpage_width(A) link(A+4) 
#define setpage_list(A) info(A+5) 
#define setpage_streams(A) link(A+5)  \

#define setstream_node hitex_ext+11
#define setstream_node_size 6
#define setstream_number(A) type(A+1) 
#define setstream_insertion(A) subtype(A+1) 
#define setstream_mag(A) link(A+1) 
#define setstream_preferred(A) type(A+2) 
#define setstream_next(A) subtype(A+2) 
#define setstream_ratio(A) link(A+2) 
#define setstream_max(A) info(A+3) 
#define setstream_width(A) link(A+3) 
#define setstream_topskip(A) info(A+4) 
#define setstream_height(A) link(A+4) 
#define setstream_before(A) info(A+5) 
#define setstream_after(A) link(A+5)  \

#define stream_node hitex_ext+12
#define stream_node_size 2
#define stream_number(A) type(A+1) 
#define stream_insertion(A) subtype(A+1)  \

#define stream_after_node hitex_ext+13
#define stream_before_node hitex_ext+14 \

#define xdimen_node hitex_ext+15
#define xdimen_node_size 4
#define xdimen_ref_count(A) link(A) 
#define xdimen_width(A) mem[A+1].sc
#define xdimen_hfactor(A) mem[A+2].sc
#define xdimen_vfactor(A) mem[A+3].sc \

#define ignore_node hitex_ext+16
#define ignore_node_size small_node_size
#define ignore_info(A) type(A+1) 
#define ignore_list(A) link(A+1)  \

#define label_node hitex_ext+17
#define label_node_size 2
#define label_has_name(A) type(A+1) 
#define label_where(A) subtype(A+1) 
#define label_ptr(A) link(A+1) 
#define label_ref(A) link(A+1)  \

#define start_link_node hitex_ext+18
#define end_link_node hitex_ext+19
#define link_node_size 2 \

#define outline_node hitex_ext+20
#define outline_node_size 4
#define outline_ptr(A) link(A+2) 
#define outline_depth(A) mem[A+3].i \
 \

#define immediate_code 4
#define set_language_code 5 \

#define adv_past(A) {} \

#define end_write_token cs_token_flag+end_write \


#line 25710 "htex.w"

enum{/*11:*/
#line 374 "htex.w"

mem_max= 65534,


mem_min= 0,


buf_size= 2048,


error_line= 72,
half_error_line= 42,

max_print_line= 79,
stack_size= 200,
max_in_open= 6,

font_max= 255,

font_mem_size= 65535,
param_size= 60,
nest_size= 400,
max_strings= 30000,
string_vacancies= 75000,


pool_size= 400000,



save_size= 600,

trie_size= 65534,

trie_op_size= 65534,
dvi_buf_size= 8,
file_name_size= 1024,
empty_string= 256

/*:11*/
#line 25711 "htex.w"
};
/*18:*/
#line 514 "htex.w"

typedef uint8_t ASCII_code;

/*:18*//*25:*/
#line 750 "htex.w"

typedef uint8_t eight_bits;
typedef struct{FILE*f;text_char d;}alpha_file;
typedef struct{FILE*f;eight_bits d;}byte_file;

/*:25*//*38:*/
#line 1111 "htex.w"

typedef int32_t pool_pointer;
typedef int16_t str_number;
typedef uint8_t packed_ASCII_code;

/*:38*//*104:*/
#line 2206 "htex.w"

typedef int scaled;
typedef int32_t nonnegative_integer;
typedef int8_t small_number;

/*:104*//*113:*/
#line 2417 "htex.w"

#if __SIZEOF_FLOAT__==4
typedef float float32_t;
#else
#error  float type must have size 4
#endif
typedef float glue_ratio;

/*:113*//*117:*/
#line 2526 "htex.w"

typedef uint8_t quarterword;
typedef uint16_t halfword;
typedef int8_t two_choices;
typedef int8_t four_choices;
typedef struct{
halfword rh;
union{
halfword lh;
struct{quarterword b0;quarterword b1;};
};}two_halves;
typedef struct{
quarterword b0;
quarterword b1;
quarterword b2;
quarterword b3;
}four_quarters;
typedef struct{
union{
int i;
glue_ratio gr;
two_halves hh;
four_quarters qqqq;
};}memory_word;
typedef struct{FILE*f;memory_word d;}word_file;

/*:117*//*155:*/
#line 3257 "htex.w"

typedef int8_t glue_ord;

/*:155*//*221:*/
#line 4420 "htex.w"

typedef struct{int16_t mode_field;
pointer head_field,tail_field;
int pg_field;
pointer bs_field,ls_field;
scaled lsl_field;
uint8_t*bs_pos;
scaled hs_field;
scaled ds_field,dw_field,di_field;
scaled ht_field;
uint32_t np_field;
memory_word aux_field;
}list_state_record;

/*:221*//*279:*/
#line 6009 "htex.w"

typedef int8_t group_code;

/*:279*//*310:*/
#line 6588 "htex.w"

typedef struct{
quarterword state_field,index_field;
halfword start_field,loc_field,limit_field,name_field;
}in_state_record;

/*:310*//*558:*/
#line 10870 "htex.w"

typedef uint8_t internal_font_number;
typedef uint16_t font_index;

/*:558*//*605:*/
#line 12062 "htex.w"

typedef int8_t dvi_index;

/*:605*//*944:*/
#line 18331 "htex.w"

typedef uint16_t trie_pointer;

/*:944*//*949:*/
#line 18400 "htex.w"

typedef int16_t hyph_pointer;

/*:949*/
#line 25712 "htex.w"

extern void list_init(void);
extern void hpack_page(void);
extern void happend_insertion(pointer p);
extern bool hbuild_page(void);
extern void hdisplay(pointer p,pointer a,bool l);
extern void mem_init(void);
extern pointer copy_node_list(pointer p);
extern void flush_node_list(pointer p);
extern
pointer vpackage(pointer p,scaled h,small_number m,scaled l);
extern pointer hpack(pointer p,scaled w,small_number m);
extern pointer new_math(scaled w,small_number s);
extern pointer new_ligature(quarterword f,quarterword c,pointer q);
extern pointer new_penalty(int m);
extern pointer new_spec(pointer p);
extern pointer new_kern(scaled w);
extern pointer new_disc(void);
extern pointer new_null_box(void);
extern pointer new_rule(void);
extern pointer new_glue(pointer q);
extern pointer new_character(internal_font_number f,eight_bits c);
extern int*const char_base;
extern int*const width_base;
extern memory_word font_info[];
extern scaled*const font_size;
extern char**const font_name;
extern void hclear_fonts(void);
extern void read_font_info(int f,char*nom,scaled s);
extern list_state_record cur_list;
extern pointer get_node(int s);
extern pointer lo_mem_max;
extern pointer hi_mem_min;
extern memory_word*const mem;
extern pointer just_box;
extern void append_to_vlist(pointer b,uint32_t offset);
extern pointer adjust_tail;
extern void freeze_page_specs(small_number s);
extern pointer page_tail;
extern scaled best_height_plus_depth;
extern halfword badness(scaled t,scaled s);
extern void show_box(pointer p);
extern pointer temp_ptr;
extern scaled best_size;
extern pointer best_page_break;
extern int least_page_cost;
extern int insert_penalties;
extern scaled page_so_far[];
extern int page_contents;
extern scaled page_max_depth;
extern int nest_ptr;
extern void pop_nest(void);
extern void push_nest(void);
extern void delete_glue_ref(pointer p);
void line_break(int final_widow_penalty,pointer par_ptr);/*:1410*/
