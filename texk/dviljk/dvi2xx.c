/* $Id: dvi2xx.c,v 2.5 1997/12/08 20:52:20 neumann Exp $ */
#define VERSION "dviljk (version 2.6p5)"
/*
#define DEBUGGS 1
*/
/**********************************************************************
 ****************************  Intro  *********************************
 **********************************************************************
 * This program translates TeX's DVI-Code into device dependent
 * code of either the
 *
 *     -   HP-LASERJET+ and compatibles (PCL), or the
 *     -   IBM 3812 pageprinter
 *
 * depending on the preprocessor switches specified before compilation.
 * The program is written to run on a PC XT/AT/PS2 under MS-DOS. It can
 * be compiled nicely with MSC Rel. 3.0-5.1 with option -AL (large memory
 * model).  Take care that in the CONFIG.SYS file the FILES parameter
 * is set to 20; otherwise reduce MAXOPEN.  640K are recommended.
 * I use link option /stack:9000 to increase runtime stack.
 * It also works without modifications under Unix System V.
 **********************************************************************
 *            Adapted for the PC:    Gustaf Neumann
 *            +1002 stuff        University of Economics
 *            +3812 support      Augasse 2-6
 *            +Output buffering      A-1090 Vienna, AUSTRIA
 *            +lpt binary support    Tel. *43-222-340525/533
 *            +pk-89 stuff                   773
 *            +pixelpaths
 *            +alternative directory structure
 *            +code compiles also under Unix V (HP/UX)
 *                      (thx Michael Haberler)
 *            +huge characters (a character bigger than 32K)
 *                      formats PXL1001 and PXL 1002
 *                     (use: raster graphics)
 *            +reduction of the produced code
 *            +new options -X -Y -c -g
 *            +changed options -r (LJ now default from first to last)
 *                     -x,-y  (accept floats)
 *            +new option -z for LJ: print testpage containing
 *                     pagecounter after printjob
 *            +try to overcome font limit on LJ (max 16 fonts/page) and
 *             (max 32 fonts/document):
 *                     additional fonts are drawn as bitmap-
 *                     graphics.
 *            +allows to set character close to the paperedge on LJ
 *            +gf-supprt (by Joe Kelsey joe@Pacer.com)
 *                gf.c and gf.h from mitdevices/dvi2ps
 *            +clipping of rules
 *            +OS/2 defines from Rutger Berns, apprmb@hheouh50.bitnet
 *
 *            BITNET/EARN:       NEUMANN at AWIWUW11
 **********************************************************************
 * fixes in LJ-mode:  rule-drawing,
 *            characters with 127<=height<=200
 *            reset printer at beginning and end of each job
 *            better positioning of rules
 * 30.1.89 (0.48) bug fixed for files containing >32 fonts (thanks A. Brosig),
 *                different font assignment heuristic
 * fixes in 3812-mode:  14.juli 87  positioning of rastered characters
 *            better positioning of rules
 * general fixes
 * 1.1.88         page origin set to 1in/1in (hopefully everywhere)
 * 22.7.88        reset y-position for each page-eject
 * 15.1.89        fixing bug is space allocation for EmitFileName
 *                (thanks to Bernhard Simon)
 * 15.3.91 (0.49) landscape support for lj ii p, lj iii and lj 2000
 *                fixing rule drawing problems for IBM3812 in landcape mode,
 *                256 character clean (lj family and IBM3812)
 * 5.5.91 (0.50)  -DSEVENBIT added for older LJ-emulations
 *                -D1, -D2, -D-, -D1-, -D2- options added due to suggestions
 *                from Tomasz Wolniewicz
 **********************************************************************
 * Preprocessor switches:
 *      #define DEBUG    for massive printing of trace information
 *               when -d cmdline option specified
 *      #define IBM3812  produce output for the IBM3812 pageprinter
 *      #define LJ       produce output for the HP Laserjet+ or LJ II
 *      #define LJ2P     produce output for the HP Laserjet LJ IIP, LJ III
 *                       or LaserJet 2000
 *      #define LJ_LARGE_FONT_MEMORY  large FONT Memory for LJ printer family
 *      #define DRAWGLYPH draws PK-Glyphs on stderr
 *      #define USEPXL   use PXL and PK fonts rather than gf fonts
 **********************************************************************
 *             Adapted for Acorn RISC OS computers: Andreas Dehmel
 *             Address:    Andreas Dehmel
 *                       Am Schorn 18
 *                       82327 Tutzing
 *                       Germany
 *                       dehmel@informatik.tu-muenchen.de
 *
 *             Changes:
 *           - Split the whole thing up into smaller files
 *           - Rearranged certain parts of the output
 *           - If fonts are missing a taskobey file is created that
 *             contains MF commands to build the fonts.
 *           - SUPPORT FOR DIAGRAMS AS USED IN DVIVIEW!!!
 *           - Fixed problem of raster chars being scrambled when diagram
 *             printouts were on the same page.
 *             - Raster chars and fonts can now also be downloaded in
 *             compressed format; many thanks to Karl Berry!
 */

#include "dvi2xx.h"
#include "tfm.h"

#ifdef SEVENBIT
#define VIS   33
#define VIS2  (VIS+32)
static unsigned char
VisChar(unsigned char c)
{
  c &= 0xff;
  if (c < VIS)
    return ((unsigned char)(160 + c));
  if (c < 128)
    return (c);
  if (c < (255 - VIS2))
    return ((unsigned char)(VIS2 + c));
  return (255);
}
#endif


/**********************************************************************/
/*******************************  main  *******************************/
/**********************************************************************/
int
main(int argc, char *argv[])
{
  struct stack_entry {  /* stack entry */
    long4    h, v, w, x, y, z;  /* what's on stack */
  };
  short   command;           /* current command                         */
  long4   count[10];         /* the 10 counters at begining of each page*/
  long4   cpagep = 0;        /* current page pointer                    */
  bool    Emitting = _FALSE; /* outputting typsetting instructions?     */
  int     i;                 /* command parameter; loop index           */
  int     k;                 /* temporary parameter                     */
  char    n[STRSIZE];        /* command parameter                       */
  int     PassNo = 0;        /* which pass over the DVI page are we on? */
  bool    SkipMode = _FALSE; /* in skip mode flag                       */
  int     sp = 0;            /* stack pointer                           */
  struct  stack_entry stack[STACK_SIZE];  /* stack                      */
  char    SpecialStr[STRSIZE]; /* "\special" strings                    */
  long4   val, val2;         /* temporarys to hold command information  */
  long4   w = 0;             /* current horizontal spacing              */
  long4   x = 0;             /* current horizontal spacing              */
  long4   y = 0;             /* current vertical spacing                */
  long4   z = 0;             /* current vertical spacing                */

#ifdef vms
  extern noshare int errno;
  extern noshare char *sys_errlist[];
#else
# if !defined (__riscos) && !defined (KPATHSEA)
  extern  char *sys_errlist[];
  extern  int   errno;
# endif
#endif

  /* Initialize pixel_files */
  for (i = 0; i <= MAXOPEN; i++)
    pixel_files[i].pixel_file_id = FPNULL;

  x_origin = XDEFAULTOFF; /* x-origin in dots                    */
  y_origin = YDEFAULTOFF; /* y-origin in dots                    */

  setbuf(ERR_STREAM, NULL);
#ifdef KPATHSEA
  kpse_set_program_name(argv[0], "dvilj");
  kpse_set_program_enabled (kpse_pk_format, MAKE_TEX_PK_BY_DEFAULT, kpse_src_compile);
  G_progname = kpse_program_name;
#else
  G_progname = argv[0];
#endif
  DecodeArgs(argc, argv);

#ifdef LJ4
  if (CompressCharWidth < 0) {
    switch(UseCompression) {
    case 0: CompressCharWidth = COMPRESS_WIDTH0; break;
    case 2: CompressCharWidth = COMPRESS_WIDTH2; break;
    case 3: CompressCharWidth = COMPRESS_WIDTH3; break;
    }
  }
#endif

#ifdef KPATHSEA
  kpse_init_prog("DVILJ", RESOLUTION, MFMODE, "cmr10");
#endif

  power[0] = 1;
  for (i = 1; i <= 31; i ++)
    power[i] = power[i - 1] << 1;
  gpower[0] = 0l;
  for ( i = 1; i <= 32; i ++)
    gpower[i] = gpower[i - 1] + power[i - 1];

  if ((i = (int)NoSignExtend(dvifp, 1)) != PRE) {
    Fatal("%s: PRE doesn't occur first--are you sure this is a DVI file?\n\n",
          G_progname);
  }
  i = (int)SignExtend(dvifp, 1);
  if (i != DVIFORMAT) {
    Fatal( "%s: DVI format = %d, can only process DVI format %d files\n\n",
           G_progname, i, DVIFORMAT);
  }

  if (*EmitFileName == '-') {
#ifdef RISC_USE_OSL
    EMTO = BOUTOPEN("Vdu:");
#else
    EMTO = stdout;
#endif
  }
  else
    if ((EMTO = BOUTOPEN(EmitFileName)) == FPNULL)
#ifndef KPATHSEA
      Fatal("opening output file: fopen(%s) : %s", EmitFileName,
#ifdef __riscos
            "Cannot open"
#else
            sys_errlist[errno]
#endif
            )
#endif /* not KPATHSEA */
            ;
#ifdef __riscos
  xosfile_set_type(EmitFileName,0xff4);
#endif

  /* If EmitFileName is "-", we use stdout.  But it is by default open
     in TEXT mode, and we need to switch it to binary, unless they are
     writing to the screen (in which case they get what they deserve).  */
  if (O_BINARY && !isatty(fileno(outfp)))
    (void)AssureBinary(fileno(outfp));

#ifdef TIMING
#ifdef BSD_TIME_CALLS
  ftime(&timebuffer);
  start_time = timebuffer.time + (float)(timebuffer.millitm) / 1000.0;
#else
  gettimeofday(&Tp, NULL);
  start_time = Tp.tv_sec + ((float)(Tp.tv_usec))/ 1000000.0;
#endif
#endif

  /* it is important that these be the very first things output !!! */
  if ( G_header )
    my_CopyFile( HeaderFileName );

  /*****************************/
  /*for( i0=0; i0<nif; i0++ )  */    /* copy all included files */
  /*    my_CopyFile( Ifile[i0] ); */
  /*****************************/

#ifdef IBM3812
  PMPout(3, "\307\310\366");          /* unload all fonts and macros */
  EMITWORD(MAX_PAGE_WIDTH);
  EMITWORD(MAX_PAGE_HEIGHT);
  if (Landscape)
    PMPout(2, "\322\1");
#endif

#ifdef LJ
# ifdef LJ4
  /* According to the PCL (p. 4-2) and PJL (p. 4-3) reference manuals, it is
     critical that the UEL escape sequence (\e%-12345X) comes before the
     reset sequence (\eE). According to PJL Reference Manual (p. 4-3) the
     correct order is (1) UEL, (2) PJL commands, (3) Reset and PCL job, (4)
     Reset, (5) UEL. */
  if (my_ResetPrinter) {
    EMIT1("\033%%-12345X"); /* UEL: Universal Exit Language */
    EMIT2("@PJL SET RESOLUTION=%d\012",RESOLUTION);
    EMIT1("@PJL SET PAGEPROTECT=OFF\012");
    if (econoMode && LJ6)
      EMIT1("@PJL SET ECONOMODE=ON\012");
    /* The PJL ENTER LANGUAGE command must be the last PJL command before
       PCL output starts. */
    EMIT1("@PJL ENTER LANGUAGE=PCL\012");
    EMIT1("\033E");
  }
  EMIT3("\033&u%dD\033*t%dR",RESOLUTION,RESOLUTION);
  if (econoMode && !LJ6)
    EMIT1("\033*v1T");
# else
  if (my_ResetPrinter)
    EMIT1("\033E");
# endif
# ifdef LJ2P
  if (DuplexMode)
    EMIT2("\033&l%dS", DuplexMode);
# endif
  if (Landscape)
    EMIT1("\033&l1O\033*rF");
  if (pagesize>0) {
# ifndef vms
    EMIT2("\033&l%hdaE\033&aL", pagesize);
# else
    EMIT2("\033&l%daE\033&aL", pagesize);
# endif
  } else
    EMIT1("\033&lE\033&aL");

  if (ncopies>1) {
# ifndef vms
    EMIT2("\033&l%hdX", ncopies);
# else
    EMIT2("\033&l%dX", ncopies);
# endif
  }
#endif /* LJ */

  if (DoublePage) {
    StartPrintPages = PrintPages;
#ifdef IBM3812
    Reverse = (bool)!Reverse; /* perverse and strange */
#endif
  }

  if (ManualFeed)
      EMIT(outfp, "\033&l2H"); /* Manual Feed */

  if (Reverse) {
#ifdef DEBUG
    if (Debug)
      fprintf(ERR_STREAM, "reverse\n");
#endif
    ReadPostAmble(_TRUE);
    FSEEK(dvifp, ppagep, SEEK_SET);
  } else {
    ReadPostAmble(_TRUE);
    FSEEK(dvifp,  14l, SEEK_SET);
    k = (int)NoSignExtend(dvifp, 1);
    GetBytes(dvifp, n, k);
  }
  PassNo = 0;

  while (_TRUE)  {
    command = (short) NoSignExtend(dvifp, 1);
#ifdef DEBUG
    if (Debug)
      fprintf(ERR_STREAM,"CMD@%ld:\t%d\n", (long) ftell(dvifp) - 1, command);
#endif
    switch (command)  {
    case SET1:
    case SET2:
    case SET3:
    case SET4:
      val = NoSignExtend(dvifp, (int)command - SET1 + 1);
      if (!SkipMode)
        SetChar(val, command, PassNo, _TRUE,_FALSE);
      break;
    case SET_RULE:
      val = NoSignExtend(dvifp, 4);
      val2 = NoSignExtend(dvifp, 4);
      if (Emitting)
        SetRule(val, val2, 1);
      break;
    case PUT1:
    case PUT2:
    case PUT3:
    case PUT4:
      val = NoSignExtend(dvifp, (int)command - PUT1 + 1);
      if (!SkipMode)
        SetChar(val, command, PassNo, _TRUE,_FALSE);
      break;
    case PUT_RULE:
      val = NoSignExtend(dvifp, 4);
      val2 = NoSignExtend(dvifp, 4);
      if (Emitting)
        SetRule(val, val2, 0);
      break;
    case NOP:
      break;
    case BOP:
      cpagep = FTELL(dvifp) - 1;
      DEBUG_PRINT ("BOP for [");
      for (i = 0; i <= 9; i++) {
        count[i] = NoSignExtend(dvifp, 4);
        DEBUG_PRINT1 ("%ld.", (long) count[i]);
      }
      DEBUG_PRINT1 ("] at %ld.\n", (long)cpagep);
      ppagep = (long)NoSignExtend(dvifp, 4);
      h = v = w = x = y = z = 0;
      hh = vv = 0;
      last_rx = last_ry = UNKNOWN;
      sp = 0;
      fontptr = NULL;
      prevfont = NULL;
      DoBop();
      /*
        fprintf(ERR_STREAM,"skimode %d, count %d, F %d, L %d\n",
        (int)SkipMode,(int)count[0],(int)FirstPageSpecified,(int)LastPageSpecified);
        */
      SkipMode = (bool)((FirstPageSpecified && count[0] < FirstPage) ||
                        (LastPageSpecified && count[0] > LastPage ));
      /*
        fprintf(ERR_STREAM,"skimode %d, count %d, F %d, L %d\n",
        (int)SkipMode,(int)count[0],(int)FirstPageSpecified,(int)LastPageSpecified);
        */

      if (DoublePage && !SkipMode) {
        if (PassNo == 0) {
          LastPtobePrinted = count[0];
          if (!Reverse && (WouldPrint == 0)) {
            if (count[0] == 0l) {
              ZeroPage = _TRUE;
              EvenPage = _FALSE;
            }
            else {
              EvenPage = (bool)( (count[0]<0? labs(count[0])+1: count[0]) %2 == 0);

              if (PrintEmptyPages && EvenPage && PageParity==1) {
                WouldPrint ++;
                if (PrintFirstPart) {
                  qfprintf(ERR_STREAM,"[EvenPage] ");
                  FormFeed();
                }
              }
            }
          }
          WouldPrint ++;
          /*
            fprintf(ERR_STREAM, "doublepage %d, page parity = %d, 1=%d 2=%d, Reverse %d, WouldPrint %d, fpZ %d\n",
            (int)DoublePage, (int)PageParity,(int)PrintFirstPart,(int)PrintSecondPart,
            (int)Reverse, (int)WouldPrint, (int)ZeroPage);
            */
        }
        if (!PrintFirstPart && PageParity==1) {
          if (count[0] == 0) {
            ZeroPage = _TRUE;
            EvenPage = _FALSE;
          }
          SkipMode = _TRUE;
        }
        else {
          /*
            fprintf(ERR_STREAM,"FirstPart\n count %d, mod %d, pp %d\n",(int)count[0],(int)count[0]%2,PageParity);
            */
          SkipMode =
            (bool)(PageParity != (short)((count[0]<0 ?
                                          labs(count[0])+1 : count[0])%2));
          if (count[0] == 0) SkipMode = (bool)!SkipMode;
        }

      }
      Emitting = (bool)((PassNo != 0) && !SkipMode);
      /*
        fprintf(ERR_STREAM,"Emitting= %d, PassNo=%d, SkipMode = %d\n",(int)Emitting,(int)PassNo,(int)SkipMode);
        */
      if ( !SkipMode ) {
        if (PassNo == 0)
          qfprintf(ERR_STREAM,"[%ld",  (long)count[0]);
      }
      break;
    case EOP:
      if ( !SkipMode ) {
        if (PassNo == 0) {
          /* start second pass on current page */
          FSEEK(dvifp, cpagep, SEEK_SET);
          PassNo = 1;
#ifdef DEBUG
          if (Debug)
            fprintf(ERR_STREAM,"\nStarting second pass\n");
#endif

        } else {
          /* end of second pass, and of page processing */

          last_rx = last_ry = UNKNOWN;
          FormFeed();
          ++ndone;

          qfprintf(ERR_STREAM,"] ");
          if ( (ndone % 10) == 0 )
            qfprintf(ERR_STREAM,"\n");

          if (DoublePage) --PrintPages;
          if (--PrintPages < 1) AllDone(_TRUE);
          PassNo = 0;
        }
      } else
        PassNo = 0;

      if ( PassNo == 0 && Reverse ) {
        if ( ppagep > 0 )
          FSEEK(dvifp, ppagep, SEEK_SET);
        else {
          if (DoublePage && !SkipMode)
            ZeroPage = (bool)(count[0] == 0);

          if (ZeroPage)
            EvenPage = _FALSE;
          else
            EvenPage = (bool)((int)LastPtobePrinted%2 == 0);

          AllDone(_FALSE);
        }
      }
      break;
    case PUSH:
      if (sp >= STACK_SIZE)
        Fatal("stack overflow");
      stack[sp].h = h;
      stack[sp].v = v;
      stack[sp].w = w;
      stack[sp].x = x;
      stack[sp].y = y;
      stack[sp].z = z;
      sp++;
      break;
    case POP:
      --sp;
      if (sp < 0)
        Fatal("stack underflow");
      h = stack[sp].h;
      v = stack[sp].v;
      w = stack[sp].w;
      x = stack[sp].x;
      y = stack[sp].y;
      z = stack[sp].z;
      last_rx = last_ry = UNKNOWN;
      break;
    case RIGHT1:
    case RIGHT2:
    case RIGHT3:
    case RIGHT4:
      val = SignExtend(dvifp, (int)command - RIGHT1 + 1);
      if (Emitting)
        MoveOver(val);
      break;
    case W0:
      if (Emitting)
        MoveOver(w);
      break;
    case W1:
    case W2:
    case W3:
    case W4:
      w = SignExtend(dvifp, (int)command - W1 + 1);
      if (Emitting)
        MoveOver(w);
      break;
    case X0:
      if (Emitting)
        MoveOver(x);
      break;
    case X1:
    case X2:
    case X3:
    case X4:
      x = SignExtend(dvifp, (int)command - X1 + 1);
      if (Emitting)
        MoveOver(x);
      break;
    case DOWN1:
    case DOWN2:
    case DOWN3:
    case DOWN4:
      val = SignExtend(dvifp, (int)command - DOWN1 + 1);
      if (Emitting)
        MoveDown(val);
      break;
    case Y0:
      if (Emitting)
        MoveDown(y);
      break;
    case Y1:
    case Y2:
    case Y3:
    case Y4:
      y = SignExtend(dvifp, (int)command - Y1 + 1);
      if (Emitting)
        MoveDown(y);
      break;
    case Z0:
      if (Emitting)
        MoveDown(z);
      break;
    case Z1:
    case Z2:
    case Z3:
    case Z4:
      z = SignExtend(dvifp, (int)command - Z1 + 1);
      if (Emitting)
        MoveDown(z);
      break;
    case FNT1:
    case FNT2:
    case FNT3:
    case FNT4:
      k = NoSignExtend(dvifp, (int) command - FNT1 + 1);
      if (!SkipMode) {
        SetFntNum(k, Emitting);
      }
      break;
    case XXX1:
    case XXX2:
    case XXX3:
    case XXX4:
      k = (int)NoSignExtend(dvifp, (int)command - XXX1 + 1);
      GetBytes(dvifp, SpecialStr, k);
      if (Emitting)
        DoSpecial(SpecialStr, k);
      break;
    case FNT_DEF1:
    case FNT_DEF2:
    case FNT_DEF3:
    case FNT_DEF4:
      k = (int)NoSignExtend(dvifp, (int)command - FNT_DEF1 + 1);
      SkipFontDef();    /* SkipFontDef(k); */
      break;
    case PRE:
      Fatal("PRE occurs within file");
      break;
    case POST:
      AllDone(_FALSE);
      PassNo = 0;
      break;
    case POST_POST:
      Fatal("POST_POST with no preceding POST");
      break;
    default:
      if (command >= FONT_00 && command <= FONT_63) {
        if (!SkipMode)
          SetFntNum((long4)command - FONT_00, Emitting);
      } else if (command >= SETC_000 && command <= SETC_127) {
        if (!SkipMode) {
          SetString(command, PassNo);
        }
      } else
        Fatal("%d is an undefined command", command);
      break;
    }
  } /* while _TRUE */
}






/*------------------------ begin dviIO.c ----------------------------------*/

/* The following functions buffer input/output during my_CopyFile / CopyHPFile
   Write functions are only needed if RISC_BUFFER is defined; otherwise output
   is not buffered. */

/* read a buffered byte */
char
b_read(FILEPTR spfp)
{
  if (biact >= binumber) {
#ifdef RISC_USE_OSL
    binumber = BUFFSIZE - read_multi(buffin,1,BUFFSIZE,spfp);
#else
    binumber = read_multi(buffin,1,BUFFSIZE,spfp);
#endif
    biact = 0;
  }
  return binumber == 0 ? 0 : buffin[biact++];
}

