/*
 * appliers.cpp -- specific OutputRule implementations
 * by pts@fazekas.hu at Sat Mar 16 14:45:02 CET 2002
 * Blanca JAI additions at Tue May 21 13:18:26 CEST 2002
 * TIFF output at Tue Jun  4 19:50:45 CEST 2002
 */

#ifdef __GNUC__
#pragma implementation
#endif

#include "rule.hpp"
#include "error.hpp"
#include "encoder.hpp"
#include "in_jai.hpp"
#include "crc32.h" /* crc32() used by out_png_work() */
#include <string.h>

/** Appends 4 bytes in MSB first (network) byte order */
static inline void mf32(char *&p, slen_t u32) {
  p[0]=(u32>>24)&255;
  p[1]=(u32>>16)&255;
  p[2]=(u32>> 8)&255;
  p[3]=(u32    )&255;
  p+=4;
}
/** Appends 4 bytes in LSB first (PC) byte order */
static inline void lf32(char *&p, slen_t u32) {
  p[0]=(u32    )&255;
  p[1]=(u32>> 8)&255;
  p[2]=(u32>>16)&255;
  p[3]=(u32>>24)&255;
  p+=4;
}
/** Appends 2 bytes in LSB first (PC) byte order */
static inline void lf16(char *&p, unsigned u16) {
  p[0]=(u16    )&255;
  p[1]=(u16>> 8)&255;
  p+=2;
}


/* --- at Sat Mar 23 15:42:17 CET 2002, removed obsolete
 * PostScript Level2 FlateEncode or LZWEncode filter, predictors supported
 * written at Mar 16 14:48:27 CET 2002
 */

/* --- Sun Mar 17 15:43:20 CET 2002 */

#if 0
/* l1fa85g.tte was a special .tte that is not built in to bts.ttt, but I've
 * integrated it into bts2.ttt at Sun Sep 22 00:48:21 CEST 2002. I've also
 * integrated _l1fa85g_ to _l1c_ that day.
 */
static char *l1fa85g_tte=
#include "l1fa85g.tth"

/** PostScript Level1 FlateEncode, A85, without Predictor */
Rule::Applier::cons_t out_l1fa85g_check_rule(Rule::OutputRule* or_) {
  Rule::Cache *cache=&or_->cache;
  if (!cache->isPS()
   || cache->Compression!=Rule::Cache::CO_ZIP
   || cache->hasPredictor()
   || !cache->isGray()
   || cache->TransferEncoding!=cache->TE_A85
   || (!cache->WarningOK && !cache->isPSL3())
     ) return Rule::Applier::DONT_KNOW;
  return Rule::Applier::OK;
}
Rule::Applier::cons_t out_l1fa85g_work(GenBuffer::Writable& out, Rule::OutputRule*or_, Image::SampledInfo *sf) {
  /* by pts@fazekas.hu at Sun Mar 17 15:52:48 CET 2002 */
  // Imp: two other TransferEncodings [no more, since #if 0]
  if (out_l1fa85g_check_rule(or_)!=Rule::Applier::OK) return Rule::Applier::DONT_KNOW;
  if (!or_->cache.isPSL3()) 
    Error::sev(Error::WARNING_DEFER) << "l1fa85g: /ZIP without /PSL3 will be slow" << (Error*)0;
  or_->doSampleFormat(sf);
  PSEncoder *bp=PSEncoder::newASCII85Encode(out, or_->cacheHints.TransferCPL);
  PSEncoder *cp=PSEncoder::newFlateEncode(*bp, or_->cacheHints.Effort);
  Rule::writeTTE(out, out, *cp, l1fa85g_tte, or_, sf, Rule::writeData);
  delete bp;
  delete cp;
  return Rule::Applier::OK;
}

Rule::Applier out_l1fa85g_applier = { "l1fa85g", out_l1fa85g_check_rule, out_l1fa85g_work, 0 };
#endif

/* l2jbin --- Sun Mar 17 21:45:22 CET 2002
 * p0jbin (integrated to p0jbin) --- Mon Apr 15 23:29:13 CEST 2002
 */

//static char *l2jbin_tte=
//#include "l2jbin.tth"

/** PostScript Level2 DCTEncode (Baseline JPEG), Binary */
Rule::Applier::cons_t out_l2jbin_check_rule(Rule::OutputRule* or_) {
  Rule::Cache *cache=&or_->cache;
  if ((!cache->isPS() && !cache->isPDF())
   || cache->Compression!=Rule::Cache::CO_JAI
     ) return Rule::Applier::DONT_KNOW;
  /* Dat: not unrequired anymore: cache->TransferEncoding!=cache->TE_Binary  */
  bool badp=false;
  if (cache->isPS() && !cache->isPSL2()) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /FileFormat/PSL* /Compression/JAI requires /PSL2+" << (Error*)0;
    badp=true;
  }
  if (cache->SampleFormat!=Image::SF_Asis) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /FileFormat/PSL*|PDF* /Compression/JAI requires /SampleFormat/Asis" << (Error*)0;
    badp=true;
  }
  if (cache->hasPredictor()) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /FileFormat/PSL*|PDF* /Compression/JAI requires /Predictor 1" << (Error*)0;
    badp=true;
  }
  return (Rule::Applier::cons_t)(badp ? 0+Rule::Applier::BAD : 0+Rule::Applier::OK);
  /* ^^^ 0+: pacify g++-3.1 */
}
Rule::Applier::cons_t out_l2jbin_work(GenBuffer::Writable& out, Rule::OutputRule*or_, Image::SampledInfo *sf) {
  char *strings[]={ (char*)NULLP/*LanguageLevel, PDF-1.`0'*/, ""/*colorSpace*/ };
  //, " F closefile T closefile"/*closes*/ };
  //  Error::sev(Error::WARNING_DEFER) << "l2jbin: /ZIP without /PSL3 will be slow" << (Error*)0;
  if (out_l2jbin_check_rule(or_)!=Rule::Applier::OK) return Rule::Applier::DONT_KNOW;
  assert(sf->getImg()->getBpc()==8);
  or_->doSampleFormat(sf);
  // PSEncoder *bp=PSEncoder::newASCII85Encode(out, or_->cacheHints.TransferCPL);
  Rule::Cache *cache=&or_->cache;
  Filter::VerbatimE outve(out); /* required since template /p0jbin is TTM */
  GenBuffer::Writable *op=cache->isPDF() ? &outve : &out, *tp=op;
  strings[0]=const_cast<char*>(cache->isPDF() ? (char const*)"0" : (char const*)"2");
       if (cache->TransferEncoding==cache->TE_A85) tp=PSEncoder::newASCII85Encode (*op, or_->cacheHints.TransferCPL);
  else if (cache->TransferEncoding==cache->TE_Hex) tp=PSEncoder::newASCIIHexEncode(*op, or_->cacheHints.TransferCPL);
  // else strings[2]=" F closefile";
  Rule::writeTTT(*op, *tp, *tp, !cache->isPDF()?"l2jbin":cache->isPDFB()?"p0jbb":"p0jbin", or_, sf, Rule::writePalData, strings);
  if (tp!=op) delete tp;
  /* Dat: outve is deleted by C++ scope */
  // if (tp!=op) delete tp; /* BUGFIX at Thu Jan 20 15:04:47 CET 2005 */
  return Rule::Applier::OK;
}

Rule::Applier out_l2jbin_applier = { "PSL2+PDF-JAI", out_l2jbin_check_rule, out_l2jbin_work, 0 };

/* ---  */

#if 0 /* integrated to _l2jbin_ at Sun Sep 22 14:29:13 CEST 2002 */
//static char *p0jbin_ttm=
//#include "p0jbin.tth"

/** PostScript Level2 DCTEncode (Baseline JPEG), Binary */
Rule::Applier::cons_t out_p0jbin_check_rule(Rule::OutputRule* or_) {
  Rule::Cache *cache=&or_->cache;
  if (!cache->isPDF()
   || cache->Compression!=Rule::Cache::CO_JAI
     ) return Rule::Applier::DONT_KNOW;
  /* Dat: not unrequired anymore: cache->TransferEncoding!=cache->TE_Binary  */
  bool badp=false;
  if (cache->SampleFormat!=Image::SF_Asis) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /FileFormat/PDF* /Compression/JAI requires /SampleFormat/Asis" << (Error*)0;
    badp=true;
  }
  if (cache->hasPredictor()) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /FileFormat/PDF* /Compression/JAI requires /Predictor 1" << (Error*)0;
    badp=true;
  }
  if (badp) return Rule::Applier::BAD;
  return Rule::Applier::OK;
}
Rule::Applier::cons_t out_p0jbin_work(GenBuffer::Writable& out, Rule::OutputRule*or_, Image::SampledInfo *sf) {
  char *strings[]={ "0"/*PDF-1.`0'*/, ""/*colorSpace*/ };
  // " F closefile"/*closes*/ };
  if (out_p0jbin_check_rule(or_)!=Rule::Applier::OK) return Rule::Applier::DONT_KNOW;
  assert(sf->getImg()->getBpc()==8);
  or_->doSampleFormat(sf);
  Rule::Cache *cache=&or_->cache;

  Filter::VerbatimE outve(out); /* required since template /p0jbin is TTM */
  GenBuffer::Writable *tp=&outve;
       if (cache->TransferEncoding==cache->TE_A85) tp=PSEncoder::newASCII85Encode (outve, or_->cacheHints.TransferCPL);
  else if (cache->TransferEncoding==cache->TE_Hex) tp=PSEncoder::newASCIIHexEncode(outve, or_->cacheHints.TransferCPL);
  else strings[2]="";

#if 0 /* old */
  MiniPS::VALUE ttm;
  { Filter::FlatR flatD(p0jbin_ttm);
    MiniPS::Parser p(&flatD);
    ttm=p.parse1();
    if (p.parse1(p.EOF_ALLOWED)!=MiniPS::Qundef || MiniPS::getType(ttm)!=MiniPS::T_ARRAY)
      Error::sev(Error::EERROR) << "TTM: the TTM file should contain a single array" << (Error*)0;
    /* ^^^ Dat: the result of the second p.parse1() doesn't get delete0(...)d */
  }
  Filter::VerbatimE outve(out);
  Rule::writeTTM(outve, outve, outve, MiniPS::RARRAY(ttm), or_, sf, Rule::writePalData, strings);
  MiniPS::delete0(ttm);
#else /* new */
  Rule::writeTTT(outve, *tp, *tp, cache->isPDFB()?"p0jbb":"p0jbin", or_, sf, Rule::writePalData, strings);
#endif
  if (tp!=&outve)delete tp;
  return Rule::Applier::OK;
}

Rule::Applier out_p0jbin_applier = { "PDF-JAI", out_p0jbin_check_rule, out_p0jbin_work, 0 };
#endif

/* --- Fri Mar 22 11:52:53 CET 2002 */
/* PDF added at Sat Apr 20 20:07:34 CEST 2002 */
/* Integrated l23ind1 at Sat Apr 20 20:24:21 CEST 2002 */

//static char *l23_tte=
//#include "l23.tth"

/** PostScript Level2 and PDF generic non-transparent */
Rule::Applier::cons_t out_l23_check_rule(Rule::OutputRule* or_) {
  Rule::Cache *cache=&or_->cache;
  unsigned char sf=cache->SampleFormat;
  if (cache->hasPredictor() && cache->Compression!=Rule::Cache::CO_ZIP && cache->Compression!=Rule::Cache::CO_LZW) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: real /Predictor requires /ZIP or /LZW" << (Error*)0;
    return Rule::Applier::BAD;
  }

  // assert(cache->isPSL2() && (sf==Image::SF_Transparent2 || sf==Image::SF_Transparent4 || sf==Image::SF_Transparent8));

  if (!cache->isPSL2() && !cache->isPDF()
   || (sf!=Image::SF_Gray1    && sf!=Image::SF_Gray2        && sf!=Image::SF_Gray4        && sf!=Image::SF_Gray8
    && sf!=Image::SF_Indexed1 && sf!=Image::SF_Indexed2     && sf!=Image::SF_Indexed4     && sf!=Image::SF_Indexed8
    && sf!=Image::SF_Rgb1     && sf!=Image::SF_Rgb2         && sf!=Image::SF_Rgb4         && sf!=Image::SF_Rgb8
                              && sf!=Image::SF_Transparent2 && sf!=Image::SF_Transparent4 && sf!=Image::SF_Transparent8
    && sf!=Image::SF_Mask)
   || cache->TransferEncoding==cache->TE_ASCII
   || !cache->isZIPOK()
     ) return Rule::Applier::DONT_KNOW;
  #if !HAVE_LZW
    if (cache->Compression==Rule::Cache::CO_LZW) return Rule::Applier::DONT_KNOW;
  #endif

  // if (cache->isDCTE() && !cache->isRGB() && !cache->isGray()) {
  bool badp=false;
  if (cache->isDCTE() && cache->SampleFormat!=Image::SF_Rgb8 && cache->SampleFormat!=Image::SF_Gray8 && cache->SampleFormat!=Image::SF_Indexed8) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /DCTEncode requires /Rgb8 or /Gray8 (or /Indexed8)" << (Error*)0;
    badp=true;
  }
  if (sf==Image::SF_Transparent2 || sf==Image::SF_Transparent4 || sf==Image::SF_Transparent8) {
    if (cache->isPDF()) {
      Error::sev(Error::WARNING_DEFER) << "check_rule: unsupported /Transparent+ for /PDF*" << (Error*)0;
      badp=true;
    } else assert(cache->isPSL2());
  }
  return (Rule::Applier::cons_t)(badp ? 0+Rule::Applier::BAD : 0+Rule::Applier::OK);
}
Rule::Applier::cons_t out_l23_work(GenBuffer::Writable& out, Rule::OutputRule*or_, Image::SampledInfo *sf) {
  Rule::stream_writer_t writeXData=Rule::writePalData;
  char LanguageLevel[2]="2", closes[30];
  SimBuffer::B colorSpace;
  char const*strings[]={ LanguageLevel, (char*)NULLP /*colorSpace*/, closes };
  Rule::Cache *cache=&or_->cache;
  // assert(0);
  if (out_l23_check_rule(or_)!=Rule::Applier::OK) return Rule::Applier::DONT_KNOW;
  or_->doSampleFormat(sf, true);
  if (cache->Compression==Rule::Cache::CO_ZIP) {
    if (!cache->isPDF()) LanguageLevel[0]='3';
  } else if (cache->isPDF()) LanguageLevel[0]='0';
  if (cache->isIndexed() || cache->isTransparentM()) {
    unsigned ncols=PTS_dynamic_cast(Image::Indexed*,sf->getImg())->getNcols();
    colorSpace << "[/Indexed/DeviceRGB " << ncols-1;
    if (ncols==0) { /* Avoid writing zero bytes to the filters */
      /* Imp: verify this in Acrobat Reader */
      colorSpace << "()]";
    } else if (!cache->isPDF()) { /* Insert an in-line hexdump. No better way :-( */
      /*sprintf(colorSpace, "[/Indexed/DeviceRGB %u T %u string readstring pop]", ncols-1, ncols*3); */
      colorSpace << " T " << ncols*3 << " string readstring pop]";
    } else {  /* Insert an in-line hexdump. No shorter way for PDF */
#if 0
      colorSpace="/DeviceGray ";
#else
      /* SUXX: gs5.50 & PDF & /Indexed doesn't work */
      colorSpace << "\n<";
      GenBuffer::Writable *hp=PSEncoder::newASCIIHexEncode(colorSpace, or_->cacheHints.TransferCPL);
      hp->vi_write(sf->getImg()->getHeadp(), ncols*3);
      hp->vi_write(0,0);
      colorSpace << ']'; /* Dat: '>' is appended by ASCIIHexEncode */
#endif
      writeXData=Rule::writeData; /* The palette has already been written. */
    }
  } else if (cache->isGray()) colorSpace="/DeviceGray ";
  else { assert(cache->isRGB()); colorSpace="/DeviceRGB "; }
  if (cache->SampleFormat==Image::SF_Mask || cache->SampleFormat==Image::SF_Indexed1) writeXData=Rule::writeData;

  GenBuffer::Writable *vp=&out;
  if (cache->isPDF()) vp=new Filter::VerbatimE(out); /* required since template /p02* is TTM */

  GenBuffer::Writable *tp=vp;
       if (cache->TransferEncoding==cache->TE_A85) tp=PSEncoder::newASCII85Encode (*vp, or_->cacheHints.TransferCPL);
  else if (cache->TransferEncoding==cache->TE_Hex) tp=PSEncoder::newASCIIHexEncode(*vp, or_->cacheHints.TransferCPL);
  
  GenBuffer::Writable *cp=tp;
  switch (cache->Compression) {
   case Rule::Cache::CO_None: break;
   case Rule::Cache::CO_ZIP: cp=PSEncoder::newFlateEncode(*tp, or_->cacheHints.Effort); break;
   case Rule::Cache::CO_LZW: cp=PSEncoder::newLZWEncode(*tp); break;
   case Rule::Cache::CO_RLE: cp=PSEncoder::newRunLengthEncode(*tp, or_->cacheHints.RecordSize); break;
   case Rule::Cache::CO_Fax: cp=PSEncoder::newCCITTFaxEncode(*tp, or_->cacheHints.K, or_->cacheHints.EncoderBPL, /*EndOfLine:*/ or_->cacheHints.K>0); break;
   /* ^^^ getBpp() BUGFIX at Wed Jul  3 20:00:30 CEST 2002 */
   /* ^^^ EndOfLine BUGFIX at Wed Jul  3 21:12:54 CEST 2002
    * With EndOfLine==false, `sam2p -c:fax:1', acroread triggers the bug.
    * With EndOfLine==false, `sam2p -c:fax:2', acroread and gs trigger the bug.
    */

   case Rule::Cache::CO_IJG: cp=PSEncoder::newDCTIJGEncode(*tp, or_->cacheHints.EncoderColumns, or_->cacheHints.EncoderRows, or_->cacheHints.EncoderColors, or_->cacheHints.Quality); break;
   case Rule::Cache::CO_DCT: { SimBuffer::B other_parameters;
    or_->cacheHints.DCT->dump(other_parameters, 0, false);
    cp=PSEncoder::newDCTEncode(*tp, or_->cacheHints.EncoderColumns, or_->cacheHints.EncoderRows, or_->cacheHints.EncoderColors, or_->cacheHints.ColorTransform, other_parameters);
    break; }
   default: assert(0);
  }

  GenBuffer::Writable *pp=cp;
  if (cache->hasPredictor()) pp=PSEncoder::newPredictor(*cp, cache->Predictor, or_->cacheHints.PredictorBPC, or_->cacheHints.PredictorColumns, or_->cacheHints.PredictorColors);

  #if 0 /* Sun Sep 22 20:40:51 CEST 2002 */
    if (cp!=tp) strcpy(closes," F closefile"); else closes[0]='\0';
    if (tp!=vp) strcpy(closes+strlen(closes)," T closefile"); 
  #endif

  strings[1]=colorSpace.term0()();
  Rule::writeTTT(*vp, *tp, *pp,
    !cache->isPDF() ? (
      cache->SampleFormat==Image::SF_Indexed1 ? "l23ind1" :
      cache->SampleFormat==Image::SF_Mask ? "l23mask" :
      cache->isTransparentM() ? "l23tran2" :
      "l23"
    ) : cache->isPDFB() ? (
      cache->SampleFormat==Image::SF_Indexed1 ? "p02ind1bb" :
      cache->SampleFormat==Image::SF_Mask ? "p02maskbb" :
      "p02bb"
    ) : (
      cache->SampleFormat==Image::SF_Indexed1 ? "p02ind1" :
      cache->SampleFormat==Image::SF_Mask ? "p02mask" :
      "p02"
    ), or_, sf, writeXData, strings);
  if (pp!=cp)  delete pp;
  if (cp!=tp)  delete cp;
  if (tp!=vp)  delete tp;
  if (vp!=&out)delete vp;
  return Rule::Applier::OK;
}

