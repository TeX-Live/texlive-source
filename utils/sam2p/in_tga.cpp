/*
 * in_tga.cpp -- read a Truevision Targa TGA file
 * by pts@fazekas.hu at Sat Mar  2 00:49:35 CET 2002
 *
 */

#ifdef __GNUC__
#pragma implementation
#endif

#include "image.hpp"
#include "error.hpp"
#include "gensio.hpp" /* Filter::UngetFILED */

#if USE_IN_TGA

#include "input-tga.ci" /* defines tga_load_image */

static Image::Sampled *in_tga_reader(Image::Loader::UFD *ufd, SimBuffer::Flat const&) {
  Image::Sampled *ret=0;
  bitmap_type bitmap=tga_load_image(((Filter::UngetFILED*)ufd)->getFILE(/*seekable:*/false));
  /* Imp: Work without duplicated memory allocation */
  if (BITMAP_PLANES(bitmap)==1) {
    Image::Gray *img=new Image::Gray(BITMAP_WIDTH(bitmap), BITMAP_HEIGHT(bitmap), 8);
    memcpy(img->getRowbeg(), BITMAP_BITS(bitmap), (slen_t)BITMAP_WIDTH(bitmap)*BITMAP_HEIGHT(bitmap));
    ret=img;
  } else if (BITMAP_PLANES(bitmap)==3) {
    Image::RGB *img=new Image::RGB(BITMAP_WIDTH(bitmap), BITMAP_HEIGHT(bitmap), 8);
    memcpy(img->getRowbeg(), BITMAP_BITS(bitmap), (slen_t)3*BITMAP_WIDTH(bitmap)*BITMAP_HEIGHT(bitmap));
    ret=img;
  } else assert(0 && "invalid TGA depth");
  delete [] BITMAP_BITS(bitmap);
  return ret;
}

static Image::Loader::reader_t in_tga_checker(char buf[Image::Loader::MAGIC_LEN], char [Image::Loader::MAGIC_LEN], SimBuffer::Flat const&, Image::Loader::UFD*) {
  /* vvv Unfortunately not all targa images have that footer */
  /* return 0==memcmp(bufend+Image::Loader::MAGIC_LEN-18, "TRUEVISION-XFILE", 16) */
  /* vvv 30..127: Aladdin Ghostscript adds 58 bytes of header */
  return (unsigned char)buf[0]>=30 && (unsigned char)buf[0]<=63 && 
    (unsigned char)buf[1]<=11 &&
    ((unsigned char)buf[16]<=8 || (unsigned char)buf[16]==24)
   ? in_tga_reader : 0;
}

#else
#define in_tga_checker NULLP
#endif /* USE_IN_XPM */

Image::Loader in_tga_loader = { "TGA", in_tga_checker, 0 };
