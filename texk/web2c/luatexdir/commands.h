/* commands.h
   
   Copyright 2008 Taco Hoekwater <taco@luatex.org>

   This file is part of LuaTeX.

   LuaTeX is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2 of the License, or (at your
   option) any later version.

   LuaTeX is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
   License for more details.

   You should have received a copy of the GNU General Public License along
   with LuaTeX; if not, see <http://www.gnu.org/licenses/>. */

/* $Id: commands.h 2094 2009-03-23 14:13:54Z taco $ */


typedef enum {
    relax_cmd = 0,              /* do nothing ( \.{\\relax} ) */
#define escape_cmd  relax_cmd   /* escape delimiter (called \.\\ in {\sl The \TeX book\/}) */
    left_brace_cmd,             /* beginning of a group ( \.\{ ) */
    right_brace_cmd,            /* ending of a group ( \.\} ) */
    math_shift_cmd,             /* mathematics shift character ( \.\$ ) */
    tab_mark_cmd,               /* alignment delimiter ( \.\&, \.{\\span} ) */
    car_ret_cmd,                /* end of line ( |carriage_return|, \.{\\cr}, \.{\\crcr} ) */
#define out_param_cmd  car_ret_cmd      /* output a macro parameter */
    mac_param_cmd,              /* macro parameter symbol ( \.\# ) */
    sup_mark_cmd,               /* superscript ( \.{\char'136} ) */
    sub_mark_cmd,               /* subscript ( \.{\char'137} ) */
    endv_cmd,                   /* end of \<v_j> list in alignment template */
#define ignore_cmd endv_cmd     /* characters to ignore ( \.{\^\^@@} ) */
    spacer_cmd,                 /* characters equivalent to blank space ( \.{\ } ) */
    letter_cmd,                 /* characters regarded as letters ( \.{A..Z}, \.{a..z} ) */
    other_char_cmd,             /* none of the special character types */
    par_end_cmd,                /* end of paragraph ( \.{\\par} ) */
#define active_char_cmd par_end_cmd     /* characters that invoke macros ( \.{\char`\~} ) */
#define match_cmd par_end_cmd   /* match a macro parameter */
    stop_cmd,                   /* end of job ( \.{\\end}, \.{\\dump} ) */
#define comment_cmd stop_cmd    /* characters that introduce comments ( \.\% ) */
#define end_match_cmd stop_cmd  /* end of parameters to macro */
    delim_num_cmd,              /* specify delimiter numerically ( \.{\\delimiter} ) */
#define invalid_char_cmd delim_num_cmd  /* characters that shouldn't appear ( \.{\^\^?} ) */
#define max_char_code_cmd delim_num_cmd /* largest catcode for individual characters */
    char_num_cmd,               /* character specified numerically ( \.{\\char} ) */
    math_char_num_cmd,          /* explicit math code ( \.{\\mathchar} ) */
    mark_cmd,                   /* mark definition ( \.{\\mark} ) */
    xray_cmd,                   /* peek inside of \TeX\ ( \.{\\show}, \.{\\showbox}, etc.~) */
    make_box_cmd,               /* make a box ( \.{\\box}, \.{\\copy}, \.{\\hbox}, etc.~) */
    hmove_cmd,                  /* horizontal motion ( \.{\\moveleft}, \.{\\moveright} ) */
    vmove_cmd,                  /* vertical motion ( \.{\\raise}, \.{\\lower} ) */
    un_hbox_cmd,                /* unglue a box ( \.{\\unhbox}, \.{\\unhcopy} ) */
    un_vbox_cmd,                /* unglue a box ( \.{\\unvbox}, \.{\\unvcopy} or 
                                   \.{\\pagediscards}, \.{\\splitdiscards} ) */
    remove_item_cmd,            /* nullify last item ( \.{\\unpenalty}, \.{\\unkern}, \.{\\unskip} ) */
    hskip_cmd,                  /* horizontal glue ( \.{\\hskip}, \.{\\hfil}, etc.~) */
    vskip_cmd,                  /* vertical glue ( \.{\\vskip}, \.{\\vfil}, etc.~) */
    mskip_cmd,                  /* math glue ( \.{\\mskip} ) */
    kern_cmd,                   /* fixed space ( \.{\\kern}) */
    mkern_cmd,                  /* math kern ( \.{\\mkern} ) */
    leader_ship_cmd,            /* use a box ( \.{\\shipout}, \.{\\leaders}, etc.~) */
    halign_cmd,                 /* horizontal table alignment ( \.{\\halign} ) */
    valign_cmd,                 /* vertical table alignment ( \.{\\valign} ) */
    no_align_cmd,               /* temporary escape from alignment ( \.{\\noalign} ) */
    vrule_cmd,                  /* vertical rule ( \.{\\vrule} ) */
    hrule_cmd,                  /* horizontal rule ( \.{\\hrule} ) */
    insert_cmd,                 /* vlist inserted in box ( \.{\\insert} ) */
    vadjust_cmd,                /* vlist inserted in enclosing paragraph ( \.{\\vadjust} ) */
    ignore_spaces_cmd,          /* gobble |spacer| tokens ( \.{\\ignorespaces} ) */
    after_assignment_cmd,       /* save till assignment is done ( \.{\\afterassignment} ) */
    after_group_cmd,            /* save till group is done ( \.{\\aftergroup} ) */
    break_penalty_cmd,          /* additional badness ( \.{\\penalty} ) */
    start_par_cmd,              /* begin paragraph ( \.{\\indent}, \.{\\noindent} ) */
    ital_corr_cmd,              /* italic correction ( \.{\\/} ) */
    accent_cmd,                 /* attach accent in text ( \.{\\accent} ) */
    math_accent_cmd,            /* attach accent in math ( \.{\\mathaccent} ) */
    discretionary_cmd,          /* discretionary texts ( \.{\\-}, \.{\\discretionary} ) */
    eq_no_cmd,                  /* equation number ( \.{\\eqno}, \.{\\leqno} ) */
    left_right_cmd,             /* variable delimiter ( \.{\\left}, \.{\\right} or \.{\\middle} ) */
    math_comp_cmd,              /* component of formula ( \.{\\mathbin}, etc.~) */
    limit_switch_cmd,           /* diddle limit conventions ( \.{\\displaylimits}, etc.~) */
    above_cmd,                  /* generalized fraction ( \.{\\above}, \.{\\atop}, etc.~) */
    math_style_cmd,             /* style specification ( \.{\\displaystyle}, etc.~) */
    math_choice_cmd,            /* choice specification ( \.{\\mathchoice} ) */
    non_script_cmd,             /* conditional math glue ( \.{\\nonscript} ) */
    vcenter_cmd,                /* vertically center a vbox ( \.{\\vcenter} ) */
    case_shift_cmd,             /* force specific case ( \.{\\lowercase}, \.{\\uppercase}~) */
    message_cmd,                /* send to user ( \.{\\message}, \.{\\errmessage} ) */
    extension_cmd,              /* extensions to \TeX\ ( \.{\\write}, \.{\\special}, etc.~) */
    in_stream_cmd,              /* files for reading ( \.{\\openin}, \.{\\closein} ) */
    begin_group_cmd,            /* begin local grouping ( \.{\\begingroup} ) */
    end_group_cmd,              /* end local grouping ( \.{\\endgroup} ) */
    omit_cmd,                   /* omit alignment template ( \.{\\omit} ) */
    ex_space_cmd,               /* explicit space ( \.{\\\ } ) */
    no_boundary_cmd,            /* suppress boundary ligatures ( \.{\\noboundary} ) */
    radical_cmd,                /* square root and similar signs ( \.{\\radical} ) */
    end_cs_name_cmd,            /* end control sequence ( \.{\\endcsname} ) */
    char_ghost_cmd,             /* \.{\\ghostleft}, \.{\\ghostright} character for kerning */
    assign_local_box_cmd,       /* box for guillemets \.{\\localleftbox} or \.{\\localrightbox} */
    char_given_cmd,             /* character code defined by \.{\\chardef} */
#define min_internal_cmd char_given_cmd /* the smallest code that can follow \.{\\the} */
    math_given_cmd,             /* math code defined by \.{\\mathchardef} */
    omath_given_cmd,            /* math code defined by \.{\\omathchardef} */
    xmath_given_cmd,            /* math code defined by \.{\\LuaTeXmathchardef} */
    last_item_cmd,              /* most recent item ( \.{\\lastpenalty}, \.{\\lastkern}, \.{\\lastskip} ) */
#define max_non_prefixed_command_cmd last_item_cmd      /* largest command code that can't be \.{\\global} */
    toks_register_cmd,          /* token list register ( \.{\\toks} ) */
    assign_toks_cmd,            /* special token list ( \.{\\output}, \.{\\everypar}, etc.~) */
    assign_int_cmd,             /* user-defined integer ( \.{\\tolerance}, \.{\\day}, etc.~) */
    assign_attr_cmd,            /*  user-defined attributes  */
    assign_dimen_cmd,           /* user-defined length ( \.{\\hsize}, etc.~) */
    assign_glue_cmd,            /* user-defined glue ( \.{\\baselineskip}, etc.~) */
    assign_mu_glue_cmd,         /* user-defined muglue ( \.{\\thinmuskip}, etc.~) */
    assign_font_dimen_cmd,      /* user-defined font dimension ( \.{\\fontdimen} ) */
    assign_font_int_cmd,        /* user-defined font integer ( \.{\\hyphenchar}, \.{\\skewchar} ) */
    set_aux_cmd,                /* specify state info ( \.{\\spacefactor}, \.{\\prevdepth} ) */
    set_prev_graf_cmd,          /* specify state info ( \.{\\prevgraf} ) */
    set_page_dimen_cmd,         /* specify state info ( \.{\\pagegoal}, etc.~) */
    set_page_int_cmd,           /* specify state info ( \.{\\deadcycles},  \.{\\insertpenalties} ) */
    set_box_dimen_cmd,          /* change dimension of box ( \.{\\wd}, \.{\\ht}, \.{\\dp} ) */
    set_tex_shape_cmd,          /* specify fancy paragraph shape ( \.{\\parshape} ) */
    set_etex_shape_cmd,         /* specify etex extended list ( \.{\\interlinepenalties}, etc.~) */
    def_char_code_cmd,          /* define a character code ( \.{\\catcode}, etc.~) */
    def_del_code_cmd,           /* define a delimiter code ( \.{\\delcode}) */
    extdef_math_code_cmd,       /* define an extended character code ( \.{\\omathcode}, etc.~) */
    extdef_del_code_cmd,        /* define an extended delimiter code ( \.{\\odelcode}, etc.~) */
    def_family_cmd,             /* declare math fonts ( \.{\\textfont}, etc.~) */
    set_math_param_cmd,         /* set math parameters ( \.{\\mathquad}, etc.~) */
    set_font_cmd,               /* set current font ( font identifiers ) */
    def_font_cmd,               /* define a font file ( \.{\\font} ) */
    register_cmd,               /* internal register ( \.{\\count}, \.{\\dimen}, etc.~) */
    assign_box_dir_cmd,         /* (\.{\\boxdir}) */
    assign_dir_cmd,             /* (\.{\\pagedir}, \.{\\textdir}) */
#define max_internal_cmd assign_dir_cmd /* the largest code that can follow \.{\\the} */
    advance_cmd,                /* advance a register or parameter ( \.{\\advance} ) */
    multiply_cmd,               /* multiply a register or parameter ( \.{\\multiply} ) */
    divide_cmd,                 /* divide a register or parameter ( \.{\\divide} ) */
    prefix_cmd,                 /* qualify a definition ( \.{\\global}, \.{\\long}, \.{\\outer} ) */
    let_cmd,                    /* assign a command code ( \.{\\let}, \.{\\futurelet} ) */
    shorthand_def_cmd,          /* code definition ( \.{\\chardef}, \.{\\countdef}, etc.~) */
    read_to_cs_cmd,             /* read into a control sequence ( \.{\\read} ) */
    def_cmd,                    /* macro definition ( \.{\\def}, \.{\\gdef}, \.{\\xdef}, \.{\\edef} ) */
    set_box_cmd,                /* set a box ( \.{\\setbox} ) */
    hyph_data_cmd,              /* hyphenation data ( \.{\\hyphenation}, \.{\\patterns} ) */
    set_interaction_cmd,        /* define level of interaction ( \.{\\batchmode}, etc.~) */
    letterspace_font_cmd,       /* letterspace a font ( \.{\\letterspacefont} ) */
    pdf_copy_font_cmd,          /* create a new font instance ( \.{\\pdfcopyfont} ) */
    set_ocp_cmd,                /* Place a translation process in the stream */
    def_ocp_cmd,                /* Define and load a translation process */
    set_ocp_list_cmd,           /* Place a list of OCPs in the stream */
    def_ocp_list_cmd,           /* Define a list of OCPs */
    clear_ocp_lists_cmd,        /* Remove all active OCP lists */
    push_ocp_list_cmd,          /* Add to the sequence of active OCP lists */
    pop_ocp_list_cmd,           /* Remove from the sequence of active OCP lists */
    ocp_list_op_cmd,            /* Operations for building a list of OCPs */
    ocp_trace_level_cmd,        /* Tracing of active OCPs, either 0 or 1 */
#define max_command_cmd ocp_trace_level_cmd     /* the largest command code seen at |big_switch| */
    undefined_cs_cmd,           /* initial state of most |eq_type| fields */
    expand_after_cmd,           /* special expansion ( \.{\\expandafter} ) */
    no_expand_cmd,              /* special nonexpansion ( \.{\\noexpand} ) */
    input_cmd,                  /* input a source file ( \.{\\input}, \.{\\endinput} or 
                                   \.{\\scantokens} or \.{\\scantextokens} ) */
    if_test_cmd,                /* conditional text ( \.{\\if}, \.{\\ifcase}, etc.~) */
    fi_or_else_cmd,             /* delimiters for conditionals ( \.{\\else}, etc.~) */
    cs_name_cmd,                /* make a control sequence from tokens ( \.{\\csname} ) */
    convert_cmd,                /* convert to text ( \.{\\number}, \.{\\string}, etc.~) */
    the_cmd,                    /* expand an internal quantity ( \.{\\the} or \.{\\unexpanded}, \.{\\detokenize} ) */
    top_bot_mark_cmd,           /* inserted mark ( \.{\\topmark}, etc.~) */
    call_cmd,                   /* non-long, non-outer control sequence */
    long_call_cmd,              /* long, non-outer control sequence */
    outer_call_cmd,             /* non-long, outer control sequence */
    long_outer_call_cmd,        /* long, outer control sequence */
    end_template_cmd,           /* end of an alignment template */
    dont_expand_cmd,            /* the following token was marked by \.{\\noexpand} */
    glue_ref_cmd,               /* the equivalent points to a glue specification */
    shape_ref_cmd,              /* the equivalent points to a parshape specification */
    box_ref_cmd,                /* the equivalent points to a box node, or is |null| */
    data_cmd,                   /* the equivalent is simply a halfword number */
#define last_cmd data_cmd
} tex_command_code;


