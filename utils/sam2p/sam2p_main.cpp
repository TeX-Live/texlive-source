/* sam2p_main.cpp
 * by pts@fazekas.hu at Fri Mar 15 16:11:03 CET 2002
 */

#include "gensi.hpp"
#include "gensio.hpp"
#include "image.hpp"
#include "error.hpp"
#include "encoder.hpp"
#include "minips.hpp"
#include "rule.hpp"
#include "main.hpp"
#include "in_jai.hpp"

/* Sat Jul  6 16:39:19 CEST 2002
 * empirical checkerg++ helper routines for gcc version 2.95.2 20000220 (Debian GNU/Linux)
 * see c_lgcc.cpp for more
 */
#if !defined(OBJDEP) && defined(__CHECKER__) && !HAVE_PTS_C_LGCC_CPP_REQUIRED
#  include "c_lgcc.cpp"
#endif

#if 0 /*#ifdef __CHECKER__*/
#include <stdlib.h>
void* __builtin_vec_new(unsigned len){ return malloc(len); }
void __builtin_vec_delete(void *p) { free(p); }
void* __builtin_new(unsigned len){ return malloc(len); }
void __builtin_delete(void *p) { free(p); }
void __rtti_user() { abort(); }
void __rtti_si() { abort(); }
void terminate() { abort(); }
extern "C" void __pure_virtual();
void __pure_virtual() { abort(); }
#endif

#if OBJDEP
#  warning PROVIDES: sam2p_main
#  warning REQUIRES: gensi.o
#  warning REQUIRES: gensio.o
#  warning REQUIRES: image.o
#  warning REQUIRES: error.o
#  warning REQUIRES: encoder.o
#  warning REQUIRES: minips.o
#  warning REQUIRES: rule.o
#endif

#include <stdio.h>
#include <string.h> /* memset() */

#if OBJDEP
#  warning REQUIRES: in_tiff.o
#  warning REQUIRES: in_jpeg.o
#  warning REQUIRES: in_png.o
#  warning REQUIRES: in_jai.o
#  warning REQUIRES: in_pcx.o
#  warning REQUIRES: in_xpm.o
#  warning REQUIRES: in_lbm.o
#  warning REQUIRES: in_gif.o
#  warning REQUIRES: in_bmp.o
#  warning REQUIRES: in_pnm.o
#  warning REQUIRES: in_tga.o
/*#  warning REQUIRES: in_pdf.o Dat: no more, integrated to in_ps.o */
#  warning REQUIRES: in_ps.o
#endif
extern Image::Loader in_tiff_loader;
extern Image::Loader in_jpeg_loader;
extern Image::Loader in_png_loader;
extern Image::Loader in_jai_loader;
extern Image::Loader in_pcx_loader;
extern Image::Loader in_xpm_loader;
extern Image::Loader in_lbm_loader;
extern Image::Loader in_gif_loader;
extern Image::Loader in_bmp_loader;
extern Image::Loader in_pnm_loader;
extern Image::Loader in_tga_loader;
extern Image::Loader in_pdf_loader;
extern Image::Loader in_ps_loader;
void init_loader() {
  static bool had_init_loader=false;
  if (had_init_loader) return;
  Image::register0(&in_tga_loader); /* checker not sure; install early */
  Image::register0(&in_pcx_loader);
  Image::register0(&in_xpm_loader);
  Image::register0(&in_lbm_loader);
  Image::register0(&in_gif_loader);
  Image::register0(&in_bmp_loader);
  Image::register0(&in_pnm_loader);
  Image::register0(&in_tiff_loader);
  Image::register0(&in_jpeg_loader);
  Image::register0(&in_png_loader);
  Image::register0(&in_jai_loader);
  Image::register0(&in_pdf_loader);
  Image::register0(&in_ps_loader);
  had_init_loader=true;
}

#if OBJDEP
#  warning REQUIRES: appliers.o
#endif
extern Rule::Applier out_pnm_applier;
extern Rule::Applier out_jpeg_applier;
extern Rule::Applier out_jpegjai_applier;
extern Rule::Applier out_tiffjai_applier;
extern Rule::Applier out_tiff_applier;
extern Rule::Applier out_png_applier;
extern Rule::Applier out_bmp_applier;
extern Rule::Applier out_gif89a_applier;
extern Rule::Applier out_xpm_applier;
extern Rule::Applier out_l1c_applier;
// extern Rule::Applier out_l1op_applier;
extern Rule::Applier out_l1tr_applier;
extern Rule::Applier out_l23_applier;
// extern Rule::Applier out_l1fa85g_applier;
extern Rule::Applier out_l2jbin_applier;
// extern Rule::Applier out_p0jbin_applier;
extern Rule::Applier out_empty_applier;
extern Rule::Applier out_meta_applier;
extern Rule::Applier out_xwd_applier;
extern Rule::Applier out_x11_applier;
void init_applier() {
  static bool had_init_applier=false;
  if (had_init_applier) return;
  // Rule::register0(&out_l1op_applier);
  Rule::register0(&out_l1tr_applier);
  // Rule::register0(&out_l1fa85g_applier);
  Rule::register0(&out_l2jbin_applier);
  // Rule::register0(&out_p0jbin_applier);
  Rule::register0(&out_l23_applier);
  Rule::register0(&out_l1c_applier);
  Rule::register0(&out_xpm_applier);
  Rule::register0(&out_gif89a_applier);
  Rule::register0(&out_pnm_applier);
  Rule::register0(&out_jpeg_applier);
  Rule::register0(&out_jpegjai_applier);
  Rule::register0(&out_tiffjai_applier);
  Rule::register0(&out_tiff_applier);
  Rule::register0(&out_png_applier);
  Rule::register0(&out_bmp_applier);
  Rule::register0(&out_empty_applier);
  Rule::register0(&out_meta_applier);
  Rule::register0(&out_xwd_applier);
  Rule::register0(&out_x11_applier);
  had_init_applier=true;
}

static char *bts_ttt=
#include "bts2.tth"
;

/* --- One-liner mode */

/** Dat: mod 16 does matter: expected # args etc. */
static const unsigned
  OPT_unknown=0,
  OPT_SampleFormat=0x02,
  OPT_LoadHints=0x12,
  OPT_Compression=0x22,
  OPT_TransferEncoding=0x32,
  OPT_TransferEncodingF=0x42,
  OPT_Asis=0x50,
  OPT_PSL1=0x60,
  OPT_PSLC=0x70,
  OPT_PSL2=0x80,
  OPT_PSL3=0x90,
  OPT_PS=0xA1,
  OPT_PDF=0xB1,
  OPT_DisplayJobFile=0xC1,
  OPT_Hints=0xD2,
  OPT_InputFile=0xE2,
  OPT_OutputFile=0xF2,
  OPT_Scale=0x101,
  OPT_Margins=0x112,
  OPT_Transparent=0x122;

/** @param s an option (lower/upper case intact), without leading `-'s
 * @param slen length of option 
 * @return          0  if invalid/unsupported/unknown option
 *       | 16*(k+1)+0  if the option never accepts parameters
 *       | 16*(k+2)+1  if the option may or may not have a parameter
 *       | 16*(k+3)+2  if the option must have a parameter
 */
