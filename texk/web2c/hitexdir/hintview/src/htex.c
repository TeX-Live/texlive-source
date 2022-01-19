/*4:*/
#line 16 "htex.ch"

#ifndef _HETX_H_
#define _HTEX_H_

#include "basetypes.h"
#include <string.h> 
#include <math.h> 

#include "error.h"
#include "format.h"
#include "get.h"
#include "htex.h"
#include "hint.h"
/*120:*/
#line 262 "htex.ch"

pointer temp_ptr;
#line 2496 "btex.w"

/*:120*//*121:*/
#line 273 "htex.ch"

memory_word mem0[mem_max-mem_min+1],*const mem= mem0-mem_min;
pointer lo_mem_max;
pointer hi_mem_min;
#line 2531 "btex.w"

/*:121*//*122:*/
#line 283 "htex.ch"

static int var_used,dyn_used;
#line 2542 "btex.w"
#ifdef STAT
#define incr_dyn_used incr(dyn_used)
#define decr_dyn_used decr(dyn_used)
#else
#define incr_dyn_used
#define decr_dyn_used
#endif

/*:122*//*123:*/
#line 293 "htex.ch"

static pointer avail;
#line 2566 "btex.w"

/*:123*//*129:*/
#line 330 "htex.ch"

static pointer rover;
#line 2670 "btex.w"

/*:129*//*658:*/
#line 1093 "htex.ch"

static scaled total_stretch0[filll-normal+1],
*const total_stretch= total_stretch0-normal,
total_shrink0[filll-normal+1],*const total_shrink= total_shrink0-normal;

#line 12962 "btex.w"

/*:658*//*660:*/
#line 1103 "htex.ch"

pointer adjust_tail= null;
#line 12970 "btex.w"

/*:660*//*829:*/
#line 1220 "htex.ch"

pointer just_box;
#line 16110 "btex.w"

/*:829*//*837:*/
#line 1270 "htex.ch"

static pointer passive;
#line 16253 "btex.w"
static pointer printed_node;
static halfword pass_number;

/*:837*//*840:*/
#line 1277 "htex.ch"

static scaled active_width0[6],*const active_width= active_width0-1;
#line 16292 "btex.w"

static scaled cur_active_width0[6],
*const cur_active_width= cur_active_width0-1;
static scaled background0[6],*const background= background0-1;
static scaled break_width0[6],*const break_width= break_width0-1;

/*:840*//*843:*/
#line 1284 "htex.ch"

static bool no_shrink_error_yet;
#line 16350 "btex.w"

/*:843*//*847:*/
#line 1315 "htex.ch"

static pointer cur_p;
#line 16410 "btex.w"
static bool second_pass;
static bool final_pass;
static int threshold;

/*:847*//*853:*/
#line 1324 "htex.ch"

static int minimal_demerits0[tight_fit-very_loose_fit+1],
*const minimal_demerits= minimal_demerits0-very_loose_fit;

#line 16513 "btex.w"
static int minimum_demerits;

static pointer best_place0[tight_fit-very_loose_fit+1],
*const best_place= best_place0-very_loose_fit;

static halfword best_pl_line0[tight_fit-very_loose_fit+1],
*const best_pl_line= best_pl_line0-very_loose_fit;


/*:853*//*860:*/
#line 1333 "htex.ch"

static scaled disc_width;
#line 16632 "btex.w"

/*:860*//*869:*/
#line 1340 "htex.ch"

static halfword easy_line;
#line 16774 "btex.w"
static halfword last_special_line;

static scaled first_width;

static scaled second_width;
static scaled first_indent;
static scaled second_indent;

/*:869*//*895:*/
#line 1444 "htex.ch"

static pointer best_bet;
#line 17288 "btex.w"
static int fewest_demerits;
static halfword best_line;
static int actual_looseness;

static int line_diff;


/*:895*//*996:*/
#line 1640 "htex.ch"

scaled best_height_plus_depth;

#line 19052 "btex.w"

/*:996*//*1005:*/
#line 1670 "htex.ch"

pointer page_tail;
#line 19258 "btex.w"
int page_contents;
scaled page_max_depth;
pointer best_page_break;
int least_page_cost;
scaled best_size;

/*:1005*//*1007:*/
#line 1681 "htex.ch"

scaled page_so_far[8];
#line 19343 "btex.w"
int insert_penalties;


/*:1007*/
#line 29 "htex.ch"


/*97:*/
#line 164 "htex.ch"

void overflow(char*s,int n)
{QUIT("Capacity exceeded, sorry [%s=%d=0x%X]\n",s,n,n);
}
#line 2028 "btex.w"

/*:97*//*98:*/
#line 188 "htex.ch"

void confusion(char*s)

{QUIT("This can't happen(%s)",s);}
#line 2053 "btex.w"

/*:98*/
#line 31 "htex.ch"

#endif
#line 268 "btex.w"

/*:4*//*103:*/
#line 197 "htex.ch"


static int half(int x)
#line 2117 "btex.w"
{if(odd(x))return(x+1)/2;
else return x/2;
}

/*:103*//*108:*/
#line 233 "htex.ch"

static bool arith_error;
static scaled rem;
#line 2201 "btex.w"

/*:108*//*110:*/
#line 241 "htex.ch"

static scaled x_over_n(scaled x,int n)
#line 2223 "btex.w"
{bool negative;
scaled x_over_n;
negative= false;
if(n==0)
{arith_error= true;x_over_n= 0;rem= x;
}
else{if(n<0)
{negate(x);negate(n);negative= true;
}
if(x>=0)
{x_over_n= x/n;rem= x%n;
}
else{x_over_n= -((-x)/n);rem= -((-x)%n);
}
}
if(negative)negate(rem);
return x_over_n;}

/*:110*//*111:*/
#line 248 "htex.ch"

static scaled xn_over_d(scaled x,int n,int d)
#line 2249 "btex.w"
{bool positive;
nonnegative_integer t,u,v;
scaled xn_over_d;
if(x>=0)positive= true;
else{negate(x);positive= false;
}
t= (x%0100000)*n;
u= (x/0100000)*n+(t/0100000);
v= (u%d)*0100000+(t%0100000);
if(u/d>=0100000)arith_error= true;
else u= 0100000*(u/d)+(v/d);
if(positive)
{xn_over_d= u;rem= v%d;
}
else{xn_over_d= -u;rem= -(v%d);
}
return xn_over_d;}

/*:111*//*112:*/
#line 255 "htex.ch"

halfword badness(scaled t,scaled s)
#line 2288 "btex.w"
{int r;

if(t==0)return 0;
else if(s<=0)return inf_bad;
else{if(t<=7230584)r= (t*297)/s;
else if(s>=1663497)r= t/(s/297);
else r= t;
if(r> 1290)return inf_bad;
else return(r*r*r+0400000)/01000000;
}
}

/*:112*//*125:*/
#line 300 "htex.ch"

static pointer get_avail(void)
#line 2584 "btex.w"
{pointer p;
p= avail;
if(p!=null)avail= link(avail);
#line 312 "htex.ch"
else{decr(hi_mem_min);p= hi_mem_min;
if(hi_mem_min<=lo_mem_max)
{
#line 2593 "btex.w"
overflow("main memory size",mem_max+1-mem_min);


}
}
link(p)= null;
#ifdef STAT
incr(dyn_used);
#endif

return p;
}

/*:125*//*128:*/
#line 321 "htex.ch"

static void flush_list(pointer p)

#line 2634 "btex.w"
{pointer q,r;
if(p!=null)
{r= p;
do{q= r;r= link(r);
#ifdef STAT
decr(dyn_used);
#endif
}while(!(r==null));
link(q)= avail;avail= p;
}
}

/*:128*//*130:*/
#line 337 "htex.ch"

pointer get_node(int s)
#line 2680 "btex.w"
{
pointer p;
pointer q;
int r;
int t;
restart:p= rover;
do{/*132:*/
#line 2729 "btex.w"

q= p+node_size(p);

while(is_empty(q))
{t= rlink(q);
if(q==rover)rover= t;
llink(t)= llink(q);rlink(llink(q))= t;
q= q+node_size(q);
}
r= q-s;
if(r> p+1)/*133:*/
#line 2744 "btex.w"

{node_size(p)= r-p;

rover= p;
goto found;
}

/*:133*/
#line 2739 "btex.w"
;
if(r==p)if(rlink(p)!=p)
/*134:*/
#line 2753 "btex.w"

{rover= rlink(p);t= llink(p);
llink(rover)= t;rlink(t)= rover;
goto found;
}

/*:134*/
#line 2741 "btex.w"
;
node_size(p)= q-p

/*:132*/
#line 2687 "btex.w"
;

p= rlink(p);
}while(!(p==rover));
if(s==010000000000)
{return max_halfword;
}
if(lo_mem_max+2<hi_mem_min)if(lo_mem_max+2<=mem_bot+max_halfword)
/*131:*/
#line 2714 "btex.w"

{if(hi_mem_min-lo_mem_max>=1998)t= lo_mem_max+1000;
else t= lo_mem_max+1+(hi_mem_min-lo_mem_max)/2;

p= llink(rover);q= lo_mem_max;rlink(p)= q;llink(rover)= q;
if(t> mem_bot+max_halfword)t= mem_bot+max_halfword;
rlink(q)= rover;llink(q)= p;link(q)= empty_flag;node_size(q)= t-lo_mem_max;
lo_mem_max= t;link(lo_mem_max)= null;info(lo_mem_max)= null;
rover= q;goto restart;
}

/*:131*/
#line 2695 "btex.w"
;
overflow("main memory size",mem_max+1-mem_min);


found:link(r)= null;
#ifdef STAT
var_used= var_used+s;
#endif

#line 344 "htex.ch"
leak_in(r,s);
return r;
#line 2705 "btex.w"
}

/*:130*//*135:*/
#line 353 "htex.ch"

static void free_node(pointer p,halfword s)

{pointer q;
leak_out(p,s);
store_map(p,0,0);
#line 2767 "btex.w"
node_size(p)= s;link(p)= empty_flag;
q= llink(rover);llink(p)= q;rlink(p)= rover;
llink(rover)= p;rlink(q)= p;
#ifdef STAT
var_used= var_used-s;
#endif

}

/*:135*//*141:*/
#line 370 "htex.ch"

pointer new_null_box(void)
#line 2913 "btex.w"
{pointer p;
p= get_node(box_node_size);type(p)= hlist_node;
subtype(p)= min_quarterword;
width(p)= 0;depth(p)= 0;height(p)= 0;shift_amount(p)= 0;list_ptr(p)= null;
glue_sign(p)= normal;glue_order(p)= normal;set_glue_ratio_zero(glue_set(p));
return p;
}

/*:141*//*144:*/
#line 377 "htex.ch"

pointer new_rule(void)
#line 2943 "btex.w"
{pointer p;
p= get_node(rule_node_size);type(p)= rule_node;
subtype(p)= 0;
width(p)= null_flag;depth(p)= null_flag;height(p)= null_flag;
return p;
}

/*:144*//*149:*/
#line 384 "htex.ch"

pointer new_ligature(quarterword f,quarterword c,pointer q)
#line 3012 "btex.w"
{pointer p;
p= get_node(small_node_size);type(p)= ligature_node;
font(lig_char(p))= f;character(lig_char(p))= c;lig_ptr(p)= q;
subtype(p)= 0;return p;
}

#line 3023 "btex.w"

/*:149*//*150:*/
#line 409 "htex.ch"

pointer new_disc(void)
#line 3047 "btex.w"
{pointer p;
p= get_node(small_node_size);type(p)= disc_node;
#line 416 "htex.ch"
subtype(p)= 0;pre_break(p)= null;post_break(p)= null;
#line 3050 "btex.w"
return p;
}

/*:150*//*152:*/
#line 422 "htex.ch"

pointer new_math(scaled w,small_number s)
#line 3081 "btex.w"
{pointer p;
p= get_node(small_node_size);type(p)= math_node;
subtype(p)= s;width(p)= w;return p;
}

/*:152*//*156:*/
#line 429 "htex.ch"

pointer new_spec(pointer p)
#line 3163 "btex.w"
{pointer q;
q= get_node(glue_spec_size);
mem[q]= mem[p];glue_ref_count(q)= null;
width(q)= width(p);stretch(q)= stretch(p);shrink(q)= shrink(p);
return q;
}

/*:156*//*158:*/
#line 436 "htex.ch"

pointer new_glue(pointer q)
#line 3189 "btex.w"
{pointer p;
p= get_node(small_node_size);type(p)= glue_node;subtype(p)= normal;
leader_ptr(p)= null;glue_ptr(p)= q;incr(glue_ref_count(q));
return p;
}

/*:158*//*161:*/
#line 443 "htex.ch"

pointer new_kern(scaled w)
#line 3227 "btex.w"
{pointer p;
p= get_node(small_node_size);type(p)= kern_node;
subtype(p)= normal;
width(p)= w;
return p;
}

/*:161*//*163:*/
#line 450 "htex.ch"

