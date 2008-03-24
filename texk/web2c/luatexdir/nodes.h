
#include <stdarg.h>

#ifndef __NODES_H__
#define __NODES_H__

/* these don't really belong here */

#define token_ref_count(a) info((a)) /* reference count preceding a token list */
#define add_token_ref(a) token_ref_count(a)++  /* new reference to a token list */


/* these are in texlang.c */

#define set_vlink(a,b)  vlink(a)=b
#define get_vlink(a)  vlink(a)
#define get_character(a)  character(a)

extern halfword insert_discretionary ( halfword t,  halfword pre,  halfword post,  halfword replace);
extern halfword insert_syllable_discretionary ( halfword t,  lang_variables *lan);
extern halfword insert_word_discretionary ( halfword t,  lang_variables *lan);
extern halfword insert_complex_discretionary ( halfword t,   lang_variables *lan, 
					       halfword pre,  halfword post,  halfword replace);
extern halfword insert_character ( halfword t,  int n);
extern void set_disc_field (halfword f, halfword t);


#define max_halfword  0x3FFFFFFF
#ifndef null
#define null         0
#endif
#define null_flag    -0x40000000
#define zero_glue 0
#define normal 0

#define vinfo(a)           varmem[(a)].hh.v.LH 
#define vlink(a)           varmem[(a)].hh.v.RH 
#define type(a)            varmem[(a)].hh.u.B0
#define subtype(a)         varmem[(a)].hh.u.B1
#define node_attr(a)       vinfo((a)+1)
#define alink(a)           vlink((a)+1)

#define node_size(a)       varmem[(a)].hh.v.LH

#define rlink(a)           vlink((a)+1) /* aka alink() */
#define llink(a)           vinfo((a)+1) /* overlaps with node_attr() */

#define add_glue_ref(a) glue_ref_count(a)++ /* new reference to a glue spec */

/* really special head node pointers that only need links */

#define temp_node_size 2

/* attribute lists */

/* it is convenient to have attribute list nodes and attribute node
 * be the same size
 */

#define attribute_node_size 2

#define attr_list_ref(a)   vinfo((a)+1) /* the reference count */
#define attribute_id(a)    vinfo((a)+1)
#define attribute_value(a) vlink((a)+1)

#define cache_disabled max_halfword
#define add_node_attr_ref(a) { if (a!=null)  attr_list_ref((a))++; }

#define  replace_attribute_list(a,b)  do {		\
    delete_attribute_ref(node_attr(a));			\
    node_attr(a)=b;					\
  } while (0)

extern void update_attribute_cache (void) ;
extern halfword copy_attribute_list(halfword n) ;
extern halfword do_set_attribute(halfword p, int i, int val);


/* a glue spec */
#define glue_spec_size 4
#define stretch(a)        vlink((a)+1)
/* width == a+2 */
#define shrink(a)         vinfo((a)+1)
#define stretch_order(a)  type((a)+3)
#define shrink_order(a)   subtype((a)+3)
#define glue_ref_count(a) vlink((a)+3)

/* pdf action spec */

#define pdf_action_size 4

typedef enum {
  pdf_action_page = 0,
  pdf_action_goto,
  pdf_action_thread,
  pdf_action_user } pdf_action_types;

#define pdf_action_type(a)        type((a) + 1)
#define pdf_action_named_id(a)    subtype((a) + 1)
#define pdf_action_id(a)          vlink((a) + 1)
#define pdf_action_file(a)        vinfo((a) + 2)
#define pdf_action_new_window(a)  vlink((a) + 2)
#define pdf_action_tokens(a)      vinfo((a) + 3)
#define pdf_action_refcount(a)    vlink((a) + 3)

/*increase count of references to this action*/
#define add_action_ref(a) pdf_action_refcount((a))++ 

/* decrease count of references to this
   action; free it if there is no reference to this action*/

#define delete_action_ref(a) {									\
    if (pdf_action_refcount(a) == null) {						\
	  if (pdf_action_type(a) == pdf_action_user) {				\
		delete_token_ref(pdf_action_tokens(a));					\
	  } else {													\
		if (pdf_action_file(a) != null)							\
		  delete_token_ref(pdf_action_file(a));					\
		if (pdf_action_type(a) == pdf_action_page)				\
		  delete_token_ref(pdf_action_tokens(a));				\
		else if (pdf_action_named_id(a) > 0)					\
		  delete_token_ref(pdf_action_id(a));					\
	  }															\
	  free_node(a, pdf_action_size);							\
	} else {													\
	  decr(pdf_action_refcount(a));								\
	}															\
  }



