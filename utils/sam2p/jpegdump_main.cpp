\/*
 * jpegdump_main.cpp -- dump a JPEG stream
 * by pts@math.bme.hu at Tue Jun  4 13:19:00 CEST 2002
 */


/* Structure of a typical JPEG file:
(D8:SOI) (E0:APP0) (FE:COM)? (DB:DQT)*2 (C0:SOF0) (C4:DHT)*4 (DA:SOS)
... (D9:EOI)
*/

#ifdef __GNUC__
#pragma implementation
#endif

#include "config2.h"
#include <stdio.h>

#if OBJDEP
#warning PROVIDES: jpegdump_main
#endif

typedef slen_t dimen_t;
static const unsigned char
    CS_UNKNOWN=0,           /* error/unspecified */
    CS_GRAYSCALE=1,         /* monochrome */
    CS_RGB=2,               /* red/green/blue */
    CS_YCbCr=3,             /* Y/Cb/Cr (also known as YUV) */
    CS_CMYK=4,              /* C/M/Y/K */
    CS_YCCK=5,              /* Y/Cb/Cr/K */
    CS_Indexed_RGB=12;


/* --- The following code is based on standard/image.c from PHP4 */

/* some defines for the different JPEG block types */
#define M_SOF0  0xC0			/* Start Of Frame N */
#define M_SOF1  0xC1			/* N indicates which compression process */
#define M_SOF2  0xC2			/* Only SOF0-SOF2 are now in common use */
#define M_SOF3  0xC3
#define M_SOF5  0xC5			/* NB: codes C4 and CC are NOT SOF markers */
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8  
#define M_EOI   0xD9			/* End Of Image (end of datastream) */
#define M_SOS   0xDA			/* Start Of Scan (begins compressed data) */
#define M_COM   0xFE                    /* comment */
#define M_DQT   0xDB			/* QuantTables */
#define M_DHT   0xC4			/* HuffTables */
#define M_APP0  0xe0
#define M_APP1  0xe1
#define M_APP2  0xe2
#define M_APP3  0xe3
#define M_APP4  0xe4
#define M_APP5  0xe5
#define M_APP6  0xe6
#define M_APP7  0xe7
#define M_APP8  0xe8
#define M_APP9  0xe9
#define M_APP10 0xea
#define M_APP11 0xeb
#define M_APP12 0xec
#define M_APP13 0xed
#define M_APP14 0xee
#define M_APP15 0xef


static unsigned short jai_read2(FILE *fp) {
  unsigned char a[ 2 ];

  /* just return 0 if we hit the end-of-file */
  if (fread(a,sizeof(a),1,fp) != 1) return 0;

  return (((unsigned short) a[ 0 ]) << 8) + ((unsigned short) a[ 1 ]);
}

static unsigned int jai_next_marker(FILE *fp)
   /* get next marker byte from file */
{
  int c;

  /* skip unimportant stuff */

  c = MACRO_GETC(fp);

  while (c != 0xff) { 
    if ((c = MACRO_GETC(fp)) == EOF)
      return M_EOI; /* we hit EOF */
  }

  /* get marker byte, swallowing possible padding */
  do {
    if ((c = MACRO_GETC(fp)) == EOF)
      return M_EOI;    /* we hit EOF */
  } while (c == 0xff);

  // printf("marker=%02X\n", c);
  return (unsigned int) c;
}

static void jai_skip_variable(FILE *fp)
   /* skip over a variable-length block; assumes proper length marker */
{
  unsigned short length;
  
  printf("%lu: skip variable\n", ftell(fp));

  length = jai_read2(fp);
  length -= 2;        /* length includes itself */
  if (length>0) {
    printf("... variable length=%u\n", length);
    #if 0
      fseek(fp, (long) length, SEEK_CUR);  /* skip the header */
    #endif
    while (length--!=0) MACRO_GETC(fp); /* make feof(fp) correct */
    printf("%lu: skipped variable; feof=%u\n", ftell(fp), feof(fp));
  }
}

struct gfxinfo {
  unsigned char bad, bpc, cpp, had_jfif, colortransform, id_rgb;
  dimen_t height, width;
  /** Offset of byte just _after_ the first SOF marker (FF C0 or alike)
   * from the beginning of the file.
   */
  slen_t SOF_offs;
};

