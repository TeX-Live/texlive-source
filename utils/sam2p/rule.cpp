/*
 * rule.cpp -- generic Rule handling
 * by pts@fazekas.hu at Fri Mar 15 21:13:47 CET 2002
 */

#ifdef __GNUC__
#pragma implementation
#endif

#include "rule.hpp"
#include "error.hpp"
#include <string.h> /* strlen() */

bool Rule::Cache::isPDF() const {
  return 80<=FileFormat && FileFormat<100;
}
bool Rule::Cache::isPDFB() const {
  return 90<=FileFormat && FileFormat<100;
}
bool Rule::Cache::isIndexed() const {
  return SampleFormat==Image::SF_Indexed1
      || SampleFormat==Image::SF_Indexed2
      || SampleFormat==Image::SF_Indexed4
      || SampleFormat==Image::SF_Indexed8;
}
bool Rule::Cache::isTransparentM() const {
  return SampleFormat==Image::SF_Mask
      || SampleFormat==Image::SF_Transparent2
      || SampleFormat==Image::SF_Transparent4
      || SampleFormat==Image::SF_Transparent8;
}
bool Rule::Cache::isOneBit() const {
  return SampleFormat==Image::SF_Mask
      || SampleFormat==Image::SF_Indexed1
      || SampleFormat==Image::SF_Gray1;
}
#if 0
bool Rule::Cache::is8() const {
  return SampleFormat==Image::SF_Indexed1
      || SampleFormat==Image::SF_Indexed2
      || SampleFormat==Image::SF_Indexed4
      || SampleFormat==Image::SF_Indexed8;
}
#endif
bool Rule::Cache::isGray() const {
  return SampleFormat==Image::SF_Gray1
      || SampleFormat==Image::SF_Gray2
      || SampleFormat==Image::SF_Gray4
      || SampleFormat==Image::SF_Gray8;
}
bool Rule::Cache::isRGB() const {
  return SampleFormat==Image::SF_Rgb1
      || SampleFormat==Image::SF_Rgb2
      || SampleFormat==Image::SF_Rgb4
      || SampleFormat==Image::SF_Rgb8;
}
bool Rule::Cache::isBinSB() const {
  return TransferEncoding==TE_Binary
      || TransferEncoding==TE_MSBfirst
      || TransferEncoding==TE_LSBfirst;
}
bool Rule::Cache::isPS() const {
  return FileFormat>=100;
}
bool Rule::Cache::isPSL2() const {
  return FileFormat>=120;
}
bool Rule::Cache::isPSL3() const {
  return FileFormat>=130;
}
bool Rule::Cache::hasPredictor() const {
  return Predictor!=PR_None;
}
bool Rule::Cache::isDCTE() const {
  /* Dat: _not_ CO_JAI */
  return Compression==CO_DCT || Compression==CO_IJG;
}
bool Rule::Cache::isZIPOK() const {
  return Compression!=CO_ZIP ||
       ( FileFormat!=FF_PSL1 && FileFormat!=FF_PSLC && FileFormat!=FF_PSL2
      && FileFormat!=FF_PDF10 && FileFormat!=FF_PDFB10
       );
}

static MiniPS::Dict *y_FileFormat=(MiniPS::Dict*)NULLP,
  *y_SampleFormat, *y_TransferEncoding,
  *y_Compression, *y_Scale;
static const slen_t SampleFormat_MAXLEN=32;

static void init_dicts() {
  register MiniPS::Dict*y;
 
  if (y_FileFormat!=NULLP) return;

  y=y_FileFormat=new MiniPS::Dict();
  y->put("/PSL1",   MiniPS::Qinteger(Rule::Cache::FF_PSL1));
  y->put("/PSLC",   MiniPS::Qinteger(Rule::Cache::FF_PSLC));
  y->put("/PSL2",   MiniPS::Qinteger(Rule::Cache::FF_PSL2));
  y->put("/PSL3",   MiniPS::Qinteger(Rule::Cache::FF_PSL3));
  y->put("/PDFB1.0",MiniPS::Qinteger(Rule::Cache::FF_PDFB10));
  y->put("/PDFB1.2",MiniPS::Qinteger(Rule::Cache::FF_PDFB12));
  y->put("/PDF1.0", MiniPS::Qinteger(Rule::Cache::FF_PDF10));
  y->put("/PDF1.2", MiniPS::Qinteger(Rule::Cache::FF_PDF12));
  y->put("/GIF89a", MiniPS::Qinteger(Rule::Cache::FF_GIF89a));
  y->put("/GIF", MiniPS::Qinteger(Rule::Cache::FF_GIF89a));
  y->put("/Empty", MiniPS::Qinteger(Rule::Cache::FF_Empty));
  y->put("/Meta", MiniPS::Qinteger(Rule::Cache::FF_Meta));
  y->put("/PNM", MiniPS::Qinteger(Rule::Cache::FF_PNM));
  y->put("/PAM", MiniPS::Qinteger(Rule::Cache::FF_PAM));
  y->put("/PIP", MiniPS::Qinteger(Rule::Cache::FF_PIP));
  y->put("/JPEG", MiniPS::Qinteger(Rule::Cache::FF_JPEG));
  y->put("/JPG", MiniPS::Qinteger(Rule::Cache::FF_JPEG));
  y->put("/TIFF", MiniPS::Qinteger(Rule::Cache::FF_TIFF));
  y->put("/TIF", MiniPS::Qinteger(Rule::Cache::FF_TIFF));
  y->put("/PNG", MiniPS::Qinteger(Rule::Cache::FF_PNG));
  y->put("/XPM", MiniPS::Qinteger(Rule::Cache::FF_XPM));
  y->put("/BMP", MiniPS::Qinteger(Rule::Cache::FF_BMP));
  y->put("/XWD", MiniPS::Qinteger(Rule::Cache::FF_XWD));
  y->put("/X11", MiniPS::Qinteger(Rule::Cache::FF_X11));

  /* vvv strlen must be shorter then SampleFormat_MAXLEN */
  y=y_SampleFormat=new MiniPS::Dict();
  y->put("/Opaque", MiniPS::Qinteger(Image::SF_Opaque));
  y->put("/Transparent", MiniPS::Qinteger(Image::SF_Transparent));
  y->put("/Transparent2", MiniPS::Qinteger(Image::SF_Transparent2));
  y->put("/Transparent4", MiniPS::Qinteger(Image::SF_Transparent4));
  y->put("/Transparent8", MiniPS::Qinteger(Image::SF_Transparent8));
  y->put("/Gray1", MiniPS::Qinteger(Image::SF_Gray1));
  y->put("/Gray2", MiniPS::Qinteger(Image::SF_Gray2));
  y->put("/Gray4", MiniPS::Qinteger(Image::SF_Gray4));
  y->put("/Gray8", MiniPS::Qinteger(Image::SF_Gray8));
  y->put("/Indexed1", MiniPS::Qinteger(Image::SF_Indexed1));
  y->put("/Indexed2", MiniPS::Qinteger(Image::SF_Indexed2));
  y->put("/Indexed4", MiniPS::Qinteger(Image::SF_Indexed4));
  y->put("/Indexed8", MiniPS::Qinteger(Image::SF_Indexed8));
  y->put("/Mask", MiniPS::Qinteger(Image::SF_Mask));
  y->put("/Rgb1", MiniPS::Qinteger(Image::SF_Rgb1)); /* recommended */
  y->put("/Rgb2", MiniPS::Qinteger(Image::SF_Rgb2));
  y->put("/Rgb4", MiniPS::Qinteger(Image::SF_Rgb4));
  y->put("/Rgb8", MiniPS::Qinteger(Image::SF_Rgb8));
  y->put("/RGB1", MiniPS::Qinteger(Image::SF_Rgb1)); /* obsolete */
  y->put("/RGB2", MiniPS::Qinteger(Image::SF_Rgb2));
  y->put("/RGB4", MiniPS::Qinteger(Image::SF_Rgb4));
  y->put("/RGB8", MiniPS::Qinteger(Image::SF_Rgb8));
  y->put("/Asis", MiniPS::Qinteger(Image::SF_Asis));
  y->put("/Bbox", MiniPS::Qinteger(Image::SF_Bbox));

  y=y_TransferEncoding=new MiniPS::Dict();
  y->put("/Binary", MiniPS::Qinteger(Rule::Cache::TE_Binary));
  y->put("/ASCII", MiniPS::Qinteger(Rule::Cache::TE_ASCII));
  y->put("/Hex", MiniPS::Qinteger(Rule::Cache::TE_Hex)); /* recommended */
  y->put("/AHx", MiniPS::Qinteger(Rule::Cache::TE_Hex));
  y->put("/ASCIIHex", MiniPS::Qinteger(Rule::Cache::TE_Hex));
  y->put("/A85", MiniPS::Qinteger(Rule::Cache::TE_A85)); /* recommended */
  y->put("/ASCII85", MiniPS::Qinteger(Rule::Cache::TE_A85));
  y->put("/MSBfirst", MiniPS::Qinteger(Rule::Cache::TE_MSBfirst));
  y->put("/LSBfirst", MiniPS::Qinteger(Rule::Cache::TE_LSBfirst));

  y=y_Compression=new MiniPS::Dict();
  y->put("/ ", MiniPS::Qinteger(Rule::Cache::CO_None)); /* default */
  y->put("/None", MiniPS::Qinteger(Rule::Cache::CO_None)); /* recommended */
  y->put("/LZW", MiniPS::Qinteger(Rule::Cache::CO_LZW)); /* recommended */
  y->put("/ZIP", MiniPS::Qinteger(Rule::Cache::CO_ZIP)); /* recommended */
  y->put("/Flate", MiniPS::Qinteger(Rule::Cache::CO_ZIP));
  y->put("/Fl", MiniPS::Qinteger(Rule::Cache::CO_ZIP));
  y->put("/RLE", MiniPS::Qinteger(Rule::Cache::CO_RLE)); /* recommended */
  y->put("/RunLength", MiniPS::Qinteger(Rule::Cache::CO_RLE));
  y->put("/RunLengthEncoded", MiniPS::Qinteger(Rule::Cache::CO_RLE));
  y->put("/RL", MiniPS::Qinteger(Rule::Cache::CO_RLE));
  y->put("/PackBits", MiniPS::Qinteger(Rule::Cache::CO_RLE));
  y->put("/Fax", MiniPS::Qinteger(Rule::Cache::CO_Fax)); /* recommended */
  y->put("/CCITTFax", MiniPS::Qinteger(Rule::Cache::CO_Fax));
  y->put("/CCF", MiniPS::Qinteger(Rule::Cache::CO_Fax));
  y->put("/DCT", MiniPS::Qinteger(Rule::Cache::CO_DCT)); /* recommended */
  y->put("/JPEG",MiniPS::Qinteger(Rule::Cache::CO_IJG)); /* changed at Sun Jun 23 17:06:34 CEST 2002 */
  y->put("/JPG", MiniPS::Qinteger(Rule::Cache::CO_IJG)); /* changed at Sun Jun 23 17:06:34 CEST 2002 */
  y->put("/JFIF",MiniPS::Qinteger(Rule::Cache::CO_IJG)); /* changed at Sun Jun 23 17:06:34 CEST 2002 */
  y->put("/IJG", MiniPS::Qinteger(Rule::Cache::CO_IJG)); /* recommended */
  y->put("/JAI", MiniPS::Qinteger(Rule::Cache::CO_JAI)); /* recommended */

  y=y_Scale=new MiniPS::Dict();
  y->put("/ ",       MiniPS::Qinteger(Rule::CacheHints::SC_None)); /* default */
  y->put("/None",    MiniPS::Qinteger(Rule::CacheHints::SC_None));
  y->put("/OK",      MiniPS::Qinteger(Rule::CacheHints::SC_OK));
  y->put("/RotateOK",MiniPS::Qinteger(Rule::CacheHints::SC_RotateOK));
}

