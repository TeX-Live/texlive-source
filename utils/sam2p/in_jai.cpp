/*
 * in_jai.cpp -- read a JPEG file as-is 
 * by pts@math.bme.hu at Sun Mar 17 20:15:25 CET 2002
 */
/* Imp: test this code with various JPEG files! */

#ifdef __GNUC__
#pragma implementation
#endif

#include "image.hpp"
#include "error.hpp"

#if USE_IN_JAI

#include "in_jai.hpp"
#include <string.h> /* memchr() */
#include <stdio.h>

/* --- */

class JAI: public Image::Sampled {
 public:
  /** (Horiz<<4+Vert) sampling factor for the first color component. Usually
   * 0x11, but TIFF defaults to 0x22.
   */
  JAI(dimen_t wd_, dimen_t ht_, unsigned char bpc_, unsigned char cs_, slen_t flen_, slen_t SOF_offs_, unsigned char hvs_);
  virtual void copyRGBRow(char *to, dimen_t whichrow) const;
  virtual void to8();
  virtual Image::Indexed* toIndexed();
  virtual bool canGray() const;
  // virtual void setBpc(unsigned char bpc_);
  virtual Image::RGB    * toRGB(unsigned char bpc_);
  virtual Image::Gray   * toGray(unsigned char bpc_);
  virtual unsigned char minRGBBpc() const;
  virtual Image::Sampled* addAlpha(Image::Gray *al);
  void fixEOI();
};

JAI::JAI(dimen_t wd_, dimen_t ht_, unsigned char bpc_, unsigned char cs_, slen_t flen_, slen_t SOF_offs_, unsigned char hvs_) {
  param_assert(cs_<=5);
  // init(0,0,wd_,ht_,bpc_,TY_BLACKBOX,cs2cpp[cs_]);
  cs=cs_;
  bpc=8; (void)bpc_; // bpc=bpc_; /* /DCTDecode supports only BitsPerComponent==8 */
  ty=TY_BLACKBOX;
  wd=wd_;
  ht=ht_;
  cpp=cs2cpp[cs_];
  // pred=1;
  transpc=0x1000000UL; /* Dat: this means: no transparent color */
  rlen=0;
  beg=new char[len=0+flen_+0+bpc];
  rowbeg=(headp=const_cast<char*>(beg))+flen_;
  xoffs=SOF_offs_;
  trail=const_cast<char*>(beg)+len-bpc;
  const_cast<char*>(beg)[len-1]=hvs_; /* dirty place */
}

void JAI::fixEOI() {
  /* by pts@fazekas.hu at Tue Jun  4 15:36:12 CEST 2002 */
  if (rowbeg[-2]!='\xFF' || rowbeg[-1]!='\xD9') {
    *rowbeg++='\xFF';
    *rowbeg++='\xD9';
  }
}

Image::Sampled* JAI::addAlpha(Image::Gray *) {
  Error::sev(Error::WARNING) << "JAI: alpha channel ignored" << (Error*)0;
  return this;
}


unsigned char JAI:: minRGBBpc() const { return bpc; }
void JAI::copyRGBRow(char*, dimen_t) const { assert(0); }
void JAI::to8() { assert(0); }
Image::Indexed* JAI::toIndexed() { return (Image::Indexed*)NULLP; }
bool JAI::canGray() const { return cs==CS_GRAYSCALE; }
Image::RGB    * JAI::toRGB(unsigned char) { assert(0); return 0; }
Image::Gray   * JAI::toGray(unsigned char) { assert(0); return 0; }

/* --- The following code is based on standard/image.c from PHP4 */

/* some defines for the different JPEG block types */
#define M_SOF0  0xC0			/* Start Of Frame0: Baseline JPEG */
#define M_SOF1  0xC1			/* N indicates which compression process */
#define M_SOF2  0xC2			/* Only SOF0-SOF2 are now in common use */
#define M_SOF3  0xC3
#define M_SOF5  0xC5			/* NB: codes C4 and CC are NOT SOF markers */
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8  
#define M_EOI   0xD9			/* End Of Image (end of datastream) */
#define M_SOS   0xDA			/* Start Of Scan (begins compressed data) */
#define M_APP0  0xe0
#define M_APP1  0xe1
#define M_APP2  0xe2
#define M_APP3  0xe3
#define M_APP4  0xe4
#define M_APP5  0xe5
#define M_APP6  0xe6
#define M_APP7  0xe7
#define M_APP8  0xe8
#define M_APP9  0xe9
#define M_APP10 0xea
#define M_APP11 0xeb
#define M_APP12 0xec
#define M_APP13 0xed
#define M_APP14 0xee
#define M_APP15 0xef


