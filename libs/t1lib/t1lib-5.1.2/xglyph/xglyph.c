/*--------------------------------------------------------------------------
  ----- File:        xglyph.c 
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2003-01-02
  ----- Description: This file is part of the t1-library. The program xglyph
                     allows viewing of character- and string-glyphs. Diverse
		     parameters may be interactively configured. 
  ----- Copyright:   t1lib is copyrighted (c) Rainer Menzner, 1996-2003.
                     As of version 0.5, t1lib is distributed under the
		     GNU General Public Library Lincense. The
		     conditions can be found in the files LICENSE and
		     LGPL, which should reside in the toplevel
		     directory of the distribution.  Please note that 
		     there are parts of t1lib that are subject to
		     other licenses:
		     The parseAFM-package is copyrighted by Adobe Systems
		     Inc.
		     The type1 rasterizer is copyrighted by IBM and the
		     X11-consortium.
  ----- Warranties:  Of course, there's NO WARRANTY OF ANY KIND :-)
  ----- Credits:     I want to thank IBM and the X11-consortium for making
                     their rasterizer freely available.
		     Also thanks to Piet Tutelaers for his ps2pk, from
		     which I took the rasterizer sources in a format
		     independent from X11.
                     Thanks to all people who make free software living!
--------------------------------------------------------------------------*/
  
/* definitions for activating special tests for features of t1lib
   via commandline arguments: */
#define CHECK_PERFORMANCE                    0x0001
#define CHECK_T1_COPYFONT_T1_ADDFONT         0x0002
#define CHECK_CONCATGLYPHS                   0x0004
#define CHECK_CONCATOUTLINES                 0x0008
#define CHECK_FOR_BAD_CHARS                  0x0010
#define CHECK_DEFAULT_ENCODING               0x0020
#define CHECK_SMART_AA                       0x0040
#define NO_GRID                              0x0080
#define CHECK_AA_CACHING                     0x0100
#define CHECK_CACHE_STROKED                  0x0200
#define CHECK_SET_RECT                       0x0400

/* #define SHOW_MANIPULATE_PATHS */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Shell.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/List.h>


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/time.h>


/* The following stuff is necessary for compiling and running
   xglyph on VMS. Submitted by John Hasstedt (John.Hasstedt@sunysb.edu),
   who did the VMS-port of t1lib. */
#if defined(__VMS) && __CRTL_VER < 70000000
#  define putenv(p) \
printf("You must define T1LIB_CONFIG before running this program\n"); \
exit;
#  define gettimeofday(p1,p2)
#endif


/* Note: We include t1lib.h and t1libx.h from lib/t1lib. That way the
   objectfile does only need to be rebuild when the header itself
   changes and not each time the library has been recompiled */
#include "../lib/t1lib/t1lib.h"
#include "../lib/t1lib/t1libx.h"

#ifndef PI
#define PI 3.1415927
#endif

/* utility functions */
BBox ComputeRotBBox( BBox, float);
long time_diff(struct timeval *, struct timeval *);

/* fallback-function */
void exitprog( Widget, XtPointer, XtPointer);
void togglekerning( Widget, XtPointer, XtPointer);
void toggleligatur( Widget, XtPointer, XtPointer);
void toggler2l( Widget, XtPointer, XtPointer);
void toggleunderline( Widget, XtPointer, XtPointer);
void toggleoverline( Widget, XtPointer, XtPointer);
void toggleoverstrike( Widget, XtPointer, XtPointer);
void showchar( Widget , XtPointer , XtPointer );
void showstring( Widget , XtPointer , XtPointer );
void aashowchar( Widget , XtPointer , XtPointer );
void aashowstring( Widget , XtPointer , XtPointer );
void showcharX( Widget , XtPointer , XtPointer );
void showstringX( Widget , XtPointer , XtPointer );
void aashowcharX( Widget , XtPointer , XtPointer );
void aashowstringX( Widget , XtPointer , XtPointer );
void showabout( Widget , XtPointer , XtPointer );
void toggleopacity( Widget, XtPointer, XtPointer);
void setcolor( Widget, XtPointer, XtPointer);
void fonttable( Widget, XtPointer, XtPointer);
void toggleaalevel( Widget, XtPointer, XtPointer);
int  ComputeAAColorsX( unsigned long fg, unsigned long bg, int nolevels);
void printusage( int max);
int CheckTransform( void);




#define  DOUBLEMARGIN 10
#define  SIMPLEMARGIN 5
#define  XOUTPUT_HALFVSIZE 200
#define  XOUTPUT_HALFHSIZE 300
#define  XGLYPH_PAD        16
#define  T1LIBENCFILE "IsoLatin1.enc"
#define  MAXENCODINGS 10
#define  TESTFONTFILE "eufm10.pfb"
#define  SHOWGRID
#define  CROSS_SIZE         6

#define  PAD(bits, pad)     (((bits)+(pad)-1)&-(pad))  
#define  DIALOGWIDTH        170
#define  DIALOGHEIGHT       20
#define  ACTIONBUTTONHEIGHT 20
#define  APPLICATIONHEIGHT  690
#define  EDITABLE_COLOR     "red"
#define  MESSAGEBOXHEIGHT   180
#define  AAMAXPLANES        17
#define  GRAY0   WhitePixel(display, screen_number)
#define  GRAY1   gray1.pixel
#define  GRAY2   gray2.pixel
#define  GRAY3   gray3.pixel
#define  GRAY4   BlackPixel(display, screen_number)

#define  min(a,b) (a < b ? a : b)
#define  max(a,b) (a > b ? a : b)


#define DEFAULT_SIZE "100.0"
#define DEFAULT_CHAR "65"

/* Global variables for rasterizer parameters */
float Size=400.0, Slant=0.0, Extent=1.0, Angle=0.0;
int Modflag=0;
int FontID=0, Caching=1, DeviceResolution=72;
int StrokeWidth;
int nofonts;

long Space=0;
int Opacity=0;
int aalevel;
char TestString[1024]="Test";
int TestChar=70;
char EncodingFile[128]="";
char LastEncodingFile[128]="";
struct
{
  char *encfilename;
  char **encoding;
}
encstruct[MAXENCODINGS];

int *last_resolution;
float *lastExtent;
float *lastSlant;
int LigDetect=0;
unsigned long fg, bg, outbg;
Pixmap tmp_pixmap=0;
Pixmap white_pixmap=0;


Pixmap perf_pixmap;
Pixmap pixmap;
XImage *ximage;
XColor edit_color, gray0, gray1, gray2, gray3, gray4;
XColor white, black, gray, red, green, blue, pink4, yellow,
  gridcolor1, gridcolor2,wozu;
XColor aacolors[AAMAXPLANES];
unsigned long aapixels[AAMAXPLANES];

XGCValues xgcvalues;

 
unsigned short dummy;
char input;
int overallwidth;
char commandline[256];
GLYPH *glyph;
GLYPH *glyph1, *glyph2;
char **Encoding=NULL;
unsigned int height, width;
char statusstring[1024];
T1_TMATRIX matrix;
T1_TMATRIX *matrixP;
static char msg_buf[256];


/* How t1lib pads lines: */
int t1_pad=0;

/* When to swap bytes in aa-pixels? */
int xglyph_byte_order;

/* The following is for time measurements: */
struct timeval time_start, time_stop;
struct timeval *time_ptr_start, *time_ptr_stop;
void *void_ptr;



/* Global X(t)-stuff */
Display *display;
Screen *screen;
int screennumber;
int depth;
Widget TopLevel;
Widget OutputWindow;
Widget Output;
Widget OutputWindow2;
Widget Output2;
Widget dialogfontid;
Widget dialogsize;
Widget dialogslant;
Widget dialogextent;
Widget dialogangle;
Widget dialogteststring;
Widget dialogtestcharacter;
Widget dialogdevres;
Widget dialogstroke;
Widget dialogencfile;
Widget dialogspace;
Widget dialogtmatrix;
Widget labelstatus;
Widget fgcolorbutton;
Widget bgcolorbutton;


/* color setting widgets */
Widget fgwhitebutton;
Widget fgblackbutton;
Widget fggraybutton;
Widget fgredbutton;
Widget fggreenbutton;
Widget fgbluebutton;
Widget bgwhitebutton;
Widget bgblackbutton;
Widget bggraybutton;
Widget bgredbutton;
Widget bggreenbutton;
Widget bgbluebutton;
Widget fglabel;
Widget bglabel;
Widget colorbox;
Widget fgstatus;
Widget bgstatus;


Widget optionsbox;


/* Commandline handling */
char *xglyphoptions[]={"--help",                     /*   0 */
		       "--Help",                     /*   1 */
		       "--noGrid",                   /*   2 */
		       "--setPad",                   /*   3 */
		       "--logError",                 /*   4 */
		       "--logWarning",               /*   5 */
		       "--logStatistic",             /*   6 */
		       "--logDebug",                 /*   7 */
		       "--ignoreForceBold",          /*   8 */
		       "--ignoreFamilyAlignment",    /*   9 */
		       "--ignoreHinting",            /*  10 */
		       "--ignoreAFM",                /*  11 */
		       "--debugLine",                /*  12 */
		       "--debugRegion",              /*  13 */
		       "--debugPath",                /*  14 */
		       "--debugFont",                /*  15 */
		       "--debugHint",                /*  16 */
		       "--checkPerformance",         /*  17 */
		       "--checkCopyFont",            /*  18 */
		       "--checkConcatGlyphs",        /*  19 */
		       "--checkConcatOutlines",      /*  20 */
		       "--checkBadCharHandling",     /*  21 */
		       "--checkDefaultEncoding",     /*  22 */
		       "--checkSmartAntialiasing",   /*  23 */
		       "--checkAACaching",           /*  24 */
		       "--checkSetRect",             /*  25 */
		       "--cacheStrokedGlyphs",       /*  26 */
		       NULL};
int extraflags=0;

/* #define SHOW_MANIPULATE_PATHS */
#ifdef SHOW_MANIPULATE_PATHS
void mymanipulate( long *x, long *y, int type)
{
  double corr;
  double dx, dy;

  dx=(double)*x;
  dy=(double)*y;
  
  /* we have to scale down because we deal with fractional pels */
  /*
  corr=0.000000000000004*dx*dx;
  dy *=1.0+corr;
  */
  /*
#define PERIOD   500.0
  corr=65536.0*0.5*sin(2*PI/PERIOD/65536.0*dx);
  dy +=corr*30.0;
  */
  *y=(long)dy;
  
}
#endif
		    
		      

