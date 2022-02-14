/*4:*/
#line 255 "htex.w"

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
#ifdef STAT
#include "hprint.h"
#endif
/*121:*/
#line 2578 "htex.w"

pointer temp_ptr;

/*:121*//*122:*/
#line 2610 "htex.w"

memory_word mem0[mem_max-mem_min+1],*const mem= mem0-mem_min;
pointer lo_mem_max;
pointer hi_mem_min;

/*:122*//*123:*/
#line 2623 "htex.w"

static int var_used,dyn_used;
#ifdef STAT
#define incr_dyn_used incr(dyn_used)
#define decr_dyn_used decr(dyn_used)
#else
#define incr_dyn_used
#define decr_dyn_used
#endif

/*:123*//*124:*/
#line 2647 "htex.w"

static pointer avail;

/*:124*//*130:*/
#line 2750 "htex.w"

static pointer rover;

/*:130*//*659:*/
#line 13058 "htex.w"

static scaled total_stretch0[filll-normal+1],
*const total_stretch= total_stretch0-normal,
total_shrink0[filll-normal+1],*const total_shrink= total_shrink0-normal;


/*:659*//*661:*/
#line 13070 "htex.w"

pointer adjust_tail= null;

/*:661*//*830:*/
#line 16195 "htex.w"

pointer just_box;

/*:830*//*838:*/
#line 16332 "htex.w"

static pointer passive;
static pointer printed_node;
static halfword pass_number;

/*:838*//*841:*/
#line 16372 "htex.w"

static scaled active_width0[6],*const active_width= active_width0-1;

static scaled cur_active_width0[6],
*const cur_active_width= cur_active_width0-1;
static scaled background0[6],*const background= background0-1;
static scaled break_width0[6],*const break_width= break_width0-1;

/*:841*//*844:*/
#line 16431 "htex.w"

static bool no_shrink_error_yet;

/*:844*//*848:*/
#line 16474 "htex.w"

static pointer cur_p;
static bool second_pass;
static bool final_pass;
static int threshold;

/*:848*//*854:*/
#line 16576 "htex.w"

static int minimal_demerits0[tight_fit-very_loose_fit+1],
*const minimal_demerits= minimal_demerits0-very_loose_fit;

static int minimum_demerits;

static pointer best_place0[tight_fit-very_loose_fit+1],
*const best_place= best_place0-very_loose_fit;

static halfword best_pl_line0[tight_fit-very_loose_fit+1],
*const best_pl_line= best_pl_line0-very_loose_fit;


/*:854*//*861:*/
#line 16698 "htex.w"

static scaled disc_width;

/*:861*//*870:*/
#line 16841 "htex.w"

static halfword easy_line;
static halfword last_special_line;

static scaled first_width;

static scaled second_width;
static scaled first_indent;
static scaled second_indent;

/*:870*//*896:*/
#line 17347 "htex.w"

static pointer best_bet;
static int fewest_demerits;
static halfword best_line;
static int actual_looseness;

static int line_diff;


/*:896*//*997:*/
#line 19151 "htex.w"

scaled best_height_plus_depth;


/*:997*//*1006:*/
#line 19353 "htex.w"

pointer page_tail;
int page_contents;
scaled page_max_depth;
pointer best_page_break;
int least_page_cost;
scaled best_size;

/*:1006*//*1008:*/
#line 19435 "htex.w"

scaled page_so_far[8];
int insert_penalties;


/*:1008*/
#line 271 "htex.w"


/*98:*/
#line 2100 "htex.w"

void overflow(char*s,int n)
{QUIT("Capacity exceeded, sorry [%s=%d=0x%X]\n",s,n,n);
}

/*:98*//*99:*/
#line 2113 "htex.w"

void confusion(char*s)

{QUIT("This can't happen(%s)",s);}

/*:99*/
#line 273 "htex.w"

#endif

/*:4*//*104:*/
#line 2180 "htex.w"


static int half(int x)
{if(odd(x))return(x+1)/2;
else return x/2;
}

/*:104*//*109:*/
#line 2278 "htex.w"

static bool arith_error;
static scaled rem;

/*:109*//*111:*/
#line 2302 "htex.w"

static scaled x_over_n(scaled x,int n)
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

/*:111*//*112:*/
#line 2329 "htex.w"

static scaled xn_over_d(scaled x,int n,int d)
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

/*:112*//*113:*/
#line 2369 "htex.w"

halfword badness(scaled t,scaled s)
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

/*:113*//*126:*/
#line 2666 "htex.w"

static pointer get_avail(void)
{pointer p;
p= avail;
if(p!=null)avail= link(avail);
else{decr(hi_mem_min);p= hi_mem_min;
if(hi_mem_min<=lo_mem_max)
{
overflow("main memory size",mem_max+1-mem_min);


}
}
link(p)= null;
#ifdef STAT
incr(dyn_used);
#endif

return p;
}

/*:126*//*129:*/
#line 2713 "htex.w"

static void flush_list(pointer p)

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

/*:129*//*131:*/
#line 2761 "htex.w"

pointer get_node(int s)
{
pointer p;
pointer q;
int r;
int t;
restart:p= rover;
do{/*133:*/
#line 2813 "htex.w"

q= p+node_size(p);

while(is_empty(q))
{t= rlink(q);
if(q==rover)rover= t;
llink(t)= llink(q);rlink(llink(q))= t;
q= q+node_size(q);
}
r= q-s;
if(r> p+1)/*134:*/
#line 2828 "htex.w"

{node_size(p)= r-p;

rover= p;
goto found;
}

/*:134*/
#line 2823 "htex.w"
;
if(r==p)if(rlink(p)!=p)
/*135:*/
#line 2837 "htex.w"

{rover= rlink(p);t= llink(p);
llink(rover)= t;rlink(t)= rover;
goto found;
}

/*:135*/
#line 2825 "htex.w"
;
node_size(p)= q-p

/*:133*/
#line 2770 "htex.w"
;

p= rlink(p);
}while(!(p==rover));
if(s==010000000000)
{return max_halfword;
}
if(lo_mem_max+2<hi_mem_min)if(lo_mem_max+2<=mem_bot+max_halfword)
/*132:*/
#line 2798 "htex.w"

{if(hi_mem_min-lo_mem_max>=1998)t= lo_mem_max+1000;
else t= lo_mem_max+1+(hi_mem_min-lo_mem_max)/2;

p= llink(rover);q= lo_mem_max;rlink(p)= q;llink(rover)= q;
if(t> mem_bot+max_halfword)t= mem_bot+max_halfword;
rlink(q)= rover;llink(q)= p;link(q)= empty_flag;node_size(q)= t-lo_mem_max;
lo_mem_max= t;link(lo_mem_max)= null;info(lo_mem_max)= null;
rover= q;goto restart;
}

/*:132*/
#line 2778 "htex.w"
;
overflow("main memory size",mem_max+1-mem_min);


found:link(r)= null;
#ifdef STAT
var_used= var_used+s;
#endif

leak_in(r,s);
return r;
}

/*:131*//*136:*/
#line 2848 "htex.w"

static void free_node(pointer p,halfword s)

{pointer q;
leak_out(p,s);
store_map(p,0,0);
node_size(p)= s;link(p)= empty_flag;
q= llink(rover);llink(p)= q;rlink(p)= rover;
llink(rover)= p;rlink(q)= p;
#ifdef STAT
var_used= var_used-s;
#endif

}

/*:136*//*142:*/
#line 2999 "htex.w"

pointer new_null_box(void)
{pointer p;
p= get_node(box_node_size);type(p)= hlist_node;
subtype(p)= min_quarterword;
width(p)= 0;depth(p)= 0;height(p)= 0;shift_amount(p)= 0;list_ptr(p)= null;
glue_sign(p)= normal;glue_order(p)= normal;set_glue_ratio_zero(glue_set(p));
return p;
}

/*:142*//*145:*/
#line 3030 "htex.w"

pointer new_rule(void)
{pointer p;
p= get_node(rule_node_size);type(p)= rule_node;
subtype(p)= 0;
width(p)= null_flag;depth(p)= null_flag;height(p)= null_flag;
return p;
}

/*:145*//*150:*/
#line 3100 "htex.w"

pointer new_ligature(quarterword f,quarterword c,pointer q)
{pointer p;
p= get_node(small_node_size);type(p)= ligature_node;
font(lig_char(p))= f;character(lig_char(p))= c;lig_ptr(p)= q;
subtype(p)= 0;return p;
}


/*:150*//*151:*/
#line 3134 "htex.w"

pointer new_disc(void)
{pointer p;
p= get_node(small_node_size);type(p)= disc_node;
subtype(p)= 0;pre_break(p)= null;post_break(p)= null;
return p;
}

/*:151*//*153:*/
#line 3169 "htex.w"

