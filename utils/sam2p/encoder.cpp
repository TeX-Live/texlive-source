/* encoder.cpp -- various PostScript and PDF encoding filter implementations
 * by pts@fazekas.hu at Sun Mar  3 14:16:00 CET 2002
 * --- Mon Mar  4 00:00:20 CET 2002
 */

/* Imp: make built-in FlateEncode DYNALLOC */
/* Imp: add lzw_codec */
/* Imp: add gzip */
/* Imp: add zlib */

#ifdef __GNUC__
#pragma implementation
#endif


#include "encoder.hpp"
#include "error.hpp"
#include "gensio.hpp"
#include <stdlib.h> /* getenv() */
#include <string.h>

/* --- */

/** Does output EOD (end-of-data) marker `>' */
class ASCIIHexEncode: public PSEncoder {
 public:
  /** @param maxcpl_: maximum # hex digits per line, should be even */
  ASCIIHexEncode(GenBuffer::Writable &out_, unsigned maxcpl_);
  virtual void vi_write(char const*buf, slen_t len);
 protected:
  unsigned maxcpl;
  /** Number of hex digits already printed. */
  unsigned curcpl;
  GenBuffer::Writable &out;
};

/** Does output EOD (end-of-data) marker `~>' */
class ASCII85Encode: public PSEncoder {
 public:
  /** @param maxcpl_: maximum # hex digits per line, should be even */
  ASCII85Encode(GenBuffer::Writable &out_, unsigned maxcpl_);
  virtual void vi_write(char const*buf, slen_t len);

 protected:
  void wout(unsigned PTS_INT32_T buf_);
  unsigned maxcpl;
  /** Number of digits available in this line */
  GenBuffer::Writable &out;
  unsigned ascii85breaklen;
  unsigned ascii85left;
  unsigned PTS_INT32_T ascii85buf;
  char *obuf, *obufend, *op;
};

/** Doesn't output EOD (end-of-data) marker `>' */
class RunLengthEncode: public PSEncoder {
 public:
  /** @param maxcpl_: maximum # hex digits per line, should be even */
  RunLengthEncode(GenBuffer::Writable &out_, slen_t RecordSize_);
  virtual void vi_write(char const*buf, slen_t len);

 protected:
  slen_t recordsize;
  GenBuffer::Writable &out;
  // char *obuf, *obufend, *op;
  slen_t record_left;
  unsigned saved_c, saved_rep;
  /** size == header+buffer+EOD */
  char saved[130];
};

static int gen_write(char *block, unsigned len, void *zfile);

#if USE_BUILTIN_ZIP
#include "pts_defl.h" /* Imp: because pts_defl_interface :-( */
#if OBJDEP
#  warning REQUIRES: pts_defl
#endif
class FlateEncode: public PSEncoder {
#if PTS_DEFL_RIPPED_ZLIB /* Dat: defined in pts_defl.h */
 public:
  /** @param level: 1..9: 9==highest compression */
  FlateEncode(GenBuffer::Writable &out_, unsigned level_);
  virtual ~FlateEncode() { zlib_deflateEnd(&zs); } /* Dat: ==Z_OK check (for unflushed buffers) omitted */
  virtual void vi_write(char const*buf, slen_t len);
 protected:
  /** Formerly only one instance of FlateEncode was allowed.
   * It exists <=> loced==true
   */
  // static bool locked;
  /** Writable that this filter writes to */
  GenBuffer::Writable &out;
  char workspace[ZLIB_DEFLATE_WORKSPACESIZE_MIN]; /* big, about 300k */
  char obuf[4096];
  /*struct*/ z_stream zs;
#else /* old, ripped from Info-ZIP 2.2 */
 public:
  /** @param level: 1..9: 9==highest compression */
  FlateEncode(GenBuffer::Writable &out_, unsigned level_);
  virtual ~FlateEncode() { if (fs!=NULLP) fs->delete2(fs); }
  virtual void vi_write(char const*buf, slen_t len);
  static void *gen_malloc(unsigned n);
  static void gen_free(void *p);
 protected:
  /** Formerly only one instance of FlateEncode was allowed.
   * It exists <=> loced==true
   */
  // static bool locked;
  /** Writable that this filter writes to */
  GenBuffer::Writable &out;
  #if SIZEOF_INT>2
    typedef unsigned s_t;
  #else
    typedef unsigned short s_t;
  #endif
  /** Adler32 checksum */
  s_t s1, s2;
  bool had_header;
  struct pts_defl_interface* fs;
#endif /* else PTS_DEFL_RIPPED_ZLIB */
};
#endif

/** Just store the data in Flate (ZLIB) format, no real compression. Adds
 * about 3 bytes of overhead per 65535 bytes (compression ratio:
 * 100.004578%)
 */
class FlateStoreEncode: public PSEncoder {
 public:
  /** @param level: 1..9: 9==highest compression */
  FlateStoreEncode(GenBuffer::Writable &out_);
  inline virtual ~FlateStoreEncode() {}
  virtual void vi_write(char const*buf, slen_t len);
 protected:
  GenBuffer::Writable &out;
  #if SIZEOF_INT>2
    typedef unsigned s_t;
  #else
    typedef unsigned short s_t;
  #endif
  /** Adler32 checksum */
  s_t s1, s2;
  bool had_header;
  /** Number of bytes already in buf */
  unsigned abuflen;
  /** Max: 65535 */
  #if HAVE_STATIC_CONST
    static const unsigned ABUFSIZE=65535;
  #else
    #define ABUFSIZE 65535
  #endif
  /** Should really not allocate a FlateStoreEncode on the stack. */
  char abuf[ABUFSIZE+5];
};

#if USE_BUILTIN_FAXE

#include "pts_fax.h"
#if OBJDEP
#  warning REQUIRES: pts_faxe
#endif
class CCITTFaxEncode: public PSEncoder {
 public:
  CCITTFaxEncode(GenBuffer::Writable &out_, slendiff_t K, slen_t Columns, bool EndOfLine, bool BlackIs1);
  virtual void vi_write(char const*buf, slen_t len);
 protected:
  GenBuffer::Writable &out;
  stream_CFE_state sCFEs;
  unsigned char rbuf[4096];
  unsigned char wbuf[4096];
  stream_cursor_read r;
  stream_cursor_write w;
  unsigned char *hard, *rlimit;

  static void*gen_xalloc(unsigned n);
  static void gen_free(void *ptr);
  static void gen_memset(void *s, int c, unsigned n);
  static void gen_memcpy(void *dest, const void *src, unsigned n);
};
#endif

#if USE_BUILTIN_LZW
#include "pts_lzw.h" /* Imp: because pts_lzw_state :-( */
#if OBJDEP
#  warning REQUIRES: pts_lzw
#endif
class LZWEncode: public PSEncoder {
 public:
  LZWEncode(GenBuffer::Writable &out_);
  virtual void vi_write(char const*buf, slen_t len);
 protected:
  GenBuffer::Writable &out;
  struct pts_lzw_state fs;
};
#endif

/**
 * Reporting of GS errors is somewhat dump: the actual message isn't reported,
 * only the fact that something went wrong.
 * Imp: force a no-error trailer output by gs, and check that
 */
class GSEncode: public PSEncoder {
 public:
  /** @param filter_psname is a full-featured PostScript *Encode filter
   * specification string, for example: "<</Effort 5>>/FlateEncode" or
   * "0/RunLengthEncode" or "72 pop/ASCIIHexEncode"; will be surrounded as
   * `currentfile ... filter'
   */
  GSEncode(GenBuffer::Writable &out_, char const*filter_psname);
  inline virtual ~GSEncode() {}
  /* vvv Imp: correct these */
  virtual void vi_write(char const*buf, slen_t len);
 protected:
  class P: public Filter::PipeE {
    protected: virtual void vi_check();
    public: P(GenBuffer::Writable &out_, char const*filter_psname);
  };
  /** We need this delegator because `delete ...' won't work with multiple
   * inheritance. (?? )
   */
  P p;
};

class CjpegEncode: public PSEncoder {
 public:
  /** @param filter_psname is a full-featured PostScript *Encode filter
   * specification string, for example: "<</Effort 5>>/FlateEncode" or
   * "0/RunLengthEncode" or "72 pop/ASCIIHexEncode"; will be surrounded as
   * `currentfile ... filter'
   */
  CjpegEncode(GenBuffer::Writable &out_, char const*filter_psname, slen_t Columns, slen_t Rows, bool rgbp_, unsigned char quality);
  inline virtual ~CjpegEncode() {}
  virtual void vi_write(char const*buf, slen_t len);
 protected:
  class P: public Filter::PipeE { public:
    // protected: virtual void vi_check();
    P(GenBuffer::Writable &out_, slen_t Columns, slen_t Rows, bool rgbp_, unsigned char quality);
   protected: /* Not needed: protected -> public: pacify VC6.0 */
    virtual void vi_copy(FILE *f);
    bool rgbp;
  };
  /** We need this delegator because `delete ...' won't work with multiple
   * inheritance. (?? )
   */
  P p;
};

