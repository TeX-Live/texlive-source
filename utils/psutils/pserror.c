/* pserror.c
 * Copyright (C) Angus J. C. Duggan 1991-1995
 * See file LICENSE for details.
 *
 * Warnings and errors for PS programs
 */

extern char *program ;	/* Defined by main program, giving program name */

#include "psutil.h"
#include "pserror.h"
#include "patchlev.h"

#include <string.h>

/* Message functions; there is a single are varargs functions for messages,
   warnings, and errors sent to stderr. If called with the flags MESSAGE_EXIT
   set, the routine does not return */

#define MAX_MESSAGE	256	/* maximum formatted message length */
#define MAX_FORMAT	16	/* maximum format length */
#define MAX_COLUMN	78	/* maximum column to print upto */

void message(int flags, char *format, ...)
{
  va_list args ;
  static column = 0 ;		/* current screen column for message wrap */
  char msgbuf[MAX_MESSAGE] ;	/* buffer in which to put the message */
  char *bufptr = msgbuf ;	/* message buffer pointer */

  if ( (flags & MESSAGE_NL) && column != 0 ) {	/* new line if not already */
    putc('\n', stderr) ;
    column = 0 ;
  }
    
  if ( flags & MESSAGE_PROGRAM ) {
    strcpy(bufptr, program) ;
    bufptr += strlen(program) ;
    *bufptr++ = ':' ;
    *bufptr++ = ' ' ;
  }

  va_start(args, format) ;
  if ( format != NULL ) {
    char c ;
    while ( (c = *format++) != '\0' ) {
      if (c == '%') {
	int done, longform, index ;
	char fmtbuf[MAX_FORMAT] ;
	longform = index = 0 ;
	fmtbuf[index++] = c ;
	do {
	  done = 1 ;
	  fmtbuf[index++] = c = *format++ ;
	  fmtbuf[index] = '\0' ;
	  switch (c) {
	  case '%':
	    *bufptr++ = '%' ;
	  case '\0':
	    break ;
	  case 'e': case 'E': case 'f': case 'g': case 'G':
	    {
	      double d = va_arg(args, double) ;
	      sprintf(bufptr, fmtbuf, d) ;
	      bufptr += strlen(bufptr) ;
	    }
	    break ;
	  case 'c': case 'd': case 'i': case 'o':
	  case 'p': case 'u': case 'x': case 'X':
	    if ( longform ) {
	      long l = va_arg(args, long) ;
	      sprintf(bufptr, fmtbuf, l) ;
	    } else {
	      int i = va_arg(args, int) ;
	      sprintf(bufptr, fmtbuf, i) ;
	    }
	    bufptr += strlen(bufptr) ;
	    break ;
	  case 's':
	    {
	      char *s = va_arg(args, char *) ;
	      sprintf(bufptr, fmtbuf, s) ;
	      bufptr += strlen(bufptr) ;
	    }
	    break ;
	  case 'l':
	    longform = 1 ;
	    /* FALLTHRU */
	  default:
	    done = 0 ;
	  }
	} while ( !done ) ;
      } else if ( c == '\n' ) {	/* write out message so far and reset column */
	int len = bufptr - msgbuf ;	/* length of current message */
	*bufptr++ = '\n' ;
	*bufptr = '\0' ;
	if ( column + len > MAX_COLUMN && column > 0 ) {
	  putc('\n', stderr) ;
	  column = 0 ;
	}
	fputs(bufptr = msgbuf, stderr) ;
	column = 0 ;
      } else
	*bufptr++ = c ;
    }
    *bufptr = '\0' ;
    {
      int len = bufptr - msgbuf ;	/* length of current message */
      if ( column + len > MAX_COLUMN && column > 0 ) {
	putc('\n', stderr) ;
	column = 0 ;
      }
      fputs(msgbuf, stderr) ;
      column += len ;
    }
    fflush(stderr) ;
  }
  va_end(args) ;

  if ( flags & MESSAGE_EXIT )	/* don't return to program */
    exit(1) ;
}