typedef enum {
    int_val_level = 0,          /* integer values */
    attr_val_level,             /* integer values */
    dimen_val_level,            /* dimension values */
    glue_val_level,             /* glue specifications */
    mu_val_level,               /* math glue specifications */
    dir_val_level,              /* directions */
    ident_val_level,            /* font identifier */
    tok_val_level,              /* token lists */
} value_level_code;

typedef enum {
    above_code = 0,
    over_code = 1,
    atop_code = 2,
    delimited_code = 3
} fraction_codes;


typedef enum {
    convert_number_code = 0,    /* command code for \.{\\number} */
    convert_roman_numeral_code, /* command code for \.{\\romannumeral} */
    convert_string_code,        /* command code for \.{\\string} */
    convert_meaning_code,       /* command code for \.{\\meaning} */
    convert_font_name_code,     /* command code for \.{\\fontname} */
    convert_etex_code,          /* command code for \.{\\eTeXVersion} */
    convert_omega_code,         /* command code for \.{\\OmegaVersion} */
    convert_aleph_code,         /* command code for \.{\\AlephVersion} */
    convert_format_name_code,   /* command code for \.{\\AlephVersion} */
    convert_pdftex_revision_code,       /* command code for \.{\\pdftexrevision} */
#define convert_pdftex_first_expand_code convert_pdftex_revision_code   /* base for \pdfTeX's command codes */
    convert_pdftex_banner_code, /* command code for \.{\\pdftexbanner} */
    convert_pdf_font_name_code, /* command code for \.{\\pdffontname} */
    convert_pdf_font_objnum_code,       /* command code for \.{\\pdffontobjnum} */
    convert_pdf_font_size_code, /* command code for \.{\\pdffontsize} */
    convert_pdf_page_ref_code,  /* command code for \.{\\pdfpageref} */
    convert_pdf_xform_name_code,        /* command code for \.{\\pdfxformname} */
    convert_left_margin_kern_code,      /* command code for \.{\\leftmarginkern} */
    convert_right_margin_kern_code,     /* command code for \.{\\rightmarginkern} */
    convert_pdf_creation_date_code,     /* command code for \.{\\pdfcreationdate} */
    convert_uniform_deviate_code,       /* command code for \.{\\uniformdeviate} */
    convert_normal_deviate_code,        /* command code for \.{\\normaldeviate} */
    convert_pdf_insert_ht_code, /* command code for \.{\\pdfinsertht} */
    convert_pdf_ximage_bbox_code,       /* command code for \.{\\pdfximagebbox} */
    convert_lua_code,           /* command code for \.{\\directlua} */
    convert_lua_escape_string_code,     /* command code for \.{\\luaescapestring} */
    convert_pdf_colorstack_init_code,   /* command code for \.{\\pdfcolorstackinit} */
    convert_luatex_revision_code,       /* command code for \.{\\luatexrevision} */
    convert_luatex_date_code,   /* command code for \.{\\luatexdate} */
    convert_expanded_code,      /* command code for \.{\\expanded} */
    convert_job_name_code,      /* command code for \.{\\jobname} */
#define  convert_pdftex_convert_codes convert_job_name_code     /* end of \pdfTeX's command codes */
    convert_Aleph_revision_code,        /* command code for \.{\\Alephrevision} */
    convert_Omega_revision_code,        /* command code for \.{\\Omegarevision} */
    convert_eTeX_revision_code, /* command code for \.{\\eTeXrevision} */
    convert_font_identifier_code,       /* command code for \.{tex.fontidentifier} (virtual) */
} convert_codes;

