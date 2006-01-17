/*  $Header$

    This is dvipdfm, a DVI to PDF translator.
    Copyright (C) 1998, 1999 by Mark A. Wicks

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    
    The author may be contacted via the e-mail address

	mwicks@kettering.edu
*/

	
#ifndef MEM_H
#define MEM_H

#include <stdlib.h>

extern void *new (size_t size, char *function, int line);
extern void *renew (void *p, size_t size, char *function, int line);
extern void release (void *mem, char *function, int line);

#ifdef MEM_DEBUG
extern void mem_debug_init(void);
extern FILE *debugfile;
#define NEW(n,type) (type *)(new (((size_t) (n))*sizeof(type),__FUNCTION__,__LINE__))
#define RENEW(p,n,type) (type *)(renew ((p),(n)*sizeof(type),__FUNCTION__,__LINE__))
#define RELEASE(p) release ((p),__FUNCTION__,__LINE__)
#define MEM_START {mem_debug_init();fprintf (debugfile, "Entered %s\n", __FUNCTION__);}
#define MEM_END fprintf (debugfile, "Leaving %s\n", __FUNCTION__);
#else /* MEM_DEBUG */
#define MEM_START
#define MEM_END
#define NEW(n,type) (type *)(new (((size_t) (n))*sizeof(type),NULL,0))
#define RENEW(p,n,type) (type *)(renew ((p),(n)*sizeof(type),NULL,0))
#define RELEASE(p) release ((p),NULL,0)
#endif /* MEM_DEBUG */
#endif /* MEM_H */