#if !USE_BUILTIN_LZW
class Lzw_codecEncode: public PSEncoder {
 public:
  /** @param filter_psname is a full-featured PostScript *Encode filter
   * specification string, for example: "<</Effort 5>>/FlateEncode" or
   * "0/RunLengthEncode" or "72 pop/ASCIIHexEncode"; will be surrounded as
   * `currentfile ... filter'
   */
  Lzw_codecEncode(GenBuffer::Writable &out_, char const*filter_psname);
  inline virtual ~Lzw_codecEncode() {}
  virtual void vi_write(char const*buf, slen_t len);
 protected:
  class P: public Filter::PipeE { public:
    // protected: virtual void vi_check();
    P(GenBuffer::Writable &out_);
   protected:
    // virtual void vi_copy(FILE *f);
  };
  /** We need this delegator because `delete ...' won't work with multiple
   * inheritance. (?? )
   */
  P p;
};
#endif

class TIFFPredictor2: public Encoder {
 public:
  /** @param maxcpl_: maximum # hex digits per line, should be even */
  TIFFPredictor2(GenBuffer::Writable &out_, unsigned char bpc_, slen_t columns_, unsigned char cpp_);
  virtual void vi_write(char const*buf, slen_t len);
 protected:
  unsigned PTS_INT32_T h;
  unsigned char *obuf, *op, bpc, cpp;
  slen_t rlen;
  GenBuffer::Writable &out;
};

class PNGPredictorNone: public Encoder {
 public:
  /** @param maxcpl_: maximum # hex digits per line, should be even */
  PNGPredictorNone(GenBuffer::Writable &out_, unsigned char bpc_, slen_t columns_, unsigned char cpp_);
  virtual void vi_write(char const*buf, slen_t len);
 protected:
  slen_t rlen, opleft;
  GenBuffer::Writable &out;
};

class PNGPredictorSub: public Encoder {
 public:
  /** @param maxcpl_: maximum # hex digits per line, should be even */
  PNGPredictorSub(GenBuffer::Writable &out_, unsigned char bpc_, slen_t columns_, unsigned char cpp_);
  virtual void vi_write(char const*buf, slen_t len);
 protected:
  unsigned PTS_INT32_T h;
  unsigned char *obuf, *op;
  slen_t rlen;
  GenBuffer::Writable &out;
  unsigned char bpccpp;
};

class PNGPredictorUp: public Encoder {
 public:
  /** @param maxcpl_: maximum # hex digits per line, should be even */
  PNGPredictorUp(GenBuffer::Writable &out_, unsigned char bpc_, slen_t columns_, unsigned char cpp_);
  virtual void vi_write(char const*buf, slen_t len);
 protected:
  unsigned char *obuf, *op, *oq;
  slen_t rlen;
  GenBuffer::Writable &out;
};

class PNGPredictorAverage: public Encoder {
 public:
  /** @param maxcpl_: maximum # hex digits per line, should be even */
  PNGPredictorAverage(GenBuffer::Writable &out_, unsigned char bpc_, slen_t columns_, unsigned char cpp_);
  virtual void vi_write(char const*buf, slen_t len);
 protected:
  unsigned PTS_INT32_T h/*, g*/;
  unsigned char *obuf, *op, *oq;
  slen_t rlen;
  GenBuffer::Writable &out;
  unsigned char bpccpp;
};

class PNGPredictorPaeth: public Encoder {
 public:
  /** @param maxcpl_: maximum # hex digits per line, should be even */
  PNGPredictorPaeth(GenBuffer::Writable &out_, unsigned char bpc_, slen_t columns_, unsigned char cpp_);
  virtual void vi_write(char const*buf, slen_t len);
 protected:
  unsigned PTS_INT32_T h, g;
  unsigned char *obuf, *op, *oq;
  slen_t rlen;
  GenBuffer::Writable &out;
  unsigned char bpccpp;
};

class PNGPredictorAuto: public Encoder {
 public:
  /** @param maxcpl_: maximum # hex digits per line, should be even */
  PNGPredictorAuto(GenBuffer::Writable &out_, unsigned char bpc_, slen_t columns_, unsigned char cpp_);
  virtual void vi_write(char const*buf, slen_t len);
 protected:
  unsigned PTS_INT32_T h, g;
  unsigned char *obuf, *o_prior, *o_0, *o_1, *o_2, *o_3, *o_4, *oo[5];
  slen_t rlen;
  GenBuffer::Writable &out;
  unsigned char bpccpp;
  slen_t opleft;
};

/* Imp: code reuse with PNGPredictorAuto */
class PNGPredictorAutoUnsigned: public Encoder {
 public:
  /** @param maxcpl_: maximum # hex digits per line, should be even */
  PNGPredictorAutoUnsigned(GenBuffer::Writable &out_, unsigned char bpc_, slen_t columns_, unsigned char cpp_);
  virtual void vi_write(char const*buf, slen_t len);
 protected:
  unsigned PTS_INT32_T h, g;
  unsigned char *obuf, *o_prior, *o_0, *o_1, *o_2, *o_3, *o_4, *oo[5];
  slen_t rlen;
  GenBuffer::Writable &out;
  unsigned char bpccpp;
  slen_t opleft;
};

/* --- */

FlateStoreEncode::FlateStoreEncode(GenBuffer::Writable &out_)
 :out(out_)
 ,s1(1)
 ,s2(0)
 ,had_header(false)
 ,abuflen(0)
 {}

#if 0
/** 0x7801: ZLIB header signaling fastest compression
 * 0: 2 bits: DEFLATE block header signaling stored (uncompressed) block +
 *    6 bits of padding.
 */
char FlateStoreEncode::header[3]= { 0x78, 1, 0 };
#endif

void FlateStoreEncode::vi_write(char const*buf, slen_t len) {
  register s_t sold;
  register char const*p=buf; char const*pend=p+len;
  (void)sold;
  if (len==0) { /* EOF: flush and send trailer */
    /* Dat: Last block has to be written even if it's empty. */
    if (!had_header) { out.vi_write("\x78\x01", 2); had_header=true; }
    abuf[0]=(char)1; /* Last, stored block with padding */
    abuf[1]=abuflen; abuf[2]=abuflen>>8;
    abuf[3]=~(abuflen); abuf[4]=~(abuflen>>8);
    out.vi_write(abuf, abuflen+5);
    if (s1>=65521) s1-=65521;
    if (s2>=65521) s2-=65521;
    unsigned char trailer[4];
    trailer[0]=s2>>8; trailer[1]=s2&255; trailer[2]=s1>>8; trailer[3]=s1&255;
    out.vi_write((char const*)trailer,4);
    out.vi_write(0,0); /* Signal EOF */
    return;
  }
  /* From rfc1950.txt:
           Adler-32 is composed of two sums accumulated per byte: s1 is
           the sum of all bytes, s2 is the sum of all s1 values. Both sums
           are done modulo 65521. s1 is initialized to 1, s2 to zero.  The
           Adler-32 checksum is stored as s2*65536 + s1 in most-
           significant-byte first (network) order.
  */
  /* Update Adler-32 checksum */
  while (p!=pend) {
    #if SIZEOF_INT>2
      if ((s1+=*(unsigned char const*)p)>=65521) s1-=65521;
      if ((s2+=s1)>=65521) s2-=65521;
    #elif SIZEOF_SHORT==2
      sold=s1;
      if ((s1+=*(unsigned char const*)p))<sold) s1+=15; /* 15==65536-21 */
      sold=s2;
      s2=(s2+s1)&0xffff;
      if ((s2+=s1)<sold) s2+=15;
    #else
      /* vvv 0xffff is needed since unsigned short may be > 0..65535 */
      sold=s1;
      s1=(s1+*(unsigned char const*)p)&0xffff;
      if (s1<sold) s1+=15; /* 15==65536-21 */
      sold=s2;
      s2=(s2+s1)&0xffff;
      if (s2<sold) s2+=15;
    #endif
    p++;
  }
  // fprintf(stderr, "len=%u\n", len);
  unsigned abufleft;
  while ((abufleft=ABUFSIZE-abuflen)<len) {
    // putchar('.');
    // fprintf(stderr, "ABUFSIZE=%u abuflen=%u\n", ABUFSIZE, abuflen);
    memcpy(abuf+abuflen+5, buf, abufleft);
    abuf[0]=0; /* Stored block with padding */
    abuf[1]=(char)ABUFSIZE; abuf[2]=(char)(ABUFSIZE>>8);
    abuf[3]=(char)~(ABUFSIZE); abuf[4]=(char)~(ABUFSIZE>>8);
    if (!had_header) { out.vi_write("\x78\x01", 2); had_header=true; }
    /* fprintf(stderr,"%02x %02x", abuf[1], abuf[2]); */
    out.vi_write(abuf, ABUFSIZE+5); /* emit next stored block with header */
    abuflen=0;
    len-=abufleft;
    buf+=abufleft;
  }
  // putchar('X');
  memcpy(abuf+abuflen+5, buf, len);
  abuflen+=len;
  /* Now abuf is possibly full. That is intentional, so we will be able to
   * emit a full last block instead of an empty one.
   */
}

/* --- */

ASCIIHexEncode::ASCIIHexEncode(GenBuffer::Writable &out_, unsigned maxcpl_)
 :maxcpl(maxcpl_)
 ,curcpl(0)
 ,out(out_) {}

void ASCIIHexEncode::vi_write(char const*buf, slen_t len) {
  /* Imp: buffering (1K etc.) */
  static char const hextable []="0123456789abcdef";
  char obuf[3];
  obuf[0]='\n';
  if (len==0) { out.vi_write(">",1); out.vi_write(0,0); }
  else while (len--!=0) {
    obuf[1]=hextable[(*(unsigned char const*)buf)>>4];
    obuf[2]=hextable[(*(unsigned char const*)buf)&15];
    if (curcpl>=maxcpl) { curcpl=2;  out.vi_write(obuf,   3); }
                   else { curcpl+=2; out.vi_write(obuf+1, 2); }
    buf++;
  }
}

