/*387:*/
#line 7736 "hint.w"

#ifndef _RENDERNATIVE_H
#define _RENDERNATIVE_H

/*339:*/
#line 6651 "hint.w"

extern void nativeInit(void);
extern void nativeClear(void);
extern int nativePrintStart(int w,int h,int bpr,int bpp,unsigned char*bits);
extern int nativePrintEnd(void);
extern int nativePrint(unsigned char*bits);
/*:339*//*340:*/
#line 6661 "hint.w"

extern void nativeSetSize(int px_h,int px_v,double xdpi,double ydpi);
/*:340*//*341:*/
#line 6667 "hint.w"

extern void nativeSetDark(int dark);
extern void nativeSetGamma(double gamma);
/*:341*//*342:*/
#line 6673 "hint.w"

extern void nativeBlank(void);
/*:342*//*343:*/
#line 6683 "hint.w"

typedef struct gcache_s*gcache_s_ptr;

extern void nativeGlyph(double x,double dx,double y,double dy,double w,double h,struct gcache_s*g,uint8_t s);
/*:343*//*344:*/
#line 6694 "hint.w"

void nativeRule(double x,double y,double w,double h);
/*:344*//*345:*/
#line 6700 "hint.w"

void nativeImage(double x,double y,double w,double h,unsigned char*istart,unsigned char*iend);
/*:345*//*346:*/
#line 6706 "hint.w"

extern void nativeSetPK(struct gcache_s*g);
extern void nativeSetFreeType(struct gcache_s*g);
/*:346*//*347:*/
#line 6713 "hint.w"

void nativeFreeGlyph(struct gcache_s*g);
/*:347*/
#line 7740 "hint.w"


#endif
/*:387*/
