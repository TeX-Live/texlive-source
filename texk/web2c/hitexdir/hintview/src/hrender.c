/*374:*/
#line 7581 "hint.w"

#include "basetypes.h"
#include "error.h"
#include "format.h"
#include <string.h> 
#include <math.h> 
#include "get.h"
#include "hrender.h"
#include "rendernative.h"
#include "htex.h"
#include "hint.h"

/*310:*/
#line 5757 "hint.w"

extern struct font_s*hget_font(unsigned char f);
/*:310*//*322:*/
#line 6102 "hint.w"

extern void render_char(int x,int y,struct font_s*f,uint32_t cc,uint8_t s);
/*:322*/
#line 7593 "hint.w"


/*236:*/
#line 4098 "hint.w"

double xdpi= 600.0,ydpi= 600.0;
/*:236*//*254:*/
#line 4456 "hint.w"

static bool forward_mode= false,backward_mode= false;
/*:254*//*273:*/
#line 4968 "hint.w"

static char*m_str;
static int m_length,m_chars;
/*:273*//*277:*/
#line 5039 "hint.w"

#define MAX_M_DIST 512
static uint8_t m_dist[MAX_M_DIST+5];
static int m_ptr,m_max;
/*:277*//*280:*/
#line 5140 "hint.w"

static int m_state,m_spaces;
static uint32_t m_d;
/*:280*//*285:*/
#line 5262 "hint.w"

static int m_focus;
static uint64_t m_page;
/*:285*//*288:*/
#line 5298 "hint.w"

static bool c_ignore;
static int m_style;
/*:288*//*296:*/
#line 5467 "hint.w"

static int cur_link= -1;
/*:296*//*325:*/
#line 6144 "hint.w"

static scaled cur_h,cur_v;
static scaled rule_ht,rule_dp,rule_wd;
static int cur_f;
static struct font_s*cur_fp;
static uint8_t cur_style= 0;
/*:325*/
#line 7595 "hint.w"

/*237:*/
#line 4102 "hint.w"

void hint_resize(int px_h,int px_v,double x_dpi,double y_dpi)
{
#if 0



static int old_px_h= 0,old_px_v= 0;
static double old_xdpi= 0.0,old_ydpi= 0.0;
if(old_px_h==px_h&&old_px_v==px_v&&old_xdpi==x_dpi&&old_ydpi==y_dpi)
return;
old_px_h= px_h;old_px_v= px_v;old_xdpi= x_dpi;old_ydpi= y_dpi;
#endif
xdpi= x_dpi;ydpi= y_dpi;
nativeSetSize(px_h,px_v,xdpi,ydpi);
hloc_clear();
hflush_contribution_list();hpage_init();
forward_mode= false;
backward_mode= false;
}
/*:237*//*248:*/
#line 4321 "hint.w"


uint64_t hint_page_top(uint64_t h)
{if(hin_addr==NULL)return 0;
hpos= hstart+LOC_POS0(h);
if(hpos>=hend)
return hint_page_bottom(hend-hstart);
hflush_contribution_list();
hloc_set(h);
if(LOC_OFF(h))
hget_par_node(LOC_OFF(h));
hint_forward();
#if 0
show_box(streams[0].p);
#endif
forward_mode= true;
backward_mode= false;
return h;
}
/*:248*//*249:*/
#line 4344 "hint.w"

uint64_t hint_page_get(void)
{
DBG(DBGPAGE,"page_get: %d : 0x%"PRIx64"\n",cur_loc,page_loc[cur_loc]);

if(hin_addr==NULL)return 0;
return page_loc[cur_loc];
}
/*:249*//*250:*/
#line 4360 "hint.w"

uint64_t hint_page(void)
{uint64_t i;
if(streams==NULL)return 0;
i= hint_page_get();
if(streams[0].p!=null)
return i;
else
return hint_page_top(i);
}
/*:250*//*251:*/
#line 4376 "hint.w"

uint64_t hint_page_home(void)
{uint64_t pos;
uint8_t where;
int n= zero_label_no;
/*71:*/
#line 1083 "hint.w"

where= labels[n].where;
#if 1
pos= ((uint64_t)labels[n].pos<<32)+(labels[n].pos-labels[n].pos0);
#else
pos= ((uint64_t)labels[n].pos0<<32);
#endif
/*:71*/
#line 4381 "hint.w"

/*267:*/
#line 4769 "hint.w"

if(where==LABEL_TOP)return hint_page_top(pos);
else if(where==LABEL_BOT)return hint_page_bottom(pos);
else if(where==LABEL_MID)return hint_page_middle(pos);
else return hint_page_get();
/*:267*/
#line 4382 "hint.w"

}
/*:251*//*252:*/
#line 4403 "hint.w"

double hint_get_fpos(void)
{
DBG(DBGPAGE,"get_fpos: %d : 0x%"PRIx64"\n",cur_loc,page_loc[cur_loc]);

if(hin_addr==NULL)return 0.0;
return(double)LOC_POS(page_loc[cur_loc])/(double)(hend-hstart);
}
/*:252*//*253:*/
#line 4417 "hint.w"

