/*377:*/
#line 7652 "hint.w"

#ifndef _RENDERNATIVE_H
#define _RENDERNATIVE_H

/*329:*/
#line 6580 "hint.w"

extern void nativeInit(void);
extern void nativeClear(void);
/*:329*//*330:*/
#line 6587 "hint.w"

extern void nativeSetSize(int px_h,int px_v,double xdpi,double ydpi);
/*:330*//*331:*/
#line 6592 "hint.w"

extern void nativeSetDark(int dark);
/*:331*//*332:*/
#line 6597 "hint.w"

extern void nativeBlank(void);
/*:332*//*333:*/
#line 6607 "hint.w"

typedef struct gcache_s*gcache_s_ptr;

extern void nativeGlyph(double x,double dx,double y,double dy,double w,double h,struct gcache_s*g,uint8_t s);
/*:333*//*334:*/
#line 6618 "hint.w"

void nativeRule(double x,double y,double w,double h);
/*:334*//*335:*/
#line 6624 "hint.w"

void nativeImage(double x,double y,double w,double h,unsigned char*istart,unsigned char*iend);
/*:335*//*336:*/
#line 6630 "hint.w"

extern void nativeSetPK(struct gcache_s*g);
extern void nativeSetFreeType(struct gcache_s*g);
/*:336*//*337:*/
#line 6637 "hint.w"

void nativeFreeGlyph(struct gcache_s*g);
/*:337*/
#line 7656 "hint.w"


#endif
/*:377*/
