/* aleph.c: C routines to support the Aleph Pascal Web distribution
 * based on omega.c from the Omega project

This file is part of the Aleph project

Copyright (C) 1994--2001 John Plaice and Yannis Haralambous
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

#define EXTERN extern
#include "alephd.h"

void
btestin(void)
{
    string fname =
    kpse_find_file (nameoffile + 1, kpse_program_binary_format, true);

    if (fname) {
      libcfree(nameoffile);
      nameoffile = xmalloc(2+strlen(fname));
      namelength = strlen(fname);
      strcpy(nameoffile+1, fname);
    }
    else {
      libcfree(nameoffile);
      nameoffile = xmalloc(2);
      namelength = 0;
      nameoffile[0] = 0;
      nameoffile[1] = 0;
    }
}

int
getfilemode (FILE *f, int def)
{
    int c,m;
    if ((def==0)||(feof(f))) m=0;
    else {
       c = getc(f);
       if (c==EOF) m=0;
       else if (c==0x5c) {
          if (feof(f)) m=0;
          else {
             c = getc(f);
             if (c==EOF) m=0;
             else if (c==0) m=4;
             else m=1;
             ungetc(c,f);
             c=0x5c;
          }
       }
       else if (c==0x25) {
          if (feof(f)) m=0;
          else {
             c = getc(f);
             if (c==EOF) m=0;
             else if (c==0) m=4;
             else m=1;
             ungetc(c,f);
             c=0x25;
          }
       }
       else if (c==0xe0) m=2;
       else if (c==0x6c) m=2;
       else if (c==0) {
          if (feof(f)) m=0;
          else {
             c = getc(f);
             if (c==EOF) m=0;
             else if (c==0x5c) m=3;
             else if (c==0x25) m=3;
             else m=def;
             ungetc(c,f);
             c=0;
          }
       }
       else m=def;
       ungetc(c,f);
    }
    return m;
}

static int
getc_two_LE (FILE *f)
{
    register int i,j;
    i = getc(f);
    if (i==EOF) { return i; }
    j = getc(f);
    if (j==EOF) { return j; }
    return ((j<<8)|i);
}

static void
ungetc_two_LE (int c, FILE *f)
{
    ungetc((c>>8), f);
    ungetc((c&0377), f);
}

int
getc_two (FILE *f)
{      
    register int i,j;
    i = getc(f);
    if (i==EOF) { return i; }
    j = getc(f);
    if (j==EOF) { return j; }
    return ((i<<8)|j);
}
 
void
ungetc_two (int c, FILE *f)
{
    ungetc((c&0377), f);
    ungetc((c>>8), f);
}
 
boolean
newinputln (FILE *f, halfword themode, halfword translation, boolean bypass)
{
    return pnewinputln(f, themode, translation, bypass);
}
 
boolean
new_input_line (FILE *f, halfword themode)
{
  register int i=EOF;
 
  last = first;
  otpinputend = 0;
 
  if (themode==1) {
     while ((otpinputend < ocpbufsize) && ((i = getc (f)) != EOF) &&
            ((i != '\r') && (i != '\n')))
        otpinputbuf[++otpinputend] = i;
     if (i=='\r') {
         i=getc(f); if (i != '\n') ungetc(i,f);
     }
  } else if (themode==2) {
     while ((otpinputend < ocpbufsize) && ((i = getc (f)) != EOF) &&
            (i != 0x25))
        otpinputbuf[++otpinputend] = i;
  } else if (themode==3) {
     while ((otpinputend < ocpbufsize) && ((i = getc_two (f)) != EOF) &&
            ((i != '\r') && (i != '\n')))
        otpinputbuf[++otpinputend] = i;
     if (i=='\r') {
         i=getc_two(f); if (i != '\n') ungetc_two(i,f);
     }
  } else /* themode==4 */ {
     while ((otpinputend < ocpbufsize) && ((i = getc_two_LE (f)) != EOF) &&
            ((i != '\r') && (i != '\n')))
        otpinputbuf[++otpinputend] = i;
     if (i=='\r') {
         i=getc_two_LE(f); if (i != '\n') ungetc_two_LE(i,f);
     }
  }
 
  if (i == EOF && otpinputend == 0)
      return false;
 
  /* We didn't get the whole line because our buffer was too small.
*/
 if (i != EOF && (((themode!=2) && (i != '\n')) || ((themode==2) && (i != 0x25))))
    {
      (void) fprintf (stderr,
                     "! Unable to read an entire line---ocp_buf_size=%ld.\n",
                     (long) ocpbufsize);
      (void) fprintf (stderr, "Please increase ocp_buf_size in texmf.cnf.\n");
      uexit (1);
    }
 
    return true;
}