Image::sf_t Rule::Cache::parseSampleFormat(char const*s, slen_t slen) {
  /* Tue Jul  2 13:48:12 CEST 2002 */
  #if 0 /* BUGFIX for g++-3.4 (needs symbols __cxa_guard_acquire, __cxa_guard_release) */
    static char rule_dummy=(init_dicts(),0); /* call once per process */
    (void)rule_dummy;
  #else
    if (y_FileFormat==NULLP) init_dicts();
  #endif
  while (slen!=0 && s[0]=='/') { s++; slen--; }
  if (slen<=0 || slen>=SampleFormat_MAXLEN) return Image::SF_max;
  char buf[SampleFormat_MAXLEN];
  GenBuffer::tolower_memcpy(buf, s, slen);
  if (buf[0]>='a' && buf[0]<='z') buf[0]+='A'-'a'; /* capitalize 1st letter */
  MiniPS::VALUE v=y_SampleFormat->get(buf, slen);
  return (Image::sf_t) (v==MiniPS::Qundef ? Image::SF_max : MiniPS::int2ii(v));
}

char const* Rule::Cache::dumpFileFormat(ff_t FileFormat, co_t Compression) {
  switch (FileFormat) {
   case FF_eps:   return Compression==CO_ZIP ? "PSL3" : "PSL2";
   case FF_pdf:   return Compression==CO_ZIP ? "PDF1.2" : "PDF1.0";
   case FF_pdfb:  return Compression==CO_ZIP ? "PDFB1.2" : "PDFB1.0";
   case FF_PSL1:  return "PSL1";
   case FF_PSLC:  return "PSLC";
   case FF_PSL2:  return "PSL2";
   case FF_PSL3:  return "PSL3";
   case FF_PDFB10:return "PDFB1.0";
   case FF_PDFB12:return "PDFB1.2";
   case FF_PDF10: return "PDF1.0";
   case FF_PDF12: return "PDF1.2";
   case FF_GIF89a:return "GIF89a";
   case FF_PNM:   return "PNM";
   case FF_PAM:   return "PAM";
   case FF_PIP:   return "PIP";
   case FF_Empty: return "Empty";
   case FF_Meta:  return "Meta";
   case FF_JPEG:  return "JPEG";
   case FF_TIFF:  return "TIFF";
   case FF_PNG:   return "PNG";
   case FF_XPM:   return "XPM";
   case FF_BMP:   return "BMP";
   case FF_XWD:   return "XWD";
   case FF_X11:   return "X11";
  }
  return (char const*)NULLP;
}
char const* Rule::Cache::dumpTransferEncoding(te_t TransferEncoding) {
  switch (TransferEncoding) {
   case TE_Binary:  return "Binary";
   case TE_ASCII:   return "ASCII";
   case TE_Hex:     return "Hex";
   case TE_A85:     return "A85";
   case TE_MSBfirst:return "MSBfirst";
   case TE_LSBfirst:return "LSBfirst";
  }
  return (char const*)NULLP;
}
char const* Rule::Cache::dumpSampleFormat(Image::sf_t SampleFormat) {
  switch (SampleFormat) {
   case Image::SF_Opaque: return "Opaque";
   case Image::SF_Transparent: return "Transparent";
   case Image::SF_Gray1: return "Gray1";
   case Image::SF_Indexed1: return "Indexed1";
   case Image::SF_Mask: return "Mask";
   case Image::SF_Transparent2: return "Transparent2";
   case Image::SF_Gray2: return "Gray2";
   case Image::SF_Indexed2: return "Indexed2";
   case Image::SF_Transparent4: return "Transparent4";
   case Image::SF_Rgb1: return "Rgb1";
   case Image::SF_Gray4: return "Gray4";
   case Image::SF_Indexed4: return "Indexed4";
   case Image::SF_Transparent8: return "Transparent8";
   case Image::SF_Rgb2: return "Rgb2";
   case Image::SF_Gray8: return "Gray8";
   case Image::SF_Indexed8: return "Indexed8";
   case Image::SF_Rgb4: return "Rgb4";
   case Image::SF_Rgb8: return "Rgb8";
   case Image::SF_Asis: return "Asis";
   case Image::SF_Bbox: return "Bbox";
  }
  return (char const*)NULLP;
}
char const* Rule::Cache::dumpCompression(co_t Compression) {
  switch (Compression) {
   case CO_None:return "None";
   case CO_LZW: return "LZW";
   case CO_ZIP: return "ZIP";
   case CO_RLE: return "RLE";
   case CO_Fax: return "Fax";
   case CO_DCT: return "DCT";
   case CO_IJG: return "IJG";
   case CO_JAI: return "JAI";
  }
  return (char const*)NULLP;
}