/* --- */

ASCII85Encode::ASCII85Encode(GenBuffer::Writable &out_, unsigned maxcpl_)
 :maxcpl(maxcpl_)
 ,out(out_)
 ,ascii85breaklen(maxcpl_)
 ,ascii85left(4) {
  obufend=(op=obuf=new char[4096])+4096;
}

void ASCII85Encode::vi_write(char const*buf, slen_t len) {
  if (len==0) {
    char encoded[6];
    assert(ascii85left<=4);
    if (ascii85left!=4) {
      unsigned PTS_INT32_T buf_=ascii85buf<<8*ascii85left;
      unsigned PTS_INT32_T q;
      unsigned w1;

      q = buf_ / ((unsigned PTS_INT32_T)85*85*85*85);  /* actually only a byte */
      assert(q<=85);
      encoded[0] = q + '!';

      buf_ -= q * ((unsigned PTS_INT32_T)85*85*85*85); q = buf_ / ((unsigned PTS_INT32_T)85*85*85);
      encoded[1] = q + '!';

      buf_ -= q * ((unsigned PTS_INT32_T)85*85*85); q = buf_ / (85*85);
      encoded[2] = q + '!';

      w1 = (unsigned) (buf_ - q*(unsigned PTS_INT32_T)(85*85));
      assert(w1/85<85);
      encoded[3] = (w1 / 85) + '!';
      encoded[4] = (w1 % 85) + '!';
      encoded[5-ascii85left] = '\0';
      char* cp;
      for (cp = encoded; *cp!='\0'; ) {
        if (op==obufend) out.vi_write(op=obuf, obufend-obuf);
        // if (*cp<='!') { fprintf(stderr, "e=%d.\n", cp-encoded); }
        assert(*cp>='!');
        assert(*cp<='~');
        *op++=*cp++;
        if (--ascii85breaklen == 0) {
          if (op==obufend) out.vi_write(op=obuf, obufend-obuf);
          *op++='\n';
          ascii85breaklen = maxcpl;
        }
      } /* NEXT */
    } /* IF */
    if (op!=obuf) out.vi_write(obuf, op-obuf); /* flush buffer cache */
    out.vi_write("~>",2); out.vi_write(0,0);
    delete [] obuf;
    obuf=(char*)NULLP;
  } else {
    assert(obuf!=NULLP);
    register unsigned PTS_INT32_T abuf=ascii85buf;
    register unsigned aleft=ascii85left;
    assert(aleft>=1 && aleft<=4);
    do {
      while (len!=0 && aleft!=0) {
        abuf=(abuf<<8)+*(unsigned char const*)buf++; len--; aleft--;
      }
      if (aleft!=0) break;
      wout(abuf);
      aleft=4;
    } while (len!=0);
    ascii85buf=abuf; ascii85left=aleft;
  }
}

void ASCII85Encode::wout(unsigned PTS_INT32_T buf_) {
  char encoded[6];
  if (buf_ != (unsigned PTS_INT32_T)0) {
    unsigned PTS_INT32_T q;
    unsigned w1;

    q = buf_ / ((unsigned PTS_INT32_T)85*85*85*85);  /* actually only a byte */
    encoded[0] = q + '!';

    buf_ -= q * ((unsigned PTS_INT32_T)85*85*85*85); q = buf_ / ((unsigned PTS_INT32_T)85*85*85);
    encoded[1] = q + '!';

    buf_ -= q * ((unsigned PTS_INT32_T)85*85*85); q = buf_ / (85*85);
    encoded[2] = q + '!';

    w1 = (unsigned) (buf_ - q*(unsigned PTS_INT32_T)(85*85));
    encoded[3] = (w1 / 85) + '!';
    encoded[4] = (w1 % 85) + '!';
    encoded[5] = '\0';
  } else {
    encoded[0] = 'z', encoded[1] = '\0';
  }
  char* cp;
  for (cp = encoded; *cp!='\0'; ) {
    if (op==obufend) out.vi_write(op=obuf, obufend-obuf);
    assert(*cp>='!');
    assert(*cp<='~');
    *op++=*cp++;
    if (--ascii85breaklen == 0) {
      if (op==obufend) out.vi_write(op=obuf, obufend-obuf);
      *op++='\n';
      ascii85breaklen = maxcpl;
    }
  }
}

/* --- */

RunLengthEncode::RunLengthEncode(GenBuffer::Writable &out_, slen_t RecordSize_)
 :recordsize(RecordSize_==0?(slen_t)-1:RecordSize_)
 ,out(out_) {
  record_left=recordsize;
  // obufend=(op=obuf=new char[4096])+4096; /* Imp: implement write buffering */
  saved_c=saved_rep=0;
}

void RunLengthEncode::vi_write(char const*buf, slen_t len) {
  unsigned j, umax;
  char b;
  if (len==0) { /* handle EOF */
    if (saved_rep!=0) {
      assert(saved_rep>=2);
      (saved+1)[-1]=257-saved_rep;
      (saved+1)[1]=(char)128; /* EOD */
      out.vi_write((saved+1)-1,3);
      // fprintf(stderr,"rd=%u\n",saved_rep);
    } else if (saved_c!=0) {
      (saved+1)[-1]=saved_c-1;
      (saved+1)[saved_c]=(char)128; /* EOD */
      out.vi_write((saved+1)-1, saved_c+2);
      // fprintf(stderr,"re=%u\n",saved_c);
    } else {
      (saved+1)[-1]=(char)128;
      out.vi_write((saved+1)-1, 1);
    }
    out.vi_write(0,0); /* propagate EOF */
    record_left=0; /* signal that no further data will be accepted */
    return;
  }
 again:
  assert(record_left>=1);
  assert(len>=1);
  /* Imp: locally cache vars saved* */
  j=0;
  if (saved_c==0) {
    saved_rep=0; saved_c=1; b=(saved+1)[0]=*buf++; len--;
    if (0==--record_left) goto put_norep;
    if (0==len) return;
    goto yes;
  }
  if (saved_c==1 && saved_rep==0) { yes:
    if (*buf++==(saved+1)[0]) {
      saved_rep=2; len--;
      if (0==--record_left) goto put_rep;
    } else {
      (saved+1)[1]=buf[-1]; saved_c=2; len--;
      if (0==--record_left) goto put_norep;
    }
    if (0==len) return;
  }
  assert(record_left>=1);
  assert(len>=1);
  assert((saved_rep==0 && saved_c>=2) || (saved_rep>=2 && saved_c==1));
  if (saved_rep!=0) { /* Try to increase the repeating sequence */
    assert(saved_rep>=2);
    assert(saved_c==1);
    assert(len>=1);
    b=(saved+1)[0];
    umax=len>128?128:len; if (umax>record_left) umax=record_left;
    /* fprintf(stderr,"um1=%u\n", umax); */
    if (umax>128-saved_rep) umax=128-saved_rep;
    assert(umax>=1);
    j=0; while (j!=umax && buf[j]==b) { j++; saved_rep++; }
    if (j!=len || saved_rep==128) {
     put_rep: /* Found a maximal repeat width */
      (saved+1)[-1]=257-saved_rep;
      out.vi_write((saved+1)-1,2);
      /* fprintf(stderr,"ra=%u\n",saved_rep); */
      if ((record_left-=j)==0) record_left=recordsize;
      buf+=j;
      saved_c=saved_rep=0;
      if (0==(len-=j)) return;
      goto again;
    } /* Current repeat width can be further increased */
  } else { /* Try to increase the non-repeating sequence */
    assert(saved_c>=2);
    if (buf[0]==(saved+1)[saved_c-1]) { /* this decision might be sub-optimal */
      saved_c--;
      /* Now: saved_c: non-repeat length, >=1 */
      (saved+1)[-1]=saved_c-1;
      out.vi_write((saved+1)-1, saved_c+1);
      /* fprintf(stderr,"rb=%u\n",saved_c); */
      // record_left++; /* because of saved_c--; but we would increase it later anyway */
      // buf+=saved_c;
      (saved+1)[0]=buf[0]; /* first matching char-pair data */
      saved_c=1; saved_rep=2; /* make the new block a >=2 repeat */
      record_left--; buf++;
      if (0==--len) return;
      goto again;
    }
    (saved+1)[saved_c++]=buf[0];
    //record_left--;
    //buf++;
    //len--;
    //if (saved_c==128) goto put_norep;
    //if (0==len) return;
    umax=len>128?128:len; if (umax>record_left) umax=record_left;
    if (umax>128-saved_c) umax=128-saved_c;
    /* fprintf(stderr,"um2=%u\n", umax); */
    assert(umax>=1);
    j=1; while (j!=umax && buf[j]!=buf[j-1]) (saved+1)[saved_c++]=buf[j++];
    if (j!=len || saved_c==128) {
     put_norep: /* Found a maximal non-repeat width */
      (saved+1)[-1]=saved_c-1;
      out.vi_write((saved+1)-1, saved_c+1);
      /* fprintf(stderr,"rc=%u\n",saved_c); */
      if ((record_left-=j)==0) record_left=recordsize;
      buf+=j;
      saved_c=saved_rep=0;
      if (0==(len-=j)) return;
      goto again;
    } /* Current non-repeat width can be further increased */
  }
  assert(j==len);
  record_left-=j;
  assert(saved_rep<128);
  assert(saved_c<128);
}