#ifdef RISC_BUFFER
void
b_write(FILEPTR spfp, char c) /* write a buffered byte */
{
  if (boact >= BUFFSIZE) {
    write_multi(buffout,1,BUFFSIZE,spfp);
    boact = 0;
  }
  buffout[boact++] = c;
}

void
b_wrtmult(FILEPTR spfp, char *buf, int len) /* write a sequence of bytes to the output buffer */
{
  register int i;

  if ((len > (BUFFSIZE - boact)) || (len >= (BUFFSIZE/4))) {
    write_multi(buffout,1,boact,spfp);
    /* Copy only small blocks; large ones are written directly */
    if (len < (BUFFSIZE/4)) {
      for (i = 0; i<len; i++)
        buffout[i] = buf[i];
      boact = len;
    } else {
      write_multi(buf,1,len,spfp);
      boact = 0;
    }
  }
  else {
    for (i = 0; i<len; i++)
      buffout[boact++] = buf[i];
  }
}

/* flush the output buffer */
void
b_oflush(FILEPTR spfp)
{
  write_multi(buffout,1,boact,spfp);
  boact = 0;
}
#endif
/* end of buffer handling functions */


/*-->my_CopyFile*/   /* copy a file straight through to output */
/*********************************************************************/
/***************************** my_CopyFile ***************************/
/*********************************************************************/
void
my_CopyFile(const char *str )
{
  FILEPTR spfp;
  int     todo;

  if ( (spfp = BINOPEN(str)) == FPNULL ) {
    if ( errno != EACCES || ! kpse_tex_hush("readable") ) {
      Warning("Unable to open file %s (errno=%d), skipping inclusion",
	      str, errno);
    }
    return;
  }
  qfprintf(ERR_STREAM," [%s", str);
#ifdef RISC_BUFFER
  b_oflush(outfp);
#endif
  do {
    todo = read_multi(buffin,1,BUFFSIZE,spfp);
    write_multi(buffin,1,todo,outfp);
  }
  while (todo == BUFFSIZE);

  BCLOSE(spfp);
  qfprintf(ERR_STREAM,"]");
}


/*-->CopyHPFile*/  /* copy a HP file to output removing unwanted control codes*/
/*********************************************************************/
/***************************** CopyHPFile ******************************/
/*********************************************************************/
static int
getnum(FILEPTR spfp, char *t, char *numstr)
{
  int count=0;
  for (*t = (char)b_read(spfp); *t<0x40; *t = (char)b_read(spfp))
    numstr[count++] = *t;
  numstr[count] = 0;
  return atoi(numstr);
}

static void
setOffset(char dir, char sign, int pos)
{
  if ((sign == '+' || sign == '-') && pos > 0 ) {
    EMIT4("\033*p%c%d%c",sign,pos,dir);
#ifdef DEBUGGS
    fprintf(stderr, "relative: *p%c%d%c\n", sign, pos, dir);
#endif
  } else if (pos>0) {
    EMIT3("\033*p%d%c",pos,dir);
#ifdef DEBUGGS
    fprintf(stderr, "absolute: *p%d%c\n", pos, dir);
#endif
    if (dir == 'X')
      last_rx = pos;
    else
      last_ry = pos;
  } else {
    /*EMIT3("\033*p%d%c",pos,dir);*/
#ifdef DEBUGGS
    fprintf(stderr, "Relative: *p%d%c\n", pos, dir);
#endif
  }
}


void
CopyHPFile(char *str )
{
  FILEPTR spfp;
  char    t,numstr[20];
  int     count,miny,minx,num;

  if ( (spfp = BINOPEN(str)) == FPNULL ) {
    if ( errno != EACCES || ! kpse_tex_hush("readable") ) {
      Warning("Unable to open file %s (errno=%d), skipping inclusion",
	      str, errno);
    }
    return;
  }
  minx = 32767;                 /* Set to a high value initially */
  miny = 32767;

  /* Pass through the input PCL file twice.  The first time find the
     smallest x- and y-offsets so that they can be subtracted out when
     sending positioning commands later.  The second pass strips the
     unwanted commands from the input file and outputs the rest */

  /* reset the input buffer */
  binumber = 0;
  biact = 0;

  qfprintf(ERR_STREAM," [%s", str);
  /* Test for !EOF now ((binumber == BUFFSIZE) || (biact < binumber)) */
  do {
    t = (char)b_read(spfp);
    if (t==0x1B) { /* Find the escape character */
      t = (char)b_read(spfp);
      if (t==0x2A) { /* This indiactes the start of a graphics command */
        t = (char)b_read(spfp);
        switch(t) {
        case('p'):
          /* These are the graphics positioning commands */
          /* Find the smallest x and y offsets */
          num = getnum(spfp, &t, numstr);

          /* Take into account the possible different ordering */
          /* of the commands (x first, y first) */

          if (t=='Y' || t=='y') {
            if (numstr[0]!='+' && numstr[0]!='-' && num<miny)
              miny = num;
            if (t=='y') {
              num = getnum(spfp, &t, numstr);
              if (numstr[0]!='+' && numstr[0]!='-' && num<minx)
                minx = num;
            }
          }
          if (t=='X' || t=='x') {
            if (numstr[0]!='+' && numstr[0]!='-' && num<minx)
              minx = num;

            if (t=='x') {
              num = getnum(spfp, &t,numstr);
              if (numstr[0]!='+' && numstr[0]!='-' && num<miny)
                miny = num;
            }
          }
          break;
          /* Ignore all other commands for the moment - just read them */
        case(0x74):
          for (t = (char)b_read(spfp); t != 0x52; t = (char)b_read(spfp));
          break;

        case(0x72):
          for (t = (char)b_read(spfp); ((t< 0x40)||(t>0x60)); t = (char)b_read(spfp));
          break;

        case(0x62):
          num = 0;
          count = 0;
          /* Read in the correct number of bytes of raster graphics */
          /* so that we don't get commands and raster graphics confused */

          for (t = (char)b_read(spfp); ((t<0x40)||(t>=0x60)); t = (char)b_read(spfp))
            numstr[count++]=t;
          numstr[count]=0;
          if (t==0x4D)
            for(t = numstr[count = 0]; t!=0; t = numstr[++count]);
          if (t==0x57) {
            for(t = numstr[count = 0]; t!=0; t = numstr[++count]);
            for(count = atoi(numstr); count>0; count--)
              t = (char)b_read(spfp);
          }
          break;

        case(0x63):
          for (t = (char)b_read(spfp); t< 0x40 || t>0x60; t = (char)b_read(spfp));
          break;

        default:
          break;
        }
      }
    }
  }
  while ((binumber == BUFFSIZE) || (biact < binumber));

  /* reset input buffer, to read it anew */
  if ( FSEEK(spfp, 0L, SEEK_SET) == -1 ) {
    Warning(" could not seek to start of file (errno=%d), abandon %s inclusion",
	    errno, str);
    return;
  }
  binumber = 0;
  biact = 0;


  /* Pass through the input file again but this time output the */
  /* retained PCL commands */
#ifdef DEBUGGS
  fprintf(stderr,"\nminx=%d, miny=%d, xg=%d, yg=%d\n",
          minx, miny, x_goffset, y_goffset);
#endif
  do {
    t = (char)b_read(spfp);
    if (t==0x1B) {
      t = (char)b_read(spfp);
      if (t==0x2A) {
        t = (char)b_read(spfp);
        switch(t) {
        case('p'):
          num = getnum(spfp, &t,numstr);
          if (t == 'Y' || t == 'y') {
            if (numstr[0]!='+' && numstr[0]!='-') {
              /* Subtract the minimum offset found in first pass */
              /* and add in the current vertical offset */
              setOffset('Y',numstr[0],
                        num-miny + (int)PIXROUND(v,vconv) + y_goffset);
            } else {
              setOffset('Y',numstr[0], num);
            }

            if (t == 'y') {
              num = getnum(spfp, &t,numstr);
              if (numstr[0]!='+' && numstr[0]!='-') {
                /*Add in correct horizontal offset */
                setOffset('X',numstr[0],
                          num - minx + (int)PIXROUND(h,hconv) + x_goffset);
              } else if (num>=0) {
                setOffset('X',numstr[0], num);
              }
            }
          }

          if (t=='X' || t=='x') {
            if (numstr[0]!='+' && numstr[0]!='-') {
              /*Add in the correct horizontal offset*/
              setOffset('X',numstr[0],
                        num - minx + (int)PIXROUND(h,hconv) + x_goffset);
            } else {
              setOffset('X',numstr[0], num);
            }

            if (t=='x') {
              num = getnum(spfp, &t,numstr);
              if (numstr[0]!='+' && numstr[0]!='-') {
                /* Subtract the minimum offset found in first pass */
                /* and add in the current vertical offset */
                setOffset('Y',numstr[0],
                          num-miny + (int)PIXROUND(v,vconv) + y_goffset);
              } else {
                setOffset('Y',numstr[0], num);
              }
            }
          }
          break;

        case(0x74):
          /* Set the Raster resolution */
          EMIT1("\033*t");
          for (t = (char)b_read(spfp); t != 0x52; t = (char)b_read(spfp))
            EMITC(t);
          EMITC(t);
          break;

        case(0x72):
          /* Raster Graphics commands such as start */
          EMIT1("\033*r");
          for (t = (char)b_read(spfp); t< 0x40 || t>0x60; t = (char)b_read(spfp))
            EMITC(t);
          EMITC(t);
          break;

        case(0x62):
          /* Transfer the correct number of bytes of raster graphics */
          EMIT1("\033*b");
          num = 0;
          count = 0;
          for (t = (char)b_read(spfp); t<0x40 || t>=0x60; t = (char)b_read(spfp))
            numstr[count++] = t;
          numstr[count] = 0;
          if (t==0x4D) {
            for (t = numstr[count = 0]; t!=0; t = numstr[++count])
              EMITC(t);
            EMIT1("M");
          }
          if (t==0x57) {
            for(t = numstr[count = 0]; t!=0; t = numstr[++count])
              EMITC(t);
            EMIT1("W");
            for (count = atoi(numstr); count>0; count--) {
              t = (char)b_read(spfp);
              EMITC(t);
            }
          }
          break;

        case(0x63):
          /* Rectangular draw commands */
          EMIT1("\033*c");
          for (t = (char)b_read(spfp); t<0x40 || t>0x60;
               t = (char)b_read(spfp))
            EMITC(t);
          EMITC(t);
          break;

        default:
          break;
        }
      }
    }
  }
  while ((binumber == BUFFSIZE) || (biact < binumber));

  BCLOSE(spfp);
  qfprintf(ERR_STREAM,"]");
}

/* This function closes all open files */
void
CloseFiles(void)
{
  struct font_entry *fe;
  FILEPTR f;

  /* First input/output files */
  if (outfp != FPNULL) {
#ifdef RISC_BUFFER
    b_oflush(outfp);
#endif
    BCLOSE(outfp);
  }
  if (dvifp != FPNULL) {
    BCLOSE(dvifp);
  }
#ifdef __riscos
  if (metafile != FPNULL) {
    BCLOSE(metafile);
  }
#endif
  /* Now all open font files */
  fe = hfontptr;
  while (fe != NULL) {
    f = fe->font_file_id;
    if ((f != FPNULL) && (f != NO_FILE)) {
      BCLOSE(f);
    }
    fe = fe->next;
  }
}

/*-->NoSignExtend*/
/**********************************************************************/
/***************************  NoSignExtend  ***************************/
/**********************************************************************/
long4
NoSignExtend(FILEPTR fp, register int n)
{
  long4 x = 0;      /* number being constructed */
  unsigned char h;
  while (n--) {
    x <<= 8;
    read_byte(fp,h);
    x |= h;
  }
  /* fprintf(stderr,"[%ld] ",(long)x);*/
  return(x);
}


#ifndef ARITHMETIC_RIGHT_SHIFT
long4   signTab[5] = {0,0x00000080,0x00008000,0x00800000,0x00000000};
long4 extendTab[5] = {0,~0^0xff,~0^0xffff,~0^0xffffff,~0^0xffffffff};
#endif

/*-->SignExtend*/
/**********************************************************************/
/****************************  SignExtend  ****************************/
/**********************************************************************/
long4
SignExtend(FILEPTR fp, register int n)
{
  int     n1;       /* number of bytes      */
  long4   x;        /* number being constructed */
  unsigned char    h;
#ifdef SIGN_DEBUG
  long4    x0;      /* copy of x  */
#endif
  read_byte(fp,h);
  x = h; /* get first (high-order) byte */
  n1 = n--;
  while (n--) {
    x <<= 8;
    read_byte(fp,h);
    x |= h;
  }
  /*
   *   NOTE: This code assumes that the right-shift is an arithmetic, rather
   *   than logical, shift which will propagate the sign bit right.   According
   *   to Kernighan and Ritchie, this is compiler dependent!
   */

#ifdef SIGN_DEBUG
  x0 = x;
#endif

#ifdef ARITHMETIC_RIGHT_SHIFT
  x <<= 32 - 8 * n1;
  x >>= 32 - 8 * n1; /* sign extend */
#else
  if (x & signTab[n1]) x |= extendTab[n1];
#endif

#ifdef SIGN_DEBUG
  fprintf(ERR_STREAM,"\tSignExtend(fp,%d)=%lX, was=%lX,%d\n",
          n1,x,x0,x0&signTab[n1]);
#endif

#ifdef DEBUG
  if (Debug > 1)
    fprintf(ERR_STREAM,"\tSignExtend(fp,%d)=%lX\n", n1, x);
#endif
  return(x);
}


#ifdef IBM3812
/*-->PMPout*/
/*****************************************************************************/
/* This routine produces the PMP-envelopes for the 3812. Its semantics are:

   first arg == 0  ... flush buffer
   first arg == -1 ... number of bytes specified in the second argument
               have to be continuous, that is they must not
               be disrupted by ENTER PMP etc.
   first arg > 0       output first arg bytes

               If arg2 > OUTBUFSIZE ... flush buffer,
                        switch to unbuffered mode
                        (dont't collect PMP commands)
               If arg2+bufferpointer > OUTBUFSIZE ... flush buffer,
                        block will fit into buffer
               otherwise ..... block will fit into buffer

  Buffering is done to reduce the ENTER PMP-commands. Initially
  the 3812 is in PC-ASCII mode. In order to issue a PMP-command it is
  necessary to enter PMP mode. The ENTER-PMP-command contains the
  number of bytes that will be interpreted as PMP-commands. In the
  most naive approach for each primitive command (eg. set cursor) you
  have to produce a seperate ENTER-PMP-envelope (5 bytes). It is
  favourable to collect as many PMP commands as possible in one envelope. */
/*****************************************************************************/
void
PMPout(int l, char *s)
{
  static char           buffer[OUTBUFSIZE];
  static unsigned short bp = 0;         /* range 0..OUTBUFSIZE */
  static long4          continuous = 0l;
  static bool           buffered = _TRUE;

  if (l == 0) {
    if (bp == 0)
      return;
    EMIT3("\033[C%c%c", (unsigned char)(bp & 0xFF), (unsigned char)(bp >> 8));
    EMITB((int)bp, buffer);
    bp = 0;
    return;
  }
  if (l == -1) {
    continuous = (long4)s;
    if (continuous + (long4)bp + 5l > (long4)OUTBUFSIZE)
      PMPflush;
    buffered = (bool)((continuous + 5l <= (long4)OUTBUFSIZE));
    if (!buffered) {
      EMIT3("\033[C%c%c", (unsigned char)(continuous & 0xFF),
            (unsigned char)((continuous >> 8) & 0xFF));
    }
    return;
  }
  if (buffered) {
    register int    i;
    if ( ((long4)l + bp) > OUTBUFSIZE)
      PMPflush;
    for (i = 0; i < l; i++)
      buffer[bp+i] = s[i];
    bp += (unsigned short)l;
  } else {
    EMITB((int)l, s);
    buffered = (bool)((continuous -= (long4)l) <= 0);
  }
}


void
PMPoutC(char c)
{
  PMPout(1, &c);
}
#endif /* IBM3812 */


#ifdef MSC5
/*-->AssureBinary*/
/**********************************************************************/
/*************************** AssureBinary *****************************/
/**********************************************************************/
/* This procedure is both DOS AND MSC dependent. The MSC file open on */
/* a device ignores the 'binary' of the "wb" parameter and opens the  */
/* file in ascii mode. This procedure sets the file f to binary mode  */
/* if it is connected to a device that is not console input or output */
/* or the null device. For other operating systems this routine is    */
/* useless. (Background: MSDOS 3.2 Technical Reference upd 1 pg 6-137 */
/**********************************************************************/
void
AssureBinary(FILEPTR f)
{
  union REGS regs;                      /* registers for bios call */

  regs.h.ah = (unsigned char) 0x44;     /* IOCTL                   */
  regs.h.al = (unsigned char) 0x00;     /* get device information  */
  regs.x.bx = (unsigned int) fileno(f); /* handle from MSC         */
  intdos(&regs, &regs);                 /* call DOS interrupt      */
                                        /* ---> result in DX       */

  if (  (regs.h.dl & 0x80)              /* file handle points to a device */
        && !(regs.h.dl & 0x07) ) {      /* neither console i/o or null    */

    regs.h.dl  |= 0x20;                 /* set BINARY bit in device info  */
    regs.h.ah = (unsigned char) 0x44;   /* IOCTL                  */
    regs.h.al = (unsigned char) 0x01;   /* set device information */
    regs.x.bx = (unsigned int) fileno(f); /* handle from MSC      */
    regs.h.dh = (unsigned char) 0x00;   /* clear DH               */
    intdos(&regs, &regs);               /* call DOS interrupt     */
  }
}
#endif /* MSC5 */

/*------------------------ end dviIO.c ----------------------------------*/

