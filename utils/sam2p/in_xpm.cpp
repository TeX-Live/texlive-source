/*
 * in_xpm.cpp -- read an XPM file
 * by pts@math.bme.hu at Fri Mar  1 09:56:54 CET 2002
 */
/* Imp: test this code with various xpm files! */

#ifdef __GNUC__
#pragma implementation
#endif

#include "image.hpp"

#if USE_IN_XPM

#if OBJDEP
#  warning REQUIRES: mapping.o
#endif

#ifndef USE_BIG_MEMORY
#define USE_BIG_MEMORY 0
#endif

#include "mapping.hpp"
#include "error.hpp"
#include "xpmc.h"

#include <string.h> /* memchr() */
#include "gensio.hpp"

#define USGE(a,b) ((unsigned char)(a))>=((unsigned char)(b))

#if 0
/** @return true iff params are different strings (not respecting case) */
static int my_strcase_neq(char *s1, char *s2) {
  while ((USGE(*s1,'A') && USGE('Z',*s1) ? *s1+'a'-'A' : *s1) ==
         (USGE(*s2,'A') && USGE('Z',*s2) ? *s2+'a'-'A' : *s2)) {
    if (*s1=='\0') return 0;
    s1++; s2++;
  }
  return 1;
}
#endif

#define my_strcase_neq(s1,s2) GenBuffer::nocase_strcmp(s1,s2)

/** @ return RGB long */
static Image::Sampled::rgb_t parse_rgb(char const*s) {
  unsigned v=0, len;
  Image::Sampled::rgb_t ret;
  if (!s || !*s) return 0x2000000; /* not found */
  // fprintf(stderr, "'%s'\n", s);
  if (*s=='#') { /* an #RRGGBB web-style color spec; or #RRRRGGGGBBBB */
    unsigned dif=0;
    ++s;
    while (dif<13 && (USGE(5,(s[dif]|32)-'a') || USGE(9,s[dif]-'0'))) dif++; /* find at most 13 hex digits */
    if (s[dif]!='\0' || (dif!=12 && dif!=6)) return 0x2000000; /* not found, spec length error */
    dif=(dif==12) ? 3 : 1;
    // shr=24; while (shr!=0) {
    ret= (Image::Sampled::rgb_t)( USGE(9,*s-'0') ? *s-'0' : 10+(*s|32)-'a' )<<20;
    s++;
    ret|=(Image::Sampled::rgb_t)( USGE(9,*s-'0') ? *s-'0' : 10+(*s|32)-'a' )<<16;
    s+=dif; /* ignore lower two hex digits of 16-bit sample value */
    ret|=(Image::Sampled::rgb_t)( USGE(9,*s-'0') ? *s-'0' : 10+(*s|32)-'a' )<<12;
    s++;
    ret|=(Image::Sampled::rgb_t)( USGE(9,*s-'0') ? *s-'0' : 10+(*s|32)-'a' )<<8;
    s+=dif; /* ignore lower two hex digits of 16-bit sample value */
    ret|=(Image::Sampled::rgb_t)( USGE(9,*s-'0') ? *s-'0' : 10+(*s|32)-'a' )<<4;
    s++;
    ret|=(Image::Sampled::rgb_t)( USGE(9,*s-'0') ? *s-'0' : 10+(*s|32)-'a' );
    return ret;
  }
  /* vvv 223==255-32: ignore case when hashing */
  char const *p=s;
  while (*p!='\0') v=xpmColors_mul*v+(223&*(unsigned char const*)p++);
  p=xpmColors_dat+xpmColors_ofs[(v&65535)%xpmColors_mod];
  while (*p!='\0') {
    len=strlen(p);
    if (0==my_strcase_neq(p,s)) {
      p+=len;
      ret=(((Image::Sampled::rgb_t)((unsigned char const*)p)[1])<<16)+
          (((Image::Sampled::rgb_t)((unsigned char const*)p)[2])<<8)+
          (((Image::Sampled::rgb_t)((unsigned char const*)p)[3]));
      return (ret==0x30201) ? 0x1000000 : ret; /* transparent color */
    }
    p+=len+4;
  }
  return 0x2000000; /* not found */
}