pointer new_math(scaled w,small_number s)
{pointer p;
p= get_node(small_node_size);type(p)= math_node;
subtype(p)= s;width(p)= w;return p;
}

/*:153*//*157:*/
#line 3252 "htex.w"

pointer new_spec(pointer p)
{pointer q;
q= get_node(glue_spec_size);
mem[q]= mem[p];glue_ref_count(q)= null;
width(q)= width(p);stretch(q)= stretch(p);shrink(q)= shrink(p);
return q;
}

/*:157*//*159:*/
#line 3279 "htex.w"

pointer new_glue(pointer q)
{pointer p;
p= get_node(small_node_size);type(p)= glue_node;subtype(p)= normal;
leader_ptr(p)= null;glue_ptr(p)= q;incr(glue_ref_count(q));
return p;
}

/*:159*//*162:*/
#line 3318 "htex.w"

pointer new_kern(scaled w)
{pointer p;
p= get_node(small_node_size);type(p)= kern_node;
subtype(p)= normal;
width(p)= w;
return p;
}

/*:162*//*164:*/
#line 3342 "htex.w"

pointer new_penalty(int m)
{pointer p;
p= get_node(small_node_size);type(p)= penalty_node;
subtype(p)= 0;
penalty(p)= m;return p;
}

/*:164*//*170:*/
#line 3422 "htex.w"

void mem_init(void)
{int k;
/*171:*/
#line 3428 "htex.w"

for(k= mem_bot+1;k<=lo_mem_stat_max;k++)mem[k].sc= 0;


k= mem_bot;while(k<=lo_mem_stat_max)

{glue_ref_count(k)= null+1;
stretch_order(k)= normal;shrink_order(k)= normal;
k= k+glue_spec_size;
}
rover= lo_mem_stat_max+1;
link(rover)= empty_flag;
node_size(rover)= 1000;
llink(rover)= rover;rlink(rover)= rover;
lo_mem_max= rover+1000;link(lo_mem_max)= null;info(lo_mem_max)= null;
for(k= hi_mem_stat_min;k<=mem_top;k++)
mem[k]= mem[lo_mem_max];
/*805:*/
#line 15738 "htex.w"

info(omit_template)= end_template_token;

/*:805*//*812:*/
#line 15845 "htex.w"

link(end_span)= max_quarterword+1;info(end_span)= null;

/*:812*//*836:*/
#line 16301 "htex.w"

type(last_active)= hyphenated;line_number(last_active)= max_halfword;
subtype(last_active)= 0;

/*:836*//*1007:*/
#line 19406 "htex.w"

subtype(page_ins_head)= qi(255);
type(page_ins_head)= split_up;link(page_ins_head)= page_ins_head;

/*:1007*//*1014:*/
#line 19562 "htex.w"

type(page_head)= glue_node;subtype(page_head)= normal;

/*:1014*/
#line 3445 "htex.w"
;
avail= null;
hi_mem_min= hi_mem_stat_min;
var_used= lo_mem_stat_max+1-mem_bot;dyn_used= hi_mem_stat_usage;


/*:171*/
#line 3425 "htex.w"

}

/*:170*//*210:*/
#line 3978 "htex.w"

static void delete_token_ref(pointer p)

{if(token_ref_count(p)==null)flush_list(p);
else decr(token_ref_count(p));
}

/*:210*//*211:*/
#line 3993 "htex.w"

void delete_glue_ref(pointer p)
fast_delete_glue_ref(p)
static void delete_xdimen_ref(pointer p)
{if(xdimen_ref_count(p)==null)free_node(p,xdimen_node_size);
else decr(xdimen_ref_count(p));
}

/*:211*//*212:*/
#line 4006 "htex.w"

void flush_node_list(pointer p)
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
case whatsit_node:/*1388:*/
#line 25389 "htex.w"

{switch(subtype(p)){
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
default:confusion("ext3");

}
goto done;
}

/*:1388*/
#line 4025 "htex.w"

case glue_node:{fast_delete_glue_ref(glue_ptr(p));
if(leader_ptr(p)!=null)flush_node_list(leader_ptr(p));
}break;
case kern_node:case math_node:case penalty_node:do_nothing;break;
case ligature_node:flush_node_list(lig_ptr(p));break;
case disc_node:{flush_node_list(pre_break(p));
flush_node_list(post_break(p));
}break;
case adjust_node:flush_node_list(adjust_ptr(p));break;
default:QUIT("Confusion while flushing node list");

}
free_node(p,small_node_size);
done:;}
p= q;
}
}

/*:212*//*214:*/
#line 4069 "htex.w"

pointer copy_node_list(pointer p)

{pointer h;
pointer q;
pointer r;
int words;
h= get_avail();q= h;
while(p!=null)
{/*215:*/
#line 4085 "htex.w"

words= 1;
if(is_char_node(p))r= get_avail();
else/*216:*/
#line 4094 "htex.w"

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
case whatsit_node:/*1387:*/
#line 25271 "htex.w"

switch(subtype(p)){
case open_node:{r= get_node(open_node_size);words= open_node_size;
}break;
case write_node:case special_node:{r= get_node(write_node_size);
add_token_ref(write_tokens(p));words= write_node_size;
}break;
case close_node:case language_node:{r= get_node(small_node_size);
words= small_node_size;
}break;
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

}

/*:1387*/
#line 4109 "htex.w"
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

/*:216*/
#line 4089 "htex.w"
;
while(words> 0)
{decr(words);mem[r+words]= mem[p+words];
}

/*:215*/
#line 4078 "htex.w"
;
link(q)= r;q= r;p= link(p);
}
link(q)= null;q= link(h);free_avail(h);
return q;
}

/*:214*//*225:*/
#line 4459 "htex.w"


/*223:*/
#line 4440 "htex.w"

static list_state_record nest[nest_size+1];
int nest_ptr;
static int max_nest_stack;
list_state_record cur_list;

/*:223*/
#line 4461 "htex.w"


void list_init(void)
{
nest_ptr= 0;max_nest_stack= 0;
memset(&cur_list,0,sizeof(cur_list));
mode= vmode;head= contrib_head;tail= contrib_head;
prev_height= prev_depth= ignore_depth;
}
/*:225*//*227:*/
#line 4479 "htex.w"

void push_nest(void)
{if(nest_ptr> max_nest_stack)
{max_nest_stack= nest_ptr;
if(nest_ptr==nest_size)overflow("semantic nest size",nest_size);

}
nest[nest_ptr]= cur_list;
incr(nest_ptr);head= get_avail();tail= head;prev_graf= 0;
cur_list.bs_pos= NULL;cur_bs= baseline_skip;cur_ls= line_skip;cur_lsl= line_skip_limit;
}

/*:227*//*228:*/
#line 4496 "htex.w"

void pop_nest(void)
{free_avail(head);decr(nest_ptr);cur_list= nest[nest_ptr];
}

/*:228*//*561:*/
#line 10872 "htex.w"

memory_word font_info[font_mem_size+1];

static font_index fmem_ptr= 0;

void hclear_fonts(void)
{fmem_ptr= 0;
}
static internal_font_number font_ptr;
static four_quarters font_check0[font_max-font_base+1],
*const font_check= font_check0-font_base;
scaled font_size0[font_max-font_base+1],
*const font_size= font_size0-font_base;
static scaled font_dsize0[font_max-font_base+1],
*const font_dsize= font_dsize0-font_base;
static font_index font_params0[font_max-font_base+1],
*const font_params= font_params0-font_base;

char*font_name0[font_max-font_base+1],
**const font_name= font_name0-font_base;
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


/*:561*//*562:*/
#line 10930 "htex.w"

int char_base0[font_max-font_base+1],
*const char_base= char_base0-font_base;

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


/*:562*//*572:*/
#line 11085 "htex.w"