int main( int argc, char **argv)
{

  
  XtAppContext TopLevelApp;
  Arg args[10];
  
  
  int i, j, numopts=0;
  
  Widget kerningbutton; 
  Widget ligaturbutton;
  Widget r2lbutton;
  Widget showcharbutton;
  Widget showstringbutton;
  Widget aashowcharbutton;
  Widget aashowstringbutton;
  Widget showcharbuttonX;
  Widget showstringbuttonX;
  Widget aashowcharbuttonX;
  Widget aashowstringbuttonX;
  Widget stringlabel;
  Widget aboutbutton;
  Widget opacitybutton;
  Widget underlinebutton;
  Widget overstrikebutton;
  Widget overlinebutton;
  Widget transbox;
  Widget fonttablebutton;
  Widget aalevelbutton;
  
  
  Widget exitbutton;
  
  Widget box;
  
  
  int rasterflags=0, initflags=0, optfound;
  int loglevel=0;
  
  i=0;
  j=0;
  
  while (xglyphoptions[i++]!=NULL)
    numopts++;
  
  for ( i=1; i<argc; i++){
    if (strncmp(&(argv[i][0]), "--", 2)==0){
      j=0;
      optfound=0;
      while ( (j<numopts) && optfound==0){
	if (strcmp(argv[i],xglyphoptions[j])==0){
	  switch (j){
	  case 0:
	    printusage( 8);  /* Show 8 options */
	    exit(0);
	  case 1:
	    printusage( 0);  /* Show all options */
	    exit(0);
	  case 2:
	    extraflags |= NO_GRID;
	    optfound=1;
	    break;
	  case 3:
	    /* We have to examine one further argument */
	    i++;
	    if (strcmp( argv[i], "8")==0)
	      t1_pad=8;
	    else if (strcmp( argv[i], "16")==0)
	      t1_pad=16;
	    else if (strcmp( argv[i], "32")==0)
	      t1_pad=32;
	    else{
	      fprintf( stderr, "xglyph: Invalid padding specification %s\n",
		       argv[i]);
	      exit(-1);
	    }
	    optfound=1;
	    break;
	  case 4:
	    loglevel = T1LOG_ERROR;
	    loglevel |= (0x01 << 8);
	    optfound=1;
	    break;
	  case 5:
	    loglevel = T1LOG_WARNING;
	    loglevel |= (0x01 << 8);
	    optfound=1;
	    break;
	  case 6:
	    loglevel = T1LOG_STATISTIC;
	    loglevel |= (0x01 << 8);
	    optfound=1;
	    break;
	  case 7:
	    loglevel = T1LOG_DEBUG;
	    loglevel |= (0x01 << 8);
	    optfound=1;
	    break;
	  case 8:
	    rasterflags |= T1_IGNORE_FORCEBOLD;
	    optfound=1;
	    break;
	  case 9:
	    rasterflags |= T1_IGNORE_FAMILYALIGNMENT;
	    optfound=1;
	    break;
	  case 10:
	    rasterflags |= T1_IGNORE_HINTING;
	    optfound=1;
	    break;
	  case 11:
	    initflags |= T1_NO_AFM;
	    optfound=1;
	    break;
	  case 12:
	    rasterflags |= T1_DEBUG_LINE;
	    optfound=1;
	    break;
	  case 13:
	    rasterflags |= T1_DEBUG_REGION;
	    optfound=1;
	    break;
	  case 14:
	    rasterflags |= T1_DEBUG_PATH;
	    optfound=1;
	    break;
	  case 15:
	    rasterflags |= T1_DEBUG_FONT;
	    optfound=1;
	    break;
	  case 16:
	    rasterflags |= T1_DEBUG_HINT;
	    optfound=1;
	    break;
	  case 17:
	    extraflags |= CHECK_PERFORMANCE;
	    optfound=1;
	    break;
	  case 18:
	    extraflags |= CHECK_T1_COPYFONT_T1_ADDFONT;
	    optfound=1;
	    break;
	  case 19:
	    extraflags |= CHECK_CONCATGLYPHS;
	    optfound=1;
	    break;
	  case 20:
	    extraflags |= CHECK_CONCATOUTLINES;
	    optfound=1;
	    break;
	  case 21:
	    extraflags |= CHECK_FOR_BAD_CHARS;
	    optfound=1;
	    break;
	  case 22:
	    extraflags |= CHECK_DEFAULT_ENCODING;
	    optfound=1;
	    break;
	  case 23:
	    extraflags |= CHECK_SMART_AA;
	    optfound=1;
	    break;
	  case 24:
	    extraflags |= CHECK_AA_CACHING;
	    optfound=1;
	    break;
	  case 25:
	    extraflags |= CHECK_SET_RECT;
	    optfound = 1;
	    break;
	  case 26:
	    extraflags |= CHECK_CACHE_STROKED;
	    optfound = 1;
	    break;
	  default: /* should not be reached */
	    break;
	  }
	} /* end of "if (strcmp.." */
	j++;
      }
      if (optfound==0){
	  fprintf( stderr, "xglyph: Unknown option %s\n",
		   argv[i]);
	  exit(-1);
      }
    }
    else /* string doesn't start with "--" -> no option */
      break;
  }
  
  
  /* Assign address of time parameter */
  time_ptr_start=&time_start;
  time_ptr_stop=&time_stop;
  
  /* Check for environment entry. If not set, set it to current
     directory so that configuration file is found there */
  if (getenv("T1LIB_CONFIG")==NULL){
    putenv( "T1LIB_CONFIG=./t1lib.config");
  }

  
  /* Set log-level: */
  T1_SetLogLevel( loglevel & (~(0x01<<8)));
 

  /* Get padding value if not set from commandline. For 32 bit architectures
     like Intel, 16 might be the best padding default value: */
  if (t1_pad==0)
    t1_pad=16;
  if (T1_SetBitmapPad( t1_pad)!=0){
    t1_pad=T1_GetBitmapPad();
  }
  
  
  /* Initialize t1-library. If xglyph is called with arguments
     that are no options, we assume that the arguments are font
     files and ignore the fontdata base file. At this point the value of
     "i" is the number of commandline arg that is supposed to contain a
     fontfile name (or it is argc, if no files where specified).

     Note further that "loglevel" contains (1) info whether to create a log
     file in the first bit of the higher byte of the lower word and 
     (2) info which loglevel to set. (3) It further may contain more
     initialization flags.   
     */
  if ((extraflags & CHECK_AA_CACHING)){
    loglevel |= (T1_AA_CACHING<<8);
  }
  
  if (i==argc){
    if (T1_InitLib( (loglevel>>8) | initflags )==NULL){
      fprintf(stderr,"Initialization of t1lib failed (T1_errno=%d)!\n",
	      T1_errno);
      return(-1); 
    }
  }
  else{ /* There are still arguments which we interprete as fontfile names */
    if (T1_InitLib( (loglevel>>8) | initflags | IGNORE_FONTDATABASE )==NULL){
      fprintf(stderr,"Initialization of t1lib failed (T1_errno=%d)!\n",
	      T1_errno);
      return(-1); 
    }
    /* We start adding fonts as the i-th commandline arguments */
    for ( j=i; j<argc; j++){
      T1_AddFont( argv[j]);
    }
  }

  /* We return if no fonts could be installed */
  if ((j=T1_GetNoFonts())<=0){
    fprintf(stderr,"xglyph: No fonts in database (T1_errno=%d)!\n", T1_errno);
    return(-1);
  }
  
  /* Set the raster parameters */
  T1_SetRasterFlags( rasterflags);


  /* Load alternate encoding and associate it with filename: */
  encstruct[0].encoding=T1_LoadEncoding(T1LIBENCFILE);
  encstruct[0].encfilename=(char *)malloc(strlen(T1LIBENCFILE)+1);
  strcpy( encstruct[0].encfilename, T1LIBENCFILE);
  for ( i=1; i<MAXENCODINGS; i++){
    encstruct[i].encoding=NULL;
    encstruct[i].encfilename=NULL;
  }
  
  
  /* If requested, set a default encoding vector */
  if (extraflags & CHECK_DEFAULT_ENCODING){
    T1_SetDefaultEncoding( encstruct[0].encoding);
  }
  
  
  if( extraflags & CHECK_T1_COPYFONT_T1_ADDFONT){
    {
      int k;
      
      /* The following code is to test the T1_CopyFont()-function. It
	 generates logical fonts with IDs twice the ID of the original
	 font which are then slanted by 0.3. */
      nofonts=T1_GetNoFonts();
      printf("Initial number of fonts: %d\n", nofonts);
      
      for (i=0; i< nofonts; i++){ 
	T1_LoadFont(i);
	if ((k=T1_CopyFont(i))<0){
	  fprintf(stderr,"T1_CopyFont() unsuccessful (k=%d), for FontID=%d\n", k, i); 
	}else{
	  T1_SlantFont(k,0.3); 
	  fprintf(stderr,"Slanted Font under FontID %d generated\n", k); 
	}
      }
      fprintf( stderr, "T1_AddFont() returned newID %d for fontfile %s!\n",
	       T1_AddFont( TESTFONTFILE), TESTFONTFILE);
      
      nofonts=T1_GetNoFonts();
      printf("Final number of fonts: %d\n", nofonts);
    }
  }

  /* Setup arrays for the last... values */
  nofonts=T1_GetNoFonts();
  if ( (last_resolution=(int *) malloc ( nofonts * sizeof(int)))==NULL){
    fprintf( stderr, "xglyph: memory allocation error\n");
    exit(1);
  }
  else
    for (i=0; i<nofonts; i++)
      last_resolution[i]=72;
  if ( (lastSlant=(float *) malloc ( nofonts * sizeof(float)))==NULL){
    fprintf( stderr, "xglyph: memory allocation error\n");
    exit(1);
  }
  else
    for (i=0; i<nofonts; i++)
      lastSlant[i]=0.0;
  if ( (lastExtent=(float *) malloc ( nofonts * sizeof(float)))==NULL){
    fprintf( stderr, "xglyph: memory allocation error\n");
    exit(1);
  }
  else
    for (i=0; i<nofonts; i++)
      lastExtent[i]=1.0;

  if (extraflags & CHECK_SMART_AA)
    T1_AASetSmartMode( T1_YES);
  
  /* If checking caching of stroked characters is enabled via command line,
     we call T1_StrokeFont() on each font in the database. This is valid
     here since we do not already have glyph data. */
  if ( extraflags & CHECK_CACHE_STROKED ) {
    for ( i=0; i<nofonts; i++) {
      T1_LoadFont(i);
      T1_StrokeFont( i, 1);
    }
  }

  /* Initialize application */
  TopLevel = XtAppInitialize(&TopLevelApp, (String) "Xglyph", (XrmOptionDescList) NULL,
			     (Cardinal) 0,
			     &argc,
			     argv,
			     (String *) NULL,
			     (ArgList) NULL,
			     (Cardinal) 0);
  i=0;
  XtSetArg(args[i], XtNheight, APPLICATIONHEIGHT); i++;
  XtSetValues(TopLevel,args,i);

  /* Create window for graphics output */
  Output = XtCreatePopupShell( "Xglyph-Output", topLevelShellWidgetClass,
			       TopLevel, NULL,0);

  OutputWindow = XtCreateManagedWidget("xglyph-Output",labelWidgetClass, Output,
				       NULL, 0);
  

  /* Get display/screen pointer and screennumber */
  display = XtDisplay(OutputWindow);
  screen = XtScreen(OutputWindow);
  screennumber = DefaultScreen(display);
  depth = DefaultDepth(display,screennumber);


  /* In case client and server have different endian architecture,
     we have to care for the 16 and 32 bit aa-pixels to have the 
     correct byte order */
  if (T1_CheckEndian()==0)
    xglyph_byte_order=0;
  else
    xglyph_byte_order=1;

    
  if( extraflags & CHECK_PERFORMANCE){
    perf_pixmap=0;
    /* The following is for testing only */
    Output2 = XtCreatePopupShell( "Xglyph-Output2", topLevelShellWidgetClass,
				  TopLevel, NULL,0);
    
    OutputWindow2 = XtCreateManagedWidget("xglyph-Output2",labelWidgetClass, Output2,
					  NULL, 0);
    i=0;
    XtSetArg( args[i], XtNwidth, 800 ); i++;
    XtSetArg( args[i], XtNheight, 600 ); i++;
    XtSetValues(OutputWindow2,args,i);
  }
  

  /* Get colors and assign pixel values: */
  XAllocNamedColor(display, DefaultColormap(display,screennumber), EDITABLE_COLOR,
		   &edit_color, &wozu);
  XAllocNamedColor(display, DefaultColormap(display,screennumber), "green",
		   &green, &wozu);
  XAllocNamedColor(display, DefaultColormap(display,screennumber), "blue",
		   &blue, &wozu);
  XAllocNamedColor(display, DefaultColormap(display,screennumber), "black",
		   &gray4, &wozu);
  XAllocNamedColor(display, DefaultColormap(display,screennumber), "gray25",
		   &gray3, &wozu);
  XAllocNamedColor(display, DefaultColormap(display,screennumber), "gray50",
		   &gray2, &wozu);
  XAllocNamedColor(display, DefaultColormap(display,screennumber), "gray75",
		   &gray1, &wozu); 
  XAllocNamedColor(display, DefaultColormap(display,screennumber), "white",
		   &gray0, &wozu); 
  XAllocNamedColor(display, DefaultColormap(display,screennumber), "yellow",
		   &yellow, &wozu);
  XAllocNamedColor(display, DefaultColormap(display,screennumber), "pink4",
		   &pink4, &wozu);
  XAllocNamedColor(display, DefaultColormap(display,screennumber), "cyan",
		   &gridcolor1, &wozu);
  XAllocNamedColor(display, DefaultColormap(display,screennumber), "magenta",
		   &gridcolor2, &wozu);
  white=gray0;
  black=gray4;
  gray=gray1;
  red=edit_color;
  /* Fill the aacolors and aapixels-array */
  ComputeAAColorsX( black.pixel, white.pixel, AAMAXPLANES);
  

  /* Set default colors for X11 rastering functions */
  fg=black.pixel;
  bg=white.pixel;
  
  
  T1_AASetBitsPerPixel(DefaultDepth(display,screennumber));
  /*  T1_LogicalPositionX( 0); */
  
  
  /* The box-widget for all the buttons */
  box=XtCreateManagedWidget("xglyph", boxWidgetClass, TopLevel, NULL, 0);

  i=0;
  XtSetArg(args[i], XtNheight,400); i++;
  XtSetValues(box,args,i);
  
  
  /* A dialogbox for the fontID-specification */
  dialogfontid=XtCreateManagedWidget("dialog1", dialogWidgetClass, box,
				     NULL, 0);
  i=0;
  XtSetArg(args[i], XtNlabel, "FontID:"); i++;
  XtSetArg(args[i], XtNvalue, "0"); i++;
  XtSetValues(dialogfontid,args,i);
  i=0;
  XtSetArg(args[i], XtNwidth,DIALOGWIDTH); i++;
  XtSetArg(args[i], XtNheight,DIALOGHEIGHT); i++;
  XtSetArg(args[i], XtNresize, FALSE); i++;
  XtSetArg(args[i], XtNforeground, edit_color.pixel); i++;
  XtSetValues(XtNameToWidget(dialogfontid,"value"),args,i); 
  

  
  /* A dialogbox for the font-Size */
  dialogsize=XtCreateManagedWidget("dialog1", dialogWidgetClass, box,
				     NULL, 0);
  i=0;
  XtSetArg(args[i], XtNlabel, "Font-Size [bp]:"); i++;
  XtSetArg(args[i], XtNvalue, DEFAULT_SIZE); i++;
  XtSetValues(dialogsize,args,i);
  i=0;
  XtSetArg(args[i], XtNwidth,DIALOGWIDTH); i++;
  XtSetArg(args[i], XtNheight,DIALOGHEIGHT); i++;
  XtSetArg(args[i], XtNresize, FALSE); i++;
  XtSetArg(args[i], XtNforeground, edit_color.pixel); i++;
  XtSetValues(XtNameToWidget(dialogsize,"value"),args,i); 
  

  
  /* A dialogbox for the font slant specification */
  dialogslant=XtCreateManagedWidget("dialog1", dialogWidgetClass, box,
				     NULL, 0);
  i=0;
  XtSetArg(args[i], XtNlabel, "Slant:"); i++;
  XtSetArg(args[i], XtNvalue, "0.0"); i++;
  XtSetValues(dialogslant,args,i);
  i=0;
  XtSetArg(args[i], XtNwidth,DIALOGWIDTH / 2 - 8); i++;
  XtSetArg(args[i], XtNheight,DIALOGHEIGHT); i++;
  XtSetArg(args[i], XtNresize, FALSE); i++;
  XtSetArg(args[i], XtNforeground, edit_color.pixel); i++;
  XtSetValues(XtNameToWidget(dialogslant,"value"),args,i); 
  

  
  /* A dialogbox for the font-extension */
  dialogextent=XtCreateManagedWidget("dialog1", dialogWidgetClass, box,
				     NULL, 0);
  i=0;
  XtSetArg(args[i], XtNlabel, "Extension:"); i++;
  XtSetArg(args[i], XtNvalue, "1.0"); i++;
  XtSetValues(dialogextent,args,i);
  i=0;
  XtSetArg(args[i], XtNwidth,DIALOGWIDTH / 2 - 8); i++;
  XtSetArg(args[i], XtNheight,DIALOGHEIGHT); i++;
  XtSetArg(args[i], XtNresize, FALSE); i++;
  XtSetArg(args[i], XtNforeground, edit_color.pixel); i++;
  XtSetValues(XtNameToWidget(dialogextent,"value"),args,i); 
  

  
  /* transformation matrix dialog widget */
  dialogtmatrix = XtCreateManagedWidget( "dialog1", dialogWidgetClass, box,
				   NULL, 0);
  i=0;
  XtSetArg(args[i], XtNlabel, "Transformation-Matrix:"); i++;
  XtSetArg(args[i], XtNvalue, "1.0, 0.0, 0.0, 1.0"); i++;
  XtSetValues(dialogtmatrix,args,i);
  i=0;
  XtSetArg(args[i], XtNwidth,DIALOGWIDTH); i++;
  XtSetArg(args[i], XtNheight,DIALOGHEIGHT); i++;
  XtSetArg(args[i], XtNresize, FALSE); i++;
  XtSetArg(args[i], XtNforeground, edit_color.pixel); i++;
  XtSetValues(XtNameToWidget(dialogtmatrix,"value"),args,i); 
  


  /* A dialogbox for the device resolution */
  dialogdevres=XtCreateManagedWidget("dialog1", dialogWidgetClass, box,
				     NULL, 0);
  i=0;
  XtSetArg(args[i], XtNlabel, "Res [DPI]"); i++;
  XtSetArg(args[i], XtNvalue, "72"); i++;
  XtSetValues(dialogdevres,args,i);
  i=0;
  XtSetArg(args[i], XtNwidth,DIALOGWIDTH / 2 - 8); i++;
  XtSetArg(args[i], XtNheight,DIALOGHEIGHT); i++;
  XtSetArg(args[i], XtNresize, FALSE); i++;
  XtSetArg(args[i], XtNforeground, edit_color.pixel); i++;
  XtSetValues(XtNameToWidget(dialogdevres,"value"),args,i); 
   

  
  /* A dialogbox for the strokewidth */
  dialogstroke=XtCreateManagedWidget("dialog1", dialogWidgetClass, box,
				     NULL, 0);
  i=0;
  XtSetArg(args[i], XtNlabel, "S-Width"); i++;
  if ( extraflags & CHECK_CACHE_STROKED ) {
    XtSetArg(args[i], XtNvalue, "10");
    i++;
  }
  else {
    XtSetArg(args[i], XtNvalue, "0");
    i++;
  }
  XtSetValues(dialogstroke,args,i);
  i=0;
  XtSetArg(args[i], XtNwidth,DIALOGWIDTH / 2 - 8); i++;
  XtSetArg(args[i], XtNheight,DIALOGHEIGHT); i++;
  XtSetArg(args[i], XtNresize, FALSE); i++;
  XtSetArg(args[i], XtNforeground, edit_color.pixel); i++;
  XtSetValues(XtNameToWidget(dialogstroke,"value"),args,i); 
   

  
  /* A dialogbox for the encoding filename */
  dialogencfile=XtCreateManagedWidget("dialog1", dialogWidgetClass, box,
				      NULL, 0);
  i=0;
  XtSetArg(args[i], XtNlabel, "Encoding-File:"); i++;
  XtSetArg(args[i], XtNvalue, ""); i++;
  XtSetValues(dialogencfile,args,i);
  i=0;
  XtSetArg(args[i], XtNwidth,DIALOGWIDTH); i++;
  XtSetArg(args[i], XtNheight,DIALOGHEIGHT); i++;
  XtSetArg(args[i], XtNresize, FALSE); i++;
  XtSetArg(args[i], XtNforeground, edit_color.pixel); i++;  
  XtSetValues(XtNameToWidget(dialogencfile,"value"),args,i); 
  

  
  /* A dialogbox for the angle */
  dialogangle=XtCreateManagedWidget("dialog1", dialogWidgetClass, box,
				     NULL, 0);
  i=0;
  XtSetArg(args[i], XtNlabel, "Angle [deg]:"); i++;
  XtSetArg(args[i], XtNvalue, "0.0"); i++;
  XtSetValues(dialogangle,args,i);
  i=0;
  XtSetArg(args[i], XtNwidth,DIALOGWIDTH); i++;
  XtSetArg(args[i], XtNheight,DIALOGHEIGHT); i++;
  XtSetArg(args[i], XtNresize, FALSE); i++;
  XtSetArg(args[i], XtNforeground, edit_color.pixel); i++;
  XtSetValues(XtNameToWidget(dialogangle,"value"),args,i); 
  

  
  /* A dialogbox for the space-offset in strings */
  dialogspace=XtCreateManagedWidget("dialog1", dialogWidgetClass, box,
				     NULL, 0);
  i=0;
  XtSetArg(args[i], XtNlabel, "Space-Off [1/1000 bp]:"); i++;
  XtSetArg(args[i], XtNvalue, "0"); i++;
  XtSetValues(dialogspace,args,i);
  i=0;
  XtSetArg(args[i], XtNwidth,DIALOGWIDTH); i++;
  XtSetArg(args[i], XtNheight,DIALOGHEIGHT); i++;
  XtSetArg(args[i], XtNresize, FALSE); i++;
  XtSetArg(args[i], XtNforeground, edit_color.pixel); i++;
  XtSetValues(XtNameToWidget(dialogspace,"value"),args,i); 
  

  
  /* A dialogbox for the character */
  dialogtestcharacter=XtCreateManagedWidget("dialog1", dialogWidgetClass, box,
				     NULL, 0);
  i=0;
  XtSetArg(args[i], XtNlabel, "Character (decimal):"); i++;
  XtSetArg(args[i], XtNvalue, DEFAULT_CHAR); i++;
  XtSetValues(dialogtestcharacter,args,i);
  i=0;
  XtSetArg(args[i], XtNwidth,DIALOGWIDTH); i++;
  XtSetArg(args[i], XtNheight,DIALOGHEIGHT); i++;
  XtSetArg(args[i], XtNresize, FALSE); i++;
  XtSetArg(args[i], XtNforeground, edit_color.pixel); i++;
  XtSetValues(XtNameToWidget(dialogtestcharacter,"value"),args,i); 
  

  
  /* The box for color selecting buttons for the 
     X11-rastering functions */
  optionsbox=XtCreateManagedWidget("box", boxWidgetClass, box, NULL, 0);

  i=0;
  XtSetArg(args[i], XtNheight, DIALOGHEIGHT); i++;
  XtSetArg(args[i], XtNwidth, DIALOGWIDTH + 30); i++;
  XtSetValues(optionsbox,args,i);
  /* The toggle widget for kerning (un-)setting */
  kerningbutton = XtCreateManagedWidget("toggle", toggleWidgetClass, optionsbox, 
				   NULL, 0); 
  i=0;
  XtSetArg( args[i], XtNwidth, 55 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, "Kerning" ); i++;
  XtSetValues(kerningbutton,args,i);
  XtAddCallback( kerningbutton, XtNcallback,
		 (XtCallbackProc) togglekerning, (XtPointer) 0 );


  /* The toggle widget for ligatur-detection (un-)setting */
  ligaturbutton = XtCreateManagedWidget("toggle", toggleWidgetClass, optionsbox, 
				   NULL, 0); 
  i=0;
  XtSetArg( args[i], XtNwidth, 55 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, "Ligature" ); i++;
  XtSetValues(ligaturbutton,args,i);
  XtAddCallback( ligaturbutton, XtNcallback,
		 (XtCallbackProc) toggleligatur, (XtPointer) 0 );


  /* The toggle widget for the typesetting direction switch message */
  r2lbutton = XtCreateManagedWidget("toggle", toggleWidgetClass, optionsbox, 
				   NULL, 0); 
  i=0;
  XtSetArg( args[i], XtNwidth, 55 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, "|-->" ); i++;
  XtSetValues(r2lbutton,args,i);
  XtAddCallback( r2lbutton, XtNcallback,
		 (XtCallbackProc) toggler2l, (XtPointer) 0 );


  /* The toggle widget for underlining (un-)setting */
  underlinebutton = XtCreateManagedWidget("toggle", toggleWidgetClass, optionsbox, 
				   NULL, 0); 
  i=0;
  XtSetArg( args[i], XtNwidth, 55 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, "Underline" ); i++;
  XtSetValues(underlinebutton,args,i);
  XtAddCallback( underlinebutton, XtNcallback,
		 (XtCallbackProc) toggleunderline, (XtPointer) 0 );


  /* The toggle widget for underlining (un-)setting */
  overlinebutton = XtCreateManagedWidget("toggle", toggleWidgetClass, optionsbox, 
				   NULL, 0); 
  i=0;
  XtSetArg( args[i], XtNwidth, 55 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, "Overline" ); i++;
  XtSetValues(overlinebutton,args,i);
  XtAddCallback( overlinebutton, XtNcallback,
		 (XtCallbackProc) toggleoverline, (XtPointer) 0 );


  /* The toggle widget for underlining (un-)setting */
  overstrikebutton = XtCreateManagedWidget("toggle", toggleWidgetClass, optionsbox, 
				   NULL, 0); 
  i=0;
  XtSetArg( args[i], XtNwidth, 55 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, "Overstrike" ); i++;
  XtSetValues(overstrikebutton,args,i);
  XtAddCallback( overstrikebutton, XtNcallback,
		 (XtCallbackProc) toggleoverstrike, (XtPointer) 0 );


  /* The button to raster and display the current character using
     the current settings */
  showcharbutton = XtCreateManagedWidget("Com", commandWidgetClass, box, 
					 NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 60 ); i++;
  XtSetArg( args[i], XtNheight, ACTIONBUTTONHEIGHT ); i++;
  XtSetArg( args[i], XtNlabel, "Char"); i++;
  XtSetValues(showcharbutton,args,i);
  XtAddCallback( showcharbutton, XtNcallback,
		 (XtCallbackProc) showchar, (XtPointer) 0 );


  /* The button to raster and display the current string using
     the current settings */
  showstringbutton = XtCreateManagedWidget("Com", commandWidgetClass, box, 
					   NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 60 ); i++;
  XtSetArg( args[i], XtNheight, ACTIONBUTTONHEIGHT ); i++;
  XtSetArg( args[i], XtNlabel, "String"); i++;
  XtSetValues(showstringbutton,args,i);
  XtAddCallback( showstringbutton, XtNcallback,
		 (XtCallbackProc) showstring, (XtPointer) 0 );


  /* The button to raster and display the current character using
     the current settings and antialiasing */
  aashowcharbutton = XtCreateManagedWidget("Com", commandWidgetClass, box, 
					   NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 120 ); i++;
  XtSetArg( args[i], XtNheight, ACTIONBUTTONHEIGHT ); i++;
  XtSetArg( args[i], XtNlabel, "AAChar"); i++;
  XtSetValues(aashowcharbutton,args,i);
  XtAddCallback( aashowcharbutton, XtNcallback,
		 (XtCallbackProc) aashowchar, (XtPointer) 0 );


  /* The button to raster and display the current string using
     the current settings and antialiasing */
  aashowstringbutton = XtCreateManagedWidget("Com", commandWidgetClass, box, 
					     NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 120 ); i++;
  XtSetArg( args[i], XtNheight, ACTIONBUTTONHEIGHT ); i++;
  XtSetArg( args[i], XtNlabel, "AAString"); i++;
  XtSetValues(aashowstringbutton,args,i);
  XtAddCallback( aashowstringbutton, XtNcallback,
		 (XtCallbackProc) aashowstring, (XtPointer) 0 );


  /* The button to raster and display the current character using
     the current settings */
  showcharbuttonX = XtCreateManagedWidget("Com", commandWidgetClass, box, 
					  NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 60 ); i++;
  XtSetArg( args[i], XtNheight, ACTIONBUTTONHEIGHT ); i++;
  XtSetArg( args[i], XtNlabel, "CharX"); i++;
  XtSetValues(showcharbuttonX,args,i);
  XtAddCallback( showcharbuttonX, XtNcallback,
		 (XtCallbackProc) showcharX, (XtPointer) 0 );
		 

  /* The button to raster and display the current string using
     the current settings */
  showstringbuttonX = XtCreateManagedWidget("Com", commandWidgetClass, box, 
					    NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 60 ); i++;
  XtSetArg( args[i], XtNheight, ACTIONBUTTONHEIGHT ); i++;
  XtSetArg( args[i], XtNlabel, "StringX"); i++;
  XtSetValues(showstringbuttonX,args,i);
  XtAddCallback( showstringbuttonX, XtNcallback,
		 (XtCallbackProc) showstringX, (XtPointer) 0 );


  /* The button to raster and display the current character using
     the current settings and antialiasing */
  aashowcharbuttonX = XtCreateManagedWidget("Com", commandWidgetClass, box, 
					   NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 120 ); i++;
  XtSetArg( args[i], XtNheight, ACTIONBUTTONHEIGHT ); i++;
  XtSetArg( args[i], XtNlabel, "AACharX"); i++;
  XtSetValues(aashowcharbuttonX,args,i);
  XtAddCallback( aashowcharbuttonX, XtNcallback,
		 (XtCallbackProc) aashowcharX, (XtPointer) 0 );


  /* The button to raster and display the current string using
     the current settings and antialiasing */
  aashowstringbuttonX = XtCreateManagedWidget("Com", commandWidgetClass, box, 
					   NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 120 ); i++;
  XtSetArg( args[i], XtNheight, ACTIONBUTTONHEIGHT ); i++;
  XtSetArg( args[i], XtNlabel, "AAStringX"); i++;
  XtSetValues(aashowstringbuttonX,args,i);
  XtAddCallback( aashowstringbuttonX, XtNcallback,
		 (XtCallbackProc) aashowstringX, (XtPointer) 0 );

  
  /* The box for color trasp/op and fonttable button */
  transbox=XtCreateManagedWidget("box", boxWidgetClass, box, NULL, 0);
  i=0;
  XtSetArg(args[i], XtNheight,20); i++;
  XtSetArg(args[i], XtNwidth,90); i++;
  XtSetValues(transbox,args,i);

  
  /* The button to display an AA font table */
  fonttablebutton = XtCreateManagedWidget("Com", commandWidgetClass, transbox, 
					   NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 80 ); i++;
  XtSetArg( args[i], XtNheight, 15 ); i++;
  XtSetArg( args[i], XtNlabel, "Font Table"); i++;
  XtSetValues(fonttablebutton,args,i);
  XtAddCallback( fonttablebutton, XtNcallback,
		 (XtCallbackProc) fonttable, (XtPointer) 0 );

  
  /* The toggle widget for opacity/transparency */
  opacitybutton = XtCreateManagedWidget("toggle", toggleWidgetClass, transbox, 
					NULL, 0); 
  i=0;
  XtSetArg( args[i], XtNwidth, 80 ); i++;
  XtSetArg( args[i], XtNheight, 15 ); i++;
  XtSetArg( args[i], XtNlabel, "Transparent" ); i++;
  XtSetValues(opacitybutton,args,i);
  XtAddCallback( opacitybutton, XtNcallback,
		 (XtCallbackProc) toggleopacity, (XtPointer) 0 );
  outbg=pink4.pixel; /* the initial bgcolor for transparent mode */

  
  /* The toggle widget for opacity/transparency */
  aalevelbutton = XtCreateManagedWidget("toggle", toggleWidgetClass, transbox, 
					NULL, 0); 
  i=0;
  XtSetArg( args[i], XtNwidth, 80 ); i++;
  XtSetArg( args[i], XtNheight, 15 ); i++;
  XtSetArg( args[i], XtNlabel, "AA-Low" ); i++;
  XtSetValues(aalevelbutton,args,i);
  XtAddCallback( aalevelbutton, XtNcallback,
		 (XtCallbackProc) toggleaalevel, (XtPointer) 0 );
  aalevel=T1_AA_LOW; /* the initial bgcolor for transparent mode */


  /* The box for color selecting buttons for the 
     X11-rastering functions */
  colorbox=XtCreateManagedWidget("box", boxWidgetClass, box, NULL, 0);
  i=0;
  XtSetArg(args[i], XtNheight,20); i++;
  XtSetArg(args[i], XtNwidth,280); i++;
  XtSetValues(colorbox,args,i);
  /* FG-label */
  fgstatus = XtCreateManagedWidget("label", labelWidgetClass, colorbox, 
					    NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 20 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, ""); i++;
  XtSetArg( args[i], XtNbackground, fg); i++;
  XtSetValues(fgstatus,args,i);
  
  fglabel = XtCreateManagedWidget("label", labelWidgetClass, colorbox, 
					    NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 80 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, "Foreground"); i++;
  XtSetValues(fglabel,args,i);
  
  fgwhitebutton = XtCreateManagedWidget("Com", commandWidgetClass, colorbox, 
					    NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 20 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, ""); i++;
  XtSetArg( args[i], XtNbackground, white.pixel); i++;
  XtSetValues(fgwhitebutton,args,i);
  XtAddCallback( fgwhitebutton, XtNcallback,
		 (XtCallbackProc) setcolor, (XtPointer) 0 );

  fgblackbutton = XtCreateManagedWidget("Com", commandWidgetClass, colorbox, 
					    NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 20 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, ""); i++;
  XtSetArg( args[i], XtNbackground, black.pixel); i++;
  XtSetValues(fgblackbutton,args,i);
  XtAddCallback( fgblackbutton, XtNcallback,
		 (XtCallbackProc) setcolor, (XtPointer) 0 );

  fggraybutton = XtCreateManagedWidget("Com", commandWidgetClass, colorbox, 
					    NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 20 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, ""); i++;
  XtSetArg( args[i], XtNbackground, gray.pixel); i++;
  XtSetValues(fggraybutton,args,i);
  XtAddCallback( fggraybutton, XtNcallback,
		 (XtCallbackProc) setcolor, (XtPointer) 0 );

  fgredbutton = XtCreateManagedWidget("Com", commandWidgetClass, colorbox, 
					    NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 20 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, ""); i++;
  XtSetArg( args[i], XtNbackground, red.pixel); i++;
  XtSetValues(fgredbutton,args,i);
  XtAddCallback( fgredbutton, XtNcallback,
		 (XtCallbackProc) setcolor, (XtPointer) 0 );

  fggreenbutton = XtCreateManagedWidget("Com", commandWidgetClass, colorbox, 
					    NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 20 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, ""); i++;
  XtSetArg( args[i], XtNbackground, green.pixel); i++;
  XtSetValues(fggreenbutton,args,i);
  XtAddCallback( fggreenbutton, XtNcallback,
		 (XtCallbackProc) setcolor, (XtPointer) 0 );

  fgbluebutton = XtCreateManagedWidget("Com", commandWidgetClass, colorbox, 
					    NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 20 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, ""); i++;
  XtSetArg( args[i], XtNbackground, blue.pixel); i++;
  XtSetValues(fgbluebutton,args,i);
  XtAddCallback( fgbluebutton, XtNcallback,
		 (XtCallbackProc) setcolor, (XtPointer) 0 );

  /* BG-label */
  bgstatus = XtCreateManagedWidget("label", labelWidgetClass, colorbox, 
					    NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 20 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, ""); i++;
  XtSetArg( args[i], XtNbackground, bg); i++;
  XtSetValues(bgstatus,args,i);
  
  bglabel = XtCreateManagedWidget("label", labelWidgetClass, colorbox, 
					    NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 80 ); i++; 
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, "Background"); i++;
  XtSetValues(bglabel,args,i);
  
  bgwhitebutton = XtCreateManagedWidget("Com", commandWidgetClass, colorbox, 
					    NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 20 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, ""); i++;
  XtSetArg( args[i], XtNbackground, white.pixel); i++;
  XtSetValues(bgwhitebutton,args,i);
  XtAddCallback( bgwhitebutton, XtNcallback,
		 (XtCallbackProc) setcolor, (XtPointer) 0 );

  bgblackbutton = XtCreateManagedWidget("Com", commandWidgetClass, colorbox, 
					    NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 20 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, ""); i++;
  XtSetArg( args[i], XtNbackground, black.pixel); i++;
  XtSetValues(bgblackbutton,args,i);
  XtAddCallback( bgblackbutton, XtNcallback,
		 (XtCallbackProc) setcolor, (XtPointer) 0 );

  bggraybutton = XtCreateManagedWidget("Com", commandWidgetClass, colorbox, 
					    NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 20 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, ""); i++;
  XtSetArg( args[i], XtNbackground, gray.pixel); i++;
  XtSetValues(bggraybutton,args,i);
  XtAddCallback( bggraybutton, XtNcallback,
		 (XtCallbackProc) setcolor, (XtPointer) 0 );

  bgredbutton = XtCreateManagedWidget("Com", commandWidgetClass, colorbox, 
					    NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 20 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, ""); i++;
  XtSetArg( args[i], XtNbackground, red.pixel); i++;
  XtSetValues(bgredbutton,args,i);
  XtAddCallback( bgredbutton, XtNcallback,
		 (XtCallbackProc) setcolor, (XtPointer) 0 );

  bggreenbutton = XtCreateManagedWidget("Com", commandWidgetClass, colorbox, 
					    NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 20 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, ""); i++;
  XtSetArg( args[i], XtNbackground, green.pixel); i++;
  XtSetValues(bggreenbutton,args,i);
  XtAddCallback( bggreenbutton, XtNcallback,
		 (XtCallbackProc) setcolor, (XtPointer) 0 );

  bgbluebutton = XtCreateManagedWidget("Com", commandWidgetClass, colorbox, 
					    NULL, 0);
  i=0;
  XtSetArg( args[i], XtNwidth, 20 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, ""); i++;
  XtSetArg( args[i], XtNbackground, blue.pixel); i++;
  XtSetValues(bgbluebutton,args,i);
  XtAddCallback( bgbluebutton, XtNcallback,
		 (XtCallbackProc) setcolor, (XtPointer) 0 );


  
  /* Label for the next box */
  stringlabel=XtCreateManagedWidget("Label", labelWidgetClass, box,
				    NULL, 0);
  i=0;
  XtSetArg(args[i], XtNwidth,380); i++;
  XtSetArg(args[i], XtNlabel, "Test-String:"); i++;
  XtSetValues(stringlabel,args,i);
  


  
  /* An input-box for the test-string */
  dialogteststring=XtCreateManagedWidget("dialog1", asciiTextWidgetClass, box,
				     NULL, 0);
  i=0;
  XtSetArg(args[i], XtNwidth,380); i++;
  XtSetArg(args[i], XtNheight,30); i++;
  XtSetArg(args[i], XtNwrap, XawtextWrapLine); i++;
  XtSetArg(args[i], XtNeditType, XawtextEdit ); i++;
  XtSetArg(args[i], XtNforeground, edit_color.pixel); i++;
  XtSetValues(dialogteststring,args,i);
  


  /* Label for the next box */
  labelstatus=XtCreateManagedWidget("Label", labelWidgetClass, box,
				    NULL, 0);
  i=0;
  XtSetArg(args[i], XtNwidth,380); i++;
  XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
  XtSetArg(args[i], XtNlabel, "No Messages!"); i++;
  XtSetValues(labelstatus,args,i);
  


  /* The button to exit the program */
  exitbutton = XtCreateManagedWidget("Com2", commandWidgetClass, box, 
				   NULL, 0); 
  i=0;
  XtSetArg( args[i], XtNwidth, 100 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNx, 0 ); i++;
  XtSetArg( args[i], XtNy, 0 ); i++;
  XtSetArg( args[i], XtNlabel, " Exit Program"); i++;
  XtSetValues(exitbutton,args,i);
  XtAddCallback( exitbutton, XtNcallback,
		 (XtCallbackProc) exitprog, (XtPointer) 0 );


  /* The About widget for the about message */
  aboutbutton = XtCreateManagedWidget("Com", commandWidgetClass, box, 
				   NULL, 0); 
  i=0;
  XtSetArg( args[i], XtNwidth, 55 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;
  XtSetArg( args[i], XtNlabel, "About" ); i++;
  XtSetValues(aboutbutton,args,i);
  XtAddCallback( aboutbutton, XtNcallback,
		 (XtCallbackProc) showabout, (XtPointer) 0 );


  XtRealizeWidget(TopLevel);
  XtPopup(Output,XtGrabNone);

  if( extraflags & CHECK_PERFORMANCE){
    XtPopup(Output2,XtGrabNone);
  }
  

  /* Set parameters for X11-support */
  T1_SetX11Params( display, DefaultVisual(display, screennumber),
		   DefaultDepth( display, screennumber),
		   DefaultColormap(display, screennumber));

  
  /* Create a dummy-pixmap because we need one to destroy before
     the next is created! */
  pixmap = XCreatePixmap(display,
			 XtWindow(OutputWindow),  
			 200,  
			 200, 
			 DefaultDepth(display,screennumber)
			 );
  
			 
  
  XtAppMainLoop(TopLevelApp); 
  
  return(0);
  
}


