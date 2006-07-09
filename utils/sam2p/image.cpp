/*
 * image.cpp (formerly sampled.cpp)
 * by pts@fazekas.hu at Wed Feb 27 09:26:05 CET 2002
 */

#ifdef __GNUC__
#pragma implementation
#endif

#include "image.hpp"
#include "error.hpp"
#include <string.h> /* strlen() */
#include "gensio.hpp"

/* --- 4-byte hashing */

#if SIZEOF_INT>=4
typedef unsigned int  u32_t;
typedef   signed int  s32_t;
#else
typedef unsigned long u32_t;
typedef   signed long s32_t;
#endif

/* vvv Dat: moved these out of Hash46 to pacify VC6.0 */
  const unsigned M=1409;
  /** Number of _value_ data bytes (they are not hashed) */
  const unsigned D=2;
  /** Size of each tuple in the array `t' */
  const unsigned HD=4+D;
  /** A tuple is considered free iff its first byte equals FREE */
  const unsigned char FREE=255;


/**
 * M=1409
 * h(K)=K%1409
 * h(i,K)=-i*(1+(K%1408)) (i in 0..1408)
 * K=[a,r,g,b]=(a<<24)+(r<<16)+(g<<8)+b (4 bytes)
 * h(K)=(253*a+722*r+(g<<8)+b)%1409
 * h(i,K)=-i*(1+(896*a+768*r+(g<<8)+b)%1408)
 *
 * -- No dynamic growing or re-hashing. Useful for hashing colormap palettes
 *    with a maximum size of 256.
 * -- Deleting not supported.
 *
 * Implementation: Use 32 bit integers for calculation.
 *
 * Imp: remove unused attr `size'
 */
class Hash46 {
 public:
  /** Creates an empty hash. */
  Hash46();
  inline unsigned getSize() const { return size; }
  inline unsigned getLength() const { return size; }
  inline unsigned getMaxSize() const { return M; }
  inline bool isFull() const { return size==M; }
  /** @return NULLP or the pointer to a tuple */
  inline unsigned char* lookup(unsigned char k[4]) {
    unsigned char *ret=walk(k);
    return ret==NULLP || *ret==FREE ? (unsigned char*)NULLP : ret;
  }
  /** Can be called only if !isFull()
   * @return NULL if isFull() and not found; otherwise: pointer to the tuple
   *         found or the place to which the insert can take place.
   */
  unsigned char* walk(unsigned char k[4]);
 protected:
  /** Number of non-free tuples in the hash. */
  unsigned size;
  unsigned char t[M*HD];
};

Hash46::Hash46(): size(0) {
  memset(t, FREE, sizeof(t));
}

unsigned char *Hash46::walk(unsigned char k[4]) {
  u32_t hk, hik;
  hk=HD*(((((u32_t)1<<24)%M)*k[0]+(((u32_t)1<<16)%M)*k[1]+
          (((u32_t)1<< 8)%M)*k[2]+k[3])%M);
  hik=HD*(1+((((u32_t)1<<24)%(M-1))*k[0]+(((u32_t)1<<16)%(M-1))*k[1]+
             (((u32_t)1<< 8)%(M-1))*k[2]+k[3])%(M-1));
  /* fprintf(stderr, "hk=%u hik=%u\n", hk, hik); */
  register unsigned char *p=t+hk;
  unsigned i=M;
  /* fprintf(stderr, "?? %02x %02x %02x %02x\n", k[0], k[1], k[2], k[3]); */
  do {
    /* fprintf(stderr, "examining %02x %02x %02x %02x %d\n", p[0], p[1], p[2], p[3], (k[0]=p[0] && k[1]==p[1] && k[2]==p[2])); */
    if (*p==FREE || (k[0]==p[0] && k[1]==p[1] && k[2]==p[2] && k[3]==p[3])) return p;
    /* ^^^ huge == BUGFIX at Sun Apr 14 00:16:59 CEST 2002 */
    if (hk>=hik) { hk-=hik; p-=hik; }
            else { hk+=M*HD-hik; p+=M*HD-hik; }
  } while (--i!=0);
  /* fprintf(stderr, "full\n"); */
  return (unsigned char*)NULLP;
}

/* --- */

const unsigned char Image::Sampled::cs2cpp[6]= { 0, 1, 3, 3, 4, 4 };
char const *Image::Sampled::cs2devcs(unsigned char cs) {
  static const char *names[]={ (char*)NULLP, "Gray", "RGB", "RGB", "CMYK", "CMYK" };
  return cs>=1 && cs<=5 ? names[cs] : (char*)NULLP;
}


void Image::Sampled::init(slen_t l_comment, slen_t l_header, dimen_t wd_, dimen_t ht_,
  /* ^^^ 24 is required for /Transparent in out_tiff_work */
  unsigned char bpc_, unsigned char ty_, unsigned char cpp_) {
  static const slen_t PADDING=24;
  bpc=bpc_;
  ty=ty_;
  wd=wd_;
  ht=ht_;
  cpp=cpp_;
  // pred=1;
  transpc=0x1000000UL; /* Dat: this means: no transparent color */
  rlen=(((rlen_t)bpc_)*cpp_*wd_+7)>>3;
  beg=new char[len=l_comment+l_header+rlen*ht_+PADDING];
  rowbeg=(headp=const_cast<char*>(beg)+l_comment)+l_header;
  trail=const_cast<char*>(beg)+len-bpc;
}

Image::Gray*    Image::Sampled::toGray0(unsigned char bpc_) {
  unsigned char *crow=new unsigned char[wd*3+7*3], *p, *pend;
  Image::Gray *img=new Image::Gray(wd, ht, bpc_);
  unsigned char *outp=(unsigned char*)img->getRowbeg();
  dimen_t htc;
  memset(crow+wd*3, '\0', 7*3); /* *3 BUGFIX at Tue Jan 18 17:04:15 CET 2005 */
  unsigned i;
  /* Dat: not optimising for minimal rounding error since caller should ensure
   *      that there is no such error at all.
   */
  if (bpc_==1) {
    assert(img->getBpc()==1);
    for (htc=0;htc<ht;htc++) {
      copyRGBRow((char*)crow, htc);
      for (p=crow-3,pend=p+wd*3; p<pend; ) {
        i =(*(p+=3)!=0)<<7; i|=(*(p+=3)!=0)<<6; i|=(*(p+=3)!=0)<<5; i|=(*(p+=3)!=0)<<4;
        i|=(*(p+=3)!=0)<<3; i|=(*(p+=3)!=0)<<2; i|=(*(p+=3)!=0)<<1; i|=(*(p+=3)!=0);
        *outp++=i;
      }
    }
  } else if (bpc_==2) {
    for (htc=0;htc<ht;htc++) {
      copyRGBRow((char*)crow, htc);
      for (p=crow-3,pend=p+wd*3; p<pend; ) {
        i =(*(p+=3)/85)<<6; i|=(*(p+=3)/85)<<4; i|=(*(p+=3)/85)<<2; i|=(*(p+=3)/85);
        *outp++=i;
      }
    }
  } else if (bpc_==4) {
    for (htc=0;htc<ht;htc++) {
      copyRGBRow((char*)crow, htc);
      for (p=crow-3,pend=p+wd*3; p<pend; ) {
        i =(*(p+=3)/17)<<4; i|=(*(p+=3)/17);
        *outp++=i;
      }
    }
  } else if (bpc_==8) {
    for (htc=0;htc<ht;htc++) {
      copyRGBRow((char*)crow, htc);
      for (p=crow-3,pend=p+wd*3; p!=pend; ) {
        *outp++=*(p+=3);
      }
    }
  } else assert(0);
  return img;
}
Image::RGB*     Image::Sampled::toRGB0(unsigned char bpc_) {
  unsigned char *crow=new unsigned char[wd*3+7], *p, *pend;
  Image::RGB *img=new Image::RGB(wd, ht, bpc_);
  unsigned char *outp=(unsigned char*)img->getRowbeg();
  dimen_t htc;
  memset(crow+wd*3, '\0', 7);
  unsigned i;
  /* Dat: not optimising for minimal rounding error since caller should ensure
   *      that there is no such error at all.
   */
  if (bpc_==1) {
    for (htc=0;htc<ht;htc++) {
      copyRGBRow((char*)crow, htc);
      for (p=crow,pend=crow+wd*3; p<pend; ) {
        i =(*p++!=0)<<7; i|=(*p++!=0)<<6; i|=(*p++!=0)<<5; i|=(*p++!=0)<<4;
        i|=(*p++!=0)<<3; i|=(*p++!=0)<<2; i|=(*p++!=0)<<1; i|=(*p++!=0);
        *outp++=i;
      }
    }
  } else if (bpc_==2) {
    for (htc=0;htc<ht;htc++) {
      copyRGBRow((char*)crow, htc);
      for (p=crow,pend=crow+wd*3; p<pend; ) {
        i =(*p++/85)<<6; i|=(*p++/85)<<4; i|=(*p++/85)<<2; i|=(*p++/85);
        *outp++=i;
      }
    }
  } else if (bpc_==4) {
    for (htc=0;htc<ht;htc++) {
      copyRGBRow((char*)crow, htc);
      for (p=crow,pend=crow+wd*3; p<pend; ) {
        i =(*p++/17)<<4; i|=(*p++/17);
        *outp++=i;
      }
    }
  } else if (bpc_==8) {
    for (htc=0;htc<ht;htc++) {
      copyRGBRow((char*)outp, htc);
      outp+=wd*3;
    }
  } else assert(0);
  return img;
}

Image::Indexed* Image::Sampled::toIndexed0()/* const*/ {
  unsigned char *crow=new unsigned char[wd*3], k[6], *p, *pend, *w;
  Image::Indexed *img=new Image::Indexed(wd, ht, 256, 8);
  dimen_t htc;
  unsigned char *pal=(unsigned char*)img->getHeadp(), *outp=(unsigned char*)img->getRowbeg();
  unsigned ncols=0;
  Hash46 h;
  k[0]=0;
  for (htc=0;htc<ht;htc++) {
    copyRGBRow((char*)crow, htc);
    for (p=crow,pend=crow+wd*3; p!=pend; p+=3) {
      memcpy(k+1, p, 3);
      w=h.walk(k);
      assert(w!=NULL); /* Hash cannot be full since h.M>=256. */
      /* fprintf(stderr, "w=%p\n", w); */
      if (*w==/*h.*/FREE) {
        if (ncols==256) { delete img; delete [] crow; return (Image::Indexed*)NULLP; }
        /* ^^^ too many colors; cannot convert image to indexed */
        memcpy(w,k,4);
        memcpy(pal,k+1,3);
        /* fprintf(stderr,"newcol=%02x #%02x%02x%02x\n", k[0], k[1], k[2], k[3]); */
        /* fprintf(stderr,"newcol=pal #%02x%02x%02x\n", pal[0], pal[1], pal[2]); */
        pal+=3;
        *outp++=w[4]=ncols++;
      } else { /* a color that we have already seen */
        *outp++=w[4];
      }
    }
  }
  img->setNcolsMove(ncols);
  delete [] crow;

  /* Now img is ready. The user should call packPal() to make it even tighter. */
  return img;
}

