#include <stdio.h>
#include <ctype.h>

/*
 * This is for people who don't have strtol defined on their systems.
 * -- Compliments of Daniel.Stodolsky@cs.cmu.edu
 */

long strtol(nptr,base,eptr)
char *nptr;
int base;
char **eptr;
{
  long l;
  
  if (base==10)
    {
      if (sscanf(nptr,"%ld",&l)!=1)
	{
	  *eptr = nptr;
	  return 0;
	}
      else
	{
	  while(!isdigit(*nptr))
	    nptr++;
	  while (isdigit(*nptr))
	    nptr++;
	  *eptr = nptr;
	  return l;
	}
    }
  else
    if (base==16)
      {
	if (sscanf(nptr,"%lx",&l)!=1)
	{
	  *eptr = nptr;
	  return 0;
	}
      else
	{
	  while(!isxdigit(*nptr))
	    nptr++;
	  while (isxdigit(*nptr))
	    nptr++;
	  *eptr = nptr;
	  return l;
	}
      }
    else
      { /* a base we don't understand. Puke */
	*eptr = nptr;
	return 0;
      }
}
