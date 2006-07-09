/*
 * in_ps.cpp -- read PS and PDF files using GS
 * by pts@fazekas.hu at Tue Sep 30 12:33:11 CEST 2003
 */

#ifdef __GNUC__
#pragma implementation
#endif

#include "image.hpp"

#if USE_IN_PS || USE_IN_PDF

#include "error.hpp"
#include "gensio.hpp"
#include "helpere.hpp"
#include <string.h> /* memchr() */
#include <stdio.h> /* printf() */

#if OS_COTY==COTY_WIN9X || OS_COTY==COTY_WINNT
#  define GS "gswin32c"
#else
#  define GS "gs"
#endif

#endif /* USE_IN_PS || USE_IN_PDF */

/** Adds a Ghostscript invocation command. Works for both PS and PDF. */
static void add_gs_cmd(SimBuffer::B &cmd, SimBuffer::Flat const& hints) {
  cmd << GS " -r72 -q -dTextAlphaBits=4 -dGraphicsAlphaBits=4 -dLastPage=1 -sDEVICE=pnmraw -dDELAYSAFER -dBATCH -dNOPAUSE -sOutputFile=%D ";
  char const *p=hints(), *r;
  /* Dat: hints ends by ',' '\0' */
  // Files::FILEW(stdout) << hints << ";;\n";
  while (*p!=',') p++; /* Dat: safe, because hints is assumed to start and end by ',' */
  while (1) {
    assert(*p==',');
    if (*++p=='\0') break;
    if (p[0]=='g' && p[1]=='s' && p[2]=='=') {
      r=p+=3;
      while (*p!=',') p++;
      cmd.vi_write(r, p-r); /* Dat: -r... in here overrides -r72 above */
      cmd << ' ';
    } else {
      while (*p!=',') p++;
    }
  }
}

#if USE_IN_PS

/* !! -r144 and scale back..., also for PDF -- to enhance image quality */

#undef  DO_KEEP_TEMP
#define DO_KEEP_TEMP 0

static Image::Sampled *in_ps_reader_low(Image::Loader::UFD* ufd, char const*bboxline, SimBuffer::Flat const& hints) {
  SimBuffer::B mainfn;
  if (!Files::find_tmpnam(mainfn)) Error::sev(Error::EERROR) << "in_ps_reader" << ": tmpnam() failed" << (Error*)0;
  mainfn.term0();
  #if DO_KEEP_TEMP
    printf("m: %s\n", mainfn());
  #else
    Files::tmpRemoveCleanup(mainfn());
  #endif
  FILE *f=fopen(mainfn(),"w");
  fprintf(f, "%s/setpagedevice/pop load def\n", bboxline); /* Imp: /a4/letter etc. */
  fprintf(f, "_IFN (r) file cvx exec\n"); /* Dat: doesn't rely on GS to
    recognise EPSF-x.y comment, so works with both old and new gs */
  // ^^^ !! DOS EPSF etc. instead of exec/run
  fclose(f);
  // Error::sev(Error::EERROR) << "Cannot load PS images yet." << (Error*)0;
  /* Dat: -dLastPage=1 has no effect, but we keep it for PDF compatibility */
  /* !! keep only 1st page, without setpagedevice for PS files */
  /* Dat: -dSAFER won't let me open the file with `/' under ESP Ghostscript 7.05.6 (2003-02-05) */
  /* Imp: win9X command line too long? */
  SimBuffer::B cmd;
  add_gs_cmd(cmd,hints);
  cmd << " -s_IFN=%S -- %*";
  HelperE helper(cmd.term0()(), mainfn()); /* Run external process GS */
  Filter::UngetFILED* ufdd=(Filter::UngetFILED*)ufd;
  int i=ufdd->vi_getcc();
  if (i<0) Error::sev(Error::EERROR) << "in_ps_reader: Empty PostScript file." << (Error*)0; /* should never happen */
  ((Filter::PipeE*)&helper)->vi_putcc(i);
  Encoder::writeFrom(*(Filter::PipeE*)&helper, *ufdd);
  ((Filter::PipeE*)&helper)->vi_write(0,0); /* Signal EOF */
  #if !DO_KEEP_TEMP
    remove(mainfn());
  #endif
  return helper.getImg();
}

static Image::Sampled *in_ps_reader(Image::Loader::UFD* ufd, SimBuffer::Flat const& hints) {
  return in_ps_reader_low(ufd, "", hints);
}