void Image::Sampled::to8mul() {
  if (bpc==8) return;
  if (wd==0 || ht==0) { bpc=8; return; }

  unsigned oldBpc=bpc;
  slen_t wdcpp=wd*cpp;
  const char *oldBeg=beg;
  unsigned char *p=(unsigned char*)rowbeg;

  bpc=8;
  rlen=wd;
  beg=new char[len=rowbeg-oldBeg+rlen*ht+bpc];
  headp= const_cast<char*>(beg)+(headp-oldBeg);
  rowbeg=const_cast<char*>(beg)+(rowbeg-oldBeg);
  trail= const_cast<char*>(beg)+len-bpc;
  memcpy(const_cast<char*>(beg), oldBeg, rowbeg-beg);
  
  unsigned char *to=(unsigned char*)rowbeg, *toend;
  unsigned int i, j;
  Image::Sampled::dimen_t htc;
  if (oldBpc==1) {
    htc=ht; while (htc--!=0) {
      toend=to+((wdcpp)&~7);
      while (to!=toend) {
        i=*p++;
        *to++=(i>>7)*255;
        *to++=((i>>6)&1)*255;
        *to++=((i>>5)&1)*255;
        *to++=((i>>4)&1)*255;
        *to++=((i>>3)&1)*255;
        *to++=((i>>2)&1)*255;
        *to++=((i>>1)&1)*255;
        *to++=(     i&1)*255;
      }
      if (0!=(j=(wdcpp)&7)) {
        i=*p; /* No mem overrun, even if (wd&7)==0 */
        while (j--!=0) { *to++=(i>>7)*255; i<<=1; }
      }
    }
  } else if (oldBpc==2) {
    htc=ht; while (htc--!=0) {
      toend=to+((wdcpp)&~3);
      while (to!=toend) {
        i=*p++;
        *to++=(i>>6)*85;
        *to++=((i>>4)&3)*85;
        *to++=((i>>2)&3)*85;
        *to++=(     i&3)*85;
      }
      if (0!=(j=(wdcpp)&3)) {
        i=*p; /* No mem overrun, even if (wd&7)==0 */
        while (j--!=0) { *to++=(i>>6)*85; i<<=2; }
      }
    }
  } else if (oldBpc==4) {
    htc=ht; while (htc--!=0) {
      toend=to+((wdcpp)&~1);
      while (to!=toend) {
        i=*p++;
        *to++=(i>>4)*17;
        *to++=(     i&15)*17;
      }
      if (0!=((wdcpp)&1)) *to++=(*p++>>4)*17;
    }
  } else assert(0 && "invalid bpc");
  
  delete [] const_cast<char*>(oldBeg);
}

void Image::Sampled::to8nomul() {
  if (bpc==8) return;
  if (wd==0 || ht==0) { bpc=8; return; }

  unsigned oldBpc=bpc;
  slen_t wdcpp=wd*cpp;
  const char *oldBeg=beg;
  unsigned char *p=(unsigned char*)rowbeg;

  bpc=8;
  rlen=wd;
  beg=new char[len=rowbeg-oldBeg+rlen*ht+bpc];
  headp= const_cast<char*>(beg)+(headp-oldBeg);
  rowbeg=const_cast<char*>(beg)+(rowbeg-oldBeg);
  trail= const_cast<char*>(beg)+len-bpc;
  memcpy(const_cast<char*>(beg), oldBeg, rowbeg-beg);
  
  unsigned char *to=(unsigned char*)rowbeg, *toend;
  unsigned int i, j;
  Image::Sampled::dimen_t htc;
  if (oldBpc==1) {
    htc=ht; while (htc--!=0) {
      toend=to+((wdcpp)&~7);
      while (to!=toend) {
        i=*p++;
        *to++=(i>>7);
        *to++=((i>>6)&1);
        *to++=((i>>5)&1);
        *to++=((i>>4)&1);
        *to++=((i>>3)&1);
        *to++=((i>>2)&1);
        *to++=((i>>1)&1);
        *to++=(     i&1);
      }
      if (0!=(j=(wdcpp)&7)) {
        i=*p++; /* No mem overrun, even if (wd&7)==0 */
        while (j--!=0) { *to++=(i>>7); i<<=1; }
      }
    }
  } else if (oldBpc==2) {
    // assert(0);
    htc=ht; while (htc--!=0) {
      toend=to+((wdcpp)&~3);
      while (to!=toend) {
        i=*p++;
        *to++=(i>>6);
        *to++=((i>>4)&3);
        *to++=((i>>2)&3);
        *to++=(     i&3);
      }
      if (0!=(j=(wdcpp)&3)) {
        i=*p++;
        // fprintf(stderr,"j=%d\n",j);
        while (j--!=0) { *to++=(i>>6); i<<=2; }
      }
    }
    assert((slen_t)((char*)to-rowbeg)==(slen_t)wd*cpp*ht);
  } else if (oldBpc==4) {
    htc=ht; while (htc--!=0) {
      toend=to+((wdcpp)&~1);
      while (to!=toend) {
        i=*p++;
        *to++=(i>>4);
        *to++=(     i&15);
      }
      if (0!=((wdcpp)&1)) *to++=(*p++>>4);
    }
  } else assert(0 && "invalid bpc");
  
  delete [] const_cast<char*>(oldBeg);
}

unsigned char Image::Sampled::minRGBBpc() const {
  unsigned char *crow=new unsigned char[wd*3], *p, *pend=crow+wd*3;
  register unsigned minbpb=0;
  dimen_t htc;
  for (htc=0;htc<ht;htc++) {
    copyRGBRow((char*)crow, htc);
    for (p=crow; p!=pend; p++) {
      if ((*p&15)*17!=*p) { delete [] crow; return 8; } /* 4 bits are not enough */
      else if ((*p&3)*85!=*p) minbpb=3; /* 2 bits are not enough */
      else if ((*p&1)*255!=*p) minbpb|=1; /* 1 bit is not enough */
    }
  }
  delete [] crow;
  return 1+minbpb;
}

bool Image::Sampled::hasPixelRGB(Image::Sampled::rgb_t rgb) const {
  /* by pts@fazekas.hu at Sat Jan  8 13:24:19 CET 2005 */
  /* Dat: this dumb implementation will be overridden */
  if (rgb>0xffffffUL) return false;
  unsigned char *crow=new unsigned char[wd*3], *p, *pend=crow+wd*3, t[3];
  dimen_t htc;
  t[0]=(rgb>>16)&255; t[1]=(rgb>>8)&255; t[2]=rgb&255;
  for (htc=0;htc<ht;htc++) {
    copyRGBRow((char*)crow, htc);
    for (p=crow; p!=pend; p+=3) {
      if (t[0]==p[0] && t[1]==p[1] && t[2]==p[2]) { delete [] crow; return true; }
    }
  }
  delete [] crow;
  return false;
}

bool Image::Gray::hasPixelRGB(Image::Sampled::rgb_t rgb) const {
  /* by pts@fazekas.hu at Sat Jan  8 13:24:19 CET 2005 */
  /* Dat: faster than Image::Sampled::hasPixelRGB */
  if (rgb>0xffffffUL) return false;
  unsigned char t[3];
  t[0]=(rgb>>16)&255; t[1]=(rgb>>8)&255; t[2]=rgb&255;
  if (t[0]!=t[1] || t[0]!=t[2]) return false;
  if (bpc==8) {
    unsigned char *p=(unsigned char*)rowbeg, *pend=p+wd*ht;
    /* Imp: use memchr() if available */
    while (p!=pend && t[0]!=p[0]) p++;
    return p!=pend;
  }
  unsigned char *crow=new unsigned char[wd*3], *p, *pend=crow+wd*3;
  dimen_t htc;
  for (htc=0;htc<ht;htc++) {
    copyRGBRow((char*)crow, htc); /* Imp: avoid this if bpp==8 */
    p=crow; while (p!=pend && t[0]!=p[0]) p+=3;
    if (p!=pend) { delete [] crow; return true; }
  }
  delete [] crow;
  return false;
}

Image::Indexed* Image::Sampled::addAlpha0(Image::Sampled *img, Image::Gray *al) {
  Image::Indexed *iimg=(Image::Indexed*)img;
  unsigned ncols=0;
  if (img==NULLP) Error::sev(Error::EERROR) << "addAlpha: too many colors, transparency impossible" << (Error*)0;
  iimg->to8();
  iimg->packPal();
  if ((ncols=iimg->getNcols())==256) Error::sev(Error::EERROR) << "addAlpha: too many colors, transparency impossible" << (Error*)0;
  iimg->setNcolsMove(ncols+1);
  /* fprintf(stderr,"old ncols=%u\n", ncols); */
  iimg->setPal(ncols,0); /* black */
  iimg->setTransp(ncols);
  assert(iimg->getRlen()==iimg->getWd());
  assert(iimg->getWd()==al->getWd());  
  char *p=iimg->getRowbeg(), *pend=p+iimg->getRlen()*iimg->getHt(), *alq=al->getRowbeg();
  while (p!=pend) {
    if ((unsigned char)*alq++!=255) *p=ncols; /* make it transparent */
    p++;
  }
  return iimg;
}

/* --- */

