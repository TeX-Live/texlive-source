/* psspec.c
 * Copyright (C) Angus J. C. Duggan 1991-1995
 * See file LICENSE for details.
 *
 * page spec routines for page rearrangement
 */

#include "psutil.h"
#include "psspec.h"
#include "pserror.h"
#include "patchlev.h"

#include <string.h>

double width = -1;
double height = -1;

/* create a new page spec */
PageSpec *newspec(void)
{
   PageSpec *temp = (PageSpec *)malloc(sizeof(PageSpec));
   if (temp == NULL)
      message(FATAL, "out of memory\n");
   temp->reversed = temp->pageno = temp->flags = temp->rotate = 0;
   temp->scale = 1;
   temp->xoff = temp->yoff = 0;
   temp->next = NULL;
   return (temp);
}

/* dimension parsing routines */
int parseint(char **sp, void (*errorfn)(void))
{
   char *s = *sp;
   int num = atoi(s);

   while (isdigit(*s))
      s++;
   if (*sp == s) (*errorfn)() ;
   *sp = s;
   return (num);
}

double parsedouble(char **sp, void (*errorfn)(void))
{
   char *s = *sp;
   double num = atof(s);

   while (isdigit(*s) || *s == '-' || *s == '.')
      s++;
   if (*sp == s) (*errorfn)() ;
   *sp = s;
   return (num);
}

double parsedimen(char **sp, void (*errorfn)(void))
{
   double num = parsedouble(sp, errorfn);
   char *s = *sp;

   if (strncmp(s, "pt", 2) == 0) {
      s += 2;
   } else if (strncmp(s, "in", 2) == 0) {
      num *= 72;
      s += 2;
   } else if (strncmp(s, "cm", 2) == 0) {
      num *= 28.346456692913385211;
      s += 2;
   } else if (strncmp(s, "mm", 2) == 0) {
      num *= 2.8346456692913385211;
      s += 2;
   } else if (*s == 'w') {
      if (width < 0)
	 message(FATAL, "width not initialised\n");
      num *= width;
      s++;
   } else if (*s == 'h') {
      if (height < 0)
	 message(FATAL, "height not initialised\n");
      num *= height;
      s++;
   }
   *sp = s;
   return (num);
}

double singledimen(char *str, void (*errorfn)(void), void (*usagefn)(void))
{
   double num = parsedimen(&str, errorfn);
   if (*str) (*usagefn)();
   return (num);
}

static const char *prologue[] = { /* PStoPS procset */
#ifndef SHOWPAGE_LOAD
   "userdict begin",
   "[/showpage/erasepage/copypage]{dup where{pop dup load",	/* prevent */
   " type/operatortype eq{1 array cvx dup 0 3 index cvx put",	/* binding */
   " bind def}{pop}ifelse}{pop}ifelse}forall",			/* in prolog */
#else
   "userdict begin",
   "[/showpage/copypage/erasepage]{dup 10 string cvs dup",
   " length 6 add string dup 0 (PStoPS) putinterval dup",
   " 6 4 -1 roll putinterval 2 copy cvn dup where",
   " {pop pop pop}{exch load def}ifelse cvx cvn 1 array cvx",
   " dup 0 4 -1 roll put def}forall",
#endif
   "[/letter/legal/executivepage/a4/a4small/b5/com10envelope",	/* nullify */
   " /monarchenvelope/c5envelope/dlenvelope/lettersmall/note",	/* paper */
   " /folio/quarto/a5]{dup where{dup wcheck{exch{}put}",	/* operators */
   " {pop{}def}ifelse}{pop}ifelse}forall",
   "/setpagedevice {pop}bind 1 index where{dup wcheck{3 1 roll put}",
   " {pop def}ifelse}{def}ifelse",
   "/PStoPSmatrix matrix currentmatrix def",
   "/PStoPSxform matrix def/PStoPSclip{clippath}def",
   "/defaultmatrix{PStoPSmatrix exch PStoPSxform exch concatmatrix}bind def",
   "/initmatrix{matrix defaultmatrix setmatrix}bind def",
   "/initclip[{matrix currentmatrix PStoPSmatrix setmatrix",
   " [{currentpoint}stopped{$error/newerror false put{newpath}}",
   " {/newpath cvx 3 1 roll/moveto cvx 4 array astore cvx}ifelse]",
   " {[/newpath cvx{/moveto cvx}{/lineto cvx}",
   " {/curveto cvx}{/closepath cvx}pathforall]cvx exch pop}",
   " stopped{$error/errorname get/invalidaccess eq{cleartomark",
   " $error/newerror false put cvx exec}{stop}ifelse}if}bind aload pop",
   " /initclip dup load dup type dup/operatortype eq{pop exch pop}",
   " {dup/arraytype eq exch/packedarraytype eq or",
   "  {dup xcheck{exch pop aload pop}{pop cvx}ifelse}",
   "  {pop cvx}ifelse}ifelse",
   " {newpath PStoPSclip clip newpath exec setmatrix} bind aload pop]cvx def",
   "/initgraphics{initmatrix newpath initclip 1 setlinewidth",
   " 0 setlinecap 0 setlinejoin []0 setdash 0 setgray",
   " 10 setmiterlimit}bind def",
   "end",
   NULL
   };