uint64_t hint_set_fpos(double fpos)
{uint32_t pos,pos0;
uint8_t*p,*q;
DBG(DBGPAGE,"set_fpos: %f\n",fpos);
if(hin_addr==NULL)return 0;
if(fpos<0.0)fpos= 0.0;
if(fpos> 1.0)fpos= 1.0;
pos= round((hend-hstart)*fpos);
p= hstart+pos;
q= hpos= hstart;
while(hpos<p)
{q= hpos;hff_hpos();}
if(hpos> p)
{pos= pos0= q-hstart;
if(KIND(*q)==par_kind&&KIND(hff_tag)==list_kind&&hff_list_size> 0)
{if(p>=hstart+hff_list_pos+hff_list_size)
pos= pos0= hpos-hstart;
else
{q= hpos= hstart+hff_list_pos;
while(hpos<=p)
{if(KIND(*hpos)==glue_kind||KIND(*hpos)==penalty_kind||KIND(*hpos)==disc_kind)
q= hpos;
hff_hpos();
}
pos= q-hstart;
}
}
}
else
pos= pos0= hpos-hstart;
return hint_page_top(PAGE_LOC(pos0,pos-pos0));
}
/*:253*//*255:*/
#line 4461 "hint.w"

uint64_t hint_page_next(void)
{if(hin_addr==NULL)return 0;
if(hloc_next()&&forward_mode)
{if(!hint_forward())
{hloc_prev();return hint_page();}
forward_mode= true;
backward_mode= false;
return hint_page_get();
}
else
{hflush_contribution_list();hpage_init();
return hint_page();
}
}
/*:255*//*256:*/
#line 4486 "hint.w"

uint64_t hint_page_prev(void)
{if(hin_addr==NULL)return 0;
if(hloc_prev())
{hflush_contribution_list();hpage_init();
return hint_page();
}
else if(backward_mode)
{if(!hint_backward())return hint_page_top(0);
backward_mode= true;
forward_mode= false;
return hint_page_get();
}
else
return hint_page_bottom(hint_page_get());
}
/*:256*//*257:*/
#line 4512 "hint.w"

uint64_t hint_page_bottom(uint64_t h)
{if(hin_addr==NULL)return 0;
hloc_set(h);
hflush_contribution_list();
hpos= hstart+LOC_POS0(h);
if(LOC_OFF(h))
hteg_par_node(LOC_OFF(h));
if(!hint_backward())return hint_page();
backward_mode= true;
forward_mode= false;
return hint_page_get();
}
/*:257*//*258:*/
#line 4537 "hint.w"