Image::Indexed::Indexed(Image::Sampled::dimen_t wd_, Image::Sampled::dimen_t ht_, unsigned short ncols_, unsigned char bpc_) {
  param_assert(ncols_<=256);
  /* vvv Dat: `3' is here for an extra palette entry */
  init(3,3*ncols_,wd_,ht_,bpc_,TY_INDEXED,1);
  transp=-1;
  cs=CS_Indexed_RGB;
}
void Image::Indexed::setNcols(unsigned short ncols_) {
  headp=rowbeg-ncols_*3;
}
void Image::Indexed::setNcolsMove(unsigned short ncols_) {
  param_assert(ncols_<=256);
  unsigned ncols=getNcols();
  if (ncols_==ncols) return;
  if (ncols_<ncols || (slen_t)(headp-beg)>=(ncols_-ncols)*3) {
    memmove(rowbeg-ncols_*3, headp, ncols_*3);
    /* ^^^ *3 BUGFIX at Sun Apr 14 00:50:34 CEST 2002 */
  } else { /* Imp: test this routine */
    /* Tue Jun 11 16:22:52 CEST 2002 */
    assert(ncols_>ncols);
    const char *oldBeg=beg, *oldHeadp=headp, *oldRowbeg=rowbeg, *oldEnd=beg+len;
    slen_t delta=(ncols_-ncols)*3;
    // substr_grow(headp-oldBeg, ncols*3, ncols_*3); /* no such method */
    beg=new char[len+delta];
    headp= const_cast<char*>(beg)+(headp-oldBeg);
    rowbeg=const_cast<char*>(beg)+(rowbeg-oldBeg)+delta;
    trail= const_cast<char*>(beg)+(trail-oldBeg)+delta;
    assert(beg+(headp-oldBeg)==rowbeg-ncols_*3);
    /* Dat: this->xoffs is left unchanged */
    memcpy(headp,  oldHeadp,  oldRowbeg-oldHeadp);
    memcpy(rowbeg, oldRowbeg, oldEnd-oldRowbeg);
    delete [] const_cast<char*>(oldBeg);
  }
  headp=rowbeg-ncols_*3;
}
void Image::Indexed::setPal(unsigned char color, Image::Sampled::rgb_t rgb) {
  assert(color<(rowbeg-headp)/3);
  unsigned char *p=(unsigned char*)headp+3*color;
  *p++=rgb>>16;
  *p++=rgb>>8;
  *p=rgb;
}
void Image::Indexed::setTransp(unsigned char color) {
  // param_assert(color>=0); /* always */
  assert(transp==-1);
  transp=color;
  unsigned char *p=(unsigned char*)headp+3*color;
  transpc=((Image::Sampled::rgb_t)p[0]<<16)+(p[1]<<8)+p[2];
}
bool Image::Indexed::setTranspc(rgb_t color) {
  char t[3];
  /** vvv BUGFIX at Sat Jun 15 13:40:30 CEST 2002 */
  if (color==0x1000000UL) return transp!=-1; /* no effect */
  if (color==transpc) return transp!=-1; /* would not change */
  t[0]=color>>16; t[1]=color>>8; t[2]=color;
  char *p=headp, *pend=rowbeg;
  while (p!=pend) { /* Examine the palette. */
    if (p[0]==t[0] && p[1]==t[1] && p[2]==t[2]) {
      transpc=color;
      transp=(p-headp)/3; /* destroy old transparency */
      return true;
    }
    p+=3;
  }
  /* No transparency set this time. Maybe there is an old one; unchanged. */
  return transp!=-1;
}

void Image::Indexed::to8() { to8nomul(); }
Image::Indexed* Image::Indexed::toIndexed()/* const*/ { return this; }
Image::RGB*     Image::Indexed::toRGB(unsigned char bpc_)/* const*/ { return toRGB0(bpc_); }
Image::Gray*    Image::Indexed::toGray(unsigned char bpc_)/* const*/ { return toGray0(bpc_); }
bool Image::Indexed::canGray() const {
  char *p=headp, *pend=rowbeg, *tp=p+transp*3;
  /* ignore transparent color at Sat Jun 15 15:18:24 CEST 2002 */
  if (transp!=-1 && tp!=pend-3) {
    while (p!=pend) { /* Examine the palette. */
      if (p!=tp && (p[0]!=p[1] || p[1]!=p[2])) return false; /* Found a non-gray color. */
      p+=3;
    }
  } else {
    if (transp!=-1 && tp==pend-3) pend-=3; /* both conditions are important */
    while (p!=pend) { /* Examine the palette. */
      if (p[0]!=p[1] || p[1]!=p[2]) return false; /* Found a non-gray color. */
      p+=3;
    }
  }
  return true;
}
unsigned char Image::Indexed::minRGBBpc() const {
  unsigned char *p=(unsigned char*)headp, *pend=(unsigned char*)rowbeg;
  unsigned char *tp=p+transp*3;
  /* ignore transparent color at Sat Jun 15 15:18:24 CEST 2002 */
  register unsigned minbpb=0;
  while (p!=pend) { /* Examine the palette. */
    if (p==tp) { p+=3; continue; } /* ignore transparent color */
    if ((*p&15)*17!=*p) return 8; /* 4 bits are not enough */
    else if ((*p&3)*85!=*p) minbpb=3; /* 2 bits are not enough */
    else if ((*p&1)*255!=*p) minbpb|=1; /* 1 bit is not enough */
    p++;
  }
  return 1+minbpb;
}
void Image::Indexed::copyRGBRow(char *to, Image::Sampled::dimen_t whichrow) const {
  param_assert(whichrow<ht);
  if (wd==0) return;
  unsigned char *p=(unsigned char*)rowbeg+rlen*whichrow;
  char *r, *toend=to+3*wd;
  unsigned int i, j;
  
  if (bpc==1) {
    toend-=3*(wd&7);
    while (to!=toend) {
      i=*p++;
      r=headp+3*(i>>7);     *to++=*r++; *to++=*r++; *to++=*r++;
      r=headp+3*((i>>6)&1); *to++=*r++; *to++=*r++; *to++=*r++;
      r=headp+3*((i>>5)&1); *to++=*r++; *to++=*r++; *to++=*r++;
      r=headp+3*((i>>4)&1); *to++=*r++; *to++=*r++; *to++=*r++;
      r=headp+3*((i>>3)&1); *to++=*r++; *to++=*r++; *to++=*r++;
      r=headp+3*((i>>2)&1); *to++=*r++; *to++=*r++; *to++=*r++;
      r=headp+3*((i>>1)&1); *to++=*r++; *to++=*r++; *to++=*r++;
      r=headp+3*(     i&1); *to++=*r++; *to++=*r++; *to++=*r++;
    }
    i=*p; /* No mem overrun, even if (wd&7)==0 */
    j=wd&7;
    while (j--!=0) { r=headp+3*(i>>7); *to++=*r++; *to++=*r++; *to++=*r++; i<<=1; }
  } else if (bpc==2) {
    toend-=3*(wd&3);
    while (to!=toend) {
      i=*p++;
      r=headp+3*(i>>6);     *to++=*r++; *to++=*r++; *to++=*r++;
      r=headp+3*((i>>4)&3); *to++=*r++; *to++=*r++; *to++=*r++;
      r=headp+3*((i>>2)&3); *to++=*r++; *to++=*r++; *to++=*r++;
      r=headp+3*(     i&3); *to++=*r++; *to++=*r++; *to++=*r++;
    }
    i=*p; /* No mem overrun, even if (wd&7)==0 */
    j=wd&3;
    while (j--!=0) { r=headp+3*(i>>6); *to++=*r++; *to++=*r++; *to++=*r++; i<<=2; }
  } else if (bpc==4) {
    toend-=3*(wd&1);
    while (to!=toend) {
      i=*p++;
      r=headp+3*(i>>4);      *to++=*r++; *to++=*r++; *to++=*r++;
      r=headp+3*(     i&15); *to++=*r++; *to++=*r++; *to++=*r++;
    }
    if (0!=(wd&1)) { r=headp+3*(*p>>4); *to++=*r++; *to++=*r++; *to++=*r++; }
  } else if (bpc==8) {
    // fprintf(stderr, "p=%u pp=%u ppp=%u\n", p[0], p[1], p[2]);
    while (to!=toend) {
      r=headp+3**p++; *to++=*r++; *to++=*r++; *to++=*r++;
    }
  } else assert(0 && "invalid bpc");
}
void Image::Indexed::packPal() {
  /* Convert samples, make bpc=8. */
  to8();

  unsigned oldNcols=getNcols();
  unsigned char *p, *pend;
  assert((rowbeg-headp)%3==0);
  assert(transp>=-1);
  assert(transp<(int)oldNcols);
  if (oldNcols<=1) return; /* Cannot optimize further. */
  
  /* Find unused colors. old2new[c]=(is c used at least once)?1:0 */
  unsigned char old2new[256], newpal[768];
  memset(old2new, 0, sizeof(old2new));
  for (p=(unsigned char*)rowbeg, pend=p+wd*ht; p!=pend; p++) old2new[*p]=1;

  /* Find and eliminate duplicate colors. Build the new palette in the
   * beginning of newpal. Use a Hash46 for a quick lookup of colors already
   * seen. Use the previously computed old2new, but also overwrite it.
   */
  Hash46 h;
  int newTransp=-1;
  unsigned char *op=old2new, *opend=op+oldNcols, *w, k[6],
                *ptransp=(unsigned char*)headp+3*transp; /* ==p-3 if no transparent color */
  /* ^^^ headp BUGFIX at Fri Mar 22 18:02:18 CET 2002 */
  p=(unsigned char*)headp;
  // fprintf(stderr, "oldNcols=%d\n", (int)oldNcols);
  unsigned newNcols=0;
  while (op!=opend) {
    // fprintf(stderr, "color=%d %d\n", (int)(op-old2new), p-ptransp);
    if (0!=*op) { /* Map the color only if it is used in the image. */
      // fprintf(stderr, "used=%d\n", (int)(op-old2new));
      if (p==ptransp) { k[0]=1; k[1]=k[2]=k[3]=0; newTransp=newNcols; }
                 else { k[0]=0; memcpy(k+1,p,3); }
      w=h.walk(k);
      assert(w!=NULL); /* Hash cannot be full since h.M>=256. */
      if (*w==/*h.*/FREE) {
        memcpy(newpal+3*newNcols, p /* k+1 */, 3);
        /* ^^^ side effect: make the transparent color black */
        memcpy(w,k,4); w[4]=newNcols; *op=newNcols++;
      } else *op=w[4];
    }
    p+=3; op++;
  }
  // fprintf(stderr,"newTransp=%d transp=%d\n", newTransp, transp);

  // assert((newTransp==-1) == (transp==-1));
  assert(newTransp==-1 || transp!=-1);
  /* ^^^ BUGFIX: not true, because image may have transparency, but no
   *     transparent pixels.
   */
  assert((char*)p==headp+oldNcols*3);
  if (newNcols==oldNcols && transp==newTransp) {
    /* Could not change # colors. */
    if (transp==-1) return;
    if ((unsigned)transp==oldNcols-1) { setPal(transp, 0); return; }
  }

  /* Make the transparent color last. */
  if (newTransp!=-1 && newTransp!=(int)newNcols-1) {
    assert(transp!=-1);
    unsigned newLast=newNcols-1;
    memcpy(newpal+3*newTransp, newpal+3*newLast, 3);
    memset(newpal+3*newLast, 0, 3); transpc=0; /* make it black */
    for (op=old2new; op!=opend; op++) if (*op==newLast) *op=newTransp;
    old2new[transp]=newLast;
    transp=newTransp=newLast;
    p=newpal+newTransp*3;
  }

  /* Update the image. */  
  for (p=(unsigned char*)rowbeg, pend=p+wd*ht; p!=pend; p++) {
    assert(*p<oldNcols);
    *p=old2new[*p];
  }
  
  /* Update the palette. */
  headp=rowbeg-3*newNcols;
  memcpy(headp, newpal, 3*newNcols);
  transp=newTransp;
  /* vvv BUGFIX at Tue May 21 13:10:30 CEST 2002 */
  if (newTransp==-1) transpc=0x1000000UL; /* Dat: this means: no transparent color */
                else { transp=-1; setTransp(newTransp); }
}
void Image::Indexed::delete_separated(register Indexed **p) {
  while (*p!=NULLP) delete *p++;
}
Image::Indexed **Image::Indexed::separate() {
  assert(getNcols()>=1);
  unsigned char ncols1=getNcols()-1;
  signed nncols=getNcols()-(transp==-1 ? 0 : 1);
  register unsigned char curcol;
  Indexed **ret=new Indexed*[nncols+1], **curimg=ret;
  Image::Sampled::dimen_t htc;
  assert(cpp==1);
  slen_t wdcpp=wd/* *cpp*/;
  register unsigned char *p;
  char *to, *toend;
  register unsigned int i;

  ret[nncols]=(Indexed*)NULLP;
  to8();
  for (curcol=0; curcol<=ncols1; curcol++) {
    if (transp==(signed int)curcol) continue;
    curimg[0]=new Indexed(wd, ht, /*ncols:*/2, /*bpc:*/1);
    memcpy(curimg[0]->headp, headp+3*curcol, 3); /* copy the color value */
    curimg[0]->setTransp(1);
    to=curimg[0]->rowbeg; p=(unsigned char*)rowbeg;
    htc=ht; while (htc--!=0) {
      toend=to+((wdcpp+7)>>3);
      while (to!=toend) {
        i =(*p++!=curcol)<<7; i|=(*p++!=curcol)<<6;
        i|=(*p++!=curcol)<<5; i|=(*p++!=curcol)<<4;
        i|=(*p++!=curcol)<<3; i|=(*p++!=curcol)<<2;
        i|=(*p++!=curcol)<<1; i|=(*p++!=curcol);
        *to++=i;
      }
      if (0!=(wdcpp&7)) p+=(wdcpp&7)-8; /* negative */
    }
    curimg++;
  }
  assert(curimg==ret+nncols);
  return ret;
}

