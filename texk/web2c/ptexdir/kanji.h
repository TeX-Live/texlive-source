/*
   kanji.h: Handling 2byte char, and so on.
*/
#ifndef KANJI_H
#define KANJI_H
#include "cpascal.h"
#include <ptexenc/ptexenc.h>

#define KANJI

/* functions */
#define Hi(x) (((x) >> 8) & 0xff)
#define Lo(x) ((x) & 0xff)

extern boolean check_kanji(integer c);
#define checkkanji check_kanji
extern boolean is_kanji(integer c);
#define iskanji is_kanji
extern boolean is_char_ascii(integer c);
#define ischarascii is_char_ascii
extern boolean is_wchar_ascii(integer c);
#define iswcharascii is_wchar_ascii
extern boolean ismultiprn(integer c);
extern integer calc_pos(integer c);
#define calcpos calc_pos
extern integer kcatcodekey(integer c);

#ifndef PRESERVE_PUTC
#undef putc
#define putc(c,fp) putc2(c,fp)
#endif /* !PRESERVE_PUTC */

#define inputline2(fp,buff,pos,size) input_line2(fp,buff,pos,size,NULL)

#ifdef MP
#undef TEXMFPOOLNAME
#undef TEXMFENGINENAME
#define TEXMFPOOLNAME   "jmp.pool"
#define TEXMFENGINENAME "jmpost"
#endif /* MP */

#endif /* not KANJI_H */
