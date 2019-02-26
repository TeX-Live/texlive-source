#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(KPATHSEA)
/* including <kpathsea/kpathsea.h> would yield conflicting typedefs */
#include <kpathsea/lib.h>
#include <kpathsea/tex-file.h>
#include <kpathsea/c-fopen.h>
#include <kpathsea/progname.h>
#include <kpathsea/c-unistd.h>
# if defined (WIN32) 
#include <kpathsea/getopt.h>
# else
#include <unistd.h>
# endif
#elif defined(WIN32)
#include <getopt.h>
#else
#include <unistd.h>
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ttf.h"
#include "ttfutil.h"
#include "ttc.h"

/* $Id: ttfdump.c,v 1.2 1998/07/04 13:17:52 werner Exp $  */

#define MAXLEN 256
#define ALL_GLYF -1
#define NO_GLYF -2

static char ttfname[MAXLEN], dumpname[MAXLEN];
static TTFontPtr font;

static void print_ttc(TTCHeaderPtr ttc, FILE *out);
static void print_prologue(FILE *out);
static void print_offset(FILE *out);
static void print_dir(FILE *out);
static void print_all_tables(FILE *dp_file);
static void print_table(FILE *out, char *tablename);

static void print_cmap(FILE *out);
static void print_glyf(FILE *out, ULONG index);
static void print_all_glyfs(FILE *out);
static void print_head(FILE *out);
static void print_hhea(FILE *out);
static void print_hmtx(FILE *out);
static void print_loca(FILE *out);
static void print_maxp(FILE *out);
static void print_name(FILE *out);
static void print_post(FILE *out);
static void print_os2(FILE *out);

static void print_cvt(FILE *out);
static void print_fpgm(FILE *out);
static void print_gasp(FILE *out);
static void print_hdmx(FILE *out);
static void print_kern(FILE *out);
static void print_ltsh(FILE *out);
static void print_pclt(FILE *out);
static void print_prep(FILE *out);
static void print_vdmx(FILE *out);
static void print_vhea(FILE *out);
static void print_vmtx(FILE *out);

static void print_gpos(FILE *out);
static void print_gsub(FILE *out);

static void usage (void);


int
main(int argc, char *argv[])
{
  FILE *dp_file;
  TTCHeaderPtr ttc = NULL;	/* avoid uninitialized warning */

  int c;
  extern int optind;
  extern char *optarg;

  char *tablename = NULL;


  /* print all glyphs by default */
  int glyphnum = ALL_GLYF, collection = 0;

#ifdef KPATHSEA
  kpse_set_program_name(argv[0], "ttfdump");
#endif

  if (argc < 2)
  {
    usage();
    exit(EXIT_FAILURE);
  }

  while ((c = getopt(argc, argv, "t:g:c:i:o:h")) != EOF)
  {
    switch (c)
    {
    case 't':
      tablename = xstrdup(optarg);
      break;
    case 'g':
      if (strcmp(optarg, "x") == 0)
        glyphnum = NO_GLYF;
      else
        glyphnum = atoi(optarg);
      break;
    case 'c':
      collection = atoi(optarg);
      break;
    case 'i':
      if(strlen(optarg) >= MAXLEN) {
        fprintf(stderr, "Too long input file name\n");
        exit(EXIT_FAILURE);
      }
      strcpy(ttfname, optarg);
      break;
    case 'o':
      if(strlen(optarg) >= MAXLEN) {
        fprintf(stderr, "Too long output file name\n");
        exit(EXIT_FAILURE);
      }
      strcpy(dumpname, optarg);
      break;
    case 'h':
    case '?':
      usage();
      exit(EXIT_FAILURE);
      break;
    }
  }

  /* processing ttf file if -i flag is not given */
  if (*ttfname == 0 && optind < argc)
  {
    if(strlen(argv[optind]) >= MAXLEN) {
      fprintf(stderr, "Too long input file name\n");
      exit(EXIT_FAILURE);
    }
    strcpy(ttfname, argv[optind]);
  }

  /* processing dumping file if -o flag is not given */
  if (*dumpname == 0)
  {
    if (optind + 1 < argc)
    {
      /* no -o flag but dumping filename specified */
      strcpy(dumpname, argv[optind + 1]);
    }
    else
      strcpy(dumpname, "-");
  }
  if (!strcmp(dumpname, "-"))
  {
#ifdef WIN32
    _setmode(fileno(stdout), _O_BINARY);
#endif
    dp_file = stdout;
  }
  else if ((dp_file = fopen(dumpname, "wb")) == NULL)
  {
    fprintf(stderr, "Can't open dumping file\n");
    exit(EXIT_FAILURE);
  }

  if (strstr (ttfname, "ttc") != NULL || strstr (ttfname, "TTC") != NULL)
  {
    ttc = ttfLoadTTCHeader(ttfname);

    if (ttc == NULL)
      exit(EXIT_FAILURE);

    if (collection < ttc->DirCount)
      font = ttc->font + collection;
    else
      fprintf (stderr, "TrueType collection number too large\n"
               "should between 0 and %d\n", ttc->DirCount - 1);
  }
  else
    font = ttfInitFont(ttfname);

  if (font == NULL)
    exit(EXIT_FAILURE);

  print_prologue(dp_file);
  if (strstr(ttfname, "ttc") != NULL || strstr(ttfname, "TTC") != NULL)
  {
    print_ttc(ttc, dp_file);
  }
  print_offset(dp_file);
  print_dir(dp_file);

  if (font->loca == NULL)
    glyphnum = NO_GLYF;

  if (tablename == NULL)
  {
    /* no table specified */
    print_all_tables(dp_file);
    if (glyphnum == ALL_GLYF)
      print_all_glyfs(dp_file);
    else if (glyphnum == NO_GLYF)
      ;
    else
      print_glyf(dp_file, glyphnum);
  }
  else
  {
    /* table specified */
    print_table(dp_file, tablename);
    if (!strcmp(tablename, "glyf"))
    {
      if (glyphnum == ALL_GLYF)
        print_all_glyfs(dp_file);
      else if (glyphnum == NO_GLYF)
        ;
      else
        print_glyf(dp_file, glyphnum);
    }
  }

  if (strstr(ttfname, "ttc") != NULL || strstr(ttfname, "TTC") != NULL)
  {
    ttfFreeTTCFont(ttc);
  }
  else
  {
    ttfFreeFont(font);
  }
  return 0;
}


