/*
   kanji.h: Handling 2byte char, and so on.
*/
#ifndef KANJI_H
#define KANJI_H
#include "cpascal.h"
#include <ptexenc/ptexenc.h>
#include <ptexenc/unicode.h>
#ifdef eupTeX
#define getintone(w) ((w).cint1)
#define setintone(w,a) ((w).cint1=(a))
#endif

#define KANJI

/* functions */
#define XXHi(x) BYTE1(x)
#define XHi(x) BYTE2(x)
#define Hi(x) BYTE3(x)
#define Lo(x) BYTE4(x)

extern integer check_kanji (integer c, integer m);
#define checkkanji check_kanji
extern boolean is_kanji (integer c);
#define iskanji is_kanji
extern boolean is_char_ascii (integer c);
#define ischarascii is_char_ascii
extern boolean is_wchar_ascii (integer c);
#define iswcharascii is_wchar_ascii
extern boolean ismultiprn (integer c);
extern integer calc_pos (integer c);
#define calcpos calc_pos
extern integer kcatcodekey (integer c);
extern integer multilenbuffchar (integer c);
#define setptexencuptex() set_enc_string("utf8", "uptex");

/* number of rest of multi-char for kcode_pos */
#define nrestmultichr(x)  ( (x)!=0 ? ((x) / 8) + 2 - ((x) % 8) : -1 )

#ifndef PRESERVE_PUTC
#undef putc
#define putc(c,fp) putc2(c,fp)
#endif /* !PRESERVE_PUTC */

#if defined(UPBIBTEX) || defined(BIBTEX)
#define inputline2(fp,buff,pos,size,ptr) input_line2(fp,buff,pos,size,ptr)
#else
#define inputline2(fp,buff,pos,size) input_line2(fp,buff,pos,size,NULL)
#endif

/* FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME */
/* Temporary fix for functions still missing in the ptexenc library.  */
extern boolean is_internalUPTEX(void);
#define isinternalUPTEX is_internalUPTEX
extern boolean ismultichr (int length, int nth, int c);
/* FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME */

#endif /* not KANJI_H */