static unsigned sam2p_optval(char const* s, slen_t slen) {
  if (slen==1) {
    switch (s[0]) {
     case 's': return OPT_SampleFormat;
     case 'l': return OPT_LoadHints;
     case 'h': return OPT_Hints;
     case 't': return OPT_TransferEncoding;
     case 'f': return OPT_TransferEncodingF;
     case 'c': return OPT_Compression;
     // case 'a': return OPT_Asis; /* disabled, automatic! */
     case '1': return OPT_PSL1;
     case '2': return OPT_PSL2;
     case '3': return OPT_PSL3;
     case 'j': return OPT_DisplayJobFile;
     case 'o': case 'O': return OPT_OutputFile;
     case 'e': return OPT_Scale;
     case 'm': return OPT_Margins;
    }
  } else {
    slen_t len=slen; /* strlen(s); */
    if (len>=32) return OPT_unknown;
    char buf[32];
    GenBuffer::tolower_memcpy(buf, s, len);
    buf[len]='\0';
    /* printf("buf=(%s)\n", buf); */
    if (0==strcmp(buf, "sampleformat")) return OPT_SampleFormat;
    if (0==strcmp(buf, "loadhints")) return OPT_LoadHints;
    if (0==strcmp(buf, "transparent")) return OPT_Transparent;
    if (0==strcmp(buf, "hints")) return OPT_Hints;
    if (0==strcmp(buf, "ps")
     || 0==strcmp(buf, "eps")) return OPT_PS;
    if (0==strcmp(buf, "pdf")) return OPT_PDF;
    if (0==strcmp(buf, "1c")) return OPT_PSLC;
    if (0==strcmp(buf, "scale")) return OPT_Scale;
    if (0==strcmp(buf, "hints")) return OPT_Margins;
  }
  return OPT_unknown;
}

static void displayJob(GenBuffer::Writable &sout, SimBuffer::Flat const& jobss) {
  fflush(stdout); fflush(stderr);
  #if 0
    puts("\n\n% begin sam2p job dump");
    fwrite(stdout, 1, jobss.getLength(), jobss);
    puts("% end sam2p job dump\n");
  #else
    /* vvv Dat: sout conflicts /OutputFile(-), but it is OK here */
    sout << "\n% begin sam2p job dump\n"
         << jobss 
         << "%)%)%)% end sam2p job dump\n\n";
  #endif
  fflush(stdout); fflush(stderr);
}

static inline char const *protect_null(char const *s) {
  return s==(char const*)NULLP ? "//" : s;
}

/** @return the value for the parameter if matches key or the first letter of
 * key; or NULLP if no match.
 * @param key lower case
 */
static char const*one_pabbr(char const*param, char const *key) {
  return GenBuffer::nocase_strbegins(param, key) ? param+strlen(key)
       : (param[0]==key[0] || param[0]==key[0]+'A'-'a') && param[1]==':' ? param+2
       : (char const*)NULLP;
}

#if 0 /* cannot set badp... */
static inline void one_setdimen2(char const*&Dimen1, char const*&Dimen2, char const*p2) {
  if (MiniPS::Real::isDimen(p2)) Dimen1=Dimen2=p2;
  else Error::sev(Error::ERROR_CONT) << "one_liner: dimen expected" /*<< (SimBuffer::B().appendDumpC(p2, true))*/ << (Error*)0;
}
#endif
#if 0
#define one_setdimen2(Dimen1,Dimen2,p2) do { \
  if (MiniPS::Real::isDimen(p2)) Dimen1=Dimen2=p2; \
  else { badmsg="one_liner: dimen expected: "; goto bad_label; } \
} while(0)
#endif

static Filter::UngetFILED *ufd;
static bool do_DisplayJobFile;
static bool buildProfile_quiet=false;
/** Creates an in-memory job file according to the command-line options.
 * @param a argv+1
 * @param job initially an empty string. On error, this function leaves it
 * as-is, but on success, a valid job file is appened.
 * @return true on syntax error
 */