typedef enum {
    last_item_lastpenalty_code = 0,     /* code for \.{\\lastpenalty} */
    last_item_lastattr_code,    /* not used */
    last_item_lastkern_code,    /* code for \.{\\lastkern} */
    last_item_lastskip_code,    /* code for \.{\\lastskip} */
    last_item_last_node_type_code,      /* code for \.{\\lastnodetype} */
    last_item_input_line_no_code,       /* code for \.{\\inputlineno} */
    last_item_badness_code,     /* code for \.{\\badness} */
    last_item_pdftex_version_code,      /* code for \.{\\pdftexversion} */
#define last_item_pdftex_first_rint_code   last_item_pdftex_version_code        /* base for \pdfTeX's command codes */
    last_item_pdf_last_obj_code,        /* code for \.{\\pdflastobj} */
    last_item_pdf_last_xform_code,      /* code for \.{\\pdflastxform} */
    last_item_pdf_last_ximage_code,     /* code for \.{\\pdflastximage} */
    last_item_pdf_last_ximage_pages_code,       /* code for \.{\\pdflastximagepages} */
    last_item_pdf_last_annot_code,      /* code for \.{\\pdflastannot} */
    last_item_pdf_last_x_pos_code,      /* code for \.{\\pdflastxpos} */
    last_item_pdf_last_y_pos_code,      /* code for \.{\\pdflastypos} */
    last_item_pdf_retval_code,  /* global multi-purpose return value */
    last_item_pdf_last_ximage_colordepth_code,  /* code for \.{\\pdflastximagecolordepth} */
    last_item_random_seed_code, /* code for \.{\\pdfrandomseed} */
    last_item_pdf_last_link_code,       /* code for \.{\\pdflastlink} */
    last_item_luatex_version_code,      /* code for \.{\\luatexversion} */
#define last_item_pdftex_last_item_codes last_item_luatex_version_code  /* end of \pdfTeX's command codes */
    last_item_Aleph_version_code,       /* code for \.{\\Alephversion} */
#define last_item_Aleph_int last_item_Aleph_version_code
    last_item_Omega_version_code,       /* code for \.{\\Omegaversion} */
    last_item_Aleph_minor_version_code, /* code for \.{\\Alephminorversion} */
    last_item_Omega_minor_version_code, /* code for \.{\\Omegaminorversion} */
    last_item_eTeX_minor_version_code,  /* code for \.{\\eTeXminorversion} */
    last_item_eTeX_version_code,        /* code for \.{\\eTeXversion} */
#define last_item_eTeX_int last_item_eTeX_version_code  /* first of \eTeX\ codes for integers */
    last_item_current_group_level_code, /* code for \.{\\currentgrouplevel} */
    last_item_current_group_type_code,  /* code for \.{\\currentgrouptype} */
    last_item_current_if_level_code,    /* code for \.{\\currentiflevel} */
    last_item_current_if_type_code,     /* code for \.{\\currentiftype} */
    last_item_current_if_branch_code,   /* code for \.{\\currentifbranch} */
    last_item_glue_stretch_order_code,  /* code for \.{\\gluestretchorder} */
    last_item_glue_shrink_order_code,   /* code for \.{\\glueshrinkorder} */
    last_item_font_char_wd_code,        /* code for \.{\\fontcharwd} */
#define last_item_eTeX_dim last_item_font_char_wd_code  /* first of \eTeX\ codes for dimensions */
    last_item_font_char_ht_code,        /* code for \.{\\fontcharht} */
    last_item_font_char_dp_code,        /* code for \.{\\fontchardp} */
    last_item_font_char_ic_code,        /* code for \.{\\fontcharic} */
    last_item_par_shape_length_code,    /* code for \.{\\parshapelength} */
    last_item_par_shape_indent_code,    /* code for \.{\\parshapeindent} */
    last_item_par_shape_dimen_code,     /* code for \.{\\parshapedimen} */
    last_item_glue_stretch_code,        /* code for \.{\\gluestretch} */
    last_item_glue_shrink_code, /* code for \.{\\glueshrink} */
    last_item_mu_to_glue_code,  /* code for \.{\\mutoglue} */
#define last_item_eTeX_glue last_item_mu_to_glue_code   /* first of \eTeX\ codes for glue */
    last_item_glue_to_mu_code,  /* code for \.{\\gluetomu} */
#define last_item_eTeX_mu last_item_glue_to_mu_code     /* first of \eTeX\ codes for muglue */
    last_item_numexpr_code,     /* code for \.{\\numexpr} */
#define last_item_eTeX_expr last_item_numexpr_code      /* first of \eTeX\ codes for expressions */
    last_item_attrexpr_code,    /* not used */
    last_item_dimexpr_code,     /* code for \.{\\dimexpr} */
    last_item_glueexpr_code,    /* code for \.{\\glueexpr} */
    last_item_muexpr_code,      /* code for \.{\\muexpr} */
} last_item_codes;