Rule::Applier out_l23_applier = { "PSL23+PDF", out_l23_check_rule, out_l23_work, 0 };

/* --- Fri Mar 22 17:22:40 CET 2002 -- Sat Jun 15 16:03:06 CEST 2002 */

/* integrated l1tr at Sat Jun 15 16:03:03 CEST 2002 */
/* added /Bbox at Sat Jun 15 16:03:31 CEST 2002 */

//static char *l1tr_tte=
//#include "l1tr.tth"

/** PostScript Level1 or PDF Fully transparent image */
Rule::Applier::cons_t out_l1tr_check_rule(Rule::OutputRule* or_) {
  Rule::Cache *cache=&or_->cache;
  if (!(cache->isPS() || cache->isPDF())
   || (cache->SampleFormat!=Image::SF_Transparent && cache->SampleFormat!=Image::SF_Bbox && cache->SampleFormat!=Image::SF_Opaque)
     ) return Rule::Applier::DONT_KNOW;
  return Rule::Applier::OK;
}
Rule::Applier::cons_t out_l1tr_work(GenBuffer::Writable& out, Rule::OutputRule*or_, Image::SampledInfo *sf) {
  char t[]="....";
  if (out_l1tr_check_rule(or_)!=Rule::Applier::OK) return Rule::Applier::DONT_KNOW;
  or_->doSampleFormat(sf);
  Rule::Cache *cache=&or_->cache;
  Filter::VerbatimE outve(out); /* required since template /p0* is TTM */
  if (cache->isPS()) { t[0]='l'; t[1]='1'; }
                else { t[0]='p'; t[1]='0'; }
  switch (cache->SampleFormat) {
    case Image::SF_Transparent: t[2]='t'; t[3]='r'; break;
    case Image::SF_Opaque:      t[2]='o'; t[3]='p'; break;
    case Image::SF_Bbox:        t[2]='b'; t[3]='b'; break;
    default: assert(0);
  }
  Rule::writeTTT(outve, outve, outve, t, or_, sf, 0/*NULLP*/);
  return Rule::Applier::OK;
}

Rule::Applier out_l1tr_applier = { "P-TrOpBb", out_l1tr_check_rule, out_l1tr_work, 0 };

/* --- */

/* --- l23mask Fri Mar 22 18:11:12 CET 2002
 * --- l23ind1 Fri Mar 22 18:11:12 CET 2002
 * removed (integrated to l23) at Sat Apr 20 20:25:57 CEST 2002
 */

/* --- l1mask Fri Mar 22 18:33:01 CET 2002
 * --- l1mashex Sun Apr 14 15:25:25 CEST 2002
 * --- l1in1 Fri Mar 22 18:33:37 CET 2002
 * --- l1in1hex Fri Mar 22 18:33:37 CET 2002
 * removed (integrated to l1c) at Sat Apr 20 20:25:57 CEST 2002
 * --- lcr Sat Jun  1 17:09:57 CEST 2002
 * removed (integrated to l1c) at Sun Jun  2 16:48:31 CEST 2002
 * --- l1gbh
 * removed (integrated to l1c) at Sun Jun  2 16:48:31 CEST 2002
 * --- l1fa85g.tte: PostScript Level1 FlateEncode, A85, without Predictor
 * removed (integrated to l1c) at Sun Sep 22 00:48:21 CEST 2002
 */

/* --- Sun Jun  2 16:48:44 CEST 2002 */

//static char *l1mask_tte=
//#include "l1mask.tth"

static void gen_tkey(char *tkey, GenBuffer::Writable& out, GenBuffer::Writable*&tp, GenBuffer::Writable*& cp, Rule::OutputRule*or_) {
  Rule::Cache *cache=&or_->cache;
  tp=&out;
       if (cache->TransferEncoding==cache->TE_A85) { tkey[3]='8'; tp=PSEncoder::newASCII85Encode (out, or_->cacheHints.TransferCPL); }
  else if (cache->TransferEncoding==cache->TE_Hex) { tkey[3]='h'; tp=PSEncoder::newASCIIHexEncode(out, or_->cacheHints.TransferCPL); }
  else tkey[3]='b';
  cp=tp;
       if (cache->Compression==Rule::Cache::CO_RLE) { tkey[4]='r'; cp=PSEncoder::newRunLengthEncode(*tp, or_->cacheHints.RecordSize); }
  else if (cache->Compression==Rule::Cache::CO_ZIP) { tkey[4]='z'; cp=PSEncoder::newFlateEncode(*tp, or_->cacheHints.Effort); }
  else if (cache->Compression==Rule::Cache::CO_LZW) { tkey[4]='l'; cp=PSEncoder::newLZWEncode(*tp); }
  else tkey[4]='n';
  /* vvv removed 'm' and '1' at Sun Sep 22 17:53:08 CEST 2002 */
  tkey[2]=// cache->SampleFormat==Image::SF_Mask     ? 'm' :
          // cache->SampleFormat==Image::SF_Indexed1 ? '1' :
             cache->SampleFormat==Image::SF_Indexed2 ? '2' :
             cache->SampleFormat==Image::SF_Indexed4 ? '4' :
             cache->SampleFormat==Image::SF_Indexed8 ? '8' :
             cache->SampleFormat==Image::SF_Transparent2 ? 't' :
             cache->SampleFormat==Image::SF_Transparent4 ? 't' :
             cache->SampleFormat==Image::SF_Transparent8 ? 't' :
             'g'; /* /Gray*, /Rgb*, /Mask, /Indexed1 */
}

/** PostScript Level1 uncompressed binary or hex */
Rule::Applier::cons_t out_l1c_check_rule(Rule::OutputRule* or_) {
  Rule::Cache *cache=&or_->cache;
  if ((cache->FileFormat!=cache->FF_PSL1 && cache->FileFormat!=cache->FF_PSLC && cache->FileFormat!=cache->FF_PSL2)
   || (cache->FileFormat==cache->FF_PSL2 && cache->Compression!=Rule::Cache::CO_ZIP)
   || (cache->TransferEncoding!=cache->TE_Binary && cache->TransferEncoding!=cache->TE_Hex && cache->TransferEncoding!=cache->TE_A85)
   || (cache->Compression!=Rule::Cache::CO_None && cache->Compression!=Rule::Cache::CO_RLE && cache->Compression!=Rule::Cache::CO_ZIP && cache->Compression!=Rule::Cache::CO_LZW)
   || cache->hasPredictor()
   || !(cache->isTransparentM() || cache->isIndexed() || cache->isGray() || cache->isRGB())
     ) return Rule::Applier::DONT_KNOW;
  bool badp=false;
  if (cache->FileFormat==cache->FF_PSL1 && cache->SampleFormat!=Image::SF_Indexed1 && cache->isIndexed()) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /SampleFormat/Indexed+ doesn't work with /FileFormat/PSL1 (use /PSLC or /PSL2)" << (Error*)0;
    badp=true;
  }
  if (cache->FileFormat==cache->FF_PSL1 && cache->isRGB()) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /SampleFormat/RGB* doesn't work with /FileFormat/PSL1 (use /PSLC or /PSL2)" << (Error*)0;
    badp=true;
  }
  char tkey[]="l1..."; /* /l1{2,4,8,t}{8,h}{r,z,l} */
  GenBuffer::Writable *tp0,*cp0,*out=(GenBuffer::Writable*)NULLP;
  gen_tkey(tkey, *out, tp0, cp0, or_);
  /* fprintf(stderr,"tkey=%s\n", tkey); */
  if (!badp && Rule::Templates->get(tkey, strlen(tkey))==MiniPS::Qundef) return Rule::Applier::DONT_KNOW;
  return (Rule::Applier::cons_t)(badp ? 0+Rule::Applier::BAD : 0+Rule::Applier::OK);
  /* ^^^ Dat: 0+: pacify gcc-3.1 */
}
Rule::Applier::cons_t out_l1c_work(GenBuffer::Writable& out, Rule::OutputRule*or_, Image::SampledInfo *sf) {
  if (out_l1c_check_rule(or_)!=Rule::Applier::OK) return Rule::Applier::DONT_KNOW;
  /* Dat: only these have been defined so far:  grep '^/l1[248tg][8h][rzl]' bts2.ttt
   * /l1thr /l1g8r /l1ghr /l128r /l12hr /l148r /l14hr /l188r /l18hr /l1g8z /l1ghz /l1g8l /l1ghl
   */
  or_->doSampleFormat(sf, true); /* Dat: `true': because of /Transparent+ */
  char tkey[]="l1..."; /* /l1{2,4,8,t}{8,h}{r,z,l} */
  GenBuffer::Writable *tp, *cp;
  gen_tkey(tkey, out, tp, cp, or_);
  // fprintf(stderr, "tkey=(%s)\n", tkey);
  Rule::writeTTT(out, *tp, *cp, tkey, or_, sf,
    tkey[2]=='2' || tkey[2]=='4' || tkey[2]=='8' || tkey[2]=='t' ? Rule::writePalData : Rule::writeData
  );
  // Rule::writeTTT(out, *tp, *cp, tkey, or_, sf, Rule::writePalData);
  if (cp!=tp)  delete cp;
  if (tp!=&out)delete tp;
  return Rule::Applier::OK;
}

Rule::Applier out_l1c_applier = { "PSL1C", out_l1c_check_rule, out_l1c_work, 0 };

/* lcrbin (lcrb) --- Sun Apr 14 16:50:14 CEST 2002
 * lcrhex (lcr8, lcrh) --- Sun Apr 14 16:50:22 CEST 2002
 * removed (integrated to _l1c_) at Sat Jun  1 17:09:52 CEST 2002
 */

#if 0
/* --- Sat Jun  1 17:09:57 CEST 2002 */

//static char *lcrbin_tte=
//#include "lcrbin.tth"

/** PostScript Level1+C uncompressed RGB image */
Rule::Applier::cons_t out_lcr_check_rule(Rule::OutputRule* or_) {
  Rule::Cache *cache=&or_->cache;
  if (!cache->isPS()
   || cache->FileFormat==cache->FF_PSL1
   || (cache->TransferEncoding!=cache->TE_Binary && cache->TransferEncoding!=cache->TE_Hex && cache->TransferEncoding!=cache->TE_A85)
   || cache->Compression!=Rule::Cache::CO_None
   || cache->hasPredictor()
   || !cache->isRGB()
     ) return Rule::Applier::DONT_KNOW;
  return Rule::Applier::OK;
}
Rule::Applier::cons_t out_lcr_work(GenBuffer::Writable& out, Rule::OutputRule*or_, Image::SampledInfo *sf) {
  Rule::Cache *cache=&or_->cache;
  if (out_lcr_check_rule(or_)!=Rule::Applier::OK) return Rule::Applier::DONT_KNOW;
  or_->doSampleFormat(sf);
  char tkey[]="lcr.";
  GenBuffer::Writable *tp=&out;
       if (cache->TransferEncoding==cache->TE_A85) { tkey[3]='8'; tp=PSEncoder::newASCII85Encode (out, or_->cacheHints.TransferCPL); }
  else if (cache->TransferEncoding==cache->TE_Hex) { tkey[3]='h'; tp=PSEncoder::newASCIIHexEncode(out, or_->cacheHints.TransferCPL); }
  else tkey[3]='b';
  Rule::writeTTT(out, *tp, *tp, tkey, or_, sf, Rule::writeData);
  if (tp!=&out)delete tp;
  return Rule::Applier::OK;
}