/* Compute the BBox of a rotated box: */
BBox ComputeRotBBox( BBox inbox, float angle)
{

  int i;
  BBox resultbox={ 0, 0, 0, 0};
  struct point
  {
    double x;
    double y;
  } p[4], P[4];
  
  double sinalpha, cosalpha;
  
  sinalpha=sin((double)angle*PI/180);
  cosalpha=cos((double)angle*PI/180);

  /* initialize points */
  p[0].x=(double) inbox.llx;
  p[0].y=(double) inbox.lly;
  p[1].x=(double) inbox.urx;
  p[1].y=(double) inbox.lly;
  p[2].x=(double) inbox.urx;
  p[2].y=(double) inbox.ury;
  p[3].x=(double) inbox.llx;
  p[3].y=(double) inbox.ury;

  /*
  fprintf( stderr, "InBox: (%f,%f), (%f,%f), (%f,%f), (%f,%f)\n",
	   p[0].x, p[0].y, p[1].x, p[1].y, p[2].x, p[2].y,
	   p[3].x, p[3].y);
	   */
  /* Compute tranformed points */
  P[0].x=p[0].x*cosalpha-p[0].y*sinalpha;
  P[0].y=p[0].x*sinalpha+p[0].y*cosalpha;
  P[1].x=p[1].x*cosalpha-p[1].y*sinalpha;
  P[1].y=p[1].x*sinalpha+p[1].y*cosalpha;
  P[2].x=p[2].x*cosalpha-p[2].y*sinalpha;
  P[2].y=p[2].x*sinalpha+p[2].y*cosalpha;
  P[3].x=p[3].x*cosalpha-p[3].y*sinalpha;
  P[3].y=p[3].x*sinalpha+p[3].y*cosalpha;
  /*
  fprintf( stderr, "RotPoints: (%f,%f), (%f,%f), (%f,%f), (%f,%f)\n",
	   P[0].x, P[0].y, P[1].x, P[1].y, P[2].x, P[2].y,
	   P[3].x, P[3].y);
	   */

  /* Get BBox: */
  for (i=0; i<4; i++){
    if (P[i].x < (float) resultbox.llx)
      resultbox.llx=(int) floor(P[i].x +0.5);
    if (P[i].x > (float) resultbox.urx)
      resultbox.urx=(int) floor(P[i].x +0.5);
    if (P[i].y < (float) resultbox.lly)
      resultbox.lly=(int) floor(P[i].y +0.5);
    if (P[i].y > (float) resultbox.ury)
      resultbox.ury=(int) floor(P[i].y +0.5);
  }

  return(resultbox);
    
}




void exitprog(Widget exitbutton, XtPointer client_data , XtPointer call_data)
{

  int i;
  
  for (i=0; i< T1_GetNoFonts(); i++){
    /*    printf("Enc-Scheme=%s\n",    T1_GetEncodingScheme( i));*/
    ;
  }

  T1_CloseLib();
  
  exit(0); 
}


void togglekerning( Widget kerningbutton, XtPointer client_data, XtPointer call_data)
{
  int i;
  char state=0;
  Arg args[10];


  /* Get state of toggle Button: */
  i=0;
  XtSetArg(args[i], XtNstate, &state); i++;
  XtGetValues(kerningbutton,args,1);

  if ((state))
    Modflag |= T1_KERNING;
  else
    Modflag &= ~T1_KERNING;
  
}



void toggleligatur( Widget ligaturbutton, XtPointer client_data, XtPointer call_data)
{
  int i;
  char state=0;
  Arg args[10];


  /* Get state of toggle Button: */
  i=0;
  XtSetArg(args[i], XtNstate, &state); i++;
  XtGetValues(ligaturbutton,args,1);

  LigDetect=state;

}



void toggler2l( Widget r2lbutton, XtPointer client_data, XtPointer call_data)
{
  int i;
  char state=0;
  Arg args[10];


  /* Get state of toggle Button: */
  i=0;
  XtSetArg(args[i], XtNstate, &state); i++;
  XtGetValues(r2lbutton,args,1);

  if ((state))
    Modflag |= T1_RIGHT_TO_LEFT;
  else
    Modflag &= ~T1_RIGHT_TO_LEFT;
  
  /* Invert state */
  i=0;
  if (state){
    XtSetArg( args[i], XtNwidth, 55 ); i++;
    XtSetArg( args[i], XtNheight, 20 ); i++;
    XtSetArg( args[i], XtNlabel, "<--|" ); i++;
  }
  else{
    XtSetArg( args[i], XtNwidth, 55 ); i++;
    XtSetArg( args[i], XtNheight, 20 ); i++;
    XtSetArg( args[i], XtNlabel, "|-->" ); i++;
  }
  XtSetValues(r2lbutton,args,i);
}



void toggleunderline( Widget underlinebutton, XtPointer client_data, XtPointer call_data)
{
  int i;
  char state=0;
  Arg args[10];


  /* Get state of toggle Button: */
  i=0;
  XtSetArg(args[i], XtNstate, &state); i++;
  XtGetValues(underlinebutton,args,1);

  if ((state))
    Modflag |= T1_UNDERLINE;
  else
    Modflag &= ~T1_UNDERLINE;
  
}



void toggleoverline( Widget overlinebutton, XtPointer client_data, XtPointer call_data)
{
  int i;
  char state=0;
  Arg args[10];


  /* Get state of toggle Button: */
  i=0;
  XtSetArg(args[i], XtNstate, &state); i++;
  XtGetValues( overlinebutton,args,1);

  if ((state))
    Modflag |= T1_OVERLINE;
  else
    Modflag &= ~T1_OVERLINE;
  
}



void toggleoverstrike( Widget overstrikebutton, XtPointer client_data, XtPointer call_data)
{
  int i;
  char state=0;
  Arg args[10];


  /* Get state of toggle Button: */
  i=0;
  XtSetArg(args[i], XtNstate, &state); i++;
  XtGetValues( overstrikebutton,args,1);

  if ((state))
    Modflag |= T1_OVERSTRIKE;
  else
    Modflag &= ~T1_OVERSTRIKE;
  
}



void toggleopacity( Widget opacitybutton, XtPointer client_data, XtPointer call_data)
{
  int i;
  char state=0;
  Arg args[10];


  /* Get state of toggle Button: */
  i=0;
  XtSetArg(args[i], XtNstate, &state); i++;
  XtGetValues(opacitybutton,args,1);

  i=0;
  XtSetArg( args[i], XtNstate, state); i++;
  XtSetArg( args[i], XtNwidth, 80 ); i++;
  XtSetArg( args[i], XtNheight, 20 ); i++;

  Opacity=state;
  
  /* Invert state */
  if (state){
    XtSetArg( args[i], XtNlabel, "Opaque" ); i++;
    XtSetValues(opacitybutton,args,i);
    outbg=bg;
  }
  else{
    XtSetArg( args[i], XtNlabel, "Transparent" ); i++;
    XtSetValues(opacitybutton,args,i);
    outbg=pink4.pixel;
  }
  
}



/* Toggle the antialiasing level */
void toggleaalevel( Widget aalevelbutton, XtPointer client_data, XtPointer call_data)
{
  int i;
  char state=0;
  Arg args[10];


  /* Get state of toggle Button: */
  i=0;
  XtSetArg(args[i], XtNstate, &state); i++;
  XtGetValues( aalevelbutton,args,1);

  i=0;
  XtSetArg( args[i], XtNstate, state); i++;
  XtSetArg( args[i], XtNwidth, 80 ); i++;
  XtSetArg( args[i], XtNheight, 15 ); i++;

  
  /* Invert state */
  if (state){
    XtSetArg( args[i], XtNlabel, "AA-High" ); i++;
    XtSetValues(aalevelbutton,args,i);
    aalevel=T1_AA_HIGH;
  }
  else{
    XtSetArg( args[i], XtNlabel, "AA-Low" ); i++;
    XtSetValues(aalevelbutton,args,i);
    aalevel=T1_AA_LOW;
  }
  
}



/* Set a new fore/background color */
void setcolor( Widget widget, XtPointer client_data, XtPointer call_data)
{
  int i;
  Arg args[10];

  /* foreground colors */
  if (widget==fgwhitebutton){
    fg=white.pixel;
    i=0;
    XtSetArg( args[i], XtNbackground, fg ); i++;
    XtSetValues(fgstatus,args,i);
    return;
  }
  if (widget==fgblackbutton){
    fg=black.pixel;
    i=0;
    XtSetArg( args[i], XtNbackground, fg ); i++;
    XtSetValues(fgstatus,args,i);
    return;
  }
  if (widget==fggraybutton){
    fg=gray.pixel;
    i=0;
    XtSetArg( args[i], XtNbackground, fg ); i++;
    XtSetValues(fgstatus,args,i);
    return;
  }
  if (widget==fgredbutton){
    fg=red.pixel;
    i=0;
    XtSetArg( args[i], XtNbackground, fg ); i++;
    XtSetValues(fgstatus,args,i);
    return;
  }
  if (widget==fggreenbutton){
    fg=green.pixel;
    i=0;
    XtSetArg( args[i], XtNbackground, fg ); i++;
    XtSetValues(fgstatus,args,i);
    return;
  }
  if (widget==fgbluebutton){
    fg=blue.pixel;
    i=0;
    XtSetArg( args[i], XtNbackground, fg ); i++;
    XtSetValues(fgstatus,args,i);
    return;
  }
  /* background colors */
  if (widget==bgwhitebutton){
    bg=white.pixel;
    if (Opacity)
      outbg=bg;
    i=0;
    XtSetArg( args[i], XtNbackground, bg ); i++;
    XtSetValues(bgstatus,args,i);
    return;
  }
  if (widget==bgblackbutton){
    bg=black.pixel;
    if (Opacity)
      outbg=bg;
    i=0;
    XtSetArg( args[i], XtNbackground, bg ); i++;
    XtSetValues(bgstatus,args,i);
    return;
  }
  if (widget==bggraybutton){
    bg=gray.pixel;
    if (Opacity)
      outbg=bg;
    i=0;
    XtSetArg( args[i], XtNbackground, bg ); i++;
    XtSetValues(bgstatus,args,i);
    return;
  }
  if (widget==bgredbutton){
    bg=red.pixel;
    if (Opacity)
      outbg=bg;
    i=0;
    XtSetArg( args[i], XtNbackground, bg ); i++;
    XtSetValues(bgstatus,args,i);
    return;
  }
  if (widget==bggreenbutton){
    bg=green.pixel;
    if (Opacity)
      outbg=bg;
    i=0;
    XtSetArg( args[i], XtNbackground, bg ); i++;
    XtSetValues(bgstatus,args,i);
    return;
  }
  if (widget==bgbluebutton){
    bg=blue.pixel;
    if (Opacity)
      outbg=bg;
    i=0;
    XtSetArg( args[i], XtNbackground, bg ); i++;
    XtSetValues(bgstatus,args,i);
    return;
  }
  /* we shouldn't get here */
  return;
  
}