Image::Indexed *Image::Indexed::calcAlpha() {
  /* by pts@fazekas.hu at Tue Jun  4 21:27:29 CEST 2002 */
  assert(getNcols()>=1);
  packPal(); /* removes transparency if no transparent pixel */
  if (transp==-1) return (Image::Indexed*)NULLP;
  to8();
  Indexed *ret=new Indexed(wd, ht, /*ncols:*/2, /*bpc:*/1);
  Image::Sampled::dimen_t htc;
  assert(cpp==1);
  slen_t wdcpp=wd/* *cpp*/;
  register unsigned char *p;
  char *to, *toend;
  register unsigned int i, i8, i7;
  unsigned char transpx=transp;

  ret->headp[0]=ret->headp[0]=ret->headp[0]='\xFF'; /* white */
  ret->headp[3]=ret->headp[4]=ret->headp[5]='\x00'; /* black, transparent */
  ret->setTransp(1);
  to=ret->rowbeg; p=(unsigned char*)rowbeg;
  assert(transpx!=0);
  #if 0
    printf("tx=%u\n", transpx);
    printf("%u %u %u\n", headp[0], headp[1], headp[2]);
  #endif
  htc=ht; while (htc--!=0) {
    // putchar('.'); printf("mod=%d\n",(to-ret->rowbeg)%ret->rlen);
    // assert((to-ret->rowbeg)%ret->rlen==0);
    toend=to+(wdcpp>>3); /* ((wdcpp+7)>>3)-1; */
    /* ^^^ BUGFIX at Tue Sep 17 11:08:46 CEST 2002 */
    assert(toend>=to);
    while (to!=toend) {
      #if 1 /* add ->pal[0] funcitonality at Sat Jun 15 14:24:25 CEST 2002 */
        i=0; i8=256;
        /* vvv p[-1]=0 BUGFIX at Sun Dec  8 23:21:47 CET 2002 */
        while ((i8>>=1)!=0) if (*p++==transpx) { p[-1]=0; i|=i8; }
      #else
        i =(*p++==transpx)<<7; i|=(*p++==transpx)<<6;
        i|=(*p++==transpx)<<5; i|=(*p++==transpx)<<4;
        i|=(*p++==transpx)<<3; i|=(*p++==transpx)<<2;
        i|=(*p++==transpx)<<1; i|=(*p++==transpx);
      #endif
      *to++=i;
    }
    #if 1 /* This works even when p gets modified; this puts fixed 0 pads at EOLs */
      if ((wdcpp&7)!=0) {
        i7=1<<(7-(wdcpp&7)); i8=256; i=0;
        /* vvv p[-1]=0 BUGFIX at Sun Dec  8 23:21:47 CET 2002 */
        while ((i8>>=1)!=i7) if (*p++==transpx) { p[-1]=0; i|=i8; }
        *to++=i;
      }
    #else
      if (0!=(wdcpp&7)) p+=(wdcpp&7)-8; /* negative */
    #endif
  }
  assert(ret->rlen==((wd+7)>>3));
  /* printf("rlen=%d %d\n", ret->rlen, to-ret->rowbeg); */
  assert(to==ret->rowbeg+ht*ret->rlen);
  return ret;
}

void Image::Indexed::setBpc(unsigned char bpc_) {
  unsigned ncols=getNcols();
  if (bpc_==0) {
    if (ncols<=2) bpc_=1;
    else if (ncols<=4) bpc_=2;
    else if (ncols<=16) bpc_=4;
    else bpc_=8;
  } else {
    if (bpc_==1) assert(ncols<=2);
    else if (bpc_==2) assert(ncols<=4);
    else if (bpc_==4) assert(ncols<=16);
    else if (bpc_!=8) param_assert(0 && "invalid bpc_");
  }
  // fprintf(stderr, "bpc: %u -> %u\n", bpc, bpc_);
  if (bpc==bpc_) return;
  to8(); /* Imp: make the transition without the intermediate 8-bits... */
  if (bpc_==8) return;
  if (ht==0 || wd==0) { bpc=bpc_; return; }
  
  const char *oldBeg=beg;
  unsigned char *p=(unsigned char*)rowbeg;
  assert(cpp==1);
  slen_t wdcpp=wd/* *cpp*/;
  bpc=bpc_;
  rlen=(((rlen_t)bpc_)*wd+7)>>3;
  beg=new char[len=rowbeg-oldBeg+rlen*ht+bpc];
  headp= const_cast<char*>(beg)+(headp-oldBeg);
  rowbeg=const_cast<char*>(beg)+(rowbeg-oldBeg);
  trail= const_cast<char*>(beg)+len-bpc;
  memcpy(const_cast<char*>(beg), oldBeg, rowbeg-beg);
  
  unsigned char *to=(unsigned char*)rowbeg, *toend;
  unsigned int i;
  Image::Sampled::dimen_t htc;
  if (bpc_==1) {
    htc=ht; while (htc--!=0) {
      toend=to+((wdcpp+7)>>3);
      while (to!=toend) {
        i =*p++<<7; i|=*p++<<6; i|=*p++<<5; i|=*p++<<4;
        i|=*p++<<3; i|=*p++<<2; i|=*p++<<1; i|=*p++;
        *to++=i;
      }
      if (0!=(wdcpp&7)) p+=(wdcpp&7)-8; /* negative */
    }
  } else if (bpc_==2) {
    htc=ht; while (htc--!=0) {
      toend=to+((wdcpp+3)>>2);
      while (to!=toend) {
        i =*p++<<6; i|=*p++<<4; i|=*p++<<2; i|=*p++;
        *to++=i;
      }
      if (0!=(wdcpp&3)) p+=(wdcpp&3)-4;
    }
  } else if (bpc_==4) {
    htc=ht; while (htc--!=0) {
      toend=to+((wdcpp+1)>>1);
      while (to!=toend) {
        i =*p++<<4; i|=*p++;
        *to++=i;
      }
      if (0!=(wdcpp&1)) p--;
    }
  } else assert(0 && "invalid bpc");
  delete [] const_cast<char*>(oldBeg);
}
Image::Sampled::rgb_t Image::Indexed::getPal(unsigned char color) const {
  unsigned char *p=(unsigned char*)headp+3*color;
  return ((Image::Sampled::rgb_t)p[0]<<16)+(p[1]<<8)+p[2];
}

void Image::Indexed::dumpDebug(GenBuffer::Writable& gw) {
  gw <<"% ncols=" << getNcols()
     << " rlen=" << rlen
     << " ht=" << ht
     << " wd=" << wd
     << " cpp=" << (unsigned)cpp
     << " bpc=" << (unsigned)bpc
     << " transp=" << transp
     << " transpc=" << (transp==-1?"none":rgb2webhash(getPal(transp)))
     << " ty=" << (unsigned)ty
     //<< " pred=" << (unsigned)pred
     << '\n';
  unsigned char *p=(unsigned char*)headp;
  while (p!=(unsigned char*)rowbeg) {
    gw << rgb2webhash(((Image::Sampled::rgb_t)p[0]<<16)+(p[1]<<8)+p[2]) << '\n';
    p+=3;
  }
  gw << '\n';
  gw.vi_write(rowbeg,rlen*ht);
}