Rule::Applier out_lcr_applier = { "PSLC-RGB", out_lcr_check_rule, out_lcr_work, 0 };
#endif

/* --- Sat Mar 23 12:37:04 CET 2002; Tue Jul  2 10:23:44 CEST 2002 */

Rule::Applier::cons_t out_gif89a_check_rule(Rule::OutputRule* or_) {
  Rule::Cache *cache=&or_->cache;
  bool badp=false;
  if (cache->FileFormat!=cache->FF_GIF89a
     ) return Rule::Applier::DONT_KNOW;
  if (!cache->isIndexed() && !cache->isTransparentM()) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /GIF89a must be /Indexed*, /Mask or /Transparent+" << (Error*)0;
    badp=true;
  }
  if (cache->TransferEncoding!=cache->TE_Binary) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /GIF89a requires /Binary" << (Error*)0;
    badp=true;
  }
  if (cache->Compression!=Rule::Cache::CO_None && cache->Compression!=Rule::Cache::CO_LZW) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /GIF89a requires /LZW" << (Error*)0;
    badp=true;
  }
  if (cache->hasPredictor()) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /GIF89a requires /Predictor 1" << (Error*)0;
    badp=true;
  }
  #if !USE_OUT_GIF
    Error::sev(Error::WARNING_DEFER) << "check_rule: please `configure --enable-gif' for /GIF89a" << (Error*)0;
    badp=true;
  #endif
  if (badp) return Rule::Applier::BAD;
  or_->cache.WarningOK=true;
  return Rule::Applier::OK;
}
#if USE_OUT_GIF
  #if OBJDEP
  #  warning REQUIRES: out_gif.o
  #endif
  extern void out_gif_write(GenBuffer::Writable& out, Image::Indexed *img); /* out_gif.cpp */
  Rule::Applier::cons_t out_gif89a_work(GenBuffer::Writable& out, Rule::OutputRule*or_, Image::SampledInfo *sf) {
    if (out_gif89a_check_rule(or_)!=Rule::Applier::OK) return Rule::Applier::DONT_KNOW;
    or_->cache.SampleFormat=(Image::sf_t)(or_->cache.isIndexed()? 0+Image::SF_Indexed8: 0+Image::SF_Transparent8);
    /* ^^^ Dat: 0+: pacify gcc-3.1; (Image::sf_t): pacify VC6.0 */
    or_->doSampleFormat(sf);
    out_gif_write(out, PTS_dynamic_cast(Image::Indexed*,sf->getImg()));
    return Rule::Applier::OK;
  }
#else
/*#  define out_gif89a_check_rule (Rule::Applier::check_rule_t)NULLP*/
#  define out_gif89a_work       (Rule::Applier::work_t)      NULLP
#endif

Rule::Applier out_gif89a_applier = {
#if HAVE_LZW
  "GIF89a+LZW"
#else
  "GIF89a"
#endif
  , out_gif89a_check_rule, out_gif89a_work, 0 };

/* --- Tue Jun  4 19:51:03 CEST 2002 */

Rule::Applier::cons_t out_xpm_check_rule(Rule::OutputRule* or_) {
  Rule::Cache *cache=&or_->cache;
  bool badp=false;
  if (cache->FileFormat!=cache->FF_XPM
     ) return Rule::Applier::DONT_KNOW;
  if (!cache->isIndexed() && !cache->isTransparentM()) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /XPM must be /Indexed*, /Mask or /Transparent+" << (Error*)0;
    badp=true;
  }
  if (cache->TransferEncoding!=cache->TE_ASCII && cache->TransferEncoding!=cache->TE_Binary) {
    /* ^^^ && BUGFIX at Thu Jul 11 21:57:09 CEST 2002 */
    Error::sev(Error::WARNING_DEFER) << "check_rule: /XPM requires /TransferEncoding/ASCII" << (Error*)0;
    badp=true;
  }
  if (cache->Compression!=Rule::Cache::CO_None) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /XPM requires /Compression/None" << (Error*)0;
    badp=true;
  }
  if (cache->hasPredictor()) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /XPM requires /Predictor 1" << (Error*)0;
    badp=true;
  }
  if (badp) return Rule::Applier::BAD;
  or_->cache.WarningOK=true;
  return Rule::Applier::OK;
}
Rule::Applier::cons_t out_xpm_work(GenBuffer::Writable& out, Rule::OutputRule*or_, Image::SampledInfo *sf) {
  if (out_xpm_check_rule(or_)!=Rule::Applier::OK) return Rule::Applier::DONT_KNOW;
  or_->cache.SampleFormat=(Image::sf_t)(or_->cache.isIndexed()? 0+Image::SF_Indexed8: 0+Image::SF_Transparent8);
  /* ^^^ force 8-bit; may trigger warnings... */
  /* ^^^ Dat: 0+: pacify gcc-3.1 */
  or_->doSampleFormat(sf);
  Image::Indexed *iimg=PTS_dynamic_cast(Image::Indexed*,sf->getImg());
  /* vvv 93 useful ASCII chars (+ '\0'), missing: " and \ ; plus a hextable */
  static char const xpmc=93;
  static char const xpms[xpmc+1]="0123456789abcdef !#$%&'()*+,-./:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`ghijklmnopqrstuvwxyz{|}~";

  Image::Sampled::dimen_t wd=iimg->getWd(), htc=iimg->getHt();
  bool pal2ch=iimg->getNcols()>xpmc;
  // pal2ch=true;
  out << "/* XPM */\nstatic char *sam2p_xpm[] = {\n"
         "/* columns rows colors chars-per-pixel */\n\""
      << wd << ' ' << htc << ' ' << iimg->getNcols()
      << ' ' << (pal2ch?2:1) << "\",\n";
  char const *p=iimg->getHeadp(), *pend=iimg->getRowbeg(), *phend;
  char coline3[]="\".. c #ABCDEF\",\n";
  char cotran3[]="\".. c None s None \",\n";
  assert((pend-p)%3==0);
  unsigned i=0;
  bool transp=false;
  if (iimg->getTransp()>=0) { pend-=3; assert((iimg->getTransp())*3+p==pend); transp=true; }
  if (pal2ch) {
    while (p!=pend) {
      coline3[ 1]=xpms[i>>2];
      coline3[ 2]=xpms[i++&3];
      coline3[ 7]=xpms[*(unsigned char const*)p>>4];
      coline3[ 8]=xpms[*(unsigned char const*)p++&15];
      coline3[ 9]=xpms[*(unsigned char const*)p>>4];
      coline3[10]=xpms[*(unsigned char const*)p++&15];
      coline3[11]=xpms[*(unsigned char const*)p>>4];
      coline3[12]=xpms[*(unsigned char const*)p++&15];
      out << coline3;
    }
    if (transp) {
      cotran3[ 1]=xpms[i>>2];
      cotran3[ 2]=xpms[i++&3];
      out << cotran3;
      p+=3;
    }
    pend=iimg->getRowbeg()+wd*htc;
    out << "/* Pixels */\n";
    char *obuf=new char[4+2*wd], *op;
    obuf[0]='"';
    obuf[2*wd+1]='"';
    obuf[2*wd+2]=','; /* Dat: it's OK to have a comma in the last line */
    obuf[2*wd+3]='\n';
    while (htc--!=0) {
      phend=p+wd;
      op=obuf;
      while (p!=phend) { *++op=xpms[*p>>2]; *++op=xpms[*p++&3]; }
      out.vi_write(obuf, 2*wd+4);
    }
  } else {
    coline3[1]='"';
    while (p!=pend) {
      coline3[ 2]=xpms[i++];
      coline3[ 7]=xpms[*(unsigned char const*)p>>4];
      coline3[ 8]=xpms[*(unsigned char const*)p++&15];
      coline3[ 9]=xpms[*(unsigned char const*)p>>4];
      coline3[10]=xpms[*(unsigned char const*)p++&15];
      coline3[11]=xpms[*(unsigned char const*)p>>4];
      coline3[12]=xpms[*(unsigned char const*)p++&15];
      out << (coline3+1);
    }
    if (transp) {
      cotran3[1]='"';
      cotran3[ 2]=xpms[i];
      out << (cotran3+1);
      p+=3;
    }
    pend=iimg->getRowbeg()+wd*htc;
    out << "/* Pixels */\n";
    char *obuf=new char[4+wd], *op;
    obuf[0]='"';
    obuf[wd+1]='"';
    obuf[wd+2]=','; /* Dat: it's OK to have a comma in the last line */
    obuf[wd+3]='\n';
    while (htc--!=0) {
      phend=p+wd;
      op=obuf;
      while (p!=phend) *++op=xpms[0U+*p++];
      out.vi_write(obuf, wd+4);
    }
  }
  assert(p==pend);
  out << "};\n";
  return Rule::Applier::OK;
}

Rule::Applier out_xpm_applier = { "XPM", out_xpm_check_rule, out_xpm_work, 0 };

/* --- Sat Mar 23 13:18:07 CET 2002 */

Rule::Applier::cons_t out_pnm_check_rule(Rule::OutputRule* or_) {
  Rule::Cache *cache=&or_->cache;
  if (cache->FileFormat!=cache->FF_PNM
     ) return Rule::Applier::DONT_KNOW;
  bool badp=false;
  // if (cache->SampleFormat!=Image::SF_Rgb8 && cache->SampleFormat!=Image::SF_Gray8 && cache->SampleFormat!=Image::SF_Gray1) {
  if (!cache->isGray() && !cache->isRGB() && !cache->isIndexed() && !cache->isTransparentM()) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /PNM must be /Rgb8, /Gray8 or /Gray1" << (Error*)0;
    badp=true;
  }
  if (cache->TransferEncoding!=cache->TE_Binary && cache->TransferEncoding!=cache->TE_ASCII) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /PNM requires /Binary or /ASCII" << (Error*)0;
    badp=true;
  }
  if (cache->Compression!=Rule::Cache::CO_None) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /PNM requires /Compression/None" << (Error*)0;
    badp=true;
  }
  if (cache->hasPredictor()) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /PNM requires /Predictor 1" << (Error*)0;
    badp=true;
  }
  if (badp) return Rule::Applier::BAD;
  or_->cache.WarningOK=true;
  return Rule::Applier::OK;
}
Rule::Applier::cons_t out_pnm_work(GenBuffer::Writable& out, Rule::OutputRule*or_, Image::SampledInfo *sf) {
  /*static*/ char head[]={ 'P', 0, '\n', '#', ' ', 'b', 'y', ' ' };
  /*static*/ char tmp[72];
  Image::sf_t sfo=or_->cache.SampleFormat;
  Image::Indexed *alphaChannel=(Image::Indexed*)NULLP;
  if (out_pnm_check_rule(or_)!=Rule::Applier::OK) return Rule::Applier::DONT_KNOW;
  if (or_->cache.SampleFormat==Image::SF_Gray1 || or_->cache.SampleFormat==Image::SF_Gray8) {
    if (or_->cache.TransferEncoding==or_->cache.TE_ASCII) or_->cache.SampleFormat=Image::SF_Gray8;
  } else if (or_->cache.SampleFormat==Image::SF_Gray2 || or_->cache.SampleFormat==Image::SF_Gray4) {
    or_->cache.SampleFormat=Image::SF_Gray8;
  } else if (or_->cache.isRGB()) {
    or_->cache.SampleFormat=Image::SF_Rgb8;
  } else {
    assert(or_->cache.isIndexed() || or_->cache.isTransparentM());
    if (or_->cache.isTransparentM()) {
      alphaChannel=PTS_dynamic_cast(Image::Indexed*,sf->getImg())->calcAlpha();
      sf->clearTransp(); /* BUGFIX at Tue Sep 17 10:05:47 CEST 2002 */
    }
    or_->cache.SampleFormat=(Image::sf_t)(
                            !sf->canGrayy() ? 0+Image::SF_Rgb8
                          : sf->minRGBBpcc()==1 ? 0+Image::SF_Gray1 : 0+Image::SF_Gray8);
    /* ^^^ Dat: 0+: pacify gcc-3.1 */
  }
  if (or_->cache.SampleFormat==Image::SF_Gray1 && or_->cache.TransferEncoding==or_->cache.TE_ASCII)
    or_->cache.SampleFormat=Image::SF_Gray8;
  or_->doSampleFormat(sf);
  sfo=or_->cache.SampleFormat;
  head[1]=(sfo==Image::SF_Rgb8 ? '3' : sfo==Image::SF_Gray8 ? '2' : '1')
          +(or_->cache.TransferEncoding==or_->cache.TE_Binary)*3;
  out.vi_write(head, sizeof(head));
  out << Error::banner0;
  Image::Sampled *img=sf->getImg();
  out << '\n' << img->getWd() << ' ' << img->getHt();
  out << " 255\n"+(sfo==Image::SF_Gray1?4:0);
  /* ^^^ SF_Gray1 BUGFIX at Tue Jun  4 21:44:17 CEST 2002 */
  register char *p=img->getRowbeg(), *t=(char*)NULLP;
  slen_t len=img->getRlen()*img->getHt();
  // fprintf(stderr, "len=%u\n", len);
  register unsigned smallen, i;
  switch (head[1]) {
   case '1': /* PBM ASCII */
    while (len>=70) {
      smallen=70; t=tmp; while (smallen--!=0) *t++=(*p++==0)?'1':'0';
      /* ^^^ note the swapping of '0' and '1' above */
      *t++='\n'; out.vi_write(tmp, 71);
      len-=70;
    }
    while (len--!=0) *t++=(*p++==0)?'0':'1';
    /* Dat: xv requires a whitespace just before EOF */
    *t++='\n'; out.vi_write(tmp, t-tmp);
    break;
   case '2': case '3': /* PGM ASCII, PPM ASCII */
    while (len!=0) {
      if (len>17) { smallen=17; len-=17; } else { smallen=len; len=0; }
      t=tmp; while (smallen--!=0) {
        if ((i=*(unsigned char*)p++)<10) *t++=i+'0';
        else if (i<100) {           *t++=i/10+'0'; *t++=i%10+'0'; }
        else if (i<200) { *t++='1'; *t++=(i-100)/10+'0'; *t++=i%10+'0'; }
                   else { *t++='2'; *t++=(i-200)/10+'0'; *t++=i%10+'0'; }
        *t++=' ';
      }
      /* Dat: xv requires a whitespace just before EOF */
      t[-1]='\n'; out.vi_write(tmp, t-tmp/*-(len==0)*/);
    }
    break;
   case '4': /* PBM RAWBITS */
    /* Invert the image */
    while (len--!=0) *p++^=-1;
    p=img->getRowbeg();
    len=img->getRlen()*img->getHt();
    /* fall through */
   default: /* PBM RAWBITS, PGM RAWBITS, PPM RAWBITS */
    /* fwrite(p, 1, len, stdout); */
    out.vi_write(p, len);
  }
  if (alphaChannel!=NULLP) {
    /* OK: don't always output rawbits PBM file */
    assert(alphaChannel->getBpc()==1);
    assert(alphaChannel->getWd()==img->getWd());
    assert(alphaChannel->getHt()==img->getHt());
    /* write PBM RAWBITS subfile (alpha channel) */
    if (or_->cache.TransferEncoding==or_->cache.TE_Binary) {
      out << "P4 " << img->getWd() << ' ' << img->getHt() << '\n';
      out.vi_write(alphaChannel->getRowbeg(), alphaChannel->getRlen()*alphaChannel->getHt());
      /* ^^^ BUGFIX at Tue Sep 17 10:18:28 CEST 2002 */
    } else {
      out << "P1 " << img->getWd() << ' ' << img->getHt() << '\n';
      alphaChannel->to8();
      p=alphaChannel->getRowbeg(); len=alphaChannel->getRlen()*alphaChannel->getHt();
      while (len>=70) {
        smallen=70; t=tmp; while (smallen--!=0) *t++=(*p++!=0)?'1':'0';
        /* ^^^ note the non-swapping of '0' and '1' above */
        *t++='\n'; out.vi_write(tmp, 71);
        len-=70;
      }
      while (len--!=0) *t++=(*p++!=0)?'0':'1';
      /* Dat: xv requires a whitespace just before EOF */
      *t++='\n'; out.vi_write(tmp, t-tmp);
    }
    delete alphaChannel;
  }
  return Rule::Applier::OK;
}

