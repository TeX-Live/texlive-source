/*374:*/
#line 7585 "hint.w"

#ifndef _HFONTS_H
#define _HFONTS_H

/*306:*/
#line 5687 "hint.w"

typedef enum{no_format,pk_format,ft_format}font_format_t;
/*:306*//*307:*/
#line 5699 "hint.w"

/*338:*/
#line 6672 "hint.w"


typedef struct
{unsigned char*pk_comment;
unsigned int cs;
double ds;
unsigned char id;
}pk_t;
/*:338*//*350:*/
#line 7003 "hint.w"

typedef struct
{FT_Face face;
}ft_t;
/*:350*//*351:*/
#line 7010 "hint.w"

typedef struct
{int dummy;
}ftg_t;
/*:351*/
#line 5700 "hint.w"


typedef struct font_s{
unsigned char n;
unsigned char*font_data;
int data_size;
double s;
double hpxs,vpxs;
/*312:*/
#line 5787 "hint.w"

struct gcache_s**g0;
struct gcache_s***g1;
struct gcache_s****g2;
struct gcache_s*****g3;

/*:312*/
#line 5708 "hint.w"

font_format_t ff;
union{pk_t pk;ft_t tt;};
}font_t;
/*:307*//*317:*/
#line 5999 "hint.w"

/*339:*/
#line 6685 "hint.w"

typedef struct
{unsigned char flag;
unsigned char*encoding;
}pkg_t;
/*:339*//*341:*/
#line 6712 "hint.w"

typedef struct{
int j;
int r;
int f;
unsigned char*data;
}pk_parse_t;
/*:341*/
#line 6000 "hint.w"


struct gcache_s{
int w,h;
int hoff,voff;
unsigned char*bits;
unsigned int GLtexture;
font_format_t ff;
union{
pkg_t pk;
ftg_t tt;
};
};
typedef struct gcache_s gcache_t;
/*:317*/
#line 7589 "hint.w"



#endif
/*:374*/
