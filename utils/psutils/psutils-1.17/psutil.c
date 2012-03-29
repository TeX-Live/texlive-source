/* psutil.c
 * Copyright (C) Angus J. C. Duggan 1991-1995
 * See file LICENSE for details.
 *
 * utilities for PS programs
 */

/*
 *  AJCD 6/4/93
 *    Changed to using ftell() and fseek() only (no length calculations)
 *  Hunter Goatley    31-MAY-1993 23:33
 *    Fixed VMS support.
 *  Hunter Goatley     2-MAR-1993 14:41
 *    Added VMS support.
 */
#include "psutil.h"
#include "pserror.h"
#include "patchlev.h"

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define iscomment(x,y) (strncmp(x,y,strlen(y)) == 0)

static char buffer[BUFSIZ];
static long bytes = 0;
static long pagescmt = 0;
static long headerpos = 0;
static long endsetup = 0;
static long beginprocset = 0;		/* start of pstops procset */
static long endprocset = 0;
static int outputpage = 0;
static int maxpages = 100;
static long *pageptr;

/* list of paper sizes supported */
static Paper papersizes[] = {
   { "a3", 842, 1191 },		/* 29.7cm * 42cm */
   { "a4", 595, 842 },		/* 21cm * 29.7cm */
   { "a5", 421, 595 },		/* 14.85cm * 21cm */
   { "b5", 516, 729 },		/* 18.2cm * 25.72cm */
   { "A3", 842, 1191 },		/* 29.7cm * 42cm */
   { "A4", 595, 842 },		/* 21cm * 29.7cm */
   { "A5", 421, 595 },		/* 14.85cm * 21cm */
   { "B5", 516, 729 },		/* 18.2cm * 25.72cm */
   { "letter", 612, 792 },	/* 8.5in * 11in */
   { "legal", 612, 1008 },	/* 8.5in * 14in */
   { "ledger", 1224, 792 },	/* 17in * 11in */
   { "tabloid", 792, 1224 },	/* 11in * 17in */
   { "statement", 396, 612 },	/* 5.5in * 8.5in */
   { "executive", 540, 720 },	/* 7.6in * 10in */
   { "folio", 612, 936 },	/* 8.5in * 13in */
   { "quarto", 610, 780 },	/* 8.5in * 10.83in */
   { "10x14", 720, 1008 },	/* 10in * 14in */
   { NULL, 0, 0 }
};

/* return pointer to paper size struct or NULL */
Paper* findpaper(const char *name)
{
   Paper *pp;
   for (pp = papersizes; PaperName(pp); pp++) {
      if (strcmp(PaperName(pp), name) == 0) {
	 return pp;
      }
   }
   return (Paper *)NULL;
}

/* Make a file seekable, using temporary files if necessary */
FILE *seekable(FILE *fp)
{
#ifndef MSDOS
  FILE *ft;
  long r, w ;
#endif
  char *p;
  char buffer[BUFSIZ] ;
#if defined(WINNT) || defined(WIN32)
  struct _stat fs ;
#else
  long fpos;
#endif

#if defined(WINNT) || defined(WIN32)
  if (_fstat(fileno(fp), &fs) == 0 && (fs.st_mode&_S_IFREG) != 0)
    return (fp);
#else
  if ((fpos = ftell(fp)) >= 0)
    if (!fseek(fp, 0L, SEEK_END) && !fseek(fp, fpos, SEEK_SET))
      return (fp);
#endif

#if defined(MSDOS)
  message(FATAL, "input is not seekable\n");
  return (NULL) ;
#else
  if ((ft = tmpfile()) == NULL)
    return (NULL);

  while ((r = fread(p = buffer, sizeof(char), BUFSIZ, fp)) > 0) {
    do {
      if ((w = fwrite(p, sizeof(char), r, ft)) == 0)
	return (NULL) ;
      p += w ;
      r -= w ;
    } while (r > 0) ;
  }

  if (!feof(fp))
    return (NULL) ;

  /* discard the input file, and rewind the temporary */
  (void) fclose(fp);
  if (fseek(ft, 0L, SEEK_SET) != 0)
    return (NULL) ;

  return (ft);
#endif
}


/* copy input file from current position upto new position to output file */
static int fcopy(long upto)
{
   long here = ftell(infile);
   while (here < upto) {
      if ((fgets(buffer, BUFSIZ, infile) == NULL) ||
	  (fputs(buffer, outfile) == EOF))
	 return(0);
      here = ftell(infile);
      bytes += strlen(buffer);
   }
   return (1);
}

