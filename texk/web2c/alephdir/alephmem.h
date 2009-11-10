/* alephmem.h: C declarations for types and accessing routines in aleph.c

This file is part of the Aleph project, which
is based on the web2c distribution of TeX.

Copyright (C) 1994--2000 John Plaice and Yannis Haralambous
Copyright (C) 2004 the Aleph team

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


extern boolean new_input_line (FILE *, halfword);
extern boolean newinputln (FILE *, halfword, halfword, boolean);
extern int getc_two (FILE *);
extern int getfilemode (FILE *, int);
extern void ungetc_two (int, FILE *);
extern int ocptemp;

#define newinputline(stream, mode, flag)   new_input_line (stream, mode)

extern memoryword **fonttables;
extern memoryword **fontsorttables;
extern int **ocptables;

typedef struct hashw
{
  integer p;
  struct hashw *ptr;
  memoryword mw;
} hashword;

extern hashword hashtable[];
extern hashword *createhashpos(int);
extern hashword *createeqtbpos(int);
extern hashword *createxeqlevel(int);
extern void inithhashtable(void);
extern void dumphhashtable(void);
extern void undumphhashtable(void);
extern void allocatefonttable(int, int);
extern void dumpfonttable(int, int);
extern void undumpfonttable(int);
extern void allocatefontsorttable(int, int);
extern void dumpfontsorttable(int, int);
extern void undumpfontsorttable(int);
extern void allocateocptable(int, int);
extern void dumpocptable(int);
extern void undumpocptable(int);
extern void odateandtime(int, int, int, int);
extern void btestin(void);
extern void runexternalocp(string);

#define initeqtbtable()		inithhashtable();
#define dumpeqtbtable()		dumphhashtable();
#define undumpeqtbtable()	undumphhashtable();

#define HASHTABLESIZE		23123
#define neweqtb(a)		(((a)==hashtable[(a)%HASHTABLESIZE].p) ? \
				 (hashtable[(a)%HASHTABLESIZE].mw) : \
                                 (createeqtbpos(a)->mw))
#define neweqtbint(a)		(((a)==hashtable[(a)%HASHTABLESIZE].p) ? \
				 (hashtable[(a)%HASHTABLESIZE].mw.cint) : \
                                 (createeqtbpos(a)->mw.cint))
#define neweqtbsc(a)		(((a)==hashtable[(a)%HASHTABLESIZE].p) ? \
				 (hashtable[(a)%HASHTABLESIZE].mw.cint) : \
                                 (createeqtbpos(a)->mw.cint))
#define newequiv(a)		(((a)==hashtable[(a)%HASHTABLESIZE].p) ? \
				 (hashtable[(a)%HASHTABLESIZE].mw.hh.rh) : \
                                 (createeqtbpos(a)->mw.hh.rh))
#define newequiv1(a)		(((a)==hashtable[(a)%HASHTABLESIZE].p) ? \
			(hashtable[(a)%HASHTABLESIZE].mw.cint1) : \
                                 (createeqtbpos(a)->mw.cint1))
#define neweqlevel(a)		(((a)==hashtable[(a)%HASHTABLESIZE].p) ? \
				 (hashtable[(a)%HASHTABLESIZE].mw.hh.b1) : \
                                 (createeqtbpos(a)->mw.hh.b1))
#define neweqtype(a)		(((a)==hashtable[(a)%HASHTABLESIZE].p) ? \
				 (hashtable[(a)%HASHTABLESIZE].mw.hh.b0) : \
                                 (createeqtbpos(a)->mw.hh.b0))
#define setneweqtb(a,v)		(((a)==hashtable[(a)%HASHTABLESIZE].p) ? \
				 (hashtable[(a)%HASHTABLESIZE].mw=v) : \
                                 (createeqtbpos(a)->mw=v))
#define setneweqtbint(a,v)	(((a)==hashtable[(a)%HASHTABLESIZE].p) ? \
				 (hashtable[(a)%HASHTABLESIZE].mw.cint=v) : \
                                 (createeqtbpos(a)->mw.cint=v))
#define setneweqtbsc(a,v)	(((a)==hashtable[(a)%HASHTABLESIZE].p) ? \
				 (hashtable[(a)%HASHTABLESIZE].mw.cint=v) : \
                                 (createeqtbpos(a)->mw.cint=v))
#define setequiv(a,v)		(((a)==hashtable[(a)%HASHTABLESIZE].p) ? \
				 (hashtable[(a)%HASHTABLESIZE].mw.hh.rh=v) : \
                                 (createeqtbpos(a)->mw.hh.rh=v))
#define setequiv1(a,v)		(((a)==hashtable[(a)%HASHTABLESIZE].p) ? \
			 (hashtable[(a)%HASHTABLESIZE].mw.cint1=v) : \
                                 (createeqtbpos(a)->mw.cint1=v))
#define seteqlevel(a,v)		(((a)==hashtable[(a)%HASHTABLESIZE].p) ? \
				 (hashtable[(a)%HASHTABLESIZE].mw.hh.b1=v) : \
                                 (createeqtbpos(a)->mw.hh.b1=v))
#define seteqtype(a,v)		(((a)==hashtable[(a)%HASHTABLESIZE].p) ? \
				 (hashtable[(a)%HASHTABLESIZE].mw.hh.b0=v) : \
                                 (createeqtbpos(a)->mw.hh.b0=v))

#define newhashnext(a)		(((a)==hashtable[(a)%HASHTABLESIZE].p) ? \
				 (hashtable[(a)%HASHTABLESIZE].mw.hh.v.LH) : \
                                 (createhashpos(a)->mw.hh.v.LH))
#define newhashtext(a)		(((a)==hashtable[(a)%HASHTABLESIZE].p) ? \
				 (hashtable[(a)%HASHTABLESIZE].mw.hh.v.RH) : \
                                 (createhashpos(a)->mw.hh.v.RH))
#define sethashnext(a,d)	(((a)==hashtable[(a)%HASHTABLESIZE].p) ? \
				 (hashtable[(a)%HASHTABLESIZE].mw.hh.v.LH=d) : \
                                 (createhashpos(a)->mw.hh.v.LH=d))
#define sethashtext(a,d)	(((a)==hashtable[(a)%HASHTABLESIZE].p) ? \
				 (hashtable[(a)%HASHTABLESIZE].mw.hh.v.RH=d) : \
                                 (createhashpos(a)->mw.hh.v.RH=d))

#define newxeqlevel(a)		(((a)==hashtable[(a)%HASHTABLESIZE].p) ? \
				 (hashtable[(a)%HASHTABLESIZE].mw.cint) : \
                                 (createxeqlevel(a)->mw.cint))

#define setintzero(w,a)		((w).cint=(a))
#define setintone(w,a)		((w).cint1=(a))