/* --- */

#if USE_BUILTIN_ZIP
// bool FlateEncode::locked=false;

#if PTS_DEFL_RIPPED_ZLIB
FlateEncode::FlateEncode(GenBuffer::Writable &out_, unsigned level_)
 :out(out_) {
  // assert(!locked); locked /* locking is not necessary anymore */
  // pts_deflate_init(&fs); /* obsolete */
  zs.total_in=0;
  zs.total_out=0;
  zs.workspace=workspace;
  zs.msg=(char*)0;
  zs.state=(struct zlib_internal_state*)0;
  zs.data_type=Z_UNKNOWN; /* Imp: do we have to initialize it? */
  assert(zlib_deflate_workspacesize()+(unsigned)0<sizeof(workspace) && "Flate workspace too small");
  if (Z_OK!=zlib_deflateInit(&zs, level_))
    Error::sev(Error::EERROR) << "Flate init error (out of memory?)" << (Error*)0;
}

void FlateEncode::vi_write(char const*buf, slen_t len) {
  slen_t got, zgot;
  /* fprintf(stderr,"wcall\n"); */
  zs.next_in=(unsigned char*)const_cast<char*>(buf);   zs.avail_in=len;
  if (len==0) { /* flush all output */
    do { /* SUXX: C compiler should emit a warning: while (1) { ... } while(...); */
      zs.next_out=(unsigned char*)obuf; zs.avail_out=sizeof(obuf);
      /* fprintf(stderr,"wdone zai=%d zao=%d\n", zs.avail_in, zs.avail_out); */
      if (Z_STREAM_END!=(zgot=zlib_deflate(&zs, Z_FINISH)) && Z_OK!=zgot)
        Error::sev(Error::EERROR) << "Flate close error: " << zs.msg << (Error*)0;
      got=sizeof(obuf)-zs.avail_out;
      /* fprintf(stderr, "got=%u zgot=%d Z_OK=%d\n", got, zgot, Z_OK); */
      if (got>0) out.vi_write(obuf, got);
    } while (zgot==Z_OK);
    /* Dat: zlib_deflateEnd() will be called in the destructur */
    out.vi_write(0,0); /* Signal EOF */
    /* Dat: zlib_deflate() adds RFC 1950 header and adler32 checksum automatically */
  } else {
    do {
      /* fprintf(stderr,"writ\n"); */
      zs.next_out=(unsigned char*)obuf;  zs.avail_out=sizeof(obuf);
      if (Z_OK!=zlib_deflate(&zs, 0))
        Error::sev(Error::EERROR) << "Flate write error: " << zs.msg << (Error*)0;
      if (0<(got=sizeof(obuf)-zs.avail_out)) out.vi_write(obuf, got);
    } while (0!=zs.avail_in);
  }
}
#else
FlateEncode::FlateEncode(GenBuffer::Writable &out_, unsigned level_)
 :out(out_)
 ,s1(1)
 ,s2(0)
 ,had_header(false) {
  // assert(!locked); locked /* locking is not necessary anymore */
  // pts_deflate_init(&fs); /* obsolete */
  fs=pts_defl_new(
    /*zpfwrite=*/ gen_write,
    /*zpfmalloc=*/ gen_malloc,
    /*zpffree=*/ gen_free,
    /*pack_level=*/ level_,
    /*zfile=*/ (void*)&out_
  );
  if (fs==NULL) Error::sev(Error::EERROR) << "Flate init error (out of memory?)" << (Error*)0;
}
void *FlateEncode::gen_malloc(unsigned n) {
  return operator new(n);
  // return new char[n];
}
void FlateEncode::gen_free(void *p) {
  /*return*/ operator delete(p);
  // delete [] (char*)p;
}

void FlateEncode::vi_write(char const*buf, slen_t len) {
  register s_t sold;
  register char const*p=buf; char const*pend=p+len;
  (void)sold;
  if (!had_header) {
    out.vi_write("\x78\xda",2); /* ZLIB (RFC 1950): max compression header */
    had_header=true;
  }
  if (len==0) { /* EOF: send trailer */
    fs->deflate2(0,0,fs); /* Let the compressor flush its buffers. */
    if (fs->err!=0) Error::sev(Error::EERROR) << "Flate compression error" << (Error*)0;
    fs->delete2(fs);
    fs=(struct pts_defl_interface*)NULL;
    if (s1>=65521) s1-=65521;
    if (s2>=65521) s2-=65521;
    unsigned char trailer[4];
    trailer[0]=s2>>8; trailer[1]=s2&255; trailer[2]=s1>>8; trailer[3]=s1&255;
    out.vi_write((char const*)trailer,4);
    out.vi_write(0,0); /* Signal EOF */
    return;
  }
  assert(fs!=NULL);
  
  /* From rfc1950.txt:
           Adler-32 is composed of two sums accumulated per byte: s1 is
           the sum of all bytes, s2 is the sum of all s1 values. Both sums
           are done modulo 65521. s1 is initialized to 1, s2 to zero.  The
           Adler-32 checksum is stored as s2*65536 + s1 in most-
           significant-byte first (network) order.
  */
  /* Update Adler-32 checksum */
  while (p!=pend) {
    #if SIZEOF_INT>2
      if ((s1+=*(unsigned char const*)p)>=65521) s1-=65521;
      if ((s2+=s1)>=65521) s2-=65521;
    #elif SIZEOF_SHORT==2
      sold=s1;
      if ((s1+=*(unsigned char const*)p))<sold) s1+=15; /* 15==65536-21 */
      sold=s2;
      s2=(s2+s1)&0xffff;
      if ((s2+=s1)<sold) s2+=15;
    #else
      /* vvv 0xffff is needed since unsigned short may be > 0..65535 */
      sold=s1;
      s1=(s1+*(unsigned char const*)p)&0xffff;
      if (s1<sold) s1+=15; /* 15==65536-21 */
      sold=s2;
      s2=(s2+s1)&0xffff;
      if (s2<sold) s2+=15;
    #endif
    p++;
  }
  while (len>=0x8000) { fs->deflate2(const_cast<char*>(buf),0x8000, fs); len-=0x8000; buf+=0x8000; }
  if (len!=0) fs->deflate2(const_cast<char*>(buf),len, fs);
}
#endif /* PTS_DEFL_RIPPED_ZLIB */
#endif /* USE_BUILTIN_ZIP */

int /*FlateEncode::*/gen_write(char *block, unsigned len, void *zfile) {
  static_cast<GenBuffer::Writable*>(zfile)->vi_write(block, len);
  return 0;
}


/* --- */

#if USE_BUILTIN_FAXE
void* CCITTFaxEncode::gen_xalloc(unsigned n) {
  return operator new(n);
  //  void *ret; if ((ret=malloc(n))==0) abort(); return ret;
}
void CCITTFaxEncode::gen_free(void *ptr) {
  /*return*/ operator delete(ptr);
  // free(ptr);
}
void CCITTFaxEncode::gen_memset(void *s, int c, unsigned n) {
  /*return*/ memset(s,c,n);
}
void CCITTFaxEncode::gen_memcpy(void *dest, const void *src, unsigned n) {
  /*return*/ memcpy(dest, src, n);
}
CCITTFaxEncode::CCITTFaxEncode(GenBuffer::Writable &out_, slendiff_t K, slen_t Columns, bool EndOfLine, bool BlackIs1): out(out_) {
  sCFEs.memset_=gen_memset;
  sCFEs.xalloc_=gen_xalloc;
  sCFEs.free_=gen_free;
  sCFEs.memcpy_=gen_memcpy;
  s_CFE_template.set_defaults((stream_state*)&sCFEs);
  sCFEs.K=K;
  sCFEs.Columns=Columns;
  sCFEs.EndOfLine=EndOfLine;
  sCFEs.BlackIs1=BlackIs1;
  const int cf_max_height=(unsigned)-1/2-100; /* Dat: was slen_t */
  if (sCFEs.K < -cf_max_height || sCFEs.K > cf_max_height /* Dat: .K is an int */
   || sCFEs.Columns < 0 || sCFEs.Columns > cfe_max_width /* Dat: .Columns is an int */
   || sCFEs.Rows < 0 || sCFEs.Rows > cf_max_height /* Dat: .Rows is an int */
   || sCFEs.DamagedRowsBeforeError < 0
   || sCFEs.DamagedRowsBeforeError > cf_max_height /* Dat: .DamagedRowsBeforeError is an int */
   || sCFEs.DecodedByteAlign < 1 || sCFEs.DecodedByteAlign > 16
   || (sCFEs.DecodedByteAlign & (sCFEs.DecodedByteAlign - 1)) != 0
     ) Error::sev(Error::EERROR) << "pts_fax: invalid params" << (Error*)0;
  if (0!=s_CFE_template.init((stream_state*)&sCFEs))
    Error::sev(Error::EERROR) << "pts_fax: init failed" << (Error*)0;
  #if __CHECKER__
    memset(&r, 0, sizeof(r));
    memset(&w, 0, sizeof(w));
  #endif
  r.ptr=rlimit=rbuf-1;
  hard=rbuf+sizeof(rbuf)-1;
  assert(hard-r.ptr>=(int)s_CFE_template.min_in_size);
}