uint64_t hint_page_middle(uint64_t l)
{uint32_t pos,pos0,offset;
pointer p,tp= null;
scaled h= 0,d= 0,ht= 0,hp,dp;
int pi= 0;
if(hin_addr==NULL)return 0;
pos= LOC_POS(l);
offset= LOC_OFF(l);
pos0= LOC_POS0(l);
if(hstart+pos0+offset>=hend)
return hint_page_bottom(hend-hstart);
hflush_contribution_list();
hpos= hstart+pos0;
hget_content();
p= link(contrib_head);
if(offset> 0)
{/*259:*/
#line 4576 "hint.w"

{while(p!=null)
{/*260:*/
#line 4603 "hint.w"

switch(type(p))
{case hlist_node:case vlist_node:case rule_node:
hp= height(p);dp= depth(p);
pi= inf_penalty;
break;
case glue_node:
hp= width(glue_ptr(p));dp= 0;
pi= 0;
break;
case kern_node:
hp= width(p);dp= 0;
pi= inf_penalty;
break;
case penalty_node:
hp= dp= 0;
pi= penalty(p);
break;
default:
pi= hp= dp= 0;
}
/*:260*/
#line 4578 "hint.w"

h+= d+hp;
d= dp;
if(pi<=0&&tp==null)
{uint32_t t= LOC_POS(hlocation(p));
if(t> pos)
{tp= p;ht= h;
break;
}
}
p= link(p);
}
if(tp==null)ht= h;
if(ht>=hvsize)
{/*261:*/
#line 4630 "hint.w"

pointer q= contrib_head;
scaled dh= ht-hvsize/2;
p= link(contrib_head);
h= d= 0;
while(p!=null)
{/*260:*/
#line 4603 "hint.w"

switch(type(p))
{case hlist_node:case vlist_node:case rule_node:
hp= height(p);dp= depth(p);
pi= inf_penalty;
break;
case glue_node:
hp= width(glue_ptr(p));dp= 0;
pi= 0;
break;
case kern_node:
hp= width(p);dp= 0;
pi= inf_penalty;
break;
case penalty_node:
hp= dp= 0;
pi= penalty(p);
break;
default:
pi= hp= dp= 0;
}
/*:260*/
#line 4636 "hint.w"

h= h+d+hp;
d= dp;
if(pi<=0&&h>=dh)break;
q= p;
p= link(p);
}
if(p!=null)
{offset= LOC_OFF(hlocation(p));
link(q)= null;
flush_node_list(link(contrib_head));
link(contrib_head)= p;
}
/*:261*/
#line 4592 "hint.w"

goto found;
}
}
/*:259*/
#line 4554 "hint.w"

}
else if(p!=null&&type(p)==penalty_node)
pi= penalty(p);
/*262:*/
#line 4655 "hint.w"

{pointer h_save= link(contrib_head);
pointer t_save= tail;
uint8_t*hpos_save= hpos;
pointer best_p= null;
int best_pi= pi;
link(contrib_head)= null;p= tail= contrib_head;
hpos= hstart+pos0;
h= ht;
while(h<(hvsize/4*3))
{while(link(p)==null&&hpos> 0)
hteg_content();
if(link(p)==null)
break;
else
p= link(p);
/*260:*/
#line 4603 "hint.w"

switch(type(p))
{case hlist_node:case vlist_node:case rule_node:
hp= height(p);dp= depth(p);
pi= inf_penalty;
break;
case glue_node:
hp= width(glue_ptr(p));dp= 0;
pi= 0;
break;
case kern_node:
hp= width(p);dp= 0;
pi= inf_penalty;
break;
case penalty_node:
hp= dp= 0;
pi= penalty(p);
break;
default:
pi= hp= dp= 0;
}
/*:260*/
#line 4671 "hint.w"

h= h+dp+hp;
if(pi<best_pi)
{best_pi= pi;
best_p= p;
if(best_pi<=eject_penalty)break;
}
}
if(best_p==null)
{flush_node_list(link(contrib_head));
hpos= hstart+pos0;
}
else
{p= link(contrib_head);
do{pointer q;
q= link(p);
link(p)= h_save;
h_save= p;
p= q;
}while(h_save!=best_p);
flush_node_list(p);
pos0= LOC_POS0(hlocation(best_p));
offset= 0;
}
link(contrib_head)= h_save;
if(t_save!=contrib_head)
tail= t_save;
hpos= hpos_save;
}
/*:262*/
#line 4558 "hint.w"

found:
hloc_set(PAGE_LOC(pos0,offset));
if(!hint_forward())return hint_page_top(0);
forward_mode= true;
backward_mode= false;
return hint_page_get();
}
/*:258*//*266:*/
#line 4758 "hint.w"

uint64_t hint_outline_page(int i)
{uint64_t pos;
uint8_t where;
if(i<0||i> max_outline)return hint_page_get();
pos= hint_outlines[i].pos;
where= hint_outlines[i].where;
/*267:*/
#line 4769 "hint.w"

if(where==LABEL_TOP)return hint_page_top(pos);
else if(where==LABEL_BOT)return hint_page_bottom(pos);
else if(where==LABEL_MID)return hint_page_middle(pos);
else return hint_page_get();
/*:267*/
#line 4765 "hint.w"

}
/*:266*//*274:*/
#line 4973 "hint.w"

void hint_set_mark(char*m,int s)
{m_str= m;
m_length= s;
/*275:*/
#line 4984 "hint.w"

if(m_length> 0)
{int i,j,k;
for(i= j= k= 0;i<m_length&&m_str[i]==' ';i++)continue;
for(;i<m_length;i++)
if(m_str[i]!=' '||m_str[i+1]!=' ')
{m_str[k]= m_str[i];
if(m_str[k]==' ')j++;
k++;
}
m_str[k]= 0;
m_length= k;
m_chars= m_length-j;
}
/*:275*/
#line 4977 "hint.w"

hmark_page();
}
/*:274*//*278:*/
#line 5073 "hint.w"

static void m_put(uint32_t d)
{if(m_ptr<MAX_M_DIST)
{if(d==0xFF)
m_dist[m_max= m_ptr]= 0xFF;
else if(d<0x7F)
m_dist[m_ptr++]= 0x80+d;
else
{if(d<(1<<14))
{
two_byte:
m_dist[m_ptr++]= d>>7;
m_dist[m_ptr++]= 0x80+(d&0x7F);
return;
}
if(d>=(1<<28))m_dist[m_ptr++]= d>>28;
if(d>=(1<<21))m_dist[m_ptr++]= (d>>21)&0x7F;
if(d>=(1<<14))m_dist[m_ptr++]= (d>>14)&0x7F;
d= d&((1<<14)-1);
goto two_byte;
}
}
}

static uint32_t m_get(void)
{uint32_t x,y;
x= m_dist[m_ptr++];
if(x==0xFF)return HINT_NO_POS;
if(x&0x80)return x&0x7F;
while(true)
{y= m_dist[m_ptr++];
if(y&0x80)return(x<<7)+(y&0x7F);
x= (x<<7)+y;
}
}
/*:278*//*279:*/
#line 5114 "hint.w"