static void
print_table(FILE *out, char *tablename)
{
  if (!strcmp(tablename, "cmap"))
    print_cmap(out);
  if (!strcmp(tablename, "head"))
    print_head(out);
  if (!strcmp(tablename, "hhea"))
    print_hhea(out);
  if (!strcmp(tablename, "hmtx"))
    print_hmtx(out);
  if (!strcmp(tablename, "loca"))
    print_loca(out);
  if (!strcmp(tablename, "maxp"))
    print_maxp(out);
  if (!strcmp(tablename, "name"))
    print_name(out);
  if (!strcmp(tablename, "post"))
    print_post(out);
  if (!strcmp(tablename, "OS/2"))
    print_os2(out);
  if (!strcmp(tablename, "cvt"))
    print_cvt(out);
  if (!strcmp(tablename, "fpgm"))
    print_fpgm(out);
  if (!strcmp(tablename, "gasp"))
    print_gasp(out);
  if (!strcmp(tablename, "hdmx"))
    print_hdmx(out);
  if (!strcmp(tablename, "kern"))
    print_kern(out);
  if (!strcmp(tablename, "LTSH"))
    print_ltsh(out);
  if (!strcmp(tablename, "prep"))
    print_prep(out);
  if (!strcmp(tablename, "VDMX"))
    print_vdmx(out);
  if (!strcmp(tablename, "vhea"))
    print_vhea(out);
  if (!strcmp(tablename, "vmtx"))
    print_vmtx(out);
  if (!strcmp(tablename, "GPOS"))
    print_gpos(out);
  if (!strcmp(tablename, "GSUB"))
    print_gsub(out);
}


static void
print_all_tables(FILE *dp_file)
{
  print_cmap(dp_file);
  print_head(dp_file);
  print_hhea(dp_file);
  print_hmtx(dp_file);
  print_loca(dp_file);
  print_maxp(dp_file);
  print_name(dp_file);
  print_post(dp_file);
  print_os2(dp_file);

  /*optional tables */
  print_cvt(dp_file);
  print_gasp(dp_file);
  print_hdmx(dp_file);
  print_kern(dp_file);
  print_ltsh(dp_file);
  print_pclt(dp_file);
  print_vdmx(dp_file);
  print_vhea(dp_file);
  print_vmtx(dp_file);

  /* Additional OpenType Tables */
  print_gpos(dp_file);
  print_gsub(dp_file);
}


static void
print_ttc(TTCHeaderPtr ttc, FILE *out)
{
  int i, b[2];

  FixedSplit(ttc->version, b);

  fprintf(out, "TrueType Collection Header\n");
  fprintf(out, "--------------------------\n");
  fprintf(out, "TTC version: \t\t %d.%d\n", b[1], b[0]);
  fprintf(out, "Number of fonts:\t %d\n", ttc->DirCount);
  for (i = 0; i < ttc->DirCount; i++)
    fprintf(out, "Offset of Directory #%d:\t %d\n", i, ttc->offset[i]);
  fprintf(out, "\n");
}


static void
print_prologue(FILE *out)
{
  fprintf(out, "True Type Font File Dumper: v 0.5.5\n");
  fprintf(out, "Copyright 1996-1998 ollie@ms1.hinet.net\n");
  fprintf(out, "Dumping File:%s\n\n\n", font->ttfname);
}