/*------------------------ begin dvichars.c -----------------------------*/
/*-->EmitChar*/
/**********************************************************************/
/****************************  EmitChar  ******************************/
/**********************************************************************/
void                     /* output a character bitmap */
EmitChar(long4 c, struct char_entry *ce)
{
  register int i;
  register unsigned char  *sl;
  unsigned short nbpl, nwpl;
  long     total;
#ifdef LJ
  unsigned char cnv_buffer[10];
#endif


#ifdef LJ
/*
printf("Emit character %c(%d) id=%d, yoff=%d[%d], w=%d[%d], h=%d[%d]\n",
        (char)c,(int)c,
    fontptr->plusid,
    ce->yOffset, fontptr->max_yoff,
    ce->width,   fontptr->max_width,
    ce->height,  fontptr->max_height
);
*/
#endif



  if ( fontptr->ncdl == 0 ) {
#ifdef IBM3812
    used_fontstorage += 1028;
#endif
#ifdef LJ
    if (fontptr->max_width == 0) { /* we have no realistic values */
      fontptr->max_yoff = CHAR_HEIGTH_LARGE;
      fontptr->max_width = CHAR_WIDTH_LARGE;
      fontptr->max_height = CHAR_HEIGTH_LARGE*2;
    }

    /* open font dict before first char, set active font */
    INT_ASCII(cnv_buffer,fontptr->plusid);
# ifdef LJ4
    EMIT2("\033*c%sD\033)s68W", cnv_buffer);
    EMITB(6, "\0\104\024\2\0\0");
# else
    EMIT2("\033*c%sD\033)s64W", cnv_buffer); /* changed 26 to 64 (khk) */
    EMITWORD( 64 );                     /* Font descriptor size */
    EMITC((char) 0 );                   /* res0 (byte) */
#  ifdef SEVENBIT
    EMITC((char) 1);                    /* Font Type */
#  else
    EMITC((char) 2);                    /* Font Type */
#  endif /* SEVENBIT */
    EMITWORD( 0 );                      /* res1 */
# endif /* LJ 4 */
    EMITWORD( fontptr->max_yoff);       /* Baseline Distance */
    EMITWORD( fontptr->max_width);      /* Cell Width */
    EMITWORD( fontptr->max_height);     /* Cell Height */
    EMITC((char) 0);            /* Orientation */
    EMITC((char) 1);            /* Spacing */
    EMITWORD( 277 );            /* Symbol Set */
    EMITWORD( 1024 );           /* Pitch */
    EMITWORD( 1024 );           /* Height */
    EMITWORD( 0 );              /* xHeight */
    EMITC((char) 0);            /* Width Type */
    EMITC((char) 0);            /* Style */
    EMITC((char) 0);            /* Stroke Weight */
    EMITC((char) 0);            /* Typeface */
    EMITC((char) 0);            /* res2 (byte) */
    EMITC((char) 0);            /* Serif Style */
    EMITWORD( 0 );              /* res 3 */
    EMITC((char) 0);            /* Underline Distance */
    EMITC((char) 0);            /* Underline Height */
    EMITWORD( 0 );              /* Text Height */
    EMITWORD( 0 );              /* Text Width */
    EMITWORD( 0 );              /* res4 */
    EMITWORD( 0 );              /* res5 */
    EMITC((char) 0);            /* Pitch Extended */
    EMITC((char) 0);            /* Height Extended */
    EMITWORD( 0 );              /* res6 */
    EMITWORD( 0 );              /* res7 */
    EMITWORD( 0 );              /* res8 */
    EMITB(16,"                ");
# ifdef LJ4
    EMITB(4 ,"\2\130\2\130");
# endif
    EMIT1("\033*c4F");
#endif /* LJ */
    }
  if ( fontptr != prevfont ) {   /* because this isn't done on pass 0 */
#ifdef LJ
    INT_ASCII(cnv_buffer,fontptr->plusid);
    EMIT2("\033(%sX", cnv_buffer);
#endif
    prevfont = fontptr;
  }

#ifdef USEPXL
  if (fontptr->id == pk89) {
    nwpl = 0; /* initialize variable */
    nbpl = ((int)(ce->width) +  7) >> 3;
    total = (long)ce->height * nbpl;
  } else if (fontptr->id == id1002) {
    nwpl = 0; /* initialize variable */
    nbpl = ((int)(ce->width) +  7) >> 3;
    total = (long)ce->height * nbpl;
  } else if (fontptr->id == id1001) {
    nwpl = ((int)(ce->width) + 31) >> 5;
    nbpl = ((int)(ce->width) + 7) >> 3;
    total = (long)ce->height * nbpl;
  } else {
    /* should never be necessary */
    nwpl = 0;
    nbpl = 0;
    total = 0;
  }
#else
  nbpl = (num_cols + 7) >> 3;
  total = num_rows * nbpl;
#endif
/***************************************************************
    if ( ((char) c == 'i') && (fontptr->plusid == 0)) {
        long j;
        fprintf(ERR_STREAM, "cols=%ld, ncols=%ld\n",(long)nwpl,(long)nbpl);

        fprintf(ERR_STREAM, "%ld Bytes:->",(long)total);
        for (j=0; j<total; j++) {
            char *ch; char ic;
            ch = (char *)(ce->where.address.pixptr);
            ic = *(ch+j);
            fprintf(ERR_STREAM,"%X.",ic);
                }
        fprintf(ERR_STREAM,"<- Now Emitting\n");
        }
***************************************************************/
#ifdef USEPXL
#ifdef IBM3812
  if ((short)(ce->height) - ce->yOffset > 55) {
    ce->yyOffset = (short) ce->height - ce->yOffset;
    ce->yOffset  = (short) ce->height;
  } else {
    ce->yyOffset = (short) 0;
  }
#endif
#ifdef LJ
  ce->yyOffset = (short) 0;
#endif
#endif

  /* ce->cw = (long4)(((double)ce->tfmw / (double)hconv) +0.5); */
  /* set active font to nn, load font pattern  xx ... */
#ifdef IBM3812
  PMPcont(total + 9);
# ifdef USEPXL
  sprintf(PMPformat, "\323%c\360%c%c%c",
          (unsigned char)fontptr->plusid,
          (unsigned char)VisChar((char)c),
          (unsigned char)ce->height,
          (unsigned char)ce->width);
  PMPout(6, PMPformat);
  PMPoutC((char)(-(ce->xOffset)));
  PMPoutC((char)(ce->cw - (-ce->xOffset + ce->width)));
  PMPoutC((char)(ce->yOffset));
# else
  sprintf(PMPformat, "\323%c\360%c%c%c",
          (unsigned char)fontptr->plusid,
          (unsigned char)VisChar((char)c),
          (unsigned char)num_rows,
          (unsigned char)num_cols);
  PMPout(6, PMPformat);
  PMPoutC((char)(-x_offset));
  PMPoutC((char)(ce->cw - (-x_offset + num_cols)));
  PMPoutC((char)num_rows-y_offset);
# endif
#endif

#ifdef LJ
# ifdef LJ4
  if (CompressFontMode) { /* Should we use compressed font downloading? */
    /* For COMPRESSED CHARS */
    if (PrevSize < nbpl) {
      PrevSize = nbpl;
      if (PrevLine != NULL)
        free(PrevLine);
      if ((PrevLine = (unsigned char*)malloc(PrevSize*sizeof(char)))
          == NULL) {
        PrevSize = 0;
        Fatal("EmitChar: Out of memory error!\n");
      }
    }
    /* Clear seed line */
    for (i = 0; i<nbpl; i++)
      PrevLine[i] = 0;
    CChar_Off = 0;
    CChar_Last = -1;

    /* This bit copied from below... */
#  ifdef USEPXL
    if (fontptr->id == pk89)
      PkRaster(ce,2);
    else if (fontptr->id == id1002)
      for (i = 0; i < (int)ce->height; i++) {
        sl = ((unsigned char *)(ce->where.address.pixptr) + i * nbpl);
        CompressedCharLine(ce,nbpl,sl);
      }
    else if (fontptr->id == id1001)
      for (i = 0; i < (int)ce->height; i++) {
        sl = (unsigned char *)(ce->where.address.pixptr + i * nwpl);
        CompressedCharLine(ce,nbpl,sl);
      }
#  else
    for (i = num_rows; i > 0; i--)
      CompressedCharLine(ce,nbpl,(bits + (i-1)*nbpl));
#  endif /* USEPXL */
  }
  else
    CChar_Off = -1;
# endif /* LJ4 */

  INT_ASCII(cnv_buffer,fontptr->plusid);
  /* Depending on whether character could be packed or not the header looks
     different! */
# ifdef LJ4
  /* printf("raw: %d (%d * %d), comp: %d\n",
     total,ce->width,ce->height,CChar_Off); */
  /* Characters that don't compress well are usually rather small so
     reading them again and writing them uncompressed won't take
     much time anyway. */
  if (CChar_Off > total)
    CChar_Off = -1;
  if (CChar_Off >= 0) {
    EMIT4("\033*c%sd%dE\033(s%ldW", cnv_buffer,
          (unsigned int)VisChar((char)c), (long)(CChar_Off + 16));
    EMITB(4, "\4\0\016\2");
  }
  else
# endif
    {
      EMIT4("\033*c%sd%dE\033(s%ldW", cnv_buffer,
            (unsigned int)VisChar((char)c), total + 16);
      EMITB(4, "\4\0\016\1");
    }
  /*    EMITC((char)(Landscape==_TRUE)); */
  EMITC((char)0);
  EMITC((char)0);
#ifdef USEPXL
  EMITWORD(-ce->xOffset);
  EMITWORD(ce->yOffset);
  EMITWORD(ce->width);
  EMITWORD(ce->height);
#else
  EMITWORD(-x_offset);
  EMITWORD(num_rows-y_offset);
  EMITWORD(num_cols);
  EMITWORD(num_rows);
#endif
  EMITWORD((int)ce->cw * 4);
#endif

#ifdef LJ4
  /* Now output compressed or uncompressed */
  if (CChar_Off >= 0) {
    EMITL(CChar_Off,buffin); /* Could compress character into buffer */
  }
  else /* Couldn't compress character - output literally */
#endif

    {
#ifdef USEPXL
      if (fontptr->id == pk89)
        PkRaster(ce, 0);
      else if (fontptr->id == id1002)
        for (i = 0; i < (int)ce->height; i++) {
          sl = ((unsigned char *)(ce->where.address.pixptr) + i * nbpl);
          EMITL(nbpl, (char *)sl);
        }
      else if (fontptr->id == id1001)
        for (i = 0; i < (int)ce->height; i++) {
          sl = (unsigned char *)(ce->where.address.pixptr + i * nwpl);
          EMITL(nbpl, (char *)sl);
        }
#else
      for (i = num_rows; i > 0; i--) {
        EMITL (nbpl, bits + (i-1) * nbpl);
      }
#endif
    }

#ifdef IBM3812
#ifdef USEPXL
  used_fontstorage += (long4)ce->height * ((ce->width + 15) >> 4) * 2 + 14;
#else
  used_fontstorage += (long4)num_rows * ((num_cols + 15) >> 4) * 2 + 14;
#endif
#endif
#ifdef LJ
#ifdef USEPXL
  used_fontstorage += 64 * (((int)((ce->height * ce->width) - 1) / 64) + 1);
#else
  used_fontstorage += 64 * ((((num_rows * num_cols) - 1) / 64) + 1);
#endif
#endif
  fontptr->ncdl += 1;
  G_ncdl += 1;
}

/*-->PkRaster*/
/*********************************************************************/
/**************************** PkRaster *******************************/
/*********************************************************************/

#ifdef USEPXL
static long4 pk_packed_num(void);

#define  PKBYTE   *pkloc; pkloc ++
#define  OUTCHAR(c) raster_line_buf[bp]= (unsigned char)c; bp++

unsigned char   bitweight, inputbyte;
unsigned char   dyn_f;
unsigned char   *pkloc;
int     repeatcount;

void             /* <Read and translate raster description@>*/
PkRaster(struct char_entry *ce, int raster)
{
  int     rp;
  int     current_line;
  int     wordwidth;
  bool turnon;
  unsigned short  nbpl;
  long4    rowsleft, word, wordweight, hbit, count, i, j, tl;
  long4    row[101];
  unsigned char   raster_line_buf[BYTES_PER_PIXEL_LINE];
  unsigned short  bp;


  if (ce->charsize == HUGE_SIZE)
    Fatal( "cannot process currently PK font patterns of that size!\n");


  current_line = 0;
  pkloc = (unsigned char *)ce->where.address.pixptr;
  dyn_f = (unsigned char)(ce->flag_byte >> 4);
  turnon = (bool)((ce->flag_byte & 8) == 8);
  wordwidth = (int)(ce->width + 31) >> 5;
  nbpl = ((int)(ce->width) +  7) >> 3;

  bitweight = 0;
  if (dyn_f == 14) {
    /*printf("<Get raster by bits@>\n");*/
    for (i = 1; i <= (long4)ce->height; i++) {
      word = 0;
      wordweight = 31;
      bp = 0;            /* Sowa */

#ifdef DRAWGLYPH
      printf("     |");
#endif
      for (j = 1; j <= (long4)ce->width; j++) {
        bool getbit;
        /* bp = 0;               SOWA */
        /*******************************************begin Getbit *********/
        bitweight /= 2;
        if ( bitweight == 0 ) {
          inputbyte = PKBYTE;
          bitweight = 128;
        }
        getbit = (bool)
          ( inputbyte >= bitweight );
        if ( getbit )
          inputbyte -= bitweight;
        /*********************************************end Getbit *********/

        if (getbit)
          word += power[wordweight];

        wordweight --;
        if (wordweight == -1) {

#ifdef DRAWGLYPH
          { int k;
          for (k = 31; k>=0; k--) {
            if ((power[k] & word)!=0) printf("M");
            else printf(".");
          }
          }
#endif

          OUTCHAR((word >> 24 & 0xFF));
          OUTCHAR((word >> 16 & 0xFF));
          OUTCHAR((word >> 8 & 0xFF));
          OUTCHAR((word    & 0xFF));

          word = 0;
          wordweight = 31;
        }
      }
      if (wordweight < 31) {
#ifdef COMMENT
# ifdef DRAWGLYPH
        {
          int k;
          for (k = 15; k>=0; k--) {
            if ((power[k] & word)!=0) printf("Z");
            else printf(":");
          }
        }
        printf("|\n ----|");
# endif
#endif

        for (j = 3; j >= (wordwidth * 4 - (long4)nbpl); j--) {
          OUTCHAR(((word >> (j << 3)) & 0xff));
#ifdef DRAWGLYPH
          {
            int k;
            for (k = 7; k>=0; k--) {
              if ((power[k] & ((word >> (j << 3)) & 0xff))!=0) printf("M");
              else printf(".");
            }
          }
#endif
        }

      }

      switch (raster) {
      case 1:
        RasterLine(ce, nbpl, current_line, raster_line_buf);
        current_line++;
        break;
#ifdef LJ4
      case 2:
        CompressedCharLine(ce,(unsigned int)nbpl,raster_line_buf);
        break;
#endif
      default:
        EMITL(bp, (char *)raster_line_buf);
      }

#ifdef DRAWGLYPH
      printf("|\n");
#endif
    }
  } else {
    /* fprintf(ERR_STREAM, "@<Create normally packed raster@>\n"); */
    rowsleft = (long4)ce->height;
    hbit = (long4)ce->width;
    repeatcount = 0;
    wordweight = 32;
    word = 0;
    rp = 1;
    while ( rowsleft > 0 ) {
      count = pk_packed_num();
      bp = 0;

      while (count > 0) {
        if ((count < wordweight) && (count < hbit)) {
          if (turnon)
            word +=
              gpower[wordweight] -
              gpower[wordweight - count];

          hbit -= count;
          wordweight -= count;
          count = 0;
        } else if ((count >= hbit) && (hbit <=
                                       wordweight)) {

          if (turnon)
            word +=
              gpower[wordweight] -
              gpower[wordweight - hbit];

          row[rp] = word;

          /*fprintf(ERR_STREAM, " @<Send row@> \n");*/
          for (i = 0; i <= (long4)repeatcount; i++) { int ii;

#ifdef DRAWGLYPH
          printf("***  |");
#endif
          for (ii = 1; ii < wordwidth; ii++) {
            tl = row[ii];

            OUTCHAR((tl >> 24 & 0xFF));
            OUTCHAR((tl >> 16 & 0xFF));
            OUTCHAR((tl >> 8  & 0xFF));
            OUTCHAR((tl       & 0xFF));

#ifdef DRAWGLYPH
            {
              int k;
              for (k = 31; k>=0; k--) {
                if ((power[k] & row[ii])!=0) printf("M");
                else printf(".");
              }
            }
#endif
          }
          tl = row[wordwidth];
          for (j = 3; j >= (wordwidth *4 - (long4)nbpl); j--) {
            OUTCHAR(((tl >> (j << 3)) & 0xff));
#ifdef DRAWGLYPH
            {
              int k;
              for (k = 7; k>=0; k--) {
                if ((power[k] & ((tl >> (j << 3)) & 0xff))!=0) printf("M");
                else printf(".");
              }
            }
#endif
          }

          switch (raster) {
          case 1:
            RasterLine(ce, (unsigned int)nbpl, current_line, raster_line_buf);
            current_line++;
            break;
#ifdef LJ4
          case 2:
            CompressedCharLine(ce,(unsigned int)nbpl,raster_line_buf);
            break;
#endif
          default:
            EMITL(bp, (char *)raster_line_buf);
          }

          bp = 0;

#ifdef DRAWGLYPH
          printf("|  ");
          for (j = 1; j <= (long4)wordwidth; j++) printf("%02lX/",row[j]);
          printf(" raster=%d\n",raster);
#endif
          }

          rowsleft -=  (long4)repeatcount + 1;
          repeatcount = 0;
          rp = 1;
          word = 0;
          wordweight = 32;
          count -= hbit;
          hbit = (long4)ce->width;
        } else {
          if (turnon) word += gpower[wordweight];
          row[rp] = word;
          rp = rp + 1;
          word = 0;
          count -= wordweight;
          hbit -= wordweight;
          wordweight = 32;
        }
      }   /* .....while count > 0 */
      if (turnon)
        turnon = _FALSE;
      else
        turnon = _TRUE;
    } /* ...... rowsleft > 0 */
    if ((rowsleft != 0) || (hbit != (long4)ce->width))
      Fatal("Bad pk file----more bits than required!\n");
  } /* .... create normally packed raster */
}

static unsigned char
getnyb(void)
{
  register unsigned char  temp;
  if ( bitweight == 0 ) {
    inputbyte = PKBYTE;
    bitweight = 16;
  }
  temp = inputbyte / bitweight;
  inputbyte -= temp * bitweight;
  bitweight /= 16;
  return ( temp );
}


static long4
pk_packed_num(void)
{ /*@<Packed number procedure@>= */
  register int    i;
  long4    j;

  i = (int)getnyb();
  if (i == 0) {
    do {
      j = (long4)getnyb();
      i++;
    } while (j == 0);
    while (i > 0) {
      j = j * 16 + (long4)getnyb();
      i--;
    };
    return (j - 15 + (13 - dyn_f) * 16 + dyn_f);
  } else if (i <= (int)dyn_f) {
    return ((long4)i);
  } else if (i < 14) {
    return ((i-(long4)dyn_f - 1) * 16 + (long4)getnyb() + dyn_f + 1);
  } else {
    if (i == 14) {
      repeatcount = (int)pk_packed_num();
    } else {
      repeatcount = 1;
    }
    /*     fprintf(ERR_STREAM,"repeatcount = [%d]\n",repeatcount);    */
    return (pk_packed_num());    /* tail end recursion !! */
  }
}
#endif

#ifndef USEPXL
void
bad_gf(int n)
{
  Fatal("Bad gf file, case %d\n",n);      /* See gf.c */
}
#endif


#ifdef LJ4
/* Compress a raster line in compression mode 2 */
int
CompressLine2(unsigned char *buffer, unsigned char *buffout, int emitbytes)
{
  unsigned char *pos,*ppos,*litpos,*upper;
  int lit,i,pack;
  char last,c;

  /* trap empty lines (otherwise problems with Packbits) */
  if (emitbytes == 0)
    return(0);
  /* Use Packbits compression (mode 2) on raster line */
  pos = buffer;
  litpos = buffer;
  pack = 1;
  ppos = buffout;
  last = *pos++;
  upper = buffer + emitbytes;
  while (pos < upper) {
    c = *pos++;
    if (c == last) {
      pack++;
      /* At least three equal bytes ==> write out preceding literal sequence */
      if ((pack == 3) && ((lit = (int)(pos - litpos - 3)) != 0)) {
        while (lit >= 128) {
          *ppos++ = 127;
          for (i = 1; i<=128; i++)
            *ppos++ = *litpos++;
          lit -= 128;
        }
        if (lit != 0) {
          *ppos++ = lit-1;
          while ((lit--) > 0)
            *ppos++ = *litpos++;
          /* litpos is now pos-3 (i.e. first of the sequence) */
        }
      }
    }
    else {
      while (pack >= 128) {
        *ppos++ = 129;
        *ppos++ = last;
        litpos += 128;
        pack -= 128;
      }
      /* At least 3 equal bytes or 2 that don't break a literal sequence
         ==> write out packed sequence */
      if ((pack >= 3) || ((pack == 2) && (litpos == pos-3))) {
        *ppos++ = 257 - pack;
        *ppos++ = last;
        litpos += pack;
      }
      pack = 1;
      last = c;
    }
    if (ppos > (buffout + BUFFSIZE/2 - 129)) {
      fprintf(ERR_STREAM,
              "Can't compact raster character; rerun without compression!\n");
      return(0);
    }
  }

  while (pack >= 128) {
    *ppos++ = 129;
    *ppos++ = last;
    litpos += 128;
    pack -= 128;
  }
  if ((pack >= 3) || ((pack == 2) && (litpos == pos-3))) {
    *ppos++ = 257 - pack;
    *ppos++ = last;
    litpos += pack;
  }
  else if ((lit = (int)(pos - litpos)) != 0) {
    while (lit >= 128) {
      *ppos++ = 127;
      for (i = 1; i<=128; i++)
        *ppos++ = *litpos++;
      lit -= 128;
    }
    if (lit != 0) {
      *ppos++ = lit-1;
      while ((lit--) > 0)
        *ppos++ = *litpos++;
    }
  }
  return((int)(ppos - buffout));
}


/* Compress a raster line in compression mode 3 */
int CompressLine3(unsigned char *buffer, unsigned char *buffout, int emitbytes)
{
  unsigned char *pos,*ppos,*litpos,*upper,*prev;
  int lit,i,pack;

  /* Use compression mode 3 */
  pos = buffer;
  ppos = buffout;
  upper = buffer + emitbytes;
  prev = PrevLine;
  while(pos < upper) {
    litpos = pos;
    while ((*prev == *pos) && (pos < upper)) {
      prev++;
      pos++;
    }
    if (pos < upper) {
      pack = (int)(pos - litpos);
      litpos = pos;
      i = upper - pos;
      if (i > 8)
        i = 8;
      while ((*pos != *prev) && (i > 0)) {
        *prev++ = *pos++;
        i--;
      }
      i = (int)(pos - litpos - 1);
      lit = i<<5;
      if (pack < 31) {
        *ppos++ = lit + pack;
      }
      else {
        *ppos++ = lit + 31;
        pack -= 31;
        while (pack >= 255) {
          *ppos++ = 255;
          pack -= 255;
        }
        *ppos++ = pack;
      }
      while (i >= 0) {
        *ppos++ = *litpos++;
        i--;
      }
    }
    if (ppos > (buffout + BUFFSIZE/2 - 16)) {
      fprintf(ERR_STREAM,
              "Can't compact raster character; rerun without compression!\n");
      return(0);
    }
  }
  return((int)(ppos - buffout));
}
#endif

/*-->RasterLine*/
/**********************************************************************/
/****************************  RasterLine  ****************************/
/**********************************************************************/
void
RasterLine(struct char_entry *ce, unsigned int nbpl,
           unsigned int current_line, unsigned char *buffer)
{
#ifdef IBM3812
  long   total;
  static unsigned short   maxlines;

  if (current_line == 0) {
# ifdef USEPXL
    maxlines = ce->height;

    MoveVert(-ce->yOffset);      /* move cursor up */
    MoveHor(-ce->xOffset);       /* move cursor right */
# else
    maxlines = num_rows;

    MoveVert(- (num_rows-y_offset) );      /* move cursor up */
    MoveHor(-x_offset);       /* move cursor right */
# endif
    last_rx = last_ry = UNKNOWN;       /* next time full positioning */
  }

  if (current_line % maxlines == 0) {
    if (current_line > 0) {    /* maxlines lines have been printed*/
      MoveVert(maxlines);   /*   move cursor down     */
      last_rx = last_ry = UNKNOWN;    /* next time full positioning */
    }
# ifdef USEPXL
    total = (long)(ce->height - current_line) * (long4)nbpl;
# else
    total = (long)(num_rows - current_line) * (long4)nbpl;
# endif
    if ((total + 9) > 65535) {
      maxlines = (unsigned short)((65535 - 9) / nbpl);
      total = (long)maxlines * (long)nbpl;
    }

    PMPcont(total + 9);
    PMPout(2, "\365\0");
    EMITWORD(maxlines);
# ifdef USEPXL
    EMITWORD(ce->width);
# else
    EMITWORD(num_cols);
# endif
    PMPoutC((unsigned char) (total >> 16) & 0xFF);
    PMPoutC((unsigned char) (total >> 8 ) & 0xFF);
    PMPoutC((unsigned char)  total     & 0xFF);
  }
  EMITL((int)nbpl, (char *)buffer);
#endif /* IBM3812 */


#ifdef LJ
  register int emitbytes;
#ifdef LJ4
  int i;
#endif

  for (emitbytes = (int)nbpl;
       (*(buffer + emitbytes - 1) == '\0') && (emitbytes > 0);
       emitbytes--);
# ifdef LJ4
  switch (CompressCharMode) {
  case 0:
    EMIT2("\033*b%dW", emitbytes);
    EMITL(emitbytes, buffer);
    break;

  case 2:
    i = CompressLine2(buffer,buffin,emitbytes);
    EMIT2("\033*b%dW", i);
    EMITL(i,buffin);
    break;

  case 3:
    i = CompressLine3(buffer,buffin + BUFFSIZE/2,(int)nbpl);
    EMIT2("\033*b%dW", i);
    EMITL(i,buffin + BUFFSIZE/2);
    break;

  default:
    fprintf(ERR_STREAM,"Unsupported compression mode!\n");
  }
# else
  EMIT2("\033*b%dW", emitbytes);
  EMITL(emitbytes, buffer);
# endif /* LJ4 */
#endif /* LJ */
}