typedef enum {
    param_par_shape_code = 0,   /* specifies paragraph shape */
    param_output_routine_code,  /* points to token list for \.{\\output} */
    param_every_par_code,       /* points to token list for \.{\\everypar} */
    param_every_math_code,      /* points to token list for \.{\\everymath} */
    param_every_display_code,   /* points to token list for \.{\\everydisplay} */
    param_every_hbox_code,      /* points to token list for \.{\\everyhbox} */
    param_every_vbox_code,      /* points to token list for \.{\\everyvbox} */
    param_every_job_code,       /* points to token list for \.{\\everyjob} */
    param_every_cr_code,        /* points to token list for \.{\\everycr} */
    param_err_help_code,        /* points to token list for \.{\\errhelp} */
    param_pdf_pages_attr_code,  /* points to token list for \.{\\pdfpagesattr} */
    param_pdf_page_attr_code,   /* points to token list for \.{\\pdfpageattr} */
    param_pdf_page_resources_code,      /* points to token list for \.{\\pdfpageresources} */
    param_pdf_pk_mode_code,     /* points to token list for \.{\\pdfpkmode} */
    param_every_eof_code,       /* points to token list for \.{\\everyeof} */
    param_ocp_trace_level_code,
    param_ocp_active_number_code,
    param_ocp_active_min_ptr_code,
    param_ocp_active_max_ptr_code,
    param_ocp_active_code
} param_local_pars;