char const* Rule::CacheHints::dumpScale(sc_t Scale) {
  switch (Scale) {
   case SC_None:    return "None";
   case SC_OK:      return "OK";
   case SC_RotateOK:return "RotateOK";
  }
  return (char const*)NULLP;
}

/* Checks and recovers Predictor. Called by scanf_dict.
 * @return Qundef if param invalid
 *         Qinteger(1) if param==Qundef
 *         valid Qinteger otherwise
 */
static MiniPS::VALUE better_predictor(MiniPS::VALUE v) {
  return v==MiniPS::Qundef ? MiniPS::Qinteger(1) /* PR_None */
       : v==MiniPS::Qinteger(1) || v==MiniPS::Qinteger(2) || v==MiniPS::Qinteger(45) ||
         ((v&1)!=0 && v>=MiniPS::Qinteger(10) && v<=MiniPS::Qinteger(15)) ? v
       : MiniPS::Qundef;
}
static MiniPS::VALUE better_xColors(MiniPS::VALUE v) {
  return v==MiniPS::Qundef ? MiniPS::Qinteger(0)
       : ((v&1)!=0 && v>=MiniPS::Qinteger(1) && v<=MiniPS::Qinteger(4)) ? v
       : MiniPS::Qundef;
}
static MiniPS::VALUE better_predictorBPC(MiniPS::VALUE v) {
  return v==MiniPS::Qundef ? MiniPS::Qinteger(0)
       : v==MiniPS::Qinteger(1) || v==MiniPS::Qinteger(2) ||
         v==MiniPS::Qinteger(4) || v==MiniPS::Qinteger(8) ? v
       : MiniPS::Qundef;
}
static MiniPS::VALUE better_effort(MiniPS::VALUE v) {
  return v==MiniPS::Qundef ? MiniPS::Qinteger(-1)
       : ((v&1)!=0 && v>=MiniPS::Qinteger(-1) && v<=MiniPS::Qinteger(9)) ? v
       : MiniPS::Qundef;
}
static MiniPS::VALUE better_k(MiniPS::VALUE v) {
  return v==MiniPS::Qundef ? MiniPS::Qinteger(0)
       : ((v&1)!=0 && v>=MiniPS::Qinteger(-2)) ? v /* -2 means: positive value marking the image height */
       : MiniPS::Qundef;
}
static MiniPS::VALUE better_quality(MiniPS::VALUE v) {
  return v==MiniPS::Qundef ? MiniPS::Qinteger(75)
       : ((v&1)!=0 && v>=MiniPS::Qinteger(0) && v<=MiniPS::Qinteger(100)) ? v
       : MiniPS::Qundef;
}
static MiniPS::VALUE better_colorTransform(MiniPS::VALUE v) {
  return v==MiniPS::Qundef ? MiniPS::Qinteger(3)
       : ((v&1)!=0 && v>=MiniPS::Qinteger(0) && v<=MiniPS::Qinteger(3)) ? v
       : MiniPS::Qundef;
}