static int m_next(int i)
{while((0x80&m_dist[i])==0)i++;
if(m_dist[i]==0xFF)return 0;
else return i+1;
}

static int m_prev(int i)
{if(i<=0)return m_max;
i--;
while(i> 0&&(0x80&m_dist[i-1])==0)i--;
return i;
}
/*:279*//*281:*/
#line 5145 "hint.w"

static void next_m_char(uint32_t c)
{
reconsider:

if(m_state==0&&c!=m_str[0])
{if(c!=' ')
m_d++;
}
else if(c==m_str[m_state])
{if(m_state==0)m_spaces= 0;
m_state++;
if(m_state==m_length)
{m_put(m_d);
m_d= 0;
m_state= 0;
}
}
else
/*282:*/
#line 5179 "hint.w"

{int i= 0,j= 0;
do{
if(m_str[i]==' ')j++;
i++;
}while(i<m_state&&strncmp(m_str,m_str+i,m_state-i)!=0);
m_d= m_d+i-j;
m_state= m_state-i;
goto reconsider;
}
/*:282*/
#line 5164 "hint.w"



}
/*:281*//*283:*/
#line 5192 "hint.w"

#if 0
static void next_m_space(void)
{if(m_state==0&&m_str[0]==' ')
{m_state= -1;m_spaces= 1;}
else if(m_state>=0&&m_str[m_state]==' ')
{if(m_state==0)m_spaces= 0;
m_state++;m_spaces++;
if(m_state==m_length)
{m_put(m_d);
m_d= 0;
m_state= 0;
}
else
m_state= -m_state;
}
else if(m_state> 0)
{m_d= m_d+m_state-m_spaces;m_state= 0;}
}
#endif
/*:283*//*284:*/
#line 5215 "hint.w"

#if 0
static void vlist_mark(pointer p);
static void hlist_mark(pointer p)
{while(p!=null)
{if(is_char_node(p)&&!m_ignore)next_m_char(character(p));
else switch(type(p))
{case hlist_node:if(list_ptr(p)!=null)hlist_mark(list_ptr(p));break;
case vlist_node:if(list_ptr(p)!=null)vlist_mark(list_ptr(p));break;
case ligature_node:
{pointer q= lig_ptr(p);
while(q!=null)
{if(!m_ignore)next_m_char(character(q));q= link(q);
}
}
break;
case glue_node:if(!m_ignore)next_m_space();break;
case whatsit_node:
if(subtype(p)==ignore_node)
{if(ignore_info(p)==1)
{hlist_mark(ignore_list(p));
m_ignore= 1;
}
else
m_ignore= 0;
}
break;
default:break;
}
p= link(p);
}
}

static void vlist_mark(pointer p)
{while(p!=null)
{switch(type(p))
{case hlist_node:if(list_ptr(p)!=null)hlist_mark(list_ptr(p));
if(!m_ignore)next_m_space();break;
case vlist_node:if(list_ptr(p)!=null)vlist_mark(list_ptr(p));break;
default:break;
}
p= link(p);
}
}
#endif
/*:284*//*286:*/
#line 5268 "hint.w"

void hmark_page(void)
{if(streams==NULL||streams[0].p==null)return;
m_ptr= 0;
if(m_page!=page_loc[cur_loc])
{m_page= page_loc[cur_loc];
m_focus= 0;
}
if(m_length> 0)
{m_d= 0;
m_state= 0;
trv_init(next_m_char);
if(type(streams[0].p)==vlist_node)
trv_vlist(list_ptr(streams[0].p));
else
trv_hlist(list_ptr(streams[0].p));
}
m_put(0xFF);
if(m_focus>=m_max)m_focus= 0;
}
/*:286*//*293:*/
#line 5383 "hint.w"

void c_ignore_list(pointer p)
{while(p!=null)
{if(is_char_node(p))
{/*290:*/
#line 5323 "hint.w"

{while(m_d==0)
{m_style^= MARK_BIT;
if(m_style&MARK_BIT)
{if(m_ptr==m_focus)m_style|= FOCUS_BIT;else m_style&= ~FOCUS_BIT;
m_d= m_chars;
}
else
{m_style&= ~FOCUS_BIT;
m_d= m_get();
}
}
m_d--;
}
/*:290*/
#line 5387 "hint.w"

cur_style|= m_style;
}
else
{switch(type(p))
{case hlist_node:
case vlist_node:c_ignore_list(list_ptr(p));break;
case ligature_node:
{pointer q= lig_ptr(p);
while(q!=null)
{/*290:*/
#line 5323 "hint.w"

{while(m_d==0)
{m_style^= MARK_BIT;
if(m_style&MARK_BIT)
{if(m_ptr==m_focus)m_style|= FOCUS_BIT;else m_style&= ~FOCUS_BIT;
m_d= m_chars;
}
else
{m_style&= ~FOCUS_BIT;
m_d= m_get();
}
}
m_d--;
}
/*:290*/
#line 5397 "hint.w"

cur_style|= m_style;
q= link(q);
}
}
break;
}
}
p= link(p);
}
}
/*:293*//*294:*/
#line 5430 "hint.w"