typedef enum {
    param_pretolerance_code = 0,        /* badness tolerance before hyphenation */
    param_tolerance_code,       /* badness tolerance after hyphenation */
    param_line_penalty_code,    /* added to the badness of every line */
    param_hyphen_penalty_code,  /* penalty for break after discretionary hyphen */
    param_ex_hyphen_penalty_code,       /* penalty for break after explicit hyphen */
    param_club_penalty_code,    /* penalty for creating a club line */
    param_widow_penalty_code,   /* penalty for creating a widow line */
    param_display_widow_penalty_code,   /* ditto, just before a display */
    param_broken_penalty_code,  /* penalty for breaking a page at a broken line */
    param_bin_op_penalty_code,  /* penalty for breaking after a binary operation */
    param_rel_penalty_code,     /* penalty for breaking after a relation */
    param_pre_display_penalty_code,     /* penalty for breaking just before a displayed formula */
    param_post_display_penalty_code,    /* penalty for breaking just after a displayed formula */
    param_inter_line_penalty_code,      /* additional penalty between lines */
    param_double_hyphen_demerits_code,  /* demerits for double hyphen break */
    param_final_hyphen_demerits_code,   /* demerits for final hyphen break */
    param_adj_demerits_code,    /* demerits for adjacent incompatible lines */
    param_mag_code,             /* magnification ratio */
    param_delimiter_factor_code,        /* ratio for variable-size delimiters */
    param_looseness_code,       /* change in number of lines for a paragraph */
    param_time_code,            /* current time of day */
    param_day_code,             /* current day of the month */
    param_month_code,           /* current month of the year */
    param_year_code,            /* current year of our Lord */
    param_show_box_breadth_code,        /* nodes per level in |show_box| */
    param_show_box_depth_code,  /* maximum level in |show_box| */
    param_hbadness_code,        /* hboxes exceeding this badness will be shown by |hpack| */
    param_vbadness_code,        /* vboxes exceeding this badness will be shown by |vpack| */
    param_pausing_code,         /* pause after each line is read from a file */
    param_tracing_online_code,  /* show diagnostic output on terminal */
    param_tracing_macros_code,  /* show macros as they are being expanded */
    param_tracing_stats_code,   /* show memory usage if \TeX\ knows it */
    param_tracing_paragraphs_code,      /* show line-break calculations */
    param_tracing_pages_code,   /* show page-break calculations */
    param_tracing_output_code,  /* show boxes when they are shipped out */
    param_tracing_lost_chars_code,      /* show characters that aren't in the font */
    param_tracing_commands_code,        /* show command codes at |big_switch| */
    param_tracing_restores_code,        /* show equivalents when they are restored */
    param_uc_hyph_code,         /* hyphenate words beginning with a capital letter */
    param_output_penalty_code,  /* penalty found at current page break */
    param_max_dead_cycles_code, /* bound on consecutive dead cycles of output */
    param_hang_after_code,      /* hanging indentation changes after this many lines */
    param_floating_penalty_code,        /* penalty for insertions heldover after a split */
    param_global_defs_code,     /* override \.{\\global} specifications */
    param_cur_fam_code,         /* current family */
    param_escape_char_code,     /* escape character for token output */
    param_default_hyphen_char_code,     /* value of \.{\\hyphenchar} when a font is loaded */
    param_default_skew_char_code,       /* value of \.{\\skewchar} when a font is loaded */
    param_end_line_char_code,   /* character placed at the right end of the buffer */
    param_new_line_char_code,   /* character that prints as |print_ln| */
    param_language_code,        /* current hyphenation table */
    param_left_hyphen_min_code, /* minimum left hyphenation fragment size */
    param_right_hyphen_min_code,        /* minimum right hyphenation fragment size */
    param_holding_inserts_code, /* do not remove insertion nodes from \.{\\box255} */
    param_error_context_lines_code,     /* maximum intermediate line pairs shown */
    param_local_inter_line_penalty_code,        /* local \.{\\interlinepenalty} */
    param_local_broken_penalty_code,    /* local \.{\\brokenpenalty} */
    param_no_local_whatsits_code,       /* counts local whatsits */
    param_no_local_dirs_code,
    param_level_local_dir_code,
    param_luastartup_id_code,
    param_disable_lig_code,
    param_disable_kern_code,
    param_cat_code_table_code,
    param_output_box_code,
    param_cur_lang_code,        /* current language id */
    param_ex_hyphen_char_code,
    param_page_direction_code,  /* the five direction params do not exist in web */
    param_body_direction_code,
    param_par_direction_code,
    param_text_direction_code,
    param_math_direction_code,
    param_pdf_output_code,      /* switch on PDF output if positive */
    param_pdf_compress_level_code,      /* compress level of streams */
    param_pdf_decimal_digits_code,      /* digits after the decimal point of numbers */
    param_pdf_move_chars_code,  /* move chars 0..31 to higher area if possible */
    param_pdf_image_resolution_code,    /* default image resolution */
    param_pdf_pk_resolution_code,       /* default resolution of PK font */
    param_pdf_unique_resname_code,      /* generate unique names for resouces */
    param_pdf_option_always_use_pdfpagebox_code,        /* if the PDF inclusion should always use a specific PDF page box */
    param_pdf_option_pdf_inclusion_errorlevel_code,     /* if the PDF inclusion should treat pdfs newer than |pdf_minor_version| as an error */
    param_pdf_minor_version_code,       /* fractional part of the PDF version produced */
    param_pdf_force_pagebox_code,       /* if the PDF inclusion should always use a specific PDF page box */
    param_pdf_pagebox_code,     /* default pagebox to use for PDF inclusion */
    param_pdf_inclusion_errorlevel_code,        /* if the PDF inclusion should treat pdfs newer than |pdf_minor_version| as an error */
    param_pdf_gamma_code,
    param_pdf_image_gamma_code,
    param_pdf_image_hicolor_code,
    param_pdf_image_apply_gamma_code,
    param_pdf_adjust_spacing_code,      /* level of spacing adjusting */
    param_pdf_protrude_chars_code,      /* protrude chars at left/right edge of paragraphs */
    param_pdf_tracing_fonts_code,       /* level of font detail in log */
    param_pdf_objcompresslevel_code,    /* activate object streams */
    param_unused_one_code,      /* deleted param (pdftex integer 21) */
    param_unused_two_code,      /* deleted param (pdftex integer 22) */
    param_unused_three_code,    /* deleted param (pdftex integer 23) */
    param_pdf_gen_tounicode_code,       /* generate ToUnicode for fonts? */
    param_pdf_draftmode_code,   /* switch on draftmode if positive */
    param_pdf_replace_font_code,        /* generate ToUnicode for fonts? */
    param_pdf_inclusion_copy_font_code, /* generate ToUnicode for fonts? */
    param_tracing_assigns_code, /* show assignments */
    param_tracing_groups_code,  /* show save/restore groups */
    param_tracing_ifs_code,     /* show conditionals */
    param_tracing_scan_tokens_code,     /* show pseudo file open and close */
    param_tracing_nesting_code, /* show incomplete groups and ifs within files */
    param_pre_display_direction_code,   /* text direction preceding a display */
    param_last_line_fit_code,   /* adjustment for last line of paragraph */
    param_saving_vdiscards_code,        /* save items discarded from vlists */
    param_saving_hyph_codes_code,       /* save hyphenation codes for languages */
    param_suppress_fontnotfound_error_code,     /* suppress errors for missing fonts */
    param_suppress_long_error_code,     /* suppress errors for \\par in non-long contexts */
    param_suppress_ifcsname_error_code, /* suppress errors for \\ifcsname junk contexts */
    param_suppress_outer_error_code,    /* suppress errors for \\outer contexts */
    param_synctex_code,         /*  is synctex file generation enabled ?  */
} int_param_codes;