class XPMTok {
 public:
  /* Imp: report line numbers on errors */
  BEGIN_STATIC_ENUM1(int)
    T_NO_UNGOT=-1,
    T_COMMA=257 /* The comma token, outside strings. */
  END_STATIC_ENUM()
  /* XPM states */
  BEGIN_STATIC_ENUM1(char)
    ST_OUT=0, /* outside s string */
    ST_STR=1, /* inside a string */
    ST_EOF=2 /* EOF */
  END_STATIC_ENUM()
  int getcc();
  /** Reads an unsigned int. */
  Image::Sampled::dimen_t getDimen();
  Image::Sampled::rgb_t getColor();
  void read(char *buf, unsigned len);
  inline void readInStr(char *buf, unsigned len);
  inline XPMTok(FILE *f_): f(f_), state(ST_OUT), ungot(T_NO_UNGOT) {}
  inline void ungetcc(int c) { if (c>=0) ungot=c; }
  void getComma();
 protected:
  FILE *f;
  /** Current state. */
  char state;
  int ungot;
};

int XPMTok::getcc() {
  int i;
  if (ungot>=0 /*T_NO_UNGOT<0*/) { i=ungot; ungot=T_NO_UNGOT; return i; }
  /* Imp: ignore C and C++ style comments (so we won't recognise strings inside comments) */
  switch (state) {
   case ST_OUT: st_out:
    while (1) {
      switch ((i=MACRO_GETC(f))) {
       case -1: state=ST_EOF; return -1;
       case ',': return T_COMMA;
       case '"': state=ST_STR; goto st_str;
       default: /* ignore outside strings */ break;
      }
    }
   case ST_STR: st_str:
    i=MACRO_GETC(f);
    if (i==-1) { ue: Error::sev(Error::EERROR) << "XPM: unexpected EOF" << (Error*)0; }
    else if (i=='"') { state=ST_OUT; goto st_out; }
    else if (i=='\\') {
      if ((i=MACRO_GETC(f))==-1) goto ue;
      if (i=='\n') goto st_str;
      /* Imp: handle octal, hexadecimal, \n etc. */
    }
    return i&255;
   default: return -1;
  }
}
Image::Sampled::dimen_t XPMTok::getDimen() {
  Image::Sampled::dimen_t ret=0, bak;
  int i;
  while ((i=getcc())==' ' || i=='\t') ;
  if (USGE(i,'0') && USGE('9',i)) {
    ret=i-'0';
    while (USGE((i=getcc()),'0') && USGE('9',i)) {
      bak=ret;
      ret=ret*10+(i-'0');
      if (ret/10!=bak) Error::sev(Error::EERROR) << "XPM: dimen overflow" << (Error*)0;
    }
    ungetcc(i);
    return ret;
  } else Error::sev(Error::EERROR) << "XPM: dimen expected" << (Error*)0;
  return 0; /*notreached*/
}
void XPMTok::getComma() {
  if (getcc()!=T_COMMA) Error::sev(Error::EERROR) << "XPM: comma expected at " << ftell(f) << (Error*)0;
}
void XPMTok::read(char *buf, unsigned len) {
  int i;
  while (len--!=0) {
    i=getcc();
    // fprintf(stderr,"i=%d\n", i);
    if (i>=0 && i<=255) *buf++=i; else Error::sev(Error::EERROR) << "XPM: data expected" << (Error*)0;
  }
}
void XPMTok::readInStr(char *buf, unsigned len) {
  /* Dat: this is OK noew */
  // assert(state==ST_STR);
  assert(ungot<0);
  param_assert(len>=1);
  if (state!=ST_STR) { len--; goto real; }
  int i;
  while (len--!=0) {
    if ((i=MACRO_GETC(f))>=0 && i<=255 && i!='"' && i!='\\') *buf++=i;
    else { assert(i>=0); ungetc(i,f); real:
      i=getcc();
      // fprintf(stderr,"i=%d\n", i);
      if (i>=0 && i<=255) *buf++=i; else Error::sev(Error::EERROR) << "XPM: data expected" << (Error*)0;
    }
  }
}
Image::Sampled::rgb_t XPMTok::getColor() {
  static char tmp[32];
  int i;
  while ((i=getcc())==' ' || i=='\t') ;
  if (i=='g') { at_g:
    i=getcc();
    if (i!='4') ungetcc(i);
    goto at_col;
  } else if (i=='c' || i=='m' || i=='b' || i=='s') { at_col:
    while ((i=getcc())==' ' || i=='\t') ;
    char *p=tmp, *pend=tmp+sizeof(tmp)-1;
    while (i>=33 && i<=126) {
      if (p==pend) goto cexp; /* color name too long */
      *p++=i;
      i=getcc();
    }
    *p='\0';
    if (i==' ' || i=='\t') { /* Maybe another color will come */
      while ((i=getcc())==' ' || i=='\t') ;
      if (i=='g') goto at_g;
      else if (i=='c' || i=='m' || i=='b' || i=='s') goto at_col;
    }
    if (i!=T_COMMA) goto cexp;
    Image::Sampled::rgb_t ret=parse_rgb(tmp);
    if (ret==0x2000000) Error::sev(Error::EERROR) << "XPM: unknown color: " << tmp << (Error*)0;
    return ret;
  } else { cexp: Error::sev(Error::EERROR) << "XPM: color expected" << (Error*)0; }
  return 0; /*notreached*/
}