Rule::Applier out_pnm_applier = { "PNM", out_pnm_check_rule, out_pnm_work, 0 };

/* --- Sat Aug 10 22:18:33 CEST 2002 */

Rule::Applier::cons_t out_xwd_check_rule(Rule::OutputRule* or_) {
  Rule::Cache *cache=&or_->cache;
  if (cache->FileFormat!=cache->FF_XWD
     ) return Rule::Applier::DONT_KNOW;
  bool badp=false;
  if (cache->SampleFormat!=Image::SF_Rgb8 && cache->SampleFormat!=Image::SF_Gray8 && cache->SampleFormat!=Image::SF_Indexed8) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /XWD must be /Rgb8, /Gray8 or /Indexed8" << (Error*)0;
    return Rule::Applier::BAD;
  }
  if (cache->TransferEncoding!=cache->TE_Binary) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /XWD requires /Binary" << (Error*)0;
    badp=true;
  }
  if (cache->Compression!=Rule::Cache::CO_None) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /XWD requires /Compression/None" << (Error*)0;
    badp=true;
  }
  if (cache->hasPredictor()) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /XWD requires /Predictor 1" << (Error*)0;
    badp=true;
  }
  if (badp) return Rule::Applier::BAD;
  return Rule::Applier::OK;
}
Rule::Applier::cons_t out_xwd_work(GenBuffer::Writable& out, Rule::OutputRule*or_, Image::SampledInfo *sf) {
  static const unsigned
    XWD_FILE_VERSION=7,
    ZPixmap=2,
    MSBFirst=1,
    DirectColor=5,
    PseudoColor=3,
    //GrayScale=1,
    StaticGray=0;
  Image::sf_t sfo=or_->cache.SampleFormat;
  if (out_xwd_check_rule(or_)!=Rule::Applier::OK) return Rule::Applier::DONT_KNOW;
  or_->doSampleFormat(sf);
  sfo=or_->cache.SampleFormat;
  char head[101], *p=head;
  Image::Sampled *img=sf->getImg();
  slen_t bits_per_pixel, bitmap_pad, bytes_per_line;
  unsigned ncolors;

  memset(head, '\0', 101);
  /*header_size*/ mf32(p,101);
  /*file_version*/ (p+=4)[-1]=XWD_FILE_VERSION;
  /*pixmap_format*/ (p+=4)[-1]=ZPixmap;
  /*pixmap_depth*/ (p+=4)[-1]=sfo==Image::SF_Rgb8 ? 24 : 8;
  /*pixmap_width */ mf32(p, img->getWd());
  /*pixmap_height*/ mf32(p, img->getHt());
  /*xoffset*/ p+=4;
  /*byte_order*/ (p+=4)[-1]=MSBFirst;
  /*bitmap_unit*/ (p+=4)[-1]=8; // sfo==Image::SF_Rgb8 ? 32 : 8;
  /*bitmap_bit_order*/ (p+=4)[-1]=MSBFirst;
  /*bitmap_pad*/ bitmap_pad=(p+=4)[-1]=8; // sfo==Image::SF_Rgb8 ? 32 : 8;
  /* ^^^ force no padding at all */
  /*bits_per_pixel*/ bits_per_pixel=(p+=4)[-1]=sfo==Image::SF_Rgb8 ? 24 : 8;
  /*bytes_per_line*/ mf32(p, bytes_per_line=((bits_per_pixel*img->getWd()+bitmap_pad-1)&~(bitmap_pad-1))>>3);
  /*visual_class*/ (p+=4)[-1]=sfo==Image::SF_Rgb8 ? DirectColor : sfo==Image::SF_Indexed8 ? PseudoColor : StaticGray;
  /*red_mask  */ (p+=4)[-3]=(char)(sfo==Image::SF_Rgb8 ? 255 : 0);
  /*green_mask*/ (p+=4)[-2]=(char)(sfo==Image::SF_Rgb8 ? 255 : 0);
  /*blue_mask */ (p+=4)[-1]=(char)(sfo==Image::SF_Rgb8 ? 255 : 0); 
  /*bits_per_rgb*/ (p+=4)[-1]=sfo==Image::SF_Rgb8 ? 24 : 8;
  /*colormap_entries*/ (p+=4)[-2]=1; /*256*/
  /*ncolors*/ mf32(p, ncolors=sfo==Image::SF_Rgb8 ? 0 : sfo==Image::SF_Indexed8 ? ((Image::Indexed*)img)->getNcols() : 256);
  /*window_width */ mf32(p, img->getWd());
  /*window_height*/ mf32(p, img->getHt());
  assert(p+13==head+101);
  /*window_x*/ /*0*/
  /*window_y*/ /*0*/
  /*window_bdrwidth*/ /*0*/
  /*filename*/ /*""*/
  out.vi_write(head, 101);

  if (sfo!=Image::SF_Rgb8) {
    char *pal=new char[ncolors*12], *pp=pal;
    unsigned pixel;
    if (sfo==Image::SF_Indexed8) {
      char const* q=img->getHeadp();
      for (pixel=0;pixel<ncolors;pixel++) {
        *pp++=0; *pp++=0; *pp++=0; *pp++=pixel;
        *pp++=*q; *pp++=*q++; /* red */
        *pp++=*q; *pp++=*q++; /* green */
        *pp++=*q; *pp++=*q++; /* blue */
        *pp++=7;
        *pp++=0;
      }
    } else { assert(sfo==Image::SF_Gray8);
      for (pixel=0;pixel<ncolors;pixel++) {
        *pp++=0; *pp++=0; *pp++=0; *pp++=pixel;
        *pp++=pixel; *pp++=pixel; /* red */
        *pp++=pixel; *pp++=pixel; /* green */
        *pp++=pixel; *pp++=pixel; /* blue */
        *pp++=7;
        *pp++=0;
      }
    }
    out.vi_write(pal, ncolors*12);
    delete [] pal;
  }

  slen_t rlen=img->getRlen();
  Image::Sampled::dimen_t htc=img->getHt();
  char const* rp=img->getRowbeg();
  unsigned scanline_pad=bytes_per_line-rlen;
  // assert(*rp=='\xff');
  if (scanline_pad!=0) {
    assert(1<=scanline_pad && scanline_pad<=3);
    while (htc--!=0) {
      out.vi_write(rp, rlen);
      rp+=rlen;
      if (scanline_pad!=0) out.vi_write("\0\0", scanline_pad);
    }
  } else out.vi_write(rp, rlen*htc);

  return Rule::Applier::OK;
}

Rule::Applier out_xwd_applier = { "XWD", out_xwd_check_rule, out_xwd_work, 0 };

Rule::Applier out_x11_applier = { "X11", 0/*out_x11_check_rule*/, 0/*out_x11_work*/, 0 };

/* --- Sat Apr 20 11:49:56 CEST 2002 */

/** Baseline (lossy) JPEG */
Rule::Applier::cons_t out_jpeg_check_rule(Rule::OutputRule* or_) {
  Rule::Cache *cache=&or_->cache;
  if (cache->FileFormat!=cache->FF_JPEG
   || cache->Compression==Rule::Cache::CO_JAI
     ) return Rule::Applier::DONT_KNOW;
  bool badp=false;
  if (cache->SampleFormat!=Image::SF_Rgb8 && cache->SampleFormat!=Image::SF_Gray8) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /DCTEncode requires /Rgb8 or /Gray8" << (Error*)0;
    badp=true;
  }
  if (cache->TransferEncoding!=cache->TE_Binary) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /JPEG requires /Binary" << (Error*)0;
    badp=true;
  }
  if (cache->Compression!=Rule::Cache::CO_None && cache->Compression!=Rule::Cache::CO_DCT && cache->Compression!=Rule::Cache::CO_IJG) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /JPEG requires /DCT or /IJG" << (Error*)0;
    badp=true;
  }
  if (cache->hasPredictor()) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /JPEG requires /Predictor 1" << (Error*)0;
    badp=true;
  }
  if (badp) return Rule::Applier::BAD;
  or_->cache.WarningOK=true; /* ?? */
  return Rule::Applier::OK;
}
Rule::Applier::cons_t out_jpeg_work(GenBuffer::Writable& out, Rule::OutputRule*or_, Image::SampledInfo *sf) {
  Rule::Cache *cache=&or_->cache;
  // assert(0);
  if (out_jpeg_check_rule(or_)!=Rule::Applier::OK) return Rule::Applier::DONT_KNOW;
  or_->doSampleFormat(sf);
  // GenBuffer::Writable *tp=&out; /* always binary */
  GenBuffer::Writable *cp=&out;
  /* SUXX: cjpeg(1) won't create a color JPEG for a grayscale image */
  if (cache->Compression==Rule::Cache::CO_DCT) {
    SimBuffer::B other_parameters;
    or_->cacheHints.DCT->dump(other_parameters, 0, false);
    cp=PSEncoder::newDCTEncode(out, or_->cacheHints.EncoderColumns, or_->cacheHints.EncoderRows, or_->cacheHints.EncoderColors, or_->cacheHints.ColorTransform, other_parameters);
  } else {
    assert(cache->Compression==Rule::Cache::CO_None || cache->Compression==Rule::Cache::CO_IJG);
    cp=PSEncoder::newDCTIJGEncode(out, or_->cacheHints.EncoderColumns, or_->cacheHints.EncoderRows, or_->cacheHints.EncoderColors, or_->cacheHints.Quality);
  }
  Rule::writePalData(out, *cp, sf);
  delete cp; 
  return Rule::Applier::OK;
}

Rule::Applier out_jpeg_applier = { "JPEG", out_jpeg_check_rule, out_jpeg_work, 0 };

/* --- Wed Apr 17 13:32:46 CEST 2002 */

Rule::Applier::cons_t out_jpegjai_check_rule(Rule::OutputRule* or_) {
  Rule::Cache *cache=&or_->cache;
  if (cache->FileFormat!=cache->FF_JPEG
   || cache->Compression!=Rule::Cache::CO_JAI
     ) return Rule::Applier::DONT_KNOW;
  bool badp=false;
  if (cache->SampleFormat!=Image::SF_Asis) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /FileFormat/JPEG /Compression/JAI requires /SampleFormat/Asis" << (Error*)0;
    badp=true;
  }
  if (cache->TransferEncoding!=cache->TE_Binary) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /FileFormat/JPEG /Compression/JAI requires /TransferEncoding/Binary" << (Error*)0;
    badp=true;
  }
  if (cache->hasPredictor()) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /FileFormat/JPEG /Compression/JAI requires /Predictor 1" << (Error*)0;
    badp=true;
  }
  if (badp) return Rule::Applier::BAD;
  return Rule::Applier::OK;
}
Rule::Applier::cons_t out_jpegjai_work(GenBuffer::Writable& out, Rule::OutputRule*or_, Image::SampledInfo *sf) {
  /* This is the second simplest Applier I've ever written. */
  if (out_jpegjai_check_rule(or_)!=Rule::Applier::OK) return Rule::Applier::DONT_KNOW;
  Image::Sampled *img=sf->getImg();
  // out.vi_write(img->getHeadp(), img->end_()-img->getHeadp());
  /* ^^^ end_() BUGFIX by pts@fazekas.hu at Sun Jun  2 22:24:32 CEST 2002 */
  /* ^^^ end_() contains 8 extra bytes */
  out.vi_write(img->getHeadp(), img->getRowbeg()-img->getHeadp());
  return Rule::Applier::OK;
}

Rule::Applier out_jpegjai_applier = { "JPEG-JAI", out_jpegjai_check_rule, out_jpegjai_work, 0 };

/* --- Sun Jun  2 22:25:16 CEST 2002 */

class TIFFPrinter {
  GenBuffer::Writable& out;
  /** true iff little endian == LSB first */
  bool le;
  SimBuffer::B s, dir;
 public:
  TIFFPrinter(GenBuffer::Writable &out_, bool le_);
  inline SimBuffer::B& getS() { return s; }
  inline SimBuffer::B const& getDir() const { return dir; }
  void aSHORT(SimBuffer::B &s, unsigned count, unsigned short const*val);
  void aLONG (SimBuffer::B &s, unsigned count, slen_t const*val);
  void dirSHORT(unsigned short tag, slen_t count, unsigned short const*val);
  void dirLONG(unsigned short tag, slen_t count, slen_t const*val);
  void dirSL(unsigned short tag, slen_t val);
  void dirLONG(unsigned short tag, slen_t val);
  void dirRATIONAL(unsigned short tag, slen_t count, slen_t const*val);
  void dirUNDEFINED(unsigned short tag, slen_t count, char const*val);
  void dirUNDEFINED(unsigned short tag, slen_t count, char const*val, slen_t count2, char const*val2);
  void dirClose();
  BEGIN_STATIC_ENUM1(unsigned short)
    EXTRASAMPLE_ASSOCALPHA=1, /* !associated alpha data */
    EXTRASAMPLE_UNASSALPHA=2  /* !unassociated alpha data */
  END_STATIC_ENUM()
  BEGIN_STATIC_ENUM1(unsigned short)
    COMPRESSION_NONE	 =1,	/* dump mode */
    COMPRESSION_CCITTRLE =2,	/* CCITT modified Huffman RLE; unused in sam2p */
    COMPRESSION_CCITTFAX3=3,	/* CCITT Group 3 fax encoding */
    COMPRESSION_CCITTFAX4=4,	/* CCITT Group 4 fax encoding */
    COMPRESSION_LZW	 =5,	/* Lempel-Ziv  & Welch */
    COMPRESSION_OJPEG	 =6,	/* !6.0 JPEG; obsolete, unused in sam2p */
    COMPRESSION_JPEG	 =7,	/* %JPEG DCT compression */
    COMPRESSION_CCITTRLEW=32771,/* #1 w/ word alignment; unused in sam2p */
    COMPRESSION_PACKBITS =32773,/* Macintosh RLE */
    COMPRESSION_DEFLATE  =32946 /* Deflate compression */
  END_STATIC_ENUM()
  BEGIN_STATIC_ENUM1(unsigned short)
    PHOTOMETRIC_MINISBLACK=1,
    PHOTOMETRIC_RGB=2,
    PHOTOMETRIC_PALETTE=3,
    PHOTOMETRIC_MASK=4,
    PHOTOMETRIC_SEPARATED=5, /* possibly CMYK */
    PHOTOMETRIC_YCBCR=6
  END_STATIC_ENUM()
  BEGIN_STATIC_ENUM1(unsigned short)
    GROUP3OPT_2DENCODING=0x1,
    GROUP3OPT_UNCOMPRESSED=0x2,
    GROUP3OPT_FILLBITS=0x4,
    GROUP4OPT_UNCOMPRESSED=0x2
  END_STATIC_ENUM()
  BEGIN_STATIC_ENUM1(unsigned short)
    ImageWidth=256,
    ImageLength=257,
    BitsPerSample=258,
    Compression=259,
    Photometric=262,
    FillOrder=266,
    StripOffsets=273,
    SamplesPerPixel=277,
    RowsPerStrip=278,
    StripByteCounts=279,
    XResolution=282,
    YResolution=283,
    PlanarConfig=284,
    Group3Options=292,
    Group4Options=293,
    ResolutionUnit=296,
    Predictor=317,
    ColorMap=320,
    InkSet=332,
    ExtraSamples=338,
    JPEGTables=347,
    YCbCrSubsampling=530,
    ReferenceBlackWhite=532
  END_STATIC_ENUM()
};