Image::Sampled* Image::Indexed::addAlpha(Image::Gray *al) {
  // Error::sev(Error::WARNING) << "Indexed: alpha channel ignored" << (Error*)0; return this;
  if (al->getHt()!=ht || al->getWd()!=wd) Error::sev(Error::EERROR) << "addAlpha: image dimension mismatch" << (Error*)0;
  bool ign_mid=false;
  unsigned char lightest, darkest;
  al->to8();
  al->calcExtrema(lightest, darkest);
  if (darkest==255) return this; /* no transparent pixels at all */
  to8();
  if (transp>=0) { /* Already have a transparent index. Join. */
    register char *p=rowbeg;
    register unsigned char transp_=transp;
    char *pend=rowbeg+rlen*ht, *alq=al->getRowbeg();
    /* Imp: choose an image color instead of black... */
    /* Dat: 0..254: transparent, 255: opaque */
    while (p!=pend) {
      if ((unsigned char)(*alq+1)>1) ign_mid=true;
      /* fprintf(stderr,"alq=%u\n", (unsigned char)*alq); */
      if ((unsigned char)*alq++!=255) p[0]=transp_; /* black out transparent-wannabe pixels */
      p++;
    }
  } else { /* No transparent color yet. */
    packPal();
    unsigned ncols=getNcols();
    char *p=rowbeg, *pend=rowbeg+rlen*ht, *alq=al->getRowbeg();
    /* Imp: choose an image color instead of black... */
    /* Dat: 0..254: transparent, 255: opaque */
    while (p!=pend) {
      if ((unsigned char)(*alq+1)>1) ign_mid=true;
      /* fprintf(stderr,"alq=%u\n", (unsigned char)*alq); */
      if ((unsigned char)*alq++!=255) p[0]=ncols; /* may set to 0 if ncols==256 */
      p++;
    }
    if (ncols==256) { /* Try again, probably now we have less colors */
      packPal();
      if ((ncols=getNcols())==256) Error::sev(Error::EERROR) << "Indexed::addAlpha: too many colors, transparency impossible" << (Error*)0;
      for (p=rowbeg,alq=al->getRowbeg(); p!=pend; p++) 
        if ((unsigned char)*alq++!=255) *p=ncols;
    }
    setNcolsMove(ncols+1);
    setPal(ncols,0); /* black */
    setTransp(ncols);
  }
  if (ign_mid) Error::sev(Error::WARNING) << "addAlpha: half-transparent pixels made transparent" << (Error*)0;
  return this;
}
void Image::Indexed::makeTranspZero() {
  if (transp<1) return; /* no transparency or already 0 */
  unsigned char oldBpc=bpc;
  register unsigned char transpch=transp;
  /* Imp: make this faster by not converting to 8 bits */
  if (oldBpc!=8) to8();

  /* Update the image. */
  register unsigned char *p; unsigned char *pend;
  for (p=(unsigned char*)rowbeg, pend=p+wd*ht; p!=pend; p++) {
    if (*p==0) *p=transp;
    else if (*p==transpch) *p=0;
  }
  rgb_t rzero=getPal(0), rtransp=getPal(transp);
  setPal(transp, rzero); setPal(0, rtransp);
  transp=0;
  if (oldBpc!=8) setBpc(oldBpc);
}

/* --- */

Image::Gray::Gray(Image::Sampled::dimen_t wd_, Image::Sampled::dimen_t ht_, unsigned char bpc_) {
  init(0,0,wd_,ht_,bpc_,TY_GRAY,1);
  cs=CS_GRAYSCALE;
}
void Image::Gray::to8() { to8mul(); }
Image::RGB    * Image::Gray::toRGB(unsigned char bpc_)/* const*/ { return toRGB0(bpc_); }
Image::Gray*    Image::Gray::toGray(unsigned char bpc_)/* const*/ { return bpc==bpc_ ? this : toGray0(bpc_); }
Image::Indexed* Image::Gray::toIndexed()/* const*/ {
  Image::Indexed *img=new Image::Indexed(wd, ht, (1<<bpc), bpc);
  unsigned i;
  rgb_t rgb;
  if (bpc==1) {
    img->setPal(0,0);
    img->setPal(1,0xffffffL);
  } else if (bpc==2) {
    img->setPal(0,0);
    img->setPal(1,0x555555L);
    img->setPal(2,0xaaaaaaL);
    img->setPal(3,0xffffffL);
  } else if (bpc==4) {
    for (i=0,rgb=0;i<16;i++,rgb+=(rgb_t)0x111111L) img->setPal(i,rgb);
  } else if (bpc==8) {
    for (i=0,rgb=0;i<256;i++,rgb+=(rgb_t)0x010101L) img->setPal(i,rgb);
  }
  memcpy(img->getRowbeg(), rowbeg, beg+len-rowbeg);
  return img;
}
bool Image::Gray::canGray() const { return true; }
void Image::Gray::copyRGBRow(char *to, Image::Sampled::dimen_t whichrow) const {
  param_assert(whichrow<ht);
  if (wd==0) return;
  unsigned char *p=(unsigned char*)rowbeg+rlen*whichrow;
  char *toend=to+3*wd;
  unsigned int i, j, k;

  if (bpc==1) {
    toend-=3*(wd&7);
    while (to!=toend) {
      i=*p++;
      k=(i>>7)*255;     *to++=k; *to++=k; *to++=k;
      k=((i>>6)&1)*255; *to++=k; *to++=k; *to++=k;
      k=((i>>5)&1)*255; *to++=k; *to++=k; *to++=k;
      k=((i>>4)&1)*255; *to++=k; *to++=k; *to++=k;
      k=((i>>3)&1)*255; *to++=k; *to++=k; *to++=k;
      k=((i>>2)&1)*255; *to++=k; *to++=k; *to++=k;
      k=((i>>1)&1)*255; *to++=k; *to++=k; *to++=k;
      k=(     i&1)*255; *to++=k; *to++=k; *to++=k;
    }
    i=*p; /* No mem overrun, even if (wd&7)==0 */
    j=wd&7;
    while (j--!=0) { k=(i>>7)*255; *to++=k; *to++=k; *to++=k; i<<=1; }
  } else if (bpc==2) {
    toend-=3*(wd&3);
    while (to!=toend) {
      i=*p++;
      k=(i>>6)*85;     *to++=k; *to++=k; *to++=k;
      k=((i>>4)&3)*85; *to++=k; *to++=k; *to++=k;
      k=((i>>2)&3)*85; *to++=k; *to++=k; *to++=k;
      k=(     i&3)*85; *to++=k; *to++=k; *to++=k;
    }
    i=*p; /* No mem overrun, even if (wd&7)==0 */
    j=wd&3;
    while (j--!=0) { k=(i>>6)*85; *to++=k; *to++=k; *to++=k; i<<=2; }
  } else if (bpc==4) {
    toend-=3*(wd&1);
    while (to!=toend) {
      i=*p++;
      k=(i>>4)*17;      *to++=k; *to++=k; *to++=k;
      k=(     i&15)*17; *to++=k; *to++=k; *to++=k;
    }
    if (0!=(wd&1)) { k=(*p>>4)*17; *to++=k; *to++=k; *to++=k; }
  } else if (bpc==8) {
    while (to!=toend) { *to++=*p; *to++=*p; *to++=*p++; }
  } else assert(0 && "invalid bpc");
}
#if 0
void Image::Gray::setBpc(unsigned char bpc_) {
  (void)bpc_;
  assert(0 && "unimplemented"); /* unimplemented */
}
#endif

Image::Sampled* Image::Gray::addAlpha(Image::Gray *al) {
  // Error::sev(Error::WARNING) << "Gray: alpha channel ignored" << (Error*)0; return this;
  if (al->getHt()!=ht || al->getWd()!=wd) Error::sev(Error::EERROR) << "addAlpha: image dimension mismatch" << (Error*)0;
  bool ign_mid=false;
  unsigned char lightest, darkest;
  al->to8();
  al->calcExtrema(lightest, darkest);
  if (darkest==255) return this; /* no transparent pixels at all */
  char *p=rowbeg, *pend=rowbeg+rlen*ht, *alq=al->getRowbeg();
  /* Imp: choose an image color instead of black... */
  /* Dat: 0..254: transparent, 255: opaque */
  while (p!=pend) {
    if ((unsigned char)(*alq+1)>1) ign_mid=true;
    /* fprintf(stderr,"alq=%u\n", (unsigned char)*alq); */
    if ((unsigned char)*alq++!=255) *p=0; /* black out transparent-wannabe pixels */
    p++;
  }
  if (ign_mid) Error::sev(Error::WARNING) << "addAlpha: half-transparent pixels made transparent" << (Error*)0;
  return addAlpha0(toIndexed(), al);
}

void Image::Gray::calcExtrema(unsigned char &lightest, unsigned char &darkest) {
  to8();
  unsigned l=0, d=255, val;
  char *p=rowbeg, *pend=rowbeg+ht*wd;
  while (p!=pend) {
    val=*(unsigned char*)p++;
    if (val>l) l=val;
    if (val<d) d=val;
  }
  lightest=l; darkest=d;
}


/* --- */

