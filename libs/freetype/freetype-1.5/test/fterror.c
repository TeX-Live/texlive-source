/****************************************************************************/
/*                                                                          */
/*  The FreeType project -- a free and portable quality TrueType renderer.  */
/*                                                                          */
/*  Copyright 1996-2002 by E. Dieterich, A.Leca,                            */
/*  D. Turner, R.Wilhelm, and W. Lemberg                                    */
/*                                                                          */
/*  fterror: test TT_ErrToString18 functionality.                           */
/*                                                                          */
/*  This program dumps locale-sensitive error messages.                     */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "freetype.h"
#include "ftxerr18.h"

/*
 *  Basically, an external program using FreeType shouldn't depend on an
 *  internal file of the FreeType library, especially not on ft_conf.h -- but
 *  to avoid another configure script which tests for the existence of the
 *  i18n stuff we include ft_conf.h here since we can be sure that our test
 *  programs use the same configuration options as the library itself.
 */

#include "ft_conf.h"


#ifdef HAVE_LIBINTL_H

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include <libintl.h>

#else /* HAVE_LIBINTL_H */

#define gettext( x )  ( x )

#endif /* HAVE_LIBINTL_H */


  static void
  Usage( void )
  {
    fprintf(stderr, gettext( "fterror: test TT_ErrToString18 functionality\n\n" ) );
    fprintf(stderr, gettext( "  with an optional numerical argument, dumps the associated message.\n\n" ) );
    exit( EXIT_FAILURE );
  }


  int
  main( int  argc, char**  argv )
  {
#ifdef HAVE_LIBINTL_H
    char*  domain;


    setlocale( LC_ALL, "" );
    bindtextdomain( "freetype", LOCALEDIR );
    domain = textdomain( "freetype" );
#endif

    if( argc <= 1 )
    {
      int  i;
#if 0
      printf( "domain: %s\n", domain = textdomain( "" ) );
#endif
      printf( gettext( "Start of fterror.\n" ) );

      for ( i = 0; i < 10; i++ )
        printf( "Code: %i, %s\n", i, TT_ErrToString18( i ) );

#if 0
      printf( "domain: %s\n", domain = textdomain( "" ) );
#endif
      printf( gettext( "End of fterror.\n" ) );
    }
    else
    {
      int            arg;
      unsigned long  cod;
      char          *end;


      for ( arg = 1; arg < argc; ++arg )
      {
        cod = strtoul(argv[arg], &end, 0);

        if( *end != '\0' || cod < 0 )
          Usage();
        printf( gettext( "Freetype error code 0x%04lX (%lu): %s\n" ),
                         cod, cod, TT_ErrToString18( cod ) );
      }
    }

    exit( EXIT_SUCCESS );      /* for safety reasons */

    return 0;       /* never reached */
  }


/* End */