TIFFPrinter::TIFFPrinter(GenBuffer::Writable &out_, bool le_) :out(out_), le(le_) {
  /* Directory now at unknown offset (will be set by .dirClose()) */
  s.vi_write(le_ ? "II*\0\0\0\0\0" : "MM\0*\0\0\0\0", 8);
  dir.vi_write("\0",2); /* initial number of directory entries */
}

void TIFFPrinter::aSHORT(SimBuffer::B &s, unsigned count, unsigned short const*val) {
  if (le) while (count--!=0) { s << (char)(*val) << (char)(*val>>8); val++; }
     else while (count--!=0) { s << (char)(*val>>8) << (char)(*val); val++; }
}
void TIFFPrinter::aLONG (SimBuffer::B &s, unsigned count, slen_t const*val) {
  /* Imp: is vi_write(..., 4); faster ? */
  if (le) while (count--!=0) { s << (char)(*val) << (char)(*val>>8) << (char)(*val>>16) << (char)(*val>>24); val++; }
     else while (count--!=0) { s << (char)(*val>>24) << (char)(*val>>16) << (char)(*val>>8) << (char)(*val); val++; }
}

void TIFFPrinter::dirSHORT(unsigned short const tag, slen_t const count, unsigned short const*val) {
  slen_t offs;
  aSHORT(dir, 1U, &tag);
  dir.vi_write("\0\3"+(le?1:0), 2);
  aLONG(dir, 1U, &count);
  switch (count) {
    case 0: dir.vi_write("\0\0\0", 4); break;
    case 1: aSHORT(dir, 1, val); dir.vi_write("\0", 2); break;
    case 2: aSHORT(dir, 2, val); break;
    default:offs=s.getLength(); aSHORT(s, count, val); aLONG(dir, 1, &offs);
  }
}
void TIFFPrinter::dirLONG(unsigned short const tag, slen_t const count, slen_t const*val) {
  slen_t offs;
  aSHORT(dir, 1U, &tag);
  dir.vi_write("\0\4"+(le?1:0), 2);
  aLONG(dir, 1U, &count);
  switch (count) {
    case 0: dir.vi_write("\0\0\0", 4); break;
    case 1: aLONG(dir, 1, val); break;
    default:offs=s.getLength(); aLONG(s, count, val); aLONG(dir, 1, &offs);
  }
}
void TIFFPrinter::dirRATIONAL(unsigned short tag, slen_t count, slen_t const*val) {
  slen_t offs;
  aSHORT(dir, 1U, &tag);
  dir.vi_write("\0\5"+(le?1:0), 2);
  aLONG(dir, 1U, &count);
  switch (count) {
    case 0: dir.vi_write("\0\0\0", 4); break;
    default:offs=s.getLength(); aLONG(s, count*2, val); aLONG(dir, 1, &offs);
  }
}
void TIFFPrinter::dirUNDEFINED(unsigned short tag, slen_t count, char const*val) {
  slen_t offs;
  aSHORT(dir, 1U, &tag);
  dir.vi_write("\0\7"+(le?1:0), 2);
  aLONG(dir, 1U, &count);
  if (count<=4) {
    dir.vi_write(val, count);
    if (count!=4) dir.vi_write("\0\0\0", 4-count);
  } else {
    offs=s.getLength(); s.vi_write(val, count); aLONG(dir, 1, &offs);
  }
}
void TIFFPrinter::dirUNDEFINED(unsigned short tag, slen_t count, char const*val, slen_t count2, char const*val2) {
  slen_t offs, countx=count+count2;
  aSHORT(dir, 1U, &tag);
  dir.vi_write("\0\7"+(le?1:0), 2);
  aLONG(dir, 1U, &countx);
  if (countx<=4) {
    dir.vi_write(val, count);
    dir.vi_write(val2, count2);
    if (countx!=4) dir.vi_write("\0\0\0", 4-countx);
  } else {
    offs=s.getLength();
    s.vi_write(val, count);
    s.vi_write(val2, count2);
    aLONG(dir, 1, &offs);
  }
}
void TIFFPrinter::dirSL(unsigned short tag, slen_t val) {
  unsigned short sh;
  if (val<1<<16U) { sh=val; dirSHORT(tag, 1, &sh); }
             else dirLONG(tag, 1, &val);
}
void TIFFPrinter::dirLONG(unsigned short tag, slen_t val) {
  dirLONG(tag, 1, &val);
}
void TIFFPrinter::dirClose() {
  char *ss=s.begin_();
  slen_t len=s.getLength();
  if (le) { ss[4]=len; ss[5]=len>>8; ss[6]=len>>16; ss[7]=len>>24; }
     else { ss[4]=len>>24; ss[5]=len>>16; ss[6]=len>>8; ss[7]=len; }
  /* ^^^ dir offset: end of file */
  out.vi_write(ss, len);
  assert(dir.getLength()%12==2);
  unsigned dirc=(dir.getLength()-2)/12;
  ss=dir.begin_();
  if (le) { ss[0]=dirc; ss[1]=dirc>>8; }
     else { ss[0]=dirc>>8; ss[1]=dirc; }
  out.vi_write(dir(), dir.getLength());
  out.vi_write("", 0);
}

/* tiffjai --- Sun Jun  2 22:25:16 CEST 2002 */

Rule::Applier::cons_t out_tiffjai_check_rule(Rule::OutputRule* or_) {
  Rule::Cache *cache=&or_->cache;
  if (cache->FileFormat!=cache->FF_TIFF
   || cache->Compression!=Rule::Cache::CO_JAI
     ) return Rule::Applier::DONT_KNOW;
  bool badp=false;
  if (cache->SampleFormat!=Image::SF_Asis) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /FileFormat/TIFF /Compression/JAI requires /SampleFormat/Asis" << (Error*)0;
    badp=true;
  }
  if (!cache->isBinSB()) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /FileFormat/TIFF /TransferEncoding/Binary|/?SBFirst" << (Error*)0;
    badp=true;
  }
  if (cache->hasPredictor()) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /FileFormat/TIFF /Compression/JAI requires /Prediror 1" << (Error*)0;
    badp=true;
  }
  if (badp) return Rule::Applier::BAD;
  return Rule::Applier::OK;
}
Rule::Applier::cons_t out_tiffjai_work(GenBuffer::Writable& out, Rule::OutputRule*or_, Image::SampledInfo *sf) {
  if (out_tiffjai_check_rule(or_)!=Rule::Applier::OK) return Rule::Applier::DONT_KNOW;
  Image::Sampled *img=sf->getImg();
  unsigned char cs=img->getCs(); /* color space */
  if (cs!=Image::Sampled::CS_GRAYSCALE && cs!=Image::Sampled::CS_RGB && cs!=Image::Sampled::CS_YCbCr && cs!=Image::Sampled::CS_CMYK) {
    /* Dat: CS_YCCK is supported by JPEG, but unsupported by TIFF-JPEG */
    Error::sev(Error::WARNING_DEFER) << "check_rule: /FileFormat/TIFF /Compression/JAI doesn't support this color space" << (Error*)0;
    return Rule::Applier::BAD;
  }
  /* Imp: test all CMYK TIFF tags */
  /* Imp: test with RGB and CMYK images */
  TIFFPrinter tp(out, or_->cache.TransferEncoding==or_->cache.TE_LSBfirst);
  /* ^^^ Dat: Binary defaults to MSBfirst, beacuse QuarkXPress 3 can read only
   * MSBfirst TIFF files.
   */
  char const *databeg=img->getHeadp()+img->getXoffs();
  assert(databeg[-2]=='\xFF' && databeg[-1]=='\xC0');
  slen_t datalen=img->getRowbeg()-databeg;
  // printf("LEN=%u gl=%u datalen=%u\n", img->end_()-img->begin_(), img->getLength(), datalen);
  tp.getS().vi_write("\xFF\xD8\xFF\xC0", 4); /* fake-SOI SOF0 */
  tp.getS().vi_write(databeg, datalen); /* DataPart */
  // tp.getS().vi_write("\xFF\xD9", 2); /* fake-EOI */
  /* ^^^ there must be an EOI already */
  
  unsigned phot=0;  bool inks=false, refe=false;
  switch (cs) {
    case Image::Sampled::CS_GRAYSCALE: phot=tp.PHOTOMETRIC_MINISBLACK; break;
    case Image::Sampled::CS_RGB: phot=tp.PHOTOMETRIC_RGB; break;
    case Image::Sampled::CS_YCbCr: phot=tp.PHOTOMETRIC_YCBCR; refe=true; break; /* preferred to RGB */
    case Image::Sampled::CS_CMYK: phot=tp.PHOTOMETRIC_SEPARATED; inks=true; break; /* preferred to RGB */
    default: Error::sev(Error::EERROR) << "TIFF6-JAI: color space " << (unsigned)cs << " not supported in TIFF-JPEG" << (Error*)0;
  }

  /* Dat: TIFF tags must appear in increasing numerical order */
  tp.dirSL(tp.ImageWidth, img->getWd());
  tp.dirSL(tp.ImageLength, img->getHt());
  unsigned short eights[]={8,8,8,8};
  tp.dirSHORT(tp.BitsPerSample, img->cs2cpp[cs], eights);
  tp.dirSL(tp.Compression, tp.COMPRESSION_JPEG); /* SHORT */
  tp.dirSL(tp.Photometric, phot); /* SHORT */
  tp.dirLONG(tp.StripOffsets, 8);
  tp.dirSL(tp.SamplesPerPixel, img->cs2cpp[cs]); /* SHORT */
  tp.dirSL(tp.RowsPerStrip, img->getHt());
  tp.dirLONG(tp.StripByteCounts, datalen+6);
  slen_t rats[]={1,1, 0,1, 255,1, 128,1, 255,1, 128,1, 255,1};
  tp.dirRATIONAL(tp.XResolution, 1, rats);
  tp.dirRATIONAL(tp.YResolution, 1, rats);
  tp.dirSL(tp.PlanarConfig, 1); /* SHORT, PLANARCONFIG_CONTIG */
  tp.dirSL(tp.ResolutionUnit, 1); /* SHORT */
  if (inks) tp.dirSL(tp.InkSet, 1); /* SHORT */
  tp.dirUNDEFINED(tp.JPEGTables, databeg-img->getHeadp()-2, img->getHeadp(), 2, "\xFF\xD9");
  unsigned char hvs=img->end_()[-1];
  if (hvs!=0x22 && cs==Image::Sampled::CS_YCbCr) {
    // printf("hvs=0x%02X\n", hvs);
    unsigned short horiz_vert[2]={ hvs>>4, hvs&15 };
    tp.dirSHORT(tp.YCbCrSubsampling, 2, horiz_vert);
  }
  if (refe) tp.dirRATIONAL(tp.ReferenceBlackWhite, 6, rats+2);
  tp.dirClose();
  #if 0
    { Files::FILEW f(fopen("t.hea","wb"));
      f.vi_write(img->getHeadp(), databeg-img->getHeadp()-2); /* HeadPart */
      f.vi_write("\xFF\xD9", 2);
      f.close();
    }
    { Files::FILEW f(fopen("t.dat","wb"));
      f.vi_write("\xFF\xD8\xFF\xC0", 4); /* fake-SOI SOF0 */
      f.vi_write(databeg, datalen); /* DataPart */
      f.close();
    }
  #endif
  // out.vi_write(img->getHeadp(), img->getRowbeg()-img->getHeadp()); /* HeadPart */
  // out.vi_write(img->getRowbeg(), img->end_()-img->getRowbeg()); /* DataPart */
  return Rule::Applier::OK;
  /* SUXX: see compatibility notes in FAQ (README) */
}

Rule::Applier out_tiffjai_applier = { "TIFF6-JAI", out_tiffjai_check_rule, out_tiffjai_work, 0 };

/* tiff --- Tue Jun  4 16:23:48 CEST 2002 */

/** by pts@fazekas.hu at Tue Jun 11 16:56:53 CEST 2002
 * Reads a JPEG Baseline (SOF0) stream on input, and drops everything before
 * the first SOF0 marker. Then writes an SOI marker, the SOF0 marker, and
 * everything after the SOI marker. The last two bytes read must be an
 * EOI marker (/\xFF\xD9/). The first thing to be read must be a SOI marker
 * (/\xFF+\xD8/). The getJPEGTables() method can be used to retrieve the
 * data read before the SOF0 marker. An SOS marker must be read after SOF0.
 * Also retains some information (hvsamples[0]) from the SOF0 marker.
 *   Suitable for reading the output of cjpeg and Ghostscript /FlateEncode.
 *   This encoder would be much harder if we were not allowed to read the
 * JPEG stream into memory.
 */
class JPEGSOF0Encode: public PSEncoder {
 public:
  /** @param maxcpl_: maximum # hex digits per line, should be even */
  JPEGSOF0Encode(GenBuffer::Writable &out_);
  virtual void vi_write(char const*buf, slen_t len);
  SimBuffer::Flat const& getJPEGTables() const { return buf; }
  unsigned char getColorSpace() const { return gi.colorspace; }
  unsigned char getHVS() const { return gi.hvs; }
 protected:
  GenBuffer::Writable &out;
  SimBuffer::B buf;
  struct jai_gfxinfo gi;
};

JPEGSOF0Encode::JPEGSOF0Encode(GenBuffer::Writable &out_): out(out_) {}
void JPEGSOF0Encode::vi_write(char const*bufr, slen_t len) {
  if (len==0) {
    { Filter::FlatD flatd(buf(), buf.getLength());
      jai_parse_jpeg(&gi, &flatd);
    }
    slen_t len=buf.getLength();
    if (gi.bad==0 && (len<4 || buf[len-2]!='\xFF' || buf[len-1]!='\xD9')) gi.bad=10;
    if (gi.bad!=0)
      Error::sev(Error::EERROR) << "JPEGS0F0: invalid JPEG stream: " << jai_errors[gi.bad] << (Error*)0;
    out.vi_write("\xFF\xD8", 2); /* extra SOI */
    out.vi_write(buf()+gi.SOF_offs-2, len-gi.SOF_offs+2); /* SOF0 and followers */
    out.vi_write(0,0);
    assert(buf[gi.SOF_offs-2]=='\xFF'); /* BUGFIX at 2002.12.02 */
    buf[gi.SOF_offs-1]='\xD9'; /* extra EOI */
    buf.keepLeft(gi.SOF_offs); /* keep only headers */
  } else buf.vi_write(bufr, len);
}