void showchar( Widget showcharbutton, XtPointer client_data, XtPointer call_data)
{
  int i, j;
  Arg args[10];

  
  sscanf( XawDialogGetValueString(dialogfontid),"%d", &FontID);
  sscanf( XawDialogGetValueString(dialogsize),"%f", &Size);
  sscanf( XawDialogGetValueString(dialogangle),"%f", &Angle);
  sscanf( XawDialogGetValueString(dialogtestcharacter), "%d", &TestChar);
  sscanf( XawDialogGetValueString(dialogdevres),"%d", &DeviceResolution);
  sscanf( XawDialogGetValueString(dialogstroke),"%d", &StrokeWidth);
       

  if (FontID<0 || FontID>=T1_GetNoFonts()) {
    sprintf(statusstring, "t1lib: FontID out of range!");
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }

  /* Ensure that font is loaded before any operation on the font */
  if ( T1_CheckForFontID( FontID) < 1 ) {
    T1_LoadFont( FontID);
  }
  
  if ( StrokeWidth == 0.0f ) {
    T1_ClearStrokeFlag( FontID);
  }
  else {
    T1_SetStrokeFlag( FontID);
    if ( T1_SetStrokeWidth( FontID, StrokeWidth) != 0 ) {
      sprintf( statusstring, "t1lib: Unable to setup strokewidth!");
      i=0;
      XtSetArg(args[i], XtNbitmap,NULL); i++;
      XtSetArg(args[i], XtNwidth,380); i++;
      XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
      XtSetArg(args[i], XtNlabel, statusstring); i++;
      XtSetValues(labelstatus,args,i);
      return;
    }
  }
  
  if (CheckTransform()==0 && Angle==0.0){
    matrixP=NULL;
  }
  else{
    matrixP=T1_RotateMatrix( &matrix, Angle);
  }
  
  if (DeviceResolution!=last_resolution[FontID]){
    /* Delete all size dependent data for that font */
    for ( i=0; i<T1_GetNoFonts(); i++)
      T1_DeleteAllSizes( i);
    /* Establish new device resolution */
    T1_SetDeviceResolutions(DeviceResolution,DeviceResolution);
    /* Store current resolution */
    last_resolution[FontID]=DeviceResolution;
  }
  i=sscanf( XawDialogGetValueString(dialogencfile),"%s", (char *)EncodingFile);
  if (i==EOF) EncodingFile[0]=0;
  if (strcmp(EncodingFile,LastEncodingFile)){ /* encoding has changed */
    if (strcmp( EncodingFile, "")==0){ /* -> reset to internal encoding */
      for (i=0; i<T1_GetNoFonts(); i++){
	T1_DeleteAllSizes(i);
	T1_ReencodeFont( i, NULL);
      }
      /* Take care that newly loaded fonts are encoded according to current
	 encoding */
      T1_SetDefaultEncoding( NULL);
    }
    else{
      for (i=0; i<MAXENCODINGS; ){
	if ( encstruct[i].encfilename != NULL)
	  if (strcmp( encstruct[i].encfilename, EncodingFile)==0){
	    i++;
	    break;
	  }
	i++;
      }
      if (i==MAXENCODINGS){ /* Encoding from that file was not
			       already loaded -> so load it */
	i=0;
	while (encstruct[i].encfilename != NULL)
	  i++;
	encstruct[i].encoding=T1_LoadEncoding(EncodingFile);
	encstruct[i].encfilename=(char *)malloc(strlen(EncodingFile)+1);
	if (encstruct[i].encoding==NULL){
	  free( encstruct[i].encfilename);
	  encstruct[i].encfilename=NULL;
	}
	i++;
      }
      for (j=0; j<T1_GetNoFonts(); j++){
	T1_DeleteAllSizes(j);
	T1_ReencodeFont( j, encstruct[i-1].encoding);
      }
      /* Take care that newly loaded fonts are encoded according to current
	 encoding */
      T1_SetDefaultEncoding( encstruct[i-1].encoding);
    }
    strcpy( LastEncodingFile, EncodingFile);
  }
  sscanf( XawDialogGetValueString(dialogslant),"%f", &Slant);
  if (Slant!=lastSlant[FontID]){
    /* Delete all size dependent data */
    T1_DeleteAllSizes(FontID);
    T1_LoadFont(FontID);
    T1_SlantFont( FontID, Slant);
    lastSlant[FontID]=Slant;
  }
  sscanf( XawDialogGetValueString(dialogextent),"%f", &Extent);
  if (Extent!=lastExtent[FontID]){
    /* Delete all size dependent data */
    T1_DeleteAllSizes(FontID);
    T1_LoadFont(FontID);
    T1_ExtendFont( FontID, Extent);
    lastExtent[FontID]=Extent;
  }
    
  if (Size<=0.0){
    sprintf(statusstring, "t1lib: Size must be positive!");
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }

  XSetForeground( display, DefaultGC( display, screennumber), black.pixel);
  XSetBackground( display, DefaultGC( display, screennumber), white.pixel);
  /* Reset T1_errno: */
  T1_errno=0;
  gettimeofday(time_ptr_start, void_ptr);
  if ( extraflags & CHECK_SET_RECT )
    glyph=T1_SetRect( FontID, Size, 1000.0, 1000.0, matrixP);
  else
    glyph=T1_SetChar( FontID, TestChar, Size, matrixP);
  gettimeofday(time_ptr_stop, void_ptr);

  if (glyph==NULL){
    sprintf(statusstring, "t1lib: Couldn't generate Bitmap,\n(%s)", T1_StrError(T1_errno));
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }


  height=glyph->metrics.ascent - glyph->metrics.descent;
  overallwidth=glyph->metrics.rightSideBearing - glyph->metrics.leftSideBearing;
  width=overallwidth;

  /* Prepare status message: */
  sprintf(statusstring,"Elapsed time:        %ld Microseconds\nLeftSideBearing:     %d\nRightSideBearing:    %d\nAscent:              %d\nDescent:             %d\nAdvanceX:            %d\nAdvanceY:            %d\nBits Per Pixel:      %ld\nImage Size:          %ld Bytes\nPostScript Fontname: %s\nCharactername:       %s\nT1_errno:            %d\n",
	  time_diff(time_ptr_start,time_ptr_stop),
	  glyph->metrics.leftSideBearing, 
	  glyph->metrics.rightSideBearing, 
	  glyph->metrics.ascent,
	  glyph->metrics.descent,
	  glyph->metrics.advanceX,
	  glyph->metrics.advanceY,
	  glyph->bpp,
	  PAD(glyph->bpp*(glyph->metrics.rightSideBearing-glyph->metrics.leftSideBearing),XGLYPH_PAD)/8*(glyph->metrics.ascent-glyph->metrics.descent),
	  T1_GetFontName(FontID),
	  T1_GetCharName(FontID,(char)TestChar),
	  T1_errno);

  if (glyph->bits !=NULL) {
    ximage=XCreateImage( display,
			 DefaultVisual(display, screennumber),
			 1, /* depths for bitmap is 1 */
			 XYBitmap, /* XYBitmap or XYPixmap */
			 0, /* No offset */
			 glyph->bits,
			 width,
			 height,
			 t1_pad,  /* lines padded to bytes */
			 0 /*PAD(width,8)/8*/  /* number of bytes per line */
			 );
    /* Force bit and byte order */
    ximage->bitmap_bit_order=0;
    ximage->byte_order=0;
    XFreePixmap(display,pixmap);
    pixmap = XCreatePixmap(display,
			   XtWindow(TopLevel),
			   width,
			   height,
			   DefaultDepth(display,screennumber)   
			   );
    XPutImage(display,
	      pixmap,
	      DefaultGC( display, screennumber),
	      ximage,
	      0,  
	      0,  
	      0,  
	      0,  
	      width,
	      height
	      );
    XDestroyImage(ximage);
  }
  
  
  i=0;
  XtSetArg(args[i], XtNbitmap,NULL); i++;
  XtSetArg(args[i], XtNwidth,380); i++;
  XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
  XtSetArg(args[i], XtNbackgroundPixmap, XtUnspecifiedPixmap); i++;
  XtSetArg(args[i], XtNlabel, statusstring); i++;
  XtSetValues(labelstatus,args,i);


  i=0;
  XtSetArg( args[i], XtNwidth, width + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNheight, height + DOUBLEMARGIN); i++;
  if (glyph->bits != NULL) {
    XtSetArg( args[i], XtNbitmap, pixmap); i++;
  }
  else {
    XtSetArg( args[i], XtNbitmap, 0); i++;
  }
  XtSetArg( args[i], XtNresize, 1);i++;
  XtSetArg( args[i], XtNinternalWidth, 0); i++;
  XtSetArg( args[i], XtNinternalHeight, 0); i++;
  XtSetArg( args[i], XtNbackground, WhitePixel(display,screennumber)); i++;
  XtSetArg( args[i], XtNforeground, BlackPixel(display,screennumber)); i++;
  XtSetValues(OutputWindow ,args,i); 
  
  
  i=0;
  XtSetArg( args[i], XtNwidth, width + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNheight, height + DOUBLEMARGIN); i++;
  if (glyph->bits != NULL) {
    XtSetArg( args[i], XtNbitmap, pixmap); i++;
  }
  else {
    XtSetArg( args[i], XtNbitmap, 0); i++;
  }
  XtSetArg( args[i], XtNresize, 1);i++;
  XtSetArg( args[i], XtNinternalWidth, 0); i++;
  XtSetArg( args[i], XtNinternalHeight, 0); i++;
  XtSetValues(Output ,args,i);

  glyph->bits=NULL;    /* Since XDestroyImage() free's this also! */
}



void showstring( Widget showstringbutton, XtPointer client_data, XtPointer call_data)
{
  int i,j,k,l,m,none_found;
  Arg args[10];
  char *theString='\0';
  char *ligtheString='\0';
  char *succs, *ligs;
  char buf_char;

  
  sscanf( XawDialogGetValueString(dialogfontid),"%d", &FontID);
  sscanf( XawDialogGetValueString(dialogsize),"%f", &Size);
  sscanf( XawDialogGetValueString(dialogangle),"%f", &Angle);
  sscanf( XawDialogGetValueString(dialogspace),"%ld", &Space);
  sscanf( XawDialogGetValueString(dialogdevres),"%d", &DeviceResolution);
  sscanf( XawDialogGetValueString(dialogstroke),"%d", &StrokeWidth);

  if (FontID<0 || FontID>=T1_GetNoFonts()) {
    sprintf(statusstring, "t1lib: FontID out of range!");
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }

  /* Ensure that font is loaded before any operation on the font */
  if ( T1_CheckForFontID( FontID) < 1 ) {
    T1_LoadFont( FontID);
  }
  
  if ( StrokeWidth == 0.0f ) {
    T1_ClearStrokeFlag( FontID);
  }
  else {
    T1_SetStrokeFlag( FontID);
    if ( T1_SetStrokeWidth( FontID, StrokeWidth) != 0 ) {
      sprintf( statusstring, "t1lib: Unable to setup strokewidth");
      i=0;
      XtSetArg(args[i], XtNbitmap,NULL); i++;
      XtSetArg(args[i], XtNwidth,380); i++;
      XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
      XtSetArg(args[i], XtNlabel, statusstring); i++;
      XtSetValues(labelstatus,args,i);
      return;
    }
  }
  
  if (CheckTransform()==0 && Angle==0.0){
    matrixP=NULL;
  }
  else{
    matrixP=T1_RotateMatrix( &matrix, Angle);
  }
  
  if (DeviceResolution!=last_resolution[FontID]){
    /* Delete all size dependent data for that font */
    for ( i=0; i<T1_GetNoFonts(); i++)
      T1_DeleteAllSizes( i);
    /* Establish new device resolution */
    T1_SetDeviceResolutions(DeviceResolution,DeviceResolution);
    /* Store current resolution */
    last_resolution[FontID]=DeviceResolution;
  }
  i=sscanf( XawDialogGetValueString(dialogencfile),"%s", (char *)EncodingFile);
  if (i==EOF) EncodingFile[0]=0;
  if (strcmp(EncodingFile,LastEncodingFile)){ /* encoding has changed */
    if (strcmp( EncodingFile, "")==0){ /* -> reset to internal encoding */
      for (i=0; i<T1_GetNoFonts(); i++){
	T1_DeleteAllSizes(i);
	T1_ReencodeFont( i, NULL);
      }
      /* Take care that newly loaded fonts are encoded according to current
	 encoding */
      T1_SetDefaultEncoding( NULL);
    }
    else{
      for (i=0; i<MAXENCODINGS; ){
	if ( encstruct[i].encfilename != NULL)
	  if (strcmp( encstruct[i].encfilename, EncodingFile)==0){
	    i++;
	    break;
	  }
	i++;
      }
      if (i==MAXENCODINGS){ /* Encoding from that file was not
			       already loaded -> so load it */
	i=0;
	while (encstruct[i].encfilename != NULL)
	  i++;
	encstruct[i].encoding=T1_LoadEncoding(EncodingFile);
	encstruct[i].encfilename=(char *)malloc(strlen(EncodingFile)+1);
	if (encstruct[i].encoding==NULL){
	  free( encstruct[i].encfilename);
	  encstruct[i].encfilename=NULL;
	}
	i++;
      }
      for (j=0; j<T1_GetNoFonts(); j++){
	T1_DeleteAllSizes(j);
	T1_ReencodeFont( j, encstruct[i-1].encoding);
      }
      /* Take care that newly loaded fonts are encoded according to current
	 encoding */
      T1_SetDefaultEncoding( encstruct[i-1].encoding);
    }
    strcpy( LastEncodingFile, EncodingFile);
  }
  sscanf( XawDialogGetValueString(dialogslant),"%f", &Slant);
  if (Slant!=lastSlant[FontID]){
    /* Delete all size dependent data */
    T1_DeleteAllSizes(FontID);
    T1_LoadFont(FontID);
    T1_SlantFont( FontID, Slant);
    lastSlant[FontID]=Slant;
  }
  sscanf( XawDialogGetValueString(dialogextent),"%f", &Extent);
  if (Extent!=lastExtent[FontID]){
    /* Delete all size dependent data */
    T1_DeleteAllSizes(FontID);
    T1_LoadFont(FontID);
    T1_ExtendFont( FontID, Extent);
    lastExtent[FontID]=Extent;
  }

  i=0;
  XtSetArg(args[i], XtNstring, &theString); i++;
  XtGetValues(dialogteststring,args,i);
  
  if (Size<=0.0){
    sprintf(statusstring, "t1lib: Size must be positive!");
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }

  /* Now comes the ligatur handling */
  if (strcmp(theString,"")==0)
    theString=(char *) TestString;
  i=strlen(theString);
  ligtheString=(char *)malloc((i+1)*sizeof(char));
  if (LigDetect){
    for (j=0,m=0;j<i;j++,m++){ /* Loop through the characters */
      if ((k=T1_QueryLigs( FontID, theString[j], &succs, &ligs))>0){  
	buf_char=theString[j];
	while (k>0){  
	  none_found=1;
	  for (l=0;l<k;l++){ /* Loop through the ligatures */
	    if (succs[l]==theString[j+1]){
	      buf_char=ligs[l];
	      j++;
	      none_found=0;
	      break;
	    }
	  }
	  if (none_found)
	    break;
	  k=T1_QueryLigs( FontID, buf_char, &succs, &ligs);
	}
	ligtheString[m]=buf_char;
      }
      else{ /* There are no ligatures */
	ligtheString[m]=theString[j];
      }
    }
    ligtheString[m]=0;
  }
  else {
    strcpy(ligtheString,theString);
  }

  /*
  {
    METRICSINFO metrics;
    
    metrics=T1_GetMetricsInfo(FontID,(char *)ligtheString,0,Space,(Modflag & T1_KERNING));
    printf("String's width: %d\n", metrics.width);
    printf("String's BBox: llx = %d\n", metrics.bbox.llx);
    printf("               lly = %d\n", metrics.bbox.lly);
    printf("               urx = %d\n", metrics.bbox.urx);
    printf("               ury = %d\n", metrics.bbox.ury);
    printf("Number of chars in string: %d\n", metrics.numchars);
    for (i=0;i<metrics.numchars;i++)
      printf("Position of %d. character in string: %d afm-units\n", i+1, metrics.charpos[i]);
  }
  */

  /*
  {
    int i,j;
    T1_COMP_CHAR_INFO *ccd;
    T1_COMP_PIECE *cp;
    
    printf("Number of composite characters in font: %d\n",
	   j=T1_GetNoCompositeChars(FontID));
    for (i=0; i<j; i++) {
      ccd=T1_GetCompCharDataByIndex( FontID, i);
      if (ccd==NULL) {
	printf("AFM-ind=%d, ccd=%p, T1_errno=%d\n", i, ccd, T1_errno);
	T1_errno=0;
      }
      else {
      printf( "AFM-ind=%d: Compchar %s (%d), has %d pieces:\n",
	      i,
	      ccd->compchar > -1 ? T1_GetCharName( FontID, ccd->compchar) : "not_enc",
	      ccd->compchar,
	      ccd->numPieces);

      printf( "    basechar %s (%d)\n",
	      ccd->pieces[0].piece > -1 ? T1_GetCharName( FontID, ccd->pieces[0].piece) : "not_enc",
	      ccd->pieces[0].piece);
      printf( "    accent %s (%d)\n",
	      ccd->pieces[1].piece > -1 ? T1_GetCharName( FontID, ccd->pieces[1].piece) : "not_enc",
	      ccd->pieces[1].piece);
      }
      T1_FreeCompCharData( ccd);
    }

  }
  */


  
  XSetForeground( display, DefaultGC( display, screennumber), black.pixel);
  XSetBackground( display, DefaultGC( display, screennumber), white.pixel);
  /* Reset T1_errno: */
  T1_errno=0;
  gettimeofday(time_ptr_start, void_ptr);

  if( extraflags & CHECK_CONCATGLYPHS){
    glyph2=T1_SetString(FontID,(char *)ligtheString,0,Space,Modflag,Size,matrixP);
    glyph1=T1_CopyGlyph( glyph2);
    glyph2=T1_SetString(FontID+1,(char *)ligtheString,0,Space,Modflag,Size,matrixP);
    glyph=T1_ConcatGlyphs( glyph1, glyph2, 0, 0, Modflag);
  }
  else if( extraflags & CHECK_CONCATOUTLINES){
    {
      T1_OUTLINE *path=NULL;
      path=T1_GetStringOutline(FontID,(char *)ligtheString,
			       0,Space,Modflag,Size,matrixP);
      if (path==NULL){
	return;
      }
#ifdef SHOW_MANIPULATE_PATHS
      T1_AbsolutePath( path);
      T1_ManipulatePath( path, &mymanipulate);
      T1_RelativePath( path);
#else
      T1_ConcatOutlines( path,
			 T1_GetStringOutline(FontID+1,(char *)ligtheString,
					     0,Space,Modflag,Size,matrixP));
#endif
      glyph=T1_FillOutline( path, Modflag);
    }
  }
  else{
    if( extraflags & CHECK_FOR_BAD_CHARS){
      sscanf( XawDialogGetValueString(dialogtestcharacter), "%d", &TestChar);
      ligtheString[strlen(ligtheString)/2]=(unsigned char) TestChar;
    }
    glyph=T1_SetString(FontID,(char *)ligtheString,0,Space,Modflag,Size,matrixP);
  }
  
  
  gettimeofday(time_ptr_stop, void_ptr);

  free(ligtheString);

  if ( glyph ==NULL ) {
    sprintf(statusstring, "t1lib: Couldn't generate Bitmap,\n(%s)", T1_StrError(T1_errno));
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }
  
  
  height=glyph->metrics.ascent - glyph->metrics.descent;
  overallwidth=glyph->metrics.rightSideBearing - glyph->metrics.leftSideBearing;
  width=overallwidth;
  
  /* Prepare status message: */
  sprintf(statusstring,"Elapsed time:        %ld Microseconds\nLeftSideBearing:     %d\nRightSideBearing:    %d\nAscent:              %d\nDescent:             %d\nAdvanceX:            %d\nAdvanceY:            %d\nBits Per Pixel:      %ld\nImage Size:          %ld Bytes\nPostScript Fontname: %s\nT1_errno:            %d\n",
	  time_diff(time_ptr_start,time_ptr_stop),
	  glyph->metrics.leftSideBearing, 
	  glyph->metrics.rightSideBearing, 
	  glyph->metrics.ascent,
	  glyph->metrics.descent,
	  glyph->metrics.advanceX,
	  glyph->metrics.advanceY,
	  glyph->bpp,
	  PAD(glyph->bpp*(glyph->metrics.rightSideBearing-glyph->metrics.leftSideBearing),XGLYPH_PAD)/8*(glyph->metrics.ascent-glyph->metrics.descent),
	  T1_GetFontName(FontID),
	  T1_errno);

  if (glyph->bits != NULL) {
    ximage=XCreateImage( display,
			 DefaultVisual(display, screennumber),
			 1, /* depths for bitmap is 1 */
			 XYBitmap, /* XYBitmap or XYPixmap */
			 0, /* No offset */
			 glyph->bits,
			 width,
			 height,
			 t1_pad,  /* lines padded to bytes */
			 0 /*PAD(width,8)/8*/  /* number of bytes per line */
			 );
    /* Force bit and byte order */
    ximage->bitmap_bit_order=LSBFirst;
    ximage->byte_order=LSBFirst;
    XFreePixmap(display,pixmap);
    pixmap = XCreatePixmap(display,
			   XtWindow(TopLevel),
			   width,
			   height,
			   DefaultDepth(display,screennumber)   
			   );
    XPutImage(display,
	      pixmap,
	      DefaultGC( display, screennumber),
	      ximage,
	      0,  
	      0,  
	      0,  
	      0,  
	      width,
	      height
	      );
    XDestroyImage(ximage);
  }
  
  
  i=0;
  XtSetArg(args[i], XtNbitmap,NULL); i++;
  XtSetArg(args[i], XtNwidth,380); i++;
  XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
  XtSetArg(args[i], XtNbackgroundPixmap, XtUnspecifiedPixmap); i++;
  XtSetArg(args[i], XtNlabel, statusstring); i++;
  XtSetValues(labelstatus,args,i);
  
  i=0;
  XtSetArg( args[i], XtNwidth, width + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNheight, height + DOUBLEMARGIN); i++;
  if (glyph->bits != NULL) {
    XtSetArg( args[i], XtNbitmap, pixmap); i++;
  }
  else {
    XtSetArg( args[i], XtNbitmap, 0); i++;
  }
  XtSetArg( args[i], XtNresize, 1);i++;
  XtSetArg( args[i], XtNinternalWidth, 0); i++;
  XtSetArg( args[i], XtNinternalHeight, 0); i++;
  XtSetArg( args[i], XtNbackground, WhitePixel(display,screennumber)); i++;
  XtSetValues(OutputWindow ,args,i);
  
  i=0;
  XtSetArg( args[i], XtNwidth, width + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNheight, height + DOUBLEMARGIN); i++;
  if (glyph->bits != NULL) {
    XtSetArg( args[i], XtNbitmap, pixmap); i++;
  }
  else {
    XtSetArg( args[i], XtNbitmap, 0); i++;
  }
  XtSetArg( args[i], XtNresize, 1);i++;
  XtSetArg( args[i], XtNinternalWidth, 0); i++;
  XtSetArg( args[i], XtNinternalHeight, 0); i++;
  XtSetArg( args[i], XtNbackground, WhitePixel(display,screennumber)); i++;
  XtSetValues(Output ,args,i);
  
  glyph->bits=NULL;    /* Since XDestroyImage() free's this also! */
}