Image::RGB::RGB(Image::Sampled::dimen_t wd_, Image::Sampled::dimen_t ht_, unsigned char bpc_) {
  init(0,0,wd_,ht_,bpc_,TY_RGB,3);
  cs=CS_RGB;
}
void Image::RGB::to8() { to8mul(); }
Image::Gray*    Image::RGB::toGray(unsigned char bpc_)/* const*/ { return toGray0(bpc_); }
Image::RGB*     Image::RGB::toRGB(unsigned char bpc_)/* const*/ { return bpc==bpc_ ? this : toRGB0(bpc_); }
Image::Indexed* Image::RGB::toIndexed()/* const*/ { return toIndexed0(); }
bool Image::RGB::canGray() const { return false; }
void Image::RGB::copyRGBRow(char *to, Image::Sampled::dimen_t whichrow) const {
  param_assert(whichrow<ht);
  if (wd==0) return;
  unsigned char *p=(unsigned char*)rowbeg+rlen*whichrow;
  char *toend=to+3*wd;
  unsigned int i, j, k;

  if (bpc==1) {
    toend-=(wd*3)&7;
    while (to!=toend) {
      i=*p++;
      k=(i>>7)*255;     *to++=k;
      k=((i>>6)&1)*255; *to++=k;
      k=((i>>5)&1)*255; *to++=k;
      k=((i>>4)&1)*255; *to++=k;
      k=((i>>3)&1)*255; *to++=k;
      k=((i>>2)&1)*255; *to++=k;
      k=((i>>1)&1)*255; *to++=k;
      k=(     i&1)*255; *to++=k;
    }
    i=*p; /* No mem overrun, even if (wd&7)==0 */
    j=(wd*3)&7;
    while (j--!=0) { k=(i>>7)*255; *to++=k; i<<=1; }
  } else if (bpc==2) {
    toend-=(wd*3)&3;
    while (to!=toend) {
      i=*p++;
      k=(i>>6)*85;     *to++=k;
      k=((i>>4)&3)*85; *to++=k;
      k=((i>>2)&3)*85; *to++=k;
      k=(     i&3)*85; *to++=k;
    }
    i=*p; /* No mem overrun, even if (wd&7)==0 */
    j=(wd*3)&3;
    while (j--!=0) { k=(i>>6)*85; *to++=k; i<<=2; }
  } else if (bpc==4) {
    toend-=(wd*3)&7;
    while (to!=toend) {
      i=*p++;
      k=(i>>4)*17;      *to++=k;
      k=(     i&15)*17; *to++=k;
    }
    if (0!=((wd*3)&1)) *to++=(*p>>4)*17;
  } else if (bpc==8) {
    memcpy(to, p, 3*wd);
  } else assert(0 && "invalid bpc");
}
#if 0
void Image::RGB::setBpc(unsigned char bpc_) {
  (void)bpc_;
  assert(0 && "unimplemented"); /* unimplemented */
}
#endif

Image::Sampled* Image::RGB::addAlpha(Image::Gray *al) {
  if (al->getHt()!=ht || al->getWd()!=wd) Error::sev(Error::EERROR) << "addAlpha: image dimension mismatch" << (Error*)0;
  bool ign_mid=false;
  unsigned char lightest, darkest;
  al->to8();
  al->calcExtrema(lightest, darkest);
  if (darkest==255) return this; /* no transparent pixels at all */
  char *p=rowbeg, *pend=rowbeg+rlen*ht, *alq=al->getRowbeg();
  /* Imp: choose an image color instead of black... */
  /* Dat: 0..254: transparent, 255: opaque */
  while (p!=pend) {
    if ((unsigned char)(*alq+1)>1) ign_mid=true;
    /* fprintf(stderr,"alq=%u\n", (unsigned char)*alq); */
    if ((unsigned char)*alq++!=255) p[0]=p[1]=p[2]=0; /* black out transparent-wannabe pixels */
    p+=3;
  }
  if (ign_mid) Error::sev(Error::WARNING) << "addAlpha: half-transparent pixels made transparent" << (Error*)0;
  return addAlpha0(toIndexed(), al);
}

/* --- */

char *Image::Sampled::rgb2webhash(rgb_t rgb) {
  static char tmp[8];
  char *p=tmp;
  *p='#';
  *++p='0'+(rgb>>20);      if (*p>'9') *p+='a'-'0'-10;
  *++p='0'+((rgb>>16)&15); if (*p>'9') *p+='a'-'0'-10;
  *++p='0'+((rgb>>12)&15); if (*p>'9') *p+='a'-'0'-10;
  *++p='0'+((rgb>>8)&15);  if (*p>'9') *p+='a'-'0'-10;
  *++p='0'+((rgb>>4)&15);  if (*p>'9') *p+='a'-'0'-10;
  *++p='0'+((rgb   )&15);  if (*p>'9') *p+='a'-'0'-10;
  *++p='\0';
  return tmp;
}

GenBuffer::Writable& operator<<(GenBuffer::Writable& gw, Image::Sampled const& img) {
  slen_t buflen=img.getWd()*3;
  char *buf=new char[buflen];
  Image::Sampled::dimen_t y, ht=img.getHt();
  /* vvv in the xv program: image file must be >=30 bytes long to be treated as image */
  gw << "P6\n###############\n" << img.getWd() << ' ' << ht;
  if (img.getTranspc()>=0x1000000UL) gw << "\n#Opaque"; 
                                else gw << "\n#T" << img.rgb2webhash(img.getTranspc());
  gw << "\n255\n";  
  for (y=0; y<ht; y++) {
    img.copyRGBRow(buf, y);
    gw.vi_write(buf, buflen);
  }
  delete [] buf;
  return gw;
}

static Image::Loader *first=(Image::Loader*)NULLP;

void Image::register0(Image::Loader *anew) {
  param_assert(anew!=NULLP);
  anew->next=first;
  first=anew;
}