static Image::Sampled *in_xpm_reader(Image::Loader::UFD *ufd, SimBuffer::Flat const&) {
  // Error::sev(Error::EERROR) << "Cannot load XPM images yet." << (Error*)0;
  XPMTok tok(((Filter::UngetFILED*)ufd)->getFILE(/*seekable:*/false));
  Image::Sampled::dimen_t wd=tok.getDimen();
  Image::Sampled::dimen_t ht=tok.getDimen();
  Image::Sampled::dimen_t colors=tok.getDimen();
  Image::Sampled::dimen_t cpp=tok.getDimen(); /* chars per pixel */

  /* width height ncolors cpp [x_hot y_hot] */
  int i; /* multiple purpose */
  while ((i=tok.getcc())==' ' || i=='\t' || USGE(9,i-'0')) ;
  tok.ungetcc(i); tok.getComma();
  
  // Error::sev(Error::DEBUG) << "wd="<<wd<<" ht="<<ht<<" colors="<<colors<<" cpp="<<cpp << (Error*)0;
  if (1UL*cpp*colors>65535) Error::sev(Error::EERROR) << "XPM: too many colors" << (Error*)0;
  // if (cpp==1) {
  // }
  Image::Sampled::dimen_t transp=colors; /* No transparent colors yet. */
  /* vvv Dat: last cpp bytes of tab are used for storing current pixel. */
  char *tab=new char[cpp*(colors+1)], *p, *pend; /* BUGFIX: unsinged at Fri Nov 26 12:18:21 CET 2004 */
  Image::Sampled::rgb_t *rgb=new Image::Sampled::rgb_t[colors], *rp;
  for (rp=rgb,p=tab,pend=tab+cpp*colors; p!=pend; p+=cpp,rp++) {
    tok.read(p,cpp);
    *rp=tok.getColor();
    if (*rp==0x1000000) {
      if (transp!=colors) Error::sev(Error::WARNING) << "XPM: subsequent transparency might be blacked" << (Error*)0;
                     else transp=rp-rgb;
    }
  }
  /* Now: transp: index of the last transparent color */
  /* Dat: since (0x1000000&0xffffff)==0, transparent colors will have palette enrty black */

  char *outbuf=0; /* avoid gcc warning */
  Image::Sampled *ret;
  Image::Indexed *iimg=(Image::Indexed*)NULLP;
  tok.ungetcc(tok.T_COMMA);

  if (colors<=256) {
    ret=iimg=new Image::Indexed(wd,ht,colors,8);
    if (transp!=colors) iimg->setTransp(transp);
  } else {
    ret=new Image::RGB(wd,ht,8);
    if (transp!=colors) Error::sev(Error::WARNING) << "XPM: too many colors, transparency blacked" << (Error*)0;
  }
  outbuf=ret->getRowbeg();
  
  if (cpp==1) { /* Easy job: make an Indexed image; defer .packPal() */
    assert(colors<=256);
    signed short bin[256], s;
    memset(bin, 255, sizeof(bin)); /* Make bin[*]=-1 */
    for (i=0;(unsigned)i<colors;i++) {
      iimg->setPal(i, rgb[i]);
      bin[(unsigned char)tab[i]]=i;
    }
    assert(p==pend);
    while (ht--!=0) {
      tok.getComma();
      for (p=outbuf+ret->getRlen(); outbuf!=p; ) {
        if ((i=tok.getcc())<0 || i>255) Error::sev(Error::EERROR) << "XPM: data expected" << (Error*)0;
        if ((s=bin[i])<0) Error::sev(Error::EERROR) << "XPM: unpaletted color" << (Error*)0;
        *outbuf++=s;
      }
    }
  #if USE_BIG_MEMORY
   } else if (cpp==2 && colors<=256) { /* Similarly easy job: make an Indexed image; defer .packPal() */
    signed short *bin=new short[65536], s;
    memset(bin, 255, sizeof(bin)); /* Make bin[*]=-1 */
    for (i=0,p=tab; (unsigned)i<colors; i++, p+=2) {
      iimg->setPal(i, rgb[i]);
      bin[(p[0]<<8)+p[1]]=i;
    }
    assert(p==pend);
    while (ht--!=0) {
      tok.getComma();
      for (p=outbuf+ret->getRlen(); outbuf!=p; ) {
        tok.readInStr(pend,2);
        if ((s=bin[(pend[0]<<8)+pend[1]])<0) Error::sev(Error::EERROR) << "XPM: unpaletted color" << (Error*)0;
        *outbuf++=s;
      }
    }
    delete [] bin;
  } else if (cpp==2 && colors<=65535) {
    Image::Sampled::rgb_t rgb1;
    unsigned short *bin=new unsigned short[65536], s;
    memset(bin, 255, sizeof(bin)); /* Make bin[*]=max */
    for (i=0,p=tab; (unsigned)i<colors; i++, p+=2) bin[(p[0]<<8)+p[1]]=i;
    while (ht--!=0) {
      tok.getComma();
      for (p=outbuf+ret->getRlen(); outbuf!=p; ) {
        tok.readInStr(pend,2);
        if ((s=bin[(pend[0]<<8)+pend[1]])==(unsigned short)-1) Error::sev(Error::EERROR) << "XPM: unpaletted color" << (Error*)0;
        *outbuf++=(rgb1=rgb[s])>>16;
        *outbuf++=rgb1>>8;
        *outbuf++=rgb1;
      }
    }
    delete [] bin;
   #endif /* USE_BIG_MEMORY */
  } else { /* Now comes the slow, but general solution */
   #if USE_IN_XPM_MAPPING /* use Mapping */
    if (colors<=256) {
      Mapping::H h(1);
      char c;
      /* vvv `c' might become negative, but it is harmless */
      for (p=tab,pend=tab+cpp*colors,c=0; p!=pend; p+=cpp,c++)
        h.set(p, cpp, &c); /* every color-string should be unique; but no error message if it isn't */
      while (ht--!=0) {
        tok.getComma();
        for (p=outbuf+ret->getRlen(); outbuf!=p; ) {
          tok.readInStr(pend,cpp);
          if (NULLP==(pend=h.get(pend, cpp))) Error::sev(Error::EERROR) << "XPM: unpaletted color" << (Error*)0;
          *outbuf++=*pend;
        }
      }
    } else { /* most general case with Mapping */
      if (transp!=colors) Error::sev(Error::WARNING) << "XPM: too many colors, transparency blacked" << (Error*)0;
      /* Dat: reading a JPEG downsampled to a 256-color XPM takes 3000 ms
       * with Mapping, and 31000 ms without mapping. Nice speed increase.
       */
      Mapping::H h(3);
      char tmpcol[3];
      for (p=tab,pend=tab+cpp*colors,rp=rgb; p!=pend; p+=cpp,rp++) {
        tmpcol[0]=rp[0]>>16;
        tmpcol[1]=rp[0]>>8;
        tmpcol[2]=rp[0];
        h.set(p, cpp, tmpcol); /* every color-string should be unique; but no error message if it isn't */
      }
      while (ht--!=0) {
        tok.getComma();
        for (p=outbuf+ret->getRlen(); outbuf!=p; outbuf+=3) {
          tok.readInStr(pend,cpp);
          if (NULLP==(pend=h.get(pend, cpp))) Error::sev(Error::EERROR) << "XPM: unpaletted color" << (Error*)0;
          memcpy(outbuf, pend, 3);
        }
      }
    }
   #else /* don't USE_IN_XPM_MAPPING */
    Image::Sampled::dimen_t lastcol=0, x;
    p=tab; /* cache pointer for the last color (lastcol) */
    if (colors<=256) {
      assert(cpp>1);
      for (lastcol=0;lastcol<colors;lastcol++) iimg->setPal(lastcol, rgb[lastcol]);
      lastcol=0;
      while (ht--!=0) {
        // putchar('.');
        tok.getComma();
        for (x=0;x<wd;x++) {
          tok.read(pend,cpp);
          if (0!=memcmp(p,pend,cpp)) {
            p=tab; lastcol=0; while (p!=pend && 0!=memcmp(p,pend,cpp)) { p+=cpp; lastcol++; }
            if (p==pend) Error::sev(Error::EERROR) << "XPM: unpaletted color" << (Error*)0;
            if (rgb[lastcol]==0x1000000) p=tab+cpp*(lastcol=transp); /* fix single transp */
          }
          *outbuf++=lastcol;
        }
      }
    } else { /* colors>256 */
      while (ht--!=0) {
        tok.getComma();
        for (x=0;x<wd;x++) {
          tok.read(pend,cpp);
          if (0!=memcmp(p,pend,cpp)) {
            p=tab; lastcol=0; while (p!=pend && 0!=memcmp(p,pend,cpp)) { p+=cpp; lastcol++; }
            if (p==pend) Error::sev(Error::EERROR) << "XPM: unpaletted color" << (Error*)0;
          }
          *outbuf++=rgb[lastcol]>>16;
          *outbuf++=rgb[lastcol]>>8;
          *outbuf++=rgb[lastcol];
        }
      }
    }
   #endif
  } 
  delete [] tab;
  delete [] rgb;
  /* Dat: we don't check for EOF. Imp: emit a warning? */
  // Error::sev(Error::DEBUG) << "rp[-1]=" << rp[-1] << (Error*)0;
  // while (-1!=(i=tok.getcc())) { putchar(i); }
  /* fclose((FILE*)file_); */
  return ret;
}

static Image::Loader::reader_t in_xpm_checker(char buf[Image::Loader::MAGIC_LEN], char [Image::Loader::MAGIC_LEN], SimBuffer::Flat const&, Image::Loader::UFD*) {
  return (0==memcmp(buf, "/* XPM */", 9)) ? in_xpm_reader : 0;
}

#else
#  define in_xpm_checker NULLP
#endif /* USE_IN_XPM */

Image::Loader in_xpm_loader = { "XPM", in_xpm_checker, 0 };