void CCITTFaxEncode::vi_write(char const*buf, slen_t len) {
  int pog;
  // unsigned char *save_wptr;
  if (len==0) {
    r.limit=rlimit;
    do {
      w.ptr=wbuf-1;
      w.limit=wbuf+sizeof(wbuf)-1;
      assert(w.limit-w.ptr>=(int)s_CFE_template.min_out_size);
      pog=s_CFE_template.process((stream_state*)&sCFEs, &r, &w, /*last:*/ true);
      // fprintf(stderr, "pog=%d write=%d last=%d\n", pog, w.ptr-(wbuf-1), true);
      assert(pog!=PTSFAX_ERRC); /* /CCITTFaxEncode filter must accept any input */
      assert(pog!=PTSFAX_EOFC); /* /CCITTFaxEncode filter doesn't have EOD markers */
      if (w.ptr!=wbuf-1) out.vi_write((char const*)wbuf, w.ptr-(wbuf-1));
    } while (pog==1);
    s_CFE_template.release((stream_state*)&sCFEs);
    out.vi_write(0,0); /* propagate EOF */
    return;
  }

  while (len!=0) {
    assert(r.ptr==rbuf-1);
    assert(hard>rlimit);
    unsigned clen=hard-rlimit;
    if (clen>len) clen=len;
    assert(clen>0);
    // fprintf(stderr, "clen=%u\n", clen);
    memcpy(rlimit+1, buf, clen);
    rlimit+=clen;
    buf+=clen;
    len-=clen;
    /* if (r.ptr==rlimit) break; */
    r.limit=rlimit;
    do {
      w.ptr=wbuf-1;
      w.limit=wbuf+sizeof(wbuf)-1;
      assert(w.limit-w.ptr>=(int)s_CFE_template.min_out_size);
      pog=s_CFE_template.process((stream_state*)&sCFEs, &r, &w, /*last:*/ false);
      // fprintf(stderr, "len=%d pog=%d write=%d last=%d\n", len, pog, w.ptr-(wbuf-1), false);
      assert(pog!=PTSFAX_ERRC); /* /CCITTFaxEncode filter must accept any input */
      assert(pog!=PTSFAX_EOFC); /* /CCITTFaxEncode filter doesn't have EOD markers */
      if (w.ptr!=wbuf-1) out.vi_write((char const*)wbuf, w.ptr-(wbuf-1));
    } while (pog==1);
    // assert(pog!=1); /* not true: output space is enough (sizeof(wbuf)>min_out_size) */
    assert(pog==0); /* more input is needed */
    if (r.ptr!=rbuf-1) {
      // fprintf(stderr, "limit=%d\n", rlimit-r.ptr);
      memmove(rbuf, r.ptr+1, rlimit-r.ptr);
      rlimit=rbuf-1+(rlimit-r.ptr);
      r.ptr=rbuf-1;
    }
  }
  // fprintf(stderr, "done\n");
}
#endif /* USE_BUILTIN_FAXE */

/* --- */

#if USE_BUILTIN_LZW
LZWEncode::LZWEncode(GenBuffer::Writable &out_): out(out_) {
  fs.tif_writer=/*FlateEncode::*/gen_write;
  fs.tif_sout=(void*)&out_;
  if (0==pts_lzw_init(&fs)) Error::sev(Error::EERROR) << "LZW init error" << (Error*)0;
}

void LZWEncode::vi_write(char const*buf, slen_t len) {
  /* Imp: report real error _reason_ (if appropriate?? ) */
  if (len==0) {
    if (0==fs.tif_feeder(0,0,&fs)) goto we;
    out.vi_write(0,0); /* propagate EOF */
    return;
  }
  while (len>=0x8000) {
    if (0==fs.tif_feeder(const_cast<char*>(buf),0x8000,&fs)) we:
      Error::sev(Error::EERROR) << "LZW write error" << (Error*)0;
    len-=0x8000; buf+=0x8000;
  }
  if (len!=0 && 0==fs.tif_feeder(const_cast<char*>(buf),len,&fs)) goto we;
}
#endif /* USE_BUILTIN_LZW */

/* --- */

#if !USE_BUILTIN_LZW
Lzw_codecEncode::Lzw_codecEncode(GenBuffer::Writable &out_, char const*filter_psname)
  :p(out_) {
  (void)filter_psname;
}
void Lzw_codecEncode::vi_write(char const*buf, slen_t len) { p.vi_write(buf,len); }

/* Imp: figure out path-to-gs: gs or gswin32c */
Lzw_codecEncode::P::P(GenBuffer::Writable &out_)
 // :PipeEncoder(out_, ">/tmp/t cat - cjpeg quality %i", quality), rgbp(rgbp_) {
 // :Filter::PipeE(out_, "cjpeg -quality %i >%D", quality), rgbp(rgbp_) {
 :Filter::PipeE(out_, "lzw_codec encode >%D") {}
#endif

/* --- */

CjpegEncode::CjpegEncode(GenBuffer::Writable &out_, char const*filter_psname, slen_t Columns, slen_t Rows, bool rgbp_, unsigned char quality)
  :p(out_, Columns, Rows, rgbp_, quality) {
  (void)filter_psname;
}
void CjpegEncode::vi_write(char const*buf, slen_t len) { p.vi_write(buf,len); }

/* Imp: figure out path-to-gs: gs or gswin32c */
CjpegEncode::P::P(GenBuffer::Writable &out_, slen_t Columns, slen_t Rows, bool rgbp_, unsigned char quality)
 // :PipeEncoder(out_, ">/tmp/t cat - cjpeg quality %i", quality), rgbp(rgbp_) {
 :Filter::PipeE(out_, "cjpeg -quality %i >%D", quality), rgbp(rgbp_) {
  /* Dat: we're extremely lucky that cjpeg can read PGM or PPM files from stdin */
  // operator<<("P5 width height 255\n"); /* RAWBITS PGM */
  // operator<<("P6 width height 255\n"); /* RAWBITS PPM */
  *this << (rgbp_?"P6 ":"P5 ") << Columns << ' ' << Rows << " 255\n";
}
void CjpegEncode::P::vi_copy(FILE *f) {
  char r[9];
  static char jfif[9]="\377\340\000\020JFIF";
  static unsigned char adobe[16]= {
    0xff, /* marker */
    0xee, /* APP14 */
    0,    /* length-hi */
    14,   /* length-lo */
    'A', 'd', 'o', 'b', 'e', /* ID */
    1,    /* version-hi */
    0,    /* version-lo */
    0, 0, /* flags0 */
    0, 0, /* flags1 */
    0,    /* ColorTransform */
  };
  if (MACRO_GETC(f)!=0xff || MACRO_GETC(f)!=0xd8 || fread(r, 1, 8, f)!=8) {
    bad: Error::sev(Error::EERROR) << "CjpegEncode: cjpeg created bad JPEG" << (Error*)0;
  }
  out.vi_putcc((char)0xff);
  out.vi_putcc((char)0xd8);
  r[9]=r[3]; r[3]='\020';
  if ((unsigned char)r[9]>=6 && 0==memcmp(r, jfif, 8)) { /* JFIF marker */
    r[3]=r[9]; unsigned skip=r[9]-6;
    out.vi_write(r, 8);
    while (skip--!=0) out.vi_putcc(MACRO_GETC(f));
  }
  if (ferror(f) || feof(f)) goto bad;
  /* Now we can emit the Adobe marker. */
  adobe[sizeof(adobe)-1]=rgbp==true; /* ColorTransform value 0 for Gray, 1 for RGB->YCbCr */
  out.vi_write((char*)adobe, sizeof(adobe));
  /* vvv Dat: pacify VC6.0: Filter::PipeE::vi_copy(f); doesn't work */
  PipeE::vi_copy(f);
  /* ^^^ copy rest of file verbatim */
  // ((Filter::PipeE)*this).vi_copy(f);
}

/* --- */

GSEncode::GSEncode(GenBuffer::Writable &out_, char const*filter_psname)
  :p(out_, filter_psname) {}
void GSEncode::vi_write(char const*buf, slen_t len) { p.vi_write(buf,len); }

/* Imp: figure out path-to-gs: gs or gswin32c */
GSEncode::P::P(GenBuffer::Writable &out_, char const*filter_psname)
  :Filter::PipeE(out_, "gs -s_OFN=%D -dNODISPLAY -q - >%E")
  {
  operator<<("{/o _OFN(w)file ");
  operator<<(filter_psname);
  operator<<(" filter def/s 4096 string def"
             "{currentfile s readstring exch o exch writestring not{exit}if}loop "
             "o closefile quit}bind exec\n");
}
void GSEncode::P::vi_check() {
  /* If STDOUT of gs is not empty, then it is very probably an error message. */
  // tmpename.term0(); /* already is */
  assert(tmpename.end_()[0]=='\0');
  if (0!=Files::statSize(tmpename())) Error::sev(Error::EERROR) << "GSEncode: GS runtime error" << (Error*)0;
  /* Imp: display a meaningful error message */
}

/* --- */