/* build array of pointers to start/end of pages */
void scanpages(void)
{
   register char *comment = buffer+2;
   register int nesting = 0;
   register long int record;

   if ((pageptr = (long *)malloc(sizeof(long)*maxpages)) == NULL)
      message(FATAL, "out of memory\n");
   pages = 0;
   fseek(infile, 0L, SEEK_SET);
   while (record = ftell(infile), fgets(buffer, BUFSIZ, infile) != NULL)
      if (*buffer == '%') {
	 if (buffer[1] == '%') {
	    if (nesting == 0 && iscomment(comment, "Page:")) {
	       if (pages >= maxpages-1) {
		  maxpages *= 2;
		  if ((pageptr = (long *)realloc((char *)pageptr,
					     sizeof(long)*maxpages)) == NULL)
		     message(FATAL, "out of memory\n");
	       }
	       pageptr[pages++] = record;
	    } else if (headerpos == 0 && iscomment(comment, "Pages:"))
	       pagescmt = record;
	    else if (headerpos == 0 && iscomment(comment, "EndComments"))
	       headerpos = ftell(infile);
	    else if (iscomment(comment, "BeginDocument") ||
		     iscomment(comment, "BeginBinary") ||
		     iscomment(comment, "BeginFile"))
	       nesting++;
	    else if (iscomment(comment, "EndDocument") ||
		     iscomment(comment, "EndBinary") ||
		     iscomment(comment, "EndFile"))
	       nesting--;
	    else if (nesting == 0 && iscomment(comment, "EndSetup"))
	       endsetup = record;
	    else if (nesting == 0 && iscomment(comment, "BeginProlog"))
	       headerpos = ftell(infile);
	    else if (nesting == 0 &&
		       iscomment(comment, "BeginProcSet: PStoPS"))
	       beginprocset = record;
	    else if (beginprocset && !endprocset &&
		     iscomment(comment, "EndProcSet"))
	       endprocset = ftell(infile);
	    else if (nesting == 0 && (iscomment(comment, "Trailer") ||
				      iscomment(comment, "EOF"))) {
	       fseek(infile, record, SEEK_SET);
	       break;
	    }
	 } else if (headerpos == 0 && buffer[1] != '!')
	    headerpos = record;
      } else if (headerpos == 0)
	 headerpos = record;
   pageptr[pages] = ftell(infile);
   if (endsetup == 0 || endsetup > pageptr[0])
      endsetup = pageptr[0];
}

/* seek a particular page */
void seekpage(int p)
{
   fseek(infile, pageptr[p], SEEK_SET);
   if (fgets(buffer, BUFSIZ, infile) != NULL &&
       iscomment(buffer, "%%Page:")) {
      char *start, *end;
      for (start = buffer+7; isspace(*start); start++);
      if (*start == '(') {
	 int paren = 1;
	 for (end = start+1; paren > 0; end++)
	    switch (*end) {
	    case '\0':
	       message(FATAL, "Bad page label while seeking page %d\n", p);
	    case '(':
	       paren++;
	       break;
	    case ')':
	       paren--;
	       break;
	    }
      } else
	 for (end = start; !isspace(*end); end++);
      strncpy(pagelabel, start, end-start);
      pagelabel[end-start] = '\0';
      pageno = atoi(end);
   } else
      message(FATAL, "I/O error seeking page %d\n", p);
}

/* Output routines. These all update the global variable bytes with the number
 * of bytes written */
void writestring(const char *s)
{
   fputs(s, outfile);
   bytes += strlen(s);
}

/* write page comment */
void writepageheader(const char *label, int page)
{
   if (verbose)
      message(LOG, "[%d] ", page);
   sprintf(buffer, "%%%%Page: %s %d\n", label, ++outputpage);
   writestring(buffer);
}

/* search for page setup */
void writepagesetup(void)
{
   char buffer[BUFSIZ];
   if (beginprocset) {
      for (;;) {
	 if (fgets(buffer, BUFSIZ, infile) == NULL)
	    message(FATAL, "I/O error reading page setup %d\n", outputpage);
	 if (!strncmp(buffer, "PStoPSxform", 11))
	    break;
	 if (fputs(buffer, outfile) == EOF)
	    message(FATAL, "I/O error writing page setup %d\n", outputpage);
	 bytes += strlen(buffer);
      }
   }
}

/* write the body of a page */
void writepagebody(int p)
{
   if (!fcopy(pageptr[p+1]))
      message(FATAL, "I/O error writing page %d\n", outputpage);
}

/* write a whole page */
void writepage(int p)
{
   seekpage(p);
   writepageheader(pagelabel, p+1);
   writepagebody(p);
}

/* write from start of file to end of header comments */
void writeheader(int p)
{
   fseek(infile, 0L, SEEK_SET);
   if (pagescmt) {
      if (!fcopy(pagescmt) || fgets(buffer, BUFSIZ, infile) == NULL)
	 message(FATAL, "I/O error in header\n");
      sprintf(buffer, "%%%%Pages: %d 0\n", p);
      writestring(buffer);
   }
   if (!fcopy(headerpos))
      message(FATAL, "I/O error in header\n");
}

/* write prologue to end of setup section excluding PStoPS procset */
int writepartprolog(void)
{
   if (beginprocset && !fcopy(beginprocset))
      message(FATAL, "I/O error in prologue\n");
   if (endprocset)
      fseek(infile, endprocset, SEEK_SET);
   writeprolog();
   return !beginprocset;
}

/* write prologue up to end of setup section */
void writeprolog(void)
{
   if (!fcopy(endsetup))
      message(FATAL, "I/O error in prologue\n");
}

/* write from end of setup to start of pages */
void writesetup(void)
{
   if (!fcopy(pageptr[0]))
      message(FATAL, "I/O error in prologue\n");
}

/* write trailer */
void writetrailer(void)
{
   fseek(infile, pageptr[pages], SEEK_SET);
   while (fgets(buffer, BUFSIZ, infile) != NULL) {
      writestring(buffer);
   }
   if (verbose)
      message(LOG, "Wrote %d pages, %ld bytes\n", outputpage, bytes);
}

/* write a page with nothing on it */
void writeemptypage(void)
{
   if (verbose)
      message(LOG, "[*] ");
   sprintf(buffer, "%%%%Page: * %d\n", ++outputpage);
   writestring(buffer);
   if (beginprocset)
      writestring("PStoPSxform concat\n");
   writestring("showpage\n");
}