pointer new_penalty(int m)
#line 3250 "btex.w"
{pointer p;
p= get_node(small_node_size);type(p)= penalty_node;
subtype(p)= 0;
penalty(p)= m;return p;
}

/*:163*//*169:*/
#line 470 "htex.ch"

void mem_init(void)
{int k;
/*170:*/
#line 476 "htex.ch"

#line 3334 "btex.w"
for(k= mem_bot+1;k<=lo_mem_stat_max;k++)mem[k].sc= 0;


k= mem_bot;while(k<=lo_mem_stat_max)

{glue_ref_count(k)= null+1;
stretch_order(k)= normal;shrink_order(k)= normal;
k= k+glue_spec_size;
}
#line 3348 "btex.w"
rover= lo_mem_stat_max+1;
link(rover)= empty_flag;
node_size(rover)= 1000;
llink(rover)= rover;rlink(rover)= rover;
lo_mem_max= rover+1000;link(lo_mem_max)= null;info(lo_mem_max)= null;
for(k= hi_mem_stat_min;k<=mem_top;k++)
mem[k]= mem[lo_mem_max];
/*804:*/
#line 15652 "btex.w"

info(omit_template)= end_template_token;

/*:804*//*811:*/
#line 15759 "btex.w"

link(end_span)= max_quarterword+1;info(end_span)= null;

/*:811*//*835:*/
#line 16221 "btex.w"

type(last_active)= hyphenated;line_number(last_active)= max_halfword;
subtype(last_active)= 0;

/*:835*//*1006:*/
#line 19309 "btex.w"

subtype(page_ins_head)= qi(255);
type(page_ins_head)= split_up;link(page_ins_head)= page_ins_head;

/*:1006*//*1013:*/
#line 19466 "btex.w"

type(page_head)= glue_node;subtype(page_head)= normal;

/*:1013*/
#line 3355 "btex.w"
;
#line 491 "htex.ch"
avail= null;
#line 3357 "btex.w"
hi_mem_min= hi_mem_stat_min;
var_used= lo_mem_stat_max+1-mem_bot;dyn_used= hi_mem_stat_usage;


/*:170*/
#line 473 "htex.ch"

}

/*:169*//*209:*/
#line 643 "htex.ch"

static void delete_token_ref(pointer p)

#line 3894 "btex.w"
{if(token_ref_count(p)==null)flush_list(p);
else decr(token_ref_count(p));
}

/*:209*//*210:*/
#line 652 "htex.ch"

void delete_glue_ref(pointer p)
fast_delete_glue_ref(p)
static void delete_xdimen_ref(pointer p)
{if(xdimen_ref_count(p)==null)free_node(p,xdimen_node_size);
else decr(xdimen_ref_count(p));
}
#line 3908 "btex.w"

/*:210*//*211:*/
#line 664 "htex.ch"

void flush_node_list(pointer p)
#line 3915 "btex.w"
{
pointer q;
while(p!=null)

{q= link(p);
if(is_char_node(p))free_avail(p)
else{switch(type(p)){
case hlist_node:case vlist_node:
case unset_node:{flush_node_list(list_ptr(p));
free_node(p,box_node_size);goto done;
}
case rule_node:{free_node(p,rule_node_size);goto done;
}
case ins_node:{flush_node_list(ins_ptr(p));
delete_glue_ref(split_top_ptr(p));
free_node(p,ins_node_size);goto done;
}
case whatsit_node:/*1387:*/
#line 24898 "btex.w"

{switch(subtype(p)){
#line 2527 "htex.ch"
case close_node:case language_node:free_node(p,small_node_size);break;
case par_node:
if(par_type(p)==glue_type)fast_delete_glue_ref(par_value(p).i);
free_node(p,par_node_size);break;
case graf_node:
delete_xdimen_ref(graf_extent(p));
flush_node_list(graf_params(p));
flush_node_list(graf_list(p));
free_node(p,graf_node_size);break;
case disp_node:
flush_node_list(display_eqno(p));
flush_node_list(display_formula(p));
flush_node_list(display_params(p));
free_node(p,disp_node_size);break;
case baseline_node:
free_node(p,baseline_node_size);break;
case hpack_node:case vpack_node:
delete_xdimen_ref(pack_extent(p));
flush_node_list(list_ptr(p));
free_node(p,pack_node_size);break;
case hset_node:case vset_node:
delete_xdimen_ref(set_extent(p));
flush_node_list(list_ptr(p));
free_node(p,set_node_size);break;
case image_node:
free_node(p,image_node_size);break;
case align_node:
delete_xdimen_ref(align_extent(p));
flush_node_list(align_preamble(p));
flush_node_list(align_list(p));
free_node(p,align_node_size);break;
case setpage_node:
delete_glue_ref(setpage_topskip(p));
delete_xdimen_ref(setpage_height(p));
delete_xdimen_ref(setpage_width(p));
flush_node_list(setpage_list(p));
flush_node_list(setpage_streams(p));
free_node(p,setpage_node_size);break;
case setstream_node:
delete_xdimen_ref(setstream_max(p));
delete_xdimen_ref(setstream_width(p));
delete_glue_ref(setstream_topskip(p));
delete_glue_ref(setstream_height(p));
flush_node_list(setstream_before(p));
flush_node_list(setstream_after(p));
free_node(p,setstream_node_size);break;
case ignore_node:
flush_node_list(ignore_list(p));
free_node(p,ignore_node_size);break;
case start_link_node:
if(label_has_name(p))delete_token_ref(label_ptr(p));
free_node(p,link_node_size);break;
case end_link_node:
free_node(p,link_node_size);break;
case label_node:
if(label_has_name(p))delete_token_ref(label_ptr(p));
free_node(p,label_node_size);break;
case outline_node:
if(label_has_name(p))delete_token_ref(label_ptr(p));
flush_node_list(outline_ptr(p));
free_node(p,outline_node_size);break;
case stream_node:
free_node(p,stream_node_size);break;
case xdimen_node:
free_node(p,xdimen_node_size);
#line 24905 "btex.w"
default:confusion("ext3");

}
goto done;
}

#line 2597 "htex.ch"
/*:1387*/
#line 3932 "btex.w"

case glue_node:{fast_delete_glue_ref(glue_ptr(p));
if(leader_ptr(p)!=null)flush_node_list(leader_ptr(p));
}break;
case kern_node:case math_node:case penalty_node:do_nothing;break;
case ligature_node:flush_node_list(lig_ptr(p));break;
#line 3939 "btex.w"
case disc_node:{flush_node_list(pre_break(p));
flush_node_list(post_break(p));
}break;
case adjust_node:flush_node_list(adjust_ptr(p));break;
#line 677 "htex.ch"
default:QUIT("Confusion while flushing node list");
#line 3945 "btex.w"

}
free_node(p,small_node_size);
done:;}
p= q;
}
}

/*:211*//*213:*/
#line 691 "htex.ch"

pointer copy_node_list(pointer p)

#line 3979 "btex.w"
{pointer h;
pointer q;
pointer r;
int words;
h= get_avail();q= h;
while(p!=null)
{/*214:*/
#line 3992 "btex.w"

words= 1;
if(is_char_node(p))r= get_avail();
else/*215:*/
#line 4001 "btex.w"

switch(type(p)){
case hlist_node:case vlist_node:case unset_node:{r= get_node(box_node_size);
mem[r+6]= mem[p+6];mem[r+5]= mem[p+5];
list_ptr(r)= copy_node_list(list_ptr(p));
words= 5;
}break;
case rule_node:{r= get_node(rule_node_size);words= rule_node_size;
}break;
case ins_node:{r= get_node(ins_node_size);mem[r+4]= mem[p+4];
add_glue_ref(split_top_ptr(p));
ins_ptr(r)= copy_node_list(ins_ptr(p));
words= ins_node_size-1;
}break;
case whatsit_node:/*1386:*/
#line 24884 "btex.w"

switch(subtype(p)){
case open_node:{r= get_node(open_node_size);words= open_node_size;
}break;
case write_node:case special_node:{r= get_node(write_node_size);
add_token_ref(write_tokens(p));words= write_node_size;
}break;
case close_node:case language_node:{r= get_node(small_node_size);
words= small_node_size;
}break;
#line 2413 "htex.ch"
case par_node:
{r= get_node(par_node_size);
if(par_type(p)==glue_type)add_glue_ref(par_value(p).i);
words= par_node_size;
}break;
case graf_node:
{r= get_node(graf_node_size);
add_xdimen_ref(graf_extent(p));
graf_params(r)= copy_node_list(graf_params(p));
graf_list(r)= copy_node_list(graf_list(p));
words= graf_node_size-1;
}break;
case disp_node:
{r= get_node(disp_node_size);
display_left(r)= display_left(p);
display_no_bs(r)= display_no_bs(p);
display_eqno(r)= copy_node_list(display_eqno(p));
display_formula(r)= copy_node_list(display_formula(p));
display_params(r)= copy_node_list(display_params(p));
words= disp_node_size-2;
}break;
case baseline_node:
{r= get_node(baseline_node_size);
words= baseline_node_size;
}break;
case hpack_node:case vpack_node:
{r= get_node(pack_node_size);
pack_m(r)= pack_m(p);
list_ptr(r)= copy_node_list(list_ptr(p));
add_xdimen_ref(pack_extent(p));
pack_limit(r)= pack_limit(p);
words= pack_node_size-3;
}break;
case hset_node:case vset_node:
{r= get_node(set_node_size);
mem[r+8]= mem[p+8];mem[r+7]= mem[p+7];mem[r+6]= mem[p+6];mem[r+5]= mem[p+5];
add_xdimen_ref(set_extent(p));
list_ptr(r)= copy_node_list(list_ptr(p));
words= 5;
}break;
case image_node:
r= get_node(image_node_size);
words= image_node_size;
break;
case align_node:
{r= get_node(align_node_size);
align_preamble(r)= copy_node_list(align_preamble(p));
align_list(r)= copy_node_list(align_list(p));
add_xdimen_ref(align_extent(p));
words= align_node_size-1;
}break;
case setpage_node:
{r= get_node(setpage_node_size);
add_glue_ref(setpage_topskip(p));
add_xdimen_ref(setpage_height(p));
add_xdimen_ref(setpage_width(p));
setpage_list(r)= copy_node_list(setpage_list(p));
setpage_streams(r)= copy_node_list(setpage_streams(p));
words= setpage_node_size-1;
}break;
case setstream_node:
{r= get_node(setstream_node_size);
add_xdimen_ref(setstream_max(p));
add_xdimen_ref(setstream_width(p));
add_glue_ref(setstream_topskip(p));
add_glue_ref(setstream_height(p));
setstream_before(r)= copy_node_list(setstream_before(p));
setstream_after(r)= copy_node_list(setstream_after(p));
words= setstream_node_size-1;
}break;
case ignore_node:
r= get_node(ignore_node_size);
ignore_info(r)= ignore_info(p);
ignore_list(r)= copy_node_list(ignore_list(p));
words= ignore_node_size-1;
break;
case start_link_node:
r= get_node(link_node_size);
if(label_has_name(p))add_token_ref(label_ptr(p));
words= link_node_size;
break;
case end_link_node:
r= get_node(link_node_size);
words= link_node_size;
break;
case label_node:
r= get_node(label_node_size);
if(label_has_name(p))add_token_ref(label_ptr(p));
words= label_node_size;
break;
case outline_node:
r= get_node(outline_node_size);
if(label_has_name(p))add_token_ref(label_ptr(p));
outline_ptr(r)= copy_node_list(outline_ptr(p));
words= outline_node_size-1;
break;
case stream_node:
r= get_node(stream_node_size);
words= stream_node_size;
break;
case xdimen_node:
r= get_node(xdimen_node_size);
words= xdimen_node_size;
break;
default:confusion("ext2");
#line 24895 "btex.w"

}

/*:1386*/
#line 4016 "btex.w"
break;
case glue_node:{r= get_node(small_node_size);add_glue_ref(glue_ptr(p));
glue_ptr(r)= glue_ptr(p);leader_ptr(r)= copy_node_list(leader_ptr(p));
}break;
case kern_node:case math_node:case penalty_node:{r= get_node(small_node_size);
words= small_node_size;
}break;
case ligature_node:{r= get_node(small_node_size);
mem[lig_char(r)]= mem[lig_char(p)];
lig_ptr(r)= copy_node_list(lig_ptr(p));
}break;
case disc_node:{r= get_node(small_node_size);
pre_break(r)= copy_node_list(pre_break(p));
post_break(r)= copy_node_list(post_break(p));
}break;
case mark_node:{r= get_node(small_node_size);add_token_ref(mark_ptr(p));
words= small_node_size;
}break;
case adjust_node:{r= get_node(small_node_size);
adjust_ptr(r)= copy_node_list(adjust_ptr(p));
}break;
default:confusion("copying");

}

/*:215*/
#line 3996 "btex.w"
;
while(words> 0)
{decr(words);mem[r+words]= mem[p+words];
}

/*:214*/
#line 3985 "btex.w"
;
link(q)= r;q= r;p= link(p);
}
link(q)= null;q= link(h);free_avail(h);
return q;
}