static bool one_liner(SimBuffer::B &jobss, char const *const* a) {
  /* Tue Jul  2 21:27:15 CEST 2002 */
  char const *p, *pend;
  bool no_option_term=true;
  bool badp=false;
  bool no_selector=true;
  char const *badmsg;
  // SimBuffer::B Profile /* MiniPS code */
  SimBuffer::B Hints; /* MiniPS code */
  /* ^^^ Imp: separate hint for each -c arg?? */
  SimBuffer::B LoadHints;
  Rule::Cache::pr_t Predictor=Rule::Cache::PR_None; /* Imp: separate for each Compression */
  Rule::Cache::ff_t FileFormat=Rule::Cache::FF_default;
  Rule::Cache::te_t TransferEncoding=Rule::Cache::TE_default;
  do_DisplayJobFile=false;
  bool do_stop_SampleFormat=false;
  char const *Transparent=(char const*)NULLP; /* change 1 color to transparent unless NULL */
  char const *OutputFile=(char const*)NULLP, *InputFile=(char const*)NULLP;
  // SimBuffer::B tmp;
  char const *TopMargin=(char const*)NULLP, *BottomMargin=(char const*)NULLP, 
             *LeftMargin=(char const*)NULLP, *RightMargin=(char const*)NULLP,
             *LowerMargin=(char const*)NULLP, *ImageDPI=(char const*)NULLP;
  bool negLowerMargin=false;
  Rule::CacheHints::sc_t Scale=Rule::CacheHints::SC_default;
  #define APPEND_sf(val) do { if (sfx[val]==0) { sfx[val]=1; sft[sflen++]=val; } } while (0)
  Image::sf_t sft[Image::SF_max+1];       char sfx[Image::SF_max+1]; memset(sfx, 0, sizeof(sfx)); unsigned sflen=0;
  #define APPEND_co(val) do { if (cox[val]==0) { cox[val]=1; cot[colen++]=val; } } while (0)
  Rule::Cache::co_t cot[Rule::Cache::CO_max+1]; char cox[Image::SF_max+1]; memset(cox, 0, sizeof(cox)); unsigned colen=0;
  /* ^^^ BUGFIX at 2002.12.02 */
  
  /** OPT_* value of current option */
  unsigned opt;
  /** Parameter for current option. May be NULL. */
  char const *param=""; /* pacify VC6.0 */
  char const *p2;
  slen_t paramlen;
  SimBuffer::B tmpnam; /* any dir -- keep on stack frame */
  
  buildProfile_quiet=true;

  for (; (p=*a)!=(char const*)NULLP; a++) {
    if (p[0]=='-' && p[1]=='\0') { /* `-': Filename: STDIN or STDOUT */
      #if 1
        goto normal_label;
      #else
        Error::sev(Error::ERROR_CONT) << "one_liner: `-' (stdin|stdout) not allowed as filename" << (Error*)0;
        badp=true;
        continue;
      #endif
    } else if (p[0]=='-' && p[1]=='-' && p[2]=='\0') { /* `--': No more options */
      if (a[1]==(char const*)NULLP && InputFile!=(char const*)NULLP) { /* `--' is last argument */
        p=InputFile; /* OutputFile:=InputFile */
        goto normal_label;
      }
      no_option_term=false;
    } else if (p[0]=='-' && no_option_term) { /* an option */
      while (*p=='-') p++; /* short options (-p) and long options (--pdf) are equivalent */
      pend=p; while (*pend!='\0' && *pend!=':' && *pend!='=') pend++;
      SimBuffer::Static optss(p, pend-p);
      if (0==(opt=sam2p_optval(p, pend-p))) {
        badmsg="one_liner: unknown option: ";
       bad_label:
        Error::sev(Error::ERROR_CONT) << badmsg << (SimBuffer::B().appendDumpC(optss, true)) << (Error*)0;
        badp=true;
        continue;
      }
      if ((opt&15)==0) { /* no parameters */
        if (*pend!='\0') { badmsg="one_liner: don't give param to option: "; goto bad_label; }
        param=(char const*)NULLP;
      } else if ((opt&15)==1) { /* an optional parameter */
        if (*pend!='\0') {
          param=pend+1; while (*pend==*param) param++;
        } else param=(char const*)NULLP;
      } else if ((opt&15)==2) { /* a mandatory parameter */
        if (*pend!='\0') {
          param=pend+1; while (*pend==*param) param++;
        } else if ((param=*++a)==(char const*)NULLP) { badmsg="one_liner: missing param for option: "; goto bad_label; }
      } else assert(0);
      paramlen=param==(char const*)NULLP ? 0 : strlen(param);

      /* Dat: now opt, paramlen and param are correct */
      switch (opt) {
       case OPT_LoadHints: LoadHints << ',' << param; break;
       case OPT_Transparent: Transparent=param; break; /* Imp: is this good memory management */
       case OPT_Hints: Hints << '\n' << param; break;
       case OPT_PSL1: FileFormat=Rule::Cache::FF_PSL1; break;
       case OPT_PSLC: FileFormat=Rule::Cache::FF_PSLC; break;
       case OPT_PSL2: FileFormat=Rule::Cache::FF_PSL2; break;
       case OPT_PSL3: FileFormat=Rule::Cache::FF_PSL3; break;
       case OPT_DisplayJobFile:
        // fprintf(stderr, "param=(%s)\n", param);
             if (paramlen==0 || 0==GenBuffer::nocase_strcmp(param, "job"))  do_DisplayJobFile=true;
        else if (0==GenBuffer::nocase_strcmp(param, "warn")) buildProfile_quiet=false;
        else if (GenBuffer::nocase_strbegins(param, "warn:")) buildProfile_quiet=!GenBuffer::parseBool(param+5, paramlen-5);
        else if (0==GenBuffer::nocase_strcmp(param, "quiet")) { buildProfile_quiet=true; Error::setTopPrinted(Error::ERROR_CONT); } /* Dat: hide warnings, info etc. */ /* at Fri Aug 26 07:54:00 CEST 2005 */
        else if (GenBuffer::nocase_strbegins(param, "job:")) do_DisplayJobFile=GenBuffer::parseBool(param+4, paramlen-4);
        else do_DisplayJobFile=GenBuffer::parseBool(param, paramlen); /* Imp: better error report */
        break;
       case OPT_Margins:
        /* Dat:abbreviation letters are distinct: [ahxvylrtubd] */
             if (0!=(p2=one_pabbr(param,"all:"))) { all5:
          if (MiniPS::Real::isDimen(p2)) LeftMargin=RightMargin=TopMargin=BottomMargin=LowerMargin=p2;
          else { err_dimexp: badmsg="one_liner: dimen expected: "; goto bad_label; }
        }
        else if (0!=(p2=one_pabbr(param,"horiz:"))
              || 0!=(p2=one_pabbr(param,"x:"))) { if (MiniPS::Real::isDimen(p2)) LeftMargin=RightMargin=p2; else goto err_dimexp; }
        else if (0!=(p2=one_pabbr(param,"vert:"))
              || 0!=(p2=one_pabbr(param,"y:"))) { if (MiniPS::Real::isDimen(p2)) TopMargin=BottomMargin=LowerMargin=p2; else goto err_dimexp; }
        else if (0!=(p2=one_pabbr(param,"left:"))) { if (MiniPS::Real::isDimen(p2)) LeftMargin=p2; else goto err_dimexp; }
        else if (0!=(p2=one_pabbr(param,"right:"))) { if (MiniPS::Real::isDimen(p2)) RightMargin=p2; else goto err_dimexp; }
        else if (0!=(p2=one_pabbr(param,"dpi:"))) { if (MiniPS::Real::isDimen(p2)) ImageDPI=p2; else goto err_dimexp; }
        else if (0!=(p2=one_pabbr(param,"top:"))
              || 0!=(p2=one_pabbr(param,"up:"))) { if (MiniPS::Real::isDimen(p2)) TopMargin=p2; else goto err_dimexp; }
        else if (0!=(p2=one_pabbr(param,"bottom:"))
              || 0!=(p2=one_pabbr(param,"down:"))) { if (MiniPS::Real::isDimen(p2)) BottomMargin=LowerMargin=p2; else goto err_dimexp; }
        else if (0!=(p2=one_pabbr(param,"raise:"))) { if (MiniPS::Real::isDimen(p2)) LowerMargin=p2; else goto err_dimexp; }
        else if (0!=(p2=one_pabbr(param,"lower:"))) { negLowerMargin=true; if (MiniPS::Real::isDimen(p2)) LowerMargin=p2; else goto err_dimexp; }
        else { p2=param; goto all5; }
        break;
       case OPT_Scale:
             if (paramlen==0 || 0==GenBuffer::nocase_strcmp(param, "scale")) Scale=Rule::CacheHints::SC_OK;
        else if (0==GenBuffer::nocase_strcmp(param, "none")) Scale=Rule::CacheHints::SC_None;
        else if (0==GenBuffer::nocase_strcmp(param, "rot")) Scale=Rule::CacheHints::SC_RotateOK;
        else if (0==GenBuffer::nocase_strcmp(param, "rotate")) Scale=Rule::CacheHints::SC_RotateOK;
        else Scale=(Rule::CacheHints::sc_t)(GenBuffer::parseBool(param, paramlen) ? 0+Rule::CacheHints::SC_OK : 0+Rule::CacheHints::SC_None);
        /* ^^^ Imp: better error report */
        /* ^^^ +0: pacify g++-3.1 */
        break;
       case OPT_TransferEncoding:
             if (0==GenBuffer::nocase_strcmp(param, "bin"))  TransferEncoding=Rule::Cache::TE_Binary;
        else if (0==GenBuffer::nocase_strcmp(param, "hex"))  TransferEncoding=Rule::Cache::TE_Hex;
        else if (0==GenBuffer::nocase_strcmp(param, "a85"))  TransferEncoding=Rule::Cache::TE_A85;
        else if (0==GenBuffer::nocase_strcmp(param, "ascii"))TransferEncoding=Rule::Cache::TE_ASCII;
        else if (0==GenBuffer::nocase_strcmp(param, "lsb1")) TransferEncoding=Rule::Cache::TE_LSBfirst;
        else if (0==GenBuffer::nocase_strcmp(param, "msb1")) TransferEncoding=Rule::Cache::TE_MSBfirst;
        else { inv_par: badmsg="one_liner: invalid param for option: "; goto bad_label; }
        break;
       case OPT_TransferEncodingF:
             if (0==GenBuffer::nocase_strcmp(param, "lsb2msb")) TransferEncoding=Rule::Cache::TE_LSBfirst;
        else if (0==GenBuffer::nocase_strcmp(param, "msb2lsb")) TransferEncoding=Rule::Cache::TE_MSBfirst;
        else goto inv_par;
        break;
       case OPT_PS:
             if (param==(char const*)NULLP || param[0]=='\0') FileFormat=Rule::Cache::FF_eps;
        else if (0==GenBuffer::nocase_strcmp(param, "1")) FileFormat=Rule::Cache::FF_PSL1;
        else if (0==GenBuffer::nocase_strcmp(param, "1c")
              || 0==GenBuffer::nocase_strcmp(param, "c")) FileFormat=Rule::Cache::FF_PSLC;
        else if (0==GenBuffer::nocase_strcmp(param, "2")) FileFormat=Rule::Cache::FF_PSL2;
        else if (0==GenBuffer::nocase_strcmp(param, "3")) FileFormat=Rule::Cache::FF_PSL3;
        else goto inv_par;
        break;
       case OPT_PDF:
             if (param==(char const*)NULLP || param[0]=='\0') FileFormat=Rule::Cache::FF_pdfb;
        else if (0==GenBuffer::nocase_strcmp(param, "b")) FileFormat=Rule::Cache::FF_pdfb; /* BI */
        else if (0==GenBuffer::nocase_strcmp(param, "x")) FileFormat=Rule::Cache::FF_pdf; /* XObject */
        else if (0==GenBuffer::nocase_strcmp(param, "b0")) FileFormat=Rule::Cache::FF_PDFB10;
        else if (0==GenBuffer::nocase_strcmp(param, "b2")) FileFormat=Rule::Cache::FF_PDFB12;
        else if (0==GenBuffer::nocase_strcmp(param, "0"))  FileFormat=Rule::Cache::FF_PDF10;
        else if (0==GenBuffer::nocase_strcmp(param, "2"))  FileFormat=Rule::Cache::FF_PDF12;
        else goto inv_par;
        break;
       case OPT_SampleFormat:
        pend=param;
        while (*pend!='\0') {
          while (*pend==':') pend++;
          if (do_stop_SampleFormat) goto inv_par; /* already stopped */
          p=pend; while (*pend!='\0' && *pend!=':')  pend++;
          paramlen=pend-p;
          if (4==paramlen && 0==memcmp(p,"stop",4)) { do_stop_SampleFormat=true; buildProfile_quiet=false; break; }
          if (5==paramlen && 0==memcmp(p,"stopq",5)) { do_stop_SampleFormat=true; break; }
          if (2==paramlen && 0==memcmp(p,"tr",2)) {
            APPEND_sf(Image::SF_Transparent);
            APPEND_sf(Image::SF_Opaque);
            APPEND_sf(Image::SF_Mask);
            APPEND_sf(Image::SF_Transparent2);
            APPEND_sf(Image::SF_Transparent4);
            APPEND_sf(Image::SF_Transparent8);
          } else {
            Image::sf_t sf=Rule::Cache::parseSampleFormat(p, paramlen);
            if (sf==Image::SF_max) goto inv_par;
            APPEND_sf(sf);
          }
        }
        break;
       case OPT_Compression:
        assert(param!=(char const*)NULLP);
             if (0==GenBuffer::nocase_strcmp(param, "none")) APPEND_co(Rule::Cache::CO_None);
        else if (0==GenBuffer::nocase_strcmp(param, "lzw"))  APPEND_co(Rule::Cache::CO_LZW);
        else if (0==GenBuffer::nocase_strcmp(param, "zip"))  APPEND_co(Rule::Cache::CO_ZIP);
        else if (0==GenBuffer::nocase_strcmp(param, "rle")
              || 0==GenBuffer::nocase_strcmp(param, "packbits")) APPEND_co(Rule::Cache::CO_RLE);
        else if (0==GenBuffer::nocase_strcmp(param, "dct"))  { APPEND_co(Rule::Cache::CO_DCT); Hints << "\n/DCT<<>>"; }
        else if (0==GenBuffer::nocase_strcmp(param, "jpg")
              || 0==GenBuffer::nocase_strcmp(param, "jpeg")) { APPEND_co(Rule::Cache::CO_JAI); APPEND_co(Rule::Cache::CO_IJG); }
        else if (0==GenBuffer::nocase_strcmp(param, "ijg"))  APPEND_co(Rule::Cache::CO_IJG);
        else if (0==GenBuffer::nocase_strcmp(param, "g4"))   { APPEND_co(Rule::Cache::CO_Fax); Hints << "\n/K -1"; }
        else if (0==GenBuffer::nocase_strcmp(param, "g3")
              || 0==GenBuffer::nocase_strcmp(param, "fax")
              || 0==GenBuffer::nocase_strcmp(param, "g3:1d"))APPEND_co(Rule::Cache::CO_Fax);
        else if (0==GenBuffer::nocase_strcmp(param, "g3:2d")){ APPEND_co(Rule::Cache::CO_Fax); Hints << "\n/K -2"; }
        else if (0==GenBuffer::nocase_strcmp(param, "jai"))  APPEND_co(Rule::Cache::CO_JAI);
        else if (GenBuffer::nocase_strbegins(param, "lzw:")) {
          SimBuffer::Static s(param+4);
          unsigned long i; /* toInteger() is quickest on long */
          if (s.toInteger(i)) goto inv_par;
          APPEND_co(Rule::Cache::CO_LZW); Predictor=(Rule::Cache::pr_t)i;
        } else if (GenBuffer::nocase_strbegins(param, "rle:")) {
          SimBuffer::Static s(param+4);
          unsigned long i; /* toInteger() is quickest on long */
          if (s.toInteger(i)) goto inv_par;
          APPEND_co(Rule::Cache::CO_RLE); Hints << "\n/RecordSize " << i;
        } else if (GenBuffer::nocase_strbegins(param, "packbits:")) {
          SimBuffer::Static s(param+9);
          unsigned long i; /* toInteger() is quickest on long */
          if (s.toInteger(i)) goto inv_par;
          APPEND_co(Rule::Cache::CO_RLE); Hints << "\n/RecordSize " << i;
        } else if (GenBuffer::nocase_strbegins(param, "fax:")) {
          SimBuffer::Static s(param+4);
          long i; /* toInteger() is quickest on long */
          if (s.toInteger(i)) goto inv_par;
          APPEND_co(Rule::Cache::CO_Fax); Hints << "\n/K " << i;
        } else if (GenBuffer::nocase_strbegins(param, "dct:")) {
          APPEND_co(Rule::Cache::CO_DCT); Hints << "\n/DCT<< " << (param+4) << " >>";
        } else if (GenBuffer::nocase_strbegins(param, "ijg:")) {
          SimBuffer::Static s(param+4);
          unsigned long i; /* toInteger() is quickest on long */
          if (s.toInteger(i)) goto inv_par;
          APPEND_co(Rule::Cache::CO_IJG); Hints << "\n/Quality " << i;
        } else if (GenBuffer::nocase_strbegins(param, "jpeg:")) {
          SimBuffer::Static s(param+5);
          unsigned long i; /* toInteger() is quickest on long */
          if (s.toInteger(i)) goto inv_par;
          APPEND_co(Rule::Cache::CO_JAI);
          APPEND_co(Rule::Cache::CO_IJG); Hints << "\n/Quality " << i;
        } else if (GenBuffer::nocase_strbegins(param, "zip:")) {
          pend=param+4; while (*pend!='\0' && *pend!=':') pend++;
          SimBuffer::Static s(param+4, pend-param-4);
          unsigned long i; /* toInteger() is quickest on long */
          if (s.toInteger(i)) goto inv_par;
          APPEND_co(Rule::Cache::CO_ZIP); Predictor=(Rule::Cache::pr_t)i;
          if (*pend==':') {
            pend++; while (*pend==':') pend++;
            SimBuffer::Static s(pend);
            if (s.toInteger(i)) goto inv_par;
            // Error::sev(Error::FATAL) << "SUXX)" << s << ',' << i << '.' << (Error*)0;
            APPEND_co(Rule::Cache::CO_ZIP); Hints << "\n/Effort " << i;
            // Error::sev(Error::FATAL) << "SUXX(" << s << ',' << Hints << '.' << (Error*)0;
          }            
        } else goto inv_par;
        break;
       default: assert(0);
      }
    } else { /* a selector or a normal argument */
      if (!(no_selector && no_option_term)) goto normal_label;
      pend=p; while (*pend!='\0' && *pend!=':') pend++;
      if (pend-p>=2 && *pend==':') { /* an ImageMagick-style FileFormat selector */
             if (GenBuffer::nocase_strbegins(p, "PSL2:")
              || GenBuffer::nocase_strbegins(p, "EPS2:")) FileFormat=Rule::Cache::FF_PSL2;
        else if (GenBuffer::nocase_strbegins(p, "EPS:"))  FileFormat=Rule::Cache::FF_eps;
        else if (GenBuffer::nocase_strbegins(p, "PS2:"))  { FileFormat=Rule::Cache::FF_PSL2; if (Scale==Rule::CacheHints::SC_default) Scale=Rule::CacheHints::SC_RotateOK; }
        else if (GenBuffer::nocase_strbegins(p, "PS:"))   { FileFormat=Rule::Cache::FF_eps;  if (Scale==Rule::CacheHints::SC_default) Scale=Rule::CacheHints::SC_RotateOK; }
        else if (GenBuffer::nocase_strbegins(p, "PSL1:")) FileFormat=Rule::Cache::FF_PSL1;
        else if (GenBuffer::nocase_strbegins(p, "PSLC:")) FileFormat=Rule::Cache::FF_PSLC;
        else if (GenBuffer::nocase_strbegins(p, "PSL3:")) FileFormat=Rule::Cache::FF_PSL3;
        else if (GenBuffer::nocase_strbegins(p, "PDF:")
              || GenBuffer::nocase_strbegins(p, "PDF:"))  FileFormat=Rule::Cache::FF_pdfb;
        else if (GenBuffer::nocase_strbegins(p, "PDFX:")) FileFormat=Rule::Cache::FF_pdf;
        else if (GenBuffer::nocase_strbegins(p, "PDFB1.0:")) FileFormat=Rule::Cache::FF_PDFB10;
        else if (GenBuffer::nocase_strbegins(p, "PDFB1.2:")) FileFormat=Rule::Cache::FF_PDFB12;
        else if (GenBuffer::nocase_strbegins(p, "PDF1.0:"))  FileFormat=Rule::Cache::FF_PDF10;
        else if (GenBuffer::nocase_strbegins(p, "PDF1.2:"))  FileFormat=Rule::Cache::FF_PDF12;
        else if (GenBuffer::nocase_strbegins(p, "GIF89a:")
              || GenBuffer::nocase_strbegins(p, "GIF:"))  FileFormat=Rule::Cache::FF_GIF89a;
        /* vvv do_stop_SampleFormat BUGFIX at Thu Nov 21 23:44:20 CET 2002 */
        else if (GenBuffer::nocase_strbegins(p, "PNM:"))  { FileFormat=Rule::Cache::FF_PNM; APPEND_sf(Image::SF_Gray1); APPEND_sf(Image::SF_Gray8); APPEND_sf(Image::SF_Rgb8); APPEND_sf(Image::SF_Transparent8); do_stop_SampleFormat=true; }
        else if (GenBuffer::nocase_strbegins(p, "PBM:"))  { FileFormat=Rule::Cache::FF_PNM; APPEND_sf(Image::SF_Gray1); do_stop_SampleFormat=true; }
        else if (GenBuffer::nocase_strbegins(p, "PGM:"))  { FileFormat=Rule::Cache::FF_PNM; APPEND_sf(Image::SF_Gray8); do_stop_SampleFormat=true; }
        else if (GenBuffer::nocase_strbegins(p, "PPM:"))  { FileFormat=Rule::Cache::FF_PNM; APPEND_sf(Image::SF_Rgb8);  do_stop_SampleFormat=true; }
        else if (GenBuffer::nocase_strbegins(p, "PAM:"))  FileFormat=Rule::Cache::FF_PAM;
        else if (GenBuffer::nocase_strbegins(p, "PIP:"))  FileFormat=Rule::Cache::FF_PIP;
        else if (GenBuffer::nocase_strbegins(p, "Empty:"))FileFormat=Rule::Cache::FF_Empty;
        else if (GenBuffer::nocase_strbegins(p, "Meta:")) FileFormat=Rule::Cache::FF_Meta;
        else if (GenBuffer::nocase_strbegins(p, "JPEG:")
              || GenBuffer::nocase_strbegins(p, "JPG:"))  FileFormat=Rule::Cache::FF_JPEG;
        else if (GenBuffer::nocase_strbegins(p, "TIFF:")
              || GenBuffer::nocase_strbegins(p, "TIF:"))  FileFormat=Rule::Cache::FF_TIFF;
        else if (GenBuffer::nocase_strbegins(p, "PNG:"))  FileFormat=Rule::Cache::FF_PNG;
        else if (GenBuffer::nocase_strbegins(p, "XPM:"))  FileFormat=Rule::Cache::FF_XPM;
        else if (GenBuffer::nocase_strbegins(p, "BMP:")
              || GenBuffer::nocase_strbegins(p, "RLE:"))  { FileFormat=Rule::Cache::FF_BMP; /*APPEND_co(Rule::Cache::CO_RLE);*/ }
        else if (GenBuffer::nocase_strbegins(p, "XWD:"))  FileFormat=Rule::Cache::FF_XWD;
        else if (GenBuffer::nocase_strbegins(p, "X11:"))  FileFormat=Rule::Cache::FF_X11;
        else {
          Error::sev(Error::ERROR_CONT) << "one_liner: invalid FileFormat selector: " << (SimBuffer::B().appendDumpC(SimBuffer::Static(p, pend-p), true)) << (Error*)0;
          badp=true;
          continue;
        }
        no_selector=false;
        while (*pend==':') pend++;
        if (*pend=='\0') continue; /* this arg is just a selector */
        /* normal argument is coming */
        p=pend;
      }
     normal_label:
      if (InputFile==(char const*)NULLP) {
        InputFile=p; /* set it even on error */
	/* vvv since Sat Apr 19 13:37:57 CEST 2003:
	 * Possibly unseekable STDIN is handled by Filter::UngetFILED
	 */
	#if 0
	  if (p[0]=='-' && p[1]=='\0') { /* Filename: STDIN */
	    Files::set_binary_mode(0, true);
	    if (0!=fseek(stdin, 0L, 0)) { unseekable:
	      #if 0
		Error::sev(Error::ERROR_CONT) << "one_liner: `-' (stdin) not allowed as InputFile (stdin unseekable)" << (Error*)0;
		badp=true; continue;
	      #endif
	      FILE *f=Files::open_tmpnam(tmpnam);
	      if (!f) { Error::sev(Error::ERROR_CONT) << "one_liner: cannot open" << " temporary file for `-' (stdin)" << (Error*)0; badp=true; continue; }
	      tmpnam.term0();
	      Files::tmpRemoveCleanup(InputFile=tmpnam());
	      char *buf=new char[4096];
	      unsigned got;
	      while (
		   (0<(got=fread(buf, 1, sizeof(buf), stdin)))
		&& got==fwrite(buf, 1, sizeof(buf), f)) {}
	      delete [] buf;
	      if (ferror(f) || 0!=fclose(f) || ferror(stdin))
		{ Error::sev(Error::ERROR_CONT) << "one_liner: cannot write" << " temporary file for `-' (stdin)" << (Error*)0; badp=true; continue; }
	      // fprintf(stderr, "if=(%s)\n", InputFile);
	      continue;
	      /* Imp: report `-' as filename on errors etc. */
	    }
	    int c=MACRO_GETC(stdin);
	    if (0!=fseek(stdin, 0L, 0)) { if (c>=0) ungetc(c, stdin); goto unseekable; } /* active test */
	    /* ungetc() is not necessary because of fseek() */
	  }
	#endif
      } else if (OutputFile==(char const*)NULLP) { /* Filename: STDOUT */
        Files::set_binary_mode(1, true);
        OutputFile=p;
        if (FileFormat==Rule::Cache::FF_default) { /* OutputFile; determine FileFormat from extension */
          pend=p+strlen(p);
          while (pend!=p && *pend!='.') pend--;
          /* ^^^ Dat: extra care later for /a/b.c/d */
          if (pend!=p) { /* have extension */
            assert(*pend=='.'); pend++;
                 if (0==GenBuffer::nocase_strcmp(pend, "eps")
                  || 0==GenBuffer::nocase_strcmp(pend, "epsi")
                  || 0==GenBuffer::nocase_strcmp(pend, "epsf")) FileFormat=Rule::Cache::FF_eps;
            else if (0==GenBuffer::nocase_strcmp(pend, "ps"))   { FileFormat=Rule::Cache::FF_eps; if (Scale==Rule::CacheHints::SC_default) Scale=Rule::CacheHints::SC_RotateOK; }
            else if (0==GenBuffer::nocase_strcmp(pend, "pdf"))  FileFormat=Rule::Cache::FF_pdfb;
            else if (0==GenBuffer::nocase_strcmp(pend, "gif"))  FileFormat=Rule::Cache::FF_GIF89a;
            else if (0==GenBuffer::nocase_strcmp(pend, "pnm"))  { FileFormat=Rule::Cache::FF_PNM; APPEND_sf(Image::SF_Gray1); APPEND_sf(Image::SF_Gray8); APPEND_sf(Image::SF_Rgb8); APPEND_sf(Image::SF_Transparent8); do_stop_SampleFormat=true; }
            else if (0==GenBuffer::nocase_strcmp(pend, "pbm"))  { FileFormat=Rule::Cache::FF_PNM; APPEND_sf(Image::SF_Gray1); do_stop_SampleFormat=true; }
            else if (0==GenBuffer::nocase_strcmp(pend, "pgm"))  { FileFormat=Rule::Cache::FF_PNM; APPEND_sf(Image::SF_Gray8); do_stop_SampleFormat=true; }
            else if (0==GenBuffer::nocase_strcmp(pend, "ppm"))  { FileFormat=Rule::Cache::FF_PNM; APPEND_sf(Image::SF_Rgb8);  do_stop_SampleFormat=true; }
            else if (0==GenBuffer::nocase_strcmp(pend, "pam"))  FileFormat=Rule::Cache::FF_PAM;
            else if (0==GenBuffer::nocase_strcmp(pend, "pip"))  FileFormat=Rule::Cache::FF_PIP;
            else if (0==GenBuffer::nocase_strcmp(pend, "empty"))FileFormat=Rule::Cache::FF_Empty;
            else if (0==GenBuffer::nocase_strcmp(pend, "meta")) FileFormat=Rule::Cache::FF_Meta;
            else if (0==GenBuffer::nocase_strcmp(pend, "jpeg")
                  || 0==GenBuffer::nocase_strcmp(pend, "jpg"))  FileFormat=Rule::Cache::FF_JPEG;
            else if (0==GenBuffer::nocase_strcmp(pend, "tiff")
                  || 0==GenBuffer::nocase_strcmp(pend, "tif"))  FileFormat=Rule::Cache::FF_TIFF;
            else if (0==GenBuffer::nocase_strcmp(pend, "png"))  FileFormat=Rule::Cache::FF_PNG;
            else if (0==GenBuffer::nocase_strcmp(pend, "xpm"))  FileFormat=Rule::Cache::FF_XPM;
            else if (0==GenBuffer::nocase_strcmp(pend, "bmp")
                  || 0==GenBuffer::nocase_strcmp(pend, "rle"))  { FileFormat=Rule::Cache::FF_BMP; /*APPEND_co(Rule::Cache::CO_RLE);*/ }
            else if (0==GenBuffer::nocase_strcmp(pend, "xwd"))  FileFormat=Rule::Cache::FF_XWD;
            // else if (0==GenBuffer::nocase_strcmp(pend, "x11"))  FileFormat=Rule::Cache::FF_X11;
            /* ^^^ .x11 extension is useless */
          }
        }
      } else {
        Error::sev(Error::ERROR_CONT) << "one_liner: got too many (>2) filenames" << (Error*)0;
        badp=true;
        continue;
      }
    } /* IF */
  } /* NEXT */
  
  if (InputFile==(char const*)NULLP) {
    Error::sev(Error::ERROR_CONT) << "one_liner: InputFile unspecified" << (Error*)0;
    badp=true;
  }
  if (OutputFile==(char const*)NULLP) {
    Error::sev(Error::ERROR_CONT) << "one_liner: OutputFile unspecified" << (Error*)0;
    badp=true;
  }
  if (FileFormat==Rule::Cache::FF_default) {
    Error::sev(Error::ERROR_CONT) << "one_liner: FileFormat unspecified" << (Error*)0;
    badp=true;
  }
  if (badp) return true;
  
  if (colen==0) { /* apply default if Compression is unspecified */
    // Error::sev(Error::FATAL) << "FileFormat=" << (unsigned)FileFormat << (Error*)0;
    switch (FileFormat) {
     case Rule::Cache::FF_TIFF:
     case Rule::Cache::FF_eps: case Rule::Cache::FF_PSL2:
     case Rule::Cache::FF_PDFB10: case Rule::Cache::FF_PDF10:
      APPEND_co(Rule::Cache::CO_JAI);
      #if HAVE_LZW
       APPEND_co(Rule::Cache::CO_LZW);
      #endif
      APPEND_co(Rule::Cache::CO_RLE);
      APPEND_co(Rule::Cache::CO_None); break;
     case Rule::Cache::FF_BMP:
     case Rule::Cache::FF_PSL1: case Rule::Cache::FF_PSLC:
      /* assert(0); */
      APPEND_co(Rule::Cache::CO_RLE);
      APPEND_co(Rule::Cache::CO_None); break;
     case Rule::Cache::FF_PSL3:
     case Rule::Cache::FF_pdfb: /* BUGFIX at Sun Sep 22 14:57:03 CEST 2002 */
     case Rule::Cache::FF_pdf:
     case Rule::Cache::FF_PDFB12: case Rule::Cache::FF_PDF12:
      APPEND_co(Rule::Cache::CO_JAI);
      APPEND_co(Rule::Cache::CO_ZIP);
      APPEND_co(Rule::Cache::CO_None); break;
     case Rule::Cache::FF_GIF89a:
      APPEND_co(Rule::Cache::CO_LZW); /* _not_ /LZWEncode filter */
      APPEND_co(Rule::Cache::CO_None); break;
     case Rule::Cache::FF_JPEG:
      APPEND_co(Rule::Cache::CO_JAI);
      APPEND_co(Rule::Cache::CO_IJG); break;
     case Rule::Cache::FF_PNG:
      APPEND_co(Rule::Cache::CO_ZIP); break;
     default:
      APPEND_co(Rule::Cache::CO_None); break;
     /* for others FileFormats: CO_None, but that is _not_ appended anyway */
    }
  }
  /* Dat: don't append /Compression/None if the user has specified `-c'. The
   *      user can append `-c none' manually.
   */

  if (Scale==Rule::CacheHints::SC_default) Scale=Rule::CacheHints::SC_None;
  if (TransferEncoding==Rule::Cache::TE_default) { /* apply default if TransferEncoding is unspecified */
    switch (FileFormat) {
     case Rule::Cache::FF_PSL1: case Rule::Cache::FF_PSLC:
      TransferEncoding=(Rule::Cache::te_t)(cox[Rule::Cache::CO_ZIP]!=0 ||
        cox[Rule::Cache::CO_LZW]!=0 ? 0+Rule::Cache::TE_A85 : 0+Rule::Cache::TE_Hex); break;
      /* ^^^ Dat: /Hex is default for /RLE */
     case Rule::Cache::FF_eps: case Rule::Cache::FF_PSL2:
     case Rule::Cache::FF_PSL3:
      TransferEncoding=Rule::Cache::TE_A85; break;
    #if 0 /* useless bugfix */
     case Rule::Cache::FF_XPM: /* BUGFIX at Thu Jul 11 21:53:56 CEST 2002 */
      // assert(0);
      TransferEncoding=Rule::Cache::TE_ASCII; break;
    #endif
     default:
      TransferEncoding=Rule::Cache::TE_Binary; break;
    }
  }

  unsigned coc=colen;

  /* Smart verify whether /Compression/JAI is requested. If so, and the input
   * file is JPEG, then load it as-is. Our heuristic is that if the user
   * allowed /Compression/JAI among others, and the input file is a baseline
   * JPEG, then /Compression/JAI will be the best (and only) compression
   * method.
   */
  bool jaip=cox[Rule::Cache::CO_JAI]!=0;
  if (jaip) {
    if (ufd==NULLP) ufd=new Filter::UngetFILED(InputFile, stdin,
      Filter::UngetFILED::CM_closep|Filter::UngetFILED::CM_keep_stdinp);
    /* vvv Imp: no error handling */
    if (1!=jai_is_baseline_jpeg(ufd)) { jaip=false; coc--; cox[Rule::Cache::CO_JAI]=0; }
    ufd->seek(0);
#if 0
    assert(ufd->vi_getcc()==255);
    assert(ufd->vi_getcc()==0xd8);
    assert(ufd->vi_getcc()==255+0);
    assert(0);
#endif
  }
  
  if (jaip) { /* Dat: might have changed to false */
    APPEND_sf(Image::SF_Asis);
    LoadHints << ",jpeg-asis,";
    cot[0]=Rule::Cache::CO_JAI; colen=1; /* disable all other compression */
  } else if (coc==1 && cox[Rule::Cache::CO_Fax]!=0) {
    APPEND_sf(Image::SF_Opaque);
    APPEND_sf(Image::SF_Transparent);
    APPEND_sf(Image::SF_Gray1);
    APPEND_sf(Image::SF_Indexed1);
    APPEND_sf(Image::SF_Mask);
  } else if (coc==1 && (cox[Rule::Cache::CO_DCT]!=0 || cox[Rule::Cache::CO_IJG]!=0)) {
    APPEND_sf(Image::SF_Opaque);
    APPEND_sf(Image::SF_Transparent);
    APPEND_sf(Image::SF_Gray8);
    APPEND_sf(Image::SF_Rgb8);
  } else if (!do_stop_SampleFormat) {
    /* JAI compression is automatically disabled since jaip==false */
    /* Guess SampleFormat automatically. The order is significant, so most
     * warnings emitted by Image::SampledInfo::setSampleFormat() are avoided.
     */
    APPEND_sf(Image::SF_Opaque);
    APPEND_sf(Image::SF_Transparent);
    APPEND_sf(Image::SF_Gray1);
    APPEND_sf(Image::SF_Indexed1);
    APPEND_sf(Image::SF_Mask);
    APPEND_sf(Image::SF_Gray2);
    APPEND_sf(Image::SF_Indexed2);
    APPEND_sf(Image::SF_Rgb1);
    APPEND_sf(Image::SF_Gray4);
    APPEND_sf(Image::SF_Indexed4);
    APPEND_sf(Image::SF_Rgb2);
    APPEND_sf(Image::SF_Gray8);
    APPEND_sf(Image::SF_Indexed8);
    APPEND_sf(Image::SF_Rgb4);
    APPEND_sf(Image::SF_Rgb8);
    APPEND_sf(Image::SF_Transparent2); /* transparents are put last because of expensive color separation */
    APPEND_sf(Image::SF_Transparent4);
    APPEND_sf(Image::SF_Transparent8);
  }
  
  /* Append more Hints */
  if (Scale!=Rule::CacheHints::SC_None) Hints << "\n/Scale /" << protect_null(Rule::CacheHints::dumpScale(Scale));
  if ((char const*)NULLP!=ImageDPI) Hints << "\n/ImageDPI " << ImageDPI;
  if ((char const*)NULLP!=LeftMargin) Hints << "\n/LeftMargin " << LeftMargin;
  if ((char const*)NULLP!=RightMargin) Hints << "\n/RightMargin " << RightMargin;
  if ((char const*)NULLP!=TopMargin) Hints << "\n/TopMargin " << TopMargin;
  if ((char const*)NULLP!=BottomMargin) Hints << "\n/BottomMargin " << BottomMargin;
  if ((char const*)NULLP!=LowerMargin) {
    Hints << "\n/LowerMargin ";
    if (negLowerMargin) {
      if (LowerMargin[0]=='-') Hints << (LowerMargin+1);
                          else Hints << '-' << LowerMargin;
    } else Hints << LowerMargin;
  }
  
  /* Emit job file */
  jobss << "<<%sam2p in-memory job file, autogenerated by " << Error::banner0
        << "\n/InputFile ";
  jobss.appendDumpPS(SimBuffer::Static(InputFile), true);
  jobss << "\n/OutputFile ";
  jobss.appendDumpPS(SimBuffer::Static(OutputFile), true);
  jobss << "\n/LoadHints ";
  jobss.appendDumpPS(LoadHints, true);
  jobss << "\n/Profile[\n";
  
  unsigned coi, sfi;
  slen_t orc=0;
  for (sfi=0;sfi<sflen;sfi++) { for (coi=0;coi<colen;coi++) {
  // for (coi=0;coi<colen;coi++) for (sfi=0;sfi<sflen;sfi++)
      if (cot[coi]==Rule::Cache::CO_JAI) {
        if (!jaip || sft[sfi]!=Image::SF_Asis) continue;
      }
      jobss << "<<% OutputRule #" << orc++
            << "\n  /FileFormat /" << protect_null(Rule::Cache::dumpFileFormat(FileFormat, cot[coi]))
            << "\n  /TransferEncoding /" << protect_null(Rule::Cache::dumpTransferEncoding(TransferEncoding))
            << "\n  /SampleFormat /" << protect_null(Rule::Cache::dumpSampleFormat(sft[sfi]))
            << "\n  /Compression /"  << protect_null(Rule::Cache::dumpCompression (cot[coi]))
            << "\n  /Predictor " << (cot[coi]==Rule::Cache::CO_LZW || cot[coi]==Rule::Cache::CO_ZIP ? Predictor : 1)
            << "\n  /Hints << " << Hints << " >>";
      // jobss << "  /Transparent (\377\377\377)\n";
      if (Transparent!=NULL) {
        jobss << "\n  /Transparent ";
        jobss.appendDumpPS(SimBuffer::Static(Transparent), true);
      }
      jobss  << "\n>>\n";
    }
  }
  jobss << "]>>% __EOF__\n";

  /* Common keys not emited here:
   * /TmpRemove true  /Templates pop
   *    /ColorTransform pop
   *    /EncoderColumns 0  /EncoderBPL 0 /EncoderRows 0  /EncoderColors 0
   *    /PredictorColumns 0   /PredictorBPC 0  /PredictorColors 0
   *  /Transparent null
   *  /WarningOK true
   *    /Comment  pop
   *    /Title    pop
   *    /Subject  pop
   *    /Author   pop
   *    /Creator  pop
   *    /Producer pop
   *    /Created  pop
   *    /Produced pop
   */
  return false;
  #undef APPEND_sf
  #undef APPEND_co
} /* one_liner() */