/* normal nodes */

#define inf_bad  10000  /* infinitely bad value */
#define inf_penalty inf_bad /*``infinite'' penalty value*/
#define eject_penalty -(inf_penalty) /*``negatively infinite'' penalty value */

#define penalty_node_size 3 
#define penalty(a)       vlink((a)+2)

#define glue_node_size 3
#define glue_ptr(a)      vinfo((a)+2)
#define leader_ptr(a)    vlink((a)+2)

/* disc nodes could eventually be smaller, because the indirect 
   pointers are not really needed (8 instead of 10).
 */

#define disc_node_size 10

typedef enum {
  discretionary_disc=0,
  explicit_disc,
  automatic_disc,
  syllable_disc } discretionary_types;

#define pre_break_head(a)   ((a)+4)
#define post_break_head(a)  ((a)+6)
#define no_break_head(a)    ((a)+8)

#define pre_break(a)     vinfo((a)+2)
#define post_break(a)    vlink((a)+2)
#define no_break(a)      vlink((a)+3)
#define tlink llink

#define vlink_pre_break(a)  vlink(pre_break_head(a))
#define vlink_post_break(a) vlink(post_break_head(a))
#define vlink_no_break(a)   vlink(no_break_head(a))

#define tlink_pre_break(a)  tlink(pre_break_head(a))
#define tlink_post_break(a) tlink(post_break_head(a))
#define tlink_no_break(a)   tlink(no_break_head(a))

#define kern_node_size 3
#define explicit 1  /*|subtype| of kern nodes from \.{\\kern} and \.{\\/}*/
#define acc_kern 2  /*|subtype| of kern nodes from accents */

#define box_node_size 8
#define width(a)         varmem[(a+2)].cint
#define depth(a)         varmem[(a+3)].cint
#define height(a)        varmem[(a+4)].cint
#define shift_amount(a)  vlink((a)+5)
#define box_dir(a)       vinfo((a)+5)
#define list_ptr(a)      vlink((a)+6)
#define glue_order(a)    subtype((a)+6)
#define glue_sign(a)     type((a)+6)
#define glue_set(a)      varmem[(a+7)].gr

/* unset nodes */
#define glue_stretch(a)  varmem[(a)+7].cint
#define glue_shrink      shift_amount
#define span_count       subtype

#define rule_node_size 6
#define rule_dir(a)      vlink((a)+5)

#define mark_node_size 3
#define mark_ptr(a)      vlink((a)+2)
#define mark_class(a)    vinfo((a)+2)

#define adjust_node_size 3
#define adjust_ptr(a)    vlink(a+2)

#define glyph_node_size 5

#define character(a)    vinfo((a)+2)
#define font(a)         vlink((a)+2)
#define lang_data(a)    vinfo((a)+3)
#define lig_ptr(a)      vlink((a)+3)
#define x_displace(a)   vinfo((a)+4)
#define y_displace(a)   vlink((a)+4)
#define is_char_node(a) (a!=null && type(a)==glyph_node)

#define char_lang(a)     ((const int)(((unsigned)lang_data(a) & 0x7FFF0000)>>16))
#define char_lhmin(a)    ((const int)(((unsigned)lang_data(a) & 0x0000FF00)>>8))
#define char_rhmin(a)    ((const int)(((unsigned)lang_data(a) & 0x000000FF)))
#define char_uchyph(a)   ((const int)(((unsigned)lang_data(a) & 0x80000000)>>31))

#define make_lang_data(a,b,c,d) (a>0 ? (1<<31): 0)+			\
  (b<<16)+ (((c>0 && c<256) ? c : 1)<<8)+(((d>0 && d<256) ? d : 1))

#define init_lang_data(a)      lang_data(a)=256+1