PSEncoder* PSEncoder::newASCIIHexEncode(GenBuffer::Writable &out_,unsigned maxcpl_) {
  // SimBuffer::B fp; (fp << maxcpl_ << " pop/ASCIIHexEncode").term0();
  // PSEncoder *ret=new GSEncode(out_, fp());  
  PSEncoder *ret=new ASCIIHexEncode(out_,maxcpl_);
  // ret->shortname="AHx";  ret->longname="ASCIIHex";
  // ret->filter_psname << fp;
  return ret;
}
PSEncoder* PSEncoder::newASCII85Encode(GenBuffer::Writable &out_,unsigned maxcpl_) {
  // SimBuffer::B fp; (fp << maxcpl_ << " pop/ASCII85Encode").term0();
  PSEncoder *ret=new ASCII85Encode(out_,maxcpl_);
  // PSEncoder *ret=new GSEncode(out_, fp());
  // ret->shortname="A85";  ret->longname="ASCII85";
  // ret->filter_psname << fp;
  return ret;
}
PSEncoder* PSEncoder::newCCITTFaxEncode(GenBuffer::Writable &out_,slendiff_t K, slen_t Columns, bool EndOfLine, bool BlackIs1) {
  #if USE_BUILTIN_FAXE
    return new CCITTFaxEncode(out_, K, Columns, EndOfLine, BlackIs1);
  #else
    // (void)out_; (void)K; (void)Columns;
    // assert(0 && "unimplemented");
    SimBuffer::B fp("<< /K ");
    fp << K << "/Columns " << Columns;
    if (EndOfLine) fp << "/EndOfLine true"; /* PS default: false */
    if (BlackIs1)  fp << "/BlackIs1 true";
    (fp << ">>/CCITTFaxEncode").term0();
    // fprintf(stderr, "fp=(%s)\n", fp());
    PSEncoder *ret=new GSEncode(out_, fp());
    // ret->shortname="CCF";  ret->longname="CCITTFax";
    // ret->filter_psname << fp; /* Dat: this could be made faster */
    return ret;
  #endif
}
PSEncoder* PSEncoder::newLZWEncode(GenBuffer::Writable &out_) {
#if USE_BUILTIN_LZW
  SimBuffer::B fp("/LZWEncode");
  PSEncoder *ret=new LZWEncode(out_);
  // PSEncoder *ret=new GSEncode(out_, fp());
  // ret->shortname="LZW";  ret->longname="LZW";
  // ret->filter_psname << fp;
  return ret;
#else
  (void)out_;
  #if 0
    Error::sev(Error::EERROR) << "LZW not supported in this compilation of sam2p" << (Error*)0;
  #endif
  Error::sev(Error::WARNING) << "LZW: please `configure --enable-lzw' for builtin /Compression/LZW support" << (Error*)0;
  #if 0
    /* This is useless, because gs 5.50--7.04 have dummy LZW compressor:
     * it emits a correct LZW stream, but does no real compression, and
     * the file size is often increased.
     */
    PSEncoder *ret=new GSEncode(out_, "/LZWEncode");
    // ret->shortname="LZW";  ret->longname="LZW";
    // ret->filter_psname << "/LZWEncode";
    return ret;
  #endif
  #if 1 /* ask lzw_codec from the author of sam2p */
    return new Lzw_codecEncode(out_, "/LZWEncode");
  #endif
  return 0;
#endif
}
PSEncoder* PSEncoder::newFlateEncode(GenBuffer::Writable &out_, signed Effort) {
  // (void)out_; (void)K; (void)Columns;
  // assert(0 && "unimplemented");
  if (Effort==0) {
    PSEncoder *ret=new FlateStoreEncode(out_);
    // ret->shortname="Fla";    ret->longname="Flate";
    // ret->filter_psname << fp;
    return ret;
  }
  if (Effort<1 || Effort>9) Effort=5;
  #if USE_BUILTIN_ZIP
    PSEncoder *ret=new FlateEncode(out_, Effort);
  #else
    SimBuffer::B fp("<</Effort "); (fp << Effort << ">>/FlateEncode").term0();
    PSEncoder *ret=new GSEncode(out_, fp());
  #endif
  // ret->shortname="Fla";  ret->longname="Flate";
  // ret->filter_psname << fp;
  return ret;
}
PSEncoder* PSEncoder::newRunLengthEncode(GenBuffer::Writable &out_, slen_t RecordSize) {
  SimBuffer::B fp; (fp << RecordSize << "/RunLengthEncode").term0();
  PSEncoder *ret=new RunLengthEncode(out_, RecordSize);
  // PSEncoder *ret=new GSEncode(out_, fp());
  // ret->shortname="RL";  ret->longname="RunLength";
  // ret->filter_psname << fp;
  return ret;
}

PSEncoder* PSEncoder::newDCTIJGEncode(GenBuffer::Writable &out_,
  slen_t Columns,
  slen_t Rows,
  unsigned char Colors, /*1..4*/
  unsigned char quality /*libJPEG quality: 0..100 */
) {
  /* Dat: this supports only the Gray and RGB color spaces of JPEG */
  param_assert(Colors==1 || Colors==3);
  param_assert(/* quality>=0 && */ quality <=100);
  SimBuffer::B fp("<<IJG ");
  (fp<< "/Columns " << Columns
     << "/Rows " << Rows
     << "/Colors " << (unsigned)Colors
     << ">>/DCTEncode").term0();
  /* Dat: the default of /ColorTransform (defined in DCTEncode in
     subsubsection 3.13.3 in PLRM.pdf) is just perfect. */
  PSEncoder *ret=new CjpegEncode(out_, fp(), Columns, Rows, Colors==3, quality);
  // ret->longname=ret->shortname="DCT";  ret->filter_psname << fp;
  return ret;
}

PSEncoder* PSEncoder::newDCTEncode(GenBuffer::Writable &out_,
  slen_t Columns,
  slen_t Rows,
  unsigned char Colors, /*1..4*/
  unsigned char quality, /*libJPEG quality: 0..100 */
  unsigned char const*HSamples, /*all 1..4, NULLP OK*/
  unsigned char const*VSamples, /*all 1..4, NULLP OK*/
  unsigned char (*QuantTables)[64], /*NULLP OK*/
  double        QFactor, /*0..1000000*/
  unsigned      numHuffTables,
  unsigned char **HuffTables, /*NULLP OK*/
  unsigned char ColorTransform /*0,1,2 3=default*/
) {
  (void)quality;
  (void)QuantTables;
  (void)QFactor;
  (void)numHuffTables;
  (void)HuffTables;
  SimBuffer::B tmp;
  /* Imp: respect quality */
  /* Imp: respect QFactor (double) */
  /* Imp: respect QuantTables */
  /* Imp: respect numHuffTables, HuffTables */
  
  SimBuffer::B fp("<<");
  fp << "/Columns " << Columns
     << "/Rows " << Rows
     << "/Colors " << (unsigned)Colors;
  if (HSamples!=(unsigned char const*)NULLP) {
    tmp.clear(); tmp.vi_write((char const*)HSamples, (unsigned)Colors);
    fp << "/HSamples "; fp.appendDumpPS(tmp,true);
  }
  if (VSamples!=(unsigned char const*)NULLP) {
    tmp.clear(); tmp.vi_write((char const*)VSamples, (unsigned)Colors);
    fp << "/VSamples "; fp.appendDumpPS(tmp,true);
  }
  if (ColorTransform!=3) fp << "/ColorTransform " << (unsigned)ColorTransform;
  (fp << ">>/DCTEncode").term0();

  // PSEncoder *ret=new DCTEncode(out_,? ? ?);
  PSEncoder *ret=new GSEncode(out_, fp());
  // ret->shortname="DCT";  ret->longname="DCT";
  // ret->filter_psname << fp;
  return ret;
}

PSEncoder* PSEncoder::newDCTEncode(GenBuffer::Writable &out_,
  slen_t Columns,
  slen_t Rows,
  unsigned char Colors, /*1..4*/
  unsigned char ColorTransform, /*0,1,2 3=default*/
  SimBuffer::Flat const& other_parameters
) {
  SimBuffer::B fp("<<");
  fp << "/Columns " << Columns
     << "/Rows " << Rows
     << "/Colors " << (unsigned)Colors;
  if (ColorTransform!=3) fp << "/ColorTransform " << (unsigned)ColorTransform;
  (fp << other_parameters << ">>/DCTEncode").term0();
  PSEncoder *ret=new GSEncode(out_, fp());
  // ret->shortname="DCT";  ret->longname="DCT";
  // ret->filter_psname << fp;
  return ret;
}

/* --- */

TIFFPredictor2::TIFFPredictor2(GenBuffer::Writable &out_, unsigned char bpc_, slen_t columns_, unsigned char cpp_)
 : h(0), bpc(bpc_), cpp(cpp_), out(out_) {
  param_assert(cpp_*bpc_<=32);
  rlen=(columns_*cpp_*bpc_+7)>>3; /* BUGFIX at Tue Mar 12 12:09:51 CET 2002 */
  op=obuf=new unsigned char[rlen];
}