void read_font_info(int f,char*nom,scaled s)
{
int k;
bool file_opened;
halfword lf,lh,bc,ec,nw,nh,nd,ni,nl,nk,ne,np;

eight_bits a,b,c,d;
four_quarters qw;scaled sw;
int bch_label;
int bchar;
scaled z;
int alpha;int beta;

/*574:*/
#line 11134 "htex.w"

/*575:*/
#line 11146 "htex.w"

file_opened= true

/*:575*/
#line 11135 "htex.w"
;
/*577:*/
#line 11170 "htex.w"

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

/*:577*/
#line 11136 "htex.w"
;
/*578:*/
#line 11197 "htex.w"

lf= lf-6-lh;
if(np<7)lf= lf+7-np;
if((font_ptr==font_max)||(fmem_ptr+lf> font_mem_size))
QUIT("Not enough room left for font %s\n",nom);
char_base[f]= fmem_ptr-bc;
width_base[f]= char_base[f]+ec+1;
height_base[f]= width_base[f]+nw;
depth_base[f]= height_base[f]+nh;
italic_base[f]= depth_base[f]+nd;
lig_kern_base[f]= italic_base[f]+ni;
kern_base[f]= lig_kern_base[f]+nl-kern_base_offset;
exten_base[f]= kern_base[f]+kern_base_offset+nk;
param_base[f]= exten_base[f]+ne

/*:578*/
#line 11137 "htex.w"
;
/*580:*/
#line 11225 "htex.w"

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

/*:580*/
#line 11138 "htex.w"
;
/*581:*/
#line 11241 "htex.w"

for(k= fmem_ptr;k<=width_base[f]-1;k++)
{store_four_quarters(font_info[k].qqqq);
if((a>=nw)||(b/020>=nh)||(b%020>=nd)||
(c/4>=ni))abort;
switch(c%4){
case lig_tag:if(d>=nl)abort;break;
case ext_tag:if(d>=ne)abort;break;
case list_tag:/*582:*/
#line 11262 "htex.w"

{check_byte_range(d);
while(d<current_character_being_worked_on)
{qw= char_info(f,d);

if(char_tag(qw)!=list_tag)goto not_found;
d= qo(rem_byte(qw));
}
if(d==current_character_being_worked_on)abort;
not_found:;}

/*:582*/
#line 11249 "htex.w"
break;
default:do_nothing;
}
}

/*:581*/
#line 11139 "htex.w"
;
/*583:*/
#line 11297 "htex.w"

{/*584:*/
#line 11307 "htex.w"

{alpha= 16;
while(z>=040000000)
{z= z/2;alpha= alpha+alpha;
}
beta= 256/alpha;alpha= alpha*z;
}

/*:584*/
#line 11298 "htex.w"
;
for(k= width_base[f];k<=lig_kern_base[f]-1;k++)
store_scaled(font_info[k].sc);
if(font_info[width_base[f]].sc!=0)abort;
if(font_info[height_base[f]].sc!=0)abort;
if(font_info[depth_base[f]].sc!=0)abort;
if(font_info[italic_base[f]].sc!=0)abort;
}

/*:583*/
#line 11140 "htex.w"
;
/*585:*/
#line 11321 "htex.w"

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

/*:585*/
#line 11141 "htex.w"
;
/*586:*/
#line 11341 "htex.w"

for(k= exten_base[f];k<=param_base[f]-1;k++)
{store_four_quarters(font_info[k].qqqq);
if(a!=0)check_existence(a);
if(b!=0)check_existence(b);
if(c!=0)check_existence(c);
check_existence(d);
}

/*:586*/
#line 11142 "htex.w"
;
/*587:*/
#line 11353 "htex.w"

{for(k= 1;k<=np;k++)
if(k==1)
{fget;sw= fbyte;if(sw> 127)sw= sw-256;
fget;sw= sw*0400+fbyte;fget;sw= sw*0400+fbyte;
fget;font_info[param_base[f]].sc= 
(sw*020)+(fbyte/020);
}
else store_scaled(font_info[param_base[f]+k-1].sc);
if(hpos>=hend)abort;
for(k= np+1;k<=7;k++)font_info[param_base[f]+k-1].sc= 0;
}

/*:587*/
#line 11143 "htex.w"
;
/*588:*/
#line 11373 "htex.w"

if(np>=7)font_params[f]= np;else font_params[f]= 7;
hyphen_char[f]= skew_char[f]= -1;
if(bch_label<nl)bchar_label[f]= bch_label+lig_kern_base[f];
else bchar_label[f]= non_address;
font_bchar[f]= qi(bchar);
font_false_bchar[f]= qi(bchar);
if(bchar<=ec)if(bchar>=bc)
{qw= char_info(f,bchar);
if(char_exists(qw))font_false_bchar[f]= non_char;
}
font_name[f]= nom;
font_bc[f]= bc;font_ec[f]= ec;font_glue[f]= null;
adjust(char_base);adjust(width_base);adjust(lig_kern_base);
adjust(kern_base);adjust(exten_base);
decr(param_base[f]);
fmem_ptr= fmem_ptr+lf;goto done

/*:588*/
#line 11144 "htex.w"


/*:574*/
#line 11101 "htex.w"
;
bad_tfm:QUIT("Bad tfm file: %s\n",nom);
done:;
}

/*:572*//*594:*/
#line 11476 "htex.w"

pointer new_character(internal_font_number f,eight_bits c)
{pointer p;
#ifdef DEBUG
if(font_bc[f]> c||font_ec[f]<c||!char_exists(char_info(f,qi(c))))
DBG(DBGFONT,"Warning: Character 0x%0X in font %d does not exist\n",c,f);
#endif
p= get_avail();font(p)= f;character(p)= qi(c);
return p;
}

/*:594*//*663:*/
#line 13077 "htex.w"

pointer hpack(pointer p,scaled w,small_number m)
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
r= get_node(box_node_size);type(r)= hlist_node;
subtype(r)= min_quarterword;shift_amount(r)= 0;
q= r+list_offset;link(q)= p;
h= 0;/*664:*/
#line 13103 "htex.w"

d= 0;x= 0;
total_stretch[normal]= 0;total_shrink[normal]= 0;
total_stretch[fil]= 0;total_shrink[fil]= 0;
total_stretch[fill]= 0;total_shrink[fill]= 0;
total_stretch[filll]= 0;total_shrink[filll]= 0

/*:664*/
#line 13092 "htex.w"
;
while(p!=null)/*665:*/
#line 13110 "htex.w"


{reswitch:while(is_char_node(p))
/*668:*/
#line 13155 "htex.w"

{f= font(p);i= char_info(f,character(p));hd= height_depth(i);
x= x+char_width(f,i);
s= char_height(f,hd);if(s> h)h= s;
s= char_depth(f,hd);if(s> d)d= s;
p= link(p);
}

/*:668*/
#line 13114 "htex.w"
;
if(p!=null)
{switch(type(p)){
case hlist_node:case vlist_node:case rule_node:
case unset_node:
/*667:*/
#line 13143 "htex.w"

{x= x+width(p);
if(type(p)>=rule_node)s= 0;else s= shift_amount(p);
if(height(p)-s> h)h= height(p)-s;
if(depth(p)+s> d)d= depth(p)+s;
}

/*:667*/
#line 13120 "htex.w"
break;
case ins_node:case mark_node:case adjust_node:if(adjust_tail!=null)
/*669:*/
#line 13170 "htex.w"

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

/*:669*/
#line 13122 "htex.w"
break;
case whatsit_node:/*1390:*/
#line 25471 "htex.w"

if(subtype(p)==image_node)
{glue_ord o;
if(image_height(p)> h)h= image_height(p);
x= x+image_width(p);
o= image_stretch_order(p);total_stretch[o]= total_stretch[o]+image_stretch(p);
o= image_shrink_order(p);total_shrink[o]= total_shrink[o]+image_shrink(p);
}

/*:1390*/
#line 13123 "htex.w"
;break;
case glue_node:/*670:*/
#line 13182 "htex.w"

{g= glue_ptr(p);x= x+width(g);
o= stretch_order(g);total_stretch[o]= total_stretch[o]+stretch(g);
o= shrink_order(g);total_shrink[o]= total_shrink[o]+shrink(g);
if(subtype(p)>=a_leaders)
{g= leader_ptr(p);
if(height(g)> h)h= height(g);
if(depth(g)> d)d= depth(g);
}
}

/*:670*/
#line 13124 "htex.w"
break;
case kern_node:case math_node:x= x+width(p);break;
case ligature_node:/*666:*/
#line 13134 "htex.w"

{mem[lig_trick]= mem[lig_char(p)];link(lig_trick)= link(p);
p= lig_trick;goto reswitch;
}

/*:666*/
#line 13126 "htex.w"

default:do_nothing;
}
p= link(p);
}
}


/*:665*/
#line 13095 "htex.w"
;
if(adjust_tail!=null)link(adjust_tail)= null;
height(r)= h;depth(r)= d;
/*671:*/
#line 13196 "htex.w"

if(m==additional)w= x+w;
width(r)= w;x= w-x;
if(x==0)
{glue_sign(r)= normal;glue_order(r)= normal;
set_glue_ratio_zero(glue_set(r));
goto end;
}
else if(x> 0)/*672:*/
#line 13209 "htex.w"

{/*673:*/
#line 13220 "htex.w"

if(total_stretch[filll]!=0)o= filll;
else if(total_stretch[fill]!=0)o= fill;
else if(total_stretch[fil]!=0)o= fil;
else o= normal

/*:673*/
#line 13210 "htex.w"
;
glue_order(r)= o;glue_sign(r)= stretching;
if(total_stretch[o]!=0)glue_set(r)= unfloat(x/(double)total_stretch[o]);

else{glue_sign(r)= normal;
set_glue_ratio_zero(glue_set(r));
}
goto end;
}