#define set_char_lang(a,b)    lang_data(a)=make_lang_data(char_uchyph(a),b,char_lhmin(a),char_rhmin(a))
#define set_char_lhmin(a,b)   lang_data(a)=make_lang_data(char_uchyph(a),char_lang(a),b,char_rhmin(a))
#define set_char_rhmin(a,b)   lang_data(a)=make_lang_data(char_uchyph(a),char_lang(a),char_lhmin(a),b)
#define set_char_uchyph(a,b)  lang_data(a)=make_lang_data(b,char_lang(a),char_lhmin(a),char_rhmin(a))

#define margin_kern_node_size 4
#define margin_char(a)  vlink((a)+3)

/*@# {|subtype| of marginal kerns}*/
#define left_side 0
#define right_side 1

#define math_node_size 3
#define surround(a)      vlink((a)+2)
#define before 0 /*|subtype| for math node that introduces a formula*/
#define after 1 /*|subtype| for math node that winds up a formula*/



#define ins_node_size 6
#define float_cost(a)    varmem[(a)+2].cint
#define ins_ptr(a)       vinfo((a)+5)
#define split_top_ptr(a) vlink((a)+5)


#define page_ins_node_size 5

#define broken_ptr(a) vlink((a)+2) /* an insertion for this class will break here if anywhere */
#define broken_ins(a) vinfo((a)+2) /* this insertion might break at |broken_ptr| */
#define last_ins_ptr(a) vlink((a)+3) /*the most recent insertion for this |subtype|*/
#define best_ins_ptr(a) vinfo((a)+3) /*the optimum most recent insertion*/
/* height = 4 */

typedef enum {
  hlist_node = 0, 
  vlist_node = 1, 
  rule_node,      
  ins_node,       
  mark_node,      
  adjust_node,    
  /* 6 used to be ligatures */
  disc_node=7,     
  whatsit_node,
  math_node,      
  glue_node,      
  kern_node,      
  penalty_node,   
  unset_node,   /* 13 */
  style_node,
  choice_node,
  ord_noad,
  op_noad,
  bin_noad,
  rel_noad,
  open_noad,
  close_noad,
  punct_noad,
  inner_noad,
  radical_noad,
  fraction_noad,
  under_noad,
  over_noad,
  accent_noad,
  vcenter_noad,
  left_noad,
  right_noad,
  margin_kern_node = 32,
  glyph_node = 33,
  align_record_node = 34,
  pseudo_file_node = 35,
  pseudo_line_node = 36,
  inserting_node = 37,
  split_up_node = 38,
  expr_node = 39,
  nesting_node = 40,
  span_node = 41,
  attribute_node = 42,
  glue_spec_node = 43,
  attribute_list_node = 44,
  action_node = 45,
  temp_node = 46,
  align_stack_node = 47,
  movement_node = 48,
  if_node = 49,
  unhyphenated_node = 50, 
  hyphenated_node = 51,
  delta_node = 52,
  passive_node = 53,
  shape_node = 54 } node_types ;

#define last_known_node temp_node /* used by \lastnodetype */

#define movement_node_size 3
#define expr_node_size 3
#define if_node_size 2
#define align_stack_node_size 6
#define nesting_node_size 2

#define span_node_size 3
#define span_span(a) vlink((a)+1)
#define span_link(a) vinfo((a)+1)

#define pseudo_file_node_size 2
#define pseudo_lines(a) vlink((a)+1)

#define nodetype_has_attributes(t) (((t)<=glyph_node) && ((t)!=unset_node))

#define style_node_size 4 /* number of words in a style node*/
#define radical_noad_size 6 /*number of |mem| words in a radical noad*/
#define accent_noad_size 6 /*number of |mem| words in an accent noad*/


#define display_mlist(a) vinfo((a)+2) /* mlist to be used in display style*/
#define text_mlist(a) vlink((a)+2) /* mlist to be used in text style */
#define script_mlist(a) vinfo((a)+3) /* mlist to be used in script style */
#define script_script_mlist(a) vlink((a)+3) /* mlist to be used in scriptscript style */

#define noad_size 5 /*number of words in a normal noad*/
#define nucleus(a) (a)+2 /* the |nucleus| field of a noad */
#define supscr(a) (a)+3 /* the |supscr| field of a noad */
#define subscr(a) (a)+4 /* the |subscr| field of a noad */
#define math_type vlink /* a |quarterword| in |mem| */
#define fam(a) type((a)) /* a |quarterword| in |mem| */