void Rule::OutputRule::fromDict(MiniPS::VALUE dict_) {
  #if 0 /* BUGFIX for g++-3.4 (needs symbols __cxa_guard_acquire, __cxa_guard_release) */
    static char rule_dummy=(init_dicts(),0); /* call once per process */
    (void)rule_dummy;
  #else
    if (y_FileFormat==NULLP) init_dicts();
  #endif
  MiniPS::VALUE dummy;  (void)dummy;
  MiniPS::VALUE FileFormat, SampleFormat, WarningOK, TransferEncoding,
    Compression, Predictor, Transparent;
  MiniPS::VALUE PredictorColumns, PredictorColors, PredictorBPC, Effort, K,
    RecordSize, Quality, ColorTransform, TransferCPL,
    EncoderRows, EncoderColumns, EncoderBPL, EncoderColors, DCT, Scale;

  MiniPS::scanf_dict(dict_, /*show_warnings:*/true,
    "FileFormat",      MiniPS::S_SENUM,   y_FileFormat,       &FileFormat,
    "SampleFormat",    MiniPS::S_SENUM,   y_SampleFormat,     &SampleFormat,
    "WarningOK",       MiniPS::T_BOOLEAN, MiniPS::Qtrue,      &WarningOK,
    "TransferEncoding",MiniPS::S_SENUM,   y_TransferEncoding, &TransferEncoding,
    "Compression",     MiniPS::S_SENUM,   y_Compression,      &Compression,
    "Predictor",       MiniPS::S_FUNC,    better_predictor,   &Predictor,
    "Transparent",     MiniPS::S_RGBSTR,  MiniPS::Qnull,      &Transparent, /* force an RGB color transparent */
    "Hints",           MiniPS::T_DICT,    MiniPS::Qnull,      &dictHints,
    NULLP
  );
  dict=(MiniPS::Dict*)dict_;
  cache.FileFormat=(Rule::Cache::ff_t)MiniPS::int2ii(FileFormat);
  cache.SampleFormat=(Image::sf_t)MiniPS::int2ii(SampleFormat);
  cache.WarningOK=(Rule::Cache::ff_t)WarningOK==MiniPS::Qtrue;
  cache.TransferEncoding=(Rule::Cache::te_t)MiniPS::int2ii(TransferEncoding);
  cache.Compression=(Rule::Cache::co_t)MiniPS::int2ii(Compression);
  cache.Predictor=(Rule::Cache::pr_t)MiniPS::int2ii(Predictor);
  cache.Transparent=0x1000000UL; /* No extra transparency */
  if (Transparent!=MiniPS::Qnull) {
    unsigned char const*p=(unsigned char const*)(MiniPS::RSTRING(Transparent)->begin_());
    cache.Transparent=(p[0]<<16)+(p[1]<<8)+p[2];
  }
  //MiniPS::dump(Predictor);
  // fprintf(stderr,"cpred=%u\n", cache.Predictor);
  
  if ((MiniPS::VALUE)dictHints==MiniPS::Qnull) dict->put("/Hints", (MiniPS::VALUE)(dictHints=new MiniPS::Dict()));
  MiniPS::scanf_dict((MiniPS::VALUE)dictHints, /*show_warnings:*/true,
    "EncoderBPL",      MiniPS::S_PINTEGER,MiniPS::Qinteger(0),   &EncoderBPL,
    "EncoderColumns",  MiniPS::S_PINTEGER,MiniPS::Qinteger(0),   &EncoderColumns,
    "EncoderRows",     MiniPS::S_PINTEGER,MiniPS::Qinteger(0),   &EncoderRows,
    "EncoderColors",   MiniPS::S_FUNC,    better_xColors,        &EncoderColors,
    "PredictorColumns",MiniPS::S_PINTEGER,MiniPS::Qinteger(0),   &PredictorColumns,
    "PredictorColors", MiniPS::S_FUNC,    better_xColors,        &PredictorColors,
    "PredictorBPC",    MiniPS::S_FUNC,    better_predictorBPC,   &PredictorBPC,
    "Effort",          MiniPS::S_FUNC,    better_effort,         &Effort,
    "RecordSize",      MiniPS::S_UINTEGER,MiniPS::Qinteger(0),   &RecordSize,
    "K",               MiniPS::S_FUNC,    better_k,              &K,
    "Quality",         MiniPS::S_FUNC,    better_quality,        &Quality,
    "ColorTransform",  MiniPS::S_FUNC,    better_colorTransform, &ColorTransform,
    "TransferCPL",     MiniPS::S_PINTEGER,MiniPS::Qinteger(78),  &TransferCPL,
    "DCT",             MiniPS::T_DICT,    MiniPS::Qnull,         &DCT,
    "Scale",           MiniPS::S_SENUM,   y_Scale,               &Scale,
    "ImageDPI",        MiniPS::S_PNUMBER, MiniPS::Qinteger(72),  &cacheHints.ImageDPI,
    "TopMargin",       MiniPS::S_NUMBER,  MiniPS::Qinteger(0),   &cacheHints.TopMargin,
    "BottomMargin",    MiniPS::S_NUMBER,  MiniPS::Qinteger(0),   &cacheHints.BottomMargin,
    "LeftMargin",      MiniPS::S_NUMBER,  MiniPS::Qinteger(0),   &cacheHints.LeftMargin,
    "RightMargin",     MiniPS::S_NUMBER,  MiniPS::Qinteger(0),   &cacheHints.RightMargin,
    "LowerMargin",     MiniPS::S_NUMBER,  MiniPS::Qinteger(0),   &cacheHints.LowerMargin,

    "Comment",         MiniPS::T_STRING,  MiniPS::Qnull,         &cacheHints.Comment,
    "Title",           MiniPS::T_STRING,  MiniPS::Qnull,         &cacheHints.Title,
    "Subject",         MiniPS::T_STRING,  MiniPS::Qnull,         &cacheHints.Subject,
    "Author",          MiniPS::T_STRING,  MiniPS::Qnull,         &cacheHints.Author,
    "Creator",         MiniPS::T_STRING,  MiniPS::Qnull,         &cacheHints.Creator,
    "Producer",        MiniPS::T_STRING,  MiniPS::Qnull,         &cacheHints.Producer,
    "Created",         MiniPS::T_STRING,  MiniPS::Qnull,         &cacheHints.Created,
    "Produced",        MiniPS::T_STRING,  MiniPS::Qnull,         &cacheHints.Produced,
#if 0
    /* vvv parameters for /DCTEncode. Currently ignored. Obsoleted by /DCT */
    "Colors",          MiniPS::S_ANY,     MiniPS::Qnull,         &dummy,
    "HSamples",        MiniPS::S_ANY,     MiniPS::Qnull,         &dummy,
    "VSamples",        MiniPS::S_ANY,     MiniPS::Qnull,         &dummy,
    "QuantTables",     MiniPS::S_ANY,     MiniPS::Qnull,         &dummy,
    "QFactor",         MiniPS::S_ANY,     MiniPS::Qnull,         &dummy,
    "HuffTables",      MiniPS::S_ANY,     MiniPS::Qnull,         &dummy,
#endif
    NULLP
  );
  if (DCT==MiniPS::Qnull) dict->put("/DCT", DCT=(MiniPS::VALUE)new MiniPS::Dict());
  // cacheHints.DCT=(DCT==MiniPS::Qnull)? new MiniPS::Dict() : MiniPS::RDICT(DCT);
  if (cache.isPS()) {
    MiniPS::setDumpPS(cacheHints.TopMargin, true);
    MiniPS::setDumpPS(cacheHints.BottomMargin, true);
    MiniPS::setDumpPS(cacheHints.LeftMargin, true);
    MiniPS::setDumpPS(cacheHints.RightMargin, true);
    MiniPS::setDumpPS(cacheHints.LowerMargin, true);
  }
  cacheHints.DCT=MiniPS::RDICT(DCT);
  cacheHints.EncoderColumns=MiniPS::int2ii(EncoderColumns);
  cacheHints.EncoderBPL=MiniPS::int2ii(EncoderBPL);
  cacheHints.EncoderColors=MiniPS::int2ii(EncoderColors);
  cacheHints.EncoderRows=MiniPS::int2ii(EncoderRows);
  cacheHints.PredictorColumns=MiniPS::int2ii(PredictorColumns);
  cacheHints.PredictorColors=MiniPS::int2ii(PredictorColors);
  cacheHints.PredictorBPC=MiniPS::int2ii(PredictorBPC);
  cacheHints.Effort=MiniPS::int2ii(Effort);
  cacheHints.RecordSize=MiniPS::int2ii(RecordSize);
  cacheHints.K=MiniPS::int2ii(K);
  cacheHints.Quality=MiniPS::int2ii(Quality);
  cacheHints.ColorTransform=MiniPS::int2ii(ColorTransform);
  cacheHints.TransferCPL=MiniPS::int2ii(TransferCPL);
  cacheHints.Scale=(Rule::CacheHints::sc_t)MiniPS::int2ii(Scale);
  /* fprintf(stderr, "scaled=%g\n", (char*)MiniPS::scale(ImageDPI,1)); */
}

void Rule::OutputRule::doSampleFormat(Image::SampledInfo *info, bool separatep) {
  /* Dat: called from appliers.cpp:out_*_work(); the sample format is final */
  bool separatep2=
      separatep &&
    ( cache.SampleFormat==Image::SF_Transparent2
   || cache.SampleFormat==Image::SF_Transparent4
   || cache.SampleFormat==Image::SF_Transparent8);
  /* vvv simplifier added at Sat Jun 15 13:59:40 CEST 2002 */
  if (separatep2) cache.SampleFormat=Image::SF_Transparent8;
  if (!info->setSampleFormat(cache.SampleFormat, cache.WarningOK, /*TryOnly*/false, cache.Transparent))
    Error::sev(Error::EERROR) << "doSampleFormat: cannot set desired SampleFormat" << (Error*)0;
  Image::Sampled *img=info->getImg();
  slen_t n=1;
  if (separatep2) {
    info->separate();
    img=info->getImgs()[0];/*NULLP OK*/;
    n=info->getNncols();
  }
  if (img!=NULLP) {
    if (0==cacheHints.EncoderBPL) cacheHints.EncoderBPL=(slen_t)img->getWd()*img->getCpp()*img->getBpc();
    if (0==cacheHints.EncoderColumns) cacheHints.EncoderColumns=img->getWd();
    if (0==cacheHints.EncoderColors) cacheHints.EncoderColors=img->getCpp();
    if (0==cacheHints.EncoderRows) cacheHints.EncoderRows=img->getHt()*n;
    if (0==cacheHints.PredictorColumns) cacheHints.PredictorColumns=img->getWd();
    if (0==cacheHints.PredictorColors) cacheHints.PredictorColors=img->getCpp();
    if (0==cacheHints.PredictorBPC) cacheHints.PredictorBPC=img->getBpc();
    if (-2==cacheHints.K) cacheHints.K=img->getHt()*n;
  }
}

static char const*getDecode(Rule::Cache::co_t Compression) {
  return (Compression==Rule::Cache::CO_LZW ? "/LZWDecode"
         :Compression==Rule::Cache::CO_Fax ? "/CCITTFaxDecode"
         :Compression==Rule::Cache::CO_ZIP ? "/FlateDecode"
         :Compression==Rule::Cache::CO_RLE ? "/RunLengthDecode"
         :Compression==Rule::Cache::CO_DCT ? "/DCTDecode"
         :Compression==Rule::Cache::CO_IJG ? "/DCTDecode"
         :Compression==Rule::Cache::CO_JAI ? "/DCTDecode"
         :"");
}

/** For PDF BI inline images */
static char const*getBIDecode(Rule::Cache::co_t Compression) {
  return (Compression==Rule::Cache::CO_LZW ? "/LZW"
         :Compression==Rule::Cache::CO_Fax ? "/CCF"
         :Compression==Rule::Cache::CO_ZIP ? "/Fl"
         :Compression==Rule::Cache::CO_RLE ? "/RL"
         :Compression==Rule::Cache::CO_DCT ? "/DCT"
         :Compression==Rule::Cache::CO_IJG ? "/DCT"
         :Compression==Rule::Cache::CO_JAI ? "/DCT"
         :"");
}