Rule::Applier::cons_t out_tiff_check_rule(Rule::OutputRule* or_) {
  Rule::Cache *cache=&or_->cache;
  if (cache->FileFormat!=cache->FF_TIFF
   || cache->Compression==Rule::Cache::CO_JAI
     ) return Rule::Applier::DONT_KNOW;
  bool badp=false;
  /* Dat: acroread TIFF predictor2 OK. (examples/fishg_lzw2_pdf.job) */
  /* Dat: /ZIP with /Predictor OK */
  if (cache->Predictor!=cache->PR_None) {
    if (cache->Predictor!=cache->PR_TIFF2) {
      Error::sev(Error::WARNING_DEFER) << "check_rule: /FileFormat/TIFF requires /Predictor 1|2" << (Error*)0;
      badp=true;
    }
    if (cache->Compression!=Rule::Cache::CO_ZIP && cache->Compression!=Rule::Cache::CO_LZW) {
      Error::sev(Error::WARNING_DEFER) << "check_rule: real /Predictor requires /ZIP or /LZW" << (Error*)0;
      badp=true;
    }
  }
  #if !HAVE_LZW
    if (cache->Compression==Rule::Cache::CO_LZW) {
      Error::sev(Error::WARNING_DEFER) << "check_rule: `configure --enable-lzw' for /Compression/LZW with /FileFormat/TIFF" << (Error*)0;
      badp=true;
    }
  #endif
  if (cache->Compression==Rule::Cache::CO_Fax && !cache->isOneBit()) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /FileFormat/TIFF /Compression/Fax requires a 1-bit /SampleFormat" << (Error*)0;
    badp=true;
  }
  if (!cache->isGray() && !cache->isTransparentM() && !cache->isIndexed() && !cache->isRGB()) {
    /* Dat: unsupported SampleFormats: /Opaque, /Transparent and /Asis. */
    Error::sev(Error::WARNING_DEFER) << "check_rule: unsupported /SampleFormat for /FileFormat/TIFF" << (Error*)0;
    badp=true;
  }
  if (!cache->isBinSB()) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /FileFormat/TIFF requires /TransferEncoding/Binary|/?SBFirst" << (Error*)0;
    badp=true;
  }
  if (cache->isDCTE() && cache->SampleFormat!=Image::SF_Rgb8 && cache->SampleFormat!=Image::SF_Gray8 && cache->SampleFormat!=Image::SF_Indexed8) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /DCTEncode requires /Rgb8 or /Gray8 (or /Indexed8)" << (Error*)0;
    badp=true;
  }
  if (!badp && cache->isTransparentM()) {
    cache->origSampleFormat=cache->SampleFormat;
    cache->SampleFormat=Image::SF_Transparent8;
  }
  return (Rule::Applier::cons_t)(badp ? 0+Rule::Applier::BAD : 0+Rule::Applier::OK);
  /* ^^^ Dat: 0+: pacify g++-3.1 */
}

Rule::Applier::cons_t out_tiff_work(GenBuffer::Writable& out, Rule::OutputRule*or_, Image::SampledInfo *sf) {
  Rule::Cache *cache=&or_->cache;
  Image::sf_t origSampleFormat=cache->origSampleFormat;
  if (out_tiff_check_rule(or_)!=Rule::Applier::OK) return Rule::Applier::DONT_KNOW;

  assert(!sf->hasTranspp() || cache->isTransparentM());
  /** Alpha channel or null. */
  Image::Indexed *alpha=(Image::Indexed*)NULLP;
  if (cache->isTransparentM()) {
    // fprintf(stderr,"sf=%u osf=%u\n", cache->SampleFormat, origSampleFormat);
    alpha=PTS_dynamic_cast(Image::Indexed*,sf->getImg())->calcAlpha();
    PTS_dynamic_cast(Image::Indexed*,sf->getImg())->getClearTransp();
    assert((alpha!=NULLP) == (sf->hasTranspp()==true));
    sf->clearTransp();
    static Image::sf_t const graytab[9]={Image::SF_None,Image::SF_Gray1,Image::SF_Gray2,Image::SF_None,Image::SF_Gray4,Image::SF_None,Image::SF_None,Image::SF_None,Image::SF_Gray8};
    static Image::sf_t const rgbtab[9]={Image::SF_None,Image::SF_Rgb1,Image::SF_Rgb2,Image::SF_None,Image::SF_Rgb4,Image::SF_None,Image::SF_None,Image::SF_None,Image::SF_Rgb8};
    static Image::sf_t const indexedtab[9]={Image::SF_None,Image::SF_Indexed1,Image::SF_Indexed2,Image::SF_None,Image::SF_Indexed4,Image::SF_None,Image::SF_None,Image::SF_None,Image::SF_Indexed8};
    unsigned char minbpc=sf->minRGBBpcc();
    // fprintf(stderr,"minbpc=%u\n",minbpc);
         if (minbpc<8 && origSampleFormat==Image::SF_Transparent8) minbpc=8;
    else if (minbpc<4 && origSampleFormat==Image::SF_Transparent4) minbpc=4;
    else if (minbpc<2 && origSampleFormat==Image::SF_Transparent2) minbpc=2;
    // fprintf(stderr,"minbpc=%u\n",minbpc);
    // cacheHints.EncoderBPL=(slen_t)img->getWd()*img->getCpp()*img->getBpc();
    /* ^^^ Dat: doSampleFormat will do it correctly */
    // assert(saf!=Image::SF_max);
    cache->SampleFormat=(alpha==NULLP ? indexedtab : sf->minGrayBpcc()==0 ? rgbtab : graytab)[minbpc];
  }

  or_->doSampleFormat(sf); /* No separations */
  Image::Sampled *img=sf->getImg(); /* call this _after_ doSampleFormat()! */

  TIFFPrinter tp(out, or_->cache.TransferEncoding==or_->cache.TE_LSBfirst);
  /* ^^^ Dat: Binary defaults to MSBfirst, beacuse QuarkXPress 3 can read only
   * MSBfirst TIFF files.
   */
  unsigned phot= cache->isRGB() ? 0+tp.PHOTOMETRIC_RGB
             // : cache->SampleFormat==Image::SF_Mask ? tp.PHOTOMETRIC_MASK
             : cache->isGray() ? 0+tp.PHOTOMETRIC_MINISBLACK
             // : cache->isIndexed() ? tp.PHOTOMETRIC_PALETTE
             : 0+tp.PHOTOMETRIC_PALETTE /* /Indexed*, /Mask, /Transparent+ */;
  unsigned compr=tp.COMPRESSION_NONE;

  Filter::VerbatimCountE vc(tp.getS());
  GenBuffer::Writable *cp=&vc;
  JPEGSOF0Encode *jp=(JPEGSOF0Encode*)NULLP;
  // Dat: g++-3.4: Rule::Cache::CO_None: `cache cannot appear in a constant expression'
  switch (cache->Compression) {
   case Rule::Cache::CO_None: break;
   case Rule::Cache::CO_ZIP: compr=tp.COMPRESSION_DEFLATE; cp=PSEncoder::newFlateEncode(*cp, or_->cacheHints.Effort); break;
   case Rule::Cache::CO_LZW: compr=tp.COMPRESSION_LZW; cp=PSEncoder::newLZWEncode(*cp); break;
   /* vvv Dat: RunLengthEncode EOD (char 128) is OK and ignored in TIFF PackBits streams */
   case Rule::Cache::CO_RLE: compr=tp.COMPRESSION_PACKBITS; cp=PSEncoder::newRunLengthEncode(*cp, or_->cacheHints.RecordSize); break;
   case Rule::Cache::CO_Fax:
    if (or_->cacheHints.K<0) compr=tp.COMPRESSION_CCITTFAX4;
    else if (or_->cacheHints.K==0) compr=tp.COMPRESSION_CCITTFAX3;
    else { compr=tp.COMPRESSION_CCITTFAX3; or_->cacheHints.K=img->getHt(); }
    cp=PSEncoder::newCCITTFaxEncode(*cp, or_->cacheHints.K, or_->cacheHints.EncoderBPL, /*EndOfLine:*/ compr==tp.COMPRESSION_CCITTFAX3, /*BlackIs1: !*/ true);
    /* Dat: libtiff enforces EndOfLine==(compr==tp.COMPRESSION_CCITTFAX3) */
    break;
   case Rule::Cache::CO_IJG:
    compr=tp.COMPRESSION_JPEG;
    jp=new JPEGSOF0Encode(vc);
    cp=PSEncoder::newDCTIJGEncode(*jp, or_->cacheHints.EncoderColumns, or_->cacheHints.EncoderRows, or_->cacheHints.EncoderColors, or_->cacheHints.Quality);
    break;
   case Rule::Cache::CO_DCT: compr=tp.COMPRESSION_JPEG; {
    SimBuffer::B other_parameters;
    or_->cacheHints.DCT->dump(other_parameters, 0, false);
    jp=new JPEGSOF0Encode(vc);
    cp=PSEncoder::newDCTEncode(*jp, or_->cacheHints.EncoderColumns, or_->cacheHints.EncoderRows, or_->cacheHints.EncoderColors, or_->cacheHints.ColorTransform, other_parameters);
    break; }
   default: assert(0);
  }

  GenBuffer::Writable *pp=cp;
  if (cache->hasPredictor()) pp=PSEncoder::newPredictor(*cp, cache->Predictor, or_->cacheHints.PredictorBPC, or_->cacheHints.PredictorColumns, or_->cacheHints.PredictorColors);

  slen_t rlenht=img->getRlen()*img->getHt();
  Image::Sampled::dimen_t wd=img->getWd();
  if (rlenht!=0) {
    /* Dat: TIFF inserts extra sample bits... */
    /* TIFF images with transparency are really inefficient. That's because
     * libtiff doesn't read or write an indexed image with transparency:
     * Sorry, can not handle contiguous data with PhotometricInterpretation=3,
     * and Samples/pixel=2.
     *   So Indexed images are converted to RGB* or Gray* first, and then the
     * alpha channel is added. The result is a big and inefficiently
     * compressible TIFF file.
     */

    if (alpha!=NULLP) {
      unsigned char bpc=img->getBpc();
      slen_t rlen=img->getRlen();
      slen_t writelen;
      char *buf=(char*)NULLP;
      char const*psave=img->getRowbeg(), *p, *ppend=psave+rlenht;
      char const*r=alpha->getRowbeg(), *rend;
      char *t;
      register unsigned u;
      Image::Sampled::dimen_t rlena=(wd+7)>>3;
      assert(rlena==alpha->getRlen());
      // printf("SF=%u\n", cache->SampleFormat);
      if (cache->isGray()) {
        /* works at Mon Dec  9 01:25:59 CET 2002 */
        static unsigned char const szor1[16]={2*0,2*1,2*4,2*5,2*16,2*17,2*20,2*21,2*64,2*65,2*68,2*69,2*80,2*81,2*84,2*85};
        static unsigned char const szor2[16]={85,84,81,80,69,68,65,64,21,20,17,16,5,4,1,0};
        // static unsigned char const szor2[8]={0,1,16,17,64,65,80,81};
        buf=new char[(writelen=((slen_t)wd*bpc+3)>>2)+24];
        assert(rlena*8>=rlen);
        while (psave!=ppend) {
          t=buf;
          rend=r+rlena;
          p=psave; psave+=rlen;
          assert(psave<=ppend);
          u=(1<<16);
          switch (bpc) {
           case 8:
            while (p!=psave) {
              /* Dat: works at Tue Sep 10 22:45:52 CEST 2002 */
              if (0!=(u&(1<<16))) u=*(unsigned char const*)r++|(1<<8);
              *t++=*p++; *t++=-(0==(u&128));
              u<<=1;
            } break;
           case 4:
            while (p!=psave) {
              if (0!=(u&(1<<16))) u=*(unsigned char const*)r++|(1<<8);
              *t++=(p[0]&0xF0)|(0==(u&128)?15:0);
              *t++=((p[0]&0xF)<<4)|(0==(u&64)?15:0);
              p++; u<<=2;
            } break;
           case 2:
            while (p!=psave) {
              if (0!=(u&(1<<16))) u=*(unsigned char const*)r++|(1<<8);
              *t++=(p[0]&0xC0    )|((u&128)!=0?0:48)|((p[0]&0x30)>>2)|((u&64)!=0?0:3);
              *t++=((p[0]&0xC)<<4)|((u& 32)!=0?0:48)|((p[0]&0x3 )<<2)|((u&16)!=0?0:3);
              p++; u<<=4;
            } break;
           case 1:
            while (p!=psave) {
              u=*(unsigned char const*)r++;
              *t++=szor1[*(unsigned char const*)p>>4]|szor2[u>>4];
              *t++=szor1[*(unsigned char const*)p&15]|szor2[u&15];
              p++;
            } break;
          } /* SWITCH Gray bpc */
          // assert(p-psave+0U==rlena*8U);
          assert(p>=psave);
          pp->vi_write(buf, writelen);
        }
      } else {
        /* works at Sun Dec  8 23:30:17 CET 2002 */
        assert(cache->isRGB());
        buf=new char[(writelen=((slen_t)wd*bpc+1)>>1)+24];
        while (psave!=ppend) {
          t=buf;
          p=psave; psave+=rlen;
          assert(psave<=ppend);
          u=(1<<16);
          rend=r+rlena; /* superfluous */
          switch (bpc) {
           case 8:
            while (p<psave) {
              if (0!=(u&(1<<16))) u=*(unsigned char const*)r++|(1<<8);
              *t++=*p++;
              *t++=*p++;
              *t++=*p++;
              *t++=-(0==(u&128));
              u<<=1;
            } break;
           case 4:
            while (p<psave) {
              if (0!=(u&(1<<16))) u=*(unsigned char const*)r++|(1<<8);
              *t++=p[0]; /* R0 and G0 */
              *t++=(p[1]&0xF0)|((u&128)!=0?0:15); /* B0 and A0 */
              *t++=(p[1]<<4)|((p[2]>>4)&15); /* R1 and G1 */
              *t++=(p[2]<<4)|((u&64)!=0?0:15); /* B1 and A1 */
              p+=3; u<<=2;
            } break;
           case 2:
            while (p<psave) {
              if (0!=(u&(1<<16))) u=*(unsigned char const*)r++|(1<<8);
              /* Dat: p[0]==R0G0B0R1  p[1]==G1B1R2G2  p[2]==B2R3G3B3 */
              *t++=(p[0]&0xFC)|((u&128)!=0?0:3); /* R0G0B0 and A0 */
              *t++=(p[0]<<6)|((p[1]>>2)&0x3C)|((u&64)!=0?0:3); /* R1 G1B1 A1 */
              *t++=(p[1]<<4)|((p[2]>>4)&0x0C)|((u&32)!=0?0:3); /* R2G1 B1 A2 */
              *t++=(p[2]<<2)|((u&16)!=0?0:3); /* R3G3B3 and A3 */
              p+=3; u<<=4;
            } break;
           default: // case 1:
            while (p<psave) {
              u=*(unsigned char const*)r++;
              /* Dat: p[0]==RGBRGBRG p[1]==BRGBRGBR  p[2]==GBRGBRGB */
              *t++=(p[0]&0xE0)|((u&128)!=0?0:16)
                  |((p[0]>>1)&0xE)|((u&64)!=0?0:1);
              *t++=(p[0]<<6)|((p[1]>>2)&0x80)|((u&32)!=0?0:16)
                  |((p[1]>>3)&0xE)|((u&16)!=0?0:1);
              *t++=((p[1]<<4)&0xE0)|((u&8)!=0?0:16)
                  |((p[1]<<3)&0x7)|((p[2]>>5)&0x06)|((u&4)!=0?0:1);
              *t++=((p[2]<<2)&0xE0)|((u&2)!=0?0:16)
                  |((p[2]<<1)&0x0E)|((u&1)!=0?0:1);
              p+=3;
            }
           break;
          } /* SWITCH RGB bpc */
          assert(r==rend); // r=rend;
          pp->vi_write(buf, writelen);
        }
      }
      delete [] buf;
    } else pp->vi_write(img->getRowbeg(), rlenht);
  }
  pp->vi_write(0,0); /* flush all */
  if (pp!=cp)  delete pp;
  if (cp!=&vc) delete cp;

  bool refe=false;
  if (compr==tp.COMPRESSION_JPEG) switch (jp->getColorSpace()) {
    case Image::Sampled::CS_GRAYSCALE:
      assert(phot==tp.PHOTOMETRIC_MINISBLACK || phot==tp.PHOTOMETRIC_PALETTE);
      /* Dat: we don't change `phot' here, so JPEG compression can be applied
       *      to the palette indexes of indexed images :-)
       */
      break;
    case Image::Sampled::CS_RGB: phot=tp.PHOTOMETRIC_RGB; break;
    case Image::Sampled::CS_YCbCr: phot=tp.PHOTOMETRIC_YCBCR; refe=true; break; /* preferred to RGB */
    // case Image::Sampled::CS_CMYK: phot=tp.PHOTOMETRIC_SEPARATED; inks=true; break; /* preferred to RGB */
    default: Error::sev(Error::EERROR) << "TIFF6: color space " << (unsigned)jp->getColorSpace() << " not supported in TIFF-JPEG" << (Error*)0;
  }

  /* Dat: TIFF tags must appear in increasing numerical order */
  tp.dirSL(tp.ImageWidth, wd);
//  tp.dirSL(tp.ImageWidth, img->getWd()/2);
  tp.dirSL(tp.ImageLength, img->getHt());
  unsigned short s4[]={img->getBpc(),img->getBpc(),img->getBpc(),img->getBpc()};
  /* ^^^ these values may be different according to the TIFF6 spec, but 
   * libtiff can read TIFF files only with same BitsPerSamples.
   */
  unsigned sppa=img->getCpp()+(alpha!=NULLP ? 1:0);
  tp.dirSHORT(tp.BitsPerSample, sppa, s4);
  tp.dirSL(tp.Compression, compr); /* SHORT */
  tp.dirSL(tp.Photometric, phot); /* SHORT */
  if (cache->Compression==Rule::Cache::CO_Fax) tp.dirSL(tp.FillOrder, 1); /* byte abcdefgh is (a<<7)+...+h */
  tp.dirLONG(tp.StripOffsets, 8);
  if (!cache->isIndexed())
    tp.dirSL(tp.SamplesPerPixel, sppa); /* SHORT */
//    tp.dirSL(tp.SamplesPerPixel, 2); /* SHORT */
  tp.dirSL(tp.RowsPerStrip, img->getHt());
  tp.dirLONG(tp.StripByteCounts, vc.getCount());
  slen_t rats[]={1,1, 0,1, 255,1, 128,1, 255,1, 128,1, 255,1};
  tp.dirRATIONAL(tp.XResolution, 1, rats);
  tp.dirRATIONAL(tp.YResolution, 1, rats);
  tp.dirSL(tp.PlanarConfig, 1); /* SHORT, PLANARCONFIG_CONTIG */
  if (compr==tp.COMPRESSION_CCITTFAX3) tp.dirLONG(tp.Group3Options, (tp.GROUP3OPT_UNCOMPRESSED)|(or_->cacheHints.K!=0?tp.GROUP3OPT_2DENCODING:0));
  if (compr==tp.COMPRESSION_CCITTFAX4) tp.dirLONG(tp.Group4Options, tp.GROUP4OPT_UNCOMPRESSED);
  tp.dirSL(tp.ResolutionUnit, 1); /* SHORT */
  if (cache->Predictor!=cache->PR_None) {
    tp.dirSL(tp.Predictor, cache->Predictor); /* 1|2 */
    if (img->getBpc()!=8)
      Error::sev(Error::WARNING) << "TIFF6: libtiff supports /Predictor only with bpc=8 and bpc=16" << (Error*)0;
  }

  if (cache->isIndexed() || cache->isTransparentM()) {
    unsigned colorlen=(1<<img->getBpc()), i=0;
    unsigned short *r=new unsigned short[3*colorlen], *g=r+colorlen, *b=g+colorlen;
    memset(r, '\0', 3*colorlen);
    /** Image palette data */
    char const *p=img->getHeadp(), *pend=img->getRowbeg();
    assert((pend-p)%3==0);
    assert(3*colorlen>=(unsigned)(pend-p));
    while (p<pend) {
      r[i  ]=*(unsigned char const*)p++*257;
      g[i  ]=*(unsigned char const*)p++*257;
      b[i++]=*(unsigned char const*)p++*257;
    }
    /* ^^^ In a TIFF palette (ColorMap) 0 is black, 65535 is lightest */
    tp.dirSHORT(tp.ColorMap, colorlen*3, r);
    delete [] r;
  }
  if (alpha!=NULLP) {
    /* vvv Dat: GIMP 1.0 load fails with tp.EXTRASAMPLE_UNASSALPHA */
    tp.dirSL(tp.ExtraSamples, tp.EXTRASAMPLE_ASSOCALPHA); /* SHORT */
    delete alpha;
  }
  /* no tp.InkSet, because no CMYK */
  
  if (compr==tp.COMPRESSION_JPEG) {
    tp.dirUNDEFINED(tp.JPEGTables, jp->getJPEGTables().getLength(), jp->getJPEGTables()() /* , 2, "\xFF\xD9" */);
    unsigned char hvs=jp->getHVS();
    if (hvs!=0x22 && phot==tp.PHOTOMETRIC_YCBCR) {
      unsigned short horiz_vert[2]={ hvs>>4, hvs&15 };
      tp.dirSHORT(tp.YCbCrSubsampling, 2, horiz_vert);
    }
    if (refe) tp.dirRATIONAL(tp.ReferenceBlackWhite, 6, rats+2);
    delete jp;
  }
  
  tp.dirClose();
  return Rule::Applier::OK;
}