static void
print_offset(FILE *out)
{
  int b[2];

  FixedSplit(font->version, b);

  fprintf(out, "Offset Table\n");
  fprintf(out, "------------\n");
  fprintf(out, "\t sfnt version: ");
  if (b[1] & 0xff00)
    fprintf(out, "'%s'\n", TagToStr (font->version));
  else
    fprintf(out, "\t\t %d.%d\n", b[1], b[0]);
  fprintf(out, "\t number of tables: %d\n", font->numTables);
}


static void
print_dir(FILE *out)
{
  int i;


  for (i = 0; i < font->numTables; i++)
  {
    fprintf(out, "  %2d. ", i);
    ttfPrintTableDir(out, font->dir + i);
  }
  fprintf(out, "\n");
}

static void
print_cmap(FILE *out)
{
  ttfPrintCMAP(out, font->cmap);
  fprintf(out, "\n");
}

static void
print_glyf(FILE *out, ULONG index)
{
  GLYFPtr glyf;


  glyf = ttfLoadGlyphIndex(font, index);
  fprintf(out, "Glyph %6d.\n", index);
  ttfPrintGLYF(out, glyf);
}


static void
print_all_glyfs(FILE *out)
{
  int i;
  GLYFPtr glyf;


  for (i = 0; i < font->maxp->numGlyphs; i++)
  {
    glyf = ttfLoadGlyphIndex(font, i);
    fprintf(out, "Glyph %6d.\n", i);
    ttfPrintGLYF(out, glyf);
  }
}


static void
print_head(FILE *out)
{
  ttfPrintHEAD(out, font->head);
  fprintf(out, "\n");
}


static void
print_hhea(FILE *out)
{
  ttfPrintHHEA(out, font->hhea);
  fprintf(out, "\n");
}


static void
print_hmtx(FILE *out)
{
  /* because of interdependencies between tables
   * we have to init hhea and maxp before hmtx */
  ttfPrintHMTX(out, font->hmtx);
  fprintf(out, "\n");
}


static void
print_loca(FILE *out)
{
  /* because of interdependencies between tables
   * we have to init head and maxp before loca */
  ttfPrintLOCA(out, font->loca);
  fprintf(out, "\n");
}


static void
print_maxp(FILE *out)
{
  ttfPrintMAXP(out, font->maxp);
  fprintf(out, "\n");
}


static void
print_name(FILE *out)
{
  ttfPrintNAME(out, font->name);
  fprintf(out, "\n");
}


static void
print_post(FILE *out)
{
  ttfPrintPOST(out, font->post);
}

static void
print_os2(FILE *out)
{
  ttfPrintOS2(out, font->os2);
}


static void
print_cvt(FILE *out)
{
  if (font->cvt != NULL)
    ttfPrintCVT(out, font->cvt, font->cvtLength);
}


static void
print_fpgm(FILE *out)
{
  if (font->fpgm != NULL)
    ttfPrintFPGM(out, font->fpgm, font->fpgmLength);
}


static void
print_gasp(FILE *out)
{
  if (font->gasp != NULL)
    ttfPrintGASP(out, font->gasp);
}


static void
print_hdmx(FILE *out)
{
  if (font->hdmx != NULL)
    ttfPrintHDMX(out, font->hdmx);
}


static void
print_kern(FILE *out)
{
  if (font->kern != NULL)
    ttfPrintKERN(out, font->kern);
}


static void
print_ltsh(FILE *out)
{
  if (font->ltsh != NULL)
    ttfPrintLTSH(out, font->ltsh);
}


static void
print_pclt(FILE *out)
{
  if (font->pclt != NULL)
    ttfPrintPCLT(out, font->pclt);
}


static void
print_prep(FILE *out)
{
  if (font->prep != NULL)
    ttfPrintPREP(out, font->prep, font->prepLength);
}


static void
print_vdmx(FILE *out)
{
  if (font->vdmx != NULL)
    ttfPrintVDMX(out, font->vdmx);
}


static void
print_vhea(FILE *out)
{
  if (font->vhea != NULL)
    ttfPrintVHEA(out, font->vhea);
}


static void
print_vmtx(FILE *out)
{
  if (font->vmtx != NULL)
    ttfPrintVMTX(out, font->vmtx);
}


static void
print_gpos(FILE *out)
{
  if (font->gpos != NULL)
    ttfPrintGPOS(out, font->gpos);
}


static void
print_gsub(FILE *out)
{
  if (font->gsub != NULL)
    ttfPrintGSUB(out, font->gsub);
}


static void
usage(void)
{
  fprintf(stderr, "Usage: ttfdump ttfname dumpname [options]\n");
}


/* end of ttfdump.c */