#if 0
static unsigned int jai_next_marker(FILE *fp)
   /* get next marker byte from file */
{
  int c;

#if 0 /**** pts ****/
  /* skip unimportant stuff */
  c = MACRO_GETC(fp);
  while (c != 0xff) { 
    if ((c = MACRO_GETC(fp)) == EOF)
      return M_EOI; /* we hit EOF */
  }
#else
  if (0xff!=(c=MACRO_GETC(fp))) return M_EOI;
#endif
    

  /* get marker byte, swallowing possible padding */
  do {
    if ((c = MACRO_GETC(fp)) == EOF)
      return M_EOI;    /* we hit EOF */
  } while (c == 0xff);

  return (unsigned int) c;
}
#endif

#if 0
static inline int getc_(FILE *f) { return MACRO_GETC(f); }
static inline long ftell_(FILE *f) { return ftell(f); }
template <class T>
static unsigned short jai_read2(T *fp);
template <class T>
static void jai_handle_jpeg(struct jai_gfxinfo *result, T *fp) {
#endif
/* Dat: removed templates at Tue Mar 11 19:59:16 CET 2003, because decoding
 * JPEG headers isn't time-critical
 */

static inline unsigned short jai_read2(GenBuffer::Readable *fp) {
#if 0
  unsigned char a[ 2 ];
  /* just return 0 if we hit the end-of-file */
  if (fread(a,sizeof(a),1,fp) != 1) return 0;
  return (((unsigned short) a[ 0 ]) << 8) + ((unsigned short) a[ 1 ]);
#else
  int a=fp->vi_getcc(), b=fp->vi_getcc();
  /* just return 0 if we hit the end-of-file */
  return a>=0 && b>=0 ? (a<<8)+b : 0;
#endif
}

/** main loop to parse JPEG structure */
void jai_parse_jpeg(struct jai_gfxinfo *result, DecoderTeller *fp, bool must_be_baseline) {
  int c;
  unsigned int length;
  unsigned char had_adobe;
  
  result->bad=9; /* signal invalid return value */
  result->id_rgb=0;
  result->had_jfif=0;
  result->colortransform=127; /* no Adobe marker yet */
  // fseek(fp, 0L, SEEK_SET);    /* position file pointer on SOF */

  /* Verify JPEG header */
  /* Dat: maybe 0xFF != '\xFF' */
  if ((c=fp->vi_getcc())!=0xFF) return;
  while ((c=fp->vi_getcc())==0xFF) ;
  if (c!=M_SOI) return;

  result->bad=1;
  // fprintf(stderr, "welcome ofs=%ld\n", fp->vi_tell()); 
  while (1) {
   if ((c=fp->vi_getcc())!=0xFF) { result->bad=8; return; }
   while ((c=fp->vi_getcc())==0xFF) ;
   if (c==-1) { result->bad=2; return; }
   switch (c) {
    case M_SOF0: do_SOF:
      if (result->bad!=1) { result->bad=4; return; } /* only one M_SOF allowed */
      result->SOF_type=c-M_SOF0;
      result->SOF_offs=fp->vi_tell();
      // fprintf(stderr, "SOF_offs=%d\n", result->SOF_offs);
      /* handle SOFn block */
      length=jai_read2(fp);
      result->bpc = fp->vi_getcc();
      result->height = jai_read2(fp);
      result->width = jai_read2(fp);
      result->cpp = fp->vi_getcc();
      if ((length-=8)!=3U*result->cpp) return;
      if (result->bpc!=8) { result->bad=6; return; }
      if (result->cpp!=1 && result->cpp!=3 && result->cpp!=4) { result->bad=5; return; }
      assert(length>=3);
      if (result->cpp==3) {
        result->id_rgb =fp->vi_getcc()=='R';  result->hvs=fp->vi_getcc(); fp->vi_getcc();
        result->id_rgb&=fp->vi_getcc()=='G';  fp->vi_getcc(); fp->vi_getcc();
        result->id_rgb&=fp->vi_getcc()=='B';  fp->vi_getcc(); fp->vi_getcc();
      } else { length-=2; fp->vi_getcc(); result->hvs=fp->vi_getcc(); while (length--!=0) fp->vi_getcc(); }
      result->bad=2;
      break;
    case M_SOF1:
    case M_SOF2:
    case M_SOF3:
    case M_SOF5:
    case M_SOF6:
    case M_SOF7:
    case M_SOF9:
    case M_SOF10:
    case M_SOF11:
    case M_SOF13:
    case M_SOF14:
    case M_SOF15:
      if (!must_be_baseline) goto do_SOF;
      // fprintf(stderr, "SOF%u\n", marker-M_SOF0); assert(0);
      result->bad=3;
      return;
    case M_SOS: /* we are about to hit image data. We're done. Success. */
      if (result->bad==2 /* && !feof(fp)*/) { /* Dat: !feof() already guaranteed */
        if (result->cpp==1) {
          result->colorspace=Image::Sampled::CS_GRAYSCALE;
        } else if (result->cpp==3) {
          result->colorspace=Image::Sampled::CS_YCbCr;
          if (result->had_jfif!=0) ;
          else if (result->colortransform==0) result->colorspace=Image::Sampled::CS_RGB;
          else if (result->colortransform==1) ;
          else if (result->colortransform!=127) Error::sev(Error::EERROR) << "JAI: unknown ColorTransform: " << (unsigned)result->colortransform << (Error*)0;
          else if (result->id_rgb!=0) result->colorspace=Image::Sampled::CS_RGB;
          /* Imp: check for id_ycbcr */
          else Error::sev(Error::WARNING) << "JAI: assuming YCbCr color space" << (Error*)0;
        } else if (result->cpp==4) {
          result->colorspace=Image::Sampled::CS_CMYK;
          if (result->colortransform==0) ;
          else if (result->colortransform==2) result->colorspace=Image::Sampled::CS_YCCK;
          else if (result->colortransform!=127) Error::sev(Error::EERROR) << "JAI: unknown ColorTransform: " << (unsigned)result->colortransform << (Error*)0;
        } else assert(0);
        result->bad=0;
      }
      /* fall through */
    case M_EOI: /* premature EOF */
      return;
    case M_APP0: /* JFIF application-specific marker */
      length=jai_read2(fp);
      if (length==2+4+1+2+1+2+2+1+1) {
        result->had_jfif=fp->vi_getcc()=='J' && fp->vi_getcc()=='F' && fp->vi_getcc()=='I' &&
                         fp->vi_getcc()=='F' && fp->vi_getcc()==0;
        length-=7;
      } else length-=2;
      while (length--!=0) fp->vi_getcc();
      break;
    case M_APP14: /* Adobe application-specific marker */
      length=jai_read2(fp);
      if ((length-=2)==5+2+2+2+1) {
        had_adobe=fp->vi_getcc()=='A' && fp->vi_getcc()=='d' && fp->vi_getcc()=='o' &&
                  fp->vi_getcc()=='b' && fp->vi_getcc()=='e' && ((unsigned char)fp->vi_getcc())>=1;
        fp->vi_getcc(); fp->vi_getcc(); fp->vi_getcc(); fp->vi_getcc(); fp->vi_getcc();
        if (had_adobe) result->colortransform=fp->vi_getcc();
                  else fp->vi_getcc();
      } else while (length--!=0) fp->vi_getcc();
      break;
    case M_APP1:
    case M_APP2:
    case M_APP3:
    case M_APP4:
    case M_APP5:
    case M_APP6:
    case M_APP7:
    case M_APP8:
    case M_APP9:
    case M_APP10:
    case M_APP11:
    case M_APP12:
    case M_APP13:
    case M_APP15:
      /* fall through */
    default: { /* anything else isn't interesting */
      /* skip over a variable-length block; assumes proper length marker */
      unsigned short length;
      length = jai_read2(fp);
      length -= 2;        /* length includes itself */
      #if 0 /**** pts: fseek would disturb later ftell()s and feof()s */
        fseek(fp, (long) length, SEEK_CUR);  /* skip the header */
      #else 
        while (length--!=0) fp->vi_getcc(); /* make feof(fp) correct */
      #endif
    }
   }
  }
}

char *jai_errors[]={
  (char*)NULLP,
  /*1*/ "missing SOF0 marker",
  /*2*/ "premature EOF",
  /*3*/ "not a Baseline JPEG (SOF must be SOF0)",
  /*4*/ "more SOF0 markers",
  /*5*/ "bad # components",
  /*6*/ "bad bpc",
  /*7*/ "?",
  /*8*/ "0xFF expected",
  /*9*/ "invalid JPEG header",
  // /*10*/ "not ending with EOI", /* not output by jai_handle_jpeg! */
};

static Image::Sampled *in_jai_reader(Image::Loader::UFD* ufd, SimBuffer::Flat const&) {
  // assert(0);
  struct jai_gfxinfo gi;
  Filter::UngetFILED* ufdd=(Filter::UngetFILED*)ufd;
  FILE *file_=ufdd->getFILE(/*seekable:*/true);
  jai_parse_jpeg(&gi, ufdd);
  // jai_parse_jpeg(&gi, (FILE*)file_);
  // long ftel=ftell((FILE*)file_);
  if (gi.bad!=0) Error::sev(Error::EERROR) << "JAI: " << jai_errors[gi.bad] << (Error*)0;
  // printf("ftell=%lu\n", ftell((FILE*)file_));
  fseek((FILE*)file_, 0L, 2); /* EOF */
  long flen=ftell((FILE*)file_); /* skip extra bytes after EOI. Imp: no need to do this */
  // fprintf(stderr, "flen=%lu\n", flen);
  assert(flen>2);
  rewind((FILE*)file_);
  JAI *ret=new JAI(gi.width,gi.height,gi.bpc,gi.colorspace,flen,gi.SOF_offs,gi.hvs);
  if (fread(ret->getHeadp(), flen, 1, (FILE*)file_)!=1 || ferror((FILE*)file_)) {
    ret->fixEOI();
    /* fclose((FILE*)file_); */
    Error::sev(Error::EERROR) << "JAI: IO error" << (Error*)0;
  }
  /* fclose((FILE*)file_); */
  return ret;
}

static Image::Loader::reader_t in_jai_checker(char buf[Image::Loader::MAGIC_LEN], char [Image::Loader::MAGIC_LEN], SimBuffer::Flat const& loadHints, Image::Loader::UFD* ufd) {
  if (0!=memcmp(buf, "\xff\xd8\xff", 3)
   || !(loadHints.findFirst((char const*)",jpeg-asis,",6)!=loadHints.getLength() || loadHints.findFirst((char const*)",asis,",6)!=loadHints.getLength())
     ) return 0;
  Filter::UngetFILED* ufdd=(Filter::UngetFILED*)ufd;
  ufdd->seek(0);
  Image::Loader::reader_t ret=jai_is_baseline_jpeg(ufdd) ? in_jai_reader : 0;
  ufdd->seek(0);
  return ret;
}

#if 0 /* Filter::FlatR* used by JPEGSOF0Encode::vi_write() in appliers.cpp */
static inline int getc_(Filter::FlatR *f) { return f->getcc(); }
static inline long ftell_(Filter::FlatR *f) { return f->tell(); }
void jai_parse_jpeg(struct jai_gfxinfo *result, Filter::FlatR *f) {
  jai_handle_jpeg(result, f);
}
void jai_parse_jpeg(struct jai_gfxinfo *result, FILE *f) {
  jai_handle_jpeg(result, f);
}
#endif

#if 0 /* unused */
int jai_is_baseline_jpeg(char const* filename) {
  /* by pts@fazekas.hu at Tue Mar 11 20:27:56 CET 2003 */
  FILE *f=fopen(filename, "rb");
  if (!f) return -1;
  Filter::FILED filed(f, /*closep:*/true);
  return jai_is_baseline_jpeg(&filed);
}
#endif
int jai_is_baseline_jpeg(/*DecoderTeller*/Filter::UngetFILED *fp) {
  char buf[3];
  struct jai_gfxinfo gi;
  bool qfalse=(3!=fp->vi_read(buf, 3) || 0!=memcmp(buf, "\xff\xd8\xff", 3));
  fp->unread(buf, 3);
  if (qfalse) return false;
  fp->getFILE(/*seekable*/true); /* make it seekable, so the caller can seek back */
  jai_parse_jpeg(&gi, fp, /*must_be_baseline:*/false);
  fp->seek(0);
  return gi.bad!=0 ? -1 : gi.SOF_type==0;
}
#else
#define in_jai_checker (Image::Loader::checker_t)NULLP
#endif /* USE_IN_JAI */

Image::Loader in_jai_loader = { "JAI", in_jai_checker, 0 };