/*-->RasterChar*/
/**********************************************************************/
/****************************  RasterChar  ****************************/
/**********************************************************************/
void                     /* raster a character bitmap */
RasterChar(struct char_entry *ce)
{
  int      i;
  register unsigned char  *sl;
  unsigned short  nbpl, nwpl;
  unsigned char   raster_line_buf[BYTES_PER_PIXEL_LINE];

#ifdef DEBUG
  if (Debug)
    fprintf(ERR_STREAM,"Raster character ...size=%d \n", (int)ce->charsize);
#endif

#ifdef USEPXL
  if (fontptr->id == pk89) {
    nwpl = 0; /* initialize variable */
    nbpl = ((int)(ce->width) +  7) >> 3;
  } else if (fontptr->id == id1002) {
    nwpl = 0; /* initialize variable */
    nbpl = ((int)ce->width +  7) >> 3;
  } else if (fontptr->id == id1001) {
    nwpl = ((int)ce->width + 31) >> 5;
    nbpl = ((int)ce->width + 7) >> 3;
  } else {
    /* should never be necessary */
    nwpl = 0;
    nbpl = 0;
  }
#else
  nbpl = (num_cols + 7) >> 3;
#endif

#ifdef LJ
# ifdef LJ4
  CompressCharMode = (ce->width < CompressCharWidth) ? 0 : UseCompression;
  EMIT3("\033*t%dR\033*r1A\033*b%dM",RESOLUTION,CompressCharMode);
  if (CompressCharMode == 3) {
    /* Check if PrevLine big enough; claim more space if not */
    if (((int)nbpl) > PrevSize) {
      if (PrevLine != NULL)
        free(PrevLine);
      if ((PrevLine = (unsigned char *)malloc(nbpl*sizeof(char))) == NULL) {
        PrevSize = 0;
        Fatal("RasterChar: out of memory!\n");
      }
      PrevSize = (int)nbpl;
    }
    /* Clear previous line */
    sl = PrevLine;
    for (i = 0; i<nbpl; i++)
      *sl++ = 0;
  }
# else
  EMIT2("\033*t%dR\033*r1A\033*b0M",RESOLUTION);
# endif /* LJ4 */
#endif
  { /* read pixel from file */
    if ((ce->charsize == HUGE_SIZE) && (fontptr->id != pk89))
      OpenFontFile();
#ifdef USEPXL
    FSEEK(pxlfp, ce->where.address.fileOffset, SEEK_SET);
#else
    FSEEK(gfin, ce->where.address.fileOffset, SEEK_SET);
    gettochar();
    readbits();
#endif /* USEPXL */
  }

#ifdef USEPXL
  if (fontptr->id == pk89)
    PkRaster(ce, 1);
  else if (fontptr->id == id1002) {
    for (i = 0; i < (int)ce->height; i++) {
      if (ce->charsize == HUGE_SIZE) {
        fread(raster_line_buf, 1, (int)nbpl, pxlfp);
        sl = raster_line_buf;
      } else
        sl = ((unsigned char *)(ce->where.address.pixptr)
              + i * nbpl);
      RasterLine(ce, (unsigned int)nbpl, i, sl);
    }
  } else if (fontptr->id == id1001) {
    long filediff;
    filediff = (long)nwpl * 4 - nbpl;
    for (i = 0; i < (int)ce->height; i++) {
      if (ce->charsize == HUGE_SIZE) {
        read_multi(raster_line_buf, 1, (int)nbpl, pxlfp);
        /* skip fill bytes */
        FSEEK(pxlfp, filediff, SEEK_CUR);
        sl = raster_line_buf;
      } else
        sl = (unsigned char *)(ce->where.address.pixptr + i * nwpl);
      RasterLine(ce, (unsigned int)nbpl, i, sl);
    }
  }
#else
  for (i = num_rows; i > 0; i--)
    RasterLine(ce, (unsigned int)nbpl, i, bits + (i-1) * nbpl);
#endif
#ifdef LJ
  EMIT1("\033*rB");
#endif
  last_rx = last_ry = UNKNOWN;
}

/*-->LoadAChar*/
/**********************************************************************/
/***************************** LoadAChar ******************************/
/**********************************************************************/
void
LoadAChar(long4 c, register struct char_entry *ptr)
{
  long4    *pr;
  long     bytes;

  if (ptr->where.address.fileOffset == NONEXISTANT
#ifdef LJ_RESIDENT_FONTS
      || fontptr->resident_p
#endif
      ) {
    ptr->where.isloaded = _FALSE;
    return;
  }

  OpenFontFile();

#ifdef DEBUG
  if (Debug)
    fprintf(ERR_STREAM, "LoadAChar: <%c>(%ld) from file at pos %ld\n",
            (char)c,(long)c,(long)ptr->where.address.fileOffset);
#endif

#ifdef USEPXL

  FSEEK(pxlfp, ptr->where.address.fileOffset, SEEK_SET);

  if (fontptr->id == pk89) {
#ifdef PARANOIA
    unsigned char   temp;
    temp = (unsigned char) NoSignExtend(pxlfp, 1);

    if ((int)(ptr->flag_byte) != (int)temp) {
      fprintf(ERR_STREAM,"font=%lx, ptr=%lx\n",fontptr,ptr);
      Fatal("%d: oh boy! old flag %d, new flag %d, ptr=%lx\n",
            (int)c,(int)(ptr->flag_byte),(int)temp,ptr);
    }
#endif

    if ((ptr->flag_byte & 7) == 7) {
      bytes = NoSignExtend(pxlfp, 4) - 28;
      FSEEK(pxlfp, ptr->where.address.fileOffset + 36, SEEK_SET);
      /*
        fprintf(ERR_STREAM,"bytes=%d, seeking at %ld\n",
        bytes, ptr->where.address.fileOffset + 36);
        */
    } else if ((ptr->flag_byte & 4) == 4) {
      bytes = ((long4)ptr->flag_byte & 3) * 65536l + NoSignExtend(pxlfp, 2) - 13;
      FSEEK(pxlfp, ptr->where.address.fileOffset + 16, SEEK_SET);
    } else {
      bytes = ((long4)ptr->flag_byte & 3) * 256 + NoSignExtend(pxlfp, 1) - 8;
      FSEEK(pxlfp, ptr->where.address.fileOffset + 10, SEEK_SET);
    }
  } else if (fontptr->id == id1002)
    bytes =  ((( (long4)ptr->width + 7) >> 3) * (long4)ptr->height);
  else if (fontptr->id == id1001)
    bytes =  4 * (((long4)ptr->width + 31) >> 5) * (long4)ptr->height;
  else
    bytes = 0;

  if (bytes > 0) {
    /* do NOT load Huge characters */
    if ((bytes > HUGE_CHAR_PATTERN) && (fontptr->id != pk89)) {
      qfprintf(ERR_STREAM,"Huge Character <%c> (%ld Bytes)\n",
               (char)c, bytes);
      ptr->charsize = HUGE_SIZE;
      ptr->where.isloaded = _FALSE;
    } else {
      if ( (pr = (long4 *)malloc( bytes )) == NULL )
        Fatal("Unable to allocate %ld bytes for char <%c>\n", bytes, (char)c);
      /*
       * else fprintf(ERR_STREAM,"allocating %ld bytes char <%c>(%d)\t at 0x%lx\n",
       *                      bytes, (char)c,(int)c,pr);
       */
#ifdef DEBUG
      if (Debug)
        fprintf(ERR_STREAM,
                "Allocating Char <%c>, FileOffset=%lX, Bytes=%ld (%d) <%d>\n",
                (char)c, ptr->where.address.fileOffset, bytes,
                (int)bytes, (unsigned int)bytes);
#endif
      allocated_storage += bytes;
      read_multi(pr, 1, (int)bytes , pxlfp);
      ptr->where.address.pixptr = pr;
    }
  }
#else
  FSEEK(gfin, ptr->where.address.fileOffset, SEEK_SET);
  gettochar();
  readbits();
  if (num_bytes > HUGE_CHAR_PATTERN)
    ptr->charsize = HUGE_SIZE;
#endif
  ptr->where.isloaded = _TRUE;
  if (ptr->charsize != SMALL_SIZE
#ifdef LJ
      /* we might have problems at the edge of the paper with diff. sized characters
       * the correct treatment would be to check whether the bounding box of
       * tfontptr is within the paper relative to the current position
       */
      || fontptr->max_height > CHAR_HEIGTH_LARGE
      || (rasterfont[fontptr->plusid])
#endif
      )
    return;

  EmitChar(c, ptr);
#ifdef USEPXL
  /* we should really free the space used by the PXL data after this
     point, but it is not large, and besides, we may want to be
     more clever in the future, about sending bitmaps.  So keep
     the data around */
#endif
}

/*-->SetChar*/
/**********************************************************************/
/*****************************  SetChar  ******************************/
/**********************************************************************/
void
SetChar(long4 c, short command, int PassNo, bool do_posn, bool in_string)
{
  register struct char_entry *ptr;  /* temporary char_entry pointer */
  bool pos_after = _FALSE;

  ptr = &(fontptr->ch[c]);
  if (!((ptr->where.isloaded) || (ptr->charsize == HUGE_SIZE)))
    LoadAChar(c, ptr);
  if (PassNo == 0)
    return;

  if (do_posn) {
#ifdef IBM3812
    if (CharStringPos>0) {
      fprintf(ERR_STREAM,"!!!! That should never happen!!!\n");
      CharStringOut;
    }
#endif
    SetPosn(h, v);
  }

  /*
    fprintf(ERR_STREAM,
            "(%d) hh=%ld (+%ld/+%ld), h=%ld, xh=%ld,xhh=%ld, [%ld|%ld] ->%d\n",
            (int)do_posn,hh,(long4)ptr->cw,(long4)ptr->cw*(long4)hconv,h,
            (long)PIXROUND(h, hconv),
            (long)PIXROUND(hh, hconv),
            (long)labs((hh-h)),(long)hconv,
            (labs((hh-h))>hconv)
            );
            */

  if (in_string && (labs((hh-h))>hconv)) {
#ifdef IBM3812
    CharStringOut;
#endif
    SetPosn(h, v);
  }

  /*
    fprintf(ERR_STREAM,"raster?=%d - last_ry=%d, last_rx=%d,mmax-height=%d\n",
    (int)last_ry < fontptr->max_height, (int)last_ry,(int)last_rx,
    (int)fontptr->max_height);
    */

  if (fontptr->font_file_id != NO_FILE) {      /* ignore missing fonts */
    if (
#ifdef LJ_RESIDENT_FONTS
        !fontptr->resident_p &&
#endif
        (ptr->charsize != SMALL_SIZE
#ifdef LJ
         /* the LaserJet cannot print characters
          * where the bounding box lies outside the
          * paper edge. Missing: right paper edge
          */
         || last_ry < (int)fontptr->max_height
         || fontptr->max_height > CHAR_HEIGTH_LARGE
         || (rasterfont[fontptr->plusid])
         || (brother_mode && c == 0)
#ifdef SEVENBIT
         || (kyocera_mode && c == 32)
#endif
#endif
         )) {
#ifdef LJ
      int     tmp;
      char    sign;

      if (!do_posn)
        SetPosn(h, v);

#ifdef USEPXL
      tmp = (int)-ptr->yOffset;
#else
      tmp = (int)num_rows-y_offset;
#endif
      if (tmp != 0) {
        if (tmp < 0) {
          sign = '-';
          tmp = -tmp;
        } else
          sign = '+';
        EMIT3("\033*p%c%dY", sign, tmp);
      }
#ifdef USEPXL
      tmp = (int)-ptr->xOffset;
#else
      tmp = (int)-x_offset;
#endif
      if (tmp != 0) {
        if (tmp < 0) {
          sign = '-';
          tmp = -tmp;
        } else
          sign = '+';
        EMIT3("\033*p%c%dX", sign, tmp);
      }
#endif
#ifdef IBM3812
      CharStringOut;
#endif
#ifdef DEBUG
      if (Debug)
#ifndef vms
        fprintf(ERR_STREAM,"Raster character <%c> %hd\n", (char) c,(short)c);
#else
      fprintf(ERR_STREAM,"Raster character <%c> %d\n", (char) c,(short)c);
#endif
#endif
      RasterChar(ptr);
      pos_after = _TRUE;
    } else {
      unsigned char cc;
      cc = VisChar((char)c);
#ifdef IBM3812
#ifdef USEPXL
      if ( ptr->yyOffset || (!in_string) ) {
        CharStringOut;
        MoveVert(ptr->yyOffset);
        sprintf(PMPformat, "\01%c", cc);
        PMPout(2, PMPformat);
        MoveVert((int)-(ptr->yyOffset));
      } else {
#endif
        if (CharStringPos==CHARSTRINGMAX)
          CharStringOut;

        CharString[CharStringPos] = cc;
        CharStringPos++;
#ifdef USEPXL
      }
#endif
#endif
#ifdef LJ
#define TRANSPARENTCHAR(ch) \
      if ((ch == 0l) || (ch >= 7l && ch <= 15l) || (ch == 27l)) \
          EMIT2("\033&p1X%c", (unsigned char)ch); \
      else { EMITC((unsigned char)ch); }
#ifdef USEPXL
      if (ptr->yyOffset) {
#ifndef vms
        EMIT2("\033*p+%hdY", ptr->yyOffset);
        TRANSPARENTCHAR(cc);
        EMIT2("\033*p-%hdY", ptr->yyOffset);     /* GUGUGU 255 */
#else
        EMIT2("\033*p+%dY", ptr->yyOffset);
        TRANSPARENTCHAR(cc);
        EMIT2("\033*p-%dY", ptr->yyOffset);     /* GUGUGU 255 */
#endif
      } else
#endif
        /*                EMITC( (unsigned char)c);*/
        { TRANSPARENTCHAR(cc);}
#endif
    }
    /*
    fprintf(stderr,"--> hh(%ld) += cw(%ld) * hconv(%ld)",
            (long)hh, (long)ptr->cw, (long) hconv);
            */
#ifdef LJ
    /* the guessed position must be a multiple of pixels */
    if (RESOLUTION == 300)
      hh += (long4) ((ptr->cw)/4)*4*hconv;
    else /* RESOLUTION == 600 */
      hh += (long4) ((ptr->cw)/2)*2*hconv;
#else
    hh += (long4)(ptr->cw * hconv);
#endif
    /*
      fprintf(stderr," = hh(%ld)\n",(long)hh);
      */
  }

  if (command <= SET4)
    h += ptr->tfmw;

  if (pos_after)
    SetPosn(h, v);
  else
    last_rx = (int)PIXROUND(h, hconv) + x_goffset;
}

void DoBop(void)
{
  struct font_entry *p;
#ifdef LJ
  register short i;
  if (fonts_used_on_this_page > MAX_FONTS_PER_PAGE) {
    for (i = 0; i < HANDLE_MAX_FONTS; i++)
      rasterfont[i] = _FALSE;
  }
  fonts_used_on_this_page = 0;
#endif
  for (p = hfontptr; p; p = p->next)
    p->used_on_this_page = _FALSE;
}

/*-->SetString*/
/**********************************************************************/
/*****************************  SetString  ****************************/
/**********************************************************************/
void
SetString(short firstch, int PassNo)
{
  short   c;
  register unsigned short i;

#ifdef DEBUG
  if (Debug)
    fprintf(ERR_STREAM, "SetString ");
#endif
  for (i = 0, c = firstch; c >= SETC_000 && c <= SETC_127; i++) {
#ifdef DEBUG
    if (Debug)
      fprintf(ERR_STREAM, "%d(%c) ", c, c);
#endif
    SetChar((long4)c, c, PassNo, (bool)(i==0), _TRUE);
    c = (short)NoSignExtend(dvifp, 1);
  }
  FSEEK(dvifp, -1l, SEEK_CUR);    /* backup one character */
#ifdef IBM3812
  CharStringOut;
#endif
#ifdef DEBUG
  if (Debug)
    fprintf(ERR_STREAM, "...SetString\n");
#endif
}

/*-->SetPosn*/
/**********************************************************************/
/*****************************  SetPosn  ******************************/
/**********************************************************************/
void                  /* output a positioning command */
SetPosn(long4 x, long4 y)
{
  int     rx, ry;
  rx = (int)PIXROUND(x, hconv) + x_goffset;
  ry = (int)PIXROUND(y, vconv) + y_goffset;

  /*
  fprintf(ERR_STREAM,
	  "setposn to %ld/%ld, %d/%d, last: %d/%d\n",
	  (long)x,(long)y,
	  rx,ry,
	  last_rx,last_ry
      );
      */

#ifdef IBM3812
  PMPcont(3);
  PMPoutC('\340');
  EMITWORD(LARGER(rx,0));

  if (last_ry != ry) { /* necessary to set new y-position */
    PMPcont(3);
    PMPoutC('\341');
    EMITWORD(LARGER(ry,0));
  }
#endif
#ifdef LJ
#if 0
/* this optimization doesn't work, as last_XX is set to the
   starting(!) position of the last string not to the end position! */

  if (last_rx != rx || last_ry != ry) {
    if (last_ry == ry)   /* not necessary to set new y-position */
      EMIT2("\033*p%dX", LARGER(rx,0));
    else if (last_rx == rx)
      EMIT2("\033*p%dY", LARGER(ry,0));
    else
      EMIT3("\033*p%dx%dY", LARGER(rx,0), LARGER(ry,0));
  }
#endif
  if (last_ry == ry)   /* not necessary to set new y-position */
    EMIT2("\033*p%dX", LARGER(rx,0));
  else
    EMIT3("\033*p%dx%dY", LARGER(rx,0), LARGER(ry,0));
#endif

  last_ry = ry;    /* last y-position on output device */
  last_rx = rx;    /* last x-position on output device */
  /*
   * must know where device "really" is horizontally, for rel. posning.
   * (maybe in the future), but we always use direct positioning for
   * vertical movement.
   */
  /* hh = rx * hconv; */
  hh = x;
  vv = y;
  /*
   * fprintf(ERR_STREAM,"DoPosn: x=%ld, y=%ld, rx=%d, ry=%d, hh=%ld, vv=%ld\n",
   *         (long)x,(long)y,rx,ry,(long)hh,(long)vv);
   */
}



#ifdef LJ4
/* Compresses a rasterline of font data and appends it to buffin (used
   for storage of the compressed character). CChar_Off is the current
   offset in the buffer, CChar_Last is the first byte of the last
   compacted row (line repeat count).  Updates the previous line
   (PrevLine) if necessary. */
void
CompressedCharLine(struct char_entry *ce, int nbpl, unsigned char *buffer)
{
  if (CChar_Off >= 0) {
    register unsigned char *obuf,*buff;
    unsigned char *prev,*end;
#undef max /* cc thinks any max macro overrides a variable */
    int t,mode,max;
    register int x,y,i;

    prev = PrevLine;
    obuf = buffer;
    end = buffer + nbpl;
    x = 0;
    while (obuf < end) {
      if (*obuf != *prev) {
        *prev = *obuf;
        x = 1;
      }
      obuf++;
      prev++;
    }
    if (x == 0 && CChar_Last >= 0) {
      (buffin[CChar_Last])++;
      return; /* line repeat count +1 */
    }

    end = buffin + BUFFSIZE - 16;
    obuf = buffin + CChar_Off;
    buff = buffer;
    *obuf++ = 0; /* Line repeat count = 0 */
    t = 0;
    CChar_Last = CChar_Off; /* Next line */
    mode = 0;
    y = *buff++;
    i = 7; /* i = current bit */
    while ((t < ce->width) && (obuf < end)) {
      max = ce->width - t; /* maximum pixels left to do */
      x = 0;
      if (mode == 0) {
        /* white run */
        while ((y & (1<<i)) == 0 && x < max) {
          x++;
          i--;
          if (i < 0) {
            i = 7;
            y = *buff++;
            while (y == 0 && x < max) {
              x += 8;
              y = *buff++;
            }
          }
        }
        mode = 1;
      }
      else {
        /* black run */
        while ((y & (1<<i)) != 0 && x < max) {
          x++;
          i--;
          if (i < 0) {
            i = 7;
            y = *buff++;
            while (y == 0xff && x < max) {
              x += 8;
              y = *buff++;
            }
          }
        }
        mode = 0;
      }
      if (x > max)
        x = max;
      t += x;
      /* make precautions for very long runs */
      while (x > 0xff) {
        *obuf++ = 0xff;
        *obuf++ = 0;
        x -= 0xff;
      }
      *obuf++ = x;
    }
    CChar_Off = obuf >= end ? -1 : obuf - buffin;
  }
}
#endif

/*------------------------ end dvichars.c ------------------------------*/


/* Report a warning if both checksums are nonzero, they don't match, and
   the user hasn't turned it off.  */

static void
check_checksum (unsigned c1, unsigned c2, const char *name)
{
  if (c1 && c2 && c1 != c2
#ifdef KPATHSEA
      && !kpse_tex_hush ("checksum")
#endif
      ) {
     Warning ("Checksum mismatch in %s", name) ;
   }
}


/*------------------------ begin dviconv.c -----------------------------*/
/*-->ActualFactor*/
/**********************************************************************/
/**************************  ActualFactor  ****************************/
/**********************************************************************/
double  /* compute the actual size factor given the approximation */
ActualFactor(long4 unmodsize)
{
  double  realsize;     /* the actual magnification factor */
  realsize = (double)unmodsize / 1000.0;
  if (abs((int)(unmodsize - 1095l))<2)
    realsize = 1.095445115; /*stephalf*/
  else if (abs((int)(unmodsize - 1315l))<2)
    realsize = 1.31453414; /*stepihalf*/
  else if (abs((int)(unmodsize - 1577l))<2)
    realsize = 1.57744097; /*stepiihalf*/
  else if (abs((int)(unmodsize - 1893l))<2)
    realsize = 1.89292916; /*stepiiihalf*/
  else if (abs((int)(unmodsize - 2074l))<2)
    realsize = 2.0736;   /*stepiv*/
  else if (abs((int)(unmodsize - 2488l))<2)
    realsize = 2.48832;  /*stepv*/
  else if (abs((int)(unmodsize - 2986l))<2)
    realsize = 2.985984; /*stepvi*/
  /* the remaining magnification steps are represented with sufficient
     accuracy already */
  return(realsize);
}