/*:213*//*224:*/
#line 743 "htex.ch"


/*222:*/
#line 729 "htex.ch"

static list_state_record nest[nest_size+1];
int nest_ptr;
static int max_nest_stack;
list_state_record cur_list;
#line 4340 "btex.w"

/*:222*/
#line 745 "htex.ch"


void list_init(void)
{
nest_ptr= 0;max_nest_stack= 0;
memset(&cur_list,0,sizeof(cur_list));
mode= vmode;head= contrib_head;tail= contrib_head;
prev_height= prev_depth= ignore_depth;
}
/*:224*//*226:*/
#line 762 "htex.ch"

void push_nest(void)
#line 4366 "btex.w"
{if(nest_ptr> max_nest_stack)
{max_nest_stack= nest_ptr;
if(nest_ptr==nest_size)overflow("semantic nest size",nest_size);

}
nest[nest_ptr]= cur_list;
#line 769 "htex.ch"
incr(nest_ptr);head= get_avail();tail= head;prev_graf= 0;
cur_list.bs_pos= NULL;cur_bs= baseline_skip;cur_ls= line_skip;cur_lsl= line_skip_limit;
#line 4373 "btex.w"
}

/*:226*//*227:*/
#line 776 "htex.ch"

void pop_nest(void)
#line 4381 "btex.w"
{free_avail(head);decr(nest_ptr);cur_list= nest[nest_ptr];
}

/*:227*//*560:*/
#line 953 "htex.ch"

memory_word font_info[font_mem_size+1];

static font_index fmem_ptr= 0;

void hclear_fonts(void)
{fmem_ptr= 0;
}
#line 10766 "btex.w"
static internal_font_number font_ptr;
static four_quarters font_check0[font_max-font_base+1],
*const font_check= font_check0-font_base;
scaled font_size0[font_max-font_base+1],
*const font_size= font_size0-font_base;
static scaled font_dsize0[font_max-font_base+1],
*const font_dsize= font_dsize0-font_base;
static font_index font_params0[font_max-font_base+1],
*const font_params= font_params0-font_base;

#line 969 "htex.ch"
char*font_name0[font_max-font_base+1],
**const font_name= font_name0-font_base;
#line 10780 "btex.w"
static eight_bits font_bc0[font_max-font_base+1],
*const font_bc= font_bc0-font_base;

static eight_bits font_ec0[font_max-font_base+1],
*const font_ec= font_ec0-font_base;

static pointer font_glue0[font_max-font_base+1],
*const font_glue= font_glue0-font_base;

static bool font_used0[font_max-font_base+1],
*const font_used= font_used0-font_base;

static int hyphen_char0[font_max-font_base+1],
*const hyphen_char= hyphen_char0-font_base;

static int skew_char0[font_max-font_base+1],
*const skew_char= skew_char0-font_base;

static font_index bchar_label0[font_max-font_base+1],
*const bchar_label= bchar_label0-font_base;


static int16_t font_bchar0[font_max-font_base+1],
*const font_bchar= font_bchar0-font_base;

static int16_t font_false_bchar0[font_max-font_base+1],
*const font_false_bchar= font_false_bchar0-font_base;


/*:560*//*561:*/
#line 978 "htex.ch"

int char_base0[font_max-font_base+1],
*const char_base= char_base0-font_base;
#line 10821 "btex.w"

int width_base0[font_max-font_base+1],
*const width_base= width_base0-font_base;

static int height_base0[font_max-font_base+1],
*const height_base= height_base0-font_base;

static int depth_base0[font_max-font_base+1],
*const depth_base= depth_base0-font_base;

static int italic_base0[font_max-font_base+1],
*const italic_base= italic_base0-font_base;

static int lig_kern_base0[font_max-font_base+1],
*const lig_kern_base= lig_kern_base0-font_base;

static int kern_base0[font_max-font_base+1],
*const kern_base= kern_base0-font_base;

static int exten_base0[font_max-font_base+1],
*const exten_base= exten_base0-font_base;

static int param_base0[font_max-font_base+1],
*const param_base= param_base0-font_base;


/*:561*//*571:*/
#line 987 "htex.ch"

void read_font_info(int f,char*nom,scaled s)
#line 10975 "btex.w"
{
int k;
bool file_opened;
halfword lf,lh,bc,ec,nw,nh,nd,ni,nl,nk,ne,np;

#line 10982 "btex.w"
eight_bits a,b,c,d;
four_quarters qw;scaled sw;
int bch_label;
int bchar;
scaled z;
int alpha;int beta;

#line 10990 "btex.w"
/*573:*/
#line 11026 "btex.w"

/*574:*/
#line 1019 "htex.ch"

file_opened= true
#line 11044 "btex.w"

/*:574*/
#line 11027 "btex.w"
;
/*576:*/
#line 11066 "btex.w"

{read_sixteen(lf);
fget;read_sixteen(lh);
fget;read_sixteen(bc);
fget;read_sixteen(ec);
if((bc> ec+1)||(ec> 255))abort;
if(bc> 255)
{bc= 1;ec= 0;
}
fget;read_sixteen(nw);
fget;read_sixteen(nh);
fget;read_sixteen(nd);
fget;read_sixteen(ni);
fget;read_sixteen(nl);
fget;read_sixteen(nk);
fget;read_sixteen(ne);
fget;read_sixteen(np);
if(lf!=6+lh+(ec-bc+1)+nw+nh+nd+ni+nl+nk+ne+np)abort;
if((nw==0)||(nh==0)||(nd==0)||(ni==0))abort;
}

/*:576*/
#line 11028 "btex.w"
;
/*577:*/
#line 11093 "btex.w"

lf= lf-6-lh;
if(np<7)lf= lf+7-np;
if((font_ptr==font_max)||(fmem_ptr+lf> font_mem_size))
#line 1035 "htex.ch"
QUIT("Not enough room left for font %s\n",nom);
#line 11099 "btex.w"
char_base[f]= fmem_ptr-bc;
width_base[f]= char_base[f]+ec+1;
height_base[f]= width_base[f]+nw;
depth_base[f]= height_base[f]+nh;
italic_base[f]= depth_base[f]+nd;
lig_kern_base[f]= italic_base[f]+ni;
kern_base[f]= lig_kern_base[f]+nl-kern_base_offset;
exten_base[f]= kern_base[f]+kern_base_offset+nk;
param_base[f]= exten_base[f]+ne

/*:577*/
#line 11029 "btex.w"
;
/*579:*/
#line 11122 "btex.w"

{if(lh<2)abort;
store_four_quarters(font_check[f]);
fget;read_sixteen(z);
fget;z= z*0400+fbyte;fget;z= (z*020)+(fbyte/020);
if(z<unity)abort;
while(lh> 2)
{fget;fget;fget;fget;decr(lh);
}
font_dsize[f]= z;
if(s!=-1000)
if(s>=0)z= s;
else z= xn_over_d(z,-s,1000);
font_size[f]= z;
}

/*:579*/
#line 11030 "btex.w"
;
/*580:*/
#line 11138 "btex.w"

for(k= fmem_ptr;k<=width_base[f]-1;k++)
{store_four_quarters(font_info[k].qqqq);
if((a>=nw)||(b/020>=nh)||(b%020>=nd)||
(c/4>=ni))abort;
switch(c%4){
case lig_tag:if(d>=nl)abort;break;
case ext_tag:if(d>=ne)abort;break;
case list_tag:/*581:*/
#line 11159 "btex.w"

{check_byte_range(d);
while(d<current_character_being_worked_on)
{qw= char_info(f,d);

if(char_tag(qw)!=list_tag)goto not_found;
d= qo(rem_byte(qw));
}
if(d==current_character_being_worked_on)abort;
not_found:;}

/*:581*/
#line 11146 "btex.w"
break;
default:do_nothing;
}
}

/*:580*/
#line 11031 "btex.w"
;
/*582:*/
#line 11194 "btex.w"

{/*583:*/
#line 11204 "btex.w"

{alpha= 16;
while(z>=040000000)
{z= z/2;alpha= alpha+alpha;
}
beta= 256/alpha;alpha= alpha*z;
}

/*:583*/
#line 11195 "btex.w"
;
for(k= width_base[f];k<=lig_kern_base[f]-1;k++)
store_scaled(font_info[k].sc);
if(font_info[width_base[f]].sc!=0)abort;
if(font_info[height_base[f]].sc!=0)abort;
if(font_info[depth_base[f]].sc!=0)abort;
if(font_info[italic_base[f]].sc!=0)abort;
}

/*:582*/
#line 11032 "btex.w"
;
/*584:*/
#line 11218 "btex.w"

bch_label= 077777;bchar= 256;
if(nl> 0)
{for(k= lig_kern_base[f];k<=kern_base[f]+kern_base_offset-1;k++)
{store_four_quarters(font_info[k].qqqq);
if(a> 128)
{if(256*c+d>=nl)abort;
if(a==255)if(k==lig_kern_base[f])bchar= b;
}
else{if(b!=bchar)check_existence(b);
if(c<128)check_existence(d)
else if(256*(c-128)+d>=nk)abort;
if(a<128)if(k-lig_kern_base[f]+a+1>=nl)abort;
}
}
if(a==255)bch_label= 256*c+d;
}
for(k= kern_base[f]+kern_base_offset;k<=exten_base[f]-1;k++)
store_scaled(font_info[k].sc);

/*:584*/
#line 11033 "btex.w"
;
/*585:*/
#line 11238 "btex.w"

for(k= exten_base[f];k<=param_base[f]-1;k++)
{store_four_quarters(font_info[k].qqqq);
if(a!=0)check_existence(a);
if(b!=0)check_existence(b);
if(c!=0)check_existence(c);
check_existence(d);
}

/*:585*/
#line 11034 "btex.w"
;
/*586:*/
#line 11250 "btex.w"

{for(k= 1;k<=np;k++)
if(k==1)
{fget;sw= fbyte;if(sw> 127)sw= sw-256;
fget;sw= sw*0400+fbyte;fget;sw= sw*0400+fbyte;
fget;font_info[param_base[f]].sc= 
(sw*020)+(fbyte/020);
}
else store_scaled(font_info[param_base[f]+k-1].sc);
#line 1041 "htex.ch"
if(hpos>=hend)abort;
#line 11260 "btex.w"
for(k= np+1;k<=7;k++)font_info[param_base[f]+k-1].sc= 0;
}

/*:586*/
#line 11035 "btex.w"
;
/*587:*/
#line 11270 "btex.w"

if(np>=7)font_params[f]= np;else font_params[f]= 7;
#line 1047 "htex.ch"
hyphen_char[f]= skew_char[f]= -1;
#line 11273 "btex.w"
if(bch_label<nl)bchar_label[f]= bch_label+lig_kern_base[f];
else bchar_label[f]= non_address;
font_bchar[f]= qi(bchar);
font_false_bchar[f]= qi(bchar);
if(bchar<=ec)if(bchar>=bc)
{qw= char_info(f,bchar);
if(char_exists(qw))font_false_bchar[f]= non_char;
}
font_name[f]= nom;
#line 11283 "btex.w"
font_bc[f]= bc;font_ec[f]= ec;font_glue[f]= null;
adjust(char_base);adjust(width_base);adjust(lig_kern_base);
adjust(kern_base);adjust(exten_base);
decr(param_base[f]);
#line 1058 "htex.ch"
fmem_ptr= fmem_ptr+lf;goto done
#line 11288 "btex.w"

/*:587*/
#line 11036 "btex.w"


#line 1019 "htex.ch"
/*:573*/
#line 10992 "btex.w"
;
#line 1007 "htex.ch"
bad_tfm:QUIT("Bad tfm file: %s\n",nom);
done:;
#line 10996 "btex.w"
}

/*:571*//*593:*/
#line 1074 "htex.ch"

pointer new_character(internal_font_number f,eight_bits c)
{pointer p;
#ifdef DEBUG
if(font_bc[f]> c||font_ec[f]<c||!char_exists(char_info(f,qi(c))))
DBG(DBGFONT,"Warning: Character 0x%0X in font %d does not exist\n",c,f);
#endif
p= get_avail();font(p)= f;character(p)= qi(c);
return p;
}
#line 11385 "btex.w"

/*:593*//*662:*/
#line 1110 "htex.ch"

