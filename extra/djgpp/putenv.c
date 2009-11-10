/* Copyright (C) 1996 DJ Delorie, see COPYING.DJ for details */
/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
#include <libc/stubs.h>
#include <stdlib.h>
#include <string.h>
#include <libc/bss.h>
#include <libc/environ.h>

/*

   This routine assumes that the environ array and all strings it
   points to were malloc'd.  Nobody else should modify the environment
   except crt1.c

*/

extern char **environ;
static char **prev_environ = NULL; /* to know when it's safe to call `free' */
static int ecount = -1;
static int emax = -1;
static int putenv_bss_count = -1;


/* This gets incremented every time some variable in the
   environment is added, deleted, or changes its value.
   It is meant to be used by functions that depend on values
   of environment variables, but don't want to call `getenv'
   unnecessarily (example: `__use_lfn').

   Users should compare this variable with their static
   variable whose value is initialized to zero; thus this
   variable begins with 1 so first time users look they will
   call `getenv'.  */
unsigned __environ_changed = 1;

int
putenv(const char *val)
{
  int vlen = strlen(val);
  char *epos = strchr(val, '=');
  /* Feature: VAL without a `=' means delete the entry.  GNU `putenv'
     works that way, and `env' from GNU Sh-utils counts on this behavior.  */
  int nlen = epos ? epos - val + 1 : vlen;
  int eindex;
  
  /* Force recomputation of the static counters.

     The second condition of the next if clause covers the case that
     somebody pointed environ to another array, which invalidates
     `ecount' and `emax'.  This can be used to change the environment
     to something entirely different, or to effectively discard it
     altogether.  GNU `env' from Sh-utils does just that.  */
  if (putenv_bss_count != __bss_count
      || environ       != prev_environ)
  {
    for (ecount=0; environ[ecount]; ecount++);
    emax = ecount;
    /* Bump the count to a value no function has yet seen,
       even if they were dumped with us.  */
    __environ_changed++;
    if (putenv_bss_count != __bss_count)
    {
      putenv_bss_count = __bss_count;
      prev_environ = environ;	/* it's malloced by crt1.c */
    }
  }

  for (eindex=0; environ[eindex]; eindex++)
    if (strncmp(environ[eindex], val, nlen) == 0
	&& (epos || environ[eindex][nlen] == '='))
    {
      char *oval = environ[eindex];
      int olen = strlen(oval);

      if (val[nlen] == 0 && !epos) /* delete the entry */
      {
	free(oval);
	environ[eindex] = environ[ecount-1];
	environ[ecount-1] = 0;
	ecount--;
	__environ_changed++;
	return 0;
      }

      /* change existing entry */
      if (strcmp(environ[eindex]+nlen, val+nlen) == 0)
	return 0; /* they're the same */

      /* If new is the same length as old, reuse the same
	 storage.  If new is shorter, call realloc to shrink the
	 allocated block: this causes less memory fragmentation.  */
      if (vlen != olen)
      {
	if (vlen > olen)
	  environ[eindex] = (char *)malloc(vlen+1);
	else	/* vlen < olen */
	  environ[eindex] = (char *)realloc(oval, vlen+1);
	if (environ[eindex] == 0)
	{
	  environ[eindex] = oval;
	  return -1;
	}
	if (vlen > olen)
	  free(oval);
      }
      strcpy(environ[eindex], val);
      __environ_changed++;
      return 0;
    }

  /* delete nonexisting entry? */
  if (val[nlen] == 0 && !epos)
    return 0;

  /* create new entry */
  if (ecount >= emax)
  {
    char **enew;
    emax += 10;
    enew = (char **)malloc((emax+1) * sizeof(char *));
    if (enew == 0)
      return -1;
    memcpy(enew, environ, ecount * sizeof(char *));
    /* If somebody set environ to another array, we can't
       safely free it.  Better leak memory than crash.  */
    if (environ == prev_environ)
      free(environ);
    environ = enew;
    prev_environ = environ;
  }

  environ[ecount] = (char *)malloc(vlen+1);
  if (environ[ecount] == 0)
    return -1;
  strcpy(environ[ecount], val);

  ecount++;
  environ[ecount] = 0;

  __environ_changed++;

  return 0;
}