bool hint_prev_mark(void)
{m_focus= m_prev(m_focus);
while(m_focus==0&&m_page> 0)
{hint_page_prev();
m_focus= m_prev(0);
}
return(m_focus!=0);
}

bool hint_next_mark(void)
{m_focus= m_next(m_focus);
while(m_focus==0)
{uint64_t p= m_page;
if(p==hint_page_next())break;
m_focus= m_next(0);
}
return(m_focus!=0);
}
/*:294*//*302:*/
#line 5531 "hint.w"

static int links_allocated= 0;
void add_new_link(int n,pointer p,scaled h,scaled v)
{hint_Link*t;
uint64_t pos;
uint8_t where;
max_link++;
if(max_link>=links_allocated)
{if(links_allocated<=0)
{links_allocated= 32;
ALLOCATE(hint_links,links_allocated,hint_Link);
}
else
{links_allocated= links_allocated*1.4142136+0.5;
REALLOCATE(hint_links,links_allocated,hint_Link);
}
}
t= hint_links+max_link;
REF_RNG(label_kind,n);
/*71:*/
#line 1083 "hint.w"

where= labels[n].where;
#if 1
pos= ((uint64_t)labels[n].pos<<32)+(labels[n].pos-labels[n].pos0);
#else
pos= ((uint64_t)labels[n].pos0<<32);
#endif
/*:71*/
#line 5550 "hint.w"

t->where= where;
t->pos= pos;
if(type(p)==hlist_node)
{scaled hp= height(p),dp= depth(p);
t->top= v-hp;
t->bottom= v+dp;
t->left= h;
t->right= h;
}
else
{t->top= v;
t->bottom= v;
t->left= h;
t->right= h+width(p);
}
}

void end_new_link(int n,pointer p,scaled h,scaled v)
{hint_Link*t;
t= hint_links+max_link;
if(type(p)==hlist_node)
t->right= h;
else
t->bottom= v;
}
/*:302*//*303:*/
#line 5600 "hint.w"

static scaled hlink_distance(scaled x,scaled y,hint_Link*t)
{scaled d,dx= 0,dy= 0;
d= t->top-y;
if(d> 0)dy= d;
else
{d= y-t->bottom;
if(d> 0)dy= d;
}
d= x-t->right;
if(d> 0)dx= d;
else
{d= t->left-x;
if(d> 0)dx= d;
}
if(dx> dy)return dx;
else return dy;

}
int hint_find_link(scaled x,scaled y,scaled precission)
{static int last_hit= -1;
int i;
hint_Link*t;
if(max_link<0)return-1;
if(last_hit<0||last_hit> max_link)last_hit= max_link/2;
i= last_hit;
t= hint_links+i;
if(hlink_distance(x,y,t)<=precission)
return i;
else if(y<t->top)
{while(i> 0)
{i--;
t= hint_links+i;
if(hlink_distance(x,y,t)<=precission)
{last_hit= i;return i;}
}
return-1;
}
else
{int k;
scaled d,min_d= precission;
int min_i= -1;
for(k= 0;k<=max_link;k++)
{i= i+1;
if(i> max_link)i= 0;
t= hint_links+i;
d= hlink_distance(x,y,t);
if(d<min_d)
{min_d= d;min_i= i;}
}
last_hit= min_i;
return last_hit;
}
}
/*:303*//*304:*/
#line 5660 "hint.w"

uint64_t hint_link_page(int i)
{uint64_t h;
uint8_t w;
if(i<0||i> max_link)return hint_page_get();
h= hint_links[i].pos;
w= hint_links[i].where;
if(w==LABEL_TOP)return hint_page_top(h);
else if(w==LABEL_BOT)return hint_page_bottom(h);
else if(w==LABEL_MID)return hint_page_middle(h);
else return hint_page_get();
}
/*:304*//*323:*/
#line 6109 "hint.w"

static void render_rule(int x,int y,int w,int h)
{if(w> 0&&h> 0)
nativeRule(SP2PT(x),SP2PT(y),SP2PT(w),SP2PT(h));
}
/*:323*//*324:*/
#line 6120 "hint.w"

void render_image(int x,int y,int w,int h,uint32_t n)
{
uint8_t*spos,*sstart,*send;
spos= hpos;sstart= hstart;send= hend;
hget_section(n);
nativeImage(SP2PT(x),SP2PT(y),SP2PT(w),SP2PT(h),hstart,hend);
hpos= spos;hstart= sstart;hend= send;
}
/*:324*//*326:*/
#line 6152 "hint.w"

static void vlist_render(pointer this_box);