void aashowchar( Widget showcharbutton, XtPointer client_data, XtPointer call_data)
{
  int i, j;
  Arg args[10];
  
  
  sscanf( XawDialogGetValueString(dialogfontid),"%d", &FontID);
  sscanf( XawDialogGetValueString(dialogsize),"%f", &Size);
  sscanf( XawDialogGetValueString(dialogangle),"%f", &Angle);
  sscanf( XawDialogGetValueString(dialogtestcharacter), "%d", &TestChar);
  sscanf( XawDialogGetValueString(dialogdevres),"%d", &DeviceResolution);
  sscanf( XawDialogGetValueString(dialogstroke),"%d", &StrokeWidth);

  if (FontID<0 || FontID>=T1_GetNoFonts()) {
    sprintf(statusstring, "t1lib: FontID out of range!");
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }

  /* Ensure that font is loaded before any operation on the font */
  if ( T1_CheckForFontID( FontID) < 1 ) {
    T1_LoadFont( FontID);
  }
  
  if ( StrokeWidth == 0.0f ) {
    T1_ClearStrokeFlag( FontID);
  }
  else {
    T1_SetStrokeFlag( FontID);
    if ( T1_SetStrokeWidth( FontID, StrokeWidth) != 0 ) {
      sprintf( statusstring, "t1lib: Unable to setup strokewidth");
      i=0;
      XtSetArg(args[i], XtNbitmap,NULL); i++;
      XtSetArg(args[i], XtNwidth,380); i++;
      XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
      XtSetArg(args[i], XtNlabel, statusstring); i++;
      XtSetValues(labelstatus,args,i);
      return;
    }
  }
  
  if (CheckTransform()==0 && Angle==0.0){
    matrixP=NULL;
  }
  else{
    matrixP=T1_RotateMatrix( &matrix, Angle);
  }
  
  if (DeviceResolution!=last_resolution[FontID]){
    /* Delete all size dependent data for that font */
    for ( i=0; i<T1_GetNoFonts(); i++)
      T1_DeleteAllSizes( i);
    /* Establish new device resolution */
    T1_SetDeviceResolutions(DeviceResolution,DeviceResolution);
    /* Store current resolution */
    last_resolution[FontID]=DeviceResolution;
  }
  i=sscanf( XawDialogGetValueString(dialogencfile),"%s", (char *)EncodingFile);
  if (i==EOF) EncodingFile[0]=0;
  if (strcmp(EncodingFile,LastEncodingFile)){ /* encoding has changed */
    if (strcmp( EncodingFile, "")==0){ /* -> reset to internal encoding */
      for (i=0; i<T1_GetNoFonts(); i++){
	T1_DeleteAllSizes(i);
	T1_ReencodeFont( i, NULL);
      }
      /* Take care that newly loaded fonts are encoded according to current
	 encoding */
      T1_SetDefaultEncoding( NULL);
    }
    else{
      for (i=0; i<MAXENCODINGS; ){
	if ( encstruct[i].encfilename != NULL)
	  if (strcmp( encstruct[i].encfilename, EncodingFile)==0){
	    i++;
	    break;
	  }
	i++;
      }
      if (i==MAXENCODINGS){ /* Encoding from that file was not
			       already loaded -> so load it */
	i=0;
	while (encstruct[i].encfilename != NULL)
	  i++;
	encstruct[i].encoding=T1_LoadEncoding(EncodingFile);
	encstruct[i].encfilename=(char *)malloc(strlen(EncodingFile)+1);
	if (encstruct[i].encoding==NULL){
	  free( encstruct[i].encfilename);
	  encstruct[i].encfilename=NULL;
	}
	i++;
      }
      for (j=0; j<T1_GetNoFonts(); j++){
	T1_DeleteAllSizes(j);
	T1_ReencodeFont( j, encstruct[i-1].encoding);
      }
      /* Take care that newly loaded fonts are encoded according to current
	 encoding */
      T1_SetDefaultEncoding( encstruct[i-1].encoding);
    }
    strcpy( LastEncodingFile, EncodingFile);
  }
  sscanf( XawDialogGetValueString(dialogslant),"%f", &Slant);
  if (Slant!=lastSlant[FontID]){
    /* Delete all size dependent data */
    T1_DeleteAllSizes(FontID);
    T1_LoadFont(FontID);
    T1_SlantFont( FontID, Slant);
    lastSlant[FontID]=Slant;
  }
  sscanf( XawDialogGetValueString(dialogextent),"%f", &Extent);
  if (Extent!=lastExtent[FontID]){
    /* Delete all size dependent data */
    T1_DeleteAllSizes(FontID);
    T1_LoadFont(FontID);
    T1_ExtendFont( FontID, Extent);
    lastExtent[FontID]=Extent;
  }
     
  if (Size<=0.0){
    sprintf(statusstring, "t1lib: Size must be positive!");
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }

  /* Reset T1_errno: */
  T1_errno=0;
  /* Set antialiasing level */
  T1_AASetLevel( aalevel);
  /* Set the colors for Anti-Aliasing */
  T1_AASetGrayValues( aapixels[0],   /* white */
		      aapixels[4],
		      aapixels[8],
		      aapixels[12],
		      aapixels[16] ); /* black */
  T1_AAHSetGrayValues( aapixels);
  T1_AANSetGrayValues( aapixels[0], aapixels[16]);
  gettimeofday(time_ptr_start, void_ptr);
  if ( extraflags & CHECK_SET_RECT )
    glyph=T1_AASetRect( FontID, Size, 1000.0, 1000.0, matrixP);
  else
    glyph=T1_AASetChar( FontID, (char)TestChar, Size, matrixP);
  gettimeofday(time_ptr_stop, void_ptr);
    
  if (glyph==NULL) {
    sprintf(statusstring, "t1lib: Couldn't generate Bitmap,\n(%s)", T1_StrError(T1_errno));
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }
  
  
  height=glyph->metrics.ascent - glyph->metrics.descent;
  overallwidth=glyph->metrics.rightSideBearing - glyph->metrics.leftSideBearing;
  width=overallwidth;
  
  /* Prepare status message: */
  sprintf(statusstring,"Elapsed time:        %ld Microseconds\nLeftSideBearing:     %d\nRightSideBearing:    %d\nAscent:              %d\nDescent:             %d\nAdvanceX:            %d\nAdvanceY:            %d\nBits Per Pixel:      %ld\nImage Size:          %ld Bytes\nPostScript Fontname: %s\nCharactername:       %s\nT1_errno:            %d\n",
	  time_diff(time_ptr_start,time_ptr_stop),
	  glyph->metrics.leftSideBearing, 
	  glyph->metrics.rightSideBearing, 
	  glyph->metrics.ascent,
	  glyph->metrics.descent,
	  glyph->metrics.advanceX,
	  glyph->metrics.advanceY,
	  glyph->bpp,
	  PAD(glyph->bpp*(glyph->metrics.rightSideBearing-glyph->metrics.leftSideBearing),XGLYPH_PAD)/8*(glyph->metrics.ascent-glyph->metrics.descent),
	  T1_GetFontName(FontID),
	  T1_GetCharName(FontID,(char)TestChar),
	  T1_errno);


  /* 
  printf("glyph->bits=%u\n", glyph->bits);
  printf("glyph->metrics.leftSideBearing=%u\n", glyph->metrics.leftSideBearing);
  printf("glyph->metrics.rightSideBearing=%u\n", glyph->metrics.rightSideBearing);
  printf("glyph->metrics.advanceX=%u\n", glyph->metrics.advanceX);
  printf("glyph->metrics.ascent=%u\n", glyph->metrics.ascent);
  printf("glyph->metrics.descent=%u\n", glyph->metrics.descent);
  printf("glyph->pFontCacheInfo=%u\n", glyph->pFontCacheInfo);
  */
	     
  if (glyph->bits != NULL) {
    ximage=XCreateImage( display,
			 DefaultVisual(display, screennumber),
			 DefaultDepth(display,screennumber), /* depths for bitmap is 1 */
			 ZPixmap, /* XYBitmap or XYPixmap */
			 0, /* No offset */
			 glyph->bits,
			 width,
			 height,
			 t1_pad,  /* lines padded to bytes */
			 0 /*PAD(width,8)/8*/  /* number of bytes per line */
			 );
    ximage->byte_order=xglyph_byte_order;
    XFreePixmap(display,pixmap);
    pixmap = XCreatePixmap(display,
			   XtWindow(TopLevel),
			   width,
			   height,
			   DefaultDepth(display,screennumber)    /* 8 */ /* depth */
			   );
    XPutImage(display,
	      pixmap,
	      DefaultGC( display, screennumber),
	      ximage,
	      0,  
	      0,  
	      0,  
	      0,  
	      width,
	      height
	      );
    XDestroyImage(ximage);
  }
  
  i=0;
  XtSetArg(args[i], XtNbitmap,NULL); i++;
  XtSetArg(args[i], XtNwidth,380); i++;
  XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
  XtSetArg(args[i], XtNbackgroundPixmap, XtUnspecifiedPixmap); i++;
  XtSetArg(args[i], XtNlabel, statusstring); i++;
  XtSetValues(labelstatus,args,i);
  
  i=0;
  XtSetArg( args[i], XtNwidth, width + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNheight, height + DOUBLEMARGIN); i++;
  if (glyph->bits != NULL) {
    XtSetArg( args[i], XtNbitmap, pixmap); i++;
  }
  else {
    XtSetArg( args[i], XtNbitmap, 0); i++;
  }
  XtSetArg( args[i], XtNresize, 1);i++;
  XtSetArg( args[i], XtNinternalWidth, 0); i++;
  XtSetArg( args[i], XtNinternalHeight, 0); i++;
  XtSetArg( args[i], XtNbackground, WhitePixel(display,screennumber)); i++;
  XtSetValues(OutputWindow ,args,i);
  
  i=0;
  XtSetArg( args[i], XtNwidth, width + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNheight, height + DOUBLEMARGIN); i++;
  if (glyph->bits != NULL) {
    XtSetArg( args[i], XtNbitmap, pixmap); i++;
  }
  else {
    XtSetArg( args[i], XtNbitmap, 0); i++;
  }
  XtSetArg( args[i], XtNresize, 1);i++;
  XtSetArg( args[i], XtNinternalWidth, 0); i++;
  XtSetArg( args[i], XtNinternalHeight, 0); i++;
  XtSetValues(Output ,args,i);
  
  glyph->bits=NULL;    /* Since XDestroyImage() free's this also! */
}



void aashowstring( Widget showstringbutton, XtPointer client_data, XtPointer call_data)
{
  int i,j,k,l,m,none_found;
  Arg args[10];
  char *theString='\0';
  char *ligtheString='\0';
  char *succs, *ligs;
  char buf_char;
  

  sscanf( XawDialogGetValueString(dialogfontid),"%d", &FontID);
  sscanf( XawDialogGetValueString(dialogsize),"%f", &Size);
  sscanf( XawDialogGetValueString(dialogangle),"%f", &Angle);
  sscanf( XawDialogGetValueString(dialogspace),"%ld", &Space);
  sscanf( XawDialogGetValueString(dialogdevres),"%d", &DeviceResolution);
  sscanf( XawDialogGetValueString(dialogstroke),"%d", &StrokeWidth);

  if (FontID<0 || FontID>=T1_GetNoFonts()) {
    sprintf(statusstring, "t1lib: FontID out of range!");
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }

  /* Ensure that font is loaded before any operation on the font */
  if ( T1_CheckForFontID( FontID) < 1 ) {
    T1_LoadFont( FontID);
  }
  
  if ( StrokeWidth == 0.0f ) {
    T1_ClearStrokeFlag( FontID);
  }
  else {
    T1_SetStrokeFlag( FontID);
    if ( T1_SetStrokeWidth( FontID, StrokeWidth) != 0 ) {
      sprintf( statusstring, "t1lib: Unable to setup strokewidth");
      i=0;
      XtSetArg(args[i], XtNbitmap,NULL); i++;
      XtSetArg(args[i], XtNwidth,380); i++;
      XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
      XtSetArg(args[i], XtNlabel, statusstring); i++;
      XtSetValues(labelstatus,args,i);
      return;
    }
  }
  
  if (CheckTransform()==0 && Angle==0.0){
    matrixP=NULL;
  }
  else{
    matrixP=T1_RotateMatrix( &matrix, Angle);
  }
  
  if (DeviceResolution!=last_resolution[FontID]){
    /* Delete all size dependent data for that font */
    for ( i=0; i<T1_GetNoFonts(); i++)
      T1_DeleteAllSizes( i);
    /* Establish new device resolution */
    T1_SetDeviceResolutions(DeviceResolution,DeviceResolution);
    /* Store current resolution */
    last_resolution[FontID]=DeviceResolution;
  }
  i=sscanf( XawDialogGetValueString(dialogencfile),"%s", (char *)EncodingFile);
  if (i==EOF) EncodingFile[0]=0;
  if (strcmp(EncodingFile,LastEncodingFile)){ /* encoding has changed */
    if (strcmp( EncodingFile, "")==0){ /* -> reset to internal encoding */
      for (i=0; i<T1_GetNoFonts(); i++){
	T1_DeleteAllSizes(i);
	T1_ReencodeFont( i, NULL);
      }
      /* Take care that newly loaded fonts are encoded according to current
	 encoding */
      T1_SetDefaultEncoding( NULL);
    }
    else{
      for (i=0; i<MAXENCODINGS; ){
	if ( encstruct[i].encfilename != NULL)
	  if (strcmp( encstruct[i].encfilename, EncodingFile)==0){
	    i++;
	    break;
	  }
	i++;
      }
      if (i==MAXENCODINGS){ /* Encoding from that file was not
			       already loaded -> so load it */
	i=0;
	while (encstruct[i].encfilename != NULL)
	  i++;
	encstruct[i].encoding=T1_LoadEncoding(EncodingFile);
	encstruct[i].encfilename=(char *)malloc(strlen(EncodingFile)+1);
	if (encstruct[i].encoding==NULL){
	  free( encstruct[i].encfilename);
	  encstruct[i].encfilename=NULL;
	}
	i++;
      }
      for (j=0; j<T1_GetNoFonts(); j++){
	T1_DeleteAllSizes(j);
	T1_ReencodeFont( j, encstruct[i-1].encoding);
      }
      /* Take care that newly loaded fonts are encoded according to current
	 encoding */
      T1_SetDefaultEncoding( encstruct[i-1].encoding);
    }
    strcpy( LastEncodingFile, EncodingFile);
  }
  sscanf( XawDialogGetValueString(dialogslant),"%f", &Slant);
  if (Slant!=lastSlant[FontID]){
    /* Delete all size dependent data */
    T1_DeleteAllSizes(FontID);
    T1_LoadFont(FontID);
    T1_SlantFont( FontID, Slant);
    lastSlant[FontID]=Slant;
  }
  sscanf( XawDialogGetValueString(dialogextent),"%f", &Extent);
  if (Extent!=lastExtent[FontID]){
    /* Delete all size dependent data */
    T1_DeleteAllSizes(FontID);
    T1_LoadFont(FontID);
    T1_ExtendFont( FontID, Extent);
    lastExtent[FontID]=Extent;
  }
  
  i=0;
  XtSetArg(args[i], XtNstring, &theString); i++;
  XtGetValues(dialogteststring,args,i);

  if (Size<=0.0){
    sprintf(statusstring, "t1lib: Size must be positive!");
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }

  /* Now comes the ligatur handling */
  if (strcmp(theString,"")==0)
    theString=(char *) TestString;
  i=strlen(theString);
  ligtheString=(char *)malloc((i+1)*sizeof(char));
  if (LigDetect){
    for (j=0,m=0;j<i;j++,m++){ /* Loop through the characters */
      if ((k=T1_QueryLigs( FontID, theString[j], &succs, &ligs))>0){  
	buf_char=theString[j];
	while (k>0){  
	  none_found=1;
	  for (l=0;l<k;l++){ /* Loop through the ligatures */
	    if (succs[l]==theString[j+1]){
	      buf_char=ligs[l];
	      j++;
	      none_found=0;
	      break;
	    }
	  }
	  if (none_found)
	    break;
	  k=T1_QueryLigs( FontID, buf_char, &succs, &ligs);
	}
	ligtheString[m]=buf_char;
      }
      else{ /* There are no ligatures */
	ligtheString[m]=theString[j];
      }
    }
    ligtheString[m]=0;
  }
  else {
    strcpy(ligtheString,theString);
  }

  /* Reset T1_errno: */
  T1_errno=0;
  /* Set antialiasing level */
  T1_AASetLevel( aalevel);
  /* Set the colors for Anti-Aliasing */
  T1_AASetGrayValues( aapixels[0],   /* white */
		      aapixels[4],
		      aapixels[8],
		      aapixels[12],
		      aapixels[16] ); /* black */
  T1_AAHSetGrayValues( aapixels);
  T1_AANSetGrayValues( aapixels[0], aapixels[16]);
  gettimeofday(time_ptr_start, void_ptr);
  
  if( extraflags & CHECK_CONCATGLYPHS){
    glyph2=T1_AASetString(FontID,(char *)ligtheString,0,Space,Modflag,Size,matrixP);
    glyph1=T1_CopyGlyph( glyph2);
    glyph2=T1_AASetString(FontID+1,(char *)ligtheString,0,Space,Modflag,Size,matrixP);
    glyph=T1_ConcatGlyphs( glyph1, glyph2, 0, 0, Modflag);
  }
  else if( extraflags & CHECK_CONCATOUTLINES){
    {
      T1_OUTLINE *path=NULL;
      path=T1_GetStringOutline(FontID,(char *)ligtheString,
			       0,Space,Modflag,Size,matrixP);
      T1_ConcatOutlines( path,
			 T1_GetMoveOutline( FontID, 1000, 0,  Modflag, Size, matrixP));
      T1_ConcatOutlines( path,
			 T1_GetStringOutline(FontID+1,(char *)ligtheString,
					     0,Space,Modflag,Size,matrixP));
      glyph=T1_AAFillOutline( path, Modflag);
    }
  }
  else{
    glyph=T1_AASetString(FontID,(char *)ligtheString,0,Space,Modflag,Size,matrixP);
  }
  
  gettimeofday(time_ptr_stop, void_ptr);
  
  free(ligtheString);
  
  
  if ( glyph == NULL){
    sprintf(statusstring, "t1lib: Couldn't generate Bitmap,\n(%s)", T1_StrError(T1_errno));
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }

  height=glyph->metrics.ascent - glyph->metrics.descent;
  overallwidth=glyph->metrics.rightSideBearing - glyph->metrics.leftSideBearing;
  width=overallwidth;
  
  /* Prepare status message: */
  sprintf(statusstring,"Elapsed time:        %ld Microseconds\nLeftSideBearing:     %d\nRightSideBearing:    %d\nAscent:              %d\nDescent:             %d\nAdvanceX:            %d\nAdvanceY:            %d\nBits Per Pixel:      %ld\nImage Size:          %ld Bytes\nPostScript Fontname: %s\nT1_errno:            %d\n",
	  time_diff(time_ptr_start,time_ptr_stop),
	  glyph->metrics.leftSideBearing, 
	  glyph->metrics.rightSideBearing, 
	  glyph->metrics.ascent,
	  glyph->metrics.descent,
	  glyph->metrics.advanceX,
	  glyph->metrics.advanceY,
	  glyph->bpp,
	  PAD(glyph->bpp*(glyph->metrics.rightSideBearing-glyph->metrics.leftSideBearing),XGLYPH_PAD)/8*(glyph->metrics.ascent-glyph->metrics.descent),
	  T1_GetFontName(FontID),
	  T1_errno);

  
  /*
  printf("glyph->bits=%u\n", glyph->bits);
  printf("glyph->metrics.leftSideBearing=%u\n", glyph->metrics.leftSideBearing);
  printf("glyph->metrics.rightSideBearing=%u\n", glyph->metrics.rightSideBearing);
  printf("glyph->metrics.advanceX=%u\n", glyph->metrics.advanceX);
  printf("glyph->metrics.ascent=%u\n", glyph->metrics.ascent);
  printf("glyph->metrics.descent=%u\n", glyph->metrics.descent);
  printf("glyph->pFontCacheInfo=%u\n", glyph->pFontCacheInfo);
  */
  
  if (glyph->bits != NULL) {
    ximage=XCreateImage( display,
			 DefaultVisual(display, screennumber),
			 DefaultDepth(display,screennumber),
			 ZPixmap, /* XYBitmap or XYPixmap */
			 0, /* No offset */
			 glyph->bits,
			 width,
			 height,
			 t1_pad,  /* lines padded to bytes */
			 0 /* number of bytes per line */
			 );
    ximage->byte_order=xglyph_byte_order; 
    XFreePixmap(display,pixmap);
    pixmap = XCreatePixmap(display,
			   XtWindow(TopLevel),  
			   width,
			   height,
			   DefaultDepth(display,screennumber)
			   );
    XPutImage(display,
	      pixmap,
	      DefaultGC( display, screennumber),
	      ximage,
	      0,  
	      0,  
	      0,  
	      0,  
	      width,
	      height
	      );
    XDestroyImage(ximage);
  }
  
  i=0;
  XtSetArg(args[i], XtNbitmap,NULL); i++;
  XtSetArg(args[i], XtNwidth,380); i++;
  XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
  XtSetArg(args[i], XtNbackgroundPixmap, XtUnspecifiedPixmap); i++;
  XtSetArg(args[i], XtNlabel, statusstring); i++;
  XtSetValues(labelstatus,args,i);
  
  i=0;
  XtSetArg( args[i], XtNwidth, width + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNheight, height + DOUBLEMARGIN); i++;
  if (glyph->bits != NULL) {
    XtSetArg( args[i], XtNbitmap, pixmap); i++;
  }
  else {
    XtSetArg( args[i], XtNbitmap, 0); i++;
  }
  XtSetArg( args[i], XtNresize, 1);i++;
  XtSetArg( args[i], XtNinternalWidth, 0); i++;
  XtSetArg( args[i], XtNinternalHeight, 0); i++;
  XtSetArg( args[i], XtNbackground, WhitePixel( display, screennumber)); i++;
  XtSetValues(OutputWindow ,args,i);
  
  i=0;
  XtSetArg( args[i], XtNwidth, width + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNheight, height + DOUBLEMARGIN); i++;
  if (glyph->bits != NULL) {
    XtSetArg( args[i], XtNbitmap, pixmap); i++;
  }
  else {
    XtSetArg( args[i], XtNbitmap, 0); i++;
  }
  XtSetArg( args[i], XtNresize, 1);i++;
  XtSetArg( args[i], XtNinternalWidth, 0); i++;
  XtSetArg( args[i], XtNinternalHeight, 0); i++;
  XtSetArg( args[i], XtNbackground, WhitePixel( display, screennumber)); i++;
  XtSetValues(Output ,args,i);
  
  glyph->bits=NULL;    /* Since XDestroyImage() free's this also! */
}