#define fraction_noad_size 7 /*number of |mem| words in a fraction noad*/
#define numerator supscr /*|numerator| field in a fraction noad*/
#define denominator subscr /*|denominator| field in a fraction noad*/

typedef enum {
  math_char=1, /* |math_type| when the attribute is simple */
  sub_box, /* |math_type| when the attribute is a box */
  sub_mlist, /* |math_type| when the attribute is a formula */
  math_text_char /* |math_type| when italic correction is dubious */
} math_types;


typedef enum {
  open_node = 0,
  write_node,
  close_node,
  special_node,
  language_node,
  set_language_code,
  local_par_node,
  dir_node,
  pdf_literal_node,
  pdf_obj_code,
  pdf_refobj_node, /* 10 */
  pdf_xform_code,
  pdf_refxform_node,
  pdf_ximage_code,
  pdf_refximage_node,
  pdf_annot_node,
  pdf_start_link_node,
  pdf_end_link_node,
  pdf_outline_code,
  pdf_dest_node,
  pdf_thread_node, /* 20 */
  pdf_start_thread_node,
  pdf_end_thread_node,
  pdf_save_pos_node,
  pdf_thread_data_node,
  pdf_link_data_node,
  pdf_names_code,
  pdf_font_attr_code,
  pdf_include_chars_code,
  pdf_map_file_code,
  pdf_map_line_code, /* 30 */
  pdf_trailer_code,
  pdf_font_expand_code,
  set_random_seed_code,
  pdf_glyph_to_unicode_code,
  late_lua_node, /* 35 */
  close_lua_node,
  save_cat_code_table_code,
  init_cat_code_table_code,
  pdf_colorstack_node,
  pdf_setmatrix_node, /* 40 */
  pdf_save_node,
  pdf_restore_node,
  cancel_boundary_node,
  user_defined_node /* 44 */ } whatsit_types ;

#define pdf_info_code pdf_thread_data_node
#define pdf_catalog_code  pdf_link_data_node


#define GLYPH_CHARACTER     (1 << 0)
#define GLYPH_LIGATURE      (1 << 1)
#define GLYPH_GHOST         (1 << 2)
#define GLYPH_LEFT          (1 << 3)
#define GLYPH_RIGHT         (1 << 4)

#define is_character(p)        ((subtype(p)) & GLYPH_CHARACTER)
#define is_ligature(p)         ((subtype(p)) & GLYPH_LIGATURE )
#define is_ghost(p)            ((subtype(p)) & GLYPH_GHOST    )

#define is_simple_character(p) (is_character(p) && !is_ligature(p) && !is_ghost(p))

#define is_leftboundary(p) 	   (is_ligature(p) && ((subtype(p)) & GLYPH_LEFT  ))
#define is_rightboundary(p)    (is_ligature(p) && ((subtype(p)) & GLYPH_RIGHT ))
#define is_leftghost(p) 	   (is_ghost(p)    && ((subtype(p)) & GLYPH_LEFT  ))
#define is_rightghost(p)  	   (is_ghost(p)    && ((subtype(p)) & GLYPH_RIGHT ))

#define set_is_glyph(p)         subtype(p) &= ~GLYPH_CHARACTER
#define set_is_character(p)     subtype(p) |= GLYPH_CHARACTER
#define set_is_ligature(p)      subtype(p) |= GLYPH_LIGATURE
#define set_is_ghost(p)         subtype(p) |= GLYPH_GHOST

#define set_to_glyph(p)         subtype(p) = (subtype(p) & 0xFF00)
#define set_to_character(p)     subtype(p) = (subtype(p) & 0xFF00) | GLYPH_CHARACTER
#define set_to_ligature(p)      subtype(p) = (subtype(p) & 0xFF00) | GLYPH_LIGATURE
#define set_to_ghost(p)         subtype(p) = (subtype(p) & 0xFF00) | GLYPH_GHOST

#define set_is_leftboundary(p)  { set_to_ligature(p); subtype(p) |= GLYPH_LEFT;  }
#define set_is_rightboundary(p) { set_to_ligature(p); subtype(p) |= GLYPH_RIGHT; }
#define set_is_leftghost(p)     { set_to_ghost(p);    subtype(p) |= GLYPH_LEFT;  }
#define set_is_rightghost(p)    { set_to_ghost(p);    subtype(p) |= GLYPH_RIGHT; }


