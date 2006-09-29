/*  $Header: /home/cvsroot/dvipdfmx/src/mem.c,v 1.3 2002/10/30 02:27:11 chofchof Exp $

    This is dvipdfmx, an eXtended version of dvipdfm by Mark A. Wicks.

    Copyright (C) 2002 by Jin-Hwan Cho and Shunsaku Hirata,
    the dvipdfmx project team <dvipdfmx@project.ktug.or.kr>
    
    Copyright (C) 1998, 1999 by Mark A. Wicks <mwicks@kettering.edu>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

#include <stdio.h>	
#include <stdlib.h>

#include "mem.h"

#ifdef MEM_DEBUG
FILE *debugfile = NULL;
static long int event = 0;
#endif /* MEM_DEBUG */

#ifdef MEM_DEBUG
void mem_debug_init(void)
{
  if (debugfile == NULL) {
    debugfile = fopen ("malloc.log", "w");
    fprintf (stderr, "*** Memory Debugging Log started ***\n");
  }
}
#endif /* MEM_DEBUG */

void *new (size_t size, char *function, int line)
{
  void *result;
  if ((result = malloc (size)) == NULL) {
    fprintf (stderr, "Out of memory in %s:%d\n", function, line);
    fprintf (stderr, "Asked for %lu\n", (unsigned long) size);
    exit (1);
  }

#ifdef MEM_DEBUG  
  mem_debug_init();
  event += 1;
  fprintf (debugfile, "%p %07ld [new] %s:%d\n", result, event, function, line);
#endif /* MEM_DEBUG */

  return result;
}

void *renew (void *mem, size_t size, char *function, int line)
{
  void *result;
#ifdef MEM_DEBUG
    mem_debug_init();
    event += 1;
  if (mem)
    fprintf (debugfile, "%p %07ld [fre] %s:%d\n", mem,
	     event, function, line);
#endif /* MEM_DEBUG */
  if ((result = realloc (mem, size)) == NULL) {
    fprintf (stderr, "Out of memory!\n");
    exit (1);
  }
#ifdef MEM_DEBUG
  if (result)
    fprintf (debugfile, "%p %07ld [new] %s:%d\n", result, event, function, line);
#endif /* MEM_DEBUG */
  return result;
}

void release (void *mem, char *function, int line)
{

#ifdef MEM_DEBUG
    mem_debug_init();
    event += 1;
    fprintf (debugfile, "%p %07ld [fre] %s:%d\n", mem, event, function, line);
#endif /* MEM_DEBUG */

  free (mem);
}