/** main loop to parse JPEG structure */
static void jai_handle_jpeg(struct gfxinfo *result, FILE *fp) {
  unsigned int length, marker;
  unsigned char had_adobe, id, hvs, qtn;
  
  result->bad=1; /* signal invalid return value */
  result->id_rgb=0;
  result->had_jfif=0;
  result->colortransform=127; /* no Adobe marker yet */
  // fseek(fp, 0L, SEEK_SET);    /* position file pointer on SOF */

  if (MACRO_GETC(fp) != 0xFF || MACRO_GETC(fp)!=M_SOI) return; /* JPEG header... */
  printf("2: marker D8 (SOI)\n");

  for (;;) {
    assert(!feof(fp));
    printf("... %lu\n", ftell(fp));
    marker=jai_next_marker(fp);
    printf("%lu: marker %02X\n", ftell(fp), marker);
    switch (marker) {
    case M_SOF0:
      if (result->bad!=1) { result->bad=4; return; } /* only one M_SOF allowed */
      result->SOF_offs=ftell(fp);
      /* handle SOFn block */
      length=jai_read2(fp);
      result->bpc = MACRO_GETC(fp);
      result->height = jai_read2(fp);
      result->width = jai_read2(fp);
      result->cpp = MACRO_GETC(fp);
      if ((length-=8)!=3U*result->cpp) return;
      if (result->cpp==3) {
        result->id_rgb =(id=MACRO_GETC(fp)=='R');  hvs=MACRO_GETC(fp); qtn=MACRO_GETC(fp);
        printf("Comp#1 id=0x%02X h_samp_fact=%u v_samp_fact=%u quant_tbl_no=%u\n",
          id, hvs>>4, hvs&15, qtn);
        result->id_rgb&=(id=MACRO_GETC(fp)=='G');  hvs=MACRO_GETC(fp); qtn=MACRO_GETC(fp);
        printf("Comp#2 id=0x%02X h_samp_fact=%u v_samp_fact=%u quant_tbl_no=%u\n",
          id, hvs>>4, hvs&15, qtn);
        result->id_rgb&=(id=MACRO_GETC(fp)=='B');  hvs=MACRO_GETC(fp); qtn=MACRO_GETC(fp);
        printf("Comp#3 id=0x%02X h_samp_fact=%u v_samp_fact=%u quant_tbl_no=%u\n",
          id, hvs>>4, hvs&15, qtn);
      } else while (length--!=0) MACRO_GETC(fp);
      if (result->cpp!=1 && result->cpp!=3 && result->cpp!=4) {
        result->bad=5; return;
      }
      if (feof(fp)) { result->bad=8; return; }
      result->bad=2;
      break;
    case M_SOF1:
    case M_SOF2:
    case M_SOF3:
    case M_SOF5:
    case M_SOF6:
    case M_SOF7:
    case M_SOF9:
    case M_SOF10:
    case M_SOF11:
    case M_SOF13:
    case M_SOF14:
    case M_SOF15:
      // fprintf(stderr, "SOF%u\n", marker-M_SOF0); assert(0);
      result->bad=3;
      return;
    case M_SOS: /* we are about to hit image data. We're done. */
      jai_skip_variable(fp);    /* anything else isn't interesting */
      if (feof(fp)) { result->bad=8; return; }
      if (result->bad==2 && !feof(fp)) result->bad=0;
      /* Dat: we should really return() here, because the SOS marker is
       * followed by binary data, not surrounded by markers.
       */
      break;
    case M_EOI: /* end of image or EOF */
      if (feof(fp)) result->bad=6;
      else if (MACRO_GETC(fp)!=EOF) result->bad=7;
      return;
    case M_APP0: /* JFIF application-specific marker */
      length=jai_read2(fp);
      if (length==2+4+1+2+1+2+2+1+1) {
        result->had_jfif=MACRO_GETC(fp)=='J' && MACRO_GETC(fp)=='F' && MACRO_GETC(fp)=='I' &&
                         MACRO_GETC(fp)=='F' && MACRO_GETC(fp)==0;
        length-=7;
      } else length-=2;
      while (length--!=0) MACRO_GETC(fp);
      if (feof(fp)) { result->bad=8; return; }
      break;
    case M_APP14: /* Adobe application-specific marker */
      length=jai_read2(fp);
      if ((length-=2)==5+2+2+2+1) {
        had_adobe=MACRO_GETC(fp)=='A' && MACRO_GETC(fp)=='d' && MACRO_GETC(fp)=='o' &&
                  MACRO_GETC(fp)=='b' && MACRO_GETC(fp)=='e' && ((unsigned char)MACRO_GETC(fp))>=1;
        MACRO_GETC(fp); MACRO_GETC(fp); MACRO_GETC(fp); MACRO_GETC(fp); MACRO_GETC(fp);
        if (had_adobe) result->colortransform=MACRO_GETC(fp);
                  else MACRO_GETC(fp);
      } else while (length--!=0) MACRO_GETC(fp);
      if (feof(fp)) { result->bad=8; return; }
      break;
    case M_APP1:
    case M_APP2:
    case M_APP3:
    case M_APP4:
    case M_APP5:
    case M_APP6:
    case M_APP7:
    case M_APP8:
    case M_APP9:
    case M_APP10:
    case M_APP11:
    case M_APP12:
    case M_APP13:
    case M_APP15:
      /* fall through */
    default:
      jai_skip_variable(fp);    /* anything else isn't interesting */
      if (feof(fp)) { result->bad=8; return; }
      break;
  } }
}

