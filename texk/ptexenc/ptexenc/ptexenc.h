/*
 *  KANJI Code conversion routines.
 */

#ifndef PTEXENC_PTEXENC_H
#define PTEXENC_PTEXENC_H

#include <kpathsea/types.h>

extern KPSEDLL const char *ptexenc_version_string;

#define KANJI_OPTS "{jis|euc|sjis|utf8}"

/* get/set Kanji encoding by string */
extern KPSEDLL const_string get_enc_string(void);
extern KPSEDLL int    set_enc_string(const_string file, const_string inter);
#define getencstring  get_enc_string
#define setencstring  set_enc_string

/* decide if internal Kanji encode is SJIS or not */
extern KPSEDLL boolean  is_internalSJIS(void);
#define isinternalSJIS  is_internalSJIS

/* check char range */
extern KPSEDLL boolean iskanji1(int c);
extern KPSEDLL boolean iskanji2(int c);

/* internal (EUC/SJIS) from/to buffer (EUC/SJIS/UTF-8) code conversion */
extern KPSEDLL int multistrlen(string s, int len, int pos);
extern KPSEDLL long fromBUFF(string s, int len, int pos);
extern KPSEDLL long toBUFF(long inter);

/* internal (EUC/SJIS) from/to DVI (JIS) code conversion */
#define fromDVI fromJIS
#define toDVI   toJIS

/* JIS/EUC/SJIS/KUTN/UCS to internal (EUC/SJIS) code conversion */
/* (only for \euc primitive, etc.) */
extern KPSEDLL long toJIS(long kcode);
extern KPSEDLL long fromJIS(long jis);
extern KPSEDLL long fromEUC(long euc);
extern KPSEDLL long fromSJIS(long sjis);
extern KPSEDLL long fromKUTEN(long kuten);

/* fputs/putc with encoding conversion */
extern KPSEDLL int putc2(int c, FILE *fp);
extern KPSEDLL int fputs2(const char *s, FILE *fp);

/* input line with encoding conversion */
extern KPSEDLL long input_line2(FILE *fp, string buff, long pos,
				const long buffsize, int *lastchar);

/* open/close through nkf */
extern KPSEDLL void nkf_disable(void);
extern KPSEDLL FILE *nkf_open(const char *path, const char *mode);
extern KPSEDLL int nkf_close(FILE *fp);

#endif /* PTEXENC_PTEXENC_H */