#if 0 /* removed by code refactoring */
Image::Sampled* Image::load(char const* format, filep_t f_, SimBuffer::Flat const& loadHints) {
#endif

// #include <unistd.h> /* sleep() */

#if 0
Rule::Sampled *Rule::load(char const* filename) {
  static char buf[2*Applier::MAGIC_LEN];
  FILE *f=fopen(filename, "rb");
  unsigned got=0;
  if (f==NULLP) Error::sev(Error::EERROR) << "Cannot open/read image file: " << FNQ(filename) << (Error*)0;
  slen_t ret=fread(buf, 1, Applier::MAGIC_LEN, f);
  /* vvv Imp: clarify error message: may be a read error */
  if (ret==0) Error::sev(Error::EERROR) << "Zero-length image file: " << FNQ(filename) << (Error*)0;
  if (ret<Applier::MAGIC_LEN) memset(buf+ret, '\0', Applier::MAGIC_LEN-ret);
#if 0
  unsigned long pos=fseek(f, 0, SEEK_END);
  pos=(pos<=Applier::MAGIC_LEN)?0:pos-Applier::MAGIC_LEN;
  if (0!=fseek(f, pos, SEEK_SET)
   || (got=fread(buf+Applier::MAGIC_LEN, 1, Applier::MAGIC_LEN, f))==0
#else
  if (0
#endif
   || (rewind(f), 0)
   || ferror(f))
    Error::sev(Error::EERROR) << "I/O error in image file: " << FNQ(filename) << (Error*)0;
  if (got!=0 && got!=Applier::MAGIC_LEN) memmove(buf+2*Applier::MAGIC_LEN-got, buf+Applier::MAGIC_LEN, got);
  Applier *p=first;
  Applier::reader_t reader;
  while (p!=NULLP) {
    if (NULLP!=(reader=p->checker(buf,buf+Applier::MAGIC_LEN))) { return reader(f); }
    p=p->next;
  }
  Error::sev(Error::EERROR) << "Unknown image format: " << FNQ(filename) << (Error*)0;
  // Error::sev(Error::WARNING) << "Zero-length image1." << (Error*)0;
  // Error::sev(Error::WARNING) << "Zero-length image2." << (Error*)0;
  return 0; /*notreached*/
}
#endif


Image::Sampled *Image::load(Image::Loader::UFD* ufd0, SimBuffer::Flat const& loadHints, char const* format) {
  Filter::UngetFILED &ufd=*(Filter::UngetFILED*)ufd0;
  /* Dat: format arg used in in_pnm.cpp */
  static char buf[Loader::MAGIC_LEN+1];
  slen_t ret=ufd.vi_read(buf, Loader::MAGIC_LEN);
  /* vvv Imp: clarify error message: may be a read error */
  if (ufd.hadError()) Error::sev(Error::EERROR) << "I/O error pre in image file: " << FNQ(ufd.getFilenameDefault("-")) << (Error*)0;
  if (ret==0) Error::sev(Error::EERROR) << "Zero-length image file: " << FNQ(ufd.getFilenameDefault("-")) << (Error*)0;
  if (ret<Loader::MAGIC_LEN) memset(buf+ret, '\0', Loader::MAGIC_LEN-ret);
  buf[Loader::MAGIC_LEN]='\0';
  /* Dat: do not read the trailer onto buf+Loader::MAGIC_LEN, because no ->checker() uses it yet. */
  Loader *p=first;
  Loader::reader_t reader;
  ufd.unread(buf, ret); /* tries to seek back, on failure calls ufd.getUnget().vi_write() */
  // ^^^ rewind(f); /* checker might have read */
  /* ^^^ do this early for the checkers */
  while (p!=NULLP) {
    /* vvv each checker() must rewind ufd for itself */
    if ((format==(char const*)NULLP || 0==strcmp(p->format, format))
     && (Loader::checker_t)NULLP!=p->checker
     && (Loader::reader_t)NULLP!=(reader=p->checker(buf,buf+Loader::MAGIC_LEN, loadHints, ufd0))
       ) {
      // fprintf(stderr, "%p %p\n", ufd0, &ufd);
      return reader(ufd0, loadHints);
    }
    p=p->next;
  }
  // sleep(1000);
  Error::sev(Error::EERROR) << "Unknown input image format: " << FNQ(ufd.getFilenameDefault("-")) << (Error*)0;
  return 0; /*notreached*/
}

#if 0 /* not used anywhere */
Image::Sampled *Image::load(char const* filename, SimBuffer::Flat const& loadHints, filep_t stdin_f, char const* format) {
  Filter::UngetFILED ufd(filename, stdin_f==NULLP ? stdin : (FILE*)stdin_f,
    Filter::UngetFILED::CM_closep|Filter::UngetFILED::CM_keep_stdinp);
  return load((Image::Loader::UFD*)&ufd, loadHints, format);
  // Imp: better error message, something like: if (f==NULLP) Error::sev(Error::EERROR) << "Cannot open/read image file: " << FNQ(filename) << (Error*)0;
}
#endif

#if 0 /* before Sat Apr 19 13:42:04 CEST 2003 */
Image::Sampled *Image::load(char const* filename, SimBuffer::Flat const& loadHints, filep_t stdin_f, char const* format) {
  /* Dat: format arg used in in_pnm.cpp */
  static char buf[2*Loader::MAGIC_LEN];
  bool stdin_p=filename[0]=='-' && filename[1]=='\0';
  FILE *f=!stdin_p ? fopen(filename, "rb") : stdin_f!=NULLP ? (FILE*)stdin_f : stdin;
  unsigned got=0;
  if (f==NULLP) Error::sev(Error::EERROR) << "Cannot open/read image file: " << FNQ(filename) << (Error*)0;
  slen_t ret=fread(buf, 1, Loader::MAGIC_LEN, f);
  /* vvv Imp: clarify error message: may be a read error */
  if (ret==0) Error::sev(Error::EERROR) << "Zero-length image file: " << FNQ(filename) << (Error*)0;
  if (ret<Loader::MAGIC_LEN) memset(buf+ret, '\0', Loader::MAGIC_LEN-ret);
#if 0
  /* Dat: do not read the trailer, because no ->checker() uses it yet. */
  unsigned long pos=fseek(f, 0, SEEK_END);
  pos=(pos<=Loader::MAGIC_LEN)?0:pos-Loader::MAGIC_LEN;
  if (0!=fseek(f, pos, SEEK_SET)
   || (got=fread(buf+Loader::MAGIC_LEN, 1, Loader::MAGIC_LEN, f))==0
#else
  if (0
#endif
   || (rewind(f), 0)
   || ferror(f))
    Error::sev(Error::EERROR) << "I/O error pre in image file: " << FNQ(filename) << (Error*)0;
  if (got!=0 && got!=Loader::MAGIC_LEN) memmove(buf+2*Loader::MAGIC_LEN-got, buf+Loader::MAGIC_LEN, got);
  Loader *p=first;
  Loader::reader_t reader;
  while (p!=NULLP) {
    if ((format==(char const*)NULLP || 0==strcmp(p->format, format))
     && (Loader::checker_t)NULLP!=p->checker
     && (Loader::reader_t)NULLP!=(reader=p->checker(buf,buf+Loader::MAGIC_LEN, loadHints, f))
       ) {
      rewind(f); /* checker might have read */
      Image::Sampled *ret=reader(f, loadHints);
      if (ferror(f) || (!stdin_p && 0!=fclose(f))) /* don't close stdin */
        Error::sev(Error::EERROR) << "I/O error post in image file: " << FNQ(filename) << (Error*)0;
      return ret;
    }
    p=p->next;
  }
  Error::sev(Error::EERROR) << "Unknown input image format: " << FNQ(filename) << (Error*)0;
  // Error::sev(Error::WARNING) << "Zero-length image1." << (Error*)0;
  // Error::sev(Error::WARNING) << "Zero-length image2." << (Error*)0;
  return 0; /*notreached*/
}
#endif

unsigned Image::printLoaders(GenBuffer::Writable &out) {
  unsigned num=0;
  Loader *p=first;
  while (p!=NULLP) {
    if (p->checker!=(Loader::checker_t)NULLP && p->format!=(char const*)NULLP) { num++; out << ' ' << p->format; }
    p=p->next;
  }
  return num;
}

/* --- */

Image::SampledInfo::SampledInfo(Sampled *img_)
 :hasTransp(false)
 ,nncols(257)
 ,canGray(false)
 ,minRGBBpc(8)
 ,img(img_)
 ,imgs((Indexed**)NULLP) {
  param_assert(img_!=NULLP);
  Sampled *bak=img;
  if ((img=img->toIndexed())==NULLP) {
    img=bak;
  } else {
    if (bak!=img) delete bak;
    assert(img->getTy()==img->TY_INDEXED);
    Indexed *iimg=PTS_dynamic_cast(Image::Indexed*,img);
    iimg->packPal();
    nncols=iimg->getNcols();
    if (true==(hasTransp=iimg->hasTransp())) nncols--;
  }
  minRGBBpc=img->minRGBBpc();
  if ((canGray=img->canGray())==true && nncols==257) nncols=256;
  sf=(sf_t)((img->getTy()==img->TY_BLACKBOX) ? 0+SF_Asis : 0+SF_None);
  /* Dat: 0+: pacify gcc-3.1 */
}

Image::SampledInfo::~SampledInfo() {
  delete img;
  if (imgs!=NULLP) { Image::Indexed::delete_separated(imgs); delete imgs; }
}

void Image::SampledInfo::separate() {
  // bool ok;
  // ASSERT_SIDE(ok=(sf!=SF_Transparent2 && sf!=SF_Transparent4 && sf!=SF_Transparent8));
  // if (!ok) return;
  if (sf!=SF_Transparent2 && sf!=SF_Transparent4 && sf!=SF_Transparent8) return;
  Indexed *iimg=PTS_dynamic_cast(Indexed*,img);
  imgs=iimg->separate();
}

bool Image::SampledInfo::setSampleFormat(sf_t sf_, bool WarningOK, bool TryOnly, Sampled::rgb_t Transparent) {
  /* at Sat Jun 15 11:48:51 CEST 2002: added transparency warnings */
  /* fprintf(stderr, "sf=%u sf_=%u transparent=0x%lx\n", sf, sf_, Transparent+0UL); */
  // assert(sf_==SF_Asis);
  Indexed *iimg;
  Sampled *bak=img;
  param_assert(sf_!=SF_None);
  param_assert(sf==SF_None || sf==SF_Asis || sf==sf_);
  if (sf_==sf) return true; /* already converted */
  bool zero=img->getWd()==0 || img->getHt()==0;
  if (sf==SF_Asis && sf_!=SF_Asis && sf_!=SF_Bbox) {
    Error::sev(Error::WARNING) << "SampleFormat: can't convert image loaded as Asis to other" << (Error*)0;
    return false;
  }
  switch (sf_) {
   case SF_Bbox:
    sf=SF_Bbox; return true;
   case SF_Opaque:
    if (!hasTransp && nncols==1 && PTS_dynamic_cast(Indexed*,img)->setTranspc(Transparent)) { hasTransp=true; nncols=0; }
    if (hasTransp || nncols!=1) return false;
    assert(img->getTy()==img->TY_INDEXED);
    /* The color can be calculated: PTS_dynamic_cast(Indexed*>(img)->getPal(0); */
    /* Conversion is not necessary. */
    sf=SF_Opaque; return true;
   case SF_Transparent:
    if (!hasTransp && nncols==1 && PTS_dynamic_cast(Indexed*,img)->setTranspc(Transparent)) { hasTransp=true; nncols=0; }
    if (!hasTransp || nncols!=0) return false;
    assert(img->getTy()==img->TY_INDEXED);
    /* Conversion is not necessary. */
    sf=SF_Transparent; return true;
   case SF_Gray1:
    /* vvv strict hasTransp added at Mon Sep  9 22:53:24 CEST 2002 */
    if (nncols>2 || !canGray || minRGBBpc>1 || zero || hasTransp || img->hasPixelRGB(Transparent)) return false;
    /* ^^^ Imp: !! make the hasPixelRFB() check a lot faster (cache results) */
    if (TryOnly) return WarningOK || (nncols>=2 && !hasTransp);
    if (hasTransp) {
      if (!WarningOK) return false;
      /* Dat: nncols may be 1 or 2 ! (both transparent and opaque black) */
      Error::sev(Error::WARNING) <<
        (nncols<=1 ? "SampleFormat: Mask would be better than "
                   : "SampleFormat: Transparent2 would be better than ")
        << "Gray1" << (Error*)0;
    }
    if (nncols<2) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: Opaque would be better than Gray1" << (Error*)0;
    }
    img=img->toGray(1); if (bak!=img) delete bak;
    assert(img!=NULLP);
    assert(img->getBpc()==1);
    sf=SF_Gray1; return true;
   case SF_Indexed1:
    if (nncols>2 || zero || hasTransp || img->hasPixelRGB(Transparent)) return false;
    if (TryOnly) return WarningOK || nncols>=2;
    if (nncols<2) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: Opaque would be better than Indexed1" << (Error*)0;
    }
    if (canGray && minRGBBpc==1) {
      Error::sev(Error::NOTICE) << "SampleFormat: Gray1 would be better than Indexed1" << (Error*)0;
    }
    assert(img->getTy()==img->TY_INDEXED);
    // img=img->toIndexed(); /* should be a no-op */
    // assert(img!=NULLP); if (bak!=img) delete bak;
    { iimg=PTS_dynamic_cast(Indexed*,img);
      iimg->setBpc(1);
      if (iimg->setTranspc(Transparent)) return false; /* Dat: false if must be changed to become transparent; Imp: undo changes */
    }
    sf=SF_Indexed1; return true;
   case SF_Mask:
    if (!hasTransp && nncols==2 && PTS_dynamic_cast(Indexed*,img)->setTranspc(Transparent)) { hasTransp=true; nncols=1; }
    if (nncols>1 || zero) return false;
    if (TryOnly) return WarningOK || nncols+(hasTransp?1:0)==2;
    if (nncols==1 && !hasTransp) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: Opaque would be better than Mask" << (Error*)0;
    }
    if (nncols==0) {
      assert(hasTransp);
      Error::sev(Error::WARNING) << "SampleFormat: Transparent would be better than Mask" << (Error*)0;
    }
    assert(img->getTy()==img->TY_INDEXED);
    { iimg=PTS_dynamic_cast(Indexed*,img);
      iimg->setBpc(1);
      if (!iimg->setTranspc(Transparent)) return false;
      iimg->packPal();
    }
    /* printf("gett=%d\n", PTS_dynamic_cast(Indexed*,img)->getTransp()); */
    /* vvv BUGFIX: <1U -> <2U */
    assert(PTS_dynamic_cast(Indexed*,img)->getTransp()==-1 || PTS_dynamic_cast(Indexed*,img)->getTransp()+0U<2U);
    /* ^^^ color 0 is opaque, color 1 is transparent, thanks to
     * img->packPal() called in SampleInfo() -- but setTranspc may have changed this
     */
    sf=SF_Mask; return true;
   case SF_Transparent2:
    if (nncols==4) {
      (iimg=PTS_dynamic_cast(Indexed*,img))->setTranspc(Transparent); /* Imp: are we Indexed*?? */
      hasTransp=iimg->hasTransp();
    }
    if (nncols>3 || zero) return false;
    if (TryOnly) return WarningOK || (hasTransp && nncols>=2);
    Error::sev(Error::NOTICE) << "SampleFormat: Transparent2 separates colors" << (Error*)0;
    if (!hasTransp) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: Indexed2 would be better than Transparent2" << (Error*)0;
    }
    if (nncols<2) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: Mask would be better than Transparent2" << (Error*)0;
    }
    assert(img->getTy()==img->TY_INDEXED);
    { iimg=PTS_dynamic_cast(Indexed*,img);
      iimg->setBpc(2); /* BUGFIX at Sat Jun 15 13:55:25 CEST 2002 */
      iimg->setTranspc(Transparent);
      // imgs=iimg->separate(); /* postponed because of GIF89a output */
    }
    sf=SF_Transparent2; return true;
   case SF_Gray2:
    if (nncols>4 || !canGray || minRGBBpc>2 || zero || hasTransp || img->hasPixelRGB(Transparent)) return false;
    if (TryOnly) return WarningOK || (nncols>2 && !hasTransp);
    if (hasTransp) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) <<
        (nncols<=3 ? "SampleFormat: Transparent2 would be better than "
                   : "SampleFormat: Transparent4 would be better than ")
        << "Gray2" << (Error*)0;
    }
    if (nncols<=2) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: Gray1 would be better than Gray2" << (Error*)0;
    }
    img=img->toGray(2); if (bak!=img) delete bak;
    assert(img!=NULLP);
    sf=SF_Gray2; return true;
   case SF_Indexed2:
    if (nncols>4 || zero || hasTransp || img->hasPixelRGB(Transparent)) return false;
    if (TryOnly) return WarningOK || nncols>2;
    if (nncols<=2) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: Indexed1 would be better than Indexed2" << (Error*)0;
    }
    if (canGray && minRGBBpc<=2) {
      Error::sev(Error::NOTICE) << "SampleFormat: Gray2 would be better than Indexed2" << (Error*)0;
    }
    assert(img->getTy()==img->TY_INDEXED);
    { iimg=PTS_dynamic_cast(Indexed*,img);
      iimg->setBpc(2);
      if (iimg->setTranspc(Transparent)) return false;
    }
    sf=SF_Indexed2; return true;
   case SF_Transparent4:
    if (nncols==16) PTS_dynamic_cast(Indexed*,img)->setTranspc(Transparent);
    if (nncols>15 || zero) return false;
    if (TryOnly) return WarningOK || (hasTransp && nncols>=4);
    Error::sev(Error::NOTICE) << "SampleFormat: Transparent4 separates colors" << (Error*)0;
    if (!hasTransp) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: Indexed4 would be better than Transparent4" << (Error*)0;
    }
    if (nncols<4) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: Transparent2 would be better than Transparent4" << (Error*)0;
    }
    assert(img->getTy()==img->TY_INDEXED);
    { iimg=PTS_dynamic_cast(Indexed*,img);
      iimg->setBpc(4);
      iimg->setTranspc(Transparent);
      // imgs=iimg->separate(); /* postponed because of GIF89a output */
    }
    sf=SF_Transparent4; return true;
   case SF_Rgb1:
    if (nncols>8 || minRGBBpc>1 || zero || hasTransp || img->hasPixelRGB(Transparent)) return false;
    if (TryOnly) return WarningOK || (nncols>4 && !canGray && !hasTransp);
    if (hasTransp) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: Transparent4 would be better than " << "Rgb1" << (Error*)0;
    }
    if (canGray) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: Gray1 would be better than Rgb1" << (Error*)0;
    }
    if (nncols<=4) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: "
        << (hasTransp ? "Transparent2 may" : "Indexed2 would")
        << " be better than Rgb1" << (Error*)0;
    }
    img=img->toRGB(1); if (bak!=img) delete bak;
    assert(img!=NULLP);
    sf=SF_Rgb1; return true;
   case SF_Gray4:
    if (nncols>16 || !canGray || minRGBBpc>4 || zero || hasTransp || img->hasPixelRGB(Transparent)) return false;
    /* ^^^ BUGFIX at Sat Jun  1 18:27:10 CEST 2002 */
    if (TryOnly) return WarningOK || (nncols>4 && !hasTransp);
    if (hasTransp) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) <<
        (nncols<=15 ? "SampleFormat: Transparent4 would be better than "
                    : "SampleFormat: Transparent8 may be better than ")
        << "Gray4" << (Error*)0;
    }
    if (nncols<=4) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: Gray2 would be better than Gray4" << (Error*)0;
    }
    img=img->toGray(4); if (bak!=img) delete bak;
    assert(img!=NULLP);
    sf=SF_Gray4; return true;
   case SF_Indexed4:
    if (nncols>16 || zero || hasTransp || img->hasPixelRGB(Transparent)) return false;
    if (TryOnly) return WarningOK || (nncols>4 && minRGBBpc>=4);
    if (nncols<=4) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: Indexed2 would be better than Indexed4" << (Error*)0;
    }
    if (minRGBBpc<=1) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: RGB1 would be better than Indexed4" << (Error*)0;
    }
    if (canGray && minRGBBpc<=4) {
      if (!WarningOK) return false;
      Error::sev(Error::NOTICE) << "SampleFormat: Gray4 would be better than Indexed4" << (Error*)0;
    }
    assert(img->getTy()==img->TY_INDEXED);
    { iimg=PTS_dynamic_cast(Indexed*,img);
      iimg->setBpc(4);
      if (iimg->setTranspc(Transparent)) return false;
    }
    sf=SF_Indexed4; return true;
   case SF_Transparent8:
    if (nncols==256) PTS_dynamic_cast(Indexed*,img)->setTranspc(Transparent);
    if (nncols>255 || zero) return false;
    if (!WarningOK) return false;
    if (TryOnly) return true;
    Error::sev(Error::WARNING) << "SampleFormat: Transparent8 separates too many colors" << (Error*)0;
    if (!hasTransp) {
      Error::sev(Error::WARNING) << "SampleFormat: Indexed8 would be much better than Transparent8" << (Error*)0;
    }
    if (nncols<16) {
      Error::sev(Error::WARNING) << "SampleFormat: Transparent4 would be better than Transparent8" << (Error*)0;
    }
    assert(img->getTy()==img->TY_INDEXED);
    { iimg=PTS_dynamic_cast(Indexed*,img);
      iimg->setBpc(8); /* should be a no-op */
      iimg->setTranspc(Transparent);
      // imgs=iimg->separate(); /* postponed because of GIF89a output */
    }
    sf=SF_Transparent8; return true;
   case SF_Rgb2:
    if (nncols>64 || minRGBBpc>2 || zero || hasTransp || img->hasPixelRGB(Transparent)) return false;
    if (TryOnly) return WarningOK || (nncols>16 && !canGray && !hasTransp);
    if (hasTransp) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: Transparent8 would be better than " << "Rgb2" << (Error*)0;
    }
    if (canGray) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: Gray2 would be better than Rgb2" << (Error*)0;
    }
    if (nncols<=16) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: "
        << (hasTransp ? "Transparent4 may" : "Indexed4 would")
        << " be better than Rgb2" << (Error*)0;
    }
    img=img->toRGB(2); if (bak!=img) delete bak;
    assert(img!=NULLP);
    sf=SF_Rgb2; return true;
   case SF_Gray8:
    if (nncols>256 || !canGray || zero || hasTransp || img->hasPixelRGB(Transparent)) return false;
    if (TryOnly) return WarningOK || (nncols>16 && !hasTransp);
    if (hasTransp) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) <<
        (nncols<=255 ? "SampleFormat: Transparent8 may be better than "
                     : "SampleFormat: ignoring transparency for ")
        << "Gray8" << (Error*)0;
    }
    if (minRGBBpc<=4) {
      /* BUGFIX at Wed Jul  3 01:07:44 CEST 2002 */
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: Gray4 would be better than Gray8" << (Error*)0;
    }
    img=img->toGray(8); if (bak!=img) delete bak;
    assert(img!=NULLP);
    sf=SF_Gray8; return true;
   case SF_Indexed8:
    // fprintf(stderr, "nncols=%u hasTransp=%u zero=%u\n", nncols, hasTransp, zero);
    if (nncols>256 || zero || hasTransp || img->hasPixelRGB(Transparent)) return false;
    // assert(0);
    if (TryOnly) return WarningOK || (nncols>16 && minRGBBpc>=8);
    if (nncols<=16) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: Indexed4 would be better than Indexed8" << (Error*)0;
    }
    if (minRGBBpc<=2) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: RGB2 would be better than Indexed8" << (Error*)0;
    }
    if (canGray) {
      if (!WarningOK) return false;
      Error::sev(Error::NOTICE) << "SampleFormat: Gray8 would be better than Indexed8" << (Error*)0;
    }
    assert(img->getTy()==img->TY_INDEXED);
    { iimg=PTS_dynamic_cast(Indexed*,img);
      iimg->setBpc(8); /* should be a no-op */
      if (iimg->setTranspc(Transparent)) return false;
    }
    sf=SF_Indexed8; return true;
   case SF_Rgb4:
    // fprintf(stderr, "minrgbbpc=%d to=%d\n", minRGBBpc, TryOnly);
    if (minRGBBpc>4 || zero || hasTransp || img->hasPixelRGB(Transparent)) return false;
    if (TryOnly) return WarningOK || (nncols>256 && !canGray && !hasTransp);
    if (hasTransp) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) <<
        (nncols<=255 ? "SampleFormat: Transparent8 may be better than "
                     : "SampleFormat: ignoring transparency for ")
        << "Rgb4" << (Error*)0;
    }
    if (canGray) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: Gray4 would be better than Rgb4" << (Error*)0;
    }
    if (nncols<=256) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: "
        << (hasTransp && nncols<=255 ? "Transparent8 may" : "Indexed8 would")
        << " be better than Rgb4" << (Error*)0;
    }
    img=img->toRGB(4); if (bak!=img) delete bak;
    assert(img!=NULLP);
    sf=SF_Rgb4; return true;
   case SF_Rgb8:
    if (zero || hasTransp || img->hasPixelRGB(Transparent)) return false;
    if (TryOnly) return WarningOK || (nncols>256 && !canGray && minRGBBpc>=8 && !hasTransp);
    if (hasTransp) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) <<
        (nncols<=255 ? "SampleFormat: Transparent8 may be better than "
                     : "SampleFormat: ignoring transparency for ")
        << "Rgb8" << (Error*)0;
    }
    if (canGray) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: Gray8 would be better than Rgb8" << (Error*)0;
    }
    if (minRGBBpc<=4) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: Rgb4 would be better than Rgb8" << (Error*)0;
    }
    if (nncols<=256) {
      if (!WarningOK) return false;
      Error::sev(Error::WARNING) << "SampleFormat: "
        << (hasTransp && nncols<=255 ? "Transparent8 may" : "Indexed8 would")
        << " be better than Rgb8" << (Error*)0;
    }
    img=img->toRGB(8); if (bak!=img) delete bak;
    assert(img!=NULLP);
    sf=SF_Rgb8; return true;
   case SF_Asis:
    if (img->getTy()!=img->TY_BLACKBOX) {
      Error::sev(Error::WARNING) << "SampleFormat: cannot convert image to /Asis" << (Error*)0;
      return false;
    }
    sf=SF_Asis; return true;
  }
  assert(0 && "unknown SampleFormat requested");  
  return false; /* NOTREACHED */
}

/* __END__ */