static void hlist_render(pointer this_box)
{scaled base_line;
scaled left_edge;
scaled h_save;
glue_ord g_order;
uint8_t g_sign;
pointer p;
pointer leader_box;
scaled leader_wd;
scaled lx;
scaled edge;
double glue_temp;
double cur_glue;
scaled cur_g;
int local_link= -1;
uint8_t f;
uint32_t c;

cur_g= 0;
cur_glue= 0.0;
g_order= glue_order(this_box);
g_sign= glue_sign(this_box);
p= list_ptr(this_box);
#ifdef DEBUG
if(p==0xffff)
QUIT("Undefined list pointer in hbox 0x%x-> mem[0x%x] -> 0x%x\n",
this_box,mem[this_box].i,p);
#endif
base_line= cur_v;
left_edge= cur_h;
/*299:*/
#line 5497 "hint.w"

if(cur_link>=0)
{add_new_link(cur_link,this_box,cur_h,cur_v);
local_link= cur_link;
cur_link= -1;
}
/*:299*/
#line 6185 "hint.w"

while(p!=null)
{
#ifdef DEBUG
if(p==0xffff)
QUIT("Undefined pointer in hlist 0x%x\n",p);
if(link(p)==0xffff)
QUIT("Undefined link in hlist mem[0x%x]=0x%x\n",p,mem[p].i);
#endif
if(is_char_node(p))
{do
{f= font(p);
c= character(p);
if(!c_ignore&&c!=' ')
{cur_style= cur_style&~(MARK_BIT|FOCUS_BIT);
/*290:*/
#line 5323 "hint.w"

{while(m_d==0)
{m_style^= MARK_BIT;
if(m_style&MARK_BIT)
{if(m_ptr==m_focus)m_style|= FOCUS_BIT;else m_style&= ~FOCUS_BIT;
m_d= m_chars;
}
else
{m_style&= ~FOCUS_BIT;
m_d= m_get();
}
}
m_d--;
}
/*:290*/
#line 6200 "hint.w"

cur_style|= m_style;
}

render_c:
if(f!=cur_f)
{
#ifdef DEBUG
if(f> max_ref[font_kind])
QUIT("Undefined Font %d mem[0x%x]=0x%x\n",
f,p,mem[p].i);
#endif
cur_fp= hget_font(f);
cur_f= f;
}
render_char(cur_h,cur_v,cur_fp,c,cur_style);
cur_h= cur_h+char_width(f,char_info(f,c));
#ifdef DEBUG
if(link(p)==0xffff)
QUIT("Undefined link in charlist mem[0x%x]=0x%x\n",p,mem[p].i);
#endif
p= link(p);
}while(is_char_node(p));
}
else
{switch(type(p))
{case hlist_node:
case vlist_node:
#ifdef DEBUG
if(list_ptr(p)==0xffff)
QUIT("Undefined list pointer in hlist mem[0x%x] = 0x%x -> 0x%x\n",
p,mem[p].i,list_ptr(p));
#endif
if(list_ptr(p)==null)cur_h= cur_h+width(p);
else
{cur_v= base_line+shift_amount(p);
edge= cur_h;
if(type(p)==vlist_node)
vlist_render(p);
else
hlist_render(p);
cur_h= edge+width(p);cur_v= base_line;
}
break;
case rule_node:
rule_ht= height(p);rule_dp= depth(p);rule_wd= width(p);
goto fin_rule;
case whatsit_node:
/*292:*/
#line 5360 "hint.w"

if(subtype(p)==ignore_node)
{if(ignore_info(p)==1)
{cur_style= cur_style&~(MARK_BIT|FOCUS_BIT);
c_ignore_list(ignore_list(p));
c_ignore= true;
}
else
c_ignore= false;
}
/*:292*/
#line 6248 "hint.w"

else/*297:*/
#line 5471 "hint.w"

if(subtype(p)==start_link_node)
{cur_style|= LINK_BIT;
local_link= label_ref(p);
add_new_link(local_link,this_box,cur_h,cur_v);
}
else if(subtype(p)==end_link_node)
{cur_style&= ~LINK_BIT;
end_new_link(local_link,this_box,cur_h,cur_v);
local_link= -1;
}
/*:297*/
#line 6249 "hint.w"

else if(subtype(p)==image_node)
{scaled h,w;
w= image_width(p);
h= image_height(p);
if(g_sign!=normal)
{if(g_sign==stretching)
{if(image_stretch_order(p)==g_order)
{vet_glue((double)(glue_set(this_box))*image_stretch(p));
w= w+round(glue_temp);
}
}
else if(image_shrink_order(p)==g_order)
{vet_glue((double)(glue_set(this_box))*image_shrink(p));
w= w-round(glue_temp);
}
}
if(w!=image_width(p))
{double f;
f= (double)w/(double)image_width(p);
h= round((double)h*f);
}
render_image(cur_h,cur_v,w,h,image_no(p));
cur_h= cur_h+w;
}
break;
case glue_node:
{pointer g;
g= glue_ptr(p);rule_wd= width(g)-cur_g;
if(g_sign!=normal)
{if(g_sign==stretching)
{if(stretch_order(g)==g_order)
{cur_glue= cur_glue+stretch(g);
vet_glue((double)(glue_set(this_box))*cur_glue);
cur_g= round(glue_temp);
}
}
else if(shrink_order(g)==g_order)
{cur_glue= cur_glue-shrink(g);
vet_glue((double)(glue_set(this_box))*cur_glue);
cur_g= round(glue_temp);
}
}
rule_wd= rule_wd+cur_g;
if(subtype(p)>=a_leaders)
{leader_box= leader_ptr(p);
if(type(leader_box)==rule_node)
{rule_ht= height(leader_box);rule_dp= depth(leader_box);
goto fin_rule;
}
leader_wd= width(leader_box);
if((leader_wd> 0)&&(rule_wd> 0))
{rule_wd= rule_wd+10;
edge= cur_h+rule_wd;lx= 0;

if(subtype(p)==a_leaders)
{h_save= cur_h;
cur_h= left_edge+leader_wd*((cur_h-left_edge)/leader_wd);
if(cur_h<h_save)cur_h= cur_h+leader_wd;
}
else
{int lq= rule_wd/leader_wd;
int lr= rule_wd%leader_wd;
if(subtype(p)==c_leaders)cur_h= cur_h+(lr/2);
else{lx= lr/(lq+1);
cur_h= cur_h+((lr-(lq-1)*lx)/2);
}
}
while(cur_h+leader_wd<=edge)

{cur_v= base_line+shift_amount(leader_box);
h_save= cur_h;
c_ignore= true;
if(type(leader_box)==vlist_node)
vlist_render(leader_box);
else
hlist_render(leader_box);
c_ignore= false;
cur_v= base_line;
cur_h= h_save+leader_wd+lx;
}
cur_h= edge-10;goto next_p;
}
}
goto move_past;
}
case kern_node:
case math_node:
cur_h= cur_h+width(p);
break;
case ligature_node:
f= font(lig_char(p));
c= character(lig_char(p));
/*291:*/
#line 5342 "hint.w"

if(!c_ignore)
{pointer q;
cur_style= cur_style&~(MARK_BIT|FOCUS_BIT);
q= lig_ptr(p);
while(q!=null)
{/*290:*/
#line 5323 "hint.w"

{while(m_d==0)
{m_style^= MARK_BIT;
if(m_style&MARK_BIT)
{if(m_ptr==m_focus)m_style|= FOCUS_BIT;else m_style&= ~FOCUS_BIT;
m_d= m_chars;
}
else
{m_style&= ~FOCUS_BIT;
m_d= m_get();
}
}
m_d--;
}
/*:290*/
#line 5348 "hint.w"

cur_style|= m_style;
q= link(q);
}
}
/*:291*/
#line 6342 "hint.w"

goto render_c;
default:;
}
goto next_p;
fin_rule:
if(is_running(rule_ht))
rule_ht= height(this_box);
if(is_running(rule_dp))
rule_dp= depth(this_box);
rule_ht= rule_ht+rule_dp;
if((rule_ht> 0)&&(rule_wd> 0))
{cur_v= base_line+rule_dp;
render_rule(cur_h,cur_v,rule_wd,rule_ht);
cur_v= base_line;
}
move_past:cur_h= cur_h+rule_wd;
next_p:
#ifdef DEBUG
if(link(p)==0xffff)
QUIT("Undefined link in hlist mem[0x%x]=0x%x\n",p,mem[p].i);
#endif
p= link(p);
}
}
/*298:*/
#line 5487 "hint.w"

if(local_link>=0)
{end_new_link(local_link,this_box,cur_h,cur_v);
cur_link= local_link;
}
/*:298*/
#line 6367 "hint.w"

}