void Rule::OutputRule::appendDecoderSpec(GenBuffer::Writable &out) const {
  assert(cacheHints.PredictorBPC!=0 && "doSampleFormat already called");
  bool hps=hasPredictorSpec();
  if (cache.isPDF()) {
    /* Dat: also implies appendTransferSpec() -- this is true only for PDF */
    if (!cache.isPDFB()) {
      if (cache.Compression==cache.CO_None) {
             if (cache.TransferEncoding==cache.TE_Hex) out << "/Filter/ASCIIHexDecode";
        else if (cache.TransferEncoding==cache.TE_A85) out << "/Filter/ASCII85Decode";
      } else if (cache.TransferEncoding!=cache.TE_Hex && cache.TransferEncoding!=cache.TE_A85) {
        out << "/Filter" << getDecode(cache.Compression);
        if (hps) { out << "/DecodeParms"; appendPredictorSpec(out); }
      } else { /* both TransferEncoding and Compression */
             if (cache.TransferEncoding==cache.TE_Hex) out << "/Filter[/ASCIIHexDecode";
        else if (cache.TransferEncoding==cache.TE_A85) out << "/Filter[/ASCII85Decode";
        else assert(0);
        out << getDecode(cache.Compression);
        if (hps) { out << "]/DecodeParms[null"; appendPredictorSpec(out); }
        /* ^^^ BUGFIX at Tue Jun  4 18:50:13 CEST 2002 */
        out << ']';
      }
    } else {
      if (cache.Compression==cache.CO_None) {
             if (cache.TransferEncoding==cache.TE_Hex) out << "/F/AHx";
        else if (cache.TransferEncoding==cache.TE_A85) out << "/F/A85";
      } else if (cache.TransferEncoding!=cache.TE_Hex && cache.TransferEncoding!=cache.TE_A85) {
        out << "/F" << getBIDecode(cache.Compression);
        if (hps) { out << "/DP"; appendPredictorSpec(out); }
      } else { /* both TransferEncoding and Compression */
             if (cache.TransferEncoding==cache.TE_Hex) out << "/F[/AHx";
        else if (cache.TransferEncoding==cache.TE_A85) out << "/F[/A85";
        else assert(0);
        out << getBIDecode(cache.Compression);
        if (hps) { out << "]/DP[null"; appendPredictorSpec(out); }
        out << ']';
      }
    } /* IFELSE PDFB */
  } else { /* NOT PDF */
    if (cache.Compression!=cache.CO_None) {
      appendPredictorSpec(out);
      out << getDecode(cache.Compression) << " filter";
    }
  } /* IFELSE PDF */
}

bool Rule::OutputRule::hasPredictorSpec() const {
  return cache.Compression==cache.CO_Fax
      || (cache.Compression==cache.CO_ZIP || cache.Compression==cache.CO_LZW) && cache.Predictor!=cache.PR_None;
}

void Rule::OutputRule::appendPredictorSpec(GenBuffer::Writable &out) const {
  assert(cacheHints.PredictorBPC!=0 && "doSampleFormat already called");
  if (cache.Compression==cache.CO_Fax)
    out << "<</K " << cacheHints.K
        << "/Columns " << cacheHints.EncoderBPL
        /* ^^^ EncoderColumns -> EncoderBPL BUGFIX at Wed Jul  3 20:05:12 CEST 2002 */
        << ">>";
  else if ((cache.Compression==cache.CO_ZIP || cache.Compression==cache.CO_LZW) &&
      cache.Predictor!=cache.PR_None)
   out<< "<</BitsPerComponent " << (unsigned)cacheHints.PredictorBPC
      << "/Columns " << cacheHints.PredictorColumns
      << "/Colors " << (unsigned)cacheHints.PredictorColors
      << (cache.Predictor==cache.PR_TIFF2 ? "/Predictor 2>>": "/Predictor 10>>");
}
void Rule::OutputRule::appendTransferSpec(GenBuffer::Writable &out) const {
       if (cache.TransferEncoding==cache.TE_Hex) out << "/ASCIIHexDecode filter";
  else if (cache.TransferEncoding==cache.TE_A85) out << "/ASCII85Decode filter";
}

/* --- */

static Rule::Applier *first=(Rule::Applier*)NULLP;

void Rule::register0(Rule::Applier *anew) {
  param_assert(anew!=NULLP);
  anew->next=first;
  first=anew;
}

unsigned Rule::printAppliers(GenBuffer::Writable &out) {
  unsigned num=0;
  Applier *p=first;
  while (p!=(Applier*)NULLP) {
    if (p->check_rule!=0/*NULLP*/ && p->work!=0/*NULLP*/) { num++;  out << ' ' << p->format; }
    p=p->next;
  }
  return num;
}

Rule::OutputRule* Rule::buildProfile(MiniPS::VALUE Profile, bool quiet) {
  param_assert(MiniPS::getType(Profile)==MiniPS::T_ARRAY);
  MiniPS::Array *pary=MiniPS::RARRAY(Profile);
  OutputRule *ret=new OutputRule[pary->getLength()+1], *or_=ret;
  /* ^^^ just enough place; there may be BAD items which won't be stored */
  MiniPS::VALUE *val;
  unsigned c;
  #if !USE_BUILTIN_LZW
    bool lzw_warning=true;
  #endif
  if (quiet) Error::pushPolicy((Error::level_t)0,
   /*printed_:*/Error::getTopPrinted()+0>Error::WARNING+0 ? Error::getTopPrinted() : Error::WARNING,
   Error::WARNING_DEFER, (GenBuffer::Writable*)NULLP); /* Dat: WARNING_DEFER untested */
  for (c=0, pary->getFirst(val); val!=NULLP; pary->getNext(val), c++) {
    /* val: each OutputRule of the Profile */
    or_->fromDict(*val);
    or_->c=c;
    Applier *p=first;
    // printf("building: %s...\n", p->format);
    #if !USE_BUILTIN_LZW
      if (or_->cache.Compression==or_->cache.CO_LZW && lzw_warning) {
        lzw_warning=false;
        Error::sev(Error::WARNING) << "buildProfile: please `configure --enable-lzw' for /Compression/LZW support in OutputRule #" << c << (Error*)0;
      }
    #endif
    while (p!=NULLP) {
      if (p->check_rule!=0/*NULLP*/) switch (p->check_rule(or_)) {
       case Applier::BAD:
        Error::sev(Error::WARNING_DEFER) << "buildProfile: ^^^ thus ignoring impossible OutputRule #" << c << (Error*)0;
        goto end_appliers;
       case Applier::MAYBE: case Applier::OK:
        if (p->work!=0/*NULLP*/) { or_++; goto end_appliers; }
        Error::sev(Error::WARNING_DEFER) << "buildProfile: ^^^ ignoring unimplemented OutputRule #" << c << (Error*)0;
       // case p->DONT_KNOW: ;
      }
      p=p->next; /* continue with other Appliers; hope other than DONT_KNOW */
    }
    Error::sev(Error::WARNING_DEFER) << "buildProfile: ignoring, no handlers for OutputRule #" << c << (Error*)0;
   end_appliers:
    if (quiet) delete Error::getRecorded();
  }
  if (quiet) { delete Error::getRecorded(); Error::popPolicy(); }
  if (or_==ret) Error::sev(Error::WARNING) << "buildProfile: all OutputRules in the .job file are useless" << (Error*)0;
  or_->dict=or_->dictHints=(MiniPS::Dict*)NULLP; /* end-of-list */
  return ret;
}