/*:672*/
#line 13205 "htex.w"

else/*678:*/
#line 13265 "htex.w"

{/*679:*/
#line 13278 "htex.w"

if(total_shrink[filll]!=0)o= filll;
else if(total_shrink[fill]!=0)o= fill;
else if(total_shrink[fil]!=0)o= fil;
else o= normal

/*:679*/
#line 13266 "htex.w"
;
glue_order(r)= o;glue_sign(r)= shrinking;
if(total_shrink[o]!=0)glue_set(r)= unfloat((-x)/(double)total_shrink[o]);

else{glue_sign(r)= normal;
set_glue_ratio_zero(glue_set(r));
}
if((total_shrink[o]<-x)&&(o==normal)&&(list_ptr(r)!=null))
set_glue_ratio_one(glue_set(r));
goto end;
}

/*:678*/
#line 13207 "htex.w"


/*:671*/
#line 13099 "htex.w"
;
end:return r;
}

/*:663*//*683:*/
#line 13318 "htex.w"

pointer vpackage(pointer p,scaled h,small_number m,scaled l)
{
pointer r;
scaled w,d,x;
scaled s;
pointer g;
glue_ord o;
r= get_node(box_node_size);type(r)= vlist_node;
subtype(r)= min_quarterword;shift_amount(r)= 0;
list_ptr(r)= p;
w= 0;/*664:*/
#line 13103 "htex.w"

d= 0;x= 0;
total_stretch[normal]= 0;total_shrink[normal]= 0;
total_stretch[fil]= 0;total_shrink[fil]= 0;
total_stretch[fill]= 0;total_shrink[fill]= 0;
total_stretch[filll]= 0;total_shrink[filll]= 0

/*:664*/
#line 13329 "htex.w"
;
while(p!=null)/*684:*/
#line 13342 "htex.w"

{if(is_char_node(p))confusion("vpack");

else switch(type(p)){
case hlist_node:case vlist_node:case rule_node:
case unset_node:
/*685:*/
#line 13359 "htex.w"

{x= x+d+height(p);d= depth(p);
if(type(p)>=rule_node)s= 0;else s= shift_amount(p);
if(width(p)+s> w)w= width(p)+s;
}

/*:685*/
#line 13349 "htex.w"
break;
case whatsit_node:/*1389:*/
#line 25462 "htex.w"

if(subtype(p)==image_node)
{glue_ord o;
if(image_width(p)> w)w= image_width(p);
x= x+d+image_height(p);d= 0;
o= image_stretch_order(p);total_stretch[o]= total_stretch[o]+image_stretch(p);
o= image_shrink_order(p);total_shrink[o]= total_shrink[o]+image_shrink(p);
}

/*:1389*/
#line 13350 "htex.w"
;break;
case glue_node:/*686:*/
#line 13365 "htex.w"

{x= x+d;d= 0;
g= glue_ptr(p);x= x+width(g);
o= stretch_order(g);total_stretch[o]= total_stretch[o]+stretch(g);
o= shrink_order(g);total_shrink[o]= total_shrink[o]+shrink(g);
if(subtype(p)>=a_leaders)
{g= leader_ptr(p);
if(width(g)> w)w= width(g);
}
}

/*:686*/
#line 13351 "htex.w"
break;
case kern_node:{x= x+d+width(p);d= 0;
}break;
default:do_nothing;
}
p= link(p);
}

/*:684*/
#line 13331 "htex.w"
;
width(r)= w;
if(d> l)
{x= x+d-l;depth(r)= l;
}
else depth(r)= d;
/*687:*/
#line 13379 "htex.w"

if(m==additional)h= x+h;
height(r)= h;x= h-x;
if(x==0)
{glue_sign(r)= normal;glue_order(r)= normal;
set_glue_ratio_zero(glue_set(r));
goto end;
}
else if(x> 0)/*688:*/
#line 13392 "htex.w"

{/*673:*/
#line 13220 "htex.w"

if(total_stretch[filll]!=0)o= filll;
else if(total_stretch[fill]!=0)o= fill;
else if(total_stretch[fil]!=0)o= fil;
else o= normal

/*:673*/
#line 13393 "htex.w"
;
glue_order(r)= o;glue_sign(r)= stretching;
if(total_stretch[o]!=0)glue_set(r)= unfloat(x/(double)total_stretch[o]);

else{glue_sign(r)= normal;
set_glue_ratio_zero(glue_set(r));
}
goto end;
}

/*:688*/
#line 13388 "htex.w"

else/*691:*/
#line 13428 "htex.w"

{/*679:*/
#line 13278 "htex.w"

if(total_shrink[filll]!=0)o= filll;
else if(total_shrink[fill]!=0)o= fill;
else if(total_shrink[fil]!=0)o= fil;
else o= normal

/*:679*/
#line 13429 "htex.w"
;
glue_order(r)= o;glue_sign(r)= shrinking;
if(total_shrink[o]!=0)glue_set(r)= unfloat((-x)/(double)total_shrink[o]);

else{glue_sign(r)= normal;
set_glue_ratio_zero(glue_set(r));
}
if((total_shrink[o]<-x)&&(o==normal)&&(list_ptr(r)!=null))
set_glue_ratio_one(glue_set(r));
goto end;
}

/*:691*/
#line 13390 "htex.w"


/*:687*/
#line 13338 "htex.w"
;
end:return r;
}

/*:683*//*831:*/
#line 16203 "htex.w"


/*694:*/
#line 13461 "htex.w"

extern pointer happend_to_vlist(pointer b);
void append_to_vlist(pointer b,uint32_t offset)
{scaled d;
pointer p;
if(prev_depth> ignore_depth)
{d= width(baseline_skip)-prev_depth-height(b);
if(d<line_skip_limit)p= new_glue(line_skip);
else{temp_ptr= new_spec(baseline_skip);
p= new_glue(temp_ptr);glue_ref_count(temp_ptr)= null;
width(temp_ptr)= d;
}
store_map(p,node_pos,offset);
link(tail)= p;tail= p;
}
link(tail)= b;tail= b;prev_depth= depth(b);
}

/*:694*//*845:*/
#line 16434 "htex.w"

static pointer finite_shrink(pointer p)
{pointer q;
QUIT("Infinite glue shrinkage found in a paragraph");
q= new_spec(p);shrink_order(q)= normal;
delete_glue_ref(p);return q;
}

/*:845*//*849:*/
#line 16497 "htex.w"

