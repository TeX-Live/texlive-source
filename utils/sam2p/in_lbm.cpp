/*
 * in_lbm.cpp -- read a Deluxe Paint IFF/ILBM file
 * by pts@fazekas.hu at Fri Mar  1 17:12:16 CET 2002
 * -- Fri Mar  1 22:28:03 CET 2002
 *
 * I've ripped the reader code from xscavenger's source, and enhanced it
 * a little bit (at Fri Mar  1 17:12:43 CET 2002), but I still think that
 * this can read only the minority of the LBM files around.
 */
/* Imp: get docs about the LBM format, and rewrite this from scratch */

#ifdef __GNUC__
#pragma implementation
#endif

#include "image.hpp"
#include "error.hpp"

#if USE_IN_LBM

#include <string.h> /* memchr() */
#include "gensio.hpp"

#define FORM 0x464f524dL
#define ILBM 0x494c424dL
#define PBM  0x50424d20L
#define CMAP 0x434d4150L
#define BODY 0x424f4459L
#define BMHD 0x424d4844L
/* typedef unsigned char uchar; */
#define MAXBYTES 128 /* ?? */

/* --- */

#if SIZEOF_INT>=4
typedef unsigned int  u32_t;
typedef   signed int  s32_t;
#else
typedef unsigned long u32_t;
typedef   signed long s32_t;
#endif

class LBMRead {
 public:
  inline LBMRead(FILE *f_): f(f_) {}
  /* inline ~LBMRead() { fclose(f); } */
  Image::Sampled *doit();
 protected:
  inline int getcn();
  void errUnEOF();
  void err(char *s);
  u32_t getUnpack(unsigned len, char *where);
  u32_t getu32();
  unsigned getu16();
  FILE *f;
};

//static inline int fixbyte(char *take,int bit,int depth) {
//  int res=0,mask=1;
//  while (depth--) {
//    if (*take & bit) res|=mask;
//    mask<<=1;
//    take+=MAXBYTES;
//  }
//  return res;
//}
#if 0
static void debit(char *picput, char *lines, unsigned wd, unsigned depth) {
  unsigned depth2, bit=128, res, i, mask;
  unsigned byteswide=((wd+15)>>4)<<1;
  unsigned char *take;
  for (i=0;i<wd;i++) {
    mask=1; depth2=depth; take=(unsigned char*)lines; res=0; while (depth2--!=0) {
      if (0!=(*take&bit)) res|=mask;
      mask<<=1;
      take+=byteswide;
    }
    *picput++=res;
    if (bit==1) { bit=128; ++lines; } else bit>>=1;
  }
}
#endif

void LBMRead::errUnEOF() {
  Error::sev(Error::EERROR) << "LBM: unexpected EOF" << (Error*)0;
}
void LBMRead::err(char *s) {
  Error::sev(Error::EERROR) << s << (Error*)0;
}

inline int LBMRead::getcn() {
  int i=MACRO_GETC(f);
  if (i==-1) errUnEOF();
  return i;
}

u32_t LBMRead::getUnpack(unsigned len, char *where) {
  /* RLE decompression */
  int ch,ch2;
  u32_t sofar=0;
  while (len>0) {
    ch=getcn();
    if (ch<128) {
      ch++;
      assert(ch>=1);
      if (len<(unsigned)ch) err("LBM: packet#1 too long");
      len-=ch;
      sofar+=1+ch;
      while (ch--) *where++=getcn();
    } else {
      ch=1-(ch-256); /* 2..129 */
      assert(ch>=1);
      // fprintf(stderr, "len=%u ch=%u\n", len, ch);
      if (len<(unsigned)ch) err("LBM: packet#2 too long");
      len-=ch;
      // fprintf(stderr, "xlen=%u ch=%u\n", len, ch);
      ch2=getcn();
      sofar+=2;
      while (ch--) *where++=ch2;
    }
  }
  return sofar;
}

u32_t LBMRead::getu32() {
  u32_t val1=0;
  val1=getcn()<<24L;
  val1|=getcn()<<16L;
  val1|=getcn()<<8;
  val1|=getcn();
  return val1;
}
unsigned LBMRead::getu16() {
  u32_t val1=0;
  val1|=getcn()<<8;
  val1|=getcn();
  return val1;
}

