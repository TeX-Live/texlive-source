/* omega.c: C routines to support the Omega Pascal Web distribution

This file is part of Omega,
which is based on the web2c distribution of TeX,

Copyright (c) 1994--2001 John Plaice and Yannis Haralambous

Omega is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Omega is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Omega; if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.

*/

#define EXTERN extern
#include "omegad.h"

void
btestin(void)
{
    string fname =
    kpse_find_file (nameoffile + 1, kpse_program_binary_format, true);

    if (fname) {
      libcfree(nameoffile);
      nameoffile = (char *)xmalloc(2+strlen(fname));
      namelength = strlen(fname);
      strcpy(nameoffile+1, fname);
    }
    else {
      libcfree(nameoffile);
      nameoffile = (char *)xmalloc(2);
      namelength = 0;
      nameoffile[0] = 0;
      nameoffile[1] = 0;
    }
}

int
getfilemode P2C(FILE *, f, int, def)
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

int
getc_two_LE P1C(FILE *, f)
{
    register int i,j;
    i = getc(f);
    if (i==EOF) { return i; }
    j = getc(f);
    if (j==EOF) { return j; }
    return ((j<<8)|i);
}

void
ungetc_two_LE P2C(int, c, FILE *, f)
{
    ungetc((c>>8), f);
    ungetc((c&0377), f);
}

int
getc_two P1C(FILE *, f)
{      
    register int i,j;
    i = getc(f);
    if (i==EOF) { return i; }
    j = getc(f);
    if (j==EOF) { return j; }
    return ((i<<8)|j);
}
 
void
ungetc_two P2C(int, c, FILE *, f)
{
    ungetc((c&0377), f);
    ungetc((c>>8), f);
}

#define advance_cin if ((c_in = fgetc(f)) == EOF) return EOF

int
getc_UTF8 P1C(FILE *, f)
{
    register int c_in,c_out;

    advance_cin;
    if (c_in<0x80) {
        c_out = c_in & 0x7f;
    } else if (c_in<0xe0) {
        c_out = (c_in & 0x1f) << 6;
        advance_cin;
        c_out |= c_in & 0x3f;
    } else if (c_in<=0xf0) {
        c_out = (c_in & 0xf) << 12;
        advance_cin;
        c_out |= (c_in & 0x3f) << 6;
        advance_cin;
        c_out |= c_in & 0x3f;
    } else if (c_in<0xf8) {
        c_out = (c_in & 0x7) << 18;
        advance_cin;
        c_out |= (c_in & 0x3f) << 12;
        advance_cin;
        c_out |= (c_in & 0x3f) << 6;
        advance_cin;
        c_out |= c_in & 0x3f;
    } else if (c_in<0xfc) {
        c_out = (c_in & 0x3) << 24;
        advance_cin;
        c_out |= (c_in & 0x3f) << 18;
        advance_cin;
        c_out |= (c_in & 0x3f) << 12;
        advance_cin;
        c_out |= (c_in & 0x3f) << 6;
        advance_cin;
        c_out |= c_in & 0x3f;
    } else { /* c>=0xfc */
        c_out = (c_in & 0x1)   << 30;
        advance_cin;
        c_out |= (c_in & 0x3f) << 24;
        advance_cin;
        c_out |= (c_in & 0x3f) << 18;
        advance_cin;
        c_out |= (c_in & 0x3f) << 12;
        advance_cin;
        c_out |= (c_in & 0x3f) << 6;
        advance_cin;
        c_out |= c_in & 0x3f;
    }
    return c_out;
}

void
ungetc_UTF8 P2C(int, c, FILE *, f)
{
    /* Still to be done */
}

#define advance_two_cin advance_cin; \
    c_in_hi = c_in; \
    advance_cin; \
    c_in = (c_in_hi & 0xff) << 8

int
getc_UTF16 P1C(FILE *, f)
{
    register int c_in,c_in_hi,c_out;

    advance_two_cin;
    if ((c_in>=0xd800) && (c_in<=0xdbff)) {
        c_out = (c_in - 0xd800) * 0x400;
        advance_two_cin;
        if ((c_in>=0xdc00) && (c_in<=0xdfff)) {
           c_out += (c_in - 0xdc00) + 0x10000;
        } else {
           return 0xfffd;
        }
    } else {
        c_out = c_in;
    }
    return c_out;
}

void
ungetc_UTF16 P2C(int, c, FILE *, f)
{
    /* Still to be done */
}

#define advance_twoLE_cin advance_cin; \
    c_in_lo = c_in; \
    advance_cin; \
    c_in = (c_in << 8) | (c_in_lo & 0xff)