void showcharX( Widget showcharbutton, XtPointer client_data, XtPointer call_data)
{
  int i, j;
  Arg args[10];
  int tmp_width, tmp_height;
  
  
  sscanf( XawDialogGetValueString(dialogfontid),"%d", &FontID);
  sscanf( XawDialogGetValueString(dialogsize),"%f", &Size);
  sscanf( XawDialogGetValueString(dialogangle),"%f", &Angle);
  sscanf( XawDialogGetValueString(dialogtestcharacter), "%d", &TestChar);
  sscanf( XawDialogGetValueString(dialogdevres),"%d", &DeviceResolution);
  sscanf( XawDialogGetValueString(dialogstroke),"%d", &StrokeWidth);

  if (FontID<0 || FontID>=T1_GetNoFonts()) {
    sprintf(statusstring, "t1lib: FontID out of range!");
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }

  /* Ensure that font is loaded before any operation on the font */
  if ( T1_CheckForFontID( FontID) < 1 ) {
    T1_LoadFont( FontID);
  }
  
  if ( StrokeWidth == 0.0f ) {
    T1_ClearStrokeFlag( FontID);
  }
  else {
    T1_SetStrokeFlag( FontID);
    if ( T1_SetStrokeWidth( FontID, StrokeWidth) != 0 ) {
      sprintf( statusstring, "t1lib: Unable to setup strokewidth");
      i=0;
      XtSetArg(args[i], XtNbitmap,NULL); i++;
      XtSetArg(args[i], XtNwidth,380); i++;
      XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
      XtSetArg(args[i], XtNlabel, statusstring); i++;
      XtSetValues(labelstatus,args,i);
      return;
    }
  }
  
  if (CheckTransform()==0 && Angle==0.0){
    matrixP=NULL;
  }
  else{
    matrixP=T1_RotateMatrix( &matrix, Angle);
  }
  
  if (DeviceResolution!=last_resolution[FontID]){
    /* Delete all size dependent data for that font */
    for ( i=0; i<T1_GetNoFonts(); i++)
      T1_DeleteAllSizes( i);
    /* Establish new device resolution */
    T1_SetDeviceResolutions(DeviceResolution,DeviceResolution);
    /* Store current resolution */
    last_resolution[FontID]=DeviceResolution;
  }
  i=sscanf( XawDialogGetValueString(dialogencfile),"%s", (char *)EncodingFile);
  if (i==EOF) EncodingFile[0]=0;
  if (strcmp(EncodingFile,LastEncodingFile)){ /* encoding has changed */
    if (strcmp( EncodingFile, "")==0){ /* -> reset to internal encoding */
      for (i=0; i<T1_GetNoFonts(); i++){
	T1_DeleteAllSizes(i);
	T1_ReencodeFont( i, NULL);
      }
      /* Take care that newly loaded fonts are encoded according to current
	 encoding */
      T1_SetDefaultEncoding( NULL);
    }
    else{
      for (i=0; i<MAXENCODINGS; ){
	if ( encstruct[i].encfilename != NULL)
	  if (strcmp( encstruct[i].encfilename, EncodingFile)==0){
	    i++;
	    break;
	  }
	i++;
      }
      if (i==MAXENCODINGS){ /* Encoding from that file was not
			       already loaded -> so load it */
	i=0;
	while (encstruct[i].encfilename != NULL)
	  i++;
	encstruct[i].encoding=T1_LoadEncoding(EncodingFile);
	encstruct[i].encfilename=(char *)malloc(strlen(EncodingFile)+1);
	if (encstruct[i].encoding==NULL){
	  free( encstruct[i].encfilename);
	  encstruct[i].encfilename=NULL;
	}
	i++;
      }
      for (j=0; j<T1_GetNoFonts(); j++){
	T1_DeleteAllSizes(j);
	T1_ReencodeFont( j, encstruct[i-1].encoding);
      }
      /* Take care that newly loaded fonts are encoded according to current
	 encoding */
      T1_SetDefaultEncoding( encstruct[i-1].encoding);
    }
    strcpy( LastEncodingFile, EncodingFile);
  }
  sscanf( XawDialogGetValueString(dialogslant),"%f", &Slant);
  if (Slant!=lastSlant[FontID]){
    /* Delete all size dependent data */
    T1_DeleteAllSizes(FontID);
    T1_LoadFont(FontID);
    T1_SlantFont( FontID, Slant);
    lastSlant[FontID]=Slant;
  }
  sscanf( XawDialogGetValueString(dialogextent),"%f", &Extent);
  if (Extent!=lastExtent[FontID]){
    /* Delete all size dependent data */
    T1_DeleteAllSizes(FontID);
    T1_LoadFont(FontID);
    T1_ExtendFont( FontID, Extent);
    lastExtent[FontID]=Extent;
  }
    
  if (Size<=0.0){
    sprintf(statusstring, "t1lib: Size must be positive!");
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }

  tmp_width=2 * XOUTPUT_HALFHSIZE;
  tmp_height=2 * XOUTPUT_HALFVSIZE;
  
  if (tmp_pixmap==0)
    tmp_pixmap=XCreatePixmap( display,
			      XtWindow(TopLevel),
			      tmp_width,
			      tmp_height,
			      depth
			      );
  /* Put some background in pixmap for demonstration of transparent-mode: */
  if (Opacity==0){
    XSetForeground( display, DefaultGC( display, screennumber), pink4.pixel);
    XFillRectangle( display, tmp_pixmap, DefaultGC( display, screennumber),
		    0, 0, tmp_width, tmp_height);
    XSetForeground( display, DefaultGC( display, screennumber), yellow.pixel);
    XFillRectangle( display, tmp_pixmap, DefaultGC( display, screennumber),
		    tmp_width/4, tmp_height/4, tmp_width/2, tmp_height/2);
    XSetForeground( display, DefaultGC( display, screennumber), fg);
  }
  else{
  XSetForeground( display, DefaultGC( display, screennumber), bg);
  XFillRectangle( display, tmp_pixmap, DefaultGC( display, screennumber),
		  0, 0, tmp_width, tmp_height);
  }
  XSetForeground( display, DefaultGC( display, screennumber), fg);
  XSetBackground( display, DefaultGC( display, screennumber), bg);
  /* Synchronize display in order to make the time measurement more acurate */
  XSync( display, True);
  /* Reset T1_errno: */
  T1_errno=0;
  gettimeofday(time_ptr_start, void_ptr);
  if ( extraflags & CHECK_SET_RECT )
    glyph=T1_SetRectX( tmp_pixmap, DefaultGC( display, screennumber), Opacity,
		       XOUTPUT_HALFHSIZE, XOUTPUT_HALFVSIZE, /* x_dest, y_dest */
		       FontID, Size, 1000.0, 1000.0, matrixP);
  else
    glyph=T1_SetCharX( tmp_pixmap, DefaultGC( display, screennumber), Opacity,
		       XOUTPUT_HALFHSIZE, XOUTPUT_HALFVSIZE, /* x_dest, y_dest */
		       FontID, TestChar, Size, matrixP);  
  gettimeofday(time_ptr_stop, void_ptr);

  if ((extraflags & NO_GRID)==0){
    XSetForeground( display, DefaultGC( display, screennumber), gridcolor1.pixel);
    XDrawLine( display, tmp_pixmap, DefaultGC( display, screennumber), 
	       XOUTPUT_HALFHSIZE, 0,  /* X1, Y1 */
	       XOUTPUT_HALFHSIZE, 2 * XOUTPUT_HALFVSIZE);
    XDrawLine( display, tmp_pixmap, DefaultGC( display, screennumber),
	       0, XOUTPUT_HALFVSIZE,  /* X1, Y1 */
	       2 * XOUTPUT_HALFHSIZE, XOUTPUT_HALFVSIZE);
    XSetForeground( display, DefaultGC( display, screennumber), gridcolor2.pixel);
    if (glyph!=NULL){
      XDrawLine( display, tmp_pixmap, DefaultGC( display, screennumber), 
		 XOUTPUT_HALFHSIZE + glyph->metrics.advanceX - CROSS_SIZE,
		 XOUTPUT_HALFVSIZE - glyph->metrics.advanceY,  
		 XOUTPUT_HALFHSIZE + glyph->metrics.advanceX + CROSS_SIZE,
		 XOUTPUT_HALFVSIZE - glyph->metrics.advanceY);  
      XDrawLine( display, tmp_pixmap, DefaultGC( display, screennumber),
		 XOUTPUT_HALFHSIZE + glyph->metrics.advanceX,
		 XOUTPUT_HALFVSIZE - glyph->metrics.advanceY - CROSS_SIZE,
		 XOUTPUT_HALFHSIZE + glyph->metrics.advanceX,
		 XOUTPUT_HALFVSIZE - glyph->metrics.advanceY + CROSS_SIZE);
    }
  }
  XSetForeground( display, DefaultGC( display, screennumber), fg);
  
  /* Check for errors */
  if (glyph==NULL) {
    sprintf(statusstring, "t1lib: Couldn't generate Bitmap,\n(%s)", T1_StrError(T1_errno));
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }
  
  /* Prepare status message: */
  sprintf(statusstring,"Elapsed time:        %ld Microseconds\nLeftSideBearing:     %d\nRightSideBearing:    %d\nAscent:              %d\nDescent:             %d\nAdvanceX:            %d\nAdvanceY:            %d\nBits Per Pixel:      %ld\nImage Size:          %ld Bytes\nPostScript Fontname: %s\nCharactername:       %s\nT1_errno:            %d\n",
	  time_diff(time_ptr_start,time_ptr_stop),
	  glyph->metrics.leftSideBearing, 
	  glyph->metrics.rightSideBearing, 
	  glyph->metrics.ascent,
	  glyph->metrics.descent,
	  glyph->metrics.advanceX,
	  glyph->metrics.advanceY,
	  glyph->bpp,
	  PAD(glyph->bpp*(glyph->metrics.rightSideBearing-glyph->metrics.leftSideBearing),XGLYPH_PAD)/8*(glyph->metrics.ascent-glyph->metrics.descent),
	  T1_GetFontName(FontID),
	  T1_GetCharName(FontID,(char)TestChar),
	  T1_errno);

  /* Finally, set the resources: */
  i=0;
  XtSetArg(args[i], XtNbitmap,NULL); i++;
  XtSetArg(args[i], XtNwidth,380); i++;
  XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
  XtSetArg(args[i], XtNbackgroundPixmap, XtUnspecifiedPixmap); i++;
  XtSetArg(args[i], XtNlabel, statusstring); i++;
  XtSetValues(labelstatus,args,i);

  i=0;
  XtSetArg( args[i], XtNwidth, tmp_width + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNheight, tmp_height + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNbitmap, tmp_pixmap); i++; 
  XtSetArg( args[i], XtNresize, 1);i++;
  XtSetArg( args[i], XtNinternalWidth, 0); i++;
  XtSetArg( args[i], XtNinternalHeight, 0); i++;
  XtSetArg( args[i], XtNbackground, outbg); i++;
  XtSetValues(OutputWindow ,args,i); 
  
  i=0;
  XtSetArg( args[i], XtNwidth, tmp_width + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNheight, tmp_height + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNbitmap, tmp_pixmap); i++; 
  XtSetArg( args[i], XtNresize, 1);i++;
  XtSetArg( args[i], XtNinternalWidth, 0); i++;
  XtSetArg( args[i], XtNinternalHeight, 0); i++;
  XtSetValues(Output ,args,i);
   
}



void showstringX( Widget showstringbutton, XtPointer client_data, XtPointer call_data)
{
  int i,j,k,l,m,none_found;
  Arg args[10];
  int tmp_width, tmp_height;
  
  char *theString='\0';
  char *ligtheString='\0';
  char *succs, *ligs;
  char buf_char;
  

  sscanf( XawDialogGetValueString(dialogfontid),"%d", &FontID);
  sscanf( XawDialogGetValueString(dialogsize),"%f", &Size);
  sscanf( XawDialogGetValueString(dialogangle),"%f", &Angle);
  sscanf( XawDialogGetValueString(dialogspace),"%ld", &Space);
  sscanf( XawDialogGetValueString(dialogdevres),"%d", &DeviceResolution);
  sscanf( XawDialogGetValueString(dialogstroke),"%d", &StrokeWidth);

  if (FontID<0 || FontID>=T1_GetNoFonts()) {
    sprintf(statusstring, "t1lib: FontID out of range!");
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }

  /* Ensure that font is loaded before any operation on the font */
  if ( T1_CheckForFontID( FontID) < 1 ) {
    T1_LoadFont( FontID);
  }
  
  if ( StrokeWidth == 0.0f ) {
    T1_ClearStrokeFlag( FontID);
  }
  else {
    T1_SetStrokeFlag( FontID);
    if ( T1_SetStrokeWidth( FontID, StrokeWidth) != 0 ) {
      sprintf( statusstring, "t1lib: Unable to setup strokewidth");
      i=0;
      XtSetArg(args[i], XtNbitmap,NULL); i++;
      XtSetArg(args[i], XtNwidth,380); i++;
      XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
      XtSetArg(args[i], XtNlabel, statusstring); i++;
      XtSetValues(labelstatus,args,i);
      return;
    }
  }
  
  if (CheckTransform()==0 && Angle==0.0){
    matrixP=NULL;
  }
  else{
    matrixP=T1_RotateMatrix( &matrix, Angle);
  }
  
  if (DeviceResolution!=last_resolution[FontID]){
    /* Delete all size dependent data for that font */
    for ( i=0; i<T1_GetNoFonts(); i++)
      T1_DeleteAllSizes( i);
    /* Establish new device resolution */
    T1_SetDeviceResolutions(DeviceResolution,DeviceResolution);
    /* Store current resolution */
    last_resolution[FontID]=DeviceResolution;
  }
  i=sscanf( XawDialogGetValueString(dialogencfile),"%s", (char *)EncodingFile);
  if (i==EOF) EncodingFile[0]=0;
  if (strcmp(EncodingFile,LastEncodingFile)){ /* encoding has changed */
    if (strcmp( EncodingFile, "")==0){ /* -> reset to internal encoding */
      for (i=0; i<T1_GetNoFonts(); i++){
	T1_DeleteAllSizes(i);
	T1_ReencodeFont( i, NULL);
      }
      /* Take care that newly loaded fonts are encoded according to current
	 encoding */
      T1_SetDefaultEncoding( NULL);
    }
    else{
      for (i=0; i<MAXENCODINGS; ){
	if ( encstruct[i].encfilename != NULL)
	  if (strcmp( encstruct[i].encfilename, EncodingFile)==0){
	    i++;
	    break;
	  }
	i++;
      }
      if (i==MAXENCODINGS){ /* Encoding from that file was not
			       already loaded -> so load it */
	i=0;
	while (encstruct[i].encfilename != NULL)
	  i++;
	encstruct[i].encoding=T1_LoadEncoding(EncodingFile);
	encstruct[i].encfilename=(char *)malloc(strlen(EncodingFile)+1);
	if (encstruct[i].encoding==NULL){
	  free( encstruct[i].encfilename);
	  encstruct[i].encfilename=NULL;
	}
	i++;
      }
      for (j=0; j<T1_GetNoFonts(); j++){
	T1_DeleteAllSizes(j);
	T1_ReencodeFont( j, encstruct[i-1].encoding);
      }
      /* Take care that newly loaded fonts are encoded according to current
	 encoding */
      T1_SetDefaultEncoding( encstruct[i-1].encoding);
    }
    strcpy( LastEncodingFile, EncodingFile);
  }
  sscanf( XawDialogGetValueString(dialogslant),"%f", &Slant);
  if (Slant!=lastSlant[FontID]){
    /* Delete all size dependent data */
    T1_DeleteAllSizes(FontID);
    T1_LoadFont(FontID);
    T1_SlantFont( FontID, Slant);
    lastSlant[FontID]=Slant;
  }
  sscanf( XawDialogGetValueString(dialogextent),"%f", &Extent);
  if (Extent!=lastExtent[FontID]){
    /* Delete all size dependent data */
    T1_DeleteAllSizes(FontID);
    T1_LoadFont(FontID);
    T1_ExtendFont( FontID, Extent);
    lastExtent[FontID]=Extent;
  }
  
  i=0;
  XtSetArg(args[i], XtNstring, &theString); i++;
  XtGetValues(dialogteststring,args,i);

  if (Size<=0.0){
    sprintf(statusstring, "t1lib: Size must be positive!");
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }

  /* Now comes the ligatur handling */
  if (strcmp(theString,"")==0)
    theString=(char *) TestString;
  i=strlen(theString);
  ligtheString=(char *)malloc((i+1)*sizeof(char));
  if (LigDetect){
    for (j=0,m=0;j<i;j++,m++){ /* Loop through the characters */
      if ((k=T1_QueryLigs( FontID, theString[j], &succs, &ligs))>0){  
	buf_char=theString[j];
	while (k>0){  
	  none_found=1;
	  for (l=0;l<k;l++){ /* Loop through the ligatures */
	    if (succs[l]==theString[j+1]){
	      buf_char=ligs[l];
	      j++;
	      none_found=0;
	      break;
	    }
	  }
	  if (none_found)
	    break;
	  k=T1_QueryLigs( FontID, buf_char, &succs, &ligs);
	}
	ligtheString[m]=buf_char;
      }
      else{ /* There are no ligatures */
	ligtheString[m]=theString[j];
      }
    }
    ligtheString[m]=0;
  }
  else {
    strcpy(ligtheString,theString);
  }
  
  tmp_width=2 * XOUTPUT_HALFHSIZE;
  tmp_height=2 * XOUTPUT_HALFVSIZE;
  
  if (tmp_pixmap==0)
    tmp_pixmap=XCreatePixmap( display,
			      XtWindow(TopLevel),
			      tmp_width,
			      tmp_height,
			      depth
			      );
  /* Put some background in pixmap for demonstration of transparent-mode: */
  if (Opacity==0){
    XSetForeground( display, DefaultGC( display, screennumber), pink4.pixel);
    XFillRectangle( display, tmp_pixmap, DefaultGC( display, screennumber),
		    0, 0, tmp_width, tmp_height);
    XSetForeground( display, DefaultGC( display, screennumber), yellow.pixel);
    XFillRectangle( display, tmp_pixmap, DefaultGC( display, screennumber),
		    tmp_width/4, tmp_height/4, tmp_width/2, tmp_height/2);
    XSetForeground( display, DefaultGC( display, screennumber), fg);
  }
  else{
  XSetForeground( display, DefaultGC( display, screennumber), bg);
  XFillRectangle( display, tmp_pixmap, DefaultGC( display, screennumber),
		  0, 0, tmp_width, tmp_height);
  }
  XSetForeground( display, DefaultGC( display, screennumber), fg);
  XSetBackground( display, DefaultGC( display, screennumber), bg);
  /* Synchronize display in order to make the time measurement more acurate */
  XSync( display, True);
  /* Reset T1_errno: */
  T1_errno=0;
  /* Draw the string into temporary pixmap */
  gettimeofday(time_ptr_start, void_ptr);

  if( extraflags & CHECK_PERFORMANCE){
    j=0;
    for (i=0; j<600; i++){
      j=(int)(Size*1.2*i);
      glyph=T1_SetStringX( XtWindow(OutputWindow2), DefaultGC( display, screennumber), Opacity,
			   0, j, /* x_dest, y_dest */
			   FontID, (char *)ligtheString,
			   0, Space, Modflag, Size, matrixP);
    }
  }
  else{
    glyph=T1_SetStringX( tmp_pixmap, DefaultGC( display, screennumber), Opacity,
			 XOUTPUT_HALFHSIZE, XOUTPUT_HALFVSIZE, 
			 FontID, (char *)ligtheString,
			 0, Space, Modflag, Size, matrixP);
  }
  
  gettimeofday(time_ptr_stop, void_ptr);

  if ((extraflags & NO_GRID)==0){
    XSetForeground( display, DefaultGC( display, screennumber), gridcolor1.pixel);
    XDrawLine( display, tmp_pixmap, DefaultGC( display, screennumber), 
	       XOUTPUT_HALFHSIZE, 0,  /* X1, Y1 */
	       XOUTPUT_HALFHSIZE, 2 * XOUTPUT_HALFVSIZE);
    XDrawLine( display, tmp_pixmap, DefaultGC( display, screennumber),
	       0, XOUTPUT_HALFVSIZE,  /* X1, Y1 */
	       2 * XOUTPUT_HALFHSIZE, XOUTPUT_HALFVSIZE);
    XSetForeground( display, DefaultGC( display, screennumber), gridcolor2.pixel);
    if (glyph!=NULL){
      XDrawLine( display, tmp_pixmap, DefaultGC( display, screennumber), 
		 XOUTPUT_HALFHSIZE + glyph->metrics.advanceX - CROSS_SIZE,
		 XOUTPUT_HALFVSIZE - glyph->metrics.advanceY,  
		 XOUTPUT_HALFHSIZE + glyph->metrics.advanceX + CROSS_SIZE,
		 XOUTPUT_HALFVSIZE - glyph->metrics.advanceY);  
      XDrawLine( display, tmp_pixmap, DefaultGC( display, screennumber),
		 XOUTPUT_HALFHSIZE + glyph->metrics.advanceX,
		 XOUTPUT_HALFVSIZE - glyph->metrics.advanceY - CROSS_SIZE,
		 XOUTPUT_HALFHSIZE + glyph->metrics.advanceX,
		 XOUTPUT_HALFVSIZE - glyph->metrics.advanceY + CROSS_SIZE);
    }
  }
  
  XSetForeground( display, DefaultGC( display, screennumber), fg);
  
  /* Free the ligature-converted string */
  free(ligtheString);
  
  /* Check for errors */
  if (glyph==NULL) {
    sprintf(statusstring, "t1lib: Couldn't generate Bitmap,\n(%s)", T1_StrError(T1_errno));
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }
  
  /* Prepare status message: */
  sprintf(statusstring,"Elapsed time:        %ld Microseconds\nLeftSideBearing:     %d\nRightSideBearing:    %d\nAscent:              %d\nDescent:             %d\nAdvanceX:            %d\nAdvanceY:            %d\nBits Per Pixel:      %ld\nImage Size:          %ld Bytes\nPostScript Fontname: %s\nT1_errno:            %d\n",
	  time_diff(time_ptr_start,time_ptr_stop),
	  glyph->metrics.leftSideBearing, 
	  glyph->metrics.rightSideBearing, 
	  glyph->metrics.ascent,
	  glyph->metrics.descent,
	  glyph->metrics.advanceX,
	  glyph->metrics.advanceY,
	  glyph->bpp,
	  PAD(glyph->bpp*(glyph->metrics.rightSideBearing-glyph->metrics.leftSideBearing),XGLYPH_PAD)/8*(glyph->metrics.ascent-glyph->metrics.descent),
	  T1_GetFontName(FontID),
	  T1_errno);

  
  /* Finally, set resources */
  i=0;
  XtSetArg(args[i], XtNbitmap,NULL); i++;
  XtSetArg(args[i], XtNwidth,380); i++;
  XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
  XtSetArg(args[i], XtNbackgroundPixmap, XtUnspecifiedPixmap); i++;
  XtSetArg(args[i], XtNlabel, statusstring); i++;
  XtSetValues(labelstatus,args,i);
  
  i=0;
  XtSetArg( args[i], XtNwidth, tmp_width + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNheight, tmp_height + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNbitmap, tmp_pixmap); i++;
  XtSetArg( args[i], XtNresize, 1);i++;
  XtSetArg( args[i], XtNinternalWidth, 0); i++;
  XtSetArg( args[i], XtNinternalHeight, 0); i++;
  XtSetArg( args[i], XtNbackground, outbg); i++;
  XtSetValues(OutputWindow ,args,i);
  
  i=0;
  XtSetArg( args[i], XtNwidth, tmp_width + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNheight, tmp_height + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNbitmap, tmp_pixmap); i++;
  XtSetArg( args[i], XtNresize, 1);i++;
  XtSetArg( args[i], XtNinternalWidth, 0); i++;
  XtSetArg( args[i], XtNinternalHeight, 0); i++;
  XtSetArg( args[i], XtNbackground, outbg); i++;
  XtSetValues(Output ,args,i);
  
}