pointer hpack(pointer p,scaled w,small_number m)
#line 12976 "btex.w"
{
pointer r;
pointer q;
scaled h,d,x;
scaled s;
pointer g;
glue_ord o;
internal_font_number f;
four_quarters i;
eight_bits hd;
#line 1117 "htex.ch"
r= get_node(box_node_size);type(r)= hlist_node;
#line 12987 "btex.w"
subtype(r)= min_quarterword;shift_amount(r)= 0;
q= r+list_offset;link(q)= p;
h= 0;/*663:*/
#line 13002 "btex.w"

d= 0;x= 0;
total_stretch[normal]= 0;total_shrink[normal]= 0;
total_stretch[fil]= 0;total_shrink[fil]= 0;
total_stretch[fill]= 0;total_shrink[fill]= 0;
total_stretch[filll]= 0;total_shrink[filll]= 0

/*:663*/
#line 12989 "btex.w"
;
while(p!=null)/*664:*/
#line 13009 "btex.w"


{reswitch:while(is_char_node(p))
/*667:*/
#line 13054 "btex.w"

{f= font(p);i= char_info(f,character(p));hd= height_depth(i);
x= x+char_width(f,i);
s= char_height(f,hd);if(s> h)h= s;
s= char_depth(f,hd);if(s> d)d= s;
p= link(p);
}

/*:667*/
#line 13013 "btex.w"
;
if(p!=null)
{switch(type(p)){
case hlist_node:case vlist_node:case rule_node:
case unset_node:
/*666:*/
#line 13042 "btex.w"

{x= x+width(p);
if(type(p)>=rule_node)s= 0;else s= shift_amount(p);
if(height(p)-s> h)h= height(p)-s;
if(depth(p)+s> d)d= depth(p)+s;
}

/*:666*/
#line 13019 "btex.w"
break;
case ins_node:case mark_node:case adjust_node:if(adjust_tail!=null)
/*668:*/
#line 13069 "btex.w"

{while(link(q)!=p)q= link(q);
if(type(p)==adjust_node)
{link(adjust_tail)= adjust_ptr(p);
while(link(adjust_tail)!=null)adjust_tail= link(adjust_tail);
p= link(p);free_node(link(q),small_node_size);
}
else{link(adjust_tail)= p;adjust_tail= p;p= link(p);
}
link(q)= p;p= q;
}

/*:668*/
#line 13021 "btex.w"
break;
case whatsit_node:/*1389:*/
#line 2610 "htex.ch"

if(subtype(p)==image_node)
{glue_ord o;
if(image_height(p)> h)h= image_height(p);
x= x+image_width(p);
o= image_stretch_order(p);total_stretch[o]= total_stretch[o]+image_stretch(p);
o= image_shrink_order(p);total_shrink[o]= total_shrink[o]+image_shrink(p);
}
#line 24914 "btex.w"

/*:1389*/
#line 13022 "btex.w"
;break;
case glue_node:/*669:*/
#line 13081 "btex.w"

{g= glue_ptr(p);x= x+width(g);
o= stretch_order(g);total_stretch[o]= total_stretch[o]+stretch(g);
o= shrink_order(g);total_shrink[o]= total_shrink[o]+shrink(g);
if(subtype(p)>=a_leaders)
{g= leader_ptr(p);
if(height(g)> h)h= height(g);
if(depth(g)> d)d= depth(g);
}
}

/*:669*/
#line 13023 "btex.w"
break;
case kern_node:case math_node:x= x+width(p);break;
case ligature_node:/*665:*/
#line 13033 "btex.w"

{mem[lig_trick]= mem[lig_char(p)];link(lig_trick)= link(p);
p= lig_trick;goto reswitch;
}

/*:665*/
#line 13025 "btex.w"

default:do_nothing;
}
p= link(p);
}
}


/*:664*/
#line 12992 "btex.w"
;
if(adjust_tail!=null)link(adjust_tail)= null;
height(r)= h;depth(r)= d;
/*670:*/
#line 13095 "btex.w"

if(m==additional)w= x+w;
width(r)= w;x= w-x;
if(x==0)
{glue_sign(r)= normal;glue_order(r)= normal;
set_glue_ratio_zero(glue_set(r));
goto end;
}
else if(x> 0)/*671:*/
#line 13108 "btex.w"

{/*672:*/
#line 13122 "btex.w"

if(total_stretch[filll]!=0)o= filll;
else if(total_stretch[fill]!=0)o= fill;
else if(total_stretch[fil]!=0)o= fil;
else o= normal

/*:672*/
#line 13109 "btex.w"
;
glue_order(r)= o;glue_sign(r)= stretching;
if(total_stretch[o]!=0)glue_set(r)= unfloat(x/(double)total_stretch[o]);

else{glue_sign(r)= normal;
set_glue_ratio_zero(glue_set(r));
}
#line 13119 "btex.w"
goto end;
}

/*:671*/
#line 13104 "btex.w"

else/*677:*/
#line 13167 "btex.w"

{/*678:*/
#line 13187 "btex.w"

if(total_shrink[filll]!=0)o= filll;
else if(total_shrink[fill]!=0)o= fill;
else if(total_shrink[fil]!=0)o= fil;
else o= normal

/*:678*/
#line 13168 "btex.w"
;
glue_order(r)= o;glue_sign(r)= shrinking;
if(total_shrink[o]!=0)glue_set(r)= unfloat((-x)/(double)total_shrink[o]);

else{glue_sign(r)= normal;
set_glue_ratio_zero(glue_set(r));
}
if((total_shrink[o]<-x)&&(o==normal)&&(list_ptr(r)!=null))
#line 1143 "htex.ch"
set_glue_ratio_one(glue_set(r));
#line 13184 "btex.w"
goto end;
}

/*:677*/
#line 13106 "btex.w"


/*:670*/
#line 12996 "btex.w"
;
#line 12999 "btex.w"
end:return r;
}

/*:662*//*682:*/
#line 1153 "htex.ch"

pointer vpackage(pointer p,scaled h,small_number m,scaled l)
#line 13226 "btex.w"
{
pointer r;
scaled w,d,x;
scaled s;
pointer g;
glue_ord o;
#line 1160 "htex.ch"
r= get_node(box_node_size);type(r)= vlist_node;
#line 13233 "btex.w"
subtype(r)= min_quarterword;shift_amount(r)= 0;
list_ptr(r)= p;
w= 0;/*663:*/
#line 13002 "btex.w"

d= 0;x= 0;
total_stretch[normal]= 0;total_shrink[normal]= 0;
total_stretch[fil]= 0;total_shrink[fil]= 0;
total_stretch[fill]= 0;total_shrink[fill]= 0;
total_stretch[filll]= 0;total_shrink[filll]= 0

/*:663*/
#line 13235 "btex.w"
;
while(p!=null)/*683:*/
#line 13250 "btex.w"

{if(is_char_node(p))confusion("vpack");

else switch(type(p)){
case hlist_node:case vlist_node:case rule_node:
case unset_node:
/*684:*/
#line 13267 "btex.w"

{x= x+d+height(p);d= depth(p);
if(type(p)>=rule_node)s= 0;else s= shift_amount(p);
if(width(p)+s> w)w= width(p)+s;
}

/*:684*/
#line 13257 "btex.w"
break;
case whatsit_node:/*1388:*/
#line 2597 "htex.ch"

if(subtype(p)==image_node)
{glue_ord o;
if(image_width(p)> w)w= image_width(p);
x= x+d+image_height(p);d= 0;
o= image_stretch_order(p);total_stretch[o]= total_stretch[o]+image_stretch(p);
o= image_shrink_order(p);total_shrink[o]= total_shrink[o]+image_shrink(p);
}
#line 24912 "btex.w"

#line 2610 "htex.ch"
/*:1388*/
#line 13258 "btex.w"
;break;
case glue_node:/*685:*/
#line 13273 "btex.w"

{x= x+d;d= 0;
g= glue_ptr(p);x= x+width(g);
o= stretch_order(g);total_stretch[o]= total_stretch[o]+stretch(g);
o= shrink_order(g);total_shrink[o]= total_shrink[o]+shrink(g);
if(subtype(p)>=a_leaders)
{g= leader_ptr(p);
if(width(g)> w)w= width(g);
}
}

/*:685*/
#line 13259 "btex.w"
break;
case kern_node:{x= x+d+width(p);d= 0;
}break;
default:do_nothing;
}
p= link(p);
}

/*:683*/
#line 13237 "btex.w"
;
width(r)= w;
if(d> l)
{x= x+d-l;depth(r)= l;
}
else depth(r)= d;
/*686:*/
#line 13287 "btex.w"

if(m==additional)h= x+h;
height(r)= h;x= h-x;
if(x==0)
{glue_sign(r)= normal;glue_order(r)= normal;
set_glue_ratio_zero(glue_set(r));
goto end;
}
else if(x> 0)/*687:*/
#line 13300 "btex.w"

{/*672:*/
#line 13122 "btex.w"

if(total_stretch[filll]!=0)o= filll;
else if(total_stretch[fill]!=0)o= fill;
else if(total_stretch[fil]!=0)o= fil;
else o= normal

/*:672*/
#line 13301 "btex.w"
;
glue_order(r)= o;glue_sign(r)= stretching;
if(total_stretch[o]!=0)glue_set(r)= unfloat(x/(double)total_stretch[o]);

else{glue_sign(r)= normal;
set_glue_ratio_zero(glue_set(r));
}
#line 13311 "btex.w"
goto end;
}

/*:687*/
#line 13296 "btex.w"

else/*690:*/
#line 13339 "btex.w"

{/*678:*/
#line 13187 "btex.w"

if(total_shrink[filll]!=0)o= filll;
else if(total_shrink[fill]!=0)o= fill;
else if(total_shrink[fil]!=0)o= fil;
else o= normal

/*:678*/
#line 13340 "btex.w"
;
glue_order(r)= o;glue_sign(r)= shrinking;
if(total_shrink[o]!=0)glue_set(r)= unfloat((-x)/(double)total_shrink[o]);

else{glue_sign(r)= normal;
set_glue_ratio_zero(glue_set(r));
}
if((total_shrink[o]<-x)&&(o==normal)&&(list_ptr(r)!=null))
#line 1186 "htex.ch"
set_glue_ratio_one(glue_set(r));
#line 13356 "btex.w"
goto end;
}

/*:690*/
#line 13298 "btex.w"


/*:686*/
#line 13244 "btex.w"
;
#line 13247 "btex.w"
end:return r;
}

/*:682*//*830:*/
#line 1229 "htex.ch"


/*693:*/
#line 1192 "htex.ch"

extern pointer happend_to_vlist(pointer b);
void append_to_vlist(pointer b,uint32_t offset)
#line 13380 "btex.w"
{scaled d;
pointer p;
if(prev_depth> ignore_depth)
{d= width(baseline_skip)-prev_depth-height(b);
#line 1203 "htex.ch"
if(d<line_skip_limit)p= new_glue(line_skip);
else{temp_ptr= new_spec(baseline_skip);
p= new_glue(temp_ptr);glue_ref_count(temp_ptr)= null;
width(temp_ptr)= d;
}
store_map(p,node_pos,offset);
#line 13388 "btex.w"
link(tail)= p;tail= p;
}
link(tail)= b;tail= b;prev_depth= depth(b);
}

/*:693*//*844:*/
#line 16351 "btex.w"

static pointer finite_shrink(pointer p)
{pointer q;
#line 1309 "htex.ch"
QUIT("Infinite glue shrinkage found in a paragraph");
#line 16373 "btex.w"
q= new_spec(p);shrink_order(q)= normal;
delete_glue_ref(p);return q;
}

/*:844*//*848:*/
#line 16431 "btex.w"