hashword hashtable[HASHTABLESIZE];

void
inithhashtable (void)
{
   int i;
   for (i=0; i<HASHTABLESIZE; i++) {
      hashtable[i].p = -1;
   }
      
}

hashword *
createeqtbpos (int p)
{
   hashword *runner= &(hashtable[p%HASHTABLESIZE]);
   if (runner->p==p) return runner;
   while (runner->p != -1) {
      runner = runner->ptr; 
      if (runner->p == p) return runner;
   } 
   runner->p = p;
   runner->mw = initeqtbentry(p);
   runner->ptr = (hashword *) xmalloc(sizeof(hashword));
   (runner->ptr)->p = -1;
   return runner;
}

#if 0 /* unused */
memoryword *
createeqtbptr (int p)
{
   hashword *runner= &(hashtable[p%HASHTABLESIZE]);
   if (runner->p==p) return (&(runner->mw));
   while (runner->p != -1) {
      runner = runner->ptr; 
      if (runner->p == p) return (&(runner->mw));
   } 
   runner->p = p;
   runner->mw = ziniteqtbentry(p);
   runner->ptr = (hashword *) xmalloc(sizeof(hashword));
   (runner->ptr)->p = -1;
   return (&(runner->mw));
}
#endif

hashword *
createxeqlevel (int p)
{
   hashword *runner= &(hashtable[p%HASHTABLESIZE]);
   if (runner->p==p) return runner;
   while (runner->p != -1) {
      runner = runner->ptr;
      if (runner->p == p) return runner;
   }
   runner->p = p;
   runner->mw.cint = 1;
   runner->mw.cint1 = 0;
   runner->ptr = (hashword *) xmalloc(sizeof(hashword));
   (runner->ptr)->p = -1;
   return runner;
}

hashword *
createhashpos (int p)
{
   hashword *runner= &(hashtable[p%HASHTABLESIZE]);
   if (runner->p==p) return runner;
   while (runner->p != -1) {
      runner = runner->ptr; 
      if (runner->p == p) return runner;
   } 
   runner->p = p;
   runner->mw.cint = 0;
   runner->mw.cint1 = 0;
   runner->ptr = (hashword *) xmalloc(sizeof(hashword));
   (runner->ptr)->p = -1;
   return runner;
}

void
dumphhashtable (void)
{
   int i;
   hashword *runner;
   for (i=0; i<HASHTABLESIZE; i++) {
      runner=&(hashtable[i]);
      if (runner->p != -1) {
         dumpint(-i);
         while (runner->p != -1) {
            dumpint(runner->p);
            dumpwd(runner->mw);
            runner=runner->ptr;
         }
      }
   }
   dumpint(-HASHTABLESIZE);
}

void
undumphhashtable (void)
{
   int i,j;
   hashword *runner;
   j=0;
   undumpint(i);
   while (j<HASHTABLESIZE) {
      i = (-i);
      while (j<i) {
         hashtable[j].p = -1;
         j++;
      }
      if (i<HASHTABLESIZE) {
         runner = &(hashtable[j]);
         undumpint(i);
         while (i>=0) {
            runner->p = i;
            undumpwd(runner->mw);
            undumpint(i);
            runner->ptr = (hashword *) xmalloc(sizeof(hashword));
            runner = runner->ptr;
         }
         runner->p = -1;
         j++;
      }
   }
}

void
odateandtime (int timecode, int daycode, int monthcode, int yearcode)
{
    integer tc,dc,mc,yc;
    dateandtime(tc,dc,mc,yc);
    setneweqtbint(timecode,tc);
    setneweqtbint(daycode,dc);
    setneweqtbint(monthcode,mc);
    setneweqtbint(yearcode,yc);
}

memoryword **fonttables;
static int font_entries = 0;

void
allocatefonttable (int font_number, int font_size)
{
    int i;
    if (font_entries==0) {
      fonttables = xmalloc(256*sizeof(memoryword*));
      font_entries=256;
    } else if ((font_number==256)&&(font_entries==256)) {
      fonttables = xrealloc(fonttables, 65536*sizeof(memoryword*));
      font_entries=65536;
    }
    fonttables[font_number] = xmalloc((font_size+1)*sizeof(memoryword));
    fonttables[font_number][0].cint = font_size;
    fonttables[font_number][0].cint1 = 0;
    for (i=1; i<=font_size; i++) {
        fonttables[font_number][i].cint  = 0;
        fonttables[font_number][i].cint1 = 0;
    }
}