void Rule::applyProfile(GenBuffer::Writable& out, OutputRule*rule_list, Image::SampledInfo *sf) {
  OutputRule *or_;
  // unsigned tryc=0; /* Wed Jul  3 19:30:33 CEST 2002 */
  Error::pushPolicy((Error::level_t)0,
    /*printed_:*/Error::getTopPrinted()+0>Error::NOTICE+0 ? Error::getTopPrinted() : Error::NOTICE,
    Error::NOTICE_DEFER, (GenBuffer::Writable*)NULLP);
  Image::Sampled::rgb_t Transparent=0x1000000UL;
  if (rule_list->dict!=NULLP) {
    Transparent=rule_list->cache.Transparent;
    for (or_=rule_list+1; or_->dict!=NULLP; or_++) {
      if (Transparent!=rule_list->cache.Transparent) {
        /* Imp: make different copies, based on different or_->cache.Transparent -- or not? */
        Error::sev(Error::EERROR) << "applyProfile: ambiguous /Transparent" << (Error*)0;
      }
    }
  }
  /* Dat: -transparent ... makes the specified color transparent, but it cannot
   *      be used to remove transparenct
   */
  // printf("Transparent=0x%lx\n",Transparent);
  for (or_=rule_list; or_->dict!=NULLP; or_++) {
    /* ^^^ Try each OutputRule (or_) in reverse order of registration */
    Error::sev(Error::NOTICE_DEFER) << "applyProfile: trying OutputRule #" << or_->c << (Error*)0;
    if (sf->setSampleFormat(or_->cache.SampleFormat, or_->cache.WarningOK, /*TryOnly*/true, or_->cache.Transparent)) {
      /* image supports the SampleFormat of OutputRule */
      Applier *p=first;
      while (p!=NULLP) {
        /* ^^^ Try each output Applier for the current candidate OutputRule */
        if (p->check_rule!=0/*NULLP*/ && p->work!=0/*NULLP*/) {
          // tryc++;
          switch (p->work(out, or_, sf)) {
           case Applier::BAD:
            Error::sev(Error::WARNING) << "applyProfile: ^^^ thus cannot apply OutputRule #" << or_->c << (Error*)0;
            goto end_appliers;
           case Applier::OK:
            // if (or_->c!=0) {
            delete Error::getRecorded(); Error::popPolicy();
            Error::sev(Error::NOTICE) << "applyProfile: applied OutputRule #" << or_->c << (Error*)0;
            return;
           /* case Applier::MAYBE: impossible */
           // case p->DONT_KNOW: ;
          }
        }
        p=p->next; /* continue with other Appliers; hope other than DONT_KNOW */
     end_appliers: ;
      }
    } /* IF image supports SampleFormat */
  }
  Error::sev(Error::EERROR) << "applyProfile: invalid combination, no applicable OutputRule" << (Error*)0;
}

void Rule::deleteProfile(OutputRule*rule_list) {
  /* MiniPS::* objects still remain -- the caller (reader) will delete them. */
  delete [] rule_list;
}

/* --- */

//#include <string.h>
//static char xx[5000];

void Rule::writeData(GenBuffer::Writable&, GenBuffer::Writable&outstream, Image::SampledInfo *sf) {
  Image::Indexed **imgs=sf->getImgs();
  slen_t rlenht;
  if (imgs!=NULLP) {
    unsigned i;
    if (sf->getNncols()!=0 && 0!=(rlenht=imgs[0]->getRlen()*imgs[0]->getHt())) {
      // fprintf(stderr,"rh=%u nc=%u\n", rlenht, sf->getNncols());
      for (i=0;i<sf->getNncols();i++) outstream.vi_write(imgs[i]->getRowbeg(), rlenht);
      //for (i=0;i<sf->getNncols();i++) {
      //  memset(xx, 1<<(i), sizeof(xx));
      //  outstream.vi_write(xx, rlenht);
      //}
    }
  } else {
    Image::Sampled *img=sf->getImg();
    rlenht=img->getRlen()*img->getHt();
    if (rlenht!=0) outstream.vi_write(img->getRowbeg(), rlenht);
  }
  outstream.vi_write(0,0); /* Dat: frees cp, bp, (ap) */
}
void Rule::writePalData(GenBuffer::Writable& outpal, GenBuffer::Writable&outstream, Image::SampledInfo *sf) {
  Image::Sampled *img=sf->getImg();
  slen_t wlen=img->getRowbeg()-img->getHeadp();
  if (wlen!=0) outpal.vi_write(img->getHeadp(), wlen);
  writeData(outpal, outstream, sf);
  /* vvv replaced by the more generic writeData */
  // slen_t rlenht=img->getRlen()*img->getHt();
  // if (rlenht!=0) outstream.vi_write(img->getRowbeg(), rlenht);
  // outstream.vi_write(0,0); /* Dat: frees cp, bp, (ap) */
}

/** Returns a divisor of v1*v2 near 4096 */
static slen_t near_div(slen_t v1, slen_t v2) {
  static const slen_t LOW=2048, MID=4096, HIGH=8192;
  slen_t d, p;
  if (v1>v2) { d=v1; v1=v2; v2=d; }
  if (LOW<=v2 && v2<=HIGH) return v2; /* larger */
  if (LOW<=v1 && v1<=HIGH) return v1; /* smaller */
  if ((p=v1*v2)<=HIGH) return p; /* smaller */
  for (d=MID;d<=HIGH;d++)  if (p%d==0) return d;
  for (d=MID-1;d>=LOW;d--) if (p%d==0) return d;
  return v1; /* the smaller one */
}

/** Basicly out.format("%g", (double)n/255);, but we don't want to depend on
 * floating point arithmetic of the underlying architecture.
 */
static void div255(GenBuffer::Writable& out, unsigned n) {
  char tmp[7];
  unsigned abc;
  param_assert(n<=255);
  tmp[0]='0'; tmp[1]='.'; tmp[3]='\0';
  if (n%51!=0) {
    /* Divisors of 255: 1, 3, 5, 15, 17, 51, 85, 255.
     *   0/255 == "0"       51/255 == "0.2"
     * 102/255 == "0.4"    153/255 == "0.6"
     * 204/255 == "0.8"    255/255 == "1"
     * All other k/255 are infinite as a decimal fraction.
     * 1000 > 2*255, so 3 digits after '.' is enough.
     */
    assert(n<=254);
   #if 0
    abc=(127L+1000*n)/255;
    tmp[2]='0'+abc/100;
    tmp[3]='0'+(abc/10)%10;
    tmp[4]='0'+abc%10;
    tmp[5]='\0';
   #else
    abc=(127L+10000*n)/255;
    tmp[2]='0'+abc/1000;
    tmp[3]='0'+(abc/100)%10;
    tmp[4]='0'+(abc/10)%10;
    tmp[5]='0'+abc%10;
    tmp[6]='\0';
   #endif
  } else if (n<153) {
    if (n<51) tmp[1]='\0';
    else if (n>51) tmp[2]='4';
    else tmp[2]='2';
  }
  else if (n<204) tmp[2]='6';
  else if (n>204) { tmp[0]='1'; tmp[1]='\0'; }
  else tmp[2]='8';
  out << tmp;
}