int
getc_UTF16LE P1C(FILE *, f)
{
    register int c_in,c_in_lo,c_out;

    advance_twoLE_cin;
    if ((c_in>=0xd800) && (c_in<=0xdbff)) {
        c_out = (c_in - 0xd800) * 0x400;
        advance_twoLE_cin;
        if ((c_in>=0xdc00) && (c_in<=0xdfff)) {
           c_out += (c_in - 0xdc00) + 0x10000;
        } else {
           return 0xfffd;
        }
    } else {
        c_out = c_in;
    }
    return c_out;
}

void
ungetc_UTF16LE P2C(int, c, FILE *, f)
{
    /* Still to be done */
}

 
extern boolean zpnewinputln ();

boolean
newinputln P4C(FILE *,f, halfword,themode, halfword,translation, boolean,bypass)
{
    return zpnewinputln((alphafile)f,
                        (halfword)themode,
                        (halfword)translation,
                        (boolean) bypass);
}
 
boolean
new_input_line P2C(FILE *, f, halfword, themode)
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
  } else if (themode==4) {
     while ((otpinputend < ocpbufsize) && ((i = getc_two_LE (f)) != EOF) &&
            ((i != '\r') && (i != '\n')))
        otpinputbuf[++otpinputend] = i;
     if (i=='\r') {
         i=getc_two_LE(f); if (i != '\n') ungetc_two_LE(i,f);
     }
  } else if (themode==5) {
     while ((otpinputend < ocpbufsize) && ((i = getc_UTF8 (f)) != EOF) &&
            ((i != '\r') && (i != '\n')))
        otpinputbuf[++otpinputend] = i;
     if (i=='\r') {
         i=getc_UTF8(f); if (i != '\n') ungetc_UTF8(i,f);
     }
  } else if (themode==6) {
     while ((otpinputend < ocpbufsize) && ((i = getc_UTF16 (f)) != EOF) &&
            ((i != '\r') && (i != '\n')))
        otpinputbuf[++otpinputend] = i;
     if (i=='\r') {
         i=getc_UTF16(f); if (i != '\n') ungetc_UTF16(i,f);
     }
  } else /* themode==7 */ {
     while ((otpinputend < ocpbufsize) && ((i = getc_UTF16LE (f)) != EOF) &&
            ((i != '\r') && (i != '\n')))
        otpinputbuf[++otpinputend] = i;
     if (i=='\r') {
         i=getc_UTF16LE(f); if (i != '\n') ungetc_UTF16LE(i,f);
     }
  }
 
  if (i == EOF && otpinputend == 0)
      return false;
 
  /* We didn't get the whole line because our buffer was too small.
*/
 if (i != EOF && (((themode!=2) && (i != '\n')) || ((themode==2) && (i != 0x25))))
    {
      (void) fprintf (stderr,
                     "! Unable to read an entire line---ocp_buf_size=%ld.\n",ocpbufsize);
      (void) fprintf (stderr, "Please increase ocp_buf_size in texmf.cnf.\n");
      uexit (1);
    }
 
    return true;
}

extern memoryword ziniteqtbentry () ;

hashword hashtable[HASHTABLESIZE];

void
inithhashtable P1H(void)
{
   int i;
   for (i=0; i<HASHTABLESIZE; i++) {
      hashtable[i].p = -1;
   }
      
}

hashword *
createeqtbpos P1C(int, p)
{
   hashword *runner= &(hashtable[p%HASHTABLESIZE]);
   if (runner->p==p) return runner;
   while (runner->p != -1) {
      runner = runner->ptr; 
      if (runner->p == p) return runner;
   } 
   runner->p = p;
   runner->mw = ziniteqtbentry(p);
   runner->ptr = (hashword *) xmalloc(sizeof(hashword));
   (runner->ptr)->p = -1;
   return runner;
}

memoryword *
createeqtbptr P1C(int, p)
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

hashword *
createxeqlevel P1C(int, p)
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
createhashpos P1C(int, p)
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
dumphhashtable P1H(void)
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
undumphhashtable P1H(void)
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
odateandtime P4C(int,timecode, int,daycode, int,monthcode, int,yearcode)
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
allocatefonttable P2C(int, font_number, int, font_size)
{
    int i;
    if (font_entries==0) {
      fonttables = (memoryword **) xmalloc(256*sizeof(memoryword**));
      font_entries=256;
    } else if ((font_number==256)&&(font_entries==256)) {
      fonttables = (memoryword **) xrealloc(fonttables, 65536);
      font_entries=65536;
    }
    fonttables[font_number] =
       (memoryword *) xmalloc((font_size+1)*sizeof(memoryword));
    fonttables[font_number][0].cint = font_size;
    fonttables[font_number][0].cint1 = 0;
    for (i=1; i<=font_size; i++) {
        fonttables[font_number][i].cint  = 0;
        fonttables[font_number][i].cint1 = 0;
    }
}