#define special_node_size 3

#define dir_node_size 4
#define dir_dir(a)       vinfo((a)+2)
#define dir_level(a)     vlink((a)+2)
#define dir_dvi_ptr(a)   vinfo((a)+3)
#define dir_dvi_h(a)     vlink((a)+3)

#define write_node_size 3
#define close_node_size 3
#define write_tokens(a)  vlink(a+2)
#define write_stream(a)  vinfo(a+2)

#define open_node_size 4
#define open_name(a)   vlink((a)+2)
#define open_area(a)   vinfo((a)+3)
#define open_ext(a)    vlink((a)+3)

#define late_lua_data(a)        vlink((a)+2)
#define late_lua_reg(a)         vinfo((a)+2)

#define local_par_size 6

#define local_pen_inter(a)       vinfo((a)+2)
#define local_pen_broken(a)      vlink((a)+2)
#define local_box_left(a)        vlink((a)+3)
#define local_box_left_width(a)  vinfo((a)+3)
#define local_box_right(a)       vlink((a)+4)
#define local_box_right_width(a) vinfo((a)+4)
#define local_par_dir(a)         vinfo((a)+5)


#define pdf_literal_data(a)  vlink(a+2)
#define pdf_literal_mode(a)  vinfo(a+2)

#define pdf_refobj_node_size 3

#define pdf_obj_objnum(a)    vinfo((a) + 2)

#define pdf_refxform_node_size  6
#define pdf_refximage_node_size 6
#define pdf_annot_node_size 8
#define pdf_dest_node_size 8
#define pdf_thread_node_size 8

#define pdf_width(a)         varmem[(a) + 2].cint
#define pdf_height(a)        varmem[(a) + 3].cint
#define pdf_depth(a)         varmem[(a) + 4].cint

#define pdf_ximage_objnum(a) vinfo((a) + 5) /* this will be removed soon */
#define pdf_ximage_ref(a)    vinfo((a) + 5)
#define pdf_xform_objnum(a)  vinfo((a) + 5)

#define pdf_annot_data(a)       vinfo((a) + 6)
#define pdf_link_attr(a)        vinfo((a) + 6)
#define pdf_link_action(a)      vlink((a) + 6)
#define pdf_annot_objnum(a)     varmem[(a) + 7].cint
#define pdf_link_objnum(a)      varmem[(a) + 7].cint

#define pdf_dest_type(a)          type((a) + 6)
#define pdf_dest_named_id(a)      subtype((a) + 6)
#define pdf_dest_id(a)            vlink((a) + 6)
#define pdf_dest_xyz_zoom(a)      vinfo((a) + 7)
#define pdf_dest_objnum(a)        vlink((a) + 7)

#define pdf_thread_named_id(a)    subtype((a) + 6)
#define pdf_thread_id(a)          vlink((a) + 6)
#define pdf_thread_attr(a)        vinfo((a) + 7)

#define pdf_end_link_node_size 3
#define pdf_end_thread_node_size 3
#define pdf_save_pos_node_size 3

#define pdf_colorstack_node_size 4
#define pdf_setmatrix_node_size 3

#define pdf_colorstack_stack(a)  vlink((a)+2)
#define pdf_colorstack_cmd(a)    vinfo((a)+2)
#define pdf_colorstack_data(a)   vlink((a)+3)
#define pdf_setmatrix_data(a)    vlink((a)+2)

#define pdf_save_node_size     3
#define pdf_restore_node_size  3

typedef enum {
  colorstack_set=0,
  colorstack_push,
  colorstack_pop,
  colorstack_current } colorstack_commands;

#define colorstack_data   colorstack_push /* last value where data field is set */

#define user_defined_node_size 4
#define user_node_type(a)  vinfo((a)+2)
#define user_node_id(a)    vlink((a)+2)
#define user_node_value(a) vinfo((a)+3)

#define cancel_boundary_size   3

typedef enum {
  exactly=0, /*a box dimension is pre-specified*/
  additional, /*a box dimension is increased from the natural one*/
  cal_expand_ratio, /* calculate amount for font expansion after breaking
		       paragraph into lines*/
  subst_ex_font  /* substitute fonts */
} hpack_subtypes;

