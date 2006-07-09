/* in_jai.hpp -- Tue Jun 11 19:05:04 CEST 2002
 */

#ifdef __GNUC__
#pragma interface
#endif

#ifndef IN_JAI_HPP
#define IN_JAI_HPP 1

#include "config2.h"

#if USE_IN_JAI

#include "gensio.hpp"

struct jai_gfxinfo {
  /** 0 for baseline JPEG, 1..15 for other JPEG compression */
  unsigned char SOF_type;
  unsigned char colorspace;
  unsigned char bad, bpc, cpp, had_jfif, colortransform, id_rgb;
  /** (Horiz<<4+Vert) sampling factor for the first color component. Usually
   * 0x11, but TIFF defaults to 0x22.
   */
  unsigned char hvs;
  /*Image::Sampled::dimen_t*/slen_t height, width;
  /** Offset of byte just _after_ the first SOF marker (FF C0 or alike)
   * from the beginning of the file.
   */
  slen_t SOF_offs;
};

extern char *jai_errors[];
#if 0
  extern void jai_parse_jpeg(struct gfxinfo *result, Filter::FlatR *f);
  extern void jai_parse_jpeg(struct gfxinfo *result, FILE *f);
#else
  extern void jai_parse_jpeg(struct jai_gfxinfo *result, DecoderTeller *fp, bool must_be_baseline=true);
  /** Doesn't treat `-' as a special filename. @return 0, 1, or -1 on I/O error */
  extern int jai_is_baseline_jpeg(char const* filename);
  extern int jai_is_baseline_jpeg(/*DecoderTeller*/ Filter::UngetFILED *fp);
#endif

#endif

#endif /* in_jai.hpp */
