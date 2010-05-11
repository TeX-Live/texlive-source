/* $Id: rxsub.h,v 1.3 2005/05/02 21:39:53 jschrod Exp $
 *------------------------------------------------------------

  This piece of code was inspired from the regsub implementation of
  Henry Spencer. I modified it to make it compatible with the GNU Rx
  library.

  I added a more flexible interface that also allows dynamic
  allocation of the string buffer and a function that computes the
  length of the resulting string.

*/

#ifndef RXSUB_H
#define RXSUB_H

#include "mkind.h"
#include "regex.h"

/* The error codes used by the rxsub-library */

typedef enum
{
  RXSUB_NOERROR = 0,  /* Success. */
  RXSUB_INVAL,        /* Invalid arguments */
  RXSUB_ESPACE,       /* The given buffer is too small */
  RXSUB_DAMAGE        /* Damaged source string */
} rxsub_errcode_t;

/*
  nrxsub - perform substitutions after a regexp match

  substpat  : a string consisting of ordinary characters and subexpression
              specifiers like `&' and `\0'..`\9'.
  pmatch[]  : the vector of subexpression specifiers into `source'
  source    : the string that was used in the regexec call yielding `pmatch'
  dest      : where to store the result
  nsize     : buffer length

  Returns one of the error codes.
 */


extern rxsub_errcode_t nrxsub
PROTO( (char *substpat,
	regmatch_t pmatch[],
	char *source,
	char *dest,
	size_t nsize) );

/*
  rxsub - perform substitutions after a regexp match

  substpat  : a string consisting of ordinary characters and subexpression
              specifiers like `&' and `\0'..`\9'.
  pmatch[]  : the vector of subexpression specifiers into `source'
  source    : the string that was used in the regexec call yielding `pmatch'
  dest      : pointer to a char*, which is used to store the pointer of the
              result string. The neccessary space for the result is allocated
	      dynamically.

  Returns one of the error codes.
 */

rxsub_errcode_t rxsub
PROTO( (char   *substpat,
	regmatch_t pmatch[],
	char   *source,
	char   **dest) );

/*
  rxsub_len - calculate the neccessary space to hold the result of the
              rxsub-call.

  substpat  : a string consisting of ordinary characters and subexpression
              specifiers like `&' and `\0'..`\9'.
  pmatch[]  : the vector of subexpression specifiers into `source'

  Returns the length of the string neccessary to hold the substituted
  result with the final '\0' character included, or 0 of a fatal error occured.
 */

extern size_t rxsub_len
PROTO( (char *substpat,
	regmatch_t pmatch[]) );

#endif /* RXSUB_H */


/*======================================================================

  $Log: rxsub.h,v $
  Revision 1.3  2005/05/02 21:39:53  jschrod
      xindy run time engine 3.0; as used for CLISP 2.33.2.

  Revision 1.2  1999/07/30 08:37:09  kehr
  Intermediate checkin.

  Revision 1.1  1996/03/26  17:31:04  kehr
  First check-in of the rx-support for clisp.

*/