static void try_break(int pi,small_number break_type)
{
pointer r;
pointer prev_r;
halfword old_l;
bool no_break_yet;
/*850:*/
#line 16525 "htex.w"

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

/*:850*/
#line 16504 "htex.w"

/*851:*/
#line 16544 "htex.w"

if(abs(pi)>=inf_penalty)
if(pi> 0)goto end;
else pi= eject_penalty

/*:851*/
#line 16505 "htex.w"
;
no_break_yet= true;prev_r= active;old_l= 0;
do_all_six(copy_to_cur_active);
loop{resume:r= link(prev_r);
/*852:*/
#line 16554 "htex.w"


if(type(r)==delta_node)
{do_all_six(update_width);
prev_prev_r= prev_r;prev_r= r;goto resume;
}

/*:852*/
#line 16510 "htex.w"
;
/*856:*/
#line 16603 "htex.w"

{l= line_number(r);
if(l> old_l)
{
if((minimum_demerits<awful_bad)&&
((old_l!=easy_line)||(r==last_active)))
/*857:*/
#line 16621 "htex.w"

{if(no_break_yet)/*858:*/
#line 16657 "htex.w"

{no_break_yet= false;do_all_six(set_break_width_to_background);
s= cur_p;
if(break_type> unhyphenated)if(cur_p!=null)
/*862:*/
#line 16701 "htex.w"

{t= replace_count(cur_p);v= cur_p;s= post_break(cur_p);
while(t> 0)
{decr(t);v= link(v);
/*863:*/
#line 16719 "htex.w"

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

/*:863*/
#line 16705 "htex.w"
;
}
while(s!=null)
{/*864:*/
#line 16736 "htex.w"

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

/*:864*/
#line 16708 "htex.w"
;
s= link(s);
}
break_width[1]= break_width[1]+disc_width;
if(post_break(cur_p)==null)s= link(v);

}

/*:862*/
#line 16661 "htex.w"
;
while(s!=null)
{if(is_char_node(s))goto done;
switch(type(s)){
case glue_node:/*859:*/
#line 16676 "htex.w"

{v= glue_ptr(s);break_width[1]= break_width[1]-width(v);
break_width[2+stretch_order(v)]= break_width[2+stretch_order(v)]-stretch(v);
break_width[6]= break_width[6]-shrink(v);
}

/*:859*/
#line 16665 "htex.w"
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

/*:858*/
#line 16622 "htex.w"
;
/*865:*/
#line 16762 "htex.w"

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

/*:865*/
#line 16623 "htex.w"
;
if(abs(adj_demerits)>=awful_bad-minimum_demerits)
minimum_demerits= awful_bad-1;
else minimum_demerits= minimum_demerits+abs(adj_demerits);
for(fit_class= very_loose_fit;fit_class<=tight_fit;fit_class++)
{if(minimal_demerits[fit_class]<=minimum_demerits)
/*867:*/
#line 16792 "htex.w"

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
/*868:*/
#line 16812 "htex.w"

{print_nl("@@");print_int(serial(passive));

print(": line ");print_int(line_number(q)-1);
print_char('.');print_int(fit_class);
if(break_type==hyphenated)print_char('-');
print(" t=");print_int(total_demerits(q));
print(" -> @@");
if(prev_break(passive)==null)print_char('0');
else print_int(serial(prev_break(passive)));
}

/*:868*/
#line 16807 "htex.w"
;
#endif

}

/*:867*/
#line 16629 "htex.w"
;
minimal_demerits[fit_class]= awful_bad;
}
minimum_demerits= awful_bad;
/*866:*/
#line 16781 "htex.w"

if(r!=last_active)
{q= get_node(delta_node_size);link(q)= r;type(q)= delta_node;
subtype(q)= 0;
do_all_six(new_delta_from_break_width);
link(prev_r)= q;prev_prev_r= prev_r;prev_r= q;
}

/*:866*/
#line 16633 "htex.w"
;
}

/*:857*/
#line 16609 "htex.w"
;
if(r==last_active)goto end;
/*873:*/
#line 16886 "htex.w"

if(l> easy_line)
{line_width= second_width;old_l= max_halfword-1;
}
else{old_l= l;
if(l> last_special_line)line_width= second_width;
else if(par_shape_ptr==null)line_width= first_width;
else line_width= mem[par_shape_ptr+2*l].sc;
}

/*:873*/
#line 16611 "htex.w"
;
}
}

/*:856*/
#line 16513 "htex.w"
;
/*874:*/
#line 16906 "htex.w"

{artificial_demerits= false;

shortfall= line_width-cur_active_width[1];
if(shortfall> 0)
/*875:*/
#line 16936 "htex.w"

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

/*:875*/
#line 16912 "htex.w"

else/*876:*/
#line 16955 "htex.w"

{if(-shortfall> cur_active_width[6])b= inf_bad+1;
else b= badness(-shortfall,cur_active_width[6]);
if(b> 12)fit_class= tight_fit;else fit_class= decent_fit;
}

/*:876*/
#line 16914 "htex.w"
;
if((b> inf_bad)||(pi==eject_penalty))
/*877:*/
#line 16970 "htex.w"

{if(final_pass&&(minimum_demerits==awful_bad)&&
(link(r)==last_active)&&
(prev_r==active))
artificial_demerits= true;
else if(b> threshold)goto deactivate;
node_r_stays_active= false;
}

/*:877*/
#line 16917 "htex.w"

else{prev_r= r;
if(b> threshold)goto resume;
node_r_stays_active= true;
}
/*878:*/
#line 16986 "htex.w"

if(artificial_demerits)d= 0;
else/*882:*/
#line 17047 "htex.w"

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

/*:882*/
#line 16988 "htex.w"
;
#ifdef STAT
if(tracing_paragraphs> 0)
/*879:*/
#line 17002 "htex.w"

{if(printed_node!=cur_p)
/*880:*/
#line 17024 "htex.w"

{print_nl("");
if(cur_p==null)short_display(link(printed_node));
else{save_link= link(cur_p);
link(cur_p)= null;print_nl("");short_display(link(printed_node));
link(cur_p)= save_link;
}
printed_node= cur_p;
}

/*:880*/
#line 17004 "htex.w"
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

/*:879*/
#line 16991 "htex.w"
;
#endif

d= d+total_demerits(r);

if(d<=minimal_demerits[fit_class])
{minimal_demerits[fit_class]= d;
best_place[fit_class]= break_node(r);best_pl_line[fit_class]= l;
if(d<minimum_demerits)minimum_demerits= d;
}

/*:878*/
#line 16922 "htex.w"
;
if(node_r_stays_active)goto resume;
deactivate:/*883:*/
#line 17069 "htex.w"

link(prev_r)= link(r);free_node(r,active_node_size);
if(prev_r==active)/*884:*/
#line 17094 "htex.w"

{r= link(active);
if(type(r)==delta_node)
{do_all_six(update_active);
do_all_six(copy_to_cur_active);
link(active)= link(r);free_node(r,delta_node_size);
}
}

/*:884*/
#line 17072 "htex.w"

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

/*:883*/
#line 16924 "htex.w"
;
}

/*:874*/
#line 16516 "htex.w"
;
}
end:;
#ifdef STAT
/*881:*/
#line 17039 "htex.w"

if(cur_p==printed_node)if(cur_p!=null)if(type(cur_p)==disc_node)
{t= replace_count(cur_p);
while(t> 0)
{decr(t);printed_node= link(printed_node);
}
}

/*:881*/
#line 16520 "htex.w"
;
#endif

}

/*:849*//*901:*/
#line 17418 "htex.w"

static void post_line_break(int final_widow_penalty)
{
pointer q,r,s;
bool disc_break;
bool post_disc_break;
bool first_line= true;
uint32_t line_offset,next_offset;
scaled cur_width;
scaled cur_indent;
quarterword t;
int pen;
halfword cur_line;
/*902:*/
#line 17469 "htex.w"

q= break_node(best_bet);cur_p= null;
do{r= q;q= prev_break(q);next_break(r)= cur_p;cur_p= r;
}while(!(q==null))

/*:902*/
#line 17432 "htex.w"
;
cur_line= prev_graf+1;
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
/*904:*/
#line 17506 "htex.w"

/*905:*/
#line 17518 "htex.w"

q= cur_break(cur_p);disc_break= false;post_disc_break= false;
if(q!=null)
if(type(q)==glue_node)
{delete_glue_ref(glue_ptr(q));
glue_ptr(q)= right_skip;
subtype(q)= right_skip_code+1;add_glue_ref(right_skip);
goto done;
}
else{if(type(q)==disc_node)
/*906:*/
#line 17537 "htex.w"

{pointer pre_q= pre_break(q);
pointer post_q= post_break(q);
t= replace_count(q);
type(q)= whatsit_node;
subtype(q)= ignore_node;
ignore_info(q)= 1;
/*907:*/
#line 17556 "htex.w"

if(t==0){ignore_list(q)= null;r= link(q);}
else{r= q;
while(t> 1)
{r= link(r);decr(t);
}
s= link(r);
r= link(s);link(s)= null;
ignore_list(q)= link(q);
}

/*:907*/
#line 17544 "htex.w"
;
s= get_node(ignore_node_size);
type(s)= whatsit_node;
subtype(s)= ignore_node;
ignore_info(s)= 0;
ignore_list(s)= null;
link(s)= r;r= s;
if(post_q!=null)/*908:*/
#line 17570 "htex.w"

{s= post_q;
while(link(s)!=null)s= link(s);
link(s)= r;r= post_q;post_disc_break= true;
}

/*:908*/
#line 17551 "htex.w"
;
if(pre_q!=null)/*909:*/
#line 17579 "htex.w"

{s= pre_q;link(q)= s;
while(link(s)!=null)s= link(s);
q= s;
}

/*:909*/
#line 17552 "htex.w"
;
link(q)= r;disc_break= true;
}

/*:906*/
#line 17528 "htex.w"

else if((type(q)==math_node)||(type(q)==kern_node))width(q)= 0;
}
else{q= temp_head;
while(link(q)!=null)q= link(q);
}
/*910:*/
#line 17585 "htex.w"

r= new_glue(right_skip);link(r)= link(q);link(q)= r;q= r

/*:910*/
#line 17534 "htex.w"
;
done:

/*:905*/
#line 17508 "htex.w"
;
/*911:*/
#line 17592 "htex.w"

r= link(q);link(q)= null;q= link(temp_head);link(temp_head)= r;
if(left_skip!=zero_glue)
{r= new_glue(left_skip);
link(r)= q;q= r;
}

/*:911*/
#line 17509 "htex.w"
;
/*913:*/
#line 17618 "htex.w"

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

/*:913*/
#line 17510 "htex.w"
;
/*912:*/
#line 17599 "htex.w"

if(first_line)
{pointer p= happend_to_vlist(just_box);
uint32_t pos= hposition(p);
store_map(p,pos,line_offset);
first_line= false;
}
else
append_to_vlist(just_box,line_offset);
if(adjust_head!=adjust_tail)
{link(tail)= link(adjust_head);tail= adjust_tail;
}
adjust_tail= null

/*:912*/
#line 17512 "htex.w"
;
/*914:*/
#line 17638 "htex.w"

if(cur_line+1!=best_line)
{pen= inter_line_penalty;
if(cur_line==prev_graf+1)pen= pen+club_penalty;
if(cur_line+2==best_line)pen= pen+final_widow_penalty;
if(disc_break)pen= pen+broken_penalty;
if(pen!=0)
{r= new_penalty(pen);store_map(r,node_pos,next_offset);
link(tail)= r;tail= r;
}
}

/*:914*/
#line 17513 "htex.w"


/*:904*/
#line 17452 "htex.w"
;
incr(cur_line);cur_p= next_break(cur_p);
if(cur_p!=null)if(!post_disc_break)
/*903:*/
#line 17481 "htex.w"

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

/*:903*/
#line 17455 "htex.w"
;
}while(!(cur_p==null));
if((cur_line!=best_line)||(link(temp_head)!=null))
confusion("line breaking");

prev_graf= best_line-1;
}

/*:901*//*919:*/
#line 17750 "htex.w"

/*:919*//*967:*/
#line 18573 "htex.w"

#ifdef INIT
#endif

/*:967*/
#line 16205 "htex.w"


void line_break(int final_widow_penalty,pointer par_ptr)
{scaled x= cur_list.hs_field;
/*885:*/
#line 17123 "htex.w"

bool auto_breaking;
pointer prev_p;
pointer q,r,s;
internal_font_number f;

/*:885*//*917:*/
#line 17733 "htex.w"


/*:917*/
#line 16209 "htex.w"

set_line_break_params();
/*832:*/
#line 16230 "htex.w"

link(temp_head)= par_ptr;

/*:832*//*846:*/
#line 16442 "htex.w"

no_shrink_error_yet= true;
check_shrinkage(left_skip);check_shrinkage(right_skip);
q= left_skip;r= right_skip;background[1]= width(q)+width(r);
background[2]= 0;background[3]= 0;background[4]= 0;background[5]= 0;
background[2+stretch_order(q)]= stretch(q);
background[2+stretch_order(r)]= background[2+stretch_order(r)]+stretch(r);
background[6]= shrink(q)+shrink(r);

/*:846*//*855:*/
#line 16589 "htex.w"

minimum_demerits= awful_bad;
minimal_demerits[tight_fit]= awful_bad;
minimal_demerits[decent_fit]= awful_bad;
minimal_demerits[loose_fit]= awful_bad;
minimal_demerits[very_loose_fit]= awful_bad;

/*:855*//*871:*/
#line 16854 "htex.w"

if(par_shape_ptr==null)
if(hang_indent==0)
{last_special_line= 0;second_width= x;
second_indent= 0;
}
else/*872:*/
#line 16865 "htex.w"

{last_special_line= abs(hang_after);
if(hang_after<0)
{first_width= x-abs(hang_indent);
if(hang_indent>=0)first_indent= hang_indent;
else first_indent= 0;
second_width= x;second_indent= 0;
}
else{first_width= x;first_indent= 0;
second_width= x-abs(hang_indent);
if(hang_indent>=0)second_indent= hang_indent;
else second_indent= 0;
}
}

/*:872*/
#line 16860 "htex.w"

else QUIT("parshape not yet implemented");
if(looseness==0)easy_line= last_special_line;
else easy_line= max_halfword

/*:871*/
#line 16211 "htex.w"
;
/*886:*/
#line 17133 "htex.w"

threshold= pretolerance;
if(threshold>=0)
{
#ifdef STAT
if(tracing_paragraphs> 0)
{print_nl("@firstpass");}
#endif

second_pass= false;final_pass= false;
}
else{threshold= tolerance;second_pass= true;
final_pass= (emergency_stretch<=0);
#ifdef STAT
#endif

}
loop{if(threshold> inf_bad)threshold= inf_bad;
/*887:*/
#line 17186 "htex.w"

q= get_node(active_node_size);
type(q)= unhyphenated;fitness(q)= decent_fit;
link(q)= last_active;break_node(q)= null;
line_number(q)= prev_graf+1;total_demerits(q)= 0;link(active)= q;
do_all_six(store_background);
passive= null;printed_node= temp_head;pass_number= 0;

/*:887*/
#line 17151 "htex.w"
;
cur_p= link(temp_head);auto_breaking= true;
prev_p= cur_p;
while((cur_p!=null)&&(link(active)!=last_active))
/*889:*/
#line 17221 "htex.w"

{if(is_char_node(cur_p))
/*890:*/
#line 17255 "htex.w"

{prev_p= cur_p;
do{f= font(cur_p);
act_width= act_width+char_width(f,char_info(f,character(cur_p)));
cur_p= link(cur_p);
}while(!(!is_char_node(cur_p)));
}

/*:890*/
#line 17223 "htex.w"
;
switch(type(cur_p)){
case hlist_node:case vlist_node:case rule_node:act_width= act_width+width(cur_p);break;
case whatsit_node:/*1392:*/
#line 25484 "htex.w"

adv_past(cur_p)

/*:1392*/
#line 17227 "htex.w"
break;
case glue_node:{/*891:*/
#line 17266 "htex.w"

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

/*:891*/
#line 17229 "htex.w"
;
}break;
case kern_node:if(subtype(cur_p)==explicit)kern_break
else act_width= act_width+width(cur_p);break;
case ligature_node:{f= font(lig_char(cur_p));
act_width= act_width+char_width(f,char_info(f,character(lig_char(cur_p))));
}break;
case disc_node:/*892:*/
#line 17282 "htex.w"

{if(!is_auto_disc(cur_p)||second_pass||final_pass)
{s= pre_break(cur_p);disc_width= 0;
if(s==null)try_break(ex_hyphen_penalty,hyphenated);
else{do{/*893:*/
#line 17302 "htex.w"

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

/*:893*/
#line 17286 "htex.w"
;
s= link(s);
}while(!(s==null));
act_width= act_width+disc_width;
try_break(hyphen_penalty,hyphenated);
act_width= act_width-disc_width;
}
}
r= replace_count(cur_p);s= link(cur_p);
while(r> 0)
{/*894:*/
#line 17319 "htex.w"

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

/*:894*/
#line 17296 "htex.w"
;
decr(r);s= link(s);
}
prev_p= cur_p;cur_p= s;goto done5;
}

/*:892*/
#line 17237 "htex.w"

case math_node:{auto_breaking= (subtype(cur_p)==after);kern_break;
}break;
case penalty_node:try_break(penalty(cur_p),unhyphenated);break;
case mark_node:case ins_node:case adjust_node:do_nothing;break;
default:confusion("paragraph");

}
prev_p= cur_p;cur_p= link(cur_p);
done5:;}

/*:889*/
#line 17157 "htex.w"
;
if(cur_p==null)
/*897:*/
#line 17356 "htex.w"

{try_break(eject_penalty,hyphenated);
if(link(active)!=last_active)
{/*898:*/
#line 17366 "htex.w"

r= link(active);fewest_demerits= awful_bad;
do{if(type(r)!=delta_node)if(total_demerits(r)<fewest_demerits)
{fewest_demerits= total_demerits(r);best_bet= r;
}
r= link(r);
}while(!(r==last_active));
best_line= line_number(best_bet)

/*:898*/
#line 17359 "htex.w"
;
if(looseness==0)goto done;
/*899:*/
#line 17380 "htex.w"

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

/*:899*/
#line 17361 "htex.w"
;
if((actual_looseness==looseness)||final_pass)goto done;
}
}

/*:897*/
#line 17160 "htex.w"
;
/*888:*/
#line 17194 "htex.w"

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

/*:888*/
#line 17161 "htex.w"
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
if(tracing_paragraphs> 0)
print_nl("@emergencypass");
#endif
background[2]= background[2]+emergency_stretch;final_pass= true;
}
}
done:
#ifdef STAT
#endif

/*:886*/
#line 16212 "htex.w"
;
/*900:*/
#line 17404 "htex.w"

post_line_break(final_widow_penalty)

/*:900*/
#line 16214 "htex.w"
;
/*888:*/
#line 17194 "htex.w"

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

/*:888*/
#line 16215 "htex.w"
;
hrestore_param_list();
}

/*:831*//*993:*/
#line 19075 "htex.w"


#define ensure_vbox(N) 

static pointer prune_page_top(pointer p)
{pointer prev_p;
pointer q;
prev_p= temp_head;link(temp_head)= p;
while(p!=null)
switch(type(p)){
case hlist_node:case vlist_node:case rule_node:/*994:*/
#line 19098 "htex.w"

{temp_ptr= new_spec(pointer_def[glue_kind][split_top_skip_no]);
q= new_glue(temp_ptr);glue_ref_count(temp_ptr)= null;link(prev_p)= q;link(q)= p;

if(width(temp_ptr)> height(p))width(temp_ptr)= width(temp_ptr)-height(p);
else width(temp_ptr)= 0;
p= null;
}

/*:994*/
#line 19086 "htex.w"
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

/*:993*//*995:*/
#line 19123 "htex.w"

static pointer vert_break(pointer p,scaled h,scaled d)

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
loop{/*998:*/
#line 19159 "htex.w"

if(p==null)pi= eject_penalty;
else/*999:*/
#line 19174 "htex.w"

switch(type(p)){
case hlist_node:case vlist_node:case rule_node:{
cur_height= cur_height+prev_dp+height(p);prev_dp= depth(p);
goto not_found;
}
case whatsit_node:/*1395:*/
#line 25493 "htex.w"

goto not_found

/*:1395*/
#line 19180 "htex.w"
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

/*:999*/
#line 19163 "htex.w"
;
/*1000:*/
#line 19195 "htex.w"

if(pi<inf_penalty)
{/*1001:*/
#line 19209 "htex.w"

if(cur_height<h)
if((active_height[3]!=0)||(active_height[4]!=0)||
(active_height[5]!=0))b= 0;
else b= badness(h-cur_height,active_height[2]);
else if(cur_height-h> active_height[6])b= awful_bad;
else b= badness(cur_height-h,active_height[6])

/*:1001*/
#line 19197 "htex.w"
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

/*:1000*/
#line 19165 "htex.w"
;
if((type(p)<glue_node)||(type(p)> kern_node))goto not_found;
update_heights:/*1002:*/
#line 19221 "htex.w"

if(type(p)==kern_node)q= p;
else{q= glue_ptr(p);
active_height[2+stretch_order(q)]= 
active_height[2+stretch_order(q)]+stretch(q);
active_height[6]= active_height[6]+shrink(q);
if((shrink_order(q)!=normal)&&(shrink(q)!=0))
{
DBG(DBGTEX,"Infinite glue shrinkage found in box being split");
r= new_spec(q);shrink_order(r)= normal;delete_glue_ref(q);
glue_ptr(p)= r;q= r;
}
}
cur_height= cur_height+prev_dp+width(q);prev_dp= 0

/*:1002*/
#line 19168 "htex.w"
;
not_found:if(prev_dp> d)
{cur_height= cur_height+prev_dp-d;
prev_dp= d;
}

/*:998*/
#line 19140 "htex.w"
;
prev_p= p;p= link(prev_p);
}
done:return best_place;
}

/*:995*//*1013:*/
#line 19520 "htex.w"

void freeze_page_specs(small_number s)
{page_contents= s;
page_goal= hvsize;page_max_depth= max_depth;
page_depth= 0;do_all_six(set_page_so_far_zero);
least_page_cost= awful_bad;
#ifdef STAT
if(tracing_pages> 0)
{begin_diagnostic();
print_nl("%% goal height=");print_scaled(page_goal);

print(", max depth=");print_scaled(page_max_depth);
end_diagnostic(false);
}
#endif

}

/*:1013*//*1021:*/
#line 19618 "htex.w"

bool hbuild_page(void)
{
pointer p;
pointer q,r;
int b,c;
int pi;
if(link(contrib_head)==null)return false;
do{resume:p= link(contrib_head);
/*1024:*/
#line 19658 "htex.w"

/*1027:*/
#line 19687 "htex.w"

switch(type(p)){
case hlist_node:case vlist_node:case rule_node:if(page_contents<box_there)
/*1028:*/
#line 19710 "htex.w"

{if(page_contents==empty)freeze_page_specs(box_there);
else page_contents= box_there;
temp_ptr= new_spec(pointer_def[glue_kind][top_skip_no]);
q= new_glue(temp_ptr);glue_ref_count(temp_ptr)= null;
if(width(temp_ptr)> height(p))width(temp_ptr)= width(temp_ptr)-height(p);
else width(temp_ptr)= 0;
link(q)= p;link(contrib_head)= q;goto resume;
}

/*:1028*/
#line 19691 "htex.w"

else/*1029:*/
#line 19720 "htex.w"

{page_total= page_total+page_depth+height(p);
page_depth= depth(p);
goto contribute;
}

/*:1029*/
#line 19693 "htex.w"
break;
case whatsit_node:/*1394:*/
#line 25490 "htex.w"

goto contribute

/*:1394*/
#line 19695 "htex.w"
;
case glue_node:if(page_contents<box_there)goto done1;
else if(precedes_break(page_tail))pi= 0;
else goto update_heights;break;
case kern_node:if(page_contents<box_there)goto done1;
else if(link(p)==null)return false;
else if(type(link(p))==glue_node)pi= 0;
else goto update_heights;break;
case penalty_node:if(page_contents<box_there)goto done1;else pi= penalty(p);break;
case mark_node:goto contribute;
case ins_node:happend_insertion(p);goto contribute;
default:confusion("page");

}

/*:1027*/
#line 19663 "htex.w"
;
/*1032:*/
#line 19748 "htex.w"

if(pi<inf_penalty)
{/*1034:*/
#line 19794 "htex.w"

if(page_total<page_goal)
if((page_so_far[3]!=0)||(page_so_far[4]!=0)||
(page_so_far[5]!=0))b= 0;
else b= badness(page_goal-page_total,page_so_far[2]);
else if(page_total-page_goal> page_shrink)b= awful_bad;
else b= badness(page_total-page_goal,page_shrink)

/*:1034*/
#line 19751 "htex.w"
;
if(b<awful_bad)
if(pi<=eject_penalty)c= pi;
else if(b<inf_bad)c= b+pi+insert_penalties;
else c= deplorable;
else c= b;
if(insert_penalties>=10000)c= awful_bad;
#ifdef STAT
if(tracing_pages> 0)/*1033:*/
#line 19780 "htex.w"

{begin_diagnostic();print_nl("%");
print(" t=");print_totals();
print(" g=");print_scaled(page_goal);
print(" b=");
if(b==awful_bad)print_char('*');else print_int(b);

print(" p=");print_int(pi);
print(" c=");
if(c==awful_bad)print_char('*');else print_int(c);
if(c<=least_page_cost)print_char('#');
end_diagnostic(false);
}

/*:1033*/
#line 19759 "htex.w"
;
#endif

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
{hloc_set_next(best_page_break);
if(p==best_page_break)best_page_break= null;
hpack_page();
hfill_page_template();
return true;
}
}

/*:1032*/
#line 19666 "htex.w"
;
if((type(p)<glue_node)||(type(p)> kern_node))goto contribute;
update_heights:/*1031:*/
#line 19733 "htex.w"

if(type(p)==kern_node)q= p;
else{q= glue_ptr(p);
page_so_far[2+stretch_order(q)]= 
page_so_far[2+stretch_order(q)]+stretch(q);
page_shrink= page_shrink+shrink(q);
if((shrink_order(q)!=normal)&&(shrink(q)!=0))
{
DBG(DBGTEX,"Infinite glue shrinkage found on current page");
r= new_spec(q);shrink_order(r)= normal;fast_delete_glue_ref(q);
glue_ptr(p)= r;q= r;
}
}
page_total= page_total+page_depth+width(q);page_depth= 0

/*:1031*/
#line 19669 "htex.w"
;
contribute:/*1030:*/
#line 19726 "htex.w"

if(page_depth> page_max_depth)
{page_total= 
page_total+page_depth-page_max_depth;
page_depth= page_max_depth;
}

/*:1030*/
#line 19670 "htex.w"
;
/*1025:*/
#line 19675 "htex.w"

link(page_tail)= p;page_tail= p;
link(contrib_head)= link(p);link(p)= null;goto done

/*:1025*/
#line 19671 "htex.w"
;
done1:/*1026:*/
#line 19679 "htex.w"

link(contrib_head)= link(p);link(p)= null;flush_node_list(p)

/*:1026*/
#line 19672 "htex.w"
;
done:

/*:1024*/
#line 19629 "htex.w"
;
}while(!(link(contrib_head)==null));
/*1022:*/
#line 19637 "htex.w"

if(nest_ptr==0)tail= contrib_head;
else contrib_tail= contrib_head

/*:1022*/
#line 19631 "htex.w"
;
return false;
}

/*:1021*//*1035:*/
#line 19802 "htex.w"

void happend_insertion(pointer p)
{uint8_t n;
scaled delta,h,w;
pointer q,r;
if(page_contents==empty)freeze_page_specs(inserts_only);
n= subtype(p);r= page_ins_head;
while(n>=subtype(link(r)))r= link(r);
n= qo(n);
if(subtype(r)!=qi(n))
/*1036:*/
#line 19834 "htex.w"

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
if((shrink_order(q)!=normal)&&(shrink(q)!=0))
DBG(DBGTEX,"Infinite glue shrinkage inserted from stream %d",n);
}

/*:1036*/
#line 19813 "htex.w"
;
if(type(r)==split_up)insert_penalties= insert_penalties+float_cost(p);
else{last_ins_ptr(r)= p;
delta= page_goal-page_total-page_depth+page_shrink;

if(count(n)==1000)h= height(p);
else h= x_over_n(height(p),1000)*count(n);
if(((h<=0)||(h<=delta))&&(height(p)+height(r)<=dimen(n)))
{page_goal= page_goal-h;height(r)= height(r)+height(p);
}
else/*1037:*/
#line 19860 "htex.w"

{if(count(n)<=0)w= max_dimen;
else{w= page_goal-page_total-page_depth;
if(count(n)!=1000)w= x_over_n(w,count(n))*1000;
}
if(w> dimen(n)-height(r))w= dimen(n)-height(r);
q= vert_break(ins_ptr(p),w,depth(p));
height(r)= height(r)+best_height_plus_depth;
#ifdef STAT
if(tracing_pages> 0)/*1038:*/
#line 19880 "htex.w"

{begin_diagnostic();print_nl("% split");print_int(n);

print(" to ");print_scaled(w);
print_char(',');print_scaled(best_height_plus_depth);
print(" p=");
if(q==null)print_int(eject_penalty);
else if(type(q)==penalty_node)print_int(penalty(q));
else print_char('0');
end_diagnostic(false);
}

/*:1038*/
#line 19869 "htex.w"
;
#endif

if(count(n)!=1000)
best_height_plus_depth= x_over_n(best_height_plus_depth,1000)*count(n);
page_goal= page_goal-best_height_plus_depth;
type(r)= split_up;broken_ptr(r)= q;broken_ins(r)= p;
if(q==null)insert_penalties= insert_penalties+eject_penalty;
else if(type(q)==penalty_node)insert_penalties= insert_penalties+penalty(q);
}

/*:1037*/
#line 19824 "htex.w"
;
}
}

/*:1035*//*1042:*/
#line 19952 "htex.w"

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
/*1046:*/
#line 20038 "htex.w"

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

/*:1046*/
#line 19972 "htex.w"
;
q= hold_head;link(q)= null;prev_p= page_head;p= link(prev_p);
while(p!=best_page_break)
{if(type(p)==ins_node)
{/*1048:*/
#line 20064 "htex.w"

{r= link(page_ins_head);
while(subtype(r)!=subtype(p))r= link(r);
if(best_ins_ptr(r)==null)wait= true;
else{wait= false;s= last_ins_ptr(r);link(s)= ins_ptr(p);
if(best_ins_ptr(r)==p)
/*1049:*/
#line 20080 "htex.w"

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
while(link(s)!=null)s= link(s);
best_ins_ptr(r)= null;
n= qo(subtype(r));
temp_ptr= list_ptr(box(n));
free_node(box(n),box_node_size);
streams[n].p= temp_ptr;
streams[n].t= s;
}

/*:1049*/
#line 20071 "htex.w"

else{while(link(s)!=null)s= link(s);
last_ins_ptr(r)= s;
}
}
/*1050:*/
#line 20102 "htex.w"

link(prev_p)= link(p);link(p)= null;
if(wait)
{link(q)= p;q= p;incr(insert_penalties);
}
else{delete_glue_ref(split_top_ptr(p));
free_node(p,ins_node_size);
}
p= prev_p

/*:1050*/
#line 20077 "htex.w"
;
}

/*:1048*/
#line 19977 "htex.w"
;
}
prev_p= p;p= link(prev_p);
}
split_top_skip= save_split_top_skip;
/*1045:*/
#line 20016 "htex.w"

if(p!=null)
{if(link(contrib_head)==null)
if(nest_ptr==0)tail= page_tail;
else contrib_tail= page_tail;
link(page_tail)= link(contrib_head);
link(contrib_head)= p;
link(prev_p)= null;
}
streams[0].p= link(page_head);link(page_head)= null;page_tail= page_head;
streams[0].t= prev_p;
if(q!=hold_head)
{link(q)= link(contrib_head);
link(contrib_head)= link(hold_head);
}

/*:1045*/
#line 19983 "htex.w"
;
/*1047:*/
#line 20052 "htex.w"

r= link(page_ins_head);
while(r!=page_ins_head)
{q= link(r);free_node(r,page_ins_node_size);r= q;
}
link(page_ins_head)= page_ins_head

/*:1047*/
#line 19984 "htex.w"
;
}

/*:1042*//*1174:*/
#line 21974 "htex.w"

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
/*1175:*/
#line 22006 "htex.w"

v= shift_amount(just_box)+2*dimen_def[quad_no];w= -max_dimen;
p= list_ptr(just_box);
while(p!=null)
{/*1176:*/
#line 22023 "htex.w"

reswitch:if(is_char_node(p))
{f= font(p);d= char_width(f,char_info(f,character(p)));
goto found;
}
switch(type(p)){
case hlist_node:case vlist_node:case rule_node:{d= width(p);goto found;
}
case ligature_node:/*666:*/
#line 13134 "htex.w"

{mem[lig_trick]= mem[lig_char(p)];link(lig_trick)= link(p);
p= lig_trick;goto reswitch;
}

/*:666*/
#line 22031 "htex.w"

case kern_node:case math_node:d= width(p);break;
case glue_node:/*1177:*/
#line 22045 "htex.w"

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

/*:1177*/
#line 22034 "htex.w"
break;
case whatsit_node:/*1391:*/
#line 25480 "htex.w"
d= 0

/*:1391*/
#line 22035 "htex.w"
;break;
default:d= 0;
}

/*:1176*/
#line 22011 "htex.w"
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

/*:1175*/
#line 21999 "htex.w"
;
}

/*1178:*/
#line 22063 "htex.w"

if(par_shape_ptr==null)
if((hang_indent!=0)&&
(((hang_after>=0)&&(prev_graf+2> hang_after))||
(prev_graf+1<-hang_after)))
{l= x-abs(hang_indent);
if(hang_indent> 0)s= hang_indent;else s= 0;
}
else{l= x;s= 0;
}
else{n= info(par_shape_ptr);
if(prev_graf+2>=n)p= par_shape_ptr+2*n;
else p= par_shape_ptr+2*(prev_graf+2);
s= mem[p-1].sc;l= mem[p].sc;
}

/*:1178*/
#line 22002 "htex.w"
;
pre_display_size= w;display_width= l;display_indent= s;
}

