/*381:*/
#line 7599 "hint.w"

#ifndef _HINT_H_
#define _HINT_H_

typedef uint16_t pointer;
typedef int scaled;

/*44:*/
#line 756 "hint.w"

extern void hrestore_param_list(void);
/*:44*//*49:*/
#line 811 "hint.w"

typedef struct{
pointer p,t;
}Stream;
extern Stream*streams;
/*:49*//*64:*/
#line 1028 "hint.w"

extern void hfill_page_template(void);
/*:64*//*79:*/
#line 1248 "hint.w"

void hget_content(void);
/*:79*//*83:*/
#line 1300 "hint.w"

extern void hteg_content(void);
/*:83*//*160:*/
#line 2591 "hint.w"

extern void hprune_unwanted_nodes(void);
/*:160*//*170:*/
#line 2787 "hint.w"

extern void set_line_break_params(void);
/*:170*//*172:*/
#line 2828 "hint.w"

extern void hget_par_node(uint32_t offset);
/*:172*//*177:*/
#line 2928 "hint.w"

extern void hteg_par_node(uint32_t offset);
/*:177*//*203:*/
#line 3295 "hint.w"

extern pointer*pointer_def[32];
extern scaled*dimen_def;
extern int32_t*integer_def;
/*:203*//*204:*/
#line 3350 "hint.w"

extern Stream*streams;
extern bool flush_pages(uint32_t pos);
extern pointer skip(uint8_t n);
extern pointer*box_ptr(uint8_t n);
extern int count(uint8_t n);
extern scaled dimen(uint8_t n);
/*:204*//*207:*/
#line 3407 "hint.w"

extern void hpage_init(void);
/*:207*//*209:*/
#line 3424 "hint.w"

extern void hflush_contribution_list(void);
/*:209*//*229:*/
#line 3827 "hint.w"

#define HINT_NO_LOC 0xFFFFFFFFFFFFFFFF
#define PAGE_LOC(POS0,OFF) (((uint64_t)((POS0)+(OFF))<<32) + (uint64_t)(OFF))
#define LOC_POS(P) ((P)>>32) 
#define LOC_OFF(P) ((P)&0xFFFFFFFF) 
#define LOC_POS0(P) (LOC_POS(P)-LOC_OFF(P)) 
/*:229*//*232:*/
#line 3864 "hint.w"

extern uint64_t page_loc[];
extern int cur_loc;
/*:232*//*234:*/
#line 3913 "hint.w"

extern void hloc_clear(void);
extern bool hloc_next(void);
extern bool hloc_prev(void);
extern uint64_t hlocation(pointer p);
/*:234*//*239:*/
#line 4031 "hint.w"

extern void hloc_init(void);
extern void store_map(pointer p,uint32_t pos,uint32_t offset);
extern uint32_t hposition(pointer p);
extern void hloc_set(uint64_t h);
extern void hloc_set_next(pointer p);
/*:239*//*241:*/
#line 4102 "hint.w"

extern int hint_begin(void);
extern void hint_end(void);
extern bool hint_map(void);
extern void hint_unmap(void);
/*:241*//*246:*/
#line 4226 "hint.w"

extern bool hint_forward(void);
extern bool hint_backward(void);
/*:246*//*249:*/
#line 4257 "hint.w"

extern scaled hvsize,hhsize;
/*:249*//*272:*/
#line 4815 "hint.w"

typedef struct{
uint64_t pos;
uint8_t depth;
uint8_t where;
int p;
char*title;
}hint_Outline;
extern hint_Outline*hint_outlines;
/*:272*//*273:*/
#line 4833 "hint.w"

#define LABEL_UNDEF 0
#define LABEL_TOP 1
#define LABEL_BOT 2
#define LABEL_MID 3
/*:273*//*277:*/
#line 4892 "hint.w"

extern int hint_get_outline_max(void);
extern hint_Outline*hint_get_outlines(void);
extern uint64_t hint_outline_page(int i);
/*:277*//*279:*/
#line 5007 "hint.w"

extern void trv_init(void(*f)(uint32_t c));
extern void trv_vlist(pointer p);
extern void trv_hlist(pointer p);
/*:279*//*307:*/
#line 5542 "hint.w"

typedef struct{
uint64_t pos;
uint8_t where;
scaled top,bottom,left,right;
}hint_Link;
extern hint_Link*hint_links;
extern int max_link;
/*:307*//*312:*/
#line 5705 "hint.w"

extern int hint_find_link(scaled x,scaled y,scaled precission);
extern uint64_t hint_link_page(int i);
/*:312*//*380:*/
#line 7582 "hint.w"

extern void leak_in(pointer p,int s);
extern void leak_out(pointer p,int s);
/*:380*/
#line 7606 "hint.w"


#endif
/*:381*/
