/* $Id: rxsub.c,v 1.4 2005/05/02 21:39:53 jschrod Exp $
 *------------------------------------------------------------

  This piece of code was inspired from the regsub implementation of
  Henry Spencer. I modified it to make it compatible with the GNU Rx
  library.

 */

#include "rxsub.h"

#include <string.h>

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

#define chkbuf( x )  if ( x ) return RXSUB_ESPACE;

rxsub_errcode_t
nrxsub  (char   *substpat,
	 regmatch_t pmatch[],
	 char   *source,
	 char   *dest,
	 size_t nsize)
{
  register char *sub;
  register char *dst;
  register char c;
  register int no;
  register size_t len;
  register char* bufend;

  if ( pmatch == NULL || source == NULL ||
       dest   == NULL || substpat == NULL ||
       nsize <= 0 ) {
    return RXSUB_INVAL;
  }

  sub = substpat;
  dst = dest;

  bufend = dst + nsize;    /* compute pointer to bufend */

  while ((c = *sub++) != '\0') {
    if (c == '&')
      no = 0;
    else if (c == '\\' && '0' <= *sub && *sub <= '9')
      no = *sub++ - '0';
    else
      no = -1;

    if (no < 0) {	/* Ordinary character. */
      if (c == '\\' && (*sub == '\\' || *sub == '&'))
	c = *sub++;
      chkbuf( dst >= bufend );
      *dst++ = c;
    } else { /* we found a subexpr specifier */
      if ( pmatch[no].rm_so >= 0 && pmatch[no].rm_eo >= 0 ) {
	len = pmatch[no].rm_eo - pmatch[no].rm_so;
	chkbuf(dst + len >= bufend);
	(void) strncpy( dst, source + pmatch[no].rm_so, len );
	dst += len;
	if (len != 0 && *(dst-1) == '\0') {   /* strncpy hit NULL */
	  return RXSUB_DAMAGE;
	}
      }
	  }
  }
  chkbuf( dst >= bufend );
  *dst++ = '\0';
  return RXSUB_NOERROR;
}


/*
  rxsub - perform substitutions after a regexp match

  substpat  : a string consisting of ordinary characters and subexpression
              specifiers like `&' and `\0'..`\9'.
  pmatch[]  : the vector of subexpression specifiers into `source'
  source    : the string that was used in the regexec call yielding `pmatch'
  dest      : pointer to a char*, which is used to store the pointer of the
              result string

  Returns one of the error codes.
 */

rxsub_errcode_t
rxsub   (char   *substpat,
	 regmatch_t pmatch[],
	 char   *source,
	 char   **dest)
{
  register size_t len;

  if ( pmatch == NULL || source == NULL ||
       dest   == NULL || substpat == NULL ) {
    return RXSUB_INVAL;
  }

  *dest = (char*)malloc( len = rxsub_len( substpat, pmatch ));

  return nrxsub( substpat, pmatch, source, *dest, len );
}



/*
  rxsub_len - calculate the neccessary space to hold the result of the
              rxsub-call.

  substpat  : a string consisting of ordinary characters and subexpression
              specifiers like `&' and `\0'..`\9'.
  pmatch[]  : the vector of subexpression specifiers into `source'

  Returns the length of the string neccessary to hold the substituted
  result with the final '\0' character included, or 0 of a fatal error occured.
  */

extern size_t
rxsub_len (char *substpat,
	   regmatch_t pmatch[])
{
  register char *sub;
  register char c;
  register int no;
  register size_t len;
  register size_t totlen = 0;

  if ( substpat == NULL || pmatch == NULL )
    return 0;

  sub = substpat;

  while ((c = *sub++) != '\0') {
    if (c == '&')
      no = 0;
    else if (c == '\\' && '0' <= *sub && *sub <= '9')
      no = *sub++ - '0';
    else
      no = -1;

    if (no < 0) {	/* Ordinary character. */
      if (c == '\\' && (*sub == '\\' || *sub == '&'))
	c = *sub++;
      totlen++;
    } else { /* we found a subexpr specifier */
      if ( pmatch[no].rm_so >= 0 && pmatch[no].rm_eo >= 0 ) {
	len = pmatch[no].rm_eo - pmatch[no].rm_so;
	totlen += len;
      }
    }
  }
  return ++totlen;
}

/*======================================================================

  $Log: rxsub.c,v $
  Revision 1.4  2005/05/02 21:39:53  jschrod
      xindy run time engine 3.0; as used for CLISP 2.33.2.

*/