Rule::Applier out_tiff_applier = { "TIFF6", out_tiff_check_rule, out_tiff_work, 0 };


/* --- Sat Apr 20 11:32:42 CEST 2002 -- Sat Apr 20 16:48:25 CEST 2002 */

#if OBJDEP
#  warning REQUIRES: crc32.o
#  warning REQUIRES: crc32.o
#endif
/** Encodes 32-bit (data.length-minus), data, crc32. */
class LenCRC32Encode: public PSEncoder {
 public:
  /** @param maxcpl_: maximum # hex digits per line, should be even */
  LenCRC32Encode(GenBuffer::Writable &out_, unsigned minus_);
  virtual void vi_write(char const*buf, slen_t len);

 protected:
  GenBuffer::Writable &out;
  unsigned minus;
  /** The length is not known in advance, so we have to buffer the whole output
   * in memory.
   */
  SimBuffer::B sofar;
};

LenCRC32Encode::LenCRC32Encode(GenBuffer::Writable &out_, unsigned minus_): out(out_), minus(minus_), sofar("1234") {}
void LenCRC32Encode::vi_write(char const*buf, slen_t len) {
  if (len==0) {
    char *s=const_cast<char*>(sofar());
    slen_t slen=sofar.getLength();
    unsigned PTS_INT32_T crc=crc32(CRC32_INITIAL, s+4, slen-4);
    slen-=minus+4;
    s[0]=slen>>24; s[1]=slen>>16; s[2]=slen>>8; s[3]=slen;
    s=sofar.vi_mkend(4);
    s[0]=crc >>24; s[1]=crc>>16;  s[2]=crc>>8;  s[3]=crc;
    out.vi_write(sofar(), sofar.getLength());
    out.vi_write(0,0);
  } else sofar.vi_write(buf, len);
}

/** PNG output (RFC 2083) */
Rule::Applier::cons_t out_png_check_rule(Rule::OutputRule* or_) {
  /* Supported PNG types: /Gray1, /Gray2, /Gray4, /Gray8, /Rgb8,
   *   /Indexed1, /Indexed2, /Indexed4, /Indexed8. 
   * Unsupported PNG types: /Gray16, /Rgb16, /GrayA8, /GrayA16, /RgbA8,
   *   /RgbA16.
   * /Indexed* supports transparency (via PNG chunk tRNS).
   */
  Rule::Cache *cache=&or_->cache;
  if (cache->FileFormat!=cache->FF_PNG
     ) return Rule::Applier::DONT_KNOW;
  bool badp=false;
  /* Dat: (by policy) we don't support /Transparent or /Opaque here; the
   * user should not specify such SampleFormat in the .job file.
   */
  if (!cache->isGray()
   && !cache->isTransparentM()
   && !cache->isIndexed()
   && cache->SampleFormat!=Image::SF_Rgb8) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /PNG requires /Gray*, /Indexed*, /Mask, /Transparent+ or /Rgb8" << (Error*)0;
    badp=true;
  }
  if (cache->TransferEncoding!=cache->TE_Binary) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /PNG requires /Binary" << (Error*)0;
    badp=true;
  }
  if (cache->Compression!=Rule::Cache::CO_None && cache->Compression!=Rule::Cache::CO_ZIP) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /PNG requires /Compression/None or /ZIP" << (Error*)0;
    badp=true;
  }
  if (cache->Predictor==cache->PR_TIFF2) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /PNG requires /Predictor >=10" << (Error*)0;
    badp=true;
  }
  if (badp) return Rule::Applier::BAD;
  /* Now we are sure about Rule::Applier::OK. */
  or_->cache.WarningOK=true; /* ?? */
  return Rule::Applier::OK;
}
Rule::Applier::cons_t out_png_work(GenBuffer::Writable& out, Rule::OutputRule*or_, Image::SampledInfo *sf) {
  Rule::Cache *cache=&or_->cache;
  char tmp[64], colortype; register char*p;
  unsigned PTS_INT32_T crc;
  
  // assert(0);
  if (out_png_check_rule(or_)!=Rule::Applier::OK) return Rule::Applier::DONT_KNOW;
  or_->doSampleFormat(sf);
  Image::Sampled *img=sf->getImg();
  unsigned char bpc=img->getBpc(); /* set _after_ doSampleFormat */
  Image::Sampled::dimen_t wd=img->getWd(), ht=img->getHt();
  
  GenBuffer::Writable *rp=new LenCRC32Encode(out,4);
  GenBuffer::Writable *cp=PSEncoder::newFlateEncode(*rp, (cache->Compression==Rule::Cache::CO_None) ? 0: or_->cacheHints.Effort);

  /* Dat: PredictorColumns, PredictorBPC and PredictorColors are forced to
   *      match the image, because PNG requires it.
   */
  GenBuffer::Writable *pp=PSEncoder::newPredictor(
    *cp,
    cache->Predictor==cache->PR_None ? cache->PR_PNGNone : cache->Predictor,
    bpc,          /*or_->cacheHints.PredictorBPC*/
    wd,           /*or_->cacheHints.PredictorColumns*/
    img->getCpp() /*or_->cacheHints.PredictorColors*/
  );

  /* Critical PNG chunks (capital 1st letter) and ancillary PNG chunks:
   *
   * -: zero, 1: exactly one, ?: 0 or 1, *: 0 or more, +: 1 or more
   * Name  Nr sam2p Ordering constraints
   * IHDR  1  1     Must be first
   * cHRM  ?  -     Before PLTE and IDAT
   * gAMA  ?  -     Before PLTE and IDAT
   * sBIT  ?  -     Before PLTE and IDAT
   * PLTE  ?  ?     Before IDAT
   * hIST  ?  -     After PLTE; before IDAT
   * tRNS  ?  ?     After PLTE; before IDAT
   * bKGD  ?  ?     After PLTE; before IDAT
   * pHYs  ?  -     Before IDAT
   * tIME  ?  -     None
   * tEXt  *  -     None
   * zTXt  *  -     None
   * IDAT  +  1     After IHDR and PLTE, multiple IDATs must be consecutive
   * IEND  1  1     Must be last
   */
  
  /* 0..3:   PNG header */
  /* 8..11:  IHDR chunk length */
  /* 12..15: IHDR chunk name */
  memcpy(tmp,"\211PNG\r\n\032\n\0\0\0\015IHDR",16); /* ASCII charset is assumed. */
  p=tmp+16;
  *p++=wd>>24; *p++=wd>>16; *p++=wd>>8; *p++=wd; /* 16: Width */
  *p++=ht>>24; *p++=ht>>16; *p++=ht>>8; *p++=ht; /* 20: Height */
  *p++=bpc; /* 24: Bit depth */
  colortype=*p++=cache->isGray()?0:cache->isRGB()?2:3; /* 25: Color type */
  *p++=0; /* 26: Compression method: deflate/32K (forced by PNG spec), overcoming with Effort==0 */
  *p++=0; /* 27: Filter type: adaptive filtering with 5 basic PNG filter types */
  *p++=0; /* 28: Non-interlaced */
  crc=crc32(CRC32_INITIAL, tmp+12, 17);
  *p++=crc>>24;*p++=crc>>16;*p++=crc>>8;*p++=crc; /* 29: IHDR CRC */
  out.vi_write(tmp, 33);
  if (colortype==3) {
    bool transp=cache->isTransparentM() && PTS_dynamic_cast(Image::Indexed*,img)->getTransp()>=0;
    /* unsigned ncols=PTS_dynamic_cast(Image::Indexed*,img)->getNcols(); */
    unsigned ncols3=img->getRowbeg()-img->getHeadp();
    
    p=tmp;
    *p++=0; *p++=0; *p++=ncols3>>8; *p++=ncols3; /* 0: PLTE chunk length */
    *p++='P'; *p++='L'; *p++='T'; *p++='E'; /* 4: PLTE chunk name */
    out.vi_write(tmp, 8);
    if (transp) {
      PTS_dynamic_cast(Image::Indexed*,img)->makeTranspZero();
      p=img->getHeadp(); p[0]=p[1]=p[2]=(char)0; /* force black backround (for consistency) */
    } 
    out.vi_write(img->getHeadp(), ncols3);
    crc=crc32(crc32(CRC32_INITIAL,tmp+4,4), img->getHeadp(), ncols3);
    p=tmp; *p++=crc>>24;*p++=crc>>16;*p++=crc>>8;*p++=crc; /* 0: PLTE CRC */
    out.vi_write(tmp, 4);
    if (transp) {
      out.vi_write("\0\0\0\1tRNS\0@\346\330f", 13); /* color 0 is transparent */
      //fprintf(stderr, "tRNS: 0x%08lx", (unsigned long)crc32(CRC32_INITIAL,"tRNS\0",5)); /* 0x40e6d866 */
      out.vi_write("\0\0\0\1bKGD\0\210\5\35H", 13); /* color 0 is background color */
      //out.vi_write("\0\0\0\1bKGD\2\x66\x0b\x7c\x64", 13); /* color 2 is background color */
      //fprintf(stderr, "bKGD: 0x%08lx", (unsigned long)crc32(CRC32_INITIAL,"bKGD\2",5)); /* 0x88051d48 */
    }
  }
  
  slen_t rlenht=img->getRlen()*img->getHt();
  #if 0 /* Calculates _bad_ CRC (before compression etc.) */
    p=tmp;
    *p++=rlenht>>24; *p++=rlenht>>16; *p++=rlenht>>8; *p++=rlenht; /* 0: IDAT chunk length */
    *p++='I'; *p++='D'; *p++='A'; *p++='T'; /* 4: IDAT chunk name */
    out.vi_write(tmp, 8);
    if (rlenht!=0) pp->vi_write(img->getRowbeg(), rlenht);
    pp->vi_write(0,0); /* flush all, write CRC */
    crc=crc32(crc32(CRC32_INITIAL,tmp+4,4), img->getRowbeg(), rlenht);
    p=tmp; *p++=crc>>24;*p++=crc>>16;*p++=crc>>8;*p++=crc; /* 0: IDAT CRC */
    out.vi_write(tmp, 4);
  #else
    rp->vi_write("IDAT",4);
    if (rlenht!=0) pp->vi_write(img->getRowbeg(), rlenht);
    pp->vi_write(0,0); /* flush all, write CRC */
  #endif
  
  out.vi_write("\0\0\0\0IEND\256B`\202",12); /* IEND chunk */
  delete pp;
  delete cp;
  delete rp;

  return Rule::Applier::OK;
}