#define param_int_pars param_synctex_code+1     /* total number of integer parameters */

typedef enum {
    param_par_indent_code = 0,  /* indentation of paragraphs */
    param_math_surround_code,   /* space around math in text */
    param_line_skip_limit_code, /* threshold for |line_skip| instead of |baseline_skip| */
    param_hsize_code,           /* line width in horizontal mode */
    param_vsize_code,           /* page height in vertical mode */
    param_max_depth_code,       /* maximum depth of boxes on main pages */
    param_split_max_depth_code, /* maximum depth of boxes on split pages */
    param_box_max_depth_code,   /* maximum depth of explicit vboxes */
    param_hfuzz_code,           /* tolerance for overfull hbox messages */
    param_vfuzz_code,           /* tolerance for overfull vbox messages */
    param_delimiter_shortfall_code,     /* maximum amount uncovered by variable delimiters */
    param_null_delimiter_space_code,    /* blank space in null delimiters */
    param_script_space_code,    /* extra space after subscript or superscript */
    param_pre_display_size_code,        /* length of text preceding a display */
    param_display_width_code,   /* length of line for displayed equation */
    param_display_indent_code,  /* indentation of line for displayed equation */
    param_overfull_rule_code,   /* width of rule that identifies overfull hboxes */
    param_hang_indent_code,     /* amount of hanging indentation */
    param_h_offset_code,        /* amount of horizontal offset when shipping pages out */
    param_v_offset_code,        /* amount of vertical offset when shipping pages out */
    param_emergency_stretch_code,       /* reduces badnesses on final pass of line-breaking */
    param_page_left_offset_code,
    param_page_top_offset_code,
    param_page_right_offset_code,
    param_page_bottom_offset_code,
    param_pdf_h_origin_code,    /* horigin of the PDF output */
#define  param_pdftex_first_dimen_code param_pdf_horigin_code   /* first number defined in this section */
    param_pdf_v_origin_code,    /* vorigin of the PDF output */
    param_page_width_code,      /* page width of the PDF output */
    param_page_height_code,     /* page height of the PDF output */
    param_pdf_link_margin_code, /* link margin in the PDF output */
    param_pdf_dest_margin_code, /* dest margin in the PDF output */
    param_pdf_thread_margin_code,       /* thread margin in the PDF output */
    param_pdf_first_line_height_code,
    param_pdf_last_line_depth_code,
    param_pdf_each_line_height_code,
    param_pdf_each_line_depth_code,
    param_pdf_ignored_dimen_code,
    param_pdf_px_dimen_code
} dimen_param_codes;

