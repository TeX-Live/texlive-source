/*
 * in_jpeg.cpp -- read JPEG (JFIF and other) files with djpeg
 * by pts@fazekas.hu at Sun Apr 14 14:50:30 CEST 2002
 */

#ifdef __GNUC__
#pragma implementation
#endif

#include "image.hpp"

#if USE_IN_JPEG

#include "error.hpp"
#include "gensio.hpp"
#include "helpere.hpp"
#include <string.h> /* memchr() */
#include <stdio.h> /* printf() */

static Image::Sampled *in_jpeg_reader(Image::Loader::UFD *ufd, SimBuffer::Flat const&) {
  // Error::sev(Error::EERROR) << "Cannot load JPEG images yet." << (Error*)0;
  HelperE helper("djpeg"); /* Run external process `djpeg' to convert JPEG -> PNM */
  Encoder::writeFrom(*(Filter::PipeE*)&helper, *(Filter::UngetFILED*)ufd);
  ((Filter::PipeE*)&helper)->vi_write(0,0); /* Signal EOF */
  return helper.getImg();
}

static Image::Loader::reader_t in_jpeg_checker(char buf[Image::Loader::MAGIC_LEN], char [Image::Loader::MAGIC_LEN], SimBuffer::Flat const& loadHints, Image::Loader::UFD*) {
  return (0==memcmp(buf, "\xff\xd8", 2)) && loadHints.findFirst((char const*)",asis,",6)==loadHints.getLength()
         ? in_jpeg_reader : 0;
}

#else
#define in_jpeg_checker (Image::Loader::checker_t)NULLP
#endif /* USE_IN_XPM */

Image::Loader in_jpeg_loader = { "JPEG", in_jpeg_checker, 0 };