/*-->DecodeArgs*/
/*********************************************************************/
/***************************** DecodeArgs ****************************/
/*********************************************************************/
void
DecodeArgs(int argc, char *argv[])
{
  int     argind;            /* argument index for flags      */
  char    *curarea;	     /* current file area             */
  char    *curname;   	     /* current file name             */
  char    *tcp, *tcp1;       /* temporary character pointers  */
  const char *ctcp;          /* temporary const char pointer  */
  char    *this_arg;
  double  x_offset = 0.0, y_offset = 0.0;
#ifdef __riscos
  int     ddi;
#endif


#ifndef KPATHSEA
  if ((tcp = getenv("TEXPXL")) != NULL) PXLpath = tcp;
#ifdef LJ_RESIDENT_FONTS
  if ((tcp = getenv("TFMFONTS")) != NULL)
    TFMpath = tcp;
  else if ((tcp = getenv("TEXFONTS")) != NULL)
    TFMpath = tcp;
#endif
#endif

  if (argc == 2 && EQ(argv[1], "--version")) {
    puts (VERSION);
    puts (kpathsea_version_string);
    puts ("Copyright (C) 1997 Gustaf Neumann.\n\
There is NO warranty.  You may redistribute this software\n\
under the terms of the GNU General Public License.\n\
For more information about these matters, see the files\n\
named COPYING and dvi2xx.c.\n\
Primary author of Dvi2xx: Gustaf Neumann; -k maintainer: K. Berry.");
    exit (0);
  }

#ifdef LJ4
  /* check if last character is a 6 */
  LJ6 = ('6' == argv[0][strlen(argv[0])-1]);
#endif

  argind = 1;
  while (argind < argc) {
    tcp = argv[argind];
    if (tcp[0] == '-' && tcp[1] != '\0') {
      ++tcp;
      switch (*tcp) {
#ifndef KPATHSEA
      case 'a':       /* a selects different pxl font area */
        PXLpath = ++tcp;
        break;
#endif
      case 'A':
        ManualFeed = _TRUE;
        break;
#ifdef IBM3812
      case 'b':       /* first page from alternate casette */
        FirstAlternate = _TRUE;
        break;
#endif
      case 'c':       /* number of copies to print */
        if ( sscanf(tcp + 1, "%hd", &ncopies) != 1 )
          Fatal("Argument of -c is not a valid integer\n");
        if (ncopies<1) {
          Warning("argument of -c < 1; set to 1!");
          ncopies = 1;
        }
        break;
#ifdef DEBUG
      case '-':       /* --D selects Debug output */
        tcp++;
        if (*tcp == 'D') {
          Debug = _TRUE;
#ifdef KPATHSEA
          sscanf(tcp + 1, "%u", &kpathsea_debug);
#endif
          qfprintf(ERR_STREAM,"Debug output enabled\n");
        }
        break;
#endif
#ifdef LJ2P
      case 'd':       /* d selects DUPLEX mode  */
        tcp++;
        if (*tcp == '1' )
          DuplexMode = 1;
        else if (*tcp == '2')
          DuplexMode = 2;
        else {
          Warning("Invalid DUPLEX mode, assuming DUPLEX=1, Long-Edge Binding");
          DuplexMode = 1;
        }
        break;
#endif
      case 'D':       /* D selects DoublePage  */
        DoublePage = _TRUE;
        tcp++;
        if (*tcp == '1' || *tcp == '2') {
          if (*tcp == '2')
            PrintFirstPart = _FALSE;
          else
            PrintSecondPart = _FALSE;
          tcp++;
        }
        if (*tcp == '-')
          PrintEmptyPages = _FALSE;
        break;
#ifdef LJ4
      case 'E':       /* do not reset printer (go) */
        econoMode = _TRUE;
        break;
#endif
      case 'e':       /* emit file is specified */
        tcp++;
#ifdef MSDOS
        /* delete trailing ':' (causing hangup) */
        if (tcp[strlen(tcp)-1] == ':')
          tcp[strlen(tcp)-1] = '\0';
#endif
#ifdef OS2  /* repeated to avoid problems with stupid c preprocessors  */
        /* delete trailing ':' (causing hangup) */
        if (tcp[strlen(tcp)-1] == ':')
          tcp[strlen(tcp)-1] = '\0';
#endif
        EmitFileName = tcp;
        break;
      case 'f':       /* next arg is starting pagenumber */
        if ( sscanf(tcp + 1, FMT_long4, &FirstPage) != 1 )
          Fatal("Argument is not a valid integer\n");
        FirstPageSpecified = _TRUE;
        break;
#ifdef LJ
      case 'g':       /* do not reset printer (go) */
        my_ResetPrinter = _FALSE;
        break;
#endif
      case 'h':     /* copy header file through to output  */
        HeaderFileName = ++tcp;
        G_header = _TRUE;
        break;
#if defined(LJ2P) || defined(IBM3812)
      case 'l':       /* landscape  */
        Landscape = _TRUE;
        break;
#endif
#ifdef MAKETEXPK
      case 'M':
        /* -M, -M1 => don't make font; -M0 => do.  */
        makeTexPK = *(tcp + 1) == '0';
#ifdef KPATHSEA
        kpse_set_program_enabled (kpse_pk_format, MAKE_TEX_PK_BY_DEFAULT, kpse_src_cmdline);
#endif /* KPATHSEA */
        break;
#endif /* MAKETEXPK */
      case 'x':       /* specify x-offset */
        this_arg = 0;
        if (!(*++tcp)) {
          this_arg = (++argind >= argc ? 0 : argv[argind]);
        } else {
          this_arg = tcp;
        }
        if (!this_arg
            || sscanf(this_arg,"%lf", &x_offset) != 1)
          Fatal("Argument of -x is not a valid floating point number\n");
        break;
      case 'y':       /* specify y-offset */
        this_arg = 0;
        if (!(*++tcp)) {
          this_arg = (++argind >= argc ? 0 : argv[argind]);
        } else {
          this_arg = tcp;
        }
        if (!this_arg || sscanf(this_arg, "%lf", &y_offset) != 1)
          Fatal("Argument of -y is not a valid floating point number\n");
        break;
      case 'X':       /* specify X-origin in dots */
        this_arg = 0;
        if (!(*++tcp)) {
          this_arg = (++argind >= argc ? 0 : argv[argind]);
        } else {
          this_arg = tcp;
        }
        if (!this_arg || sscanf(this_arg,"%hd", &x_origin) != 1)
          Fatal("Argument of -X is not a valid integer\n");
        break;
      case 'Y':       /* specify Y-origin in dots */
        this_arg = 0;
        if (!(*++tcp)) {
          this_arg = (++argind >= argc ? 0 : argv[argind]);
        } else {
          this_arg = tcp;
        }
        if (!this_arg ||
            sscanf(this_arg, "%hd", &y_origin) != 1)
          Fatal("Argument of -Y is not a valid integer\n");
        break;
      case 'm':       /* specify magnification to use */
        switch ( (*++tcp) ) {
        case '#':
          /* next arg is a magnification to use */
          if ( sscanf(tcp + 1, "%ld", &usermag) != 1 )
            Fatal("Argument of mag is not a valid integer\n");
          break;
        case '0':
          usermag = 1000;
          break;
        case 'h':
        case 'H':
          usermag = 1095;
          break;
        case '1':
          usermag = 1200;
          break;
        case 'q':
          usermag = 1250;
          break;
        case '2':
          usermag = 1440;
          break;
        case '3':
          usermag = 1728;
          break;
        case '4':
          usermag = 2074;
          break;
        case '5':
          usermag = 2488;
          break;
        default:
          Fatal("%c is a bad mag step\n", *tcp);
        }
        break;
#ifdef SUPERCOMMENT
      case 'o':     /* PostScript command to send */
        if ( ++argind >= argc )
          Fatal("No argument following -o\n", 0);
        PScmd[nps++] = argv[argind];
        break;
#endif
      case 'p':       /* print n pages  */
        if ( sscanf(tcp + 1, FMT_long4, &PrintPages) != 1 )
          Fatal("Argument is not a valid integer\n");
        if (PrintPages < 1)
          Fatal("Argument of -p must be greater than 0\n");
        break;
      case 'q':       /* quiet operation */
        G_quiet = _TRUE;
        break;
      case 'r':       /* switch order to process pages */
        Reverse = (bool)(!Reverse);
        break;
#ifdef LJ
      case 's':       /* specify X-origin in dots */
        this_arg = 0;
        if (!(*++tcp)) this_arg = (++argind >= argc ? 0 : argv[argind]);
        else           this_arg = tcp;
        if (!this_arg || sscanf(this_arg,"%hd", &pagesize) != 1)
          Fatal("Argument of -s is not a valid integer\n");
        /*
         * The pgsiz_dots value assumes a resolution of 300dpi. This loses
         * at 600dpi so we will scale them below for the LJ4.
         */
        switch (pagesize) {
        case  1: pgsiz_dots = 10.5 * 300; break;       /* executive */
        case  2: pgsiz_dots = 11 * 300; break;	       /* letter */
        case  3: pgsiz_dots = 14 * 300; break;	       /* legal */
        case  6: pgsiz_dots = 17 * 300; break;	       /* ledger */
        case 25: pgsiz_dots = 210 * 300 / 25.4; break; /* a5 */
        case 26: pgsiz_dots = 297 * 300 / 25.4; break; /* a4 */
        case 27: pgsiz_dots = 420 * 300 / 25.4; break; /* a3 */
        case 45: pgsiz_dots = 257 * 300 / 25.4; break; /* jis b5 */
        case 46: pgsiz_dots = 354 * 300 / 25.4; break; /* jis b4 */
        case 71: pgsiz_dots = 148 * 300 / 25.4; break; /* hagaki postcard */
        case 72: pgsiz_dots = 148 * 300 / 25.4; break; /* oufuku-hagaki */
        case 80: pgsiz_dots = 7.5 * 300; break;        /* monarch envelope */
        case 81: pgsiz_dots = 9.5 * 300; break;        /* com10 envelope */
        case 90: pgsiz_dots = 220 * 300 / 25.4; break; /* int dl */
        case 91: pgsiz_dots = 229 * 300 / 25.4; break; /* int c5 */
        case 100: pgsiz_dots = 250 * 300 / 25.4; break; /* int b5 */
        default: Fatal(
#ifndef vms
                       "%hd is a bad value for pagesize (1,2,3,26,80,81,90,91)",
#else
                       "%d is a bad value for pagesize (1,2,3,26,80,81,90,91)",
#endif
                       pagesize);
        }
        break;
#endif
      case 't':       /* ending pagenumber */
        if ( sscanf(tcp + 1, FMT_long4, &LastPage) != 1 )
          Fatal("Argument is not a valid integer\n");
        LastPageSpecified = _TRUE;
        break;
      case 'v':    /* verbatim mode (print pxl-file names) */
        G_verbose = _TRUE;
        break;

#ifdef LJ
      case 'V':   /* bugfixes for various vendor's PCL emulations */
        tcp++;
        if (*tcp == 'K' || *tcp == 'k')
          kyocera_mode = _TRUE;
        else
          if (*tcp == 'B' || *tcp == 'b')
            brother_mode = _TRUE;
# ifdef LJ4
	else
        if (*tcp == '6' || *tcp == '6')
	    LJ6 = _TRUE;
# endif
        break;
#endif

      case 'w':       /* don't print out warnings */
        G_nowarn = _TRUE;
        break;
#ifdef LJ
      case 'z':
        PrintTestPage = (bool)(!PrintTestPage);
        break;
#endif
#ifdef LJ4
      case 'R':       /* number of copies to print */
        if ( sscanf(tcp + 1, "%d", &RESOLUTION) != 1 )
          Fatal("Argument of -R is not a valid integer\n");
        if (RESOLUTION != 300 && RESOLUTION != 600 ) {
          Warning("Resolution must be 300 or 600! Assuming 300.dpi.");
          RESOLUTION = 300;
        } else {
          if (RESOLUTION == 600) {
            MFMODE = MFMODE600;
            x_origin = 600;
            y_origin = 600;
          }
        }
        break;
      case 'W':  /* Set minimum width of compressed raster characters */
        CompressCharWidth = atoi(tcp+1);
        break;
      case 'C':  /* Set compression mode to use for raster characters */
        UseCompression = atoi(tcp+1);
        if ((UseCompression != 0) && (UseCompression != 2) && (UseCompression != 3))
          Fatal("Unsupported compression mode %d\n",UseCompression);
        break;
      case 'n': /* Download fonts raw */
        CompressFontMode = _FALSE;
        break;
#endif
#ifdef __riscos
      case 'i': /* name of directory to store diagrams in */
        tcp++;
        ddi = 0;
        while (*tcp != ' ' && *tcp != '\0' && ddi < DIAGDIRSIZE-1) {
          diagdir[ddi++] = *tcp++;
        }
        diagdir[ddi] = '\0';
        break;
      case 'j': /* don't print digrams */
        printdiag = _FALSE;
        break;
      case 'k': /* cache diagram bitmap in document folder */
        cachediag = _TRUE;
        break;
      case 'P':
        RasterMultipass = 1;
        break;
#endif
      default:
        fprintf(ERR_STREAM, "%c is not a valid flag\n", *tcp);
      }
    } else {

      filename = tcp;
      if (EQ(filename, "-")) {
        EmitFileName = "-";
#ifdef RISC_USE_OSL
        dvifp = BINOPEN("Kbd:");
#else
        dvifp = stdin;
	if (O_BINARY && !isatty(fileno(dvifp)))
	  AssureBinary(fileno(dvifp));
#endif
      } else {
	/* Since this code is used only once during startup, we don't care
	   about free()ing the allocated strings that represent filenames.
	   It will be more work to realize proper deallocation handling than
	   it's worth in terms of saving a few bytes. We consider these
	   bytes actually static memory where we don't know the size in
	   advance and don't add them to the allocated_storage count.
	   [27 Jun 07 -js] */
#ifdef KPATHSEA
        /* split into directory + file name */
	int tcplen, argvlen;
	ctcp = xbasename(argv[argind]);/* this knows about any kind of slashes */
	tcplen = strlen(ctcp);
	if ( tcplen == 0 ) {
	  /* This happens when the DVI file name has a trailing slash; this
	     is not a valid name. Then we terminate the argument parsing
	     loop, a usage message will be output below. */
	  break;
	}
	argvlen = strlen(argv[argind]);
	if (tcplen == argvlen)
	  curarea = xstrdup("");
	else {
	  curarea = xstrdup(argv[argind]);
	  curarea[argvlen-tcplen] = '\0';
	}
#else
        ctcp = strrchr(argv[argind], '/');
        /* split into directory + file name */
        if (ctcp == NULL) {
	  curarea = xstrdup("");
          ctcp = argv[argind];
        } else {
	  curarea = xstrdup(argv[argind]);
          curarea[ctcp-argv[argind]+1] = '\0';
          ctcp += 1;
        }
#endif

        curname = (char *) xmalloc(strlen(ctcp)+5);  /* + space for ".dvi" */
	(void) strcpy(curname, ctcp);
        /* split into file name + extension */
        tcp1 = strrchr(curname, '.');
        if (tcp1 == NULL) {
          rootname = xstrdup(curname);
          strcat(curname, ".dvi");
        } else {
          *tcp1 = '\0';
          rootname = xstrdup(curname);
          *tcp1 = '.';
        }

	filename = (char *) xmalloc(strlen(curarea)+strlen(curname)+1);
        (void) strcpy(filename, curarea);
        (void) strcat(filename, curname);

        if ((dvifp = BINOPEN(filename)) == FPNULL) {
          /* do not insist on .dvi */
          if (tcp1 == NULL) {
	    filename[strlen(filename) - 4] = '\0';
	    dvifp = BINOPEN(filename);
          }
          if (dvifp == FPNULL) {
#ifdef MSC5
            Fatal("%s: can't find DVI file \"%s\"\n\n",
                  G_progname, filename);
#else
            perror(filename);
            exit (EXIT_FAILURE);
#endif
          }
        }
      } /* dvi filename != '-" */
    }
    argind++;
  }

#ifdef LJ4
  pgsiz_dots *= (int)(RESOLUTION/300); /* rescale dots to page */
#endif
  x_goffset = (short) MM_TO_PXL(x_offset) + x_origin;
  y_goffset = (short) MM_TO_PXL(y_offset) + y_origin;

  if (dvifp == FPNULL) {
    fprintf(ERR_STREAM,"\nThis is the DVI to %s converter %s",
            PRINTER, VERSION);
#ifdef SEVENBIT
    fprintf(ERR_STREAM,", 7bit");
#endif
    fprintf(ERR_STREAM," (%s)\n", OS);
    fprintf(ERR_STREAM,"usage: %s [OPTION]... DVIFILE\n", G_progname);

    fprintf(ERR_STREAM,"OPTIONS are:\n");
#ifdef DEBUG
    fprintf(ERR_STREAM,"\t--D ..... turns debug output on\n");
#endif
#ifndef KPATHSEA
    fprintf(ERR_STREAM,
            "\t-aX ..... X= searchpath leading to pixel-files (.pk or .pxl)\n");
#endif
#ifdef IBM3812
    fprintf(ERR_STREAM,
            "\t-b  ..... take paper for first page from alternate casette\n");
#endif
    fprintf(ERR_STREAM,"\t-cX ..... X= number of copies\n");
#ifdef LJ4
    fprintf(ERR_STREAM,"\t-CX ..... X= compression mode for raster chars; can be 0,2,3 (default=%d)\n", DEFAULT_COMPRESS_MODE);
#endif
#ifdef LJ2P
    fprintf(ERR_STREAM,"\t-dX ..... duplex, X=1: long-edge, 2: short-edge binding\n");
#endif
    fprintf(ERR_STREAM,"\t-D  ..... turns doublepage output on; ");
    fprintf(ERR_STREAM,"-D1 odd pages only, -D2 even\n");
#ifdef LJ4
    fprintf(ERR_STREAM,"\t-E  ..... print in econo-mode\n");
#endif
    fprintf(ERR_STREAM,"\t-eX ..... X= output file\n");
    fprintf(ERR_STREAM,"\t-fX ..... print from begin of page number X\n");
#ifdef LJ
    fprintf(ERR_STREAM,
            "\t-g  ..... do not reset printer at begin of job (go)\n");
#endif
#ifdef __riscos
    fprintf(ERR_STREAM,"\t-iX ..... X= name of dir to cache diagrams in\n");
    fprintf(ERR_STREAM,"\t-j  ..... don't print diagrams\n");
    fprintf(ERR_STREAM,"\t-k  ..... cache diagram bitmaps\n");
#endif
    fprintf(ERR_STREAM,"\t-hX ..... X= name of headerfile\n");
#ifdef LJ2P
    fprintf(ERR_STREAM,"\t-l  ..... landscape mode\n");
#endif
#ifdef MAKETEXPK
    fprintf(ERR_STREAM,"\t-MX ..... Don't generate missing PK files\n");
#endif
    fprintf(ERR_STREAM,"\t-mX ..... magnification X=0;h;1;2;3;4;5;#xxxx\n");
#ifdef LJ4
    fprintf(ERR_STREAM,"\t-n  ..... download fonts raw (default: compressed)\n");
#endif
    fprintf(ERR_STREAM,"\t-pX ..... print X pages\n");
#ifdef __riscos
    fprintf(ERR_STREAM,"\t-P  ..... Process printouts in 2 passes\n");
#endif
    fprintf(ERR_STREAM,"\t-q  ..... quiet operation\n");
    fprintf(ERR_STREAM,"\t-r  ..... process pages in reverse order\n");
#ifdef LJ4
    fprintf(ERR_STREAM,"\t-RX ..... set resolution to 300 or 600 dpi\n");
#endif
#ifdef LJ
    fprintf(ERR_STREAM,"\t-sX ..... set paper size to X (see documentation)\n");
#endif
    fprintf(ERR_STREAM,"\t-tX ..... print to end of page number X\n");
    fprintf(ERR_STREAM,"\t-w  ..... don't print out warnings\n");
#ifdef LJ4
    fprintf(ERR_STREAM,"\t-WX ..... X= minimum width of compressed chars (default=%d)\n",
            DEFAULT_COMPRESS_WIDTH);
#endif
    fprintf(ERR_STREAM,"\t-v  ..... tell user which pixel-files are used\n");
#ifdef LJ
    fprintf(ERR_STREAM,"\t-VX  .... Vendor options (kyocara or brother)\n");
#endif
    fprintf(ERR_STREAM,"\t-xX ..... X= x-offset on printout in mm\n");
    fprintf(ERR_STREAM,"\t-yX ..... X= y-offset on printout in mm\n");
    fprintf(ERR_STREAM,"\t-XO ..... O= x page origin in dots (default=%d)\n",
            XDEFAULTOFF );
    fprintf(ERR_STREAM,"\t-YO ..... O= y page origin in dots (default=%d)\n",
            YDEFAULTOFF );
#ifdef LJ
    fprintf(ERR_STREAM,"\t-z  ..... print test page with pagecounter after job\n");
#endif
    fprintf(ERR_STREAM,"\t-   ..... dvifile is stdin (must be seekable); implies -e-\n");
#ifdef KPATHSEA
    {
      putc ('\n', ERR_STREAM);
      fputs (kpse_bug_address, ERR_STREAM);
    }
#endif
    exit(1);
  }
  if (EQ(EmitFileName, "")) {
    tcp = (char *) xmalloc(strlen(rootname)+sizeof(EMITFILE_EXTENSION));
    (void) strcpy(tcp, rootname);
    strcat(tcp, EMITFILE_EXTENSION);
    EmitFileName = tcp;
  }
  if (G_quiet)
    G_verbose = _FALSE;
}

/*-->DoConv*/
/*********************************************************************/
/********************************  DoConv  ***************************/
/*********************************************************************/
long4
DoConv(long4 num, long4 den, int convResolution)
{
  /*register*/ double conv;
  conv = ((double)num / (double)den) *
    ((double)mag / 1000.0) *
    ((double)convResolution/254000.0);

  return((long4)((1.0/conv)+0.5));
}
/*------------------------ end dviconv.c -------------------------------*/


/*------------------------ begin dvimisc.c -----------------------------*/
/*-->AllDone*/
/**********************************************************************/
/****************************** AllDone  ******************************/
/**********************************************************************/
void AllDone(bool PFlag)
{
#ifdef TIMING
  double  time;
#endif

  if (DoublePage && (PageParity==1)) { /* Shall we go around again?*/
    int k;
    char    n[STRSIZE];

    if (PrintEmptyPages && EvenPage && Reverse && PrintFirstPart) {
      WouldPrint ++;
      qfprintf(ERR_STREAM,"[EvenPage] ");
      FormFeed();
    }
#ifdef LJ
    Reverse = (bool)!Reverse;
#endif
    if (Reverse) {
      if (!PFlag) {
        FSEEK(dvifp, postambleptr, SEEK_SET);
        (void) NoSignExtend(dvifp, 1);
        ppagep = (long) NoSignExtend(dvifp, 4);
      }
      FSEEK(dvifp, ppagep, SEEK_SET);
    } else {
      FSEEK(dvifp,  14l, SEEK_SET);
      k = (int)NoSignExtend(dvifp, 1);
      GetBytes(dvifp, n, k);
    }

    if (PrintSecondPart) {
      if (PrintFirstPart) {
        qfprintf(ERR_STREAM,"\n----------------------starting second pass\n");
#ifdef LJ
        EMIT1("\033&l2H"); /* Manual Feed */
#endif
#ifdef IBM3812
        PMPout(6,"\326\001\305\300\326\252");
        /* set display; ring bell; stop; clear display */
        PMPflush;
#endif
      }

      if (PrintEmptyPages && Reverse) {
        if (ZeroPage) WouldPrint++;
        if ((WouldPrint%2) == 1) {
          qfprintf(ERR_STREAM,"[Padding] ");
          FormFeed();
        }
      }
      WouldPrint = 0;
      if (PrintEmptyPages && !Reverse && ZeroPage) {
        WouldPrint++;
        qfprintf(ERR_STREAM,"[ZeroPage] ");
        FormFeed();
      }
      PageParity = 0;
      PrintPages = StartPrintPages;
      return;
    }
  }

  if (EvenPage && DoublePage && !Reverse) WouldPrint++;

  if (PrintEmptyPages && DoublePage && PrintSecondPart) {
    if (Reverse) {
      if (ZeroPage) {
        WouldPrint ++;
        qfprintf(ERR_STREAM,"[ZeroPage] ");
        FormFeed();
      }
    }
    else if ((WouldPrint%2) != 0) {
      qfprintf(ERR_STREAM,"[Padding] ");
      FormFeed();
    }
  }

#ifdef IBM3812
  PMPout(10, "\373\010PMP.init");  /* re-init printer  */
  PMPflush;

  if (used_fontstorage > MAXFONTSTORAGE) {
    Warning("\n\7used font_storage of %s: %ld Bytes (of %ld)\7",
            PRINTER, (long)used_fontstorage, MAXFONTSTORAGE);
    Warning("Try to format file in separate runs!");
  } else
    qfprintf(ERR_STREAM,"\nAll done, used font_storage of %s: %ld Bytes (of %ld)",
              PRINTER, (long)used_fontstorage, MAXFONTSTORAGE);
#endif
#ifdef LJ
# ifdef SHARP_JX_9500
  if (used_fontstorage > MAXFONTSTORAGE) {
    Warning("\n\7used font_storage of %s: %ld Bytes (of %ld)\7",
            PRINTER, (long)used_fontstorage, MAXFONTSTORAGE);
    Warning("Try to format file in separate runs!");
  } else
    qfprintf(ERR_STREAM,"\nAll done, used font_storage of %s: %ld Bytes (of %ld)",
              PRINTER, (long)used_fontstorage, MAXFONTSTORAGE);
# else
  qfprintf(ERR_STREAM,"\nAll done, used font_storage of %s: %ld Bytes",
            PRINTER, (long)used_fontstorage);
# endif
#ifdef LJ_RESIDENT_FONTS
  qfprintf(ERR_STREAM," + %d resident font%s", resident_count,
           resident_count == 1 ? "" : "s");
#endif
  EMIT1("\033E");
#ifdef LJ4
  EMIT1("\033%%-12345X");   /* what does it? */
#endif
  if (PrintTestPage) EMIT1("\033z");
#ifdef vms
  /* last record is not flushed to file, unless it is completely filled */
  for (kk = (int)((*outfp)->_cnt); kk > 0; --kk)
    putc('\0',outfp);
  fflush(outfp);
#endif
#endif

  if (!G_quiet) {
    fprintf(ERR_STREAM,"\nDynamically allocated storage: %ld Bytes \n",
            (long)allocated_storage);
    fprintf(ERR_STREAM,"%d characters downloaded as soft fonts\n", G_ncdl);

#ifdef TIMING
#ifdef BSD_TIME_CALLS
    ftime(&timebuffer);
    time = (timebuffer.time + (float)(timebuffer.millitm)/1000.0) - start_time;
#else
    gettimeofday(&Tp, NULL);
    time = (Tp.tv_sec + (float)(Tp.tv_usec)/1000000.0) - start_time;
#endif

    if (ndone > 0) {
      fprintf(ERR_STREAM,
              "Time of complete run: %.2f seconds, %d page(s), %.2f seconds/page",
              time, ndone, time / ndone);
      fprintf(ERR_STREAM," (%.2f ppm)\n",(ndone * 60) / time);
    }
    fprintf(ERR_STREAM,"\n");
#endif
  }
  CloseFiles();
  if ( tmp_dir[0] != '\0' )
    rmdir (tmp_dir);			/* ignore errors */
  exit(G_errenc);
}

