#ifndef _TFM_H
#define _TFM_H

/* ******** Declarations used in dvi2xx.[ch] and tfm.c ******** */

/* ******** Types ******** */

/* ******** Information returned by tfm_read_info ******** */
typedef struct {
  /* These string lengths are imposed by the TFM format. Either of these
     values may be the empty string.  */
  unsigned char coding_scheme[40];
  unsigned char family[20];

  /* The second fontdimen. */
  unsigned interword;

  /* These values are what will work to select the font in PCL. If this
     TFM file doesn't have the `KN' extensions (distinguishable by the
     family == "HPAUTOTFM"). */
#define SPACING_FIXED 0
#define SPACING_PROPORTIONAL 1
  unsigned spacing;
  int weight;
  unsigned style;
  unsigned typeface_id;

  /* TFM files can always have 256 characters, even if we're using the
     old pixel format that only supports 128. The values are fix-words
     scaled by the design size; i.e., straight from the TFM file. */
  long4 widths[256];
} tfm_info_type;

/* ******** Defined in dvi2xx.h ******** */

#ifndef KPATHSEA
extern char* TFMpath;
#endif
extern bool G_quiet;

/* ******** Defined in dvi2xx.c ******** */

extern void Fatal(const char *, ...);
extern long4 NoSignExtend(FILEPTR, int);
extern void Warning(const char *, ...);

/* ******** Defined in tfm.c ******** */

extern bool tfm_read_info(char *, tfm_info_type *);

#endif /* _TFM_H */
