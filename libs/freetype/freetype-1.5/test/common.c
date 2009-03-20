/****************************************************************************/
/*                                                                          */
/*  The FreeType project -- a free and portable quality TrueType renderer.  */
/*                                                                          */
/*  Copyright 1996-2001 by                                                  */
/*  D. Turner, R.Wilhelm, and W. Lemberg                                    */
/*                                                                          */
/*  common.c: Various utility functions.                                    */
/*                                                                          */
/****************************************************************************/


/*
 *  This is a cheap replacement for getopt() because that routine is not
 *  available on some platforms and behaves differently on other platforms.
 *  This code was written from scratch without looking at any other
 *  implementation.
 *
 *  This code is hereby expressly placed in the public domain.
 *  mleisher@crl.nmsu.edu (Mark Leisher)
 *  10 October 1997
 */

#ifndef lint
#ifdef __GNUC__
  static char  rcsid[] __attribute__ ((unused)) = "$Id: common.c,v 1.16 2001/12/25 20:13:04 antoineL Exp $";
#else
  static char  rcsid[] = "$Id: common.c,v 1.16 2001/12/25 20:13:04 antoineL Exp $";
#endif
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

  /*
   *  Externals visible to programs.
   */

  int    ft_opterr = 1;
  int    ft_optind = 1;
  char*  ft_optarg;

  /*
   *  Internal variables that are used to detect when the global values
   *  need to be reset.
   */

  static int  cmdac;
#ifdef __STDC__
  static const char*   cmdname;
  static char* const*  cmdav;
#else
  static char*   cmdname;
  static char**  cmdav;
#endif


  int
#ifdef __STDC__
  ft_getopt( int  ac, char* const*  av, const char*  pat )
#else
  ft_getopt( ac, av, pat )
    int     ac;
    char**  av;
    char*   pat;
#endif
  {
    int  opt;
#ifdef __STDC__
    const char*  p;
    const char*  pp;
#else
    char*  p;
    char*  pp;
#endif


    /*
     *  If there is no pattern, indicate the parsing is done.
     */
    if ( pat == 0 || *pat == 0 )
      return -1;

    /*
     *  Always reset the option argument to NULL.
     */
    ft_optarg = 0;

    /*
     *  If the number of arguments or argument list do not match the last
     *  values seen, reset the internal pointers and the globals.
     */
    if ( ac != cmdac || av != cmdav )
    {
      ft_optind = 1;
      cmdac = ac;
      cmdav = av;

      /*
       *  Determine the command name in case it is needed for warning
       *  messages.
       */
      for ( cmdname = 0, p = av[0]; *p; p++ )
      {
        if ( *p == '/' || *p == '\\' )
          cmdname = p;
      }

      /*
       *  Skip the path separator if the name was assigned.
       */
      if ( cmdname )
        cmdname++;
      else
        cmdname = av[0];
    }

    /*
     *  If the next index is greater than or equal to the number of
     *  arguments, then the command line is done.
     */
    if ( ft_optind >= ac )
      return -1;

    /*
     *  Test the next argument for one of three cases:
     *    1. The next argument does not have an initial '-'.
     *    2. The next argument is '-'.
     *    3. The next argument is '--'.
     *
     *  In either of these cases, command line processing is done.
     */
    if ( av[ft_optind][0] != '-'            ||
         strcmp( av[ft_optind], "-" ) == 0  ||
         strcmp( av[ft_optind], "--" ) == 0 )
      return -1;

    /*
     *  Point at the next command line argument and increment the
     *  command line index.
     */
    p = av[ft_optind++];

    /*
     *  Look for the first character of the command line option.
     */
    for ( opt = *(p + 1), pp = pat; *pp && *pp != opt; pp++ )
      ;

    /*
     *  If nothing in the pattern was recognized, then issue a warning
     *  and return a '?'.
     */
    if ( *pp == 0 )
    {
      if ( ft_opterr )
        fprintf( stderr, "%s: illegal option -- %c\n", cmdname, opt );
      return '?';
    }

    /*
     *  If the option expects an argument, get it.
     */
    if ( *(pp + 1) == ':' && (ft_optarg = av[ft_optind]) == 0 )
    {
      /*
       *  If the option argument is NULL, issue a warning and return a '?'.
       */
      if ( ft_opterr )
        fprintf( stderr, "%s: option requires an argument -- %c\n",
                         cmdname, opt );
      opt = '?';
    }
    else if ( ft_optarg )
    /*
     *  Increment the option index past the argument.
     */
      ft_optind++;

    /*
     *  Return the option character.
     */
    return opt;
  }


/****************************************************************************/
/*                                                                          */
/* ft_basename():                                                           */
/*                                                                          */
/* a stupid but useful function...                                          */
/*                                                                          */
/* rewritten by DavidT to get rid of GPLed programs in the FreeType engine. */
/*                                                                          */
/****************************************************************************/

  char*
#ifdef __STDC__
  ft_basename( const char*  name )
#else
  ft_basename( name )
    char*  name;
#endif
  {
#ifdef __STDC__
    const char*  base;
    const char*  current;
#else
    char*        base;
    char*        current;
#endif
    char         c;


    base    = name;
    current = name;

    c = *current;

    while ( c )
    {
      if ( c == '/' || c == '\\' )
        base = current + 1;

      current++;
      c = *current;
    }

    return (char*)base;
  }

/****************************************************************************/

  void
#ifdef __STDC__
  Panic( const char*  fmt, ... )
#else
  Panic( fmt )
    const char*  fmt;
#endif
  {
    va_list  ap;


    va_start( ap, fmt );
    vprintf( fmt, ap );
    va_end( ap );

    exit( EXIT_FAILURE );
  }

/****************************************************************************/

  void
#ifdef __STDC__
  separator_line( FILE*  out, const int  length )
#else
  separator_line( out, length )
    FILE*  out;
    int    length;
#endif
  {
    int  i;


    for ( i = 0; i < length; i++ )
      fputc( '-', out );
    fprintf( out, "\n\n" );
  }


/* End */
