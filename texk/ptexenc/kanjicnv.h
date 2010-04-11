/*
 *  KANJI Code conversion routines.
 */

#ifndef PTEXENC_KANJICNV_H
#define PTEXENC_KANJICNV_H

#include <kpathsea/types.h>

#define HI(x)     ((((int)(x)) >> 8) & 0xff)
#define LO(x)     ( ((int)(x))       & 0xff)
#define HILO(x,y) ((LO(x) << 8) | LO(y))

extern KPSEDLL boolean isEUCkanji1(int c);
extern KPSEDLL boolean isEUCkanji2(int c);

extern KPSEDLL boolean isSJISkanji1(int c);
extern KPSEDLL boolean isSJISkanji2(int c);

/* EUC <=> JIS X 0208 code conversion */
extern KPSEDLL int EUCtoJIS(int);
extern KPSEDLL int JIStoEUC(int);

/* Shift JIS <=> JIS Kanji code conversion */
extern KPSEDLL int SJIStoJIS(int);
extern KPSEDLL int JIStoSJIS(int);

/* Shift JIS <=> EUC Kanji code conversion */
extern KPSEDLL int SJIStoEUC(int);
extern KPSEDLL int EUCtoSJIS(int);

/* KUTEN => JIS kanji code conversion */
extern KPSEDLL int KUTENtoJIS(int);

#endif /* PTEXENC_KANJICNV_H */
