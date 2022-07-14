/*376:*/
#line 7629 "hint.w"

#ifndef _HFONTS_H
#define _HFONTS_H

/*307:*/
#line 5740 "hint.w"

typedef enum{no_format,pk_format,ft_format}FontFormat;
/*:307*//*308:*/
#line 5752 "hint.w"

/*340:*/
#line 6705 "hint.w"


typedef struct
{unsigned char*pk_comment;
unsigned int cs;
double ds;
unsigned char id;
}PKfont;
/*:340*//*352:*/
#line 7036 "hint.w"

typedef struct
{FT_Face face;
}FTfont;
/*:352*//*353:*/
#line 7043 "hint.w"

typedef struct
{int dummy;
}FTglyph;
/*:353*/
#line 5753 "hint.w"


typedef struct font_s{
unsigned char n;
unsigned char*font_data;
int data_size;
double s;
double hpxs,vpxs;
/*314:*/
#line 5847 "hint.w"

struct gcache_s**g0;
struct gcache_s***g1;
struct gcache_s****g2;
struct gcache_s*****g3;

/*:314*/
#line 5761 "hint.w"

FontFormat ff;
union{PKfont pk;FTfont tt;};
}Font;
/*:308*//*319:*/
#line 6059 "hint.w"

/*341:*/
#line 6718 "hint.w"

typedef struct
{unsigned char flag;
unsigned char*encoding;
}PKglyph;
/*:341*//*343:*/
#line 6745 "hint.w"

typedef struct{
int j;
int r;
int f;
unsigned char*data;
}PKparse;
/*:343*/
#line 6060 "hint.w"


struct gcache_s{
int w,h;
int hoff,voff;
unsigned char*bits;
unsigned int GLtexture;
FontFormat ff;
union{
PKglyph pk;
FTglyph tt;
};
};
typedef struct gcache_s Gcache;
/*:319*/
#line 7633 "hint.w"



#endif
/*:376*/
