/*
 * in_png.cpp -- read PNG (Portable Network Graphics, PNG is Not GIF) files with pngtopnm
 * by pts@fazekas.hu at Sun Apr 14 14:50:30 CEST 2002
 */

#ifdef __GNUC__
#pragma implementation
#endif

#include "image.hpp"

#if USE_IN_PNG

#include "error.hpp"
#include "gensio.hpp"
#include "helpere.hpp"
#include <string.h> /* memchr() */
#include <stdio.h> /* printf() */

static Image::Sampled *in_png_reader(Image::Loader::UFD* ufd, SimBuffer::Flat const&) {
  // Error::sev(Error::EERROR) << "Cannot load PNG images yet." << (Error*)0;
  char const* cmd=
#if 0
  #if OS_COTY==COTY_WIN9X || OS_COTY==COTY_WINNT
    "pngtopnm %S >%D\npngtopnm -alpha %S >>%D";
  #else
    #if OS_COTY==COTY_UNIX
      "(pngtopnm <%S && pngtopnm -alpha <%S) >%D";
    #else
      "pngtopnm %S >%D\npngtopnm -alpha %S >>%D";
    #endif
  #endif
#else /* Wed Feb  5 19:03:58 CET 2003 */
  #if OS_COTY==COTY_WIN9X || OS_COTY==COTY_WINNT
    "png22pnm -rgba %S >%D";
  #else
    #if OS_COTY==COTY_UNIX
      #if 1
        "(png22pnm -rgba %S || (pngtopnm <%S && pngtopnm -alpha <%S)) >%D";
      #else
        /* Dat: not using this to suppress `sh: png22pnm: command not found', because
         * it would hide precious error messages printed by png22pnm.
         */
         "((png22pnm -rgba %S 2>/dev/null)|| (pngtopnm <%S && pngtopnm -alpha <%S)) >%D";
      #endif
    #else
      "png22pnm -rgba %S >%D";
    #endif
  #endif
#endif
  HelperE helper(cmd); /* Run external process pngtopnm */
  Encoder::writeFrom(*(Filter::PipeE*)&helper, *(Filter::UngetFILED*)ufd);
  ((Filter::PipeE*)&helper)->vi_write(0,0); /* Signal EOF */
  return helper.getImg();
}

static Image::Loader::reader_t in_png_checker(char buf[Image::Loader::MAGIC_LEN], char [Image::Loader::MAGIC_LEN], SimBuffer::Flat const&, Image::Loader::UFD*) {
  return 0==memcmp(buf,"\211PNG\r\n\032\n",8) ? in_png_reader : 0;
}

#else
#define in_png_checker (Image::Loader::checker_t)NULLP
#endif /* USE_IN_PNG */

Image::Loader in_png_loader = { "PNG", in_png_checker, 0 };