Rule::Applier out_png_applier = { "PNG", out_png_check_rule, out_png_work, 0 };

/* --- Sat Jun 15 19:30:41 CEST 2002 */

/** BMP RLE compression, type 1. Seems to be optimal.
 * @param dst buffer with enough place for compressed data. The caller must
 *        pre-allocate >=(send-sbeg)+(send-sbeg+128)*2/255.
 * @param sbeg first raw data char to compress
 * @param send char to finish compression just before
 * @return dst+(number of characters occupied by compressed data)
 *
 * in-memory implementation Sun Jun 23 20:02:41 CEST 2002
 */

static char *bmp_compress1_row(char *dst, char const *sbeg, char const *send) {
  #if 0
  #  define BMP_ASSERT(x) assert(x)
  #else
  #  define BMP_ASSERT(x)
  #endif
  #undef  BUF
  #define BUF(x) (*(x))
  #undef  PUTCH__
  #define PUTCH__(c) (*dst++=(c))
  char c, c2;
  char const *beg, *end, *rend, *q, *r, *best;
  signed bestca, rca; /* both must fit into -255..255 */
  slen_t frl, efrl;
  int ci;
  // bool oddp;
  
  beg=sbeg;
  end=(send-sbeg>255) ? beg+255 : send;

  while (beg!=end) { /* there is still unprocessed data in the buffer */
    c=BUF(beg++);
    if (beg==end) { PUTCH__(1); PUTCH__(c); break; } /* last char */
    if (c==BUF(beg)) { /* sure r chunk */
      ci=2; beg++;
     rep:
      while (beg!=end && c==BUF(beg)) { beg++; ci++; }
      PUTCH__(ci); PUTCH__(c); /* r chunk */
    } else { /* possible c chunk */
      rend=end;
      BMP_ASSERT(end-beg<=254);
      if (end!=send) { /* read an extra char as the terminator of the last run-length of c, buf:0..254 */
        end++;
        BMP_ASSERT(end-beg==255); /* buffer is full (255 chars) */
      }
      
      best=r=beg;
      bestca=rca=-1; /* best and current advantage of c over r */
      
      while (r!=rend) { /* c chunk should stop only at run-length boundaries */
        BMP_ASSERT(-255<=rca && rca<=255);
        BMP_ASSERT(-255<=bestca && bestca<=255);
        q=r; r=q+1; ci=1; while (r!=end && BUF(r)==BUF(q)) { r++; ci++; }
        if (r==end && end!=rend) break;
        if (((r-beg)&1)==0) { /* odd (!) copy length */
          rca+=3-ci;
          if (rca<=bestca-1) { r--; break; } /* fix-5 (instead of rule-4): `xyz|bbbbb|xyz|', `abcdef|gggggggg|abababababab|', `abcdef|ggg|hhh|ggg|hhh|ggg|hhh|ggg|hhh|abababababab|' */
          if (bestca<rca) { bestca=rca; best=r-1; } /* make c as short as possible */
          rca--;
        } else { /* even copy length */
          rca+=2-ci;
          if (rca<=bestca-2) break; /* fix-5 (instead of rule-4): `xyz|bbbbb|xyz|', `abcdef|gggggggg|abababababab|', `abcdef|ggg|hhh|ggg|hhh|ggg|hhh|ggg|hhh|abababababab|' */
          if (bestca<rca) { bestca=rca; best=r; } /* make c as short as possible */
        }
      }
      BMP_ASSERT(-255<=rca && rca<=255);
      BMP_ASSERT(-255<=bestca && bestca<=255);
      if (bestca<=0 /* no possible positive advantage */
       || best-beg<=1  /* 1: c is one char, plus 1 char in buf.  Imp: ==1?! */
         ) { ci=1; goto rep; }
      r=best; /* Imp: get rid of this assignment */
      BMP_ASSERT(beg!=r);
      BMP_ASSERT(((r-beg)&1)==1); /* even copy length */

      if (end==r) { /* no followers, last chunk */
        /* BMP_ASSERT(had_eof); */
        // oddp=(1+(r-beg)&1)==1;
        PUTCH__(0);
        PUTCH__((r-beg)+1);
        PUTCH__(c);
        while (beg!=r) { PUTCH__(BUF(beg)); beg++; } /* emit c chunk */
        // if (oddp) PUTCH__(0); /* Imp: padding breaks optimality */
      } else {
        BMP_ASSERT(r!=end);
        /* BMP_ASSERT(r!=rend); */ /* r==rend is possible here */
        c2=BUF(r); frl=1; q=r+1;
        while (q!=end && c2==BUF(q)) { q++; frl++; } /* count follower run length */
        efrl=frl; ci=-2; if (q==end) { /* Imp: get rid of -2 (-2 -> -1) */
          BMP_ASSERT(q==end);
          while ((ci=(q==send)?-1:(unsigned char)*q++)!=-1 && (char)ci==c2) efrl++;
        }

        /* printf("clen=%u\n", clen); */
        if (1+(r>beg ? r-beg : 256+beg-r)<255 && efrl>=256 && efrl%255==1) { r++; efrl--; } /* make the c chunk one char longer if appropriate */

        // oddp=(1+(r-beg)&1)==1;
        PUTCH__(0);
        PUTCH__(1+(r-beg));
        PUTCH__(c);
        while (beg!=r) { PUTCH__(BUF(beg)); beg++; } /* emit c chunk */
        // if (oddp) PUTCH__(0); /* Imp: padding breaks optimality */

        beg=q; /* remove beginning of the r chunk from the buffer */
        if (ci>=0) { beg--; BMP_ASSERT((unsigned char)BUF(beg)==ci); }
        
        while (efrl>=255) { PUTCH__('\377'); PUTCH__(c2); efrl-=255; } /* emit full r chunks */
        if (efrl>=2) { /* emit last r chunk */
          PUTCH__(efrl); PUTCH__(c2);
        } else if (efrl!=0) {
          BMP_ASSERT(efrl==1);
          beg--; /* leave a single instance of c2 in beginning of the buffer */
          BMP_ASSERT(BUF(beg)==c2);
        }
      } /* IF c chunk has followers */
    } /* IF r or c chunk */
    end=(send-beg>255) ? beg+255 : send;
  } /* WHILE main loop */
  return dst;
  #undef  BUF
  #undef  PUTCH__
}


/** Windows Bitmap BMP output */
Rule::Applier::cons_t out_bmp_check_rule(Rule::OutputRule* or_) {
  /* Supported BMP types: /Rgb8,
   *   /Indexed1, /Indexed4, /Indexed8. 
   */
  Rule::Cache *cache=&or_->cache;
  if (cache->FileFormat!=cache->FF_BMP
     ) return Rule::Applier::DONT_KNOW;
  bool badp=false;
  /* Dat: (by policy) we don't support /Transparent or /Opaque here; the
   * user should not specify such SampleFormat in the .job file.
   */
  if (cache->SampleFormat!=Image::SF_Indexed1
      /* Dat: /Indexed2 is not supported by /BMP */
   && cache->SampleFormat!=Image::SF_Indexed4
   && cache->SampleFormat!=Image::SF_Indexed8
   && cache->SampleFormat!=Image::SF_Rgb8) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /BMP requires /Indexed1, /Indexed4, /Indexed8 or /Rgb8" << (Error*)0;
    badp=true;
  }
  if (cache->TransferEncoding!=cache->TE_Binary) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /BMP requires /Binary" << (Error*)0;
    badp=true;
  }
  if (cache->Compression!=Rule::Cache::CO_None && cache->Compression!=Rule::Cache::CO_RLE) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /BMP requires /Compression/None or /RLE" << (Error*)0;
    badp=true;
  }
  if (cache->Compression==Rule::Cache::CO_RLE && cache->SampleFormat!=Image::SF_Indexed8) {
    /* !! Imp: Implement compr==2 for /Indexed4 */
    Error::sev(Error::WARNING_DEFER) << "check_rule: /BMP/RLE requires /Indexed8" << (Error*)0;
    badp=true;
  }
  if (cache->hasPredictor()) {
    Error::sev(Error::WARNING_DEFER) << "check_rule: /BMP requires /Predictor 1" << (Error*)0;
    badp=true;
  }
  if (badp) return Rule::Applier::BAD;
  /* Now we are sure about Rule::Applier::OK. */
  return Rule::Applier::OK;
}
Rule::Applier::cons_t out_bmp_work(GenBuffer::Writable& out, Rule::OutputRule*or_, Image::SampledInfo *sf) {
  if (out_bmp_check_rule(or_)!=Rule::Applier::OK) return Rule::Applier::DONT_KNOW;
  or_->doSampleFormat(sf);
  Image::Sampled *img=sf->getImg();
  Image::Sampled::dimen_t wd=img->getWd(), ht=img->getHt(), htc=ht;
  slen_t rlen=img->getRlen();
  char const*pend=img->getRowbeg()+rlen*ht;
  char const*palbeg=img->getHeadp(), *palend=img->getRowbeg();
  slen_t ncols4=(palend-palbeg)/3*4;
  unsigned biCompr=or_->cache.Compression!=or_->cache.CO_RLE ? 0
   : or_->cache.SampleFormat==Image::SF_Indexed4 ? 2 : 1;
  
  SimBuffer::B data;
  slen_t crowsize=2+ rlen+(rlen+128)*2/255; /* !! Imp: real upper bound? */
  char *crow=new char[crowsize];
  /* !! GIMP compatibility */
  if (or_->cache.Compression==or_->cache.CO_RLE) {
    /* Imp: bmp_compress2_row */
    char *crow2;
    while (htc--!=0) { /* BMP stores rows from down */
      crow2=bmp_compress1_row(crow, pend-rlen, pend);
#if 0
      crow2=crow;
      *crow2++=10;
      *crow2++=1;
      *crow2++=10;
      *crow2++=2;
      *crow2++=10;
      *crow2++=3;
      *crow2++=10;
      *crow2++=4;
      *crow2++=10;
      *crow2++=5;
      *crow2++=10;
      *crow2++=6;
#endif
      
      assert((slen_t)(crow2-crow)<=crowsize-2);
      pend-=rlen;
      *crow2++='\0'; *crow2++='\0'; /* signal end of compressed data row */
      /* fprintf(stderr, "htc=%u cl=%u\n", htc, crow2-crow); */
      data.vi_write(crow, crow2-crow);
    }
    data.vi_write("\0\1", 2); /* signal end of bitmap */
  } else {
    unsigned pad=(4-(rlen&3))&3; /* Dat: pad rows to 32 bits */
    if (or_->cache.SampleFormat==Image::SF_Rgb8) { /* SWAP RGB values */
      /* BUGFIX at Thu Dec 12 21:36:57 CET 2002 */
      char *buf=new char[rlen];
      while (htc--!=0) { /* BMP stores rows from down */
        char const*pxend=pend, *px=pend-=rlen;
        char *q=buf;
        while (px!=pxend) {
          *q++=px[2];
          *q++=px[1];
          *q++=px[3];
          px+=3;
        }          
        data.vi_write(buf, rlen);
        if (pad!=0) data.vi_write("\0\0\0", pad);
      }
      delete [] buf;
    } else {
      while (htc--!=0) { /* BMP stores rows from down */
        data.vi_write(pend-=rlen, rlen);
        if (pad!=0) data.vi_write("\0\0\0", pad);
      }
    }
  }
  assert(pend==img->getRowbeg());
  delete [] crow;
  /* Now data is ready. */

  char *bmphead=new char[54+ncols4+data.getLength()], *p=bmphead;
  *p++='B'; *p++='M'; /* magic number header */
  lf32(p, 54+ncols4+data.getLength()); /* bfSize */
  lf32(p, 0); /* zzHotX, zzHotY */
  lf32(p, 54+ncols4); /* bfOffs */
  lf32(p, 40); /* biSize==40 => Windows 3.x style BMP file */
  lf32(p, wd); /* biWidth */
  lf32(p, ht); /* biHeight */
  lf16(p, 1);  /* biPlanes, must be 1 */
  lf16(p, img->getBpc()*img->getCpp());  /* biBitCnt: bits per pixel (1, 4, 8: /Indexed*; 24: /Rgb8) */
  lf32(p, biCompr); /* biCompr */
  lf32(p, 0); /* biSizeIm */
  lf32(p, 0); /* biXPels */
  lf32(p, 0); /* biYPels */
  lf32(p, (palend-palbeg)/3); /* biClrUsed */
  lf32(p, (palend-palbeg)/3); /* biClrImp */
  /* vvv Now comes the palette (zero length for /Rgb) */
  while (palbeg!=palend) {
    *p++=palbeg[2];  *p++=palbeg[1];  *p++=palbeg[0];  *p++='\0';
    palbeg+=3;
  }
  assert((slen_t)(p-bmphead)==54+ncols4);
  
  out.vi_write(bmphead, p-bmphead);  delete [] bmphead;
#if 0
  out.vi_write("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    100);
  out.vi_write("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    100);
  out.vi_write("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    100);
  out.vi_write("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    100);
  out.vi_write("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    100);
#endif
  out << data;
  out.vi_write(0,0); /* signal EOF */

  return Rule::Applier::OK;
}

Rule::Applier out_bmp_applier = { "BMP", out_bmp_check_rule, out_bmp_work, 0 };

/* --- Sun Mar 24 13:48:57 CET 2002 */

/** The Empty applier produces an empty output file. */
Rule::Applier::cons_t out_empty_check_rule(Rule::OutputRule* or_) {
  Rule::Cache *cache=&or_->cache;
  if (cache->FileFormat!=cache->FF_Empty
     ) return Rule::Applier::DONT_KNOW;
  return Rule::Applier::OK;
}
Rule::Applier::cons_t out_empty_work(GenBuffer::Writable& out, Rule::OutputRule*or_, Image::SampledInfo *sf) {
  (void)out;
  (void)sf;
  if (out_empty_check_rule(or_)!=Rule::Applier::OK) return Rule::Applier::DONT_KNOW;
  return Rule::Applier::OK;
}

Rule::Applier out_empty_applier = { "Empty", out_empty_check_rule, out_empty_work, 0 };

/* --- Sun Mar 24 13:48:57 CET 2002 */

/** The Meta applier writes some meta-information into the output file */
Rule::Applier::cons_t out_meta_check_rule(Rule::OutputRule* or_) {
  Rule::Cache *cache=&or_->cache;
  if (cache->FileFormat!=cache->FF_Meta
     ) return Rule::Applier::DONT_KNOW;
  // return Rule::Applier::OK; /* Imp: implement the applier */
  return Rule::Applier::BAD;
}
Rule::Applier::cons_t out_meta_work(GenBuffer::Writable& out, Rule::OutputRule*or_, Image::SampledInfo *sf) {
  (void)out;
  (void)sf;
  if (out_meta_check_rule(or_)!=Rule::Applier::OK) return Rule::Applier::DONT_KNOW;
  /* Imp: real content here */
  return Rule::Applier::OK;
}

Rule::Applier out_meta_applier = { "Meta", out_meta_check_rule, out_meta_work, 0 };

/* __END__ */