void TIFFPredictor2::vi_write(char const*buf, slen_t len) {
  unsigned char const *p=(unsigned char const*)buf, *pend0=p+len;
  slen_t opleft=rlen-(op-obuf);
  register unsigned int i, d, o, bpccpp;
  if (len==0) {
    assert(opleft==rlen); /* unflushed (half-ready) row disallowed */
    assert(obuf!=NULLP);
    delete [] obuf;
    obuf=(unsigned char*)NULLP;
    out.vi_write(0,0);
    return;
  }
  bpccpp=(cpp-1)*bpc;
  if (bpc==1) {
    while (p!=pend0) {
      i=*p++;
      d=(i>>7); o =((d-((h>>bpccpp)))&1)<<7; h=(h<<1)|d;
      d=(i>>6); o|=((d-((h>>bpccpp)))&1)<<6; h=(h<<1)|d;
      d=(i>>5); o|=((d-((h>>bpccpp)))&1)<<5; h=(h<<1)|d;        
      d=(i>>4); o|=((d-((h>>bpccpp)))&1)<<4; h=(h<<1)|d;
      d=(i>>3); o|=((d-((h>>bpccpp)))&1)<<3; h=(h<<1)|d;        
      d=(i>>2); o|=((d-((h>>bpccpp)))&1)<<2; h=(h<<1)|d;
      d=(i>>1); o|=((d-((h>>bpccpp)))&1)<<1; h=(h<<1)|d;        
      d=(i   ); o|=((d-((h>>bpccpp)))&1)   ; h=(h<<1)|d;
      *op++=o;
      if (--opleft==0) { h=0; out.vi_write((char*)obuf,rlen); op=obuf; opleft=rlen; }
    }
  } else if (bpc==2) {
    while (p!=pend0) {
      i=*p++;
      d=(i>>6); o =((d-((h>>bpccpp)))&3)<<6; h=(h<<2)|d; // fprintf(stderr,"d=%#x\n", d);
      d=(i>>4); o|=((d-((h>>bpccpp)))&3)<<4; h=(h<<2)|d;
      d=(i>>2); o|=((d-((h>>bpccpp)))&3)<<2; h=(h<<2)|d;        
      d=(i   ); o|=((d-((h>>bpccpp)))&3)   ; h=(h<<2)|d;
      *op++=o;
      if (--opleft==0) { h=0; out.vi_write((char*)obuf,rlen); op=obuf; opleft=rlen; }
    }
  } else if (bpc==4) {
    while (p!=pend0) {
      i=*p++;
      d=(i>>4); o =((d-((h>>bpccpp)))&15)<<4; h=(h<<4)|d;
      d=(i   ); o|=((d-((h>>bpccpp)))&15)   ; h=(h<<4)|d;
      *op++=o;
      if (--opleft==0) { h=0; out.vi_write((char*)obuf,rlen); op=obuf; opleft=rlen; }
    }
  } else if (bpc==8) {
    while (p!=pend0) {
      i=*p++; *op++=((i-((h>>bpccpp)))/*&255*/); h=(h<<8)|i;
      if (--opleft==0) { h=0; out.vi_write((char*)obuf,rlen); op=obuf; opleft=rlen; }
    }
  } else assert(0);  
}

/* --- */

PNGPredictorNone::PNGPredictorNone(GenBuffer::Writable &out_, unsigned char bpc_, slen_t columns_, unsigned char cpp_)
 : opleft(0), out(out_) {
  rlen=(columns_*cpp_*bpc_+7)>>3;
}

void PNGPredictorNone::vi_write(char const*buf, slen_t len) {
  if (len==0) {
    assert(opleft==0); /* unflushed (half-ready) row disallowed */
    out.vi_write(0,0);
    return;
  }
  /* The following code just inserts a '\0' in front of each scanline */
  /* Imp: make it faster by collapsing vi_writes */
  if (opleft==0) { opleft=rlen; out.vi_write("\0",1); } /* Scanline (row) header: describes predictor used */
  while (len>opleft) {
    out.vi_write(buf,opleft);
    buf+=opleft; len-=opleft;
    opleft=rlen; out.vi_write("\0",1); /* Scanline (row) header: describes predictor used */
  }
  if (len!=0) out.vi_write(buf,len);
  opleft-=len;
}

/* --- */

PNGPredictorSub::PNGPredictorSub(GenBuffer::Writable &out_, unsigned char bpc_, slen_t columns_, unsigned char cpp_)
 : h(0), out(out_) {
  param_assert(cpp_*bpc_<=32);
  rlen=(columns_*cpp_*bpc_+7)>>3;
  op=obuf=1+new unsigned char[rlen+1];
  obuf[-1]='\1'; /* Scanline (row) header: describes predictor used */
  bpccpp=((cpp_*bpc_+7)&~7)-8;
}

void PNGPredictorSub::vi_write(char const*buf, slen_t len) {
  unsigned char const *p=(unsigned char const*)buf, *pend0=p+len;
  slen_t opleft=rlen-(op-obuf);
  register unsigned int i;
  if (len==0) {
    assert(opleft==rlen); /* unflushed (half-ready) row disallowed */
    assert(obuf!=NULLP);
    delete [] (obuf-1);
    obuf=(unsigned char*)NULLP;
    out.vi_write(0,0);
    return;
  }
  while (p!=pend0) {
    i=*p++; *op++=((i-((h>>bpccpp)))/*&255*/); h=(h<<8)|i;
    if (--opleft==0) { h=0;
      out.vi_write((char*)obuf-1,rlen+1); op=obuf; opleft=rlen;
    }
  }
}

/* --- */

PNGPredictorUp::PNGPredictorUp(GenBuffer::Writable &out_, unsigned char bpc_, slen_t columns_, unsigned char cpp_)
 : out(out_) {
  rlen=(columns_*cpp_*bpc_+7)>>3;
  // fprintf(stderr, "rUp.rlen=%u cpp=%u bpc=%u\n", rlen, cpp_, bpc_);
  op=obuf=1+new unsigned char[2*rlen+1];
  oq=op+rlen; /* prev scanline */
  memset(oq, '\0', rlen);
  obuf[-1]='\2'; /* Scanline (row) header: describes predictor used */
}

void PNGPredictorUp::vi_write(char const*buf, slen_t len) {
  unsigned char const *p=(unsigned char const*)buf, *pend0=p+len;
  slen_t opleft=rlen-(op-obuf);
  if (len==0) {
    assert(opleft==rlen); /* unflushed (half-ready) row disallowed */
    assert(obuf!=NULLP);
    delete [] (obuf-1);
    obuf=(unsigned char*)NULLP;
    out.vi_write(0,0);
    return;
  }
  while (p!=pend0) {
    *op++=((*p-*oq)/*&255*/); *oq++=*p++;
    if (--opleft==0) {
      out.vi_write((char*)obuf-1,rlen+1); opleft=rlen;
      op=obuf; oq=obuf+rlen;
    }
  }
}

/* --- */

PNGPredictorAverage::PNGPredictorAverage(GenBuffer::Writable &out_, unsigned char bpc_, slen_t columns_, unsigned char cpp_)
 : h(0), /*g(0),*/ out(out_) {
  param_assert(cpp_*bpc_<=32);
  rlen=(columns_*cpp_*bpc_+7)>>3;
  op=obuf=1+new unsigned char[2*rlen+1];
  oq=op+rlen; /* prev scanline */
  memset(oq, '\0', rlen);
  obuf[-1]='\3'; /* Scanline (row) header: describes predictor used */
  bpccpp=((cpp_*bpc_+7)&~7)-8;
}

void PNGPredictorAverage::vi_write(char const*buf, slen_t len) {
  unsigned char const *p=(unsigned char const*)buf, *pend0=p+len;
  slen_t opleft=rlen-(op-obuf);
  register unsigned int i;
  if (len==0) {
    assert(opleft==rlen); /* unflushed (half-ready) row disallowed */
    assert(obuf!=NULLP);
    delete [] (obuf-1);
    obuf=(unsigned char*)NULLP;
    out.vi_write(0,0);
    return;
  }
  while (p!=pend0) {
    /* vvv Data: *og+h can be 0..510 */
    i=*p; *op++=i-((((h>>bpccpp)&255)+*oq)>>1); h=(h<<8)|i; *oq++=*p++;
    // i=*p; *op++=(*p-((*oq+h)>>1)/*&255*/); h=i; *oq++=*p++;
    if (--opleft==0) {
      out.vi_write((char*)obuf-1,rlen+1); opleft=rlen;
      op=obuf; oq=obuf+rlen; h=0;
    }
  }
}

/* --- */

/* Dat: egcs-2.91.60 is buggy */
// static inline unsigned abs_(unsigned i) { return ((signed)i)<0 ? -i : i; }
static inline unsigned abs_(unsigned i) { return ((signed)i)<0 ? (i*-1) : i; }

static inline unsigned paeth_predictor(unsigned a, unsigned b, unsigned c) {
  /* Code ripped from RFC 2083 (PNG specification), which also says:
   * The calculations within the PaethPredictor function must be
   * performed exactly, without overflow.  Arithmetic modulo 256 is to
   * be used only for the final step of subtracting the function result
   * from the target byte value.
   */
  /* a = left, b = above, c = upper left */
  unsigned p  = a + b - c;       /* initial estimate */
  unsigned pa = abs_(p - a);     /* distances to a, b, c */
  unsigned pb = abs_(p - b);
  unsigned pc = abs_(p - c);
  // assert(abs_(3*(-1))==3);
  // fprintf(stderr, "b=%u c=%u %d %d\n", b, c, p-b, ((signed)(p-b))<0 ? (b-p) : (p-b) );
  // fprintf(stderr, "pa=%u pb=%u pc=%u\n", pa, pb, pc);
  assert(pa<=255);
  assert(pb<=255);
  assert(pc<=255*2);
  /* return nearest of a,b,c, breaking ties in order a,b,c. */
  return (pa <= pb && pa <= pc) ? a
       : pb <= pc ? b
       : c;
}

PNGPredictorPaeth::PNGPredictorPaeth(GenBuffer::Writable &out_, unsigned char bpc_, slen_t columns_, unsigned char cpp_)
 : h(0), g(0), out(out_) {
  param_assert(cpp_*bpc_<=32);
  rlen=(columns_*cpp_*bpc_+7)>>3;
  op=obuf=1+new unsigned char[2*rlen+2];
  oq=obuf+rlen+1; /* prev scanline */
  memset(obuf+rlen, '\0', rlen+1);
  obuf[-1]='\4'; /* Scanline (row) header: describes predictor used */
  bpccpp=((cpp_*bpc_+7)&~7)-8;
}