static void try_break(int pi,small_number break_type)
{
pointer r;
pointer prev_r;
halfword old_l;
bool no_break_yet;
/*849:*/
#line 16459 "btex.w"

pointer prev_prev_r;
pointer s;
pointer q;
pointer v;
int t;
internal_font_number f;
halfword l;
bool node_r_stays_active;
scaled line_width;
int fit_class;
halfword b;
int d;
bool artificial_demerits;
#ifdef STAT
pointer save_link;
#endif
scaled shortfall;

/*:849*/
#line 16438 "btex.w"

/*850:*/
#line 16478 "btex.w"

if(abs(pi)>=inf_penalty)
if(pi> 0)goto end;
else pi= eject_penalty

/*:850*/
#line 16439 "btex.w"
;
no_break_yet= true;prev_r= active;old_l= 0;
do_all_six(copy_to_cur_active);
loop{resume:r= link(prev_r);
/*851:*/
#line 16488 "btex.w"


if(type(r)==delta_node)
{do_all_six(update_width);
prev_prev_r= prev_r;prev_r= r;goto resume;
}

/*:851*/
#line 16444 "btex.w"
;
/*855:*/
#line 16536 "btex.w"

{l= line_number(r);
if(l> old_l)
{
if((minimum_demerits<awful_bad)&&
((old_l!=easy_line)||(r==last_active)))
/*856:*/
#line 16554 "btex.w"

{if(no_break_yet)/*857:*/
#line 16590 "btex.w"

{no_break_yet= false;do_all_six(set_break_width_to_background);
s= cur_p;
if(break_type> unhyphenated)if(cur_p!=null)
/*861:*/
#line 16633 "btex.w"

{t= replace_count(cur_p);v= cur_p;s= post_break(cur_p);
while(t> 0)
{decr(t);v= link(v);
/*862:*/
#line 16651 "btex.w"

if(is_char_node(v))
{f= font(v);
break_width[1]= break_width[1]-char_width(f,char_info(f,character(v)));
}
else switch(type(v)){
case ligature_node:{f= font(lig_char(v));
break_width[1]= break_width[1]-
char_width(f,char_info(f,character(lig_char(v))));
}break;
case hlist_node:case vlist_node:case rule_node:
case kern_node:
break_width[1]= break_width[1]-width(v);break;
default:confusion("disc1");

}

/*:862*/
#line 16637 "btex.w"
;
}
while(s!=null)
{/*863:*/
#line 16668 "btex.w"

if(is_char_node(s))
{f= font(s);
break_width[1]= break_width[1]+char_width(f,char_info(f,character(s)));
}
else switch(type(s)){
case ligature_node:{f= font(lig_char(s));
break_width[1]= break_width[1]+
char_width(f,char_info(f,character(lig_char(s))));
}break;
case hlist_node:case vlist_node:case rule_node:
case kern_node:
break_width[1]= break_width[1]+width(s);break;
default:confusion("disc2");

}

/*:863*/
#line 16640 "btex.w"
;
s= link(s);
}
break_width[1]= break_width[1]+disc_width;
if(post_break(cur_p)==null)s= link(v);

}

/*:861*/
#line 16594 "btex.w"
;
while(s!=null)
{if(is_char_node(s))goto done;
switch(type(s)){
case glue_node:/*858:*/
#line 16609 "btex.w"

{v= glue_ptr(s);break_width[1]= break_width[1]-width(v);
break_width[2+stretch_order(v)]= break_width[2+stretch_order(v)]-stretch(v);
break_width[6]= break_width[6]-shrink(v);
}

/*:858*/
#line 16598 "btex.w"
break;
case penalty_node:do_nothing;break;
case math_node:break_width[1]= break_width[1]-width(s);break;
case kern_node:if(subtype(s)!=explicit)goto done;
else break_width[1]= break_width[1]-width(s);break;
default:goto done;
}
s= link(s);
}
done:;}

/*:857*/
#line 16555 "btex.w"
;
/*864:*/
#line 16694 "btex.w"

if(type(prev_r)==delta_node)
{do_all_six(convert_to_break_width);
}
else if(prev_r==active)
{do_all_six(store_break_width);
}
else{q= get_node(delta_node_size);link(q)= r;type(q)= delta_node;
subtype(q)= 0;
do_all_six(new_delta_to_break_width);
link(prev_r)= q;prev_prev_r= prev_r;prev_r= q;
}

/*:864*/
#line 16556 "btex.w"
;
if(abs(adj_demerits)>=awful_bad-minimum_demerits)
minimum_demerits= awful_bad-1;
else minimum_demerits= minimum_demerits+abs(adj_demerits);
for(fit_class= very_loose_fit;fit_class<=tight_fit;fit_class++)
{if(minimal_demerits[fit_class]<=minimum_demerits)
/*866:*/
#line 16724 "btex.w"

{q= get_node(passive_node_size);
link(q)= passive;passive= q;cur_break(q)= cur_p;
#ifdef STAT
incr(pass_number);serial(q)= pass_number;
#endif

prev_break(q)= best_place[fit_class];
q= get_node(active_node_size);break_node(q)= passive;
line_number(q)= best_pl_line[fit_class]+1;
fitness(q)= fit_class;type(q)= break_type;
total_demerits(q)= minimal_demerits[fit_class];
link(q)= r;link(prev_r)= q;prev_r= q;
#ifdef STAT
if(tracing_paragraphs> 0)
/*867:*/
#line 16744 "btex.w"

{print_nl("@@");print_int(serial(passive));

print(": line ");print_int(line_number(q)-1);
print_char('.');print_int(fit_class);
if(break_type==hyphenated)print_char('-');
print(" t=");print_int(total_demerits(q));
print(" -> @@");
if(prev_break(passive)==null)print_char('0');
else print_int(serial(prev_break(passive)));
}

/*:867*/
#line 16739 "btex.w"
;
#endif

}

/*:866*/
#line 16562 "btex.w"
;
minimal_demerits[fit_class]= awful_bad;
}
minimum_demerits= awful_bad;
/*865:*/
#line 16713 "btex.w"

if(r!=last_active)
{q= get_node(delta_node_size);link(q)= r;type(q)= delta_node;
subtype(q)= 0;
do_all_six(new_delta_from_break_width);
link(prev_r)= q;prev_prev_r= prev_r;prev_r= q;
}

/*:865*/
#line 16566 "btex.w"
;
}

/*:856*/
#line 16542 "btex.w"
;
if(r==last_active)goto end;
/*872:*/
#line 16820 "btex.w"

if(l> easy_line)
{line_width= second_width;old_l= max_halfword-1;
}
else{old_l= l;
if(l> last_special_line)line_width= second_width;
else if(par_shape_ptr==null)line_width= first_width;
else line_width= mem[par_shape_ptr+2*l].sc;
}

/*:872*/
#line 16544 "btex.w"
;
}
}

/*:855*/
#line 16447 "btex.w"
;
/*873:*/
#line 16840 "btex.w"

{artificial_demerits= false;

shortfall= line_width-cur_active_width[1];
if(shortfall> 0)
/*874:*/
#line 16870 "btex.w"

if((cur_active_width[3]!=0)||(cur_active_width[4]!=0)||
(cur_active_width[5]!=0))
{b= 0;fit_class= decent_fit;
}
else{if(shortfall> 7230584)if(cur_active_width[2]<1663497)
{b= inf_bad;fit_class= very_loose_fit;goto done1;
}
b= badness(shortfall,cur_active_width[2]);
if(b> 12)
if(b> 99)fit_class= very_loose_fit;
else fit_class= loose_fit;
else fit_class= decent_fit;
done1:;
}

/*:874*/
#line 16846 "btex.w"

else/*875:*/
#line 16889 "btex.w"

{if(-shortfall> cur_active_width[6])b= inf_bad+1;
else b= badness(-shortfall,cur_active_width[6]);
if(b> 12)fit_class= tight_fit;else fit_class= decent_fit;
}

/*:875*/
#line 16848 "btex.w"
;
if((b> inf_bad)||(pi==eject_penalty))
/*876:*/
#line 16904 "btex.w"

{if(final_pass&&(minimum_demerits==awful_bad)&&
(link(r)==last_active)&&
(prev_r==active))
artificial_demerits= true;
else if(b> threshold)goto deactivate;
node_r_stays_active= false;
}

/*:876*/
#line 16851 "btex.w"

else{prev_r= r;
if(b> threshold)goto resume;
node_r_stays_active= true;
}
/*877:*/
#line 16920 "btex.w"

if(artificial_demerits)d= 0;
else/*881:*/
#line 16981 "btex.w"

{d= line_penalty+b;
if(abs(d)>=10000)d= 100000000;else d= d*d;
if(pi!=0)
if(pi> 0)d= d+pi*pi;
else if(pi> eject_penalty)d= d-pi*pi;
if((break_type==hyphenated)&&(type(r)==hyphenated))
if(cur_p!=null)d= d+double_hyphen_demerits;
else d= d+final_hyphen_demerits;
if(abs(fit_class-fitness(r))> 1)d= d+adj_demerits;
}

/*:881*/
#line 16922 "btex.w"
;
#ifdef STAT
if(tracing_paragraphs> 0)
/*878:*/
#line 16936 "btex.w"

{if(printed_node!=cur_p)
/*879:*/
#line 16958 "btex.w"

{print_nl("");
if(cur_p==null)short_display(link(printed_node));
else{save_link= link(cur_p);
link(cur_p)= null;print_nl("");short_display(link(printed_node));
link(cur_p)= save_link;
}
printed_node= cur_p;
}

/*:879*/
#line 16938 "btex.w"
;
print_nl("@");

if(cur_p==null)print_esc("par");
else if(type(cur_p)!=glue_node)
{if(type(cur_p)==penalty_node)print_esc("penalty");
else if(type(cur_p)==disc_node)print_esc("discretionary");
else if(type(cur_p)==kern_node)print_esc("kern");
else print_esc("math");
}
print(" via @@");
if(break_node(r)==null)print_char('0');
else print_int(serial(break_node(r)));
print(" b=");
if(b> inf_bad)print_char('*');else print_int(b);

print(" p=");print_int(pi);print(" d=");
if(artificial_demerits)print_char('*');else print_int(d);
}

/*:878*/
#line 16925 "btex.w"
;
#endif

d= d+total_demerits(r);

if(d<=minimal_demerits[fit_class])
{minimal_demerits[fit_class]= d;
best_place[fit_class]= break_node(r);best_pl_line[fit_class]= l;
if(d<minimum_demerits)minimum_demerits= d;
}

/*:877*/
#line 16856 "btex.w"
;
if(node_r_stays_active)goto resume;
deactivate:/*882:*/
#line 17003 "btex.w"

link(prev_r)= link(r);free_node(r,active_node_size);
if(prev_r==active)/*883:*/
#line 17028 "btex.w"

{r= link(active);
if(type(r)==delta_node)
{do_all_six(update_active);
do_all_six(copy_to_cur_active);
link(active)= link(r);free_node(r,delta_node_size);
}
}

/*:883*/
#line 17006 "btex.w"

else if(type(prev_r)==delta_node)
{r= link(prev_r);
if(r==last_active)
{do_all_six(downdate_width);
link(prev_prev_r)= last_active;
free_node(prev_r,delta_node_size);prev_r= prev_prev_r;
}
else if(type(r)==delta_node)
{do_all_six(update_width);
do_all_six(combine_two_deltas);
link(prev_r)= link(r);free_node(r,delta_node_size);
}
}

/*:882*/
#line 16858 "btex.w"
;
}

/*:873*/
#line 16450 "btex.w"
;
}
end:;
#ifdef STAT
/*880:*/
#line 16973 "btex.w"

if(cur_p==printed_node)if(cur_p!=null)if(type(cur_p)==disc_node)
{t= replace_count(cur_p);
while(t> 0)
{decr(t);printed_node= link(printed_node);
}
}

/*:880*/
#line 16454 "btex.w"
;
#endif

}

/*:848*//*900:*/
#line 17357 "btex.w"