Image::Sampled *LBMRead::doit() {
  char temparea[16];
  u32_t type, size, i;
  unsigned char comp=0;
  unsigned j;
  int ii;
  Image::Sampled::dimen_t lbm_wd, lbm_ht, byteswide, depth=0; /* pts */
  unsigned transp=256;
  Image::Sampled *img=(Image::Sampled*)NULLP;
  bool had_cmap=false, had_body=false, do_skip, had_transp=false;

  /* sprintf(temparea,"%s/%s/%s",localname,localdirname,name); */
  for (i=0;i<12;i++) temparea[i]=getcn();
  if (0!=memcmp(temparea,"FORM",4) ||
      0!=memcmp(temparea+8,"ILBM",4)) err("LBM: magic says: this is not an LBM");
  lbm_wd=lbm_ht=byteswide=0;
  while ((ii=MACRO_GETC(f))!=-1 && ii!='\0') {
    ungetc(ii,f);
    if ((type=getu32())==0xffffffffL) break;
    size=getu32();
    do_skip=(size&1); /* pad to even size */
    // fprintf(stderr,"size=%u\n", size);
    if (type==BMHD) {
      if (img!=NULLP) err("LBM: duplicate BMHD");
      // if (size>sizeof(temparea)) {
      //  for (i=0;i<sizeof(temparea);i++) temparea[i]=getcn();
      //  for (;i<size;i++) getcn();
      // } else for (i=0;i<size;i++) temparea[i]=getcn();
      if (size<20) errUnEOF();
      lbm_wd=getu16();
      lbm_ht=getu16();
      if (getu32()!=0) err("LBM: expected offsets==0");
      depth=getcn();
      if (depth!=24 && (depth<1 || depth>8)) err("LBM: invalid color depth");
      if (getcn()!=0) err("LBM: expected masking==0");
      if ((comp=getcn())>=2) err("LBM: expected comp==0 || comp==1");
      getcn(); /* ignore padding */
      transp=getu16(); /* index of transparent color */
      // fprintf(stderr, "htt=%u transp=%u\n", lbm_ht, transp);
      getu16(); /* Ignore aspect ratio. */
      if (lbm_wd!=getu16() || lbm_ht!=getu16()) err("LBM: inconsistent dimens");
      /* ^^^ Dat: pagex, pagey */
      byteswide=((lbm_wd+15)>>4)<<1;
      /*printf("(%d,%d) %d bpp\n",lbm_wd,lbm_ht,depth);*/
      if (depth==24) {
        had_cmap=true;
        img=new Image::RGB(lbm_wd, lbm_ht, 8);
      } else {
        img=new Image::Indexed(lbm_wd, lbm_ht, 1<<depth, 8);
        memset(img->getHeadp(), '\0', 3*(1<<depth)); /* clear palette */
      }
    } else if (type==BODY) {
      if (!had_cmap) err("LBM: should be CMAP, BODY");
      // fprintf(stderr, "wd=%u, %u exp=%u\n", lbm_wd, size, byteswide*depth*lbm_ht);
      if (comp==0 && size!=byteswide*depth*lbm_ht) err("LBM: wrong BODY length");
      char *buf=new char[byteswide*depth], *picput=img->getRowbeg(); // const_cast<char*>(img->getCString());
      for (i=0;i<lbm_ht;i++) {
        // fprintf(stderr, "i=%u ht=%u\n", i, lbm_ht);
        if (comp) { for (j=0;j<depth;j++) size-=getUnpack(byteswide, buf+byteswide*j); }
             else { for (j=0;j<depth*byteswide;j++) buf[j]=getcn(); }
        /* ^^^ Imp: test !comp: find uncompressed LBM files */
        if ((s32_t)size<0) goto dtl;

        /* debit() */
        /* LBM compression is quite stupid: it run-length-compresses
         * individiual bits of the color depth as 8-bit bytes. Plus it requires
         * each bit vector to be aligned to 16-bit boundary.
         */
        Image::Sampled::dimen_t depth2, bit=128, res, i, mask;
        // unsigned byteswide=((lbm_wd+15)>>4)<<1;
        unsigned char *take, *lines=(unsigned char*)buf;
        if (depth!=24) {
          for (i=0;i<lbm_wd;i++) {
            mask=1; depth2=depth; take=lines; res=0; while (depth2--!=0) {
              if (0!=(*take&bit)) res|=mask;
              mask<<=1;
              take+=byteswide;
            }
            if (res==transp) had_transp=true; 
            *picput++=res;
            if (bit==1) { bit=128; ++lines; } else bit>>=1;
          }
        } else {
          u32_t res;
          for (i=0;i<lbm_wd;i++) {
            mask=1; depth2=24; take=lines; res=0; while (depth2--!=0) {
              if (0!=(*take&bit)) res|=mask;
              mask<<=1;
              take+=byteswide;
            }
            /* No transparency here. */
            *picput++=res; /* Shift order verified. */
            *picput++=res>>8;
            *picput++=res>>16;
            if (bit==1) { bit=128; ++lines; } else bit>>=1;
          }
        }
      }
      // fprintf(stderr,"size=%u\n", size);
      if (size>1) { dtl: err("LBM: data too long"); }
      if (size==1) getcn();
      if (had_transp) { assert(depth!=24); static_cast<Image::Indexed*>(img)->setTransp(transp); }
      // img->getRowbeg()[0]='\001'; img->getRowbeg()[1]='\002';
      delete [] buf;
      had_body=true;
    } else if (type==CMAP) {
      if (img==NULLP) err("LBM: should be: BMHD, CMAP");
      if (had_cmap) err("LBM: duplicate CMAP");
      if (size>3*(1U<<depth)) err("LBM: CMAP too large");
      char *colormap=img->getHeadp();
      // fprintf(stderr,"Si=%d size=%u\n", img->getRowbeg()-colormap, size);
      for (i=0;i<size;i++) colormap[i]=getcn();
      had_cmap=true;
    } else {
      Error::sev(Error::WARNING) << "LBM: unknown tag ignored: " << type << (Error*)0;
      while (size--) MACRO_GETC(f);
    }
    if (do_skip) getcn();
    // fprintf(stderr,"do_skip=%d t=%lu\n", do_skip, ftell(f));
  }
  if (!had_body) err("LBM: missing BODY");
  return img;
}

static Image::Sampled *in_lbm_reader(Image::Loader::UFD *ufd, SimBuffer::Flat const&) {
  return LBMRead(((Filter::UngetFILED*)ufd)->getFILE(/*seekable:*/false)).doit();
  /* ^^^ Destructor: fclose((FILE*)file_); */
}

static Image::Loader::reader_t in_lbm_checker(char buf[Image::Loader::MAGIC_LEN], char [Image::Loader::MAGIC_LEN], SimBuffer::Flat const&, Image::Loader::UFD*) {
  return (0==memcmp(buf,"FORM",4) && 0==memcmp(buf+8,"ILBM",4)) ? in_lbm_reader : 0;
}

#else
#define in_lbm_checker NULLP
#endif /* USE_IN_XPM */

Image::Loader in_lbm_loader = { "LBM", in_lbm_checker, 0 };