/*-->DoSpecial*/
/*********************************************************************/
/*****************************  DoSpecial  ***************************/
/*********************************************************************/

typedef enum {
  ORIENTATION,
  RESETPOINTS,
  DEFPOINT,
  FILL,
  GRAY,
  my_PATTERN,
  COMMENT,
  HPFILE,
  HPFILE_VERBATIM,
  PSFILE_SYNTAX,
  PSFILE,
  LLX,
  LLY,
  URX,
  URY,
  RWI,
  RHI
} SpecialKeywords;

KeyDesc KeyTab[] = {
  { ORIENTATION, "orientation", Integer},
  { RESETPOINTS, "resetpoints", String},
  { DEFPOINT, "defpoint", String},
  { FILL, "fill", String},
  { GRAY, "gray", Integer},
  { GRAY, "grey", Integer},
  { my_PATTERN, "pattern", Integer},
  { COMMENT, "comment", String},
  { HPFILE, "hpfile", String},
  { HPFILE_VERBATIM, "hpfile-verbatim", String},
  { PSFILE_SYNTAX, "dvilj-psfile-syntax", String },
  { PSFILE, "psfile", String },
  { LLX, "llx", Integer},
  { LLY, "lly", Integer},
  { URX, "urx", Integer},
  { URY, "ury", Integer},
  { RWI, "rwi", Integer},
  { RHI, "rhi", Integer}
  /*,
    {"hsize", Dimension},
    {"vsize", Dimension},
    {"hoffset", Dimension},
    {"voffset", Dimension},
    {"hscale", Number},
    {"vscale", Number}*/
};

#define NKEYS (sizeof(KeyTab)/sizeof(KeyTab[0]))

#ifdef __riscos
# ifdef LJ

/* Compare two strings, ignoring case;
   s1 pointer to null-terminated keyword, s2 pointer to parseline;
   returns (if successful) pointer to character following keyword in s2 */
bool StrCompare(char *s1, char *s2, char **end)
{
  char *a,*b;

  a = s1;
  b = s2;
  while (*a != '\0') {
    if (tolower(*a) != tolower(*b)) return(_FALSE);
    a++;
    b++;
  }
  *end = b;
  return(_TRUE);
}

/* Read <number> integer values from string and store results in
   <result>. Returns number + of arguments actually read, end =
   pointer to char following last number */
int
ParseNumbers(char *str, int *result, int number, char **end)
{
  char *s;
  int count = 0;

  s = str;
  while ((*s != '\0') && (count < number)) {
    while ((*s == ' ') || (*s == ',') || (*s == '='))
      s++;
    if (*s != '\0') {
      result[count++] = strtod(s,&s);
    }
  }
  while ((*s == ' ') || (*s == ',') || (*s == '='))
    s++;
  *end = s;
  return(count);
}


/* Diagram commands are parsed separately since the format varies from the one
   used by the other special commands.  */
bool ParseDiagram(char *str)
{
  diagtrafo dt;
  char *s,*sh;
  char diagname[STRSIZE];
  int results[4],no;

  s = str;
  while (*s == ' ')
    s++;
  if ((StrCompare("drawfile",s,&s)) || (StrCompare("DVIview_diagram",s,&s))) {

    if (printdiag == _FALSE)
      return(_TRUE); /* it's a diagram, but don't print */

    while ((*s == ' ') || (*s == '='))
      s++; /* space or '=' separates keyword/keyval */

    if (*s == '\0') {
      fprintf(ERR_STREAM,"No filename given for \\special-diagram!\n");
      return(_TRUE);
    }
    sh = diagname;
    while ((*s != ' ') && (*s != ',') && (*s != '\0'))
      *sh++ = *s++;
    *sh = '\0';

    /* Set up default values */
    dt.scalex = dt.scaley = 100;
    dt.cropl = dt.cropb = dt.cropr = dt.cropt = 0;
    while (*s != '\0') {
      while ((*s == ' ') || (*s == ','))
        s++;
      if (*s != '\0') {
        if (StrCompare("scale",s,&s)) {
          if ((no = ParseNumbers(s,results,2,&s)) != 2) {
            fprintf(ERR_STREAM,
                   "Too few arguments (%d) given for <scale> - ignored.\n",no);
          }
          dt.scalex = results[0];
          dt.scaley = results[1];
        }
        else if (StrCompare("crop",s,&s)) {
          if ((no = ParseNumbers(s,results,4,&s)) != 4) {
            fprintf(ERR_STREAM,
                   "Too few arguments (%d) given for <crop> - ignored.\n",no);
          }
          dt.cropl = results[0];
          dt.cropr = results[1];
          dt.cropt = results[2];
          dt.cropb = results[3];
        }
        else {
          fprintf(ERR_STREAM,"Bad \\special keyword - <%s> ignored\n",s);
          /* skip over this word */
          while ((*s != ' ') && (*s != ',') && (*s != '=') && (*s != '\0'))
            s++;
        }
      }
    }
    /* fprintf(ERR_STREAM,"Diagram: %s, scale %d %d, crop %d %d %d %d\n",
       diagname,dt.scalex,dt.scaley,dt.cropl,dt.cropb,dt.cropr,dt.cropt);*/
    diagram(diagname,&dt);
    return(_TRUE);
  }
  else
    return(_FALSE);
}
# endif /* LJ */
#endif /* __riscos */


#ifndef HAVE_MKDTEMP
/* If mkdtemp() is not available, supply an (unsecure) version of it. We try
   to use mktemp() to get the temporary directory name, since that maps best
   to mkdtemp() behavior. But if mktemp() is also not available, we resort
   to tmpnam() which is supposed to be there from the C standard. */
#ifndef HAVE_MKTEMP
#define mktemp tmpnam
#endif
static char * mkdtemp ( char * template )
{
  if ( mktemp(template) == NULL  ||  template[0] == '\0' ) {
    if ( errno == 0 )  errno = EINVAL;	/* if it's tmpnam() */
    return NULL;
  }
#ifdef WIN32
#undef mkdir
#define mkdir(path, mode) mkdir(path)
#endif
  if ( mkdir(template, 0700) == -1 ) {
    return NULL;
  }
  return template;
}
#endif


/* interpret a \special command, made up of keyword=value pairs */
void DoSpecial(char *str, int n)
{
  bool	  first_keyword = _TRUE;
  char    xs[STRSIZE], ys[STRSIZE];
  char    *include_file = NULL;
  enum    { None, VerbFile, HPFile, PSFile } file_type = None;
  float   x,y;
  long4   x_pos, y_pos;
  KeyWord k;
  int     i, j, j1;
  static  int   GrayScale = 10, Pattern = 1;
  static  bool  GrayFill = _TRUE;
  static  long4 p_x[MAX_SPECIAL_DEFPOINTS], p_y[MAX_SPECIAL_DEFPOINTS];
  static  bool  need_init_pxy = _TRUE;
  int llx=0, lly=0, urx=0, ury=0, rwi=0;

  str[n] = '\0';
  if ( need_init_pxy ) {
    for ( i=0 ; i<MAX_SPECIAL_DEFPOINTS ; i++ )
      p_x[i] = p_y[i] = -1;
    need_init_pxy = _FALSE;
  }

  SetPosn(h, v);
#ifdef __riscos
#ifdef LJ
  if (ParseDiagram(str))
    return;
#endif
#endif

  /* When the first keyword is already unknown, we skip the whole special.
     This is probably one for another driver and the user got notified
     already about the problem. */
  while ( (str = GetKeyStr(str, &k)) != NULL ) {
    /* get all keyword-value pairs */
    if ( k.vt == None ) {		/* no value */
      /* Single word might also be "comment", then ignore the rest */
      if ( EQ(k.Key, "comment") )
	return;
      /* For compatibility, single words are taken as file names. But then
	 the include file must be accessible without being searched with
	 kpathsea. */
      if ( access(k.Key, 0) == 0 ) {
	if ( include_file && !kpse_tex_hush ("special") ) {
	  Warning("More than one \\special file name given, %s ignored", include_file);
	  free (include_file);
	}
	include_file = xstrdup(k.Key);
	file_type = VerbFile;
      } else {
	if ( !kpse_tex_hush ("special") )
	  Warning("Invalid keyword or value in \\special - <%s> ignored", k.Key);
	if ( first_keyword )
	  return;
      }
    } else if ( GetKeyVal( &k, KeyTab, NKEYS, &i ) && i != -1 ) {
      switch (i) {

      case ORIENTATION:
#ifdef IBM3812
        if ((k.v.i >= 0) && (k.v.i < 4)) {
          last_ry = UNKNOWN;
          sprintf(PMPformat, "\322%c", (unsigned char)k.v.i);
          PMPout(2, PMPformat);
          if (k.v.i == 1) Landscape = _TRUE;
          else if (k.v.i == 0) Landscape = _FALSE;
        }
#endif
#ifdef LJ
        if ( k.v.i >= 0 && k.v.i <= 3 ) {
          last_rx = last_ry = UNKNOWN;
          EMIT2("\033&l%dO\033*rF", (unsigned char)k.v.i);
        }
#endif
        else {
	  Warning( "Invalid orientation (%d) given; ignored.", k.v.i);
	}
        break;

      case RESETPOINTS:
	for ( i=0 ; i<MAX_SPECIAL_DEFPOINTS ; i++ )
	  p_x[i] = p_y[i] = -1;
        break;

      case DEFPOINT:
	/* 254 is STRSIZE-1. cpp should be used to construct that number. */
        i = sscanf(k.Val,"%d(%254[^,],%254s)",&j,xs,ys);
        if (i>0) {
	  if ( j < 0  ||  j >= MAX_SPECIAL_DEFPOINTS ) {
	    Warning ("defpoint %d ignored, must be between 0 and %d",
		     j, MAX_SPECIAL_DEFPOINTS-1);
	    break;
	  }
          x_pos = h;
          y_pos = v;
          if ( i > 1  &&  sscanf(xs,"%fpt",&x) > 0 )
              x_pos = PT_TO_DVI(x);
          if ( i > 2  &&  sscanf(ys,"%fpt",&y) > 0 )
              y_pos = PT_TO_DVI(y);
          p_x[j]=x_pos;
          p_y[j]=y_pos;
        } else {
	  Warning("invalid point definition");
	}
        break;

      case FILL:
	/* 254 is STRSIZE-1. cpp should be used to construct that number. */
        i = sscanf(k.Val,"%d/%d",&j,&j1);
        if (i>1) {
#ifdef LJ
	  if ( j < 0 || j >= MAX_SPECIAL_DEFPOINTS ) {
	    Warning ("fill ignored, point %d must be between 0 and %d",
		     j, MAX_SPECIAL_DEFPOINTS);
	    break;
	  }
	  if ( p_x[j] == -1 ) {
	    Warning ("fill ignored, point %d is undefined\n", j);
	    break;
	  }
	  if ( j1 < 0 || j1 >= MAX_SPECIAL_DEFPOINTS ) {
	    Warning ("fill ignored, point %d must be between 0 and %d",
		     j1, MAX_SPECIAL_DEFPOINTS);
	    break;
	  }
	  if ( p_x[j1] == -1 ) {
	    Warning ("fill ignored, point %d is undefined\n", j1);
	    break;
	  }
          SetPosn(p_x[j], p_y[j]);
          x_pos = (long4)PIXROUND(p_x[j1]-p_x[j], hconv);
          y_pos = (long4)PIXROUND(p_y[j1]-p_y[j], vconv);
          if (labs(x_pos)<labs(y_pos)) x_pos = x_pos+3;
          else                         y_pos = y_pos+3;
          if (GrayFill) {
            EMIT4("\033*c%lda%ldb%dg2P", (long)x_pos, (long)y_pos, GrayScale);
          } else {
            EMIT4("\033*c%lda%ldb%dg3P", (long)x_pos, (long)y_pos, Pattern);
          }
          last_rx = last_ry = UNKNOWN;
#endif
        }
        break;

      case GRAY:
        if ((k.v.i >= 0) && (k.v.i < 101)) {
          GrayScale = k.v.i;
          GrayFill = _TRUE;
        } else {
          Warning( "Invalid gray scale (%d) given; ignored.", k.v.i);
	}
        break;

      case my_PATTERN:
        if ((k.v.i >= 0) && (k.v.i < 7)) {
          Pattern = k.v.i;
          GrayFill = _FALSE;
        } else {
          Warning( "Invalid pattern (%d) given; ignored.", k.v.i);
	}
        break;

      case COMMENT:
	return;
	/*NOTREACHED*/
	break;

      case HPFILE:
        if ( include_file && !kpse_tex_hush ("special") ) {
	  Warning("More than one \\special file name given. %s ignored", include_file);
	  free(include_file);
	}
        include_file = xstrdup(k.Val);
	file_type = HPFile;
        break;

      case HPFILE_VERBATIM:
        if ( include_file && !kpse_tex_hush ("special") ) {
	  Warning("More than one \\special file name given. %s ignored", include_file);
	  free(include_file);
	}
        include_file = xstrdup(k.Val);
	file_type = VerbFile;
        break;

      case PSFILE_SYNTAX:
	if ( EQ(k.Val, "ignore") )
	  PSFileSyntaxTyp = Ignore;
	else if ( EQ(k.Val, "dvilj") )
	  PSFileSyntaxTyp = PSFile_dvilj;
	else
	  Warning("Ignored invalid value '%s' for dvilj-psfile-syntax", k.Val);
	break;

      case PSFILE:
        if ( include_file ) {
	  Warning("More than one \\special file name given. %s ignored", include_file);
	  free(include_file);
	}
	if ( PSFileSyntaxTyp != Ignore ) {
	  include_file = xstrdup(k.Val);
	  file_type = PSFile;
	} else {
	  include_file = NULL;
	}
        break;

      case LLX: llx = k.v.i; break;
      case LLY: lly = k.v.i; break;
      case URX: urx = k.v.i; break;
      case URY: ury = k.v.i; break;
      case RWI: rwi = k.v.i; break;
      case RHI:
	if (!kpse_tex_hush ("special"))
	  Warning("Whatever rhi was good for once, it is ignored now.");
	break;

      default:
	if ( !kpse_tex_hush ("special") )
	  Warning("Can't handle %s=%s command; ignored.", k.Key, k.Val);
	if ( first_keyword )
	  return;
        break;
      }

    } else {
      if ( !kpse_tex_hush ("special") )
	Warning("Invalid keyword or value in \\special - <%s> ignored", k.Key);
      if ( first_keyword )
	return;
    }

    free (k.Key);
    if ( k.Val != NULL )  free(k.Val);
    first_keyword = _FALSE;
  }

  if ( include_file ) {
    char * include_path;
    last_rx = last_ry = UNKNOWN;
#ifdef IBM3812
    PMPflush;
#endif

    /* Search the include file with kpathsea, but don't open it immediately.
       If it's a psfile special, it will get passed to Ghostscript. An
       include file is often placed at the same place as documents: We use
       both a program specific search path and the tex search path. We can
       not use kpse_find_tex() as it would call mktex, which we don't want
       to do for included PCL files. */
    if ( (include_path = kpse_find_file(include_file, kpse_program_binary_format, false)) == NULL &&
	 (include_path = kpse_find_file(include_file, kpse_tex_format, false)) == NULL ) {
      Warning ("Could not locate %s, ignoring inclusion special", include_file);
      file_type = None;
    } else {
      free (include_file);
      include_file = include_path;
    }

#ifdef LJ
    if ( file_type == PSFile) {
      /* int height = rwi * (urx - llx) / (ury - lly);*/
      int width  = urx - llx;
      int height = ury - lly;
      char cmd[255];
      const char *cmd_format = "%s -q -dSIMPLE -dSAFER -dNOPAUSE -sDEVICE=%s -sOutputFile=%s %s %s showpage.ps -c quit";
      const char *gs_cmd;
      int scale_factor, adjusted_height, adjusted_llx;
      const char *printer = "ljetplus"; /* use the most stupid one */

      char pcl_file[STRSIZE];
      char scale_file[STRSIZE];
      FILEPTR scalef;

      if ( urx == 0 || ury == 0 || rwi == 0 ) {
	/* Since dvips' psfile special has a different syntax, this might
	   well be one of those specials, i.e., a non-dviljk special. Then
	   the Warning should be suppressable. */
	if ( !kpse_tex_hush ("special") )
	  Warning ("Ignoring psfile special without urx, ury and rwi attributes");
	free (include_file);
	return;
      }
      scale_factor    = 3000 * width / rwi;
      adjusted_height = height * 300/scale_factor;
      adjusted_llx    = llx    * 300/scale_factor;

      /* We cannot use mkstemp, as we cannot pass two open file descriptors
	 portably to Ghostscript. We don't want to use tmpnam() or tempnam()
	 either, as they have tempfile creation race conditions. Instead we
	 create a temporary directory with mkdtemp().
	 We need to create the temporary directory only once per
	 run; it will be deleted in AllDone(). */
      if ( tmp_dir[0] == '\0' ) {
	const char * base_dir;
	if ( (base_dir = getenv("TMPDIR")) == NULL ) {
	  base_dir = "/tmp";
	} else if ( strlen(base_dir) > STRSIZE - sizeof("/dviljkXXXXXX/include.pcl") ) {
	  Warning ("TMPDIR %s is too long, using /tmp instead", base_dir);
	  base_dir = "/tmp";
	}
	/* FIXME: Actually, we would need a function to sanitize base_dir here.
	   There may still be constructs like /.. or similar. [03 Jul 07 -js] */
	if ( base_dir[0] == '/'  && base_dir[1] == '\0' ) {
	  Warning ("Feeling naughty, do we? / is no temporary directory, dude");
	  base_dir = "/tmp";
	}
	strcpy (tmp_dir, base_dir);
	strcat (tmp_dir, "/dviljkXXXXXX");
	if ( mkdtemp(tmp_dir) == NULL ) {
	  Warning ("Could not create temporary directory %s, errno = %d; ignoring include file special",
		   tmp_dir, errno);
	  return;
	}
      }
      strcpy(pcl_file, tmp_dir);
      strcat(pcl_file, "/include.pcl");
      strcpy(scale_file, tmp_dir);
      strcat(scale_file, "/scale.ps");

      if ( (scalef = BOUTOPEN(scale_file)) == FPNULL ) {
	Warning("Unable to open file %s for writing", scale_file );
	free (include_file);
	unlink(scale_file);		/* ignore error */
	return;
      }
      fprintf(scalef, "%.2f %.2f scale\n%d %d translate\n",
	      300.0/scale_factor, 300.0/scale_factor,
	      0, adjusted_height == height ? 0 : ury);
      BCLOSE( scalef );

#ifdef WIN32
      if ( (gs_cmd = getenv("GS_PATH")) == NULL )
	gs_cmd = "gswin32c.exe";
#else
      gs_cmd = "gs";
#endif
      if ( strlen(cmd_format)-10 + strlen(gs_cmd) + strlen(printer) +
	       strlen(pcl_file) + strlen(scale_file) + strlen(include_file) +1 >
	   sizeof(cmd) ) {
	Warning ("Ghostscript command for %s would be too long, skipping special", include_file);
	free (include_file);
	unlink(scale_file);		/* ignore errors */
	unlink(pcl_file);
	return;
      }
      sprintf(cmd, cmd_format,
	      gs_cmd, printer, pcl_file, scale_file, include_file);
#ifdef DEBUGGS
      fprintf(stderr,
	"PS-file '%s' w=%d, h=%d, urx=%d, ury=%d, llx=%d, lly=%d, rwi=%d\n",
	      include_file, urx - llx, height, urx,ury,llx,lly, rwi);
      fprintf(stderr,"%s\n",cmd);
#endif
      if (system(cmd)) {
	Warning("execution of '%s' returned an error", cmd);
      } else {
#ifdef DEBUGGS
	fprintf(stderr, "o=%d, h=%d, so=%d, sh=%d\n",
		llx, height, adjusted_llx, adjusted_height);

	fprintf(stderr, "OLD x=%d, y=%d\n",
		(int)PIXROUND(h, hconv) + x_goffset,
		(int)PIXROUND(v, vconv) + y_goffset);
#endif
	v -= 65536l*adjusted_height; /**300/scale_factor;*/
	h -= 65536l*adjusted_llx; /* *300/scale_factor;*/
	SetPosn(h, v);
#ifdef DEBUGGS
	fprintf(stderr, "NEW x=%d, y=%d\n",
		(int)PIXROUND(h, hconv) + x_goffset,
		(int)PIXROUND(v, vconv) + y_goffset);
#endif

	CopyHPFile( pcl_file );
      }
      unlink(scale_file);		/* ignore errors */
      unlink(pcl_file);
    }
    else
#endif /* LJ */

    if ( file_type == HPFile )
      CopyHPFile( include_file );
    else if ( file_type == VerbFile )
      my_CopyFile( include_file );
    else if ( file_type == None )
      /* do nothing */ ;
    else
      Warning ("This can't happen: unknown file_type value %d", file_type);

    if ( include_file != NULL )  free(include_file);
  }
}




/*-->GetKeyStr*/
/**********************************************************************/
/*****************************  GetKeyStr  ****************************/
/**********************************************************************/
/* Extract first keyword-value pair from string (value part may be null),
 * keyword and value are allocated and must be free by caller.
 * Return pointer to remainder of string,
 * return NULL if none found.
 */