static void vlist_render(pointer this_box)
{
scaled left_edge;
scaled top_edge;
scaled save_v;
glue_ord g_order;
uint8_t g_sign;
pointer p;
pointer leader_box;
scaled leader_ht;
scaled lx;
scaled edge;
double glue_temp;
double cur_glue;
scaled cur_g;

cur_g= 0;cur_glue= float_constant(0);
g_order= glue_order(this_box);
g_sign= glue_sign(this_box);p= list_ptr(this_box);
#ifdef DEBUG
if(p==0xffff)
QUIT("Undefined list pointer in vbox 0x%x-> mem[0x%x] -> 0x%x\n",
this_box,mem[this_box].i,p);
#endif
left_edge= cur_h;cur_v= cur_v-height(this_box);
top_edge= cur_v;
while(p!=null)
{if(is_char_node(p))DBG(DBGTEX,"Glyph in vertical list ignored");
else
{switch(type(p))
{case hlist_node:
case vlist_node:
#ifdef DEBUG
if(list_ptr(p)==0xffff)
QUIT("Undefined list pointer in vlist mem[0x%x] = 0x%x -> 0x%x\n",
p,mem[p].i,list_ptr(p));
#endif
if(list_ptr(p)==null)cur_v= cur_v+height(p)+depth(p);
else
{cur_v= cur_v+height(p);save_v= cur_v;
cur_h= left_edge+shift_amount(p);
if(type(p)==vlist_node)vlist_render(p);
else
hlist_render(p);
cur_v= save_v+depth(p);cur_h= left_edge;
}
break;
case rule_node:
rule_ht= height(p);rule_dp= depth(p);rule_wd= width(p);
goto fin_rule;
case whatsit_node:
if(subtype(p)==image_node)
{scaled h,w;
w= image_width(p);
h= image_height(p);
if(g_sign!=normal)
{if(g_sign==stretching)
{if(image_stretch_order(p)==g_order)
{vet_glue((double)(glue_set(this_box))*image_stretch(p));
h= h+round(glue_temp);
}
}
else if(image_shrink_order(p)==g_order)
{vet_glue((double)(glue_set(this_box))*image_shrink(p));
h= h-round(glue_temp);
}
}
if(h!=image_height(p))
{double f;
f= (double)h/(double)image_height(p);
w= round((double)w*f);
}
cur_v= cur_v+h;
render_image(cur_h,cur_v,w,h,image_no(p));
}
break;
case glue_node:
{pointer g= glue_ptr(p);rule_ht= width(g)-cur_g;
if(g_sign!=normal)
{if(g_sign==stretching)
{if(stretch_order(g)==g_order)
{cur_glue= cur_glue+stretch(g);
vet_glue((double)(glue_set(this_box))*cur_glue);
cur_g= round(glue_temp);
}
}
else if(shrink_order(g)==g_order)
{cur_glue= cur_glue-shrink(g);
vet_glue((double)(glue_set(this_box))*cur_glue);
cur_g= round(glue_temp);
}
}
rule_ht= rule_ht+cur_g;
if(subtype(p)>=a_leaders)
{leader_box= leader_ptr(p);
if(type(leader_box)==rule_node)
{rule_wd= width(leader_box);rule_dp= 0;
goto fin_rule;
}
leader_ht= height(leader_box)+depth(leader_box);
if((leader_ht> 0)&&(rule_ht> 0))
{rule_ht= rule_ht+10;
edge= cur_v+rule_ht;lx= 0;
if(subtype(p)==a_leaders)
{save_v= cur_v;
cur_v= top_edge+leader_ht*((cur_v-top_edge)/leader_ht);
if(cur_v<save_v)cur_v= cur_v+leader_ht;
}
else
{int lq= rule_ht/leader_ht;
int lr= rule_ht%leader_ht;
if(subtype(p)==c_leaders)cur_v= cur_v+(lr/2);
else
{lx= lr/(lq+1);
cur_v= cur_v+((lr-(lq-1)*lx)/2);
}
}
while(cur_v+leader_ht<=edge)
{cur_h= left_edge+shift_amount(leader_box);
cur_v= cur_v+height(leader_box);save_v= cur_v;
c_ignore= true;
if(type(leader_box)==vlist_node)
vlist_render(leader_box);
else
hlist_render(leader_box);
c_ignore= false;
cur_h= left_edge;
cur_v= save_v-height(leader_box)+leader_ht+lx;
}
cur_v= edge-10;goto next_p;
}
}
}
goto move_past;
case kern_node:
cur_v= cur_v+width(p);
break;
default:;
}
goto next_p;

fin_rule:
if(is_running(rule_wd))rule_wd= width(this_box);
rule_ht= rule_ht+rule_dp;
cur_v= cur_v+rule_ht;
if((rule_ht> 0)&&(rule_wd> 0))
{render_rule(cur_h,cur_v,rule_wd,rule_ht);
}
goto next_p;

move_past:
cur_v= cur_v+rule_ht;
}
next_p:
#if 0
if(link(p)==1||link(p)==0xffff){
show_box(streams[0].p);
QUIT("vertical node mem[0x%x] =0x%x ->linking to node 0x%x\n",
p,mem[p].i,link(p));
}
#endif
p= link(p);
}
}

/*:326*//*327:*/
#line 6542 "hint.w"

uint64_t hint_blank(void)
{nativeBlank();
return 0;
}
/*:327*//*328:*/
#line 6551 "hint.w"


void hint_render(void)
{nativeBlank();
if(streams==NULL||streams[0].p==null)return;
cur_h= 0;
cur_v= height(streams[0].p);
cur_f= -1;cur_fp= NULL;
cur_link= -1;max_link= -1;
/*289:*/
#line 5316 "hint.w"

m_ptr= 0;m_d= 0;m_style= MARK_BIT;c_ignore= false;cur_style= 0;
/*:289*/
#line 6560 "hint.w"

if(type(streams[0].p)==vlist_node)
vlist_render(streams[0].p);
else
hlist_render(streams[0].p);
}
/*:328*/
#line 7596 "hint.w"

/*:374*/