static bool option_eq(char const *arg, char const*option) {
  if (option[0]=='-') { option++; while (arg[0]=='-') arg++; }
  return 0==GenBuffer::nocase_strcmp(arg, option);
}

void init_sam2p_engine(char const*argv0) {
  Error::long_argv0=argv0==(char const*)NULLP ? "sam2p" : argv0;
  Error::argv0=Files::only_fext(Error::long_argv0);
  Error::tmpargv0="_sam2p_";
  Error::banner0="sam2p v0.44";
}

/* --- */

void run_sam2p_engine(Files::FILEW &sout, Files::FILEW &serr, char const*const*argv1, bool helpp) {
  Error::serr=&serr;

  /* --- Parse arguments, generate/read .job file */
  
  MiniPS::VALUE job=MiniPS::Qundef;
  Filter::FlatD bts(bts_ttt);
  ufd=(Filter::UngetFILED*)NULLP;
  if (!helpp && argv1[0]!=(char const*)NULLP && argv1[1]==(char const*)NULLP) {
    /* A single argument: must be the name of the .job file */
    MiniPS::Parser p(argv1[0]);
    p.addSpecRun("%bts", &bts); /* bts: Built-in TemplateS, the bts.ttt file in the sources */
    job=p.parse1();
    if (p.parse1(p.EOF_ALLOWED)!=MiniPS::Qundef)
      Error::sev(Error::EERROR) << "job: the .job file should contain a single job" << (Error*)0;
    /* ^^^ Dat: the result of the second p.parse1() doesn't get delete0(...)d */
  } else if (helpp || argv1[0]==(char const*)NULLP) { help: /* help message */
    sout << "Usage:   " << Error::long_argv0 << " <filename.job>\n" <<
            "         " << Error::long_argv0 << " [options] <in.img> [OutputFormat:] <out.img>\n" <<
            "Example: " << Error::long_argv0 << " test.gif EPS: test.eps\n";
    if (helpp) Error::cexit(0);
    Error::sev(Error::EERROR) << "Incorrect command line" << (Error*)0;
  } else { /* one_liner */
    SimBuffer::B jobss;
    if (one_liner(jobss, argv1)) goto help;
    if (do_DisplayJobFile) displayJob(sout, jobss);
    Filter::FlatD jobr(jobss(), jobss.getLength());
    MiniPS::Parser p(&jobr);
    p.addSpecRun("%bts", &bts); /* bts: Built-in TemplateS, the bts.ttt file in the sources */
    if (MiniPS::Qerror==(job=p.parse1(p.EOF_ILLEGAL, Error::ERROR_CONT))
     || p.parse1(p.EOF_ALLOWED)!=MiniPS::Qundef
       ) {
      displayJob(sout, jobss);
      Error::sev(Error::EERROR) << "job: in-memory .job file corrupt (bug?" "?)" << (Error*)0;
      /* ^^^ Dat: the result of the second p.parse1() doesn't get delete0(...)d */
    }
  }
  Files::doSignalCleanup();

  /* --- Pre-parse .job file */
  
  /* Dat: memory storage for the MiniPS objects in the .job file are
   *      allocated by p.parse1() and freed by MiniPS::delete0(job). Other
   *      structures throughout this program hold pointers to inside `job',
   *      but storage is not copied.
   */
  
  MiniPS::String *InputFile, *OutputFile, *LoadHints;
  MiniPS::Array *Profile;
  MiniPS::VALUE TmpRemove;
  MiniPS::scanf_dict(job, /*show_warnings:*/true,
    "InputFile",  MiniPS::T_STRING, MiniPS::Qundef, &InputFile,
    "OutputFile", MiniPS::T_STRING, MiniPS::Qundef, &OutputFile,
    "LoadHints",  MiniPS::T_STRING, MiniPS::Qnull,  &LoadHints,
    "Templates",  MiniPS::T_DICT,   MiniPS::Qnull,  &Rule::Templates,
    "Profile",    MiniPS::T_ARRAY,  MiniPS::Qundef, &Profile,
    "TmpRemove",  MiniPS::T_BOOLEAN,MiniPS::Qtrue,  &TmpRemove,   /* remove temporary files upon exit? */
    NULLP
  );
  // MiniPS::dump(sout, job, 1);
  Files::tmpRemove=TmpRemove==MiniPS::Qtrue;
  if (MiniPS::Qnull==(MiniPS::VALUE)Rule::Templates) {
    Filter::FlatD flatd("<< (%bts) run >>");
    MiniPS::Parser p(&flatd);
    p.addSpecRun("%bts", &bts);
    Rule::Templates=(MiniPS::Dict*)p.parse1();
    MiniPS::RDICT(job)->put("/Templates", (MiniPS::VALUE)Rule::Templates); /* avoid memory leak (job gets freed recursively) */
  }

  /* --- Preprocess Appliers */
  
  Rule::OutputRule *rule_list=Rule::buildProfile((MiniPS::VALUE)Profile, buildProfile_quiet);
  
  /* --- Read/open other files */

  if ((MiniPS::VALUE)LoadHints==MiniPS::Qnull) { /* smart default */
    if (!rule_list[0].isEOL() && rule_list[0].cache.Compression==Rule::Cache::CO_JAI) {
      LoadHints=new MiniPS::String("jpeg-asis",4); /* used by in_jai.cpp and in_jpeg.cpp; even TIFF/JPEG */
    } else {
      LoadHints=new MiniPS::String("",0);
    }
    MiniPS::RDICT(job)->put("/LoadHints", (MiniPS::VALUE)LoadHints); /* avoid memory leak (job gets freed recursively) */
  }
      
  /* Imp: eliminate memory leak from default LoadHints */
  /* vvv may raise tons of error messages */
  // InputFile->term0(); /* always term0() for MiniPS::String */
  if (ufd==NULLP) ufd=new Filter::UngetFILED(InputFile->begin_(), stdin,
    Filter::UngetFILED::CM_closep|Filter::UngetFILED::CM_keep_stdinp);
  // fprintf(stderr, "ufd=%p\n", ufd);
  Image::SampledInfo info(Image::load(
    #if 0
      InputFile->begin_(), /* knows about stdin `-' */
      SimBuffer::B(",",1, LoadHints->begin_(),LoadHints->getLength(), ",",1 ).term0() /* three-way concatentation */
    #else
      (Image::Loader::UFD*)ufd,
      SimBuffer::B(",",1, LoadHints->begin_(),LoadHints->getLength(), ",",1 ).term0(), /* three-way concatentation */
      /*format:*/(char const*)NULLP
    #endif
  ));
  Error::sev(Error::NOTICE) << "job: read InputFile: " <<
    FNQ2STDOK(InputFile->begin_(),InputFile->getLength()) << (Error*)0;
  delete ufd;
  // assert(0);

  bool overwrite=false;
  if (0==strcmp(InputFile->begin_(), OutputFile->begin_()) && 0!=strcmp("-", InputFile->begin_())) {
    // Error::sev(Error::WARNING) << "job: InputFile == OutputFile" << (Error*)0;
    OutputFile->replace(OutputFile->begin_(), OutputFile->getLength(), ".s2new", 6);
    overwrite=true;
  }
  FILE *of=stdout;
  if (OutputFile->getLength()!=1 || OutputFile->begin_()[0]!='-') {
    if ((of=fopen(OutputFile->begin_(), "wb"))==(FILE*)NULLP) {
      Error::sev(Error::EERROR) << "job: cannot rewrite OutputFile: " << FNQ2STDOK(OutputFile->begin_(),OutputFile->getLength()) << (Error*)0;
    }
    /*if (!overwrite)*/
    Files::tmpRemoveCleanup(OutputFile->begin_(), &of);
  } else Files::set_binary_mode(1, true);
  
  /* --- Find and apply best applier */
  
  Files::FILEW wf(of);
  Rule::applyProfile(wf, rule_list, &info);
  
  /* --- Done, flush files and clean up. */

  fflush(of);
  if (ferror(of)) {
    /* FILE *backup=of; of=(FILE*)NULLP; fclose(backup); */
    Error::sev(Error::EERROR) << "job: error writing OutputFile: " << FNQ2STDOK(OutputFile->begin_(),OutputFile->getLength()) << (Error*)0;
  }
  if (of!=stdout) { /* Don't interfere with tmpRemoveCleanup() */
    FILE *backup=of; of=(FILE*)NULLP; fclose(backup);
  }
  if (overwrite) {
    if (0!=rename(OutputFile->begin_(), InputFile->begin_())) {
      // remove(OutputFile->begin_()); /* tmpRemoveCleanup() does it */
      Error::sev(Error::EERROR) << "job: error renaming, InputFile left intact" << (Error*)0;
    }
  }
  Error::sev(Error::NOTICE) << "job: written OutputFile: " << FNQ2STDOK(OutputFile->begin_(),OutputFile->getLength()) << (Error*)0;

  /* Freeing memory may cause segfaults because of possibly 
   * bad code design :-(. Luckily we have already saved the output file.
   */
  Rule::deleteProfile(rule_list);
  MiniPS::delete0(job); /* frees OutputFile etc. */

  if (Error::getTopPrinted()+0<=Error::NOTICE+0) fputs("Success.\n", stderr);
  fflush(stdout); fflush(stderr);
  Error::cexit(0);
}