void pstops(int modulo, int pps, int nobind, PageSpec *specs, double draw)
{
   int thispg, maxpage;
   int pageindex = 0;
   const char **pro;

   scanpages();

   maxpage = ((pages+modulo-1)/modulo)*modulo;

   /* rearrange pages: doesn't cope properly with loaded definitions */
   writeheader((maxpage/modulo)*pps);
#ifndef SHOWPAGE_LOAD
   writestring("%%BeginProcSet: PStoPS");
#else
   writestring("%%BeginProcSet: PStoPS-spload");
#endif
   if (nobind)
      writestring("-nobind");
   writestring(" 1 15\n");
   for (pro = prologue; *pro; pro++) {
      writestring(*pro);
      writestring("\n");
   }
   if (nobind) /* desperation measures */
      writestring("/bind{}def\n");
   writestring("%%EndProcSet\n");
   /* save transformation from original to current matrix */
   if (writepartprolog()) {
      writestring("userdict/PStoPSxform PStoPSmatrix matrix currentmatrix\n");
      writestring(" matrix invertmatrix matrix concatmatrix\n");
      writestring(" matrix invertmatrix put\n");
   }
   writesetup();
   for (thispg = 0; thispg < maxpage; thispg += modulo) {
      int add_last = 0;
      PageSpec *ps;
      for (ps = specs; ps != NULL; ps = ps->next) {
	 int actualpg;
	 int add_next = ((ps->flags & ADD_NEXT) != 0);
	 if (ps->reversed)
	    actualpg = maxpage-thispg-modulo+ps->pageno;
	 else
	    actualpg = thispg+ps->pageno;
	 if (actualpg < pages)
	    seekpage(actualpg);
	 if (!add_last) {	/* page label contains original pages */
	    PageSpec *np = ps;
	    char *eob = pagelabel;
	    char sep = '(';
	    do {
	       *eob++ = sep;
	       if (np->reversed)
		  sprintf(eob, "%d", maxpage-thispg-modulo+np->pageno);
	       else
		  sprintf(eob, "%d", thispg+np->pageno);
	       eob = eob + strlen(eob);
	       sep = ',';
	    } while ((np->flags & ADD_NEXT) && (np = np->next));
	    strcpy(eob, ")");
	    writepageheader(pagelabel, ++pageindex);
	 }
	 writestring("userdict/PStoPSsaved save put\n");
	 if (ps->flags & GSAVE) {
	    char buffer[BUFSIZ];
	    writestring("PStoPSmatrix setmatrix\n");
	    if (ps->flags & OFFSET) {
	       sprintf(buffer, "%f %f translate\n", ps->xoff, ps->yoff);
	       writestring(buffer);
	    }
	    if (ps->flags & ROTATE) {
	       sprintf(buffer, "%d rotate\n", ps->rotate);
	       writestring(buffer);
	    }
	    if (ps->flags & SCALE) {
	       sprintf(buffer, "%f dup scale\n", ps->scale);
	       writestring(buffer);
	    }
	    writestring("userdict/PStoPSmatrix matrix currentmatrix put\n");
	    if (width > 0 && height > 0) {
	       writestring("userdict/PStoPSclip{0 0 moveto\n");
	       sprintf(buffer, " %f 0 rlineto 0 %f rlineto -%f 0 rlineto\n",
		       width, height, width);
	       writestring(buffer);
	       writestring(" closepath}put initclip\n");
	       if (draw > 0) {
		  sprintf(buffer, "gsave clippath 0 setgray %f setlinewidth stroke grestore\n", draw);
		  writestring(buffer);
	       }
	    }
	 }
	 if (add_next) {
#ifndef SHOWPAGE_LOAD
	    writestring("/showpage{}def/copypage{}def/erasepage{}def\n");
#else
	    writestring("/PStoPSshowpage{}store/PStoPScopypage{}store/PStoPSerasepage{}store\n");
#endif
	 }
	 if (actualpg < pages) {
	    writepagesetup();
	    writestring("PStoPSxform concat\n");
	    writepagebody(actualpg);
	 } else {
	    writestring("PStoPSxform concat\n");
	    writestring("showpage\n");
	 }
	 writestring("PStoPSsaved restore\n");
	 add_last = add_next;
      }
   }
   writetrailer();
}
