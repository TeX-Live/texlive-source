/*383:*/
#line 7641 "hint.w"

#ifndef _HRENDER_H
#define _HRENDER_H
/*284:*/
#line 5094 "hint.w"

#define MARK_BIT 0x1
#define FOCUS_BIT 0x2
/*:284*//*302:*/
#line 5485 "hint.w"

#define LINK_BIT 0x4
/*:302*//*329:*/
#line 6154 "hint.w"

#define SP2PT(X) ((X)/(double)(1<<16))
/*:329*/
#line 7644 "hint.w"


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
extern void hint_render_on(void);
extern void hint_render_off(void);
extern void hint_dark(int dark);
extern void hint_gamma(double gamma);
extern int hint_print_on(int w,int h,int bpr,int bpp,unsigned char*bits);
extern int hint_print_off(void);
extern int hint_print(unsigned char*bits);
#endif
/*:383*/