/** main: process entry point for the sam2p utility. */
int main(int, char const*const* argv) {
  Files::FILEW sout(stdout);
  Files::FILEW serr(stderr);
  /* --- Initialize */

  bool helpp=argv[0]==(char const*)NULLP || argv[0]!=(char const*)NULLP && argv[1]!=(char const*)NULLP && argv[2]==(char const*)NULLP && (
             option_eq(argv[1], "-help") || 
             option_eq(argv[1], "-h") || 
             option_eq(argv[1], "-?") || 
             option_eq(argv[1], "/h") || 
             option_eq(argv[1], "/?"));
  bool versionp=argv[0]!=(char const*)NULLP && argv[1]!=(char const*)NULLP && argv[2]==(char const*)NULLP && (
             option_eq(argv[1], "-version") || 
             option_eq(argv[1], "-v"));
  bool quietp=argv[0]!=(char const*)NULLP && argv[1]!=(char const*)NULLP &&
             option_eq(argv[1], "-j:quiet");
  init_sam2p_engine(argv[0]);
  
  if (versionp) {
    sout << "This is " << Error::banner0 << ".\n";
    return 0;
  }

  /* Don't print diagnostics to stdout, becuse it might be the OutputFile */
  if (!quietp) { serr << "This is " << Error::banner0 << ".\n"; }
  init_loader();
  if (!quietp) { serr << "Available Loaders:"; Image::printLoaders(serr); serr << ".\n"; }
  init_applier();
  if (!quietp) { serr << "Available Appliers:"; Rule::printAppliers(serr); serr << ".\n"; }

  run_sam2p_engine(sout, serr, argv+(argv[0]!=(char const*)NULLP), helpp);
  return 0; /*notreached*/
}
