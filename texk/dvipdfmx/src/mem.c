/*  $Header: /home/cvsroot/dvipdfmx/src/mem.c,v 1.4 2007/11/14 03:36:01 chofchof Exp $

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

#include "dpxutil.h"
#include "mem.h"

static long int mem_count = 0;
static int mem_internal = 0;
static struct ht_table *mem_ht = NULL;

#ifdef MEM_DEBUG

#define MEM_LINE   128

long int mem_event = 0;

static void
mem_noop (void *vp)
{
}

static void
mem_error (void *vp)
{
  fprintf(stderr, "pointer %p released, but not allocated\n", vp);
}

#endif /* MEM_DEBUG */

void mem_debug_check(void)
{
#ifdef MEM_DEBUG
  struct ht_iter iter;

  fprintf(stderr, "*** Memory still in use ***\n");

  if (ht_set_iter(mem_ht, &iter) == 0) {
    do {
      int pl;
      void *p = ht_iter_getkey(&iter, &pl);
      char *s = ht_iter_getval(&iter);

      fprintf(stderr, "%p %s\n", p, s);
    } while (!ht_iter_next(&iter));
  }
  ht_clear_iter(&iter);

  fprintf(stderr, "*** End of used memory ***\n");
#endif /* MEM_DEBUG */

  if (mem_count)
    fprintf(stderr, "*** WARNING: %ld memory objects still allocated\n", mem_count);
}

void mem_debug_init(void)
{
#ifdef MEM_DEBUG
  mem_ht = malloc(sizeof(struct ht_table));
  ht_init_table(mem_ht);
  fprintf(stderr, "*** Memory debugging started ***\n");
#else
  mem_ht = (struct ht_table *) &mem_count; /* just any non-null value */
#endif /* MEM_DEBUG */
}

void *new (size_t size, const char *file, const char *function, int line)
{
  void *result;
  if ((result = malloc (size)) == NULL) {
    fprintf (stderr, "Out of memory in %s:%d\n", function, line);
    fprintf (stderr, "Asked for %lu\n", (unsigned long) size);
    exit (1);
  }

  if (!mem_internal) {
    mem_count++;
#ifdef MEM_DEBUG
    mem_internal = 1;
    char *s = malloc(MEM_LINE), **p = malloc(sizeof(result));
    *p = result;
    snprintf(s, MEM_LINE, "(0x%08lx) %s (%s, %d)"
#ifdef __GNUC__
            ", called from %p"
#endif
            , ++mem_event, function, file, line
#ifdef __GNUC__
            , __builtin_return_address(1)
#endif
            );
    ht_insert_table(mem_ht, p, sizeof(result), s, mem_error);
    mem_internal = 0;
#endif /* MEM_DEBUG */
  }

  return result;
}

void *renew (void *mem, size_t size, const char *file, const char *function, int line)
{
  void *result;

  if (!mem_internal) {
    if (!mem)
      mem_count++;
#ifdef MEM_DEBUG
    else {
      mem_internal = 1;
      if (!(mem_ht && ht_remove_table(mem_ht, &mem, sizeof(mem), mem_noop))) {
	fprintf(stderr, "*** ERROR: Trying to re-allocate non-allocated memory\n");
	fprintf(stderr, "%p %s (%s, %d)"
#ifdef __GNUC__
		", called from %p"
#endif
		"\n", mem, function, file, line
#ifdef __GNUC__
		, __builtin_return_address(1)
#endif
		);
	exit(1);
      }
      mem_internal = 0;
    }
#endif /* MEM_DEBUG */
  }

  if ((result = realloc (mem, size)) == NULL) {
    fprintf (stderr, "Out of memory!\n");
    exit (1);
  }

#ifdef MEM_DEBUG
  if (!mem_internal) {
    mem_internal = 1;
    char *s = malloc(MEM_LINE), **p = malloc(sizeof(result));
    *p = result;
    snprintf(s, MEM_LINE, "(0x%08lx) %s (%s, %d)"
#ifdef __GNUC__
            ", called from %p"
#endif
            , ++mem_event, function, file, line
#ifdef __GNUC__
            , __builtin_return_address(1)
#endif
            );
    ht_insert_table(mem_ht, p, sizeof(result), s, mem_error);
    mem_internal = 0;
  }
#endif /* MEM_DEBUG */

  return result;
}

void release (void *mem, const char *file, const char *function, int line)
{

  if (mem && !mem_internal) {
      mem_count--;
#ifdef MEM_DEBUG
      mem_internal = 1;
      if (!(mem_ht && ht_remove_table(mem_ht, &mem, sizeof(mem), mem_noop))) {
	fprintf(stderr, "*** ERROR: Trying to free non-allocated memory\n");
	fprintf(stderr, "%p %s (%s, %d)"
#ifdef __GNUC__
		", called from %p"
#endif
		"\n", mem, function, file, line
#ifdef __GNUC__
		, __builtin_return_address(1)
#endif
		);
	exit(1);
      }
      mem_internal = 0;
#endif /* MEM_DEBUG */
  }

  free (mem);
}
