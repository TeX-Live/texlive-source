/* psselect.c
 * Copyright (C) Angus J. C. Duggan 1991-1995
 * See file LICENSE for details.
 *
 * rearrange pages in conforming PS file for printing in signatures
 *
 * Usage:
 *       psselect [-q] [-e] [-o] [-r] [-p<pages>] [infile [outfile]]
 */

#include "psutil.h"
#include "pserror.h"
#include "patchlev.h"

char *program ;
int pages ;
int verbose ;
FILE *infile ;
FILE *outfile ;
char pagelabel[BUFSIZ] ;
int pageno ;

static void usage(void)
{
   fprintf(stderr, "%s release %d patchlevel %d\n", program, RELEASE, PATCHLEVEL);
   fprintf(stderr, "Copyright (C) Angus J. C. Duggan, 1991-1995. See file LICENSE for details.\n");
   fprintf(stderr,
	   "Usage: %s [-q] [-e] [-o] [-r] [-p<pages>] [infile [outfile]]\n",
	   program);
   fflush(stderr);
   exit(1);
}

typedef struct pgrange {
   int first, last;
   struct pgrange *next;
} PageRange ;

static PageRange *makerange(int beg, int end, PageRange *next)
{
   PageRange *new;
   if ((new = (PageRange *)malloc(sizeof(PageRange))) == NULL)
      message(FATAL, "out of memory\n");
   new->first = beg;
   new->last = end;
   new->next = next;
   return (new);
}


static PageRange *addrange(char *str, PageRange *rp)
{
   int first=0;
   int sign;
   sign = (*str == '_' && ++str) ? -1 : 1;
   if (isdigit(*str)) {
      first = sign*atoi(str);
      while (isdigit(*str)) str++;
   }
   switch (*str) {
   case '\0':
      if (first || sign < 0)
	 return (makerange(first, first, rp));
      break;
   case ',':
      if (first || sign < 0)
	 return (addrange(str+1, makerange(first, first, rp)));
      break;
   case '-':
   case ':':
      str++;
      sign = (*str == '_' && ++str) ? -1 : 1;
      if (!first)
	 first = 1;
      if (isdigit(*str)) {
	 int last = sign*atoi(str);
	 while (isdigit(*str)) str++;
	 switch (*str) {
	 case '\0':
	   return (makerange(first, last, rp));
	 case ',':
	   return (addrange(str+1, makerange(first, last, rp)));
	 }
      } else if (*str == '\0')
	 return (makerange(first, -1, rp));
      else if (*str == ',')
	 return (addrange(str+1, makerange(first, -1, rp)));
   }
   message(FATAL, "invalid page range\n");
   return (PageRange *)0 ;
}


int main(int argc, char *argv[])
{
   int currentpg, maxpage = 0;
   int even = 0, odd = 0, reverse = 0;
   int pass, all;
   PageRange *pagerange = NULL;

   infile = stdin;
   outfile = stdout;
   verbose = 1;
   for (program = *argv++; --argc; argv++) {
      if (argv[0][0] == '-') {
	 switch (argv[0][1]) {
	 case 'e':	/* even pages */
	    even = 1;
	    break;
	 case 'o':	/* odd pages */
	    odd = 1;
	    break;
	 case 'r':	/* reverse */
	    reverse = 1;
	    break;
	 case 'p':	/* page spec */
	    pagerange = addrange(*argv+2, pagerange);
	    break;
	 case 'q':	/* quiet */
	    verbose = 0;
	    break;
	 case 'v':	/* version */
	 default:
	    usage();
	 }
      } else if (pagerange == NULL && !reverse && !even && !odd) {
	 pagerange = addrange(*argv, NULL);
      } else if (infile == stdin) {
	 if ((infile = fopen(*argv, OPEN_READ)) == NULL)
	    message(FATAL, "can't open input file %s\n", *argv);
      } else if (outfile == stdout) {
	 if ((outfile = fopen(*argv, OPEN_WRITE)) == NULL)
	    message(FATAL, "can't open output file %s\n", *argv);
      } else usage();
   }
#if defined(MSDOS) || defined(WINNT) || defined(WIN32)
   if ( infile == stdin ) {
      int fd = fileno(stdin) ;
      if ( setmode(fd, O_BINARY) < 0 )
         message(FATAL, "can't reset stdin to binary mode\n");
    }
   if ( outfile == stdout ) {
      int fd = fileno(stdout) ;
      if ( setmode(fd, O_BINARY) < 0 )
         message(FATAL, "can't reset stdout to binary mode\n");
    }
#endif
   if ((infile=seekable(infile))==NULL)
      message(FATAL, "can't seek input\n");

   scanpages();

   /* select all pages or all in range if odd or even not set */
   all = !(odd || even);

   /* add default page range */
   if (!pagerange)
      pagerange = makerange(1, -1, NULL);

   /* reverse page list if not reversing pages (list constructed bottom up) */
   if (!reverse) {
      PageRange *revlist = NULL;
      PageRange *next = NULL;
      while (pagerange) {
	 next = pagerange->next;
	 pagerange->next = revlist;
	 revlist = pagerange;
	 pagerange = next;
      }
      pagerange = revlist;
   } else { /* swap start & end if reversing */
      PageRange *r;
      for (r = pagerange; r; r = r->next) {
         int temp = r->last;
         r->last = r->first;
         r->first = temp;
      }
   }

   { /* adjust for end-relative pageranges */
      PageRange *r;
      for (r = pagerange; r; r = r->next) {
	 if (r->first < 0) {
	    r->first += pages + 1;
	    if (r->first < 1)
	       r->first = 1;
	 }
	 if (r->last < 0) {
	    r->last += pages + 1;
	    if (r->last < 1)
	       r->last = 1;
	 }
      }
   }

   /* count pages on first pass, select pages on second pass */
   for (pass = 0; pass < 2; pass++) {
      PageRange *r;
      if (pass) {                           /* write header on second pass */
	 writeheader(maxpage);
	 writeprolog();
	 writesetup();
      }
      for (r = pagerange; r; r = r->next) {
	 if (r->last < r->first) {
	    for (currentpg = r->first; currentpg >= r->last; currentpg--) {
	       if (currentpg == 0 ||
		   (currentpg <= pages &&
		    ((currentpg&1) ? (odd || all) : (even || all)))) {
		  if (pass) {
		     if (currentpg)
		        writepage(currentpg-1);
		     else
		        writeemptypage() ;
		  } else
		     maxpage++;
	       }
	    }
	 } else {
	    for (currentpg = r->first; currentpg <= r->last; currentpg++) {
	       if (currentpg == 0 ||
		   (currentpg <= pages &&
		    ((currentpg&1) ? (odd || all) : (even || all)))) {
		  if (pass) {
		     if (currentpg)
		        writepage(currentpg-1);
		     else
		        writeemptypage() ;
		  } else
		     maxpage++;
	       }
	    }
	 }
      }
   }
   writetrailer();

   return 0;
}