void Rule::writeTTE(
 GenBuffer::Writable& out,
 GenBuffer::Writable&outpal,
 GenBuffer::Writable&outstream,
 char const*template_, Rule::OutputRule*or_, Image::SampledInfo *sf, stream_writer_t stream_writer, char const*const*strings) {
  unsigned int i, j;
  register char const*p;
  char *r;
  Image::Sampled *img=sf->getImg();
  param_assert(template_!=(const char*)NULLP);
  p=template_;
  bool nzp, scp;
  SimBuffer::B scf;
  while (1) {  
    assert(template_==p);
    while (*p!='`' && *p!='\0') p++; /* '`' is the escape character */
    if (p!=template_) out.vi_write(template_, p-template_);
    if (*p++=='\0') break;
    switch (*p++) {
     case '\0':
      p--; /* fall through */
     case '`':
      out.vi_write("`", 1); break;
     case '#': /* number of non-transparent colors of /Indexed*, /Transparent* */
      out << sf->getNncols(); break;
     case 'p': /* 3*(number of non-transparent colors of /Indexed*, /Transparent*) */
      out << (3*sf->getNncols()); break;
     case 'P': /* number of palette bytes (including the transparent color) */
      out << img->getRowbeg()-img->getHeadp(); break;
     case '0': case '1': case '2': /* arbitrary, user-defined string */
      param_assert(strings!=(char const*const*)NULLP);
      out << strings[p[-1]-'0']; break;
     case 'C': /* PDF /Procset entries */
      if (or_->cache.isIndexed()) out << "/ImageI";
      else if (or_->cache.isGray()) out << "/ImageB";
      else out << "/ImageC"; /* SF_Rgb*, SF_Asis etc. */
      break;
     case 'i': /* PS image or colorimage operator */
      out << (or_->cache.isGray() ? "image"
            : or_->cache.SampleFormat==Image::SF_Mask || or_->cache.SampleFormat==Image::SF_Indexed1 ? "imagemask"
            : "false 3 colorimage");
      break;
     case 'I': /* PS warning for usage of colorimage operator */
       // if (!or_->cache.isGray()) out << "% PSLC required\n";
       if (!or_->cache.isGray() && !or_->cache.isTransparentM()) out << "%%Extensions: CMYK\n";
       break;
#if 0
     case "p:invalid": /* optional predictor specification */
      { unsigned char pred=or_->cache.Predictor;
             if (pred== 2) out << "/Predictor 2";  /* TIFF predictor */
        else if (pred>=10) out << "/Predictor 10"; /* any PNG predictor */
      }
#endif
     case 'w': /* image width, decimal */
      out << img->getWd();
      break;
     case 'h': /* image height, decimal */
      out << img->getHt();
      break;
     case '?': /* a divisor of rlen*height near 4096 */
      if (sf->getImgs()!=NULLP) { /* /Transparent+ */
        out << near_div(sf->getImgs()[0]->getRlen(), img->getHt());
        /* Dat: `img->getHt()*sf->getNncols()' would be wrong */
      } else {
        out << near_div(img->getRlen(), img->getHt());
      }
      break;
     case 'B': /* Clean7Bits or Binary; Sun Jun 23 18:55:35 CEST 2002 */
      out << (or_->cache.isPDF() ? (or_->cache.isBinSB() ? "%\307\354\217\242\n" : "")
                                 : (or_->cache.isBinSB() ? "Binary" : "Clean7Bit"));
      break;
     case 'b': /* image bpc, decimal */
      /* added "true"|"false" for sam2p-0.39 at Sun Sep 22 17:49:25 CEST 2002 */
      if (or_->cache.SampleFormat==Image::SF_Mask) out << "false";
      else if (or_->cache.SampleFormat==Image::SF_Indexed1) out << "true";
      else out << (unsigned)img->getBpc();
      break;
     case 'c': /* image cpp, decimal */
      out << (unsigned)img->getCpp();
      break;
     case 'd': /* device-specific color-space name */
      out << Image::Sampled::cs2devcs(img->getCs());
      break;
     case 't': /* closefile specification in PostScript */
      if (or_->cache.Compression!=or_->cache.CO_None) out << " F closefile";
      if (or_->cache.TransferEncoding!=or_->cache.TE_Binary) out << " T closefile";
      break;
     case 'T': /* TransferEncoding specification in PostScript */
      or_->appendTransferSpec(out);
      break;
     case 'F': /* decoding filter specification in PostScript */
      or_->appendDecoderSpec(out);
      break;
     case 'O': /* 0..1 /Decode values */
      i=1;
      emit_Decode:
      j=img->getCpp(); 
      out << "0 " << i;
      while (j--!=1) out << " 0 " << i;
      break;
     case 'D': /* 0..1 /Decode values for indexed, 0..max-1 for others */
      i=(or_->cache.isIndexed())?(1<<img->getBpc())-1:1;
      goto emit_Decode;
     case 'S': /* image data stream */
      stream_writer(outpal, outstream, sf);
      break;
     case 'g': /* PDF 0..1 RGB triplet of the 0th palette color */
      assert(img->getTy()==img->TY_INDEXED);
      r=img->getHeadp();
      goto appG;
     case 'G': /* PDF 0..1 RGB triplet of the 1st palette color */
      assert(img->getTy()==img->TY_INDEXED);
      r=img->getHeadp()+3;
     appG:
      div255(out, (unsigned char)r[0]); out << ' ';
      div255(out, (unsigned char)r[1]); out << ' ';
      div255(out, (unsigned char)r[2]);
      break;
     case 'r': /* PS 0..1 RGB triplet of the 0th palette color */
      assert(img->getTy()==img->TY_INDEXED);
      r=img->getHeadp();
      out << (unsigned)(unsigned char)r[0] << " 255 div "
          << (unsigned)(unsigned char)r[1] << " 255 div "
          << (unsigned)(unsigned char)r[2] << " 255 div";
      break;
     case 'R': /* PS code for setting up bg and fg colors for an imagemask */
      /* formerly: PS 0..1 RGB triplet of the 1st palette color */
      /* changed at Sun Sep 22 18:02:41 CEST 2002 */
      if (or_->cache.SampleFormat==Image::SF_Mask || or_->cache.SampleFormat==Image::SF_Indexed1) {
        assert(img->getTy()==img->TY_INDEXED);
        r=img->getHeadp();
        out << (unsigned)(unsigned char)r[0] << " 255 div " /* 0th palette color */
            << (unsigned)(unsigned char)r[1] << " 255 div "
            << (unsigned)(unsigned char)r[2] << " 255 div setrgbcolor\n";
        if (or_->cache.SampleFormat==Image::SF_Indexed1) {
          out << "0 0 moveto\n"
              << img->getWd() << " 0 lineto\n0 "
              << img->getHt() << " rlineto\n-"
              << img->getWd() << " 0 rlineto\nclosepath fill\n"
              << (unsigned)(unsigned char)r[3] << " 255 div " /* 1st palette color */
              << (unsigned)(unsigned char)r[4] << " 255 div "
              << (unsigned)(unsigned char)r[5] << " 255 div\nsetrgbcolor\n";
        }
      }
      break;
     case 'E': /* EPS header for unscaled PS files */
      if (or_->cacheHints.Scale==or_->cacheHints.SC_None) out << " EPSF-3.0";
      break;
     case 'X': /* BoundingBox for EPS, MediaBox for PDF */
      if (or_->cache.isPDF()) {
        // out << "0 0 ";
        out << "0 ";
        goto do_bbox;
      } else if (or_->cacheHints.Scale==or_->cacheHints.SC_None) {
        /* It is no point to start the BoundingBox of EPS files at
         * (LeftMargin,BottomMargin). The effect would be the same as
         * specifying no margins at all.  Our choice is better: the
         * BoundingBox contains the image and the margins too.
         */
        // out << "%%BoundingBox: 0 0 ";
        out << "%%BoundingBox: 0 ";
       do_bbox:
        // out << MiniPS::RVALUE(or_->cacheHints.LowerMargin) << ';'; /* SUXX: this would put `1 72 mul' */
        // out << MiniPS::RVALUE(or_->cacheHints.BottomMargin) << ';'; /* SUXX: this would put `1 72 mul' */
        MiniPS::dumpAdd3(out, MiniPS::Qinteger(0), MiniPS::Qinteger(0), MiniPS::Qinteger(0), or_->cacheHints.LowerMargin, or_->cacheHints.BottomMargin, 1);
        out << ' ';
        MiniPS::dumpAdd3(out, or_->cacheHints.ImageDPI, MiniPS::Qinteger(img->getWd()),
          or_->cacheHints.LeftMargin, or_->cacheHints.RightMargin, MiniPS::Qinteger(0), 2);
        out << ' ';
        MiniPS::dumpAdd3(out, or_->cacheHints.ImageDPI, MiniPS::Qinteger(img->getHt()),
          or_->cacheHints.TopMargin, or_->cacheHints.LowerMargin, MiniPS::Qinteger(0), 2);
        if (!or_->cache.isPDF()) out << '\n';
      }
      break;
     case 's': /* scaling to a full PostScript page or translation for PDF and EPS */
      nzp=!(MiniPS::isZero(or_->cacheHints.LowerMargin)
         && MiniPS::isZero(or_->cacheHints.TopMargin));
      scp=!MiniPS::isEq(or_->cacheHints.ImageDPI, 72);
      
      if (or_->cache.isPDF()) {
        SimBuffer::B scf;
        if (scp) MiniPS::dumpScale(scf, or_->cacheHints.ImageDPI);
            else scf << 'x';
        if (nzp || scp) out << " " << scf << " 0 0 " << scf << " " << MiniPS::RVALUE(or_->cacheHints.LeftMargin)
                            << ' ' << MiniPS::RVALUE(or_->cacheHints.LowerMargin)
                     << " cm"; /* translate */
      } else switch (or_->cacheHints.Scale) {
       case Rule::CacheHints::SC_None:
        if (nzp) out << '\n' << MiniPS::RVALUE(or_->cacheHints.LeftMargin)
                     << ' '  << MiniPS::RVALUE(or_->cacheHints.LowerMargin)
                     << " translate";
        if (scp) {
          MiniPS::dumpScale(scf, or_->cacheHints.ImageDPI);
          out << '\n' << scf << " dup scale";
        }
        break;
       case Rule::CacheHints::SC_OK:
        /* from pshack/big.ps */
        out <<"\n6 dict begin currentpagedevice/PageSize get dup 0 get\n " << MiniPS::RVALUE(or_->cacheHints.LeftMargin) << " sub " << MiniPS::RVALUE(or_->cacheHints.RightMargin) << " sub/w exch\n"
               " def 1 get " << MiniPS::RVALUE(or_->cacheHints.TopMargin) << " sub " << MiniPS::RVALUE(or_->cacheHints.BottomMargin) << " sub/h exch\n"
               " def/x " << img->getWd() << " def/y " << img->getHt() <<  " def " << MiniPS::RVALUE(or_->cacheHints.LeftMargin) << ' ' << MiniPS::RVALUE(or_->cacheHints.BottomMargin) << " translate x h\n"
               " mul y w mul gt{w x 0 h y w mul x div sub 2 div}{h y w x h mul y div sub 2 div\n"
               " 0}ifelse translate div dup scale\nend";
        break;
       case Rule::CacheHints::SC_RotateOK:
        /* from pshack/big.ps */
        out <<"\n6 dict begin currentpagedevice/PageSize get dup 0 get\n " << MiniPS::RVALUE(or_->cacheHints.LeftMargin) << " sub " << MiniPS::RVALUE(or_->cacheHints.RightMargin) << " sub/w exch\n"
               " def 1 get " << MiniPS::RVALUE(or_->cacheHints.TopMargin) << " sub " << MiniPS::RVALUE(or_->cacheHints.BottomMargin) << " sub/h exch\n"
               " def/x " << img->getWd() << " def/y " << img->getHt() <<  " def " << MiniPS::RVALUE(or_->cacheHints.LeftMargin) << ' ' << MiniPS::RVALUE(or_->cacheHints.BottomMargin) << "/b y h mul x\n"
               " w mul gt def b{w y}{h x}ifelse div/c x h mul y w mul gt def c{w x}{h y}ifelse\n"
               " div gt{h add translate -90 rotate b{w y h x w mul y div sub 2 div 0}{h\n"
               " x 0 w y h mul x div sub 2 div}}{translate c{w x 0 h y w mul x div sub 2 div}{h\n"
               " y w x h mul y div sub 2 div 0}}ifelse ifelse translate div dup scale\nend";
        break;
      }
      break;
     default:
      Error::sev(Error::EERROR) << "writeTTE: unknown escape: " << (char)p[-1] << (Error*)0;
    }
    template_=p;
  }
}