char *GetKeyStr(char *str, KeyWord *kw )
{
  char *s, *start;
  char save_char, quote_char;
  if ( !str )
    return( NULL );
  for (s = str; *s == ' '; s++)
    ;          /* skip over blanks */
  if (*s == '\0')
    return( NULL );
  start = s++;				/* start of keyword */
  while ( *s != ' ' && *s != '\0' && *s != '=' )  /* locate end */
    s++;
  save_char = *s;
  *s = '\0';
  kw->Key = xstrdup(start);
  kw->Val = NULL;
  kw->vt = None;
  if ( save_char == '\0' )		/* shortcut when we're at the end */
    return (s);
  *s = save_char;			/* restore keyword end char */
  while ( *s == ' ' ) s++ ;		/* skip over blanks */
  if ( *s != '=' )			/* no "=" means no value */
    return( s );
  for (s++; *s == ' '; s++)
    ;					/* skip over blanks */
  if ( *s == '\'' || *s == '\"' )	/* get string delimiter */
    quote_char = *s++;
  else
    quote_char = ' ';
  start = s;				/* no increment, might be "" as value */
  while ( *s != quote_char && *s != '\0' )
    s++;			  /* locate end of value portion */
  save_char = *s;
  *s = '\0';
  kw->Val = xstrdup(start);
  kw->vt = String;
  if ( save_char != '\0' ) {		/* save_char is now quote_char */
    *s = save_char;
    if ( quote_char != ' ' )		/* we had real quote chars */
      s++;
  }
  return( s );
}


/*-->GetKeyVal*/
/**********************************************************************/
/*****************************  GetKeyVal  ****************************/
/**********************************************************************/
/* get next keyword-value pair decode value according to table entry  */
bool GetKeyVal(KeyWord *kw, KeyDesc tab[], int nt, int *tno)
{
  int     i;
  char    c = '\0';
  *tno = -1;
  for (i = 0; i < nt; i++)
    if ( IsSame(kw->Key, tab[i].Entry) ) {
      *tno = tab[i].KeyId;
      switch ( tab[i].Typ ) {
      case None:
        if ( kw->vt != None )
          return( _FALSE );
        break;
      case String:
        if ( kw->vt != String )
          return( _FALSE );
        break;
      case Integer:
        if ( kw->vt != String )
          return( _FALSE );
        if ( sscanf(kw->Val, "%d%c", &(kw->v.i), &c) != 1 || c != '\0' )
          return( _FALSE );
        break;
      }
      kw->vt = tab[i].Typ;
      return( _TRUE );
    }
  return( _TRUE );
}


/*-->IsSame*/
/**********************************************************************/
/*******************************  IsSame  *****************************/
/**********************************************************************/
/* compare strings, ignore case */
bool IsSame(const char *a, const char *b)
{
  const char *x, *y;

  for (x = a, y = b; *a; a++, b++)
    if ( tolower(*a) != tolower(*b) )
      return( _FALSE );

  return( *a == *b ? _TRUE : _FALSE );
}


/*-->FindPostAmblePtr*/
/**********************************************************************/
/************************  FindPostAmblePtr  **************************/
/**********************************************************************/
/* this routine will move to the end of the file and find the start
    of the postamble */
void FindPostAmblePtr(long *postambleptr)
{
  long4    i;
  FSEEK(dvifp,  0l, SEEK_END);   /* goto end of file */
  *postambleptr = FTELL(dvifp) - 4;
  FSEEK(dvifp, *postambleptr, SEEK_SET);
  while (_TRUE) {
    FSEEK(dvifp, --(*postambleptr), SEEK_SET);
    if (((i = NoSignExtend(dvifp, 1)) != 223) &&
        (i != DVIFORMAT))
      Fatal("Bad end of DVI file");
    if (i == DVIFORMAT)
      break;
  }
  FSEEK(dvifp, (*postambleptr) - 4, SEEK_SET);
  (*postambleptr) = NoSignExtend(dvifp, 4);
  FSEEK(dvifp, *postambleptr, SEEK_SET);
}


/*-->ReadPostAmble*/
/**********************************************************************/
/**************************  ReadPostAmble  ***************************/
/**********************************************************************/
/***********************************************************************
    This routine is used to read in the postamble values.  It
    initializes the magnification and checks the stack height prior to
    starting printing the document.
***********************************************************************/
void ReadPostAmble(bool load)
{
  long4 den, num; /* denominator and numerator */
  FindPostAmblePtr(&postambleptr);
  if (NoSignExtend(dvifp, 1) != POST)
    Fatal("POST missing at head of postamble");
#ifdef DEBUG
  if (Debug)
    fprintf(ERR_STREAM,"got POST command\n");
#endif
  ppagep = (long)NoSignExtend(dvifp, 4);
  num = NoSignExtend(dvifp, 4);
  den = NoSignExtend(dvifp, 4);
  mag = NoSignExtend(dvifp, 4);
  if ( usermag > 0 && usermag != mag )
    Warning("DVI magnification of %ld over-ridden by user (%ld)",
            (long)mag, usermag );
  if ( usermag > 0 )
    mag = usermag;
  hconv = DoConv(num, den, hconvRESOLUTION);
  vconv = DoConv(num, den, vconvRESOLUTION);
  (void) NoSignExtend(dvifp, 4);   /* height-plus-depth of tallest page */
  (void) NoSignExtend(dvifp, 4);   /* width of widest page */
  if (NoSignExtend(dvifp, 2) >= STACK_SIZE)
    Fatal("Stack size is too small");
  (void) NoSignExtend(dvifp, 2);   /* this reads the number of pages in */
  /* the DVI file */
#ifdef DEBUG
  if (Debug)
    fprintf(ERR_STREAM,"now reading font defs");
#endif
  if (load)
    GetFontDef();
}

#ifdef IBM3812
/*-->PMPLine*/
/**********************************************************************/
/*****************************  PMPLine  ******************************/
/**********************************************************************/
/* drawing lines on the 3812 using PMP vector commands */
void PMPLine(int w, int y, int x)
{

  if ((w == 0) || (x == 0 && y == 0))
    return;

  /*
    fprintf(ERR_STREAM,"w=%d / %d - %d, y=%d / %d - %d, x=%d / %d - %d\n",
    w,(char)(w & 0xff),(int)((signed_char)(w & 0xff)),
    y,(char)(y & 0xff),(int)((signed_char)(y & 0xff)),
    x,(char)(x & 0xff),(int)((signed_char)(x & 0xff)));
    */

  if ( (((signed_char)(x & 0xff)) == x ) &&
       ( ((signed_char)(y & 0xff)) == y ) ) {
    PMPcont(6);
    PMPout(1, "\370");
    EMITWORD(3);      /* length of vector */
    PMPoutC((unsigned char)(0x80 | 0x00 | (unsigned char) w));
    PMPoutC((signed_char)(y & 0xff));
    PMPoutC((signed_char)(x & 0xff));
    /*
      fprintf(ERR_STREAM,"F8 00 03: w=%d, x=%d(%d-%.2X), y=%d(%d-%.2X),\n",
      w,x,(char)(x & 0xff),(signed_char)(x & 0xff),
      y,(char)(y & 0xff),(signed_char)(y & 0xff));
      */

  } else {
    PMPcont(8);
    PMPout(1, "\370");
    EMITWORD(4 + 1);      /* length of vector */
    PMPoutC((unsigned char)(0xC0 | 0x00 | (unsigned char) w));
    EMITWORD(y);
    EMITWORD(x);
    /*
      fprintf(ERR_STREAM,"F8 00 05: w=%d, x=%d, y=%d,\n", w,x,y);
      */
  }
}


#endif
/*-->SetRule*/
/**********************************************************************/
/*****************************  SetRule  ******************************/
/**********************************************************************/
/*   this routine will draw a rule */
void SetRule(long4 a, long4 b, int Set)
{
  long4    xx, yy;
#ifdef IBM3812
  short   hor_offset, vert_offset, ll;
#endif
  if ( a > 0 && b > 0 ) {
    SetPosn(h, v);             /* lower left corner */
    xx = (long4)PIXROUND(b, hconv);     /* width */
    yy = (long4)PIXROUND(a, vconv);     /* height */

#ifdef DEBUG
    if (Debug)
      fprintf(ERR_STREAM,"Rule xx=%ld, yy=%ld\n", (long)xx, (long)yy);
#endif

#ifdef IBM3812
    hor_offset  = (short)(last_ry - yy);
    if (hor_offset < 0) yy += hor_offset;
    if (last_rx < 0) xx += last_rx;

    if (Landscape) {
      if (last_ry > MAX_PAGE_WIDTH) yy += MAX_PAGE_WIDTH-last_ry;
      hor_offset  = (short)(MAX_PAGE_HEIGHT - (last_rx + xx));
    } else {
      if (last_ry > MAX_PAGE_HEIGHT) yy += MAX_PAGE_HEIGHT-last_ry;
      hor_offset  = (short)(MAX_PAGE_WIDTH - (last_rx + xx));
    }
    if (hor_offset < 0) xx += hor_offset;

    if ((xx > 31) && (yy > 31)) {
      /*
       *   fill area by multiple lines  (kind of a mess)
       *   process for simplicity always horizontally
       */

      /*
         fprintf(ERR_STREAM,"large box: w=%d,x=%d,y=%d\n",(int)yy,(int)xx,0);
         */

      hor_offset  = HOR_HALF(30);
      MoveHor(hor_offset);
      vert_offset = VERT_HALF(30);
      MoveVert(-vert_offset);
      ll = (short)xx - 30;

      for (; yy > 30; yy -= 30) {
        PMPLine(30, 0, ll);
        MoveHor(-ll);
        MoveVert(-30);
      }

      hor_offset  = -hor_offset     + HOR_HALF(yy);
      MoveHor(hor_offset);
      vert_offset = (vert_offset - 30) + VERT_HALF(yy);
      MoveVert(-vert_offset);

      PMPLine((int)yy, 0, (int)(xx - yy));

    } else if ( (yy < xx) && (xx > 0) ) {

      /* fprintf(ERR_STREAM, "hori rule: w=%d, x=%d, y=%d\n",(int)yy,(int)(xx-yy),0);*/

      hor_offset  = HOR_HALF(yy);
      vert_offset = VERT_HALF(yy);

      MoveHor(hor_offset);
      MoveVert(-vert_offset);

      PMPLine((int)yy, 0, (int)(xx - yy));
    } else if ( (xx < yy) && (yy > 0)) {

      hor_offset  = HOR_HALF(xx);
      vert_offset = VERT_HALF(xx);
      /*
        fprintf(ERR_STREAM, "move: x=%d, y=%d\n",hor_offset,-vert_offset);
        fprintf(ERR_STREAM, "vert rule: w=%d, x=%d, y=%d\n",(int)xx,0,(int)-(yy-xx));
        */
      MoveHor(hor_offset);
      MoveVert(-vert_offset);

      PMPLine((int)xx, (int)-(yy - xx), 0);
    } else if (xx == yy) {
      short     y0;  /* small square box!! */

      y0 = (short)yy / 2;
      hor_offset  = HOR_HALF(y0);
      MoveHor(hor_offset);
      vert_offset = VERT_HALF(y0);
      MoveVert(-vert_offset);
      ll = (short)xx - y0;

      PMPLine((int)y0, 0, ll);

      hor_offset  = - (ll + hor_offset);
      vert_offset = (y0 - vert_offset);

      yy -= (long4)y0;
      hor_offset  += HOR_HALF(yy);
      MoveHor(hor_offset);
      vert_offset += VERT_HALF(yy);
      MoveVert(-vert_offset);

      PMPLine((int)yy, 0, (int)xx - yy);
    }
#endif
#ifdef LJ
    if (last_ry + 1 < yy) yy = last_ry + 1;
    if (last_rx < 0) xx += last_rx;

    if ((int)pgsiz_dots > 0 && (int)last_ry > (int)pgsiz_dots)
      yy += (long4)pgsiz_dots - (long4)last_ry;

    if ((yy>0) && (xx>0))
      EMIT4("\033*p-%ldY\033*c%lda%ldbP",
            (long)yy - 1, (long)xx, (long)yy);
#endif
    last_rx = last_ry = UNKNOWN;       /* next time full positioning */
  }
  if (Set)
    h += b;
}



/*-->FormFeed*/
/**********************************************************************/
/*****************************  FormFeed ******************************/
/**********************************************************************/
void FormFeed(void)
{

#ifdef IBM3812
  unsigned short pages;
  if ( ndone == 0 && FirstAlternate ){
    for (pages = 1; pages < ncopies; pages++) {
      PMPout(2, "\321\300"); /* PMP-command xD1C0 */
    }
    PMPout(2, "\321\100"); /* PMP-command xD140 */
  } else {
    for (pages = 1; pages < ncopies; pages++){
      PMPout(2, "\321\200"); /* PMP-command xD180 */
    }
    PMPout(2, "\321\0"); /* PMP-command xD100 */
  }
#endif
#ifdef LJ
  EMITC('\f');
#endif
}
/*------------------------ end dvimisc.c -------------------------------*/




/*------------------------ begin dvifont.c -----------------------------*/
/*-->GetFontDef*/
/**********************************************************************/
/**************************** GetFontDef  *****************************/
/**********************************************************************/
void GetFontDef(void)
/***********************************************************************
   Read the font  definitions as they  are in the  postamble of the  DVI
   file.
***********************************************************************/
{
  unsigned char   byte;
  while (((byte = (unsigned char) NoSignExtend(dvifp, 1)) >= FNT_DEF1) &&
         (byte <= FNT_DEF4)) {
    switch (byte) {
    case FNT_DEF1:
      ReadFontDef( NoSignExtend(dvifp, 1));
      break;
    case FNT_DEF2:
      ReadFontDef( NoSignExtend(dvifp, 2));
      break;
    case FNT_DEF3:
      ReadFontDef( NoSignExtend(dvifp, 3));
      break;
    case FNT_DEF4:
      ReadFontDef( NoSignExtend(dvifp, 4));
      break;
    default:
      Fatal("Bad byte value in font defs");
      break;
    }
  }
  if (byte != POST_POST)
    Fatal("POST_POST missing after fontdefs");
}




/*-->OpenFontFile*/
/**********************************************************************/
/************************** OpenFontFile  *****************************/
/**********************************************************************/
void OpenFontFile(void)
/***********************************************************************
    The original version of this dvi driver reopened the font file  each
    time the font changed, resulting in an enormous number of relatively
    expensive file  openings.   This version  keeps  a cache  of  up  to
    MAXOPEN open files,  so that when  a font change  is made, the  file
    pointer, pxlfp, can  usually be  updated from the  cache.  When  the
    file is not found in  the cache, it must  be opened.  In this  case,
    the next empty slot  in the cache  is assigned, or  if the cache  is
    full, the least used font file is closed and its slot reassigned for
    the new file.  Identification of the least used file is based on the
    counts of the number  of times each file  has been "opened" by  this
    routine.  On return, the file pointer is always repositioned to  the
    beginning of the file.
***********************************************************************/

#if MAXOPEN > 1

{
  int     i, least_used, current;
  struct pixel_list tmp;
  FILEPTR fid;
  struct font_entry *fp;

#ifdef DEBUG
  if (Debug)
    fprintf(ERR_STREAM,"open font file %s\n", fontptr->name);
#endif
  /*
    fprintf(ERR_STREAM,"? %lx == %lx\n", pfontptr,fontptr);
    */
  if ((pfontptr == fontptr) && (pxlfp != NO_FILE))
    return;         /* we need not have been called */

  if (fontptr->font_file_id == NO_FILE)
    return;         /* we need not have been called */

  tmp = pixel_files[1];
  current = 1;
  while (current <= nopen && tmp.pixel_file_id != fontptr->font_file_id) {
    ++current;
    tmp = pixel_files[current];
  }
  /* try to find file in open list */

  if (current <= nopen)       /* file already open */ {
    if ( pixel_files[current].pixel_file_id != NO_FILE ) {
      pxlfp = pixel_files[current].pixel_file_id;
      /* reposition to start of file */
      FSEEK(pxlfp, 0l, SEEK_SET);
    }
  } else {
    /* file not in open list          */
    if (nopen < MAXOPEN)    /* just add it to list    */
      current = ++nopen;
    else  {
      /* list full -- find least used file,     */
      /* close it, and reuse slot for new file  */
      least_used = 1;
      for (i = 2; i <= MAXOPEN; ++i)
        if (pixel_files[least_used].use_count > pixel_files[i].use_count)
          least_used = i;
      if ((fid = pixel_files[least_used].pixel_file_id) != NO_FILE) {
        /* mark file as being closed in the entry */
        fp = hfontptr;
        while (fp != NULL && fp->font_file_id != fid)
          fp = fp->next;
        if (fp == NULL)
          Fatal("Open file %x not found in font entry list.\n", fid);
        else {
          fp->font_file_id = FPNULL;
        }
        BCLOSE( fid );
      }
#ifdef DEBUG
      if (Debug)
        fprintf(ERR_STREAM,"\n__reuse slot %d\n", least_used);
#endif
      current = least_used;
    }
    if ((pxlfp = BINOPEN(fontptr->name)) == FPNULL) {
      Warning("PXL-file %s could not be opened", fontptr->name);
      pxlfp = NO_FILE;
    } else {
#ifdef DEBUG
      if (Debug)
        fprintf(ERR_STREAM,"Opening File  <%s> /%p/, Size(font_entry)=%d\n",
                fontptr->name, pxlfp, sizeof(struct font_entry ));
#endif

    }
    pixel_files[current].pixel_file_id = pxlfp;
    pixel_files[current].use_count = 0;
  }
  pfontptr = fontptr;         /* make previous = current font */
  fontptr->font_file_id = pxlfp;      /* set file identifier */
  pixel_files[current].use_count++;   /* update reference count */
#ifndef USEPXL
  gfin = pxlfp;
#endif
}

#else /* ! MAXOPEN > 1 */

{
  FILEPTR f;
  struct font_entry *fp;

  if (pfontptr == fontptr && pxlfp != NO_FILE)
    return;         /* we need not have been called */
  if (fontptr->font_file_id == NO_FILE)
    return;         /* we need not have been called */

  f = pfontptr->font_file_id;
  if (f != FPNULL) {
    if (pxlfp != FPNULL) {
      fp = hfontptr;
      while ((fp != NULL) && (fp->font_file_id != f))
        fp = fp->next;

      if (fp == NULL)
        Fatal("Open file %x not found in font entry list.\n",f);
      else
        fp->font_file_id = FPNULL;
    }
    BCLOSE(f);
  }
  if ((pxlfp = BINOPEN(fontptr->name)) == FPNULL) {
    Warning("PXL-file %s could not be opened", fontptr->name);
    pxlfp = NO_FILE;
  }
  pfontptr = fontptr;
  fontptr->font_file_id = pxlfp;
}

#endif


/*-->PixRound*/
/**********************************************************************/
/*****************************  PixRound  *****************************/
/**********************************************************************/
long4 PixRound(long4 x, long4 conv)
{
  return((x + conv) / conv);
}


#ifdef LJ_RESIDENT_FONTS
/*-->TryResident*/
/**********************************************************************/
/****************************  TryResident  ***************************/
/**********************************************************************/
static bool
TryResident(struct font_entry *fontptr)
{
  tfm_info_type tfm_info;

  /* To determine if a font is resident, check for a special family
     value (header bytes 12..16 in the TFM file). This seems cleaner,
     and certainly more convenient, than somehow reading an external
     ljfonts.map file in which we'd have to specify information for all
     the resident fonts.  */
  if (tfm_read_info(fontptr->n, &tfm_info)
      && tfm_info.family[0]
      && EQ((char *)tfm_info.family, "HPAUTOTFM")) {
    unsigned i;
    double factor = fontptr->s / (double)0x100000;

    resident_count++;
    fontptr->resident_p = _TRUE;
    strncpy(fontptr->symbol_set, (char *)tfm_info.coding_scheme, 39);
    fontptr->symbol_set[39] = '\0';
    fontptr->resid = tfm_info.typeface_id;
    fontptr->spacing = tfm_info.spacing;
    fontptr->style = tfm_info.style;
    fontptr->weight = tfm_info.weight;

    if (fontptr->spacing == SPACING_FIXED) {
      /* Have to select the point in pitch (characters per inch) instead
         of point size, and thus have to figure out the pitch that
         corresponds to the point size at which the font is used.

         To do this, take the width of the interword space, and see how
         many of those characters will fit in the at size. Then convert
         to how many characters will fit in one inch. That's our pitch.

         All the builtin LJ4 fonts that are monospaced are Intellifont,
         which have 72.307 points per inch. Not that it really makes any
         difference. We don't worry about this elsewhere, since all
         point sizes are rounded to .25pt anyway, which is more than the
         difference between the various definitions of `point'. */
      double ds_in_points = fontptr->s / 65536.0;
      double w_in_points = tfm_info.interword / (double)0x100000;
      if (ds_in_points == 0 || w_in_points == 0) {
        /* Avoid division by zero if no interword space. */
        Warning("%s: Can't determine pitch for this monospaced font.\n",
                 fontptr->n);
        fontptr->pitch = 10; /* Result will look awful, which is good. */
      } else {
        fontptr->pitch = 72.307 / (ds_in_points * w_in_points);
      }
    }

#ifdef DEBUG
    if (Debug)
      fprintf(ERR_STREAM,"%6s: typeface=%u\tspacing=%u\tstyle=%u\tweight=%d\n",
              fontptr->n, fontptr->resid, fontptr->spacing,
              fontptr->style, fontptr->weight);
#endif
    for (i = 0; i < NFNTCHARS; i++) {
      struct char_entry *cptr = &(fontptr->ch[i]);
      cptr->tfmw = (long4)(tfm_info.widths[i] * factor);
      cptr->cw = ((fontptr->ch[i].tfmw) / (double)hconv) + .5;
      cptr->width =
        cptr->height =
        cptr->xOffset =
        cptr->yOffset =
        cptr->yyOffset = 0;
    }
    return _TRUE;
  } else {
    fontptr->resident_p = _FALSE;
    return _FALSE;
  }
}
#endif



/*-->ReadFontDef*/
/**********************************************************************/
/****************************  ReadFontDef  ***************************/
/**********************************************************************/

unsigned char skip_specials(long4 *pkloc)
{
  long4    i, j;
  register unsigned char  flag_byte;
  do {
    flag_byte = (unsigned char) NoSignExtend(pxlfp, 1);
    /*
      fprintf(ERR_STREAM,"flagbyte = %d, pkloc=%ld\n",(int)flag_byte,(long)*pkloc);
      */

    (*pkloc) ++;
    if (flag_byte  >= 240)
      switch (flag_byte) {
      case 240:
      case 241:
      case 242:
      case 243 : {
        i = 0;
        for (j = 240; j <= (long4)flag_byte; j++) {
          i = 256 * i + NoSignExtend(pxlfp, 1);
          (*pkloc) ++;
        }
        for (j = 1; j <= i; j++) {
          (void) NoSignExtend(pxlfp, 1);
          (*pkloc) ++;
        }
        break;
      }
      case 244 : {
        i = NoSignExtend(pxlfp, 4);
        (*pkloc) += 4;
        break;
      }
      case 245 :
        break;
      case 246 :
        break;
      case 247:
      case 248:
      case 249:
      case 250:
      case 251:
      case 252:
      case 253:
      case 254:
      case 255: {
        Fatal("Unexpected flagbyte %d!\n", (int)flag_byte);
      }
      }
  } while (!((flag_byte < 240) || (flag_byte == PK_POST)));
  return(flag_byte);
}


