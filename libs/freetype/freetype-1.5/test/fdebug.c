/****************************************************************************}
{*                                                                          *}
{*  The FreeType project - a Free and Portable Quality TrueType Renderer.   *}
{*                                                                          *}
{*  Copyright 1996-2001 by                                                  *}
{*  D. Turner, R.Wilhelm, and W. Lemberg                                    *}
{*                                                                          *}
{*  fdebug : A very simple TrueType bytecode debugger.                      *}
{*                                                                          *}
{*  NOTE : You must compile the interpreter with the DEBUG_INTERPRETER      *}
{*         macro defined in order to link this program!                     *}
{*                                                                          *}
{****************************************************************************/

#include <math.h>    /* libc ANSI */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freetype.h"
#include "tttypes.h"
#include "ttdebug.h"
#include "ttobjs.h"


#ifdef UNIX
#ifndef HAVE_POSIX_TERMIOS
#include <sys/ioctl.h>
#include <termio.h>
#else
#ifndef HAVE_TCGETATTR
#define HAVE_TCGETATTR
#endif /* HAVE_TCGETATTR */
#ifndef HAVE_TCSETATTR
#define HAVE_TCSETATTR
#endif /* HAVE_TCSETATTR */
#include <termios.h>
#endif /* HAVE_POSIX_TERMIOS */
#endif

  /* MAGIC: This variable is only defined in ttinterp.c if the      */
  /*         macro DEBUG_INTERPRETER is set.  It specifies the code */
  /*         range to debug.  By default, it is TT_CodeRange_Glyph. */
  /*                                                                */
  extern
  int  debug_coderange;


#define  Font_Buff_Size  256000        /* this buffer holds all     */
                                       /* font specific data.       */

  TT_Engine    engine;
  TT_Face      face;
  TT_Instance  instance;
  TT_Glyph     glyph;
  TT_Error     error;

  TT_Face_Properties  properties;

  int    num_glyphs;
  int    ptsize;

  Int           Fail;
  Int           Num;
  int           mode = 2;
  unsigned char autorun;

/*********************************************************************
 *
 * Usage : print usage message
 *
 *********************************************************************/

  static
  void Usage( const char* execname )
  {
    TT_Message( "fdebug: a simple TrueType bytecode debugger - part of the FreeType project\n" );
    TT_Message( "--------------------------------------------------------------------------\n\n");
    TT_Message( "Usage: %s glyphnum ppem fontname[.ttf]\n", execname );
    TT_Message( "    or %s --cvt ppem fontname[.ttf]\n", execname );
    TT_Message( "    or %s --font fontname[.ttf]\n\n", execname );
    exit( EXIT_FAILURE );
  }


/*********************************************************************
 *
 * Init_Keyboard : set the input file descriptor to char-by-char
 *                 mode on Unix..
 *
 *********************************************************************/

#ifdef UNIX

 struct termios  old_termio;
 
 static
 void Init_Keyboard( void )
 {
   struct termios  termio;


#ifndef HAVE_TCGETATTR
   ioctl( 0, TCGETS, &old_termio ); 
#else
   tcgetattr( 0, &old_termio );
#endif

   termio = old_termio;
   
   termio.c_lflag &= ~(ICANON+ECHO+ECHOE+ECHOK+ECHONL+ECHOKE);

#ifndef HAVE_TCSETATTR
   ioctl( 0, TCSETS, &termio );
#else
   tcsetattr( 0, TCSANOW, &termio );
#endif
 }

 static
 void Reset_Keyboard( voi )
 {
#ifndef HAVE_TCSETATTR
   ioctl( 0, TCSETS, &old_termio );
#else
   tcsetattr( 0, TCSANOW, &old_termio );
#endif

 }

#else

 static
 void Init_Keyboard( void )
 {
 }

 static
 void Reset_Keyboard( voi )
 {
 }

#endif

  static
  void  Print_Banner( void )
  {
    TT_Message( "fdebug    - a simple TrueType bytecode debugger for FreeType\n" );
    TT_Message( "------------------------------------------------------------\n" );
    TT_Message( "type '?' for help - copyright 1996-2000 the FreeType Project\n\n" );
  }


  static
  void  Error( const char* message,
               const char* filename )
  {
    static char tempstr[256];

    sprintf( tempstr, "ERROR (%s): %s\n", filename, message );
    TT_Message( tempstr );

    sprintf( tempstr, "  code = 0x%04lx\n", error );
    TT_Message( tempstr );

    Reset_Keyboard();
    exit( EXIT_FAILURE );
  }


  static
  void  Init_Face( const char*  filename )
  {
    error = TT_Init_FreeType(&engine);
    if (error) Error( "could not initialise FreeType", filename );

    /* open face object */
    error = TT_Open_Face( engine, filename, &face );
    if (error) Error( "could not find or open file", filename );

    /* get properties */
    TT_Get_Face_Properties( face, &properties );
    num_glyphs = properties.num_Glyphs;

    /* create instance */
    error = TT_New_Instance( face, &instance );
    if (error) Error( "could not create instance", filename );

    error = TT_New_Glyph( face, &glyph );
    if (error) Error( "could not create glyph container", filename );

    TT_Set_Instance_Resolutions( instance, 96, 96 );

    error = TT_Set_Instance_CharSize( instance, ptsize << 6 );
    if (error) Error( "could not set text size", filename );
  }


  int  main( int  argc, char**  argv )
  {
    int     i;
    char    filename[128+4];
    char*   execname;


    execname = argv[0];
    if ( argc < 2 )
      Usage( execname );

    if ( strcmp( argv[1], "--font" ) == 0 )
    {
      debug_coderange = TT_CodeRange_Font;
      mode = 0;
      argc--;
      argv++;
    }
    else if ( strcmp( argv[1], "--cvt" ) == 0 )
    {
      debug_coderange = TT_CodeRange_Cvt;
      argv++;
      argc--;
      mode = 1;
    }
    else if ( sscanf( argv[1], "%d", &Num ) == 1 )
    {
      mode = 2;
      argv++;
      argc--;
    }
    else
      Usage( execname );

    /* read the point size for cvt and glyph modes */
    if (mode > 0)
    {
      if ( sscanf( argv[1], "%d", &ptsize ) != 1 )
        Usage( execname );
      argc--;
      argv++;
    }

    if ( argc != 2 )
      Usage(execname);

    i = strlen( argv[1] );
    while ( i > 0 && argv[1][i] != '\\' )
    {
      if ( argv[1][i] == '.' )
        i = 0;
      i--;
    }

    filename[128] = 0;

    strncpy( filename, argv[1], 128 );
    if ( i >= 0 )
      strncpy( filename + strlen(filename), ".ttf", 4 );

    Init_Keyboard();

    if (mode == 2)
    {
      Init_Face( filename );
      Print_Banner();

      error = TT_Load_Glyph( instance, glyph, Num, TTLOAD_DEFAULT );
      if (error) Error( "Error during bytecode execution", filename );
    }
    else
    {
      Print_Banner();
      Init_Face( filename );
    }

    TT_Close_Face( face );
    TT_Done_FreeType(engine);

    Reset_Keyboard();
    
    exit( EXIT_SUCCESS );      /* for safety reasons */

    return 0;       /* never reached */
  }


/* End */
