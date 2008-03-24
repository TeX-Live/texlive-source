/* $Id$ */


#define token_list 0
#define cs_token_flag 0x1FFFFFFF
#define string_offset 0x200000
#define string_offset_bits 21

#define token_type index /* type of current token list */
#define param_start limit /* base of macro parameters in |param_stack| */

typedef enum {
  parameter=0,  /*  code for parameter */
  u_template=1,  /*  code for \<u_j> template */
  v_template=2,  /*  code for \<v_j> template */
  backed_up=3,  /*  code for text to be reread */
  inserted=4,  /*  code for inserted texts */
  macro=5,  /*  code for defined control sequences */
  output_text=6,  /*  code for output routines */
  every_par_text=7,  /*  code for \.{\\everypar}  */
  every_math_text=8,  /*  code for \.{\\everymath} */
  every_display_text=9,  /*  code for \.{\\everydisplay} */
  every_hbox_text=10,  /*  code for \.{\\everyhbox} */
  every_vbox_text=11,  /*  code for \.{\\everyvbox} */
  every_job_text=12,  /*  code for \.{\\everyjob}/ */
  every_cr_text=13,  /*  code for \.{\\everycr} */
  mark_text=14,  /*  code for \.{\\topmark}, etc. */
  every_eof_text=19 /* was calculated, value fetched from the generated C */
} token_types;

#undef link /* defined by cpascal.h */
#define info(a)    fixmem[(a)].hhlh
#define link(a)    fixmem[(a)].hhrh

#define store_new_token(a) { q=get_avail(); link(p)=q; info(q)=(a); p=q; }
#define free_avail(a)      { link((a))=avail; avail=(a); decr(dyn_used); }


#define str_start_macro(a) str_start[(a) - string_offset]

#define length(a) (str_start_macro((a)+1)-str_start_macro(a))

#if defined(index)
#undef index
#endif

#define loc      cur_input.loc_field       /* location of first unread character in |buffer| */
#define state    cur_input.state_field     /* current scanner state */
#define index    cur_input.index_field     /* reference for buffer information */
#define start    cur_input.start_field     /* starting position in |buffer| */
#define limit    cur_input.limit_field     /* end of current line in |buffer| */
#define nofilter cur_input.nofilter_field  /* is token filtering explicitly disallowed? */
#define name     cur_input.name_field      /* name of the current file */
#define current_ocp_lstack cur_input.ocp_lstack_field /* name of the current ocp */
#define current_ocp_no cur_input.ocp_no_field /* name of the current ocp */

#define max_char_code 15 /* largest catcode for individual characters */

typedef enum {  
  mid_line=1, /* |state| code when scanning a line of characters */
  skip_blanks=2+max_char_code, /* |state| code when ignoring blanks */
  new_line=3+max_char_code+max_char_code, /* |state| code at start of line */
} state_codes ;


extern void make_token_table (lua_State *L, int cmd, int chr, int cs);