static void post_line_break(int final_widow_penalty)
{
pointer q,r,s;
bool disc_break;
#line 1451 "htex.ch"
bool post_disc_break;
bool first_line= true;
uint32_t line_offset,next_offset;
#line 17363 "btex.w"
scaled cur_width;
scaled cur_indent;
quarterword t;
int pen;
halfword cur_line;
/*901:*/
#line 17389 "btex.w"

q= break_node(best_bet);cur_p= null;
do{r= q;q= prev_break(q);next_break(r)= cur_p;cur_p= r;
}while(!(q==null))

/*:901*/
#line 17369 "btex.w"
;
cur_line= prev_graf+1;
#line 1460 "htex.ch"
next_offset= hposition(link(temp_head));
if(next_offset> node_pos)
next_offset= next_offset-node_pos;
else
next_offset= 0;
do{
line_offset= next_offset;
{pointer q= cur_break(cur_p);
if(q==null)
next_offset= (hstart-hpos);
else
next_offset= hposition(q);
if(next_offset> node_pos)
next_offset= next_offset-node_pos;
else
next_offset= 0;
}
/*903:*/
#line 17426 "btex.w"

/*904:*/
#line 17438 "btex.w"

q= cur_break(cur_p);disc_break= false;post_disc_break= false;
if(q!=null)
if(type(q)==glue_node)
{delete_glue_ref(glue_ptr(q));
glue_ptr(q)= right_skip;
subtype(q)= right_skip_code+1;add_glue_ref(right_skip);
goto done;
}
else{if(type(q)==disc_node)
/*905:*/
#line 1491 "htex.ch"

{pointer pre_q= pre_break(q);
pointer post_q= post_break(q);
t= replace_count(q);
type(q)= whatsit_node;
subtype(q)= ignore_node;
ignore_info(q)= 1;
/*906:*/
#line 1523 "htex.ch"

if(t==0){ignore_list(q)= null;r= link(q);}
else{r= q;
while(t> 1)
{r= link(r);decr(t);
}
s= link(r);
r= link(s);link(s)= null;
ignore_list(q)= link(q);
}
#line 17476 "btex.w"

/*:906*/
#line 1498 "htex.ch"
;
s= get_node(ignore_node_size);
type(s)= whatsit_node;
subtype(s)= ignore_node;
ignore_info(s)= 0;
ignore_list(s)= null;
link(s)= r;r= s;
if(post_q!=null)/*907:*/
#line 1542 "htex.ch"

{s= post_q;
while(link(s)!=null)s= link(s);
link(s)= r;r= post_q;post_disc_break= true;
}
#line 17485 "btex.w"

/*:907*/
#line 1505 "htex.ch"
;
if(pre_q!=null)/*908:*/
#line 1556 "htex.ch"

{s= pre_q;link(q)= s;
while(link(s)!=null)s= link(s);
q= s;
}
#line 17494 "btex.w"

/*:908*/
#line 1506 "htex.ch"
;
link(q)= r;disc_break= true;
}
#line 17465 "btex.w"

#line 1523 "htex.ch"
/*:905*/
#line 17448 "btex.w"

else if((type(q)==math_node)||(type(q)==kern_node))width(q)= 0;
}
else{q= temp_head;
while(link(q)!=null)q= link(q);
}
/*909:*/
#line 17495 "btex.w"

#line 1566 "htex.ch"
r= new_glue(right_skip);link(r)= link(q);link(q)= r;q= r
#line 17497 "btex.w"

/*:909*/
#line 17454 "btex.w"
;
done:

#line 1491 "htex.ch"
/*:904*/
#line 17428 "btex.w"
;
/*910:*/
#line 17502 "btex.w"

r= link(q);link(q)= null;q= link(temp_head);link(temp_head)= r;
if(left_skip!=zero_glue)
#line 1572 "htex.ch"
{r= new_glue(left_skip);
#line 17506 "btex.w"
link(r)= q;q= r;
}

/*:910*/
#line 17429 "btex.w"
;
/*912:*/
#line 17521 "btex.w"

if(cur_line> last_special_line)
{cur_width= second_width;cur_indent= second_indent;
}
else if(par_shape_ptr==null)
{cur_width= first_width;cur_indent= first_indent;
}
else{cur_width= mem[par_shape_ptr+2*cur_line].sc;
cur_indent= mem[par_shape_ptr+2*cur_line-1].sc;
}
adjust_tail= adjust_head;just_box= hpack(q,cur_width,exactly);
shift_amount(just_box)= cur_indent

/*:912*/
#line 17430 "btex.w"
;
/*911:*/
#line 17509 "btex.w"

#line 1578 "htex.ch"
if(first_line)
{pointer p= happend_to_vlist(just_box);
uint32_t pos= hposition(p);
store_map(p,pos,line_offset);
first_line= false;
}
else
append_to_vlist(just_box,line_offset);
#line 17511 "btex.w"
if(adjust_head!=adjust_tail)
{link(tail)= link(adjust_head);tail= adjust_tail;
}
adjust_tail= null

/*:911*/
#line 17432 "btex.w"
;
/*913:*/
#line 17541 "btex.w"

if(cur_line+1!=best_line)
{pen= inter_line_penalty;
if(cur_line==prev_graf+1)pen= pen+club_penalty;
if(cur_line+2==best_line)pen= pen+final_widow_penalty;
if(disc_break)pen= pen+broken_penalty;
if(pen!=0)
#line 1591 "htex.ch"
{r= new_penalty(pen);store_map(r,node_pos,next_offset);
#line 17549 "btex.w"
link(tail)= r;tail= r;
}
}

/*:913*/
#line 17433 "btex.w"


/*:903*/
#line 1478 "htex.ch"
;
#line 17373 "btex.w"
incr(cur_line);cur_p= next_break(cur_p);
if(cur_p!=null)if(!post_disc_break)
/*902:*/
#line 17401 "btex.w"

{r= temp_head;
loop{q= link(r);
if(q==cur_break(cur_p))goto done1;


if(is_char_node(q))goto done1;
if(non_discardable(q))goto done1;
if(type(q)==kern_node)if(subtype(q)!=explicit)goto done1;
r= q;
}
done1:if(r!=temp_head)
{link(r)= null;flush_node_list(link(temp_head));
link(temp_head)= q;
}
}

/*:902*/
#line 17375 "btex.w"
;
}while(!(cur_p==null));
if((cur_line!=best_line)||(link(temp_head)!=null))
confusion("line breaking");

prev_graf= best_line-1;
}

/*:900*//*918:*/
#line 17655 "btex.w"

#line 1603 "htex.ch"
/*:918*//*966:*/
#line 18478 "btex.w"

#ifdef INIT
#line 18481 "btex.w"
#endif

/*:966*/
#line 1231 "htex.ch"


void line_break(int final_widow_penalty,pointer par_ptr)
{scaled x= cur_list.hs_field;
#line 16119 "btex.w"
/*884:*/
#line 17057 "btex.w"

bool auto_breaking;
pointer prev_p;
#line 1380 "htex.ch"
pointer q,r,s;
#line 17061 "btex.w"
internal_font_number f;

/*:884*//*916:*/
#line 17636 "btex.w"

#line 17639 "btex.w"

/*:916*/
#line 16119 "btex.w"

#line 1240 "htex.ch"
set_line_break_params();
#line 16121 "btex.w"
/*831:*/
#line 1263 "htex.ch"

link(temp_head)= par_ptr;
#line 16152 "btex.w"

/*:831*//*845:*/
#line 16377 "btex.w"

no_shrink_error_yet= true;
check_shrinkage(left_skip);check_shrinkage(right_skip);
q= left_skip;r= right_skip;background[1]= width(q)+width(r);
background[2]= 0;background[3]= 0;background[4]= 0;background[5]= 0;
background[2+stretch_order(q)]= stretch(q);
background[2+stretch_order(r)]= background[2+stretch_order(r)]+stretch(r);
background[6]= shrink(q)+shrink(r);

/*:845*//*854:*/
#line 16522 "btex.w"

minimum_demerits= awful_bad;
minimal_demerits[tight_fit]= awful_bad;
minimal_demerits[decent_fit]= awful_bad;
minimal_demerits[loose_fit]= awful_bad;
minimal_demerits[very_loose_fit]= awful_bad;

/*:854*//*870:*/
#line 16785 "btex.w"

if(par_shape_ptr==null)
if(hang_indent==0)
#line 1347 "htex.ch"
{last_special_line= 0;second_width= x;
#line 16789 "btex.w"
second_indent= 0;
}
else/*871:*/
#line 16799 "btex.w"

{last_special_line= abs(hang_after);
if(hang_after<0)
#line 1362 "htex.ch"
{first_width= x-abs(hang_indent);
#line 16803 "btex.w"
if(hang_indent>=0)first_indent= hang_indent;
else first_indent= 0;
#line 1371 "htex.ch"
second_width= x;second_indent= 0;
}
else{first_width= x;first_indent= 0;
second_width= x-abs(hang_indent);
#line 16809 "btex.w"
if(hang_indent>=0)second_indent= hang_indent;
else second_indent= 0;
}
}

/*:871*/
#line 16791 "btex.w"

#line 1356 "htex.ch"
else QUIT("parshape not yet implemented");
#line 16796 "btex.w"
if(looseness==0)easy_line= last_special_line;
else easy_line= max_halfword

/*:870*/
#line 16121 "btex.w"
;
/*885:*/
#line 17067 "btex.w"

threshold= pretolerance;
if(threshold>=0)
{
#ifdef STAT
#line 1387 "htex.ch"
 if(tracing_paragraphs> 0)
{print_nl("@firstpass");}
#line 17074 "btex.w"
#endif

second_pass= false;final_pass= false;
}
else{threshold= tolerance;second_pass= true;
final_pass= (emergency_stretch<=0);
#ifdef STAT
#line 17082 "btex.w"
#endif

}
loop{if(threshold> inf_bad)threshold= inf_bad;
#line 17087 "btex.w"
/*886:*/
#line 17125 "btex.w"

q= get_node(active_node_size);
type(q)= unhyphenated;fitness(q)= decent_fit;
link(q)= last_active;break_node(q)= null;
line_number(q)= prev_graf+1;total_demerits(q)= 0;link(active)= q;
do_all_six(store_background);
passive= null;printed_node= temp_head;pass_number= 0;
#line 17133 "btex.w"

/*:886*/
#line 17087 "btex.w"
;
cur_p= link(temp_head);auto_breaking= true;
prev_p= cur_p;
while((cur_p!=null)&&(link(active)!=last_active))
/*888:*/
#line 17161 "btex.w"

{if(is_char_node(cur_p))
/*889:*/
#line 17197 "btex.w"

{prev_p= cur_p;
do{f= font(cur_p);
act_width= act_width+char_width(f,char_info(f,character(cur_p)));
cur_p= link(cur_p);
}while(!(!is_char_node(cur_p)));
}

/*:889*/
#line 17163 "btex.w"
;
switch(type(cur_p)){
case hlist_node:case vlist_node:case rule_node:act_width= act_width+width(cur_p);break;
case whatsit_node:/*1391:*/
#line 24920 "btex.w"

adv_past(cur_p)

/*:1391*/
#line 17167 "btex.w"
break;
case glue_node:{/*890:*/
#line 17208 "btex.w"

if(auto_breaking)
{if(is_char_node(prev_p))try_break(0,unhyphenated);
else if(precedes_break(prev_p))try_break(0,unhyphenated);
else if((type(prev_p)==kern_node)&&(subtype(prev_p)!=explicit))
try_break(0,unhyphenated);
}
check_shrinkage(glue_ptr(cur_p));q= glue_ptr(cur_p);
act_width= act_width+width(q);
active_width[2+stretch_order(q)]= 
active_width[2+stretch_order(q)]+stretch(q);
active_width[6]= active_width[6]+shrink(q)

/*:890*/
#line 17169 "btex.w"
;
#line 17172 "btex.w"
}break;
case kern_node:if(subtype(cur_p)==explicit)kern_break
else act_width= act_width+width(cur_p);break;
case ligature_node:{f= font(lig_char(cur_p));
act_width= act_width+char_width(f,char_info(f,character(lig_char(cur_p))));
}break;
case disc_node:/*891:*/
#line 17224 "btex.w"

#line 1430 "htex.ch"
{if(!is_auto_disc(cur_p)||second_pass||final_pass)
{s= pre_break(cur_p);disc_width= 0;
#line 17226 "btex.w"
if(s==null)try_break(ex_hyphen_penalty,hyphenated);
else{do{/*892:*/
#line 17242 "btex.w"

if(is_char_node(s))
{f= font(s);
disc_width= disc_width+char_width(f,char_info(f,character(s)));
}
else switch(type(s)){
case ligature_node:{f= font(lig_char(s));
disc_width= disc_width+
char_width(f,char_info(f,character(lig_char(s))));
}break;
case hlist_node:case vlist_node:case rule_node:
case kern_node:
disc_width= disc_width+width(s);break;
default:confusion("disc3");

}

/*:892*/
#line 17227 "btex.w"
;
s= link(s);
}while(!(s==null));
act_width= act_width+disc_width;
try_break(hyphen_penalty,hyphenated);
act_width= act_width-disc_width;
}
#line 1437 "htex.ch"
}
r= replace_count(cur_p);s= link(cur_p);
#line 17235 "btex.w"
while(r> 0)
{/*893:*/
#line 17259 "btex.w"

if(is_char_node(s))
{f= font(s);
act_width= act_width+char_width(f,char_info(f,character(s)));
}
else switch(type(s)){
case ligature_node:{f= font(lig_char(s));
act_width= act_width+
char_width(f,char_info(f,character(lig_char(s))));
}break;
case hlist_node:case vlist_node:case rule_node:
case kern_node:
act_width= act_width+width(s);break;
default:confusion("disc4");

}

/*:893*/
#line 17236 "btex.w"
;
decr(r);s= link(s);
}
prev_p= cur_p;cur_p= s;goto done5;
}

/*:891*/
#line 17179 "btex.w"

case math_node:{auto_breaking= (subtype(cur_p)==after);kern_break;
}break;
case penalty_node:try_break(penalty(cur_p),unhyphenated);break;
case mark_node:case ins_node:case adjust_node:do_nothing;break;
default:confusion("paragraph");

}
prev_p= cur_p;cur_p= link(cur_p);
done5:;}

/*:888*/
#line 17093 "btex.w"
;
if(cur_p==null)
/*896:*/
#line 17295 "btex.w"

{try_break(eject_penalty,hyphenated);
if(link(active)!=last_active)
{/*897:*/
#line 17305 "btex.w"

r= link(active);fewest_demerits= awful_bad;
do{if(type(r)!=delta_node)if(total_demerits(r)<fewest_demerits)
{fewest_demerits= total_demerits(r);best_bet= r;
}
r= link(r);
}while(!(r==last_active));
best_line= line_number(best_bet)

/*:897*/
#line 17298 "btex.w"
;
if(looseness==0)goto done;
/*898:*/
#line 17319 "btex.w"

{r= link(active);actual_looseness= 0;
do{if(type(r)!=delta_node)
{line_diff= line_number(r)-best_line;
if(((line_diff<actual_looseness)&&(looseness<=line_diff))||
((line_diff> actual_looseness)&&(looseness>=line_diff)))
{best_bet= r;actual_looseness= line_diff;
fewest_demerits= total_demerits(r);
}
else if((line_diff==actual_looseness)&&
(total_demerits(r)<fewest_demerits))
{best_bet= r;fewest_demerits= total_demerits(r);
}
}
r= link(r);
}while(!(r==last_active));
best_line= line_number(best_bet);
}

/*:898*/
#line 17300 "btex.w"
;
if((actual_looseness==looseness)||final_pass)goto done;
}
}

/*:896*/
#line 17096 "btex.w"
;
/*887:*/
#line 17134 "btex.w"

q= link(active);
while(q!=last_active)
{cur_p= link(q);
if(type(q)==delta_node)free_node(q,delta_node_size);
else free_node(q,active_node_size);
q= cur_p;
}
q= passive;
while(q!=null)
{cur_p= link(q);
free_node(q,passive_node_size);
q= cur_p;
}

/*:887*/
#line 17097 "btex.w"
;
if(!second_pass)
{
#ifdef STAT
if(tracing_paragraphs> 0)print_nl("@secondpass");
#endif
threshold= tolerance;second_pass= true;final_pass= (emergency_stretch<=0);
}
else{
#ifdef STAT
#line 1405 "htex.ch"
 if(tracing_paragraphs> 0)
print_nl("@emergencypass");
#line 17109 "btex.w"
#endif
background[2]= background[2]+emergency_stretch;final_pass= true;
}
}
done:
#ifdef STAT
#line 17118 "btex.w"
#endif

/*:885*/
#line 16122 "btex.w"
;
/*899:*/
#line 17343 "btex.w"

post_line_break(final_widow_penalty)

/*:899*/
#line 16124 "btex.w"
;
/*887:*/
#line 17134 "btex.w"

q= link(active);
while(q!=last_active)
{cur_p= link(q);
if(type(q)==delta_node)free_node(q,delta_node_size);
else free_node(q,active_node_size);
q= cur_p;
}
q= passive;
while(q!=null)
{cur_p= link(q);
free_node(q,passive_node_size);
q= cur_p;
}

/*:887*/
#line 16125 "btex.w"
;
#line 1246 "htex.ch"
hrestore_param_list();
#line 16127 "btex.w"
}