void ReadFontDef(long4 k)
{
  long4    t;
  unsigned short i;
  struct font_entry *tfontptr; /* temporary font_entry pointer   */
  struct char_entry *tcharptr; /* temporary char_entry pointer  */
  static int      plusid = 0;
  bool font_found = _FALSE;
#ifdef LJ_RESIDENT_FONTS
  bool resident_font_located = _FALSE;
#endif
#ifdef LJ
  int depth, max_depth;
#endif

#ifdef DEBUG
  if (Debug)
    fprintf(ERR_STREAM,"Mallocating %d Bytes)...\n", sizeof(struct font_entry ));
#endif

  if ((tfontptr = NEW(struct font_entry )) == NULL)
    Fatal("can't malloc space for font_entry");

  allocated_storage += sizeof(struct font_entry );

  tfontptr->next = hfontptr;
  tfontptr->font_file_id = FPNULL;
  fontptr = hfontptr = tfontptr;
  tfontptr->ncdl = 0;
  tfontptr->k = k;
  tfontptr->c = NoSignExtend(dvifp, 4); /* checksum */
  tfontptr->s = NoSignExtend(dvifp, 4); /* space size */
  tfontptr->d = NoSignExtend(dvifp, 4); /* design size */
  tfontptr->a = (int)NoSignExtend(dvifp, 1); /* length for font name */
  tfontptr->l = (int)NoSignExtend(dvifp, 1); /* device length */

#ifdef LJ
  tfontptr->max_width = tfontptr->max_height = tfontptr->max_yoff =
    max_depth = 0;
#endif

  GetBytes(dvifp, tfontptr->n, tfontptr->a + tfontptr->l);
  tfontptr->n[tfontptr->a+tfontptr->l] = '\0';

  tfontptr->font_mag =
    (long4)((ActualFactor((long4)(1000.0*tfontptr->s/(double)tfontptr->d+0.5))
             * ActualFactor(mag)
#ifdef USEPXL
             * RESOLUTION * 5.0
#else
             * RESOLUTION
#endif
             ) + 0.5);
  /*
printf("[%ld]=%lf * %lf * %lf + 0.5 = %ld\n",
    ((long)(1000.0*tfontptr->s/(double)tfontptr->d+0.5)),
    ActualFactor((long4)(1000.0*tfontptr->s/(double)tfontptr->d+0.5)),
    ActualFactor(mag),
    (double)RESOLUTION * 5,
    (long)tfontptr->font_mag );
*/

#ifdef LJ_RESIDENT_FONTS
  /* Pass in the name; fills in resident_p and resid (if resident). */

  resident_font_located = (bool)TryResident(tfontptr);

  if (tfontptr->resident_p)
    return;

  if (!(resident_font_located))
#endif

    {
      kpse_glyph_file_type font_ret;
      char *name;
      unsigned dpi
        = kpse_magstep_fix ((unsigned) (tfontptr->font_mag / 5.0 + .5),
                            RESOLUTION, NULL);
      tfontptr->font_mag = dpi * 5; /* save correct dpi */

      name = kpse_find_pk (tfontptr->n, dpi, &font_ret);
      if (name)
        {
          font_found = _TRUE;
          tfontptr->name = name;
          allocated_storage += strlen(name)+1;

          if (!FILESTRCASEEQ (tfontptr->n, font_ret.name)) {
              fprintf (stderr,
                       "dvilj: Font %s not found, using %s at %d instead.\n",
                       tfontptr->n, font_ret.name, font_ret.dpi);
              tfontptr->c = 0; /* no checksum warning */
            }
          else if (!kpse_bitmap_tolerance ((double)font_ret.dpi, (double) dpi))
            fprintf (stderr,
                     "dvilj: Font %s at %d not found, using %d instead.\n",
                     tfontptr->name, dpi, font_ret.dpi);
          if (G_verbose)
            fprintf(stderr,"%d: using font <%s>\n", plusid,tfontptr->name);
        }
      else
        {
          tfontptr->font_file_id = NO_FILE;
          fprintf (stderr,
            "dvilj: Font %s at %u not found, characters will be left blank.\n",
            tfontptr->n, dpi);
        }
    }

  tfontptr->plusid = plusid;
  plusid++;

  /* sprintf(tfontptr->psname,"%s.%ld.%d",
       tfontptr->n, (long)tfontptr->font_mag, (long)tfontptr->plusid);*/

#ifdef LJ
  if (plusid >= HANDLE_MAX_FONTS)
    Fatal("can handle only %d fonts! ask a wizzard...\n",
          HANDLE_MAX_FONTS);
#endif
  if (tfontptr != pfontptr) {
    if (font_found)
      OpenFontFile();
    else
      pxlfp = NO_FILE;
  }
#ifdef USEPXL
  if ( pxlfp == NO_FILE ) {        /* allow missing pxl files */
    tfontptr->magnification = 0;
    tfontptr->designsize = 0;
#endif
    for (i = FIRSTFNTCHAR; i <= LASTFNTCHAR; i++) {
      tcharptr = &(tfontptr->ch[i]);
#ifdef USEPXL
      tcharptr->width = 0;
      tcharptr->height = 0;
      tcharptr->xOffset = 0;
      tcharptr->yOffset = 0;
#endif
      tcharptr->where.isloaded = _FALSE;
      tcharptr->where.address.fileOffset = NONEXISTANT;
      tcharptr->tfmw = 0;
    }
#ifdef USEPXL
    return;
  }
  t = NoSignExtend(pxlfp, 1);
  if (t == 0) {
    t = NoSignExtend(pxlfp, 1);
    t = NoSignExtend(pxlfp, 2);
    if (t == 1002)
      tfontptr->id = id1002;
    else if (t == 1001)
      tfontptr->id = id1001;
    else
      Fatal("Unknown Version of PXL-format\n");
  } else {
    if (t == PK_PRE) {
      unsigned char   temp_byte;
      temp_byte = (unsigned char) NoSignExtend(pxlfp, 1);
      if (temp_byte != PK_ID)
        Fatal( "Wrong Version of pk file!  (%d should be 89)\n",
               (int)temp_byte);
      else
        tfontptr->id = pk89;
    } else
      Fatal("unknown font format in file <%s> !\n",fontptr->name);
  }

  if ((tfontptr->id == id1002) || (tfontptr->id == id1001)) {
    FSEEK(pxlfp, -20l, SEEK_END);

    t = NoSignExtend(pxlfp, 4);
    check_checksum (tfontptr->c, t, tfontptr->name);

    tfontptr->magnification = NoSignExtend(pxlfp, 4);
    tfontptr->designsize    = NoSignExtend(pxlfp, 4);

    if (tfontptr->id == id1001)
      FSEEK(pxlfp, (long)(NoSignExtend(pxlfp, 4) * 4), SEEK_SET);
    else
      FSEEK(pxlfp, (long)NoSignExtend(pxlfp, 4), SEEK_SET);

    for (i = FIRSTFNTCHAR; i <= 127; i++) {   /* only defined for 7bit*/
      tcharptr = &(tfontptr->ch[i]);
      tcharptr->width   = (unsigned short) NoSignExtend(pxlfp, 2);
      tcharptr->height  = (unsigned short) NoSignExtend(pxlfp, 2);
      tcharptr->xOffset = (short) SignExtend(pxlfp, 2);
      tcharptr->yOffset = (short) SignExtend(pxlfp, 2);
      tcharptr->where.isloaded = _FALSE;
      if (tfontptr->id == id1001)
        tcharptr->where.address.fileOffset = NoSignExtend(pxlfp,4) * 4;
      else
        tcharptr->where.address.fileOffset = NoSignExtend(pxlfp,4);
      tcharptr->tfmw = (long4)
        (   (double)(NoSignExtend(pxlfp, 4))
            * (double)tfontptr->s / (double)0x100000 );
      tcharptr->cw = (long4)(((double)tcharptr->tfmw/(double)hconv) + 0.5);

      if (tcharptr->width  > CHAR_WIDTH_LARGE  ||
          tcharptr->height > CHAR_HEIGTH_LARGE )
        tcharptr->charsize = LARGE_SIZE;
      else
        tcharptr->charsize = SMALL_SIZE;
#ifdef LJ
# define set_max(x,y) if ((y)>(x)) x = y

      set_max(tfontptr->max_width,tcharptr->width);
      set_max(tfontptr->max_height,tcharptr->height);
      if (tcharptr->yOffset > 0  && (int)tfontptr->max_yoff < (int)tcharptr->yOffset)
        tfontptr->max_yoff = tcharptr->yOffset;
      if ((depth = tcharptr->height - tcharptr->yOffset)>max_depth)
        max_depth = depth;
#endif

    }
#ifdef LJ
    tfontptr->max_height = max_depth ? tfontptr->max_yoff+max_depth :
      tfontptr->max_yoff+1;
#endif
  } else { /* PK 89 format */
    unsigned char   temp_byte;
    register unsigned char  flag_byte;
    long4    hppp, vppp, pkloc, packet_length;
    int     car, ii;

    /* read comment */
    for ( ii = temp_byte = (unsigned char)NoSignExtend(pxlfp, 1); ii>0; ii--) {
      flag_byte = (unsigned char) NoSignExtend(pxlfp, 1);
#ifdef DEBUG
      if (Debug) fprintf(ERR_STREAM, "%c", flag_byte );
#endif
    }
#ifdef DEBUG
    if (Debug) fprintf(ERR_STREAM, "\n");
#endif
    pkloc = 3 + (int)temp_byte;
    tfontptr->designsize = NoSignExtend(pxlfp, 4);

    t = NoSignExtend(pxlfp, 4);
    check_checksum (tfontptr->c, t, tfontptr->name);

    hppp = NoSignExtend(pxlfp, 4);
    vppp = NoSignExtend(pxlfp, 4);
    if (hppp != vppp)
      Warning("aspect ratio is %ld:%ld (should be 1:1)!",
              (long)hppp, (long)vppp);
    tfontptr->magnification = (long4)(hppp * 72.27 * 5 / 65536l + 0.5);

    pkloc += 16;
    flag_byte = skip_specials(&pkloc);

    while (flag_byte != PK_POST) {
      if ((flag_byte & 7) == 7) {
        /* fprintf(ERR_STREAM,"\nRead long character preamble\n"); */

        packet_length = (unsigned long4)NoSignExtend(pxlfp,4);
        if ((car = (int)NoSignExtend(pxlfp, 4)) > (LASTFNTCHAR))
          Fatal("Bad character (%d) in PK-File\n",(int)car);

        tcharptr = &(tfontptr->ch[car]);
        tcharptr->where.address.fileOffset = pkloc;
        /* set pkloc to end_of_packet */
        pkloc += packet_length + 8;

        tcharptr->tfmw = NoSignExtend(pxlfp, 4);
        (void) NoSignExtend(pxlfp, 4); /* horesc not used */
        (void) NoSignExtend(pxlfp, 4); /* not used */

        tcharptr->width   = (unsigned short) NoSignExtend(pxlfp, 4);
        tcharptr->height  = (unsigned short) NoSignExtend(pxlfp, 4);
        tcharptr->xOffset = (short) SignExtend(pxlfp, 4);
        tcharptr->yOffset = (short) SignExtend(pxlfp, 4);
        tcharptr->where.isloaded = _FALSE;
      } else if (flag_byte & 4) {
        /* fprintf(ERR_STREAM,"Read extended short character preamble\n"); */

        packet_length = ((long4)flag_byte & 3) * 65536l +
          (unsigned short) NoSignExtend(pxlfp, 2);
        if ((car = (int)NoSignExtend(pxlfp, 1)) > (LASTFNTCHAR))
          Fatal("Bad character (%d) in PK-File\n",(int)car);

        tcharptr = &(tfontptr->ch[car]);
        tcharptr->where.address.fileOffset = pkloc;
        /* set pkloc to end_of_packet */
        pkloc += packet_length + 3;

        tcharptr->tfmw = NoSignExtend(pxlfp, 3);
        /*
          { register unsigned short t;
          t = (unsigned short) NoSignExtend(pxlfp, 1);
          tcharptr->tfmw = t * 65536l +
          (unsigned short) NoSignExtend(pxlfp, 2);
          }
          */
        /* horesc not used */
        (void) NoSignExtend(pxlfp, 2);
        tcharptr->width   = (unsigned short) NoSignExtend(pxlfp,2);
        tcharptr->height  = (unsigned short) NoSignExtend(pxlfp,2);
        tcharptr->xOffset = (short) SignExtend(pxlfp, 2);
        tcharptr->yOffset = (short) SignExtend(pxlfp, 2);
        tcharptr->where.isloaded = _FALSE;
      } else {
        /* fprintf(ERR_STREAM,"<Read short character preamble@>\n"); */

        packet_length = ((long4)flag_byte & 3) * 256 +
          NoSignExtend(pxlfp, 1);
        if ((car = (int)NoSignExtend(pxlfp, 1)) > (LASTFNTCHAR))
          Fatal("Bad character (%d) in PK-File\n",(int)car);

        tcharptr = &(tfontptr->ch[car]);
        tcharptr->where.address.fileOffset = pkloc;
        /* set pkloc to end_of_packet */
        pkloc += packet_length + 2;

        tcharptr->tfmw = NoSignExtend(pxlfp, 3);
        /*
          { register unsigned short t;
          t = (unsigned short) NoSignExtend(pxlfp, 1);
          tcharptr->tfmw = t * 65536l +
          (unsigned short) NoSignExtend(pxlfp, 2);
          }
          */
        /* horesc not used */
        (void) NoSignExtend(pxlfp, 1);
        tcharptr->width   = (unsigned short) NoSignExtend(pxlfp,1);
        tcharptr->height  = (unsigned short) NoSignExtend(pxlfp,1);
        tcharptr->xOffset = (short) SignExtend(pxlfp, 1);
        tcharptr->yOffset = (short) SignExtend(pxlfp, 1);
        tcharptr->where.isloaded = _FALSE;
      }

      tcharptr->tfmw = (long4)
        ( tcharptr->tfmw * (double)tfontptr->s / (double)0x100000 );

      tcharptr->cw = (long4)(((double)tcharptr->tfmw / (double)hconv) + 0.5);

      if (tcharptr->width  > CHAR_WIDTH_LARGE  ||
          tcharptr->height > CHAR_HEIGTH_LARGE )
        tcharptr->charsize = LARGE_SIZE;
      else
        tcharptr->charsize = SMALL_SIZE;

#ifdef LJ
      /*
        printf("char=%d: this=%d, max_width=%d, this=%d,max_height=%d, this=%d,max_yoff=%d\n",
        car, tcharptr->width, tfontptr->max_width,
        tcharptr->height,tfontptr->max_height,
        tcharptr->yOffset,tfontptr->max_yoff);
        */
      set_max(tfontptr->max_width, tcharptr->width);
      set_max(tfontptr->max_height,tcharptr->height);
      if (tcharptr->yOffset > 0  && (int)tfontptr->max_yoff < (int)tcharptr->yOffset)
        tfontptr->max_yoff = tcharptr->yOffset;

      if ((depth = tcharptr->height - tcharptr->yOffset) > max_depth)
        max_depth = depth;
#endif
      /*
        fprintf(ERR_STREAM,"char=%d, yotcharptr=%lx, flag_byte=%d, font=%lx\n",car, tcharptr,flag_byte,tfontptr);
        */
      tcharptr->flag_byte = flag_byte;
      FSEEK(pxlfp, (long)pkloc, SEEK_SET);
      flag_byte = skip_specials(&pkloc);

    } /* end of while */
#ifdef LJ
    tfontptr->max_height = max_depth ? tfontptr->max_yoff+max_depth :
      tfontptr->max_yoff+1;
#endif

    /*
printf("fontid=%d: max_width=%u, max_height=%d, max_yoff=%u\n",
        tfontptr->plusid, tfontptr->max_width,
        tfontptr->max_height, tfontptr->max_yoff);
        */

#else /* USEPXL */
    if ( pxlfp == NO_FILE )        /* allow missing pxl files */
      return;

    gfin = pxlfp;
    seekpost();
    readpost();
    check_checksum (tfontptr->c, checksum, tfontptr->name);

    for(i = FIRSTFNTCHAR; i<=LASTFNTCHAR; i++) {
      if (char_exists[i]) {
        tcharptr = &(tfontptr->ch[i]);
        tcharptr->tfmw = (long4)(((float)tfm_wd[i]*(float)tfontptr->s) /
                                 (float)((long4)1l<<20));
        tcharptr->where.address.fileOffset = char_pointer[i];
      }
# ifdef LJ
      /*                 GF USER PLEASE CHECK IF THIS CODE WORKS
                         tfontptr->max_width = gf_font_max_m;
                         tfontptr->max_height = gf_font_max_n;
                         tfontptr->max_yoff = gf_font_min_n;
                         */
# endif /* LJ */
#endif /* USEPXL */

/*****************************************************************************/
  /*if (tcharptr->charsize==LARGE_SIZE)
    fprintf(ERR_STREAM,"%d:\t <%c> w=%d h=%d xO=%d yO=%d tfmw=%ld cw=%ld %d\n",
    i,(char) i,
    tcharptr->width,tcharptr->height,tcharptr->xOffset,tcharptr->yOffset,
    (long)tcharptr->tfmw, (long)tcharptr->cw, (int)(tcharptr->charsize));
    */
/*****************************************************************************/
  }
}





/*-->SetFntNum*/
/**********************************************************************/
/****************************  SetFntNum  *****************************/
/**********************************************************************/
void SetFntNum(long4 k, bool Emitting)
/*  this routine is used to specify the font to be used in printing future
    characters */
{
#ifdef LJ
  static unsigned short plusid = 0;
#endif
  fontptr = hfontptr;
  while ((fontptr != NULL) && (fontptr->k != k))
    fontptr = fontptr->next;
  if (fontptr == NULL)
    Fatal("font %ld undefined", (long)k);
  if (Emitting && fontptr->font_file_id != NO_FILE) {
    if (!fontptr->used_on_this_page
#ifdef LJ_RESIDENT_FONTS
        && !fontptr->resident_p
#endif
        ) {
      fontptr->used_on_this_page = _TRUE;
#ifdef LJ
      if (++fonts_used_on_this_page > MAX_FONTS_PER_PAGE) {
        qfprintf(ERR_STREAM,"%s is font #%d font on this page!",
                  fontptr->n, fonts_used_on_this_page);
        qfprintf(ERR_STREAM," (max = %d) rastering characters!\n",
                  MAX_FONTS_PER_PAGE);
        rasterfont[fontptr->plusid] = _TRUE;
      }
#endif
    }
#ifdef DEBUG
    if (Debug)
      fprintf(ERR_STREAM, "Switching to font #%ld (%s).\n", k, fontptr->n);
#endif
    /* activate font */
#ifdef IBM3812
    sprintf(PMPformat, "\323%c", (unsigned char)fontptr->plusid);
    PMPout(2, PMPformat);
#endif
#ifdef LJ
    if (!rasterfont[fontptr->plusid]) {
#ifdef LJ_RESIDENT_FONTS
      if (fontptr->resident_p) {
#ifdef DEBUG
        if (Debug)
          fprintf(ERR_STREAM, "Resident font #%d.\n", fontptr->resid);
#endif
        EMIT2("\033(%s", fontptr->symbol_set);
        EMIT4("\033(s%up%.2f%c",
              fontptr->spacing,
              /* height in points, or pitch */
              fontptr->spacing ? fontptr->s / 65536.0 : fontptr->pitch,
              fontptr->spacing ? 'v' : 'h' /* height or pitch? */
              );
        EMIT4("%us%db%uT",
              fontptr->style,       /* upright, italic, ... */
              fontptr->weight,      /* regular, bold, ... */
              fontptr->resid
              );
      } else
#endif /* LJ_RESIDENT_FONTS */
        if (fontptr->plusid>0) EMIT2("\033(%dX", fontptr->plusid);
        else                   EMIT1("\033(X");
    }
    /* else fprintf(ERR_STREAM,"I am doing rasterfont for plusid=%d instead\n",
       fontptr->plusid);
       */
#endif
  }
#ifdef LJ    /* reassignment of printer font id  0.48 */
  else if (fontptr->font_file_id != NO_FILE
#ifdef LJ_RESIDENT_FONTS
           && !fontptr->resident_p
#endif
           ) {
    if (fontptr->ncdl == 0) {
#ifdef DEBUG
      if (Debug)
        fprintf(ERR_STREAM, "Changing plusid from %d to %d\n",
                fontptr->plusid, (int)plusid);
#endif
      fontptr->plusid = plusid;
      plusid ++;
    }
  }
#endif
}



/*-->SkipFontDef*/
/**********************************************************************/
/****************************  SkipFontDef  ***************************/
/**********************************************************************/
void SkipFontDef(void)
{
  int     a, l;
  char    n[STRSIZE];

  (void) NoSignExtend(dvifp, 4);
  (void) NoSignExtend(dvifp, 4);
  (void) NoSignExtend(dvifp, 4);
  a = (int)NoSignExtend(dvifp, 1);
  l = (int)NoSignExtend(dvifp, 1);
  GetBytes(dvifp, n, a + l);
}



/*------------------------ end dvifont.c -------------------------------*/

/*-->Fatal*/
/**********************************************************************/
/******************************  Fatal  *******************************/
/**********************************************************************/
void
Fatal (const char *fmt, ...)
{
  va_list args;

  va_start(args, fmt);
  fprintf(ERR_STREAM, "\n");
  fprintf(ERR_STREAM, "%s: FATAL--", G_progname);
  vfprintf(ERR_STREAM, fmt, args);

  fprintf(ERR_STREAM, "\n\n");
  va_end(args);
  CloseFiles();
#ifndef vms
  exit(2);
#else
  exit(SS$_ABORT);
#endif
}



/*-->Warning*/
/**********************************************************************/
/*****************************  Warning  ******************************/
/**********************************************************************/
void                           /* issue a warning */
Warning(const char *fmt, ...)
{
  va_list args;

  va_start(args, fmt);

#ifndef vms
  G_errenc = 1;
#else
  G_errenc = (SS$_ABORT | STS$M_INHIB_MSG);  /* no message on screen */
#endif
  if ( G_nowarn || G_quiet )
    return;

  fprintf(ERR_STREAM, "%s: warning: ", G_progname);
  vfprintf(ERR_STREAM, fmt, args);
  fprintf(ERR_STREAM, "\n");
  va_end(args);
}


/* ------------------------------------------------------------
 * Local Variables:
 * c-file-style: "gnu"
 * fill-column: 76
 * End:
 */
