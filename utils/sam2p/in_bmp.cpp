/*
 * in_bmp.cpp -- read a Windows(?) BMP bitmap file
 * by pts@fazekas.hu at Sat Mar  2 00:46:54 CET 2002
 *
 */

#ifdef __GNUC__
#pragma implementation
#endif

#include "image.hpp"
#include "error.hpp"

#if USE_IN_BMP

#include "gensio.hpp"
#include "input-bmp.ci"

static Image::Sampled *in_bmp_reader(Image::Loader::UFD *ufd, SimBuffer::Flat const&) {
  Image::Sampled *ret=0;
  bitmap_type bitmap=bmp_load_image(((Filter::UngetFILED*)ufd)->getFILE(/*seekable:*/false));
  /* Imp: Work without duplicated memory allocation */
  if (BITMAP_PLANES(bitmap)==1) {
    Image::Gray *img=new Image::Gray(BITMAP_WIDTH(bitmap), BITMAP_HEIGHT(bitmap), 8);
    memcpy(img->getRowbeg(), BITMAP_BITS(bitmap), (slen_t)BITMAP_WIDTH(bitmap)*BITMAP_HEIGHT(bitmap));
    ret=img;
  } else if (BITMAP_PLANES(bitmap)==3) {
    Image::RGB *img=new Image::RGB(BITMAP_WIDTH(bitmap), BITMAP_HEIGHT(bitmap), 8);
    memcpy(img->getRowbeg(), BITMAP_BITS(bitmap), (slen_t)3*BITMAP_WIDTH(bitmap)*BITMAP_HEIGHT(bitmap));
    ret=img;
  } else assert(0 && "invalid BMP depth");
  delete [] BITMAP_BITS(bitmap);
  return ret;
}

static Image::Loader::reader_t in_bmp_checker(char buf[Image::Loader::MAGIC_LEN], char [Image::Loader::MAGIC_LEN], SimBuffer::Flat const&, Image::Loader::UFD*) {
  return (buf[0]=='B' && buf[1]=='M'
   && buf[6]==0 && buf[7]==0 && buf[8]==0 && buf[9]==0
   && (unsigned char)(buf[14])<=64 && buf[15]==0 && buf[16]==0 && buf[17]==0)
   ? in_bmp_reader : 0;
}

#else
#define in_bmp_checker NULLP
#endif /* USE_IN_BMP */

Image::Loader in_bmp_loader = { "BMP", in_bmp_checker, 0 };