/*:1174*//*1229:*/
#line 22730 "htex.w"

{/*1227:*/
#line 22713 "htex.w"

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

/*:1227*/
#line 22731 "htex.w"

adjust_tail= adjust_head;b= hpack(p,natural);p= list_ptr(b);
t= adjust_tail;adjust_tail= null;
w= width(b);z= display_width;s= display_indent;
if(a==null)
{e= 0;q= 0;
}
else{e= width(a);q= e+math_quad;
}
if(w+q> z)
/*1231:*/
#line 22769 "htex.w"

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

/*:1231*/
#line 22742 "htex.w"
;
/*1232:*/
#line 22793 "htex.w"

d= half(z-w);
if((e> 0)&&(d<2*e))
{d= half(z-w-e);
if(p!=null)if(!is_char_node(p))if(type(p)==glue_node)d= 0;
}

/*:1232*/
#line 22744 "htex.w"
;
/*1233:*/
#line 22806 "htex.w"

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

/*:1233*/
#line 22745 "htex.w"
;
/*1234:*/
#line 22821 "htex.w"

if(e!=0)
{r= new_kern(z-w-e-d);
if(l)
{link(a)= r;link(r)= b;b= a;d= 0;
}
else{link(b)= r;link(r)= a;
}
b= hpack(b,natural);
}
shift_amount(b)= s+d;append_to_vlist(b,offset)

/*:1234*/
#line 22746 "htex.w"
;
/*1235:*/
#line 22833 "htex.w"

if((a!=null)&&(e==0)&&!l)
{tail_append(new_penalty(inf_penalty));
shift_amount(a)= s+z-width(a);
append_to_vlist(a,offset);
g2= 0;
}
if(t!=adjust_head)
{link(tail)= link(adjust_head);tail= t;
}
tail_append(new_penalty(post_display_penalty));
offset= (hpos-hstart)+1-node_pos;
store_map(tail,node_pos,offset);
if(g2> 0){tail_append(new_glue(pointer_def[glue_kind][g2]));store_map(tail,node_pos,offset);}

/*:1235*/
#line 22747 "htex.w"
;
prev_graf= prev_graf+3;
cur_list.bs_pos= hpos+node_pos;
push_nest();mode= hmode;
}
}

/*:1229*/