/*:830*//*992:*/
#line 1615 "htex.ch"


#define ensure_vbox(N) 

static pointer prune_page_top(pointer p)
#line 18981 "btex.w"
{pointer prev_p;
pointer q;
prev_p= temp_head;link(temp_head)= p;
while(p!=null)
switch(type(p)){
case hlist_node:case vlist_node:case rule_node:/*993:*/
#line 18999 "btex.w"

#line 1625 "htex.ch"
{temp_ptr= new_spec(pointer_def[glue_kind][split_top_skip_no]);
q= new_glue(temp_ptr);glue_ref_count(temp_ptr)= null;link(prev_p)= q;link(q)= p;
#line 19001 "btex.w"

if(width(temp_ptr)> height(p))width(temp_ptr)= width(temp_ptr)-height(p);
else width(temp_ptr)= 0;
p= null;
}

/*:993*/
#line 18987 "btex.w"
break;
case whatsit_node:case mark_node:case ins_node:{prev_p= p;p= link(prev_p);
}break;
case glue_node:case kern_node:case penalty_node:{q= p;p= link(q);link(q)= null;
link(prev_p)= p;flush_node_list(q);
}break;
default:confusion("pruning");

}
return link(temp_head);
}

/*:992*//*994:*/
#line 1632 "htex.ch"

static pointer vert_break(pointer p,scaled h,scaled d)
#line 19024 "btex.w"

{
pointer prev_p;

pointer q,r;
int pi;
int b;
int least_cost;
pointer best_place;
scaled prev_dp;
small_number t;
prev_p= p;
least_cost= awful_bad;do_all_six(set_height_zero);prev_dp= 0;
loop{/*997:*/
#line 19057 "btex.w"

if(p==null)pi= eject_penalty;
else/*998:*/
#line 19072 "btex.w"

switch(type(p)){
case hlist_node:case vlist_node:case rule_node:{
cur_height= cur_height+prev_dp+height(p);prev_dp= depth(p);
goto not_found;
}
case whatsit_node:/*1394:*/
#line 24929 "btex.w"

goto not_found

/*:1394*/
#line 19078 "btex.w"
;
case glue_node:if(precedes_break(prev_p))pi= 0;
else goto update_heights;break;
case kern_node:{if(link(p)==null)t= penalty_node;
else t= type(link(p));
if(t==glue_node)pi= 0;else goto update_heights;
}break;
case penalty_node:pi= penalty(p);break;
case mark_node:case ins_node:goto not_found;
default:confusion("vertbreak");

}

/*:998*/
#line 19061 "btex.w"
;
/*999:*/
#line 19093 "btex.w"

if(pi<inf_penalty)
{/*1000:*/
#line 19107 "btex.w"

if(cur_height<h)
if((active_height[3]!=0)||(active_height[4]!=0)||
(active_height[5]!=0))b= 0;
else b= badness(h-cur_height,active_height[2]);
else if(cur_height-h> active_height[6])b= awful_bad;
else b= badness(cur_height-h,active_height[6])

/*:1000*/
#line 19095 "btex.w"
;
if(b<awful_bad)
if(pi<=eject_penalty)b= pi;
else if(b<inf_bad)b= b+pi;
else b= deplorable;
if(b<=least_cost)
{best_place= p;least_cost= b;
best_height_plus_depth= cur_height+prev_dp;
}
if((b==awful_bad)||(pi<=eject_penalty))goto done;
}

/*:999*/
#line 19063 "btex.w"
;
if((type(p)<glue_node)||(type(p)> kern_node))goto not_found;
update_heights:/*1001:*/
#line 19119 "btex.w"

if(type(p)==kern_node)q= p;
else{q= glue_ptr(p);
active_height[2+stretch_order(q)]= 
active_height[2+stretch_order(q)]+stretch(q);
active_height[6]= active_height[6]+shrink(q);
#line 1658 "htex.ch"
if((shrink_order(q)!=normal)&&(shrink(q)!=0))
{
DBG(DBGTEX,"Infinite glue shrinkage found in box being split");
r= new_spec(q);shrink_order(r)= normal;delete_glue_ref(q);
glue_ptr(p)= r;q= r;
}
#line 19136 "btex.w"
}
cur_height= cur_height+prev_dp+width(q);prev_dp= 0

/*:1001*/
#line 19066 "btex.w"
;
not_found:if(prev_dp> d)
{cur_height= cur_height+prev_dp-d;
prev_dp= d;
}

/*:997*/
#line 19039 "btex.w"
;
prev_p= p;p= link(prev_p);
}
done:return best_place;
}

/*:994*//*1012:*/
#line 1688 "htex.ch"

void freeze_page_specs(small_number s)
#line 19426 "btex.w"
{page_contents= s;
#line 1695 "htex.ch"
page_goal= hvsize;page_max_depth= max_depth;
#line 19428 "btex.w"
page_depth= 0;do_all_six(set_page_so_far_zero);
least_page_cost= awful_bad;
#line 19439 "btex.w"

}

/*:1012*//*1020:*/
#line 1721 "htex.ch"

bool hbuild_page(void)
#line 19524 "btex.w"
{
pointer p;
pointer q,r;
int b,c;
int pi;
#line 1730 "htex.ch"
if(link(contrib_head)==null)return false;
#line 19532 "btex.w"
do{resume:p= link(contrib_head);
#line 19534 "btex.w"
/*1023:*/
#line 19564 "btex.w"

/*1026:*/
#line 19593 "btex.w"

switch(type(p)){
case hlist_node:case vlist_node:case rule_node:if(page_contents<box_there)
/*1027:*/
#line 19616 "btex.w"

{if(page_contents==empty)freeze_page_specs(box_there);
else page_contents= box_there;
#line 1760 "htex.ch"
temp_ptr= new_spec(pointer_def[glue_kind][top_skip_no]);
q= new_glue(temp_ptr);glue_ref_count(temp_ptr)= null;
#line 19620 "btex.w"
if(width(temp_ptr)> height(p))width(temp_ptr)= width(temp_ptr)-height(p);
else width(temp_ptr)= 0;
link(q)= p;link(contrib_head)= q;goto resume;
}

/*:1027*/
#line 19597 "btex.w"

else/*1028:*/
#line 19625 "btex.w"

{page_total= page_total+page_depth+height(p);
page_depth= depth(p);
goto contribute;
}

/*:1028*/
#line 19599 "btex.w"
break;
case whatsit_node:/*1393:*/
#line 24926 "btex.w"

goto contribute

/*:1393*/
#line 19601 "btex.w"
;
case glue_node:if(page_contents<box_there)goto done1;
else if(precedes_break(page_tail))pi= 0;
else goto update_heights;break;
case kern_node:if(page_contents<box_there)goto done1;
#line 1748 "htex.ch"
else if(link(p)==null)return false;
#line 19607 "btex.w"
else if(type(link(p))==glue_node)pi= 0;
else goto update_heights;break;
case penalty_node:if(page_contents<box_there)goto done1;else pi= penalty(p);break;
case mark_node:goto contribute;
#line 1754 "htex.ch"
case ins_node:happend_insertion(p);goto contribute;
#line 19612 "btex.w"
default:confusion("page");

}

/*:1026*/
#line 19569 "btex.w"
;
/*1031:*/
#line 19659 "btex.w"

if(pi<inf_penalty)
{/*1033:*/
#line 19703 "btex.w"

if(page_total<page_goal)
if((page_so_far[3]!=0)||(page_so_far[4]!=0)||
(page_so_far[5]!=0))b= 0;
else b= badness(page_goal-page_total,page_so_far[2]);
else if(page_total-page_goal> page_shrink)b= awful_bad;
else b= badness(page_total-page_goal,page_shrink)

#line 1811 "htex.ch"
/*:1033*/
#line 19662 "btex.w"
;
if(b<awful_bad)
if(pi<=eject_penalty)c= pi;
else if(b<inf_bad)c= b+pi+insert_penalties;
else c= deplorable;
else c= b;
if(insert_penalties>=10000)c= awful_bad;
#line 19672 "btex.w"

if(c<=least_page_cost)
{best_page_break= p;best_size= page_goal;
least_page_cost= c;
r= link(page_ins_head);
while(r!=page_ins_head)
{best_ins_ptr(r)= last_ins_ptr(r);
r= link(r);
}
}
if((c==awful_bad)||(pi<=eject_penalty))
#line 1799 "htex.ch"
{hloc_set_next(best_page_break);
if(p==best_page_break)best_page_break= null;
hpack_page();
hfill_page_template();
return true;
}
#line 19687 "btex.w"
}

/*:1031*/
#line 19572 "btex.w"
;
if((type(p)<glue_node)||(type(p)> kern_node))goto contribute;
update_heights:/*1030:*/
#line 19638 "btex.w"

if(type(p)==kern_node)q= p;
else{q= glue_ptr(p);
page_so_far[2+stretch_order(q)]= 
page_so_far[2+stretch_order(q)]+stretch(q);
page_shrink= page_shrink+shrink(q);
#line 1778 "htex.ch"
if((shrink_order(q)!=normal)&&(shrink(q)!=0))
{
DBG(DBGTEX,"Infinite glue shrinkage found on current page");
r= new_spec(q);shrink_order(r)= normal;fast_delete_glue_ref(q);
glue_ptr(p)= r;q= r;
}
#line 19656 "btex.w"
}
page_total= page_total+page_depth+width(q);page_depth= 0

/*:1030*/
#line 19575 "btex.w"
;
contribute:/*1029:*/
#line 19631 "btex.w"

if(page_depth> page_max_depth)
{page_total= 
page_total+page_depth-page_max_depth;
page_depth= page_max_depth;
}

/*:1029*/
#line 19576 "btex.w"
;
/*1024:*/
#line 19581 "btex.w"

link(page_tail)= p;page_tail= p;
link(contrib_head)= link(p);link(p)= null;goto done

/*:1024*/
#line 19577 "btex.w"
;
done1:/*1025:*/
#line 19585 "btex.w"

link(contrib_head)= link(p);link(p)= null;flush_node_list(p)

/*:1025*/
#line 19578 "btex.w"
;
done:

/*:1023*/
#line 19536 "btex.w"
;
}while(!(link(contrib_head)==null));
/*1021:*/
#line 19543 "btex.w"

if(nest_ptr==0)tail= contrib_head;
else contrib_tail= contrib_head

/*:1021*/
#line 19538 "btex.w"
;
#line 1741 "htex.ch"
return false;
}
#line 19540 "btex.w"

/*:1020*//*1034:*/
#line 1811 "htex.ch"