void aashowcharX( Widget showcharbutton, XtPointer client_data, XtPointer call_data)
{
  int i, j;
  Arg args[10];
  int tmp_width, tmp_height;
  
  
  sscanf( XawDialogGetValueString(dialogfontid),"%d", &FontID);
  sscanf( XawDialogGetValueString(dialogsize),"%f", &Size);
  sscanf( XawDialogGetValueString(dialogangle),"%f", &Angle);
  sscanf( XawDialogGetValueString(dialogtestcharacter), "%d", &TestChar);
  sscanf( XawDialogGetValueString(dialogdevres),"%d", &DeviceResolution);
  sscanf( XawDialogGetValueString(dialogstroke),"%d", &StrokeWidth);

  if (FontID<0 || FontID>=T1_GetNoFonts()) {
    sprintf(statusstring, "t1lib: FontID out of range!");
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }

  /* Ensure that font is loaded before any operation on the font */
  if ( T1_CheckForFontID( FontID) < 1 ) {
    T1_LoadFont( FontID);
  }
  
  if ( StrokeWidth == 0.0f ) {
    T1_ClearStrokeFlag( FontID);
  }
  else {
    T1_SetStrokeFlag( FontID);
    if ( T1_SetStrokeWidth( FontID, StrokeWidth) != 0 ) {
      sprintf( statusstring, "t1lib: Unable to setup strokewidth");
      i=0;
      XtSetArg(args[i], XtNbitmap,NULL); i++;
      XtSetArg(args[i], XtNwidth,380); i++;
      XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
      XtSetArg(args[i], XtNlabel, statusstring); i++;
      XtSetValues(labelstatus,args,i);
      return;
    }
  }
  
  if (CheckTransform()==0 && Angle==0.0){
    matrixP=NULL;
  }
  else{
    matrixP=T1_RotateMatrix( &matrix, Angle);
  }
  
  if (DeviceResolution!=last_resolution[FontID]){
    /* Delete all size dependent data for that font */
    for ( i=0; i<T1_GetNoFonts(); i++)
      T1_DeleteAllSizes( i);
    /* Establish new device resolution */
    T1_SetDeviceResolutions(DeviceResolution,DeviceResolution);
    /* Store current resolution */
    last_resolution[FontID]=DeviceResolution;
  }
  i=sscanf( XawDialogGetValueString(dialogencfile),"%s", (char *)EncodingFile);
  if (i==EOF) EncodingFile[0]=0;
  if (strcmp(EncodingFile,LastEncodingFile)){ /* encoding has changed */
    if (strcmp( EncodingFile, "")==0){ /* -> reset to internal encoding */
      for (i=0; i<T1_GetNoFonts(); i++){
	T1_DeleteAllSizes(i);
	T1_ReencodeFont( i, NULL);
      }
      /* Take care that newly loaded fonts are encoded according to current
	 encoding */
      T1_SetDefaultEncoding( NULL);
    }
    else{
      for (i=0; i<MAXENCODINGS; ){
	if ( encstruct[i].encfilename != NULL)
	  if (strcmp( encstruct[i].encfilename, EncodingFile)==0){
	    i++;
	    break;
	  }
	i++;
      }
      if (i==MAXENCODINGS){ /* Encoding from that file was not
			       already loaded -> so load it */
	i=0;
	while (encstruct[i].encfilename != NULL)
	  i++;
	encstruct[i].encoding=T1_LoadEncoding(EncodingFile);
	encstruct[i].encfilename=(char *)malloc(strlen(EncodingFile)+1);
	if (encstruct[i].encoding==NULL){
	  free( encstruct[i].encfilename);
	  encstruct[i].encfilename=NULL;
	}
	i++;
      }
      for (j=0; j<T1_GetNoFonts(); j++){
	T1_DeleteAllSizes(j);
	T1_ReencodeFont( j, encstruct[i-1].encoding);
      }
      /* Take care that newly loaded fonts are encoded according to current
	 encoding */
      T1_SetDefaultEncoding( encstruct[i-1].encoding);
    }
    strcpy( LastEncodingFile, EncodingFile);
  }
  sscanf( XawDialogGetValueString(dialogslant),"%f", &Slant);
  if (Slant!=lastSlant[FontID]){
    /* Delete all size dependent data */
    T1_DeleteAllSizes(FontID);
    T1_LoadFont(FontID);
    T1_SlantFont( FontID, Slant);
    lastSlant[FontID]=Slant;
  }
  sscanf( XawDialogGetValueString(dialogextent),"%f", &Extent);
  if (Extent!=lastExtent[FontID]){
    /* Delete all size dependent data */
    T1_DeleteAllSizes(FontID);
    T1_LoadFont(FontID);
    T1_ExtendFont( FontID, Extent);
    lastExtent[FontID]=Extent;
  }
    
  if (Size<=0.0){
    sprintf(statusstring, "t1lib: Size must be positive!");
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }

  tmp_width=2 * XOUTPUT_HALFHSIZE;
  tmp_height=2 * XOUTPUT_HALFVSIZE;
  
  if (tmp_pixmap==0)
    tmp_pixmap=XCreatePixmap( display,
			      XtWindow(TopLevel),
			      tmp_width,
			      tmp_height,
			      depth
			      );
  /* Put some background in pixmap for demonstration of transparent-mode: */
  if (Opacity==0){
    XSetForeground( display, DefaultGC( display, screennumber), pink4.pixel);
    XFillRectangle( display, tmp_pixmap, DefaultGC( display, screennumber),
		    0, 0, tmp_width, tmp_height);
    XSetForeground( display, DefaultGC( display, screennumber), yellow.pixel);
    XFillRectangle( display, tmp_pixmap, DefaultGC( display, screennumber),
		    tmp_width/4, tmp_height/4, tmp_width/2, tmp_height/2);
    XSetForeground( display, DefaultGC( display, screennumber), fg);
  }
  else{
  XSetForeground( display, DefaultGC( display, screennumber), bg);
  XFillRectangle( display, tmp_pixmap, DefaultGC( display, screennumber),
		  0, 0, tmp_width, tmp_height);
  }
  XSetForeground( display, DefaultGC( display, screennumber), fg);
  XSetBackground( display, DefaultGC( display, screennumber), bg);
  /* Synchronize display in order to make the time measurement more acurate */
  XSync( display, True);
  /* Reset T1_errno: */
  T1_errno=0;
  /* Set antialiasing level */
  T1_AASetLevel( aalevel);
  gettimeofday(time_ptr_start, void_ptr);
  if ( extraflags & CHECK_SET_RECT )
    glyph=T1_AASetRectX( tmp_pixmap, DefaultGC( display, screennumber), Opacity,
			 XOUTPUT_HALFHSIZE, XOUTPUT_HALFVSIZE, /* x_dest, y_dest */
			 FontID, Size, 1000.0, 1000.0, matrixP);
  else
    glyph=T1_AASetCharX( tmp_pixmap, DefaultGC( display, screennumber), Opacity,
			 XOUTPUT_HALFHSIZE, XOUTPUT_HALFVSIZE, /* x_dest, y_dest */
			 FontID, TestChar, Size, matrixP);  
  gettimeofday(time_ptr_stop, void_ptr);

  if ((extraflags & NO_GRID)==0){
    XSetForeground( display, DefaultGC( display, screennumber), gridcolor1.pixel);
    XDrawLine( display, tmp_pixmap, DefaultGC( display, screennumber), 
	       XOUTPUT_HALFHSIZE, 0,  /* X1, Y1 */
	       XOUTPUT_HALFHSIZE, 2 * XOUTPUT_HALFVSIZE);
    XDrawLine( display, tmp_pixmap, DefaultGC( display, screennumber),
	       0, XOUTPUT_HALFVSIZE,  /* X1, Y1 */
	       2 * XOUTPUT_HALFHSIZE, XOUTPUT_HALFVSIZE);
    XSetForeground( display, DefaultGC( display, screennumber), gridcolor2.pixel);
    if (glyph!=NULL){
      XDrawLine( display, tmp_pixmap, DefaultGC( display, screennumber), 
		 XOUTPUT_HALFHSIZE + glyph->metrics.advanceX - CROSS_SIZE,
		 XOUTPUT_HALFVSIZE - glyph->metrics.advanceY,  
		 XOUTPUT_HALFHSIZE + glyph->metrics.advanceX + CROSS_SIZE,
		 XOUTPUT_HALFVSIZE - glyph->metrics.advanceY);  
      XDrawLine( display, tmp_pixmap, DefaultGC( display, screennumber),
		 XOUTPUT_HALFHSIZE + glyph->metrics.advanceX,
		 XOUTPUT_HALFVSIZE - glyph->metrics.advanceY - CROSS_SIZE,
		 XOUTPUT_HALFHSIZE + glyph->metrics.advanceX,
		 XOUTPUT_HALFVSIZE - glyph->metrics.advanceY + CROSS_SIZE);
    }
  }
  
  XSetForeground( display, DefaultGC( display, screennumber), fg);
  
  /* Check for errors */
  if (glyph==NULL) {
    sprintf(statusstring, "t1lib: Couldn't generate Bitmap,\n(%s)", T1_StrError(T1_errno));
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }
  
  /* Prepare status message: */
  sprintf(statusstring,"Elapsed time:        %ld Microseconds\nLeftSideBearing:     %d\nRightSideBearing:    %d\nAscent:              %d\nDescent:             %d\nAdvanceX:            %d\nAdvanceY:            %d\nBits Per Pixel:      %ld\nImage Size:          %ld Bytes\nPostScript Fontname: %s\nCharactername:       %s\nT1_errno:            %d\n",
	  time_diff(time_ptr_start,time_ptr_stop),
	  glyph->metrics.leftSideBearing, 
	  glyph->metrics.rightSideBearing, 
	  glyph->metrics.ascent,
	  glyph->metrics.descent,
	  glyph->metrics.advanceX,
	  glyph->metrics.advanceY,
	  glyph->bpp,
	  PAD(glyph->bpp*(glyph->metrics.rightSideBearing-glyph->metrics.leftSideBearing),XGLYPH_PAD)/8*(glyph->metrics.ascent-glyph->metrics.descent),
	  T1_GetFontName(FontID),
	  T1_GetCharName(FontID,(char)TestChar),
	  T1_errno);

  /* Finally, set the resources: */
  i=0;
  XtSetArg(args[i], XtNbitmap,NULL); i++;
  XtSetArg(args[i], XtNwidth,380); i++;
  XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
  XtSetArg(args[i], XtNbackgroundPixmap, XtUnspecifiedPixmap); i++;
  XtSetArg(args[i], XtNlabel, statusstring); i++;
  XtSetValues(labelstatus,args,i);

  i=0;
  XtSetArg( args[i], XtNwidth, tmp_width + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNheight, tmp_height + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNbitmap, tmp_pixmap); i++; 
  XtSetArg( args[i], XtNresize, 1);i++;
  XtSetArg( args[i], XtNinternalWidth, 0); i++;
  XtSetArg( args[i], XtNinternalHeight, 0); i++;
  XtSetArg( args[i], XtNbackground, outbg); i++;
  XtSetValues(OutputWindow ,args,i); 
  
  i=0;
  XtSetArg( args[i], XtNwidth, tmp_width + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNheight, tmp_height + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNbitmap, tmp_pixmap); i++; 
  XtSetArg( args[i], XtNresize, 1);i++;
  XtSetArg( args[i], XtNinternalWidth, 0); i++;
  XtSetArg( args[i], XtNinternalHeight, 0); i++;
  XtSetValues(Output ,args,i);
   
}



void aashowstringX( Widget showstringbutton, XtPointer client_data, XtPointer call_data)
{
  int i,j,k,l,m,none_found;
  Arg args[10];
  int tmp_width, tmp_height;

  char *theString='\0';
  char *ligtheString='\0';
  char *succs, *ligs;
  char buf_char;
  

  sscanf( XawDialogGetValueString(dialogfontid),"%d", &FontID);
  sscanf( XawDialogGetValueString(dialogsize),"%f", &Size);
  sscanf( XawDialogGetValueString(dialogangle),"%f", &Angle);
  sscanf( XawDialogGetValueString(dialogspace),"%ld", &Space);
  sscanf( XawDialogGetValueString(dialogdevres),"%d", &DeviceResolution);
  sscanf( XawDialogGetValueString(dialogstroke),"%d", &StrokeWidth);

  if (FontID<0 || FontID>=T1_GetNoFonts()) {
    sprintf(statusstring, "t1lib: FontID out of range!");
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }

  /* Ensure that font is loaded before any operation on the font */
  if ( T1_CheckForFontID( FontID) < 1 ) {
    T1_LoadFont( FontID);
  }
  
  if ( StrokeWidth == 0.0f ) {
    T1_ClearStrokeFlag( FontID);
  }
  else {
    T1_SetStrokeFlag( FontID);
    if ( T1_SetStrokeWidth( FontID, StrokeWidth) != 0 ) {
      sprintf( statusstring, "t1lib: Unable to setup strokewidth");
      i=0;
      XtSetArg(args[i], XtNbitmap,NULL); i++;
      XtSetArg(args[i], XtNwidth,380); i++;
      XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
      XtSetArg(args[i], XtNlabel, statusstring); i++;
      XtSetValues(labelstatus,args,i);
      return;
    }
  }
  
  if (CheckTransform()==0 && Angle==0.0){
    matrixP=NULL;
  }
  else{
    matrixP=T1_RotateMatrix( &matrix, Angle);
  }
  
  if (DeviceResolution!=last_resolution[FontID]){
    /* Delete all size dependent data for that font */
    for ( i=0; i<T1_GetNoFonts(); i++)
      T1_DeleteAllSizes( i);
    /* Establish new device resolution */
    T1_SetDeviceResolutions(DeviceResolution,DeviceResolution);
    /* Store current resolution */
    last_resolution[FontID]=DeviceResolution;
  }
  i=sscanf( XawDialogGetValueString(dialogencfile),"%s", (char *)EncodingFile);
  if (i==EOF) EncodingFile[0]=0;
  if (strcmp(EncodingFile,LastEncodingFile)!=0){ /* encoding has changed */
    if (strcmp( EncodingFile, "")==0){ /* -> reset to internal encoding */
      for (i=0; i<T1_GetNoFonts(); i++){
	T1_DeleteAllSizes(i);
	T1_ReencodeFont( i, NULL);
      }
      /* Take care that newly loaded fonts are encoded according to current
	 encoding */
      T1_SetDefaultEncoding( NULL);
    }
    else{
      for (i=0; i<MAXENCODINGS; ){
	if ( encstruct[i].encfilename != NULL)
	  if (strcmp( encstruct[i].encfilename, EncodingFile)==0){
	    i++;
	    break;
	  }
	i++;
      }
      if (i==MAXENCODINGS){ /* Encoding from that file was not
			       already loaded -> so load it */
	i=0;
	while (encstruct[i].encfilename != NULL)
	  i++;
	encstruct[i].encoding=T1_LoadEncoding(EncodingFile);
	encstruct[i].encfilename=(char *)malloc(strlen(EncodingFile)+1);
	if (encstruct[i].encoding==NULL){
	  free( encstruct[i].encfilename);
	  encstruct[i].encfilename=NULL;
	}
	i++;
      }
      else
      for (j=0; j<T1_GetNoFonts(); j++){
	T1_DeleteAllSizes(j);
	T1_ReencodeFont( j, encstruct[i-1].encoding);
      }
      /* Take care that newly loaded fonts are encoded according to current
	 encoding */
      T1_SetDefaultEncoding( encstruct[i-1].encoding);
    }
    strcpy( LastEncodingFile, EncodingFile);
  }
  sscanf( XawDialogGetValueString(dialogslant),"%f", &Slant);
  if (Slant!=lastSlant[FontID]){
    /* Delete all size dependent data */
    T1_DeleteAllSizes(FontID);
    T1_LoadFont(FontID);
    T1_SlantFont( FontID, Slant);
    lastSlant[FontID]=Slant;
  }
  sscanf( XawDialogGetValueString(dialogextent),"%f", &Extent);
  if (Extent!=lastExtent[FontID]){
    /* Delete all size dependent data */
    T1_DeleteAllSizes(FontID);
    T1_LoadFont(FontID);
    T1_ExtendFont( FontID, Extent);
    lastExtent[FontID]=Extent;
  }
  
  i=0;
  XtSetArg(args[i], XtNstring, &theString); i++;
  XtGetValues(dialogteststring,args,i);

  if (Size<=0.0){
    sprintf(statusstring, "t1lib: Size must be positive!");
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }

  /* Now comes the ligatur handling */
  if (strcmp(theString,"")==0)
    theString=(char *) TestString;
  i=strlen(theString);
  ligtheString=(char *)malloc((i+1)*sizeof(char));
  if (LigDetect){
    for (j=0,m=0;j<i;j++,m++){ /* Loop through the characters */
      if ((k=T1_QueryLigs( FontID, theString[j], &succs, &ligs))>0){  
	buf_char=theString[j];
	while (k>0){  
	  none_found=1;
	  for (l=0;l<k;l++){ /* Loop through the ligatures */
	    if (succs[l]==theString[j+1]){
	      buf_char=ligs[l];
	      j++;
	      none_found=0;
	      break;
	    }
	  }
	  if (none_found)
	    break;
	  k=T1_QueryLigs( FontID, buf_char, &succs, &ligs);
	}
	ligtheString[m]=buf_char;
      }
      else{ /* There are no ligatures */
	ligtheString[m]=theString[j];
      }
    }
    ligtheString[m]=0;
  }
  else {
    strcpy(ligtheString,theString);
  }
  
  tmp_width=2 * XOUTPUT_HALFHSIZE;
  tmp_height=2 * XOUTPUT_HALFVSIZE;
  
  if (tmp_pixmap==0)
    tmp_pixmap=XCreatePixmap( display,
			      XtWindow(TopLevel),
			      tmp_width,
			      tmp_height,
			      depth
			      );
  
  /* Put some background in pixmap for demonstration of transparent-mode: */
  if (Opacity==0){
    XSetForeground( display, DefaultGC( display, screennumber), pink4.pixel);
    XFillRectangle( display, tmp_pixmap, DefaultGC( display, screennumber),
		    0, 0, tmp_width, tmp_height);
    XSetForeground( display, DefaultGC( display, screennumber), yellow.pixel);
    XFillRectangle( display, tmp_pixmap, DefaultGC( display, screennumber),
		    tmp_width/4, tmp_height/4, tmp_width/2, tmp_height/2);
    XSetForeground( display, DefaultGC( display, screennumber), fg);
  }
  else{
  XSetForeground( display, DefaultGC( display, screennumber), bg);
  XFillRectangle( display, tmp_pixmap, DefaultGC( display, screennumber),
		  0, 0, tmp_width, tmp_height);
  }
  XSetForeground( display, DefaultGC( display, screennumber), fg);
  XSetBackground( display, DefaultGC( display, screennumber), bg);
  /* Synchronize display in order to make the time measurement more acurate */
  XSync( display, True);
  /* Reset T1_errno: */
  T1_errno=0;
  /* Set antialiasing level */
  T1_AASetLevel( aalevel);
  /* Draw the string into temporary pixmap */
  gettimeofday(time_ptr_start, void_ptr);
  
  if( extraflags & CHECK_PERFORMANCE){
    j=0;
    for (i=0; j<600; i++){
      j=(int)(Size*1.2*i);
      glyph=T1_AASetStringX( XtWindow(OutputWindow2), DefaultGC( display, screennumber), Opacity,
			     0, j, /* x_dest, y_dest */
			     FontID, (char *)ligtheString,
			     0, Space, Modflag, Size, matrixP);
    }
  }
  else{
    glyph=T1_AASetStringX( tmp_pixmap, DefaultGC( display, screennumber), Opacity,
			   XOUTPUT_HALFHSIZE, XOUTPUT_HALFVSIZE, /* x_dest, y_dest */
			   FontID, (char *)ligtheString,
			   0, Space, Modflag, Size, matrixP);
  }
  
  gettimeofday(time_ptr_stop, void_ptr);

  if ((extraflags & NO_GRID)==0){
    XSetForeground( display, DefaultGC( display, screennumber), gridcolor1.pixel);
    XDrawLine( display, tmp_pixmap, DefaultGC( display, screennumber), 
	       XOUTPUT_HALFHSIZE, 0,  /* X1, Y1 */
	       XOUTPUT_HALFHSIZE, 2 * XOUTPUT_HALFVSIZE);
    XDrawLine( display, tmp_pixmap, DefaultGC( display, screennumber),
	       0, XOUTPUT_HALFVSIZE,  /* X1, Y1 */
	       2 * XOUTPUT_HALFHSIZE, XOUTPUT_HALFVSIZE);
    XSetForeground( display, DefaultGC( display, screennumber), gridcolor2.pixel);
    if (glyph!=NULL){
      XDrawLine( display, tmp_pixmap, DefaultGC( display, screennumber), 
		 XOUTPUT_HALFHSIZE + glyph->metrics.advanceX - CROSS_SIZE,
		 XOUTPUT_HALFVSIZE - glyph->metrics.advanceY,  
		 XOUTPUT_HALFHSIZE + glyph->metrics.advanceX + CROSS_SIZE,
		 XOUTPUT_HALFVSIZE - glyph->metrics.advanceY);  
      XDrawLine( display, tmp_pixmap, DefaultGC( display, screennumber),
		 XOUTPUT_HALFHSIZE + glyph->metrics.advanceX,
		 XOUTPUT_HALFVSIZE - glyph->metrics.advanceY - CROSS_SIZE,
		 XOUTPUT_HALFHSIZE + glyph->metrics.advanceX,
		 XOUTPUT_HALFVSIZE - glyph->metrics.advanceY + CROSS_SIZE);
    }
  }
  
  XSetForeground( display, DefaultGC( display, screennumber), fg);
  
  /* Free the ligature-converted string */
  free(ligtheString);
  
  /* Check for errors */
  if (glyph==NULL) {
    sprintf(statusstring, "t1lib: Couldn't generate Bitmap,\n(%s)", T1_StrError(T1_errno));
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }
  
  /* Prepare status message: */
  sprintf(statusstring,"Elapsed time:        %ld Microseconds\nLeftSideBearing:     %d\nRightSideBearing:    %d\nAscent:              %d\nDescent:             %d\nAdvanceX:            %d\nAdvanceY:            %d\nBits Per Pixel:      %ld\nImage Size:          %ld Bytes\nPostScript Fontname: %s\nT1_errno:            %d\n",
	  time_diff(time_ptr_start,time_ptr_stop),
	  glyph->metrics.leftSideBearing, 
	  glyph->metrics.rightSideBearing, 
	  glyph->metrics.ascent,
	  glyph->metrics.descent,
	  glyph->metrics.advanceX,
	  glyph->metrics.advanceY,
	  glyph->bpp,
	  PAD(glyph->bpp*(glyph->metrics.rightSideBearing-glyph->metrics.leftSideBearing),XGLYPH_PAD)/8*(glyph->metrics.ascent-glyph->metrics.descent),
	  T1_GetFontName(FontID),
	  T1_errno);


  
  /* Finally, set resources */
  i=0;
  XtSetArg(args[i], XtNbitmap,NULL); i++;
  XtSetArg(args[i], XtNwidth,380); i++;
  XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
  XtSetArg(args[i], XtNbackgroundPixmap, XtUnspecifiedPixmap); i++;
  XtSetArg(args[i], XtNlabel, statusstring); i++;
  XtSetValues(labelstatus,args,i);
  
  i=0;
  XtSetArg( args[i], XtNwidth, tmp_width + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNheight, tmp_height + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNbitmap, tmp_pixmap); i++;
  XtSetArg( args[i], XtNresize, 1);i++;
  XtSetArg( args[i], XtNinternalWidth, 0); i++;
  XtSetArg( args[i], XtNinternalHeight, 0); i++;
  XtSetArg( args[i], XtNbackground, outbg); i++;
  XtSetValues(OutputWindow ,args,i);
  
  i=0;
  XtSetArg( args[i], XtNwidth, tmp_width + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNheight, tmp_height + DOUBLEMARGIN); i++;
  XtSetArg( args[i], XtNbitmap, tmp_pixmap); i++;
  XtSetArg( args[i], XtNresize, 1);i++;
  XtSetArg( args[i], XtNinternalWidth, 0); i++;
  XtSetArg( args[i], XtNinternalHeight, 0); i++;
  XtSetArg( args[i], XtNbackground, outbg); i++;
  XtSetValues(Output ,args,i);
  
}