/** by pts@fazekas.hu at Mon Apr 15 22:31:03 CEST 2002 */
void Rule::writeTTM(
 Filter::VerbatimE &outve,
 GenBuffer::Writable&outpal,
 GenBuffer::Writable&outstream,
 MiniPS::Array *chunkArray,
 Rule::OutputRule*or_,
 Image::SampledInfo *sf,
 stream_writer_t stream_writer,
 char const*const*strings) {
  static const unsigned MAXLEN=64;
  /* Imp: use heap instead of stack space */
  slen_t offsets[MAXLEN+1]; /* 260 bytes stack */
  SimBuffer::B chunks[MAXLEN]; /* <=1024 bytes stack; default constructor */
  MiniPS::VALUE *chunk;
  param_assert(chunkArray!=NULLP);
  // param_assert(chunkArray->getLength()<=MAXLEN);
  if (chunkArray->getLength()>(int)MAXLEN) Error::sev(Error::EERROR) << "writeTTM: TTM too long" << (Error*)0;
  GenBuffer::Writable& out=outve.getOut();
  MiniPS::ii_t i, ii;
  for (chunkArray->getFirst(chunk), i=0; chunk!=NULLP; chunkArray->getNext(chunk), i++) {
    if (MiniPS::getType(*chunk)==MiniPS::T_ARRAY) {
      outve.setOut(chunks[i]);
      writeTTM(outve, outpal, outstream, MiniPS::RARRAY(*chunk), or_, sf, stream_writer, strings);
    }
  }
  for (chunkArray->getFirst(chunk), offsets[i=0]=0; chunk!=NULLP; chunkArray->getNext(chunk), i++) {
    switch (MiniPS::getType(*chunk)) {
     case MiniPS::T_ARRAY:
      break;
     case MiniPS::T_STRING: /* always null-terminated */
      outve.setOut(chunks[i]);
      writeTTE(outve, outpal, outstream, MiniPS::RSTRING(*chunk)->begin_(), or_, sf, stream_writer, strings);
      break;
     case MiniPS::T_INTEGER:
      if (0==(ii=MiniPS::int2ii(*chunk))) Error::sev(Error::EERROR) << "writeTTM: zero is an invalid chunk" << (Error*)0;
      if (ii>0) { /* an offset */
        if (ii>i) Error::sev(Error::EERROR) << "writeTTM: cannot predict chunk offset" << (Error*)0;
        if (MiniPS::T_ARRAY==MiniPS::getType(chunkArray->get(ii)))
               chunks[i].write_num(offsets[ii], 10); /* Dat: 10: 10 digits (for PDF xref table), not base 10 */
          else chunks[i] << offsets[ii];
      } else { /* a length */
        chunks[i] << chunks[-ii].getLength();
      }
      break;
     default:
      Error::sev(Error::EERROR) << "writeTTM: invalid chunk type: " << MiniPS::getTypeStr(MiniPS::getType(*chunk)) << (Error*)0;
    }
    offsets[i+1]=offsets[i]+chunks[i].getLength();
  } /* NEXT */
  outve.setOut(out);
  for (i=0; i<chunkArray->getLength(); i++) out << chunks[i];
  /* Imp: organize that chunks[.] gets freed earlier than this */
} /* Rule::writeTTM() */

MiniPS::Dict *Rule::Templates=(MiniPS::Dict*)NULLP;

void Rule::writeTTT(
 GenBuffer::Writable&out,
 GenBuffer::Writable&outpal,
 GenBuffer::Writable&outstream,
 char const *template_key,
 Rule::OutputRule*or_,
 Image::SampledInfo *sf,
 stream_writer_t stream_writer,
 char const*const*strings) {
  assert(Templates!=NULLP);
  Error::sev(Error::NOTICE) << "writeTTT: using template: " << template_key << (Error*)0;
  MiniPS::VALUE val=Templates->get(template_key, strlen(template_key));
  assert(val!=MiniPS::Qundef);
  switch (MiniPS::getType(val)) {
   case MiniPS::T_STRING:
    writeTTE(out, outpal, outstream, MiniPS::RSTRING(val)->begin_(), or_, sf, stream_writer, strings);
    break;
   case MiniPS::T_ARRAY:
    /* Imp: vvv This up-cast is unsafe! */
    writeTTM(*(Filter::VerbatimE*)&out, outpal, outstream, MiniPS::RARRAY(val), or_, sf, stream_writer, strings);
    break;
   default:
    Error::sev(Error::EERROR) << "writeTTT: invalid template type: " << MiniPS::getTypeStr(MiniPS::getType(val)) << (Error*)0;
  }
}

/* __END__ */