void
dumpfonttable P2C(int, font_number, int, words)
{
    fonttables[font_number][0].cint=words;
    dumpthings(fonttables[font_number][0], fonttables[font_number][0].cint+1);
}

void
undumpfonttable P1C(int, font_number)
{
    memoryword sizeword;
    if (font_entries==0) {
      fonttables = (memoryword **) xmalloc(256*sizeof(memoryword**));
      font_entries=256;
    } else if ((font_number==256)&&(font_entries==256)) {
      fonttables = (memoryword **) xrealloc(fonttables, 65536);
      font_entries=65536;
    }

    undumpthings(sizeword,1);
    fonttables[font_number] =
        (memoryword *) xmalloc((sizeword.cint+1)*sizeof(memoryword));
    fonttables[font_number][0].cint = sizeword.cint;
    undumpthings(fonttables[font_number][1], sizeword.cint);
}

memoryword **fontsorttables;
static int fontsort_entries = 0;

void
allocatefontsorttable P2C(int, fontsort_number, int, fontsort_size)
{
    int i;
    if (fontsort_entries==0) {
      fontsorttables = (memoryword **) xmalloc(256*sizeof(memoryword**));
      fontsort_entries=256;
    } else if ((fontsort_number==256)&&(fontsort_entries==256)) {
      fontsorttables = (memoryword **) xrealloc(fontsorttables, 65536);
      fontsort_entries=65536;
    }
    fontsorttables[fontsort_number] =
       (memoryword *) xmalloc((fontsort_size+1)*sizeof(memoryword));
    fontsorttables[fontsort_number][0].cint = fontsort_size;
    fontsorttables[fontsort_number][0].cint1 = 0;
    for (i=1; i<=fontsort_size; i++) {
        fontsorttables[fontsort_number][i].cint  = 0;
        fontsorttables[fontsort_number][i].cint1 = 0;
    }
}

void
dumpfontsorttable P2C(int, fontsort_number, int, words)
{
    fontsorttables[fontsort_number][0].cint=words;
    dumpthings(fontsorttables[fontsort_number][0],
               fontsorttables[fontsort_number][0].cint+1);
}

void
undumpfontsorttable P1C(int, fontsort_number)
{
    memoryword sizeword;
    if (fontsort_entries==0) {
      fontsorttables = (memoryword **) xmalloc(256*sizeof(memoryword**));
      fontsort_entries=256;
    } else if ((fontsort_number==256)&&(fontsort_entries==256)) {
      fontsorttables = (memoryword **) xrealloc(fontsorttables, 65536);
      fontsort_entries=65536;
    }

    undumpthings(sizeword,1);
    fontsorttables[fontsort_number] =
        (memoryword *) xmalloc((sizeword.cint+1)*sizeof(memoryword));
    fontsorttables[fontsort_number][0].cint = sizeword.cint;
    undumpthings(fontsorttables[fontsort_number][1], sizeword.cint);
}

int **ocptables;
static int ocp_entries = 0;

void
allocateocptable P2C(int, ocp_number, int, ocp_size)
{
    int i;
    if (ocp_entries==0) {
      ocptables = (int **) xmalloc(256*sizeof(int**));
      ocp_entries=256;
    } else if ((ocp_number==256)&&(ocp_entries==256)) {
      ocptables = (int **) xrealloc(ocptables, 65536);
      ocp_entries=65536;
    }
    ocptables[ocp_number] =
       (int *) xmalloc((1+ocp_size)*sizeof(int));
    ocptables[ocp_number][0] = ocp_size;
    for (i=1; i<=ocp_size; i++) {
        ocptables[ocp_number][i]  = 0;
    }
}

void
dumpocptable P1C(int, ocp_number)
{
    dumpthings(ocptables[ocp_number][0], ocptables[ocp_number][0]+1);
}

void
undumpocptable P1C(int, ocp_number)
{
    int sizeword;
    if (ocp_entries==0) {
      ocptables = (int **) xmalloc(256*sizeof(int**));
      ocp_entries=256;
    } else if ((ocp_number==256)&&(ocp_entries==256)) {
      ocptables = (int **) xrealloc(ocptables, 65536);
      ocp_entries=65536;
    }
    undumpthings(sizeword,1);
    ocptables[ocp_number] =
        (int *) xmalloc((1+sizeword)*sizeof(int));
    ocptables[ocp_number][0] = sizeword;
    undumpthings(ocptables[ocp_number][1], sizeword);
}