#define param_pdftex_last_dimen_code  param_pdftex_px_dimen_code        /* last number defined in this section */
#define param_dimen_pars param_pdftex_last_dimen_code+1 /* total number of dimension parameters */

typedef enum {
    param_line_skip_code = 0,   /* interline glue if |baseline_skip| is infeasible */
    param_baseline_skip_code,   /* desired glue between baselines */
    param_par_skip_code,        /* extra glue just above a paragraph */
    param_above_display_skip_code,      /* extra glue just above displayed math */
    param_below_display_skip_code,      /* extra glue just below displayed math */
    param_above_display_short_skip_code,        /* glue above displayed math following short lines */
    param_below_display_short_skip_code,        /* glue below displayed math following short lines */
    param_left_skip_code,       /* glue at left of justified lines */
    param_right_skip_code,      /* glue at right of justified lines */
    param_top_skip_code,        /* glue at top of main pages */
    param_split_top_skip_code,  /* glue at top of split pages */
    param_tab_skip_code,        /* glue between aligned entries */
    param_space_skip_code,      /* glue between words (if not |zero_glue|) */
    param_xspace_skip_code,     /* glue after sentences (if not |zero_glue|) */
    param_par_fill_skip_code,   /* glue on last line of paragraph */
    param_thin_mu_skip_code,    /* thin space in math formula */
    param_med_mu_skip_code,     /* medium space in math formula */
    param_thick_mu_skip_code,   /* thick space in math formula */
} param_skip_codes;