static char *jai_errors[]={
  (char*)NULLP,
  /*1*/ "missing SOF0 marker",
  /*2*/ "EOF|EOI before SOS", /* "premature EOF", */
  /*3*/ "not a Baseline JPEG (SOF must be SOF0)",
  /*4*/ "more SOF0 markers",
  /*5*/ "bad # components",
  /*6*/ "EOF before EOI",
  /*7*/ "extra bytes after EOI",
  /*8*/ "EOF before end of parametric marker",
};

int main(int argc, char **argv) {
  struct gfxinfo gi;
  FILE *f=argc>=2 ? fopen(argv[1], "rb"): stdin;
  jai_handle_jpeg(&gi, f);
  if (gi.bad==1) {
    printf("JAI: Warning: %s.\n", jai_errors[gi.bad]);
    gi.cpp=1;
    gi.width=0;
    gi.height=0;
    gi.SOF_offs=0;
    gi.bpc=0;
  } else if (gi.bad!=0) { printf("JAI: %s.\n", jai_errors[gi.bad]); if (gi.bad!=1) return 1; }
  unsigned char colorspace;
  if (gi.cpp==1) {
    colorspace=CS_GRAYSCALE;
  } else if (gi.cpp==3) {
    colorspace=CS_YCbCr;
    if (gi.had_jfif!=0) ;
    else if (gi.colortransform==0) colorspace=CS_RGB;
    else if (gi.colortransform==1) ;
    else if (gi.colortransform!=127) { printf("JAI: unknown ColorTransform: %u\n", (unsigned)gi.colortransform); return 2; }
    else if (gi.id_rgb!=0) colorspace=CS_RGB;
    /* Imp: check for id_ycbcr */
    else printf("JAI: assuming YCbCr color space\n");
  } else if (gi.cpp==4) {
    colorspace=CS_CMYK;
    if (gi.colortransform==0) ;
    else if (gi.colortransform==2) colorspace=CS_YCCK;
    else if (gi.colortransform!=127) { printf("JAI: unknown ColorTransform: %u\n", (unsigned)gi.colortransform); return 2; }
  } else assert(0);
  fseek(f, 0L, 2); /* EOF */
  long flen=ftell(f); /* skip extra bytes after EOI */
  fclose(f);
  printf("%lu: flen\n", flen);
  printf("bpc=%u cpp=%u had_jfif=%u colortransform=%u id_rgb=%u colorspace=%u\n",
    gi.bpc, gi.cpp, gi.had_jfif, gi.colortransform, gi.id_rgb, colorspace);
  printf("wd=%u ht=%u SOF_offs=%u\n", gi.width, gi.height, gi.SOF_offs);
  return 0;
}

/* __EOF__ */