void fonttable( Widget fonttablebutton, XtPointer client_data, XtPointer call_data)
{

  int i, j;
  
  Arg args[10];
  static Pixmap pixmap=0;
  int x, y;
  int CellLeftMargin, CellTopMargin;
  int ColAdvance, RowAdvance;
  int width, height;
  int FontID, DeviceResolution;
  float Size, Extent, Slant;
  float scale;
  
  BBox fontbbox, tmpbbox;
  
  sscanf( XawDialogGetValueString(dialogfontid),"%d", &FontID);
  sscanf( XawDialogGetValueString(dialogsize),"%f", &Size);
  sscanf( XawDialogGetValueString(dialogangle),"%f", &Angle);
  sscanf( XawDialogGetValueString(dialogspace),"%ld", &Space);
  sscanf( XawDialogGetValueString(dialogdevres),"%d", &DeviceResolution);
  sscanf( XawDialogGetValueString(dialogstroke),"%d", &StrokeWidth);

  if (FontID<0 || FontID>=T1_GetNoFonts()) {
    sprintf(statusstring, "t1lib: FontID out of range!");
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }

  /* Ensure that font is loaded before any operation on the font */
  if ( T1_CheckForFontID( FontID) < 1 ) {
    T1_LoadFont( FontID);
  }
  
  if ( StrokeWidth == 0.0f ) {
    T1_ClearStrokeFlag( FontID);
  }
  else {
    T1_SetStrokeFlag( FontID);
    if ( T1_SetStrokeWidth( FontID, StrokeWidth) != 0 ) {
      sprintf( statusstring, "t1lib: Unable to setup strokewidth");
      i=0;
      XtSetArg(args[i], XtNbitmap,NULL); i++;
      XtSetArg(args[i], XtNwidth,380); i++;
      XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
      XtSetArg(args[i], XtNlabel, statusstring); i++;
      XtSetValues(labelstatus,args,i);
      return;
    }
  }
  
  /* We don't obey rotation when displaying a fonttable */
  matrixP=NULL;
  
  if (DeviceResolution!=last_resolution[FontID]){
    /* Delete all size dependent data for that font */
    for ( i=0; i<T1_GetNoFonts(); i++)
      T1_DeleteAllSizes( i);
    /* Establish new device resolution */
    T1_SetDeviceResolutions(DeviceResolution,DeviceResolution);
    /* Store current resolution */
    last_resolution[FontID]=DeviceResolution;
  }
  i=sscanf( XawDialogGetValueString(dialogencfile),"%s", (char *)EncodingFile);
  if (i==EOF) EncodingFile[0]=0;
  if (strcmp(EncodingFile,LastEncodingFile)!=0){ /* encoding has changed */
    if (strcmp( EncodingFile, "")==0){ /* -> reset to internal encoding */
      for (i=0; i<T1_GetNoFonts(); i++){
	T1_DeleteAllSizes(i);
	T1_ReencodeFont( i, NULL);
      }
      /* Take care that newly loaded fonts are encoded according to current
	 encoding */
      T1_SetDefaultEncoding( NULL);
    }
    else{
      for (i=0; i<MAXENCODINGS; ){
	if ( encstruct[i].encfilename != NULL)
	  if (strcmp( encstruct[i].encfilename, EncodingFile)==0){
	    i++;
	    break;
	  }
	i++;
      }
      if (i==MAXENCODINGS){ /* Encoding from that file was not
			       already loaded -> so load it */
	i=0;
	while (encstruct[i].encfilename != NULL)
	  i++;
	encstruct[i].encoding=T1_LoadEncoding(EncodingFile);
	encstruct[i].encfilename=(char *)malloc(strlen(EncodingFile)+1);
	if (encstruct[i].encoding==NULL){
	  free( encstruct[i].encfilename);
	  encstruct[i].encfilename=NULL;
	}
	i++;
      }
      else
      for (j=0; j<T1_GetNoFonts(); j++){
	T1_DeleteAllSizes(j);
	T1_ReencodeFont( j, encstruct[i-1].encoding);
      }
      /* Take care that newly loaded fonts are encoded according to current
	 encoding */
      T1_SetDefaultEncoding( encstruct[i-1].encoding);
    }
    strcpy( LastEncodingFile, EncodingFile);
  }
  sscanf( XawDialogGetValueString(dialogslant),"%f", &Slant);
  if (Slant!=lastSlant[FontID]){
    /* Delete all size dependent data */
    T1_DeleteAllSizes(FontID);
    T1_LoadFont(FontID);
    T1_SlantFont( FontID, Slant);
    lastSlant[FontID]=Slant;
  }
  sscanf( XawDialogGetValueString(dialogextent),"%f", &Extent);
  if (Extent!=lastExtent[FontID]){
    /* Delete all size dependent data */
    T1_DeleteAllSizes(FontID);
    T1_LoadFont(FontID);
    T1_ExtendFont( FontID, Extent);
    lastExtent[FontID]=Extent;
  }
  
  if (Size<=0.0){
    sprintf(statusstring, "t1lib: Size must be positive!");
    i=0;
    XtSetArg(args[i], XtNbitmap,NULL); i++;
    XtSetArg(args[i], XtNwidth,380); i++;
    XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
    XtSetArg(args[i], XtNlabel, statusstring); i++;
    XtSetValues(labelstatus,args,i);
    return;
  }
  
  /* We have read all parameters, now lets get font bbox in order to be
     able to compute the cell-metrics */
  T1_LoadFont( FontID);
  fontbbox=T1_GetFontBBox( FontID);

  /* we check for a valid fontbbox, otherwise we compute it ourselves */
  if ( fontbbox.llx==0 &&
       fontbbox.lly==0 &&
       fontbbox.urx==0 &&
       fontbbox.ury==0 ){
    T1_PrintLog( "fonttable()", "Re-Computing FontBBox", T1LOG_DEBUG);
    for (i=0; i<256; i++){
      tmpbbox=T1_GetCharBBox( FontID, i);
      if (tmpbbox.llx<fontbbox.llx)
	fontbbox.llx=tmpbbox.llx;
      if (tmpbbox.lly<fontbbox.lly)
	fontbbox.lly=tmpbbox.lly;
      if (tmpbbox.urx>fontbbox.urx)
	fontbbox.urx=tmpbbox.urx;
      if (tmpbbox.ury>fontbbox.ury)
	fontbbox.ury=tmpbbox.ury;
    }
  }
       
  /* We scale the font bounding box according to extent, slant and
     device resolution. For the slant, we assume that the font contains
     at least one character which reaches to the upper right corner and
     the same for lower left corner. */ 
  scale=DeviceResolution/72.0;
  fontbbox.urx=fontbbox.urx+(int)((float)fontbbox.ury*Slant);
  fontbbox.llx=fontbbox.llx+(int)((float)fontbbox.lly*Slant);
  fontbbox.urx=(int)((float)fontbbox.urx*Extent);
  fontbbox.llx=(int)((float)fontbbox.llx*Extent);
  fontbbox.llx=(int)((float)fontbbox.llx*scale);
  fontbbox.urx=(int)((float)fontbbox.urx*scale);
  fontbbox.lly=(int)((float)fontbbox.lly*scale);
  fontbbox.ury=(int)((float)fontbbox.ury*scale);
  
  /* Left and top margin of the char-origins with respect upper left corner
     of cell */
  CellTopMargin=SIMPLEMARGIN + fontbbox.ury;
  if (fontbbox.llx < 0){
    CellLeftMargin= - fontbbox.llx;
  }
  else{
    CellLeftMargin=0;
  }
  /* How much to advance in which direction when incrementing */
  ColAdvance=CellLeftMargin + fontbbox.urx ;
  RowAdvance=CellTopMargin - fontbbox.lly ;
  /* Scale the charspace values */
  CellTopMargin = (int) floor((double)CellTopMargin*Size/1000.0+0.5)+SIMPLEMARGIN;
  CellLeftMargin = (int) floor((double)CellLeftMargin*Size/1000.0+0.5)+SIMPLEMARGIN;
  ColAdvance = (int) floor((double)ColAdvance*Size/1000.0+0.5)+DOUBLEMARGIN;
  RowAdvance = (int) floor((double)RowAdvance*Size/1000.0+0.5)+DOUBLEMARGIN;
  /* Overall width and height of map */
  width=1+(16*ColAdvance);
  height=1+(16*RowAdvance);

  /* We clip to a window as large as the screen in this function */
  if (width > 1024)
    width=WidthOfScreen(screen);
  if (height > 768)
    height=HeightOfScreen(screen);
  
  /* Create pixmap of appropriate size, */
  if (pixmap!=0)
    XFreePixmap( display, pixmap);
  pixmap=XCreatePixmap( display,
			XtWindow(TopLevel),
			width,
			height,
			depth
			);
  /* We always use opaque mode */
  XSetForeground( display, DefaultGC( display, screennumber), bg);
  XFillRectangle( display, pixmap, DefaultGC( display, screennumber),
		  0, 0, width, height);
  XSetForeground( display, DefaultGC( display, screennumber), fg);
  XSetBackground( display, DefaultGC( display, screennumber), bg);
  
  /* Draw cell grid: */
  for (j=0; j<17; j++){
    XDrawLine( display, pixmap, DefaultGC( display, screennumber), 
	       j*ColAdvance, 0,
	       j*ColAdvance, height);
  }
  for (j=0; j<17; j++){
    XDrawLine( display, pixmap, DefaultGC( display, screennumber), 
	       0, j*RowAdvance,
	       width, j*RowAdvance);
  }

  /* Reset T1_errno: */
  T1_errno=0;
  /* Set antialiasing level */
  T1_AASetLevel( aalevel);
  /* Draw characters into pixmap */
  for ( i=0; i<16; ) { /* row-loop */
    for ( j=0; j<16; j++) {
      x=1+(j*ColAdvance)+CellLeftMargin;
      y=1+CellTopMargin+(i*RowAdvance);
      glyph=T1_AASetCharX( pixmap, DefaultGC( display, screennumber), 1,
			   x, y, FontID, (char) (i*16+j), Size, matrixP);
      if (x>width) {
	sprintf( msg_buf, "Clipping row %d horizontally at column %d", i, j);
	T1_PrintLog( "fonttable()", msg_buf, T1LOG_STATISTIC);
	break;
      }
      
    }
    if (y>height) {
      sprintf( msg_buf, "Clipping vertically at row %d", i);
      T1_PrintLog( "fonttable()", msg_buf, T1LOG_STATISTIC);
      break;
    }
    i++;
  }

  /* If font was not loadable */
  if (T1_GetFontName( FontID)==NULL)
    sprintf(statusstring, "t1lib: Can't get font name, T1_errno=%d!", T1_errno);
  else
    sprintf(statusstring,"Font %s, final T1_errno = %d\n %s",
	    T1_GetFontName( FontID), T1_errno, T1_StrError(T1_errno));
  
  /* Finally, set resources */
  i=0;
  XtSetArg(args[i], XtNbitmap,NULL); i++;
  XtSetArg(args[i], XtNwidth,380); i++;
  XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
  XtSetArg(args[i], XtNbackgroundPixmap, XtUnspecifiedPixmap); i++;
  XtSetArg(args[i], XtNlabel, statusstring); i++;
  XtSetValues(labelstatus,args,i);
  
  i=0;
  XtSetArg( args[i], XtNwidth, width); i++;
  XtSetArg( args[i], XtNheight, height); i++;
  XtSetArg( args[i], XtNbitmap, pixmap); i++;
  XtSetArg( args[i], XtNresize, 1);i++;
  XtSetArg( args[i], XtNinternalWidth, 0); i++;
  XtSetArg( args[i], XtNinternalHeight, 0); i++;
  XtSetArg( args[i], XtNbackground, outbg); i++;
  XtSetValues(OutputWindow ,args,i);
  
  i=0;
  XtSetArg( args[i], XtNwidth, width); i++;
  XtSetArg( args[i], XtNheight, height); i++;
  XtSetArg( args[i], XtNbitmap, pixmap); i++;
  XtSetArg( args[i], XtNresize, 1);i++;
  XtSetArg( args[i], XtNinternalWidth, 0); i++;
  XtSetArg( args[i], XtNinternalHeight, 0); i++;
  XtSetArg( args[i], XtNbackground, outbg); i++;
  XtSetValues(Output ,args,i);
  
  
}

  

/* This function shows the About-message */
void showabout( Widget aboutbutton, XtPointer client_data, XtPointer call_data)
{
  int i;
  Arg args[10];
  Pixmap about=0;
  XGCValues xgcvalues;
  GC gc;
  static char ident[80]="";
  
#define T1GCMASK GCForeground | GCBackground 

  if (about==0) {
    about=XCreatePixmap( display, XtWindow(TopLevel),
			 380, MESSAGEBOXHEIGHT, depth);
  }
  gc=DefaultGC( display, screennumber);
    
  XGetGCValues( display, gc, T1GCMASK, &xgcvalues);
  fg=xgcvalues.foreground;
  bg=xgcvalues.background;
  
  XSetForeground( display, gc, white.pixel);
  XFillRectangle( display, about, gc, 0, 0, 380, MESSAGEBOXHEIGHT);
  XSetForeground( display, gc, black.pixel);
  XSetBackground( display, gc, white.pixel);

  /* Set antialiasing level */
  T1_AASetLevel( aalevel);
  sprintf( ident, "This is xglyph, T1Lib Version %s", T1_GetLibIdent());
  glyph=T1_AASetStringX( about, gc, 1, 10, 30, /* x_dest, y_dest */
			 0, ident,
			 0, 0.0, T1_UNDERLINE | T1_KERNING, 20.0, NULL);
  glyph=T1_AASetStringX( about, gc, 1, 10, 60, /* x_dest, y_dest */
			 0, "xglyph is an interactive tool for illustrating some",
			 0, 0.0, T1_KERNING, 15.0, NULL);
  glyph=T1_AASetStringX( about, gc, 1, 10, 80, /* x_dest, y_dest */
			 0, "of the T1Lib features. T1Lib is distributed under",
			 0, 0.0, T1_KERNING, 15.0, NULL);
  glyph=T1_AASetStringX( about, gc, 1, 10, 100, /* x_dest, y_dest */
			 0, "the GNU General Public Library License (LGPL).",
			 0, 0.0, T1_KERNING, 15.0, NULL);
  glyph=T1_AASetStringX( about, gc, 1, 10, 133, /* x_dest, y_dest */
			 0, "Enjoy it!",
			 0, 0.0, T1_KERNING, 15.0, NULL);
  XSetForeground( display, gc, fg);
  XSetBackground( display, gc, bg);

  i=0;
  XtSetArg(args[i], XtNwidth,380); i++;
  XtSetArg(args[i], XtNheight, MESSAGEBOXHEIGHT); i++;
  XtSetArg(args[i], XtNbackgroundPixmap, about); i++;
  XtSetArg(args[i], XtNlabel, ""); i++;
  XtSetValues(labelstatus,args,i);
}



/* This function returns a time difference in Microseconds, provided that
   the time difference is not greater than approximately 35 minutes. */
long time_diff(struct timeval *time_ptr_start, struct timeval *time_ptr_stop)
{
  return((time_ptr_stop->tv_sec -
	  time_ptr_start->tv_sec)*1000000
	 +(time_ptr_stop->tv_usec -
	   time_ptr_start->tv_usec));
}

 
/* T1_ComputeAAColorsX(): Compute the antialiasing colors in dependency
   of foreground and background */
int ComputeAAColorsX( unsigned long fg, unsigned long bg, int nolevels)
{

  static unsigned long last_fg;
  static unsigned long last_bg;
  unsigned long delta_red, delta_green, delta_blue;
  int i;
  int nocolors=0;
  
  
  aacolors[0].pixel=bg;
  aacolors[nolevels-1].pixel=fg;

  if ((fg==last_fg)&&(bg==last_bg))
    return(nocolors);
  
  /* Get RGB values for fore- and background */
  XQueryColor( display, DefaultColormap(display,screennumber), &aacolors[0]);
  XQueryColor( display, DefaultColormap(display,screennumber), &aacolors[nolevels-1]);
  delta_red   = (aacolors[nolevels-1].red - aacolors[0].red)/(nolevels-1);
  delta_green = (aacolors[nolevels-1].green - aacolors[0].green)/(nolevels-1);
  delta_blue  = (aacolors[nolevels-1].blue - aacolors[0].blue)/(nolevels-1);
  aapixels[0]=aacolors[0].pixel;
  aapixels[nolevels-1]=aacolors[nolevels-1].pixel;
  
  for (i=1; i<nolevels-1; i++){
    aacolors[i].red   = aacolors[i-1].red + delta_red;
    aacolors[i].green = aacolors[i-1].green + delta_green;
    aacolors[i].blue  = aacolors[i-1].blue + delta_blue;
    /* Allocate color in current palette */
    if (XAllocColor( display, DefaultColormap(display,screennumber),
		     &aacolors[i])!=0){
      aapixels[i]=aacolors[i].pixel;
      nocolors++;
    }
    
  }
  
  return(nocolors);

}



void printusage( int max)
{
  int i=0;

  if (max==0)
    max=100000; /* should be large enough :) */

  fprintf(stdout, "xglyph -- t1lib Version %s\n\n", T1_GetLibIdent());
  fprintf(stdout, "Usage: xglyph [options] [fontfile1 [fontfile2 [...]]]\n\n");
  fprintf(stdout, "where options is one of:\n");
  while ( xglyphoptions[i]!=NULL && i<max){
    if (i==3) /* We print the possible values here. */
      fprintf( stdout, "\t\t%s {8,16,32}\n", xglyphoptions[i++]);
    else
      fprintf( stdout, "\t\t%s\n", xglyphoptions[i++]);
  }
  
  fprintf(stdout, "For additional information see the t1lib manual!\n");
  return;
}

	 
int CheckTransform( void)
{

  int err;
  float x1, x2, x3, x4;
  
  err=sscanf( XawDialogGetValueString(dialogtmatrix), "%f,%f,%f,%f",
	      &x1, &x2, &x3, &x4 );
  if ( (x1 == (float) 1.0) &&
       (x2 == (float) 0.0) &&
       (x3 == (float) 0.0) &&
       (x4 == (float) 1.0) ){
    matrix.cxx=x1;
    matrix.cyx=x2;
    matrix.cxy=x3;
    matrix.cyy=x4;
    return( 0);
  }
  else{
    matrix.cxx=x1;
    matrix.cyx=x2;
    matrix.cxy=x3;
    matrix.cyy=x4;
    return( 1);
  }
}