#define glue_pars thick_mu_skip_code+1  /* total number of glue parameters */

#define eq_type(a)   zeqtb[a].hh.u.B0
#define equiv(a)     zeqtb[a].hh.v.RH

#define int_par(A)   zeqtb[static_int_base+(A)].cint
#define dimen_par(A) zeqtb[static_dimen_base+(A)].cint
#define loc_par(A)   equiv(static_local_base+(A))
#define glue_par(A)  equiv(static_glue_base+(A))


typedef enum {
    bottom_level = 0,           /* group code for the outside world */
    simple_group,               /* group code for local structure only */
    hbox_group,                 /* code for `\.{\\hbox}\grp' */
    adjusted_hbox_group,        /* code for `\.{\\hbox}\grp' in vertical mode */
    vbox_group,                 /* code for `\.{\\vbox}\grp' */
    vtop_group,                 /* code for `\.{\\vtop}\grp' */
    align_group,                /* code for `\.{\\halign}\grp', `\.{\\valign}\grp' */
    no_align_group,             /* code for `\.{\\noalign}\grp' */
    output_group,               /* code for output routine */
    math_group,                 /* code for, e.g., `\.{\char'136}\grp' */
    disc_group,                 /* code for `\.{\\discretionary}\grp\grp\grp' */
    insert_group,               /* code for `\.{\\insert}\grp', `\.{\\vadjust}\grp' */
    vcenter_group,              /* code for `\.{\\vcenter}\grp' */
    math_choice_group,          /* code for `\.{\\mathchoice}\grp\grp\grp\grp' */
    semi_simple_group,          /* code for `\.{\\begingroup...\\endgroup}' */
    math_shift_group,           /* code for `\.{\$...\$}' */
    math_left_group,            /* code for `\.{\\left...\\right}' */
    local_box_group,            /* code for `\.{\\localleftbox...\\localrightbox}' */
    max_group_code,
    split_off_group,            /* box code for the top part of a \.{\\vsplit} */
    split_keep_group,           /* box code for the bottom part of a \.{\\vsplit} */
    preamble_group,             /* box code for the preamble processing  in an alignment */
    align_set_group,            /* box code for the final item pass in an alignment */
    fin_row_group               /* box code for a provisory line in an alignment */
} tex_group_codes;