#define active_node_size 4 /*number of words in extended active nodes*/
#define fitness subtype /*|very_loose_fit..tight_fit| on final line for this break*/
#define break_node(a) vlink((a)+1) /*pointer to the corresponding passive node */
#define line_number(a) vinfo((a)+1) /*line that begins at this breakpoint*/
#define total_demerits(a) varmem[(a)+2].cint /* the quantity that \TeX\ minimizes*/
#define active_short(a) vinfo(a+3) /* |shortfall| of this line */
#define active_glue(a)  vlink(a+3) /*corresponding glue stretch or shrink*/

#define passive_node_size 7
#define cur_break(a)                   vlink((a)+1) /*in passive node, points to position of this breakpoint*/
#define prev_break(a)                  vinfo((a)+1) /*points to passive node that should precede this one */
#define passive_pen_inter(a)           vinfo((a)+2)
#define passive_pen_broken(a)          vlink((a)+2)
#define passive_left_box(a)            vlink((a)+3)
#define passive_left_box_width(a)      vinfo((a)+3)
#define passive_last_left_box(a)       vlink((a)+4)
#define passive_last_left_box_width(a) vinfo((a)+4)
#define passive_right_box(a)           vlink((a)+5)
#define passive_right_box_width(a)     vinfo((a)+5)
#define serial(a)                      vlink((a)+6) /* serial number for symbolic identification*/

#define delta_node_size 10 /* 8 fields, stored in a+1..9 */

#define couple_nodes(a,b) {assert(b!=null);vlink(a)=b;alink(b)=a;}
#define try_couple_nodes(a,b) if (b==null) vlink(a)=b; else {couple_nodes(a,b);}
#define uncouple_node(a) {assert(a!=null);vlink(a)=null;alink(a)=null;}

#define cache_disabled max_halfword

extern void delete_attribute_ref(halfword b) ;
extern void build_attribute_list(halfword b) ;

extern int unset_attribute(halfword n, int c, int w);
extern void set_attribute(halfword n, int c, int w);
extern int has_attribute(halfword n, int c, int w);

extern halfword new_span_node (halfword n, int c, scaled w);
extern halfword string_to_pseudo(integer l,integer pool_ptr, integer nl);

/* TH: these two defines still need checking. The node ordering in luatex is not 
   quite the same as in tex82 */

#define precedes_break(a) (type((a))<math_node && (type(a)!=whatsit_node || subtype(a)!=dir_node))
#define non_discardable(a) (type((a))<math_node)

/* from luanode.c */

typedef struct _node_info {
  int id;
  int size;
  char **fields;
  char *name; 
}  node_info;

extern node_info node_data[];
extern node_info whatsit_node_data[];
extern halfword new_node(int i, int j);
extern void flush_node_list(halfword);
extern void flush_node(halfword);
extern halfword copy_node_list(halfword);
extern halfword copy_node(halfword);
extern void check_node(halfword);
extern void check_node_mem (void);
extern void fix_node_list (halfword);
extern int fix_node_lists;
extern char *sprint_node_mem_usage (void) ;
extern halfword raw_glyph_node(void) ;
extern halfword new_glyph_node(void);
extern int valid_node(halfword);

#define unity 0x10000
typedef enum {
  normal_g=0,
  sfi,
  fil,
  fill,
  filll } glue_orders;

#define zero_glue       0
#define sfi_glue        zero_glue+glue_spec_size
#define fil_glue        sfi_glue+glue_spec_size
#define fill_glue       fil_glue+glue_spec_size
#define ss_glue         fill_glue+glue_spec_size
#define fil_neg_glue    ss_glue+glue_spec_size
#define page_ins_head   fil_neg_glue+glue_spec_size
#define contrib_head    page_ins_head+temp_node_size
#define page_head       contrib_head+temp_node_size
#define temp_head       page_head+temp_node_size
#define hold_head       temp_head+temp_node_size
#define adjust_head     hold_head+temp_node_size
#define pre_adjust_head adjust_head+temp_node_size
#define active          pre_adjust_head+temp_node_size
#define align_head      active+active_node_size
#define end_span        align_head+temp_node_size
#define begin_point     end_span+glyph_node_size
#define end_point       begin_point+glyph_node_size

#endif