void happend_insertion(pointer p)
{uint8_t n;
scaled delta,h,w;
pointer q,r;
if(page_contents==empty)freeze_page_specs(inserts_only);
#line 19713 "btex.w"
n= subtype(p);r= page_ins_head;
while(n>=subtype(link(r)))r= link(r);
n= qo(n);
if(subtype(r)!=qi(n))
/*1035:*/
#line 19740 "btex.w"

{q= get_node(page_ins_node_size);link(q)= link(r);link(r)= q;r= q;
subtype(r)= qi(n);type(r)= inserting;ensure_vbox(n);
if(box(n)==null)height(r)= 0;
else height(r)= height(box(n))+depth(box(n));
best_ins_ptr(r)= null;
q= skip(n);
if(count(n)==1000)h= height(r);
else h= x_over_n(height(r),1000)*count(n);
page_goal= page_goal-h-width(q);
page_so_far[2+stretch_order(q)]= page_so_far[2+stretch_order(q)]+stretch(q);
page_shrink= page_shrink+shrink(q);
#line 1835 "htex.ch"
if((shrink_order(q)!=normal)&&(shrink(q)!=0))
DBG(DBGTEX,"Infinite glue shrinkage inserted from stream %d",n);
#line 19761 "btex.w"
}

/*:1035*/
#line 19718 "btex.w"
;
if(type(r)==split_up)insert_penalties= insert_penalties+float_cost(p);
else{last_ins_ptr(r)= p;
delta= page_goal-page_total-page_depth+page_shrink;

if(count(n)==1000)h= height(p);
else h= x_over_n(height(p),1000)*count(n);
if(((h<=0)||(h<=delta))&&(height(p)+height(r)<=dimen(n)))
{page_goal= page_goal-h;height(r)= height(r)+height(p);
}
else/*1036:*/
#line 19773 "btex.w"

{if(count(n)<=0)w= max_dimen;
else{w= page_goal-page_total-page_depth;
if(count(n)!=1000)w= x_over_n(w,count(n))*1000;
}
if(w> dimen(n)-height(r))w= dimen(n)-height(r);
q= vert_break(ins_ptr(p),w,depth(p));
height(r)= height(r)+best_height_plus_depth;
#line 19784 "btex.w"

if(count(n)!=1000)
best_height_plus_depth= x_over_n(best_height_plus_depth,1000)*count(n);
page_goal= page_goal-best_height_plus_depth;
type(r)= split_up;broken_ptr(r)= q;broken_ins(r)= p;
if(q==null)insert_penalties= insert_penalties+eject_penalty;
else if(type(q)==penalty_node)insert_penalties= insert_penalties+penalty(q);
}

/*:1036*/
#line 19729 "btex.w"
;
}
#line 19732 "btex.w"
}

/*:1034*//*1041:*/
#line 1869 "htex.ch"

void hpack_page(void)
{
pointer p,q,r,s;
pointer prev_p;
uint8_t n;
bool wait;
pointer save_split_top_skip;
#if 0
print_str("\npage_head:\n");
show_box(link(page_head));
print_str("\nstream 0:\n");
show_box(streams[0].p);
print_str("\nstream 1:\n");
show_box(streams[1].p);
#endif
if(box(0)!=null)
{flush_node_list(box(0));box(0)= null;}
insert_penalties= 0;
save_split_top_skip= split_top_skip;
/*1045:*/
#line 19937 "btex.w"

{r= link(page_ins_head);
while(r!=page_ins_head)
{if(best_ins_ptr(r)!=null)
{n= qo(subtype(r));ensure_vbox(n);
if(box(n)==null)box(n)= new_null_box();
p= box(n)+list_offset;
while(link(p)!=null)p= link(p);
last_ins_ptr(r)= p;
}
r= link(r);
}
}

/*:1045*/
#line 1889 "htex.ch"
;
q= hold_head;link(q)= null;prev_p= page_head;p= link(prev_p);
while(p!=best_page_break)
{if(type(p)==ins_node)
{/*1047:*/
#line 19963 "btex.w"

{r= link(page_ins_head);
while(subtype(r)!=subtype(p))r= link(r);
if(best_ins_ptr(r)==null)wait= true;
else{wait= false;s= last_ins_ptr(r);link(s)= ins_ptr(p);
if(best_ins_ptr(r)==p)
/*1048:*/
#line 19979 "btex.w"

{if(type(r)==split_up)
if((broken_ins(r)==p)&&(broken_ptr(r)!=null))
{while(link(s)!=broken_ptr(r))s= link(s);
link(s)= null;
split_top_skip= split_top_ptr(p);
ins_ptr(p)= prune_page_top(broken_ptr(r));
if(ins_ptr(p)!=null)
{temp_ptr= vpack(ins_ptr(p),natural);
height(p)= height(temp_ptr)+depth(temp_ptr);
free_node(temp_ptr,box_node_size);wait= true;
}
}
#line 1927 "htex.ch"
while(link(s)!=null)s= link(s);
best_ins_ptr(r)= null;
#line 19993 "btex.w"
n= qo(subtype(r));
temp_ptr= list_ptr(box(n));
free_node(box(n),box_node_size);
#line 1934 "htex.ch"
streams[n].p= temp_ptr;
streams[n].t= s;
#line 19997 "btex.w"
}

/*:1048*/
#line 19970 "btex.w"

else{while(link(s)!=null)s= link(s);
last_ins_ptr(r)= s;
}
}
/*1049:*/
#line 19999 "btex.w"

link(prev_p)= link(p);link(p)= null;
if(wait)
{link(q)= p;q= p;incr(insert_penalties);
}
else{delete_glue_ref(split_top_ptr(p));
free_node(p,ins_node_size);
}
p= prev_p

/*:1049*/
#line 19976 "btex.w"
;
}

/*:1047*/
#line 1894 "htex.ch"
;
}
prev_p= p;p= link(prev_p);
}
split_top_skip= save_split_top_skip;
/*1044:*/
#line 19912 "btex.w"

if(p!=null)
{if(link(contrib_head)==null)
if(nest_ptr==0)tail= page_tail;
else contrib_tail= page_tail;
link(page_tail)= link(contrib_head);
link(contrib_head)= p;
link(prev_p)= null;
}
#line 1916 "htex.ch"
streams[0].p= link(page_head);link(page_head)= null;page_tail= page_head;
streams[0].t= prev_p;
if(q!=hold_head)
{link(q)= link(contrib_head);
link(contrib_head)= link(hold_head);
}
#line 19930 "btex.w"

/*:1044*/
#line 1900 "htex.ch"
;
/*1046:*/
#line 19951 "btex.w"

r= link(page_ins_head);
while(r!=page_ins_head)
{q= link(r);free_node(r,page_ins_node_size);r= q;
}
link(page_ins_head)= page_ins_head

/*:1046*/
#line 1901 "htex.ch"
;
}
#line 19882 "btex.w"

/*:1041*//*1173:*/
#line 1951 "htex.ch"

void hdisplay(pointer p,pointer a,bool l)
{
scaled x;
uint32_t offset= node_pos-node_pos1;
{scaled w;
scaled l;
scaled s;
pointer p;
pointer q;
internal_font_number f;
int n;
scaled v;
scaled d;

if(head==tail)
{pop_nest();w= -max_dimen;x= cur_list.hs_field;offset= 0;
}
else{pointer par_ptr= link(head);
pop_nest();
store_map(par_ptr,node_pos,0);
line_break(display_widow_penalty,par_ptr);
x= cur_list.hs_field;
#line 21876 "btex.w"
/*1174:*/
#line 21891 "btex.w"

#line 1991 "htex.ch"
v= shift_amount(just_box)+2*dimen_def[quad_no];w= -max_dimen;
#line 21893 "btex.w"
p= list_ptr(just_box);
while(p!=null)
{/*1175:*/
#line 21908 "btex.w"

reswitch:if(is_char_node(p))
{f= font(p);d= char_width(f,char_info(f,character(p)));
goto found;
}
switch(type(p)){
case hlist_node:case vlist_node:case rule_node:{d= width(p);goto found;
}
case ligature_node:/*665:*/
#line 13033 "btex.w"

{mem[lig_trick]= mem[lig_char(p)];link(lig_trick)= link(p);
p= lig_trick;goto reswitch;
}

/*:665*/
#line 21916 "btex.w"

case kern_node:case math_node:d= width(p);break;
case glue_node:/*1176:*/
#line 21930 "btex.w"

{q= glue_ptr(p);d= width(q);
if(glue_sign(just_box)==stretching)
{if((glue_order(just_box)==stretch_order(q))&&
(stretch(q)!=0))
v= max_dimen;
}
else if(glue_sign(just_box)==shrinking)
{if((glue_order(just_box)==shrink_order(q))&&
(shrink(q)!=0))
v= max_dimen;
}
if(subtype(p)>=a_leaders)goto found;
}

/*:1176*/
#line 21919 "btex.w"
break;
case whatsit_node:/*1390:*/
#line 24915 "btex.w"
d= 0

#line 2624 "htex.ch"
/*:1390*/
#line 21920 "btex.w"
;break;
default:d= 0;
}

/*:1175*/
#line 21896 "btex.w"
;
if(v<max_dimen)v= v+d;
goto not_found;
found:if(v<max_dimen)
{v= v+d;w= v;
}
else{w= max_dimen;goto done;
}
not_found:p= link(p);
}
done:

/*:1174*/
#line 21878 "btex.w"
;
}

/*1177:*/
#line 21948 "btex.w"

if(par_shape_ptr==null)
if((hang_indent!=0)&&
(((hang_after>=0)&&(prev_graf+2> hang_after))||
(prev_graf+1<-hang_after)))
#line 1997 "htex.ch"
{l= x-abs(hang_indent);
#line 21954 "btex.w"
if(hang_indent> 0)s= hang_indent;else s= 0;
}
#line 2003 "htex.ch"
else{l= x;s= 0;
#line 21957 "btex.w"
}
else{n= info(par_shape_ptr);
if(prev_graf+2>=n)p= par_shape_ptr+2*n;
else p= par_shape_ptr+2*(prev_graf+2);
s= mem[p-1].sc;l= mem[p].sc;
}

/*:1177*/
#line 21881 "btex.w"
;
#line 1985 "htex.ch"
pre_display_size= w;display_width= l;display_indent= s;
#line 21889 "btex.w"
}

/*:1173*//*1228:*/
#line 2015 "htex.ch"

{/*1226:*/
#line 22598 "btex.w"

pointer b;
scaled w;
scaled z;
scaled e;
scaled q;
scaled d;
scaled s;
small_number g1,g2;
pointer r;
pointer t;

/*:1226*/
#line 2016 "htex.ch"

adjust_tail= adjust_head;b= hpack(p,natural);p= list_ptr(b);
t= adjust_tail;adjust_tail= null;
w= width(b);z= display_width;s= display_indent;
#line 2025 "htex.ch"
if(a==null)
#line 22621 "btex.w"
{e= 0;q= 0;
}
#line 2031 "htex.ch"
else{e= width(a);q= e+math_quad;
#line 22624 "btex.w"
}
if(w+q> z)
/*1230:*/
#line 22650 "btex.w"

{if((e!=0)&&((w-total_shrink[normal]+q<=z)||
(total_shrink[fil]!=0)||(total_shrink[fill]!=0)||
(total_shrink[filll]!=0)))
{free_node(b,box_node_size);
b= hpack(p,z-q,exactly);
}
else{e= 0;
if(w> z)
{free_node(b,box_node_size);
b= hpack(p,z,exactly);
}
}
w= width(b);
}

/*:1230*/
#line 22627 "btex.w"
;
/*1231:*/
#line 22674 "btex.w"

d= half(z-w);
if((e> 0)&&(d<2*e))
{d= half(z-w-e);
if(p!=null)if(!is_char_node(p))if(type(p)==glue_node)d= 0;
}

/*:1231*/
#line 22629 "btex.w"
;
/*1232:*/
#line 22687 "btex.w"

#line 2058 "htex.ch"
tail_append(new_penalty(pre_display_penalty));
store_map(tail,node_pos,offset);
if((d+s<=pre_display_size)||l)
{g1= above_display_skip_no;g2= below_display_skip_no;
}
else{g1= above_display_short_skip_no;
g2= below_display_short_skip_no;
}
if(l&&(e==0))
{shift_amount(a)= s;append_to_vlist(a,offset);
tail_append(new_penalty(inf_penalty));store_map(tail,node_pos,offset);
}
else{tail_append(new_glue(pointer_def[glue_kind][g1]));store_map(tail,node_pos,offset);}
#line 22700 "btex.w"

/*:1232*/
#line 22630 "btex.w"
;
/*1233:*/
#line 22701 "btex.w"

if(e!=0)
{r= new_kern(z-w-e-d);
if(l)
{link(a)= r;link(r)= b;b= a;d= 0;
}
else{link(b)= r;link(r)= a;
}
b= hpack(b,natural);
}
#line 2076 "htex.ch"
shift_amount(b)= s+d;append_to_vlist(b,offset)
#line 22712 "btex.w"

/*:1233*/
#line 22631 "btex.w"
;
/*1234:*/
#line 22713 "btex.w"

if((a!=null)&&(e==0)&&!l)
{tail_append(new_penalty(inf_penalty));
shift_amount(a)= s+z-width(a);
#line 2082 "htex.ch"
append_to_vlist(a,offset);
#line 22718 "btex.w"
g2= 0;
}
if(t!=adjust_head)
{link(tail)= link(adjust_head);tail= t;
}
#line 2089 "htex.ch"
tail_append(new_penalty(post_display_penalty));
offset= (hpos-hstart)+1-node_pos;
store_map(tail,node_pos,offset);
if(g2> 0){tail_append(new_glue(pointer_def[glue_kind][g2]));store_map(tail,node_pos,offset);}
#line 22725 "btex.w"

/*:1234*/
#line 22632 "btex.w"
;
#line 2037 "htex.ch"
prev_graf= prev_graf+3;
cur_list.bs_pos= hpos+node_pos;
push_nest();mode= hmode;
}
}
#line 22634 "btex.w"

/*:1228*/