void
dumpfonttable (int  font_number, int  words)
{
    fonttables[font_number][0].cint=words;
    dumpthings(fonttables[font_number][0], fonttables[font_number][0].cint+1);
}

void
undumpfonttable(int font_number)
{
    memoryword sizeword;
    if (font_entries==0) {
      fonttables = xmalloc(256*sizeof(memoryword*));
      font_entries=256;
    } else if ((font_number==256)&&(font_entries==256)) {
      fonttables = xrealloc(fonttables, 65536*sizeof(memoryword*));
      font_entries=65536;
    }

    undumpthings(sizeword,1);
    fonttables[font_number] = xmalloc((sizeword.cint+1)*sizeof(memoryword));
    fonttables[font_number][0].cint = sizeword.cint;
    undumpthings(fonttables[font_number][1], sizeword.cint);
}

#if 0 /* unused */
memoryword **fontsorttables;
static int fontsort_entries = 0;

void
allocatefontsorttable (int  fontsort_number, int  fontsort_size)
{
    int i;
    if (fontsort_entries==0) {
      fontsorttables = xmalloc(256*sizeof(memoryword*));
      fontsort_entries=256;
    } else if ((fontsort_number==256)&&(fontsort_entries==256)) {
      fontsorttables = xrealloc(fontsorttables, 65536*sizeof(memoryword*));
      fontsort_entries=65536;
    }
    fontsorttables[fontsort_number] = xmalloc((fontsort_size+1)*sizeof(memoryword));
    fontsorttables[fontsort_number][0].cint = fontsort_size;
    fontsorttables[fontsort_number][0].cint1 = 0;
    for (i=1; i<=fontsort_size; i++) {
        fontsorttables[fontsort_number][i].cint  = 0;
        fontsorttables[fontsort_number][i].cint1 = 0;
    }
}

void
dumpfontsorttable (int fontsort_number, int words)
{
    fontsorttables[fontsort_number][0].cint=words;
    dumpthings(fontsorttables[fontsort_number][0],
               fontsorttables[fontsort_number][0].cint+1);
}

void
undumpfontsorttable(int fontsort_number)
{
    memoryword sizeword;
    if (fontsort_entries==0) {
      fontsorttables = xmalloc(256*sizeof(memoryword*));
      fontsort_entries=256;
    } else if ((fontsort_number==256)&&(fontsort_entries==256)) {
      fontsorttables = xrealloc(fontsorttables, 65536*sizeof(memoryword*));
      fontsort_entries=65536;
    }

    undumpthings(sizeword,1);
    fontsorttables[fontsort_number] = xmalloc((sizeword.cint+1)*sizeof(memoryword));
    fontsorttables[fontsort_number][0].cint = sizeword.cint;
    undumpthings(fontsorttables[fontsort_number][1], sizeword.cint);
}
#endif

int **ocptables;
static int ocp_entries = 0;

void
allocateocptable (int ocp_number, int ocp_size)
{
    int i;
    if (ocp_entries==0) {
      ocptables = xmalloc(256*sizeof(int*));
      ocp_entries=256;
    } else if ((ocp_number==256)&&(ocp_entries==256)) {
      ocptables = xrealloc(ocptables, 65536*sizeof(int*));
      ocp_entries=65536;
    }
    ocptables[ocp_number] = xmalloc((1+ocp_size)*sizeof(int));
    ocptables[ocp_number][0] = ocp_size;
    for (i=1; i<=ocp_size; i++) {
        ocptables[ocp_number][i]  = 0;
    }
}

void
dumpocptable (int ocp_number)
{
    dumpthings(ocptables[ocp_number][0], ocptables[ocp_number][0]+1);
}

void
undumpocptable (int ocp_number)
{
    int sizeword;
    if (ocp_entries==0) {
      ocptables = xmalloc(256*sizeof(int*));
      ocp_entries=256;
    } else if ((ocp_number==256)&&(ocp_entries==256)) {
      ocptables = xrealloc(ocptables, 65536*sizeof(int*));
      ocp_entries=65536;
    }
    undumpthings(sizeword,1);
    ocptables[ocp_number] = xmalloc((1+sizeword)*sizeof(int));
    ocptables[ocp_number][0] = sizeword;
    undumpthings(ocptables[ocp_number][1], sizeword);
}
