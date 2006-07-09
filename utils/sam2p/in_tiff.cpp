/*
 * in_tiff.cpp -- read TIFF (Tag Image File Format) files with tif22pnm
 * by pts@fazekas.hu at Sun Apr 14 14:50:30 CEST 2002
 */
/* Imp: get docs about the TIFF format, and rewrite this from scratch */
/* Imp: use xviff.c */

#ifdef __GNUC__
#pragma implementation
#endif

#include "image.hpp"

#if USE_IN_TIFF

#include "error.hpp"
#include "gensio.hpp"
#include "helpere.hpp"
#include <string.h> /* memchr() */
#include <stdio.h> /* printf() */

static Image::Sampled *in_tiff_reader(Image::Loader::UFD *ufd, SimBuffer::Flat const&) {
  // Error::sev(Error::EERROR) << "Cannot load TIFF images yet." << (Error*)0;
  // HelperE helper("tifftopnm %S"); /* Cannot extract alpha channel */
  // HelperE helper("tif22pnm -rgba %S"); /* tif22pnm <= 0.07 */
  char const *cmd=
  #if OS_COTY==COTY_WIN9X || OS_COTY==COTY_WINNT
#if 0 /* unsafe if both tifftopnm and png22pnm exist */
    "tifftopnm <%S >%D\ntif22pnm -rgba %S pnm: %D"; /* slow but safe */
#else
    "tif22pnm -rgba %S pnm:";
#endif
  #else
    #if OS_COTY==COTY_UNIX
      #if 1
        "(tif22pnm -rgba %S pnm: || tifftopnm %S)";
      #else
        /* Dat: not using this to suppress `sh: tif22pnm: command not found', because
         * it would hide precious error messages printed by tif22pnm.
         */
         "((tif22pnm -rgba %S pnm: 2>/dev/null)|| tifftopnm %S)";
      #endif
    #else
      "tif22pnm -rgba %S pnm:"; /* Wants to seek in the file. */
    #endif
  #endif
  HelperE helper(cmd);
  Encoder::writeFrom(*(Filter::PipeE*)&helper, *(Filter::UngetFILED*)ufd);
  ((Filter::PipeE*)&helper)->vi_write(0,0); /* Signal EOF */
  return helper.getImg();
}

static Image::Loader::reader_t in_tiff_checker(char buf[Image::Loader::MAGIC_LEN], char [Image::Loader::MAGIC_LEN], SimBuffer::Flat const&, Image::Loader::UFD*) {
  /* MM\x00\x2a: TIFF image data, big-endian
   * II\x2a\x00: TIFF image data, little-endian
   * The second word of TIFF files is the TIFF version number, 42, which has 
   * never changed.  The TIFF specification recommends testing for it.
   */
  return (0==memcmp(buf,"MM\x00\x2a",4) || 0==memcmp(buf,"II\x2a\x00",4)) ? in_tiff_reader : 0;
}

#else
#define in_tiff_checker (Image::Loader::checker_t)NULLP
#endif /* USE_IN_XPM */

Image::Loader in_tiff_loader = { "TIFF", in_tiff_checker, 0 };