static Image::Sampled *in_eps_reader(Image::Loader::UFD* ufd, SimBuffer::Flat const& hints) {
  double llx=0.0, lly=0.0, urx=0.0, ury=0.0;
  Filter::UngetFILED* ufdd=(Filter::UngetFILED*)ufd;
  /* ^^^ SUXX: no warning for ufdd=ufdd */
  /* SUXX: valgrind, checkergcc: no indication of segfault due to stack overflow inside fgetc() */
  SimBuffer::B line; /* Imp: limit for max line length etc. */
  #if 0
  while ((line.clearFree(), ufdd->appendLine(line), line)) {
    line.term0();
    printf("line: %s", line());
  }
  #endif
  slen_t line0ofs;
  int had=0;
  while ((line0ofs=line.getLength(), ufdd->appendLine(line), line0ofs!=line.getLength())) {
    char const *thisline=line()+line0ofs;
    line.term0();
    // printf("line: %s", thisline);
    if (thisline[0]=='\n' || thisline[0]=='\r') continue; /* empty line */
    if (thisline[0]=='%' && thisline[1]=='!') continue; /* %!PS-... */
    if (thisline[0]!='%' || thisline[1]!='%') break; /* non-ADSC comment */
         if (had<3 && 4==sscanf(thisline+2, "ExactBoundingBox:%lg%lg%lg%lg", &llx, &lly, &urx, &ury)) had=3;
    else if (had<2 && 4==sscanf(thisline+2, "HiResBoundingBox:%lg%lg%lg%lg", &llx, &lly, &urx, &ury)) had=2;
    else if (had<1 && 4==sscanf(thisline+2, "BoundingBox:%lg%lg%lg%lg", &llx, &lly, &urx, &ury)) had=1;
    /* Dat: finds MetaPost hiresbbox after %%EndComments */
    // printf("line: %s", line()+line0ofs);
  }
  ufdd->unread(line(), line.getLength()); line.clearFree();
  char bboxline[300];
  if (had!=0) {
    // fprintf(stderr, "bbox=[%"PTS_CFG_PRINTFGLEN"g %"PTS_CFG_PRINTFGLEN"g %"PTS_CFG_PRINTFGLEN"g %"PTS_CFG_PRINTFGLEN"g]\n", llx, lly, urx, ury);
    /* Dat: we must call translate _after_ setpagedevice (so it will take effect), at least with ESP Ghostscript 7.05.6 (2003-02-05); BUGFIX at Fri Aug 12 22:49:07 CEST 2005 */
    sprintf(bboxline,
      "<</PageSize[%"PTS_CFG_PRINTFGLEN"g %"PTS_CFG_PRINTFGLEN"g]>>setpagedevice\n"
      "%"PTS_CFG_PRINTFGLEN"g %"PTS_CFG_PRINTFGLEN"g translate\n"
      , urx-llx, ury-lly, -llx, -lly);
  } else {
    Error::sev(Error::WARNING) << "in_eps_reader: missing EPS bbox" << (Error*)0;
    bboxline[0]='\0';
  }
  
  return in_ps_reader_low(ufd, bboxline, hints);
}

static Image::Loader::reader_t in_ps_checker(char buf[Image::Loader::MAGIC_LEN], char [Image::Loader::MAGIC_LEN], SimBuffer::Flat const&, Image::Loader::UFD*) {
  if (0!=memcmp(buf,"%!PS-Adobe-",11)) return 0;
  char const *p=buf+11, *pend=buf+Image::Loader::MAGIC_LEN;
  while (p!=pend && *p!=' ' && *p!='\t') p++;
  while (p!=pend && (*p==' ' || *p=='\t')) p++;
  /* Imp: option to accept BoundingBox for non-EPS PS */
  return (0==strncmp(p,"EPSF-",5)) ? in_eps_reader : in_ps_reader;
  /* ^^^ BUGFIX at Fri Nov 26 12:13:58 CET 2004 */
  /* ^^^ BUGFIX again at Thu Jan  6 10:25:54 CET 2005 */
}

#else
#define in_ps_checker (Image::Loader::checker_t)NULLP
#endif /* USE_IN_PS */

Image::Loader in_ps_loader = { "PS", in_ps_checker, 0 };

#if USE_IN_PDF

static Image::Sampled *in_pdf_reader(Image::Loader::UFD* ufd, SimBuffer::Flat const& hints) {
  // Error::sev(Error::EERROR) << "Cannot load PDF images yet." << (Error*)0;
  SimBuffer::B cmd;
  add_gs_cmd(cmd,hints);
  cmd << " -- %S";
  /* Dat: -dSAFER won't let me open the file with `/' under ESP Ghostscript 7.05.6 (2003-02-05) */
  /* Imp: win9X command line too long? */
  fprintf(stderr, "gs_cmd=(%s)\n", cmd.term0()());
  HelperE helper(cmd.term0()()); /* Run external process GS */
  Encoder::writeFrom(*(Filter::PipeE*)&helper, *(Filter::UngetFILED*)ufd);
  ((Filter::PipeE*)&helper)->vi_write(0,0); /* Signal EOF */
  return helper.getImg();
}

static Image::Loader::reader_t in_pdf_checker(char buf[Image::Loader::MAGIC_LEN], char [Image::Loader::MAGIC_LEN], SimBuffer::Flat const&, Image::Loader::UFD*) {
  return 0==memcmp(buf,"%PDF-",5) ? in_pdf_reader : 0;
}

#else
#define in_pdf_checker (Image::Loader::checker_t)NULLP
#endif /* USE_IN_PDF */

Image::Loader in_pdf_loader = { "PDF", in_pdf_checker, 0 };
