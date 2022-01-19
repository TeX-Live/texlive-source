/*373:*/
#line 7551 "hint.w"

#ifndef _HRENDER_H
#define _HRENDER_H
/*276:*/
#line 5006 "hint.w"

#define MARK_BIT 0x1
#define FOCUS_BIT 0x2
/*:276*//*295:*/
#line 5456 "hint.w"

#define LINK_BIT 0x4
/*:295*//*320:*/
#line 6083 "hint.w"

#define SP2PT(X) ((X)/(double)(1<<16))
/*:320*/
#line 7554 "hint.w"


extern int page_h,page_v;
extern double xdpi,ydpi;
extern uint64_t hint_blank(void);
extern void hint_render(void);
extern uint64_t hint_page_get(void);
extern uint64_t hint_page_top(uint64_t h);
extern uint64_t hint_page_middle(uint64_t h);
extern uint64_t hint_page_bottom(uint64_t h);
extern uint64_t hint_page(void);
extern uint64_t hint_page_next(void);
extern uint64_t hint_page_prev(void);
extern uint64_t hint_page_home(void);
extern void hint_resize(int px_h,int px_v,double xdpi,double ydpi);
extern void hint_clear_fonts(bool rm);
extern void hmark_page(void);
extern void hint_set_mark(char*m,int s);
extern bool hint_prev_mark(void);
extern bool hint_next_mark(void);
extern double hint_get_fpos(void);
extern uint64_t hint_set_fpos(double fpos);

#endif
/*:373*/