void PNGPredictorPaeth::vi_write(char const*buf, slen_t len) {
  unsigned char const *p=(unsigned char const*)buf, *pend0=p+len;
  slen_t opleft=rlen-(op-obuf);
  register unsigned int i;
  if (len==0) {
    assert(opleft==rlen); /* unflushed (half-ready) row disallowed */
    assert(obuf!=NULLP);
    delete [] (obuf-1);
    obuf=(unsigned char*)NULLP;
    out.vi_write(0,0);
    return;
  }
  while (p!=pend0) {
    // assert(0==obuf[rlen] && 4==obuf[-1]);
    i=*p;  *op++=i-paeth_predictor((h>>bpccpp)&255, *oq, (g>>bpccpp)&255);
    h=(h<<8)|i; g=(g<<8)|*oq; *oq++=*p++;
    // i=*p; *op++=i-h; h=i; *oq++=*p++;
    if (--opleft==0) {
      out.vi_write((char*)obuf-1,rlen+1); opleft=rlen;
      op=obuf; oq=obuf+rlen+1; h=0; g=0;
    }
  }
}

/* --- */

PNGPredictorAuto::PNGPredictorAuto(GenBuffer::Writable &out_, unsigned char bpc_, slen_t columns_, unsigned char cpp_)
 : h(0), g(0), out(out_) {
  param_assert(cpp_*bpc_<=32);
  opleft=rlen=(columns_*cpp_*bpc_+7)>>3;
  obuf=new unsigned char[6*rlen+6];
  o_prior=obuf+rlen+1; /* Prior(x): ooprior[-opleft] */
  obuf[rlen*1+1]='\0'; o_0=obuf+2*rlen+2;
  obuf[rlen*2+2]='\1'; o_1=obuf+3*rlen+3;
  obuf[rlen*3+3]='\2'; o_2=obuf+4*rlen+4;
  obuf[rlen*4+4]='\3'; o_3=obuf+5*rlen+5;
  obuf[rlen*5+5]='\4'; o_4=obuf+6*rlen+6;
  oo[0]=o_0; oo[1]=o_1; oo[2]=o_2; oo[3]=o_3; oo[4]=o_4;
  memset(obuf, '\0', rlen+1);
  bpccpp=((cpp_*bpc_+7)&~7)-8;
}

void PNGPredictorAuto::vi_write(char const*buf, slen_t len) {
  unsigned char const *p=(unsigned char const*)buf, *pend0=p+len;
  register unsigned int i;
  register unsigned raw_x_bpp, prior_x, prior_x_bpp;
  if (len==0) {
    assert(opleft==rlen); /* unflushed (half-ready) row disallowed */
    assert(obuf!=NULLP);
    delete [] obuf;
    obuf=(unsigned char*)NULLP;
    out.vi_write(0,0);
    return;
  }
  while (p!=pend0) {
    raw_x_bpp=(h>>bpccpp) &255;
    prior_x=*(o_prior-opleft);
    prior_x_bpp=(g>>bpccpp) &255;
    i=*p;
    *(o_0-opleft)=i;
    *(o_1-opleft)=i-raw_x_bpp;
    *(o_2-opleft)=i-prior_x;
    *(o_3-opleft)=i-((raw_x_bpp+prior_x)>>1);
    *(o_4-opleft)=i-paeth_predictor(raw_x_bpp, prior_x, prior_x_bpp);
    h=(h<<8)|i; g=(g<<8)|*(o_prior-opleft); *(o_prior-opleft)=*p++;
    if (--opleft==0) {
      /* Select the predictor having the smallest signed sum of values. */
      slendiff_t min_weight, cur_weight;
      unsigned min_pred=0, cur_pred;
      register signed char *beg, *end;
      min_weight=0; beg=(end=(signed char*)o_0)-rlen; while (beg!=end) min_weight+=*beg++;
      if (min_weight<0) min_weight*=-1;
      for (cur_pred=1; cur_pred<=4; cur_pred++) {
        cur_weight=0; beg=(end=(signed char*)oo[cur_pred])-rlen; while (beg!=end) cur_weight+=*beg++;
        if (cur_weight<0) cur_weight*=-1;
        if (cur_weight<min_weight) { min_weight=cur_weight; min_pred=cur_pred; }
      }
      // fprintf(stderr, "cp=%u\n", min_pred);

      out.vi_write((char*)(oo[min_pred]-rlen-1),rlen+1);
      opleft=rlen; h=0; g=0;
    }
  }
}

/* --- */

PNGPredictorAutoUnsigned::PNGPredictorAutoUnsigned(GenBuffer::Writable &out_, unsigned char bpc_, slen_t columns_, unsigned char cpp_)
 : h(0), g(0), out(out_) {
  param_assert(cpp_*bpc_<=32);
  opleft=rlen=(columns_*cpp_*bpc_+7)>>3;
  obuf=new unsigned char[6*rlen+6];
  o_prior=obuf+rlen+1; /* Prior(x): ooprior[-opleft] */
  obuf[rlen*1+1]='\0'; o_0=obuf+2*rlen+2;
  obuf[rlen*2+2]='\1'; o_1=obuf+3*rlen+3;
  obuf[rlen*3+3]='\2'; o_2=obuf+4*rlen+4;
  obuf[rlen*4+4]='\3'; o_3=obuf+5*rlen+5;
  obuf[rlen*5+5]='\4'; o_4=obuf+6*rlen+6;
  oo[0]=o_0; oo[1]=o_1; oo[2]=o_2; oo[3]=o_3; oo[4]=o_4;
  memset(obuf, '\0', rlen+1);
  bpccpp=((cpp_*bpc_+7)&~7)-8;
}

void PNGPredictorAutoUnsigned::vi_write(char const*buf, slen_t len) {
  unsigned char const *p=(unsigned char const*)buf, *pend0=p+len;
  register unsigned int i;
  register unsigned raw_x_bpp, prior_x, prior_x_bpp;
  // unsigned lines=0;
  if (len==0) {
    assert(opleft==rlen); /* unflushed (half-ready) row disallowed */
    assert(obuf!=NULLP);
    delete [] obuf;
    obuf=(unsigned char*)NULLP;
    out.vi_write(0,0);
    return;
  }
  // fprintf(stderr, "rlen=%u len=%u opleft=%u\n", rlen, len, opleft);
  while (p!=pend0) {
    raw_x_bpp=(h>>bpccpp) &255;
    prior_x=*(o_prior-opleft);
    prior_x_bpp=(g>>bpccpp) &255;
    i=*p;
    *(o_0-opleft)=i;
    *(o_1-opleft)=i-raw_x_bpp;
    *(o_2-opleft)=i-prior_x;
    *(o_3-opleft)=i-((raw_x_bpp+prior_x)>>1);
    *(o_4-opleft)=i-paeth_predictor(raw_x_bpp, prior_x, prior_x_bpp);
    h=(h<<8)|i; g=(g<<8)|*(o_prior-opleft); *(o_prior-opleft)=*p++;
    if (--opleft==0) {
      /* Select the predictor having the smallest unsigned sum of values. */
      slen_t min_weight, cur_weight;
      unsigned min_pred=0, cur_pred;
      register unsigned char *beg, *end;
      min_weight=0; beg=(end=(unsigned char*)o_0)-rlen; while (beg!=end) min_weight+=*beg++;
      for (cur_pred=1; cur_pred<=4; cur_pred++) {
        cur_weight=0; beg=(end=(unsigned char*)oo[cur_pred])-rlen; while (beg!=end) cur_weight+=*beg++;
        if (cur_weight<min_weight) { min_weight=cur_weight; min_pred=cur_pred; }
      }
      // fprintf(stderr, "cp=%u\n", min_pred);

      out.vi_write((char*)(oo[min_pred]-rlen-1),rlen+1);
      opleft=rlen; h=0; g=0;
      // lines++;
    }
  }
  // fprintf(stderr, "oen=%u opleft=%u lines=%u\n", len, opleft, lines);
}

/* --- */

Encoder* PSEncoder::newPredictor(GenBuffer::Writable &out_, unsigned char type, unsigned char bpc_, slen_t columns_, unsigned char cpp_) {
  switch ((unsigned)type) {
   /* Imp: make these faster with `register int' etc. tricks */
   case 1:  return new Filter::VerbatimE(out_);
   case 2:  return new TIFFPredictor2(out_, bpc_, columns_, cpp_);
   case 10: return new PNGPredictorNone(out_, bpc_, columns_, cpp_);
   case 11: return new PNGPredictorSub(out_, bpc_, columns_, cpp_);
   case 12: return new PNGPredictorUp(out_, bpc_, columns_, cpp_);
   case 13: return new PNGPredictorAverage(out_, bpc_, columns_, cpp_);
   case 14: return new PNGPredictorPaeth(out_, bpc_, columns_, cpp_);
   case 15: return new PNGPredictorAuto(out_, bpc_, columns_, cpp_);
   case 45: return new PNGPredictorAutoUnsigned(out_, bpc_, columns_, cpp_); /* pts' extension */
  }
  // fprintf(stderr, "pred=%d\n", type);
  param_assert(0 && "invalid predictor requested");
  return (Encoder*)0; /*notreached*/
}

/* __END__ */
